/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：数据表服务管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
	2004-09-15,增加Table_Flush
******************************************************/

#include <eframe.h>
#include <etable.h>
#include <eapisrv.h>

#include <tablesrv.h>

static DWORD WINAPI TableServerHandler( HANDLE handle, DWORD dwIoControlCode , DWORD dwParam, LPVOID lpParam );

static const SERVER_CTRL_INFO sci = { sizeof( SERVER_CTRL_INFO ),
                                      SCI_NOT_HANDLE_THREAD_EXIT,
									  TableServerHandler };

///////////////////////////////////////////////////
// 数据表API 函数入口
static const PFNVOID lpTableAPI[] = 
{
	(PFNVOID)&sci,//TableServerHandler,
	(PFNVOID)Table_CreateByStruct,
	(PFNVOID)Table_Delete,
	(PFNVOID)Table_Close,
	(PFNVOID)Table_SetRecordPointer,
	(PFNVOID)Table_NextRecordPointer,
	(PFNVOID)Table_ReadField,
	(PFNVOID)Table_WriteField,
	(PFNVOID)Table_WriteRecord,
	(PFNVOID)Table_DeleteRecord,
	(PFNVOID)Table_SortRecord,
	(PFNVOID)Table_AppendRecord,
	(PFNVOID)Table_ReadRecord,
	(PFNVOID)Table_InsertRecord,
	(PFNVOID)Table_CountRecord,
	(PFNVOID)Table_GetRecordInfo,
	(PFNVOID)Table_GetTableInfo,
	(PFNVOID)Table_GetRecordSize,
	(PFNVOID)Table_FindFirst,
	(PFNVOID)Table_FindNext,
	(PFNVOID)Table_CloseFind,
	(PFNVOID)Table_Pack,
	(PFNVOID)Table_ReadMultiRecords,
    (PFNVOID)Table_GetUserData,
	(PFNVOID)Table_SetUserData,
	(PFNVOID)Table_Flush,
	(PFNVOID)Table_PackByHandle
};

// 数据表API函数参数定义
static const DWORD dwTableArgs[] = 
{
	ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),
	ARG1_MAKE( PTR ),//Table_CreateByStruct,
	ARG1_MAKE( PTR ),//Table_Delete,
	ARG1_MAKE( DWORD ),//Table_Close,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//Table_SetRecordPointer,
	ARG2_MAKE( DWORD, DWORD ),//Table_NextRecordPointer,
	ARG5_MAKE( DWORD, DWORD, DWORD, PTR, DWORD ),//Table_ReadField,
	ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//Table_WriteField,
	ARG2_MAKE( DWORD, PTR ),//Table_WriteRecord,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//Table_DeleteRecord,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//Table_SortRecord,
	ARG2_MAKE( DWORD, PTR ),//Table_AppendRecord,
	ARG4_MAKE( DWORD, PTR, PTR, PTR ),//Table_ReadRecord,
	ARG3_MAKE( DWORD, DWORD, PTR ),//Table_InsertRecord,
	ARG1_MAKE( DWORD ),//Table_CountRecord,
	ARG2_MAKE( DWORD, PTR ),//Table_GetRecordInfo,
	ARG2_MAKE( PTR, PTR ),//Table_GetTableInfo,
	ARG1_MAKE( DWORD ),//Table_GetRecordSize,
	ARG5_MAKE( DWORD, DWORD, PTR, DWORD, PTR ),//Table_FindFirst,
	ARG2_MAKE( DWORD, PTR ),//Table_FindNext,
	ARG1_MAKE( DWORD ),//Table_CloseFind,
	ARG2_MAKE( PTR, DWORD ),//Table_Pack,
	ARG2_MAKE( DWORD, PTR ),  //Table_ReadMultiRecords,
	ARG3_MAKE( DWORD, PTR, DWORD ),//Table_GetUserData
	ARG3_MAKE( DWORD, PTR, DWORD ),//Table_SetUserData
	ARG1_MAKE( DWORD ),//Table_Flush
	ARG2_MAKE( DWORD, DWORD ),//Table_PackByHandle
};

