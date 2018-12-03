/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：管理 RAS设备和其TSP
版本号：  1.0.0
开发时期：2003-12-01
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <ras.h>
#ifdef RAS_LOAD_TSPDLL
#include <pcfile.h>
#endif
//"\inc_drv"
#include "tapi.h"
//"\inc_local"
#include "netif_dl_ras.h"
#include "ras_call.h"
//"local"
#include "ras_def.h"
#include "ras_dev.h"


/***************  全局区 定义， 声明 *****************/

//-------------------------------------------------
#ifdef RAS_LOAD_TSPDLL
#ifdef	EML_WIN32
#define PATH_TSPDLL			"E:\\kingmos\\build\\unimodem\\Debug\\%s"
#else
#define PATH_TSPDLL			"\\system\\%s"
#endif

#else
extern	void	TSPI_GetTable( OUT TSPITBL** ppTspiTbl, HINSTANCE hInst, FNRASNOTIFY pNotifyRas );

#endif

//-------------------------------------------------
extern	BOOL	RasD_LookActive( LPCTSTR pszActiveReg, OUT LPTSTR pszDrvReg, OUT LPTSTR pszOpenName, DWORD dwLenName );

/******************************************************/

// ------------------------------------------------------
// 拨号设备用的API函数----注册和枚举
// ------------------------------------------------------

// ********************************************************************
// 声明：
// 参数：
//	IN pszActiveReg-指定设备的Active注册表
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：注册Modem类型的拨号设备
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_RegisterModem( LPCTSTR pszActiveReg )
{
	RAS_DEV*	pRasDev;
	DWORD		dwDevType;
	TCHAR		szDrvReg[RAS_MaxDrvReg+8];
	TCHAR		szOpenName[ MAX_RASOPENNAME+8 ];
	TCHAR		szTsp[MAX_PATH];

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	//参数检查
	RETAILMSG( 1, (TEXT("  \r\n RAPI_RegisterModem: enter!\r\n")) );
	if( !pszActiveReg || !pszActiveReg[0] )
	{
		RETAILMSG( 1, (TEXT("  \r\n RAPI_RegisterModem: leave fail---1\r\n")) );
		return FALSE;
	}
	// 获取 设备信息：szDrvReg , szOpenName , TSP, DevType
	if( !RasD_LookActive( pszActiveReg, szDrvReg, szOpenName, MAX_RASOPENNAME ) )
	{
		RETAILMSG( 1, (TEXT("  \r\n RAPI_RegisterModem: leave fail---2\r\n")) );
		return FALSE;
	}
	if( !RAPI_LookDriver( szDrvReg, &dwDevType, 0, szTsp, 0 ) )
	{
		RETAILMSG( 1, (TEXT("  \r\n RAPI_RegisterModem: leave fail---3\r\n")) );
		return FALSE;
	}
	//检查拨号类型
	if( RASDT_GETMAIN(dwDevType)!=RASDT_Modem )
	{
		RETAILMSG( 1, (TEXT("  \r\n RAPI_RegisterModem: leave fail---4\r\n")) );
		return FALSE;
	}
	//分配结构
	if( !RasD_AllocDev( &pRasDev ) )
	{
		RETAILMSG( 1, (TEXT("  \r\n RAPI_RegisterModem: leave fail---5\r\n")) );
		return FALSE;
	}
	//设备信息
	pRasDev->dwDevType = dwDevType;
	pRasDev->rasOpen.fOpenName = TRUE;
	strcpy( pRasDev->rasOpen.szOpenName, szOpenName );
	strcpy( pRasDev->szDrvReg, szDrvReg );

	//添加 设备
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pRasDev->dwEnumID = (++g_lpGlobalRas->dwEnumNewID);
	List_InsertTail( &g_lpGlobalRas->hListRasDev, &pRasDev->hListRasDev );
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );

	RETAILMSG( 1, (TEXT("  \r\n RAPI_RegisterModem: leave ok!\r\n")) );
	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pszActiveReg-指定设备的Active注册表
