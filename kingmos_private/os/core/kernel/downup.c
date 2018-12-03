/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����down, up
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
    2003-06-11: LN,��INTR_ON, INTR_OFF ��Ϊ LockIRQSave UnlockIRQRestore
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
������void UpSemaphore( LPSEMAPHORE lpsem, int iCount )
������
	IN lpsem - �ź�������
	IN iCount - �ͷŵ��ź���
����ֵ��
	��
����������
	�ͷ�һ�����ź���
����: 
	��semaphore.c����ع��ܵ���
************************************************/
void UpSemaphore( LPSEMAPHORE lpsem, int iCount )
{
	UINT uiSave;

    LockIRQSave( &uiSave );
	
    // ��ס������   
	//LOCK_SCHE();   //�жϽ������ UpSemaphore�����Բ����øú���

	// ���iCount�ĺϷ���	
	if( lpsem->nCount + iCount <= lpsem->nMaxCount )
	{   // ���ź����Ƿ���Ҫ�趨ӵ����
		if( lpsem->semFlag & SF_OWNER )
		{   // �ǣ�����Mutex, ����ӵ���ߺͼ�������Ϊ0
			lpsem->nLockCount = 0;
			lpsem->lpOwner = NULL;
		}
        // ���ӿ����ź���
		lpsem->nCount += iCount;
		// �Ƿ��еȴ����߳�
		if( lpsem->nWaiting )
		{   // �ǣ��ͷ����еĵȴ����߳�
			lpsem->nWaiting = 0;
			
        	UnlockIRQRestore( &uiSave );
        //    UNLOCK_SCHE();			//�жϽ������ UpSemaphore�����Բ����øú���
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

    // ����������   
	//UNLOCK_SCHE();
}

/**************************************************
������void UpCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs )
������
	IN lpsem - �ź�������
	IN lpcs - �����ָ��
����ֵ��
	��
����������
	�ͷŻ���Σ�����еȴ��̣߳��ͷ�����
����: 
	��semaphore.c��KL_LeaveCritialSection��ع��ܵ���
************************************************/

void UpCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs )
{
	UINT uiSave;

    // ��ס������   
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
	{   // �ָ�ԭʼ���ȼ�
	    //RETAILMSG( 1, ( "restore pri from(%d) to (%d).\r\n", lpCurThread->nCurPriority, lpCurThread->nOriginPriority ) );
		//ASSERT( lpCurThread != lpInitKernelThread );
		SetThreadCurrentPriority( lpCurThread, lpCurThread->nOriginPriority );
	}

	// �Ƿ��еȴ����߳�
	if( lpsem->nWaiting )
	{   // �ǣ��ͷ����ǣ����ӵõ�����Դ���߳���ͬ�����ȼ��ı���ֵ�����ȵ���Ȩ��
		lpsem->nBoost = lpsem->nWaiting - 1;
		lpsem->nWaiting = 0;

		UnlockIRQRestore( &uiSave );//2003-06-11, ADD
		__Up( lpsem );
	}
	else
	{   // û���κ��̵߳ȴ��� CS
		lpsem->nHighPriority = IDLE_PRIORITY;//
		UnlockIRQRestore( &uiSave );//2003-06-11, ADD
	}
    // ����������   
	//UNLOCK_SCHE();
}


/**************************************************
������BOOL DownCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs, BOOL bEntryBlock )
������
	IN lpsem - �ź�������
	IN lpcs - �����ָ��
	IN bEntryBlock - ���û��ӵ��,�Ƿ�ֱ���˳�
����ֵ��
	TRUE/FALSE
����������
	���뻥��Σ�������ܵõ���Դ�������ȴ�����
����: 
	��semaphore.c��KL_EntryCritialSection��ع��ܵ���
************************************************/

BOOL DownCrit( LPSEMAPHORE lpsem, LPCRITICAL_SECTION lpcs, BOOL bEntryBlock )
{
	UINT uiSave;


    // ��ס������   

//	LOCK_SCHE();
	
__CHECK_COUNT:
	LockIRQSave( &uiSave );  //2003-06-11, ADD
    // �Ƿ��ܹ��õ�����Դ
	if( lpsem->nCount > 0 )
	{   // ����Դ���ã�now own the cs
		ASSERT( lpsem->nCount == 1 );
		
		lpsem->nCount = 0;
		// ������Դ��ӵ������Ϊ��ǰ�߳�
		lpsem->lpOwner = lpCurThread;
		// ������Դ��ӵ����ID��Ϊ��ǰ�߳�ID
		lpcs->dwOwnerThreadId = lpCurThread->dwThreadId;
		lpcs->iLockCount = 1;
		
		//�Ƿ�̳����ȼ�
		if( lpCurThread->nCurPriority > lpsem->nHighPriority )
		{   // �ǣ��̳����ȼ�
			//RETAILMSG( 1, ( "high current owner.\r\n" ) );
			//ASSERT( lpCurThread != lpInitKernelThread );
			SetThreadCurrentPriority( lpCurThread, lpsem->nHighPriority );
		}
		else
		{  // ��������ͬ�����ȼ��ڵı���ֵ
		    lpCurThread->nBoost += lpsem->nBoost;
		}
		UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
	}
	else if( bEntryBlock )
	{   //����Դ������ 
		// 2004-12-09, �жϸ�ӵ�����Ƿ���Ч
	//	if( !_GetThreadPtr( lpcs->dwOwnerThreadId ) )
	//	{	//��Ч�������Ѿ�û���ͷž��˳���
	//	}
		//

		//�Ƿ�����ӵ���ߵ����ȼ�
		if( lpsem->lpOwner->nCurPriority > lpCurThread->nCurPriority )
		{   //��
			//RETAILMSG( 1, ( "high owner: orig_pri(%d) to new_pri(%d),Owner state(0x%x).\r\n", lpsem->lpOwner->nCurPriority, lpCurThread->nCurPriority, lpsem->lpOwner->dwState ) );
			//����ӵ���ߵ����ȼ�
			lpsem->nHighPriority = lpCurThread->nCurPriority;
			ASSERT( lpsem->lpOwner != lpInitKernelThread );
			SetThreadCurrentPriority( lpsem->lpOwner, lpsem->nHighPriority );
		}
		UnlockIRQRestore( &uiSave ); //2003-06-11, ADD

#ifdef __DEBUG
		KL_TlsSetValue( TLS_CRITICAL_SECTION, lpcs );
#endif
        // ����ȴ�״̬
		__DownSemphores( &lpsem, 1, INFINITE );//dwExpireJiffies, TRUE );

#ifdef __DEBUG
		KL_TlsSetValue( TLS_CRITICAL_SECTION, 0 );
#endif
		// �ȴ����أ�����ȥ����Ƿ��ܹ��õ���Դ
		goto __CHECK_COUNT;
	}
	else
		return FALSE;
	//
    // ����������   
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
������DWORD DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwMilliseconds, BOOL bWaitAll )
������
	IN lpsem - �ź�����������
	IN nCount - �������ź���������Ķ�����
	IN dwMilliseconds - ������Ҫ�ȴ����ȴ�ʱ��
	IN bWaitAll - �Ƿ�ȴ����ж������ź� 
����ֵ��
	WAIT_OBJECT_0 + n �� WAIT_TIMEOUT �� WAIT_FAILED
����������
	�ж��Ƿ��ܹ��õ���Դ��������ܵõ���Դ�������ȴ�����
����: 
	��semaphore.c��ع��ܵ���
************************************************/

DWORD DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwMilliseconds, BOOL bWaitAll )
{
	DWORD dwExpireJiffies;
	DWORD i;
	LPSEMAPHORE * lpp;
	BOOL bGetIt = FALSE;
	UINT uiSave;

	// ������Ҫ�ȴ�ʱ������Ӧ��ʱ��Ƭ
	if( dwMilliseconds != INFINITE )
	    dwExpireJiffies = OEM_TimeToJiffies( dwMilliseconds, 0 );
	else
		dwExpireJiffies = INFINITE;	
    
    // ��ס������   
    //LOCK_SCHE();  //�жϽ������ UpSemaphore�����Բ����øú���
    //lpCurThread->nsemRet = 0;
    LockIRQSave( &uiSave );

	lpCurThread->flag &= ~FLAG_PULSE_EVENTING;

__CHECK_COUNT:

	lpp = lppsem;

	if( bWaitAll )
	{  // �ж����ж����Ƿ����ź�
		for( i = 0; i < nCount; i++, lpp++ )
		{
			if( (*lpp)->nCount <= 0 )
			{   // ����ö���û���źţ�����Ƿ���Mutex
				if( (*lpp)->semFlag & SF_OWNER )
				{   // ��Mutex���жϸö����Ƿ��ѱ�ӵ��
					if( (*lpp)->lpOwner == lpCurThread )
						continue; // it ok
				}
				if( (*lpp)->semFlag & SF_MANUALRESET )
				{					
					if( lpCurThread->nsemRet == i &&
						lpCurThread->flag & FLAG_PULSE_EVENTING )
					{   //��PulseEvent����
						lpCurThread->flag &= ~FLAG_PULSE_EVENTING;
						continue; // it ok
					}
				}
				//else if( (*lpp)->nPulseCount )
				//{	//�ض���event ���󣬲�����PulseEvent����
					//continue; // it ok
				//}
				break;
			}
		}
		//�Ƿ����еĶ��������ź�
		bGetIt = ( i == nCount );
		if( bGetIt )
		{   // �ǣ��õ�����
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
					{   // ����ö���û���ֹ����蹦�ܣ�������Ϊ���ź�
						(*lpp)->nCount--;					
					}
					if( (*lpp)->semFlag & SF_OWNER )
					{   // ����ö�����Mutex������ӵ������Ϊ��ǰ�߳�
						(*lpp)->lpOwner = lpCurThread;
						(*lpp)->nLockCount = 1;
					}
				}
			}
			i = 0;
		}
	}
	else
	{   //�ж��Ƿ���һ���������źţ�����еõ���
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
					{   //��PulseEvent����
						lpCurThread->flag &= ~FLAG_PULSE_EVENTING;
						bGetIt = TRUE;
						break;
					}
				}

				//else if( (*lpp)->nPulseCount )
				//{	//�ض���event ���󣬲�����PulseEvent����
					//bGetIt = TRUE;
					//break;
				//}
			}
		}
	}

	UnlockIRQRestore( &uiSave ); //
	if( !bGetIt )
	{   // û�еõ���Դ������ȴ�״̬
		i = __DownSemphores( lppsem, nCount, dwExpireJiffies );

		// �ȴ�����
		if( i == WAIT_TIMEOUT ||
			i == WAIT_FAILED )
		{   // �ȴ������ʱ
			//UNLOCK_SCHE();//�жϽ������ UpSemaphore�����Բ����øú���
			return i;
		}
		i -= WAIT_OBJECT_0;
		ASSERT( i < nCount );

		// ������ǵȴ����ж��󣬲��Ҹö�����Ҫ�ֹ�������ֱ�ӷ��أ����򣬼����
	    if( bWaitAll ||
			( lppsem[i]->semFlag & SF_MANUALRESET ) == 0 )
		{
			LockIRQSave( &uiSave );
			goto __CHECK_COUNT;
		}
	}
	//if( (*lpp)->nPulseCount )
		//(*lpp)->nPulseCount--;

    // ����������   

    //UNLOCK_SCHE();//�жϽ������ UpSemaphore�����Բ����øú���

    return (WAIT_OBJECT_0 + i);
}
