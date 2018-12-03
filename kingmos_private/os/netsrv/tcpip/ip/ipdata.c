/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：IP数据包的上传下送处理
版本号：  1.0.0
开发时期：2004-04-21
作者：    肖远钢
修改记录：

注释（肖远钢 2004-04-21 写）：
1。用队列和线程的方法做，主要为下面功能提供支持：
   1> 在接收处理中，可以转发数据包
   2> 在发送处理中，可以环回提交
2。目前接收IP数据包的机制：
   1> 用户在提交到接收线程前，该包经过
      校验、本机检查和转发检查、转换IP头是HOST顺序、组装、形成1个IP数据包BUFFER
   2> 
3。目前发送IP数据包的机制：
   1> 用户在提交到发送线程前，请作好相关工作：
      选接口、选路、填充选路地址、填充IP头信息、分片、形成1个IP数据包BUFFER
   2> 发送线程的任务，就是获取该包，直接提交给数据链路层发送
4。该做法的缺点：
   1> 如果在数据链路层发送失败，传输层得不到该错误信息
   2> 比直接提交传输层处理的机制，会浪费1些时间；
      一般情况，数据速度主要由网络影响，而不是处理速度
5。该做法的优点：
   1> 接收的数据，对IP_HDR来说是对齐的
   2> 发送数据因为是队列形式，所以，发送速度会比较快

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
//"local"
#include "ip_def.h"


/***************  全局区 定义， 声明 *****************/

void	IP_Deliver( IP_INTF* pNetIntf, IPDATA_IN* pIPDataIn, WORD wLenData );

DWORD	WINAPI	IP_ThrdDataIn( LPVOID lpParam );
DWORD	WINAPI	IP_ThrdDataOut( LPVOID lpParam );

static	long	g_nCntRx_TCP = 0;
/******************************************************/
IPDATA_IN* IPDataIn_Alloc( UINT uDataSize );



BOOL	IPData_Start( IP_INTF* pNetIntf )
{
	DWORD	dwThrdID;

	//启动接收的线程
	pNetIntf->hThrdDataIn = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)IP_ThrdDataIn, (LPVOID)pNetIntf, 0, &dwThrdID );
	if( !pNetIntf->hThrdDataIn )
	{
		return FALSE;
	}
	//启动发送的线程
	pNetIntf->hThrdDataOut = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)IP_ThrdDataOut, (LPVOID)pNetIntf, 0, &dwThrdID );
	if( !pNetIntf->hThrdDataOut )
	{
		return FALSE;
	}
	return TRUE;
}


DWORD	WINAPI	IP_ThrdDataIn( LPVOID lpParam )
{
	IP_INTF*		pNetIntf = (IP_INTF*)lpParam;
	IPDATA_IN*		pDataIn;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	pUnitHeader = &pNetIntf->hListDataIn;
	while( HANDLE_F_OK(pNetIntf, IPIFF_EXIT) )
	{
		//等待数据包
		WaitForSingleObject( pNetIntf->hEvtDataIn, INFINITE );
		if( HANDLE_F_FAIL(pNetIntf, IPIFF_EXIT) )
		{
			break;
		}
		//检取数据包
		while( !(pNetIntf->wFlag & IPIFF_EXIT) )
		{
			//提取数据包
			EnterCriticalSection( &pNetIntf->csListDataIn );
			pUnit = pUnitHeader->pNext;
			if( pUnit!=pUnitHeader )
			{
				//获取数据包
				pDataIn = LIST_CONTAINER( pUnit, IPDATA_IN, hListDataIn );
				List_RemoveUnit( &pDataIn->hListDataIn );
			}
			else
			{
				LeaveCriticalSection( &pNetIntf->csListDataIn );
				break;
			}
			LeaveCriticalSection( &pNetIntf->csListDataIn );
			//处理数据包
			if( pUnit!=pUnitHeader )
			{
				//上传数据包
				//pDataIn->bAutoFree = 1;
				IP_Deliver( pNetIntf, pDataIn, pDataIn->wLenData );
				//ASSERT( pDataIn->bAutoFree == 1 || pDataIn->bAutoFree == 0 );
				//释放数据包
				//free( pDataIn );
				//if(  pDataIn->bAutoFree == 1 )
				//	IPDataIn_Free( pDataIn );	//auto free
				//else
				//{
				//	pDataIn = pDataIn;  // only for test
				//}
			}
		}
	}

	return 0;
}

