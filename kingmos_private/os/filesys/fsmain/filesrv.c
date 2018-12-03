/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：文件系统管理模块-服务注册部分
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <efile.h>
#include <eapisrv.h>
#include <efsdmgr.h>
#include <filesrv.h>

#define FILESRV_ZONE 0
extern void InitRegistry( void );

///////////////////////////////////////////////////
// 文件系统API功能集
static const PFNVOID lpFileSysAPI[] = 
{
	NULL,
    (PFNVOID)FileSys_CreateDirectory,
    (PFNVOID)FileSys_CreateFile,
	(PFNVOID)FileSys_CloseFile,
    (PFNVOID)FileSys_DeleteAndRename,
    (PFNVOID)FileSys_Delete,
    (PFNVOID)FileSys_DeviceIoControl,
    (PFNVOID)FileSys_FindClose,
    (PFNVOID)FileSys_FindFirst,
    (PFNVOID)FileSys_FindNext,
    (PFNVOID)FileSys_FlushBuffers,
    (PFNVOID)FileSys_GetDiskFreeSpaceEx,
    (PFNVOID)FileSys_GetAttributes,
    (PFNVOID)FileSys_GetInformation,
    (PFNVOID)FileSys_GetSize,
    (PFNVOID)FileSys_GetTime,
    (PFNVOID)FileSys_Move,
    (PFNVOID)FileSys_Read,
    (PFNVOID)FileSys_ReadWithSeek,
    (PFNVOID)FileSys_RemoveDirectory,
    (PFNVOID)FileSys_SetEnd,
    (PFNVOID)FileSys_SetAttributes,
    (PFNVOID)FileSys_SetPointer,
    (PFNVOID)FileSys_SetTime,
    (PFNVOID)FileSys_Write,
    (PFNVOID)FileSys_WriteWithSeek,
    (PFNVOID)FileSys_Copy
};

// 文件系统API功能集-参数说明
static const DWORD dwFileSysArgs[] = 
{
	0,
    ARG2_MAKE( PTR, PTR ),//CreateDirectory,
    ARG7_MAKE( PTR, DWORD, DWORD, PTR, DWORD, DWORD, DWORD ),//CreateFile,
	ARG1_MAKE( DWORD ),//CloseFile,
    ARG2_MAKE( PTR, PTR ),//DeleteAndRename,
    ARG1_MAKE( PTR ),//Delete,
    ARG8_MAKE( DWORD, DWORD, PTR, DWORD, PTR, DWORD, PTR, PTR ),//DeviceIoControl,
    ARG1_MAKE( DWORD ),//FindClose,
    ARG2_MAKE( PTR, PTR ),//FindFirst,
    ARG2_MAKE( DWORD, PTR ),//FindNext,
    ARG1_MAKE( DWORD ),//FlushBuffers,
    ARG4_MAKE( PTR, PTR, PTR, PTR ),//GetDiskFreeSpaceEx,
    ARG1_MAKE( PTR ),//GetAttributes,
    ARG2_MAKE( DWORD, PTR ),//GetInformation,
    ARG2_MAKE( DWORD, PTR ),//GetSize,
    ARG4_MAKE( DWORD, PTR, PTR, PTR ),//GetTime,
    ARG2_MAKE( PTR, PTR ),//Move,
    ARG5_MAKE( DWORD, PTR, DWORD, PTR, PTR ),//Read,
    ARG7_MAKE( DWORD, PTR, DWORD, PTR, PTR, DWORD, DWORD ),//ReadWithSeek,
    ARG1_MAKE( PTR ),//RemoveDirectory,
    ARG1_MAKE( DWORD ),//SetEnd,
    ARG2_MAKE( PTR, DWORD ),//SetAttributes,
    ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//SetPointer,
    ARG4_MAKE( DWORD, PTR, PTR, PTR ),//SetTime,
    ARG5_MAKE( DWORD, PTR, DWORD, PTR, PTR ),//Write,
    ARG6_MAKE( DWORD, PTR, DWORD, PTR, DWORD, DWORD ),//WriteWithSeek,
    ARG3_MAKE( PTR, PTR, DWORD ),//Copy
};

