//////////////////////////////////////////////////////////////////////
//																	//
//	SystemManager.h@: interface of the CSystemManager class		//
//						Writed by J.J.W.							//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SYSTEMMANAGER_H__22041C0D_2C9D_4565_96C3_A3AF92A1CBA0__INCLUDED_)
#define AFX_SYSTEMMANAGER_H__22041C0D_2C9D_4565_96C3_A3AF92A1CBA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define GetRegesteredPathName GetRegisteredPathName
#define GetRegisteredPathName GetRegisteredPath
class  __declspec( dllexport ) CSystemManager  
{
public:
	static BOOL SetPrivilege(LPCTSTR Privilege, BOOL bEnablePrivilege);
	CSystemManager();
	virtual ~CSystemManager();
	static BOOL RunApplication(LPCTSTR szAppPath,LPCTSTR szParameters = NULL,WORD wShowMode = SW_SHOW);
	static BOOL GetRegisteredPath(LPCTSTR szRegisteredName,LPTSTR szAppPath,int nPathLength);
	static BOOL GetApplicationPath(LPCTSTR szAppName,LPTSTR szAppPath,LONG lSize);
	static int GetProcessCount(LPCTSTR lpszProcName = NULL,BOOL bTerminateOthers = FALSE);
	static BOOL GetWindowModuleName(HWND hWnd, TCHAR* szModuleName, int cchName);
	
	static BOOL BrowseRegistry(HKEY hKey, const TCHAR* szKeyFullName, const TCHAR* szSubKeyName, LPVOID lpParam = NULL, REGSAM samDesired = KEY_ALL_ACCESS,
					BOOL(*pfRegValueOperator)(HKEY hKey, const TCHAR* szKeyFullName,const TCHAR* szValueName,DWORD dwType,BYTE* szValueData,DWORD dwSize, LPVOID lpParam) = NULL,
					BOOL(*pfRegKeyPreOperator)(HKEY hKey, const TCHAR* szKeyFullName, const TCHAR* szSubKeyName, LPVOID lpParam) = NULL,
					BOOL(*pfRegKeyPostOperator)(HKEY hKey, const TCHAR* szKeyFullName, const TCHAR* szSubKeyName, LPVOID lpParam) = NULL
					);
	static BOOL DeleteRegistry(HKEY hKey, LPCTSTR szKeyFullName, LPCTSTR szSubKeyName);

	static int SearchMemoryLeak(const TCHAR* szReportPath, const TCHAR* szLeakReportPath,void(*pfProgressProc)(float fProgressRate) = NULL,int nKind = 0);

};

#endif // !defined(AFX_SYSTEMMANAGER_H__22041C0D_2C9D_4565_96C3_A3AF92A1CBA0__INCLUDED_)
