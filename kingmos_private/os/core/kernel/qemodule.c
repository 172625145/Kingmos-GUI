/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����quick exe module ģ�����
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <eprogram.h>
#include <efile.h>

#include <epalloc.h>
#include <epheap.h>
#include <eprocess.h>
#include <epcore.h>
#include <coresrv.h>

typedef struct _MODULE_CLASS{
	struct _MODULE_CLASS * lpNext;
	LPTSTR lpszApName;
	WINMAINPROC lpProc;
	HICON hIcon;
	HICON hSmallIcon;
	HANDLE hResFile;
	UINT uiRefCount;
}MODULE_CLASS, FAR * PMODULE_CLASS;

static HANDLE _CreateModule(  LPCTSTR lpszFileName, HANDLE hOwnerProcess, UINT uiFlag );
static BOOL _FreeModule( HANDLE hModule );
static HANDLE _GetModuleFileHandle( HANDLE hModule ); 
static LPVOID _GetModuleProcAddress( HANDLE hModule, LPCSTR lpProcName );
static UINT _LoadModule( HANDLE hModule, HANDLE hProcess, LPCTSTR lpCmdLine, UINT uiLoadFlag );
static LPCTSTR _GetModuleFileName( HANDLE hModule ); 
static DWORD  _GetSectionOffset( HANDLE hModule, LPCTSTR lpcszSectionName );

//��ע��ģ������
static PMODULE_CLASS lpAppList = NULL;
//qeģ��ӿ�
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
	if( uiFlag == CREATE_EXE )
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
			KL_CloseHandle( ((PMODULE_CLASS)hModule)->hResFile );
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
		TCHAR szFile[MAX_PATH] = "\\kingmos\\";
		//��QEģ�飬����Դ�ļ�������Ϊ��apname+<.res>.����AP=hello,����Դ�ļ�Ϊhello.res
		strcat( szFile, ((PMODULE_CLASS)hModule)->lpszApName );
		strcat( szFile, ".res" );
		((PMODULE_CLASS)hModule)->hResFile = KL_CreateFile( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
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
	while( *lpszCommandLine && *lpszCommandLine != ' ' )
		lpszCommandLine++;
	if( *lpszCommandLine == ' ' )
		lpszCommandLine++;

	return ((PMODULE_CLASS)hModule)->lpProc( hInst, NULL, (LPTSTR)lpszCommandLine, SW_SHOW );
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
BOOL WINAPI KL_RegisterApplication( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon )
{
	int len;
	PMODULE_CLASS lp;

	DEBUGMSG( DEBUG_REGISTERAPPLICATION, ( "Register Application:%s.\r\n", lpcszName ) );
	len = strlen( lpcszName ) + 1;
	lp = (PMODULE_CLASS)KHeap_Alloc( sizeof( MODULE_CLASS ) );//_kalloc( sizeof( MODULE ) + len * sizeof( TCHAR ) );
	
	if( lp )
	{	//��ʼ��MODULE_CLASS�ṹ
		memset( lp, 0, sizeof( MODULE_CLASS ) );
		lp->lpszApName = KHeap_AllocString( len * sizeof( TCHAR ) );//(LPTSTR)(lp + 1);
		
		if( lp->lpszApName )
		{
			lp->lpProc = lpApProc;
			lp->hIcon = hIcon;
			strcpy( lp->lpszApName, lpcszName );
			//����ģ������
			lp->lpNext = lpAppList;
			lpAppList = lp;
			DEBUGMSG( DEBUG_REGISTERAPPLICATION, ( "Register Application Success.\r\n" ) );//KL_DebugOutString( "Register Application Success\r\n" );
		    return TRUE;
		}
		else
			KHeap_Free( lp, sizeof( MODULE_CLASS ) );
	}
	RETAILMSG( 1, ( "Register Application Failure\r\n" ) );//KL_DebugOutString( "Register Application Failure\r\n" );
	return FALSE;
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
#define DEBUG_LOADAPPLICATION 0

BOOL WINAPI KL_LoadApplication( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine )
{
	PMODULE_CLASS lp = FindAp( lpcszName );
	DEBUGMSG( DEBUG_LOADAPPLICATION, ( "LoadApplication=%s, cmdline=%s\r\n", lpcszName, lpcszCmdLine ) );
	if(lp)
	{
#ifdef SINGLE_THREAD
		HWND hWnd = Wnd_GetDesktop();
		DEBUGMSG( DEBUG_LOADAPPLICATION, ( "Find DeskTop\r\n" ) );//KL_DebugOutString( "Find DeskTop\r\n" );
        if( hWnd )
        {
            DEBUGMSG( DEBUG_LOADAPPLICATION, ( "Call Application\r\n" ) );//KL_DebugOutString( "Call Application\r\n" );
			lp->lpProc( _GetHWNDPtr(hWnd)->hinst, NULL, lpcszCmdLine, SW_SHOW );
            return TRUE;
        }
        return FALSE;
#else        
		return DoCreateProcess( lpcszName, lpcszCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL, &qemd );
		//return LoadProcess( lp, lpcszCmdLine );
#endif
	}    
	RETAILMSG( 1, ("not find lpProc\r\n") );
    return FALSE;
}

#ifdef INLINE_PROGRAM

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
	TCHAR filePath[MAX_PATH] = "\\kingmos\\";
	HANDLE handle = NULL;

	if( lpcszName )
	{	//��Ӧ�ó����Ӧ����Դ�ļ���= lpcszName + '.res'���õ�ͼ��
	    strcat( filePath, lpcszName );
		strcat( filePath, ".res" );
	}
	else
	{	//��ϵͳ����Դ�ļ��õ�ͼ��
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
#endif

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

HICON WINAPI KL_GetApplicationIcon( LPCTSTR lpcszName, UINT fIconType )
{
#ifdef INLINE_PROGRAM

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
#else
	return NULL;
#endif
}

////////////////////////////////////////////
