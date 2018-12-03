/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵���������豸�����ʵ��---RAS
�汾�ţ�  2.0.0
����ʱ�ڣ�2003-12-01
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
#include <eapisrv.h>
#include <listunit.h>
//"\inc_app"
#include <ras.h>
//"\inc_drv"
#include "tapi.h"
//"\inc_local"
#include "netif_dl_ras.h"
#include "ras_call.h"
//"local"
#include "ras_def.h"
#include "ras_dev.h"


/***************  ȫ���� ���壬 ���� *****************/
		RAS_GLOBAL*		g_lpGlobalRas = NULL;

//
static	void	RasT_SendEvt( DWORD dwNotifierType, LPVOID notifier, DWORD dwEvt, DWORD dwErr );

static	DWORD	RasT_GetTsp( OUT TSPINFO** ppTspInfo, OUT TSPITBL** ppTspTbl, RAS_DEV* pRasDev, DWORD dwNotifierType, LPVOID notifier );

static	BOOL	RasT_CheckOp( RAS_DEV* pRasDev, DWORD dwOp );
static	DWORD	WINAPI	RasT_ThrdDiscnn( LPVOID lpParam );

static	BOOL	RasT_BeginStat( RAS_DEV* pRasDev );
static	BOOL	RasT_EnableBps( RAS_DEV* pRasDev, BOOL fEnableBps );
static	DWORD	WINAPI	RasT_ThrdStatBps( LPVOID lpParam );

/******************************************************/


//------------------------------------------------------
//���ܣ� ��ʼ��
//------------------------------------------------------
BOOL	Ras_Init( HWND hWndNetApp )
{
	g_lpGlobalRas = (RAS_GLOBAL*)HANDLE_ALLOC( sizeof(RAS_GLOBAL) );
	if( !g_lpGlobalRas )
	{
		return FALSE;
	}
	HANDLE_INIT( g_lpGlobalRas, sizeof(RAS_GLOBAL) );
	List_InitHead( &g_lpGlobalRas->hListRasDev );
	List_InitHead( &g_lpGlobalRas->hListTsp );
	InitializeCriticalSection( &g_lpGlobalRas->csListRasDev );
	InitializeCriticalSection( &g_lpGlobalRas->csListTsp );

	g_lpGlobalRas->hWndNetMgr = hWndNetApp;

	return TRUE;
}

void	Ras_Deinit( )
{
	//�Ҷ������豸

	//�ͷ�ȫ�ֽṹ
	DeleteCriticalSection( &g_lpGlobalRas->csListRasDev );
	DeleteCriticalSection( &g_lpGlobalRas->csListTsp );
	HANDLE_FREE( g_lpGlobalRas );
}

DWORD	WINAPI	RAPI_CallBack( HANDLE hServer, DWORD dwServerCode, DWORD dwParam, LPVOID lpParam )
{
	switch( dwServerCode )
	{
	case SCC_BROADCAST_SHUTDOWN:
		//�Ͽ���������....
		break;
	default:
		return Sys_DefServerProc( hServer, dwServerCode , dwParam, lpParam );
	}
	return 0;
}


// ------------------------------------------------------
// �����õ�API����----���š��Ҷϡ���ȡ��Ϣ��ö������
// ------------------------------------------------------

// ********************************************************************
// ������
// ������
//	IN pDialParam-ָ�����Ų���
//	IN dwNotifierType-ָ�������¼���֪ͨ����
//	IN notifier-ָ�������¼��Ľ�����
//	OUT phRasConn-���沦�����ӵľ��
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// �������������ݲ�����Ŀ�Ͳ��Ų��������в���
// ����: 
// ********************************************************************
DWORD	WINAPI	RAPI_Dial( LPCTSTR lpszPhonebook, DIALPARAM* pDialParam, DWORD dwNotifierType, LPVOID notifier, OUT HANDLE* phRasConn )
{
	DWORD			dwErr;
	
	DIALPARAM		stDialParam_Def;
	RAS_DEV*		pRasDev;
	TSPINFO*		pTspInfo;
	TSPITBL*		pTspTbl;
	HANDLE			hLineDev;

	RASENTRY		stRasEntry;
	PARAM_OPENDEV	ParamOpenDev;
	PPP_LINK_RAS	InfoLink;
	TSP_DLINFO		InfoDl;

	DWORD			dwTmp;
	BOOL			fSuccess;

	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDialParam, RASERR_NOPARAM );
		return RASERR_NOPARAM;
	}
	//�������
	if( pDialParam==NULL )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDialParam, RASERR_SUCCESS );
		fSuccess = TRUE;
		stDialParam_Def.dwSize = sizeof(stDialParam_Def);
		stDialParam_Def.szEntryName[0] = 0;
		stDialParam_Def.szPhoneNumber[0] = 0;
		stDialParam_Def.szUserName[0] = 0;
		stDialParam_Def.szPassword[0] = 0;
		stDialParam_Def.szDomain[0] = 0;
		if( !RAPI_GetEntryDialParams( lpszPhonebook, &stDialParam_Def, &fSuccess ) )
		{
			RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDialParam, RASERR_NOPARAM );
			return RASERR_NOPARAM;
		}
		pDialParam = &stDialParam_Def;
	}
	//{
	//	//RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOPARAM );
	//	//return RASERR_NOPARAM;
	//}
	dwErr = RASERR_SUCCESS;

