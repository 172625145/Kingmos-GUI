/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _PPP_DEF_H_
#define _PPP_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

//
#define DEBUG_PPP 1
//#define DEBUG_PPP_LOG 1

//---------------------------------------------------
//PPP HDLC�Ķ���
//---------------------------------------------------

typedef	struct	_PPP_HDLC
{
	//�շ�����
	WORD				wStateRecv;
	WORD				wFCS;
	WORD				wLenRecv;
	WORD				wXXX;

	BYTE				pFrmRecv[1];

} PPP_HDLC;

//wStateRecv
#define	FRM_RECV_START			0x01
#define	FRM_RECV_ESC			0x02


//---------------------------------------------------
//PPP �豸�Ķ���
//---------------------------------------------------

typedef	struct	_PPP_DEV
{
	HANDLE_THIS( _PPP_DEV );

	//PPP�豸����
	LIST_UNIT			hListIfDev;
	
	//����㣺
	HANDLE				hIfNet;
	FNPRONOTIFY			pFnNetNotify;
	FNPROIN				pFnNetIn;
	//Over�㣺
	HANDLE				hIfOver;
	FNPRONOTIFY			pFnOverNotify;
	FNDLOUT				pFnOverOut;
	//RAS�㣺
	HANDLE				hRasDev;	//RAS�豸���
	DIALPARAM			DialParam;	//���Ų���
	//TSP�㣺
	//HANDLE			hTspDev;	//TSP�豸���
	//FNTSPOUT			pFnTspOut;	//TSP���ͺ���
	//WORD				wMRU_Self;	//�Լ��� ���MRU����TSP��ȡ����
	//WORD				wMRU_Other;	//�Է��� ���MRU����LCP�л��
	WORD				wMRU;
	WORD				wMaxRecv;
	//HDLC��Ľ�����Ϣ��
	PPP_HDLC*			pInfoHdlc;

	// PPP ������Ϣ
	// PPP �Ĺ��� �� �ù��̵�״̬��־
	WORD				wStateCnn;		//PPP�Ĺ���
	WORD				wFlagCnn;		//���ӵı�־
	// ���õĶ�ʱ��� �� �������û���
	HANDLE				hThrdCfg;
	DWORD				dwTickCfg;		//
	
	WORD				wCfgReqOpt;		//ÿ�����̵� ����ѡ��

	BYTE				bAckerID;		//ÿ�����̵� �������ݺ�����ID�ţ��Ա�Ƚ�ACK
	BYTE				bRunCfg;		//�����̵߳����б�־
	BYTE				bCntTx_Cfg;		//�ط�����
	BYTE				bCntRx_Nak_Rej;	//�յ�Nak/Rej����
	BYTE				bCntTx_ReCfg;	//�ҷ�Ҫ���������õĴ���
	BYTE				bCntRx_ReCfg;	//�Է�Ҫ���������õĴ���

	// �񵽵� --- LCP ��Ϣ
	DWORD				dwMN;			//�Լ���
	DWORD				dwMN_Other;		//�Է��� MN����
	DWORD				dwACCM_Other;	//�Է��� ACCM����
	WORD				wFlagLCP;		//�Է��� LCP��������
	WORD				wXXX;
	// �񵽵� --- IPCP ��Ϣ
	DWORD				dwIP_Clt;		//���ص�
	DWORD				dwIP_Srv;		//SRV IP
	DWORD				dwIP_DNS;		//DNS IP
	DWORD				dwIP_DNS2;		//DNS2 IP
	DWORD				dwIP_Mask;		//���ص�

	//
	//ͳ����Ϣ
	//
	//
	DWORD				dwFramesXmited;
	DWORD				dwFramesRcved;
	DWORD				dwCrcErr;
	//DWORD				dwTimeoutErr;
	//DWORD				dwAlignmentErr;
	//DWORD				dwHardwareOverrunErr;
	//DWORD				dwFramingErr;
	//DWORD				dwBufferOverrunErr;

} PPP_DEV;

//wMRU_Self/wMRU_Other's max value
#define	MRU_PPPRECV				1500
#define	MRU_PPP_MIN				32		//

// wStateCnn
#define	PPP_CNN_DIALED			0x0000
#define	PPP_CNN_LCP				0x0002
#define	PPP_CNN_PAP				0x0004
#define	PPP_CNN_CHAP			0x0005
#define	PPP_CNN_IPCP			0x0010
#define	PPP_CNN_IP				0x0050

#define	PPP_CNN_TERMINATE		0x2000
#define	PPP_CNN_DOWN			0x1000

// wFlagCnn
#define	LCP_CNN_RXACK			0x0001
#define	LCP_CNN_TXACK			0x0002
#define	AP_CNN_RXACK			0x0004
#define	IPCP_CNN_RXACK			0x0001
#define	IPCP_CNN_TXACK			0x0002
#define	DOWN_CNN_TERMINATE		0x1000

#define	PPP_TEST_ACK(wF,n)		( (wF & n)==n )

// wFlagLCP
#define	PF_LCP_PFC				0x0001
#define	PF_LCP_ACFC				0x0002
#define	PF_LCP_PAP				0x0004
#define	PF_LCP_CHAP				0x0008

//dwACCM_Other
#define	LCP_ACCM_DEFAULT		0xFFFFFFFF

//bCntTx_Cfg, bCntRx_Nak_Rej, bCntTx_ReCfg, bCntRx_ReCfg
#define	MAX_RE_TX_CFG			10
#define	MAX_RX_NAK_REJ			20
#define	MAX_TX_RE_CFG			3
#define	MAX_RX_RE_CFG			3
#define	WAII_TX_CFG				5500

