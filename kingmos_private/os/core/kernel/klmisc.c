/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����Ӵչ���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
    2003-06-18�� �����ROM Filesys��֧��    
******************************************************/

#include <eframe.h>
#include <eapisrv.h>
#include <eufile.h>
#include <eugwme.h>
#include <eutable.h>
#include <epcore.h>
#include <eobjcall.h>
#include <coresrv.h>
#include <kromfs.h>
#include <epalloc.h>
#include <oemfunc.h>

#define DEBUG_FILE 0
// ********************************************************************
//������BOOL WINAPI KL_CloseFile( HANDLE hFile )
//������
//	hFile - �ļ��������CreateFile���������ؾ����
//����ֵ��
//	����ɹ�������TURE�����򣬷���FALSE
//����������
//	�ر��ļ�
//����: 
//	���ں�ʹ��
// ********************************************************************
typedef BOOL ( WINAPI * PFILE_CLOSEFILE )( HANDLE );
#define DEBUG_KL_CloseFile 0
BOOL WINAPI KL_CloseFile( HANDLE hFile )
{
	PFILE_CLOSEFILE pCloseFile;
	CALLSTACK cs;
	//CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	BOOL   retv = FALSE;
    // �õ���������ڲ��л���������̿ռ�
	if( KC_EnterAPIHandle( API_FILESYS, FILE_CLOSEFILE, &pCloseFile, &cs, &hFile ) )
	{
		retv = pCloseFile( hFile );
		// �뿪����
		KL_LeaveAPI();
	}
#ifndef INLINE_PROGRAM
	else
	{   // ROM filesys only
		HANDLE hf;

		hf = HandleToPtr( hFile, -1 );
	    DEBUGMSG( DEBUG_KL_CloseFile, ( "call builtin CloseFile,h=0x%x,p=0x%x.\r\n",hFile, hf ) );		
		
		retv = ROM_CloseFile( hf );
		//DEBUGMSG( DEBUG_KL_CloseFile, ( "builtin CloseFile--,h=0x%x.\r\n",hFile ) );		
	}
#endif
	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );

	return retv;
}

// ********************************************************************
//������HANDLE WINAPI KL_CreateFile( LPCTSTR lpcszFileName,
//							   DWORD dwAccess, 
//							   DWORD dwShareMode, 
//							   PSECURITY_ATTRIBUTES pSecurityAttributes, 
//							   DWORD dwCreate, 
//							   DWORD dwFlagsAndAttributes, 
//							   HANDLE hTemplateFile )
//
//������
//	lpcszFileName-�ļ���
//	fAccess-��ȡ���ƣ�������ֵ��λ��ϣ�
//		GENERIC_WRITE-������
//		GENERIC_READ-д����
//	dwShareMode-����ģʽ��������ֵ��λ��ϣ�
//		 FILE_SHARE_READ-�����
//		 FILE_SHARE_WRITE-����д
//	pSecurityAttributes-��ȫ����(��֧�֣�ΪNULL)
//	dwCreate-������ʽ��������
//		CREATE_NEW-�������ļ��������ļ����ڣ���ʧ��
//		CREATE_ALWAYS-�����ļ��������ļ����ڣ��򸲸���
//		OPEN_EXISTING-���ļ��������ļ������ڣ���ʧ��
//		OPEN_ALWAYS-���ļ��������ļ������ڣ��򴴽�
//	dwFlagsAndAttributes-�ļ�����
//	hTemplateFile-��ʱ�ļ����(��֧��,ΪNULL)


