/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：初始化非标控件
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：
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

