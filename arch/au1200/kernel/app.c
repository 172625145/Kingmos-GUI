/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：应用程序管理
版本号：1.0.0
开发时期：2005-07-29
作者：李林
修改记录：
******************************************************/

#include <ewindows.h>
/*
BOOL WINAPI Application_Register( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon )
{
	return TRUE;
}

BOOL WINAPI Application_Load( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine )
{
	return TRUE;
}

*/

/*****************************************************
文件说明：quick exe module 模块管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

//#include <eframe.h>
//#include <eprogram.h>
//#include <efile.h>

//#include <epalloc.h>
//#include <epheap.h>
//#include <eprocess.h>
//#include <epcore.h>
//#include <coresrv.h>
//static char const GWME_ROOT_PATH[] = ".\\___romdisk.in\\";//".\\kingmos\\";
//static char const GWME_ROOT_PATH[] = ".\\kingmos\\";

#define GWME_ROOT_PATH "./kingmos/gwme/"

//typedef int ( WINAPI * WINMAINPROC)( HINSTANCE, HINSTANCE, LPTSTR, int );

typedef struct _MODULE_CLASS{
	struct _MODULE_CLASS * lpNext;
	LPTSTR lpszApName;
	WINMAINPROC lpProc;
	HICON hIcon;
	HICON hSmallIcon;
	HANDLE hResFile;
	UINT uiRefCount;
	CRITICAL_SECTION csModule;
	//LPTASK_DATA lpTask;
}MODULE_CLASS, FAR * PMODULE_CLASS;

typedef struct _TASK_DATA
{
	struct _TASK_DATA * lpNext;
	HANDLE hModule;
	DWORD  dwTaskId;
	char szCmdLine[1];
}TASK_DATA, FAR * LPTASK_DATA;


static HANDLE _CreateModule(  LPCTSTR lpszFileName, HANDLE hOwnerProcess, UINT uiFlag );
static BOOL _FreeModule( HANDLE hModule );
static HANDLE _GetModuleFileHandle( HANDLE hModule ); 
static LPVOID _GetModuleProcAddress( HANDLE hModule, LPCSTR lpProcName );
static UINT _LoadModule( HANDLE hModule, HANDLE hProcess, LPCTSTR lpCmdLine, UINT uiLoadFlag );
static LPCTSTR _GetModuleFileName( HANDLE hModule ); 
static DWORD  _GetSectionOffset( HANDLE hModule, LPCTSTR lpcszSectionName );

//已注册模块链表
static PMODULE_CLASS lpAppList = NULL;
static LPTASK_DATA   lpTaskList = NULL;

static CRITICAL_SECTION csTaskList;

//qe模块接口
/*
const MODULE_DRV qemd = 
{
    _CreateModule,
    _FreeModule,
    _GetModuleFileHandle,
	_GetModuleFileName,
    _GetModuleProcAddress,
	_GetSectionOffset,
    _LoadModule
};
*/

// ********************************************************************
//声明： static PMODULE_CLASS FindAp( LPCTSTR lpcszApName )
//参数：
//	IN lpcszApName - 应用程序名
//返回值：
//	假如成功，返回已注册的模块指针；否则，返回NULL
//功能描述：
//	得到之前用RegisterApplication注册的模块
//引用：
// ********************************************************************
static PMODULE_CLASS FindAp( LPCTSTR lpcszApName )
{
	PMODULE_CLASS lp = lpAppList;
	
	while( lp )
	{
		if( stricmp( lp->lpszApName, lpcszApName ) == 0 )
		{
			return lp;
		}
		lp = lp->lpNext;
	}
	return NULL;
}

// ********************************************************************
//声明： BOOL IsQEModuleExist( LPCTSTR lpcszApName )
//参数：
//	IN lpcszApName - 应用程序名
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//  判断该AP名是否是已经注册的QE模块
//引用：
// ********************************************************************

BOOL IsQEModuleExist( LPCTSTR lpcszApName )
{
	return FindAp( lpcszApName ) ? TRUE : FALSE;
}