//RasDial ���Ź������̣�
	
	//=< 1 >====<< Ѱ��RAS_DEV�������벦��ʹ��״̬ >>============================================
	RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_SUCCESS );
	//1����ȡ RASENTRY->szDrvReg: ���� DIALPARAMSW->szEntryName����ȡ RASENTRY
	dwTmp = sizeof(RASENTRY);
	stRasEntry.szDrvReg[0] = 0;
	if( !RAPI_GetEntryProperties( lpszPhonebook, pDialParam->szEntryName, &stRasEntry, &dwTmp ) )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOENTRY );
		return FALSE;
	}
	if( stRasEntry.szDrvReg[0]==0 )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOENTRY );
		return RASERR_NOENTRY;
	}
	//2��Ѱ�� RAS_DEV: ���� RASENTRY->szDrvReg ����������ע��Ĳ����豸
	if( !RasD_FindDev( &pRasDev, stRasEntry.szDrvReg ) )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NODEVICE );
		return RASERR_NODEVICE;
	}
	//3���ж��Ƿ���Բ��ţ����� RAS_DEV ��״̬�жϣ������ ʹ��״̬
	if( pRasDev->dwDevStatus!=RASDEV_IDLE )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTIDLE );
		return RASERR_NOTIDLE;
	}
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_IDLE )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTIDLE );
		return RASERR_NOTIDLE;
	}
	pRasDev->dwDevStatus = RASDEV_LOAD;
	pRasDev->dwNotifierType = dwNotifierType;
	pRasDev->notifier = notifier;
	if( pDialParam->szEntryName[0] )
	{
		strncpy( pRasDev->szEntryName, pDialParam->szEntryName, RAS_MaxEntryName );
		pRasDev->szEntryName[RAS_MaxEntryName]=0;
	}
	else
	{
		RAPI_GetEntryDef( lpszPhonebook, pRasDev->szEntryName, RAS_MaxEntryName );
	}
	ResetEvent( pRasDev->hEvtWaitCnn );
	LeaveCriticalSection( &pRasDev->csRasDev );

	*phRasConn = (HANDLE)pRasDev;
	//=< 2 >====<< Ѱ�һ���� TSPINFO >>========================================================
	dwErr = RasT_GetTsp( &pTspInfo, &pTspTbl, pRasDev, dwNotifierType, notifier );
	if( dwErr!=RASERR_SUCCESS )
	{
		goto EXIT_RASDIAL;
	}

	//=< 3 >====<< ����TSP�������� >>===========================================================
	//1��TSPI_CreateDev---Ѱ�һ򴴽� TSP�豸��
		// ���� RAS_DEV->dwEnumID��RAS_DEV��������غ��ΨһID�ţ�Ѱ�һ򴴽���
		// ͬʱ���� szDrvRegPath �� �򿪷�ʽ
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_LOAD )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_BREAK );
		dwErr = RASERR_BREAK;
		goto EXIT_RASDIAL;
	}
	if( pRasDev->hLineDev==NULL )
	{
		hLineDev = (*pTspTbl->TSPI_CreateDev)( pRasDev->dwEnumID, pRasDev->szDrvReg, &pRasDev->rasOpen );
		if( hLineDev==NULL )
		{
			LeaveCriticalSection( &pRasDev->csRasDev );
			RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTSPDEV );
			dwErr = RASERR_NOTSPDEV;
			goto EXIT_RASDIAL;
		}
		pRasDev->hLineDev = hLineDev;
	}
	else
	{
		hLineDev = pRasDev->hLineDev;
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	//2��TSPI_OpenDev---TSP�豸�� �󶨡��򿪡����� ��
		// hRasDev---������ TSP�豸 �Ѿ��󶨵� RAS�ϣ������໥֪ͨ�ˡ�
		// pszEntry �� lpEntry---���ڸ��豸 ���ò���
	ParamOpenDev.hRasDev = (HANDLE)pRasDev;
	ParamOpenDev.pszEntry = pRasDev->szEntryName;
	ParamOpenDev.lpEntry = &stRasEntry;
	if( !(*pTspTbl->TSPI_OpenDev)(hLineDev, &ParamOpenDev) )
	{
		dwErr = RASERR_NOTSPOPEN;
		goto EXIT_RASDIAL;
	}
	//3��TSPI_lineDial---TSP�豸��Init���������������в��ţ� ���� pDialParam
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_LOAD )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_BREAK );
		dwErr = RASERR_BREAK;
		goto EXIT_RASDIAL;
	}
	pRasDev->dwDevStatus = RASDEV_DIALING;
	LeaveCriticalSection( &pRasDev->csRasDev );
	if( !(*pTspTbl->TSPI_lineDial)( hLineDev, pDialParam->szPhoneNumber ) )
	{
		dwErr = RASERR_TSP_DIAL;
		goto EXIT_RASDIAL;
	}
	RasT_SendEvt( dwNotifierType, notifier, RASCS_DeviceConnected, RASERR_SUCCESS );
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_DIALING )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_DeviceConnected, RASERR_BREAK );
		dwErr = RASERR_BREAK;
		goto EXIT_RASDIAL;
	}
	pRasDev->dwDevStatus = RASDEV_DIALED;
	LeaveCriticalSection( &pRasDev->csRasDev );

	//��ʼ��ͳ����Ϣ
	//if( !RasT_BeginStat( pRasDev ) )
	//{
	//}

	//=< 4 >====<< ���� ������·�� �� RAS�㡢TSP�� >>===========================================
	//1��TSPI_lineGetInfo---��ȡ TSP��������Ϣ���Ա���� ������·�㣺�� AC��MAC��ַ
	if( !(*pTspTbl->TSPI_lineGetInfo)( hLineDev, &InfoDl ) )
	{
		dwErr = RASERR_TSP_NOINFO;
		goto EXIT_RASDIAL;
	}
	//2��xxx_CreateDev---�ڲ�����·�� ����豸�ӿڣ��������� PPP_CreateDev ���� PPPoEs_CreateDev
		//RAS�㣺�ṩ ���Ų��� �� RAS�豸���
		//TSP�㣺�ṩ TSP�豸��� �� ���ͺ���
		//������·�㣺�����ӿڣ��ṩ���պ�����
		//���� ������·��ľ�� ��RAS��
	memset( &InfoLink, 0, sizeof(PPP_LINK_RAS) );
	InfoLink.hRasDev = (HANDLE)pRasDev;
	InfoLink.pDialParam = pDialParam;
	//InfoLink.hTspDev = hLineDev;
	//InfoLink.pFnTspOut = pTspTbl->TSPI_lineSend;
	InfoLink.pInfoFromTsp = &InfoDl;
	//if( RASDT_GETMAIN(pRasDev->dwDevType)==RASDT_PPPoE )
	//{
	//	fSuccess = PPPoEs_CreateDev( &InfoLink, TRUE );
	//}
	//else
	{
		fSuccess = PPP_CreateDev( &InfoLink, TRUE );
	}
	//3��TSPI_lineSetRecv---��������·��������Ϣ ����TSP��
	if( fSuccess )
	{
		//����TSP��
		(*pTspTbl->TSPI_lineSetRecv)( hLineDev, 0, 0 );
		//����Ϣ���浽RAS��
		pRasDev->hIfDl = InfoLink.LocalDl.hIfDev;
		pRasDev->pFnDlNotify = InfoLink.LocalDl.pFnDlNotify;
		pRasDev->pFnDlIn = InfoLink.LocalDl.pFnDlIn;
	}
	else
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_PPP, RASERR_PPP_DEV );
		dwErr = RASERR_PPP_DEV;
		goto EXIT_RASDIAL;
	}
	//4��xxx_Notify---֪ͨ ������·�� (PROEVT_DEV_LINKED)��PPP_Notify ���� PPPoEs_Notify
	RasT_SendEvt( dwNotifierType, notifier, RASCS_PPP, RASERR_SUCCESS );
	(*pRasDev->pFnDlNotify)( pRasDev->hIfDl, PROEVT_DEV_LINKED );

	//=< 5 >====<< �ȴ�������·������֪ͨ >>==================================================
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_DIALED )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_DeviceConnected, RASERR_BREAK );
		dwErr = RASERR_BREAK;
		goto EXIT_RASDIAL;
	}
	pRasDev->dwDevStatus = RASDEV_WAITCNN;
	LeaveCriticalSection( &pRasDev->csRasDev );
	//WaitForSingleObject( pRasDev->hEvtWaitCnn, 20*60*1000 );
	WaitForSingleObject( pRasDev->hEvtWaitCnn, INFINITE );

	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_LINK )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		dwErr = RASERR_TIMEOUT;
		goto EXIT_RASDIAL;
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	//��ʼ��ͳ����Ϣ
	RETAILMSG(1,(TEXT("\r\n RAPI_Dial: dwDevStatus[0x%x]\r\n"), pRasDev->dwDevStatus));
	RasT_BeginStat( pRasDev );

