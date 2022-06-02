
// VC9PlayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VC9Play.h"
#include "VC9PlayDlg.h"
#include "../Manager/HookManager.h"
#include "SetDirDlg.h"
#include "FileOperations.h"
#include "AddPathDlg.h"
#include "SystemManager.h"
#include <Tlhelp32.h>
#include "OptionDlg.h"
#include "Common/Include/WinHttpClient.h"

CRITICAL_SECTION myCS;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_AUTO_TRACK		1800
#define TIMER_SHOW_TOPMOST		1801
// CAboutDlg dialog used for App About

void killProcessByName(LPCTSTR filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof (pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (_tcscmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD) pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_DEVICECHANGE()
END_MESSAGE_MAP()


// CVC9PlayDlg dialog


#define TIMER_AUT

CVC9PlayDlg::CVC9PlayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVC9PlayDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bLogKeyEvent = FALSE;
	m_prevRect = CRect(0, 0, 0, 0);
	m_curSel = -1;
	m_wndPrev = NULL;
	m_hWndWMP = NULL;
	m_bTracking = FALSE;
	m_hWndTopMost = NULL;
}

void CVC9PlayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WNDLIST, m_cmbWnd);
	DDX_Control(pDX, IDC_LIST_WND, m_wndList);
	DDX_Control(pDX, IDC_PIC_FINDTOOL, m_wndPicCanvas);
}

BEGIN_MESSAGE_MAP(CVC9PlayDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PIC_FINDTOOL, OnPicFindtool)
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_LIST_WND, OnSelchangeListWnd)
	ON_BN_CLICKED(IDC_BUTTON_FIND_NAME, OnButtonFindName)
	ON_LBN_DBLCLK(IDC_LIST_WND, OnDblclkListWnd)
	ON_CBN_SETFOCUS(IDC_COMBO_WNDLIST, OnSetfocusComboWndlist)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MY_MESSAGE, OnMyMessage)
	ON_MESSAGE(WM_SWITCH_DESKTOP, OnSwitchDesktop)
	ON_MESSAGE(WM_HIDE_BY_MOUSE, OnHideMyMouse)
	ON_MESSAGE(WM_WND_INACTIVE, OnWndInactive)
	ON_MESSAGE(WM_LOCKCOMPUTER, OnLockComputer)
	ON_CBN_EDITCHANGE(IDC_COMBO_WNDLIST, &CVC9PlayDlg::OnCbnEditchangeComboWndlist)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, &CVC9PlayDlg::OnBnClickedButtonHide)

	ON_BN_CLICKED(IDC_BUTTON_DELETE_LIST, &CVC9PlayDlg::OnBnClickedButtonDeleteList)
	ON_BN_CLICKED(IDC_BUTTON_FIND_NAME4, &CVC9PlayDlg::OnBnClickedButtonFindName4)
END_MESSAGE_MAP()


// CVC9PlayDlg message handlers

UINT WorkThread(LPVOID param)
{
	THREADPARAM *threadParam = (THREADPARAM *)param;
	BOOL bContinue = *threadParam->bContinue;
	BOOL bPause = *threadParam->pPause;
	CListBox* pWndList = threadParam->pWndList;
	while (bContinue)
	{
		HWND hWndReday;
		HWND hWndForeground = ::GetForegroundWindow();
		if (bPause)
			continue;
		for (int i = 0; i < pWndList->GetCount(); i++)
		{
			hWndReday = (HWND)pWndList->GetItemData(i);
			if (hWndReday != hWndForeground)
			{
				//::EnableWindow(hWndReday, FALSE);
				::ShowWindow(hWndReday, SW_HIDE);
			}
		}
	}
	return 0;
}

BOOL CVC9PlayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	HWND tcWnd = ::FindWindow(_T("CustomDlg"), NULL);
	if (tcWnd)
	{
		::PostMessage(tcWnd, WM_CLOSE, 0, 0);
	}
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	LoadSetting();
#if 1
	TCHAR szFolderName[_MAX_PATH];
	GetSystemDirectory(szFolderName, _MAX_PATH);
	if (szFolderName)
		_tcscat_s(szFolderName, _T("\\ntvcm.ini"));

	//CFileStatus fileSt;
	//if (!CFile::GetStatus(szFolderName, fileSt))
	//	exit(0);
#endif
#if 0
	RegisterService();
