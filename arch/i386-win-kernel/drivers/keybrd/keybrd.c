#include <ewindows.h>
//#include <edef.h>
//#include <isr.h>
#include <epintr.h>
#include <w32intr.h>
//#include <ekeydrv.h>
#include <w32cfg.h>
#include <oalintr.h>

#include <gwmeobj.h>
#include <cpu.h>
HANDLE hKeyEvent;

static DWORD CALLBACK Keyboard_ISR( LPVOID lp )//DWORD dwISRHandle )
{
	MSG msg;
    //LPMSG lpmsg = &msg;//&lpKernelData->IntrMsg[ID_INTR_KEYBOARD];
	DWORD dwCurDownTickCount = 0;
	UINT uiCurDownKey = 0;
	UINT uiFlag = KEYEVENTF_KEYUP;

//    KEYRECORD keyRecord;
    
    while( 1 )
	{		
		WaitForSingleObject( hKeyEvent, INFINITE );
		msg = lpKernelData->IntrMsg[ID_INTR_KEYBOARD];

		if( msg.wParam == VK_POWEROFF )
		{
			RETAILMSG( 1, ( "PowerOff....\r\n" ) );
			ExitSystem(EXS_SHUTDOWN,0);
			continue;
		}
		else if( msg.wParam == VK_F1 )
		{
			LoadApplication( "Tasker", NULL );
			continue;
		}
/*
		keyRecord.dwKeyValue = lpmsg->wParam;
		keyRecord.wCount = 1;
		if( lpmsg->message == WM_KEYDOWN )
			keyRecord.wState = KS_KEYDOWN;
		//!!!! Add By Jami Chen In 2001.06.29
		else if( lpmsg->message == WM_CHAR )
			keyRecord.wState = KS_CHAR;
		//!!!! Add ENd By Jami Chen In 2001.06.29
		else if( lpmsg->message == WM_KEYUP )
			keyRecord.wState = KS_KEYUP;
		else
		{
			INTR_Done( SYSINTR_KEYBOARD );
			continue;
		}

		__PutKeybrdEvent( &keyRecord );
*/
		//if( lpmsg->message == WM_KEYDOWN )
		//	uiFlag = KS_KEYDOWN;
		//!!!! Add By Jami Chen In 2001.06.29

		//if( lpmsg->message == WM_CHAR )
		//	uiFlag = KEYEVENTF_CHAR;
		if( msg.message == WM_KEYDOWN )
		{
			if( uiFlag == 0 && 
				msg.wParam == VK_ESCAPE && 
				uiCurDownKey == VK_ESCAPE )
			{	//之前是keydown
				RETAILMSG( 1, ( "curTick=0x%x, oldTick=0x%x.\r\n", GetTickCount(), dwCurDownTickCount ) );
				if( GetTickCount() - dwCurDownTickCount >= 3000 )
				{ // power off
					dwCurDownTickCount = GetTickCount();
					uiFlag = KEYEVENTF_KEYUP;
					INTR_Done( SYSINTR_KEYBOARD );
					Sys_Exit(EXS_POWEROFF,0);					
					continue;
				}
			}
			else
			{
				if( uiFlag != 0 )
					dwCurDownTickCount = GetTickCount(); //new down
				uiCurDownKey = msg.wParam;
				uiFlag = 0;//KEYEVENTF_CHAR;
			}
		}

		//!!!! Add ENd By Jami Chen In 2001.06.29
		else if( msg.message == WM_KEYUP )
			uiFlag = KEYEVENTF_KEYUP;

		keybd_event( (UCHAR)msg.wParam, 0, uiFlag, 0  );
		
		INTR_Done( SYSINTR_KEYBOARD );
	}
}


BOOL InstallKeyboardDevice( void )
{
//    return INTR_Initialize( ISR_KEYBOARD, Keyboard_ISR, 0, 
  //                           SYSINTR_KEYBOARD, 0, 0 );
	hKeyEvent = CreateEvent( NULL, NULL, NULL, NULL );
	//ISR_Register( ISR_TOUCH, TouchPanel_ISR, 0 );
	INTR_Init( SYSINTR_KEYBOARD, hKeyEvent, 0, 0 );

	CreateThread(NULL, 0, Keyboard_ISR, 0, 0, 0 );
	return TRUE;

}

DWORD CALLBACK GWDI_KeyEnter( UINT msg, DWORD dwParam, LPVOID lParam )
{
    switch( msg )
    {
    case GWDI_CREATE:   //初始化硬件
        return InstallKeyboardDevice();
    case GWDI_DESTROY:   //关掉显示设备
        return 1;
    //case GWDI_POWEROFF:
        // 开关机处理
      //  return DoPowerOff();
    //case GWDI_POWERON:
    // 开关机处理
      //  return DoPowerOn();
    }
    return 0;
}

