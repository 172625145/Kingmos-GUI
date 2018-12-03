/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：IP 分片操作
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
#include "ipfrag.h"


/***************  全局区 定义， 声明 *****************/

extern	BOOL	IPData_Out( IP_INTF* pNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData );
extern	BOOL	IPData_In( IP_INTF* pNetIntf, IP_HDR* pHdrIP, WORD wLenData );

static	BOOL	IP_FindFgNode( IPFRAG_NODE** ppFgNode, IP_GLOBAL* lpGlobalIP, IP_HDR* pHdrIP );
static	void	IP_NewFgNode( IPFRAG_NODE** ppFgNode, IP_INTF* pNetIntf, IP_HDR* pHdrIP );
static	BOOL	IP_SavFgNode( IPFRAG_NODE* pFgNode, IP_HDR* pHdrIP, LPBYTE* ppBuf, WORD* pwLenData );
static	void	IP_DelFgNode( IPFRAG_NODE* pFgNode );
static	DWORD	WINAPI	IP_ThrdFgNode( LPVOID lpParam );

/******************************************************/

BOOL	IPFrag_Init( )
{
	DWORD			dwThrdID;

	g_lpGlobalIP->hEvtFgNode = CreateEvent( NULL, FALSE, FALSE, NULL );
	//启动
	g_lpGlobalIP->hThrdFgNode = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)IP_ThrdFgNode, (LPVOID)g_lpGlobalIP, 0, &dwThrdID );
	if( !g_lpGlobalIP->hThrdFgNode )
	{
		return FALSE;
	}
	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpParam-IP的全局信息
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：线程---检测“”，并删除它
// 引用: 
// ********************************************************************
DWORD	WINAPI	IP_ThrdFgNode( LPVOID lpParam )
{
	IP_GLOBAL*		lpGlobalIP = (IP_GLOBAL*)lpParam;
	IPFRAG_NODE*	pFgNode_Find;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	while( HANDLE_F_OK(lpGlobalIP, GIPF_EXIT) )
	{
		//等待
		WaitForSingleObject( lpGlobalIP->hEvtFgNode, TIMER_FRAGNODE );
		if( HANDLE_F_FAIL(lpGlobalIP, GIPF_EXIT) )
		{
			break;
		}
		//查询所有的 FRAGNODE，并删除指定的
		pUnitHeader = &lpGlobalIP->hListFgNode;
		EnterCriticalSection( &lpGlobalIP->csListFgNode );
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader )
		{
			pFgNode_Find = LIST_CONTAINER( pUnit, IPFRAG_NODE, hListFgNode );
			pUnit = pUnit->pNext;
			
			if( !HANDLE_CHECK(pFgNode_Find) )
			{
				break;
				//continue;
			}
			if( (GetTickCount()-pFgNode_Find->dwTickLife)>=TIMER_FRAGNODE )
			{
				//请释放该FRAGNODE
				IP_DelFgNode( pFgNode_Find );
			}
		}
		LeaveCriticalSection( &lpGlobalIP->csListFgNode );
	}

	return 0;
}

#define DEBUG_IPFrag_Rx 0
void	IPFrag_Rx( IP_INTF* pNetIntf, IP_HDR* pHdrIP )
{
	IPFRAG_NODE*	pFgNode;
	LPBYTE			pBuf;
	WORD			wLenData;

	pBuf = NULL;
	EnterCriticalSection( &g_lpGlobalIP->csListFgNode );
	//寻找该结点
	if( IP_FindFgNode( &pFgNode, g_lpGlobalIP, pHdrIP ) )
	{
		//直接保存数据，并判断是否可以提交
		DEBUGMSG( DEBUG_IPFrag_Rx, ( "call  IP_SavFgNode.\r\n" ) );
		IP_SavFgNode( pFgNode, pHdrIP, &pBuf, &wLenData );
	}
	else
	{
		//添加该结点，并保存数据
		DEBUGMSG( DEBUG_IPFrag_Rx, ( "call   IP_NewFgNode.\r\n" ) );
		IP_NewFgNode( &pFgNode, pNetIntf, pHdrIP );
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListFgNode );

	//
	if( pBuf )
	{
		//提交到接收队列中
		IPData_In( pNetIntf, (IP_HDR*)pBuf, wLenData );
		free( pBuf );
	}
}


