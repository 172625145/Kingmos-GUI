/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：UDP 数据处理
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
//"\inc_local"
#include "proto_ip_tcp.h"
#include "netif_tcp_ip.h"
//#include "netif_tcp.h"
#include "netif_skinst.h"
//"local"
#include "tcp_udp.h"
#include "udp_def.h"


/***************  全局区 定义， 声明 *****************/

//
static	BOOL	Udp_MatchTcb( OUT UDP_TCB** ppTcbUdp, UDP_GLOBAL* lpGlobalUdp, IP_HDR* pHdrIP, UDP_HDR* pHdrUdp );
//static	void	UdpIn_SaveData( UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort, LPBYTE pUdpData, WORD wLenUdpData );
static BOOL UdpIn_SaveData(
								UDP_TCB* pTcbUdp,
								 DWORD dwSrcIP,
								 WORD wSrcPort,
								 IPDATA_IN * pDataIn, 
								 LPBYTE pUdpData, 
								 WORD wLenUdpData);

static	DWORD	WINAPI	UdpIn_ThrdRxNode( LPVOID lpParam );

/******************************************************/



// ********************************************************************
// 声明：
// 参数：
//	IN pInfoSkt-socket实例信息
//	IN pMsgRxTx-指定发送数据的信息
// 返回值：
//	
// 功能描述：发送数据
// 引用: 
// ********************************************************************
#define DEBUG_Udp_Send 0
SKERR	Udp_Send( UDP_TCB* pTcbUdp, RXTXMSG* pMsgRxTx )
{
	BYTE			bBuffer[sizeof(IP_HDR) + sizeof(UDP_HDR)];
	IP_HDR*			pHdrIP = (IP_HDR*)bBuffer;
	UDP_HDR*		pHdrUdp = (UDP_HDR*)(bBuffer + sizeof(IP_HDR));
	SOCKADDR_IN*	pAddrIn;
	LPBYTE			pOffset;
	DWORD			dwLenAll;

	BYTE			pBufFragSet[sizeof(FRAGSETA)+sizeof(FRAGMENT)];
	FRAGSETA*		pFragSetA = (FRAGSETA*)pBufFragSet;
	DWORD			dwRet;

	WORD			wLenData;
	WORD			wLenTotal;

	//
	RETAILMSG( 1, ( "Call Udp_Send entry,0x%x\r\n", pMsgRxTx ) );

	memset( bBuffer, 0, sizeof(bBuffer) );
	//填充 
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_UDP;
	//填充 目的信息
	if( pMsgRxTx->lpPeer )
	{
		if( pMsgRxTx->dwLenPeer<sizeof(SOCKADDR_IN) )
		{
			//参数出错
			WARNMSG( DEBUG_Udp_Send, ( "Udp_Send error: pMsgRxTx->dwLenPeer<sizeof(SOCKADDR_IN).\r\n" ) );
			return E_FAULT;
		}
		pAddrIn = (SOCKADDR_IN*)pMsgRxTx->lpPeer;
		if( (pAddrIn->sin_family!=AF_INET) || 
			(pAddrIn->sin_addr.S_un.S_addr==INADDR_ANY) ||
			(pAddrIn->sin_port==0) )
		{
			//远程地址 无效
			WARNMSG( DEBUG_Udp_Send, ( "Udp_Send warn: E_ADDRNOTAVAIL.\r\n" ) );
			return E_ADDRNOTAVAIL;
		}
		pHdrIP->dwDesIP = pAddrIn->sin_addr.S_un.S_addr;
		pHdrUdp->wDesPort = pAddrIn->sin_port;
	}
	else
	{
		if( (pTcbUdp->dwDesIP==INADDR_ANY) || (pTcbUdp->wDesPort==0) )
		{
			//需要 远程地址
			WARNMSG( DEBUG_Udp_Send, ( "Udp_Send warn: E_DESTADDRREQ.\r\n" ) );
			return E_DESTADDRREQ;
		}
		pHdrIP->dwDesIP = pTcbUdp->dwDesIP;
		pHdrUdp->wDesPort = pTcbUdp->wDesPort;
	}
	//填充 源信息
	if( !(pTcbUdp->dwOption & TCPIPO_IPINTF) )
	{
		if( pTcbUdp->dwSrcIP==INADDR_ANY )
		{
			if( pTcbUdp->dwSrcIP_Snd==INADDR_ANY )
			{
				//获取适当的本机IP地址
				if( !IP_GetBestAddr( &pTcbUdp->dwSrcIP_Snd, pHdrIP->dwDesIP, TRUE ) )
				{
					WARNMSG( DEBUG_Udp_Send, ( "Udp_Send warn: IP_GetBestAddr return false.\r\n" ) );
					return E_NOTCONN;
				}
			}
			pHdrIP->dwSrcIP = pTcbUdp->dwSrcIP_Snd;
		}
		else
		{
			pHdrIP->dwSrcIP = pTcbUdp->dwSrcIP;
		}
	}
	pHdrUdp->wSrcPort = pTcbUdp->wSrcPort;

	//
	//准备数据包
	pOffset = pMsgRxTx->pData;
	dwLenAll = pMsgRxTx->dwLenData;
	pFragSetA->nCnt = 2;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrUdp;
	pFragSetA->pFrag[0].nLen = sizeof(UDP_HDR);
	do
	{
		//计算 发送长度
		if( dwLenAll>IPF_MAXFRAG )
		{
			wLenData = IPF_MAXFRAG;
		}
		else
		{
			wLenData = (WORD)dwLenAll;
		}
		pFragSetA->pFrag[1].pData = pOffset;
		pFragSetA->pFrag[1].nLen = wLenData;

		//计算 UDP HEADER信息
		wLenTotal = wLenData+sizeof(UDP_HDR);
		pHdrUdp->wLenTotal = htons_m(wLenTotal);
		pHdrUdp->wTotalCRC = Tcp_CRC2( pHdrIP->dwSrcIP, pHdrIP->dwDesIP, pHdrIP->bProType, pFragSetA );

		//Udp_DbgOut( 0, pHdrUdp, wLenData, FALSE );
		//发送数据包
		pHdrIP->wLenTotal = sizeof(IP_HDR)+wLenTotal;
		if( pTcbUdp->dwOption & TCPIPO_IPINTF )
		{
			DEBUGMSG( DEBUG_Udp_Send, ( "Udp_Send : use TCPIPO_IPINTF.\r\n" ) );
			dwRet = IP_NetOut( pTcbUdp->hNetIntf, pHdrIP, pFragSetA );
		}
		else
		{
			DEBUGMSG( DEBUG_Udp_Send, ( "Udp_Send : send data.\r\n" ) );
			dwRet = IP_NetOut( NULL, pHdrIP, pFragSetA );
		}
		//分析IP_NetOut的发送结果
		if( dwRet )
		{
			switch( dwRet )
			{
			case IPERR_NO_INTF:
				return E_NOTCONN;
				break;
			case IPERR_SND_NOBUF:
				return E_NOBUFS;
				break;
			default :
				break;
			}

			return E_SND_UNKNOWN;
		}
		//更新发送信息
		dwLenAll -= wLenData;
		pOffset += wLenData;

	}while( dwLenAll );

	return E_SUCCESS;
}


// ********************************************************************
// 声明：
// 参数：
//	IN lpGlobalUdp-TCPIP的全局信息
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：启动线程检测“长时间没有被连接成功的被动连接TCB”
// 引用: 
// ********************************************************************
BOOL	UdpIn_StartRxNode( UDP_GLOBAL* lpGlobalUdp )
{
	DWORD	dwThrdID;

	if( !lpGlobalUdp->hThrdRxNode )
	{
		lpGlobalUdp->hThrdRxNode = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)UdpIn_ThrdRxNode, (LPVOID)lpGlobalUdp, 0, &dwThrdID );
		if( !lpGlobalUdp->hThrdRxNode )
		{
			return FALSE;
		}
	}
	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpParam-UDPIP的全局信息
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：线程---检测“长时间没有接收数据的RxNode，如果他没有数据的话”，并删除它
// 引用: 
// ********************************************************************
DWORD	WINAPI	UdpIn_ThrdRxNode( LPVOID lpParam )
{
	UDP_GLOBAL*		lpGlobalUdp = (UDP_GLOBAL*)lpParam;
	UDP_TCB*		pTcbUdp;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	while( HANDLE_F_OK(lpGlobalUdp, TCPIPF_EXIT) )
	{
		//等待
		WaitForSingleObject( lpGlobalUdp->hEvtRxNode, UDPTM_WAITRXNODE );
		if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
		{
			break;
		}
		//查询所有的 RxNode，并删除指定的
		pUnitHeader = &lpGlobalUdp->hListTcb;
		EnterCriticalSection( &lpGlobalUdp->csListTcb );
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader )
		{
			//根据结构成员地址，得到结构地址，并继续
			pTcbUdp = LIST_CONTAINER( pUnit, UDP_TCB, hListTcb );
			pUnit = pUnit->pNext;    // advance to next 

			//句柄安全寻找
			if( !HANDLE_CHECK(pTcbUdp) )
			{
				break;
			}
			if( pTcbUdp->wFlag & UDPF_EXIT )
			{
				continue;
			}
			//
			Udp_CheckRxNode( pTcbUdp, FALSE );
		}
		LeaveCriticalSection( &lpGlobalUdp->csListTcb );
	}

	return 0;
}


