/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：IP
版本号：  1.0.0
开发时期：2003-12-01
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <socket.h>
#include <iphlpapi.h>
//"\inc_local"
#include "proto_ip_tcp.h"
#include "netif_ip_dl.h"
#include "netif_tcp_ip.h"
#include "iphlpapi_call.h"
//"local"
#include "ip_def.h"


/***************  全局区 定义， 声明 *****************/



/******************************************************/



// -------------------------------------------------------------
// Network 的操作
// -------------------------------------------------------------
//获取 本机Network信息
DWORD	WINAPI	IPHAPI_GetNetworkParams( OUT PFIXED_INFO pFixedInfo, IN OUT DWORD* pOutBufLen )
{
	*pOutBufLen = 0;
	return 0;
}


BOOL	WINAPI	IPHAPI_GetDnsSrvList( OUT DWORD* pdwDnsList, DWORD dwCntMax )
{
	DWORD			dwCnt;
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//检查参数
	if( !dwCntMax )
	{
		return FALSE;
	}
	//网络检查
	if( HANDLE_F_FAIL(g_lpGlobalIP, GIPF_EXIT) )
	{
		return FALSE;
	}
	//准备
	dwCnt = 0;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	//枚举所有的 Dns Server
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		//根据结构成员地址，得到结构地址，并继续
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pNetIntf_Find) )
		{
			break;
		}
		if( pNetIntf_Find->wFlag & IPIFF_EXIT )
		{
			continue;
		}

		//保存信息
		if( pdwDnsList )
		{
			if( dwCnt>=dwCntMax )
			{
				break;
			}
			if( pNetIntf_Find->dwIP_DNS )
			{
				memcpy( pdwDnsList, &pNetIntf_Find->dwIP_DNS, ADDRLEN_IPV4 );
				pdwDnsList ++;
				dwCnt ++;
			}
			//
			if( dwCnt>=dwCntMax )
			{
				break;
			}
			if( pNetIntf_Find->dwIP_DNS2 )
			{
				memcpy( pdwDnsList, &pNetIntf_Find->dwIP_DNS2, ADDRLEN_IPV4 );
				pdwDnsList ++;
				dwCnt ++;
			}
		}
		else
		{
			dwCnt ++;
		}
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	return dwCnt?TRUE:FALSE;
}


// -------------------------------------------------------------
// IP接口 和 默认信息 的操作
// -------------------------------------------------------------

DWORD	WINAPI	IPHAPI_GetLocalIPList( OUT DWORD* pdwIPList, DWORD dwCntMax )
{
	DWORD			dwCnt;
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//检查参数
	if( dwCntMax==0 )
	{
		return FALSE;
	}
	//网络检查
	if( HANDLE_F_FAIL(g_lpGlobalIP, GIPF_EXIT) )
	{
		return FALSE;
	}
	//准备
	dwCnt = 0;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	//枚举所有的 Dns Server
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		//根据结构成员地址，得到结构地址，并继续
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pNetIntf_Find) )
		{
			break;
		}
		if( pNetIntf_Find->wFlag & IPIFF_EXIT )
		{
			continue;
		}

		//保存信息
		if( pdwIPList )
		{
			if( dwCnt>=dwCntMax )
			{
				break;
			}
			memcpy( pdwIPList, &pNetIntf_Find->dwIP, ADDRLEN_IPV4 );
			pdwIPList += 1;
			dwCnt ++;
		}
		else
		{
			dwCnt ++;
		}
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	return TRUE;
}

//获取 所有IP接口的 个数
DWORD	WINAPI	IPHAPI_GetNumberOfInterfaces( OUT DWORD* pdwNumIf )
{
	DWORD			dwCnt;
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//检查参数
	if( !pdwNumIf )
	{
		return ERROR_INVALID_PARAMETER;
	}
	//网络检查
	if( HANDLE_F_FAIL(g_lpGlobalIP, GIPF_EXIT) )
	{
		*pdwNumIf = 0;
		return ERROR_SUCCESS;
	}
	//准备
	dwCnt = 0;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	//
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//
		if( HANDLE_F_FAIL(pNetIntf_Find, IPIFF_EXIT) )
		{
			break;
		}
		dwCnt ++;
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	*pdwNumIf = dwCnt;
	return ERROR_SUCCESS;
}

