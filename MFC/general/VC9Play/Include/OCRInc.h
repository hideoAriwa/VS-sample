#ifndef ___OCR___INCLUDE___
#define ___OCR___INCLUDE___



#if defined(_WIN32)
	#define DIR_SEP			_T('\\')
	#define DIR_SEP_			_T("\\")
	#define SD_API                   __stdcall
        #define SD_CDECL		__cdecl
	#define SD_FASTCALL		__fastcall
	#define SD_DATA
	#define SD_DATADEF
	#define SD_COMDAT
	#define SD_PACKING    4
	#ifndef PASCAL
	#define PASCAL  __stdcall
	#endif
	#define SD_INLINE                
	
	#define SD_STATIC	extern

	#define _MAX_PATH   260 /* max. length of full pathname */
	#define _MAX_DRIVE  3   /* max. length of drive component */
	#define _MAX_DIR    256 /* max. length of path component */
	#define _MAX_FNAME  256 /* max. length of file name component */
	#define _MAX_EXT    256 /* max. length of extension component */



//	#define SD_EXPORT		__declspec(dllexport)
//	#define SD_EXPORT		__declspec(dllimport)

	#define SD_EXPORT		
	#define SD_IMPORT		extern

	#ifndef AFX_ALWAYS_VTABLE
	#ifndef AFX_NOVTABLE
	#if _MSC_VER >= 1100 && !defined(_DEBUG)
	#define AFX_NOVTABLE __declspec(novtable)
	#else
	#define AFX_NOVTABLE
	#endif
	#endif
	#endif
#else
	#define _MSC_VER       1200
	//#define ASSERT(n)	
	#define DIR_SEP			_T('/')
	#define DIR_SEP_		_T("/")
	#define SD_API
//			__stdcall
	#define SD_CDECL
//		__cdecl
	#define SD_FASTCALL
//		__fastcall
	#define SD_DATA
	#define SD_DATADEF
	#define SD_COMDAT
	#define SD_PACKING    4
//	#ifndef PASCAL
	#define PASCAL
//  __stdcall
//	#endif
        #define SD_INLINE
	
	#define SD_STATIC	extern

	#define _MAX_PATH   260 /* max. length of full pathname */
	#define _MAX_DRIVE  3   /* max. length of drive component */
	#define _MAX_DIR    256 /* max. length of path component */
	#define _MAX_FNAME  256 /* max. length of file name component */
	#define _MAX_EXT    256 /* max. length of extension component */



	//#define SD_EXPORT		__declspec(dllexport)

	#define SD_EXPORT		
	#define SD_IMPORT		extern
	#ifndef AFX_ALWAYS_VTABLE
	#ifndef AFX_NOVTABLE
	#if _MSC_VER >= 1100 && !defined(_DEBUG)
	#define AFX_NOVTABLE
	#else
	#define AFX_NOVTABLE
	#endif
	#endif
	#endif
#endif



#endif
