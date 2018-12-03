/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����TSP MODEM ����
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
#include "uni_mdm.h"


/***************  ȫ���� ���壬 ���� *****************/

#ifdef VC386
#define	MDM_LOOK_CTS
#endif

//#define	MDM_SET_BAUD

#ifdef VC386
#undef	MDM_SET_BAUD
#endif

#define	MDM_INIT_SET

#define SEARCH_GPRS_NET

#ifndef VC386
#undef	SEARCH_GPRS_NET
#endif

//
static	const	char	MDM_CRLF_STR			[]="\r\n";
static	const	char	MDM_OK_STR				[]="\r\nOK\r\n";
static	const	char	MDM_ERR_STR				[]="\r\nERROR\r\n";
static	const	char	MDM_NOCAR_STR			[]="\r\nNO CARRIER\r\n";
static	const	char	MDM_NOANS_STR			[]="\r\nNO ANSWER\r\n";
static	const	char	MDM_NOTONE_STR			[]="\r\nNO DIALTONE\r\n";
static	const	char	MDM_BUSY_STR			[]="\r\nBUSY\r\n";
static	const	char	MDM_CONNECT_STR			[]="CONNECT";

static	const	char	MDM_OK_STR2				[]="OK";
static	const	char	MDM_ERR_STR2			[]="ERROR";
static	const	char	MDM_NOCAR_STR2			[]="NO CARRIER";
static	const	char	MDM_NOANS_STR2			[]="NO ANSWER";

static	LPSTR	strstrX( LPCSTR pStr1, DWORD dwLenStr1, LPCSTR pStr2, DWORD dwLenStr2 );
static	BOOL	Mdm_StartMonitor( LINE_DEV* pLineDev );
static	BOOL	MdmAT_TestOk( LINE_DEV* pLineDev, DWORD dwWait );
static	BOOL	MdmAT_Set1( LINE_DEV* pLineDev );
static	BOOL	MdmAT_Setting( LINE_DEV* pLineDev );

static	BOOL	GsmAT_ReadGprsReg( LINE_DEV* pLineDev, OUT BYTE* pbNetStatus );
static	BOOL	GsmAT_AttachGprs( LINE_DEV* pLineDev, BOOL fIsAttach );
static	DWORD	GsmAT_SetGprsContext( LINE_DEV* pLineDev, DWORD dwCid, LPSTR pszApn );


static	BOOL	Mdm_FindResp( LINE_DEV* pLineDev, IN OUT MDM_CMD_EXE* pInfoCmd, DWORD dwExeFlag, OUT DWORD* pdwRet );
static	BOOL	Mdm_ReadResp( LINE_DEV* pLineDev, HANDLE hPort );
static	BOOL	Mdm_WaitResp( LINE_DEV* pLineDev, IN OUT MDM_CMD_EXE* pInfoCmd, DWORD dwExeFlag, DWORD dwWait );

static	LPSTR	Mdm_StrAndCRLF( LPSTR pszBufRes, LPSTR pszBufFind, DWORD dwLenFind, BOOL* pfNoCRLF );
static	DWORD	Mdm_GetCbInQue( HANDLE hPort );

static	DWORD	WINAPI	Mdm_ThrdMonitor( LPVOID lpParam );
static	BOOL	Mdm_SetBaud( LINE_DEV* pLineDev, DWORD dwBaud );

/******************************************************/

// ********************************************************************
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
// ����ֵ��
//	�ɹ�������TRUE
// �����������������š������в��ų�ʼ��
// ����: 
// ********************************************************************
BOOL	UniMdm_PrepareDial( LINE_DEV* pLineDev )
{
#ifdef MDM_SET_BAUD
	MDM_CMD_EXE		InfoCmd;
#endif

	// ��������---Modem����¼��Ͷ����ݵ��߳�
	if( !Mdm_StartMonitor(pLineDev) )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_START );
		return FALSE;
	}
	// �����豸�Ƿ��л�Ӧ
	if( !MdmAT_TestOk(pLineDev, 25*1000) )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_TESTMDM );
		return FALSE;
	}
#ifdef MDM_SET_BAUD
	InfoCmd.pszCmd = "AT+IPR=9600\r";
	InfoCmd.dwLenCmd = sizeof("AT+IPR=9600\r") - 1;
	if( UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDERR, 3000 )!=MR_OK )
	{
		return FALSE;
	}
	if( !Mdm_SetBaud( pLineDev, 9600 ) )
	{
		return FALSE;
	}
#endif
	// ��ʼ���豸�Ĳ���
	if( !MdmAT_Setting(pLineDev) )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_SETMDM );
		return FALSE;
	}
	return TRUE;
}

