//////////////////////////////////////////////////////////////////////
//																	//
//	  FileManager.cpp : implementation of the CFileManager class	//
//						Writed by J.J.W.							//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileManager.h"
#include "direct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// \’z/Á–Å
//////////////////////////////////////////////////////////////////////
int _nCounter;
TCHAR _szTemp1[0x400];
TCHAR _szTemp2[0x400];
BOOL(*CFileManager::m_pFileOperator)(LPCTSTR szPath) = NULL;
BOOL(*CFileManager::m_pFolderPreOperator)(LPCTSTR szPath) = NULL;
BOOL(*CFileManager::m_pFolderPostOperator)(LPCTSTR szPath) = NULL;

BOOL(*CFileManager::m_pFileOperatorBack)(LPCTSTR szPath) = NULL;
BOOL(*CFileManager::m_pFolderPreOperatorBack)(LPCTSTR szPath) = NULL;
BOOL(*CFileManager::m_pFolderPostOperatorBack)(LPCTSTR szPath) = NULL;

int FindChar_Custom(LPCTSTR lpszPath, TCHAR ch, int nStart)
{
	ASSERT( lpszPath );
	ASSERT( nStart >= 0 );

	TCHAR* lpStr = new TCHAR[lstrlen(lpszPath)+1], *lpChar;
	if ( lpStr == NULL)
	{
		return -1;
	}
	if(lstrlen(lpszPath) <= nStart)
	{
		delete lpStr;
		return -1;
	}
	lstrcpy(lpStr, lpszPath + nStart);
	lpChar = lpStr;
	int nPos = 0;
	BOOL bFind = FALSE;
	for ( ;lpChar[0] != _T('\0'); lpChar = _tcsinc(lpChar) ) {
		if ( lpChar[0] == ch ) {
			bFind = TRUE;
			break;
		}
	}
	delete lpStr;

	return bFind?(nStart+lpChar-lpStr):-1;
}

BOOL Counter(LPCTSTR szFilePath)
{
	_nCounter++;
	return TRUE;
}

BOOL CopyFile(LPCTSTR szFilePath)
{
	TCHAR szFilePathNew[0x104];
	lstrcpy(szFilePathNew,_szTemp2);
	lstrcat(szFilePathNew,szFilePath+lstrlen(_szTemp1));
	::CopyFile(szFilePath,szFilePathNew,FALSE);
	return TRUE;
}

BOOL RemoveFile(LPCTSTR szPath)
{
	DWORD dwAttr;
	dwAttr = ::GetFileAttributes(szPath);
	if(dwAttr == 0xffffffff)
	{
		return false;
	}
	if(dwAttr & FILE_ATTRIBUTE_READONLY)
	{
		dwAttr &=~FILE_ATTRIBUTE_READONLY;
		::SetFileAttributes(szPath,dwAttr);
	}
	CFile::Remove(szPath);
	return TRUE;
}

BOOL RemoveEmptyFolder(LPCTSTR szPath)
{
	DWORD wAttr;
	if ((wAttr = GetFileAttributes(szPath)) == (DWORD)-1L)
		CFileException::ThrowOsError((LONG)GetLastError());
	if(wAttr & FILE_ATTRIBUTE_READONLY)
	{
		wAttr&=~FILE_ATTRIBUTE_READONLY;
		if (!SetFileAttributes(szPath, wAttr))
			CFileException::ThrowOsError((LONG)GetLastError());
	}
	
	return RemoveDirectory(szPath);
}

BOOL MakeEmptyFolder(LPCTSTR szPath)
{
	int nResult;
	TCHAR szTemp[0x104];
	lstrcpy(szTemp,_szTemp2);
	lstrcat(szTemp,szPath+lstrlen(_szTemp1));

	nResult = CFileManager::ExistFile(szTemp);
	if(nResult == CFileManager::FILE)
	{
		AfxMessageBox(_T("There is any file whose name is equal with folder to create"));
		return FALSE;
	}

	if(nResult == CFileManager::NOEXIST)
	{
		CreateDirectory(szTemp,NULL);
	}
	return TRUE;
}