EXIT_RASDIAL:
	pRasDev->wDevOp &= ~RASOP_WAITMS;	//ע��!!!
	if( dwErr!=RASERR_SUCCESS )
	{
		RasIf_HangUp( (HANDLE)pRasDev, RASH_OP_DIRECT, 0, 0 );
	}

	return dwErr;
}

// ********************************************************************
// ������
// ������
//	IN hRasConn-�������ӵľ��
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// �����������Ҷ� 1����������
// ����: 
// ********************************************************************
DWORD	WINAPI	RAPI_HangUp( HANDLE hRasConn )
{
#if 0
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;

	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	//
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}
	return RasIf_HangUp( (HANDLE)pRasDev, RASH_OP_WAITOVER, 0, 0 );
#else
	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	return RasIf_HangUp( hRasConn, RASH_OP_WAITOVER, 0, 0 );
#endif
}

// ********************************************************************
// ������
// ������
//	IN hRasConn-�������ӵľ��
//	OUT lpRasConnInfo-�������ӵ���Ϣ
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// �����������Ҷ� 1����������
// ����: 
// ********************************************************************
DWORD	WINAPI	RAPI_GetConnectInfo( HANDLE hRasConn, OUT RASCNNINFO* lpRasConnInfo )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;

	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}
	//����״̬
	if( (pRasDev->wDevOp & (RASOP_HANGUP|RASOP_FREE)) || (pRasDev->dwDevStatus!=RASDEV_LINK) )
	{
		return 1;
	}

	//��ȡ��Ϣ
	//lpRasConnInfo->rasDevInfo.dwSize       = sizeof(RASDEVINFO);
	lpRasConnInfo->rasDevInfo.dwDevType    = pRasDev->dwDevType;
	lpRasConnInfo->rasDevInfo.dwDevStatus  = pRasDev->dwDevStatus;
	strcpy( lpRasConnInfo->rasDevInfo.szDrvReg, pRasDev->szDrvReg );
	//��ȡ RAS�豸NAME
	lpRasConnInfo->rasDevInfo.szDeviceName[0] = 0;
	RAPI_LookDriver( pRasDev->szDrvReg, 0, 0, 0, lpRasConnInfo->rasDevInfo.szDeviceName );

	//�ָ�״̬
	pRasDev->wDevOp &= ~RASOP_WAITMS;

	return 0;
}

DWORD	WINAPI	RAPI_GetConnectStatus( HANDLE hRasConn, OUT DWORD* pdwDevStatus )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;

	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}

	return 0;
}


DWORD	WINAPI	RAPI_EnableStat( HANDLE hRasConn, BOOL fEnbaleBps )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;

	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}
	//����״̬
	if( (pRasDev->wDevOp & (RASOP_HANGUP|RASOP_FREE)) || (pRasDev->dwDevStatus!=RASDEV_LINK) )
	{
		return 1;
	}
	//
	RasT_EnableBps( pRasDev, fEnbaleBps );
	return 0;
}

DWORD	WINAPI	RAPI_GetLinkStatistics( HANDLE hRasConn, OUT RAS_STATS *lpStatistics )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;
#ifdef RAS_BPS_1
#else
	DWORD		dwTickCur;
	DWORD		dwTickS;
