/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_IP_DEF_H_
#define	_IP_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// IP_INTF �Ķ��� �� ����
//-------------------------------------------------------------------------

typedef	struct	_IP_INTF
{
	HANDLE_THIS( _IP_INTF );

	LIST_UNIT			hListIntf;

	//----������Ϣ
	DWORD				dwIndex;
	//NETINFO_IPV4		InfoNet;
	DWORD				dwIP;
	DWORD				dwIP_Mask;
	DWORD				dwIP_Gateway;
	DWORD				dwIP_DNS;
	DWORD				dwIP_DNS2;
	//DWORD				dwIP_DHCP;
	//DWORD				dwT1;
	//DWORD				dwT2;
	//DWORD				dwT_Lease;
	WORD				wMTU;	//һ��ָIP�㴫��ĳ��ȣ�����IPͷ��IP���ݣ�
	BYTE				bTTL;
	BYTE				bXXX;

	//���� DHCP����Ϣ
	IP_ADDR_STRING		DhcpServer;
	time_t				LeaseObtained;
	time_t				LeaseExpires;

	//DataLink�㣺
	HANDLE				hIfDl;
	FNPRONOTIFY			pFnDlNotify;
	FNPROQUERY			pFnDlQuery;
	FNDLGETMIB			pFnDlGetMIB;
	FNPROSET			pFnDlSet;
	FNDLOUT				pFnDlOut;
	FNPRONAME			pFnDlName;

	//������Ϣ
	WORD				wFlag;
	WORD				wXID;

	//���յ��¼����̡߳�����
	HANDLE				hEvtDataIn;
	HANDLE				hThrdDataIn;
	CRITICAL_SECTION	csListDataIn;
	LIST_UNIT			hListDataIn;
	//���͵��¼����̡߳�����
	HANDLE				hEvtDataOut;
	HANDLE				hThrdDataOut;
	CRITICAL_SECTION	csListDataOut;
	LIST_UNIT			hListDataOut;

} IP_INTF;

//dwState
#define	IPS_LOADING		0
#define	IPS_WORK		1
#define	IPS_STOP		2
#define	IPS_EXIT		3

//wFlag
#define	IPIFF_EXIT			0x8000
#define	IPIFF_ROUTE			0x0002	//����ת�����ݰ�
#define	IPIFF_DHCP			0x0004	//����DHCP����

#include <ipdata.h>
/*
//-------------------------------------------------------------------------
// IP���ݶ���
//-------------------------------------------------------------------------
#define IPINF_NOAUTOFREE 0x1
typedef	struct	_IPDATA_IN
{
	LIST_UNIT			hListDataIn;
	//IP_INTF*			pNetIntf;

	WORD				wLenData;
	WORD				wFlag;		//��ʱ����
	//VOID				*pData;	//ָ���IP�ײ��ĵ�ַ(TCP/UDP...ͷ)
	union
	{
		IP_HDR			HdrIP;
		BYTE			pBufIP[1];
	};

} IPDATA_IN;

typedef	struct	_IPDATA_OUT
{
	LIST_UNIT			hListDataOut;

	//Ŀ�ĵ�ַ
	DWORD				dwDesIP;
	WORD				wLenTotal;
	WORD				wFlag;		//��ʱ����
	union
	{
		IP_HDR			HdrIP;
		BYTE			pBufIP[1];
	};

} IPDATA_OUT;
*/

//-------------------------------------------------------------------------
// IP ȫ����Ϣ
//-------------------------------------------------------------------------
typedef	struct	_IP_GLOBAL
{
	HANDLE_THIS( _IP_GLOBAL );

	//����IP�ӿڵ�����
	CRITICAL_SECTION	csListIntf;
	LIST_UNIT			hListIntf;

	//����IP��Ƭ������
	CRITICAL_SECTION	csListFgNode;
	LIST_UNIT			hListFgNode;
	HANDLE				hEvtFgNode;
	HANDLE				hThrdFgNode;

	//������Ϣ
	DWORD				dwAuto_Index;
	WORD				wFlag;
	WORD				wXXX;

	//for debug
	DWORD				dwTickDbg;
	DWORD				dwCntDbgIn;
	DWORD				dwCntDbgOut;

} IP_GLOBAL;

//
extern	IP_GLOBAL*		g_lpGlobalIP;

//wFlag
#define	GIPF_EXIT				0x8000
#define	GIPF_DEBUG				0x0002

//
#define	TIMER_FRAGNODE			(10*1000)



//
extern	BOOL	IP_FindSubnet( IP_INTF** ppNetIntf, DWORD dwDesIP, BOOL fGetAny );


#ifdef __cplusplus
}	
#endif

#endif	//_IP_DEF_H_
