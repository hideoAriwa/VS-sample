#ifndef _____INCLUDE_____SUBLIB______
#define _____INCLUDE_____SUBLIB______

#define __EXTRACT__MFC__
#include <String.h>
#include <stdio.h>

#include <stdlib.h>

#include "../SubLib/Archive.h"
#include "../SubLib/BridgeApi.h"
#include "../SubLib/Coll.h"
#include "../SubLib/File.h"
#include "../SubLib/File.h"
#include "../SubLib/FixAlloc.h"
#include "../SubLib/MFCPredefine.h"
#include "../SubLib/Object.h"
#include "OCRInc.h"
#include "../SubLib/Plex.h"
#include "../SubLib/String_.h"
#include "../SubLib/Templ.h"
#include "../SubLib/Types_.h"
#include "../SubLib/lxType.h"
#include "../SubLib/Dimmension.h"
#include "../SubLib/DummyObject.h"
#include "../SubLib/lxString.h"
#include "../SubLib/lxMemory.h"
#include "../SubLib/StrConvCore.h"

#ifdef _WIN32
//#define DLL_EXPORT		__declspec(dllexport)
#define DLL_EXPORT		
#else 
#define DLL_EXPORT		
#endif
#define POSITION		WPOSITION
#define __POSITION		W__POSITION
#define _AFX_DOUBLE		W_AFX_DOUBLE
#define _AFX_FLOAT		W_AFX_FLOAT
#define _CTypedPtrList	W_CTypedPtrList
#define AFX_CLASSINIT	W_AFX_CLASSINIT
#define CArchive		CwArchive
#define CArray			CwArray
#define CFile			CwFile
#define CFileStatus		CwFileStatus
#define CFixedAlloc		CwFixedAlloc
#define CList			CwList
#define CMap			CwMap
#define CObArray		CwObArray
#define CObject			CwObject
#define CObList			CwObList
#define CPlex			CwPlex
#define CPtrList		CwPtrList
#define CRuntimeClass	CwRuntimeClass
#define CString			CwString
#define CStringData		CwStringData
#define CTime			CwTime
#define CTimeSpan		CwTimeSpan
#define CTypedPtrArray	CwTypedPtrArray
#define CTypedPtrList	CwTypedPtrList
#define CByteArray		CwByteArray
#define CWordArray		CwWordArray
#define CDWordArray		CwDWordArray
#define CUIntArray		CwUIntArray

#define CStringArray	CwStringArray
#define CStringList		CwStringList
#define CMapPtrToPtr	CwMapPtrToPtr
#define CTypedPtrMap	CwTypedPtrMap
#define CPoint			CwPoint
#define CSize			CwSize
#define CRect			CwRect
#define CStdioFile		CwStdioFile
#endif