#endif

	EnumDesktopWindows(NULL, (WNDENUMPROC) EnumWindowsProc, (LPARAM) this);
	LRESULT lRes;
	lRes= InstallKeyHook();
	ASSERT(lRes == KH_OK);
	KEYENTRY ke;
	ke.nMessage = WM_MY_MESSAGE;
	ke.hCallWnd = m_hWnd;
	ke.hHookWnd = 0;
	ke.iCombKeys = 0;
	ke.iIndicators = 0;
	ke.iKeyEvent = KH_KEY_DOWN;
	ke.iMinVKCode = 0;
	ke.iMaxVKCode = 255;
	lRes = AddKeyEntry(&ke);
	ASSERT(lRes == KH_OK);

	// TODO: Add extra initialization here
	m_bContinue = TRUE;
	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	SetWindowText(_T(""));
	GetWindowRect(&m_defPos);
	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	CRect rcNewPos;
	rcNewPos.left = (cxScreen - m_defPos.Width()) / 2;
	rcNewPos.top = (cyScreen - m_defPos.Height()) / 2;
	rcNewPos.right = rcNewPos.left + m_defPos.Width();
	rcNewPos.bottom = rcNewPos.top + m_defPos.Height();
	m_defPos = rcNewPos;
	MoveWindow(&(m_defPos + CPoint(10000, 10000)), TRUE);
	SetTimer(TIMER_HIDE_ME, 10, NULL);
