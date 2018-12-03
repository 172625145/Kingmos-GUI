/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：GWME主入口，负责窗口，图形，事件的初始化 
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <bheap.h>
#include <edevice.h>
#include <eassert.h>
#include <eapisrv.h>
#include <epwin.h>
#include <gwmeobj.h>
#include <winsrv.h>
#include <gdisrv.h>
#include <touch.h>
#include "touchcal.h"


extern BOOL _InitialGdiObjMgr( void );
extern BOOL _InitialGdiIconCursorMgr( void );
extern void _DeInitialGdiIconCursorMgr( void );
extern void _DeInitialGdiObjMgr( void );

extern BOOL DeleteGdiObject( HANDLE hOwner );
extern BOOL DeleteDCObject( HANDLE hOwner );
extern void FreeInstanceCursor( HANDLE hOwner );
extern void FreeInstanceIcon( HANDLE hOwner );
extern BOOL _DeInitWindowManager( void );
extern void _DeInitialWndClassMgr( void );
extern void _DeInitialPaintRgn( void );
extern void _DeInitialRgn( void );
extern BOOL __InitialEventDriver(void);
extern BOOL _InitialRgn( void );
extern BOOL _InitiFont( void );
extern BOOL _InitScreen( void );
extern BOOL _ExitGraphic( void );
extern void _ReleaseEventDriver(void);
extern BOOL _InitStockObject( void );
extern BOOL _InitSysSetting( void );
extern BOOL _InitialGdi( void );
extern BOOL _InitialWndClassMgr( void );
extern BOOL _InitialPaintRgn( void );
extern BOOL _InitWindowManager( HINSTANCE hInst );
extern BOOL InstallMouseDevice( void );
extern BOOL _InitSysQueue( void );
extern BOOL _InitCaret( void );
extern void _DeInitialCaret( void );
extern void _DeInitialGdi( void );
extern void _DeInitSysColorBrush( void );
extern void _DeinitCalibration( void );
extern void _InitCalibration( void );
extern BOOL _InitGwmeTaskList( void );

extern void InstallGwmeServer( void );

extern VOID FASTCALL _ClearProcessTimer( HANDLE hProcess );

//定义系统资源
/*
HANDLE hbmpClose; //标题栏上的关闭方框
HANDLE hbmpHelp; //标题栏上的帮助方框
HANDLE hbmpOk;	//标题栏上的确认方框
HANDLE hbmpGrayClose; //标题栏上的灰关闭方框
HANDLE hbmpGrayHelp;	//标题栏上的灰帮助方框
HANDLE hbmpGrayOk;//标题栏上的灰确认方框
HICON hicoSysMenu;//标题栏上的系统方框
HICON hicoGraySysMenu;//标题栏上的灰系统方框

*/
//
HANDLE hgwmeBlockHeap;//块堆
HANDLE hgwmeEvent = NULL;
HINSTANCE hgwmeInstance = NULL;
HANDLE hgwmeProcess = NULL;

//定义默认的显示面
_BITMAPDATA * lpDisplayBitmap;
HBITMAP hbmpDisplayBitmap;			// 设备所用的显示面
HBITMAP hbmpCurrentFrameBuffer;    //  当前所用的显示面

ULONG   ufNeedFlushGDICount = 0;        // 当前所用的显示面已经被写入数据，假如可能，需要做flush

//定义默认的显示驱动程序
_DISPLAYDRV * lpDrvDisplayDefault;
_DISPLAYDRV * lpDrvDisplayDevice;

//定义默认的图形显示驱动界面入口-显示设备
_LPGWDI_ENTER lpGwdiDisplayEnter = NULL;

//定义默认的图形显示驱动界面入口-点设备
// pos driver , like mouse, touch...
_LPGWDI_ENTER lpGwdiPosEnter = NULL;

//定义默认的图形显示驱动界面入口-按键设备
// key driver
_LPGWDI_ENTER lpGwdiKeyEnter = NULL;
//

