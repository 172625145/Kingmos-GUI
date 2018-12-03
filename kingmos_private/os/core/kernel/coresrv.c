#include <eframe.h>
#include <eucore.h>
#include <eapisrv.h>
#include <eobjcall.h>
#include <coresrv.h>
 
static const PFNVOID lpCoreAPI[] = {
	NULL,
//  intr api
    (PFNVOID)KL_IntrInit,
	(PFNVOID)KL_IntrDone,
	(PFNVOID)KL_IntrDisable,
	(PFNVOID)KL_IntrEnable,
// interlock
    (PFNVOID)KL_InterlockedIncrement,
    (PFNVOID)KL_InterlockedDecrement,
    (PFNVOID)KL_InterlockedExchange,
    (PFNVOID)KL_InterlockedExchangeAdd,
    (PFNVOID)KL_InterlockedCompareExchange,
// debug
    (PFNVOID)KL_DebugOutString,
// page
	(PFNVOID)KL_AllocPageMem,
	(PFNVOID)KL_FreePageMem,
// semphore...
    (PFNVOID)KL_CreateSemaphore,
    (PFNVOID)KL_ReleaseSemaphore,
    (PFNVOID)KL_CreateMutex,
    (PFNVOID)KL_ReleaseMutex,
    (PFNVOID)KL_CreateEvent,
    (PFNVOID)KL_ResetEvent,
    (PFNVOID)KL_SetEvent,
	(PFNVOID)KL_PulseEvent,
    (PFNVOID)KL_InitializeCriticalSection,
    (PFNVOID)KL_DeleteCriticalSection,
    (PFNVOID)KL_LeaveCriticalSection,
    (PFNVOID)KL_EnterCriticalSection,
	(PFNVOID)NULL,//KL_InitializeCriticalSectionAndSpinCount
	(PFNVOID)NULL,//KL_SetCriticalSectionSpinCount
	(PFNVOID)KL_TryEnterCriticalSection,//KL_TryEnterCriticalSection
	
	(PFNVOID)NULL,//TIMERQUEUE_CREATE,
    (PFNVOID)NULL,//TIMERQUEUE_DELETE,
	(PFNVOID)NULL,//TIMERQUEUE_CREATETIMER,
	(PFNVOID)NULL,//TIMERQUEUE_DELETETIMER,
	(PFNVOID)NULL,//TIMERQUEUE_CHANGETIMER,
	(PFNVOID)NULL,//TIMERQUEUE_DELETEEX,
// misc
    (PFNVOID)KL_WaitForSingleObject,
	(PFNVOID)KL_WaitForMultipleObjects,
	(PFNVOID)KL_AllocAPIHandle,
	(PFNVOID)KL_CloseHandle,
	(PFNVOID)KL_Sleep,
	(PFNVOID)KL_GetTickCount,
	(PFNVOID)KL_GetSystemTime,
	(PFNVOID)KL_SetSystemTime,
	(PFNVOID)KL_ExitSystem,
	(PFNVOID)KL_GetSystemInfo,
	(PFNVOID)KL_GetVersion,
	(PFNVOID)KL_ImplementCallBack,
	(PFNVOID)KL_ImplementCallBack4,
	(PFNVOID)KL_GetSysTOC,
// heap
    (PFNVOID)KL_HeapCreate,
	(PFNVOID)KL_HeapDestroy,
	(PFNVOID)KL_HeapAlloc,
	(PFNVOID)KL_HeapFree,
	(PFNVOID)KL_HeapReAlloc,
	(PFNVOID)KL_HeapValidate,
	NULL,                //reserver for HeapLock
	NULL,                //reserver for HeapUnlock
	NULL,                //reserver for HeapCompact
	NULL,                //reserver for HeapSize
	NULL,                //reserver for HeapWalk
// thread
    (PFNVOID)KL_CreateThread,
    (PFNVOID)KL_SetThreadPriority,
	(PFNVOID)KL_GetThreadPriority,

	(PFNVOID)KL_SetThreadPolicy,
	(PFNVOID)KL_GetThreadPolicy,

    (PFNVOID)KL_GetCurrentThread,
    (PFNVOID)KL_GetCurrentThreadId,
    (PFNVOID)KL_GetExitCodeThread,
    (PFNVOID)KL_ExitThread,
    (PFNVOID)KL_SwitchToThread,
	(PFNVOID)KL_TerminateThread,
	(PFNVOID)KL_LockSchedule,
	(PFNVOID)KL_UnlockSchedule,
    (PFNVOID)KL_SuspendThread,
    (PFNVOID)KL_ResumeThread,

	(PFNVOID)KL_CaptureException,
	(PFNVOID)KL_ReleaseException,
	(PFNVOID)KL_GetComputerNameEx,
	(PFNVOID)KL_SetComputerNameEx,

	(PFNVOID)KL_RegistryIO,
	(PFNVOID)KL_ChangeThreadRotate,//
	NULL,//reserver
	NULL,//reserver for KernelIoControl
//
#ifdef INLINE_PROGRAM	
	(PFNVOID)KL_RegisterApplication,
    (PFNVOID)KL_LoadApplication,
    (PFNVOID)KL_GetApplicationIcon,
#else
	(PFNVOID)NULL,
    (PFNVOID)NULL,
    (PFNVOID)NULL,
#endif
//
	(PFNVOID)KL_CreateProcess,
	(PFNVOID)KL_TerminateProcess,
	(PFNVOID)KL_GetCurrentProcessId,
	(PFNVOID)KL_GetProcessHeap,
	(PFNVOID)KL_MapPtrToProcess,
	(PFNVOID)KL_UnMapPtr,
    (PFNVOID)KL_GetCurrentProcess,
    (PFNVOID)KL_GetCallerProcess,
    (PFNVOID)KL_GetOwnerProcess,
	(PFNVOID)KL_GetExitCodeProcess,
// tls
    (PFNVOID)KL_TlsAlloc,
    (PFNVOID)KL_TlsFree,
    (PFNVOID)KL_TlsGetValue,
    (PFNVOID)KL_TlsSetValue,
	(PFNVOID)KL_TlsGetThreadValue,
	(PFNVOID)KL_TlsSetThreadValue,
// error
    (PFNVOID)KL_SetLastError,
	(PFNVOID)KL_GetLastError,
// module
    (PFNVOID)KL_LoadLibraryEx,
	(PFNVOID)KL_FreeLibrary,
	(PFNVOID)KL_GetProcAddress,
	(PFNVOID)KL_GetModuleFileName,
	(PFNVOID)KL_GetModuleHandle,
	(PFNVOID)KL_FreeLibraryAndExitThread,
	(PFNVOID)KL_DisableThreadLibraryCalls,
	(PFNVOID)KL_GetModuleFileHandle,
	(PFNVOID)KL_GetModuleSectionOffset,
// elf dll open
    (PFNVOID)KL_OpenDllModule,
	(PFNVOID)KL_OpenDllDependence,
// virtual mem
#ifdef VIRTUAL_MEM
    (PFNVOID)KL_AllocPhysMem,
	(PFNVOID)KL_FreePhysMem,
	(PFNVOID)KL_VirtualAlloc,
	(PFNVOID)KL_VirtualFree,
	(PFNVOID)KL_VirtualCopy,
	(PFNVOID)KL_VirtualProtect,
#else
    (PFNVOID)NULL,
	(PFNVOID)NULL,
	(PFNVOID)NULL,
	(PFNVOID)NULL,
	(PFNVOID)NULL,
	(PFNVOID)NULL,
#endif
//
	(PFNVOID)KL_GlobalMemoryStatus,//MEM_GLOBALSTATUS
// power manager
    (PFNVOID)KL_PowerOffSystem,
// profil
    (PFNVOID)KL_ProfileKernel,
//
	(PFNVOID)KL_GetCommandLine,
	(PFNVOID)KL_GetProcessUserData,
	(PFNVOID)KL_SetProcessUserData,
	(PFNVOID)KL_OpenProcess,
	(PFNVOID)KL_ReleaseModuleFileHandle,
	(PFNVOID)KL_SetPermissions,
	(PFNVOID)KL_GetPermissions,
	(PFNVOID)KL_SetAlarmInfo,
	(PFNVOID)KL_RefreshAlarm,
	(PFNVOID)KL_GetProcessId

/*
// registry
    (PFNVOID)KL_RegCloseKey,
    (PFNVOID)KL_RegCreateKeyEx,
    (PFNVOID)KL_RegDeleteKey,
    (PFNVOID)KL_RegDeleteValue,
    (PFNVOID)KL_RegEnumKeyEx,
    (PFNVOID)KL_RegFlushKey,
    (PFNVOID)KL_RegEnumValue,
    (PFNVOID)KL_RegOpenKeyEx,
    (PFNVOID)KL_RegQueryInfoKey,
    (PFNVOID)KL_RegQueryValueEx,
    (PFNVOID)KL_RegSetValueEx
*/
};

