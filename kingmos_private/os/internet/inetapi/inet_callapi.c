/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����RAS API
�汾�ţ�  2.0.0
����ʱ�ڣ�2004-05-09
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
#include <eapisrv.h>
#include <apiexe.h>
//"\inc_app"
#include <internet.h>


/***************  ȫ���� ���壬 ���� *****************/

enum
{
// inet ����
	  EINETAPI_InternetOpen = 1
	, EINETAPI_InternetConnect
	, EINETAPI_InternetReadFile
	, EINETAPI_InternetCloseHandle
	, EINETAPI_InternetSetStatusCallback

// http ����
	, EINETAPI_HttpOpenRequest
	, EINETAPI_InternetOpenUrl
	, EINETAPI_HttpAddRequestHeaders
	, EINETAPI_HttpSendRequest

	, EINETAPI_HttpQueryQuick
	, EINETAPI_HttpQueryInfo

// ftp ����

};

// inet ����
typedef	HINTERNET	(WINAPI* PINETAPI_InternetOpen)( LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags );
typedef	HINTERNET	(WINAPI* PINETAPI_InternetConnect)( HINTERNET hInetOpen, LPCSTR lpszSrvName, WORD wSrvPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwSrvType, DWORD dwSrvFlags, DWORD dwContext );
typedef	BOOL		(WINAPI* PINETAPI_InternetReadFile)( HINTERNET hInetReq, LPVOID lpBuffer, DWORD dwBytesToRead, OUT LPDWORD lpdwBytesRead );
typedef	BOOL		(WINAPI* PINETAPI_InternetCloseHandle)( HINTERNET hInet );
typedef	BOOL		(WINAPI* PINETAPI_InternetSetStatusCallback)( HINTERNET hInet, HWND hWndINet, DWORD dwContext );


// http ����
typedef	HINTERNET	(WINAPI* PINETAPI_HttpOpenRequest)( HINTERNET hInetCnn, LPCSTR lpszVerb, LPCSTR lpszPath, LPCSTR lpszVersion, LPCSTR lpszRefer, LPCSTR lpszAcceptTypes, DWORD dwReqFlags, DWORD dwContext );
typedef	HINTERNET	(WINAPI* PINETAPI_InternetOpenUrl)( HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwReqFlags, DWORD dwContext );
typedef	BOOL		(WINAPI* PINETAPI_HttpAddRequestHeaders)( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwModifiers );
typedef	BOOL		(WINAPI* PINETAPI_HttpSendRequest)( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, LPVOID lpBody, DWORD dwLenBody );

typedef	BOOL		(WINAPI* PINETAPI_HttpQueryQuick)( HINTERNET hInetReq, OUT DWORD* pdwStatusHttp, OUT DWORD* pdwContentLen, OUT LPSTR pszContentType, DWORD dwLenBuf, IN OUT LPDWORD lpdwIndex );
typedef	BOOL		(WINAPI* PINETAPI_HttpQueryInfo)( HINTERNET hInetReq, DWORD dwInfoLevel, OUT LPVOID lpBuffer OPTIONAL, OUT LPDWORD lpdwBufferLength, OUT LPDWORD lpdwIndex OPTIONAL );


// ftp ����


/******************************************************/

// inet ����
HINTERNET	WINAPI	INet_Open( LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags )
{
	EXE_API5( API_INTERNET, EINETAPI_InternetOpen, PINETAPI_InternetOpen, HINTERNET, lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags );
}
HINTERNET	WINAPI	INet_Connect( HINTERNET hInetOpen, LPCSTR lpszSrvName, WORD wSrvPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwSrvType, DWORD dwSrvFlags, DWORD dwContext )
{
	EXE_API8( API_INTERNET, EINETAPI_InternetConnect, PINETAPI_InternetConnect, HINTERNET, hInetOpen, lpszSrvName, wSrvPort, lpszUserName, lpszPassword, dwSrvType, dwSrvFlags, dwContext );
}
BOOL		WINAPI	INet_ReadFile( HINTERNET hInetReq, LPVOID lpBuffer, DWORD dwBytesToRead, OUT LPDWORD lpdwBytesRead )
{
	EXE_API4( API_INTERNET, EINETAPI_InternetReadFile, PINETAPI_InternetReadFile, BOOL, hInetReq, lpBuffer, dwBytesToRead, lpdwBytesRead );
}
BOOL		WINAPI	INet_CloseHandle( HINTERNET hInet )
{
	EXE_API1( API_INTERNET, EINETAPI_InternetCloseHandle, PINETAPI_InternetCloseHandle, BOOL, hInet );
}
BOOL		WINAPI	INet_SetStatusCallback( HINTERNET hInet, HWND hWndINet, DWORD dwContext )
{
	EXE_API3( API_INTERNET, EINETAPI_InternetSetStatusCallback, PINETAPI_InternetSetStatusCallback, BOOL, hInet, hWndINet, dwContext );
}


// http ����
HINTERNET	WINAPI	IHttp_OpenRequest( HINTERNET hInetCnn, LPCSTR lpszVerb, LPCSTR lpszPath, LPCSTR lpszVersion, LPCSTR lpszRefer, LPCSTR lpszAcceptTypes, DWORD dwFlags, DWORD dwContext )
{
	EXE_API8( API_INTERNET, EINETAPI_HttpOpenRequest, PINETAPI_HttpOpenRequest, HINTERNET, hInetCnn, lpszVerb, lpszPath, lpszVersion, lpszRefer, lpszAcceptTypes, dwFlags, dwContext );
}
HINTERNET	WINAPI	INet_OpenUrl( HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext )
{
	EXE_API6( API_INTERNET, EINETAPI_InternetOpenUrl, PINETAPI_InternetOpenUrl, HINTERNET, hInetOpen, lpszUrl, lpszHdrs, dwLenHdrs, dwFlags, dwContext );
}
BOOL		WINAPI	IHttp_AddRequestHeaders( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwModifiers )
{
	EXE_API4( API_INTERNET, EINETAPI_HttpAddRequestHeaders, PINETAPI_HttpAddRequestHeaders, BOOL, hInetReq, lpszHdrs, dwLenHdrs, dwModifiers );
}
BOOL		WINAPI	IHttp_SendRequest( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, LPVOID lpBody, DWORD dwLenBody )
{
	EXE_API5( API_INTERNET, EINETAPI_HttpSendRequest, PINETAPI_HttpSendRequest, BOOL, hInetReq, lpszHdrs, dwLenHdrs, lpBody, dwLenBody );
}

BOOL		WINAPI	IHttp_QueryQuick( HINTERNET hInetReq, OUT DWORD* pdwStatusHttp, OUT DWORD* pdwContentLen, OUT LPSTR pszContentType, DWORD dwLenBuf, IN OUT LPDWORD lpdwIndex )
{
	EXE_API6( API_INTERNET, EINETAPI_HttpQueryQuick, PINETAPI_HttpQueryQuick, BOOL, hInetReq, pdwStatusHttp, pdwContentLen, pszContentType, dwLenBuf, lpdwIndex );
}

BOOL		WINAPI	IHttp_QueryInfo( HINTERNET hInetReq, DWORD dwInfoLevel, IN OUT LPVOID lpBuf, IN OUT LPDWORD lpdwLenBuf, IN OUT LPDWORD lpdwIndex )
{
	EXE_API5( API_INTERNET, EINETAPI_HttpQueryInfo, PINETAPI_HttpQueryInfo, BOOL, hInetReq, dwInfoLevel, lpBuf, lpdwLenBuf, lpdwIndex );
}


// ftp ����


