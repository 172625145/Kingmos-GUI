//#include <windows.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#include <time.h>
//#include <edef.h>
#include <eframe.h>
#include "handlepackage.h"
#include "semaphore.h"
#include <enls.h>
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
	//printf( "Sys_Sleep entry.\r\n" );
#ifdef LINUX_KERNEL	
	usleep(dwMilliseconds * 1000);
#else
	Sleep( dwMilliseconds );
#endif	
	//printf( "Sys_Sleep leave.\r\n" );
}

DWORD WINAPI Sys_GetTickCount( void )
{
#ifdef LINUX_KERNEL
	struct timeval tv;

	//printf( "Sys_GetTickCount entry.\r\n" );
	gettimeofday(&tv, NULL);
	return (tv.tv_sec*1000 + tv.tv_usec/1000);
#else
	return GetTickCount();
#endif
}

VOID WINAPI Sys_GetSystemTime( LPSYSTEMTIME lpst )
{
#ifdef LINUX_KERNEL
	time_t ltime;
	struct tm *lpmt;

	//printf( "Sys_GetSystemTime entry.\r\n" );

	time(&ltime);
	lpmt = gmtime( &ltime );
	lpst->wMilliseconds = 0;
	lpst->wSecond       = lpmt->tm_sec; 
	lpst->wMinute       = lpmt->tm_min;
	lpst->wHour         = lpmt->tm_hour;
	lpst->wDay          = lpmt->tm_mday;
	lpst->wMonth        = lpmt->tm_mon;
	lpst->wYear         = lpmt->tm_year;
	lpst->wDayOfWeek    = lpmt->tm_wday;
	//lpmt->tm_yday;	  //days since January 1 [0..365]	
	//lpmt->tm_isdst;   //daylight saving time flag
	//printf( "Sys_GetSystemTime leave.\r\n" );

#else
	GetSystemTime( lpst );
#endif
}

BOOL WINAPI Sys_SetSystemTime( const SYSTEMTIME *lpst )
{
#ifdef LINUX_KERNEL
	struct timeval tv;
	time_t ltime;
	struct tm tm1;
//	struct timezone tz;  //linux default not support , need define BSD

//	printf( "Sys_SetSystemTime entry.\r\n" );

	tm1.tm_sec     = lpst->wSecond;
	tm1.tm_min     = lpst->wMinute; 
	tm1.tm_hour    = lpst->wHour;
	tm1.tm_mday    = lpst->wDay;
	tm1.tm_mon     = lpst->wMonth;
	tm1.tm_year    = lpst->wYear;
	tm1.tm_wday    = lpst->wDayOfWeek; 
	//tm1.tm_yday
	//tm1.tm_isdst
	ltime = mktime(&tm1);
	tv.tv_sec  = ltime; 
//	tv.tv_usec = 0; 
//	tz.tz_minuteswest = 0;
//	tz.tz_dsttime = 0;
//	settimeofday(&tv, &tz);
	//printf( "Sys_SetSystemTime leave.\r\n" );
#else
	return SetSystemTime( lpst );
#endif
}

#define MAX_THREAD_INDEX 32
#define MAX_THREAD_NUM   1024
#define THREAD_ID_MASK   (1023)

typedef struct _THREAD_PRI_DATA
{
	DWORD dwThreadData[MAX_THREAD_INDEX];
	DWORD dwErrorCode;
}THREAD_PRI_DATA;


static THREAD_PRI_DATA threadData[MAX_THREAD_NUM];

//DWORD dwErrorCode = 0;
DWORD WINAPI Error_GetLast( void )
{
	//printf( "Error_GetLast entry.\r\n" );
	//return GetLastError();
	return threadData[pthread_self()&THREAD_ID_MASK].dwErrorCode;
	//return dwErrorCode;
}

