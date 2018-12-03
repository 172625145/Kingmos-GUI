/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：句柄管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
	2004-10-22: 谢修改 KL_AllocAPIHandle
    2003-05-22: 增加对 CURRENT_PROCESS_HANDLE 和　CURRENT_THREAD_HANDLE 的处理
******************************************************/



#include <eframe.h>
#include <eobjlist.h>
#include <epalloc.h>
#include <epcore.h>
#include <coresrv.h>

extern BOOL WINAPI KL_CloseFile( HANDLE hFile );
extern HANDLE WINAPI KL_CreateFile( LPCTSTR lpcszFileName,
							   DWORD dwAccess, 
							   DWORD dwShareMode, 
							   PSECURITY_ATTRIBUTES pSecurityAttributes, 
							   DWORD dwCreate, 
							   DWORD dwFlagsAndAttributes, 
							   HANDLE hTemplateFile );
extern DWORD WINAPI KL_GetFileAttributes( LPCTSTR lpcszFileName );


extern BOOL FASTCALL Semaphore_Close( HANDLE handle );
extern BOOL FASTCALL Thread_Close( HANDLE handle );
extern BOOL FASTCALL Event_Close( HANDLE handle );
extern BOOL FASTCALL Mutex_Close( HANDLE hMutex );
extern BOOL FASTCALL Process_Close( HANDLE hProcess );

extern VOID DumpCallStack( LPTHREAD lpThread );

static LPOBJLIST __lpHandleObjList = NULL;
static CRITICAL_SECTION csHandleObjList;

/**************************************************
声明：LPHANDLEDATA _GetHANDLEPtr( HANDLE h )
参数：
	IN h - 句柄对象
返回值：
	句柄对象指针
功能描述：
	由句柄得到句柄对象指针
引用: 
	
************************************************/
#define DEBUG__GetHANDLEPtr 0
LPHANDLEDATA _GetHANDLEPtr( HANDLE h )
{
	extern LPBYTE  lpbSysMainMem;
	extern LPBYTE  lpbSysMainMemEnd;
	extern DWORD   dwHandleBase; 
	if( (LPHANDLEDATA)h == KERNEL_PROCESS_HANDLE || 
		(LPHANDLEDATA)h == KERNEL_THREAD_HANDLE )
		return (LPHANDLEDATA)h;
	else
	{
		LPHANDLEDATA lphd = GET_PTR( h );
		//RETAILMSG( 1, ( "_GetHANDLEPtr:lphd=%x,h=%x.\r\n",lphd, h ) );
		// lilin -test code
//		if( (DWORD)h == -1 )
//		{
//			RETAILMSG( 1, ( "_GetHANDLEPtr:lphd=%x,h=%x,lpbSysMainMem=%x,lpbSysMainMemEnd=%x.\r\n",lphd, h, lpbSysMainMem,lpbSysMainMemEnd ) );
//		}
		//
        if( (LPBYTE)lphd >= lpbSysMainMem && 
		    (LPBYTE)lphd < lpbSysMainMemEnd &&
		    ((LPHANDLEDATA)lphd)->hThis == h )
		{
			return lphd;
		}		
	}
    //ASSERT( 0 );
	WARNMSG( DEBUG__GetHANDLEPtr, ( "_GetHANDLEPtr error: Invalid handle=0x%x.\r\n", h ) );
	DumpCallStack( lpCurThread );
	return NULL;
}

/**************************************************
声明：BOOL _InitHandleMgr( void )
参数：
	无
返回值：
	假如成功，返回TRUE；否则，返回FALSE
功能描述：
	初始化句柄管理器
引用: 
	system.c
************************************************/

BOOL _InitHandleMgr( void )
{
	extern VOID FASTCALL KC_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection );
	KC_InitializeCriticalSection( &csHandleObjList );
	csHandleObjList.lpcsName = "CS-HOM";
	return TRUE;
}

/**************************************************
声明：HANDLE Handle_Alloc( LPPROCESS lpOwnerProcess, LPVOID lpvObjData, UINT uiObjType )
参数：
	IN lpOwnerProcess - 句柄拥有者进程
	IN lpvObjData - 对象数据
	IN uiObjType - 对象类型
返回值：
	假如成功，返回有效的句柄；否则，返回NULL
功能描述：
	为一个数据对象分配一个句柄
引用: 
	
************************************************/

