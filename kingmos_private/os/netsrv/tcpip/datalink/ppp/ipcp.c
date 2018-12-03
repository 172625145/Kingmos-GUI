/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����IPCP
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
//"\inc_local"
#include "encode.h"
#include "netif_dl_ras.h"
//"local"
#include "ppp_def.h"
#include "ipcp.h"

/***************  ȫ���� ���壬 ���� *****************/
#define	IPCP_TLD_LEN_ALL		( sizeof(CTL_HDR)+ 3*IPCP_TLD_LEN_IP_ADDR )
#define	IPCP_TLD_TYPE_UP		(IPCP_OPT_IP_ADDR | IPCP_OPT_P_DNS_ADDR | IPCP_OPT_S_DNS_ADDR )

static	void	IPCP_Handle_Echo_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	IPCP_Handle_Cfg_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	IPCP_Handle_Cfg_Ack( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	IPCP_Handle_Cfg_Nak( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	IPCP_Handle_Cfg_Rej( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	IPCP_Handle_Ter_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr );
/******************************************************/

//------------------------------------------------------
//���ܣ�������������
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//------------------------------------------------------
void	IPCP_SendCfgReq( PPP_DEV* pIfDev )
{
	LPBYTE		pBuf;
	LPBYTE		pCopy;
	WORD		wCfgReqOpt;
	WORD		wTmp;

	pBuf = (LPBYTE)malloc( IPCP_TLD_LEN_ALL );
	if( !pBuf )
	{
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_SendCfgReq: failed.\r\n")));
		return ;
	}
	wCfgReqOpt = pIfDev->wCfgReqOpt;
	pCopy = pBuf + sizeof(CTL_HDR);
	//
	if( wCfgReqOpt & IPCP_OPT_IP_ADDR )
	{
		*pCopy++ = IPCP_TLD_TYPE_IP_ADDR;
		*pCopy++ = IPCP_TLD_LEN_IP_ADDR;
		memcpy( pCopy, &pIfDev->dwIP_Clt, sizeof(DWORD) );
		pCopy += sizeof(DWORD);
	}
#if 1
	if( wCfgReqOpt & IPCP_OPT_P_DNS_ADDR )
	{
		*pCopy++ = IPCP_TLD_TYPE_P_DNS_ADDR;
		*pCopy++ = IPCP_TLD_LEN_IP_ADDR;
		memcpy( pCopy, &pIfDev->dwIP_DNS, sizeof(DWORD) );
		pCopy += sizeof(DWORD);
	}
	if( wCfgReqOpt & IPCP_OPT_S_DNS_ADDR )
	{
		*pCopy++ = IPCP_TLD_TYPE_S_DNS_ADDR;
		*pCopy++ = IPCP_TLD_LEN_IP_ADDR;
		memcpy( pCopy, &pIfDev->dwIP_DNS2, sizeof(DWORD) );
		pCopy += sizeof(DWORD);
	}
#endif
	//
	wTmp = (WORD)(pCopy - pBuf);
	pIfDev->dwTickCfg = GetTickCount();
	pIfDev->bCntTx_Cfg ++;
	IPCP_SendPro( pIfDev, CTL_CODE_CFG_REQ, (++pIfDev->bAckerID), wTmp, pBuf );
	RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_SendCfgReq: bAckerID=[%d], wLen=[%d]\r\n"), pIfDev->bAckerID, wTmp));
	//
	free( pBuf );
}


//------------------------------------------------------
//���ܣ�������IPCP״̬
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//------------------------------------------------------
void	IPCP_Up( PPP_DEV* pIfDev )
{
	RETAILMSG(DEBUG_PPP,(TEXT("\r\n")));
	RETAILMSG(DEBUG_PPP,(TEXT(" ++++ netsrv_ap: IPCP ------notify app!")));
	RETAILMSG(DEBUG_PPP,(TEXT("\r\n")));

	RasIf_NotifyEvt( pIfDev->hIfNet, RASCS_IPCP, RASERR_SUCCESS );
	//����LCP ״̬����ʼ���Լ�����������
	pIfDev->wStateCnn = PPP_CNN_IPCP;
	pIfDev->wFlagCnn = 0;
	pIfDev->bCntTx_Cfg = 0;
	pIfDev->wCfgReqOpt = IPCP_TLD_TYPE_UP;
	//��ʼ�� ��������---for IPCP
	pIfDev->dwIP_Clt = 0;
	pIfDev->dwIP_DNS = 0;
	pIfDev->dwIP_DNS2 = 0;
	pIfDev->dwIP_Srv = 0;
	//����---��������
	IPCP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//���ܣ������յ���IPCP���ݰ�
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� IPCP���ݰ���Ϣ
//------------------------------------------------------
void	IPCP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	ASSERT( wLen>=sizeof(CTL_HDR) );
	if( wLen<sizeof(CTL_HDR) )
	{
		return ;
	}
	//
	switch( *pBuf )
	{
	case CTL_CODE_CFG_REQ	:
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_Handle_rx: CFG_REQ, bAckerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		IPCP_Handle_Cfg_Req( pIfDev, pBuf, wLen );
		break;

	case CTL_CODE_CFG_ACK	:
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_Handle_rx: CFG_ACK, bAckerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		IPCP_Handle_Cfg_Ack( pIfDev, pBuf, wLen );
		break;
	case CTL_CODE_CFG_NAK	:
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_Handle_rx: CFG_NAK, bAckerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		IPCP_Handle_Cfg_Nak( pIfDev, pBuf, wLen );
		break;
	case CTL_CODE_CFG_REJ	:
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_Handle_rx: CFG_REJ, bAckerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		IPCP_Handle_Cfg_Rej( pIfDev, pBuf, wLen );
		break;

	case CTL_CODE_TER_REQ	:
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_Handle_rx: TER_REQ, bAckerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		IPCP_Handle_Ter_Req( pIfDev, pBuf, wLen );
		break;
	case CTL_CODE_TER_ACK	:
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_Handle_rx: TER_ACK, bAckerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		//IPCP_Handle_Ter_Ack( pIfDev, pBuf, wLen );
		break;
	case CTL_CODE_REJ_CODE	:
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_Handle_rx: REJ_CODE, bAckerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		break;
	default :
		//Reject, and ����ԭ��
		RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_Handle_rx: unknown=[%d], bAckerID=[%d], wLen=[%d]\r\n"), *pBuf, pBuf[1], wLen));
		IPCP_SendPro( pIfDev, CTL_CODE_REJ_CODE, pBuf[1], wLen, pBuf );
		break;
	}
}


//------------------------------------------------------
//���ܣ������յ���IPCP���ݰ�---Config request
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� IPCP���ݰ���Ϣ
//------------------------------------------------------
void	IPCP_Handle_Cfg_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	LPBYTE	pBufNak;
	LPBYTE	pBufRej;
	LPBYTE	pBufNak_Copy;
	LPBYTE	pBufRej_Copy;
	WORD	wLenNak;
	WORD	wLenRej;

	BOOL	fGoOn;
	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// ��֤ 1����ǰ��IPCP�׶� �� 4��wLenTotal����
	if( (pIfDev->wStateCnn!=PPP_CNN_IPCP) && (pIfDev->wStateCnn!=PPP_CNN_IP) )
	{
		return ;
	}
	pCopy ++;
	bAskerID = *pCopy ++;
	wLenTotal = pntohs( pCopy );
	pCopy += sizeof(WORD);
	if( wLenTotal>wLen )//�����ж���
	{
		return ;
	}
	// ׼��pBufNak��pBufRej
	pBufNak = (LPBYTE)malloc( wLenTotal+20 );
	if( !pBufNak )
	{
		return ;
	}
	pBufRej = (LPBYTE)malloc( wLenTotal+20 );
	if( !pBufRej )
	{
		free( pBufNak );
		return ;
	}
	wLenNak = sizeof(CTL_HDR);
	wLenRej = sizeof(CTL_HDR);
	pBufNak_Copy = pBufNak + sizeof(CTL_HDR);
	pBufRej_Copy = pBufRej + sizeof(CTL_HDR);
	fGoOn = TRUE;
	// ��֤ͨ���ˣ���ʼ��������
	pIfDev->dwIP_Srv = 0;
	// ���� LCP Config Option
	pCopyEnd = pBuf + wLenTotal;
	while( (pCopy < pCopyEnd) && fGoOn )
	{
		//TLD___Type
		bLenTotal = pCopy[1];
		switch( *pCopy )
		{
		case IPCP_TLD_TYPE_IP_ADDR:
			//������Ȳ��ԣ�������Ƕ���ģ���Ҫ��ʾ�Է�����������
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				memcpy( &pIfDev->dwIP_Srv, pCopy+2, sizeof(DWORD) );
				if( DEBUG_PPP )
				{
					char	pAddr[32];
					inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_Srv, pAddr );
					RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_IPCP: SrvIP[ %s ]\r\n"), pAddr));
				}
			}
			break;

		case IPCP_TLD_TYPE_IP_COMP:
		default :
			memcpy( pBufRej_Copy, pCopy, bLenTotal );
			pBufRej_Copy += bLenTotal;
			wLenRej += bLenTotal;
			break;
		}
		pCopy += bLenTotal;	//go on
	}
	// ������Ϣ
	if( wLenRej>sizeof(CTL_HDR) )
	{
		IPCP_SendPro( pIfDev, CTL_CODE_CFG_REJ, bAskerID, wLenRej, pBufRej );
	}
	else if( wLenNak>sizeof(CTL_HDR) )
	{
		IPCP_SendPro( pIfDev, CTL_CODE_CFG_NAK, bAskerID, wLenNak, pBufNak );
	}
	else
	{
		pBufNak_Copy = pBufNak;
		memcpy( pBufNak+sizeof(CTL_HDR), (pBuf+sizeof(CTL_HDR)), (wLen-sizeof(CTL_HDR)) );
		IPCP_SendPro( pIfDev, CTL_CODE_CFG_ACK, bAskerID, wLen, pBufNak_Copy );
		//���� ��Ϣ
		if( pIfDev->wStateCnn==PPP_CNN_IPCP )
		{
			pIfDev->wFlagCnn |= IPCP_CNN_TXACK;
			if( pIfDev->wFlagCnn & IPCP_CNN_RXACK )
			{
				//���� IPCP ���
				PPP_Notify( (HANDLE)pIfDev, PPP_EVT_IPCP_OK );
			}
		}
	}
	//
	free( pBufRej );
	free( pBufNak );
}


