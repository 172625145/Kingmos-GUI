#ifndef __EPCORE_H
#define __EPCORE_H

//#ifdef CPU_ARM
//#include <drivers\pdd\arm\include\cpu.h>
//#endif

//#ifdef EML_WIN32
//#include <drivers\pdd\i386win\include\cpu.h>
//#endif
#include <cpu.h>

#ifndef __ECORE_H
#include <ecore.h>
#endif

#ifndef __ESETJMP_H
#include <esetjmp.h>
#endif


#ifndef __EOBJLIST_H
#include <eobjlist.h>
#endif

#ifndef __EPHEAP_H
#include <epheap.h>
#endif

#ifndef __VIRTUAL_H
#include <virtual.h>
#endif

#ifndef __EAPISRV_H
#include <eapisrv.h>
#endif

#define ALIGN_PAGE_UP( v ) ( ( (v) + PAGE_SIZE - 1 ) & ~(PAGE_SIZE-1) )
#define ALIGN_PAGE_DOWN( v ) ( (v) & ~(PAGE_SIZE-1) )
#define ALIGN_DWORD( v ) ( ( (v) + sizeof(DWORD) - 1 ) & ~( sizeof(DWORD) - 1 ) )

//#ifndef EML_WIN32

//#endif

#define SIGNAL_CLEAR           0x00000001		//告诉该线程去清除子线程的资源
//#define SIGNAL_KILLING          0x00000002
#define SIGNAL_EXIT            0x00000002		//告诉该线程去退出

//define thread flag
#define FLAG_KILLING           0x1
#define FLAG_PULSE_EVENTING    0x2

#define THREAD_RUNNING		    0
#define THREAD_INTERRUPTIBLE	1
#define THREAD_UNINTERRUPTIBLE	2
#define THREAD_ZOMBIE		    3
#define THREAD_SUSPENDED	    4
//#define THREAD_SWAPPING		    5

//0  - higest  255 - lowest
// 0 ~ 63  -> realtime pri
// (255-32)223  ~ 254      -> normal thread
// (0xff) 255    -> idle 

#define IDLE_PRIORITY   255

#define DEF_PRIORITY    (IDLE_PRIORITY - 15)	
#define DEF_BOOST   	(0)

#define NR_THREADS	512
#define RESERVE_THREADS 4

#define STACK_MAGIC  0x89abcdefl

//#define STP_OTHER		0
//#define STP_FIFO		1
//#define STP_ROTATION	2
#define MAX_ROTATE      32
#define MIN_ROTATE      1

#define DEF_ROTATE      16

extern BOOL SchedulerInit(void);
void CALLBACK Schedule(void);

// define task queue
typedef struct _TASK
{
	struct _TASK *lpNext;		/* linked list of active bh's */
	int iSync;			/* must be initialized to zero */
	void (*lpRoutine)(void *);	/* function to call */
	void *lpData;			/* argument to function */
}TASK, * LPTASK;

typedef TASK * LPTASKQUEUE;

//typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)( LPVOID lpThreadParameter );
//typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef struct _TIMER
{
	DWORD dwExpires;
	void (*lpFunction)(void);
}TIMER, * LPTIMER;

typedef struct _TIMERLIST {
	struct _TIMERLIST * lpNext;
	struct _TIMERLIST * lpPrev;
	DWORD dwExpires;
	DWORD dwData;
	void (*lpFunction)( DWORD dwData );
}TIMERLIST, * LPTIMERLIST;

#define INIT_TIMERLIST_STRUCT  { NULL, NULL, 0, 0, NULL }
//#define INIT_TIMERLIST_STRUCT  { 0 }

void _AddTimerList( LPTIMERLIST lpTask );
int _DelTimerList( LPTIMERLIST lpTask );
void ItRealFunction( DWORD dwData );
//void LockIRQSave( UINT * lpSave );
//void UnlockIRQRestore( UINT * lpRestore );

#define INIT_TIMER_LIST(  lpTask ) { (lpTask)->lpNext = (lpTask)->lpPrev = 0; }

struct _THREAD;

typedef struct _WAITQUEUE
{
	struct _THREAD FAR * lpThread;
	struct _WAITQUEUE * lpNext;
}WAITQUEUE, * LPWAITQUEUE;

#define WAIT_QUEUE_HEAD( lppQueue ) ((LPWAITQUEUE)( (lppQueue) - 1 ))

#define INIT_WAIT_QUEUE( lppQueue )	*(lppQueue) = WAIT_QUEUE_HEAD( (lppQueue) )

extern void SleepOn( LPWAITQUEUE * lpp );
extern void InterruptibleSleepOn( LPWAITQUEUE * lpp );
extern void WakeUp(LPWAITQUEUE * lpp );
extern void WakeUpInterruptible(LPWAITQUEUE * lpp);
extern void WakeUpThread( struct _THREAD FAR * lpThread );

// define SEMAPHORE

