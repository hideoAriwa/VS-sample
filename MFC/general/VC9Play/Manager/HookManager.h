//////////////////////////////////////////////////////////////////////////////////
//				KeyHook.h
//
// This file contains exported data struct and function definitions used for an
// all-purpose keyboard hooker.
//
// To use the code, please follow these steps:
//
// 1, Call "InstallKeyHook" once to install the hook.
//
// 2, Call "AddKeyEntry" to register key entries which consist of information on what
//    kinds of keyboard events should be captured and whom should be notified.
//
// 3, Call "GetKeyEventResult" to retrieve information of the captured key events after
//    your application window receives the notification messages, with value of the
//    wParam and lParam unaltered.
//
// 4, Call "UninstallKeyHook" to release resources as soon as you've finished using
//    the hook.
//
//
// Note on Process & entry number limit:
//
// There are limits on maximum number of processes that can use the hook simultaneously
// and maximum number of entries that can be registered by all processes, if those limits
// are reached, subsequent operations may fail. Current version supports up to 256 unique
// processes to add up to 1024 key entries in total.
//
// Written by Abin (abinn32@yahoo.com)
//
// History:
//
// v1.00 May 06, 2004 - Initial release.
// v1.01 May 14, 2004 - Fixed an error on verifying combination keys and keyboard indicators.
// v1.02 May 26, 2004 - Fixed some minor problems on events translation.
//                      Removed unnecessary combo-key flags and indicator flags.
//////////////////////////////////////////////////////////////////////////////////

#ifndef __KEYHOOK_H__
#define __KEYHOOK_H__

#include <windef.h>
////////////////////////////////////////////////////////////////////////////////
//	The KEYENTRY Struct -- Defines What Kinds of Events Should be Captured,
//						   and How Should the Captured Events to be Processed.
////////////////////////////////////////////////////////////////////////////////
typedef struct tagKeyEntry
{
	UINT nMessage;    // The notification message that will be sent when appropriate key events are captured, must not be zero.
	HWND hCallWnd;    // Handle to the window to which the notification messages will be sent, must not be null.
	HWND hHookWnd;    // Handle to the window in which key events occur. If set to null, key events occurred in any windows are to be captured.
	BYTE iKeyEvent;   // Types of the key events(key-down, key-up, key-repeat) to capture. If set to 0, all kinds of events are to be captured.
	BYTE iMinVKCode;  // The minimum virtual key code to capture, must not be greater than iMaxVKCode.
	BYTE iMaxVKCode;  // The maximum virtual key code to capture, must not be smaller than iMinVKCode.
	BYTE iCombKeys;   // Combination Key Flags(alt, ctrl, shift). If set to 0, combination key states do not matter.
	BYTE iIndicators; // On/off states of keyboard indicators(caps-lock, num-lock, scroll-lock). If set to 0, indicator states do not matter.
	DWORD dwReserved; // Reserved, do not use.
} KEYENTRY, *LPKEYENTRY;
typedef const LPKEYENTRY LPCKEYENTRY;

////////////////////////////////////////////////////////////////////////////////
//	The KEYRESULT Struct -- To be Passed to the "GetKeyEventResult" and Receives
//							Detailed Information of the Captured Key Events.
////////////////////////////////////////////////////////////////////////////////
typedef struct tagKeyResultA
{
	HWND hOccurredInWnd;  // Handle to the window in which the key event occurred
	BYTE iKeyEvent;       // The captured key event type(key-down, key-up, or key-repeat).		
	BYTE iVKCode;         // The virtual key code produced by the captured key-stroke.
	BYTE iCombKeys;       // Combination Key Flags(alt, ctrl, shift) when the key event was captured.
	BYTE iIndicators;     // On/off states of keyboard indicators(caps-lock, num-lock, scroll-lock) when the key event was captured.
	char chPrintableChar; // The printable character produced by the captured key-stroke, 0 if not printable.
	char szKeyName[32];   // Name of the key. e.g. "Shift".
} KEYRESULTA, *LPKEYRESULTA;

