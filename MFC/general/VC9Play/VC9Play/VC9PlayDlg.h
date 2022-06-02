
// VC9PlayDlg.h : header file
//

#pragma once

#include <afxtempl.h>

#define WM_LOCKCOMPUTER	WM_USER + 101
#define WM_MY_MESSAGE WM_USER + 100
#define WM_HIDE_BY_MOUSE	WM_USER + 102
#define WM_SWITCH_DESKTOP	WM_USER + 103
#define WM_WND_INACTIVE WM_USER + 8000
#define MY_TIMER	100

#define TIMER_HIDE_ME		0x0116
#define TIMER_LOCK_COMPUTER	0x1990
#define TIMER_PING_HTTPREQUEST 0x1991

//#define LOCK_COMPUTER_AT_STARTUP
// CVC9PlayDlg dialog
class CVC9PlayDlg : public CDialog
{
// Construction
public:
	CVC9PlayDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VC9PLAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HWND m_hWndWMP;
	HWND m_hWndTopMost;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPicFindtool();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeListWnd();
	afx_msg void OnButtonShow();
	afx_msg void OnButtonHide();
	virtual void OnCancel();
	afx_msg void OnCheckKeyhooklog();
	afx_msg void OnButtonAddWindow();
	afx_msg void OnButtonFindName();
	afx_msg void OnDblclkListWnd();
	afx_msg void OnSetfocusComboWndlist();
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSwitchDesktop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHideMyMouse(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWndInactive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLockComputer(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	void InitAll();
	void ExitAll();
	void CopySth();
	void SnapShot();
	void AddToCombo(HWND hWnd);
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
public:
	CComboBox	m_cmbWnd;
	CListBox	m_wndList;
	CStatic	m_wndPicCanvas;
	HCURSOR m_hCursor;
	HWND m_wndPrev;
	BOOL	m_bLogKeyEvent;
protected:
	void AddTopMostWindow(HWND hwnd);
	void RemoveTopMostWindow();
	BOOL CheckAlreadyIn();
	BOOL RegisterService();
	CRect m_defPos;
	void ShowWindowByNum(int nVKCodeNum);
	BOOL AddWindow(HWND hWnd);
	void TerminateSelWnd();
	void TerminateAll();
	void DrawWndBorder(HWND hWnd);
	HWND GetCurSelWnd();
	int CheckRepeat(HWND hWnd);
	void StartOrStopTracking();
	void ClipWindow(int nDirection, int nOffset, BOOL bRestore);
	void LoadSetting();
	void SaveSetting();
protected:
	CStringArray m_aryPath;
	BOOL m_bTracking;
	int m_curSel;
	CRect m_prevRect;
	BOOL m_bContinue;
	BOOL m_bPause;
public:
	CString m_csNewWndName;
	afx_msg void OnCbnEditchangeComboWndlist();
	afx_msg void OnBnClickedButtonHide();
	afx_msg void OnBnClickedButtonDeleteList();
	afx_msg void OnBnClickedButtonFindName4();
	virtual BOOL DestroyWindow();
};

struct THREADPARAM	
{
	BOOL *bContinue;
	BOOL *pPause;
	CListBox *pWndList;
};