DWORD dwActiveTimerCount;	//最新的输入事件时间，被KEY Event and Mouse Event update
// **************************************************
// 声明：static BOOL InitGwmeBlockHeap( void )
// 参数：
// 	无
// 返回值：
//	进入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化块堆
// 引用: 
//	
// ************************************************
#define DEBUG_InitGwmeBlockHeap 0
static BOOL InitGwmeBlockHeap( void )
{
	UINT blockSize[] = { sizeof( _REGCLASS ), sizeof(_PROCESS_CLASS),
		                 sizeof(_WINDATA), sizeof(_MSGQUEUE), sizeof(GWEDATA), sizeof(_RECTNODE), 
						 sizeof(_RGNDATA), sizeof(_BITMAPDATA), sizeof(_BITMAP_DIB), sizeof(_FONT),
						 sizeof(_ICONDATA), sizeof(_BRUSHDATA), sizeof(_PENDATA), sizeof(_LINEDATA),
						 sizeof(_PIXELDATA), sizeof(_BLKBITBLT), sizeof(_DISPLAYDRV), sizeof(_FONTDRV),
						 128, 256, 512, 1024, 1024+128, 1024+256, 1024+512, 2048, 4096                     // for 可变的字串和位图准备 
	};

	hgwmeBlockHeap = BlockHeap_Create( blockSize, sizeof( blockSize ) / sizeof( UINT ) );
	if( hgwmeBlockHeap == NULL )
	{
		ERRORMSG( DEBUG_InitGwmeBlockHeap, ( "error in InitGwmeBlockHeap: __Init GWME BlockHeap failure!.\r\n" ) );
		
		return FALSE;
	}
	return TRUE;
}

// **************************************************
// 声明：static BOOL InstallGwmeObject( HINSTANCE hInst )
// 参数：
// 	IN hInst - 实例句柄
// 返回值：
//	进入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化图形对象管理器
// 引用: 
//	
// ************************************************
#define DEBUG_InstallGwmeObject 0
static BOOL InstallGwmeObject( HINSTANCE hInst )
{
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitialGdiObjMgr.\r\n" ) );
	if( _InitialGdiObjMgr() == FALSE )
		return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitialGdiIconCursorMgr.\r\n" ) );
	if( _InitialGdiIconCursorMgr() == FALSE )
		return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitStockObject\r\n" ) );
    if( _InitStockObject() == FALSE )
        return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitialGdi\r\n" ) );
    if( _InitialGdi() == FALSE )
        return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitSysSetting\r\n" ) );
	if( _InitSysSetting() == FALSE )
		return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "__InitialRgn\r\n" ) );
    if( _InitialRgn() == FALSE )
        return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitialPaintRgn\r\n" ) );
	if( _InitialPaintRgn() == FALSE )
		return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitGwmeTaskList\r\n" ) );
	if( _InitGwmeTaskList() == FALSE )
		return FALSE;

	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitialWndClassMgr\r\n" ) );
	if( _InitialWndClassMgr() == FALSE )
		return FALSE;
    //if( _InitialSystemWindowClass(hInst) == FALSE )
        //return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitWindowManager.\r\n" ) );
    if( _InitWindowManager( hInst ) == FALSE )
        return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitSysQueue.\r\n" ) );
    if( _InitSysQueue() == FALSE )
        return FALSE;
	DEBUGMSG( DEBUG_InstallGwmeObject, ( "_InitCaret.\r\n" ) );
    if( _InitCaret() == FALSE )
        return FALSE;
    DEBUGMSG( DEBUG_InstallGwmeObject, ( "InstallGWMEObject Finish\r\n" ) );
    return TRUE;
}

// **************************************************
// 声明：static BOOL InstallGwmeDevice( HINSTANCE hInst )
// 参数：
// 	IN hInst - 实例句柄
// 返回值：
//	进入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化图形设备界面
// 引用: 
//	
// ************************************************

