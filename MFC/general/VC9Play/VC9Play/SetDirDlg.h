#pragma once
#include "afxwin.h"


// CSetDirDlg dialog

class CCustomEditListBox : public CVSListBox
{
	virtual void OnBrowse()
	{
		int nSel = GetSelItem();

		MessageBox(_T("Browse item..."));

		if (nSel == GetCount()) // New item
		{
			nSel = AddItem(_T("New text"));
			SelectItem(nSel);
		}
		else
		{
			SetItemText(nSel, _T("Updated text"));
		}
	}
};

class CSetDirDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetDirDlg)

public:
	CSetDirDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetDirDlg();

// Dialog Data
	enum { IDD = IDD_SET_DIR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	CCustomEditListBox m_wndEditListBox;
};

