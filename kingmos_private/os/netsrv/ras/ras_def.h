/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _RAS_DEF_H_
#define _RAS_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------
// 宏定义
//------------------------------------------------------
#define	TIMER_STATBPS			1200

#define	RAS_BPS_1

//------------------------------------------------------
// 定义 结构
//------------------------------------------------------

// TSPI DLL的信息
typedef	struct	_TSPINFO
{
	HANDLE_THIS( _TSPINFO );

	LIST_UNIT			hListTsp;
	//操作信息
	LONG				nRefCnt;
	//动态库信息
#ifdef RAS_LOAD_TSPDLL
	HANDLE				hDll;
#endif
	TCHAR				pszDllName[MAX_PATH];
	TSPITBL*			pTspTbl;

} TSPINFO;


// RAS拨号设备 的信息
typedef	struct	_RAS_DEV
{
	HANDLE_THIS( _RAS_DEV );

	LIST_UNIT			hListRasDev;	//设备链表

	//设备信息:
	DWORD				dwEnumID;	//在 ras_tapi.c内 唯一分配
	DWORD				dwDevType;
	TCHAR				szDrvReg[ RAS_MaxDrvReg+8 ];
	RASDEVOPEN			rasOpen;

	//操作信息：
	WORD				wFlag;
	WORD				wDevOp;
	CRITICAL_SECTION	csRasDev;
	DWORD				dwDevStatus;
	HANDLE				hEvtWaitCnn;

	//拨号条目信息
	TCHAR				szEntryName[ RAS_MaxEntryName + 4 ];

	//RAS APP信息
	DWORD				dwNotifierType;
	LPVOID				notifier;
	//TSP DLL 和 TSP 设备  的信息
	TSPINFO*			lpTspInfo;
	HANDLE				hLineDev;
	//PPP 操作信息
	HANDLE				hIfDl;
	FNPRONOTIFY			pFnDlNotify;
	FNPROIN				pFnDlIn;

	//
	//统计信息
	//
	//统计---连接时间
	DWORD				dwTickCnn;
	//统计---连接后 收到/发送 的字节数
	DWORD				dwBytesXmited;
	DWORD				dwBytesRcved;
	//DWORD				dwCompressionRatioIn;
	//DWORD				dwCompressionRatioOut;
	//DWORD				dwConnectDuration;

	//统计---速度
#ifdef RAS_BPS_1
	HANDLE				hThrdBps;
	HANDLE				hEvtBps;

	DWORD				dwBytesRx_Bps;  //每个统计周期内 接收的 Bytes
	DWORD				dwTickRx_Bps;   //每次统计周期的 开始
	DWORD				dwRxBps;        //统计接收速度

	DWORD				dwBytesTx_Bps;  //每个统计周期内 发送的 Bytes
	DWORD				dwTickTx_Bps;   //每次统计周期的 开始
	DWORD				dwTxBps;        //统计发送速度

#else
	DWORD				dwBytesRx_Bps;  //每个统计周期内 接收的 Bytes
	DWORD				dwTickRx_Bps;   //每次统计周期的 开始

	DWORD				dwBytesTx_Bps;  //每个统计周期内 发送的 Bytes
	DWORD				dwTickTx_Bps;   //每次统计周期的 开始

#endif

} RAS_DEV;

//wFlag
#define	RASDF_EXIT				0x8000

//wDevOp
#define	RASOP_HANGUP			0x0010
#define	RASOP_FREE				0x0020
#define	RASOP_WAITMS			0x0040

#define	RASOP_NO_BPS			0x0100


// RAS 的全局结构
typedef	struct	_RAS_GLOBAL
{
	HANDLE_THIS( _RAS_GLOBAL );

	//所有注册的 RAS设备
	CRITICAL_SECTION	csListRasDev;	//所有设备的操作
	LIST_UNIT			hListRasDev;
	//所有加载的 Tsp Dll信息
	CRITICAL_SECTION	csListTsp;
	LIST_UNIT			hListTsp;

	//操作信息
	DWORD				dwEnumNewID;
	HWND				hWndNetMgr;

} RAS_GLOBAL;

extern	RAS_GLOBAL*		g_lpGlobalRas;


#ifdef __cplusplus
}	
#endif

#endif	//_RAS_DEF_H_
