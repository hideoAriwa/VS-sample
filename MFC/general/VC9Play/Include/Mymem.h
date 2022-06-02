// access.h : header file
//
#ifndef MYMEM_INCLUDED
#define MYMEM_INCLUDED

#ifdef WIN32
#include <windows.h>
#include "windowsx.h"
#include "malloc.h"
#else
#include "subinc.h"
#include "stdlib.h"
#include "stdio.h"
#endif
//<--------------------------Writed by JJW------------------------------------

//#define MEM_LOGGING

#ifdef WIN32
#if defined(MEM_LOGGING) && defined(_DEBUG) // && !defined(GlobalDebugAlloc)// && defined(_DEBUG) 


//	#define USE_OUTPUT
#define FILE_WRITING

	
	#include "stdio.h"
	#include "TCHAR.h"
#ifndef ALLOC_MARK
	#define ALLOC_MARK	_T("Alloc : ")
	#define FREE_MARK	_T("Free : ")
#endif
	

	inline int Logging(char* szLog, LPCTSTR lpszLogKind)
	{
		HANDLE hSmph;
		HANDLE hFile;

		TCHAR szSmphName[128];
		TCHAR szLogFileMappingName[128];
		TCHAR szFileName[MAX_PATH + 1];

		DWORD dwResult = GetModuleFileName(NULL,szFileName,sizeof(szFileName));
		if(dwResult == 0)
		{
			Beep(1000, 100);
//			TRACE(_T("ERROR : GetModuleFileName\r\n"));
		}

		TCHAR* lpszTemp;
		lpszTemp = _tcsrchr(szFileName,DIR_SEP);
		if(lpszTemp){
			memmove(szFileName, lpszTemp + 1, _tcslen(lpszTemp + 1) + 1);
		}

		
		_stprintf(szSmphName, _T("%s_%s Logging Semaphore"), szFileName, lpszLogKind);
		_stprintf(szLogFileMappingName, _T("%s_%s Logging File Mapping"), szFileName, lpszLogKind);
		
		hSmph = ::OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, szSmphName);
		if(!hSmph)	/////////////////////First Access/////////////////////
		{
			hSmph = ::CreateSemaphore(NULL, 1, 1, szSmphName);
			WaitForSingleObject(hSmph, WAIT_TIMEOUT);
			if(!hSmph){
				::MessageBox(NULL, _T("Failed to Logging Sempahore Create"), _T("Logging Error"), MB_OK);
				return 0;
			}

			int nParamLength;
			nParamLength = sizeof(DWORD);
			HANDLE hMapFile = ::CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, nParamLength, szLogFileMappingName);
			if(!hMapFile){
				::MessageBox(NULL, _T("Failed to Logging Shared Memory Open"), _T("Logging Error"), MB_OK);
				return 0;
			}
			TCHAR szLogFileName[512];
			lstrcpy(szLogFileName,_T("C:"));
			lstrcat(szLogFileName,_T("\\mem_use_"));
			lstrcat(szLogFileName,szFileName);
			lstrcat(szLogFileName,_T("_"));
			lstrcat(szLogFileName,lpszLogKind);
			
			lstrcat(szLogFileName,_T(".log"));
			if((hFile = ::CreateFile(szLogFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
			{
				::MessageBox(NULL, _T("Logging File Open Failed"), _T("Logging Error"), MB_OK);
			}

			DWORD* pMapFile = (DWORD*)::MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
			*pMapFile = (DWORD)hFile;
		}
		else
		{
			WaitForSingleObject(hSmph, WAIT_TIMEOUT);
			HANDLE hMapFile = ::OpenFileMapping(FILE_MAP_WRITE, FALSE, szLogFileMappingName);
			if(!hMapFile)
			{
				::MessageBox(NULL, _T("Open Parameter Fail"), _T("Logging Error"), MB_OK);
				return 0;
			}
			DWORD* pMapFile = (DWORD*)::MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
			hFile = (HANDLE)*pMapFile;
			::UnmapViewOfFile(pMapFile);
		}

		char szLogLine[1024];
		strcpy(szLogLine,szLog);
		strcat(szLogLine, "\r\n");
		DWORD nWritten;
		::SetFilePointer(hFile,0,NULL,FILE_END);
		::WriteFile((HANDLE)hFile, szLogLine,sizeof(char)*strlen(szLogLine),&nWritten, NULL);
		if (nWritten != sizeof(char)*strlen(szLogLine))
		{
			Beep(1000,100);
		}
		::ReleaseSemaphore(hSmph, 1, NULL);
		return 1;
	}

	
	//{{AFX
	#if 0

	inline int GetLoggingInfo(HGLOBAL* phLogHandle,int* pnAllocedSize,int* pnWritedSize)
	{
		HANDLE hMapFile;
		int* pMapView;

		if(hMapFile = OpenFileMapping(FILE_MAP_WRITE,FALSE,_T("MemLeak Log")))
		{
			pMapView = (int *)::MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
		}
		else
		{
			hMapFile = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READWRITE, 0, sizeof(int)*10, _T("MemLeak Log"));
			pMapView = (int *)::MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
			pMapView[0] = 0;
			pMapView[1] = 0;
			pMapView[2] = 0;
			pMapView[3] = 0;
		}
		while(pMapView[0])
		{
			Sleep(0);
		}

		pMapView[0] = 1;

		*phLogHandle = (HGLOBAL)pMapView[1];
		if(pnAllocedSize) *pnAllocedSize = pMapView[2];
		if(pnWritedSize) *pnWritedSize = pMapView[3];
		
		
		UnmapViewOfFile(pMapView);
		
		return 1;
	}
	inline int SetLoggingInfo(HGLOBAL hLogHandle,int nAllocedSize,int nWritedSize)
	{
		HANDLE hMapFile;
		int* pMapView;

		if(hMapFile = OpenFileMapping(FILE_MAP_WRITE,FALSE,_T("MemLeak Log")))
		{
			pMapView = (int *)::MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
		}
		else
		{
			hMapFile = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READWRITE, 0, sizeof(int)*10, _T("MemLeak Log"));
			pMapView = (int *)::MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
		}

		pMapView[0] = 0;

		pMapView[1] = (int)hLogHandle;
		pMapView[2] = nAllocedSize;
		pMapView[3] = nWritedSize;
		
		pMapView[0] = 0;
		UnmapViewOfFile(pMapView);

		return 1;
	}
	
	inline int LoggingOrg(TCHAR* szLog)
	{
#if defined(FILE_WRITING)
		TCHAR szFileName[512],szLogFileName[512];
		DWORD dwResult = GetModuleFileName(NULL,szFileName,sizeof(szFileName));
		if(_tcsrchr(szFileName,DIR_SEP)){
			*_tcsrchr(szFileName,DIR_SEP) = NULL;
		}
//		lstrcpy(szLogFileName,szFileName);
		lstrcpy(szLogFileName,_T("C:"));
		lstrcat(szLogFileName,_T("\\mem_use_"));
		lstrcat(szLogFileName,szFileName+lstrlen(szFileName)+1);
		lstrcat(szLogFileName,_T(".log"));


		TCHAR szLogLine[1024];
		lstrcpy(szLogLine,szLog);
		lstrcat(szLogLine,_T("\r\n"));

		HANDLE hFile;

		int nRetryCount = 0;

		SetLastError(0);
		while((hFile = ::CreateFile(szLogFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		{
			if(nRetryCount++>50)
			{
				HANDLE hFile_;
				int nErrorNo;
				nErrorNo = GetLastError();
				TCHAR szErrorNo[256];
				_stprintf(szErrorNo,_T(" : %d"),nErrorNo);
				_tcscat(szLogLine,szErrorNo);
				if((hFile_ = ::CreateFile(_T("c:\\mem_err.txt"), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
				{
					::SetFilePointer(hFile_,0,NULL,FILE_END);
					DWORD nWritten;
					if (!::WriteFile((HANDLE)hFile_, szLogLine,sizeof(TCHAR)*lstrlen(szLogLine),&nWritten, NULL))
					{
						Beep(1000,100);
					}
					if (nWritten != sizeof(TCHAR)*lstrlen(szLogLine))
					{
						Beep(1000,100);
					}
					::CloseHandle((HANDLE)hFile_);
				}
				else
				{
					MessageBox(NULL,szLogLine,NULL,MB_OK);
				}
				return 0;
			}
			Sleep(10);
		}

		::SetFilePointer(hFile,0,NULL,FILE_END);
		DWORD nWritten;
		static int nAccessCount = 0;
		
		static CRITICAL_SECTION cs;
		if(!nAccessCount++)
		{
			InitializeCriticalSection(&cs);
		}
		EnterCriticalSection(&cs);
		if (!::WriteFile((HANDLE)hFile, szLogLine,sizeof(TCHAR)*lstrlen(szLogLine),&nWritten, NULL))
		{
			Beep(1000,100);
		}
		LeaveCriticalSection(&cs);
		if (nWritten != sizeof(TCHAR)*lstrlen(szLogLine))
		{
			Beep(1000,100);
		}
		::CloseHandle((HANDLE)hFile);
#else

		HGLOBAL hLogHandle;
		int nWritedSize;
		int nAllocedSize;
		int nNeedSize = strlen(szLog)*sizeof(TCHAR) + sizeof(TCHAR)*3;

		GetLoggingInfo(&hLogHandle,&nAllocedSize,&nWritedSize);

		
		if(nAllocedSize<nWritedSize+nNeedSize)
		{
			nAllocedSize+=10000;
			if(!hLogHandle){
				hLogHandle = GlobalAlloc(GHND,nAllocedSize);
			}
			else{
				hLogHandle = GlobalReAlloc(hLogHandle,nAllocedSize,GHND);
			}
		}

		if(!hLogHandle)
		{
			return 0;
		}

		LPTSTR lpBuf = (LPTSTR)GlobalLock(hLogHandle);
		_tcscpy((lpBuf+nWritedSize),szLog);
		_tcscat((lpBuf+nWritedSize),"\n");
		nWritedSize+=_tcslen(szLog)*sizeof(TCHAR) + sizeof(TCHAR);
		GlobalUnlock(hLogHandle);

		SetLoggingInfo(hLogHandle,nAllocedSize,nWritedSize);
#endif
		
		return 1;
	}
	inline int SearchMemoryLeak()
	{
		HGLOBAL hLogHandle;
		GetLoggingInfo(&hLogHandle,NULL,NULL);
		if(!hLogHandle){
			return 0;
		}

		LPTSTR lpLog = (LPTSTR)GlobalLock(hLogHandle);
		if(!lpLog){
			return -1;
		}
		
		int i,j;
		int nCount = 0;
		for(i=0; i<(int)GlobalSize(hLogHandle); i++){
			if(lpLog[i] == '\n'){
				nCount++;
			}
		}

		if(!nCount){
			return 0;
		}

		int* pnIndexes;
		pnIndexes = new int[nCount];
		for(i=0; i<nCount; i++) pnIndexes[i] = -1;


		TCHAR* token;
		token = strtok(lpLog,"\r\n");
		nCount = 0;
		while( token != NULL )
		{
			pnIndexes[nCount++] = token - lpLog;
			token = strtok(NULL,"\r\n");
		}

		
		int nAllocAddr,nFreeAddr;
		for(i=0; i<nCount; i++)
		{
			if(pnIndexes[i] == -1){
					continue;
			}
			if(_tcsncmp(lpLog+pnIndexes[i],ALLOC_MARK,strlen(ALLOC_MARK))){
				continue;
			}
			TCHAR* pszAddr_End = _tcschr(lpLog+pnIndexes[i]+strlen(ALLOC_MARK),_TCHAR(' '));
			if(!pszAddr_End){
				continue;
			}
			
			*pszAddr_End = _TCHAR('\0');
			sscanf(lpLog+pnIndexes[i]+strlen(ALLOC_MARK),"%x",&nAllocAddr);
			*pszAddr_End = _TCHAR(' ');

	
			for(j=i+1; j<nCount; j++)
			{
				if(pnIndexes[j] == -1){
					continue;
				}
				if(_tcsncmp(lpLog+pnIndexes[j],FREE_MARK,strlen(FREE_MARK))){
					continue;
				}
				pszAddr_End = _tcschr(lpLog+pnIndexes[j]+strlen(FREE_MARK),_TCHAR(' '));
				if(!pszAddr_End){
					continue;
				}
				*pszAddr_End = _TCHAR('\0');
				sscanf(lpLog+pnIndexes[j]+strlen(FREE_MARK),"%x",&nFreeAddr);
				*pszAddr_End = _TCHAR(' ');
				if(nAllocAddr == nFreeAddr){
					pnIndexes[i] = pnIndexes[j] = -1;
				}
			}
		}

		int nLeakCount = 0;
		TCHAR szBuf[1024];
		for(i=0; i<nCount; i++)
		{
			if(pnIndexes[i] == -1){
				continue;
			}
			_tcscpy(szBuf,lpLog+pnIndexes[i]);
			strcat(szBuf,_T("\n"));
			OutputDebugString(szBuf);
			nLeakCount++;
		}
		
		GlobalFree(hLogHandle);
		hLogHandle = NULL;
		if(pnIndexes) delete pnIndexes;

		return nLeakCount;
	}
	#endif
	//}}AFX

	
	inline HGLOBAL GlobalDebugAllocFunc(UINT uFlags,DWORD dwBytes,LPCSTR lpszFile,int nLine)
	{
		HGLOBAL h = GlobalAlloc(uFlags,dwBytes);
		
	#ifdef _DEBUG
		char szBuf[256];
		sprintf(szBuf, "%s%p %s %d (size:%d)", ALLOC_MARK, h, lpszFile, nLine, dwBytes);
		#ifdef USE_OUTPUT
		#ifndef _UNICODE
		OutputDebugString(szBuf);
		OutputDebugString(_T("\n"));
		#endif
		#else
		Logging(szBuf, _T("GlobalAllocFree"));
		#endif

	#endif
		return h;
	}

	inline HGLOBAL GlobalDebugFreeFunc(HGLOBAL hMem,LPCSTR lpszFile,int nLine)
	{
		
	#ifdef _DEBUG
		char szBuf[256];
		sprintf(szBuf, "%s%p %s %d (size:%d)", FREE_MARK, hMem, lpszFile, nLine, GlobalSize(hMem));
		#ifdef USE_OUTPUT
		#ifndef _UNICODE
		OutputDebugString(szBuf);
		OutputDebugString(_T("\n"));
		#endif
		#else
		Logging(szBuf, _T("GlobalAllocFree"));
		#endif
	#endif
		HGLOBAL h = GlobalFree(hMem);
		return h;
	}

	inline HGLOBAL GlobalDebugReAllocFunc(HGLOBAL hMem, DWORD dwBytes, UINT uFlags, LPCSTR lpszFile, int nLine)
	{
	#ifdef _DEBUG
		char szBuf[256];
		sprintf(szBuf, "%s%p %s %d (size:%d)", FREE_MARK, hMem, lpszFile, nLine, GlobalSize(hMem));
		#ifdef USE_OUTPUT
		#ifndef _UNICODE
		OutputDebugString(szBuf);
		OutputDebugString(_T("\n"));
		#endif
		#else
		Logging(szBuf, _T("GlobalAllocFree"));
		#endif
	#endif
		hMem = GlobalReAlloc(hMem, dwBytes, uFlags);
		
	#ifdef _DEBUG
		sprintf(szBuf, "%s%p %s %d (size:%d)", ALLOC_MARK, hMem, lpszFile, nLine, dwBytes);
		#ifdef USE_OUTPUT
		#ifndef _UNICODE
		OutputDebugString(szBuf);
		OutputDebugString(_T("\n"));
		#endif
		#else
		Logging(szBuf, _T("GlobalAllocFree"));
		#endif

	#endif
		return hMem;
	}



	#define GlobalDebugAlloc(flags,size) GlobalDebugAllocFunc((flags), (size), __FILE__, __LINE__)
	#define GlobalDebugFree(h) GlobalDebugFreeFunc((h), __FILE__, __LINE__)
	#define GlobalDebugReAlloc(h,size,flags) GlobalDebugReAllocFunc((h), (size), (flags), __FILE__, __LINE__)

	#define GlobalAlloc		GlobalDebugAlloc
	#define GlobalFree		GlobalDebugFree
	#define GlobalReAlloc		GlobalDebugReAlloc

#if 1 // Now normal action
	inline void* DebugMallocFunc(size_t size, LPCSTR lpszFile,int nLine)
	{
		void* p = malloc(size);
		
	#ifdef _DEBUG
		char szBuf[256];
		sprintf(szBuf, "%s%p %s %d (size:%d)", ALLOC_MARK, p, lpszFile, nLine, size);
		#ifdef USE_OUTPUT
		#ifndef _UNICODE
		OutputDebugString(szBuf);
		OutputDebugString(_T("\n"));
		#endif
		#else
		Logging(szBuf, _T("MallocFree"));
		#endif

	#endif
		return p;
	}

	inline void* DebugReallocFunc(void* p, size_t size, LPCSTR lpszFile,int nLine)
	{
	#ifdef _DEBUG
		char szBuf[256];
		sprintf(szBuf, "%s%p %s %d", FREE_MARK, p, lpszFile, nLine);
		#ifdef USE_OUTPUT
		#ifndef _UNICODE
		OutputDebugString(szBuf);
		OutputDebugString(_T("\n"));
		#endif
		#else
		Logging(szBuf, _T("MallocFree"));
		#endif

	#endif
		p = realloc(p, size);
		
	#ifdef _DEBUG
		
		sprintf(szBuf, "%s%p %s %d (size:%d)", ALLOC_MARK, p, lpszFile, nLine, size);
		#ifdef USE_OUTPUT
		#ifndef _UNICODE
		OutputDebugString(szBuf);
		OutputDebugString(_T("\n"));
		#endif
		#else
		Logging(szBuf, _T("MallocFree"));
		#endif

	#endif
		return p;
	}
	
	inline void DebugFreeFunc(void* p, LPCSTR lpszFile,int nLine)
	{
		
	#ifdef _DEBUG
		char szBuf[256];
		sprintf(szBuf, "%s%p %s %d", FREE_MARK, p, lpszFile, nLine);
		#ifdef USE_OUTPUT
		#ifndef _UNICODE
		OutputDebugString(szBuf);
		OutputDebugString(_T("\n"));
		#endif
		#else
		Logging(szBuf, _T("MallocFree"));
		
		#endif

	#endif
		free(p);
		
	}
	
		
	#define DebugMalloc(size)		DebugMallocFunc((size), __FILE__, __LINE__)
	#define DebugFree(mem)			DebugFreeFunc((mem), __FILE__, __LINE__)
	#define DebugRealloc(mem, size)	DebugReallocFunc((mem), (size), __FILE__, __LINE__)

	#define malloc			DebugMalloc
	#define free			DebugFree
	#define realloc			DebugRealloc
#endif

#endif	
#endif
//--------------------------------------------------------------------------->


class CMemAllocFree{
	LPVOID lpAddr;
	void ParamInit(){
		lpAddr = NULL;
		hMem = NULL;
	}
public:
	int nSize;
	HGLOBAL hMem;
	LPVOID lpData;
	~CMemAllocFree(){
		if(lpAddr)
			GlobalFreePtr(lpAddr);
		if(hMem)
			GlobalFree(hMem);
	}
	CMemAllocFree(UINT uFlag,DWORD dwSize,LPVOID &lpAddrSrc){
		ParamInit();
		if(dwSize){
			lpData = lpAddr = lpAddrSrc = GlobalAllocPtr(uFlag,dwSize ? dwSize : 1);
			nSize=(int)dwSize;
		}
	}
	CMemAllocFree(UINT uFlag,DWORD dwSize){
		ParamInit();
		if(dwSize){
			lpData = lpAddr = GlobalAllocPtr(uFlag,dwSize ? dwSize : 1);
			nSize=(int)dwSize;
		}
	}
	CMemAllocFree(UINT uFlag,DWORD dwSize,HGLOBAL *phMemSrc){
		ParamInit();
		if(dwSize){
			hMem = GlobalAlloc(uFlag,dwSize ? dwSize : 1);
			if(phMemSrc)
				*phMemSrc = hMem;
			nSize=(int)dwSize;
		}
	}
	void Refresh(LPVOID &lpAddrSrc){
		lpData = lpAddrSrc = lpAddr;
	}
	void Refresh(){
		lpData = lpAddr;
	}
	void Refresh(HGLOBAL *hMemSrc){
		*hMemSrc = hMem;
	}
};

template <class TObjType> class CBackupRestoreValue{
	TObjType tObjBk;
	TObjType* ptObjBk;
public:
	CBackupRestoreValue(TObjType* objOrg){
		ptObjBk = objOrg;
		tObjBk = *objOrg;
	}
	~CBackupRestoreValue(){
		*ptObjBk = tObjBk;
	}

};

template <class TMemType> class CMemNewDelete {
	TMemType* lpAddr;
	void ParamInit(){
		lpData = lpAddr = NULL;
		nSize = 0;
	}
public:
	TMemType* lpData;
	int nSize;
	~CMemNewDelete(){
		if(lpAddr)
			delete[] lpAddr;
	}
	CMemNewDelete(){
		ParamInit();
	}

	CMemNewDelete(DWORD dwSize,TMemType &lpAddrSrc,BOOL bInit = true){
		ParamInit();
		if(dwSize){
			lpData = lpAddr = new TMemType[dwSize ? dwSize : 1];
			nSize=(int)dwSize * sizeof(TMemType);
			memset(lpData,0,nSize);
		}
	}
	void ReNew(DWORD dwSize,BOOL bInit = true){
		if(lpAddr)
			delete[] lpAddr;
		ParamInit();
		if(dwSize){
			lpData = lpAddr = new TMemType[dwSize ? dwSize : 1];
			nSize=(int)dwSize * sizeof(TMemType);
			memset(lpData,0,nSize);
		}
	}

	CMemNewDelete(DWORD dwSize,BOOL bInit = true){
		ParamInit();
		if(dwSize){
			lpData = lpAddr = new TMemType[dwSize ? dwSize : 1];
			nSize=(int)dwSize * sizeof(TMemType);
			memset(lpData,0,nSize);
		}
	}
};


class CMemLockUnlock{
	HGLOBAL hMem;
public:
	int nSize;
	LPVOID lpData;
	~CMemLockUnlock(){
		if(hMem)
			GlobalUnlock(hMem);
	}
	CMemLockUnlock(HGLOBAL hMem_,LPVOID &lpAddr){
		hMem = hMem_;
		if(hMem){
			lpData = lpAddr = GlobalLock(hMem);
			nSize=GlobalSize(hMem);
		}
	}
	CMemLockUnlock(HGLOBAL hMem_){
		hMem = hMem_;
		if(hMem){
			lpData = GlobalLock(hMem);
			nSize=GlobalSize(hMem);
		}
	}
	void Refresh(LPVOID &lpAddr){
		lpData = lpAddr = GlobalLock(hMem);
		GlobalUnlock(hMem);
	}
	void Refresh(){
		lpData = GlobalLock(hMem);
		GlobalUnlock(hMem);
	}
};

/*
class CMemCommitDecommit{
	LPVOID vpMem;
public:
	int nSize;
	LPVOID lpData;
	~CMemCommitDecommit(){
		if(vpMem){
			  BOOL bSuccess = VirtualFree(
							 vpMem,            // base address of block
							 nSize,//nPageLimit*dwPageSize, // bytes of committed pages
							 MEM_DECOMMIT);//MEM_RELEASE);//MEM_DECOMMIT| MEM_RELEASE); // decommit the pages
		}
	}

	CMemCommitDecommit(LPVOID vpMem_){
		vpMem = vpMem_;
		LPVOID lpNext = vpMem;
		if(vpMem){
			MEMORY_BASIC_INFORMATION mbiBuffer;
			VirtualQuery(vpMem,&mbiBuffer, sizeof(mbiBuffer));

			lpData = VirtualAlloc(
							   (LPVOID) lpNext, // next page to commit
							   mbiBuffer.RegionSize,         // page size, in bytes
							   MEM_COMMIT,         // allocate a committed page
							   PAGE_READWRITE);    // read/write access
			nSize = mbiBuffer.RegionSize;
		}
	}
};

inline LPVOID MyVirtualAlloc(UINT uFlg,int nSize){
	  LPVOID lpvBase = VirtualAlloc(
							 NULL,                 // system selects address
							 nSize, // size of allocation
							 MEM_RESERVE,          // allocate reserved pages
							 PAGE_NOACCESS);       // protection = no access

	  if(lpvBase && (uFlg | GMEM_ZEROINIT)){
		  CMemCommitDecommit mem(lpvBase);
		  memset(mem.lpData,0,mem.nSize);
	  }
	  return lpvBase;
}

inline void MyVirtualFree(LPVOID vpMem){
	  BOOL bSuccess = VirtualFree(
					 vpMem,            // base address of block
					 0,//nPageLimit*dwPageSize, // bytes of committed pages
					 MEM_RELEASE);//MEM_DECOMMIT| MEM_RELEASE); // decommit the pages
}

inline LPVOID MyVirtualReAlloc(LPVOID lpvBaseOrg,int nSizeNew,UINT uFlg){
	  LPVOID lpvBase = VirtualAlloc(
							 NULL,                 // system selects address
							 nSizeNew, // size of allocation
							 MEM_RESERVE,          // allocate reserved pages
							 PAGE_NOACCESS);       // protection = no access

	  if(lpvBase && (uFlg | GMEM_ZEROINIT)){
		  CMemCommitDecommit mem(lpvBase);
		  memset(mem.lpData,0,mem.nSize);
	  }

	  CMemCommitDecommit mem(lpvBase);
	  CMemCommitDecommit memOrg(lpvBaseOrg);
	  int nMoveSize = mem.nSize;
	  if(nMoveSize > memOrg.nSize)
		  nMoveSize = memOrg.nSize;
	  memmove(mem.lpData,memOrg.lpData,nMoveSize);

	  MyVirtualFree(lpvBaseOrg);
	  return lpvBase;
}

inline void MyVirtualUnlock(LPVOID vpMem){
	  BOOL bSuccess = VirtualFree(
					 vpMem,            // base address of block
					 0,//nPageLimit*dwPageSize, // bytes of committed pages
					 MEM_DECOMMIT);//MEM_RELEASE);//MEM_DECOMMIT| MEM_RELEASE); // decommit the pages
}


inline LPVOID MyVirtualLock(LPVOID vpMem){
	MEMORY_BASIC_INFORMATION mbiBuffer;
	VirtualQuery(vpMem,&mbiBuffer, sizeof(mbiBuffer));

	LPVOID lpNext = vpMem;
	VirtualAlloc(
					   (LPVOID) lpNext, // next page to commit
					   mbiBuffer.RegionSize,         // page size, in bytes
					   MEM_COMMIT,         // allocate a committed page
					   PAGE_READWRITE);    // read/write access
	return vpMem;
}
*/


#endif
