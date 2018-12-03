/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ź�������
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2004-12-09, ���Ӷ��߳��˳��� CS �ĸ�λ���� _Semaphore_HandleThreadExit
	2004-08-31, ȥ�� SEMAPHORE�� objType�� hThis ����
    2004-08-11, �����ö�̬����( WaitForSingleObject��WaitMultiSingleObject  ) �ź������漰�� semaphpor.c thread.c sche.c
	2004-05-18: ��� PulseEvent
    2003-05-22: ���Ӷ� CURRENT_PROCESS_HANDLE �͡�CURRENT_THREAD_HANDLE �Ĵ���
    2003-04-24:
	    1.�����ź����ġ����� ����
		2.WaitForSingleObject���Ӷ��߳̾���Ĵ���

******************************************************/

#include <eframe.h>
#include <eobjlist.h>
#include <epcore.h>
#include <epalloc.h>
#include <coresrv.h>

extern void UpCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs );
extern BOOL DownCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs, BOOL bEntryBlock );
extern VOID DumpCallStack( LPTHREAD );

static LPSEMAPHORE lpSemList = NULL;
static CRITICAL_SECTION csSemList;

// ********************************************************************
// ������BOOL _InitSemMgr( void )
// ������
//		��
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		��ʼ���ź���������
// ���ã�
//	
// ********************************************************************
BOOL _InitSemMgr( void )
{
	

	KC_InitializeCriticalSection( &csSemList );
#ifdef __DEBUG
	csSemList.lpcsName = "CS-SEM"; // ���ڵ���
#endif
	return TRUE;
}

// ********************************************************************
// ������LPSEMAPHORE _SemaphoreCreate(
//							 LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  // must is 0
//							 int nInitialCount,                         // initial count
//							 int nMaximumCount,                     // maximum count
//							 LPCTSTR lpName,     // must is 0
//							 UINT uiFlag
//							 )
// ������
//	IN lpSemaphoreAttributes - ��ȫ���ԣ���֧�֣�����ΪNULL��
//	IN nInitialCount - ��ʼ��ʱӵ�е��ź�����
//	IN nMaximumCount - �����ź���
//	IN lpName - �ź�����������ΪNULL��
//	IN uiFlag - ��־,Ϊ��
//			SF_SEMAPHORE - Ϊ�ź�������
//			SF_MUTEX - ����������
//			SF_EVENT - �¼�����
//			SF_OWNER - �趨ӵ�����߳�(���MUTEX����)
//			SF_MANUALRESET - �ֹ����裨���EVENT����
// ����ֵ��
//	����ɹ���������Ч��SEMAPHORE�ṹָ�룻���򣬷���NULL
// ����������
//	�����ź�������
// ���ã�
//	
// ********************************************************************

LPSEMAPHORE _SemaphoreCreate(
							 LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  // must is 0
							 int nInitialCount,                         // initial count
							 int nMaximumCount,                     // maximum count
							 LPCTSTR lpName,     // must is 0
							 UINT uiFlag
							 )
{
    LPSEMAPHORE lpsem;
    int iStrLen;

    if( lpName && lpSemList )
    {    //���������Ȳ��ҵ�ǰ�Ƿ���ڸ���
        for( lpsem = lpSemList; lpsem; lpsem = lpsem->lpNext )
        {
            if( lpsem->lpszName && stricmp( lpName, lpsem->lpszName ) == 0 )
            {   // �ҵ���ͬ�� ��find same name
				KL_SetLastError( ERROR_ALREADY_EXISTS );
				if( (WORD)(uiFlag & 0xf) == (lpsem->semFlag & 0xf) )
				{   // ͬ���Ķ���
					lpsem->nRefCount++;	//��������
					goto _ret;
				}
				else
				{   // ͬ������ͬ�Ķ���diff object but same name
					lpsem = NULL; // error
					goto _ret;
				}
            }
        }
    }
	// û���ҵ�
    iStrLen = 0;
    if( lpName )
    {
        iStrLen = strlen( lpName );
    }
	// ����ṹ
    lpsem = (LPSEMAPHORE)KHeap_Alloc( sizeof(SEMAPHORE) );
	if( lpsem )
    {	// ��ʼ��
		memset( lpsem, 0, sizeof( SEMAPHORE ) );
		if( lpName )
		{
		    lpsem->lpszName = KHeap_AllocString( iStrLen + 1 );
			if( lpsem->lpszName == NULL )
			{
				KHeap_Free( lpsem, sizeof(SEMAPHORE) );
				lpsem = NULL;
				goto _ret;
            }
			strcpy( lpsem->lpszName, lpName );
		}
		else
			lpsem->lpszName = NULL;
        //lpsem->objType = OBJ_SEMAPHORE; // 2004-08-31
        lpsem->nCount = nInitialCount;
        lpsem->nWaiting = 0;
        lpsem->nMaxCount = nMaximumCount;
        lpsem->lpWaitQueue = 0;
        lpsem->nRefCount = 1;
        lpsem->semFlag = uiFlag;
		lpsem->nHighPriority = IDLE_PRIORITY;
		
		if( (uiFlag & SF_OWNER) && nInitialCount )
		{
			lpsem->lpOwner = lpCurThread;
			lpsem->nLockCount = 1;
		}
		//	��������ͷ
        lpsem->lpNext = lpSemList;
        lpSemList = lpsem;
    }
_ret:
    return lpsem;
}

// ********************************************************************
// ������BOOL _SemaphoreRemove( LPSEMAPHORE lpsem )
// ������
//		IN lpsem -  SEMAPHORE�ṹָ��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�ù��ܽ��ź����Ƴ�����
// ���ã�
//	
// ********************************************************************

static BOOL _SemaphoreRemove( LPSEMAPHORE lpsem )
{
	//KL_EnterCriticalSection( &csSemList );
	//if( --lpsem->nRefCount == 0 )	// ���ü��� = 0 ?
	{	// �ǣ�û���κ����� ���ͷ��� 
		LPSEMAPHORE lp = lpSemList;

//		ASSERT( lpSemList );
		//	�������Ƴ�
		if( lp == lpsem )
		{  // head 
			lpSemList = lp->lpNext;
		}
		else
		{	//
		    while( lp && lp->lpNext != lpsem )
			{
				lp = lp->lpNext;
			}
			if( lp && lp->lpNext == lpsem )
			{
				lp->lpNext = lpsem->lpNext;
			}
			else
			{
				ASSERT( lpSemList );
			}
		}
		//lpsem->objType = OBJ_NULL;
		// �ͷ�
		//if( lpsem->lpszName )
		  //  KHeap_FreeString( lpsem->lpszName );
        //KHeap_Free( lpsem, sizeof(SEMAPHORE) );
	}
	//KL_LeaveCriticalSection( &csSemList );
	return TRUE;
}

