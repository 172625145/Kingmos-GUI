#include <ewindows.h>
/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
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

	RegisterApplication( "shell",  Shell_WinMain, NULL ); // 注册到程序管理器
	printf( "call LoadApplication(shell).\r\n" );

	LoadApplication( "shell", NULL );	
	//InitServerApplication();
	//InstallApplication();
}

