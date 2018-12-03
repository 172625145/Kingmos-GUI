/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：触摸屏驱动程序
版本号：1.0.0
开发时期：2003-12-01
作者：AlexZeng
修改记录：
******************************************************/

#include <ewindows.h>
#include <touch.h>
#include <gwmeobj.h>

#define CAL_DELTA_RESET			20
#define CAL_HOLD_STEADY_TIME	1500

static LPPOS_CALLBACK lpPosCallBack;
static LPPOS_CALIBRATION_CALLBACK lpCalibrationCallBack;
static HANDLE hTouchPanelEvent;
static BOOL uiCalibrationState = 0;

//UINT idTouchIrq;
//UINT idTouchChangedIrq;
#define ABS( v ) ( (v) < 0 ? -(v) : (v) )

#ifdef USE_AVERAGE

#define MAX_SAVE 4

int GetAverage( int *lpX, int *lpY, UINT uiMouseFlag )
{
    static int xSave[MAX_SAVE], ySave[MAX_SAVE];
    static int iSaveCount = 0;
    static int iSavePos = 0;
    int xNew, yNew;
    
	if( uiMouseFlag & MOUSEEVENTF_LEFTDOWN )
	{
		xSave[iSavePos] = *lpX;
		ySave[iSavePos] = *lpY;				
		iSavePos++;
		if( iSavePos >= MAX_SAVE )
			iSavePos = 0;
		
		if( iSaveCount < MAX_SAVE )
			iSaveCount++;
		
		if( iSaveCount > 1 )
		{
			int p = iSavePos - 1;
			int n;
			xNew = yNew = 0;
			for( n = 0; n < iSaveCount; n++, p-- )
			{
				if( p < 0 )
					p = MAX_SAVE - 1;
				xNew += xSave[p];
				yNew += ySave[p];
			}
			*lpX = ( xNew / iSaveCount );
			*lpY = ( yNew / iSaveCount );
		}
	}
	else
	{   // 没有改变
		iSaveCount = 0;
	}
}

#endif

static DWORD WINAPI TouchPanelISR( LPVOID lParam )
{
	int xRaw, yRaw;
	DWORD dwFlags;
	DWORD dwCurrentDownState = 0;
	UINT uCalibrationSampleCount;
	int cx, cy;
	int xBase, yBase;
	DWORD dwBaseTick;
	DWORD dwSampleFlags;
	BOOL fSetBase, fGotSample;
	
//    SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_HIGHEST );

	while( 1 ){

		WaitForSingleObject( hTouchPanelEvent, INFINITE );

		TouchPddGetPoint( &xRaw, &yRaw, &dwSampleFlags );
//		RETAILMSG( 1, ( "xRaw=%d,yRaw=%d.\r\n", xRaw, yRaw ) );

		if( dwSampleFlags & SAMPLE_IGNORE ){
//			ignore++;
			continue;
		}
		if( dwSampleFlags & SAMPLE_VALID ){
			if ( dwCurrentDownState )
				dwSampleFlags |= SAMPLE_PREVIOUS_DOWN;
			else
				dwSampleFlags &= ~SAMPLE_PREVIOUS_DOWN;

			dwCurrentDownState = dwSampleFlags & SAMPLE_DOWN;
		} 

        if( uiCalibrationState ){
			//系统处于较正状态
			if( !(dwSampleFlags & SAMPLE_VALID) ) {
				 WARNMSG( 1, ( "invalid toucher panle sample.\r\n" ) );
 				continue;
			}
			if( SAMPLE_DOWN == ( dwSampleFlags & (SAMPLE_DOWN|SAMPLE_PREVIOUS_DOWN) ) ){
				//第一次下笔
				uiCalibrationState = CALIBRATION_DOWN;
				fSetBase = TRUE;
				uCalibrationSampleCount = 0;
				fGotSample = FALSE;
			}
			if ( (uiCalibrationState == CALIBRATION_DOWN) && !fGotSample ) {
				if ( dwSampleFlags & SAMPLE_DOWN ) {
					long xDelta, yDelta;

					uCalibrationSampleCount++;
					cx = xRaw;
					cy = yRaw;
					if ( fSetBase )	{
						xBase = cx;
						yBase = cy;
						dwBaseTick = GetTickCount();
						fSetBase = FALSE;
					}
					xDelta = cx - xBase;
					yDelta = cy - yBase;
					if ( (GetTickCount() - dwBaseTick) > CAL_HOLD_STEADY_TIME ) {
						fGotSample = TRUE;
					}
					else if ( ( ABS(xDelta) > CAL_DELTA_RESET ) ||
							  ( ABS(yDelta) > CAL_DELTA_RESET ) ) {
					    WARNMSG( 1, ( "ABS(xDelta)=%d,ABS(yDelta)=%d is large,again!.\r\n", ABS(xDelta), ABS(yDelta) ) );
						fSetBase = TRUE;
					}
				}
				else {
					//抬笔
					if ( uCalibrationSampleCount >= uMinCalCount ) {
						fGotSample = TRUE;
					}
					else {
						uiCalibrationState = CALIBRATION_WAITING;
					}
				}
    			if ( fGotSample ) {
					uiCalibrationState = CALIBRATION_VALID;
					lpCalibrationCallBack( cx, cy);
				}
			}
        } 
		else {
			//系统处于非较正状态
			UINT uiMouseFlag;

			if( dwSampleFlags & SAMPLE_DOWN ) {
				uiMouseFlag = MOUSEEVENTF_LEFTDOWN;
			}
			else {
				uiMouseFlag = MOUSEEVENTF_LEFTUP;
			}
			uiMouseFlag |= MOUSEEVENTF_ABSOLUTE;

#ifdef USE_AVERAGE
			GetAverage( &xRaw, &yRaw, uiMouseFlag );
#endif
			lpPosCallBack( uiMouseFlag, xRaw, yRaw, 0, 0 );
		}
	}
}