typedef struct tagKeyResultW
{
	HWND hOccurredInWnd;     // Handle to the window in which the key event occurred
	BYTE iKeyEvent;          // The captured key event type(key-down, key-up, or key-repeat).		
	BYTE iVKCode;            // The virtual key code produced by the captured key-stroke.
	BYTE iCombKeys;          // Combination Key Flags(alt, ctrl, shift) when the key event was captured.
	BYTE iIndicators;        // On/off states of keyboard indicators(caps-lock, num-lock, scroll-lock) when the key event was captured.
	wchar_t chPrintableChar; // The printable character produced by the captured key-stroke, 0 if not printable.
	wchar_t szKeyName[32];   // Name of the key. e.g. "Shift".
} KEYRESULTW, *LPKEYRESULTW;

#ifdef UNICODE
typedef KEYRESULTW				KEYRESULT;
typedef LPKEYRESULTW			LPKEYRESULT;
typedef const LPKEYRESULTW		LPCKEYRESULT;
#else
typedef KEYRESULTA				KEYRESULT;
typedef LPKEYRESULTA			LPKEYRESULT;
typedef const LPKEYRESULTA		LPCKEYRESULT; 
#endif // UNICODE

//////////////////////////////////////////////////////////////////////////////////
//	KEYRESULT Masks -- Used by the "GetKeyEventResult" Function, Specifies
//					   Which Members of the KEYRESULT struct must be filled in.
//////////////////////////////////////////////////////////////////////////////////
#define KH_MASK_ALL		  0xffffffff // All members must be filled in
#define KH_MASK_OCCURREDWND		0x01 // The "hOccurredInWnd" member must be filled in
#define KH_MASK_EVENTTYPE		0x02 // The "iKeyEvent" member must be filled in
#define KH_MASK_VKCODE			0x04 // The "iVKCode" member must be filled in
#define KH_MASK_COMBKEYS		0x08 // The "iCombKeys" member must be filled in
#define KH_MASK_INDICATORS		0x10 // The "iIndicators" member must be filled in
#define KH_MASK_PRINTABLECHAR	0x20 // The "chPrintableChar" member must be filled in
#define KH_MASK_KEYNAME			0x40 // The "szKeyName" member must be filled in

//////////////////////////////////////////////////////////////////////////////////
//	Key Event Types -- Key-Down, Key-Up, Key-Repeat.
//////////////////////////////////////////////////////////////////////////////////
#define KH_KEY_DOWN				0x01 // Key-down event
#define KH_KEY_UP				0x02 // Key-up event
#define KH_KEY_REPEAT			0x04 // Key-repeat event, the key is held down for long enough

//////////////////////////////////////////////////////////////////////////////////
//	Combination Key Flags -- States of the Alt, Ctrl, and Shift Keys.
//////////////////////////////////////////////////////////////////////////////////
#define KH_ALT_PRESSED			0x01 // The "Alt" key is(or must be) pressed
#define KH_ALT_NOT_PRESSED		0x02 // The "Alt" key must not be pressed
#define KH_CTRL_PRESSED			0x04 // The "Ctrl" key is(or must be) pressed
#define KH_CTRL_NOT_PRESSED		0x08 // The "Ctrl" key must not be pressed
#define KH_SHIFT_PRESSED		0x10 // The "Shift" key is(or must be) pressed
#define KH_SHIFT_NOT_PRESSED	0x20 // The "Shift" key must not be pressed

