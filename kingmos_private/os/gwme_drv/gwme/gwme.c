/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����GWME����ڣ����𴰿ڣ�ͼ�Σ��¼��ĳ�ʼ�� 
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
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

//����ϵͳ��Դ
/*
HANDLE hbmpClose; //�������ϵĹرշ���
HANDLE hbmpHelp; //�������ϵİ�������
HANDLE hbmpOk;	//�������ϵ�ȷ�Ϸ���
HANDLE hbmpGrayClose; //�������ϵĻҹرշ���
HANDLE hbmpGrayHelp;	//�������ϵĻҰ�������
HANDLE hbmpGrayOk;//�������ϵĻ�ȷ�Ϸ���
HICON hicoSysMenu;//�������ϵ�ϵͳ����
HICON hicoGraySysMenu;//�������ϵĻ�ϵͳ����

*/
//
HANDLE hgwmeBlockHeap;//���
HANDLE hgwmeEvent = NULL;
HINSTANCE hgwmeInstance = NULL;
HANDLE hgwmeProcess = NULL;

//����Ĭ�ϵ���ʾ��
_BITMAPDATA * lpDisplayBitmap;
HBITMAP hbmpDisplayBitmap;			// �豸���õ���ʾ��
HBITMAP hbmpCurrentFrameBuffer;    //  ��ǰ���õ���ʾ��

ULONG   ufNeedFlushGDICount = 0;        // ��ǰ���õ���ʾ���Ѿ���д�����ݣ�������ܣ���Ҫ��flush

//����Ĭ�ϵ���ʾ��������
_DISPLAYDRV * lpDrvDisplayDefault;
_DISPLAYDRV * lpDrvDisplayDevice;

//����Ĭ�ϵ�ͼ����ʾ�����������-��ʾ�豸
_LPGWDI_ENTER lpGwdiDisplayEnter = NULL;

//����Ĭ�ϵ�ͼ����ʾ�����������-���豸
// pos driver , like mouse, touch...
_LPGWDI_ENTER lpGwdiPosEnter = NULL;

//����Ĭ�ϵ�ͼ����ʾ�����������-�����豸
// key driver
_LPGWDI_ENTER lpGwdiKeyEnter = NULL;
//

DWORD dwActiveTimerCount;	//���µ������¼�ʱ�䣬��KEY Event and Mouse Event update
// **************************************************
// ������static BOOL InitGwmeBlockHeap( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ�����
// ����: 
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
						 128, 256, 512, 1024, 1024+128, 1024+256, 1024+512, 2048, 4096                     // for �ɱ���ִ���λͼ׼�� 
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
// ������static BOOL InstallGwmeObject( HINSTANCE hInst )
// ������
// 	IN hInst - ʵ�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ��ͼ�ζ��������
// ����: 
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
// ������static BOOL InstallGwmeDevice( HINSTANCE hInst )
// ������
// 	IN hInst - ʵ�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ��ͼ���豸����
// ����: 
//	
// ************************************************

extern BOOL CALLBACK HandlePosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo );
#define DEBUG_InstallGwmeDevice 0
static BOOL InstallGwmeDevice( HINSTANCE hInst )
{
	_InitDefaultGwdi(); //����arch��ĳ�ʼ������

	if( lpGwdiDisplayEnter )	//�Ƿ���Ĭ�ϵ���ʾ�豸 ��
	{	//��
	    DEBUGMSG( DEBUG_InstallGwmeDevice, ( "Init Display...\r\n" ) );
		if( lpGwdiDisplayEnter( GWDI_CREATE, 0, 0 ) )	//��ʼ���ɹ���
		{	//��
			//�õ�Ĭ�ϵ���ʾ��			
		    lpGwdiDisplayEnter( GWDI_GET_BITMAP_PTR, 0, &lpDisplayBitmap );
			if( lpDisplayBitmap )
			    hbmpCurrentFrameBuffer = hbmpDisplayBitmap = PTR_TO_HANDLE(lpDisplayBitmap);
			else
				hbmpCurrentFrameBuffer = hbmpDisplayBitmap = NULL;

		    //�õ�Ĭ�ϵ���ʾ��������
		    lpGwdiDisplayEnter( GWDI_GET_DISPLAY_PTR, 0, &lpDrvDisplayDevice );
			lpDrvDisplayDefault = lpDrvDisplayDevice;
		}
		else
			return FALSE;
	}

	if( lpGwdiPosEnter )	//�Ƿ���Ĭ�ϵĵ㶨λ�豸 ��
	{	//��
		DEBUGMSG( DEBUG_InstallGwmeDevice, ("Init Pos device...\r\n" ) );
		if( !lpGwdiPosEnter( GWDI_CREATE, 0, (LPVOID)HandlePosEvent ) )
			return FALSE;
	}

	if( lpGwdiKeyEnter )	//�Ƿ���Ĭ�ϵļ����豸 ��
	{	//��
		DEBUGMSG( DEBUG_InstallGwmeDevice, ( "Init key device...\r\n" ) );
		if( !lpGwdiKeyEnter( GWDI_CREATE, 0, 0 ) )
			return FALSE;
	}
	return TRUE;
}

