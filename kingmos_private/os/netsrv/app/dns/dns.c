/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：DNS
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

		DNS_GLOBAL*		g_lpGlobalDNS = NULL;

//
static	DWORD	DnsQry_GetFromNet( DNS_ITEM* pItemDNS );
static	BOOL	DnsQry_SendReq( DNS_ITEM* pItemDNS, BOOL* pfExit );
static	BOOL	DnsQry_Handle( DNS_ITEM* pItemDNS );

static	BOOL	DnsItem_New( DNS_ITEM** ppItemDNS, WORD wLenDns );
static	void	DnsItem_Del( DNS_ITEM* pItemDNS );
static	void	DnsItem_Set( DNS_ITEM* pItemDNS, LPCSTR pszNameDns );
static	BOOL	DnsItem_SaveAddr( DNS_ITEM* pItemDNS, LPBYTE pAddr );

//
static	WORD	DNS_CheckName( LPBYTE pName );
static	LPBYTE	DNS_ToSavName( OUT LPBYTE pNameByDNS, LPBYTE pName );

//static	BOOL	DNS_GetDomain( DNS_ITEM* pItemDNS, LPBYTE pDomain, LPBYTE pDataBegin, LPBYTE pDataEnd );
static	LPBYTE	DNS_ToNormalName( LPBYTE pNameByDNS, OUT LPBYTE pName );

#define	DNSWAIT_TIME		(8)

/******************************************************/


BOOL	DNS_Init( )
{
	DNS_GLOBAL*		lpGlobalDNS;

	//分配1个TCP的功能 和基本初始化
	lpGlobalDNS = (DNS_GLOBAL*)HANDLE_ALLOC( sizeof(DNS_GLOBAL) );
	if( !lpGlobalDNS )
	{
		return FALSE;
	}
	HANDLE_INIT( lpGlobalDNS, sizeof(DNS_GLOBAL) );
	List_InitHead( &lpGlobalDNS->hListHAddr );
	InitializeCriticalSection( &lpGlobalDNS->csListHAddr );

	g_lpGlobalDNS = lpGlobalDNS;
	return TRUE;
}

void	DNS_Deinit( )
{
}

DWORD	DnsCall_Query( LPCSTR pszNameDns, OUT HOSTENT* pHostInfo, int len, OUT WORD* pwCntAddr, DWORD dwOptDns )
{
	DWORD			dwErr;
	DNS_ITEM*		pItemDNS;
	WORD			wLenName;
	WORD			wTmp;

	//参数检查
	RETAILMSG(1,(TEXT("  DnsCall_Query: Enter\r\n")));
	//if( !(wLenName=DNS_CheckName( (LPBYTE)pszNameDns )) )
	if( !pwCntAddr )
	{
		RETAILMSG(1,(TEXT("  DnsCall_Query: leave failed--1\r\n")));
		return DNSERR_PARAM;
	}
	*pwCntAddr = 0;
	if( !pszNameDns || !(wLenName=strlen(pszNameDns)) || !pHostInfo )
	{
		RETAILMSG(1,(TEXT("  DnsCall_Query: leave failed--2\r\n")));
		return DNSERR_PARAM;
	}
	//
	//从域名缓冲里 查找
	//
	if( !(dwOptDns & DNSOPT_NOT_FROM_TBL) )
	{
		DNS_HOSTADDR*	pHAddrDns;

		if( DnsTbl_FindHostByName(&pHAddrDns, pszNameDns) && pHAddrDns->wCntAddrReal )
		{
			wTmp = pHAddrDns->wCntAddrReal;
			Skt_SavHostentAddr( pHostInfo, (DWORD)len, pHAddrDns->pAddrList, pHAddrDns->wAddrLen, &wTmp );
			*pwCntAddr = wTmp;
			RETAILMSG(1,(TEXT("  DnsCall_Query: leave ok from table\r\n")));
			return DNSERR_SUCCESS;
		}
	}

	//
	//发送DNS数据包 查询
	//
	if( dwOptDns & DNSOPT_NOT_FROM_NET )
	{
		RETAILMSG(1,(TEXT("  DnsCall_Query: leave failed--3\r\n")));
		return DNSERR_NOT_FOUND;
	}
	//分配 1个查询项
	wTmp = sizeof(DNS_HDR) +wLenName +2 +sizeof(WORD) +sizeof(WORD);
	if( !DnsItem_New( &pItemDNS, wTmp ) )
	{
		RETAILMSG(1,(TEXT("  DnsCall_Query: leave failed--4\r\n")));
		return DNSERR_NO_BUFS;
	}
	DnsItem_Set( pItemDNS, pszNameDns );

	//搜索所有IP接口的DNS地址链表，逐个查询
	dwErr = DnsQry_GetFromNet( pItemDNS );

	//如果得到信息
	if( (dwErr==DNSERR_SUCCESS) && pItemDNS->pAddrList && pItemDNS->wCntAddrReal )
	{
		//获取信息，给用户
		wTmp = pItemDNS->wCntAddrReal;
		Skt_SavHostentAddr( pHostInfo, (DWORD)len, pItemDNS->pAddrList, sizeof(DWORD), &wTmp );
		*pwCntAddr = wTmp;
		
		//保存信息到数据库中
		DnsTbl_SaveHostAddr( pszNameDns, pItemDNS->pAddrList, ADDRLEN_IPV4, wTmp );
	}

	//释放
	DnsItem_Del( pItemDNS );

	RETAILMSG(1,(TEXT("  DnsCall_Query: leave return [%d]--\r\n"), dwErr));
	return dwErr;
}


#define	DNSSRV_MAX			20
DWORD	DnsQry_GetFromNet( DNS_ITEM* pItemDNS )
{
	BYTE			pBufDnsList[DNSSRV_MAX*ADDRLEN_IPV4];
	DWORD*			pdwDnsSrvIP = (DWORD*)pBufDnsList;
	DWORD			dwCntDnsSrv;
	DWORD			dwErr;
	BOOL			fExit;
	DWORD			i;

	RETAILMSG(1,(TEXT("  DnsQry_GetFromNet: Enter\r\n")));
	//搜索 所有IP接口的DNS地址
	if( !(dwCntDnsSrv=IPHAPI_GetDnsSrvList( pdwDnsSrvIP, DNSSRV_MAX )) )
	{
		RETAILMSG(1,(TEXT("  DnsQry_GetFromNet: leave ffffff\r\n")));
		return DNSERR_NO_DNSSRV;
	}

	//查询所有 DNS Server
	dwErr = DNSERR_NOT_FOUND;
	fExit = FALSE;
	for( i=0; i<dwCntDnsSrv; i++ )
	{
		pItemDNS->dwDnsSrvIP = *pdwDnsSrvIP;
		//发送查询，等待应答
		RETAILMSG(1,(TEXT("  DnsQry_GetFromNet: call DnsQry_SendReq\r\n")));
		if( DnsQry_SendReq( pItemDNS, &fExit ) )
		{
			//处理应答
			if( !DnsQry_Handle( pItemDNS ) )
			{
				dwErr = DNSERR_WRONG_RESP;
				break;
			}
			//处理数据包
			if( pItemDNS->pAddrList && pItemDNS->wCntAddrReal )
			{
				//如果得到想要的信息，就退出
				dwErr = DNSERR_SUCCESS;
				break;
			}
		}
		//
		if( fExit )
		{
			break;
		}
		//
		pdwDnsSrvIP ++;
	}
	
	RETAILMSG(1,(TEXT("  DnsQry_GetFromNet: leave xxxx\r\n")));
	return dwErr;
}


