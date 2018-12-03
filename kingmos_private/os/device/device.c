/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：设备管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：

******************************************************/

#include <eframe.h>
#include <edevice.h>
#include <efile.h>
#include <eapisrv.h>
#include <eobjlist.h>
#include <epdev.h>
#include <devsrv.h>

// struct define 

// 文件设备结构
typedef struct _DEVFSD
{
    struct _DEVFSD * lpNext;

	LPFSDINITDRV lpFSDInitDrv;
	HANDLE hFSDRegister;
	LPTSTR lpszFSDName;	
	int iRefCount;
}DEVFSD, FAR * LPDEVFSD;

// 设备注册结构
typedef struct _DEVICE_DATA
{
	OBJLIST obj;

    LPTSTR lpszName;
    BYTE index;
    BYTE bFree;
	WORD wReserve;
    const DEVICE_DRIVER * lpDriver;
    DWORD hDevInit;   // init handle
	// file sys
	LPDEVFSD lpfsd;
	HANDLE hFSDAttach;
}DEVICE_DATA, FAR * LPDEVICE_DATA;

// 设备打开结构
typedef struct _DEVICE_OPEN_DATA
{
	OBJLIST obj;

	LPDEVICE_DATA lpDev;
	DWORD  hDevOpen;
    HANDLE hProcess;
}DEVICE_OPEN_DATA, FAR * LPDEVICE_OPEN_FILE;

// 全局数据
static LPDEVICE_DATA lpDeviceObjList = NULL;  //已注册设备列式
static LPDEVICE_OPEN_FILE lpDeviceOpenObjList = NULL; //已打开设备列式
static CRITICAL_SECTION csDeviceObjList; //临界段

static const TCHAR szDll[]= "Dll";
static const TCHAR szIndex[] = "Index";
static const TCHAR szPrefix[] = "Prefix";
static const TCHAR szFlags[] = "Flags";
static const TCHAR szActiveKey[] = "Drivers\\Active";
static const TCHAR szBuiltInKey[] = "Drivers\\BuiltIn";
static const TCHAR szHandle[] = "handle";
// 函数声名
static LPOBJLIST FindDevice( LPCTSTR lpszName, UINT index );
//

extern BOOL SaveAsRegister(LPTSTR lpRegisterFile);

// *****************************************************************
//声明：static LPDEVICE_DATA _GetHDEVPtr( HANDLE handle )
//参数：
//	IN handle-设备句柄

//返回值：
//	成功，返回设备数据指针;失败，返回NULL
//功能描述：要访问设备数据，必须调用该函数以得到设备数据
//引用: 要访问设备数据的代码
// *****************************************************************

static LPDEVICE_DATA _GetHDEVPtr( HANDLE handle )
{
	
    DEVICE_DATA * lp = handle;

	if( lp && lp->obj.objType == OBJ_DEV )
		return lp;
	ASSERT_NOTIFY( 0, "Invalid device handle\r\n" );
	return NULL;
}

// *****************************************************************
//声明：static LPDEVICE_OPEN_FILE _GetHDEVFilePtr( HANDLE hFile )
//参数：
//	IN hFile-设备打开句柄

//返回值：
//	成功，返回设备打开数据指针;失败，返回NULL
//功能描述：
//	要访问设备打开数据，必须调用该函数以得到设备打开数据
//引用: 
//	要访问设备打开数据的代码
// *****************************************************************

static LPDEVICE_OPEN_FILE _GetHDEVFilePtr( HANDLE hFile )
{
	LPDEVICE_OPEN_FILE lpFile = (HANDLE)hFile;
	if( lpFile && lpFile->lpDev && lpFile->obj.objType == OBJ_DEVOPEN )
	{
		return lpFile;
	}
	return NULL;
}


// *****************************************************************
//声明：HANDLE DoRegisterDriver( LPCTSTR lpDevName,
//                                     int index, 
//                                     const DEVICE_DRIVER FAR * lpDriver,
//                                     LPVOID lpParam )
//参数：
//	IN lpDevName-设备类型名
//	IN index-设备索引号
//  IN lpDriver-设备调用界面指针
//  IN lpParam-调用该设备的Init函数时将传递该参数
//返回值：
//	成功，返回设备句柄;失败，返回NULL
//功能描述：
//	调用设备驱动程序的初始化函数，假如成功，将该设备接口加入设备链表。
//引用: 

// *****************************************************************

#define DEBUG_DOREGISTERDRIVER 0
static HANDLE DoRegisterDriver( LPCTSTR lpDevName, UINT index, const DEVICE_DRIVER FAR * lpDriver, LPVOID lpParam, BOOL bFreeStruct )
{
    int s;
    DEVICE_DATA * lpdd;
    if( index < 0 || index > 9 )
        return NULL;

	DEBUGMSG( DEBUG_DOREGISTERDRIVER, ( "Device_Register:%s,lpDriver=%x.\r\n", lpDevName, lpDriver ) );

    s = strlen( lpDevName );
    lpdd = malloc( sizeof( DEVICE_DATA ) + s + 2 );
    if( lpdd )
    {
        memset( lpdd, 0, sizeof( DEVICE_DATA ) + s + 2 );
        lpdd->hDevInit = lpDriver->lpInit( (DWORD)lpParam ); // 调用设备驱动程序接口的初始化函数
        if( lpdd->hDevInit )  // 初始化成功 ？
        {	// 是，将该设备加入设备链表
            lpdd->lpszName = (LPTSTR)(lpdd + 1);
            strcpy( lpdd->lpszName, lpDevName );
            *(lpdd->lpszName + s) = '0' + index;
            *(lpdd->lpszName + s + 1) = 0;

            lpdd->index = index;
            lpdd->lpDriver = lpDriver;
			lpdd->hFSDAttach = NULL;
			lpdd->lpfsd = NULL;
			lpdd->bFree = bFreeStruct;

			EnterCriticalSection( &csDeviceObjList );	

			ObjList_Init( (LPOBJLIST*)&lpDeviceObjList, &lpdd->obj, OBJ_DEV, (ULONG)GetCurrentProcess() );//(ULONG)GetCurrentProcessId() );
 
			LeaveCriticalSection( &csDeviceObjList );	

            return (HANDLE)lpdd;
        }
		else
		{
			WARNMSG( DEBUG_DOREGISTERDRIVER, ( "Device_Register: Device(%s) Index(%d) Init failure!.\r\n.", lpDevName, index ) );
		}
        free( lpdd );
    }
    return NULL;
}

// *****************************************************************
//声明：HANDLE WINAPI Device_RegisterDriver( LPCTSTR lpDevName,
//                                     int index, 
//                                     const DEVICE_DRIVER FAR * lpDriver,
//                                     LPVOID lpParam )
//参数：
//	IN lpDevName-设备类型名
//	IN index-设备索引号
//  IN lpDriver-设备调用界面指针
//  IN lpParam-调用该设备的Init函数时将传递该参数
//返回值：
//	成功，返回设备句柄;失败，返回NULL
//功能描述：
//	注册设备驱动程序。静态连接版本
//引用: 
//	系统API
// *****************************************************************
HANDLE WINAPI Dev_RegisterDriver( LPCTSTR lpDevName, UINT index, const DEVICE_DRIVER FAR * lpDriver, LPVOID lpParam )
{
	return DoRegisterDriver( lpDevName, index, lpDriver, lpParam, FALSE );
}

