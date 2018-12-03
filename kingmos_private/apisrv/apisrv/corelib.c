/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：核心库
版本号：1.0.0
开发时期：2002-04-20
作者：李林
修改记录：
    2003-05-13: LN, 增加 DLL_OpenModule & DLL_OpenDependence
******************************************************/

#include <eframe.h>
#include <eucore.h>
#include <eapisrv.h>

#include <epcore.h>
/*
//test
#define CALL_TRAP
typedef struct _CALLTRAP
{
    DWORD apiInfo;
    DWORD arg0;
}CALLTRAP;
    
//
*/


// application
typedef BOOL ( WINAPI * PAP_REGISTER )( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon );
BOOL WINAPI Application_Register( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, AP_REGISTER, 3 );
    cs.arg0 = (DWORD)lpcszName;
    return (DWORD)CALL_SERVER( &cs, lpApProc, hIcon );
#else

	PAP_REGISTER pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, AP_REGISTER, &pfn, &cs ) )
	{
		//lpcszName = MapProcessPtr( lpcszName, (LPPROCESS)cs.lpvData );
		retv = pfn( lpcszName, lpApProc, hIcon );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PAP_LOAD )( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine );
BOOL WINAPI Application_Load( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, AP_LOAD, 2 );
    cs.arg0 = (DWORD)lpcszName;
    return (DWORD)CALL_SERVER( &cs, lpcszCmdLine );
#else

	PAP_LOAD pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, AP_LOAD, &pfn, &cs ) )
	{
		//lpcszName = MapProcessPtr( lpcszName, (LPPROCESS)cs.lpvData );
		//lpcszCmdLine = MapProcessPtr( lpcszCmdLine, (LPPROCESS)cs.lpvData );
		retv = pfn( lpcszName, lpcszCmdLine );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HICON ( WINAPI * PAP_GETICON )( LPCTSTR lpcszName, UINT fIconType );
HICON WINAPI Application_GetIcon( LPCTSTR lpcszName, UINT fIconType )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, AP_GETICON, 2 );
    cs.arg0 = (DWORD)lpcszName;
    return (HICON)CALL_SERVER( &cs, fIconType );
#else

	PAP_GETICON pfn;

	CALLSTACK cs;
	HICON   retv = NULL;

	if( API_Enter( API_KERNEL, AP_GETICON, &pfn, &cs ) )
	{
		//lpcszName = MapProcessPtr( lpcszName, (LPPROCESS)cs.lpvData );
		retv = pfn( lpcszName, fIconType );
		API_Leave(  );
	}
	return retv;
#endif
}

//PROCESS_CREATE,
typedef BOOL ( WINAPI * PPROCESS_CREATE )(
						   LPCTSTR lpszApplicationName,                 // name of executable module
						   LPCTSTR lpszCommandLine,                      // command line string
						   LPSECURITY_ATTRIBUTES lpProcessAttributes, // set to NULL
						   LPSECURITY_ATTRIBUTES lpThreadAttributes,  // set to NULL
						   BOOL bInheritHandles,                      // set to FALSE
						   DWORD dwCreationFlags,                     // set to 0
						   LPVOID lpEnvironment,                      // set to NULL
						   LPCTSTR lpCurrentDirectory,                // set to NULL
						   LPSTARTUPINFO lpStartupInfo,               // set to NULL
						   LPPROCESS_INFORMATION lpProcessInformation 
						   );

BOOL WINAPI Process_Create(
						   LPCTSTR lpszApplicationName,                 // name of executable module
						   LPCTSTR lpszCommandLine,                      // command line string
						   LPSECURITY_ATTRIBUTES lpProcessAttributes, // set to NULL
						   LPSECURITY_ATTRIBUTES lpThreadAttributes,  // set to NULL
						   BOOL bInheritHandles,                      // set to FALSE
						   DWORD dwCreationFlags,                     // set to 0
						   LPVOID lpEnvironment,                      // set to NULL
						   LPCTSTR lpCurrentDirectory,                // set to NULL
						   LPSTARTUPINFO lpStartupInfo,               // set to NULL
						   LPPROCESS_INFORMATION lpProcessInformation 
						   )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_CREATE, 10 );
    cs.arg0 = (DWORD)
																			lpszApplicationName;
    return (DWORD)CALL_SERVER( &cs,
											lpszCommandLine,
											lpProcessAttributes,
											lpThreadAttributes,
											bInheritHandles,
											dwCreationFlags,
											lpEnvironment,
											lpCurrentDirectory,
											lpStartupInfo,
											lpProcessInformation );
#else

	PPROCESS_CREATE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, PROCESS_CREATE, &pfn, &cs ) )
	{
		retv = pfn( 
					lpszApplicationName,
					lpszCommandLine,
					lpProcessAttributes,
					lpThreadAttributes,
					bInheritHandles,
					dwCreationFlags,
					lpEnvironment,
					lpCurrentDirectory,
					lpStartupInfo,
					lpProcessInformation
			     );
		API_Leave(  );
	}
	return retv;
#endif
}

//	PROCESS_TERMINATE,
typedef VOID ( WINAPI * PPROCESS_TERMINATE )( HANDLE hProcess, UINT uExitCode );
VOID WINAPI Process_Terminate(
							HANDLE hProcess,
							UINT uExitCode   // exit code for all threads
						 )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_TERMINATE, 2 );
    cs.arg0 = (DWORD)hProcess;
    CALL_SERVER( &cs, uExitCode );
#else

	PPROCESS_TERMINATE pfn;

	CALLSTACK cs;

	if( API_Enter( API_KERNEL, PROCESS_TERMINATE, &pfn, &cs ) )
	{
		pfn( hProcess, uExitCode );
		API_Leave(  );
	}
#endif
}


//	PROCESS_EXIT,
typedef VOID ( WINAPI * PPROCESS_EXIT )( UINT uExitCode );
VOID WINAPI Process_Exit(
						 UINT uExitCode   // exit code for all threads
						 )
{
	Process_Terminate( Process_GetCurrent(), uExitCode );
}

//	PROCESS_GETCURRENTID,
typedef DWORD ( WINAPI * PPROCESS_GETCURRENTID )(void);
DWORD WINAPI Process_GetCurrentId(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_GETCURRENTID, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PPROCESS_GETCURRENTID pfn;

	CALLSTACK cs;
	DWORD   retv = -1;

	if( API_Enter( API_KERNEL, PROCESS_GETCURRENTID, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

//	PROCESS_GETHEAP,
typedef HANDLE ( WINAPI * PPROCESS_GETHEAP )( void );
HANDLE WINAPI Process_GetHeap( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_GETHEAP, 0 );
    cs.arg0 = 0;
    return (HANDLE)CALL_SERVER( &cs );
#else

	PPROCESS_GETHEAP pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, PROCESS_GETHEAP, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

//	PROCESS_MAPPTR,
typedef LPVOID ( WINAPI * PPROCESS_MAPPTR )( LPVOID lpv, HANDLE hProcess );
LPVOID WINAPI Process_MapPtr( LPVOID lpv, HANDLE hProcess )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_MAPPTR, 2 );
    cs.arg0 = (DWORD)lpv;
    return (LPVOID)CALL_SERVER( &cs, hProcess );
#else

	PPROCESS_MAPPTR pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, PROCESS_MAPPTR, &pfn, &cs ) )
	{
		retv = pfn( lpv, hProcess );
		API_Leave(  );
	}
	return retv;
#endif
}

//	PROCESS_UNMAPPTR
typedef LPVOID ( WINAPI * PPROCESS_UNMAPPTR )( LPVOID lpv );
LPVOID WINAPI Process_UnMapPtr( LPVOID lpv )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_UNMAPPTR, 1 );
    cs.arg0 = (DWORD)lpv ;
    return (LPVOID)CALL_SERVER( &cs );
#else

	PPROCESS_UNMAPPTR pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, PROCESS_UNMAPPTR, &pfn, &cs ) )
	{
		retv = pfn( lpv );
		API_Leave(  );
	}
	return retv;
#endif
}

//    PROCESS_GETCURRENT,
typedef HANDLE ( WINAPI * PPROCESS_GETCURRENT )( void );
HANDLE WINAPI Process_GetCurrent( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_GETCURRENT, 0 );
    cs.arg0 = 0;
    return (HANDLE)CALL_SERVER( &cs );
#else

	PPROCESS_GETCURRENT pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, PROCESS_GETCURRENT, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

//    PROCESS_GETCALLER,
typedef HANDLE ( WINAPI * PPROCESS_GETCALLER )( void );
HANDLE WINAPI Process_GetCaller( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_GETCALLER, 0 );
    cs.arg0 = 0;
    return (HANDLE)CALL_SERVER( &cs );
#else

	PPROCESS_GETCALLER pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, PROCESS_GETCALLER, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

//   PROCESS_GETOWNER
typedef HANDLE ( WINAPI * PPROCESS_GETOWNER )( void );
HANDLE WINAPI Process_GetOwner( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_GETOWNER, 0 );
    cs.arg0 = 0;
    return (HANDLE)CALL_SERVER( &cs );
#else

	PPROCESS_GETOWNER pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, PROCESS_GETOWNER, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PPROCESS_GETEXITCODE )( HANDLE hProcess, LPDWORD lpExitCode );
