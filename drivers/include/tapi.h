/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _TAPI_H_
#define _TAPI_H_

#ifndef	_RAS_H_
#include <ras.h>
#endif

#ifndef	_NETIF_COMM_H_
#include "netif_comm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------
//  定义
// ----------------------------------------------------------

extern	DWORD	RasIf_UpData( HANDLE hRasDev, LPBYTE pBuf, DWORD dwLen );

//RAS设备配置
#define	MAX_CFG_BUF			128
typedef	struct	_RASDEVCFG
{
	WORD		wVersion;
	WORD		wSize;

	DCB			dcb;
	BYTE		pBufCfg[ MAX_CFG_BUF ];

} RASDEVCFG;


//TSP设备的 打开方式
#define	MAX_RASOPENNAME		20
typedef	struct	_RASDEVOPEN
{
	BOOL		fOpenName;
	union
	{
		HANDLE	hDev;
		TCHAR	szOpenName[ MAX_RASOPENNAME+8 ];
	};

} RASDEVOPEN;


//通知 RAS的函数
typedef	void	(*FNRASNOTIFY)( HANDLE hRasDev, DWORD dwRasCnnStatus, DWORD dwErr );
//dwEvent
#define	PROEVT_DEV_INSERT	0x10000001	//设备 插入---用于 RAS设备注册者 发送
#define	PROEVT_DEV_REMOVE	0x10000002	//设备 拔走---用于 RAS设备注册者 发送

#define	PROEVT_DEV_LINKED	0x10000010	//设备层 已经连接成功(如拨号成功或获取AC成功)
#define	PROEVT_DEV_STOP		0x10000020	//停用 设备层
#define	PROEVT_DEV_CLOSE	0x10000030	//释放 设备层


//TSPI_OpenDev 的参数
typedef	struct	_PARAM_OPENDEV
{
	HANDLE		hRasDev;
	LPTSTR		pszEntry;
	RASENTRY*	lpEntry;
	//

} PARAM_OPENDEV;


//TSP设备提供的链路层信息
typedef	struct	_TSP_DLINFO
{
	DWORD		dwMRU;

	//
	DWORD		dwDevType;
	//
	DWORD		dwNetType; //当是GPRS时 的 设备拨号信息
	//
	LPBYTE		pMAC;      //当是ETHER时 的 设备拨号信息
	DWORD		dwLenMAC;

} TSP_DLINFO;



//TSPI函数表
typedef struct _TSPITBL
{
	//版本 和 大小
	WORD	wSize;
	WORD	wVersion;

	//设备操作
	HANDLE	(*TSPI_CreateDev)( DWORD dwEnumID, LPTSTR pszDrvReg, RASDEVOPEN* lpRasOpenDev );
	BOOL	(*TSPI_OpenDev)( HANDLE hLineDev, PARAM_OPENDEV* pParamOpen );
	//void	(*TSPI_CloseDev)( HANDLE hLineDev );
	DWORD	(*TSPI_lineNotify)( HANDLE hLineDev, DWORD dwEvent );
	//拨号
	BOOL	(*TSPI_lineDial)( HANDLE hLineDev, LPCSTR pszLocalNumber );
	//拨号成功后，获取相关信息 和 设置接收函数
	BOOL	(*TSPI_lineGetInfo)( HANDLE hLineDev, OUT TSP_DLINFO* pInfoDl );
	void	(*TSPI_lineSetRecv)( HANDLE hLineDev, HANDLE hIfDl, FNPROIN pFnIn );

	//TSP设备发送函数
	DWORD	(*TSPI_lineSend)( HANDLE hLineDev, LPBYTE pBuf, DWORD dwLen );

} TSPITBL;


//TSP DLL 的输出函数
typedef	 void	(*TSPI_GETTABLE)( TSPITBL** ppTspiTbl, HINSTANCE hInst, FNRASNOTIFY pNotifyRas );

#if 0
//modem操作的统一返回值: lineDev_SendCmd / TSPI_SendData / 
#define	MR_OK			0x00
#define	MR_FAILURE		0x01
#define	MR_TIMEOUT		0x02
#define	MR_UNKNOWN		0x03
#define	MR_DOWN			0x04

#define	MR_CONNECT		0x13
#define	MR_NOANSWER		0x14
#define	MR_NODIALTONE	0x15
#define	MR_NOCARRIER	0x16
#define	MR_BUSY			0x17
#endif


#ifdef __cplusplus
}	
#endif

#endif	//_TAPI_H_
