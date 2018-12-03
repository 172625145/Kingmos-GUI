/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：CHAP
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
static	void	CHAP_Handle_Challenge( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );
/******************************************************/


//------------------------------------------------------
//功能：启动到CHAP状态
//参数：
//pIfDev---指定 PPP的下端接口
//------------------------------------------------------
void	CHAP_Up( PPP_DEV* pIfDev )
{
	RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_Authenticate, RASERR_SUCCESS );
	//进入LCP 状态
	pIfDev->wStateCnn = PPP_CNN_CHAP;
	pIfDev->wFlagCnn = 0;
	//初始化---自己的配置请求
	//发送---配置请求
	//CHAP_SendCfgReq( pIfDev );
}

//------------------------------------------------------
//功能：处理收到的CHAP数据包
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 CHAP数据包信息
//------------------------------------------------------
void	CHAP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	ASSERT( wLen>=sizeof(CTL_HDR) );
	if( pIfDev->wStateCnn!=PPP_CNN_CHAP || wLen<sizeof(CTL_HDR) )
	{
		return ;
	}

	//
	switch( *pBuf )
	{
	case CHAP_CODE_CHALLENGE	:
		RETAILMSG(DEBUG_PPP,(TEXT("  CHAP_Handle_rx: CHAP_CODE_CHALLENGE, wLen=[%d]\r\n"), wLen));
		CHAP_Handle_Challenge( pIfDev, pBuf, wLen );
		break;
	case CHAP_CODE_SUCCESS	:
		RETAILMSG(DEBUG_PPP,(TEXT("  CHAP_Handle_rx: CHAP_CODE_SUCCESS, wLen=[%d]\r\n"), wLen));
		pIfDev->wFlagCnn |= AP_CNN_RXACK;
		RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_AuthAck, RASERR_SUCCESS );
		PPP_Notify( (HANDLE)pIfDev, PPP_EVT_AP_OK );
		break;
	case CHAP_CODE_FAILURE	:
		RETAILMSG(DEBUG_PPP,(TEXT("  CHAP_Handle_rx: CHAP_CODE_FAILURE, wLen=[%d]\r\n"), wLen));
		RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_Authenticate, RASERR_PPP_PASSWORD );
		break;
	case CHAP_CODE_RESPONSE	:
		RETAILMSG(DEBUG_PPP,(TEXT("  CHAP_Handle_rx: CHAP_CODE_RESPONSE, wLen=[%d]\r\n"), wLen));
		break;
	default :
		RETAILMSG(DEBUG_PPP,(TEXT("  CHAP_Handle_rx: unknown, wLen=[%d]\r\n"), wLen));
		CHAP_SendPro( pIfDev, CTL_CODE_REJ_CODE, *(pBuf+1), wLen, pBuf );
		break;
	}

}

//------------------------------------------------------
//功能：处理收到的CHAP数据包---回答 对方的挑战报文
//参数：
//pIfDev---指定 PPP的下端接口
//pBuf/wLen---指定 CHAP数据包信息
//   A summary of the Challenge and Response packet format :
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Code      |  Identifier   |            Length             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  Value-Size   |  Value ...
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  Name ...
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//------------------------------------------------------
void	CHAP_Handle_Challenge( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen )
{
	DIALPARAM*		pDialParam;	//拨号参数
	LPBYTE			pBufSend;
	
	//LPBYTE			pBufIn;
	BYTE			pBufFragSet[sizeof(FRAGSETA)+3*sizeof(FRAGMENT)];
	FRAGSETA*		pFragSetA = (FRAGSETA*)pBufFragSet;
	BYTE			digest[MD5_SIGNATURE_SIZE];

	WORD			wLenTotal;
	BYTE			bAskerID;
	BYTE			bSizeValue;
	BYTE			bLenUsr;
	//BYTE			bLenPw;

	//获取信息---bSizeValue、wLenTotal、   bLenUsr、bLenPw、wLenSend、
	wLenTotal = pntohs( pBuf+2 );
	if( wLenTotal>wLen )
	{
		return ;
	}
	bAskerID = pBuf[1];
	bSizeValue = *(pBuf + sizeof(CTL_HDR));

	//统计长度：本地 PW/USER，要发送数据的长度
	pDialParam = &pIfDev->DialParam;
	bLenUsr = (BYTE)strlen(pDialParam->szUserName);
	//bLenPw  = (BYTE)strlen(pDialParam->szPassword);
	wLenTotal = sizeof(CTL_HDR) +1+MD5_SIGNATURE_SIZE +bLenUsr;

	//分配： 产生密文的长度 / 要发送数据的长度
	//pBufIn = (LPBYTE)malloc( 1+bLenPw+bSizeValue );
	//if( !pBufIn )
	//{
	//	return ;
	//}
	pBufSend = (LPBYTE)malloc( wLenTotal );
	if( !pBufSend )
	{
		//free( pBufIn );
		return ;
	}

	//产生密文: ID PW VALUE
	//*pBufIn = bAskerID;
	//memcpy( pBufIn+1, pDialParam->szPassword, bLenPw );
	//memcpy( pBufIn+1+bLenPw, pBuf+(sizeof(CTL_HDR)+1), bSizeValue );
	//CreateMD5( pBufIn, 1+bLenPw+bSizeValue, digest );
	pFragSetA->nCnt = 3;
	pFragSetA->pFrag[0].pData = &bAskerID;
	pFragSetA->pFrag[0].nLen = 1;
	pFragSetA->pFrag[1].pData = pDialParam->szPassword;
	pFragSetA->pFrag[1].nLen = strlen(pDialParam->szPassword);//bLenPw
	pFragSetA->pFrag[2].pData = pBuf+(sizeof(CTL_HDR)+1);
	pFragSetA->pFrag[2].nLen = bSizeValue;
	CreateMD5_Ex( pFragSetA, digest );

    //  ------------------------------
    // | CTL_HDR | 16 | digest | name |
    //  ------------------------------
	//填充 CTL_HDR
	*pBufSend = CHAP_CODE_RESPONSE;
	pBufSend[1] = bAskerID;
	phtonsp( pBufSend+2, (LPBYTE)&wLenTotal );
	//填充 密文
	pBufSend[sizeof(CTL_HDR)] = MD5_SIGNATURE_SIZE;
	memcpy( pBufSend+(sizeof(CTL_HDR)+1), digest, MD5_SIGNATURE_SIZE );
	//填充 用户
	memcpy( pBufSend+(sizeof(CTL_HDR)+1)+MD5_SIGNATURE_SIZE, (LPBYTE)pDialParam->szUserName, bLenUsr );

	//发送
	PPP_OutBuf( pIfDev, PPP_PRO_CHAP, pBufSend, wLenTotal );

	//
	//free( pBufIn );
	free( pBufSend );
}




