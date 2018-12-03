/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ļ�ϵͳ�����ý���

�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <efile.h>
#include <edevice.h>
#include <eapisrv.h>

#include <eobjlist.h>
#include <efsdmgr.h>
#include <epdev.h>
#include <epfs.h>
#include <filesrv.h>


static const TCHAR sysVolume[] = "\\disk0";

#define SPECIAL_ATTRIB (FILE_ATTRIBUTE_DEVICE|FILE_ATTRIBUTE_SPARSE_FILE|FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_COMPRESSED|FILE_ATTRIBUTE_ENCRYPTED|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_ROM)
#define FSMAIN_ZONE 0

// **************************************************
// ������_LPFILEDATA _GetHFILEPtr( HANDLE hFile )
// ������
//		IN hFile - �ļ�����
// ����ֵ��
//		������Ч�����ط�NULL�ļ�����ָ�룻���򣬷���NULL
// ����������
//		��֤�ļ��������Ч�Բ�������Ч��ָ��
// ����: 
//		
// **************************************************

static _LPFILEDATA _GetHFILEPtr( HANDLE hFile )
{
	if( hFile == 0 || hFile == INVALID_HANDLE_VALUE )
	    return NULL;
	return (_LPFILEDATA)hFile;
}

// **************************************************
// ������_LPFINDDATA _GetHFINDPtr( HANDLE hfd )
// ������
//		IN hfd - �ļ����Ҷ���
// ����ֵ��
//		������Ч�����ط�NULL�ļ����Ҷ���ָ�룻���򣬷���NULL
// ����������
//		��֤�ļ��������Ч�Բ�������Ч��ָ��
// ����: 
//		
// **************************************************

_LPFINDDATA _GetHFINDPtr( HANDLE hfd )
{
	if( hfd && hfd != INVALID_HANDLE_VALUE )
	{
		_LPFINDDATA lpfd = (_LPFINDDATA)hfd;
		if( lpfd->obj.objType == OBJ_FINDFILE )
			return lpfd;  // ��Ч
	}
    return NULL;
}

// **************************************************
// ������static int CheckFilePathName( LPCTSTR lpfpn )
// ������
//		IN lpfpn - �ļ���
// ����ֵ��
//		����ɹ��������ļ����ĳ��ȣ����򣬷���0
// ����������
//		��֤�ļ�������Ч��
// ����: 
//		
// **************************************************

static int CheckFilePathName( LPCTSTR lpfpn )
{
	if( lpfpn )
	{
		int iMaxLen;
		iMaxLen = strlen( lpfpn );
		if( iMaxLen < MAX_PATH )
			return iMaxLen;
	}
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}

// **************************************************
// ������static int _GetObjName( LPCTSTR lpfn, UINT * lpNameLen )
// ������
//		IN lpfn - ������·����
//		OUT lpNameLen - ���ڽ��ܴ洢�������ĳ���
// ����ֵ��
//		���ش洢��������ͣ�����Ϊ����ֵ��
//			0 - ��֪��
//			1 - �豸���ͣ����磺"COM2:"
//			2 - �ļ����ͣ����磺"\\folder\\test.txt"
//
// ����������
//		�õ�����������
// ����: 
//		
// **************************************************

static int _GetObjName( LPCTSTR lpfn, UINT * lpNameLen )
{
    int type = 0;
    int i;

    *lpNameLen = 0;
    for( i = 0; i < 5 && *lpfn && *lpfn != '\\'; i++ )
        lpfn++;
    if( i == 5 && *(lpfn-1) == ':' )
    {
        type = 1;   // �豸���ͣ�device type
        *lpNameLen = 4;
    }
    else
    {
        if( *lpfn != '\\' )
            return 0; // ����
        lpfn++;
        i++;
        while( *lpfn && *lpfn != '\\' )
        {
            i++;
            lpfn++;
        }
        type = 2;   // �ļ����� file type         
        *lpNameLen = i;
    }
    return type;
}

// **************************************************
// ������static _LPVOLUME _GetVolumePtr( LPCTSTR lpcszFileName, UINT * pLen )
// ������
//		IN lpcszFileName - �����������ļ�ָ�루����ΪNULL,��ʱ������ϵͳĬ�Ͼ������磺"\\storage disk\\filename.ext"
//		IN/OUT pLen - ��������,��lpcszFileNameΪNULLʱ�����Ĭ�Ͼ����ĳ���
// ����ֵ��
//		��
// ����������
//		���ݾ����õ�������ǰϵͳ���ڵľ����ָ�룬�������ΪNULL����õ�ϵͳĬ�Ͼ�
//		ϵͳĬ�Ͼ���һ��������
// ����: 
//		
// **************************************************

static _LPVOLUME _GetVolumePtr( LPCTSTR lpcszFileName, UINT * pLen )
{
    _LPVOLUME lpVol = NULL;
	if( lpcszFileName[*pLen] == '\\' )  // ����Ƿ�Ϸ�
        lpVol= (_LPVOLUME)_FileMgr_FindVolume( lpcszFileName, *pLen );
    if( lpVol == NULL )  
    {	// ��ϵͳĬ�Ͼ�use sysVolume
        lpVol= (_LPVOLUME)_FileMgr_FindVolume( sysVolume, 6 );
        *pLen = 0;
    }
    return lpVol;
}

// **************************************************
// ������static BOOL _CopyFile( HANDLE hFileDest, HANDLE hFileSrc )
// ������
//		IN hFileDest - Ŀ���ļ�
//		IN hFileSrc - Դ�ļ�
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		��Դ�ļ����ݿ�����Ŀ���ļ�
// ����: 
//		
// **************************************************

