/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：DNS table
版本号：  1.0.0
开发时期：2004-03-09
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
#include "skt_call.h"
#include "iphlpapi_call.h"
#include "dns_call.h"
//"local"
#include "dns_def.h"

/***************  全局区 定义， 声明 *****************/

static	BOOL	DnsTbl_NewHostAddr( LPCSTR pszNameDns, LPBYTE pAddrList, WORD wAddrLen, WORD wCntAddr );
static	void	DnsTbl_DelHostAddr( DNS_HOSTADDR* pHAddrDns, BOOL fUseCri );

/******************************************************/


BOOL	DnsTbl_FindHostByName( OUT DNS_HOSTADDR** ppHAddrDns, LPCSTR pszNameHost )
{
	DNS_HOSTADDR*	pHAddrDns_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppHAddrDns = NULL;
	pUnitHeader = &g_lpGlobalDNS->hListHAddr;
	EnterCriticalSection( &g_lpGlobalDNS->csListHAddr );
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pHAddrDns_Find = LIST_CONTAINER( pUnit, DNS_HOSTADDR, hListHAddr );
		pUnit = pUnit->pNext;    // advance to next 

		if( (GetTickCount()-pHAddrDns_Find->dwTickLife) > TIMER_HOSTIPLIFE )
		{
			DnsTbl_DelHostAddr( pHAddrDns_Find, FALSE );
			continue;
		}
		if( stricmp( pHAddrDns_Find->pszNameDns, pszNameHost )==0 )
		{
			*ppHAddrDns = pHAddrDns_Find;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalDNS->csListHAddr );

	return (*ppHAddrDns)?TRUE:FALSE;
}


BOOL	DnsTbl_SaveHostAddr( LPCSTR pszNameDns, LPBYTE pAddrList, WORD wAddrLen, WORD wCntAddr )
{
	DNS_HOSTADDR*	pHAddrDns;

	//要么覆盖
	if( DnsTbl_FindHostByName(&pHAddrDns, pszNameDns) )
	{
		if( (pHAddrDns->wCntAddrBuf>=wCntAddr) && (pHAddrDns->wAddrLen==wAddrLen) )
		{
			//如果找到，如果 足够保存 地址列表，就覆盖它
			pHAddrDns->wCntAddrReal = wCntAddr;
			memcpy( pHAddrDns->pAddrList, pAddrList, wAddrLen*wCntAddr );
			return TRUE;
		}
		DnsTbl_DelHostAddr( pHAddrDns, TRUE );
	}
	//要么新建
	return DnsTbl_NewHostAddr( pszNameDns, pAddrList, wAddrLen, wCntAddr );
}

void	DnsTbl_DelHostAddr( DNS_HOSTADDR* pHAddrDns, BOOL fUseCri )
{
	//
	if( !HANDLE_CHECK(pHAddrDns) )
	{
		return ;
	}
	//
	if( fUseCri )
	{
		EnterCriticalSection( &g_lpGlobalDNS->csListHAddr );
	}
	List_RemoveUnit( &pHAddrDns->hListHAddr );
	if( fUseCri )
	{
		LeaveCriticalSection( &g_lpGlobalDNS->csListHAddr );
	}

	//
	//Sleep( 1 );		//lilin remove
	//Sleep( 1 );		//lilin remove
	//Sleep( 1 );		//lilin remove
	//
	HANDLE_FREE( pHAddrDns );

	return ;
}


BOOL	DnsTbl_NewHostAddr( LPCSTR pszNameDns, LPBYTE pAddrList, WORD wAddrLen, WORD wCntAddr )
{
	DNS_HOSTADDR*		pHAddrDns;
	DWORD				dwLenNameDns;
	DWORD				dwLenListAddr;

	//计算 pszNameDns长度 和 所需pAddrList长度，分配整个长度，基本初始化
	dwLenNameDns = strlen( pszNameDns );
	dwLenListAddr = wAddrLen*(wCntAddr+CNTADDR_GROW);
	pHAddrDns = (DNS_HOSTADDR*)HANDLE_ALLOC( sizeof(DNS_HOSTADDR) +dwLenListAddr +dwLenNameDns+4 );
	if( !pHAddrDns )
	{
		return FALSE;
	}
	HANDLE_INIT( pHAddrDns, sizeof(DNS_HOSTADDR) );
	List_InitHead( &pHAddrDns->hListHAddr );
	//初始化
	pHAddrDns->dwTickLife = GetTickCount();
	pHAddrDns->pAddrList = (LPBYTE)pHAddrDns + sizeof(DNS_HOSTADDR);
	pHAddrDns->pszNameDns = pHAddrDns->pAddrList +dwLenListAddr;
	//保存 地址列表
	pHAddrDns->wAddrLen = wAddrLen;
	pHAddrDns->wCntAddrBuf = wCntAddr+CNTADDR_GROW;
	pHAddrDns->wCntAddrReal = wCntAddr;
	memcpy( pHAddrDns->pAddrList, pAddrList, wCntAddr*wAddrLen );
	//保存 pszNameDns
	memcpy( pHAddrDns->pszNameDns, pszNameDns, dwLenNameDns );
	pHAddrDns->pszNameDns[dwLenNameDns] = 0;

	//添加 关联
	EnterCriticalSection( &g_lpGlobalDNS->csListHAddr );
	List_InsertTail( &g_lpGlobalDNS->hListHAddr, &pHAddrDns->hListHAddr );
	LeaveCriticalSection( &g_lpGlobalDNS->csListHAddr );

	return TRUE;
}




