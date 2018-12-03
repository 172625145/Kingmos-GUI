/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：PPP HDLC
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
#include <pcfile.h>
//"\inc_local"
#include "netif_dl_ras.h"
//"local"
#include "ppp_def.h"


/***************  全局区 定义， 声明 *****************/
//
#if DEBUG_PPP_LOG
extern	void	PppDbg_Write( PPPDBG_ITEM* pItemDbg, LPBYTE pBuf );
#endif

//---------------------------------------------------
// FCS 计算
//---------------------------------------------------
static	const	WORD	fcstab[256] = 
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
// PPP Frame CRC
#define HDLC_FCS_INIT			0xFFFF// Initial FCS value 
#define HDLC_FCS_FINAL			0xF0B8// Good final FCS value 
#define	HDLC_FCS16(wFCS, bCh)	(((WORD)wFCS >> 8) ^ fcstab[((WORD)wFCS ^ (BYTE)bCh) & 0xff])

// byte of ppp frame
#define	HDLC_FLAG				0x7E
#define	HDLC_ESC_ASYNC			0x7D
#define	HDLC_ESC_XOR			0x20

#define	HDLC_ADDR				0xFF
#define	HDLC_CTL				0x03

//ACCM Only use 32 bits, so, look ACCM_Other, 
// PPP Frame stuff
#define	BUF_STUFF( pFrame, j, wFCS, dwACCM_Other, bTmp)\
{												\
	wFCS = HDLC_FCS16( wFCS, bTmp );			\
	if( (bTmp==HDLC_FLAG) || (bTmp==HDLC_ESC_ASYNC) || ( dwACCM_Other && (bTmp<0x20) && ( dwACCM_Other & (((DWORD)1)<<bTmp) ) ) )\
	{											\
		pFrame[j] = HDLC_ESC_ASYNC;				\
		j ++;									\
		pFrame[j] = (bTmp ^ HDLC_ESC_XOR);		\
	}											\
	else										\
	{											\
		pFrame[j] = bTmp;						\
	}											\
	j ++;										\
}

#define	FCS_BUF_STUFF( pFrame, j, dwACCM_Other, bTmp)\
{												\
	if( (bTmp==HDLC_FLAG) || (bTmp==HDLC_ESC_ASYNC) || ( dwACCM_Other && (bTmp<0x20) && ( dwACCM_Other & (((DWORD)1)<<bTmp) ) ) )\
	{											\
		pFrame[j] = HDLC_ESC_ASYNC;				\
		j ++;									\
		pFrame[j] = (bTmp ^ HDLC_ESC_XOR);		\
	}											\
	else										\
	{											\
		pFrame[j] = bTmp;						\
	}											\
	j ++;										\
}

/******************************************************/


