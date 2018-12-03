/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __CORESRV_H
#define __CORESRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#ifndef __EOBJCALL_H
#include <eobjcall.h>
#endif

#ifndef __ESETJMP_H
#include <esetjmp.h>
#endif

BOOL WINAPI KL_IntrInit( DWORD idInt, 
				  HANDLE hIntrEvent,
				  LPVOID lpvData, 
				  DWORD dwSize );
void WINAPI KL_IntrDone( DWORD idInt );
void WINAPI KL_IntrDisable( DWORD idInt );
void WINAPI KL_IntrEnable( DWORD idInt, LPVOID lpvData, DWORD dwSize );

#undef GetSystemTime
#define GetSystemTime KL_GetSystemTime
VOID WINAPI KL_GetSystemTime( LPSYSTEMTIME lpst );

#undef SetSystemTime
#define SetSystemTime KL_SetSystemTime
BOOL WINAPI KL_SetSystemTime( const SYSTEMTIME *lpst );

#undef ExitSystem
#define ExitSystem KL_ExitSystem
BOOL WINAPI KL_ExitSystem( UINT, DWORD );

#undef GetSystemInfo
#define GetSystemInfo KL_GetSystemInfo
VOID WINAPI KL_GetSystemInfo( LPSYSTEM_INFO lpsi );

#undef GetVersion
#define GetVersion KL_GetVersion
BOOL WINAPI KL_GetVersion( LPOSVERSIONINFO lpvi );

LRESULT KL_ImplementCallBack( LPCALLBACKDATA, ... );
LRESULT KL_ImplementCallBack4( LPCALLBACKDATA, ... );
LPCVOID WINAPI KL_GetSysTOC( void );
//DWORD KL_ImplementCallBack( LPVOID lpv, ... );
//DWORD KL_ImplementCallBack4( LPVOID lpv, ... );

#undef SetLastError
#define SetLastError KL_SetLastError
void WINAPI KL_SetLastError( DWORD dwErrorCode );

#undef TerminateThread
#define TerminateThread KL_TerminateThread
BOOL WINAPI KL_TerminateThread( HANDLE hThread, DWORD dwExitCode );


#undef GetLastError
#define GetLastError KL_GetLastError 
DWORD WINAPI KL_GetLastError( void );

#undef CloseHandle
#define CloseHandle KL_CloseHandle
BOOL WINAPI KL_CloseHandle( HANDLE handle );

#undef AllocAPIHandle
#define AllocAPIHandle KL_AllocAPIHandle
HANDLE WINAPI KL_AllocAPIHandle( UINT uiAPIId, LPVOID lpvData, UINT objType );

#undef GetTickCount
#define GetTickCount KL_GetTickCount
DWORD WINAPI KL_GetTickCount( void );

#undef Sleep
#define Sleep KL_Sleep
VOID WINAPI KL_Sleep( DWORD dwMilliseconds );

#undef WaitForSingleObject
#define WaitForSingleObject KL_WaitForSingleObject
DWORD WINAPI KL_WaitForSingleObject( HANDLE handle, DWORD dwMilliseconds );

#undef WaitForMultipleObjects
#define WaitForMultipleObjects KL_WaitForMultipleObjects
DWORD WINAPI KL_WaitForMultipleObjects( DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwMilliseconds );

#undef InterlockedIncrement
#define InterlockedIncrement KL_InterlockedIncrement
LONG WINAPI KL_InterlockedIncrement( LPLONG lpDest );

#undef InterlockedDecrement
#define InterlockedDecrement KL_InterlockedDecrement
LONG WINAPI KL_InterlockedDecrement( LPLONG lpDest );
 
#undef InterlockedExchange
#define InterlockedExchange KL_InterlockedExchange
LONG WINAPI KL_InterlockedExchange( LPLONG lpDest, LONG lNewValue );

//LONG WINAPI KL_InterlockedTestExchange( LPLONG lpDest, LONG lOldValue, LONG lNewValue );
#undef InterlockedExchangeAdd
#define InterlockedExchangeAdd KL_InterlockedExchangeAdd
LONG WINAPI KL_InterlockedExchangeAdd( LPLONG lpDest, LONG lIncrement );

#undef InterlockedCompareExchange
#define InterlockedCompareExchange KL_InterlockedCompareExchange
LONG WINAPI KL_InterlockedCompareExchange( LPLONG lpDest, LONG lExchange, LONG lComperand );

