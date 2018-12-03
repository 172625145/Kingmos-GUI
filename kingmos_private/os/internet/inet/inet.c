/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：internet
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
//"\inc_local"
#include "skt_call.h"
#include "inet_call.h"
#include "inet_def.h"


/***************  全局区 定义， 声明 *****************/
		INET_GLOBAL*		g_lpGlobalINet = NULL;

//
static	void	INetHlp_ForceClose( );
static	void	INetHlp_Close( INET_OPEN* pINetOpen, BOOL fRemove );
static	void	INetHlp_CloseReq( INET_COMM* pINetComm, BOOL fRemove );
static	void	INetHlp_CloseCnnReq( INET_COMM* pINetComm );


/******************************************************/


BOOL	Internet_Init( )
{
	//分配1个TCP的功能 和基本初始化
	g_lpGlobalINet = (INET_GLOBAL*)HANDLE_ALLOC( sizeof(INET_GLOBAL) );
	if( !g_lpGlobalINet )
	{
		return FALSE;
	}
	HANDLE_INIT( g_lpGlobalINet, sizeof(INET_GLOBAL) );
	List_InitHead( &g_lpGlobalINet->hListOpen );
	InitializeCriticalSection( &g_lpGlobalINet->csListOpen );
	//
	return TRUE;
}

void	Internet_Deinit( )
{
}


HINTERNET	WINAPI	INETAPI_InternetOpen( LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags )
{
	INET_OPEN*		pINetOpen;
	int				nLen;
	WSADATA			WSAData;
	int				iErrCode;

	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpen: enter\r\n")));
	//句柄安全检查
	if( INETG_CHECK_FAIL(g_lpGlobalINet) )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpen: leave failed --1\r\n")));
		return NULL;
	}
	//参数检查---目前只支持 两种
	if( dwAccessType==INTERNET_OPEN_TYPE_PROXY )
	{
		if( !lpszProxy || !(nLen=strlen(lpszProxy)) || (nLen>MAX_PROXY) )
		{
			SetLastError( ERROR_INVALID_PARAMETER );
			RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpen: leave failed --2\r\n")));
			return NULL;
		}
	}
	else if( dwAccessType!=INTERNET_OPEN_TYPE_DIRECT )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpen: leave failed --3\r\n")));
		return NULL;
	}

	//启动socket
	if( iErrCode=SKAPI_WSAStartup( MAKEWORD(1,1), &WSAData ) )
	{
		SetLastError( ERR_INET_XXXXX );
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpen: leave failed --4\r\n")));
		return NULL;
	}

	//分配和基本初始化
	pINetOpen = (INET_OPEN*)HANDLE_ALLOC( sizeof(INET_OPEN) );
	if( !pINetOpen )
	{
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpen: leave failed --5\r\n")));
		return NULL;
	}
	HANDLE_INIT( pINetOpen, sizeof(INET_OPEN) );
	pINetOpen->bHandleType = INETHTYPE_OPEN;
	List_InitHead( &pINetOpen->hListOpen );
	pINetOpen->hWndStatus = (HWND)-1;
	pINetOpen->dwContext = -1;
	//
	List_InitHead( &pINetOpen->hListCnn );
	InitializeCriticalSection( &pINetOpen->csListCnn );
	pINetOpen->hProc = GetCallerProcess( );

	//保存参数信息
	pINetOpen->dwAccessType = dwAccessType;
	if( lpszAgent )
	{
		strncpy( pINetOpen->pszAgent, lpszAgent, MAX_USRAGENT );
		pINetOpen->pszAgent[MAX_USRAGENT] = 0;
	}
	else
	{
		strcpy( pINetOpen->pszAgent, "xyg Internet Explore" );
	}
	//关联
	EnterCriticalSection( &g_lpGlobalINet->csListOpen );
	List_InsertTail( &g_lpGlobalINet->hListOpen, &pINetOpen->hListOpen );
	LeaveCriticalSection( &g_lpGlobalINet->csListOpen );

	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetOpen: leave ok h=[0x%x]\r\n"), pINetOpen));
	return (HINTERNET)pINetOpen;
}

