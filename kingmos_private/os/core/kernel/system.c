/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：操作系统系统功能
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
 
******************************************************/

#include <ewindows.h>
#include <coresrv.h>
#include <epcore.h>
#include <sysintr.h>
#include <oemfunc.h>
#include <epalloc.h>
extern BOOL _InitHandleMgr( void );
extern BOOL _InitSemMgr( void );
extern BOOL _InitAPIMgr( void );
extern BOOL _InitKernelHeap( void );
extern BOOL InitRegistry( void );
extern BOOL InitComputerName( void );
extern void RegisterInlineProgram( void );
extern BOOL InitElfModuleMgr( void );
extern BOOL InitAlarmMgr( void );

#ifdef VIRTUAL_MEM
extern void Seg_GetInfo( LPSEGMENT lpSeg, 
						 LPDWORD lpdwAvailVirtual,
						 LPDWORD lpdwReserve,
						 LPDWORD lpdwCommit
						 );
#endif

#ifdef INLINE_PROGRAM

// ********************************************************************
// 声明：static BOOL RegisterDefaultShell( void  )
// 参数：
//		无
// 返回值：
//		假如成功，返回TRUE；否则，返回失败
// 功能描述：
//		注册默认的shell程序，这是一个内联版的程序
// 引用：
//		
// ********************************************************************
static BOOL RegisterDefaultShell( void  )
{
    extern int CALLBACK Shell_WinMain( HINSTANCE hInstance,
		            HINSTANCE hPrevInstance,
		            LPTSTR    lpCmdLine,
		            int       nCmdShow );

	KL_RegisterApplication( "shell",  Shell_WinMain, NULL ); // 注册到程序管理器
	return TRUE;

}

#endif   //INLINE_PROGRAM

// 
// #ifndef INLINE_PROGRAM
// 
// void LoadProgramFromRegistry( void )
// {
// 	HKEY hSubKey;
// 
//     if( KL_RegOpenKeyEx( HKEY_SOFTWARE_ROOT, "boot", 0, 0, &hSubKey ) == ERROR_SUCCESS )
// 	{
// 		TCHAR filePath[MAX_PATH];
// 		int n = 0;
// 
// 		KL_DebugOutString( "open root ok.\r\n" ); 
// 
// 		for( ; n < 10; n++ )
// 		{
// 			DWORD dwType;
// 			TCHAR tcValueName[16];  // boot01
// 			DWORD dwValueNameSize = sizeof( tcValueName );
// 			TCHAR tcValue[MAX_PATH];
// 			DWORD dwValueSize = sizeof( tcValue );
// 			
// 			if( ERROR_SUCCESS == KL_RegEnumValue( hSubKey, n, tcValueName, &dwValueNameSize, NULL, &dwType, tcValue, &dwValueSize ) )
// 			{
// 				KL_DebugOutString( "KL_RegEnumValue ok.\r\n" ); 
// 				if( dwType == REG_SZ &&
// 					strnicmp( tcValueName, "boot", 4 ) == 0 && 
// 					tcValueName[4] >= '0' && tcValueName[4] <= '9' && 
// 					tcValueName[5] >= '0' && tcValueName[5] <= '9' )
// 				{
// 					DWORD dwSize;
// 					dwSize = MAX_PATH;
// 					{
// 						strcpy( filePath, "\\kingmos\\" );
// 						strcat( filePath,  tcValue );
// 
// 	                    KL_DebugOutString( "boot load " ); 
// 						KL_DebugOutString( filePath ); 
// 						KL_DebugOutString( ".\r\n" ); 
//                         KL_CreateProcess( filePath, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL, NULL );
// 						msWait( 1000 );
// 					}
// 				}
// 			}
// 		}
// 	}
// }
// 
// #endif
// 

