/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WSP_CONTENT_TYPE_H_
#define _WSP_CONTENT_TYPE_H_

#ifndef _WSP_VAR_TYPE_H_
#include "wsp_vartype.h"
#endif

#ifndef _WSP_CHAR_SET_H_
#include "wsp_charset.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//
//常规 content-type
//

#define	CNTTYPE_ANY_ANY					0x00 //"*/*"

#define	CNTTYPE_TEXT_ANY				0x01 //"text/*"
#define	CNTTYPE_TEXT_HTML				0x02 //"text/html"
#define	CNTTYPE_TEXT_PLAIN				0x03 //"text/plain"

#define	CNTTYPE_TEXT_X_VCALENDAR		0x06 //"text/x-vCalendar"
#define	CNTTYPE_TEXT_X_VCARD			0x07 //"text/x-vCard"	

#define	CNTTYPE_WAP_WML					0x08 //"text/vnd.wap.wml"	
#define	CNTTYPE_WAP_WMLSCRIPT			0x09 //"text/vnd.wap.wmlscript"
#define	CNTTYPE_WAP_WMLC				0x14 //"application/vnd.wap.wmlc"	
#define	CNTTYPE_WAP_WMLCSCRIPT			0x15 //"application/vnd.wap.wmlscriptc"

#define	CNTTYPE_IMAGE_ANY				0x1C //"image/*"			
#define	CNTTYPE_IMAGE_GIF				0x1D //"image/gif"		
#define	CNTTYPE_IMAGE_JPEG				0x1E //"image/jpeg"		
#define	CNTTYPE_IMAGE_TIFF				0x1F //"image/tiff"		
#define	CNTTYPE_IMAGE_PNG				0x20 //"image/png"		
#define	CNTTYPE_IMAGE_WBMP				0x21 //"image/vnd.wap.wbmp"

#define	CNTTYPE_WAP_MULTI_ANY			0x22 //"application/vnd.wap.multipart.*"	
#define	CNTTYPE_WAP_MULTI_MIXED			0x23 //"application/vnd.wap.multipart.mixed"
#define	CNTTYPE_WAP_MULTI_RELATED		0x33 //"application/vnd.wap.multipart.related"
#define	CNTTYPE_WAP_MULTI_BYTERANGES	0x25 //"application/vnd.wap.multipart.byteranges"

#define	CNTTYPE_APP_XML					0x27 //"application/xml"	
#define	CNTTYPE_TEXT_XML				0x28 //"text/xml"		

#define	CNTTYPE_WAP_MMS_MSG				0x3E //"application/vnd.wap.mms-message"
#define	CNTTYPE_OMA_DRM_MSG				0x48 //"application/vnd.oma.drm.message"

//
// content-type Assignments table
//

//字段 Name与Number对应的 结构
#define	LEN_WSP_CNTTYPE_NAME		64
typedef	struct	_CNTTYPE_ASSIGN
{
	char		szName[LEN_WSP_CNTTYPE_NAME];
	DWORD		dwNum;

} CNTTYPE_ASSIGN;

//字段 表
extern	const	CNTTYPE_ASSIGN	g_lpWell_Know_Content_Type_Tab[];
extern	const	DWORD	g_dwWell_Know_Content_Type_Max;

//字段 查找函数
extern	BOOL	VarFind_CntTypeAssign( DWORD dwFlag, IN OUT CNTTYPE_ASSIGN* lpCntTypeAssign );
#define	CNTTYPEFIND_NAME_BYNUM			1
#define	CNTTYPEFIND_NUM_BYNAME			2


//
// Well-Known Parameter Assignments 
//

typedef	struct	_CNTTYPE_PARAM
{
	DWORD				dwMask; //字段屏蔽码
	
	DWORD				dwQ_1_1;			// Q-value = 1*2 OCTET, is the same as in Uintvar-integer,
	DWORD				dwCharSet_1_1;		// Well-known-charset = Any-charset | Integer-value  ;Any-charset = <Octet 128>
	SHORT_STR			stLevel_1_1;		// Version-value = Short-integer | Text-string
	LONG				dwType_1_1;			// Integer-value
	LPSTR				pszName_1_1;		// Text-string
	LPSTR				pszFileName_1_1;	// Text-string
	SHORT_STR			stDifferences_1_1;	// Field-name = Token-text | Well-known-field-name
	SHORTINT			bPadding_1_1;		// Short-integer
	SHORT_STR			stType_1_2;			// Constrained-encoding = Extension-Media | Short-integer
	LPSTR				pszStart_1_2;		// Text-string
	LPSTR				pszStart_info_1_2;	// Text-string
	LPSTR				pszComment_1_3;		// Text-string
	LPSTR				pszDomain_1_3;		// Text-string
	DWORD				dwMax_Age_1_3;		// Delta-seconds-value = Integer-value
	LPSTR				pszPath_1_3;		// Text-string
	BYTE				bSecure_1_3;		// No-value
	SHORTINT			bSEC_1_4;			// Short-integer
	LPSTR				pszMAC_1_4;			// Text-value
	DWORD				dwCreation_date_1_4;	// Date-value
	DWORD				dwModification_date_1_4;// Date-value
	DWORD				dwRead_date_1_4;		// Date-value
	DWORD				dwSize_1_4;			// Integer-value
	LPSTR				pszName_1_4;		// Text-value
	LPSTR				pszFilename_1_4;	// Text-value
	LPSTR				pszStart_1_4;		// Text-value
	LPSTR				pszStart_info_1_4;	// Text-value
	LPSTR				pszComment_1_4;		// Text-value
	LPSTR				pszDomain_1_4;		// Text-value
	LPSTR				pszPath_1_4;		// Text-value

} CNTTYPE_PARAM, *LPCNTTYPE_PARAM;

