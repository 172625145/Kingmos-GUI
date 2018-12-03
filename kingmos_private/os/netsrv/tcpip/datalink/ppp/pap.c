/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：PAP
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
#include "ap.h"

/***************  全局区 定义， 声明 *****************/
static	void	PAP_Handle_AckOrNak( PPP_DEV* pIfDev, BOOL fIsAck );

extern DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr );
/******************************************************/


//------------------------------------------------------
//功能：发送配置请求
//参数：
//pIfDev---指定 PPP的下端接口
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
//功能：启动到PAP状态
//参数：
//pIfDev---指定 PPP的下端接口
//------------------------------------------------------
void	PAP_Up( PPP_DEV* pIfDev )
{
	RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_Authenticate, RASERR_SUCCESS );
	//进入LCP 状态
	pIfDev->wStateCnn = PPP_CNN_PAP;
	pIfDev->wFlagCnn = 0;
	//初始化---自己的配置请求
	//发送---配置请求
	PAP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//功能：处理收到的PAP数据包
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 PAP数据包信息
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
		//Reject, and 返回原文
		RETAILMSG(DEBUG_PPP,(TEXT("  PAP_Handle_rx: unknown=[%d], bAskerID=[%d], wLen=[%d]\r\n"), *pBuf, pBuf[1], wLen));
		PAP_SendPro( pIfDev, CTL_CODE_REJ_CODE, pBuf[1], wLen, pBuf );
		break;
	}

}

//------------------------------------------------------
//功能：处理收到的PAP数据包---Ack or Nak
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 PAP数据包信息
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
		//启动 AP 完成
		PPP_Notify( (HANDLE)pIfDev, PPP_EVT_AP_OK );
	}
	else
	{
		//要断开整个网络
		RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_ap: PAP_rx_nak!\r\n")));
		RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_Authenticate, RASERR_PPP_PASSWORD );
	}
}




