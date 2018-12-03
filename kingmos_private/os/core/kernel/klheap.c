/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：程序堆管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
    2003-05-14: 将API调用改为 Kernerl调用, By ln
    2003-05-13: 增加 互斥段代码, By ln
    2003-05-12: HeapAlloc 增加 HEAP_ZERO_MEMORY , By ln
    2003-05-09: LN, 将堆的必须限制改为可选择限制, By ln
******************************************************/


#include <eframe.h>
#include <epheap.h>
#include <coresrv.h>

#include <epalloc.h>

#define DISABLE()
#define ENABLE()


#define BUSY_FLAG 1		/* use this define for the BUSY_FLAG bit in lpNext */
#define ALLOC_FLAG 2      

#define IS_BUSY(x)	((long)(x)->busy.lpNext&BUSY_FLAG)
#define IS_ALLOC(x)	((long)(x)->busy.lpNext&ALLOC_FLAG)

// 对齐8bytes
#define ALIGNMASK (7)//sizeof(char*)<=2? 1: (sizeof(char*)<=4? 3: 7))

// 
static void _InsertToFree( LPHEAP, LPBLOCK );
static void _RemoveFromFree( LPHEAP, LPBLOCK );
static BOOL Heap_AddToSection( LPHEAP lpHeap, LPSECTION lp, ULONG ulLen );

// 互斥操作，进入
#define EnterCriticalLock( lpHeap, bSerial ) \
	do{ \
 	    if( (bSerial) && ( (lpHeap)->dwOption & HEAP_NO_SERIALIZE ) == 0 ) \
		{ \
			KL_EnterCriticalSection( &(lpHeap)->csHeap );\
		} \
	}while(0);

// 互斥操作，离开
#define LeaveCriticalLock( lpHeap, bSerial ) \
	do{ \
 	    if( (bSerial) && ( (lpHeap)->dwOption & HEAP_NO_SERIALIZE ) == 0 ) \
		{ \
			KL_LeaveCriticalSection( &(lpHeap)->csHeap );\
		} \
	}while(0);


#define PAGE_SIZE (4*1024)
#define ALLOC_SIZE (64*1024)

/**************************************************
声明：BOOL Heap_Init( LPHEAP lpHeap, DWORD dwOption, ULONG ulInitialSize, ULONG ulLimit )
参数：
	lpHeap - HEAP 结构指针，用于保存堆信息
	dwOption - 附加的功能，可以为以下值：
		HEAP_NO_SERIALIZE - 以后的HeapAlloc, HeapFree等功能不需要串行化（互斥操作）
		HEAP_VIRTUAL_ALLOC - 从系统内存空间分配内存的方法，该值说明用VirtualAlloc功能从系统分配；
		                     否则，用DoAllocPageMem功能从系统分配
	ulInitialSize - 初始化堆尺寸
	ulLimit - 堆尺寸最大限制
返回值：
	假如成功，TRUE；否则，返回FALSE
功能描述：
	初始化一个堆，以便与于以后的堆操作
引用: 
	
************************************************/
BOOL Heap_Init( LPHEAP lpHeap, DWORD dwOption, ULONG ulInitialSize, ULONG ulLimit )
{
	if( ulInitialSize > ulLimit )
		return FALSE;
	memset( lpHeap, 0, sizeof( HEAP ) );
	if( (dwOption & HEAP_NO_SERIALIZE) == 0 )
	{
		KL_InitializeCriticalSection( &lpHeap->csHeap );
	}
	lpHeap->dwOption = dwOption;
	ulInitialSize = (ulInitialSize + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));
	ulLimit = (ulLimit + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));
	lpHeap->dwHeapSizeLimit = ulLimit;

#ifdef VIRTUAL_MEM
	if( dwOption & HEAP_VIRTUAL_ALLOC )
	{
		BOOL bRetv = TRUE;
		
		if( ulLimit || ulInitialSize )
		{   // 预留内存
			DWORD dwReverse = MAX( ulLimit, ulInitialSize );
			LPVOID lpv;
			
			lpv = KL_VirtualAlloc( 0, dwReverse, MEM_RESERVE, PAGE_READWRITE );
			
			if( lpv )
			{
				lpHeap->lpbCur = lpHeap->lpbCurReserveBase = lpv;
				lpHeap->dwCurReserveSize = dwReverse;
				lpHeap->dwCurUseSize = 0;
				lpHeap->dwTotalReserveSize = dwReverse;
			}
			else
				bRetv = FALSE;
		}
		
		if( ulInitialSize && lpHeap->lpbCurReserveBase )
		{   // 提交物理内存
			DWORD dwCommitSize;
			LPVOID p;
			
			ulInitialSize = MAX( ulInitialSize, sizeof( SECTION ) );
			
			dwCommitSize = (ulInitialSize + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));
			p = KL_VirtualAlloc( lpHeap->lpbCur, dwCommitSize, MEM_COMMIT, PAGE_READWRITE );
			if( p == NULL )
			{
				bRetv = FALSE;
				KL_VirtualFree( lpHeap->lpbCurReserveBase, 0, MEM_RELEASE );
				lpHeap->lpbCurReserveBase = lpHeap->lpbCur = NULL;
				lpHeap->dwCurReserveSize = 0;
				lpHeap->dwTotalReserveSize = 0;
			}
			else
			{
				lpHeap->dwTotalCommitSize = dwCommitSize;
				lpHeap->lpbCur += sizeof( SECTION );
				lpHeap->dwCurUseSize += sizeof( SECTION );
				
				Heap_AddToSection( lpHeap, p, lpHeap->dwTotalReserveSize );
			}
		}
		//RETAILMSG( 1, ( "Heap_Init:lpHeap=%x.\r\n", lpHeap ) );
		return bRetv;
	}
