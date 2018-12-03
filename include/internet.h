/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _KINGMOS_INTERNET_H_
#define _KINGMOS_INTERNET_H_

#ifndef _ERROR_INFO_H_
#include <errinfo.h>
#endif

#ifndef _KINGMOS_SOCKET_H_
#include <socket.h>
#endif

#ifndef _KINGMOS_INETSTR_H_
#include <inetstr.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ------------------------------------------------
// 数据类型、长度宏、字符串定义
// ------------------------------------------------

//数据类型定义
typedef HANDLE			HINTERNET;
typedef HINTERNET*		LPHINTERNET;


//
//字符串定义
//

extern	const	char	INET_CRLF    [];//="\r\n";

extern	const	char	HTTP_SCHEME  [];//="http:";
extern	const	char	HTTP_SCHEME2 [];//="http";
extern	const	char	HTTP_VERSION [];//="HTTP/1.1";
extern	const	char	HTTP_GET     [];//="GET";
extern	const	char	HTTP_END     [];//="\r\n\r\n";


#define	LEN_INET_CRLF		2  //(sizeof(INET_CRLF)-1)

#define	LEN_HTTP_SCHEME		5  //(sizeof(HTTP_SCHEME)-1)
#define	LEN_HTTP_SCHEME2	4  //(sizeof(HTTP_SCHEME2)-1)
#define	LEN_HTTP_VERSION	8  //(sizeof(HTTP_VERSION)-1)
#define	LEN_HTTP_GET		3  //(sizeof(HTTP_GET)-1)
#define	LEN_HTTP_END		4  //(sizeof(HTTP_END)-1)


//
// the default major/minor HTTP version numbers
//

#define HTTP_MAJOR_VERSION      1
#define HTTP_MINOR_VERSION      1

#define	HTTP_INVALID_CONTENT_SIZE	0xFFFFFFFF

//
//字符串长度定义
//
//lpszAgent / lpszProxy
#define	MAX_USRAGENT		64
#define	MAX_PROXY			128
//lpszSrvName / lpszUserName and pszPassword
#define	MAX_SRVNAME			128
#define	MAX_INETUSR			128
#define	MAX_INETPW			128
//lpszVerb / lpszPath / lpszVersion
#define	MAX_HTTP_VERB		20
#define	MAX_HTTP_PATH		256
#define	MAX_HTTP_VERSION	20


// ------------------------------------------------
// INTERNET 错误信息
// ------------------------------------------------
//#define	WM_INETSTATUS	0x12121
#define	ERR_INET_XXXXX  123987



// ------------------------------------------------
// 参数宏定义
// ------------------------------------------------

// dwAccessType
//#define INTERNET_OPEN_TYPE_PRECONFIG                    0   // use registry configuration
#define INTERNET_OPEN_TYPE_DIRECT                       1   // direct to net
#define INTERNET_OPEN_TYPE_PROXY                        3   // via named proxy
//#define INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY  4   // prevent using java/script/INS

//
#define INTERNET_INVALID_PORT_NUMBER                    1

// wSrvPort
#define INTERNET_DEFAULT_FTP_PORT       21          // default for FTP servers
#define INTERNET_DEFAULT_GOPHER_PORT    70          //    "     "  gopher "
#define INTERNET_DEFAULT_HTTP_PORT      80          //    "     "  HTTP   "
#define INTERNET_DEFAULT_HTTPS_PORT     443         //    "     "  HTTPS  "
#define INTERNET_DEFAULT_SOCKS_PORT     1080        // default for SOCKS firewall servers.

// dwSrvType
#define INTERNET_SERVICE_URL    0
#define INTERNET_SERVICE_FTP    1
#define INTERNET_SERVICE_GOPHER 2
#define INTERNET_SERVICE_HTTP   3


// 通用 dwFlags
#define INTERNET_FLAG_RELOAD            0x80000000  // retrieve the original item

// dwFlags of InternetOpenUrl():
#define INTERNET_FLAG_RAW_DATA          0x40000000  // FTP/gopher find: receive the item as raw (structured) data
#define INTERNET_FLAG_EXISTING_CONNECT  0x20000000  // FTP: use existing InternetConnect handle for server if possible
#define INTERNET_FLAG_PASSIVE           0x08000000  // used for FTP connections

