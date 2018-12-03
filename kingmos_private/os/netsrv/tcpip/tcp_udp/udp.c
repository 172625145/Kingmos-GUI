/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：UDP 
版本号：  1.0.0
开发时期：2004-03-09
作者：    肖远钢
修改记录：

 本版本TCPIP 说明（肖远钢 2004-04-12 写）：
1。 这套代码的完成，是在下述资料的肩膀上，重新写的，并集成了其中的相关机制。
    参考资料：RFC/IANA、WHL的TCPIP版本、TCPIP详解、网际互连
2。 支持Block和Nonblock操作
3。 
4。 

******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <socket.h>
//"\inc_local"
#include "proto_ip_tcp.h"
#include "netif_tcp_ip.h"
#include "netif_skinst.h"
//"local"
#include "tcp_udp.h"
#include "udp_def.h"


/***************  全局区 定义， 声明 *****************/
UDP_GLOBAL*		g_lpGlobalUdp = NULL;


//
extern	BOOL	UdpSk_Init( UDP_GLOBAL* lpGlobalUdp );


//
void	Udp_DeleteTCB( UDP_TCB* pTcbUdp );
void	Udp_Free( UDP_GLOBAL* lpGlobalUdp );



/******************************************************/



// ********************************************************************
// 声明：
// 参数：
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：初始化UDP
// 引用: 
// ********************************************************************
BOOL	Udp_Init( )
{
	UDP_GLOBAL*		lpGlobalUdp;

	//分配1个UDP的功能 和基本初始化
	lpGlobalUdp = (UDP_GLOBAL*)HANDLE_ALLOC( sizeof(UDP_GLOBAL) );
	if( !lpGlobalUdp )
	{
		return FALSE;
	}
	HANDLE_INIT( lpGlobalUdp, sizeof(UDP_GLOBAL) );
	List_InitHead( &lpGlobalUdp->hListTcb );
	InitializeCriticalSection( &lpGlobalUdp->csListTcb );
	lpGlobalUdp->hEvtRxNode = CreateEvent( NULL, FALSE, FALSE, NULL );

	//
	lpGlobalUdp->wPort_Auto = PORTAUTO_MIN;
	//
	if( !UdpSk_Init( lpGlobalUdp ) )
	{
		goto EXIT_UDPINIT;
	}

	//
	if( !Sock_Register( &lpGlobalUdp->LocalTl ) )
	{
		goto EXIT_UDPINIT;
	}

	g_lpGlobalUdp = lpGlobalUdp;
	return TRUE;
EXIT_UDPINIT:
	if( lpGlobalUdp )
	{
		Udp_Free( lpGlobalUdp );
	}
	return FALSE;
}

void	Udp_Deinit( )
{
}

// ********************************************************************
// 声明：
// 参数：
//	IN hGlobalTcp-TCPIP的全局信息
// 返回值：
//	
// 功能描述：释放TCP
// 引用: 
// ********************************************************************
void	Udp_Free( UDP_GLOBAL* lpGlobalUdp )
{
	//句柄安全检查
	if( !HANDLE_CHECK(lpGlobalUdp) )
	{
		return ;
	}

	//
	DeleteCriticalSection( &lpGlobalUdp->csListTcb );

	//
	HANDLE_FREE( lpGlobalUdp );
}


