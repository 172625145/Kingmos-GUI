/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：down, up
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
    2003-06-11: LN,将INTR_ON, INTR_OFF 改为 LockIRQSave UnlockIRQRestore
******************************************************/

#include <eframe.h>
#include <epcore.h>
#include <coresrv.h>
#include <epalloc.h>
#include <oemfunc.h>

extern DWORD __Down(LPSEMAPHORE lpsem, DWORD dwTimeout);
extern void __Up(LPSEMAPHORE lpsem);
extern DWORD __DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwTimeout );
//extern DWORD OEM_TimeToJiffies( DWORD dwMilliseconds, DWORD dwNanoseconds );

/**************************************************
声明：void UpSemaphore( LPSEMAPHORE lpsem, int iCount )
参数：
	IN lpsem - 信号量对象
	IN iCount - 释放的信号量
返回值：
	无
功能描述：
	释放一定的信号量
引用: 
	被semaphore.c的相关功能调用
************************************************/
void UpSemaphore( LPSEMAPHORE lpsem, int iCount )
{
	UINT uiSave;

    LockIRQSave( &uiSave );
	
    // 锁住调度器   
	//LOCK_SCHE();   //中断将会调用 UpSemaphore，所以不能用该函数

	// 检查iCount的合法性	
	if( lpsem->nCount + iCount <= lpsem->nMaxCount )
	{   // 该信号量是否需要设定拥有者
		if( lpsem->semFlag & SF_OWNER )
		{   // 是，必是Mutex, 将其拥有者和计数器设为0
			lpsem->nLockCount = 0;
			lpsem->lpOwner = NULL;
		}
        // 增加可用信号量
		lpsem->nCount += iCount;
		// 是否有等待的线程
		if( lpsem->nWaiting )
		{   // 是，释放所有的等待的线程
			lpsem->nWaiting = 0;
			
        	UnlockIRQRestore( &uiSave );
        //    UNLOCK_SCHE();			//中断将会调用 UpSemaphore，所以不能用该函数
			__Up( lpsem );
		}
		else
			UnlockIRQRestore( &uiSave );
	}
	else
	{
		UnlockIRQRestore( &uiSave );
		ASSERT( !(lpsem->nCount && lpsem->nWaiting) );        
	}	

    // 解锁调度器   
	//UNLOCK_SCHE();
}

/**************************************************
声明：void UpCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs )
参数：
	IN lpsem - 信号量对象
	IN lpcs - 互斥段指针
返回值：
	无
功能描述：
	释放互斥段，如果有等待线程，释放它们
引用: 
	被semaphore.c的KL_LeaveCritialSection相关功能调用
************************************************/

void UpCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs )
{
	UINT uiSave;

    // 锁住调度器   
	//LOCK_SCHE();
	
	ASSERT( lpsem->nCount == 0 );
	ASSERT( lpsem->lpOwner == lpCurThread );

    LockIRQSave( &uiSave );	
	
    //
	lpsem->nCount = 1;
	lpcs->dwOwnerThreadId = -1;
	lpcs->iLockCount = 0;
	//	
	lpsem->lpOwner = NULL;
	if( lpCurThread->nCurPriority != lpCurThread->nOriginPriority )
	{   // 恢复原始优先级
	    //RETAILMSG( 1, ( "restore pri from(%d) to (%d).\r\n", lpCurThread->nCurPriority, lpCurThread->nOriginPriority ) );
		//ASSERT( lpCurThread != lpInitKernelThread );
		SetThreadCurrentPriority( lpCurThread, lpCurThread->nOriginPriority );
	}

	// 是否有等待的线程
	if( lpsem->nWaiting )
	{   // 是，释放它们，增加得到该资源的线程在同级优先级的爆发值（优先调度权）
		lpsem->nBoost = lpsem->nWaiting - 1;
		lpsem->nWaiting = 0;

		UnlockIRQRestore( &uiSave );//2003-06-11, ADD
		__Up( lpsem );
	}
	else
	{   // 没有任何线程等待该 CS
		lpsem->nHighPriority = IDLE_PRIORITY;//
		UnlockIRQRestore( &uiSave );//2003-06-11, ADD
	}
    // 解锁调度器   
	//UNLOCK_SCHE();
}


/**************************************************
声明：BOOL DownCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs, BOOL bEntryBlock )
参数：
	IN lpsem - 信号量对象
	IN lpcs - 互斥段指针
	IN bEntryBlock - 如果没有拥有,是否直接退出
返回值：
	TRUE/FALSE
功能描述：
	进入互斥段，如果不能得到资源，则进入等待队列
引用: 
	被semaphore.c的KL_EntryCritialSection相关功能调用
************************************************/