static const DWORD dwCoreArgs[] = {
	0,	
//  INTR api
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//KL_IntrInit
	ARG1_MAKE( DWORD ),//KL_IntrDone,
	ARG1_MAKE( DWORD ),//KL_IntrDisable
	ARG1_MAKE( DWORD ),//KL_IntrEnable
// interlock
    ARG1_MAKE( PTR ),//InterlockedIncrement,
    ARG1_MAKE( PTR ),//InterlockedDecrement,
    ARG2_MAKE( PTR, DWORD ),//InterlockedExchange,
    ARG2_MAKE( PTR, DWORD ),//InterlockedExchangeAdd,
    ARG3_MAKE( PTR, DWORD, DWORD ),//InterlockedCompareExchange,
// debug
    ARG1_MAKE( PTR ),//DebugOutString,
//  
    ARG3_MAKE( DWORD, PTR, DWORD ), // AllocPageMem
	ARG2_MAKE( PTR, DWORD ),  // FreePageMem
// semphore...
    ARG4_MAKE( PTR, DWORD, DWORD, PTR ),//CreateSemaphore,
    ARG3_MAKE( DWORD, DWORD, PTR ),//ReleaseSemaphore,
    ARG2_MAKE( PTR, DWORD ),//CreateMutex,
    ARG1_MAKE( DWORD ),//ReleaseMutex,
    ARG4_MAKE( PTR, DWORD, DWORD, PTR ),//CreateEvent,
    ARG1_MAKE( DWORD ),//ResetEvent,
    ARG1_MAKE( DWORD ),//SetEvent,
	ARG1_MAKE( DWORD ),//PulseEvent,
    ARG1_MAKE( PTR ),//InitializeCriticalSection,
    ARG1_MAKE( PTR ),//DeleteCriticalSection,
    ARG1_MAKE( PTR ),//LeaveCriticalSection,
    ARG1_MAKE( PTR ),//EnterCriticalSection,
	0,//ARG1_MAKE( PTR ),//InitializeCriticalSectionAndSpinCount
	0,//ARG0_MAKE(),	 //SetCriticalSectionSpinCount
	ARG1_MAKE( PTR ),    //TryEnterCriticalSection
	0,
	0,
	0,
	0,
	0,
	0,
// misc
    ARG2_MAKE( DWORD, DWORD ),//WaitForSingleObject,
	ARG4_MAKE( DWORD, PTR, DWORD, DWORD ),//WaitForMultipleObjects,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//AllocAPIHandle,
	ARG1_MAKE( DWORD ),//CloseHandle,
	ARG1_MAKE( DWORD ),//Sleep,
	ARG0_MAKE(),//GetTickCount,
	ARG1_MAKE( PTR ),//GetSystemTime,
	ARG1_MAKE( PTR ),//SetSystemTime,
	ARG2_MAKE( DWORD, DWORD ),//KL_ExitSystem
	ARG1_MAKE( PTR ),//KL_GetSystemInfo
	ARG1_MAKE( PTR ),//KL_GetVersion
	ARG12_MAKE( PTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD ),// ImplementCallBack
	ARG4_MAKE( PTR, DWORD, DWORD, DWORD ),  // ImplementCallBack4
	ARG0_MAKE(), // KL_GetSysTOC
// heap
    ARG3_MAKE( DWORD, DWORD, DWORD ),//HeapCreate
	ARG1_MAKE( DWORD ),//HeapDestroy,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//HeapAlloc,
	ARG3_MAKE( DWORD, DWORD, PTR ),//HeapFree,
	ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//HeapReAlloc,
	ARG3_MAKE( DWORD, DWORD, PTR ),//HeapValidate,
	0,                //reserver for HeapLock
	0,                //reserver for HeapUnlock
	0,                //reserver for HeapCompact
	0,                //reserver for HeapSize
	0,                //reserver for HeapWalk
// thread
    ARG6_MAKE( PTR, DWORD, PTR, PTR, DWORD, PTR ),//CreateThread,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//SetThreadPriority,
	ARG1_MAKE( DWORD ),//GetThreadPriority

	ARG2_MAKE( DWORD, DWORD ),//SetThreadPolicy,
	ARG1_MAKE( DWORD ),//
    ARG0_MAKE(),//GetCurrentThread,
    ARG0_MAKE(),//GetCurrentThreadId,
    ARG2_MAKE( DWORD, PTR ),//GetExitCodeThread,
    ARG1_MAKE( DWORD ),//ExitThread,
    ARG0_MAKE(),//SwitchToThread,
	ARG2_MAKE( DWORD, DWORD ),//TerminateThread
	ARG0_MAKE(), //LockSchedule
	ARG0_MAKE(), // UnlockSchedule
    ARG1_MAKE( DWORD ),// KL_SuspendThread,
    ARG1_MAKE( DWORD ),// KL_ResumeThread,
	ARG0_MAKE(),//CaptureException
	ARG0_MAKE(),//ReleaseException
	ARG3_MAKE( DWORD, PTR, PTR ),//GetComputerNameEx
	ARG2_MAKE( DWORD, PTR),//SetComputerNameEx

	ARG3_MAKE( PTR, DWORD, DWORD ),//reserver
	ARG2_MAKE( DWORD, DWORD ),//reserver
	NULL,//reserver
	NULL,//reserver
//
#ifdef INLINE_PROGRAM	
	ARG3_MAKE( PTR, PTR, DWORD ),//RegisterApplication,
    ARG2_MAKE( PTR, PTR ),//LoadApplication,
    ARG2_MAKE( PTR, DWORD ),//GetApplicationIcon,
#else
	0,
	0,
	0,
#endif
//
	ARG10_MAKE( PTR, PTR, PTR, PTR, DWORD, DWORD, PTR, PTR, PTR, PTR ),//CreateProcess,
	ARG2_MAKE( DWORD, DWORD ),//TerminateProcess,
	ARG0_MAKE(),//GetCurrentProcessId,
	ARG0_MAKE(),//GetProcessHeap,
	ARG2_MAKE( DWORD, DWORD ),//MapPtrToProcess,
	ARG1_MAKE( DWORD ),//UnMapPtr
    ARG0_MAKE(),//GetCurrentProcess,
    ARG0_MAKE(),//GetCallerProcess,
    ARG0_MAKE(),//GetOwnerProcess,
	ARG2_MAKE( DWORD, PTR ),//GetExitCodeProcess,
// tls
    ARG0_MAKE(),//TlsAlloc,
    ARG1_MAKE( DWORD ),//TlsFree,
    ARG1_MAKE( DWORD ),//TlsGetValue,
    ARG2_MAKE( DWORD, DWORD ),//TlsSetValue,
	ARG2_MAKE( DWORD, DWORD ),//TlsGetThreadValue,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//TlsSetThreadValue,
// error
    ARG1_MAKE( DWORD ),//SetLastError,
	ARG0_MAKE(),//GetLastError,
// module
    ARG3_MAKE( PTR, DWORD, DWORD ),//LoadLibraryEx,
	ARG1_MAKE( DWORD ),//FreeLibrary,
	ARG2_MAKE( DWORD, PTR ),//GetProcAddress,
	ARG3_MAKE( DWORD, PTR, DWORD ),//GetModuleFileName,
	ARG1_MAKE( PTR ),//GetModuleHandle,
	ARG2_MAKE( DWORD, DWORD ),//FreeLibraryAndExitThread,
	ARG1_MAKE( DWORD ),//DisableThreadLibraryCalls,
	ARG1_MAKE( DWORD ),//GetModuleFileHandle,
	ARG2_MAKE( DWORD, PTR ),//GetModuleSectionOffset,
// elf dll open
    ARG3_MAKE( PTR, DWORD, PTR),//OpenDllModule,
	ARG2_MAKE( PTR, DWORD ),//OpenDllDependence,
// mem
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, PTR ),//AllocPhysMem,
	ARG1_MAKE( DWORD ),//FreePhysMem,
	ARG4_MAKE( PTR, DWORD, DWORD, DWORD ),//VirtualAlloc,
	ARG3_MAKE( PTR, DWORD, DWORD ),//VirtualFree
	ARG4_MAKE( PTR, PTR, DWORD, DWORD ),//VirtualCopy
	ARG4_MAKE( PTR, DWORD, DWORD, PTR ),//VirtualProtect,
	ARG1_MAKE( PTR ),// KL_GlobalMemoryStatus