HINTERNET	WINAPI	INETAPI_InternetConnect( HINTERNET hInetOpen, LPCSTR lpszSrvName, WORD wSrvPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwSrvType, DWORD dwSrvFlags, DWORD dwContext )
{
	INET_OPEN*		pINetOpen = (INET_OPEN*)hInetOpen;
	INET_CNN*		pINetCnn;

	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetConnect: enter\r\n")));
	//检查和分配
	pINetCnn = (INET_CNN*)INetHlp_OpenCnn( hInetOpen, sizeof(INET_CNN), lpszSrvName, wSrvPort, lpszUserName, lpszPassword, dwSrvType, dwSrvFlags, dwContext );
	if( pINetCnn )
	{
		//关联
		EnterCriticalSection( &pINetOpen->csListCnn );
		List_InsertTail( &pINetOpen->hListCnn, &pINetCnn->hListCnn );
		LeaveCriticalSection( &pINetOpen->csListCnn );
	}
	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetConnect: leave [0x%x]\r\n"), pINetCnn));
	return (HINTERNET)pINetCnn;
}

BOOL	WINAPI	INETAPI_InternetReadFile( HINTERNET hInetReq, LPVOID lpBuffer, DWORD dwBytesToRead, OUT LPDWORD lpdwBytesRead )
{
	INET_COMM*		pINetComm = (INET_COMM*)hInetReq;
	INET_CNN*		pINetCnn;
	INET_OPEN*		pINetOpen;

	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetReadFile: Enter [%d]\r\n"), dwBytesToRead));
	//句柄安全检查
	if( SREQ_CHECK_FAIL(pINetComm) )
	{
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetReadFile: Leave Failed--1\r\n")));
		return FALSE;
	}
	//参数检查
	pINetCnn = (INET_CNN*)pINetComm->dwData;
	ASSERT( pINetCnn );
	pINetOpen = pINetCnn->pINetOpen;
	if( !lpBuffer || !lpdwBytesRead )
	{
		INETE_SETERR_GEN( pINetOpen, ERROR_INVALID_PARAMETER );
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetReadFile: Leave Failed--2\r\n")));
		return FALSE;
	}
	*lpdwBytesRead = 0;
	if( !dwBytesToRead )
	{
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetReadFile: Leave ok 000\r\n")));
		return TRUE;
	}

	//
	switch( pINetCnn->dwSrvType )
	{
	case INTERNET_SERVICE_HTTP:
		if( HttpHlp_ReadFile( (HTTP_REQ*)pINetComm, lpBuffer, dwBytesToRead, lpdwBytesRead ) )
		{
			RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetReadFile: Leave ok [%d]\r\n"), *lpdwBytesRead));
			return TRUE;
		}
		break;
	case INTERNET_SERVICE_FTP:
		break;
	default:
		INETE_SETERR_GEN( pINetOpen, ERROR_INVALID_PARAMETER );
		break;
	}

	//RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetReadFile: leave [0x%x]\r\n"), pINetCnn));
	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetReadFile: Leave Failed--4\r\n")));
	return FALSE;
}