// ********************************************************************
// ������BOOL _SemaphoreDeletePtr( LPSEMAPHORE lpsem )
// ������
//		IN lpsem -  SEMAPHORE�ṹָ��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		ɾ���ź����ڴ�
// ���ã�
//	
// ********************************************************************
/*
static BOOL _SemaphoreDeletePtr( LPSEMAPHORE lpsem )
{	//2004-08-31, delete
	if( lpsem->lpszName )
		KHeap_FreeString( lpsem->lpszName );
	KHeap_Free( lpsem, sizeof(SEMAPHORE) );			
	return TRUE;
}
*/
// ********************************************************************
// ������BOOL _SemaphoreDelete( LPSEMAPHORE lpsem )
// ������
//		IN lpsem -  SEMAPHORE�ṹָ��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�� _SemaphoreCreate ��Ӧ���ù����ͷ����������Դ
// ���ã�
//	
// ********************************************************************
#define DEBUG_SemaphoreDelete 0
static BOOL _SemaphoreDelete( LPSEMAPHORE lpsem )
{
	//BOOL nRefCount;
//	UINT uiSave;
	//LockIRQSave( &uiSave );
	//nRefCount = lpsem->nRefCount;
	//if( lpsem->nRefCount )
		//lpsem->nRefCount--;
	//UnlockIRQRestore( &uiSave );
	
	if( lpsem->nRefCount == 0 )
	{// �ǣ�û���κ����� �����Ƴ������ͷ�
		if( _SemaphoreRemove( lpsem ) )
		{	//�ͷ�
			//_SemaphoreDeletePtr( lpsem );
			if( lpsem->lpszName )
				KHeap_FreeString( lpsem->lpszName );
			KHeap_Free( lpsem, sizeof(SEMAPHORE) );
		}
	}
	else
	{
		//ASSERT( 0 );
		ERRORMSG( DEBUG_SemaphoreDelete, ( "_SemaphoreDelete: error lpsem->nRefCount=%d.\r\n", lpsem->nRefCount ) );
	}
	return TRUE;
}
// ********************************************************************
// ������BOOL _SemaphoreRelease( 
//                            LPSEMAPHORE lpsem, 
//							  int lReleaseCount, 
//							  LPINT lpPreviousCount )

// ������
//	IN lpsem - SEMAPHORE �ṹָ��
//	IN lReleaseCount - �ͷŵ���Դ��
//	OUT lpPreviousCount - ���ڽ���֮ǰ����Դ�� 
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�ͷ��ź��������統ǰ�еȴ��ź������̣߳�Ҳ�ͷ�����
// ���ã�
//	
// ********************************************************************

BOOL _SemaphoreRelease( 
					   LPSEMAPHORE lpsem, 
					   int lReleaseCount, 
					   LPINT lpPreviousCount )
{
    //ASSERT( lpsem->objType == OBJ_SEMAPHORE ); //2004-08-31
	// ���浱ǰ��
	if( lpPreviousCount )
		*lpPreviousCount = lpsem->nCount;
	// �ͷ��ź��� & �߳�
	if( lReleaseCount )
	    UpSemaphore( lpsem, lReleaseCount );
	return TRUE;
}


// ********************************************************************
// ������VOID _Semaphore_HandleThreadExit( 
//                            LPVOID lpOwnerID  )
// ������
//	IN lpOwnerID - ӵ����ID
// ����ֵ��
//	��
// ����������
//	���߳��˳���û���ͷ�semʱ�����������ĸ�λ
// ���ã�
//	
// ********************************************************************

VOID _Semaphore_HandleThreadExit( LPVOID lpOwnerID )
{
	LPSEMAPHORE lpsem;

	KL_EnterCriticalSection( &csSemList );//�����뻥���

	lpsem = lpSemList;
		
	while( lpsem )
	{
		if( lpsem != csSemList.hSemaphore &&
			lpsem->lpOwner == lpOwnerID )
		{
			UpSemaphore( lpsem, 1 );
			lpsem->lpOwner = NULL;
			lpsem = lpSemList;
			lpsem->nHighPriority = IDLE_PRIORITY;
			continue;
		}
		else
			lpsem = lpsem->lpNext;
	}


	KL_LeaveCriticalSection( &csSemList );//�����뻥���
}

// ********************************************************************
// ������static BOOL _SemaphoreReset( LPSEMAPHORE lpsem )
// ������
//	IN lpsem - SEMAPHORE �ṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	��λ�ź��������ź�״̬
// ���ã�
//	
// ********************************************************************

static BOOL _SemaphoreReset( LPSEMAPHORE lpsem )
{
	UINT uiSave;
	LockIRQSave( &uiSave );

	//INTR_OFF(); // �ر��ж�
	lpsem->nCount = 0;
	//INTR_ON();  // ���ж�

	UnlockIRQRestore( &uiSave );

	return TRUE;
}


// ********************************************************************
// ������static HANDLE _CreateHandleObj(
//                   LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  // must is NULL
//                   int nInitialCount,                         // initial count
//                   int nMaximumCount,                     // maximum count
//                   LPCTSTR lpName,     // must is NULL
//                   UINT uiFlag,
//				     DWORD objType,
//				     LPPROCESS lpOwnerProcess
//                   )
// ������
//	IN lpSemaphoreAttributes - ��ȫ���ԣ���֧�֣�����ΪNULL)
//	IN nInitialCount - ��ʼ���ź�������
//	IN nMaximumCount - �����ź�����
//	IN lpName - ���ַ�����ʾ�Ķ�����
//	IN uiFlag - ��־��Ϊ��
//					SF_SEMAPHORE���� �ź�������
//					SF_MUTEX - ����������
//					SF_EVENT - �¼�����
//					SF_OWNER - �趨ӵ�����߳�(���MUTEX����)
//					SF_MANUALRESET - �ֹ����裨���EVENT����
//	IN objType - �������ͣ����ݸ�Hanle_Alloc�Ĳ���������Ϊ��
//						OBJ_SEMAPHORE
//						OBJ_MUTEX - ����������
//						OBJ_EVENT - �¼�����
//	IN lpOwnerProcess - �ö����ӵ����
// ����ֵ��
//	����ɹ������ؾ�������򣬷���NULL
// ����������
//	����һ�����ھ���Ķ���
// ���ã�
//	
// ********************************************************************