// *****************************************************************
//声明：HANDLE WINAPI Device_RegisterDevice( LPCTSTR lpDevName, UINT index, LPCTSTR lpszLib, LPVOID lpParam )
//参数：
//	IN lpDevName - 设备名
//	IN index - 设备索引名
//	IN lpszLib - 动态连接库文件名
//	IN lpParam - 传递给设备初始化函数的参数
//返回值：
//	假如成功返回非NULL的句柄,否则，返回NULL
//功能描述：
//	注册设备驱动程序。动态连接库版本
//引用: 
//	系统API
// *****************************************************************

HANDLE WINAPI Device_RegisterDevice( LPCTSTR lpDevName, UINT index, LPCTSTR lpszLib, LPVOID lpParam )
{
	HANDLE handle = NULL;
	//check param
	if( strlen( lpDevName ) != 3 || index <= 9 )
	{
		HMODULE hModule = LoadLibrary( lpszLib );
		
		if( hModule )
		{
			const LPCTSTR szDrvName[] = { "_Init",
				                          "_Deinit",
										  "_IOControl",
										  "_Open",
										  "_Close",
										  "_Read",
										  "_Write",
										  "_Seek",
										  "_PowerUp",
										  "_PowerDown" };

			DEVICE_DRIVER * lpDriver = malloc( sizeof(DEVICE_DRIVER) );
			if( lpDriver )
			{
				int i;
				PFNVOID * lppFun = (PFNVOID *)lpDriver;
				TCHAR bufName[32];
				memset( lpDriver, 0, sizeof(DEVICE_DRIVER) );

				for( i = 0; i < sizeof(szDrvName) / sizeof(LPCTSTR); i++, lppFun++ )
				{
					strcpy( bufName, lpDevName );
					strcat( bufName, szDrvName[i] ); 
					
					*lppFun = (PFNVOID)GetProcAddress( hModule, bufName );
					if( *lppFun == 0 )
					{
						goto _error_return;
					}
				}
				//ok
				if( (handle = DoRegisterDriver( lpDevName, index, lpDriver, lpParam, TRUE )) == NULL )
				{
					free( lpDriver );
				}
			}
		}
	}
	else
	{
		SetLastError( ERROR_INVALID_PARAMETER );
	}
_error_return:
	return handle;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_Deregister( HANDLE handle )
// 参数：
//		IN handle - 设备句柄（由RegisterDevice 或 RegisterDriver返回的句柄）
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		注销设备驱动程序
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_Deregister( HANDLE handle )
{
    DEVICE_DATA * lpdd = _GetHDEVPtr( handle ); // 由句柄得到设备数据结构指针
	LPOBJLIST lpDevOpen;
	
	BOOL bRetv = FALSE;

	if( lpdd  )
	{
        EnterCriticalSection( &csDeviceObjList ); // 进入冲突段

		lpDevOpen = (LPOBJLIST)lpDeviceOpenObjList; //全局设备打开/引用对象链表
		// 查找已打开该设备的对象，如果有则注销它
		for( ; lpDevOpen; lpDevOpen = lpDevOpen->lpNext )
		{
			if( ( (LPDEVICE_OPEN_FILE)lpDevOpen )->lpDev == lpdd )  // 
			{   // 找到该对象
				( (LPDEVICE_OPEN_FILE)lpDevOpen )->lpDev = NULL; // 不许再用
				Interlock_Decrement( (LPLONG)&lpdd->obj.iRefCount ); // 减少引用
			}
		}
		// 该对象是否与某文件系统关联 ？
		if( lpdd->hFSDAttach && lpdd->lpfsd )
		{	//是，通知该文件系统去注销它
			RETAILMSG( 1, ( "Detach device.\r\n." ) );
			lpdd->lpfsd->lpFSDInitDrv->lpDetachDevice( lpdd->hFSDAttach );
			--lpdd->lpfsd->iRefCount;// 减少对该文件系统的引用
		}
		// 将该设备对象从系统对象链表去掉
	    if( ObjList_Remove( (LPOBJLIST*)&lpDeviceObjList,  &lpdd->obj ) )
		{	// 通知设备驱动程序的释放函数并释放该设备对象
			lpdd->lpDriver->lpDeinit( lpdd->hDevInit );
			lpdd->obj.objType = OBJ_NULL;
			if( lpdd->bFree )
				free( (LPVOID)lpdd->lpDriver );
			free( lpdd );
			bRetv = TRUE;
		}
		LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段
	}
	return bRetv;
}

// *****************************************************************
// 声明：static DWORD FASTCALL EnumDevice( LPDEVICE_ENUM_PROC lpEnumFunc, LPVOID lpParam )
// 参数：
//		IN lpEnumFunc - 枚举回调函数入口
//		IN lpParam - 传递给回调函数的参数
// 返回值：
//		0	
// 功能描述：
//		枚举所有设备对象
// 引用: 
// *****************************************************************

typedef BOOL ( CALLBACK * LPDEVICE_ENUM_PROC )( LPDEVICE_DATA lpDev, LPVOID lpParam );
static DWORD FASTCALL EnumDevice( LPDEVICE_ENUM_PROC lpEnumFunc, LPVOID lpParam )
{
	LPDEVICE_DATA lpDevList;

	lpDevList = lpDeviceObjList;
	
	for( ; lpDevList ; lpDevList = (LPDEVICE_DATA)lpDevList->obj.lpNext )
	{
		if( lpEnumFunc( lpDevList, lpParam ) == FALSE )
			break;
	}
	return 0;
}

// *****************************************************************
// 声明：DWORD WINAPI Device_Enum( LPTSTR lpszDevList, LPDWORD lpdwBuflen )
// 参数：
//		OUT lpszDevList – 用于接受设备名列式的缓存，返回的数据的格式为:“COM0:\0PRN1:\0USB5\0\0”。
//		IN lpdwBufLen - lpszDevList缓存的大小，假如缓存不足以保存数据，则填入需要的缓存大小
// 返回值：
//		ERROR_SUCCESS －　成功
//		ERROR_INSUFFICIENT_BUFFER － 缓存的大小不足以保存任何一个数据
//		ERROR_INVALID_PARAMETER　－　无效的参数
//		ERROR_MORE_DATA　－　缓存的大小不足以保存所有的数据
//		ERROR_NO_MORE_DEVICES　－　没有设备
// 功能描述：
//		枚举当前系统的设备，返回其设备名
// 引用: 
//		系统API
// *****************************************************************

DWORD WINAPI Device_Enum( LPTSTR lpszDevList, LPDWORD lpdwBuflen )
{
	LPDEVICE_DATA lpDevList;
	DWORD dwBuflen;
	DWORD dwDeviceLen;
	DWORD dwRetv;
	LPTSTR lpstrBuf;

	//	检查参数
	if( lpdwBuflen == NULL || lpszDevList == NULL )
	{
		return ERROR_INVALID_PARAMETER;
	}

	dwDeviceLen = 0;
	dwRetv = ERROR_SUCCESS;
	lpstrBuf = lpszDevList;
	dwBuflen = *lpdwBuflen - 1; // 减一是为最后的\0结束符

	EnterCriticalSection( &csDeviceObjList );

	lpDevList = lpDeviceObjList;
	// 搜索所有的设备对象，并将它们的名字添入lpszDevList
	for( ; lpDevList ; lpDevList = (LPDEVICE_DATA)lpDevList->obj.lpNext )
	{
		// 2 one is null char , one is ':'. because the lpDevList->lpszName no ':', example:"COM1"
		UINT len = strlen( lpDevList->lpszName ) + 2;
		dwDeviceLen += len;
		if( dwDeviceLen < dwBuflen )// 用户给的内存够吗 ？
		{	//够
		    strcpy( lpstrBuf, lpDevList->lpszName );
			*(lpstrBuf+len-2) = ':';
			*(lpstrBuf+len-1) = 0;
			lpstrBuf += len;
		}
		else
			dwRetv = !ERROR_SUCCESS; // 不够，继续循环已得到真实的内存长度
	}
    if( *lpdwBuflen )
	    *lpstrBuf = 0;
	dwDeviceLen++; // 总需要的长度（包括最后的'\0'）
	if( lpstrBuf != lpszDevList )
	{   // 已经保存了数据。has data in lpstrbuf, but may be not all data
		if( dwRetv == !ERROR_SUCCESS ) // 是否保存完所有的数据 ？
		{	//否，设置需要的长度和返回值
			*lpdwBuflen = dwDeviceLen;
			dwRetv = ERROR_MORE_DATA;
		}
	}
	else
	{  // 没有保存任何数据。maybe ERROR_INSUFFICIENT_BUFFER  or ERROR_NO_MORE_DEVICES 
		if( lpDeviceOpenObjList == NULL )
			dwRetv = ERROR_NO_MORE_DEVICES; // 没有如何设备
		else
		{
			*lpdwBuflen = dwDeviceLen;
			dwRetv = ERROR_INSUFFICIENT_BUFFER; // 内存长度不足以保存任何数据
		}
	}
	
	LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段

	return dwRetv;
}

// *****************************************************************
// 声明：HANDLE WINAPI Device_CreateFile( 
//							 LPCTSTR lpszName, 
//							 DWORD dwAccess, 
//							 DWORD dwShareMode, 
//							 HANDLE hProc )
// 参数：
//		IN lpszName - 设备名，例如"COM0:","PRN1:" 
//		IN dwAccess - 存取属性
//		IN dwShareMode - 共享模式
//		IN hProc - 拥有者进程句柄
// 返回值：
//		假如成功，返回有效的文件句柄；失败，返回INVALID_HANDLE_VALUE
// 功能描述：
//		打开指定的设备
// 引用: 
//		系统API
// *****************************************************************
#define DEBUG_DEVICE_CREATEFILE 0
HANDLE WINAPI Device_CreateFile( LPCTSTR lpszName, 
							 DWORD dwAccess, 
							 DWORD dwShareMode, 
							 HANDLE hProc )
{
    LPDEVICE_OPEN_FILE lpOpenFile = NULL;
	LPDEVICE_DATA lpDevObj;
	UINT index;

	DEBUGMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: lpszDevName=%s,dwAccess=0x%x,dwShareMode=0x%x.\r\n", lpszName, dwAccess, dwShareMode ) );

	// 检查参数
	if( lpszName == NULL )
	{
		return INVALID_HANDLE_VALUE;  
	}
    if( strlen( lpszName ) < 4 )
		return INVALID_HANDLE_VALUE;
	if( *(lpszName + 3) < '0' || *(lpszName + 3) > '9' )
		return INVALID_HANDLE_VALUE;
	
	index = *(lpszName + 3) - '0';

	if( (lpDevObj = (LPDEVICE_DATA)FindDevice( lpszName, index ) ) ) // 在系统设备对象链表里查找是否有该设备
	{	//系统已经注册
		DEBUGMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: has find device=%s.\r\n", lpszName ) );
		//分配/准备打开设备对象数据结构
		lpOpenFile = malloc( sizeof(DEVICE_OPEN_DATA) );
		if( lpOpenFile )
		{
			lpOpenFile->hProcess = hProc;
			lpOpenFile->lpDev = lpDevObj;
			//调用设备驱动程序的打开函数
			lpOpenFile->hDevOpen = 0;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				lpOpenFile->hDevOpen = lpDevObj->lpDriver->lpOpen( lpDevObj->hDevInit, dwAccess, dwShareMode );
				Sys_ReleaseException();
			}
			if( lpOpenFile->hDevOpen )  //打开成功 ？
			{  // 是
				Interlock_Increment( (LPLONG)&lpDevObj->obj.iRefCount ); // 增加对该设备的引用数
				//加入打开链表
			    ObjList_Init( (LPOBJLIST*)&lpDeviceOpenObjList, &lpOpenFile->obj, OBJ_DEVOPEN, (ULONG)hProc );
				return (HANDLE)lpOpenFile;
			}
			else
			{
				WARNMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: Dev_Open return NULL.\r\n" ) );
			}
		}
		else
		{
			WARNMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: no enougn memory.\r\n" ) );
		}
		if( lpOpenFile )
		{
		    free( lpOpenFile );
		    lpOpenFile = NULL;
		}
	}
	else
	{
		WARNMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: not find the device(%s).\r\n", lpszName ) );
	}
//	WARNMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: not find the device(%s) or no memory!.\r\n", lpszName ) );
	return INVALID_HANDLE_VALUE;  
}

