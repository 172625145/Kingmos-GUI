/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：http
版本号：  1.0.0
开发时期：2004-05-31
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <internet.h>
#include <pcfile.h>
//"\inc_wap"
#include <varbuf.h>
//"\inc_local"
#include "skt_call.h"
#include "inet_call.h"
#include "inet_def.h"


/***************  全局区 定义， 声明 *****************/

static	BOOL	HttpHlp_InitReq( HTTP_REQ* pReqHttp, INET_CNN* pINetCnn, BYTE bHandleType, LPCSTR lpszVerb, LPCSTR lpszPath, DWORD dwLenPath, LPCSTR lpszVersion, DWORD dwReqFlags, DWORD dwContext );
static	LPVOID	HttpHlp_OpenCnnReq( INET_OPEN* pINetOpen, LPCSTR lpszUrl, DWORD dwReqFlags, DWORD dwContext );

static	BOOL	HttpHlp_SendReq( HTTP_REQ* pReqHttp, LPCSTR lpszHdrs, DWORD dwLenHdrs, LPVOID lpBody, DWORD dwLenBody );
static	BOOL	HttpHlp_WaitResp( HTTP_REQ* pReqHttp );

static	void	HttpHlp_CopyStr( LPCSTR pszStr, DWORD dwLenStr, OUT LPSTR pszCopy, IN OUT DWORD* pdwLenCopy );
static	DWORD	HttpHlp_LenField( LPCSTR pszHttpField );
static	void	HttpHlp_CopyField( LPCSTR pszHttpField, DWORD dwLenSkip, OUT LPSTR pszCopy, IN OUT DWORD* pdwLenCopy );
static	BOOL	HttpHlp_FetchField( LPCSTR pBufRespHdr, LPCSTR pszFind, DWORD dwLenSkip, OUT LPSTR pszCopy, IN OUT DWORD* pdwLenCopy );

/******************************************************/


HINTERNET	WINAPI	INETAPI_InternetOpenUrl( HINTERNET hInetOpen, 
											LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, 
											DWORD dwReqFlags, DWORD dwContext )
{
	INET_OPEN*		pINetOpen = (INET_OPEN*)hInetOpen;
	HTTP_CNNREQ*	pCnnReqHttp;
	INET_CNN*		pINetCnn;
	HTTP_REQ*		pReqHttp;

	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpenUrl: enter hInetOpen=[0x%x]\r\n"), pINetOpen));
	//句柄安全检查
	if( SOPEN_CHECK_FAIL(pINetOpen) )
	{
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpenUrl: leave failed --- 1, this[0x%x] t[%d]\r\n"), pINetOpen->this, pINetOpen->bHandleType));
		return NULL;
	}
	//参数检查
	if( !lpszUrl || !*lpszUrl )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpenUrl: leave failed --- 2\r\n")));
		return NULL;
	}
	RETAILMSG(DEBUG_INET,(TEXT("  \r\n")));
	RETAILMSG(DEBUG_INET,(TEXT("  Http_OpenUrl: [%s]\r\n"), lpszUrl));
	RETAILMSG(DEBUG_INET,(TEXT("  \r\n")));

	//打开、初始化 请求
	pCnnReqHttp = (HTTP_CNNREQ*)HttpHlp_OpenCnnReq( pINetOpen, lpszUrl, dwReqFlags, dwContext );
	if( !pCnnReqHttp )
	{
		return NULL;
	}
	pINetCnn = &pCnnReqHttp->stCnnINet;
	pReqHttp = &pCnnReqHttp->stReqHttp;

	//发送请求
	if( INETAPI_HttpSendRequest( (HINTERNET)pReqHttp, lpszHdrs, dwLenHdrs, NULL, 0 ) )
	{
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpenUrl: leave ok [0x%x]\r\n"), pReqHttp));
		return (HINTERNET)pReqHttp;
	}
	else
	{
		//请删除
		INetHlp_CloseCnn( pINetCnn, TRUE );
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpenUrl: leave [0x00000]\r\n")));
		return NULL;
	}
}

