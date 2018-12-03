/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�������ȹ���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2004-09-29, �޸� Schedule
    2004-08-24, 08-14���޸ĵ����¼��޷�����
    2004-08-14, __DownSemphores INTR_ON();  //2004-08-14, add by lilin
	2004-08-11, �����ö�̬����( WaitForSingleObject��WaitMultiSingleObject  ) �ź������漰�� semaphpor.c thread.c sche.c
    2003-08-30: timeout ��0
    2003-08-21:LN, WakeupThread,�������߳�ʱ���������ѵ�
	           ���ȼ����һ���㡣
    2003-04-24:LN,����supend and resume ����
******************************************************/

#include <eframe.h>
#include <eobjtype.h>
#include <eassert.h>
#include <epcore.h>
#include <sysintr.h>
#include <coresrv.h>
#include <epalloc.h>
#include <oemfunc.h>

//1024 for USE_THREAD_PAGE_TABLE
struct {
	THREAD thread;
	BYTE   dump[THREAD_STRUCT_SIZE - sizeof(THREAD) + 1024];
}_InitKernelThread = { INIT_THREAD, };
const LPTHREAD lpInitKernelThread = (LPTHREAD)&_InitKernelThread;

PROCESS InitKernelProcess = INIT_PROCESS;

HANDLEDATA InitKernelProcessHandle = { { OBJ_PROCESS, NULL, NULL, (DWORD)&InitKernelProcessHandle, 1 }, (HANDLE)&InitKernelProcessHandle, &InitKernelProcess  };
HANDLEDATA InitKernelThreadHandle = { { OBJ_THREAD, NULL, NULL, (DWORD)&InitKernelProcessHandle, 1 }, (HANDLE)&InitKernelThreadHandle, &InitKernelProcess  };
DWORD InitKernelThreadTLS[TLS_MAX_INDEXS];
LPTHREAD lpCurThread = (LPTHREAD)&_InitKernelThread;//lpInitKernelThread;//lpInitKernelThread;

LPPROCESS_SEGMENTS lppProcessSegmentSlots[MAX_PROCESS_ID];
LPPROCESS lppProcessPtr[MAX_PROCESS_ID];

#ifdef VIRTUAL_MEM
//LPPROCESS_SEGMENTS KernelSegment;   // virtual = KERNEL_SEGMENT = 0xC0000000
#endif

static void SetPriorityBitmap( UINT uPriority );
static void ClearPriorityBitmap( UINT uPriority );

//	�ص��ȱ�־
BOOL bNeedResched = 0;
//	��ǰ��������̬���߳���
int nCountRunning = 0;
//	ʱ��Ƭ
volatile DWORD dwJiffies = 0;

// ���ȼ�ӳ���
// ϵͳ��256�����ȼ���Ϊ8���飬��uPriorityGroup��ʾ�� 
// uPriorityGroup��ÿ��bit��ʾһ�����飨0��ʾ�÷���û�������̣߳�1��ʾ�У�
// bRTIndexTable���ڲ���uPriorityGroup������ȼ�����
static const BYTE bRTIndexTable[] = 
{ 
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x00 - 0x0f(15)
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x10
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x20
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x30

    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x40
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x50
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x60
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,    // 0x70   - 0x7f

    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x80
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x90
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0xa0
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0xb0

    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0xc0
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0xd0
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0xe0
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0    // 0xf0  - 0xff
};

// 8�����ȼ����� * 32 �����ȼ�
static UINT uPriorityGroup;
// 256��bits, ÿ��DWORD��ʾһ������
static DWORD dwPriorityBitmap[8];
// ���ڵõ���0-32��һ��DWORD����Ӧ��bitsλ
static const DWORD bitMask[] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
                                 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
								 0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
								 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 };

//	256�����ȼ��̶߳��У����о�����ͬ���ȼ�[pri]�Ŀ������̶߳�����lpThreadList[pri]���������
static LPTHREAD lpThreadList[256];


// ********************************************************************
//������ int KL_GetCountRunning( void )
//������
//	��
//����ֵ��
//	���ص�ǰ�������߳���
//����������
//	�õ�ϵͳ��ǰ�������߳���
//���ã�
//	ϵͳAPI
// ********************************************************************
int KL_GetCountRunning( void )
{
	return nCountRunning;
}
/*
// ********************************************************************
//������ int GetRuningThreadList( void )
//������
//	��
//����ֵ��
//	���ص�ǰ�������߳���
//����������
//	�õ�ϵͳ��ǰ�������߳���
//���ã�
//	ϵͳAPI
// ********************************************************************
LPTHREAD * GetRuningThreadList( void )
{
	return lpThreadList;
}
*/
// ********************************************************************
//������void __AddWaitQueue( LPWAITQUEUE * lppQueue, LPWAITQUEUE lpWait ) 
//������
//	IN lppQueue - LPWAITQUEUEָ�룬ָ�����ͷ�ĵ�ַ
//	IN lpWait - WAITQUEUE�ṹָ�룬��Ҫ����ĵȴ��ڵ�
//����ֵ��
//	��
//����������
//	��һ���ȴ��̼߳���ȴ����е���λ��
//���ã�
//	
// ********************************************************************

static void __AddWaitQueue( LPWAITQUEUE * lppQueue, LPWAITQUEUE lpWait )
{
	UINT uiSave;
	LPWAITQUEUE lpHead;
	LPWAITQUEUE lpNext;
		
	LockIRQSave( &uiSave ); // ���� & ���ж�
	
	lpHead = *lppQueue;
	
	if ( lpHead )
		lpNext = lpHead;
	else
	    lpNext = WAIT_QUEUE_HEAD( lppQueue );

	//������λ��
	*lppQueue = lpWait;
	lpWait->lpNext = lpNext;
	
	UnlockIRQRestore( &uiSave );  // �ָ�
}