BOOL	DnsQry_SendReq( DNS_ITEM* pItemDNS, BOOL* pfExit )
{
	SOCKADDR_IN		stAddrNet;
	fd_set			fds;
	struct timeval	tv;
	int				nRet;
	int				nTmp;
	int				j;
	DWORD			dwErr;
	BOOL			fSuccess;

	//查询 3次
	dwErr = E_SUCCESS;
	fSuccess = FALSE;
	for( j=0; j<3; j++ )
	{
		RETAILMSG(1,(TEXT("  DnsQry_SendReq: SKAPI_SendTo[%d]\r\n"), pItemDNS->wLenDns));
		//发送数据包
		stAddrNet.sin_family = AF_INET;
		stAddrNet.sin_port = pItemDNS->wDnsSrvPort;
		stAddrNet.sin_addr.S_un.S_addr = pItemDNS->dwDnsSrvIP;
		if( (nRet=SKAPI_SendTo( pItemDNS->skDns, pItemDNS->pBufDNS, pItemDNS->wLenDns, 0, (struct sockaddr *)&stAddrNet, sizeof(SOCKADDR_IN) ))==SOCKET_ERROR )
		{
			dwErr = SKAPI_WSAGetLastError();
			goto EXIT_DNSREQ;
		}
		//等待数据包
		FD_ZERO(&fds);
		FD_SET( pItemDNS->skDns, &fds );
		tv.tv_sec = DNSWAIT_TIME;
		tv.tv_usec = 0;
		if( SKAPI_Select( 0, &fds, NULL, NULL, &tv )!=1 )
		{
			dwErr = SKAPI_WSAGetLastError();
			if( dwErr==E_NOTINITIALISED )
			{
				goto EXIT_DNSREQ;
			}
		}
		else
		{
			break;
		}
	}
	//接收数据包
	nRet = 0;
	if( (SKAPI_IoctlSocket( pItemDNS->skDns, FIONREAD, (DWORD*)&nRet )==SOCKET_ERROR) || (nRet==0) )
	{
		dwErr = SKAPI_WSAGetLastError();
		goto EXIT_DNSREQ;
	}
	Sleep( 1 );
	nTmp = sizeof( SOCKADDR_IN );
	if( ((nRet=SKAPI_RecvFrom( pItemDNS->skDns, (char *)pItemDNS->pDataRes, LEN_DNSRES, 0, (struct sockaddr *)&stAddrNet, &nTmp ))==SOCKET_ERROR) || !nRet )
	{
		dwErr = SKAPI_WSAGetLastError();
		goto EXIT_DNSREQ;
	}
	RETAILMSG(1,(TEXT("  DnsQry_SendReq: SKAPI_RecvFrom[%d]\r\n"), nRet));
	//
	pItemDNS->wLenResRx = (WORD)nRet;
	fSuccess = TRUE;

EXIT_DNSREQ:
	if( dwErr==E_NOTINITIALISED )
	{
		*pfExit = TRUE;
	}

	return fSuccess;
}