//#else
	else
#endif   //VIRTUAL_MEM
		return TRUE;
//#endif

}

/**************************************************
声明：HANDLE DoHeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize )
参数：
	flOptions - 附加的功能，可以为以下值：
		HEAP_NO_SERIALIZE - 以后的HeapAlloc, HeapFree等功能不需要串行化（互斥操作）
		HEAP_VIRTUAL_ALLOC - 从系统内存空间分配内存的方法，该值说明用VirtualAlloc功能从系统分配；
		                     否则，用DoAllocPageMem功能从系统分配
	dwInitialSize - 初始化堆尺寸
	dwMaximumSize - 堆尺寸最大限制
返回值：
	假如成功，有效的句柄；否则，返回NULL
功能描述：
	初始化一个堆，以便与于以后的堆操作
引用: 
	process.c用该函数去初始化进程堆
************************************************/

HANDLE DoHeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize )
{
	LPHEAP lpHeap = KHeap_Alloc( sizeof( HEAP ) );
	if( lpHeap )
	{
		if( Heap_Init( lpHeap, flOptions, dwInitialSize, dwMaximumSize ) == FALSE )
		{
			KHeap_Free( lpHeap, sizeof( HEAP ) );
			lpHeap = NULL;
		}
	}
	return lpHeap;
}

/**************************************************
声明：BOOL WINAPI DoHeapDestroy( HANDLE hHeap )
参数：
	hHeap - 堆句柄（为之前调用KL_HeapCreate所返回的句柄）
返回值：
	假如成功，返回TRUE; 否则，返回FALSE
功能描述：
	破坏堆的部分数据
引用: 
	内核调用

************************************************/

BOOL DoHeapDestroy( LPHEAP lpHeap )
{
//	LPHEAP lpHeap = (LPHEAP)hHeap;

//   1.因为 用户堆的链表指针也是在用户堆里，其指针也有可能被破坏，
//		如果是这样，在调用KL_HeapDestroy有可能出现异常，所以这里不用去释放 heap
//   2.Seg 管理器会释放所有的用户空间（包括用户堆）
//   3.内核在释放进程时会调用该函数

	// 进入互斥段		
	if( lpHeap )
	{
		if( (lpHeap->dwOption & HEAP_NO_SERIALIZE) == 0 )
			KL_DeleteCriticalSection( &lpHeap->csHeap );
	       // 释放分配的堆信息结构		
		KHeap_Free( lpHeap, sizeof( HEAP ) );
		return TRUE;
	}
	return FALSE;
}


/**************************************************
声明：HANDLE WINAPI KL_HeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize )
参数：
	flOptions - 附加的功能，可以为以下值：
		HEAP_NO_SERIALIZE - 以后的HeapAlloc, HeapFree等功能不需要串行化（互斥操作）
	dwInitialSize - 初始化堆尺寸
	dwMaximumSize - 堆尺寸最大限制
返回值：
	假如成功，有效的句柄；否则，返回NULL
功能描述：
	初始化一个堆，以便与于以后的堆操作
引用: 
	系统API调用
************************************************/

HANDLE WINAPI KL_HeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize )
{
//	
#ifdef VIRTUAL_MEM	
	return DoHeapCreate( flOptions|HEAP_VIRTUAL_ALLOC, dwInitialSize, dwMaximumSize );
#else
	flOptions &= ~HEAP_VIRTUAL_ALLOC;
	return DoHeapCreate( flOptions, dwInitialSize, dwMaximumSize );
#endif	
}

/**************************************************
声明：static BOOL Heap_AddToSection( LPHEAP lpHeap, LPSECTION lp, ULONG ulLen )
参数：
	lpHeap - HEAP 结构指针，指向堆信息
	lpsc - SECTION 结构指针，向堆增加的新内存段
	ulLen - 新内存段尺寸
返回值：
	假如成功，返回TRUE；否则，返回FALSE
功能描述：
	将一个新内存段加入堆链表
引用: 
	
************************************************/
static BOOL Heap_AddToSection( LPHEAP lpHeap, LPSECTION lpsc, ULONG ulLen )
{
	// 初始化SECTION 结构
	lpsc->ulLen = ulLen;
	lpsc->lpNext = lpHeap->lpSection;
	// 加入堆链表
	lpHeap->lpSection = lpsc;
	return TRUE;
}