// ********************************************************************
// 声明：BOOL InitialKingmosSystem( void * lpParam )
// 参数：
//		IN lpParam - 保留(为NULL)
// 返回值：
//		假如成功，返回TRUE；否则，返回失败
// 功能描述：
//		初始化Kingmos系统
// 引用：
//		被start.c调用
// ********************************************************************
//HANDLE hDeviceExitEvent = NULL;
//HANDLE hgwmeExitEvent = NULL;
BOOL InitialKingmosSystem( void * lpParam )
{
	extern void InitRomFsd(void);
	extern BOOL InitialCoreServer( void );
	extern BOOL InitialVirtualMgr( void );
	extern BOOL KHeap_Init( void );

	KL_DebugOutString( "InitialKingmosSystem entry.\r\n" );
    lpParam = 0;  // 去掉编译器警告

	std_srand( KL_GetTickCount() );	//初始化内核随机数种子

	//KL_DebugOutString( "ISR_Init.\r\n" );
    if( ISR_Init() == FALSE )	// 中断服务管理器初始化
		return FALSE;
	//KL_DebugOutString( "_InitSysMem.\r\n" );
	if( _InitSysMem() == FALSE )	// 初始化系统主内存
		return FALSE;
	//KL_DebugOutString( "KHeap_Init.\r\n" );
	if( KHeap_Init() == FALSE )	//初始化快堆管理器
		return FALSE;
	//KL_DebugOutString( "InitScheduler.\r\n" );
    if( InitScheduler() == FALSE )	// 初始化调度器
        return FALSE;
	//KL_DebugOutString( "_InitSemMgr.\r\n" );
	if( _InitSemMgr() == FALSE )	//初始化信号量管理器
		return FALSE;
	//KL_DebugOutString( "InitHandleMgr.\r\n" );
	if( _InitHandleMgr() == FALSE )	//初始化句柄管理器
		return FALSE;
#ifdef VIRTUAL_MEM
	//KL_DebugOutString( "InitialVirtualMgr.\r\n" );
	if( InitialVirtualMgr() == FALSE )	//初始化虚存管理器
		return FALSE;
#endif   //VIRTUAL_MEM
	//KL_DebugOutString( "_InitKernelHeap.\r\n" );
	if( _InitKernelHeap() == FALSE )	//初始化动态堆管理器
        return FALSE;
	//KL_DebugOutString( "_InitAPIMgr.\r\n" );
	if( _InitAPIMgr() == FALSE )	//初始化API服务管理器
		return FALSE;
#ifndef INLINE_PROGRAM
	//KL_DebugOutString( "InitRomFsd.\r\n" );
    InitRomFsd();	//初始化ROM文件系统
#endif  //INLINE_PROGRAM

	//KL_DebugOutString( "InitialCoreServer.\r\n" );
	if( InitialCoreServer() == FALSE )	//初始化核心服务功能
		return FALSE;
	//KL_DebugOutString( "InitComputerName.\r\n" );	
    if( InitComputerName() == FALSE )
    {
		return FALSE;    	
    }
#ifndef INLINE_PROGRAM
	//KL_DebugOutString( "InitElfModuleMgr.\r\n" );	
	if( InitElfModuleMgr() == FALSE )
	{
		return FALSE;
	}
#endif

	//KL_DebugOutString( "InitAlarmMgr.\r\n" );
	if( InitAlarmMgr() == FALSE )
	{
		return FALSE;
	}

//	 hDeviceExitEvent = KL_CreateEvent( NULL, FALSE, FALSE, "device_event" );
//	 hgwmeExitEvent = KL_CreateEvent( NULL, FALSE, FALSE, "gwme_exit_event" );

#ifndef INLINE_PROGRAM
	 //RegisterInlineProgram();
#endif  //INLINE_PROGRAM
	//

	//内核初始化完成，启动shell
#ifdef INLINE_PROGRAM
    //KL_DebugOutString( "InstallSystemServer.\r\n" );
	if( RegisterDefaultShell() == FALSE )
	    return FALSE;

	//KL_DebugOutString( "run shell.exe...\r\n" ); 
	KL_LoadApplication( "shell", NULL );	

#else
	 KL_DebugOutString( "run shell.exe...\r\n" );
#ifdef __CHECK_MEM
	 {
		extern int startCheckMem;
		startCheckMem = 1;
	 }
#endif	 
	// KL_CreateProcess( "\\Kingmos\\test.exe", NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL, NULL );	 
	 KL_CreateProcess( "\\Kingmos\\shell.exe", NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL, NULL );
#endif

	KL_DebugOutString( "InitialKingmosSystem leave.\r\n" );
    return TRUE;
}

