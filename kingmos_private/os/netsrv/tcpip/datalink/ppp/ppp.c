/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����PPP
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-12-01
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <socket.h>
#include <gprscfg.h>
//"\inc_local"
#include "netif_dl_ras.h"
#include "netif_ip_dl.h"
//#include "iphlpapi_call.h"
//"local"
#include "ppp_def.h"
#include "lcp.h"


/***************  ȫ���� ���壬 ���� *****************/

//for debug
#if DEBUG_PPP_LOG
extern	void	PppDbg_Open();
extern	void	PppDbg_Write( PPPDBG_ITEM* pItemDbg, LPBYTE pBuf );
extern	void	PppDbg_Close();

static	long	g_nCntRx_IP = 0;
static	long	g_nCntRx_ppp = 0;
#endif

// ppp hdlc
extern	DWORD	PPP_InByHdlc( HANDLE hIfDev, LPBYTE pBufData, WORD wLen );
extern	DWORD	PPP_OutByHdlc( PPP_DEV* pIfDev, WORD wProtocol, FRAGSETD* pFragSetD );

// lcp / pap / chap / ipcp ---up and handle function
extern	void	LCP_SendCfgReq( PPP_DEV* pIfDev );
extern	void	LCP_Up( PPP_DEV* pIfDev );
extern	void	LCP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern	void	PAP_SendCfgReq( PPP_DEV* pIfDev );
extern	void	PAP_Up( PPP_DEV* pIfDev );
extern	void	PAP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern	void	CHAP_Up( PPP_DEV* pIfDev );
extern	void	CHAP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern	void	IPCP_SendCfgReq( PPP_DEV* pIfDev );
extern	void	IPCP_Up( PPP_DEV* pIfDev );
extern	void	IPCP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr );

//---------------------------------------------------
// PPP ����
//---------------------------------------------------
// 
#define	MAX_LCPCFG_TIMEOUT		680

PPP_GLOBAL*		g_lpGlobalPPP = NULL;

static	void	PPP_Free( );
static	void	PPP_FreeDev( PPP_DEV* pIfDev );
static	DWORD	PPP_GetMask( BYTE bIP );

static	void	PPP_SendCfg( PPP_DEV* pIfDev, BOOL fTerminate );
static	DWORD	WINAPI	PPP_ThrdCfg( LPVOID lpParameter );

//
static	void	PPP_DoUp( PPP_DEV* pIfDev );
static	void	PPP_DoStop( PPP_DEV* pIfDev );
static	void	PPP_DoClose( PPP_DEV* pIfDev );
static	void	PPP_StartIP( PPP_DEV* pIfDev );

//
static	DWORD	PPP_DlOut( HANDLE hIfDl, PARAM_DLOUT* pParamOut );
static	BOOL	PPP_Query( HANDLE hIfDev, OUT NETINFO_IPV4* pInfoNet );
static	BOOL	PPP_Set( HANDLE hIfDev, NETINFO_IPV4* pInfoNet );

/******************************************************/


//---------------------------------------------------
//PPP 
//---------------------------------------------------

// ********************************************************************
// ������
// ������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ����������PPP��ʼ��
// ����: 
// ********************************************************************
BOOL	PPP_Init( )
{
	//
	g_lpGlobalPPP = (PPP_GLOBAL*)HANDLE_ALLOC( sizeof(PPP_GLOBAL) );
	if( !g_lpGlobalPPP )
	{
		return FALSE;
	}
	HANDLE_INIT( g_lpGlobalPPP, sizeof(PPP_GLOBAL) );
	List_InitHead( &g_lpGlobalPPP->hListIfDev );
	InitializeCriticalSection( &g_lpGlobalPPP->csListIfDev );
	//

	return TRUE;
}

void	PPP_Deinit( )
{
}


