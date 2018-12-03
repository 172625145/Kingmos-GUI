/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：Socket API
版本号：  2.0.0
开发时期：2004-05-09
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
#include <eapisrv.h>
#include <apiexe.h>
//"\inc_app"
#include <socket.h>
#include <iphlpapi.h>


/***************  全局区 定义， 声明 *****************/

//
// socket实现函数的被API调用
//

enum
{
	  ESKAPI_WSAStartup = 1
	, ESKAPI_WSACleanup
	, ESKAPI_WSASetLastError
	, ESKAPI_WSAGetLastError

	, ESKAPI_socket			//----5
	, ESKAPI_bind
	, ESKAPI_connect
	, ESKAPI_listen 
	, ESKAPI_accept
	, ESKAPI_ioctlsocket		//----10
	, ESKAPI_send
	, ESKAPI_sendto
	, ESKAPI_recv
	, ESKAPI_recvfrom
	, ESKAPI_select			//----15

	, ESKAPI_getsockopt	
	, ESKAPI_setsockopt
	, ESKAPI_getpeername	
	, ESKAPI_getsockname

	, ESKAPI_shutdown			//----20
	, ESKAPI_closesocket 

	, ESKAPI_gethostname
	, ESKAPI_gethostbyname
	, ESKAPI_gethostbyaddr
	, ESKAPI_gethostbynameex	//----25
	, ESKAPI_gethostbyaddrex


	// IP层提供的 函数
	, EIPHAPI_GetNetworkParams
	, EIPHAPI_GetDnsSrvList

	, EIPHAPI_GetNumberOfInterfaces
	, EIPHAPI_GetAdaptersInfo
	, EIPHAPI_GetInterfaceInfo
	, EIPHAPI_GetIfEntry
	, EIPHAPI_GetIfTable
	, EIPHAPI_SetIfEntry
	, EIPHAPI_GetIpAddrTable
	, EIPHAPI_GetBestInterface

	, EIPHAPI_SetIpTTL
	, EIPHAPI_GetIpStatistics

	, EIPHAPI_AddIPAddress
	, EIPHAPI_IpReleaseAddress
	, EIPHAPI_IpRenewAddress
	, EIPHAPI_NotifyAddrChange
	, EIPHAPI_NotifyRouteChange

	, EIPHAPI_CreateIpForwardEntry
	, EIPHAPI_SetIpForwardEntry
	, EIPHAPI_DeleteIpForwardEntry

	// ICMP层提供的 函数
	, EIPHAPI_GetIcmpStatistics
	, EIPHAPI_IcmpCreateFile
	, EIPHAPI_IcmpCloseHandle
	, EIPHAPI_IcmpSendEcho

	
	// TCP层提供的 函数

};

typedef	SKERR	(WINAPI* PSKAPI_WSAStartup)( WORD wVersionRequired, OUT LPWSADATA lpWSAData );
typedef	SKRET	(WINAPI* PSKAPI_WSACleanup)(void);
typedef	void	(WINAPI* PSKAPI_WSASetLastError)( SKERR nErrSock );
typedef	SKERR	(WINAPI* PSKAPI_WSAGetLastError)(void);

typedef	SOCKET	(WINAPI* PSKAPI_socket)( int af, int type, int protocol );
typedef	SKRET	(WINAPI* PSKAPI_bind)( SOCKET s, const SOCKADDR* myaddr, int addr_len );
typedef	SKRET	(WINAPI* PSKAPI_connect)( SOCKET s, const SOCKADDR* addr, int addr_len );
typedef	SKRET	(WINAPI* PSKAPI_listen)( SOCKET s, int queue_len );
typedef	SOCKET	(WINAPI* PSKAPI_accept)( SOCKET s, OUT SOCKADDR* addr, OUT int* paddr_len );
typedef	SKRET	(WINAPI* PSKAPI_ioctlsocket)( SOCKET s, long cmd, IN OUT DWORD* argp );
typedef	SKRET	(WINAPI* PSKAPI_send)( SOCKET s, const char* buf, int len, int flags );
typedef	SKRET	(WINAPI* PSKAPI_sendto)( SOCKET s, const char* buf, int len, int flags, const SOCKADDR* to, int tolen );
typedef	SKRET	(WINAPI* PSKAPI_recv)( SOCKET s, OUT char* buf, int len, int flags );
typedef	SKRET	(WINAPI* PSKAPI_recvfrom)( SOCKET s, OUT char* buf, int len, int flags, OUT SOCKADDR* from, IN OUT int* fromlen );
typedef	SKRET	(WINAPI* PSKAPI_select)( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout );

typedef	SKRET	(WINAPI* PSKAPI_getsockopt)( SOCKET s, int level, int optname, OUT char* optval, IN OUT int* optlen );
typedef	SKRET	(WINAPI* PSKAPI_setsockopt)( SOCKET s, int level, int optname, const char* optval, int optlen );
typedef	SKRET	(WINAPI* PSKAPI_getpeername)( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen );
typedef	SKRET	(WINAPI* PSKAPI_getsockname)( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen );

typedef	SKRET	(WINAPI* PSKAPI_shutdown)( SOCKET s, int how );
typedef	SKRET	(WINAPI* PSKAPI_closesocket)( SOCKET s );

typedef	SKRET	(WINAPI* PSKAPI_gethostname)( OUT char* name, int namelen );
typedef	HOSTENT*	(WINAPI* PSKAPI_gethostbyname)( const char* name );
typedef	HOSTENT*	(WINAPI* PSKAPI_gethostbyaddr)( const char* addr, int len, int type );
typedef	HOSTENT*	(WINAPI* PSKAPI_gethostbynameex)( const char* name, OUT LPBYTE pBufInfo, int lenInfo );
typedef	HOSTENT*	(WINAPI* PSKAPI_gethostbyaddrex)( const char* addr, int len, int type, OUT LPBYTE pBufInfo, int lenInfo );


// IP层提供的 函数
typedef	DWORD	(WINAPI* PIPHAPI_GetNetworkParams)( OUT PFIXED_INFO pFixedInfo, IN OUT DWORD* pOutBufLen );
typedef	BOOL	(WINAPI* PIPHAPI_GetDnsSrvList)( OUT LPBYTE pDnsList, DWORD dwLenBuf, OUT DWORD* pdwCntDnsSrv );

typedef	DWORD	(WINAPI* PIPHAPI_GetNumberOfInterfaces)( OUT DWORD* pdwNumIf );
typedef	DWORD	(WINAPI* PIPHAPI_GetAdaptersInfo)( OUT PIP_ADAPTER_INFO pAdapterInfo, IN OUT DWORD* pOutBufLen );
typedef	DWORD	(WINAPI* PIPHAPI_GetInterfaceInfo)( OUT PIP_INTERFACE_INFO pIfTable, IN OUT DWORD* pdwOutBufLen );
typedef	DWORD	(WINAPI* PIPHAPI_GetIfEntry)( IN OUT PMIB_IFROW pIfRow );
typedef	DWORD	(WINAPI* PIPHAPI_GetIfTable)( OUT PMIB_IFTABLE pIfTable, IN OUT DWORD* pdwSize, BOOL bOrder );
typedef	DWORD	(WINAPI* PIPHAPI_SetIfEntry)( PMIB_IFROW pIfRow );
typedef	DWORD	(WINAPI* PIPHAPI_GetIpAddrTable)( OUT PMIB_IPADDRTABLE pIpAddrTable, IN OUT DWORD* pdwSize, BOOL bOrder );
typedef	DWORD	(WINAPI* PIPHAPI_GetBestInterface)( IPAddr dwDestAddr, OUT DWORD* pdwBestIfIndex );

typedef	DWORD	(WINAPI* PIPHAPI_SetIpTTL)( UINT nTTL );
typedef	DWORD	(WINAPI* PIPHAPI_GetIpStatistics)( OUT PMIB_IPSTATS pStats );

