/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ECORE_H
#define __ECORE_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define TIME_ZONE_ID_UNKNOWN  0
#define TIME_ZONE_ID_STANDARD 1
#define TIME_ZONE_ID_DAYLIGHT 2

typedef struct _FILETIME 
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, FAR * LPFILETIME;

typedef struct _SYSTEMTIME 
{
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, * PSYSTEMTIME, FAR * LPSYSTEMTIME;

#define CALL_KERNEL   0x80000000
#define CALL_ALLOC    0x40000000

typedef unsigned int ACCESS_KEY;   //aky
// 假如改变改结构，必须同时改变 KL_ImplementCallBack, KL_ImplementCallBack4
typedef struct _CALLSTACK
{
	struct _CALLSTACK * lpNext;
	LPVOID lpvData;
	DWORD dwCallInfo;
	DWORD dwStatus; 
	PFNVOID pfnRetAdress;
	ACCESS_KEY akyAccessKey;
}CALLSTACK, FAR * LPCALLSTACK;


#define CompareFileTime Time_CompareFile
LONG WINAPI Time_CompareFile( const FILETIME * lpft1, const FILETIME * lpft2 );

#define FileTimeToSystemTime Time_FileToSystem
BOOL WINAPI Time_FileToSystem( const FILETIME *lpft, LPSYSTEMTIME lpst );

#define FileTimeToLocalFileTime Time_FileToLocalFile
BOOL WINAPI Time_FileToLocalFile( const FILETIME *lpft, LPFILETIME lpftLocal );

#define GetLocalTime  Time_GetLocal
VOID WINAPI Time_GetLocal( LPSYSTEMTIME lpst );

#define LocalFileTimeToFileTime Time_LocalFileToFile
BOOL WINAPI Time_LocalFileToFile( const FILETIME *lpftLocal, LPFILETIME lpft );

#define SystemTimeToFileTime Time_SystemToFile
BOOL WINAPI Time_SystemToFile( const SYSTEMTIME *lpst, LPFILETIME lpft );

#define SetLocalTime Time_SetLocal
BOOL WINAPI Time_SetLocal( const SYSTEMTIME *lpst );

#define SetSystemTime Sys_SetSystemTime
BOOL WINAPI Sys_SetSystemTime( const SYSTEMTIME *lpst );

#define GetSystemTime Sys_GetSystemTime
VOID WINAPI Sys_GetSystemTime( LPSYSTEMTIME lpst );

#define EXS_POWEROFF   1
#define EXS_REBOOT     2
#define EXS_SHUTDOWN   3

#define ExitSystem Sys_Exit
BOOL WINAPI Sys_Exit( UINT uFlags, DWORD dwReserved );


// processor architecture
#define PROCESSOR_ARCHITECTURE_INTEL 0
#define PROCESSOR_ARCHITECTURE_MIPS  1
#define PROCESSOR_ARCHITECTURE_ALPHA 2
#define PROCESSOR_ARCHITECTURE_PPC   3
#define PROCESSOR_ARCHITECTURE_SHX   4
#define PROCESSOR_ARCHITECTURE_ARM   5
#define PROCESSOR_ARCHITECTURE_IA64  6
#define PROCESSOR_ARCHITECTURE_ALPHA64 7
#define PROCESSOR_ARCHITECTURE_UNKNOWN 0xFFFF
// processor type
#define PROCESSOR_INTEL_386     386
#define PROCESSOR_INTEL_486     486
#define PROCESSOR_INTEL_PENTIUM 586
#define PROCESSOR_MIPS_R4000    4000    // incl R4101 & R3910
#define PROCESSOR_ALPHA_21064   21064
#define PROCESSOR_PPC_601       601
#define PROCESSOR_PPC_603       603
#define PROCESSOR_PPC_604       604
#define PROCESSOR_PPC_620       620
#define PROCESSOR_HITACHI_SH3   10003   //
#define PROCESSOR_HITACHI_SH3E  10004   //
#define PROCESSOR_HITACHI_SH4   10005   //
#define PROCESSOR_MOTOROLA_821  821     //
#define PROCESSOR_SHx_SH3       103     //
#define PROCESSOR_SHx_SH4       104     //
#define PROCESSOR_STRONGARM     2577    //
#define PROCESSOR_ARM720        1824    //
#define PROCESSOR_ARM820        2080    //
#define PROCESSOR_ARM920        2336    //
#define PROCESSOR_ARM_7TDMI     70001   //

typedef struct _SYSTEM_INFO
{
    union {
        DWORD dwOemId;
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        }arch;
    }sysID;
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
}SYSTEM_INFO, FAR * LPSYSTEM_INFO;