static HANDLE _CreateHandleObj(
                   LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  //
                   int nInitialCount,                         // initial count
                   int nMaximumCount,                     // maximum count
                   LPCTSTR lpName,     //
                   UINT uiFlag,
				   DWORD objType,
				   LPPROCESS lpProcess
                   )
{
	LPSEMAPHORE lpvData;
	HANDLE h = NULL;

	KL_EnterCriticalSection( &csSemList );//�����뻥���
	// �����ڴ����
	lpvData = _SemaphoreCreate( lpSemaphoreAttributes, nInitialCount, nMaximumCount, lpName, uiFlag );

	if( lpvData ) // �ɹ��𡡣�
	{	//���ɹ���Ϊ�ö������һ�����
		h = Handle_Alloc( lpProcess, lpvData, objType );
		if( h == NULL )  // �������ɹ���
		{	// ��ɾ��֮ǰ������ڴ����
			lpvData->nRefCount = 0;
			_SemaphoreDelete( lpvData );
		}
		else
		{   //2005-01-31, delete
			;//lpvData->hThis = h;
		}
	}
	KL_LeaveCriticalSection( &csSemList );// �뿪�����
	return h;


}

//�ͷ��ź�����ԭ�Ӳ�����
BOOL _CloseSemaphoreObj( HANDLE hSemaphore, UINT uObjType )
{
	BOOL retv = FALSE;
	LPSEMAPHORE lpho;

	KL_EnterCriticalSection( &csSemList );
	
	if( uObjType )
		lpho = HandleToPtr( hSemaphore, uObjType );// �ɾ���õ�����ָ��
	else
		lpho = (LPSEMAPHORE)hSemaphore;  //ָ�����
	//2004-08-31
	//ASSERT( lpho->objType == OBJ_SEMAPHORE );
	if( lpho )
	{
		//nRefCount = lpsem->nRefCount;
		if( lpho->nRefCount == 1 )
		{
			lpho->nRefCount = 0;
			if( _SemaphoreDelete( lpho ) )
			{
				retv = TRUE;
			}
		}
		else
		{
			ASSERT( lpho->nRefCount > 1 );
			lpho->nRefCount--;
		}
	}
	KL_LeaveCriticalSection( &csSemList );

	return retv;
}

// ********************************************************************
// ������HANDLE WINAPI KL_CreateSemaphore(
//                   LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  //
//                   int nInitialCount,                         // initial count
//                   int nMaximumCount,                     // maximum count
//                   LPCTSTR lpName     //
//                   )

// ������
//	IN lpSemaphoreAttributes-��ȫ������,ΪNULL
//	IN nInitialCount-��ʼ����
//	IN nMaximumCount-������
//	IN lpName-���ַ�����ʾ�Ķ�����

// ����ֵ��
//	�ɹ��������ź������
//	���򣺷���NULL
// ����������
//	����һ���ź���	
// ���ã�
//	
// ********************************************************************

HANDLE WINAPI KL_CreateSemaphore(
                   LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  //
                   int nInitialCount,                         // initial count
                   int nMaximumCount,                     // maximum count
                   LPCTSTR lpName     //
                   )
{
    return _CreateHandleObj( lpSemaphoreAttributes, nInitialCount, nMaximumCount, lpName, SF_SEMAPHORE, OBJ_SEMAPHORE, GetAPICallerProcessPtr() );
}

// ********************************************************************
// ������BOOL WINAPI KL_ReleaseSemaphore(
//					   HANDLE hSemaphore,
//					   int lReleaseCount,       //
//					   LPINT lpPreviousCount   // 
//					   )

// ������
//	IN hSemaphore-�ź������
//	IN lReleaseCount-�ͷż���
//	OUT lpPreviousCount-���ڽ���ǰ���ź�������

// ����ֵ��
//	�ɹ������ط���
//	���򣺷���0
	
// ����������
//	�ͷ��źţ��������߳��ڵȴ����ź������ͷ���Щ�߳�
// ���ã�
//	ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_ReleaseSemaphore(
					   HANDLE hSemaphore,
					   int lReleaseCount,       //
					   LPINT lpPreviousCount   //
					   )
{
    LPSEMAPHORE lpho = HandleToPtr( hSemaphore, OBJ_SEMAPHORE ); // �ɾ���õ�����ָ��
	
	if( lpPreviousCount )
		*lpPreviousCount = 0;

    if( lpho )
    {
		return _SemaphoreRelease( lpho, lReleaseCount, lpPreviousCount );
    }
    return FALSE;
}

// ********************************************************************
// ������BOOL FASTCALL Semaphore_Close( HANDLE hSemaphore ) 
// ������
//	IN hSemaphore - �ź������
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	ɾ���ź������������
// ���ã�
//	��CloseHandle����
// ********************************************************************

BOOL FASTCALL Semaphore_Close( HANDLE hSemaphore ) 
{
	return _CloseSemaphoreObj( hSemaphore, OBJ_SEMAPHORE );
}

// ********************************************************************
// ������HANDLE WINAPI KL_CreateMutex(
//					      LPSECURITY_ATTRIBUTES lpMutexAttributes,    // must is 0
//					      BOOL bInitialOwner,                     // flag for initial ownership
//					      LPCTSTR lpName                      // must is 0
//						)
// ������
//	IN lpMutexAttributes - ��ȫ���ԣ�����ΪNULL��
//	IN bInitialOwner - �Ƿ񴴽�ʱ��ӵ�иû�����
//	IN lpName - ��������������ΪNULL��
// ����ֵ��
//	����ɹ������ػ��������; ���򷵻�NULL
// ����������
//		����������, ���lpName��ΪNULL���򴴽�һ���������������������̿�����ͬ�����øú����õ��Ըû������Ŀ��������
//	������������������Ҷ�����̻��õ�����Ӧ��ʹbInitialOwnerΪFALSE��ӵ�л��������߳̿��Զ�ε��õȴ����ܶ�����
//	����ס����ʱ����Ӧ�õ���ͬ������ͷŹ��ܣ�ReleaseMutex����
//		������ֻ�ܱ�һ���߳���ӵ��
//		��CloseHandleȥɾ��������
// ���ã�
//	ϵͳAPI
// ********************************************************************

HANDLE WINAPI KL_CreateMutex(
      LPSECURITY_ATTRIBUTES lpMutexAttributes,    // must is 0
      BOOL bInitialOwner,                     // flag for initial ownership
      LPCTSTR lpName                      // must is 0
     )
{
	return _CreateHandleObj( lpMutexAttributes, bInitialOwner ? 0 : 1, 1, lpName, SF_MUTEX | SF_OWNER, OBJ_MUTEX, GetAPICallerProcessPtr() );
}

