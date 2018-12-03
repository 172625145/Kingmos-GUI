/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����̹߳���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2004-12-21, �� thread struct ��Ϊ 4 *1024 , ����1024 ��Ϊ loadfailuerpage, 
				[thread data]  xxx
				[thread kernel stack] xxx -> 3*1024, user by exception, int ...
				[use by loadfailurepage(1024)]
	         
	2004-08-11, �����ö�̬����( WaitForSingleObject��WaitMultiSingleObject  ) �ź������漰�� semaphpor.c thread.c sche.c
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
// ������static void * AllocThreadStack( DWORD dwCreateFlag, LPPROCESS lpProcess, DWORD dwSize )
// ������
//	IN dwCreateFlag - ������־���ο�CreateThread��
//	IN lpProcess - PROCESS �ṹָ��
//	IN dwSize - ��Ҫ��ջ��С
// ����ֵ��
//	���ط�����ڴ��ַ
// ����������
//	Ϊһ���µ��̷߳���һ������ջ
// ���ã�
//		
// ********************************************************************
#define DEBUG_AllocThreadStack 0

static void * AllocThreadStack( DWORD dwCreateFlag, LPPROCESS lpProcess, DWORD dwSize )
{
#ifdef VIRTUAL_MEM

    LPVOID lpStack, lpUpProtect, lpDownProtect;

	DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: use virtual mem:dwSize(0x%x) entry.\r\n", dwSize ) );
	if( dwCreateFlag & CREATE_MAINTHREAD )	//�����߳���
	{	//�ǣ��ӽ��̿ռ��ĩ�˷���
		DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: main thread.\r\n" ) );
		ASSERT( dwSize <= 64 * 1024 ); 
	    lpStack = KC_VirtualAlloc( (LPVOID)( lpProcess->dwVirtualAddressBase + PROCESS_SPACE - dwSize - UP_PROTECT_SIZE ), dwSize, MEM_RESERVE|MEM_AUTO_COMMIT, PAGE_READWRITE );
		
		//lpStack = KC_VirtualAlloc( (LPVOID)lpProcess->dwVirtualAddressBase, dwSize, MEM_RESERVE|MEM_AUTO_COMMIT, PAGE_READWRITE );
		
	}
	else	//�ӽ��̿ռ������ط�����
		lpStack = KC_VirtualAlloc( (LPVOID)( lpProcess->dwVirtualAddressBase ), dwSize, MEM_RESERVE|MEM_AUTO_COMMIT, PAGE_READWRITE );

	DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: lpStack=0x%x.\r\n", lpStack ) );
	if( lpStack )
	{
		LPVOID p;	

		DEBUGMSG( DEBUG_AllocThreadStack, ( "AllocThreadStack: commit.\r\n" ) );
		//�ύ�ڴ�
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
// ������static void FreeThreadCallStack( CALLSTACK * lpcs )
// ������
//		IN lpcs - CALLSTACK �ṹָ��
// ����ֵ��
//		��
// ����������
//		�ͷ��̺߳��жѽṹ
// ���ã�
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
// ������static void FreeThreadExcaption( CALLSTACK * lpcs )
// ������
//		IN lpexp - LPEXCEPTION �ṹָ��
// ����ֵ��
//		��
// ����������
//		�ͷ��̵߳��쳣�ṹ
// ���ã�
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
// ������static void FreeThreadStack( LPPROCESS lpProcess, LPVOID lpStack, DWORD dwStackSize )
// ������
//	IN lpProcess - PROCESS �ṹָ��
//	IN lpStack - ջָ��
//	IN dwStackSizes - ջ��С
// ����ֵ��
//	��
// ����������
//	�ͷ��̵߳�ջ�ռ�
// ���ã�
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
// ������LPTHREAD _GetThreadPtr( DWORD id )
// ������
//	IN id - �߳�ID
// ����ֵ��
//	�߳̽ṹָ��
// ����������
//	���߳�ID�õ��߳̽ṹָ��
// ���ã�
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
// ������void AddToProcess( LPTHREAD lpThread )
// ������
//		IN lpThread - THREAD �ṹָ��
// ����ֵ��
//		��
// ����������
//		���̼߳����������
// ���ã�
//		
// ********************************************************************
static void AddToProcess( LPTHREAD lpThread )
{
	UINT uiSave;

	//ASSERT( lpThread->lpNextThreadInProcess == NULL && lpThread->lpPrevThreadInProcess == NULL );
	LockIRQSave( &uiSave );
	
	// ���ӵ���������
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
// ������void RemoveFromProcess( LPTHREAD lpThread )
// ������
//	IN lpThread - THREAD �ṹָ��
// ����ֵ��
//	��
// ����������
//	��	AddToProcess �෴���ú������̴߳ӽ����������Ƴ�
// ���ã�
//		
// ********************************************************************
static void RemoveFromProcess( LPTHREAD lpThread )
{
	UINT uiSave;

	LockIRQSave( &uiSave );	//���ж�
	// �Ƴ���������
	if( lpThread->lpPrevThreadInProcess )
		lpThread->lpPrevThreadInProcess->lpNextThreadInProcess = lpThread->lpNextThreadInProcess;
	else
	{   // ��һ��
		lpThread->lpOwnerProcess->lpFirstThread = lpThread->lpNextThreadInProcess;
	}
	if( lpThread->lpNextThreadInProcess )
		lpThread->lpNextThreadInProcess->lpPrevThreadInProcess = lpThread->lpPrevThreadInProcess;
	lpThread->lpNextThreadInProcess = lpThread->lpPrevThreadInProcess = NULL;

	UnlockIRQRestore( &uiSave );
}

// ********************************************************************
// ������DWORD WINAPI FirstThreadProc( LPVOID lParam )
// ������
//		IN lParam - ��  CreateThread ���ݵĲ���
// ����ֵ��
//		����0
// ����������
//		���̵߳��ں˴��벿��
// ���ã�
//		�ɸô������ CreateThread ���ݵĺ������
// ********************************************************************
static DWORD WINAPI FirstThreadProc( LPVOID lParam )
{
	DWORD dwExitCode;

	//EdbgOutputDebugString( "Call ThreadProc:hThread=%x,lParam=%x,ThreadID=%x.\r\n", lpCurThread->hThread, lParam, lpCurThread->dwThreadId );
    // ����CreateThread���ݵĺ������
	if( lpCurThread->lpCurProcess->lpMainThread == lpCurThread )
	    dwExitCode =  ((LPTHREAD_START_ROUTINE)( lpCurThread->lpfnStartAddress ))( lParam );
	else
	{
		dwExitCode = CallUserStartupCode( lpCurThread->lpfnStartAddress, lParam );
	}
	// �˳��߳�
	KL_ExitThread( dwExitCode );
	return 0;
}

// ********************************************************************
// ������static LPTHREAD InitThreadData(
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
// ������
//		IN lpThread - THREAD �ṹָ��
//		IN lpProcess - PROCESS �ṹָ��
//		IN hThread - �߳̾��
//		IN dwStackSize - ���̵߳���Ҫ������ջ��С
//		IN dwPreservStackSize - ���ѷ����ջ�ռ�����Ҫ������ϵͳ�õĴ�С, dwPreservStackSize < dwStackSize
//		IN lpStartAddress - �߳������������
//		IN lpParameter - ���ݸ��̵߳Ĳ���
//		OUT lpdwId - ���ڽ����߳�ID��ָ��
// ����ֵ��
//		
// ����������
//		
// ���ã�
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
	// �����ź���	
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
	// ��ʼ���߳̽ṹ
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
	// ���߳�ָ��õ��߳� ID
    *lpdwId = lpThread->dwThreadId = (DWORD)MAKE_HANDLE( lpThread );
	// 
	lpThread->dwThreadStackSize = dwStackSize;
	lpThread->lpdwThreadStack = (LPDWORD)lpStack;
	// Ԥ�� Thread local slots �Ŀռ䲢��ʼ��
	// stack space format:
	// low address                                  high address
	// lpStack                                   
	// [user space uuuuuuuuuuuuuuuuuuuuuuuuuuu][TLS][cmdline]
	// 2004-12-30 TLS ���� stack �з���
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
	// ���߳����ӽ����߳�����	
	_LinkThread( lpThread );
	// ���̼߳�������������ӽ��̵��߳���
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
	// ������
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
// ������void ThreadSelfExit( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		���̲߳��ǵ�ǰ�߳�ʱ�� �õ��� MakeThreadToDie �����̵߳�IP��ΪThreadSelfExit
//		ʹ��Ҫ��ɱ�����̳߳�Ϊ��ǰ�߳�
// ���ã�
//		�ڲ�ʹ��
// ********************************************************************
#define DEBUG_ThreadSelfExit 0
static void ThreadSelfExit( void )
{
	KL_ExitThread( lpCurThread->dwExitCode );
	ERRORMSG( DEBUG_ThreadSelfExit, ( "ThreadSelfExit: error.\r\n" ) );	
}


// ********************************************************************
// ������static void MakeThreadToDie( LPTHREAD lpThread, DWORD dwExitCode )
// ������
//		IN lpThread - �߳̽ṹָ��
//		IN dwExitCode - �߳��˳�����
// ����ֵ��
//		��
// ����������
//		�����̵߳��˳�
// ���ã�
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
		// Ӧ���ж��߳��Ƿ����ں�̬
		/*
		if( lpThread->lpCallStack && 
			( lpThread->lpCallStack->dwCallInfo & CALL_KERNEL ) )
		{	//���ں�̬
			lpThread->dwSignal |= SIGNAL_EXIT;
			bNeedResched = TRUE;
		}
		else
		{	//���ں�̬
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
// ������void _DoThreadExit( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		�����߳��˳�
// ���ã�
//		�ڲ�
// ********************************************************************
#define DEBUG_DoThreadExit 0

static void _DoThreadExit( void )
{
	LPVOID lpvTemp;
	DEBUGMSG( DEBUG_DoThreadExit|1, ( "_DoThreadExit pThread(0x%x), idThread(0x%x).\r\n", lpCurThread, lpCurThread->dwThreadId ) );
	// �����з���㲥�߳��˳��¼�
	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:DO SCC_BROADCAST_THREAD_EXIT.\r\n" ) );	
	EnumServerHandler( SCC_BROADCAST_THREAD_EXIT, lpCurThread->dwThreadId, 0 );//( ThreadExitServerNotify, lpCurThread->dwThreadId );
	if( lpCurThread->lpOwnerProcess->lpFirstThread->lpNextThreadInProcess == NULL )
	{  // ����Ψһ���̣߳���Ҫ�����е��������
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

		// �����з���㲥�����˳��¼�
    	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:DO SCC_BROADCAST_PROCESS_EXIT.\r\n" ) );			
		EnumServerHandler( SCC_BROADCAST_PROCESS_EXIT, (WPARAM)lpProcess->hProcess, 0 );
		
		// �ͷ��߳̾��
		if(lpProcess->lpMainThread->hThread)
		    KL_CloseHandle( lpProcess->lpMainThread->hThread );
		DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:Module_DeInit.\r\n" ) );
		// �ͷ�ģ��
		
		Module_DeInit( lpProcess->pModule );

		lpProcess->pModule = NULL;// LN 2003-05-21, add
		
    	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:KL_HeapDestroy.\r\n" ) );
    	
		// �ͷŽ��̶�
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
		// �ͷ����еľ��
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

	//�ͷ�ӵ�е�CS MUTEX
	_Semaphore_HandleThreadExit( lpCurThread );
	// ������ܣ��ͷ� timer
	//if( lpCurThread->lpTimer )
	if( lpCurThread->timer.lpNext )
	{
		DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:_DelTimerList.\r\n" ) );
		_DelTimerList(&lpCurThread->timer);
		//lpCurThread->lpTimer = NULL;
	}
	// ������ܣ��ͷ��ź���
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
		//2004-08-11, �����ö�̬����

		lpCurThread->nWaitCount = 0;
		lpCurThread->lpWaitQueue = NULL;
	}
	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:_SemaphoreRelease.\r\n" ) );
	// �ͷ��߳��ź���
	_SemaphoreRelease( lpCurThread->lpsemExit, 1, NULL );

	lpvTemp = (LPVOID)KL_InterlockedExchange( (LPLONG)&lpCurThread->lpsemExit, 0 );
	_CloseSemaphoreObj( (LPSEMAPHORE)lpvTemp, 0 );
	
	// �Ƴ��߳��ź���
	//_SemaphoreRemove( lpCurThread->lpsemExit );

	DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:FreeThreadCallStack.\r\n" ) );
	//�ͷ��쳣�ṹ
	lpvTemp = (LPVOID)lpCurThread->lpException;
	lpCurThread->lpException = NULL;
    FreeThreadExcaption( (LPEXCEPTION)lpvTemp );	

	// �ͷź���ջ
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
	    // �ӽ����Ƴ�
    	lpCurThread->dwThreadId = 0; // ��ֹ��ʹ�ø�thread
        RemoveFromProcess( lpCurThread );
		
    	lpCurThread->lpCurProcess = &InitKernelProcess;
		
		lppProcessSegmentSlots[0] = NULL;
		//�л����ں�ջ
		//lpCurThread->lpdwThreadUserStack = lpStack;
		//lpCurThread->lpdwThreadKernelStack = lpStack;
	    SwitchToStackSpace( (LPVOID)( (DWORD)lpStack + PAGE_SIZE - sizeof(long) ) );
		INTR_ON();
	}
    DEBUGMSG( DEBUG_DoThreadExit, ( "_DoThreadExit:_FreeThreadResourceAndReschedule.\r\n" ) );
    _FreeThreadResourceAndReschedule();
}

// ********************************************************************
// ������void HandleThreadExit( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		�����߳��˳�
// ���ã�
//		��ģ�� �� process.c Ҳ����øú��� 
// ********************************************************************
#define DEBUG_HANDLETHREADEXIT 0
void HandleThreadExit( void )
{
	//��Ϊ��Զ��Ӧ�÷��أ��������л����µ�sp,���������¿��ˣ�
	//1����thread ��Զ���᷵��
	//2����Ϊexit thread ʱ����� server�� �������������п��ܻ��õ����stack
	//
	SwitchToStackSpace( lpCurThread->lpdwThreadUserStack );
	_DoThreadExit();
//	ASSERT(0); //�������� 
}


// ********************************************************************
// ������void WINAPI KL_ExitThread( DWORD dwExitCode )
// ������
//		IN dwExitCode - �˳�����
// ����ֵ��
//		��
// ����������
//		�˳���ǰ�߳�
// ���ã�
//		ϵͳAPI		
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
	{ // ���߳� main thread
		EdbgOutputDebugString( "Main Thread Exit. now, call ExitProcess.\r\n" );
		//KL_TerminateProcess( lpCurThread->lpOwnerProcess->hProcess, dwExitCode );
		KL_ExitProcess( dwExitCode );
	}
	else
	{
		//�����߳�
		lpCurThread->dwExitCode = dwExitCode;
		HandleThreadExit();
	}
//	ASSERT( 0 );	
}

// ********************************************************************
// ������BOOL WINAPI KL_TerminateThread( HANDLE hThread, DWORD dwExitCode )
// ������
//		IN hThread - �߳̾��
//		IN dwExitCode - �˳�����
// ����ֵ��
//		�ɹ�������TRUE��ʧ�ܣ�����FALSE
// ����������
//		��������ֹһ���߳�
// ���ã�
//		ϵͳAPI
// ********************************************************************
#define DEBUG_TerminateThread 0
BOOL WINAPI KL_TerminateThread( HANDLE hThread, DWORD dwExitCode )
{
	LPTHREAD lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD ); // �ɾ���õ��̶߳���ָ��

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
// ������DWORD WINAPI KL_GetCurrentThreadId( void )
// ������
//		��
// ����ֵ��
//		���ص�ǰ�̵߳�ID
// ����������
//		�õ���ǰ�̵߳�ID
// ���ã�
//		ϵͳAPI
// ********************************************************************
DWORD WINAPI KL_GetCurrentThreadId( void )
{
    return lpCurThread->dwThreadId;
}

// ********************************************************************
// ������HANDLE WINAPI KL_GetCurrentThread( void )
// ������
//		��
// ����ֵ��
//		��ǰ�̵߳ľ��
// ����������
//		�õ���ǰ�̵߳ľ�����þ���ǵ�ǰ�̵߳ı���������һ����ʵ�ľ��
// ���ã�
//		ϵͳAPI
// ********************************************************************
HANDLE WINAPI KL_GetCurrentThread( void )
{
	return CURRENT_THREAD_HANDLE; 
}


// ********************************************************************
// ������static BOOL _DeleteThreadObjectPtr( LPTHREAD lpThread )
// ������
//		IN lpThread - THREAD �ṹָ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		ɾ���̶߳���
// ���ã�
//		
// ********************************************************************
#define DEBUG_DELETETHREADOBJECT 0
static BOOL _DeleteThreadObjectPtr( LPTHREAD lpThread )
{
	ASSERT( lpThread->objType == OBJ_THREAD && lpThread->dwState == THREAD_ZOMBIE );
	//ASSERT( lpThread->dwState == THREAD_ZOMBIE );
	// �ͷ��߳̽ṹ
	_RemoveThread( lpThread );  // ���߳������Ƴ�
	RemoveFromProcess( lpThread );
	lpThread->objType = OBJ_NULL;

		// �ͷ��ź���
	if( lpThread->lpsemExit )
	{
		ASSERT(0);
	    //_SemaphoreDeletePtr( lpThread->lpsemExit );
	    lpThread->lpsemExit = NULL;
	}
    // lpThread->lpdwThreadUserStack �� HandleThreadExit �б����¸�ֵ
	// ���СΪ PAGE_SIZE
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
// ������static BOOL _DeleteThreadObject( LPTHREAD lpThread )
// ������
//		IN lpThread - THREAD �ṹָ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		ɾ���̶߳���
// ���ã�
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
			SetHandleObjPtr( lpThread->hThread, NULL );  // �������Ӧ�Ķ���ָ����ΪNULL

		lpdwTLS = (LPDWORD)KL_InterlockedExchange( (LPLONG)&lpThread->lpdwTLS, 0 );
		
		KHeap_Free( lpdwTLS,TLS_ALLOC_SIZE );


		//_RemoveThread( lpThread );  // ���߳������Ƴ� 2004.03-03 , remove to DeleteThreadObjectPtr
		//lpThread->objType = OBJ_NULL;
		//�ͷ��߳�ջ�ռ�
		FreeThreadStack( lpThread->lpOwnerProcess, (void*)lpThread->lpdwThreadStack, lpThread->dwThreadStackSize );
		lpThread->lpdwThreadStack = NULL;
		
		// test
		DEBUGMSG( DEBUG_DELETETHREADOBJECT, ( "D:MainThread CallBack(0x%x).\r\n", lpThread->lpOwnerProcess->lpMainThread->lpCallStack ) );
		//
		
		if( lpThread->lpOwnerProcess->dwThreadCount == 1 )
		{   // ���ǽ��̵����һ���̣߳��߳������� ����� FreeProcessObject��ȥ�ͷ�
			DEBUGMSG( DEBUG_DELETETHREADOBJECT, ( "Free main thread.\r\n" ) );
			lpProcess = lpThread->lpOwnerProcess;
			lpProcess->lpMainThread = NULL;
//			ASSERT( lpProcess );
		}
		else
		{	// ��ȥ���ü���
			KL_InterlockedDecrement( (LPLONG)&lpThread->lpOwnerProcess->dwThreadCount );
			DEBUGMSG( DEBUG_DELETETHREADOBJECT, ( "Free not main thread.\r\n" ) );
			lpProcess = NULL;
		}
	
		if( lpProcess )
		{   // �ͷŽ��̶���
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
	//�����̼߳��뵽�ں˽���
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
// ������DWORD DoSuspendThread( LPTHREAD lpThread )
// ������
//		IN  lpThread - THREAD �ṹָ��
// ����ֵ��
//		����ɹ�������֮ǰ�ļ���ֵ��������󣬷���-1
// ����������
//		�����߳�
// ���ã�
//		
// ********************************************************************
DWORD DoSuspendThread( LPTHREAD lpThread )
{
	DWORD dwOldV;
	UINT uiSave;

	LockIRQSave( &uiSave );
	if( (dwOldV = lpThread->nSuspendCount) == MAXIMUM_SUSPEND_COUNT )  // �Ƿ�ﵽ������
	{
		dwOldV = -1;
		goto _ret;
	}
	if( ++lpThread->nSuspendCount == 1 )  // �Ƿ��һ�ι��� ��
	{  // ��
		if( lpThread == lpCurThread )  // ��ǰ�߳� �� 
		{  // ��
			lpThread->dwState = THREAD_SUSPENDED;
			UnlockIRQRestore( &uiSave );
			Schedule();
			return dwOldV;
		}
		else if( lpThread->dwState == THREAD_RUNNING )
		{   // �ǵ�ǰ�̣߳����ҵ�ǰ�̵߳ȴ�����			
			RemoveFromRunQueue( lpThread );		
			lpThread->dwState = THREAD_SUSPENDED;
		}
	}
_ret:
    UnlockIRQRestore( &uiSave );
    return dwOldV;
}

// ********************************************************************
// ������DWORD DoResumeThread( LPTHREAD lpThread )
// ������
//		IN  lpThread - THREAD �ṹָ��  
// ����ֵ��
//		����ɹ�������֮ǰ�ļ���ֵ��������󣬷���-1
// ����������
//		��DoSuspendThread��Ӧ���ù�������/�ָ��߳�
// ���ã�
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
// ������void KillAllOtherThreads( LPPROCESS lpProcess )
// ������
//		IN  lpProcess - PROCESS �ṹָ��
// ����ֵ��
//		��
// ����������
//		ɱ�����ڸý��̵������߳�
// ���ã�
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
			lpThread = lpProcess->lpFirstThread; // �ӵ�һ�����¿�ʼ
		}
		else
		{	//ע�⣡����������Ϊ�ǹ��ж�״̬��
			//���������Ϣ������Ҫ���ʱ�䣬Ӱ�쵽����豸��ʧ����
			//��ˣ�������ù��ܺ�Ӧ�ý�������ص�����
			//DEBUGMSG( DEBUG_KillAllOtherThreads, ( "KillAllOtherThreads:dwState=0x%x,lpThread->lpCurProcess=0x%x,lpProcess=0x%x,lpThread->flag=0x%x,lpCurThread=0x%x.\r\n", lpThread->dwState, lpThread->lpCurProcess,lpProcess,lpThread->flag,lpCurThread ) );
			lpThread = lpThread->lpNextThreadInProcess;
		}
	}

	UnlockIRQRestore( &uiSave );
}

// ********************************************************************
// ������void HandleSignal( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		�����ź�
// ���ã�
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
	{	// �����õ��̶߳�����Ҫ���
		lpCurThread->dwBlocked |= SIGNAL_CLEAR;  // disable SIGNAL_CHILD
		lpCurThread->dwSignal &= ~SIGNAL_CLEAR;

		// �������źŵ��̲߳������ź�
		lpThread = InitKernelProcess.lpFirstThread;
		while( lpThread )
		{
			if( lpThread->dwState == THREAD_ZOMBIE )
			{
				UnlockIRQRestore( &uiSave );

				DEBUGMSG( DEBUG_HANDLESIGNAL, ( "Thread(%x) Zombie.\r\n", lpThread ) );
				
				_DeleteThreadObjectPtr( lpThread );
				LockIRQSave( &uiSave );
				//���¿�ʼ
				lpThread = InitKernelProcess.lpFirstThread;
				continue;
			}
			//��һ��
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
// ������static LPTHREAD AllocThreadStruct( VOID )
// ������
//	��
// ����ֵ��
//		����ɹ��������߳̽ṹָ�룻���򣬷���NULL
// ����������
//		�����߳̽ṹ
// ���ã�
//		
// ********************************************************************

static LPTHREAD AllocThreadStruct( VOID )
{
	LPTHREAD lpThread;
	lpThread = (LPTHREAD)KHeap_Alloc( THREAD_STRUCT_SIZE ); // �����߳̽ṹ
	if( lpThread )
	{
		memset( lpThread, 0, THREAD_STRUCT_SIZE );
	}
	return lpThread;
}

// ********************************************************************
// ������static LPTHREAD AllocThreadStruct( VOID )
// ������
//	��
// ����ֵ��
//		����ɹ��������߳̽ṹָ�룻���򣬷���NULL
// ����������
//		�����߳̽ṹ
// ���ã�
//		
// ********************************************************************
static VOID FreeThreadStruct( LPTHREAD lpThread )
{
	KHeap_Free( lpThread, THREAD_STRUCT_SIZE );
}

// ********************************************************************
// ������LPTHREAD FASTCALL _CreateThread(
//								LPPROCESS lpProcess,
//								LPSECURITY_ATTRIBUTES lpThreadAttributes,   // pointer to thread security attributes
//								DWORD dwStackSize,                      // initial thread stack size, in bytes
//								DWORD dwPreservStackSize,
//								LPTHREAD_START_ROUTINE lpStartAddress, // pointer to thread function
//								LPVOID lpParameter,                // argument for new thread
//								DWORD dwCreationFlags,         // creation flags
//								LPDWORD lpThreadId         // pointer to returned thread identifier
//								)

// ������
//		IN lpProcess - PROCESS �ṹָ��
//		IN lpThreadAttributes - ��ȫ����
//		IN dwStackSize - ���̵߳���Ҫ������ջ��С
//		IN dwPreservStackSize - ���ѷ����ջ�ռ�����Ҫ������ϵͳ�õĴ�С, dwPreservStackSize < dwStackSize
//		IN lpStartAddress - �߳������������
//		IN lpParameter - ���ݸ��̵߳Ĳ���
//		IN dwCreationFlags - ������־
//		OUT lpThreadId - ���ڽ����߳�ID��ָ��

// ����ֵ��
//		����ɹ��������߳̽ṹָ�룻���򣬷���NULL
// ����������
//		�����߳�
// ���ã�
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
			dwStackSize = 64 * 1024;  //������64k�� ��Ϊ���̵߳�stack��64k-128k
#else
	#ifdef EML_WIN32
		if( dwStackSize == 0 )  //��Window��ģ��
			dwStackSize = 128 * 1024;
	#else
		if( dwStackSize == 0 )
			dwStackSize = 16 * 1024;
	#endif

#endif
		dwThreadID = 0;
		DEBUGMSG( DEBUG_CREATETHREAD, ( "_CreateThread:alloc thread struct.\r\n" ) );
		// 2004-12-21, remove by lilin
		//lpThread = (LPTHREAD)KHeap_Alloc( sizeof( THREAD ) ); // �����߳̽ṹ
		lpThread = AllocThreadStruct();
		//

		
		if( lpThread )
		{
			DEBUGMSG( DEBUG_CREATETHREAD, ( "_CreateThread: alloc thread handle.\r\n" ) );
			hThread = Handle_Alloc( lpProcess, lpThread, OBJ_THREAD ); // ������
			//DEBUGMSG( DEBUG_CREATETHREAD, ( "_C5.\r\n" ) );
			if( hThread )
			{
				DEBUGMSG( DEBUG_CREATETHREAD, ( "_CreateThread: call InitThreadData.\r\n" ) );
				// ��ʼ���߳̽ṹ����
				if( InitThreadData( 
					lpThread, 
					lpProcess, 
					hThread,
					dwStackSize, 
					dwPreservStackSize,
					lpStartAddress, 
					lpParameter,
					dwCreationFlags | CREATE_SUSPENDED, // �ȹ���
					&dwThreadID	 ) )
				{
					if( lpThreadId )
						*lpThreadId = dwThreadID;
					lpThread->uiRefCount = 1;  // add referent count
					DEBUGMSG( DEBUG_CREATETHREAD, ( "_CreateThread: success, handle=0x%x,id=0x%x,proc=0x%s.\r\n", hThread, dwThreadID, lpProcess->lpszApplicationName ) );
					if( (dwCreationFlags&CREATE_SUSPENDED) == 0 ) // �Ƿ��ȹ���
						//_WakeupThread( lpThread, FALSE ); // �񣬽��̼߳������ж���
						DoResumeThread( lpThread );// �񣬽��̼߳������ж���
				}
				else
				{   // ��ʼ��ʧ��
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
			{   // ������ʧ��
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
// ������HANDLE WINAPI KL_CreateThread(
//					  LPSECURITY_ATTRIBUTES lpThreadAttributes,   //
//					  DWORD dwStackSize,                      //
//					  LPTHREAD_START_ROUTINE lpStartAddress, //
//					  LPVOID lpParameter,                //
//					  DWORD dwCreationFlags,         //
//					  LPDWORD lpThreadId         //
//					  )

// ������
//		IN lpThreadAttributes - ��ȫ����
//		IN dwStackSize - ���̵߳���Ҫ������ջ��С
//		IN lpStartAddress - �߳������������
//		IN lpParameter - ���ݸ��̵߳Ĳ���
//		IN dwCreationFlags - ������־,����Ϊ:
//					CREATE_SUSPENDED - ���������ֱ������ResumeThread
//		OUT lpThreadId - ���ڽ����߳�ID��ָ��
// ����ֵ��
//		����ɹ��������߳̾�������򣬷���NULL	
// ����������
//		�����µ��߳�
// ���ã�
//		ϵͳAPI	
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
// ������BOOL WINAPI KL_SetThreadPriority( HANDLE hThread, long lPriority, UINT flag )
// ������
//		IN hThread - �߳̾��
//		IN lPriority - �߳����ȼ�, �� bPriorityValue ΪFALSEʱ Ϊ-15 ~ 15,��ʱָ���ֵ������lPriorityָ0 ~ 255�ľ���ֵ
//		IN flag - ָlPriority����������ֵ���Ǿ���ֵ
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�����߳����ȼ�
// ���ã�
//		ϵͳAPI
// ********************************************************************
#define DEBUG_SetThreadPriority 0
BOOL WINAPI KL_SetThreadPriority( HANDLE hThread, long lPriority, UINT flag )
{
	LPTHREAD lpThread;// = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );//2003-05-22, �޸�

	DEBUGMSG( DEBUG_SetThreadPriority, ( "KL_SetThreadPriority:hThread=0x%x.lPriority=0x%x,flag=0x%x.\r\n", hThread,lPriority,flag  ) );
	if( flag == 0 )
	{  // ���ֵ 
		if( lPriority >= -15 && lPriority <= 15)
		{		
			lPriority = DEF_PRIORITY - lPriority;//lPriorityΪ�����DEF_PRIORITY��ֵ
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
// ������int WINAPI KL_GetThreadPriority( HANDLE hThread, UINT flag )
// ������
//		IN hThread - �߳̾��
//		IN flag - ָlPriority����������ֵ���Ǿ���ֵ
// ����ֵ��
//		����ɹ����������ȼ�; ���򣬷��� THREAD_PRIORITY_ERROR_RETURN
// ����������
//		�õ��߳����ȼ�
// ���ã�
//		ϵͳAPI
// ********************************************************************
int WINAPI KL_GetThreadPriority( HANDLE hThread, UINT flag )
{
	LPTHREAD lpThread;

	if( hThread == CURRENT_THREAD_HANDLE )
		lpThread = lpCurThread;
	else
		lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD ); // �ɾ���õ��̶߳����ָ��
	
	if( lpThread )
	{
		int retv;
		if( flag == 0 )
		{  // �õ����ֵ���� -15 ~ 15 ֮��
		    retv = DEF_PRIORITY - lpThread->nOriginPriority;
			if( retv > 15 || retv < -15 )
				goto _error_return;
		}
		else  // ����ֵ
			retv = lpThread->nOriginPriority;
		return retv;
	}
_error_return:
    return THREAD_PRIORITY_ERROR_RETURN;
}
// ********************************************************************
// ������BOOL WINAPI KL_SetThreadPolicy( HANDLE hThread, UINT uiPolicy )
// ������
//		IN hThread - �߳̾��
//		IN uiPolicy - �̹߳���Ϊ����ֵ��
//				THREAD_POLICY_OTHER - Ĭ�ϵĲ���
//				THREAD_POLICY_ROTATION - ��ת���ȣ�ÿ�ε���ʱ����ǰ�����̶߳���ŵ����ж���β
//				THREAD_POLICY_FIFO - �Ƚ��ȳ�
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		���õ��Ȳ���
// ���ã�
//		ϵͳAPI
// ********************************************************************
BOOL WINAPI KL_SetThreadPolicy( HANDLE hThread, UINT uiPolicy )
{
	LPTHREAD lpThread;// = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );//2003-05-22, �޸�

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
// ������int WINAPI KL_GetThreadPolicy( HANDLE hThread )
// ������
//		IN hThread - �߳̾��
// ����ֵ��
//		����ֵ����Ϊ����ֵ
//			THREAD_POLICY_OTHER: ���߳���ϵͳ�����䶯̬���ȼ�����������ϵͳ�����Ƿ��ø�thread����CPU��ʹ��Ȩ
//	��������THREAD_POLICY_FIFO��ϵͳ���ή�͸��̵߳Ķ�̬���ȼ�����
//			THREAD_POLICY_ROTATION��ϵͳ���ή�͸��̵߳Ķ�̬���ȼ��������������̲߳����л�ʱ�����߳̽��طŵ�ͬ�����ȼ����еĶ�β
// ����������
//		�õ��̵߳��ȹ���
// ���ã�
//		ϵͳAPI
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
// ������DWORD WINAPI KL_ResumeThread( HANDLE hThread )
// ������
//		IN hThread - �߳̾��
// ����ֵ��
//		����ɹ�������֮ǰ�ļ���ֵ�����򣬷���-1
// ����������
//		�ָ��߳�ִ�У�����߳�֮ǰ�ж�ι��𣨵���SuspendThread���������������
//		ͬ�����ResumeThread���ܲ������߳��ɹ���̬תΪ����̬��
// ���ã�
//		ϵͳAPI
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
// ������DWORD WINAPI KL_SuspendThread( HANDLE hThread )
// ������
//		IN hThread - �߳̾��
// ����ֵ��
//		����ɹ�������֮ǰ�ļ���ֵ�����򣬷���-1
// ����������
//		�����̣߳�����̵߳�ǰ������̬����ִ�иù��ܺ��߳̽���תΪ����̬��
//		����߳�֮ǰ�ж�ι��𣨵���SuspendThread���������������
//		ͬ�����ResumeThread���ܲ������߳��ɹ���̬תΪ����̬��
// ���ã�
//		ϵͳAPI
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
// ������BOOL WINAPI KL_GetExitCodeThread( HANDLE hThread, LPDWORD lpExitCode )
// ������
//		IN hThread - �߳̾��
//		OUT lpExitCode - ���ڽ����߳��˳�����
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�õ��̵߳��˳����롣������̵߳�ǰû���˳����򷵻�STILL_ACTIVE������Ϊһ��ֵ֮һ��
//		ExitThread �� TerminateThread ��ֵ���̷߳���ֵ���̵߳Ľ��̵ķ���ֵ
// ���ã�
//		ϵͳAPI
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
// ������BOOL FASTCALL Thread_Close( HANDLE hThread )
// ������
//		IN hThread - �߳̾��
// ����ֵ��
//		����ɹ�,����TRUE�����򣬷���FALSE
// ����������
//		���û�����CloseHandleʱ������øú���ȥ�����ͷ��̶߳���Ĺ���
// ���ã�
//		hmgr.c 
// ********************************************************************
BOOL FASTCALL Thread_Close( HANDLE hThread )
{
	LPTHREAD lpThread = (LPTHREAD)HandleToPtr( hThread, OBJ_THREAD );  // �ɾ���õ��̶߳���ָ��

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
// ������BOOL WINAPI KL_SwitchToThread( VOID )
// ������
//		��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		��ǰ�߳���������CPU
// ���ã�
//		ϵͳAPI
// ********************************************************************
BOOL WINAPI KL_SwitchToThread( VOID )
{
    extern void CALLBACK Schedule(void);
	ScheduleTimeout(0);
	return TRUE;
}

// ********************************************************************
// ������void WINAPI KL_SetLastError( DWORD dwErrorCode )
// ������
//		IN dwErrorCode - �������
// ����ֵ��
//		��
// ����������
//		�����̵߳�ǰ�Ĵ������
// ���ã�
//		ϵͳAPI
// ********************************************************************
void WINAPI KL_SetLastError( DWORD dwErrorCode )
{
    lpCurThread->dwErrorCode = dwErrorCode;
}

// ********************************************************************
// ������DWORD WINAPI KL_GetLastError( void )
// ������
//		��
// ����ֵ��
//		�̵߳�ǰ�Ĵ������
// ����������
//		�õ��̵߳�ǰ�Ĵ������
// ���ã�
//		ϵͳAPI
// ********************************************************************
DWORD WINAPI KL_GetLastError( void )
{
	return lpCurThread->dwErrorCode;
}

// ********************************************************************
// ������DWORD WINAPI KL_TlsAlloc(VOID)
// ������
//		��
// ����ֵ��
//		����ɹ������ػ���0��TLS����ֵ�����򣬷���TLS_OUT_OF_INDEXES
// ����������
//		�ӵ�ǰ�̷߳���һ������Slot����ֵ��������̵߳ĵ�ǰ���̲��Ǹ��̵߳�ӵ���ߣ������ߣ�����ù���ʧ��
// ���ã�
//		ϵͳAPI
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
			dwOldMask = *lpdwMask;  // �õ���ǰ���̵�solt mask
			if( (dwOldMask & mask) == 0 )  // û��ʹ���� ��
			{	// û�У��õ���
				if( KL_InterlockedCompareExchange( (LPLONG)lpdwMask, dwOldMask | mask, dwOldMask ) == (LONG)dwOldMask )
				{   // �ɹ�
					return i;
				}
				else
				{  // ʧ�ܣ����¿�ʼ����
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
// ������void ClearAllThreadTlsValue( LPPROCESS lpProcess, DWORD dwTlsIndex )
// ������
//		IN lpProcess - ����ָ��
//		IN dwTlsIndex - TLS����ֵ
// ����ֵ��
//		��
// ����������
//		����ǰ���̵������̵߳�tlsֵ��Ϊ0
// ���ã�
//		ϵͳAPI
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
// ������BOOL WINAPI KL_TlsFree( DWORD dwTlsIndex )
// ������
//		IN  dwTlsIndex - TLS����ֵ
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FLASE
// ����������
//		�ͷ�һ��TLS����ֵ��������̵߳ĵ�ǰ���̲��Ǹ��̵߳�ӵ���ߣ������ߣ�����ù���ʧ��
// ���ã�
//		ϵͳAPI
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
			ClearAllThreadTlsValue(lpCurProcess, dwTlsIndex); // ������еĸ�slot��ֵ����Ϊ0��
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
// ������LPVOID DoTlsGetValue( LPTHREAD lpThread, DWORD dwTlsIndex )
// ������
//		IN lpThread - �߳̽ṹָ��
//		IN dwTlsIndex - slot����ֵ
// ����ֵ��
//		tlsֵ
// ����������
//		�õ�tlsֵ
// ���ã�
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
// ������BOOL DoTlsSetValue( LPTHREAD lpThread, DWORD dwTlsIndex, LPVOID lpValue )
// ������
//		IN lpThread - �߳̽ṹָ��
//		IN dwTlsIndex - slot����ֵ
//		IN lpValue - �µ�ֵ
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		����tlsֵ
// ���ã�
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
// ������LPVOID DoTlsGetValue( LPTHREAD lpThread, DWORD dwTlsIndex )
// ������
//		IN lpThread - �߳̽ṹָ��
//		IN dwTlsIndex - slot����ֵ
// ����ֵ��
//		tlsֵ
// ����������
//		�õ�tlsֵ��������̵߳ĵ�ǰ���̲��Ǹ��̵߳�ӵ���ߣ������ߣ�����ù���ʧ��
// ���ã�
//		ϵͳAPI	
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
// ������BOOL WINAPI KL_TlsSetValue( DWORD dwTlsIndex, LPVOID lpValue )
//		IN dwTlsIndex - slot����ֵ
//		IN lpValue - �µ�ֵ
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		����tlsֵ��������̵߳ĵ�ǰ���̲��Ǹ��̵߳�ӵ���ߣ������ߣ�����ù���ʧ��
// ���ã�
//		ϵͳAPI
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
// ������LPVOID WINAPI KL_TlsGetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex )
// ������
//		IN dwThreadId - �߳�ID
//		IN dwTlsIndex - TLS����
// ����ֵ��
//		֮ǰ��TlsSetValue���õ�ֵ
// ����������
//		�õ��̵߳�TLSֵ
// ���ã�
//		ϵͳAPI
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
// ������
//		IN dwThreadId - �߳�ID
//		IN dwTlsIndex - TLS����
//		IN lpvData - �µ�TLSֵ
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�����̵߳� TLS ֵ
// ���ã�
//		ϵͳAPI
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
// ������UINT WINAPI KL_LockSchedule( void )
// ������
//		��
// ����ֵ��
//		��ǰ��ס����
// ����������
//		��ס����������ֹ�������л��������߳�
//      ÿ���øù���һ�Σ���ס�������ͼ�һ����ÿ����Thread_UnlockScheduleһ�Σ���ס�������ͼ�һ��
//		����ס������Ϊ0ʱ�����������Ե��ȡ�����ס����������0ʱ������������ס�������κ��̲߳���
//		���ȵ���ֱ�����߳���������CPU��ʹ��Ȩ����Ϊ���̵߳ȴ���Դ������CPU��ʹ��Ȩ�������߳��ٴ�
//		ӵ����Դ�󣬵������ٴα���סֱ����ס������Ϊ0��
//		ֻ��Thread_LockSchedule��Thread_UnlockSchedule���ܿ��Ըı���ס��������ֵ��
// ���ã�
//		ϵͳAPI
// ********************************************************************
UINT WINAPI KL_LockSchedule( void )
{
	LOCK_SCHE();
	return lpCurThread->nLockScheCount; 
}

// ********************************************************************
// ������UINT WINAPI KL_UnlockSchedule( void )
// ������
//		��  
// ����ֵ��
//		��ǰ��ס����
// ����������
//		ÿ���øù���һ�Σ���ס�������ͼ�һ����ÿ����Thread_LockScheduleһ�Σ���ס�������ͼ�һ��
//		����ס������Ϊ0ʱ�����������Ե��ȡ�����ס����������0ʱ������������ס�������κ��̲߳���
//		���ȵ���ֱ�����߳���������CPU��ʹ��Ȩ����Ϊ���̵߳ȴ���Դ������CPU��ʹ��Ȩ�������߳���
//		��ӵ����Դ�󣬵������ٴα���סֱ����ס������Ϊ0��
//		ֻ��Thread_LockSchedule��Thread_UnlockSchedule���ܿ��Ըı���ס��������ֵ
//		
// ���ã�
//		ϵͳAPI
// ********************************************************************
UINT WINAPI KL_UnlockSchedule( void )
{
	UNLOCK_SCHE();
	return lpCurThread->nLockScheCount; 
}


// ********************************************************************
// ������UINT WINAPI KL_CaptureException( jmp_buf jmp_data )
// ������
//		IN jmp_data - EXCEPTION_CONTEXT �ṹ���ݣ������û����� 
// ����ֵ��
//		�������try ����飨block��������0������,���ش������
// ����������
//		�����߳������ģ�����try�����
//		���쳣����ʱ���˳������
// ���ã�
//		ϵͳAPI
// ********************************************************************
#define DEBUG_ENTRYTRY 0
UINT WINAPI KL_CaptureException( LPEXCEPTION_CONTEXT jmp_data )
{
	LPEXCEPTION lpexp = KHeap_Alloc( sizeof( EXCEPTION ) ); // �����쳣�ṹ
	
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
// ������VOID WINAPI LeaveException( BOOL bException )
// ������
//		IN bException - ��ǰ�Ƿ��쳣
// ����ֵ��
//		��
// ����������
//		���֮ǰ����TryEntry,����ص�TryEntry��
// ���ã�
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
	{	// ֮ǰ�е���TryEntry
		//�ȱ��浽��ʱbuf
		exp = *lpexp;
		// �Ƴ�����
		lpCurThread->lpException = lpexp->lpNext;
		//			
		UnlockIRQRestore( &uiSave );
		//�ͷ�֮ǰ���������
		KHeap_Free( lpexp, sizeof( EXCEPTION ) );
_LOOP:
		//��
		if( bException )
		{
			if( exp.lpCallStack )
			{	// �쳣��ϵͳ������ execption in sys call link
				// ���Ҳ��ͷ�
				LPCALLSTACK lpcsPrev, lpcs = lpCurThread->lpCallStack;
				lpcsPrev = lpcs;
				DEBUGMSG( DEBUG_LEAVE_EXCEPTION, ( "LeaveException: find lpcs.\r\n" ) );
				while( lpcs )
				{
					if( lpcs == exp.lpCallStack )
					{	//�ҵ�
						if( lpcs != lpcsPrev )
						{	//�ǵ�һ��
							lpcsPrev->lpNext = NULL;
							// �ͷŴ� lpCurThread->lpCallStack �� lpcsPrev �� callstack
							DEBUGMSG( DEBUG_LEAVE_EXCEPTION|1, ( "LeaveException: FreeThreadCallStack.\r\n" ) );
							FreeThreadCallStack( lpCurThread->lpCallStack );
							// ���˵�����
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
			//���ᵽ����
			goto _LOOP;
		}
	}
	else
		UnlockIRQRestore( &uiSave );
}


// ********************************************************************
// ������VOID WINAPI KL_ReleaseException( void )
// ������
//		��  
// ����ֵ��
//		��
// ����������
//		�����뿪��try ִ�п�
// ���ã�
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
			lpThread->nRotate = uCur;  // ��Ч
		else
		{	//��Ч
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
    

    // ������ӵ���߽���Ȩ�� �� ���ں��ܹ���ȡ
    AccessKey_Add( &dwNewPerm, (lpCurThread->lpOwnerProcess->akyAccessKey | InitKernelProcess.akyAccessKey) );
	lpcs = lpCurThread->lpCallStack;
    
    if ( lpcs == NULL || (lpcs->dwCallInfo & CALL_KERNEL) == 0 ) 
	{	//��ǰ�߳����ں�̬        
		dwOldPerm = lpCurThread->akyAccessKey;
		AccessKey_Set( &lpCurThread->akyAccessKey, dwNewPerm );
    } 
	else 
	{   // ϵͳ����, ������ʱ�����ø� ֵ
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