BOOL WINAPI Process_GetExitCode( HANDLE hProcess, LPDWORD lpExitCode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_GETEXITCODE, 2 );
    cs.arg0 = (DWORD)hProcess;
    return (DWORD)CALL_SERVER( &cs, lpExitCode );
#else

	PPROCESS_GETEXITCODE pfn;

	CALLSTACK cs;
	BOOL   retv = 0;

	if( API_Enter( API_KERNEL, PROCESS_GETEXITCODE, &pfn, &cs ) )
	{
		retv = pfn( hProcess, lpExitCode );
		API_Leave(  );
	}
	return retv;
#endif
}

static DWORD WINAPI UserThreadProc( LPVOID lpParameter )
{
}

// thread
//    THREAD_CREATE,
typedef HANDLE ( WINAPI * PTHREAD_CREATE )(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    DWORD dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
   );

HANDLE WINAPI Thread_Create(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    DWORD dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
   )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_CREATE, 6 );
    cs.arg0 = (DWORD)			lpThreadAttributes;
    return (HANDLE)CALL_SERVER( &cs,
			dwStackSize,
			lpStartAddress,
			lpParameter,
			dwCreationFlags,
			lpThreadId );
#else

	PTHREAD_CREATE pfn;

	CALLSTACK cs;
	HANDLE   retv = FALSE;

	if( API_Enter( API_KERNEL, THREAD_CREATE, &pfn, &cs ) )
	{
		retv = pfn(
			lpThreadAttributes,
			dwStackSize,
			lpStartAddress,
			lpParameter,
			dwCreationFlags,
			lpThreadId
			);
		API_Leave(  );
	}
	return retv;
#endif
}

//    THREAD_SETPRIORITY
typedef BOOL ( WINAPI * PTHREAD_SETPRIORITY )( HANDLE hThread, long lPriority, UINT flag );
BOOL WINAPI Thread_SetPriority( HANDLE hThread, long lPriority )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_SETPRIORITY, 3 );
    cs.arg0 = (DWORD)hThread;
    return (DWORD)CALL_SERVER( &cs, lPriority, 0 );
#else

	PTHREAD_SETPRIORITY pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, THREAD_SETPRIORITY, &pfn, &cs ) )
	{
		retv = pfn( hThread, lPriority, 0 );
		API_Leave(  );
	}
	return retv;
#endif
}

//KL_SetThreadPolicy
typedef BOOL ( WINAPI * PTHREAD_SETPOLICY )( HANDLE hThread, UINT uiPolicy );
BOOL WINAPI Thread_SetPolicy( HANDLE hThread, UINT uiPolicy )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_SETPOLICY, 2 );
    cs.arg0 = (DWORD)hThread;
    return (DWORD)CALL_SERVER( &cs, uiPolicy );
#else

	PTHREAD_SETPOLICY pfn;
	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, THREAD_SETPOLICY, &pfn, &cs ) )
	{
		retv = pfn( hThread, uiPolicy );
		API_Leave(  );
	}
	return retv;
#endif
}


//    THREAD_GETCURRENT,
typedef HANDLE ( WINAPI * PTHREAD_GETCURRENT )( void );
HANDLE WINAPI Thread_GetCurrent( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_GETCURRENT, 0 );
    cs.arg0 = 0;
    return (HANDLE)CALL_SERVER( &cs );
#else

	PTHREAD_GETCURRENT pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, THREAD_GETCURRENT, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

//    THREAD_GETCURRENTID,
typedef DWORD ( WINAPI * PTHREAD_GETCURRENTID )( void );
DWORD WINAPI Thread_GetCurrentId( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_GETCURRENTID, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PTHREAD_GETCURRENTID pfn;

	CALLSTACK cs;
	DWORD   retv = -1;

	if( API_Enter( API_KERNEL, THREAD_GETCURRENTID, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

//    THREAD_GETEXITCODE,
typedef BOOL ( WINAPI * PTHREAD_GETEXITCODE )( HANDLE hThread, LPDWORD lpExitCode );
BOOL WINAPI Thread_GetExitCode( HANDLE hThread, LPDWORD lpExitCode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_GETEXITCODE, 2 );
    cs.arg0 = (DWORD)hThread;
    return (DWORD)CALL_SERVER( &cs, lpExitCode );
#else

	PTHREAD_GETEXITCODE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, THREAD_GETEXITCODE, &pfn, &cs ) )
	{
		retv = pfn( hThread, lpExitCode );
		API_Leave(  );
	}
	return retv;
#endif
}

//    THREAD_EXIT,
typedef void ( WINAPI * PTHREAD_EXIT )( DWORD dwExitCode );
void WINAPI Thread_Exit( DWORD dwExitCode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_EXIT, 1 );
    cs.arg0 = (DWORD)dwExitCode ;
    CALL_SERVER( &cs );
#else

	PTHREAD_EXIT pfn;

	CALLSTACK cs;
//	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, THREAD_EXIT, &pfn, &cs ) )
	{
		pfn( dwExitCode );
		API_Leave(  );
	}
	//return retv;
#endif
}

//    THREAD_SWITCHTO,
typedef BOOL ( WINAPI * PTHREAD_SWITCHTO ) ( VOID );
BOOL WINAPI Thread_SwitchTo( VOID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_SWITCHTO, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PTHREAD_SWITCHTO pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, THREAD_SWITCHTO, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTHREAD_TERMINATE ) ( HANDLE hThread, DWORD dwExitCode );
BOOL WINAPI Thread_Terminate( HANDLE hThread, DWORD dwExitCode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_TERMINATE, 2 );
    cs.arg0 = (DWORD)hThread;
    return (DWORD)CALL_SERVER( &cs, dwExitCode );
#else

	PTHREAD_TERMINATE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, THREAD_TERMINATE, &pfn, &cs ) )
	{
		retv = pfn( hThread, dwExitCode );
		API_Leave(  );
	}
	return retv;
#endif
}


// semphore...
//    SEMAPHORE_CREATE,
typedef HANDLE ( WINAPI * PSEMAPHORE_CREATE )( LPSECURITY_ATTRIBUTES lpAttr, int nInitialCount, int nMaximumCount, LPCTSTR lpcszName );
HANDLE WINAPI Semaphore_Create( LPSECURITY_ATTRIBUTES lpAttr,
							    int nInitialCount, 
								int nMaximumCount, 
								LPCTSTR lpcszName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SEMAPHORE_CREATE, 4 );
    cs.arg0 = (DWORD)lpAttr;
    return (HANDLE)CALL_SERVER( &cs, nInitialCount, nMaximumCount, lpcszName );
#else

	PSEMAPHORE_CREATE pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, SEMAPHORE_CREATE, &pfn, &cs ) )
	{
		retv = pfn( lpAttr, nInitialCount, nMaximumCount, lpcszName );
		API_Leave(  );
	}
	return retv;
#endif
}

//    SEMAPHORE_RELEASE,
typedef BOOL ( WINAPI * PSEMAPHORE_RELEASE )( HANDLE hSemaphore, int lReleaseCount, LPINT lpPreviousCount );
BOOL WINAPI Semaphore_Release( HANDLE hSemaphore, 
							  int lReleaseCount, 
							  LPINT lpPreviousCount )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SEMAPHORE_RELEASE, 3 );
    cs.arg0 = (DWORD)hSemaphore;
    return (DWORD)CALL_SERVER( &cs, lReleaseCount, lpPreviousCount );
#else

	PSEMAPHORE_RELEASE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, SEMAPHORE_RELEASE, &pfn, &cs ) )
	{
		retv = pfn( hSemaphore, lReleaseCount, lpPreviousCount );
		API_Leave(  );
	}
	return retv;
#endif
}

//    MUTEX_CREATE,
typedef HANDLE ( WINAPI * PMUTEX_CREATE )( LPSECURITY_ATTRIBUTES lpAttr, BOOL bInitialOwner, LPCTSTR lpcszName );
HANDLE WINAPI Mutex_Create( LPSECURITY_ATTRIBUTES lpAttr,
						   BOOL bInitialOwner, LPCTSTR lpcszName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MUTEX_CREATE, 3 );
    cs.arg0 = (DWORD)lpAttr;
    return (HANDLE)CALL_SERVER( &cs, bInitialOwner, lpcszName );
#else

	PMUTEX_CREATE pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, MUTEX_CREATE, &pfn, &cs ) )
	{
		retv = pfn( lpAttr, bInitialOwner, lpcszName );
		API_Leave(  );
	}
	return retv;
#endif
}

//    MUTEX_RELEASE,
typedef BOOL ( WINAPI * PMUTEX_RELEASE )( HANDLE hMutex );
BOOL WINAPI Mutex_Release( HANDLE hMutex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MUTEX_RELEASE, 1 );
    cs.arg0 = (DWORD)hMutex ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMUTEX_RELEASE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, MUTEX_RELEASE, &pfn, &cs ) )
	{
		retv = pfn( hMutex );
		API_Leave(  );
	}
	return retv;
#endif
}

//    EVENT_CREATE,
typedef HANDLE ( WINAPI * PEVENT_CREATE )(
      LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
  	  BOOL bManualReset, 
	  BOOL bInitialState, 
	  LPCTSTR lpName );

HANDLE WINAPI Event_Create(
      LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
  	  BOOL bManualReset, 
	  BOOL bInitialState, 
	  LPCTSTR lpName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, EVENT_CREATE, 4 );
    cs.arg0 = (DWORD)lpEventAttributes;
    return (HANDLE)CALL_SERVER( &cs, bManualReset, bInitialState, lpName );