#define MAX_COPY_CACHE_SIZE (32 * 1024)
#define COPY_FILE_ZONE 1
static BOOL _CopyFile( HANDLE hFileDest, HANDLE hFileSrc )
{
    int unit = MAX_COPY_CACHE_SIZE;
	DWORD dwSize;
	void *p, * lpBuf = NULL;
	BOOL bRetv = FALSE;
	
	ASSERT( hFileDest && hFileSrc );
	while( lpBuf == NULL && unit >= 128 )
	{
		DWORD dwRealAllocSize;
		lpBuf = Page_AllocMem( unit, &dwRealAllocSize, 0 );  // ������ҳΪ��λ��
		if( lpBuf )
		{	// ����ɹ�
			unit = dwRealAllocSize;
			break;
		}
		unit >>= 1; //���䲻�ɹ����ý�С�Ĵ�С����
	}
	
	if( lpBuf )
	{
		dwSize = FileSys_GetSize( hFileSrc, NULL );
		if( FileSys_SetPointer( hFileDest, dwSize, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
			goto _error;
		// ��Ŀ���ļ��Ĵ�С��Ϊ��Դ�ļ���С��ͬ
		if( FileSys_SetEnd( hFileDest ) == FALSE )
			goto _error;
		FileSys_SetPointer( hFileDest, 0, NULL, FILE_BEGIN );
		p = lpBuf;
		// ��ο����ļ�����
		while( 1 )
		{
			if( (bRetv = FileSys_Read( hFileSrc, p, unit, &dwSize, NULL )) == TRUE  )
			{
				if( dwSize == 0 )
					break;  // end of file
				else
					FileSys_Write( hFileDest, p, dwSize, &dwSize, NULL );
			}
			else
				break;
		}
_error:
		Page_FreeMem( lpBuf, unit ); // �ͷ�֮ǰ����Ļ���
	}
	else
	{
		DEBUGMSG( FSMAIN_ZONE, ( "no enough memory when CopyFile.\r\n" ) );
	}
	return bRetv;
}

// **************************************************
// ������static BOOL IsSystemProtected( LPCTSTR lpcszPathName )
// ������
//		IN lpcszPathName - �ļ�·����
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		����ļ��Ƿ���ϵͳԤ������
// ����: 
//		
// **************************************************

static BOOL IsSystemProtected( LPCTSTR lpcszPathName )
{  // disk0~disk9 if protected by system
	int uiLen = strlen( lpcszPathName );
	if( *lpcszPathName == '\\' )
	{
		lpcszPathName++;
		uiLen--;
	}
	if( uiLen == 5 )
	{
	    if( lpcszPathName[4] >= '0' && lpcszPathName[4] <= '9' && 
			strnicmp( "disk", lpcszPathName, 4 ) == 0 )
		{
			SetLastError( ERROR_ACCESS_DENIED );
			return TRUE;
		}
	}
	return FALSE;
}


// **************************************************
// ������BOOL WINAPI KL_CreateDirectory(
//										LPCTSTR lpcszPathName, 
//										PSECURITY_ATTRIBUTES pSecurityAttributes )
// ������
//		IN lpcszPathName - ·����
//		IN pSecurityAttributes - ��ȫ����(��֧�֣�ΪNULL)
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�����ļ���
// ����: 
//		�ڲ�ʹ��
// **************************************************

BOOL WINAPI KL_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes)
{
    int type;
    UINT uLen;
	
	if( CheckFilePathName( lpcszPathName ) )	// ����ļ����Ƿ�Ϸ�
	{
		type  = _GetObjName( lpcszPathName, &uLen );	// �õ���������
		if( type == 2 )
		{   // �ļ����� file
			// �õ����ļ���Ӧ�ľ����ָ��
			_LPVOLUME lpVol = _GetVolumePtr( lpcszPathName, &uLen );  
			if( lpVol && lpVol->lpdsk->lpfsd->lpCreateDirectory )
			{
				if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
				{
					BOOL bRetv;
					bRetv = lpVol->lpdsk->lpfsd->lpCreateDirectory( lpVol->pVol, lpcszPathName + uLen, pSecurityAttributes );
					Sys_ReleaseException();
					return bRetv;
				}
			}
		}
	}
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI FileSys_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes)
// ������
//		IN lpcszPathName - ·����
//		IN pSecurityAttributes - ��ȫ����(��֧�֣�ΪNULL)
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�����ļ���
// ����: 
//		ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes)
{
	if( IsSystemProtected( lpcszPathName ) ) // ���ļ����Ƿ��Ѿ������� ��
		return FALSE;	// �ǣ������������ش���
	return KL_CreateDirectory( lpcszPathName, pSecurityAttributes );	// 
}

// **************************************************
// ������static _LPFILEDATA _CreateFile( 
//							LPCTSTR lpcszFileName, 
//							DWORD dwAccess, 
//							DWORD dwShareMode, 
//							PSECURITY_ATTRIBUTES pSecurityAttributes, 
//							DWORD dwCreate, 
//							DWORD dwFlagsAndAttributes, 
//							HANDLE hTemplateFile )
// ������
//		IN lpcszFileName-�ļ���
//		IN fAccess-��ȡ���ƣ�������ֵ��λ��ϣ�
//				GENERIC_WRITE-������
//				GENERIC_READ-д����
//		IN wShareMode-����ģʽ��������ֵ��λ��ϣ�
//				FILE_SHARE_READ-�����
//				FILE_SHARE_WRITE-����д
//      IN pSecurityAttributes-��ȫ����(��֧�֣�ΪNULL)
//		IN dwCreate-������ʽ��������
//				CREATE_NEW-�������ļ��������ļ����ڣ���ʧ��
//				CREATE_ALWAYS-�����ļ��������ļ����ڣ��򸲸���
//				OPEN_EXISTING-���ļ��������ļ������ڣ���ʧ��
//				OPEN_ALWAYS-���ļ��������ļ������ڣ��򴴽�
//      IN dwFlagsAndAttributes-�ļ�����
//		IN hTemplateFile-��ʱ�ļ����(��֧��,ΪNULL)
// ����ֵ��
//		�ļ��ṹָ��
// ����������
//		�����ļ�
// ����: 
//		�ڲ�ʹ��
// **************************************************

static _LPFILEDATA _CreateFile( 
						LPCTSTR lpcszFileName, 
						DWORD dwAccess, 
						DWORD dwShareMode, 
						PSECURITY_ATTRIBUTES pSecurityAttributes, 
						DWORD dwCreate, 
						DWORD dwFlagsAndAttributes, 
						HANDLE hTemplateFile )
{
    int type; 
    UINT uLen;

    _LPFILEDATA lpfd;

	DEBUGMSG( FSMAIN_ZONE, ( "_CreateFile.\r\n" ) );

	lpfd = (_LPFILEDATA)malloc( sizeof( _FILEDATA ) ); // �����ļ�����ṹ

    if( lpfd == NULL )
		return INVALID_HANDLE_VALUE;

	lpfd->pFile = (PFILE)INVALID_HANDLE_VALUE;  // �ȳ�ʼ��Ϊ��Ч��ֵ

    type  = _GetObjName( lpcszFileName, &uLen );	// �õ���������
    if( type == 1 )
    {   // �豸���� device
		lpfd->hVol = 0;
		lpfd->pFile = (DWORD)Dev_CreateFile( 
								lpcszFileName,
								dwAccess,
								dwShareMode,
								GetCallerProcess() 	);
    }
    else if( type == 2 )
    {   // �ļ����� file
        _LPVOLUME lpVol = _GetVolumePtr( lpcszFileName, &uLen ); // �õ������

		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			if( lpVol && lpVol->lpdsk->lpfsd->lpCreateFile )
			{
				lpfd->hVol = (HVOL)lpVol;
				lpfd->pFile = (PFILE)lpVol->lpdsk->lpfsd->lpCreateFile( 
														lpVol->pVol, 
														GetCallerProcess(),
														lpcszFileName + uLen,
														dwAccess, 
														dwShareMode, 
														pSecurityAttributes,
														dwCreate, 
														dwFlagsAndAttributes,
														hTemplateFile );
			}
			Sys_ReleaseException();
		}
    }
	if( lpfd->pFile != (DWORD)INVALID_HANDLE_VALUE )
	{	// �����ļ��ɹ�
		return lpfd;
	}
	else
	{	// ���ɹ����ͷ�֮ǰ�����ָ��
		free( lpfd );
		return INVALID_HANDLE_VALUE;
	}    
}

