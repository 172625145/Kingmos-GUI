/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：cpu模拟器
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/
//typedef void * HKEY;
#include <windows.h>
#include <eversion.h>
//#include <ewindows.h>
#include <w32intr.h>
//#include <ekeydrv.h>
#include <cpu.h>
#include <crtdbg.h>

#define WM_RTCMSG (WM_USER+2000)
#define WM_TIMERMSG (WM_USER+2001)
/*
typedef struct _KERNEL_DATA
{
	_INTRBITS intrBits;
	_INTRMASK intrMask;
	int IntrEnable;
	MSG IntrMsg[32];
	MSG msgCurrent;
	HANDLE hKingmosThread;
	HANDLE hIntrThread;
	HANDLE hHardwareEvent;
	CONTEXT  ctSave;
	UINT uiLockSegStart;
	UINT uiLockSegEnd;
	CRITICAL_SECTION csMsg;
	LONG nLockCount;
}KERNEL_DATA, FAR * LPKERNEL_DATA;
*/
volatile LPKERNEL_DATA lpKernelData;
static BOOL bPowerOff = FALSE;
static BOOL bPowerKeyDown = FALSE;
static BOOL CPU_CheckIRQEvent( void );

extern UINT LockSeg_Start( void );
extern UINT LockSeg_End( void );
extern void INTR_Interrupt( void );
extern void KingmosStart( LPVOID );

void CPU_IRQTrap( void );
//extern void JmpBack( TSS *, volatile int * lpIntrEnable );

//VOID ReleaseKernelData( void )
//{
//	LONG lOldProtect;
//	if( InterlockedDecrement( &lpKernelData->nLockCount ) == 0 )
//	    VirtualProtect( lpKernelData, 1024 * 4, PAGE_NOACCESS, &lOldProtect );	
//}

//VOID GetKernelData( void )
//{
//	LONG lOldProtect;
//	if( InterlockedIncrement( &lpKernelData->nLockCount ) > 0 )
//	    VirtualProtect( lpKernelData, 1024 * 4, PAGE_READWRITE, &lOldProtect );
//	else
//	{
//		_ASSERT( 0 );
//	}	
//}

void UnlockIRQRestore( UINT * lpuiSave )
{
//    GetKernelData();
	InterlockedExchange( (LPVOID)&lpKernelData->IntrEnable, *lpuiSave );
//	ReleaseKernelData();
}

void LockIRQSave( UINT * lpuiSave )
{
  //  GetKernelData();
	*lpuiSave = (UINT)InterlockedExchange( (LPVOID)&lpKernelData->IntrEnable, FALSE );
	//ReleaseKernelData();
}

#ifdef __DEBUG

static char * lpFileName;
static int iLine;
void _INTR_OFF( char * lpfn, int line )
{  
	lpFileName = lpfn;
	iLine = line;

	//GetKernelData();
	//_ASSERT( lpKernelData->IntrEnable );
	InterlockedExchange( (LPLONG)&lpKernelData->IntrEnable, FALSE );
	//ReleaseKernelData();

	lpFileName = lpfn;
	iLine = line;
}

void _INTR_ON( char * lpfn, int line )
{
	//GetKernelData();
	InterlockedExchange( (LPLONG)&lpKernelData->IntrEnable, TRUE );
	//ReleaseKernelData();
}

#else

void INTR_OFF( void )
{
	//GetKernelData();
	InterlockedExchange( (LPLONG)&IntrEnable, FALSE );
	//ReleaseKernelData();
}

void INTR_ON( void )
{
	//GetKernelData();
	InterlockedExchange( (LPLONG)&IntrEnable, TRUE );
	//ReleaseKernelData();
}

#endif


//中断源
void CPU_SetIRQEvent( MSG * lpmsg )
{
	EnterCriticalSection( &lpKernelData->csMsg );
    //GetKernelData();
	lpKernelData->msgCurrent = *lpmsg;
	CPU_CheckIRQEvent();
		//SetEvent(lpKernelData->hHardwareEvent);
	//ReleaseKernelData();
	LeaveCriticalSection( &lpKernelData->csMsg );
}
//检查中断源是否可以使用

extern HANDLE hMouseEvent;
extern HANDLE hKeyEvent;