BOOL	DnsQry_Handle( DNS_ITEM* pItemDNS )
{
	LPBYTE			pDataRes = pItemDNS->pDataRes;//接收的 回答数据包
	LPBYTE			pDataEnd = pDataRes + pItemDNS->wLenResRx;
	LPBYTE			pData;
	LPTSTR			pszName;
	WORD			wCntQD;
	WORD			wCntAnswer;
	WORD			wCntReply;

	WORD			wTypeRData;
	WORD			wClassRData;
	WORD			wLenRData;
	WORD			wTmp;

	//保证长度
	if( pItemDNS->wLenResRx <= (sizeof(DNS_HDR) +4) )
	{
		return FALSE;
	}
	//保证ID正确
	if( pntohs(pDataRes)!=pItemDNS->wXID )
	{
		return FALSE;
	}
	//分析 wFlag
	memcpy( &wTmp, pDataRes+2, sizeof(WORD) );
	if( !(wTmp & DNSF_QR_R) )
	{
		return FALSE;
	}
	if( (wTmp & DNSF_RCODE_MASK)!=DNSF_RCODE_OK )
	{
		return FALSE;
	}
	//分析 RR的个数
	wCntQD = pntohs( pDataRes+DNS_OFFSET_QD );
	wCntAnswer = pntohs(pDataRes+DNS_OFFSET_AN);
	wCntReply = wCntAnswer + pntohs(pDataRes+DNS_OFFSET_NS) + pntohs(pDataRes+DNS_OFFSET_AR);
	pData = pDataRes + sizeof(DNS_HDR);

	//略过 Question section
	while( wCntQD-- )
	{
		while( (pData<pDataEnd) && *pData )
		{
			pData ++;
		}
		pData ++;
		pData += 4;
	}
	if( pData>=pDataEnd )
	{
		return FALSE;
	}

	//分析 对方给的RR信息
	//while( wCntReply-- )
	while( wCntAnswer-- )
	{
		//略过 RR's NAME
		if( (*pData & 0xC0)==0xC0 )
		{
			//获取压缩后的位置
			pntohsp( pData, (LPBYTE)&wTmp );
			wTmp &= ~0xC000;
			pszName = (LPTSTR)(pDataRes + wTmp);

			pData += 2;
		}
		else
		{
			pszName = (LPTSTR)(pDataRes);
			while( (pData<pDataEnd) && *pData++ )
			{
				;
			}
		}
		//检查长度
		if( (pData>=pDataEnd) || (pData>(pDataEnd-DNS_RR_ATTR_LEN-DNS_RR_RLEN_LEN)) )
		{
			break;
		}
		//获取 TYPE、CLASS、RDLENGTH
		memcpy( &wTypeRData, pData, sizeof(WORD) );
		memcpy( &wClassRData, pData+2, sizeof(WORD) );
		wLenRData = pntohs( pData+DNS_RR_ATTR_LEN );
		if( wClassRData!=DNS_RRCLASS_IN )
		{
			//寻找下一个 RR
			pData += (DNS_RR_ATTR_LEN +DNS_RR_RLEN_LEN +wLenRData);
			if( pData>=pDataEnd )
			{
				break;
			}
			continue;
		}
		pData += (DNS_RR_ATTR_LEN+DNS_RR_RLEN_LEN);
		//分析“RDATA”，根据RR头信息：TYPE、CLASS、略过TTL、RDLENGTH
		switch( wTypeRData )
		{
		case DNS_RRTYPE_A:
			//保存1个主机地址信息: 
			if( wLenRData==sizeof(DWORD) )
			{
				DnsItem_SaveAddr( pItemDNS, pData );
			}
			break;

		//case DNS_RRTYPE_CNAME:
		//	//直接保存 CNAME信息
		//	break;

		//case DNS_RRTYPE_SOA:
		//	//获取 MNAME
		//	DNS_GetDomain( pItemDNS, pData, pDataRes, pDataEnd );
		//	//略过 后面的信息
		//	break;

		default :
			break;
		}

		//寻找下一个 RR
		pData += (wLenRData);
		if( pData>=pDataEnd )
		{
			break;
		}
	}

	return TRUE;
}



BOOL	DnsItem_New( DNS_ITEM** ppItemDNS, WORD wLenDns )
{
	DNS_ITEM*	pItemDNS;
	LPBYTE		pBufDNS;
	SOCKET		skDns;
	
	pItemDNS = (DNS_ITEM*)malloc( sizeof(DNS_ITEM) );
	if( !pItemDNS )
	{
		return FALSE;
	}
	pBufDNS = (LPBYTE)malloc( wLenDns );
	if( !pBufDNS )
	{
		free( pItemDNS );
		return FALSE;
	}
	skDns = SKAPI_Socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( skDns==INVALID_SOCKET )
	{
		free( pItemDNS );
		free( pBufDNS );
		return FALSE;
	}
	//
	memset( pItemDNS, 0, sizeof(DNS_ITEM) );
	memset( pBufDNS, 0, wLenDns );
	//
	pItemDNS->pBufDNS = pBufDNS;
	pItemDNS->wLenDns = wLenDns;
	pItemDNS->skDns=skDns;
	pItemDNS->wDnsSrvPort = htons_m(IPPORT_DNS_SRV);
	pItemDNS->wXID= g_lpGlobalDNS->wXID++;
	//
	*ppItemDNS = pItemDNS;
	return TRUE;
}