// **************************************************
// ������HANDLE WINAPI FileSys_CreateFile( 
//						LPCTSTR lpcszFileName, 
//						DWORD dwAccess, 
//						DWORD dwShareMode, 
//						PSECURITY_ATTRIBUTES pSecurityAttributes, 
//						DWORD dwCreate, 
//						DWORD dwFlagsAndAttributes, 
//						HANDLE hTemplateFile )
// ������
//		IN lpcszFileName-�ļ���
//		IN dwAccess-��ȡ���ƣ�������ֵ��λ��ϣ�
//				GENERIC_WRITE-������
//				GENERIC_READ-д����
//		IN dwShareMode-����ģʽ��������ֵ��λ��ϣ�
//				FILE_SHARE_READ-�����
//				FILE_SHARE_WRITE-����д
//      IN pSecurityAttributes-��ȫ����(��֧�֣�ΪNULL)
//		IN dwCreate-������ʽ��������
//				CREATE_NEW-�������ļ��������ļ����ڣ���ʧ��
//				CREATE_ALWAYS-�����ļ��������ļ����ڣ��򸲸���
//				OPEN_EXISTING-���ļ��������ļ������ڣ���ʧ��
//				OPEN_ALWAYS-���ļ��������ļ������ڣ��򴴽�
//      IN dwFlagsAndAttributes-�ļ�����
//		IN hTemplateFile-��ʱ�ļ����(��֧��,ΪNULL)
// ����ֵ��
//		�ɹ������ؾ��
//		ʧ�ܣ����� INVALID_HANDLE_VALUE
// ����������
//		�����ļ�
// ����: 
//		
// **************************************************
#define DEBUG_FileSys_CreateFile 0
HANDLE WINAPI FileSys_CreateFile( LPCTSTR lpcszFileName, 
					   DWORD dwAccess, 
					   DWORD dwShareMode, 
					   PSECURITY_ATTRIBUTES pSecurityAttributes, 
					   DWORD dwCreate, 
					   DWORD dwFlagsAndAttributes, 
					   HANDLE hTemplateFile )
{
	_LPFILEDATA lpFile;

	// ������ check param  
    if( dwShareMode & ( ~( FILE_SHARE_READ | FILE_SHARE_WRITE ) ) )
        goto RET_INVALIDPARAM;
    if( dwAccess & ( ~( GENERIC_READ | GENERIC_WRITE | GENERIC_DATA ) ) )
        goto RET_INVALIDPARAM;
	if( !( dwCreate == CREATE_NEW ||
		  dwCreate == CREATE_ALWAYS ||
		  dwCreate == OPEN_ALWAYS ||
		  dwCreate == OPEN_EXISTING ||
		  dwCreate == TRUNCATE_EXISTING ) )
		goto RET_INVALIDPARAM;
	if( dwCreate == TRUNCATE_EXISTING && 
		!( dwAccess & GENERIC_WRITE ) )
	{
		SetLastError( ERROR_ACCESS_DENIED );
		return INVALID_HANDLE_VALUE;
	}
    if( dwFlagsAndAttributes & FILE_ATTRIBUTE_DIRECTORY )
		goto RET_INVALIDPARAM;
	if( (BYTE)dwFlagsAndAttributes == FILE_ATTRIBUTE_NORMAL )
        dwFlagsAndAttributes &= ~0xFFFF;
    else
        dwFlagsAndAttributes &= ~FILE_ATTRIBUTE_NORMAL;
    dwFlagsAndAttributes |= FILE_ATTRIBUTE_ARCHIVE;
	// ����ļ���
	if( CheckFilePathName( lpcszFileName ) )// LN 2003-05-23, ADD
    {	// ��Ч
		lpFile = _CreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
		if( lpFile != INVALID_HANDLE_VALUE )
		{	// �����ļ��ɹ�������һ�����
			return Sys_AllocAPIHandle( API_FILESYS, lpFile, OBJ_FILE );
		}
	}
	DEBUGMSG( FSMAIN_ZONE | DEBUG_FileSys_CreateFile, ( "CreateFile0(=%s) failure.\r\n" , lpcszFileName ) );
	return INVALID_HANDLE_VALUE;
RET_INVALIDPARAM:
	DEBUGMSG( FSMAIN_ZONE, ( "CreateFile1(=%s) failure.\r\n" , lpcszFileName ) );
    SetLastError( ERROR_INVALID_PARAMETER );
	return INVALID_HANDLE_VALUE;
}

// **************************************************
// ������static BOOL _CloseFile( _LPFILEDATA lpFile )
// ������
//		IN lpFile - �ļ�����ָ��
// ����ֵ��
//		�ɹ�������TRUE
//		ʧ�ܣ�����FALSE
// ����������
//		�ر��ļ�
// ����: 
//		
// **************************************************

static BOOL _CloseFile( _LPFILEDATA lpFile )
{
	BOOL bRetv = FALSE;

	if( lpFile->hVol )
	{   // �ļ����� file            
		_LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;

		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			if( lpVol->lpdsk->lpfsd->lpCloseFile )
				bRetv = lpVol->lpdsk->lpfsd->lpCloseFile( lpFile->pFile );
			Sys_ReleaseException();
		}
	}
	else
	{   // �豸���� device
		bRetv = Dev_CloseFile( (HANDLE)lpFile->pFile );
	}
	free( lpFile );  
	return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_CloseFile( HANDLE hFile )
// ������
// 	IN hFile - �ļ�����
// ����ֵ��
//	����ɹ�������TRUE; ʧ�ܣ�����FALSE
// ����������
//	�ر��ļ�����
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_CloseFile( HANDLE hFile )
{
    _LPFILEDATA lpFile;

	lpFile = _GetHFILEPtr( hFile );
	if( lpFile )
	{
		return _CloseFile( lpFile );
	}
	return FALSE;
}

// **************************************************
// ������BOOL WINAPI FileSys_DeleteAndRename( 
//                              LPCTSTR lpszNewFileName, 
//                              LPCTSTR lpszOldFileName )
// ������
// 	IN lpcszNewFileName �C Ŀ���ļ�
// 	IN lpcszOldFileName �C Դ�ļ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��Դ�ļ����ݿ�����Ŀ���ļ�����ɾ��Դ�ļ�
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_DeleteAndRename( LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName )
{
    int type;
    UINT uLen;
	// ��������Ƿ���Ч
	if( CheckFilePathName( lpcszNewFileName ) && CheckFilePathName( lpcszOldFileName ) )
	{		
		type  = _GetObjName( lpcszOldFileName, &uLen );  // �õ���������
		if( type == 2 )
		{   // �ļ����� file
			UINT ul = uLen;
			// �õ������
			_LPVOLUME lpVol = _GetVolumePtr( lpcszOldFileName, &uLen );
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				BOOL bRetv = FALSE;
				if( lpVol && 
					lpVol == _GetVolumePtr( lpcszNewFileName, &ul ) && 
					lpVol->lpdsk->lpfsd->lpDeleteAndRenameFile )
					bRetv = lpVol->lpdsk->lpfsd->lpDeleteAndRenameFile( lpVol->pVol, lpcszNewFileName + uLen, lpcszOldFileName + uLen );
				Sys_ReleaseException();
				return bRetv;
			}
		}
	}
    return FALSE;
}

