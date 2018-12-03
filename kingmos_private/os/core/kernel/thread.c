/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：线程管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
	2004-12-21, 将 thread struct 定为 4 *1024 , 其中1024 作为 loadfailuerpage, 
				[thread data]  xxx
				[thread kernel stack] xxx -> 3*1024, user by exception, int ...
				[use by loadfailurepage(1024)]
	         
	2004-08-11, 不是用动态分配( WaitForSingleObject。WaitMultiSingleObject  ) 信号量，涉及到 semaphpor.c thread.c sche.c
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <eobjlist.h>
#include <epheap.h>
#include <epwin.h> 
#include <epalloc.h>
#include <epcore.h>
//#include <isr.h>
#include <coresrv.h>
#include <eerror.h>

#define MIN_STACK_SIZE (4 * 1024)
#define PROCESS_SPACE  (32 * 1024 * 1024)
#define UP_PROTECT_SIZE (64*1024)
#define DOWN_PROTECT_SIZE (64*1024)
#define PROTECT_SIZE ( UP_PROTECT_SIZE + DOWN_PROTECT_SIZE )

static void DoTerminateThread( LPTHREAD lpThread, DWORD dwExitCode );
static BOOL _DeleteThreadObjectPtr( LPTHREAD lpThread );
static BOOL _FreeThreadResourceAndReschedule( void );

static VOID FreeThreadStruct( LPTHREAD lpThread );
static LPTHREAD AllocThreadStruct( VOID );

// ********************************************************************
// 声明：static void * AllocThreadStack( DWORD dwCreateFlag, LPPROCESS lpProcess, DWORD dwSize )
// 参数：
//	IN dwCreateFlag - 创建标志（参考CreateThread）
//	IN lpProcess - PROCESS 结构指针
//	IN dwSize - 需要的栈大小
// 返回值：
//	返回分配的内存地址
// 功能描述：
//	为一个新的线程分配一个运行栈
// 引用：
//		
// ********************************************************************
#define DEBUG_AllocThreadStack 0

static void * AllocThreadStack( DWORD dwCreateFlag, LPPROCESS lpProcess, DWORD dwSize )
{
#ifdef VIRTUAL_MEM

    LPVOID lpStack, lpUpProtect, lpDownProtect;

	DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: use virtual mem:dwSize(0x%x) entry.\r\n", dwSize ) );
	if( dwCreateFlag & CREATE_MAINTHREAD )	//是主线程吗？
	{	//是，从进程空间的末端分配
		DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: main thread.\r\n" ) );
		ASSERT( dwSize <= 64 * 1024 ); 
	    lpStack = KC_VirtualAlloc( (LPVOID)( lpProcess->dwVirtualAddressBase + PROCESS_SPACE - dwSize - UP_PROTECT_SIZE ), dwSize, MEM_RESERVE|MEM_AUTO_COMMIT, PAGE_READWRITE );
		
		//lpStack = KC_VirtualAlloc( (LPVOID)lpProcess->dwVirtualAddressBase, dwSize, MEM_RESERVE|MEM_AUTO_COMMIT, PAGE_READWRITE );
		
	}
	else	//从进程空间的任意地方分配
		lpStack = KC_VirtualAlloc( (LPVOID)( lpProcess->dwVirtualAddressBase ), dwSize, MEM_RESERVE|MEM_AUTO_COMMIT, PAGE_READWRITE );

	DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: lpStack=0x%x.\r\n", lpStack ) );
	if( lpStack )
	{
		LPVOID p;	

		DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: commit.\r\n" ) );
		//提交内存
        if( ( p = KC_VirtualAlloc( (LPVOID)( (DWORD)lpStack+dwSize-MIN_STACK_SIZE ), MIN_STACK_SIZE, MEM_COMMIT, PAGE_READWRITE ) ) ) 
		{
			DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: commit ptr(0x%x).\r\n", p ) );
#ifdef __DEBUG
		    memset( p, 0xCCCCCCCC, MIN_STACK_SIZE );
#endif
		}
		else
		{
			RETAILMSG( 1, ( "error at AllocThreadStack: not commit success!.\r\n" ) );
		}
	}
	else
	{
		RETAILMSG( 1, ( "error at AllocThreadStack: not alloc success!.\r\n" ) );
	}

	return lpStack;

#else

	LPVOID lpStack = KL_AllocPageMem( dwSize, NULL, 0 );
	
      DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: call KL_AllocPageMem:dwSize(0x%x) lpStack=(0x%x) entry.\r\n", dwSize, lpStack ) );	
#ifdef __DEBUG
	if( lpStack )
	{
		memset( lpStack, 0xCCCCCCCC, dwSize );
	}
#endif
	return lpStack;

#endif
}

// ********************************************************************
// 声明：static void FreeThreadCallStack( CALLSTACK * lpcs )
// 参数：
//		IN lpcs - CALLSTACK 结构指针
// 返回值：
//		无
// 功能描述：
//		释放线程呼叫堆结构
// 引用：
//		
// ********************************************************************
#define DEBUG_FREETHREADCALLSTACK 0

static void FreeThreadCallStack( CALLSTACK * lpcs )
{
	while( lpcs )
	{
		CALLSTACK * lpNext = lpcs->lpNext;

		DEBUGMSG( DEBUG_FREETHREADCALLSTACK, ( "fcs:(0x%x), callinfo(0x%x).\r\n", lpcs, lpcs->dwCallInfo ) );
		ASSERT( lpcs->dwCallInfo & CALL_ALLOC );
        if( lpcs->dwCallInfo & CALL_ALLOC )
		{
            KHeap_Free( lpcs, sizeof( CALLSTACK ) );
		}
		lpcs = lpNext;
	}
}

// ********************************************************************
// 声明：static void FreeThreadExcaption( CALLSTACK * lpcs )
// 参数：
//		IN lpexp - LPEXCEPTION 结构指针
// 返回值：
//		无
// 功能描述：
//		释放线程的异常结构
// 引用：
//		
// ********************************************************************
#define DEBUG_FreeThreadExcaption 0

static void FreeThreadExcaption( LPEXCEPTION lpexp )
{
	while( lpexp )
	{
		LPEXCEPTION lpNext = lpexp->lpNext;
		KHeap_Free( lpexp, sizeof( EXCEPTION ) );
		lpexp = lpNext;
	}
}


// ********************************************************************
// 声明：static void FreeThreadStack( LPPROCESS lpProcess, LPVOID lpStack, DWORD dwStackSize )
// 参数：
//	IN lpProcess - PROCESS 结构指针
//	IN lpStack - 栈指针
//	IN dwStackSizes - 栈大小
// 返回值：
//	无
// 功能描述：
//	释放线程的栈空间
// 引用：
//		
// ********************************************************************
static void FreeThreadStack( LPPROCESS lpProcess, LPVOID lpStack, DWORD dwStackSize )
{
#ifdef VIRTUAL_MEM
	KC_VirtualFree( lpStack, 0, MEM_RELEASE );
#else
	KL_FreePageMem( lpStack, dwStackSize );
#endif
}

// ********************************************************************
// 声明：LPTHREAD _GetThreadPtr( DWORD id )
// 参数：
//	IN id - 线程ID
// 返回值：
//	线程结构指针
// 功能描述：
//	由线程ID得到线程结构指针
// 引用：
//		
// ********************************************************************
LPTHREAD _GetThreadPtr( DWORD id )
{	
	LPTHREAD lpThread = (LPTHREAD)GET_PTR( id );
	
	if( (LPBYTE)lpThread >= lpbSysMainMem &&
		(LPBYTE)lpThread < lpbSysMainMemEnd &&
		lpThread->dwThreadId == id &&
		lpThread->objType == OBJ_THREAD )
	    return lpThread;
	else
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
	}
	RETAILMSG( 1, ( "error: invalid thread id=0x%x.\r\n", id ) );
	return NULL;
}

// ********************************************************************
// 声明：void AddToProcess( LPTHREAD lpThread )
// 参数：
//		IN lpThread - THREAD 结构指针
// 返回值：
//		无
// 功能描述：
//		将线程加入进程链表
// 引用：
//		
// ********************************************************************
static void AddToProcess( LPTHREAD lpThread )
{
	UINT uiSave;

	//ASSERT( lpThread->lpNextThreadInProcess == NULL && lpThread->lpPrevThreadInProcess == NULL );
	LockIRQSave( &uiSave );
	
	// 连接到进程链表
	if( (lpThread->lpNextThreadInProcess = lpThread->lpOwnerProcess->lpFirstThread) )
	{
		lpThread->lpOwnerProcess->lpFirstThread->lpPrevThreadInProcess = lpThread;
	}
	lpThread->lpOwnerProcess->lpFirstThread = lpThread;
	lpThread->lpPrevThreadInProcess = NULL;
	//
	UnlockIRQRestore( &uiSave );
}

// ********************************************************************
// 声明：void RemoveFromProcess( LPTHREAD lpThread )
// 参数：
//	IN lpThread - THREAD 结构指针
// 返回值：
//	无
// 功能描述：
//	与	AddToProcess 相反，该函数将线程从进程链表里移出
// 引用：
//		
// ********************************************************************
static void RemoveFromProcess( LPTHREAD lpThread )
{
	UINT uiSave;

	LockIRQSave( &uiSave );	//关中断
	// 移出进程链表
	if( lpThread->lpPrevThreadInProcess )
		lpThread->lpPrevThreadInProcess->lpNextThreadInProcess = lpThread->lpNextThreadInProcess;
	else
	{   // 第一个
		lpThread->lpOwnerProcess->lpFirstThread = lpThread->lpNextThreadInProcess;
	}
	if( lpThread->lpNextThreadInProcess )
		lpThread->lpNextThreadInProcess->lpPrevThreadInProcess = lpThread->lpPrevThreadInProcess;
	lpThread->lpNextThreadInProcess = lpThread->lpPrevThreadInProcess = NULL;

	UnlockIRQRestore( &uiSave );
}