BOOL CFileManager::CopyFolder(LPCTSTR szSrcPath,LPCTSTR szDstPath)
{
	if(ExistFile(szDstPath) != DIRECTORY)
	{
		if(!MakeDirectory(szDstPath))
		{
			return FALSE;
		}
	}
	lstrcpy(_szTemp2,szDstPath);
	if(_szTemp2[lstrlen(_szTemp2)-1]!='\\')
	{
		lstrcat(_szTemp2,_T("\\"));
	}
	
	lstrcpy(_szTemp1,szSrcPath);
	if(szSrcPath[lstrlen(_szTemp1)-1]!='\\')
	{
		lstrcat(_szTemp1,_T("\\"));
	}
	
	return BrowseDirExt(szSrcPath,CopyFile,MakeEmptyFolder);
}

BOOL CFileManager::RemoveFile(LPCTSTR szPath)
{
	return ::RemoveFile(szPath);
}

BOOL CFileManager::RemoveFolder(LPCTSTR szPath)
{
	BrowseDirExt(szPath,::RemoveFile,NULL,RemoveEmptyFolder);
	return TRUE;
}

CFileManager::CFileManager()
{

}

CFileManager::~CFileManager()
{

}

BOOL CFileManager::GetDirectoryPath(HWND hwndOwner, LPCTSTR szTitle, LPTSTR szFolderPath, int nMaxLeng, UINT uFlags)
{
	TCHAR szPath[MAX_PATH];
	CString strTemp;
	::CoInitialize(NULL);
	BROWSEINFO bi;
	bi.hwndOwner		= hwndOwner;
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= NULL;
	bi.lpszTitle		= szTitle;
	bi.ulFlags			= uFlags ;
	bi.lpfn				= NULL;
	

	LPITEMIDLIST lpItemList = ::SHBrowseForFolder(&bi);
	
	if( lpItemList == NULL )
	{
		::CoUninitialize();
		return FALSE;
	}

	::SHGetPathFromIDList(lpItemList,szPath);
	::CoUninitialize();
	int lenString;
	lenString = lstrlen(szPath);
	if(szPath[lenString-1] != _T('\\'))
		lstrcat(szPath, _T("\\"));
	if(lstrlen(szPath) < nMaxLeng)
	{
		lstrcpy(szFolderPath,szPath);
	}
	else	
	{
//		_tcsncpy(szFolderPath,szPath,nMaxLeng-1);
		lstrcpyn(szFolderPath,szPath,nMaxLeng);
		return FALSE;
	}
	return TRUE;
}

BOOL CFileManager::GetDirectoryPath(LPCTSTR szTitle, LPTSTR szFolderPath, int nMaxLeng, UINT uFlags)
{
	return GetDirectoryPath(NULL, szTitle, szFolderPath, nMaxLeng, uFlags);
}

BOOL CFileManager::SaveStatus()
{
	m_pFileOperatorBack = m_pFileOperator;
	m_pFolderPreOperatorBack = m_pFolderPreOperator;
	m_pFolderPostOperatorBack = m_pFolderPostOperator;
	return true;
}

BOOL CFileManager::RestoreStatus()
{
	m_pFileOperator = m_pFileOperatorBack;
	m_pFolderPreOperator = m_pFolderPreOperatorBack;
	m_pFolderPostOperator = m_pFolderPostOperatorBack;
	return true;
}

BOOL CFileManager::BrowseDirExt(LPCTSTR szFolderPath,BOOL(*FileOperator)(LPCTSTR szPath),BOOL(*FolderPreOperator)(LPCTSTR szPath),BOOL(*FolderPostOperator)(LPCTSTR szPath))
{
	m_pFileOperator = FileOperator;
	m_pFolderPreOperator = FolderPreOperator;
	m_pFolderPostOperator = FolderPostOperator;
	TCHAR* szPath;
	szPath = new TCHAR[lstrlen(szFolderPath)+2];
	lstrcpy(szPath,szFolderPath);
	if(szPath[lstrlen(szPath)-1] != _T('\\'))
	{
		lstrcat(szPath,_T("\\"));
	}
	BOOL bResult = BrowseDir(szPath);
	delete[] szPath;
	return bResult;
}

