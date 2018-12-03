/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _INET_DEF_H_
#define _INET_DEF_H_

#include "handle_check.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//---------------------------------------------------
//宏 定义
//---------------------------------------------------

#define DEBUG_INET 1

//bHandleType
#define	INETHTYPE_OPEN				1
#define	INETHTYPE_CNN				2
#define	INETHTYPE_REQ				3
#define	INETHTYPE_CNNREQ			4
#define	INETHTYPE_CNNREQ2			5

#define	INETH_ISOPEN(lp)			(lp->bHandleType==INETHTYPE_OPEN)
#define	INETH_ISCNN(lp)				(lp->bHandleType==INETHTYPE_CNN)
#define	INETH_ISREQ(lp)				((lp->bHandleType==INETHTYPE_REQ) || (lp->bHandleType==INETHTYPE_CNNREQ2))
#define	INETH_ISREQ_1(lp)			(lp->bHandleType==INETHTYPE_REQ)
#define	INETH_ISREQ_2(lp)			(lp->bHandleType==INETHTYPE_CNNREQ2)
#define	INETH_ISCNNREQ(lp)			(lp->bHandleType==INETHTYPE_CNNREQ)

#define	INETH_NOTOPEN(lp)			(lp->bHandleType!=INETHTYPE_OPEN)
#define	INETH_NOTCNN(lp)			(lp->bHandleType!=INETHTYPE_CNN)
#define	INETH_NOTREQ(lp)			((lp->bHandleType!=INETHTYPE_REQ) && (lp->bHandleType!=INETHTYPE_CNNREQ2))
#define	INETH_NOTREQ_1(lp)			(lp->bHandleType!=INETHTYPE_REQ)
#define	INETH_NOTREQ_2(lp)			(lp->bHandleType!=INETHTYPE_CNNREQ2)
#define	INETH_NOTCNNREQ(lp)			(lp->bHandleType!=INETHTYPE_CNNREQ)


//wFlag
#define	INETF_EXIT					0x8000

#define	INETG_CHECK_OK(lp)			( HANDLE_CHECK(lp) && !(lp->wFlag & INETF_EXIT) )
#define	INETG_CHECK_FAIL(lp)		( !HANDLE_CHECK(lp) || (lp->wFlag & INETF_EXIT) )

#define	INETG_ISEXIT(lp)			( lp->wFlag & INETF_EXIT )

//
#define	SOPEN_CHECK_FAIL(lp)		( INETG_CHECK_FAIL(lp) || INETH_NOTOPEN(lp) )
#define	SCNN_CHECK_FAIL(lp)			( INETG_CHECK_FAIL(lp) || INETH_NOTCNN(lp) )
#define	SREQ_CHECK_FAIL(lp)			( INETG_CHECK_FAIL(lp) || INETH_NOTREQ(lp) )

//
#define	INETCMM_CHECK_FAIL(lp,bb)	( INETG_CHECK_FAIL(lp) || (lp->bHandleType!=bb) )


//
//
#define	INETE_SETERR2(lp,dT,dV)		{lp->stInfoErr.dwType=dT;lp->stInfoErr.dwValue=dV;}
#define	INETE_SETERR_GEN(lp,dV)		{lp->stInfoErr.dwType=(ERR_TYPE_GENERAL|ERR_TYPE_GENERAL);lp->stInfoErr.dwValue=dV;}

//
#define	LEN_HDRRESP					2048


//---------------------------------------------------
//INET REQ 信息
//---------------------------------------------------

typedef	struct	_INET_COMM
{
	HANDLE_THIS( _INET_COMM );

	//句柄类型、操作标志、回调信息
	BYTE				bHandleType;
	BYTE				bXXXXX;
	WORD				wFlag;
	HWND				hWndStatus;
	DWORD				dwContext;

	//链表信息
	LIST_UNIT			hListComm;

	//
	DWORD				dwData;

} INET_COMM;


//---------------------------------------------------
//INET OPEN 信息
//---------------------------------------------------
typedef	struct	_INET_OPEN
{
	HANDLE_THIS( _INET_OPEN );

	//句柄类型、操作标志、回调信息
	BYTE				bHandleType; //INETHTYPE_OPEN
	BYTE				bXXXXX;
	WORD				wFlag;
	HWND				hWndStatus;
	DWORD				dwContext;
	//链表信息
	LIST_UNIT			hListOpen;

	//链表信息
	CRITICAL_SECTION	csListCnn;
	LIST_UNIT			hListCnn;
	//
	HANDLE				hProc;
	ERROR_INFO			stInfoErr;

	//
	DWORD				dwAccessType;
	char				pszAgent[MAX_USRAGENT+4];
	char				pszProxy[MAX_PROXY+4];

} INET_OPEN;