BOOL DownCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs, BOOL bEntryBlock )
{
	UINT uiSave;


    // 锁住调度器   

//	LOCK_SCHE();
	
__CHECK_COUNT:
	LockIRQSave( &uiSave );  //2003-06-11, ADD
    // 是否能够得到该资源
	if( lpsem->nCount > 0 )
	{   // 该资源可用，now own the cs
		ASSERT( lpsem->nCount == 1 );
		
		lpsem->nCount = 0;
		// 将该资源的拥有者设为当前线程
		lpsem->lpOwner = lpCurThread;
		// 将该资源的拥有者ID设为当前线程ID
		lpcs->dwOwnerThreadId = lpCurThread->dwThreadId;
		lpcs->iLockCount = 1;
		
		//是否继承优先级
		if( lpCurThread->nCurPriority > lpsem->nHighPriority )
		{   // 是，继承优先级
			//RETAILMSG( 1, ( "high current owner.\r\n" ) );
			//ASSERT( lpCurThread != lpInitKernelThread );
			SetThreadCurrentPriority( lpCurThread, lpsem->nHighPriority );
		}
		else
		{  // 否，增加在同级优先级内的爆发值
		    lpCurThread->nBoost += lpsem->nBoost;
		}
		UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
	}
	else if( bEntryBlock )
	{   //该资源不可用 
		// 2004-12-09, 判断该拥有者是否有效
	//	if( !_GetThreadPtr( lpcs->dwOwnerThreadId ) )
	//	{	//无效，可能已经没有释放就退出或
	//	}
		//

		//是否提升拥有者的优先级
		if( lpsem->lpOwner->nCurPriority > lpCurThread->nCurPriority )
		{   //是
			//RETAILMSG( 1, ( "high owner: orig_pri(%d) to new_pri(%d),Owner state(0x%x).\r\n", lpsem->lpOwner->nCurPriority, lpCurThread->nCurPriority, lpsem->lpOwner->dwState ) );
			//提升拥有者的优先级
			lpsem->nHighPriority = lpCurThread->nCurPriority;
			ASSERT( lpsem->lpOwner != lpInitKernelThread );
			SetThreadCurrentPriority( lpsem->lpOwner, lpsem->nHighPriority );
		}
		UnlockIRQRestore( &uiSave ); //2003-06-11, ADD

#ifdef __DEBUG
		KL_TlsSetValue( TLS_CRITICAL_SECTION, lpcs );
#endif
        // 进入等待状态
		__DownSemphores( &lpsem, 1, INFINITE );//dwExpireJiffies, TRUE );

#ifdef __DEBUG
		KL_TlsSetValue( TLS_CRITICAL_SECTION, 0 );
#endif
		// 等待返回，重新去检查是否能够得到资源
		goto __CHECK_COUNT;
	}
	else
		return FALSE;
	//
    // 解锁调度器   
	//UNLOCK_SCHE();
	
	return TRUE;
}

#ifdef __DEBUG
void CriticalSection_Check( void )
{
	LPTHREAD lpThread;
	//UINT uiSave;

	//INTR_OFF();//2003-06-11, DEL
    //LockIRQSave( &uiSave );
	LOCK_SCHE();
    for ( lpThread = lpInitKernelThread ; (lpThread = lpThread->lpNextThread) != lpInitKernelThread; )
	{
		LPCRITICAL_SECTION lpcs;
		if( lpThread->dwState == THREAD_UNINTERRUPTIBLE &&
			( lpcs = (LPCRITICAL_SECTION)(lpThread->lpdwTLS[TLS_CRITICAL_SECTION]) ) )
		{
			EdbgOutputDebugString( "lpcs=0x%x.\r\n", lpcs );
		}
	}
	//INTR_ON();//2003-06-11, DEL
	//UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
	UNLOCK_SCHE();
}
#endif


/**************************************************
声明：DWORD DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwMilliseconds, BOOL bWaitAll )
参数：
	IN lpsem - 信号量对象数组
	IN nCount - 保存在信号量数组里的对象数
	IN dwMilliseconds - 假如需要等待，等待时间
	IN bWaitAll - 是否等待所有对象有信号 
返回值：
	WAIT_OBJECT_0 + n 或 WAIT_TIMEOUT 或 WAIT_FAILED
功能描述：
	判断是否能够得到资源，如果不能得到资源，则进入等待队列
引用: 
	被semaphore.c相关功能调用
************************************************/