//����ֵ��
//	����ɹ���������Ч���ļ���������򣬷���INVALID_HANDLE_VALUE
//����������
//	���ļ�
//����: 
//	���ں�ʹ��
// ********************************************************************
#define DEBUG_KL_CreateFile 0
typedef HANDLE ( WINAPI * PFILE_CREATEFILE )( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
HANDLE WINAPI KL_CreateFile( LPCTSTR lpcszFileName,
							   DWORD dwAccess, 
							   DWORD dwShareMode, 
							   PSECURITY_ATTRIBUTES pSecurityAttributes, 
							   DWORD dwCreate, 
							   DWORD dwFlagsAndAttributes, 
							   HANDLE hTemplateFile )
{
	PFILE_CREATEFILE pCreateFile;

	CALLSTACK cs;
	//CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	HANDLE   retv = NULL;    
	
	// �õ���������ڲ��л���������̿ռ�

	if( KC_EnterAPI( API_FILESYS, FILE_CREATEFILE, &pCreateFile, &cs ) )
	{
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );
		pSecurityAttributes = MapProcessPtr( pSecurityAttributes, (LPPROCESS)cs.lpvData );
		retv = pCreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
		KL_LeaveAPI(  );
	}
#ifndef INLINE_PROGRAM
	else
	{   // ROM Filesys only
		HANDLE h;
		h = retv = ROM_CreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
		//2004-10-21, add code
		if( retv != INVALID_HANDLE_VALUE )
		{
	        retv = (HANDLE)Handle_Alloc( lpCurThread->lpCurProcess, h, OBJ_FILE );
			//retv = (HANDLE)Handle_Alloc( 0, h, OBJ_FILE );
		}
	    DEBUGMSG( DEBUG_KL_CreateFile, ( "call builtin CreateFile=%s,h=0x%x,p=0x%x,lpCurThread->lpCurProcess=0x%x.\r\n",lpcszFileName, retv, h, lpCurThread->lpCurProcess ) );
	}
#endif

	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return retv;
}

// ********************************************************************
//������DWORD WINAPI KL_GetFileAttributes( LPCTSTR lpcszFileName )
//������
//	lpcszFileName-�ļ���
//����ֵ��
//	����ɹ����������ԣ����򣬷���0xFFFFFFFF
//����������
//	�õ��ļ�����
//����: 
//	���ں�ʹ��
// ********************************************************************
#define DEBUG_KL_GetFileAttributes 0
typedef DWORD ( WINAPI * PFILE_GETATTRIBUTES )( LPCTSTR lpcszFileName );
DWORD WINAPI KL_GetFileAttributes( LPCTSTR lpcszFileName )
{
	PFILE_GETATTRIBUTES pGetAttributes;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	//CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	DWORD   retv = -1;

	if( KC_EnterAPI( API_FILESYS, FILE_GETATTRIBUTES, &pGetAttributes, &cs ) )
	{
        //RETAILMSG( 1, ( "filesys GetFileAttributes=%s.\r\n",lpcszFileName ) );
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );

		retv = pGetAttributes( lpcszFileName );
		KL_LeaveAPI(  );
	}
#ifndef INLINE_PROGRAM
	else
	{   // ROM Filesys only
		DEBUGMSG( DEBUG_KL_GetFileAttributes, ( "call builtin filesys GetFileAttributes=%s.\r\n",lpcszFileName ) );
		retv = ROM_GetFileAttributes( lpcszFileName );
	}
#endif

//	KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return retv;
}

// ********************************************************************
//������BOOL WINAPI KL_ReadFile( HANDLE hFile, 
//					   LPVOID lpvBuffer, 
//					   DWORD dwNumToRead, 
//					   LPDWORD lpdwNumRead, 
//					   LPOVERLAPPED lpOverlapped )

//������
//	hFile-�ļ����
//	lpvBuffer-���ڽ������ݵ��ڴ�
//	dwNumToRead-�������ֽ���
//	lpdwNumRead-����ʵ�ʶ����ֽ���������lpdwNumReadΪNULL,�򲻷���
//	lpOverlapped-����(��֧�֣�ΪNULL)