// **************************************************
// ������BOOL OpenGwmeDevice( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ͼ���豸
// ����: 
//	
// ************************************************
#define DEBUG_OpenGwmeDevice 0
BOOL OpenGwmeDevice( void )
{
	DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice entry.\r\n" ) );	
	DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice open dislplay device.\r\n" ) );
	if( lpGwdiDisplayEnter )	//�Ƿ���Ĭ�ϵĵ㶨λ�豸 ��
	    lpGwdiDisplayEnter( GWDI_OPEN, 0, 0 ); // ��
	DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice open pos device.\r\n" ) );
	if( lpGwdiPosEnter )	//�Ƿ���Ĭ�ϵĵ㶨λ�豸 ��
		lpGwdiPosEnter( GWDI_OPEN, 0, 0 );
    DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice open key device.\r\n" ) );
	if( lpGwdiKeyEnter )	//�Ƿ���Ĭ�ϵļ����豸 ��
		lpGwdiKeyEnter( GWDI_OPEN, 0, 0 );//��
	DEBUGMSG( DEBUG_OpenGwmeDevice, ( "OpenGwmeDevice leave.\r\n" ) );		
	return TRUE;
}


// **************************************************
// ������static void ReleaseGwme( void )
// ������
//    ��
// ����ֵ��
//	��
// ����������
//	ж��gwme	
// ����: 
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
// ������void WINAPI WinSys_CloseObject( HANDLE hProcess )
// ������
//    IN hProcess - ���̾��
// ����ֵ��
//	��
// ����������
//	�ͷ��������ڽ���hProcess����Դ
// ����: 
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
		    lpGwdiPosEnter( GWDI_CALIBRATION, CALIBRATION_INACIVE, 0 );  //���		
	}
	return TRUE;			
}

BOOL WINAPI WinSys_SetCalibratePoints( int cCalibrationPoints, int *pScreenXBuffer,int *pScreenYBuffer, int *pUncalXBuffer, int *pUncalYBuffer )
{
	return TouchPanelSetCalibration( cCalibrationPoints, pScreenXBuffer, pScreenYBuffer, pUncalXBuffer, pUncalYBuffer );
}

// **************************************************
// ������static BOOL CALLBACK DoEnumAPMQUERYSUSPEND( HWND hWnd, LPARAM lParam )
// ������
//    IN hWnd - ���ھ��
//    IN lParam - �������û�����
// ����ֵ��
//	�������������TRUE;���򣬷���FALSE
// ����������
//	ö��֪ͨ����
// ����: 
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
// ������static BOOL CALLBACK DoEnumAPMSUSPEND( HWND hWnd, LPARAM lParam )
// ������
//    IN hWnd - ���ھ��
//    IN lParam - �������û�����
// ����ֵ��
//	�������������TRUE;���򣬷���FALSE
// ����������
//	ö�ٹ�����
// ����: 
//	
// ************************************************

static BOOL CALLBACK DoEnumAPMSUSPEND( HWND hWnd, LPARAM lParam )
{
	LRESULT lret;
	SendMessageTimeout( hWnd, WM_POWERBROADCAST, PBT_APMSUSPEND, 0, SMTO_NORMAL, 10000, &lret );
	return TRUE;
}