#define SF_MANUALRESET  0x8000
#define SF_OWNER        0x4000
#define SF_DELETING     0x2000
#define SF_MUTEX        0x0001
#define SF_SEMAPHORE    0x0002
#define SF_EVENT        0x0003

typedef struct _SEMAPHORE
{
    //HANDLE hThis;// objType;
	struct _SEMAPHORE * lpNext;
	short nCount;   // resource count used
	short nWaiting;
	LPWAITQUEUE lpWaitQueue;

    short nMaxCount;  // max resource count
	WORD  semFlag;  // 0x0000 01 : bManualReset, 
    int   nRefCount;
    
	//WORD nPulseCount;
	//WORD dump;//
	LPTSTR lpszName;
	struct _THREAD FAR * lpOwner;  // valid if is MUTEX
	short nLockCount;
	BYTE nBoost; // 每有一个线程等待该资源，该值加一，拥有该资源的线程的Bootup值将加上该量
    BYTE nHighPriority; // highest to wait thread
}SEMAPHORE, * LPSEMAPHORE;
/*
typedef struct _RESOURCE
{
	BYTE uObjType;  //下面定义
	BYTE nResCount;
	WORD dump;//
	LPVOID lpvResource;	
	struct _RESOURCE * lpNext;
}RESOURCE, * LPRESOURCE;

#define RES_CRITICAL_SECTION 1
#define RES_SEMAPHORE        2

//#define RES_MULTISEMAPHORE   3
*/

#define MUTEX { OBJ_SEMAPHORE, NULL, 1, 0, NULL, 1, 1, SF_MUTEX | SF_OWNER, NULL, NULL, 0 }
//#define MUTEX_LOCKED { OBJ_SEMAPHORE, NULL, 0, 0, NULL, 1, 1, SF_MUTEX, NULL, NULL }


//DWORD Down( LPSEMAPHORE lpsem, DWORD dwMilliseconds );
void UpSemaphore( LPSEMAPHORE lpsem, int iCount );
DWORD DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwMilliseconds, BOOL bWaitAll );

//struct __MSGQUEUE;


struct _THREAD;

#define MAX_WAITOBJS           8
#define MAX_WAITOBJS_PTR_SIZE  ( MAX_WAITOBJS * sizeof( LPSEMAPHORE ) )
#define TLS_MAX_INDEXS         32
#define TLS_ALLOC_SIZE			   ( TLS_MAX_INDEXS*sizeof(DWORD) )

#define TLS_MSGQUEUE           31    // the value define at epwin.h too
									 // change must sync!!!

#define TLS_TRY                30
#define TLS_CRITICAL_SECTION   29
#define TLS_SOCKET             28    // the value define at stk_callapi.c too, 
									 // change must sync!!!
									

#define TLS_SYS_INDEXS         8

//可分配 tls
#define TLS_ALLOCABLE_INDEXS        (TLS_MAX_INDEXS-TLS_SYS_INDEXS)
#define IS_RESERVE_TLS( idx ) ( (idx) >= TLS_ALLOCABLE_INDEXS && (idx) < TLS_MAX_INDEXS )
#define TLS_MASK           0xff000000

typedef struct _HANDLEDATA
{
	OBJLIST obj;
	HANDLE hThis;
	LPVOID  lpvObjData;
	DWORD   dwUserInfo;
}HANDLEDATA, FAR * LPHANDLEDATA;

// user by intr.s , if change, you must change intr.s's code
typedef struct _CALLRET
{
	UINT uiRetMode;
	LPVOID lpRetSP; //该版本暂时未用,以后....
}CALLRET, FAR * LPCALLRET;

/*
typedef struct _MODULE{
	UINT objType;
	struct _MODULE * lpNext;
	LPTSTR lpszApName;
	//WINMAINPROC lpProc;
	//void * lpProc;
	HICON hSmallIcon;//
	HICON hLargeIcon;//
	DWORD dwModule; // user module data
}MODULE, FAR * PMODULE;
*/
//typedef void ( WINAPI * LPFNSTARTMODULE )( LPTCSTR lpszFileName, LPCTSTR lpszCommandLine );

#define CREATE_EXE           0
#define CREATE_LIBRARY       1

#define LF_EXEC             0x0000001    // run .exe file or LoadLibrary with .dll 
#define LF_LOAD_TO_MEM      0x0000002    // LoadLibrary with .exe file only

#define LF_DEFLOAD          (LF_EXEC|LF_LOAD_TO_MEM)

#define LF_NOT_INIT_DLL     0x0000004    // LoadLibrary with .dll file but not call Dll_Init


// 得到CPU依赖模式
// 传入 0： 核心   1：系统  2：用户
#define M_KERNEL	0
#define M_SYSTEM	1
#define M_USER		2

