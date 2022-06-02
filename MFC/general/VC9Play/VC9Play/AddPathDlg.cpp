// AddPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VC9Play.h"
#include "AddPathDlg.h"
#include "FileManager.h"

// CAddPathDlg dialog

IMPLEMENT_DYNAMIC(CAddPathDlg, CDialog)

CAddPathDlg::CAddPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddPathDlg::IDD, pParent)
	, m_strFolderPath(_T(""))
{

}

CAddPathDlg::~CAddPathDlg()
{
}

void CAddPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strFolderPath);
	DDX_Control(pDX, IDC_LIST_PATH, m_lstPath);
}


BEGIN_MESSAGE_MAP(CAddPathDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CAddPathDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CAddPathDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CAddPathDlg::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CAddPathDlg message handlers

void CAddPathDlg::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here
	TCHAR szFolderPath[MAX_PATH];
	if(!CFileManager::GetDirectoryPath(GetSafeHwnd(), _T("Select Directory"), szFolderPath, MAX_PATH))
	{
		return;
	}

	m_strFolderPath = szFolderPath;
	UpdateData(FALSE);
}

void CAddPathDlg::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	m_pAryPath->Add(m_strFolderPath);
	m_lstPath.AddString(m_strFolderPath);
}

BOOL CAddPathDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	for (int i = 0; i < m_pAryPath->GetCount(); i++)
	{
		m_lstPath.AddString(m_pAryPath->GetAt(i));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAddPathDlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	m_pAryPath->RemoveAt(m_lstPath.GetCurSel());
	m_lstPath.DeleteString(m_lstPath.GetCurSel());
}