//DWORD	Udp_TrIn( HANDLE hNetIntf, IP_HDR* pHdrIP, LPBYTE pBufData, WORD wLenData )
#define DEBUG_Udp_TrIn 0
DWORD	Udp_TrIn( HANDLE hNetIntf, IPDATA_IN* pDataIn, LPBYTE pBufData, WORD wLenData )
{
	UDP_TCB*		pTcbUdp;
	UDP_HDR*		pHdrUdp;
	BOOL			fSavData;

	LPBYTE			pUdpData;
	WORD			wLenUdpData;
	WORD			wLenTotal;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	IP_HDR* pHdrIP = &pDataIn->HdrIP;
	DWORD dwRetv = 0;

	DEBUGMSG( DEBUG_Udp_TrIn, ( "Udp_TrIn entry.\r\n", wLenData ) );
	//句柄安全检查
	if( HANDLE_F_FAIL(g_lpGlobalUdp, TCPIPF_EXIT) )
	{
		goto _return;
	}
	//检查---BUFFER长度
	if( wLenData<sizeof(UDP_HDR) )
	{
		goto _return;
	}
	//检查---校验码
	if( Tcp_CRC(pHdrIP->dwSrcIP, pHdrIP->dwDesIP, pHdrIP->bProType, pBufData, wLenData) )
	{
		goto _return;
	}
	//检查---UDP_HDR长度成员
	pHdrUdp = (UDP_HDR*)(pBufData);
#ifndef	BIG_END
	wLenTotal = ntohs(pHdrUdp->wLenTotal);
#else
	wLenTotal = pHdrUdp->wLenTotal;
#endif
	if( wLenTotal>wLenData )
	{
		goto _return;
	}
	//检查---成功
	pUdpData = pBufData + sizeof(UDP_HDR);
	wLenUdpData = wLenTotal-sizeof(UDP_HDR);

	//寻找 目标TCB，并投递数据
	pUnitHeader = &g_lpGlobalUdp->hListTcb;
	EnterCriticalSection( &g_lpGlobalUdp->csListTcb );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalUdp->wFlag & TCPIPF_EXIT) )
	{
		//根据结构成员地址，得到结构地址，并继续
		pTcbUdp = LIST_CONTAINER( pUnit, UDP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pTcbUdp) )
		{
			break;
		}
		if( pTcbUdp->wFlag & UDPF_EXIT )
		{
			continue;
		}
		//本地端口比较
		if( pTcbUdp->wSrcPort==pHdrUdp->wDesPort )
		{
			if( pTcbUdp->dwOption & TCPIPO_IPINTF )
			{
				//是否直接绑定到该IP接口
				if( pTcbUdp->hNetIntf==hNetIntf )
				{
					//投递，保存数据
					fSavData = TRUE;
				}
				else
				{
					fSavData = FALSE;
				}
			}
			else
			{
				//地址匹配
				if( (pTcbUdp->dwSrcIP==pHdrIP->dwDesIP) ||			//唯一接收者
					(pTcbUdp->dwSrcIP==INADDR_ANY) ||				//唯一接收者
					( INADDR_TEST_BC(pHdrIP->dwDesIP) &&			//如果是广播数据
					  ( (pTcbUdp->dwOption & TCPIPO_BROADCAST) ||		//TCB可以收发广播
					    (pTcbUdp->dwDesIP==pHdrIP->dwDesIP)				//TCB已经connect到1个广播地址
					  )
					)
				  )
				{
					//投递，保存数据
					fSavData = TRUE;
				}
				else
				{
					fSavData = FALSE;
				}
			}
			//投递，保存数据
			if( fSavData )
			{
				RETAILMSG( 1, ( "Call UdpIn_SaveData=%d.\r\n", wLenUdpData ) );
				//if( UdpIn_SaveData( pTcbUdp, pHdrIP->dwSrcIP, pHdrUdp->wSrcPort, pUdpData, wLenUdpData ) )
				if( UdpIn_SaveData( pTcbUdp, pHdrIP->dwSrcIP, pHdrUdp->wSrcPort, pDataIn, pUdpData, wLenUdpData ) )
				{
					LeaveCriticalSection( &g_lpGlobalUdp->csListTcb );
					return dwRetv;
				}
			}
		}
	}
	//
	LeaveCriticalSection( &g_lpGlobalUdp->csListTcb );

	//
	if( !fSavData )
	{
		//产生ICMP错误报文---目的端口不可到达
		Icmp_TrOut_Unreach( ICMP_CODE_DESUR_PORT, pHdrIP->dwSrcIP, pHdrIP->dwDesIP, (LPBYTE)pHdrIP, sizeof(IP_HDR)+sizeof(UDP_HDR) );
	}

	//Udp_DbgIn( 0, pHdrUdp, wLenUdpData, FALSE );
