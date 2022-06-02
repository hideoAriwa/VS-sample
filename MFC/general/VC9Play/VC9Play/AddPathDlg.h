#pragma once
#include "afxwin.h"


// CAddPathDlg dialog

class CAddPathDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddPathDlg)

public:
	CAddPathDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddPathDlg();

// Dialog Data
	enum { IDD = IDD_ADD_PATH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowse();
	CString m_strFolderPath;
	afx_msg void OnBnClickedButtonAdd();
	CStringArray* m_pAryPath;
	CListBox m_lstPath;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDelete();
};