BOOL	IP_FindFgNode( IPFRAG_NODE** ppFgNode, IP_GLOBAL* lpGlobalIP, IP_HDR* pHdrIP )
{
	IPFRAG_NODE*	pFgNode_Find;
	IP_HDR*			pHdrIP_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppFgNode = NULL;
	pUnitHeader = &lpGlobalIP->hListFgNode;
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		pFgNode_Find = LIST_CONTAINER( pUnit, IPFRAG_NODE, hListFgNode );
		pUnit = pUnit->pNext;    // advance to next 

		//
		if( !(HANDLE_CHECK(pFgNode_Find)) )
		{
			break;
			//continue;
		}
		//匹配参数
		pHdrIP_Find = &pFgNode_Find->HdrIP;
		if( (pHdrIP_Find->bProType==pHdrIP->bProType) &&
			(pHdrIP_Find->dwSrcIP==pHdrIP->dwSrcIP) &&
			(pHdrIP_Find->dwDesIP==pHdrIP->dwDesIP) &&
			(pHdrIP_Find->wXID==pHdrIP->wXID) 
		  )
		{
			//如果是
			*ppFgNode = pFgNode_Find;
			break;
		}
	}

	return (*ppFgNode)?TRUE:FALSE;
}


void	IP_NewFgNode( IPFRAG_NODE** ppFgNode, IP_INTF* pNetIntf, IP_HDR* pHdrIP )
{
	IPFRAG_NODE*	pFgNode;
	WORD			wLen;
	WORD			wTmp;

	//保证这是 首个分片数据包---IPF_MF, Offset==0
	if( ((pHdrIP->wFragment & IPF_MF)==0) || (pHdrIP->wFragment & IPF_OFFSET) )
	{
		return ;
	}
	//
	wLen = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wTmp = wLen + sizeof(IPFRAG_NODE) - sizeof(IP_HDR);
	//
	pFgNode = (IPFRAG_NODE*)HANDLE_ALLOC( wTmp );
	if( !pFgNode )
	{
		return ;
	}
	HANDLE_INIT( pFgNode, wTmp );
	List_InitHead( &pFgNode->hListFgNode );
	//
	pFgNode->pNetIntf = pNetIntf;
	pFgNode->dwTickLife = GetTickCount();
	//保存头信息
	memcpy( &pFgNode->HdrIP, pHdrIP, wLen );

	//保存数据
	if( !IP_SavFgNode( pFgNode, pHdrIP, NULL, NULL ) )
	{
		//请释放该FRAGNODE
		IP_DelFgNode( pFgNode );
		return ;
	}
	else
	{
		//
		List_InsertTail( &g_lpGlobalIP->hListFgNode, &pFgNode->hListFgNode );
		return ;
	}
}

static DWORD IP_IsFgNodeOk( IPFRAG_NODE* pFgNode )
{
	IPFRAG_BUF*		pFgBufCur;
	WORD wOffset = 0;
	pFgBufCur = pFgNode->pHeadBuf;
	while( pFgBufCur )
	{
		//继续走
		if( pFgBufCur->wOffset != wOffset )
		{
			return 0;
		}
		wOffset += pFgBufCur->wLenData;
		pFgBufCur = pFgBufCur->pNext;
	}
	return wOffset;
}