typedef struct _MODULE_DRV
{  //
	HANDLE (*lpCreateModule)( LPCTSTR lpszProcessName , HANDLE hOwnerProcess, UINT uiCreateFlag );
	BOOL (*lpFreeModule)( HANDLE hModule );
	HANDLE (*lpGetModuleFileHandle)( HANDLE hModule ); 
	LPCTSTR (*lpGetModuleFileName)( HANDLE hModule ); 
	LPVOID (*lpGetProcAddress)( HANDLE hModule, LPCSTR lpProcName );
	DWORD  (*lpGetModuleSectionOffset)( HANDLE hModule, LPCTSTR lpcszSectionName );
	UINT (*lpLoadModule)( HANDLE hModule, HANDLE hInst, LPCTSTR lpCmdLine, UINT uiLoadFlag );
}MODULE_DRV, FAR * PMODULE_DRV;
typedef const FAR MODULE_DRV * PCMODULE_DRV;

typedef struct _MODULE
{
	PCMODULE_DRV lpmd;  //方法
	HANDLE hModule;		//return by HandleAlloc
	HANDLE hModuleData; //return by lpCreateModule
	CRITICAL_SECTION csModule;
}MODULE, FAR * PMODULE;

typedef struct _SECURITY
{
	DWORD dwInfo;
}SECURITY, FAR * LPSECURITY;


#define AccessKey_Add( lpAccessKey, aky ) ( *(lpAccessKey) |= (aky) )
#define AccessKey_Remove( lpAccessKey, aky ) ( *(lpAccessKey) &= ~(aky) )
#define AccessKey_Test( lpAccessKey, aky ) ( *(lpAccessKey) & (aky) )
#define AccessKey_Set( lpAccessKey, aky ) ( *(lpAccessKey) = (aky) )



//进程空间的段结构
typedef struct _PROCESS_SEGMENTS
{
	struct _PROCESS_SEGMENTS FAR * lpNext;
    LPSEGMENT lpSeg;
	DWORD     dwSegBaseAddress; //对齐段边界（32M）
	// 2005-02-02 增加一个关键二级页表
	WORD    uiPageTableStart;	//二级页表开始位置，必须对齐 1m = (1024 * 1024) / 4096 = 256
	WORD    uiPageTableCount;	 //二级页表个数
	LPDWORD lpdwSecondPageTable;	//二级页表, uncache
	LPDWORD lpdwPhySecondPageTable;	//
}PROCESS_SEGMENTS, FAR * LPPROCESS_SEGMENTS;
LPPROCESS_SEGMENTS AllocSegmentIndexAndSegment( DWORD dwSegBaseAdr );

#define KERNEL_SEGMENT_INDEX 96
//=0xC0000000
#define KERNEL_SEGMENT_BASE  (KERNEL_SEGMENT_INDEX << SEGMENT_SHIFT) //
//extern PROCESS_SEGMENTS KernelSegment;   


typedef struct _PROCESS{
    DWORD objType;    // must be OBJ_INSTANCE	

    struct _THREAD FAR * lpFirstThread; //
	struct _THREAD FAR * lpMainThread;
	HANDLE hProcess;  // this process's handle
	DWORD  dwProcessId;
	//HANDLE hModule;
	//PCMODULE_DRV pModuleDrv;
	PMODULE pModule;
	DWORD  dwThreadCount;

    SECURITY * lpSecurity;

	DWORD   dwtlsMask;
	LPHEAP  lpHeap;          // process heap

	DWORD dwVirtualAddressBase;   // virtual address base
	//LPSEGMENT lpSegments;
	LPPROCESS_SEGMENTS lpProcessSegments;
    DWORD dwFlags;		// 0xF 为 process mode
	LPCTSTR lpszApplicationName;
	LPCTSTR lpszCommandLine;
	LPVOID  lpUserData;
	//DWORD * lpPTBL[32];
	VOID * lpCpuPTS;			//为页调度准备的数据
	ACCESS_KEY akyAccessKey;     //进程合法空间
}PROCESS, FAR * LPPROCESS;

extern HANDLEDATA InitKernelProcessHandle;
extern HANDLEDATA InitKernelThreadHandle;
//extern MODULE InitKernelModule;
extern DWORD InitKernelThreadTLS[TLS_MAX_INDEXS];
//extern SEGMENT NKSegment;
#define KERNEL_PROCESS_HANDLE  ((HANDLE)&InitKernelProcessHandle)
#define KERNEL_THREAD_HANDLE   ((HANDLE)&InitKernelThreadHandle)
#define KERNEL_MODULE_PTR  NULL //(&InitKernelModule)
#define INIT_PROCESS \
{ \
	OBJ_PROCESS, \
	(LPTHREAD)&_InitKernelThread, \
	(LPTHREAD)&_InitKernelThread, \
	KERNEL_PROCESS_HANDLE, \
/*id*/	0, \
/*module*/	KERNEL_MODULE_PTR, \
	1, \
/*security*/	NULL, \
/*tls*/	TLS_MASK,\
/*heap*/ NULL, \
	KERNEL_SEGMENT_BASE, \
	NULL, \
	M_SYSTEM, \
    "kernel.exe", \
	NULL, \
	NULL, \
	NULL, \
	0x1 \
}

