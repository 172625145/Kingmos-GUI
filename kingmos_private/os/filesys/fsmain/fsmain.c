/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：文件系统主调用界面

版本号：1.0.0
开发时期：2000
作者：李林
修改记录：
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
// 声明：_LPFILEDATA _GetHFILEPtr( HANDLE hFile )
// 参数：
//		IN hFile - 文件对象
// 返回值：
//		假如有效，返回非NULL文件对象指针；否则，返回NULL
// 功能描述：
//		验证文件对象的有效性并返回有效的指针
// 引用: 
//		
// **************************************************

static _LPFILEDATA _GetHFILEPtr( HANDLE hFile )
{
	if( hFile == 0 || hFile == INVALID_HANDLE_VALUE )
	    return NULL;
	return (_LPFILEDATA)hFile;
}

// **************************************************
// 声明：_LPFINDDATA _GetHFINDPtr( HANDLE hfd )
// 参数：
//		IN hfd - 文件查找对象
// 返回值：
//		假如有效，返回非NULL文件查找对象指针；否则，返回NULL
// 功能描述：
//		验证文件对象的有效性并返回有效的指针
// 引用: 
//		
// **************************************************

_LPFINDDATA _GetHFINDPtr( HANDLE hfd )
{
	if( hfd && hfd != INVALID_HANDLE_VALUE )
	{
		_LPFINDDATA lpfd = (_LPFINDDATA)hfd;
		if( lpfd->obj.objType == OBJ_FINDFILE )
			return lpfd;  // 有效
	}
    return NULL;
}

// **************************************************
// 声明：static int CheckFilePathName( LPCTSTR lpfpn )
// 参数：
//		IN lpfpn - 文件名
// 返回值：
//		假如成功，返回文件名的长度；否则，返回0
// 功能描述：
//		验证文件名的有效性
// 引用: 
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
// 声明：static int _GetObjName( LPCTSTR lpfn, UINT * lpNameLen )
// 参数：
//		IN lpfn - 完整的路径名
//		OUT lpNameLen - 用于接受存储对象名的长度
// 返回值：
//		返回存储对象的类型，可以为以下值：
//			0 - 不知道
//			1 - 设备类型，例如："COM2:"
//			2 - 文件类型，例如："\\folder\\test.txt"
//
// 功能描述：
//		得到对象名类型
// 引用: 
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
        type = 1;   // 设备类型，device type
        *lpNameLen = 4;
    }
    else
    {
        if( *lpfn != '\\' )
            return 0; // 错误
        lpfn++;
        i++;
        while( *lpfn && *lpfn != '\\' )
        {
            i++;
            lpfn++;
        }
        type = 2;   // 文件类型 file type         
        *lpNameLen = i;
    }
    return type;
}

// **************************************************
// 声明：static _LPVOLUME _GetVolumePtr( LPCTSTR lpcszFileName, UINT * pLen )
// 参数：
//		IN lpcszFileName - 包含卷名的文件指针（可以为NULL,这时代表用系统默认卷），例如："\\storage disk\\filename.ext"
//		IN/OUT pLen - 卷名长度,当lpcszFileName为NULL时，输出默认卷名的长度
// 返回值：
//		无
// 功能描述：
//		根据卷名得到地区当前系统存在的卷对象指针，如果卷名为NULL，则得到系统默认卷；
//		系统默认卷是一个“根卷”
// 引用: 
//		
// **************************************************

static _LPVOLUME _GetVolumePtr( LPCTSTR lpcszFileName, UINT * pLen )
{
    _LPVOLUME lpVol = NULL;
	if( lpcszFileName[*pLen] == '\\' )  // 检查是否合法
        lpVol= (_LPVOLUME)_FileMgr_FindVolume( lpcszFileName, *pLen );
    if( lpVol == NULL )  
    {	// 用系统默认卷use sysVolume
        lpVol= (_LPVOLUME)_FileMgr_FindVolume( sysVolume, 6 );
        *pLen = 0;
    }
    return lpVol;
}

// **************************************************
// 声明：static BOOL _CopyFile( HANDLE hFileDest, HANDLE hFileSrc )
// 参数：
//		IN hFileDest - 目标文件
//		IN hFileSrc - 源文件
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		将源文件内容拷贝到目标文件
// 引用: 
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
		lpBuf = Page_AllocMem( unit, &dwRealAllocSize, 0 );  // 分配以页为单位的
		if( lpBuf )
		{	// 分配成功
			unit = dwRealAllocSize;
			break;
		}
		unit >>= 1; //分配不成功，用较小的大小来试
	}
	
	if( lpBuf )
	{
		dwSize = FileSys_GetSize( hFileSrc, NULL );
		if( FileSys_SetPointer( hFileDest, dwSize, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
			goto _error;
		// 将目标文件的大小设为与源文件大小相同
		if( FileSys_SetEnd( hFileDest ) == FALSE )
			goto _error;
		FileSys_SetPointer( hFileDest, 0, NULL, FILE_BEGIN );
		p = lpBuf;
		// 逐段拷贝文件内容
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
		Page_FreeMem( lpBuf, unit ); // 释放之前分配的缓存
	}
	else
	{
		DEBUGMSG( FSMAIN_ZONE, ( "no enough memory when CopyFile.\r\n" ) );
	}
	return bRetv;
}

// **************************************************
// 声明：static BOOL IsSystemProtected( LPCTSTR lpcszPathName )
// 参数：
//		IN lpcszPathName - 文件路径名
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		检查文件是否是系统预留名字
// 引用: 
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
// 声明：BOOL WINAPI KL_CreateDirectory(
//										LPCTSTR lpcszPathName, 
//										PSECURITY_ATTRIBUTES pSecurityAttributes )
// 参数：
//		IN lpcszPathName - 路径名
//		IN pSecurityAttributes - 安全属性(不支持，为NULL)
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		创建文件夹
// 引用: 
//		内部使用
// **************************************************

BOOL WINAPI KL_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes)
{
    int type;
    UINT uLen;
	
	if( CheckFilePathName( lpcszPathName ) )	// 检查文件名是否合法
	{
		type  = _GetObjName( lpcszPathName, &uLen );	// 得到对象类型
		if( type == 2 )
		{   // 文件类型 file
			// 得到该文件对应的卷对象指针
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
// 声明：BOOL WINAPI FileSys_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes)
// 参数：
//		IN lpcszPathName - 路径名
//		IN pSecurityAttributes - 安全属性(不支持，为NULL)
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		创建文件夹
// 引用: 
//		系统API
// **************************************************

BOOL WINAPI FileSys_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes)
{
	if( IsSystemProtected( lpcszPathName ) ) // 该文件名是否已经被保护 ？
		return FALSE;	// 是，被保护，返回错误
	return KL_CreateDirectory( lpcszPathName, pSecurityAttributes );	// 
}

// **************************************************
// 声明：static _LPFILEDATA _CreateFile( 
//							LPCTSTR lpcszFileName, 
//							DWORD dwAccess, 
//							DWORD dwShareMode, 
//							PSECURITY_ATTRIBUTES pSecurityAttributes, 
//							DWORD dwCreate, 
//							DWORD dwFlagsAndAttributes, 
//							HANDLE hTemplateFile )
// 参数：
//		IN lpcszFileName-文件名
//		IN fAccess-存取控制，由以下值的位组合：
//				GENERIC_WRITE-读操作
//				GENERIC_READ-写操作
//		IN wShareMode-共享模式，由以下值的位组合：
//				FILE_SHARE_READ-共享读
//				FILE_SHARE_WRITE-共享写
//      IN pSecurityAttributes-安全设置(不支持，为NULL)
//		IN dwCreate-创建方式，包含：
//				CREATE_NEW-创建新文件；假如文件存在，则失败
//				CREATE_ALWAYS-创建文件；假如文件存在，则覆盖它
//				OPEN_EXISTING-打开文件；假如文件不存在，则失败
//				OPEN_ALWAYS-打开文件；假如文件不存在，则创建
//      IN dwFlagsAndAttributes-文件属性
//		IN hTemplateFile-临时文件句柄(不支持,为NULL)
// 返回值：
//		文件结构指针
// 功能描述：
//		创建文件
// 引用: 
//		内部使用
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

	lpfd = (_LPFILEDATA)malloc( sizeof( _FILEDATA ) ); // 分配文件句柄结构

    if( lpfd == NULL )
		return INVALID_HANDLE_VALUE;

	lpfd->pFile = (PFILE)INVALID_HANDLE_VALUE;  // 先初始化为无效的值

    type  = _GetObjName( lpcszFileName, &uLen );	// 得到对象类型
    if( type == 1 )
    {   // 设备对象 device
		lpfd->hVol = 0;
		lpfd->pFile = (DWORD)Dev_CreateFile( 
								lpcszFileName,
								dwAccess,
								dwShareMode,
								GetCallerProcess() 	);
    }
    else if( type == 2 )
    {   // 文件对象 file
        _LPVOLUME lpVol = _GetVolumePtr( lpcszFileName, &uLen ); // 得到卷对象

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
	{	// 创建文件成功
		return lpfd;
	}
	else
	{	// 不成功，释放之前分配的指针
		free( lpfd );
		return INVALID_HANDLE_VALUE;
	}    
}

// **************************************************
// 声明：HANDLE WINAPI FileSys_CreateFile( 
//						LPCTSTR lpcszFileName, 
//						DWORD dwAccess, 
//						DWORD dwShareMode, 
//						PSECURITY_ATTRIBUTES pSecurityAttributes, 
//						DWORD dwCreate, 
//						DWORD dwFlagsAndAttributes, 
//						HANDLE hTemplateFile )
// 参数：
//		IN lpcszFileName-文件名
//		IN dwAccess-存取控制，由以下值的位组合：
//				GENERIC_WRITE-读操作
//				GENERIC_READ-写操作
//		IN dwShareMode-共享模式，由以下值的位组合：
//				FILE_SHARE_READ-共享读
//				FILE_SHARE_WRITE-共享写
//      IN pSecurityAttributes-安全设置(不支持，为NULL)
//		IN dwCreate-创建方式，包含：
//				CREATE_NEW-创建新文件；假如文件存在，则失败
//				CREATE_ALWAYS-创建文件；假如文件存在，则覆盖它
//				OPEN_EXISTING-打开文件；假如文件不存在，则失败
//				OPEN_ALWAYS-打开文件；假如文件不存在，则创建
//      IN dwFlagsAndAttributes-文件属性
//		IN hTemplateFile-临时文件句柄(不支持,为NULL)
// 返回值：
//		成功：返回句柄
//		失败：返回 INVALID_HANDLE_VALUE
// 功能描述：
//		创建文件
// 引用: 
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

	// 检查参数 check param  
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
	// 检查文件名
	if( CheckFilePathName( lpcszFileName ) )// LN 2003-05-23, ADD
    {	// 有效
		lpFile = _CreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
		if( lpFile != INVALID_HANDLE_VALUE )
		{	// 创建文件成功，分配一个句柄
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
// 声明：static BOOL _CloseFile( _LPFILEDATA lpFile )
// 参数：
//		IN lpFile - 文件对象指针
// 返回值：
//		成功：返回TRUE
//		失败：返回FALSE
// 功能描述：
//		关闭文件
// 引用: 
//		
// **************************************************

static BOOL _CloseFile( _LPFILEDATA lpFile )
{
	BOOL bRetv = FALSE;

	if( lpFile->hVol )
	{   // 文件对象 file            
		_LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;

		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			if( lpVol->lpdsk->lpfsd->lpCloseFile )
				bRetv = lpVol->lpdsk->lpfsd->lpCloseFile( lpFile->pFile );
			Sys_ReleaseException();
		}
	}
	else
	{   // 设备对象 device
		bRetv = Dev_CloseFile( (HANDLE)lpFile->pFile );
	}
	free( lpFile );  
	return bRetv;
}

// **************************************************
// 声明：BOOL WINAPI FileSys_CloseFile( HANDLE hFile )
// 参数：
// 	IN hFile - 文件对象
// 返回值：
//	假如成功，返回TRUE; 失败，返回FALSE
// 功能描述：
//	关闭文件对象
// 引用: 
//	系统API
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
// 声明：BOOL WINAPI FileSys_DeleteAndRename( 
//                              LPCTSTR lpszNewFileName, 
//                              LPCTSTR lpszOldFileName )
// 参数：
// 	IN lpcszNewFileName – 目标文件
// 	IN lpcszOldFileName – 源文件
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将源文件内容拷贝到目标文件后再删除源文件
// 引用: 
//	系统API
// **************************************************

BOOL WINAPI FileSys_DeleteAndRename( LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName )
{
    int type;
    UINT uLen;
	// 检查名字是否有效
	if( CheckFilePathName( lpcszNewFileName ) && CheckFilePathName( lpcszOldFileName ) )
	{		
		type  = _GetObjName( lpcszOldFileName, &uLen );  // 得到对象类型
		if( type == 2 )
		{   // 文件对象 file
			UINT ul = uLen;
			// 得到卷对象
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
// 声明：BOOL FileSys_Delete(
//                    LPCTSTR lpszFileName )
// 参数：
//	IN lpszFileName - 文件名
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	删除文件
// 引用: 
//	系统API
// **************************************************

BOOL WINAPI FileSys_Delete(  LPCTSTR lpcszFileName )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;

    if( CheckFilePathName( lpcszFileName ) )  // 检查文件名
	{
		type  = _GetObjName( lpcszFileName, &uLen );	// 得到文件类型
		if( type == 2 )
		{   // 文件类型 file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszFileName, &uLen ); // 得到卷对象
			
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
// 声明：BOOL WINAPI FileSys_DeviceIoControl(
//							HANDLE hFile,
//							DWORD dwIoControlCode,
//							LPVOID lpInBuf,
//							DWORD nInBufSize,
//							LPVOID lpOutBuf,
//							DWORD nOutBufSize,
//							LPDWORD lpBytesReturned
//							)
// 参数：
//	IN hFile-文件对象
//	IN dwIoControlCode-I/O控制命令，文件系统包含：
//			IOCTL_DISK_SCAN_VOLUME－通知文件系统去扫描卷的错误
//			IOCTL_DISK_FORMAT_VOLUME－通知文件系统去格式化卷
//	IN lpInBuf-输入缓存
//	IN nInBufSize-输入缓存的尺寸
//	OUT lpOutBuf-输出缓存
//	IN nOutBufSize-输出缓存的尺寸
//	OUT lpBytesReturned-返回的数据的尺寸，该数据在返回时已存入lpOutBuf
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	文件系统控制管理
// 引用: 
//	系统API
// **************************************************

BOOL WINAPI FileSys_DeviceIoControl( HANDLE hFile, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpdwBytesReturned, LPOVERLAPPED lpOverlapped )
{
	_LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	
	if( lpFile )
	{
		if( lpFile->hVol )
		{   // 文件对象 file
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
		{   // 设备对象 device
			return Dev_IoControl( (HANDLE)lpFile->pFile, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned, lpOverlapped );
		}
	}
    return FALSE;
}

// **************************************************
// 声明：BOOL FileSys_FindClose( HANDLE hFind )
// 参数：
// 	IN hFind - 查找对象
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	关闭查找对象
// 引用: 
//	系统API
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
// 声明：HANDLE WINAPI FileSys_FindFirst( 
//                         LPCTSTR lpszFileSpec, 
//                         FILE_FIND_DATA * pfd )
// 参数：
// 	IN lpszFileSpec - 文件名（可以含通配字符）
//	OUT pfd - FILE_FIND_DATA数据结构指针，用于接收找到的数据
// 返回值：
//	成功：返回有效的句柄
//	否则：返回INVALID_HANDLE_VALUE
// 功能描述：
//	查找匹配的文件
// 引用: 
//	系统API
// **************************************************

HANDLE WINAPI FileSys_FindFirst( LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd )
{
    int type;
    UINT uLen;
	HANDLE hCallerProcess = GetCallerProcess(); // 得到呼叫进程
	HANDLE hRetv = INVALID_HANDLE_VALUE;

    if( CheckFilePathName( lpcszFileSpec ) ) // 检查文件名的有效性
	{		
		type  = _GetObjName( lpcszFileSpec, &uLen );	// 得到对象类型
		if( type == 2 )
		{   // 文件类型 file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszFileSpec, &uLen );


			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpFindFirstFile )
				{
					HANDLE hFind =  (HANDLE)lpVol->lpdsk->lpfsd->lpFindFirstFile( lpVol->pVol, hCallerProcess, lpcszFileSpec + uLen, pfd );
					if( hFind != INVALID_HANDLE_VALUE )
					{	// 成功，为文件系统驱动程序的对象分配一个句柄
						hRetv = FileMgr_CreateSearchHandle( (HVOL)lpVol, hCallerProcess, (DWORD)hFind );
						if( hRetv == NULL )
						{
							//return hRetv;  // 成功
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
// 声明：BOOL WINAPI FileSys_FindNext( 
//                      HANDLE hFind,
//                      FILE_FIND_DATA * pffd ) 
// 参数：
// 	IN hFind - 查找对象
//	OUT pffd - FILE_FIND_DATA数据结构指针，用于接收找到的数据
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	查找下一个匹配的文件
// 引用: 
//	系统API
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
// 声明：BOOL WINAPI FileSys_FlushBuffers( HANDLE hFile ) 
// 参数：
// 	IN hFile - 文件对象指针
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	刷新文件缓存（假如有的话）
// 引用: 
//	系统API
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
// 声明：BOOL FileSys_GetDiskFreeSpaceEx(
//                          LPCTSTR lpszPathName,
//                          LPDWORD lpdwFreeBytesAvailableToCaller,
//                          LPDWORD lpdwTotalNumberOfBytes,
//                          LPDWORD lpdwTotalNumberOfFreeBytes
//							)
// 参数：
//	IN lpszPathName - 文件名
//	OUT lpdwFreeBytesAvailableToCaller - 呼叫者进程空闲的磁盘字节数
//	OUT lpdwTotalNumberOfBytes - 总的磁盘字节数
//	OUT lpdwTotalNumberOfFreeBytes - 总的空闲磁盘字节数
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到当前卷的磁盘使用情况
// 引用: 
//	系统API
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
		{   // 文件对象 file
			DWORD dwSectorsPerCluster;
			DWORD dwBytesPerSector;
			DWORD dwFreeClusters;
			DWORD dwClusters;
			_LPVOLUME lpVol = _GetVolumePtr( lpcszPathName, &uLen ); // 得到该文件对应的卷对象
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
// 声明：DWORD WINAPI FileSys_GetAttributes( LPCTSTR lpcszFileName )
// 参数：
// 	IN lpszFileName - 文件名
// 返回值：
//	假如成功，返回文件属性；失败,返回0xffffffff
// 功能描述：
//	得到文件属性
// 引用: 
//	系统API
// **************************************************

DWORD WINAPI FileSys_GetAttributes( LPCTSTR lpcszFileName )
{
    UINT uLen;
	DWORD dwRetv = -1;

    if( CheckFilePathName( lpcszFileName ) )	//检查文件名
	{		
		if( _GetObjName( lpcszFileName, &uLen ) == 2 )	// 得到文件类型
		{   // 文件类型 file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszFileName, &uLen );  // 得到卷对象指针
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
// 声明：BOOL WINAPI FileSys_GetInformation( HANDLE hFile, FILE_INFORMATION * pfi )
// 参数：
//	IN hFile - 文件对象
// 	OUT pfi - FILE_INFORMATION结构指针，用于接受文件信息
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到文件信息
// 引用: 
//	系统API
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
// 声明：DWORD WINAPI FileSys_GetSize( HANDLE hFile, LPDWORD pFileSizeHigh )
// 参数：
//	IN hFile-文件对象
//	IN pFileSizeHigh - 文件尺寸的高32bits(不支持，为NULL)
// 返回值：
//	成功：返回文件大小
//	否则：返回 INVALID_FILE_SIZE
// 功能描述：
//	得到文件大小
// 引用: 
//	系统API
// **************************************************

DWORD WINAPI FileSys_GetSize( HANDLE hFile, LPDWORD pFileSizeHigh )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	DWORD dwRetv = INVALID_FILE_SIZE;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // 文件类型 file
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
// 声明：BOOL WINAPI KFSD_GetFileTime( 
//                      HANDLE hFile,
//                      FILETIME * pCreate,
//					    FILETIME * pLastAccess,
//					    FILETIME * pLastWrite )
// 参数：
//	IN pf - 文件对象指针
//	OUT pCreate - 指向FILETIME结构的指针，文件创建时间
//	OUT pLastAccess - 指向FILETIME结构的指针，文件最后一次存取时间(目前暂不支持，为NULL)
//	OUT pLastWrite - 指向FILETIME结构的指针，文件最后一次更新数据的时间(目前暂不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	得到文件时间
// 引用: 
//	系统API
// **************************************************

BOOL WINAPI FileSys_GetTime( HANDLE hFile, FILETIME * pCreate, FILETIME * pLastAccess, FILETIME * pLastWrite )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // 文件对象 file
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
// 声明：BOOL WINAPI FileSys_Move( LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName )
// 参数：
// 	IN lpszOldFileName - 当前已存在的文件或文件夹
// 	IN lpcszNewFileName - 新的文件或文件夹名(新的文件或文件夹名不能已存在)
// 返回值：
//	成功：返回 TRUE
//	否则：返回 FALSE
// 功能描述：
//	移动文件或文件夹
//	新的文件与已存在的文件可在不同的卷;新的文件夹与已存在的文件夹必须在相同的卷。
// 引用: 
//	系统API
// **************************************************

BOOL WINAPI FileSys_Move( LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName )
{
    int typeOld, typeNew;
    UINT uOldLen, uNewLen;
    BOOL bRetv = FALSE;
    _LPVOLUME lpOldVol, lpNewVol;

    // 检查文件的合法性
    if( CheckFilePathName( lpcszOldFileName ) && CheckFilePathName( lpcszNewFileName ) )
	{	
		typeOld  = _GetObjName( lpcszOldFileName, &uOldLen );
		typeNew  = _GetObjName( lpcszNewFileName, &uNewLen );
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			if( typeOld == 2 && typeNew == 2 )
			{   // 文件类型 file
				lpOldVol = _GetVolumePtr( lpcszOldFileName, &uOldLen ); // 得到卷对象
				lpNewVol = _GetVolumePtr( lpcszNewFileName, &uNewLen ); // 得到卷对象
				if( lpOldVol == lpNewVol )
				{	// 同样的卷，调用文件系统驱动程序
					if( lpOldVol->lpdsk->lpfsd->lpMoveFile )
						bRetv = lpOldVol->lpdsk->lpfsd->lpMoveFile( lpOldVol->pVol, lpcszOldFileName + uOldLen, lpcszNewFileName + uNewLen );
				}
				else if( lpOldVol && lpNewVol )
				{   // 不同的卷
					DWORD dwOldAttrib = 0xFFFFFFFFL;
					// 得到文件属性
					if( lpOldVol->lpdsk->lpfsd->lpGetFileAttributes )
						dwOldAttrib = lpOldVol->lpdsk->lpfsd->lpGetFileAttributes( lpOldVol->pVol, lpcszOldFileName + uOldLen );
					// 
					if( !(dwOldAttrib & FILE_ATTRIBUTE_DIRECTORY) )
					{	// 非文件夹
						DWORD dwNewAttrib = 0xFFFFFFFFL;
						if( lpNewVol->lpdsk->lpfsd->lpGetFileAttributes )
							dwNewAttrib = lpNewVol->lpdsk->lpfsd->lpGetFileAttributes( lpNewVol->pVol, lpcszNewFileName + uNewLen );
						
						if( dwNewAttrib == 0xFFFFFFFFL && GetLastError() == ERROR_FILE_NOT_FOUND )
						{   // now copy old file's data to new file
							// 打开老文件
							_LPFILEDATA hOldFile = _CreateFile(  lpcszOldFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
							if( hOldFile != INVALID_HANDLE_VALUE )
							{	// 创建新文件
								_LPFILEDATA hNewFile = _CreateFile(  lpcszNewFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_NEW, dwOldAttrib & (~SPECIAL_ATTRIB), NULL );
								if( hNewFile != INVALID_HANDLE_VALUE )
								{	// 拷贝文件内容（从老文件到新文件）
									bRetv = _CopyFile( hNewFile, hOldFile );
									_CloseFile( hNewFile );  // 关闭新文件
									if( bRetv == FALSE )
									{	// 拷贝失败，删除之前创建的文件
										if( lpNewVol->lpdsk->lpfsd->lpDeleteFile )
											lpNewVol->lpdsk->lpfsd->lpDeleteFile( lpNewVol->pVol, lpcszNewFileName + uNewLen );
									}								
								}
								_CloseFile( hOldFile ); // 关闭老文件
								if( bRetv )
								{	// 假如拷贝成功，删除老文件
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
// 声明：BOOL WINAPI FileSys_Read(
//                  HANDLE hFile,
//					LPVOID lpvBuffer,
//                  DWORD dwNumToRead,
//                  LPDWORD lpdwNumRead, 
//					LPOVERLAPPED lpOverlapped )

// 参数：
//	IN hFile-文件对象
//	OUT lpvBuffer-用于接受数据的内存
//	IN dwNumToRead-欲读的字节数
//	OUT lpdwNumRead-返回实际读的字节数；假如lpdwNumRead为NULL,则不返回
//	IN lpOverlapped-覆盖(不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	从文件读取数据
// 引用: 
// **************************************************
#define DEBUG_FileSys_Read 1
BOOL WINAPI FileSys_Read( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile ); // 检查并返回文件对象指针
	BOOL bRetv = FALSE;

    
	if( lpFile && lpvBuffer )
    {	// 参数有效
        if( lpFile->hVol )
        {   // 文件对象 file
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
        {   // 设备对象 device
			bRetv = Dev_ReadFile( (HANDLE)lpFile->pFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
        }
    }
	
    return bRetv;
}

// **************************************************
// 声明：BOOL WINAPI FileSys_ReadWithSeek(
//                          HANDLE hFile,
//                          LPVOID pBuffer,
//                          DWORD cbRead,
//                          LPDWORD pcbRead,
//							LPOVERLAPPED pOverlapped,
//                          DWORD dwLowOffset,
//                          DWORD dwHighOffset)
// 参数：
//	IN hFile-文件对象
//	OUT pBuffer-用于接受数据的内存
//	IN cbRead-欲读的字节数
//	OUT pcbRead-返回实际读的字节数；假如lpdwNumRead为NULL,则不返回
//	IN pOverlapped-覆盖(不支持，为NULL)
//	IN dwLowOffset - 偏移值的低32位
//	IN dwHighOffset - 偏移值的高32位
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	将文件存取位置设到由dwLowOffset dwHighOffset确定的值,然后从文件读取数据
// 引用: 
//	系统API,不支持
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
// 声明：BOOL WINAPI KL_RemoveDirectory( 
//                         LPCTSTR lpszPathName )
// 参数：
// 	IN lpszPathName - 文件夹名
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	删除文件夹
// 引用: 
//	内部使用
// **************************************************

BOOL WINAPI KL_RemoveDirectory( LPCTSTR lpcszPathName )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;

    if( CheckFilePathName( lpcszPathName ) )	// 检查文件名合法性
	{
		type  = _GetObjName( lpcszPathName, &uLen );	// 得到文件对象类型
		if( type == 2 )
		{   // 文件对象类型 file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszPathName, &uLen ); // 卷对象
			
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
// 声明：BOOL WINAPI FileSys_RemoveDirectory( 
//                         LPCTSTR lpszPathName )
// 参数：
// 	IN lpszPathName - 文件夹名
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	删除文件夹
// 引用: 
//	内部使用
// **************************************************

BOOL WINAPI FileSys_RemoveDirectory( LPCTSTR lpcszPathName )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;
	
    if( CheckFilePathName( lpcszPathName ) )	// 检查文件名合法性
	{		
		type  = _GetObjName( lpcszPathName, &uLen ); 	// 得到文件对象类型
		if( type == 2 )
		{   // 文件对象 file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszPathName, &uLen );

			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol && lpVol->lpdsk->lpfsd->lpGetFileAttributes )
				{
					DWORD dwAttrib = lpVol->lpdsk->lpfsd->lpGetFileAttributes( lpVol->pVol, lpcszPathName + uLen );
					if( dwAttrib != -1 ) //	
					{	// 有效的属性
						if( !(dwAttrib & FILE_ATTRIBUTE_DEVICE) )
						{	// 不是设备文件夹
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
// 声明：BOOL WINAPI FileSys_SetEnd( HANDLE hFile )
// 参数：
// 	IN hFile - 文件对象
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	设置文件结束位置
// 引用: 
//	系统API
// **************************************************

BOOL WINAPI FileSys_SetEnd( HANDLE hFile )
{
    _LPFILEDATA lpFile = _GetHFILEPtr( hFile );
	BOOL bRetv = FALSE;

    if( lpFile )
    {
        if( lpFile->hVol )
        {   // 文件对象 file
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
// 声明：BOOL WINAPI KL_SetFileAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )

// 参数：
//	IN lpcszFileName-文件名
//	IN dwFileAttributes-文件属性,包含：
//			FILE_ATTRIBUTE_ARCHIVE-文档文件
//			FILE_ATTRIBUTE_HIDDEN-隐藏文件
//			FILE_ATTRIBUTE_READONLY-只读文件
//			FILE_ATTRIBUTE_SYSTEM-系统文件
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	设置文件属性
// 引用: 
//	内部使用
// **************************************************

BOOL WINAPI KL_SetFileAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )
{
    int type;
    UINT uLen;
	BOOL bRetv = FALSE;

    if( CheckFilePathName( lpcszFileName ) )  // 检查文件名的合法性
	{
		type  = _GetObjName( lpcszFileName, &uLen );	// 得到文件类型
		if( type == 2 )
		{   // 文件类型 file
			_LPVOLUME lpVol = _GetVolumePtr( lpcszFileName, &uLen ); // 得到文件的卷对象
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
// 声明：BOOL WINAPI FileSys_SetAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )

// 参数：
//	IN lpcszFileName-文件名
//	IN dwFileAttributes-文件属性,包含：
//			FILE_ATTRIBUTE_ARCHIVE-文档文件
//			FILE_ATTRIBUTE_HIDDEN-隐藏文件
//			FILE_ATTRIBUTE_READONLY-只读文件
//			FILE_ATTRIBUTE_SYSTEM-系统文件
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	设置文件属性
// 引用: 
//	系统API
// **************************************************
BOOL WINAPI FileSys_SetAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )
{
    dwFileAttributes &= ~SPECIAL_ATTRIB;
	return KL_SetFileAttributes( lpcszFileName, dwFileAttributes );
}

// **************************************************
// 声明：DWORD WINAPI FileSys_SetPointer(
//							HANDLE hFile,
//							LONG lDistanceToMove,
//							LPLONG lpDistanceToMoveHigh,
//							DWORD dwMoveMethod )
// 参数：
//	IN hFile - 文件对象
//	IN lDistanceToMove-相对偏移值（低32bits）
//	IN lpDistanceToMoveHigh-(高32bits,不支持,为NULL)
//	IN dwMoveMethod-偏移的起始位置，包含：
//		FILE_BEGIN-文件开始位置
//		FILE_CURRENT-文件当前位置
//		FILE_END-文件结束位置
//
// 返回值：
//	成功：返回新的文件位置
//	否则：返回0xffffffff
// 功能描述：
//	设置文件当前存取位置
// 引用: 
//	系统API
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
        {   // 文件对象 file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpSetFilePointer )
					dwRetv = lpVol->lpdsk->lpfsd->lpSetFilePointer( lpFile->pFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
				Sys_ReleaseException();
			}
        }
        else
        {   // 设备对象 device
			return Dev_SetFilePointer( (HANDLE)lpFile->pFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
        }
    }
    return dwRetv;
}

// **************************************************
// 声明：BOOL WINAPI FileSys_SetTime(
//						HANDLE hFile, 
//						const FILETIME *pCreate,
//						const FILETIME *pLastAccess,
//						const FILETIME *pLastWrite )
// 参数：
// 	IN hFile - 文件句柄
//	IN pCreate-FILETIME结构指针，文件创建时间
//	IN pLastAccess-FILETIME结构指针，文件存取时间(目前不支持，为NULL)
//	IN pLastWrite-FILETIME结构指针，文件更新时间(目前不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	设置文件时间
// 引用: 
//	系统API
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
// 声明：BOOL WINAPI FileSys_Write( 
//                  HANDLE hFile, 
//                  LPCVOID lpvBuffer,
//                  DWORD dwNumToWrite, 
//                  LPDWORD lpdwNumWrite,
//					LPOVERLAPPED pOverlapped)
// 参数：
//	IN hFile-文件对象
//	IN lpvBuffer-欲写的数据的存放内存
//	IN dwNumToWrite-欲写的字节数
//	OUT lpdwNumWrite-返回实际写的字节数；假如lpdwNumWrite为NULL,则不返回
//	IN pOverlapped-覆盖(不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	写数据到文件
// 引用: 
//	系统API
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
        {   // 文件对象 file
            _LPVOLUME lpVol = (_LPVOLUME)lpFile->hVol;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				if( lpVol->lpdsk->lpfsd->lpWriteFile )
					bRetv = lpVol->lpdsk->lpfsd->lpWriteFile( lpFile->pFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
				Sys_ReleaseException();
			}
        }
        else
        {  // 设备对象 device
			return Dev_WriteFile( (HANDLE)lpFile->pFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
        }
    }
    return bRetv;
}

// **************************************************
// 声明：BOOL WINAPI FileSys_WriteWithSeek( 
//                           HANDLE hFile, 
//                           LPCVOID lpcvBuffer, 
//                           DWORD dwWrite, 
//                           LPDWORD lpdwWritten, 
//                           DWORD dwLowOffset, 
//                           DWORD dwHighOffset ) 
// 参数：
//	IN hFile-文件对象
//	OUT lpcvBuffer-写入数据的内存
//	IN dwWrite-欲写的字节数
//	OUT lpdwWritten-返回实际写的字节数；假如 lpdwWritten 为NULL,则不返回
//	IN dwLowOffset - 偏移值的低32位
//	IN dwHighOffset - 偏移值的高32位
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	将文件存取位置设到由dwLowOffset dwHighOffset确定的值,然后写数据到文件
// 引用: 
//	系统API
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
// 声明：BOOL WINAPI FileSys_Copy(
//						LPCTSTR lpcszExistingFileName,
//						LPCTSTR lpcszNewFileName,
//						BOOL bFailIfExists
//						)
// 参数：
// 	IN lpcszExistingFileName – 源文件名（必须存在）
// 	IN lpcszNewFileName – 目标文件名（可以存在也可以不存在）
// 	IN bFailIfExists – 假如目标文件存在，是否该函数失败。
//						假如为TRUE并且目标文件已经存在，该函数失败；
//						假如为FALSE并且目标文件已经存在，该函数将清除目标文件的内容并将源文件内容拷贝到目标文件
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将文件内容从源文件拷贝到目标文件
// 引用: 
//	系统API
// **************************************************

BOOL WINAPI FileSys_Copy( LPCTSTR lpcszExistingFileName, LPCTSTR lpcszNewFileName, BOOL bFailIfExists )
{
    int typeOld, typeNew;
    UINT uOldLen, uNewLen;
    BOOL bRetv = FALSE;
    _LPVOLUME lpOldVol, lpNewVol;
	
    if( CheckFilePathName( lpcszExistingFileName ) && 
		CheckFilePathName( lpcszNewFileName ) )	// 检查文件名的合法性
	{
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			typeOld  = _GetObjName( lpcszExistingFileName, &uOldLen );  // 得到文件对象类型
			typeNew  = _GetObjName( lpcszNewFileName, &uNewLen );	// 得到文件对象类型
			if( typeOld == 2 && typeNew == 2 )
			{   // 都是文件类型 file
				// 得到文件对应的磁盘卷
				lpOldVol = _GetVolumePtr( lpcszExistingFileName, &uOldLen );
				lpNewVol = _GetVolumePtr( lpcszNewFileName, &uNewLen );
				if( lpOldVol == lpNewVol )
				{	// 相同的磁盘卷
					if( lpOldVol->lpdsk->lpfsd->lpCopyFile )
						bRetv = lpOldVol->lpdsk->lpfsd->lpCopyFile( lpOldVol->pVol, lpcszExistingFileName + uOldLen, lpcszNewFileName + uNewLen, bFailIfExists );
				}
				else
				{   // 不同的磁盘卷 volume is different
					DWORD dwOldAttrib = -1;
					
					if( lpOldVol->lpdsk->lpfsd->lpGetFileAttributes )				
						dwOldAttrib = lpOldVol->lpdsk->lpfsd->lpGetFileAttributes( lpOldVol->pVol, lpcszExistingFileName + uOldLen );
					
					if( !(dwOldAttrib & FILE_ATTRIBUTE_DIRECTORY) )
					{	// 非文件夹，打开文件 is file
						_LPFILEDATA hOldFile = _CreateFile(  lpcszExistingFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
						
						if( hOldFile != INVALID_HANDLE_VALUE )
						{	// 打开老文件成功
							DWORD dwCreateFlag = bFailIfExists ? CREATE_NEW : CREATE_ALWAYS;
							// 创建新文件
							_LPFILEDATA hNewFile = _CreateFile( lpcszNewFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, dwCreateFlag, 0, NULL ); 
							
							if( hNewFile != INVALID_HANDLE_VALUE )
							{	// 创建新文件成功
								FILETIME ft;
								// 得到老文件时间
								FileSys_GetTime( hOldFile, &ft, NULL, NULL );
								// 设置新文件时间
								bRetv = FileSys_SetTime( hNewFile, &ft, NULL, NULL );
								// 拷贝文件
								if( bRetv && 
									(bRetv = _CopyFile( hNewFile, hOldFile )) == TRUE )
								{
									;								
								}
								// 关闭新文件
								_CloseFile( hNewFile );
								if( bRetv == FALSE )  //
								{
									if( lpNewVol->lpdsk->lpfsd->lpDeleteFile )
										lpNewVol->lpdsk->lpfsd->lpDeleteFile( lpNewVol->pVol, lpcszNewFileName + uNewLen  );
								}
								else // 将新文件属性设置为与老文件相同 
									lpNewVol->lpdsk->lpfsd->lpSetFileAttributes( lpNewVol->pVol, lpcszNewFileName + uNewLen, dwOldAttrib & (~SPECIAL_ATTRIB)  );
							}
							// 关闭老文件
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


