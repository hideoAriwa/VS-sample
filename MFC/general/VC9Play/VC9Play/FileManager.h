//////////////////////////////////////////////////////////////////////
//																	//
//		FileManager.h@: interface of the CFileManager class		//
//						Writed by J.J.W.						//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_FILEMANAGER_H__C7D75FA4_51CD_40D0_BB25_643B160E4A95__INCLUDED_)
#define AFX_FILEMANAGER_H__C7D75FA4_51CD_40D0_BB25_643B160E4A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxtempl.h>
#ifndef DllExport
#define DllExport __declspec(dllexport)
#define DllImport __declspec(dllimport)
#endif
class  __declspec( dllexport ) CFileManager  
{
	
public:
	enum{NOEXIST,FILE,DIRECTORY};
	CFileManager();
	virtual ~CFileManager();
	static BOOL BrowseDirExt(LPCTSTR szFolderPath,BOOL(*FileOperator)(LPCTSTR szPath),BOOL(*PreFolderOperator)(LPCTSTR szPath) = NULL,BOOL(*PostFolderOperator)(LPCTSTR szPath) = NULL);
	static BOOL BrowseFolder(
		LPCTSTR szFolderPath,
		BOOL(*FileOperator)(LPCTSTR szPath, LPVOID lpParam),
		BOOL(*PreFolderOperator)(LPCTSTR szPath, LPVOID lpParam) = NULL,
		BOOL(*PostFolderOperator)(LPCTSTR szPath, LPVOID lpParam) = NULL,
		LPVOID lpParam = NULL);
	static BOOL GetDirectoryPath(LPCTSTR szTitle, LPTSTR szFolderPath, int nMaxLeng, UINT uFlags = BIF_EDITBOX);
	static BOOL GetDirectoryPath(HWND hwndOwner, LPCTSTR szTitle, LPTSTR szFolderPath, int nMaxLeng, UINT uFlags = BIF_EDITBOX);
	static int GetFilesInFolder(LPCTSTR szFolderPath,BOOL bSearchSubFolder = true);
	static int GetFoldersInFolder(LPCTSTR szFolderPath);
	static int ExistFile(LPCTSTR szPath);
	static BOOL MakeDirectory(LPCTSTR szPath);
	static BOOL RemoveFile(LPCTSTR szPath);
	static BOOL RemoveFolder(LPCTSTR szPath);
	static BOOL CopyFolder(LPCTSTR szSrcPath,LPCTSTR szDstPath);
	static BOOL WriteProfileInt(LPCTSTR szFilePath,LPCTSTR szKeyName,int nValue);
	static int GetProfileInt(LPCTSTR szFilePath,LPCTSTR szKeyName,int nDefaultVal=0);
	static BOOL NormalizeFolderPath(LPTSTR szPath,int nLen = -1);
	static BOOL SaveStatus();
	static BOOL RestoreStatus();
//	static BOOL LoadTextLines(char* szPath,int* pnLineNum,char** ppTextLine,int** ppnPos);
//	static BOOL SaveTextLines(char* szPath,char* pTextLine,int nLength);

private:
	static BOOL(*m_pFileOperator)(LPCTSTR szPath);
	static BOOL(*m_pFolderPreOperator)(LPCTSTR szPath);
	static BOOL(*m_pFolderPostOperator)(LPCTSTR szPath);

	static BOOL(*m_pFileOperatorBack)(LPCTSTR szPath);
	static BOOL(*m_pFolderPreOperatorBack)(LPCTSTR szPath);
	static BOOL(*m_pFolderPostOperatorBack)(LPCTSTR szPath);

	static BOOL BrowseDir(LPCTSTR szFolderPath);
	static BOOL BrowseDirEx(
		LPCTSTR szFolderPath, 
		BOOL(*FileOperator)(LPCTSTR szPath, LPVOID lpParam),
		BOOL(*PreFolderOperator)(LPCTSTR szPath, LPVOID lpParam) = NULL,
		BOOL(*PostFolderOperator)(LPCTSTR szPath, LPVOID lpParam) = NULL, 
		LPVOID lpParam = NULL);
	

};

#endif // !defined(AFX_FILEMANAGER_H__C7D75FA4_51CD_40D0_BB25_643B160E4A95__INCLUDED_)