BOOL	MdmAT_TestOk( LINE_DEV* pLineDev, DWORD dwWait )
{
	DWORD			dwRet;
	DWORD			dwTick;
	DWORD			dwCntTest;

	MDM_CMD_EXE		InfoCmd;

	//
	InfoCmd.nTryNum = 3;
	dwCntTest = 0;
	dwTick = GetTickCount();
	//
	while( 1 )
	{
		if( lineDev_IsDown(pLineDev) )
		{
			return FALSE;
		}
		//��λ
		if( (++dwCntTest)%8==0 )
		{
			InfoCmd.pszCmd = "ATZ\r";
			InfoCmd.dwLenCmd = sizeof("ATZ\r") - 1;
			if( UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_TRYONE|MEXEF_FINDERR, 1500 )==MR_DOWN )
			{
				return FALSE;
			}
		}
		//����
		InfoCmd.pszCmd = "ATE0V1\r";
		InfoCmd.dwLenCmd = sizeof("ATE0V1\r") - 1;
		if( UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_TRYONE|MEXEF_ONLYWRITE|MEXEF_FINDERR, 1000 )==MR_DOWN )
		{
			return FALSE;
		}
		//����
		InfoCmd.pszCmd = "AT\r";
		InfoCmd.dwLenCmd = sizeof("AT\r") - 1;
		if( (dwRet=UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_TRYONE|MEXEF_FINDERR, 1200 ))==MR_DOWN )
		{
			return FALSE;
		}
		//�������
		if( dwRet==MR_OK )
		{
			return TRUE;
		}
		//����
		if( RASDT_GETSUB(pLineDev->dwDevType)==RASDT_SUB_GPRS )
		{
			InfoCmd.pszCmd = "+++";
			InfoCmd.dwLenCmd = sizeof("+++") - 1;
			if( (dwRet=UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_TRYONE|MEXEF_FINDERR, 2500 ))==MR_DOWN )
			{
				return FALSE;
			}
			if(dwRet == MR_OK) //Added by tanphy
			{
				ASSERT(GsmAT_AttachGprs(pLineDev, FALSE));
			}
		}
		InfoCmd.pszCmd = "ATH\r";
		InfoCmd.dwLenCmd = sizeof("ATH\r") - 1;
		UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_TRYONE|MEXEF_MUSTOK|MEXEF_FINDMEE|MEXEF_FINDERR, 1500 );
		//�������
		if( dwRet==MR_OK )
		{
			return TRUE;
		}

		//��ʱ�˳�
		if( (GetTickCount()-dwTick)>dwWait )
		{
			break;
		}
	}

	return FALSE;
}

BOOL	MdmAT_Set1( LINE_DEV* pLineDev )
{
	DWORD			dwRet;
	MDM_CMD_EXE		InfoCmd;
	//BOOL			fEnableCW;

	//׼��
	InfoCmd.nTryNum = 4;

	//
	// ��ͨmodemһ��Ҫ֧�ֵ�
	//
	//
	//set "RLSD remains ON at all times", and "DTR drop causes the modem to hang up."
	InfoCmd.pszCmd = "ATE0V1&C0&D2\r";
	InfoCmd.dwLenCmd = sizeof("ATE0V1&C0&D2\r") - 1;
	if( (dwRet=UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDERR, 1200 ))!=MR_OK )
	{
		return FALSE;
	}
	//
	//set "Enales monitoring of usy tones: send only OK, CONNECT, RING, NO CARRIER, ERROR, NO ANSWER, AND ...."
	InfoCmd.pszCmd = "ATX3\r";
	InfoCmd.dwLenCmd = sizeof("ATX3\r") - 1;
	if( (dwRet=UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDERR, 1200 ))!=MR_OK )
	{
		return FALSE;
	}
	//
	//set "wait time for carrier"
	InfoCmd.pszCmd = "ATS7=120\r";
	InfoCmd.dwLenCmd = sizeof("ATS7=120\r") - 1;
	if( (dwRet=UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDERR, 1200 ))!=MR_OK )
	{
		return FALSE;
	}

	//GPRS ����
	if( RASDT_GETSUB(pLineDev->dwDevType)!=RASDT_SUB_GPRS )
	{
		InfoCmd.pszCmd = "ATM0\r";
		InfoCmd.dwLenCmd = sizeof("ATM1\r") - 1;
		if( (dwRet=UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDERR, 1200 ))!=MR_OK )
		{
			return FALSE;
		}
		InfoCmd.pszCmd = "ATL1\r";
		InfoCmd.dwLenCmd = sizeof("ATL1\r") - 1;
		if( (dwRet=UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDERR, 1200 ))!=MR_OK )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL	MdmAT_SetGprs( LINE_DEV* pLineDev )
{
	int		i;
#ifdef SEARCH_GPRS_NET
	int		j;
#endif
	char	szApn[LEN_GPRSAPN];
	BYTE	bGprsNetS;
	
	//���������� APN
	if( GprsCfg_GetApn( GPRS_NETTYPE_CURRENT, szApn, LEN_GPRSAPN ) )
	{
		bGprsNetS = 0;
		for( i=0; i<40; i++ )
		{
			if( lineDev_IsDown(pLineDev) )
			{
				return FALSE;
			}
			if( GsmAT_SetGprsContext( pLineDev, 1, szApn ) )
			{
				break;
			}
			if( bGprsNetS!=0x30 )
			{
				if( !GsmAT_ReadGprsReg( pLineDev, &bGprsNetS ) )
				{
					continue;
				}
				if( (bGprsNetS==0x31) || (bGprsNetS==0x35) )
				{
					GsmAT_AttachGprs( pLineDev, FALSE );
				}
			}
			Sleep( 5 );
		}
	}

	//��ѯ����
	bGprsNetS = 0;
#ifdef SEARCH_GPRS_NET
	for( i=0,j=0; i<18 && j<40; )
	{
		if( lineDev_IsDown(pLineDev) )
		{
			return FALSE;
		}
		//RETAILMSG(1,(TEXT("  tsp_unimodem: look gprs\r\n")));
		if( !GsmAT_ReadGprsReg( pLineDev, &bGprsNetS ) )
		{
			i ++;
			continue;
		}
		if( bGprsNetS==0x32 )
		{
			j ++;
			Sleep( 800 );
			continue;
		}
		if( (bGprsNetS==0x31) || (bGprsNetS==0x35) )
		{
			break;
		}
		if( bGprsNetS==0x33 )
		{
			return FALSE;
		}
		//RETAILMSG(1,(TEXT("  tsp_unimodem: attach gprs\r\n")));
		GsmAT_AttachGprs( pLineDev, TRUE );
		Sleep( 5 );
		i ++;
	}
	if( (bGprsNetS!=0x31) && (bGprsNetS!=0x35) )
	{
		return FALSE;
	}
#else
#if 0
	for( i=0,j=0; i<18 && j<40; )
	{
		if( lineDev_IsDown(pLineDev) )
		{
			return FALSE;
		}
		//RETAILMSG(1,(TEXT("  tsp_unimodem: look gprs\r\n")));
		if( !GsmAT_ReadGprsReg( pLineDev, &bGprsNetS ) )
		{
			i ++;
			continue;
		}
		if( bGprsNetS==0x30 )
		{
			return TRUE;
		}

		//RETAILMSG(1,(TEXT("  tsp_unimodem: attach gprs\r\n")));
		GsmAT_AttachGprs( pLineDev, FALSE );
		Sleep( 5 );
		i ++;
	}
#endif
#endif
	return TRUE;
}