// dwMask of CNTTYPE_PARAM
#define CTPMASK_Q_1_1					0x00000001
#define CTPMASK_CHARSET_1_1				0x00000002
#define CTPMASK_LEVEL_1_1				0x00000004
#define CTPMASK_TYPE_1_1				0x00000008
#define CTPMASK_NAME_1_1				0x00000010
#define CTPMASK_FILENAME_1_1			0x00000020
#define CTPMASK_DIFFERENCES_1_1			0x00000040
#define CTPMASK_PADDING_1_1				0x00000080
#define CTPMASK_TYPE_1_2				0x00000100
#define CTPMASK_START_1_2				0x00000200
#define CTPMASK_START_INFO_1_2			0x00000400
#define CTPMASK_COMMENT_1_3				0x00000800
#define CTPMASK_DOMAIN_1_3				0x00001000
#define CTPMASK_MAX_AGE_1_3				0x00002000
#define CTPMASK_PATH_1_3				0x00004000
#define CTPMASK_SECURE_1_3				0x00008000
#define CTPMASK_SEC_1_4					0x00010000
#define CTPMASK_MAC_1_4					0x00020000
#define CTPMASK_CREATION_DATE_1_4		0x00040000
#define CTPMASK_MODIFICATION_DATE_1_4	0x00080000
#define CTPMASK_READ_DATE_1_4			0x00100000
#define CTPMASK_SIZE_1_4				0x00200000
#define CTPMASK_NAME_1_4				0x00400000
#define CTPMASK_FILENAME_1_4			0x00800000
#define CTPMASK_START_1_4				0x01000000
#define CTPMASK_START_INFO_1_4			0x02000000
#define CTPMASK_COMMENT_1_4				0x04000000
#define CTPMASK_DOMAIN_1_4				0x08000000
#define CTPMASK_PATH_1_4				0x10000000

//
// Table 38. Well-Known Parameter Assignments
//

#define CTP_Q_1_1						0x00
#define CTP_CHARSET_1_1					0x01
#define CTP_LEVEL_1_1					0x02
#define CTP_TYPE_1_1					0x03
#define CTP_NAME_1_1					0x05
#define CTP_FILENAME_1_1				0x06
#define CTP_DIFFERENCES_1_1				0x07
#define CTP_PADDING_1_1					0x08
#define CTP_TYPE_1_2					0x09
#define CTP_START_1_2					0x0A
#define CTP_START_INFO_1_2				0x0B
#define CTP_COMMENT_1_3					0x0C
#define CTP_DOMAIN_1_3					0x0D
#define CTP_MAX_AGE_1_3					0x0E
#define CTP_PATH_1_3					0x0F
#define CTP_SECURE_1_3					0x10
#define CTP_SEC_1_4						0x11
#define CTP_MAC_1_4						0x12
#define CTP_CREATION_DATE_1_4			0x13
#define CTP_MODIFICATION_DATE_1_4		0x14
#define CTP_READ_DATE_1_4				0x15
#define CTP_SIZE_1_4					0x16
#define CTP_NAME_1_4					0x17
#define CTP_FILENAME_1_4				0x18
#define CTP_START_1_4					0x19
#define CTP_START_INFO_1_4				0x1A
#define CTP_COMMENT_1_4					0x1B
#define CTP_DOMAIN_1_4					0x1C
#define CTP_PATH_1_4					0x1D


//
// Content_Type Field define
//

typedef	struct	_CONTENT_TYPE
{
	INTEGER_STR			stType;
	CNTTYPE_PARAM		stParam;

} CONTENT_TYPE, *LPCONTENT_TYPE;

//
extern	BOOL	VarDec_ContentType( DATA_DEAL* lpDealData, OUT CONTENT_TYPE* lpContentType );
extern	BOOL	VarEnc_ContentType( OUT DATA_DEAL* lpDealData, CONTENT_TYPE* lpContentType );
extern	void	VarFree_ContentType( CONTENT_TYPE* lpContentType );
extern	DWORD	VarLen_ContentType( CONTENT_TYPE* lpContentType );
extern	void	VarMap_ContentType( OUT CONTENT_TYPE* lpContentType );
extern	void	VarMove_ContentType( OUT CONTENT_TYPE* lpContentTypeDes, IN OUT CONTENT_TYPE* lpContentTypeSrc );

extern	BOOL	VarCmp_ContentType( INTEGER_STR* lpContentType1, INTEGER_STR* lpContentType2 );

	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WSP_CONTENT_TYPE_H_
