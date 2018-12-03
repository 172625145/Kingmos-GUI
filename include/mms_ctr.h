/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _MMS_CTR_H_
#define _MMS_CTR_H_

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


//
#ifndef	HMMS
#define	HMMS			HANDLE
#endif

#define	LEN_MMS_MMSC			200


// --------------------------------------------------------------------------------
//
// 彩信传输操作
//
// --------------------------------------------------------------------------------

//----------
//函数 的实现
//----------

extern	HMMS	WINAPI	MCtr_Open( BOOL fCallWapBase, OUT ERROR_INFO* pInfoErr );
extern	void	WINAPI	MCtr_Stop( HMMS hMCtr );
extern	void	WINAPI	MCtr_Close( HMMS hMCtr );
extern	BOOL	WINAPI	MCtr_SetStatusCallback( HMMS hMClt, HWND hWnd, DWORD dwMessage, DWORD dwContext );

extern	BOOL	WINAPI	MCtr_Retrieval( HMMS hMCtr, LPSTR pszUri, OUT HANDLE* phPktMCtr, OUT ERROR_INFO* pInfoErr );
extern	BOOL	WINAPI	MCtr_PostReq( HMMS hMCtr, BYTE bMsgType_Waited, LPBYTE pBufReq, DWORD dwLenReq, OUT HANDLE* phPktMCtr, OUT ERROR_INFO* pInfoErr );

extern	BOOL	WINAPI	MCtr_LookPacket( HANDLE hPktMCtr, OUT DATA_DEAL* lpDealData, OUT ERROR_INFO* pInfoErr );
extern	void	WINAPI	MCtr_LookFree( HANDLE hPktMCtr );
extern	BOOL	WINAPI	MCtr_GetPacket( HANDLE hPktMCtr, OUT LPBYTE lpBody, IN OUT DWORD* pdwLenBody, OUT ERROR_INFO* pInfoErr );
extern	void	WINAPI	MCtr_FreePacket( HANDLE hPktMCtr );

//
extern	BOOL	WINAPI	MCtr_GetSC( LPSTR pszMmsSC, DWORD dwLen );
extern	BOOL	WINAPI	MCtr_SetSC( LPCSTR pszMmsSC );

extern	BOOL	WINAPI	MPush_SetCallback( DWORD dwFlag, HWND hWnd, DWORD dwMessage );

//----------
//函数 的宏替代
//----------

#define	MCtrOpen				MCtr_Open
#define	MCtrStop				MCtr_Stop
#define	MCtrClose				MCtr_Close
#define	MCtrSetStatusCallback	MCtr_SetStatusCallback

#define	MCtrRetrieval			MCtr_Retrieval
#define	MCtrPostReq				MCtr_PostReq

#define	MCtrLookPacket			MCtr_LookPacket
#define	MCtrLookFree			MCtr_LookFree
#define	MCtrGetPacket			MCtr_GetPacket
#define	MCtrFreePacket			MCtr_FreePacket

#define	MCtrGetSC				MCtr_GetSC
#define	MCtrSetSC				MCtr_SetSC

#define	MPushSetCallback		MPush_SetCallback



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_MMS_CTR_H_

