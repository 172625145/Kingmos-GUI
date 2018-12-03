/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：设备服务管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/
#include <eframe.h>
#include <efile.h>
#include <edevice.h>
#include <eapisrv.h>
#include <devdrv.h>
#include <devsrv.h>

static const SERVER_CTRL_INFO sci = { sizeof( SERVER_CTRL_INFO ),
                                      SCI_NOT_HANDLE_THREAD_EXIT | SCI_NOT_HANDLE_PROCESS_EXIT,
									  Device_ServerHandler };

// 系统API调用接口
static const PFNVOID lpDeviceAPI[] = {
	(PFNVOID)&sci,//Device_ServerHandler,
	(PFNVOID)Device_RegisterDevice,
	(PFNVOID)Device_Deregister,
	(PFNVOID)Device_RegisterFSD,
	(PFNVOID)Device_UnregisterFSD,
	(PFNVOID)Device_LoadFSD,
	(PFNVOID)Device_Enum,
	(PFNVOID)Device_UnloadFSD,
	(PFNVOID)Device_GetSystemPowerStatusEx
};
// 系统API调用接口参数类型
static const DWORD dwDeviceArgs[] = {
	0,
	ARG4_MAKE( PTR, DWORD, PTR, DWORD ),  //Device_RegisterDevice
	ARG1_MAKE( DWORD ),  //Device_Deregister
	ARG3_MAKE( PTR, DWORD, PTR ),  //Device_RegisterFSD
	ARG1_MAKE( PTR ),   //Device_UnregisterFSD
	ARG2_MAKE( DWORD, PTR ), //Device_LoadFSD,
	ARG2_MAKE( PTR, PTR ),
	ARG1_MAKE( DWORD ),		// Device_UnloadFSD
	ARG2_MAKE( PTR, DWORD ),//Device_GetSystemPowerStatusEx
};
// 系统API调用接口
static const PFNVOID lpDeviceFileAPI[] = 
{
	NULL,
    (PFNVOID)Device_CreateFile,
	(PFNVOID)Device_CloseFile,
    (PFNVOID)Device_ReadFile,
    (PFNVOID)Device_WriteFile,
    (PFNVOID)Device_GetFileSize,
    (PFNVOID)Device_SetFilePointer,
    (PFNVOID)Device_SetEndOfFile,
    (PFNVOID)Device_GetFileTime,
    (PFNVOID)Device_SetFileTime,
    (PFNVOID)Device_FlushFileBuffers,
    (PFNVOID)Device_GetFileInformationByHandle,
    (PFNVOID)Device_IoControl
};
// 系统API调用接口参数类型
static const DWORD dwDeviceFileArgs[] = 
{
	0,
	ARG4_MAKE( PTR, DWORD, DWORD, DWORD ),//Device_CreateFile
	ARG1_MAKE( DWORD ),//Device_CloseFile
	ARG5_MAKE( DWORD, PTR, DWORD, PTR, PTR ),//Device_ReadFile
    ARG5_MAKE( DWORD, PTR, DWORD, PTR, PTR ),//Device_WriteFile
    ARG2_MAKE( DWORD, PTR ),//Device_GetFileSize
	ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//Device_SetFilePointer
	ARG1_MAKE( DWORD ),//Device_SetEndOfFile
	ARG4_MAKE( DWORD, PTR, PTR, PTR ),//Device_GetFileTime
	ARG4_MAKE( DWORD, PTR, PTR, PTR ),//Device_SetFileTime
	ARG1_MAKE( DWORD ),//Device_FlushFileBuffers
	ARG2_MAKE( DWORD, PTR ),//Device_GetFileInformationByHandle
	ARG8_MAKE( DWORD, DWORD, PTR, DWORD, PTR, DWORD, PTR, PTR )//ARGDevice_IoControl
};

//static HANDLE hDeviceEvent; // 关机事件

extern BOOL OEM_InitDefaultDevice( void );
extern int CALLBACK FileSys_Init( void );
extern void CALLBACK FileSys_Deinit( void );
extern BOOL _InitDeviceMgr( void );
extern void DeregisterAllDevice( void );
extern BOOL _DeinitDeviceMgr( void );
//extern void WINAPI PowerOffSystem( void );
extern BOOL DeinitRegistry( void );