#endif

	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}
	//����״̬
	if( (pRasDev->wDevOp & (RASOP_HANGUP|RASOP_FREE)) || (pRasDev->dwDevStatus!=RASDEV_LINK) )
	{
		return 1;
	}
	if( pRasDev->wDevOp & RASOP_NO_BPS )
	{
		return 2;
	}
	//
	lpStatistics->dwBytesXmited = pRasDev->dwBytesXmited;
	lpStatistics->dwBytesRcved = pRasDev->dwBytesRcved;
	lpStatistics->dwConnectDuration = GetTickCount()-pRasDev->dwTickCnn;
#ifdef RAS_BPS_1
	lpStatistics->dwBps = pRasDev->dwRxBps;
	lpStatistics->dwBps_Tx = pRasDev->dwTxBps;
	//lpStatistics->dwFramesXmited = ;
	//lpStatistics->dwFramesRcved = ;
	//lpStatistics->dwCrcErr = ;
#else
	//
	dwTickCur = GetTickCount();
	dwTickS = dwTickCur - pRasDev->dwTickRx_Bps;
	if( dwTickS )
	{
		lpStatistics->dwBps = pRasDev->dwBytesRx_Bps / dwTickS;
		if( dwTickS>TIMER_STATBPS )
		{
			pRasDev->dwTickRx_Bps = dwTickCur;
			pRasDev->dwBytesRx_Bps = 0;
		}
	}
	else
	{
		lpStatistics->dwBps = pRasDev->dwBytesRx_Bps;
	}
	//
	dwTickS = dwTickCur - pRasDev->dwTickTx_Bps;
	if( dwTickS )
	{
		lpStatistics->dwBps_Tx = pRasDev->dwBytesTx_Bps / dwTickS;
		if( dwTickS>TIMER_STATBPS )
		{
			pRasDev->dwTickTx_Bps = dwTickCur;
			pRasDev->dwBytesTx_Bps = 0;
		}
	}
	else
	{
		lpStatistics->dwBps_Tx = pRasDev->dwBytesTx_Bps;
	}
#endif

	return 0;
}


DWORD	WINAPI	RAPI_EnumConnections( HANDLE* phRasConn, IN OUT LPDWORD lpdwCntRasConn )
{
	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	return 0;
}

// RAS���Ÿ�������
DWORD	WINAPI	RAPI_GetStateStr( DWORD dwRasState, LPTSTR pszStateStr, DWORD dwBufSize )
{
	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	//�������
	if( !pszStateStr || dwBufSize<2 )
	{
		return 0; //ERROR_INSUFFICIENT_BUFFER
	}
	dwBufSize -= 1;
	pszStateStr[dwBufSize] = 0;
	//
	switch( dwRasState )
	{
	case RASCS_LoadDialParam:
		strncpy( pszStateStr, TEXT("���ڼ���Ĭ�ϲ��Ų���..."), dwBufSize );
		break;

	case RASCS_LoadDev:
		strncpy( pszStateStr, TEXT("���ڼ����豸..."), dwBufSize );
		break;
	case RASCS_OpenPort:
		strncpy( pszStateStr, TEXT("���ڴ��豸..."), dwBufSize );
		break;
	case RASCS_PortOpened:
		strncpy( pszStateStr, TEXT("�豸�򿪳ɹ�!"), dwBufSize );
		break;
	case RASCS_ModemReady:
		strncpy( pszStateStr, TEXT("modem����׼��..."), dwBufSize );
		break;
	case RASCS_ConnectDevice:
		strncpy( pszStateStr, TEXT("�豸���ڲ���..."), dwBufSize );
		break;
	case RASCS_DeviceConnected:
		strncpy( pszStateStr, TEXT("�豸���ųɹ�!"), dwBufSize );
		break;

	case RASCS_PPP:
		strncpy( pszStateStr, TEXT("���ڽ�����������..."), dwBufSize );
		break;
	case RASCS_LCP_OK:
		strncpy( pszStateStr, TEXT("�������óɹ���"), dwBufSize );
		break;
	case RASCS_Authenticate:
		strncpy( pszStateStr, TEXT("������֤�û�������..."), dwBufSize );
		break;
	case RASCS_AuthAck:
		strncpy( pszStateStr, TEXT("��֤�û�������ɹ�!"), dwBufSize );
		break;
	case RASCS_IPCP:
		strncpy( pszStateStr, TEXT("����������ע������..."), dwBufSize );
		break;
	case RASCS_IPCP_OK:
		strncpy( pszStateStr, TEXT("������ע�������ɹ���"), dwBufSize );
		break;
	case RASCS_Connected:
		strncpy( pszStateStr, TEXT("���ӳɹ�!"), dwBufSize );
		break;
	case RASCS_Disconnected:
		strncpy( pszStateStr, TEXT("���ӶϿ�!"), dwBufSize );
		break;
	default:
		{
			TCHAR	pszTemp[50];
			sprintf( pszTemp, "Unknown state[%d]", dwRasState );
			strncpy( pszStateStr, pszTemp, dwBufSize );
		}
		break;
	}
	return 0;
}