// ********************************************************************
// 声明：DWORD WINAPI FirstThreadProc( LPVOID lParam )
// 参数：
//		IN lParam - 由  CreateThread 传递的参数
// 返回值：
//		返回0
// 功能描述：
//		新线程的内核代码部分
// 引用：
//		由该代码调用 CreateThread 传递的函数入口
// ********************************************************************
static DWORD WINAPI FirstThreadProc( LPVOID lParam )
{
	DWORD dwExitCode;

	//EdbgOutputDebugString( "Call ThreadProc:hThread=%x,lParam=%x,ThreadID=%x.\r\n", lpCurThread->hThread, lParam, lpCurThread->dwThreadId );
    // 调用CreateThread传递的函数入口
	if( lpCurThread->lpCurProcess->lpMainThread == lpCurThread )
	    dwExitCode =  ((LPTHREAD_START_ROUTINE)( lpCurThread->lpfnStartAddress ))( lParam );
	else
	{
		dwExitCode = CallUserStartupCode( lpCurThread->lpfnStartAddress, lParam );
	}
	// 退出线程
	KL_ExitThread( dwExitCode );
	return 0;
}

// ********************************************************************
// 声明：static LPTHREAD InitThreadData(
//			   LPTHREAD lpThread,
//			   LPPROCESS lpProcess,   // process
//			   HANDLE  hThread,   // thread handle
//			   DWORD dwStackSize,                      // initial thread stack size, in bytes
//			   DWORD dwPreservStackSize,
//			   LPTHREAD_START_ROUTINE lpStartAddress, // pointer to thread function
//			   LPVOID lpParameter,                // argument for new thread
//			   DWORD dwCreationFlags,
//			   LPDWORD lpdwId
//			   )
// 参数：
//		IN lpThread - THREAD 结构指针
//		IN lpProcess - PROCESS 结构指针
//		IN hThread - 线程句柄
//		IN dwStackSize - 该线程的需要的运行栈大小
//		IN dwPreservStackSize - 在已分配的栈空间里需要保留给系统用的大小, dwPreservStackSize < dwStackSize
//		IN lpStartAddress - 线程启动函数入口
//		IN lpParameter - 传递给线程的参数
//		OUT lpdwId - 用于接受线程ID的指针
// 返回值：
//		
// 功能描述：
//		
// 引用：
//		
// ********************************************************************
// create a thread
#define DEBUG_InitThreadData 0
static LPTHREAD InitThreadData(
			   LPTHREAD lpThread,
			   LPPROCESS lpProcess,   // process
			   HANDLE  hThread,   // thread handle
			   DWORD dwStackSize,                      // initial thread stack size, in bytes
			   DWORD dwPreservStackSize,
			   LPTHREAD_START_ROUTINE lpStartAddress, // pointer to thread function
			   LPVOID lpParameter,                // argument for new thread
			   DWORD dwCreationFlags,
			   LPDWORD lpdwId
			   )
{
	int error = 0;
	LPVOID lpStack;

    DEBUGMSG( DEBUG_InitThreadData, ( "InitThreadData: lpThread(0x%x) entry.\r\n", lpThread ) );
//	memset( lpThread, 0, sizeof( THREAD ) ); //2004-12-21, clear before
    lpThread->akyAccessKey = lpProcess->akyAccessKey | InitKernelProcess.akyAccessKey;
	lpThread->dwMagic = THREAD_MAGIC;

	DEBUGMSG( DEBUG_InitThreadData, ( "InitThreadData: alloc thread stack(0x%x).\r\n", dwStackSize ) );
    lpStack = AllocThreadStack( dwCreationFlags, lpProcess, dwStackSize );//+ TLS_MAX_INDEXS * sizeof( DWORD ) );
    
	if ( !lpStack )
		goto ERROR_RETURN;    
	
	error = ERROR_TRY_AGAIN;
	DEBUGMSG( DEBUG_InitThreadData, ( "InitThreadData: call _SemaphoreCreate.\r\n" ) );
	// 创建信号量	
	lpThread->lpsemExit = _SemaphoreCreate( NULL, 0, 1, NULL, SF_EVENT | SF_MANUALRESET );
	if( lpThread->lpsemExit == NULL )
		goto ERROR_RETURN;
	lpThread->lppsemWait = (LPSEMAPHORE *)KHeap_Alloc( MAX_WAITOBJS_PTR_SIZE );
	if( lpThread->lppsemWait == NULL )
		goto ERROR_RETURN;

//	lpThread->lpCpuPTS = AllocCPUPTS();
	//if( lpThread->lpCpuPTS == NULL )
		//goto ERROR_RETURN;

	DEBUGMSG( DEBUG_InitThreadData, ( "InitThreadData: init thread struct.\r\n" ) );
	// 初始化线程结构
	lpThread->objType = OBJ_THREAD;
	if( dwCreationFlags & CREATE_SUSPENDED )
	{
		lpThread->nSuspendCount = 1;
		lpThread->dwState = THREAD_SUSPENDED;
	}
	else
        lpThread->dwState = THREAD_UNINTERRUPTIBLE;
	lpThread->nCurPriority = lpThread->nOriginPriority = DEF_PRIORITY;
	lpThread->nTickCount = DEF_ROTATE;//lpCurThread->nPriority >> 1;
	lpThread->fPolicy = THREAD_POLICY_OTHER;
	lpThread->nRotate = DEF_ROTATE;//15;//DEF_ROTATE;// 
	// 由线程指针得到线程 ID
    *lpdwId = lpThread->dwThreadId = (DWORD)MAKE_HANDLE( lpThread );
	// 
	lpThread->dwThreadStackSize = dwStackSize;
	lpThread->lpdwThreadStack = (LPDWORD)lpStack;
	// 预留 Thread local slots 的空间并初始化
	// stack space format:
	// low address                                  high address
	// lpStack                                   
	// [user space uuuuuuuuuuuuuuuuuuuuuuuuuuu][TLS][cmdline]
	// 2004-12-30 TLS 不从 stack 中分配
	//dwStackSize -= TLS_MAX_INDEXS * sizeof( DWORD ) + ( ( dwPreservStackSize + sizeof(int) - 1 ) & ( ~( sizeof(int) - 1 ) ) );
	dwStackSize -= ( ( dwPreservStackSize + sizeof(int) - 1 ) & ( ~( sizeof(int) - 1 ) ) );
	// 2004-12-30 
    lpThread->lpdwTLS = (LPDWORD)KHeap_Alloc( TLS_ALLOC_SIZE );//( (LPBYTE)lpStack + dwStackSize );
	if( lpThread->lpdwTLS )
	    memset( lpThread->lpdwTLS, 0, TLS_ALLOC_SIZE );
	else
		goto ERROR_RETURN;
	//  user sp space start
	lpThread->lpdwThreadUserStack = (LPDWORD)( (LPBYTE)lpStack + dwStackSize - sizeof( UINT ) * 2 );	
	
	lpThread->lpCurProcess = lpThread->lpOwnerProcess = lpProcess;//(LPPROCESS)HandleToPtr(hProcess);
	lpThread->hThread = hThread;
	lpThread->lpfnStartAddress = (LPVOID)lpStartAddress;
	
	DEBUGMSG( DEBUG_InitThreadData, ( "InitThreadData: call _LinkThread.\r\n" ) );
	// 将线程连接进入线程链表	
	_LinkThread( lpThread );
	// 将线程加入进程链表并增加进程的线程数
	AddToProcess( lpThread );
	KL_InterlockedIncrement( (LPLONG)&lpThread->lpOwnerProcess->dwThreadCount );

	DEBUGMSG( DEBUG_InitThreadData, ( "InitThreadData: call InitThreadTSS.\r\n" ) );

#ifdef USE_THREAD_PAGE_TABLE
	{
		extern VOID InitThreadPageTable( LPTHREAD lpThread );
		InitThreadPageTable( lpThread );
	}
#endif
	InitThreadTSS( lpThread, FirstThreadProc, (LPBYTE)lpThread->lpdwThreadUserStack, lpParameter );
	
	return lpThread;
	
ERROR_RETURN:
	// 错误处理
	RETAILMSG( 1, ( "error at InitThreadData: failue.\r\n" ) );
	if( lpThread->lpdwTLS )
	{
		KHeap_Free( lpThread->lpdwTLS,TLS_ALLOC_SIZE );
	}
	if( lpStack )
	{
		FreeThreadStack( lpProcess, lpStack, lpThread->dwThreadStackSize );
	}
	if( lpThread->lpsemExit )
	{
		_CloseSemaphoreObj( lpThread->lpsemExit, 0 );//_SemaphoreDelete( lpThread->lpsemExit );
	}
	if( lpThread->lppsemWait )
	{
		KHeap_Free( lpThread->lppsemWait, MAX_WAITOBJS_PTR_SIZE );
	}

//	if( lpThread->lpCpuPTS == NULL )
//	{
//		FreeCPUPTS( lpThread->lpCpuPTS );
//	}
    if( lpThread->lpCallStack )
		KHeap_Free( lpThread->lpCallStack, sizeof( CALLSTACK ) ); 
	return 0;
}

// ********************************************************************
// 声明：void ThreadSelfExit( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		当线程不是当前线程时， 用调用 MakeThreadToDie 将该线程的IP设为ThreadSelfExit
//		使需要被杀死的线程成为当前线程
// 引用：
//		内部使用
// ********************************************************************
#define DEBUG_ThreadSelfExit 0
static void ThreadSelfExit( void )
{
	KL_ExitThread( lpCurThread->dwExitCode );
	ERRORMSG( DEBUG_ThreadSelfExit, ( "ThreadSelfExit: error.\r\n" ) );	
}