HINTERNET	WINAPI	INETAPI_HttpOpenRequest( HINTERNET hInetCnn, 
											LPCSTR lpszVerb, LPCSTR lpszPath, LPCSTR lpszVersion, 
											LPCSTR lpszRefer, LPCSTR lpszAcceptTypes, 
											DWORD dwReqFlags, DWORD dwContext )
{
	INET_CNN*		pINetCnn = (INET_CNN*)hInetCnn;
	INET_OPEN*		pINetOpen;
	HTTP_REQ*		pReqHttp;
	LPSTR			pszTmp;
	DWORD			dwTmp;
	DWORD			dwLenPath;
	DWORD			dwAccessType;
	BOOL			fSuccess;

	//句柄安全检查
	if( SCNN_CHECK_FAIL(pINetCnn) || INETG_CHECK_FAIL(g_lpGlobalINet) )
	{
		return NULL;
	}
	pINetOpen = pINetCnn->pINetOpen;
	//参数检查
	if( !lpszVerb || !*lpszVerb || (strlen(lpszVerb)>MAX_HTTP_VERB) )
	{
		INETE_SETERR_GEN( pINetOpen, ERROR_INVALID_PARAMETER );
		return NULL;
	}
	if( !lpszPath || !*lpszPath || ((dwLenPath=strlen(lpszPath))>MAX_HTTP_PATH) )
	{
		INETE_SETERR_GEN( pINetOpen, ERROR_INVALID_PARAMETER );
		return NULL;
	}
	if( lpszVersion && *lpszVersion )
	{
		if( strlen(lpszVersion)>MAX_HTTP_VERSION )
		{
			INETE_SETERR_GEN( pINetOpen, ERROR_INVALID_PARAMETER );
			return NULL;
		}
	}
	else
	{
		lpszVersion = (LPCSTR)HTTP_VERSION;
	}
	dwAccessType = pINetCnn->pINetOpen->dwAccessType;

	//分配和基本初始化
	pReqHttp = (HTTP_REQ*)HANDLE_ALLOC( sizeof(HTTP_REQ) );
	if( !pReqHttp )
	{
		INETE_SETERR_GEN( pINetOpen, ERROR_NOT_ENOUGH_MEMORY );
		return NULL;
	}
	//
	if( dwAccessType==INTERNET_OPEN_TYPE_PROXY )
	{
		DWORD			dwLenSrvName;
		char			lpszPath_Srv[MAX_HTTP_PATH+4];
		
		//
		if( dwLenPath==-1 )
		{
			dwLenPath = strlen(lpszPath);
		}
		dwLenSrvName = strlen(pINetCnn->pszSrvName);
		if( (dwLenPath+dwLenSrvName)>=MAX_HTTP_PATH )
		{
			INETE_SETERR_GEN( pINetOpen, ERROR_INVALID_PARAMETER );
			HANDLE_FREE(pReqHttp);
			return NULL;
		}
		//
		memcpy( lpszPath_Srv, pINetCnn->pszSrvName, dwLenSrvName );
		memcpy( lpszPath_Srv+dwLenSrvName, lpszPath, dwLenPath );
		dwLenPath += dwLenSrvName;
		//
		fSuccess = HttpHlp_InitReq( pReqHttp, pINetCnn, INETHTYPE_REQ, lpszVerb, lpszPath_Srv, dwLenPath, lpszVersion, dwReqFlags, dwContext );
	}
	else
	{
		fSuccess = HttpHlp_InitReq( pReqHttp, pINetCnn, INETHTYPE_REQ, lpszVerb, lpszPath, dwLenPath, lpszVersion, dwReqFlags, dwContext );
	}
	//
	if( !fSuccess )
	{
		HttpHlp_CloseReq( pReqHttp );
		return NULL;
	}
	//继续 保存参数信息
	if( lpszRefer && *lpszRefer )
	{
		dwTmp = strlen(lpszRefer);
		pszTmp = (char*)malloc( dwTmp+4 );
		if( !pszTmp )
		{
			HttpHlp_CloseReq( pReqHttp );
			INETE_SETERR_GEN( pINetOpen, ERROR_NOT_ENOUGH_MEMORY );
			return NULL;
		}
		memcpy( pszTmp, lpszRefer, dwTmp );
		pszTmp[dwTmp] = 0;
		//
		pReqHttp->pszRefer = pszTmp;
		pReqHttp->dwLenRefer = dwTmp;
	}
	pReqHttp->pszAccept = lpszAcceptTypes;

	return (HINTERNET)pReqHttp;
}


BOOL		WINAPI	INETAPI_HttpAddRequestHeaders( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwModifiers )
{
	return FALSE;
}


BOOL		WINAPI	INETAPI_HttpSendRequest( HINTERNET hInetReq, 
											LPCSTR lpszHdrs, DWORD dwLenHdrs, 
											LPVOID lpBody, DWORD dwLenBody )
{
	HTTP_REQ*		pReqHttp = (HTTP_REQ*)hInetReq;
	INET_CNN*		pINetCnn;
	INET_OPEN*		pINetOpen;
	BOOL			fSuccess;

	//句柄安全检查
	if( SREQ_CHECK_FAIL(pReqHttp) )
	{
		return FALSE;
	}
	pINetCnn = pReqHttp->pINetCnn;
	if( INETG_CHECK_FAIL(pINetCnn) )
	{
		SetLastError( ERROR_INVALID_HANDLE );
		return FALSE;
	}
	pINetOpen = pINetCnn->pINetOpen;
	if( INETG_CHECK_FAIL(pINetOpen) )
	{
		SetLastError( ERROR_INVALID_HANDLE );
		return FALSE;
	}
	//参数检查

	//
	EnterCriticalSection( &pReqHttp->csReq );
	if( pReqHttp->wFlag & IREQF_SENTREQ )
	{
		//如果发送过请求，就不要再用了，请另 OpenRequest
		LeaveCriticalSection( &pReqHttp->csReq );
		return FALSE;
	}
	pReqHttp->dwRef ++;
	//连接服务器
	if( !(pReqHttp->wFlag & IREQF_CNN) )
	{
		if( INetHlp_ConnectSrv(&pReqHttp->wFlag, pReqHttp->skReq, pINetCnn->pszSrvName, pINetCnn->wSrvPort, &pINetCnn->dwSrvIP, pReqHttp->hWndStatus, pReqHttp->dwContext ) )
		{
			pReqHttp->wFlag |= IREQF_CNN;
		}
		else
		{
			pReqHttp->dwRef --;
			LeaveCriticalSection( &pReqHttp->csReq );
			return FALSE;
		}
	}
	//发送请求
	if( !HttpHlp_SendReq( pReqHttp, lpszHdrs, dwLenHdrs, lpBody, dwLenBody ) )
	{
		pReqHttp->dwRef --;
		LeaveCriticalSection( &pReqHttp->csReq );
		return FALSE;
	}
	pReqHttp->wFlag |= IREQF_SENTREQ;
	//等待应答
	fSuccess = HttpHlp_WaitResp( pReqHttp );
	//
	pReqHttp->dwRef --;
	LeaveCriticalSection( &pReqHttp->csReq );

	return fSuccess;
}