LPPROCESS _GetPROCESSPtr( HANDLE hProcess );

// 定义异常处理
typedef struct _EXCEPTION
{
	struct _EXCEPTION FAR * lpNext;
//	DWORD dwStack;	//需要恢复的栈 stack
//	DWORD dwIP;		//需要恢复的PC地址 stack
	UINT uiMode;
	LPPROCESS lpOwnerProcess;
	LPCALLSTACK lpCallStack;
	EXCEPTION_CONTEXT jmp_data;	//线程上下文
}EXCEPTION, * LPEXCEPTION;

VOID LeaveException( BOOL bException );
VOID HandlerException( LPEXCEPTION_CONTEXT jmp_data, int retv, UINT mode );


LPPROCESS WINAPI KL_SetForegroundProcess( LPPROCESS lpProcess );
LPPROCESS WINAPI KL_GetForegroundProcess( void );
BOOL WINAPI KL_IsForegroundProcess( void );

//#ifdef VIRTUAL_MEM
//#define MAKE_HANDLE( lphd ) (HANDLE)( (((DWORD)lphd) & 0x00ffffff) | ( random() & 0xff000000 ) )
//#define GET_PTR( h ) (LPHANDLEDATA)( ( ((DWORD)h) & 0x00ffffff ) | 0x8C000000 )
// format:
// FR PP PPPP PPPP
//extern DWORD dwMainMemBase;
#define HANDLE_PTR_MASK 0x1FFFFFFC

//define HANDLE_BASE( h ) ( ( (DWORD)&(h) ) & 0xFF000000 )
//#define MAKE_HANDLE( ptr ) (HANDLE)( ( ( random() << 24 ) & 0x0F000000 ) | 0xE0000000 | ( (DWORD)ptr >> 2 ) )
//#define GET_PTR( h ) (LPHANDLEDATA)( ( (DWORD)h & 0xE0000000 ) == 0xE0000000 ? ( ( ( (DWORD)h & 0x00FFFFFF ) << 2 ) | HANDLE_BASE(h) ) : 0 )
extern DWORD   dwHandleBase;
extern LPBYTE  lpbSysMainMem;// = _mem;
extern LPBYTE  lpbSysMainMemEnd;

#define MAKE_HANDLE( ptr ) (HANDLE)( ( random() & (~HANDLE_PTR_MASK) ) |  ( (DWORD)ptr & HANDLE_PTR_MASK ) )
#define GET_PTR( h ) (LPHANDLEDATA)( ( (DWORD)h & HANDLE_PTR_MASK ) + dwHandleBase )

#define SYS_CALL_RETURN 0xF0100000
//PFNVOID MakeAPICall( DWORD dwCallInfo, LPDWORD lpdwCallSP, PFNVOID pfnRetAdress, UINT uiRetMode );
// 必须 与 swi.s and intr.s 同步
typedef struct _SYSCALL
{
	UINT uiCallMode;	// 输入之前的模式; 输出，将要切换到的新模式
	DWORD dwCallInfo;
	PFNVOID pfnRetAdress;

	UINT uiArgs[1];		//可变长度
}SYSCALL, FAR * LPSYSCALL;
//PFNVOID MakeAPICall( DWORD dwCallInfo, LPDWORD lpdwCallSP, PFNVOID pfnRetAdress, UINT uiRetMode );
PFNVOID MakeSysCall( LPSYSCALL lpCall );

//#endif
//
//#ifdef EML_WIN32
//#define MAKE_HANDLE( ptr ) (HANDLE)( ( ( random() << 24 ) & 0x0F000000 ) | 0xC0000000 | ( (DWORD)ptr >> 2 ) )
//#define GET_PTR( h ) (LPHANDLEDATA)( ( (DWORD)h & 0xC0000000 ) == 0xC0000000 ? ( (DWORD)h & 0x00FFFFFF ) << 2 : 0 )
//#endif

void * HandleToPtr( HANDLE handle, UINT objType );
void * SetHandleObjPtr( HANDLE handle, LPVOID lpvObj );


BOOL CloseProcessHandle( LPPROCESS lpProcess );
HANDLE Handle_Alloc( LPPROCESS lpProcess, LPVOID lpvObjData, UINT uiObjType );
UINT GetHandleObjType( HANDLE handle );
// 2004-02-19
BOOL GetHandleUserInfo( HANDLE handle, LPDWORD lpdwUserInfo );
//DWORD GetHandleUserInfo( HANDLE handle );
DWORD SetHandleUserInfo( HANDLE handle, DWORD dwUserInfo );

//#define MAX_SUSPEND_COUNT 127
//#define MAX_LOCKSCHE_COUNT 127