//------------------------------------------------------
//���ܣ������յ���IPCP���ݰ�---Config ACK
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� IPCP���ݰ���Ϣ
//------------------------------------------------------
void	IPCP_Handle_Cfg_Ack( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	BOOL	fGoOn;
	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// ��֤ 1����ǰ��IPCP�׶� �� 2����ǰ��û���յ�ACK��3��bAskerID is right��4��wLenTotal����
	if( (pIfDev->wStateCnn!=PPP_CNN_IPCP) || (pIfDev->wFlagCnn & IPCP_CNN_RXACK) )
	{
		return ;
	}
	pCopy ++;
	bAskerID = *pCopy;
	if( bAskerID!=pIfDev->bAckerID )
	{
		return ;
	}
	pCopy ++;
	wLenTotal = pntohs( pCopy );
	pCopy += sizeof(WORD);
	if( wLenTotal>wLen )//�����ж���
	{
		return ;
	}
	// ��֤ͨ���ˣ���ʼ��������
	pIfDev->wFlagCnn |= IPCP_CNN_RXACK;
	// ���� LCP Config Option
	pCopyEnd = pBuf + wLenTotal;
	fGoOn = TRUE;
	while( (pCopy < pCopyEnd) && fGoOn )
	{
		//TLD___Type
		bLenTotal = pCopy[1];
		switch( *pCopy )
		{
		case IPCP_TLD_TYPE_IP_ADDR:
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				memcpy( &pIfDev->dwIP_Clt, pCopy+2, sizeof(DWORD) );
				if( DEBUG_PPP )
				{
					char	pAddr[32];
					inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_Clt, pAddr );
					RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_IPCP: CltIP[ %s ]\r\n"), pAddr));
				}
			}
			//else
			//{
			//	fGoOn = FALSE;
			//}
			break;

		case IPCP_TLD_TYPE_P_DNS_ADDR:
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				memcpy( &pIfDev->dwIP_DNS, pCopy+2, sizeof(DWORD) );
				if( DEBUG_PPP )
				{
					char	pAddr[32];
					inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_DNS, pAddr );
					RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_IPCP: DnsIP[ %s ]\r\n"), pAddr));
				}
			}
			//else
			//{
			//	fGoOn = FALSE;
			//}
			break;

		case IPCP_TLD_TYPE_S_DNS_ADDR:
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				memcpy( &pIfDev->dwIP_DNS2, pCopy+2, sizeof(DWORD) );
				if( DEBUG_PPP )
				{
					char	pAddr[32];
					inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_DNS2, pAddr );
					RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_IPCP: Dns2IP[ %s ]\r\n"), pAddr));
				}
			}
			//else
			//{
			//	fGoOn = FALSE;
			//}
			break;

		default :
			break;
		}
		//
		pCopy += bLenTotal;	//go on
	}
	//
	if( pIfDev->wFlagCnn & IPCP_CNN_TXACK )
	{
		//���� IPCP ���
		PPP_Notify( (HANDLE)pIfDev, PPP_EVT_IPCP_OK );
	}
}