// 返回值：
//	无
// 功能描述：取消注册PPPoE类型的拨号设备
// 引用: 
// ********************************************************************
void	WINAPI	RAPI_DeregisterModem( LPCTSTR pszActiveReg )
{
	TCHAR		szDrvReg[RAS_MaxDrvReg+8];

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return ;
	}
	//参数检查
	if( !pszActiveReg || pszActiveReg[0] )
	{
		return ;
	}
	//Get szDrvReg
	if( !RasD_LookActive( pszActiveReg, szDrvReg, 0, 0 ) )
	{
		return ;
	}
	//Deregister device
	RasD_DeregisterDev( szDrvReg );
}


// ********************************************************************
// 声明：
// 参数：
//	IN pszDrvReg-指定设备的注册表
//	IN hDev-指定设备的操作句柄
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：注册PPPoE类型的拨号设备
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_RegisterPPPoEd( LPCTSTR pszDrvReg, HANDLE hDev )
{
	RAS_DEV*	pRasDev;
	DWORD		dwDevType;
	TCHAR		szTsp[MAX_PATH];

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	// 获取 设备信息：szDrvReg , szOpenName , TSP, DevType
	if( !RAPI_LookDriver( pszDrvReg, &dwDevType, 0, szTsp, 0 ) )
	{
		return FALSE;
	}
	if( RASDT_GETMAIN(dwDevType)!=RASDT_PPPoE )
	{
		return FALSE;
	}
	//分配结构
	if( !RasD_AllocDev( &pRasDev ) )
	{
		return FALSE;
	}
	//设备信息
	pRasDev->dwDevType = dwDevType;
	pRasDev->rasOpen.fOpenName = FALSE;
	pRasDev->rasOpen.hDev = hDev;

	//添加 设备
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pRasDev->dwEnumID = (++g_lpGlobalRas->dwEnumNewID);
	List_InsertTail( &g_lpGlobalRas->hListRasDev, &pRasDev->hListRasDev );
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );

	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pszDrvReg-指定设备的注册表
// 返回值：
//	无
// 功能描述：取消注册PPPoE类型的拨号设备
// 引用: 
// ********************************************************************
void	WINAPI	RAPI_DeregisterPPPoEd( LPCTSTR pszDrvReg )
{
	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return ;
	}
	RasD_DeregisterDev( pszDrvReg );
}

// ********************************************************************
// 声明：
// 参数：
//	OUT lpRasDevinfo-保存枚举到的设备信息的数组
//	IN OUT lpdwCntDevices-指定枚举到的设备的个数
// 返回值：
//	无
// 功能描述：枚举拨号设备
// 引用: 
// ********************************************************************
void	WINAPI	RAPI_EnumDevices( OUT RASDEVINFO* lpRasDevinfo, IN OUT DWORD* lpdwCntDevices )
{
	RAS_DEV*		pRasDev_Find = NULL;
	DWORD			nCntDev;
	RASDEVINFO*		pCopy;
	PLIST_UNIT		pHeader;
	PLIST_UNIT		pUnit;

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		*lpdwCntDevices = 0;
		return ;
	}
	//参数检查
	if( !lpdwCntDevices )
	{
		return ;
	}
	//
	nCntDev = 0;
	pCopy = lpRasDevinfo;
	pHeader = &g_lpGlobalRas->hListRasDev;
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pRasDev_Find = LIST_CONTAINER( pUnit, RAS_DEV, hListRasDev);
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pRasDev_Find) )
		{
			break;
		}
		if( (pRasDev_Find->wFlag & RASDF_EXIT) || (pRasDev_Find->wDevOp & RASOP_FREE) )
		{
			continue;
		}
		//
		if( lpRasDevinfo )
		{
			if( nCntDev<(*lpdwCntDevices) )
			{
				pCopy->dwDevType = pRasDev_Find->dwDevType;
				pCopy->dwDevStatus = pRasDev_Find->dwDevStatus;
				strcpy( pCopy->szDrvReg, pRasDev_Find->szDrvReg);
				pCopy->szDeviceName[0] = 0;
				RAPI_LookDriver( pRasDev_Find->szDrvReg, 0, 0, 0, pCopy->szDeviceName );

				//save this 
				pCopy ++;
				nCntDev ++;
			}
		}
		else
		{
			nCntDev ++;
		}
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );

	//
	(*lpdwCntDevices) = nCntDev;

	return ;
}