typedef	DWORD	(WINAPI* PIPHAPI_AddIPAddress)( IPAddr Address, IPMask IpMask, DWORD IfIndex, DWORD* NTEContext, DWORD* NTEInstance );
typedef	DWORD	(WINAPI* PIPHAPI_IpReleaseAddress)( PIP_ADAPTER_INDEX_MAP AdapterInfo );
typedef	DWORD	(WINAPI* PIPHAPI_IpRenewAddress)( PIP_ADAPTER_INDEX_MAP AdapterInfo );
typedef	DWORD	(WINAPI* PIPHAPI_NotifyAddrChange)( OUT HANDLE* Handle, LPOVERLAPPED overlapped );
typedef	DWORD	(WINAPI* PIPHAPI_NotifyRouteChange)( OUT HANDLE* Handle, LPOVERLAPPED overlapped );

typedef	DWORD	(WINAPI* PIPHAPI_CreateIpForwardEntry)( PMIB_IPFORWARDROW pRoute );
typedef	DWORD	(WINAPI* PIPHAPI_SetIpForwardEntry)( PMIB_IPFORWARDROW pRoute );
typedef	DWORD	(WINAPI* PIPHAPI_DeleteIpForwardEntry)( PMIB_IPFORWARDROW pRoute );


// ICMP层提供的 函数
typedef	DWORD	(WINAPI* PIPHAPI_GetIcmpStatistics)( OUT PMIB_ICMP pStats );

typedef	HANDLE	(WINAPI* PIPHAPI_IcmpCreateFile)(void);
typedef	BOOL	(WINAPI* PIPHAPI_IcmpCloseHandle)( HANDLE hIcmpHandle );
typedef	DWORD	(WINAPI* PIPHAPI_IcmpSendEcho)( HANDLE hIcmpHandle, IPAddr dwDesIP, LPVOID lpRequestData, WORD dwRequestSize, 
											  PIP_OPTION_INFORMATION RequestOptions, OUT LPVOID lpReplyBuffer, IN OUT DWORD* pdwReplySize, DWORD dwTimeout );



/******************************************************/

SKERR	WINAPI	Skt_WSAStartup( WORD wVersionRequired, LPWSADATA lpWSAData )
{
	EXE_API2( API_TCPIP, ESKAPI_WSAStartup, PSKAPI_WSAStartup, SKRET, wVersionRequired, lpWSAData );
}
SKRET	WINAPI	Skt_WSACleanup(void)
{
	EXE_API0( API_TCPIP, ESKAPI_WSACleanup, PSKAPI_WSACleanup, SKRET );
}
void	WINAPI	Skt_WSASetLastError( SKERR nErrSock )
{
	EXE_API1_VOID( API_TCPIP, ESKAPI_WSASetLastError, PSKAPI_WSASetLastError, nErrSock );
}
SKERR	WINAPI	Skt_WSAGetLastError(void)
{
	EXE_API0( API_TCPIP, ESKAPI_WSAGetLastError, PSKAPI_WSAGetLastError, SKRET );
}