//设置 指定IP接口Adapter的 详细信息
DWORD	WINAPI	IPHAPI_GetAdaptersInfo( OUT PIP_ADAPTER_INFO pAdapterInfo, IN OUT DWORD* pOutBufLen )
{
	return 0;
}
//获取 所有IP接口的 列表（只有Index and AdapterName）
DWORD	WINAPI	IPHAPI_GetInterfaceInfo( OUT PIP_INTERFACE_INFO pIfTable, IN OUT DWORD* pdwOutBufLen )
{
	IP_ADAPTER_INDEX_MAP*	pItem;
	DWORD			dwCntMax;
	DWORD			dwCnt;
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//参数检查
	if( pdwOutBufLen==NULL )
	{
		return ERROR_INVALID_PARAMETER;
	}
	//网络检查
	if( HANDLE_F_FAIL(g_lpGlobalIP, GIPF_EXIT) )
	{
		*pdwOutBufLen = 0;
		return 0;
	}
	//判断 所有IP接口的 列表需要的内存
	if( pIfTable==NULL )
	{
		IPHAPI_GetNumberOfInterfaces( &dwCntMax );
		if( dwCntMax==0 )
		{
			*pdwOutBufLen = 0;
		}
		else
		{
			*pdwOutBufLen = sizeof(IP_INTERFACE_INFO) + (dwCntMax-1)*sizeof(IP_ADAPTER_INDEX_MAP);
		}
		return 0;
	}
	else if( *pdwOutBufLen<sizeof(IP_INTERFACE_INFO) )
	{
		return ERROR_INSUFFICIENT_BUFFER;
	}
	else
	{
		dwCntMax = (*pdwOutBufLen - sizeof(IP_INTERFACE_INFO)) / sizeof(IP_ADAPTER_INDEX_MAP) + 1;
	}
	//准备
	memset( pIfTable, 0, (*pdwOutBufLen) );
	pItem = pIfTable->Adapter;
	dwCnt = 0;
	*pdwOutBufLen = 0;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	//枚举所有的 IP接口
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pNetIntf_Find) )
		{
			break;
		}
		if( pNetIntf_Find->wFlag & IPIFF_EXIT )
		{
			continue;
		}

		//保存信息 Index、NAME
		pItem->Index = pNetIntf_Find->dwIndex;
		(*pNetIntf_Find->pFnDlName)( pNetIntf_Find->hIfDl, pItem->szName, MAX_ADAPTER_NAME );
		pItem ++;
		//
		if( ++dwCnt>=dwCntMax )
		{
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	pIfTable->NumAdapters = dwCnt;
	if( dwCnt )
	{
		*pdwOutBufLen = sizeof(IP_INTERFACE_INFO) + (dwCnt-1)*sizeof(IP_ADAPTER_INDEX_MAP);
	}
	return ERROR_SUCCESS;
}



//获取 指定IP接口的 详细信息
DWORD	WINAPI	IPHAPI_GetIfEntry( IN OUT PMIB_IFROW pIfRow )
{
	//IP_INTF*		pNetIntf;

	//pNetIntf = (IP_INTF*)IP_GetIntfByIndex( pIfRow->dwIndex );
	//if( pNetIntf )
	//{
	//	if( (*pNetIntf->pFnDlGetMIB)( pNetIntf->hIfDl, pIfRow ) )
	//	{
	//		return ERROR_SUCCESS;
	//	}
	//}

	return ERROR_SUCCESS;
}
//获取 所有IP接口的 详细信息
DWORD	WINAPI	IPHAPI_GetIfTable( OUT PMIB_IFTABLE pIfTable, IN OUT DWORD* pdwSize, BOOL bOrder )
{
	return 0;
}
//设置 指定IP接口的 详细信息
DWORD	WINAPI	IPHAPI_SetIfEntry( PMIB_IFROW pIfRow )
{
	return 0;
}


BOOL	WINAPI	IPHAPI_GetIpAddrList( LPBYTE pAddrList, DWORD dwLenBuf, OUT DWORD* pdwCntAddr )
{
	DWORD			dwCnt;
	DWORD			dwCntMax;
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//参数检查
	if( pdwCntAddr==NULL )
	{
		return FALSE;
	}
	//网络检查
	if( HANDLE_F_FAIL(g_lpGlobalIP, GIPF_EXIT) )
	{
		*pdwCntAddr = 0;
		return FALSE;
	}
	//判断 所有IP接口的 列表需要的内存
	if( (pAddrList==NULL) || (dwLenBuf<ADDRLEN_IPV4) )
	{
		IPHAPI_GetNumberOfInterfaces( &dwCnt );
		*pdwCntAddr = dwCnt;
		return dwCnt?TRUE:FALSE;
	}
	//准备
	memset( pAddrList, 0, dwLenBuf );
	dwCnt = 0;
	dwCntMax = dwLenBuf / ADDRLEN_IPV4;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	//枚举所有的 IP接口
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pNetIntf_Find) )
		{
			break;
		}
		if( pNetIntf_Find->wFlag & IPIFF_EXIT )
		{
			continue;
		}
		//保存信息 Index、NAME
		memcpy( pAddrList, &pNetIntf_Find->dwIP, ADDRLEN_IPV4 );
		pAddrList += ADDRLEN_IPV4;
		//
		if( (++dwCnt)>=dwCntMax )
		{
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	*pdwCntAddr = dwCnt;
	return TRUE;
}

//
DWORD	WINAPI	IPHAPI_GetIpAddrTable( OUT PMIB_IPADDRTABLE pIpAddrTable, IN OUT DWORD* pdwSize, BOOL bOrder )
{
	MIB_IPADDRROW*	pItem;
	DWORD			dwCntMax;
	DWORD			dwCnt;
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//参数检查
	if( pdwSize==NULL )
	{
		return ERROR_INVALID_PARAMETER;
	}
	//网络检查
	if( HANDLE_F_FAIL(g_lpGlobalIP, GIPF_EXIT) )
	{
		*pdwSize = 0;
		return 0;
	}
	//判断 所有IP接口的 列表需要的内存
	if( pIpAddrTable==NULL )
	{
		IPHAPI_GetNumberOfInterfaces( &dwCntMax );
		if( dwCntMax==0 )
		{
			*pdwSize = 0;
		}
		else
		{
			*pdwSize = sizeof(MIB_IPADDRTABLE) + (dwCntMax-1)*sizeof(MIB_IPADDRROW);
		}
		return 0;
	}
	else if( *pdwSize<sizeof(MIB_IPADDRTABLE) )
	{
		return ERROR_INSUFFICIENT_BUFFER;
	}
	else
	{
		dwCntMax = (*pdwSize - sizeof(MIB_IPADDRTABLE)) / sizeof(MIB_IPADDRROW) + 1;
	}
	//准备
	memset( pIpAddrTable, 0, (*pdwSize) );
	pItem = pIpAddrTable->table;
	dwCnt = 0;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	*pdwSize = 0;
	//枚举所有的 IP接口
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pNetIntf_Find) )
		{
			break;
		}
		if( pNetIntf_Find->wFlag & IPIFF_EXIT )
		{
			continue;
		}
		//保存信息 Index、NAME
		pItem->dwIndex = pNetIntf_Find->dwIndex;
		pItem->dwAddr = pNetIntf_Find->dwIP;
		pItem->dwMask = pNetIntf_Find->dwIP_Mask;
		pItem ++;
		//
		if( ++dwCnt>=dwCntMax )
		{
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	pIpAddrTable->dwNumEntries = dwCnt;
	if( dwCnt )
	{
		*pdwSize = sizeof(MIB_IPADDRTABLE) + (dwCnt-1)*sizeof(MIB_IPADDRROW);
	}
	return ERROR_SUCCESS;
}



//
DWORD	WINAPI	IPHAPI_GetBestInterface( IPAddr dwDestAddr, OUT DWORD* pdwBestIfIndex )
{
	IP_INTF*		pNetIntf;

	if( IP_FindSubnet( &pNetIntf, dwDestAddr, FALSE ) )
	{
		*pdwBestIfIndex = pNetIntf->dwIndex;
		return TRUE;
	}
	else
	{
		return 1;
	}
}

// -------------------------------------------------------------
// IP 的操作
// -------------------------------------------------------------
//设置 默认的TTL
DWORD	WINAPI	IPHAPI_SetIpTTL( UINT nTTL )
{
	return 0;
}


//获取 IP接口的 统计信息
DWORD	WINAPI	IPHAPI_GetIpStatistics( OUT PMIB_IPSTATS pStats )
{
	return 0;
}


// -------------------------------------------------------------
// IP地址 的操作
// -------------------------------------------------------------



//设置 指定IP接口的 地址信息等
//DWORD	SetIPAddress( DWORD IfIndex,  )
//{
//	return 0;
//}


//添加 指定IP接口的 地址
DWORD	WINAPI	IPHAPI_AddIPAddress( IPAddr Address, IPMask IpMask, DWORD IfIndex, DWORD* NTEContext, DWORD* NTEInstance )
{
	return 0;
}

//DHCP 释放指定IP接口的 地址
DWORD	WINAPI	IPHAPI_IpReleaseAddress( PIP_ADAPTER_INDEX_MAP AdapterInfo )
{
	return 0;
}

//DHCP 重获指定IP接口的 地址
DWORD	WINAPI	IPHAPI_IpRenewAddress( PIP_ADAPTER_INDEX_MAP AdapterInfo )
{
	return 0;
}


//设置 IP地址变化时通知
DWORD	WINAPI	IPHAPI_NotifyAddrChange( OUT HANDLE* Handle, LPOVERLAPPED overlapped )
{
	return 0;
}

//设置 路由地址变化时通知
DWORD	WINAPI	IPHAPI_NotifyRouteChange( OUT HANDLE* Handle, LPOVERLAPPED overlapped )
{
	return 0;
}



// -------------------------------------------------------------
// IP转发 的操作
// -------------------------------------------------------------

DWORD	WINAPI	IPHAPI_CreateIpForwardEntry( PMIB_IPFORWARDROW pRoute )
{
	return 0;
}

DWORD	WINAPI	IPHAPI_SetIpForwardEntry( PMIB_IPFORWARDROW pRoute )
{
	return 0;
}

DWORD	WINAPI	IPHAPI_DeleteIpForwardEntry( PMIB_IPFORWARDROW pRoute )
{
	return 0;
}












