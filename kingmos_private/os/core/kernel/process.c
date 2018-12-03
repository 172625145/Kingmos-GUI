/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：进程管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
	2004-12-30 SwitchBackProcess 去掉两句多余的代码 ？？
    2004-10-22 修改	LPPROCESS GetHandleOwner( void )

	2004-09-20: 去掉了在 SwitchToProcess 中的一段代码：
	            //dwProcessSlots[0] = (DWORD)lpProcess->lpSegIndex->lpSeg;
	            //SetCPUId( lpProcess->dwVirtualAddressBase );

    2003-09-09: DoCreateProcess 增加对参数的检查
    2003-09-02: 将Application Name and lpCmdLine 放到 mainthread stack
    2003-05-23: LN, GetCallerProcess, 增加对　lpCurProcess的判断
    2003-05-22-增加对 CURRENT_PROCESS_HANDLE 的处理
    2003-05-21: 将 Process结构的 lpSegment 改为 lpSegIndex
    2003-05-19: 将CreateModule 放到后面
    2003-05-7:LN修改MODULE 结构
    2003-04-30:LN,为可执行（exe）增加接口
	           删除无用的代码
******************************************************/

#include <eframe.h>
#include <eprogram.h>
#include <efile.h>

#include <epheap.h>
#include <epalloc.h>
#include <epcore.h>
#include <coresrv.h>

extern LPTHREAD FASTCALL _CreateThread(
									   HANDLE hProcess,
									   LPSECURITY_ATTRIBUTES lpThreadAttributes,   // 
									   DWORD dwStackSize,                      // 
									   DWORD dwPreservStackSize,
									   LPTHREAD_START_ROUTINE lpStartAddress, // 
									   LPVOID lpParameter,                // 
									   DWORD dwCreationFlags,         // 
									   LPDWORD lpThreadId         // 
									   );




// ********************************************************************
//声明：static DWORD WINAPI MainProcessProc( LPVOID lParam ) 
//参数：
//	IN lParam - 进程结构指针，指向当前进程
//返回值：
//	不返回
//功能描述：
//	该函数是创建的新进程的起始入口，也是进程的内核态，同时为该进程初始化一个默认的堆
//  该进程将会将调用模块加载器（负责将进程模块装入内存并运行）
//引用：
// ********************************************************************

#define DEBUG_MAINPROCESSPROC 0
static DWORD WINAPI MainProcessProc( LPVOID lParam )
{
	extern HANDLE DoHeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize );
	DWORD dwExitCode = -1;
    LPPROCESS lpProcess = (LPPROCESS)lParam;

	RETAILMSG( DEBUG_MAINPROCESSPROC, ( "Load MainProcessProc=%s,id=%d,VMB=0x%x.\r\n", lpProcess->lpszApplicationName, lpProcess->dwProcessId, lpProcess->dwVirtualAddressBase ) );

	// 初始化一个默认的进程堆
#ifdef VIRTUAL_MEM
	lpProcess->lpHeap = DoHeapCreate( HEAP_VIRTUAL_ALLOC, 0, 0 );// &lpProcess->heap );
#else
	lpProcess->lpHeap = DoHeapCreate( 0, 0, 0 );// &lpProcess->heap );
#endif


	RETAILMSG( DEBUG_MAINPROCESSPROC, ( "MainProcessProc:call module exe.\r\n" ) );
	
	// 调用模块加载器
	dwExitCode = lpProcess->pModule->lpmd->lpLoadModule( lpProcess->pModule->hModuleData, lpProcess->pModule->hModule, lpProcess->lpszCommandLine, LF_DEFLOAD );//LN:2003-05-07, Add
	//

	while( 1 )
		KL_ExitProcess( dwExitCode );
	// 永不返回
}

// ********************************************************************
//声明：static DWORD AllocProcessId( void ) 
//参数：
//	无
//返回值：
//	假如成功，返回进程ID；否则，返回0
//功能描述：
//	分配一个进程ID
//引用：
// ********************************************************************

static DWORD AllocProcessId( void )
{
	DWORD i;
	// 0 slots is reserver
	for( i = 1; i < MAX_PROCESSES; i++ )
	{
		if( KL_InterlockedCompareExchange( (LPLONG)&lppProcessSegmentSlots[i], 1, 0 ) == 0 )
		{
			return i;
		}
	}
	return 0;
}

// ********************************************************************
//声明：static BOOL FreeProcessId( DWORD dwProcessId ) 
//参数：
//	IN dwProcessId - 进程ID
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	释放一个进程ID
//引用：
// ********************************************************************

static BOOL FreeProcessId( DWORD dwProcessId )
{
	if( dwProcessId < MAX_PROCESSES && dwProcessId > 0 )
	{	
		UINT uiSave;
		LockIRQSave( &uiSave );

		lppProcessSegmentSlots[dwProcessId] = 0;
		lppProcessPtr[dwProcessId] = 0;

	    //if( KL_InterlockedCompareExchange( (LPLONG)&dwProcessSlots[dwProcessId], 0, dwProcessSlots[dwProcessId] ) )
		//{
			//dwProcessPtr[dwProcessId] = 0;
		//}
		UnlockIRQRestore( &uiSave );
	}
	return FALSE;
}

//LN:2003-05-12, Add	
#ifdef VIRTUAL_MEM
// ********************************************************************
//声明：static LPPROCESS_SEGMENTS AllocSegmentIndexAndSegment( DWORD dwSegBaseAdr ) 
//参数：
//	IN dwSegBaseAdr - 该段对应的基地址
//返回值：
//	假如成功，返回一个段索引结构指针；否则，返回NULL
//功能描述：
//	分配一个段索引和段结构（段索引用于连接其它段，段结构用于描述/记录32M的进程空间使用情况）
//引用：
// ********************************************************************