// ********************************************************************
// ������
// ������
//	IN OUT pInfoLink---ָ����Datalink�� �� RAS/TSP�� �Ĺ�����Ϣ
//	IN fOverHdlc---ָ���Ƿ���RASֱ�ӵ��õ�
// ����ֵ��
//	����TRUE
// ������������� PPP�豸���ͻ�ȡ���PPP����Ϣ
// ����: 
// ********************************************************************
BOOL	PPP_CreateDev( PPP_LINK_RAS* pInfoLink, BOOL fOverHdlc )
{
	PPP_DEV*	pIfDev;
	HANDLE		hThrd;
	DWORD		dwThrdID;

	//
	RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: begin\r\n")));
	//����
	pIfDev = (PPP_DEV*)HANDLE_ALLOC( sizeof(PPP_DEV) );
	if( !pIfDev )
	{
		RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: fail000 to end\r\n")));
		return FALSE;
	}
	HANDLE_INIT( pIfDev, sizeof(PPP_DEV) );
	List_InitHead( &pIfDev->hListIfDev );

	//���� RAS��Ϣ
	pIfDev->hRasDev = pInfoLink->hRasDev;
	memcpy( &pIfDev->DialParam, pInfoLink->pDialParam, sizeof(DIALPARAM) );
	//���� TSP��Ϣ
	//pIfDev->hTspDev = pInfoLink->hTspDev;
	//pIfDev->pFnTspOut = pInfoLink->pFnTspOut;
	if( !pInfoLink->pInfoFromTsp->dwMRU )
	{
		pIfDev->wMRU = MRU_PPPRECV;
	}
	else if( pInfoLink->pInfoFromTsp->dwMRU<MRU_PPP_MIN )
	{
		pIfDev->wMRU = MRU_PPP_MIN;
	}
	else
	{
		pIfDev->wMRU = (WORD)pInfoLink->pInfoFromTsp->dwMRU;
	}
	if( pIfDev->wMRU < MRU_PPPRECV )
	{
		pIfDev->wMaxRecv = MRU_PPPRECV;
	}
	else
	{
		pIfDev->wMaxRecv = pIfDev->wMRU;
	}
#ifdef VC386
	//pIfDev->wMRU = 512; // for debug
#endif
	pIfDev->wMaxRecv += 20;
	//���� Over��Ϣ �� �����Ϣ
	if( fOverHdlc )
	{
		pInfoLink->LocalDl.pFnDlIn = PPP_InByHdlc;
	}
	else
	{
		pIfDev->hIfOver = pInfoLink->LocalDl.hIfDev;
		pIfDev->pFnOverNotify = pInfoLink->LocalDl.pFnDlNotify;		//���� PPP����DLʱ��֪ͨPPPOES
		pIfDev->pFnOverOut = pInfoLink->LocalDl.pFnDlOut;	 //���� PPPѡ����·����UNIMODEM ���� PPPOES

		pInfoLink->LocalDl.pFnDlIn = PPP_DlIn;
	}
	pInfoLink->LocalDl.hIfDev = (HANDLE)pIfDev;
	pInfoLink->LocalDl.pFnDlNotify = PPP_Notify;
	pInfoLink->LocalDl.pFnDlQuery = PPP_Query;
	pInfoLink->LocalDl.pFnDlSet = 0;

	//��ʼ��
	if( fOverHdlc )
	{
		DWORD	dwTmp;

		dwTmp = sizeof(PPP_HDLC)+pIfDev->wMaxRecv+10;
		pIfDev->pInfoHdlc = (PPP_HDLC*)malloc( dwTmp );
		if( !pIfDev->pInfoHdlc )
		{
			PPP_FreeDev( pIfDev );
			RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: fail111 to end\r\n")));
			return FALSE;
		}
		memset( pIfDev->pInfoHdlc, 0, sizeof(PPP_HDLC) );
	}
	//pIfDev->pInfoHdlc->wFCS = 0;
	pIfDev->wStateCnn = PPP_CNN_DIALED;	//PPP״̬ 
	pIfDev->bRunCfg = 1;

#if DEBUG_PPP_LOG
	PppDbg_Open();
	g_nCntRx_IP = 0;
	g_nCntRx_ppp = 0;
#endif

	//
	//���������߳�
	hThrd = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)PPP_ThrdCfg, (LPVOID)pIfDev, 0, &dwThrdID );
	if( !hThrd )
	{
#if DEBUG_PPP_LOG
		PppDbg_Close();
#endif
		PPP_FreeDev( pIfDev );
		RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: fail222 to end\r\n")));
		return FALSE;
	}
	pIfDev->hThrdCfg = hThrd;

	//��Ӹýӿ�
	EnterCriticalSection( &g_lpGlobalPPP->csListIfDev );
	List_InsertTail( &g_lpGlobalPPP->hListIfDev, &pIfDev->hListIfDev );
	LeaveCriticalSection( &g_lpGlobalPPP->csListIfDev );
	//
	RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: succeed to end\r\n")));
	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN hIfDev---ָ��PPP�豸