#define GetSystemInfo Sys_GetInfo
VOID WINAPI Sys_GetInfo( LPSYSTEM_INFO lpsi );

typedef struct _OSVERSIONINFO
{ 
  DWORD dwOSVersionInfoSize; 
  DWORD dwMajorVersion; 
  DWORD dwMinorVersion; 
  DWORD dwBuildNumber; 
  DWORD dwPlatformId; 
  TCHAR szCSDVersion[ 128 ]; 
}OSVERSIONINFO, FAR * LPOSVERSIONINFO; 

#define GetVersionEx Sys_GetVersion
BOOL WINAPI Sys_GetVersion( LPOSVERSIONINFO lpvi );

#define GetSysTOC  Sys_GetTOC
LPCVOID WINAPI Sys_GetTOC( void );

typedef struct _TIME_ZONE_INFORMATION 
{
    LONG lBias;
    TCHAR szStandardName[32];
    SYSTEMTIME stStandardDate;
    LONG lStandardBias;
    TCHAR szDaylightName[32];
    SYSTEMTIME stDaylightDate;
    LONG lDaylightBias;
}TIME_ZONE_INFORMATION, * PTIME_ZONE_INFORMATION, FAR * LPTIME_ZONE_INFORMATION;

#define GetTimeZoneInformation Time_GetZoneInformation
DWORD WINAPI Time_GetZoneInformation ( LPTIME_ZONE_INFORMATION lpTimeZoneInformation );

#define SetDaylightTime Time_SetDaylight
void WINAPI Time_SetDaylight( DWORD dst );

#define SetTimeZoneInformation Time_SetZoneInformation
BOOL WINAPI Time_SetZoneInformation ( const TIME_ZONE_INFORMATION *lpTimeZoneInformation );

#define SetLastError Error_SetLast
void WINAPI Error_SetLast( DWORD dwErrorCode );

#define GetLastError Error_GetLast
DWORD WINAPI Error_GetLast( void );

//是一个共享内存
#define PM_SHARE 0x00000001
LPVOID WINAPI Page_AllocMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag );
BOOL WINAPI Page_FreeMem( LPVOID lp, DWORD dwSize );

typedef struct _OVERLAPPED 
{
    DWORD   Internal;
    DWORD   InternalHigh;
    DWORD   Offset;
    DWORD   OffsetHigh;
    HANDLE  hEvent;
} OVERLAPPED, * POVERLAPPED, FAR * LPOVERLAPPED;

typedef struct _SECURITY_ATTRIBUTES 
{
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, * PSECURITY_ATTRIBUTES, FAR * LPSECURITY_ATTRIBUTES;

#define INIT_SECURITY_ATTRIBUTES \
{ sizeof(SECURITY_ATTRIBUTES), NULL, FALSE }

#define CloseHandle Sys_CloseHandle
BOOL WINAPI Sys_CloseHandle( HANDLE handle );

HANDLE WINAPI Sys_AllocAPIHandle( UINT uiAPIId, LPVOID lpvData, UINT objType );

#define GetTickCount Sys_GetTickCount
DWORD WINAPI Sys_GetTickCount( void );

#define Sleep Sys_Sleep
VOID WINAPI Sys_Sleep( DWORD dwMilliseconds );

#define INFINITE 0xffffffffl
#define WAIT_FAILED (DWORD)0xFFFFFFFF
#define WAIT_OBJECT_0       0
#define WAIT_ABANDONED      0x80
#define WAIT_TIMEOUT        0x102
#define STILL_ACTIVE        0x103

#define WaitForSingleObject Sys_WaitForSingleObject
DWORD WINAPI Sys_WaitForSingleObject( HANDLE handle, DWORD dwMilliseconds );

#define WaitForMultipleObjects Sys_WaitForMultipleObjects
DWORD WINAPI Sys_WaitForMultipleObjects( DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwMilliseconds );


#define InterlockedIncrement  Interlock_Increment
LONG WINAPI Interlock_Increment( LPLONG lpDest );

#define InterlockedDecrement  Interlock_Decrement
LONG WINAPI Interlock_Decrement( LPLONG lpDest );

#define InterlockedExchange   Interlock_Exchange
LONG WINAPI Interlock_Exchange( LPLONG lpDest, LONG lNewValue );

#define InterlockedExchangeAdd    Interlock_ExchangeAdd
LONG WINAPI Interlock_ExchangeAdd( LPLONG lpDest, LONG lIncrement );

#define InterlockedCompareExchange Interlock_CompareExchange
LONG WINAPI Interlock_CompareExchange( LPLONG lpDest, LONG lExchange, LONG lComperand );


#define InterlockedTestExchange Interlock_TestExchange
#define Interlock_TestExchange( lpDest, lOldValue, lNewValue )  Interlock_CompareExchange( (lpDest), (lNewValue), (lOldValue) )

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
    LPVOID lpThreadParameter
    );
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef struct _EXECDOMAIN {
	LPCSTR *lpName;
	struct _EXECDOMAIN *lpNext;
}EXECDOMAIN;