// **************************************************
// ������BOOL FileSys_Delete(
//                    LPCTSTR lpszFileName )
// ������
//	IN lpszFileName - �ļ���
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	ɾ���ļ�
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_Delete(  LPCTSTR lpcszFileName )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;

    if( CheckFilePathName( lpcszFileName ) )  // ����ļ���
	{
		type  = _GetObjName( lpcszFileName, &uLen );	// �õ��ļ�����
		if( type == 2 )
		{   // �ļ����� file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszFileName, &uLen ); // �õ������
			
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpDeleteFile )
				{
					bRetv = lpVol->lpdsk->lpfsd->lpDeleteFile( lpVol->pVol, lpcszFileName + uLen );
				}
				Sys_ReleaseException();
			}
		}
	}
    return bRetv;
}

//typedef BOOL (*SHELLFILECHANGEFUNC)( LPFILECHANGEINFO );
//static BOOL ShellFileNotify( LPFILECHANGEINFO lpfci )
//{
  //  FILECHANGEINFO fci = *lpfci;
//}

// **************************************************
// ������BOOL WINAPI FileSys_DeviceIoControl(
//							HANDLE hFile,
//							DWORD dwIoControlCode,
//							LPVOID lpInBuf,
//							DWORD nInBufSize,
//							LPVOID lpOutBuf,
//							DWORD nOutBufSize,
//							LPDWORD lpBytesReturned
//							)
// ������
//	IN hFile-�ļ�����
//	IN dwIoControlCode-I/O��������ļ�ϵͳ������
//			IOCTL_DISK_SCAN_VOLUME��֪ͨ�ļ�ϵͳȥɨ���Ĵ���
//			IOCTL_DISK_FORMAT_VOLUME��֪ͨ�ļ�ϵͳȥ��ʽ����
//	IN lpInBuf-���뻺��
//	IN nInBufSize-���뻺��ĳߴ�
//	OUT lpOutBuf-�������
//	IN nOutBufSize-�������ĳߴ�
//	OUT lpBytesReturned-���ص����ݵĳߴ磬�������ڷ���ʱ�Ѵ���lpOutBuf
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�ļ�ϵͳ���ƹ���
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_DeviceIoControl( HANDLE hFile, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpdwBytesReturned, LPOVERLAPPED lpOverlapped )
{
	_LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	
	if( lpFile )
	{
		if( lpFile->hVol )
		{   // �ļ����� file
			_LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			BOOL bRetv = FALSE;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpDeviceIoControl )
					bRetv = lpVol->lpdsk->lpfsd->lpDeviceIoControl( lpFile->pFile, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned );
				Sys_ReleaseException();
				return bRetv;
			}
		}
		else
		{   // �豸���� device
			return Dev_IoControl( (HANDLE)lpFile->pFile, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned, lpOverlapped );
		}
	}
    return FALSE;
}

// **************************************************
// ������BOOL FileSys_FindClose( HANDLE hFind )
// ������
// 	IN hFind - ���Ҷ���
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�رղ��Ҷ���
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_FindClose( HANDLE hFind )
{
    _LPFINDDATA lpFind = _GetHFINDPtr( hFind );

    if( lpFind )
    {
        if( lpFind->hVol )
        {
            _LPVOLUME lpVol = (_LPVOLUME)lpFind->hVol;
			BOOL bRetv = FALSE;

			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpFindClose )
				{
					if( lpVol->lpdsk->lpfsd->lpFindClose( lpFind->pFind ) )
					{
						bRetv = TRUE;
					}
				}
				
				FileMgr_CloseSearchHandle( hFind );
				Sys_ReleaseException();

				return bRetv;
			}
        }
		FileMgr_CloseSearchHandle( hFind );
    }
	
    return FALSE;
}

// **************************************************
// ������HANDLE WINAPI FileSys_FindFirst( 
//                         LPCTSTR lpszFileSpec, 
//                         FILE_FIND_DATA * pfd )
// ������
// 	IN lpszFileSpec - �ļ��������Ժ�ͨ���ַ���
//	OUT pfd - FILE_FIND_DATA���ݽṹָ�룬���ڽ����ҵ�������
// ����ֵ��
//	�ɹ���������Ч�ľ��
//	���򣺷���INVALID_HANDLE_VALUE
// ����������
//	����ƥ����ļ�
// ����: 
//	ϵͳAPI
// **************************************************

HANDLE WINAPI FileSys_FindFirst( LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd )
{
    int type;
    UINT uLen;
	HANDLE hCallerProcess = GetCallerProcess(); // �õ����н���
	HANDLE hRetv = INVALID_HANDLE_VALUE;

    if( CheckFilePathName( lpcszFileSpec ) ) // ����ļ�������Ч��
	{		
		type  = _GetObjName( lpcszFileSpec, &uLen );	// �õ���������
		if( type == 2 )
		{   // �ļ����� file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszFileSpec, &uLen );


			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpFindFirstFile )
				{
					HANDLE hFind =  (HANDLE)lpVol->lpdsk->lpfsd->lpFindFirstFile( lpVol->pVol, hCallerProcess, lpcszFileSpec + uLen, pfd );
					if( hFind != INVALID_HANDLE_VALUE )
					{	// �ɹ���Ϊ�ļ�ϵͳ��������Ķ������һ�����
						hRetv = FileMgr_CreateSearchHandle( (HVOL)lpVol, hCallerProcess, (DWORD)hFind );
						if( hRetv == NULL )
						{
							//return hRetv;  // �ɹ�
							if( lpVol->lpdsk->lpfsd->lpFindClose )
								lpVol->lpdsk->lpfsd->lpFindClose( (DWORD)hFind );
							hRetv = INVALID_HANDLE_VALUE;
						}
					}
				}
				Sys_ReleaseException();
			}
		}
	}
    return hRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_FindNext( 
//                      HANDLE hFind,
//                      FILE_FIND_DATA * pffd ) 
// ������
// 	IN hFind - ���Ҷ���
//	OUT pffd - FILE_FIND_DATA���ݽṹָ�룬���ڽ����ҵ�������
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	������һ��ƥ����ļ�
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_FindNext( HANDLE hFind, FILE_FIND_DATA * pfd )
{
    _LPFINDDATA lpFind = _GetHFINDPtr( hFind );
	BOOL bRetv = FALSE;

    if( lpFind && pfd )
    {
        if( lpFind->hVol )
        {
            _LPVOLUME lpVol = (_LPVOLUME)lpFind->hVol;
			
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpFindNextFile )
					bRetv = lpVol->lpdsk->lpfsd->lpFindNextFile( lpFind->pFind, pfd );
				Sys_ReleaseException();
			}
        }
    }
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_FlushBuffers( HANDLE hFile ) 
// ������
// 	IN hFile - �ļ�����ָ��
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	ˢ���ļ����棨�����еĻ���
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_FlushBuffers( HANDLE hFile )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpFlushFileBuffers )
					bRetv = lpVol->lpdsk->lpfsd->lpFlushFileBuffers( lpFile->pFile );
				Sys_ReleaseException();
			}
        }
    }
    return bRetv;
}

