#include <eframe.h>
#include <eassert.h>
#include <epcore.h>
#include <emlos.h>

extern void Switch( TSS* lpPrev, TSS* lpNext, volatile int * lpui );
void SwitchTo( LPTHREAD lpPrev, LPTHREAD lpNext )
{
	//extern volatile int lpKernelData->IntrEnable;
//    extern VOID Win32_Sleep( UINT dwTicks );
    //EdbgOutputDebugString("SwitchTo=%d\r\n", lpNext->dwThreadId );

//	Win32_Sleep(1);
    lpCurThread = lpNext;
    Switch( &lpPrev->tss, &lpNext->tss, &lpKernelData->IntrEnable );
}

