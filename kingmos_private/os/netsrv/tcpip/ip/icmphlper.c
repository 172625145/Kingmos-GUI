/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：ICMP HELPER
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
#include "icmp_def.h"


/***************  全局区 定义， 声明 *****************/

static	BOOL	IcmpE_Create( ECHO_ITEM** ppItemEcho );
static	void	IcmpE_Delete( ECHO_ITEM* pItemEcho );

/******************************************************/


DWORD	WINAPI	IPHAPI_GetIcmpStatistics( OUT PMIB_ICMP pStats )
{
	return 0;
}


HANDLE	WINAPI	IPHAPI_IcmpCreateFile(void)
{
	ECHO_ITEM*			pItemEcho;

	IcmpE_Create( &pItemEcho );
	return (HANDLE)pItemEcho;
}

BOOL	WINAPI	IPHAPI_IcmpCloseHandle( HANDLE hIcmpHandle )
{
	ECHO_ITEM*			pItemEcho = (ECHO_ITEM*)hIcmpHandle;
	int					i;

	//句柄安全检查
	if( !HANDLE_CHECK(pItemEcho) )
	{
		return 0;
	}
	//
	EnterCriticalSection( &g_lpGlobalIcmp->csListEcho );
	if( pItemEcho->wFlag & ECHOITEMF_EXIT )
	{
		LeaveCriticalSection( &g_lpGlobalIcmp->csListEcho );
		return 0;
	}
	pItemEcho->wFlag |= ECHOITEMF_EXIT;
	List_RemoveUnit( &pItemEcho->hListEcho );
	LeaveCriticalSection( &g_lpGlobalIcmp->csListEcho );
	//等待所有的退出
	for( i=0; ((pItemEcho->wCntUsed) && (i<300)); i++ )
	{
		SetEvent( pItemEcho->hEvtReply );
		Sleep( 10 );
	}
	//开始释放
	IcmpE_Delete( pItemEcho );

	return TRUE;
}

