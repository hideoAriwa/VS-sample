//////////////////////////////////////////////////////////////////////
//																	//
//	SystemManager.cpp : implementation of the CSystemManager class	//
//						Writed by J.J.W.							//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <AfxConv.h>
#include <TLHELP32.H>
#include "psapi.h"
#include "Share.h"
#include "SystemManager.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemManager::CSystemManager()
{

}

CSystemManager::~CSystemManager()
{

}

BOOL CSystemManager::RunApplication(LPCTSTR szAppPath,LPCTSTR szParameters,WORD wShowMode)
{
	TCHAR szCommandLine[512];
	TCHAR szCurFolderPath[512];
	szCurFolderPath[0] = szCommandLine[0] = _T('\0');
//	lstrcpy(szCommandLine,szAppPath);
	if(szParameters)
	{
		lstrcat(szCommandLine,_T(" "));
		if(szParameters[0] != _T('\"'))
		{
			lstrcat(szCommandLine,_T("\""));
		}
		lstrcat(szCommandLine,szParameters);
		if(szParameters[_tcslen(szParameters)-1] != _T('\"'))
		{
			lstrcat(szCommandLine,_T("\""));
		}
	}

	if(_tcsrchr(szAppPath,_T('\\')))
	{
		lstrcpy(szCurFolderPath,szAppPath);
		*_tcsrchr(szCurFolderPath,_T('\\')) = _T('\0');
	}
	else
	{
		GetCurrentDirectory(sizeof(szCurFolderPath),szCurFolderPath);
	}
	if( !szCommandLine && !_tcslen(szCommandLine) )
		return FALSE;
	TCHAR szBuf[256];
	szBuf[255] = 0;
	
	STARTUPINFO StartupInfo; 
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	StartupInfo.wShowWindow = wShowMode;
	PROCESS_INFORMATION ProcessInformation; // ÉvÉçÉZÉXèÓïÒ
	if( !::CreateProcess(szAppPath,(LPTSTR)szCommandLine,NULL,NULL,
		FALSE,CREATE_NEW_PROCESS_GROUP |NORMAL_PRIORITY_CLASS,NULL,
		szCurFolderPath,&StartupInfo,&ProcessInformation ) )
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CSystemManager::GetRegisteredPath(LPCTSTR szRegisteredName,LPTSTR szAppPath,int nPathLength)
{
	HKEY    hkCLSID ,hk;
    CLSID   clsid; 
	USES_CONVERSION;
	BOOL bSuccess = FALSE;
	*szAppPath = '\0';
	hkCLSID = hk = NULL;

    if (RegOpenKey(HKEY_CLASSES_ROOT,_T("CLSID"),&hkCLSID) == ERROR_SUCCESS)
	{
        LONG     cb ;
        //TCHAR    szBuf[MAX_PATH] ;
        TCHAR    szName[128] ;
        TCHAR    szCLSID[128] ;
        DWORD    dwIndex ;
		int  const nMAX_VALLEN = MAX_PATH + 256 ;
		//TCHAR    szKey[nMAX_VALLEN] ;
        for ( dwIndex = 0 ;RegEnumKey( hkCLSID, dwIndex, szCLSID, sizeof(szCLSID)) == ERROR_SUCCESS ;++dwIndex )
        {
            cb = sizeof(szName);
            if (RegQueryValue( hkCLSID, szCLSID, szName , &cb) == ERROR_SUCCESS)
            {
				CString csName(szName);
				if(-1 != csName.Find(szRegisteredName)){
					::CLSIDFromString( T2OLE(szCLSID), &clsid);
					{
						int  const nMAX_VALLEN = MAX_PATH + 256 ;
						TCHAR  szKey[nMAX_VALLEN] ;

						if(!IsEqualCLSID(clsid,CLSID_NULL))
						{
							OLECHAR szOleCLSID[64];
							::StringFromGUID2(clsid, szOleCLSID, 
								sizeof(szOleCLSID)/sizeof(OLECHAR) ) ;
							LPTSTR lpszCLSID = OLE2T(szOleCLSID);
							wsprintf( szKey, _T("CLSID\\%s"), lpszCLSID ) ;                           

							
							if (RegOpenKey( HKEY_CLASSES_ROOT, szKey, &hk) == ERROR_SUCCESS) 
							{
								int  const nMAX_VALLEN = MAX_PATH + 256 ;
								TCHAR    szValue[nMAX_VALLEN] ;
        						LONG cb = sizeof( szValue );
								if (RegQueryValue(hk,(LPCTSTR)(_T("LocalServer32")), szValue, &cb) == ERROR_SUCCESS)
								{
									CString csTemp = CString(szValue);
					/*				int nIndex = csTemp.Find(' ');
									if(nIndex != -1)
										csTemp = csTemp.Left(nIndex);
									nIndex = csTemp.Find('\"');
									while(nIndex != -1){
										CString csA = csTemp.Left(nIndex);
										CString csB = csTemp.Right(csTemp.GetLength() - nIndex -1);
										csTemp = csA+csB;
										nIndex = csTemp.Find('\"');
									}
					*/				
									// Check Special Chars "\"";
									while(true){
										int nIndex = csTemp.Find(_T("\""));
										if(nIndex < 0)
											break;
										csTemp = csTemp.Left(nIndex) + csTemp.Right(csTemp.GetLength() - nIndex - 1);
									}
									int nPos = csTemp.ReverseFind('/');
									if( nPos > -1 ){
										csTemp = csTemp.Left( nPos );
										while( !csTemp.Right(1).CompareNoCase(_T(" ")) )
										{
											csTemp.Delete(csTemp.GetLength()-1,1);
										}
									}
									CFileStatus rSt;
									if(!CFile::GetStatus(csTemp,rSt))
									{
//										bSuccess = false;
//										break;
									}
									if(csTemp.GetLength()<nPathLength)
									{
										lstrcpy(szAppPath,(LPCTSTR)csTemp);
										bSuccess = true;
										break;
									}
									else
									{
										bSuccess = false;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if(hkCLSID){
		RegCloseKey(hkCLSID);
	}
	if(hk){
		RegCloseKey(hk);
	}

	return bSuccess;
}

BOOL CSystemManager::GetApplicationPath(LPCTSTR szAppName,LPTSTR szAppPath,LONG lSize)
{
	CString strSubKeyName;
	strSubKeyName = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\");
	strSubKeyName += szAppName;
	
	if(RegQueryValue(HKEY_LOCAL_MACHINE,strSubKeyName,szAppPath,&lSize) == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CSystemManager::GetProcessCount(LPCTSTR lpszProcName,BOOL bTerminateOthers)
{
	HANDLE hSnapshot;
	DWORD dwFlags = TH32CS_SNAPPROCESS;
	DWORD th32ProcessID = 0;
	int nCount = 0;
	TCHAR szProcName[512];
	hSnapshot = CreateToolhelp32Snapshot(dwFlags,th32ProcessID);
	if( (int)hSnapshot == -1 )
		return 0;
	LPPROCESSENTRY32 lppe = new PROCESSENTRY32;
	lppe->dwSize = sizeof(PROCESSENTRY32);
	if( !Process32First(hSnapshot,lppe) )
		return 0;
	DWORD dwCurProcessID = GetCurrentProcessId();
	if(lpszProcName == NULL)
	{
		GetModuleFileName(NULL,szProcName,sizeof(szProcName));
		TCHAR* ptr = _tcsrchr(szProcName,_T('\\'));
		if(ptr)
		{
			memmove(szProcName,ptr+1,sizeof(TCHAR)*(_tcslen(ptr+1)+1));
		}
	}
	else
	{
		_tcsncpy(szProcName,lpszProcName,sizeof(szProcName)/sizeof(TCHAR)-1);
	}
	if(_tcsrchr(szProcName,_T('\\')))
	{
		memmove(szProcName,_tcsrchr(szProcName,_T('\\'))+1,szProcName+_tcslen(szProcName)-_tcsrchr(szProcName,_T('\\')));
	}
	
	while(Process32Next(hSnapshot,lppe))
	{
		CString csPath(lppe->szExeFile);
		int nLastSlashPos = csPath.ReverseFind('\\');
		if(nLastSlashPos>=0 && nLastSlashPos<csPath.GetLength())
		{
			csPath = csPath.Right(csPath.GetLength()-nLastSlashPos-1);
		}
		
		if( !csPath.CompareNoCase((LPCTSTR)szProcName) )
		{
			nCount++;
			if(lppe->th32ProcessID==dwCurProcessID)
			{
				continue;
			}
			if( bTerminateOthers )
			{
				DWORD dwDesiredAccess = PROCESS_TERMINATE | PROCESS_ALL_ACCESS |
					PROCESS_QUERY_INFORMATION ;//STANDARD_RIGHTS_REQUIRED;
				BOOL bInheritHandle = FALSE;
				HANDLE hProcess = OpenProcess(dwDesiredAccess,bInheritHandle,lppe->th32ProcessID);
				
				if( hProcess )
				{
					DWORD dwExitCode;
					dwExitCode = 0;
					if( !GetExitCodeProcess(hProcess,&dwExitCode) )
					{
						CloseHandle(hProcess);
						break ;
					}
					UINT uExitCode = (UINT)dwExitCode;
					if( !TerminateProcess(hProcess,uExitCode) )
					{
						CloseHandle(hProcess);
						break ;
					}
				}
			}
		}
	}
	delete lppe;
	CloseHandle(hSnapshot);
	return nCount;
}

BOOL CSystemManager::SetPrivilege(LPCTSTR Privilege, BOOL bEnablePrivilege)
{
	HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    TOKEN_PRIVILEGES tpPrevious;
    DWORD cbPrevious=sizeof(TOKEN_PRIVILEGES);
    BOOL bSuccess=FALSE;

    if(!LookupPrivilegeValue(NULL, Privilege, &luid)) return FALSE;

    if(!OpenProcessToken(
            GetCurrentProcess(),
            TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
            &hToken
            )) return FALSE;

    //
    // first pass.  get current privilege setting
    //
    tp.PrivilegeCount           = 1;
    tp.Privileges[0].Luid       = luid;
    tp.Privileges[0].Attributes = 0;

    AdjustTokenPrivileges(
            hToken,
            FALSE,
            &tp,
            sizeof(TOKEN_PRIVILEGES),
            &tpPrevious,
            &cbPrevious
            );

    if(GetLastError() == ERROR_SUCCESS) {
        //
        // second pass.  set privilege based on previous setting
        //
        tpPrevious.PrivilegeCount     = 1;
        tpPrevious.Privileges[0].Luid = luid;

        if(bEnablePrivilege) {
            tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
        }
        else {
            tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
                tpPrevious.Privileges[0].Attributes);
        }

        AdjustTokenPrivileges(
                hToken,
                FALSE,
                &tpPrevious,
                cbPrevious,
                NULL,
                NULL
                );

        if(GetLastError() == ERROR_SUCCESS) bSuccess=TRUE;
    }

    CloseHandle(hToken);

    return bSuccess;
}

BOOL CSystemManager::BrowseRegistry(HKEY hKey, const TCHAR* szKeyFullName, const TCHAR* szSubKeyName, LPVOID lpParam, REGSAM samDesired,
					BOOL(*pfRegValueOperator)(HKEY hKey, const TCHAR* szKeyFullName,const TCHAR* szValueName,DWORD dwType,BYTE* szValueData,DWORD dwSize, LPVOID lpParam),
					BOOL(*pfRegKeyPreOperator)(HKEY hKey, const TCHAR* szKeyFullName, const TCHAR* szSubKeyName, LPVOID lpParam),
					BOOL(*pfRegKeyPostOperator)(HKEY hKey, const TCHAR* szKeyFullName, const TCHAR* szSubKeyName, LPVOID lpParam))
{
	if(!hKey) return FALSE;

	if(pfRegKeyPreOperator)
	{
		if(!pfRegKeyPreOperator(hKey, szKeyFullName, szSubKeyName, lpParam))
		{
			return FALSE;
		}
	}

	HKEY hSubKey = NULL;

	if(szSubKeyName == NULL || !_tcslen(szSubKeyName))
	{
		hSubKey = hKey;
	}
	else
	{
		if(RegOpenKeyEx(hKey,szSubKeyName,0,samDesired,&hSubKey) != ERROR_SUCCESS)
		{
			return FALSE ;
		}
	}
	
	LONG lResult;
	DWORD dwIndex;
	TCHAR szSubKeyEnumName[MAX_PATH], szClass[MAX_PATH];
	DWORD dwClassSize, dwKeySize;
	dwClassSize = sizeof(szSubKeyEnumName);
	dwKeySize = sizeof(szClass);

	TCHAR* pszSubKeyFullName;
	pszSubKeyFullName = new TCHAR[_tcslen(szKeyFullName) + MAX_PATH];
	_tcscpy(pszSubKeyFullName, szKeyFullName);
	if(pszSubKeyFullName[_tcslen(pszSubKeyFullName) - 1] != '\\')
	{
		pszSubKeyFullName[_tcslen(pszSubKeyFullName) + 1] = '\0';
		pszSubKeyFullName[_tcslen(pszSubKeyFullName) + 0] = '\\';
	}

	_tcscat(pszSubKeyFullName, szSubKeyName);


	
	//////////////////////////////Scan Value//////////////////////////////
	BYTE buf[512];
	DWORD dwType;
	DWORD dwSize; 
	buf[0] = 0;
	dwType = REG_SZ;

	dwSize = sizeof(buf);

	TCHAR szValueName[512];
	DWORD dwValueSize;
	for ( dwIndex = 0 ;(dwValueSize = sizeof(szValueName),dwSize = sizeof(buf),
						lResult = RegEnumValue( hSubKey, dwIndex, szValueName, &dwValueSize,NULL,&dwType,buf,&dwSize)) == ERROR_SUCCESS ;
		++dwIndex )
	{
		if(pfRegValueOperator)
		{
			pfRegValueOperator(hSubKey,pszSubKeyFullName,szValueName,dwType,buf,dwSize,lpParam);
		}
	}

	//////////////////////////////Scan Key//////////////////////////////
	for ( dwIndex = 0 ;(dwClassSize = sizeof(szSubKeyEnumName),dwKeySize = sizeof(szClass),
						lResult = RegEnumKeyEx( hSubKey, dwIndex, szSubKeyEnumName, &dwKeySize,NULL,szClass,&dwClassSize,NULL)) == ERROR_SUCCESS ;
		++dwIndex )
	{
		BrowseRegistry(hSubKey,pszSubKeyFullName,szSubKeyEnumName,lpParam,samDesired,pfRegValueOperator,pfRegKeyPreOperator,pfRegKeyPostOperator);
	}

	if(hKey == hSubKey)
	{
		RegCloseKey(hKey);
	}

	if(pfRegKeyPostOperator)
	{
		if(!pfRegKeyPostOperator(hKey, szKeyFullName, szSubKeyName, lpParam))
		{
			return FALSE;
		}
	}

	if(pszSubKeyFullName){
		delete pszSubKeyFullName;
	}

	return TRUE;
}

typedef struct tagREGINFO {
	BOOL bKey;
	CString strRegName ;
} REGINFO;

BOOL CollectRegValueForDelReg(HKEY hKey, 
							  const TCHAR* szKeyFullName, 
							  const TCHAR* szValueName, 
							  DWORD dwType, 
							  BYTE* szValueData, 
							  DWORD dwSize,
							  LPVOID lpParam)
{
	REGINFO ri;
	ri.bKey = FALSE;
	ri.strRegName  = szKeyFullName;
	ri.strRegName += _T('\\');
	ri.strRegName += szValueName;

	((CArray<REGINFO, REGINFO&>*)lpParam)->Add(ri);

	return TRUE;
}

BOOL CollectRegKeyForDelReg(HKEY hKey, 
							const TCHAR* szKeyFullName, 
							const TCHAR* 
							szSubKeyName, 
							LPVOID lpParam)
{
	REGINFO ri;
	ri.bKey = TRUE;
	ri.strRegName = szKeyFullName;
	ri.strRegName += _T('\\');
	ri.strRegName += szSubKeyName;
	((CArray<REGINFO, REGINFO&>*)lpParam)->Add(ri);

	return TRUE;
}

BOOL CSystemManager::DeleteRegistry(HKEY hKey, LPCTSTR szKeyFullName, LPCTSTR szSubKeyName)
{

	CArray<REGINFO, REGINFO&> aryKeyInfo;
	BrowseRegistry(
		hKey, 
		szKeyFullName, 
		szSubKeyName, 
		(LPVOID)&aryKeyInfo,
		KEY_ALL_ACCESS,
		CollectRegValueForDelReg,
		NULL,
		CollectRegKeyForDelReg
		);

	TCHAR* pszRemoveKeyName;
	pszRemoveKeyName = new TCHAR[_tcslen(szKeyFullName) + 1 + _tcslen(szSubKeyName) + 1];
	_tcscpy(pszRemoveKeyName, szKeyFullName);
	if(pszRemoveKeyName[_tcslen(pszRemoveKeyName) - 1] != _T('\\'))
	{
		pszRemoveKeyName[_tcslen(pszRemoveKeyName) + 1] = _T('\0');
		pszRemoveKeyName[_tcslen(pszRemoveKeyName) + 0] = _T('\\');
	}
	_tcscat(pszRemoveKeyName, szSubKeyName);

	int i;
	for(i = 0; i < aryKeyInfo.GetSize(); i++)
	{
		TCHAR* pszRegName;
		pszRegName = new TCHAR[aryKeyInfo[i].strRegName.GetLength() + 1];

		_tcscpy(pszRegName, aryKeyInfo[i].strRegName);
		if(_tcsnicmp(pszRemoveKeyName, pszRegName, _tcslen(pszRemoveKeyName))){
			delete[] pszRemoveKeyName;
			delete[] pszRegName;
			return FALSE;
		}

		TCHAR* pszRegInfoToken = _tcstok(pszRegName, _T("\\"));
		HKEY hMainKey;
		if(_tcscmp(pszRegInfoToken, _T("HKEY_CLASSES_ROOT")) == 0)
		{
			hMainKey = HKEY_CLASSES_ROOT;
		}
		else if(_tcscmp(pszRegInfoToken, _T("HKEY_CURRENT_USER")) == 0)
		{
			hMainKey = HKEY_CURRENT_USER;
		}
		else if(_tcscmp(pszRegInfoToken, _T("HKEY_LOCAL_MACHINE")) == 0)
		{
			hMainKey = HKEY_LOCAL_MACHINE;
		}
		else if(_tcscmp(pszRegInfoToken, _T("HKEY_USERS")) == 0)
		{
			hMainKey = HKEY_USERS;
		}
		else if(_tcscmp(pszRegInfoToken, _T("HKEY_CURRENT_CONFIG")) == 0)
		{
			hMainKey = HKEY_CURRENT_CONFIG;
		}
		
		pszRegInfoToken = _tcstok(NULL, _T("\0"));

		if(aryKeyInfo[i].bKey){
			RegDeleteKey(hMainKey, pszRegInfoToken);
		}
		else{
			RegDeleteValue(hMainKey, pszRegInfoToken);
		}
		delete[] pszRegName;
	}

	delete[] pszRemoveKeyName;
	return TRUE;
}

int CSystemManager::SearchMemoryLeak(const TCHAR* szReportPath, const TCHAR* szLeakReportPath,void(*pfProgressProc)(float fProgressRate),int nKind)
{
	if(nKind != 0)
	{
		return -1;
	}

	{
		#define ALLOC_MARK	_T("Alloc : ")
		#define FREE_MARK	_T("Free : ")
		CStdioFile file;
		if(!file.Open(szReportPath,CFile::modeRead))
		{
			return -1;
		}
		CString strLine;
		CStringList listMultiAllocFree;
		
		CMapPtrToPtr ptr2ptr;
		CString strWork;
		DWORD dwAddress;
		
		int nTotalCount = 0;
		while(file.ReadString(strLine))
		{
			nTotalCount++;
		}
		file.Seek(0, CFile::begin);
		int nProgressedCount = 0;
		
		while(file.ReadString(strLine))
		{
			if(strLine.Left(_tcslen(ALLOC_MARK)) == ALLOC_MARK)
			{
				strWork = strLine.Right(strLine.GetLength() - _tcslen(ALLOC_MARK));
				strWork = strWork.Left(strWork.Find(' '));

				_stscanf(strWork, _T("%x"), &dwAddress);
				void* pszPath;
				pszPath = (void*)new TCHAR[256];
				_tcscpy((TCHAR*)pszPath, strLine);

//{{AFX
#if 1
				if(ptr2ptr.Lookup((void*)dwAddress, pszPath))
				{
					listMultiAllocFree.AddTail(strLine);
				}
#endif
//}}AFX

				ptr2ptr.SetAt((void*)dwAddress, pszPath);

			}
			else if(strLine.Left(_tcslen(FREE_MARK)) == FREE_MARK)
			{
				strWork = strLine.Right(strLine.GetLength() - _tcslen(FREE_MARK));
				strWork = strWork.Left(strWork.Find(_T(' ')));

				void* pszPath;
				_stscanf(strWork, _T("%x"), &dwAddress);
				if(ptr2ptr.Lookup((void*)dwAddress, pszPath))
				{
					if(pszPath)
					{
						ptr2ptr.RemoveKey((void*)dwAddress);
						delete pszPath;
					}
				}
				else
				{
					listMultiAllocFree.AddTail(strLine);
				}

			}
			if(pfProgressProc)
			{
				pfProgressProc(float((float)nProgressedCount++/nTotalCount)*2);
			}

		}

		file.Close();
		file.Open(szLeakReportPath,CFile::modeCreate | CFile::modeWrite);
		
		int nCount = 0;

		POSITION pos;
		pos = ptr2ptr.GetStartPosition();
		int nAllUnFreedSize = 0;
#define LEAK_REPORT_SIZE_MARK	_T("size:")
		while(pos)
		{
			void* pszPath;
			void* address;
			ptr2ptr.GetNextAssoc(pos, address, pszPath);
			TCHAR szCrLf[] = {0x0D, 0x0A, 0};
			//////////////////Collection of unfreed memory size//////////////////
			if(pszPath)
			{
				file.Write((LPCTSTR)pszPath, strlen((char*)pszPath));

				TCHAR* pszPosStart, *pszPosEnd, *pszLineEnd;
				pszPosStart = (LPTSTR)_tcsstr((LPTSTR)(LPCTSTR)pszPath, (LPCTSTR)LEAK_REPORT_SIZE_MARK);
				pszLineEnd = (LPTSTR)pszPath + _tcslen((LPTSTR)pszPath);

				if(pszPosStart)
				{
					pszPosStart += _tcslen(LEAK_REPORT_SIZE_MARK);
					
					while(pszPosStart < pszLineEnd)
					{
						if(*pszPosStart>_T('9') || *pszPosStart<_T('0'))
						{
							pszPosStart++;
							continue;
						}
						break;
					}

					if(pszPosStart < pszLineEnd)
					{
						pszPosEnd = pszPosStart + 1;
						while(pszPosEnd < pszLineEnd)
						{
							if(*pszPosEnd<=_T('9') && *pszPosEnd>=_T('0'))
							{
								pszPosEnd++;
								continue;
							}
							break;
						}
						*pszPosEnd = _T('\0');
						int nUnfreedSize = 0;
						_stscanf(pszPosStart, _T("%d"), &nUnfreedSize);
						nAllUnFreedSize += nUnfreedSize;
					}
				}
				///////////////////////////////////////////////////////////////////

				delete pszPath;
				file.Write(szCrLf, _tcslen(szCrLf));
				nCount++;
			}
		}

		pos = listMultiAllocFree.GetHeadPosition();
		while(pos)
		{
			CString strFreed = listMultiAllocFree.GetNext(pos);
			TCHAR szCrLf[] = {0x0D, 0x0A, 0};
			file.Write(strFreed, strFreed.GetLength());
			file.Write(szCrLf, _tcslen(szCrLf));
			nCount++;
		}

		CString strReportUnFreedSize;
		strReportUnFreedSize.Format(_T("Total unfreed size : %d bytes(%.3f MBytes)"), nAllUnFreedSize, float(nAllUnFreedSize/(1024.0*1024)));
		file.Write(strReportUnFreedSize, strReportUnFreedSize.GetLength());
		
		file.Close();
		return nCount;
	}

	//{{AFX
	#if 0

	#define ALLOC_MARK	_T("Alloc")
	#define FREE_MARK	_T("Free")
	CStdioFile file;
	if(!file.Open(szReportPath,CFile::modeRead))
	{
		return -1;
	}
	CString strLine;
	CStringList listString;
	while(file.ReadString(strLine))
	{
		listString.AddTail(strLine);
	}

	POSITION pos,pos_1,pos_2;
	pos = listString.GetHeadPosition();

	int nOffset;
	
	int nProgressedCount = 0;
	int nTotalCount;
	nTotalCount = listString.GetCount();
	
	while(pos)
	{
		pos_1 = pos;
		if(!listString.GetCount())
		{
			break;
		}
		if(pfProgressProc)
		{
			pfProgressProc(float((float)nProgressedCount++/nTotalCount)*2);
		}
		CString& strPath = listString.GetNext(pos);
		POSITION posBack;
		posBack = pos;
		if(strPath.Left(_tcslen(ALLOC_MARK)).Compare(ALLOC_MARK))
		{
			continue;
		}
		CString strAddr,strAddr2;
		int nPos;
		if((nPos = strPath.Find(_T(" : ")))<0)
		{
			continue;
		}
		strAddr = strPath.Right(strPath.GetLength() - (nPos+_tcslen(" : ")));		
		if((nPos = strAddr.Find(_T(" ")))<0)
		{
			continue;
		}
		strAddr = strAddr.Left(nPos);
		BOOL bFreed = FALSE;
		nOffset = 0;
		while(pos)
		{
			pos_2 = pos;
			CString& strPath2 = listString.GetNext(pos);
			nOffset++;
			
			if(strPath2.Left(_tcslen(FREE_MARK)).Compare(FREE_MARK))
			{
				continue;
			}
			if((nPos = strPath2.Find(_T(" : ")))<0)
			{
				continue;
			}
			strAddr2 = strPath2.Right(strPath2.GetLength() - (nPos+_tcslen(" : ")));
			if((nPos = strAddr2.Find(_T(" ")))<0)
			{
				continue;
			}
			strAddr2 = strAddr2.Left(nPos);

			if(strAddr != strAddr2)
			{
				continue;
			}
			bFreed = TRUE;
			break;
		}
		if(bFreed)
		{
			if(nOffset == 1)
			{
				posBack = pos_2;
				listString.GetNext(posBack);
			}
			listString.RemoveAt(pos_1);
			listString.RemoveAt(pos_2);
		}
		pos = posBack;
	}

	file.Close();
	file.Open(szLeakReportPath,CFile::modeCreate | CFile::modeWrite);
	pos = listString.GetHeadPosition();
	while(pos)
	{
		strLine = listString.GetNext(pos);
		strLine += _T("\r\n");
		file.Write((LPCTSTR)strLine,strLine.GetLength());
	}
	file.Close();

	return listString.GetCount();
	#endif
	//}}AFX
}

typedef DWORD (__stdcall *PFNGETMODULEFILENAMEEX) (HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize);
typedef BOOL (__stdcall *PFNENUMPROCESSMODULES) (HANDLE hProcess, HMODULE* lphModule, DWORD cb, LPDWORD lpcbNeeded);

PFNGETMODULEFILENAMEEX pfnGetModuleFileNameEx;
PFNENUMPROCESSMODULES pfnEnumProcessModules;

BOOL CSystemManager::GetWindowModuleName(HWND hWnd, TCHAR* szModuleName, int cchName)
{
    HINSTANCE hPsApiDll = ::LoadLibrary(_T("psapi.dll")); 
	if(!hPsApiDll){
		return FALSE;
	}

	#ifdef UNICODE
		pfnGetModuleFileNameEx = (PFNGETMODULEFILENAMEEX) GetProcAddress(hPsApiDll, "GetModuleFileNameExW"); 
	#else
		pfnGetModuleFileNameEx = (PFNGETMODULEFILENAMEEX) GetProcAddress(hPsApiDll, "GetModuleFileNameExA"); 
	#endif // !UNICODE
    if(!pfnGetModuleFileNameEx){
		::FreeLibrary(hPsApiDll);
		return FALSE;
	}

	pfnEnumProcessModules = (PFNENUMPROCESSMODULES) GetProcAddress(hPsApiDll, "EnumProcessModules"); 
	pfnEnumProcessModules = 
       (PFNENUMPROCESSMODULES) GetProcAddress  
          (hPsApiDll, "EnumProcessModules"); 
	if(!pfnEnumProcessModules){
		::FreeLibrary(hPsApiDll);
		return FALSE;
	}

	HMODULE* lphModule;
    
    DWORD procid = 0;
    DWORD modulesize = 0;
    BOOL bInheritHandle = false;

	if(hWnd != NULL){
        GetWindowThreadProcessId(hWnd,&procid);
        HANDLE process = OpenProcess(PROCESS_ALL_ACCESS | 
           PROCESS_QUERY_INFORMATION, bInheritHandle,procid);
        if(process != NULL){
            lphModule = new HMODULE[1];
            if(pfnEnumProcessModules(process,lphModule, 
                      (sizeof(HMODULE)),&modulesize) != 0){
                pfnGetModuleFileNameEx(process,lphModule[0],szModuleName,cchName);
                CloseHandle(process);
                delete lphModule;
				::FreeLibrary(hPsApiDll);
                return TRUE;
            }
            delete lphModule;
        }
        CloseHandle(process);
    }
	::FreeLibrary(hPsApiDll);
    return FALSE;
}