//---------------------------------------------------
//INET CNN 信息
//---------------------------------------------------
typedef	struct	_INET_CNN
{
	HANDLE_THIS( _INET_CNN );

	//句柄类型、操作标志、回调信息
	BYTE				bHandleType; //INETHTYPE_CNN
	BYTE				bXXXXX;
	WORD				wFlag;
	HWND				hWndStatus;
	DWORD				dwContext;
	//链表信息
	LIST_UNIT			hListCnn;

	//
	INET_OPEN*			pINetOpen;
	//链表信息
	CRITICAL_SECTION	csListReq;
	LIST_UNIT			hListReq;

	//Server Name and IP
	DWORD				dwSrvFlags; //attention
	DWORD				dwSrvType;
	DWORD				dwSrvIP;
	WORD				wSrvPort;
	WORD				wXXX;
	char				pszSrvName[MAX_SRVNAME+4];
	char				pszUserName[MAX_INETUSR+4];
	char				pszPassword[MAX_INETPW+4];

} INET_CNN;


//---------------------------------------------------
//INET REQ 信息
//---------------------------------------------------
typedef	struct	_HTTP_REQ
{
	HANDLE_THIS( _HTTP_REQ );

	//句柄类型、操作标志、回调信息
	BYTE				bHandleType; //INETHTYPE_REQ
	BYTE				bXXXXX;
	WORD				wFlag;
	HWND				hWndStatus;
	DWORD				dwContext;
	//链表信息
	LIST_UNIT			hListReq;

	//
	INET_CNN*			pINetCnn;

	//保存 发送信息
	char				szVerb[MAX_HTTP_VERB+4];
	char				szPath[MAX_HTTP_PATH+4];
	DWORD				dwLenPath;
	char				szVersion[MAX_HTTP_VERSION+4];
	LPSTR				pszRefer;
	DWORD				dwLenRefer;
	LPCSTR				pszAccept;

	//保存 操作信息
	CRITICAL_SECTION	csReq;
	DWORD				dwRef;
	SOCKET				skReq;

	//
	//保存 应答信息
	//
	LPBYTE				pBufRespData;
	DWORD				dwLenRespData;
	DWORD				dwLenRespData_Offset;
	DWORD				dwLenRespData_Read;

	DWORD				dwLenRespHdr;
	LPBYTE				pBufRespHdr;

	//DWORD				dwMaskRespHdr;
	DWORD				dwStatusHttp;
	DWORD				dwFld_ContentLen;

} HTTP_REQ;

//wFlag
#define	IREQF_CNN					0x0001
#define	IREQF_HDRRESP				0x0002
#define	IREQF_SENTREQ				0x0004


//dwMaskRespHdr
#define	HTTPR_CONTENT_LENGTH				0x00000001



//---------------------------------------------------
//INET CNNREQ 信息
//---------------------------------------------------
typedef	struct	_HTTP_CNNREQ
{
	INET_CNN			stCnnINet; //INETHTYPE_CNNREQ
	HTTP_REQ			stReqHttp; //INETHTYPE_CNNREQ2

} HTTP_CNNREQ;

//---------------------------------------------------
//INET 全局信息
//---------------------------------------------------
typedef	struct	_INET_GLOBAL
{
	HANDLE_THIS( _INET_GLOBAL );

	//链表信息
	CRITICAL_SECTION	csListOpen;
	LIST_UNIT			hListOpen;

	//信息
	WORD				wFlag;
	WORD				wXXX;

} INET_GLOBAL;

extern	INET_GLOBAL*		g_lpGlobalINet;

/////////////////
extern	LPVOID	INetHlp_OpenCnn( HINTERNET hInetOpen, DWORD dwSize, LPCSTR lpszSrvName, WORD wSrvPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwSrvType, DWORD dwSrvFlags, DWORD dwContext );

extern	BOOL	INetHlp_LookUrl( LPCSTR pszScheme2, DWORD dwLenScheme2, LPCSTR pszUrl, OUT WORD* pwSrvPort, 
						OUT LPSTR pszSrvName, DWORD dwLenSrv, OUT LPSTR pszPath, DWORD dwLenPath );

extern	BOOL	INetHlp_ConnectSrv( WORD* pwFlag, SOCKET skReq, LPCSTR pszSrvName, WORD wSrvPort, IN OUT DWORD* pdwSrvIP, HWND hWndStatus, DWORD dwContext );
extern	DWORD	INetHlp_WaitData( SOCKET skReq, DWORD dwWaitSec, OUT DWORD* pdwLenRecv );
extern	DWORD	INetHlp_RecvData( SOCKET skReq, DWORD dwWaitSec, LPBYTE pBufRecv, DWORD dwLenBuf, OUT DWORD* pdwLenRecv );

extern	void	INetHlp_CloseCnn( INET_CNN* pINetCnn, BOOL fRemove );


extern	void	HttpHlp_CloseReq( HTTP_REQ* pReqHttp );
extern	BOOL	HttpHlp_ReadFile( HTTP_REQ* pReqHttp, LPVOID lpBuffer, DWORD dwBytesToRead, OUT LPDWORD lpdwBytesRead );
extern	BOOL	HttpHlp_SaveFile( HTTP_REQ* pReqHttp, HANDLE hFileSav, OUT LPDWORD lpdwBytesRead );


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_INET_DEF_H_
