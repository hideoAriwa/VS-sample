
#ifndef _COMMON_TYPES_
#define _COMMON_TYPES_
#ifdef WIN32
#include <windowsx.h>
#else
#include "../ImgIO_CX/CxImage/ximadef.h"
#endif
#include "../include/subinc.h"


#define PROJECT_FOR_TELEPHON_READER	 0

#define PROJECT_FOR_NPR_SYS	 0

#define PROJECT_FOR_HW	1

#define PROJECT_FOR_ASP	 0

#define MULTI_CPU_SUPPORT	 0	// Pls describe num of grantable CPU
								// 0: Free, 1: Single support only nevertherless there is more...
								// 2 or more : describes working CPUs

#define CONTEST_MODE	 0

#define DPRK_MODE

//---- Appended by MMG for Numeric Reco...2009.05.17 ----
//#define _NUMERIC_RECOG
//-------------------------------------------------------

//---- Appended by MMG for Numeric Reco...2009.05.24 ----
//#define _FOR_KOREAN_LETTER
//-------------------------------------------------------

//---- Appended by MMG for Lawyer Table...2009.08.14 ----
//#define _FOR_JAPANESE_LAWYER_TABLE
//-------------------------------------------------------

//---- Appended by MMG for Visiting Card...2010.08.09 ----
#define _FOR_VISCARD_MODE
//--------------------------------------------------------

//---- Appended by MMG for Publication ...2010.08.18 ----
//#define _FOR_PUBLICATION_MODE
//--------------------------------------------------------

#define _RELIABLE_RECOG_RATE_FOR_DIRECTION 280
#define _MAX_RECOG_RATE			500					// max recog rate in layout
#define _LIMIT_RECOG_RATE		200					// limit recog rate as text layout ( 200 ? 230 ? 180 ? )



typedef BOOL(*PF_GetOrSetRecogProcessCanceled)(int nCancel);

enum CharCodeID 
{ 
	CID_Korean , CID_Japanes , CID_Chines, CID_Taiwan, CID_Russian, CID_English, CID_Digit , 
	CID_Symbol , CID_Hiragana , CID_Gatagana , CID_K_Kanji 
};

// Common Types...
// Language enumeration
enum Language 
{ 
	Unknown=-1, Korean, English, Russian, Japanes, Chines, Latin, Univers, lang_count = Univers, UniCode
};

enum LanguageEx
{ 
	Franc = Univers + 1,German,Italy,Portugues,Spanish,General
};

#define pdf_language Language

// Layer Type enumeration
enum LayerType
{
	unknownLayer=-1,textLayer_Horz,textLayer_Vert,tableLayer,imageLayer
};


// Recognition character's kind enumeration
enum _RECOG_CHAR_KIND
{ 
	CHAR_SYMBOL = 0x0001, 
	CHAR_DIGIT = 0x0002, 
	CHAR_HIRAKANA = 0x0004, 
	CHAR_GATAKANA = 0x0008, 
	CHAR_ENGLISH = 0x0010, 
	CHAR_RUSSIAN = 0x0020,
	CHAR_KOREAN = 0x0040, 
	CHAR_JAPANES_HANJA = 0x0080, 
	CHAR_CHINES_HANJA = 0x0100, 
	CHAR_K_KANJI = 0x0200,
	CHAR_ALL = 0x0ffff
};

struct _LAYER_TEMPLATE 
{
	int m_nLayerNum;
	int m_nLayerType; 
	int m_nTextType; 
	int m_nHalfConv;
	int m_nCR_LF;
	int m_bGetOnlyOneChar;
	int m_bGetOnlyOneLine;
	int m_nRecogCharKind;

	float m_fLeftScale;
	float m_fRightScale;
	float m_fTopScale;
	float m_fBottomScale;
};

// Layer's description
struct SD_EXPORT CLayerItemScript
{
	CLayerItemScript()
	{
		memset(this,0,sizeof(*this));
	};

	RECT m_rcLayer;
	LayerType m_nLayerType;
	_RECOG_CHAR_KIND m_dwRecogCharKind;
	int m_nHalfConv;// TextLayer ...	Bit0 : Symbol, Bit1 : Digit, Bit2 : English, Bit3 : Katakana
	int m_nCR_LF; // TextLayer ...		0 : Auto, 1 : Each Line, 2 : Non
	BOOL m_bGetOnlyOneChar; // If TRUE, layer include only one char
	BOOL m_bGetOnlyOneLine; // If TRUE, layer include only one linez
};