// ********************************************************************
// 声明：
// 参数：
//	IN hIfDev---指定PPP设备
//	IN wProtocol---指定PPP的协议字
//	IN pFragSetD---指定发送的PPP数据
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：PPP发送HDLC数据函数
// 引用: 
// ********************************************************************
#define DEBUG_PPP_OutByHdlc 0
DWORD	PPP_OutByHdlc( PPP_DEV* pIfDev, WORD wProtocol, FRAGSETD* pFragSetD )
{
#if DEBUG_PPP_LOG
	PPPDBG_ITEM	ItemDbg;
#endif
	LPBYTE		pFrame;
	LPBYTE		pBack;
	LPBYTE		pData;
	LONG		i, j, k, nLen;
	FRAGMENT*	pFrag;
	DWORD		dwACCM_Other;
	WORD		wFCS;
	BYTE		bTmp;
	
	DEBUGMSG( DEBUG_PPP_OutByHdlc, ( "PPP_OutByHdlc entry.\r\n" ) );
	//CheckOutPtr( __FILE__, __LINE__ );
	//检查
	if( wProtocol==PPP_PRO_IP )
	{
		if( pIfDev->wStateCnn!=PPP_CNN_IP )
		{
			DEBUGMSG( DEBUG_PPP_OutByHdlc, ( "PPP_OutByHdlc leave01.\r\n" ) );
			return 1;
		}
		//请检查状态，是否可以发送FRAME
		wProtocol = PPP_PRO_IP;
		dwACCM_Other = pIfDev->dwACCM_Other;
	}
	else
	{
		dwACCM_Other = LCP_ACCM_DEFAULT;
	}
	//统计 数据包大小
	k = pFragSetD->nCntUsed;
	j = pFragSetD->dwBytes;
	//CheckOutPtr( __FILE__, __LINE__ );
	if( j>MRU_PPPRECV )
	{
		DEBUGMSG( DEBUG_PPP_OutByHdlc, ( "PPP_OutByHdlc leave02.\r\n" ) );
		return 2;//attention here
	}
	pFrame = (LPBYTE)malloc( j*2+20 );	//attention here
	if( !pFrame )
	{
		DEBUGMSG( DEBUG_PPP_OutByHdlc, ( "PPP_OutByHdlc leave03.\r\n" ) );
		return 3;
	}
	pBack = pFrame;

	//for debug
#if DEBUG_PPP_LOG
	ItemDbg.wFlag = PDBGF_DIROUT;
	ItemDbg.wXXX = 0;
	ItemDbg.wProtocol = wProtocol;
	ItemDbg.wLen = (WORD)pFragSetD->dwBytes;
	ItemDbg.dwTick = GetTickCount();
	FgD_CopyToBuf( pFragSetD, pFrame );
	PppDbg_Write( &ItemDbg, pFrame );
#endif

	// 填充ppp数据包开始------------
	// 1> 填充 PPP Flag
	//CheckOutPtr( __FILE__, __LINE__ );
	j = 0;
	pFrame[j++] = HDLC_FLAG;
	wFCS = HDLC_FCS_INIT;
	// 2> 填充 PPP Header---Addr and Ctl and Protocol
	if( wProtocol==PPP_PRO_LCP || !(pIfDev->wFlagLCP & PF_LCP_ACFC) )
	{
		//必需 填充 Addr and Ctl
		wFCS = HDLC_FCS16( wFCS, HDLC_ADDR );
		pFrame[j++] = HDLC_ADDR;
		bTmp = HDLC_CTL;
		BUF_STUFF( pFrame, j, wFCS, dwACCM_Other, bTmp);
	}
	if( (GET_NBYTE0_S(wProtocol)) || !(pIfDev->wFlagLCP & PF_LCP_PFC) )
	{
		//必需 填充 不能压缩 ProtoID
		bTmp = GET_NBYTE0_S( wProtocol );
		BUF_STUFF( pFrame, j, wFCS, dwACCM_Other, bTmp);
	}

	//CheckOutPtr( __FILE__, __LINE__ );

	bTmp = GET_NBYTE1_S( wProtocol );
	BUF_STUFF( pFrame, j, wFCS, dwACCM_Other, bTmp);
	// 3> 填充 PPP Data
	pFrag = &(pFragSetD->pFrag[k-1]);
	for( ; k; k--, pFrag-- )
	{
		pData = pFrag->pData;
		nLen  = pFrag->nLen;
		for( i=0; i<nLen; i++ )
		{
			bTmp = pData[i];
			BUF_STUFF( pFrame, j, wFCS, dwACCM_Other, bTmp);
		}
	}
	wFCS ^= HDLC_FCS_INIT;
	// 4> 填充 PPP FCS
	bTmp = GET_HBYTE1_S( wFCS );	//LSB first 
	FCS_BUF_STUFF( pFrame, j, dwACCM_Other, bTmp);
	bTmp = GET_HBYTE0_S( wFCS );
	FCS_BUF_STUFF( pFrame, j, dwACCM_Other, bTmp);
	//
	// 5> 填充 PPP Flag
	pFrame[j++] = HDLC_FLAG;

	//CheckOutPtr( __FILE__, __LINE__ );

	//(*pIfDev->pFnTspOut)( pIfDev->hTspDev, pFrame, (WORD)j );
	RasIf_DownData( pIfDev->hRasDev, pFrame, j );
	//
	//CheckOutPtr( __FILE__, __LINE__ );
	free( pBack );
	//CheckOutPtr( __FILE__, __LINE__ );

	DEBUGMSG( DEBUG_PPP_OutByHdlc, ( "PPP_OutByHdlc leave04.\r\n" ) );
	return 0;
}