extern BOOL CALLBACK HandlePosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo );
#define DEBUG_InstallGwmeDevice 0
static BOOL InstallGwmeDevice( HINSTANCE hInst )
{
	_InitDefaultGwdi(); //调用arch层的初始化函数

	if( lpGwdiDisplayEnter )	//是否有默认的显示设备 ？
	{	//是
	    DEBUGMSG( DEBUG_InstallGwmeDevice, ( "Init Display...\r\n" ) );
		if( lpGwdiDisplayEnter( GWDI_CREATE, 0, 0 ) )	//初始化成功？
		{	//是
			//得到默认的显示面			
		    lpGwdiDisplayEnter( GWDI_GET_BITMAP_PTR, 0, &lpDisplayBitmap );
			if( lpDisplayBitmap )
			    hbmpCurrentFrameBuffer = hbmpDisplayBitmap = PTR_TO_HANDLE(lpDisplayBitmap);
			else
				hbmpCurrentFrameBuffer = hbmpDisplayBitmap = NULL;

		    //得到默认的显示驱动程序
		    lpGwdiDisplayEnter( GWDI_GET_DISPLAY_PTR, 0, &lpDrvDisplayDevice );
			lpDrvDisplayDefault = lpDrvDisplayDevice;
		}
		else
			return FALSE;
	}

	if( lpGwdiPosEnter )	//是否有默认的点定位设备 ？
	{	//有
		DEBUGMSG( DEBUG_InstallGwmeDevice, ("Init Pos device...\r\n" ) );
		if( !lpGwdiPosEnter( GWDI_CREATE, 0, (LPVOID)HandlePosEvent ) )
			return FALSE;
	}

	if( lpGwdiKeyEnter )	//是否有默认的键盘设备 ？
	{	//是
		DEBUGMSG( DEBUG_InstallGwmeDevice, ( "Init key device...\r\n" ) );
		if( !lpGwdiKeyEnter( GWDI_CREATE, 0, 0 ) )
			return FALSE;
	}
	return TRUE;
}

// **************************************************
// 声明：BOOL OpenGwmeDevice( void )
// 参数：
// 	无
// 返回值：
//	进入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	打开图形设备
// 引用: 
//	
// ************************************************
#define DEBUG_OpenGwmeDevice 0
BOOL OpenGwmeDevice( void )
{
	DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice entry.\r\n" ) );	
	DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice open dislplay device.\r\n" ) );
	if( lpGwdiDisplayEnter )	//是否有默认的点定位设备 ？
	    lpGwdiDisplayEnter( GWDI_OPEN, 0, 0 ); // 打开
	DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice open pos device.\r\n" ) );
	if( lpGwdiPosEnter )	//是否有默认的点定位设备 ？
		lpGwdiPosEnter( GWDI_OPEN, 0, 0 );
    DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice open key device.\r\n" ) );
	if( lpGwdiKeyEnter )	//是否有默认的键盘设备 ？
		lpGwdiKeyEnter( GWDI_OPEN, 0, 0 );//打开
	DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice leave.\r\n" ) );		
	return TRUE;
}


// **************************************************
// 声明：static void ReleaseGwme( void )
// 参数：
//    无
// 返回值：
//	无
// 功能描述：
//	卸下gwme	
// 引用: 
//	
// ************************************************

static void ReleaseGwme( void )
{
	_DeinitCalibration();
	_DeInitialCaret();
	_DeInitialGdi();
	_DeInitSysColorBrush();
	_DeInitialRgn();
	_DeInitialPaintRgn();
	_DeInitialWndClassMgr();
    _DeInitWindowManager();

	if( lpGwdiDisplayEnter )
	{
		lpGwdiDisplayEnter( GWDI_CLOSE, 0, 0 );
		lpGwdiDisplayEnter( GWDI_DESTROY, 0, 0 );
	}	
	if( lpGwdiPosEnter )
	{
		lpGwdiPosEnter( GWDI_CLOSE, 0, 0 );
		lpGwdiPosEnter( GWDI_DESTROY, 0, 0 );
	}
	if( lpGwdiKeyEnter )
	{
		lpGwdiKeyEnter( GWDI_CLOSE, 0, 0 );
		lpGwdiKeyEnter( GWDI_DESTROY, 0, 0 );
	}
	_DeInitialGdiIconCursorMgr();
	_DeInitialGdiObjMgr();
}

// **************************************************
// 声明：void WINAPI WinSys_CloseObject( HANDLE hProcess )
// 参数：
//    IN hProcess - 进程句柄
// 返回值：
//	无
// 功能描述：
//	释放所有属于进程hProcess的资源
// 引用: 
//	
// ************************************************