HANDLE Handle_Alloc( LPPROCESS lpOwnerProcess, LPVOID lpvObjData, UINT uiObjType )
{
	LPHANDLEDATA lphObj = KHeap_Alloc( sizeof( OBJLIST ) );
	if( lphObj )
	{		
		lphObj->lpvObjData = lpvObjData;
		lphObj->hThis = MAKE_HANDLE( lphObj );
		//RETAILMSG( 1, ( "lphObj:%x,h=%x.\r\n", lphObj,lphObj->hThis ) );

		KL_EnterCriticalSection( &csHandleObjList );

		ObjList_Init( &__lpHandleObjList, &lphObj->obj, uiObjType, (ULONG)lpOwnerProcess );

		KL_LeaveCriticalSection( &csHandleObjList );
	}
	return lphObj ? lphObj->hThis : NULL;
}

extern LPPROCESS GetHandleOwner( void );

/**************************************************
声明：HANDLE WINAPI KL_AllocAPIHandle( UINT uiAPIId, LPVOID lpvData, UINT objType )
参数：
	IN uiAPIId - 句柄拥有者进程
	IN lpvData - 对象数据
	IN objType - 对象类型
返回值：
	假如成功，返回有效的句柄；否则，返回NULL
功能描述：
	为一个数据对象分配一个句柄
引用: 
	系统API
	
************************************************/

HANDLE WINAPI KL_AllocAPIHandle( UINT uiAPIId, LPVOID lpvData, UINT objType )
{
	return Handle_Alloc( GetHandleOwner(), lpvData, objType );
}

/**************************************************
声明：void Handle_Free( HANDLE handle, BOOL bRemove )
参数：
	IN handle - 句柄
	IN bRemove - 是否从对象列式里移出
返回值：
	无
功能描述：
	释放句柄对象
引用: 
	
************************************************/

void Handle_Free( HANDLE handle, BOOL bRemove )
{
	LPHANDLEDATA lphObj = _GetHANDLEPtr( handle );
	if( lphObj )
	{
		if( bRemove )
		{
			KL_EnterCriticalSection( &csHandleObjList );
			
			ObjList_Remove( &__lpHandleObjList, &lphObj->obj );
			
			KL_LeaveCriticalSection( &csHandleObjList );
		}
		lphObj->obj.objType = OBJ_NULL;
		lphObj->hThis = NULL;
		KHeap_Free( lphObj, sizeof(OBJLIST) );// _kfree( lphObj );
	}
}

/**************************************************
声明：void * HandleToPtr( HANDLE handle, UINT uiObjType )
参数：
	IN handle - 句柄
	IN uiObjType - 对象类型，假如为-1，不检查类型是否合法
返回值：
	假如成功，返回对象数据；否则，返回NULL
功能描述：
	由句柄得到对象数据
引用: 
	
************************************************/

void * HandleToPtr( HANDLE handle, UINT uiObjType )
{
	LPHANDLEDATA lphObj = _GetHANDLEPtr( handle );
	if( lphObj )
	{
		if( uiObjType = -1 || lphObj->obj.objType == uiObjType )
			return lphObj->lpvObjData;
	}
	KL_SetLastError( ERROR_INVALID_HANDLE );
	RETAILMSG( 1, (  "HandleToPtr Error: Invalide handle=0x%x, objType=0x%x. \r\n" ,  handle, uiObjType ) );
	return NULL;
}

/**************************************************
声明：void * SetHandleObjPtr( HANDLE handle, LPVOID lpvObj )
参数：
	IN handle - 句柄
	IN lpvObj - 对象数据
返回值：
	假如成功，返回之前的对象数据；否则，返回NULL
功能描述：
	设置句柄的对象数据
引用: 
	
************************************************/

void * SetHandleObjPtr( HANDLE handle, LPVOID lpvObj )
{
	LPHANDLEDATA lphObj = _GetHANDLEPtr( handle );
	if( lphObj )
	{
		LPVOID lpv = lphObj->lpvObjData;
		lphObj->lpvObjData = lpvObj;
		return lpv;
	}
	return NULL;
}

