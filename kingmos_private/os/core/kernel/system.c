/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵��������ϵͳϵͳ����
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
 
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
// ������static BOOL RegisterDefaultShell( void  )
// ������
//		��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���ʧ��
// ����������
//		ע��Ĭ�ϵ�shell��������һ��������ĳ���
// ���ã�
//		
// ********************************************************************
static BOOL RegisterDefaultShell( void  )
{
    extern int CALLBACK Shell_WinMain( HINSTANCE hInstance,
		            HINSTANCE hPrevInstance,
		            LPTSTR    lpCmdLine,
		            int       nCmdShow );

	KL_RegisterApplication( "shell",  Shell_WinMain, NULL ); // ע�ᵽ���������
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
// ������BOOL InitialKingmosSystem( void * lpParam )
// ������
//		IN lpParam - ����(ΪNULL)
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���ʧ��
// ����������
//		��ʼ��Kingmosϵͳ
// ���ã�
//		��start.c����
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
    lpParam = 0;  // ȥ������������

	std_srand( KL_GetTickCount() );	//��ʼ���ں����������

	//KL_DebugOutString( "ISR_Init.\r\n" );
    if( ISR_Init() == FALSE )	// �жϷ����������ʼ��
		return FALSE;
	//KL_DebugOutString( "_InitSysMem.\r\n" );
	if( _InitSysMem() == FALSE )	// ��ʼ��ϵͳ���ڴ�
		return FALSE;
	//KL_DebugOutString( "KHeap_Init.\r\n" );
	if( KHeap_Init() == FALSE )	//��ʼ����ѹ�����
		return FALSE;
	//KL_DebugOutString( "InitScheduler.\r\n" );
    if( InitScheduler() == FALSE )	// ��ʼ��������
        return FALSE;
	//KL_DebugOutString( "_InitSemMgr.\r\n" );
	if( _InitSemMgr() == FALSE )	//��ʼ���ź���������
		return FALSE;
	//KL_DebugOutString( "InitHandleMgr.\r\n" );
	if( _InitHandleMgr() == FALSE )	//��ʼ�����������
		return FALSE;
#ifdef VIRTUAL_MEM
	//KL_DebugOutString( "InitialVirtualMgr.\r\n" );
	if( InitialVirtualMgr() == FALSE )	//��ʼ����������
		return FALSE;
#endif   //VIRTUAL_MEM
	//KL_DebugOutString( "_InitKernelHeap.\r\n" );
	if( _InitKernelHeap() == FALSE )	//��ʼ����̬�ѹ�����
        return FALSE;
	//KL_DebugOutString( "_InitAPIMgr.\r\n" );
	if( _InitAPIMgr() == FALSE )	//��ʼ��API���������
		return FALSE;
#ifndef INLINE_PROGRAM
	//KL_DebugOutString( "InitRomFsd.\r\n" );
    InitRomFsd();	//��ʼ��ROM�ļ�ϵͳ
#endif  //INLINE_PROGRAM

	//KL_DebugOutString( "InitialCoreServer.\r\n" );
	if( InitialCoreServer() == FALSE )	//��ʼ�����ķ�����
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

	//�ں˳�ʼ����ɣ�����shell
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

//��ѯ���еķ����Ƿ���ɹ���
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
// ������void WINAPI KL_PowerOffSystem( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		ִ�йص���Դ����
// ���ã�
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
	
	LOCK_SCHE();//��ס������
	// power off

	//EnumServerHandler( SCC_BROADCAST_POWEROFF, 0, 0 );//�����з������㲥�ص�Դ�¼�
	//�����з����������ص�Դ���
	DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  EnumServer.\r\n" ) );
	if( EnumServer( DoPowerQuerySuspend, 0 ) )
	{
		//�����з������㲥�ص�Դ�¼�
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  SBP_APMSUSPEND.\r\n" ) );
		EnumServerHandler( SCC_BROADCAST_POWER, SBP_APMSUSPEND, 0 );
		//ִ������ص�Դ����
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  OEM_PowerOff.\r\n" ) );
		OEM_PowerOff( 0 );
		// ��Դ�Ѵ򿪡�power on	
		// �����з������㲥����Դ�¼�
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: OEM_PowerOff return.\r\n" ) );
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  SBP_APMRESUMESUSPEND.\r\n" ) );
		EnumServerHandler( SCC_BROADCAST_POWER, SBP_APMRESUMESUSPEND, 0 );
	}
	else
	{
		DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem: call  SBP_APMQUERYSUSPENDFAILED.\r\n" ) );
		EnumServerHandler( SCC_BROADCAST_POWER, SBP_APMQUERYSUSPENDFAILED, 0 );
	}

	UNLOCK_SCHE();//�⿪������
 
	DEBUGMSG( DEBUG_KL_PowerOffSystem, ( "KL_PowerOffSystem leave.\r\n" ) );
//#endif

}

