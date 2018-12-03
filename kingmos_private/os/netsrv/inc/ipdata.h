/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	__IP_DATA_H
#define	__IP_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// IP数据队列
//-------------------------------------------------------------------------
#define IPINF_NOAUTOFREE 0x1
typedef	struct	_IPDATA_IN
{
	LIST_UNIT			hListDataIn;
	//IP_INTF*			pNetIntf;

	WORD				wLenData;
	WORD				reserver;		//暂时保留
//	BYTE                bAutoFree;
	VOID				*pData;	//指向非IP首部的地址(TCP/UDP...头)
	union
	{
		IP_HDR			HdrIP;
		BYTE			pBufIP[1];
	};

} IPDATA_IN;

VOID IPDataIn_Free( IPDATA_IN* pDataIn );

typedef	struct	_TCPDATA_IN
{
	LIST_UNIT			hListDataIn;

	WORD				wLenData;//用户数据长度
	WORD				reserver;		//暂时保留
//	BYTE                bAutoFree;
	LPBYTE				pData;			//指向用户数据
	struct	_TCPDATA_IN * pThis;    // check 
	TCP_HDR				*pHdrTcp;	//指向非IP首部的地址(TCP/UDP...头)
} TCPDATA_IN;

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

#ifdef __cplusplus
}	
#endif

#endif	//__IP_DATA_H