// ********************************************************************
// 声明：static void MakeThreadToDie( LPTHREAD lpThread, DWORD dwExitCode )
// 参数：
//		IN lpThread - 线程结构指针
//		IN dwExitCode - 线程退出代码
// 返回值：
//		无
// 功能描述：
//		处理线程的退出
// 引用：
//		
// ********************************************************************
#define DEBUG_MAKETHREADTODIE 0
static void MakeThreadToDie( LPTHREAD lpThread, DWORD dwExitCode )
{
	DWORD dwState;
	UINT uiSave;

	ASSERT( lpThread != lpCurThread );
	DEBUGMSG( DEBUG_MAKETHREADTODIE, ( "MakeThreadToDie: idThread(0x%x),state(0x%x).\r\n", lpThread->dwThreadId, lpThread->dwState ) );

    LockIRQSave( &uiSave );

    dwState = lpThread->dwState;
    lpThread->dwExitCode = dwExitCode;
    lpThread->nSuspendCount = 0;
	switch( dwState )
	{
	case THREAD_INTERRUPTIBLE:
	case THREAD_UNINTERRUPTIBLE:		
	case THREAD_SUSPENDED:
	    
		SetThreadIP( lpThread, (DWORD)ThreadSelfExit );

		lpThread->dwState = THREAD_RUNNING;		
		AddToRunQueue( lpThread );

		UnlockIRQRestore( &uiSave );				
		
		break;
	case THREAD_RUNNING:
		// 应该判断线程是否在内核态
		/*
		if( lpThread->lpCallStack && 
			( lpThread->lpCallStack->dwCallInfo & CALL_KERNEL ) )
		{	//在内核态
			lpThread->dwSignal |= SIGNAL_EXIT;
			bNeedResched = TRUE;
		}
		else
		{	//非内核态
			SetThreadIP( lpThread, (DWORD)ThreadSelfExit );
		}
		*/
		SetThreadIP( lpThread, (DWORD)ThreadSelfExit );
		UnlockIRQRestore( &uiSave );
		//
		break;
	default:
		ERRORMSG( DEBUG_MAKETHREADTODIE, ("error in MakeThreadToDie: undef thread state(0x%x).\r\n", dwState ) );
		break;
	}	
}


// ********************************************************************
// 声明：void _DoThreadExit( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		处理线程退出
// 引用：
//		内部
// ********************************************************************
#define DEBUG_DoThreadExit 0

static void _DoThreadExit( void )
{
	LPVOID lpvTemp;
	DEBUGMSG( DEBUG_DoThreadExit|1, ( "_DoThreadExit pThread(0x%x), idThread(0x%x).\r\n", lpCurThread, lpCurThread->dwThreadId ) );
	// 向所有服务广播线程退出事件
	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:DO SCC_BROADCAST_THREAD_EXIT.\r\n" ) );	
	EnumServerHandler( SCC_BROADCAST_THREAD_EXIT, lpCurThread->dwThreadId, 0 );//( ThreadExitServerNotify, lpCurThread->dwThreadId );
	if( lpCurThread->lpOwnerProcess->lpFirstThread->lpNextThreadInProcess == NULL )
	{  // 这是唯一的线程，需要做所有的清除工作
		LPPROCESS lpProcess = lpCurThread->lpOwnerProcess;
		//RETAILMSG( 1, ( "KL_ClearClass.\r\n" ) );
		//KL_ClearClass( lpProcess->hProcess );// LN, 2003-05-22
		//RETAILMSG( 1, ( "KL_CloseAllTable.\r\n" ) );
		//KL_CloseAllTable( lpProcess->hProcess );// LN, 2003-05-22
		//RETAILMSG( 1, ( "KL_CloseSysObject.\r\n" ) );
		//KL_CloseSysObject( lpProcess->hProcess );//LN, 2003-05-22
		//RETAILMSG( 1, ( "KL_CloseHandle.\r\n" ) );
		//EnumServer( ProcessExitServerNotify, lpProcess->hProcess );
		ASSERT( lpCurThread->lpOwnerProcess == lpCurThread->lpCurProcess );

		// 向所有服务广播进程退出事件
    	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:DO SCC_BROADCAST_PROCESS_EXIT.\r\n" ) );			
		EnumServerHandler( SCC_BROADCAST_PROCESS_EXIT, (WPARAM)lpProcess->hProcess, 0 );
		
		// 释放线程句柄
		if(lpProcess->lpMainThread->hThread)
		    KL_CloseHandle( lpProcess->lpMainThread->hThread );
		DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:Module_DeInit.\r\n" ) );
		// 释放模块
		
		Module_DeInit( lpProcess->pModule );

		lpProcess->pModule = NULL;// LN 2003-05-21, add
		
    	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:KL_HeapDestroy.\r\n" ) );
    	
		// 释放进程堆
    	{
    		LPHEAP lpHeap = (LPHEAP)KL_InterlockedExchange( (LPLONG)&lpProcess->lpHeap, 0 );
//    		if( lpHeap )
//				KL_HeapDestroy( lpHeap );
//			lpProcess->lpHeap = NULL;// LN 2003-05-21, add
    		if( lpHeap )
    		{
			#ifdef VIRTUAL_MEM    		
   	 			DoHeapDestroy( lpHeap );
			#else    		
				KL_HeapDestroy( lpHeap );
			#endif
    		}
    	}

		 
		DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:Handle_CloseAll.\r\n" ) );
		// 释放所有的句柄
		Handle_CloseAll( lpProcess );
		lpCurThread->hThread = NULL;
		
		lpProcess->lpszCommandLine = NULL;// LN 2003-05-21, add
#ifdef VIRTUAL_MEM
		ASSERT( lpProcess->lpProcessSegments->lpSeg->lpBlks[0] == NULL );
#endif
	}
	if( lpCurThread->hThread )
	{
		SetHandleObjPtr( lpCurThread->hThread, NULL );
	    SetHandleUserInfo( lpCurThread->hThread, lpCurThread->dwExitCode ); // LN, 2002-05-22
		lpCurThread->hThread = NULL; 
	}

	//释放拥有的CS MUTEX
	_Semaphore_HandleThreadExit( lpCurThread );
	// 假如可能，释放 timer
	//if( lpCurThread->lpTimer )
	if( lpCurThread->timer.lpNext )
	{
		DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:_DelTimerList.\r\n" ) );
		_DelTimerList(&lpCurThread->timer);
		//lpCurThread->lpTimer = NULL;
	}
	// 假如可能，释放信号量
	//if( lpCurThread->lppsemWait )//2005-01-29, remove
	{
		int nCount = lpCurThread->nWaitCount;
		LPSEMAPHORE  * lppsemWait = lpCurThread->lppsemWait;
		WAITQUEUE * lpWaitQueue = lpCurThread->lpWaitQueue;
		int i;
		if( nCount )
		{
			DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:__RemoveWaitQueue.\r\n" ) );
			for( i = 0; i < nCount; i++, lppsemWait++, lpWaitQueue++ )
			{
				__RemoveWaitQueue( &(*lppsemWait)->lpWaitQueue, lpWaitQueue );
				ReleaseSemaphoreAndUnlock( *lppsemWait ); //2004-08-11
			}			
			KHeap_Free( lpCurThread->lpWaitQueue, nCount * sizeof( WAITQUEUE ) );
		}
		//2005-01-29, add
		//2004-08-11, 不是用动态分配

		lpCurThread->nWaitCount = 0;
		lpCurThread->lpWaitQueue = NULL;
	}
	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:_SemaphoreRelease.\r\n" ) );
	// 释放线程信号量
	_SemaphoreRelease( lpCurThread->lpsemExit, 1, NULL );

	lpvTemp = (LPVOID)KL_InterlockedExchange( (LPLONG)&lpCurThread->lpsemExit, 0 );
	_CloseSemaphoreObj( (LPSEMAPHORE)lpvTemp, 0 );
	
	// 移出线程信号量
	//_SemaphoreRemove( lpCurThread->lpsemExit );

	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:FreeThreadCallStack.\r\n" ) );
	//释放异常结构
	lpvTemp = (LPVOID)lpCurThread->lpException;
	lpCurThread->lpException = NULL;
    FreeThreadExcaption( (LPEXCEPTION)lpvTemp );	

	// 释放呼叫栈
	lpvTemp = (LPVOID)lpCurThread->lpCallStack;
	lpCurThread->lpCallStack = NULL;
    FreeThreadCallStack( (LPCALLSTACK)lpvTemp );

	{
		LPVOID lpStack;

		if( lpCurThread->lpdwThreadKernelStack == NULL )
		{
			while(1)
			{
				lpStack = KHeap_Alloc( PAGE_SIZE );
				if( lpStack )
					break;
				WARNMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:no enough memory!.\r\n" ) );
				Sleep(10);
			}
			lpCurThread->lpdwThreadKernelStack = lpStack;
		}
		// 
     	INTR_OFF();
	    // 从进程移出
    	lpCurThread->dwThreadId = 0; // 阻止再使用该thread
        RemoveFromProcess( lpCurThread );
		
    	lpCurThread->lpCurProcess = &InitKernelProcess;
		
		lppProcessSegmentSlots[0] = NULL;
		//切换到内核栈
		//lpCurThread->lpdwThreadUserStack = lpStack;
		//lpCurThread->lpdwThreadKernelStack = lpStack;
	    SwitchToStackSpace( (LPVOID)( (DWORD)lpStack + PAGE_SIZE - sizeof(long) ) );
		INTR_ON();
	}
    DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:_FreeThreadResourceAndReschedule.\r\n" ) );
    _FreeThreadResourceAndReschedule();
}

// ********************************************************************
// 声明：void HandleThreadExit( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		处理线程退出
// 引用：
//		该模块 和 process.c 也会调用该函数 
// ********************************************************************
#define DEBUG_HANDLETHREADEXIT 0
void HandleThreadExit( void )
{
	//因为永远不应该返回，所以我切换到新的sp,这样有以下考滤：
	//1。该thread 永远不会返回
	//2。因为exit thread 时会调用 server的 相关清除函数，有可能会用到许多stack
	//
	SwitchToStackSpace( lpCurThread->lpdwThreadUserStack );
	_DoThreadExit();
//	ASSERT(0); //永不返回 
}


// ********************************************************************
// 声明：void WINAPI KL_ExitThread( DWORD dwExitCode )
// 参数：
//		IN dwExitCode - 退出代码
// 返回值：
//		无
// 功能描述：
//		退出当前线程
// 引用：
//		系统API		
// ********************************************************************
void WINAPI KL_ExitThread( DWORD dwExitCode )
{			
	//if( GetAPICallerProcessPtr()->lpMainThread == lpCurThread )
	
	if( lpCurThread->lpOwnerProcess != lpCurThread->lpCurProcess )
	{
		SwitchToProcess( lpCurThread->lpOwnerProcess, NULL );
	}
	ASSERT( lpCurThread->lpOwnerProcess == lpCurThread->lpCurProcess );

	if( lpCurThread->lpCurProcess->lpMainThread == lpCurThread )
	{ // 主线程 main thread
		EdbgOutputDebugString( "Main Thread Exit. now, call ExitProcess.\r\n" );
		//KL_TerminateProcess( lpCurThread->lpOwnerProcess->hProcess, dwExitCode );
		KL_ExitProcess( dwExitCode );
	}
	else
	{
		//非主线程
		lpCurThread->dwExitCode = dwExitCode;
		HandleThreadExit();
	}
//	ASSERT( 0 );	
}

// ********************************************************************
// 声明：BOOL WINAPI KL_TerminateThread( HANDLE hThread, DWORD dwExitCode )
// 参数：
//		IN hThread - 线程句柄
//		IN dwExitCode - 退出代码
// 返回值：
//		成功，返回TRUE；失败，返回FALSE
// 功能描述：
//		非正常终止一个线程
// 引用：
//		系统API
// ********************************************************************
#define DEBUG_TerminateThread 0
BOOL WINAPI KL_TerminateThread( HANDLE hThread, DWORD dwExitCode )
{
	LPTHREAD lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD ); // 由句柄得到线程对象指针

	if( lpThread )
	{	
		if( lpThread == lpCurThread )
			KL_ExitThread( dwExitCode );
		else
		{
			WARNMSG( DEBUG_TerminateThread, ( "KL_TerminateThread: kill other thread.\r\n" ) );
            MakeThreadToDie( lpThread, dwExitCode );
		}
		return TRUE;
	}
	return FALSE;
}

// ********************************************************************
// 声明：DWORD WINAPI KL_GetCurrentThreadId( void )
// 参数：
//		无
// 返回值：
//		返回当前线程的ID
// 功能描述：
//		得到当前线程的ID
// 引用：
//		系统API
// ********************************************************************
DWORD WINAPI KL_GetCurrentThreadId( void )
{
    return lpCurThread->dwThreadId;
}

// ********************************************************************
// 声明：HANDLE WINAPI KL_GetCurrentThread( void )
// 参数：
//		无
// 返回值：
//		当前线程的句柄
// 功能描述：
//		得到当前线程的句柄，该句柄是当前线程的别名而不是一个真实的句柄
// 引用：
//		系统API
// ********************************************************************
HANDLE WINAPI KL_GetCurrentThread( void )
{
	return CURRENT_THREAD_HANDLE; 
}


// ********************************************************************
// 声明：static BOOL _DeleteThreadObjectPtr( LPTHREAD lpThread )
// 参数：
//		IN lpThread - THREAD 结构指针
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		删除线程对象
// 引用：
//		
// ********************************************************************
#define DEBUG_DELETETHREADOBJECT 0
static BOOL _DeleteThreadObjectPtr( LPTHREAD lpThread )
{
	ASSERT( lpThread->objType == OBJ_THREAD && lpThread->dwState == THREAD_ZOMBIE );
	//ASSERT( lpThread->dwState == THREAD_ZOMBIE );
	// 释放线程结构
	_RemoveThread( lpThread );  // 从线程链表移出
	RemoveFromProcess( lpThread );
	lpThread->objType = OBJ_NULL;

		// 释放信号量
	if( lpThread->lpsemExit )
	{
		ASSERT(0);
	    //_SemaphoreDeletePtr( lpThread->lpsemExit );
	    lpThread->lpsemExit = NULL;
	}
    // lpThread->lpdwThreadUserStack 在 HandleThreadExit 中被重新付值
	// 其大小为 PAGE_SIZE
	//KHeap_Free( lpThread->lpdwThreadUserStack, PAGE_SIZE );
	if( lpThread->lpdwThreadKernelStack )
		KHeap_Free( lpThread->lpdwThreadKernelStack, PAGE_SIZE );
	// 2004-12-21, ln
	//KHeap_Free( lpThread, sizeof( THREAD ) );
	FreeThreadStruct( lpThread );
	//

	return TRUE;
}

// ********************************************************************
// 声明：static BOOL _DeleteThreadObject( LPTHREAD lpThread )
// 参数：
//		IN lpThread - THREAD 结构指针
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		删除线程对象
// 引用：
//		
// ********************************************************************
#define DEBUG_DELETETHREADOBJECT 0
static BOOL _FreeThreadResourceAndReschedule( void )
{
	LPTHREAD lpThread = lpCurThread;
	ASSERT( lpThread->objType == OBJ_THREAD && lpThread->uiRefCount == 0 );
	//ASSERT( lpThread->uiRefCount == 0 );

	//if( lpThread->dwState == THREAD_ZOMBIE )
	{
		LPPROCESS lpProcess;
		LPDWORD lpdwTLS;

		if( lpThread->hThread )
			SetHandleObjPtr( lpThread->hThread, NULL );  // 将句柄对应的对象指针置为NULL

		lpdwTLS = (LPDWORD)KL_InterlockedExchange( (LPLONG)&lpThread->lpdwTLS, 0 );
		
		KHeap_Free( lpdwTLS,TLS_ALLOC_SIZE );


		//_RemoveThread( lpThread );  // 从线程链表移出 2004.03-03 , remove to DeleteThreadObjectPtr
		//lpThread->objType = OBJ_NULL;
		//释放线程栈空间
		FreeThreadStack( lpThread->lpOwnerProcess, (void*)lpThread->lpdwThreadStack, lpThread->dwThreadStackSize );
		lpThread->lpdwThreadStack = NULL;
		
		// test
		DEBUGMSG( DEBUG_DELETETHREADOBJECT, ( "D:MainThread CallBack(0x%x).\r\n", lpThread->lpOwnerProcess->lpMainThread->lpCallStack ) );
		//
		
		if( lpThread->lpOwnerProcess->dwThreadCount == 1 )
		{   // 这是进程的最后一个线程，线程数据在 下面的 FreeProcessObject中去释放
			DEBUGMSG( DEBUG_DELETETHREADOBJECT, ( "Free main thread.\r\n" ) );
			lpProcess = lpThread->lpOwnerProcess;
			lpProcess->lpMainThread = NULL;
//			ASSERT( lpProcess );
		}
		else
		{	// 减去引用计数
			KL_InterlockedDecrement( (LPLONG)&lpThread->lpOwnerProcess->dwThreadCount );
			DEBUGMSG( DEBUG_DELETETHREADOBJECT, ( "Free not main thread.\r\n" ) );
			lpProcess = NULL;
		}
	
		if( lpProcess )
		{   // 释放进程对象
			extern void FreeProcessObject( LPPROCESS lpProcess );
			FreeProcessObject( lpProcess );
		}
	}
	//else
	//{
      //  DEBUGMSG( DEBUG_DELETETHREADOBJECT, ( "error at _DeleteThreadObject: thread is not zombie!.\r\n" ) );
		//return FALSE;
	//}
	if( lpCurThread->lppsemWait )
	{
		KHeap_Free( lpCurThread->lppsemWait, MAX_WAITOBJS_PTR_SIZE );
		lpCurThread->lppsemWait = NULL;//lpsem;
	}

	INTR_OFF();
    lpInitKernelThread->dwSignal |= SIGNAL_CLEAR;//
	lpInitKernelThread->nTickCount=1;//DEF_ROTATE;//254;
	lpCurThread->dwState = THREAD_ZOMBIE;
	//将该线程加入到内核进程
	lpCurThread->lpOwnerProcess = &InitKernelProcess;
	AddToProcess( lpCurThread );

	INTR_ON();

ERROR_LOOP:

	Schedule(); 
    lpCurThread->dwState = THREAD_ZOMBIE;
    ERRORMSG( DEBUG_DELETETHREADOBJECT, ( "error loop.\r\n" ) );

	goto ERROR_LOOP;

	return TRUE;
}

// ********************************************************************
// 声明：DWORD DoSuspendThread( LPTHREAD lpThread )
// 参数：
//		IN  lpThread - THREAD 结构指针
// 返回值：
//		假如成功，返回之前的计数值。假如错误，返回-1
// 功能描述：
//		挂起线程
// 引用：
//		
// ********************************************************************
DWORD DoSuspendThread( LPTHREAD lpThread )
{
	DWORD dwOldV;
	UINT uiSave;

	LockIRQSave( &uiSave );
	if( (dwOldV = lpThread->nSuspendCount) == MAXIMUM_SUSPEND_COUNT )  // 是否达到最大计数
	{
		dwOldV = -1;
		goto _ret;
	}
	if( ++lpThread->nSuspendCount == 1 )  // 是否第一次挂起 ？
	{  // 是
		if( lpThread == lpCurThread )  // 当前线程 ？ 
		{  // 是
			lpThread->dwState = THREAD_SUSPENDED;
			UnlockIRQRestore( &uiSave );
			Schedule();
			return dwOldV;
		}
		else if( lpThread->dwState == THREAD_RUNNING )
		{   // 非当前线程，并且当前线程等待运行			
			RemoveFromRunQueue( lpThread );		
			lpThread->dwState = THREAD_SUSPENDED;
		}
	}
_ret:
    UnlockIRQRestore( &uiSave );
    return dwOldV;
}

// ********************************************************************
// 声明：DWORD DoResumeThread( LPTHREAD lpThread )
// 参数：
//		IN  lpThread - THREAD 结构指针  
// 返回值：
//		假如成功，返回之前的计数值。假如错误，返回-1
// 功能描述：
//		与DoSuspendThread对应，该功能重设/恢复线程
// 引用：
//		
// ********************************************************************
DWORD DoResumeThread( LPTHREAD lpThread )
{
	DWORD dwOldV;
	UINT uiSave;

	//ASSERT( lpThread != lpCurThread );

	LockIRQSave( &uiSave );

    if( (dwOldV = lpThread->nSuspendCount) )
		lpThread->nSuspendCount--;
	if( lpThread->dwState == THREAD_SUSPENDED &&
		lpThread->nSuspendCount == 0 )
	{
	    lpThread->dwState = THREAD_RUNNING;
		UnlockIRQRestore( &uiSave );
	    AddToRunQueue( lpThread );
	}
	else
	    UnlockIRQRestore( &uiSave );
	return dwOldV;
}