//
typedef struct _REALTIME_BITMAP
{
// 8个插槽， 每个byte的每一位代表一个就绪线程
    BYTE bRTSlots[8];
// 当前 实时线程运行状况，每一位代表一个插槽
    UINT uRTReadySlot;
}REALTIME_BITMAP, FAR * LPREALTIME_BITMAP;

typedef struct _OWNER_OBJ_LIST
{
	struct _OWNER_OBJ_LIST * lpNext;
	struct _OWNER_OBJ_LIST * lpPrev;
	UINT    uObjType;
	LPVOID  lpvData;	
}OWNER_OBJ_LIST, FAR * LPOWNER_OBJ_LIST;


typedef struct _THREAD_PAGE_TABLE
{
	volatile DWORD * lpdwFirstLevelEntry;  //该页表对应（代表）的第一级入口地址;//
	volatile DWORD * lpdwSecondLevelEntry;	 //该页表当前的设置的进入点对应（代表）的有效 第xxx_4K位置
	volatile DWORD * lpdwVirtualPageTable; //页表 1k ( 256 * 4k)
	volatile DWORD * lpdwPhyPageTable; //物理页表 1k ( 256 * 4k)
}THREAD_PAGE_TABLE, FAR * LPTHREAD_PAGE_TABLE;
#define INIT_THREAD_PAGE_TABLE { 0, 0, 0, 0 }
#ifdef VIRTUAL_MEM
#define USE_THREAD_PAGE_TABLE
#endif
#define THREAD_MAGIC	0x4b6d6f73
typedef struct _THREAD {
	DWORD objType;			// must use OBJ_SCHEDULE
	UINT  uiRefCount;
	volatile DWORD dwState;	

	int  nTickCount;  // 一次 tick, 减-

	// priority = nCounter + nBoost + [nRTPriority]
	BYTE nBoost;  // 临时加速, 当thread 主动放弃CPU时,被置为0
	BYTE nCurPriority; //
	BYTE nOriginPriority;  //   
	BYTE fPolicy;  //

    DWORD dwSignal;
    DWORD dwBlocked;
	//DWORD dwFlags;	        // thread flags, defined below
    
	DWORD dwErrorCode;   // set by SetLastError

	DWORD dwThreadId;

	DWORD dwTimeout;
	TIMERLIST timer;


	BYTE nSuspendCount;		// suspend depth
    BYTE nLockScheCount;    // lock sche depth

	BYTE nRotate;          // rotate value
	BYTE flag;

	struct _THREAD * lpNextThread, * lpPrevThread;  // threads alloced

	struct _THREAD * lpNextRun, * lpPrevRun;        // threads  running

	DWORD dwExitCode;	// exit code;

    // thread switch state, the tss struct is different with cpu
	TSS tss;
    
    //struct _THREAD * lpParent,  * lpChild, * lpPrevSibling, * lpNextSibling;
	struct _THREAD * lpPrevThreadInProcess, * lpNextThreadInProcess;

    LPDWORD lpdwThreadStack;   // thread stack
	DWORD dwThreadStackSize;
	LPDWORD lpdwThreadUserStack; // thread user stack

	SECURITY * lpSecurity;

	LPSEMAPHORE  lpsemExit;  // semaphore to exit
	LPSEMAPHORE  * lppsemWait;  // semaphore to wait
	WAITQUEUE * lpWaitQueue; //
	WORD         nWaitCount;// count of semaphore to wait
	WORD         nsemRet;
	LPVOID       lpRouser;  // 唤醒者current set signal's semaphore

	DWORD        dwTimerCount;  //线程使用处理器时间
	LPVOID       lpfnStartAddress;   // thread start address
	LPDWORD      lpdwTLS;  //thread local storage 
	//LPDWORD      lptlsSys;  //thread local storage ( for sys )

	HANDLE hThread;    // this thread's handle
	LPPROCESS lpOwnerProcess;
	LPPROCESS lpCurProcess;
	LPCALLSTACK lpCallStack;
//    LPRESOURCE lpresOwned;
    //TIMERLIST * lpTimer;// sleep or timeout
	LPEXCEPTION lpException;	//异常处理上下文
	//LPOWNER_OBJ_LIST lpOwnerList;	//线程对资源的拥有
	//THREAD_PAGE_TABLE pageTable;
	LPDWORD lpdwThreadKernelStack;   // thread Kernel stack
	ACCESS_KEY akyAccessKey;     //该线程在运行时的合法进程的可存取空间
	DWORD  dwMagic; // check value = 'Kmos' 
}THREAD, * LPTHREAD;

#define THREAD_STRUCT_SIZE (1024*1)

//LPTHREAD _GetHTHREADPtr( HANDLE hThread );

//  thread flags

#define TF_STARTING	    0x00000001	// being created 
#define TF_EXITING	    0x00000002	// getting shut down 
#define TF_FORKNOEXEC   0x00000004	// forked but didn't exec

