//////////////////////////////////////////////////////////////////////////////////
//				KeyHook.cpp
//
// Implementation of hook functions
//
// Written by Abin (abinn32@yahoo.com)
// Apr 30, 2004
//
// History:
//
// v1.00 May 06, 2004 - Initial release.
// v1.01 May 14, 2004 - Fixed an error on verifying combination keys and keyboard indicators.
// v1.02 May 26, 2004 - Fixed some minor problems on events translation.
//                      Removed unnecessary combo-key flags and indicator flags.
//////////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "KeyHook.h"

////////////////////////////////////////////////////////////////////////////////
//		The Synchronization Class
////////////////////////////////////////////////////////////////////////////////
#define KH_UUID	"{3FBB26D9-D352-409A-9352-7BDD78A1EBA1}" // The unique string
class CHookMutex
{
public:

	// construct the mutex with an unique string
	CHookMutex(LPCSTR lpszUniqueString)
	{
		m_bLocked = FALSE;
		m_hMutex = ::CreateMutexA(NULL, FALSE, lpszUniqueString);
	}

	~CHookMutex()
	{
		Unlock();
		::CloseHandle(m_hMutex);
	}

	BOOL Lock(DWORD dwTimeout = 250)
	{
		if (m_bLocked)
			return TRUE; // Prevents multiple locking

		m_bLocked = ::WaitForSingleObject(m_hMutex, dwTimeout) == WAIT_OBJECT_0;
		return m_bLocked;
	}

	void Unlock()
	{
		::ReleaseMutex(m_hMutex);
		m_bLocked = FALSE;
	}

private:
	HANDLE m_hMutex; // The mutex handle
	BOOL m_bLocked; // Wether the mutex is currently locked
};

#define MAX_PROCESS_COUNT	256	 // Maximum number of unique processes can use the hook simultaneously
#define MAX_ENTRY_COUNT		1024 // Maximum number of global entries can be registered

////////////////////////////////////////////////////////////////////////////////
//		Data to be Shared Among Processes
////////////////////////////////////////////////////////////////////////////////
#pragma data_seg("SHARED")
static HHOOK g_hHook = NULL; // the hook handle
static INT g_nProcessCount = 0; // process count
static DWORD g_aProcesses[MAX_PROCESS_COUNT] = { 0 }; // An array of registered processes
static INT g_nEntryCount = 0; // entry count(global)
static KEYENTRY g_aEntries[MAX_ENTRY_COUNT] = { 0 }; // An array of key entries
static BYTE g_iCombKeys = 0;  // Combo key flags
static BYTE g_iIndicators = 0; // Keyboard indicator state flags
#pragma data_seg()
#pragma comment(linker, "/section:SHARED,RWS")

////////////////////////////////////////////////////////////////////////////////
//		Non-Shared Data
////////////////////////////////////////////////////////////////////////////////
HINSTANCE g_hModInstance = NULL; // instance handle
DWORD g_dwOwnerID = 0; // ID of the process who owns this hook

// we will have to pack vk code, key event types, comb keys states, keyboard indicator
// states into those "irrelevant" bit fields of lParam (0-15)

// 0-7:   The virtual key code (lowe byte)
// 8-9:   01 for key down, 10 for key up, 11 for key repeat
// 10:    1 if alt key is down
// 11:    1 if ctrl key is down
// 12:    1 if shift key is down
// 13:    1 if caps-lock is on
// 14:    1 if num-lock is on
// 15:    1 if scroll-lock is on

#define MASK_KEY_DOWN		0x01
#define MASK_KEY_UP			0x02
#define MASK_KEY_REPEAT		0x03
#define MASK_ALT			0x04
#define MASK_CTRL			0x08
#define MASK_SHIFT			0x10
#define MASK_CAPSLOCK		0x20
#define MASK_NUMLOCK		0x40
#define MASK_SCRLOCK		0x80

// Check a pointer and assign value to the address it points to
#define ASSIGNRESULT(x, y) ((x) ? (*(x) = (y)) : NULL)

////////////////////////////////////////////////////////////////////////////////
//		Non-Exported Function Prototypes
////////////////////////////////////////////////////////////////////////////////
void UpdateCombKeyInfo(WPARAM wParam, BYTE iKeyEvent);
void UpdateIndicatorInfo(WPARAM wParam, BYTE iKeyEvent);
LRESULT CALLBACK KeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam); // the callback function to be passed into ::SetWindowsHookEx
INT FindOwner(DWORD dwProcessID); // find a process ID in the array
BOOL EventMatch(LPCKEYENTRY lpEntry, HWND hForeground, BYTE iVKCode, BYTE iKeyEvent, BYTE iCombKeys, BYTE iIndicators); // check if an event matches the information in an entry
INT FindEntry(LPCKEYENTRY lpEntry); // find an entry in the global entry array
BOOL VerifyCombKeys(BYTE iCombKeys, BYTE iActualComb, BOOL bIgnoreAlt, BOOL bIgnoreCtrl, BOOL bIgnoreSHift); // verify combination key states
BOOL VerifyIndicators(BYTE iIndicators, BYTE iActualIndicators); // verify keyboard indicator states
BYTE GetKeyEventType(LPARAM lParam); // determine key event type
LPARAM ComposeLParam(LPARAM lParam, BYTE iVKCode, BYTE iKeyEvent, BYTE iCombKeys, BYTE iIndicators); // pack information into 0-15 bits of lParam
void ParseParams(UINT nMask, WPARAM wParam, LPARAM lParam, HWND& hOccurredWnd, BYTE& iKeyEvent, BYTE& iVKCode, BYTE& iCombKeys, BYTE& iIndicators, BYTE* aKeyboard);
void ClearAll(); // Remove all key entries

