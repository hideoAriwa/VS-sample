
// VC9Play.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VC9Play.h"
#include "VC9PlayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVC9PlayApp

BEGIN_MESSAGE_MAP(CVC9PlayApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVC9PlayApp construction

CVC9PlayApp::CVC9PlayApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CVC9PlayApp object

CVC9PlayApp theApp;


// CVC9PlayApp initialization

BOOL CVC9PlayApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("VCPlayer"));
	LoadSetting();
	CVC9PlayDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CVC9PlayApp::LoadSetting()
{
	m_option.bTimeTrack = GetProfileInt(_T("Option"), _T("TimeTrack"), 1);
	m_option.bResize	= GetProfileInt(_T("Option"), _T("Resize"), 1);
	m_option.bSwitchDesktop = GetProfileInt(_T("Option"), _T("SwitchDesktop"), 1);
}

void CVC9PlayApp::SaveSetting()
{
	WriteProfileInt(_T("Option"), _T("TimeTrack"), m_option.bTimeTrack);
	WriteProfileInt(_T("Option"), _T("Resize"), m_option.bResize);
	WriteProfileInt(_T("Option"), _T("SwitchDesktop"), m_option.bSwitchDesktop);
}