// ********************************************************************
// 声明：static void InstallDeviceServer( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		注册设备服务API
// 引用: 
//		
// ********************************************************************
static void InstallDeviceServer( void )
{	// 向系统注册设备管理API
    API_RegisterEx( API_DEV,  lpDeviceAPI, dwDeviceArgs, sizeof( lpDeviceAPI ) / sizeof(PFNVOID) );
	API_RegisterEx( API_DEV_FILE,  lpDeviceFileAPI, dwDeviceFileArgs, sizeof( lpDeviceFileAPI ) / sizeof(PFNVOID) );
	API_SetReady( API_DEV );
	API_SetReady( API_DEV_FILE );
}



// ********************************************************************
// 声明：void ShutDownDevice( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		关机事件处理
// 引用: 
//		被 Device_ServerHandler 调用
// ********************************************************************
#define DEBUG_SHUTDOWN 0
void ShutDownDevice( void )
{
	DEBUGMSG( DEBUG_SHUTDOWN, ( "Device WinMain: exit device now.\r\n" ) );

	DeinitRegistry();
#ifdef EML_WIN32
	{
		extern BOOL _RestoreESOFTFileSystemData( void );
//		extern BOOL _RestoreSRAMDiskData( void );
	    //_RestoreESOFTFileSystemData();
//	    _RestoreSRAMDiskData();
	}
#endif
	DEBUGMSG( DEBUG_SHUTDOWN, ( "Device WinMain: exit driver now.\r\n" ) );
	DeregisterAllDevice();	

	DEBUGMSG( DEBUG_SHUTDOWN, ( "Device WinMain: exit file sys now.\r\n" ) );
	FileSys_Deinit();

	//CloseHandle( hDeviceEvent );
	_DeinitDeviceMgr();

}

// ********************************************************************
// 声明：int CALLBACK Device_WinMain(HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPSTR     lpCmdLine,
//                     int       nCmdShow)
// 参数：
//		IN hInstance - 当前进程实例
//		IN hPrevInstance - 无用（为NULL,前一进程实例）
//		IN lpCmdLine - 命令行参数
//		IN nCmdShow - 窗口显示参数
// 返回值：
//		根据应用的具体定义。通常无错误，返回0
// 功能描述：
//		进程入口点
// 引用: 
//		
// ********************************************************************

#define DEBUG_DEVICE_WINMAIN 0
#ifdef INLINE_PROGRAM 
int CALLBACK Device_WinMain(HINSTANCE hInstance,
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

	int k = 10;
	DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "============enter device winmain============\r\n" ) );
//	DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "enter device winmain,%d,%s", 12345678, "hello, device" ) );	

	if( API_IsReady( API_DEV ) )
	{
		WARNMSG( 1, ( "the device application has already load!, now exit me.!\r\n" ) );
		return -1;
	}

    DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "Device WinMain: _InitDeviceMgr.\r\n" ) );
	_InitDeviceMgr();
	DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "Device WinMain: InstallDeviceServer.\r\n" ) );
    InstallDeviceServer();
    DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "Device WinMain: FileSys_Init.\r\n" ) );
	FileSys_Init();
	DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "Device WinMain: OEM_InitDefaultDevice.\r\n" ) );
	if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
	{
        OEM_InitDefaultDevice();
		Sys_ReleaseException();
	}
	DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "Device WinMain: OEM_InitDefaultDevice OK.\r\n" ) );	

#ifndef INLINE_PROGRAM
	_InitDllDevice();
#endif

	
//	hDeviceEvent = CreateEvent( NULL, FALSE, FALSE, "device_event" );
	//	ASSERT(hDeviceEvent);

	// 等待服务，主线程没有任何事情可以做
	DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "Device WinMain: entry INFINITE!!!!!!!!!.\r\n" ) );
	while( 1 )
	{
		//int rv;
		//rv = WaitForSingleObject( hDeviceEvent, INFINITE );
		Sleep( INFINITE );
		DEBUGMSG( DEBUG_DEVICE_WINMAIN, ( "Device WinMain: error!!!!!!!!!!.\r\n" ) );
		break;
	}
	

	return 0;
}