//查询所有的服务是否许可挂起
#define DEBUG_DoPowerQuerySuspend 1
static BOOL CALLBACK DoPowerQuerySuspend( APIINFO * lpAPI, LPVOID lpParam )
{
//	CALLSTACK cs;
	CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	BOOL bRetv = FALSE;
	if( SwitchToProcess( lpAPI->lpProcess, lpcs ) )
	{
		lpcs->dwCallInfo |= CALL_ALLOC;
		DEBUGMSG( DEBUG_DoPowerQuerySuspend, ( "DoPowerQuerySuspend call pServerHandler(%s).\r\n", lpAPI->lpProcess->lpszApplicationName ) );
		bRetv = ( (PSERVER_HANDLER)lpAPI->lpServerCtrlInfo->pServerHandler )( lpAPI->hServer, SCC_BROADCAST_POWER, SBP_APMQUERYSUSPEND, 0 );
		DEBUGMSG( DEBUG_DoPowerQuerySuspend, ( "DoPowerQuerySuspend pServerHandler(%s) return(%d).\r\n", lpAPI->lpProcess->lpszApplicationName, bRetv ) );
		SwitchBackProcess();
	}
	KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return bRetv;
}

// ********************************************************************
// 声明：void WINAPI KL_PowerOffSystem( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		执行关掉电源工作
// 引用：
//		
// ********************************************************************
#define DEBUG_KL_PowerOffSystem 1
void WINAPI KL_PowerOffSystem( void )
{
// 	
// #ifdef EML_WIN32
// 	extern void CPU_Reset( void );
// 
// 	CPU_Reset();
// 	while(1);
// #else
// 	
	DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem entry.\r\n" ) );
	
	LOCK_SCHE();//锁住调度器
	// power off

	//EnumServerHandler( SCC_BROADCAST_POWEROFF, 0, 0 );//向所有服务程序广播关电源事件
	//向所有服务程序请求关电源许可
	DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  EnumServer.\r\n" ) );
	if( EnumServer( DoPowerQuerySuspend, 0 ) )
	{
		//向所有服务程序广播关电源事件
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  SBP_APMSUSPEND.\r\n" ) );
		EnumServerHandler( SCC_BROADCAST_POWER, SBP_APMSUSPEND, 0 );
		//执行物理关电源工作
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  OEM_PowerOff.\r\n" ) );
		OEM_PowerOff( 0 );
		// 电源已打开。power on	
		// 向所有服务程序广播开电源事件
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: OEM_PowerOff return.\r\n" ) );
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  SBP_APMRESUMESUSPEND.\r\n" ) );
		EnumServerHandler( SCC_BROADCAST_POWER, SBP_APMRESUMESUSPEND, 0 );
	}
	else
	{
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  SBP_APMQUERYSUSPENDFAILED.\r\n" ) );
		EnumServerHandler( SCC_BROADCAST_POWER, SBP_APMQUERYSUSPENDFAILED, 0 );
	}

	UNLOCK_SCHE();//解开调度器
 
	DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem leave.\r\n" ) );
//#endif

}

// ********************************************************************
// 声明：
// 参数：
//		IN  
// 返回值：
//		
// 功能描述：
//		
// 引用：
//		
// ********************************************************************
BOOL WINAPI KL_ExitSystem(
						  UINT uFlags,       // shutdown operation
						  DWORD dwReserved   // reserved
						  )
{
	switch( uFlags )
	{
	case EXS_POWEROFF:
		KL_PowerOffSystem();
		return TRUE;
	case EXS_REBOOT:
	case EXS_SHUTDOWN:
		//KL_SetEvent( hDeviceExitEvent );
		EnumServerHandler( SCC_BROADCAST_SHUTDOWN, 0, 0 );
#ifdef EML_WIN32
		{
			extern void CPU_Reset( void );
			CPU_Reset();
			while(1);
		}
#else
		OEM_PowerOff( 0 );//执行物理关电源工作
		break;
#endif
	default:
		return FALSE;
	}
	return TRUE;
}