/**************************************************
声明：UINT GetHandleObjType( HANDLE handle )
参数：
	IN handle - 句柄
返回值：
	假如成功，返回对象类型；否则，返回OBJ_NULL
功能描述：
	得到句柄的对象类型
引用: 
	
************************************************/
UINT GetHandleObjType( HANDLE handle )
{
	LPHANDLEDATA lphObj = _GetHANDLEPtr( handle );
	if( lphObj )
		return lphObj->obj.objType;
	return OBJ_NULL;
}

/**************************************************
声明：BOOL GetHandleUserInfo( HANDLE handle, LPDWORD lpdwUserInfo )
参数：
	IN handle - 句柄
	OUT lpdwUserInfo - 接受用户信息 
返回值：
	假如成功，返回TRUE & 用户定义信息，否则返回FALSE
功能描述：
	得到句柄的对象类型
引用: 
	
************************************************/
BOOL GetHandleUserInfo( HANDLE handle, LPDWORD lpdwUserInfo )
{
	LPHANDLEDATA lphObj = _GetHANDLEPtr( handle );
	if( lphObj )
	{
		*lpdwUserInfo = lphObj->dwUserInfo;
		return TRUE;
	}
	return FALSE;
}

/**************************************************
声明：DWORD SetHandleUserInfo( HANDLE handle, DWORD dwUserInfo )
参数：
	IN handle - 句柄
	IN dwUserInfo - 用户定义信息
返回值：
	假如成功，返回之前的用户定义信息，否则返回0
功能描述：
	设置句柄的用户定义信息
引用: 
	
************************************************/
DWORD SetHandleUserInfo( HANDLE handle, DWORD dwUserInfo )
{
	LPHANDLEDATA lphObj = _GetHANDLEPtr( handle );
	if( lphObj )
	{
		DWORD dwOld = lphObj->dwUserInfo;
		lphObj->dwUserInfo = dwUserInfo ;

		return dwOld;
	}
	return 0;
}


/**************************************************
声明：BOOL CALLBACK _DeleteHandleObject( LPOBJLIST lphObjList, LPARAM lParam )
参数：
	IN lphObjList - 对象列式
	IN lParam - 假如有错，是否打印错误消息
返回值：
	假如成功，返回TRUE；否则返回FALSE
功能描述：
	调用对象的释放函数并删除句柄
引用: 
	
************************************************/
#define DEBUG_DELETEHANDLEOBJECT 0
static BOOL CALLBACK _DeleteHandleObject( LPOBJLIST lphObjList, LPARAM lParam )
{
	BOOL bRetv = FALSE;
	LPHANDLEDATA lphObj = (LPHANDLEDATA)lphObjList;
	int objType;

	switch( (objType = lphObj->obj.objType) )
	{
	case OBJ_FILE:
		bRetv = KL_CloseFile( lphObj->hThis );
		break;
	case OBJ_EVENT:
		bRetv =  Event_Close( lphObj->hThis );
		break;
	case OBJ_SEMAPHORE:
		bRetv = Semaphore_Close( lphObj->hThis );
		break;
	case OBJ_MUTEX:
		bRetv = Mutex_Close( lphObj->hThis );
		break;
	case OBJ_THREAD:
		bRetv = Thread_Close( lphObj->hThis );
		break;
	case OBJ_PROCESS:
		bRetv = Process_Close( lphObj->hThis );
		break;
	default:
		WARNMSG( DEBUG_DELETEHANDLEOBJECT, ( "_DeleteHandleObject: Invalid Handle Obj=0x%x, type=0x%x.\r\n", lphObj, objType ) );
		return FALSE;
	}
//	WARNMSG( 1, ( "_DeleteHandleObject: lphObj=0x%x.\r\n", lphObj ) );

	Handle_Free( lphObj->hThis, FALSE );

	if( lParam && bRetv == FALSE )
	{  // print warn message
		switch( objType )
		{
		case OBJ_FILE:
			WARNMSG( DEBUG_DELETEHANDLEOBJECT, ( "not free file Handle: 0x%x\r\n", lphObj ) );
			break;
		case OBJ_EVENT:
			WARNMSG( DEBUG_DELETEHANDLEOBJECT, ( "not free event Handle: 0x%x\r\n", lphObj ) );
			break;
		case OBJ_SEMAPHORE:
			WARNMSG( DEBUG_DELETEHANDLEOBJECT, ( "not free semaphor Handle: 0x%x\r\n", lphObj ) );
			break;
		case OBJ_MUTEX:
			WARNMSG( DEBUG_DELETEHANDLEOBJECT, ( "not free mutex Handle: 0x%x\r\n", lphObj ) );
			break;
		case OBJ_THREAD:
			WARNMSG( DEBUG_DELETEHANDLEOBJECT, ( "not free thread Handle: 0x%x\r\n", lphObj ) );
			break;
	    case OBJ_PROCESS:
			WARNMSG( DEBUG_DELETEHANDLEOBJECT, ( "not free process Handle: 0x%x\r\n", lphObj ) );
			break;
		}
	}

	return bRetv;
}

