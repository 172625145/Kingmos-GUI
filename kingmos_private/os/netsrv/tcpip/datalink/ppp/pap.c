/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����PAP
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
#include "ap.h"

/***************  ȫ���� ���壬 ���� *****************/
static	void	PAP_Handle_AckOrNak( PPP_DEV* pIfDev, BOOL fIsAck );

extern DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr );
/******************************************************/


//------------------------------------------------------
//���ܣ�������������
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Code      |  Identifier   |            Length             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   | Peer-ID Length|  Peer-Id ...
//   +-+-+-+-+-+-+-+-+-+-+-+-+
//   | Passwd-Length |  Password  ...
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+
//------------------------------------------------------
void	PAP_SendCfgReq( PPP_DEV* pIfDev )
{
	DIALPARAM*		pDialParam;
	LPBYTE			pBufAP;
	LPBYTE			pCopy;
	WORD			wLen;
	BYTE			bLenDn;
	BYTE			bLenUsr;
	BYTE			bLenPw;

	//
	pDialParam = &pIfDev->DialParam;
	bLenDn  = (BYTE)strlen(pDialParam->szDomain);
	bLenUsr = (BYTE)strlen(pDialParam->szUserName);
	bLenPw  = (BYTE)strlen(pDialParam->szPassword);
	if( bLenDn )
	{
		wLen = sizeof(CTL_HDR)+ 1+bLenDn+1+bLenUsr+ 1+bLenPw;
	}
	else
	{
		wLen = sizeof(CTL_HDR)+ 1+bLenUsr+ 1+bLenPw;
	}
	//
	pBufAP  = (LPBYTE)malloc( wLen );
	if( !pBufAP )
	{
		RETAILMSG(DEBUG_PPP,(TEXT("  PAP_SendCfgReq: failed.\r\n")));
		return ;
	}
	pCopy = (pBufAP+sizeof(CTL_HDR));
	//Fill User
	if( bLenDn )
	{
		*pCopy++ = (bLenDn+1+bLenUsr);

		memcpy( pCopy, pDialParam->szDomain, bLenDn );
		pCopy += bLenDn;
		*pCopy++ = '\\';
	}
	else
	{
		*pCopy++ = bLenUsr;
	}
	if( bLenUsr )
	{
		memcpy( pCopy, pDialParam->szUserName, bLenUsr );
		pCopy += bLenUsr;
	}
	//Fill Password
	*pCopy++ = bLenPw;
	if( bLenPw )
	{
		memcpy( pCopy, pDialParam->szPassword, bLenPw );
		pCopy += bLenPw;
	}
	//Send data
	pIfDev->dwTickCfg = GetTickCount();
	pIfDev->bCntTx_Cfg ++;
	PAP_SendPro( pIfDev, PAP_CODE_REQ, (++pIfDev->bAckerID), wLen, pBufAP );
	RETAILMSG(DEBUG_PPP,(TEXT("  PAP_SendCfgReq: bAckerID=[%d], wLen=[%d]\r\n"), pIfDev->bAckerID, wLen));
	//
	free( pBufAP );
}

//------------------------------------------------------
//���ܣ�������PAP״̬
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//------------------------------------------------------
void	PAP_Up( PPP_DEV* pIfDev )
{
	RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_Authenticate, RASERR_SUCCESS );
	//����LCP ״̬
	pIfDev->wStateCnn = PPP_CNN_PAP;
	pIfDev->wFlagCnn = 0;
	//��ʼ��---�Լ�����������
	//����---��������
	PAP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//���ܣ������յ���PAP���ݰ�
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� PAP���ݰ���Ϣ
//------------------------------------------------------
void	PAP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	ASSERT( wLen>=sizeof(CTL_HDR) );
	if( wLen<sizeof(CTL_HDR) )
	{
		return ;
	}

	//
	switch( *pBuf )
	{
	case PAP_CODE_ACK	:
		RETAILMSG(DEBUG_PPP,(TEXT("  PAP_Handle_rx: PAP_CODE_ACK, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		PAP_Handle_AckOrNak( pIfDev, TRUE );
		break;
	case PAP_CODE_NAK	:
		RETAILMSG(DEBUG_PPP,(TEXT("  PAP_Handle_rx: PAP_CODE_NAK, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		PAP_Handle_AckOrNak( pIfDev, FALSE );
		break;
	default :
		//Reject, and ����ԭ��
		RETAILMSG(DEBUG_PPP,(TEXT("  PAP_Handle_rx: unknown=[%d], bAskerID=[%d], wLen=[%d]\r\n"), *pBuf, pBuf[1], wLen));
		PAP_SendPro( pIfDev, CTL_CODE_REJ_CODE, pBuf[1], wLen, pBuf );
		break;
	}

}

//------------------------------------------------------
//���ܣ������յ���PAP���ݰ�---Ack or Nak
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� PAP���ݰ���Ϣ
//------------------------------------------------------
void	PAP_Handle_AckOrNak( PPP_DEV* pIfDev, BOOL fIsAck )
{
	if( pIfDev->wStateCnn!=PPP_CNN_PAP || (pIfDev->wFlagCnn & AP_CNN_RXACK) )
	{
		return ;
	}
	if( fIsAck )
	{
		RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_ap: PAP_rx_ack!\r\n")));
		pIfDev->wFlagCnn |= AP_CNN_RXACK;
		RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_AuthAck, RASERR_SUCCESS );
		//���� AP ���
		PPP_Notify( (HANDLE)pIfDev, PPP_EVT_AP_OK );
	}
	else
	{
		//Ҫ�Ͽ���������
		RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_ap: PAP_rx_nak!\r\n")));
		RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_Authenticate, RASERR_PPP_PASSWORD );
	}
}




