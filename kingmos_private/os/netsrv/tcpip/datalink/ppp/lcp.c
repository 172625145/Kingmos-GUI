/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����LCP
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
#include "lcp.h"


/***************  ȫ���� ���壬 ���� *****************/
#define	LCP_CFG_UP		(LCP_OPT_MRU | LCP_OPT_ACCM | LCP_OPT_MN | LCP_OPT_PFC | LCP_OPT_ACFC)
#define	LCP_CFG_UP_LEN	(sizeof(CTL_HDR)+ LCP_TLD_LEN_MRU+ LCP_TLD_LEN_ACCM+ LCP_TLD_LEN_MN+ LCP_TLD_LEN_PFC+ LCP_TLD_LEN_ACFC +20)

static	void	LCP_Handle_Cfg_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	LCP_Handle_Echo_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	LCP_Handle_Cfg_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	LCP_Handle_Cfg_Ack( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	LCP_Handle_Cfg_Nak( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	LCP_Handle_Cfg_Rej( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
static	void	LCP_Handle_Ter_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr );
/******************************************************/


//------------------------------------------------------
//���ܣ�Check�յ���TLD���͵����ݰ����
//������
//pBuf/wLen---ָ�� ���ݰ���Ϣ
//------------------------------------------------------
#if 0
BOOL	TLD_CheckBuf( LPBYTE pBuf, WORD wLen )
{
	LPBYTE		pCopy = pBuf;
	LPBYTE		pCopyEnd = pBuf + wLen;
	BOOL		fIsRight = TRUE;
	BYTE		bTmp;
	
	while( pCopy<pCopyEnd )
	{
		if( *pCopy )
		{
			if( pCopy <= (pCopyEnd-2) )
			{
				pCopy ++;			//T
				bTmp = pCopy ++;	//L
			}
			else
			{
			}
		}
		else
		{
			pCopy ++;
		}
	}

	return fIsRight;
}
#endif

//------------------------------------------------------
//���ܣ�������������
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//------------------------------------------------------
void	LCP_SendCfgReq( PPP_DEV* pIfDev )
{
	LPBYTE		pBuf;
	LPBYTE		pCopy;
	WORD		wCfgReqOpt;
	WORD		wTmp;

	pBuf = (LPBYTE)malloc( LCP_CFG_UP_LEN );
	if( !pBuf )
	{
//#ifdef DEBUG_PPP
		RETAILMSG(1,(TEXT("  LCP_SendCfgReq: malloc=[%d], Failed!\r\n"), LCP_CFG_UP_LEN));
//#endif
		return ;
	}
	wCfgReqOpt = pIfDev->wCfgReqOpt;
	pCopy = pBuf + sizeof(CTL_HDR);
	//
	if( wCfgReqOpt & LCP_OPT_MRU )
	{
		*pCopy++ = LCP_TLD_TYPE_MRU;
		*pCopy++ = LCP_TLD_LEN_MRU;
		phtonsp( pCopy, (LPBYTE)&pIfDev->wMRU );
		pCopy += sizeof(WORD);
		RETAILMSG(1,(TEXT("  LCP_SendCfgReq: send wMRU=[%d]!\r\n"), pIfDev->wMRU));
	}
	if( wCfgReqOpt & LCP_OPT_ACCM )
	{
		*pCopy++ = LCP_TLD_TYPE_ACCM;
		*pCopy++ = LCP_TLD_LEN_ACCM;
		memset( pCopy, 0, sizeof(DWORD) );
		pCopy += sizeof(DWORD);
	}
	//һ�����������Ҫ�Է�ʹ�������½���ҷ�
	//if( wCfgReqOpt & LCP_OPT_PAP )		//��PAP��½
	//{
	//	*pCopy++ = LCP_CFG_AP;
	//	*pCopy++ = LCP_TLD_LEN_PAP;
	//	wTmp = PPP_PRO_PAP;
	//	memcpy( pCopy, &wTmp, sizeof(WORD) );
	//	pCopy += sizeof(WORD);
	//}
	//else if( wCfgReqOpt & LCP_OPT_CHAP )//��CHAP��½
	//{
	//	*pCopy++ = LCP_CFG_AP;
	//	*pCopy++ = LCP_TLD_LEN_CHAP;
	//	wTmp = PPP_PRO_CHAP;
	//	memcpy( pCopy, &wTmp, sizeof(WORD) );
	//	pCopy += sizeof(WORD);
	//	*pCopy++ = CHAP_MD5;
	//}
	if( wCfgReqOpt & LCP_OPT_MN )
	{
		*pCopy++ = LCP_TLD_TYPE_MN;
		*pCopy++ = LCP_TLD_LEN_MN;
		pIfDev->dwMN = GetTickCount();
		memcpy( pCopy, &pIfDev->dwMN, sizeof(DWORD) );
		pCopy += sizeof(DWORD);
	}
	if( wCfgReqOpt & LCP_OPT_PFC )
	{
		*pCopy++ = LCP_TLD_TYPE_PFC;
		*pCopy++ = LCP_TLD_LEN_PFC;
	}
	if( wCfgReqOpt & LCP_OPT_ACFC )
	{
		*pCopy++ = LCP_TLD_TYPE_ACFC;
		*pCopy++ = LCP_TLD_LEN_ACFC;
	}
	//
	wTmp = (WORD)(pCopy - pBuf);
	pIfDev->dwTickCfg = GetTickCount();
	pIfDev->bCntTx_Cfg ++;
	LCP_SendPro( pIfDev, CTL_CODE_CFG_REQ, (++pIfDev->bAckerID), wTmp, pBuf );
	RETAILMSG(1,(TEXT("  LCP_SendCfgReq: bAckerID=[%d], wLen=[%d]\r\n"), pIfDev->bAckerID, wTmp));
#if 0
#ifdef STA_DEBUG_PPP_LCP
	RETAILMSG(1,(TEXT("  LCP_SendCfgReq: bAckerID, wLen=[%d]\r\n"), pIfDev->bAckerID, wTmp));
#elif defined(DYN_DEBUG_PPP_LCP)
	RETAILMSG(SEL_PPP_LCP,(TEXT("  LCP_SendCfgReq: bAckerID, wLen=[%d]\r\n"), pIfDev->bAckerID, wTmp));
#endif
#endif
	//
	free( pBuf );
}

//------------------------------------------------------
//���ܣ�������LCP״̬
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//------------------------------------------------------
void	LCP_Up( PPP_DEV* pIfDev )
{
	RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Up: LCP_SendCfgReq\r\n")));
	RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_PPP, RASERR_SUCCESS );
	//����LCP ״̬
	pIfDev->wStateCnn = PPP_CNN_LCP;
	pIfDev->wFlagCnn = 0;
	pIfDev->bCntTx_Cfg = 0;
	//��ʼ��---�Լ�����������
	pIfDev->wCfgReqOpt = LCP_CFG_UP;
	pIfDev->dwMN = GetTickCount( );//0x015886a8;
	//��ʼ��---�Է�����������
	pIfDev->dwMN_Other = 0;
	pIfDev->dwACCM_Other = 0;
	pIfDev->wFlagLCP = 0;
	//pIfDev->wMRU = MRU_PPPRECV;
	//����---��������
	LCP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//���ܣ������յ���LCP���ݰ�
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� LCP���ݰ���Ϣ
//------------------------------------------------------
void	LCP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
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
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: CFG_REQ, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		LCP_Handle_Cfg_Req( pIfDev, pBuf, wLen );
		break;

	case CTL_CODE_CFG_ACK	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: CFG_ACK, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		LCP_Handle_Cfg_Ack( pIfDev, pBuf, wLen );
		break;
	case CTL_CODE_CFG_NAK	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: CFG_NAK, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		LCP_Handle_Cfg_Nak( pIfDev, pBuf, wLen );
		break;
	case CTL_CODE_CFG_REJ	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: CFG_REJ, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		LCP_Handle_Cfg_Rej( pIfDev, pBuf, wLen );
		break;

	case CTL_CODE_TER_REQ	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: TER_REQ, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		LCP_Handle_Ter_Req( pIfDev, pBuf, wLen );
		break;
	case CTL_CODE_TER_ACK	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: TER_ACK, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		//LCP_Handle_Ter_Ack( pIfDev, pBuf, wLen );
		break;

	case LCP_CODE_REJ_CODE	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: REJ_CODE, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		break;
	case LCP_CODE_REJ_PRO	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: REJ_PRO, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		break;
	case LCP_CODE_ECHO_REQ	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: ECHO_REQ, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		LCP_Handle_Echo_Req( pIfDev, pBuf, wLen );
		break;
	case LCP_CODE_ECHO_REPLY:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: ECHO_REPLY, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		break;
	case LCP_CODE_DIS_REQ	:
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: DIS_REQ, bAskerID=[%d], wLen=[%d]\r\n"), pBuf[1], wLen));
		break;
	default :
		//Reject, and ����ԭ��
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: unknown=[%d], bAskerID=[%d], wLen=[%d]\r\n"), *pBuf, pBuf[1], wLen));
		LCP_SendPro( pIfDev, CTL_CODE_REJ_CODE, pBuf[1], wLen, pBuf );
		break;
	}
}