// dwFlags of InternetOpenUrl(): 跟 安全 相关
#define INTERNET_FLAG_SECURE            0x00800000  // use PCT/SSL if applicable (HTTP)
#define INTERNET_FLAG_KEEP_CONNECTION   0x00400000  // use keep-alive semantics
#define INTERNET_FLAG_NO_AUTO_REDIRECT  0x00200000  // don't handle redirections automatically
#define INTERNET_FLAG_READ_PREFETCH     0x00100000  // do background read prefetch
#define INTERNET_FLAG_NO_COOKIES        0x00080000  // no automatic cookie handling
#define INTERNET_FLAG_NO_AUTH           0x00040000  // no automatic authentication handling
#define INTERNET_FLAG_CACHE_IF_NET_FAIL 0x00010000  // return cache file if net request fails

// dwFlags of InternetOpenUrl(): 跟 ache 相关
#define INTERNET_FLAG_RESYNCHRONIZE     0x00000800  // asking wininet to update an item if it is newer
#define INTERNET_FLAG_HYPERLINK         0x00000400  // asking wininet to do hyperlinking semantic which works right for scripts
#define INTERNET_FLAG_NO_UI             0x00000200  // no cookie popup
#define INTERNET_FLAG_PRAGMA_NOCACHE    0x00000100  // asking wininet to add "pragma: no-cache"
#define INTERNET_FLAG_CACHE_ASYNC       0x00000080  // ok to perform lazy cache-write
#define INTERNET_FLAG_FORMS_SUBMIT      0x00000040  // this is a forms submit
#define INTERNET_FLAG_NEED_FILE         0x00000010  // need a file for this request
#define INTERNET_FLAG_MUST_CACHE_REQUEST INTERNET_FLAG_NEED_FILE

// dwFlags of InternetOpenUrl(): 跟 FTP 相关
#define INTERNET_FLAG_TRANSFER_ASCII    FTP_TRANSFER_TYPE_ASCII     // 0x00000001
#define INTERNET_FLAG_TRANSFER_BINARY   FTP_TRANSFER_TYPE_BINARY    // 0x00000002


//
#define	WM_INTERNET_EVENT		(WM_NET_FIRST+1)
//wParam = dwEvt
//lParam = dwContext
#define INTERNET_STATUS_RESOLVING_NAME          10
#define INTERNET_STATUS_NAME_RESOLVED           11
#define INTERNET_STATUS_CONNECTING_TO_SERVER    20
#define INTERNET_STATUS_CONNECTED_TO_SERVER     21
#define INTERNET_STATUS_SENDING_REQUEST         30
#define INTERNET_STATUS_REQUEST_SENT            31
#define INTERNET_STATUS_RECEIVING_RESPONSE      40
#define INTERNET_STATUS_RESPONSE_RECEIVED       41
#define INTERNET_STATUS_CTL_RESPONSE_RECEIVED   42
#define INTERNET_STATUS_PREFETCH                43
#define INTERNET_STATUS_CLOSING_CONNECTION      50
#define INTERNET_STATUS_CONNECTION_CLOSED       51
#define INTERNET_STATUS_HANDLE_CREATED          60
#define INTERNET_STATUS_HANDLE_CLOSING          70
#define INTERNET_STATUS_REQUEST_COMPLETE        100
#define INTERNET_STATUS_REDIRECT                110
#define INTERNET_STATUS_INTERMEDIATE_RESPONSE   120
#define INTERNET_STATUS_STATE_CHANGE            200


//
// HttpQueryInfo info levels. Generally, there is one info level
// for each potential RFC822/HTTP/MIME header that an HTTP server
// may send as part of a request response.
//
// The HTTP_QUERY_RAW_HEADERS info level is provided for clients
// that choose to perform their own header parsing.
//

#define HTTP_QUERY_MIME_VERSION                 0
#define HTTP_QUERY_CONTENT_TYPE                 1
#define HTTP_QUERY_CONTENT_TRANSFER_ENCODING    2
#define HTTP_QUERY_CONTENT_ID                   3
#define HTTP_QUERY_CONTENT_DESCRIPTION          4
#define HTTP_QUERY_CONTENT_LENGTH               5
#define HTTP_QUERY_CONTENT_LANGUAGE             6
#define HTTP_QUERY_ALLOW                        7
#define HTTP_QUERY_PUBLIC                       8
#define HTTP_QUERY_DATE                         9
#define HTTP_QUERY_EXPIRES                      10
#define HTTP_QUERY_LAST_MODIFIED                11
#define HTTP_QUERY_MESSAGE_ID                   12
#define HTTP_QUERY_URI                          13
#define HTTP_QUERY_DERIVED_FROM                 14
#define HTTP_QUERY_COST                         15
#define HTTP_QUERY_LINK                         16
#define HTTP_QUERY_PRAGMA                       17
#define HTTP_QUERY_VERSION                      18  // special: part of status line
#define HTTP_QUERY_STATUS_CODE                  19  // special: part of status line
#define HTTP_QUERY_STATUS_TEXT                  20  // special: part of status line
#define HTTP_QUERY_RAW_HEADERS                  21  // special: all headers as ASCIIZ
#define HTTP_QUERY_RAW_HEADERS_CRLF             22  // special: all headers
#define HTTP_QUERY_CONNECTION                   23
#define HTTP_QUERY_ACCEPT                       24
#define HTTP_QUERY_ACCEPT_CHARSET               25
#define HTTP_QUERY_ACCEPT_ENCODING              26
#define HTTP_QUERY_ACCEPT_LANGUAGE              27
#define HTTP_QUERY_AUTHORIZATION                28
#define HTTP_QUERY_CONTENT_ENCODING             29
#define HTTP_QUERY_FORWARDED                    30
#define HTTP_QUERY_FROM                         31
#define HTTP_QUERY_IF_MODIFIED_SINCE            32
#define HTTP_QUERY_LOCATION                     33
#define HTTP_QUERY_ORIG_URI                     34
#define HTTP_QUERY_REFERER                      35
#define HTTP_QUERY_RETRY_AFTER                  36
#define HTTP_QUERY_SERVER                       37
#define HTTP_QUERY_TITLE                        38
#define HTTP_QUERY_USER_AGENT                   39
#define HTTP_QUERY_WWW_AUTHENTICATE             40
#define HTTP_QUERY_PROXY_AUTHENTICATE           41
#define HTTP_QUERY_ACCEPT_RANGES                42
#define HTTP_QUERY_SET_COOKIE                   43
#define HTTP_QUERY_COOKIE                       44
#define HTTP_QUERY_REQUEST_METHOD               45  // special: GET/POST etc.
#define HTTP_QUERY_REFRESH                      46
#define HTTP_QUERY_CONTENT_DISPOSITION          47

//
// HTTP 1.1 defined headers
//

#define HTTP_QUERY_AGE                          48
#define HTTP_QUERY_CACHE_CONTROL                49
#define HTTP_QUERY_CONTENT_BASE                 50
#define HTTP_QUERY_CONTENT_LOCATION             51
#define HTTP_QUERY_CONTENT_MD5                  52
#define HTTP_QUERY_CONTENT_RANGE                53
#define HTTP_QUERY_ETAG                         54
#define HTTP_QUERY_HOST                         55
#define HTTP_QUERY_IF_MATCH                     56
#define HTTP_QUERY_IF_NONE_MATCH                57
#define HTTP_QUERY_IF_RANGE                     58
#define HTTP_QUERY_IF_UNMODIFIED_SINCE          59
#define HTTP_QUERY_MAX_FORWARDS                 60
#define HTTP_QUERY_PROXY_AUTHORIZATION          61
#define HTTP_QUERY_RANGE                        62
#define HTTP_QUERY_TRANSFER_ENCODING            63
#define HTTP_QUERY_UPGRADE                      64
#define HTTP_QUERY_VARY                         65
#define HTTP_QUERY_VIA                          66
#define HTTP_QUERY_WARNING                      67

#define HTTP_QUERY_MAX                          67

//
// HTTP_QUERY_CUSTOM - if this special value is supplied as the dwInfoLevel
// parameter of HttpQueryInfo() then the lpBuffer parameter contains the name
// of the header we are to query
//

#define HTTP_QUERY_CUSTOM                       65535

//
// HTTP_QUERY_FLAG_REQUEST_HEADERS - if this bit is set in the dwInfoLevel
// parameter of HttpQueryInfo() then the request headers will be queried for the
// request information
//

#define HTTP_QUERY_FLAG_REQUEST_HEADERS         0x80000000

//
// HTTP_QUERY_FLAG_SYSTEMTIME - if this bit is set in the dwInfoLevel parameter
// of HttpQueryInfo() AND the header being queried contains date information,
// e.g. the "Expires:" header then lpBuffer will contain a SYSTEMTIME structure
// containing the date and time information converted from the header string
//

#define HTTP_QUERY_FLAG_SYSTEMTIME              0x40000000

//
// HTTP_QUERY_FLAG_NUMBER - if this bit is set in the dwInfoLevel parameter of
// HttpQueryInfo(), then the value of the header will be converted to a number
// before being returned to the caller, if applicable
//

#define HTTP_QUERY_FLAG_NUMBER                  0x20000000

//
// HTTP_QUERY_FLAG_COALESCE - combine the values from several headers of the
// same name into the output buffer
//

#define HTTP_QUERY_FLAG_COALESCE                0x10000000


#define HTTP_QUERY_MODIFIER_FLAGS_MASK          (HTTP_QUERY_FLAG_REQUEST_HEADERS    \
                                                | HTTP_QUERY_FLAG_SYSTEMTIME        \
                                                | HTTP_QUERY_FLAG_NUMBER            \
                                                | HTTP_QUERY_FLAG_COALESCE          \
                                                )

#define HTTP_QUERY_HEADER_MASK                  (~HTTP_QUERY_MODIFIER_FLAGS_MASK)



// ------------------------------------------------
// 结构定义
// ------------------------------------------------



// ------------------------------------------------------
// internet 提供的 API
// ------------------------------------------------------

//
//函数 的实现
//

// inet 函数
extern	HINTERNET	WINAPI	INet_Open( LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags );
extern	HINTERNET	WINAPI	INet_Connect( HINTERNET hInetOpen, LPCSTR lpszSrvName, WORD wSrvPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwSrvType, DWORD dwSrvFlags, DWORD dwContext );
extern	BOOL		WINAPI	INet_ReadFile( HINTERNET hInetReq, LPVOID lpBuffer, DWORD dwBytesToRead, OUT LPDWORD lpdwBytesRead );
extern	BOOL		WINAPI	INet_CloseHandle( HINTERNET hInet );
extern	BOOL		WINAPI	INet_SetStatusCallback( HINTERNET hInet, HWND hWndINet, DWORD dwContext );
extern	BOOL		WINAPI	INet_GetLastError( HINTERNET hInet, OUT ERROR_INFO* pInfoErr );


// http 函数
extern	HINTERNET	WINAPI	IHttp_OpenRequest( HINTERNET hInetCnn, LPCSTR lpszVerb, LPCSTR lpszPath, LPCSTR lpszVersion, LPCSTR lpszRefer, LPCSTR lpszAcceptTypes, DWORD dwFlags, DWORD dwContext );
extern	HINTERNET	WINAPI	INet_OpenUrl( HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext );
extern	BOOL		WINAPI	IHttp_AddRequestHeaders( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwModifiers );
extern	BOOL		WINAPI	IHttp_SendRequest( HINTERNET hInetReq, LPCSTR lpszHdrs, DWORD dwLenHdrs, LPVOID lpBody, DWORD dwLenBody );

extern	BOOL		WINAPI	IHttp_QueryQuick( HINTERNET hInetReq, OUT DWORD* pdwStatusHttp, OUT DWORD* pdwContentLen, OUT LPSTR pszContentType, DWORD dwLenBuf, IN OUT LPDWORD lpdwIndex );
extern	BOOL		WINAPI	IHttp_QueryInfo( HINTERNET hInetReq, DWORD dwInfoLevel, IN OUT LPVOID lpBuf, IN OUT LPDWORD lpdwLenBuf, IN OUT LPDWORD lpdwIndex );


// ftp 函数



//
//函数 的宏替代
//

// inet 函数
#define	InternetOpen					INet_Open
#define	InternetConnect					INet_Connect
#define	InternetReadFile				INet_ReadFile
#define	InternetCloseHandle				INet_CloseHandle
#define	InternetSetStatusCallback		INet_SetStatusCallback
#define	InternetGetLastError			INet_GetLastError

// http 函数
#define	HttpOpenRequest					IHttp_OpenRequest
#define	InternetOpenUrl					INet_OpenUrl
#define	HttpAddRequestHeaders			IHttp_AddRequestHeaders
#define	HttpSendRequest					IHttp_SendRequest

#define	HttpQueryQuick					IHttp_QueryQuick
#define	HttpQueryInfo					IHttp_QueryInfo

// ftp 函数



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_KINGMOS_INTERNET_H_