BOOL __TRY( void * , int len );

HANDLE WINAPI KL_CreateThread(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    DWORD dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
   );
BOOL WINAPI KL_SetThreadPriority( HANDLE hThread, long lPriority, UINT flag );
BOOL WINAPI KL_SetThreadPolicy( HANDLE hThread, UINT uiPolicy );
int WINAPI KL_GetThreadPriority( HANDLE hThread, UINT flag );
int WINAPI KL_GetThreadPolicy( HANDLE hThread );
HANDLE WINAPI KL_GetCurrentThread( void );
DWORD WINAPI KL_GetCurrentThreadId( void );
BOOL WINAPI KL_GetExitCodeThread( HANDLE hThread, LPDWORD lpExitCode );
void WINAPI KL_ExitThread( DWORD dwExitCode ); 
BOOL WINAPI KL_SwitchToThread( VOID );
DWORD WINAPI KL_SuspendThread( HANDLE hThread );
DWORD WINAPI KL_ResumeThread( HANDLE hThread );

DWORD WINAPI KL_TlsAlloc(VOID);
BOOL WINAPI KL_TlsFree( DWORD dwTlsIndex );
LPVOID WINAPI KL_TlsGetValue( DWORD dwTlsIndex );
BOOL WINAPI KL_TlsSetValue( DWORD dwTlsIndex, LPVOID lpValue );
LPVOID WINAPI KL_TlsGetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex );
BOOL WINAPI KL_TlsSetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex, LPVOID lpvData );

UINT WINAPI KL_LockSchedule( void );
UINT WINAPI KL_UnlockSchedule( void );

HANDLE WINAPI KL_CreateSemaphore( LPSECURITY_ATTRIBUTES lpAttr, int nInitialCount, int nMaximumCount, LPCTSTR lpcszName );
BOOL WINAPI KL_ReleaseSemaphore( HANDLE hSemaphore, int lReleaseCount, LPINT lpPreviousCount );
HANDLE WINAPI KL_CreateMutex( LPSECURITY_ATTRIBUTES lpAttr, BOOL bInitialOwner, LPCTSTR lpcszName );
BOOL WINAPI KL_ReleaseMutex( HANDLE hMutex );
HANDLE WINAPI KL_CreateEvent(
      LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
  	  BOOL bManualReset, 
	  BOOL bInitialState, 
	  LPCTSTR lpName ) ;
BOOL WINAPI KL_ResetEvent( HANDLE hEvent );
BOOL WINAPI KL_SetEvent( HANDLE hEvent );
BOOL WINAPI KL_PulseEvent( HANDLE hEvent );

#undef InitializeCriticalSection
#define InitializeCriticalSection KL_InitializeCriticalSection
VOID WINAPI KL_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection );

#undef DeleteCriticalSection
#define DeleteCriticalSection KL_DeleteCriticalSection
VOID WINAPI KL_DeleteCriticalSection( LPCRITICAL_SECTION lpCriticalSection );

#undef LeaveCriticalSection
#define LeaveCriticalSection KL_LeaveCriticalSection
VOID WINAPI KL_LeaveCriticalSection( LPCRITICAL_SECTION lpCriticalSection );

#undef EnterCriticalSection
#define EnterCriticalSection KL_EnterCriticalSection
VOID WINAPI KL_EnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection );

#undef TryEnterCriticalSection
#define TryEnterCriticalSection KL_TryEnterCriticalSection
BOOL WINAPI KL_TryEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection );


BOOL WINAPI KL_CreateProcess(
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
VOID WINAPI KL_ExitProcess(
						 UINT uExitCode   // exit code for all threads
						 );
VOID WINAPI KL_TerminateProcess(
						 HANDLE hProcess,
						 UINT uExitCode   // exit code for all threads
						 );

DWORD WINAPI KL_GetCurrentProcessId(void);
HANDLE WINAPI KL_GetProcessHeap( void );

#undef MapPtrToProcess
#define MapPtrToProcess KL_MapPtrToProcess
LPVOID WINAPI KL_MapPtrToProcess( LPVOID lpv, HANDLE hProcess );

#undef GetCommandLine
#define GetCommandLine KL_GetCommandLine
LPTSTR WINAPI KL_GetCommandLine(VOID);

LPVOID WINAPI KL_GetProcessUserData( VOID );
LPVOID WINAPI KL_SetProcessUserData( LPVOID lpvData );

LPVOID WINAPI KL_UnMapPtr( LPVOID lpv );
HANDLE WINAPI KL_GetCurrentProcess( void );
HANDLE WINAPI KL_GetCallerProcess( void );
HANDLE WINAPI KL_GetOwnerProcess( void );
BOOL WINAPI KL_GetExitCodeProcess( HANDLE hProcess, LPDWORD lpExitCode );

BOOL WINAPI KL_RegisterApplication( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon );
BOOL WINAPI KL_LoadApplication( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine );
HICON WINAPI KL_GetApplicationIcon( LPCTSTR lpcszName, UINT fIconType );

//HMODULE WINAPI Module_Load( LPCTSTR lpFileName, DWORD dwFlag );
HMODULE WINAPI KL_LoadLibraryEx( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags );
BOOL WINAPI KL_FreeLibrary( HMODULE hModule );
FARPROC WINAPI KL_GetProcAddress( HMODULE hModule, LPCTSTR lpProcName );
DWORD WINAPI KL_GetModuleFileName( HMODULE hModule, LPTSTR lpFilename, DWORD nSize );
HMODULE WINAPI KL_GetModuleHandle( LPCTSTR lpModuleName );
VOID WINAPI KL_FreeLibraryAndExitThread( HMODULE hModule, DWORD dwExitCode );
BOOL WINAPI KL_DisableThreadLibraryCalls( HMODULE hModule );
HANDLE WINAPI KL_GetModuleFileHandle( HMODULE hModule );
DWORD WINAPI KL_GetModuleSectionOffset( HMODULE hModule, LPCTSTR lpcszSectioName  );
BOOL WINAPI KL_ReleaseModuleFileHandle( HMODULE hModule, HANDLE hFile );

DWORD WINAPI KL_SetPermissions( DWORD dwNewPerm );
DWORD WINAPI KL_GetPermissions( VOID );

void WINAPI KL_RefreshAlarm( DWORD dwFlag );
BOOL WINAPI KL_SetAlarmInfo( LPALARM_INFO lpai );
// mem
LPVOID WINAPI KL_AllocPhysMem( DWORD dwSize, DWORD fdwProtect, DWORD dwAlignmentMask, DWORD dwFlags, ULONG * pPhysicalAddress );
BOOL WINAPI KL_FreePhysMem( LPVOID lpvAdr );

LPVOID DoAllocPageMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag );
BOOL DoFreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  );
//
LPVOID WINAPI KL_AllocPageMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag );
BOOL WINAPI KL_FreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  );
//

HANDLE WINAPI KL_RegisterAPI( UINT uiAPIId, const PFNVOID * lpfn, UINT uiOptions );
HANDLE WINAPI KL_RegisterAPIEx( UINT uiAPIId, const PFNVOID * lpfn, const DWORD * lpArgs, UINT uiOptions );

BOOL WINAPI KL_UnregisterAPI( HANDLE hAPI );
BOOL WINAPI KL_IsAPIReady( UINT uiAPI );
BOOL WINAPI KL_SetAPIReady( UINT uiAPI );
BOOL WINAPI KL_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle );
BOOL WINAPI KL_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs );
BOOL WINAPI KC_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle );
BOOL WINAPI KC_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs );

void WINAPI KL_LeaveAPI( void );

#undef VirtualAlloc
#define VirtualAlloc KL_VirtualAlloc
LPVOID WINAPI KL_VirtualAlloc( LPVOID lpAddress, DWORD dwSize, DWORD dwAllocationType, DWORD dwProtect );
LPVOID WINAPI KC_VirtualAlloc( LPVOID lpAddress, DWORD dwSize, DWORD dwAllocationType, DWORD dwProtect );

#undef VirtualAllocEx
#define VirtualAllocEx KL_VirtualAllocEx
LPVOID WINAPI KL_VirtualAllocEx( HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD dwAllocationType, DWORD dwProtect );


#undef VirtualFree
#define VirtualFree KL_VirtualFree
BOOL WINAPI KL_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType ); 
BOOL WINAPI KC_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType ); 

#undef VirtualFreeEx
#define VirtualFreeEx KL_VirtualFreeEx
BOOL WINAPI KL_VirtualFreeEx( HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType ); 

#undef VirtualCopy
#define VirtualCopy KL_VirtualCopy
BOOL WINAPI KL_VirtualCopy( LPVOID lpAddress, LPVOID lpSrcAdr, DWORD dwSize, DWORD dwProtect );

