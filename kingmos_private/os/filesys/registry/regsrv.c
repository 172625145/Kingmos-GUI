/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：gwme 服务器
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <efile.h>
#include <eapisrv.h>
#include <efsdmgr.h>
#include <filesrv.h>


extern void InitRegistry( void );

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

#define REGSRV_ZONE 0
#ifdef INLINE_PROGRAM

int CALLBACK WinMain_Registry(HINSTANCE hInstance,
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
	InitRegistry();
	DEBUGMSG( REGSRV_ZONE, ( "InstallFileServer API_REGISTRY.\r\n" ) );
    API_RegisterEx( API_REGISTRY,  lpRegistryAPI, dwRegistryArgs, sizeof( lpRegistryAPI ) / sizeof(PFNVOID) );
	API_SetReady( API_REGISTRY );

	Sleep( -1 );
	return 0;
}
