#include <eframe.h>
#include <ecore.h>
#include <eassert.h>
#include "oalintr.h"

#include "oemfunc.h"
#include "epcore.h"

//int InterruptEnable = 0;


void INTR_Software( void )
{
	EdbgOutputDebugString("Software int\r\n");
	while(1);
}

void INTR_PrefetchAbort( void )
{
	EdbgOutputDebugString("Prefetch Abort\r\n");
	while(1);
}

void INTR_DataAbort( void )
{
	EdbgOutputDebugString("Data Abort\r\n");
	while(1);
}

extern BOOL bNeedResched;
extern int iISREntry;

void INTR_Interrupt(void)
{
	int id;
    extern void DoTimer( void * );
    //extern BOOL bNeedResched;
	id = OEM_InterruptHandler(0);
    switch ( id ) 
    {
    case SYSINTR_RESCHED:
        DoTimer( 0 );
        break;
    case SYSINTR_NOP:
		goto _error;
	default:
		ISR_Active( id );
		break;
    }
	
	if( iISRActiveCount )
    {   // if enable, to do isr
        ISR_Handler( id );
    }
#ifdef TIMER_RESCHE
	//if( bNeedResched && iISREntry == 0 && lpCurThread->nLockScheCount == 0 )
	if( bNeedResched && lpCurThread->nLockScheCount == 0 )

	//if( id == SYSINTR_RESCHED )
	
	{
		Schedule();
	}
#endif

_error:
	;
}