void WINAPI WinSys_CloseObject( HANDLE hProcess )
{
	if( !API_IsReady( API_GWE ) )
		return;

	DeleteDCObject( hProcess );
	FreeInstanceIcon( hProcess );
	FreeInstanceCursor( hProcess );
	DeleteGdiObject( hProcess );
	//FreeInstanceMenu( hProcess );
}

static HWND hWndCalibrate = NULL;
static VOID CALLBACK SetCalibrationXY(int x, int y)
{ 
	if( !hWndCalibrate )
		return;
	RETAILMSG( 1, ( "SetCalibrationXY:x=%d,y=%d.\r\n", x, y ) );

	PostMessage( hWndCalibrate, WM_LBUTTONDOWN, 0, MAKELONG( x, y ) );
}

BOOL WINAPI WinSys_SetCalibrateWindow( HWND hWnd )
{
	if( hWnd )
	{
		if( lpGwdiPosEnter )
		{
		    lpGwdiPosEnter( GWDI_CALIBRATION, CALIBRATION_DOWN, SetCalibrationXY );
		    hWndCalibrate = hWnd;
		}
	}
	else
	{
		hWndCalibrate = NULL; //clear cal state		
		if( lpGwdiPosEnter )
		    lpGwdiPosEnter( GWDI_CALIBRATION, CALIBRATION_INACIVE, 0 );  //完成		
	}
	return TRUE;			
}

BOOL WINAPI WinSys_SetCalibratePoints( int cCalibrationPoints, int *pScreenXBuffer,int *pScreenYBuffer, int *pUncalXBuffer, int *pUncalYBuffer )
{
	return TouchPanelSetCalibration( cCalibrationPoints, pScreenXBuffer, pScreenYBuffer, pUncalXBuffer, pUncalYBuffer );
}

// **************************************************
// 声明：static BOOL CALLBACK DoEnumAPMQUERYSUSPEND( HWND hWnd, LPARAM lParam )
// 参数：
//    IN hWnd - 窗口句柄
//    IN lParam - 附带的用户参数
// 返回值：
//	如果继续，返回TRUE;否则，返回FALSE
// 功能描述：
//	枚举通知功能
// 引用: 
//	
// ************************************************
#define DEBUG_DoEnumAPMQUERYSUSPEND 0
static BOOL CALLBACK DoEnumAPMQUERYSUSPEND( HWND hWnd, LPARAM lParam )
{
	LRESULT lret;
	
	SendMessageTimeout( hWnd, WM_POWERBROADCAST, PBT_APMQUERYSUSPEND, lParam, SMTO_NORMAL, 10000, &lret );
	if( lret == BROADCAST_QUERY_DENY )
	{
		WARNMSG( DEBUG_DoEnumAPMQUERYSUSPEND, ( "hWnd(0x%x) return deny.\r\n", hWnd ) );
		return FALSE;
	}
	return TRUE;
}


// **************************************************
// 声明：static BOOL CALLBACK DoEnumAPMSUSPEND( HWND hWnd, LPARAM lParam )
// 参数：
//    IN hWnd - 窗口句柄
//    IN lParam - 附带的用户参数
// 返回值：
//	如果继续，返回TRUE;否则，返回FALSE
// 功能描述：
//	枚举挂起功能
// 引用: 
//	
// ************************************************

static BOOL CALLBACK DoEnumAPMSUSPEND( HWND hWnd, LPARAM lParam )
{
	LRESULT lret;
	SendMessageTimeout( hWnd, WM_POWERBROADCAST, PBT_APMSUSPEND, 0, SMTO_NORMAL, 10000, &lret );
	return TRUE;
}


// **************************************************
// 声明：static BOOL CALLBACK DoPostAPMQUERYSUSPENDFAILED(VOID)
// 参数：
//    IN hWnd - 窗口句柄
//    IN lParam - 附带的用户参数
// 返回值：
//	如果继续，返回TRUE;否则，返回FALSE
// 功能描述：
//	通知所有窗口挂起失败
// 引用: 
//	
// ************************************************