SOCKET	WINAPI	Skt_socket( int af, int type, int protocol )
{
	EXE_API3( API_TCPIP, ESKAPI_socket, PSKAPI_socket, SOCKET, af, type, protocol );
}
SKRET	WINAPI	Skt_bind( SOCKET s, const SOCKADDR* myaddr, int addr_len )
{
	EXE_API3( API_TCPIP, ESKAPI_bind, PSKAPI_bind, SKRET, s, myaddr, addr_len );
}
SKRET	WINAPI	Skt_connect( SOCKET s, const SOCKADDR* addr, int addr_len )
{
	EXE_API3( API_TCPIP, ESKAPI_connect, PSKAPI_connect, SKRET, s, addr, addr_len );
}
SKRET	WINAPI	Skt_listen( SOCKET s, int queue_len )
{
	EXE_API2( API_TCPIP, ESKAPI_listen, PSKAPI_listen, SKRET, s, queue_len );
}
SOCKET	WINAPI	Skt_accept( SOCKET s, OUT SOCKADDR* addr, OUT int* paddr_len )
{
	EXE_API3( API_TCPIP, ESKAPI_accept, PSKAPI_accept, SKRET, s, addr, paddr_len );
}
SKRET	WINAPI	Skt_ioctlsocket( SOCKET s, long cmd, IN OUT DWORD* argp )
{
	EXE_API3( API_TCPIP, ESKAPI_ioctlsocket, PSKAPI_ioctlsocket, SKRET, s, cmd, argp );
}
SKRET	WINAPI	Skt_send( SOCKET s, const char* buf, int len, int flags )
{
	EXE_API4( API_TCPIP, ESKAPI_send, PSKAPI_send, SKRET, s, buf, len, flags );
}
SKRET	WINAPI	Skt_sendto( SOCKET s, const char* buf, int len, int flags, const SOCKADDR* to, int tolen )
{
	EXE_API6( API_TCPIP, ESKAPI_sendto, PSKAPI_sendto, SKRET, s, buf, len, flags, to, tolen );
}
SKRET	WINAPI	Skt_recv( SOCKET s, OUT char* buf, int len, int flags )
{
	EXE_API4( API_TCPIP, ESKAPI_recv, PSKAPI_recv, SKRET, s, buf, len, flags );
}
SKRET	WINAPI	Skt_recvfrom( SOCKET s, OUT char* buf, int len, int flags, OUT SOCKADDR* from, IN OUT int* fromlen )
{
	EXE_API6( API_TCPIP, ESKAPI_recvfrom, PSKAPI_recvfrom, SKRET, s, buf, len, flags, from, fromlen );
}
SKRET	WINAPI	Skt_select( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout )
{
	EXE_API5( API_TCPIP, ESKAPI_select, PSKAPI_select, SKRET, nfds, readfds, writefds, exceptfds, timeout );
}


SKRET	WINAPI	Skt_getsockopt( SOCKET s, int level, int optname, OUT char* optval, IN OUT int* optlen )
{
	EXE_API5( API_TCPIP, ESKAPI_getsockopt, PSKAPI_getsockopt, SKRET, s, level, optname, optval, optlen );
}
SKRET	WINAPI	Skt_setsockopt( SOCKET s, int level, int optname, const char* optval, int optlen )
{
	EXE_API5( API_TCPIP, ESKAPI_setsockopt, PSKAPI_setsockopt, SKRET, s, level, optname, optval, optlen );
}
SKRET	WINAPI	Skt_getpeername( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen )
{
	EXE_API3( API_TCPIP, ESKAPI_getpeername, PSKAPI_getpeername, SKRET, s, name, namelen );
}
SKRET	WINAPI	Skt_getsockname( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen )
{
	EXE_API3( API_TCPIP, ESKAPI_getsockname, PSKAPI_getsockname, SKRET, s, name, namelen );
}


SKRET	WINAPI	Skt_shutdown( SOCKET s, int how )
{
	EXE_API2( API_TCPIP, ESKAPI_shutdown, PSKAPI_shutdown, SKRET, s, how );
}
SKRET	WINAPI	Skt_closesocket( SOCKET s )
{
	EXE_API1( API_TCPIP, ESKAPI_closesocket, PSKAPI_closesocket, SKRET, s );
}


SKRET	WINAPI	Skt_gethostname( OUT char* name, int namelen )
{
	EXE_API2( API_TCPIP, ESKAPI_gethostname, PSKAPI_gethostname, SKRET, name, namelen );
}
HOSTENT*	WINAPI	Skt_gethostbyname( const char* name )
{
	EXE_API1( API_TCPIP, ESKAPI_gethostbyname, PSKAPI_gethostbyname, HOSTENT*, name );
}
HOSTENT*	WINAPI	Skt_gethostbyaddr( const char* addr, int len, int type )
{
	EXE_API3( API_TCPIP, ESKAPI_gethostbyaddr, PSKAPI_gethostbyaddr, HOSTENT*, addr, len, type );
}
HOSTENT*	WINAPI	Skt_gethostbynameex( const char* name, OUT LPBYTE pBufInfo, int len )
{
	EXE_API3( API_TCPIP, ESKAPI_gethostbynameex, PSKAPI_gethostbynameex, HOSTENT*, name, pBufInfo, len );
}
HOSTENT*	WINAPI	Skt_gethostbyaddrex( const char* addr, int len, int type, OUT LPBYTE pBufInfo, int lenInfo )
{
	EXE_API5( API_TCPIP, ESKAPI_gethostbyaddrex, PSKAPI_gethostbyaddrex, HOSTENT*, addr, len, type, pBufInfo, lenInfo );
}