BOOL		WINAPI	INETAPI_HttpQueryQuick( HINTERNET hInetReq, 
										   OUT DWORD* pdwStatusHttp, OUT DWORD* pdwContentLen, 
										   OUT LPSTR pszContentType, DWORD dwLenBuf, 
										   IN OUT LPDWORD lpdwIndex )
{
	HTTP_REQ*		pReqHttp = (HTTP_REQ*)hInetReq;

	//句柄安全检查
	if( SREQ_CHECK_FAIL(pReqHttp) )
	{
		return FALSE;
	}
	//参数检查
	if( !lpdwIndex )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}
	if( pszContentType && !dwLenBuf )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	//
	EnterCriticalSection( &pReqHttp->csReq );
	if( !(pReqHttp->wFlag & IREQF_HDRRESP) )
	{
		//如果
		LeaveCriticalSection( &pReqHttp->csReq );
		return FALSE;
	}
	pReqHttp->dwRef ++;
	//
	*pdwStatusHttp = pReqHttp->dwStatusHttp;
	*pdwContentLen = pReqHttp->dwFld_ContentLen;
	HttpHlp_FetchField( pReqHttp->pBufRespHdr, HTTP_CONTENT_TYPE, LEN_HTTP_CONTENT_TYPE+1, pszContentType, &dwLenBuf );
	//
	pReqHttp->dwRef --;
	LeaveCriticalSection( &pReqHttp->csReq );

	return TRUE;
}


BOOL		WINAPI	INETAPI_HttpQueryInfo( HINTERNET hInetReq, 
										  DWORD dwInfoLevel, IN OUT LPVOID lpBuf, 
										  IN OUT LPDWORD lpdwLenBuf, IN OUT LPDWORD lpdwIndex )
{
	HTTP_REQ*		pReqHttp = (HTTP_REQ*)hInetReq;
	LPSTR			pszInfo = (LPSTR)lpBuf;
	DWORD			dwFormat;
	DWORD			dwFun;
	BOOL			fSuccess;

	//句柄安全检查
	if( SREQ_CHECK_FAIL(pReqHttp) )
	{
		return FALSE;
	}
	//参数检查
	dwFormat = (dwInfoLevel & HTTP_QUERY_MODIFIER_FLAGS_MASK);
	dwFun = (dwInfoLevel & HTTP_QUERY_HEADER_MASK);
	if( dwFun>HTTP_QUERY_MAX )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}
	if( !pszInfo || !lpdwLenBuf || !lpdwIndex )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}
	if( (dwFormat & HTTP_QUERY_FLAG_NUMBER) && (*lpdwLenBuf!=sizeof(DWORD)) )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	//

	//判断当前的 INDEX

	//
	EnterCriticalSection( &pReqHttp->csReq );
	pReqHttp->dwRef ++;
	//查询操作信息
	if( dwFun==HTTP_QUERY_REQUEST_METHOD )
	{
		HttpHlp_CopyField( pReqHttp->szVerb, 0, pszInfo, lpdwLenBuf );
		LeaveCriticalSection( &pReqHttp->csReq );
		return TRUE;
	}
	//
	if( !(pReqHttp->wFlag & IREQF_HDRRESP) )
	{
		LeaveCriticalSection( &pReqHttp->csReq );
		return FALSE;
	}
	//
	switch( dwFun )
	{
	//对查找到的信息，可以转化
	//case HTTP_QUERY_MIME_VERSION              :
	//	break;
	case HTTP_QUERY_CONTENT_LENGTH            :
		if( dwFormat==HTTP_QUERY_FLAG_NUMBER )
		{
			*(DWORD*)lpBuf = pReqHttp->dwFld_ContentLen;
			fSuccess = TRUE;
		}
		else if( !dwFormat )
		{
			fSuccess = HttpHlp_FetchField( pReqHttp->pBufRespHdr, HTTP_CONTENT_LENGTH, LEN_HTTP_CONTENT_LENGTH+1, pszInfo, lpdwLenBuf );
		}
		else
		{
			fSuccess = FALSE;
		}
		break;

	//查询头信息
	//case HTTP_QUERY_RAW_HEADERS               :
	//	break;
	case HTTP_QUERY_RAW_HEADERS_CRLF          :
		HttpHlp_CopyStr( pReqHttp->pBufRespHdr, pReqHttp->dwLenRespHdr, pszInfo, lpdwLenBuf );
		fSuccess = TRUE;
		break;

	//直接查找 字符串信息---很常用
	case HTTP_QUERY_CONTENT_TYPE              :
		fSuccess = HttpHlp_FetchField( pReqHttp->pBufRespHdr, HTTP_CONTENT_TYPE, LEN_HTTP_CONTENT_TYPE+1, pszInfo, lpdwLenBuf );
		break;
	case HTTP_QUERY_STATUS_CODE               :
		if( dwFormat==HTTP_QUERY_FLAG_NUMBER )
		{
			*(DWORD*)lpBuf = pReqHttp->dwStatusHttp;
			fSuccess = TRUE;
		}
		else if( !dwFormat )
		{
			HttpHlp_CopyField( pReqHttp->pBufRespHdr, LEN_HTTP_VERSION+1, pszInfo, lpdwLenBuf );
			fSuccess = TRUE;
		}
		else
		{
			fSuccess = FALSE;
		}
		break;
	case HTTP_QUERY_STATUS_TEXT               :
		HttpHlp_CopyField( pReqHttp->pBufRespHdr, 0, pszInfo, lpdwLenBuf );
		fSuccess = TRUE;
		break;
	case HTTP_QUERY_VERSION                   :
		HttpHlp_CopyStr( pReqHttp->pBufRespHdr, LEN_HTTP_VERSION, pszInfo, lpdwLenBuf );
		fSuccess = TRUE;
		break;

	case HTTP_QUERY_LOCATION                  :
		fSuccess = HttpHlp_FetchField( pReqHttp->pBufRespHdr, HTTP_LOCATION, LEN_HTTP_LOCATION+1, pszInfo, lpdwLenBuf );
		break;


	case HTTP_QUERY_CONTENT_RANGE             :

	//暂时取消的
	case HTTP_QUERY_MESSAGE_ID                :

	//直接查找 字符串信息
	case HTTP_QUERY_CONTENT_TRANSFER_ENCODING :
	case HTTP_QUERY_CONTENT_ID                :
	case HTTP_QUERY_CONTENT_DESCRIPTION       :
	case HTTP_QUERY_CONTENT_LANGUAGE          :
	case HTTP_QUERY_ALLOW                     :
	case HTTP_QUERY_PUBLIC                    :
	case HTTP_QUERY_DATE                      :
	case HTTP_QUERY_EXPIRES                   :
	case HTTP_QUERY_LAST_MODIFIED             :
	case HTTP_QUERY_URI                       :
	case HTTP_QUERY_DERIVED_FROM              :
	case HTTP_QUERY_COST                      :
	case HTTP_QUERY_LINK                      :
	case HTTP_QUERY_PRAGMA                    :
	case HTTP_QUERY_CONNECTION                :
	case HTTP_QUERY_ACCEPT                    :
	case HTTP_QUERY_ACCEPT_CHARSET            :
	case HTTP_QUERY_ACCEPT_ENCODING           :
	case HTTP_QUERY_ACCEPT_LANGUAGE           :
	case HTTP_QUERY_AUTHORIZATION             :
	case HTTP_QUERY_CONTENT_ENCODING          :
	case HTTP_QUERY_FORWARDED                 :
	case HTTP_QUERY_FROM                      :
	case HTTP_QUERY_IF_MODIFIED_SINCE         :
//	case HTTP_QUERY_LOCATION                  :
	case HTTP_QUERY_ORIG_URI                  :
	case HTTP_QUERY_REFERER                   :
	case HTTP_QUERY_RETRY_AFTER               :
	case HTTP_QUERY_SERVER                    :
	case HTTP_QUERY_TITLE                     :
	case HTTP_QUERY_USER_AGENT                :
	case HTTP_QUERY_WWW_AUTHENTICATE          :
	case HTTP_QUERY_PROXY_AUTHENTICATE        :
	case HTTP_QUERY_ACCEPT_RANGES             :
	case HTTP_QUERY_SET_COOKIE                :
	case HTTP_QUERY_COOKIE                    :
	case HTTP_QUERY_REFRESH                   :
	case HTTP_QUERY_CONTENT_DISPOSITION       :

	case HTTP_QUERY_AGE                       :
	case HTTP_QUERY_CACHE_CONTROL             :
	case HTTP_QUERY_CONTENT_BASE              :
	case HTTP_QUERY_CONTENT_LOCATION          :
	case HTTP_QUERY_CONTENT_MD5               :
	case HTTP_QUERY_ETAG                      :
	case HTTP_QUERY_HOST                      :
	case HTTP_QUERY_IF_MATCH                  :
	case HTTP_QUERY_IF_NONE_MATCH             :
	case HTTP_QUERY_IF_RANGE                  :
	case HTTP_QUERY_IF_UNMODIFIED_SINCE       :
	case HTTP_QUERY_MAX_FORWARDS              :
	case HTTP_QUERY_PROXY_AUTHORIZATION       :
	case HTTP_QUERY_RANGE                     :
	case HTTP_QUERY_TRANSFER_ENCODING         :
	case HTTP_QUERY_UPGRADE                   :
	case HTTP_QUERY_VARY                      :
	case HTTP_QUERY_VIA                       :
	case HTTP_QUERY_WARNING                   :

	default :
		fSuccess = FALSE;
		break;
	}
	//
	pReqHttp->dwRef --;
	LeaveCriticalSection( &pReqHttp->csReq );

	return fSuccess;
}



