/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����TSP ��׼ ����
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-12-01
���ߣ�    ФԶ��
�޸ļ�¼��

  ФԶ�� 2004-05-11ע�ͣ�
    1�����Ҫ�����dll��ʽ���� define _USRDLL
    2�����Ҫ�����modem��ʽ���� define RDT_MDM
    3�����Ҫ�����pppoed��ʽ���� define RDT_PPPOE
    4�������PC���ϣ��� define VC386

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
//"\inc_drv"
#include "tapi.h"
//"local"
#define RDT_MDM
#include "tsp_fun.h"
#include "uni_dev.h"
#include "uni_mdm.h"


/***************  ȫ���� ���壬 ���� *****************/

//ȫ�ֱ���
static	TSP_GLOBAL*	g_lpGlobalUni = NULL;
static	TSPITBL		g_hFunTblTsp;


//�ڲ�����
static	BOOL	TSPI_Init( HINSTANCE hInst );
static	void	TSPI_Deinit( );

static	void	lineDev_Find( DWORD dwEnumID, OUT LINE_DEV** ppLineDev );


/******************************************************/


#ifdef	_USRDLL

// ********************************************************************
// ������
// ������
//	IN hInstDLL-ʵ��
//	IN dwReason-����
//	IN lpvReserved-����
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ����������DLL��ں���
// ����: 
// ********************************************************************
BOOL	APIENTRY	DllMain( HANDLE hInstDLL, DWORD dwReason, LPVOID lpvReserved )
{
    switch (dwReason) 
    {
    case DLL_PROCESS_ATTACH:
		return TSPI_Init( hInstDLL );
		break;
	case DLL_PROCESS_DETACH:
		TSPI_Deinit( );
		break;
    default:
        break;
    }
    return TRUE;
}

#endif	//_USRDLL

// ********************************************************************
// ������
// ������
//	OUT ppTspiTbl-����TSP�ĺ�����
//	IN hInst-ʵ��
//	IN pNotifyRas-SP��RAS�Ļص�֪ͨ����
// ����ֵ��
//	��
// ������������ȡ TSP�ĺ������ѻص����� ����TSP
// ����: 
// ********************************************************************
UNIMODEM_API	void	TSPI_GetTable( OUT TSPITBL** ppTspiTbl, HINSTANCE hInst, FNRASNOTIFY pNotifyRas )
{
	TSPI_Init( hInst );

	//g_lpGlobalUni->pFnNotifyRas = pNotifyRas;
	*ppTspiTbl = &g_hFunTblTsp;
}

// ********************************************************************
// ������
// ������
//	IN dwEnumID---ָ�����豸ID
//	IN pszDrvReg-�豸������ע���λ��
//	IN lpRasOpenDev-�豸�Ĵ򿪷�ʽ
// ����ֵ��
//	�ɹ������ؾ��
// ������������ȡ TSP�ĺ������ѻص����� ����TSP
// ����: 
// ********************************************************************
UNIMODEM_API	HANDLE	TSPI_CreateDev( DWORD dwEnumID, LPTSTR pszDrvReg, RASDEVOPEN* lpRasOpenDev )
{
	LINE_DEV*	pLineDev;
	DWORD		dwDevType;

	//Ѱ�� TSP�豸
	lineDev_Find( dwEnumID, &pLineDev );
	if( pLineDev )
	{
		return (HANDLE)pLineDev;
	}
	//ȷ�� TSP�豸������ �� TSP DLL������ ������ϵġ�
	if( !lineDev_CheckType( pszDrvReg, &dwDevType ) )
	{
		return NULL;
	}
	//���� TSP�豸
	pLineDev = (LINE_DEV*)HANDLE_ALLOC( sizeof(LINE_DEV) );
	if( !pLineDev )
	{
		return NULL;
	}
	HANDLE_INIT( pLineDev, sizeof(LINE_DEV) );
	InitializeCriticalSection( &pLineDev->csDev );
	List_InitHead( &pLineDev->hListDev );

	//��ʼ��
	pLineDev->dwEnumID = dwEnumID;
	memcpy( &pLineDev->rasOpen, lpRasOpenDev, sizeof(RASDEVOPEN) );
	strcpy( pLineDev->szDrvReg, pszDrvReg );

	pLineDev->dwDevType = dwDevType;
	pLineDev->dwDevState = MDM_IDLE;

	if( !lineDev_Create(pLineDev) )
	{
		lineDev_Free( pLineDev );
		return NULL;
	}

	//��� RAS�豸
	EnterCriticalSection( &g_lpGlobalUni->csListDev );
	List_InsertTail( &g_lpGlobalUni->hListDev, &pLineDev->hListDev );
	LeaveCriticalSection( &g_lpGlobalUni->csListDev );
	
	return (HANDLE)pLineDev;
}


