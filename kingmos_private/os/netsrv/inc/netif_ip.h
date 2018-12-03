/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_IP_H_
#define	_NETIF_IP_H_

#ifdef __cplusplus
extern "C" {
#endif




//------Net's ���ͺ��� �� �����ṹ ����--------
typedef	DWORD	(*FNNETOUT)(HANDLE hIfNet, LPBYTE pBuf, DWORD dwLen );


// ------------------------------------------------------
// 
// Net(IP) ������ṩ�� ��������
// 
// ------------------------------------------------------

//Net�� �ṩ����Ϣ
typedef	struct	_NET_LOCAL
{
	HANDLE			hIfDev;

	FNPRONOTIFY		pFnNetNotify;
	FNPROQUERY		pFnNetQuery;
	FNPROSET		pFnNetSet;
	FNNETOUT		pFnNetOut;

	FNPROIN			pFnNetIn;

} NET_LOCAL;



//
extern	BOOL	IP_IsUp( );
extern	BOOL	IP_GetBestAddr( DWORD* pdwSrcIP, DWORD dwDesIP, BOOL fGetAny );
extern	BOOL	IP_IsValidAddr( DWORD dwSrcIP );
extern	BOOL	IP_Query( DWORD dwSrcIP, NETINFO_IPV4* pInfoNet );
extern	HANDLE	IP_GetIntfByIndex( DWORD dwIndex );



#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_IP_H_
