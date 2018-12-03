/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/

 
/*****************************************************
文件说明：消息，事件队列管理器
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
    2003-10-07： 增加HWND 为NULL的处理
    2003-06-23: AddToThreadQueue , 增加对队列的初始化代码
    2003-06-03: 增加对 dwForegroundThreadID 的互斥
******************************************************/
 
#include <eframe.h>
#include <eassert.h>
#include <eobjtype.h>
#include <eapisrv.h>
#include <epwin.h>
#include <eevent.h>
#include <winsrv.h>
#include <gdisrv.h>
#include <bheap.h>
#include <gwmeobj.h>

extern _EVENTDRV _posEvent;
extern _EVENTDRV _timerEvent;
extern _EVENTDRV _keyEvent;

static BOOL _InsertThreadQueue( LPGWEDATA lpgwe, LPMSG lpMsg, DWORD dwEventType );
static int _CheckThreadQueue( LPGWEDATA lpgwe, _LPMSGDATA lpMsgRec, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, BOOL fRemove, BOOL fCheckChild );
static BOOL _InitThreadQueue( LPGWEDATA lpgwe );

void HandleSystemQueue( void );
static DWORD _dwSYSQueueState = 0;
static CRITICAL_SECTION  csMsgQueue;
extern DWORD dwActiveTimerCount;
#ifdef EML_DOS
    #include <dos.h>
    #include <bios.h>
#endif

// declare post driver
static int _PostInstall(void);
static int _PostRelease(void);
static int _PostWho(void);
static int _PostPeek( LPMSG, UINT );
static int _PostPut( LPMSG );

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

LPGWEDATA GetGWEPtr( DWORD dwThreadId )
{
	LPGWEDATA lpgwe = NULL;
	if( dwThreadId )
	{
		lpgwe = (LPGWEDATA)GetThreadGWEPtr( dwThreadId );//TlsGetThreadValue( dwThreadId, TLS_MSGQUEUE );
		if( lpgwe == NULL )
		{
		    EnterCriticalSection( &csMsgQueue );
			// lpgwe of thread may be change here, so i get it and check it again
			lpgwe = (LPGWEDATA)GetThreadGWEPtr( dwThreadId );
			if( lpgwe == NULL )  
			{
				lpgwe = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(GWEDATA) );

				if( lpgwe )
				{
					memset( lpgwe, 0, sizeof( GWEDATA ) );
					_InitThreadQueue( lpgwe );
					SetThreadGWEPtr( dwThreadId, lpgwe );
				}
				else
				{
					RETAILMSG( 1, ( "error : GetGWEPtr:no enough memory.\r\n" ) );
				}
			}
			LeaveCriticalSection( &csMsgQueue );
		}
	}
	else
	{
		RETAILMSG( 1, ( "GetGWEPtr error: invalid dwThreadId == 0.\r\n" ) );
	}
	return lpgwe;
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

