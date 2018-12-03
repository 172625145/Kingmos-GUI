/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：信号量管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
	2004-12-09, 增加对线程退出的 CS 的复位处理 _Semaphore_HandleThreadExit
	2004-08-31, 去掉 SEMAPHORE的 objType用 hThis 代替
    2004-08-11, 不是用动态分配( WaitForSingleObject。WaitMultiSingleObject  ) 信号量，涉及到 semaphpor.c thread.c sche.c
	2004-05-18: 完成 PulseEvent
    2003-05-22: 增加对 CURRENT_PROCESS_HANDLE 和　CURRENT_THREAD_HANDLE 的处理
    2003-04-24:
	    1.增加信号量的　互斥 功能
		2.WaitForSingleObject增加对线程句柄的处理

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
// 声明：BOOL _InitSemMgr( void )
// 参数：
//		无
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		初始化信号量管理器
// 引用：
//	
// ********************************************************************
BOOL _InitSemMgr( void )
{
	

	KC_InitializeCriticalSection( &csSemList );
#ifdef __DEBUG
	csSemList.lpcsName = "CS-SEM"; // 用于调试
#endif
	return TRUE;
}

// ********************************************************************
// 声明：LPSEMAPHORE _SemaphoreCreate(
//							 LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  // must is 0
//							 int nInitialCount,                         // initial count
//							 int nMaximumCount,                     // maximum count
//							 LPCTSTR lpName,     // must is 0
//							 UINT uiFlag
//							 )
// 参数：
//	IN lpSemaphoreAttributes - 安全属性（不支持，必须为NULL）
//	IN nInitialCount - 初始化时拥有的信号量数
//	IN nMaximumCount - 最大的信号量
//	IN lpName - 信号量名（可以为NULL）
//	IN uiFlag - 标志,为：
//			SF_SEMAPHORE - 为信号量对象
//			SF_MUTEX - 互斥量对象
//			SF_EVENT - 事件对象
//			SF_OWNER - 设定拥有者线程(针对MUTEX对象)
//			SF_MANUALRESET - 手工重设（针对EVENT对象）
// 返回值：
//	假如成功，返回有效的SEMAPHORE结构指针；否则，返回NULL
// 功能描述：
//	创造信号量对象
// 引用：
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
    {    //命名对象，先查找当前是否存在该名
        for( lpsem = lpSemList; lpsem; lpsem = lpsem->lpNext )
        {
            if( lpsem->lpszName && stricmp( lpName, lpsem->lpszName ) == 0 )
            {   // 找到相同的 ，find same name
				KL_SetLastError( ERROR_ALREADY_EXISTS );
				if( (WORD)(uiFlag & 0xf) == (lpsem->semFlag & 0xf) )
				{   // 同样的对象
					lpsem->nRefCount++;	//增加引用
					goto _ret;
				}
				else
				{   // 同名但不同的对象，diff object but same name
					lpsem = NULL; // error
					goto _ret;
				}
            }
        }
    }
	// 没有找到
    iStrLen = 0;
    if( lpName )
    {
        iStrLen = strlen( lpName );
    }
	// 分配结构
    lpsem = (LPSEMAPHORE)KHeap_Alloc( sizeof(SEMAPHORE) );
	if( lpsem )
    {	// 初始化
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
		//	加入链表头
        lpsem->lpNext = lpSemList;
        lpSemList = lpsem;
    }
_ret:
    return lpsem;
}

// ********************************************************************
// 声明：BOOL _SemaphoreRemove( LPSEMAPHORE lpsem )
// 参数：
//		IN lpsem -  SEMAPHORE结构指针
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		该功能将信号量移出链表
// 引用：
//	
// ********************************************************************