#define DEBUG_IP_Deliver 0
void	IP_Deliver( IP_INTF* pNetIntf, IPDATA_IN* pDataIn, WORD wLenData )
{
	LPBYTE		pData;
	IP_HDR * pHdrIP ;

	//分发协议包
	pHdrIP = &pDataIn->HdrIP;
	pDataIn->pData = pData = (LPBYTE)pHdrIP + ((pHdrIP->bVerHLen & IPL_MASK)<<2);
	
	switch( pHdrIP->bProType )
	{
	case IP_PROTYPE_TCP:
		//RETAILMSG(1,(TEXT(" \r\nIP_Deliver_TCP: g_nCntRx_TCP=[%d], wLenData=[%d]\r\n"), ++g_nCntRx_TCP, wLenData));
		Tcp_TrIn( (HANDLE)pNetIntf, pDataIn, pData, wLenData );
		break;
	case IP_PROTYPE_UDP:
		Udp_TrIn( (HANDLE)pNetIntf, pDataIn, pData, wLenData );
		break;
	case IP_PROTYPE_ICMP:
		Icmp_TrIn( (HANDLE)pNetIntf, pDataIn, pData, wLenData );
		break;
	default :
		WARNMSG( DEBUG_IP_Deliver,(TEXT("IP_Deliver: get a unused packet.\r\n") ));
		break;
	}

}
/*
LPVOID lpCheckPtr = NULL;

static void CheckPtr( LPVOID p, char * file, int line )
{
	if( p )
	{
		LPDWORD lp = (LPDWORD)( (DWORD)p - 4 );
		
		if( *lp != 0x1234abcd )
		{
			RETAILMSG( 1, ( "error ptr chesum(0x%x),file=%s,line=%d.\r\n", *lp, file, line ) );
		}			
		else if( HeapValidate( GetProcessHeap(), 0, lp ) == FALSE )
		{
			RETAILMSG( 1, ( "error ptr:file=%s,line=%d.\r\n", file, line ) );
		}
	}
}
*/
//void CheckOutPtr( char * file, int line )
//{
	//CheckPtr( lpCheckPtr, file, line );
//}

#define	MAX_FRAGCNT			20
DWORD	WINAPI	IP_ThrdDataOut( LPVOID lpParam )
{
	IP_INTF*		pNetIntf = (IP_INTF*)lpParam;
	IPDATA_OUT*		pDataOut;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	PARAM_DLOUT		ParamOut;
	BYTE			pBufFragSet[sizeof(FRAGSETD) + MAX_FRAGCNT*sizeof(FRAGMENT)];
	FRAGSETD*		pFragSetD = (FRAGSETD*)pBufFragSet;
	DWORD			dwRet;

	//
	pUnitHeader = &pNetIntf->hListDataOut;
	pFragSetD->nCntAll = MAX_FRAGCNT;
	//
	while( HANDLE_F_OK(pNetIntf, IPIFF_EXIT) )
	{
		//等待数据包
		WaitForSingleObject( pNetIntf->hEvtDataOut, INFINITE );
		if( HANDLE_F_FAIL(pNetIntf, IPIFF_EXIT) )
		{
			break;
		}
		//检取数据包
		while( !(pNetIntf->wFlag & IPIFF_EXIT) )
		{
			//获取数据包
			EnterCriticalSection( &pNetIntf->csListDataOut );
			pUnit = pUnitHeader->pNext;
/*
			if( pUnit!=pUnitHeader )
			{
				pDataOut = LIST_CONTAINER( pUnit, IPDATA_OUT, hListDataOut );
				List_RemoveUnit( &pDataOut->hListDataOut );
			}
			else
			{
				LeaveCriticalSection( &pNetIntf->csListDataOut );
				break;
			}
*/
			if( pUnit==pUnitHeader )
			{ 
				LeaveCriticalSection( &pNetIntf->csListDataOut );
				break;
			}			

			pDataOut = LIST_CONTAINER( pUnit, IPDATA_OUT, hListDataOut );

			//RETAILMSG(1,(TEXT("out001,pDataOut=%x,next=%x,prev=%x.\r\n"), pDataOut, pDataOut->hListDataOut.pNext, pDataOut->hListDataOut.pPrev ));
			//RETAILMSG(1,(TEXT("out001-1,pUnitHeader=%x,next=%x,prev=%x.\r\n"), pUnitHeader, pUnitHeader->pNext, pUnitHeader->pPrev ));

			//CheckPtr( pDataOut, __FILE__, __LINE__ );
			
			List_RemoveUnit( &pDataOut->hListDataOut );



			//RETAILMSG(1,(TEXT("out0022.\r\n") ));

			LeaveCriticalSection( &pNetIntf->csListDataOut );
			//处理数据包
			//RETAILMSG(1,(TEXT("out003.\r\n") ));
//			if( pUnit!=pUnitHeader )
			{
				//下送数据包
				ParamOut.dwProtoID = PROID_IP;
				ParamOut.dwOption = DLOUT_OPT_ADDR_IP;
				ParamOut.pDesAddr = (LPBYTE)&pDataOut->dwDesIP;
				ParamOut.dwLenAddr = sizeof(DWORD);
				ParamOut.pFragSetD = pFragSetD;



				pFragSetD->nCntUsed = 1;
				pFragSetD->dwBytes = pDataOut->wLenTotal;
				pFragSetD->pFrag[0].pData = pDataOut->pBufIP;
				pFragSetD->pFrag[0].nLen = pDataOut->wLenTotal;

				//CheckPtr( pDataOut, __FILE__, __LINE__ );
												
				//RETAILMSG(1,(TEXT("out004.\r\n") ));
				
				//lpCheckPtr = pDataOut;
				dwRet = (*pNetIntf->pFnDlOut)( pNetIntf->hIfDl, &ParamOut );
				//lpCheckPtr = NULL;

				//RETAILMSG(1,(TEXT(" \r\n IP_ThrdDataOut: wLenTotal=[%d], dwRet=[%d]\r\n"), pFragSetD->dwBytes, dwRet));
				//HeapValidate( GetProcessHeap(), 0, pDataOut );
				//CheckOutPtr( __FILE__, __LINE__ );
				//CheckPtr( pDataOut, __FILE__, __LINE__ );

				//RETAILMSG(1,(TEXT("out005.\r\n") ));
				//释放数据包
				free( (LPBYTE)pDataOut );
				//RETAILMSG(1,(TEXT("out006.\r\n") ));				
			}
			//RETAILMSG(1,(TEXT("out006.\r\n") ));
		}
	}

	return 0;
}

