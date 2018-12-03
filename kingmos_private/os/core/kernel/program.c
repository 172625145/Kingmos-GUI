/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����������
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
     LN:2003.05.07, �޸�MODULE �ṹ
******************************************************/

#include <eframe.h>
#include <eprogram.h>
#include <efile.h>

#include <epalloc.h>
#include <epheap.h>
#include <eprocess.h>
#include <epcore.h>
#include <coresrv.h>


// ********************************************************************
//������ PMODULE Module_Init( 
//								LPPROCESS lpOwner, 
//								LPCTSTR lpszApplicationName,
//								PCMODULE_DRV lpmd,
//								UINT uiCreateFlag )
//������
//	IN lpOwner - ���̽ṹָ�룬ָ��ӵ����
//	IN lpszApplicationName - ������
//	IN lpmd - MODULE_DRV�ṹָ�룬ģ�������ӿ�
//	IN uiCreateFlag - ������־
//����ֵ��
//	����ɹ�������MODULE�ṹָ�룻���򣬷���NULL
//����������
//	���䲢��ʼ��MODULE�ṹָ��
//���ã�
//	LoadApplication & CreateProcess ����ȥ��ʼ��ģ��
// ********************************************************************
PMODULE Module_Init( LPPROCESS lpOwner, LPCTSTR lpszApplicationName, PCMODULE_DRV lpmd, UINT uiCreateFlag )
{
	PMODULE pModule = KHeap_Alloc( sizeof( MODULE ) );
	if( pModule )
	{	//����һ�����	
		pModule->hModule = Handle_Alloc( lpOwner, pModule, OBJ_MODULE );
		if( pModule->hModule )
		{
			//��ʼ��ģ��
			KC_InitializeCriticalSection( &pModule->csModule );
			pModule->lpmd = lpmd;
			pModule->hModuleData = lpmd->lpCreateModule( lpszApplicationName, lpOwner->hProcess, uiCreateFlag );
			if( pModule->hModuleData )
				return pModule;
		}
		//ʧ�ܣ����...
		if( pModule->hModule )
			Handle_Free( pModule->hModule, TRUE );
		KHeap_Free( pModule, sizeof( MODULE ) );
	}
	return NULL;
}

// ********************************************************************
//������void Module_DeInit( PMODULE lpModule ) 
//������
//	IN lpModule - MODULE�ṹָ�룬Ϊ֮ǰ��Module_Init���ص�
//����ֵ��
//	��
//����������
//	��Module_Init��Ӧ���ù����ͷ�ģ��
//���ã�
// ********************************************************************
void Module_DeInit( PMODULE lpModule )
{	
	lpModule->lpmd->lpFreeModule( lpModule->hModuleData );
	Handle_Free( lpModule->hModule, TRUE );
	KL_DeleteCriticalSection( &lpModule->csModule );
	KHeap_Free( lpModule, sizeof( MODULE ) );
}

// ********************************************************************
//������FARPROC WINAPI KL_GetProcAddress( HMODULE hModule, LPCTSTR lpProcName ) 
//������
//	IN hModule - ģ����
//	IN lpProcName - ������
//����ֵ��
//	����ɹ�������ģ��ĺ������õ�ַ�����򣬷���NULL
//����������
//	�õ�ģ��ĺ������õ�ַ
//���ã�
// ********************************************************************
FARPROC WINAPI KL_GetProcAddress( HMODULE hModule, LPCTSTR lpProcName )
{
	PMODULE pModule = HandleToPtr( hModule, OBJ_MODULE );
	if( pModule )
	{	// ��Ч
		return (FARPROC)pModule->lpmd->lpGetProcAddress( pModule->hModuleData,  lpProcName );
	}
	else
        KL_SetLastError( ERROR_INVALID_PARAMETER );
	return NULL;
}

