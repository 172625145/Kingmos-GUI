/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：杂凑功能
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
    2003-06-18， 加入对ROM Filesys的支持    
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
//声明：BOOL WINAPI KL_CloseFile( HANDLE hFile )
//参数：
//	hFile - 文件句柄（由CreateFile创建并返回句柄）
//返回值：
//	假如成功，返回TURE；否则，返回FALSE
//功能描述：
//	关闭文件
//引用: 
//	由内核使用
// ********************************************************************
typedef BOOL ( WINAPI * PFILE_CLOSEFILE )( HANDLE );
#define DEBUG_KL_CloseFile 0
BOOL WINAPI KL_CloseFile( HANDLE hFile )
{
	PFILE_CLOSEFILE pCloseFile;
	CALLSTACK cs;
	//CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	BOOL   retv = FALSE;
    // 得到服务功能入口并切换到服务进程空间
	if( KC_EnterAPIHandle( API_FILESYS, FILE_CLOSEFILE, &pCloseFile, &cs, &hFile ) )
	{
		retv = pCloseFile( hFile );
		// 离开服务
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
//声明：HANDLE WINAPI KL_CreateFile( LPCTSTR lpcszFileName,
//							   DWORD dwAccess, 
//							   DWORD dwShareMode, 
//							   PSECURITY_ATTRIBUTES pSecurityAttributes, 
//							   DWORD dwCreate, 
//							   DWORD dwFlagsAndAttributes, 
//							   HANDLE hTemplateFile )
//
//参数：
//	lpcszFileName-文件名
//	fAccess-存取控制，由以下值的位组合：
//		GENERIC_WRITE-读操作
//		GENERIC_READ-写操作
//	dwShareMode-共享模式，由以下值的位组合：
//		 FILE_SHARE_READ-共享读
//		 FILE_SHARE_WRITE-共享写
//	pSecurityAttributes-安全设置(不支持，为NULL)
//	dwCreate-创建方式，包含：
//		CREATE_NEW-创建新文件；假如文件存在，则失败
//		CREATE_ALWAYS-创建文件；假如文件存在，则覆盖它
//		OPEN_EXISTING-打开文件；假如文件不存在，则失败
//		OPEN_ALWAYS-打开文件；假如文件不存在，则创建
//	dwFlagsAndAttributes-文件属性
//	hTemplateFile-临时文件句柄(不支持,为NULL)


//返回值：
//	假如成功，返回有效的文件句柄；否则，返回INVALID_HANDLE_VALUE
//功能描述：
//	打开文件
//引用: 
//	由内核使用
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
	
	// 得到服务功能入口并切换到服务进程空间

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
//声明：DWORD WINAPI KL_GetFileAttributes( LPCTSTR lpcszFileName )
//参数：
//	lpcszFileName-文件名
//返回值：
//	假如成功，返回属性；否则，返回0xFFFFFFFF
//功能描述：
//	得到文件属性
//引用: 
//	由内核使用
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
//声明：BOOL WINAPI KL_ReadFile( HANDLE hFile, 
//					   LPVOID lpvBuffer, 
//					   DWORD dwNumToRead, 
//					   LPDWORD lpdwNumRead, 
//					   LPOVERLAPPED lpOverlapped )

//参数：
//	hFile-文件句柄
//	lpvBuffer-用于接受数据的内存
//	dwNumToRead-欲读的字节数
//	lpdwNumRead-返回实际读的字节数；假如lpdwNumRead为NULL,则不返回
//	lpOverlapped-覆盖(不支持，为NULL)

//返回值：
//	假如成功，返回TRUE，lpdwNumRead保存实际读的数据；否则，返回FALSE
//功能描述：
//	从文件里读数据
//引用: 
//	由内核使用
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
//声明：BOOL WINAPI KL_WriteFile( HANDLE hFile, 
//					    LPCVOID lpvBuffer, 
//						DWORD dwNumToWrite, 
//						LPDWORD lpdwNumWrite, 
//						LPOVERLAPPED pOverlapped )

//参数：
//	hFile-文件句柄
//	lpvBuffer-欲写的数据的存放内存
//	dwNumToWrite-欲写的字节数
//	lpdwNumWrite-返回实际写的字节数；假如lpdwNumWrite为NULL,则不返回
//	pOverlapped-覆盖(不支持，为NULL)

//返回值：
//	假如成功，返回TRUE，lpdwNumWrite保存实际写的数据；否则，返回FALSE
//功能描述：
//	写数据到文件
//引用: 
//	由内核使用
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
//声明：DWORD WINAPI KL_SetFilePointer( HANDLE hFile, 
//							  LONG lDistanceToMove, 
//							  LPLONG lpDistanceToMoveHigh, 
//							  DWORD dwMoveMethod )
//参数：
//hFile-文件句柄
//lDistance-相对偏移值
//lpDistanceHigh-(高32bits,不支持,为NULL)
//dwMethod-偏移的起始位置，包含：
//    FILE_BEGIN-文件开始位置
//    FILE_CURRENT-文件当前位置
//    FILE_END-文件结束位置
//返回值：
//	假如成功，返回返回新的文件位置；否则，返回0xffffffff
//功能描述：
//	设置存取文件位置
//引用: 
//	由内核使用
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
//声明：BOOL WINAPI KL_DeviceIoControl( 
//                          HANDLE hFile,
//                          DWORD dwIoControlCode,
//                          LPVOID lpInBuf,
//                          DWORD nInBufSize,
//                          LPVOID lpOutBuf,
//                          DWORD nOutBufSize,
//                          LPDWORD lpdwBytesReturned,
//                          LPOVERLAPPED lpOverlapped )
//参数：
//hFile-文件句柄
//dwIoControlCode-I/O控制命令，文件系统包含：
//    IOCTL_DISK_SCAN_VOLUME－通知文件系统去扫描卷的错误
//    IOCTL_DISK_FORMAT_VOLUME－通知文件系统去格式化卷
//    lpInBuf-输入缓存
//    nInBufSize－输入缓存的尺寸
//    lpOutBuf－输出缓存
//    nOutBufSize－输出缓存的尺寸
//    lpBytesReturned-返回的数据的尺寸，该数据在返回时已存入lpOutBuf
//    lpOverlapped-支持覆盖(不支持，为NULL)
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	文件控制
//引用: 
//	由内核使用
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
//声明：HANDLE WINAPI KL_LoadImage( 
//                     HINSTANCE hinst,
//                     LPCTSTR lpszName, 
//                     UINT uType, 
//                     int cxDesired, 
//                     int cyDesired, 
//                     UINT fuLoad 
//                     )
//参数：
//	hinst - 模块句柄
//	lpszName - 需要装入的映象名。假如hinst为NULL并且fuLoad标志含有LR_LOADFROMFILE，该参数表示一个文件名；假如hinst为非NULL并且fuLoad标志没有LR_LOADFROMFILE，则该参数表示一个在资源里的映象名。用MAKEINTRESOURCE去转化资源ID值为LPCTSTR类型值
//	uType - 需要装入的资源类型，可以为以下值：
//		IMAGE_BITMAP - 位图
//		IMAGE_CURSOR - 光标
//		IMAGE_ICON - 图标
//	cxDesired - 需要装入的光标或图标资源宽度，假如是0并且fuLoad含有LR_DEFAULTSIZE标志，
//				则系统用SM_CXICON或SM_CXCURSOR系统度量去设置该值，假如是0并且fuLoad没有LR_DEFAULTSIZE标志，则系统用资源的真实的宽度
//	cyDesired - 需要装入的光标或图标资源高度，假如是0并且fuLoad含有LR_DEFAULTSIZE标志，
//				则系统用SM_CXICON或SM_CXCURSOR系统度量去设置该值，假如是0并且fuLoad没有LR_DEFAULTSIZE标志，则系统用资源的真实的高度
//	fuLoad - 为以下值：
//		LR_LOADFROMFILE - 从文件装入资源
//		LR_DEFAULTSIZE - 用默认的系统度量
//		LR_SHARED - 是否共享该资源。假如该标志设置，第二次装入该资源不会产生新的拷贝，只会增加该资源的引用计数；否则会产生新的拷贝并返回不同的句柄
//返回值：
//	假如成功，返回有效的句柄值，负责返回NULL
//功能描述：
//	装入一个位图、图标、光标
//引用:
//	由内核使用
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
//声明：int WINAPI KL_GetSysMetrics( int nIndex )
//参数：
//	IN nIndex - 索引值
//返回值：
//	返回量度值
//功能描述：
//	返回系统量度
//引用:
//	由内核使用
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
//声明：BOOL WINAPI KL_DebugOutString( LPTSTR lpszStr )
//参数：
//	IN lpszStr - 字符串
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	输出调试信息
//引用:
//	系统API
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
//声明：LPCVOID WINAPI KL_GetSysTOC( void )
//参数：
//	无
//返回值：
//	返回TOC
//功能描述：
//	返回系统的TOC信息
//引用:
//	系统API
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
//声明：BOOL WINAPI KL_GetComputerNameEx(
//								 COMPUTER_NAME_FORMAT NameType,  // name type
//								 LPTSTR lpBuffer,                // name buffer
//								 LPDWORD lpnSize                 // size of name buffer
//								 )
//参数：
//	IN NameType - 类型，包含：
//				ComputerNameNetBIOS - NetBIOS名
//	OUT lpBuffer - 用于接受数据的内存
//	IN/OUT lpnSize - 输入lpBuffer的长度，输出需要拷贝的字符串长度（含'\0'结束符）
//返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
//功能描述：
//	得到计算机名
//引用:
//	系统API
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
//声明：BOOL WINAPI KL_SetComputerNameEx(
//								 COMPUTER_NAME_FORMAT NameType,  // name type
//								 LPCTSTR lpBuffer                // name buffer
//								 )
//参数：
//	IN NameType - 类型，包含：
//				ComputerNameNetBIOS - NetBIOS名
//	IN lpBuffer - 需要设置的名字
//返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
//功能描述：
//	设置计算机名
//引用:
//	系统API
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
		//释放之前的名字		
		if( lpszComputerNameNetBIOS )
			KHeap_FreeString( lpszComputerNameNetBIOS );
		len = strlen( lpBuffer ) + 1;
		//重新分配内存
	    lpszComputerNameNetBIOS = KHeap_AllocString( len );
	    if( lpszComputerNameNetBIOS )
		    strcpy( lpszComputerNameNetBIOS, lpBuffer );
	}
	else
		KL_SetLastError( ERROR_INVALID_PARAMETER );
	return bRetv;
}