//	IN dwEvent---ָ��PPP�豸���¼�
// ����ֵ��
//	����0
// ������������������ PPP���¼�
// ����: 
// ********************************************************************
DWORD	PPP_Notify( HANDLE hIfDev, DWORD dwEvent )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pIfDev) )
	{
		return 0;
	}
	//PPP������ûỰ���� ��λ
	pIfDev->bCntRx_Nak_Rej = 0;
	pIfDev->bCntTx_ReCfg = 0;
	pIfDev->bCntRx_ReCfg = 0;
	//
	switch( dwEvent )
	{
	//����   RAS
	case PROEVT_DEV_LINKED:
		//PPP_DoUp( pIfDev );
		LCP_Up( pIfDev );
		break;
	//����   RAS ���� PPPOES
	case PROEVT_DEV_STOP:
		PPP_DoStop( pIfDev );
		break;
	//����   RAS ���� PPPOES
	case PROEVT_DEV_CLOSE:
		PPP_DoClose( pIfDev );
		break;
	
	//����   PPP�ڲ�---LCP
	case PPP_EVT_LCP_OK:
		RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_LCP_OK, RASERR_SUCCESS );
		if( pIfDev->wFlagLCP & PF_LCP_PAP )
		{
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_ap: PAP!\r\n")));
			PAP_Up( pIfDev );
		}
		else if( pIfDev->wFlagLCP & PF_LCP_CHAP )
		{
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n")));
			RETAILMSG(DEBUG_PPP,(TEXT(" ++++ netsrv_ap: CHAP!")));
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n")));
			CHAP_Up( pIfDev );
		}
		else
		{
			IPCP_Up( pIfDev );
		}
		break;
	//����   PPP�ڲ�---PAP/CHAP
	case PPP_EVT_AP_OK:
		RasIf_NotifyEvt( pIfDev->hIfNet, RASCS_IPCP, RASERR_SUCCESS );
		IPCP_Up( pIfDev );
		break;
	//����   PPP�ڲ�---IPCP
	case PPP_EVT_IPCP_OK:
		//�¼�֪ͨ
		RasIf_NotifyEvt( pIfDev->hIfNet, RASCS_IPCP_OK, RASERR_SUCCESS );
		//�жϷ��� IP��ַ��MASK
		pIfDev->dwIP_Mask = PPP_GetMask( GET_HBYTE3_L(pIfDev->dwIP_Clt) );

		if( !pIfDev->dwIP_Srv )
		{
			GprsCfg_GetPPG( GPRS_NETTYPE_CURRENT, &pIfDev->dwIP_Srv, NULL );
		}

		if( IN_LOOPBACK(pIfDev->dwIP_DNS) )
		{
			pIfDev->dwIP_DNS = pIfDev->dwIP_Srv;
		}
		if( IN_LOOPBACK(pIfDev->dwIP_DNS2) )
		{
			pIfDev->dwIP_DNS2 = pIfDev->dwIP_Srv;
		}
		if( DEBUG_PPP )
		{
			char	pAddr[32];

			RETAILMSG(DEBUG_PPP,(TEXT("\r\n"), pAddr));
			RETAILMSG(DEBUG_PPP,(TEXT(" ++++ netsrv_ap: IPCP_ok\r\n")));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_Srv, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: SrvIP[ %s ]\r\n"), pAddr));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_Clt, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: CltIP[ %s ]\r\n"), pAddr));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_Mask, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: CltIP_Mask[ %s ]\r\n"), pAddr));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_DNS, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: DnsIP[ %s ]\r\n"), pAddr));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_DNS2, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: Dns2IP[ %s ]\r\n"), pAddr));

			RETAILMSG(DEBUG_PPP,(TEXT("\r\n"), pAddr));
		}

		PPP_StartIP( pIfDev );
		break;

	//����   ����(IP)��
	case PROEVT_DL_UP:
		break;
	//����   ����(IP)��
	case PROEVT_DL_DN:
		break;
	}
	
	return 0;
}