_return:
	IPDataIn_Free(pDataIn);

	return dwRetv;
}


void	UdpIn_IcmpNotify( BYTE bType, BYTE bCode, IP_HDR* pHdrIP_icmp, WORD wLenData )
{
	UDP_HDR*		pHdrUdp;
	UDP_TCB*		pTcbUdp;
	WORD			wLenHdr;

	//句柄安全检查
	if( HANDLE_F_FAIL(g_lpGlobalUdp, TCPIPF_EXIT) )
	{
		return ;
	}
	//保证 wLenData>=(sizeof(IP_HDR)+sizeof(UDP_HDR))
	wLenHdr = ((pHdrIP_icmp->bVerHLen & IPL_MASK)<<2);
	if( wLenData<(wLenHdr + sizeof(UDP_HDR)) )
	{
		return ;
	}
	//寻找该端口
	pHdrUdp = (UDP_HDR*)((LPBYTE)pHdrIP_icmp + wLenHdr);
	if( !Udp_MatchTcb( &pTcbUdp, g_lpGlobalUdp, pHdrIP_icmp, pHdrUdp ) )
	{
		return ;
	}

	//分析ICMP的信息
	switch( bType )
	{
	case ICMP_TYPE_DES_UNREACH:
		switch( bCode )
		{
		case ICMP_CODE_DESUR_NET	:
			pTcbUdp->nErr = E_NETUNREACH;
			break;
		case ICMP_CODE_DESUR_HOST	:
		case ICMP_CODE_DESUR_PROTOCOL:
			pTcbUdp->nErr = E_HOSTUNREACH;
			break;
		case ICMP_CODE_DESUR_PORT	:
			pTcbUdp->nErr = E_CONNRESET;
			break;
		case ICMP_CODE_DESUR_FRAGMENT:
		case ICMP_CODE_DESUR_ROUTE	:
		case ICMP_CODE_DESUR_UNNET	:
		case ICMP_CODE_DESUR_UNHOST	:
		default :
			break;
		}
		PulseEvent( pTcbUdp->hEvtPoll );

	case ICMP_TYPE_SRC_QUENCH:
		break;

	default :
		break;
	}

}




/////////////////////////////////////////////////////////////////////
//
//内部辅助函数
//
/////////////////////////////////////////////////////////////////////

#define DEBUG_UdpIn_SaveData 0
//void BOOL UdpIn_SaveData( UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort, LPBYTE pUdpData, WORD wLenUdpData )
static BOOL UdpIn_SaveData( UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort, IPDATA_IN* pDataIn, LPBYTE pUdpData, WORD wLenUdpData )

