/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：优先级查找及优先级队列管理

版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <eobjtype.h>
#include <eassert.h>
#include <epcore.h>
#include <sysintr.h>
#include <coresrv.h>
#include <epalloc.h>

// 优先级映射表
// 系统将256个优先级分为8组，用uPriorityGroup表示， 
// uPriorityGroup的每个bit表示一组（0表示该组没有运行线程，1表示有）
// bRTIndexTable用于查找uPriorityGroup最高优先级组
static const BYTE bRTIndexTable[] = 
{ 
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x00 - 0x0f(15)
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x10
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   // 0x20 -
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
UINT uPriorityGroup;

DWORD dwPriorityBitmap[8];
static const DWORD bitMask[] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
                                 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
								 0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
								 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000 };

static LPTHREAD lpThreadList[256];

LPTHREAD GetHighestPriority( LPUINT lpPriority )
{
	UINT uIndex = bRTIndexTable[uPriorityGroup];
    DWORD dwBitmap = dwPriorityBitmap[uIndex];

	uIndex <<= 5;
	while( dwBitmap )
	{
		UINT bit = dwBitmap & 0xff;
		if( bit )
		{
			uIndex += dwPriorityBitmap[bit];
			if( lpPriority )
				*lpPriority = uIndex;
			return lpThreadList[uIndex];
		}
		dwBitmap >>= 8;
		uIndex += 8;
	}
	return NULL;
//	ASSERT( 0 );
}

void SetPriorityBitmap( UINT uPriority )
{
	uPriorityGroup |= bitMask[uPriority >> 5];
	dwPriorityBitmap[uPriority >> 5] |= bitMask[uPriority & 0X1F];
}

void ClearPriorityBitmap( UINT uPriority )
{
	if( ( dwPriorityBitmap[uPriority >> 5] &= ~(bitMask[uPriority & 0X1F]) ) == 0 )
	    uPriorityGroup &= ~bitMask[uPriority >> 5];
}

void AddToRunQueue( LPTHREAD lpThread )
{
    UINT uiSave;
	UINT uPri;
	LPTHREAD lpList;

	LockIRQSave( &uiSave );


	if( lpThread->nSuspendCount ) //2003-04-24:LN,增加
	{
		lpThread->dwState = THREAD_SUSPENDED;//THREAD_STOPPED; //2003-04-24:LN,增加
		goto _ADD_RET; //2003-04-24:LN,增加
	} //2003-04-24:LN,增加

	if( lpThread->lpNextRun || lpThread->lpPrevRun )
	{
		ASSERT( 0 );
		//error
		goto _ADD_RET;
	}
	uPri = lpThread->nCurPriority;
	lpList = lpThreadList[uPri];
	if( lpList )
	{
		( lpThread->lpPrevRun = lpList->lpPrevRun )->lpNextRun = lpThread;
		lpThread->lpNextRun = lpList;//&InitKernelThread;
		lpList->lpPrevRun = lpThread;
	}
	else
	{
		lpThreadList[uPri] = lpThread;
		lpThread->lpPrevRun = lpThread->lpNextRun = lpThread;
	    SetPriorityBitmap( uPri );
	}
	
	
	if( uPri < lpCurThread->nCurPriority )
		bNeedResched = 1;
	
	//	( lpThread->lpPrevRun = InitKernelThread.lpPrevRun )->lpNextRun = lpThread;
	//	lpThread->lpNextRun = &InitKernelThread;
	//  InitKernelThread.lpPrevRun = lpThread;
	
	//if( ( lpThread->nTickCounter - lpThread->nBoost <= lpCurThread->nTickCounter - lpThread->nBoost - 2 ) ||
	//	lpThread->fPolicy != SCHED_OTHER )
	//	bNeedResched = 1;

	nCountRunning++;   // add count of running thread

_ADD_RET:

	UnlockIRQRestore( &uiSave );
}

void RemoveFromRunQueue( LPTHREAD lpThread )
{
	LPTHREAD lpNext;
	LPTHREAD lpPrev;
	UINT uiSave;

	LockIRQSave( &uiSave );


	if( lpThread != &InitKernelThread )
	{
		UINT uPri = lpThread->nCurPriority;
//		LPTHREAD lpList = lpThreadList[uPri];


		lpNext = lpThread->lpNextRun;
		lpPrev = lpThread->lpPrevRun;
		
		if( lpNext == lpPrev )
		{
			ASSERT(lpNext == lpThread);
			lpThreadList[uPri] = NULL;
			ClearPriorityBitmap(uPri);
		}
		else
		{
		    lpNext->lpPrevRun = lpPrev;
		    lpPrev->lpNextRun = lpNext;
		}
		lpThread->lpNextRun = NULL;
		lpThread->lpPrevRun = NULL;
		lpThread->nBoost = 0;
		nCountRunning--; 
	}

//_REMOVE_RET:

	UnlockIRQRestore( &uiSave );
}

void MoveToEndOfRunQueue( LPTHREAD lpThread )
{
	UINT uiSave;
	LPTHREAD lpNext = lpThread->lpNextRun;
	LPTHREAD lpPrev = lpThread->lpPrevRun;
	UINT uPri = lpThread->nCurPriority;
	LPTHREAD lpList = lpThreadList[uPri];
	
	LockIRQSave( &uiSave );
	
	// 移出
	lpNext->lpPrevRun = lpPrev;
	lpPrev->lpNextRun = lpNext;
	// 加入
	lpThread->lpNextRun = lpList;//&InitKernelThread;
	lpPrev = lpList;//InitKernelThread.lpPrevRun;
	lpList->lpPrevRun = lpThread;
	lpThread->lpPrevRun = lpPrev;
	lpPrev->lpNextRun = lpThread;
	
	UnlockIRQRestore( &uiSave );
}