#else

	PEVENT_CREATE pfn;

	CALLSTACK cs;
	HANDLE  retv = NULL;

	if( API_Enter( API_KERNEL, EVENT_CREATE, &pfn, &cs ) )
	{
		retv = pfn( lpEventAttributes, bManualReset, bInitialState, lpName );
		API_Leave(  );
	}
	return retv;
#endif
}


//    EVENT_RESET,
typedef BOOL ( WINAPI * PEVENT_RESET )( HANDLE hEvent );
BOOL WINAPI Event_Reset( HANDLE hEvent )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, EVENT_RESET, 1 );
    cs.arg0 = (DWORD)hEvent ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PEVENT_RESET pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, EVENT_RESET, &pfn, &cs ) )
	{
		retv = pfn( hEvent );
		API_Leave(  );
	}
	return retv;
#endif
}

//EVENT_SET,
typedef BOOL ( WINAPI * PEVENT_SET )( HANDLE hEvent );
BOOL WINAPI Event_Set( HANDLE hEvent )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, EVENT_SET, 1 );
    cs.arg0 = (DWORD)hEvent ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PEVENT_SET pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, EVENT_SET, &pfn, &cs ) )
	{
		retv = pfn( hEvent );
		API_Leave(  );
	}
	return retv;
#endif
}

//EVENT_PULSE,
typedef BOOL ( WINAPI * PEVENT_PULSE )( HANDLE hEvent );
BOOL WINAPI Event_Pulse( HANDLE hEvent )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, EVENT_PULSE, 1 );
    cs.arg0 = (DWORD)hEvent ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PEVENT_PULSE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, EVENT_PULSE, &pfn, &cs ) )
	{
		retv = pfn( hEvent );
		API_Leave(  );
	}
	return retv;
#endif
}


//    CRITICALSECTION_INITIALIZE,
//void WINAPI ddd( DWORD * lp )
//{
//	lp++;
//}
typedef VOID ( WINAPI * PCRITICALSECTION_INITIALIZE )( LPCRITICAL_SECTION lpCriticalSection );

VOID WINAPI CriticalSection_Initialize( LPCRITICAL_SECTION lpCriticalSection )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, CRITICALSECTION_INITIALIZE, 1 );
    cs.arg0 = (DWORD)lpCriticalSection ;
    CALL_SERVER( &cs );
	//return ddd( lpCriticalSection );
#else

	PCRITICALSECTION_INITIALIZE pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, CRITICALSECTION_INITIALIZE, &pfn, &cs ) )
	{
		pfn( lpCriticalSection );
		API_Leave(  );
	}
	//return retv;
#endif
}

//    CRITICALSECTION_DELETE,
typedef VOID ( WINAPI * PCRITICALSECTION_DELETE )( LPCRITICAL_SECTION lpCriticalSection );
VOID WINAPI CriticalSection_Delete( LPCRITICAL_SECTION lpCriticalSection )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, CRITICALSECTION_DELETE, 1 );
    cs.arg0 = (DWORD)lpCriticalSection ;
    CALL_SERVER( &cs );
#else

	PCRITICALSECTION_DELETE pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, CRITICALSECTION_DELETE, &pfn, &cs ) )
	{
		pfn( lpCriticalSection );
		API_Leave(  );
	}
	//return retv;
#endif
}

//    CRITICALSECTION_LEAVE,
typedef VOID ( WINAPI * PCRITICALSECTION_LEAVE )( LPCRITICAL_SECTION lpCriticalSection );
VOID WINAPI CriticalSection_Leave( LPCRITICAL_SECTION lpCriticalSection )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, CRITICALSECTION_LEAVE, 1 );
    cs.arg0 = (DWORD)lpCriticalSection ;
    CALL_SERVER( &cs );
#else

	PCRITICALSECTION_LEAVE pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, CRITICALSECTION_LEAVE, &pfn, &cs ) )
	{
		pfn( lpCriticalSection );
		API_Leave(  );
	}
	//return retv;
#endif
}

//    CRITICALSECTION_ENTER,
typedef VOID ( WINAPI * PCRITICALSECTION_ENTER )( LPCRITICAL_SECTION lpCriticalSection );
VOID WINAPI CriticalSection_Enter( LPCRITICAL_SECTION lpCriticalSection )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, CRITICALSECTION_ENTER, 1 );
    cs.arg0 = (DWORD)lpCriticalSection ;
    CALL_SERVER( &cs );
#else

	PCRITICALSECTION_ENTER pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, CRITICALSECTION_ENTER, &pfn, &cs ) )
	{
		pfn( lpCriticalSection );
		API_Leave(  );
	}
	//return retv;
#endif
}

//    CRITICALSECTION_TRYENTER,
typedef BOOL ( WINAPI * PCRITICALSECTION_TRYENTER )( LPCRITICAL_SECTION lpCriticalSection );
BOOL WINAPI CriticalSection_TryEnter( LPCRITICAL_SECTION lpCriticalSection )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, CRITICALSECTION_TRY_ENTER, 1 );
    cs.arg0 = (DWORD)lpCriticalSection ;
    return CALL_SERVER( &cs );
#else

	PCRITICALSECTION_TRYENTER pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, CRITICALSECTION_TRY_ENTER, &pfn, &cs ) )
	{
		retv = pfn( lpCriticalSection );
		API_Leave(  );
	}
	return retv;
#endif
}



// tls
//    TLS_ALLOC,
typedef DWORD ( WINAPI * PTLS_ALLOC )(VOID);
DWORD WINAPI Tls_Alloc(VOID)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, TLS_ALLOC, 1 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PTLS_ALLOC pfn;

	CALLSTACK cs;
	DWORD   retv = TLS_OUT_OF_INDEXES;

	if( API_Enter( API_KERNEL, TLS_ALLOC, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

//    TLS_FREE,
typedef BOOL ( WINAPI * PTLS_FREE )( DWORD dwTlsIndex );
BOOL WINAPI Tls_Free( DWORD dwTlsIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, TLS_FREE, 1 );
    cs.arg0 = (DWORD)dwTlsIndex ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PTLS_FREE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, TLS_FREE, &pfn, &cs ) )
	{
		retv = pfn( dwTlsIndex );
		API_Leave(  );
	}
	return retv;
#endif
}

//    TLS_GETVALUE,
typedef LPVOID ( WINAPI * PTLS_GETVALUE )( DWORD dwTlsIndex );
LPVOID WINAPI Tls_GetValue( DWORD dwTlsIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, TLS_GETVALUE, 1 );
    cs.arg0 = (DWORD)dwTlsIndex ;
    return (LPVOID)CALL_SERVER( &cs );
#else

	PTLS_GETVALUE pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, TLS_GETVALUE, &pfn, &cs ) )
	{
		retv = pfn( dwTlsIndex );
		API_Leave(  );
	}
	return retv;
#endif
}

//    TLS_SETVALUE,
typedef BOOL ( WINAPI * PTLS_SETVALUE )( DWORD dwTlsIndex, LPVOID lpValue );
BOOL WINAPI Tls_SetValue( DWORD dwTlsIndex, LPVOID lpValue )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, TLS_SETVALUE, 2 );
    cs.arg0 = (DWORD)dwTlsIndex;
    return (DWORD)CALL_SERVER( &cs, lpValue );
#else

	PTLS_SETVALUE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, TLS_SETVALUE, &pfn, &cs ) )
	{
		retv = pfn( dwTlsIndex, lpValue );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LPVOID ( WINAPI * PTLS_GETTHREADVALUE )( DWORD dwThreadId, DWORD dwTlsIndex );
LPVOID WINAPI Tls_GetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, TLS_GETTHREADVALUE, 2 );
    cs.arg0 = (DWORD)dwThreadId;
    return (LPVOID)CALL_SERVER( &cs, dwTlsIndex );
#else

	PTLS_GETTHREADVALUE pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, TLS_GETTHREADVALUE, &pfn, &cs ) )
	{
		retv = pfn( dwThreadId, dwTlsIndex );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTLS_SETTHREADVALUE )( DWORD dwThreadId, DWORD dwTlsIndex, LPVOID lpvData );
BOOL WINAPI Tls_SetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex, LPVOID lpvData )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, TLS_SETTHREADVALUE, 3 );
    cs.arg0 = (DWORD)dwThreadId;
    return (BOOL)CALL_SERVER( &cs, dwTlsIndex, lpvData );
#else

	PTLS_SETTHREADVALUE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, TLS_SETTHREADVALUE, &pfn, &cs ) )
	{
		retv = pfn( dwThreadId, dwTlsIndex, lpvData );
		API_Leave(  );
	}
	return retv;
#endif
}


// misc
//    SYS_WAITFORSINGLEOBJECT,
typedef DWORD ( WINAPI * PSYS_WAITFORSINGLEOBJECT )( HANDLE handle, DWORD dwMilliseconds );
DWORD WINAPI Sys_WaitForSingleObject( HANDLE handle, DWORD dwMilliseconds )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_WAITFORSINGLEOBJECT, 2 );
    cs.arg0 = (DWORD)handle;
    return (DWORD)CALL_SERVER( &cs, dwMilliseconds );