// ********************************************************************
// ������
// ������
//	IN hIfDev---ָ��PPP�豸
//	IN OUT pInfoQuery---�����ѯ��������Ϣ
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// ������������ѯ������Ϣ
// ����: 
// ********************************************************************
BOOL	PPP_Query( HANDLE hIfDev, OUT NETINFO_IPV4* pInfoNet )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pIfDev) )
	{
		return FALSE;
	}
	//״̬���
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return FALSE;
	}

	//��ѯ������Ϣ: IP
	pInfoNet->dwIP         = pIfDev->dwIP_Clt;
	pInfoNet->dwIP_Mask    = pIfDev->dwIP_Mask;
	pInfoNet->dwIP_Gateway = pIfDev->dwIP_Srv;
	pInfoNet->dwIP_DNS     = pIfDev->dwIP_DNS;
	pInfoNet->dwIP_DNS2    = pIfDev->dwIP_DNS2;

	pInfoNet->dwEnableDhcp = FALSE;
	pInfoNet->wMTU = pIfDev->wMRU;

	return TRUE;
}

BOOL	PPP_GetMIB( HANDLE hIfDev, OUT PMIB_IFROW pIfRow )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pIfDev) )
	{
		return FALSE;
	}
	//״̬���
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return FALSE;
	}

	return FALSE;
}


// ********************************************************************
// ������
// ������
//	IN hIfDev---ָ��PPP�豸
//	IN pInfoQuery---ָ�����õ�������Ϣ
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// ��������������������Ϣ
// ����: 
// ********************************************************************
BOOL	PPP_Set( HANDLE hIfDev, NETINFO_IPV4* pInfoNet )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pIfDev) )
	{
		return FALSE;
	}
	//״̬���
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return FALSE;
	}

	//����������Ϣ: IP
	if( pInfoNet->dwMask & NET_MASK_IP_Gateway )
	{
		pIfDev->dwIP_Srv = pInfoNet->dwIP_Gateway;
	}

	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN hIfDev---ָ��PPP�豸
//	IN pParamOut---ָ�����͵���Ϣ
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// ������������������
// ����: 
// ********************************************************************
DWORD	PPP_DlOut( HANDLE hIfDl, PARAM_DLOUT* pParamOut )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDl;
	FRAGSETD*	pFragSetD = pParamOut->pFragSetD;

	WORD		wProtocol;

	//�����ȫ���
	if( !HANDLE_CHECK(pIfDev) )
	{
		return 1;
	}
	//������
	if( !pFragSetD || !pFragSetD->nCntAll || !pFragSetD->nCntUsed || !pFragSetD->dwBytes )
	{
		return ERROR_SUCCESS;
	}
	//״̬���
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return 1;
	}
	//
	pIfDev->dwFramesXmited ++;
	//�õ� Э����
	if( pParamOut->dwProtoID==PROID_IP )
	{
		wProtocol = PPP_PRO_IP;
	}
	else
	{
		switch( pParamOut->dwProtoID )
		{
		case PROID_LCP:
			wProtocol = PPP_PRO_LCP;
			break;
		case PROID_PAP:
			wProtocol = PPP_PRO_PAP;
			break;
		case PROID_CHAP:
			wProtocol = PPP_PRO_CHAP;
			break;
		case PROID_IPCP:
			wProtocol = PPP_PRO_IPCP;
			break;
		default:
			return 2;
			break;
		}
		//attention here
	}
	//CheckOutPtr( __FILE__, __LINE__ );
	//�������͵�·
	if( pIfDev->pFnOverOut )
	{
		//�� Э��ͷ
		pFragSetD->pFrag[pFragSetD->nCntUsed].pData = (LPBYTE)&wProtocol;
		pFragSetD->pFrag[pFragSetD->nCntUsed].nLen  = sizeof(WORD);
		pFragSetD->nCntUsed += 1;
		pFragSetD->dwBytes += sizeof(WORD);

		//���͸� pFnOverOut
		return (*pIfDev->pFnOverOut)( pIfDev->hIfOver, pParamOut );
	}
	else
	{
		//�� Э����
		//���͸� pFnTspOut
		return PPP_OutByHdlc( pIfDev, wProtocol, pFragSetD );
	}
}


