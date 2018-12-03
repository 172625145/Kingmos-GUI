/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����������Ĺ��� ���ļ�
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-05-09
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
#include <eapisrv.h>


/***************  ȫ���� ���壬 ���� *****************/
extern	BOOL	SMgr_Init( );
extern	void	SMgr_Deinit( );
extern	BOOL	SMgr_RegAPI( );

static	BOOL	API_WaitReady( UINT uiAPIId, DWORD dwWaitMS, LPCSTR pszInfo );

/******************************************************/


// ********************************************************************
//����������Services ���������
// ********************************************************************
#ifdef	INLINE_PROGRAM
LRESULT CALLBACK WinMain_NetSrv(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#else
LRESULT CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#endif
{
	HANDLE	hEvtNetSrv;

	if( !API_IsReady( API_TCPIP ) )
	{
		//ע��API
		if( !SMgr_RegAPI( ) )
		{
			return 0;
		}
		//�ȴ��ļ�ϵͳ
		API_WaitReady( API_FILESYS, -1, "WinMain_NetSrv" );

		//��ʼ��
		if( !SMgr_Init( ) )
		{
			return 0;
		}

		//��ʼ����
		hEvtNetSrv = CreateEvent( NULL, FALSE, FALSE, "Event_Net_Services" );
		API_SetReady( API_TCPIP );

		//��ʼ����κ��¼�
		while( 1 )
		{
			WaitForSingleObject( hEvtNetSrv, INFINITE );
			//break;
		}
		CloseHandle( hEvtNetSrv );

		//
		SMgr_Deinit( );
	}

	return 0;
}


BOOL	API_WaitReady( UINT uiAPIId, DWORD dwWaitSec, LPCSTR pszInfo )
{
	DWORD		dwTick;

	// �ȴ�ָ��ID�ŵ�API�Ƿ��Ѿ����
	dwTick = GetTickCount();
	while( 1 )
	{
		//
		if( API_IsReady( uiAPIId ) )
		{
			return TRUE;
		}
		RETAILMSG( 1, ( "(%s): wait api [%d] ready!\r\n", pszInfo, uiAPIId ) );
		//
		if( dwWaitSec!=-1 )
		{
			if( ((GetTickCount()-dwTick)/1000)>=dwWaitSec )
			{
				return FALSE;
			}
		}
		Sleep(100);
	}
	return FALSE;
}