#ifdef LINUX_KERNEL
	#include <pthread.h>
	typedef struct _CRITICAL_SECTION
	{
		pthread_mutex_t mutex;
	#ifdef __DEBUG
		LPTSTR lpcsName;
		char * lpFileName;
		int iFileLine;
		DWORD dwTickCount;
	#endif

	} CRITICAL_SECTION, *LPCRITICAL_SECTION;
#elif defined WINDOWS_KERNEL

	typedef struct _CRITICAL_SECTION
	{
		DWORD dump[32];
	#ifdef __DEBUG
		LPTSTR lpcsName;
		char * lpFileName;
		int iFileLine;
		DWORD dwTickCount;
	#endif
	} CRITICAL_SECTION, *LPCRITICAL_SECTION;

#elif defined KINGMOS_KERNEL
	typedef struct _CRITICAL_SECTION
	{
		LONG  iLockCount;
		DWORD dwOwnerThreadId;
		HANDLE hSemaphore;
		UINT   uiFlag;  //

	#ifdef __DEBUG
		LPTSTR lpcsName;
		char * lpFileName;
		int iFileLine;
		DWORD dwTickCount;
	#endif
	} CRITICAL_SECTION, *LPCRITICAL_SECTION;
#else
	#error "You must select kernel type"
#endif


// 0xffff 被用户使用
#define CREATE_SUSPENDED            0x00000001


#define CreateThread Thread_Create
HANDLE WINAPI Thread_Create(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    DWORD dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
   );

#define THREAD_BASE_PRIORITY_LOWRT  15  
#define THREAD_BASE_PRIORITY_MAX    2   
#define THREAD_BASE_PRIORITY_MIN    -2  
#define THREAD_BASE_PRIORITY_IDLE   -15 

#define THREAD_PRIORITY_LOWEST          THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL    (THREAD_PRIORITY_LOWEST+1)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_HIGHEST         THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL    (THREAD_PRIORITY_HIGHEST-1)

#define THREAD_PRIORITY_TIME_CRITICAL   THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE            THREAD_BASE_PRIORITY_IDLE
#define THREAD_PRIORITY_ERROR_RETURN    (MAXLONG)

#define SetThreadPriority Thread_SetPriority
BOOL WINAPI Thread_SetPriority( HANDLE hThread, long lPriority );

#define GetThreadPriority Thread_GetPriority
int WINAPI GetThreadPriority( HANDLE hThread );

#define THREAD_POLICY_OTHER		0
#define THREAD_POLICY_FIFO		1
#define THREAD_POLICY_ROTATION    2

#define THREAD_POLICY_ERROR       (-1)

#define SetThreadPolicy Thread_SetPolicy
BOOL WINAPI Thread_SetPolicy( HANDLE hThread, UINT uiPolicy );
#define GetThreadPolicy Thread_GetPolicy
int WINAPI Thread_GetPolicy( HANDLE hThread );

#define GetCurrentThread Thread_GetCurrent
HANDLE WINAPI Thread_GetCurrent( void );