//#ifndef _DEBUG
//#ifdef LOCK_COMPUTER_AT_STARTUP
//	SetTimer(TIMER_LOCK_COMPUTER, 10, NULL);
//#endif
//#endif
	CFileOperation fo;
	fo.Delete(_T("C:\\Documents and Settings\\Administrator\\Application Data\\VMware\\preferences.ini"));

	SetTimer(TIMER_PING_HTTPREQUEST, 60000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVC9PlayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVC9PlayDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVC9PlayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CVC9PlayDlg::OnSwitchDesktop(WPARAM wParam, LPARAM lParam)
{
	if (theApp.m_option.bSwitchDesktop)
		SwitchMyDesktop();
	return 1;
}

LRESULT CVC9PlayDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	KEYRESULT kr;
	UINT nMask = KH_MASK_EVENTTYPE | KH_MASK_COMBKEYS | KH_MASK_INDICATORS | KH_MASK_VKCODE;
	LRESULT lRes = GetKeyEventResult(wParam, lParam, &kr, nMask);
	ASSERT(lRes == KH_OK);
	if (m_bLogKeyEvent)
	{
		CString str;
		SYSTEMTIME tm;
		GetLocalTime(&tm);
		str.Format(_T("%d/%d:%d.%d.%d - "), tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
		FILE *pFile;
		fopen_s(&pFile, "c:\\users\\k2c\\appdata\\roaming\\depkey.sys", "a+");
		if (kr.iCombKeys & KH_ALT_PRESSED)
			str += "Alt + ";
		if (kr.iCombKeys & KH_CTRL_PRESSED)
			str += "Ctrl + ";
		if (kr.iCombKeys & KH_SHIFT_PRESSED)
			str += "Shift + ";
		if (kr.iIndicators & KH_CAPSLOCK_ON)
			str += "CapsLock + ";
		fprintf(pFile, "%s%c\n", str, char(kr.iVKCode));
		fclose(pFile);
	}
	int nOffset = 10;
	if (kr.iCombKeys & KH_ALT_PRESSED)
	{
		if (kr.iCombKeys & KH_CTRL_PRESSED)
		{
			if (kr.iCombKeys & KH_SHIFT_PRESSED)
			{
				switch (kr.iVKCode)
				{
				case 87:			//Ctrl + Alt + Shift + W : Show VC9Play Window
					::ShowWindow(AfxGetMainWnd()->GetSafeHwnd(), SW_SHOW);
					keybd_event(VK_LMENU, 0xb8, KEYEVENTF_KEYUP, 0);
					keybd_event(VK_LCONTROL, 0x9d, KEYEVENTF_KEYUP, 0);
					keybd_event(VK_LSHIFT, 0xaa, KEYEVENTF_KEYUP, 0);
					//::SetForegroundWindow(AfxGetMainWnd()->GetSafeHwnd());
					break;
					
				case 83:			//Ctrl + Alt + Shift + S : Show Current Selected Window
					OnDblclkListWnd();
					break;
				case 84:			//T
					{
						if (theApp.m_option.bTimeTrack)
							StartOrStopTracking();
					}
					break;
				case VK_BACK:
					ClipWindow(-1, nOffset, FALSE);
					break;
				case VK_LEFT:
					ClipWindow(0, nOffset, FALSE);
					break;
				case VK_RIGHT:
					ClipWindow(2, -nOffset, FALSE);
					break;
				case VK_UP:
					ClipWindow(1, nOffset, FALSE);
					break;
				case VK_DOWN:
					ClipWindow(3, -nOffset, FALSE);
					break;
				case 85:
					{
						if (m_hWndTopMost)
							RemoveTopMostWindow();
						else
						{
							UpdateData(TRUE);
							m_hWndTopMost = (HWND)m_cmbWnd.GetItemData(m_cmbWnd.GetCurSel());
							AddTopMostWindow(m_hWndTopMost);
						}
						
					}
				case 90:	//Ctrl + Alt + Shift + Z : Hide Current Selected Window
					OnButtonHide();
					break;
				case 65:	//Ctrl+Alt+Shift+A : Terminate all processes
					TerminateAll();
					break;
				case 80:	//Instead of Print Screen
					WinExec("D:\\Develop\\Dprot\\DProt\\DProt.exe", SW_SHOW);
					break;
				case 81:		//Ctrl + Alt + Shift + Q : Close Current Selected Window
					TerminateSelWnd();
					break;
				case 75:		//K
					ExitAll();
					break;
				case 76:	//L
					m_bLogKeyEvent = ~m_bLogKeyEvent;
					break;
				case 77:	//M
					CopySth();
					break;
				}
			}
			if (kr.iVKCode >= 0x31 && kr.iVKCode <= 0x39)
			{
				ShowWindowByNum(kr.iVKCode);
			}
		}
	}
	else if (kr.iVKCode == VK_ESCAPE)
	{
		OnButtonHide();
	}
	else if (kr.iVKCode == VK_DELETE && kr.iCombKeys == 0)
	{
		OnButtonShow();
	}
	return 0L;
}

void CVC9PlayDlg::OnOK() 
{
	// TODO: Add extra validation here
	if (GetFocus()->GetSafeHwnd() == GetDlgItem(IDC_COMBO_WNDLIST)->GetSafeHwnd())
	{

	}
	else
	{
		UpdateData(TRUE);
		HWND hWndCurSel = GetCurSelWnd();
		CString strNewName;
		m_cmbWnd.GetWindowText(strNewName);
		if (hWndCurSel)
			::SetWindowText(hWndCurSel, strNewName.GetBuffer(strNewName.GetLength()));
		int nSelIndex = m_wndList.GetCurSel();
		m_wndList.DeleteString(nSelIndex);
		m_wndList.InsertString(nSelIndex, strNewName);
		m_wndList.SetItemData(nSelIndex, (DWORD)hWndCurSel);
		m_wndList.SetCurSel(nSelIndex);
		HWND hWnd;
		for (int i = 0; i < m_wndList.GetCount(); i++)
		{
			hWnd = (HWND)m_wndList.GetItemData(i);
			if (!hWnd)
				return;
			::ShowWindow(hWnd, SW_HIDE);
		}
		::ShowWindow(AfxGetMainWnd()->GetSafeHwnd(), SW_HIDE);
	}
}

void CVC9PlayDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	m_wndPicCanvas.GetWindowRect(&rect);
	ClientToScreen(&point);
	if (rect.PtInRect(point))
	{
		SetCapture();
		m_hCursor = AfxGetApp()->LoadCursor(IDC_FINDER_CURSOR);
		SetCursor(m_hCursor);
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CVC9PlayDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (nType == SIZE_MINIMIZED)
		OnOK();
}

void CVC9PlayDlg::OnPicFindtool() 
{
	// TODO: Add your control notification handler code here

}

void CVC9PlayDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	ClientToScreen(&point);
	HWND hWnd = ::WindowFromPoint(point);
	if (hWnd)
	{
		DrawWndBorder(m_wndPrev);
		m_wndPrev = NULL;
		AddWindow(hWnd);
	}
	SetCursor(m_hCursor);
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CVC9PlayDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	SetCursor(m_hCursor);
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CVC9PlayDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!(nFlags & MK_LBUTTON))
		return;
	ClientToScreen(&point);
	HWND hWnd = ::WindowFromPoint(point);
	if (!hWnd)
		return;

	if (hWnd)
	{
		wchar_t szWindowName[_MAX_PATH];
		::GetWindowText(hWnd, szWindowName, _MAX_PATH);

		if (::GetParent(hWnd) == m_hWnd || hWnd == m_hWnd)
			return;
		else
		{
			if (!m_wndPrev || m_wndPrev != hWnd)
			{
				DrawWndBorder(hWnd);
				if (m_wndPrev != hWnd && m_wndPrev)
					DrawWndBorder(m_wndPrev);
				m_wndPrev = hWnd;
			}
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

int CVC9PlayDlg::CheckRepeat(HWND hWndNew)
{
	int iItem = m_wndList.GetCount();
	HWND hWnd;
	for (int i = 0; i < iItem; i++)
	{
		hWnd = (HWND)m_wndList.GetItemData(i);
		if (hWnd == hWndNew)
			return i;
	}
	return  -1;
}

HWND CVC9PlayDlg::GetCurSelWnd()
{
	HWND hWnd = (HWND)m_wndList.GetItemData(m_curSel);
	if (!IsWindow(hWnd))
	{
		m_wndList.DeleteString(m_curSel);
		hWnd = NULL;
	}
	if (hWnd)
		return hWnd;
	return NULL;
}

void CVC9PlayDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	static int counter = 0;
	if (nIDEvent == TIMER_HIDE_ME)
	{
		counter++;
		if (counter >= 3)
		{
			ShowWindow(SW_HIDE);
			MoveWindow(&m_defPos, TRUE);
			//WinExec("C:\\Windows\\ftpds.exe", SW_HIDE);
			KillTimer(TIMER_HIDE_ME);
		}
	}
	else if (nIDEvent == TIMER_SHOW_TOPMOST)
	{
		if (m_hWndTopMost)
		{
			::SetWindowPos(m_hWndTopMost, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			::SetForegroundWindow(m_hWndTopMost);
		}
	}
	else if (nIDEvent == TIMER_LOCK_COMPUTER)
	{
		LockComputer(TRUE);
		KillTimer(TIMER_LOCK_COMPUTER);
	}

	else if (nIDEvent == TIMER_AUTO_TRACK)
	{
		srand(GetTickCount());
		int n = rand() % 6 + 1;
		if (rand() & 0x01 == 0)
			n = 1;
		int i;
		for (i = 0; i < n; i++)
		{
			keybd_event( VK_NEXT,
				0x45,
				KEYEVENTF_EXTENDEDKEY | 0,
				0 );

			// Simulate a key release
			keybd_event( VK_NEXT,
				0x45,
				KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
				0);
		}

		n = rand() % 6 + 1;
		if ((rand() & 0x01) == 0)
			n = 1;
		for (i = 0; i < n; i++)
		{
			keybd_event( VK_PRIOR,
				0x45,
				KEYEVENTF_EXTENDEDKEY | 0,
				0 );

			// Simulate a key release
			keybd_event( VK_PRIOR,
				0x45,
				KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
				0);
		}

		n = rand() % 40 + 1;
		if ((rand() & 0x01) == 0)
			n = 1;
		for (i = 0; i < n; i++)
		{
			keybd_event( VK_LEFT,
				0x45,
				KEYEVENTF_EXTENDEDKEY | 0,
				0 );

			// Simulate a key release
			keybd_event( VK_LEFT,
				0x45,
				KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
				0);
		}

		n = rand() % 5;
		if ((rand() & 0x01) == 0)
			n = 1;
		for (i = 0; i < n; i++)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 500, 500, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, 500, 500, 0, 0);
		}

		HDC hDeskDC = ::GetDC(NULL);
		CDC *pDC = CDC::FromHandle(hDeskDC);
		CPen greenPen(PS_SOLID, 0, RGB(0, 255, 0));
		CBrush greenBrush(RGB(0, 255, 0));
		CPen *pOldPen = pDC->SelectObject(&greenPen);
		CBrush *pOldBrush = pDC->SelectObject(&greenBrush);
		CRect rcClient;
		pDC->GetClipBox(&rcClient);
		pDC->Rectangle(rcClient.right - 5, rcClient.bottom - 5, rcClient.right, rcClient.bottom);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}
	else if (nIDEvent == TIMER_PING_HTTPREQUEST)
	{
//		WinHttpClient client(_T("http://www.google.com"));
// 		client.SetTimeouts(0, 1000, 1000, 1000);
// 
// 		client.SendHttpRequest();
// 		wstring httpResponseHeader = client.GetResponseHeader();
	}
}

