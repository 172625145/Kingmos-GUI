/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����Ӧ�ó������
�汾�ţ�1.0.0
����ʱ�ڣ�2005-07-29
���ߣ�����
�޸ļ�¼��
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
�ļ�˵����quick exe module ģ�����
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
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
#define GWME_ROOT_PATH "./kingmos/"

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

//��ע��ģ������
static PMODULE_CLASS lpAppList = NULL;
static LPTASK_DATA   lpTaskList = NULL;

static CRITICAL_SECTION csTaskList;

//qeģ��ӿ�
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
//������ static PMODULE_CLASS FindAp( LPCTSTR lpcszApName )
//������
//	IN lpcszApName - Ӧ�ó�����
//����ֵ��
//	����ɹ���������ע���ģ��ָ�룻���򣬷���NULL
//����������
//	�õ�֮ǰ��RegisterApplicationע���ģ��
//���ã�
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
//������ BOOL IsQEModuleExist( LPCTSTR lpcszApName )
//������
//	IN lpcszApName - Ӧ�ó�����
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//  �жϸ�AP���Ƿ����Ѿ�ע���QEģ��
//���ã�
// ********************************************************************

BOOL IsQEModuleExist( LPCTSTR lpcszApName )
{
	return FindAp( lpcszApName ) ? TRUE : FALSE;
}


// ********************************************************************
//������static HANDLE _CreateModule(  LPCTSTR lpszFileName, HANDLE hOwnerProcess, UINT uiFlag ) 
//������
//	IN lpszFileName - Ӧ�ó�����
//  IN hOwnerProcess - ������ģ��Ľ���
//	IN uiFlag - ������־(����ΪCREATE_EXE)��
//				CREATE_EXE -  ������ִ��ģ��
//����ֵ��
//	����ɹ�������ģ���������򣬷���NULL
//����������
//	����Ӧ�ó���ʵ��
//���ã�
// ********************************************************************

static HANDLE _CreateModule(  LPCTSTR lpszFileName, HANDLE hOwnerProcess, UINT uiFlag )
{
	//if( uiFlag == CREATE_EXE )
	{
		PMODULE_CLASS pmc = FindAp( lpszFileName );
		if( pmc )
		{	//�������ü���
			pmc->uiRefCount ++;
		}
		return (HANDLE)pmc;
	}
	return NULL;
}

// ********************************************************************
//������static BOOL _FreeModule( HANDLE hModule ) 
//������
//	IN hModule - ģ����
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	��_CreateModule��Ӧ���ͷ�ģ��
//���ã�
// ********************************************************************