void	DnsItem_Set( DNS_ITEM* pItemDNS, LPCSTR pszNameDns )
{
	DNS_HDR*		pHdrDNS;
	LPBYTE			pBufDNS;
	LPBYTE			pData;
	WORD			wTmp;

	//初始化 DNS数据包
	pBufDNS = pItemDNS->pBufDNS;
	pHdrDNS = (DNS_HDR*)pBufDNS;
	//填充 wFlag 和 wCntQD
	pHdrDNS->wXID = htons(pItemDNS->wXID);
	pHdrDNS->wFlag = (DNSF_QR_Q | DNSF_OP_STD | DNSF_RD );//
	pHdrDNS->wCntQD = htons_m(1);
	//填充 Question section----- QNAME / QTYPE / QCLASS 
	pData = DNS_ToSavName( pBufDNS+sizeof(DNS_HDR), (LPBYTE)pszNameDns );
	wTmp = DNS_RRTYPE_A;
	memcpy( pData, &wTmp, sizeof(WORD) );
	pData += sizeof(WORD);
	wTmp = DNS_RRCLASS_IN;
	memcpy( pData, &wTmp, sizeof(WORD) );
	pData += sizeof(WORD);
}

void	DnsItem_Del( DNS_ITEM* pItemDNS )
{
	//释放
	if( pItemDNS->pBufDNS )
	{
		free( pItemDNS->pBufDNS );
	}
	if( pItemDNS->pAddrList )
	{
		free( pItemDNS->pAddrList );
	}
	if( (pItemDNS->skDns) && (pItemDNS->skDns!=INVALID_SOCKET) )
	{
		//pItemDNS->skDns=INVALID_SOCKET;
		SKAPI_CloseSocket( pItemDNS->skDns );
	}

	free( pItemDNS );
}

BOOL	DnsItem_SaveAddr( DNS_ITEM* pItemDNS, LPBYTE pAddr )
{
	LPBYTE		pBuf;
	WORD		i;
	WORD		nReal;

	//判断 地址列表空间是否够
	if( !pItemDNS->pAddrList )
	{
		//分配主机地址信息BUFFER
		pItemDNS->wCntAddrBuf = 3;
		pItemDNS->pAddrList = (LPBYTE)malloc( pItemDNS->wCntAddrBuf*sizeof(DWORD) );
		if( !pItemDNS->pAddrList )
		{
			return FALSE;
		}
	}
	else if( pItemDNS->wCntAddrReal>=pItemDNS->wCntAddrBuf )
	{
		LPBYTE	pTmp;

		//重新 分配主机地址信息BUFFER
		pTmp = (LPBYTE)malloc( (pItemDNS->wCntAddrBuf+3)*sizeof(DWORD) );
		if( !pTmp )
		{
			return FALSE;
		}
		memcpy( pTmp, pItemDNS->pAddrList, pItemDNS->wCntAddrBuf*sizeof(DWORD) );
		free( pItemDNS->pAddrList );
		
		pItemDNS->pAddrList = pTmp;
		pItemDNS->wCntAddrBuf += 3;
	}
	//比较是否重复
	pBuf = pItemDNS->pAddrList;
	nReal = pItemDNS->wCntAddrReal;
	for( i=0; i<nReal; i++ )
	{
		if( memcmp(pBuf, pAddr, sizeof(DWORD))==0 )
		{
			return TRUE;
		}
		pBuf += sizeof(DWORD);
	}
	//保存地址
	memcpy( pItemDNS->pAddrList+pItemDNS->wCntAddrReal*sizeof(DWORD), pAddr, sizeof(DWORD) );
	pItemDNS->wCntAddrReal ++;
	return TRUE;
}