static BOOL CALLBACK DoPostAPMQUERYSUSPENDFAILED(VOID)
{
//	LRESULT lret;
	
	PostMessage( HWND_BROADCAST, WM_POWERBROADCAST, PBT_APMQUERYSUSPENDFAILED, 0 );
	return TRUE;
}

// **************************************************
// 声明：static BOOL CALLBACK DoPostAPMRESUMESUSPEND(VOID)
// 参数：
//    IN hWnd - 窗口句柄
//    IN lParam - 附带的用户参数
// 返回值：
//	如果继续，返回TRUE;否则，返回FALSE
// 功能描述：
//	从挂起中返回，恢复所有窗口
// 引用: 
//	
// ************************************************

static BOOL CALLBACK DoPostAPMRESUMESUSPEND(VOID)
{
	PostMessage( HWND_BROADCAST, WM_POWERBROADCAST, PBT_APMRESUMESUSPEND, 0 );
	return TRUE;
}

#define DEBUG_DoPower 1
static DWORD DoPower( HANDLE hServer, DWORD dwParam, LPVOID lpParam )
{
	DWORD dwRetv = 0;
	DEBUGMSG( DEBUG_DoPower, ( "GWME-DoPower entry.\r\n" ) );

	if( dwParam == SBP_APMQUERYSUSPEND )
	{	//查寻挂起许可
		if( DoEnumTopLevelWindow( DoEnumAPMQUERYSUSPEND, 0, NULL ) == FALSE )
		{	//许可挂起失败				
			DoPostAPMQUERYSUSPENDFAILED();
			dwRetv = FALSE;
		}
		else
		    dwRetv = TRUE;
	}
	else if( dwParam == SBP_APMSUSPEND )
	{	//通知挂起		
		DoEnumTopLevelWindow( DoEnumAPMSUSPEND, 0, NULL );
		if( lpGwdiDisplayEnter )
		{
			lpGwdiDisplayEnter( GWDI_POWEROFF, 0, 0 );
		}
		if( lpGwdiPosEnter )
		{
			lpGwdiPosEnter( GWDI_POWEROFF, 0, 0 );
		}
		if( lpGwdiKeyEnter )
		{
			lpGwdiKeyEnter( GWDI_POWEROFF, 0, 0 );
		}
	}
	else if( dwParam == SBP_APMQUERYSUSPENDFAILED )
	{	//系统挂起请求失败
		DoPostAPMQUERYSUSPENDFAILED();
	}
	else if( dwParam == SBP_APMRESUMESUSPEND )
	{	//挂起后的恢复
		if( lpGwdiDisplayEnter )
		{
			lpGwdiDisplayEnter( GWDI_POWERON, 0, 0 );
		}
		if( lpGwdiPosEnter )
		{
			lpGwdiPosEnter( GWDI_POWERON, 0, 0 );
		}
		if( lpGwdiKeyEnter )
		{
			lpGwdiKeyEnter( GWDI_POWERON, 0, 0 );		
		}
		DoPostAPMRESUMESUSPEND();
	}
	DEBUGMSG( DEBUG_DoPower, ( "GWME-DoPower leave,dwRetv(%d).\r\n",  dwRetv ) );
	return dwRetv;
}