static BOOL _FreeModule( HANDLE hModule )
{	//������ü�����Ϊ�㣬������
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
//������static HANDLE _GetModuleFileHandle( HANDLE hModule ) 
//������
//	IN hModule - ģ����
//����ֵ��
//	����ɹ��������ļ���������򣬷���NULL
//����������
//	�õ�ģ����������QEģ�飬��ָ��Ӧ�ó������Դ�ļ����
//���ã�
// ********************************************************************

static HANDLE _GetModuleFileHandle( HANDLE hModule )
{	// ��ap����Դ�ļ��Ѵ���
	if( ((PMODULE_CLASS)hModule)->hResFile == NULL )
	{	//û�д򿪣�����
		TCHAR szFile[MAX_PATH] = GWME_ROOT_PATH;
		//��QEģ�飬����Դ�ļ�������Ϊ��apname+<.res>.����AP=hello,����Դ�ļ�Ϊhello.res
		strcat( szFile, ((PMODULE_CLASS)hModule)->lpszApName );
		strcat( szFile, "/" );
		strcat( szFile, ((PMODULE_CLASS)hModule)->lpszApName );
		strcat( szFile, ".res" );
		((PMODULE_CLASS)hModule)->hResFile = CreateFile( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	}

	return ((PMODULE_CLASS)hModule)->hResFile;//NULL;
}

// ********************************************************************
//������ static LPCTSTR _GetModuleFileName( HANDLE hModule )
//������
//	IN hModule - ģ����
//����ֵ��
//	����ģ����
//����������
//	�õ��Ѵ�ģ�������
//���ã�
// ********************************************************************

static LPCTSTR _GetModuleFileName( HANDLE hModule )
{
	return ((PMODULE_CLASS)hModule)->lpszApName;
}

// ********************************************************************
//������ LPVOID _GetModuleProcAddress( HANDLE hModule, LPCSTR lpProcName )
//������
//	IN hModule - ģ����
//	IN lpProcName - ������
//����ֵ��
//	����NULL
//����������
//	�õ�ģ��ĺ�����ַ��QEģ�鲻֧��
//���ã�
// ********************************************************************

static LPVOID _GetModuleProcAddress( HANDLE hModule, LPCSTR lpProcName )
{
	return NULL;
}

// ********************************************************************
//������ static DWORD  _GetSectionOffset( HANDLE hModule, LPCTSTR lpcszSectionName )
//������
//	IN hModule - ģ����
//	IN lpcszSectionName - ����
//����ֵ��
//	���� 0
//����������
//	�õ��ε�ƫ����
//���ã�
// ********************************************************************
static DWORD  _GetSectionOffset( HANDLE hModule, LPCTSTR lpcszSectionName )
{
	return 0;
}

// ********************************************************************
//������static UINT _LoadModule( HANDLE hModule, HANDLE hInst, LPCTSTR lpszCommandLine, UINT uiLoadFlag )
//������
//	IN hModule - ģ����
//	IN hInst - ʵ�����
//  IN lpszCommandLine - ���ݸ�WinMain�������в���
//  IN uiLoadFlag - ����������Ϊ0��
//����ֵ��
//	����WinMain�ķ���ֵ
//����������
//	����һ����ִ��ģ��
//���ã�
// ********************************************************************

static UINT _LoadModule( HANDLE hModule, HANDLE hInst, LPCTSTR lpszCommandLine, UINT uiLoadFlag )
{
	//while( *lpszCommandLine && *lpszCommandLine != ' ' )
	//	lpszCommandLine++;
	//if( *lpszCommandLine == ' ' )
	//	lpszCommandLine++;

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
//������BOOL WINAPI KL_RegisterApplication( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon )
//������
//	IN lpcszName - ע���Ӧ�ó�����
//	IN lpApProc - Ӧ�ó����WinMain���
//	IN hIcon - ����������ʾ��ͼ��
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//���ã�
//	ϵͳAPI
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
	{	//��ʼ��MODULE_CLASS�ṹ
		memset( lp, 0, sizeof( MODULE_CLASS ) );
		lp->lpszApName = malloc( len * sizeof( TCHAR ) );//
		
		if( lp->lpszApName )
		{
			lp->lpProc = lpApProc;
			lp->hIcon = hIcon;
			strcpy( lp->lpszApName, lpcszName );
			//����ģ������
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
// ��������
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
//������BOOL WINAPI KL_LoadApplication( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine ) 
//������
//	IN lpcszName - ��Ҫ���е�Ӧ�ó�����
//	IN lpcszCmdLine - ����Ӧ�ó���WinMain��ڵ������в���
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	����Ӧ�ó���
//���ã�
//	ϵͳAPI
// ********************************************************************
#define DEBUG_LOADAPPLICATION 1

BOOL WINAPI Application_Load( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine )
{
	//PMODULE_CLASS lp = FindAp( lpcszName );
	HANDLE hModule;
	
	DEBUGMSG( DEBUG_LOADAPPLICATION, ( "LoadApplication=%s, cmdline=%s\r\n", lpcszName, lpcszCmdLine ? lpcszCmdLine : "" ) );

	hModule = _CreateModule( lpcszName, NULL, 0 );
	
	if(hModule)
	{   //��������		
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
//������static HICON _GetApIcon( LPCTSTR lpcszName, UINT fIconType ) 
//������
//	IN lpcszName - Ӧ�ó�����
//	IN fIconType - ͼ�����ͣ�
//			SMALL_ICON - Сͼ��
//			
//����ֵ��
//	����ɹ�������ͼ���������򣬷���NULL
//����������
//���ã�
// ********************************************************************

static HICON _GetApIcon( LPCTSTR lpcszName, UINT fIconType )
{
	
	MODULE_CLASS * lp = NULL;
	TCHAR filePath[MAX_PATH] = GWME_ROOT_PATH;
	HANDLE handle = NULL;

	if( lpcszName )
	{	//��Ӧ�ó����Ӧ����Դ�ļ���= lpcszName + '.res'���õ�ͼ��
	    strcat( filePath, lpcszName );
		strcat( filePath, "/" );
		strcat( filePath, lpcszName );
		strcat( filePath, ".res" );
	}
	else
	{	//��ϵͳ����Դ�ļ��õ�ͼ��
		strcat( filePath, "gwme/gwme.res" );
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
//������HICON WINAPI KL_GetApplicationIcon( LPCTSTR lpcszName, UINT fIconType ) 
//������
//	IN lpcszName - Ӧ�ó�����
//	IN fIconType - ͼ�����ͣ�
//			SMALL_ICON - Сͼ��
//����ֵ��
//	����ɹ�������ͼ���������򣬷���FALSE
//����������
//	�õ�Ӧ�ó����ͼ��
//���ã�
//	ϵͳAPI
// ********************************************************************

HICON WINAPI GetApplication_Icon( LPCTSTR lpcszName, UINT fIconType )
{
	//	����QEģ��
	PMODULE_CLASS pm = FindAp( lpcszName );
	if( pm )
	{
		if( fIconType == SMALL_ICON )
		{
			if( pm->hSmallIcon == NULL )
			{	//���ļ��õ�
				pm->hSmallIcon = _GetApIcon( lpcszName, fIconType );
				if( pm->hSmallIcon == NULL && lpcszName )
					pm->hSmallIcon = _GetApIcon( NULL, fIconType );
			}
			return pm->hSmallIcon;
		}
		else
		{	//large icon
			if( pm->hIcon == NULL )
			{	//���ļ��õ�
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
