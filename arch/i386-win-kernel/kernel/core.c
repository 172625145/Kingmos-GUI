#include <windows.h>
//#include <pthread.h>
//#include <errno.h>
//#include <stdio.h>

//#include <time.h>
//#include <edef.h>
//#include <eframe.h>
//#include "handlepackage.h"
//#include "semaphore.h"

//#define KINGMOS_GWME_THREAD_ID     0xFF094540
//#define KINGMOS_GWME_THREAD_HANDLE 0xFF094541

/*static pthread_key_t   key = 0;

void InitKernel(void)
{
	pthread_key_create(&key,echomsg);
}
*/

VOID WINAPI Sys_Sleep( DWORD dwMilliseconds )
{
	Sleep( dwMilliseconds );
}

DWORD WINAPI Sys_GetTickCount( void )
{
	return GetTickCount();
}

VOID WINAPI Sys_GetSystemTime( LPSYSTEMTIME lpst )
{
	GetSystemTime( lpst );
}

BOOL WINAPI Sys_SetSystemTime( const SYSTEMTIME *lpst )
{
	return SetSystemTime( lpst );
}

DWORD WINAPI Error_GetLast( void )
{
	return GetLastError();
}

void WINAPI Error_SetLast( DWORD dwErrorCode )
{
	SetLastError( dwErrorCode );
}

HANDLE WINAPI Thread_Create(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    DWORD dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpdwThreadId
   )
{
	return CreateThread( lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpdwThreadId );
}

BOOL WINAPI Sys_CloseHandle( HANDLE handle )
{
	return CloseHandle( handle );	
}

HANDLE WINAPI Semaphore_Create( LPSECURITY_ATTRIBUTES lpAttr, int nInitialCount, int nMaximumCount, LPCTSTR lpcszName )
{
	return CreateSemaphore( NULL, nInitialCount, nMaximumCount, lpcszName );
}

BOOL WINAPI Semaphore_Release( HANDLE hSemaphore, int lReleaseCount, LPINT lpPreviousCount )
{
	return ReleaseSemaphore( hSemaphore, lReleaseCount, lpPreviousCount );
}

HANDLE WINAPI Event_Create(
      LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
  	  BOOL bManualReset, 
	  BOOL bInitialState, 
	  LPCTSTR lpName ) 
{	
	return CreateEvent( NULL, bManualReset, bInitialState, lpName );
}

BOOL WINAPI Event_Set( HANDLE hEvent )
{
	return SetEvent( hEvent );
}

BOOL WINAPI Event_Reset( HANDLE hEvent )
{	
	return SetEvent( hEvent );
}

DWORD WINAPI Sys_WaitForSingleObject( HANDLE handle, DWORD dwMilliseconds )
{
	return WaitForSingleObject( handle, dwMilliseconds );
}

DWORD WINAPI Sys_WaitForMultipleObjects( DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwMilliseconds )
{
	return WaitForMultipleObjects( nCount, lpHandles, fWaitAll, dwMilliseconds );
}

DWORD WINAPI Thread_GetCurrentId( void )
{
	return GetCurrentThreadId();
}

DWORD WINAPI Thread_SetPermissions( DWORD dwNewPerm )
{
	//printf( "Thread_SetPermissions entry.\r\n" );

	return -1;
}

LPVOID WINAPI Page_AllocMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag )
{
	LPVOID lpv;

	//printf( "Page_AllocMem entry.\r\n" );

	lpv = malloc( dwNeedSize );

	if( lpv )
	{
		if( lpdwRealSize )
			*lpdwRealSize = dwNeedSize;
	}
	else
	{
		if( lpdwRealSize )
			*lpdwRealSize = 0;
	}

	//printf( "Page_AllocMem leave.\r\n" );
	return lpv;
}

BOOL WINAPI Page_FreeMem( LPVOID lp, DWORD dwSize )
{
	//printf( "Page_FreeMem entry.\r\n" );

	free( lp );
	//printf( "Page_FreeMem leave.\r\n" );

	return TRUE;
}

UINT WINAPI Thread_ChangeRotate( DWORD dwThreadId, int nAddRotate )
{
	//printf( "Thread_ChangeRotate entry.\r\n" );
	return 1;
}

UINT WINAPI Sys_CaptureException( void )
{
	//printf( "Sys_CaptureException entry.\r\n" );
	return 0;
}

BOOL WINAPI Sys_ReleaseException( void )
{
	//printf( "Sys_ReleaseException entry.\r\n" );
	return TRUE;
}