// ********************************************************************
// ������
// ������
//	IN hLineDev---ָ����TSP�豸
//	IN pParamOpen-�豸�Ĵ򿪷�ʽ
// ����ֵ��
//	�ɹ������ؾ��
// �����������󶨡��� �� �����豸
// ����: 
// ********************************************************************
UNIMODEM_API	BOOL	TSPI_OpenDev( HANDLE hLineDev, PARAM_OPENDEV* pParamOpen )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pLineDev) )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_NOTIDLE );
		return FALSE;
	}
	//�ж��豸״̬���󶨡����沦����Ŀ�����������豸�ȣ�����ʾ���豸���жϴ򿪷�ʽ
	EnterCriticalSection( &pLineDev->csDev );
	//�ж��豸״̬
	if( pLineDev->dwDevState!=MDM_IDLE )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_NOTIDLE );
		LeaveCriticalSection( &pLineDev->csDev );
		return FALSE;
	}
	//�󶨡����沦����Ŀ�����������豸�ȣ�
	pLineDev->hRasDev = pParamOpen->hRasDev;
	strcpy( pLineDev->szEntryName, pParamOpen->pszEntry );

	//��ʾ���豸
	RASEVT_SEND( RASCS_OpenPort, RASERR_SUCCESS );

	//�жϴ򿪷�ʽ��׼��
	if( pLineDev->rasOpen.fOpenName )
	{
		if( pLineDev->hPort && pLineDev->hPort!=INVALID_HANDLE_VALUE )
		{
			CloseHandle( pLineDev->hPort );
			pLineDev->hPort = NULL;
		}
	}
	else
	{
		pLineDev->hPort = pLineDev->rasOpen.hDev;
	}

	LeaveCriticalSection( &pLineDev->csDev );

	//�� �� �����豸
	return lineDev_Open( pLineDev );
}


// ********************************************************************
// ������
// ������
//	IN hLineDev---ָ����TSP�豸
//	OUT pInfoDl-����TSP�豸�ṩ����·����Ϣ
// ����ֵ��
//	�ɹ�������TRUE
// ������������ȡTSP�豸�ṩ����·����Ϣ
// ����: 
// ********************************************************************
UNIMODEM_API	BOOL	TSPI_lineGetInfo( HANDLE hLineDev, OUT TSP_DLINFO* pInfoDl )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pLineDev) )
	{
		return FALSE;
	}
	//
	if( !lineDev_GetInfo( pLineDev, pInfoDl ) )
	{
		RASEVT_SEND( RASCS_DeviceConnected, RASERR_TSP_NOINFO );
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


// ********************************************************************
// ������
// ������
//	IN hLineDev---ָ����TSP�豸
//	IN pDialParam-�豸�Ĳ��Ų���
// ����ֵ��
//	�ɹ�������TRUE
// �����������豸����
// ����: 
// ********************************************************************
UNIMODEM_API	BOOL	TSPI_lineDial( HANDLE hLineDev, LPCSTR pszLocalNumber )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pLineDev) )
	{
		return FALSE;
	}

	return lineDev_Dial( pLineDev, pszLocalNumber );
}


// ********************************************************************
// ������
// ������
//	IN hLineDev---ָ����TSP�豸
//	IN hIfDl---TSP�豸�����ݵ� �����߾��
//	IN pFnIn---TSP�豸�����ݵ� �����ߺ���
// ����ֵ��
//	�ɹ�������TRUE
// �������������� TSP�豸�����ݵ� �����ߣ����л���MDM_DATA
// ����: 
// ********************************************************************
UNIMODEM_API	void	TSPI_lineSetRecv( HANDLE hLineDev, HANDLE hIfDl, FNPROIN pFnIn )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pLineDev) )
	{
		return ;
	}
	//lineDev_SetRecv( pLineDev, hIfDl, pFnIn );
	//��modem�л���  ����״̬
	lineDev_NewState( pLineDev, MDM_DATA );
}