// IP层提供的 函数

DWORD	WINAPI	IPH_GetNetworkParams( OUT PFIXED_INFO pFixedInfo, IN OUT DWORD* pOutBufLen )
{
	EXE_API2( API_TCPIP, EIPHAPI_GetNetworkParams, PIPHAPI_GetNetworkParams, DWORD, pFixedInfo, pOutBufLen );
}
BOOL	WINAPI	IPH_GetDnsSrvList( OUT LPBYTE pDnsList, DWORD dwLenBuf, OUT DWORD* pdwCntDnsSrv )
{
	EXE_API3( API_TCPIP, EIPHAPI_GetDnsSrvList, PIPHAPI_GetDnsSrvList, BOOL, pDnsList, dwLenBuf, pdwCntDnsSrv );
}


DWORD	WINAPI	IPH_GetNumberOfInterfaces( OUT DWORD* pdwNumIf )
{
	EXE_API1( API_TCPIP, EIPHAPI_GetNumberOfInterfaces, PIPHAPI_GetNumberOfInterfaces, DWORD, pdwNumIf );
}
DWORD	WINAPI	IPH_GetAdaptersInfo( OUT PIP_ADAPTER_INFO pAdapterInfo, IN OUT DWORD* pOutBufLen )
{
	EXE_API2( API_TCPIP, EIPHAPI_GetAdaptersInfo, PIPHAPI_GetAdaptersInfo, DWORD, pAdapterInfo, pOutBufLen );
}
DWORD	WINAPI	IPH_GetInterfaceInfo( OUT PIP_INTERFACE_INFO pIfTable, IN OUT DWORD* pdwOutBufLen )
{
	EXE_API2( API_TCPIP, EIPHAPI_GetInterfaceInfo, PIPHAPI_GetInterfaceInfo, DWORD, pIfTable, pdwOutBufLen );
}
DWORD	WINAPI	IPH_GetIfEntry( IN OUT PMIB_IFROW pIfRow )
{
	EXE_API1( API_TCPIP, EIPHAPI_GetIfEntry, PIPHAPI_GetIfEntry, DWORD, pIfRow );
}
DWORD	WINAPI	IPH_GetIfTable( OUT PMIB_IFTABLE pIfTable, IN OUT DWORD* pdwSize, BOOL bOrder )
{
	EXE_API3( API_TCPIP, EIPHAPI_GetIfTable, PIPHAPI_GetIfTable, DWORD, pIfTable, pdwSize, bOrder );
}
DWORD	WINAPI	IPH_SetIfEntry( PMIB_IFROW pIfRow )
{
	EXE_API1( API_TCPIP, EIPHAPI_SetIfEntry, PIPHAPI_SetIfEntry, DWORD, pIfRow );
}
DWORD	WINAPI	IPH_GetIpAddrTable( OUT PMIB_IPADDRTABLE pIpAddrTable, IN OUT DWORD* pdwSize, BOOL bOrder )
{
	EXE_API3( API_TCPIP, EIPHAPI_GetIpAddrTable, PIPHAPI_GetIpAddrTable, DWORD, pIpAddrTable, pdwSize, bOrder );
}
DWORD	WINAPI	IPH_GetBestInterface( IPAddr dwDestAddr, OUT DWORD* pdwBestIfIndex )
{
	EXE_API2( API_TCPIP, EIPHAPI_GetBestInterface, PIPHAPI_GetBestInterface, DWORD, dwDestAddr, pdwBestIfIndex );
}


DWORD	WINAPI	IPH_SetIpTTL( UINT nTTL )
{
	EXE_API1( API_TCPIP, EIPHAPI_SetIpTTL, PIPHAPI_SetIpTTL, DWORD, nTTL );
}
DWORD	WINAPI	IPH_GetIpStatistics( OUT PMIB_IPSTATS pStats )
{
	EXE_API1( API_TCPIP, EIPHAPI_GetIpStatistics, PIPHAPI_GetIpStatistics, DWORD, pStats );
}