//////////////////////////////////////////

BOOL	HttpHlp_InitReq( HTTP_REQ* pReqHttp, INET_CNN* pINetCnn, BYTE bHandleType, LPCSTR lpszVerb, LPCSTR lpszPath, DWORD dwLenPath, LPCSTR lpszVersion, DWORD dwReqFlags, DWORD dwContext )
{
	DWORD			dwBufSize;
	SKRET			dwRet;

	//基本初始化
	HANDLE_INIT( pReqHttp, sizeof(HTTP_REQ) );
	pReqHttp->bHandleType = bHandleType;
	List_InitHead( &pReqHttp->hListReq );
	InitializeCriticalSection( &pReqHttp->csReq );
	pReqHttp->hWndStatus = pINetCnn->hWndStatus;
	if( dwContext==-1 )
	{
		pReqHttp->dwContext = pINetCnn->dwContext;
	}
	else
	{
		pReqHttp->dwContext = dwContext;
	}
	//保存参数信息
	pReqHttp->pINetCnn = pINetCnn;

	strcpy( pReqHttp->szVerb, lpszVerb );
	if( dwLenPath==-1 )
	{
		dwLenPath = strlen( lpszPath );
	}
	pReqHttp->dwLenPath = dwLenPath;
	memcpy( pReqHttp->szPath, lpszPath, dwLenPath );
	pReqHttp->szPath[dwLenPath] = 0;
	strcpy( pReqHttp->szVersion, lpszVersion );

	//
	pReqHttp->skReq = SKAPI_Socket( AF_INET, SOCK_STREAM, 0 );
	if( pReqHttp->skReq==INVALID_SOCKET )
	{
		return FALSE;
	}
	dwBufSize = 8192+2048;
	dwRet = SKAPI_SetSockOpt( pReqHttp->skReq, SOL_SOCKET, SO_RCVBUF, (char*)&dwBufSize, sizeof(DWORD) );
	if( dwRet!=E_SUCCESS )
	{
		return FALSE;
	}
	//dwBufSize=1024;
	//SKAPI_SetSockOpt( pInternet->socket,SOL_SOCKET,SO_SNDBUF,(char*)&dwBufSize,4);

	//关联
	EnterCriticalSection( &pINetCnn->csListReq );
	List_InsertTail( &pINetCnn->hListReq, &pReqHttp->hListReq );
	LeaveCriticalSection( &pINetCnn->csListReq );

	return TRUE;
}


