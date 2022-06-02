// SetTimeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VC9Play.h"
#include "SetTimeDialog.h"


// CSetTimeDialog dialog

IMPLEMENT_DYNAMIC(CSetTimeDialog, CDialog)

CSetTimeDialog::CSetTimeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetTimeDialog::IDD, pParent)
	, m_bShutdown(FALSE)
	, m_timeStart(0)
	, m_timeEnd(0)
{

}

CSetTimeDialog::~CSetTimeDialog()
{
}

void CSetTimeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_SHUTDOWN, m_bShutdown);
	DDX_DateTimeCtrl(pDX, IDC_TIME_START, m_timeStart);
	DDX_DateTimeCtrl(pDX, IDC_TIME_END, m_timeEnd);
}


BEGIN_MESSAGE_MAP(CSetTimeDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CSetTimeDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetTimeDialog message handlers

void CSetTimeDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}
