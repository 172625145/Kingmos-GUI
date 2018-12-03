/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：调度管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
	2004-09-29, 修改 Schedule
    2004-08-24, 08-14的修改导致事件无法激活
    2004-08-14, __DownSemphores INTR_ON();  //2004-08-14, add by lilin
	2004-08-11, 不是用动态分配( WaitForSingleObject。WaitMultiSingleObject  ) 信号量，涉及到 semaphpor.c thread.c sche.c
    2003-08-30: timeout 清0
    2003-08-21:LN, WakeupThread,当唤醒线程时，将被唤醒的
	           优先级提高一个点。
    2003-04-24:LN,增加supend and resume 功能
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

//	重调度标志
BOOL bNeedResched = 0;
//	当前处于运行态的线程数
int nCountRunning = 0;
//	时间片
volatile DWORD dwJiffies = 0;

// 优先级映射表
// 系统将256个优先级分为8分组，用uPriorityGroup表示， 
// uPriorityGroup的每个bit表示一个分组（0表示该分组没有运行线程，1表示有）
// bRTIndexTable用于查找uPriorityGroup最高优先级分组
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

// 8个优先级分组 * 32 个优先级
static UINT uPriorityGroup;
// 256个bits, 每个DWORD表示一个分组
static DWORD dwPriorityBitmap[8];
// 用于得到数0-32在一个DWORD所对应的bits位
static const DWORD bitMask[] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
                                 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
								 0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
								 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 };

//	256级优先级线程对列，所有具有相同优先级[pri]的可运行线程都放在lpThreadList[pri]里组成链表
static LPTHREAD lpThreadList[256];


// ********************************************************************
//声明： int KL_GetCountRunning( void )
//参数：
//	无
//返回值：
//	返回当前可运行线程数
//功能描述：
//	得到系统当前可运行线程数
//引用：
//	系统API
// ********************************************************************
int KL_GetCountRunning( void )
{
	return nCountRunning;
}
/*
// ********************************************************************
//声明： int GetRuningThreadList( void )
//参数：
//	无
//返回值：
//	返回当前可运行线程数
//功能描述：
//	得到系统当前可运行线程数
//引用：
//	系统API
// ********************************************************************
LPTHREAD * GetRuningThreadList( void )
{
	return lpThreadList;
}
*/
// ********************************************************************
//声明：void __AddWaitQueue( LPWAITQUEUE * lppQueue, LPWAITQUEUE lpWait ) 
//参数：
//	IN lppQueue - LPWAITQUEUE指针，指向队列头的地址
//	IN lpWait - WAITQUEUE结构指针，需要加入的等待节点
//返回值：
//	无
//功能描述：
//	将一个等待线程加入等待队列的首位置
//引用：
//	
// ********************************************************************

static void __AddWaitQueue( LPWAITQUEUE * lppQueue, LPWAITQUEUE lpWait )
{
	UINT uiSave;
	LPWAITQUEUE lpHead;
	LPWAITQUEUE lpNext;
		
	LockIRQSave( &uiSave ); // 保存 & 关中断
	
	lpHead = *lppQueue;
	
	if ( lpHead )
		lpNext = lpHead;
	else
	    lpNext = WAIT_QUEUE_HEAD( lppQueue );

	//加入首位置
	*lppQueue = lpWait;
	lpWait->lpNext = lpNext;
	
	UnlockIRQRestore( &uiSave );  // 恢复
}

// ********************************************************************
//声明：void __RemoveWaitQueue( LPWAITQUEUE *lppQueue, LPWAITQUEUE lpWait ) 
//参数：
//	IN lppQueue - LPWAITQUEUE指针，指向队列头的地址
//	IN lpWait - WAITQUEUE结构指针，需要移出的等待节点
//返回值：
//	无
//功能描述：
//	将一个等待线程移出等待队列
//引用：
//	
// ********************************************************************

