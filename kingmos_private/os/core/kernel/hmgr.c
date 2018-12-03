/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����������
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2004-10-22: л�޸� KL_AllocAPIHandle
    2003-05-22: ���Ӷ� CURRENT_PROCESS_HANDLE �͡�CURRENT_THREAD_HANDLE �Ĵ���
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
������LPHANDLEDATA _GetHANDLEPtr( HANDLE h )
������
	IN h - �������
����ֵ��
	�������ָ��
����������
	�ɾ���õ��������ָ��
����: 
	
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
������BOOL _InitHandleMgr( void )
������
	��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE
����������
	��ʼ�����������
����: 
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
������HANDLE Handle_Alloc( LPPROCESS lpOwnerProcess, LPVOID lpvObjData, UINT uiObjType )
������
	IN lpOwnerProcess - ���ӵ���߽���
	IN lpvObjData - ��������
	IN uiObjType - ��������
����ֵ��
	����ɹ���������Ч�ľ�������򣬷���NULL
����������
	Ϊһ�����ݶ������һ�����
����: 
	
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
������HANDLE WINAPI KL_AllocAPIHandle( UINT uiAPIId, LPVOID lpvData, UINT objType )
������
	IN uiAPIId - ���ӵ���߽���
	IN lpvData - ��������
	IN objType - ��������
����ֵ��
	����ɹ���������Ч�ľ�������򣬷���NULL
����������
	Ϊһ�����ݶ������һ�����
����: 
	ϵͳAPI
	
************************************************/

HANDLE WINAPI KL_AllocAPIHandle( UINT uiAPIId, LPVOID lpvData, UINT objType )
{
	return Handle_Alloc( GetHandleOwner(), lpvData, objType );
}

/**************************************************
������void Handle_Free( HANDLE handle, BOOL bRemove )
������
	IN handle - ���
	IN bRemove - �Ƿ�Ӷ�����ʽ���Ƴ�
����ֵ��
	��
����������
	�ͷž������
����: 
	
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
������void * HandleToPtr( HANDLE handle, UINT uiObjType )
������
	IN handle - ���
	IN uiObjType - �������ͣ�����Ϊ-1������������Ƿ�Ϸ�
����ֵ��
	����ɹ������ض������ݣ����򣬷���NULL
����������
	�ɾ���õ���������
����: 
	
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
������void * SetHandleObjPtr( HANDLE handle, LPVOID lpvObj )
������
	IN handle - ���
	IN lpvObj - ��������
����ֵ��
	����ɹ�������֮ǰ�Ķ������ݣ����򣬷���NULL
����������
	���þ���Ķ�������
����: 
	
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
������UINT GetHandleObjType( HANDLE handle )
������
	IN handle - ���
����ֵ��
	����ɹ������ض������ͣ����򣬷���OBJ_NULL
����������
	�õ�����Ķ�������
����: 
	
************************************************/
UINT GetHandleObjType( HANDLE handle )
{
	LPHANDLEDATA lphObj = _GetHANDLEPtr( handle );
	if( lphObj )
		return lphObj->obj.objType;
	return OBJ_NULL;
}

/**************************************************
������BOOL GetHandleUserInfo( HANDLE handle, LPDWORD lpdwUserInfo )
������
	IN handle - ���
	OUT lpdwUserInfo - �����û���Ϣ 
����ֵ��
	����ɹ�������TRUE & �û�������Ϣ�����򷵻�FALSE
����������
	�õ�����Ķ�������
����: 
	
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
������DWORD SetHandleUserInfo( HANDLE handle, DWORD dwUserInfo )
������
	IN handle - ���
	IN dwUserInfo - �û�������Ϣ
����ֵ��
	����ɹ�������֮ǰ���û�������Ϣ�����򷵻�0
����������
	���þ�����û�������Ϣ
����: 
	
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
������BOOL CALLBACK _DeleteHandleObject( LPOBJLIST lphObjList, LPARAM lParam )
������
	IN lphObjList - ������ʽ
	IN lParam - �����д��Ƿ��ӡ������Ϣ
����ֵ��
	����ɹ�������TRUE�����򷵻�FALSE
����������
	���ö�����ͷź�����ɾ�����
����: 
	
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
������BOOL WINAPI KL_CloseHandle( HANDLE handle )
������
	IN handle - ���
����ֵ��
	����ɹ�������TRUE�����򷵻�FALSE
����������
	�رվ������
����: 
	ϵͳ����	
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
			// ���ü����Ƿ�Ϊ0
			if( --lphObj->obj.iRefCount == 0 )
			{   // �ǣ��رոö���
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
������Handle_CloseAll( LPPROCESS lpOwnerProcess )
������
	IN lpOwnerProcess - ���ӵ���߽���
����ֵ��
	����ɹ�������TRUE�����򷵻�FALSE
����������
	�رս���ӵ�е����о������
����: 
	�����˳�ʱ����
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