// ********************************************************************
// ������BOOL WINAPI KL_ReleaseMutex( HANDLE hMutex )
// ������
//		IN hMutex - ���������
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		�ͷŶԻ�������ӵ��Ȩ��ʹ�������������źš�ֻ��ӵ�иû��������̲߳����ͷ���
// ���ã�
//		ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_ReleaseMutex( HANDLE hMutex )
{
    LPSEMAPHORE lpho = HandleToPtr( hMutex, OBJ_MUTEX );	//�ɾ���õ�����ָ��
    if( lpho )	//��Ч��
	{	//��
		if( lpho->lpOwner == lpCurThread )		//�Ƿ�ǰ�߳�ӵ�иû�����
		{	//��
			if( lpho->nLockCount == 1 )		//��ǰ�߳��ظ��ȴ��û������Ĵ����Ƿ�Ϊ 1
		        return _SemaphoreRelease( lpho, 1, 0 );  // �ǣ��ͷ���
			else	
			{
				lpho->nLockCount--;	//�񣬽�����ȥ�ظ�����
			    return TRUE;
			}
		}
	}
	return FALSE;
}



// ********************************************************************
// ������HANDLE WINAPI KL_CreateEvent(
//				LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
//  			BOOL bManualReset, 
//				BOOL bInitialState, 
//				LPTSTR lpName )
// ������
//		IN lpEventAttributes - ��ȫ����
//		IN bManualReset - �Ƿ��ֹ������ź�
//		IN bInitialState - �¼���״̬,����ΪTRUE,�¼�������Ϊ���ź�״̬
//		IN lpName - �¼���������ΪNULL��
// ����ֵ��
//		����ɹ���������Ч�ľ��ֵ�����򣬷���NULL
// ����������
//	�����¼����󡣼���lpName��ΪNULL���򴴽�һ���������̿��Է��ʵ������¼���
//	���Ϊ�����¼���֮ǰ�Ѿ������˸��¼��������bManualReset��bInitialState
//	���bManualResetΪFALSE,���¼����źŲ������̵߳ȴ����¼�����ϵͳ�ͷŵ�
//	���̲߳����Զ������¼�������Ϊ���ź�״̬
// ���ã�
//		ϵͳAPI
// ********************************************************************

HANDLE WINAPI KL_CreateEvent(
      LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
  	  BOOL bManualReset, 
	  BOOL bInitialState, 
	  LPCTSTR lpName ) 
{
	UINT uiFlag;
	HANDLE h;
	if( bManualReset )
		uiFlag = SF_MANUALRESET;
	else
		uiFlag = 0;

	h = _CreateHandleObj( lpEventAttributes, bInitialState ? 1 : 0, 1, lpName, SF_EVENT | uiFlag , OBJ_EVENT, GetAPICallerProcessPtr() );
	if( h && bInitialState )
	{
	    LPVOID lpho = HandleToPtr( h, OBJ_EVENT );//�ɾ���õ�����ָ��
        _SemaphoreRelease( (LPSEMAPHORE)lpho, 1, NULL );
	}
	return h;
}

// ********************************************************************
// ������BOOL WINAPI KL_ResetEvent( HANDLE hEvent )
// ������
//		IN hEvent - �¼����
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE	
// ����������
//		���¼�������Ϊ���ź�״̬
// ���ã�
//		ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_ResetEvent( HANDLE hEvent )
{
    LPVOID lpho = HandleToPtr( hEvent, OBJ_EVENT );	//�ɾ���õ�����ָ��
    if( lpho )//
	{	// ��Ч��ָ��
		return _SemaphoreReset( (LPSEMAPHORE)lpho );//
	}

	return FALSE;        
}

// ********************************************************************
// ������BOOL WINAPI KL_SetEvent( HANDLE hEvent )
// ������
//		IN hEvent - �¼�������
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		�����¼�Ϊ�ź�״̬������еȴ����̣߳����ͷ����ǡ�
//		�߳�ͨ���ȴ����ܼ���¼��źţ����̵߳õ��źź󣬶��źŵĴ���Ϊ�£�
//		�����¼�Ϊ�ֹ����蹦�ܣ����¼����źŲ���ı�ֱ��������ResetEvent��
//		�����¼�Ϊ���ֹ����蹦�ܣ����ź�����Ϊ���źš�
//		����ֻ��һ���ȴ����߳��ܵõ��ź�
// ���ã�
//		ϵͳAPI
// ********************************************************************
BOOL WINAPI KL_SetEvent( HANDLE hEvent )
{
    LPVOID lpho = HandleToPtr( hEvent, OBJ_EVENT );//�ɾ���õ�����ָ��
    if( lpho )
	{
		//(LPSEMAPHORE)lpho->nPulseCount = 0;
		return _SemaphoreRelease( (LPSEMAPHORE)lpho, 1, NULL );
	}
	return FALSE;
}

// ********************************************************************
// ������BOOL WINAPI KL_PulseEvent( HANDLE hEvent )
// ������
//		IN hEvent - �¼�������
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		�����¼�Ϊ�ź�״̬������еȴ����̣߳����ͷ����ǡ�
//		�߳�ͨ���ȴ����ܼ���¼��źţ����̵߳õ��źź󣬶��źŵĴ���Ϊ�£�
//		�����¼�Ϊ�ֹ����蹦�ܣ����ź�����Ϊ���źţ�
//		�����¼�Ϊ���ֹ����蹦�ܣ����ź�����Ϊ���źš�
//		�����¼�Ϊ�ֹ����蹦�ܣ���ǰ�ȴ��������߳��ܵõ��źŲ����ͷ�
//		�����¼�Ϊ���ֹ����蹦�ܣ�����һ���߳��ܵõ��ź�
// ���ã�
//	
// ********************************************************************