#define GetCurrentThreadId Thread_GetCurrentId
DWORD WINAPI Thread_GetCurrentId( void );

#define GetExitCodeThread Thread_GetExitCode
BOOL WINAPI Thread_GetExitCode( HANDLE hThread, LPDWORD lpExitCode );

#define ExitThread Thread_Exit
void WINAPI Thread_Exit( DWORD dwExitCode ); 

#define SwitchToThread Thread_SwitchTo
BOOL WINAPI Thread_SwitchTo( VOID );

#define TerminateThread Thread_Terminate
BOOL WINAPI Thread_Terminate( HANDLE hThread, DWORD dwExitCode );

#define MAXIMUM_SUSPEND_COUNT 255
#define SuspendThread Thread_Suspend
DWORD WINAPI Thread_Suspend( HANDLE hThread );

#define ResumeThread Thread_Resume
DWORD WINAPI Thread_Resume( HANDLE hThread );

#define TLS_OUT_OF_INDEXES (-1)
#define TlsAlloc Tls_Alloc
DWORD WINAPI Tls_Alloc(VOID);

#define TlsFree  Tls_Free
BOOL WINAPI Tls_Free( DWORD dwTlsIndex );

#define TlsGetValue Tls_GetValue
LPVOID WINAPI Tls_GetValue( DWORD dwTlsIndex );

#define TlsSetValue Tls_SetValue
BOOL WINAPI Tls_SetValue( DWORD dwTlsIndex, LPVOID lpValue );

#define TlsGetThreadValue Tls_GetThreadValue
LPVOID WINAPI Tls_GetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex );

#define TlsSetThreadValue Tls_SetThreadValue
BOOL WINAPI Tls_SetThreadValue( DWORD dwThreadId, DWORD dwTlsIndex, LPVOID lpvData );

#define CreateSemaphore Semaphore_Create
HANDLE WINAPI Semaphore_Create( LPSECURITY_ATTRIBUTES lpAttr, int nInitialCount, int nMaximumCount, LPCTSTR lpcszName );

#define ReleaseSemaphore Semaphore_Release
BOOL WINAPI Semaphore_Release( HANDLE hSemaphore, int lReleaseCount, LPINT lpPreviousCount );

#define CreateMutex Mutex_Create
HANDLE WINAPI Mutex_Create( LPSECURITY_ATTRIBUTES lpAttr, BOOL bInitialOwner, LPCTSTR lpcszName );

#define ReleaseMutex Mutex_Release
BOOL WINAPI Mutex_Release( HANDLE hMutex );

#define CreateEvent  Event_Create
HANDLE WINAPI Event_Create(
      LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
  	  BOOL bManualReset, 
	  BOOL bInitialState, 
	  LPCTSTR lpName ) ;

#define ResetEvent Event_Reset
BOOL WINAPI Event_Reset( HANDLE hEvent );

#define SetEvent Event_Set
BOOL WINAPI Event_Set( HANDLE hEvent );

#define PulseEvent Event_Pulse
BOOL WINAPI Event_Pulse( HANDLE hEvent );

#define InitializeCriticalSection CriticalSection_Initialize
VOID WINAPI CriticalSection_Initialize( LPCRITICAL_SECTION lpCriticalSection );

#define DeleteCriticalSection CriticalSection_Delete
VOID WINAPI CriticalSection_Delete( LPCRITICAL_SECTION lpCriticalSection );

#define LeaveCriticalSection CriticalSection_Leave
VOID WINAPI CriticalSection_Leave( LPCRITICAL_SECTION lpCriticalSection );

//#ifdef __DEBUG
//    VOID Debug_CriticalSection_Enter( LPCRITICAL_SECTION lpCriticalSection, char * lpFile, int iFileLine );
  //  #define EnterCriticalSection( lpcs ) Debug_CriticalSection_Enter( (lpcs), __FILE__, __LINE__ )
//#else
//    #define EnterCriticalSection CriticalSection_Enter
//#endif
#define EnterCriticalSection CriticalSection_Enter
VOID WINAPI CriticalSection_Enter( LPCRITICAL_SECTION lpCriticalSection );

#define TryEnterCriticalSection CriticalSection_TryEnter
BOOL WINAPI CriticalSection_TryEnter( LPCRITICAL_SECTION lpCriticalSection );