//static HANDLE hTableEvent;

// **************************************************
// 声明：DWORD WINAPI TableServerHandler( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
// 参数：
//    IN hServer - 服务句柄
//	IN dwServerCode - 控制码
//    IN dwParam - 第一个参数
//    IN lpParam - 第二个参数
// 返回值：
//	依赖与不同的控制
// 功能描述：
//	服务总线控制，处理各种系统服务，通知功能
// 引用: 
//	
// ************************************************

static DWORD WINAPI TableServerHandler( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
{
	switch( dwServerCode )
	{
	case SCC_BROADCAST_PROCESS_EXIT:
		Table_CloseAll( (HANDLE)dwParam );
		break;
	case SCC_BROADCAST_POWER:
		if( dwParam == SBP_APMSUSPEND )
		{	//仅仅刷新所有的数据，不关闭打开的句柄
			Table_PowerHandler( FALSE );
		}
		return TRUE;
	case SCC_BROADCAST_SHUTDOWN:
		//刷新所有的数据，关闭打开的句柄
		Table_PowerHandler( TRUE );
		break;
	default:
		return Sys_DefServerProc( hServer, dwServerCode , dwParam, lpParam );
	}
	return 0;
}

// **************************************************
// 声明：static void InstallTableServer( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		注册数据表服务功能
// 引用: 
//		
// ************************************************

static void InstallTableServer( void )
{
	//API_Register( API_TABLE,  (PFNVOID)lpTableAPI, sizeof( lpTableAPI ) / sizeof(PFNVOID) );
	API_RegisterEx( API_TABLE,  lpTableAPI, dwTableArgs, sizeof( lpTableAPI ) / sizeof(PFNVOID) );
	API_SetReady( API_TABLE );
}

#define DEBUG_FlushTableThread 1
static DWORD WINAPI FlushTableThread( LPVOID lParam )
{
	extern BOOL TableAutoHandle( BOOL bShutDown ); 
	DEBUGMSG( DEBUG_FlushTableThread, ( "FlushTableThread: entry.\r\n" ) );
	while( 1 )
	{
		Sleep( 5000 );	//闲置x秒钟
		TableAutoHandle( FALSE );		//检查是否需要刷新数据	
	}
	DEBUGMSG( DEBUG_FlushTableThread, ( "FlushTableThread: leave.\r\n" ) );
	return 0;
}

// ********************************************************************
// 声明：int CALLBACK Table_WinMain(HINSTANCE hInstance,
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
#define DEBUG_TABLE_WINMAIN 0

#ifdef INLINE_PROGRAM
int CALLBACK Table_WinMain(
							 HINSTANCE hInstance,
							 HINSTANCE hPrevInstance,
							 LPSTR     lpCmdLine,
							 int       nCmdShow )
#else
int CALLBACK WinMain(
							 HINSTANCE hInstance,
							 HINSTANCE hPrevInstance,
							 LPSTR     lpCmdLine,
							 int       nCmdShow )
#endif
{
    extern BOOL InitialTable( void );
	HANDLE hThread;
    DEBUGMSG( DEBUG_TABLE_WINMAIN, ( "Init Table System\r\n" ) );

	if( API_IsReady( API_TABLE ) )
	{	// 系统已经有该进程的实例，退出本身
		WARNMSG( 1, ( "the table has already load.exit!\r\n" ) );
		return -1;
	}

	while( API_IsReady( API_FILESYS ) == FALSE ) //文件系统是否准备好 
		Sleep(50);  // 没有，等待文件系统

    if( InitialTable() == FALSE ) // 
		return FALSE;

	InstallTableServer();

	hThread = CreateThread( NULL, 0, FlushTableThread, 0, 0, NULL );

	//hTableEvent = CreateEvent( NULL, FALSE, FALSE, "table_event" );
    // 等待服务，主线程没有任何事情可以做
	while( 1 )
	{
		//int rv;
		//rv = WaitForSingleObject( hTableEvent, INFINITE );
		Sleep( INFINITE );
		break;
	}
	CloseHandle( hThread );
	
	//CloseHandle( hTableEvent );
	return 0;
}



