/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WSP_PDU_H_
#define _WSP_PDU_H_

#ifndef _WSP_HEADERS_H_
#include "wsp_hdrs.h"
#endif

#ifndef _WSP_CAPS_H_
#include "wsp_caps.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ***********************************************************
// PDU 类型 定义
// ***********************************************************
// WSP TABLE 34
//#define	SPDU_Reserved			0x00
#define	SPDU_Connect			0x01
#define	SPDU_ConnectReply		0x02
#define	SPDU_Redirect			0x03
#define	SPDU_Reply				0x04
#define	SPDU_Disconnect			0x05

#define	SPDU_Push				0x06
#define	SPDU_ConfirmedPush		0x07
#define	SPDU_Suspend			0x08
#define	SPDU_Resume				0x09
//#define	SPDU_Unassigned 0x10–0x3F
#define	SPDU_Get				0x40
#define	SPDU_Options			0x41// (Get PDU)
#define	SPDU_Head				0x42// (Get PDU) 
#define	SPDU_Delete				0x43// (Get PDU) 
#define	SPDU_Trace				0x44// (Get PDU)
//#define	SPDU_Unassigned (Get PDU) 0x45_0x4F
//#define	SPDU_Extended Method (Get PDU) 0x50_0x5F
#define	SPDU_Post				0x60
#define	SPDU_Put				0x61// (Post PDU)
//#define	SPDU_Unassigned (Post PDU) 0x62–0x6F
//#define	SPDU_Extended Method (Post PDU) 0x70_0x7F
#define	SPDU_Data				0x80// Fragment PDU 
//#define	SPDU_Reserved 0x81_0xFF


#define	SPDU_MAINTYPE(b)		(b & 0xF0)
#define	SPDU_SUBTYPE(b)			(b & 0x0F)


// ***********************************************************
// bVersion 定义
// ***********************************************************
#define	WSP_VER_MAKE(a,b)			( ((a<<4) & 0xF0) | (b & 0x0F) )


// ***********************************************************
// Capability Assignments 定义
// ***********************************************************

#define	SCAP_Client_SDU_Size		0x00	//uintvar
#define	SCAP_Server_SDU_Size		0x01	//uintvar
#define	SCAP_Protocol_Options		0x02
#define	SCAP_Method_MOR				0x03	//uint8
#define	SCAP_Push_MOR				0x04	//uint8
#define	SCAP_Extended_Methods		0x05
#define	SCAP_Header_Code_Pages		0x06
#define	SCAP_Aliases				0x07
#define	SCAP_Client_Message_Size	0x08	//uintvar
#define	SCAP_Server_Message_Size	0x09	//uintvar
//#define	SCAP_Unassigned 0x0A to 0x7F


// ***********************************************************
// 通用 类型 的PDU定义
// ***********************************************************

//通用MASK
#define	WSP_GEN_MASK_URILEN					0x80000000
#define	WSP_GEN_MASK_URI					0x40000000
#define	WSP_GEN_MASK_HDRLEN					0x20000000
#define	WSP_GEN_MASK_CNTTYPE				0x10000000
#define	WSP_GEN_MASK_HEADERS				0x08000000
#define	WSP_GEN_MASK_BODY					0x04000000
#define	WSP_GEN_MASK_VERSION				0x02000000
#define	WSP_GEN_MASK_CAPLEN					0x01000000
#define	WSP_GEN_MASK_CAP					0x00800000
#define	WSP_GEN_MASK_SSID					0x00400000


//通用HEADER
typedef	struct	_WSP_GEN_HDR
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;  // 是否是要求连接状态
	BYTE				bTID; //uint8

} WSP_GEN_HDR;


//通用BODY
//typedef	struct	_WSP_GEN_BODY
//{
//	LPBYTE			pBody;
//	DWORD			dwLenBody;
//
//} WSP_GEN_BODY;


// ***********************************************************
// Connect / ConnectReply / Disconnect  类型 的PDU定义
// ***********************************************************

typedef	struct	_SCONNECT_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	OCTET				bVersion; //uint8
	DWORD				dwCapabilitiesLen; //uintvar
	DWORD				dwHeadersLen; //uintvar
	SCAPABILITIES		stCapabilities;
	WSP_HEADERS			stSHeaders;

} SCONNECT_PDU;