void WINAPI Error_SetLast( DWORD dwErrorCode )
{
	//printf( "Error_SetLast entry.\r\n" );
	//SetLastError( dwErrorCode );
	//dwErrorCode = dwErrorCode;
	//printf( "Error_SetLast leave.\r\n" );
	threadData[pthread_self()&THREAD_ID_MASK].dwErrorCode = dwErrorCode;
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
	//return CreateThread( lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId );
	LPTRHANDLEPACKAGE lpPackage;
	DWORD dwSize;
	pthread_attr_t threadAttr;
	pthread_t *lpPtheadId; 

//	printf( "Thread_Create entry.\r\n" );

	lpPackage = (LPTRHANDLEPACKAGE)malloc( sizeof(HANDLEPACKAGE) );
	lpPtheadId = (pthread_t *)malloc( sizeof(pthread_t) );
	
	if((lpPackage==NULL) || (lpPtheadId==NULL)){
		if(lpPackage){
			free(lpPackage);
		}
		if(lpPtheadId){
			free(lpPtheadId);
		}
		printf( "Thread_Create leave0.\r\n" );
		return (HANDLE)NULL;
	}

	lpPackage->nHandleType = THREAD_HANDLE;


	if(dwStackSize == 0){
		dwStackSize = 4 * 1024;
	}
	// initialize the thread attribute
	//pthread_attr_init(&threadAttr);

	// Set the stack size of the thread
	//pthread_attr_setstacksize(&threadAttr, dwStackSize);

	// Set thread to detached state. No need for pthread_join
	//pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

	// Create the threads
	//lpPackage->lpv = (HANDLE)pthread_create(lpPtheadId, &threadAttr, lpStartAddress, lpParameter);
	pthread_create(lpPtheadId, NULL, lpStartAddress, lpParameter);
	lpPackage->lpv = (HANDLE)lpPtheadId;

	if( lpdwThreadId )
		lpdwThreadId = (DWORD*)lpPtheadId;

	// Destroy the thread attributes
	//pthread_attr_destroy(&threadAttr);

//	printf( "Thread_Create leave1.\r\n" );

	return (HANDLE)lpPackage;
}

BOOL WINAPI Sys_CloseHandle( HANDLE handle )
{
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)handle; 
	BOOL bRetv = TRUE;

//	printf( "Sys_CloseHandle entry.\r\n" );

	if(handle == NULL){
		bRetv = FALSE;
		goto _return;
	}

	if(lptr->nHandleType == FILE_HANDLE){
		if(fclose((FILE*)(lptr->lpv)) == (int)0){
			lptr->lpv = NULL;
			free((void*)lptr);
			goto _return;
		}else{
			bRetv = FALSE;
			goto _return;
		}
	}else if(lptr->nHandleType == SEMAPHORE_HANDLE){
		sem_destroy( (sem_t*)(lptr->lpv) );

		free( (void*)(lptr->lpv) );
		lptr->lpv = NULL; 	

		free((void*)lptr);
		lptr = NULL;

		goto _return;
	}else if(lptr->nHandleType == EVENTNONAME_HANDLE){
		sem_destroy((sem_t*)(lptr->lpv));

		free((void*)(lptr->lpv));
		lptr->lpv = NULL; 	

		free((void*)lptr);
		lptr = NULL;

		goto _return;
	}else if(lptr->nHandleType == THREAD_HANDLE){
		//pthread_kill((pthread_t)(lptr->lpv);
		free((void*)(lptr->lpv));
		free((void*)lptr);
		lptr = NULL;
		goto _return;
	}else if(lptr->nHandleType == PROCESS_HANDLE){
		kill(lptr->lpv, 0);
		free(lptr);
		lptr = NULL;
		goto _return;
	}else{
		CloseHandle( lptr->lpv );
		free((void *)lptr);
		lptr = NULL;
		goto _return;
	}
_return:

//	printf( "Sys_CloseHandle leave.\r\n" );
	
	return bRetv;
}

HANDLE WINAPI Semaphore_Create( LPSECURITY_ATTRIBUTES lpAttr, int nInitialCount, int nMaximumCount, LPCTSTR lpcszName )
{
	int	   nRetCode;	
	LPTRHANDLEPACKAGE lpPackage;
	sem_t  *lpSem;  

//	printf( "Semaphore_Create entry.\r\n" );
			
	lpPackage = malloc( sizeof(HANDLEPACKAGE) );
	lpSem = (sem_t *)malloc(sizeof(sem_t));
	if(	(!lpPackage) || (!lpSem) ){
		if(lpPackage){
			free((void*)lpPackage);
		}
		if(lpSem){
			free((void*)lpSem);
		}
//		printf( "Semaphore_Create leave0.\r\n" );
		return (HANDLE)0;
	}

	nRetCode = sem_init(lpSem, 0, nInitialCount);
	if(nRetCode != 0){
		free((void *)lpPackage);
		free((void *)lpSem);
//		printf( "Semaphore_Create leave1.\r\n" );
		return (HANDLE)0;
	}

	lpPackage->nHandleType = SEMAPHORE_HANDLE;
	lpPackage->lpv = (HANDLE)lpSem;
//	printf( "Semaphore_Create leave2.\r\n" );

	return (HANDLE)lpPackage;

	//return CreateSemaphore( NULL, nInitialCount, nMaximumCount, lpcszName );
}