static BOOL TouchPanelCreate( LPPOS_CALLBACK lpCallBackfn )
{
    TouchPddCreate();
    
	lpPosCallBack = lpCallBackfn;

	hTouchPanelEvent = CreateEvent( NULL, NULL, NULL, NULL );

	INTR_Init( idTouchIrq, hTouchPanelEvent, 0, 0 );
	INTR_Init( idTouchChangedIrq, hTouchPanelEvent, 0, 0 );
	
    INTR_Disable( idTouchIrq );
    INTR_Disable( idTouchChangedIrq );

	CreateThread( NULL, 0, TouchPanelISR, 0, 0, 0 );	

    return TRUE;

}

static int TouchPanelPowerHandle( BOOL bPowerOff )
{
	TouchPddPowerHandle( bPowerOff );
	return 0;
}

static BOOL TouchPanelOpen( void )
{
	return TouchPddEnable();
}

static void TouchPanelClose( void )
{
	TouchPddDisable();
}

void TouchPanelDestroy( void )
{
    TouchPddDestroy();
}

DWORD CALLBACK GWDI_TouchPanelEnter( UINT msg, DWORD dwParam, LPVOID lParam )
{
    switch( msg )
    {
	case GWDI_CREATE:   //初始化
		return TouchPanelCreate( (LPPOS_CALLBACK)lParam );
	case GWDI_OPEN:
    	//初始化硬件
        return TouchPanelOpen();
	case GWDI_CLOSE:
		TouchPanelClose();
    	break;
    case GWDI_DESTROY:   //卸载该设备
        TouchPanelDestroy();//DoDestroy();
        break;
    case GWDI_POWEROFF:
        // 开关机处理
        TouchPanelPowerHandle(1);
		break;
    case GWDI_POWERON:
    // 开关机处理
        TouchPanelPowerHandle(0);
		break;
	case GWDI_CALIBRATION:
		uiCalibrationState = (UINT)dwParam;
		lpCalibrationCallBack =  (LPPOS_CALIBRATION_CALLBACK)lParam;
		break;		
    }
    return 0;
}