/**************************************************
声明：void * GetSystemMem( LPHEAP lpHeap, DWORD dwSize, DWORD * lpdwRealSize )
参数：
	lpHeap - HEAP 结构指针，指向堆信息
	dwSize - 需要的内存大小
	lpdwRealSize - 返回的真实内存大小（ > dwSize )
返回值：
	假如成功，返回非NULL的内存指针；否则，返回NULL
功能描述：
	既然堆里没有足够的内存块，则用该函数从系统内存分配内存块，并将其加入堆链表
 
引用: 
    Heap_Alloc, Heap_Realloc	
************************************************/
#define DEBUG_GETSYSTEMMEM 0
static void * GetSystemMem( LPHEAP lpHeap, DWORD dwSize, DWORD * lpdwRealSize )
{
	//RETAILMSG( 1, ( "GetSystemMem:lpHeap=%x,dwSize=%d,ProcessId=%d.\r\n", lpHeap, dwSize, KL_GetCurrentProcessId() ) );

#ifdef VIRTUAL_MEM
	// 如果系统有MMU管理，则用该段代码
    if( lpHeap->dwOption & HEAP_VIRTUAL_ALLOC )
	{   // 用VirtualAlloc去分配内存
		void * p = NULL;
		DWORD dwCommitSize;
		UINT uiNewSectionSize = 0;
		
		*lpdwRealSize = 0;
		
		dwCommitSize = (dwSize + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));  // align to page
		
		if( lpHeap->dwCurUseSize + dwCommitSize > lpHeap->dwCurReserveSize )
		{   // 当前堆里未提交（保留）的内存域 小于 需要的内存尺寸，必须分配新的内存域/段
			//dwSize += sizeof(SECTION);
			if( lpHeap->dwHeapSizeLimit )
			{   // 如果有整个堆的大小限制，则错误。
				RETAILMSG( 1, ( "error: GetSystemMem:Heap reach size limite(%d).\r\n", lpHeap->dwHeapSizeLimit ) );
				return NULL;
			}
			else
			{	// 没有尺寸限制,分配一个新的
				DWORD dwReserveSize = dwSize + sizeof(SECTION);
				// 对齐到ALLOC_SIZE大小
				dwReserveSize = (dwReserveSize + ALLOC_SIZE - 1) & (~(ALLOC_SIZE - 1));// align to 64k
				// 先保留内存域
				lpHeap->lpbCurReserveBase = KL_VirtualAlloc( 0, dwReserveSize, MEM_RESERVE, PAGE_READWRITE );
				if( lpHeap->lpbCurReserveBase == NULL )
				{   //保留内存域失败
					WARNMSG( DEBUG_GETSYSTEMMEM, ( "error: GetSystemMem: can't to reserve space size(%d).\r\n", dwReserveSize ) );
					dwReserveSize = 0;
				}
				else
					uiNewSectionSize = sizeof(SECTION);
				// 初始化新的内存域
				lpHeap->lpbCur = lpHeap->lpbCurReserveBase;
				lpHeap->dwCurReserveSize = dwReserveSize;
				lpHeap->dwCurUseSize = 0;
				lpHeap->dwTotalReserveSize += dwReserveSize;
			}
		}
		
		if( lpHeap->dwHeapSizeLimit && 
			lpHeap->dwCurUseSize == NULL )
			uiNewSectionSize = sizeof(SECTION);  // 为section保留空间
		// 真实的需要提交/分配的内存
		dwCommitSize = (dwSize + uiNewSectionSize + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));
		// 合法检查
		if( lpHeap->dwCurUseSize + dwCommitSize <= lpHeap->dwCurReserveSize )
		{   // 提交/分配新的内存块
			p = KL_VirtualAlloc( lpHeap->lpbCur, dwCommitSize, MEM_COMMIT, PAGE_READWRITE );
			if( p )
			{
#ifdef __DEBUG
				memset( p, 0xCCCCCCCC, dwCommitSize );
#endif
				
				if( uiNewSectionSize )
				{  // 需要将其加入堆链表
					Heap_AddToSection( lpHeap, (LPSECTION)p, lpHeap->dwCurReserveSize );
					p = (LPBLOCK)( (DWORD)p + uiNewSectionSize );
				}
				// 减去已分配的内存大小
				lpHeap->lpbCur += dwCommitSize;
				lpHeap->dwCurUseSize += dwCommitSize;
				lpHeap->dwTotalCommitSize += dwCommitSize;
				*lpdwRealSize = dwCommitSize - uiNewSectionSize;
				//RETAILMSG( 1, ( "GetSystemMem:*lpdwRealSize=%d,dwSize=%d,p=0x%x,lpHeap->lpbCur=%x, ProcID=%x, ThreadID=%x, CPSR=%x.\r\n",*lpdwRealSize, dwSize, p, lpHeap->lpbCur, KL_GetCurrentProcessId(), KL_GetCurrentThreadId(), KL_GetCPSR() ) );
			}
			else
			{
				WARNMSG( DEBUG_GETSYSTEMMEM, ( "error: GetSystemMem: can't to commit enough memory size(%d).\r\n", dwCommitSize ) );
			}
		}
		DEBUGMSG( DEBUG_GETSYSTEMMEM, ( "GetSystemMem:lpHeap=%x,dwTotalSize=%d,ProcessId=%d.\r\n", lpHeap, lpHeap->dwTotalCommitSize, KL_GetCurrentProcessId() ) );
		
		return p;
	}

//#else

	else

#endif
	{   // 用DoAllocPageMem去分配内存
		LPBYTE p = NULL;
		*lpdwRealSize = 0;
		dwSize += sizeof(SECTION);  // 真实的需要分配的内存大小
		
		p = DoAllocPageMem( dwSize, lpdwRealSize, 0 );
		if( p )
		{   // 将其加入堆链表
			Heap_AddToSection( lpHeap, (LPSECTION)p, *lpdwRealSize );
			lpHeap->dwTotalReserveSize  += *lpdwRealSize;
			lpHeap->dwTotalCommitSize  += *lpdwRealSize;


			p = p + sizeof(SECTION);
			*lpdwRealSize -= sizeof(SECTION);

			
		}
		return p;
	}

}

/**************************************************
声明：static BOOL Heap_MemCheck( LPHEAP lpHeap )
参数：
	lpHeap - HEAP 结构指针，指向堆信息
返回值：
	假如成功，返回TRUE; 否则，返回FALSE
功能描述：
	检查堆是否合法

引用: 
	
************************************************/

static BOOL Heap_MemCheck( LPHEAP lpHeap )
{
    LPBLOCK lpBlock, lpFirstBlock;

	// 进入互斥段
	EnterCriticalLock( lpHeap, TRUE ); 

	lpFirstBlock = lpBlock = lpHeap->lpFirstBlock;
	KL_DebugOutString( "........check heap.........\r\n" );
	while( lpBlock )
	{
		if( IS_ALLOC(lpBlock) )//&& !IS_GAP( lpBlock ) )
		{
		    ASSERT_NOTIFY(  0 , (char*)lpBlock + sizeof( BUSY ) );//"memory not free when exit ap!!" );
		}
	    lpBlock = lpBlock->busy.lpNext;// - (ALLOC_FLAG+BUSY_FLAG);
		lpBlock = (LPBLOCK) ( (DWORD)lpBlock & ( ~(ALLOC_FLAG+BUSY_FLAG) ) );
	}

	// 离开互斥段
	LeaveCriticalLock( lpHeap, TRUE );

	return TRUE;
}