typedef	struct	_SCONNECTREPLY_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	DWORD				dwServerSessionId; //uintvar
	DWORD				dwCapabilitiesLen; //uintvar
	DWORD				dwHeadersLen; //uintvar
	SCAPABILITIES		stCapabilities;
	WSP_HEADERS			stSHeaders;

} SCONNECTREPLY_PDU;


typedef	struct	_SDISCONNECT_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	DWORD				dwServerSessionId; //uintvar

} SDISCONNECT_PDU;

// ***********************************************************
// Suspend / Resume  类型 的PDU定义
// ***********************************************************

typedef	struct	_SSUSPEND_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	DWORD				dwServerSessionId; //uintvar

} SSUSPEND_PDU;


typedef	struct	_SRESUME_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	DWORD				dwServerSessionId; //uintvar
	DWORD				dwCapabilitiesLen; //uintvar
	SCAPABILITIES		stCapabilities;
	WSP_HEADERS			stSHeaders;

} SRESUME_PDU;


// ***********************************************************
// Get / Post / Reply 类型 的PDU定义
// ***********************************************************

typedef	struct	_SGET_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	DWORD				dwUriLen; //uintvar
	LPSTR				pszUri;
	WSP_HEADERS			stSHeaders;

} SGET_PDU;


typedef	struct	_SPOST_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	DWORD				dwUriLen; //uintvar
	DWORD				dwHeadersLen; //uintvar
	LPSTR				pszUri;
	CONTENT_TYPE		stContentType;
	WSP_HEADERS			stSHeaders;

	GEN_BODY			stBodyGen;

} SPOST_PDU;


typedef	struct	_SREPLY_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8
	
	BYTE				bStatus;
	DWORD				dwHeadersLen; //uintvar
	CONTENT_TYPE		stContentType;
	WSP_HEADERS			stSHeaders;

	GEN_BODY			stBodyGen;

} SREPLY_PDU;

// ***********************************************************
// Data / Push / Ack 类型 的PDU定义
// ***********************************************************

typedef	struct	_SDATA_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	DWORD				dwHeadersLen; //uintvar
	WSP_HEADERS			stSHeaders;

	GEN_BODY			stBodyGen;

} SDATA_PDU;


typedef	struct	_SPUSH_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	DWORD				dwHeadersLen; //uintvar
	CONTENT_TYPE		stContentType;
	WSP_HEADERS			stSHeaders;

	GEN_BODY			stBodyGen;

} SPUSH_PDU;


typedef	struct	_SACK_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;
	BYTE				bType; //uint8
	BYTE				bIsCnnMode;
	BYTE				bTID; //uint8

	WSP_HEADERS			stSHeaders;

} SACK_PDU;


// ***********************************************************
// 各种类型 的PDU定义 和 操作函数
// ***********************************************************

typedef	union	_WSP_PDU
{
	WSP_GEN_HDR			SHdrGen;
	
	SCONNECT_PDU		SConnect;
	SCONNECTREPLY_PDU	SConnectReply;
	SDISCONNECT_PDU		SDisconnect;
	
	SSUSPEND_PDU		SSuspend;
	SRESUME_PDU			SResume;
	
	SDATA_PDU			SData;
	SPUSH_PDU			SPush;
	SACK_PDU			SAck;
	
	SGET_PDU			SGet;
	SPOST_PDU			SPost;
	SREPLY_PDU			SReply;

} WSP_PDU;


extern	BOOL	Wsp_UnpackType( DATA_DEAL* lpDealData, BYTE bIsCnnMode, BYTE* pbWspType );
extern	BOOL	Wsp_UnpackPdu( DATA_DEAL* lpDealData, OUT WSP_PDU* pPduWsp );
extern	void	Wsp_FreePduUnpack( WSP_PDU* pPduWsp );

extern	BOOL	Wsp_PackPdu( OUT DATA_DEAL* lpDealData, BOOL fAllocData, WSP_PDU* pPduWsp );



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WSP_PDU_H_