// ********************************************************************
// 声明：
// 参数：
//  OUT ppTcbUdp-保存创建的TCB
//	IN lpGlobalUdp-TCPIP的全局信息
// 返回值：
//	
// 功能描述：创建TCB
// 引用: 
// ********************************************************************
BOOL	Udp_CreateTcb( UDP_TCB** ppTcbUdp, UDP_GLOBAL* lpGlobalUdp )
{
	UDP_TCB*	pTcbUdp;

	//分配 和 基本初始化
	*ppTcbUdp = NULL;
	pTcbUdp = (UDP_TCB*)HANDLE_ALLOC( sizeof(UDP_TCB) );
	if( !pTcbUdp )
	{
		return FALSE;
	}
	HANDLE_INIT( pTcbUdp, sizeof(UDP_TCB) );
	List_InitHead( &pTcbUdp->hListTcb );
	//InitializeCriticalSection( &pTcbUdp->stRxBufRW.csBufRW );
	List_InitHead( &pTcbUdp->hListRxNode );
	InitializeCriticalSection( &pTcbUdp->csListRxNode );
	pTcbUdp->dwLenRcvBuf = LEN_RXBUF;

	pTcbUdp->lpGlobalUdp = lpGlobalUdp;
	pTcbUdp->hEvtPoll = CreateEvent( NULL, FALSE, FALSE, NULL );

	//收---分配BUFFER
	//if( !RWBuf_Alloc( &pTcbUdp->stRxBufRW, LEN_RXBUF, MIN_RXBUF ) )
	//{
	//	goto EXIT_CREATETCB;
	//}

	*ppTcbUdp = pTcbUdp;
//EXIT_CREATETCB:
	if( !(*ppTcbUdp) && pTcbUdp )
	{
		Udp_DeleteTCB( pTcbUdp );
	}

	return (*ppTcbUdp)?TRUE:FALSE;
}


// ********************************************************************
// 声明：
// 参数：
//  IN pTcbUdp-指定的TCB
// 返回值：
//	
// 功能描述：删除TCB
// 引用: 
// ********************************************************************
void	Udp_DeleteTCB( UDP_TCB* pTcbUdp )
{
	UDP_GLOBAL*		lpGlobalUdp;

	//句柄安全检查
	if( !HANDLE_CHECK(pTcbUdp) )
	{
		return ;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( !HANDLE_CHECK(lpGlobalUdp) )
	{
		lpGlobalUdp = NULL;
	}
	//
	pTcbUdp->wFlag |= UDPF_EXIT;
	//从 lpGlobalUdp链表上 断开
	if( lpGlobalUdp )
	{
		EnterCriticalSection( &lpGlobalUdp->csListTcb );
		List_RemoveUnit( &pTcbUdp->hListTcb );
		List_InitHead( &pTcbUdp->hListTcb );
		LeaveCriticalSection( &lpGlobalUdp->csListTcb );
	}
	Sleep( 1 );
	//停止所有TCB的操作
	if( pTcbUdp->hEvtPoll )
	{
		PulseEvent( pTcbUdp->hEvtPoll );
		SetEvent( pTcbUdp->hEvtPoll );
		Sleep( 1 );
	}
	Sleep( 1 );
	//
	Udp_CheckRxNode( pTcbUdp, TRUE );

	//
	if( pTcbUdp->hEvtPoll )
	{
		CloseHandle( pTcbUdp->hEvtPoll );
	}
	//
	//RWBuf_Free( &pTcbUdp->stRxBufRW );

	//释放句柄
	HANDLE_FREE( pTcbUdp );
}


BOOL	Udp_CreateRxNode( OUT UDP_RXNODE** ppUdpRxNode, UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort )
{
	UDP_RXNODE*		pUdpRxNode;

	*ppUdpRxNode = NULL;
	//
	pUdpRxNode = (UDP_RXNODE*)HANDLE_ALLOC( sizeof(UDP_RXNODE) );
	if( !pUdpRxNode )
	{
		return FALSE;
	}
	HANDLE_INIT( pUdpRxNode, sizeof(UDP_RXNODE) );
	List_InitHead( &pUdpRxNode->hListRxNode );
	List_InitHead( &pUdpRxNode->hIPList );
	pUdpRxNode->dwDataLenLimit = pTcbUdp->dwLenRcvBuf;
	/* lilin remove
	//
	if( !RWBuf2_Alloc( &pUdpRxNode->stRx_RWBuf, pTcbUdp->dwLenRcvBuf, MIN_RXBUF ) )
	{
		Udp_DelRxNode( pUdpRxNode );
		return FALSE;
	}
	*/
	//
	//caller has EnterCriticalSection
	//EnterCriticalSection( &pTcbUdp->csListRxNode );
	List_InsertTail( &pTcbUdp->hListRxNode, &pUdpRxNode->hListRxNode );

	pUdpRxNode->dwIP = dwSrcIP;
	pUdpRxNode->wPort = wSrcPort;

	//LeaveCriticalSection( &pTcbUdp->csListRxNode );

	*ppUdpRxNode = pUdpRxNode;
	return TRUE;
}

static UINT Udp_GetRxDataLen( UDP_RXNODE*		pUdpRxNode )
{
	
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	IPDATA_IN* pDataIn;
	
	pUnitHeader = &pUdpRxNode->hIPList;
	
	//pUnit = pUnitHeader->pNext;
	if( (pUnit= pUnitHeader->pNext) !=pUnitHeader  )
	{
		pDataIn = LIST_CONTAINER( pUnit, IPDATA_IN, hListDataIn );
		return pDataIn->wLenData;
	}
	return 0;

	//return pUdpRxNode->dwAvailDataLen;
}

static UINT Udp_GetRxData( UDP_RXNODE*		pUdpRxNode, VOID * pData, DWORD * pdwDataLen )
{
	if( pdwDataLen )
	{
		PLIST_UNIT		pUnitHeader;
		PLIST_UNIT		pUnit;
		IPDATA_IN* pDataIn;

		UINT size = *pdwDataLen;//MIN( *pdwDataLen, pUdpRxNode->dwAvailDataLen );

		pUnitHeader = &pUdpRxNode->hIPList;
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader && size )
		{
			UINT minSize;
			pDataIn = LIST_CONTAINER( pUnit, IPDATA_IN, hListDataIn );
			pUnit = pUnit->pNext;    // advance to next 
			minSize = MIN( pDataIn->wLenData, size );
			memcpy( pData, pDataIn->pData, minSize ); 

			pDataIn->wLenData -= minSize;
			
			if( pDataIn->wLenData == 0 )
			{	//没有更多的数据，移出& 删除
				List_RemoveUnit( &pDataIn->hListDataIn );
				IPDataIn_Free(pDataIn);
			}

			pDataIn->pData = (LPBYTE)pDataIn->pData + minSize;
			size -= minSize;
			break;
		}
		//实际拷贝的数据
		*pdwDataLen -= size;

		pUdpRxNode->dwAvailDataLen -= *pdwDataLen;
		ASSERT( ( (LONG)pUdpRxNode->dwAvailDataLen ) >= 0 );


		return TRUE;
	}
	return FALSE;
}

