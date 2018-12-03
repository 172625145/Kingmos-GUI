/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：管理点设备消息
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
    2003-04-14-增加冲突段检查
******************************************************/

#include <eframe.h>
#include "epos.h"
#include <eevent.h>
#include <epwin.h>
#include <gwmeobj.h>
#include <esysset.h>
#ifdef EML_DOS
#include <dos.h>
#endif

void TouchPanelCalibrateAPoint(
    int   xUnCalibrate,     //原始的x坐标
    int   yUnCalibrate,     //原始的y坐标
    int   *pCalX,     //转换后的x坐标
    int   *pCalY      //转换后的y坐标
    );

static int _GetPosEvent( POSRECORD FAR * lpRecord, UINT fRemove );
static int _PosPeek( LPMSG, UINT fRemove );
static int _PosPut( LPMSG );
static int _PosRemove(void);
static int _PosInstall(void);
static int _PosRelease(void);
static int _PosWho(void);
static UINT _PosCount(void);
static BOOL CALLBACK DoPosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo );

_EVENTDRV _posEvent = {
    _PosInstall,
    _PosRelease,
    _PosWho,
    _PosPeek,
    _PosPut,
    _PosRemove,
    _PosCount
};

#undef  MAX_QUEUE
#define MAX_QUEUE 16
static int _queueHead = 0;
static int _queueTail = 0;
static int _queueCount = 0;
static POSRECORD _curPos;
static POSRECORD _lastPos;
static POSRECORD _posQueue[MAX_QUEUE];
static CRITICAL_SECTION csPosQueue;
static LPPOS_CALLBACK lpPosCallBackFun = NULL;//定位设备回调函数

BOOL bTestQueue = 0;

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