/*
//此函数使用条件： 对Domain的压缩表示，只能纵向进入，不能出现横向有2个或以上的压缩字节。
BOOL	DNS_GetDomain( DNS_ITEM* pItemDNS, LPBYTE pDomain, LPBYTE pDataBegin, LPBYTE pDataEnd )
{
	LPBYTE		pSavDomain;
	LPBYTE		pTmp;
	WORD		wLenSav;
	WORD		wLenReal;
	WORD		wTmp;
	BYTE		bLen;

	//分配缓冲
	wLenSav = 256;
	wLenReal = 0;
	pSavDomain = (LPBYTE)malloc( wLenSav );
	if( !pSavDomain )
	{
		return FALSE;
	}
	//分析所有字节
	while( (pDomain<pDataEnd) && (*pDomain) )
	{
		if( (*pDomain & 0xC0)==0xC0 )
		{
			//获取压缩后的位置
			memcpy( &wTmp, pDomain, 2 );
			pTmp = &wTmp;
			(*pTmp) &= ~0xC0;
#ifndef	BIG_END
			wTmp = ntots_m( wTmp );
#endif
			pDomain = pDataRes + wTmp;
		}
		else
		{
			//获取段长度
			bLen = *pDomain++;
			//获取段长度内的数据
			while( (pDomain<pDataEnd) && bLen-- )
			{
				if( wLenReal>=wLenSav )
				{
					//重新分配
					pTmp = (LPBYTE)malloc( wLenSav+256 );
					if( !pTmp )
					{
						free( pSavDomain );
						return FALSE;
					}
					memcpy( pTmp, pSavDomain, wLenReal );
					wLenSav += 256;
				}
				pSavDomain[wLenReal++] = *pDomain++;
			}
			//设置段长度的DOT
			pSavDomain[wLenReal++] = '.';
		}
	}
	//
	if( wLenReal )
	{
		free( pSavDomain );
		return FALSE;
	}
	else
	{
		//保存数据
		pSavDomain[wLenReal] = 0;
		DNS_SavDomain( pItemDNS, pSavDomain );

		//
		free( pSavDomain );
		return TRUE;
	}
}
*/

BOOL	DNS_SavDomain( DNS_ITEM* pItemDNS, LPBYTE pName )
{
	//比较是否重复

	//保存

	return TRUE;
}


WORD	DNS_CheckName( LPBYTE pName )
{
	WORD		wTmp = 0;

	while( pName[wTmp] )
	{
		if( (pName[wTmp]>='a' && *pName<='z') ||
			(pName[wTmp]>='A' && *pName<='Z') ||
			(pName[wTmp]=='.') ||
			(pName[wTmp]=='-')
		  )
		{
			wTmp ++;
		}
		else
		{
			wTmp = 0;
			break;
		}
	}

	return wTmp;
}

LPBYTE	DNS_ToSavName( OUT LPBYTE pNameByDNS, LPBYTE pName )
{
	LPBYTE		pTmp;

	pTmp = pNameByDNS + 1;
	while( *pName )
	{
		if( *pName=='.' )
		{
			*pNameByDNS = (BYTE)(pTmp - pNameByDNS - 1);
			pNameByDNS = pTmp;
		}
		else
		{
			*pTmp = *pName;
		}

		pTmp ++;
		pName ++;
	}
	*pNameByDNS = (BYTE)(pTmp - pNameByDNS - 1);
	*pTmp++ = 0;

	return pTmp;
}


LPBYTE	DNS_ToNormalName( LPBYTE pNameByDNS, OUT LPBYTE pName )
{
	LPBYTE		pTmp;

	pTmp = pNameByDNS + 1;
	while( *pName )
	{
		if( *pName=='.' )
		{
			*pNameByDNS = (BYTE)(pTmp - pNameByDNS - 1);
			pNameByDNS = pTmp;
		}
		else
		{
			*pTmp = *pName;
		}

		pTmp ++;
		pName ++;
	}
	*pNameByDNS = (BYTE)(pTmp - pNameByDNS - 1);
	*pTmp++ = 0;

	return pTmp;
}