// ********************************************************************
// ������
// ������
//	IN hLineDev---ָ����TSP�豸
//	IN pBuf---Ҫ���͵�����
//	IN dwLen---Ҫ���͵����ݵĳ���
// ����ֵ��
//	�ɹ�������TRUE
// ����������TSP�豸������ ���ͺ���
// ����: 
// ********************************************************************
UNIMODEM_API	DWORD	TSPI_lineSend( HANDLE hLineDev, LPBYTE pBuf, DWORD dwLen )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pLineDev) )
	{
		return MR_FAILURE;
	}

	return lineDev_Send( pLineDev, pBuf, dwLen );
}


// ********************************************************************
// ������
// ������
//	IN hLineDev---ָ����TSP�豸
//	IN dwEvent---TSP�豸���¼�
// ����ֵ��
//	����0
// ����������RAS ͨ���ú��� ֪ͨTSP�豸�Ĳ���
// ����: 
// ********************************************************************
UNIMODEM_API	DWORD	TSPI_lineNotify( HANDLE hLineDev, DWORD dwEvent )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//�����ȫ���
	if( !HANDLE_CHECK(pLineDev) )
	{
		return 0;
	}
	if( dwEvent==PROEVT_DEV_STOP )
	{
		// 0 ͣ�ø��豸,  
		lineDev_Stop0( pLineDev );

		// 1 ͣ�ø��豸,  
		lineDev_NewState( pLineDev, MDM_DOWN );
		pLineDev->dwDevState = MDM_DOWN;
		Sleep( 1 );
		pLineDev->dwDevState = MDM_DOWN;
		//2 LINE_DEVȥ��������PPP��ص���Ϣ
		//pLineDev->pFnDlIn = NULL;
		
		//2 stop �豸 
		lineDev_Stop( pLineDev );
	}
	else if( dwEvent==PROEVT_DEV_CLOSE )
	{
		//�ͷŵ�ǰ�豸
		EnterCriticalSection( &g_lpGlobalUni->csListDev );
		List_RemoveUnit( &pLineDev->hListDev );
		LeaveCriticalSection( &g_lpGlobalUni->csListDev );

		lineDev_Free( pLineDev );
	}

	return 0;
}





//--------------------------------------------------------------------------------------------
// TSP �ڲ�����..........................
//--------------------------------------------------------------------------------------------