BOOL	WINAPI	RAPI_CheckDev( DWORD dwChkFlag, DWORD dwChkDevType )
{
	BOOL			fFind = FALSE;
	RAS_DEV*		pRasDev_Find = NULL;
	PLIST_UNIT		pHeader;
	PLIST_UNIT		pUnit;

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	//
	pHeader = &g_lpGlobalRas->hListRasDev;
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pRasDev_Find = LIST_CONTAINER( pUnit, RAS_DEV, hListRasDev);
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pRasDev_Find) )
		{
			break;
		}
		if( (pRasDev_Find->wFlag & RASDF_EXIT) || (pRasDev_Find->wDevOp & RASOP_FREE) )
		{
			continue;
		}

		//
		//RETAILMSG(1,(TEXT("\r\n RAPI_CheckDev: dwDevStatus[0x%x]\r\n"), pRasDev_Find->dwDevStatus));
		if( dwChkFlag & RASDT_CHKLINK )
		{
			if( pRasDev_Find->dwDevStatus!=RASDEV_LINK )
			{
				continue;
			}
		}
		if( !dwChkDevType )
		{
			fFind = TRUE;
			break;
		}
		//
		if( (dwChkFlag & RASDT_CHKTYPE)==RASDT_CHKTYPE )
		{
			if( pRasDev_Find->dwDevType==dwChkDevType )
			{
				fFind = TRUE;
				break;
			}
		}
		else
		{
			if( dwChkFlag & RASDT_CHKMAIN )
			{
				if( RASDT_GETMAIN(pRasDev_Find->dwDevType)==RASDT_GETMAIN(dwChkDevType) )
				{
					fFind = TRUE;
					break;
				}
			}
			else if( dwChkFlag & RASDT_CHKSUB )
			{
				if( RASDT_GETSUB(pRasDev_Find->dwDevType)==RASDT_GETSUB(dwChkDevType) )
				{
					fFind = TRUE;
					break;
				}
			}
		}
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );

	//RETAILMSG(1,(TEXT("\r\n RAPI_CheckDev: find[%d]\r\n"), fFind));
	return fFind;
}



// ========================================================================================== //
// .......................................................................................... //
// ========================================================================================== //


// ********************************************************************
// 声明：
// 参数：
//	IN pszDrvReg-指定设备的注册表
// 返回值：
//	无
// 功能描述：取消注册拨号设备
// 引用: 
// ********************************************************************
DWORD	RasD_DeregisterDev( LPCTSTR pszDrvReg )
{
	RAS_DEV*	pRasDev;

	//GET DEVICE
	if( !RasD_FindDev( &pRasDev, pszDrvReg ) )
	{
		return 1;
	}
	pRasDev->wDevOp &= ~RASOP_WAITMS;	//注意!!!
	//
	return RasIf_HangUp( (HANDLE)pRasDev, RASH_OP_FREEDEV, RASCS_Disconnected, RASERR_SUCCESS );
}