#undef VirtualProtect
#define VirtualProtect KL_VirtualProtect
BOOL WINAPI KL_VirtualProtect( LPVOID lpvAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect );

#undef GlobalMemoryStatus
#define GlobalMemoryStatus KL_GlobalMemoryStatus
void WINAPI KL_GlobalMemoryStatus( LPMEMORYSTATUS lpms );

#undef ProfileKernel
#define ProfileKernel KL_ProfileKernel
void WINAPI KL_ProfileKernel( UINT uiOption, VOID * lpv );


// heap
HANDLE WINAPI KL_HeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize );
BOOL WINAPI KL_HeapDestroy( HANDLE hHeap );
void * WINAPI KL_HeapAlloc( HANDLE hHeap, DWORD dwFlags, DWORD dwSizeNeeded );
BOOL WINAPI KL_HeapFree( HANDLE hHeap, DWORD dwFlags, void FAR * lpvUsed );
LPVOID WINAPI KL_HeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID old, DWORD sz );

BOOL WINAPI KL_HeapValidate( HANDLE hHeap, 
						   DWORD dwFlags,
						   LPCVOID lpMem );


// debug
BOOL WINAPI KL_DebugOutString( LPTSTR );

// powerm manganer
void WINAPI KL_PowerOffSystem( void );

// elf dll open 
LPVOID WINAPI KL_OpenDllModule( LPCTSTR lpcszName, UINT uiMode, LPVOID lpvCaller );
int WINAPI KL_OpenDllDependence( LPVOID lpvModule, UINT uiMode );

// gwme
#undef GetSystemMetrics
#define GetSystemMetrics KL_GetSysMetrics
int WINAPI KL_GetSysMetrics( int nIndex );

#undef LoadImage
#define LoadImage KL_LoadImage
HANDLE WINAPI KL_LoadImage( 
                     HINSTANCE hinst,
                     LPCTSTR lpszName, 
                     UINT uType, 
                     int cxDesired, 
                     int cyDesired, 
                     UINT fuLoad 
                     );
//

// file sys
#undef  CreateFile
#define CreateFile KL_CreateFile
HANDLE WINAPI KL_CreateFile( LPCTSTR lpcszFileName,
							   DWORD dwAccess, 
							   DWORD dwShareMode, 
							   PSECURITY_ATTRIBUTES pSecurityAttributes, 
							   DWORD dwCreate, 
							   DWORD dwFlagsAndAttributes, 
							   HANDLE hTemplateFile );
#undef  ReadFile 
#define ReadFile   KL_ReadFile
BOOL WINAPI KL_ReadFile( HANDLE hFile, 
					   LPVOID lpvBuffer, 
					   DWORD dwNumToRead, 
					   LPDWORD lpdwNumRead, 
					   LPOVERLAPPED lpOverlapped );

#undef  CloseFile
#define CloseFile KL_CloseFile
BOOL WINAPI KL_CloseFile( HANDLE hFile );

#undef WriteFile
#define WriteFile  KL_WriteFile
BOOL WINAPI KL_WriteFile( HANDLE hFile, 
					    LPCVOID lpvBuffer, 
						DWORD dwNumToWrite, 
						LPDWORD lpdwNumWrite, 
						LPOVERLAPPED pOverlapped );

#undef GetFileAttributes
#define GetFileAttributes KL_GetFileAttributes
DWORD WINAPI KL_GetFileAttributes( LPCTSTR lpcszFileName );

#undef DeviceIoControl 
#define DeviceIoControl KL_DeviceIoControl
BOOL WINAPI KL_DeviceIoControl( 
                          HANDLE hFile,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped );
#undef SetFilePointer
#define SetFilePointer KL_SetFilePointer
DWORD WINAPI KL_SetFilePointer( HANDLE hFile, 
							  LONG lDistanceToMove, 
							  LPLONG lpDistanceToMoveHigh, 
							  DWORD dwMoveMethod );

HANDLE WINAPI KL_OpenProcess( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId );

UINT WINAPI KL_CaptureException( LPEXCEPTION_CONTEXT jmp_data );
BOOL WINAPI KL_ReleaseException( void );

BOOL WINAPI KL_GetComputerNameEx(
								 COMPUTER_NAME_FORMAT NameType,  // name type
								 LPTSTR lpBuffer,                // name buffer
								 LPDWORD lpnSize                 // size of name buffer
								 );