typedef struct _PROCESS_INFORMATION 
{ 
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
}PROCESS_INFORMATION, FAR * LPPROCESS_INFORMATION;

typedef struct _STARTUPINFO 
{ 
    DWORD   cb; 
    LPTSTR  lpReserved; 
    LPTSTR  lpDesktop; 
    LPTSTR  lpTitle; 
    DWORD   dwX; 
    DWORD   dwY; 
    DWORD   dwXSize; 
    DWORD   dwYSize; 
    DWORD   dwXCountChars; 
    DWORD   dwYCountChars; 
    DWORD   dwFillAttribute; 
    DWORD   dwFlags; 
    WORD    wShowWindow; 
    WORD    cbReserved2; 
    LPBYTE  lpReserved2; 
    HANDLE  hStdInput; 
    HANDLE  hStdOutput; 
    HANDLE  hStdError; 
} STARTUPINFO, *LPSTARTUPINFO; 

#define DLL_PROCESS_ATTACH 1    
#define DLL_THREAD_ATTACH  2    
#define DLL_THREAD_DETACH  3    
#define DLL_PROCESS_DETACH 0    

#define CreateProcess Process_Create
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
						   );

#define ExitProcess Process_Exit
VOID WINAPI Process_Exit(
						 UINT uExitCode   // exit code for all threads
						 );

#define TerminateProcess Process_Terminate
VOID WINAPI Process_Terminate(
						  HANDLE hProcess,
						  UINT uExitCode   // exit code for all threads
						 );

#define GetCurrentProcessId Process_GetCurrentId
DWORD WINAPI Process_GetCurrentId(void);

#define GetProcessId Process_GetId
DWORD WINAPI Process_GetId( HANDLE hProcess );

//定义通常的存取mask
#define SYNCHRONIZE                      (0x00100000L)
#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

//定义进程存取权限
#define PROCESS_ALL_ACCESS     ( STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFF)

#define OpenProcess Process_Open
HANDLE WINAPI Process_Open( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId );

#define GetProcessHeap Process_GetHeap
HANDLE WINAPI Process_GetHeap( void );

#define MapPtrToProcess Process_MapPtr
LPVOID WINAPI Process_MapPtr( LPVOID lpv, HANDLE hProcess );

#define UnMapPtr Process_UnMapPtr
LPVOID WINAPI Process_UnMapPtr( LPVOID lpv );

#define GetCurrentProcess Process_GetCurrent
HANDLE WINAPI Process_GetCurrent( void );

#define GetCallerProcess Process_GetCaller
HANDLE WINAPI Process_GetCaller( void );

#define GetOwnerProcess Process_GetOwner
HANDLE WINAPI Process_GetOwner( void );

#define GetProcAddress  Module_GetProcAddress
FARPROC WINAPI Module_GetProcAddress( HMODULE hModule, LPCTSTR lpProcName );

#define GetModuleFileName Module_GetFileName
DWORD WINAPI Module_GetFileName( HMODULE hModule, LPTSTR lpFilename, DWORD nSize );

#define LoadLibrary( lpfn ) Module_LoadLibraryEx( (lpfn), NULL, 0 )
#define LoadLibraryEx Module_LoadLibraryEx

HMODULE WINAPI Module_LoadLibraryEx( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags );

#define FreeLibrary Module_FreeLibrary
BOOL WINAPI Module_FreeLibrary( HMODULE hModule );

#define GetProcAddress Module_GetProcAddress
FARPROC WINAPI Module_GetProcAddress( HMODULE hModule, LPCTSTR lpProcName );

#define GetModuleFileName Module_GetFileName
DWORD WINAPI Module_GetFileName( HMODULE hModule, LPTSTR lpFilename, DWORD nSize );

#define GetModuleHandle Module_GetHandle
HMODULE WINAPI Module_GetHandle( LPCTSTR lpModuleName );

#define FreeLibraryAndExitThread Module_FreeLibraryAndExitThread 
VOID WINAPI Module_FreeLibraryAndExitThread( HMODULE hModule, DWORD dwExitCode );

#define DisableThreadLibraryCalls Module_DisableThreadLibraryCalls
BOOL WINAPI Module_DisableThreadLibraryCalls( HMODULE hModule );