// ********************************************************************
// ������
// ������
//	IN hIfDev---ָ��PPP�豸
//	IN wProtocol---ָ��PPP��Э����
//	IN pBuf---ָ�����͵�PPP����
//	IN wLen---ָ�����͵�PPP���ݳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// ����������PPP�ڲ� ���ͺ���
// ����: 
// ********************************************************************
#define	MAX_FRAGCNT			20
DWORD	PPP_OutBuf( PPP_DEV* pIfDev, WORD wProtocol, LPBYTE pBuf, WORD wLen )
{
	//
	pIfDev->dwFramesXmited ++;
	//
	if( pIfDev->pFnOverOut )
	{
		PARAM_DLOUT	ParamOut;
		BYTE		pBufFragSet[sizeof(FRAGSETD) + MAX_FRAGCNT*sizeof(FRAGMENT)];
		FRAGSETD*	pFragSetD = (FRAGSETD*)pBufFragSet;
		FRAGMENT*	pFrag = pFragSetD->pFrag;
		
		//�� Э���Ƭ
		pFragSetD->nCntAll = MAX_FRAGCNT;
		pFragSetD->nCntUsed = 2;
		pFragSetD->dwBytes = wLen + sizeof(WORD);
		pFrag[0].pData = pBuf;
		pFrag[0].nLen  = (LONG)wLen;
		pFrag[1].pData = (LPBYTE)&wProtocol;
		pFrag[1].nLen  = sizeof(WORD);

		memset( &ParamOut, 0, sizeof(PARAM_DLOUT) );
		ParamOut.dwProtoID = PROID_PPP;
		ParamOut.pFragSetD = pFragSetD;
		//���͸� pFnOverOut
		return (*pIfDev->pFnOverOut)( pIfDev->hIfOver, &ParamOut );
	}
	else
	{
		FRAGSETD		FragSetD;

		//�� Э����
		FragSetD.nCntAll = 1;
		FragSetD.nCntUsed = 1;
		FragSetD.dwBytes = wLen;

		FragSetD.pFrag[0].pData = pBuf;
		FragSetD.pFrag[0].nLen  = (LONG)wLen;
		//���͸� pFnTspOut
		return PPP_OutByHdlc( pIfDev, wProtocol, &FragSetD );
	}
}

//------------------------------------------------------
//���ܣ����� LCP��PAP��CHAP��IPCP�� ���ݰ�����Ϊ���Ƕ�����ͬ�� Header---CTL_HDR
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//wProtocol---ָ��Э��
//bCode/bAskerID/wLenTotal---ָ�����ǵ�CTL_HDR����ֵ
//pBuf---ָ��Ҫ���͵�BUFFER��������CTL_HDR��������
//------------------------------------------------------
void	PPP_SendPro( PPP_DEV* pIfDev, WORD wProtocol, BYTE bCode, BYTE bAskerID, WORD wLenTotal, IN OUT LPBYTE pBuf )
{
	// ��ʼ�� Header
	pBuf[0] = bCode;
	pBuf[1] = bAskerID;
	phtonsp( pBuf+2, (LPBYTE)&wLenTotal );
	// ����
	PPP_OutBuf( pIfDev, wProtocol, pBuf, wLenTotal );
}


// ********************************************************************
// ������
// ������
//	IN hIfDev---ָ��PPP�豸
//	IN pFrame---ָ�����յ�PPP��PROTOCOL����
//	IN dwLen---ָ�����յ�PPP��PROTOCOL���ݵĳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// ��������������PPP��PROTOCOL����
// ����: 
// ********************************************************************
DWORD	PPP_DlIn( HANDLE hIfDev, LPBYTE pFrmBuf, WORD wLen )
{
#if DEBUG_PPP_LOG
	PPPDBG_ITEM	ItemDbg;
#endif
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;
	LPBYTE		pFrame = pFrmBuf;
	WORD		wProtocol;

	//�����ȫ���
	if( !HANDLE_CHECK(pIfDev) )
	{
		return 1;
	}
	//״̬���
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return 1;
	}
	//
	pIfDev->dwFramesRcved ++;

	// ProtoID
	if( *pFrame & 1 )	//˵�� ���Ѿ���ѹ������ProtoID
	{
		wProtocol = TO_NBYTE1_S( *pFrame );
		pFrame += 1;
		wLen   -= 1;
	}
	else
	{
		memcpy( &wProtocol, pFrame, sizeof(WORD) );
		pFrame += 2;
		wLen   -= 2;
	}

	//for debug
#if DEBUG_PPP_LOG
	ItemDbg.wFlag = PDBGF_DIRIN;
	ItemDbg.wXXX = 0;
	ItemDbg.wProtocol = wProtocol;
	ItemDbg.wLen = wLen;
	ItemDbg.dwTick = GetTickCount();
	PppDbg_Write( &ItemDbg, pFrame );
#endif

