#pragma once
#include "afxdtctl.h"


// CSetTimeDialog dialog

class CSetTimeDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetTimeDialog)

public:
	CSetTimeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetTimeDialog();

// Dialog Data
	enum { IDD = IDD_SET_TIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	BOOL m_bShutdown;
	CTime m_timeStart;
	CTime m_timeEnd;
};