BOOL	MdmAT_Setting( LINE_DEV* pLineDev )
{
	int		i;

	//
	for( i=0; i<10; i++ )
	{
		if( lineDev_IsDown(pLineDev) )
		{
			return FALSE;
		}
		if( MdmAT_Set1(pLineDev) )
		{
			break;
		}
		Sleep( 5 );
	}
	if( i>=10 )
	{
		return FALSE;
	}
	//
	if( RASDT_GETSUB(pLineDev->dwDevType)==RASDT_SUB_GPRS )
	{
		pLineDev->fDialed = TRUE;
		if( !MdmAT_SetGprs(pLineDev) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL	GsmAT_ReadGprsReg( LINE_DEV* pLineDev, OUT BYTE* pbNetStatus )
{
	MDM_CMD_EXE		InfoCmd;
	LPSTR			pszResCur;
	BYTE			bGprsNetS;

	//ִ������
	InfoCmd.pszCmd = "AT+CGREG?\r";
	InfoCmd.dwLenCmd = strlen(InfoCmd.pszCmd);
	InfoCmd.pszPrefix = "+CGREG: ";
	InfoCmd.dwLenPrefix = strlen(InfoCmd.pszPrefix);
	if( UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_NOTFINDOK|MEXEF_FINDMEE|MEXEF_FINDPREFIX, 3000 )==MR_PREFIX )
	{
		pszResCur = InfoCmd.pszFound;
		//�õ���Ϣ��
		if( pszResCur[1]==',' )
		{
#if defined(GSM_WAVECOM)
			bGprsNetS = pszResCur[2];
#else
			if( pszResCur[2]!=0x20 )
			{
				bGprsNetS = pszResCur[2];
			}
			else
			{
				bGprsNetS = pszResCur[3];
			}
#endif
		}
		else
		{
			bGprsNetS = pszResCur[0];
		}
		*pbNetStatus = bGprsNetS;
		return TRUE;
	}
	return FALSE;
}

BOOL	GsmAT_AttachGprs( LINE_DEV* pLineDev, BOOL fIsAttach )
{
	MDM_CMD_EXE		InfoCmd;
	char			pszBufCmd[20];
	int				nLenCmd;

	//׼��
	nLenCmd = sprintf( pszBufCmd, "AT+CGATT=%d\r", fIsAttach?1:0 );
	//ִ������
	InfoCmd.pszCmd = pszBufCmd;
	InfoCmd.dwLenCmd = nLenCmd;
	if( UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDMEE, 12*1000 )==MR_OK )
	{
		return TRUE;
	}
	return FALSE;
}

DWORD	GsmAT_SetGprsContext( LINE_DEV* pLineDev, DWORD dwCid, LPSTR pszApn )
{
	MDM_CMD_EXE		InfoCmd;
	char			pszBufCmd[64];
	int				nLenCmd;

	//׼��
	nLenCmd = sprintf( pszBufCmd, "AT+CGDCONT=%d,\"IP\",\"%s\",,0,0\r", dwCid, pszApn );
	//ִ������жϽ��
	InfoCmd.pszCmd = pszBufCmd;
	InfoCmd.dwLenCmd = nLenCmd;
	if( UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDERR|MEXEF_FINDMEE, 2000 )==MR_OK )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL	UniMdm_MakeCall( LINE_DEV* pLineDev, LPSTR pszBufCmd, DWORD dwCntRedial )
{
	DWORD			dwExeFlag;
	DWORD			dwRet;
	MDM_CMD_EXE		InfoCmd;
	DWORD			i;
	DWORD			dwSecs;

	//
	if( RASDT_GETSUB(pLineDev->dwDevType)==RASDT_SUB_GPRS )
	{
		dwSecs = 5000;
		InfoCmd.dwReadWait = 10000;
	}
	else
	{
		dwSecs = 18000;//18000
		InfoCmd.dwReadWait = 18000;
	}
	dwRet = MR_NOCAR;
	//ִ������
	InfoCmd.pszCmd = pszBufCmd;
	InfoCmd.dwLenCmd = strlen(pszBufCmd);
	dwExeFlag = MEXEF_TRYONE|MEXEF_READWAIT|MEXEF_NOTFINDOK|MEXEF_FINDERR|MEXEF_FINDMEE|MEXEF_FINDATD;
	for( i=0; i<dwCntRedial; i++ )
	{
		RETAILMSG(1,(TEXT("\r\nlineDev_Dial: cnt=[%d], [%s]\r\n"), i, pszBufCmd));
		dwRet = UniMdm_SendCmd( pLineDev, &InfoCmd, dwExeFlag, dwSecs );
		if( dwRet==MR_CONNECT )
		{
			RASEVT_SEND( RASCS_DeviceConnected, RASERR_SUCCESS );
			return TRUE;
		}
		//if( RASDT_GETSUB(pLineDev->dwDevType)==RASDT_SUB_GPRS )
		//{
		//	GsmAT_AttachGprs( pLineDev, FALSE );
		//}
	}
	//
	switch( dwRet )
	{
	case MR_NOCAR:
		RASEVT_SEND( RASCS_DeviceConnected, RASERR_TSP_DIAL_NOCAR );
		return FALSE;
	case MR_BUSY:
		RASEVT_SEND( RASCS_DeviceConnected, RASERR_TSP_DIAL_BUSY );
		return FALSE;
	default :
		break;
	}
	Sleep( 2 );
	//�в�������
	RASEVT_SEND( RASCS_ConnectDevice, RASERR_TSP_DIAL );
	return FALSE;
}
void	UniMdm_HangupCall( LINE_DEV* pLineDev )
{
	MDM_CMD_EXE		InfoCmd;

	//ִ������
	GsmAT_AttachGprs( pLineDev, FALSE );

	InfoCmd.pszCmd = "ATH\r";
	InfoCmd.dwLenCmd = sizeof("ATH\r") - 1;
	UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDMEE, 2000 );

	if( RASDT_GETSUB(pLineDev->dwDevType)==RASDT_SUB_GPRS )
	{
		InfoCmd.pszCmd = "+++";
		InfoCmd.dwLenCmd = sizeof("+++") - 1;
		UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_MUSTOK|MEXEF_FINDMEE, 5000 );
	}
}