#define LOCK_SCHE()       \
            do \
			{ \
		        UINT _uiSave; \
		        LockIRQSave( &_uiSave ); \
			    if( lpCurThread->nLockScheCount < 255 ) \
				    lpCurThread->nLockScheCount++; \
				UnlockIRQRestore( &_uiSave ); \
			}while(0)

#define UNLOCK_SCHE()       \
            do \
			{ \
		        UINT _uiSave; \
		        LockIRQSave( &_uiSave ); \
			    ASSERT( lpCurThread->nLockScheCount ); \
				if( lpCurThread->nLockScheCount ) \
				    lpCurThread->nLockScheCount--; \
                UnlockIRQRestore( &_uiSave ); \
				if( bNeedResched && lpCurThread->nLockScheCount == 0 ) \
				    Schedule(); \
			}while(0)
 
#define INIT_THREAD \
/* state etc */	{ OBJ_THREAD, 0, 0, DEF_PRIORITY, DEF_BOOST, DEF_PRIORITY, DEF_PRIORITY, \
/*fPolicy*/	      THREAD_POLICY_OTHER, \
/*dwSignal*/      0, \
/*dwBlocked */    0, \
/* dwErrorCode*/  0, \
/* wThreadID */   0, \
/* timeout  */    0, \
/*timer*/        INIT_TIMERLIST_STRUCT, \
/* nSuspendCount */ 0, \
/* nLockScheCount */ 1, \
/* nRotate */      0, \
/* dumy */         0, \
/* lpNext... */   (LPTHREAD)&_InitKernelThread, (LPTHREAD)&_InitKernelThread, (LPTHREAD)&_InitKernelThread, (LPTHREAD)&_InitKernelThread, \
/* exit code */   0, \
/* tss */         INIT_TSS, \
/* nextInProc...*/ 0, 0, \
/* stack */       NULL, \
/* stack size*/   0, \
/* user stack*/   NULL, \
/* sericuty */    NULL, \
  /*lpsemExit*/	  NULL, \
/*lpsemWait*/     NULL, \
/*lpWaitQueue*/   NULL, \
/*nWaitCount*/    0, \
/*nsemRet*/       0, \
/*lpRouser*/      NULL, \
/*uiTimerCount*/  0,\
/*lpfnStartAddress*/    0,\
/*tls*/           InitKernelThreadTLS, \
/*hThread*/       KERNEL_THREAD_HANDLE,  \
/*lpOwnerProcess*/&InitKernelProcess, \
/*lpCurProcess*/  &InitKernelProcess, \
/*lpCallStack*/   NULL, \
/*lpException*/	  NULL, \
/*lpdwThreadKernelStack*/	  NULL, \
/*accesskey*/     0xffffffff, \
/*magic*/		  THREAD_MAGIC \
				}

//extern void INTR_ON( void );
//extern void INTR_OFF( void );

//define interrupt server rountion

//extern DWORD dwISRActive;
//extern DWORD dwISRMask;   // used by DisableISR and EnableISR

//#define ACTIVE_ISR( nIndex ) { dwISRActive |= 1 << (nIndex); }
//void ISR_Init( void );
//void ISR_Handler(void);
//void ISR_Enable( int intr );
//void ISR_Disable( int intr );
//void ISR_Active( int intr );
//void ISR_Register( UINT intr, LPISR lpfun );

extern void DoTimer( LPVOID );

void _LinkThread( LPTHREAD lpThread );
void _RemoveThread( LPTHREAD lpThread );
void _WakeupThread( LPTHREAD lpThread, BOOL );

void AddToRunQueue( LPTHREAD lpThread );
void RemoveFromRunQueue( LPTHREAD lpThread );
void MoveToEndOfRunQueue( LPTHREAD lpThread );

void InitThreadTSS( LPTHREAD lpThread, 
					LPTHREAD_START_ROUTINE lpStartAdr,
					LPBYTE lpStack,
					LPVOID lpUserParameter
					 );

void SwitchTo( LPTHREAD lpFrom, LPTHREAD lpTo );
void SwitchToStackSpace( LPVOID );

void BH_FreeMemBlock( int id, LPVOID lpvBlock );
void BH_AllocMemBlock( int id, LPVOID lpvBlock );

typedef struct _APIINFO{
	const PFNVOID * lpfn;
	const DWORD * lpArgs;
	UINT    uiMaxOption;
	LPPROCESS lpProcess;
	HANDLE hServer;
	LPSERVER_CTRL_INFO lpServerCtrlInfo;
	DWORD dwServerInfo;
}APIINFO, FAR * LPAPIINFO;

typedef BOOL ( CALLBACK * LPSERVER_ENUM_PROC )( APIINFO * lpAPI, LPVOID lpParam );
BOOL FASTCALL EnumServer( LPSERVER_ENUM_PROC lpEnumFunc, LPVOID lpParam );