// DLL entry point
BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	lpReserved = lpReserved; // Appeases VC6 warning level 4
	g_hModInstance = hinstDLL; // this will be passed into ::SetWindowsHookEx

	if (ul_reason_for_call == DLL_PROCESS_DETACH)		
		UninstallKeyHook(); // uninstall the hook, in case the application forgot to do so

    return TRUE;
}

LRESULT __declspec(dllexport) InstallKeyHook()
{
	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
		return KH_ERR_LOCK_FAIL;

	if (g_dwOwnerID != 0)
		return KH_ERR_ALREADY_INSTALLED;

	if (g_nProcessCount >= MAX_PROCESS_COUNT)
		return KH_ERR_MAX_PROCESS_REACHED; // process limitation reached

	DWORD dwProcessID = ::GetCurrentProcessId(); // Obtain process id	

	// prevent same process from installing the hook more than once, this should
	// never be able to happen, but just in case...
	if (FindOwner(dwProcessID) != -1)
		return KH_ERR_PROCESS_IN_USE;
		
	// if this is the first reference, then create the hook, otherwise just increment
	// the reference count
	if (g_hHook == NULL)
	{
		// this is the first reference, create the hook
		g_hHook = ::SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_hModInstance, 0);
	
		if (g_hHook == NULL)
			return KH_ERR_INSTALL_FAIL;
	}	

	g_aProcesses[g_nProcessCount] = dwProcessID; // record the process so subsequent calls of "InstallKeyHook" using this process ID will fail
	g_dwOwnerID = dwProcessID;
	g_nProcessCount++;	
	ClearAll(); // Removes all entries that were registered by the same process id
	return KH_OK;
}
 
LRESULT __declspec(dllexport) UninstallKeyHook()
{
	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
		return KH_ERR_LOCK_FAIL;

	if (g_dwOwnerID == 0 || g_hHook == NULL || g_nProcessCount <= 0)
		return KH_ERR_NOT_INSTALLED;

	ClearAll();

	// if this is the only reference, then destroy the hook, otherwise just decrement
	// the reference count
	if (g_nProcessCount == 1)
	{	
		// remove the hook
		if (!::UnhookWindowsHookEx(g_hHook))
			return KH_ERR_UNINSTALL_FAIL;

		g_hHook = NULL;
	}	

	// find the owner process ID and remove it from the reference array
	const INT IDX = FindOwner(g_dwOwnerID);
	if (IDX != -1)
		::memmove(&g_aProcesses[IDX], &g_aProcesses[IDX + 1], sizeof(DWORD) * (g_nProcessCount - IDX - 1)); // Left-shift the memory
		
	g_nProcessCount--;
	g_dwOwnerID = 0;
	return KH_OK;
}

BOOL __declspec(dllexport) IsKeyHookInstalled()
{
	return g_dwOwnerID != 0;
}