DWORD	WINAPI	RAPI_GetErrStr( DWORD dwRasErr, LPTSTR pszErrStr, DWORD dwBufSize )
{
	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	//�������
	if( !pszErrStr || dwBufSize<2 )
	{
		return 0; //ERROR_INSUFFICIENT_BUFFER
	}
	dwBufSize -= 1;
	pszErrStr[dwBufSize] = 0;
	//
	switch( dwRasErr )
	{
	case RASERR_NOPARAM:
		strncpy( pszErrStr, TEXT("û�в��Ų��������߲��Ų����д���!"), dwBufSize );
		break;
	case RASERR_BREAK:
		strncpy( pszErrStr, TEXT("�豸���Ų��������!"), dwBufSize );
		break;
	case RASERR_NOSIGNAL:
		strncpy( pszErrStr, TEXT("�豸û���ź�!"), dwBufSize );
		break;
	case RASERR_TIMEOUT:
		strncpy( pszErrStr, TEXT("�豸���ų�ʱ!"), dwBufSize );
		break;
	case RASERR_NOTIDLE:
		strncpy( pszErrStr, TEXT("�豸���Ǵ��ڿ���״̬!"), dwBufSize );
		break;

	case RASERR_NOENTRY:
		strncpy( pszErrStr, TEXT("�ڼ�鲦�����ӵ�����ʱ����!"), dwBufSize );
		break;
	case RASERR_NODEVICE:
		strncpy( pszErrStr, TEXT("û���ҵ�modem�豸!"), dwBufSize );
		break;
	case RASERR_NOTSP:
		strncpy( pszErrStr, TEXT("����TSPʱʧ��!"), dwBufSize );
		break;
	case RASERR_NOTSPDEV:
		strncpy( pszErrStr, TEXT("����TSP�豸ʱʧ��!"), dwBufSize );
		break;
	case RASERR_NOTSPOPEN:
		strncpy( pszErrStr, TEXT("��TSP�豸ʱʧ��!"), dwBufSize );
		break;
		
	case RASERR_TSP_OPEN:
		strncpy( pszErrStr, TEXT("���豸ʱʧ��!"), dwBufSize );
		break;
	case RASERR_TSP_CFG:
		strncpy( pszErrStr, TEXT("�����豸ʱʧ��!"), dwBufSize );
		break;
	case RASERR_TSP_ENTRY:
		strncpy( pszErrStr, TEXT("��ȡ������Ŀʱʧ��!"), dwBufSize );
		break;
	case RASERR_TSP_START:
		strncpy( pszErrStr, TEXT("����MODEMʱʧ��!"), dwBufSize );
		break;
	case RASERR_TSP_TESTMDM:
		strncpy( pszErrStr, TEXT("����MODEMʱʧ��!"), dwBufSize );
		break;
	case RASERR_TSP_SETMDM:
		strncpy( pszErrStr, TEXT("����MODEMʱʧ��!"), dwBufSize );
		break;
		
	case RASERR_TSP_INIT:
		strncpy( pszErrStr, TEXT("�����豸����ʱʧ��!"), dwBufSize );
		break;
	case RASERR_TSP_DIAL:
		strncpy( pszErrStr, TEXT("�豸����ʧ��!"), dwBufSize );
		break;
	case RASERR_TSP_DIAL_BUSY:
		strncpy( pszErrStr, TEXT("�豸����ʧ�ܣ���Ϊռ��!"), dwBufSize );
		break;
	case RASERR_TSP_DIAL_NOCAR:
		strncpy( pszErrStr, TEXT("�豸����ʧ�ܣ���Ϊû���ز��ź�!"), dwBufSize );
		break;
		
	case RASERR_TSP_NOINFO:
		strncpy( pszErrStr, TEXT("��TSP DLL�л�ȡ�����Ϣʱʧ��!"), dwBufSize );
		break;
		
	case RASERR_PPP_DEV:
		strncpy( pszErrStr, TEXT("�ڲ�����·������豸ʱʧ��!"), dwBufSize );
		break;
	case RASERR_PPP_CFG:
		strncpy( pszErrStr, TEXT("�����������ʧ��!"), dwBufSize );
		break;
		
	case RASERR_PPP_PASSWORD:
		strncpy( pszErrStr, TEXT("������û��������!"), dwBufSize );
		break;
	case RASERR_PPP_TICKOFF:
		strncpy( pszErrStr, TEXT("���Է���ֹ���Ͽ�!"), dwBufSize );
		break;
	default:
		{
			TCHAR	pszTemp[50];
			sprintf( pszTemp, "Unknown error[%d]", dwRasErr );
			strncpy( pszErrStr, pszTemp, dwBufSize );
		}
		break;
	}
	return 0;
}


// ------------------------------------------------------
// RAS�ṩ��TSP/PPP���ڲ�ʹ�õĽӿں���
// ------------------------------------------------------

// ********************************************************************
// ������
// ������
//	IN hRasDev-����RAS�豸�ľ��
//	IN dwRasCnnStatus-֪ͨ״̬
//	IN dwErr-֪ͨ����
// ����ֵ��
//	��
// ������������RAS֪ͨ�¼�����RAS��RASAPP֪ͨ�¼�
// ����: 
// ********************************************************************
void	RasIf_NotifyEvt( HANDLE hRasDev, DWORD dwRasCnnStatus, DWORD dwErr )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasDev;
	BOOL		fResend = FALSE;

	//��RAS֪ͨ�¼��Ĺ���
	//TSP�豸���ڴ򿪰󶨺󣬸���֪ͨ���й��̵��¼�
	//PPP�豸����CreateDev�󣬸���֪ͨ���й��̵��¼�

	//�� �����
	if( !HANDLE_CHECK(pRasDev) )
	{
		return ;
	}
	//
	if( pRasDev->dwNotifierType==DIALNOTIFY_WND )
	{
		if( pRasDev->notifier )
		{
			//SendMessage( (HWND)pRasDev->notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
			PostMessage( (HWND)pRasDev->notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
		}
		else
		{
			fResend = TRUE;
		}
	}
	//else
	//{
	//}

	//
	if( fResend )
	{
		//PostMessage( g_lpGlobalRas->hWndNetMgr, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
	}
	//
	if( dwRasCnnStatus==RASCS_Connected && dwErr==RASERR_SUCCESS )
	{
		//������״̬
		EnterCriticalSection( &pRasDev->csRasDev );
		if( pRasDev->dwDevStatus==RASDEV_WAITCNN )
		{
			pRasDev->dwDevStatus = RASDEV_LINK;
			//֪ͨ���ӳɹ�
			if( pRasDev->hEvtWaitCnn )
			{
				SetEvent( pRasDev->hEvtWaitCnn );
			}
		}
		LeaveCriticalSection( &pRasDev->csRasDev );
	}
	//
	else if( dwErr==RASERR_NOSIGNAL )
	{
		RasIf_HangUp( hRasDev, 0, 0, 0 );
	}
	else
	{
		//
		//if( dwRasCnnStatus==RASCS_Disconnected )
		//{
		//	int kk=0;
		//}
	}
}