BOOL WINAPI Semaphore_Release( HANDLE hSemaphore, int lReleaseCount, LPINT lpPreviousCount )
{
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)hSemaphore; 

//	printf( "Semaphore_Release entry.\r\n" );

	if(lptr->nHandleType == SEMAPHORE_HANDLE){
		sem_post((sem_t *)(lptr->lpv));
//		printf( "Semaphore_Release leave0.\r\n" );
		return TRUE;
	}

//	printf( "Semaphore_Release leave1.\r\n" );
	return FALSE;
	//return ReleaseSemaphore( hSemaphore, lReleaseCount, lpPreviousCount );
}

HANDLE WINAPI Event_Create(
      LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
  	  BOOL bManualReset, 
	  BOOL bInitialState, 
	  LPCTSTR lpName ) 
{	
	int	   nRetCode;	
	LPTRHANDLEPACKAGE lpPackage = NULL;
	sem_t  *lpSem;  

//	printf( "Event_Create entry.\r\n" );
	
	lpPackage = malloc( sizeof(HANDLEPACKAGE) );
	lpSem = (sem_t *)malloc(sizeof(sem_t));
	if(	(!lpPackage) || (!lpSem) ){
		if(lpPackage){
			free((void *)lpPackage);
		}
		if(lpSem){
			free((void *)lpSem);
		}
		goto _return;
	}

	nRetCode = sem_init(lpSem, 0, 0);
	if(nRetCode != 0){
		free((void *)lpPackage);
		free((void *)lpSem);
		goto _return;
	}

	lpPackage->nHandleType = EVENTNONAME_HANDLE;
	lpPackage->lpv = (HANDLE)lpSem;
	
_return:
//	printf( "Event_Create leave(0x%x).\r\n", lpPackage );

	return (HANDLE)lpPackage;
	//return CreateEvent( NULL, bManualReset, bInitialState, lpName );
}

BOOL WINAPI Event_Set( HANDLE hEvent )
{
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)hEvent; 

//	printf( "Event_Set entry.\r\n" );

	if(lptr->nHandleType == EVENTNONAME_HANDLE){
		sem_post((sem_t *)(lptr->lpv));
//		printf( "Event_Set leave0.\r\n" );
		return TRUE;
	}
//	printf( "Event_Set leave1.\r\n" );

	return FALSE;

	//return SetEvent( hEvent );
}

BOOL WINAPI Event_Reset( HANDLE hEvent )
{
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)hEvent; 


	if(lptr->nHandleType == EVENTNONAME_HANDLE){
		return TRUE;
	}

	return FALSE;

	//return SetEvent( hEvent );
}

DWORD WINAPI Sys_WaitForSingleObject( HANDLE handle, DWORD dwMilliseconds )
{
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)handle; 
	struct timespec delay;
	DWORD timeout = 0;
	int retCode = 0;


//	printf( "Sys_WaitForSingleObject entry(handle=0x%x),dwMilliseconds=0x%x.\r\n", handle, dwMilliseconds );

	if( (lptr->nHandleType==SEMAPHORE_HANDLE) || (lptr->nHandleType==EVENTNONAME_HANDLE)){
//		printf("wait for corect enter .. \r\n");
		while ((DWORD)timeout < dwMilliseconds ) {   // maxim wait is 49 days
			delay.tv_sec = 0;
//			delay.tv_nsec = 1000000;  /* 1 milli sec */
			delay.tv_nsec = 20000000;  /* 1 milli sec */
			// Wait for the event be signaled
			retCode = sem_trywait((sem_t*)lptr->lpv); // event semaphore handle
			// non blocking call
			if (!retCode)  {
				/* Event is signaled */
				//break;
//				printf( "Sys_WaitForSingleObject((handle=0x%x)) leave0.\r\n", handle );

				return (DWORD)WAIT_OBJECT_0;
			}else{
//				retCode == errno;
				/* check whether somebody else has the mutex */
				if (errno == EAGAIN ) {/* sleep for delay time */
					nanosleep(&delay, NULL);
					timeout+=20 ;
				}else{
					/* error  */
					//printf( "Sys_WaitForSingleObject(handle=0x%x) leave1,retCode=%d.\r\n", handle, retCode );
					printf("errcode=0x%x, %d \r\n", retCode, errno);
					return (DWORD)WAIT_FAILED; 
				}
			}
		}

	//	printf( "Sys_WaitForSingleObject leave2.\r\n" );
		return (DWORD)WAIT_TIMEOUT;
	}
	else
	{
//		printf("wait for error enter .. \r\n");
	}
	

