#include <ewindows.h>
/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/

extern int CALLBACK Shell_WinMain( HINSTANCE hInstance,
		            HINSTANCE hPrevInstance,
		            LPTSTR    lpCmdLine,
		            int       nCmdShow );

void InitGWMEKernel( void )
{
	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	printf( "call InitApplicationMgr.\r\n" );
	InitApplicationMgr();


	printf( "call RegisterApplication(shell).\r\n" );

	RegisterApplication( "shell",  Shell_WinMain, NULL ); // ע�ᵽ���������
	printf( "call LoadApplication(shell).\r\n" );

	LoadApplication( "shell", NULL );	
	//InitServerApplication();
	//InstallApplication();
}