static BOOL _SemaphoreRemove( LPSEMAPHORE lpsem )
{
	//KL_EnterCriticalSection( &csSemList );
	//if( --lpsem->nRefCount == 0 )	// 引用计数 = 0 ?
	{	// 是，没有任何引用 ，释放她 
		LPSEMAPHORE lp = lpSemList;

//		ASSERT( lpSemList );
		//	从链表移出
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
		// 释放
		//if( lpsem->lpszName )
		  //  KHeap_FreeString( lpsem->lpszName );
        //KHeap_Free( lpsem, sizeof(SEMAPHORE) );
	}
	//KL_LeaveCriticalSection( &csSemList );
	return TRUE;
}

// ********************************************************************
// 声明：BOOL _SemaphoreDeletePtr( LPSEMAPHORE lpsem )
// 参数：
//		IN lpsem -  SEMAPHORE结构指针
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		删除信号量内存
// 引用：
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
// 声明：BOOL _SemaphoreDelete( LPSEMAPHORE lpsem )
// 参数：
//		IN lpsem -  SEMAPHORE结构指针
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		与 _SemaphoreCreate 对应，该功能释放所分配的资源
// 引用：
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
	{// 是，没有任何引用 ，先移出，再释放
		if( _SemaphoreRemove( lpsem ) )
		{	//释放
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
// 声明：BOOL _SemaphoreRelease( 
//                            LPSEMAPHORE lpsem, 
//							  int lReleaseCount, 
//							  LPINT lpPreviousCount )

// 参数：
//	IN lpsem - SEMAPHORE 结构指针
//	IN lReleaseCount - 释放的资源数
//	OUT lpPreviousCount - 用于接受之前的资源数 
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	释放信号量。假如当前有等待信号量的线程，也释放它们
// 引用：
//	
// ********************************************************************

BOOL _SemaphoreRelease( 
					   LPSEMAPHORE lpsem, 
					   int lReleaseCount, 
					   LPINT lpPreviousCount )
{
    //ASSERT( lpsem->objType == OBJ_SEMAPHORE ); //2004-08-31
	// 保存当前的
	if( lpPreviousCount )
		*lpPreviousCount = lpsem->nCount;
	// 释放信号量 & 线程
	if( lReleaseCount )
	    UpSemaphore( lpsem, lReleaseCount );
	return TRUE;
}


// ********************************************************************
// 声明：VOID _Semaphore_HandleThreadExit( 
//                            LPVOID lpOwnerID  )
// 参数：
//	IN lpOwnerID - 拥有者ID
// 返回值：
//	无
// 功能描述：
//	当线程退出而没有释放sem时，这里做最后的复位
// 引用：
//	
// ********************************************************************

VOID _Semaphore_HandleThreadExit( LPVOID lpOwnerID )
{
	LPSEMAPHORE lpsem;

	KL_EnterCriticalSection( &csSemList );//　进入互斥段

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


	KL_LeaveCriticalSection( &csSemList );//　进入互斥段
}

// ********************************************************************
// 声明：static BOOL _SemaphoreReset( LPSEMAPHORE lpsem )
// 参数：
//	IN lpsem - SEMAPHORE 结构指针
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	复位信号量到无信号状态
// 引用：
//	
// ********************************************************************

static BOOL _SemaphoreReset( LPSEMAPHORE lpsem )
{
	UINT uiSave;
	LockIRQSave( &uiSave );

	//INTR_OFF(); // 关闭中断
	lpsem->nCount = 0;
	//INTR_ON();  // 打开中断

	UnlockIRQRestore( &uiSave );

	return TRUE;
}


// ********************************************************************
// 声明：static HANDLE _CreateHandleObj(
//                   LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  // must is NULL
//                   int nInitialCount,                         // initial count
//                   int nMaximumCount,                     // maximum count
//                   LPCTSTR lpName,     // must is NULL
//                   UINT uiFlag,
//				     DWORD objType,
//				     LPPROCESS lpOwnerProcess
//                   )
// 参数：
//	IN lpSemaphoreAttributes - 安全属性（不支持，必须为NULL)
//	IN nInitialCount - 初始化信号量计数
//	IN nMaximumCount - 最大的信号量数
//	IN lpName - 已字符串表示的对象名
//	IN uiFlag - 标志，为：
//					SF_SEMAPHORE　－ 信号量对象
//					SF_MUTEX - 互斥量对象
//					SF_EVENT - 事件对象
//					SF_OWNER - 设定拥有者线程(针对MUTEX对象)
//					SF_MANUALRESET - 手工重设（针对EVENT对象）
//	IN objType - 对象类型，传递给Hanle_Alloc的参数，这里为：
//						OBJ_SEMAPHORE
//						OBJ_MUTEX - 互斥量对象
//						OBJ_EVENT - 事件对象
//	IN lpOwnerProcess - 该对象的拥有者
// 返回值：
//	假如成功，返回句柄；否则，返回NULL
// 功能描述：
//	创建一个基于句柄的对象
// 引用：
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

	KL_EnterCriticalSection( &csSemList );//　进入互斥段
	// 创建内存对象
	lpvData = _SemaphoreCreate( lpSemaphoreAttributes, nInitialCount, nMaximumCount, lpName, uiFlag );

	if( lpvData ) // 成功吗　？
	{	//　成功，为该对象分配一个句柄
		h = Handle_Alloc( lpProcess, lpvData, objType );
		if( h == NULL )  // 句柄分配成功吗？
		{	// 否，删除之前分配的内存对象
			lpvData->nRefCount = 0;
			_SemaphoreDelete( lpvData );
		}
		else
		{   //2005-01-31, delete
			;//lpvData->hThis = h;
		}
	}
	KL_LeaveCriticalSection( &csSemList );// 离开互斥段
	return h;


}

//释放信号量（原子操作）
BOOL _CloseSemaphoreObj( HANDLE hSemaphore, UINT uObjType )
{
	BOOL retv = FALSE;
	LPSEMAPHORE lpho;

	KL_EnterCriticalSection( &csSemList );
	
	if( uObjType )
		lpho = HandleToPtr( hSemaphore, uObjType );// 由句柄得到对象指针
	else
		lpho = (LPSEMAPHORE)hSemaphore;  //指针对象
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
// 声明：HANDLE WINAPI KL_CreateSemaphore(
//                   LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  //
//                   int nInitialCount,                         // initial count
//                   int nMaximumCount,                     // maximum count
//                   LPCTSTR lpName     //
//                   )

// 参数：
//	IN lpSemaphoreAttributes-安全性描述,为NULL
//	IN nInitialCount-初始计数
//	IN nMaximumCount-最大计数
//	IN lpName-以字符串表示的对象名

// 返回值：
//	成功：返回信号量句柄
//	否则：返回NULL
// 功能描述：
//	创建一个信号量	
// 引用：
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
// 声明：BOOL WINAPI KL_ReleaseSemaphore(
//					   HANDLE hSemaphore,
//					   int lReleaseCount,       //
//					   LPINT lpPreviousCount   // 
//					   )

// 参数：
//	IN hSemaphore-信号量句柄
//	IN lReleaseCount-释放计数
//	OUT lpPreviousCount-用于接受前次信号量计数

// 返回值：
//	成功：返回非零
//	否则：返回0
	
// 功能描述：
//	释放信号，假如有线程在等待该信号量则释放这些线程
// 引用：
//	系统API
// ********************************************************************

BOOL WINAPI KL_ReleaseSemaphore(
					   HANDLE hSemaphore,
					   int lReleaseCount,       //
					   LPINT lpPreviousCount   //
					   )
{
    LPSEMAPHORE lpho = HandleToPtr( hSemaphore, OBJ_SEMAPHORE ); // 由句柄得到对象指针
	
	if( lpPreviousCount )
		*lpPreviousCount = 0;

    if( lpho )
    {
		return _SemaphoreRelease( lpho, lReleaseCount, lpPreviousCount );
    }
    return FALSE;
}

// ********************************************************************
// 声明：BOOL FASTCALL Semaphore_Close( HANDLE hSemaphore ) 
// 参数：
//	IN hSemaphore - 信号量句柄
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	删除信号量句柄及对象
// 引用：
//	被CloseHandle调用
// ********************************************************************

BOOL FASTCALL Semaphore_Close( HANDLE hSemaphore ) 
{
	return _CloseSemaphoreObj( hSemaphore, OBJ_SEMAPHORE );
}

// ********************************************************************
// 声明：HANDLE WINAPI KL_CreateMutex(
//					      LPSECURITY_ATTRIBUTES lpMutexAttributes,    // must is 0
//					      BOOL bInitialOwner,                     // flag for initial ownership
//					      LPCTSTR lpName                      // must is 0
//						)
// 参数：
//	IN lpMutexAttributes - 安全属性（必须为NULL）
//	IN bInitialOwner - 是否创建时就拥有该互斥量
//	IN lpName - 互斥量名（可以为NULL）
// 返回值：
//	假如成功，返回互斥量句柄; 否则返回NULL
// 功能描述：
//		创建互斥量, 如果lpName不为NULL，则创建一个命名互斥量，其它进程可以用同名调用该函数得到对该互斥量的跨进程引用
//	如果是命名互斥量并且多个进程会用到它，应该使bInitialOwner为FALSE。拥有互斥量的线程可以多次调用等待功能而不会
//	被锁住，这时，你应该调用同样多次释放功能（ReleaseMutex）。
//		互斥量只能被一个线程所拥有
//		用CloseHandle去删除互斥量
// 引用：
//	系统API
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
// 声明：BOOL WINAPI KL_ReleaseMutex( HANDLE hMutex )
// 参数：
//		IN hMutex - 互斥量句柄
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		释放对互斥量的拥有权并使互斥量对象有信号。只有拥有该互斥量的线程才能释放它
// 引用：
//		系统API
// ********************************************************************

BOOL WINAPI KL_ReleaseMutex( HANDLE hMutex )
{
    LPSEMAPHORE lpho = HandleToPtr( hMutex, OBJ_MUTEX );	//由句柄得到对象指针
    if( lpho )	//有效吗？
	{	//是
		if( lpho->lpOwner == lpCurThread )		//是否当前线程拥有该互斥量
		{	//是
			if( lpho->nLockCount == 1 )		//当前线程重复等待该互斥量的次数是否为 1
		        return _SemaphoreRelease( lpho, 1, 0 );  // 是，释放它
			else	
			{
				lpho->nLockCount--;	//否，仅仅减去重复计数
			    return TRUE;
			}
		}
	}
	return FALSE;
}



// ********************************************************************
// 声明：HANDLE WINAPI KL_CreateEvent(
//				LPSECURITY_ATTRIBUTES lpEventAttributes,    // must is NULL
//  			BOOL bManualReset, 
//				BOOL bInitialState, 
//				LPTSTR lpName )
// 参数：
//		IN lpEventAttributes - 安全属性
//		IN bManualReset - 是否手工重设信号
//		IN bInitialState - 事件的状态,假如为TRUE,事件创建后为有信号状态
//		IN lpName - 事件名（可以为NULL）
// 返回值：
//		假如成功，返回有效的句柄值；否则，返回NULL
// 功能描述：
//	创建事件对象。假如lpName不为NULL，则创建一个其它进程可以访问的命名事件。
//	如果为命名事件且之前已经创建了该事件，则忽略bManualReset和bInitialState
//	如果bManualReset为FALSE,则当事件有信号并且有线程等待该事件，则系统释放等
//	待线程并且自动将该事件重新设为无信号状态
// 引用：
//		系统API
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
	    LPVOID lpho = HandleToPtr( h, OBJ_EVENT );//由句柄得到对象指针
        _SemaphoreRelease( (LPSEMAPHORE)lpho, 1, NULL );
	}
	return h;
}