// ********************************************************************
// ������
// ������
//		IN  
// ����ֵ��
//		
// ����������
//		
// ���ã�
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
		OEM_PowerOff( 0 );//ִ������ص�Դ����
		break;
#endif
	default:
		return FALSE;
	}
	return TRUE;
}

// ********************************************************************
// ������VOID WINAPI KL_GetSystemInfo( LPSYSTEM_INFO lpsi )
// ������
//		IN  lpsi - SYSTEM_INFO�ṹָ��
// ����ֵ��
//		��
// ����������
//		���ص�ǰϵͳ��Ϣ
// ���ã�
//		ϵͳAPI
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
// ������BOOL WINAPI KL_GetVersion( LPOSVERSIONINFO lpvi )
// ������
//		IN  lpvi - OSVERSIONINFO�ṹָ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���ʧ��
// ����������
//		����Core�汾��
// ���ã�
//		ϵͳAPI	
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
// ������void WINAPI KL_GlobalMemoryStatus( LPMEMORYSTATUS lpms )
// ������
//		IN lpms - MEMORYSTATUS�ṹָ��
// ����ֵ��
//		��
// ����������
//		�õ���ǰϵͳ�ڴ�״̬
// ���ã�
//		ϵͳAPI
// ********************************************************************
void WINAPI KL_GlobalMemoryStatus( LPMEMORYSTATUS lpms )
{
	extern ULONG ulSysMainMemLength;


	if( lpms && lpms->dwLength == sizeof( MEMORYSTATUS ) )
	{	// �����õ�ҳ�ļ�	
		lpms->dwAvailPageFile = 0;
		// �����õ������ڴ�
		lpms->dwAvailPhys = Page_CountFreePages() * PAGE_SIZE;
		// ��ʹ���ڴ�İٷֱ�
		lpms->dwMemoryLoad = (ulSysMainMemLength - lpms->dwAvailPhys) * 100 / ulSysMainMemLength;
		// �ܵ�ҳ�ļ�
		lpms->dwTotalPageFile = 0;
		// �ܵ������ڴ�
		lpms->dwTotalPhys = ulSysMainMemLength;

#ifdef VIRTUAL_MEM
		{
			DWORD dwAvailVirtual;
			DWORD dwReserve;
			DWORD dwCommit;
			
			Seg_GetInfo( lpCurThread->lpCurProcess->lpProcessSegments->lpSeg, &dwAvailVirtual, &dwReserve, &dwCommit );
			// �����̵Ŀ��������ڴ�	
			lpms->dwAvailVirtual = dwAvailVirtual; //??
			// �����̵��ܵ����ڴ�
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
// ������static void CheckThread( LPTHREAD lpThread )
// ������
//		IN  lpThread - THREAD�ṹָ��
// ����ֵ��
//		
// ����������
//		����
// ���ã�
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
// ������void WINAPI KL_ProfileKernel( UINT uiOption, VOID * lpv )
// ������
//		IN uiOption - ���ܺ�
//		IN lpv - ���ܶ�Ӧ�ĸ��Ӳ���
// ����ֵ��
//		��
// ����������
//		���ں�״̬���з��������ڵ���
// ���ã�
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
	{  // percent of cpu consume / ����		
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