DWORD	WINAPI	IPHAPI_IcmpSendEcho( HANDLE hIcmpHandle, IPAddr dwDesIP, LPVOID lpRequestData, WORD dwRequestSize, PIP_OPTION_INFORMATION RequestOptions, OUT LPVOID lpReplyBuffer, IN OUT DWORD* pdwReplySize, DWORD dwTimeout )
{
	ECHO_ITEM*			pItemEcho = (ECHO_ITEM*)hIcmpHandle;
	LPBYTE				pBufEcho;
	ICMP_ECHO_HDR*		pHdrEcho;
	IP_HDR*				pHdrIP;

	BYTE				pBufFragSet[sizeof(FRAGSETA)];
	FRAGSETA*			pFragSetA = (FRAGSETA*)pBufFragSet;
	DWORD				dwRet;

	ICMP_ECHO_REPLY*	pItemIcmpReply;
	DWORD				dwSize;
	WORD				wLenTotal;
	DWORD				wLenEcho;

	//
	if( pdwReplySize )
	{
		dwSize = *pdwReplySize;
		*pdwReplySize = 0;
	}
	else
	{
		dwSize = 0;
	}
	//句柄安全检查
	if( HANDLE_F_FAIL(g_lpGlobalIcmp, GICMPF_EXIT) )
	{
		return ERROR_INVALID_HANDLE;
	}
	if( HANDLE_F_FAIL(pItemEcho, ECHOITEMF_EXIT) )
	{
		return ERROR_INVALID_HANDLE;
	}
	//参数检查
	if( (dwDesIP==INADDR_ANY) || INADDR_TEST_BC(dwDesIP) )
	{
		return ERROR_INVALID_PARAMETER;
	}
	if( !lpRequestData )
	{
		dwRequestSize = 0;
	}
	if( dwRequestSize>8192 )
	{
		return ERROR_INVALID_PARAMETER;
	}
	//分析当前状态
	EnterCriticalSection( &g_lpGlobalIcmp->csListEcho );
	if( pItemEcho->wFlag & ECHOITEMF_EXIT )
	{
		LeaveCriticalSection( &g_lpGlobalIcmp->csListEcho );
		return ERROR_INVALID_HANDLE;
	}
	if( pItemEcho->wCntUsed++ )
	{
		SetEvent( pItemEcho->hEvtReply );
	}
	pItemEcho->wFlag &= ~ECHOITEMF_REPLY;
	LeaveCriticalSection( &g_lpGlobalIcmp->csListEcho );
	//统计要发送的长度
	wLenTotal = sizeof(IP_HDR) + sizeof(ICMP_ECHO_HDR) + dwRequestSize;
	wLenEcho = wLenTotal;
	if( !(pItemEcho->pBufEcho) || (wLenEcho > pItemEcho->wLenEcho) )
	{
		pBufEcho = (LPBYTE)malloc( wLenEcho );
		if( !pBufEcho )
		{
			pItemEcho->wCntUsed --;
			return ERROR_NOT_ENOUGH_MEMORY;
		}
		memset( pBufEcho, 0, wLenEcho );
		if( pItemEcho->pBufEcho )
		{
			free( pItemEcho->pBufEcho );
		}
		pItemEcho->pBufEcho = pBufEcho;
	}
	else
	{
		pBufEcho = pItemEcho->pBufEcho;
	}
	pItemEcho->dwDesIP = dwDesIP;
	pHdrIP = (IP_HDR*)pBufEcho;
	pHdrEcho = (ICMP_ECHO_HDR*)(pBufEcho+sizeof(IP_HDR));
	//
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrEcho;
	pFragSetA->pFrag[0].nLen = sizeof(ICMP_ECHO_HDR) + dwRequestSize;
	//
	if( dwRequestSize )
	{
		memcpy( (LPBYTE)pHdrEcho+sizeof(ICMP_ECHO_HDR), lpRequestData, dwRequestSize );
	}
	//
	pHdrEcho->bType = ICMP_TYPE_ECHO;
	pHdrEcho->bCode = 0;
	pHdrEcho->wXID = pItemEcho->wXID;
	pHdrEcho->wSeqNum = ++pItemEcho->wSeqNum;
	pHdrEcho->wTotalCRC = 0;
	pHdrEcho->wTotalCRC = IP_CRC((LPBYTE)pHdrEcho, (WORD)(sizeof(ICMP_ECHO_HDR) + dwRequestSize) );
	//
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_ICMP;
	pHdrIP->dwDesIP = dwDesIP;
	//pHdrIP->wFragment = 0;
	pHdrIP->wLenTotal = wLenTotal;
	//
	if( pItemEcho->wFlag & ECHOITEMF_EXIT )
	{
		pItemEcho->wCntUsed --;
		return ERROR_INVALID_HANDLE;
	}
	pItemEcho->dwTickTx = GetTickCount();
	dwRet = IP_NetOut2( IPTXOPT_INTF_MULT, pHdrIP, pFragSetA );
	if( !dwRet )
	{
		pItemEcho->wCntUsed --;
		return ERROR_FUNCTION_FAILED;
	}
	//等待结果
	if( pItemEcho->wFlag & ECHOITEMF_EXIT )
	{
		pItemEcho->wCntUsed --;
		return ERROR_INVALID_HANDLE;
	}
	ResetEvent( pItemEcho->hEvtReply );
	WaitForSingleObject( pItemEcho->hEvtReply, dwTimeout );
	//分析结果
	if( pItemEcho->wFlag & ECHOITEMF_EXIT )
	{
		pItemEcho->wCntUsed --;
		return ERROR_INVALID_HANDLE;
	}
	if( pItemEcho->wFlag & ECHOITEMF_REPLY )
	{
		//获取信息
		if( dwSize>=sizeof(ICMP_ECHO_REPLY) )
		{
			pItemIcmpReply = (ICMP_ECHO_REPLY*)lpReplyBuffer;
			memset( pItemIcmpReply, 0, dwSize );
			pItemIcmpReply->RoundTripTime = GetTickCount() - pItemEcho->dwTickTx;
			pItemIcmpReply->Address = pItemEcho->dwDesIP;
			pItemIcmpReply->Options.Ttl = pItemEcho->Options.Ttl;

			if( dwSize>=(sizeof(ICMP_ECHO_REPLY)+pItemEcho->wLenReply) )
			{
				memcpy( (LPBYTE)pItemIcmpReply+sizeof(ICMP_ECHO_REPLY), pItemEcho->pBufReply, pItemEcho->wLenReply );
				pItemIcmpReply->DataSize = pItemEcho->wLenReply;
				pItemIcmpReply->Data = pItemEcho->pBufReply;

				*pdwReplySize = (sizeof(ICMP_ECHO_REPLY)+pItemEcho->wLenReply);

			}
			else
			{
				*pdwReplySize = sizeof(ICMP_ECHO_REPLY);
			}
		}

		pItemEcho->wCntUsed --;
		return ERROR_SUCCESS;
	}
	else
	{
		pItemEcho->wCntUsed --;
		return ERROR_TIMEOUT;
	}
}