// ********************************************************************
// 声明：
// 参数：
//	OUT ppRasDev-保存分配的设备
// 返回值：
//	无
// 功能描述：分配1个设备
// 引用: 
// ********************************************************************
BOOL	RasD_AllocDev( RAS_DEV** ppRasDev )
{
	RAS_DEV*	pRasDev;
	
	*ppRasDev = NULL;
	//
	pRasDev = (RAS_DEV*)HANDLE_ALLOC( sizeof(RAS_DEV) );
	if( !pRasDev )
	{
		return FALSE;
	}
	HANDLE_INIT( pRasDev, sizeof(RAS_DEV) );
	List_InitHead( &pRasDev->hListRasDev );
	InitializeCriticalSection( &pRasDev->csRasDev );
	pRasDev->dwDevStatus = RASDEV_IDLE;
	//
#ifdef RAS_BPS_1
	pRasDev->hEvtBps = CreateEvent( NULL, FALSE, FALSE, NULL );
#endif
	pRasDev->hEvtWaitCnn = CreateEvent( NULL, FALSE, FALSE, NULL );

	//
	*ppRasDev = pRasDev;
	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pRasDev-指定的RAS设备
// 返回值：
//	无
// 功能描述：释放1个设备
// 引用: 
// ********************************************************************
void	RasD_FreeDev( RAS_DEV* pRasDev )
{
#ifdef RAS_BPS_1
	if( pRasDev->hEvtBps )
	{
		CloseHandle( pRasDev->hEvtBps );
	}
	if( pRasDev->hThrdBps )
	{
		CloseHandle( pRasDev->hThrdBps );
	}
#endif
	if( pRasDev->hEvtWaitCnn )
	{
		CloseHandle( pRasDev->hEvtWaitCnn );
	}
	DeleteCriticalSection( &pRasDev->csRasDev );
	HANDLE_FREE( pRasDev );
}


// ********************************************************************
// 声明：
// 参数：
//	OUT ppRasDev-保存寻找的设备
//	IN pszDrvReg-指定设备的注册表
// 返回值：
//	无
// 功能描述：寻找1个设备
// 引用: 
// ********************************************************************
BOOL	RasD_FindDev( OUT RAS_DEV** ppRasDev, LPCTSTR pszDrvReg )
{
	RAS_DEV*	pRasDev_Find;
	PLIST_UNIT	pHeader;
	PLIST_UNIT	pUnit;

	*ppRasDev = NULL;
	pHeader = &g_lpGlobalRas->hListRasDev;
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pRasDev_Find = LIST_CONTAINER( pUnit, RAS_DEV, hListRasDev);
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pRasDev_Find) )
		{
			break;
		}
		if( (pRasDev_Find->wFlag & RASDF_EXIT) || (pRasDev_Find->wDevOp & RASOP_FREE) )
		{
			continue;
		}

		//
		if( stricmp(pRasDev_Find->szDrvReg, pszDrvReg)==0 )
		{
			pRasDev_Find->wDevOp |= RASOP_WAITMS;	//注意!!!
			*ppRasDev = pRasDev_Find;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );

	return (*ppRasDev)?TRUE:FALSE;
}

// ********************************************************************
// 声明：
// 参数：
//	OUT ppTspInfo-保存寻找的TSP
//	IN pszDrvReg-指定TSP DLL NAME
//	IN fRef-指定是否引用
// 返回值：
//	无
// 功能描述：寻找1个TSP
// 引用: 
// ********************************************************************
BOOL	RasT_FindTsp( OUT TSPINFO** ppTspInfo, LPCTSTR pszDllName, BOOL fRef )
{
	TSPINFO*	pTspInfo_Find;
	PLIST_UNIT	pHeader;
	PLIST_UNIT	pUnit;

	*ppTspInfo = NULL;
	pHeader = &g_lpGlobalRas->hListTsp;
	EnterCriticalSection( &g_lpGlobalRas->csListTsp );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pTspInfo_Find = LIST_CONTAINER( pUnit, TSPINFO, hListTsp);
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pTspInfo_Find) )
		{
			break;
		}

		//
		if( stricmp(pTspInfo_Find->pszDllName, pszDllName)==0 )
		{
			if( fRef )
			{
				pTspInfo_Find->nRefCnt ++;
			}
			*ppTspInfo = pTspInfo_Find;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListTsp );

	return (*ppTspInfo)?TRUE:FALSE;
}

