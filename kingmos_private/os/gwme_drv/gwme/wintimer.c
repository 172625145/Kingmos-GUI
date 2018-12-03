/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：窗口时钟
版本号：2.0.0
开发时期：1999
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <epwin.h>

extern UINT FASTCALL _SetTimer( HANDLE hOwnerProcess, HWND hWnd, UINT msg, UINT id, UINT uElapse, TIMERPROC lpTimerProc );
extern UINT FASTCALL _KillTimer( HWND hWnd, UINT msg, UINT id );
// **************************************************
// 声明：UINT WINAPI Win_SetTimer(	HWND hWnd, 
//									UINT id, 
//									UINT uElapse, 
//									TIMERPROC lpTimerProc )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN id - 时钟的ID号
//	IN uElapse - 到期时间（毫秒为单位）
//  IN lpTimerProc - 回调函数
// 返回值：
//	成功：返回id
//	否则：返回0
// 功能描述：
//	安装定时器
// 引用: 
//	系统API
// ************************************************
 
UINT WINAPI Win_SetTimer( HWND hWnd, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
{
	HANDLE hProcess = GetCallerProcess();

	if( hProcess == NULL )
		hProcess = hgwmeProcess;
    if( hWnd )
	    return _SetTimer( hProcess, hWnd, WM_TIMER, id, uElapse, lpTimerProc );
	else
		return _SetTimer( hProcess, NULL, 0, id, uElapse, lpTimerProc );
}

// **************************************************
// 声明：BOOL WINAPI Win_KillTimer( HWND hWnd, UINT id )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN id - 时钟的ID号
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	注销掉一个时钟ID
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Win_KillTimer( HWND hWnd, UINT id )
{
    if( hWnd )
	    return _KillTimer( hWnd, WM_TIMER, id );
	else
		return _KillTimer( hWnd, 0, id );

}

// **************************************************
// 声明：UINT WINAPI Win_SetSysTimer( HWND hWnd, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN id - 时钟的ID号
//	IN uElapse - 到期时间（毫秒为单位）
//  IN lpTimerProc - 回调函数
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	安装系统定时器
// 引用: 
//	系统API
// ************************************************

UINT WINAPI Win_SetSysTimer( HWND hWnd, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
{
	HANDLE hProcess = GetCallerProcess();

	if( hProcess == NULL )
		hProcess = hgwmeProcess;

    if( hWnd )
	    return _SetTimer( hProcess, hWnd, WM_SYSTIMER, id, uElapse, lpTimerProc );
	else
		return _SetTimer( hProcess, NULL, 0, id, uElapse, lpTimerProc );
}

// **************************************************
// 声明：BOOL WINAPI Win_KillSysTimer( HWND hWnd, UINT id )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN id - 时钟的ID号
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	注销掉一个时钟ID
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Win_KillSysTimer( HWND hWnd, UINT id )
{
    if( hWnd )
	    return _KillTimer( hWnd, WM_SYSTIMER, id );
	else
		return _KillTimer( hWnd, 0, id );
}