DWORD	Icmp_HandleEchoReply( IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData )
{
	ICMP_ECHO_HDR*	pHdrEcho = (ICMP_ECHO_HDR*)pData;
	ECHO_ITEM*		pItemEcho;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	WORD			wLenIcmpData;

	if( HANDLE_F_FAIL(g_lpGlobalIcmp, GICMPF_EXIT) )
	{
		return 0;
	}
	pUnitHeader = &g_lpGlobalIcmp->hListEcho;
	EnterCriticalSection( &g_lpGlobalIcmp->csListEcho );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIcmp->wFlag & GICMPF_EXIT) )
	{
		pItemEcho = LIST_CONTAINER( pUnit, ECHO_ITEM, hListEcho );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pItemEcho) )
		{
			break;
		}
		if( pItemEcho->wFlag & ECHOITEMF_EXIT )
		{
			continue;
		}
		//本地比较
		if( (pItemEcho->dwDesIP==pHdrIP->dwSrcIP) && 
			(pItemEcho->wXID==pHdrEcho->wXID) && 
			(pItemEcho->wSeqNum==pHdrEcho->wSeqNum) && 
			!(pItemEcho->wFlag & ECHOITEMF_REPLY)
		  )
		{
			//请拷贝信息，并退出
			//
			pItemEcho->wFlag |= ECHOITEMF_REPLY;
			//
			wLenIcmpData = wLenData - sizeof(ICMP_ECHO_HDR);
			if( !pItemEcho->pBufReply || (pItemEcho->wLenReply_Buf < wLenIcmpData) )
			{
				pItemEcho->wLenReply_Buf = 0;
				if( pItemEcho->pBufReply )
				{
					free( pItemEcho->pBufReply );
					pItemEcho->pBufReply = NULL;
				}
				pItemEcho->pBufReply = (LPBYTE)malloc( wLenIcmpData+100 );
				if( pItemEcho->pBufReply )
				{
					pItemEcho->wLenReply_Buf = wLenIcmpData+100;
				}
			}
			if( pItemEcho->wLenReply_Buf >= wLenIcmpData )
			{
				memcpy( pItemEcho->pBufReply, pData+sizeof(ICMP_ECHO_HDR), wLenIcmpData );
				pItemEcho->wLenReply = wLenIcmpData;
			}
			//
			memset( &pItemEcho->Options, 0, sizeof(IP_OPTION_INFORMATION) );
			pItemEcho->Options.Ttl = pHdrIP->bTTL;
			pItemEcho->Options.Tos = pHdrIP->bTOS;
			//
			SetEvent( pItemEcho->hEvtReply );
			break;
		}
	}
	//
	LeaveCriticalSection( &g_lpGlobalIcmp->csListEcho );

	return 0;
}


////////////////////////////////////////////////////////

BOOL	IcmpE_Create( ECHO_ITEM** ppItemEcho )
{
	ECHO_ITEM*		pItemEcho;

	//分配1个TCP的功能 和基本初始化
	*ppItemEcho = NULL;
	pItemEcho = (ECHO_ITEM*)HANDLE_ALLOC( sizeof(ECHO_ITEM) );
	if( !pItemEcho )
	{
		return FALSE;
	}
	HANDLE_INIT( pItemEcho, sizeof(ECHO_ITEM) );
	List_InitHead( &pItemEcho->hListEcho );
	//
	pItemEcho->hEvtReply = CreateEvent( NULL, FALSE, FALSE, NULL );
	pItemEcho->hProc = GetCallerProcess( );
	pItemEcho->wXID = g_lpGlobalIcmp->wXID++;

	//关联到
	EnterCriticalSection( &g_lpGlobalIcmp->csListEcho );
	List_InsertTail( &g_lpGlobalIcmp->hListEcho, &pItemEcho->hListEcho );
	LeaveCriticalSection( &g_lpGlobalIcmp->csListEcho );

	*ppItemEcho = pItemEcho;
	return TRUE;
}

void	IcmpE_Delete( ECHO_ITEM* pItemEcho )
{
	//句柄安全检查
	if( !HANDLE_CHECK(pItemEcho) )
	{
		return ;
	}
	if( pItemEcho->hEvtReply )
	{
		CloseHandle( pItemEcho->hEvtReply );
	}
	if( pItemEcho->pBufEcho )
	{
		free( pItemEcho->pBufEcho );
	}
	if( pItemEcho->pBufReply )
	{
		free( pItemEcho->pBufReply );
	}

	//释放句柄
	HANDLE_FREE( pItemEcho );
}

//BOOL	IcmpE_Find(  )
//{
//}