/**************************************************
声明：BOOL WINAPI KL_HeapDestroy( HANDLE hHeap )
参数：
	hHeap - 堆句柄（为之前调用KL_HeapCreate所返回的句柄）
返回值：
	假如成功，返回TRUE; 否则，返回FALSE
功能描述：
	破坏堆
引用: 
************************************************/

BOOL WINAPI KL_HeapDestroy( HANDLE hHeap )
{
	LPHEAP lpHeap = (LPHEAP)hHeap;

#ifdef VIRTUAL_MEM

	// 如果系统有MMU管理，则用该段代码

    if( lpHeap->dwOption & HEAP_VIRTUAL_ALLOC )
	{   // // 用VirtualFree去释放内存
		LPSECTION lps, lpNext;

		// 进入互斥段
		EnterCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add
		
		RETAILMSG( 1, ( "Heap_Release:lpHeap=%x.lpbBase=%x.\r\n", lpHeap, lpHeap->lpbCurReserveBase ) );
		// 释放所有的内存段
		lps = lpHeap->lpSection;
		while( lps )
		{
			lpNext = lps->lpNext;
			KL_VirtualFree( lps, 0, MEM_RELEASE );
			lps = lpNext;
		}
		// 离开互斥段
		LeaveCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add
		
		return DoHeapDestroy( lpHeap );
		
//		if( (lpHeap->dwOption & HEAP_NO_SERIALIZE) == 0 )
//			KL_DeleteCriticalSection( &lpHeap->csHeap );
        // 释放分配的堆信息结构		
//		KHeap_Free( lpHeap, sizeof( HEAP ) );
		
//		return TRUE;
	}

	else

#endif

	{   // 用DoFreePageMem去释放内存		
		LPSECTION lps, lpNext;
		// 进入互斥段
		EnterCriticalLock( lpHeap, TRUE );
		// 释放所有的内存段
		lps = lpHeap->lpSection;
		while( lps )
		{
			lpNext = lps->lpNext;
			DoFreePageMem( lps, lps->ulLen );
			lps = lpNext;
		}
		// 离开互斥段
		LeaveCriticalLock( lpHeap, TRUE );
//		if( (lpHeap->dwOption & HEAP_NO_SERIALIZE) == 0 )
//			KL_DeleteCriticalSection( &lpHeap->csHeap );
		
//		KHeap_Free( lpHeap, sizeof( HEAP ) );
		return DoHeapDestroy( lpHeap );		
		
//		return TRUE;
	}

}


/**************************************************
声明：void _InsertToFree( LPHEAP lpHeap, LPBLOCK p )
参数：
	lpHeap - HEAP结构指针
	p - 内存块
返回值：
	无
功能描述：
	将一个内存块插入堆空闲链表，以便以后使用
引用: 
************************************************/

static void _InsertToFree( LPHEAP lpHeap, LPBLOCK p )
{
	p->lpPrevFree = 0;
    if ( (p->lpNextFree = lpHeap->lpFreeList) != 0 )
        lpHeap->lpFreeList->lpPrevFree = p;
    lpHeap->lpFreeList = p;
}

/**************************************************
声明：void _RemoveFromFree( LPHEAP lpHeap, LPBLOCK p )
参数：
	lpHeap - HEAP结构指针
	p - 内存块
返回值：
	无
功能描述：
	将一个内存块移出堆空闲链表，以便使用
引用: 
************************************************/
static void _RemoveFromFree( LPHEAP lpHeap, LPBLOCK p )
{
    LPBLOCK f, b;
    f = p->lpNextFree;
    b = p->lpPrevFree;
    if ( b ) 
		b->lpNextFree = f;
    else     
		lpHeap->lpFreeList = f;
    if ( f ) 
		f->lpPrevFree = b;
}

/**************************************************
声明：void * WINAPI KL_HeapAlloc( HANDLE hHeap, DWORD dwFlags, DWORD dwSizeNeeded )
参数：
	hHeap - 堆句柄
	dwFlags - 功能，可以为以下值：
		HEAP_NO_SERIALIZE - 不需要串行化（互斥操作）
	dwSizeNeeded - 需要分配的内存大小
返回值：
	假如成功，返回非NULL的指针; 否则返回NULL
功能描述：
	从堆里分配一个内存快
引用:
	系统API 
************************************************/