void UniMdm_SwitchToCmd( LINE_DEV* pLineDev )
{
	MDM_CMD_EXE		InfoCmd;
	DWORD dwRet;
	
	if(!lineDev_NewState(pLineDev, MDM_COMMAND))
	{
		ASSERT(0);
		return ;
	}
	InfoCmd.pszCmd = "+++";
	InfoCmd.dwLenCmd = sizeof("+++") - 1;
	InfoCmd.nTryNum = 2;//ִ������Ĵ���
	dwRet = UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_TRYNUM|MEXEF_MUSTOK|MEXEF_FINDMEE, 1500 );
	if(dwRet != MR_OK)
	{
		ASSERT(lineDev_NewState(pLineDev, MDM_DATA));
		ASSERT(0);
		return;
	}
	ASSERT(GsmAT_AttachGprs(pLineDev, FALSE));
	InfoCmd.pszCmd = "ATH\r";
	InfoCmd.dwLenCmd = sizeof("ATH\r") - 1;
	if( UniMdm_SendCmd( pLineDev, &InfoCmd, MEXEF_TRYONE|MEXEF_MUSTOK|MEXEF_FINDMEE, 1500 )!=MR_OK )
	{
		ASSERT(0);
	}
}


BOOL	UniMdm_OpenAndCfg( LINE_DEV* pLineDev )
{
	HANDLE			hPort;
#ifdef MDM_INIT_SET
	DCB				dcb;
	COMMTIMEOUTS	tmRW;
	RASDEVCFG		rasDevCfg;
	DWORD			dwTmp;
#endif

	//
	//�������豸
	//
	//���豸
	RETAILMSG(1,(TEXT("\r\n UniMdm_OpenAndCfg: open[%s]\r\n"), pLineDev->rasOpen.szOpenName));
	hPort = ver_CreateFile( pLineDev->rasOpen.szOpenName, GENERIC_READ|GENERIC_WRITE|GENERIC_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( !hPort || hPort==INVALID_HANDLE_VALUE )
	{
		RETAILMSG(1,(TEXT("\r\n UniMdm_OpenAndCfg: open [%s] failed!\r\n"), pLineDev->rasOpen.szOpenName));
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_OPEN );
		return FALSE;
	}
	ver_SetupComm( hPort, 8192, 0 );
	RETAILMSG(1,(TEXT("\r\n UniMdm_OpenAndCfg: open [%s] ok!\r\n"), pLineDev->rasOpen.szOpenName));

#ifdef MDM_INIT_SET
	// ��ȡ ָ����Ŀ������ �����豸������
	dwTmp = sizeof(RASDEVCFG);
	if( !RAPI_GetEntryDevConfig( NULL, pLineDev->szEntryName, pLineDev->szDrvReg, &rasDevCfg, &dwTmp ) )
	{
		//return FALSE;
	}
	// ���� �豸
	if( !ver_GetCommState(hPort, &dcb) )
	{
		ver_CloseHandle( hPort );
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_CFG );
		return FALSE;
	}
	pLineDev->dwBaud = CBR_115200;//CBR_115200 rasDevCfg.dcb.BaudRate;
	dcb.BaudRate = CBR_115200;//CBR_115200 rasDevCfg.dcb.BaudRate;
	//pLineDev->dwBaud = CBR_115200;//CBR_115200 rasDevCfg.dcb.BaudRate;
	//dcb.BaudRate = CBR_115200;//CBR_115200 rasDevCfg.dcb.BaudRate;
	dcb.fParity  = 0;//rasDevCfg.dcb.fParity ;
	dcb.Parity   = NOPARITY;//rasDevCfg.dcb.Parity  ;
	dcb.StopBits = ONESTOPBIT;//rasDevCfg.dcb.StopBits;
	dcb.ByteSize = 8;//rasDevCfg.dcb.ByteSize;
	dcb.fDsrSensitivity = 0;//rasDevCfg.dcb.fDsrSensitivity;

	dcb.fTXContinueOnXoff = 0;//rasDevCfg.dcb.fDsrSensitivity;
	dcb.fRtsControl  = RTS_CONTROL_HANDSHAKE;//rasDevCfg.dcb.fRtsControl ;
	dcb.fDtrControl  = DTR_CONTROL_ENABLE;//rasDevCfg.dcb.fDtrControl ;
	//dcb.fDtrControl  = DTR_CONTROL_HANDSHAKE;//rasDevCfg.dcb.fDtrControl ;
	dcb.fOutxCtsFlow = 1;//rasDevCfg.dcb.fOutxCtsFlow;
	dcb.fOutxDsrFlow = 0;//rasDevCfg.dcb.fOutxDsrFlow;
	//dcb.fOutxDsrFlow = 1;//rasDevCfg.dcb.fOutxDsrFlow;

	dcb.fOutX        = 0;//rasDevCfg.dcb.fOutX       ;
	dcb.fInX         = 0;//rasDevCfg.dcb.fInX        ;

	if( !ver_SetCommState(hPort, &dcb) )
	{
		//DWORD dw=GetLastError();
		ver_CloseHandle( hPort );
		RASEVT_SEND( RASCS_OpenPort, RASERR_TSP_CFG );
		return FALSE;
	}
	
	//
	tmRW.ReadIntervalTimeout = 5;
	tmRW.ReadTotalTimeoutMultiplier = 3;
	tmRW.ReadTotalTimeoutConstant = 100;
	tmRW.WriteTotalTimeoutMultiplier = 5;//attention here, prevent timeout when wirtefile
	tmRW.WriteTotalTimeoutConstant = 1000;
	ver_SetCommTimeouts( hPort, &tmRW );

	ver_SetCommMask( hPort, 0 );
	Sleep( 800 ); //�ȴ��㹻��ʱ��

	ver_PurgeComm( hPort, PURGE_RXCLEAR | PURGE_TXCLEAR );