// ********************************************************************
// 声明：void KillAllOtherThreads( LPPROCESS lpProcess )
// 参数：
//		IN  lpProcess - PROCESS 结构指针
// 返回值：
//		无
// 功能描述：
//		杀死属于该进程的所有线程
// 引用：
//		process.c's ExitProcess
// ********************************************************************
// the code called by mainthread or exitprocess
#define DEBUG_KillAllOtherThreads 1
void KillAllOtherThreads( LPPROCESS lpProcess )
{	
	LPTHREAD lpThread;
	UINT uiSave;

	LockIRQSave( &uiSave );

	lpThread = lpProcess->lpFirstThread;

	while( lpThread )
	{
		if( lpThread->lpCurProcess == lpProcess &&
			( lpThread->flag & FLAG_KILLING ) == 0 &&
			lpThread != lpCurThread )
		{
			lpThread->flag |= FLAG_KILLING;
			UnlockIRQRestore( &uiSave );
			MakeThreadToDie( lpThread, -1 );
			LockIRQSave( &uiSave );
			lpThread = lpProcess->lpFirstThread; // 从第一个重新开始
		}
		else
		{	//注意！！，这里因为是关中断状态，
			//输出调试信息将会需要许多时间，影响到别的设备丢失数据
			//因此，调试完该功能后，应该将该输出关掉！！
			//DEBUGMSG( DEBUG_KillAllOtherThreads, ( "KillAllOtherThreads:dwState=0x%x,lpThread->lpCurProcess=0x%x,lpProcess=0x%x,lpThread->flag=0x%x,lpCurThread=0x%x.\r\n", lpThread->dwState, lpThread->lpCurProcess,lpProcess,lpThread->flag,lpCurThread ) );
			lpThread = lpThread->lpNextThreadInProcess;
		}
	}

	UnlockIRQRestore( &uiSave );
}

// ********************************************************************
// 声明：void HandleSignal( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		处理信号
// 引用：
//		
// ********************************************************************
#define DEBUG_HANDLESIGNAL 0
void HandleSignal( void )
{
	LPTHREAD lpThread;
	UINT uiSave;

	DEBUGMSG( DEBUG_HANDLESIGNAL, ( "HandleSignal++:dwSignal(0x%x),dwBlocked(0x%x).\r\n", lpCurThread->dwSignal, lpCurThread->dwBlocked ) );

	LockIRQSave( &uiSave );

	if( lpCurThread->dwSignal & SIGNAL_CLEAR )
	{	// 有无用的线程对象需要清除
		lpCurThread->dwBlocked |= SIGNAL_CLEAR;  // disable SIGNAL_CHILD
		lpCurThread->dwSignal &= ~SIGNAL_CLEAR;

		// 查找有信号的线程并处理信号
		lpThread = InitKernelProcess.lpFirstThread;
		while( lpThread )
		{
			if( lpThread->dwState == THREAD_ZOMBIE )
			{
				UnlockIRQRestore( &uiSave );

				DEBUGMSG( DEBUG_HANDLESIGNAL, ( "Thread(%x) Zombie.\r\n", lpThread ) );
				
				_DeleteThreadObjectPtr( lpThread );
				LockIRQSave( &uiSave );
				//重新开始
				lpThread = InitKernelProcess.lpFirstThread;
				continue;
			}
			//下一个
			lpThread = lpThread->lpNextThreadInProcess;
		}
		lpCurThread->dwBlocked &= ~SIGNAL_CLEAR;  // enable SIGNAL_CHILD
	}
	else if( lpCurThread->dwSignal & SIGNAL_EXIT )
	{
		lpCurThread->dwBlocked |= SIGNAL_EXIT;  // disable SIGNAL_CHILD
		lpCurThread->dwSignal &= ~SIGNAL_EXIT;
		UnlockIRQRestore( &uiSave );
		KL_ExitThread(-1);
		return;
	}	
	else
	{
		WARNMSG( DEBUG_HANDLESIGNAL, ( "HandleSignal: SIGNAL_CHILD masked!.\r\n" ) );
	}

	UnlockIRQRestore( &uiSave );

	DEBUGMSG( DEBUG_HANDLESIGNAL, ( "HandleSignal--.\r\n" ) );
}

// ********************************************************************
// 声明：static LPTHREAD AllocThreadStruct( VOID )
// 参数：
//	无
// 返回值：
//		假如成功，返回线程结构指针；否则，返回NULL
// 功能描述：
//		分配线程结构
// 引用：
//		
// ********************************************************************

static LPTHREAD AllocThreadStruct( VOID )
{
	LPTHREAD lpThread;
	lpThread = (LPTHREAD)KHeap_Alloc( THREAD_STRUCT_SIZE ); // 分配线程结构
	if( lpThread )
	{
		memset( lpThread, 0, THREAD_STRUCT_SIZE );
	}
	return lpThread;
}

// ********************************************************************
// 声明：static LPTHREAD AllocThreadStruct( VOID )
// 参数：
//	无
// 返回值：
//		假如成功，返回线程结构指针；否则，返回NULL
// 功能描述：
//		分配线程结构
// 引用：
//		
// ********************************************************************
static VOID FreeThreadStruct( LPTHREAD lpThread )
{
	KHeap_Free( lpThread, THREAD_STRUCT_SIZE );
}

// ********************************************************************
// 声明：LPTHREAD FASTCALL _CreateThread(
//								LPPROCESS lpProcess,
//								LPSECURITY_ATTRIBUTES lpThreadAttributes,   // pointer to thread security attributes
//								DWORD dwStackSize,                      // initial thread stack size, in bytes
//								DWORD dwPreservStackSize,
//								LPTHREAD_START_ROUTINE lpStartAddress, // pointer to thread function
//								LPVOID lpParameter,                // argument for new thread
//								DWORD dwCreationFlags,         // creation flags
//								LPDWORD lpThreadId         // pointer to returned thread identifier
//								)

// 参数：
//		IN lpProcess - PROCESS 结构指针
//		IN lpThreadAttributes - 安全属性
//		IN dwStackSize - 该线程的需要的运行栈大小
//		IN dwPreservStackSize - 在已分配的栈空间里需要保留给系统用的大小, dwPreservStackSize < dwStackSize
//		IN lpStartAddress - 线程启动函数入口
//		IN lpParameter - 传递给线程的参数
//		IN dwCreationFlags - 创建标志
//		OUT lpThreadId - 用于接受线程ID的指针

// 返回值：
//		假如成功，返回线程结构指针；否则，返回NULL
// 功能描述：
//		创建线程
// 引用：
//		
// ********************************************************************

#define DEBUG_CREATETHREAD 0
LPTHREAD FASTCALL _CreateThread(
								LPPROCESS lpProcess,
								LPSECURITY_ATTRIBUTES lpThreadAttributes,   // pointer to thread security attributes
								DWORD dwStackSize,                      // initial thread stack size, in bytes
								DWORD dwPreservStackSize,
								LPTHREAD_START_ROUTINE lpStartAddress, // pointer to thread function
								LPVOID lpParameter,                // argument for new thread
								DWORD dwCreationFlags,         // creation flags
								LPDWORD lpThreadId         // pointer to returned thread identifier
								)
{
	DWORD dwThreadID;
	LPTHREAD lpThread = NULL;
	HANDLE hThread = NULL;

    if( lpProcess )
    {
#ifdef VIRTUAL_MEM
		if( dwStackSize == 0 )
			dwStackSize = 64 * 1024;  //必须用64k， 因为主线程的stack在64k-128k
#else
	#ifdef EML_WIN32
		if( dwStackSize == 0 )  //在Window下模拟
			dwStackSize = 128 * 1024;
	#else
		if( dwStackSize == 0 )
			dwStackSize = 16 * 1024;
	#endif

#endif
		dwThreadID = 0;
		DEBUGMSG( DEBUG_CREATETHREAD, ( "_CreateThread:alloc thread struct.\r\n" ) );
		// 2004-12-21, remove by lilin
		//lpThread = (LPTHREAD)KHeap_Alloc( sizeof( THREAD ) ); // 分配线程结构
		lpThread = AllocThreadStruct();
		//

		
		if( lpThread )
		{
			DEBUGMSG( DEBUG_CREATETHREAD, ( "_CreateThread: alloc thread handle.\r\n" ) );
			hThread = Handle_Alloc( lpProcess, lpThread, OBJ_THREAD ); // 分配句柄
			//DEBUGMSG( DEBUG_CREATETHREAD, ( "_C5.\r\n" ) );
			if( hThread )
			{
				DEBUGMSG( DEBUG_CREATETHREAD, ( "_CreateThread: call InitThreadData.\r\n" ) );
				// 初始化线程结构数据
				if( InitThreadData( 
					lpThread, 
					lpProcess, 
					hThread,
					dwStackSize, 
					dwPreservStackSize,
					lpStartAddress, 
					lpParameter,
					dwCreationFlags | CREATE_SUSPENDED, // 先挂起
					&dwThreadID	 ) )
				{
					if( lpThreadId )
						*lpThreadId = dwThreadID;
					lpThread->uiRefCount = 1;  // add referent count
					DEBUGMSG( DEBUG_CREATETHREAD, ( "_CreateThread: success, handle=0x%x,id=0x%x,proc=0x%s.\r\n", hThread, dwThreadID, lpProcess->lpszApplicationName ) );
					if( (dwCreationFlags&CREATE_SUSPENDED) == 0 ) // 是否先挂起
						//_WakeupThread( lpThread, FALSE ); // 否，将线程加入运行队列
						DoResumeThread( lpThread );// 否，将线程加入运行队列
				}
				else
				{   // 初始化失败
					RETAILMSG( 1, ( "error : _CreateThread:call InitThreadData failure!.\r\n" ) );
					Handle_Free( hThread, TRUE );
					//2004-12-21, remove by lilin
					//KHeap_Free( lpThread, sizeof( THREAD ) );//_kfree( lpThread );
					FreeThreadStruct( lpThread );
					//
					lpThread = NULL;
				}
			}
			else
			{   // 分配句柄失败
				RETAILMSG( 1, ( "error : _CreateThread:not alloc handle!.\r\n" ) );
				// 2004-12-21, remove by lilin
				//KHeap_Free( lpThread, sizeof( THREAD ) );
				FreeThreadStruct( lpThread );
				//
				lpThread = NULL;
			}
		}
		else
		{   //
			RETAILMSG( 1, ( "error : _CreateThread:no enough memory!.\r\n" ) );
		}
    }
    return lpThread;
}

// ********************************************************************
// 声明：HANDLE WINAPI KL_CreateThread(
//					  LPSECURITY_ATTRIBUTES lpThreadAttributes,   //
//					  DWORD dwStackSize,                      //
//					  LPTHREAD_START_ROUTINE lpStartAddress, //
//					  LPVOID lpParameter,                //
//					  DWORD dwCreationFlags,         //
//					  LPDWORD lpThreadId         //
//					  )