// ********************************************************************
//������void __RemoveWaitQueue( LPWAITQUEUE *lppQueue, LPWAITQUEUE lpWait ) 
//������
//	IN lppQueue - LPWAITQUEUEָ�룬ָ�����ͷ�ĵ�ַ
//	IN lpWait - WAITQUEUE�ṹָ�룬��Ҫ�Ƴ��ĵȴ��ڵ�
//����ֵ��
//	��
//����������
//	��һ���ȴ��߳��Ƴ��ȴ�����
//���ã�
//	
// ********************************************************************

void __RemoveWaitQueue( LPWAITQUEUE *lppQueue, LPWAITQUEUE lpWait )
{
	UINT uiSave;
	LPWAITQUEUE lpNext;
	LPWAITQUEUE lpHead;
	
	LockIRQSave( &uiSave );// ���� & ���ж�

	lpNext = lpWait->lpNext;
	lpHead = lpNext;
	
	while( 1 ) 
	{
		LPWAITQUEUE lpNextList = lpHead->lpNext;
		if ( lpNextList == lpWait )
			break;  // �ҵ�
		lpHead = lpNextList;
	}
	//�Ƴ�
	lpHead->lpNext = lpNext;

	UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������LPTHREAD GetHighestPriority( LPUINT lpPriority ) 
//������
//	OUT lpPriority - ���ڽ��ܵ�ǰ������ȼ��ļ���������ΪNULL��
//����ֵ��
//	�����߳�ָ��
//����������
//	�õ���ǰ������ȼ��߳�
//���ã�
//	
// ********************************************************************

static LPTHREAD GetHighestPriority( LPUINT lpPriority )
{
	UINT uIndex;
    DWORD dwBitmap;
	LPTHREAD lpThread = NULL;
    UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�
	//�õ���߷���
	uIndex = bRTIndexTable[uPriorityGroup];
	//�õ��÷�������ȼ�λͼ
	dwBitmap = dwPriorityBitmap[uIndex];
	//�÷���ĵ�һ�����ȼ���
	uIndex <<= 5;
	while( dwBitmap )
	{
		UINT bit = dwBitmap & 0xff; // 8�����ȼ�
		if( bit )
		{	//���ڣ��õ���ߵ�
			uIndex += bRTIndexTable[bit];
			if( lpPriority )
				*lpPriority = uIndex;
			lpThread = lpThreadList[uIndex];
			break;
		}
		//û�У�ȡ��һ����λ
		dwBitmap >>= 8;//
		uIndex += 8;
	}

    UnlockIRQRestore( &uiSave );// �ָ�
	return lpThread;
}

// ********************************************************************
//������LPTHREAD GetHigherPriority( UINT uiHighestPriority, LPUINT lpPriority ) 
//������
//	IN uiHighestPriority - ������ȼ�
//	OUT lpPriority - ���ڽ��ܴ����ȼ��ļ���
//����ֵ��
//	����ɹ������ص�ǰ�θ����ȼ��߳�ָ�룬*lpPriorityΪ�θ����ȼ�����������û�У�����NULL
//����������
//	�õ���ǰ�θ����ȼ��̼߳��伶��
//���ã�
//	
// ********************************************************************

static LPTHREAD GetHigherPriority( UINT uiHighestPriority, LPUINT lpPriority )
{
	LPTHREAD lpThread;
    UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�

	// ���������ȼ�
	ClearPriorityBitmap( uiHighestPriority );
	// �õ�������ȼ�
    lpThread = GetHighestPriority( lpPriority );
	// �ָ�������ȼ�
	SetPriorityBitmap( uiHighestPriority );

    UnlockIRQRestore( &uiSave );// �ָ�
	return lpThread;
}

// ********************************************************************
//������static void SetPriorityBitmap( UINT uPriority ) 
//������
//	IN uPriority - ���ȼ�
//����ֵ��
//	��
//����������
//	�������ȼ�λͼ
//���ã�
//	
// ********************************************************************

static void SetPriorityBitmap( UINT uPriority )
{
    UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�
	// ���÷���
	uPriorityGroup |= bitMask[uPriority >> 5];
	//	���ø����λͼ
	dwPriorityBitmap[uPriority >> 5] |= bitMask[uPriority & 0X1F];

    UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������void ClearPriorityBitmap( UINT uPriority ) 
//������
//	IN uPriority - ���ȼ�
//����ֵ��
//	��
//����������
//	������ȼ�λͼ
//���ã�
//	
// ********************************************************************

static void ClearPriorityBitmap( UINT uPriority )
{
    UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�
	
	if( ( dwPriorityBitmap[uPriority >> 5] &= ~(bitMask[uPriority & 0X1F]) ) == 0 )
	    uPriorityGroup &= ~bitMask[uPriority >> 5];

    UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������void SetThreadCurrentPriority( LPTHREAD lpThread, UINT lPriority ) 
//������
//	IN lpThread - THREAD �ṹָ��
//	IN lPriority - ���ȼ�
//����ֵ��
//	��
//����������
//	�����̵߳ĵ�ǰ���ȼ�
//���ã�
//	
// ********************************************************************

void SetThreadCurrentPriority( LPTHREAD lpThread, UINT lPriority )
{
    UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�
//    ASSERT( lpThread != lpInitKernelThread );
	if( lpThread->dwState == THREAD_RUNNING )
	{	//�������߳�
		//�����ж������Ƴ�
		RemoveFromRunQueue( lpThread );
		//�����µ����ȼ�������������ж���
		lpThread->nCurPriority = lPriority;
		AddToRunQueue( lpThread );
	}
	else
		lpThread->nCurPriority = lPriority;
	
	UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������void AddToRunQueue( LPTHREAD lpThread ) 
//������
//	IN lpThread - THREAD�ṹָ��
//����ֵ��
//	��
//����������
//	���̼߳������ж���
//���ã�
//	
// ********************************************************************

void AddToRunQueue( LPTHREAD lpThread )
{
    UINT uiSave;
	UINT uPri;
	LPTHREAD lpList;

	LockIRQSave( &uiSave );// ���� & ���ж�


	if( lpThread->nSuspendCount ) //2003-04-24:LN,����
	{  //��Ȼ����
		lpThread->dwState = THREAD_SUSPENDED;//2003-04-24:LN,����
		goto _ADD_RET; //2003-04-24:LN,����
	}  //2003-04-24:LN,����

	if( lpThread->lpNextRun || lpThread->lpPrevRun )
	{	
		goto _ADD_RET;
	}
	//�õ���ǰ���ȼ����ڵĿ����ж���
	uPri = lpThread->nCurPriority;
	lpList = lpThreadList[uPri];
	//�ŵ�����ͷ
	if( lpList )
	{	
		( lpThread->lpPrevRun = lpList->lpPrevRun )->lpNextRun = lpThread;
		lpThread->lpNextRun = lpList;
		lpList->lpPrevRun = lpThread;
	}
	else
	{	//
		lpThread->lpPrevRun = lpThread->lpNextRun = lpThread;
		//�������ȼ�λͼ
	    SetPriorityBitmap( uPri );
	}
	lpThreadList[uPri] = lpThread;	
	//�Ƿ������ص���	
	if( uPri <= lpCurThread->nCurPriority )
		bNeedResched = 1;//��
	nCountRunning++;   // ���������߳���

_ADD_RET:

	UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������void RemoveFromRunQueue( LPTHREAD lpThread ) 
//������
//	IN lpThread - THREAD�ṹָ��
//����ֵ��
//	��
//����������
//	���߳��Ƴ����ж���
//���ã�
//	
// ********************************************************************

void RemoveFromRunQueue( LPTHREAD lpThread )
{
	LPTHREAD lpNext;
	LPTHREAD lpPrev;
	UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�
     
	if( lpThread != lpInitKernelThread )
	{	//
		UINT uPri = lpThread->nCurPriority;

		lpNext = lpThread->lpNextRun;
		lpPrev = lpThread->lpPrevRun;
		//ASSERT( lpNext && lpPrev );
		
		//�Ƴ�
		if( lpNext == lpThread )
		{	//�����ȼ����н���һ��
			//ASSERT(lpPrev == lpThread);
			lpThreadList[uPri] = NULL;
			//������ȼ�λͼ
			ClearPriorityBitmap(uPri);
		}
		else
		{	//�Ƿ��ڶ���ͷ ��
		    if( lpThreadList[uPri] == lpThread )
			{	//��
				lpThreadList[uPri] = lpNext;
			}

			lpNext->lpPrevRun = lpPrev;
		    lpPrev->lpNextRun = lpNext;
		}
		
		lpThread->lpNextRun = NULL;
		lpThread->lpPrevRun = NULL;
		lpThread->nBoost = 0;
		nCountRunning--; 
	}
	else
	{	//����Ӧ�õ��������
		ASSERT(0);
	}

	UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������void MoveToEndOfRunQueue( LPTHREAD lpThread ) 
//������
//	IN lpThread - THREAD�ṹָ��
//����ֵ��
//	��
//����������
//	���߳��Ƶ������ж���β��
//���ã�
//	
// ********************************************************************

void MoveToEndOfRunQueue( LPTHREAD lpThread )
{
	UINT uiSave;
	LPTHREAD lpNext;
	LPTHREAD lpPrev;
	UINT uPri;
	LPTHREAD lpList;
	
	LockIRQSave( &uiSave );// ���� & ���ж�

	lpNext = lpThread->lpNextRun;
	lpPrev = lpThread->lpPrevRun;
	uPri = lpThread->nCurPriority;
	lpList = lpThreadList[uPri];
	//�Ƿ��ڶ���ͷ ��
    if( lpList == lpThread )
	{	//��
		lpThreadList[uPri] = lpNext;
	}

	// �Ƴ�
	lpNext->lpPrevRun = lpPrev;
	lpPrev->lpNextRun = lpNext;
	// �ŵ�β��
	lpThread->lpNextRun = lpList;
	lpPrev = lpList;
	lpList->lpPrevRun = lpThread;
	lpThread->lpPrevRun = lpPrev;
	lpPrev->lpNextRun = lpThread;
	
	UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������void _LinkThread( LPTHREAD lpThread ) 
//������
//	IN lpThread - THREAD�ṹָ��
//����ֵ��
//	��
//����������
//	���̼߳��뵽�߳�˫����
//���ã�
//	
// ********************************************************************

void _LinkThread( LPTHREAD lpThread )
{
	UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�

    // ���ӵ�����
	lpThread->lpNextThread = lpInitKernelThread;	
	lpThread->lpPrevThread = lpInitKernelThread->lpPrevThread;

	lpInitKernelThread->lpPrevThread->lpNextThread = lpThread;
	lpInitKernelThread->lpPrevThread = lpThread;

	UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������void _RemoveThread( LPTHREAD lpThread ) 
//������
//	IN lpThread - THREAD�ṹָ��
//����ֵ��
//	��
//����������
//	��_LinkThread��Ӧ���ù��ܽ��̴߳��߳�˫�����Ƴ�
//���ã�
//	
// ********************************************************************

void _RemoveThread( LPTHREAD lpThread )
{
	UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�

	// �Ƴ�ϵͳ����
	ASSERT( lpThread->dwMagic == THREAD_MAGIC );

	lpThread->lpNextThread->lpPrevThread = lpThread->lpPrevThread;
	lpThread->lpPrevThread->lpNextThread = lpThread->lpNextThread;

   
	UnlockIRQRestore( &uiSave );// �ָ�
}

// ********************************************************************
//������void _WakeupThread( LPTHREAD lpThread, BOOL bHighLevel ) 
//������
//	IN lpThread - THREAD�ṹָ��
//	IN bHighLevel - �Ƿ�����һ���㣬����TRUE��������nBoostһ����
//����ֵ��
//	��
//����������
//	�����߳�
//���ã�
//	
// ********************************************************************

void _WakeupThread( LPTHREAD lpThread, BOOL bHighLevel )
{
	UINT uiSave;

	LockIRQSave( &uiSave );// ���� & ���ж�

	lpThread->dwState = THREAD_RUNNING;


	if ( !lpThread->lpNextRun )
	{   
		if( bHighLevel )
			lpThread->nBoost++;  //��ʱ���� 1 tick
		//���뵽�����ж���
	    AddToRunQueue( lpThread );
	}

	UnlockIRQRestore( &uiSave );// �ָ�
}


// ********************************************************************
//������static void ThreadTimeout( DWORD dwData ) 
//������
//	IN dwData - THREAD�ṹָ��
//����ֵ��
//	��
//����������
//	˯�ߵ��ڻص����������߳�˯�ߵ���ʱ������øú���ȥ���Ѹ��߳�
//���ã�
//	
// ********************************************************************
 
static void ThreadTimeout( DWORD dwData )
{
	((LPTHREAD)dwData)->dwTimeout = 0;
	if( ((LPTHREAD)dwData)->dwState == THREAD_INTERRUPTIBLE ||
		((LPTHREAD)dwData)->dwState == THREAD_UNINTERRUPTIBLE )
	    _WakeupThread( (LPTHREAD)dwData, FALSE );//���Ѹ��߳�
	else
	{
		;
	}
}


// ********************************************************************
//������ static LPTHREAD GetNextRun( LPTHREAD lpPrev, DWORD dwTimeout )
//������
//	IN lpPrev  - THREAD�ṹָ��
//	IN dwTimeout - ǰһ���߳��Ƿ�����Ҫ˯��
//����ֵ��
//	THREADָ��
//����������
//	�õ���Ҫ���ȵ��̡߳��ù��ܱ������еĿ������̲߳��������ȼ���ߵ�һ����
//	ͬ�����ȼ�������Ҫ���е�һ��
//���ã�
//	
// ********************************************************************

static LPTHREAD GetNextRun( LPTHREAD lpPrev, DWORD dwTimeout )
{
	{		
		LONG lCount;
		LPTHREAD p, lpNext;
		LPTHREAD lpHeadList = GetHighestPriority( NULL );//�õ�������ȼ�����
		
		//ASSERT( lpHeadList );
		//�ȳ�ʼ��һ��
		lCount = lpHeadList->nTickCount + lpHeadList->nBoost;
		p = lpHeadList->lpNextRun;
		lpNext = lpHeadList;
		//����ͬ�����ȼ��������߳�
		while( p != lpHeadList )
		{
			LONG lCurCount;
			lCurCount = p->nTickCount + p->nBoost;
			if( lCurCount  )
			{	
				if( p == lpPrev )	//�Ƿ���ǰһ����
				{	//��
					if( dwTimeout )	//ǰһ���Ƿ���Ҫ˯��
						lCurCount = -1000;  //�ǣ�����Ҫ����
					else
						lCurCount++;  //�����һ����
				}
			}
			//ѡ���������lCurCountֵ���߳�
			if( lCount < lCurCount )
			{	
				lCount = lCurCount;
				lpNext = p;
			}
			p = p->lpNextRun;
		}
		
		// ���籾���ȼ��������̵߳�counterֵ��Ϊ0�������³�ʼ������
		if( !lCount )
		{	//��Ϊ0
			p = lpHeadList;
			do{
			    p->nTickCount = (short)p->nRotate;
				p = p->lpNextRun;
			}while( p != lpHeadList );
		}
		// ��������ȼ�û���κ��̣߳��������أ�������´���ȥ��
		//if( lpNext == lpPrev && dwTimeout )
		//{   // �����ȼ��������̶߳�����ҪCPU,��ǰһ���̱߳������һ��CPU
			// �����ô����ȼ����߳�ȥ����
        //   lpNext = GetHigherPriority( lpNext->nCurPriority, NULL );
		//	ASSERT( lpNext != lpPrev );
		//}

		if( lpNext->nBoost ) //������ܣ���ȥ��ʱ����ֵ
			lpNext->nBoost--;

	
		return lpNext;
	}
}

// ********************************************************************
//������ void CALLBACK Schedule(void)
//������
//	��
//����ֵ��
//	��
//����������
//	���Ⱥ��������߳���Ҫ����CPU�����ʱ��Ƭ����ʱ����øú�����ѡ��һ���µ��߳�ȥ����
//���ã�
//	
// ********************************************************************

void CALLBACK Schedule(void)
{

	LPTHREAD lpPrev, lpNext;
	DWORD dwTimeout = 0;

	INTR_OFF();	//�ص��ж�  
//2004-09-29, remove by lilin, ��Ϊ ISR_Handler ����ж� ���Ҹù����� DefaultHandler Ҳ�д���
	
	if( iISRActiveCount )	//�Ƿ��м�����ж�����
	{   // �Ǵ�������
		ISR_Handler( ISR_ALL_INTRS );
	}
//2004-09-29	
	
	//
	bNeedResched = 0;
	lpPrev = lpCurThread;

	// ��ת����
	if( lpPrev->nTickCount != 0 && lpPrev->fPolicy == THREAD_POLICY_ROTATION )
	{	//
		lpPrev->nTickCount = (short)lpPrev->nRotate;
		//�Ƶ�����β
		MoveToEndOfRunQueue( lpPrev );
	}

//2004-09-29, remove by lilin, ��Ϊ HandleSignal ����ж�
//�ҽ���ŵ�ĩβ������Ҿ���Ӧ��ͬʱ DefaultHandler
	if( lpPrev->dwSignal & ~lpPrev->dwBlocked )	//�Ƿ����ź�
	{	//�ǣ������ź�
		HandleSignal();
	}
//2004-09-29
	//��鵱ǰ�߳�
	switch( lpPrev->dwState )
	{
		case THREAD_INTERRUPTIBLE:
			if( lpPrev->dwSignal & ~lpPrev->dwBlocked )
			{
				lpPrev->dwState = THREAD_RUNNING;
				break;
			}
			if( lpPrev->dwTimeout && lpPrev->dwTimeout <= dwJiffies )
			{   // continue, but ����һ��CPU
				dwTimeout = lpPrev->dwTimeout;
				lpPrev->dwTimeout = 0;
				lpPrev->nBoost = 0;
				lpPrev->dwState = THREAD_RUNNING;
				break;
			}
		default:
            RemoveFromRunQueue( lpPrev );//�Ƴ������ж���
			break;
		case THREAD_RUNNING:
			break;
	}
	//�������п������̣߳��õ�һ�����ŵ��߳�ȥ����
	lpNext = GetNextRun( lpPrev, dwTimeout );

	if( lpPrev != lpNext )
	{
	    //lpCurThread = lpNext;
//#ifdef VIRTUAL_MEM
		//������MMU���������µĽ��̿ռ�
		if( lpPrev->lpCurProcess != lpNext->lpCurProcess ||
			lpPrev->akyAccessKey != lpNext->akyAccessKey )
		    GetMMUContext( lpNext, 0, lpNext->lpCurProcess );
//#endif
		//�л�
		SwitchTo( lpPrev, lpNext );
	}

	INTR_ON();

	return;
}


// ********************************************************************
//������static void WakeupWaitQueue( LPWAITQUEUE *lppQueue ) 
//������
//	IN lppQueue - LPWAITQUEUEָ��
//����ֵ��
//	��
//����������
//	���������ڸö��еĵȴ��߳�
//���ã�
//	
// ********************************************************************

static void WakeupWaitQueue( LPWAITQUEUE *lppQueue )
{
	LPWAITQUEUE lpNext;
	LPWAITQUEUE lpHead;
	UINT uiSave;

	if( !lppQueue || !(lpNext = *lppQueue) )
	{
		RETAILMSG( 1, ("error at WakeupWaitQueue: lppQueue(0x%x),lpNext(0x%x).\r\n", lppQueue, lpNext ) );
		return;
	}
	

	LockIRQSave( &uiSave );// ���� & ���ж�

	lpHead = WAIT_QUEUE_HEAD( lppQueue );
	while( lpNext != lpHead )
	{
		LPTHREAD lpThread = lpNext->lpThread;
		LPWAITQUEUE lpCur = lpNext;

		lpNext = lpNext->lpNext;
		if ( lpThread ) 
		{
			if ( lpThread->dwState == THREAD_UNINTERRUPTIBLE ||
			     lpThread->dwState == THREAD_INTERRUPTIBLE )
			{
				if( lpThread->lpRouser == NULL )
				    lpThread->lpRouser = lpCur;
				_WakeupThread( lpThread, TRUE ); //������
			}
		}
		if ( !lpNext )
		{
			RETAILMSG( 1, ("error at WakeupWaitQueue: lpNext==NULL!.\r\n" ) );
			break;
		}
	}

	UnlockIRQRestore( &uiSave );//�ָ�
	return;
}

// ********************************************************************
//������void __Up( LPSEMAPHORE lpsem ) 
//������
//	IN lpsem - SEMAPHORE�ṹָ��
//����ֵ��
//	��
//����������
//	�ͷŵȴ��ź������߳�
//���ã�
//	
// ********************************************************************

void __Up( LPSEMAPHORE lpsem )
{
	WakeupWaitQueue( &lpsem->lpWaitQueue );
}

// ********************************************************************
//������DWORD __DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwTimeout ) 
//������
//	IN lppsem - LPSEMAPHOREָ������
//	IN nCount - ������LPSEMAPHOREָ���������ָ���� <= MAX_WAITOBJS
//	IN dwTimeout - ��ʱ�ȴ�ʱ�䣨�Ѻ���Ϊ��λ��
//����ֵ��
//	����ɹ������� WAIT_OBJECT_0+n ��������һ���������źţ� �� WAIT_TIMEOUT��û���κζ������źŲ��ҵȴ�ʱ�䵽����
//	���򣬷��� WAIT_FAILED
//����������
//	
//���ã�
//	�����ź���
// ********************************************************************

DWORD __DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwTimeout )
{
	LPTHREAD lpThread = lpCurThread;
	//WAITQUEUE waitQueue[MAX_WAITOBJS];
	WAITQUEUE * lpWaitQueue;
	//TIMERLIST timer;
	DWORD dwExpire;
	DWORD dwRetv = WAIT_OBJECT_0;
	DWORD i;

	//2004-08-11������ö�̬����ķ���������Ҫ��HandleThreadExit����Ӧ���ͷŴ���
	//lpWaitQueue = &waitQueue[0];
	lpWaitQueue = (WAITQUEUE *)KHeap_Alloc( nCount * sizeof( WAITQUEUE ) );
	if( lpWaitQueue == NULL )
		return WAIT_FAILED;
	
	INTR_OFF();

	lpThread->nsemRet = WAIT_OBJECT_0;

    //	��ʼ��ÿһ���ȴ�����
	for( i = 0; i < nCount; i++ )
	{
		lpWaitQueue[i].lpThread = lpThread;
		lpWaitQueue[i].lpNext = NULL;
	    __AddWaitQueue( &lppsem[i]->lpWaitQueue, &lpWaitQueue[i] );
		lpThread->lppsemWait[i] = lppsem[i];	//2005-01-29, add
	}
	//	�趨���̵߳ĵȴ��¼�
	//lpThread->lppsemWait = lppsem; //2005-01-29, remove
	lpThread->nWaitCount = (WORD)nCount;
    lpThread->lpWaitQueue = lpWaitQueue;
	if( dwTimeout == INFINITE )
	{	//�޳�ʱҪ������
		lpThread->dwState = THREAD_UNINTERRUPTIBLE;
	}
	else
	{  // ��Ҫ��ʱ������������һ��timer
		dwExpire = dwTimeout + dwJiffies;
		lpCurThread->dwTimeout = dwExpire;//dwTimeout;
		lpThread->dwState = THREAD_INTERRUPTIBLE;

		INIT_TIMER_LIST( &lpCurThread->timer );

		lpCurThread->timer.dwExpires = dwExpire;
		lpCurThread->timer.dwData = (DWORD)lpCurThread;
		lpCurThread->timer.lpFunction = ThreadTimeout;

		_AddTimerList(&lpCurThread->timer);
		//lpCurThread->lpTimer = &timer;
	}

	while( 1 )
	{	//�Ƿ����źţ�
		for( i = 0; i < nCount; i++ )
		{
			if( lppsem[i]->nCount )
			{
				dwRetv = WAIT_OBJECT_0 + i;//�У�����
				while( i-- )
				{
					lppsem[i]->nWaiting--;  // ���֮ǰ���ӵĵȴ�����
				}
				lpThread->nsemRet = (WORD)dwRetv;
				goto _return;
			}			
			lppsem[i]->nWaiting++;
		}
		// û���κζ������ź�
		
		if( dwTimeout != INFINITE && lpCurThread->dwTimeout == 0 )
		{// ��Ҫ��ʱ������ʱʱ��Ϊ0������
			i = nCount;
			while( i-- )
			{
				lppsem[i]->nWaiting--;  // ���֮ǰ���ӵĵȴ�����
			}
			dwRetv = WAIT_TIMEOUT;
			break;
		}

		//����CPU���ȴ��ź�
		//INTR_ON();  //2004-08-14, add by lilin

		Schedule();
		
		INTR_OFF();

		if( lpThread->lpRouser )
		{
			 i = (WAITQUEUE*)lpThread->lpRouser - lpWaitQueue;
			 ASSERT( i < nCount );
			 if( i < nCount )
			 {
			    if( lppsem[i]->nCount ||
					(lpThread->flag & FLAG_PULSE_EVENTING) )
				{
					dwRetv = WAIT_OBJECT_0 + i;//�У�����
					lpThread->nsemRet = (WORD)dwRetv;
					lpThread->lpRouser = NULL;
					goto _return;
				}
			 }
			 else
			 {
				 ERRORMSG( 1, ( "__DownSemphores: error lpRouser(0x%x),lpWaitQueue(0x%x),i( %d )", lpThread->lpRouser, lpWaitQueue, i ) );
			 }
			 lpThread->lpRouser = NULL;
		}
		
		if( dwTimeout == INFINITE )
		    lpThread->dwState = THREAD_UNINTERRUPTIBLE;
		else
			lpThread->dwState = THREAD_INTERRUPTIBLE;
	}

_return:
	//ASSERT( lpThread == lpCurThread );
	//
	lpThread->dwTimeout = 0;
	lpThread->dwState = THREAD_RUNNING;
//	lpThread->lpTimer = NULL;
//	lpThread->lppsemWait = NULL;	//2005-01-29, remove
	lpThread->nWaitCount = 0;
    lpThread->lpWaitQueue = NULL;
    

	INTR_ON();    

	if( dwTimeout != INFINITE )
	{	//ɾ��timer
		_DelTimerList(&lpCurThread->timer);

	}
	

    //���
	for( i = 0; i < nCount; i++ )
	{
	    __RemoveWaitQueue( &lppsem[i]->lpWaitQueue, &lpWaitQueue[i] );
	}

	KHeap_Free( lpWaitQueue, nCount * sizeof( WAITQUEUE ) );  

	

//	ASSERT( lpThread->lpPrevRun && lpThread->lpNextRun );

	

	return dwRetv;
}

// ********************************************************************
//������DWORD ScheduleTimeout( DWORD dwTimeout ) 
//������
//	IN dwTimeout - �ӳ�ʱ�䣨�Ժ���Ϊ��λ��
//����ֵ��
//	����ʵ�ʵȴ���ʱ��
//����������
//	���߳��ӳ�һ��ʱ��
//���ã�
//	
// ********************************************************************

DWORD ScheduleTimeout( DWORD dwTimeout )
{
	//TIMERLIST timer;
	DWORD dwExpire;
	DWORD dwNeedJiffies;

	if( dwTimeout == INFINITE )
	{	//����
		lpCurThread->dwState = THREAD_UNINTERRUPTIBLE;
		Schedule();
		return dwTimeout;		
	}
	else
	{	//
	    dwNeedJiffies = OEM_TimeToJiffies( dwTimeout, 0 );  // �õ���������Ӧ��ʱ��Ƭ

	    INTR_OFF();
		//��ʼ��timer
		dwExpire = dwNeedJiffies + dwJiffies;
		lpCurThread->dwTimeout = dwExpire;
		// 2004-05-13�� lilin,���ʱ��̫�̣��޷�ʵ��Sleep�Ĺ���
		// ���������̵߳ò���ʱ��
		//lpCurThread->dwState = THREAD_INTERRUPTIBLE;
		if( dwTimeout )
		{	//�����л����������߳����Ƴ���
			lpCurThread->dwState = THREAD_UNINTERRUPTIBLE;
		}
		else			
		{	//���ͬ���ȼ����̴߳��ڣ����л���ͬ���ȼ��������߳�
			//����ֱ�ӷ���
		    lpCurThread->dwState = THREAD_INTERRUPTIBLE;
		}
		//

		INIT_TIMER_LIST( &lpCurThread->timer );
		lpCurThread->timer.dwExpires = dwExpire;
		lpCurThread->timer.dwData = (DWORD)lpCurThread;
		lpCurThread->timer.lpFunction = ThreadTimeout;//�����̵߳Ļص�����
		//����timer��ʽ
		_AddTimerList(&lpCurThread->timer);
		//lpCurThread->lpTimer = &timer;

	    INTR_ON();
		//����CPU
		Schedule();

		//�Ƴ�
		_DelTimerList(&lpCurThread->timer);

		//lpCurThread->lpTimer = NULL;
		//ʵ�ʳ�ʱ
		dwTimeout = dwExpire - dwJiffies;
	}
	return dwTimeout;
}

static TIMERLIST timerHead = { &timerHead, &timerHead, ~0, 0, NULL };
// ********************************************************************
//������void _AddTimerList( LPTIMERLIST lpTimer ) 
//������
//	IN lpTimer - TIMERLIST�ṹָ��
//����ֵ��
//	��
//����������
//	��һ����ʱ���������timerHead����
//���ã�
//	
// ********************************************************************

void _AddTimerList( LPTIMERLIST lpTimer )
{
	LPTIMERLIST p;
	UINT uiSave;    

	p = &timerHead;
    LockIRQSave( &uiSave );// ���� & ���ж�
	//����ʱʱ���С��������
	do {
		p = p->lpNext;
	} while ( lpTimer->dwExpires > p->dwExpires );

	lpTimer->lpNext = p;
	lpTimer->lpPrev = p->lpPrev;
	p->lpPrev = lpTimer;
	lpTimer->lpPrev->lpNext = lpTimer;

	UnlockIRQRestore( &uiSave );//�ָ�
}

// ********************************************************************
//������int _DelTimerList( LPTIMERLIST lpTimer ) 
//������
//	IN lpTimer - TIMERLIST�ṹָ��
//����ֵ��
//	����ɹ�������1�����򣬷���0
//����������
//	��_AddTimerList��Ӧ���ù��ܴ�timerHead�����Ƴ�һ����ʱ������
//���ã�
//	
// ********************************************************************

int _DelTimerList( LPTIMERLIST lpTimer )
{
	int ret = 0;
	UINT uiSave;

    LockIRQSave( &uiSave );// ���� & ���ж�

	if ( lpTimer->lpNext )	//�Ϸ��� 
	{	//��
		LPTIMERLIST lpNext;
		if ( ( lpNext = lpTimer->lpNext ) != NULL )	//�Ϸ���
		{	//�ǣ��Ƴ�
			( lpNext->lpPrev = lpTimer->lpPrev )->lpNext = lpNext;
			lpTimer->lpNext = lpTimer->lpPrev = NULL;
			ret = 1;
		}
	}

	UnlockIRQRestore( &uiSave );//�ָ�

	return ret;
}


// ********************************************************************
//������static void RunTimerList( void ) 
//������
//	��
//����ֵ��
//	��
//����������
//	������timerHead����������ж�ʱ����������е��ڵģ��Ƴ������ص���ʱ������
//���ã�
//	
// ********************************************************************

static void RunTimerList( void )
{
	LPTIMERLIST lpTimer;

	INTR_OFF();//�ص��ж�
	while( (lpTimer = timerHead.lpNext) != &timerHead && 
		   lpTimer->dwExpires <= dwJiffies ) 
	{
		// �Ƴ���remove lpTimer from list
		lpTimer->lpNext->lpPrev = lpTimer->lpPrev;
		lpTimer->lpPrev->lpNext = lpTimer->lpNext;
		lpTimer->lpNext = lpTimer->lpPrev = NULL;
		INTR_ON();//���ж�
		//���ö�ʱ������
		lpTimer->lpFunction( lpTimer->dwData );
		INTR_OFF();//�ص��ж�
	}
	INTR_ON();//���ж�
}

// ********************************************************************
//������static void UpdateThreadTimes( DWORD dwTicks, DWORD dwSystem ) 
//������
//	IN dwTicks - ϵͳʱ��Ƭ
//	IN dwSystem - ϵͳ�ڲ��Ѿ�ʹ�õ�ʱ��Ƭ
//����ֵ��
//	��
//����������
//	���µ�ǰ�߳������ĵ�ʱ��Ƭ
//���ã�
//	
// ********************************************************************

static void UpdateThreadTimes( DWORD dwTicks, DWORD dwSystem )
{
	LPTHREAD lpThread = lpCurThread;
	if( lpThread->fPolicy != THREAD_POLICY_FIFO )	//���Ȳ���ΪFIFO�� 
	{	//��FIFO���Ȳ��Բ���Ҫ�޸�ʱ��Ƭ
		int nUser = dwTicks - dwSystem;	//nUserΪ�û�ʹ�õ�ʱ��Ƭ
		if( nUser > 0 )
		{
			int nTick = lpThread->nTickCount - nUser;
			if( nTick < 0 )
			{	//�����꣬�����ص���
				lpThread->nTickCount = 0;
				bNeedResched = 1;
			}
			else
				lpThread->nTickCount = nTick; //�޸�Ϊ�µ�
			lpThread->dwTimerCount += nUser;
		}
	}
}

static DWORD dwLostTicks = 0;	//ϵͳʱ��Ƭ����ϵͳ����ʱ����ֵ��������
static DWORD dwLostTicksSystem = 0;	//��ϵͳ�ڴ�������ʱ�����ĵ�ʱ��Ƭ

// ********************************************************************
//������static void UpdateTimes(void) 
//������
//	��
//����ֵ��
//	��
//����������
//	���µ�ǰ�߳������ĵ�ʱ��Ƭ
//���ã�
//	
// ********************************************************************

static void UpdateTimes(void)
{
	DWORD dwTicks;

	dwTicks = KL_InterlockedExchange( (LPLONG)&dwLostTicks, 0 );

	if ( dwTicks ) 
	{
		DWORD dwSystem;

		dwSystem = KL_InterlockedExchange( (LPLONG)&dwLostTicksSystem, 0 );

		UpdateThreadTimes( dwTicks, dwSystem );
	}
}

// ********************************************************************
//������void DoTimer( void * lp ) 
//������
//	IN lp - ����
//����ֵ��
//	��
//����������
//	ϵͳtimer����
//���ã�
//	
// ********************************************************************

void DoTimer( void * lp )
{	
	dwJiffies++;
	dwLostTicks++;
	//UpdateTimes();


	ISR_Active( SYSINTR_TIMING );
	if ( 0 )//!IS_USER_MODE( lp ) )
    {
		dwLostTicksSystem++;
	}
}

// ********************************************************************
//������static void CALLBACK TimerISR( DWORD dwISRHandle ) 
//������
//	IN dwISRHandle - ���� ISR_RegisterServer ʱ�����ݵĲ���
//����ֵ��
//	��
//����������
//	timer�жϷ������̣�������ʱ���йص������� ��ʱ�������ѹ��ܵ�
//���ã�
//	
// ********************************************************************

static void CALLBACK TimerISR( DWORD dwISRHandle )
{
	UpdateTimes();
	RunTimerList();
}


// ********************************************************************
//������BOOL InitScheduler(void)
//������
//	��
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	��ʼ��������
//���ã�
//	call by start.c
// ********************************************************************

BOOL InitScheduler(void)
{	
	int i = sizeof( THREAD );

    memset( lpThreadList, 0, sizeof(lpThreadList) );
	memset( dwPriorityBitmap, 0, sizeof(dwPriorityBitmap) );
	uPriorityGroup = 0;
	
	memset( lppProcessSegmentSlots, 0, sizeof( lppProcessSegmentSlots ) );
	memset( lppProcessPtr, 0, sizeof( lppProcessPtr ) );
	InitKernelProcess.lpCpuPTS = AllocCPUPTS();
	//��ʼ���ں��߳�
	lpInitKernelThread->dwThreadId = (DWORD)MAKE_HANDLE( lpInitKernelThread );
	lpInitKernelThread->nCurPriority = lpInitKernelThread->nOriginPriority = IDLE_PRIORITY;
	lpCurThread = lpInitKernelThread;
    lpThreadList[IDLE_PRIORITY] = lpCurThread;
	uPriorityGroup = 0x80;
	dwPriorityBitmap[7] = 0x80000000;
	//ע��timer�жϷ�������
	ISR_RegisterServer( SYSINTR_TIMING, TimerISR, 0 );
	lpCurThread->nLockScheCount = 0;
	INIT_TIMER_LIST( &lpCurThread->timer );
	//test only
//	InitKernelProcess.akyAccessKey = -1;//
	//

#ifdef VIRTUAL_MEM
	lppProcessSegmentSlots[SHARE_SEGMENT_INDEX] = AllocSegmentIndexAndSegment(SHARE_MEM_BASE);//(DWORD)Seg_Alloc();
	//memset( &KernelSegment, 0, sizeof(KernelSegment) );
	//KernelSegment.dwSegBaseAddress = KERNEL_SEGMENT_BASE;
	//KernelSegment.lpSeg = Seg_Alloc();
	//InitKernelProcess.lpProcessSegments = &KernelSegment;
	InitKernelProcess.lpProcessSegments = AllocSegmentIndexAndSegment(KERNEL_SEGMENT_BASE);
	//InitKernelProcess.lpProcessSegments->dwSegBaseAddress = KERNEL_SEGMENT_BASE;
	

#endif

#ifdef USE_THREAD_PAGE_TABLE
	InitThreadPageTable( lpInitKernelThread );
//	lpInitKernelThread->pageTable.lpdwVirtualPageTable = ( ( ( (DWORD)lpInitKernelThread + (3 * 1024) ) + (1024-1) ) & ~(1024-1) );//ALIGN_PAGE_UP( (DWORD)lpInitKernelThread + (3 * 1024) );
//	lpInitKernelThread->pageTable.lpdwPhyPageTable = _GetPhysicalPageAdr(lpInitKernelThread->pageTable.lpdwVirtualPageTable);
//	lpInitKernelThread->pageTable.lpdwVirtualPageTable = CACHE_TO_UNCACHE(lpInitKernelThread->pageTable.lpdwVirtualPageTable);
//	RETAILMSG( 1, ("lpInitKernelThread=0x%x,V=0x%x,P=0x%x.\r\n", lpInitKernelThread, lpInitKernelThread->pageTable.lpdwVirtualPageTable, lpInitKernelThread->pageTable.lpdwPhyPageTable ) );	
#endif
	
//	RETAILMSG( 1, ("lpInitKernelThread->lpCpuPTS=0x%x.\r\n", lpCurThread->lpCpuPTS ) );	
    return TRUE;
}