#define DEBUG_HEAPALLOC 0
void * WINAPI KL_HeapAlloc( HANDLE hHeap, DWORD dwFlags, DWORD dwSizeNeeded )
{
    LPBLOCK p, x, lpBigBlock;
    DWORD dwBigSize, dwSize;
    DWORD dwRealSize;
	LPHEAP lpHeap = (LPHEAP)hHeap;
	DWORD dwSave = dwSizeNeeded;

    if ( !dwSizeNeeded )
        goto M_RETNULL;

    // 对齐大小
	dwSizeNeeded = ((dwSizeNeeded + ALIGNMASK) & ~ALIGNMASK) + sizeof(BUSY);

    if ( dwSizeNeeded < sizeof(BLOCK) ) 
		dwSizeNeeded = sizeof(BLOCK);

    lpBigBlock = 0;	// 用于记录 > dwSize 的最小的一个空闲快
    dwBigSize = 0;	// 用于记录 lpBigBlock的大小

    //进入互斥段
    EnterCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add

	DEBUGMSG( DEBUG_HEAPALLOC, ( "+++M(0x%x,lock=%d,dwOption=0x%x)M+++.\r\n", KL_GetCurrentThreadId(), lpHeap->iLockCount, lpHeap->dwOption ) );

    // 查找堆空闲链表-在所有大于dwSizeNeeded的空闲块里得到一块适当的空闲块
    for ( p = lpHeap->lpFreeList; p; p = p->lpNextFree )
    {
	    if ( (dwSize = (char*)p->busy.lpNext - (char*)p) >= dwSizeNeeded )
        {   // 空闲块的尺寸大于需要的尺寸
	        if ( dwSize < dwSizeNeeded + dwSizeNeeded ) 
			{   // 假如该空闲块的尺寸小于需要的尺寸的两倍，符合要求，退出查找
				break;
			}
			// 空闲块的尺寸太大，如果符合条件（是目前得到的空闲块里最小的一块）
			// 将其保存在 lpBigBlock, dwBigSize里
	        if ( !lpBigBlock || dwSize < dwBigSize )
            {
                lpBigBlock = p; 
				dwBigSize = dwSize;
            }
        }
    }
    // 是否已经得到符合条件的空闲块
    if ( p == NULL && (p = lpBigBlock) == NULL )
    {   //没有，需要从系统空闲内存去分配大的内存，dwRealSize用于保存实际得到的内存块大小

		p = GetSystemMem( lpHeap, dwSizeNeeded+sizeof(BUSY), (DWORD*)&dwRealSize );

	    if ( p == NULL )
        {  // 无法从系统分配内存，退出
			DEBUGMSG( DEBUG_HEAPALLOC, ( "---M(0x%x)M---.\r\n", KL_GetCurrentThreadId() ) );
			LeaveCriticalLock( lpHeap, TRUE );
	        goto M_RETNULL;
        }

		ASSERT( dwRealSize >= dwSizeNeeded+sizeof(BUSY) );

	    dwRealSize &= ~ALIGNMASK;

	    if ( lpHeap->lpLastBlock &&
	         (char*)p == ((char*)lpHeap->lpLastBlock + sizeof(BUSY)) )
        {   // 新的内存段与之前所分配的内存段是连续的，我们将其组合为一个大段

	        p = lpHeap->lpLastBlock;
	        dwRealSize += sizeof(BUSY);
	        /********************************************************
		    Check if previous block is free and combine it now.
	        ********************************************************/
	        // 如果前一个块是空闲的，则将它与新的连接在一起
			if ( (x = p->busy.lpPrev) != 0 && !IS_BUSY(x) )
            {
		        _RemoveFromFree( lpHeap, x );  // 从空闲链表移出
		        dwRealSize += (char*)p - (char*)x;
		        p = x;
            }
        }
    	else
        {
	        if ( lpHeap->lpLastBlock )
            {
		        p->busy.lpPrev = lpHeap->lpLastBlock;
		        lpHeap->lpLastBlock->busy.lpNext = (LPBLOCK)((char*)p+BUSY_FLAG);
            }
	        else
            {
		        p->busy.lpPrev = NULL;
		        lpHeap->lpFirstBlock = p;
            }
        }

		// 保留一个最小的BLOCK结构，用于记录该内存段的结束

	    p->busy.lpNext = lpHeap->lpLastBlock =
	        (LPBLOCK)((char*)p + dwRealSize - sizeof(BUSY));
	    lpHeap->lpLastBlock->busy.lpPrev = p;
		lpHeap->lpLastBlock->busy.lpNext = (LPBLOCK)(BUSY_FLAG);
        // 将新的段插入空闲链表
	    _InsertToFree( lpHeap, p);
    }
    // p 指向空闲块
    if ( (char*)p->busy.lpNext-(char*)p-dwSizeNeeded > sizeof(BLOCK) )
    {
		// 如果p指向的空闲块较大，则从p指向的空闲块分割族够的内存块，并将剩下的放入空闲链表

	    x = (LPBLOCK)((char*)p + dwSizeNeeded);
	    p->busy.lpNext->busy.lpPrev = x;
	    x->busy.lpPrev = p;
	    x->busy.lpNext = p->busy.lpNext;
	    p->busy.lpNext = (LPBLOCK)((char*)x + BUSY_FLAG + ALLOC_FLAG );
	    _RemoveFromFree( lpHeap, p );
	    _InsertToFree( lpHeap, x );
    }
    else
    {   //将p从空闲链表移出
	    _RemoveFromFree( lpHeap, p );
	    p->busy.lpNext = (LPBLOCK)((char*)p->busy.lpNext+BUSY_FLAG +ALLOC_FLAG);
    }

	//离开互斥段
	DEBUGMSG( DEBUG_HEAPALLOC, ( "---M(0x%x)M---.\r\n", KL_GetCurrentThreadId() ) );
	LeaveCriticalLock( lpHeap, TRUE );
    
	if( dwFlags & HEAP_ZERO_MEMORY )
	{
		memset( (char*)p + sizeof(BUSY), 0, dwSave );
	}

    return (char*)p + sizeof(BUSY);
M_RETNULL:
	KL_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    WARNMSG( DEBUG_HEAPALLOC, ( TEXT("error HeapAlloc: Not enough memory.!\r\n") ) );
    return NULL;
}