/**************************************************
声明：BOOL WINAPI KL_CloseHandle( HANDLE handle )
参数：
	IN handle - 句柄
返回值：
	假如成功，返回TRUE；否则返回FALSE
功能描述：
	关闭句柄对象
引用: 
	系统调用	
************************************************/
#define DEBUG_CLOSEHANDLE 0
BOOL WINAPI KL_CloseHandle( HANDLE handle )
{
	LPHANDLEDATA lphObj;

	if( handle == CURRENT_THREAD_HANDLE ||
		handle == CURRENT_PROCESS_HANDLE )
		return TRUE;
	lphObj = _GetHANDLEPtr( handle );

	DEBUGMSG(DEBUG_CLOSEHANDLE,  ("KL_CloseHandle(0x%x).\r\n", handle ) );

	if( lphObj )
	{
		switch( lphObj->obj.objType )
		{
		case OBJ_FILE:
		case OBJ_THREAD:
		case OBJ_EVENT:
		case OBJ_SEMAPHORE:
		case OBJ_MUTEX:
		case OBJ_PROCESS:
			KL_EnterCriticalSection( &csHandleObjList );
			// 引用计数是否为0
			if( --lphObj->obj.iRefCount == 0 )
			{   // 是，关闭该对象
				ObjList_Remove( &__lpHandleObjList, &lphObj->obj );
				_DeleteHandleObject( &lphObj->obj, 0 );
			}
			KL_LeaveCriticalSection( &csHandleObjList );
			return TRUE;
		default:
			WARNMSG( DEBUG_CLOSEHANDLE, ( "CloseHandle: Invalid Handle Value.\r\n" ) );
			return FALSE;
		}
	}
	WARNMSG( DEBUG_CLOSEHANDLE, ( "CloseHandle: Invalid Handle Value.\r\n" ) );
    return FALSE;
}

/**************************************************
声明：Handle_CloseAll( LPPROCESS lpOwnerProcess )
参数：
	IN lpOwnerProcess - 句柄拥有者进程
返回值：
	假如成功，返回TRUE；否则返回FALSE
功能描述：
	关闭进程拥有的所有句柄对象
引用: 
	进程退出时调用
************************************************/
#define DEBUG_Handle_CloseAll 0
BOOL Handle_CloseAll( LPPROCESS lpOwnerProcess )
{
	extern BOOL KL_CloseSemaphore( HINSTANCE hOwner );

    KL_EnterCriticalSection( &csHandleObjList );

	DEBUGMSG( DEBUG_Handle_CloseAll, ( "Handle_CloseAll: lpOwnerProcess=0x%x.\r\n", lpOwnerProcess ) );
	ObjList_Delete( &__lpHandleObjList , (ULONG)lpOwnerProcess, _DeleteHandleObject, 1 );
	
	KL_LeaveCriticalSection( &csHandleObjList );
	
	return TRUE;
}

VOID Handle_AddRef( HANDLE handle )
{
	LPHANDLEDATA lphObj;

	lphObj = _GetHANDLEPtr( handle );
	if( lphObj )
		lphObj->obj.iRefCount++;
}
