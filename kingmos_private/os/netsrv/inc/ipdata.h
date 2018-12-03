/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	__IP_DATA_H
#define	__IP_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// IP���ݶ���
//-------------------------------------------------------------------------
#define IPINF_NOAUTOFREE 0x1
typedef	struct	_IPDATA_IN
{
	LIST_UNIT			hListDataIn;
	//IP_INTF*			pNetIntf;

	WORD				wLenData;
	WORD				reserver;		//��ʱ����
//	BYTE                bAutoFree;
	VOID				*pData;	//ָ���IP�ײ��ĵ�ַ(TCP/UDP...ͷ)
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

	WORD				wLenData;//�û����ݳ���
	WORD				reserver;		//��ʱ����
//	BYTE                bAutoFree;
	LPBYTE				pData;			//ָ���û�����
	struct	_TCPDATA_IN * pThis;    // check 
	TCP_HDR				*pHdrTcp;	//ָ���IP�ײ��ĵ�ַ(TCP/UDP...ͷ)
} TCPDATA_IN;

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

#ifdef __cplusplus
}	
#endif

#endif	//__IP_DATA_H