/**************************************************
声明：BOOL IsPtrValid( void FAR * lpvUsed )
参数：
	lpHeap - 堆句柄指针
	lpvUsed - 需要检验的指针（已分配的内存地址）
返回值：
	假如成功，返回TRUE; 否则返回FALSE
功能描述：
	检验的指针是否合法
引用:
	
************************************************/
#define DEBUG_IsPtrValid 1
static BOOL IsPtrValid( LPCVOID lpvUsed, LPBLOCK * lppRet, LPBLOCK * lpxRet )
{
	LPBLOCK p, x;

    p = (LPBLOCK)((char*)lpvUsed - sizeof(BUSY));
    if ( !IS_BUSY(p) )
    {   // 无效的指针
		WARNMSG( DEBUG_IsPtrValid, ( "error0: invalid ptr(0x%x),(p=0x%x).\r\n" , lpvUsed, p ) );
        KL_SetLastError( ERROR_INVALID_PARAMETER );
        //goto ret;
		return FALSE;
    }
    x = (LPBLOCK)((char*)p->busy.lpNext-(BUSY_FLAG+ALLOC_FLAG) );
    if ( !x || x->busy.lpPrev != p )
    {   // 无效的指针
        KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_IsPtrValid, ( "error1: invalid ptr(0x%x), x=0x%x.\r\n" , lpvUsed, x ) );
        //goto ret;
		return FALSE;
    }
	if( lppRet )
		*lppRet = p;
	if( lpxRet )
		*lpxRet = x;
	return TRUE;
}

/**************************************************
声明：BOOL WINAPI KL_HeapFree( HANDLE hHeap, DWORD dwFlags, void FAR * lpvUsed )
参数：
	hHeap - 堆句柄
	dwFlags - 功能，可以为以下值：
		HEAP_NO_SERIALIZE - 不需要串行化（互斥操作）
	lpvUsed - 之前分配的内存地址
返回值：
	假如成功，返回TRUE; 否则返回FALSE
功能描述：
	释放之前用KL_HeapAlloc 或KL_HeapRealloc返回的一个内存块
引用:
	系统API 
************************************************/
#define DEBUG_HEAPFREE 0
BOOL WINAPI KL_HeapFree( HANDLE hHeap, DWORD dwFlags, void FAR * lpvUsed )
{
    LPBLOCK p, x;
	BOOL bRetv = FALSE;
	LPHEAP lpHeap = (LPHEAP)hHeap;

	ASSERT_NOTIFY( lpvUsed, "Invalid param at _Free:" );

    if ( !lpvUsed ) return FALSE;

	//进入互斥段

	EnterCriticalLock( lpHeap, TRUE );
	
	DEBUGMSG( DEBUG_HEAPALLOC, ( "+++F(0x%x,lock=%d)F+++.\r\n", KL_GetCurrentThreadId(), lpHeap->iLockCount ) );
/*	2004-12-03 remoeve to IsPtrValid 
    p = (LPBLOCK)((char*)lpvUsed - sizeof(BUSY));
    if ( !IS_BUSY(p) )
    {   // 无效的指针
		WARNMSG( DEBUG_HEAPFREE, ( "error0: invalid ptr(0x%x),(p=0x%x) when free.\r\n" , lpvUsed, p ) );
        KL_SetLastError( ERROR_INVALID_PARAMETER );
        goto ret;
    }
    x = (LPBLOCK)((char*)p->busy.lpNext-(BUSY_FLAG+ALLOC_FLAG) );
    if ( !x || x->busy.lpPrev != p )
    {   // 无效的指针
        KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_HEAPFREE, ( "error1: invalid ptr(0x%x), x=0x%x. when free.\r\n" , lpvUsed, x ) );
        goto ret;
    }
*/
	if( IsPtrValid( lpvUsed, &p, &x ) == FALSE )
	{
		WARNMSG( DEBUG_HEAPFREE, ( "erro in KL_HeapFree: invalid ptr(0x%x).\r\n" , lpvUsed ) );
		goto ret;
	}

    p->busy.lpNext = x;

	x = p->busy.lpNext;
    
    if ( !IS_BUSY(x) ) 
	{ // 如果下一个块也是空闲的，则组合它们
		_RemoveFromFree( lpHeap, x );
		p->busy.lpNext = x->busy.lpNext;
		x->busy.lpNext->busy.lpPrev = p; 
	}
	
    if ( (x = p->busy.lpPrev) != 0 && !IS_BUSY(x) ) 
	{ // 如果前一个块也是空闲的，则组合它们
		_RemoveFromFree( lpHeap, x );
		x->busy.lpNext = p->busy.lpNext;
		p->busy.lpNext->busy.lpPrev = x;
		p = x; 
	}

    //将该块插入堆空闲链表
    _InsertToFree( lpHeap, p );

	bRetv = TRUE;

ret:
	//离开互斥段
    DEBUGMSG( DEBUG_HEAPALLOC, ( "---F(0x%x)F---.\r\n", KL_GetCurrentThreadId() ) );
    LeaveCriticalLock( lpHeap, TRUE );
	
	return bRetv;
}