// power manager
    ARG0_MAKE( ),//PowerOffSystem,    
// profile
	ARG2_MAKE( DWORD, PTR ),//Profile
//
    ARG0_MAKE(), // GetCommandLine
	ARG0_MAKE(),
	ARG1_MAKE( DWORD ),
	ARG3_MAKE( DWORD, DWORD, DWORD ),
	ARG2_MAKE( DWORD, DWORD ),//KL_ReleaseModuleFileHandle
	ARG1_MAKE( DWORD ),//KL_SetPermissions
	ARG0_MAKE(),//KL_GetPermissions
	ARG1_MAKE( PTR ), //(PFNVOID)KL_SetAlarmInfo,
	ARG0_MAKE(),//(PFNVOID)KL_RefreshAlarm
	ARG1_MAKE( DWORD )

/*
// registry
    ARG1_MAKE( DWORD ),//RegCloseKey,
    ARG9_MAKE( DWORD, PTR, DWORD, PTR, DWORD, DWORD, PTR, PTR, PTR ),//RegCreateKeyEx,
    ARG2_MAKE( DWORD, PTR ),//RegDeleteKey,
    ARG2_MAKE( DWORD, PTR ),//RegDeleteValue,
    ARG8_MAKE( DWORD, DWORD, PTR, PTR, PTR, PTR, PTR, PTR ),//RegEnumKeyEx,
    ARG1_MAKE( DWORD ),//RegFlushKey,
    ARG8_MAKE( DWORD, DWORD, PTR, PTR, PTR, PTR, PTR, PTR ),//RegEnumValue,
    ARG5_MAKE( DWORD, PTR, DWORD, DWORD, PTR ),
    ARG12_MAKE( DWORD, PTR, PTR, PTR, PTR, PTR, PTR, PTR, PTR, PTR, PTR, PTR ),//RegQueryInfoKey,
    ARG6_MAKE( DWORD, PTR, PTR, PTR, PTR, PTR ),//RegQueryValueEx,
    ARG6_MAKE( DWORD, PTR, DWORD, DWORD, PTR, DWORD ),//RegSetValueEx
*/
};

/**************************************************
声明：BOOL InitialCoreServer( void )
参数：
	无
返回值：
	假如成功，返回TRUE；否则，返回FALSE
功能描述：
	初始化内核服务API
引用:
	被 system.c调用
************************************************/

BOOL InitialCoreServer( void )
{
	//API_Register( API_KERNEL,  (PFNVOID)lpCoreAPI, sizeof( lpCoreAPI ) / sizeof(PFNVOID) );
	KL_RegisterAPIEx( API_KERNEL,  lpCoreAPI, dwCoreArgs, sizeof( lpCoreAPI ) / sizeof(PFNVOID) );
	KL_SetAPIReady( API_KERNEL );
	
	return TRUE;
}





