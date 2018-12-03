/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _TSP_UNIMODEM_H_
#define _TSP_UNIMODEM_H_

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------
//动态库 的特征
//---------------------------------------------------
//
#ifdef	_USRDLL

//for PC dll request
#ifdef UNIMODEM_EXPORTS
#define UNIMODEM_API __declspec(dllexport)
#else
#define UNIMODEM_API __declspec(dllimport)
#endif

#define GENERIC_DATA		0

#else	//_USRDLL

//for Kingmos
#define UNIMODEM_API

#endif	//_USRDLL


//---------------------------------------------------
// 定义
//---------------------------------------------------

#ifdef	RDT_PPPOE

#ifndef	ETHER_MAC_LEN
#define	ETHER_MAC_LEN			6
#endif	//ETHER_MAC_LEN

#define	MAX_HOSTUNIQ			16

#endif

//---------------------------------------------------
//TSP 设备信息 定义
//---------------------------------------------------

typedef	struct	_LINE_DEV
{
	HANDLE_THIS( _LINE_DEV );

	//TSP设备链表
	LIST_UNIT			hListDev;

	//Line设备信息(配置)
	DWORD				dwDevType;
	DWORD				dwEnumID;	//在 ras devices内 唯一分配
	RASDEVOPEN			rasOpen;
	TCHAR				szDrvReg[ RAS_MaxDrvReg+8 ];
	//Line设备操作信息
	HANDLE				hPort;
    CRITICAL_SECTION	csDev;
	DWORD				dwDevState;
	BOOL				fDialed;
	HANDLE				hEvtCmd;	//命令通知
	DWORD				dwBaud;

	//拨号信息
	TCHAR				szEntryName[ RAS_MaxEntryName + 8 ];

#ifdef	RDT_PPPOE
	WORD				wSID;
	WORD				wXXX;
	WORD				wLenName;
	WORD				wLenCookie;
	LPBYTE				pAcName;
	LPBYTE				pAcCookie;
	BYTE				bSrcMAC[ ETHER_MAC_LEN ];
	BYTE				bDesMAC[ ETHER_MAC_LEN ];
	BYTE				bHostUniq[ MAX_HOSTUNIQ ];

#else
	DWORD				dwResWrite;
#define	LEN_BUFRES			1024//512
	char				szBufRes[LEN_BUFRES+4];

#endif

	//Ras设备信息
	HANDLE				hRasDev;

	//数据上传信息

#ifdef	RDT_PPPOE
	FNDLOUT				pFnDlOut;
#endif

//#ifdef	RDT_MDM
//	HANDLE				hIfDl;
//	FNPROIN				pFnDlIn;
//#endif

} LINE_DEV;

//dwDevState
#define	MDM_DOWN			0x10000000
#define	MDM_IDLE			0x20000000
#define	MDM_OPENED			0x80000000

#ifdef	RDT_PPPOE
#define	MDM_PADO			(0x00000002)
#endif

#define	MDM_COMMAND			(0x00000014)
#define	MDM_DATA			(0x00000015)


//---------------------------------------------------
//TSP 全局信息 定义
//---------------------------------------------------
typedef	struct	_TSP_GLOBAL
{
	HANDLE_THIS( _TSP_GLOBAL );

	HINSTANCE			hInst;
	//通知RAS 的函数
	//FNRASNOTIFY		pFnNotifyRas;
	
	//所有的 TSP设备
	LIST_UNIT			hListDev;
	CRITICAL_SECTION	csListDev;

} TSP_GLOBAL;


//---------------------------------------------------
//全局引用 声明
//---------------------------------------------------
//

//发送事件
//#define	RASEVT_SEND(dwEvt, dwErr)	(*g_lpGlobalUni->pFnNotifyRas)( pLineDev->hRasDev, dwEvt, dwErr )
extern	void	RasIf_NotifyEvt( HANDLE hRasDev, DWORD dwRasCnnStatus, DWORD dwErr );
#define	RASEVT_SEND(dwEvt, dwErr)	RasIf_NotifyEvt( pLineDev->hRasDev, dwEvt, dwErr )

//状态
extern	BOOL	lineDev_NewState( LINE_DEV* pLINE_DEV, DWORD dwDevState );
extern	BOOL	lineDev_IsDown( LINE_DEV* pLINE_DEV );


#ifdef __cplusplus
}	
#endif

#endif	//_TSP_UNIMODEM_H_