BOOL WINAPI KL_PulseEvent( HANDLE hEvent )
{
    LPSEMAPHORE lpho = (LPSEMAPHORE)HandleToPtr( hEvent, OBJ_EVENT );//�ɾ���õ�����ָ��
    if( lpho )
	{
		
		//(LPSEMAPHORE)lpho->nPulseCount = (LPSEMAPHORE)lpho->nWaiting;
		if( lpho->semFlag & SF_MANUALRESET )
		{
			UINT uiSave;
			LockIRQSave( &uiSave );
			if(lpho->lpWaitQueue)
			{
				LPWAITQUEUE lpWaitHead = WAIT_QUEUE_HEAD( &lpho->lpWaitQueue );
				LPWAITQUEUE lpWaitQueue = lpho->lpWaitQueue;
				while( lpWaitQueue != lpWaitHead )
				{
					lpWaitQueue->lpThread->flag |= FLAG_PULSE_EVENTING;
					lpWaitQueue = lpWaitQueue->lpNext;
				}
			}
			UnlockIRQRestore( &uiSave );
		}
		
		if( _SemaphoreRelease( (LPSEMAPHORE)lpho, 1, NULL ) )
		{
			if( lpho->semFlag & SF_MANUALRESET )
			{
			    return _SemaphoreReset( (LPSEMAPHORE)lpho );
			}
		}
	}
	return FALSE;        
}

// ********************************************************************
// ������BOOL FASTCALL Event_Close( HANDLE hEvent )
// ������
//		IN hEvent - �¼�������
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		��ϵͳ����CloseHandleʱ������������Ϊ�¼�����CloseHandle�����øú���
// ���ã�
//		hmgr.c
// ********************************************************************

BOOL FASTCALL Event_Close( HANDLE hEvent ) 
{
	return _CloseSemaphoreObj( hEvent, OBJ_EVENT );
}

// ********************************************************************
// ������BOOL FASTCALL Mutex_Close( HANDLE hMutex )
// ������
//		IN hEvent - �¼�������
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		��ϵͳ����CloseHandleʱ������������ΪMutex����CloseHandle�����øú���
// ���ã�
//		hmgr.c
// ********************************************************************

BOOL FASTCALL Mutex_Close( HANDLE hMutex )
{
	return _CloseSemaphoreObj( hMutex, OBJ_MUTEX );
}

// ********************************************************************
// ������VOID WINAPI KL_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// ������
//		IN lpCriticalSection - CRITICAL_SECTION�ṹָ��
// ����ֵ��
//		��
// ����������
//		��������ʼ��һ����ͻ�ζ���
// ���ã�
//		ϵͳAPI
// ********************************************************************

VOID WINAPI KL_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	if( lpCriticalSection )
	{
		memset( lpCriticalSection, 0, sizeof( CRITICAL_SECTION ) );
		lpCriticalSection->dwOwnerThreadId = -1;
		lpCriticalSection->hSemaphore = _CreateHandleObj( NULL, 1, 1, NULL, SF_SEMAPHORE, OBJ_SEMAPHORE, GetAPICallerProcessPtr() );//KL_CreateMutex( 0, 0, NULL );//lpCriticalSection->lpcsName );
		//ASSERT( lpCriticalSection->hSemaphore );
	}
	else
	{
		ERRORMSG( 1 , ( "KL_InitializeCriticalSection error: null ptr,pc=0x%x,proc=%s.\r\n", lpCurThread->lpCallStack->pfnRetAdress,lpCurThread->lpCurProcess->lpszApplicationName ) );
	}
}


// ********************************************************************
// ������VOID FASTCALL KC_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// ������
//		IN lpCriticalSection - CRITICAL_SECTION�ṹָ��
// ����ֵ��
//		��
// ����������
//		��������ʼ��һ����ͻ�ζ���
// ���ã�
//		�ڲ�����
// ********************************************************************

// use by kernel
#define CSF_KERNEL  0x80000000
#define CSF_DEBUG   0x00000001

VOID FASTCALL KC_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	memset( lpCriticalSection, 0, sizeof( CRITICAL_SECTION ) );
	lpCriticalSection->dwOwnerThreadId = -1;
    lpCriticalSection->hSemaphore = _SemaphoreCreate( NULL, 1, 1, NULL, SF_SEMAPHORE );
	//ASSERT( lpCriticalSection->hSemaphore );
	lpCriticalSection->uiFlag = CSF_KERNEL;
}

// ********************************************************************
// ������VOID WINAPI KL_DeleteCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// ������
//		IN lpCriticalSection - CRITICAL_SECTION�ṹָ��
// ����ֵ��
//		��
// ����������
//		��KL_InitializeCriticalSection�෴���ù���ɾ����ͻ�ζ���
// ���ã�
//	
// ********************************************************************

VOID WINAPI KL_DeleteCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	if( lpCriticalSection )
	{
		if( lpCriticalSection->uiFlag & CSF_KERNEL )
		{
			_CloseSemaphoreObj( lpCriticalSection->hSemaphore, 0 );
		}
		else
			KL_CloseHandle( lpCriticalSection->hSemaphore );
		lpCriticalSection->hSemaphore = NULL;
		if( lpCriticalSection->uiFlag & CSF_DEBUG )
		{
			RETAILMSG( 1, ( "DeleteCriticalSection:lpcs=0x%x,lpcsName=%s.\r\n", lpCriticalSection, lpCriticalSection->lpcsName ) );
		}
	}
	else
	{
		ERRORMSG( 1 , ( "KL_DeleteCriticalSection error: null ptr,pc=0x%x,proc=%s.\r\n", lpCurThread->lpCallStack->pfnRetAdress,lpCurThread->lpCurProcess->lpszApplicationName ) );
		DumpCallStack( lpCurThread );		
	}
}

// ********************************************************************
// ������VOID WINAPI KL_LeaveCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// ������
//		IN lpCriticalSection - CRITICAL_SECTION�ṹָ��
// ����ֵ��
//		��
// ����������
//		�뿪��ͻ��
// ���ã�
//		ϵͳAPI
// ********************************************************************