//------------------------------------------------------
//���ܣ������յ���IPCP���ݰ�---Config NAK
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� IPCP���ݰ���Ϣ
//------------------------------------------------------
void	IPCP_Handle_Cfg_Nak( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	BOOL	fGoOn;
	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// ��֤ 1����ǰ��LCP�׶� �� 2����ǰ��û���յ�ACK��3��bAskerID is right��4��wLenTotal����
	if( (pIfDev->wStateCnn!=PPP_CNN_IPCP) || (pIfDev->wFlagCnn & IPCP_CNN_RXACK) )
	{
		return ;
	}
	pCopy ++;
	bAskerID = *pCopy;
	if( bAskerID!=pIfDev->bAckerID )
	{
		return ;
	}
	pCopy ++;
	wLenTotal = pntohs( pCopy );
	pCopy += sizeof(WORD);
	if( wLenTotal>wLen )//�����ж���
	{
		return ;
	}
	// ��֤ͨ���ˣ���ʼ��������
	pIfDev->bCntRx_Nak_Rej ++;
	// ���� LCP Config Option
	pCopyEnd = pBuf + wLenTotal;
	fGoOn = TRUE;
	while( (pCopy < pCopyEnd) && fGoOn )
	{
		//TLD___Type
		bLenTotal = pCopy[1];
		switch( *pCopy )
		{
		case IPCP_TLD_TYPE_IP_ADDR:
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				memcpy( &pIfDev->dwIP_Clt, pCopy+2, sizeof(DWORD) );
			}
			//else
			//{
			//	fGoOn = FALSE;
			//}
			break;

		case IPCP_TLD_TYPE_P_DNS_ADDR:
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				memcpy( &pIfDev->dwIP_DNS, pCopy+2, sizeof(DWORD) );
			}
			//else
			//{
			//	fGoOn = FALSE;
			//}
			break;

		case IPCP_TLD_TYPE_S_DNS_ADDR:
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				memcpy( &pIfDev->dwIP_DNS2, pCopy+2, sizeof(DWORD) );
			}
			//else
			//{
			//	fGoOn = FALSE;
			//}
			break;

		default :
			break;
		}
		//
		pCopy += bLenTotal;	//go on
	}
	//
	IPCP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//���ܣ������յ���IPCP���ݰ�---REJ
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� IPCP���ݰ���Ϣ
//------------------------------------------------------
void	IPCP_Handle_Cfg_Rej( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	BOOL	fGoOn;
	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// ��֤ 1����ǰ��LCP�׶� �� 2����ǰ��û���յ�ACK��3��bAskerID is right��4��wLenTotal����
	if( (pIfDev->wStateCnn!=PPP_CNN_IPCP) || (pIfDev->wFlagCnn & IPCP_CNN_RXACK) )
	{
		return ;
	}
	pCopy ++;
	bAskerID = *pCopy;
	if( bAskerID!=pIfDev->bAckerID )
	{
		return ;
	}
	pCopy ++;
	wLenTotal = pntohs( pCopy );
	pCopy += sizeof(WORD);
	if( wLenTotal>wLen )//�����ж���
	{
		return ;
	}
	// ��֤ͨ���ˣ���ʼ��������
	pIfDev->bCntRx_Nak_Rej ++;
	// ���� LCP Config Option
	pCopyEnd = pBuf + wLenTotal;
	fGoOn = TRUE;
	while( (pCopy < pCopyEnd) && fGoOn )
	{
		//TLD___Type
		bLenTotal = pCopy[1];
		switch( *pCopy )
		{
		case IPCP_TLD_TYPE_P_DNS_ADDR:
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				pIfDev->wCfgReqOpt &= ~IPCP_OPT_P_DNS_ADDR;
			}
			//else
			//{
			//	fGoOn = FALSE;
			//}
			break;

		case IPCP_TLD_TYPE_S_DNS_ADDR:
			if( bLenTotal==IPCP_TLD_LEN_IP_ADDR )
			{
				pIfDev->wCfgReqOpt &= ~IPCP_OPT_S_DNS_ADDR;
			}
			//else
			//{
			//	fGoOn = FALSE;
			//}
			break;

		default :
			break;
		}
		//
		pCopy += bLenTotal;	//go on
	}
	//
	IPCP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//���ܣ������յ���IPCP���ݰ�---Terminate request
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� IPCP���ݰ���Ϣ
//------------------------------------------------------
void	IPCP_Handle_Ter_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;

	// �ش�Է�ACK
	*pBuf = CTL_CODE_TER_ACK;
	PPP_OutBuf( pIfDev, PPP_PRO_IPCP, pBuf, wLen );

	//����״̬
	if( pIfDev->wStateCnn!=PPP_CNN_IP )
	{
		//��Ҫ��������
		pIfDev->bCntRx_ReCfg ++;
		IPCP_Up( pIfDev );
	}
	else
	{
		//������Ӧ����
		pIfDev->bRunCfg = 0;
		//pIfDev->wStateCnn = PPP_CNN_TERMINATE;
		//pIfDev->wFlagCnn = 0;
		//Ҫ�Ͽ���������
		RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_Disconnected, RASERR_SUCCESS );
	}
}

//------------------------------------------------------
//���ܣ������յ���IPCP���ݰ�---Terminate ack
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� IPCP���ݰ���Ϣ
//------------------------------------------------------
//void	IPCP_Handle_Ter_Ack( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
//{
//}