// *****************************************************************
// 声明：BOOL WINAPI Device_CloseFile( HANDLE hOpenFile )
// 参数：
//		IN hOpenFile - 调用Device_Create返回的句柄 
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		关闭之前打开的设备
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_CloseFile( HANDLE hOpenFile )
{
    BOOL bRetv = FALSE;
	LPDEVICE_OPEN_FILE lpOpenFile = (HANDLE)hOpenFile;

	if( lpOpenFile && lpOpenFile->obj.objType == OBJ_DEVOPEN )
	{	// 从对象链表移出
		ObjList_Remove( (LPOBJLIST*)&lpDeviceOpenObjList , &lpOpenFile->obj );
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			if( lpOpenFile->lpDev )
				lpOpenFile->lpDev->lpDriver->lpClose( lpOpenFile->hDevOpen ); // 调用设备驱动程序的关闭函数
			Sys_ReleaseException();
		}
		lpOpenFile->obj.objType = OBJ_NULL;
		free( lpOpenFile );
		bRetv = TRUE;
	}
	return bRetv;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_ReadFile( 
//				  HANDLE hOpenFile,
//				  LPVOID lpvBuffer, 
//				  DWORD dwBytesToRead, 
//				  LPDWORD lpdwNumBytesRead,
//				  LPOVERLAPPED lpOverlapped )
// 参数：
//		IN hOpenFile - 打开文件句柄
//		OUT lpvBuffer - 读入数据的内存
//		IN dwBytesToRead - 需要读的数据大小
//		IN lpdwNumBytesRead - 用来接受真实读取的数据大小
//		IN lpOverlapped - 不支持(必须为NULL)
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		从设备读取数据
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_ReadFile( 
				  HANDLE hOpenFile,
				  LPVOID lpvBuffer, 
				  DWORD dwBytesToRead, 
				  LPDWORD lpdwNumBytesRead,
				  LPOVERLAPPED lpOverlapped )
{
    LPDEVICE_OPEN_FILE lpOpenFile = _GetHDEVFilePtr( hOpenFile );
    BOOL bRetv = FALSE;

	if( lpOpenFile )
	{	//简单调用设备驱动程序的读函数
		*lpdwNumBytesRead = 0xffffffff;
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			*lpdwNumBytesRead = lpOpenFile->lpDev->lpDriver->lpRead( lpOpenFile->hDevOpen, lpvBuffer, dwBytesToRead );
			Sys_ReleaseException();
		}
		if( *lpdwNumBytesRead == 0xffffffff )
			*lpdwNumBytesRead = 0;
		else
			bRetv = TRUE;
	}
	return bRetv;

}