// ********************************************************************
// 声明：VOID WINAPI KL_GetSystemInfo( LPSYSTEM_INFO lpsi )
// 参数：
//		IN  lpsi - SYSTEM_INFO结构指针
// 返回值：
//		无
// 功能描述：
//		返回当前系统信息
// 引用：
//		系统API
// ********************************************************************
VOID WINAPI KL_GetSystemInfo( LPSYSTEM_INFO lpsi )
{
#ifdef ARM_CPU
	lpsi->sysID.arch.wProcessorArchitecture = PROCESSOR_ARCHITECTURE_ARM;
	lpsi->dwProcessorType = PROCESSOR_STRONGARM;
	lpsi->wProcessorRevision = 4; //ARMV4
#endif
#ifdef X86_CPU
	lpsi->sysID.arch.wProcessorArchitecture = PROCESSOR_ARCHITECTURE_INTEL;
	lpsi->dwProcessorType = PROCESSOR_INTEL_386;	
	lpsi->wProcessorRevision = 3; //386
#endif
	lpsi->sysID.arch.wReserved = 0;
	lpsi->dwPageSize = PAGE_SIZE;//4 * 1024;
	lpsi->lpMinimumApplicationAddress = (LPVOID)(64 * 1024);
	lpsi->lpMaximumApplicationAddress = (LPVOID)(32 * 1024 * 1024); //32m
	lpsi->dwActiveProcessorMask = 0x00000001;
	lpsi->dwNumberOfProcessors = 1;
	lpsi->dwAllocationGranularity = 64 * 1024;
	lpsi->wProcessorLevel = 1;
}

// ********************************************************************
// 声明：BOOL WINAPI KL_GetVersion( LPOSVERSIONINFO lpvi )
// 参数：
//		IN  lpvi - OSVERSIONINFO结构指针
// 返回值：
//		假如成功，返回TRUE；否则，返回失败
// 功能描述：
//		返回Core版本号
// 引用：
//		系统API	
// ********************************************************************
BOOL WINAPI KL_GetVersion( LPOSVERSIONINFO lpvi )
{
	if( lpvi->dwOSVersionInfoSize == sizeof( OSVERSIONINFO ) )
	{
		memset( lpvi, 0, sizeof( OSVERSIONINFO ) );
		lpvi->dwMajorVersion = MAJOR_VERSION;
		lpvi->dwMinorVersion = MINOR_VERSION;
		lpvi->dwBuildNumber = BUILD_VERSION;
		lpvi->dwPlatformId = 0;
	}
	return TRUE;
}

// ********************************************************************
// 声明：void WINAPI KL_GlobalMemoryStatus( LPMEMORYSTATUS lpms )
// 参数：
//		IN lpms - MEMORYSTATUS结构指针
// 返回值：
//		无
// 功能描述：
//		得到当前系统内存状态
// 引用：
//		系统API
// ********************************************************************
void WINAPI KL_GlobalMemoryStatus( LPMEMORYSTATUS lpms )
{
	extern ULONG ulSysMainMemLength;


	if( lpms && lpms->dwLength == sizeof( MEMORYSTATUS ) )
	{	// 可利用的页文件	
		lpms->dwAvailPageFile = 0;
		// 可利用的物理内存
		lpms->dwAvailPhys = Page_CountFreePages() * PAGE_SIZE;
		// 已使用内存的百分比
		lpms->dwMemoryLoad = (ulSysMainMemLength - lpms->dwAvailPhys) * 100 / ulSysMainMemLength;
		// 总的页文件
		lpms->dwTotalPageFile = 0;
		// 总的物理内存
		lpms->dwTotalPhys = ulSysMainMemLength;

#ifdef VIRTUAL_MEM
		{
			DWORD dwAvailVirtual;
			DWORD dwReserve;
			DWORD dwCommit;
			
			Seg_GetInfo( lpCurThread->lpCurProcess->lpProcessSegments->lpSeg, &dwAvailVirtual, &dwReserve, &dwCommit );
			// 本进程的可利用虚内存	
			lpms->dwAvailVirtual = dwAvailVirtual; //??
			// 本进程的总的虚内存
			lpms->dwTotalVirtual = 32 * 1024;		
		}
#else
		lpms->dwAvailVirtual = 0;
		lpms->dwTotalVirtual = 0;
#endif
	}
#ifdef __DEBUG
	{
        extern void KHeap_Check( void );
		KHeap_Check();
	}
#endif
}