DWORD DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwMilliseconds, BOOL bWaitAll )
{
	DWORD dwExpireJiffies;
	DWORD i;
	LPSEMAPHORE * lpp;
	BOOL bGetIt = FALSE;
	UINT uiSave;

	// 计算需要等待时间所对应的时间片
	if( dwMilliseconds != INFINITE )
	    dwExpireJiffies = OEM_TimeToJiffies( dwMilliseconds, 0 );
	else
		dwExpireJiffies = INFINITE;	
    
    // 锁住调度器   
    //LOCK_SCHE();  //中断将会调用 UpSemaphore，所以不能用该函数
    //lpCurThread->nsemRet = 0;
    LockIRQSave( &uiSave );

	lpCurThread->flag &= ~FLAG_PULSE_EVENTING;

__CHECK_COUNT:

	lpp = lppsem;

	if( bWaitAll )
	{  // 判断所有对象是否有信号
		for( i = 0; i < nCount; i++, lpp++ )
		{
			if( (*lpp)->nCount <= 0 )
			{   // 假如该对象没有信号，检查是否是Mutex
				if( (*lpp)->semFlag & SF_OWNER )
				{   // 是Mutex，判断该对象是否已被拥有
					if( (*lpp)->lpOwner == lpCurThread )
						continue; // it ok
				}
				if( (*lpp)->semFlag & SF_MANUALRESET )
				{					
					if( lpCurThread->nsemRet == i &&
						lpCurThread->flag & FLAG_PULSE_EVENTING )
					{   //用PulseEvent唤醒
						lpCurThread->flag &= ~FLAG_PULSE_EVENTING;
						continue; // it ok
					}
				}
				//else if( (*lpp)->nPulseCount )
				//{	//必定是event 对象，并且用PulseEvent唤醒
					//continue; // it ok
				//}
				break;
			}
		}
		//是否所有的对象已有信号
		bGetIt = ( i == nCount );
		if( bGetIt )
		{   // 是，得到它们
		    lpp = lppsem;
			for( i = 0; i < nCount; i++, lpp++ )
			{
				if( (*lpp)->nCount <= 0 )
				{  // must be uiFlag = SF_OWNER
					(*lpp)->nLockCount++;
				}
				else
				{
					if( ( (*lpp)->semFlag & SF_MANUALRESET ) == 0 )
					{   // 如果该对象没有手工重设功能，将其设为无信号
						(*lpp)->nCount--;					
					}
					if( (*lpp)->semFlag & SF_OWNER )
					{   // 如果该对象是Mutex，将其拥有者设为当前线程
						(*lpp)->lpOwner = lpCurThread;
						(*lpp)->nLockCount = 1;
					}
				}
			}
			i = 0;
		}
	}
	else
	{   //判断是否有一个对象有信号，如果有得到它
		for( i = 0; i < nCount; i++, lpp++ )
		{
			if( (*lpp)->nCount > 0 )
			{
				if( ( (*lpp)->semFlag & SF_MANUALRESET) == 0 )
					(*lpp)->nCount--;
				if( (*lpp)->semFlag & SF_OWNER )
				{
                    (*lpp)->lpOwner = lpCurThread;
					(*lpp)->nLockCount = 1;
				}
				bGetIt = TRUE;
				break;
			}
			else
			{
				if( (*lpp)->semFlag & SF_OWNER )
				{
                    if( (*lpp)->lpOwner == lpCurThread )
					{
					    (*lpp)->nLockCount++;
						bGetIt = TRUE;
						break;
					}
				}
				else if( (*lpp)->semFlag & SF_MANUALRESET )
				{
					if( lpCurThread->nsemRet == i &&
						lpCurThread->flag & FLAG_PULSE_EVENTING )
					{   //用PulseEvent唤醒
						lpCurThread->flag &= ~FLAG_PULSE_EVENTING;
						bGetIt = TRUE;
						break;
					}
				}

				//else if( (*lpp)->nPulseCount )
				//{	//必定是event 对象，并且用PulseEvent唤醒
					//bGetIt = TRUE;
					//break;
				//}
			}
		}
	}

	UnlockIRQRestore( &uiSave ); //
	if( !bGetIt )
	{   // 没有得到资源，进入等待状态
		i = __DownSemphores( lppsem, nCount, dwExpireJiffies );

		// 等待返回
		if( i == WAIT_TIMEOUT ||
			i == WAIT_FAILED )
		{   // 等待错误或超时
			//UNLOCK_SCHE();//中断将会调用 UpSemaphore，所以不能用该函数
			return i;
		}
		i -= WAIT_OBJECT_0;
		ASSERT( i < nCount );

		// 如果不是等待所有对象，并且该对象需要手工重设则直接返回，否则，检查它
	    if( bWaitAll ||
			( lppsem[i]->semFlag & SF_MANUALRESET ) == 0 )
		{
			LockIRQSave( &uiSave );
			goto __CHECK_COUNT;
		}
	}
	//if( (*lpp)->nPulseCount )
		//(*lpp)->nPulseCount--;

    // 解锁调度器   

    //UNLOCK_SCHE();//中断将会调用 UpSemaphore，所以不能用该函数

    return (WAIT_OBJECT_0 + i);
}