//PMODULE FASTCALL LoadModule( LPCTSTR lpFileName, DWORD dwFlag );

BOOL FASTCALL DoCreateProcess(
						   LPCTSTR lpszApplicationName,                 // name of executable module
						   LPCTSTR lpszCommandLine,                      // command line string
						   LPSECURITY_ATTRIBUTES lpProcessAttributes, // SD
						   LPSECURITY_ATTRIBUTES lpThreadAttributes,  // SD
						   BOOL bInheritHandles,                      // handle inheritance option
						   DWORD dwCreationFlags,                     // creation flags
						   LPVOID lpEnvironment,                      // new environment block
						   LPCTSTR lpCurrentDirectory,                // current directory name
						   LPSTARTUPINFO lpStartupInfo,               // startup information
						   LPPROCESS_INFORMATION lpProcessInformation, // process information
						   PCMODULE_DRV pmd
						   );

extern VOID FASTCALL KC_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection );

BOOL SwitchToProcess( LPPROCESS hProcess, LPCALLSTACK lpcs );
void SwitchBackProcess( void );
BOOL SwitchToProcessByHandle( HANDLE hProcess, LPCALLSTACK lpcs );
DWORD ScheduleTimeout( DWORD dwTimeout );
//LPPROCESS GetAPICallerProcessPtr( void );
#define GetAPICallerProcessPtr()  (lpCurThread->lpCurProcess)

//#define ALIGN_SIZE( n )    ( ( (n) + 3 ) & ~3 )
//#define HEAP_PROCESS   ALIGN_SIZE( sizeof( PROCESS ) )
//#define 
#ifdef VIRTUAL_MEM
#define MapProcessPtr( lpv, lpProcess ) \
    ( ( (DWORD)(lpv) >= 0x10000 && (DWORD)(lpv) < 0x2000000 ) ? \
      (LPVOID)( (DWORD)(lpv) | (lpProcess)->dwVirtualAddressBase ) : (lpv) )
#define UnMapProcessPtr( lpv ) \
    ( ( (DWORD)(lpv) >= 0x2000000 ) ? \
      (LPVOID)( (DWORD)(lpv) & (0x2000000-1) ) : (lpv) )

#else
    #define MapProcessPtr( lpv, lpProcess ) (lpv)
    #define UnMapProcessPtr( lpv ) ( lpv)
#endif

#define IsKernelVirtualAddress( dwAdr ) ( ( (DWORD)(dwAdr) ) >=  0x80000000 && ( (DWORD)(dwAdr) ) <  0xC0000000 )

///////////GLOBAL DATA REGION////////////////////////
//extern HANDLEDATA InitKernelProcessHandle;
//extern THREAD InitKernelThread;
extern const LPTHREAD lpInitKernelThread;
//#define InitKernelThread (*lpInitKernelThread)

extern PROCESS InitKernelProcess;
extern HEAP InitKernelHeap;
//extern LPTHREAD lpSysThreads[NR_THREADS];
extern LPTHREAD lpCurThread;

// 实是时功能

//extern REALTIME_BITMAP rtb_Run;

//#define IS_REALTIME_THREAD( nPriority ) ( (nPriority) < 64 )

//extern BOOL RemoveRealTimeThread( LPTHREAD lpThread );
//extern LPTHREAD GetNextRealTimeThread( LPREALTIME_BITMAP lprtb );
//extern LPTHREAD GetNextRunRealTimeThread( void );
//extern BOOL AddRealTimeThread( LPTHREAD lpThread );
//extern void InitRealTimeData( void );
//#define HAS_REALTIME_THREAD ( rtb_Run.uRTReadySlot )
//

//LPTHREAD GetHighestPriority( LPUINT lpPriority );
//void SetPriorityBitmap( UINT uPriority );
//void ClearPriorityBitmap( UINT uPriority );


extern DWORD volatile dwJiffies;
extern DWORD dwTimerTicks;
//extern int iNeedResched;
extern BOOL bNeedResched;
extern int volatile nInterruptCount;

extern DWORD * lpdwFirstPTE;
extern DWORD * lpdwSecondPTE;
extern DWORD * lpdwSecondPhyPTE;
#define MAX_API   32
extern APIINFO apiInfo[];
extern LPSEGMENT Seg_Alloc( void );
extern void Seg_Free( LPSEGMENT lpSeg );

extern void Handle_Free( HANDLE handle, BOOL bRemove );
extern HANDLE Handle_Alloc( LPPROCESS lpOwnerProcess, LPVOID lpvObjData, UINT uiObjType );
extern void * HandleToPtr( HANDLE handle, UINT uiObjType );
extern BOOL Handle_CloseAll( LPPROCESS lpOwnerProcess );
extern VOID Handle_AddRef( HANDLE handle );

UINT GetCPUMode( DWORD dwMode );

DWORD SetThreadIP( LPTHREAD lpThread, DWORD dwIP );
int CallUserStartupCode( LPVOID lpIP, LPVOID lpParam );
void HandleThreadExit( void );