#endif
	pLineDev->hPort = hPort;

	return TRUE;
}

#ifdef MDM_SET_BAUD
BOOL	Mdm_SetBaud( LINE_DEV* pLineDev, DWORD dwBaud )
{
	DCB				dcb;

	//�����豸
	if( !ver_GetCommState(pLineDev->hPort, &dcb) )
	{
		return FALSE;
	}
	pLineDev->dwBaud = dwBaud;
	dcb.BaudRate = dwBaud;
	//Set
	if( !ver_SetCommState(pLineDev->hPort, &dcb) )
	{
		return FALSE;
	}
	return TRUE;
}
#endif

// ********************************************************************
// ������
// ������
//	IN pLineDev---ָ����TSP�豸
// ����ֵ��
//	�ɹ�������TRUE
// �����������������š������в��ų�ʼ��
// ����: 
// ********************************************************************
BOOL	Mdm_StartMonitor( LINE_DEV* pLineDev )
{
	HANDLE	hThrdDetect;
	DWORD	dwThrdID;

	// ��������---Modem����¼��Ͷ����ݵ��߳�
	if( lineDev_IsDown(pLineDev) )
	{
		return FALSE;
	}
	if( !pLineDev->hEvtCmd )
	{
		pLineDev->hEvtCmd = CreateEvent( NULL, FALSE, FALSE, NULL );
		if( !pLineDev->hEvtCmd )
		{
			return FALSE;
		}
	}
	if( lineDev_IsDown(pLineDev) )
	{
		return FALSE;
	}
	hThrdDetect = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)Mdm_ThrdMonitor, (LPVOID)pLineDev, 0, &dwThrdID );
	if( !hThrdDetect )
	{
		return FALSE;
	}
	CloseHandle( hThrdDetect );
	Sleep( 10 );
	if( lineDev_IsDown(pLineDev) )
	{
		return FALSE;
	}

	return TRUE;
}