void __RemoveWaitQueue( LPWAITQUEUE *lppQueue, LPWAITQUEUE lpWait )
{
	UINT uiSave;
	LPWAITQUEUE lpNext;
	LPWAITQUEUE lpHead;
	
	LockIRQSave( &uiSave );// 保存 & 关中断

	lpNext = lpWait->lpNext;
	lpHead = lpNext;
	
	while( 1 ) 
	{
		LPWAITQUEUE lpNextList = lpHead->lpNext;
		if ( lpNextList == lpWait )
			break;  // 找到
		lpHead = lpNextList;
	}
	//移出
	lpHead->lpNext = lpNext;

	UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：LPTHREAD GetHighestPriority( LPUINT lpPriority ) 
//参数：
//	OUT lpPriority - 用于接受当前最高优先级的级数（可以为NULL）
//返回值：
//	返回线程指针
//功能描述：
//	得到当前最高优先级线程
//引用：
//	
// ********************************************************************

static LPTHREAD GetHighestPriority( LPUINT lpPriority )
{
	UINT uIndex;
    DWORD dwBitmap;
	LPTHREAD lpThread = NULL;
    UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断
	//得到最高分组
	uIndex = bRTIndexTable[uPriorityGroup];
	//得到该分组的优先级位图
	dwBitmap = dwPriorityBitmap[uIndex];
	//该分组的第一个优先级基
	uIndex <<= 5;
	while( dwBitmap )
	{
		UINT bit = dwBitmap & 0xff; // 8个优先级
		if( bit )
		{	//存在，得到最高的
			uIndex += bRTIndexTable[bit];
			if( lpPriority )
				*lpPriority = uIndex;
			lpThread = lpThreadList[uIndex];
			break;
		}
		//没有，取下一个八位
		dwBitmap >>= 8;//
		uIndex += 8;
	}

    UnlockIRQRestore( &uiSave );// 恢复
	return lpThread;
}

// ********************************************************************
//声明：LPTHREAD GetHigherPriority( UINT uiHighestPriority, LPUINT lpPriority ) 
//参数：
//	IN uiHighestPriority - 最高优先级
//	OUT lpPriority - 用于接受次优先级的级数
//返回值：
//	假如成功，返回当前次高优先级线程指针，*lpPriority为次高优先级级数；假如没有，返回NULL
//功能描述：
//	得到当前次高优先级线程及其级数
//引用：
//	
// ********************************************************************

static LPTHREAD GetHigherPriority( UINT uiHighestPriority, LPUINT lpPriority )
{
	LPTHREAD lpThread;
    UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断

	// 清除最高优先级
	ClearPriorityBitmap( uiHighestPriority );
	// 得到最高优先级
    lpThread = GetHighestPriority( lpPriority );
	// 恢复最高优先级
	SetPriorityBitmap( uiHighestPriority );

    UnlockIRQRestore( &uiSave );// 恢复
	return lpThread;
}

// ********************************************************************
//声明：static void SetPriorityBitmap( UINT uPriority ) 
//参数：
//	IN uPriority - 优先级
//返回值：
//	无
//功能描述：
//	设置优先级位图
//引用：
//	
// ********************************************************************

static void SetPriorityBitmap( UINT uPriority )
{
    UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断
	// 设置分组
	uPriorityGroup |= bitMask[uPriority >> 5];
	//	设置该组的位图
	dwPriorityBitmap[uPriority >> 5] |= bitMask[uPriority & 0X1F];

    UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：void ClearPriorityBitmap( UINT uPriority ) 
//参数：
//	IN uPriority - 优先级
//返回值：
//	无
//功能描述：
//	清除优先级位图
//引用：
//	
// ********************************************************************

static void ClearPriorityBitmap( UINT uPriority )
{
    UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断
	
	if( ( dwPriorityBitmap[uPriority >> 5] &= ~(bitMask[uPriority & 0X1F]) ) == 0 )
	    uPriorityGroup &= ~bitMask[uPriority >> 5];

    UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：void SetThreadCurrentPriority( LPTHREAD lpThread, UINT lPriority ) 
//参数：
//	IN lpThread - THREAD 结构指针
//	IN lPriority - 优先级
//返回值：
//	无
//功能描述：
//	设置线程的当前优先级
//引用：
//	
// ********************************************************************

void SetThreadCurrentPriority( LPTHREAD lpThread, UINT lPriority )
{
    UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断
//    ASSERT( lpThread != lpInitKernelThread );
	if( lpThread->dwState == THREAD_RUNNING )
	{	//可运行线程
		//从运行队列先移出
		RemoveFromRunQueue( lpThread );
		//设置新的优先级并将其放入运行队列
		lpThread->nCurPriority = lPriority;
		AddToRunQueue( lpThread );
	}
	else
		lpThread->nCurPriority = lPriority;
	
	UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：void AddToRunQueue( LPTHREAD lpThread ) 
//参数：
//	IN lpThread - THREAD结构指针
//返回值：
//	无
//功能描述：
//	将线程加入运行队列
//引用：
//	
// ********************************************************************

void AddToRunQueue( LPTHREAD lpThread )
{
    UINT uiSave;
	UINT uPri;
	LPTHREAD lpList;

	LockIRQSave( &uiSave );// 保存 & 关中断


	if( lpThread->nSuspendCount ) //2003-04-24:LN,增加
	{  //仍然挂起
		lpThread->dwState = THREAD_SUSPENDED;//2003-04-24:LN,增加
		goto _ADD_RET; //2003-04-24:LN,增加
	}  //2003-04-24:LN,增加

	if( lpThread->lpNextRun || lpThread->lpPrevRun )
	{	
		goto _ADD_RET;
	}
	//得到当前优先级所在的可运行对列
	uPri = lpThread->nCurPriority;
	lpList = lpThreadList[uPri];
	//放到队列头
	if( lpList )
	{	
		( lpThread->lpPrevRun = lpList->lpPrevRun )->lpNextRun = lpThread;
		lpThread->lpNextRun = lpList;
		lpList->lpPrevRun = lpThread;
	}
	else
	{	//
		lpThread->lpPrevRun = lpThread->lpNextRun = lpThread;
		//设置优先级位图
	    SetPriorityBitmap( uPri );
	}
	lpThreadList[uPri] = lpThread;	
	//是否请求重调度	
	if( uPri <= lpCurThread->nCurPriority )
		bNeedResched = 1;//是
	nCountRunning++;   // 增加运行线程数

_ADD_RET:

	UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：void RemoveFromRunQueue( LPTHREAD lpThread ) 
//参数：
//	IN lpThread - THREAD结构指针
//返回值：
//	无
//功能描述：
//	将线程移出运行队列
//引用：
//	
// ********************************************************************

void RemoveFromRunQueue( LPTHREAD lpThread )
{
	LPTHREAD lpNext;
	LPTHREAD lpPrev;
	UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断
     
	if( lpThread != lpInitKernelThread )
	{	//
		UINT uPri = lpThread->nCurPriority;

		lpNext = lpThread->lpNextRun;
		lpPrev = lpThread->lpPrevRun;
		//ASSERT( lpNext && lpPrev );
		
		//移出
		if( lpNext == lpThread )
		{	//该优先级队列仅仅一个
			//ASSERT(lpPrev == lpThread);
			lpThreadList[uPri] = NULL;
			//清除优先级位图
			ClearPriorityBitmap(uPri);
		}
		else
		{	//是否在队列头 ？
		    if( lpThreadList[uPri] == lpThread )
			{	//是
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
	{	//决不应该到这里！！！
		ASSERT(0);
	}

	UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：void MoveToEndOfRunQueue( LPTHREAD lpThread ) 
//参数：
//	IN lpThread - THREAD结构指针
//返回值：
//	无
//功能描述：
//	将线程移到本运行队列尾部
//引用：
//	
// ********************************************************************

void MoveToEndOfRunQueue( LPTHREAD lpThread )
{
	UINT uiSave;
	LPTHREAD lpNext;
	LPTHREAD lpPrev;
	UINT uPri;
	LPTHREAD lpList;
	
	LockIRQSave( &uiSave );// 保存 & 关中断

	lpNext = lpThread->lpNextRun;
	lpPrev = lpThread->lpPrevRun;
	uPri = lpThread->nCurPriority;
	lpList = lpThreadList[uPri];
	//是否在队列头 ？
    if( lpList == lpThread )
	{	//是
		lpThreadList[uPri] = lpNext;
	}

	// 移出
	lpNext->lpPrevRun = lpPrev;
	lpPrev->lpNextRun = lpNext;
	// 放到尾部
	lpThread->lpNextRun = lpList;
	lpPrev = lpList;
	lpList->lpPrevRun = lpThread;
	lpThread->lpPrevRun = lpPrev;
	lpPrev->lpNextRun = lpThread;
	
	UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：void _LinkThread( LPTHREAD lpThread ) 
//参数：
//	IN lpThread - THREAD结构指针
//返回值：
//	无
//功能描述：
//	将线程加入到线程双链表
//引用：
//	
// ********************************************************************

void _LinkThread( LPTHREAD lpThread )
{
	UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断

    // 连接到链表
	lpThread->lpNextThread = lpInitKernelThread;	
	lpThread->lpPrevThread = lpInitKernelThread->lpPrevThread;

	lpInitKernelThread->lpPrevThread->lpNextThread = lpThread;
	lpInitKernelThread->lpPrevThread = lpThread;

	UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：void _RemoveThread( LPTHREAD lpThread ) 
//参数：
//	IN lpThread - THREAD结构指针
//返回值：
//	无
//功能描述：
//	与_LinkThread对应，该功能将线程从线程双链表移出
//引用：
//	
// ********************************************************************

void _RemoveThread( LPTHREAD lpThread )
{
	UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断

	// 移出系统链表
	ASSERT( lpThread->dwMagic == THREAD_MAGIC );

	lpThread->lpNextThread->lpPrevThread = lpThread->lpPrevThread;
	lpThread->lpPrevThread->lpNextThread = lpThread->lpNextThread;

   
	UnlockIRQRestore( &uiSave );// 恢复
}

// ********************************************************************
//声明：void _WakeupThread( LPTHREAD lpThread, BOOL bHighLevel ) 
//参数：
//	IN lpThread - THREAD结构指针
//	IN bHighLevel - 是否增加一个点，假如TRUE，则增加nBoost一个点
//返回值：
//	无
//功能描述：
//	唤醒线程
//引用：
//	
// ********************************************************************

void _WakeupThread( LPTHREAD lpThread, BOOL bHighLevel )
{
	UINT uiSave;

	LockIRQSave( &uiSave );// 保存 & 关中断

	lpThread->dwState = THREAD_RUNNING;


	if ( !lpThread->lpNextRun )
	{   
		if( bHighLevel )
			lpThread->nBoost++;  //临时增加 1 tick
		//加入到可运行对列
	    AddToRunQueue( lpThread );
	}

	UnlockIRQRestore( &uiSave );// 恢复
}


// ********************************************************************
//声明：static void ThreadTimeout( DWORD dwData ) 
//参数：
//	IN dwData - THREAD结构指针
//返回值：
//	无
//功能描述：
//	睡眠到期回调函数。当线程睡眠到期时，会调用该函数去唤醒该线程
//引用：
//	
// ********************************************************************
 
static void ThreadTimeout( DWORD dwData )
{
	((LPTHREAD)dwData)->dwTimeout = 0;
	if( ((LPTHREAD)dwData)->dwState == THREAD_INTERRUPTIBLE ||
		((LPTHREAD)dwData)->dwState == THREAD_UNINTERRUPTIBLE )
	    _WakeupThread( (LPTHREAD)dwData, FALSE );//唤醒该线程
	else
	{
		;
	}
}


// ********************************************************************
//声明： static LPTHREAD GetNextRun( LPTHREAD lpPrev, DWORD dwTimeout )
//参数：
//	IN lpPrev  - THREAD结构指针
//	IN dwTimeout - 前一个线程是否是需要睡眠
//返回值：
//	THREAD指针
//功能描述：
//	得到需要调度的线程。该功能遍历所有的可运行线程并返回优先级最高的一个或
//	同级优先级中最需要运行的一个
//引用：
//	
// ********************************************************************

static LPTHREAD GetNextRun( LPTHREAD lpPrev, DWORD dwTimeout )
{
	{		
		LONG lCount;
		LPTHREAD p, lpNext;
		LPTHREAD lpHeadList = GetHighestPriority( NULL );//得到最高优先级队列
		
		//ASSERT( lpHeadList );
		//先初始化一个
		lCount = lpHeadList->nTickCount + lpHeadList->nBoost;
		p = lpHeadList->lpNextRun;
		lpNext = lpHeadList;
		//遍历同级优先级的所有线程
		while( p != lpHeadList )
		{
			LONG lCurCount;
			lCurCount = p->nTickCount + p->nBoost;
			if( lCurCount  )
			{	
				if( p == lpPrev )	//是否是前一个？
				{	//是
					if( dwTimeout )	//前一个是否需要睡眠
						lCurCount = -1000;  //是，不需要运行
					else
						lCurCount++;  //否，提高一个点
				}
			}
			//选择具有最大的lCurCount值的线程
			if( lCount < lCurCount )
			{	
				lCount = lCurCount;
				lpNext = p;
			}
			p = p->lpNextRun;
		}
		
		// 假如本优先级的所有线程的counter值都为0，则重新初始化他们
		if( !lCount )
		{	//都为0
			p = lpHeadList;
			do{
			    p->nTickCount = (short)p->nRotate;
				p = p->lpNextRun;
			}while( p != lpHeadList );
		}
		// 如果本优先级没有任何线程，立即返回，因此以下代码去掉
		//if( lpNext == lpPrev && dwTimeout )
		//{   // 本优先级的所有线程都不需要CPU,而前一个线程必须放弃一次CPU
			// 所以让次优先级的线程去运行
        //   lpNext = GetHigherPriority( lpNext->nCurPriority, NULL );
		//	ASSERT( lpNext != lpPrev );
		//}

		if( lpNext->nBoost ) //假如可能，减去临时爆发值
			lpNext->nBoost--;

	
		return lpNext;
	}
}

// ********************************************************************
//声明： void CALLBACK Schedule(void)
//参数：
//	无
//返回值：
//	无
//功能描述：
//	调度函数。当线程需要放弃CPU或调度时间片到期时会调用该函数来选择一个新的线程去运行
//引用：
//	
// ********************************************************************

void CALLBACK Schedule(void)
{

	LPTHREAD lpPrev, lpNext;
	DWORD dwTimeout = 0;

	INTR_OFF();	//关掉中断  
//2004-09-29, remove by lilin, 因为 ISR_Handler 会打开中断 并且该功能在 DefaultHandler 也有处理
	
	if( iISRActiveCount )	//是否有激活的中断例程
	{   // 是处理它们
		ISR_Handler( ISR_ALL_INTRS );
	}
//2004-09-29	
	
	//
	bNeedResched = 0;
	lpPrev = lpCurThread;

	// 轮转策略
	if( lpPrev->nTickCount != 0 && lpPrev->fPolicy == THREAD_POLICY_ROTATION )
	{	//
		lpPrev->nTickCount = (short)lpPrev->nRotate;
		//移到对列尾
		MoveToEndOfRunQueue( lpPrev );
	}

//2004-09-29, remove by lilin, 因为 HandleSignal 会打开中断
//我将其放到末尾，今后我觉得应该同时 DefaultHandler
	if( lpPrev->dwSignal & ~lpPrev->dwBlocked )	//是否有信号
	{	//是，处理信号
		HandleSignal();
	}
//2004-09-29
	//检查当前线程
	switch( lpPrev->dwState )
	{
		case THREAD_INTERRUPTIBLE:
			if( lpPrev->dwSignal & ~lpPrev->dwBlocked )
			{
				lpPrev->dwState = THREAD_RUNNING;
				break;
			}
			if( lpPrev->dwTimeout && lpPrev->dwTimeout <= dwJiffies )
			{   // continue, but 放弃一次CPU
				dwTimeout = lpPrev->dwTimeout;
				lpPrev->dwTimeout = 0;
				lpPrev->nBoost = 0;
				lpPrev->dwState = THREAD_RUNNING;
				break;
			}
		default:
            RemoveFromRunQueue( lpPrev );//移出可运行队列
			break;
		case THREAD_RUNNING:
			break;
	}
	//遍历所有可运行线程，得到一个最优的线程去运行
	lpNext = GetNextRun( lpPrev, dwTimeout );

	if( lpPrev != lpNext )
	{
	    //lpCurThread = lpNext;
//#ifdef VIRTUAL_MEM
		//假如有MMU管理，设置新的进程空间
		if( lpPrev->lpCurProcess != lpNext->lpCurProcess ||
			lpPrev->akyAccessKey != lpNext->akyAccessKey )
		    GetMMUContext( lpNext, 0, lpNext->lpCurProcess );
//#endif
		//切换
		SwitchTo( lpPrev, lpNext );
	}

	INTR_ON();

	return;
}


// ********************************************************************
//声明：static void WakeupWaitQueue( LPWAITQUEUE *lppQueue ) 
//参数：
//	IN lppQueue - LPWAITQUEUE指针
//返回值：
//	无
//功能描述：
//	唤醒所有在该队列的等待线程
//引用：
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
	

	LockIRQSave( &uiSave );// 保存 & 关中断

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
				_WakeupThread( lpThread, TRUE ); //唤醒他
			}
		}
		if ( !lpNext )
		{
			RETAILMSG( 1, ("error at WakeupWaitQueue: lpNext==NULL!.\r\n" ) );
			break;
		}
	}

	UnlockIRQRestore( &uiSave );//恢复
	return;
}

// ********************************************************************
//声明：void __Up( LPSEMAPHORE lpsem ) 
//参数：
//	IN lpsem - SEMAPHORE结构指针
//返回值：
//	无
//功能描述：
//	释放等待信号量的线程
//引用：
//	
// ********************************************************************

void __Up( LPSEMAPHORE lpsem )
{
	WakeupWaitQueue( &lpsem->lpWaitQueue );
}

// ********************************************************************
//声明：DWORD __DownSemphores( LPSEMAPHORE * lppsem, DWORD nCount, DWORD dwTimeout ) 
//参数：
//	IN lppsem - LPSEMAPHORE指针数组
//	IN nCount - 保存在LPSEMAPHORE指针数组里的指针数 <= MAX_WAITOBJS
//	IN dwTimeout - 超时等待时间（已毫秒为单位）
//返回值：
//	假如成功，返回 WAIT_OBJECT_0+n （假如有一个对象有信号） 或 WAIT_TIMEOUT（没有任何对象有信号并且等待时间到）；
//	否则，返回 WAIT_FAILED
//功能描述：
//	
//引用：
//	请求信号量
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

	//2004-08-11，如果用动态分配的方法，则需要在HandleThreadExit做对应的释放处理
	//lpWaitQueue = &waitQueue[0];
	lpWaitQueue = (WAITQUEUE *)KHeap_Alloc( nCount * sizeof( WAITQUEUE ) );
	if( lpWaitQueue == NULL )
		return WAIT_FAILED;
	
	INTR_OFF();

	lpThread->nsemRet = WAIT_OBJECT_0;

    //	初始化每一个等待队列
	for( i = 0; i < nCount; i++ )
	{
		lpWaitQueue[i].lpThread = lpThread;
		lpWaitQueue[i].lpNext = NULL;
	    __AddWaitQueue( &lppsem[i]->lpWaitQueue, &lpWaitQueue[i] );
		lpThread->lppsemWait[i] = lppsem[i];	//2005-01-29, add
	}
	//	设定该线程的等待事件
	//lpThread->lppsemWait = lppsem; //2005-01-29, remove
	lpThread->nWaitCount = (WORD)nCount;
    lpThread->lpWaitQueue = lpWaitQueue;
	if( dwTimeout == INFINITE )
	{	//无超时要求，死等
		lpThread->dwState = THREAD_UNINTERRUPTIBLE;
	}
	else
	{  // 需要超时处理，这里增加一个timer
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
	{	//是否有信号？
		for( i = 0; i < nCount; i++ )
		{
			if( lppsem[i]->nCount )
			{
				dwRetv = WAIT_OBJECT_0 + i;//有，返回
				while( i-- )
				{
					lppsem[i]->nWaiting--;  // 清除之前增加的等待计数
				}
				lpThread->nsemRet = (WORD)dwRetv;
				goto _return;
			}			
			lppsem[i]->nWaiting++;
		}
		// 没有任何对象有信号
		
		if( dwTimeout != INFINITE && lpCurThread->dwTimeout == 0 )
		{// 需要超时处理但超时时间为0，返回
			i = nCount;
			while( i-- )
			{
				lppsem[i]->nWaiting--;  // 清除之前增加的等待计数
			}
			dwRetv = WAIT_TIMEOUT;
			break;
		}

		//放弃CPU并等待信号
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
					dwRetv = WAIT_OBJECT_0 + i;//有，返回
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
	{	//删除timer
		_DelTimerList(&lpCurThread->timer);

	}
	

    //清除
	for( i = 0; i < nCount; i++ )
	{
	    __RemoveWaitQueue( &lppsem[i]->lpWaitQueue, &lpWaitQueue[i] );
	}

	KHeap_Free( lpWaitQueue, nCount * sizeof( WAITQUEUE ) );  

	

//	ASSERT( lpThread->lpPrevRun && lpThread->lpNextRun );

	

	return dwRetv;
}

// ********************************************************************
//声明：DWORD ScheduleTimeout( DWORD dwTimeout ) 
//参数：
//	IN dwTimeout - 延迟时间（以毫秒为单位）
//返回值：
//	返回实际等待的时间
//功能描述：
//	让线程延迟一段时间
//引用：
//	
// ********************************************************************

DWORD ScheduleTimeout( DWORD dwTimeout )
{
	//TIMERLIST timer;
	DWORD dwExpire;
	DWORD dwNeedJiffies;

	if( dwTimeout == INFINITE )
	{	//死等
		lpCurThread->dwState = THREAD_UNINTERRUPTIBLE;
		Schedule();
		return dwTimeout;		
	}
	else
	{	//
	    dwNeedJiffies = OEM_TimeToJiffies( dwTimeout, 0 );  // 得到毫秒数对应的时间片

	    INTR_OFF();
		//初始化timer
		dwExpire = dwNeedJiffies + dwJiffies;
		lpCurThread->dwTimeout = dwExpire;
		// 2004-05-13， lilin,如果时间太短，无法实现Sleep的功能
		// 导致其它线程得不到时间
		//lpCurThread->dwState = THREAD_INTERRUPTIBLE;
		if( dwTimeout )
		{	//必须切换（从运行线程中移出）
			lpCurThread->dwState = THREAD_UNINTERRUPTIBLE;
		}
		else			
		{	//如果同优先级的线程存在，则切换到同优先级的其它线程
			//否则直接返回
		    lpCurThread->dwState = THREAD_INTERRUPTIBLE;
		}
		//

		INIT_TIMER_LIST( &lpCurThread->timer );
		lpCurThread->timer.dwExpires = dwExpire;
		lpCurThread->timer.dwData = (DWORD)lpCurThread;
		lpCurThread->timer.lpFunction = ThreadTimeout;//唤醒线程的回调函数
		//插入timer列式
		_AddTimerList(&lpCurThread->timer);
		//lpCurThread->lpTimer = &timer;

	    INTR_ON();
		//放弃CPU
		Schedule();

		//移出
		_DelTimerList(&lpCurThread->timer);

		//lpCurThread->lpTimer = NULL;
		//实际超时
		dwTimeout = dwExpire - dwJiffies;
	}
	return dwTimeout;
}

static TIMERLIST timerHead = { &timerHead, &timerHead, ~0, 0, NULL };
// ********************************************************************
//声明：void _AddTimerList( LPTIMERLIST lpTimer ) 
//参数：
//	IN lpTimer - TIMERLIST结构指针
//返回值：
//	无
//功能描述：
//	将一个定时器对象插入timerHead链表
//引用：
//	
// ********************************************************************

void _AddTimerList( LPTIMERLIST lpTimer )
{
	LPTIMERLIST p;
	UINT uiSave;    

	p = &timerHead;
    LockIRQSave( &uiSave );// 保存 & 关中断
	//按定时时间从小到大连接
	do {
		p = p->lpNext;
	} while ( lpTimer->dwExpires > p->dwExpires );

	lpTimer->lpNext = p;
	lpTimer->lpPrev = p->lpPrev;
	p->lpPrev = lpTimer;
	lpTimer->lpPrev->lpNext = lpTimer;

	UnlockIRQRestore( &uiSave );//恢复
}

// ********************************************************************
//声明：int _DelTimerList( LPTIMERLIST lpTimer ) 
//参数：
//	IN lpTimer - TIMERLIST结构指针
//返回值：
//	假如成功，返回1；否则，返回0
//功能描述：
//	与_AddTimerList对应，该功能从timerHead链表移出一个定时器对象
//引用：
//	
// ********************************************************************

int _DelTimerList( LPTIMERLIST lpTimer )
{
	int ret = 0;
	UINT uiSave;

    LockIRQSave( &uiSave );// 保存 & 关中断

	if ( lpTimer->lpNext )	//合法？ 
	{	//是
		LPTIMERLIST lpNext;
		if ( ( lpNext = lpTimer->lpNext ) != NULL )	//合法？
		{	//是，移出
			( lpNext->lpPrev = lpTimer->lpPrev )->lpNext = lpNext;
			lpTimer->lpNext = lpTimer->lpPrev = NULL;
			ret = 1;
		}
	}

	UnlockIRQRestore( &uiSave );//恢复

	return ret;
}


// ********************************************************************
//声明：static void RunTimerList( void ) 
//参数：
//	无
//返回值：
//	无
//功能描述：
//	遍历在timerHead链表里的所有定时器对象，如果有到期的，移出它并回调定时器功能
//引用：
//	
// ********************************************************************

static void RunTimerList( void )
{
	LPTIMERLIST lpTimer;

	INTR_OFF();//关掉中断
	while( (lpTimer = timerHead.lpNext) != &timerHead && 
		   lpTimer->dwExpires <= dwJiffies ) 
	{
		// 移出，remove lpTimer from list
		lpTimer->lpNext->lpPrev = lpTimer->lpPrev;
		lpTimer->lpPrev->lpNext = lpTimer->lpNext;
		lpTimer->lpNext = lpTimer->lpPrev = NULL;
		INTR_ON();//打开中断
		//调用定时器功能
		lpTimer->lpFunction( lpTimer->dwData );
		INTR_OFF();//关掉中断
	}
	INTR_ON();//打开中断
}

// ********************************************************************
//声明：static void UpdateThreadTimes( DWORD dwTicks, DWORD dwSystem ) 
//参数：
//	IN dwTicks - 系统时间片
//	IN dwSystem - 系统内部已经使用的时间片
//返回值：
//	无
//功能描述：
//	更新当前线程所消耗的时间片
//引用：
//	
// ********************************************************************

static void UpdateThreadTimes( DWORD dwTicks, DWORD dwSystem )
{
	LPTHREAD lpThread = lpCurThread;
	if( lpThread->fPolicy != THREAD_POLICY_FIFO )	//调度策略为FIFO吗？ 
	{	//否，FIFO调度策略不需要修改时间片
		int nUser = dwTicks - dwSystem;	//nUser为用户使用的时间片
		if( nUser > 0 )
		{
			int nTick = lpThread->nTickCount - nUser;
			if( nTick < 0 )
			{	//消耗完，请求重调度
				lpThread->nTickCount = 0;
				bNeedResched = 1;
			}
			else
				lpThread->nTickCount = nTick; //修改为新的
			lpThread->dwTimerCount += nUser;
		}
	}
}

static DWORD dwLostTicks = 0;	//系统时间片，当系统启动时，该值不断增加
static DWORD dwLostTicksSystem = 0;	//当系统在处理事务时所消耗的时间片

// ********************************************************************
//声明：static void UpdateTimes(void) 
//参数：
//	无
//返回值：
//	无
//功能描述：
//	更新当前线程所消耗的时间片
//引用：
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
//声明：void DoTimer( void * lp ) 
//参数：
//	IN lp - 保留
//返回值：
//	无
//功能描述：
//	系统timer处理
//引用：
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
//声明：static void CALLBACK TimerISR( DWORD dwISRHandle ) 
//参数：
//	IN dwISRHandle - 调用 ISR_RegisterServer 时所传递的参数
//返回值：
//	无
//功能描述：
//	timer中断服务例程，处理与时间有关的事务，如 定时器，唤醒功能等
//引用：
//	
// ********************************************************************

static void CALLBACK TimerISR( DWORD dwISRHandle )
{
	UpdateTimes();
	RunTimerList();
}


// ********************************************************************
//声明：BOOL InitScheduler(void)
//参数：
//	无
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	初始化调度器
//引用：
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
	//初始化内核线程
	lpInitKernelThread->dwThreadId = (DWORD)MAKE_HANDLE( lpInitKernelThread );
	lpInitKernelThread->nCurPriority = lpInitKernelThread->nOriginPriority = IDLE_PRIORITY;
	lpCurThread = lpInitKernelThread;
    lpThreadList[IDLE_PRIORITY] = lpCurThread;
	uPriorityGroup = 0x80;
	dwPriorityBitmap[7] = 0x80000000;
	//注册timer中断服务例程
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
