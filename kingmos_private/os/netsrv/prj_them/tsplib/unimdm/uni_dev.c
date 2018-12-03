/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：TSP Line Device 操作
版本号：  1.0.0
开发时期：2003-12-01
作者：    肖远钢
修改记录：
******************************************************/
#ifdef	_USRDLL
#include <windows.h>
#include <stdio.h>
#else	//_USRDLL
#include <ewindows.h>
#endif	//_USRDLL
#include <listunit.h>
//"\inc_app"
#include <ras.h>
#include <pcfile.h>
#include <gprscfg.h>
//"\inc_drv"
#include "tapi.h"
//"\inc_local"
#include "ras_call.h"
//"local"
#define RDT_MDM

#include "tsp_fun.h"
#include "uni_dev.h"
#include "uni_mdm.h"


/***************  全局区 定义， 声明 *****************/
/******************************************************/



// ********************************************************************
// 声明：
// 参数：
//	IN pszDrvReg-设备驱动的注册表位置
//	OUT pdwDevType---保存设备的类型
// 返回值：
//	成功TRUE
// 功能描述：获取设备的类型
// 引用: 
// ********************************************************************
BOOL	lineDev_CheckType( LPTSTR pszDrvReg, DWORD* pdwDevType )
{
	if( !RAPI_LookDriver( pszDrvReg, pdwDevType, 0, 0, 0 ) )
	{
		return FALSE;
	}
	if( RASDT_GETMAIN(*pdwDevType)!=RASDT_Modem )
	{
		return FALSE;
	}
	return TRUE;
}



// ********************************************************************
// 声明：
// 参数：
//	IN dwEnumID---指定的设备ID
//	IN lpRasOpenDev-设备的打开方式
//	IN pszDrvReg-设备驱动的注册表位置
// 返回值：
//	无
// 功能描述：创建指定设备ID的设备
// 引用: 
// ********************************************************************
BOOL	lineDev_Create( LINE_DEV* pLineDev )
{
	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pLineDev---指定的TSP设备
// 返回值：
//	成功，返回句柄
// 功能描述：打开 和 配置设备
// 引用: 
// ********************************************************************
BOOL	lineDev_Open( LINE_DEV* pLineDev )
{
	//1> 打开和配置端口，并进入OPENED状态，并进入工作状态
	EnterCriticalSection( &pLineDev->csDev );
	if( !UniMdm_OpenAndCfg( pLineDev ) )
	{
		LeaveCriticalSection( &pLineDev->csDev );
		return FALSE;
	}
	LeaveCriticalSection( &pLineDev->csDev );
	if( !lineDev_NewState( pLineDev, MDM_COMMAND ) )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_OPEN );
		return FALSE;
	}

	RASEVT_SEND( RASCS_PortOpened, RASERR_SUCCESS );
	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pLineDev---指定的TSP设备
//	IN pDialParam-设备的拨号参数
// 返回值：
//	成功，返回TRUE
// 功能描述：设备拨号
// 引用: 
// ********************************************************************
BOOL	lineDev_GetInfo( LINE_DEV* pLineDev, OUT TSP_DLINFO* pInfoDl )
{
	memset( pInfoDl, 0, sizeof(TSP_DLINFO) );
	if( !RAPI_LookDriver( pLineDev->szDrvReg, 0, &pInfoDl->dwMRU, 0, 0 ) )
	{
		pInfoDl->dwMRU = 1500;
		RETAILMSG(1,(TEXT("  lineDev_GetInfo: set !!! wMRU=[%d]!\r\n"), pInfoDl->dwMRU));
	}
	else
	{
		RETAILMSG(1,(TEXT("  lineDev_GetInfo: Get ### wMRU=[%d]!\r\n"), pInfoDl->dwMRU));
	}
	pInfoDl->dwDevType = pLineDev->dwDevType;

	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pLineDev---指定的TSP设备
//	IN pDialParam-设备的拨号参数
// 返回值：
//	成功，返回TRUE
// 功能描述：设备拨号
// 引用: 
// ********************************************************************
BOOL	lineDev_Dial( LINE_DEV* pLineDev, LPCSTR pszLocalNumber )
{
	RASENTRY	stRasEntry;
	DWORD		dwSize;
	TCHAR		pszDialNum[200+8];
	LPTSTR		pszTmp;
	PBYTE p = (PBYTE)&stRasEntry;
	//获取 拨号条目
	dwSize = sizeof(RASENTRY);
	memset( &stRasEntry, 0, dwSize );
	if( !RAPI_GetEntryProperties( NULL, pLineDev->szEntryName, &stRasEntry, &dwSize) )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_ENTRY );
		return FALSE;
	}

	// 启动拨号、并进行拨号初始化
	RASEVT_SEND( RASCS_ModemReady, ERROR_SUCCESS );
	if( !UniMdm_PrepareDial( pLineDev ) )
	{
		//RASEVT_SEND( RASCS_ConnectDevice, RASERR_TSP_INIT );
		return FALSE;
	}

	//

	if( RASDT_GETSUB(pLineDev->dwDevType)==RASDT_SUB_GPRS )
	{
#if GPRS_MODULE == MC35I_MODULE
		strcpy( pszDialNum, "AT+CGDATA=\"PPP\",1\r" );
#elif GPRS_MODULE == WAVECOM_MODULE
		strcpy( pszDialNum, "AT+CGDATA=1\r" );
#else
	#error not define GPRS_MODULE in tapi.h!!!
#endif
	}
	else
	{
		// 获取 拨号信息
		strcpy( pszDialNum, "ATD" );
		pszTmp = pszDialNum + strlen(pszDialNum);
		if( !RAPI_GenNum(&stRasEntry, pszLocalNumber, pszTmp, 200) )
		{
			RETAILMSG(1,(TEXT("  lineDev_Dial: Wrong for RAPI_GenNum!\r\n")));
			RASEVT_SEND( RASCS_ConnectDevice, RASERR_TSP_INIT );
			return FALSE;
		}
		strcat( pszTmp, "\r" );
		//sprintf( pszDialNum, "ATD%s\r", pDialParam->szPhoneNumber );
	}

	//开始拨号
	RASEVT_SEND( RASCS_ConnectDevice, ERROR_SUCCESS );
	pLineDev->fDialed = TRUE;
	return UniMdm_MakeCall( pLineDev, pszDialNum, stRasEntry.dwCntRedial );
}

