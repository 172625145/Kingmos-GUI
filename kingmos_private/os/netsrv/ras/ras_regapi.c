/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����RAS API ע��
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-05-09
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
#include <eapisrv.h>
#include <apiexe.h>
//"\inc_app"
#include <ras.h>
//"\inc_drv"
#include "handle_check.h"
#include "netif_ras.h"
//"\inc_local"
#include "ras_call.h"


/***************  ȫ���� ���壬 ���� *****************/

extern	DWORD	WINAPI	RAPI_CallBack( HANDLE handle, DWORD dwEventCode, DWORD dwParam, LPVOID lpParam );
static	const	SERVER_CTRL_INFO	sci = { sizeof( SERVER_CTRL_INFO ), SCI_NOT_HANDLE_THREAD_EXIT, RAPI_CallBack };
static	const	PFNVOID	g_lpRASAPI[] =
{
	  (PFNVOID)&sci

// RAS����ģʽ
	, (PFNVOID)RAPI_EnumDialMethod
	, (PFNVOID)RAPI_GetDialMethod
	, (PFNVOID)RAPI_GenNum

// RAS������Ŀ
	, (PFNVOID)RAPI_EnumEntries
	, (PFNVOID)RAPI_GetEntryDef
	, (PFNVOID)RAPI_SetEntryDef
	, (PFNVOID)RAPI_DeleteEntry
	, (PFNVOID)RAPI_RenameEntry
	, (PFNVOID)RAPI_IsValidEntryName

	, (PFNVOID)RAPI_GetEntryProperties
	, (PFNVOID)RAPI_SetEntryProperties

	, (PFNVOID)RAPI_GetEntryDialParams
	, (PFNVOID)RAPI_SetEntryDialParams

	, (PFNVOID)RAPI_GetEntryDevConfig
	, (PFNVOID)RAPI_SetEntryDevConfig

// RAS�����豸
	, (PFNVOID)RAPI_EnumDevices
	, (PFNVOID)RAPI_LookDriver
	, (PFNVOID)RAPI_CheckDev

// RAS��������
	, (PFNVOID)RAPI_Dial
	, (PFNVOID)RAPI_HangUp
	, (PFNVOID)RAPI_EnumConnections

	, (PFNVOID)RAPI_GetConnectInfo
	, (PFNVOID)RAPI_GetConnectStatus

	, (PFNVOID)RAPI_EnableStat
	, (PFNVOID)RAPI_GetLinkStatistics

// RAS���Ÿ�������
	, (PFNVOID)RAPI_GetStateStr
	, (PFNVOID)RAPI_GetErrStr

// RAS����modem
	, (PFNVOID)RAPI_RegisterModem
	, (PFNVOID)RAPI_DeregisterModem
	, (PFNVOID)RAPI_RegisterPPPoEd
	, (PFNVOID)RAPI_DeregisterPPPoEd

};


static const DWORD g_lpRASArgs[] = 
{
	0

// RAS����ģʽ
	, ARG2_MAKE( PTR, PTR )					//RAPI_EnumDialMethod
	, ARG3_MAKE( PTR, DWORD, PTR )			//RAPI_GetDialMethod
	, ARG4_MAKE( PTR, PTR, PTR, DWORD )		//RAPI_GenNum

// RAS������Ŀ
	, ARG3_MAKE( PTR, PTR, PTR )			//RAPI_EnumEntries
	, ARG3_MAKE( PTR, PTR, DWORD )			//RAPI_GetEntryDef
	, ARG2_MAKE( PTR, PTR )					//RAPI_SetEntryDef
	, ARG2_MAKE( PTR, PTR )					//RAPI_DeleteEntry
	, ARG3_MAKE( PTR, PTR, DWORD )			//RAPI_RenameEntry
	, ARG2_MAKE( PTR, PTR )					//RAPI_IsValidEntryName

	, ARG4_MAKE( PTR, PTR, PTR, PTR )		//RAPI_GetEntryProperties
	, ARG4_MAKE( PTR, PTR, PTR, DWORD )		//RAPI_SetEntryProperties

	, ARG3_MAKE( PTR, PTR, PTR )			//RAPI_GetEntryDialParams
	, ARG3_MAKE( PTR, PTR, DWORD )			//RAPI_SetEntryDialParams

	, ARG5_MAKE( PTR, PTR, PTR, PTR, PTR )	//RAPI_GetEntryDevConfig
	, ARG5_MAKE( PTR, PTR, PTR, PTR, DWORD )//RAPI_SetEntryDevConfig

// RAS�����豸
	, ARG2_MAKE( PTR, PTR )					//RAPI_EnumDevices
	, ARG5_MAKE( PTR, PTR, PTR, PTR, PTR )	//RAPI_LookDriver
	, ARG2_MAKE( DWORD, DWORD )				//RAPI_CheckDev

// RAS��������
	, ARG5_MAKE( PTR, PTR, DWORD, PTR, PTR )//RAPI_Dial
	, ARG1_MAKE( DWORD )				//RAPI_HangUp
	, ARG2_MAKE( PTR, PTR )				//RAPI_EnumConnections

	, ARG2_MAKE( DWORD, PTR )			//RAPI_GetConnectInfo
	, ARG2_MAKE( DWORD, PTR )			//RAPI_GetConnectStatus

	, ARG2_MAKE( DWORD, DWORD )			//RAPI_EnableStat
	, ARG2_MAKE( DWORD, PTR )			//RAPI_GetLinkStatistics

// RAS���Ÿ�������
	, ARG3_MAKE( DWORD, PTR, DWORD )	//RAPI_GetStateStr
	, ARG3_MAKE( DWORD, PTR, DWORD )	//RAPI_GetErrStr

// RAS����modem
	, ARG1_MAKE( PTR )					//RAPI_RegisterModem
	, ARG1_MAKE( PTR )					//RAPI_DeregisterModem
	, ARG2_MAKE( PTR, DWORD )			//RAPI_RegisterPPPoEd
	, ARG1_MAKE( PTR )					//RAPI_DeregisterPPPoEd

};

/******************************************************/


void	Ras_RegAPI()
{
	API_RegisterEx( API_RAS, (PFNVOID*)g_lpRASAPI, g_lpRASArgs, ARR_COUNT(g_lpRASAPI) );
	API_SetReady( API_RAS );
}