// ********************************************************************
//声明：static HANDLE _CreateModule(  LPCTSTR lpszFileName, HANDLE hOwnerProcess, UINT uiFlag ) 
//参数：
//	IN lpszFileName - 应用程序名
//  IN hOwnerProcess - 创建该模块的进程
//	IN uiFlag - 创建标志(必须为CREATE_EXE)：
//				CREATE_EXE -  创建可执行模块
//返回值：
//	假如成功，返回模块句柄；否则，返回NULL
//功能描述：
//	创建应用程序实例
//引用：
// ********************************************************************

static HANDLE _CreateModule(  LPCTSTR lpszFileName, HANDLE hOwnerProcess, UINT uiFlag )
{
	//if( uiFlag == CREATE_EXE )
	{
		PMODULE_CLASS pmc = FindAp( lpszFileName );
		if( pmc )
		{	//增加引用计数
			pmc->uiRefCount ++;
		}
		return (HANDLE)pmc;
	}
	return NULL;
}

// ********************************************************************
//声明：static BOOL _FreeModule( HANDLE hModule ) 
//参数：
//	IN hModule - 模块句柄
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	与_CreateModule对应，释放模块
//引用：
// ********************************************************************

static BOOL _FreeModule( HANDLE hModule )
{	//如果引用计数不为零，减少它
	if( ((PMODULE_CLASS)hModule)->uiRefCount )
	    ((PMODULE_CLASS)hModule)->uiRefCount--;
	//
	if( ((PMODULE_CLASS)hModule)->uiRefCount == 0 )
	{
		if( ((PMODULE_CLASS)hModule)->hResFile &&
			((PMODULE_CLASS)hModule)->hResFile != INVALID_HANDLE_VALUE )
		{			
			CloseHandle( ((PMODULE_CLASS)hModule)->hResFile );
			((PMODULE_CLASS)hModule)->hResFile = NULL;
		}
	}
	return TRUE;
}

// ********************************************************************
//声明：static HANDLE _GetModuleFileHandle( HANDLE hModule ) 
//参数：
//	IN hModule - 模块句柄
//返回值：
//	假如成功，返回文件句柄；否则，返回NULL
//功能描述：
//	得到模块句柄，对于QE模块，是指该应用程序的资源文件句柄
//引用：
// ********************************************************************