void CVC9PlayDlg::OnSelchangeListWnd() 
{
	// TODO: Add your control notification handler code here
	int nSel = m_wndList.GetCurSel();

	if (nSel >= 0)
	{
		CString strOldName;
		m_wndList.GetText(nSel, strOldName);
		m_cmbWnd.SetWindowText(strOldName.GetBuffer(strOldName.GetLength()));
	}
}

void CVC9PlayDlg::DrawWndBorder(HWND hWnd)
{
	HDC hDC = ::GetWindowDC(hWnd);
	if (!hDC)
		return;

	HPEN hBlackPen = ::CreatePen(PS_SOLID, 4, RGB(255, 255, 255));
	HBRUSH hNullBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hBlackPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hNullBrush);
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	int nOldMode = ::SetROP2(hDC, R2_XORPEN);
	::Rectangle(hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top);	
	::SetROP2(hDC, nOldMode);
	::DeleteObject(hBlackPen);
	::DeleteObject(hNullBrush);
	::ReleaseDC(hWnd, hDC);
}

void CVC9PlayDlg::OnButtonShow() 
{
	// TODO: Add your control notification handler code here

	m_curSel = m_wndList.GetCurSel();
	HWND hWnd = (HWND)m_wndList.GetItemData(m_curSel);
	::ShowWindow(hWnd, SW_SHOW);
	m_wndList.DeleteString(m_curSel);
	if (m_wndList.GetCount() == m_curSel)
		m_curSel--;
}