static BOOL CPU_CheckIRQEvent( void ) //MSG * lpmsg )
{
	int * lpIntr;
	int * lpMask;
	MSG * lpmsg;
	BOOL bRetv = TRUE;
//	UINT uiSave;
//	LockIRQSave( &uiSave );

	EnterCriticalSection( &lpKernelData->csMsg );
    //GetKernelData();

    lpIntr = (int *)&lpKernelData->intrBits;
	lpMask = (int *)&lpKernelData->intrMask;
	lpmsg = &lpKernelData->msgCurrent;

	if( lpmsg->message == WM_RTCMSG )
	{
	    lpKernelData->intrBits.timer1 = 1;
        lpKernelData->IntrMsg[ID_INTR_TIMER1] = *lpmsg;// = *lpmsg;
	}
   
    else if( lpmsg->message >= WM_MOUSEFIRST &&
		lpmsg->message <= WM_MOUSELAST )
	{
		
		if( lpmsg->message == WM_MOUSEMOVE && 
			(lpmsg->wParam & MK_LBUTTON) == 0 )
		{
			bRetv = FALSE; // nothing to do
		}
        else
		{
			lpKernelData->intrBits.mouse = 1;        
			lpKernelData->IntrMsg[ID_INTR_MOUSE] = *lpmsg;
		}
		SetEvent( hMouseEvent );
    }
    else if( lpmsg->message == WM_KEYDOWN || 
             lpmsg->message == WM_KEYUP )
    {
		if( bPowerOff )
		{
			bRetv = FALSE;
			if( lpmsg->message == WM_KEYDOWN )
			{
				if( bPowerKeyDown == FALSE )
				{	//wake up event
					lpKernelData->intrBits.keyboard = 1;
					lpKernelData->IntrMsg[ID_INTR_KEYBOARD] = *lpmsg;
					bRetv = TRUE;
				}
			}
			else if( lpmsg->message == WM_KEYUP )
			{
				bPowerKeyDown = FALSE;
			}
		}
		else
		{
			lpKernelData->intrBits.keyboard = 1;
			lpKernelData->IntrMsg[ID_INTR_KEYBOARD] = *lpmsg;
		}
		if( bRetv )
		    SetEvent( hKeyEvent );
    }
    else if( lpmsg->message == WM_CLOSE )
    {
        lpKernelData->intrBits.keyboard = 1;
		lpKernelData->IntrMsg[ID_INTR_KEYBOARD].message = WM_KEYDOWN;
		lpKernelData->IntrMsg[ID_INTR_KEYBOARD].wParam = 0X06;//=VK_POWEROFF;
		lpKernelData->IntrMsg[ID_INTR_KEYBOARD].lParam = 0;
	}
    else if( lpmsg->message == WM_TIMERMSG )
    {
		lpKernelData->intrBits.timer0 = 1;
        //lpKernelData->IntrMsg[ID_INTR_TIMER0] = *lpmsg;
		lpKernelData->IntrMsg[ID_INTR_TIMER0].message = WM_TIMER;
    }
	else
	{
		bRetv = FALSE;
	}


	//ReleaseKernelData();

	LeaveCriticalSection( &lpKernelData->csMsg );
	return bRetv;

	

    //if( *lpMask & *lpIntr )
	//{
		//UnlockIRQRestore( &uiSave );
	    //SetEvent(hHardwareEvent);
	//}
	//else
      //  UnlockIRQRestore( &uiSave );


	//Sleep(1);
}
/*
static void IRQ_Handler( void )
{	
	extern void INTR_Interrupt(void);
	int * lpIntr = (int *)&intrBits;
	int * lpMask = (int *)&intrMask;

	if( *lpMask & *lpIntr )
	{
        INTR_Interrupt();
	}
}
*/

// the  CPU_IRQTrap is called by cpu_eml_thread
//static CONTEXT * lpJumpContext = NULL;
/*
void CPU_IRQTrap( void )
{
	//CONTEXT context;
	TSS tss;
	//extern LPVOID lpCurThread;
	//extern LPVOID DoTlsGetValue( LPVOID lpThread, DWORD dwTlsIndex );
	//extern LPVOID DoTlsSetValue( LPVOID lpThread, DWORD dwTlsIndex );
	
	//context = *((CONTEXT*)lpKernelData->bCPUConext);
	//GetKernelData();
	tss.eax = ((CONTEXT*)lpKernelData->bCPUConext)->Eax;
	tss.ebx = ((CONTEXT*)lpKernelData->bCPUConext)->Ebx;
	tss.ecx = ((CONTEXT*)lpKernelData->bCPUConext)->Ecx;
	tss.edx = ((CONTEXT*)lpKernelData->bCPUConext)->Edx;
	tss.edi = ((CONTEXT*)lpKernelData->bCPUConext)->Edi;
	tss.esi = ((CONTEXT*)lpKernelData->bCPUConext)->Esi;
	
	tss.ebp = ((CONTEXT*)lpKernelData->bCPUConext)->Ebp;
	tss.eip = ((CONTEXT*)lpKernelData->bCPUConext)->Eip;
	tss.esp = ((CONTEXT*)lpKernelData->bCPUConext)->Esp;

	tss.ss = (WORD)((CONTEXT*)lpKernelData->bCPUConext)->SegSs;
	tss.cs = (WORD)((CONTEXT*)lpKernelData->bCPUConext)->SegCs;

    tss.gs = (WORD)((CONTEXT*)lpKernelData->bCPUConext)->SegGs;
    tss.fs = (WORD)((CONTEXT*)lpKernelData->bCPUConext)->SegFs;
    tss.es = (WORD)((CONTEXT*)lpKernelData->bCPUConext)->SegEs;
    tss.ds = (WORD)((CONTEXT*)lpKernelData->bCPUConext)->SegDs;


	tss.eflags = ((CONTEXT*)lpKernelData->bCPUConext)->EFlags;

	_ASSERT( tss.eip != (long)CPU_IRQTrap );

	{
		int * lpIntr = (int *)&lpKernelData->intrBits;
		int * lpMask = (int *)&lpKernelData->intrMask;
		
		_ASSERT( *lpIntr && *lpMask );

	}
	_ASSERT( lpKernelData->IntrEnable == 0 );
	//ReleaseKernelData();

    INTR_Interrupt();

	// here to restart the thread
	//
	//lpKernelData->IntrEnable = 0;
	INTR_OFF();
	//lpJumpContext = &context;
	//SetEvent( lpKernelData->hJumpEvent );
	//Sleep( INFINITE );

	JmpBack( &tss, &lpKernelData->IntrEnable );

	_ASSERT(0);
	while(1);

}
*/


//#define MEM_SIZE (1024*1024*16)
//extern LPBYTE  lpbSysMainMem;// = _mem;
//extern ULONG    ulSysMainMemLength;// = MEM_SIZE;
//int  __ebp;
//extern int fEnter;
BOOL bExitEsoft = 0;
extern HWND hwndDeskTop;


BOOL IsESOFTExit( void )
{
	return bExitEsoft;
}

extern void WINAPI KL_DebugOutString( LPTSTR );

//static DWORD WINAPI CPU_StartUpThread( LPVOID lParam )
//{
//	lpKernelData->intrMask.timer0 = 1;
//	return 0;
//}


int CPU_PowerOff( DWORD dwReserve )
{
//	MSG msg;
	
	lpKernelData->intrMask.timer0 = 0;
	lpKernelData->intrMask.mouse = 0;
	lpKernelData->intrMask.keyboard = 1;
	lpKernelData->intrBits.timer0 = 0; 
	lpKernelData->intrBits.mouse = 0;
	lpKernelData->intrBits.keyboard = 0;

	//lpKernelData->intrBits.
	bPowerOff = TRUE;	
	bPowerKeyDown = TRUE;
	SuspendThread( lpKernelData->hKingmosThread );  //wait to be wakeup
	bPowerOff = FALSE;
	lpKernelData->intrMask.timer0 = 1;
	lpKernelData->intrMask.mouse = 1;
	return 0;
}

void CPU_Reset( void )
{
    PostMessage( hwndDeskTop, WM_CLOSE, 0, 0 );	
    bExitEsoft = 1;
	SetEvent( lpKernelData->hHardwareEvent );
}

WINBASEAPI
BOOL
WINAPI
IsDebuggerPresent(
    VOID
    );
