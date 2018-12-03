/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：TCP/IP的CRC校验函数
版本号：  1.0.0
开发时期：2004-03-09
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <endalign.h>
//"\inc_local"
#include "proto_ip_tcp.h"


/***************  全局区 定义， 声明 *****************/

/******************************************************/


//little end---BYTE流： << 高，“低高，低高，”。。。“低 或者 低高”结束 >>
//big    end---BYTE流： << 低，“高低，高低，”。。。“高 或者 高低”结束 >>
WORD	IP_CRC( LPBYTE pBuf, DWORD dwLenBuf )
{
	DWORD		dwSum;
	BOOL		fReorder;
#ifndef	BIG_END
	LPBYTE		pData;
#endif
	DWORD		nLen = dwLenBuf;
	WORD*		pwBuf;
	WORD		wCRC;
	
	if( !dwLenBuf )
	{
		return 0xFFFF;
	}
	dwSum = 0;
	//奇数地址
	if( (DWORD)pBuf & 1 )
	{
		fReorder = TRUE;
#ifndef	BIG_END
		dwSum += ( (*pBuf)<<8 ) & 0xFF00;
#else
		dwSum += (*pBuf);
#endif
		nLen --;
		//偶数地址
		pwBuf = (WORD*)(pBuf + 1);
		while( nLen > sizeof(BYTE) )
		{
			dwSum += *(pwBuf);
			
			pwBuf ++;
			nLen -= sizeof(WORD);
		}
	}
	else
	{
		fReorder = FALSE;
		pwBuf = (WORD*)pBuf;
		while( nLen>sizeof(BYTE) )
		{
			dwSum += (*pwBuf);
			
			pwBuf ++;
			nLen -= sizeof(WORD);
		}
	}
	//
	if( nLen )
	{
#ifndef	BIG_END
		// “低”结束
		//dwSum += ( ((WORD)(*(BYTE*)pwBuf)) & 0x00FF );
		pData = (LPBYTE)pwBuf;
		dwSum += (*pData);
#else
		// “高”结束
		dwSum += ( ((WORD)(*(BYTE*)pwBuf))<<8 ) & 0xFF00;
		//dwSum += ( ((WORD)(*(BYTE*)pwBuf)) & 0x00FF );
#endif
	}

	//最后高位字
	while( dwSum & 0xFFFF0000L )
	{
		dwSum = (dwSum & 0xFFFFL) + (dwSum >> 16);
	}
	//返回校验码
	wCRC = ~((WORD)dwSum);
	if( fReorder )
	{
		return REORDER_S_OK(wCRC);
	}
	else
	{
		return wCRC;
	}
}


WORD	Tcp_CRC( DWORD dwSrcIP, DWORD dwDesIP, BYTE bProType, LPBYTE pBufData, WORD wLenData )
{
	PSEUDO_HDR	HdrPseudo;
	DWORD		dwSum;
	LPBYTE		pData;
	WORD		wLenTotal;

	//参数检查
	if( !pBufData || !wLenData )
	{
		return 0xFFFF;
	}
	//
	dwSum = 0;
	wLenTotal = 0;
	pData = pBufData;
	//所有数据字节求和
	while( wLenData>sizeof(BYTE) )
	{
		dwSum += ( (*pData)<<8 ) & 0xFF00;
		pData++;
		dwSum += *pData;
		pData++;
		
		wLenData -= sizeof(WORD);
		wLenTotal += sizeof(WORD);
	}
	if( wLenData )
	{
		dwSum += ( (*pData)<<8 ) & 0xFF00;
		wLenTotal ++;
	}
	//伪码头 初始化
	HdrPseudo.dwSrcIP = dwSrcIP;
	HdrPseudo.dwDesIP = dwDesIP;
	HdrPseudo.bZero = 0;
	HdrPseudo.bProType = bProType;
	HdrPseudo.wTotalUDPLen = htons_m(wLenTotal);
	//伪码头 求和
	pData = (BYTE*)&HdrPseudo;
	wLenData = sizeof(PSEUDO_HDR);
	while( wLenData>sizeof(BYTE) )
	{
		dwSum += ( (*pData)<<8 ) & 0xFF00;
		pData++;
		dwSum += *pData;
		pData++;
		
		wLenData -= sizeof(WORD);
	}

	//最后高位字
	while( dwSum & 0xFFFF0000L )
	{
		dwSum = (dwSum & 0xFFFFL) + (dwSum >> 16);
	}
	//返回校验码
	wLenTotal = ~((WORD)dwSum);
	return REORDER_S(wLenTotal);
}


WORD	Tcp_CRC2( DWORD dwSrcIP, DWORD dwDesIP, BYTE bProType, FRAGSETA* pFragSet )
{
	PSEUDO_HDR	HdrPseudo;
	FRAGMENT*	pFrag;
	FRAGMENT*	pFragEnd;
	LONG		nLen;

	DWORD		dwSum;
	LPBYTE		pData;
	WORD		wLenTotal;

	//
	dwSum = 0;
	wLenTotal = 0;
	//所有数据字节求和
	pFrag = pFragSet->pFrag;
	pFragEnd = pFrag + pFragSet->nCnt;
	for( ; pFrag<pFragEnd; pFrag++ )
	{
		pData = pFrag->pData;
		nLen  = pFrag->nLen;
		if( !pData || !nLen )
		{
			break;
		}

		//
		//开始处理CRC
		//
		while( nLen>sizeof(BYTE) )
		{
			dwSum += ( (*pData)<<8 ) & 0xFF00;
			pData++;
			dwSum += *pData;
			pData++;

			nLen -= sizeof(WORD);
			wLenTotal += sizeof(WORD);
		}
		if( nLen )
		{
			dwSum += ( (*pData)<<8 ) & 0xFF00;
			wLenTotal ++;
		}
	}
	//伪码头 初始化
	HdrPseudo.dwSrcIP = dwSrcIP;
	HdrPseudo.dwDesIP = dwDesIP;
	HdrPseudo.bZero = 0;
	HdrPseudo.bProType = bProType;
	HdrPseudo.wTotalUDPLen = htons_m(wLenTotal);
	//伪码头 求和
	pData = (BYTE*)&HdrPseudo;
	nLen = sizeof(PSEUDO_HDR);
	while( nLen>sizeof(BYTE) )
	{
		dwSum += ( (*pData)<<8 ) & 0xFF00;
		pData++;
		dwSum += *pData;
		pData++;
		
		nLen -= sizeof(WORD);
	}

	//最后高位字
	while( dwSum & 0xFFFF0000L )
	{
		dwSum = (dwSum & 0xFFFFL) + (dwSum >> 16);
	}
	//返回校验码
	wLenTotal = ~((WORD)dwSum);
	return REORDER_S(wLenTotal);
}


