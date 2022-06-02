
// VC9Play.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CVC9PlayApp:
// See VC9Play.cpp for the implementation of this class
//
struct VC9_OPTION
{
	BOOL bTimeTrack;
	BOOL bResize;
	BOOL bSwitchDesktop;
	VC9_OPTION()
	{
		memset(this, 0, sizeof(VC9_OPTION));
	}
};
class CVC9PlayApp : public CWinAppEx
{
public:
	CVC9PlayApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
public:
	VC9_OPTION m_option;
	void LoadSetting();
	void SaveSetting();
};

extern CVC9PlayApp theApp;