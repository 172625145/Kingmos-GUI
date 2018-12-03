#include <ewindows.h>
//#include <edef.h>
//#include <isr.h>
//#include <epintr.h>
#include <w32intr.h>
//#include <epos.h>
#include <w32cfg.h>
#include <oalintr.h>
//#include <edevice.h>
#include <gwmeobj.h>
#include <cpu.h>

static UINT uiCalibrationState=0;             //@globalvar int | CalibrationState | Flag which indicates the
//extern VOID CALLBACK CalibrationXY(int x, int y);
//extern VOID CALLBACK SetCalibrationXY(int x, int y);
static LPPOS_CALLBACK lpPosCallBack;
static LPPOS_CALIBRATION_CALLBACK lpCalibrationCallBack;
/*
BOOL TouchPanelSetCalibration(
    int   cCalibrationPoints,     //@PARM The number of calibration points
    int   *pScreenXBuffer,        //@PARM List of screen X coords displayed
    int   *pScreenYBuffer,        //@PARM List of screen Y coords displayed
    int   *pUncalXBuffer,         //@PARM List of X coords collected
    int   *pUncalYBuffer          //@PARM List of Y coords collected
    )
{
	return TRUE;
}
*/
HANDLE hMouseEvent = NULL;

static DWORD CALLBACK Mouse_ISR( LPVOID lParam )//DWORD dwISRHandle )
{
    LPMSG lpmsg = &lpKernelData->IntrMsg[ID_INTR_MOUSE];
    //POSRECORD record;
	int x, y;
	UINT flag;

    while( 1 )
	{
		WaitForSingleObject( hMouseEvent, INFINITE );
		x = (short)LOWORD( lpmsg->lParam ) - iDisplayOffsetX;
		y = (short)HIWORD( lpmsg->lParam ) -  iDisplayOffsetY;
		if( ( x >= 0 && x < iDisplayWidth ) &&
			( y >= 0 && y < iDisplayHeight ) )
		{
			if( lpmsg->message == WM_LBUTTONDOWN )
			{
				flag = MOUSEEVENTF_LEFTDOWN;
			}
			else if( lpmsg->message == WM_LBUTTONUP )
			{
				flag = MOUSEEVENTF_LEFTUP;
			}
			else if( lpmsg->message == WM_MOUSEMOVE )
			{
				flag = (MK_LBUTTON & lpmsg->wParam) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
			}
		}
		else
		{
			if( x < 0 )
				x = 0;
			else if( x >= iDisplayWidth )
				x = iDisplayWidth-1;
			if( y < 0  )
				y = 0;
			else if( y >= iDisplayHeight )
				y = iDisplayHeight - 1;
			flag = MOUSEEVENTF_LEFTUP;
		}
		if( uiCalibrationState && flag == MOUSEEVENTF_LEFTDOWN )
		{
			//SetCalibrationXY( x, y);
			lpCalibrationCallBack( x * 4, y * 4 );
		}
		else
		{
			flag |= MOUSEEVENTF_ABSOLUTE;
			//mouse.x = CalX/4; 
			//mouse.y = CalY/4;
			//__PutPointerEvent(&mouse); 
			//mouse_event( flag, x, y, 0, 0 );
			lpPosCallBack( flag, x * 4, y * 4, 0, 0 );
			//__PutPointerEvent( &record );
		}

		INTR_Done( SYSINTR_MOUSE );
	}
}
/*
DWORD CALLBACK __DevPointerEnter( int msg, DWORD lParam )
{
    switch( msg )
    {
    case DEV_CREATE:
        return 1;
    case DEV_DESTROY:
        return 0;
    }
    return 0;
}
*/



//BOOL INTR_Initialize( DWORD idISR, LPISR lpISRFun, DWORD dwISRData,
//					  DWORD idInt, LPVOID lpvData, DWORD dwData )

BOOL InstallMouseDevice( LPVOID lParam )
{
//    BOOL    ReturnValue;
    lpPosCallBack = (LPPOS_CALLBACK)lParam;
	hMouseEvent = CreateEvent( NULL, NULL, NULL, NULL );
	//ISR_Register( ISR_TOUCH, TouchPanel_ISR, 0 );
	INTR_Init( SYSINTR_MOUSE, hMouseEvent, 0, 0 );

	CreateThread(NULL, 0, Mouse_ISR, 0, 0, 0 );
	return TRUE;

//    return INTR_Initialize( ISR_MOUSE, Mouse_ISR, 0, 
  //                   SYSINTR_MOUSE, 0, 0 );                         
}


DWORD CALLBACK GWDI_MouseEnter( UINT msg, DWORD dwParam, LPVOID lParam )
{
    switch( msg )
    {
    case GWDI_CREATE:   //初始化硬件
        return InstallMouseDevice(lParam);
    case GWDI_DESTROY:   //关掉显示设备
        return 1;
    //case GWDI_POWEROFF:
        // 开关机处理
      //  return DoPowerOff();
    //case GWDI_POWERON:
    // 开关机处理
      //  return DoPowerOn();
	case GWDI_CALIBRATION:
		uiCalibrationState = (BOOL)dwParam;
		lpCalibrationCallBack =  (LPPOS_CALIBRATION_CALLBACK)lParam;
		break;		
 
    }
    return 0;
}