LRESULT __declspec(dllexport) AddKeyEntry(LPCKEYENTRY lpEntry)
{
	// Verify the parameter
	if (lpEntry == NULL)
		return KH_ERR_INVALID_PARAM;

	if (lpEntry->hCallWnd == NULL)
		return KH_ERR_INVALID_CALLWND;

	if (lpEntry->nMessage == 0)
		return KH_ERR_INVALID_USERMESSAGE;

	if (lpEntry->iMinVKCode > lpEntry->iMaxVKCode)
		return KH_ERR_INVALID_VKCODE_SEQUENCE;

	if (((lpEntry->iCombKeys & KH_ALT_PRESSED) && (lpEntry->iCombKeys & KH_ALT_NOT_PRESSED))
		|| ((lpEntry->iCombKeys & KH_CTRL_PRESSED) && (lpEntry->iCombKeys & KH_CTRL_NOT_PRESSED))
		|| ((lpEntry->iCombKeys & KH_SHIFT_PRESSED) && (lpEntry->iCombKeys & KH_SHIFT_NOT_PRESSED)))
		return KH_ERR_INVALID_COMBKEY_FLAGS;

	if (((lpEntry->iIndicators & KH_CAPSLOCK_ON) && (lpEntry->iIndicators & KH_CAPSLOCK_OFF))
		|| ((lpEntry->iIndicators & KH_NUMLOCK_ON) && (lpEntry->iIndicators & KH_NUMLOCK_OFF))
		|| ((lpEntry->iIndicators & KH_SCRLOCK_ON) && (lpEntry->iIndicators & KH_SCRLOCK_OFF)))
		return KH_ERR_INVALID_INDICATOR_FLAGS;

	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
		return KH_ERR_LOCK_FAIL;	

	if (g_dwOwnerID == 0)
		return KH_ERR_NOT_INSTALLED;

	if (g_nEntryCount >= MAX_ENTRY_COUNT)
		return KH_ERR_MAX_ENTRY_REACHED; // entry limitation reached

	// no duplicated entries
	if (FindEntry(lpEntry) != -1)
		return KH_ERR_ALREADY_REGISTERED;
	
	// append the new entry to the rear-end of the entry array, copy the entry but
	// not including its "dwReserved" variable since this variable specified by the
	// caller has no meaning for here
	::memcpy(&g_aEntries[g_nEntryCount], lpEntry, sizeof(KEYENTRY) - sizeof(DWORD));
	g_aEntries[g_nEntryCount].dwReserved = g_dwOwnerID;
	g_nEntryCount++;
	return KH_OK;
}

LRESULT __declspec(dllexport) RemoveKeyEntry(LPCKEYENTRY lpEntry)
{
	if (lpEntry == NULL)
		return KH_ERR_INVALID_PARAM;

	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
		return KH_ERR_LOCK_FAIL;

	if (g_dwOwnerID == 0 )
		return KH_ERR_NOT_INSTALLED;

	const INT IDX = FindEntry(lpEntry);
	if (IDX == -1)
		return KH_ERR_NOT_REGISTERED; // entry not exists

	::memmove(&g_aEntries[IDX], &g_aEntries[IDX + 1], sizeof(KEYENTRY) * (g_nEntryCount - IDX - 1));
	g_nEntryCount--;
	
	return KH_OK;
}

LRESULT __declspec(dllexport) RemoveAllKeyEntries()
{
	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
		return KH_ERR_LOCK_FAIL;

	if (g_dwOwnerID == 0 )
		return KH_ERR_NOT_INSTALLED;

	ClearAll();
	return KH_OK;
}

BOOL __declspec(dllexport) KeyEntryExists(LPCKEYENTRY lpEntry, LRESULT* pError)
{
	if (lpEntry == NULL)
	{
		ASSIGNRESULT(pError, KH_ERR_INVALID_PARAM);
		return FALSE;
	}

	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
	{
		ASSIGNRESULT(pError, KH_ERR_LOCK_FAIL);
		return FALSE;
	}

	if (g_dwOwnerID == 0)
	{
		ASSIGNRESULT(pError, KH_ERR_NOT_INSTALLED);
		return FALSE;
	}

	ASSIGNRESULT(pError, KH_OK);
	return FindEntry(lpEntry) != -1;
}

INT __declspec(dllexport) GetKeyEntryCount(LRESULT* pError)
{		
	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
	{
		ASSIGNRESULT(pError, KH_ERR_LOCK_FAIL);
		return -1;
	}

	if (g_dwOwnerID == 0)
	{
		ASSIGNRESULT(pError, KH_ERR_NOT_INSTALLED);
		return -1;
	}

	INT nCount = 0;

	// find all entries that were registered by current process
	for (INT i = 0; i < g_nEntryCount; i++)
	{	
		if (g_aEntries[i].dwReserved == g_dwOwnerID)
			nCount++;
	}
	
	ASSIGNRESULT(pError, KH_OK);
	return nCount;
}

INT __declspec(dllexport) GetKeyEntryList(LPKEYENTRY lpBuffer, INT nMaxCount, LRESULT* pError/*= NULL*/)
{
	if (lpBuffer == NULL || nMaxCount == 0)
	{
		ASSIGNRESULT(pError, KH_ERR_INVALID_PARAM);
		return -1;
	}	

	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
	{
		ASSIGNRESULT(pError, KH_ERR_LOCK_FAIL);
		return -1;
	}

	if (g_dwOwnerID == 0)
	{
		ASSIGNRESULT(pError, KH_ERR_NOT_INSTALLED);
		return -1;
	}

	// copy all(up to nMaxCount) entries that were registered by current process
	INT nCopied = 0;
	for (INT i = 0; i < g_nEntryCount && nCopied < nMaxCount; i++)
	{
		if (g_aEntries[i].dwReserved == g_dwOwnerID)
		{
			::memcpy(&lpBuffer[nCopied], &g_aEntries[i], sizeof(KEYENTRY) - sizeof(DWORD));
			nCopied++;
		}
	}
	
	ASSIGNRESULT(pError, KH_OK);
	return nCopied;
}

