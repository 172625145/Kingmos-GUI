/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _MMS_CLT_H_
#define _MMS_CLT_H_

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

#ifndef _MMS_ENC_CLT_H_
#include "mms_enc_clt.h"
#endif

#ifndef _ERROR_INFO_H_
#include "errinfo.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// --------------------------
//
// 彩信传输---客户端 的定义
//
// --------------------------


// ***********************************************************
// 通用 类型 的定义
// ***********************************************************

//
#ifndef	HMMS
#define	HMMS			HANDLE
#endif

#ifndef	LEN_MMS_MMSC
#define	LEN_MMS_MMSC				200
#endif

//dwMask
#define	MCLT_MASK_From				0x80000000
#define	MCLT_MASK_To				0x40000000
#define	MCLT_MASK_Cc				0x20000000
#define	MCLT_MASK_Bcc				0x10000000
#define	MCLT_MASK_Subject			0x08000000
#define	MCLT_MASK_Date				0x08000000

#define	MCLT_MASK_MClass			0x04000000
#define	MCLT_MASK_Priority			0x02000000
#define	MCLT_MASK_Expiry			0x01000000

#define	MCLT_MASK_Response_Status	0x00800000

#define	MCLT_MASK_CntType			0x00400000
#define	MCLT_MASK_CntLocation		0x00200000
#define	MCLT_MASK_Body				0x00100000

#define	MCLT_MASK_MSize				0x00080000

//通用HEADER
typedef	struct	_MCLT_GEN_HDR
{
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M

} MCLT_GEN_HDR;

// ***********************************************************
// 发送 结构 的定义
// ***********************************************************

// 
typedef	struct	_MCLT_SEND_REQ
{
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M

	SHORT_A_ENCSTR		stFrom;						//M
	ENCODED_STR			stTo;						//O1
	ENCODED_STR			stCc;						//O1
	ENCODED_STR			stBcc;						//O1
	ENCODED_STR			stSubject;					//O

	CONTENT_TYPE		stContent_Type;				//M

	//MMS body data
	GEN_BODY			stBodyGen;

} MCLT_SEND_REQ;
// 注释：
// MClt模块 肯定要补充的字段有： MTID / MVer / Date 


//
typedef	struct	_MCLT_SEND_CONF
{
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M
	SHORTINT			bX_Mms_Response_Status;		//M

	LPSTR				pszMessage_ID;				//O--xyg: MMS UA 不用显示该项，这是为了与更多功能兼容

} MCLT_SEND_CONF;

//
typedef	struct	_MCLT_DELIVERY_IND
{
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M
	LPSTR				pszMessage_ID;				//M
	ENCODED_STR			stTo;						//M
	DWORD				dwDate;						//M
	SHORTINT			bX_Mms_MM_State;			//M

} MCLT_DELIVERY_IND;


// ***********************************************************
// 通知和接收 结构 的定义
// ***********************************************************

//
typedef	struct	_MCLT_NOTIFICATION_IND
{
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M

	SHORT_A_ENCSTR		stFrom;						//O
	ENCODED_STR			stSubject;					//O
	SHORT_STR			stX_Mms_Message_Class;		//M
	SHORTINT			bX_Mms_Priority;			//O

	DWORD				dwX_Mms_Message_Size;		//M
	DATE_A_DELTASEC		stX_Mms_Expiry;				//M

	LPSTR				pszX_Mms_Content_Location;	//M

} MCLT_NOTIFICATION_IND;


//
typedef	struct	_MCLT_RETRIEVE_CONF
{
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M
	LPSTR				pszMessage_ID;				//C--xyg: MMS UA 不用显示该项，这是为了与更多功能兼容
	DWORD				dwDate;						//M
	SHORT_A_ENCSTR		stFrom;						//O
	ENCODED_STR			stTo;						//O
	ENCODED_STR			stCc;						//O
	ENCODED_STR			stSubject;					//O
	SHORT_STR			stX_Mms_Message_Class;		//O
	SHORTINT			bX_Mms_Priority;			//O

	CONTENT_TYPE		stContent_Type;				//M

	//MMS body data
	GEN_BODY			stBodyGen;

} MCLT_RETRIEVE_CONF;



// ***********************************************************
// 各种类型 的PDU定义 和 操作函数
// ***********************************************************

//
typedef	union	_MCLT_PDU
{
	MCLT_GEN_HDR			HdrGen;

	//发送、确认、报告
	MCLT_SEND_REQ			Send_Req;
	MCLT_SEND_CONF			Send_Conf;
	MCLT_DELIVERY_IND		Delivery_Ind;

	//通知、接收内容
	MCLT_NOTIFICATION_IND	Notification_Ind;
	MCLT_RETRIEVE_CONF		Retrieve_Conf;

} MCLT_PDU;


//----------
//函数 的实现
//----------

//TR 传输机制
extern	HMMS	MClt_Open( OUT ERROR_INFO* pInfoErr );
extern	void	MClt_Close( HMMS hMClt );
extern	void	MClt_Stop( HMMS hMClt );
extern	BOOL	MClt_SetStatusCallback( HMMS hMClt, HWND hWnd, DWORD dwMessage, DWORD dwContext );

extern	BOOL	MClt_SetField( HMMS hMClt, LPVOID pBuffer );

extern	BOOL	MClt_Retrieval( HMMS hMClt, LPSTR pszUri, OUT MCLT_RETRIEVE_CONF* pPduMCltConf, OUT ERROR_INFO* pInfoErr );
extern	BOOL	MClt_SendReq( HMMS hMClt, MCLT_SEND_REQ* pPduMCltReq, OUT MCLT_SEND_CONF* pPduMCltConf, OUT ERROR_INFO* pInfoErr );
extern	BOOL	MClt_SendReq2( HMMS hMClt, LPCTSTR pszFileMMS, MCLT_SEND_REQ* pPduMCltReq, OUT MCLT_SEND_CONF* pPduMCltConf, OUT ERROR_INFO* pInfoErr );

extern	void	MClt_FreePdu( MCLT_PDU* pPduMClt );
extern	BOOL	MClt_PackPdu( HMMS hMClt, OUT DATA_DEAL* lpDealData, MCLT_PDU* pPduMClt, OUT ERROR_INFO* pInfoErr );


//Push 通知机制---(注释：可以不用 hMClt 句柄---意味着APP不能打断操作)
extern	BOOL	MClt_SetPushCallback( HWND hWnd, DWORD dwMessage );
extern	BOOL	MClt_ReadNotifyInd( DWORD dwDataID, OUT MCLT_NOTIFICATION_IND* pPduMCltInd, OUT ERROR_INFO* pInfoErr );
extern	BOOL	MClt_ReadMmsPush( DWORD dwDataID, OUT MCLT_PDU* pPduMCltInd, OUT ERROR_INFO* pInfoErr );


//----------
//函数 的宏替代
//----------

//
#define	MCltOpen				MClt_Open
#define	MCltClose				MClt_Close
#define	MCltStop				MClt_Stop
#define	MCltSetStatusCallback	MClt_SetStatusCallback

#define	MCltSetField			MClt_SetField

#define	MCltRetrieval			MClt_Retrieval
#define	MCltSendReq				MClt_SendReq
#define	MCltSendReq2			MClt_SendReq2

#define	MCltFreePdu				MClt_FreePdu
#define	MCltPackPdu				MClt_PackPdu


//
#define	MCltSetPushCallback		MClt_SetPushCallback
#define	MCltReadNotifyInd		MClt_ReadNotifyInd
#define	MCltReadMmsPush			MClt_ReadMmsPush

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_MMS_CLT_H_