BOOL	WINAPI	INETAPI_InternetCloseHandle( HINTERNET hInet )
{
	INET_OPEN*		pINetOpen = (INET_OPEN*)hInet;

	//句柄安全检查
	if( !HANDLE_CHECK(pINetOpen) || !HANDLE_CHECK(g_lpGlobalINet) )
	{
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetCloseHandle: wrong\r\n")));
		return FALSE;
	}
	//
	switch( pINetOpen->bHandleType )
	{
	case INETHTYPE_OPEN:
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetCloseHandle: h=[0x%x], INETHTYPE_OPEN\r\n"), pINetOpen));
		INetHlp_Close( pINetOpen, TRUE );
		break;
	case INETHTYPE_CNN:
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetCloseHandle: h=[0x%x], INETHTYPE_CNN\r\n"), pINetOpen));
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetCloseHandle: INETHTYPE_CNN\r\n")));
		INetHlp_CloseCnn( (INET_CNN*)pINetOpen, TRUE );
		break;
	case INETHTYPE_REQ:
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetCloseHandle: h=[0x%x], INETHTYPE_REQ\r\n"), pINetOpen));
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetCloseHandle: INETHTYPE_REQ\r\n")));
		INetHlp_CloseReq( (INET_COMM*)pINetOpen, TRUE );
		break;
	case INETHTYPE_CNNREQ2:
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetCloseHandle: h=[0x%x], INETHTYPE_CNNREQ2\r\n"), pINetOpen));
		INetHlp_CloseCnnReq( (INET_COMM*)pINetOpen );
		break;
	default :
		RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetCloseHandle: h=[0x%x], INETHTYPE_OPEN\r\n"), pINetOpen));
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL	WINAPI	INETAPI_InternetSetStatusCallback( HINTERNET hInet, HWND hWndINet, DWORD dwContext )
{
	INET_COMM*		pINetComm = (INET_COMM*)hInet;

	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetSetStatusCallback: enter [0x%x]\r\n"), pINetComm));
	//句柄安全检查
	if( INETG_CHECK_FAIL(pINetComm) )
	{
		return FALSE;
	}
	//
	pINetComm->hWndStatus = hWndINet;
	pINetComm->dwContext = dwContext;

	RETAILMSG(DEBUG_INET,(TEXT("  INETAPI_InternetSetStatusCallback: leave [1]\r\n")));
	return TRUE;
}



///////////////

LPVOID	INetHlp_OpenCnn( HINTERNET hInetOpen, DWORD dwSize, LPCSTR lpszSrvName, WORD wSrvPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwSrvType, DWORD dwSrvFlags, DWORD dwContext )
{
	INET_OPEN*		pINetOpen = (INET_OPEN*)hInetOpen;
	INET_CNN*		pINetCnn;

	//句柄安全检查
	if( SOPEN_CHECK_FAIL(pINetOpen) || INETG_CHECK_FAIL(g_lpGlobalINet) )
	{
		return NULL;
	}
	//参数检查
	if( !lpszSrvName || !(lpszSrvName[0]) )
	{
		INETE_SETERR_GEN( pINetOpen, ERROR_INVALID_PARAMETER );
		return NULL;
	}
	if( dwSrvType!=INTERNET_SERVICE_HTTP )	//目前只支持 HTTP
	{
		INETE_SETERR_GEN( pINetOpen, ERROR_INVALID_PARAMETER );
		return NULL;
	}

	//分配和基本初始化
	pINetCnn = (INET_CNN*)HANDLE_ALLOC( dwSize );
	if( !pINetCnn )
	{
		INETE_SETERR_GEN( pINetOpen, ERROR_NOT_ENOUGH_MEMORY );
		return NULL;
	}
	HANDLE_INIT( pINetCnn, dwSize );
	if( dwSize==sizeof(INET_CNN) )
	{
		pINetCnn->bHandleType = INETHTYPE_CNN;
	}
	else
	{
		pINetCnn->bHandleType = INETHTYPE_CNNREQ;
	}
	List_InitHead( &pINetCnn->hListCnn );
	pINetCnn->hWndStatus = pINetOpen->hWndStatus;
	if( dwContext==-1 )
	{
		pINetCnn->dwContext = pINetOpen->dwContext;
	}
	else
	{
		pINetCnn->dwContext = dwContext;
	}
	//
	List_InitHead( &pINetCnn->hListReq );
	InitializeCriticalSection( &pINetCnn->csListReq );

	//保存参数信息
	pINetCnn->pINetOpen = pINetOpen;

	pINetCnn->dwSrvType = dwSrvType;
	pINetCnn->dwSrvFlags = dwSrvFlags;
	pINetCnn->wSrvPort = wSrvPort;
	strncpy( pINetCnn->pszSrvName, lpszSrvName, MAX_SRVNAME );
	pINetCnn->pszSrvName[MAX_SRVNAME] = 0;
	if( lpszUserName && *lpszUserName )
	{
		strncpy( pINetCnn->pszUserName, lpszUserName, MAX_INETUSR );
		pINetCnn->pszUserName[MAX_INETUSR] = 0;
	}
	if( lpszPassword && *lpszPassword )
	{
		strncpy( pINetCnn->pszPassword, lpszPassword, MAX_INETPW );
		pINetCnn->pszPassword[MAX_INETPW] = 0;
	}

	return pINetCnn;
}

BOOL	INetHlp_LookUrl( LPCSTR pszScheme2, DWORD dwLenScheme2, LPCSTR pszUrl, OUT WORD* pwSrvPort, 
						OUT LPSTR pszSrvName, DWORD dwLenSrv, OUT LPSTR pszPath, DWORD dwLenPath )
{
	LPSTR				pszTmp;
	DWORD				i;
	DWORD				dwTmp;

	//基本检查
	ASSERT( pszScheme2 && pszSrvName && pszPath && (dwLenSrv>2) && (dwLenPath>2) );
	//判断Scheme
	if( strnicmp( pszUrl, pszScheme2, dwLenScheme2 )==0 )
	{
		//判断和忽略Scheme
		pszTmp = (LPSTR)(pszUrl + dwLenScheme2);
		if( (*pszTmp!=':') ||  (pszTmp[1]!='/') || (pszTmp[2]!='/') )
		{
			return FALSE;
		}
		pszTmp += 3;
		
	}
	else
	{
		if( *pszUrl=='/' ) 
		{
			return FALSE;
		}
		pszTmp = (LPSTR)pszUrl;
	}

	//检查出来 服务器名称
	*pwSrvPort = INTERNET_DEFAULT_HTTP_PORT;
	i = 0;
	while( *pszTmp && (*pszTmp!='/') && i<dwLenSrv )
	{
		pszSrvName[i++] = *pszTmp ++;
	}
	pszSrvName[i] = 0;
	if( i>=dwLenSrv )
	{
		//说明空间太小
		return FALSE;
	}
	
	//检查出来 路径名称
	if( *pszTmp=='/' )
	{
		dwTmp = strlen( pszTmp );
		if( dwTmp>=dwLenPath )
		{
			//说明空间太小
			return FALSE;
		}
		memcpy( pszPath, pszTmp, dwTmp );
		pszPath[dwTmp] = 0;
	}
	else
	{
		pszPath[0] = '/';
		pszPath[1] = 0;
	}

	return TRUE;
}

BOOL	INetHlp_ConnectSrv( WORD* pwFlag, SOCKET skReq, LPCSTR pszSrvName, WORD wSrvPort, IN OUT DWORD* pdwSrvIP, HWND hWndStatus, DWORD dwContext )
{
	SOCKADDR_IN		stAddrNet;

	//
	if( *pdwSrvIP )
	{
		//通知过程状态
		if( hWndStatus!=(HWND)-1 )
		{
			PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_CONNECTING_TO_SERVER, dwContext );
		}
		if( DEBUG_INET )
		{
			char	pAddr[32];
			inet_ntoa_ip( (LPBYTE)pdwSrvIP, pAddr );
			RETAILMSG(DEBUG_INET,(TEXT("  Http_Connecting_To_Server: [%s]\r\n"), pAddr));
		}
		//进行连接
		stAddrNet.sin_family = AF_INET;
		stAddrNet.sin_port = htons(wSrvPort);
		memcpy( &stAddrNet.sin_addr, pdwSrvIP, sizeof(DWORD) );
		if( SKAPI_Connect(skReq, (SOCKADDR*)&stAddrNet, sizeof(SOCKADDR_IN) )!=SOCKET_ERROR )
		{
			//通知过程状态
			if( hWndStatus!=(HWND)-1 )
			{
				PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_CONNECTED_TO_SERVER, dwContext );
			}
			RETAILMSG(DEBUG_INET,(TEXT("  Http_Connected_To_Server: \r\n")));
			return TRUE;
		}
	}
	else
	{
		DWORD*			pdwTmp;
		int				i;
		HOSTENT*		pHostInfo;
		BYTE			pBufInfo[200];

		//通知过程状态
		if( hWndStatus!=(HWND)-1 )
		{
			PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_RESOLVING_NAME, dwContext );
		}
		//RETAILMSG(DEBUG_INET,(TEXT("  Http_Resolving_Name: [%s]\r\n"), pszSrvName));
		//域名查询
		pHostInfo = SKAPI_GetHostByNameEx( pszSrvName, pBufInfo, sizeof(pBufInfo) );
		if( pHostInfo==NULL )
		{
			RETAILMSG(DEBUG_INET,(TEXT("  Http_Resolving_Name: [%s], Failed!\r\n"), pszSrvName));
			return FALSE;
		}
		//通知过程状态
		if( hWndStatus!=(HWND)-1 )
		{
			PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_NAME_RESOLVED, dwContext );
		}
		//RETAILMSG(DEBUG_INET,(TEXT("  Http_Name_Resolved: [%s]\r\n"), pszSrvName));
		//尝试连接
		for( i=0; (pdwTmp=(DWORD*)(pHostInfo->h_addr_list[i])) && !(*pwFlag & INETF_EXIT); i++ )
		{
			//通知过程状态
			if( hWndStatus!=(HWND)-1 )
			{
				PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_CONNECTING_TO_SERVER, dwContext );
			}
			if( DEBUG_INET )
			{
				char	pAddr[32];
				inet_ntoa_ip( (LPBYTE)pdwTmp, pAddr );
				RETAILMSG(DEBUG_INET,(TEXT("  Http_Connecting_To_Server: [%s]-[%s]\r\n"), pAddr, pszSrvName));
			}
			//进行连接
			stAddrNet.sin_family = AF_INET;
			stAddrNet.sin_port = htons(wSrvPort);
			memcpy( &stAddrNet.sin_addr, pdwTmp, sizeof(DWORD) );
			if( SKAPI_Connect(skReq, (SOCKADDR*)&stAddrNet, sizeof(SOCKADDR_IN) )!=SOCKET_ERROR )
			{
				*pdwSrvIP = *pdwTmp;
				if( hWndStatus!=(HWND)-1 )
				{
					PostMessage( hWndStatus, WM_INTERNET_EVENT, INTERNET_STATUS_CONNECTED_TO_SERVER, dwContext );
				}
				RETAILMSG(DEBUG_INET,(TEXT("  Http_Connected_To_Server: \r\n")));
				return TRUE;
			}
		}
	}

	return FALSE;
}

