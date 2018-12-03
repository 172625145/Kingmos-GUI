#ifndef INTERNET_H_2002_1_15
#define INTERNET_H_2002_1_15

#include <socket.h>


typedef LPVOID			HINTERNET;
typedef short			HTTP_STATUS;

#define INTERNET_OPEN_TYPE_PRECONFIG                    0   // use registry configuration
#define INTERNET_OPEN_TYPE_DIRECT                       1   // direct to net
#define INTERNET_OPEN_TYPE_PROXY                        3   // via named proxy
#define INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY  4   // prevent using java/script/INS

#define INTERNET_INVALID_PORT_NUMBER                    1



#define   INTERNET_STATE_INIT           1 //the handle is created by InternetOpen function.
#define   INTERNET_STATE_OPENED         2 //the handle is created by InternetOpenUrl function.


#define   INTERNET_CONNECT_STATE_INIT				1
#define   INTERNET_CONNECT_STATE_UNCONNECTED        2
#define   INTERNET_CONNECT_STATE_CONNECTED			5

#define		    HTTP_ACCESS_SUCCESS                     0
#define 		HTTP_ACCESS_NODATA						2
#define 		HTTP_ACCESS_NEED_AUTH					3
#define 		HTTP_ACCESS_ERROR 						4
#define 		HTTP_ACCESS_SERVER_ERROR				5



#define MAX_SERVER			96
#define HTTP_MAX_PATH		512
#define MAX_DATA_BUFFER     1024
#define HEAD_END_STRING     "\r\n\r\n"
#define HTTP_END_STRING    "</HTML>\r\n"
#define HTTP_CON_LENGTH    "Content-Length:"
#define HTTP_CON_TYPE      "Content-Type:"
#define HTTP_HEAD_START    "HTTP/1.1"


#define HTTP_CONTENT_TYPE_HTM                      1 


#define WM_INTERNET_EVENT                         (WM_USER + 1002)



#define INTERNET_EVENT_CONNECTING_SERVER            10
#define INTERNET_EVENT_SERVER_CONNECTED             20
#define INTERNET_EVENT_SERVER_REQUESTING            30
#define INTERNET_EVENT_SERVER_REQUEST_SUCESS        40
#define INTERNET_EVENT_SERVER_CONNECT_FAIL          50


//+++++++++++++++++++++++++++++  ERROR code.

#define   ERROR_INTERNET_SERVER_ERROR    (WM_USER + 1000)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define   HTTP_AUTH_DIGEST                          10
#define   HTTP_AUTH_BASIC							20



#define HTTP_INVALID_CONTENT_SIZE                   0xFFFFFFFF


typedef  struct __INTERNET_OPEN {

    IN_ADDR        proxy_name; //the proxy name should be the IP address of the computer which run the proxy.
    DWORD          dwAccessType;
	DWORD          dwFlags;
	struct     __INTERNET_OPEN * pNext;
	DWORD          dwState;

	//following 5 members are only valid when calling InternetOpenUrl function, in InternetOpen, it's invalid.
	struct     __INTERNET_OPEN * pOpenHandle;//record the InternetOpen handle when open a Url, it's invalid when calling InternetOpen. 
	SOCKET         socket;
	struct sockaddr_in    server_addr;//the address of server,it's invalid if created by InternetOpen.
	DWORD          dwConnectState; //the client is connecting to server whether or not.

	DWORD          dwContentSize;  //size in bytes of the data that will be transfered.
	DWORD          dwDataHasRead;

	WORD           wServerPort;//server port, such as  FTP, HTTP.
	WORD           wDataOffset;

	HWND			hWndCallback;
	DWORD		   dwBufferDataLong;
	DWORD		   dwEndCharPass;
	CRITICAL_SECTION   cs;

	//WORD           wHeadEnd;   //the end of head string "\r\n\r\n" has passed.
	WORD           wContentType;
	

	char           cServerName[MAX_SERVER]; //when it's created by InternetOpen, cServerName takes the agent name,otherwise,takes
	                                    //the name of the server.
	char           cData[MAX_DATA_BUFFER +1];

}INTERNET_OPEN , *PINTERNET_OPEN;




BOOL WINAPI InternetCloseHandle( HINTERNET hInternet);
BOOL WINAPI InternetReadFile(
				IN HINTERNET hFile, 
				IN LPVOID lpBuffer, 
				IN DWORD dwNumberOfBytesToRead, 
				OUT LPDWORD lpdwNumberOfBytesRead
				) ;

HINTERNET InternetOpenUrl(
			HINTERNET hInternetSession,  
			LPCTSTR lpszUrl, 
			LPCTSTR lpszHeaders, 
			DWORD dwHeadersLength, 
			DWORD dwFlags, 
			DWORD dwContext
			) ;


HINTERNET WINAPI InternetOpen(
		LPCTSTR lpszAgent,
		DWORD dwAccessType,//   only support two :  INTERNET_OPEN_TYPE_DIRECT, INTERNET_OPEN_TYPE_PROXY 
		LPCTSTR lpszProxy,
		LPCTSTR lpszProxyBypass, //now we don't support proxy pass.
		DWORD dwFlags			// only INTERNET_INVALID_PORT_NUMBER  is supported.
		);


BOOL InternetSetStatusCallback( HINTERNET hInt, HWND   hWnd	);

//#define  SetDlgItemText( hWnd, item, cText )   \
//	     SendMessage( GetDlgItem( (hWnd),(item) ), WM_SETTEXT, 0,(WPARAM)(cText) ) 

#endif