// ********************************************************************
//声明：BOOL WINAPI KL_RegistryIO( DWORD dwFlag, LPVOID lpDataBuf, DWORD nNumberOfBytes );
//参数：
//	IN/OUT lpDataBuf - 用于保存或写入的数据
//  IN nNumberOfBytes - lpDataBuf 的数据量
//	IN dwFlag - 以下类型的组合:
//                   REGIO_BEGIN - 第一次操作
//                   REGIO_READ -  读
//                   REGIO_WRITE - 写
//                   REGIO_END -   结束操作
//返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
//功能描述：
//	输入输出注册表
//引用:
//	系统API
// ********************************************************************

LPOEM_REGISTRYIO lpOEM_RegistryIO;

BOOL WINAPI KL_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
{
	//if( (dwFlag & REGIO_READ) && (dwFlag & REGIO_WRITE) )
	//{ //不能同时为读写
	//	KL_SetLastError( ERROR_INVALID_PARAMETER );
	//	return FALSE;
	//}

	if( lpOEM_RegistryIO )
		return lpOEM_RegistryIO( lpDataBuf, nNumberOfBytes, dwFlag );
	return FALSE;
}

// *****************************************************************
// 声明：VOID DumpCallStack( LPTHREAD lpThread )
// 参数：
//		IN lpThread - 线程结构指针
// 返回值：
//		无
// 功能描述：
//		一个调试函数，用于输出当前的系统调用链
// 引用:
//		内部使用
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
// 声明：BOOL WINAPI KL_KernelIoControl(
//				   HANDLE hOpenFile,
//				   DWORD  dwIoControlCode,
//				   LPVOID lpvInBuf,
//				   DWORD dwInBufSize,
//				   LPVOID lpvOutBuf,
//				   DWORD dwOutBufSize,
//				   LPDWORD lpdwBytesReturned,
//				   LPOVERLAPPED lpOverlapped )
// 参数：
//		IN hOpenFile - 已打开文件句柄
//		IN dwIoControlCode - I/O控制代码
//		IN lpvInBuf - 输入参数内存地址
//		IN dwInBufSize - lpvInBuf的内存大小
//		OUT lpvOutBuf - 输出数据内存地址
//		IN dwOutBufSize - lpvOutBuf的内存大小
//		OUT lpdwBytesReturned - 写入lpvOutBuf的数据大小
//		IN lpOverlapped - 不支持（为NULL）
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		内核I/O控制函数入口，通过发不同的控制代码已实现对内核的管理控制
// 引用:
//		系统API 
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


