/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：gwme 时钟管理
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
	2004-07-02:将对象改为动态分配，静态分配的文件为 timer_static_alloc.c
    2003-04-14:增加冲突段检查
******************************************************/

#include <eframe.h>
#include <eevent.h>
#include <epwin.h>
#include <bheap.h>
#include <gwmeobj.h>

static int _TimerInstall(void);
static int _TimerRelease(void);
static int _TimerWho(void);
static int _TimerPeek( LPMSG, UINT );
static int _TimerPut( LPMSG );
static int _TimerRemove(void);
static UINT _TimerCount(void);

_EVENTDRV _timerEvent = {
    _TimerInstall,
    _TimerRelease,
    _TimerWho,
    _TimerPeek,
    _TimerPut,
    _TimerRemove,
    _TimerCount
};

// define timer driver

#define TEF_SIGNAL   0x1
typedef struct _TIMEREVENT
{
	struct _TIMEREVENT * lpNext;
	struct _TIMEREVENT * lpPrev;
	HANDLE hOwnerProcess;
    HWND hWnd;
    UINT msg;
    UINT uID;
    UINT uElaspe;
	UINT flag;//Used; // TEF_SIGNAL
    DWORD oldTicks;
    TIMERPROC lpTimerProc;
}TIMEREVENT;


static TIMEREVENT * lpTimerServer = NULL;
static CRITICAL_SECTION csTimerQueue;

// **************************************************
// 声明：static VOID AddTimer( TIMEREVENT * lpTimer )
// 参数：
// 	IN/OUT lpTimer - TIMEREVENT 结构指针
// 返回值：
//	无
// 功能描述：
//	将 timer 加入队列
// 引用: 
//	
// ************************************************

static VOID AddTimer( TIMEREVENT * lpTimer )
{
	ASSERT( lpTimer->lpNext == NULL );
	if( lpTimerServer )
	{	// 连接到链表
		lpTimer->lpNext = lpTimerServer;//->lpNext;
		lpTimer->lpPrev = lpTimerServer->lpPrev;
		lpTimerServer->lpPrev->lpNext = lpTimer;
		lpTimerServer->lpPrev = lpTimer;

	}
	else
	{
		lpTimer->lpNext = lpTimer;
		lpTimer->lpPrev = lpTimer;
	}
	lpTimerServer = lpTimer;
}

// **************************************************
// 声明：static VOID RemoveTimer( TIMEREVENT * lpTimer )
// 参数：
// 	IN/OUT lpTimer - TIMEREVENT 结构指针
// 返回值：
//	无
// 功能描述：
//	将 timer 移出队列
// 引用: 
//	
// ************************************************

static VOID RemoveTimer( TIMEREVENT * lpTimer )
{
	ASSERT( lpTimerServer && lpTimer && lpTimer->lpNext );
	if( lpTimer->lpNext == lpTimer )
	{	//仅仅一个
		lpTimerServer = NULL;
		lpTimer->lpNext = NULL;
	}
	else
	{	//多个,移出链表
		lpTimer->lpNext->lpPrev = lpTimer->lpPrev;
		lpTimer->lpPrev->lpNext = lpTimer->lpNext;
		if( lpTimerServer == lpTimer )	//在队列头吗 ？
			lpTimerServer = lpTimer->lpNext;	//是
	}
}


// **************************************************
// 声明：static TIMEREVENT * _GetWinTimer( void )
// 参数：
// 	无
// 返回值：
//	假入成功，返回 TIMEREVENT 结构指针；否则，返回 NULL
// 功能描述：
//	得到一个到期的 timer
// 引用: 
//	
// ************************************************

