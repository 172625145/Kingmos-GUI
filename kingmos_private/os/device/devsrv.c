/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����豸�������
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
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

// ϵͳAPI���ýӿ�
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
// ϵͳAPI���ýӿڲ�������
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
// ϵͳAPI���ýӿ�
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
// ϵͳAPI���ýӿڲ�������
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

//static HANDLE hDeviceEvent; // �ػ��¼�

extern BOOL OEM_InitDefaultDevice( void );
extern int CALLBACK FileSys_Init( void );
extern void CALLBACK FileSys_Deinit( void );
extern BOOL _InitDeviceMgr( void );
extern void DeregisterAllDevice( void );
extern BOOL _DeinitDeviceMgr( void );
//extern void WINAPI PowerOffSystem( void );
extern BOOL DeinitRegistry( void );

// ********************************************************************
// ������static void InstallDeviceServer( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		ע���豸����API
// ����: 
//		
// ********************************************************************
static void InstallDeviceServer( void )
{	// ��ϵͳע���豸����API
    API_RegisterEx( API_DEV,  lpDeviceAPI, dwDeviceArgs, sizeof( lpDeviceAPI ) / sizeof(PFNVOID) );
	API_RegisterEx( API_DEV_FILE,  lpDeviceFileAPI, dwDeviceFileArgs, sizeof( lpDeviceFileAPI ) / sizeof(PFNVOID) );
	API_SetReady( API_DEV );
	API_SetReady( API_DEV_FILE );
}



// ********************************************************************
// ������void ShutDownDevice( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		�ػ��¼�����
// ����: 
//		�� Device_ServerHandler ����
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
// ������int CALLBACK Device_WinMain(HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPSTR     lpCmdLine,
//                     int       nCmdShow)
// ������
//		IN hInstance - ��ǰ����ʵ��
//		IN hPrevInstance - ���ã�ΪNULL,ǰһ����ʵ����
//		IN lpCmdLine - �����в���
//		IN nCmdShow - ������ʾ����
// ����ֵ��
//		����Ӧ�õľ��嶨�塣ͨ���޴��󣬷���0
// ����������
//		������ڵ�
// ����: 
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

	// �ȴ��������߳�û���κ����������
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