// ********************************************************************
// ������
// ������
//	IN dwEnumID---ָ�����豸ID
//	OUT ppLineDev-������ҵ����豸
// ����ֵ��
//	��
// ��������������ָ���豸ID���豸
// ����: 
// ********************************************************************
void	lineDev_Find( DWORD dwEnumID, OUT LINE_DEV** ppLineDev )
{
	LINE_DEV*	pLineDev = NULL;
	BOOL		fFind = FALSE;
	PLIST_UNIT	pHeader;
	PLIST_UNIT	pUnit;

	pHeader = &g_lpGlobalUni->hListDev;
	EnterCriticalSection( &g_lpGlobalUni->csListDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pLineDev = LIST_CONTAINER( pUnit, LINE_DEV, hListDev );
		pUnit = pUnit->pNext;    // advance to next 

		if( pLineDev->dwEnumID==dwEnumID )
		{
			fFind = TRUE;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalUni->csListDev );

	if( fFind )
	{
		*ppLineDev = pLineDev;
	}
	else
	{
		*ppLineDev = 0;
	}
}


// ********************************************************************
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
//	IN dwDevState---ָ����TSP�豸����״̬
// ����ֵ��
//	�ɹ�TRUE
// �������������� TSP�豸����״̬��ֻҪԭ״̬����MDM_DOWN�����򷵻�TRUE
// ����: 
// ********************************************************************
BOOL	lineDev_NewState( LINE_DEV* pLineDev, DWORD dwDevState )
{
	BOOL	fSuccess;

	EnterCriticalSection( &pLineDev->csDev );
	if( pLineDev->dwDevState!=MDM_DOWN )
	{
		fSuccess = TRUE;
		pLineDev->dwDevState = dwDevState;
	}
	else
	{
		fSuccess = FALSE;
		if( pLineDev->hEvtCmd )
		{
			SetEvent( pLineDev->hEvtCmd );
			Sleep( 2 );
		}
	}
	LeaveCriticalSection( &pLineDev->csDev );
	RETAILMSG(1,(TEXT("\r\n\r\n lineDev_NewState: dwDevState=[0x%x]\r\n\r\n"), pLineDev->dwDevState));

	return fSuccess;
}

// ********************************************************************
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
// ����ֵ��
//	�ɹ�TRUE
// �����������ж� TSP�豸��״̬���ǲ���MDM_DOWN
// ����: 
// ********************************************************************
BOOL	lineDev_IsDown( LINE_DEV* pLineDev )
{
	BOOL	fDown;
	
	if( !HANDLE_CHECK(pLineDev) )
	{
		return TRUE;
	}

	EnterCriticalSection( &pLineDev->csDev );
	if( pLineDev->dwDevState!=MDM_DOWN )
	{
		fDown = FALSE;
	}
	else
	{
		fDown = TRUE;
	}
	LeaveCriticalSection( &pLineDev->csDev );

	return fDown;
}




// ********************************************************************
// ������
// ������
//	IN hInstDLL-ʵ��
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ����������TSP DLL�ĳ�ʼ��
// ����: 
// ********************************************************************
BOOL	TSPI_Init( HINSTANCE hInst )
{
	//���� g_hFunTblTsp
	if( g_lpGlobalUni && g_hFunTblTsp.TSPI_CreateDev==TSPI_CreateDev && g_hFunTblTsp.TSPI_lineDial==TSPI_lineDial )
	{
		return TRUE;
	}
	//
	g_lpGlobalUni = (TSP_GLOBAL*)HANDLE_ALLOC( sizeof(TSP_GLOBAL) );
	if( !g_lpGlobalUni )
	{
		return FALSE;
	}
	HANDLE_INIT( g_lpGlobalUni, sizeof(TSP_GLOBAL) );
	List_InitHead( &g_lpGlobalUni->hListDev );
	InitializeCriticalSection( &g_lpGlobalUni->csListDev );
	//
	g_lpGlobalUni->hInst = (HINSTANCE)hInst;
	g_hFunTblTsp.TSPI_CreateDev = TSPI_CreateDev;
	g_hFunTblTsp.TSPI_OpenDev = TSPI_OpenDev;
	//g_hFunTblTsp.TSPI_CloseDev = TSPI_CloseDev;
	g_hFunTblTsp.TSPI_lineNotify = TSPI_lineNotify;

	g_hFunTblTsp.TSPI_lineDial = TSPI_lineDial;
	g_hFunTblTsp.TSPI_lineGetInfo = TSPI_lineGetInfo;
	g_hFunTblTsp.TSPI_lineSetRecv = TSPI_lineSetRecv;
	g_hFunTblTsp.TSPI_lineSend = TSPI_lineSend;

	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN hInstDLL-ʵ��
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ����������TSP DLL���ͷ�
// ����: 
// ********************************************************************
void	TSPI_Deinit( )
{
	LINE_DEV*	pLineDev = NULL;
	BOOL		fFind = FALSE;
	PLIST_UNIT	pHeader;
	PLIST_UNIT	pUnit;

	//�ͷ������豸
	pHeader = &g_lpGlobalUni->hListDev;
	EnterCriticalSection( &g_lpGlobalUni->csListDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pLineDev = LIST_CONTAINER( pUnit, LINE_DEV, hListDev );
		pUnit = pUnit->pNext;    // advance to next 

		if( HANDLE_CHECK(pLineDev) )
		{
			List_RemoveUnit( &pLineDev->hListDev );
			lineDev_Free( pLineDev );
		}
	}
	LeaveCriticalSection( &g_lpGlobalUni->csListDev );

	//�ͷ�TSPI��������Ϣ
	DeleteCriticalSection( &g_lpGlobalUni->csListDev );
	free( g_lpGlobalUni );
	g_lpGlobalUni = NULL;
}