static HANDLE _GetModuleFileHandle( HANDLE hModule )
{	// 该ap的资源文件已打开吗？
	if( ((PMODULE_CLASS)hModule)->hResFile == NULL )
	{	//没有打开，打开它
		TCHAR szFile[MAX_PATH] = GWME_ROOT_PATH;
		//对QE模块，其资源文件的名字为：apname+<.res>.例如AP=hello,则资源文件为hello.res
		strcat( szFile, ((PMODULE_CLASS)hModule)->lpszApName );
		strcat( szFile, ".res" );
		((PMODULE_CLASS)hModule)->hResFile = CreateFile( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	}

	return ((PMODULE_CLASS)hModule)->hResFile;//NULL;
}

// ********************************************************************
//声明： static LPCTSTR _GetModuleFileName( HANDLE hModule )
//参数：
//	IN hModule - 模块句柄
//返回值：
//	返回模块名
//功能描述：
//	得到已打开模块的名字
//引用：
// ********************************************************************

static LPCTSTR _GetModuleFileName( HANDLE hModule )
{
	return ((PMODULE_CLASS)hModule)->lpszApName;
}

// ********************************************************************
//声明： LPVOID _GetModuleProcAddress( HANDLE hModule, LPCSTR lpProcName )
//参数：
//	IN hModule - 模块句柄
//	IN lpProcName - 函数名
//返回值：
//	返回NULL
//功能描述：
//	得到模块的函数地址，QE模块不支持
//引用：
// ********************************************************************

static LPVOID _GetModuleProcAddress( HANDLE hModule, LPCSTR lpProcName )
{
	return NULL;
}

// ********************************************************************
//声明： static DWORD  _GetSectionOffset( HANDLE hModule, LPCTSTR lpcszSectionName )
//参数：
//	IN hModule - 模块句柄
//	IN lpcszSectionName - 段名
//返回值：
//	返回 0
//功能描述：
//	得到段的偏移量
//引用：
// ********************************************************************
static DWORD  _GetSectionOffset( HANDLE hModule, LPCTSTR lpcszSectionName )
{
	return 0;
}

// ********************************************************************
//声明：static UINT _LoadModule( HANDLE hModule, HANDLE hInst, LPCTSTR lpszCommandLine, UINT uiLoadFlag )
//参数：
//	IN hModule - 模块句柄
//	IN hInst - 实例句柄
//  IN lpszCommandLine - 传递给WinMain的命令行参数
//  IN uiLoadFlag - 参数（必须为0）
//返回值：
//	返回WinMain的返回值
//功能描述：
//	运行一个可执行模块
//引用：
// ********************************************************************

static UINT _LoadModule( HANDLE hModule, HANDLE hInst, LPCTSTR lpszCommandLine, UINT uiLoadFlag )
{
//	while( *lpszCommandLine && *lpszCommandLine != ' ' )
//		lpszCommandLine++;
//	if( *lpszCommandLine == ' ' )
//		lpszCommandLine++;

	return ((PMODULE_CLASS)hModule)->lpProc( hModule, NULL, (LPTSTR)lpszCommandLine, SW_SHOW );
}

static BOOL AddTask( LPTASK_DATA lpTask )
{
	// enter cs
	EnterCriticalSection( &csTaskList );
	lpTask->lpNext = lpTaskList;
	lpTaskList = lpTask;
	LeaveCriticalSection( &csTaskList );	
	// leave cs
	return TRUE;
}

static VOID ReamoveTask( LPTASK_DATA lpTask )
{
	LPTASK_DATA lpPrevTask;

	lpPrevTask = lpTaskList;
	if( lpPrevTask )
	{
		if( lpPrevTask == lpTask )
		{  //first 
			lpTaskList = lpPrevTask->lpNext;
		}
		else
		{
			while( lpPrevTask->lpNext != lpTask )
				lpPrevTask = lpPrevTask->lpNext;
			if( lpPrevTask->lpNext == lpTask )
			{  //find it
				lpPrevTask->lpNext = lpTask->lpNext;
			}
		}
	}
	// leave cs
}


// ********************************************************************
//声明：BOOL WINAPI KL_RegisterApplication( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon )
//参数：
//	IN lpcszName - 注册的应用程序名
//	IN lpApProc - 应用程序的WinMain入口
//	IN hIcon - 在桌面上显示的图标
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//引用：
//	系统API
// ********************************************************************

#define DEBUG_REGISTERAPPLICATION 0
BOOL WINAPI Application_Register( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon )
{
	int len;
	PMODULE_CLASS lp;

//	DEBUGMSG( DEBUG_REGISTERAPPLICATION, ( "Register Application:%s.\r\n", lpcszName ) );
	len = strlen( lpcszName ) + 1;
	lp = (PMODULE_CLASS)malloc( sizeof( MODULE_CLASS ) );//
	
	if( lp )
	{	//初始化MODULE_CLASS结构
		memset( lp, 0, sizeof( MODULE_CLASS ) );
		lp->lpszApName = malloc( len * sizeof( TCHAR ) );//
		
		if( lp->lpszApName )
		{
			lp->lpProc = lpApProc;
			lp->hIcon = hIcon;
			strcpy( lp->lpszApName, lpcszName );
			//加入模块链表
			lp->lpNext = lpAppList;
			lpAppList = lp;
			InitializeCriticalSection( &lp->csModule );
//			DEBUGMSG( DEBUG_REGISTERAPPLICATION, ( "Register Application Success.\r\n" ) );//KL_DebugOutString( "Register Application Success\r\n" );
		    return TRUE;
		}
		else
		{
			free( lp );
		}
	}
//	RETAILMSG( 1, ( "Register Application Failure\r\n" ) );//KL_DebugOutString( "Register Application Failure\r\n" );
	return FALSE;
}


#define DEBUG_StartApplicationThread 1
// 启动任务
int WINAPI StartApplicationThread( LPVOID lParam )
{
	LPTASK_DATA lpTask = (LPTASK_DATA)lParam;


	DEBUGMSG( DEBUG_StartApplicationThread, ( "thread run...\r\n" ) );

	lpTask->dwTaskId = GetCurrentThreadId();
	AddTask( lpTask );
	_LoadModule( lpTask->hModule, NULL, lpTask->szCmdLine, 0 );	
	ReamoveTask( lpTask );
	free( lpTask );
	return 0;
}


// ********************************************************************
//声明：BOOL WINAPI KL_LoadApplication( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine ) 
//参数：
//	IN lpcszName - 需要运行的应用程序名
//	IN lpcszCmdLine - 传给应用程序WinMain入口的命令行参数
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	运行应用程序
//引用：
//	系统API
// ********************************************************************
#define DEBUG_LOADAPPLICATION 1

BOOL WINAPI Application_Load( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine )
{
	//PMODULE_CLASS lp = FindAp( lpcszName );
	HANDLE hModule;
	
	DEBUGMSG( DEBUG_LOADAPPLICATION, ( "LoadApplication=%s, cmdline=%s\r\n", lpcszName, lpcszCmdLine ? lpcszCmdLine : "" ) );

	hModule = _CreateModule( lpcszName, NULL, 0 );
	
	if(hModule)
	{   //运行任务		
		char * lpParam = NULL;
		int len = 0;
		LPTASK_DATA lpTask = NULL;
		if( lpcszCmdLine  )
		{
			len = strlen( lpcszCmdLine );
		}
		len += sizeof(TASK_DATA);
		lpTask = malloc( len );
		if( lpTask )
		{			
			memset( lpTask, 0, len );
			
			lpTask->hModule = hModule;
			if( lpcszCmdLine )
			{
				strcpy( lpTask->szCmdLine, lpcszCmdLine );
			}
			CreateThread( NULL, 0, StartApplicationThread, lpTask, 0, 0 );
		}
		else
		{
			;//			RETAILMSG( 1, ("Application_Load:not enough memory!\r\n") );
		}

		//CreateThread( NULL, 0, StartApplicationThread, lpParam, 0, 0 );
		//return DoCreateProcess( lpcszName, lpcszCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL, &qemd );
	}    
	else
	{
		;//RETAILMSG( 1, ("Application_Load:not find lpProc\r\n") );
	}
    return FALSE;
}


// ********************************************************************
//声明：static HICON _GetApIcon( LPCTSTR lpcszName, UINT fIconType ) 
//参数：
//	IN lpcszName - 应用程序名
//	IN fIconType - 图标类型：
//			SMALL_ICON - 小图标
//			
//返回值：
//	假如成功，返回图标句柄；否则，返回NULL
//功能描述：
//引用：
// ********************************************************************

static HICON _GetApIcon( LPCTSTR lpcszName, UINT fIconType )
{
	
	MODULE_CLASS * lp = NULL;
	TCHAR filePath[MAX_PATH] = GWME_ROOT_PATH;
	HANDLE handle = NULL;

	if( lpcszName )
	{	//从应用程序对应的资源文件（= lpcszName + '.res'）得到图标
	    strcat( filePath, lpcszName );
		strcat( filePath, ".res" );
	}
	else
	{	//从系统的资源文件得到图标
		strcat( filePath, "system.res" );
	}
	RETAILMSG( 1, ( "filePath=%s.\r\n", filePath ) ); 
	if( fIconType == SMALL_ICON )	
	    handle = LoadImage( NULL, 
		                  filePath, 
						  IMAGE_ICON, 
						  GetSystemMetrics( SM_CXSMICON ),  
						  GetSystemMetrics( SM_CYSMICON ), 
						  LR_LOADFROMFILE|LR_SHARED );
	else
	    handle = LoadImage( NULL, 
		                  filePath, 
						  IMAGE_ICON, 
						  GetSystemMetrics( SM_CXICON ), 
						  GetSystemMetrics( SM_CYICON ),
						  LR_LOADFROMFILE|LR_SHARED );
	return handle;
}


// ********************************************************************
//声明：HICON WINAPI KL_GetApplicationIcon( LPCTSTR lpcszName, UINT fIconType ) 
//参数：
//	IN lpcszName - 应用程序名
//	IN fIconType - 图标类型：
//			SMALL_ICON - 小图标
//返回值：
//	假如成功，返回图标句柄；否则，返回FALSE
//功能描述：
//	得到应用程序的图标
//引用：
//	系统API
// ********************************************************************

HICON WINAPI GetApplication_Icon( LPCTSTR lpcszName, UINT fIconType )
{
	//	查找QE模块
	PMODULE_CLASS pm = FindAp( lpcszName );
	if( pm )
	{
		if( fIconType == SMALL_ICON )
		{
			if( pm->hSmallIcon == NULL )
			{	//从文件得到
				pm->hSmallIcon = _GetApIcon( lpcszName, fIconType );
				if( pm->hSmallIcon == NULL && lpcszName )
					pm->hSmallIcon = _GetApIcon( NULL, fIconType );
			}
			return pm->hSmallIcon;
		}
		else
		{	//large icon
			if( pm->hIcon == NULL )
			{	//从文件得到
				pm->hIcon = _GetApIcon( lpcszName, fIconType );
				if( pm->hIcon == NULL && lpcszName )
					pm->hIcon = _GetApIcon( NULL, fIconType );
			}
			return pm->hIcon;
		}
	}
	return NULL;
}

static LPTASK_DATA GetAPICallerTaskPtr( void )
{
	LPTASK_DATA lpTask;
	DWORD dwTaskId = GetCurrentThreadId();

	// enter cs
	EnterCriticalSection( &csTaskList );
	lpTask = lpTaskList;
	//ASSERT( lpTask );
	
	while( lpTask  )
	{
		if( lpTask->dwTaskId == dwTaskId )
			break;
		lpTask = lpTask->lpNext;		
	}
	// leave cs
	LeaveCriticalSection( &csTaskList );
	//ASSERT( 0 );
	return lpTask;
}


//HMODULE WINAPI Module_GetHandle( LPCTSTR lpModuleName )
//{
//}

#define DEBUG_GET_MODULE_HANDLE 0
HMODULE WINAPI Module_GetHandle( LPCTSTR lpModuleName )
{
//	DEBUGMSG( DEBUG_GET_MODULE_HANDLE, ( "Module_GetHandle=0x%x.\r\n", lpModuleName ? NULL : GetAPICallerTaskPtr()->hModule ) );
	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	if( lpModuleName )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return NULL;
	}
	else
		return GetAPICallerTaskPtr()->hModule;
}