////////////////////////////////////
// 文件系统管理API注册功能集
static const PFNVOID lpFsdMgrAPI[] = {
	NULL,
	(PFNVOID)FileMgr_GetDiskInfo,
	(PFNVOID)FileMgr_ReadDisk,
	(PFNVOID)FileMgr_WriteDisk,
	(PFNVOID)FileMgr_RegisterVolume,
	(PFNVOID)FileMgr_RegisterFSD,
	(PFNVOID)FileMgr_UnregisterFSD,
	(PFNVOID)FileMgr_GetVolumeName,
	(PFNVOID)FileMgr_DeregisterVolume,
	(PFNVOID)FileMgr_CreateFileHandle,
	(PFNVOID)FileMgr_CloseFileHandle,
	(PFNVOID)FileMgr_CreateSearchHandle,
	(PFNVOID)FileMgr_CloseSearchHandle,
	(PFNVOID)FileMgr_DeviceIoControl
};

// 文件系统管理API注册功能集-参数说明
static const DWORD dwFsdMgrArgs[] = {
	NULL,
	ARG2_MAKE( DWORD, PTR ),//GetDiskInfo,
	ARG5_MAKE( DWORD, DWORD, DWORD, PTR, DWORD ),//ReadDisk,
	ARG5_MAKE( DWORD, DWORD, DWORD, PTR, DWORD ),//WriteDisk,
	ARG3_MAKE( DWORD, PTR, DWORD ),//RegisterVolume,
	ARG2_MAKE( PTR, PTR ),//RegisterFSD,
	ARG1_MAKE( PTR ),//UnregisterFSD,
	ARG3_MAKE( DWORD, PTR, DWORD ),//GetVolumeName,
	ARG1_MAKE( DWORD ),//DeregisterVolume,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//CreateFileHandle,
	ARG1_MAKE( DWORD ),//CloseFileHandle,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//CreateSearchHandle,
	ARG1_MAKE( DWORD ),//CloseSearchHandle,
	ARG8_MAKE( DWORD, DWORD, PTR, DWORD, PTR, DWORD, PTR, PTR ),//DeviceIoControl
};

// 注册表API功能集
static const PFNVOID lpRegistryAPI[] = {
	NULL,
    (PFNVOID)FileReg_CloseKey,
    (PFNVOID)FileReg_CreateKeyEx,
    (PFNVOID)FileReg_DeleteKey,
    (PFNVOID)FileReg_DeleteValue,
    (PFNVOID)FileReg_EnumKeyEx,
    (PFNVOID)FileReg_FlushKey,
    (PFNVOID)FileReg_EnumValue,
    (PFNVOID)FileReg_OpenKeyEx,
    (PFNVOID)FileReg_QueryInfoKey,
    (PFNVOID)FileReg_QueryValueEx,
    (PFNVOID)FileReg_SetValueEx
};

// 注册表API功能集-参数说明
static const DWORD dwRegistryArgs[] = {
	0,
    ARG1_MAKE( DWORD ),//RegCloseKey,
    ARG9_MAKE( DWORD, PTR, DWORD, PTR, DWORD, DWORD, PTR, PTR, PTR ),//RegCreateKeyEx,
    ARG2_MAKE( DWORD, PTR ),//RegDeleteKey,
    ARG2_MAKE( DWORD, PTR ),//RegDeleteValue,
    ARG8_MAKE( DWORD, DWORD, PTR, PTR, PTR, PTR, PTR, PTR ),//RegEnumKeyEx,
    ARG1_MAKE( DWORD ),//RegFlushKey,
    ARG8_MAKE( DWORD, DWORD, PTR, PTR, PTR, PTR, PTR, PTR ),//RegEnumValue,
    ARG5_MAKE( DWORD, PTR, DWORD, DWORD, PTR ),
    ARG12_MAKE( DWORD, PTR, PTR, PTR, PTR, PTR, PTR, PTR, PTR, PTR, PTR, PTR ),//RegQueryInfoKey,
    ARG6_MAKE( DWORD, PTR, PTR, PTR, PTR, PTR ),//RegQueryValueEx,
    ARG6_MAKE( DWORD, PTR, DWORD, DWORD, PTR, DWORD ),//RegSetValueEx
};

