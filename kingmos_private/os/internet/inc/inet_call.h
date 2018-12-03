/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _INET_CALL_H_
#define _INET_CALL_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus



// ------------------------------------------------
// 函数定义
// ------------------------------------------------

// inet 函数
extern	HINTERNET	WINAPI	INETAPI_InternetOpen( LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags );
extern	HINTERNET	WINAPI	INETAPI_InternetConnect( HINTERNET hInetOpen, LPCSTR lpszSrvName, WORD wSrvPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwSrvType, DWORD dwSrvFlags, DWORD dwContext );
extern	BOOL		WINAPI	INETAPI_InternetReadFile( HINTERNET hInetReq, LPVOID lpBuffer, DWORD dwBytesToRead, OUT LPDWORD lpdwBytesRead );
extern	BOOL		WINAPI	INETAPI_InternetCloseHandle( HINTERNET hInet );
extern	BOOL		WINAPI	INETAPI_InternetSetStatusCallback( HINTERNET hInet, HWND hWndINet, DWORD dwContext );


// http 函数
extern	HINTERNET	WINAPI	INETAPI_HttpOpenRequest( HINTERNET hInetCnn, LPCSTR lpszVerb, LPCSTR lpszPath, LPCSTR lpszVersion, LPCSTR lpszRefer, LPCSTR lpszAcceptTypes, DWORD dwReqFlags, DWORD dwContext );
extern	HINTERNET	WINAPI	INETAPI_InternetOpenUrl( HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwReqFlags, DWORD dwContext );
extern	BOOL		WINAPI	INETAPI_HttpAddRequestHeaders( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwModifiers );
extern	BOOL		WINAPI	INETAPI_HttpSendRequest( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, LPVOID lpBody, DWORD dwLenBody );

extern	BOOL		WINAPI	INETAPI_HttpQueryQuick( HINTERNET hInetReq, OUT DWORD* pdwStatusHttp, OUT DWORD* pdwContentLen, OUT LPSTR pszContentType, DWORD dwLenBuf, IN OUT LPDWORD lpdwIndex );
extern	BOOL		WINAPI	INETAPI_HttpQueryInfo( HINTERNET hInetReq, DWORD dwInfoLevel, OUT LPVOID lpBuffer OPTIONAL, OUT LPDWORD lpdwBufferLength, OUT LPDWORD lpdwIndex OPTIONAL );


// ftp 函数



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_INET_CALL_H_

