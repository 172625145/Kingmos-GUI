/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WSP_HEADERS_H_
#define _WSP_HEADERS_H_

#ifndef _WSP_VAR_TYPE_H_
#include "wsp_vartype.h"
#endif

#ifndef _WSP_CHAR_SET_H_
#include "wsp_charset.h"
#endif

#ifndef _WSP_LANGUAGE_H_
#include "wsp_lang.h"
#endif

#ifndef _WSP_CONTENT_TYPE_H_
#include "wsp_contenttype.h"
#endif

#ifndef _WSP_PUSH_APP_ID_H_
#include "wsp_pushappid.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus



//
//  Header Field Name define
//

typedef	struct	_SHEADER_ASSIGN
{
	char				szName[64];
	SHORTINT			bNum;

} SHEADER_ASSIGN;

extern	const	SHEADER_ASSIGN	g_lpWell_Know_Headers_Tab[];
extern	const	DWORD	g_dwWell_Know_Headers_Max;




// ***********************************************************
// Header Field Name Assignments 定义
// ***********************************************************

//for Ver1.1
#define	SHDR_Accept					0x00
#define	SHDR_Accept_Charset1		0x01
#define	SHDR_Accept_Encoding1		0x02
#define	SHDR_Accept_Language		0x03
#define	SHDR_Accept_Ranges			0x04
#define	SHDR_Age					0x05
#define	SHDR_Allow					0x06
#define	SHDR_Authorization			0x07
#define	SHDR_Cache_Control1			0x08
#define	SHDR_Connection				0x09
#define	SHDR_Content_Base1			0x0A
#define	SHDR_Content_Encoding		0x0B
#define	SHDR_Content_Language		0x0C
#define	SHDR_Content_Length			0x0D
#define	SHDR_Content_Location		0x0E
#define	SHDR_Content_MD5			0x0F
#define	SHDR_Content_Range1			0x10
#define	SHDR_Content_Type			0x11
#define	SHDR_Date					0x12
#define	SHDR_Etag					0x13
#define	SHDR_Expires				0x14
#define	SHDR_From					0x15
#define	SHDR_Host					0x16
#define	SHDR_If_Modified_Since		0x17
#define	SHDR_If_Match				0x18
#define	SHDR_If_None_Match			0x19
#define	SHDR_If_Range				0x1A
#define	SHDR_If_Unmodified_Since	0x1B
#define	SHDR_Location				0x1C
#define	SHDR_Last_Modified			0x1D
#define	SHDR_Max_Forwards			0x1E
#define	SHDR_Pragma					0x1F
#define	SHDR_Proxy_Authenticate		0x20
#define	SHDR_Proxy_Authorization	0x21
#define	SHDR_Public					0x22
#define	SHDR_Range					0x23
#define	SHDR_Referer				0x24
#define	SHDR_Retry_After			0x25
#define	SHDR_Server					0x26
#define	SHDR_Transfer_Encoding		0x27
#define	SHDR_Upgrade				0x28
#define	SHDR_User_Agent				0x29
#define	SHDR_Vary					0x2A
#define	SHDR_Via					0x2B
#define	SHDR_Warning				0x2C
#define	SHDR_WWW_Authenticate		0x2D
#define	SHDR_Content_Disposition1	0x2E
//for Ver1.2
#define	SHDR_X_Wap_Application_Id	0x2F
#define	SHDR_X_Wap_Content_URI		0x30
#define	SHDR_X_Wap_Initiator_URI	0x31
#define	SHDR_Accept_Application		0x32
#define	SHDR_Bearer_Indication		0x33
#define	SHDR_Push_Flag				0x34
#define	SHDR_Profile				0x35
#define	SHDR_Profile_Diff			0x36
#define	SHDR_Profile_Warning1		0x37
//for Ver1.3
#define	SHDR_Expect					0x38
#define	SHDR_TE						0x39
#define	SHDR_Trailer				0x3A
#define	SHDR_Accept_Charset			0x3B
#define	SHDR_Accept_Encoding		0x3C
#define	SHDR_Cache_Control1_1_4		0x3D
#define	SHDR_Content_Range			0x3E
#define	SHDR_X_Wap_Tod				0x3F
#define	SHDR_Content_ID				0x40
#define	SHDR_Set_Cookie				0x41
#define	SHDR_Cookie					0x42
#define	SHDR_Encoding_Version		0x43
//for Ver1.4
#define	SHDR_Profile_Warning		0x44
#define	SHDR_Content_Disposition	0x45
#define	SHDR_X_WAP_Security			0x46
#define	SHDR_Cache_Control			0x47
//xyg_2004_10_27_added  maybe wrong
#define	SHDR_Proxy_Connection		0x73
#define	SHDR_X_Wap_Proxy_Set_Cookie	0x74

// ***********************************************************
// Header Field Mask 定义
// ***********************************************************

//for Ver1.1
#define	SHMASK1_Accept					0x00000001
#define	SHMASK1_Accept_Charset1			0x00000002
#define	SHMASK1_Accept_Encoding1		0x00000004
#define	SHMASK1_Accept_Language			0x00000008
#define	SHMASK1_Accept_Ranges			0x00000010
#define	SHMASK1_Age						0x00000020
#define	SHMASK1_Allow					0x00000040
#define	SHMASK1_Authorization			0x00000080
#define	SHMASK1_Cache_Control1			0x00000100
#define	SHMASK1_Connection				0x00000200
#define	SHMASK1_Content_Base1			0x00000400
#define	SHMASK1_Content_Encoding		0x00000800
#define	SHMASK1_Content_Language		0x00001000
#define	SHMASK1_Content_Length			0x00002000
#define	SHMASK1_Content_Location		0x00004000
#define	SHMASK1_Content_MD5				0x00008000
#define	SHMASK1_Content_Range1			0x00010000
#define	SHMASK1_Content_Type			0x00020000
#define	SHMASK1_Date					0x00040000
#define	SHMASK1_Etag					0x00080000
#define	SHMASK1_Expires					0x00100000
#define	SHMASK1_From					0x00200000
#define	SHMASK1_Host					0x00400000
#define	SHMASK1_If_Modified_Since		0x00800000
#define	SHMASK1_If_Match				0x01000000
#define	SHMASK1_If_None_Match			0x02000000
#define	SHMASK1_If_Range				0x04000000
#define	SHMASK1_If_Unmodified_Since		0x08000000
#define	SHMASK1_Location				0x10000000
#define	SHMASK1_Last_Modified			0x20000000
#define	SHMASK1_Max_Forwards			0x40000000
#define	SHMASK1_Pragma					0x80000000

#define	SHMASK2_Proxy_Authenticate		0x00000001
#define	SHMASK2_Proxy_Authorization		0x00000002
#define	SHMASK2_Public					0x00000004
#define	SHMASK2_Range					0x00000008
#define	SHMASK2_Referer					0x00000010
#define	SHMASK2_Retry_After				0x00000020
#define	SHMASK2_Server					0x00000040
#define	SHMASK2_Transfer_Encoding		0x00000080
#define	SHMASK2_Upgrade					0x00000100
#define	SHMASK2_User_Agent				0x00000200
#define	SHMASK2_Vary					0x00000400
#define	SHMASK2_Via						0x00000800
#define	SHMASK2_Warning					0x00001000
#define	SHMASK2_WWW_Authenticate		0x00002000
#define	SHMASK2_Content_Disposition1	0x00004000
//for Ver1.2
#define	SHMASK2_X_Wap_Application_Id	0x00008000
#define	SHMASK2_X_Wap_Content_URI		0x00010000
#define	SHMASK2_X_Wap_Initiator_URI		0x00020000
#define	SHMASK2_Accept_Application		0x00040000
#define	SHMASK2_Bearer_Indication		0x00080000
#define	SHMASK2_Push_Flag				0x00100000
#define	SHMASK2_Profile					0x00200000
#define	SHMASK2_Profile_Diff			0x00400000
#define	SHMASK2_Profile_Warning1		0x00800000
//for Ver1.3
#define	SHMASK2_Expect					0x01000000
#define	SHMASK2_TE						0x02000000
#define	SHMASK2_Trailer					0x04000000
#define	SHMASK2_Accept_Charset			0x08000000
#define	SHMASK2_Accept_Encoding			0x10000000
#define	SHMASK2_Cache_Control1_1_4		0x20000000
#define	SHMASK2_Content_Range			0x40000000
#define	SHMASK2_X_Wap_Tod				0x80000000

