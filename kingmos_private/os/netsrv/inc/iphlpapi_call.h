/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _IPHLPAPI_CALL_H_
#define _IPHLPAPI_CALL_H_

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------
//
// IP层提供的 函数
//
// -------------------------------------------------------------

//
extern	DWORD	WINAPI	IPHAPI_GetNetworkParams( OUT PFIXED_INFO pFixedInfo, IN OUT DWORD* pOutBufLen );

extern	BOOL	WINAPI	IPHAPI_GetDnsSrvList( OUT DWORD* pdwDnsList, DWORD dwCntMax );


//IP接口的操作
extern	DWORD	WINAPI	IPHAPI_GetLocalIPList( OUT DWORD* pdwIPList, DWORD dwCntMax );

extern	DWORD	WINAPI	IPHAPI_GetNumberOfInterfaces( OUT DWORD* pdwNumIf );

extern	DWORD	WINAPI	IPHAPI_GetAdaptersInfo( OUT PIP_ADAPTER_INFO pAdapterInfo, IN OUT DWORD* pOutBufLen );
extern	DWORD	WINAPI	IPHAPI_GetInterfaceInfo( OUT PIP_INTERFACE_INFO pIfTable, IN OUT DWORD* pdwOutBufLen );

extern	DWORD	WINAPI	IPHAPI_GetIfEntry( IN OUT PMIB_IFROW pIfRow );
extern	DWORD	WINAPI	IPHAPI_GetIfTable( OUT PMIB_IFTABLE pIfTable, IN OUT DWORD* pdwSize, BOOL bOrder );
extern	DWORD	WINAPI	IPHAPI_SetIfEntry( PMIB_IFROW pIfRow );

extern	BOOL	WINAPI	IPHAPI_GetIpAddrList( LPBYTE pAddrList, DWORD dwLenBuf, OUT DWORD* pdwCntAddr );

extern	DWORD	WINAPI	IPHAPI_GetIpAddrTable( OUT PMIB_IPADDRTABLE pIpAddrTable, IN OUT DWORD* pdwSize, BOOL bOrder );

extern	DWORD	WINAPI	IPHAPI_GetBestInterface( IPAddr dwDestAddr, OUT DWORD* pdwBestIfIndex );


//
extern	DWORD	WINAPI	IPHAPI_SetIpTTL( UINT nTTL );

extern	DWORD	WINAPI	IPHAPI_GetIpStatistics( OUT PMIB_IPSTATS pStats );


//IP地址的操作
extern	DWORD	WINAPI	IPHAPI_AddIPAddress( IPAddr Address, IPMask IpMask, DWORD IfIndex, DWORD* NTEContext, DWORD* NTEInstance );

extern	DWORD	WINAPI	IPHAPI_IpReleaseAddress( PIP_ADAPTER_INDEX_MAP AdapterInfo );
extern	DWORD	WINAPI	IPHAPI_IpRenewAddress( PIP_ADAPTER_INDEX_MAP AdapterInfo );

extern	DWORD	WINAPI	IPHAPI_NotifyAddrChange( OUT HANDLE* Handle, LPOVERLAPPED overlapped );
extern	DWORD	WINAPI	IPHAPI_NotifyRouteChange( OUT HANDLE* Handle, LPOVERLAPPED overlapped );


//IP转发的操作
extern	DWORD	WINAPI	IPHAPI_CreateIpForwardEntry( PMIB_IPFORWARDROW pRoute );
extern	DWORD	WINAPI	IPHAPI_SetIpForwardEntry( PMIB_IPFORWARDROW pRoute );
extern	DWORD	WINAPI	IPHAPI_DeleteIpForwardEntry( PMIB_IPFORWARDROW pRoute );


// -------------------------------------------------------------
//
// ICMP层提供的 函数
//
// -------------------------------------------------------------

extern	DWORD	WINAPI	IPHAPI_GetIcmpStatistics( OUT PMIB_ICMP pStats );

extern	HANDLE	WINAPI	IPHAPI_IcmpCreateFile(void);
extern	BOOL	WINAPI	IPHAPI_IcmpCloseHandle( HANDLE hIcmpHandle );
extern	DWORD	WINAPI	IPHAPI_IcmpSendEcho( HANDLE hIcmpHandle, IPAddr dwDesIP, LPVOID lpRequestData, WORD dwRequestSize, 
											PIP_OPTION_INFORMATION RequestOptions, OUT LPVOID lpReplyBuffer, IN OUT DWORD* pdwReplySize, DWORD dwTimeout );


#ifdef __cplusplus
}	
#endif

#endif	//_IPHLPAPI_CALL_H_

