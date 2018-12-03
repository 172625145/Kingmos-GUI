/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����TSP Line Device ����
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-12-01
���ߣ�    ФԶ��
�޸ļ�¼��
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


/***************  ȫ���� ���壬 ���� *****************/
/******************************************************/



// ********************************************************************
// ������
// ������
//	IN pszDrvReg-�豸������ע���λ��
//	OUT pdwDevType---�����豸������
// ����ֵ��
//	�ɹ�TRUE
// ������������ȡ�豸������
// ����: 
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
// ������
// ������
//	IN dwEnumID---ָ�����豸ID
//	IN lpRasOpenDev-�豸�Ĵ򿪷�ʽ
//	IN pszDrvReg-�豸������ע���λ��
// ����ֵ��
//	��
// ��������������ָ���豸ID���豸
// ����: 
// ********************************************************************
BOOL	lineDev_Create( LINE_DEV* pLineDev )
{
	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
// ����ֵ��
//	�ɹ������ؾ��
// ������������ �� �����豸
// ����: 
// ********************************************************************
BOOL	lineDev_Open( LINE_DEV* pLineDev )
{
	//1> �򿪺����ö˿ڣ�������OPENED״̬�������빤��״̬
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
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
//	IN pDialParam-�豸�Ĳ��Ų���
// ����ֵ��
//	�ɹ�������TRUE
// �����������豸����
// ����: 
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
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
//	IN pDialParam-�豸�Ĳ��Ų���
// ����ֵ��
//	�ɹ�������TRUE
// �����������豸����
// ����: 
// ********************************************************************
BOOL	lineDev_Dial( LINE_DEV* pLineDev, LPCSTR pszLocalNumber )
{
	RASENTRY	stRasEntry;
	DWORD		dwSize;
	TCHAR		pszDialNum[200+8];
	LPTSTR		pszTmp;
	PBYTE p = (PBYTE)&stRasEntry;
	//��ȡ ������Ŀ
	dwSize = sizeof(RASENTRY);
	memset( &stRasEntry, 0, dwSize );
	if( !RAPI_GetEntryProperties( NULL, pLineDev->szEntryName, &stRasEntry, &dwSize) )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_ENTRY );
		return FALSE;
	}

	// �������š������в��ų�ʼ��
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
		// ��ȡ ������Ϣ
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

	//��ʼ����
	RASEVT_SEND( RASCS_ConnectDevice, ERROR_SUCCESS );
	pLineDev->fDialed = TRUE;
	return UniMdm_MakeCall( pLineDev, pszDialNum, stRasEntry.dwCntRedial );
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
void	lineDev_SetRecv( LINE_DEV* pLineDev, HANDLE hIfDl, FNPROIN pFnIn )
{
	//pLineDev->hIfDl = hIfDl;
	//pLineDev->pFnDlIn = pFnIn;
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
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
// ����ֵ��
//	��
// ����������ͣ��TSP�豸
// ����: 
// ********************************************************************
void	lineDev_Stop0( LINE_DEV* pLineDev )
{
	// LINE_DEVȥ��������PPP��ص���Ϣ
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
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
// ����ֵ��
//	��
// ����������ͣ��TSP�豸
// ����: 
// ********************************************************************
void	lineDev_Stop( LINE_DEV* pLineDev )
{
#if 0
	// LINE_DEVȥ��������PPP��ص���Ϣ
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
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
// ����ֵ��
//	�ɹ�������TRUE
// �����������ͷŲ����豸
// ����: 
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