#define OpenDllModule Dll_OpenModule
LPVOID WINAPI Dll_OpenModule( LPCTSTR lpcszName, UINT uiMode, LPVOID lpvCaller );

#define OpenDllDependence Dll_OpenDependence
int WINAPI Dll_OpenDependence( LPVOID lpvModule, UINT uiMode );

#define PAGE_NOACCESS          0x01
#define PAGE_READONLY          0x02
#define PAGE_READWRITE         0x04
//#define PAGE_WRITECOPY         0x08
#define PAGE_EXECUTE           0x10
#define PAGE_EXECUTE_READ      0x20
#define PAGE_EXECUTE_READWRITE 0x40
//#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD            0x100
#define PAGE_NOCACHE          0x200
//#define PAGE_WRITECOMBINE     0x400
#define PAGE_PHYSICAL         0x400

#define MEM_COMMIT           0x1000
#define MEM_RESERVE          0x2000
#define MEM_DECOMMIT         0x4000
#define MEM_RELEASE          0x8000
#define MEM_PHYSICAL         0x0100 
#define MEM_CONTIGUOUS       0x0200 

//#define MEM_FREE            0x10000
//#define MEM_PRIVATE         0x20000
//#define MEM_MAPPED          0x40000
//#define MEM_RESET           0x80000
#define MEM_AUTO_COMMIT      0x80000000

#define AllocPhysMem Mem_AllocPhysical
LPVOID WINAPI Mem_AllocPhysical( DWORD cbSize, DWORD fdwProtect, DWORD dwAlignmentMask, DWORD dwFlags, ULONG * pPhysicalAddress );

#define FreePhysMem Mem_FreePhysical 
BOOL WINAPI Mem_FreePhysical( LPVOID lpvAdr );

#define VirtualAlloc Mem_VirtualAlloc
LPVOID WINAPI Mem_VirtualAlloc( LPVOID lpAddress, DWORD dwSize, DWORD dwAllocationType, DWORD dwProtect );

#define VirtualFree Mem_VirtualFree
BOOL WINAPI Mem_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType ); 

#define VirtualCopy Mem_VirtualCopy
BOOL WINAPI Mem_VirtualCopy( LPVOID lpDestAdr, LPVOID lpSrcAdr, DWORD dwSize, DWORD dwProtect );

#define VirtualProtect Mem_VirtualProtect
BOOL WINAPI Mem_VirtualProtect( LPVOID lpvAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect );

typedef struct _MEMORYSTATUS
{
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORD dwTotalPhys;
    DWORD dwAvailPhys;
    DWORD dwTotalPageFile;
    DWORD dwAvailPageFile;
    DWORD dwTotalVirtual;
    DWORD dwAvailVirtual;
} MEMORYSTATUS, FAR *LPMEMORYSTATUS;

#undef GlobalMemoryStatus
#define GlobalMemoryStatus Mem_GlobalStatus
void WINAPI Mem_GlobalStatus( LPMEMORYSTATUS lpms );

#define ProfileKernel Sys_ProfileKernel
void WINAPI Sys_ProfileKernel( UINT uiOption, LPVOID lpv );

#define GetCommandLine Sys_GetCommandLine
LPTSTR WINAPI Sys_GetCommandLine( VOID );

// heap manger
#define HeapCreate Heap_Create 
HANDLE WINAPI Heap_Create( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize );

#define HeapDestroy Heap_Destroy
BOOL WINAPI Heap_Destroy( HANDLE hHeap );

//#define HEAP_NO_SERIALIZE   1
//#define HEAP_ZERO_MEMORY    2 
#define HEAP_NO_SERIALIZE               0x00000001      
#define HEAP_ZERO_MEMORY                0x00000008      


#define HeapAlloc Heap_Alloc
LPVOID WINAPI Heap_Alloc( HANDLE hHeap, DWORD dwFlags, ULONG dwBytes );

#define HeapFree Heap_Free
BOOL WINAPI Heap_Free( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );

#define HeapReAlloc Heap_ReAlloc
LPVOID WINAPI Heap_ReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, ULONG dwBytes );

#define HeapValidate Heap_Validate
BOOL WINAPI Heap_Validate( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem );

// debug
#define DbgOutString Dbg_OutString
BOOL WINAPI Dbg_OutString( LPTSTR lpszString );