//////////////////////////////////////////////////////////////////////////////////
//	Keyboard Indicator Flags -- On/Off States of the Caps-Lock, Num-Lock, and
//								Scroll-Lock Indicators. 
//////////////////////////////////////////////////////////////////////////////////
#define KH_CAPSLOCK_ON			0x01 // The "Caps-Lock" indicator is(or must be) on
#define KH_CAPSLOCK_OFF			0x02 // The "Caps-Lock" indicator must be off
#define KH_NUMLOCK_ON			0x04 // The "Num-Lock" indicator is(or must be) on
#define KH_NUMLOCK_OFF			0x08 // The "Num-Lock" indicator must be off
#define KH_SCRLOCK_ON			0x10 // The "Scroll-Lock" indicator is(or must be) on
#define KH_SCRLOCK_OFF			0x20 // The "Scroll-Lock" indicator must be off

//////////////////////////////////////////////////////////////////////////////////
//		Function Return Values & Error Codes
//////////////////////////////////////////////////////////////////////////////////
enum {  KH_OK = 0,						// Operation completed successfully.
		KH_ERR_LOCK_FAIL,				// Failed to obtain ownership of the mutex.
		KH_ERR_ALREADY_INSTALLED,		// Hook is already installed by current process.
		KH_ERR_NOT_INSTALLED,			// Hook is not installed by current process.
		KH_ERR_INSTALL_FAIL,			// ::SetWindowsHookEx API failed.
		KH_ERR_UNINSTALL_FAIL,			// ::UnhookWindowsHookEx API failed.
		KH_ERR_MAX_ENTRY_REACHED,		// Maxumum entry number is reached.
		KH_ERR_ALREADY_REGISTERED,		// Entry already registered.
		KH_ERR_NOT_REGISTERED,			// Entry is not registered.
		KH_ERR_PROCESS_IN_USE,			// The specified process has already installed the hook.
		KH_ERR_MAX_PROCESS_REACHED,		// Maximum process number is reached
		KH_ERR_INVALID_PARAM,			// Invalid parameter(s).
		KH_ERR_INVALID_CALLWND,			// Invalid call window handle.
		KH_ERR_INVALID_USERMESSAGE,		// Invalid user message ID
		KH_ERR_INVALID_VKCODE_SEQUENCE, // Invalid virtual key code sequence.
		KH_ERR_INVALID_COMBKEY_FLAGS,	// Invalid combination key flags.
		KH_ERR_INVALID_INDICATOR_FLAGS  // Invalid keyboard indicator flags.
	};

