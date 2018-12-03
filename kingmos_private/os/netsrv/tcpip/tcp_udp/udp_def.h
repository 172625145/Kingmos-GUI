/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _UDP_DEF_H_
#define _UDP_DEF_H_

#ifndef _RW_BUF_H_
#include "rwbuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------
//  ������ dwIP & wPort ��ͬ��ip���ݷ���ö���
//---------------------------------------------------
typedef	struct	_UDP_RXNODE
{
	HANDLE_THIS( _UDP_RXNODE );

	//RxNode����
	LIST_UNIT			hListRxNode;  //����UDP_TCB list

	//
	DWORD				dwTickRcv;

	//������Ϣ
	DWORD				dwIP;
	WORD				wPort;
	WORD				wXXX;
	//RWBUF2				stRx_RWBuf;//���ջ��� //lilin 

	LIST_UNIT			hIPList;  //�ܵ���IP FIFO ���У�IPDATA_IN* �ṹ��
	DWORD               dwAvailDataLen;// �ö��п������ݳ���
	DWORD               dwDataLenLimit;// �ö������ݳ�������


	//IPDATA_IN* pDataIn;  // lilin add;

} UDP_RXNODE;

//---------------------------------------------------
// TCB
//---------------------------------------------------
struct	_UDP_GLOBAL;
typedef	struct	_UDP_GLOBAL		UDP_GLOBAL;

typedef	struct	_UDP_TCB
{
	HANDLE_THIS( _UDP_TCB );

	UDP_GLOBAL*			lpGlobalUdp;

	//TCB����
	LIST_UNIT			hListTcb;

	//Socket��
	DWORD				dwSrcIP;
	DWORD				dwDesIP;
	WORD				wSrcPort;
	WORD				wDesPort;
	DWORD				dwSrcIP_Snd;

	//������Ϣ
	WORD				wAttr;		//���Ա�־
	WORD				wFlag;		//������־
	DWORD				dwOption;	//����ѡ�_SOL_SOCKET����_BOOL���͵�����
	HANDLE				hEvtPoll;	//֪ͨ Block/Nonblock���¼�
										//Blockʱ��connect / accept / send / recv
										//Nonblockʱ��select
	SKERR				nErr;		//����
	//LPBYTE				pUdpData;	//���� ÿ�ν��ռ������Ϣ
	//DWORD				dwLenData;

	//���ջ���
	//RWBUFFER			stRxBufRW;
	//RxNode����
	LIST_UNIT			hListRxNode;
	CRITICAL_SECTION	csListRxNode;
	DWORD				dwLenRcvBuf;


	//ֱ�ӹ����� IP��ӿ�
	HANDLE				hNetIntf;

} UDP_TCB;

//wFlag
#define	UDPF_EXIT				0x1000	//Ҫ���˳�

//dwLenRx
#define	LEN_RXBUF				4096
#define	MIN_RXBUF				2048
#define	MAX_RXBUF				0x10000

//
#define	UDPTM_WAITRECV			(5*1000)
#define	UDPTM_WAITRXNODE		(60*1000)


// ------------------------------------------------------
// UDP ȫ����Ϣ
// ------------------------------------------------------
//typedef	struct	_UDP_GLOBAL
struct	_UDP_GLOBAL
{
	HANDLE_THIS( _UDP_GLOBAL );

	//���� TCP SOCKETʵ��
	LIST_UNIT			hListTcb;
	CRITICAL_SECTION	csListTcb;

	//������Ϣ
	WORD				wFlag;		//��־
	WORD				wPort_Auto;	//�Զ��󶨵Ķ˿�

	//����� ���������Ϣ
	TL_LOCAL			LocalTl;

	//
	HANDLE				hEvtRxNode;
	HANDLE				hThrdRxNode;

} ;//UDP_GLOBAL;


extern	UDP_GLOBAL*		g_lpGlobalUdp;

extern	BOOL	Udp_CreateTcb( UDP_TCB** ppTcbUdp, UDP_GLOBAL* lpGlobalUdp );
extern	void	Udp_DeleteTCB( UDP_TCB* pTcbUdp );

extern	SKERR	Udp_Send( UDP_TCB* pTcbUdp, RXTXMSG* pMsgRxTx );

extern	BOOL	Udp_CreateRxNode( OUT UDP_RXNODE** ppUdpRxNode, UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort );
extern	void	Udp_DelRxNode( UDP_RXNODE* pUdpRxNode );
extern	void	Udp_CheckRxNode( UDP_TCB* pTcbUdp, BOOL fDelete );
extern	DWORD	Udp_GetRxFirstData( UDP_TCB* pTcbUdp, OUT LPBYTE pData, IN OUT DWORD* pdwLenData, OUT RXTXMSG* pMsgRxTx );
extern	BOOL	Udp_SetRxNodeLen( UDP_TCB* pTcbUdp, DWORD dwLenNew );

#ifdef __cplusplus
}	
#endif

#endif	//_UDP_DEF_H_