#else

	PSYS_WAITFORSINGLEOBJECT pfn;

	CALLSTACK cs;
	DWORD   retv = WAIT_FAILED;

	if( API_Enter( API_KERNEL, SYS_WAITFORSINGLEOBJECT, &pfn, &cs ) )
	{
		retv = pfn( handle, dwMilliseconds );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef DWORD ( WINAPI * PSYS_WAITFORMULTIPLEOBJECTS )( DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwMilliseconds );
DWORD WINAPI Sys_WaitForMultipleObjects( DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwMilliseconds )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_WAITFORMULTIPLEOBJECTS, 4 );
    cs.arg0 = (DWORD)nCount;
    return (DWORD)CALL_SERVER( &cs, lpHandles, fWaitAll, dwMilliseconds );
#else

	PSYS_WAITFORMULTIPLEOBJECTS pfn;

	CALLSTACK cs;
	DWORD   retv = WAIT_FAILED;

	if( API_Enter( API_KERNEL, SYS_WAITFORMULTIPLEOBJECTS, &pfn, &cs ) )
	{
		retv = pfn( nCount, lpHandles, fWaitAll, dwMilliseconds );
		API_Leave(  );
	}
	return retv;
#endif
}


//	SYS_ALLOCAPIHANDLE,
typedef HANDLE ( WINAPI * PSYS_ALLOCAPIHANDLE )( UINT uiAPIId, LPVOID lpvData, UINT objType );
HANDLE WINAPI Sys_AllocAPIHandle( UINT uiAPIId, LPVOID lpvData, UINT objType )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_ALLOCAPIHANDLE, 3 );
    cs.arg0 = (DWORD)uiAPIId;
    return (HANDLE)CALL_SERVER( &cs, lpvData, objType );
#else

	PSYS_ALLOCAPIHANDLE pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, SYS_ALLOCAPIHANDLE, &pfn, &cs ) )
	{
		retv = pfn( uiAPIId, lpvData, objType );
		API_Leave(  );
	}
	return retv;
#endif
}

//	SYS_CLOSEHANDLE,
typedef BOOL ( WINAPI * PSYS_CLOSEHANDLE )( HANDLE handle );
BOOL WINAPI Sys_CloseHandle( HANDLE handle )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_CLOSEHANDLE, 1 );
    cs.arg0 = (DWORD)handle ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PSYS_CLOSEHANDLE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_CLOSEHANDLE, &pfn, &cs ) )
	{
		retv = pfn( handle );
		API_Leave(  );
	}
	return retv;
#endif
}

//	SYS_SLEEP,
typedef VOID ( WINAPI * PSYS_SLEEP )( DWORD dwMilliseconds );
VOID WINAPI Sys_Sleep( DWORD dwMilliseconds )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_SLEEP, 1 );
    cs.arg0 = (DWORD)dwMilliseconds ;
    CALL_SERVER( &cs );
#else

	PSYS_SLEEP pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_SLEEP, &pfn, &cs ) )
	{
		pfn( dwMilliseconds );
		API_Leave(  );
	}
	//return retv;
#endif
}

//	SYS_GETTICKCOUNT,
typedef DWORD ( WINAPI * PSYS_GETTICKCOUNT )( void );
DWORD WINAPI Sys_GetTickCount( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_GETTICKCOUNT, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PSYS_GETTICKCOUNT pfn;

	CALLSTACK cs;
	DWORD   retv = 0;

	if( API_Enter( API_KERNEL, SYS_GETTICKCOUNT, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

//	TIME_GETSYSTEM
typedef VOID ( WINAPI * PSYS_GETSYSTEMTIME )( LPSYSTEMTIME lpst );
VOID WINAPI Sys_GetSystemTime( LPSYSTEMTIME lpst )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_GETSYSTEMTIME, 1 );
    cs.arg0 = (DWORD)lpst ;
    CALL_SERVER( &cs );
#else

	PSYS_GETSYSTEMTIME pfn;

	CALLSTACK cs;
	//DWORD   retv = 0;

	if( API_Enter( API_KERNEL, SYS_GETSYSTEMTIME, &pfn, &cs ) )
	{
		pfn(lpst);
		API_Leave(  );
	}
	//return retv;
#endif
}

typedef BOOL ( WINAPI * PSYS_SETSYSTEMTIME )( const SYSTEMTIME *lpst );
BOOL WINAPI Sys_SetSystemTime( const SYSTEMTIME *lpst )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_SETSYSTEMTIME, 1 );
    cs.arg0 = (DWORD)lpst ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PSYS_SETSYSTEMTIME pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_SETSYSTEMTIME, &pfn, &cs ) )
	{
		retv = pfn(lpst);
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PSYS_EXIT )( UINT uFlags, DWORD dwReserved );
BOOL WINAPI Sys_Exit( UINT uFlags, DWORD dwReserved )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_EXIT, 2 );
    cs.arg0 = (DWORD)uFlags;
    return (DWORD)CALL_SERVER( &cs, dwReserved );
#else

	PSYS_EXIT pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_EXIT, &pfn, &cs ) )
	{
		retv = pfn( uFlags, dwReserved );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef VOID ( WINAPI * PSYS_GETINFO )( LPSYSTEM_INFO lpsi );
VOID WINAPI Sys_GetInfo( LPSYSTEM_INFO lpsi )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_GETINFO, 1 );
    cs.arg0 = (DWORD)lpsi ;
    CALL_SERVER( &cs );
#else
	PSYS_GETINFO pfn;
	CALLSTACK cs;
	//BOOL retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_GETINFO, &pfn, &cs ) )
	{
		pfn( lpsi );
		API_Leave(  );
	}
	//return retv;
#endif
}

typedef BOOL ( WINAPI * PSYS_GETVERSION )( LPOSVERSIONINFO lpvi );
BOOL WINAPI Sys_GetVersion( LPOSVERSIONINFO lpvi )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_GETVERSION, 1 );
    cs.arg0 = (DWORD)lpvi ;
    return (DWORD)CALL_SERVER( &cs );
#else
	PSYS_GETVERSION pfn;
	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_GETVERSION, &pfn, &cs ) )
	{
		retv = pfn( lpvi );
		API_Leave(  );
	}
	return retv;
#endif
}

//    INTERLOCK_INCREMENT,
typedef LONG ( WINAPI * PINTERLOCK_INCREMENT )( LPLONG lpDest );
LONG WINAPI Interlock_Increment( LPLONG lpDest )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTERLOCK_INCREMENT, 1 );
    cs.arg0 = (DWORD)lpDest ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PINTERLOCK_INCREMENT pfn;

	CALLSTACK cs;
	LONG   retv = 0;

	if( API_Enter( API_KERNEL, INTERLOCK_INCREMENT, &pfn, &cs ) )
	{
		retv = pfn( lpDest );
		API_Leave(  );
	}
	return retv;
#endif
}

//    INTERLOCK_DECREMENT,
typedef LONG ( WINAPI * PINTERLOCK_DECREMENT )( LPLONG lpDest );
LONG WINAPI Interlock_Decrement( LPLONG lpDest )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTERLOCK_DECREMENT, 1 );
    cs.arg0 = (DWORD)lpDest ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PINTERLOCK_DECREMENT pfn;

	CALLSTACK cs;
	LONG   retv = 0;

	if( API_Enter( API_KERNEL, INTERLOCK_DECREMENT, &pfn, &cs ) )
	{
		retv = pfn(lpDest);
		API_Leave(  );
	}
	return retv;
#endif
}

//    INTERLOCK_EXCHANGE,
typedef LONG ( WINAPI * PINTERLOCK_EXCHANGE )( LPLONG lpDest, LONG lNewValue );
LONG WINAPI Interlock_Exchange( LPLONG lpDest, LONG lNewValue )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTERLOCK_EXCHANGE, 2 );
    cs.arg0 = (DWORD)lpDest;
    return (DWORD)CALL_SERVER( &cs, lNewValue );
#else

	PINTERLOCK_EXCHANGE pfn;

	CALLSTACK cs;
	LONG   retv = 0;

	if( API_Enter( API_KERNEL, INTERLOCK_EXCHANGE, &pfn, &cs ) )
	{
		retv = pfn(lpDest, lNewValue);
		API_Leave(  );
	}
	return retv;
#endif
}

//    INTERLOCK_EXCHANGEADD,
typedef LONG ( WINAPI * PINTERLOCK_EXCHANGEADD )( LPLONG lpDest, LONG lIncrement );
LONG WINAPI Interlock_ExchangeAdd( LPLONG lpDest, LONG lIncrement )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTERLOCK_EXCHANGEADD, 2 );
    cs.arg0 = (DWORD)lpDest;
    return (DWORD)CALL_SERVER( &cs, lIncrement );
#else

	PINTERLOCK_EXCHANGEADD pfn;

	CALLSTACK cs;
	LONG   retv = 0;

	if( API_Enter( API_KERNEL, INTERLOCK_EXCHANGEADD, &pfn, &cs ) )
	{
		retv = pfn(lpDest, lIncrement);
		API_Leave(  );
	}
	return retv;
#endif
}

//    INTERLOCK_COMPAREEXCHANGE,
typedef LONG ( WINAPI * PINTERLOCK_COMPAREEXCHANGE )( LPLONG lpDest, LONG lExchange, LONG lComperand );
LONG WINAPI Interlock_CompareExchange( LPLONG lpDest, LONG lExchange, LONG lComperand )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTERLOCK_COMPAREEXCHANGE, 3 );
    cs.arg0 = (DWORD)lpDest;
    return (DWORD)CALL_SERVER( &cs, lExchange, lComperand );