#define	SHMASK3_Content_ID				0x00000001
#define	SHMASK3_Set_Cookie				0x00000002
#define	SHMASK3_Cookie					0x00000004
#define	SHMASK3_Encoding_Version		0x00000008
//for Ver1.4
#define	SHMASK3_Profile_Warning			0x00000010
#define	SHMASK3_Content_Disposition		0x00000020
#define	SHMASK3_X_WAP_Security			0x00000040
#define	SHMASK3_Cache_Control			0x00000080
//xyg_2004_10_27_added  maybe wrong
#define	SHMASK3_Proxy_Connection		0x00000100
#define	SHMASK3_X_Wap_Proxy_Set_Cookie	0x00000200


//
#define	SHDRS_EXIST_PAPPID(ph)			((ph)->stMask.dwMask2 & SHMASK2_X_Wap_Application_Id)

//
typedef	struct	_WSPHDR_MASK
{
	DWORD			dwMask1;
	DWORD			dwMask2;
	DWORD			dwMask3;

} WSPHDR_MASK;


// ***********************************************************
// Header Value 定义
// ***********************************************************


// ********************************
// Accept_Ranges value 定义
// ********************************
#define	SAccept_Ranges_None					0x80
#define	SAccept_Ranges_Bytes				0x81



typedef	struct	_CONTENT_RANGE
{
	DWORD			dwPosFirstByte;
	DWORD			dwLenEntity;
	BOOL			fUnknowLenEntity;

} CONTENT_RANGE;
//dwLenEntity of CONTENT_RANGE
#define	UNKNOW_LENENTITY					128


typedef	struct	_ENCODING_VER
{
	SHORTINT		bCodePage;
	SHORT_STR		stVer;

} ENCODING_VER;


typedef	struct	_ACCEPT_TYPE
{
	INTEGER_STR			stType;

} ACCEPT_TYPE, *LPACCEPT_TYPE;

// ***********************************************************
// Header Field struct 定义
// ***********************************************************