HANDLE WINAPI Module_GetSectionOffset( HMODULE hModule, LPCTSTR lpsz )
{
	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return 0;
}


HANDLE WINAPI Module_GetFileHandle( HMODULE hModule )
{	
	HANDLE hFile;

	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	hFile = _GetModuleFileHandle( hModule );

	if( hFile != NULL && hFile != INVALID_HANDLE_VALUE )
	{
		PMODULE_CLASS pModule = (PMODULE_CLASS)hModule;
		EnterCriticalSection( &pModule->csModule ); 
		return hFile;
	}
	return INVALID_HANDLE_VALUE;
}

BOOL WINAPI Module_ReleaseFileHandle( HANDLE hModule, HANDLE hFile )
{
	PMODULE_CLASS pModule = (PMODULE_CLASS)hModule;
	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	if( hFile != NULL && hFile != INVALID_HANDLE_VALUE )
	{
		if( pModule->hResFile == hFile )
		{
			LeaveCriticalSection( &pModule->csModule ); 
			return TRUE;
		}
	}
	return FALSE;	
}

BOOL InitApplicationMgr( void )
{
	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	lpTaskList = NULL;
	lpAppList = NULL;
	InitializeCriticalSection( &csTaskList );	
	return TRUE;
}

VOID DeinitApplicationMgr( void )
{
	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	DeleteCriticalSection( &csTaskList );	
}


////////////////////////////////////////////