VOID WINAPI KL_LeaveCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	if( lpCriticalSection )
	{
		if( lpCriticalSection->dwOwnerThreadId == lpCurThread->dwThreadId )	//��ǰ�߳�ӵ�иó�ͻ����
		{	//��
#ifdef __DEBUG
			if( lpCriticalSection->uiFlag & CSF_DEBUG )		//�Ƿ����������Ϣ
			{	//��
				RETAILMSG( 1, ( "LeaveCriticalSection:lpcs=0x%x,lpcsName=%s,iLockCount=%d.\r\n", lpCriticalSection, lpCriticalSection->lpcsName, lpCriticalSection->iLockCount ) );
			}
#endif
			
			if( lpCriticalSection->iLockCount != 1 )  // ���һ���뿪�� ��
				lpCriticalSection->iLockCount--;  // ���ǣ��򵥵Ľ���������һ��ͬһ���߳��ܶ�ν����ͻ�Σ�
			else
			{	// �����һ���뿪
				LPSEMAPHORE lpho;
				
#ifdef DEBUG_CRITSEC
				if( lpCriticalSection->lpcsName )
				{
					DWORD dwCurThreadId = KL_GetCurrentThreadId();
					RETAILMSG( 1, ( "upcs=%s, cid=0x%x, oid=0x%x, count=%d\r\n", lpCriticalSection->lpcsName, dwCurThreadId, lpCriticalSection->dwOwnerThreadId, lpCriticalSection->iLockCount ) );
				}
#endif //DEBUG_CRITSEC
				//	
				lpCriticalSection->dwOwnerThreadId |= 1;  // ��ֹ���е��̲߳����ö��� mask all threads with the crit
				if( lpCriticalSection->uiFlag & CSF_KERNEL )	//�Ƿ����ں��Լ��Ĵ�����
					lpho = (LPSEMAPHORE)lpCriticalSection->hSemaphore;  // ��
				else
					lpho = HandleToPtr( lpCriticalSection->hSemaphore, -1 );	// �û�����	
				if( lpho )
					UpCrit( lpho, lpCriticalSection );	//����ԭ�Ӳ���
			}
		}
		else
		{
			RETAILMSG( 1, ( "KL_LeaveCriticalSection error: the thread( handle(0x%x), id(0x%x), curproc=%s, ownerproc=%s )is not own CriticalSection.\r\n", lpCurThread->hThread, lpCurThread->dwThreadId, lpCurThread->lpCurProcess->lpszApplicationName, lpCurThread->lpOwnerProcess->lpszApplicationName ) );
			DumpCallStack( lpCurThread );			
		}
	}
	else
	{
		ERRORMSG( 1 , ( "KL_LeaveCriticalSection error: null ptr,pc=0x%x,id=0x%x,curproc=%s, ownerproc=%s.\r\n", lpCurThread->lpCallStack->pfnRetAdress,lpCurThread->dwThreadId, lpCurThread->lpCurProcess->lpszApplicationName, lpCurThread->lpOwnerProcess->lpszApplicationName ) );
		DumpCallStack( lpCurThread );		
	}
}

// ********************************************************************
// ������DoEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection, BOOL bEntryBlock )
// ������
//		IN lpCriticalSection - CRITICAL_SECTION�ṹָ��
//		IN bEntryBlock - ����ӵ�г�ͻ��ʱ���Ƿ��˳�
// ����ֵ��
//		��ӵ�г�ͻ�Σ�����TRUE; ���򣬷���FALSE
// ����������
//		�����ͻ��,�����Ƿ�ӵ�У������˳�
// ���ã�
//		ϵͳAPI
// ********************************************************************

static BOOL DoEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection, BOOL bEntryBlock )
{
	BOOL bRetv = FALSE;
	if( lpCriticalSection )
	{
		if( lpCriticalSection->dwOwnerThreadId == lpCurThread->dwThreadId )		//��ǰ�߳�ӵ�иó�ͻ����
		{	//�ǣ��򵥵Ľ���������һ
			lpCriticalSection->iLockCount++;
			bRetv = TRUE;
			
#ifdef __DEBUG
			if( lpCriticalSection->uiFlag & CSF_DEBUG )
			{
				RETAILMSG( 1, ( "EnterCriticalSection skip:lpcs=0x%x,lpcsName=%s,iLockCount=%d.\r\n", lpCriticalSection, lpCriticalSection->lpcsName, lpCriticalSection->iLockCount ) );
			}
#endif
		}
		else
		{	// ��ӵ�У���
			LPSEMAPHORE lpho;
			
#ifdef __DEBUG
			if( lpCriticalSection->uiFlag & CSF_DEBUG )
			{
				RETAILMSG( 1, ( "CS-wait...:lpcs=0x%x,lpcsName=%s,Owner Thread=0x%x.\r\n", lpCriticalSection, lpCriticalSection->lpcsName, lpCriticalSection->dwOwnerThreadId ) );
			}
#endif
			
#ifdef DEBUG_CRITSEC
			
			if( lpCriticalSection->lpcsName ) 
			{
				DWORD dwCurThreadId = KL_GetCurrentThreadId();
				RETAILMSG( 1, ( "downcs-enter=%s, cid=0x%x, oid=0x%x, count=%d\r\n", lpCriticalSection->lpcsName, dwCurThreadId, lpCriticalSection->dwOwnerThreadId, lpCriticalSection->iLockCount ) );
			}
#endif	
			
			if( lpCriticalSection->uiFlag & CSF_KERNEL )//�Ƿ����ں��Լ��Ĵ�����
				lpho = (LPSEMAPHORE)lpCriticalSection->hSemaphore; //��
			else
				lpho = (LPSEMAPHORE)HandleToPtr( lpCriticalSection->hSemaphore, -1 ); // ���û�����
			lpho = GetSemaphoreAndLock( lpho, 0 );
			if( lpho )
			{			
				bRetv = DownCrit( lpho, lpCriticalSection, bEntryBlock );	//����ԭ�Ӳ���
				ReleaseSemaphoreAndUnlock( lpho );			
			}
			else
			{
				ERRORMSG( 1 , ( "KL_EnterCriticalSection error: null lpho,id=0x%x,pc=0x%x,proc=%s.\r\n", lpCurThread->dwThreadId, lpCurThread->lpCallStack->pfnRetAdress,lpCurThread->lpCurProcess->lpszApplicationName ) );
				DumpCallStack( lpCurThread );
			}
			
#ifdef DEBUG_CRITSEC
			if( lpCriticalSection->lpcsName ) 
			{
				DWORD dwCurThreadId = KL_GetCurrentThreadId();
				RETAILMSG( 1, ( "downcs-exit=%s, cid=0x%x, oid=0x%x, count=%d\r\n", lpCriticalSection->lpcsName, dwCurThreadId, lpCriticalSection->dwOwnerThreadId, lpCriticalSection->iLockCount ) );
			}
#endif
		}
	}
	else
	{
		ERRORMSG( 1 , ( "KL_EnterCriticalSection error: null ptr,pc=0x%x,proc=%s.\r\n", lpCurThread->lpCallStack->pfnRetAdress,lpCurThread->lpCurProcess->lpszApplicationName ) );
		DumpCallStack( lpCurThread );		
	}
	return bRetv;
}

// ********************************************************************
// ������VOID WINAPI KL_EnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// ������
//		IN lpCriticalSection - CRITICAL_SECTION�ṹָ��
// ����ֵ��
//		��
// ����������
//		�����ͻ��
// ���ã�
//		ϵͳAPI
// ********************************************************************

VOID WINAPI KL_EnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	DoEnterCriticalSection( lpCriticalSection, TRUE );
}

