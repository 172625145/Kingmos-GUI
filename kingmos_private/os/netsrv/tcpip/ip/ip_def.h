/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_IP_DEF_H_
#define	_IP_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// IP_INTF 的定义 和 操作
//-------------------------------------------------------------------------

typedef	struct	_IP_INTF
{
	HANDLE_THIS( _IP_INTF );

	LIST_UNIT			hListIntf;

	//----网络信息
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
	WORD				wMTU;	//一般指IP层传输的长度（包括IP头和IP数据）
	BYTE				bTTL;
	BYTE				bXXX;

	//保存 DHCP的信息
	IP_ADDR_STRING		DhcpServer;
	time_t				LeaseObtained;
	time_t				LeaseExpires;

	//DataLink层：
	HANDLE				hIfDl;
	FNPRONOTIFY			pFnDlNotify;
	FNPROQUERY			pFnDlQuery;
	FNDLGETMIB			pFnDlGetMIB;
	FNPROSET			pFnDlSet;
	FNDLOUT				pFnDlOut;
	FNPRONAME			pFnDlName;

	//操作信息
	WORD				wFlag;
	WORD				wXID;

	//接收的事件、线程、链表
	HANDLE				hEvtDataIn;
	HANDLE				hThrdDataIn;
	CRITICAL_SECTION	csListDataIn;
	LIST_UNIT			hListDataIn;
	//发送的事件、线程、链表
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
#define	IPIFF_ROUTE			0x0002	//可以转发数据包
#define	IPIFF_DHCP			0x0004	//可以DHCP功能

#include <ipdata.h>
/*
//-------------------------------------------------------------------------
// IP数据队列
//-------------------------------------------------------------------------
#define IPINF_NOAUTOFREE 0x1
typedef	struct	_IPDATA_IN
{
	LIST_UNIT			hListDataIn;
	//IP_INTF*			pNetIntf;

	WORD				wLenData;
	WORD				wFlag;		//暂时保留
	//VOID				*pData;	//指向非IP首部的地址(TCP/UDP...头)
	union
	{
		IP_HDR			HdrIP;
		BYTE			pBufIP[1];
	};

} IPDATA_IN;

typedef	struct	_IPDATA_OUT
{
	LIST_UNIT			hListDataOut;

	//目的地址
	DWORD				dwDesIP;
	WORD				wLenTotal;
	WORD				wFlag;		//暂时保留
	union
	{
		IP_HDR			HdrIP;
		BYTE			pBufIP[1];
	};

} IPDATA_OUT;
*/

//-------------------------------------------------------------------------
// IP 全局信息
//-------------------------------------------------------------------------
typedef	struct	_IP_GLOBAL
{
	HANDLE_THIS( _IP_GLOBAL );

	//所有IP接口的链表
	CRITICAL_SECTION	csListIntf;
	LIST_UNIT			hListIntf;

	//所有IP分片的链表
	CRITICAL_SECTION	csListFgNode;
	LIST_UNIT			hListFgNode;
	HANDLE				hEvtFgNode;
	HANDLE				hThrdFgNode;

	//操作信息
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