// ********************************************************************
// 声明：static void CheckThread( LPTHREAD lpThread )
// 参数：
//		IN  lpThread - THREAD结构指针
// 返回值：
//		
// 功能描述：
//		调试
// 引用：
//		
// ********************************************************************
static void CheckThread( LPTHREAD lpThread )
{
	extern volatile DWORD dwJiffies;
	LPCRITICAL_SECTION lpcs;
	if( lpThread->dwState == THREAD_UNINTERRUPTIBLE &&
		( lpcs = (LPCRITICAL_SECTION)(lpThread->lpdwTLS[TLS_CRITICAL_SECTION]) ) )
	{
		//CALLSTACK cs;
		CALLSTACK * lpcall = KHeap_Alloc( sizeof( CALLSTACK ) );

		SwitchToProcess( lpThread->lpOwnerProcess, lpcall );
		if( lpcs->uiFlag & 0x1 )//CSF_DEBUG )
			EdbgOutputDebugString( "hThread(0x%x),OwnerProc(%s),CurProc(%s),Locked by CS(0x%x,%s), at file(%s),line(%d),time(0x%x).\r\n", lpThread->hThread, lpThread->lpOwnerProcess->lpszApplicationName, lpThread->lpCurProcess->lpszApplicationName, lpcs, lpcs->lpcsName, lpcs->lpFileName, lpcs->iFileLine, lpThread->dwTimerCount );
		else
			EdbgOutputDebugString( "hThread(0x%x),OwnerProc(%s),CurProc(%s),Locked by CS(0x%x,%s),time(0x%x).\r\n", lpThread->hThread, lpThread->lpOwnerProcess->lpszApplicationName, lpThread->lpCurProcess->lpszApplicationName, lpcs, lpcs->lpcsName, lpThread->dwTimerCount );
		SwitchBackProcess();

		KHeap_Free( lpcall, sizeof( CALLSTACK ) );

	}
	else if( lpThread->dwState == THREAD_RUNNING )
	{
		EdbgOutputDebugString( "Thread running,hThread(0x%x),OwnerProc(%s),CurProc(%s),time(0x%x).\r\n", lpThread->hThread, lpThread->lpOwnerProcess->lpszApplicationName,lpThread->lpCurProcess->lpszApplicationName, lpThread->dwTimerCount );
	}
	else if( lpThread->dwState == THREAD_ZOMBIE )
	{
		EdbgOutputDebugString( "Thread zombie(die),ThreadID(0x%x),OwnerProc(%s),CurProc(%s),time(0x%x).\r\n", lpThread->dwThreadId, lpThread->lpOwnerProcess->lpszApplicationName,lpThread->lpCurProcess->lpszApplicationName, lpThread->dwTimerCount );
	}
	else// if( lpThread->dwState != THREAD_RUNNING )
	{
		EdbgOutputDebugString( "hThread(0x%x),OwnerProc(%s),CurProc(%s),Locked by somthing...,timeout(%d),dwJiffies(%d),diff(%d),lpsemWait(0x%x),time(0x%x).\r\n", lpThread->hThread,lpThread->lpOwnerProcess->lpszApplicationName,lpThread->lpCurProcess->lpszApplicationName,lpThread->dwTimeout, dwJiffies, dwJiffies - lpThread->dwTimeout, lpThread->lppsemWait, lpThread->dwTimerCount );
	}
}