#define DEBUG_RasIf_DownData 0
DWORD	RasIf_DownData( HANDLE hRasDev, LPBYTE pBuf, DWORD dwLen )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasDev;

	DEBUGMSG( DEBUG_RasIf_DownData, ( "RasIf_DownData entry.\r\n" ) );
	//CheckOutPtr( __FILE__, __LINE__ );
	//�� �����
	if( !HANDLE_CHECK(pRasDev) || ((pRasDev->dwDevStatus!=RASDEV_LINK) && (pRasDev->dwDevStatus!=RASDEV_WAITCNN)) )
	{
		DEBUGMSG( DEBUG_RasIf_DownData, ( "RasIf_DownData leave01.\r\n" ) );
		return 1;
	}
	//ͳ��
	pRasDev->dwBytesXmited += dwLen;
	pRasDev->dwBytesTx_Bps += dwLen;
	//
	//CheckOutPtr( __FILE__, __LINE__ );
	if( pRasDev->lpTspInfo->pTspTbl->TSPI_lineSend )
	{
		(*pRasDev->lpTspInfo->pTspTbl->TSPI_lineSend)( pRasDev->hLineDev, pBuf, dwLen );
	}
	//CheckOutPtr( __FILE__, __LINE__ );
	DEBUGMSG( DEBUG_RasIf_DownData, ( "RasIf_DownData leave02.\r\n" ) );
	return 0;
}

DWORD	RasIf_UpData( HANDLE hRasDev, LPBYTE pBuf, DWORD dwLen )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasDev;

	//�� �����
	if( !HANDLE_CHECK(pRasDev) || ((pRasDev->dwDevStatus!=RASDEV_LINK) && (pRasDev->dwDevStatus!=RASDEV_WAITCNN)) )
	{
		return 1;
	}
	//ͳ��
	pRasDev->dwBytesRcved += dwLen;
	pRasDev->dwBytesRx_Bps += dwLen;
	//InterlockedExchangeAdd( &pRasDev->dwBytesRx_Bps, dwLen );
	//InterlockedExchangeAdd( &pRasDev->dwBytesRcved, dwLen );
	//
	if( pRasDev->pFnDlIn )
	{
		(*pRasDev->pFnDlIn)( pRasDev->hIfDl, pBuf, (WORD)dwLen );
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN hRasDev-����RAS�豸�ľ��
//	IN dwOperation-�Ҷ� �Ĳ���(ͬ�����첽�������豸)
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// �����������Ҷ� 1����������
// ����: 
// ********************************************************************
DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasDev;

	//��֤�豸����
	RETAILMSG(1,(TEXT("\r\n RasIf_HangUp: Enter h=[0x%x], opt=[0x%x]\r\n"), pRasDev, dwOperation));
	if( !HANDLE_CHECK(pRasDev) )
	{
		RETAILMSG(1,(TEXT("\r\n RasIf_HangUp: Leave Failed--1\r\n")));
		return 0;
	}
	//�������
	if( !RasT_CheckOp(pRasDev, dwOperation) )
	{
		RETAILMSG(1,(TEXT("\r\n RasIf_HangUp: Leave Failed--2\r\n")));
		return 0;
	}
	//
	if( dwRasCnnStatus && (pRasDev->dwNotifierType==DIALNOTIFY_WND) && pRasDev->notifier )
	{
		PostMessage( (HWND)pRasDev->notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
	}
	//��������
	if( dwOperation & RASH_OP_DIRECT )
	{
		//ֱ�� ����
		RasT_ThrdDiscnn( (LPVOID)pRasDev );
	}
	else
	{
		HANDLE	hThrd;
		DWORD	dwThrdID;

		//�߳� ����
		hThrd = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)RasT_ThrdDiscnn, (LPVOID)pRasDev, 0, &dwThrdID );
		if( !hThrd )
		{
			RETAILMSG(1,(TEXT("\r\n RasIf_HangUp: Leave Failed--3\r\n")));
			return 1;
		}
		//�߳� �ȴ�
		if( dwOperation & RASH_OP_WAITOVER )
		{
			WaitForSingleObject( hThrd, INFINITE );
		}
		CloseHandle( hThrd );
	}

	RETAILMSG(1,(TEXT("  RasIf_HangUp: Leave ok\r\n")));
	return 0;
}


// ------------------------------------------------------
// �����õ��ڲ�����
// ------------------------------------------------------


DWORD	RasT_GetTsp( OUT TSPINFO** ppTspInfo, OUT TSPITBL** ppTspTbl, RAS_DEV* pRasDev, DWORD dwNotifierType, LPVOID notifier )
{
	TCHAR			pszTspDll[MAX_PATH];
	DWORD			dwTmp;

	//1����ȡ pszTspDll�����ݸ�RAS_DEV->szDrvRegPath
	dwTmp = MAX_PATH;
	if( !RAPI_LookDriver( pRasDev->szDrvReg, 0, 0, pszTspDll, 0 ) )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTSP );
		return RASERR_NOTSP;
	}
	//2��Ѱ�� TSPINFO�����ݸ� pszTspDll������� ����״̬
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_LOAD )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_BREAK );
		return RASERR_BREAK;
	}
	//ASSERT( pRasDev->lpTspInfo==NULL && pRasDev->hLineDev==NULL );//Attention
	if( pRasDev->lpTspInfo==NULL )
	{
		if( !RasT_FindTsp( ppTspInfo, pszTspDll, TRUE ) )
		{
			//3������ TSPINFO�����ݸ� pszTspDll������� ����״̬
			if( !RasT_LoadTsp( ppTspInfo, pszTspDll, TRUE ) )
			{
				pRasDev->dwDevStatus = RASDEV_IDLE;
				LeaveCriticalSection( &pRasDev->csRasDev );
				RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTSP );
				return RASERR_NOTSP;
			}
			pRasDev->lpTspInfo = *ppTspInfo;
		}
	}
	else
	{
		*ppTspInfo = pRasDev->lpTspInfo;
		EnterCriticalSection( &g_lpGlobalRas->csListTsp );
		(*ppTspInfo)->nRefCnt ++;
		LeaveCriticalSection( &g_lpGlobalRas->csListTsp );
	}
	*ppTspTbl = (*ppTspInfo)->pTspTbl;
	LeaveCriticalSection( &pRasDev->csRasDev );

	return RASERR_SUCCESS;
}