LPVOID	HttpHlp_OpenCnnReq( INET_OPEN* pINetOpen, LPCSTR lpszUrl, DWORD dwReqFlags, DWORD dwContext )
{
	HTTP_CNNREQ*	pCnnReqHttp;
	INET_CNN*		pINetCnn;
	HTTP_REQ*		pReqHttp;

	char			lpszSrvName[MAX_SRVNAME+4];
	char			lpszPath[MAX_HTTP_PATH+4];
	DWORD			dwLenPath;
	WORD			wSrvPort;

	//分析服务器信息
	if( pINetOpen->dwAccessType==INTERNET_OPEN_TYPE_PROXY )
	{
		//从配置中获取 代理服务器信息: lpszSrvName/wSrvPort
		strcpy( lpszSrvName, pINetOpen->pszProxy );

		//
		dwLenPath = strlen(lpszUrl);
		if( dwLenPath>MAX_HTTP_PATH )
		{
			SetLastError( ERROR_INVALID_PARAMETER );
			return NULL;
		}
		memcpy( lpszPath, lpszUrl, dwLenPath );
	}
	else
	{
		if( !INetHlp_LookUrl( (LPCSTR)HTTP_SCHEME2, LEN_HTTP_SCHEME2, lpszUrl, &wSrvPort, lpszSrvName, MAX_SRVNAME, lpszPath, MAX_HTTP_PATH ) )
		{
			return NULL;
		}
		dwLenPath = -1;
	}

	//检查和分配
	pCnnReqHttp = (HTTP_CNNREQ*)INetHlp_OpenCnn( pINetOpen, sizeof(HTTP_CNNREQ), lpszSrvName, wSrvPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, dwContext );
	if( !pCnnReqHttp )
	{
		return NULL;
	}
	//初始化 请求
	pINetCnn = &pCnnReqHttp->stCnnINet;
	pReqHttp = &pCnnReqHttp->stReqHttp;
	if( !HttpHlp_InitReq( pReqHttp, pINetCnn, INETHTYPE_CNNREQ2, (LPCSTR)HTTP_GET, lpszPath, dwLenPath, (LPCSTR)HTTP_VERSION, dwReqFlags, dwContext ) )
	{
		INetHlp_CloseCnn( pINetCnn, TRUE );
		return NULL;
	}

	//关联
	EnterCriticalSection( &pINetOpen->csListCnn );
	List_InsertTail( &pINetOpen->hListCnn, &pINetCnn->hListCnn );
	LeaveCriticalSection( &pINetOpen->csListCnn );

	return (LPVOID)pCnnReqHttp;
}

void	HttpHlp_CloseReq( HTTP_REQ* pReqHttp )
{
	DWORD			dwTickStart;

	RETAILMSG( 1, ( "HttpHlp_CloseReq enrty.\r\n" ) );
	SKAPI_CloseSocket( pReqHttp->skReq );
	Sleep( 1 );

	RETAILMSG( 1, ( "HttpHlp_CloseReq 001.\r\n" ) );
	//等待所有的引用 退出
	dwTickStart = GetTickCount();
	while( pReqHttp->dwRef )
	{
		if( (GetTickCount()-dwTickStart)>(15*1000) )
		{
			break;
		}
		Sleep( 3 );
	}

	RETAILMSG( 1, ( "HttpHlp_CloseReq 002.\r\n" ) );

	//释放 退出
	if( pReqHttp->pszRefer )
	{
		free( pReqHttp->pszRefer );
		pReqHttp->pszRefer = NULL;
	}
	if( pReqHttp->pBufRespHdr )
	{
		free( pReqHttp->pBufRespHdr );
		pReqHttp->pBufRespHdr = NULL;
	}
	if( pReqHttp->pBufRespData )
	{
		free( pReqHttp->pBufRespData );
		pReqHttp->pBufRespData = NULL;
	}
	Sleep( 1 );
	DeleteCriticalSection( &pReqHttp->csReq );
	if( pReqHttp->bHandleType==INETHTYPE_CNNREQ2 )
	{
		HANDLE_RESET( pReqHttp );
	}
	else
	{
		HANDLE_FREE( pReqHttp );
	}
	RETAILMSG( 1, ( "HttpHlp_CloseReq leave.\r\n" ) );
}