// 参数：
//		IN lpThreadAttributes - 安全属性
//		IN dwStackSize - 该线程的需要的运行栈大小
//		IN lpStartAddress - 线程启动函数入口
//		IN lpParameter - 传递给线程的参数
//		IN dwCreationFlags - 创建标志,可以为:
//					CREATE_SUSPENDED - 创建后挂起，直到调用ResumeThread
//		OUT lpThreadId - 用于接受线程ID的指针
// 返回值：
//		假如成功，返回线程句柄；否则，返回NULL	
// 功能描述：
//		创建新的线程
// 引用：
//		系统API	
// ********************************************************************
HANDLE WINAPI KL_CreateThread(
					  LPSECURITY_ATTRIBUTES lpThreadAttributes,   //
					  DWORD dwStackSize,                      //
					  LPTHREAD_START_ROUTINE lpStartAddress, //
					  LPVOID lpParameter,                //
					  DWORD dwCreationFlags,         //
					  LPDWORD lpThreadId         //
					  )
{
	LPTHREAD lpThread;
	lpThread = _CreateThread( GetAPICallerProcessPtr(), lpThreadAttributes, dwStackSize, 0, lpStartAddress, lpParameter, dwCreationFlags & ~CREATE_MAINTHREAD, lpThreadId );
	if( lpThread )
		return lpThread->hThread;
	return NULL;
}

// ********************************************************************
// 声明：BOOL WINAPI KL_SetThreadPriority( HANDLE hThread, long lPriority, UINT flag )
// 参数：
//		IN hThread - 线程句柄
//		IN lPriority - 线程优先级, 当 bPriorityValue 为FALSE时 为-15 ~ 15,这时指相对值，否则lPriority指0 ~ 255的绝对值
//		IN flag - 指lPriority代表的是相对值或是绝对值
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		设置线程优先级
// 引用：
//		系统API
// ********************************************************************
#define DEBUG_SetThreadPriority 0
BOOL WINAPI KL_SetThreadPriority( HANDLE hThread, long lPriority, UINT flag )
{
	LPTHREAD lpThread;// = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );//2003-05-22, 修改

	DEBUGMSG( DEBUG_SetThreadPriority, ( "KL_SetThreadPriority:hThread=0x%x.lPriority=0x%x,flag=0x%x.\r\n", hThread,lPriority,flag  ) );
	if( flag == 0 )
	{  // 相对值 
		if( lPriority >= -15 && lPriority <= 15)
		{		
			lPriority = DEF_PRIORITY - lPriority;//lPriority为相对于DEF_PRIORITY的值
		}
		else
			goto _error_return;
	}
	
	if( (ULONG)lPriority > 255 ) 
        goto _error_return;

	if( hThread == CURRENT_THREAD_HANDLE )//2003-05-22, ADD
		lpThread = lpCurThread;//2003-05-22, ADD
	else//2003-05-22, ADD
		lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );//2003-05-22, ADD
	
	if( lpThread )
	{
		lpThread->nOriginPriority = (BYTE)lPriority;
		SetThreadCurrentPriority( lpThread, lPriority );
		DEBUGMSG( DEBUG_SetThreadPriority, ( "KL_SetThreadPriority: lpThread->nOriginPriority=0x%x.\r\n", lpThread->nOriginPriority ) );
		return TRUE;
	}
	else
	{
		WARNMSG( DEBUG_SetThreadPriority, ( "KL_SetThreadPriority: Invalid handle=0x%x.\r\n", hThread ) );
	}

_error_return:
	KL_SetLastError( ERROR_INVALID_PARAMETER );
	return FALSE;
}
// ********************************************************************
// 声明：int WINAPI KL_GetThreadPriority( HANDLE hThread, UINT flag )
// 参数：
//		IN hThread - 线程句柄
//		IN flag - 指lPriority代表的是相对值或是绝对值
// 返回值：
//		假如成功，返回优先级; 否则，返回 THREAD_PRIORITY_ERROR_RETURN
// 功能描述：
//		得到线程优先级
// 引用：
//		系统API
// ********************************************************************
int WINAPI KL_GetThreadPriority( HANDLE hThread, UINT flag )
{
	LPTHREAD lpThread;

	if( hThread == CURRENT_THREAD_HANDLE )
		lpThread = lpCurThread;
	else
		lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD ); // 由句柄得到线程对象的指针
	
	if( lpThread )
	{
		int retv;
		if( flag == 0 )
		{  // 得到相对值，在 -15 ~ 15 之间
		    retv = DEF_PRIORITY - lpThread->nOriginPriority;
			if( retv > 15 || retv < -15 )
				goto _error_return;
		}
		else  // 绝对值
			retv = lpThread->nOriginPriority;
		return retv;
	}
_error_return:
    return THREAD_PRIORITY_ERROR_RETURN;
}
// ********************************************************************
// 声明：BOOL WINAPI KL_SetThreadPolicy( HANDLE hThread, UINT uiPolicy )
// 参数：
//		IN hThread - 线程句柄
//		IN uiPolicy - 线程规则，为以下值：
//				THREAD_POLICY_OTHER - 默认的策略
//				THREAD_POLICY_ROTATION - 轮转调度，每次调度时，当前运行线程都会放到运行队列尾
//				THREAD_POLICY_FIFO - 先进先出
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		设置调度策略
// 引用：
//		系统API
// ********************************************************************
BOOL WINAPI KL_SetThreadPolicy( HANDLE hThread, UINT uiPolicy )
{
	LPTHREAD lpThread;// = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );//2003-05-22, 修改

	if( hThread == CURRENT_THREAD_HANDLE )//2003-05-22, ADD
		lpThread = lpCurThread;//2003-05-22, ADD
	else//2003-05-22, ADD
		lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );//2003-05-22, ADD

	if( lpThread )
    {
		if( (WORD)uiPolicy == THREAD_POLICY_OTHER ||
			(WORD)uiPolicy == THREAD_POLICY_ROTATION ||
			(WORD)uiPolicy == THREAD_POLICY_FIFO )
		{
            lpThread->fPolicy = (BYTE)uiPolicy;
			return TRUE;
		}
		KL_SetLastError( ERROR_INVALID_PARAMETER );
    }	
    return FALSE;
}
// ********************************************************************
// 声明：int WINAPI KL_GetThreadPolicy( HANDLE hThread )
// 参数：
//		IN hThread - 线程句柄
// 返回值：
//		返回值可以为以下值
//			THREAD_POLICY_OTHER: 该线程由系统降低其动态优先级计数，并由系统决定是否让该thread放弃CPU的使用权
//	　　　　THREAD_POLICY_FIFO：系统不会降低该线程的动态优先级计数
//			THREAD_POLICY_ROTATION：系统不会降低该线程的动态优先级计数，但当该线程产生切换时，该线程将回放到同级优先级队列的队尾
// 功能描述：
//		得到线程调度规则
// 引用：
//		系统API
// ********************************************************************
int WINAPI KL_GetThreadPolicy( HANDLE hThread )
{
	LPTHREAD lpThread;

	if( hThread == CURRENT_THREAD_HANDLE )
		lpThread = lpCurThread;
	else
		lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );

	if( lpThread )
    {
        return lpThread->fPolicy;
    }
    return THREAD_POLICY_ERROR;
}

// ********************************************************************
// 声明：DWORD WINAPI KL_ResumeThread( HANDLE hThread )
// 参数：
//		IN hThread - 线程句柄
// 返回值：
//		假如成功，返回之前的计数值；否则，返回-1
// 功能描述：
//		恢复线程执行，如果线程之前有多次挂起（调用SuspendThread）操作，必须调用
//		同样多次ResumeThread功能才能让线程由挂起态转为运行态。
// 引用：
//		系统API
// ********************************************************************

DWORD WINAPI KL_ResumeThread( HANDLE hThread )
{
	LPTHREAD lpThread;

	lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );

    if( lpThread )
		return DoResumeThread( lpThread );
	return -1;
}

// ********************************************************************
// 声明：DWORD WINAPI KL_SuspendThread( HANDLE hThread )
// 参数：
//		IN hThread - 线程句柄
// 返回值：
//		假如成功，返回之前的计数值；否则，返回-1
// 功能描述：
//		挂起线程，如果线程当前是运行态，则当执行该功能后，线程将会转为挂起态。
//		如果线程之前有多次挂起（调用SuspendThread）操作，必须调用
//		同样多次ResumeThread功能才能让线程由挂起态转为运行态。
// 引用：
//		系统API
// ********************************************************************

DWORD WINAPI KL_SuspendThread( HANDLE hThread )
{
	LPTHREAD lpThread;

	if( hThread == CURRENT_THREAD_HANDLE )
		lpThread = lpCurThread;
	else//2003-05-22, ADD
		lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );//2003-05-22, ADD
    if( lpThread )
	    return DoSuspendThread( lpThread );

	return -1;
}

// ********************************************************************
// 声明：BOOL WINAPI KL_GetExitCodeThread( HANDLE hThread, LPDWORD lpExitCode )
// 参数：
//		IN hThread - 线程句柄
//		OUT lpExitCode - 用于接受线程退出代码
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		得到线程的退出代码。如果该线程当前没有退出，则返回STILL_ACTIVE。否则，为一下值之一：
//		ExitThread 或 TerminateThread 的值；线程返回值；线程的进程的返回值
// 引用：
//		系统API
// ********************************************************************
BOOL WINAPI KL_GetExitCodeThread( HANDLE hThread, LPDWORD lpExitCode )
{
	LPTHREAD lpThread;

	if( hThread == CURRENT_THREAD_HANDLE )
	{
		*lpExitCode = STILL_ACTIVE;
		return TRUE;
	}
	else
		lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );

	if( lpThread )
    {
		*lpExitCode = STILL_ACTIVE;
        return TRUE;
	}
	else
	{
		 return GetHandleUserInfo( hThread, lpExitCode );
	}
}

// ********************************************************************
// 声明：BOOL FASTCALL Thread_Close( HANDLE hThread )
// 参数：
//		IN hThread - 线程句柄
// 返回值：
//		假如成功,返回TRUE；否则，返回FALSE
// 功能描述：
//		当用户调用CloseHandle时，会调用该函数去处理释放线程对象的工作
// 引用：
//		hmgr.c 
// ********************************************************************
BOOL FASTCALL Thread_Close( HANDLE hThread )
{
	LPTHREAD lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );  // 由句柄得到线程对象指针

	if( lpThread )
	{
		if( lpThread->uiRefCount )
		    lpThread->uiRefCount--;
		if( hThread == lpThread->hThread )
		    lpThread->hThread = NULL;
		return TRUE;
	}
	return FALSE;
}

