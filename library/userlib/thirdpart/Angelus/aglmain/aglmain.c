/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵������ʼ���Ǳ�ؼ�
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>

extern int RegisterAngelusButtonClass( HANDLE h );
VOID InitThirdpartControls( VOID )
{
	HANDLE h = GetModuleHandle( NULL );
	RegisterAngelusButtonClass( h );
}

VOID ActiveDesktop( VOID )
{
	HWND hwndDesktop = FindWindow( "_DESKTOP_CLASS_", NULL );

	if( hwndDesktop )
	{
		PostMessage( hwndDesktop, WM_ACTIVATEAPP, 0, 0 );
	}
}