BOOL	HttpHlp_SendReq( HTTP_REQ* pReqHttp, LPCSTR lpszHdrs, DWORD dwLenHdrs, LPVOID lpBody, DWORD dwLenBody )
{
	INET_CNN*		pINetCnn;
	INET_OPEN*		pINetOpen;
	HWND			hWndStatus;
	DWORD			dwContext;

	DATA_DEAL		stDealData;
	SKRET			nSent;

	//
	pINetCnn = pReqHttp->pINetCnn;
	pINetOpen = pINetCnn->pINetOpen;
	hWndStatus = pReqHttp->hWndStatus;
	dwContext = pReqHttp->dwContext;
	//准备 HTTP数据包
	if( !VarBuf_Alloc( &stDealData, DDF_GROW, 0, 0, 200 ) )
	{
		return FALSE;
	}
	//添加 Request-Line = Method SP Request-URI SP HTTP-Version CRLF
	VarEnc_Buffer( &stDealData, (LPBYTE)pReqHttp->szVerb, strlen(pReqHttp->szVerb) );
	VarEnc_Byte( &stDealData, 0x20 );
	VarEnc_Buffer( &stDealData, (LPBYTE)pReqHttp->szPath, pReqHttp->dwLenPath );
	VarEnc_Byte( &stDealData, 0x20 );
	VarEnc_Buffer( &stDealData, (LPBYTE)pReqHttp->szVersion, strlen(pReqHttp->szVersion) );
	VarEnc_Buffer( &stDealData, (LPBYTE)INET_CRLF, LEN_INET_CRLF );

	//添加 *(( general-header | request-header | entity-header ) CRLF)
	//
	if( pReqHttp->pszAccept )
	{
		VarEnc_Buffer( &stDealData, (LPBYTE)HTTP_ACCEPT, LEN_HTTP_ACCEPT );
		VarEnc_Byte( &stDealData, ':' );
		VarEnc_Buffer( &stDealData, (LPBYTE)pReqHttp->pszAccept, strlen(pReqHttp->pszAccept) );
		VarEnc_Buffer( &stDealData, (LPBYTE)INET_CRLF, LEN_INET_CRLF );
	}
	//
	if( lpszHdrs && dwLenHdrs )
	{
		if( dwLenHdrs==-1 )
		{
			dwLenHdrs = strlen(lpszHdrs);
		}
		VarEnc_Buffer( &stDealData, (LPBYTE)lpszHdrs, dwLenHdrs );
		VarEnc_Buffer( &stDealData, (LPBYTE)INET_CRLF, LEN_INET_CRLF );
	}
	//
	VarEnc_Buffer( &stDealData, (LPBYTE)HTTP_USER_AGENT, LEN_HTTP_USER_AGENT );
	VarEnc_Byte( &stDealData, ':' );
	VarEnc_Buffer( &stDealData, (LPBYTE)pINetOpen->pszAgent, strlen(pINetOpen->pszAgent) );
	VarEnc_Buffer( &stDealData, (LPBYTE)INET_CRLF, LEN_INET_CRLF );
	//
	VarEnc_Buffer( &stDealData, (LPBYTE)HTTP_HOST, LEN_HTTP_HOST );
	VarEnc_Byte( &stDealData, ':' );
	VarEnc_Buffer( &stDealData, (LPBYTE)pINetCnn->pszSrvName, strlen(pINetCnn->pszSrvName) );
	VarEnc_Buffer( &stDealData, (LPBYTE)INET_CRLF, LEN_INET_CRLF );
	//
	if( pReqHttp->pszRefer )
	{
		VarEnc_Buffer( &stDealData, (LPBYTE)HTTP_REFERER, LEN_HTTP_REFERER );
		VarEnc_Byte( &stDealData, ':' );
		VarEnc_Buffer( &stDealData, (LPBYTE)pReqHttp->pszRefer, pReqHttp->dwLenRefer );
		VarEnc_Buffer( &stDealData, (LPBYTE)INET_CRLF, LEN_INET_CRLF );
	}

	//添加 CRLF
	VarEnc_Buffer( &stDealData, (LPBYTE)INET_CRLF, LEN_INET_CRLF );

	//添加 message-body = entity-body | <entity-body encoded as per Transfer-Encoding>
	if( lpBody && dwLenBody )
	{
		if( dwLenBody==-1 )
		{
			dwLenBody = strlen((LPSTR)lpBody);
		}
		VarEnc_Buffer( &stDealData, lpBody, dwLenBody );
	}

	//发送 HTTP数据包
	if( hWndStatus!=(HWND)-1 )
	{
		PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_SENDING_REQUEST, dwContext );
	}
	//RETAILMSG(DEBUG_INET,(TEXT("  Http_Sending_Request: [%s]\r\n"), pReqHttp->szPath));
	if( (nSent=SKAPI_Send( pReqHttp->skReq, stDealData.lpData, stDealData.dwLenDealed, 0 ))==SOCKET_ERROR )
	{
		//
		VarBuf_Free( &stDealData );
		return FALSE;
	}
	if( hWndStatus!=(HWND)-1 )
	{
		PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_REQUEST_SENT, dwContext );
	}
	//RETAILMSG(DEBUG_INET,(TEXT("  Http_Request_Sent: \r\n")));

	VarBuf_Free( &stDealData );
	return TRUE;
}