// ********************************************************************
//������DWORD WINAPI KL_GetModuleFileName( 
//										HMODULE hModule,
//										LPTSTR lpFileName, 
//										DWORD nSize )
//������
//	IN hModule - ģ����
//  OUT lpFileName - �ַ���ָ�룬���ڽ���hModuleģ����ļ���
//  IN nSize - lpFileNameָ����ڴ泤��
//����ֵ��
//	���ؿ�����lpFileName���ַ���
//����������
//	�õ�ģ���ļ���
//���ã�
//	ϵͳAPI
// ********************************************************************
DWORD WINAPI KL_GetModuleFileName( 
							   HMODULE hModule,
							   LPTSTR lpFileName, 
							   DWORD nSize )
{
	PMODULE pModule;
	if( hModule )
	{
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	else
	{	//�õ�ǰ����ģ��
		pModule = GetAPICallerProcessPtr()->pModule;
	}

	if( pModule )
	{
		LPCTSTR lpszfn = pModule->lpmd->lpGetModuleFileName( pModule->hModuleData );
		if( lpszfn )
		{
			strncpy( lpFileName, lpszfn, nSize );
			return strlen( lpFileName );
		}
	}
	else
        KL_SetLastError( ERROR_INVALID_PARAMETER );

	return 0;
}

// ********************************************************************
//������ HMODULE WINAPI KL_LoadLibraryEx( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags )
//������
//	IN lpFileName �C ��Ҫ���ص�ģ���ļ���
//	IN hFile �C �ļ����������������ΪNULL��
//	IN dwFlags �C ����������Ϊ0
//����ֵ��
//	����ɹ���������Ч�ľ�������򣬷���NULL
//����������
//	����һ��ģ�鵽��ǰ���̿ռ�
//���ã�
//	ϵͳAPI
// ********************************************************************
HMODULE WINAPI KL_LoadLibraryEx( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags )
{
#ifdef INLINE_PROGRAM
	return NULL;
#else
	extern const MODULE_DRV ElfModule;
	extern const MODULE_DRV qemd;
    extern DWORD WINAPI KL_GetFileAttributes( LPCTSTR lpcszFileName );
	extern BOOL IsQEModuleExist( LPCTSTR lpcszApName );
	BOOL bIsELF = FALSE;	
/*

	if( hFile == NULL )
	{
		if( IsQEModuleExist( lpFileName ) == FALSE )
		{   // qe-module is not existing,  elf format
			bIsELF = TRUE;
		}

	}
	else
*/
		bIsELF = TRUE;
	if( bIsELF )
	{	// elf�ļ���ʽ
		DWORD dwInitFlag = -1;// ����Ϊ��Ч
		int len = strlen( lpFileName );
		//�� .exe �ļ� ���� .so�ļ�
		if( len >= 5 && stricmp( &lpFileName[len-4], ".exe" ) == 0 )
			dwInitFlag = CREATE_EXE; // .exe �ļ�
		else if( len >= 4 && stricmp( &lpFileName[len-3], ".so" ) == 0 )
		    dwInitFlag = CREATE_LIBRARY; // .so �ļ�
		else
		{	// �Ƿ��ļ����б������
			while( len )
			{
				if( lpFileName[len-1] == '.' )
					break;
				len--;
			}
			// ���û�У�Ĭ�ϸ��ļ�Ϊ .so�ļ�����
			if( len == 0 )
			{  // no ext, default is .so
				dwInitFlag = CREATE_LIBRARY;
			}
		}
		// dwInitFlag ��־�Ƿ���Ч
		if( dwInitFlag != -1 )
		{
			PMODULE pModule = Module_Init( GetAPICallerProcessPtr(), lpFileName, &ElfModule, dwInitFlag ); 
			if( pModule )
			{
				if( dwInitFlag == CREATE_EXE )
				{	//����ִ��ģ��װ���ڴ�
				    ElfModule.lpLoadModule( pModule->hModuleData, pModule->hModule, NULL, LF_LOAD_TO_MEM );
				}
				else //����̬���ӿ�װ�뵱ǰ���̿ռ�
					ElfModule.lpLoadModule( pModule->hModuleData, pModule->hModule, NULL, LF_DEFLOAD );

				return pModule->hModule;
			}
		}
		else
	        KL_SetLastError( ERROR_INVALID_PARAMETER );

	}
#endif 
	return NULL;

}

// ********************************************************************
//������ BOOL WINAPI KL_FreeLibrary( HMODULE hModule )
//������
//	IN hModule - ģ����
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	��KL_LoadLibraryEx��Ӧ���ù����ͷ�ģ��
//���ã�
//	ϵͳAPI
// ********************************************************************
BOOL WINAPI KL_FreeLibrary( HMODULE hModule )
{
	PMODULE pModule = NULL;
	// ����Ƿ���Ч
	if( hModule )
	{
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	if( pModule )
	{
		Module_DeInit( pModule ); // �ͷ�ģ��
		return TRUE;
	}
	else
		KL_SetLastError( ERROR_INVALID_PARAMETER );
	return FALSE;
}

// ********************************************************************
//������ HMODULE WINAPI KL_GetModuleHandle( LPCTSTR lpModuleName )
//������
//	IN lpModuleName - ģ����������ΪNULL�����Ǿ͵��ڷ��ص�ǰ���̵�ģ������
//����ֵ��
//	ģ����
//����������
//	�õ��Ѿ������ص����̿ռ��ģ��
//���ã�
// ********************************************************************
#define DEBUG_GET_MODULE_HANDLE 0
HMODULE WINAPI KL_GetModuleHandle( LPCTSTR lpModuleName )
{
	DEBUGMSG( DEBUG_GET_MODULE_HANDLE, ( "KL_GetModuleHandle=0x%x.\r\n", lpModuleName ? NULL : GetAPICallerProcessPtr()->pModule->hModule ) );
	if( lpModuleName )
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return NULL;
	}
	else
		return GetAPICallerProcessPtr()->pModule->hModule;
}

// ********************************************************************
//������ HANDLE WINAPI KL_GetModuleFileHandle( HMODULE hModule )
//������
//	IN hModule - ģ����
//����ֵ��
//	�ļ����
//����������
//	�õ�ģ�����򿪵��ļ����
//���ã�
//	ϵͳAPI
// ********************************************************************
#define DEBUG_GET_MODULE_FILE_HANDLE 0
HANDLE WINAPI KL_GetModuleFileHandle( HMODULE hModule )
{
	PMODULE pModule;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	// ���Ϸ���
	if( hModule )
	{   // ��Ч��ģ��
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	else
	{   // ��ǰ����ģ��
		pModule = GetAPICallerProcessPtr()->pModule;
	}

	if( pModule )
	{   // �����ļ����
		hFile = pModule->lpmd->lpGetModuleFileHandle( pModule->hModuleData );
		if( hFile == NULL )
		{
			hFile = INVALID_HANDLE_VALUE;
		}
		else
		{   // ��ס���
			KL_EnterCriticalSection( &pModule->csModule );
		}
	}
	else
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_GET_MODULE_FILE_HANDLE, ( "error at KL_GetModuleFileHandle invalid hModule(0x%x).\r\n", hModule ) );
	}
	DEBUGMSG( DEBUG_GET_MODULE_FILE_HANDLE, ( "KL_GetModuleFileHandle return (0x%x).\r\n", hFile ) );

	return hFile;
}