// **************************************************
// ������static BOOL CALLBACK DoPostAPMQUERYSUSPENDFAILED(VOID)
// ������
//    IN hWnd - ���ھ��
//    IN lParam - �������û�����
// ����ֵ��
//	�������������TRUE;���򣬷���FALSE
// ����������
//	֪ͨ���д��ڹ���ʧ��
// ����: 
//	
// ************************************************

static BOOL CALLBACK DoPostAPMQUERYSUSPENDFAILED(VOID)
{
//	LRESULT lret;
	
	PostMessage( HWND_BROADCAST, WM_POWERBROADCAST, PBT_APMQUERYSUSPENDFAILED, 0 );
	return TRUE;
}

// **************************************************
// ������static BOOL CALLBACK DoPostAPMRESUMESUSPEND(VOID)
// ������
//    IN hWnd - ���ھ��
//    IN lParam - �������û�����
// ����ֵ��
//	�������������TRUE;���򣬷���FALSE
// ����������
//	�ӹ����з��أ��ָ����д���
// ����: 
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
	{	//��Ѱ�������
		if( DoEnumTopLevelWindow( DoEnumAPMQUERYSUSPEND, 0, NULL ) == FALSE )
		{	//��ɹ���ʧ��				
			DoPostAPMQUERYSUSPENDFAILED();
			dwRetv = FALSE;
		}
		else
		    dwRetv = TRUE;
	}
	else if( dwParam == SBP_APMSUSPEND )
	{	//֪ͨ����		
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
	{	//ϵͳ��������ʧ��
		DoPostAPMQUERYSUSPENDFAILED();
	}
	else if( dwParam == SBP_APMRESUMESUSPEND )
	{	//�����Ļָ�
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
// ������DWORD WINAPI GwmeServerHandler( HANDLE hServer, DWORD dwIoControlCode , DWORD dwParam, LPVOID lpParam )
// ������
//    IN hServer - ������
//	IN dwIoControlCode - ������
//    IN dwParam - ��һ������
//    IN lpParam - �ڶ�������
// ����ֵ��
//	�����벻ͬ�Ŀ���
// ����������
//	�������߿��ƣ��������ϵͳ����֪ͨ����
// ����: 
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
// ������int CALLBACK WinMain(
//                     HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPSTR     lpCmdLine,
//                     int       nCmdShow)
// ������
//    IN hInstance - ����ʵ�����
//	IN hPrevInstance - ǰһ������ʵ����������ã�
//    IN lpCmdLine - ������
//    IN nCmdShow - ������ʾ������
// ����ֵ��
//	�����벻ͬ�Ŀ���
// ����������
//	GWME���������
// ����: 
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

	//��ֹ�ظ�����
	if( API_IsReady( API_GWE ) )
	{
		DEBUGMSG( DEBUG_GWME, ( "the gwme has already load.exit!\r\n" ) );
		return -1;
	}
	hgwmeInstance = hInstance;
	DEBUGMSG( DEBUG_GWME, ( "gwme hInstance=%x.\r\n", hInstance ) );
	//�õ���ǰ������ʵ���
	hgwmeProcess = OpenProcess( PROCESS_ALL_ACCESS, 0, GetCurrentProcessId() );//GetCurrentProcess();
	//��Ҫ�ļ�ϵͳ�����Ƿ�׼���� ��
	while( API_IsReady( API_FILESYS ) == FALSE )
	{		
		Sleep(100);
	}
	//��ʼ����ѹ���ģ��
	InitGwmeBlockHeap();
	//��װ GWME ������
	InstallGwmeServer();
	//��ʼ�� GWME �豸�����磺mouse, displayer keyboard..
	InstallGwmeDevice( hInstance );
	//��ʼ�� GWME ͼ�ζ���
	InstallGwmeObject( hInstance );
	//��װ GWME ϵͳ��Դ
	//LoadGwmeResource( hInstance );
	//���豸
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

		//�ȴ��¼�
		//50��������WM_TIMER�Ĵ����
		rv = WaitForSingleObject( hgwmeEvent, 50 );
		if( rv == WAIT_TIMEOUT )
		{   // ������� timer �¼�
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

		if( GetSYSQueueState() ) //ϵͳ�����Ƿ�����Ϣ���¼� ��
		{	//��
			HandleSystemQueue();
		}
//		else
			//_AutoFlushGdi();
		//�ж��û������¼���Ӧʱ��
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