BOOL	HttpHlp_WaitResp( HTTP_REQ* pReqHttp )
{
	HWND			hWndStatus;
	DWORD			dwContext;

	DWORD			dwErr;
	DWORD			dwCntTry;
	DWORD			dwLenRecv;

	DWORD			dwReadHdr;
	DWORD			dwReadHdr_Sav;
	BYTE			pHdrResp[LEN_HDRRESP+4];
	LPSTR			pszHttpField;
	DWORD			dwStatusHttp;

	//
	hWndStatus = pReqHttp->hWndStatus;
	dwContext = pReqHttp->dwContext;
	//等待 HTTP数据包的应答头
	if( hWndStatus!=(HWND)-1 )
	{
		PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_RECEIVING_RESPONSE, dwContext );
	}
	//RETAILMSG(DEBUG_INET,(TEXT("  Http_Receiving_Response: \r\n")));
	//
	dwReadHdr = 0;
	for( dwCntTry=0; !INETG_ISEXIT(pReqHttp) && (dwCntTry<5); dwCntTry++ )
	{
		//等待并接收数据
		if( (dwErr=INetHlp_RecvData( pReqHttp->skReq, 30, pHdrResp+dwReadHdr, LEN_HDRRESP-dwReadHdr, &dwLenRecv ))!=E_SUCCESS )
		{
			break;
		}
		if( !dwLenRecv )
		{
			continue;
		}
		//保存数据信息
		dwReadHdr += dwLenRecv;
		pHdrResp[dwReadHdr] = 0;

		//
		if( dwReadHdr>LEN_HDRRESP )
		{
			int kk=0;
		}

		//判断 是否已经读完 HTTP数据包的应答头
		pszHttpField = strstr( pHdrResp, HTTP_END );
		if( pszHttpField==NULL )
		{
			continue;
		}
		pszHttpField += LEN_HTTP_END;
		dwReadHdr_Sav = ((DWORD)pszHttpField - (DWORD)pHdrResp);

		//判断 Status-Code
		dwStatusHttp = atoi( pHdrResp+LEN_HTTP_VERSION+1 );
		pReqHttp->dwStatusHttp = dwStatusHttp;
		RETAILMSG(DEBUG_INET,(TEXT("  Http_WaitResp: Get Hdr=[%d],status=[%d]\r\n"), dwReadHdr_Sav, dwStatusHttp));
		//判断 是否需要继续等待
		if( dwStatusHttp<200 )
		{
			if( dwReadHdr_Sav<dwReadHdr )
			{
				dwReadHdr -= dwReadHdr_Sav;
				memcpy( pHdrResp, pszHttpField, dwReadHdr );
			}
			else
			{
				dwReadHdr = 0;
			}
			pHdrResp[dwReadHdr] = 0;
			RETAILMSG(DEBUG_INET,(TEXT("  ")));
			RETAILMSG(DEBUG_INET,(TEXT("  ")));
			RETAILMSG(DEBUG_INET,(TEXT("  Http_WaitResp: status=[%d]\r\n"), dwStatusHttp));
			RETAILMSG(DEBUG_INET,(TEXT("  ")));
			RETAILMSG(DEBUG_INET,(TEXT("  ")));
			continue;
		}
		//dwStatusHttp /= 100;

		//保存 HTTP数据包的应答头
		pReqHttp->pBufRespHdr = (LPBYTE)malloc( dwReadHdr_Sav+4 );
		if( !pReqHttp->pBufRespHdr )
		{
			//
			break;
		}
		memcpy( pReqHttp->pBufRespHdr, pHdrResp, dwReadHdr_Sav );
		pReqHttp->pBufRespHdr[dwReadHdr_Sav] = 0;
		pReqHttp->dwLenRespHdr = dwReadHdr_Sav;
		RETAILMSG(DEBUG_INET,(TEXT("  Http_hdr: [%s] \r\n"), pReqHttp->pBufRespHdr));
		//保存 HTTP数据包的Body
		if( dwReadHdr_Sav<dwReadHdr )
		{
			dwReadHdr -= dwReadHdr_Sav;
			pReqHttp->pBufRespData = (LPBYTE)malloc( dwReadHdr );
			if( !pReqHttp->pBufRespData )
			{
				//
				break;
			}
			memcpy( pReqHttp->pBufRespData, pszHttpField, dwReadHdr );
			pReqHttp->dwLenRespData = dwReadHdr;
		}

		//
		//分析 HTTP数据包的应答头
		//
		//看---HTTP_CONTENT_LENGTH
		if( pszHttpField=strstr(pReqHttp->pBufRespHdr+LEN_HTTP_VERSION+1+4, HTTP_CONTENT_LENGTH) )
		{
			//
			pszHttpField += (LEN_HTTP_CONTENT_LENGTH+1);
			while( *pszHttpField )
			{
				if( *pszHttpField!=' ' )
				{
					break;
				}
				pszHttpField ++;
			}
			pReqHttp->dwFld_ContentLen = atoi( pszHttpField );
			RETAILMSG(DEBUG_INET,(TEXT("  Http_WaitResp: ContentLen=[%d]\r\n"), pReqHttp->dwFld_ContentLen));
		}
		else
		{
			pReqHttp->dwFld_ContentLen = HTTP_INVALID_CONTENT_SIZE;
			RETAILMSG(DEBUG_INET,(TEXT("  Http_WaitResp: ContentLen=[0xFFFFFFFF]\r\n")));
		}
		//
		pReqHttp->wFlag |= IREQF_HDRRESP;
		//
		if( hWndStatus!=(HWND)-1 )
		{
			PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_RESPONSE_RECEIVED, dwContext );
		}
		//RETAILMSG(DEBUG_INET,(TEXT("  Http_Response_Received: \r\n")));
		return TRUE;
	}

	return FALSE;
}

BOOL	HttpHlp_ReadFile( HTTP_REQ* pReqHttp, LPVOID lpBuffer, DWORD dwBytesToRead, OUT LPDWORD lpdwBytesRead )
{
	DWORD			dwLenRespData_Read;
	LPBYTE			pBufRead;
	DWORD			dwLenRecv;
	DWORD			dwErr;

	//
	EnterCriticalSection( &pReqHttp->csReq );
	if( !(pReqHttp->wFlag & IREQF_HDRRESP) )
	{
		//如果
		LeaveCriticalSection( &pReqHttp->csReq );
		return FALSE;
	}
	pBufRead = (LPBYTE)lpBuffer;
	pReqHttp->dwRef ++;
	//直接拷贝数据
	if( pReqHttp->pBufRespData )
	{
		//拷贝数据
		dwLenRespData_Read = pReqHttp->dwLenRespData - pReqHttp->dwLenRespData_Offset;
		if( dwLenRespData_Read > dwBytesToRead )
		{
			dwLenRespData_Read = dwBytesToRead;
		}
		memcpy( pBufRead, pReqHttp->pBufRespData+pReqHttp->dwLenRespData_Offset, dwLenRespData_Read );
		*lpdwBytesRead = dwLenRespData_Read;
		pBufRead += dwLenRespData_Read;
		dwBytesToRead -= dwLenRespData_Read;
		//判断 数据是否读完
		pReqHttp->dwLenRespData_Offset += dwLenRespData_Read;
		pReqHttp->dwLenRespData_Read += dwLenRespData_Read;
		if( pReqHttp->dwLenRespData_Offset>=pReqHttp->dwLenRespData )
		{
			free( pReqHttp->pBufRespData );
			pReqHttp->pBufRespData = NULL;
		}
		//判断 空间是否用完
		if( dwBytesToRead==0 )
		{
			pReqHttp->dwRef --;
			LeaveCriticalSection( &pReqHttp->csReq );
			return TRUE;
		}
	}
	//
	while( !INETG_ISEXIT(pReqHttp) && dwBytesToRead )
	{
		//判断 是否有必要继续接收数据
		if( pReqHttp->dwLenRespData_Read>=pReqHttp->dwFld_ContentLen )
		{
			break;
		}
		//等待并接收数据
		if( (dwErr=INetHlp_RecvData( pReqHttp->skReq, 15, pBufRead, dwBytesToRead, &dwLenRecv ))!=E_SUCCESS )
		{
			RETAILMSG(DEBUG_INET,(TEXT("  HttpHlp_ReadFile: INetHlp_RecvData==dwErr [%d]\r\n"), dwErr));
			break;
		}
		if( !dwLenRecv )
		{
			RETAILMSG(DEBUG_INET,(TEXT("  HttpHlp_ReadFile: INetHlp_RecvData==No data\r\n")));
			break;
		}
		//保存数据信息
		pBufRead += dwLenRecv;
		(*lpdwBytesRead) += dwLenRecv;
		dwBytesToRead -= dwLenRecv;
		pReqHttp->dwLenRespData_Read += dwLenRecv;
	}
	//
	pReqHttp->dwRef --;
	LeaveCriticalSection( &pReqHttp->csReq );

	return TRUE;
}