// ********************************************************************
// 声明：BOOL WINAPI KL_SwitchToThread( VOID )
// 参数：
//		无
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		当前线程主动放弃CPU
// 引用：
//		系统API
// ********************************************************************
BOOL WINAPI KL_SwitchToThread( VOID )
{
    extern void CALLBACK Schedule(void);
	ScheduleTimeout(0);
	return TRUE;
}

// ********************************************************************
// 声明：void WINAPI KL_SetLastError( DWORD dwErrorCode )
// 参数：
//		IN dwErrorCode - 错误代码
// 返回值：
//		无
// 功能描述：
//		设置线程当前的错误代码
// 引用：
//		系统API
// ********************************************************************
void WINAPI KL_SetLastError( DWORD dwErrorCode )
{
    lpCurThread->dwErrorCode = dwErrorCode;
}

// ********************************************************************
// 声明：DWORD WINAPI KL_GetLastError( void )
// 参数：
//		无
// 返回值：
//		线程当前的错误代码
// 功能描述：
//		得到线程当前的错误代码
// 引用：
//		系统API
// ********************************************************************
DWORD WINAPI KL_GetLastError( void )
{
	return lpCurThread->dwErrorCode;
}

// ********************************************************************
// 声明：DWORD WINAPI KL_TlsAlloc(VOID)
// 参数：
//		无
// 返回值：
//		假如成功，返回基于0的TLS索引值；否则，返回TLS_OUT_OF_INDEXES
// 功能描述：
//		从当前线程分配一个空闲Slot索引值，如果该线程的当前进程不是该线程的拥有者（创建者），则该功能失败
// 引用：
//		系统API
// ********************************************************************
DWORD WINAPI KL_TlsAlloc(VOID)
{
	LPPROCESS lpCurProcess = lpCurThread->lpCurProcess;

	if( lpCurProcess == lpCurThread->lpOwnerProcess )
	{
		DWORD i, mask, dwOldMask;
		LPDWORD lpdwMask = &lpCurProcess->dwtlsMask;
		for( i = 0, mask = 1; i < TLS_ALLOCABLE_INDEXS; i++, mask <<= 1 )
		{
			dwOldMask = *lpdwMask;  // 得到当前进程的solt mask
			if( (dwOldMask & mask) == 0 )  // 没有使用吗 ？
			{	// 没有，得到它
				if( KL_InterlockedCompareExchange( (LPLONG)lpdwMask, dwOldMask | mask, dwOldMask ) == (LONG)dwOldMask )
				{   // 成功
					return i;
				}
				else
				{  // 失败，重新开始查找
					i = 0;
					mask = 1;
				}
			}
		}
	}
	else
	{
		RETAILMSG( 1, ( "error at KL_TlsAlloc: owner process != current process.\r\n" ) );
	}
	return TLS_OUT_OF_INDEXES;	
}

// ********************************************************************
// 声明：void ClearAllThreadTlsValue( LPPROCESS lpProcess, DWORD dwTlsIndex )
// 参数：
//		IN lpProcess - 进程指针
//		IN dwTlsIndex - TLS索引值
// 返回值：
//		无
// 功能描述：
//		将当前进程的所有线程的tls值设为0
// 引用：
//		系统API
// ********************************************************************

static void ClearAllThreadTlsValue( LPPROCESS lpProcess, DWORD dwTlsIndex )
{
	LPTHREAD lpThread;
	UINT uiSave;

	LockIRQSave( &uiSave );

	lpThread = lpProcess->lpFirstThread;
	while( lpThread )
	{
        lpThread->lpdwTLS[dwTlsIndex] = 0;
		lpThread = lpThread->lpNextThreadInProcess;
	}

	UnlockIRQRestore( &uiSave );
}

// ********************************************************************
// 声明：BOOL WINAPI KL_TlsFree( DWORD dwTlsIndex )
// 参数：
//		IN  dwTlsIndex - TLS索引值
// 返回值：
//		假如成功，返回TRUE；否则，返回FLASE
// 功能描述：
//		释放一个TLS索引值。如果该线程的当前进程不是该线程的拥有者（创建者），则该功能失败
// 引用：
//		系统API
// ********************************************************************
BOOL WINAPI KL_TlsFree( DWORD dwTlsIndex )
{
	LPPROCESS lpCurProcess = lpCurThread->lpCurProcess;

	if( lpCurProcess == lpCurThread->lpOwnerProcess )
	{
		DWORD dwOldMask;
		LPDWORD lpdwMask = &lpCurProcess->dwtlsMask;
		
		if( dwTlsIndex < TLS_ALLOCABLE_INDEXS )
		{
			ClearAllThreadTlsValue(lpCurProcess, dwTlsIndex); // 清除所有的该slot的值（设为0）
			do 
			{
				dwOldMask = *lpdwMask;
			} while( KL_InterlockedCompareExchange( (LPLONG)lpdwMask, dwOldMask & (~(1<<dwTlsIndex)), dwOldMask ) != (LONG)dwOldMask );
			return TRUE;
		}
	}
	return FALSE;
	
}

// ********************************************************************
// 声明：LPVOID DoTlsGetValue( LPTHREAD lpThread, DWORD dwTlsIndex )
// 参数：
//		IN lpThread - 线程结构指针
//		IN dwTlsIndex - slot索引值
// 返回值：
//		tls值
// 功能描述：
//		得到tls值
// 引用：
//		
// ********************************************************************
LPVOID DoTlsGetValue( LPTHREAD lpThread, DWORD dwTlsIndex )
{
	LPVOID lpRetv = NULL;
	BOOL bError = FALSE;
	if( dwTlsIndex < TLS_MAX_INDEXS )
	{
		UINT uiSave;
		
		LockIRQSave( &uiSave );
		
		if( lpThread->lpdwTLS )
		{
			lpRetv = (LPVOID)lpThread->lpdwTLS[dwTlsIndex];
		}
		else
		{
			bError = TRUE;
		}
		UnlockIRQRestore( &uiSave );
			
		if( bError == FALSE )
		{
			if( lpRetv == NULL )
				KL_SetLastError( ERROR_SUCCESS );
		}
		else
			KL_SetLastError( ERROR_ACCESS_DENIED );				
	}
	return lpRetv;
}
// ********************************************************************
// 声明：BOOL DoTlsSetValue( LPTHREAD lpThread, DWORD dwTlsIndex, LPVOID lpValue )
// 参数：
//		IN lpThread - 线程结构指针
//		IN dwTlsIndex - slot索引值
//		IN lpValue - 新的值
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		设置tls值
// 引用：
//		
// ********************************************************************
BOOL DoTlsSetValue( LPTHREAD lpThread, DWORD dwTlsIndex, LPVOID lpValue )
{
	if( dwTlsIndex < TLS_MAX_INDEXS )
	{
		UINT uiSave;
		LockIRQSave( &uiSave );
		
		//ASSERT( lpThread->lpdwTLS );
		if( lpThread->lpdwTLS )
			lpThread->lpdwTLS[dwTlsIndex] = (DWORD)lpValue;
		else
		{
			KL_SetLastError( ERROR_ACCESS_DENIED );
		}
		
		UnlockIRQRestore( &uiSave );
		
		return TRUE;
	}
	return FALSE;	
}


// ********************************************************************
// 声明：LPVOID DoTlsGetValue( LPTHREAD lpThread, DWORD dwTlsIndex )
// 参数：
//		IN lpThread - 线程结构指针
//		IN dwTlsIndex - slot索引值
// 返回值：
//		tls值
// 功能描述：
//		得到tls值。如果该线程的当前进程不是该线程的拥有者（创建者），则该功能失败
// 引用：
//		系统API	
// ********************************************************************

LPVOID WINAPI KL_TlsGetValue( DWORD dwTlsIndex )
{
	if( IS_RESERVE_TLS(dwTlsIndex) ||
		lpCurThread->lpCurProcess == lpCurThread->lpOwnerProcess
		)
	    return DoTlsGetValue( lpCurThread, dwTlsIndex );
	else
	{
		KL_SetLastError( ERROR_ACCESS_DENIED );
		RETAILMSG( 1, ( "error at KL_TlsGetValue: owner process != current process.\r\n" ) );
		return NULL;
	}
}
// ********************************************************************
// 声明：BOOL WINAPI KL_TlsSetValue( DWORD dwTlsIndex, LPVOID lpValue )
//		IN dwTlsIndex - slot索引值
//		IN lpValue - 新的值
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		设置tls值。如果该线程的当前进程不是该线程的拥有者（创建者），则该功能失败
// 引用：
//		系统API
// ********************************************************************
BOOL WINAPI KL_TlsSetValue( DWORD dwTlsIndex, LPVOID lpValue )
{
	if( IS_RESERVE_TLS(dwTlsIndex) ||
		lpCurThread->lpCurProcess == lpCurThread->lpOwnerProcess )
	    return DoTlsSetValue( lpCurThread, dwTlsIndex, lpValue );
	else
	{
		KL_SetLastError( ERROR_ACCESS_DENIED );
		RETAILMSG( 1, ( "error at KL_TlsAlloc: owner process != current process.\r\n" ) );
		return NULL;
	}
}
// ********************************************************************
// 声明：LPVOID WINAPI KL_TlsGetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex )
// 参数：
//		IN dwThreadId - 线程ID
//		IN dwTlsIndex - TLS索引
// 返回值：
//		之前用TlsSetValue设置的值
// 功能描述：
//		得到线程的TLS值
// 引用：
//		系统API
// ********************************************************************
LPVOID WINAPI KL_TlsGetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex )
{
	LPTHREAD lpThread = _GetThreadPtr( dwThreadId );
	if( lpThread )
	{
		return DoTlsGetValue( lpThread, dwTlsIndex ); 
	}
	else
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		RETAILMSG( 1, ( "error at KL_TlsGetThreadValue: KL_GetThreadTlsValue, invalid dwThreadId=%x.\r\n", dwThreadId ) );
	}
	return 0;
}

