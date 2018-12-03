/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _TCP_DEF_H_
#define _TCP_DEF_H_

#ifndef _RW_BUF_H_
#include "rwbuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif



//---------------------------------------------------
// TCB
//---------------------------------------------------

struct	_TCP_GLOBAL;
typedef	struct	_TCP_GLOBAL		TCP_GLOBAL;

//����ÿ�� ��TCP SOCKET�ԡ� �����в�����Ϣ
typedef	struct	_TCP_TCB
{
	HANDLE_THIS( _TCP_TCB );

	TCP_GLOBAL*			lpGlobalTcp;

	//TCB����
	LIST_UNIT			hListTcb;

	//Socket��
	DWORD				dwSrcIP;
	DWORD				dwDesIP;
	WORD				wSrcPort;
	WORD				wDesPort;

	//������Ϣ
	//
	CRITICAL_SECTION	csTcb;

	//TCP �� TCP Socket ������Ϣ
	WORD				wAttr;		//���Ա�־
	WORD				wFlag;		//������־
	DWORD				dwState;	//�����׶�
	DWORD				dwOption;	//����ѡ�_SOL_SOCKET����_BOOL���͵�����
	HANDLE				hEvtPoll;	//֪ͨ Block/Nonblock���¼�
										//Blockʱ��connect / accept / send / recv
										//Nonblockʱ��select
	SKERR				nErr;		//����
	DWORD				dwTickLife;	//�������ڵ���㣬����SYN_RECVD��TCB

	//accept����
	LIST_UNIT			hListSynRcvd;//��accept�����еĽ���TCB������û�йҵ�pInfoSocket��
	struct	_TCP_TCB*	pTcbParent;

	//����� ����socket
	WORD				wMaxConn;
	WORD				wCntConn;

	//�����̲߳���
	DWORD				dwTickRTT;
	DWORD				dwRTO;
	HANDLE				hEvtSend;
	HANDLE				hThrdSend;

	//���ͻ���
//
//���1-----
//
//         dwSnd_una                 dwSnd_nxt     dwSnd_max
//  |---------|-------------------------|--------------|------------|
//  | empty   | has sent but no ack     | wait to send |   empty    |
//  |---------|-------------------------|--------------|------------|
//            |----  wBytesSent  -------|-- wBytesTx --|
//            |----------------  dwCntTx  -------------|
//  0      dwReadTx                 dwStartTx      dwWriteTx
//
//
//���2-----
//
//dwSnd_max               dwSnd_una            dwSnd_nxt     
//  |--------------|---------|---------------------|----------------|
//  | wait to send | empty   | has sent but no ack | wait to send   |
//  |--------------|---------|---------------------|----------------|
//  |-- wBytesTx2--|         |----  wBytesSent  ---|-- wBytesTx1 ---|
//  |-- dwCntTx2 --|         |----------------  dwCntTx1  ----------|
//  0          dwWriteTx  dwReadTx              dwStartTx 
//
	RWBUFFER			stTxBufRW;
	DWORD				dwLenLeftTx;

	DWORD				dwSnd_una;	//���Ѿ����͵��Է�û��ACK�������ݵ� SEQ
	DWORD				dwSnd_nxt;	//�´η��͵����ݵ�λ�� SEQ
	DWORD				dwSnd_max;	//���Ҫ���͵����λ��
	DWORD				dwSnd_real;	//Ŀǰʵ�ʷ��͵��λ��

	//WORD				wSnd_wnd;	//�Լ���ǰ��Window size
	WORD				wXXX;
	WORD				wSnd_wnd_Last;	//�Լ���ǰ��Window size---���1�η��Ͱ�������Ϣ
	WORD				wSnd_MSS;	//�Լ���ǰ��MSS
	WORD				wSnd_quick;	//�����ش��ļ���

	//���ջ���
//	RWBUFFER			stRxBufRW;

	DWORD				dwRcv_nxt;	//�´ν��յ����ݵ�SEQ  ��  ÿ��Ҫ���͵�Ack
	WORD				wRcv_wnd;	//�Է�ͨ���Window size
	WORD				wRcv_MSS;	//�Է�ͨ���MSS

	//
	//���� ÿ�ν��ռ������Ϣ
	//
	LPBYTE				pTcpData;
	WORD				wLenHdr;
	WORD				wLenData;


	//���� ÿ�η�����Ϣ
	//
	DWORD				dwAck_Last;

	IPDATA_IN		    * pCurDataIn;//��ǰ�������
	LIST_UNIT			hCommitPacketDataIn;	//˳����δ�������ݰ�
	LIST_UNIT			hUnCommitPacketDataIn;	//δ�����������ݰ�
	DWORD               dwMaxRxBufSize;
	DWORD               dwUsedRxBufLen;
	CRITICAL_SECTION     csRx;
	CRITICAL_SECTION     csRxUnCommit;
	LONG                 lPackCount;
} TCP_TCB;

