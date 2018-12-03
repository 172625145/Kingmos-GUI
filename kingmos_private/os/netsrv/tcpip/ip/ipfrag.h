/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_IP_FRAG_H_
#define	_IP_FRAG_H_

#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------------------------
// ��Ƭ��� �Ķ��� �� ����
//-------------------------------------------------------------------------

typedef	struct	_IPFRAG_BUF
{
	PTR_NEXT( _IPFRAG_BUF );

	WORD			wLenData;
	//WORD			wXXX;
	WORD			wOffset;  //lilin change 
	BYTE			pData[1];

} IPFRAG_BUF;

typedef	struct	_IPFRAG_NODE
{
	HANDLE_THIS( _IPFRAG_NODE );

	LIST_UNIT			hListFgNode;

	//�ý�����ڵ�
	IP_INTF*			pNetIntf;

	//�ý����������ݰ�
	IPFRAG_BUF*			pHeadBuf;
	//IPFRAG_BUF*			pTailBuf; //lilin remove
	
	//�ý��ķ�Ƭ��Ϣ
	DWORD				dwTickLife;
	WORD				wOffset_remove;
	WORD				wEndFlag;//lilin add XXX;
	IP_HDR				HdrIP;		//�׸�IPͷ��Ϣ����malloc�����

} IPFRAG_NODE;


#ifdef __cplusplus
}	
#endif

#endif	//_IP_FRAG_H_