/**************************************************
声明：LPVOID WINAPI KL_HeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpOldPtr, DWORD nNewAllocSize )
参数：
	hHeap - 堆句柄
	dwFlags - 功能，可以为以下值：
		HEAP_NO_SERIALIZE - 不需要串行化（互斥操作）
	lpOldPtr - 之前已经分配的内存。如果是NULL，等同与KL_HeapAlloc
	nNewAllocSize - 需要重新分配的内存大小。 如果是0,等同与KL_HeapFree
返回值：
	假如成功，返回非NULL的指针; 否则返回NULL，不会改变之前的指针
功能描述：
	从堆里重新分配一个内存快
引用:
	系统API 
************************************************/
#define DEBUG_HEAPREALLOC 0
LPVOID WINAPI KL_HeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpOldPtr, DWORD nNewAllocSize )
{
    register void *newp = 0;
    LPBLOCK p, next, x;
    register DWORD oldsz;
	LPHEAP lpHeap = (LPHEAP)hHeap;
	DWORD dwOldSizeSave = 0;
	
	// 如果是NULL，等同与KL_HeapAlloc
    if ( !lpOldPtr )
        return KL_HeapAlloc( lpHeap, dwFlags, nNewAllocSize );
	// 如果是0,等同与KL_HeapFree
    if ( !nNewAllocSize )
    {
        KL_HeapFree( lpHeap, dwFlags, lpOldPtr );
        return 0;
    }

    // 对齐
    nNewAllocSize = (nNewAllocSize + ALIGNMASK) & ~ALIGNMASK;
    
	//进入互斥段
	EnterCriticalLock( lpHeap, TRUE );

	/* 2004-12-03 remove to IsPtrValid
    // 检查合法性
    p = (LPBLOCK)((char*)lpOldPtr - sizeof(BUSY));
    if ( !IS_BUSY(p) ) 
	{// 无效的指针
        KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_HEAPREALLOC, ( "error: realloc: invalid ptr(0x%x).\r\n", p ) );
		goto ret;
	}
    next = (LPBLOCK)((char*)p->busy.lpNext-(BUSY_FLAG+ALLOC_FLAG) );
    if ( !next || next->busy.lpPrev != p ) 
	{// 无效的指针
        KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_HEAPREALLOC, ( "error: realloc: invalid ptr(0x%x), next(0x%x).\r\n", p, next ) );
		goto ret;
	}
	*/
	if( IsPtrValid( lpOldPtr, &p, &next ) == FALSE )
	{
		WARNMSG( DEBUG_HEAPFREE, ( "erro in KL_HeapReAlloc: invalid ptr(0x%x).\r\n" , lpOldPtr ) );
		goto ret;
	}

	
	//得到尺寸（不包含头）
    dwOldSizeSave = oldsz = (char *)next - (char *)lpOldPtr;
	
    if ( oldsz >= nNewAllocSize ) 
	{  
		register DWORD sizeleft;
        // 新的内存需要小于之前分配的老的内存块，我们只需减少老的内存块
resize:
		sizeleft = oldsz - nNewAllocSize;  //释放某些bytes
		if ( sizeleft > sizeof(BLOCK) ) 
		{   // 将其插入空闲链表
			x = (LPBLOCK)((char*)next - sizeleft);
			x->busy.lpNext = next;
			x->busy.lpPrev = p;
			p->busy.lpNext = (LPBLOCK)( (char*)x+(BUSY_FLAG+ALLOC_FLAG) );
			next->busy.lpPrev = x;
			_InsertToFree( lpHeap, x );
			if ( !IS_BUSY(next) ) 
			{   //如果与下一个空闲块的地址是连续，则组合它们
				_RemoveFromFree( lpHeap, next );
				x->busy.lpNext = next->busy.lpNext;
				next->busy.lpNext->busy.lpPrev = x; 
			}
		}
		newp = lpOldPtr; 
	}
	
    else if ( !IS_BUSY(next) && (x=next->busy.lpNext) != 0 &&
		(oldsz = (char*)x - (char*)lpOldPtr) >= nNewAllocSize ) 
	{  //下一个块是空闲块并且其大小满足需要的尺寸
		_RemoveFromFree( lpHeap, next );
		p->busy.lpNext = (LPBLOCK)( (char*)x+(BUSY_FLAG+ALLOC_FLAG) );
		x->busy.lpPrev = p;
		next = x;
		goto resize; 
	}
	
    else if ( (x=p->busy.lpPrev) != 0 && !IS_BUSY(x) &&
		      (oldsz = (char*)next - ((char*)x+sizeof(BUSY))) >= nNewAllocSize )
	{
		char *from, *to; 
		DWORD n;
		//前一个块是空闲块并且其大小满足需要的尺寸
		_RemoveFromFree( lpHeap, x );
		x->busy.lpNext = (LPBLOCK)( (char*)next + (BUSY_FLAG+ALLOC_FLAG) );
		next->busy.lpPrev = x;
		from = (char*)p + sizeof(BUSY);
		to   = (char*)x + sizeof(BUSY);
		n = (char *)next - (char *)lpOldPtr;
        // 拷贝内容
		memmove( to, from, n );
		p = x;
		lpOldPtr = (char*)p + sizeof(BUSY);
		goto resize; 
	}
    else 
	{
		// 需要一个完全新的块
		LeaveCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add

        // 重新分配
		if ( !(newp = KL_HeapAlloc( lpHeap, dwFlags, nNewAllocSize ) ) ) //;Mem_Alloc( nNewAllocSize )) ) 
			return 0;
		// 拷贝内容
		memmove( newp, lpOldPtr, (char *)next - (char *)lpOldPtr );
		// 释放老的
		KL_HeapFree( lpHeap, dwFlags, lpOldPtr );
		return newp; 
	}
	
ret:

	//离开互斥段

	LeaveCriticalLock( lpHeap, TRUE );

	if( dwOldSizeSave < nNewAllocSize && (dwFlags & HEAP_ZERO_MEMORY ) )
	{   // 将多于的内存内容清为0
		memset( (char*)newp + dwOldSizeSave, 0, nNewAllocSize - dwOldSizeSave );
	}
    return newp;
}

/**************************************************
声明：int Heap_Enum( LPHEAP lpHeap, LPHEAPENUMPROC lpfn, LPARAM lParam, UINT uiFlag )

参数：
	lpHeap - HEAP结构指针
	lpfn - 枚举功能
	lParam - 传递给枚举功能的参数
	uiFlag - 保留
返回值：
	假如成功，返回TRUE; 否则返回FALSE
功能描述：
	枚举所有已分配的块
引用:
	
************************************************/