static VOID Udp_FreeRxData( UDP_RXNODE*		pUdpRxNode )
{
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	IPDATA_IN* pDataIn;
	
	pUnitHeader = &pUdpRxNode->hIPList;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pDataIn = LIST_CONTAINER( pUnit, IPDATA_IN, hListDataIn );
		pUnit = pUnit->pNext;    // advance to next 			
		
		//移出& 删除
		List_RemoveUnit( &pDataIn->hListDataIn );
		IPDataIn_Free(pDataIn);
	}
}

DWORD	Udp_GetRxFirstData( UDP_TCB* pTcbUdp, OUT LPBYTE pData, IN OUT DWORD* pdwLenData, OUT RXTXMSG* pMsgRxTx )
{
	DWORD			dwFirstLen = 0;
	UDP_RXNODE*		pUdpRxNode_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	EnterCriticalSection( &pTcbUdp->csListRxNode );
	//寻找 目标RxNode，并投递数据
	pUnitHeader = &pTcbUdp->hListRxNode;

	//
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		//
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			break;
		}

		//
		pUdpRxNode_Find = LIST_CONTAINER( pUnit, UDP_RXNODE, hListRxNode );
		pUnit = pUnit->pNext;    // advance to next 

		//如果有数据
		//if( (dwFirstLen= pUdpRxNode_Find->stRx_RWBuf.dwCntRW) )
		if( ( dwFirstLen= Udp_GetRxDataLen(pUdpRxNode_Find) ) )
		{
			if( pData )
			{
				//获取数据
				//if( RWBuf2_ReadData( &pUdpRxNode_Find->stRx_RWBuf, pData, pdwLenData ) )
				if( Udp_GetRxData( pUdpRxNode_Find, pData, pdwLenData ) )
				{
					dwFirstLen = *pdwLenData;
				}
				else
				{
					dwFirstLen = 0;
				}

				//如果取完数据则移到最后
				//if( !pUdpRxNode_Find->stRx_RWBuf.dwCntRW )
				if( !Udp_GetRxDataLen(pUdpRxNode_Find) )
				{
					List_RemoveUnit( &pUdpRxNode_Find->hListRxNode );
					List_InsertTail( &pTcbUdp->hListRxNode, &pUdpRxNode_Find->hListRxNode );
				}

				//
				if( pMsgRxTx->lpPeer )
				{
					SOCKADDR_IN*	pAddrIn;

					pAddrIn = (SOCKADDR_IN*)pMsgRxTx->lpPeer;
					memset( pAddrIn, 0, sizeof(SOCKADDR_IN) );
					pAddrIn->sin_family = AF_INET;
					pAddrIn->sin_port = pUdpRxNode_Find->wPort;
					pAddrIn->sin_addr.S_un.S_addr = pUdpRxNode_Find->dwIP;
					pMsgRxTx->dwLenPeer = sizeof(SOCKADDR_IN);
				}
			}
			else
			{
				//如果有数据，则移到最前端
				List_RemoveUnit( &pUdpRxNode_Find->hListRxNode );
				List_InsertHead( &pTcbUdp->hListRxNode, &pUdpRxNode_Find->hListRxNode );
			}

			break;
		}
	}
	//
	LeaveCriticalSection( &pTcbUdp->csListRxNode );
	return dwFirstLen;
}