#define CACHE_PAGES 2
#define CACHE_PAGES_SIZE (1024*CACHE_PAGES)
#define DEBUG_AllocSegmentIndexAndSegment 0
LPPROCESS_SEGMENTS AllocSegmentIndexAndSegment( DWORD dwSegBaseAdr )
{	// 分配一个段索引
	LPPROCESS_SEGMENTS lpSegIndex = KHeap_Alloc( sizeof(PROCESS_SEGMENTS) );

	if( lpSegIndex )
	{   // 分配一个段结构
		memset( lpSegIndex, 0, sizeof(PROCESS_SEGMENTS) );

		if( (lpSegIndex->lpSeg = Seg_Alloc()) )
		{
			lpSegIndex->lpdwSecondPageTable = KHeap_Alloc( CACHE_PAGES_SIZE );
			//return lpSegIndex;
			if( lpSegIndex->lpdwSecondPageTable )
			{
				lpSegIndex->lpdwPhySecondPageTable = (LPDWORD)( (DWORD)_GetPhysicalAdr( lpSegIndex->lpdwSecondPageTable ) | 1 );
				lpSegIndex->lpdwSecondPageTable = (LPDWORD)CACHE_TO_UNCACHE(lpSegIndex->lpdwSecondPageTable);				
				lpSegIndex->dwSegBaseAddress = dwSegBaseAdr;
				lpSegIndex->uiPageTableCount = CACHE_PAGES;

				memset( lpSegIndex->lpdwSecondPageTable, 0, CACHE_PAGES_SIZE );
				DEBUGMSG( DEBUG_AllocSegmentIndexAndSegment, ( "dwSegBaseAdr=0x%x,lpdwPhySecondPageTable=0x%x.\r\n", dwSegBaseAdr, lpSegIndex->lpdwPhySecondPageTable ) );
				 
				return lpSegIndex;
			}
		}
		if( lpSegIndex->lpSeg )
		    Seg_Free( lpSegIndex->lpSeg );

		KHeap_Free( lpSegIndex, sizeof(PROCESS_SEGMENTS) );
		lpSegIndex = NULL;
	}
	return lpSegIndex;
}
// ********************************************************************
//声明： static void FreeSegmentIndexAndSegment( LPPROCESS_SEGMENTS lpSegIndex )
//参数：
//	IN lpSegIndex - 段索引结构指针
//返回值：
//	无
//功能描述：
//	与AllocSegmentIndexAndSegment相对应，释放一个段索引结构指针及其段空间
//引用：
// ********************************************************************

static void FreeSegmentIndexAndSegment( LPPROCESS_SEGMENTS lpSegIndex )
{
	ASSERT( lpSegIndex->lpSeg->lpBlks[0] == NULL );//每个段的前64k是保留不用的，必须是NULL

	if( lpSegIndex->lpdwSecondPageTable )
	{
		LPDWORD lpT = (LPDWORD)UNCACHE_TO_CACHE( lpSegIndex->lpdwSecondPageTable );
		lpSegIndex->lpdwSecondPageTable = NULL;
	    KHeap_Free( lpT, CACHE_PAGES_SIZE );
	}
	Seg_Free( lpSegIndex->lpSeg );
	KHeap_Free( lpSegIndex, sizeof(PROCESS_SEGMENTS) );
}

// ********************************************************************
//声明： static void FreeProcessSegmentIndexAndSegment( LPPROCESS_SEGMENTS lpSegIndex  )
//参数：
//	IN lpSegIndex - 段索引结构指针
//返回值：
//	无
//功能描述：
//	释放进程的所有段（一个进程段用于描述/记录32M的进程空间使用情况）
//引用：
// ********************************************************************

static void FreeProcessSegmentIndexAndSegment( LPPROCESS_SEGMENTS lpSegIndex  )
{
	LPPROCESS_SEGMENTS lpSegNext;
	while( lpSegIndex )
	{
		lpSegNext = lpSegIndex->lpNext;
		FreeSegmentIndexAndSegment( lpSegIndex );
		lpSegIndex = lpSegNext;
	}
}
#endif  //VIRTUAL_MEM

// ********************************************************************
//声明： void FreeProcessObject( LPPROCESS lpProcess )
//参数：
//	IN lpProcess - PROCESS结构指针
//返回值：
//	无
//功能描述：
//	释放进程结构，当调用该函数时，不在该进程空间（该进程已经死了）
//引用：
//  当创建进程失败或进程已经死了时会调用该函数
// ********************************************************************

void FreeProcessObject( LPPROCESS lpProcess )
{
	DWORD dwProcessId = lpProcess->dwProcessId;
	if( lpProcess->lpMainThread )
		;// ????
	// 释放模块的数据
	if( lpProcess->pModule )
		Module_DeInit( lpProcess->pModule );

#ifdef VIRTUAL_MEM
	// 释放进程的段
	if( lpProcess->lpProcessSegments )
	{
		VOID * lpT = lpProcess->lpProcessSegments;
		lpProcess->lpProcessSegments = NULL;

		FreeProcessSegmentIndexAndSegment( lpT );
	}
#endif

    // 释放进程句柄
	if( lpProcess->hProcess )
        Handle_Free( lpProcess->hProcess, TRUE );
	if( lpProcess->lpCpuPTS )
		FreeCPUPTS( lpProcess->lpCpuPTS );

	lpProcess->objType = OBJ_NULL;
    // 释放进程结构
    KHeap_Free( lpProcess, sizeof( PROCESS ) );
    // 释放进程ID
    if( dwProcessId )
	    FreeProcessId( dwProcessId );

}