/////////////////////////////////////////////////////
// Appended By CMY in 2007.07.10...
/////////////////////////////////////////////////////
// Line Dir enumeration
enum LineDir
{
	line_Horz,line_Vert
};
// Line Attr enumeration
enum LineAttr
{
	line_Unknown = 0, line_Solid,line_Dash, line_Dot
};

// Line's description
struct SD_EXPORT CLineItemScript
{
	CLineItemScript()
	{
		memset(this,0,sizeof(*this));
	};

	RECT m_rcLayer;
	LineDir m_LineDir;		// 0: Horz Line, 1: Vert Line
	LineAttr m_LineAttr;	// 
	COLORREF m_crLineColor;	//
};
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// Recognition result description
struct SD_EXPORT CRecogResultStruct
{
	WORD* m_pwRecogChar_Uni;
	WORD* m_pwRecogRate;
	int m_nAllocCount;
	int m_nReturnCount;

	CRecogResultStruct(){
		m_nAllocCount = 0;
		m_nReturnCount = 0;
		m_pwRecogChar_Uni = NULL;
		m_pwRecogRate = NULL;
	};
	~CRecogResultStruct(){
		if(m_pwRecogChar_Uni)
			GlobalFreePtr(m_pwRecogChar_Uni);
		if(m_pwRecogRate)
			GlobalFreePtr(m_pwRecogRate);
	}
};
#ifdef _FOR_VISCARD_MODE

#define VISCARD_CHARTYPE_NAME			0x01
#define VISCARD_CHARTYPE_COMPANY		0x02
#define VISCARD_CHARTYPE_TEAM			0x04
#define VISCARD_CHARTYPE_DUTY			0x08
#define VISCARD_CHARTYPE_TELNUM			0x10
#define VISCARD_CHARTYPE_HANDNUM		0x20
#define VISCARD_CHARTYPE_FAXNUM			0x40
#define VISCARD_CHARTYPE_EMAIL			0x80
#define VISCARD_CHARTYPE_ADDR			0x100
#define VISCARD_CHARTYPE_URL			0x200
#define VISCARD_CHARTYPE_FIRST_NAME		0x400
#define VISCARD_CHARTYPE_LAST_NAME		0x800

#define VISCARD_CHARTYPE_MARK_CHAR		0x40000000
#define VISCARD_CHARTYPE_LAST_CHAR		0x80000000

#endif

struct SD_EXPORT CCharRectInfo
{
	enum FLAGS{ Char_SpellErr = 0x01 };
	DWORD dwLayerItemNum_TextItemNum;
	RECT rcRectOfChar;
	int nCharID;
	COLORREF color;
	WORD wFlag;
	WORD wRecogRate;
	WORD wLineNum;
	BOOL	bItalic;
	BOOL	bUnderline;
	BOOL	bBold;
	BYTE	bStrikeOut;
	BOOL bFuzzy;
	DWORD dwLang_CodeKindID; // CRecogObject::CharCodeID 
	BYTE	bFontIndex;
#ifdef _FOR_VISCARD_MODE
	DWORD	dwVisCharType;
	int		nHeightLebel;
#endif
};

#pragma warning(disable:4251)
class SD_EXPORT CAuxResult{
public:
	CStringArray m_csFontArray;
	WORD* m_pwFontID;
	CCharRectInfo* m_pCharRectInfo;

public:
	CAuxResult(){
		m_pwFontID = (WORD* )GlobalAllocPtr(GHND,sizeof(*m_pwFontID));
		m_pCharRectInfo = NULL;
	}

	~CAuxResult(){
		if(m_pwFontID)
			GlobalFreePtr(m_pwFontID);
		if(m_pCharRectInfo)
			GlobalFreePtr(m_pCharRectInfo);

		m_pwFontID = NULL;
		m_pCharRectInfo = NULL;
	}

	CAuxResult& operator = (CAuxResult& src){
		m_csFontArray.RemoveAll();
		m_csFontArray.Append(src.m_csFontArray);

		if(src.m_pwFontID){
			m_pwFontID = (WORD* )GlobalAllocPtr(GHND,GlobalSize(GlobalPtrHandle(src.m_pwFontID)));
			memmove(m_pwFontID,src.m_pwFontID,GlobalSize(GlobalPtrHandle(src.m_pwFontID)));
		}
		if(src.m_pCharRectInfo){
			m_pCharRectInfo = (CCharRectInfo* )GlobalAllocPtr(GHND,GlobalSize(src.m_pCharRectInfo));
			memmove(m_pCharRectInfo,src.m_pCharRectInfo,GlobalSize(GlobalPtrHandle(src.m_pCharRectInfo)));
		}
		return *this;
	}
};
#pragma warning(default:4251)
#endif