// **************************************************
// ������BOOL FileSys_GetDiskFreeSpaceEx(
//                          LPCTSTR lpszPathName,
//                          LPDWORD lpdwFreeBytesAvailableToCaller,
//                          LPDWORD lpdwTotalNumberOfBytes,
//                          LPDWORD lpdwTotalNumberOfFreeBytes
//							)
// ������
//	IN lpszPathName - �ļ���
//	OUT lpdwFreeBytesAvailableToCaller - �����߽��̿��еĴ����ֽ���
//	OUT lpdwTotalNumberOfBytes - �ܵĴ����ֽ���
//	OUT lpdwTotalNumberOfFreeBytes - �ܵĿ��д����ֽ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ���ǰ��Ĵ���ʹ�����
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_GetDiskFreeSpaceEx( 
										LPCTSTR lpcszPathName, 
										LPDWORD lpdwFreeBytesAvailableToCaller, 
										LPDWORD lpdwTotalNumberOfBytes, 
										LPDWORD lpdwTotalNumberOfFreeBytes )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;

    if( CheckFilePathName( lpcszPathName ) )// LN 2003-05-23, ADD	
	{		
		type  = _GetObjName( lpcszPathName, &uLen );
		if( type == 2 )
		{   // �ļ����� file
			DWORD dwSectorsPerCluster;
			DWORD dwBytesPerSector;
			DWORD dwFreeClusters;
			DWORD dwClusters;
			_LPVOLUME lpVol = _GetVolumePtr( lpcszPathName, &uLen ); // �õ����ļ���Ӧ�ľ����
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpGetDiskFreeSpace )
				{	// 
					if( lpVol->lpdsk->lpfsd->lpGetDiskFreeSpace( lpVol->pVol, lpcszPathName + uLen, &dwSectorsPerCluster, &dwBytesPerSector, &dwFreeClusters, &dwClusters ) )
					{
						*lpdwTotalNumberOfBytes = dwSectorsPerCluster * dwClusters * dwBytesPerSector;
						*lpdwTotalNumberOfFreeBytes = *lpdwFreeBytesAvailableToCaller = dwSectorsPerCluster * dwFreeClusters * dwBytesPerSector;
						bRetv = TRUE;
					}
				}
				Sys_ReleaseException();
			}
		}
	}
    return bRetv;
}

// **************************************************
// ������DWORD WINAPI FileSys_GetAttributes( LPCTSTR lpcszFileName )
// ������
// 	IN lpszFileName - �ļ���
// ����ֵ��
//	����ɹ��������ļ����ԣ�ʧ��,����0xffffffff
// ����������
//	�õ��ļ�����
// ����: 
//	ϵͳAPI
// **************************************************

DWORD WINAPI FileSys_GetAttributes( LPCTSTR lpcszFileName )
{
    UINT uLen;
	DWORD dwRetv = -1;

    if( CheckFilePathName( lpcszFileName ) )	//����ļ���
	{		
		if( _GetObjName( lpcszFileName, &uLen ) == 2 )	// �õ��ļ�����
		{   // �ļ����� file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszFileName, &uLen );  // �õ������ָ��
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpGetFileAttributes )
				{
					dwRetv = lpVol->lpdsk->lpfsd->lpGetFileAttributes( lpVol->pVol, lpcszFileName + uLen );
				}
				Sys_ReleaseException();
			}
		}
	}

    return dwRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_GetInformation( HANDLE hFile, FILE_INFORMATION * pfi )
// ������
//	IN hFile - �ļ�����
// 	OUT pfi - FILE_INFORMATION�ṹָ�룬���ڽ����ļ���Ϣ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ��ļ���Ϣ
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_GetInformation( HANDLE hFile, FILE_INFORMATION * pfi )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile && pfi )
    {
        if( lpFile->hVol )
        {   // file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpGetFileInformationByHandle )
					bRetv = lpVol->lpdsk->lpfsd->lpGetFileInformationByHandle( lpFile->pFile, pfi );
				Sys_ReleaseException();
			}
        }
    }
    return bRetv;
}

// **************************************************
// ������DWORD WINAPI FileSys_GetSize( HANDLE hFile, LPDWORD pFileSizeHigh )
// ������
//	IN hFile-�ļ�����
//	IN pFileSizeHigh - �ļ��ߴ�ĸ�32bits(��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ��������ļ���С
//	���򣺷��� INVALID_FILE_SIZE
// ����������
//	�õ��ļ���С
// ����: 
//	ϵͳAPI
// **************************************************

DWORD WINAPI FileSys_GetSize( HANDLE hFile, LPDWORD pFileSizeHigh )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	DWORD dwRetv = INVALID_FILE_SIZE;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // �ļ����� file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpGetFileSize )
					dwRetv = lpVol->lpdsk->lpfsd->lpGetFileSize( lpFile->pFile, pFileSizeHigh );
				Sys_ReleaseException();
			}
        }
    }
    return dwRetv;
}