DWORD GetSYSQueueState( void )
{
    return _dwSYSQueueState;
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

DWORD SetSYSQueueState( DWORD dwStateBit, BOOL bSetBit )
{
    DWORD dwOld = _dwSYSQueueState;

	EnterCriticalSection( &csMsgQueue );
    if( bSetBit )
    {
        extern HANDLE hgwmeEvent;
        
        _dwSYSQueueState |= dwStateBit;

		ASSERT( hgwmeEvent );
		LeaveCriticalSection( &csMsgQueue );
		if( hgwmeEvent )
		    SetEvent(hgwmeEvent);
    }
    else
    {
        _dwSYSQueueState &= ~dwStateBit;
		LeaveCriticalSection( &csMsgQueue );
    }
	
    return dwOld;
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

BOOL _InitSysQueue( void )
{
	InitializeCriticalSection( &csMsgQueue );

	_posEvent.lpInstall();
	_timerEvent.lpInstall();
	_keyEvent.lpInstall();
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

static BOOL _InitQueue( _MSGQUEUE * lpmq, int nInitSize )
{
    void * p;

	int size = nInitSize * sizeof(_MSGDATA);

	ASSERT( lpmq->size == 0 );

	memset( lpmq, 0, sizeof( _MSGQUEUE ) );
	InitializeCriticalSection( &lpmq->csThreadQueue );
	lpmq->csThreadQueue.lpcsName = "CS-QUE";

	lpmq->hWaitEvent = CreateEvent( NULL, FALSE, 0, NULL );

    p = malloc( size  );
    if( p )
    {
        memset( p, 0, size );
        lpmq->size = nInitSize;
        lpmq->nQueueTail = lpmq->nQueueHead = 0;
		lpmq->lpMsgData = (_LPMSGDATA)p;
    }
    return p != NULL;
}
/// free the message queue with the thread

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

BOOL FASTCALL MQ_FreeThreadQueue( void )
{
	//LPGWEDATA lpgwe = TlsGetValue( TLS_MSGQUEUE );
	DWORD dwCurThreadId = GetCurrentThreadId();
    LPGWEDATA lpgwe = GetThreadGWEPtr(dwCurThreadId);

    if( lpgwe )
	{
		//TlsSetValue( TLS_MSGQUEUE, 0 );
		SetThreadGWEPtr( dwCurThreadId, 0 );   

		
		//if( lpgwe->msgQueue.csThreadQueue.hSemaphore )
		{
			EnterCriticalSection( &lpgwe->msgQueue.csThreadQueue );
			LeaveCriticalSection( &lpgwe->msgQueue.csThreadQueue );
			
			DeleteCriticalSection( &lpgwe->msgQueue.csThreadQueue );
		}
		if( lpgwe->msgQueue.hWaitEvent )
			CloseHandle( lpgwe->msgQueue.hWaitEvent );
		
		if( lpgwe->msgQueue.lpMsgData )
		{
			free( lpgwe->msgQueue.lpMsgData );
			lpgwe->msgQueue.lpMsgData = NULL;
		}
		
		BlockHeap_Free( hgwmeBlockHeap, 0, lpgwe, sizeof(GWEDATA) );
	}

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

static BOOL _InsertQueue(_MSGQUEUE * lpQueue, LPMSG lpMsg, UINT eventType )
{
    BOOL bRetv = FALSE;

    ASSERT( lpMsg->hwnd != INVALID_HANDLE_VALUE );

    EnterCriticalSection( &lpQueue->csThreadQueue );

_I_REPEAT:    
    if( lpQueue->count < lpQueue->size )
    {    
        lpQueue->lpMsgData[lpQueue->nQueueTail].msg = *lpMsg;
		lpQueue->lpMsgData[lpQueue->nQueueTail].eventType = (WORD)eventType;

        if( ++(lpQueue->nQueueTail) >= lpQueue->size)
            lpQueue->nQueueTail = 0;
        lpQueue->count++;
		bRetv = TRUE;
    }
    else
    {   //queue is full
        _LPMSGDATA p;
        p = (_LPMSGDATA)realloc( lpQueue->lpMsgData, (lpQueue->size+8) * sizeof(_MSGDATA) );
        if( p )
        {                
            int i;

            lpQueue->nQueueTail = lpQueue->size;
            lpQueue->size += 8;
			lpQueue->lpMsgData = p;

            for ( i = 0; i < lpQueue->nQueueHead; i++ )
            {
                lpQueue->lpMsgData[lpQueue->nQueueTail] = lpQueue->lpMsgData[i];
                if( ++(lpQueue->nQueueTail) >= lpQueue->size )
                    lpQueue->nQueueTail = 0;
            }

            goto _I_REPEAT;
        }
		else
		{
			RETAILMSG( 1, ( "error: in _InsertQueue, no enough memory!.\r\n" ) );
		}
    }

	LeaveCriticalSection( &lpQueue->csThreadQueue );

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

static BOOL _RemoveQueue(_MSGQUEUE *lpQueue)
{
    BOOL bRetv = TRUE;

    EnterCriticalSection( &lpQueue->csThreadQueue );

    if( lpQueue->count > 0 )
    {
        if( ++(lpQueue->nQueueHead) >= lpQueue->size )
            lpQueue->nQueueHead = 0;
        lpQueue->count--;
    }
	else
		bRetv = FALSE;

	LeaveCriticalSection( &lpQueue->csThreadQueue );

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

static BOOL _InitThreadQueue( LPGWEDATA lpgwe )
{
    {
        {
			_InitQueue( &lpgwe->msgQueue, 8 );

            return TRUE;
        }
    }
    ASSERT( 0 );
    return 0;
}

// **************************************************
// 声明：DWORD WINAPI _WinSys_WaitMessage( DWORD dwTimeOut )
// 参数：
// 	dwTimeOut - 超时等待时间
// 返回值：
//	假入成功，返回WAIT_OBJET_0；否则，返回WAIT_FAILED
// 功能描述：
//	
// 引用: 
//	
// ************************************************

DWORD WINAPI _WinSys_WaitMessage( DWORD dwTimeOut )
{       
#ifdef SINGLE_THREAD
    return WAIT_OBJECT_0;
#else
	LPGWEDATA lpgwe = GetGWEPtr( GetCurrentThreadId() );

	if( lpgwe )
        return WaitForSingleObject( lpgwe->msgQueue.hWaitEvent, dwTimeOut );
	else
		return WAIT_FAILED;

#endif

}

// **************************************************
// 声明：VOID WINAPI _WinSys_SetMessageEvent( void )
// 参数：
// 	无
// 返回值：
//	无
// 功能描述：
//	设置消息事件
// 引用: 
//	
// ************************************************

VOID WINAPI _WinSys_SetMessageEvent( DWORD dwThreadId )
{       
	LPGWEDATA lpgwe = GetGWEPtr( dwThreadId );

	if( lpgwe )
	{
		SetEvent( lpgwe->msgQueue.hWaitEvent );
	}
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

BOOL WINAPI _WinSys_PostThreadMessage( DWORD dwThreadID, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPGWEDATA lpgwe = GetThreadGWEPtr( dwThreadID );

	if( lpgwe )
    {
        //put message to queue
        MSG msg;
        memset( &msg, 0, sizeof( msg ) );
        msg.hwnd = NULL;
        msg.lParam = lParam;
        msg.message = uMsg;
        msg.wParam = wParam;

		_InsertThreadQueue( lpgwe, &msg, QS_POSTMESSAGE );
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

static BOOL _InsertThreadQueue( LPGWEDATA lpgwe, LPMSG lpMsg, DWORD dwEventType )
{
    if( lpgwe->msgQueue.size )
    {
        _InsertQueue( &lpgwe->msgQueue, lpMsg, dwEventType );

		lpgwe->msgQueue.dwEventMask |= dwEventType;
		SetEvent( lpgwe->msgQueue.hWaitEvent );
        return TRUE;
    }
	else
	{
		RETAILMSG( 1, ( "error: in _InsertThreadQueue, lpgwe->msgQueue.size == 0!.\r\n" ) );
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

BOOL AddMsgToThreadQueue( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT uFlag )
{
    MSG mst;
	DWORD dwID;
	LPGWEDATA lpgwe;
	BOOL bRetv = FALSE;

	if( hWnd )
	{
		dwID = Win_GetThreadProcessId( hWnd, NULL );
		if( dwID == 0 )
		    dwID = Win_GetThreadProcessId( hWnd, NULL );
		lpgwe = GetGWEPtr( dwID );
		//RETAILMSG( 1, ( "test1.\r\n" ) );
		if( lpgwe )
		{
			//RETAILMSG( 1, ( "test2.\r\n" ) );
			if( (uFlag & ATQ_ONLYONE) )        
			{
				//RETAILMSG( 1, ( "test3.\r\n" ) );
				if( !_CheckThreadQueue( lpgwe, 0, hWnd, msg, msg, FALSE, 0 ) )
				{
					mst.message = msg;
					mst.wParam = wParam;
					mst.lParam = lParam;
					mst.hwnd = hWnd;
					bRetv = _InsertThreadQueue( lpgwe, &mst, uFlag & QS_EVENT_MASK );
				}
			}
			else
			{   
				//RETAILMSG( 1, ( "test4.\r\n" ) );
				// LN, 2003-06-23-begin
				if( !lpgwe->msgQueue.size )
				{   // 第一次， create a queue
					//RETAILMSG( 1, ( "test4-1.\r\n" ) );
					if( !_InitThreadQueue(lpgwe) )
					{
						//RETAILMSG( 1, ( "error: in AddToThreadQueue, !_InitThreadQueue(lpgwe).\r\n" ) );
						return FALSE;
					}
				}
				// LN, 2003-06-23-end
				//RETAILMSG( 1, ( "test5.\r\n" ) );

				mst.message = msg;
				mst.wParam = wParam;
				mst.lParam = lParam;
				mst.hwnd = hWnd;
				
				bRetv = _InsertThreadQueue( lpgwe, &mst, uFlag & QS_EVENT_MASK );				
			}
		}
		else
		{
			RETAILMSG( 1, ( "error: AddToThreadQueue, no lpgwe!.\r\n" ) );
		}
	}
    return bRetv;
}

// **************************************************
// 声明：BOOL ClearThreadQueue( DWORD dwThreadId, HWND hWnd, UINT msgStart, UINT msgEnd )
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从线程队列清除窗口消息
// 引用: 
//	
// ************************************************

BOOL ClearThreadQueue( DWORD dwThreadID, HWND hWnd, UINT msgStart, UINT msgEnd )
{   // if 0xffffffff, all msg about the window  
    _MSGQUEUE * lpmq;
    LPGWEDATA lpgwe;
	BOOL bRetv = TRUE;

	ASSERT( msgStart <= msgEnd );

	if( dwThreadID == 0 )
	{
		if( hWnd )
			dwThreadID = _GetHWNDPtr(hWnd)->dwThreadID;
		else
			dwThreadID = GetCurrentThreadId();
	}

    lpgwe = GetGWEPtr( dwThreadID );	

    if( lpgwe && lpgwe->msgQueue.size )
    {
        int count;
        int nHead;
        int nTail;
        int nSize;
        _LPMSGDATA lpMsgData;        

		lpmq = &lpgwe->msgQueue;

		EnterCriticalSection( &lpmq->csThreadQueue );

        if( (count = lpmq->count) == 0 )
		{
			goto CLEAR_LEAVE;
		}
        
        nHead = lpmq->nQueueHead;
        nTail = lpmq->nQueueTail;
        nSize = lpmq->size;

        lpMsgData =  (lpmq->lpMsgData+lpmq->nQueueHead);
        while( count > 0 )
        {   
            if( hWnd == NULL ||
				lpMsgData->msg.hwnd == hWnd )
            {
                if( msgStart == 0 &&
					msgEnd == 0 )
                    lpMsgData->msg.hwnd = INVALID_HANDLE_VALUE;
                else if( msgStart <= lpMsgData->msg.message &&
					     msgEnd >=  lpMsgData->msg.message )
                    lpMsgData->msg.hwnd = INVALID_HANDLE_VALUE;
            }

            lpMsgData++;
            if( ++nHead >= nSize )
            {
                nHead = 0;
                lpMsgData = lpmq->lpMsgData;
            }            
            count--;           
        }
CLEAR_LEAVE:
		LeaveCriticalSection( &lpmq->csThreadQueue );
    }
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

static int _CheckThreadQueue( LPGWEDATA lpgwe, _LPMSGDATA lpMsgRec, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, BOOL fRemove, BOOL fCheckChild )
{
    int retv = FALSE;
    BOOL bFilterWindow = (BOOL)hWnd;
    BOOL bFilterMsg = wMsgFilterMax | wMsgFilterMin;
	if( lpgwe == NULL )
	{
		return 0;
	}

    if( !lpgwe->msgQueue.size )
    {   // create a queue
        if( !_InitThreadQueue(lpgwe) )
            return 0;
    }

    {
        _MSGQUEUE * lpmq = &lpgwe->msgQueue;

        EnterCriticalSection( &lpmq->csThreadQueue );

        if( lpmq->count > 0 )
        {
            int nHead = lpmq->nQueueHead;
            int nSize = lpmq->size;
            int nCount = lpmq->count;
            BOOL bClearInvalidMsg = FALSE;
            _LPMSGDATA lpMsgData = lpmq->lpMsgData+lpmq->nQueueHead;

            while( nCount )
            {
				HWND hwndMsg = lpMsgData->msg.hwnd;
				UINT message = lpMsgData->msg.message;
                if( hwndMsg == INVALID_HANDLE_VALUE || 
                    (hwndMsg && !Win_IsWindow(hwndMsg)) )
                {                    
                    if( bFilterWindow || bFilterMsg )
                    {
                        bClearInvalidMsg = TRUE;
                        lpMsgData->msg.hwnd = INVALID_HANDLE_VALUE;
                        goto _L_NEXT;
                    }
                    else
                    {  // remove the message
                        if( ++(lpmq->nQueueHead) >= lpmq->size )
                            lpmq->nQueueHead = 0;
                        lpmq->count--;
                        goto _L_NEXT;
                    }                
                }
                if( bFilterWindow )
                {
                    if( hwndMsg != hWnd )
                    {   
                        if( !( fCheckChild && hwndMsg && Win_IsChild( hWnd, hwndMsg ) ) )
                            goto _L_NEXT;
                    }
                }
                if( bFilterMsg && 
                    (message < wMsgFilterMin || message > wMsgFilterMax ) )
                {
                    goto _L_NEXT;
                }

                // a valid message
                if( lpMsgRec )
                    *lpMsgRec = *lpMsgData;
                if( fRemove && message != WM_PAINT )
                {
                    if( bFilterWindow || bFilterMsg )
                    {
                        lpMsgData->msg.hwnd = INVALID_HANDLE_VALUE;
                        bClearInvalidMsg = TRUE;
                    }
                    else
                    {
                        if( ++(lpmq->nQueueHead) >= lpmq->size )
                            lpmq->nQueueHead = 0;
                        lpmq->count--;
                    }
                }
                retv = TRUE;
                break;
_L_NEXT:
                if( ++nHead >= nSize )
                {
                    nHead = 0;
                    lpMsgData = lpmq->lpMsgData;
                }
                else
                    lpMsgData++;
                nCount--;
            }
            // clear invalid message if possible
            if( bClearInvalidMsg )
            {
                int nSave, nSaveCount;
                _LPMSGDATA lpSaveMsg;

                nSave = nHead = lpmq->nQueueHead;
                nCount = lpmq->count;
                lpSaveMsg = lpMsgData = lpmq->lpMsgData+lpmq->nQueueHead;
                nSaveCount = 0;

                while( nCount )
                {
                    if( lpMsgData->msg.hwnd != INVALID_HANDLE_VALUE )
                    {
                        nSaveCount++;
                        if( lpSaveMsg != lpMsgData )
                            *lpSaveMsg = *lpMsgData;
                        if( ++nSave >= nSize )
                        {
                            nSave = 0;
                            lpSaveMsg = lpmq->lpMsgData;
                        }
                        else
                            lpSaveMsg++;
                    }
                    if( ++nHead >= nSize )
                    {
                        nHead = 0;
                        lpMsgData = lpmq->lpMsgData;
                    }
                    else
                        lpMsgData++;
                    nCount--;
                }
                lpmq->count = nSaveCount;
                lpmq->nQueueTail = nSave;
            }
        }
        LeaveCriticalSection( &lpmq->csThreadQueue );
    }
    return retv;
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

int CheckThreadQueue( _LPMSGDATA lpMsgRec, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, BOOL fRemove, BOOL fCheckChild )
{
    LPGWEDATA lpgwe = GetGWEPtr( GetCurrentThreadId() );
	if( lpgwe )
	{
		lpgwe->msgQueue.dwEventMask = 0;  // clear
	   return _CheckThreadQueue( lpgwe, lpMsgRec, hWnd, wMsgFilterMin, wMsgFilterMax, fRemove, fCheckChild );
	}
	return 0;
}


typedef void (*LPEVENTPROC)(void);
static void _DoMouseEvent(void);
static void _DoKeyEvent(void);
static void _DoTimerEvent(void);

LPEVENTPROC lpEventProc[32] = { 
    //_DoPostEvent,  // ES_POSTMESSAGE
    _DoKeyEvent,   // ES_KEY
    _DoMouseEvent, // ES_MOUSEMOVE
	_DoMouseEvent, // ES_MOUSEBUTTON
	NULL,   // QS_POSTMESSAGE
    _DoTimerEvent, // ES_TIMER
    // from 4~30
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0,0,0,0,
    0,0, };

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

static void _DoMouseEvent( void )
{
    extern HWND _Win_FromPoint( POINT pt, int * lpfDisable );
    MSG msg;
    HWND hWnd = NULL;
    int fDisable;
	LPGWEDATA lpgweFore = NULL;
	DWORD dwForeThreadID;

	dwActiveTimerCount = GetTickCount();

    memset( &msg, 0, sizeof( msg ) );
    dwForeThreadID = LockForegroundThreadId(); 
	if( dwForeThreadID )
		lpgweFore = GetGWEPtr( dwForeThreadID );		
	UnlockForegroundThreadId();  
	
    while( _posEvent.lpPeek( &msg, PM_REMOVE ) )
    {
		static DWORD dwDownThreadID = 0;
		LPGWEDATA lpgwe;
	    int bPostMsg = FALSE;
	    DWORD dwWindowThreadID = 0;

	    hWnd = _Win_FromPoint( msg.pt, &fDisable );

		if( hWnd )
		    dwWindowThreadID = GetWindowThreadProcessId( hWnd, NULL );
		if( lpgweFore && lpgweFore->hwndCapture )
		{
			msg.hwnd = lpgweFore->hwndCapture;
			_InsertThreadQueue( lpgweFore, &msg, QS_MOUSE );
			bPostMsg = TRUE;
			// 2005-09-26, 因为增加了 ，该功能会向 其它线程的窗口发消息
			// 当拖动窗口时，会导致拖动框 绘制有 脏点
			// 先暂时去掉
			//if( dwWindowThreadID == dwForeThreadID )
			//
			    dwWindowThreadID = 0;  // 不要再给dwWindowThreadID发消息
		}
		if( bPostMsg == FALSE && dwDownThreadID  )
		{   // when the downwindow's thread is capturing the mouse but 
			// the downwindow is not foreground thread, 
			// the handle here
			if( dwDownThreadID != dwForeThreadID &&
				dwDownThreadID != dwWindowThreadID )
			{
				lpgwe = GetGWEPtr( dwDownThreadID );
				if( lpgwe )
				{
					if( lpgwe->hwndCapture )
					{
						_InsertThreadQueue( lpgwe, &msg, QS_MOUSE );
						bPostMsg = TRUE;
					}
				}	
				else
					dwDownThreadID = 0;
			}
		}
		if( dwWindowThreadID )
		{
			lpgwe = GetGWEPtr( dwWindowThreadID );
			if( lpgwe )
			{
			    msg.hwnd = hWnd;
			    _InsertThreadQueue( lpgwe, &msg, QS_MOUSE );
			}
		}
		if( msg.message == WM_LBUTTONDOWN )
		{
			dwDownThreadID = dwWindowThreadID;
		}
		else if( msg.message == WM_LBUTTONUP )  // up
			dwDownThreadID = 0;
    }
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

static void _DoKeyEvent( void )
{
    MSG msg;
    DWORD dwID = LockForegroundThreadId();
	LPGWEDATA lpgwe = NULL;

	if( dwID )
	{
		lpgwe = GetGWEPtr( dwID );
	}
	UnlockForegroundThreadId();

	dwActiveTimerCount = GetTickCount();

	if( dwID )
	{
		memset( &msg, 0, sizeof( msg ) );

		if( lpgwe )
		{
			msg.hwnd = lpgwe->hwndFocus;
			
			while( _keyEvent.lpPeek( &msg, PM_REMOVE ) )
			{        
				if( msg.hwnd )
					_InsertThreadQueue( lpgwe, &msg, QS_KEY );
			}
			return;
		}
	}

	//clear key event if no any window need 
	while( _keyEvent.lpPeek( &msg, TRUE ) )
		;
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

static void _DoTimerEvent( void )
{
    MSG msg;
    
    while( _timerEvent.lpPeek( &msg, PM_REMOVE ) )
    {
        if( msg.hwnd )
		{
			AddMsgToThreadQueue( msg.hwnd, msg.message, msg.wParam, msg.lParam, ATQ_ONLYONE | QS_TIMER );
		}
    }
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

void HandleSystemQueue( void )
{
    int i = 0;
    BOOL bHasEvent = FALSE;
    LPEVENTPROC * lpProc = lpEventProc;
	DWORD dwState;

	EnterCriticalSection( &csMsgQueue );

	dwState = _dwSYSQueueState;
	_dwSYSQueueState = 0;
	LeaveCriticalSection( &csMsgQueue );

    for( i = 0; i < 31 && dwState; i++, lpProc++ )
    {
        if( dwState & 0x1 )
        {
            if( *lpProc )
                (*lpProc)();
            bHasEvent = TRUE;
        }
		dwState >>= 1;
    }
}