//------------------------------------------------------
//���ܣ������յ���LCP���ݰ�---Echo
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� LCP���ݰ���Ϣ
//------------------------------------------------------
void	LCP_Handle_Echo_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	WORD	wLenTotal;

	// ��֤ 1����ǰ��LCP�׶�
	//if( pIfDev->wStateCnn!=PPP_CNN_LCP || wLen<(sizeof(CTL_HDR)+sizeof(DWORD)) )
	if( wLen<(sizeof(CTL_HDR)+sizeof(DWORD)) )
	{
		return ;
	}
	wLenTotal = sizeof(CTL_HDR)+sizeof(DWORD);
	//��� CTL_HDR
	*pCopy ++ = LCP_CODE_ECHO_REPLY;
	pCopy ++;
	phtonsp( pCopy, (LPBYTE)&wLenTotal );
	pCopy += sizeof(WORD);
	pIfDev->dwMN = GetTickCount();
	memcpy( pCopy, &pIfDev->dwMN, sizeof(DWORD) );
	//����
	PPP_OutBuf( pIfDev, PPP_PRO_LCP, pBuf, wLenTotal );
}

//------------------------------------------------------
//���ܣ������յ���LCP���ݰ�---Config request
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� LCP���ݰ���Ϣ
//------------------------------------------------------
void	LCP_Handle_Cfg_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	LPBYTE	pBufNak;
	LPBYTE	pBufRej;
	LPBYTE	pBufNak_Copy;
	LPBYTE	pBufRej_Copy;
	WORD	wLenNak;
	WORD	wLenRej;