// ********************************************************************
//声明：BOOL FASTCALL DoCreateProcess(
//						   LPCTSTR lpszApplicationName,
//						   LPCTSTR lpszCommandLine, 
//						   LPSECURITY_ATTRIBUTES lpProcessAttributes,
//						   LPSECURITY_ATTRIBUTES lpThreadAttributes,
//						   BOOL bInheritHandles,
//						   DWORD dwCreationFlags,
//						   LPVOID lpEnvironment,
//						   LPCTSTR lpCurrentDirectory,
//						   LPSTARTUPINFO lpStartupInfo,
//						   LPPROCESS_INFORMATION lpProcessInformation,
//						   PCMODULE_DRV pmd
//						   )
//参数：
//	   IN lpszApplicationName - 可执行模块名
//	   IN lpszCommandLine - 传给WinMain的参数
//	   IN lpProcessAttributes - 进程安全属性
//	   IN lpThreadAttributes - 线程安全属性
//	   IN bInheritHandles - 是否继承父进程的句柄，必须为FALSE
//	   IN dwCreationFlags - 创建标志：
//							CREATE_SUSPENDED - 创建后挂起
//	   IN lpEnvironment - 环境块（必须为NULL）
//	   IN lpCurrentDirectory - 进程运行的当前目录（必须为NULL）
//	   IN lpStartupInfo - 启动信息（必须为NULL）
//	   OUT lpProcessInformation - PROCESS_INFORMATION结构指针（可以为NULL），用于接受进程的某些信息
//	   IN pmd - 该进程模块所对应的加载器
//返回值：
//	假如成功，返回TRUE，如果lpProcessInformation不为NULL，回返回进程信息；
//  否则，返回FALSE。
//功能描述：
//	创建新的进程
//引用：
//	KL_CreateProcess & KL_LoadApplication
// ********************************************************************

