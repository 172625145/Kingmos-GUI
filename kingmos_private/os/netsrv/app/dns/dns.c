/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����DNS
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��
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

/***************  ȫ���� ���壬 ���� *****************/

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

	//����1��TCP�Ĺ��� �ͻ�����ʼ��
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

	//�������
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
	//������������ ����
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
	//����DNS���ݰ� ��ѯ
	//
	if( dwOptDns & DNSOPT_NOT_FROM_NET )
	{
		RETAILMSG(1,(TEXT("  DnsCall_Query: leave failed--3\r\n")));
		return DNSERR_NOT_FOUND;
	}
	//���� 1����ѯ��
	wTmp = sizeof(DNS_HDR) +wLenName +2 +sizeof(WORD) +sizeof(WORD);
	if( !DnsItem_New( &pItemDNS, wTmp ) )
	{
		RETAILMSG(1,(TEXT("  DnsCall_Query: leave failed--4\r\n")));
		return DNSERR_NO_BUFS;
	}
	DnsItem_Set( pItemDNS, pszNameDns );

	//��������IP�ӿڵ�DNS��ַ���������ѯ
	dwErr = DnsQry_GetFromNet( pItemDNS );

	//����õ���Ϣ
	if( (dwErr==DNSERR_SUCCESS) && pItemDNS->pAddrList && pItemDNS->wCntAddrReal )
	{
		//��ȡ��Ϣ�����û�
		wTmp = pItemDNS->wCntAddrReal;
		Skt_SavHostentAddr( pHostInfo, (DWORD)len, pItemDNS->pAddrList, sizeof(DWORD), &wTmp );
		*pwCntAddr = wTmp;
		
		//������Ϣ�����ݿ���
		DnsTbl_SaveHostAddr( pszNameDns, pItemDNS->pAddrList, ADDRLEN_IPV4, wTmp );
	}

	//�ͷ�
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
	//���� ����IP�ӿڵ�DNS��ַ
	if( !(dwCntDnsSrv=IPHAPI_GetDnsSrvList( pdwDnsSrvIP, DNSSRV_MAX )) )
	{
		RETAILMSG(1,(TEXT("  DnsQry_GetFromNet: leave ffffff\r\n")));
		return DNSERR_NO_DNSSRV;
	}

	//��ѯ���� DNS Server
	dwErr = DNSERR_NOT_FOUND;
	fExit = FALSE;
	for( i=0; i<dwCntDnsSrv; i++ )
	{
		pItemDNS->dwDnsSrvIP = *pdwDnsSrvIP;
		//���Ͳ�ѯ���ȴ�Ӧ��
		RETAILMSG(1,(TEXT("  DnsQry_GetFromNet: call DnsQry_SendReq\r\n")));
		if( DnsQry_SendReq( pItemDNS, &fExit ) )
		{
			//����Ӧ��
			if( !DnsQry_Handle( pItemDNS ) )
			{
				dwErr = DNSERR_WRONG_RESP;
				break;
			}
			//�������ݰ�
			if( pItemDNS->pAddrList && pItemDNS->wCntAddrReal )
			{
				//����õ���Ҫ����Ϣ�����˳�
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

	//��ѯ 3��
	dwErr = E_SUCCESS;
	fSuccess = FALSE;
	for( j=0; j<3; j++ )
	{
		RETAILMSG(1,(TEXT("  DnsQry_SendReq: SKAPI_SendTo[%d]\r\n"), pItemDNS->wLenDns));
		//�������ݰ�
		stAddrNet.sin_family = AF_INET;
		stAddrNet.sin_port = pItemDNS->wDnsSrvPort;
		stAddrNet.sin_addr.S_un.S_addr = pItemDNS->dwDnsSrvIP;
		if( (nRet=SKAPI_SendTo( pItemDNS->skDns, pItemDNS->pBufDNS, pItemDNS->wLenDns, 0, (struct sockaddr *)&stAddrNet, sizeof(SOCKADDR_IN) ))==SOCKET_ERROR )
		{
			dwErr = SKAPI_WSAGetLastError();
			goto EXIT_DNSREQ;
		}
		//�ȴ����ݰ�
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
	//�������ݰ�
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
	LPBYTE			pDataRes = pItemDNS->pDataRes;//���յ� �ش����ݰ�
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

	//��֤����
	if( pItemDNS->wLenResRx <= (sizeof(DNS_HDR) +4) )
	{
		return FALSE;
	}
	//��֤ID��ȷ
	if( pntohs(pDataRes)!=pItemDNS->wXID )
	{
		return FALSE;
	}
	//���� wFlag
	memcpy( &wTmp, pDataRes+2, sizeof(WORD) );
	if( !(wTmp & DNSF_QR_R) )
	{
		return FALSE;
	}
	if( (wTmp & DNSF_RCODE_MASK)!=DNSF_RCODE_OK )
	{
		return FALSE;
	}
	//���� RR�ĸ���
	wCntQD = pntohs( pDataRes+DNS_OFFSET_QD );
	wCntAnswer = pntohs(pDataRes+DNS_OFFSET_AN);
	wCntReply = wCntAnswer + pntohs(pDataRes+DNS_OFFSET_NS) + pntohs(pDataRes+DNS_OFFSET_AR);
	pData = pDataRes + sizeof(DNS_HDR);

	//�Թ� Question section
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

	//���� �Է�����RR��Ϣ
	//while( wCntReply-- )
	while( wCntAnswer-- )
	{
		//�Թ� RR's NAME
		if( (*pData & 0xC0)==0xC0 )
		{
			//��ȡѹ�����λ��
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
		//��鳤��
		if( (pData>=pDataEnd) || (pData>(pDataEnd-DNS_RR_ATTR_LEN-DNS_RR_RLEN_LEN)) )
		{
			break;
		}
		//��ȡ TYPE��CLASS��RDLENGTH
		memcpy( &wTypeRData, pData, sizeof(WORD) );
		memcpy( &wClassRData, pData+2, sizeof(WORD) );
		wLenRData = pntohs( pData+DNS_RR_ATTR_LEN );
		if( wClassRData!=DNS_RRCLASS_IN )
		{
			//Ѱ����һ�� RR
			pData += (DNS_RR_ATTR_LEN +DNS_RR_RLEN_LEN +wLenRData);
			if( pData>=pDataEnd )
			{
				break;
			}
			continue;
		}
		pData += (DNS_RR_ATTR_LEN+DNS_RR_RLEN_LEN);
		//������RDATA��������RRͷ��Ϣ��TYPE��CLASS���Թ�TTL��RDLENGTH
		switch( wTypeRData )
		{
		case DNS_RRTYPE_A:
			//����1��������ַ��Ϣ: 
			if( wLenRData==sizeof(DWORD) )
			{
				DnsItem_SaveAddr( pItemDNS, pData );
			}
			break;

		//case DNS_RRTYPE_CNAME:
		//	//ֱ�ӱ��� CNAME��Ϣ
		//	break;

		//case DNS_RRTYPE_SOA:
		//	//��ȡ MNAME
		//	DNS_GetDomain( pItemDNS, pData, pDataRes, pDataEnd );
		//	//�Թ� �������Ϣ
		//	break;

		default :
			break;
		}

		//Ѱ����һ�� RR
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

	//��ʼ�� DNS���ݰ�
	pBufDNS = pItemDNS->pBufDNS;
	pHdrDNS = (DNS_HDR*)pBufDNS;
	//��� wFlag �� wCntQD
	pHdrDNS->wXID = htons(pItemDNS->wXID);
	pHdrDNS->wFlag = (DNSF_QR_Q | DNSF_OP_STD | DNSF_RD );//
	pHdrDNS->wCntQD = htons_m(1);
	//��� Question section----- QNAME / QTYPE / QCLASS 
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
	//�ͷ�
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

	//�ж� ��ַ�б�ռ��Ƿ�
	if( !pItemDNS->pAddrList )
	{
		//����������ַ��ϢBUFFER
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

		//���� ����������ַ��ϢBUFFER
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
	//�Ƚ��Ƿ��ظ�
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
	//�����ַ
	memcpy( pItemDNS->pAddrList+pItemDNS->wCntAddrReal*sizeof(DWORD), pAddr, sizeof(DWORD) );
	pItemDNS->wCntAddrReal ++;
	return TRUE;
}





/*
//�˺���ʹ�������� ��Domain��ѹ����ʾ��ֻ��������룬���ܳ��ֺ�����2�������ϵ�ѹ���ֽڡ�
BOOL	DNS_GetDomain( DNS_ITEM* pItemDNS, LPBYTE pDomain, LPBYTE pDataBegin, LPBYTE pDataEnd )
{
	LPBYTE		pSavDomain;
	LPBYTE		pTmp;
	WORD		wLenSav;
	WORD		wLenReal;
	WORD		wTmp;
	BYTE		bLen;

	//���仺��
	wLenSav = 256;
	wLenReal = 0;
	pSavDomain = (LPBYTE)malloc( wLenSav );
	if( !pSavDomain )
	{
		return FALSE;
	}
	//���������ֽ�
	while( (pDomain<pDataEnd) && (*pDomain) )
	{
		if( (*pDomain & 0xC0)==0xC0 )
		{
			//��ȡѹ�����λ��
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
			//��ȡ�γ���
			bLen = *pDomain++;
			//��ȡ�γ����ڵ�����
			while( (pDomain<pDataEnd) && bLen-- )
			{
				if( wLenReal>=wLenSav )
				{
					//���·���
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
			//���öγ��ȵ�DOT
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
		//��������
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
	//�Ƚ��Ƿ��ظ�

	//����

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