// ********************************************************************
// 声明：
// 参数：
//	OUT ppTspInfo-保存加载的TSP
//	IN pszDrvReg-指定TSP DLL NAME
//	IN fRef-指定是否引用
// 返回值：
//	无
// 功能描述：加载1个TSP，并初始化该DLL
// 引用: 
// ********************************************************************
BOOL	RasT_LoadTsp( OUT TSPINFO** ppTspInfo, LPCTSTR pszDllName, BOOL fRef )
{
	TSPINFO*		pTspInfo;
	TSPI_GETTABLE	pFnGetTable;
#ifdef RAS_LOAD_TSPDLL
	TCHAR			pszDllPath[MAX_PATH];
#endif

	*ppTspInfo = NULL;
	//malloc
	pTspInfo = (TSPINFO*)HANDLE_ALLOC( sizeof(TSPINFO) );
	if( !pTspInfo )
	{
		return FALSE;
	}
	HANDLE_INIT( pTspInfo, sizeof(TSPINFO) );
	if( fRef )
	{
		pTspInfo->nRefCnt ++;
	}
	//LoadLibrary
#ifdef RAS_LOAD_TSPDLL
	sprintf( pszDllPath, PATH_TSPDLL, pszDllName );
	pTspInfo->hDll = ver_LoadLibrary( pszDllPath );
	if( !pTspInfo->hDll )
	{
		HANDLE_FREE( pTspInfo );
		return FALSE;
	}
	//get fun table
	pFnGetTable = (TSPI_GETTABLE)ver_GetProcAddress( pTspInfo->hDll, "TSPI_GetTable" );
	if( !pFnGetTable )
	{
		ver_FreeLibrary( pTspInfo->hDll );
		HANDLE_FREE( pTspInfo );
		return FALSE;
	}
#else
	pFnGetTable = TSPI_GetTable;
#endif
	(*pFnGetTable)( &pTspInfo->pTspTbl, 0, RasIf_NotifyEvt );
	if( !pTspInfo->pTspTbl || !pTspInfo->pTspTbl->TSPI_OpenDev )
	{
#ifdef RAS_LOAD_TSPDLL
		ver_FreeLibrary( pTspInfo->hDll );
#endif
		HANDLE_FREE( pTspInfo );
		return FALSE;
	}
	strcpy( pTspInfo->pszDllName, pszDllName );
	//添加到 g_hNetGlobal
	EnterCriticalSection( &g_lpGlobalRas->csListTsp );
	List_InsertTail( &g_lpGlobalRas->hListTsp, &pTspInfo->hListTsp );
	LeaveCriticalSection( &g_lpGlobalRas->csListTsp );

	*ppTspInfo = pTspInfo;
	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pTspInfo-指定的TSP
// 返回值：
//	无
// 功能描述：释放1个TSP
// 引用: 
// ********************************************************************
void	RasT_FreeTsp( TSPINFO* lpTspInfo )
{
	BOOL		fFreeTsp = FALSE;
	
	if( !HANDLE_CHECK(lpTspInfo) )
	{
		return ;
	}
	EnterCriticalSection( &g_lpGlobalRas->csListTsp );
	lpTspInfo->nRefCnt --;
	if( lpTspInfo->nRefCnt<=0 )
	{
		fFreeTsp = TRUE;
		List_RemoveUnit( &lpTspInfo->hListTsp );
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListTsp );
	//
	if( fFreeTsp )
	{
#ifdef RAS_LOAD_TSPDLL
		ver_FreeLibrary( lpTspInfo->hDll );
#endif
		HANDLE_FREE( lpTspInfo );
	}
}


// ********************************************************************
// 声明：
// 参数：
//	IN pRasDev-拨号RAS设备的句柄
// 返回值：
//	无
// 功能描述：释放 1个拨号设备
// 引用: 
// ********************************************************************
void	RasT_FreeDev( RAS_DEV* pRasDev )
{
	int		i;

	//解脱自己
	pRasDev->wFlag |= RASDF_EXIT;
	Sleep( 1 );

	//EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	//List_RemoveUnit( &pRasDev->hListDev );
	//LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );
	//
#ifdef RAS_BPS_1
	if( pRasDev->hEvtBps )
	{
		SetEvent( pRasDev->hEvtBps );
	}
#endif
	//释放TSP
	EnterCriticalSection( &pRasDev->csRasDev );
	RasT_FreeTsp( pRasDev->lpTspInfo );
	pRasDev->lpTspInfo = NULL;
	LeaveCriticalSection( &pRasDev->csRasDev );

	//开始释放
	for( i=0; (i<0x7F) && (pRasDev->wDevOp & RASOP_WAITMS); i++ )
	{
		Sleep( 2 );
	}
	RasD_FreeDev( pRasDev );
}