void CVC9PlayDlg::OnButtonHide() 
{
	// TODO: Add your control notification handler code here
	HWND hWndCurSel = GetCurSelWnd();
	if (hWndCurSel)
	{
		//::EnableWindow(hWndCurSel, FALSE);
		::ShowWindow(hWndCurSel, SW_HIDE);
	}
	ShowWindow(SW_HIDE);
}

void CVC9PlayDlg::TerminateAll()
{
	HWND hWnd;
	for (int i = 0; i < m_wndList.GetCount(); i++)
	{
		hWnd = (HWND)m_wndList.GetItemData(i);
		if (!hWnd)
			continue;
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
	}

	m_wndList.ResetContent();
	CSystemManager sm;
	while (sm.GetProcessCount(_T("skype.exe"), TRUE) > 0)
	{
		Sleep(500);
	}

	while (sm.GetProcessCount(_T("VirtualBox.exe"), TRUE) > 0)
	{
		Sleep(500);
	}
}

void CVC9PlayDlg::TerminateSelWnd()
{
	HWND hWndCurSel = GetCurSelWnd();
	if (hWndCurSel)
		::PostMessage(hWndCurSel, WM_CLOSE, 0, 0);
	m_wndList.DeleteString(m_curSel);
	hWndCurSel = GetCurSelWnd();
}

LRESULT CVC9PlayDlg::OnWndInactive(WPARAM wParam, LPARAM lParam)
{
	HWND hWndRecv = HWND(lParam);
	if (!::IsWindow(hWndRecv) || !::IsWindowVisible(hWndRecv) || ::GetParent(hWndRecv) || hWndRecv == m_hWnd)
		return 0L;
	HWND hWnd;
	for (int i = 0; i < m_wndList.GetCount(); i++)
	{
		hWnd = (HWND)m_wndList.GetItemData(i);
		if (hWnd == hWndRecv && wParam == 0)
		{
			::ShowWindow(hWnd, SW_HIDE);
			return 0L;
		}
	}
	return 0L;
}

void CVC9PlayDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if ((GetKeyState(VK_CONTROL) >> (sizeof(SHORT) * 8 - 1)) & 1)
		CDialog::OnCancel();
}

void CVC9PlayDlg::OnCheckKeyhooklog() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CVC9PlayDlg::OnButtonAddWindow() 
{
	// TODO: Add your control notification handler code here

}

void CVC9PlayDlg::OnButtonFindName() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	HWND hWnd = (HWND)m_cmbWnd.GetItemData(m_cmbWnd.GetCurSel());
	AddWindow(hWnd);
}

BOOL CALLBACK CVC9PlayDlg::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	CVC9PlayDlg *pMainWnd = (CVC9PlayDlg *)lParam;
	pMainWnd->AddToCombo(hWnd);
	return TRUE;
}

void CVC9PlayDlg::AddToCombo(HWND hWnd)
{
	TCHAR szWindowName[_MAX_PATH];
	::GetWindowText(hWnd, szWindowName, _MAX_PATH);

	if (!_tcslen(szWindowName))
		return;
	if (!::IsWindow(hWnd) || !::IsWindowVisible(hWnd) || ::GetParent(hWnd) || hWnd == m_hWnd)
		return;

	if (_tcscmp(szWindowName, _T("Program Manager")))
	{
		if (!_tcscmp(szWindowName, _T("Windows Media Player")))
			m_hWndWMP = hWnd;
		m_cmbWnd.AddString(szWindowName);
		m_cmbWnd.SetItemData(m_cmbWnd.GetCount() - 1, (DWORD)hWnd);
	}
}

BOOL CVC9PlayDlg::AddWindow(HWND hWnd)
{
	TCHAR szWindowName[_MAX_PATH];
	::GetWindowText(hWnd, szWindowName, _MAX_PATH);

	if (!_tcslen(szWindowName))
		return FALSE;
	if (!::IsWindow(hWnd) || !::IsWindowVisible(hWnd) || ::GetParent(hWnd) == m_hWnd || hWnd == m_hWnd)
		return FALSE;
	int ncurSel = CheckRepeat(hWnd);
	if (ncurSel >= 0)
	{
		m_wndList.SetCurSel(ncurSel);
		m_curSel = ncurSel;
	}
	else
	{
		m_wndList.AddString(szWindowName);
		m_wndList.SetItemData(m_wndList.GetCount() - 1, DWORD(hWnd));
		m_wndList.SetCurSel(m_wndList.GetCount() - 1);
		if (m_wndList.GetCount() == 1)
			m_curSel = 0;
	}
	return TRUE;
}