BOOL WINAPI KL_SetComputerNameEx(
								  COMPUTER_NAME_FORMAT NameType,  // name type 
								  LPCTSTR lpBuffer                // new name buffer 
								  );
BOOL WINAPI KL_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag );
UINT WINAPI KL_ChangeThreadRotate( DWORD dwThreadId, int nAddRotate );

DWORD WINAPI KL_GetProcessId( HANDLE hProcess );
//
/*
// registry
#undef RegCloseKey
#define RegCloseKey KL_RegCloseKey
LONG WINAPI KL_RegCloseKey( HKEY hKey );

#undef RegCreateKeyEx
#define RegCreateKeyEx KL_RegCreateKeyEx
LONG WINAPI KL_RegCreateKeyEx( 
							HKEY hKey, 
							LPCTSTR lpSubKey, 
							DWORD Reserved, 
							LPTSTR lpClass, 
							DWORD dwOptions, 
							REGSAM samDesired, 
							LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
							PHKEY phkResult, 
							LPDWORD lpdwDisposition 
							);
#undef RegDeleteKey
#define RegDeleteKey KL_RegDeleteKey
LONG WINAPI KL_RegDeleteKey( HKEY hKey, LPCTSTR lpSubKey );

#undef RegDeleteValue
#define RegDeleteValue KL_RegDeleteValue
LONG WINAPI KL_RegDeleteValue( HKEY hKey, LPCTSTR lpValueName );

#undef RegEnumKeyEx
#define RegEnumKeyEx KL_RegEnumKeyEx
LONG WINAPI KL_RegEnumKeyEx(
						  HKEY hKey, 
						  DWORD dwIndex, 
						  LPTSTR lpName, 
						  LPDWORD lpcbName, 
						  LPDWORD lpReserved, 
						  LPTSTR lpClass, 
						  LPDWORD lpcbClass, 
						  PFILETIME lpftLastWriteTime 
						  );
#undef RegFlushKey
#define RegFlushKey KL_RegFlushKey
LONG WINAPI KL_RegFlushKey( HKEY hKey );

#undef RegEnumValue
#define RegEnumValue KL_RegEnumValue
LONG WINAPI KL_RegEnumValue( 
				   HKEY hKey, 
				   DWORD dwIndex, 
				   LPTSTR lpValueName, 
				   LPDWORD lpcbValueName, 
				   LPDWORD lpReserved, 
				   LPDWORD lpType, 
				   LPBYTE lpData, 
				   LPDWORD lpcbData 
				   );

#undef RegOpenKeyEx
#define RegOpenKeyEx KL_RegOpenKeyEx
LONG WINAPI KL_RegOpenKeyEx( 
				   HKEY hKey, 
				   LPCTSTR lpSubKey, 
				   DWORD ulOptions, 
				   REGSAM samDesired, 
				   PHKEY phkResult 
				   );

#undef RegQueryInfoKey
#define RegQueryInfoKey KL_RegQueryInfoKey
LONG WINAPI KL_RegQueryInfoKey( 
					  HKEY hKey, 
					  LPTSTR lpClass, 
					  LPDWORD lpcbClass, 
					  LPDWORD lpReserved, 
					  LPDWORD lpcSubKeys, 
					  LPDWORD lpcbMaxSubKeyLen, 
					  LPDWORD lpcbMaxClassLen, 
					  LPDWORD lpcValues, 
					  LPDWORD lpcbMaxValueNameLen, 
					  LPDWORD lpcbMaxValueLen, 
					  LPDWORD lpcbSecurityDescriptor, 
					  PFILETIME lpftLastWriteTime 
					  );

#undef RegQueryValueEx
#define RegQueryValueEx KL_RegQueryValueEx
LONG WINAPI KL_RegQueryValueEx( 
					  HKEY hKey, 
					  LPCTSTR lpValueName, 
					  LPDWORD lpReserved, 
					  LPDWORD lpType, 
					  LPBYTE lpData, 
					  LPDWORD lpcbData 
					  );

#undef RegSetValueEx
#define RegSetValueEx KL_RegSetValueEx
LONG WINAPI KL_RegSetValueEx( 
					HKEY hKey, 
					LPCTSTR lpValueName, 
					DWORD Reserved, 
					DWORD dwType, 
					const BYTE *lpData, 
					DWORD cbData 
					);
// 
*/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__CORESRV_H