//	printf( "Sys_WaitForSingleObject leave3.\r\n" );
	return (DWORD)WAIT_FAILED; 

	//return WaitForSingleObject( handle, dwMilliseconds );
}

DWORD WINAPI Sys_WaitForMultipleObjects( DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwMilliseconds )
{
	//return WaitForMultipleObjects( nCount, lpHandles, fWaitAll, dwMilliseconds );
	printf( "no support Sys_WaitForMultipleObjects.\r\n" );
	return WAIT_FAILED;
}

DWORD WINAPI Thread_GetCurrentId( void )
{
	//printf( "Thread_GetCurrentId entry.\r\n" );

	return (DWORD)pthread_self();
	//return GetCurrentThreadId();
}

DWORD WINAPI Thread_SetPermissions( DWORD dwNewPerm )
{
	printf( "Thread_SetPermissions entry.\r\n" );

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

BOOL WINAPI Tls_SetValue( DWORD dwTlsIndex, LPVOID lpValue )
{
	//printf( "Tls_SetValue entry.\r\n" );
	if( dwTlsIndex < MAX_THREAD_INDEX )
	{
		//lpvThreadData[pthread_self()&THREAD_ID_MASK][dwTlsIndex] = lpValue;
		threadData[pthread_self()&THREAD_ID_MASK].dwThreadData[dwTlsIndex] = lpValue;
		//printf( "Tls_SetValue leave0.\r\n" );
		return TRUE;
	}
	printf( "Tls_SetValue leave1.\r\n" );
	return FALSE;
}

LPVOID WINAPI Tls_GetValue( DWORD dwTlsIndex )
{
	//printf( "Tls_GetValue entry.\r\n" );
	if( dwTlsIndex < MAX_THREAD_INDEX )
	{
		//printf( "Tls_GetValue leave0.\r\n" );
		//return lpvThreadData[pthread_self()&THREAD_ID_MASK][dwTlsIndex];
		return threadData[pthread_self()&THREAD_ID_MASK].dwThreadData[dwTlsIndex];
	}
	printf( "Tls_GetValue leave1.\r\n" );
	return NULL;
}

DWORD WINAPI Sys_DefServerProc( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
{
	printf( "Sys_DefServerProc entry.\r\n" );
	return 0;
}


BOOL WINAPI Sys_Exit( UINT uFlags, DWORD dwReserved )
{
	printf( "Sys_Exit entry.\r\n" );
	return TRUE;
}

BOOL WINAPI Dbg_OutString( LPTSTR lpszString )
{
	printf( lpszString );
	return TRUE;
}

/*
HMODULE WINAPI Module_GetHandle( LPCTSTR lpModuleName )
{
}


HANDLE WINAPI Module_GetSectionOffset( HMODULE hModule, LPCTSTR lpsz )
{
	return 0;
}

extern HANDLE hgwmeProcess;
HANDLE WINAPI Module_GetFileHandle( HMODULE hModule )
{	
	if( hModule == hgwmeProcess )
	{
		return (HANDLE)fopen( "gwme.res", "rb" );
	}
	else
		return INVALID_HANDLE_VALUE;
}

BOOL WINAPI Module_ReleaseFileHandle( HANDLE hModule, HANDLE hFile )
{
	if( hModule == hgwmeProcess )
	{
		fclose( (FILE*)hFile );
		return TRUE;
	}
	else
		return FALSE;
}
*/

/*
BOOL WINAPI Gbl_Unlock(  HANDLE hMem   // 全局内存句柄
)
{
}

LPVOID WINAPI Gbl_Lock(
  HANDLE hMem   // 全局内存句柄
)
{
}

HANDLE WINAPI Gbl_Alloc(
  UINT uFlags,     // 分配标志
  DWORD dwBytes   // 要分配的内存大小
)
{
}
*/


BOOL WINAPI Sys_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
{
	return FALSE;
}

#if 1
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

#endif