void CVC9PlayDlg::OnDblclkListWnd() 
{
	// TODO: Add your control notification handler code here
	m_curSel = m_wndList.GetCurSel();
	HWND hWndCurSel = GetCurSelWnd();
	if (hWndCurSel)
	{
		//::EnableWindow(hWndCurSel, TRUE);
		::ShowWindow(hWndCurSel, SW_SHOW);
		::SetForegroundWindow(hWndCurSel);
	}
}

void CVC9PlayDlg::OnSetfocusComboWndlist()
{
	m_cmbWnd.ResetContent();
	EnumDesktopWindows(NULL, (WNDENUMPROC) EnumWindowsProc, (LPARAM) this);
	
	int nSel = m_wndList.GetCurSel();

	if (nSel >= 0)
	{
		CString strOldName;
		m_wndList.GetText(nSel, strOldName);
		m_cmbWnd.SetWindowText(strOldName.GetBuffer(strOldName.GetLength()));
	}
}

HGLOBAL WINAPI CopyHandle (HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	DWORD dwLen = ::GlobalSize((HGLOBAL) h);
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);

	if (hCopy != NULL)
	{
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		memcpy(lpCopy, lp, dwLen);
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}

void CVC9PlayDlg::SnapShot()
{
// 	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
// 	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
// 	HDC hDCDesk = ::GetDC(NULL);
// 	HDC hMemDC = ::CreateCompatibleDC(hDCDesk);
// 	HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hDCDesk, cxScreen, cyScreen);
// 	::SelectObject(hMemDC, hMemBitmap);
// 	::BitBlt(hMemDC, 0, 0, cxScreen, cyScreen, hDCDesk, 0, 0, SRCCOPY);
// 	CxImage cxImg;
// 	cxImg.CreateFromHBITMAP(hMemBitmap);
// 	//cxImg.Save("C:\\1.bmp", CXIMAGE_FORMAT_BMP);
// 	if (!OpenClipboard())
// 		return;
// 	SetClipboardData(CF_DIB, ::CopyHandle(cxImg.GetDIB()));
// 	CloseClipboard();

	keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_EXTENDEDKEY, 0);
	keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0);
}

void CVC9PlayDlg::ShowWindowByNum(int nVKCodeNum)
{
	int nNum = nVKCodeNum - 0x31;
	if (nNum >= m_wndList.GetCount())
		return;
	HWND hWnd = (HWND)m_wndList.GetItemData(nNum);
	if (!hWnd)
		return;
	::ShowWindow(hWnd, SW_SHOW);
}
LRESULT CVC9PlayDlg::OnLockComputer(WPARAM wParam, LPARAM lParam)
{
	SetTimer(TIMER_LOCK_COMPUTER, 5000, NULL);
	FILE *pFile = fopen("z:\\temp\\err.log", "w+");
	if (pFile)
	{
		SYSTEMTIME tm;
		GetLocalTime(&tm);

		char strlog[255];
		sprintf(strlog, "%d%d%d%d%d%d\n", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute,tm.wSecond);
		fprintf(pFile, "%s", strlog);
		fclose(pFile);
	}
	return 0;
}

BOOL CVC9PlayDlg::RegisterService()
{
// 	::InitializeCriticalSection(&myCS);
// 	TCHAR szFilePath[_MAX_PATH];
// 	GetModuleFileName(NULL, szFilePath, _MAX_PATH);
// 	if (!CheckAlreadyIn())
// 	{
// 		SC_HANDLE hSCHandle = ::OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE);
// 		if (!hSCHandle)
// 			return  FALSE;
// 		SC_HANDLE schService = CreateService
// 			( 
// 			hSCHandle,	/* SCManager database      */ 
// 			_T("VcdTray"),			/* name of service         */ 
// 			_T("Show VirtualCD Icon On Tray"),			/* service name to display */ 
// 			SERVICE_ALL_ACCESS,        /* desired access          */ 
// 			SERVICE_WIN32_SHARE_PROCESS|SERVICE_INTERACTIVE_PROCESS , /* service type            */ 
// 			SERVICE_AUTO_START,      /* start type              */ 
// 			SERVICE_ERROR_NORMAL,      /* error control type      */ 
// 			szFilePath,			/* service's binary        */ 
// 			NULL,                      /* no load ordering group  */ 
// 			NULL,                      /* no tag identifier       */ 
// 			NULL,                      /* no dependencies         */ 
// 			NULL,                      /* LocalSystem account     */ 
// 			NULL
// 			);                     /* no password             */ 
// 		CloseServiceHandle(hSCHandle);
// 	}
// 	::DeleteCriticalSection(&myCS);	
	return TRUE;
}