BOOL	HttpHlp_SaveFile( HTTP_REQ* pReqHttp, HANDLE hFileSav, OUT LPDWORD lpdwBytesRead )
{
	DWORD			dwSizeAll;
	DWORD			dwTmp;
	BYTE			pBufRead[4096];
	int				i;
	int				iAll;

	//
	pReqHttp->dwRef ++;
	//
	dwSizeAll = pReqHttp->dwFld_ContentLen;
	if( (dwSizeAll==0) || (dwSizeAll==HTTP_INVALID_CONTENT_SIZE) )
	{
		while( !INETG_ISEXIT(pReqHttp) )
		{
			if( !HttpHlp_ReadFile( pReqHttp, pBufRead, sizeof(pBufRead), &dwTmp ) )
			{
				break;
			}
			if( dwTmp )
			{
				(*lpdwBytesRead) += dwTmp;
				if( !ver_WriteFile( hFileSav, pBufRead, dwTmp, &dwTmp, NULL ) )
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		//
		i = 0;
		iAll = 1 + dwSizeAll / 4096;
		if( iAll > 8 )
		{
			iAll = 8;
		}
		while( !INETG_ISEXIT(pReqHttp) && (*lpdwBytesRead<dwSizeAll) )
		{
			//
			if( !HttpHlp_ReadFile( pReqHttp, pBufRead, sizeof(pBufRead), &dwTmp ) )
			{
				break;
			}
			if( dwTmp )
			{
				(*lpdwBytesRead) += dwTmp;
				if( !ver_WriteFile( hFileSav, pBufRead, dwTmp, &dwTmp, NULL ) )
				{
					break;
				}
			}
			else
			{
				if( i++>5 )
				{
					break;
				}
			}
		}
	}

	//
	pReqHttp->dwRef --;
	//
	return TRUE;
}

DWORD	HttpHlp_LenField( LPCSTR pszHttpField )
{
	LPCSTR			pszTemp;

	//统计1行的长度
	pszTemp = pszHttpField;
	while( *pszTemp )
	{
		if( (*pszTemp=='\r') || (*pszTemp=='\n') )
		{
			break;
		}
		pszTemp ++;
	}
	return (DWORD)(pszTemp - pszHttpField);
}

void	HttpHlp_CopyField( LPCSTR pszHttpField, DWORD dwLenSkip, OUT LPSTR pszCopy, IN OUT DWORD* pdwLenCopy )
{
	DWORD			dwLenField;

	pszCopy[0] = 0;
	//
	if( dwLenSkip )
	{
		pszHttpField += dwLenSkip;
		if( *pszHttpField==' ' )
		{
			pszHttpField ++;
		}
	}
	//
	dwLenField = HttpHlp_LenField( pszHttpField );
	if( dwLenField>=*pdwLenCopy )
	{
		dwLenField = *pdwLenCopy - 1;
	}
	memcpy( pszCopy, pszHttpField, dwLenField );
	pszCopy[dwLenField] = 0;
	*pdwLenCopy = dwLenField;
}

void	HttpHlp_CopyStr( LPCSTR pszStr, DWORD dwLenStr, OUT LPSTR pszCopy, IN OUT DWORD* pdwLenCopy )
{
	pszCopy[0] = 0;
	//
	if( dwLenStr==-1 )
	{
		dwLenStr = strlen(pszStr);
	}
	//
	if( dwLenStr>=*pdwLenCopy )
	{
		dwLenStr = *pdwLenCopy - 1;
	}
	memcpy( pszCopy, pszStr, dwLenStr );
	pszCopy[dwLenStr] = 0;
	*pdwLenCopy = dwLenStr;
}

BOOL	HttpHlp_FetchField( LPCSTR pBufRespHdr, LPCSTR pszFind, DWORD dwLenSkip, OUT LPSTR pszCopy, IN OUT DWORD* pdwLenCopy )
{
	LPSTR			pszHttpField;
	DWORD			dwLenField;

	//
	pszCopy[0] = 0;
	pszHttpField = strstr( pBufRespHdr, pszFind );
	if( pszHttpField )
	{
		if( dwLenSkip )
		{
			pszHttpField += dwLenSkip;
			if( *pszHttpField==' ' )
			{
				pszHttpField ++;
			}
		}
		dwLenField = HttpHlp_LenField( pszHttpField );
		if( dwLenField>=*pdwLenCopy )
		{
			dwLenField = *pdwLenCopy - 1;
		}
		memcpy( pszCopy, pszHttpField, dwLenField );
		pszCopy[dwLenField] = 0;
		*pdwLenCopy = dwLenField;
		return TRUE;
	}
	return FALSE;
}