// ********************************************************************
// 声明：BOOL WINAPI KL_ResetEvent( HANDLE hEvent )
// 参数：
//		IN hEvent - 事件句柄
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE	
// 功能描述：
//		将事件对象设为无信号状态
// 引用：
//		系统API
// ********************************************************************

BOOL WINAPI KL_ResetEvent( HANDLE hEvent )
{
    LPVOID lpho = HandleToPtr( hEvent, OBJ_EVENT );	//由句柄得到对象指针
    if( lpho )//
	{	// 有效的指针
		return _SemaphoreReset( (LPSEMAPHORE)lpho );//
	}

	return FALSE;        
}

// ********************************************************************
// 声明：BOOL WINAPI KL_SetEvent( HANDLE hEvent )
// 参数：
//		IN hEvent - 事件对象句柄
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		设置事件为信号状态，如果有等待的线程，则释放它们。
//		线程通过等待功能检查事件信号，当线程得到信号后，对信号的处理为下：
//		当该事件为手工重设功能，该事件的信号不会改变直到调用了ResetEvent，
//		当该事件为非手工重设功能，该信号重设为无信号。
//		仅仅只有一个等待的线程能得到信号
// 引用：
//		系统API
// ********************************************************************
BOOL WINAPI KL_SetEvent( HANDLE hEvent )
{
    LPVOID lpho = HandleToPtr( hEvent, OBJ_EVENT );//由句柄得到对象指针
    if( lpho )
	{
		//(LPSEMAPHORE)lpho->nPulseCount = 0;
		return _SemaphoreRelease( (LPSEMAPHORE)lpho, 1, NULL );
	}
	return FALSE;
}