// power
#define PowerOffSystem Power_SystemOff
void WINAPI Power_SystemOff( void );

#define GetProcessUserData Process_GetUserData
LPVOID WINAPI Process_GetUserData( VOID );

#define SetProcessUserData Process_SetUserData
LPVOID WINAPI Process_SetUserData( LPVOID lpvData );

UINT WINAPI Thread_LockSchedule( void );
UINT WINAPI Thread_UnlockSchedule( void );


// 返回错误代码
#define EXCEPTION_NO_ERROR          0
#define EXCEPTION_ACCESS_VIOLATION  1
#define EXCEPTION_BREAKPOINT	    2 

UINT WINAPI Sys_CaptureException( void );
BOOL WINAPI Sys_ReleaseException( void );
//使用异常处理函数的方法：
//	if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
//	{
//		//正常处理流程...
//		//......
//		//释放异常处理
//		Sys_ReleaseException();
//	}
//	else
//	{	//出现异常,处理异常
//		//...
//	}
//


typedef enum _COMPUTER_NAME_FORMAT {
  ComputerNameNetBIOS,
  ComputerNameDnsHostname,
  ComputerNameDnsDomain,
  ComputerNameDnsFullyQualified,
  ComputerNamePhysicalNetBIOS,
  ComputerNamePhysicalDnsHostname,
  ComputerNamePhysicalDnsDomain,
  ComputerNamePhysicalDnsFullyQualified,
  ComputerNameMax
} COMPUTER_NAME_FORMAT ;

#define MAX_COMPUTERNAME_LENGTH 31
#define GetComputerNameEx Sys_GetComputerNameEx
BOOL WINAPI Sys_GetComputerNameEx(
						   COMPUTER_NAME_FORMAT NameType,  // name type
						   LPTSTR lpBuffer,                // name buffer
						   LPDWORD lpnSize                 // size of name buffer
						   );

#define SetComputerNameEx Sys_SetComputerNameEx
BOOL WINAPI Sys_SetComputerNameEx(
								  COMPUTER_NAME_FORMAT NameType,  // name type 
								  LPCTSTR lpBuffer                // new name buffer 
								  );


//	输入输出注册表
//定义 Sys_RegistryIO的 dwFlag - 以下类型的组合:
#define REGIO_READ		0x00000001		   //读注册表
#define REGIO_WRITE		0x00000002		   //写注册表

#define REGIO_BEGIN		0x00000004         //第一次IO操作 并且 开始IO操作 
#define REGIO_END		0x00000008	       //最后一次IO操作 并且 结束IO操作 

#define REGIO_CLEAR     0x00000010			//清除所有的数据

BOOL WINAPI Sys_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag );

//改变线程的时间片消耗配额数( (min)=1 ~ (max)32 )
// NewRoate = Current Rotate + nAddRotate
UINT WINAPI Thread_ChangeRotate( DWORD dwThreadId, int nAddRotate );
//解锁对该模块的访问
BOOL WINAPI Module_ReleaseFileHandle( HANDLE hModule, HANDLE hFile );

#define SetProcPermissions Thread_SetPermissions
DWORD WINAPI Thread_SetPermissions( DWORD dwNewPerm );
#define GetCurrentPermissions  Thread_GetPermissions
DWORD WINAPI Thread_GetPermissions( VOID );

//闹铃功能支持
typedef struct _ALARM_INFO
{	//ai
	UINT uiSize; // must = sizeof( ALARM_INFO )
	HANDLE hAlarmEvent;
	SYSTEMTIME stAlarmTime;
}ALARM_INFO, FAR * LPALARM_INFO;
// lpai 可以等于 NULL, 以为着清楚当前的设置
BOOL WINAPI Sys_SetAlarmInfo( LPALARM_INFO lpai );
// dwFlag , reserve , must = 0
void WINAPI Sys_RefreshAlarm( DWORD dwFlag );
//默认得分服务处理
DWORD WINAPI Sys_DefServerProc( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam );



//  存取类型

#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define WRITE_DAC                        (0x00040000L)
#define WRITE_OWNER                      (0x00080000L)
#define SYNCHRONIZE                      (0x00100000L)

#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)

#define STANDARD_RIGHTS_ALL              (0x001F0000L)


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__ECORE_H