typedef	struct	_WSP_HEADERS
{
	WSPHDR_MASK		stMask;
	//DWORD			dwLenHdr;
	WORD			wFlag;
	WORD			wXXX;

	//for Ver1.1
	ACCEPT_TYPE			Accept					;
	ACCEPT_CHARSET	stAccept_Charset1		; //Constrained-charset | Accept-charset-general-form
	//UNTYPE			Accept_Encoding1		;
	ACCEPT_LANGUAGE	stAccept_Language	; //Constrained-language | Accept-language-general-form
	SHORT_STR		stAccept_Ranges			; //(None | Bytes | Token_text)
	DWORD			dwAge					; //Delta_seconds_value
	SHORTINT		bAllow					; //Well_known_method
	//UNTYPE			Authorization			;
	//UNTYPE			Cache_Control1			;
	SHORT_STR		stConnection			; //(Close | Token_text)
	//UNTYPE		Content_Base1			; //deprecated.
	SHORT_STR		stContent_Encoding		; //(Gzip | Compress | Deflate | Token_text)
	INTEGER_STR		stContent_Language		; //(Well_known_language | Token_text)
	DWORD			dwContent_Length		; //Integer_value
	LPSTR			pszContent_Location		; //Uri_value
	LENGTH_BUF		stContent_MD5			; //Value_length Digest
	CONTENT_RANGE	stContent_Range1		; //Value_length First_byte_pos Entity_length
	CONTENT_TYPE	stContent_Type			; //Content_type_value
	DWORD			dwDate					; //Date_value
	LPSTR			pszEtag					; //Text_string
	DWORD			dwExpires				; //Date_value
	LPSTR			pszFrom					; //Text_string
	LPSTR			pszHost					; //Text_string
	DWORD			dwIf_Modified_Since		; //Date_value
	LPSTR			pszIf_Match				; //Text_string
	LPSTR			pszIf_None_Match		; //Text_string
	LONG_STR		stIf_Range				; //Text_string | Date_value
	DWORD			dwIf_Unmodified_Since	; //Date_value
	LPSTR			pszLocation				; //Uri_value
	//DWORD			dwLast_Modified			; //Date_value
	//DWORD			dwMax_Forwards			; //Integer_value
	//UNTYPE			Pragma					;
	//UNTYPE			Proxy_Authenticate		;
	//UNTYPE			Proxy_Authorization		;
	//UNTYPE			Public					;
	//UNTYPE			Range					; //Value_Length (Byte_range_spec | Suffix_byte_range_spec)
	LPSTR			pszReferer				; //Uri_value
	//UNTYPE			Retry_After				;
	LPSTR			pszServer				; //Text_string
	SHORT_STR		stTransfer_Encoding		; //Chunked | Token_text
	//UNTYPE			Upgrade					;
	LPSTR			pszUser_Agent			; //Text_string
	//UNTYPE			Vary					;
	LPSTR			pszVia					; //Text_string
	//UNTYPE			Warning					;
	//UNTYPE			WWW_Authenticate		;
	//UNTYPE			Content_Disposition1	;

	//for Ver1.2
	INTEGER_STR		stX_Wap_Application_Id	; //Uri_value | App_assigned_code
	LPSTR			pszX_Wap_Content_URI	; //Uri-value
	LPSTR			pszX_Wap_Initiator_URI	; //Uri_value
	INTEGER_STR		stAccept_Application	; //Any-application | Application-id-value
	UNTYPE			Bearer_Indication		;
	SHORTINT		bPush_Flag				; //Short-integer
	LPSTR			pszProfile				; //Uri-value
	UNTYPE			Profile_Diff			;
	UNTYPE			Profile_Warning1		;

	//for Ver1.3
	UNTYPE			Expect					;
	UNTYPE			TE						;
	UNTYPE			Trailer					;
	ACCEPT_CHARSET	stAccept_Charset		; //Constrained-charset | Accept-charset-general-form
	UNTYPE			Accept_Encoding			;
	UNTYPE			Cache_Control1_1_4		;
	CONTENT_RANGE	stContent_Range			; //Value_length First_byte_pos Entity_length
	UNTYPE			X_Wap_Tod				;
	LPSTR			pszContent_ID			; //Quoted-string
	//UNTYPE			Set_Cookie				;
	//UNTYPE			Cookie					;
	ENCODING_VER	stEncoding_Version		; //Version-value | Value-length Code-page [Version-value]

	//for Ver1.4
	//UNTYPE			Profile_Warning			;
	//UNTYPE			Content_Disposition		;
	//UNTYPE			X_WAP_Security			;
	//UNTYPE			Cache_Control			;

	//
	LPSTR			pszProxy_Connection;
	LPSTR			pszX_Wap_Proxy_Set_Cookie;

} WSP_HEADERS;

//wFlag
#define	SHF_NODEC			0x80


extern	BOOL	VarDec_SHeaders( DATA_DEAL* lpDealData, DWORD dwLenHeaders, OUT WSP_HEADERS* lpSHeaders );
extern	BOOL	VarEnc_SHeaders( DATA_DEAL* lpDealData, OUT DWORD* pdwLenHeaders, WSP_HEADERS* lpSHeaders );
extern	void	VarFree_SHeaders( WSP_HEADERS* lpSHeaders );

extern	BOOL	VarFind_SHeaders( DWORD dwFlag, IN OUT SHEADER_ASSIGN* lpSHeaderAssign );
#define	SHDRFIND_NAME_BYNUM		1
#define	SHDRFIND_NUM_BYNAME		2

extern	BOOL	VarIsEmpty_SHeaders( WSP_HEADERS* lpSHeaders );




#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WSP_HEADERS_H_