static int Heap_Enum( LPHEAP lpHeap, LPHEAPENUMPROC lpfn, LPARAM lParam, UINT uiFlag )
{
    LPBLOCK lpBlock, lpFirstBlock;

	EnterCriticalLock( lpHeap, TRUE );

	lpFirstBlock = lpBlock = lpHeap->lpFirstBlock;
	//KL_DebugOutString( "........check heap.........\r\n" );
	while( lpBlock )
	{
		if( IS_ALLOC(lpBlock) )//&& !IS_GAP( lpBlock ) )
		{
			if( lpfn( ((LPBYTE)lpBlock) + sizeof( BUSY ), lParam ) == 0 )
				break;  // not contunue
		}
	    lpBlock = lpBlock->busy.lpNext;// - (ALLOC_FLAG+BUSY_FLAG);
		lpBlock = (LPBLOCK) ( (DWORD)lpBlock & ( ~(ALLOC_FLAG+BUSY_FLAG) ) );
	}

	LeaveCriticalLock( lpHeap, TRUE );

	return TRUE;
}
/*
static int __HeapCheck(LPHEAP lpHeap,char *__file, int __line)
{
    LPBLOCK p, lpPrev;
	int iRetv = 1;
	if( lpHeap )
	{
		EnterCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add

		lpPrev = NULL;
		for ( p = lpHeap->lpFreeList; p; p = p->lpNextFree )
		{
			if( p->lpPrevFree != lpPrev )
			{
				WARNMSG( 1, ("Invalidate heap freelist ptr:lpHeap=%x,p=%x,p->lpPrevFree=%x,lpPrev=%x,file=%s,line=%d\r\n", lpHeap, p , p->lpPrevFree, lpPrev, __file, __line ) );
				iRetv = 0;
				break;
			}
			lpPrev = p;   
		}

		lpPrev = NULL;
		for ( p = lpHeap->lpFirstBlock; p; p = (LPBLOCK)( (DWORD)p->busy.lpNext & (~(BUSY_FLAG+ALLOC_FLAG) ) ) )
		{
			if( p->busy.lpPrev != lpPrev )
			{
			    WARNMSG( 1, ("Invalidate heap busy list, lpHeap=%x,p=%x,file=%s,line=%d\r\n", lpHeap, p, __file, __line) );
				iRetv = 0;
				break;
			}
			lpPrev = p;

		}
		LeaveCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add
	}
	return iRetv;


}

static LPHEAP lphpTest = NULL;

int _Heap_Check( char *__file, int __line )
{
	LPHEAP lpHeap = (LPHEAP)KL_GetProcessHeap();

	if( lpHeap )	
	    __HeapCheck( lpHeap,__file, __line );

	return 0;
}
*/


/**************************************************
声明：BOOL WINAPI KL_HeapValidate( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem )

参数：
	hHeap -堆句柄（调用HeapCreate返回的句柄）
	dwFlags – 属性，可以是：
		HEAP_NO_SERIALIZE – 说明当进行堆操作时不必进行互斥
	lpMem – 保留，必须为NULL
返回值：
	假如成功，返回TRUE; 否则返回FALSE
功能描述：
	检查堆是否有效
引用:
	系统API	
************************************************/
#define IS_ZERO_PTR( p ) ( ( (DWORD)p ) < 0x10000 )
BOOL WINAPI KL_HeapValidate( HANDLE hHeap, 
						   DWORD dwFlags,
						   LPCVOID lpMem )
{
	LPHEAP lpHeap = (LPHEAP)hHeap;
    LPBLOCK p, lpPrev; 
	int iRetv = 1;
	if( lpHeap )
	{		
	    EnterCriticalLock( lpHeap, !(dwFlags & HEAP_NO_SERIALIZE) );

		if( lpMem )
		{
			iRetv = IsPtrValid( lpMem, NULL, NULL );
		}
		else 
		{
			
			lpPrev = NULL;
			for ( p = lpHeap->lpFreeList; p; p = p->lpNextFree )
			{
				if( IS_ZERO_PTR( p ) )
				{
					ERRORMSG( 1, ("Invalidate heap freelist ptr:lpHeap=%x,p=0x%x.\r\n", lpHeap, p ) );
					iRetv = 0;
					break;
				}
				else if( p->lpPrevFree != lpPrev )
				{
					ERRORMSG( 1, ("Invalidate heap freelist ptr:lpHeap=%x,p=0x%x,p->lpPrevFree=%x,lpPrev=%x.\r\n", lpHeap, p , p->lpPrevFree, lpPrev ) );
					iRetv = 0;
					break;
				}
				lpPrev = p;   
			}
			
			lpPrev = NULL;
			for ( p = lpHeap->lpFirstBlock; p; p = (LPBLOCK)( (DWORD)p->busy.lpNext & (~(BUSY_FLAG+ALLOC_FLAG) ) ) )
			{
				if( IS_ZERO_PTR( p ) )
				{
					ERRORMSG( 1, ("Invalidate heap freelist ptr:lpHeap=%x,0xp=0x%x.\r\n", lpHeap, p ) );
					iRetv = 0;
					break;
				}			
				else if( p->busy.lpPrev != lpPrev )
				{
					ERRORMSG( 1, ("Invalidate heap busy list, lpHeap=%x,p=0x%x.\r\n", lpHeap, p ) );
					iRetv = 0;
					break;
				}
				lpPrev = p;
				
			}
		}
		LeaveCriticalLock( lpHeap, !(dwFlags & HEAP_NO_SERIALIZE) );
	}
	return iRetv;
}
/*
//内部使用
BOOL WINAPI _KL_HeapGetInfo( HANDLE hHeap, 
							 LPDWORD lpdwTotalReserveSize,
							 LPDWORD lpdwTotalCommitSize )
{
	LPHEAP lpHeap = (LPHEAP)hHeap;
	if( lpHeap )
	{
		*lpdwTotalReserveSize = lpHeap->dwTotalReserveSize;
		*lpdwTotalCommitSize = lpHeap->dwTotalCommitSize;
		return TRUE;
	}
	return FALSE;
}
*/