LRESULT CALLBACK KeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam)
{	
	if (nCode != HC_ACTION)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

	const BYTE VKCODE = (BYTE)wParam;
	const BYTE KEYEVENT = GetKeyEventType(lParam);
	const HWND FOREGROUND = ::GetForegroundWindow();	

	CHookMutex mutex(KH_UUID);
	if (!mutex.Lock())
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam); // lock failed, ignore this event

	UpdateCombKeyInfo(wParam, KEYEVENT);
	UpdateIndicatorInfo(wParam, KEYEVENT);
	
	for (INT i = 0; i < g_nEntryCount; i++)
	{
		// Send message to all matched entries
		if (EventMatch(&g_aEntries[i], FOREGROUND, VKCODE, KEYEVENT, g_iCombKeys, g_iIndicators))
		{
			// Use PostMessage instead of SendMessage so that one frozen application
			// will not ruin others. This is actually one of the reasons why I use
			// windows message system instead of callback functions to notify the
			// client applications about the captured key events.
			::PostMessage(g_aEntries[i].hCallWnd,
						g_aEntries[i].nMessage,
						WPARAM(FOREGROUND),
						ComposeLParam(lParam, VKCODE, KEYEVENT, g_iCombKeys, g_iIndicators));
		}
	}	

	mutex.Unlock(); // CallNextHookEx may not return immediately, so we need to unlock first
	
	return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);	
}

// Overloaded for non-unicode
LRESULT __declspec(dllexport) GetKeyEventResult(WPARAM wParam, LPARAM lParam, tagKeyResultA* lpKeyResult, UINT nMask)
{
	if (lpKeyResult == NULL)
		return KH_ERR_INVALID_PARAM;

	BYTE* pKeyboard = NULL; // only needed when the application requires to fill iPrintableChar
	if (nMask & KH_MASK_PRINTABLECHAR)
		pKeyboard = new BYTE[256];
	
	// Parse basic information
	BYTE iVKCode = 0;
	ParseParams(nMask, wParam, lParam, lpKeyResult->hOccurredInWnd, lpKeyResult->iKeyEvent, iVKCode, lpKeyResult->iCombKeys, lpKeyResult->iIndicators, pKeyboard);

	if (nMask & KH_MASK_PRINTABLECHAR)
	{
		// Get the printable char
		WORD wCh = 0;
		if (::ToAscii(iVKCode, MAKELPARAM(0, HIWORD(lParam)), pKeyboard, &wCh, 0) == 1)
			lpKeyResult->chPrintableChar = (CHAR)wCh;
		else
			lpKeyResult->chPrintableChar = '\0';

		if (lpKeyResult->chPrintableChar < ' ')
			lpKeyResult->chPrintableChar = '\0'; // Only printable
	}

	if (pKeyboard != NULL)
		delete [] pKeyboard;

	if (nMask & KH_MASK_VKCODE)
		lpKeyResult->iVKCode = iVKCode;

	if (nMask & KH_MASK_KEYNAME)
	{
		// Get the key name text
		lpKeyResult->szKeyName[0] = '\0';
		::GetKeyNameTextA(MAKELPARAM(0, HIWORD(lParam)), lpKeyResult->szKeyName, 31);
	}	

	return KH_OK;
}

// Overloaded for unicode
LRESULT __declspec(dllexport) GetKeyEventResult(WPARAM wParam, LPARAM lParam, tagKeyResultW* lpKeyResult, UINT nMask)
{
	if (lpKeyResult == NULL)
		return KH_ERR_INVALID_PARAM;

	BYTE* pKeyboard = NULL;
	if (nMask & KH_MASK_PRINTABLECHAR)
		pKeyboard = new BYTE[256];

	BYTE iVKCode = 0;
	ParseParams(nMask, wParam, lParam, lpKeyResult->hOccurredInWnd, lpKeyResult->iKeyEvent, iVKCode, lpKeyResult->iCombKeys, lpKeyResult->iIndicators, pKeyboard);

	if (nMask & KH_MASK_PRINTABLECHAR)
	{
		WCHAR szw[10] = L"";
		if (::ToUnicode(iVKCode, MAKELPARAM(0, HIWORD(lParam)), pKeyboard, szw, 9, 0) == 1)
			lpKeyResult->chPrintableChar = szw[0];
		else
			lpKeyResult->chPrintableChar = L'\0';
	
		if (lpKeyResult->chPrintableChar < L' ')
			lpKeyResult->chPrintableChar = L'\0'; // Only printable
	}

	if (pKeyboard != NULL)
		delete [] pKeyboard;

	if (nMask & KH_MASK_VKCODE)
		lpKeyResult->iVKCode = iVKCode;

	if (nMask & KH_MASK_KEYNAME)
	{
		lpKeyResult->szKeyName[0] = L'\0';
		::GetKeyNameTextW(MAKELPARAM(0, HIWORD(lParam)), lpKeyResult->szKeyName, 31);
	}	

	return KH_OK;
}

