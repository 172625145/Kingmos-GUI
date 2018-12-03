#include <ewindows.h>
#include <w32intr.h>
#include <oalintr.h>
//#include <isr.h>
#include <cpu.h>
#include <emlos.h>

BOOL OEM_InterruptEnable (
    DWORD idInt,       // @parm Interrupt ID to be enabled. See 
                       //  <l Interrupt ID's.Interrupt ID's>  for a list of possble values.
    LPVOID pvData,     // @parm ptr to data passed in in the <f InterruptInitialize> call
    DWORD cbData       // @parm Size of data pointed to be <p pvData>
)
{
    BOOL bRetv = TRUE; 

    INTR_OFF();
	
    switch (idInt) 
	{
	case SYSINTR_RESCHED:		
		lpKernelData->intrMask.timer0  = 1;
		break;
	case SYSINTR_KEYBOARD:
        //intrMask.mask[ID_INTR_KEYBOARD] = 1;// enable intr
		lpKernelData->intrMask.keyboard = 1;// enable intr
		break;
	case SYSINTR_MOUSE:
        lpKernelData->intrMask.mouse = 1;// enable intr
		//intrMask.mask[ID_INTR_MOUSE] = 1;// enable intr
		break;
	//case SYSINTR_TOUCH_CHANGED:
	//	break;
	case SYSINTR_RTC_ALARM:
		lpKernelData->intrMask.timer1  = 1;
		break;
    default:
		bRetv = FALSE; 
    }

    INTR_ON();
    return bRetv;
}


void OEM_InterruptDisable( DWORD idInt )
{
    INTR_OFF();

    switch (idInt) 
	{
	case SYSINTR_GSM:
		break;		
   	case SYSINTR_RTC_ALARM:
		lpKernelData->intrMask.timer1  = 0;
		break;		
	case SYSINTR_KEYBOARD:
        lpKernelData->intrMask.keyboard = 0;// disable intr
		//intrMask.mask[ID_INTR_KEYBOARD] = 1;// enable intr
		break;		
	case SYSINTR_MOUSE:
        lpKernelData->intrMask.mouse = 0;// disable intr
		//intrMask.mask[ID_INTR_MOUSE] = 1;// enable intr
		break;		
	//case SYSINTR_TOUCH_CHANGED:
	//	break;
	}
    INTR_ON();
}

void OEM_InterruptDone( DWORD idInt )
{
    INTR_OFF();

    switch (idInt) {
	case SYSINTR_RESCHED:
		break;		
    case SYSINTR_TIMER3:
		break;
	case SYSINTR_KEYBOARD:
        lpKernelData->intrMask.keyboard = 1;// enable intr
		//intrMask.mask[ID_INTR_KEYBOARD] = 1;// enable intr
		break;		
	case SYSINTR_MOUSE:
        lpKernelData->intrMask.mouse = 1;// enable intr
		//intrMask.mask[ID_INTR_MOUSE] = 1;// enable intr
		break;		
   	case SYSINTR_RTC_ALARM:
		lpKernelData->intrMask.timer1  = 1;
		break;		
	//case SYSINTR_TOUCH_CHANGED:
	//	break;
	}
    INTR_ON();
}

#define IDLE_COUNT_LIMITE 0
static UINT ulTickCount=0;
static DWORD dwSysIdleTickCount;
int OEM_InterruptHandler( int dumy )
{
    ///if( intrBits.intr[ID_INTR_TIMER0] )
    //else if( intrBits.intr[ID_INTR_MOUSE] )

	if( lpKernelData->intrBits.mouse )
    {
        lpKernelData->intrBits.mouse = 0; // clear intr state bit
		//intrBits.intr[ID_INTR_MOUSE] = 0; // clear intr state bit
        lpKernelData->intrMask.mouse = 0;// disable intr
		//intrMask.mask[ID_INTR_MOUSE] = 0; // disable intr

        //ISR_Active( ISR_MOUSE );
		//RETAILMSG( 1, ( "mouse.\r\n" ) );
		dwSysIdleTickCount = ulTickCount;
        return SYSINTR_MOUSE;
    }
    //else if( intrBits.intr[ID_INTR_KEYBOARD] )
	else if( lpKernelData->intrBits.keyboard )
    {
        lpKernelData->intrBits.keyboard = 0; // clear intr state bit
		//intrBits.intr[ID_INTR_KEYBOARD] = 0;
        lpKernelData->intrMask.keyboard = 0;// disable intr
		//intrMask.mask[ID_INTR_KEYBOARD] = 0;

        //ISR_Active( ISR_KEYBOARD );
//		RETAILMSG( 1, ( "key.\r\n" ) );
		dwSysIdleTickCount = ulTickCount;
        return SYSINTR_KEYBOARD;
    }
	else if( lpKernelData->intrBits.timer0 )
    {
        lpKernelData->intrBits.timer0 = 0; // clear intr state bit
		//intrMask.mask[ID_INTR_KEYBOARD] = 1;// enable intr
		//intrBits.intr[ID_INTR_TIMER0] = 0; // clear intr state bit
        ulTickCount++;
		//if( (ulTickCount % 1000) == 0 )
		    //RETAILMSG( 1, ( "timer0=%d.\r\n", ulTickCount ) );
        return SYSINTR_RESCHED;
    }
    //else if( intrBits.intr[ID_INTR_TIMER1] )
	else if( lpKernelData->intrBits.timer1 )
    {
        lpKernelData->intrBits.timer1 = 0; // clear intr state bit
		//intrBits.intr[ID_INTR_TIMER1] = 0; // clear intr state bit
        //ulTickCount++;
        return SYSINTR_RTC_ALARM;
    }
	ASSERT(0);

	return SYSINTR_NOP;
}


#define IDLE_MODE 0
#define RUN_MODE  1
int OEM_EnterIdleMode( DWORD dwFlag )
{
	int bNewMode = RUN_MODE;
	int bCurrentMode = RUN_MODE;

	while( 1 )
	{
		bNewMode = RUN_MODE;
		if( ulTickCount >= dwSysIdleTickCount )
		{
			if( (ulTickCount - dwSysIdleTickCount) >= IDLE_COUNT_LIMITE )
			{
				bNewMode = IDLE_MODE;
			}
		}
		else
		{
			if( dwSysIdleTickCount + (~ulTickCount) >= IDLE_COUNT_LIMITE )
			{
				bNewMode = IDLE_MODE;
			}
		}
		if( bNewMode == IDLE_MODE )
		{
			if( bCurrentMode == RUN_MODE )
			{
				bCurrentMode = IDLE_MODE;
				;//RETAILMSG( 1, ( "Enter idle mode now!.\r\n" ) );
			}
			Win32_Sleep(1);   // enter idle mode now;			
		}
		else
		{   // enter run mode
			if( bCurrentMode == IDLE_MODE )
			{
			    ;//RETAILMSG( 1, ( "Enter run mode now!.\r\n" ) );
			}
			break;
		}
	}

	return 0;
}

int OEM_XXX( DWORD dwFlag )
{
	return 0;
}