{
//	DWORD			dwLenData;
	UDP_RXNODE*		pUdpRxNode = NULL;
	UDP_RXNODE*		pUdpRxNode_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	BOOL bRetv = FALSE;

	EnterCriticalSection( &pTcbUdp->csListRxNode );


	//寻找 目标RxNode，并投递数据
	pUnitHeader = &pTcbUdp->hListRxNode;

	//
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && HANDLE_F_OK(pTcbUdp, UDPF_EXIT) )
	{
		//根据结构成员地址，得到结构地址，并继续
		pUdpRxNode_Find = LIST_CONTAINER( pUnit, UDP_RXNODE, hListRxNode );
		pUnit = pUnit->pNext;    // advance to next 

		//
		if( (pUdpRxNode_Find->dwIP==dwSrcIP) && (pUdpRxNode_Find->wPort==wSrcPort) )
		{
			pUdpRxNode = pUdpRxNode_Find;
			break;
		}
	}
	//
	if( !pUdpRxNode )
	{
		if( !Udp_CreateRxNode(&pUdpRxNode, pTcbUdp, dwSrcIP, wSrcPort) )
		{
			//LeaveCriticalSection( &pTcbUdp->csListRxNode );
			//return FALSE;
			goto _return;
		}
	}
	//dwLenData = wLenUdpData;
	//RWBuf2_WriteData( &pUdpRxNode->stRx_RWBuf, pUdpData, &dwLenData, 0 );
	pUdpRxNode->dwTickRcv = GetTickCount();

	//if( pUdpRxNode->pDataIn == NULL )
	if( pUdpRxNode->dwAvailDataLen + wLenUdpData <= pUdpRxNode->dwDataLenLimit )
	{
		//pUdpRxNode->pDataIn = pDataIn;
		//更新数据
		pDataIn->pData = pUdpData;
		pDataIn->wLenData = wLenUdpData;
		
		// FIFO
		List_InsertTail( &pUdpRxNode->hIPList, &pDataIn->hListDataIn );
		pUdpRxNode->dwAvailDataLen += wLenUdpData;		

		bRetv = TRUE;
	}
	else
	{
		WARNMSG( DEBUG_UdpIn_SaveData, ( "pUdpRxNode->pDataIn has data!.\r\n" ) );
	}
	
	//
_return:

	LeaveCriticalSection( &pTcbUdp->csListRxNode );
	return bRetv;

}


// ********************************************************************
// 声明：
// 参数：
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrUdp-接收的数据包TCP Header
//	IN lpGlobalUdp-TCPIP的全局信息
//	IN/OUT ppTcbUdp-要匹配的TCB
// 返回值：
//	
// 功能描述：匹配pHdrIP和pHdrUdp指定的TCB
// 引用: 
// ********************************************************************
BOOL	Udp_MatchTcb( OUT UDP_TCB** ppTcbUdp, UDP_GLOBAL* lpGlobalUdp, IP_HDR* pHdrIP, UDP_HDR* pHdrUdp )
{
	UDP_TCB*		pTcbUdp_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppTcbUdp = NULL;
	pUnitHeader = &lpGlobalUdp->hListTcb;
	EnterCriticalSection( &lpGlobalUdp->csListTcb );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(lpGlobalUdp->wFlag & TCPIPF_EXIT) )
	{
		//根据结构成员地址，得到结构地址，并继续
		pTcbUdp_Find = LIST_CONTAINER( pUnit, UDP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pTcbUdp_Find) )
		{
			break;
		}
		if( pTcbUdp_Find->wFlag & UDPF_EXIT )
		{
			continue;
		}
		//本地端口比较
		if( pTcbUdp_Find->wSrcPort==pHdrUdp->wDesPort )
		{
			//地址比较---
			if( (pTcbUdp_Find->dwSrcIP==pHdrIP->dwDesIP) && 
				(pTcbUdp_Find->dwDesIP==pHdrIP->dwSrcIP) && 
				(pTcbUdp_Find->wDesPort==pHdrUdp->wSrcPort)
				)
			{
				*ppTcbUdp = pTcbUdp_Find;
				break;
			}
		}
	}
	//
	LeaveCriticalSection( &lpGlobalUdp->csListTcb );

	return (*ppTcbUdp)?TRUE:FALSE;
}