#else

	PINTERLOCK_COMPAREEXCHANGE pfn;

	CALLSTACK cs;
	LONG   retv = 0;

	if( API_Enter( API_KERNEL, INTERLOCK_COMPAREEXCHANGE, &pfn, &cs ) )
	{
		retv = pfn(lpDest, lExchange, lComperand);
		API_Leave(  );
	}
	return retv;
#endif
}

// module
//	MODULE_GETPROCADDRESS,
typedef FARPROC ( WINAPI * PMODULE_GETPROCADDRESS )( HMODULE hModule, LPCTSTR lpProcName );
FARPROC WINAPI Module_GetProcAddress( HMODULE hModule, LPCTSTR lpProcName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_GETPROCADDRESS, 2 );
    cs.arg0 = (DWORD)hModule;
    return (FARPROC)CALL_SERVER( &cs, lpProcName );
#else

	PMODULE_GETPROCADDRESS pfn;

	CALLSTACK cs;
	FARPROC   retv = NULL;

	if( API_Enter( API_KERNEL, MODULE_GETPROCADDRESS, &pfn, &cs ) )
	{
		retv = pfn( hModule, lpProcName );
		API_Leave(  );
	}
	return retv;
#endif
}

//	MODULE_GETFILENAME,
typedef DWORD ( WINAPI * PMODULE_GETFILENAME )( HMODULE hModule, LPTSTR lpFilename, DWORD nSize );
DWORD WINAPI Module_GetFileName( HMODULE hModule, LPTSTR lpFilename, DWORD nSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_GETFILENAME, 3 );
    cs.arg0 = (DWORD)hModule;
    return (DWORD)CALL_SERVER( &cs, lpFilename, nSize );
#else

	PMODULE_GETFILENAME pfn;

	CALLSTACK cs;
	DWORD   retv = 0;

	if( API_Enter( API_KERNEL, MODULE_GETFILENAME, &pfn, &cs ) )
	{
		retv = pfn( hModule, lpFilename, nSize );
		API_Leave(  );
	}
	return retv;
#endif
}

// MODULE_LOADLIBRARYEX
typedef HMODULE ( WINAPI * PMODULE_LOADLIBRARYEX )( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags );
HMODULE WINAPI Module_LoadLibraryEx( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_LOADLIBRARYEX, 3 );
    cs.arg0 = (DWORD)lpFileName;
    return (HMODULE)CALL_SERVER( &cs, hFile, dwFlags );
#else

	PMODULE_LOADLIBRARYEX pfn;

	CALLSTACK cs;
	HMODULE   retv = NULL;

	if( API_Enter( API_KERNEL, MODULE_LOADLIBRARYEX, &pfn, &cs ) )
	{
		retv = pfn(lpFileName, hFile, dwFlags);
		API_Leave(  );
	}
	return retv;
#endif
}

//MODULE_FREELIBRARY
typedef BOOL ( WINAPI * PMODULE_FREELIBRARY )( HMODULE hModule );
BOOL WINAPI Module_FreeLibrary( HMODULE hModule )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_FREELIBRARY, 1 );
    cs.arg0 = (DWORD)hModule ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMODULE_FREELIBRARY pfn;

	CALLSTACK cs;
	BOOL   retv = 0;

	if( API_Enter( API_KERNEL, MODULE_FREELIBRARY, &pfn, &cs ) )
	{
		retv = pfn(hModule);
		API_Leave(  );
	}
	return retv;
#endif
}


//MODULE_GETHANDLE
typedef HMODULE ( WINAPI * PMODULE_GETHANDLE )( LPCTSTR lpModuleName );
HMODULE WINAPI Module_GetHandle( LPCTSTR lpModuleName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_GETHANDLE, 1 );
    cs.arg0 = (DWORD)lpModuleName ;
    return (HMODULE)CALL_SERVER( &cs );
#else

	PMODULE_GETHANDLE pfn;

	CALLSTACK cs;
	HMODULE   retv = NULL;

	if( API_Enter( API_KERNEL, MODULE_GETHANDLE, &pfn, &cs ) )
	{
		retv = pfn(lpModuleName);
		API_Leave(  );
	}
	return retv;
#endif
}

//MODULE_FREELIBRARYANDEXITTHREAD
typedef VOID ( WINAPI * PMODULE_FREELIBRARYANDEXITTHREAD )( HMODULE hModule, DWORD dwExitCode );
VOID WINAPI Module_FreeLibraryAndExitThread( HMODULE hModule, DWORD dwExitCode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_FREELIBRARYANDEXITTHREAD, 2 );
    cs.arg0 = (DWORD)hModule;
    CALL_SERVER( &cs, dwExitCode );
#else

	PMODULE_FREELIBRARYANDEXITTHREAD pfn;

	CALLSTACK cs;
	//DWORD   retv = 0;

	if( API_Enter( API_KERNEL, MODULE_FREELIBRARYANDEXITTHREAD, &pfn, &cs ) )
	{
		pfn(hModule, dwExitCode);
		API_Leave(  );
	}
	//return retv;
#endif
}

//MODULE_DISABLETHREADLIBRARYCALLS
typedef BOOL ( WINAPI * PMODULE_DISABLETHREADLIBRARYCALLS )( HMODULE hModule );
BOOL WINAPI Module_DisableThreadLibraryCalls( HMODULE hModule )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_DISABLETHREADLIBRARYCALLS, 1 );
    cs.arg0 = (DWORD)hModule ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMODULE_DISABLETHREADLIBRARYCALLS pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, MODULE_DISABLETHREADLIBRARYCALLS, &pfn, &cs ) )
	{
		retv = pfn(hModule);
		API_Leave(  );
	}
	return retv;
#endif
}

//MODULE_GETFILEHANDLE
typedef HANDLE ( WINAPI * PMODULE_GETFILEHANDLE )( HMODULE hModule );
HANDLE WINAPI Module_GetFileHandle( HMODULE hModule )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_GETFILEHANDLE, 1 );
    cs.arg0 = (DWORD)hModule ;
    return (HANDLE)CALL_SERVER( &cs );
#else

	PMODULE_GETFILEHANDLE pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, MODULE_GETFILEHANDLE, &pfn, &cs ) )
	{
		retv = pfn(hModule);
		API_Leave(  );
	}
	return retv;
#endif
}

//MODULE_GETSECTIONOFFSET
typedef DWORD ( WINAPI * PMODULE_GETSECTIONOFFSET )( HMODULE hModule, LPCTSTR lpcszSectionName );
DWORD WINAPI Module_GetSectionOffset( HMODULE hModule, LPCTSTR lpcszSectionName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_GETSECTIONOFFSET, 2 );
    cs.arg0 = (DWORD)hModule;
    return (DWORD)CALL_SERVER( &cs, lpcszSectionName );
#else

	PMODULE_GETSECTIONOFFSET pfn;

	CALLSTACK cs;
	DWORD   retv = -1;

	if( API_Enter( API_KERNEL, MODULE_GETSECTIONOFFSET, &pfn, &cs ) )
	{
		retv = pfn(hModule,lpcszSectionName);
		API_Leave(  );
	}
	return retv;
#endif
}


// ERROR_SETLAST
typedef void ( WINAPI * PERROR_SETLAST )( DWORD dwErrorCode );
void WINAPI Error_SetLast( DWORD dwErrorCode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, ERROR_SETLAST, 1 );
    cs.arg0 = (DWORD)dwErrorCode ;
    CALL_SERVER( &cs );
#else

	PERROR_SETLAST pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, ERROR_SETLAST, &pfn, &cs ) )
	{
		pfn(dwErrorCode);
		API_Leave(  );
	}
	//return retv;
#endif
}

typedef DWORD ( WINAPI * PERROR_GETLAST )( void );
DWORD WINAPI Error_GetLast( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, ERROR_GETLAST, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PERROR_GETLAST pfn;

	CALLSTACK cs;
	DWORD   retv = 0;

	if( API_Enter( API_KERNEL, ERROR_GETLAST, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LPVOID ( WINAPI * PMEM_ALLOCPHYSICAL )( DWORD dwSize, DWORD fdwProtect, DWORD dwAlignmentMask, DWORD dwFlags, ULONG * pPhysicalAddress );
LPVOID WINAPI Mem_AllocPhysical( DWORD dwSize, 
								 DWORD fdwProtect,
								 DWORD dwAlignmentMask,
								 DWORD dwFlags,
								 ULONG * pPhysicalAddress )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MEM_ALLOCPHYSICAL, 5 );
    cs.arg0 = (DWORD)dwSize;
    return (LPVOID)CALL_SERVER( &cs, fdwProtect, dwAlignmentMask, dwFlags, pPhysicalAddress );
#else

	PMEM_ALLOCPHYSICAL pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, MEM_ALLOCPHYSICAL, &pfn, &cs ) )
	{
		retv = pfn( dwSize, fdwProtect, dwAlignmentMask, dwFlags, pPhysicalAddress );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMEM_FREEPHYSICAL )( LPVOID lpvAddress );

