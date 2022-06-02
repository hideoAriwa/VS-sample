// SetDirDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VC9Play.h"
#include "SetDirDlg.h"


// CSetDirDlg dialog

IMPLEMENT_DYNAMIC(CSetDirDlg, CDialog)

CSetDirDlg::CSetDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDirDlg::IDD, pParent)
{

}

CSetDirDlg::~CSetDirDlg()
{
}

void CSetDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LIST, m_wndEditListBox);
}


BEGIN_MESSAGE_MAP(CSetDirDlg, CDialog)
END_MESSAGE_MAP()


// CSetDirDlg message handlers