IPDATA_IN* IPDataIn_Alloc( UINT uDataSize )
{
	return (IPDATA_IN*)malloc( sizeof(IPDATA_IN)+uDataSize );
}

VOID IPDataIn_Free( IPDATA_IN* pDataIn )
{
	free( pDataIn );
}

BOOL	IPData_In( IP_INTF* pNetIntf, IP_HDR* pHdrIP, WORD wLenData )
{
	IPDATA_IN*		pDataIn;

	//分配接收项
	pDataIn = IPDataIn_Alloc(pHdrIP->wLenTotal);//(IPDATA_IN*)malloc( sizeof(IPDATA_IN)+pHdrIP->wLenTotal );
	if( !pDataIn )
	{
		return FALSE;
	}
	memset( pDataIn, 0, sizeof(IPDATA_IN) );
	List_InitHead( &pDataIn->hListDataIn );
	pDataIn->wLenData = wLenData;
	memcpy( pDataIn->pBufIP, pHdrIP, pHdrIP->wLenTotal );
	//添加
	EnterCriticalSection( &pNetIntf->csListDataIn );
	List_InsertTail( &pNetIntf->hListDataIn, &pDataIn->hListDataIn );
	LeaveCriticalSection( &pNetIntf->csListDataIn );
	SetEvent( pNetIntf->hEvtDataIn );
	//PulseEvent( pNetIntf->hEvtDataIn );

	return TRUE;
}

#define DEBUG_IPData_Out 0
BOOL	IPData_Out( IP_INTF* pNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData )
{
	IPDATA_OUT*		pDataOut;
	WORD			wLenHdr;
	WORD			wLenTotal;

	DEBUGMSG( DEBUG_IPData_Out, ( "IPData_Out: wLenData=%d.\r\n", wLenData ) );
	//
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wLenTotal = wLenHdr+wLenData;
	//分配发送项
	pDataOut = (IPDATA_OUT*)malloc( sizeof(IPDATA_OUT)+wLenTotal);
	if( !pDataOut )
	{
		return FALSE;
	}
	//*( (LPDWORD)pDataOut ) = 0x1234abcd;
	//pDataOut = (IPDATA_OUT*)((LPBYTE)pDataOut + 4);

	//RETAILMSG( 1, ( "pDataOut:0x%x.\r\n", pDataOut ) );
	memset( pDataOut, 0, sizeof(IPDATA_OUT) );
	List_InitHead( &pDataOut->hListDataOut );
	pDataOut->dwDesIP = pHdrIP->dwDesIP;
	pDataOut->wLenTotal = wLenTotal;

	//转化 IP信息--- wLenTotal / wFragment / wHdrCRC
	pHdrIP->wHdrCRC = 0;
#ifndef BIG_ENDIAN
	pHdrIP->wLenTotal  = htons_m(wLenTotal);
	pHdrIP->wFragment  = htons(pHdrIP->wFragment);
#else
	pHdrIP->wLenTotal  = wLenTotal;
	//pHdrIP->wFragment  = pHdrIP->wFragment;
#endif
	pHdrIP->wHdrCRC = IP_CRC( (LPBYTE)pHdrIP, wLenHdr );

	//拷贝数据
	memcpy( pDataOut->pBufIP, pHdrIP, wLenHdr );
	memcpy( pDataOut->pBufIP+wLenHdr, pData, wLenData );

	//CheckPtr( pDataOut, __FILE__, __LINE__ );

	//添加
	EnterCriticalSection( &pNetIntf->csListDataOut );
	List_InsertTail( &pNetIntf->hListDataOut, &pDataOut->hListDataOut );
	LeaveCriticalSection( &pNetIntf->csListDataOut );

	//CheckPtr( pDataOut, __FILE__, __LINE__ );
	//发送
	SetEvent( pNetIntf->hEvtDataOut );
	//PulseEvent( pNetIntf->hEvtDataOut );

	return TRUE;
}