// Overloaded for non-unicode
INT __declspec(dllexport) FormatKeyHookError(LRESULT lErrorCode, LPSTR lpszBuffer, INT nMaxLength)
{
	// format an error code into error message
	if (lpszBuffer == NULL || nMaxLength == 0)
		return FALSE;

	switch (lErrorCode)
	{
	case KH_OK:
		::strncpy(lpszBuffer, "Operation completed successfully.", nMaxLength);
		break;

	case KH_ERR_LOCK_FAIL:
		::strncpy(lpszBuffer, "Failed to obtain ownership of the mutex.", nMaxLength);
		break;
		
	case KH_ERR_INVALID_PARAM:
		::strncpy(lpszBuffer, "Invalid parameter(s).", nMaxLength);
		break;
		
	case KH_ERR_ALREADY_INSTALLED:
		::strncpy(lpszBuffer, "Hook is already installed by current process.", nMaxLength);
		break;

	case KH_ERR_NOT_INSTALLED:
		::strncpy(lpszBuffer, "Hook is not installed by current process.", nMaxLength);
		break;
		
	case KH_ERR_INSTALL_FAIL:
		::strncpy(lpszBuffer, "::SetWindowsHookEx API failed.", nMaxLength);
		break;

	case KH_ERR_UNINSTALL_FAIL:
		::strncpy(lpszBuffer, "::UnhookWindowsHookEx API failed.", nMaxLength);
		break;

	case KH_ERR_MAX_ENTRY_REACHED:
		::strncpy(lpszBuffer, "Maxumum entry number is reached.", nMaxLength);
		break;

	case KH_ERR_ALREADY_REGISTERED:
		::strncpy(lpszBuffer, "Entry already registered.", nMaxLength);
		break;

	case KH_ERR_NOT_REGISTERED:
		::strncpy(lpszBuffer, "Entry is not registered.", nMaxLength);
		break;
		
	case KH_ERR_MAX_PROCESS_REACHED:
		::strncpy(lpszBuffer, "Maxumum process number is reached.", nMaxLength);
		break;

	case KH_ERR_PROCESS_IN_USE:
		::strncpy(lpszBuffer, "The specified process has already installed the hook.", nMaxLength);
		break;

	case KH_ERR_INVALID_CALLWND:
		::strncpy(lpszBuffer, "Invalid call window handle.", nMaxLength);
		break;

	case KH_ERR_INVALID_USERMESSAGE:
		::strncpy(lpszBuffer, "Invalid user message ID.", nMaxLength);
		break;

	case KH_ERR_INVALID_VKCODE_SEQUENCE:
		::strncpy(lpszBuffer, "Invalid virtual key code sequence.", nMaxLength);
		break;

	case KH_ERR_INVALID_COMBKEY_FLAGS:
		::strncpy(lpszBuffer, "Invalid combination key flags.", nMaxLength);
		break;

	case KH_ERR_INVALID_INDICATOR_FLAGS:
		::strncpy(lpszBuffer, "Invalid keyboard indicator flags.", nMaxLength);
		break;

	default:
		::strncpy(lpszBuffer, "", nMaxLength);
	}

	return ::strlen(lpszBuffer);
}