static VOID IP_InsertFg( IPFRAG_NODE* pFgNode, IPFRAG_BUF*		pNewFragBuf )
{
	IPFRAG_BUF*		pFgBufCur, *pFgBufPrev;
	
	
	WORD wOffset = pNewFragBuf->wOffset;

	if( !pFgNode->pHeadBuf )
	{
		pFgNode->pHeadBuf = pNewFragBuf;
	}
	//if( !pFgNode->pTailBuf )
	//{
	//	pFgNode->pTailBuf = pFragBuf;
	//}
	//else if( wOffset == pFgNode->wOffset )
	//{
	//	pFgNode->pTailBuf->pNext = pFragBuf;
	//	pFgNode->pTailBuf = pFragBuf;
	//}
	else
	{
		pFgBufPrev = pFgBufCur = pFgNode->pHeadBuf;
		
		while( pFgBufCur )
		{
			if( wOffset < pFgBufCur->wOffset )
			{				
				if( pFgBufPrev == pFgBufCur )
				{	//在头
					pNewFragBuf->pNext = pFgBufCur;
					pFgNode->pHeadBuf = pNewFragBuf;
				}
				else
				{
					pNewFragBuf->pNext = pFgBufCur;
					pFgBufPrev->pNext = pNewFragBuf;
				}
				break;
			}			
			//继续走
			pFgBufPrev = pFgBufCur;
			pFgBufCur = pFgBufCur->pNext;
		}
		if( pFgBufCur == NULL && pFgBufPrev )
		{  //放到末尾
			pFgBufPrev->pNext = pNewFragBuf;
		}
	}
}

BOOL	IP_SavFgNode( IPFRAG_NODE* pFgNode, IP_HDR* pHdrIP, LPBYTE* ppBuf, WORD* pwLenData )
{
	IPFRAG_BUF*		pFragBuf;
	IP_HDR*			pHdrIP_Run;
	LPBYTE			pTmp;
	IPFRAG_BUF*		pFgBufCur;
	WORD			wOffset;
	WORD			wLenHdr;
	WORD			wLenData;

	//比较当前等待的分片编号，是不是要等待的编号
	wOffset = (pHdrIP->wFragment & IPF_OFFSET) <<3;
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wLenData = pHdrIP->wLenTotal - wLenHdr;
/* lilin remove
	if( (wOffset!=pFgNode->wOffset) || !wLenData )
	{
		return FALSE;
	}
	*/
	if( !wLenData )
		return FALSE;

	//判断每个分片的间隔时间，是否到期
	if( pHdrIP->wFragment & IPF_MF )
	{
		if( (GetTickCount()-pFgNode->dwTickLife)>=TIMER_FRAGNODE )
		{
			//请释放该FRAGNODE
			IP_DelFgNode( pFgNode );
			return FALSE;
		}
	}

	
	pFgNode->dwTickLife = GetTickCount(); 

	//分配
	pFragBuf = (IPFRAG_BUF*)malloc( sizeof(IPFRAG_BUF) + wLenData );
	if( !pFragBuf )
	{
		//是否考虑放弃该 FRAGNODE
		return FALSE;
	}
	memset( pFragBuf, 0, sizeof(IPFRAG_BUF) );

	//保存数据
	memcpy( pFragBuf->pData, (LPBYTE)pHdrIP + wLenHdr, wLenData );
	pFragBuf->wLenData = wLenData;
	pFragBuf->wOffset = wOffset; //lilin add -2004-12-14

	IP_InsertFg( pFgNode, pFragBuf ); //lilin add-insert & sort-2004-12-14

	//添加到FRAGNODE
/*
	//pFgNode->wOffset += wLenData;	//lilin remove

	if( !pFgNode->pHeadBuf )
	{
		pFgNode->pHeadBuf = pFragBuf;
	}
	if( !pFgNode->pTailBuf )
	{
		pFgNode->pTailBuf = pFragBuf;
	}
	else
	{
		pFgNode->pTailBuf->pNext = pFragBuf;
		pFgNode->pTailBuf = pFragBuf;
	}
*/
	//是否最后一个片
	if( pFgNode->wEndFlag == FALSE )
		pFgNode->wEndFlag = !(pHdrIP->wFragment & IPF_MF);
	//判断是不是最后1片
	
	//if( !(pHdrIP->wFragment & IPF_MF) )
	if( pFgNode->wEndFlag && 
		(wOffset = (WORD)IP_IsFgNodeOk(pFgNode)) != 0 )  //lilin modify
	{			
		//请提交该数据包
		wLenHdr = (pFgNode->HdrIP.bVerHLen & IPL_MASK)<<2;
		//pTmp = (LPBYTE)malloc( pFgNode->wOffset+wLenHdr );
		pTmp = (LPBYTE)malloc( wOffset+wLenHdr );
		if( pTmp )
		{
			*ppBuf = pTmp;
			//获取头信息
			memcpy( pTmp, &pFgNode->HdrIP, wLenHdr );
			pTmp += wLenHdr;

			//获取数据
			wLenData = 0;
			pFgBufCur = pFgNode->pHeadBuf;
			while( pFgBufCur )
			{
				//保存数据
				memcpy( pTmp, pFgBufCur->pData, pFgBufCur->wLenData );
				pTmp += pFgBufCur->wLenData;
				wLenData += pFgBufCur->wLenData;

				//继续走
				pFgBufCur = pFgBufCur->pNext;
			}
			*pwLenData = wLenData;
			pHdrIP_Run = (IP_HDR*)*ppBuf;
			pHdrIP_Run->wLenTotal = wLenHdr + wLenData;
		}

		//请释放该FRAGNODE
		IP_DelFgNode( pFgNode );
	}
	
	return TRUE;
}