// ********************************************************************
// 声明：
// 参数：
//	IN hIfDev---指定PPP设备
//	IN pBufData---指定接收的PPP的HDLC数据
//	IN dwLen---指定接收的PPP的HDLC数据的长度
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：接收PPP的HDLC数据
// 引用: 
// ********************************************************************
DWORD	PPP_InByHdlc( HANDLE hIfDev, LPBYTE pBufData, WORD wLen )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;
	LPBYTE		pFrmRecv;
	DWORD		wOffset;
	WORD		wLenReal;
	WORD		wMaxRecv;
	WORD		wStateRecv;
	WORD		wFCS;
	BYTE		bRead;

	//RETAILMSG(DEBUG_PPP,(TEXT(" PPP_InByHdlc: Enter wLen=[%d]\r\n"), wLen));
	//句柄安全检查
	if( !HANDLE_CHECK(pIfDev) )
	{
		RETAILMSG(DEBUG_PPP,(TEXT(" PPP_InByHdlc: leave Failed--1\r\n")));
		return 1;
	}
	//状态检查
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		RETAILMSG(DEBUG_PPP,(TEXT(" PPP_InByHdlc: leave Failed--2\r\n")));
		return 1;
	}
	if( !pIfDev->pInfoHdlc || !pIfDev->pInfoHdlc->pFrmRecv )
	{
		RETAILMSG(DEBUG_PPP,(TEXT(" PPP_InByHdlc: leave Failed--3\r\n")));
		return 1;
	}

	// 获取 状态
	pFrmRecv = pIfDev->pInfoHdlc->pFrmRecv;
	wLenReal = pIfDev->pInfoHdlc->wLenRecv;
	wMaxRecv = pIfDev->wMaxRecv;

	wStateRecv = pIfDev->pInfoHdlc->wStateRecv;
	wFCS = pIfDev->pInfoHdlc->wFCS;
	// 对接收的数据---分析、保存、处理
	for( wOffset=0; wOffset<wLen; wOffset++ )
	{
		bRead = pBufData[wOffset];
		//
		if( bRead==HDLC_FLAG )
		{
			if( wLenReal==0 )			//---准备开始
			{
				wStateRecv = FRM_RECV_START;
			}
			else
			{
				if( (wLenReal>=(2+2+sizeof(CTL_HDR))) && (wFCS==HDLC_FCS_FINAL) )
				{
					//
					wLenReal -= 2;//for FCS
					// Addr and Ctl
					if( *pFrmRecv==HDLC_ADDR )
					{
						PPP_DlIn( (HANDLE)pIfDev, pFrmRecv+2, (WORD)(wLenReal-2) );
					}
					else
					{
						PPP_DlIn( (HANDLE)pIfDev, pFrmRecv, wLenReal );
					}
				}
				else
				{
					RETAILMSG(DEBUG_PPP,(TEXT(" ^^^^^^^^PPP_InByHdlc: Failed-- 4444 , wLen=[%d], wLenReal=[%d], wFCS=[%x]\r\n"), wLen, wLenReal, wFCS));
				}

				//恢复状态，准备接收下个数据
				wLenReal = 0;			//---重新开始
				wStateRecv = 0;
			}
			//
			wFCS = HDLC_FCS_INIT;
		}
		else if( wStateRecv & FRM_RECV_START )
		{
			if( bRead==HDLC_ESC_ASYNC )//转义---
			{
				wStateRecv |= FRM_RECV_ESC;
			}
			else if( wLenReal < wMaxRecv )//
			{
				if( wStateRecv & FRM_RECV_ESC )
				{
					bRead ^= HDLC_ESC_XOR;
					wStateRecv &= ~FRM_RECV_ESC;
				}
				pFrmRecv[wLenReal] = bRead;//接收数据
				wLenReal ++;
				wFCS = HDLC_FCS16( wFCS, bRead );
			}
			else	//超载---重新开始
			{
				wLenReal = 0;
				wStateRecv = 0;
				RETAILMSG(DEBUG_PPP,(TEXT(" PPP_InByHdlc: Failed--5, wLen=[%d], wLenReal=[%d]\r\n"), wLen, wLenReal));
			}
		}
	}
	//
	pIfDev->pInfoHdlc->wLenRecv = wLenReal;
	pIfDev->pInfoHdlc->wStateRecv = wStateRecv;
	pIfDev->pInfoHdlc->wFCS = wFCS;

	//RETAILMSG(DEBUG_PPP,(TEXT(" PPP_InByHdlc: leave ok\r\n")));
	return 0;
}