// ********************************************************************
// BOOLs WINAPI KL_TlsSetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex, LPVOID lpvData )
// 参数：
//		IN dwThreadId - 线程ID
//		IN dwTlsIndex - TLS索引
//		IN lpvData - 新的TLS值
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		设置线程的 TLS 值
// 引用：
//		系统API
// ********************************************************************

BOOL WINAPI KL_TlsSetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex, LPVOID lpvData )
{
	LPTHREAD lpThread = _GetThreadPtr( dwThreadId );
	if( lpThread )
	{
	    return DoTlsSetValue( lpThread, dwTlsIndex, lpvData );
	}
	else
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		RETAILMSG( 1, ( "error at KL_TlsSetThreadValue: invalid dwThreadId=%x.\r\n", dwThreadId ) );
	}
	return NULL;
}

// ********************************************************************
// 声明：UINT WINAPI KL_LockSchedule( void )
// 参数：
//		无
// 返回值：
//		当前锁住计数
// 功能描述：
//		锁住调度器，阻止调度器切换到其它线程
//      每调用该功能一次，锁住计数器就加一，当每调用Thread_UnlockSchedule一次，锁住计数器就减一。
//		当锁住计数器为0时，调度器可以调度。当锁住计数器大于0时，调度器被锁住，其它任何线程不能
//		抢先调度直到该线程主动放弃CPU的使用权或因为该线程等待资源而放弃CPU的使用权；当该线程再次
//		拥有资源后，调度器再次被锁住直到锁住计数器为0。
//		只有Thread_LockSchedule和Thread_UnlockSchedule功能可以改变锁住计数器的值。
// 引用：
//		系统API
// ********************************************************************
UINT WINAPI KL_LockSchedule( void )
{
	LOCK_SCHE();
	return lpCurThread->nLockScheCount; 
}

// ********************************************************************
// 声明：UINT WINAPI KL_UnlockSchedule( void )
// 参数：
//		无  
// 返回值：
//		当前锁住计数
// 功能描述：
//		每调用该功能一次，锁住计数器就减一，当每调用Thread_LockSchedule一次，锁住计数器就加一。
//		当锁住计数器为0时，调度器可以调度。当锁住计数器大于0时，调度器被锁住，其它任何线程不能
//		抢先调度直到该线程主动放弃CPU的使用权或因为该线程等待资源而放弃CPU的使用权；当该线程再
//		次拥有资源后，调度器再次被锁住直到锁住计数器为0。
//		只有Thread_LockSchedule和Thread_UnlockSchedule功能可以改变锁住计数器的值
//		
// 引用：
//		系统API
// ********************************************************************
UINT WINAPI KL_UnlockSchedule( void )
{
	UNLOCK_SCHE();
	return lpCurThread->nLockScheCount; 
}


// ********************************************************************
// 声明：UINT WINAPI KL_CaptureException( jmp_buf jmp_data )
// 参数：
//		IN jmp_data - EXCEPTION_CONTEXT 结构数据，包含用户数据 
// 返回值：
//		假如进入try 代码块（block），返回0；否则,返回错误代码
// 功能描述：
//		保存线程上下文，进入try代码块
//		当异常产生时，退出代码块
// 引用：
//		系统API
// ********************************************************************
#define DEBUG_ENTRYTRY 0
UINT WINAPI KL_CaptureException( LPEXCEPTION_CONTEXT jmp_data )
{
	LPEXCEPTION lpexp = KHeap_Alloc( sizeof( EXCEPTION ) ); // 分配异常结构
	
	RETAILMSG( DEBUG_ENTRYTRY, ( "KL_CaptureException entry.\r\n" ) );
	if( lpexp )
	{
		UINT uiSave;

		lpexp->jmp_data = *jmp_data;
		lpexp->uiMode = lpCurThread->lpCallStack->dwStatus;
		lpexp->lpOwnerProcess = GetAPICallerProcessPtr();
		lpexp->lpCallStack = lpCurThread->lpCallStack->lpNext;

		LockIRQSave( &uiSave );

		lpexp->lpNext = lpCurThread->lpException;
		lpCurThread->lpException = lpexp;
			
		UnlockIRQRestore( &uiSave );

		RETAILMSG( DEBUG_ENTRYTRY, ( "KL_CaptureException leave.\r\n" ) );
		return 0;
	}
	WARNMSG( DEBUG_ENTRYTRY, ( "KL_CaptureException: no memory!.\r\n" ) );
	return 1;
}


// ********************************************************************
// 声明：VOID WINAPI LeaveException( BOOL bException )
// 参数：
//		IN bException - 当前是否异常
// 返回值：
//		无
// 功能描述：
//		如果之前有做TryEntry,这里回到TryEntry里
// 引用：
//		
// ********************************************************************

#define DEBUG_LEAVE_EXCEPTION 1
VOID LeaveException( BOOL bException )
{
	EXCEPTION exp, * lpexp;
	UINT uiSave;
	ACCESS_KEY aky;

	LockIRQSave( &uiSave );
	aky = lpCurThread->akyAccessKey;
	if( (lpexp = lpCurThread->lpException) )
	{	// 之前有调用TryEntry
		//先保存到临时buf
		exp = *lpexp;
		// 移出链表
		lpCurThread->lpException = lpexp->lpNext;
		//			
		UnlockIRQRestore( &uiSave );
		//释放之前分配的数据
		KHeap_Free( lpexp, sizeof( EXCEPTION ) );
_LOOP:
		//跳
		if( bException )
		{
			if( exp.lpCallStack )
			{	// 异常在系统调用链 execption in sys call link
				// 查找并释放
				LPCALLSTACK lpcsPrev, lpcs = lpCurThread->lpCallStack;
				lpcsPrev = lpcs;
				DEBUGMSG( DEBUG_LEAVE_EXCEPTION, ( "LeaveException: find lpcs.\r\n" ) );
				while( lpcs )
				{
					if( lpcs == exp.lpCallStack )
					{	//找到
						if( lpcs != lpcsPrev )
						{	//非第一个
							lpcsPrev->lpNext = NULL;
							// 释放从 lpCurThread->lpCallStack 到 lpcsPrev 的 callstack
							DEBUGMSG( DEBUG_LEAVE_EXCEPTION|1, ( "LeaveException: FreeThreadCallStack.\r\n" ) );
							FreeThreadCallStack( lpCurThread->lpCallStack );
							// 回退调用链
							lpCurThread->lpCallStack = lpcs;							
						}
						break;
					}
					lpcsPrev = lpcs;
					lpcs = lpcs->lpNext;
				}
				if( lpcs == NULL )
				{
					ERRORMSG( DEBUG_LEAVE_EXCEPTION, ( "LeaveException: error, not find lpcs.\r\n" ) );
					KL_ExitThread( -1 );
				}
				aky = lpcs->akyAccessKey | lpexp->lpOwnerProcess->akyAccessKey;
			}
			DEBUGMSG( DEBUG_LEAVE_EXCEPTION, ( "LeaveException: SwitchToProcess.\r\n" ) );
			SwitchToProcess( lpexp->lpOwnerProcess, NULL );
			AccessKey_Set( &lpCurThread->akyAccessKey, aky );
			ASSERT( lpCurThread->dwMagic == THREAD_MAGIC );
		    HandlerException( &exp.jmp_data, 1, exp.uiMode );
			//不会到这里
			goto _LOOP;
		}
	}
	else
		UnlockIRQRestore( &uiSave );
}


// ********************************************************************
// 声明：VOID WINAPI KL_ReleaseException( void )
// 参数：
//		无  
// 返回值：
//		无
// 功能描述：
//		正常离开　try 执行块
// 引用：
//		
// ********************************************************************
#define DEBUG_LEAVETRY 0
BOOL WINAPI KL_ReleaseException( void )
{
    RETAILMSG( DEBUG_LEAVETRY, ( "KL_ReleaseException entry.\r\n" ) );
	LeaveException( FALSE );
    RETAILMSG( DEBUG_LEAVETRY, ( "KL_ReleaseException leave.\r\n" ) );
	return TRUE;
}

#define DEBUG_KL_ChangeThreadRotate 0
UINT WINAPI KL_ChangeThreadRotate( DWORD dwThreadId, int nAddRotate )
{
	LPTHREAD lpThread = _GetThreadPtr( dwThreadId );
	UINT uOld = 0;
	if( lpThread )
	{
		UINT uCur;
		uOld = uCur = lpThread->nRotate;
		uCur += nAddRotate;
		if( uCur >= MIN_ROTATE && uCur <= MAX_ROTATE )
			lpThread->nRotate = uCur;  // 有效
		else
		{	//无效
			KL_SetLastError( ERROR_INVALID_PARAMETER );
			uOld = 0;
			WARNMSG( DEBUG_KL_ChangeThreadRotate, ( "KL_ChangeThreadRotate: nAddRotate(%d) is too larger.\r\n", nAddRotate ) );
		}
	}
	return uOld;
}

#define DEBUG_KL_SetPermissions 0
DWORD WINAPI KL_SetPermissions( DWORD dwNewPerm )
{
	DWORD dwOldPerm = 0;
    //PCALLSTACK pcstk = pCurThread->pcstkTop;
	CALLSTACK * lpcs;
    

    // 必须有拥有者进程权限 和 让内核能够存取
    AccessKey_Add( &dwNewPerm, (lpCurThread->lpOwnerProcess->akyAccessKey | InitKernelProcess.akyAccessKey) );
	lpcs = lpCurThread->lpCallStack;
    
    if ( lpcs == NULL || (lpcs->dwCallInfo & CALL_KERNEL) == 0 ) 
	{	//当前线程在内核态        
		dwOldPerm = lpCurThread->akyAccessKey;
		AccessKey_Set( &lpCurThread->akyAccessKey, dwNewPerm );
    } 
	else 
	{   // 系统调用, 当返回时会设置该 值
        dwOldPerm = lpcs->akyAccessKey;
        lpcs->akyAccessKey = dwNewPerm;
    }
	DEBUGMSG( DEBUG_KL_SetPermissions, ( "KL_SetPermissions:dwNewPerm=0x%x,dwOldPerm=0x%x,ThreadID=0x%x.\r\n", dwNewPerm, dwOldPerm, lpCurThread->dwThreadId ) );
    
    return dwOldPerm;
}

DWORD WINAPI KL_GetPermissions( VOID )
{
	return lpCurThread->akyAccessKey;
}