// ********************************************************************
// ������
// ������
//	IN dwNotifierType-ָ�������¼���֪ͨ����
//	IN notifier-ָ�������¼��Ľ�����
//	IN dwRasCnnStatus-֪ͨ״̬
//	IN dwErr-֪ͨ����
// ����ֵ��
//	��
// ����������RAS��RASAPP֪ͨ�¼�
// ����: 
// ********************************************************************
void	RasT_SendEvt( DWORD dwNotifierType, LPVOID notifier, DWORD dwRasCnnStatus, DWORD dwErr )
{
	if( dwNotifierType==DIALNOTIFY_WND )
	{
		//SendMessage( (HWND)notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
		PostMessage( (HWND)notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
	}
}

// ********************************************************************
// ������
// ������
//	IN pRasDev-����RAS�豸�ľ��
//	IN dwOp-�Ҷ� �Ĳ���(ͬ�����첽�������豸)
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// �����������Ҷ� 1����������ǰ�ļ�飬�Ա�ȷ���Ƿ��������
// ����: 
// ********************************************************************
BOOL	RasT_CheckOp( RAS_DEV* pRasDev, DWORD dwOp )
{
	RAS_DEV*	pRasDev_Find;
	BOOL		fContinue = FALSE;
	PLIST_UNIT	pHeader;
	PLIST_UNIT	pUnit;

	pHeader = &g_lpGlobalRas->hListRasDev;
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pRasDev_Find = LIST_CONTAINER( pUnit, RAS_DEV, hListRasDev);
		pUnit = pUnit->pNext;    // advance to next 

		//�����ȫѰ��
		if( !HANDLE_CHECK(pRasDev_Find) )
		{
			break;
		}
		if( (pRasDev->wFlag & RASDF_EXIT) || (pRasDev->wDevOp & RASOP_FREE) )
		{
			continue;
		}

		//����ҵ���˵����û�б�ɾ����������Ҳ�����˵���Ѿ���ɾ����
		if( pRasDev==pRasDev_Find )
		{
			if( pRasDev->wDevOp & RASOP_HANGUP )
			{
				//����Ѿ����� �Ͽ�����ɾ���Ĳ��� �Ļ����������״̬��
				//�����Ҫ�Ļ������ó�ɾ�����������Ƿ��������
				if( dwOp & RASH_OP_FREEDEV )
				{
					pRasDev->wDevOp |= RASOP_FREE;
				}
			}
			else
			{
				//�����û�н��� �Ͽ�����ɾ���Ĳ��� �Ļ���������
				//EnterCriticalSection( &pRasDev->csRasDev );
				pRasDev->dwDevStatus = RASDEV_DISCNN;
				//LeaveCriticalSection( &pRasDev->csRasDev );
				pRasDev->wDevOp = RASOP_HANGUP;
				if( dwOp & RASH_OP_FREEDEV )
				{
					pRasDev->wDevOp |= RASOP_FREE;
				}
				fContinue = TRUE;
			}
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );

	return fContinue;
}

// ********************************************************************
// ������
// ������
//	IN lpParam-����RAS�豸�ľ��
// ����ֵ��
//	0
// �����������Ҷ� 1���������� �� �豸����
// ����: 
// ********************************************************************
DWORD	WINAPI	RasT_ThrdDiscnn( LPVOID lpParam )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)lpParam;
	TSPITBL*	pTspTbl = NULL;

	//
	RETAILMSG(1,(TEXT("\r\n RasT_ThrdDiscnn: Enter h=[0x%x]\r\n"), pRasDev));
	if( !HANDLE_CHECK(pRasDev) )
	{
		RETAILMSG(1,(TEXT("\r\n RasT_ThrdDiscnn: leave Failed--1\r\n")));
		return 0;
	}
	//
	//1. ״̬������
	Sleep(1);
	EnterCriticalSection( &pRasDev->csRasDev );
	pRasDev->dwDevStatus = RASDEV_DISCNN;
	LeaveCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->hEvtWaitCnn )
	{
		SetEvent( pRasDev->hEvtWaitCnn );
		SetEvent( pRasDev->hEvtWaitCnn );
	}
	Sleep(1);

	//2. ֪ͨ PPP��ͣ�ø��豸�ӿ�
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->hIfDl && pRasDev->pFnDlNotify )
	{
		(*pRasDev->pFnDlNotify)( pRasDev->hIfDl, PROEVT_DEV_STOP );
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	Sleep(1);

	//3.���� TSP��ͣ�ø��豸
	EnterCriticalSection( &pRasDev->csRasDev );
	if( (pRasDev->lpTspInfo) && (pTspTbl=pRasDev->lpTspInfo->pTspTbl) && (pRasDev->hLineDev) )
	{
		(*pTspTbl->TSPI_lineNotify)( pRasDev->hLineDev, PROEVT_DEV_STOP );
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	Sleep(1);

	//4. ֪ͨ PPP��رո��豸�ӿ�
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->hIfDl && pRasDev->pFnDlNotify )
	{
		(*pRasDev->pFnDlNotify)( pRasDev->hIfDl, PROEVT_DEV_CLOSE );
		pRasDev->hIfDl = NULL;
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	Sleep(1);

	//5. TSP�رո��豸�ӿ�
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pTspTbl && (pRasDev->hLineDev) )
	{
		// 1 TSP�رո��豸�ӿ�
		(*pTspTbl->TSPI_lineNotify)( pRasDev->hLineDev, PROEVT_DEV_CLOSE );
		pRasDev->hLineDev = NULL;
		// 2 Check TSP Dll wether free
		RasT_FreeTsp( pRasDev->lpTspInfo );	//
		pRasDev->lpTspInfo = NULL;
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	Sleep(1);

	//6. ���ø��豸�Ĳ���״̬
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pRasDev->wDevOp &= ~RASOP_HANGUP;
	if( pRasDev->wDevOp & RASOP_FREE )
	{
		List_RemoveUnit( &pRasDev->hListRasDev );
	}
	else
	{
		pRasDev->dwDevStatus = RASDEV_IDLE;
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );
	Sleep(1);

	//7. ɾ�����豸
	if( pRasDev->wDevOp & RASOP_FREE )
	{
		RETAILMSG(1,(TEXT("\r\n RasT_ThrdDiscnn: Free device!\r\n")));
		RasT_FreeDev( pRasDev );
	}

	RETAILMSG(1,(TEXT("\r\n RasT_ThrdDiscnn: leave ok\r\n")));
	return 0;
}


BOOL	RasT_BeginStat( RAS_DEV* pRasDev )
{
	//
	pRasDev->dwTickCnn = GetTickCount();
	pRasDev->dwBytesXmited = 0;
	pRasDev->dwBytesRcved = 0;
	//
#ifdef RAS_BPS_1
	pRasDev->dwRxBps = 0;
	pRasDev->dwBytesRx_Bps = 0;
	pRasDev->dwTxBps = 0;
	pRasDev->dwBytesTx_Bps = 0;
	return RasT_EnableBps( pRasDev, TRUE );
#else
	pRasDev->dwBytesRx_Bps = 0;
	pRasDev->dwTickRx_Bps = GetTickCount();

	pRasDev->dwBytesTx_Bps = 0;
	pRasDev->dwTickTx_Bps = GetTickCount();
	return TRUE;
#endif
}

BOOL	RasT_EnableBps( RAS_DEV* pRasDev, BOOL fEnableBps )
{
#ifdef RAS_BPS_1
	//��ʼ��
	pRasDev->dwRxBps = 0;
	pRasDev->dwTxBps = 0;
	//
	if( fEnableBps )
	{
		DWORD	dwThrdID;

		pRasDev->dwBytesRx_Bps = 0;
		pRasDev->dwBytesTx_Bps = 0;
		pRasDev->dwTickRx_Bps = GetTickCount();
		pRasDev->dwTickTx_Bps = GetTickCount();
		pRasDev->wDevOp &= ~RASOP_NO_BPS;
		Sleep( 1 );
		//�߳� ����
		if( pRasDev->hThrdBps )
		{
			return TRUE;
		}
		//�߳� ����
		pRasDev->hThrdBps = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)RasT_ThrdStatBps, (LPVOID)pRasDev, 0, &dwThrdID );
		if( !pRasDev->hThrdBps )
		{
			return FALSE;
		}
	}
	else
	{
		if( pRasDev->hThrdBps )
		{
			pRasDev->wDevOp |= RASOP_NO_BPS;
			SetEvent( pRasDev->hEvtBps );
		}
	}