#define DEBUG_DOCREATEPROCESS 0
BOOL FASTCALL DoCreateProcess(
						   LPCTSTR lpszApplicationName,
						   LPCTSTR lpszCommandLine,
						   LPSECURITY_ATTRIBUTES lpProcessAttributes,
						   LPSECURITY_ATTRIBUTES lpThreadAttributes,
						   BOOL bInheritHandles,
						   DWORD dwCreationFlags,
						   LPVOID lpEnvironment,
						   LPCTSTR lpCurrentDirectory,
						   LPSTARTUPINFO lpStartupInfo,
						   LPPROCESS_INFORMATION lpProcessInformation,
						   PCMODULE_DRV pmd
						   )
{

    DWORD id = 0;
	LPPROCESS lpProcess = NULL;
	int iCmdLineLen;
	ACCESS_KEY akySave;

	if( lpszApplicationName == NULL ||
		*lpszApplicationName == 0 )
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	//
    DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:%s.\r\n", lpszApplicationName ) );
	if( lpszCommandLine == 0 )
	{
		lpszCommandLine = "";
	}

	iCmdLineLen = strlen( lpszCommandLine ) + strlen( lpszApplicationName ) + 2; // one null end, one space
	// 分配进程结构
    DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:alloc process struct.\r\n" ) );
	lpProcess = KHeap_Alloc( sizeof( PROCESS ) );
	if( lpProcess )
	{   
		memset( lpProcess, 0, sizeof( PROCESS ) );
		// 分配进程ID
		if( ( lpProcess->dwProcessId = AllocProcessId() ) == 0 )
		    goto _alloc_error;
		// 分配进程句柄
		DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:alloc process handle.\r\n" ) );
		if( ( lpProcess->hProcess = Handle_Alloc( lpProcess, lpProcess, OBJ_PROCESS ) ) == NULL )
			goto _alloc_error;

#ifdef VIRTUAL_MEM
		// 分配一个段
		if( ( lpProcess->lpProcessSegments = AllocSegmentIndexAndSegment(lpProcess->dwProcessId << SEGMENT_SHIFT) ) == NULL )
			goto _alloc_error;
		lppProcessSegmentSlots[lpProcess->dwProcessId] = lpProcess->lpProcessSegments;//->lpSeg;
		//lpProcess->lpProcessSegments->dwSegBaseAddress = lpProcess->dwProcessId << SEGMENT_SHIFT;
#else  
		//dwProcessSlots[lpProcess->dwProcessId] = NULL; //2004-09-21，lilin 没必要		
#endif  //VIRTUAL_MEM

        

		
		if( (lpProcess->lpCpuPTS = AllocCPUPTS()) == NULL )
			goto _alloc_error;


		DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:call Module_Init.\r\n" ) );

		
		// 分配模块加载器结构
		if( (lpProcess->pModule = Module_Init( lpProcess, lpszApplicationName, pmd, CREATE_EXE )) == NULL )	
			goto _alloc_error;
		//
		
		// 初始化其他进程数据
		lpProcess->objType = OBJ_PROCESS;
		lpProcess->dwtlsMask = TLS_MASK;
#ifdef VIRTUAL_MEM
		lpProcess->dwVirtualAddressBase = lpProcess->lpProcessSegments->dwSegBaseAddress;
#else
		lpProcess->dwVirtualAddressBase = 0;
#endif

		//
		lpProcess->akyAccessKey = (0x1 << lpProcess->dwProcessId);
		//test only
//		lpProcess->akyAccessKey = -1;
		//
		
		
		lpProcess->lpszApplicationName = pmd->lpGetModuleFileName( lpProcess->pModule->hModuleData );//pModule->lpszApName;//LN:2003-05-07, Add
		
		//

		//lpProcess->dwCreationFlags = dwCreationFlags;
		lpProcess->dwFlags = (dwCreationFlags & CREATE_SYSTEM_MODE) ? M_SYSTEM : M_USER;
		RETAILMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess: lpProcess->dwFlags=0x%x.\r\n", lpProcess->dwFlags ) );
		akySave = lpCurThread->akyAccessKey;
		AccessKey_Add( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
		// 创建主线程并挂起它
		lpProcess->lpMainThread = _CreateThread( lpProcess, 
			                                     lpThreadAttributes, 
												 0, //必须用0（默认），因为主线程的stack在64k-128k
												 iCmdLineLen,
												 MainProcessProc, 
												 lpProcess, 
												 dwCreationFlags | CREATE_SUSPENDED | CREATE_MAINTHREAD, 
												 NULL );
		// 
		DEBUGMSG( DEBUG_DOCREATEPROCESS, (TEXT("DoCreateProcess:lpMainThread=%x.\r\n"), lpProcess->lpMainThread ) );

		if( lpProcess->lpMainThread  )
		{
			// 命令行参数放到用户栈里
			lpProcess->lpszCommandLine = (LPTSTR)lpProcess->lpMainThread->lpdwThreadStack + lpProcess->lpMainThread->dwThreadStackSize - iCmdLineLen;
			strcpy( (char*)lpProcess->lpszCommandLine, (char*)lpszApplicationName );
			if( *lpszCommandLine )
			{
			    strcat( (char*)lpProcess->lpszCommandLine, " " );
			    strcat( (char*)lpProcess->lpszCommandLine, (char*)lpszCommandLine );
			}
			lppProcessPtr[lpProcess->dwProcessId] = lpProcess;
			DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:check suspend.\r\n" ) );
			// 唤醒主线程
			if( (dwCreationFlags & CREATE_SUSPENDED) == 0 )
			    //_WakeupThread( lpProcess->lpMainThread, FALSE );
				DoResumeThread(lpProcess->lpMainThread);

			DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:set lpProcessInformation.\r\n" ) );
			// 假如可能，设置进程信息结构
			if( lpProcessInformation )
			{
				lpProcessInformation->hProcess = lpProcess->hProcess;
				lpProcessInformation->hThread = lpProcess->lpMainThread->hThread;
				lpProcessInformation->dwProcessId = lpProcess->dwProcessId;
				lpProcessInformation->dwThreadId = lpProcess->lpMainThread->dwThreadId;
			}
//			AccessKey_Remove( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
			AccessKey_Set( &lpCurThread->akyAccessKey, akySave );
			
	        goto _ret;
		}
		else
		{
			RETAILMSG( 1, ( "error: KL_CreateProcess: Create Main Thread failure!!!!.\r\n" ) );
		}
//		AccessKey_Remove( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
		AccessKey_Set( &lpCurThread->akyAccessKey, akySave );
	}
_alloc_error:
    RETAILMSG( 1, ( "error : KL_CreateProcess failure:%s.\r\n", lpszApplicationName ) );
	// 处理错误
    if( lpProcess )
    {
		FreeProcessObject( lpProcess );
	}
	return FALSE;
_ret:
    return lpProcess ? TRUE : FALSE;
}

// ********************************************************************
//声明：BOOL WINAPI KL_CreateProcess(
//						   LPCTSTR lpszApplicationName,                 // name of executable module
//						   LPCTSTR lpszCommandLine,                      // command line string
//						   LPSECURITY_ATTRIBUTES lpProcessAttributes, // SD
//						   LPSECURITY_ATTRIBUTES lpThreadAttributes,  // SD
//						   BOOL bInheritHandles,                      // handle inheritance option
//						   DWORD dwCreationFlags,                     // creation flags
//						   LPVOID lpEnvironment,                      // new environment block
//						   LPCTSTR lpCurrentDirectory,                // current directory name
//						   LPSTARTUPINFO lpStartupInfo,               // startup information
//						   LPPROCESS_INFORMATION lpProcessInformation // process information
//						   )
 
//参数：
//	   IN lpszApplicationName - 可执行模块名
//	   IN lpszCommandLine - 传给WinMain的参数
//	   IN lpProcessAttributes - 进程安全属性
//	   IN lpThreadAttributes - 线程安全属性
//	   IN bInheritHandles - 是否继承父进程的句柄，必须为FALSE
//	   IN dwCreationFlags - 创建标志：
//							CREATE_SUSPENDED - 创建后挂起
//	   IN lpEnvironment - 环境块（必须为NULL）
//	   IN lpCurrentDirectory - 进程运行的当前目录（必须为NULL）
//	   IN lpStartupInfo - 启动信息（必须为NULL）
//	   OUT lpProcessInformation - PROCESS_INFORMATION结构指针（可以为NULL），用于接受进程的某些信息
//返回值：
//	假如成功，返回TRUE，如果lpProcessInformation不为NULL，回返回进程信息；
//  否则，返回FALSE。
//功能描述：
//	创建新的进程
//引用：
//	系统调用
// ********************************************************************

BOOL WINAPI KL_CreateProcess(
						   LPCTSTR lpszApplicationName, 
						   LPCTSTR lpszCommandLine,
						   LPSECURITY_ATTRIBUTES lpProcessAttributes,
						   LPSECURITY_ATTRIBUTES lpThreadAttributes,
						   BOOL bInheritHandles,
						   DWORD dwCreationFlags,
						   LPVOID lpEnvironment,
						   LPCTSTR lpCurrentDirectory,
						   LPSTARTUPINFO lpStartupInfo,
						   LPPROCESS_INFORMATION lpProcessInformation
						   )
{
#ifndef INLINE_PROGRAM
	// future , i will find module format.
	//
	extern MODULE_DRV ElfModule;
	return DoCreateProcess( lpszApplicationName, 
		                    lpszCommandLine, 
							lpProcessAttributes, 
							lpThreadAttributes, 
							bInheritHandles,
							dwCreationFlags,
							lpEnvironment,
							lpCurrentDirectory,
							lpStartupInfo,
							lpProcessInformation,
							&ElfModule );
#endif
	return FALSE;

}


// ********************************************************************
//声明： BOOL FASTCALL Process_Close( HANDLE hProcess )
//参数：
//	IN hProcess - 进程句柄
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	释放进程句柄
//引用：
//	CloseHandle
// ********************************************************************

BOOL FASTCALL Process_Close( HANDLE hProcess )
{
	LPPROCESS lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
	lpProcess->hProcess = NULL;
	return TRUE;
}

// ********************************************************************
//声明： VOID WINAPI KL_ExitProcess( UINT uExitCode )
//参数：
//	IN uExitCode - 退出代码
//返回值：
//	无
//功能描述：
//	退出进程
//引用：
//	系统API
// ********************************************************************
extern void KillAllChildThreads( LPPROCESS );
#define DEBUG_EXITPROCESS 0
VOID WINAPI KL_ExitProcess(
						 UINT uExitCode   // exit code for all threads
						 )
{
    extern void KillAllOtherThreads( LPPROCESS lpProcess );
	LPPROCESS lpProcess;
    
	ASSERT( lpCurThread->lpCurProcess == lpCurThread->lpOwnerProcess );//
	// 得到当前进程，current process
	lpProcess = lpCurThread->lpCurProcess;//GetAPICallerProcessPtr();
	DEBUGMSG( DEBUG_EXITPROCESS, ( "KL_ExitProcess:Exit Process(=%s) entry.\r\n", lpProcess->lpszApplicationName ) );

#if 0	// test
	{
		LPCALLSTACK lpcs = lpCurThread->lpCallStack;
		while( lpcs )
		{
			CALLSTACK * lpNext = lpcs->lpNext;
			
			RETAILMSG( 1, ( "lpcs:(0x%x).\r\n", lpcs ) );
			lpcs = lpNext;
		}
	}
#endif
	//
	// 将退出信息保存到进程句柄里
	SetHandleUserInfo( lpProcess->hProcess, uExitCode );
	lpCurThread->dwExitCode = uExitCode; 
	// 
	DEBUGMSG( DEBUG_EXITPROCESS, ( "KL_ExitProcess:KillAllChildThreads.\r\n" ) );
	while( 1 )
	{	//判断是否有更多的子线程
		// 将该进程拥有的其它线程杀死
		KillAllOtherThreads( lpProcess );
		if( lpProcess->lpFirstThread->lpNextThreadInProcess == NULL )
			HandleThreadExit();
		else
			KL_Sleep( 3000 );
		WARNMSG( DEBUG_EXITPROCESS, ( "KL_ExitProcess:wait for child thread exit,ProcName=%s.\r\n", lpProcess->lpszApplicationName ) );
		//ASSERT( 0 );
	}
    //  not return here
}

// ********************************************************************
//声明： VOID WINAPI KL_ExitProcess( UINT uExitCode )
//参数：
//	IN uExitCode - 退出代码
//返回值：
//	无
//功能描述：
//	退出进程
//引用：
//	系统API
// ********************************************************************
//extern void KillAllChildThreads( LPPROCESS );
//#define DEBUG_EXITPROCESS 1
VOID WINAPI KL_TerminateProcess(
						 HANDLE hProcess,
						 UINT uExitCode   // exit code for all threads
						 )
{
    extern void KillAllOtherThreads( LPPROCESS lpProcess );
	LPPROCESS lpProcess;
    
	if( hProcess == CURRENT_PROCESS_HANDLE )
		hProcess = lpCurThread->lpCurProcess->hProcess;

	lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
	if( lpProcess )
	{
		if( lpCurThread->lpCurProcess == lpProcess  )
		{
			KL_ExitProcess(uExitCode);
		}
		else
		{
			KL_TerminateThread( lpProcess->lpMainThread->hThread, uExitCode );
		}
	}
    //  not return here
}


// ********************************************************************
//声明： HANDLE WINAPI KL_GetProcessHeap( void )
//参数：
//	无
//返回值：
//	假如成功，返回进程的默认堆句柄；否则，返回NULL
//功能描述：
//	返回进程的默认堆
//引用：
//	系统API
// ********************************************************************

HANDLE WINAPI KL_GetProcessHeap( void )
{
	LPPROCESS lpProcess;
	lpProcess = GetAPICallerProcessPtr();
	if( lpProcess )
		return lpProcess->lpHeap;
	else
		return NULL;
}

// ********************************************************************
//声明：DWORD WINAPI KL_GetCurrentProcessId( void ) 
//参数：
//	无
//返回值：
//	返回进程id
//功能描述：
//	得到当前的进程id
//引用：
//	系统API
// ********************************************************************

DWORD WINAPI KL_GetCurrentProcessId( void )
{
	return GetAPICallerProcessPtr()->dwProcessId;
	
}

// ********************************************************************
//声明：BOOL WINAPI KL_GetExitCodeProcess( HANDLE hProcess, LPDWORD lpdwExitCode ) 
//参数：
//	IN hProcess - 进程句柄
//	OUT lpdwExitCode - 用于接受进程退出代码
//返回值：
//	假如成功，返回TRUE，lpdwExitCode保存退出代码；否则，返回FALSE
//功能描述：
//	得到当前进程的退出代码
//引用：
//	系统API
// ********************************************************************

BOOL WINAPI KL_GetExitCodeProcess( HANDLE hProcess, LPDWORD lpdwExitCode )
{
	LPPROCESS lpProcess;
	if( hProcess == CURRENT_PROCESS_HANDLE )//2003-05-22, ADD
	{
		*lpdwExitCode = STILL_ACTIVE;
		return TRUE;
	}
	lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
	if( lpProcess )
	{
		*lpdwExitCode = STILL_ACTIVE;
		return TRUE;
	}
	else
	{	// 进程已死
	    return GetHandleUserInfo( hProcess, lpdwExitCode );
	}
}

// ********************************************************************
//声明： LPVOID WINAPI KL_MapPtrToProcess( LPVOID lpv, HANDLE hProcess )
//参数：
//	IN lpv - 需要映射的指针
//	IN hProcess - 指针指向的地址所属的进程的句柄
//返回值：
//	假如成功，返回映射后的指针；否则，返回NULL
//功能描述：
//	映射进程的指针，使得该指针能被其他进程访问
//引用：
// ********************************************************************

LPVOID WINAPI KL_MapPtrToProcess( LPVOID lpv, HANDLE hProcess )
{
#ifdef VIRTUAL_MEM
	LPPROCESS lpProcess;

	if( hProcess == CURRENT_PROCESS_HANDLE )
		lpProcess = lpCurThread->lpCurProcess;
	else
		lpProcess = (LPPROCESS)HandleToPtr( hProcess, OBJ_PROCESS );
	// 映射指针
    if( lpProcess )
		lpv = MapProcessPtr( lpv, lpProcess );
    else 
		lpv = 0;
    return lpv;

#else
	return lpv;
#endif
}

// ********************************************************************
//声明：LPVOID WINAPI KL_UnMapPtr( LPVOID lpv ) 
//参数：
//	IN lpv - 之前被映射的指针
//返回值：
//	假如成功，返回无映射的指针；否则，返回NULL
//功能描述：
//	与KL_MapPtrToProcess对应， 将之前被映射指针还原
//引用：
//	系统API
// ********************************************************************

LPVOID WINAPI KL_UnMapPtr( LPVOID lpv )
{
	return UnMapProcessPtr( lpv );
}

// ********************************************************************
//声明： HANDLE WINAPI KL_GetCurrentProcess( void )
//参数：
//	无
//返回值：
//	返回当前进程的别名
//功能描述：
//	得到当前进程句柄
//引用：
//	系统API
// ********************************************************************

HANDLE WINAPI KL_GetCurrentProcess( void )
{
	//return GetAPICallerProcessPtr()->hProcess;//2003-05-22, DEL
	return CURRENT_PROCESS_HANDLE;//2003-05-22, ADD
}

// ********************************************************************
//声明：HANDLE WINAPI KL_GetOwnerProcess( void ) 
//参数：
//	无
//返回值：
//	假如成功，返回当前线程的拥有者进程（创建该线程的进程）
//功能描述：
//	得到当前线程的拥有者进程（创建该线程的进程）
//引用：
//  系统API
// ********************************************************************

HANDLE WINAPI KL_GetOwnerProcess( void )
{
    return lpCurThread->lpOwnerProcess->hProcess;//
}

// ********************************************************************
//声明： LPPROCESS GetHandleOwner( void )
//参数：
//	无
//返回值：
//	拥有者进程结构指针
//功能描述：
//	得到句柄的拥有者
//引用：
//  句柄管理器(hmgr.c)
// ********************************************************************
/* 2004-10-22
LPPROCESS GetHandleOwner( void )
{
    CALLSTACK * lpcs = lpCurThread->lpCallStack;	

	if( lpcs )
	    return (LPPROCESS)lpcs->lpvData;
	else
		return lpCurThread->lpCurProcess;  // the must kernel
}
*/
LPPROCESS GetHandleOwner( void )
{
    CALLSTACK * lpcs = lpCurThread->lpCallStack;	
	while( lpcs && (lpcs->dwCallInfo & CALL_KERNEL) )
		lpcs = lpcs->lpNext;
	return lpcs ? (LPPROCESS)lpcs->lpvData : NULL;
    //return lpcs ? ( (LPPROCESS)lpcs->lpvData )->hProcess : NULL;
/* origin
    CALLSTACK * lpcs = lpCurThread->lpCallStack;	

	if( lpcs )
	    return (LPPROCESS)lpcs->lpvData;
	else
		return lpCurThread->lpCurProcess;  // the must kernel
		*/
}


// ********************************************************************
//声明：BOOL SwitchToProcess( LPPROCESS lpProcess, LPCALLSTACK lpcs )
//参数：
//	IN  lpProcess - 进程结构指针
//	OUT lpcs - 用于保存调用上下文的CALLSTACK结构, 可以为NULL
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	切换到新的进程空间
//引用：
//	进行API调用时会用到该函数，当用户调用API时，首先会切换到API服务进程空间，
//	当API执行完后，将会恢复到用户空间
// ********************************************************************

BOOL SwitchToProcess( LPPROCESS lpProcess, LPCALLSTACK lpcs )
{
	//LPPROCESS lpProcess;
	UINT uiSave;
	BOOL bRetv = FALSE;
	
	//ASSERT( lpProcess );

	if( lpProcess == &InitKernelProcess )//KERNEL_PROCESS_HANDLE )
	{   // 切换到内核
		if( lpcs )
		{
			LockIRQSave( &uiSave );
			
			lpcs->lpvData = lpCurThread->lpCurProcess;
			lpcs->lpNext = lpCurThread->lpCallStack;
			lpcs->dwCallInfo = CALL_KERNEL;
			AccessKey_Set( &lpcs->akyAccessKey, lpCurThread->akyAccessKey );
			lpCurThread->lpCallStack = lpcs;			
			
			UnlockIRQRestore( &uiSave );
		}
		
		bRetv = TRUE;
	}
	else
	{	// 切换到进程
		//lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
		//if( lpProcess )
		{   
			BOOL bSwitchContext;
//			ASSERT( lpCurThread->lpCurProcess  );
	
			// 如果新的进程与线程的当前进程不同，则需要切换进程空间
			if( lpProcess != lpCurThread->lpCurProcess )
				bSwitchContext = TRUE;
			else
				bSwitchContext = FALSE;

			LockIRQSave( &uiSave );

			if( lpcs )
			{				
				// 设置CALLSTACK结构
				lpcs->dwCallInfo = 0;
				lpcs->lpvData = lpCurThread->lpCurProcess;
				AccessKey_Set( &lpcs->akyAccessKey, lpCurThread->akyAccessKey );

				// only test
				//if( lpcs )
				//{
				//	DWORD myAky = lpCurThread->lpCurProcess->akyAccessKey | lpCurThread->lpOwnerProcess->akyAccessKey;
				//	if( (lpcs->akyAccessKey & myAky) != myAky )
				//	{
				//		RETAILMSG( 1, ( "error: lpcs->akyAccessKey=0x%x,myAky=0x%x,CurProc=0x%x,OwnerProc=0x%x.\r\n",lpcs->akyAccessKey,myAky,lpCurThread->lpCurProcess->akyAccessKey,lpCurThread->lpOwnerProcess->akyAccessKey ) );
				//	}
				//}
				//


				// 保存调用链
				lpcs->lpNext = lpCurThread->lpCallStack;		
				lpCurThread->lpCallStack = lpcs;
			}
			// 将lpProcess设为当前线程的当前进程
		//	lpCurThread->lpCurProcess = lpProcess;




			if( bSwitchContext )
			{
//#ifdef VIRTUAL_MEM
				//装入当前进程的MMU有关数据，无效CACHE。。。
				//2004-09-20，ln.?????-在 GetMMUContext 中也有这段代码，现在看来好象是没有必要的
				//有可能是但是为了解决一个错误而加的。
	           // dwProcessSlots[0] = (DWORD)lpProcess->lpSegIndex->lpSeg;
	           // SetCPUId( lpProcess->dwVirtualAddressBase );
				//?????
				AccessKey_Add( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
			    GetMMUContext( lpCurThread, 1, lpProcess );
//#endif
            }
	        
			UnlockIRQRestore( &uiSave );

			bRetv = TRUE;
		}
	}
	return bRetv;
}

// ********************************************************************
//声明：BOOL SwitchToProcessByHandle( LPPROCESS lpProcess, LPCALLSTACK lpcs )
//参数：
//	IN  lpProcess - 进程结构指针
//	OUT lpcs - 用于保存调用上下文的CALLSTACK结构, 可以为NULL
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	切换到新的进程空间
//引用：
//	进行API调用时会用到该函数，当用户调用API时，首先会切换到API服务进程空间，
//	当API执行完后，将会恢复到用户空间
// ********************************************************************

BOOL SwitchToProcessByHandle( HANDLE hProcess, LPCALLSTACK lpcs )
{
	LPPROCESS lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
	if( lpProcess )
	{
		return SwitchToProcess( lpProcess, lpcs );
	}
	return FALSE;
}



// ********************************************************************
//声明： void SwitchBackProcess( void )
//参数：
//	无
//返回值：
//	无
//功能描述：
//	与SwitchToProcess相对应，切换回调用者进程
//引用：
//	进行API调用时会用到该函数，当用户调用AP时，首先会切换到API服务进程空间，
//	当API执行完后，将会恢复到用户空间
// ********************************************************************

void SwitchBackProcess( void )
{
	LPPROCESS lpProcess;
	UINT uiSave;
	BOOL bSwitchContext;
	ACCESS_KEY aky;


//	ASSERT(lpCurThread->lpCallStack);

	LockIRQSave( &uiSave );

	lpProcess = (LPPROCESS)lpCurThread->lpCallStack->lpvData;
	aky = lpCurThread->lpCallStack->akyAccessKey;
	// 如果新的进程与线程的当前进程不同，则需要切换进程空间
	//ASSERT( lpProcess != &InitKernelProcess );
	if( 
		//lpProcess != &InitKernelProcess && 
		( lpProcess != lpCurThread->lpCurProcess || 
		  aky != lpCurThread->akyAccessKey ) )
		bSwitchContext = TRUE;
	else
		bSwitchContext = FALSE;
	// 清除调用链
    lpCurThread->lpCallStack = lpCurThread->lpCallStack->lpNext;


    //lpCurThread->lpCurProcess = lpProcess;

	// only test
	//{
	//	DWORD myAky = lpCurThread->lpCurProcess->akyAccessKey | lpCurThread->lpOwnerProcess->akyAccessKey;
	//	if( (aky & myAky) != myAky )
	//	{
	//		RETAILMSG( 1, ( "error: aky=0x%x,myAky=0x%x,CurProc=0x%x,OwnerProc=0x%x.\r\n",aky,myAky,lpCurThread->lpCurProcess->akyAccessKey,lpCurThread->lpOwnerProcess->akyAccessKey ) );
	//	}
	//}

	
    
	
	
	
	//

	if( bSwitchContext )
	{
//#ifdef VIRTUAL_MEM
		
		//装入当前进程的MMU有关数据，无效CACHE。。。
		//2004-12-30，ln.?????-在 GetMMUContext 中也有这段代码，现在看来好象是没有必要的
		//有可能是但是为了解决一个错误而加的。
        //dwProcessSlots[0] = (DWORD)lpProcess->lpSegIndex->lpSeg;
        //SetCPUId( lpProcess->dwVirtualAddressBase );
		//
		AccessKey_Set( &lpCurThread->akyAccessKey, aky );
		GetMMUContext( lpCurThread, 2, lpProcess );
//#endif
	}

	UnlockIRQRestore( &uiSave );

}

// ********************************************************************
//声明：HANDLE WINAPI KL_GetCallerProcess( void ) 
//参数：
//	无 
//返回值：
//	返回API调用者的进程句柄；如果没有，返回NULL
//功能描述：
//	得到API调用者的进程句柄
//引用：
//	系统API
// ********************************************************************

// ap->server->kernel->getcaller
// ap->kernel->other->getcaller

HANDLE WINAPI KL_GetCallerProcess( void )
{
	LPPROCESS lpProcess = GetHandleOwner();
	if( lpProcess )
		return lpProcess->hProcess;
	return NULL;
	/*
    CALLSTACK * lpcs = lpCurThread->lpCallStack;	
	while( lpcs && (lpcs->dwCallInfo & CALL_KERNEL) )
		lpcs = lpcs->lpNext;
    return lpcs ? ( (LPPROCESS)lpcs->lpvData )->hProcess : NULL;
	*/
}

// ********************************************************************
//声明：LPTSTR WINAPI KL_GetCommandLine(VOID) 
//参数：
//	无
//返回值：
//	假如成功，返回有效的指针；否则，返回NULL
//功能描述：
//	得到命令行参数
//引用：
//	系统API
// ********************************************************************
#define DEBUG_GET_COMMAND_LINE 0
LPTSTR WINAPI KL_GetCommandLine(VOID)
{
	//LOCK_SCHE();
	DEBUGMSG( DEBUG_GET_COMMAND_LINE,( "KL_GetCommandLine:0x%x,%s.\r\n", lpCurThread->lpCurProcess->lpszCommandLine, lpCurThread->lpCurProcess->lpszCommandLine ) );
	//UNLOCK_SCHE();
	return (LPTSTR)lpCurThread->lpCurProcess->lpszCommandLine;
}

// ********************************************************************
//声明：LPVOID WINAPI KL_GetProcessUserData( VOID ) 
//参数：
//	无
//返回值：
//	返回之前用KL_SetProcessUserData设置的值
//功能描述：
//	返回当前进程之前用KL_SetProcessUserData设置的值
//引用：
//	系统API
// ********************************************************************

LPVOID WINAPI KL_GetProcessUserData( VOID )
{
	return lpCurThread->lpCurProcess->lpUserData;
}

// ********************************************************************
//声明：LPVOID WINAPI KL_SetProcessUserData( LPVOID lpvData ) 
//参数：
//	IN lpvData - 需要设置的用户值
//返回值：
//	返回之前设置的值
//功能描述：
//	设置进程用户数据
//引用：
//	系统API
// ********************************************************************

LPVOID WINAPI KL_SetProcessUserData( LPVOID lpvData )
{
	LPVOID lpvRetv = lpCurThread->lpCurProcess->lpUserData;
	lpCurThread->lpCurProcess->lpUserData = lpvData;
	return lpvRetv;
}


// ********************************************************************
//声明：HANDLE WINAPI KL_OpenProcess( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
//参数：
//	IN dwDesiredAccess - 存取权限（不支持，必须为0）
//	IN bInheritHandle - 继承标志（不支持，必须为FALSE）
//	IN dwProcessId - 进程ID
//返回值：
//	假如成功，返回进程的句柄；失败，返回NULL
//功能描述：
//	通过进程ID得到进程句柄
//引用：
//	系统API
// ********************************************************************

HANDLE WINAPI KL_OpenProcess( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
{
	if( dwProcessId < MAX_PROCESSES && dwProcessId > 0 )
	{
		//LPPROCESS lpProcess = lppProcessPtr[dwProcessId] ? lppProcessPtr[dwProcessId]->hProcess : NULL;
		LPPROCESS lpProcess;
		UINT uiSave;

		LockIRQSave( &uiSave );
		lpProcess = lppProcessPtr[dwProcessId] ? lppProcessPtr[dwProcessId] : NULL;
		if( lpProcess )
		{
			Handle_AddRef( lpProcess->hProcess );
		}
		UnlockIRQRestore( &uiSave );
		if( lpProcess )
			return lpProcess->hProcess;
	}
	return NULL;
}


// ********************************************************************
//声明：DWORD WINAPI KL_GetProcessId( HANDLE hProcess )
//参数：
//	IN hProcess - 进程句柄
//返回值：
//	假如成功，返回进程的ID；失败，返回 0xffffffff
//功能描述：
//	通过进程句柄的得到进程ID
//引用：
//	系统API
// ********************************************************************

#define DEBUG_KL_GetProcessId 0
DWORD WINAPI KL_GetProcessId( HANDLE hProcess )
{
    LPPROCESS lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
    DEBUGMSG( DEBUG_KL_GetProcessId, ( "KL_GetProcessId: hProcess=0x%x entry.\r\n",hProcess ) );
    if( lpProcess ) 
	{
		return lpProcess->dwProcessId;
    }
	else
	{
		return 0xffffffff;
	}
}