BOOL WINAPI Mem_FreePhysical( LPVOID lpvAddress )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MEM_FREEPHYSICAL, 1 );
    cs.arg0 = (DWORD)lpvAddress ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMEM_FREEPHYSICAL pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, MEM_FREEPHYSICAL, &pfn, &cs ) )
	{
		retv = pfn( lpvAddress );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LPVOID ( WINAPI * PMEM_VIRTUALALLOC )( LPVOID lpAddress, DWORD dwSize, DWORD dwAllocationType, DWORD dwProtect );
LPVOID WINAPI Mem_VirtualAlloc( LPVOID lpAddress,
							    DWORD dwSize, 
								DWORD dwAllocationType, 
								DWORD dwProtect )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MEM_VIRTUALALLOC, 4 );
    cs.arg0 = (DWORD)lpAddress;
    return (LPVOID)CALL_SERVER( &cs, dwSize, dwAllocationType, dwProtect );
#else

	PMEM_VIRTUALALLOC pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, MEM_VIRTUALALLOC, &pfn, &cs ) )
	{
		retv = pfn( lpAddress, dwSize, dwAllocationType, dwProtect );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMEM_VIRTUALFREE )( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType );
BOOL WINAPI Mem_VirtualFree( LPVOID lpAddress,
							 DWORD dwSize,
							 DWORD dwFreeType )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MEM_VIRTUALFREE, 3 );
    cs.arg0 = (DWORD)lpAddress;
    return (DWORD)CALL_SERVER( &cs, dwSize, dwFreeType );
#else

	PMEM_VIRTUALFREE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, MEM_VIRTUALFREE, &pfn, &cs ) )
	{
		retv = pfn( lpAddress, dwSize, dwFreeType );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMEM_VIRTUALCOPY )( LPVOID lpvDest, LPVOID lpvSrc, DWORD cbSize, DWORD fdwProtect );
BOOL WINAPI Mem_VirtualCopy( LPVOID lpvDest, LPVOID lpvSrc, DWORD cbSize, DWORD fdwProtect )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MEM_VIRTUALCOPY, 4 );
    cs.arg0 = (DWORD)lpvDest;
    return (DWORD)CALL_SERVER( &cs, lpvSrc, cbSize, fdwProtect );
#else

	PMEM_VIRTUALCOPY pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, MEM_VIRTUALCOPY, &pfn, &cs ) )
	{
		retv = pfn( lpvDest, lpvSrc, cbSize, fdwProtect );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMEM_VIRTUALPROTECT )( LPVOID lpvAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect );
BOOL WINAPI Mem_VirtualProtect( LPVOID lpvAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MEM_VIRTUALPROTECT, 4 );
    cs.arg0 = (DWORD)lpvAddress;
    return (DWORD)CALL_SERVER( &cs, dwSize, flNewProtect, lpflOldProtect );
#else

	PMEM_VIRTUALPROTECT pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, MEM_VIRTUALPROTECT, &pfn, &cs ) )
	{
		retv = pfn( lpvAddress, dwSize, flNewProtect, lpflOldProtect );
		API_Leave(  );
	}
	return retv;
#endif
}


// heap manger
typedef HANDLE ( WINAPI * PHEAP_CREATE )( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize );
HANDLE WINAPI Heap_Create( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, HEAP_CREATE, 3 );
    cs.arg0 = (DWORD)flOptions;
    return (HANDLE)CALL_SERVER( &cs, dwInitialSize, dwMaximumSize );
#else

	PHEAP_CREATE pfn;

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_KERNEL, HEAP_CREATE, &pfn, &cs ) )
	{
		retv = pfn( flOptions, dwInitialSize, dwMaximumSize );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PHEAP_DESTROY )( HANDLE hHeap );
BOOL WINAPI Heap_Destroy( HANDLE hHeap )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, HEAP_DESTROY, 1 );
    cs.arg0 = (DWORD)hHeap ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PHEAP_DESTROY pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, HEAP_DESTROY, &pfn, &cs ) )
	{
		retv = pfn( hHeap );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LPVOID ( WINAPI * PHEAP_ALLOC )( HANDLE hHeap, DWORD dwFlags, ULONG dwBytes );
LPVOID WINAPI Heap_Alloc( HANDLE hHeap, DWORD dwFlags, ULONG dwBytes )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, HEAP_ALLOC, 3 );
    cs.arg0 = (DWORD)hHeap;
    return (LPVOID)CALL_SERVER( &cs, dwFlags, dwBytes );
#else

	PHEAP_ALLOC pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, HEAP_ALLOC, &pfn, &cs ) )
	{
		retv = pfn( hHeap, dwFlags, dwBytes );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PHEAP_FREE )( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );
BOOL WINAPI Heap_Free( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, HEAP_FREE, 3 );
    cs.arg0 = (DWORD)hHeap;
    return (DWORD)CALL_SERVER( &cs, dwFlags, lpMem );
#else

	PHEAP_FREE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, HEAP_FREE, &pfn, &cs ) )
	{
		retv = pfn( hHeap, dwFlags, lpMem );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LPVOID ( WINAPI * PHEAP_REALLOC )( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, ULONG dwBytes );
LPVOID WINAPI Heap_ReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, ULONG dwBytes )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, HEAP_REALLOC, 4 );
    cs.arg0 = (DWORD)hHeap;
    return (LPVOID)CALL_SERVER( &cs, dwFlags, lpMem, dwBytes );
#else

	PHEAP_REALLOC pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, HEAP_REALLOC, &pfn, &cs ) )
	{
		retv = pfn( hHeap, dwFlags, lpMem, dwBytes );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PHEAP_VALIDATE )( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem );
BOOL WINAPI Heap_Validate( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, HEAP_VALIDATE, 3 );
    cs.arg0 = (DWORD)hHeap;
    return (DWORD)CALL_SERVER( &cs, dwFlags, lpMem );
#else

	PHEAP_VALIDATE pfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_KERNEL, HEAP_VALIDATE, &pfn, &cs ) )
	{
		retv = pfn( hHeap, dwFlags, lpMem );
		API_Leave(  );
	}
	return retv;
#endif
}


// debug
typedef BOOL ( WINAPI * PDBG_OUTSTRING )( LPTSTR lpszString );
BOOL WINAPI Dbg_OutString( LPTSTR lpszString )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, DBG_OUTSTRING, 1 );
    cs.arg0 = (DWORD)lpszString ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PDBG_OUTSTRING pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, DBG_OUTSTRING, &pfn, &cs ) )
	{
		retv = pfn( lpszString );
		API_Leave(  );
	}
	return retv;
#endif
}

// power manager
typedef void ( WINAPI * PPOWER_SYSTEMOFF )( void );
void WINAPI Power_SystemOff( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, POWER_SYSTEMOFF, 0 );
    cs.arg0 = 0;
    CALL_SERVER( &cs );
#else

	PPOWER_SYSTEMOFF pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, POWER_SYSTEMOFF, &pfn, &cs ) )
	{
		pfn();
		API_Leave(  );
	}
	//return retv;
#endif
}

// elf dll open 
//int __dlopen_dependence (struct __dlmodule* module, unsigned int mode)
//struct __dlmodule* __dlopen_module ( const char * name, unsigned int mode, 
//						   struct __dlmodule *caller )


typedef LPVOID ( WINAPI * PDLL_OPENMODULE )( LPCTSTR lpcszName, UINT uiMode, LPVOID lpvCaller );
LPVOID WINAPI Dll_OpenModule( LPCTSTR lpcszName, UINT uiMode, LPVOID lpvCaller )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, DLL_OPENMODULE, 3 );
    cs.arg0 = (DWORD)lpcszName;
    return (LPVOID)CALL_SERVER( &cs, uiMode, lpvCaller );
#else

	PDLL_OPENMODULE pfn;

	CALLSTACK cs;
	LPVOID   retv = NULL;

	if( API_Enter( API_KERNEL, DLL_OPENMODULE, &pfn, &cs ) )
	{
		retv = pfn( lpcszName, uiMode, lpvCaller );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLL_OPENDEPENDENCE )( LPVOID lpvModule, UINT uiMode );
int WINAPI Dll_OpenDependence( LPVOID lpvModule, UINT uiMode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, DLL_OPENDEPENDENCE, 2 );
    cs.arg0 = (DWORD)lpvModule;
    return (DWORD)CALL_SERVER( &cs, uiMode );
#else

	PDLL_OPENDEPENDENCE pfn;

	CALLSTACK cs;
	int   retv = 0;

	if( API_Enter( API_KERNEL, DLL_OPENDEPENDENCE, &pfn, &cs ) )
	{
		retv = pfn( lpvModule, uiMode );
		API_Leave(  );
	}
	return retv;
#endif
}



// interrupt api
typedef BOOL ( WINAPI * PINTR_INIT )( UINT idInt, HANDLE hIntEvent, LPVOID lpvData, DWORD dwSize );
BOOL WINAPI INTR_Init( UINT idInt, 
				       HANDLE hIntEvent,  // // intr event
				       LPVOID lpvData,   // transfer to OEM_InterruptEnable
				       DWORD dwSize      // transfer to OEM_InterruptEnable
				     )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTR_INIT, 4 );
    cs.arg0 = (DWORD)idInt;
    return (DWORD)CALL_SERVER( &cs, hIntEvent, lpvData, dwSize );