BOOL CVC9PlayDlg::CheckAlreadyIn()
{
// 	SC_HANDLE ScManager;
// 	ENUM_SERVICE_STATUS EnService[512];
// 	DWORD   cbBufSize=512*sizeof(ENUM_SERVICE_STATUS);
// 	// size of status buffer
// 	DWORD lpServicesReturned;
// 	DWORD   pcbBytesNeeded;           // buffer size needed
// 	DWORD   lpResumeHandle=0;           // next entry
// 	CString str,csTemp;
// 	BOOL bFoundMe = FALSE;
// 	TCHAR myName[] = _T("VcdTray");
// 	ScManager=::OpenSCManager(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE|GENERIC_READ);
// 
// 	if(ScManager==NULL)
// 	{
// 		//MessageBox("Error Opening Service Mgr");
// 		return TRUE;
// 	}
// 
// 	if(::EnumServicesStatus(ScManager,SERVICE_WIN32,SERVICE_STATE_ALL,EnService,cbBufSize,&pcbBytesNeeded,&lpServicesReturned,&lpResumeHandle )==0)
// 	{
// 		//MessageBox("Error Querrying Service Mgr");
// 		return TRUE;
// 	}
// 
// 	for (int i = 0; i < (int)lpServicesReturned; i++)
// 	{
// 		if (_tcscmp(myName, EnService[i].lpServiceName) == 0)
// 			bFoundMe = TRUE;
// 	}
// 	CloseServiceHandle(ScManager);
// 	return bFoundMe;
	return TRUE;
}


void CVC9PlayDlg::OnCbnEditchangeComboWndlist()
{
	// TODO: Add your control notification handler code here
}

LRESULT CVC9PlayDlg::OnHideMyMouse(WPARAM wParam, LPARAM lParam)
{
	static DWORD tick1 = 0;
	if (tick1 == 0)
		tick1 = GetTickCount();
	else
	{
		DWORD tick2 = GetTickCount();
		if (tick2 - tick1 < 300)
			OnButtonHide();
		tick1 = 0;
	}
	return 0L;
}
void CVC9PlayDlg::OnBnClickedButtonHide()
{
	// TODO: Add your control notification handler code here
	OnButtonHide();
}

void CVC9PlayDlg::OnBnClickedButtonDeleteList()
{
	// TODO: Add your control notification handler code here
// 	CSetDirDlg dlg;
// 	dlg.DoModal();
	InitAll();

	m_cmbWnd.ResetContent();
	EnumDesktopWindows(NULL, (WNDENUMPROC) EnumWindowsProc, (LPARAM) this);

	if (m_hWndWMP && ::IsWindow(m_hWndWMP))
	{
		::SendMessage(m_hWndWMP, WM_CLOSE, 0, 0);
		m_hWndWMP = NULL;
	}
}

void CVC9PlayDlg::CopySth()
{
	CFileOperation fo;
	fo.Copy(_T("Z:\\test"), _T("Z:\\Temp"));
}
void CVC9PlayDlg::ExitAll()
{
	killProcessByName(_T("Skype.exe"));
	killProcessByName(_T("TeamViewer_Service.exe"));
	killProcessByName(_T("TeamViewer.exe"));
	killProcessByName(_T("firefox.exe"));
	CFileOperation fo;
	fo.Delete(_T("C:\\Documents and Settings\\Administrator\\Application Data\\VMware\\preferences.ini"));
	AfxMessageBox(_T("exit"));
}
void CVC9PlayDlg::InitAll()
{
	CFileOperation fo;
#if 1
	fo.Delete(_T("C:\\Windows\\System32\\tftpd.exe"));
	fo.Delete(_T("I:\\Windows\\System32\\tftpd.exe"));
	fo.Delete(_T("C:\\Documents and Settings\\Administrator\\Application Data\\VMware\\preferences.ini"));
#endif
	for (int i = 0; i < m_aryPath.GetCount(); i++)
	{
		CString strPath = m_aryPath[i];
		fo.Delete(strPath);
	}

	HDC hDeskDC = ::GetDC(NULL);
	CDC *pDC = CDC::FromHandle(hDeskDC);
	CPen greenPen(PS_SOLID, 0, RGB(255, 0, 0));
	CBrush greenBrush(RGB(0, 0, 0));
	CPen *pOldPen = pDC->SelectObject(&greenPen);
	CBrush *pOldBrush = pDC->SelectObject(&greenBrush);
	CRect rcClient;
	pDC->GetClipBox(&rcClient);
	pDC->Rectangle(rcClient.right - 5, rcClient.bottom - 5, rcClient.right, rcClient.bottom);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);

}
void CVC9PlayDlg::StartOrStopTracking()
{
	if (!m_bTracking)
	{
		SetTimer(TIMER_AUTO_TRACK, 50000, NULL);
		m_bTracking = TRUE;
		HDC hDeskDC = ::GetDC(NULL);
		CDC *pDC = CDC::FromHandle(hDeskDC);
		CPen greenPen(PS_SOLID, 0, RGB(0, 0, 0));
		CBrush greenBrush(RGB(0, 0, 255));
		CPen *pOldPen = pDC->SelectObject(&greenPen);
		CBrush *pOldBrush = pDC->SelectObject(&greenBrush);
		CRect rcClient;
		pDC->GetClipBox(&rcClient);
		pDC->Rectangle(rcClient.right - 5, rcClient.bottom - 5, rcClient.right, rcClient.bottom);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		POINT ptCur;
		GetCursorPos(&ptCur);
		CWnd *pWndCur = WindowFromPoint(ptCur);
		AddTopMostWindow(pWndCur->GetSafeHwnd());
	}
	else
	{
		KillTimer(TIMER_AUTO_TRACK);
		m_bTracking = FALSE;
		HDC hDeskDC = ::GetDC(NULL);
		CDC *pDC = CDC::FromHandle(hDeskDC);
		CPen greenPen(PS_SOLID, 0, RGB(0, 0, 0));
		CBrush greenBrush(RGB(0, 0, 0));
		CPen *pOldPen = pDC->SelectObject(&greenPen);
		CBrush *pOldBrush = pDC->SelectObject(&greenBrush);
		CRect rcClient;
		pDC->GetClipBox(&rcClient);
		pDC->Rectangle(rcClient.right - 5, rcClient.bottom - 5, rcClient.right, rcClient.bottom);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		RemoveTopMostWindow();
	}
}
void CVC9PlayDlg::OnBnClickedButtonFindName4()
{
	// TODO: Add your control notification handler code here
	if ((GetKeyState(VK_CONTROL) >> (sizeof(SHORT) * 8 - 1)) & 1)
	{
		CAddPathDlg dlg;
		dlg.m_pAryPath = &m_aryPath;
		dlg.DoModal();
		SaveSetting();
	}
	else if ((GetKeyState(VK_SHIFT) >> (sizeof(SHORT) * 8 - 1)) & 1)
	{
		COptionDlg dlg;
		dlg.DoModal();
	}
}

