/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_COMM_H_
#define	_NETIF_COMM_H_

#ifndef _HANDLE_CHK_2004_H_
#include "handle_check.h"
#endif

#ifndef _FRAGMENT_H_
#include "fragment.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------
// TLV and TLD struct
//------------------------------------------------------
#if 0
typedef	struct	_TLD_HDR
{
	BYTE	bType;
	BYTE	bLenTotal;
//	BYTE	bData[0];

} TLD_HDR;

typedef	struct	_TLV_HDR
{
	WORD	wType;
	WORD	wLenTotal;	// the total lenght of TLV
//	BYTE	bValue[0];

} TLV_HDR;
#endif

//------------------------------------------------------
// NETINFO_IPV4 
//------------------------------------------------------

//�����豸�� ������Ϣ
typedef	struct	_NETINFO_IPV4
{
	DWORD		dwSize;
	DWORD		dwMask;

	DWORD		dwIP;
	DWORD		dwIP_Mask;
	DWORD		dwIP_Gateway;
	DWORD		dwIP_DNS;
	DWORD		dwIP_DNS2;

	DWORD		dwEnableDhcp;
	DWORD		dwIP_DHCP;
	DWORD		dwT1;
	DWORD		dwT2;
	DWORD		dwT_Lease;

	WORD		wMTU;	//һ��ָIP�㴫��ĳ��ȣ�����IPͷ��IP���ݣ�
	WORD		wXXX;

} NETINFO_IPV4;

//dwMask
#define	NET_MASK_IP					0x80000000
#define	NET_MASK_IP_Mask			0x40000000
#define	NET_MASK_IP_Gateway			0x20000000
#define	NET_MASK_IP_DNS				0x10000000
#define	NET_MASK_IP_DNS2			0x80000000

#define	NET_MASK_EnableDhcp			0x04000000
#define	NET_MASK_IP_DHCP			0x02000000
#define	NET_MASK_T1					0x01000000
#define	NET_MASK_T2					0x08000000
#define	NET_MASK_T_Lease			0x00400000
#define	NET_MASK_MTU				0x00200000


//------֪ͨ���� �� �¼��� ����--------
typedef	DWORD	(*FNPRONOTIFY)(HANDLE hIfPro, DWORD dwEvent);
//dwEvent
/*
#define	PROEVT_DEV_INSERT	0x10000001	//�豸 ����---���� RAS�豸ע���� ����
#define	PROEVT_DEV_REMOVE	0x10000002	//�豸 ����---���� RAS�豸ע���� ����

#define	PROEVT_DEV_LINKED	0x10000010	//�豸�� �Ѿ����ӳɹ�(�粦�ųɹ����ȡAC�ɹ�)
#define	PROEVT_DEV_STOP		0x10000020	//ͣ�� �豸��
#define	PROEVT_DEV_CLOSE	0x10000030	//�ͷ� �豸��
*/
#define	PROEVT_DL_UP		0x10000100
#define	PROEVT_DL_DN		0x10000200
#define	PROEVT_DL_STOP		0x10000300

#define	PROEVT_NET_UP		0x10001000
#define	PROEVT_NET_DN		0x10002000

#define	PROEVT_TL_UP		0x10010000
#define	PROEVT_TL_DN		0x10020000


//------��ѯ���� �� �����ṹ ����--------
typedef	BOOL	(*FNPROQUERY)( HANDLE hIfDev, OUT NETINFO_IPV4* pInfoNet );
typedef	BOOL	(*FNPROSET)( HANDLE hIfDev, OUT NETINFO_IPV4* pInfoNet );
typedef	BOOL	(*FNPRONAME)( HANDLE hIfDev, LPBYTE pBufName, DWORD dwLen );


//------���պ��� ����--------
typedef	DWORD	(*FNPROIN)(HANDLE hIfPro, LPBYTE pBufData, WORD wLenData);



#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_COMM_H_