DWORD	WINAPI	IPH_AddIPAddress( IPAddr Address, IPMask IpMask, DWORD IfIndex, DWORD* NTEContext, DWORD* NTEInstance )
{
	EXE_API5( API_TCPIP, EIPHAPI_AddIPAddress, PIPHAPI_AddIPAddress, DWORD, Address, IpMask, IfIndex, NTEContext, NTEInstance );
}
DWORD	WINAPI	IPH_IpReleaseAddress( PIP_ADAPTER_INDEX_MAP AdapterInfo )
{
	EXE_API1( API_TCPIP, EIPHAPI_IpReleaseAddress, PIPHAPI_IpReleaseAddress, DWORD, AdapterInfo );
}
DWORD	WINAPI	IPH_IpRenewAddress( PIP_ADAPTER_INDEX_MAP AdapterInfo )
{
	EXE_API1( API_TCPIP, EIPHAPI_IpRenewAddress, PIPHAPI_IpRenewAddress, DWORD, AdapterInfo );
}
DWORD	WINAPI	IPH_NotifyAddrChange( OUT HANDLE* Handle, LPOVERLAPPED overlapped )
{
	EXE_API2( API_TCPIP, EIPHAPI_NotifyAddrChange, PIPHAPI_NotifyAddrChange, DWORD, Handle, overlapped );
}
DWORD	WINAPI	IPH_NotifyRouteChange( OUT HANDLE* Handle, LPOVERLAPPED overlapped )
{
	EXE_API2( API_TCPIP, EIPHAPI_NotifyRouteChange, PIPHAPI_NotifyRouteChange, DWORD, Handle, overlapped );
}


DWORD	WINAPI	IPH_CreateIpForwardEntry( PMIB_IPFORWARDROW pRoute )
{
	EXE_API1( API_TCPIP, EIPHAPI_CreateIpForwardEntry, PIPHAPI_CreateIpForwardEntry, DWORD, pRoute );
}
DWORD	WINAPI	IPH_SetIpForwardEntry( PMIB_IPFORWARDROW pRoute )
{
	EXE_API1( API_TCPIP, EIPHAPI_SetIpForwardEntry, PIPHAPI_SetIpForwardEntry, DWORD, pRoute );
}
DWORD	WINAPI	IPH_DeleteIpForwardEntry( PMIB_IPFORWARDROW pRoute )
{
	EXE_API1( API_TCPIP, EIPHAPI_DeleteIpForwardEntry, PIPHAPI_DeleteIpForwardEntry, DWORD, pRoute );
}


// ICMP层提供的 函数

DWORD	WINAPI	IPH_GetIcmpStatistics( OUT PMIB_ICMP pStats )
{
	EXE_API1( API_TCPIP, EIPHAPI_GetIcmpStatistics, PIPHAPI_GetIcmpStatistics, DWORD, pStats );
}

HANDLE	WINAPI	IPH_IcmpCreateFile(void)
{
	EXE_API0( API_TCPIP, EIPHAPI_IcmpCreateFile, PIPHAPI_IcmpCreateFile, HANDLE );
}
BOOL	WINAPI	IPH_IcmpCloseHandle( HANDLE hIcmpHandle )
{
	EXE_API1( API_TCPIP, EIPHAPI_IcmpCloseHandle, PIPHAPI_IcmpCloseHandle, BOOL, hIcmpHandle );
}
DWORD	WINAPI	IPH_IcmpSendEcho( HANDLE hIcmpHandle, IPAddr dwDesIP, LPVOID lpRequestData, WORD dwRequestSize, 
											PIP_OPTION_INFORMATION RequestOptions, OUT LPVOID lpReplyBuffer, IN OUT DWORD* pdwReplySize, DWORD dwTimeout )
{
	EXE_API8( API_TCPIP, EIPHAPI_IcmpSendEcho, PIPHAPI_IcmpSendEcho, DWORD, hIcmpHandle, dwDesIP, lpRequestData, dwRequestSize, RequestOptions, lpReplyBuffer, pdwReplySize, dwTimeout );
}