// ********************************************************************
// ������BOOL WINAPI KL_TryEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// ������
//		IN lpCriticalSection - CRITICAL_SECTION�ṹָ��
// ����ֵ��
//		��ӵ�г�ͻ�Σ�����TRUE; ���򣬷���FALSE
// ����������
//		�����ͻ��,�����Ƿ�ӵ�У������˳�
// ���ã�
//		ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_TryEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	return DoEnterCriticalSection( lpCriticalSection, FALSE );
}


// ********************************************************************
// ������static LPVOID GetThreadSemp( LPTHREAD lpThread )
// ������
//		IN lpThread - THREAD�߳̽ṹָ��
// ����ֵ��
//		�ź�������
// ����������
//		�õ��̵߳��ź���
// ���ã�
//	
// ********************************************************************


static LPVOID GetThreadSemp( LPTHREAD lpThread )
{
//	KL_EnterCriticalSection( &csSemList );//�����뻥���
//	if( lpThread->lpsemExit == NULL )
//	    lpThread->lpsemExit = _SemaphoreCreate( NULL, 0, 1, NULL, SF_EVENT | SF_MANUALRESET );
//	KL_LeaveCriticalSection( &csSemList );//�����뻥���
	return lpThread->lpsemExit;
}

// ********************************************************************
// ������LPSEMAPHORE GetSemaphoreAndLock( HANDLE hSem )
// ������
//		IN hSem - �ź������
//		IN uObjType - ��������
// ����ֵ��
//		�ź�������ָ��
// ����������
//		�õ��ź��������Ӧ��ָ�벢����ס��
// ���ã�
//	
// ********************************************************************

#define DEBUG_GetSemaphoreAndLock 0
LPSEMAPHORE GetSemaphoreAndLock( HANDLE hSem, UINT uObjType )
{
    LPSEMAPHORE lpSem;
	UINT uiSave;

	LockIRQSave( &uiSave );

	if( uObjType )
	    lpSem = HandleToPtr( hSem, uObjType );//�ɾ���õ�����ָ��
	else
		lpSem = (LPSEMAPHORE)hSem;

	if( lpSem ) 
	{
		if( lpSem->nRefCount )
		    lpSem->nRefCount++;
		else
		{
			lpSem = NULL;  //�Ѿ����ͷţ�			
		}
	}

    UnlockIRQRestore( &uiSave );

#ifdef __DEBUG
	if( lpSem == NULL )
	{
		WARNMSG( DEBUG_GetSemaphoreAndLock, ( "GetSemaphoreAndLock been released or invalid handle!.\r\n" ) );
	}
#endif

	return lpSem;
}

// ********************************************************************
// ������LPSEMAPHORE ReleaseSemaphoreAndUnlock( HANDLE hSem, UINT uObjType )
// ������
//		IN hSem - �ź������
//		IN uObjType - ��������
// ����ֵ��
//		�ź�������ָ��
// ����������
//		�õ��ź��������Ӧ��ָ�벢����ס��
// ���ã�
//	
// ********************************************************************

#define DEBUG_ReleaseSemaphoreAndUnlock 0

VOID ReleaseSemaphoreAndUnlock( LPSEMAPHORE lpSem )
{
	UINT uiSave;

	LockIRQSave( &uiSave );

	if( lpSem ) 
	{	//2004-08-31
		//ASSERT( lpSem->objType == OBJ_SEMAPHORE );
		ASSERT( lpSem->nRefCount > 0 );
		//if( lpSem->nRefCount == 1 )
		//{
		    //ASSERT( lpSem->nRefCount );
			if( lpSem->nRefCount == 1 )
			{
				lpSem->nRefCount = 0;
				UnlockIRQRestore( &uiSave );

				_SemaphoreDelete( lpSem );
				
				WARNMSG( DEBUG_ReleaseSemaphoreAndUnlock, ( "ReleaseSemaphoreAndUnlock: not free semaphore.\r\n" ) );
				//_CloseSemaphoreObj( lpSem, 0 );
				//ASSERT( lpSem->hThis );

				//2005-01-31, delete
				//if( lpSem->hThis )
				    //KL_CloseHandle( lpSem->hThis );
				//2005-01-31
				//_CloseSemaphoreObj( lpSem, 0 );
				return;
			}
			else
			{
				//ASSERT( 0 );
				lpSem->nRefCount--;
				DEBUGMSG( DEBUG_ReleaseSemaphoreAndUnlock, ( "ReleaseSemaphoreAndUnlock: refcount(%d)!.\r\n",lpSem->nRefCount ) );
			}
		//}
		//else
			//lpSem->nRefCount--;    // >= 2
	}
	else
	{
		ERRORMSG( DEBUG_ReleaseSemaphoreAndUnlock, ( "ReleaseSemaphoreAndUnlock: not know error!.\r\n" ) );
	}


    UnlockIRQRestore( &uiSave );
}


// ********************************************************************
// ������DWORD WINAPI KL_WaitForSingleObject( HANDLE handle, DWORD dwMilliseconds )
// ������
//		IN handle - ������
//		IN dwMilliseconds - �ȴ�ʱ�䣬����ΪINFINITE��Ϊ����
// ����ֵ��
//		WAIT_OBJECT_0 - �ȴ��Ķ������ź�
//		WAIT_TIMEOUT - �ȴ��Ķ������źţ���ʱ�˳�
//		WAIT_FAILED - �����˳�
// ����������
//		�ȴ�һ������ֱ�������źŻ�ʱ
// ���ã�
//		ϵͳAPI
// ********************************************************************