#endif
	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN lpParam-����RAS�豸�ľ��
// ����ֵ��
//	0
// �����������Ҷ� 1���������� �� �豸����
// ����: 
// ********************************************************************
#ifdef RAS_BPS_1
DWORD	WINAPI	RasT_ThrdStatBps( LPVOID lpParam )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)lpParam;
	DWORD		dwTickCur;

	while( 1 )
	{
		//�ȴ�
		WaitForSingleObject( pRasDev->hEvtBps, TIMER_STATBPS );
		if( !HANDLE_CHECK(pRasDev) )
		{
			break;
		}
		if( (pRasDev->wDevOp & (RASOP_HANGUP|RASOP_FREE|RASOP_NO_BPS)) || (pRasDev->dwDevStatus!=RASDEV_LINK) )
		{
			break;
		}
		dwTickCur = GetTickCount();
		//ͳ��---�ٶ�---����
		//pRasDev->dwRxBps = InterlockedExchange( &pRasDev->dwBytesRx_Bps, 0 )/(dwTickCur-InterlockedExchange( &pRasDev->dwTickRx_Bps, dwTickCur ));
		pRasDev->dwRxBps = (pRasDev->dwBytesRx_Bps*1000)/(dwTickCur-pRasDev->dwTickRx_Bps);
		pRasDev->dwBytesRx_Bps = 0;
		pRasDev->dwTickRx_Bps = dwTickCur;
		//ͳ��---�ٶ�---����
		//pRasDev->dwRxBps = InterlockedExchange( &pRasDev->dwBytesRx_Bps, 0 )/(dwTickCur-InterlockedExchange( &pRasDev->dwTickRx_Bps, dwTickCur ));
		pRasDev->dwTxBps = (pRasDev->dwBytesTx_Bps*1000)/(dwTickCur-pRasDev->dwTickTx_Bps);
		pRasDev->dwBytesTx_Bps = 0;
		pRasDev->dwTickTx_Bps = dwTickCur;
	}
	//
	if( pRasDev->hThrdBps )
	{
		CloseHandle( pRasDev->hThrdBps );
		pRasDev->hThrdBps = NULL;
	}

	return 0;
}
#endif