// **************************************************
// ������BOOL WINAPI KFSD_GetFileTime( 
//                      HANDLE hFile,
//                      FILETIME * pCreate,
//					    FILETIME * pLastAccess,
//					    FILETIME * pLastWrite )
// ������
//	IN pf - �ļ�����ָ��
//	OUT pCreate - ָ��FILETIME�ṹ��ָ�룬�ļ�����ʱ��
//	OUT pLastAccess - ָ��FILETIME�ṹ��ָ�룬�ļ����һ�δ�ȡʱ��(Ŀǰ�ݲ�֧�֣�ΪNULL)
//	OUT pLastWrite - ָ��FILETIME�ṹ��ָ�룬�ļ����һ�θ������ݵ�ʱ��(Ŀǰ�ݲ�֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�õ��ļ�ʱ��
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_GetTime( HANDLE hFile, FILETIME * pCreate, FILETIME * pLastAccess, FILETIME * pLastWrite )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // �ļ����� file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpGetFileTime )
					bRetv = lpVol->lpdsk->lpfsd->lpGetFileTime( lpFile->pFile, pCreate, pLastAccess, pLastWrite );
				Sys_ReleaseException();
			}
        }
    }
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_Move( LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName )
// ������
// 	IN lpszOldFileName - ��ǰ�Ѵ��ڵ��ļ����ļ���
// 	IN lpcszNewFileName - �µ��ļ����ļ�����(�µ��ļ����ļ����������Ѵ���)
// ����ֵ��
//	�ɹ������� TRUE
//	���򣺷��� FALSE
// ����������
//	�ƶ��ļ����ļ���
//	�µ��ļ����Ѵ��ڵ��ļ����ڲ�ͬ�ľ�;�µ��ļ������Ѵ��ڵ��ļ��б�������ͬ�ľ�
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_Move( LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName )
{
    int typeOld, typeNew;
    UINT uOldLen, uNewLen;
    BOOL bRetv = FALSE;
    _LPVOLUME lpOldVol, lpNewVol;

    // ����ļ��ĺϷ���
    if( CheckFilePathName( lpcszOldFileName ) && CheckFilePathName( lpcszNewFileName ) )
	{	
		typeOld  = _GetObjName( lpcszOldFileName, &uOldLen );
		typeNew  = _GetObjName( lpcszNewFileName, &uNewLen );
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			if( typeOld == 2 && typeNew == 2 )
			{   // �ļ����� file
				lpOldVol = _GetVolumePtr( lpcszOldFileName, &uOldLen ); // �õ������
				lpNewVol = _GetVolumePtr( lpcszNewFileName, &uNewLen ); // �õ������
				if( lpOldVol == lpNewVol )
				{	// ͬ���ľ������ļ�ϵͳ��������
					if( lpOldVol->lpdsk->lpfsd->lpMoveFile )
						bRetv = lpOldVol->lpdsk->lpfsd->lpMoveFile( lpOldVol->pVol, lpcszOldFileName + uOldLen, lpcszNewFileName + uNewLen );
				}
				else if( lpOldVol && lpNewVol )
				{   // ��ͬ�ľ�
					DWORD dwOldAttrib = 0xFFFFFFFFL;
					// �õ��ļ�����
					if( lpOldVol->lpdsk->lpfsd->lpGetFileAttributes )
						dwOldAttrib = lpOldVol->lpdsk->lpfsd->lpGetFileAttributes( lpOldVol->pVol, lpcszOldFileName + uOldLen );
					// 
					if( !(dwOldAttrib & FILE_ATTRIBUTE_DIRECTORY) )
					{	// ���ļ���
						DWORD dwNewAttrib = 0xFFFFFFFFL;
						if( lpNewVol->lpdsk->lpfsd->lpGetFileAttributes )
							dwNewAttrib = lpNewVol->lpdsk->lpfsd->lpGetFileAttributes( lpNewVol->pVol, lpcszNewFileName + uNewLen );
						
						if( dwNewAttrib == 0xFFFFFFFFL && GetLastError() == ERROR_FILE_NOT_FOUND )
						{   // now copy old file's data to new file
							// �����ļ�
							_LPFILEDATA hOldFile = _CreateFile(  lpcszOldFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
							if( hOldFile != INVALID_HANDLE_VALUE )
							{	// �������ļ�
								_LPFILEDATA hNewFile = _CreateFile(  lpcszNewFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_NEW, dwOldAttrib & (~SPECIAL_ATTRIB), NULL );
								if( hNewFile != INVALID_HANDLE_VALUE )
								{	// �����ļ����ݣ������ļ������ļ���
									bRetv = _CopyFile( hNewFile, hOldFile );
									_CloseFile( hNewFile );  // �ر����ļ�
									if( bRetv == FALSE )
									{	// ����ʧ�ܣ�ɾ��֮ǰ�������ļ�
										if( lpNewVol->lpdsk->lpfsd->lpDeleteFile )
											lpNewVol->lpdsk->lpfsd->lpDeleteFile( lpNewVol->pVol, lpcszNewFileName + uNewLen );
									}								
								}
								_CloseFile( hOldFile ); // �ر����ļ�
								if( bRetv )
								{	// ���翽���ɹ���ɾ�����ļ�
									if( lpOldVol->lpdsk->lpfsd->lpDeleteFile )
										lpOldVol->lpdsk->lpfsd->lpDeleteFile( lpOldVol->pVol, lpcszOldFileName + uOldLen );
								}
							}
						}
					}
				}
			}
			Sys_ReleaseException();
		}
	}
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_Read(
//                  HANDLE hFile,
//					LPVOID lpvBuffer,
//                  DWORD dwNumToRead,
//                  LPDWORD lpdwNumRead, 
//					LPOVERLAPPED lpOverlapped )

// ������
//	IN hFile-�ļ�����
//	OUT lpvBuffer-���ڽ������ݵ��ڴ�
//	IN dwNumToRead-�������ֽ���
//	OUT lpdwNumRead-����ʵ�ʶ����ֽ���������lpdwNumReadΪNULL,�򲻷���
//	IN lpOverlapped-����(��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	���ļ���ȡ����
// ����: 
// **************************************************
#define DEBUG_FileSys_Read 1
BOOL WINAPI FileSys_Read( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile ); // ��鲢�����ļ�����ָ��
	BOOL bRetv = FALSE;

    
	if( lpFile && lpvBuffer )
    {	// ������Ч
        if( lpFile->hVol )
        {   // �ļ����� file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpReadFile )
				{
					bRetv = lpVol->lpdsk->lpfsd->lpReadFile( lpFile->pFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
				}
				Sys_ReleaseException();
			}
			else
			{
				DEBUGMSG( DEBUG_FileSys_Read, ( "FileSys_Read get a exception!.\r\n" ) );
			}
        }
        else
        {   // �豸���� device
			bRetv = Dev_ReadFile( (HANDLE)lpFile->pFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
        }
    }
	
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_ReadWithSeek(
//                          HANDLE hFile,
//                          LPVOID pBuffer,
//                          DWORD cbRead,
//                          LPDWORD pcbRead,
//							LPOVERLAPPED pOverlapped,
//                          DWORD dwLowOffset,
//                          DWORD dwHighOffset)
// ������
//	IN hFile-�ļ�����
//	OUT pBuffer-���ڽ������ݵ��ڴ�
//	IN cbRead-�������ֽ���
//	OUT pcbRead-����ʵ�ʶ����ֽ���������lpdwNumReadΪNULL,�򲻷���
//	IN pOverlapped-����(��֧�֣�ΪNULL)
//	IN dwLowOffset - ƫ��ֵ�ĵ�32λ
//	IN dwHighOffset - ƫ��ֵ�ĸ�32λ
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	���ļ���ȡλ���赽��dwLowOffset dwHighOffsetȷ����ֵ,Ȼ����ļ���ȡ����
// ����: 
//	ϵͳAPI,��֧��
// **************************************************

BOOL WINAPI FileSys_ReadWithSeek( HANDLE hFile, LPVOID pBuffer,DWORD cbRead,LPDWORD pcbRead,LPOVERLAPPED pOverlapped,DWORD dwLowOffset,DWORD dwHighOffset )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;
	
    if( lpFile && pBuffer )
    {
        if( lpFile->hVol )
        {   // file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpReadFileWithSeek )
					bRetv = lpVol->lpdsk->lpfsd->lpReadFileWithSeek( lpFile->pFile, pBuffer, cbRead, pcbRead, pOverlapped, dwLowOffset, dwHighOffset );
				Sys_ReleaseException();
			}
        }
    }
    return bRetv;
}

//BOOL WINAPI FileSys_RegisterFileSystemFunction)( PVOL pVol, SHELLFILECHANGEFUNC pft );

// **************************************************
// ������BOOL WINAPI KL_RemoveDirectory( 
//                         LPCTSTR lpszPathName )
// ������
// 	IN lpszPathName - �ļ�����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	ɾ���ļ���
// ����: 
//	�ڲ�ʹ��
// **************************************************

BOOL WINAPI KL_RemoveDirectory( LPCTSTR lpcszPathName )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;

    if( CheckFilePathName( lpcszPathName ) )	// ����ļ����Ϸ���
	{
		type  = _GetObjName( lpcszPathName, &uLen );	// �õ��ļ���������
		if( type == 2 )
		{   // �ļ��������� file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszPathName, &uLen ); // �����
			
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpRemoveDirectory )
				{
					DEBUGMSG(FSMAIN_ZONE, (TEXT("KL_RemoveDirectory=%s,lpVol=0x%x, lpVolName=%s.\r\n"), lpcszPathName, lpVol, lpVol->lpszVolName ) );
					
					bRetv = lpVol->lpdsk->lpfsd->lpRemoveDirectory( lpVol->pVol, lpcszPathName + uLen );
				}
				Sys_ReleaseException();				
			}
		}
	}
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_RemoveDirectory( 
//                         LPCTSTR lpszPathName )
// ������
// 	IN lpszPathName - �ļ�����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	ɾ���ļ���
// ����: 
//	�ڲ�ʹ��
// **************************************************

BOOL WINAPI FileSys_RemoveDirectory( LPCTSTR lpcszPathName )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;
	
    if( CheckFilePathName( lpcszPathName ) )	// ����ļ����Ϸ���
	{		
		type  = _GetObjName( lpcszPathName, &uLen ); 	// �õ��ļ���������
		if( type == 2 )
		{   // �ļ����� file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszPathName, &uLen );

			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpGetFileAttributes )
				{
					DWORD dwAttrib = lpVol->lpdsk->lpfsd->lpGetFileAttributes( lpVol->pVol, lpcszPathName + uLen );
					if( dwAttrib != -1 ) //	
					{	// ��Ч������
						if( !(dwAttrib & FILE_ATTRIBUTE_DEVICE) )
						{	// �����豸�ļ���
							if( lpVol->lpdsk->lpfsd->lpRemoveDirectory )
								bRetv = lpVol->lpdsk->lpfsd->lpRemoveDirectory( lpVol->pVol, lpcszPathName + uLen );
						}
					}
				}
				Sys_ReleaseException();
			}
		}
	}
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_SetEnd( HANDLE hFile )
// ������
// 	IN hFile - �ļ�����
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ļ�����λ��
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_SetEnd( HANDLE hFile )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // �ļ����� file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpSetEndOfFile )
					bRetv = lpVol->lpdsk->lpfsd->lpSetEndOfFile( lpFile->pFile );
				Sys_ReleaseException();
			}
        }
    }
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI KL_SetFileAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )

// ������
//	IN lpcszFileName-�ļ���
//	IN dwFileAttributes-�ļ�����,������
//			FILE_ATTRIBUTE_ARCHIVE-�ĵ��ļ�
//			FILE_ATTRIBUTE_HIDDEN-�����ļ�
//			FILE_ATTRIBUTE_READONLY-ֻ���ļ�
//			FILE_ATTRIBUTE_SYSTEM-ϵͳ�ļ�
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ļ�����
// ����: 
//	�ڲ�ʹ��
// **************************************************

BOOL WINAPI KL_SetFileAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;

    if( CheckFilePathName( lpcszFileName ) )  // ����ļ����ĺϷ���
	{
		type  = _GetObjName( lpcszFileName, &uLen );	// �õ��ļ�����
		if( type == 2 )
		{   // �ļ����� file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszFileName, &uLen ); // �õ��ļ��ľ����
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpSetFileAttributes )
				{
					bRetv = lpVol->lpdsk->lpfsd->lpSetFileAttributes( lpVol->pVol, lpcszFileName + uLen, dwFileAttributes );
				}
				Sys_ReleaseException();
			}
		}
	}
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_SetAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )

// ������
//	IN lpcszFileName-�ļ���
//	IN dwFileAttributes-�ļ�����,������
//			FILE_ATTRIBUTE_ARCHIVE-�ĵ��ļ�
//			FILE_ATTRIBUTE_HIDDEN-�����ļ�
//			FILE_ATTRIBUTE_READONLY-ֻ���ļ�
//			FILE_ATTRIBUTE_SYSTEM-ϵͳ�ļ�
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ļ�����
// ����: 
//	ϵͳAPI
// **************************************************
BOOL WINAPI FileSys_SetAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )
{
    dwFileAttributes &= ~SPECIAL_ATTRIB;
	return KL_SetFileAttributes( lpcszFileName, dwFileAttributes );
}

// **************************************************
// ������DWORD WINAPI FileSys_SetPointer(
//							HANDLE hFile,
//							LONG lDistanceToMove,
//							LPLONG lpDistanceToMoveHigh,
//							DWORD dwMoveMethod )
// ������
//	IN hFile - �ļ�����
//	IN lDistanceToMove-���ƫ��ֵ����32bits��
//	IN lpDistanceToMoveHigh-(��32bits,��֧��,ΪNULL)
//	IN dwMoveMethod-ƫ�Ƶ���ʼλ�ã�������
//		FILE_BEGIN-�ļ���ʼλ��
//		FILE_CURRENT-�ļ���ǰλ��
//		FILE_END-�ļ�����λ��
//
// ����ֵ��
//	�ɹ��������µ��ļ�λ��
//	���򣺷���0xffffffff
// ����������
//	�����ļ���ǰ��ȡλ��
// ����: 
//	ϵͳAPI
// **************************************************