#else
	PINTR_INIT pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, INTR_INIT, &pfn, &cs ) )
	{
		retv = pfn( idInt, hIntEvent, lpvData, dwSize );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef void ( WINAPI * PINTR_DONE )( UINT idInt );
void WINAPI INTR_Done( UINT idInt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTR_DONE, 1 );
    cs.arg0 = (DWORD)idInt ;
    CALL_SERVER( &cs );
#else

	PINTR_DONE pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, INTR_DONE, &pfn, &cs ) )
	{
		pfn( idInt );
		API_Leave(  );
	}
	//return retv;
#endif
}

typedef void ( WINAPI * PINTR_DISABLE )( UINT idInt );
void WINAPI INTR_Disable( UINT idInt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTR_DISABLE, 1 );
    cs.arg0 = (DWORD)idInt ;
    CALL_SERVER( &cs );
#else

	PINTR_DISABLE pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, INTR_DISABLE, &pfn, &cs ) )
	{
		pfn( idInt );
		API_Leave(  );
	}
	//return retv;
#endif
}

typedef void ( WINAPI * PINTR_ENABLE )( UINT idInt, LPVOID lpvData, DWORD dwSize );
void WINAPI INTR_Enable( UINT idInt, LPVOID lpvData, DWORD dwSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, INTR_ENABLE, 3 );
    cs.arg0 = (DWORD)idInt;
    CALL_SERVER( &cs, lpvData, dwSize );
#else

	PINTR_ENABLE pfn;

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_KERNEL, INTR_ENABLE, &pfn, &cs ) )
	{
		pfn( idInt, lpvData, dwSize );
		API_Leave(  );
	}
	//return retv;
#endif
}

// TOC
typedef LPCVOID ( WINAPI * PSYS_GETTOC )( void );
LPCVOID WINAPI Sys_GetTOC( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_GETTOC, 0 );
    cs.arg0 = 0;
    return (LPCVOID)CALL_SERVER( &cs );
#else

	PSYS_GETTOC pfn;

	CALLSTACK cs;
	LPCVOID   retv = NULL;

	if( API_Enter( API_KERNEL, SYS_GETTOC, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn();
		API_Leave( );
	}
	return retv;
#endif
}
//

typedef void ( WINAPI * PMEM_GLOBALSTATUS )( LPMEMORYSTATUS lpms );
void WINAPI Mem_GlobalStatus( LPMEMORYSTATUS lpms )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MEM_GLOBALSTATUS, 1 );
    cs.arg0 = (DWORD)lpms;
    CALL_SERVER( &cs );
#else

	PMEM_GLOBALSTATUS pfn;

	CALLSTACK cs;

	if( API_Enter( API_KERNEL, MEM_GLOBALSTATUS, (LPVOID)&pfn, &cs ) )
	{
		pfn( lpms );
		API_Leave( );
	}
#endif
}

// 
typedef void ( WINAPI * PSYS_PROFILEKERNEL )( UINT uiOption, LPVOID lpv  );
void WINAPI Sys_ProfileKernel( UINT uiOption, LPVOID lpv )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_PROFILEKERNEL, 2 );
    cs.arg0 = (DWORD)uiOption;
    CALL_SERVER( &cs, lpv );
#else

	PSYS_PROFILEKERNEL pfn;

	CALLSTACK cs;

	if( API_Enter( API_KERNEL, SYS_PROFILEKERNEL, (LPVOID)&pfn, &cs ) )
	{
		pfn( uiOption, lpv );
		API_Leave( );
	}
#endif
}

typedef LPTSTR ( WINAPI * PSYS_GETCOMMANDLINE )( VOID  );
LPTSTR WINAPI Sys_GetCommandLine( VOID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_GETCOMMANDLINE, 0 );
    cs.arg0 = 0;
    return (LPTSTR)CALL_SERVER( &cs );
#else

	PSYS_GETCOMMANDLINE pfn;

	CALLSTACK cs;
	LPTSTR retv = NULL;

	if( API_Enter( API_KERNEL, SYS_GETCOMMANDLINE, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn();
		API_Leave( );
	}
	return retv;
#endif
}

typedef LPVOID ( WINAPI * PPROCESS_GETUSERDATA )( VOID  );
LPVOID WINAPI Process_GetUserData( VOID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_GETUSERDATA, 0 );
    cs.arg0 = 0;
    return (LPVOID)CALL_SERVER( &cs );
#else

	PPROCESS_GETUSERDATA pfn;

	CALLSTACK cs;
	LPVOID retv = NULL;

	if( API_Enter( API_KERNEL, PROCESS_GETUSERDATA, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn();
		API_Leave( );
	}
	return retv;
#endif
}

typedef LPVOID ( WINAPI * PPROCESS_SETUSERDATA )( LPVOID lpvData );
LPVOID WINAPI Process_SetUserData( LPVOID lpvData )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_SETUSERDATA, 1 );
    cs.arg0 = (DWORD)lpvData ;
    return (LPVOID)CALL_SERVER( &cs );
#else

	PPROCESS_SETUSERDATA pfn;

	CALLSTACK cs;
	LPVOID retv = NULL;

	if( API_Enter( API_KERNEL, PROCESS_SETUSERDATA, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn( lpvData );
		API_Leave( );
	}
	return retv;
#endif
}

typedef UINT ( WINAPI * PTHREAD_LOCKSCHEDULE )( void );
UINT WINAPI Thread_LockSchedule( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_LOCKSCHEDULE, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PTHREAD_LOCKSCHEDULE pfn;

	CALLSTACK cs;
	UINT retv = -1;

	if( API_Enter( API_KERNEL, THREAD_LOCKSCHEDULE, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn();
		API_Leave();
	}
	return retv;
#endif
}

typedef UINT ( WINAPI * PTHREAD_UNLOCKSCHEDULE )( void );
UINT WINAPI Thread_UnlockSchedule( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_UNLOCKSCHEDULE, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PTHREAD_UNLOCKSCHEDULE pfn;

	CALLSTACK cs;
	UINT retv = -1;

	if( API_Enter( API_KERNEL, THREAD_UNLOCKSCHEDULE, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn();
		API_Leave();
	}
	return retv;
#endif
}

typedef LPVOID ( WINAPI * PPAGE_ALLOCMEM )( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag );

LPVOID WINAPI Page_AllocMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PAGE_ALLOCMEM, 3 );
    cs.arg0 = (DWORD)dwNeedSize;
    return (LPVOID)CALL_SERVER( &cs, lpdwRealSize, uiFlag);
#else
	PPAGE_ALLOCMEM pfn;
	CALLSTACK cs;
	LPVOID retv = NULL;

	if( API_Enter( API_KERNEL, PAGE_ALLOCMEM, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn(dwNeedSize, lpdwRealSize, uiFlag);
		API_Leave();
	}
	return retv;
#endif
}

typedef UINT ( WINAPI * PPAGE_FREEMEM )( LPVOID lp, DWORD dwSize );

BOOL WINAPI Page_FreeMem( LPVOID lp, DWORD dwSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PAGE_FREEMEM, 2 );
    cs.arg0 = (DWORD)lp;
    return (DWORD)CALL_SERVER( &cs, dwSize);
#else

	PPAGE_FREEMEM pfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_KERNEL, PAGE_FREEMEM, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn(lp, dwSize);
		API_Leave();
	}
	return retv;
#endif
}

typedef int ( WINAPI * PTHREAD_GETPRIORITY )( HANDLE );

int WINAPI Thread_GetPriority( HANDLE hThread )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_GETPRIORITY, 1 );
    cs.arg0 = (DWORD)hThread;
    return (DWORD)CALL_SERVER( &cs, 0 );
#else

	PTHREAD_GETPRIORITY pfn;

	CALLSTACK cs;
	int retv = 0;

	if( API_Enter( API_KERNEL, THREAD_GETPRIORITY, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn(hThread);
		API_Leave();
	}
	return retv;
#endif
}

typedef int ( WINAPI * PTHREAD_GETPOLICY )( HANDLE hThread );

BOOL WINAPI Thread_GetPolicy( HANDLE hThread )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_GETPOLICY , 1 );
    cs.arg0 = (DWORD)hThread;
    return (DWORD)CALL_SERVER( &cs );
#else

	PTHREAD_GETPOLICY pfn;

	CALLSTACK cs;
	int retv = 0;

	if( API_Enter( API_KERNEL, THREAD_GETPOLICY, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn(hThread);
		API_Leave();
	}
	return retv;
#endif
}


//HANDLE WINAPI Process_Open( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId );
typedef HANDLE ( WINAPI * PPROCESS_OPEN )( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId );
HANDLE WINAPI Process_Open( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_OPEN , 3 );
    cs.arg0 = (DWORD)dwDesiredAccess;
    return (HANDLE)CALL_SERVER( &cs, bInheritHandle, dwProcessId );