// Overloaded for unicode
BOOL __declspec(dllexport) FormatKeyHookError(LRESULT lErrorCode, LPWSTR lpszBuffer, INT nMaxLength)
{
	// format an error code into error message
	if (lpszBuffer == NULL || nMaxLength == 0)
		return FALSE;

	switch (lErrorCode)
	{
	case KH_OK:
		::wcsncpy(lpszBuffer, L"Operation completed successfully.", nMaxLength);
		break;

	case KH_ERR_LOCK_FAIL:
		::wcsncpy(lpszBuffer, L"Failed to obtain ownership of the mutex.", nMaxLength);
		break;
		
	case KH_ERR_INVALID_PARAM:
		::wcsncpy(lpszBuffer, L"Invalid parameter(s).", nMaxLength);
		break;
		
	case KH_ERR_ALREADY_INSTALLED:
		::wcsncpy(lpszBuffer, L"Hook is already installed by current process.", nMaxLength);
		break;

	case KH_ERR_NOT_INSTALLED:
		::wcsncpy(lpszBuffer, L"Hook is not installed by current process.", nMaxLength);
		break;
		
	case KH_ERR_INSTALL_FAIL:
		::wcsncpy(lpszBuffer, L"::SetWindowsHookEx API failed.", nMaxLength);
		break;

	case KH_ERR_UNINSTALL_FAIL:
		::wcsncpy(lpszBuffer, L"::UnhookWindowsHookEx API failed.", nMaxLength);
		break;

	case KH_ERR_MAX_ENTRY_REACHED:
		::wcsncpy(lpszBuffer, L"Maxumum entry number is reached.", nMaxLength);
		break;

	case KH_ERR_ALREADY_REGISTERED:
		::wcsncpy(lpszBuffer, L"Entry already registered.", nMaxLength);
		break;

	case KH_ERR_NOT_REGISTERED:
		::wcsncpy(lpszBuffer, L"Entry is not registered.", nMaxLength);
		break;
		
	case KH_ERR_MAX_PROCESS_REACHED:
		::wcsncpy(lpszBuffer, L"Maxumum process number is reached.", nMaxLength);
		break;

	case KH_ERR_PROCESS_IN_USE:
		::wcsncpy(lpszBuffer, L"The specified process has already installed the hook.", nMaxLength);
		break;

	case KH_ERR_INVALID_CALLWND:
		::wcsncpy(lpszBuffer, L"Invalid call window handle.", nMaxLength);
		break;

	case KH_ERR_INVALID_USERMESSAGE:
		::wcsncpy(lpszBuffer, L"Invalid user message ID.", nMaxLength);
		break;

	case KH_ERR_INVALID_VKCODE_SEQUENCE:
		::wcsncpy(lpszBuffer, L"Invalid virtual key code sequence.", nMaxLength);
		break;

	case KH_ERR_INVALID_COMBKEY_FLAGS:
		::wcsncpy(lpszBuffer, L"Invalid combination key flags.", nMaxLength);
		break;

	case KH_ERR_INVALID_INDICATOR_FLAGS:
		::wcsncpy(lpszBuffer, L"Invalid keyboard indicator flags.", nMaxLength);
		break;

	default:
		::wcsncpy(lpszBuffer, L"", nMaxLength);
	}

	return ::wcslen(lpszBuffer);
}

INT FindOwner(DWORD dwProcessID)
{
	// assume the caller has already locked the mutex!!!
	if (dwProcessID == 0)
		return -1;
	
	for (INT i = 0; i < g_nProcessCount; i++)
	{
		if (g_aProcesses[i] == dwProcessID)
			return i;
	}
	return -1;
}

BOOL EventMatch(LPCKEYENTRY lpEntry, HWND hForeground, BYTE iVKCode, BYTE iKeyEvent, BYTE iCombKeys, BYTE iIndicators)
{
	// assume the caller has already locked the mutex!!!

	// exam each members in the order of "less-time-consuming to more time consuming"
	// so that the function returns as earlier as possible if the event does not match
	if (lpEntry == NULL)
		return FALSE; // invalid parameter

	if (lpEntry->nMessage == 0)
		return FALSE; // invalid user defined message ID

	if (lpEntry->hHookWnd != 0 && lpEntry->hHookWnd != hForeground)
		return FALSE; // hook window mismatch

	if (iVKCode < lpEntry->iMinVKCode || iVKCode > lpEntry->iMaxVKCode)
		return FALSE; // VK code mismatch

	if (lpEntry->iKeyEvent != 0 && !(lpEntry->iKeyEvent & iKeyEvent))
		return FALSE; // key down/up/repeat event type mismatch	

	if (!VerifyCombKeys(lpEntry->iCombKeys, iCombKeys, iVKCode == VK_MENU, iVKCode == VK_CONTROL, iVKCode == VK_SHIFT))
		return FALSE; // combination key states mismatch
	
	if (!VerifyIndicators(lpEntry->iIndicators, iIndicators))
		return FALSE; // keyboard indicator states mismatch

	if (lpEntry->hCallWnd == NULL || !::IsWindow(lpEntry->hCallWnd))
		return FALSE; // invalid call window

	return TRUE; // Was a tough exam, can be no more picky	
}

INT FindEntry(LPCKEYENTRY lpEntry)
{
	if (lpEntry == NULL || g_dwOwnerID == 0)
		return -1;

	for (INT i = 0; i < g_nEntryCount; i++)
	{
		if (g_aEntries[i].dwReserved == g_dwOwnerID && ::memcmp(&g_aEntries[i], lpEntry, sizeof(KEYENTRY) - sizeof(DWORD)) == 0)
			return i;
	}

	return -1;
}

BYTE GetKeyEventType(LPARAM lParam)
{
	// Reference: WM_KEYDOWN on MSDN
	if (lParam & 0x80000000) // check bit 31 for up/down
	{
		return KH_KEY_UP;
	}
	else
	{
		if (lParam & 0x40000000) // check bit 30 for previous up/down
			return KH_KEY_REPEAT; // It was pressed down before this key-down event, so it's a key-repeat for sure
		else
			return KH_KEY_DOWN;
	}
}