// ********************************************************************
// 声明：void WINAPI KL_ProfileKernel( UINT uiOption, VOID * lpv )
// 参数：
//		IN uiOption - 功能号
//		IN lpv - 功能对应的附加参数
// 返回值：
//		无
// 功能描述：
//		对内核状态进行分析，用于调试
// 引用：
//		
// ********************************************************************
void WINAPI KL_ProfileKernel( UINT uiOption, VOID * lpv )
{
	EdbgOutputDebugString( "ProfileKernel start.\r\n" );
    if( uiOption == 1 )
	{   // profile running thread
		LPTHREAD p;
		//UINT uiSave;

	    //LockIRQSave( &uiSave );
        LOCK_SCHE();
		p = lpInitKernelThread;
		do
		{
			if( p->dwState == THREAD_RUNNING )
			    EdbgOutputDebugString( "ThreadId=0x%x,OwnerProcess=%s,CurProcess=%s,CurPrio=%d,nBoost=%d,time(%d).\r\n", p->dwThreadId, p->lpOwnerProcess->lpszApplicationName, p->lpCurProcess->lpszApplicationName, p->nCurPriority, p->nBoost, p->dwTimerCount );
			p = p->lpNextThread;
		}while( p != lpInitKernelThread );

	    //UnlockIRQRestore( &uiSave );
		UNLOCK_SCHE();
	}
	else if( uiOption == 2 )
	{    //  profile all thread
		LPTHREAD lpThread;
		//UINT uiSave;
		//LockIRQSave( &uiSave );
		LOCK_SCHE();
		for ( lpThread = lpInitKernelThread ; (lpThread = lpThread->lpNextThread) != lpInitKernelThread; )
		{
			CheckThread( lpThread );
		}
		//UnlockIRQRestore( &uiSave );
		UNLOCK_SCHE();
	}
	else if( uiOption == 3 )
	{// get a thread handle, and see it if in wait ... or sleep
		LPTHREAD lpThread = (LPTHREAD)HandleToPtr( (HANDLE)lpv, OBJ_THREAD );
		
		if( lpThread )
		{
//			UINT uiSave;
//			LockIRQSave( &uiSave );
			LOCK_SCHE();
			CheckThread( lpThread );
			UNLOCK_SCHE();
//			UnlockIRQRestore( &uiSave );
		}
	}
	else if( uiOption == 4 )
	{  // percent of cpu consume / 消耗		
		//if( lpv )
		{
			static DWORD dwSaveTimerCount = 0;
			static DWORD dwSaveJiffies = 0;

			DWORD dwDiffTimerCount = lpInitKernelThread->dwTimerCount - dwSaveTimerCount;
			DWORD dwDiffJiffies = dwJiffies - dwSaveJiffies;

			dwSaveTimerCount = lpInitKernelThread->dwTimerCount;
			dwSaveJiffies = dwJiffies;

			LOCK_SCHE();
			//*(DWORD*)lpv = InitKernelThread.dwTimerCount;
			//*( (DWORD*)lpv+1 ) = dwJiffies;

		    EdbgOutputDebugString( "CPU Idle=(%d).\r\n", dwDiffTimerCount * 100 / dwDiffJiffies );
			UNLOCK_SCHE();
		}
	}
	else if( uiOption == 5 )
	{	// process memory consume
		int i;
		DWORD dwTotalCommitHeapSize  = 0;
		DWORD dwTotalCommitVirtualSize  = 0;
		LOCK_SCHE();

		for( i = 0; i < MAX_PROCESSES; i++ )
		{
			if( lppProcessPtr[i] )
			{
				EdbgOutputDebugString( "ProcessHeap:%s,dwTotalReserveHeapSize=%d,dwTotalCommitHeapSize=%d.\r\n",
										lppProcessPtr[i]->lpszApplicationName,
										lppProcessPtr[i]->lpHeap ? lppProcessPtr[i]->lpHeap->dwTotalReserveSize : 0, 
										lppProcessPtr[i]->lpHeap ? lppProcessPtr[i]->lpHeap->dwTotalCommitSize : 0 );
				dwTotalCommitHeapSize += lppProcessPtr[i]->lpHeap->dwTotalCommitSize;

#ifdef VIRTUAL_MEM
				{
					DWORD dwAvailVirtual = 0;
					DWORD dwReserve = 0;
					DWORD dwCommit = 0;
					
					if( lppProcessPtr[i]->lpProcessSegments )
					{
						Seg_GetInfo( lppProcessPtr[i]->lpProcessSegments->lpSeg, &dwAvailVirtual, &dwReserve, &dwCommit );
						EdbgOutputDebugString( "Processe dwCommitVirtualSize=%d, %dk.\r\n", dwCommit, dwCommit / 1024 );
						dwTotalCommitVirtualSize += dwCommit;
					}
				}
#endif
			}
		}
		EdbgOutputDebugString( "Processes dwTotalCommitHeapSize=%d, %dk.\r\n", dwTotalCommitHeapSize, dwTotalCommitHeapSize / 1024 );
		EdbgOutputDebugString( "Processes dwTotalCommitVirtualSize=%d, %dk.\r\n", dwTotalCommitVirtualSize, dwTotalCommitVirtualSize / 1024 );
		UNLOCK_SCHE();
	}

	EdbgOutputDebugString( "ProfileKernel end.\r\n" );
}