void	IP_DelFgNode( IPFRAG_NODE* pFgNode )
{
	IPFRAG_BUF*		pFgBufCur;
	IPFRAG_BUF*		pFgBufNxt;

	//
	if( !HANDLE_CHECK(pFgNode) )
	{
		return ;
	}

	//断开
	List_RemoveUnit( &pFgNode->hListFgNode );

	//释放所有子项
	pFgBufCur = pFgNode->pHeadBuf;
	while( pFgBufCur )
	{
		//继续走
		pFgBufNxt = pFgBufCur->pNext;

		//释放子项
		free( pFgBufCur );
		
		//继续走
		pFgBufCur = pFgBufNxt;
	}

	//释放该FRAGNODE
	free( pFgNode );
}

DWORD	IPFrag_Tx( IP_INTF* pNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData )
{
	LPBYTE		pOffset;
	WORD		wLenHdr;
	WORD		wLenAll;
	WORD		wMaxData;
	WORD		wTmp;
	
	//准备一些共同的信息
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wMaxData = pNetIntf->wMTU - wLenHdr;
	wMaxData >>= 3;
	wMaxData <<= 3;
	pOffset = pData;
	wLenAll = wLenData;

	pHdrIP->bVerHLen = IPV4_VHDEF;

	//
	//发送非最后分片: ------------------
	//
	while( (wLenAll > wMaxData) && !(pNetIntf->wFlag & IPIFF_EXIT) )
	{
		//提交到发送队列中
		wTmp = ((WORD)(pOffset - pData))>>3;
		pHdrIP->wFragment = (IPF_MF | wTmp);	//表示：有更多分片，且是wTmp偏移
		//提交到发送队列中
		if( !IPData_Out( pNetIntf, pHdrIP, pOffset, wMaxData ) )
		{
			return IPERR_SND_NOBUF;
		}
		wLenAll -= wMaxData;
		pOffset  += wMaxData;
	}

	//
	//发送最后分片: ------------------
	//
	if( wLenAll )
	{
		//提交到发送队列中
		wTmp = ((WORD)(pOffset - pData))>>3;
		pHdrIP->wFragment = wTmp;	//表示：最后分片，且是wTmp偏移
		//提交到发送队列中
		if( !IPData_Out( pNetIntf, pHdrIP, pOffset, wLenAll ) )
		{
			return IPERR_SND_NOBUF;
		}
	}

	return IPERR_SUCCESS;
}