//#if DEBUG_PPP
//	if( wProtocol==PPP_PRO_IP )
//	{
//		RETAILMSG(DEBUG_PPP,(TEXT(" \r\nPPP_DlIn_IP: g_nCntRx_IP=[%d], g_nCntRx_ppp=[%d], wLen=[%d]\r\n"), ++g_nCntRx_IP, g_nCntRx_ppp, wLen));
//	}
//	else
//	{
//		RETAILMSG(DEBUG_PPP,(TEXT(" \r\nPPP_DlIn_ppp: g_nCntRx_IP=[%d], g_nCntRx_ppp=[%d], wLen=[%d]\r\n"), g_nCntRx_IP, ++g_nCntRx_ppp, wLen));
//	}
//#endif

	// ����Э�鴦��
	switch( wProtocol )
	{
	case PPP_PRO_IP	:
		if( pIfDev->pFnNetIn )
		{
			(*pIfDev->pFnNetIn)( pIfDev->hIfNet, pFrame, wLen );
		}
		break;

	case PPP_PRO_LCP:
		LCP_Handle( pIfDev, pFrame, wLen );
		break;
	case PPP_PRO_PAP:
		PAP_Handle( pIfDev, pFrame, wLen );
		break;
	case PPP_PRO_CHAP:
		CHAP_Handle( pIfDev, pFrame, wLen );
		break;
	case PPP_PRO_IPCP:
		IPCP_Handle( pIfDev, pFrame, wLen );
		break;

	default:
		//XYGע�ͣ���������ݰ���֧�֣�ֱ�Ӷ�����
		//LCP_SendPro( pIfDev, LCP_CODE_REJ_PRO, 33, wLen, pFrame );
		RETAILMSG(DEBUG_PPP,(TEXT(" \r\n PPP_DlIn: unknown=[0x%x], wLen=[%d]\r\n"), REORDER_S(wProtocol), wLen));
		break;
	}

	return 0;
}






//------------------------------------------------------
// PPP �ڲ���������..........................
//------------------------------------------------------


// ********************************************************************
// ������
// ������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ����������PPP��ʼ��
// ����: 
// ********************************************************************
void	PPP_Free( )
{
	//֪ͨ���е��豸
	//????????????????????????????????

	//
	//
	DeleteCriticalSection( &g_lpGlobalPPP->csListIfDev );
	//
	free( g_lpGlobalPPP );
	g_lpGlobalPPP = NULL;
}


void	PPP_FreeDev( PPP_DEV* pIfDev )
{
	//�Ͽ�
	//EnterCriticalSection( &g_lpGlobalPPP->csListIfDev );
	//List_RemoveUnit( &pIfDev->hListIfDev );
	//LeaveCriticalSection( &g_lpGlobalPPP->csListIfDev );

	pIfDev->bRunCfg = 0;
	Sleep( 1 );

	//
	if( pIfDev->pInfoHdlc )
	{
		free( pIfDev->pInfoHdlc );
		pIfDev->pInfoHdlc = NULL;
	}
	if( pIfDev->hThrdCfg )
	{
		Sleep( MAX_LCPCFG_TIMEOUT );
		CloseHandle( pIfDev->hThrdCfg );
	}

	//
	free( pIfDev );
}

DWORD	PPP_GetMask( BYTE bIP )
{
	if( bIP<128 )
	{
		return ntohl_m(0xFF000000);
	}
	else if( bIP<192 )
	{
		return ntohl_m(0xFFFF0000);
	}
	else if( bIP<224 )
	{
		return ntohl_m(0xFFFFFF00);
	}
	else
	{
		return ntohl_m(0xFFFFFFFF);
	}
}

void	PPP_DoUp( PPP_DEV* pIfDev )
{
	LCP_Up( pIfDev );
}

void	PPP_DoStop( PPP_DEV* pIfDev )
{
	//֪ͨ�Է�
	if( pIfDev->bRunCfg )
	{
		int		i;
		BYTE	bBuf[sizeof(CTL_HDR)];//�Ƿ���� MN�����Է���һ��
	
		for( i=0; i<3; i++ )
		{
			LCP_SendPro( pIfDev, CTL_CODE_TER_REQ, 0, sizeof(CTL_HDR), bBuf );
		}
		pIfDev->wStateCnn = PPP_CNN_TERMINATE;
		
		pIfDev->bRunCfg = 0;
	}

	//PPPͣ�ø��豸�ӿ�
	pIfDev->wStateCnn = PPP_CNN_DOWN;
	if( pIfDev->hThrdCfg )
	{
		WaitForSingleObject( pIfDev->hThrdCfg, 4000 );
		CloseHandle( pIfDev->hThrdCfg );
		pIfDev->hThrdCfg = NULL;
	}

	//�ж��Ƿ���IP��ע��ӿ�
	if( pIfDev->hIfNet )
	{
		//ȥ�� ��IP��ע��Ľӿ�
		(*pIfDev->pFnNetNotify)( pIfDev->hIfNet, PROEVT_DL_STOP );
	}
}

