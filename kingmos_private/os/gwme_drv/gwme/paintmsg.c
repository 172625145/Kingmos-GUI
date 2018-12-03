/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：管理WM_PAINT消息
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <epalloc.h>
#include <eassert.h>
#include <eapisrv.h>
#include <winsrv.h>
#include <gdisrv.h>

static HRGN hRepaintRgn;
static CRITICAL_SECTION csPaintRgn;

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL _InitialPaintRgn( void )
{
	InitializeCriticalSection( &csPaintRgn );
	csPaintRgn.lpcsName = "CS-PRN";
	return TRUE;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

void _DeInitialPaintRgn( void )
{
    DeleteCriticalSection( &csPaintRgn );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL CALLBACK __LockRepaintRgn( HRGN * lphrgn )
{
    RECT rect;

    EnterCriticalSection( &csPaintRgn );
    if( lphrgn )
        *lphrgn = hRepaintRgn;
    if( WinRgn_GetBox( hRepaintRgn, &rect ) == NULLREGION )
        return FALSE;
    else
        return TRUE;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL CALLBACK __UnlockRepaintRgn( void )
{
	LeaveCriticalSection( &csPaintRgn );
	return TRUE;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int CALLBACK __PutPaintRgn( HRGN hrgn )
{
    int retv;
	EnterCriticalSection( &csPaintRgn );

    if( !hRepaintRgn )
        hRepaintRgn = WinRgn_CreateRect( 0, 0, 0 ,0 );
    retv = WinRgn_Combine( hRepaintRgn,
                       hRepaintRgn,
                       hrgn, RGN_OR );
	LeaveCriticalSection( &csPaintRgn );
	return retv;
}