extern LPVOID DoTlsGetValue( LPTHREAD lpThread, DWORD dwTlsIndex );
extern BOOL DoTlsSetValue( LPTHREAD lpThread, DWORD dwTlsIndex, LPVOID lpValue );

LPSEMAPHORE _SemaphoreCreate(
							 LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  // must is 0
							 int nInitialCount,                         // initial count
							 int nMaximumCount,                     // maximum count
							 LPCTSTR lpName,     // must is 0
							 UINT uiFlag
							 );
//BOOL _SemaphoreDelete( LPSEMAPHORE lpsem );
BOOL _SemaphoreRelease( LPSEMAPHORE lpsem, int lReleaseCount, LPINT lpPreviousCount );
//BOOL _SemaphoreRemove( LPSEMAPHORE lpsem );
//BOOL _SemaphoreDeletePtr( LPSEMAPHORE lpsem );
BOOL _CloseSemaphoreObj( HANDLE hSemaphore, UINT uObjType );
VOID _Semaphore_HandleThreadExit( LPVOID lpOwnerID );

//PMODULE InitModule( LPPROCESS lpOwner, HANDLE hModuleData, PCMODULE_DRV lpmd );
PMODULE Module_Init( LPPROCESS lpOwner, LPCTSTR lpszApplicationName, PCMODULE_DRV lpmd, UINT uiCreateFlag );
void Module_DeInit( PMODULE );

//extern void GetMMUContext( LPPROCESS, UINT uiDebugFlag );
extern void GetMMUContext( LPTHREAD lpNewThread, UINT uiDebugFlag, LPPROCESS lpNewProcess );
extern void HandleSignal( void );

//extern BOOL FreeProcessId( DWORD dwProcessId );
//extern DWORD AllocProcessId( void );
extern VOID FASTCALL KC_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection );

void __AddWaitQueue( LPWAITQUEUE *lppQueue, LPWAITQUEUE lpWait );
void __RemoveWaitQueue( LPWAITQUEUE *lppQueue, LPWAITQUEUE lpWait );

void SetThreadCurrentPriority( LPTHREAD lpThread, UINT lPriority );

#define MAX_PROCESSES  32	// 必须与 accesskey 的最大 maskbit 数一致
#define MAX_PROCESS_ID 64
//共享数据段 0x42000000 ~ 44000000 / 32m
#define SHARE_SEGMENT_INDEX  (MAX_PROCESSES+1)
#define SHARE_MEM_BASE (SHARE_SEGMENT_INDEX<<SEGMENT_SHIFT)


//extern lppProcessSegmentSlots dwProcessSlots[MAX_PROCESS_ID];
extern LPPROCESS_SEGMENTS lppProcessSegmentSlots[MAX_PROCESS_ID];
extern LPPROCESS lppProcessPtr[MAX_PROCESS_ID];

// CREATE flag use by CreateProcess and CreateThread
#define CREATE_MAINTHREAD           0x80000000
#define CREATE_SYSTEM_MODE          0x40000000

DWORD DoSuspendThread( LPTHREAD lpThread );
DWORD DoResumeThread( LPTHREAD lpThread );

BOOL _InitSysMem( void );
BOOL InitScheduler(void);
DWORD FASTCALL EnumServerHandler( DWORD dwEventCode, DWORD dwParam, LPVOID lpParam );

//////////////////PAGE MGR-BEGIN////////
UINT Page_CountFreePages( void );
BOOL Page_Lock( DWORD dwPages );
BOOL Page_Unlock( DWORD dwPages );
void * Page_Alloc( BOOL bUpdateFreePages );
BOOL Page_Free( LPVOID lpvMemAdr  );
BOOL Page_Duplicate( LPVOID lpvMemAdr );
BOOL Page_InitSysPages( void );
//////////////////PAGE MGR-END  ////////


LPSEMAPHORE GetSemaphoreAndLock( HANDLE hSem, UINT uObjType );
VOID ReleaseSemaphoreAndUnlock( LPSEMAPHORE lpSem );

//
#define CURRENT_PROCESS_HANDLE ( (HANDLE)0xFFFF0001 )
#define CURRENT_THREAD_HANDLE  ( (HANDLE)0xFFFF0002 )

//CPU 依赖函数
DWORD _GetPhysicalPageAdr( DWORD dwVirtualAdr );
DWORD _GetVirtualPageAdr( DWORD dwPhysicalAdr );
DWORD GetProtectFromAttrib( DWORD dwAttrib );
DWORD GetAttribFromProtect( DWORD dwProtect );
DWORD GetPhyPageAdrMask( void );
DWORD GetPhyPageFlagMask( void );
VOID  FreeCPUPTS( VOID * lpPTS );
VOID * AllocCPUPTS( VOID );

/////////////////////////////////////////////////////

#endif  //__EPCORE_H