void	PPP_DoClose( PPP_DEV* pIfDev )
{
	//�ж��Ƿ���IP��ע��ӿ�
	if( pIfDev->hIfNet )
	{
		//ȥ�� ��IP��ע��Ľӿ�
		(*pIfDev->pFnNetNotify)( pIfDev->hIfNet, PROEVT_DL_DN );
		pIfDev->hIfNet = NULL;
	}

#if DEBUG_PPP_LOG
	PppDbg_Close();
#endif

	//��PPP��ɾ����ǰ����ӿ�
	EnterCriticalSection( &g_lpGlobalPPP->csListIfDev );
	List_RemoveUnit( &pIfDev->hListIfDev );
	LeaveCriticalSection( &g_lpGlobalPPP->csListIfDev );

	//
	HANDLE_FREE( pIfDev );
}

void	PPP_StartIP( PPP_DEV* pIfDev )
{
	NET_LINK_DL		InfoLink;

	//��� IP������ӿ�
	memset( &InfoLink, 0, sizeof(NET_LINK_DL) );
	InfoLink.LocalDl.hIfDev = (HANDLE)pIfDev;
	InfoLink.LocalDl.pFnDlNotify = PPP_Notify;
	InfoLink.LocalDl.pFnDlQuery = PPP_Query;
	InfoLink.LocalDl.pFnDlGetMIB = PPP_GetMIB;
	InfoLink.LocalDl.pFnDlSet = PPP_Set;
	InfoLink.LocalDl.pFnDlOut = PPP_DlOut;
	InfoLink.LocalDl.pFnDlIn = 0;
	if( !IP_CreateIf( &InfoLink ) )
	{
		//֪ͨ RAS
		RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_Disconnected, 0 );
		return ;
	}
	//���
	//����
	pIfDev->hIfNet = InfoLink.LocalNet.hIfDev;
	pIfDev->pFnNetNotify = InfoLink.LocalNet.pFnNetNotify;
	pIfDev->pFnNetIn = InfoLink.LocalNet.pFnNetIn;

	//����״̬
	pIfDev->wStateCnn = PPP_CNN_IP;

	//֪ͨ
	(*pIfDev->pFnNetNotify)( pIfDev->hIfNet, PROEVT_DL_UP );

	//֪ͨ RAS
	RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_Connected, RASERR_SUCCESS );
}




//�����ط�����
DWORD	WINAPI	PPP_ThrdCfg( LPVOID lpParameter )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)lpParameter;
	BOOL		fTerminate;

	//û�е�IP�㣬����û��DOWN
	while( 1 )
	{
		if( (!pIfDev->bRunCfg) || (pIfDev->wStateCnn==PPP_CNN_IP) || (pIfDev->wStateCnn==PPP_CNN_DOWN) )
		{
			break;
		}
		Sleep( MAX_LCPCFG_TIMEOUT );

		//
		// Ҫ���˳����ҷ������������õĴ������ࡢ�ҷ������������õĴ������ࡢ��NAK��REJ�Ĵ�������
		if( (pIfDev->bCntTx_ReCfg>MAX_TX_RE_CFG) || (pIfDev->bCntRx_ReCfg>MAX_TX_RE_CFG) || (pIfDev->bCntRx_Nak_Rej>MAX_RX_NAK_REJ) )
		{
			RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_PPP, RASERR_PPP_CFG );
			return 0;
		}
		//���Է�����������
		if( (GetTickCount()-pIfDev->dwTickCfg) > WAII_TX_CFG )
		{
			//���Է���Terminate, Ҫ����������
			if( pIfDev->bCntTx_Cfg>MAX_RE_TX_CFG )
			{
				pIfDev->bCntTx_Cfg = 0;
				fTerminate = TRUE;
			}
			else
			{
				fTerminate = FALSE;
			}
			PPP_SendCfg( pIfDev, fTerminate );
		}
	}

	return 0;
}