BOOL CFileManager::BrowseFolder(
	LPCTSTR szFolderPath,
	BOOL(*FileOperator)(LPCTSTR szPath, LPVOID lpParam),
	BOOL(*PreFolderOperator)(LPCTSTR szPath, LPVOID lpParam),
	BOOL(*PostFolderOperator)(LPCTSTR szPath, LPVOID lpParam),
	LPVOID lpParam)
{
	TCHAR* szPath;
	szPath = new TCHAR[lstrlen(szFolderPath)+2];
	lstrcpy(szPath,szFolderPath);
	if(szPath[lstrlen(szPath)-1] != _T('\\'))
	{
		lstrcat(szPath,_T("\\"));
	}
	BOOL bResult = BrowseDirEx(szPath, FileOperator, PreFolderOperator, PostFolderOperator, lpParam);
	delete[] szPath;
	return bResult;
}

BOOL CFileManager::BrowseDir(LPCTSTR szFolderPath)
{
	TCHAR chBuf[4] = {0x2E, 0x2E, 0, 0};
	HANDLE	hFile;
    TCHAR	szPath[0x104];
	int		nNextFile = 1;

	WIN32_FIND_DATA  finddata;

	lstrcpy(szPath, szFolderPath);
	lstrcat(szPath, _T("*.*"));
	if(m_pFolderPreOperator)
	{
		if(!m_pFolderPreOperator(szFolderPath))
		{
			return FALSE;
		}
	}

	hFile = FindFirstFile(szPath, &finddata);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	while (nNextFile)
	{
		lstrcpy(szPath, szFolderPath);
		lstrcat(szPath, finddata.cFileName);

		if (finddata.dwFileAttributes & 0x10)
		{
			if (lstrcmp(finddata.cFileName, &chBuf[1]) != 0 && lstrcmp(finddata.cFileName, chBuf) != 0)
			{
				int nLen = lstrlen(szPath);
				
				szPath[nLen] = 0x5C;
				szPath[nLen+1] = 0;
				BrowseDir(szPath);
				szPath[nLen] = 0;
			}
			nNextFile = FindNextFile(hFile, &finddata);
			continue;
		}
		if(m_pFileOperator)
		{
			m_pFileOperator(szPath);
		}
		nNextFile = FindNextFile(hFile, &finddata);
	}
	FindClose(hFile);
	if(m_pFolderPostOperator)
	{
		m_pFolderPostOperator(szFolderPath);
	}
	return TRUE;
}

BOOL CFileManager::BrowseDirEx(
								LPCTSTR szFolderPath,
								BOOL(*FileOperator)(LPCTSTR szPath, LPVOID lpParam),
								BOOL(*PreFolderOperator)(LPCTSTR szPath, LPVOID lpParam),
								BOOL(*PostFolderOperator)(LPCTSTR szPath, LPVOID lpParam),
								LPVOID lpParam)
{
	TCHAR chBuf[4] = {0x2E, 0x2E, 0, 0};
	HANDLE	hFile;
    TCHAR	szPath[0x104];
	int		nNextFile = 1;

	WIN32_FIND_DATA  finddata;

	lstrcpy(szPath, szFolderPath);
	lstrcat(szPath, _T("*.*"));
	if(PreFolderOperator)
	{
		if(!PreFolderOperator(szFolderPath, lpParam))
		{
			return FALSE;
		}
	}

	hFile = FindFirstFile(szPath, &finddata);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	while (nNextFile)
	{
		lstrcpy(szPath, szFolderPath);
		lstrcat(szPath, finddata.cFileName);

		if (finddata.dwFileAttributes & 0x10)
		{
			if (lstrcmp(finddata.cFileName, &chBuf[1]) != 0 && lstrcmp(finddata.cFileName, chBuf) != 0)
			{
				int nLen = lstrlen(szPath);
				
				szPath[nLen] = 0x5C;
				szPath[nLen+1] = 0;
				BrowseDirEx(szPath, FileOperator, PreFolderOperator, PostFolderOperator, lpParam);
				szPath[nLen] = 0;
			}
			nNextFile = FindNextFile(hFile, &finddata);
			continue;
		}
		if(FileOperator)
		{
			FileOperator(szPath, lpParam);
		}
		nNextFile = FindNextFile(hFile, &finddata);
	}
	FindClose(hFile);
	if(PostFolderOperator)
	{
		PostFolderOperator(szFolderPath, lpParam);
	}
	return TRUE;
}

TCHAR _szRootFolderPath[1024];
BOOL _bSearchSubFolder = TRUE;
BOOL IsRootFolder(LPCTSTR szPath)
{
	if(lstrcmp(szPath,_szRootFolderPath))
	{
		return false;
	}
	return true;
}

int CFileManager::GetFilesInFolder(LPCTSTR szFolderPath,BOOL bSearchSubFolder)
{
	_nCounter = 0;
	TCHAR* szNormalizedFolderPath;
	BOOL IsAllocNormalizedChar =false;
	if(szFolderPath[lstrlen(szFolderPath)-1] != _T('\\'))
	{
		szNormalizedFolderPath = new TCHAR[lstrlen(szFolderPath)+2];
		lstrcpy(szNormalizedFolderPath,szFolderPath);
		szNormalizedFolderPath[lstrlen(szFolderPath)+1] = _T('\0');
		szNormalizedFolderPath[lstrlen(szFolderPath)] = _T('\\');
		IsAllocNormalizedChar = true;
	}
	else
	{
		szNormalizedFolderPath = (LPTSTR)szFolderPath;
	}
	if(!bSearchSubFolder)
	{
		_bSearchSubFolder = FALSE;
		lstrcpy(_szRootFolderPath,szNormalizedFolderPath);
		BrowseDirExt(szNormalizedFolderPath,Counter,IsRootFolder);
	}
	else
	{
		BrowseDirExt(szNormalizedFolderPath,Counter);
	}
	if(IsAllocNormalizedChar)
	{
		delete[] szNormalizedFolderPath;
	}

	return _nCounter;
}

int CFileManager::GetFoldersInFolder(LPCTSTR szFolderPath)
{
 	_nCounter = 0;
	BrowseDirExt(szFolderPath,NULL,Counter);
	return _nCounter;
}

int CFileManager::ExistFile(LPCTSTR szPath)
{
	DWORD dwAttr;
	dwAttr = GetFileAttributes(szPath);
	if(dwAttr == -1) return NOEXIST;
	if(dwAttr & FILE_ATTRIBUTE_DIRECTORY) return DIRECTORY;
	return FILE;
}