DWORD WINAPI FileSys_SetPointer( HANDLE hFile, 
								 LONG lDistanceToMove, 
								 LPLONG lpDistanceToMoveHigh, 
								 DWORD dwMoveMethod)
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	DWORD dwRetv = INVALID_SET_FILE_POINTER;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // �ļ����� file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpSetFilePointer )
					dwRetv = lpVol->lpdsk->lpfsd->lpSetFilePointer( lpFile->pFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
				Sys_ReleaseException();
			}
        }
        else
        {   // �豸���� device
			return Dev_SetFilePointer( (HANDLE)lpFile->pFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
        }
    }
    return dwRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_SetTime(
//						HANDLE hFile, 
//						const FILETIME *pCreate,
//						const FILETIME *pLastAccess,
//						const FILETIME *pLastWrite )
// ������
// 	IN hFile - �ļ����
//	IN pCreate-FILETIME�ṹָ�룬�ļ�����ʱ��
//	IN pLastAccess-FILETIME�ṹָ�룬�ļ���ȡʱ��(Ŀǰ��֧�֣�ΪNULL)
//	IN pLastWrite-FILETIME�ṹָ�룬�ļ�����ʱ��(Ŀǰ��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ļ�ʱ��
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_SetTime( HANDLE hFile, const FILETIME * pCreate, const FILETIME * pLastAccess, const FILETIME * pLastWrite )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpSetFileTime )
					bRetv = lpVol->lpdsk->lpfsd->lpSetFileTime( lpFile->pFile, pCreate, pLastAccess, pLastWrite );
				Sys_ReleaseException();
			}
        }
    }
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_Write( 
//                  HANDLE hFile, 
//                  LPCVOID lpvBuffer,
//                  DWORD dwNumToWrite, 
//                  LPDWORD lpdwNumWrite,
//					LPOVERLAPPED pOverlapped)
// ������
//	IN hFile-�ļ�����
//	IN lpvBuffer-��д�����ݵĴ���ڴ�
//	IN dwNumToWrite-��д���ֽ���
//	OUT lpdwNumWrite-����ʵ��д���ֽ���������lpdwNumWriteΪNULL,�򲻷���
//	IN pOverlapped-����(��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	д���ݵ��ļ�
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_Write( HANDLE hFile, 
						   LPCVOID lpvBuffer, 
						   DWORD dwNumToWrite,
						   LPDWORD lpdwNumWrite,
						   LPOVERLAPPED pOverlapped )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile && lpvBuffer )
    {
        if( lpFile->hVol )
        {   // �ļ����� file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpWriteFile )
					bRetv = lpVol->lpdsk->lpfsd->lpWriteFile( lpFile->pFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
				Sys_ReleaseException();
			}
        }
        else
        {  // �豸���� device
			return Dev_WriteFile( (HANDLE)lpFile->pFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
        }
    }
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_WriteWithSeek( 
//                           HANDLE hFile, 
//                           LPCVOID lpcvBuffer, 
//                           DWORD dwWrite, 
//                           LPDWORD lpdwWritten, 
//                           DWORD dwLowOffset, 
//                           DWORD dwHighOffset ) 
// ������
//	IN hFile-�ļ�����
//	OUT lpcvBuffer-д�����ݵ��ڴ�
//	IN dwWrite-��д���ֽ���
//	OUT lpdwWritten-����ʵ��д���ֽ��������� lpdwWritten ΪNULL,�򲻷���
//	IN dwLowOffset - ƫ��ֵ�ĵ�32λ
//	IN dwHighOffset - ƫ��ֵ�ĸ�32λ
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	���ļ���ȡλ���赽��dwLowOffset dwHighOffsetȷ����ֵ,Ȼ��д���ݵ��ļ�
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_WriteWithSeek( HANDLE hFile, 
								   LPCVOID lpcvBuffer,
								   DWORD dwWrite,
								   LPDWORD lpdwWritten,
								   DWORD dwLowOffset,
								   DWORD dwHighOffset )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile && lpcvBuffer )
    {
        if( lpFile->hVol )
        {   // file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpWriteFileWithSeek )
					bRetv = lpVol->lpdsk->lpfsd->lpWriteFileWithSeek( lpFile->pFile, lpcvBuffer, dwWrite, lpdwWritten, dwLowOffset, dwHighOffset );
				Sys_ReleaseException();
			}
        }
    }
    return bRetv;
}

// **************************************************
// ������BOOL WINAPI FileSys_Copy(
//						LPCTSTR lpcszExistingFileName,
//						LPCTSTR lpcszNewFileName,
//						BOOL bFailIfExists
//						)
// ������
// 	IN lpcszExistingFileName �C Դ�ļ�����������ڣ�
// 	IN lpcszNewFileName �C Ŀ���ļ��������Դ���Ҳ���Բ����ڣ�
// 	IN bFailIfExists �C ����Ŀ���ļ����ڣ��Ƿ�ú���ʧ�ܡ�
//						����ΪTRUE����Ŀ���ļ��Ѿ����ڣ��ú���ʧ�ܣ�
//						����ΪFALSE����Ŀ���ļ��Ѿ����ڣ��ú��������Ŀ���ļ������ݲ���Դ�ļ����ݿ�����Ŀ���ļ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ļ����ݴ�Դ�ļ�������Ŀ���ļ�
// ����: 
//	ϵͳAPI
// **************************************************

BOOL WINAPI FileSys_Copy( LPCTSTR lpcszExistingFileName, LPCTSTR lpcszNewFileName, BOOL bFailIfExists )
{
    int typeOld, typeNew;
    UINT uOldLen, uNewLen;
    BOOL bRetv = FALSE;
    _LPVOLUME lpOldVol, lpNewVol;
	
    if( CheckFilePathName( lpcszExistingFileName ) && 
		CheckFilePathName( lpcszNewFileName ) )	// ����ļ����ĺϷ���
	{
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			typeOld  = _GetObjName( lpcszExistingFileName, &uOldLen );  // �õ��ļ���������
			typeNew  = _GetObjName( lpcszNewFileName, &uNewLen );	// �õ��ļ���������
			if( typeOld == 2 && typeNew == 2 )
			{   // �����ļ����� file
				// �õ��ļ���Ӧ�Ĵ��̾�
				lpOldVol = _GetVolumePtr( lpcszExistingFileName, &uOldLen );
				lpNewVol = _GetVolumePtr( lpcszNewFileName, &uNewLen );
				if( lpOldVol == lpNewVol )
				{	// ��ͬ�Ĵ��̾�
					if( lpOldVol->lpdsk->lpfsd->lpCopyFile )
						bRetv = lpOldVol->lpdsk->lpfsd->lpCopyFile( lpOldVol->pVol, lpcszExistingFileName + uOldLen, lpcszNewFileName + uNewLen, bFailIfExists );
				}
				else
				{   // ��ͬ�Ĵ��̾� volume is different
					DWORD dwOldAttrib = -1;
					
					if( lpOldVol->lpdsk->lpfsd->lpGetFileAttributes )				
						dwOldAttrib = lpOldVol->lpdsk->lpfsd->lpGetFileAttributes( lpOldVol->pVol, lpcszExistingFileName + uOldLen );
					
					if( !(dwOldAttrib & FILE_ATTRIBUTE_DIRECTORY) )
					{	// ���ļ��У����ļ� is file
						_LPFILEDATA hOldFile = _CreateFile(  lpcszExistingFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
						
						if( hOldFile != INVALID_HANDLE_VALUE )
						{	// �����ļ��ɹ�
							DWORD dwCreateFlag = bFailIfExists ? CREATE_NEW : CREATE_ALWAYS;
							// �������ļ�
							_LPFILEDATA hNewFile = _CreateFile( lpcszNewFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, dwCreateFlag, 0, NULL ); 
							
							if( hNewFile != INVALID_HANDLE_VALUE )
							{	// �������ļ��ɹ�
								FILETIME ft;
								// �õ����ļ�ʱ��
								FileSys_GetTime( hOldFile, &ft, NULL, NULL );
								// �������ļ�ʱ��
								bRetv = FileSys_SetTime( hNewFile, &ft, NULL, NULL );
								// �����ļ�
								if( bRetv && 
									(bRetv = _CopyFile( hNewFile, hOldFile )) == TRUE )
								{
									;								
								}
								// �ر����ļ�
								_CloseFile( hNewFile );
								if( bRetv == FALSE )  //
								{
									if( lpNewVol->lpdsk->lpfsd->lpDeleteFile )
										lpNewVol->lpdsk->lpfsd->lpDeleteFile( lpNewVol->pVol, lpcszNewFileName + uNewLen  );
								}
								else // �����ļ���������Ϊ�����ļ���ͬ 
									lpNewVol->lpdsk->lpfsd->lpSetFileAttributes( lpNewVol->pVol, lpcszNewFileName + uNewLen, dwOldAttrib & (~SPECIAL_ATTRIB)  );
							}
							// �ر����ļ�
							_CloseFile( hOldFile );
						}
					}
				}
			}
			Sys_ReleaseException();
		}
	}
    return bRetv;
}