// ********************************************************************
// 声明：
// 参数：
//	IN hLineDev---指定的TSP设备
//	IN hIfDl---TSP设备的数据的 处理者句柄
//	IN pFnIn---TSP设备的数据的 处理者函数
// 返回值：
//	成功，返回TRUE
// 功能描述：设置 TSP设备的数据的 处理者，并切换到MDM_DATA
// 引用: 
// ********************************************************************
void	lineDev_SetRecv( LINE_DEV* pLineDev, HANDLE hIfDl, FNPROIN pFnIn )
{
	//pLineDev->hIfDl = hIfDl;
	//pLineDev->pFnDlIn = pFnIn;
	//将modem切换到  数据状态
	lineDev_NewState( pLineDev, MDM_DATA );
}


// ********************************************************************
// 声明：
// 参数：
//	IN hLineDev---指定的TSP设备
//	IN pBuf---要发送的数据
//	IN dwLen---要发送的数据的长度
// 返回值：
//	成功，返回TRUE
// 功能描述：TSP设备的数据 发送函数
// 引用: 
// ********************************************************************
DWORD	lineDev_Send( LINE_DEV* pLineDev, LPBYTE pBuf, DWORD dwLen )
{
	HANDLE		hPort;
	DWORD		dwTmp;

	hPort = pLineDev->hPort;
	if( hPort && hPort!=INVALID_HANDLE_VALUE )
	{
		if( !ver_WriteFile(hPort, pBuf, dwLen, &dwTmp, NULL) )
		{
			return MR_FAILURE;
		}
		else if( dwLen!=dwTmp )
		{
			return MR_FAILURE;
		}
	}
	else
	{
		return MR_FAILURE;
	}

	return MR_OK;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pLineDev---指定的TSP设备
// 返回值：
//	无
// 功能描述：停用TSP设备
// 引用: 
// ********************************************************************
void	lineDev_Stop0( LINE_DEV* pLineDev )
{
	// LINE_DEV去掉所有与PPP相关的信息
	//pLineDev->pFnDlIn = NULL;
	EnterCriticalSection( &pLineDev->csDev );
	if( pLineDev->hPort && pLineDev->hPort!=INVALID_HANDLE_VALUE )
	{
		if( pLineDev->fDialed )
		{
			UniMdm_SwitchToCmd( pLineDev );
			pLineDev->fDialed = FALSE;
		}
		ver_CloseHandle( pLineDev->hPort );
		pLineDev->hPort = NULL;
	}
	LeaveCriticalSection( &pLineDev->csDev );
	Sleep( 5 );
}

// ********************************************************************
// 声明：
// 参数：
//	IN pLineDev---指定的TSP设备
// 返回值：
//	无
// 功能描述：停用TSP设备
// 引用: 
// ********************************************************************
void	lineDev_Stop( LINE_DEV* pLineDev )
{
#if 0
	// LINE_DEV去掉所有与PPP相关的信息
	//pLineDev->pFnDlIn = NULL;
	EnterCriticalSection( &pLineDev->csDev );
	if( pLineDev->hPort && pLineDev->hPort!=INVALID_HANDLE_VALUE )
	{
		if( pLineDev->fDialed )
		{
			pLineDev->fDialed = FALSE;
			UniMdm_HangupCall( pLineDev );
		}
		ver_CloseHandle( pLineDev->hPort );
		pLineDev->hPort = NULL;
	}
	LeaveCriticalSection( &pLineDev->csDev );
	Sleep( 5 );
#endif
}

// ********************************************************************
// 声明：
// 参数：
//	IN pLineDev---指定的TSP设备
// 返回值：
//	成功，返回TRUE
// 功能描述：释放拨号设备
// 引用: 
// ********************************************************************
void	lineDev_Free( LINE_DEV* pLineDev )
{
	if( pLineDev->rasOpen.fOpenName )
	{
		if( pLineDev->hPort && pLineDev->hPort!=INVALID_HANDLE_VALUE )
		{
			ver_CloseHandle( pLineDev->hPort );
			pLineDev->hPort = NULL;
		}
	}
	if( pLineDev->hEvtCmd )
	{
		CloseHandle( pLineDev->hEvtCmd );
	}
	DeleteCriticalSection( &pLineDev->csDev );

	HANDLE_FREE( pLineDev );
}