//dwLenRx
#define	LEN_RXBUF				4096
#define	MIN_RXBUF				2048
#define	MAX_RXBUF				0x10000
//wBytesTx
#define	LEN_TXBUF				4096
#define	MIN_TXBUF				2048
#define	MAX_TXBUF				0x10000

//wSnd_quick
#define	TCPQK_MAX				3

//dwRTO
#define	TCPTM_SEND_FIRST		(6*1000)
#define	TCPTM_SEND_MIN			(2*1000)
#define	TCPTM_SEND_MAX			(2*60*1000)  //(5*60*1000)

#define	TCPTM_CONNECT			(1+2*60*1000)	//connect�����ȴ���ʱ��
#define	TCPTM_WAITACCEPT		(60*1000)
#define	TCPTM_WAITBUSY			(3*1000)
#define	TCPTM_WAITSEND			(6*1000)
#define	TCPTM_SEND				(15*60*1000)
#define	TCPTM_WAITRECV			(5*1000)
#define	TCPTM_WAITSHUT			(60*1000)
#define	TCPTM_WAITPERSIST		(30*1000)


#define	TCPTM_SYNRCVD			(3*60*1000)

//dwState
#define	TCPS_CLOSED				0	//�رս���
#define	TCPS_LISTEN				1	//����
#define	TCPS_SYN_SENT			2	//����SYN
#define	TCPS_SYN_RECVD			3	//����SYN
#define	TCPS_ESTABLISHED		4	//����
#define	TCPS_CLOSE_WAIT			5	//�����ر�
#define	TCPS_LAST_ACK			6	//Ӧ��ر�
#define	TCPS_FIN_WAIT1			7	//�����ر�
#define	TCPS_FIN_WAIT2			8	//�����ر�
#define	TCPS_CLOSING			9	//ͬʱ�ر�
#define	TCPS_TIME_WAIT			10	//�ȴ��ر�
#define	TCPS_MAX				11

//wAttr---TCB���ԣ�һ��������ڵĻ���������������������֮�ڶ����ڣ�����ֻ�����´���TCB
#define	TCPA_CONNECT			0x0001	//
#define	TCPA_LISTEN				0x0002	//
//#define	TCPA_SYNRCVD			0x0004	//for debug
#define	TCPA_CLOSING			0x0008	//ͬʱ�ر�
#define	TCPA_ESTABLISH			0x0010	//

#define	TCPA_RECV				0x0040	// for shutdown
#define	TCPA_SEND				0x0080	// for shutdown
#define	TCPA_CONNRESET			0x0100	// for recv FIN/SYN
#define	TCPA_NETRESET			0x0200	// for keep-alive failure
#define	TCPA_HOST_UNREACH		0x0400	// for recv icmp... when sendmsg
#define	TCPA_CONN_ABORTED		0x0800	//

#define	TCPA_ERR_ALL			(TCPA_CONNRESET |TCPA_NETRESET |TCPA_HOST_UNREACH |TCPA_CONN_ABORTED)


//wFlag---���ݰ�
#define	TCPF_LSYN				0x0001	//�յ��Է���SYN's ACK--��ʾ�Լ����͵�SYN�Ѿ����ɹ�����
#define	TCPF_RSYN				0x0002	//�յ��Է���SYN
#define	TCPF_ALLSYN				(TCPF_LSYN | TCPF_RSYN)	//�����շ�SYN���ɹ�

#define	TCPF_LFIN				0x0004	//�յ��Է���FIN's ACK--��ʾ�Լ����͵�FIN�Ѿ����ɹ�����
#define	TCPF_RFIN				0x0008	//�յ��Է���FIN
#define	TCPF_ALLFIN				( TCPF_LFIN | TCPF_RFIN )	//�����շ�FIN���ɹ�

#define	TCPF_ALLFRM				(TCPF_ALLSYN | TCPF_ALLFIN)

//wFlag---��ѯ��־
#define	TCPQ_CNN_YES			0x0100
#define	TCPQ_CNN_NO				0x0200
#define	TCPQ_LISTEN_YES			0x0400
#define	TCPQ_LISTEN_NO			0x0800

//wFlag---���ͱ�־
#define	TCPF_EXIT				0x1000	//Ҫ���˳�
#define	TCPF_NEEDACK			0x2000	//Ҫ�� ���͡�����ʱ�ӵ�ACK��
//#define	TCPF_2MSL_ACK			0x4000	//��2MSL�ȴ�ʱ���ڣ�����ACK
#define	TCPF_SENDING			0x4000	//��2MSL�ȴ�ʱ���ڣ�����ACK
#define	TCPF_NEEDPERSIST		0x8000	//��Ҫʹ�ü�ֶ�ʱ��



//---------------------------------------------------
// Global
//---------------------------------------------------

//typedef	SKERR	(*FNTCPIN)(TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp);

//typedef	struct	_TCP_GLOBAL
struct	_TCP_GLOBAL
{
	HANDLE_THIS( _TCP_GLOBAL );

	//���� TCP SOCKETʵ��
	LIST_UNIT			hListTcb;
	CRITICAL_SECTION	csListTcb;

	//������Ϣ
	WORD				wFlag;		//��־
	WORD				wPort_Auto;	//�Զ��󶨵Ķ˿�
	DWORD				dwTickISN;	//ISN
	DWORD				dwISN;
	HANDLE				hEvtPersist;//Persist
	HANDLE				hThrdPersist;

	//����� ���������Ϣ
	TL_LOCAL			LocalTl;
	//TCP ����״̬���� ������
//	FNTCPIN				pFnTcpIn[ TCPS_MAX ];

} ;//TCP_GLOBAL;

extern	TCP_GLOBAL*		g_lpGlobalTcp;

//---------------------------------------------------
// ����
//---------------------------------------------------
extern	BOOL	Tcp_CreateTcb( TCP_TCB** ppTcbTcp, TCP_GLOBAL* lpGlobalTcp );
extern	void	Tcp_DeleteTCB( TCP_TCB* pTcbTcp );
extern	BOOL	Tcp_QueryMSS( DWORD dwSrcIP, DWORD dwDesIP, OUT WORD* pwMSS_me );
extern	WORD	Tcp_CalWndSize( TCP_TCB* pTcbTcp );
extern	DWORD	Tcp_CalISN( TCP_TCB* pTcbTcp );

extern	BOOL	TcpSk_Init( TCP_GLOBAL* lpGlobalTcp );
extern	BOOL	TcpIn_Init( TCP_GLOBAL* lpGlobalTcp );
extern	BOOL	TcpOut_StartSend( TCP_TCB* pTcbTcp );
extern	BOOL	TcpOut_StartPersist( TCP_GLOBAL* lpGlobalTcp );

extern	IPRET	TcpOut_SendRst( TCP_TCB* pTcbTcp, DWORD dwSrcIP, WORD wSrcPort, DWORD dwDesIP, WORD wDesPort, DWORD dwAckNum );
extern	IPRET	TcpOut_SendAck( TCP_TCB* pTcbTcp, DWORD dwSrcIP, WORD wSrcPort, DWORD dwDesIP, WORD wDesPort, BOOL fSetZeroWnd );

extern	void	Tcp_DbgIn( DWORD dwState, TCP_HDR* pHdrTcp, WORD wLenTotal, BOOL fIsHost );
extern	void	Tcp_DbgOut( DWORD dwState, TCP_HDR* pHdrTcp, WORD wLenTotal, BOOL fIsHost );
//dwOption
//#define	TCPDBG_HOSTNUM			0x0001
//#define	TCPDBG_HOSTPORT			0x0001

#ifdef __cplusplus
}	
#endif

#endif	//_TCP_DEF_H_

