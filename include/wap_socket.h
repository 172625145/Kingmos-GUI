/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WAP_SOCKET_H_
#define _WAP_SOCKET_H_

#ifndef _WSP_VAR_TYPE_H_
#include "wsp_vartype.h"
#endif

#ifndef _WSP_CONTENT_TYPE_H_
#include "wsp_contenttype.h"
#endif

#ifndef _WSP_CAPS_H_
#include "wsp_caps.h"
#endif

#ifndef _ERROR_INFO_H_
#include "errinfo.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//
#define	HWSP					HANDLE
#define	LEN_WSP_UA				128
#define	LEN_WSP_USER			64
#define	LEN_WSP_PASSWORD		128


//WAP 端口定义 -- \RFC_IANA\IANA\assignments\port-numbers-old
#define IPPORT_WAP_WSP			9200
#define IPPORT_WAP_WSP_WTP		9201
#define IPPORT_WAP_WSP_S		9202
#define IPPORT_WAP_WSP_WTP_S	9203

#define IPPORT_WAP_VCARD		9204
#define IPPORT_WAP_VCAL			9205
#define IPPORT_WAP_VCARD_S		9206
#define IPPORT_WAP_VCAL_S		9207


//wCnnMode of WspConnect function
#define	SCNNF_UINTCALL			0x0000
#define	SCNNF_ISCNNMODE			0x0001
#define	SCNNF_ISTLSMODE			0x0002
#define	SCNNF_SPECPORT			0x0004
#define	SCNNF_CNNPARAM			(SCNNF_ISCNNMODE |SCNNF_ISTLSMODE |SCNNF_SPECPORT)
#define	SCNNF_ISCNNED			0x0008



//----------
//函数 的实现
//----------

extern	BOOL	WINAPI	Wsp_GetPPG( OUT DWORD* pdwIP_PPG, OUT WORD* pwPort_PPG );
extern	BOOL	WINAPI	Wsp_SetPPG( DWORD dwIP_PPG, WORD wPort_PPG );
extern	BOOL	WINAPI	Wsp_SubmitPort( DWORD dwPorts, DWORD dwFragment, LPBYTE lpData, DWORD dwLen );
extern	BOOL	WINAPI	Wsp_ReadPort( DWORD dwPortsID, LPBYTE lpData, DWORD* pdwLen );

extern	HWSP	WINAPI	Wsp_Open( LPCSTR pszUA, DWORD dwFlag, OUT ERROR_INFO* pInfoErr );
extern	void	WINAPI	Wsp_CloseHandle( HWSP hWsp );
extern	BOOL	WINAPI	Wsp_SetStatusCallback( HWSP hWsp, HWND hWnd, DWORD dwMessage );
extern	HWSP	WINAPI	Wsp_Connect( HWSP hWspOpen, WORD wCnnMode, LPCSTR pszUser, LPCSTR pszPassword, LPBYTE lpSHeaders, DWORD dwLenSHeaders, DWORD dwContext, OUT ERROR_INFO* pInfoErr );
extern	HWSP	WINAPI	Wsp_OpenUri( HWSP hWspOpen, WORD wCnnMode, BYTE bPduType, LPCSTR pszUri, INTEGER_STR* lpCntType, DWORD dwFlags, DWORD dwContext, OUT ERROR_INFO* pInfoErr );
extern	HWSP	WINAPI	Wsp_OpenRequest( HWSP hWspCnn, BYTE bPduType, LPCSTR pszUri, INTEGER_STR* lpCntType, DWORD dwFlags, DWORD dwContext, OUT ERROR_INFO* pInfoErr );

extern	BOOL	WINAPI	Wsp_AddRequestHeaders( HWSP hWspReq, LPBYTE lpSHeaders, DWORD dwLenSHeaders, DWORD dwModifiers, OUT ERROR_INFO* pInfoErr );
extern	BOOL	WINAPI	Wsp_SendRequest( HWSP hWspReq, LPBYTE lpSHeaders, DWORD dwLenSHeaders, LPBYTE lpBody, DWORD dwLenBody, OUT ERROR_INFO* pInfoErr );
extern	BOOL	WINAPI	Wsp_ReadStatus( HWSP hWspReq, DWORD dwWait, OUT DWORD* pdwStatus, OUT INTEGER_STR* lpCntType, IN OUT DWORD* pdwLenContentType, OUT DWORD* lpdwIndex, OUT ERROR_INFO* pInfoErr );
extern	BOOL	WINAPI	Wsp_ReadBody( HWSP hWspReq, OUT LPBYTE lpBody, IN OUT DWORD* pdwLenBody, OUT DWORD* lpdwIndex, OUT ERROR_INFO* pInfoErr );
extern	BOOL	WINAPI	Wsp_QueryInfo( HWSP hWspReq, DWORD dwInfoLevel, OUT LPVOID lpBuffer, OUT DWORD* lpdwLenBuf, OUT DWORD* lpdwIndex, OUT ERROR_INFO* pInfoErr );

extern	BOOL	WINAPI	Wsp_QueryOption( HWSP hWsp, DWORD dwOption, OUT LPVOID lpBuffer, IN OUT DWORD* lpdwBufferLength, OUT ERROR_INFO* pInfoErr );
extern	BOOL	WINAPI	Wsp_GetCap( HWSP hWsp, OUT LPBYTE lpSCapabilities, DWORD* lpdwLenSCap );
extern	BOOL	WINAPI	Wsp_SetCap( HWSP hWsp, LPBYTE lpSCapabilities, DWORD dwLenSCap );



//----------
//函数 的宏替代
//----------

#define	WspSubmitPort			Wsp_SubmitPort
#define	WspReadPort				Wsp_ReadPort


#define	WspOpen					Wsp_Open
#define	WspCloseHandle			Wsp_CloseHandle
#define	WspSetStatusCallback	Wsp_SetStatusCallback
#define	WspConnect				Wsp_Connect
#define	WspOpenUri				Wsp_OpenUri
#define	WspOpenRequest			Wsp_OpenRequest

#define	WspAddRequestHeaders	Wsp_AddRequestHeaders
#define	WspSendRequest			Wsp_SendRequest
#define	WspReadStatus			Wsp_ReadStatus
#define	WspReadBody				Wsp_ReadBody
#define	WspQueryInfo			Wsp_QueryInfo

#define	WspSetCap				Wsp_SetCap
#define	WspGetCap				Wsp_GetCap
#define	WspQueryOption			Wsp_QueryOption




#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WAP_SOCKET_H_