//	BOOL	fGoOn;

	WORD	wLenTotal;
	WORD	wTmp;
//	BYTE	bCode;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// ��֤ 1����ǰ��LCP�׶� �� 4��wLenTotal����
	//if( pIfDev->wStateCnn!=PPP_CNN_LCP )
	//{
	//	return ;
	//}
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
	// ��֤ͨ���ˣ���ʼ��������
	pIfDev->dwMN_Other = 0;
	pIfDev->dwACCM_Other = 0;
	pIfDev->wFlagLCP = 0;
	// ���� LCP Config Option
//	fGoOn = TRUE;
	pCopyEnd = pBuf + wLenTotal;
	while( (pCopy < pCopyEnd) )//(&& fGoOn )
	{
		bLenTotal = pCopy[1];
		switch( *pCopy )	//TLD___Type
		{
		case LCP_TLD_TYPE_MRU:
			//������Ȳ��ԣ�������Ƕ���ģ���Ҫ��ʾ�Է�����������
			if( bLenTotal==LCP_TLD_LEN_MRU )
			{
				wTmp = pntohs( pCopy+2 );
				if( wTmp!=pIfDev->wMRU )
				{
					if( (wTmp > pIfDev->wMRU) || (wTmp < MRU_PPP_MIN) )
					{
						//������ݲ��ԣ���ֻҪ��ʾ�Է��Ϳ�����
						*pBufNak_Copy++ = *pCopy;
						*pBufNak_Copy++ = LCP_TLD_LEN_MRU;

						phtonsp( pBufNak_Copy, (LPBYTE)&pIfDev->wMRU );
						pBufNak_Copy += sizeof(WORD);

						wLenNak += LCP_TLD_LEN_MRU;
					}
					else// if( wTmp > MRU_PPP_MIN )
					{
						pIfDev->wMRU = wTmp;
						//break;
					}
				}
			}
			break;

		case LCP_TLD_TYPE_ACCM:
			//������Ȳ��ԣ�������Ƕ���ģ���Ҫ��ʾ�Է�����������
			if( bLenTotal==LCP_TLD_LEN_ACCM )
			{
				pIfDev->dwACCM_Other = pntohl( pCopy+2 );
			}
			break;

		case LCP_TLD_TYPE_AP	:
			//Ŀǰֻ֧�� PAP��CHAP
			memcpy( &wTmp, pCopy+2, sizeof(WORD) );
			if( wTmp==PPP_PRO_PAP )
			{
				pIfDev->wFlagLCP |= PF_LCP_PAP;
			}
#if	1		//��ʱ�Է������� CHAP
			else if( wTmp==PPP_PRO_CHAP && bLenTotal==LCP_TLD_LEN_CHAP )//MD5
			{
				if( *(pCopy+4)==CHAP_MD5 )
				{
					pIfDev->wFlagLCP |= PF_LCP_CHAP;
				}
				else
				{
					memcpy( pBufNak_Copy, pCopy, bLenTotal );
					pBufNak_Copy += bLenTotal;
					*(pBufNak_Copy-1) = CHAP_MD5;

					wLenNak += bLenTotal;
				}
			}
#endif
			else
			{
				*pBufNak_Copy++ = *pCopy;
				*pBufNak_Copy++ = LCP_TLD_LEN_PAP;
		
				wTmp = PPP_PRO_PAP;
				memcpy( pBufNak_Copy, &wTmp, sizeof(WORD) );
				pBufNak_Copy += sizeof(WORD);
		
				wLenNak += LCP_TLD_LEN_PAP;
			}
			break;

		case LCP_TLD_TYPE_MN	:
			//������Ȳ��ԣ�������Ƕ���ģ���Ҫ��ʾ�Է�����������
			if( bLenTotal==LCP_TLD_LEN_MN )
			{
				memcpy( &pIfDev->dwMN_Other, pCopy+2, sizeof(DWORD) );
			}
			break;

		case LCP_TLD_TYPE_PFC:
			//������Ȳ��ԣ�������Ƕ���ģ���Ҫ��ʾ�Է�����������
			if( bLenTotal==LCP_TLD_LEN_PFC )
			{
				//���� �Է�֧�� PFC
				pIfDev->wFlagLCP |= PF_LCP_PFC;
			}
			break;
		case LCP_TLD_TYPE_ACFC:
			//������Ȳ��ԣ�������Ƕ���ģ���Ҫ��ʾ�Է�����������
			if( bLenTotal==LCP_TLD_LEN_ACFC )
			{
				//���� �Է�֧�� ACFC
				pIfDev->wFlagLCP |= PF_LCP_ACFC;
			}
			break;

		default :
			memcpy( pBufRej_Copy, pCopy, bLenTotal );
			pBufRej_Copy += bLenTotal;

			wLenRej += bLenTotal;
			break;
		}
		//
		pCopy += bLenTotal;	//go on
	}
	//�ش�Է�
	if( wLenRej>sizeof(CTL_HDR) )
	{
		LCP_SendPro( pIfDev, CTL_CODE_CFG_REJ, bAskerID, wLenRej, pBufRej );
	}
	else if( wLenNak>sizeof(CTL_HDR) )
	{
		LCP_SendPro( pIfDev, CTL_CODE_CFG_NAK, bAskerID, wLenNak, pBufNak );
	}
	else
	{
		memcpy( pBufNak+sizeof(CTL_HDR), (pBuf+sizeof(CTL_HDR)), (wLen-sizeof(CTL_HDR)) );
		LCP_SendPro( pIfDev, CTL_CODE_CFG_ACK, bAskerID, wLen, pBufNak );
		//
		if( pIfDev->wStateCnn==PPP_CNN_LCP )
		{
			pIfDev->wFlagCnn |= LCP_CNN_TXACK;
			//
			if( pIfDev->wFlagCnn & LCP_CNN_RXACK )
			{
				//���� LCP ���
				PPP_Notify( (HANDLE)pIfDev, PPP_EVT_LCP_OK );
			}
		}
	}
	//
	free( pBufRej );
	free( pBufNak );
}

//------------------------------------------------------
//���ܣ������յ���LCP���ݰ�---Config ACK
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� LCP���ݰ���Ϣ
//------------------------------------------------------
void	LCP_Handle_Cfg_Ack( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	BOOL	fIsOurs;
	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// ��֤ 1����ǰ��LCP�׶� �� 2����ǰ��û���յ�ACK��3��bAskerID is right��4��wLenTotal����
	if( (pIfDev->wStateCnn!=PPP_CNN_LCP) || (pIfDev->wFlagCnn & LCP_CNN_RXACK) )
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
	//
	// ���� LCP Config Option
	pCopyEnd = pBuf + wLenTotal;
	fIsOurs = TRUE;
	while( (pCopy < pCopyEnd) && fIsOurs )
	{
		bLenTotal = pCopy[1];
		switch( *pCopy )	//TLD___Type
		{
		case LCP_TLD_TYPE_MRU:
			if( !(pIfDev->wCfgReqOpt & LCP_OPT_MRU) )
			{
				fIsOurs = FALSE;
			}
			break;
		case LCP_TLD_TYPE_ACCM	:
			if( !(pIfDev->wCfgReqOpt & LCP_OPT_ACCM) )
			{
				fIsOurs = FALSE;
			}
			break;
		case LCP_TLD_TYPE_MN	:
			if( !(pIfDev->wCfgReqOpt & LCP_OPT_MN) )
			{
				fIsOurs = FALSE;
			}
			break;
		case LCP_TLD_TYPE_PFC:
			if( !(pIfDev->wCfgReqOpt & LCP_OPT_PFC) )
			{
				fIsOurs = FALSE;
			}
			break;
		case LCP_TLD_TYPE_ACFC:
			if( !(pIfDev->wCfgReqOpt & LCP_OPT_ACFC) )
			{
				fIsOurs = FALSE;
			}
			break;
		default :
			break;
		}
		//
		pCopy += bLenTotal;	//go on
	}
	if( !fIsOurs )
	{
		return ;
	}
	//���ܣ����� Peer�Ѿ�ͬ�� ����֧�ֵĹ���(MRU/MN/PFC/ACFC)������(PAP/CHAP)
	// ��֤ͨ���ˣ���ʼ��������
	pIfDev->wFlagCnn |= LCP_CNN_RXACK;
	if( pIfDev->wFlagCnn & LCP_CNN_TXACK )
	{
		//���� LCP ���
		PPP_Notify( (HANDLE)pIfDev, PPP_EVT_LCP_OK );
	}
}

//------------------------------------------------------
//���ܣ������յ���LCP���ݰ�---Config NAK
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� LCP���ݰ���Ϣ
//------------------------------------------------------
void	LCP_Handle_Cfg_Nak( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	WORD	wTmp;
	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// ��֤ 1����ǰ��LCP�׶� �� 2����ǰ��û���յ�ACK��3��bAskerID is right��4��wLenTotal����
	if( (pIfDev->wStateCnn!=PPP_CNN_LCP) || (pIfDev->wFlagCnn & LCP_CNN_RXACK) )
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
	//
	pIfDev->bCntRx_Nak_Rej ++;
	// ���� LCP Config Option
	pCopyEnd = pBuf + wLenTotal;
	while( pCopy < pCopyEnd )
	{
		bLenTotal = pCopy[1];
		switch( *pCopy )	//TLD___Type
		{
		case LCP_TLD_TYPE_MRU	:
			RETAILMSG(1,(TEXT("  LCP_CFG_NAK: MRU!\r\n")));
			if( bLenTotal==LCP_TLD_LEN_MRU )
			{
				wTmp = pntohs( pCopy+2 );
				RETAILMSG(1,(TEXT("  LCP_CFG_NAK: MRU=[%d]!\r\n"), wTmp));
				if( wTmp<pIfDev->wMRU )
				{
					if( wTmp>=MRU_PPP_MIN )
					{
						pIfDev->wMRU = wTmp;
					}
				}
			}
			break;
		case LCP_TLD_TYPE_ACCM	:
			RETAILMSG(1,(TEXT("  LCP_CFG_NAK: ACCM!\r\n")));
			pIfDev->wCfgReqOpt &= ~LCP_OPT_ACCM;
			if( bLenTotal==LCP_TLD_LEN_ACCM )
			{
				pIfDev->dwACCM_Other = pntohl( pCopy+2 );
			}
			break;
		//case LCP_CFG_AP	:
		//	//Ŀǰֻ֧�� PAP��CHAP
		//	memcpy( &wTmp, pCopy+2, sizeof(WORD) );
		//	if( wTmp==PPP_PRO_PAP )
		//	{
		//		pIfDev->wFlagLCP |= PF_LCP_PAP;
		//	}
		//	else if( wTmp==PPP_PRO_CHAP && bLenTotal==LCP_TLD_LEN_CHAP )//MD5
		//	{
		//		if( *(pCopy+4)==CHAP_MD5 )
		//		{
		//			pIfDev->wFlagLCP |= PF_LCP_CHAP;
		//		}
		//		else
		//		{
		//			memcpy( pBufNak_Copy, pCopy, bLenTotal );
		//			pBufNak_Copy += bLenTotal;
		//			*(pBufNak_Copy-1) = CHAP_MD5;

		//			wLenNak += bLenTotal;
		//		}
		//	}
		//	else
		//	{
		//		*pBufNak_Copy++ = *pCopy;
		//		*pBufNak_Copy++ = LCP_TLD_LEN_PAP;
		//
		//		wTmp = PPP_PRO_PAP;
		//		memcpy( pBufNak_Copy, &wTmp, sizeof(WORD) );
		//		pBufNak_Copy += sizeof(WORD);
		//
		//		wLenNak += LCP_TLD_LEN_PAP;
		//	}
		//	break;

		case LCP_TLD_TYPE_MN	:
			RETAILMSG(1,(TEXT("  LCP_CFG_NAK: MN!\r\n")));
			pIfDev->dwMN = GetTickCount();
			break;
		default :
			RETAILMSG(1,(TEXT("  LCP_CFG_NAK: Unknown=[%d]!\r\n"), *pCopy));
			break;
		}
		//
		pCopy += bLenTotal;	//go on
	}
	//������ ��������
	LCP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//���ܣ������յ���LCP���ݰ�---REJ
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� LCP���ݰ���Ϣ
//------------------------------------------------------
void	LCP_Handle_Cfg_Rej( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// ��֤ 1����ǰ��LCP�׶� �� 2����ǰ��û���յ�ACK��3��bAskerID is right��4��wLenTotal����
	if( (pIfDev->wStateCnn!=PPP_CNN_LCP) || (pIfDev->wFlagCnn & LCP_CNN_RXACK) )
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
	pIfDev->bCntRx_Nak_Rej ++;
	// ���� LCP Config Option
	pCopyEnd = pBuf + wLenTotal;
	while( pCopy < pCopyEnd )
	{
		bLenTotal = pCopy[1];
		switch( *pCopy )	//TLD___Type
		{
		case LCP_TLD_TYPE_MRU	:
			pIfDev->wCfgReqOpt &= ~LCP_OPT_MRU;
			break;
		case LCP_TLD_TYPE_ACCM	:
			pIfDev->wCfgReqOpt &= ~LCP_OPT_ACCM;
			break;
		case LCP_TLD_TYPE_AP	:
			pIfDev->wCfgReqOpt &= ~LCP_OPT_PAP;
			break;
		case LCP_TLD_TYPE_MN	:
			pIfDev->wCfgReqOpt &= ~LCP_OPT_MN;
			break;
		case LCP_TLD_TYPE_PFC	:
			pIfDev->wCfgReqOpt &= ~LCP_OPT_PFC;
			break;
		case LCP_TLD_TYPE_ACFC	:
			pIfDev->wCfgReqOpt &= ~LCP_OPT_ACFC;
			break;
		default :
			break;
		}
		//
		pCopy += bLenTotal;	//go on
	}
	//
	LCP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//���ܣ������յ���LCP���ݰ�---Terminate request
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� LCP���ݰ���Ϣ
//------------------------------------------------------
void	LCP_Handle_Ter_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;

	// �ش�Է�ACK
	*pBuf = CTL_CODE_TER_ACK;
	PPP_OutBuf( pIfDev, PPP_PRO_LCP, pBuf, wLen );

	//����״̬
	//if( pIfDev->wStateCnn!=PPP_CNN_IP )
	//{
	//	//��Ҫ��������
	//	pIfDev->bCntRx_ReCfg ++;
	//	LCP_Up( pIfDev );
	//}
	//else
	{
		//������ Terminate Request �Է���
		pIfDev->bRunCfg = 0;
		//pIfDev->wStateCnn = PPP_CNN_TERMINATE;
		//pIfDev->wFlagCnn = 0;
		//Ҫ�Ͽ���������
		RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_Disconnected, RASERR_SUCCESS );
	}
}

//------------------------------------------------------
//���ܣ������յ���LCP���ݰ�---Terminate ack
//������
//pIfDev---ָ�� PPP���¶˽ӿ�
//pBuf/wLen---ָ�� LCP���ݰ���Ϣ
//------------------------------------------------------
//void	LCP_Handle_Ter_Ack( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
//{
//}



