/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _SKT_CALL_H_
#define _SKT_CALL_H_

#ifdef __cplusplus
extern "C" {
#endif


//
// socket实现函数的声明
//

extern	SKERR	WINAPI	SKAPI_WSAStartup( WORD wVersionRequired, OUT LPWSADATA lpWSAData );
extern	SKRET	WINAPI	SKAPI_WSACleanup(void);
extern	void	WINAPI	SKAPI_WSASetLastError( SKERR nErrSock );
extern	SKERR	WINAPI	SKAPI_WSAGetLastError(void);

extern	SOCKET	WINAPI	SKAPI_Socket( int af, int type, int protocol );
extern	SKRET	WINAPI	SKAPI_Bind( SOCKET s, const SOCKADDR* myaddr, int addr_len );
extern	SKRET	WINAPI	SKAPI_Connect( SOCKET s, const SOCKADDR* addr, int addr_len );
extern	SKRET	WINAPI	SKAPI_Listen( SOCKET s, int queue_len );
extern	SOCKET	WINAPI	SKAPI_Accept( SOCKET s, OUT SOCKADDR* addr, OUT int* paddr_len );
extern	SKRET	WINAPI	SKAPI_IoctlSocket( SOCKET s, long cmd, IN OUT DWORD* argp );
extern	SKRET	WINAPI	SKAPI_Send( SOCKET s, const char* buf, int len, int flags );
extern	SKRET	WINAPI	SKAPI_SendTo( SOCKET s, const char* buf, int len, int flags, const SOCKADDR* to, int tolen );
extern	SKRET	WINAPI	SKAPI_Recv( SOCKET s, OUT char* buf, int len, int flags );
extern	SKRET	WINAPI	SKAPI_RecvFrom( SOCKET s, OUT char* buf, int len, int flags, OUT SOCKADDR* from, IN OUT int* fromlen );
extern	SKRET	WINAPI	SKAPI_Select( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout );

extern	SKRET	WINAPI	SKAPI_GetSockOpt( SOCKET s, int level, int optname, OUT char* optval, IN OUT int* optlen );
extern	SKRET	WINAPI	SKAPI_SetSockOpt( SOCKET s, int level, int optname, const char* optval, int optlen );
extern	SKRET	WINAPI	SKAPI_GetPeerName( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen );
extern	SKRET	WINAPI	SKAPI_GetSockName( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen );

extern	SKRET	WINAPI	SKAPI_ShutDown( SOCKET s, int how );
extern	SKRET	WINAPI	SKAPI_CloseSocket( SOCKET s );

extern	SKRET	WINAPI	SKAPI_GetHostName( OUT char* name, int namelen );
extern	HOSTENT*	WINAPI	SKAPI_GetHostByName( const char* name );
extern	HOSTENT*	WINAPI	SKAPI_GetHostByAddr( const char* addr, int len, int type );
extern	HOSTENT*	WINAPI	SKAPI_GetHostByNameEx( const char* name, OUT LPBYTE pBufInfo, int lenInfo );
extern	HOSTENT*	WINAPI	SKAPI_GetHostByAddrEx( const char* addr, int len, int type, OUT LPBYTE pBufInfo, int lenInfo );


#ifdef __cplusplus
}	
#endif

#endif	//_SKT_CALL_H_