void	Udp_DelRxNode( UDP_RXNODE* pUdpRxNode )
{
	List_RemoveUnit( &pUdpRxNode->hListRxNode );
	//RWBuf2_Free( &pUdpRxNode->stRx_RWBuf ); //lilin 
	Udp_FreeRxData( pUdpRxNode );

	HANDLE_FREE( pUdpRxNode );
}

void	Udp_CheckRxNode( UDP_TCB* pTcbUdp, BOOL fDelete )
{
	UDP_RXNODE*		pUdpRxNode_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	EnterCriticalSection( &pTcbUdp->csListRxNode );
	//寻找 目标RxNode，并投递数据
	pUnitHeader = &pTcbUdp->hListRxNode;
	//
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		//
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			break;
		}

		//
		pUdpRxNode_Find = LIST_CONTAINER( pUnit, UDP_RXNODE, hListRxNode );
		pUnit = pUnit->pNext;    // advance to next 

		//如果没有数据
		if( fDelete || 
			//( !pUdpRxNode_Find->stRx_RWBuf.dwCntRW && 
			( !Udp_GetRxDataLen(pUdpRxNode_Find) &&
			  ((GetTickCount()-pUdpRxNode_Find->dwTickRcv)>=UDPTM_WAITRXNODE)) )
		{
			//删除他
			Udp_DelRxNode( pUdpRxNode_Find );
			break;
		}
	}
	//
	LeaveCriticalSection( &pTcbUdp->csListRxNode );
}


BOOL	Udp_SetRxNodeLen( UDP_TCB* pTcbUdp, DWORD dwLenNew )
{
	UDP_RXNODE*		pUdpRxNode_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;


	//
	if( !dwLenNew || dwLenNew==pTcbUdp->dwLenRcvBuf || dwLenNew<MIN_RXBUF )
	{
		return TRUE;
	}
	EnterCriticalSection( &pTcbUdp->csListRxNode );

	pTcbUdp->dwLenRcvBuf = dwLenNew;
	//寻找 目标RxNode，并投递数据
	pUnitHeader = &pTcbUdp->hListRxNode;

	//
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		//
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			break;
		}

		//
		pUdpRxNode_Find = LIST_CONTAINER( pUnit, UDP_RXNODE, hListRxNode );
		pUnit = pUnit->pNext;    // advance to next 
		pUdpRxNode_Find->dwDataLenLimit = dwLenNew;



		//如果
//		if( pUdpRxNode_Find->stRx_RWBuf.dwLenRW!=dwLenNew )
//		{
//			RWBuf2_Alloc( &pUdpRxNode_Find->stRx_RWBuf, dwLenNew, MIN_RXBUF );
//		}
	}
	//
	LeaveCriticalSection( &pTcbUdp->csListRxNode );

	return TRUE;

}