// *****************************************************************
// 声明：BOOL WINAPI Device_WriteFile( 
//				   HANDLE hOpenFile, 
//				   LPCVOID lpvBuffer, 
//				   DWORD dwBytesToWrite,
//				   LPDWORD lpdwNumBytesWritten,
//				   LPOVERLAPPED lpOverlapped )
// 参数：
//		IN hOpenFile - 打开文件句柄
//		OUT lpvBuffer - 写入设备的数据内存
//		IN dwBytesToWrite - 需要写的数据大小
//		IN lpdwNumBytesWritten - 用来接受真实写的数据大小
//		IN lpOverlapped - 不支持(必须为NULL)
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		写数据到设备
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_WriteFile( 
				   HANDLE hOpenFile, 
				   LPCVOID lpvBuffer, 
				   DWORD dwBytesToWrite,
				   LPDWORD lpdwNumBytesWritten,
				   LPOVERLAPPED lpOverlapped )
{
    LPDEVICE_OPEN_FILE lpOpenFile = _GetHDEVFilePtr( hOpenFile );
    BOOL bRetv = FALSE;

	*lpdwNumBytesWritten = 0;
	if( lpOpenFile )
	{	//简单调用设备驱动程序的写函数
		*lpdwNumBytesWritten = 0xffffffff;
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			*lpdwNumBytesWritten = lpOpenFile->lpDev->lpDriver->lpWrite( lpOpenFile->hDevOpen, lpvBuffer, dwBytesToWrite );
			Sys_ReleaseException();
		}
		if( *lpdwNumBytesWritten == 0xffffffff )
			*lpdwNumBytesWritten = 0;
		else
			bRetv = TRUE;
	}
	return bRetv;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_FlushFileBuffers( HANDLE hOpenFile ) 