/*
BOOL WINAPI Tls_SetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex, LPVOID lpvData )
{
	//printf( "Tls_SetThreadValue entry.\r\n" );
	if( dwTlsIndex < MAX_THREAD_INDEX )
	{
		//lpvThreadData[dwThreadId&THREAD_ID_MASK][dwTlsIndex] = lpvData;
		threadData[dwThreadId&THREAD_ID_MASK].dwThreadData[dwTlsIndex] = lpvData;
	}
	printf( "Tls_SetThreadValue leave.\r\n" );
	return TRUE;
}

LPVOID WINAPI Tls_GetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex )
{
	//printf( "Tls_GetThreadValue entry.\r\n" );
	if( dwTlsIndex < MAX_THREAD_INDEX )
	{
		//printf( "Tls_GetThreadValue leave0.\r\n" );
		return threadData[dwThreadId&THREAD_ID_MASK].dwThreadData[dwTlsIndex];//lpvThreadData[dwThreadId&THREAD_ID_MASK][dwTlsIndex];
	}
	printf( "Tls_GetThreadValue leave1.\r\n" );
	return NULL;
}
*/

BOOL WINAPI Tls_SetValue( DWORD dwTlsIndex, LPVOID lpValue )
{
	return TlsSetValue( dwTlsIndex, lpValue );
}

LPVOID WINAPI Tls_GetValue( DWORD dwTlsIndex )
{
	return TlsGetValue( dwTlsIndex);
}

DWORD WINAPI Sys_DefServerProc( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
{
//	printf( "Sys_DefServerProc entry.\r\n" );
	return 0;
}


BOOL WINAPI Sys_Exit( UINT uFlags, DWORD dwReserved )
{
//	printf( "Sys_Exit entry.\r\n" );
	return TRUE;
}

BOOL WINAPI Dbg_OutString( LPTSTR lpszString )
{
	OutputDebugString( lpszString );
	return TRUE;
}


BOOL WINAPI Heap_Free( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
	return HeapFree(hHeap,  dwFlags,  lpMem);
}

LPVOID WINAPI Heap_Alloc( HANDLE hHeap, DWORD dwFlags, ULONG dwBytes )
{
	return HeapAlloc(hHeap, dwFlags, dwBytes);
}

LPVOID WINAPI Heap_ReAlloc(
  HANDLE hHeap,   // handle to heap block
  DWORD dwFlags,  // heap reallocation options
  LPVOID lpMem,   // pointer to memory to reallocate
  SIZE_T dwBytes  // number of bytes to reallocate
)
{
	return HeapReAlloc( hHeap, dwFlags, lpMem, dwBytes );
}


HANDLE WINAPI Process_GetHeap( VOID )
{
	return GetProcessHeap();
}

BOOL WINAPI Sys_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
{
	return FALSE;
}

HANDLE WINAPI Mutex_Create(
  LPSECURITY_ATTRIBUTES lpMutexAttributes,  // SD
  BOOL bInitialOwner,                       // initial owner
  LPCTSTR lpName                            // object name
)
{
	return CreateMutex( lpMutexAttributes, bInitialOwner, lpName );
}

BOOL WINAPI Mutex_Release(
  HANDLE hMutex   // handle to mutex
)
{
	return ReleaseMutex( hMutex );
}

// 
static LANGID g_LCID = 0x0409; // 一个系统全局 ID
int WINAPI NLS_GetLocaleInfo(
  LCID Locale,      // locale identifier
  LCTYPE LCType,    // information type
  LPTSTR lpLCData,  // information buffer
  int cchData       // size of buffer
)
{
	int valid = FALSE;
	if( LCType == LOCALE_IDEFAULTANSICODEPAGE )
	{
		if( cchData >= 6 )
		{	// 返回代码页 codepage
			if( Locale == LOCALE_SYSTEM_DEFAULT ||
				Locale == LOCALE_USER_DEFAULT )
				Locale = g_LCID;

			switch( Locale )
			{	
			case 0x0804:
				strcpy( lpLCData, "936" );
				valid = TRUE;
				break;
			case 0x0409:
				strcpy( lpLCData, "1252" );
				valid = TRUE;
				break;
			}
		}
	}
	if( valid )
		return strlen( lpLCData );
	return 0;
}

LANGID WINAPI NLS_GetSystemDefaultLangID( VOID )
{
	return g_LCID;
}

LANGID WINAPI NLS_SetSystemDefaultLangID( LANGID idNew )
{
	LANGID old = g_LCID;
	g_LCID = idNew;
	//
	// 应该进行系统广播 ...
	//
	return old;
}

WINBASEAPI
BOOL
WINAPI
SwitchToThread(
    VOID
    );

BOOL WINAPI Thread_SwitchTo(VOID)
{
	SwitchToThread();
	return TRUE;
}