static BOOL __PutPointerEvent( LPPOSRECORD lpRecord )
{
	BOOL bRetv = FALSE;
	if( lpRecord->x == _curPos.x &&
		lpRecord->y == _curPos.y &&
		lpRecord->wState == _curPos.wState )
		return FALSE;

    EnterCriticalSection( &csPosQueue );

    _curPos = *lpRecord;
 
    if( _queueCount < MAX_QUEUE )
    {
        _posQueue[_queueTail] = _curPos;
        if( ++_queueTail>= MAX_QUEUE )
            _queueTail = 0;
        _queueCount++;
		bRetv = TRUE;
    }
	else
	{
		RETAILMSG( 1, ( "mouse queue is full, _queueCount=%d.\r\n", _queueCount ) );

		// TEST code
		{
			extern void LockCSWindow( void );
			extern void UnlockCSWindow( void );
			
			DWORD dwForeThreadID;
			bTestQueue = TRUE;
			dwForeThreadID = LockForegroundThreadId(); //LN, 2003-06-03, ADD
			UnlockForegroundThreadId();
			RETAILMSG( 1, ( "UnlockForegroundThreadId.\r\n" ) );
			LockCSWindow();
			UnlockCSWindow();			
			RETAILMSG( 1, ( "UnlockCSWindow.\r\n" ) );
			{
				HRGN hClip;
			__LockRepaintRgn( &hClip );
			__UnlockRepaintRgn();
			RETAILMSG( 1, ( "UnlockRepaintRgn.\r\n" ) );
			}
			
		}
		//

	}

	LeaveCriticalSection( &csPosQueue );

	SetSYSQueueState( ES_MOUSE, TRUE );

    return bRetv;
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

int _GetPosEvent( POSRECORD FAR * lpRecord, UINT fRemove )
{
	EnterCriticalSection( &csPosQueue );


    if( _queueCount == 0 )
    {
        *lpRecord = _curPos;
    }
    else
    {
        *lpRecord = _posQueue[_queueHead];
        if( fRemove == PM_REMOVE )
        {
            if( ++_queueHead >= MAX_QUEUE )
                _queueHead = 0;
            _queueCount--;
        }
    }
	LeaveCriticalSection( &csPosQueue );
    return 0;
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

static UINT _PosCount(void)
{
    return _queueCount;
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

static int _PosPeek( LPMSG lpEventData, UINT fRemove )
{
    POSRECORD _PosRecord;

	// TEST
	if( bTestQueue )
	{
		RETAILMSG( 1, ( "_PosQueueCount = %d, fRemove=%d.\r\n", _queueCount, fRemove ) );
		bTestQueue = FALSE;
	}
	//

    _GetPosEvent( &_PosRecord, fRemove );

    if( _PosRecord.wState == 0 && _lastPos.wState != 0 )
    {
        lpEventData->message = WM_LBUTTONUP;
        lpEventData->wParam = _PosRecord.wState;
		lpEventData->pt.x = _PosRecord.x;
		lpEventData->pt.y = _PosRecord.y;

    }
    else if( _PosRecord.wState != 0 && _lastPos.wState == 0 )
    {
        lpEventData->message = WM_LBUTTONDOWN;
        lpEventData->wParam = _PosRecord.wState;
		lpEventData->pt.x = _PosRecord.x;
		lpEventData->pt.y = _PosRecord.y;
    }
    else if( _PosRecord.x != _lastPos.x ||
            _PosRecord.y != _lastPos.y )
    {
        lpEventData->message = WM_MOUSEMOVE;
        lpEventData->wParam = _PosRecord.wState;
		lpEventData->pt.x = _PosRecord.x;
		lpEventData->pt.y = _PosRecord.y;

    }
    else
    {
        if( _queueCount > 1 )
             _GetPosEvent( &_PosRecord, PM_REMOVE );
        return 0;
    }
    if( fRemove == PM_REMOVE )
        _lastPos = _PosRecord;
    return 1;
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

static int _PosInstall()
{
	InitializeCriticalSection( &csPosQueue );

    return 1;
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

static int _PosRelease()
{
	DeleteCriticalSection( &csPosQueue );
    return 1;
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

static int _PosWho()
{
    return ID_POINTER;
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

static int _PosPut( LPMSG lp )
{
    return 0;
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

static int _PosRemove()
{
    return 0;
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

static BOOL CALLBACK DoPosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo )
{
	POSRECORD pos;

	if( dwFlags & MOUSEEVENTF_ABSOLUTE )
	{
	    pos.x = (short)dx;
	    pos.y = (short)dy;
		if( dwFlags & MOUSEEVENTF_LEFTDOWN )
		{
			pos.wState = PS_LBUTTON;
		}
		else if( dwFlags & MOUSEEVENTF_LEFTUP )
			pos.wState = 0;
		else
			pos.wState = 0;
		__PutPointerEvent( &pos );
		return TRUE;
	}
	return FALSE;
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

BOOL CALLBACK HandlePosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo )
{
	int xCal, yCal;

	TouchPanelCalibrateAPoint( dx, dy, &xCal, &yCal );  
	//	RETAILMSG( 1, ( "origin x(%d),origin y(%d), xCal(%d)/4=(%d), yCal(%d)/4=(%d).\r\n", dx, dy, xCal, xCal/4, yCal, yCal/4 ) );
	xCal /= 4; yCal /= 4;
	if( lpPosCallBackFun )	// 调用回调函数
		return lpPosCallBackFun( dwFlags, xCal, yCal, dwData, dwExtraInfo );
	else  // 调用默认的
	    return DoPosEvent( dwFlags, xCal, yCal, dwData, dwExtraInfo );
}

// **************************************************
// 声明：LPPOS_CALLBACK _SetPosEventCallBack( LPPOS_CALLBACK lpNewCallBackFun )
// 参数：
// 	IN lpNewCallBackFun - 定位设备回调函数
// 返回值：
//	假入成功，返回之前用的回调函数；否则，返回 NULL
// 功能描述：
//	设置新的定位设备回调函数
// 引用: 
//	
// ************************************************

LPPOS_CALLBACK _SetPosEventCallBack( LPPOS_CALLBACK lpNewCallBackFun )
{
	LPPOS_CALLBACK lpOldFun = lpPosCallBackFun;
	if( lpNewCallBackFun )
	{
		lpPosCallBackFun = lpNewCallBackFun;
		if( lpOldFun )
		    return lpOldFun;
		else
			return (LPPOS_CALLBACK)DoPosEvent;  //返回系统默认的
	}
	else
		return NULL;	
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

VOID WINAPI WinEvent_MouseEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo )
{
	DoPosEvent(dwFlags,  dx,  dy,  dwData,  dwExtraInfo);
}