#else

	PPROCESS_OPEN pfn;

	CALLSTACK cs;
	HANDLE retv = 0;

	if( API_Enter( API_KERNEL, PROCESS_OPEN, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn( dwDesiredAccess, bInheritHandle, dwProcessId );
		API_Leave();
	}
	return retv;
#endif
}
/*
typedef UINT ( WINAPI * PSYS_ENTRYTRY )( jmp_buf jmp_data );
UINT WINAPI _Sys_EntryTry( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
	jmp_buf jmp_data;
	UINT retv;
	if( ( retv = sys_setjmp( jmp_data ) ) == 0 )
	{
		RETAILMSG( 1, ( "Sys_EntryTry cs=0x%x, sp=0x%x, lr=0x%x.\r\n", &cs, jmp_data->sp, jmp_data->lr ) );
		cs.apiInfo = CALL_API( API_KERNEL, SYS_ENTRYTRY , 1 );
		cs.arg0 = (DWORD)jmp_data;
		return (DWORD)CALL_SERVER( &cs );
	}
    RETAILMSG( 1, ( "Sys_EntryTry leave.\r\n" ) );
	return retv;
#else

	PSYS_ENTRYTRY pfn;

	CALLSTACK cs;
	UINT retv;
	jmp_buf jmp_data;

	if( ( retv = sys_setjmp( jmp_data ) ) == 0 )
	{
		if( API_Enter( API_KERNEL, SYS_ENTRYTRY, (LPVOID)&pfn, &cs ) )
		{
			retv = pfn( jmp_data );
			API_Leave();
		}
	}
	return retv;
#endif
}
*/

typedef UINT ( WINAPI * PSYS_CAPTURE_EXCEPTION )( LPEXCEPTION_CONTEXT lpctx );
UINT WINAPI _Sys_CaptureException( LPEXCEPTION_CONTEXT lpctx )
{
#ifdef CALL_TRAP

	CALLTRAP cs;
    UINT retv;

	//RETAILMSG( 1, ( "_Sys_CaptureException cs=0x%x,sp=0x%x,lr=0x%x,ip=0x%x.\r\n", &cs, jmp_data->sp, jmp_data->lr, jmp_data->ip ) );
	cs.apiInfo = CALL_API( API_KERNEL, SYS_CAPTUREEXCEPTION , 1 );
	cs.arg0 = (DWORD)lpctx;
	retv = (DWORD)CALL_SERVER( &cs );

    //RETAILMSG( 1, ( "_Sys_EntryTry leave(%d).\r\n", retv ) );
	return retv;
	
#else

	PSYS_CAPTURE_EXCEPTION pfn;

	CALLSTACK cs;
	UINT retv;

	if( API_Enter( API_KERNEL, SYS_CAPTUREEXCEPTION, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn( lpctx );
		API_Leave();
	}
	return retv;
#endif
}

typedef UINT ( WINAPI * PSYS_RELEASE_EXCEPTION )( void );
BOOL WINAPI Sys_ReleaseException( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_RELEASEEXCEPTION , 0 );
    cs.arg0 = 0;
    return CALL_SERVER( &cs );
#else

	PSYS_RELEASE_EXCEPTION pfn;
	CALLSTACK cs;
	UINT retv = 0;

	if( API_Enter( API_KERNEL, SYS_RELEASEEXCEPTION, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn();
		API_Leave();
	}
	return retv;
#endif
}


typedef UINT ( WINAPI * PSYS_GET_COMPUTER_NAME_EX )( COMPUTER_NAME_FORMAT NameType, LPTSTR lpBuffer, LPDWORD lpnSize );
BOOL WINAPI Sys_GetComputerNameEx( COMPUTER_NAME_FORMAT NameType, LPTSTR lpBuffer, LPDWORD lpnSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_GET_COMPUTER_NAME_EX , 3 );
    cs.arg0 = NameType;
    return CALL_SERVER( &cs, lpBuffer, lpnSize );
#else

	PSYS_GET_COMPUTER_NAME_EX pfn;
	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_GET_COMPUTER_NAME_EX, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn(NameType, lpBuffer, lpnSize);
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PSYS_SET_COMPUTER_NAME_EX )( COMPUTER_NAME_FORMAT NameType, LPCTSTR lpBuffer );
BOOL WINAPI Sys_SetComputerNameEx( COMPUTER_NAME_FORMAT NameType, LPCTSTR lpBuffer  )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_SET_COMPUTER_NAME_EX , 2 );
    cs.arg0 = NameType;
    return CALL_SERVER( &cs, lpBuffer );
#else

	PSYS_SET_COMPUTER_NAME_EX pfn;
	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_SET_COMPUTER_NAME_EX, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn(NameType,lpBuffer);
		API_Leave();
	}
	return retv;
#endif
}


typedef BOOL ( WINAPI * PSYS_REGISTRYIO )( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag );
BOOL WINAPI Sys_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_REGISTRYIO , 3 );
    cs.arg0 = (DWORD)lpDataBuf;
    return CALL_SERVER( &cs, nNumberOfBytes, dwFlag );
#else

	PSYS_REGISTRYIO pfn;
	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_KERNEL, SYS_REGISTRYIO, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn(lpDataBuf,nNumberOfBytes, dwFlag);
		API_Leave();
	}
	return retv;
#endif
}

//UINT WINAPI KL_ChangeThreadRotate( DWORD dwThreadId, int nAddRotate )
typedef UINT ( WINAPI * PTHREAD_CHANGE_ROTATE )( DWORD dwThreadId, int nAddRotate );
UINT WINAPI Thread_ChangeRotate( DWORD dwThreadId, int nAddRotate )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_CHANGE_ROTATE , 2 );
    cs.arg0 = dwThreadId;
    return CALL_SERVER( &cs, nAddRotate );
#else

	PTHREAD_CHANGE_ROTATE pfn;
	CALLSTACK cs;
	UINT retv = 0;

	if( API_Enter( API_KERNEL, THREAD_CHANGE_ROTATE, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn( dwThreadId, nAddRotate );
		API_Leave();
	}
	return retv;
#endif
}


typedef BOOL ( WINAPI * PMODULE_RELEASEFILEHANDLE )( HANDLE hModule, HANDLE hFile );
BOOL WINAPI Module_ReleaseFileHandle( HANDLE hModule, HANDLE hFile )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, MODULE_RELEASEFILEHANDLE , 2 );
    cs.arg0 = (DWORD)hModule;
    return CALL_SERVER( &cs, hFile );
#else

	PMODULE_RELEASEFILEHANDLE pfn;
	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_KERNEL, MODULE_RELEASEFILEHANDLE, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn( hModule, hFile );
		API_Leave();
	}
	return retv;
#endif
}


typedef DWORD ( WINAPI * PThread_SetPermissions )( DWORD dwNewPerm );
DWORD WINAPI Thread_SetPermissions( DWORD dwNewPerm )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_SETPERMISSIONS , 1 );
    cs.arg0 = (DWORD)dwNewPerm;
    return CALL_SERVER( &cs );
#else

	PThread_SetPermissions pfn;
	CALLSTACK cs;
	DWORD retv = 0;

	if( API_Enter( API_KERNEL, THREAD_SETPERMISSIONS, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn( dwNewPerm );
		API_Leave();
	}
	return retv;
#endif
}

typedef DWORD ( WINAPI * PThread_GetPermissions )( VOID );
DWORD WINAPI Thread_GetPermissions( VOID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, THREAD_GETPERMISSIONS , 0 );
    cs.arg0 = (DWORD)0;
    return CALL_SERVER( &cs );
#else

	PThread_GetPermissions pfn;
	CALLSTACK cs;
	DWORD retv = 0;

	if( API_Enter( API_KERNEL, THREAD_GETPERMISSIONS, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn();
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PSys_SetAlarmInfo )( LPALARM_INFO lpai );
BOOL WINAPI Sys_SetAlarmInfo( LPALARM_INFO lpai )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_SETALARMINFO, 1 );
    cs.arg0 = (DWORD)lpai;
    return CALL_SERVER( &cs );
#else

	PSys_SetAlarmInfo pfn;
	CALLSTACK cs;
	BOOL retv = 0;

	if( API_Enter( API_KERNEL, SYS_SETALARMINFO, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn( lpai );
		API_Leave();
	}
	return retv;
#endif
}

typedef VOID ( WINAPI * PSys_RefreshAlarm )( DWORD dwFlag );
VOID WINAPI Sys_RefreshAlarm( DWORD dwFlag )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, SYS_REFRESHALARM, 1 );
    cs.arg0 = (DWORD)dwFlag;
    CALL_SERVER( &cs );
#else

	PSys_RefreshAlarm pfn;
	CALLSTACK cs;

	if( API_Enter( API_KERNEL, SYS_REFRESHALARM, (LPVOID)&pfn, &cs ) )
	{
		pfn( dwFlag );
		API_Leave();
	}
#endif
}

DWORD WINAPI Sys_DefServerProc( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
{
	switch( dwServerCode )
	{
	//case SCC_BROADCAST_THREAD_EXIT:
	//	break;
	//case SCC_BROADCAST_PROCESS_EXIT:
	//	break;
	case SCC_BROADCAST_POWER:
		return TRUE;
//	case SCC_BROADCAST_POWERON:
//		return 0;
//	case SCC_BROADCAST_SHUTDOWN:
//		break;
	}
	return 0;
}


typedef DWORD ( WINAPI * PProcess_GetId )( HANDLE hProcess );
DWORD WINAPI Process_GetId( HANDLE hProcess )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KERNEL, PROCESS_GETID, 1 );
    cs.arg0 = (DWORD)hProcess;
    return CALL_SERVER( &cs );
#else

	PProcess_GetId pfn;
	CALLSTACK cs;
	DWORD retv = -1;

	if( API_Enter( API_KERNEL, PROCESS_GETID, (LPVOID)&pfn, &cs ) )
	{
		retv = pfn( hProcess );
		API_Leave();
	}
	return retv;

#endif
}
