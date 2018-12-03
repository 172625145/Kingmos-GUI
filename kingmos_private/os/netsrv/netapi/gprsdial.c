/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����GPRS Config API
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-08-24
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/

//root include
#include <ewindows.h>
//"\inc_app"
#include <ras.h>
#include <gprscfg.h>


/***************  ȫ���� ���壬 ���� *****************/
/******************************************************/


DWORD	GprsDial( DWORD dwNetworkType, HWND hWndRas, OUT HANDLE* phRasConn )
{
	DWORD		dwErr;
	DIALPARAM	stDialParam;

	//
	stDialParam.dwSize = sizeof(DIALPARAM);
	stDialParam.szEntryName[0] = 0;
	strcpy( stDialParam.szPhoneNumber, "*99#" );
	stDialParam.szUserName[0] = 0;
	stDialParam.szPassword[0] = 0;
	stDialParam.szDomain[0] = 0;

	//����ӽ��̻������--begin

	//
	GprsCfg_SetCurNetType( dwNetworkType );
	//
	dwErr = RasDial( &stDialParam, DIALNOTIFY_WND, (LPVOID)hWndRas, phRasConn );

	//����ӽ��̻������--end

	return dwErr;
}


BOOL	GprsCheckDial( OUT DWORD* pdwNetworkType )
{
	//1 �Ƿ���GPRS��������
	if( !RasIsDevExist( TRUE, RASDT_SUB_GPRS, RASDT_CHKSUB ) )
	{
		return FALSE;
	}
	//2 �õ�GPRS���ŵ���������
	GprsCfg_GetCurNetType( pdwNetworkType );
	return TRUE;
}