DWORD	UniMdm_SendCmd( LINE_DEV* pLineDev, IN OUT MDM_CMD_EXE* pInfoCmd, DWORD dwExeFlag, DWORD dwWait )
{
	DWORD			dwRet;
	int				nTryCmd;
	int				i;

	//ͳ����Ϣ
	if( dwExeFlag & MEXEF_TRYONE )
	{
		nTryCmd = 1;
	}
	else if( dwExeFlag & MEXEF_TRYNUM )
	{
		nTryCmd = pInfoCmd->nTryNum;//ִ������Ĵ���
	}
	else
	{
		nTryCmd = 3;
	}

	//ִ������
	pInfoCmd->pszFound = NULL;
	dwRet = MR_FAILURE;
	i = 0;
	while( i<nTryCmd )
	{
		//��������
		//if( !(dwExeFlag & MEXEF_NOTPURGE) )
		//{
		//	ver_PurgeComm( pLineDev->hPort, PURGE_RXCLEAR );
		//}
		if( !ver_WriteFile(pLineDev->hPort, pInfoCmd->pszCmd, pInfoCmd->dwLenCmd, &dwRet, NULL) )
		{
			dwRet = MR_FAILURE;
			continue;
		}
		//GsmDbg_WriteAt( pInfoCmd->pszCmd, pInfoCmd->dwLenCmd );
		RETAILMSG(1,(TEXT("\r\nSendCmd: [%s]\r\n"), pInfoCmd->pszCmd));
		//�����飬״̬���
		if( lineDev_IsDown(pLineDev) )
		{
			dwRet = MR_DOWN;
			break;
		}

		//�ȴ�����
		if( Mdm_WaitResp( pLineDev, pInfoCmd, dwExeFlag, dwWait ) )
		{
			dwRet = MR_DOWN;
			break;
		}

		//
		if( dwExeFlag & MEXEF_ONLYWRITE )
		{
			dwRet = MR_ONLYWRITE;
			break;
		}

		//��ȡ���ݣ���������
		if( Mdm_FindResp( pLineDev, pInfoCmd, dwExeFlag, &dwRet ) )
		{
			pLineDev->dwResWrite = 0;
			pLineDev->szBufRes[0] = 0;
			break;
		}
		pLineDev->dwResWrite = 0;
		pLineDev->szBufRes[0] = 0;

		//
		i ++;
	}

	return dwRet;
}

BOOL	Mdm_FindResp( LINE_DEV* pLineDev, IN OUT MDM_CMD_EXE* pInfoCmd, DWORD dwExeFlag, OUT DWORD* pdwRet )
{
	LPSTR			pBufRes;
	DWORD			dwResWrite;
	DWORD			dwResLen;
	LPSTR			pszTemp;
	BOOL			fNoCRLF;
	DWORD			dwTickStart;
	DWORD			dwTickWait;

	//׼��
	dwResWrite = pLineDev->dwResWrite;
	pBufRes = pLineDev->szBufRes + dwResWrite;
	if( dwExeFlag & MEXEF_READWAIT )
	{
		dwTickWait = pInfoCmd->dwReadWait;
	}
	else
	{
		dwTickWait = 2000;
	}
	pInfoCmd->pszBufRes = pBufRes;
	//����1�λ���ȥ��ȡ����
	dwTickStart = GetTickCount();
	while( (GetTickCount()-dwTickStart)<=dwTickWait )
	{
		//�����飬״̬���
		if( lineDev_IsDown(pLineDev) )
		{
			*pdwRet = MR_DOWN;
			return TRUE;
		}
		//��ȡ����
		if( !Mdm_ReadResp( pLineDev, pLineDev->hPort ) )
		{
			Sleep( 10 );
		}
		if( !pLineDev->dwResWrite )
		{
			Sleep( 100 );
			continue;
		}
		dwResLen = pLineDev->dwResWrite - dwResWrite;

		//���� "ERROR"---����
		if( dwExeFlag & MEXEF_FINDERR )
		{
			if( pszTemp=strstr(pBufRes, MDM_ERR_STR) )
			{
				pInfoCmd->pszFound = pszTemp;
				*pdwRet = MR_GSMERR;
				return TRUE;
			}
		}
		//���� "OK"---ȷ������
		if( dwExeFlag & MEXEF_MUSTOK )
		{
			if( !(pszTemp=strstr(pBufRes, MDM_OK_STR)) )
			{
				continue;
			}
			pInfoCmd->pszFound = pszTemp;
		}
		if( dwExeFlag & MEXEF_FINDATD )
		{
			//RETAILMSG(1,(TEXT("\r\nFINDATD: [%s]"), pBufRes));
			//���� "CONNECT"
			if( pszTemp=strstr(pBufRes, MDM_CONNECT_STR) )
			{
				pInfoCmd->pszFound = pszTemp;
				*pdwRet = MR_CONNECT;
				return TRUE;
			}
			//���� "NO CARRIER"
			if( pszTemp=strstr(pBufRes, MDM_NOCAR_STR) )
			{
				pInfoCmd->pszFound = pszTemp;
				*pdwRet = MR_NOCAR;
				return TRUE;
			}
			//���� "NO ANSWER"
			if( pszTemp=strstr(pBufRes, MDM_NOANS_STR) )
			{
				pInfoCmd->pszFound = pszTemp;
				*pdwRet = MR_NOANS;
				return TRUE;
			}
			//���� "NO DIALTONE"
			if( pszTemp=strstr(pBufRes, MDM_NOTONE_STR) )
			{
				pInfoCmd->pszFound = pszTemp;
				*pdwRet = MR_NOTONE;
				return TRUE;
			}
			//���� "BUSY"
			if( pszTemp=strstr(pBufRes, MDM_BUSY_STR) )
			{
				pInfoCmd->pszFound = pszTemp;
				*pdwRet = MR_BUSY;
				return TRUE;
			}
		}
		//���� ָ��ǰ׺---��Ϣ
		if( dwExeFlag & MEXEF_FINDPREFIX )
		{
			if( (dwExeFlag & (MEXEF_AFTEROK|MEXEF_MUSTOK))==(MEXEF_AFTEROK|MEXEF_MUSTOK) )
			{
				pszTemp = pInfoCmd->pszFound+sizeof(MDM_OK_STR)-1;
			}
			else
			{
				pszTemp = pBufRes;
			}
			if( pszTemp=Mdm_StrAndCRLF(pszTemp, pInfoCmd->pszPrefix, pInfoCmd->dwLenPrefix, &fNoCRLF) )
			{
				if( fNoCRLF )
				{
					Sleep( 10 );
					continue;
				}
				//
				pInfoCmd->pszFound = pszTemp + pInfoCmd->dwLenPrefix;
				dwTickWait = (DWORD)(pszTemp - pLineDev->szBufRes);
				pInfoCmd->dwLenFound = pLineDev->dwResWrite - dwTickWait - pInfoCmd->dwLenPrefix;
				*pdwRet = MR_PREFIX;
				return TRUE;
			}
		}
		//���� "OK"---������ڣ����˳�
		if( !(dwExeFlag & MEXEF_NOTFINDOK) )
		{
			if( (dwExeFlag & (MEXEF_MUSTOK)) || (pszTemp=strstrX(pBufRes, dwResLen, MDM_OK_STR, sizeof(MDM_OK_STR)-1)) )
			{
				if( !(dwExeFlag & (MEXEF_MUSTOK)) )
				{
					pInfoCmd->pszFound = pszTemp;
				}
				*pdwRet = MR_OK;
				return TRUE;
			}
		}
		if( dwExeFlag & MEXEF_MUSTCRLF )
		{
			if( !(pszTemp=strstrX(pBufRes, dwResLen, MDM_CRLF_STR, sizeof(MDM_CRLF_STR)-1)) )
			{
				continue;
			}
			pInfoCmd->pszFound = pszTemp;
		}
		
		//������� UNFOUND �˳�
		if( dwExeFlag & MEXEF_MAYUNFOUND )
		{
			*pdwRet = MR_UNFOUND;
			return TRUE;
		}

		//
		Sleep( 15 );
	}

	*pdwRet = MR_UNFOUND;
	return FALSE;
}


BOOL	Mdm_ReadResp( LINE_DEV* pLineDev, HANDLE hPort )
{
	DWORD			dwLen;
	LPSTR			pBufRes;
	DWORD			dwLenResMax;
	DWORD			dwResRead;

	//����ʣ�໺��
	pBufRes = pLineDev->szBufRes + pLineDev->dwResWrite;
	dwLenResMax = LEN_BUFRES - pLineDev->dwResWrite;
	//��ȡ���ݵĳ���
	if( !(dwLen=Mdm_GetCbInQue( hPort )) )
	{
		return FALSE;
	}
	dwLen += 10; //���һ���������ν
	if( dwLen > dwLenResMax )
	{
		dwLen = dwLenResMax;//һ�㲻����ָ������ע�⣡
	}
	//��ȡ����
	dwResRead = 0;
	if( !ver_ReadFile( hPort, pBufRes, dwLen, &dwResRead, NULL ) )
	{
		return FALSE;
	}
	//GsmDbg_WriteAt( pBufRes, dwResRead );
	pLineDev->dwResWrite += dwResRead;
	pBufRes[dwResRead] = 0;
	RETAILMSG(1,(TEXT("\r\nSendCmd_rx: [%s]"), pBufRes));

	return TRUE;
}


LPSTR	Mdm_StrAndCRLF( LPSTR pszBufRes, LPSTR pszBufFind, DWORD dwLenFind, BOOL* pfNoCRLF )
{
	if( pszBufRes=strstr(pszBufRes, pszBufFind) )
	{
		if( strstr(pszBufRes+dwLenFind, MDM_CRLF_STR) )
		{
			*pfNoCRLF = FALSE;
			return pszBufRes;
		}
		else
		{
			*pfNoCRLF = FALSE;
		}
		return pszBufRes;
	}

	return NULL;
}


BOOL	Mdm_WaitResp( LINE_DEV* pLineDev, IN OUT MDM_CMD_EXE* pInfoCmd, DWORD dwExeFlag, DWORD dwWait )
{
	DWORD			dwLen;
	DWORD			dwTick;
	int				i;

	//�ȴ����
	dwTick = GetTickCount( );
	for( i=0; i<100; i++ )
	{
		//�����飬״̬���
		if( lineDev_IsDown(pLineDev) )
		{
			return TRUE;
		}
		//�������Ҫ�ȴ�����ȡ���ݵĳ���
		if( Mdm_GetCbInQue( pLineDev->hPort ) )
		{
			break;
		}

		//�����ȴ�
		WaitForSingleObject( pLineDev->hEvtCmd, dwWait );

		//�����飬״̬���
		if( lineDev_IsDown(pLineDev) )
		{
			return TRUE;
		}
		//�������Ҫ�ȴ�����ȡ���ݵĳ���
		if( Mdm_GetCbInQue( pLineDev->hPort ) )
		{
			break;
		}

		//����ʱ�䵽��
		dwLen = GetTickCount( )-dwTick;
		if( dwLen>dwWait )
		{
			//���ʱ�䵽��
			break;
		}
		dwWait -= dwLen;
	}
	//��Ҫ�ȴ��㹻��ʱ�䣬���ܵõ�ȫ������Ϣ
	Sleep( 120 );

	return FALSE;
}


// ********************************************************************
// ������
// ������
//	IN lpParam---TSP�豸���
// ����ֵ��
//	����0
// ����������TSP�豸�Ĺ����̣߳����modem�����ݺ�״̬
// ����: 
// ********************************************************************
DWORD	WINAPI	Mdm_ThrdMonitor( LPVOID lpParam )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)lpParam;
	HANDLE		hPort;
	DWORD		dwTmp;

	Sleep( 20 );
	hPort = pLineDev->hPort;
	//RETAILMSG(1,(TEXT("\r\n Mdm_ThrdMonitor: port enter [0x%x] [0x%x]\r\n"), pLineDev, pLineDev->hPort));

	ver_PurgeComm( hPort, PURGE_RXCLEAR | PURGE_TXCLEAR );