// ********************************************************************
// 声明：BOOL WINAPI KL_PulseEvent( HANDLE hEvent )
// 参数：
//		IN hEvent - 事件对象句柄
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		设置事件为信号状态，如果有等待的线程，则释放它们。
//		线程通过等待功能检查事件信号，当线程得到信号后，对信号的处理为下：
//		当该事件为手工重设功能，该信号重设为无信号，
//		当该事件为非手工重设功能，该信号重设为无信号。
//		当该事件为手工重设功能，当前等待的所有线程能得到信号并被释放
//		当该事件为非手工重设功能，仅仅一个线程能得到信号
// 引用：
//	
// ********************************************************************

BOOL WINAPI KL_PulseEvent( HANDLE hEvent )
{
    LPSEMAPHORE lpho = (LPSEMAPHORE)HandleToPtr( hEvent, OBJ_EVENT );//由句柄得到对象指针
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
// 声明：BOOL FASTCALL Event_Close( HANDLE hEvent )
// 参数：
//		IN hEvent - 事件对象句柄
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		当系统调用CloseHandle时，如果句柄对象为事件对象，CloseHandle则会调用该函数
// 引用：
//		hmgr.c
// ********************************************************************

BOOL FASTCALL Event_Close( HANDLE hEvent ) 
{
	return _CloseSemaphoreObj( hEvent, OBJ_EVENT );
}

// ********************************************************************
// 声明：BOOL FASTCALL Mutex_Close( HANDLE hMutex )
// 参数：
//		IN hEvent - 事件对象句柄
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		当系统调用CloseHandle时，如果句柄对象为Mutex对象，CloseHandle则会调用该函数
// 引用：
//		hmgr.c
// ********************************************************************

BOOL FASTCALL Mutex_Close( HANDLE hMutex )
{
	return _CloseSemaphoreObj( hMutex, OBJ_MUTEX );
}

// ********************************************************************
// 声明：VOID WINAPI KL_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// 参数：
//		IN lpCriticalSection - CRITICAL_SECTION结构指针
// 返回值：
//		无
// 功能描述：
//		创建并初始化一个冲突段对象
// 引用：
//		系统API
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
// 声明：VOID FASTCALL KC_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// 参数：
//		IN lpCriticalSection - CRITICAL_SECTION结构指针
// 返回值：
//		无
// 功能描述：
//		创建并初始化一个冲突段对象
// 引用：
//		内部调用
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
// 声明：VOID WINAPI KL_DeleteCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// 参数：
//		IN lpCriticalSection - CRITICAL_SECTION结构指针
// 返回值：
//		无
// 功能描述：
//		与KL_InitializeCriticalSection相反，该功能删除冲突段对象
// 引用：
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
// 声明：VOID WINAPI KL_LeaveCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// 参数：
//		IN lpCriticalSection - CRITICAL_SECTION结构指针
// 返回值：
//		无
// 功能描述：
//		离开冲突段
// 引用：
//		系统API
// ********************************************************************

VOID WINAPI KL_LeaveCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	if( lpCriticalSection )
	{
		if( lpCriticalSection->dwOwnerThreadId == lpCurThread->dwThreadId )	//当前线程拥有该冲突段吗？
		{	//是
#ifdef __DEBUG
			if( lpCriticalSection->uiFlag & CSF_DEBUG )		//是否输出调试信息
			{	//是
				RETAILMSG( 1, ( "LeaveCriticalSection:lpcs=0x%x,lpcsName=%s,iLockCount=%d.\r\n", lpCriticalSection, lpCriticalSection->lpcsName, lpCriticalSection->iLockCount ) );
			}
#endif
			
			if( lpCriticalSection->iLockCount != 1 )  // 最后一次离开吗 ？
				lpCriticalSection->iLockCount--;  // 不是，简单的将进入数减一（同一个线程能多次进入冲突段）
			else
			{	// 是最后一次离开
				LPSEMAPHORE lpho;
				
#ifdef DEBUG_CRITSEC
				if( lpCriticalSection->lpcsName )
				{
					DWORD dwCurThreadId = KL_GetCurrentThreadId();
					RETAILMSG( 1, ( "upcs=%s, cid=0x%x, oid=0x%x, count=%d\r\n", lpCriticalSection->lpcsName, dwCurThreadId, lpCriticalSection->dwOwnerThreadId, lpCriticalSection->iLockCount ) );
				}
#endif //DEBUG_CRITSEC
				//	
				lpCriticalSection->dwOwnerThreadId |= 1;  // 阻止所有的线程操作该对象 mask all threads with the crit
				if( lpCriticalSection->uiFlag & CSF_KERNEL )	//是否是内核自己的创建的
					lpho = (LPSEMAPHORE)lpCriticalSection->hSemaphore;  // 是
				else
					lpho = HandleToPtr( lpCriticalSection->hSemaphore, -1 );	// 用户对象	
				if( lpho )
					UpCrit( lpho, lpCriticalSection );	//进入原子操作
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
// 声明：DoEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection, BOOL bEntryBlock )
// 参数：
//		IN lpCriticalSection - CRITICAL_SECTION结构指针
//		IN bEntryBlock - 当不拥有冲突段时，是否退出
// 返回值：
//		当拥有冲突段，返回TRUE; 否则，返回FALSE
// 功能描述：
//		进入冲突段,不管是否拥有，都回退出
// 引用：
//		系统API
// ********************************************************************

static BOOL DoEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection, BOOL bEntryBlock )
{
	BOOL bRetv = FALSE;
	if( lpCriticalSection )
	{
		if( lpCriticalSection->dwOwnerThreadId == lpCurThread->dwThreadId )		//当前线程拥有该冲突段吗？
		{	//是，简单的将计数器加一
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
		{	// 不拥有，否
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
			
			if( lpCriticalSection->uiFlag & CSF_KERNEL )//是否是内核自己的创建的
				lpho = (LPSEMAPHORE)lpCriticalSection->hSemaphore; //是
			else
				lpho = (LPSEMAPHORE)HandleToPtr( lpCriticalSection->hSemaphore, -1 ); // 否，用户对象
			lpho = GetSemaphoreAndLock( lpho, 0 );
			if( lpho )
			{			
				bRetv = DownCrit( lpho, lpCriticalSection, bEntryBlock );	//进入原子操作
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
// 声明：VOID WINAPI KL_EnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// 参数：
//		IN lpCriticalSection - CRITICAL_SECTION结构指针
// 返回值：
//		无
// 功能描述：
//		进入冲突段
// 引用：
//		系统API
// ********************************************************************

VOID WINAPI KL_EnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	DoEnterCriticalSection( lpCriticalSection, TRUE );
}

// ********************************************************************
// 声明：BOOL WINAPI KL_TryEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
// 参数：
//		IN lpCriticalSection - CRITICAL_SECTION结构指针
// 返回值：
//		当拥有冲突段，返回TRUE; 否则，返回FALSE
// 功能描述：
//		进入冲突段,不管是否拥有，都回退出
// 引用：
//		系统API
// ********************************************************************

BOOL WINAPI KL_TryEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
	return DoEnterCriticalSection( lpCriticalSection, FALSE );
}


// ********************************************************************
// 声明：static LPVOID GetThreadSemp( LPTHREAD lpThread )
// 参数：
//		IN lpThread - THREAD线程结构指针
// 返回值：
//		信号量对象
// 功能描述：
//		得到线程的信号量
// 引用：
//	
// ********************************************************************


static LPVOID GetThreadSemp( LPTHREAD lpThread )
{
//	KL_EnterCriticalSection( &csSemList );//　进入互斥段
//	if( lpThread->lpsemExit == NULL )
//	    lpThread->lpsemExit = _SemaphoreCreate( NULL, 0, 1, NULL, SF_EVENT | SF_MANUALRESET );
//	KL_LeaveCriticalSection( &csSemList );//　进入互斥段
	return lpThread->lpsemExit;
}

// ********************************************************************
// 声明：LPSEMAPHORE GetSemaphoreAndLock( HANDLE hSem )
// 参数：
//		IN hSem - 信号量句柄
//		IN uObjType - 对象类型
// 返回值：
//		信号量对象指针
// 功能描述：
//		得到信号量句柄对应的指针并且锁住它
// 引用：
//	
// ********************************************************************

#define DEBUG_GetSemaphoreAndLock 0
LPSEMAPHORE GetSemaphoreAndLock( HANDLE hSem, UINT uObjType )
{
    LPSEMAPHORE lpSem;
	UINT uiSave;

	LockIRQSave( &uiSave );

	if( uObjType )
	    lpSem = HandleToPtr( hSem, uObjType );//由句柄得到对象指针
	else
		lpSem = (LPSEMAPHORE)hSem;

	if( lpSem ) 
	{
		if( lpSem->nRefCount )
		    lpSem->nRefCount++;
		else
		{
			lpSem = NULL;  //已经被释放！			
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
// 声明：LPSEMAPHORE ReleaseSemaphoreAndUnlock( HANDLE hSem, UINT uObjType )
// 参数：
//		IN hSem - 信号量句柄
//		IN uObjType - 对象类型
// 返回值：
//		信号量对象指针
// 功能描述：
//		得到信号量句柄对应的指针并且锁住它
// 引用：
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
// 声明：DWORD WINAPI KL_WaitForSingleObject( HANDLE handle, DWORD dwMilliseconds )
// 参数：
//		IN handle - 对象句柄
//		IN dwMilliseconds - 等待时间，假如为INFINITE则为死等
// 返回值：
//		WAIT_OBJECT_0 - 等待的对象有信号
//		WAIT_TIMEOUT - 等待的对象无信号，超时退出
//		WAIT_FAILED - 错误退出
// 功能描述：
//		等待一个对象直到其有信号或超时
// 引用：
//		系统API
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
	//2004-08-11，如果用动态分配的方法，则需要在HandleThreadExit做对应的释放处理
    //lppSem = (LPSEMAPHORE *)KHeap_Alloc( sizeof( LPSEMAPHORE ) );
	//if( lppSem == NULL )
		//goto _error;

	objType = GetHandleObjType( handle );	//得到对象类型
	if( objType == OBJ_PROCESS )
	{
		LPPROCESS lpProcess = (LPPROCESS)HandleToPtr( handle, OBJ_PROCESS );
		if( lpProcess )
		{	//主进程
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
			lpho = GetSemaphoreAndLock( handle, objType );//HandleToPtr( handle, objType );//由句柄得到对象指针
			if( lpho )
			{   
			   //*lppSem = lpho;	
				// 进入原子操作
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
				lpho = GetThreadSemp( lpThread );	//得到线程的退出信号对象				
				lpho = GetSemaphoreAndLock( lpho, 0 );
				if( lpho )
				{
					//*lppSem = lpho;
					//	原子操作
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
// 声明：DWORD WINAPI KL_WaitForMultipleObjects(
//							 DWORD nCount, 
//							 CONST HANDLE *lpHandles, 
//							 BOOL fWaitAll, 
//							 DWORD dwMilliseconds )
// 参数：
//		IN nCount - 信号对象的数量
//		IN lpHandles - 保存有信号对象的句柄数组
//		IN fWaitAll - 是否等待所有对象有信号才返回
//		IN dwMilliseconds - 是否用超时返回功能，当值为INFINITE，表示没有超时功能；
//			否则，当等待时间达到dwMilliseconds所标定的值但仍没有信号产生时，
//			该函数将退出并返回WAIT_TIMEOUT
// 返回值：
//		返回WAIT_OBJECT_0 to (WAIT_OBJECT_0 + nCount –1) 标示那一个对象有信号；
//		返回WAIT_TIMEOUT　－　说明一个超时返回，当dwMilliseconds有效时才会返回该值
//		返回WAIT_FAILED　－　系统产生一个错误
// 功能描述：
//		根据条件，等待一个或多个对象直到它们有信号或超时
// 引用：
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
	LPSEMAPHORE lppSem[MAX_WAITOBJS];  //最大32个

	if( nCount > MAX_WAITOBJS )
	{   //参数无效
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return WAIT_FAILED;
	}
	//2004-08-11，如果用动态分配的方法，则需要在HandleThreadExit做对应的释放处理
	//分配信号量指针内存
    //lppSem = (LPSEMAPHORE *)KHeap_Alloc( nCount * sizeof( LPSEMAPHORE ) );
	//if( lppSem )
	{	//每个信号量指针指向对应的信号量
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
					{   //无效
					    break;
					}
					//是否锁住？
					if( (lppSem[i] = GetSemaphoreAndLock( lpSem, 0 )) == NULL )
						break;
				}
				else
				{  // thread 已经死掉
					if( fWaitAll == FALSE || nCount == 1 )
					{
						dwRetv = WAIT_OBJECT_0 + i;
						break;//goto _ret;//return WAIT_OBJECT_0 + i;
					}
					else
					{	//失败
						break;// 将来增加
					}
				}
			}
			else
				break;
		}
		//
		if( i == nCount )
		{	// 所有对象有效，进入原子操作
			dwRetv = DownSemphores( lppSem, nCount, dwMilliseconds, fWaitAll );
		}
		else
		{
			RETAILMSG( 1, ( "WaitForMultipleObjects error: not all wait objects are valid.\r\n" ) );
		}

		//KHeap_Free( lppSem, nCount * sizeof( LPSEMAPHORE ) );
	}

	//清除
	nCount = i;
	for( i = 0; i < nCount; i++ )
	{
		ReleaseSemaphoreAndUnlock( lppSem[i] );
	}

	return dwRetv;
}


