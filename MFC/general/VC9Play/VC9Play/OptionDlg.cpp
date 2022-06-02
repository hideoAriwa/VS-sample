// AddPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VC9Play.h"
#include "OptionDlg.h"
#include "FileManager.h"

// COptionDlg dialog

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
{
	m_bTimeTrack = theApp.m_option.bTimeTrack;
	m_bResize = theApp.m_option.bResize;
	m_bSwitchDesktop = theApp.m_option.bSwitchDesktop;
}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Check(pDX, IDC_CHECK_TIME_TRACK, m_bTimeTrack);
	DDX_Check(pDX, IDC_CHECK_RESIZE, m_bResize);
	DDX_Check(pDX, IDC_CHECK_SWITCH_DESKTOP, m_bSwitchDesktop);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COptionDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDCANCEL, &COptionDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COptionDlg message handlers

BOOL COptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionDlg::OnBnClickedOK()
{
	UpdateData(TRUE);

	theApp.m_option.bTimeTrack = m_bTimeTrack;
	theApp.m_option.bResize = m_bResize;
	theApp.m_option.bSwitchDesktop = m_bSwitchDesktop;

	theApp.SaveSetting();
	CDialog::OnOK();
}

void COptionDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}