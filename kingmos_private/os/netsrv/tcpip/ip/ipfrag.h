/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_IP_FRAG_H_
#define	_IP_FRAG_H_

#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------------------------
// 分片结点 的定义 和 操作
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

	//该结点所在的
	IP_INTF*			pNetIntf;

	//该结点的有序数据包
	IPFRAG_BUF*			pHeadBuf;
	//IPFRAG_BUF*			pTailBuf; //lilin remove
	
	//该结点的分片信息
	DWORD				dwTickLife;
	WORD				wOffset_remove;
	WORD				wEndFlag;//lilin add XXX;
	IP_HDR				HdrIP;		//首个IP头信息，由malloc分配的

} IPFRAG_NODE;


#ifdef __cplusplus
}	
#endif

#endif	//_IP_FRAG_H_