// **************************************************
// 声明：DWORD WINAPI GwmeServerHandler( HANDLE hServer, DWORD dwIoControlCode , DWORD dwParam, LPVOID lpParam )
// 参数：
//    IN hServer - 服务句柄
//	IN dwIoControlCode - 控制码
//    IN dwParam - 第一个参数
//    IN lpParam - 第二个参数
// 返回值：
//	依赖与不同的控制
// 功能描述：
//	服务总线控制，处理各种系统服务，通知功能
// 引用: 
//	
// ************************************************
#define DEBUG_GwmeServerHandler 1
DWORD WINAPI GwmeServerHandler( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
{
	switch( dwServerCode )
	{
	case SCC_BROADCAST_THREAD_EXIT:
		Win_Clear(dwParam);
		break;
	case SCC_BROADCAST_PROCESS_EXIT:
		WinClass_Clear( (HANDLE)dwParam);
		WinSys_CloseObject( (HANDLE)dwParam );
		_ClearProcessTimer( (HANDLE)dwParam );
		break;
	case SCC_BROADCAST_POWER:
		return DoPower( hServer, dwParam, lpParam );
	case SCC_BROADCAST_SHUTDOWN:
		DoEnumTopLevelWindow( DoEnumAPMSUSPEND, 0, NULL );
		break;
	default:
		return Sys_DefServerProc( hServer, dwServerCode , dwParam, lpParam );
	}
	return 0;
}


// **************************************************
// 声明：int CALLBACK WinMain(
//                     HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPSTR     lpCmdLine,
//                     int       nCmdShow)
// 参数：
//    IN hInstance - 进程实例句柄
//	IN hPrevInstance - 前一个进程实例句柄（无用）
//    IN lpCmdLine - 命令行
//    IN nCmdShow - 怎样显示主窗口
// 返回值：
//	依赖与不同的控制
// 功能描述：
//	GWME进程总入口
// 引用: 
//	
// ************************************************
#define DEBUG_GWME 0
#ifdef INLINE_PROGRAM

int CALLBACK Gwme_WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)

#else


int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)

#endif
{
	HANDLE hActiveTimerEvent;
	DWORD dwCurTickCount;

	//防止重复加载
	if( API_IsReady( API_GWE ) )
	{
		DEBUGMSG( DEBUG_GWME, ( "the gwme has already load.exit!\r\n" ) );
		return -1;
	}
	hgwmeInstance = hInstance;
	DEBUGMSG( DEBUG_GWME, ( "gwme hInstance=%x.\r\n", hInstance ) );
	//得到当前进程真实句柄
	hgwmeProcess = OpenProcess( PROCESS_ALL_ACCESS, 0, GetCurrentProcessId() );//GetCurrentProcess();
	//需要文件系统，看是否准备好 ？
	while( API_IsReady( API_FILESYS ) == FALSE )
	{		
		Sleep(100);
	}
	//初始化块堆管理模块
	InitGwmeBlockHeap();
	//安装 GWME 服务器
	InstallGwmeServer();
	//初始化 GWME 设备，例如：mouse, displayer keyboard..
	InstallGwmeDevice( hInstance );
	//初始化 GWME 图形对象
	InstallGwmeObject( hInstance );
	//安装 GWME 系统资源
	//LoadGwmeResource( hInstance );
	//打开设备
	OpenGwmeDevice();
	_InitCalibration();

	_InitLayer();

	API_SetReady( API_GWE );
	
	DEBUGMSG( DEBUG_GWME, ( "GWE is ready.\r\n" ) );
	
	hgwmeEvent = CreateEvent( NULL, FALSE, FALSE, "GWME_EVENT" );
	hActiveTimerEvent = CreateEvent( NULL, FALSE, FALSE, "__GWME_ACTIVE_TIMER__" );
	dwActiveTimerCount = GetTickCount();

	while( 1 )
	{
		int rv;

		//等待事件
		//50是用来做WM_TIMER的处理的
		rv = WaitForSingleObject( hgwmeEvent, 50 );
		if( rv == WAIT_TIMEOUT )
		{   // 检查所有 timer 事件
			CheckWinTimerEvent();
		}
		else if( rv == WAIT_OBJECT_0 )
		{  // gwme event
			;//RETAILMSG( 1, ( "Gwme event\r\n." ) );
		}
		else  // if( rv == WAIT_OBJECT_0 + 1 )
		{
			;//break;
		}

		if( GetSYSQueueState() ) //系统队列是否有消息、事件 ？
		{	//有
			HandleSystemQueue();
		}
//		else
			//_AutoFlushGdi();
		//判断用户输入事件响应时间
		dwCurTickCount = GetTickCount();
		if( ( dwCurTickCount - dwActiveTimerCount ) >= 10000 )
		{
			dwActiveTimerCount = GetTickCount();
			SetEvent(hActiveTimerEvent);
		}
	}

	while(1)
		Sleep(10);

	CloseHandle( hgwmeEvent );
	ReleaseGwme();

	return 0;
}