// ********************************************************************
//������ BOOL WINAPI KL_ReleaseModuleFileHandle( HMODULE hModule, HANDLE hFile )
//������
//	IN hModule - ģ����
//	IN hFile - �ļ����
//����ֵ��
//	�ɹ�������TRUE; ʧ�ܣ�����FALSE
//����������
//	�ͷŸ�ģ��ľ��(����)
//���ã�
//	ϵͳAPI
// ********************************************************************
#define DEBUG_RELEASE_MODULE_FILE_HANDLE 0
BOOL WINAPI KL_ReleaseModuleFileHandle( HMODULE hModule, HANDLE hFile )
{
	BOOL bRetv = FALSE;
	PMODULE pModule;
	
	// ���Ϸ���
	if( hModule )
	{   // ��Ч��ģ��
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	else
	{   // ��ǰ����ģ��
		pModule = GetAPICallerProcessPtr()->pModule;
	}

	if( pModule )
	{   // �����ļ����
		HANDLE hf;
		hf = pModule->lpmd->lpGetModuleFileHandle( pModule->hModuleData );
		if( hf == hFile )
		{	// �ͷž��
			KL_LeaveCriticalSection( &pModule->csModule );
			bRetv = TRUE;
		}
	}

	if( bRetv == FALSE )
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_RELEASE_MODULE_FILE_HANDLE, ( "error at KL_ReleaseModuleFileHandle invalid hModule(0x%x),hFile(0x%x).\r\n", hModule, hFile ) );
	}

	return bRetv;
}

// ********************************************************************
//������ DWORD WINAPI KL_GetModuleSectionOffset( HMODULE hModule, LPCTSTR lpcszSectionName )
//������
//	IN hModule - ģ����
//  IN lpcszSectionName - ����
//����ֵ��
//	����ɹ�������ģ��Ķ���ģ���ļ����ƫ�ƣ����򣬷���-1
//����������
//	�õ�ģ��Ķ���ģ���ļ����ƫ�ƣ��������ָģ���ļ���������֯��̬
//���ã�
//	ϵͳAPI
// ********************************************************************
DWORD WINAPI KL_GetModuleSectionOffset( HMODULE hModule, LPCTSTR lpcszSectionName )
{
	PMODULE pModule;

	// ���Ϸ���

	if( hModule )
	{   // ��Ч��ģ��
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	else
	{   // ��ǰ����ģ��
		pModule = GetAPICallerProcessPtr()->pModule;
	}

	if( pModule )
	{
		return pModule->lpmd->lpGetModuleSectionOffset( pModule->hModuleData, lpcszSectionName );
	}
	else
		KL_SetLastError( ERROR_INVALID_PARAMETER );
	return -1;
}

// ********************************************************************
//������VOID WINAPI KL_FreeLibraryAndExitThread( HMODULE hModule, DWORD dwExitCode ) 
//������
//	IN hModule - ģ����
//	IN dwExitCode - �˳�����
//����ֵ��
//	
//����������
//	
//���ã�
// ********************************************************************
VOID WINAPI KL_FreeLibraryAndExitThread( HMODULE hModule, DWORD dwExitCode )
{
	KL_ExitThread(0);
}

// ********************************************************************
//������ 
//������
//	IN
//����ֵ��
//	
//����������
//	
//���ã�
// ********************************************************************
BOOL WINAPI KL_DisableThreadLibraryCalls( HMODULE hModule )
{
	return FALSE;
}


////////////////////////////////////////////