// **************************************************
// 声明：void InstallFileServer( void )
// 参数：
//	无
// 返回值：
//	无	
// 功能描述：
//	安装文件系统
// 引用: 
//	被 fsmain.c 调用
// **************************************************

void InstallFileServer( void )
{
	DEBUGMSG( FILESRV_ZONE, ( "InstallFileServer API_FILESYS.\r\n" ) );
	API_RegisterEx( API_FILESYS,  lpFileSysAPI, dwFileSysArgs, sizeof( lpFileSysAPI ) / sizeof(PFNVOID) );
	DEBUGMSG( FILESRV_ZONE, ( "InstallFileServer API_FSDMGR.\r\n" ) );
    API_RegisterEx( API_FSDMGR,  lpFsdMgrAPI, dwFsdMgrArgs, sizeof( lpFsdMgrAPI ) / sizeof(PFNVOID) );
	DEBUGMSG( FILESRV_ZONE, ( "InstallFileServer API_REGISTRY.\r\n" ) );
    API_RegisterEx( API_REGISTRY,  lpRegistryAPI, dwRegistryArgs, sizeof( lpRegistryAPI ) / sizeof(PFNVOID) );
}

//HANDLE hFileSysEvent;
// **************************************************
// 声明：int CALLBACK FileSys_Init( void )
// 参数：
//	无	
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	文件系统初始化
// 引用: 
//	
// **************************************************
#define DEBUG_FileSys_Init 0
int CALLBACK FileSys_Init( void )
{
	extern BOOL _LoadRamdisk( void );
	extern BOOL _LoadRomdisk( void );
	extern BOOL _InstallKMFS( void );


    //DbgOutString( "InitFileSystemMgr\r\n" );
    //if( _InitFileSystemMgr() == FALSE )
      //  return FALSE;
	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "InitFileSystemMgr.\r\n" ) );
	InstallFileServer();
//    DbgOutString( "_Install Kingmos file system.\r\n" );
	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "Install Kingmos file system.\r\n" ) );
    if( _InstallKMFS() == FALSE )
	{
		DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "InstallDefaultFAT return FALSE.\r\n" ) );
        return FALSE;    
	}

	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "now load sys ramdisk device-begin\r\n" ) );
	_LoadRamdisk();
	//_LoadSRAMDISK();
	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "now load sys ramdisk device-ok\r\n" ) );

#ifndef INLINE_PROGRAM
	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "now load sys romfsd device-begin.\r\n" ) );
	InitRomFsd();  // 
	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "now load sys romfsd device-end.\r\n" ) );
#endif

	_LoadRomdisk();
	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "now load sys registry-begin.\r\n" ) );
    InitRegistry();

	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "now load sys registry-end.\r\n" ) );
	API_SetReady( API_FILESYS );
	API_SetReady( API_FSDMGR );

	DEBUGMSG( FILESRV_ZONE | DEBUG_FileSys_Init, ( "filesys is ready.\r\n" ) );
	return TRUE;
}

// **************************************************
// 声明：void CALLBACK FileSys_Deinit( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		与 FileSys_Init 相反，卸载文件系统
// 引用: 
// **************************************************

void CALLBACK FileSys_Deinit( void )
{
	extern void _UnInstallKMFS( void );
    _UnInstallKMFS();
}

/*
int CALLBACK FileSys_WinMain(
							 HINSTANCE hInstance,
							 HINSTANCE hPrevInstance,
							 LPSTR     lpCmdLine,
							 int       nCmdShow )

{

    DbgOutString( "InitFileSystemMgr\r\n" );
    if( _InitFileSystemMgr() == FALSE )
        return FALSE;
	InstallFileServer();
    DbgOutString( "_InstallDefaultFAT\r\n" );
    if( _InstallDefaultFAT() == FALSE )
        return FALSE;    

	hFileSysEvent = CreateEvent( NULL, FALSE, FALSE, "filesys_event" );

	while( 1 )
	{
		int rv;
		rv = WaitForSingleObject( hFileSysEvent, INFINITE );
		break;
	}
	
	CloseHandle( hFileSysEvent );
	return 0;
}


  */