//�� PPPoE ʱ��LCP: 
//  RECOMMENDED:     MN, MRU<=1492
//  NOT RECOMMENDED: PFC, 
//  MUST NOT:        FCS, ACFC, ACCM


//---------------------------------------------------
//PPP ����
//---------------------------------------------------

typedef	struct	_PPPDBG_ITEM
{
	//
	WORD		wFlag;
	WORD		wXXX;

	WORD		wProtocol;
	WORD		wLen;

	DWORD		dwPosSav;
	DWORD		dwTick;

} PPPDBG_ITEM;

//
#define	PDBGF_DIRIN			0x00000000
#define	PDBGF_DIROUT		0x00000001


//---------------------------------------------------
//PPP ȫ����Ϣ
//---------------------------------------------------
typedef	struct	_PPP_GLOBAL
{
	HANDLE_THIS( _PPP_GLOBAL );

	//
	LIST_UNIT			hListIfDev;	//����
	CRITICAL_SECTION	csListIfDev;

	//for debug
#ifdef DEBUG_PPP
	HANDLE				hDbgItem;
	HANDLE				hDbgBuf;
	DWORD				dwPosItem;
	DWORD				dwPosBuf;
#endif

} PPP_GLOBAL;

extern	PPP_GLOBAL*		g_lpGlobalPPP;

// PPP Event
#define	PPP_EVT_LCP_OK			0x0010
#define	PPP_EVT_AP_OK			0x0020
#define	PPP_EVT_IPCP_OK			0x0030


//---------------------------------------------------
// PPP---RFC 1661
//---------------------------------------------------
//
// ppp frame :
//
// +----------+----------+----------+
// |   Flag   | Address  | Control  |
// | 01111110 | 11111111 | 00000011 |
// +----------+----------+----------+
// +----------+-------------+---------+
// | Protocol | Information | Padding |
// | 8/16 bits|      *      |    *    |
// +----------+-------------+---------+
// +----------+----------+-----------------
// |   FCS    |   Flag   | Inter-frame Fill
// |16/32 bits| 01111110 | or next Address
// +----------+----------+-----------------
// wProtocol(or wProtoID) of PPP frame---�ص㣺MSB��ż����LSB����������MSB��0������ѹ��
#define	PPP_PRO_IP				REORDER_S(0x0021)//
#define PPP_PRO_LCP				REORDER_S(0xc021)	//Link Control Protocol
#define PPP_PRO_IPCP			REORDER_S(0x8021)	//IP control protocol
#define PPP_PRO_PAP				REORDER_S(0xc023)	//Password Authentication Protocol
#define PPP_PRO_LQR				REORDER_S(0xc025)	//Link Quality Report
#define PPP_PRO_CHAP			REORDER_S(0xc223)	//Challenge Handshake Authentication Protocol
#define PPP_PRO_IPCCP			REORDER_S(0x80fd)	//IP Compression Control Protocol.

//
//Every PPP packet: AC(2bytes) + Protocol(2bytes) + the following:
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Code      |  Identifier   |            Length             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |    Data ...
//   +-+-+-+-+
typedef	struct	_CTL_HDR
{
	BYTE	bCode;
	BYTE	bAskerID;
	WORD	wLenTotal;

} CTL_HDR;

// bCode
#define	CTL_CODE_CFG_REQ			1  //     Configure-Reque
#define	CTL_CODE_CFG_ACK			2  //     Configure-Ack
#define	CTL_CODE_CFG_NAK			3  //     Configure-Nak
#define	CTL_CODE_CFG_REJ			4  //     Configure-Rejec
#define	CTL_CODE_TER_REQ			5  //     Terminate-Requet
#define	CTL_CODE_TER_ACK			6  //     Terminate-Ackst
#define	CTL_CODE_REJ_CODE			7  //     Code-Reject


//---------------------------------------------------
//PPP ����
//---------------------------------------------------


//�ڲ�
extern	DWORD	PPP_Notify( HANDLE hIfDev, DWORD dwEvent );
extern	DWORD	PPP_DlIn( HANDLE hIfDev, LPBYTE pFrmBuf, WORD wLen );

extern	DWORD	PPP_OutBuf( PPP_DEV* pIfDev, WORD wProtocol, LPBYTE pBuf, WORD wLen );
extern	void	PPP_SendPro( PPP_DEV* pIfDev, WORD wProtocol, BYTE bCode, BYTE bAskerID, WORD wLenTotal, LPBYTE pBuf );

#define	LCP_SendPro( pIfDev, bCode, bAskerID, wLenTotal, pBuf )	PPP_SendPro( pIfDev, PPP_PRO_LCP, bCode, bAskerID, wLenTotal, pBuf )
#define	PAP_SendPro( pIfDev, bCode, bAskerID, wLenTotal, pBuf )	PPP_SendPro( pIfDev, PPP_PRO_PAP, bCode, bAskerID, wLenTotal, pBuf )
#define	CHAP_SendPro( pIfDev, bCode, bAskerID, wLenTotal, pBuf )	PPP_SendPro( pIfDev, PPP_PRO_CHAP, bCode, bAskerID, wLenTotal, pBuf )
#define	IPCP_SendPro( pIfDev, bCode, bAskerID, wLenTotal, pBuf )	PPP_SendPro( pIfDev, PPP_PRO_IPCP, bCode, bAskerID, wLenTotal, pBuf )

//


#ifdef __cplusplus
}	
#endif

#endif	//_PPP_DEF_H_