static TIMEREVENT * _GetWinTimer( BOOL bRemove )
{
	TIMEREVENT * lpTimer, * lpGetTimer = NULL;

	EnterCriticalSection( &csTimerQueue );

	if( (lpTimer = lpTimerServer) )
	{
		do{
			if( lpTimer->flag & TEF_SIGNAL )
			{
				if( bRemove )
				{
					lpTimer->flag &= ~TEF_SIGNAL;
					lpTimer->oldTicks = GetTickCount();
				}
				lpGetTimer = lpTimer;
				lpTimerServer = lpTimer->lpNext;	//以后从下一次开始检查
				break;
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	
	LeaveCriticalSection( &csTimerQueue );
	return lpGetTimer;

}

// **************************************************
// 声明：BOOL CheckWinTimerEvent( void )
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	检查是否有 timer 事件产生
// 引用: 
//	
// ************************************************

BOOL CheckWinTimerEvent( void )
{
	BOOL bRetv = FALSE;
    DWORD diff;
    DWORD ticks;
	TIMEREVENT * lpTimer;

	EnterCriticalSection( &csTimerQueue );

	lpTimer = lpTimerServer;

	
	if( lpTimer )
	{
		ticks = GetTickCount();
		do
		{
			ASSERT( lpTimer->hWnd || lpTimer->lpTimerProc );
			//是否已做信号标志
			if( (lpTimer->flag & TEF_SIGNAL) == 0 )
			{	//没有，检查它
				if( ticks > lpTimer->oldTicks )
					diff = ( ticks - lpTimer->oldTicks );
				else
					diff = ticks + ( ~lpTimer->oldTicks );
				if( diff >= lpTimer->uElaspe )
				{
					lpTimerServer = lpTimer;
					lpTimer->flag |= TEF_SIGNAL;					
					bRetv = TRUE; // 有信号
					break;
				}
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}

	LeaveCriticalSection( &csTimerQueue );

	if( bRetv )
		SetSYSQueueState( ES_TIMER, TRUE );
	return bRetv;
}

// **************************************************
// 声明：static UINT _TimerCount(void)
// 参数：
// 	无
// 返回值：
//	返回 当前系统的 timer 数
// 功能描述：
//	得到当前系统已设置的 timer 数
// 引用: 
//	
// ************************************************

static UINT _TimerCount(void)
{
    int n  = 0;
	TIMEREVENT * lpTimer;
	
	EnterCriticalSection( &csTimerQueue );	

	if( (lpTimer = lpTimerServer) )
	{
		do
		{
			n++;
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	LeaveCriticalSection( &csTimerQueue );
    return n;
}

// **************************************************
// 声明：static int _TimerInstall()
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化 timer 管理器
// 引用: 
//	
// ************************************************

static int _TimerInstall()
{
	InitializeCriticalSection( &csTimerQueue );
	lpTimerServer = NULL;
    return 1;
}

// **************************************************
// 声明：static int _TimerRelease()
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	与 _TimerInstall 相反， 释放timer 管理器
// 引用: 
//	
// ************************************************

static int _TimerRelease()
{
	DeleteCriticalSection( &csTimerQueue );
    return 1;
}

// **************************************************
// 声明：static int _TimerWho()
// 参数：
// 	无
// 返回值：
//	返回 ID_TIMER
// 功能描述：
//	得到 该事件管理器的ID
// 引用: 
//	
// ************************************************

static int _TimerWho()
{
    return ID_TIMER;
}

// **************************************************
// 声明：static int _TimerPeek( LPMSG lpEvent, UINT fRemove )
// 参数：
// 	OUT lpEvent - 用于接受有信号的事件
//	IN fRemove - 是否移出信号
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从 timer 管理器检索一个有信号的事件
// 引用: 
//	
// ************************************************

static int _TimerPeek( LPMSG lpEvent, UINT fRemove )
{
	int iRetv = 0;
	TIMEREVENT * lpTimer;

	EnterCriticalSection( &csTimerQueue );

_repeat:
	lpTimer = _GetWinTimer(fRemove);
	if( lpTimer )
	{		
		if( lpTimer->hWnd )
		{
			if( !IsWindow( lpTimer->hWnd ) )
			{
				WARNMSG( 1, ("Invalid window handle:%x in timer server\r\n", lpTimer->hWnd ) );
				RemoveTimer( lpTimer );
				BlockHeap_Free( hgwmeBlockHeap, 0, lpTimer, sizeof(TIMEREVENT) );
				goto _repeat;
			}
		}
		lpEvent->message = lpTimer->msg;
		lpEvent->hwnd = lpTimer->hWnd;
		lpEvent->wParam = lpTimer->uID;
		lpEvent->lParam = (LPARAM)lpTimer->lpTimerProc;
		iRetv = 1;
	}

	LeaveCriticalSection( &csTimerQueue );
	return iRetv;
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

static int _TimerPut( LPMSG lpUnused )
{
    return 0;
}

// **************************************************
// 声明：static int _TimerRemove()
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将当前有信号的 timer 去掉信号，再将队列移到下一个
// 引用: 
//	
// ************************************************

static int _TimerRemove()
{
	EnterCriticalSection( &csTimerQueue );

	if( lpTimerServer )
	{
		lpTimerServer->flag &= ~TEF_SIGNAL;
		lpTimerServer = lpTimerServer->lpNext;		
	}
	LeaveCriticalSection( &csTimerQueue );

    return 1;
}

// **************************************************
// 声明：UINT FASTCALL _SetTimer( HWND hWnd, UINT msg, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN msg - 需要发送的消息
//	IN id - timer ID
//	IN uElapse - 到期时间
//	IN lpTimerProc - timer 处理函数，可以为 NULL
// 返回值：
//	假入成功，返回 ID, 失败，返回 0
// 功能描述：
//	创建新的 timer
// 引用: 
//	
// ************************************************

UINT FASTCALL _SetTimer( HANDLE hOwnerProcess, HWND hWnd, UINT msg, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
{
	UINT uiRetv = 0;
    TIMEREVENT * lpTimer, * lpTimerUse = NULL;
	

    ASSERT( hWnd || lpTimerProc );
	if( uElapse == 0 )
		uElapse = 1;
	// EnterCri
	EnterCriticalSection( &csTimerQueue );
	//检查是否已有该 timer
	lpTimer = lpTimerServer;
	if( lpTimer )
	{
		do{
			if( lpTimer->uID == id && lpTimer->hWnd == hWnd )
			{
				lpTimerUse = lpTimer;
				break;
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	if( lpTimerUse == NULL )
	{
		lpTimerUse = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(TIMEREVENT) );
		if( lpTimerUse )
			memset( lpTimerUse, 0, sizeof(TIMEREVENT) );
	}
	if( lpTimerUse )
	{
		lpTimerUse->hWnd = hWnd;
		lpTimerUse->msg = msg;
		lpTimerUse->uElaspe = uElapse;
		lpTimerUse->lpTimerProc = lpTimerProc;
		lpTimerUse->hOwnerProcess = hOwnerProcess;
		if( hWnd ) // the  id is valid
			lpTimerUse->uID = id;
		else
		{
			id = lpTimerUse->uID = (UINT)lpTimerUse;
		}		
		lpTimerUse->oldTicks = GetTickCount();
		uiRetv = lpTimerUse->uID;
		//timer 是新分配的吗 ？
		if( lpTimerUse->lpNext == NULL )
		{	//是，擦入
			AddTimer( lpTimerUse );
		}
	}

	LeaveCriticalSection( &csTimerQueue );
	return uiRetv;
}

// **************************************************
// 声明：UINT FASTCALL _KillTimer( HWND hWnd, UINT msg, UINT id )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN msg - 需要发送的消息
//	IN id - timer ID
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放 timer
// 引用: 
//	
// ************************************************

UINT FASTCALL _KillTimer( HWND hWnd, UINT msg, UINT id )
{
	UINT uiRetv = FALSE;
    TIMEREVENT * lpTimer;

	EnterCriticalSection( &csTimerQueue );
	lpTimer = lpTimerServer;

	if( lpTimer )
	{
		do{
			if( 
				lpTimer->hWnd == hWnd &&
				lpTimer->msg == msg &&
				lpTimer->uID == id )
			{
				RemoveTimer( lpTimer );
				BlockHeap_Free( hgwmeBlockHeap, 0, lpTimer, sizeof(TIMEREVENT) );
				if( hWnd )
					ClearThreadQueue( 0, hWnd, msg, msg );
				uiRetv = TRUE;
				break;
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	LeaveCriticalSection( &csTimerQueue );
	
    return uiRetv;
}


// **************************************************
// 声明：VOID FASTCALL _ClearProcessTimer( HANDLE hProcess )
// 参数：
// 	IN hProcess - 进程句柄
// 返回值：
//	无
// 功能描述：
//	当一个进程退出时，释放该进程的 timer资源
// 引用: 
//	
// ************************************************

VOID FASTCALL _ClearProcessTimer( HANDLE hProcess )
{
    TIMEREVENT * lpTimer;

	//进入冲突段
	EnterCriticalSection( &csTimerQueue );

	lpTimer = lpTimerServer;

	if( lpTimer )
	{
		do{
			if( lpTimer->hOwnerProcess == hProcess )
			{
				TIMEREVENT * lpSave = lpTimer->lpNext;
				//移出和释放
				RemoveTimer( lpTimer );
				BlockHeap_Free( hgwmeBlockHeap, 0, lpTimer, sizeof(TIMEREVENT) );
				if( lpTimerServer == NULL )
					break;	//没有任何 timer 了
				lpTimer = lpSave;
				continue;
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	LeaveCriticalSection( &csTimerQueue );	
}