BOOL VerifyCombKeys(BYTE iCombKeys, BYTE iActualComb, BOOL bIgnoreAlt, BOOL bIgnoreCtrl, BOOL bIgnoreSHift)
{
	// check alt
	if (!bIgnoreAlt)
	{
		if (iCombKeys & KH_ALT_PRESSED)
		{
			if ((iActualComb & KH_ALT_PRESSED) == 0)
				return FALSE;
		}
	
		if (iCombKeys & KH_ALT_NOT_PRESSED)
		{
			if (iActualComb & KH_ALT_PRESSED)
				return FALSE;
		}
	}

	// check ctrl
	if (!bIgnoreCtrl)
	{
		if (iCombKeys & KH_CTRL_PRESSED)
		{
			if ((iActualComb & KH_CTRL_PRESSED) == 0)
				return FALSE;
		}
	
		if (iCombKeys & KH_CTRL_NOT_PRESSED)
		{
			if (iActualComb & KH_CTRL_PRESSED)
				return FALSE;
		}
	}

	if (!bIgnoreSHift)
	{
		// check shift
		if (iCombKeys & KH_SHIFT_PRESSED)
		{
			if ((iActualComb & KH_SHIFT_PRESSED) == 0)
				return FALSE;
		}
	
		if (iCombKeys & KH_SHIFT_NOT_PRESSED)
		{
			if (iActualComb & KH_SHIFT_PRESSED)
				return FALSE;
		}
	}

	return TRUE;
}

BOOL VerifyIndicators(BYTE iIndicators, BYTE iActualIndicators)
{
	// check caps lock
	if (iIndicators & KH_CAPSLOCK_ON)
	{
		if ((iActualIndicators & KH_CAPSLOCK_ON) == 0)
			return FALSE;
	}

	if (iIndicators & KH_CAPSLOCK_OFF)
	{
		if (iActualIndicators & KH_CAPSLOCK_ON)
			return FALSE;
	}		

	// check num lock
	if (iIndicators & KH_NUMLOCK_ON)
	{
		if ((iActualIndicators & KH_NUMLOCK_ON) == 0)
			return FALSE;
	}
	
	if (iIndicators & KH_NUMLOCK_OFF)
	{
		if (iActualIndicators & KH_NUMLOCK_ON)
			return FALSE;
	}		

	// check scroll lock
	if (iIndicators & KH_SCRLOCK_ON)
	{
		if ((iActualIndicators & KH_SCRLOCK_ON) == 0)
			return FALSE;
	}
	
	if (iIndicators & KH_SCRLOCK_OFF)
	{
		if (iActualIndicators & KH_SCRLOCK_ON)
			return FALSE;
	}

	return TRUE;
}

LPARAM ComposeLParam(LPARAM lParam, BYTE iVKCode, BYTE iKeyEvent, BYTE iCombKeys, BYTE iIndicators)
{
	// The biggest problem on using windows message system is that, it can only send
	// at most 64 bits of data on Win32 platform(wParam 32 bits, lParam 32 bits), that
	// is not sufficient in this case, thus we need to compact all necessary data into
	// those 64 bits. Hell, we must not damage the original lParam we received from 
	// "KeyboardProc" because it contains essential data(such as scan code) which will
	// be needed for future process.
	BYTE iMask = 0;
	if (iKeyEvent & KH_KEY_DOWN)
		iMask |= MASK_KEY_DOWN;
	else if (iKeyEvent & KH_KEY_UP)
		iMask |= MASK_KEY_UP;
	else if (iKeyEvent & KH_KEY_REPEAT)
		iMask |= MASK_KEY_REPEAT;

	if (iVKCode != VK_MENU && (iCombKeys & KH_ALT_PRESSED))
		iMask |= MASK_ALT;

	if (iVKCode != VK_CONTROL && (iCombKeys & KH_CTRL_PRESSED))
		iMask |= MASK_CTRL;

	if (iVKCode != VK_SHIFT && (iCombKeys & KH_SHIFT_PRESSED))
		iMask |= MASK_SHIFT;

	if (iIndicators & KH_CAPSLOCK_ON)
		iMask |= MASK_CAPSLOCK;

	if (iIndicators & KH_NUMLOCK_ON)
		iMask |= MASK_NUMLOCK;

	if (iIndicators & KH_SCRLOCK_ON)
		iMask |= MASK_SCRLOCK;

	return MAKELPARAM(MAKEWORD(iVKCode, iMask), HIWORD(lParam));
}