#ifndef	VC386
	ver_SetCommMask( hPort, EV_RXCHAR|EV_RLSD|EV_ERR );
#endif	//VC386
	//
	while( (pLineDev->dwDevState!=MDM_DOWN) && pLineDev->hPort && (pLineDev->hPort!=INVALID_HANDLE_VALUE) )
	{
#ifndef	VC386
		DWORD		dwEvent;

		//RETAILMSG(1,(TEXT("\r\n Mdm_ThrdMonitor: port222 look\r\n")));
		ver_WaitCommEvent( pLineDev->hPort, &dwEvent, NULL );
		if( pLineDev->dwDevState==MDM_DOWN )
		{
			break;
		}
		//RETAILMSG(1,(TEXT("\r\n Mdm_ThrdMonitor: port 333 [0x%x]\r\n"), dwEvent));
		if( !dwEvent )
		{
			continue ;
		}
#ifdef MDM_LOOK_CTS
		if( dwEvent & EV_RLSD )
#endif

#endif

#ifdef MDM_LOOK_CTS
		{
			DWORD	dwMdmStatus;
			if( !ver_GetCommModemStatus(hPort, &dwMdmStatus) || !(dwMdmStatus & (MS_DSR_ON|MS_CTS_ON)) )
			{
				//�����⵽ ���ߣ�����ΰ�
				RETAILMSG(1,(TEXT("\r\n ThrdCmd: Modem has No Dsr/Cts signal\r\n")));
				RASEVT_SEND( RASCS_Disconnected, RASERR_NOSIGNAL );
				break;
			}
		}
#endif

		//MODEM ������Ϣ
#ifndef	VC386
		if( !(dwEvent & EV_RXCHAR) )
		{
			continue;
		}
#endif
		while( (pLineDev->dwDevState!=MDM_DOWN) && (dwTmp=Mdm_GetCbInQue(hPort)) )
		{
			//���ݽ׶�
			if( pLineDev->dwDevState==MDM_DATA )
			{
				//RETAILMSG(1,(TEXT("\r\n ThrdCmd: MDM_DATA AAA bytes=[%d]\r\n"),dwTmp));
				if( dwTmp > LEN_BUFRES )
				{
					dwTmp = LEN_BUFRES;
				}
				if( ver_ReadFile( hPort, pLineDev->szBufRes, dwTmp, &dwTmp, NULL ) && dwTmp )
				{
					//RETAILMSG(1,(TEXT("\r\n ThrdCmd: MDM_DATA bytes=[%d]\r\n"),dwTmp));
					RasIf_UpData( pLineDev->hRasDev, pLineDev->szBufRes, dwTmp );
				}
			}
			//����׶�
			else
			{
				//RETAILMSG(1,(TEXT("\r\nThrdCmd: responde bytes=[%d]\r\n"),dwTmp));
				//Sleep( 10 );
				SetEvent( pLineDev->hEvtCmd );
				break;
			}
			Sleep( 1 );
		}
		//
#ifdef	VC386
		Sleep( 15 );
#endif	//VC386
	}

	RETAILMSG(1,(TEXT("\r\n ---- ***** -----\r\n")));
	RETAILMSG(1,(TEXT("\r\n Mdm_ThrdMonitor: port555_EXIT \r\n")));
	RETAILMSG(1,(TEXT("\r\n ---- ***** -----\r\n")));
	return 0;
}

DWORD	Mdm_GetCbInQue( HANDLE hPort )
{
	DWORD			dwError;
	COMSTAT			ComStatus;

	if( ver_ClearCommError(hPort, &dwError, &ComStatus) && ComStatus.cbInQue )
	{
		return ComStatus.cbInQue;
	}
	return 0;
}

// ********************************************************************
//������
//������
//	IN pStr1 - Դ�ַ���
//	IN pStr2 - �����ҵ��ַ���
//����ֵ��
//	�ɹ����ز��ҵ��ַ�����λ��
//������������pStr1�в����ַ���pStr2
//����: 
// ********************************************************************
LPSTR	strstrX( LPCSTR pStr1, DWORD dwLenStr1, LPCSTR pStr2, DWORD dwLenStr2 )
{
	LPSTR	s1;
	LPSTR	s2;
	LPSTR	cp;
	LPSTR	pStr2End;
	
	//
	if( !*pStr2 || !dwLenStr2 )
	{
		return (LPSTR)pStr1;
	}
	//if( !*pStr1 )
	//{
	//	return NULL;
	//}
	if( dwLenStr1==-1 )
	{
		dwLenStr1 = strlen( pStr1 );
	}
	if( dwLenStr2==-1 )
	{
		dwLenStr1 = strlen( pStr2 );
	}
	//
	pStr2End = (LPSTR)pStr2 + dwLenStr2 - 1;
	cp = (LPSTR)pStr1 + dwLenStr1 - 1;
	while( cp>=pStr1 )
	{
		s1 = cp;
		s2 = pStr2End;
		while( (s1>=pStr1) && (s2>=pStr2) )
		{
			if( *s1!=*s2 )
			{
				break;
			}
			s1 --;
			s2 --;
		}
		//
		if( s2<pStr2 )
		{
			return (s1+1);
		}
		//
		cp --;
	}

	return NULL;
}