void	PPP_SendCfg( PPP_DEV* pIfDev, BOOL fTerminate )
{
	switch( pIfDev->wStateCnn )
	{
	case PPP_CNN_LCP:
		if( fTerminate )
		{
			BYTE	bBuf[sizeof(CTL_HDR)];//�Ƿ���� MN�����Է���һ��
			pIfDev->bCntTx_ReCfg ++;
			LCP_SendPro( pIfDev, CTL_CODE_TER_REQ, (++pIfDev->bAckerID), sizeof(CTL_HDR), bBuf );
			RETAILMSG(DEBUG_PPP,(TEXT("  LCP_SendPro: send [CTL_CODE_TER_REQ]\r\n")));
		}
		else if( !(pIfDev->wFlagCnn & LCP_CNN_RXACK) )
		{
			LCP_SendCfgReq( pIfDev );
		}
		break;

	case PPP_CNN_PAP:
		if( fTerminate )
		{
			BYTE	bBuf[sizeof(CTL_HDR)];//�Ƿ���� MN�����Է���һ��
			pIfDev->bCntTx_ReCfg ++;
			LCP_SendPro( pIfDev, CTL_CODE_TER_REQ, (++pIfDev->bAckerID), sizeof(CTL_HDR), bBuf );
			RETAILMSG(DEBUG_PPP,(TEXT("  LCP_SendPro: send [CTL_CODE_TER_REQ]\r\n")));
		}
		else if( !(pIfDev->wFlagCnn & AP_CNN_RXACK) )
		{
			PAP_SendCfgReq( pIfDev );
		}
		break;

	case PPP_CNN_CHAP:
		//attention
		//if( !(pIfDev->wFlagCnn & AP_CNN_RXACK) )
		//{
		//	CHAP_SendCfgReq( pIfDev );
		//}
		break;
	case PPP_CNN_IPCP:
		if( fTerminate )
		{
			BYTE	bBuf[sizeof(CTL_HDR)];//�Ƿ���� MN�����Է���һ��
			pIfDev->bCntTx_ReCfg ++;
			IPCP_SendPro( pIfDev, CTL_CODE_TER_REQ, (++pIfDev->bAckerID), sizeof(CTL_HDR), bBuf );
			RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_SendPro: send [CTL_CODE_TER_REQ]\r\n")));
		}
		else if( !(pIfDev->wFlagCnn & IPCP_CNN_RXACK) )
		{
			IPCP_SendCfgReq( pIfDev );
		}
		break;
	}
}


//---------------------------------------------------
//PPP ����Ͽ� ����
//---------------------------------------------------
//

//����Ͽ������ʹ���
//1�����Է�֪ͨ���յ�Terminate���Ҳ���IP�㽨��ʱ��---�����Ͽ�
     //1> PPP֪ͨ RAS APP��������Ͽ�������Ϣ
     //2> ��PPP������ ��ȫ�Ͽ��Ĳ���

//2�����õĴ������࣬Ҫ�������˳�---���ñ��ܾ���PPP�����Ͽ�
     //1> PPP֪ͨ RAS APP��������Ͽ�������Ϣ
     //2> ����״̬�����ڸýӿڲ����ٽ����κ�����(�ӿڳ��֣�ֻ�ܷ��ͣ����ٽ�������)
     //3> ����Terminate 3��֪ͨ�Է�
     //4> ��PPP������ ��ȫ�Ͽ��Ĳ���

//3���豸�Ѿ����Ͽ������ߡ�û��Ӧ���豸�رգ���֪ͨPPP---�豸�Ͽ�
     //1> TSP֪ͨ RAS APP��������Ͽ�������Ϣ
     //2> ��RAS APP������ �Ͽ�����---attention
     //2> ���� ��ȫ�Ͽ��Ĳ���

//4���Ѿ����ӳɹ����û�Ҫ��Ͽ�---�û��Ͽ�
     //0> ��RAS APP������ �Ͽ�����---attention
     //1> ����״̬�����ڸýӿڲ����ٽ����κ�����(�ӿڳ��֣�ֻ�ܷ��ͣ����ٽ�������)
     //2> ����Terminate 3��֪ͨ�Է�
     //3> ���� ��ȫ�Ͽ��Ĳ���

//��ȫ�Ͽ��Ĳ���
     //1> ����״̬��ֹͣ���� �ýӿ�pIfDev�����ж���
     //2> ֪ͨPPP��ȡ�����ͷŹ��� �ýӿ�pIfDev�����ж����������Ƿ����IP��Ľӿ�
     //3> ֪ͨPPP�ӿڲ㣬�رղ����豸
     //4> �ر�PPP�ӿڲ�

//ע�⣺�Ͽ��Ĳ�����ֻ���� RAS ���в���

