/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：LCP
版本号：  1.0.0
开发时期：2003-12-01
作者：    肖远钢
修改记录：
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


/***************  全局区 定义， 声明 *****************/
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
//功能：Check收到的TLD类型的数据包组合
//参数：
//pBuf/wLen---指定 数据包信息
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
//功能：发送配置请求
//参数：
//pIfDev---指定 PPP的下端接口
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
	//一般情况，不需要对方使用密码登陆到我方
	//if( wCfgReqOpt & LCP_OPT_PAP )		//用PAP登陆
	//{
	//	*pCopy++ = LCP_CFG_AP;
	//	*pCopy++ = LCP_TLD_LEN_PAP;
	//	wTmp = PPP_PRO_PAP;
	//	memcpy( pCopy, &wTmp, sizeof(WORD) );
	//	pCopy += sizeof(WORD);
	//}
	//else if( wCfgReqOpt & LCP_OPT_CHAP )//用CHAP登陆
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
//功能：启动到LCP状态
//参数：
//pIfDev---指定 PPP的下端接口
//------------------------------------------------------
void	LCP_Up( PPP_DEV* pIfDev )
{
	RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Up: LCP_SendCfgReq\r\n")));
	RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_PPP, RASERR_SUCCESS );
	//进入LCP 状态
	pIfDev->wStateCnn = PPP_CNN_LCP;
	pIfDev->wFlagCnn = 0;
	pIfDev->bCntTx_Cfg = 0;
	//初始化---自己的配置请求
	pIfDev->wCfgReqOpt = LCP_CFG_UP;
	pIfDev->dwMN = GetTickCount( );//0x015886a8;
	//初始化---对方的配置请求
	pIfDev->dwMN_Other = 0;
	pIfDev->dwACCM_Other = 0;
	pIfDev->wFlagLCP = 0;
	//pIfDev->wMRU = MRU_PPPRECV;
	//发送---配置请求
	LCP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//功能：处理收到的LCP数据包
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 LCP数据包信息
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
		//Reject, and 返回原文
		RETAILMSG(DEBUG_PPP,(TEXT("  LCP_Handle_rx: unknown=[%d], bAskerID=[%d], wLen=[%d]\r\n"), *pBuf, pBuf[1], wLen));
		LCP_SendPro( pIfDev, CTL_CODE_REJ_CODE, pBuf[1], wLen, pBuf );
		break;
	}
}

//------------------------------------------------------
//功能：处理收到的LCP数据包---Echo
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 LCP数据包信息
//------------------------------------------------------
void	LCP_Handle_Echo_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	WORD	wLenTotal;

	// 保证 1。当前是LCP阶段
	//if( pIfDev->wStateCnn!=PPP_CNN_LCP || wLen<(sizeof(CTL_HDR)+sizeof(DWORD)) )
	if( wLen<(sizeof(CTL_HDR)+sizeof(DWORD)) )
	{
		return ;
	}
	wLenTotal = sizeof(CTL_HDR)+sizeof(DWORD);
	//填充 CTL_HDR
	*pCopy ++ = LCP_CODE_ECHO_REPLY;
	pCopy ++;
	phtonsp( pCopy, (LPBYTE)&wLenTotal );
	pCopy += sizeof(WORD);
	pIfDev->dwMN = GetTickCount();
	memcpy( pCopy, &pIfDev->dwMN, sizeof(DWORD) );
	//发送
	PPP_OutBuf( pIfDev, PPP_PRO_LCP, pBuf, wLenTotal );
}

//------------------------------------------------------
//功能：处理收到的LCP数据包---Config request
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 LCP数据包信息
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

	// 保证 1。当前是LCP阶段 ； 4。wLenTotal合理
	//if( pIfDev->wStateCnn!=PPP_CNN_LCP )
	//{
	//	return ;
	//}
	pCopy ++;
	bAskerID = *pCopy ++;
	wLenTotal = pntohs( pCopy );
	pCopy += sizeof(WORD);
	if( wLenTotal>wLen )//可能有恶意
	{
		return ;
	}
	// 准备pBufNak、pBufRej
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
	// 验证通过了，初始化、设置
	pIfDev->dwMN_Other = 0;
	pIfDev->dwACCM_Other = 0;
	pIfDev->wFlagLCP = 0;
	// 分析 LCP Config Option
//	fGoOn = TRUE;
	pCopyEnd = pBuf + wLenTotal;
	while( (pCopy < pCopyEnd) )//(&& fGoOn )
	{
		bLenTotal = pCopy[1];
		switch( *pCopy )	//TLD___Type
		{
		case LCP_TLD_TYPE_MRU:
			//如果长度不对，则可能是恶意的，需要提示对方并立即返回
			if( bLenTotal==LCP_TLD_LEN_MRU )
			{
				wTmp = pntohs( pCopy+2 );
				if( wTmp!=pIfDev->wMRU )
				{
					if( (wTmp > pIfDev->wMRU) || (wTmp < MRU_PPP_MIN) )
					{
						//如果内容不对，则只要提示对方就可以了
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
			//如果长度不对，则可能是恶意的，需要提示对方并立即返回
			if( bLenTotal==LCP_TLD_LEN_ACCM )
			{
				pIfDev->dwACCM_Other = pntohl( pCopy+2 );
			}
			break;

		case LCP_TLD_TYPE_AP	:
			//目前只支持 PAP、CHAP
			memcpy( &wTmp, pCopy+2, sizeof(WORD) );
			if( wTmp==PPP_PRO_PAP )
			{
				pIfDev->wFlagLCP |= PF_LCP_PAP;
			}
#if	1		//暂时对方不接受 CHAP
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
			//如果长度不对，则可能是恶意的，需要提示对方并立即返回
			if( bLenTotal==LCP_TLD_LEN_MN )
			{
				memcpy( &pIfDev->dwMN_Other, pCopy+2, sizeof(DWORD) );
			}
			break;

		case LCP_TLD_TYPE_PFC:
			//如果长度不对，则可能是恶意的，需要提示对方并立即返回
			if( bLenTotal==LCP_TLD_LEN_PFC )
			{
				//表明 对方支持 PFC
				pIfDev->wFlagLCP |= PF_LCP_PFC;
			}
			break;
		case LCP_TLD_TYPE_ACFC:
			//如果长度不对，则可能是恶意的，需要提示对方并立即返回
			if( bLenTotal==LCP_TLD_LEN_ACFC )
			{
				//表明 对方支持 ACFC
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
	//回答对方
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
				//启动 LCP 完成
				PPP_Notify( (HANDLE)pIfDev, PPP_EVT_LCP_OK );
			}
		}
	}
	//
	free( pBufRej );
	free( pBufNak );
}

//------------------------------------------------------
//功能：处理收到的LCP数据包---Config ACK
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 LCP数据包信息
//------------------------------------------------------
void	LCP_Handle_Cfg_Ack( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	BOOL	fIsOurs;
	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// 保证 1。当前是LCP阶段 ； 2。当前还没有收到ACK；3。bAskerID is right；4。wLenTotal合理
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
	if( wLenTotal>wLen )//可能有恶意
	{
		return ;
	}
	//
	// 分析 LCP Config Option
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
	//功能：表明 Peer已经同意 本地支持的功能(MRU/MN/PFC/ACFC)和请求(PAP/CHAP)
	// 验证通过了，初始化、设置
	pIfDev->wFlagCnn |= LCP_CNN_RXACK;
	if( pIfDev->wFlagCnn & LCP_CNN_TXACK )
	{
		//启动 LCP 完成
		PPP_Notify( (HANDLE)pIfDev, PPP_EVT_LCP_OK );
	}
}

//------------------------------------------------------
//功能：处理收到的LCP数据包---Config NAK
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 LCP数据包信息
//------------------------------------------------------
void	LCP_Handle_Cfg_Nak( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	WORD	wTmp;
	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// 保证 1。当前是LCP阶段 ； 2。当前还没有收到ACK；3。bAskerID is right；4。wLenTotal合理
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
	if( wLenTotal>wLen )//可能有恶意
	{
		return ;
	}
	//
	pIfDev->bCntRx_Nak_Rej ++;
	// 分析 LCP Config Option
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
		//	//目前只支持 PAP、CHAP
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
	//再重新 配置请求
	LCP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//功能：处理收到的LCP数据包---REJ
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 LCP数据包信息
//------------------------------------------------------
void	LCP_Handle_Cfg_Rej( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;
	LPBYTE	pCopyEnd;

	WORD	wLenTotal;
	BYTE	bAskerID;
	BYTE	bLenTotal;

	// 保证 1。当前是LCP阶段 ； 2。当前还没有收到ACK；3。bAskerID is right；4。wLenTotal合理
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
	if( wLenTotal>wLen )//可能有恶意
	{
		return ;
	}
	pIfDev->bCntRx_Nak_Rej ++;
	// 分析 LCP Config Option
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
//功能：处理收到的LCP数据包---Terminate request
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 LCP数据包信息
//------------------------------------------------------
void	LCP_Handle_Ter_Req( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	LPBYTE	pCopy = pBuf;

	// 回答对方ACK
	*pBuf = CTL_CODE_TER_ACK;
	PPP_OutBuf( pIfDev, PPP_PRO_LCP, pBuf, wLen );

	//设置状态
	//if( pIfDev->wStateCnn!=PPP_CNN_IP )
	//{
	//	//需要重新配置
	//	pIfDev->bCntRx_ReCfg ++;
	//	LCP_Up( pIfDev );
	//}
	//else
	{
		//不用再 Terminate Request 对方了
		pIfDev->bRunCfg = 0;
		//pIfDev->wStateCnn = PPP_CNN_TERMINATE;
		//pIfDev->wFlagCnn = 0;
		//要断开整个网络
		RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_Disconnected, RASERR_SUCCESS );
	}
}

//------------------------------------------------------
//功能：处理收到的LCP数据包---Terminate ack
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 LCP数据包信息
//------------------------------------------------------
//void	LCP_Handle_Ter_Ack( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
//{
//}