DWORD	INetHlp_WaitData( SOCKET skReq, DWORD dwWaitSec, OUT DWORD* pdwLenRecv )
{
	fd_set			fdread;
	struct	timeval	twait;

	//等待数据
	twait.tv_sec = dwWaitSec;
	twait.tv_usec= 1;
	FD_ZERO( &fdread );
	FD_SET( skReq, &fdread );
	if( SKAPI_Select(0, &fdread, NULL, NULL, &twait)==SOCKET_ERROR )
	{
		return SKAPI_WSAGetLastError();
	}
	//判断是否有数据
	if( SKAPI_IoctlSocket(skReq, FIONREAD, pdwLenRecv)==SOCKET_ERROR )
	{
		return SKAPI_WSAGetLastError();
	}
	return E_SUCCESS;
}

DWORD	INetHlp_RecvData( SOCKET skReq, DWORD dwWaitSec, LPBYTE pBufRecv, DWORD dwLenBuf, OUT DWORD* pdwLenRecv )
{
	DWORD			dwErr;
	fd_set			fdread;
	struct	timeval	twait;
	SKRET			nRet;

	//等待数据
	twait.tv_sec = dwWaitSec;
	twait.tv_usec= 1;
	FD_ZERO( &fdread );
	FD_SET( skReq, &fdread );
	if( SKAPI_Select(0, &fdread, NULL, NULL, &twait)==SOCKET_ERROR )
	{
		dwErr = SKAPI_WSAGetLastError();
		if( dwErr==E_INVAL )
		{
			dwErr = E_SUCCESS;
			*pdwLenRecv = 0;
		}
		return dwErr;
	}
	//判断是否有数据
	if( SKAPI_IoctlSocket(skReq, FIONREAD, pdwLenRecv)==SOCKET_ERROR )
	{
		return SKAPI_WSAGetLastError();
	}
	if( !*pdwLenRecv )
	{
		return E_SUCCESS;
	}
	//开始接收数据
	nRet = SKAPI_Recv( skReq, (char*)pBufRecv, dwLenBuf, 0 );
	if( nRet==SOCKET_ERROR )
	{
		return SKAPI_WSAGetLastError();
	}
	*pdwLenRecv = nRet;
	return E_SUCCESS;
}


void	INetHlp_ForceClose( )
{
	INET_OPEN*		pINetOpen_Find;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalINet) )
	{
		return ;
	}
	//等待所有的 Open 退出
	pUnitHeader = &g_lpGlobalINet->hListOpen;
	EnterCriticalSection( &g_lpGlobalINet->csListOpen );
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pINetOpen_Find = LIST_CONTAINER( pUnit, INET_OPEN, hListOpen );
		pUnit = pUnit->pNext;
		
		//句柄检查
		if( !HANDLE_CHECK(pINetOpen_Find) )
		{
			//break;
			continue;
		}
		List_RemoveUnit( &pINetOpen_Find->hListOpen );
		List_InitHead( &pINetOpen_Find->hListOpen );
		INetHlp_Close( pINetOpen_Find, FALSE );
	}
	LeaveCriticalSection( &g_lpGlobalINet->csListOpen );
}

void	INetHlp_Close( INET_OPEN* pINetOpen, BOOL fRemove )
{
	INET_CNN*		pINetCnn_Find;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	//
	pINetOpen->wFlag |= INETF_EXIT;
	Sleep( 1 );
	//从链表中 断开
	if( fRemove )
	{
		EnterCriticalSection( &g_lpGlobalINet->csListOpen );
		List_RemoveUnit( &pINetOpen->hListOpen );
		List_InitHead( &pINetOpen->hListOpen );
		LeaveCriticalSection( &g_lpGlobalINet->csListOpen );
		Sleep( 1 );
	}

	//等待所有的Cnn 退出
	pUnitHeader = &pINetOpen->hListCnn;
	EnterCriticalSection( &pINetOpen->csListCnn );
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pINetCnn_Find = LIST_CONTAINER( pUnit, INET_CNN, hListCnn );
		pUnit = pUnit->pNext;
		
		//句柄检查
		if( !HANDLE_CHECK(pINetCnn_Find) )
		{
			break;
		}
		List_RemoveUnit( &pINetCnn_Find->hListCnn );
		List_InitHead( &pINetCnn_Find->hListCnn );
		INetHlp_CloseCnn( pINetCnn_Find, FALSE );
	}
	LeaveCriticalSection( &pINetOpen->csListCnn );
	Sleep( 1 );

	//
	SKAPI_WSACleanup( );

	//释放 退出
	DeleteCriticalSection( &pINetOpen->csListCnn );
	HANDLE_FREE( pINetOpen );
}

void	INetHlp_CloseCnn( INET_CNN* pINetCnn, BOOL fRemove )
{
	INET_OPEN*		pINetOpen;
	INET_COMM*		pINetComm_Req_Find;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	//RETAILMSG( 1, ( "INetHlp_CloseCnn entry.\r\n" ) );
	//
	pINetCnn->wFlag |= INETF_EXIT;
	//从链表中 断开
	pINetOpen = pINetCnn->pINetOpen;
	ASSERT( pINetCnn->pINetOpen );
	if( fRemove )
	{
		EnterCriticalSection( &pINetOpen->csListCnn );
		List_RemoveUnit( &pINetCnn->hListCnn );
		List_InitHead( &pINetCnn->hListCnn );
		LeaveCriticalSection( &pINetOpen->csListCnn );
		Sleep( 1 );
	}

	//等待所有的Req 退出
	if( !List_IsEmpty( &pINetCnn->hListReq ) )
	{
		pUnitHeader = &pINetCnn->hListReq;
		EnterCriticalSection( &pINetCnn->csListReq );
		pUnit = pUnitHeader->pNext;

		//RETAILMSG( 1, ( "INetHlp_CloseCnn while001.\r\n" ) );
		
		while( pUnit!=pUnitHeader )
		{
			pINetComm_Req_Find = LIST_CONTAINER( pUnit, INET_COMM, hListComm );
			pUnit = pUnit->pNext;
			
			//句柄检查
			if( !HANDLE_CHECK(pINetComm_Req_Find) )
			{
				break;
			}
			List_RemoveUnit( &pINetComm_Req_Find->hListComm );
			List_InitHead( &pINetComm_Req_Find->hListComm );
			INetHlp_CloseReq( pINetComm_Req_Find, FALSE );
		}

		//RETAILMSG( 1, ( "INetHlp_CloseCnn while002.\r\n" ) );

		LeaveCriticalSection( &pINetCnn->csListReq );
		Sleep( 1 );
	}

	//if( pReqHttp->bHandleType==INETHTYPE_CNN )
	{
		DeleteCriticalSection( &pINetCnn->csListReq );
		HANDLE_FREE( pINetCnn );
	}
	//RETAILMSG( 1, ( "INetHlp_CloseCnn leave.\r\n" ) );
}

void	INetHlp_CloseReq( INET_COMM* pINetComm, BOOL fRemove )
{
	INET_CNN*		pINetCnn;

	//
	//RETAILMSG( 1, ( "INetHlp_CloseReq enrty.\r\n" ) );
	pINetComm->wFlag |= INETF_EXIT;
	//从链表中 断开
	pINetCnn = (INET_CNN*)pINetComm->dwData;
	ASSERT( pINetCnn );
	if( fRemove && pINetCnn )
	{
		EnterCriticalSection( &pINetCnn->csListReq );
		List_RemoveUnit( &pINetComm->hListComm );
		List_InitHead( &pINetComm->hListComm );
		LeaveCriticalSection( &pINetCnn->csListReq );
		Sleep( 1 );
	}
	//
	switch( pINetCnn->dwSrvType )
	{
	case INTERNET_SERVICE_HTTP:
		HttpHlp_CloseReq( (HTTP_REQ*)pINetComm );
		break;
	case INTERNET_SERVICE_FTP:
		break;
	default:
		break;
	}
	//RETAILMSG( 1, ( "INetHlp_CloseReq leave.\r\n" ) );
}

void	INetHlp_CloseCnnReq( INET_COMM* pINetComm )
{
	//
	pINetComm->wFlag |= INETF_EXIT;
	Sleep( 1 );
	//
	ASSERT( pINetComm->dwData );
	INetHlp_CloseCnn( (INET_CNN*)pINetComm->dwData, TRUE );
}