//////////////////////////////////////////////////////////////////////////////////
//		Exported Functions
//////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------------*/
LRESULT __declspec(dllexport) InstallKeyHook();					
																				/*					
*Return Values*

	The function returns KH_OK if succeeded, an error code otherwise.

*Remarks*
	
	Registers current process to the hook. Applications must call this function
	before using the hook.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
LRESULT __declspec(dllexport) UninstallKeyHook();
																				/*										
*Return Values*

	The function returns KH_OK if succeeded, an error code otherwise.

*Remarks*
	
	Unregisters current process from the hook. Applications should call this 
	function as soon as they finish using the hook, to release all occupied global
	resources.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
BOOL __declspec(dllexport) IsKeyHookInstalled();
																				/*					
*Return Values*

	The function returns non-zero if current process has already been registered
	to the hook, zero otherwise.

*Remarks*
	
	Check for whether current process has already been registered to the hook.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
LRESULT __declspec(dllexport) AddKeyEntry(LPCKEYENTRY lpEntry);
																				/*					
*Parameters*
	
	lpEntry	
		[in] Address of a KEYENTRY struct which contains information of the key
		event to be captured.

*Return Values*

	The function returns KH_OK if succeeded, an error code otherwise.

*Remarks*
	
	Adds a new key entry to the hook entry list. So that key events match the
	information defined in lpEntry will be captured.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
LRESULT __declspec(dllexport) RemoveKeyEntry(LPCKEYENTRY lpEntry);
																				/*
*Parameters*
	
	lpEntry	
		[in] Address of a KEYENTRY struct which contains information of the key
		entry to be removed from the hook entry list.

*Return Values*

	The function returns KH_OK if succeeded, an error code otherwise.

*Remarks*
	
	Removes a specified key entry from the hook entry list, so that the specified
	key event will no longer be captured. Key entries that were added by other
	processes are not accessible and will not be affected.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
LRESULT __declspec(dllexport) RemoveAllKeyEntries();
																				/*
*Return Values*

	The function returns KH_OK if succeeded, an error code otherwise.

*Remarks*
	
	Removes all key entries that were added by current process from the hook entry
	list. Key entries that were added by other processes are not accessible and will
	not be affected.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
BOOL __declspec(dllexport) KeyEntryExists(LPCKEYENTRY lpEntry, LRESULT* pError = NULL);
																				/*
*Parameters*
	
	lpEntry
		[in] Address of a KEYENTRY struct which contains information of the key entry.

	pError
		[out] Address of a LRESULT variable which will receive ocurred error codes.

*Return Values*

	The function returns non-zero if the specified key entry was added by current
	process, zero otherwise.

*Remarks*
	
	Checks for whether a specified key entry was added by current process. Key entries
	that were added by other processes are not accessible and will not be counted.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
INT __declspec(dllexport) GetKeyEntryCount(LRESULT* pError = NULL);
																				/*
*Parameters*
	
	pError
		[out] Address of a LRESULT variable which will receive ocurred error codes.

*Return Values*

	If succeeded, the function returns number of key entries that were added by current
	process, otherwise it returns -1.

*Remarks*
	
	Retrieves number of key entries that were added by current process. Key entries
	that were added by other processes are not accessible and will not be counted.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
INT __declspec(dllexport) GetKeyEntryList(LPKEYENTRY lpBuffer, INT nMaxCount, LRESULT* pError = NULL);
																				/*
*Parameters*
	
	lpBuffer
		[out] Address of the buffer which will receive the key entry array.

	nMaxCount
		[in] Maximum number of KEYENTRY objects that the buffer can receive.

	pError
		[out] Address of a LRESULT variable which will receive ocurred error codes.

*Return Values*

	If succeeded, the function returns number of key entries that have been copied
	to the buffer, otherwise it returns -1.

*Remarks*
	
	Retrieves a list of key entries that were added by current process. Key entries
	that were added by other processes are not accessible and will not be counted.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
LRESULT __declspec(dllexport) GetKeyEventResult(WPARAM wParam, LPARAM lParam, LPKEYRESULT lpKeyResult, UINT nMask = KH_MASK_ALL);
																				/*					
*Parameters*
	
	wParam
		[in] The WPARAM that was received with the notification message.

	lParam
		[in] The LPARAM that was received with the notification message.	

	lpKeyResult
		[out] Address of a KEYRESULT struct which will receive detailed information
		of the captured key event.

	nMask
		[in] Specifies which member(s) of the KEYRESULT struct must be filled in.

*Return Values*

	The function returns KH_OK if succeeded, an error code otherwise.

*Remarks*
	
	When a predefined key event is captured, the message defined in KEYENTRY::nMessage
	will be sent to the target window defined in KEYENTRY::hCallWnd along with a wParam
	and a lParam, which contain detailed information of the captured key event. This
	function parses those parameters and extracts the information.
--------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
INT __declspec(dllexport) FormatKeyHookError(LRESULT lErrorCode, LPTSTR lpszBuffer, INT nMaxLength);
																				/*				
*Parameters*
	
	lErrorCode
		[in] The error code to be formated.

	lpszBuffer
		[out] Address of the buffer that will receive the formatted string.

	nMaxLength
		[in] Maximum number of characters the buffer can receive.

*Return Values*

	The function returns number of characters that were copied to the buffer.

*Remarks*
	
	Formats a predefined error code into a human understandable character string which
	describes the error information.
--------------------------------------------------------------------------------*/
void  __declspec(dllexport) LockComputer(BOOL bLock);
void __declspec(dllexport)  SwitchMyDesktop();
inline bool TestUnlockCondition();
#endif // #ifndef __KEYHOOK_H__