/*
DWORD WINAPI CPU_InterruptControlerThread( LPVOID lParam )
{
	int * lpIntr = (int *)&lpKernelData->intrBits;
	int * lpMask = (int *)&lpKernelData->intrMask;
	int intrCount = 0;
	//HANDLE hEvent;//s[2] ;

	//if( IsDebuggerPresent() )
	//{			
	//}
	//hEvents[0] = lpKernelData->hHardwareEvent;
	//hEvents[1] = lpKernelData->hJumpEvent;
	while( bExitEsoft == 0 )
	{
		int retv;
		UINT uiSuspendCount;
		retv = WaitForSingleObject( lpKernelData->hHardwareEvent, INFINITE );

		//retv = WaitForMultipleObjects( 2, hEvents, FALSE, INFINITE );

		if( bExitEsoft )
			break;

		if( lpKernelData->IntrEnable == FALSE )
		{			
			intrCount++;
			if( intrCount > 1000 )
			{
				KL_DebugOutString( "intrCount==1000.\r\n" );
				intrCount = 0;
			}
		}

#ifdef KINGMOS_DEMO
	    if( IsDebuggerPresent() )
		    break;
#endif
	    //while( IsDebuggerPresent() )
		//{
			//OutputDebugString( "debuger...\r\n" );
		    //Sleep(1000);
		//}
		//while( SuspendThread( lpKernelData->hRTCControllerThread ) == 0xFFFFFFFF )
			//Sleep(1);

		while( (uiSuspendCount = SuspendThread( lpKernelData->hKingmosThread )) == 0xFFFFFFFF )
			Sleep(1);

		//while( *lpMask & *lpIntr )
		_ASSERT( bPowerOff || uiSuspendCount == 0 );
		//if( retv == WAIT_TIMEOUT )
		//{
		    //lpKernelData->intrBits.timer0 = 1;
            //lpKernelData->IntrMsg[ID_INTR_TIMER0].message = WM_TIMER;// = *lpmsg;
		//}
//		else
		//{
			//CPU_CheckIRQEvent();
		//}

		if( (*lpMask & *lpIntr) && lpKernelData->IntrEnable )
		{
			BOOL bRetv;
			if( bPowerOff )
			{	// 触发了一个唤醒中断事件
				ResumeThread( lpKernelData->hKingmosThread );
				bPowerOff = FALSE;
			}

			// ok, the thread suspended
			((CONTEXT*)lpKernelData->bCPUConext)->ContextFlags = CONTEXT_FULL;
			bRetv = GetThreadContext( lpKernelData->hKingmosThread, (CONTEXT*)&lpKernelData->bCPUConext );
			_ASSERT( bRetv );
			_ASSERT( ((CONTEXT*)lpKernelData->bCPUConext)->Eip != (UINT)CPU_IRQTrap );
			if( !( ((CONTEXT*)lpKernelData->bCPUConext)->Eip >= (DWORD)lpKernelData->uiLockSegStart &&
				((CONTEXT*)lpKernelData->bCPUConext)->Eip < (DWORD)lpKernelData->uiLockSegEnd ) )
			{
				CONTEXT newct = *( (CONTEXT*)&lpKernelData->bCPUConext );//&lpKernelData->ctSave;
				
				lpKernelData->IntrEnable = FALSE;
				newct.Eip = (UINT)CPU_IRQTrap;
				bRetv = SetThreadContext( lpKernelData->hKingmosThread, &newct );
				_ASSERT( bRetv );
			}
		}
		ResumeThread( lpKernelData->hKingmosThread );
		//ResumeThread( lpKernelData->hRTCControllerThread );
		//Sleep(1);
	}
	return 0;
}
*/

VOID CPU_SetRTCTime( DWORD dwSeconds )
{
	lpKernelData->lRTCCount = (int)(dwSeconds * 1000); //to 毫秒
}

int CALLBACK CPU_RTCControllerThread( LPVOID lpv )
{
	MSG msgTimer0;
	MSG msgRTC;
	lpKernelData->lRTCCount = MAXLONG; //max value
	msgRTC.message = WM_RTCMSG;
	msgTimer0.message = WM_TIMERMSG;
	while( 1 )
	{
		Sleep(RESCHED_PERIOD);
		lpKernelData->lRTCCount -= RESCHED_PERIOD;
		if( lpKernelData->lRTCCount <= 0 )
		{
			CPU_SetIRQEvent( &msgRTC );
			lpKernelData->lRTCCount = MAXLONG;
		}
		else
		{
			CPU_SetIRQEvent( &msgTimer0 );
		}		
	}
}

void CPU_Init( void )
{
	DWORD dwId;
	DWORD dwStartAdr = 0x70000000;

	while( lpKernelData == NULL )
	{
	    lpKernelData = (LPKERNEL_DATA)VirtualAlloc( (LPVOID)dwStartAdr, 1024 * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE | PAGE_NOCACHE );
		dwStartAdr -= 0x10000000;
	}
	if( lpKernelData )
	    memset( lpKernelData, 0, 1024 * 4 );
	else
		return;

    InitializeCriticalSection( &lpKernelData->csMsg );

    lpKernelData->uiLockSegStart = 0;//LockSeg_Start();
    lpKernelData->uiLockSegEnd = 0;//LockSeg_End();

    lpKernelData->hHardwareEvent = CreateEvent(0,FALSE,FALSE,0);
	lpKernelData->hJumpEvent = CreateEvent(0,FALSE,FALSE,0);
	// create main thread
	//lpKernelData->hKingmosThread = CreateThread( 0, MEM_SIZE + 1024 * 10, CPU_StartUpThread, 0, 0, &dwId );
	//lpKernelData->hIntrThread = CreateThread( 0, 0, CPU_InterruptControlerThread, 0, 0, &dwId );
//	SetThreadPriority(lpKernelData->hIntrThread, THREAD_PRIORITY_TIME_CRITICAL);
	//lpKernelData->hRTCControllerThread = CreateThread( 0, 0, CPU_RTCControllerThread, 0, 0, &dwId );
	
}

void CPU_Deinit( void )
{
	//CloseHandle( hResetEsoftEvent );
	TerminateThread( lpKernelData->hRTCControllerThread, 0 );
	CloseHandle( lpKernelData->hKingmosThread );
	CloseHandle( lpKernelData->hIntrThread );
	CloseHandle( lpKernelData->hHardwareEvent );
	
	VirtualFree( lpKernelData, 0, MEM_RELEASE );
	lpKernelData = NULL;
}