#define DEBUG_KL_WaitForSingleObject 0
DWORD WINAPI KL_WaitForSingleObject( HANDLE handle, DWORD dwMilliseconds )
{
	UINT objType;
	//LPSEMAPHORE * lppSem;
	DWORD dwRetv = WAIT_FAILED;
	LPSEMAPHORE lpho;

	if( handle == CURRENT_THREAD_HANDLE || 
		handle == CURRENT_PROCESS_HANDLE  )
		goto _error;	
	//2004-08-11������ö�̬����ķ���������Ҫ��HandleThreadExit����Ӧ���ͷŴ���
    //lppSem = (LPSEMAPHORE *)KHeap_Alloc( sizeof( LPSEMAPHORE ) );
	//if( lppSem == NULL )
		//goto _error;

	objType = GetHandleObjType( handle );	//�õ���������
	if( objType == OBJ_PROCESS )
	{
		LPPROCESS lpProcess = (LPPROCESS)HandleToPtr( handle, OBJ_PROCESS );
		if( lpProcess )
		{	//������
			handle = lpProcess->lpMainThread->hThread;
			objType = OBJ_THREAD;
		}
		else
		{
			goto _error;
		}
	}
    switch( objType )
    { 
	case OBJ_EVENT:
    case OBJ_SEMAPHORE:
    case OBJ_MUTEX:
		{
			lpho = GetSemaphoreAndLock( handle, objType );//HandleToPtr( handle, objType );//�ɾ���õ�����ָ��
			if( lpho )
			{   
			   //*lppSem = lpho;	
				// ����ԭ�Ӳ���
			    dwRetv = DownSemphores( &lpho, 1, dwMilliseconds, TRUE );
				ReleaseSemaphoreAndUnlock( lpho );
			}
		}
		break;
	case OBJ_THREAD:
		{
			LPTHREAD lpThread = (LPTHREAD)HandleToPtr( handle, OBJ_THREAD );

			if( lpThread )
			{				
				lpho = GetThreadSemp( lpThread );	//�õ��̵߳��˳��źŶ���				
				lpho = GetSemaphoreAndLock( lpho, 0 );
				if( lpho )
				{
					//*lppSem = lpho;
					//	ԭ�Ӳ���
					dwRetv = DownSemphores( &lpho, 1, dwMilliseconds, TRUE );
					ReleaseSemaphoreAndUnlock( lpho );
				}
				
			}
			else
				dwRetv = WAIT_OBJECT_0;
		}
		break;
    default:
        ASSERT_NOTIFY( 0, "Invalid semaphore handle.\r\n" );
    }
	//KHeap_Free( lppSem, sizeof( LPSEMAPHORE ) );
_error:
	KL_SetLastError( ERROR_INVALID_PARAMETER );
	return dwRetv;
}

// ********************************************************************
// ������DWORD WINAPI KL_WaitForMultipleObjects(
//							 DWORD nCount, 
//							 CONST HANDLE *lpHandles, 
//							 BOOL fWaitAll, 
//							 DWORD dwMilliseconds )
// ������
//		IN nCount - �źŶ��������
//		IN lpHandles - �������źŶ���ľ������
//		IN fWaitAll - �Ƿ�ȴ����ж������źŲŷ���
//		IN dwMilliseconds - �Ƿ��ó�ʱ���ع��ܣ���ֵΪINFINITE����ʾû�г�ʱ���ܣ�
//			���򣬵��ȴ�ʱ��ﵽdwMilliseconds���궨��ֵ����û���źŲ���ʱ��
//			�ú������˳�������WAIT_TIMEOUT
// ����ֵ��
//		����WAIT_OBJECT_0 to (WAIT_OBJECT_0 + nCount �C1) ��ʾ��һ���������źţ�
//		����WAIT_TIMEOUT������˵��һ����ʱ���أ���dwMilliseconds��Чʱ�Ż᷵�ظ�ֵ
//		����WAIT_FAILED������ϵͳ����һ������
// ����������
//		�����������ȴ�һ����������ֱ���������źŻ�ʱ
// ���ã�
//	
// ********************************************************************

DWORD WINAPI KL_WaitForMultipleObjects(
							 DWORD nCount, 
							 CONST HANDLE *lpHandles, 
							 BOOL fWaitAll, 
							 DWORD dwMilliseconds )
{
    DWORD i = 0;
	DWORD dwRetv = WAIT_FAILED;
//	UINT uiSave;
	LPSEMAPHORE lppSem[MAX_WAITOBJS];  //���32��

	if( nCount > MAX_WAITOBJS )
	{   //������Ч
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return WAIT_FAILED;
	}
	//2004-08-11������ö�̬����ķ���������Ҫ��HandleThreadExit����Ӧ���ͷŴ���
	//�����ź���ָ���ڴ�
    //lppSem = (LPSEMAPHORE *)KHeap_Alloc( nCount * sizeof( LPSEMAPHORE ) );
	//if( lppSem )
	{	//ÿ���ź���ָ��ָ���Ӧ���ź���
		for( i = 0; i < nCount; i++ )
		{
	        UINT objType;

			if( lpHandles[i] == CURRENT_THREAD_HANDLE || 
		        lpHandles[i] == CURRENT_PROCESS_HANDLE  )
		        break;
			
			objType = GetHandleObjType( lpHandles[i] );

			if( objType == OBJ_EVENT ||
				objType == OBJ_SEMAPHORE ||
				objType == OBJ_MUTEX )
			{
				//lpho = GetSemaphoreAndLock( lpho, 0 );
			    //if( ( lppSem[i] = HandleToPtr( lpHandles[i], objType ) ) == NULL )
				    //break;
			    if( ( lppSem[i] = GetSemaphoreAndLock( lpHandles[i], objType ) ) == NULL )
				    break;
			}
			else if( objType == OBJ_THREAD || objType == OBJ_PROCESS )
			{
				LPTHREAD lpThread;
				HANDLE h = lpHandles[i];

				if( objType == OBJ_PROCESS )
				{
					LPPROCESS lpProcess;

					lpProcess = HandleToPtr( h, OBJ_PROCESS );
					if( lpProcess )
						h = lpProcess->lpMainThread->hThread;
					else
						h = 0;
				}

				lpThread = (LPTHREAD)HandleToPtr( h, OBJ_THREAD );
				if( lpThread )
				{
					LPSEMAPHORE lpSem;
					if( ( lpSem = GetThreadSemp( lpThread ) ) == NULL )
					{   //��Ч
					    break;
					}
					//�Ƿ���ס��
					if( (lppSem[i] = GetSemaphoreAndLock( lpSem, 0 )) == NULL )
						break;
				}
				else
				{  // thread �Ѿ�����
					if( fWaitAll == FALSE || nCount == 1 )
					{
						dwRetv = WAIT_OBJECT_0 + i;
						break;//goto _ret;//return WAIT_OBJECT_0 + i;
					}
					else
					{	//ʧ��
						break;// ��������
					}
				}
			}
			else
				break;
		}
		//
		if( i == nCount )
		{	// ���ж�����Ч������ԭ�Ӳ���
			dwRetv = DownSemphores( lppSem, nCount, dwMilliseconds, fWaitAll );
		}
		else
		{
			RETAILMSG( 1, ( "WaitForMultipleObjects error: not all wait objects are valid.\r\n" ) );
		}

		//KHeap_Free( lppSem, nCount * sizeof( LPSEMAPHORE ) );
	}

	//���
	nCount = i;
	for( i = 0; i < nCount; i++ )
	{
		ReleaseSemaphoreAndUnlock( lppSem[i] );
	}

	return dwRetv;
}