// 参数：
//		IN hOpenFile - 打开文件句柄
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		将所有数据刷新到设备
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_FlushFileBuffers( HANDLE hOpenFile ) 
{	//暂时不支持
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_SetFileTime(
// 				     HANDLE hOpenFile, 
//					 CONST FILETIME *lpCreation, 
//					 CONST FILETIME *lpLastAccess, 
//					 CONST FILETIME *lpLastWrite ) 
// 参数：
//		IN hOpenFile - 打开文件句柄
//		IN lpCreation - 文件创建时间 
//		IN lpLastAccess - 文件最后存取时间
//		IN lpLastWrite  - 文件最后写入时间
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		设置文件时间
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_SetFileTime(
 				     HANDLE hOpenFile, 
					 CONST FILETIME *lpCreation, 
					 CONST FILETIME *lpLastAccess, 
					 CONST FILETIME *lpLastWrite ) 
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_GetFileTime(
// 				     HANDLE hOpenFile, 
//					 LPFILETIME lpftCreation, 
//					 LPFILETIME lpftLastAccess, 
//					 LPFILETIME lpftLastWrite ) 
// 参数：
//		IN hOpenFile - 打开文件句柄
//		OUT lpftCreation - 文件创建时间 
//		OUT lpftLastAccess - 文件最后存取时间
//		OUT lpftLastWrite  - 文件最后写入时间
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		得到文件时间
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_GetFileTime(
					HANDLE hOpenFile, 
					LPFILETIME lpftCreation, 
					LPFILETIME lpftLastAccess, 
					LPFILETIME lpftLastWrite ) 
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// 声明：DWORD WINAPI Device_GetFileSize( 
//						HANDLE hOpenFile, 
//						LPDWORD lpdwFileSizeHigh )

// 参数：
//		IN hOpenFile - 打开文件句柄
//		IN lpdwFileSizeHigh - 文件大小的高32位数据
// 返回值：
//		假如成功，返回文件大小的低32位数据，lpdwFileSizeHigh为文件大小的高32位数据；否则，返回0xffffffff
// 功能描述：
// 引用:
//		系统API 
// *****************************************************************

DWORD WINAPI Device_GetFileSize( 
						HANDLE hOpenFile, 
						LPDWORD lpdwFileSizeHigh )
{
    *lpdwFileSizeHigh = 0;
	SetLastError(ERROR_INVALID_FUNCTION);
    return 0xffffffff;
}

// *****************************************************************
// 声明：DWORD WINAPI Device_SetFilePointer(
//						 HANDLE hOpenFile, 
//						 LONG lDistanceToMove, 
//						 PLONG lpDistanceToMoveHigh,
//						 DWORD dwMoveMethod ) 
//
// 参数：
//		IN hOpenFile - 打开文件句柄
//		IN lDistanceToMove - 移动距离的低32位数据
//		IN lpDistanceToMoveHigh - 假如有的话，移动距离的高32位数据
//		IN dwMoveMethod  - 移动方法
// 返回值：
//		假如成功，返回当前的值；否则，返回0xffffffff
// 功能描述：
//		设置文件当前位置
// 引用: 
//		系统API
// *****************************************************************

DWORD WINAPI Device_SetFilePointer(
						 HANDLE hOpenFile, 
						 LONG lDistanceToMove, 
						 PLONG lpDistanceToMoveHigh,
						 DWORD dwMoveMethod ) 
{
    LPDEVICE_OPEN_FILE lpOpenFile = _GetHDEVFilePtr( hOpenFile );

    DWORD dwRetv = 0xffffffff;
    DWORD dodec = 0;

	if( lpOpenFile )
	{	// 简单调用设备驱动程序的定位函数
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			dwRetv = lpOpenFile->lpDev->lpDriver->lpSeek( lpOpenFile->hDevOpen, lDistanceToMove, dwMoveMethod );
			Sys_ReleaseException();
		}
	}
    return dwRetv;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_SetEndOfFile( HANDLE hOpenFile )
// 参数：
//		IN hOpenFile - 打开文件句柄
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		设置文件结束位置
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_SetEndOfFile( HANDLE hOpenFile )
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_GetFileInformationByHandle(
//									HANDLE hOpenFile,
//									LPBY_HANDLE_FILE_INFORMATION lpFileInfo )

// 参数：
//		IN hOpenFile - 打开文件句柄
//		OUT lpFileInfo - 文件信息结构，用于接受文件信息
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		得到文件信息
// 引用: 
//		系统API
// *****************************************************************

BOOL WINAPI Device_GetFileInformationByHandle(
									HANDLE hOpenFile,
									LPBY_HANDLE_FILE_INFORMATION lpFileInfo )
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_IoControl(
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
//		设备I/O控制函数入口，通过发不同的控制代码已实现对设备的管理控制
// 引用:
//		系统API 
// *****************************************************************

BOOL WINAPI Device_IoControl(
				   HANDLE hOpenFile,
				   DWORD  dwIoControlCode,
				   LPVOID lpvInBuf,
				   DWORD dwInBufSize,
				   LPVOID lpvOutBuf,
				   DWORD dwOutBufSize,
				   LPDWORD lpdwBytesReturned,
				   LPOVERLAPPED lpOverlapped )
{
    LPDEVICE_OPEN_FILE lpOpenFile = _GetHDEVFilePtr( hOpenFile );

    BOOL bRetv = FALSE;

	if( lpOpenFile )
	{	//简单调用设备驱动程序的相关函数
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			bRetv = lpOpenFile->lpDev->lpDriver->lpIOControl( 
													lpOpenFile->hDevOpen, 
													dwIoControlCode,
													lpvInBuf,
													dwInBufSize,
													lpvOutBuf,
													dwOutBufSize,
													lpdwBytesReturned );
			Sys_ReleaseException();
		}
	}
	return bRetv;
}


// *****************************************************************
// 声明：static LPOBJLIST FindDevice( LPCTSTR lpszName, UINT index )
// 参数：
//		IN lpszName - 设备名，例如："COM5"
//		IN index - 设备索引号， 从0 ~ 9
// 返回值：
//		假如发现，返回该设备的对象指针；否则，返回NULL
// 功能描述：
//		有设备名查找设备对象指针
// 引用: 
// *****************************************************************

static LPOBJLIST FindDevice( LPCTSTR lpszName, UINT index )
{
	LPOBJLIST lpRetv = NULL;
	LPOBJLIST lpObj;
//	int index;
	// 检查参数
 //	if( strlen( lpszName ) < 4 )
//		goto DEV_RET;
//	if( *(lpszName + 3) < '0' || *(lpszName + 3) > '9' )
//		goto DEV_RET;//return FALSE;
//	index = *(lpszName + 3) - '0';

    EnterCriticalSection( &csDeviceObjList ); // 进入冲突段	
	lpObj = (LPOBJLIST)lpDeviceObjList;
	//搜索对象链表
	while( lpObj )
	{
		if( strnicmp( ( (DEVICE_DATA *)lpObj)->lpszName, lpszName, 3 ) == 0 && 
			( (DEVICE_DATA *)lpObj )->index == index )
		{   // 找到。found it
			lpRetv = lpObj;
			break;
		}
		lpObj = lpObj->lpNext; // 下一个对象
	}

//DEV_RET:
	LeaveCriticalSection( &csDeviceObjList ); //离开冲突段

	return lpRetv;
}

// *****************************************************************
// 声明：BOOL _InitDeviceMgr( void )
// 参数：
//		无
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		初始化设备管理器
// 引用: 
//		当设备程序运行时，会调用该函数去初始化设备管理器。devsrc.c 
// *****************************************************************

BOOL _InitDeviceMgr( void )
{
	InitializeCriticalSection( &csDeviceObjList );
#ifdef __DEBUG
	csDeviceObjList.lpcsName = "CS-DEV";
#endif
    return TRUE;
}

// *****************************************************************
// 声明：BOOL _DeinitDeviceMgr( void )
// 参数：
//		无
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		释放设备管理器
// 引用: 
//		当设备程序退出时，会调用该函数去释放设备管理器资源。devsrc.c 
// *****************************************************************

BOOL _DeinitDeviceMgr( void )
{
	DeleteCriticalSection( &csDeviceObjList );
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// file driver mgr

// *****************************************************************
// 声明：static LPDEVFSD GetFSD( LPCTSTR lpcszFSDName )
// 参数：
//		IN lpcszFSDName - 文件系统名
// 返回值：
//		假如成功，返回当前系统存在的文件系统对象指针。否则返回NULL
// 功能描述：
//		得到已加载的文件系统对象
// 引用: 
// *****************************************************************

//	文件系统对象链表
static LPDEVFSD lpFsdList = NULL;

static LPDEVFSD GetFSD( LPCTSTR lpcszFSDName )
{
    LPDEVFSD pfsd = lpFsdList;
    while( pfsd )
    {
        if( stricmp( pfsd->lpszFSDName, lpcszFSDName ) == 0 )
            return pfsd; //找到。found
        pfsd = pfsd->lpNext;
    }
    return NULL;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_RegisterFSD ( LPCTSTR lpcszFSDName, HANDLE hFSDRegister, const FSDINITDRV * lpfsd )
// 参数：
//		IN lpcszFSDName - 文件系统名 
//		IN hFSDRegister - 传递给文件系统初始化接口的参数
//		IN lpfsd - 文件系统初始化接口
// 返回值：
//		假如成功，返回TRUE。否则返回FALSE
// 功能描述：
//		注册文件系统
// 引用: 
// *****************************************************************
#define DEBUG_DEVICE_REGISTERFSD 0
BOOL WINAPI Device_RegisterFSD ( LPCTSTR lpcszFSDName, HANDLE hFSDRegister, const FSDINITDRV * lpfsd )
{	
    LPDEVFSD p;

	// 分配文件系统结构对象
    DEBUGMSG( DEBUG_DEVICE_REGISTERFSD, ( "Device_RegisterFSD: RegisterFSD(=%s).\r\n", lpcszFSDName ) );
	p = (LPDEVFSD)malloc( sizeof( DEVFSD ) + sizeof( FSDINITDRV ) + strlen( lpcszFSDName ) + 1 );
    if( p )
    {
	    EnterCriticalSection( &csDeviceObjList ); // 进入冲突段	
        DEBUGMSG( DEBUG_DEVICE_REGISTERFSD, ( "Device_RegisterFSD: GetFSD.\r\n" ) );
		if( GetFSD( lpcszFSDName ) == NULL )
		{	// 没有发现
			p->lpFSDInitDrv = (LPFSDINITDRV)(p + 1);
			p->lpszFSDName = (LPTSTR)(p->lpFSDInitDrv + 1);
			
			strcpy( p->lpszFSDName, lpcszFSDName );
			memcpy( p->lpFSDInitDrv, lpfsd, sizeof( FSDINITDRV ) );
			p->hFSDRegister = hFSDRegister;
			//p->hFsdInit = hFSD;
			p->lpNext = lpFsdList;
			p->iRefCount = 1;
			lpFsdList = p;

			LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段
		    DEBUGMSG( DEBUG_DEVICE_REGISTERFSD, ( "Device_RegisterFSD: RegisterFSD(=%s) success.\r\n", lpcszFSDName ) );
		}
		else
		{   // 已经存在该文件系统，退出
		    LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段
			free( p );
			p = NULL;
		}
    }
	else
	{
		WARNMSG( DEBUG_DEVICE_REGISTERFSD, ( "Device_RegisterFSD:no enough mem.\r\n" ) );
	}
    return p != NULL;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_UnregisterFSD ( LPCTSTR lpcszFSDName )
// 参数：
//		IN lpcszFSDName - 文件系统名 
// 返回值：
//		假如成功，返回TRUE。否则返回FALSE
// 功能描述：
//		注销文件系统
// 引用: 
// *****************************************************************

BOOL WINAPI Device_UnregisterFSD ( LPCTSTR lpcszFSDName )
{
    LPDEVFSD pfsd, prev;

    EnterCriticalSection( &csDeviceObjList ); // 进入冲突段	
   
	prev = pfsd = lpFsdList;
    
    while( pfsd )
    {
        if( stricmp( pfsd->lpszFSDName, lpcszFSDName ) == 0 )
        {	//发现它
			if( --pfsd->iRefCount == 0 )
			{	//假如引用数为0，则释放该对象
				if( prev == lpFsdList )
					lpFsdList = lpFsdList->lpNext;
				else
					prev->lpNext = pfsd->lpNext;
				free( pfsd );
			}
		    LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段	

            return TRUE;
        }
        prev = pfsd;
        pfsd = pfsd->lpNext;
    }

    LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段	
    return FALSE;
}

// *****************************************************************
// 声明：BOOL WINAPI Device_LoadFSD ( HANDLE hDevice, LPCTSTR lpcszFSDName )
// 参数：
//		IN hDevice - 设备句柄(调用RegisterDevice / RegisterDriver返回的句柄)
//		IN lpcszFSDName - 文件系统名
// 返回值：
//		假如成功，返回TRUE。否则返回FALSE
// 功能描述：
//		将一个设备与一个文件系统关联
// 引用: 
//		系统API
// *****************************************************************

#define DEBUG_DEVICE_LOADFSD 0
BOOL WINAPI Device_LoadFSD ( HANDLE hDevice, LPCTSTR lpcszFSDName )
{    
	LPDEVFSD lpfsd;
	DEVICE_DATA * lpdd;

    EnterCriticalSection( &csDeviceObjList ); // 进入冲突段	
	lpfsd = GetFSD( lpcszFSDName );
	if( lpfsd ) // 假如有该文件系统，增加其引用数
	    lpfsd->iRefCount++;
    LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段	

	lpdd = _GetHDEVPtr( hDevice );
		
	if( lpfsd && lpdd )
	{   // 调用文件系统的连接功能
		lpdd->hFSDAttach = lpfsd->lpFSDInitDrv->lpAttachDevice( lpfsd->hFSDRegister, lpdd->lpszName );
		DEBUGMSG( DEBUG_DEVICE_LOADFSD, ( "Device_LoadFSD:Attach Handle=%x.\r\n", lpdd->hFSDAttach ) );
		if( lpdd->hFSDAttach )
		{	//连接成功
			lpdd->lpfsd = lpfsd;
			return TRUE;
		}
	}
	// 失败，减少其引用数
	if( lpfsd )
	    lpfsd->iRefCount--;

    return FALSE;
}

// *****************************************************************
// 声明：static BOOL _Device_LoadFSD ( DEVICE_DATA * lpdd )
// 参数：
//		IN lpdd - 设备对象指针
//		IN lpcszFSDName - 文件系统名
// 返回值：
//		假如成功，返回TRUE。否则返回FALSE
// 功能描述：
//		将一个设备与一个文件系统去掉关联
// 引用: 
//		
// *****************************************************************

#define DEBUG_DoUnloadFSD 1
static BOOL DoUnloadFSD ( DEVICE_DATA * lpdd )
{    
	BOOL bRetv = FALSE;

	DEBUGMSG( DEBUG_DoUnloadFSD, ( "DoUnloadFSD entry.\r\n." ) );
	if( lpdd  )
	{
		// 该对象是否与某文件系统关联 ？
		if( lpdd->hFSDAttach && lpdd->lpfsd )
		{	//是，通知该文件系统去注销它
			//RETAILMSG( 1, ( "Detach device.\r\n." ) );
			lpdd->lpfsd->lpFSDInitDrv->lpDetachDevice( lpdd->hFSDAttach );
			--lpdd->lpfsd->iRefCount;// 减少对该文件系统的引用
			lpdd->hFSDAttach = lpdd->lpfsd = 0;
			bRetv = TRUE;
		}
		else
		{
			WARNMSG( DEBUG_DoUnloadFSD, ( "DoUnloadFSD the device not connect to a fsd.\r\n." ) );
		}
		

		/*

        EnterCriticalSection( &csDeviceObjList ); // 进入冲突段


		lpDevOpen = (LPOBJLIST)lpDeviceOpenObjList; //全局设备打开/引用对象链表
		// 查找已打开该设备的对象，如果有则注销它
		for( ; lpDevOpen; lpDevOpen = lpDevOpen->lpNext )
		{
			if( ( (LPDEVICE_OPEN_FILE)lpDevOpen )->lpDev == lpdd )  // 
			{   // 找到该对象
				( (LPDEVICE_OPEN_FILE)lpDevOpen )->lpDev = NULL; // 不许再用
				Interlock_Decrement( (LPLONG)&lpdd->obj.iRefCount ); // 减少引用
			}
		}

		LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段
		*/
	}
	DEBUGMSG( DEBUG_DoUnloadFSD, ( "DoUnloadFSD leave(ret=%d).\r\n", bRetv ) );
	return bRetv;
}


// *****************************************************************
// 声明：BOOL WINAPI Device_LoadFSD ( HANDLE hDevice, LPCTSTR lpcszFSDName )
// 参数：
//		IN hDevice - 设备句柄(调用RegisterDevice / RegisterDriver返回的句柄)
//		IN lpcszFSDName - 文件系统名
// 返回值：
//		假如成功，返回TRUE。否则返回FALSE
// 功能描述：
//		将一个设备与一个文件系统关联
// 引用: 
//		系统API
// *****************************************************************

#define DEBUG_DEVICE_UNLOADFSD 0
BOOL WINAPI Device_UnloadFSD ( HANDLE hDevice )
{    
    DEVICE_DATA * lpdd = _GetHDEVPtr( hDevice ); // 由句柄得到设备数据结构指针
	if( lpdd )
		return DoUnloadFSD( lpdd );
	return FALSE;
}

// *****************************************************************
// 声明：void DeregisterAllDevice( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		注销所有的设备
// 引用: 
// *****************************************************************
//
void DeregisterAllDevice( void )
{
	LPDEVICE_DATA lpdev;

	EnterCriticalSection( &csDeviceObjList ); // 进入冲突段	
	lpdev = (LPDEVICE_DATA)lpDeviceObjList;
	while( lpdev )
	{
		DoUnloadFSD( lpdev );
		Device_Deregister( (HANDLE)lpdev );
		lpdev = (LPDEVICE_DATA)lpDeviceObjList;
	}
	LeaveCriticalSection( &csDeviceObjList ); // 离开冲突段	
}
//

// *****************************************************************
// 声明：void _InitDllDevice( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		从注册表加载驱动程序
// 引用: 
// *****************************************************************

//在注册表里加入相关的 文件连接
//[HKEY_HARDWARE_ROOT\Drivers\BuiltIn\Serial]
//"Prefix"="XXX" //函数前缀名,参看“驱动程序接口定义”
//"Dll"="serial.so" //驱动程序文件名
//"Index"=dword:1 //当注册驱动程序时的索引号
//参看“RegisterDriver”和”RegisterDevice”

#define DEBUG_InitDllDevice 0
void _InitDllDevice( void )
{
	HKEY hSubKey;
	HKEY hDriverKey;


    if( RegOpenKeyEx( HKEY_HARDWARE_ROOT, szBuiltInKey, 0, KEY_ALL_ACCESS, &hSubKey ) == ERROR_SUCCESS )
	{
		//TCHAR filePath[MAX_PATH];
		int n = 0;

		while( 1 )
		{
//			DWORD dwType;
			TCHAR tcValueName[32];  //
			DWORD dwValueNameSize = sizeof( tcValueName );
			TCHAR tcPrefix[8];
			DWORD dwPrefixSize = sizeof( tcPrefix );
			TCHAR tcDll[MAX_PATH];
			DWORD dwDllSize = sizeof( tcDll );
			DWORD dwIndex;
			DWORD dwIndexSize = sizeof(dwIndex);
			DEBUGMSG( DEBUG_InitDllDevice, ( "_InitDllDevice: call RegEnumKeyEx(%d).\r\n", n ) );			
			if( ERROR_SUCCESS != RegEnumKeyEx( 
										hSubKey, 
										n++, 
										tcValueName, 
										&dwValueNameSize, 
										0, 
										NULL, 
										NULL, 
										NULL ) )
				break;
			if( ERROR_SUCCESS != RegOpenKeyEx( 
										hSubKey,
										tcValueName,
										0,
										KEY_ALL_ACCESS,
										&hDriverKey ) )
				continue;
			if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, szPrefix, NULL, NULL, tcPrefix, &dwPrefixSize ) )
			{
				goto _CLOSE_HKEY;
			}
			if( dwPrefixSize != 4 )
			{
				goto _CLOSE_HKEY;
			}
			if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, szDll, NULL, NULL, tcDll, &dwDllSize ) )
			{
				goto _CLOSE_HKEY;
			}
			if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, szIndex, NULL, NULL, (LPBYTE)&dwIndex, &dwIndexSize ) )
			{
				goto _CLOSE_HKEY;
			}
			if( dwIndex > 9 )
			{
				goto _CLOSE_HKEY;
			}
			Device_RegisterDevice( tcPrefix, dwIndex, tcDll, 0 );
_CLOSE_HKEY:
			RegCloseKey( hDriverKey );
		}
		RegCloseKey( hSubKey );
	}
}

// *****************************************************************
// 声明：static BOOL CALLBACK DoPowerHandler( LPDEVICE_DATA lpDev, LPVOID lpParam )
// 参数：
//		IN lpDev - 设备驱动程序接口对象
//		IN lpParam - 假如非0，做开机动作；假如0，做关机动作
// 返回值：
//		TRUE
// 功能描述：
//		枚举所有的设备对象，通知其开机或关机
// 引用: 
//
// *****************************************************************

static BOOL CALLBACK DoPowerHandler( LPDEVICE_DATA lpDev, LPVOID lpParam )
{
	if( lpParam )
	{  //on
	    if( lpDev->lpDriver->lpPowerUp )
			lpDev->lpDriver->lpPowerUp(lpDev->hDevInit);
	}
	else
	{
	    if( lpDev->lpDriver->lpPowerDown )
			lpDev->lpDriver->lpPowerDown(lpDev->hDevInit);
	}
	return TRUE;
}

// **************************************************
// 声明：DWORD WINAPI Device_ServerHandler( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
// 参数：
//		IN hServer - 服务句柄
//		IN dwServerCode - 控制码
//		IN dwParam - 第一个参数
//		IN lpParam - 第二个参数
// 返回值：
//		依赖与不同的控制
// 功能描述：
//		服务总线控制，处理各种系统服务，通知功能
// 引用: 
//		系统API	
// ************************************************

DWORD WINAPI Device_ServerHandler( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
{
	extern void ShutDownDevice( void );
	switch( dwServerCode )
	{
	case SCC_BROADCAST_POWER:
		if( dwParam == SBP_APMSUSPEND )
		{
			EnumDevice( DoPowerHandler, 0 );
			SaveAsRegister(NULL);  //保存所有的注册表信息
		}
		else if( dwParam == SBP_APMRESUMESUSPEND )
		{
			EnumDevice( DoPowerHandler, (LPVOID)1 );
		}
		return TRUE;
	//case SCC_BROADCAST_POWERON:
		//EnumDevice( DoPowerHandler, (LPVOID)1 );
		//break;
	case SCC_BROADCAST_SHUTDOWN:
		ShutDownDevice();
		break;
	default:
		return Sys_DefServerProc( hServer, dwServerCode , dwParam, lpParam );
	}
	return 0;
}
/*
#define ACTIVE_KEY_PATH_LEN ( sizeof(szActiveKey) + 8 )
static BOOL ActiveAndLoadDriver( 
								LPCTSTR lpszDevKey,
								const REGINI * lpRegEnts, 
								DWORD cRegEnts, 
								LPVOID lpvParam,
								DWORD dwIndex,
								)
{
	UINT uiActiveNum;
    TCHAR szActiveKeyPath[ACTIVE_KEY_PATH_LEN];
	HKEY hActiveKey = NULL;
	DWORD dwDisposition;
	int n = 1;

    // 创建新的Active键
	while( n < 100 )
	{
		uiActiveNum = InterlockedIncrement( &uiCurrentActiveNum ) - 1;
		sprintf( szActiveKeyPath, "%s\\%02d", szActiveKey, uiActiveNum );
		
		// 创建新键
		if( ERROR_SUCCESS != RegCreateKeyEx(
			HKEY_HARDWARE_ROOT,
			szActiveKeyPath,
			0,
			NULL,
			0,
			0,
			NULL,
			&hActiveKey,     // HKEY result
			&dwDisposition) )
		{
			goto _error_return;
		}
		if( dwDisposition != REG_CREATED_NEW_KEY )
		{
			RegCloseKey( hActiveKey );
		}
		else
			break;
	}
	if( n == 100 )
		goto _error_return;
	// 写用户键值到 Active Keu
	if( lpRegEnts && cReg )
	{
		HANDLE hCallerProcess = GetCallerProcess();
        for( n = 0; n < cReg; n++ )
		{
            if( EEROR_SUCESS != RegSetValueEx(
									hActiveKey,
									MapPtrToProcess( lpReg[cReg].lpszVal, hCallerProcess ),
									0,
									lpReg[cReg].dwType,
									MapPtrToProcess( lpReg[cReg].pData, lpReg[cReg].dwLen ),
									lpReg[cReg].dwLen ) )
			{
				break;
			}
		}
		if( n != cReg )
			goto _error_return;
    }

	// 写驱动程序键值到 Active Keu的Load Key
    if( lpszDevKey != NULL )
	{    
        if( ERROR_SUCCESS != RegSetValueEx(
								hActiveKey,
								szPath,
								0,
								REG_SZ,
								(LPBYTE)lpszDevKey,
								strlen( lpszDevKey ) + 1 ) )
		{
			goto _error_return;
		}
    }

    if( dwIndex == -1 )
	{	// 由系统分配索引值
        dwIndex = 0;
		EnterCriticalSection( &csDeviceObjList ); // 进入冲突段	

        while( dwIndex < 10 )
		{
			if( FindDevice( lpszPrefix, dwIndex ) == NULL )
				break;
			dwIndex++;
		}
		LeaveCriticalSection( &csDeviceObjList );
	}
	if( dwIndex < 10 )
	{
         char szDevName[16];
		 int len;
		 
		 strcpy( szDevName, lpszPrefix );
		 len = strlen( szDevName );
		 szDevName[len]=dwIndex+'0';
		 szDevName[len+1]=':';
		 szDevName[len+2]=0;
         if( ERROR_SUCCESS == RegSetValueEx( 
			                       hActiveKey,
                                   szName,
                                   0,
                                   REG_SZ,
                                   (LPBYTE)szDevName,
                                   strlen( szDevName ) + 1 ) )
		 {
			 HANDLE hDev;
			 if( ( hDev = Device_Register( lpszPrefix, dwIndex, lpszDll, lpvParam ) ) != NULL )
			 {	// 设备注册成功
				 if( ERROR_SUCCESS != RegSetValueEx( 
											hActiveKey,
											szHandle,
											0,
											REG_DWORD,
											(LPBYTE)hDev,
											sizeof(hDev) ) )
				 {
					 ERRORMSG( 1, ( "Can't write registry!.\r\n" ) );
				 }
				 if( *lpszPrefix )
				     SendDeviceInitMsg( lpszPrefix, dwIndex, hDev );

				 PNP_NotifyHandler( szActiveKeyPath );
				 return TRUE;
			 }
		 }
	}
_error_return:
	if( hActiveKey )
	{
		RegCloseKey( hActiveKey );
		RegDeleteKey( HKEY_HARDWARE_ROOT, szActiveKeyPath );		
	}
	return FALSE;
}

// **************************************************
// 声明：

// 参数：
//		IN 
// 返回值：
//		假如成功，返回；否则，返回
// 功能描述：
// 引用: 
//		
// ************************************************

#define DEBUG_READ_DRIVER_REG 0
BOOL ReadDriverKeyFromRegistry( HKEY hDriverKey, 
							    DWORD * lpdwFlags,
								char * lpszDllBuf,
								UINT uiDllLen,
								char * lpszPrefixBuf,
								UINT uiPrefixLen，
								DWORD * lpdwIndex )
{
	DWORD dwValLen;	
    // 
    // 读 flags
    dwValLen = sizeof(*lpdwFlags);
    if( ERROR_SUCCESS != RegQueryValueEx(
        hDriverKey,
        szFlags,
        NULL,
        NULL,
        (LPBYTE)lpdwFlags,
        &dwValLen ) )
	{
        WARNMSG( DEBUG_READ_DRIVER_REG, ( "ReadDriverKeyFromRegistry: read flag error.\r\n") );
		goto _error_return;
	}

    // 读动态连接库名 read so name
    dwValLen = uiDllLen;
    if( ERROR_SUCCESS != RegQueryValueEx(
        hDriverKey,
        szDll,
        NULL,
        NULL,
        lpszDllBuf,
        &dwValLen ) )
	{
		WARNMSG( DEBUG_READ_DRIVER_REG, ( "ReadDriverKeyFromRegistry: read dll error.\r\n") );
		goto _error_return;
	}

    // 读前缀
    dwValLen = uiPrefixLen;
    if( ERROR_SUCCESS != RegQueryValueEx(
        hDriverKey,
        szPrefix,
        NULL,
        NULL,
        lpszPrefixBuf,
        &dwValLen ) )
	{
		WARNMSG( DEBUG_READ_DRIVER_REG, ( "ReadDriverKeyFromRegistry: read prefix error.\r\n") );
		goto _error_return;
	}

    // 读索引

    dwValLen = sizeof(*lpdwIndex);
    if( ERROR_SUCCESS != RegQueryValueEx(
        hDriverKey,
        szIndex,
        NULL,
        NULL,
        (LPBYTE)lpdwIndex,
        &dwValLen ) )
	{
		*lpdwIndex = -1;
	}
	return TRUE;
_error_return:
	return FALSE;
}


typedef struct _REGINI {
	LPCWSTR lpszVal;
	LPBYTE pData;
	DWORD dwLen;
	DWORD dwType;
} REGINI;

// **************************************************
// 声明：HANDLE WINAPI Device_ActivateEx(
//							LPCWSTR lpszDevKey, 
//							const REGINI * lpRegEnts, 
//							DWORD cRegEnts, 
//							LPVOID lpvParam
//							)

// 参数：
//		IN lpszDevKey - 需要注册的设备的注册键
//		IN lpRegEnts - REGINI结构数组指针，指向需要增加的键值
//		IN cRegEnts - 指明EGINI结构数组的个数
//		IN lpvParam - 传递给 XXX_Init( xxx, lpvParam ) 的参数
// 返回值：
//		假如成功，返回非NULL句柄；否则，返回NULL
// 功能描述：
//		将用户指定的键值写入注册表的[HKEY_HARDWARE_ROOT\Drivers\Active\(NO.xxx)]
//		通过lpszDevKey的设备名，前缀，索引号等加载设备
//		传递 lpvParam 给 XXX_init( xxx, lpvParam )
// 引用: 
//		系统API	
// ************************************************
//static TCHAR const szBuiltInDriver[] = "Drivers\\Active";

#define DEBUG_ACTIVATEEX 0
HANDLE WINAPI Device_ActivateEx(
							LPCWSTR lpszDevKey, 
							const REGINI * lpRegEnts, 
							DWORD cRegEnts, 
							LPVOID lpvParam
							)
{
	TCHAR tcActiveNum[sizeof(szActiveKey)+2];
	HKEY hDriverKey;

	// 打开/检查是否存在 lpszDevKey

    if( EEROR_SUCCESS != RegOpenKeyEx(
        HKEY_HARDWARE_ROOT,
        lpszDevKey,
        0,
        KEY_ALL_ACCESS,
        &hDriverKey ) )
	{
        WARNMSG( DEBUG_ACTIVATEEX, (TEXT("registry key lpszDevKey=(%s) is not exist!.\r\n"), lpszDevKey ) );
		return NULL;
	}

    if( DoWriteInitReg( lpRegEnts, cRegEnts, tcActiveNum+sizeof(szActiveKey) ) )
	{
		tcActiveNum[sizeof(tcActiveNum)-1] = 0;
		if( ActiveAndLoadDriver( lpszDevKey, lpvParam ) == FALSE )
		{
			RegDeleteKey( HKEY_HARDWARE_ROOT, tcActiveNum );
		}
	}
}
	HKEY hDriverKey;
	if( ERROR_SUCCESS != RegOpenKeyEx( 
							HKEY_HARDWARE_ROOT,
							lpszDevKey,
							0,
							KEY_ALL_ACCESS,
							&hDriverKey ) )
		continue;
	if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, szPrefix, NULL, NULL, tcPrefix, &dwPrefixSize ) )
	{
		goto _CLOSE_HKEY;
	}
	if( dwPrefixSize != 4 )
	{
		goto _CLOSE_HKEY;
	}
	if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, "Dll", NULL, NULL, tcDll, &dwDllSize ) )
	{
		goto _CLOSE_HKEY;
	}
	if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, "Index", NULL, NULL, (LPBYTE)&dwIndex, &dwIndexSize ) )
	{
		goto _CLOSE_HKEY;
	}
	if( dwIndex > 9 )
	{
		goto _CLOSE_HKEY;
	}
	Device_RegisterDevice( tcPrefix, dwIndex, tcDll, 0 );

}


// **************************************************
// 声明：BOOL WINAPI Device_Deactivate(
//							HANDLE hActiveDevice
//							)

// 参数：
//		IN hActiveDevice - 由Device_ActivateEx返回的句柄
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		注销设备驱动程序
//		删除注册表的[HKEY_HARDWARE_ROOT\Drivers\Active\(NO.xxx)]
//		广播WM_DEVICECHANGE
// 引用: 
//		系统API	
// ************************************************

BOOL WINAPI Device_Deactivate(
								HANDLE hActiveDevice
							)
{
}
*/

extern BOOL OEM_GetSystemPowerStatusEx(PSYSTEM_POWER_STATUS_EX pstatus, BOOL fUpdate);
BOOL WINAPI Device_GetSystemPowerStatusEx(
				PSYSTEM_POWER_STATUS_EX pstatus, 
				BOOL fUpdate )
{
#ifndef EML_WIN32
	if( pstatus )
		return OEM_GetSystemPowerStatusEx( pstatus, fUpdate );
#endif
	return FALSE;
}
