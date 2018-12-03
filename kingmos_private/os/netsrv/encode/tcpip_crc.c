/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����TCP/IP��CRCУ�麯��
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <endalign.h>
//"\inc_local"
#include "proto_ip_tcp.h"


/***************  ȫ���� ���壬 ���� *****************/

/******************************************************/


//little end---BYTE���� << �ߣ����͸ߣ��͸ߣ������������� ���� �͸ߡ����� >>
//big    end---BYTE���� << �ͣ����ߵͣ��ߵͣ������������� ���� �ߵ͡����� >>
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
	//������ַ
	if( (DWORD)pBuf & 1 )
	{
		fReorder = TRUE;
#ifndef	BIG_END
		dwSum += ( (*pBuf)<<8 ) & 0xFF00;
#else
		dwSum += (*pBuf);
#endif
		nLen --;
		//ż����ַ
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
		// ���͡�����
		//dwSum += ( ((WORD)(*(BYTE*)pwBuf)) & 0x00FF );
		pData = (LPBYTE)pwBuf;
		dwSum += (*pData);
#else
		// ���ߡ�����
		dwSum += ( ((WORD)(*(BYTE*)pwBuf))<<8 ) & 0xFF00;
		//dwSum += ( ((WORD)(*(BYTE*)pwBuf)) & 0x00FF );
#endif
	}

	//����λ��
	while( dwSum & 0xFFFF0000L )
	{
		dwSum = (dwSum & 0xFFFFL) + (dwSum >> 16);
	}
	//����У����
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

	//�������
	if( !pBufData || !wLenData )
	{
		return 0xFFFF;
	}
	//
	dwSum = 0;
	wLenTotal = 0;
	pData = pBufData;
	//���������ֽ����
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
	//α��ͷ ��ʼ��
	HdrPseudo.dwSrcIP = dwSrcIP;
	HdrPseudo.dwDesIP = dwDesIP;
	HdrPseudo.bZero = 0;
	HdrPseudo.bProType = bProType;
	HdrPseudo.wTotalUDPLen = htons_m(wLenTotal);
	//α��ͷ ���
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

	//����λ��
	while( dwSum & 0xFFFF0000L )
	{
		dwSum = (dwSum & 0xFFFFL) + (dwSum >> 16);
	}
	//����У����
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
	//���������ֽ����
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
		//��ʼ����CRC
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
	//α��ͷ ��ʼ��
	HdrPseudo.dwSrcIP = dwSrcIP;
	HdrPseudo.dwDesIP = dwDesIP;
	HdrPseudo.bZero = 0;
	HdrPseudo.bProType = bProType;
	HdrPseudo.wTotalUDPLen = htons_m(wLenTotal);
	//α��ͷ ���
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

	//����λ��
	while( dwSum & 0xFFFF0000L )
	{
		dwSum = (dwSum & 0xFFFFL) + (dwSum >> 16);
	}
	//����У����
	wLenTotal = ~((WORD)dwSum);
	return REORDER_S(wLenTotal);
}