BOOL CFileManager::MakeDirectory(LPCTSTR szPath)
{
	CString strPath,strTemp;
	TCHAR szFolderPath[1024];
	if(sizeof(szFolderPath) <= lstrlen(szPath)){
		return FALSE;
	}
	lstrcpy(szFolderPath,szPath);
	int nType = ExistFile(szFolderPath);
	if(nType == DIRECTORY) return TRUE;
	if(nType == FILE) return FALSE;

	if(szFolderPath[lstrlen(szFolderPath)-1] != _T('\\'))
	{
		szFolderPath[lstrlen(szFolderPath)+1] = _T('\0');
		szFolderPath[lstrlen(szFolderPath)] = _T('\\');
	}
	
	
	strPath = szFolderPath;
	int nStart = -1;
	if(lstrlen(szFolderPath)<3)
	{
		return FALSE;
	}
	BOOL IsNetworkPath = false;
	if(szFolderPath[0] == _T('\\') && szFolderPath[1] == _T('\\'))
	{
		IsNetworkPath = true;
	}
	if(!((szFolderPath[0]>=_T('A') && szFolderPath[0]<=_T('Z')) || (szFolderPath[0]>=_T('a') && szFolderPath[0]<=_T('z'))) || szFolderPath[1] != _T(':') || szFolderPath[2] != _T('\\')) 
	{
		if(!IsNetworkPath)
		{
			return false;
		}
	}
	TCHAR szDrive[3];
	_tcsncpy(szDrive,szFolderPath,2);
	szDrive[2] = 0;
	if(!IsNetworkPath && (GetDriveType(szDrive) == DRIVE_UNKNOWN || GetDriveType(szDrive) == DRIVE_NO_ROOT_DIR))
	{
		return FALSE;
	}
	nStart = 2;
	if(IsNetworkPath)
	{
		nStart = FindChar_Custom((LPCTSTR)strPath,_T('\\'),nStart+1);
	}

	do
	{
		nStart = FindChar_Custom((LPCTSTR)strPath,_T('\\'),nStart+1);
		if(nStart == -1)
		{
			break;
		}
		strTemp = strPath.Left(nStart);
		int nFileKind = ExistFile((LPCTSTR)strTemp);
		if(nFileKind == FILE){
			return FALSE;
		}
		if(nFileKind == NOEXIST){
			break;
		}
	}
	while(TRUE);

	while(nStart!=-1)
	{
		strTemp = strPath.Left(nStart);
		SetLastError(0);
		if(!CreateDirectory(strTemp,NULL))
		{
			return FALSE;
		}
		nStart = FindChar_Custom((LPCTSTR)strPath,_T('\\'),nStart+1);
	}
	return ExistFile((LPCTSTR)strPath) == DIRECTORY;
}
/*
BOOL CFileManager::LoadTextLines(char* szPath,int* pnLineNum,char** ppTextLine,int** ppnPos)
{
	CString strPath;
	CStdioFile f;
	if( !f.Open( szPath, CFile::modeRead | CFile::typeText ) ) {
		return FALSE;
	}
	int nCount = 0,nLength = 0;
	
	while(f.ReadString(strPath))
	{
		if(!strPath.IsEmpty())
		{
			nLength += (strPath.GetLength()+1);
		}
	}

	
	char* pszTextLines = new char[nLength];
	int* pnPosition = new int[nLength];
	f.Seek(0,CFile::begin);
	int nPos = 0;
	while(f.ReadString(strPath))
	{
		if(!strPath.IsEmpty())
		{
			pnPosition[nCount++] = nPos;
			lstrcpy(pszTextLines+nPos,(LPSTR)(LPCSTR)strPath);
			nPos+=(strPath.GetLength()+1);
		}
	}
	
	*ppTextLine=pszTextLines;
	*pnLineNum = nCount;
	*ppnPos = pnPosition;
	f.Close();
	return TRUE;
	
}
BOOL CFileManager::SaveTextLines(char* szPath,char* pTextLine,int nLength)
{


	return TRUE;
}
*/
/*
BOOL CFileManager::LoadStringArray(char* szPath,CArray<CString,CString&>& aryString)
{
	CString strPath;
	aryString.RemoveAll();
	CStdioFile f;
	if( !f.Open( szPath, CFile::modeRead | CFile::typeText ) ) {
		return FALSE;
	}
	
	while(f.ReadString(strPath))
	{
		if(!strPath.IsEmpty())
		{
			aryString.Add(strPath);
		}
	}
	f.Close();
	return TRUE;
}
BOOL CFileManager::SaveStringArray(char* szPath,CStringArray& aryString)
{
	CString strPath;
	CStdioFile f;
	if( !f.Open( szPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText ) ) {
		AfxMessageBox("File creating error");
		return FALSE;
	}
	for(int i = 0; i < aryString.GetSize(); i++)
	{
		strPath = aryString.ElementAt(i);
		if(strPath[strPath.GetLength()-1] != 0x0A)
		{
			strPath += 0x0A;
		}
		f.WriteString(strPath);
	}
	f.Close();
	return TRUE;
}

*/

