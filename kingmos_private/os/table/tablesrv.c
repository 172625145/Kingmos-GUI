/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�������ݱ�������
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2004-09-15,����Table_Flush
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
// ���ݱ�API �������
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

// ���ݱ�API������������
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
// ������DWORD WINAPI TableServerHandler( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
// ������
//    IN hServer - ������
//	IN dwServerCode - ������
//    IN dwParam - ��һ������
//    IN lpParam - �ڶ�������
// ����ֵ��
//	�����벻ͬ�Ŀ���
// ����������
//	�������߿��ƣ��������ϵͳ����֪ͨ����
// ����: 
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
		{	//����ˢ�����е����ݣ����رմ򿪵ľ��
			Table_PowerHandler( FALSE );
		}
		return TRUE;
	case SCC_BROADCAST_SHUTDOWN:
		//ˢ�����е����ݣ��رմ򿪵ľ��
		Table_PowerHandler( TRUE );
		break;
	default:
		return Sys_DefServerProc( hServer, dwServerCode , dwParam, lpParam );
	}
	return 0;
}

// **************************************************
// ������static void InstallTableServer( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		ע�����ݱ������
// ����: 
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
		Sleep( 5000 );	//����x����
		TableAutoHandle( FALSE );		//����Ƿ���Ҫˢ������	
	}
	DEBUGMSG( DEBUG_FlushTableThread, ( "FlushTableThread: leave.\r\n" ) );
	return 0;
}

// ********************************************************************
// ������int CALLBACK Table_WinMain(HINSTANCE hInstance,
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
	{	// ϵͳ�Ѿ��иý��̵�ʵ�����˳�����
		WARNMSG( 1, ( "the table has already load.exit!\r\n" ) );
		return -1;
	}

	while( API_IsReady( API_FILESYS ) == FALSE ) //�ļ�ϵͳ�Ƿ�׼���� 
		Sleep(50);  // û�У��ȴ��ļ�ϵͳ

    if( InitialTable() == FALSE ) // 
		return FALSE;

	InstallTableServer();

	hThread = CreateThread( NULL, 0, FlushTableThread, 0, 0, NULL );

	//hTableEvent = CreateEvent( NULL, FALSE, FALSE, "table_event" );
    // �ȴ��������߳�û���κ����������
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



