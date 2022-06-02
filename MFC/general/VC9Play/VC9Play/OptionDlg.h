#pragma once
#include "afxwin.h"


// COptionDlg dialog

class COptionDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL m_bTimeTrack;
	BOOL m_bResize;
	BOOL m_bSwitchDesktop;
	void OnBnClickedOK();
	void OnBnClickedCancel();
};