BOOL CFileManager::WriteProfileInt(LPCTSTR szFilePath,LPCTSTR szKeyName,int nValue)
{
	CStdioFile file;
	int nResult;

	if(ExistFile(szFilePath) == CFileManager::NOEXIST)
	{
		nResult = file.Open(szFilePath,CFile::modeCreate | CFile::modeReadWrite);
	}
	else
	{
		nResult = file.Open(szFilePath,CFile::modeReadWrite);
	}
	if(!nResult) return FALSE;
	CString strLine,strTemp;
	BOOL bKeyExist = FALSE;
	int nBeforeSeek = 0,nCurSeek = 0;
	while(file.ReadString(strLine))
	{
		nCurSeek = (int)file.GetPosition();
		strLine.TrimLeft();
		if(strLine.GetLength() < (int)lstrlen(szKeyName))
		{
			nBeforeSeek = nCurSeek;
			continue;
		}
		int nPos = strLine.ReverseFind(_T('='));
		if(nPos != -1)
		{
			strLine = strLine.Left(nPos);
		}
		strLine.TrimRight();
		if(!strLine.CompareNoCase(szKeyName))
		{
			bKeyExist = TRUE;
			break;
		}
		nBeforeSeek = nCurSeek;
	}
	strTemp.Format(_T("%s=%d"),szKeyName,nValue);
	if(!bKeyExist)
	{
		file.Close();
		nResult = file.Open(szFilePath,CFile::modeReadWrite | CFile::typeBinary);
		char szBuf[]={0,0};
		if(file.GetLength()>2)
		{
			file.Seek(-2,CFile::end);
			file.Read(szBuf,2);
			if(szBuf[0] != 0x0D || szBuf[1] != 0x0A)
			{
				strTemp.Insert(0,0x0A);
				strTemp.Insert(0,0x0D);
			}
		}
		file.Seek(0,CFile::end);
		TCHAR CRLF[] = {0x0d,0x0a};
		strTemp+=CRLF;
		file.Write(strTemp,strTemp.GetLength());
	}
	else
	{
		int nFileSize = (int)file.GetLength();
		char *lpBuf;
		file.Close();
		nResult = file.Open(szFilePath,CFile::modeReadWrite | CFile::typeBinary);
		if(nFileSize - nCurSeek)
		{
			lpBuf = new char[nFileSize - nCurSeek];
			file.Seek(nCurSeek,CFile::begin);
			file.Read(lpBuf,nFileSize - nCurSeek);
		}
		file.Seek(nBeforeSeek,CFile::begin);
		TCHAR CRLF[] = {0x0d,0x0a};
		strTemp+=CRLF;
		file.Write((LPCTSTR)strTemp,strTemp.GetLength());
		file.Seek(file.GetPosition(),CFile::begin);
		if(nFileSize - nCurSeek)
		{
			file.Write(lpBuf,nFileSize - nCurSeek);
		}
	}
	file.SetLength(file.GetPosition());
	file.Close();
	return TRUE;
}

int CFileManager::GetProfileInt(LPCTSTR szFilePath,LPCTSTR szKeyName,int nDefaultVal)
{
	CStdioFile file;
	int nResult = file.Open(szFilePath,CFile::modeRead);
	if(!nResult) return nDefaultVal;
	CString strLine,strTemp;
	BOOL bKeyExist = FALSE;
	int nBeforeSeek = 0,nCurSeek = 0;
	int nValue;
	while(file.ReadString(strLine))
	{
		strLine.TrimLeft();
		if(strLine.GetLength() < (int)lstrlen(szKeyName))
		{
			continue;
		}
		if(!strLine.Left(lstrlen(szKeyName)).CompareNoCase(szKeyName))
		{
			strTemp = strLine.Right(strLine.GetLength() - lstrlen(szKeyName));
			int nPos;
			nPos = strTemp.ReverseFind('=');
			if(nPos == -1 || !(strTemp.GetLength()-nPos-1)) return nDefaultVal;
#ifdef _UNICODE
			swscanf((LPCTSTR)strTemp.Right(strTemp.GetLength()-nPos-1),_T("%d"),&nValue);
#else
			sscanf((LPSTR)(LPCSTR)strTemp.Right(strTemp.GetLength()-nPos-1),"%d",&nValue);
#endif
			bKeyExist = TRUE;
			break;
		}
	}
	file.Close();
	if(!bKeyExist) return nDefaultVal;


	return nValue;
}

BOOL CFileManager::NormalizeFolderPath(LPTSTR szPath, int nLen)
{
	if(nLen>=0){
		if(lstrlen(szPath)+2 > nLen){
			return FALSE;
		}
	}
	while(szPath[lstrlen(szPath)-1]==_T('\\'))
	{
		szPath[lstrlen(szPath)-1] = 0;
	}
	szPath[lstrlen(szPath)+1] = 0;
	szPath[lstrlen(szPath)] = _T('\\');
	return TRUE;
}