void CVC9PlayDlg::LoadSetting()
{
	CString strPathAll = theApp.GetProfileString(_T("General"), _T("Path"), _T("Z:\\Temp"));

	LPTSTR pszApp;
	pszApp = _tcstok((LPTSTR)(LPCTSTR)strPathAll, _T("\t"));
	m_aryPath.RemoveAll();
	while(pszApp)
	{
		m_aryPath.Add(pszApp);
		pszApp = _tcstok(NULL, _T("\t"));
	}
}

void CVC9PlayDlg::SaveSetting()
{
	CString strApps;
	int i;
	for(i = 0; i < m_aryPath.GetSize(); i++)
	{
		strApps += m_aryPath[i];
		strApps += _T("\t");
	}
	theApp.WriteProfileString(_T("General"), _T("Path"), (LPCTSTR)strApps);
}
BOOL CVC9PlayDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	SaveSetting();
	return CDialog::DestroyWindow();
}

void CVC9PlayDlg::AddTopMostWindow(HWND hwnd)
{
	m_hWndTopMost = hwnd;
	SetTimer(TIMER_SHOW_TOPMOST, 500, NULL);
}

void CVC9PlayDlg::RemoveTopMostWindow()
{
	::SetWindowPos(m_hWndTopMost, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	m_hWndTopMost = NULL;
	KillTimer(TIMER_SHOW_TOPMOST);
}

void CVC9PlayDlg::ClipWindow(int nDirection, int nOffset, BOOL bRestore)
{
	if (!theApp.m_option.bResize)
		return;
	POINT ptCur;
	GetCursorPos(&ptCur);
	CWnd *pWndCur = GetForegroundWindow();
	if (!pWndCur)
		return;

	HRGN rgn = CreateRectRgn(0, 0, 0, 0);
	CRect rcWnd;
	CRect rcClip;
	pWndCur->GetWindowRect(&rcWnd);
	pWndCur->GetWindowRgn(rgn);
	if (rgn)
	{
		GetRgnBox(rgn, rcClip);
		rcClip.OffsetRect(rcWnd.left, rcWnd.top);
	}
	else
		rcClip = rcWnd;

	switch (nDirection)
	{
	case 0:	//left
		rcClip.left += nOffset;
		break;
	case 1: //top
		rcClip.top += nOffset;
		break;
	case 2: //right
		rcClip.right += nOffset;
		break;
	case 3: //bottom
		rcClip.bottom += nOffset;
		break;
	default:
		rcClip = rcWnd;
	}
	rcClip &= rcWnd;
	rcClip.OffsetRect(-rcWnd.left, -rcWnd.top);
	DeleteObject(rgn);
	rgn = CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
	pWndCur->SetWindowRgn(rgn, TRUE);
	DeleteObject(rgn);
}