//����ֵ��
//	����ɹ�������TRUE��lpdwNumRead����ʵ�ʶ������ݣ����򣬷���FALSE
//����������
//	���ļ��������
//����: 
//	���ں�ʹ��
// ********************************************************************
#define DEBUG_KL_ReadFile 0
typedef BOOL ( WINAPI * PFILE_READ )( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped );
BOOL WINAPI KL_ReadFile( HANDLE hFile, 
					   LPVOID lpvBuffer, 
					   DWORD dwNumToRead, 
					   LPDWORD lpdwNumRead, 
					   LPOVERLAPPED lpOverlapped )
{
	PFILE_READ pRead;

	CALLSTACK cs;
//	CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	BOOL   retv = FALSE;

	if( KC_EnterAPIHandle( API_FILESYS, FILE_READ, &pRead, &cs, &hFile ) )
	{
		lpvBuffer = MapProcessPtr( lpvBuffer, (LPPROCESS)cs.lpvData );
		lpdwNumRead = MapProcessPtr( lpdwNumRead, (LPPROCESS)cs.lpvData );
		lpOverlapped = MapProcessPtr( lpOverlapped, (LPPROCESS)cs.lpvData );

		retv = pRead( hFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
		KL_LeaveAPI(  );
	}

#ifndef INLINE_PROGRAM
	else
	{   // ROM Filesys only
		DEBUGMSG( DEBUG_KL_ReadFile, ( "call builtin filesys ReadFile, handle=0x%x.\r\n",hFile ) );
		hFile = HandleToPtr( hFile, -1 );		
		retv = ROM_ReadFile( hFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
	}
#endif

	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return retv;
}

// ********************************************************************
//������BOOL WINAPI KL_WriteFile( HANDLE hFile, 
//					    LPCVOID lpvBuffer, 
//						DWORD dwNumToWrite, 
//						LPDWORD lpdwNumWrite, 
//						LPOVERLAPPED pOverlapped )

//������
//	hFile-�ļ����
//	lpvBuffer-��д�����ݵĴ���ڴ�
//	dwNumToWrite-��д���ֽ���
//	lpdwNumWrite-����ʵ��д���ֽ���������lpdwNumWriteΪNULL,�򲻷���
//	pOverlapped-����(��֧�֣�ΪNULL)

//����ֵ��
//	����ɹ�������TRUE��lpdwNumWrite����ʵ��д�����ݣ����򣬷���FALSE
//����������
//	д���ݵ��ļ�
//����: 
//	���ں�ʹ��
// ********************************************************************

typedef BOOL ( WINAPI * PFILE_WRITE )( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped );
BOOL WINAPI KL_WriteFile( HANDLE hFile, 
					    LPCVOID lpvBuffer, 
						DWORD dwNumToWrite, 
						LPDWORD lpdwNumWrite, 
						LPOVERLAPPED pOverlapped )
{
	PFILE_WRITE pWrite;

	CALLSTACK cs;
//	CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	BOOL   retv = FALSE;

	if( KC_EnterAPIHandle( API_FILESYS, FILE_WRITE, &pWrite, &cs, &hFile ) )
	{
		lpvBuffer = MapProcessPtr( lpvBuffer, (LPPROCESS)cs.lpvData );
		lpdwNumWrite = MapProcessPtr( lpdwNumWrite, (LPPROCESS)cs.lpvData );
		pOverlapped = MapProcessPtr( pOverlapped, (LPPROCESS)cs.lpvData );

		retv = pWrite( hFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
		KL_LeaveAPI(  );
	}
	else
	{
	}

	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return retv;
}

// ********************************************************************
//������DWORD WINAPI KL_SetFilePointer( HANDLE hFile, 
//							  LONG lDistanceToMove, 
//							  LPLONG lpDistanceToMoveHigh, 
//							  DWORD dwMoveMethod )
//������
//hFile-�ļ����
//lDistance-���ƫ��ֵ
//lpDistanceHigh-(��32bits,��֧��,ΪNULL)
//dwMethod-ƫ�Ƶ���ʼλ�ã�������
//    FILE_BEGIN-�ļ���ʼλ��
//    FILE_CURRENT-�ļ���ǰλ��
//    FILE_END-�ļ�����λ��
//����ֵ��
//	����ɹ������ط����µ��ļ�λ�ã����򣬷���0xffffffff
//����������
//	���ô�ȡ�ļ�λ��
//����: 
//	���ں�ʹ��
// ********************************************************************
typedef DWORD ( WINAPI * PFILE_SETPOINTER )( HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );
DWORD WINAPI KL_SetFilePointer( HANDLE hFile, 
							  LONG lDistanceToMove, 
							  LPLONG lpDistanceToMoveHigh, 
							  DWORD dwMoveMethod )
{
	PFILE_SETPOINTER pSetPointer;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
//	CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	BOOL   retv = FALSE;

	if( KC_EnterAPIHandle( API_FILESYS, FILE_SETPOINTER, &pSetPointer, &cs, &hFile ) )
	{
		lpDistanceToMoveHigh = MapProcessPtr( lpDistanceToMoveHigh, (LPPROCESS)cs.lpvData );
		retv = pSetPointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
		KL_LeaveAPI(  );
	}
#ifndef INLINE_PROGRAM
	else
	{
		hFile = HandleToPtr( hFile, -1 );
		retv = ROM_SetFilePointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
	}
#endif

	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return retv;
}

// ********************************************************************
//������BOOL WINAPI KL_DeviceIoControl( 
//                          HANDLE hFile,
//                          DWORD dwIoControlCode,
//                          LPVOID lpInBuf,
//                          DWORD nInBufSize,
//                          LPVOID lpOutBuf,
//                          DWORD nOutBufSize,
//                          LPDWORD lpdwBytesReturned,
//                          LPOVERLAPPED lpOverlapped )
//������
//hFile-�ļ����
//dwIoControlCode-I/O��������ļ�ϵͳ������
//    IOCTL_DISK_SCAN_VOLUME��֪ͨ�ļ�ϵͳȥɨ���Ĵ���
//    IOCTL_DISK_FORMAT_VOLUME��֪ͨ�ļ�ϵͳȥ��ʽ����
//    lpInBuf-���뻺��
//    nInBufSize�����뻺��ĳߴ�
//    lpOutBuf���������
//    nOutBufSize���������ĳߴ�
//    lpBytesReturned-���ص����ݵĳߴ磬�������ڷ���ʱ�Ѵ���lpOutBuf
//    lpOverlapped-֧�ָ���(��֧�֣�ΪNULL)
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	�ļ�����
//����: 
//	���ں�ʹ��
// ********************************************************************
typedef BOOL ( WINAPI * PFILE_DEVICEIOCONTROL )( HANDLE hFile, DWORD dwIoControlCode,LPVOID lpInBuf,DWORD nInBufSize,LPVOID lpOutBuf,DWORD nOutBufSize,LPDWORD lpdwBytesReturned,LPOVERLAPPED lpOverlapped );
BOOL WINAPI KL_DeviceIoControl( 
                          HANDLE hFile,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped )
{
	PFILE_DEVICEIOCONTROL pDeviceIoControl;

	CALLSTACK cs;
//	CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	BOOL   retv = FALSE;

	if( KC_EnterAPIHandle( API_FILESYS, FILE_DEVICEIOCONTROL, &pDeviceIoControl, &cs, &hFile ) )
	{
        lpInBuf = MapProcessPtr( lpInBuf, (LPPROCESS)cs.lpvData );
		lpOutBuf = MapProcessPtr( lpOutBuf, (LPPROCESS)cs.lpvData );
		lpdwBytesReturned = MapProcessPtr( lpdwBytesReturned, (LPPROCESS)cs.lpvData );
		lpOverlapped = MapProcessPtr( lpOverlapped, (LPPROCESS)cs.lpvData );

		retv = pDeviceIoControl( hFile, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned, lpOverlapped );
		KL_LeaveAPI(  );
	}
#ifndef INLINE_PROGRAM
	else
	{
		hFile = HandleToPtr( hFile, -1 );
		retv = ROM_DeviceIoControl( hFile, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned, lpOverlapped );
	}
#endif

	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return retv;
}
/*
////////////////////////////////////
typedef BOOL ( WINAPI * PWND_CLEAR )( DWORD );
BOOL WINAPI KL_WndClear( DWORD dwThreadID )
{
	PWND_CLEAR pClear;
	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( KC_EnterAPI( API_WND, WND_CLEAR, &pClear, &cs ) )
	{
		bRetv = pClear( dwThreadID );
		KL_LeaveAPI(  );
	}
	return bRetv;
}
/////////////////////////////
*/
/*
typedef BOOL ( CALLBACK * PCLASS_CLEAR )( HINSTANCE );
BOOL CALLBACK KL_ClearClass( HANDLE hProcess )
{
	PCLASS_CLEAR pClear;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( KC_EnterAPI( API_WND, CLASS_CLEAR, &pClear, &cs ) )
	{
		retv = pClear( hProcess );
		KL_LeaveAPI(  );
	}
	return retv;
}

typedef BOOL ( CALLBACK * PTB_CLOSEALL )( HANDLE hProcess );
BOOL CALLBACK KL_CloseAllTable( HANDLE hProcess )
{
	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( KC_EnterAPI( API_TABLE, TB_CLOSEALL, &lpfn, &cs ) )
	{
		retv = ((PTB_CLOSEALL)lpfn)( hProcess );
		KL_LeaveAPI(  );
	}
	return retv;
}

typedef void ( WINAPI * PSYS_CLOSEOBJECT )( HANDLE hProcess );
void WINAPI KL_CloseSysObject( HANDLE hProcess )
{
	PSYS_CLOSEOBJECT pfn;// = (PSYS_GETCOLORBRUSH)lpSysSetAPI[SYS_GETCOLORBRUSH];

	CALLSTACK cs;
	//RUSH retv = NULL;

	if( KC_EnterAPI( API_GWE, SYS_CLOSEOBJECT, &pfn, &cs ) )
	{
		pfn( hProcess );
		KL_LeaveAPI(  );
	}
}
*/

// ********************************************************************
//������HANDLE WINAPI KL_LoadImage( 
//                     HINSTANCE hinst,
//                     LPCTSTR lpszName, 
//                     UINT uType, 
//                     int cxDesired, 
//                     int cyDesired, 
//                     UINT fuLoad 
//                     )
//������
//	hinst - ģ����
//	lpszName - ��Ҫװ���ӳ����������hinstΪNULL����fuLoad��־����LR_LOADFROMFILE���ò�����ʾһ���ļ���������hinstΪ��NULL����fuLoad��־û��LR_LOADFROMFILE����ò�����ʾһ������Դ���ӳ��������MAKEINTRESOURCEȥת����ԴIDֵΪLPCTSTR����ֵ
//	uType - ��Ҫװ�����Դ���ͣ�����Ϊ����ֵ��
//		IMAGE_BITMAP - λͼ
//		IMAGE_CURSOR - ���
//		IMAGE_ICON - ͼ��
//	cxDesired - ��Ҫװ��Ĺ���ͼ����Դ��ȣ�������0����fuLoad����LR_DEFAULTSIZE��־��
//				��ϵͳ��SM_CXICON��SM_CXCURSORϵͳ����ȥ���ø�ֵ��������0����fuLoadû��LR_DEFAULTSIZE��־����ϵͳ����Դ����ʵ�Ŀ��
//	cyDesired - ��Ҫװ��Ĺ���ͼ����Դ�߶ȣ�������0����fuLoad����LR_DEFAULTSIZE��־��
//				��ϵͳ��SM_CXICON��SM_CXCURSORϵͳ����ȥ���ø�ֵ��������0����fuLoadû��LR_DEFAULTSIZE��־����ϵͳ����Դ����ʵ�ĸ߶�
//	fuLoad - Ϊ����ֵ��
//		LR_LOADFROMFILE - ���ļ�װ����Դ
//		LR_DEFAULTSIZE - ��Ĭ�ϵ�ϵͳ����
//		LR_SHARED - �Ƿ������Դ������ñ�־���ã��ڶ���װ�����Դ��������µĿ�����ֻ�����Ӹ���Դ�����ü��������������µĿ��������ز�ͬ�ľ��
//����ֵ��
//	����ɹ���������Ч�ľ��ֵ�����𷵻�NULL
//����������
//	װ��һ��λͼ��ͼ�ꡢ���
//����:
//	���ں�ʹ��
// ********************************************************************
typedef HANDLE ( WINAPI * PGDI_LOADIMAGE )( 
                     HINSTANCE hinst,
                     LPCTSTR lpszName, 
                     UINT uType, 
                     int cxDesired, 
                     int cyDesired, 
                     UINT fuLoad 
                     );
HANDLE WINAPI KL_LoadImage( 
                     HINSTANCE hinst,
                     LPCTSTR lpszName, 
                     UINT uType, 
                     int cxDesired, 
                     int cyDesired, 
                     UINT fuLoad 
                     )
{
	PGDI_LOADIMAGE pLoadImage;// = (PGDI_LOADIMAGE)lpGDIAPI[GDI_LOADIMAGE];

	HANDLE retv = NULL;
	CALLSTACK cs;
//	CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );

	if( KC_EnterAPI( API_GDI, GDI_LOADIMAGE, &pLoadImage, &cs ) )
	{
		lpszName = MapProcessPtr( lpszName, (LPPROCESS)cs.lpvData );

		retv = pLoadImage( hinst, lpszName, uType, cxDesired, cyDesired, fuLoad );
		KL_LeaveAPI(  );
	}

	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return retv;
}

// ********************************************************************
//������int WINAPI KL_GetSysMetrics( int nIndex )
//������
//	IN nIndex - ����ֵ
//����ֵ��
//	��������ֵ
//����������
//	����ϵͳ����
//����:
//	���ں�ʹ��
// ********************************************************************
typedef int ( WINAPI * PSYS_GETMETRICS )( int );
int WINAPI KL_GetSysMetrics( int nIndex )
{
	PSYS_GETMETRICS pGetMetrics;

	CALLSTACK cs;
//	CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	int   retv = 0;

	if( KC_EnterAPI( API_GWE, SYS_GETMETRICS, &pGetMetrics, &cs ) )
	{
		retv = pGetMetrics( nIndex );
		KL_LeaveAPI(  );
	}

	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return retv;
}


// ********************************************************************
//������BOOL WINAPI KL_DebugOutString( LPTSTR lpszStr )
//������
//	IN lpszStr - �ַ���
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	���������Ϣ
//����:
//	ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_DebugOutString( LPTSTR lpszStr )
{
//	extern void OEM_WriteDebugString( LPTSTR lpszStr );
	//FormatStr( tMsg, lpszStr );
	//RETAILMSG( 1, (tMsg) );
	OEM_WriteDebugString( lpszStr );
	return TRUE;
}

// ********************************************************************
//������LPCVOID WINAPI KL_GetSysTOC( void )
//������
//	��
//����ֵ��
//	����TOC
//����������
//	����ϵͳ��TOC��Ϣ
//����:
//	ϵͳAPI
// ********************************************************************

LPCVOID WINAPI KL_GetSysTOC( void )
{
#ifdef EML_WIN32
	return NULL;
#else
	#ifndef INLINE_PROGRAM
	    typedef struct ROMHDR;
	    extern struct ROMHDR * const pTOC;

	    return pTOC;
    #else
	    return NULL;
    #endif
#endif
}

#ifndef INLINE_PROGRAM

void __AssertFail( char *__msg, char *__cond, char *__file, int __line, char * __notify )
{	
	if( __notify )
	{
		EdbgOutputDebugString( "%s", __notify );
	}
	EdbgOutputDebugString(__msg, __cond, __file, __line);
}

#else

LPVOID WINAPI KL_OpenDllModule( LPCTSTR lpcszName, UINT uiMode, LPVOID lpvCaller )
{
	return NULL;
}
int WINAPI KL_OpenDllDependence( LPVOID lpvModule, UINT uiMode )
{
	return 0;
}

#endif


static TCHAR * lpszComputerNameNetBIOS;
BOOL InitComputerName( void )
{
	static const TCHAR szKingmos[] = "Kingmos";
	lpszComputerNameNetBIOS = (TCHAR*)KHeap_AllocString( sizeof(szKingmos) );
	if( lpszComputerNameNetBIOS )
	{
		strcpy( lpszComputerNameNetBIOS, szKingmos );
	    return TRUE;
	}
	return FALSE;
}

// ********************************************************************
//������BOOL WINAPI KL_GetComputerNameEx(
//								 COMPUTER_NAME_FORMAT NameType,  // name type
//								 LPTSTR lpBuffer,                // name buffer
//								 LPDWORD lpnSize                 // size of name buffer
//								 )
//������
//	IN NameType - ���ͣ�������
//				ComputerNameNetBIOS - NetBIOS��
//	OUT lpBuffer - ���ڽ������ݵ��ڴ�
//	IN/OUT lpnSize - ����lpBuffer�ĳ��ȣ������Ҫ�������ַ������ȣ���'\0'��������
//����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
//����������
//	�õ��������
//����:
//	ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_GetComputerNameEx(
								 COMPUTER_NAME_FORMAT NameType,  // name type
								 LPTSTR lpBuffer,                // name buffer
								 LPDWORD lpnSize                 // size of name buffer
								 )
{
	BOOL bRetv = FALSE;
	if( ComputerNameNetBIOS == NameType )
	{
		UINT len = 0;

		if( lpszComputerNameNetBIOS && 
			(len = strlen( lpszComputerNameNetBIOS )) < *lpnSize )
		{
		    strcpy( lpBuffer, lpszComputerNameNetBIOS );
			bRetv = TRUE;
		}
		*lpnSize = len + 1;
	}
	else
		KL_SetLastError( ERROR_INVALID_PARAMETER );
	return bRetv;
}


// ********************************************************************
//������BOOL WINAPI KL_SetComputerNameEx(
//								 COMPUTER_NAME_FORMAT NameType,  // name type
//								 LPCTSTR lpBuffer                // name buffer
//								 )
//������
//	IN NameType - ���ͣ�������
//				ComputerNameNetBIOS - NetBIOS��
//	IN lpBuffer - ��Ҫ���õ�����
//����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
//����������
//	���ü������
//����:
//	ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_SetComputerNameEx(
								  COMPUTER_NAME_FORMAT NameType,  // name type 
								  LPCTSTR lpBuffer                // new name buffer 
								  )
{
	BOOL bRetv = FALSE;
	if( ComputerNameNetBIOS == NameType )
	{
		int len;
		//�ͷ�֮ǰ������		
		if( lpszComputerNameNetBIOS )
			KHeap_FreeString( lpszComputerNameNetBIOS );
		len = strlen( lpBuffer ) + 1;
		//���·����ڴ�
	    lpszComputerNameNetBIOS = KHeap_AllocString( len );
	    if( lpszComputerNameNetBIOS )
		    strcpy( lpszComputerNameNetBIOS, lpBuffer );
	}
	else
		KL_SetLastError( ERROR_INVALID_PARAMETER );
	return bRetv;
}


// ********************************************************************
//������BOOL WINAPI KL_RegistryIO( DWORD dwFlag, LPVOID lpDataBuf, DWORD nNumberOfBytes );
//������
//	IN/OUT lpDataBuf - ���ڱ����д�������
//  IN nNumberOfBytes - lpDataBuf ��������
//	IN dwFlag - �������͵����:
//                   REGIO_BEGIN - ��һ�β���
//                   REGIO_READ -  ��
//                   REGIO_WRITE - д
//                   REGIO_END -   ��������
//����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
//����������
//	�������ע���
//����:
//	ϵͳAPI
// ********************************************************************

LPOEM_REGISTRYIO lpOEM_RegistryIO;

BOOL WINAPI KL_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
{
	//if( (dwFlag & REGIO_READ) && (dwFlag & REGIO_WRITE) )
	//{ //����ͬʱΪ��д
	//	KL_SetLastError( ERROR_INVALID_PARAMETER );
	//	return FALSE;
	//}

	if( lpOEM_RegistryIO )
		return lpOEM_RegistryIO( lpDataBuf, nNumberOfBytes, dwFlag );
	return FALSE;
}

// *****************************************************************
// ������VOID DumpCallStack( LPTHREAD lpThread )
// ������
//		IN lpThread - �߳̽ṹָ��
// ����ֵ��
//		��
// ����������
//		һ�����Ժ��������������ǰ��ϵͳ������
// ����:
//		�ڲ�ʹ��
// *****************************************************************

VOID DumpCallStack( LPTHREAD lpThread )
{
	if( lpThread->lpCallStack )
	{
		CALLSTACK * lpcs = lpThread->lpCallStack;
		do{
	        EdbgOutputDebugString("lpcs(0x%x),CallStack APIId(%d),OptionId(%d),RetAdr(0x%x).\r\n", 
	                               lpcs,
	                               GET_APIID( lpcs->dwCallInfo ), 
	                               GET_OPTIONID( lpcs->dwCallInfo ), 
	                               lpcs->pfnRetAdress );
	        lpcs = lpcs->lpNext;
		}while( lpcs );
	}
}

// *****************************************************************
// ������BOOL WINAPI KL_KernelIoControl(
//				   HANDLE hOpenFile,
//				   DWORD  dwIoControlCode,
//				   LPVOID lpvInBuf,
//				   DWORD dwInBufSize,
//				   LPVOID lpvOutBuf,
//				   DWORD dwOutBufSize,
//				   LPDWORD lpdwBytesReturned,
//				   LPOVERLAPPED lpOverlapped )
// ������
//		IN hOpenFile - �Ѵ��ļ����
//		IN dwIoControlCode - I/O���ƴ���
//		IN lpvInBuf - ��������ڴ��ַ
//		IN dwInBufSize - lpvInBuf���ڴ��С
//		OUT lpvOutBuf - ��������ڴ��ַ
//		IN dwOutBufSize - lpvOutBuf���ڴ��С
//		OUT lpdwBytesReturned - д��lpvOutBuf�����ݴ�С
//		IN lpOverlapped - ��֧�֣�ΪNULL��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�ں�I/O���ƺ�����ڣ�ͨ������ͬ�Ŀ��ƴ�����ʵ�ֶ��ں˵Ĺ������
// ����:
//		ϵͳAPI 
// *****************************************************************

BOOL WINAPI KL_KernelIoControl ( 
			DWORD dwIoControlCode, 
			LPVOID lpInBuf, 
			DWORD nInBufSize, 
			LPVOID lpOutBuf, 
			DWORD nOutBufSize, 
			LPDWORD lpBytesReturned )
{
	return FALSE;
}