void ParseParams(UINT nMask, WPARAM wParam, LPARAM lParam, HWND& hOccurredWnd, BYTE& iKeyEvent, BYTE& iVKCode, BYTE& iCombKeys, BYTE& iIndicators, BYTE* aKeyboard)
{
	// kinda a reversal of "ComposeLParam"
	const BYTE MASK = HIBYTE(LOWORD(lParam));

	if (nMask & KH_MASK_OCCURREDWND)
		hOccurredWnd = (HWND)wParam;

	iVKCode = LOBYTE(LOWORD(lParam));

	if (aKeyboard != NULL)
		::memset(aKeyboard, NULL, 256);	

	if (nMask & KH_MASK_EVENTTYPE)
	{
		iKeyEvent = 0;

		// there can only be one key event
		if ((MASK & MASK_KEY_REPEAT) == MASK_KEY_REPEAT)
		{
			iKeyEvent |= KH_KEY_REPEAT;	

			if (aKeyboard != NULL)
				aKeyboard[iVKCode] = 0x80;
		}
		else if (MASK & MASK_KEY_UP)
		{
			iKeyEvent |= KH_KEY_UP;
		}
		else
		{
			iKeyEvent |= KH_KEY_DOWN;

			if (aKeyboard != NULL)
				aKeyboard[iVKCode] = 0x80;
		}
	}

	if (nMask & KH_MASK_COMBKEYS)
	{
		iCombKeys = 0;

		if (MASK & MASK_ALT)
		{
			iCombKeys |= KH_ALT_PRESSED;

			if (aKeyboard != NULL)
				aKeyboard[VK_MENU] = 0x80;
		}
		if (MASK & MASK_CTRL)
		{
			iCombKeys |= KH_CTRL_PRESSED;

			if (aKeyboard != NULL)
				aKeyboard[VK_CONTROL] = 0x80;
		}
		if (MASK & MASK_SHIFT)
		{
			iCombKeys |= KH_SHIFT_PRESSED;

			if (aKeyboard != NULL)
				aKeyboard[VK_SHIFT] = 0x80;
		}
	}

	if (nMask & KH_MASK_INDICATORS)
	{
		iIndicators = 0;

		if (MASK & MASK_CAPSLOCK)
		{
			iIndicators |= KH_CAPSLOCK_ON;

			if (aKeyboard != NULL)
				aKeyboard[VK_CAPITAL] = 0x01;
		}
		if (MASK & MASK_NUMLOCK)
		{
			iIndicators |= KH_NUMLOCK_ON;

			if (aKeyboard != NULL)
				aKeyboard[VK_NUMLOCK] = 0x01;
		}
		if (MASK & MASK_SCRLOCK)
		{
			iIndicators |= KH_SCRLOCK_ON;

			if (aKeyboard != NULL)
				aKeyboard[VK_SCROLL] = 0x01;
		}
	}	
}

void ClearAll()
{
	// Assume the caller had already locked the mutex!
	if (g_dwOwnerID == 0)
		return;

	// Remove all entries that were registered by current process
	for (INT i = 0; i < g_nEntryCount; i++)
	{
		if (g_aEntries[i].dwReserved == g_dwOwnerID)
		{
			// Left shift elements
			::memmove(&g_aEntries[i], &g_aEntries[i + 1], sizeof(KEYENTRY) * (g_nEntryCount - i - 1));
			i--;
			g_nEntryCount--;
		}
	}
}

void UpdateCombKeyInfo(WPARAM wParam, BYTE iKeyEvent)
{
	// update combo keys on both key-down and key-up
	if (iKeyEvent == KH_KEY_DOWN || iKeyEvent == KH_KEY_UP)
	{		
		if ((BYTE)wParam == VK_MENU)
		{
			if (::GetKeyState(VK_MENU) & 0x8000)
				g_iCombKeys |= KH_ALT_PRESSED;
			else
				g_iCombKeys &= ~KH_ALT_PRESSED;
		}

		if ((BYTE)wParam == VK_CONTROL)
		{
			if (::GetKeyState(VK_CONTROL) & 0x8000)
				g_iCombKeys |= KH_CTRL_PRESSED;
			else
				g_iCombKeys &= ~KH_CTRL_PRESSED;
		}

		if ((BYTE)wParam == VK_SHIFT)
		{
			if (::GetKeyState(VK_SHIFT) & 0x8000)
				g_iCombKeys |= KH_SHIFT_PRESSED;
			else
				g_iCombKeys &= ~KH_SHIFT_PRESSED;
		}		
	}
}

void UpdateIndicatorInfo(WPARAM wParam, BYTE iKeyEvent)
{
	// update indicators on key-down only
	if (iKeyEvent == KH_KEY_DOWN)
	{		
		if ((BYTE)wParam == VK_CAPITAL)
		{
			if (::GetKeyState(VK_CAPITAL) & 0x0001)
				g_iIndicators |= KH_CAPSLOCK_ON;
			else
				g_iIndicators &= ~KH_CAPSLOCK_ON;
		}

		if ((BYTE)wParam == VK_NUMLOCK)
		{
			if (::GetKeyState(VK_NUMLOCK) & 0x0001)
				g_iIndicators |= KH_NUMLOCK_ON;
			else
				g_iIndicators &= ~KH_NUMLOCK_ON;
		}

		if ((BYTE)wParam == VK_SCROLL)
		{
			if (::GetKeyState(VK_SCROLL) & 0x0001)
				g_iIndicators |= KH_SCRLOCK_ON;
			else
				g_iIndicators &= ~KH_SCRLOCK_ON;
		}		
	}
}
