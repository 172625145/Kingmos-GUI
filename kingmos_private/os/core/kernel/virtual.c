/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：虚拟内存管理
版本号：2.0.0
开发时期：2002
作者：李林
修改记录：
	2005-01-18 : 增加对 内核进程的虚地址段的处理，例如去掉 dwSegIndex < MAX_SEGMENTS 等
******************************************************/

#include <eframe.h>
#include <cpu.h>
#include <pagemgr.h>
#include <coresrv.h>
#include <virtual.h>
#include <epalloc.h>
#include <epcore.h>

//  The user mode virtual address space is 2GB split into 64 32M sections
//  of 512 64K blocks of 16 4K pages.
//  2GB的虚地址空间被划分为 64个段,每个段（32M）被划分为512个块，每个块（64K ）被划分为16个页(每页4K)
//  Virtual address format:
//  3322222 222221111 1111 110000000000
//  1098765 432109876 5432 109876543210
//  zSSSSSS BBBBBBBBB PPPP oooooooooooo


//static LPSEGMENT KernelSegments[MAX_SEGMENTS];

#define PAGE_IN_RANGE( page, minPage, maxPage ) ( (page) >= minPage && (page) < maxPage )

static CRITICAL_SECTION csVirtualMem;

BOOL Seg_DecommitPages( LPPROCESS_SEGMENTS lpProcessSeg, LPSEGMENT lpSeg, int idxStartBlk, int idxStartPages, int iPages );
static LPPROCESS_SEGMENTS Seg_FindProcessSegment( DWORD dwAddress );

// ********************************************************************
// 声明：BOOL InitialVirtualMgr( void )
// 参数：
//		无
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		初始化虚拟内存管理器
// 引用：
//		system.c 
// ********************************************************************
BOOL InitialVirtualMgr( void )
{
	KC_InitializeCriticalSection( &csVirtualMem );
	return TRUE;
}

// ********************************************************************
// 声明：BOOL IsPageInCache( LPPROCESS_SEGMENTS lpProcessSeg, 
//							 UINT uiPageIndex )
// 参数：
//		IN lpProcessSeg - 进程段组
//		IN uiPageIndex - 页在段内的索引
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		判断一个页是否在 页表内
// 引用：
//		system.c 
// ********************************************************************
/*
static BOOL IsPageInCache( LPPROCESS_SEGMENTS lpProcessSeg, UINT uiPageIndex )
{
	if( uiPageIndex > lpProcessSeg->uiPageTableStart && 
		uiPageIndex < lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA )
		return TRUE;
	return FALSE;
}
*/

// ********************************************************************
// 声明：BOOL GetPagesInCacheRange( LPPROCESS_SEGMENTS lpProcessSeg,
//							        UINT * lpuiPageIndexStart, UINT * lpuiPageIndexEnd )
// 参数：
//		IN lpProcessSeg - 进程段组
//		IN/OUT lpuiPageIndexStart - 页在段内的索引
//		IN/OUT lpuiPageIndexStart - 页在段内的索引
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		判断一个页是否在 页表内
// 引用：
//		system.c 
// ********************************************************************
static BOOL GetPagesInCacheRange( LPPROCESS_SEGMENTS lpProcessSeg,
					             UINT * lpuiPageIndexStart, 
								 UINT * lpuiPageIndexEnd )
{
	UINT uStartCachePage, uEndCachePage, minPage, maxPage;

	if( lpProcessSeg ) //&& lpProcessSeg->lpdwSecondPageTable )
	{
		uStartCachePage = lpProcessSeg->uiPageTableStart;
		uEndCachePage = uStartCachePage + lpProcessSeg->uiPageTableCount * PAGES_PER_MEGA;
	
		//ASSERT( lpProcessSeg->lpdwSecondPageTable );
		
		minPage = MAX( uStartCachePage, *lpuiPageIndexStart );
		maxPage = MIN( uEndCachePage, *lpuiPageIndexEnd );
		if( minPage < maxPage )
		{
			*lpuiPageIndexStart = minPage;
			*lpuiPageIndexEnd = maxPage;
			return TRUE;
		}
	}
	return FALSE;
}


// ********************************************************************
// 声明：LPSEGMENT Seg_Alloc( void )
// 参数：
//		无
// 返回值：
//		段指针
// 功能描述：
//		分配一个段结构指针
// 引用：
//		
// ********************************************************************
LPSEGMENT Seg_Alloc( void )
{
	LPSEGMENT lpSeg = (LPSEGMENT)KHeap_Alloc( sizeof( SEGMENT ) );
	
	if( lpSeg )
	{
		memset( lpSeg, 0, sizeof( SEGMENT ) );
	}
	return lpSeg;
}

// ********************************************************************
// 声明：void Seg_Free ( LPSEGMENT lpSeg ) 
// 参数：
//		IN  lpSeg - 段结构指针
// 返回值：
//		无
// 功能描述：
//		释放段结构指针及其所占用的物理页
// 引用：
//		
// ********************************************************************
void Seg_Free ( LPSEGMENT lpSeg ) 
{
	LPMEMBLOCK * lppBlks;
	int i;
	BOOL bFlush = 0;

   // ASSERT( (DWORD)lpSeg != dwProcessSlots[0] );
	FlushCacheAndClearTLB();  // 先刷新CACHE

	lppBlks = lpSeg->lpBlks;
	// 查找每一个块，如果有未释放的物理页，则释放它们
	for( i = 0; i < BLOCKS_PER_SEGMENT; i++, lppBlks++ )
	{
		if( *lppBlks > RESERVED_BLOCK ) // 是否有块结构 ？
		{   // 有
//			ASSERT( i != 0 );
			// 释放该段该块的物理页
			bFlush |= Seg_DecommitPages( NULL, lpSeg, i, 0, PAGES_PER_BLOCK );
			// 释放该块
			KHeap_Free( *lppBlks, sizeof(MEMBLOCK) );
		}
	}
	// 释放段结构
	KHeap_Free( lpSeg, sizeof(SEGMENT) );
	if( bFlush )  // 释放完，刷新CACHE
	    FlushCacheAndClearTLB();
}

// ********************************************************************
// 声明：static int Seg_SearchFirstBlock( LPSEGMENT lpSeg, int index )
// 参数：
//		IN lpSeg - 段结构指针
//		IN index - 连续块组内其中的一个块在段内的索引 
// 返回值：
//		假如成功，返回块索引值；否则，返回NOT_FIND_FIRST
// 功能描述：
//		得到一个连续块组的第一个块
// 引用：
//		
// ********************************************************************
static int Seg_SearchFirstBlock( LPSEGMENT lpSeg, int index ) 
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + index;
	if( *lppBlks != NULL_BLOCK && index >= 0 )
	{   // 
		if( *lppBlks > RESERVED_BLOCK )  // 块值是第一个块索引值吗 ？
			return (*lppBlks)->idxFirst;  // 否，是内存块结构。成员idxFirst指向第一个块
		else
			return (int)*lppBlks;  // 是，直接返回该值
	}
	return NOT_FIND_FIRST;
}
// ********************************************************************
// 声明：void Seg_GetInfo( LPSEGMENT lpSeg, 
//				  LPDWORD lpdwAvailVirtual,
//				  LPDWORD lpdwReserve,
//				  LPDWORD lpdwCommit )

// 参数：
//		IN lpSeg - 段结构指针
//		OUT lpdwAvailVirtual - 用于接受可利用虚空间 
//		OUT lpdwReserve - 用于接受已被保留的虚空间
//		OUT lpdwCommit - 用于接受已提交（分配）的空间
// 返回值：
//		无
// 功能描述：
//		得到段信息
// 引用：
//		
// ********************************************************************
void Seg_GetInfo( LPSEGMENT lpSeg, 
				  LPDWORD lpdwAvailVirtual,
				  LPDWORD lpdwReserve,
				  LPDWORD lpdwCommit )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks+1;  // 每个段的第0块是被保留的
	int idxStart = 1;
	DWORD dwAvailVirtual = 0;
	DWORD dwReserve = 0;
	DWORD dwCommit = 0;

	// 搜索每一个块
	for(  ; idxStart < BLOCKS_PER_SEGMENT; idxStart++, lppBlks++ )
	{	// 
		if( *lppBlks != NULL_BLOCK )  // 是否被使用（分配或保留） ？
		{   // 是，has alloc
			if( *lppBlks <= RESERVED_BLOCK )  // 是保留吗 ？
				dwReserve += BLOCK_SIZE;  // 是
			else
			{   // 已经提交的块
				int i;
				LPUINT lpuiPage = (*lppBlks)->uiPages;
				// 统计每个块内的页数
				for( i = 0; i < PAGES_PER_BLOCK; i++, lpuiPage++ )
				{
					if( *lpuiPage == 0 )  // 未提交/分配
						dwReserve += PAGE_SIZE;
					else if( *lpuiPage != INVALID_PAGE )  // 如果不是无效页就是被分配的页
					{  // commited pages
						dwCommit += PAGE_SIZE;
					}
				}
			}
		}
		else
		{   // 该块未使用
			dwAvailVirtual += BLOCK_SIZE;
		}
	}	
	*lpdwAvailVirtual = dwAvailVirtual;
	*lpdwReserve = dwReserve;
	*lpdwCommit = dwCommit;
}
// ********************************************************************
// 声明：static int Seg_GetFirstBlockPos( LPSEGMENT lpSeg, int idxStart, int iNeedPages, BOOL bAutoNext )
// 参数：
//		IN lpSeg - 段结构指针
//		IN idxStart - 起始块
//		IN iNeedPages - 需要的页数
//		IN bAutoNext - 是否自动查找下一个位置
// 返回值：
//		如果成功，返回第一个块的索引值；否则，返回0
// 功能描述：
//		得到一个大小为iNeedPahes的连续的未使用的块组，并返回该组的第一个块的索引值
// 引用：
//		
// ********************************************************************

static int Seg_GetFirstBlockPos( LPSEGMENT lpSeg, int idxStart, int iNeedPages, BOOL bAutoNext )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks+idxStart;
	int idxFirst = idxStart;
	int iPages = iNeedPages;

	//RETAILMSG( 1, ( "idxStart=%d, iNeedPages=%d, bAutoNext=%d.\r\n", idxStart, iNeedPages, bAutoNext ) );

	for(  ; idxStart < BLOCKS_PER_SEGMENT; idxStart++, lppBlks++ )
	{
		if( *lppBlks != NULL_BLOCK )  // 是否已被占用
		{   // 是，该组不能用。has alloc
			if( bAutoNext )  // 是否可以自动向下寻找
			{   // 是，重新查找
				iPages = iNeedPages;
				idxFirst = idxStart + 1;
			}
			else
				return 0;  // 不要向下寻找, 返回
		}
		else
		{   // 未占用，可以用
			if( (iPages -= PAGES_PER_BLOCK) <= 0 ) // 块组全部满足需要吗 ？
				return idxFirst; // 是，返回首索引。yes , i get it
		}
	}
	//RETAILMSG( 1, ( "idxStar_end=%d.\r\n", idxStart ) );

	return 0;
}

// ********************************************************************
// 声明：static int Seg_ReserveNeedPages( 
//								LPSEGMENT lpSeg,
//								int idxStart,
//								int iNeedPages, 
//								BOOL bAutoCommit, 
//								DWORD dwProtect )
// 参数：
//		IN lpSeg - 段结构指针
//		IN idxStart - 开始块索引
//		IN iNeedPages - 需要的页数
//		IN bAutoCommit - 是否自动提交
//		IN dwProtect
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		从开始块保留需要的页
// 引用：
//		
// ********************************************************************
static int Seg_ReserveNeedPages( LPSEGMENT lpSeg, int idxStart, int iNeedPages, BOOL bAutoCommit, DWORD dwProtect )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks+idxStart;
	LPMEMBLOCK lpStartBlk, lpBlk;//, lpBlkEnd;

	lpBlk = lpStartBlk = (LPMEMBLOCK)KHeap_Alloc( sizeof( MEMBLOCK ) );//分配块结构

	if( lpStartBlk )
	{
		// 初始化第一个块
		memset( lpStartBlk, 0, sizeof( MEMBLOCK ) );

		lpStartBlk->idxFirst = idxStart;
		lpStartBlk->uiKey = 0;
		lpStartBlk->wLockCount = 0;
		lpStartBlk->uiFlag = bAutoCommit ? ( MF_AUTO_COMMIT | GetAttribFromProtect( dwProtect ) ) : 0;
		*lppBlks++ = lpStartBlk;
		
		iNeedPages -= PAGES_PER_BLOCK;

		//RETAILMSG( 1, ( "Seg_ReserveNeedPages: lpSeg=%x,idxStart=%d,*lppBlks=%x,iNeedPages=%d.\r\n", lpSeg, idxStart, *lppBlks, iNeedPages ) );
        
		// 保留其它块
		for(  ; iNeedPages > 0; iNeedPages -= PAGES_PER_BLOCK, lppBlks++ )
		{	// 
			if( bAutoCommit || iNeedPages < PAGES_PER_BLOCK )
			{   // 如果是自动提交 或 最后一个块，则分配一个块结构
	            lpBlk = (LPMEMBLOCK)KHeap_Alloc( sizeof( MEMBLOCK ) );
				if( lpBlk )
				{   // 初始化块结构
		            memset( lpBlk, 0, sizeof( MEMBLOCK ) );
					lpBlk->idxFirst = idxStart;//
					lpBlk->uiKey = lpStartBlk->uiKey;
					lpBlk->wLockCount = lpStartBlk->wLockCount;
					lpBlk->uiFlag = lpStartBlk->uiFlag;
					*lppBlks = lpBlk;
				}
                else
				{	// 没有内存，释放之前分配的块结构。no enough memory, clear
					LPMEMBLOCK * lppBlkStart = lpSeg->lpBlks+idxStart;
					//RETAILMSG( 1, ( "Seg_ReserveNeedPages:no enough memory, clear.\r\n" ) );
					while( lppBlkStart < lppBlks  )
					{
						if( *lppBlkStart > RESERVED_BLOCK ) // 块指针是分配的，释放它
						{
							KHeap_Free( *lppBlkStart, sizeof( MEMBLOCK ) );
						}
						*lppBlkStart++ = NULL_BLOCK;
					}
					lpStartBlk = NULL; // use by retv
					iNeedPages = 0;
				}
			}
			else
			{   // 不需要自动提交并且是中间的块。设置保留标志 set reserve flag
			    *lppBlks = (LPMEMBLOCK)idxStart;// 保留标志是该组的第一个块
			}
		}
		if( iNeedPages )  // 是否块组的所有的页都被保留 ？
		{   // 不是，块组的一些页是无效的。这里设置无效标志。not all pages is valid, set invalid page flag
			LPUINT lpuiPage;

			iNeedPages += PAGES_PER_BLOCK;
			lpuiPage = &lpBlk->uiPages[iNeedPages];
			for( ; iNeedPages < PAGES_PER_BLOCK; iNeedPages++ )
			{
				*lpuiPage++ = INVALID_PAGE;  //设置无效标志
			}
		}
		//RETAILMSG( 1, ( "Seg_ReserveNeedPages-end: lpSeg=%x,idxStart=%d,*lppBlks=%x.\r\n", lpSeg, idxStart, *(lpSeg->lpBlks+idxStart) ) );
	}
	return lpStartBlk ? TRUE : FALSE;
}

// ********************************************************************
// 声明：static void Seg_FreeRegion( LPSEGMENT lpSeg, int idxFirstBlk )
// 参数：
//		IN lpSeg - 段结构指针
//		IN idxFirstBlk - 起始块
// 返回值：
//		无
// 功能描述：
//		释放块组（内存区域）
// 引用：
//		
// ********************************************************************
static void Seg_FreeRegion( LPSEGMENT lpSeg, int idxFirstBlk )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxFirstBlk;  // 起始块地址
	LPMEMBLOCK * lppBlkEnd = lpSeg->lpBlks + BLOCKS_PER_SEGMENT; // 最大结束块地址
	LPMEMBLOCK lpBlk;

	while( lppBlks < lppBlkEnd )
	{
		lpBlk = *lppBlks;
		if( lpBlk == NULL_BLOCK )
			break;  // 结束
		else if( lpBlk <= RESERVED_BLOCK )
		{   // 设为NULL_BLOCK
			*lppBlks = NULL_BLOCK;
		}
		else if( lpBlk->idxFirst != idxFirstBlk )
		{   // 结束 end
			break;
		}
		else
		{   // 已分配的块，释放它commited block
			KHeap_Free( lpBlk, sizeof(MEMBLOCK) );//_kfree( lpBlk );
			*lppBlks = NULL_BLOCK;
		}
		lppBlks++;  // 下一个块
	}
}

// ********************************************************************
// 声明：int Seg_ReviewRegion( LPSEGMENT lpSeg, 
//					 int idxFirstBlk,
//					 int idxReviewBlk,
//					 int idxStartPage,
//					 int iReviewPages,
//					 int * lpiCountPages )
// 参数：
//		IN lpSeg - 段结构指针
//		IN idxFirstBlk - 内存域第一个块
//		IN idxReviewBlk - 内存域中需要检视的起始块
//		IN idxStartPage - 内存域中需要检视的起始块中的开始页. 假如为-1,则检视到该域的结束
//		IN iReviewPages - 需要检视的页数
//		IN/OUT lpiCountPages - 用于接受检视的合法页数, 同时也说明该函数是否仅仅执行统计功能。
//							如果为NULL，则为每一个保留块分配一个块结构
// 返回值：
//		假如成功，返回需要分配的物理页数；错误返回-1
// 功能描述：
//		从指定区域位置开始检视一定的页数。假如lpiCountPages为NULL,则为每一个保留的块位置分配一个块结构
// 引用：
//		
// ********************************************************************
// returen need alloc physical page
#define DEBUG_SEG_REVIEWREGION 0
int Seg_ReviewRegion( LPSEGMENT lpSeg, 
					 int idxFirstBlk,
					 int idxReviewBlk,
					 int idxStartPage,
					 int iReviewPages,
					 int * lpiCountPages )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxReviewBlk; // 起始块地址
	LPMEMBLOCK lpBlk, lpStartBlk;
	int iPages = 0;
	int iAllocPages = 0;


    lpStartBlk = lpSeg->lpBlks[idxFirstBlk];
	if( iReviewPages == -1 )  // 需要检视到结束吗 ？
	{	// 是，设置为最大
		iReviewPages = (BLOCKS_PER_SEGMENT - idxReviewBlk) * PAGES_PER_BLOCK - idxStartPage;//BLOCKS_PER_SEGMENT * PAGES_PER_BLOCK;
	}
	DEBUGMSG( DEBUG_SEG_REVIEWREGION, ( "idxFirstBlk=%d,idxReviewBlk=%d,idxStartPage=%d,iReviewPages=%d.\r\n", idxFirstBlk,idxReviewBlk,idxStartPage,iReviewPages ) );
	while( iPages < iReviewPages )
	{
		lpBlk = *lppBlks;
		DEBUGMSG( DEBUG_SEG_REVIEWREGION, ( "Seg_ReviewRegion:lpBlk=%x.\r\n", lpBlk ) );
		if( lpBlk <= RESERVED_BLOCK && lpBlk != NULL_BLOCK )  // 该块已被保留并且没有为其分配块结构吗 ？
		{   // 已被保留，假如可能，分配一个块结构。now to commit a reserver block if need
			int nAlloc;
			if( lpiCountPages == NULL )   // 仅仅为了统计吗 ？
			{    // 否，需要分配一个块结构
				lpBlk = KHeap_Alloc( sizeof( MEMBLOCK ) );
				if( lpBlk )
				{   // 初始化块结构
					memset( lpBlk, 0, sizeof( MEMBLOCK ) );					
					lpBlk->idxFirst = lpStartBlk->idxFirst;  // 指向内存域的第一个块
					lpBlk->uiKey = lpStartBlk->uiKey;
					lpBlk->wLockCount = lpStartBlk->wLockCount;	
					*lppBlks = lpBlk;
				}
				else //no memory
				{   // 没有内存
					ERRORMSG( 1, ( "Seg_ReviewRegion error: no enough memory.\r\n" ) );
					KL_SetLastError( ERROR_NOT_ENOUGH_MEMORY );
					goto _err_return;//break;
				}
			}
			nAlloc = PAGES_PER_BLOCK - idxStartPage; // 该块的页数
			if( iPages + nAlloc > iReviewPages )  // 是否已统计完所有的页？
			{   // 是
				iAllocPages += iReviewPages - iPages; // 真实的物理页
				iPages = iReviewPages;	// 统计完，设置退出循环条件				
			}
			else
			{	// 
				iPages += nAlloc;//已检视的页数
				iAllocPages += nAlloc;//需要的物理页数
			}
		}
		else if( lpBlk == NULL_BLOCK || lpBlk->idxFirst != idxFirstBlk )
		{	// 该内存域已结束
			if( lpiCountPages )
				break;  // 如果仅仅是为了统计，正常退出。end count
			// 不是为了统计，有错误！！！
			ERRORMSG( 1, ( "Seg_ReviewRegion error: invalid lpBlk=0x%x,lpBlk->idxFirst=%d,idxFirstBlk=%d.\r\n", lpBlk, (lpBlk) ? lpBlk->idxFirst : 0, idxFirstBlk ) );
			KL_SetLastError( ERROR_INVALID_PARAMETER );
			goto _err_return;
		}
		else
		{   // 该块有一个内存块结构，检视其页
			LPUINT lpuiPage = &lpBlk->uiPages[idxStartPage];
			DEBUGMSG( DEBUG_SEG_REVIEWREGION, ( "Seg_ReviewRegion:Commit Blk,iPages=%d.\r\n", iPages ) );
			while( iPages < iReviewPages && idxStartPage < PAGES_PER_BLOCK ) //
			{
				UINT uiPage;
				if( (uiPage = *lpuiPage) == 0 )  // 还没有提交物理页吗　？
				{   // 没有，增加
					iAllocPages++;
				}
				else if( uiPage == INVALID_PAGE )
				{   // 该页是一个无效页，在内存域的结束边界。error
					if( lpiCountPages )  // 如果是为了统计，正确返回
						goto _return;
					// 错误，没有达到指定的检视页数
			        KL_SetLastError( ERROR_INVALID_PARAMETER );
					ERRORMSG( 1, ( "Seg_ReviewRegion error: invalid page.\r\n" ) );
					goto _err_return;
				}
				idxStartPage++;
				lpuiPage++;
				iPages++;
			}
		}
		idxStartPage = 0;  // 下一个块的开始页
		lppBlks++;	// 下一个块
	}
_return:
	if( lpiCountPages )
		*lpiCountPages = iPages;
	return iAllocPages;
_err_return:
	return -1;
}

// ********************************************************************
// 声明：BOOL Seg_ScanCommitRegion( LPSEGMENT lpSeg, 
//					 int idxFirstBlk,
//					 int idxScanBlk,
//					 int idxStartPage,
//					 int iScanPages )

// 参数：
//		IN lpSeg - 段结构指针
//		IN idxFirstBlk - 内存域第一个块
//		IN idxScanBlk - 内存域中需要扫描的起始块
//		IN idxStartPage - 内存域中需要扫描的起始块中的开始页
//		IN iScanPages - 需要扫描的页数
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		扫描指定的内存域是否有指定的(iReviewPages)已经提交的物理页。假如有，返回TRUE.
// 引用：
//		
// ********************************************************************
// returen need alloc physical page
BOOL Seg_ScanCommitRegion( LPSEGMENT lpSeg, 
					 int idxFirstBlk,
					 int idxScanBlk,
					 int idxStartPage,
					 int iScanPages )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxScanBlk;  // 开始扫描块
	LPMEMBLOCK lpBlk;
	int iPages = 0;
	BOOL bRetv = TRUE;

	while( iPages < iScanPages )
	{
		lpBlk = *lppBlks;
		// 2004-01-28, this is a error ?, i will check it
		//if( lpBlk > RESERVED_BLOCK && lpBlk->idxFirst != idxFirstBlk )
		if( lpBlk > RESERVED_BLOCK && lpBlk->idxFirst == idxFirstBlk )
		// 
		{
			LPUINT lpuiPage = &lpBlk->uiPages[idxStartPage];
			while( iPages < iScanPages && idxStartPage < PAGES_PER_BLOCK )
			{
				UINT uiPage;
				if( (uiPage = *lpuiPage) == 0 ||
					 uiPage == INVALID_PAGE )   //  页已经提交了吗 ？
				{	// 没有，错误
					bRetv = FALSE;
			        KL_SetLastError( ERROR_INVALID_PARAMETER );
					goto _return;
				}
				idxStartPage++;
				lpuiPage++;  // 下一页
				iPages++;  // 已统计的页数
			}
		}
		else
		{	// 有非该内存域的块或有非提交页，错误
			bRetv = FALSE;
	        KL_SetLastError( ERROR_INVALID_PARAMETER );
			goto _return;
		}

		idxStartPage = 0;
		lppBlks++;	//下一个块
	}
_return:
	return bRetv;
}

// ********************************************************************
// 声明：BOOL Seg_ResetPageAttrib( 
//					    LPPROCESS_SEGMENTS lpProcessSeg,
//						LPSEGMENT lpSeg, 
//						int idxFirstBlk,
//						int idxReviewBlk,
//						int idxStartPage,
//						int iReviewPages,
//						DWORD dwNewAttrib )

// 参数：
//		IN lpProcessSeg - 进程段组
//		IN lpSeg - 段结构指针
//		IN idxFirstBlk - 内存域第一个块
//		IN idxReviewBlk - 内存域中需要重设的起始块
//		IN idxStartPage - 内存域中需要重设的起始块中的开始页
//		IN iReviewPages - 需要重设的页数
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		重新设定页的属性
// 引用：
//		
// ********************************************************************
//
BOOL Seg_ResetPageAttrib( 
						 LPPROCESS_SEGMENTS lpProcessSeg,
						 LPSEGMENT lpSeg, 
						 int idxFirstBlk,
						 int idxReviewBlk,
						 int idxStartPage,
						 int iReviewPages,
						 DWORD dwNewAttrib )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxReviewBlk;  // 开始页
	LPMEMBLOCK lpBlk;
	int iPages = 0;
	BOOL bRetv = TRUE;
	UINT uStartPage; 
	BOOL bWriteToSecondPage;
	UINT uMinPage, uMaxPage;

	DWORD dwPhysAddrMask = GetPhyPageAdrMask();

	//需要CACHE ?-2005-02-02
	//bWriteToSecondPage = FALSE;
	//if( lpProcessSeg->lpdwSecondPageTable )
	//{
	uMinPage = uStartPage = idxReviewBlk * PAGES_PER_BLOCK + idxStartPage;
	uMaxPage = uStartPage + iReviewPages;

	bWriteToSecondPage = GetPagesInCacheRange( lpProcessSeg, &uMinPage, &uMaxPage );
	//}
	//

	while( iPages < iReviewPages )
	{
		LPUINT lpuiPage;

		lpBlk = *lppBlks;
		// 2004-01-28， this is a error, i will test it
		// ASSERT( lpBlk > RESERVED_BLOCK && lpBlk->idxFirst != idxFirstBlk );
		ASSERT( lpBlk > RESERVED_BLOCK && lpBlk->idxFirst == idxFirstBlk );
		//
		//重新设定该块的页的属性 
		lpuiPage = &lpBlk->uiPages[idxStartPage];
		while( iPages < iReviewPages && idxStartPage < PAGES_PER_BLOCK )
		{
			ASSERT( *lpuiPage != 0 && *lpuiPage != INVALID_PAGE );
			*lpuiPage = ( *lpuiPage & dwPhysAddrMask ) | dwNewAttrib;

			//更新页表-2005-02-03
			if( bWriteToSecondPage && 
				PAGE_IN_RANGE( uStartPage, uMinPage, uMaxPage ) )
			{
				lpProcessSeg->lpdwSecondPageTable[uStartPage] = *lpuiPage;
			}
			uStartPage++;
			//

			lpuiPage++;  // 下一页
			idxStartPage++;
			iPages++;
		}

		idxStartPage = 0;
		lppBlks++;	//下一个块
	}
	return TRUE;
}

// ********************************************************************
// 声明：BOOL Seg_DecommitPages( 
//							LPPROCESS_SEGMENTS lpProcessSeg,
//							LPSEGMENT lpSeg,
//							int idxStartBlk,
//							int idxStartPages,
//							int iPages 
//							)
// 参数：
//		IN lpProcessSeg - 进程段组
//		IN lpSeg - 段结构指针
//		IN idxStartBlk - 内存域中需要重设的起始块
//		IN idxStartPages - 内存域中需要重设的起始块中的开始页
//		IN iPages - 需要重设的页数
// 返回值：
//		假如有释放物理页，返回TRUE；否则，返回FALSE
// 功能描述：
//		释放指定内存域的物理页
// 引用：
//		
// ********************************************************************
#define DEBUG_SEG_DECOMMITPAGES 0
BOOL Seg_DecommitPages( LPPROCESS_SEGMENTS lpProcessSeg, 
					    LPSEGMENT lpSeg, 
						int idxStartBlk, 
						int idxStartPages, 
						int iPages )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxStartBlk; // 开始块
	BOOL bFlush = FALSE;
	UINT uStartPage, uMinPage, uMaxPage;
	BOOL bWriteToSecondPage;

	//需要CACHE ?-2005-02-02
	uMinPage = uStartPage = idxStartBlk * PAGES_PER_BLOCK + idxStartPages;
	uMaxPage = uStartPage + iPages;
	bWriteToSecondPage = GetPagesInCacheRange( lpProcessSeg, &uMinPage, &uMaxPage );

	/*bWriteToSecondPage = FALSE;
	if( lpProcessSeg && lpProcessSeg->lpdwSecondPageTable )
	{
		uStartPage = idxStartBlk * PAGES_PER_BLOCK + idxStartPages;
		//页在范围内
		if( uStartPage >= lpProcessSeg->uiPageTableStart &&
			uStartPage < (lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA) )
		{
			bWriteToSecondPage = TRUE;
		}		
	}
	*/
	//

	while( iPages > 0 )
	{
		ASSERT( *lppBlks != NULL_BLOCK );
		if( *lppBlks <= RESERVED_BLOCK )
		{	// 是保留块，减去其中的页数
			int n = PAGES_PER_BLOCK - idxStartPages;
			iPages -= n;

			uStartPage += n;
		}
		else
		{	// 内存块
			UINT *lpui;
			// 查找块内是否有物理页
			lpui = &(*lppBlks)->uiPages[idxStartPages];
			while( iPages && idxStartPages < PAGES_PER_BLOCK )
			{
				if( *lpui && *lpui != INVALID_PAGE )
				{	// 有有效的物理页，释放它
					DEBUGMSG( DEBUG_SEG_DECOMMITPAGES, ( "Phy=%x,b=%d,p=%d.\r\n", *lpui, idxStartBlk, idxStartPages ) );
					Page_Free( (LPVOID)_GetVirtualPageAdr( *lpui ) );
					*lpui = 0;

					//更新页表-2005-02-03
					if( bWriteToSecondPage && 
						PAGE_IN_RANGE( uStartPage, uMinPage, uMaxPage ) )
					{
						lpProcessSeg->lpdwSecondPageTable[uStartPage] = *lpui;
					}

					bFlush = TRUE; //刷新CACHE
				}
				uStartPage++;

				iPages--;
				lpui++;
				idxStartPages++;				
			}
		}
		idxStartPages = 0;
		lppBlks++;//下一个块
	}
	return bFlush;
}


// ********************************************************************
// 声明：BOOL AutoCommitPage( LPMEMBLOCK lpBlk, int idxStartPage )
// 参数：
//		IN lpBlk - MEMBLOCK（内存块）结构指针
//		IN idxStartPage - 块内的开始页
// 返回值：
//		假如提交成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		自动提交物理页。通常被异常处理调用
// 引用：
//		
// ********************************************************************
// handle by exception handler
#define DEBUG_AUTOCOMMITPAGE 0
//BOOL AutoCommitPage( LPMEMBLOCK lpBlk, int idxStartPage )
BOOL AutoCommitPage( LPMEMBLOCK lpBlk, DWORD dwAddress )
{
	UINT uiPhyPage;
	DWORD dwPhyFlagMask = GetPhyPageFlagMask();
	int idxStartPage = PAGE_INDEX( dwAddress );
	LPPROCESS_SEGMENTS lpProcessSeg;

	ASSERT( lpBlk->uiPages[idxStartPage] == 0 );
	uiPhyPage = (UINT)Page_Alloc( TRUE );//cache 地址
	if( uiPhyPage )
	{
#ifdef __DEBUG
		// 这里是否有一个错误 ？！，因为uiPhyPage是内核CACHE地址>0x80000000，
		// 而该页随后要被用到，并且是已另一个CACHE地址 < 0x80000000 出现.
		// 因此我用UNCACHE地址去清除
	    //memset( (LPVOID)uiPhyPage, 0xCCCCCCCC, PAGE_SIZE );
		//
		memset( (LPVOID)CACHE_TO_UNCACHE(uiPhyPage), 0xCCCCCCCC, PAGE_SIZE );
		//
#endif
		// 得到对应的物理页和属性组合
		uiPhyPage = _GetPhysicalPageAdr( uiPhyPage ) | (lpBlk->uiFlag & dwPhyFlagMask);
	    lpBlk->uiPages[idxStartPage] = uiPhyPage;
		if( ( lpProcessSeg = Seg_FindProcessSegment( dwAddress ) ) )
		{
			ASSERT( lpProcessSeg->lpdwSecondPageTable );
			if( PAGE_INDEX_IN_SEGMENT( dwAddress ) >= lpProcessSeg->uiPageTableStart &&
				PAGE_INDEX_IN_SEGMENT( dwAddress ) < lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA )
			{
				lpProcessSeg->lpdwSecondPageTable[PAGE_INDEX_IN_MEGA( dwAddress )] = uiPhyPage;
			}
		}
		DEBUGMSG( DEBUG_AUTOCOMMITPAGE, ( "auto commit sucess=%x.\r\n", uiPhyPage ) );
	}
	else
	{
#ifdef __DEBUG
		RETAILMSG( 1, ( "auto commit error: no physical page!!! .\r\n" ) );
		RETAILMSG( 1, ( "auto commit error: no physical page!!! .\r\n" ) );
		RETAILMSG( 1, ( "auto commit error: no physical page!!! .\r\n" ) );
		RETAILMSG( 1, ( "auto commit error: no physical page!!! .\r\n" ) );
		while(1);//ASSERT( 0 );  // no physical page!!!
#else
		return FALSE;
#endif
	}
	return TRUE;
}

// ********************************************************************
// 声明：BOOL Seg_MapPhyPages( 
//					  LPPROCESS_SEGMENTS lpProcessSeg,
//					  LPSEGMENT lpSeg, 
//					  int idxCommitBlk, 
//					  int idxPageOffset, 
//					  int iNeedPages, 
//					  int iAllocPages,
//					  UINT uiAllocType,
//					  DWORD * lpdwPhycialPages,
//					  DWORD dwProtect
//					  )
// 参数：
//		IN lpProcessSeg - 进程段组
//		IN lpSeg - 段结构指针 
//		IN idxCommitBlk - 需要提交的开始块
//		IN idxPageOffset - 开始块内的开始页
//		IN iNeedPages - 需要查找的页
//		IN iAllocPages - 需要提交的页
//		IN uiAllocType - 分配类型，为以下值：
//					MEM_PHYSICAL - lpdwPhycialPages 包含物理页，如果没有MEM_CONTIGUOUS属性，
//									则 lpdwPhycialPages[n] 包含第n个物理页。
//									如果没有该属性，则由该函数分配物理页
//					MEM_CONTIGUOUS - 物理页是连续的，并且lpdwPhycialPages本身表示物理页地址的开始
//		IN lpdwPhycialPages - 指向物理页或物理页地址（依赖于uiAllocType）。
//		IN dwProtect - 保护属性
// 返回值：
//		假如成功，返回TRUE;否则，返回FALSE
// 功能描述：
//		将物理页映射到相应的虚空间
// 引用：
//		
// ********************************************************************
#define DEBUG_SEG_MAPPHYPAGES 0
BOOL Seg_MapPhyPages(
					  LPPROCESS_SEGMENTS lpProcessSeg,
					  LPSEGMENT lpSeg, 
					  int idxCommitBlk, 
					  int idxPageOffset, 
					  int iNeedPages, 
					  int iAllocPages,
					  UINT uiAllocType,
					  DWORD * lpdwPhycialPages,
					  DWORD dwProtect
					  )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxCommitBlk; // 开始页
	LPMEMBLOCK lpBlk;
	int iPages = 0;
    BOOL bFlush = FALSE;
	UINT uStartPage, uMinPage, uMaxPage; 
	BOOL bWriteToSecondPage;

	if( (uiAllocType & MEM_PHYSICAL) == 0 )	//由本函数分配物理页吗 ？
	{	// 是
	    if( Page_Lock( iAllocPages ) == FALSE )  // 锁住需要的页
		{	// 不成功
			WARNMSG( DEBUG_SEG_MAPPHYPAGES, ( "Seg_MapPhyPages: failure to LockPages=%d.\r\n", iAllocPages ) );
		    return FALSE;
		}
	}
	// 得到CPU对应的页属性
	dwProtect = GetAttribFromProtect( dwProtect );
	
	//需要CACHE ?-2005-02-02
	uMinPage = uStartPage = idxCommitBlk * PAGES_PER_BLOCK + idxPageOffset;
	uMaxPage = uStartPage + iNeedPages;
	bWriteToSecondPage = GetPagesInCacheRange( lpProcessSeg, &uMinPage, &uMaxPage );
/*
	bWriteToSecondPage = FALSE;
	if( lpProcessSeg->lpdwSecondPageTable )
	{
		uStartPage = idxCommitBlk * PAGES_PER_BLOCK + idxPageOffset;
		//页在范围内
		if( uStartPage >= lpProcessSeg->uiPageTableStart &&
			uStartPage < (lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA) )
		{
			bWriteToSecondPage = TRUE;
		}		
	}
*/
	//
	while( iPages < iNeedPages )
	{
		lpBlk = *lppBlks++;
		ASSERT( lpBlk && lpBlk > RESERVED_BLOCK );

		// 搜索块内的所有页，search the block's all pages
		while( idxPageOffset < PAGES_PER_BLOCK &&
			   iPages < iNeedPages )
		{
			if( lpBlk->uiPages[idxPageOffset] == 0 ) // 分配了物理页吗 ？
			{   // 没有，为其分配一个。now, to alloc a physical page
				int iTryCount = 0;
				UINT uiPhyPage;
				// 虽然不应该出现没有物理页的情况，之前已用Page_Lock锁住需要的页面，但多试几次也无访
				// 不过，现在我觉得应该将循环检查的代码 移到 Page_Lock 部分！！！！
				//	改变它以后
				for( ; iTryCount < 10; iTryCount++ )
				{
					if( (uiAllocType & MEM_PHYSICAL) == 0 )
					{	// 自动分配
					    uiPhyPage = (UINT)Page_Alloc( FALSE );
						if( uiPhyPage )
						    uiPhyPage = _GetPhysicalPageAdr( uiPhyPage );
					}
					else
					{   //用参数给的物理地址 MEM_PHYSICAL
						if( uiAllocType & MEM_CONTIGUOUS )
						{	// 连续的地址，lpdwPhycialPages本身就是物理页地址
							uiPhyPage = (UINT)lpdwPhycialPages; // 当前的物理页地址
							lpdwPhycialPages = (DWORD*)NEXT_PHYSICAL_PAGE( lpdwPhycialPages );
						}
						else
						{   // 非连续的物理页，physical pages is not contigous, put in the pointer
							uiPhyPage = *lpdwPhycialPages;
							lpdwPhycialPages++; // 下一个物理页
						}
					}

					if( uiPhyPage )
					{
						uiPhyPage |= dwProtect; // 或上保护属性
						lpBlk->uiPages[idxPageOffset] = uiPhyPage;
						bFlush = TRUE;  // 刷新CACHE
						//2005-02-02是否CACHE
						if( bWriteToSecondPage && 
							PAGE_IN_RANGE( uStartPage, uMinPage, uMaxPage ) )
						{
							lpProcessSeg->lpdwSecondPageTable[uStartPage] = uiPhyPage;
						}
						//
						break;
					}
					WARNMSG( DEBUG_SEG_MAPPHYPAGES, ("Seg_MapPhyPages, entry sleep.\r\n" ) );
					Sleep(200);
				}
				if( uiPhyPage == 0 )
				{	// 不应该发生，某些错误！
					ERRORMSG( DEBUG_SEG_MAPPHYPAGES, ( "error in Seg_MapPhyPages: fault error!, no enough phypage.\r\n" ) );
				}
			}
			idxPageOffset++; // 块内的下一个页
			iPages++;
			uStartPage++;
		}
		idxPageOffset = 0;
	}
	//
	// 我假定所有虚内存管理中的释放功能将会调用FlushCacheAndClearTLB功能，
	// 所以我在分配功能里不调用FlushCacheAndClearTLB功能
	// 这就是为什么我去掉了以下的代码
	//if( bFlush )
	  //  FlushCacheAndClearTLB();
	return TRUE;
}

// ********************************************************************
// 声明：LPSEGMENT Seg_FindSegmentByIndex( UINT index )
// 参数：
//		IN index - 段索引
// 返回值：
//		段结构指针
// 功能描述：
//		由段索引得到其所在的段
// 引用：
//		
// ********************************************************************
LPSEGMENT Seg_FindSegmentByIndex( UINT index )
{
	if( index < 64 )
	{	// 用户空间 user space
		if( lppProcessSegmentSlots[index] )
		    return (LPSEGMENT)lppProcessSegmentSlots[index]->lpSeg;
	}
	else 
	{	// 内核空间 0x80000000
		if( index == KERNEL_SEGMENT_INDEX )
		{
			return InitKernelProcess.lpProcessSegments->lpSeg;
		}
	}
	return NULL;
}


// ********************************************************************
// 声明：LPSEGMENT Seg_FindSegment( DWORD dwAddress )
// 参数：
//		IN dwAddress - 虚地址
// 返回值：
//		段结构指针
// 功能描述：
//		由虚地址得到其所在的段
// 引用：
//		
// ********************************************************************
#define DEBUG_SEG_FINDSEGMENT 0
static LPSEGMENT Seg_FindSegment( DWORD dwAddress )
{
	return Seg_FindSegmentByIndex( GET_SEGMENT_INDEX( dwAddress ) );
}

// ********************************************************************
// 声明：LPPROCESS_SEGMENTS Seg_FindProcessSegment( DWORD dwAddress )
// 参数：
//		IN dwAddress - 虚地址
// 返回值：
//		段结构指针
// 功能描述：
//		由虚地址得到其所在的段
// 引用：
//		
// ********************************************************************
#define DEBUG_SEG_FINDSEGMENT 0
static LPPROCESS_SEGMENTS Seg_FindProcessSegment( DWORD dwAddress )
{
	UINT index = GET_SEGMENT_INDEX( dwAddress );
	if( index < 64 )
	{	// 用户空间 user space
		return lppProcessSegmentSlots[index];
	}
	else 
	{	// 内核空间 0x80000000
		if( index == KERNEL_SEGMENT_INDEX )
		{
			return InitKernelProcess.lpProcessSegments;
		}
	}
	return NULL;

}


// ********************************************************************
// 声明：BOOL Seg_Copy( 
//				LPPROCESS_SEGMENTS lpProcessSeg,
//				LPSEGMENT lpSegDst, 
//			    UINT idxDstStartBlk,
//				UINT idxStartPage,
//				DWORD dwCopyPages,
//		        LPSEGMENT lpSegSrc,
//				UINT idxSrcStartBlk,
//				DWORD dwProtect )
// 参数：
//		IN lpProcessSeg - 进程段组
//		IN lpSegDst - 目标段结构指针 
//		IN idxDstStartBlk - 目标段开始块
//		IN idxStartPage - 目标段块内开始页
//		IN dwCopyPages - 需要拷贝的页
//		IN lpSegSrc - 源段结构指针
//		IN idxSrcStartBlk - 源段开始块
//		IN dwProtect - 保护属性
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		将源区域的物理页映射到目标区域
// 引用：
//		
// ********************************************************************
BOOL Seg_Copy( 
			    LPPROCESS_SEGMENTS lpProcessSeg,
				LPSEGMENT lpSegDst, 
			    UINT idxDstStartBlk,
				UINT idxStartPage,
				DWORD dwCopyPages,
		        LPSEGMENT lpSegSrc,
				UINT idxSrcStartBlk,
				DWORD dwProtect )
{
	LPMEMBLOCK * lppDstBlks, * lppSrcBlks;
	DWORD dwPhysAddrMask = GetPhyPageAdrMask();
	BOOL bWriteToSecondPage;
	UINT uStartPage, uMinPage, uMaxPage; 
	
	dwProtect = GetAttribFromProtect( dwProtect );//得到CPU的属性
    lppDstBlks = lpSegDst->lpBlks + idxDstStartBlk; // 开始目标块
	lppSrcBlks = lpSegSrc->lpBlks + idxSrcStartBlk;//开始源块


	//需要CACHE ?-2005-02-03, add
	uMinPage = uStartPage = idxDstStartBlk * PAGES_PER_BLOCK + idxStartPage;
	uMaxPage = uStartPage + dwCopyPages;
	bWriteToSecondPage = GetPagesInCacheRange( lpProcessSeg, &uMinPage, &uMaxPage );

	/*
	bWriteToSecondPage = FALSE;
	if( lpProcessSeg->lpdwSecondPageTable )
	{
		uStartPage = idxDstStartBlk * PAGES_PER_BLOCK + idxStartPage;
		//页在范围内
		if( uStartPage >= lpProcessSeg->uiPageTableStart &&
			uStartPage < (lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA) )
		{
			bWriteToSecondPage = TRUE;
		}		
	}
	*/
	//

	while( dwCopyPages )
	{
		UINT * lpuiDstPage = &(*lppDstBlks)->uiPages[idxStartPage];//开始目标页
		UINT * lpuiSrcPage = &(*lppSrcBlks)->uiPages[idxStartPage];//开始源页

		lppDstBlks++; lppSrcBlks++;

		while( idxStartPage < PAGES_PER_BLOCK &&
			   dwCopyPages )
		{
			DWORD dwPhy = *lpuiSrcPage & dwPhysAddrMask; //得到物理地址
			// 拷贝
			Page_Duplicate( (LPVOID)_GetVirtualPageAdr( dwPhy ) );
			// 
			*lpuiDstPage = dwPhy | dwProtect;
			//更新页表-2005-02-03
			if( bWriteToSecondPage &&
				PAGE_IN_RANGE( uStartPage, uMinPage, uMaxPage) )
			{
				lpProcessSeg->lpdwSecondPageTable[uStartPage] = *lpuiDstPage;
			}
			uStartPage++;
			//			

			// 下一页
			lpuiDstPage++;
			lpuiSrcPage++;
			idxStartPage++;
			dwCopyPages--;
		}
		idxStartPage = 0;
	}
	return dwCopyPages ? FALSE : TRUE;
}


// ********************************************************************
// 声明：static LPVOID DoVirtualAlloc( 
//						LPVOID lpvAddress, 
//						DWORD dwSize, 
//						DWORD dwAllocType, 
//						DWORD dwProtect,
//						DWORD * lpdwPhycialPages )
// 参数：
//		IN lpvAddress - 虚地址
//		IN dwSize - 需要分配的大小（以byte为单位）
//		IN uiAllocType - 分配类型，为以下值的组合：
//					MEM_RESERVE - 保留虚地址
//					MEM_COMMIT - 提交物理页
//
//					MEM_PHYSICAL - lpdwPhycialPages 包含物理页，如果没有MEM_CONTIGUOUS属性，
//									则 lpdwPhycialPages[n] 包含第n个物理页。
//									如果没有该属性，则由该函数分配物理页
//					MEM_CONTIGUOUS - 物理页是连续的，并且lpdwPhycialPages本身表示物理页地址的开始
//		IN dwProtect - 保护属性
//		IN lpdwPhycialPages - 指向物理页或物理页地址（依赖于uiAllocType）。
// 返回值：
//		假如成功，返回非NULL指针；否则返回NULL
// 功能描述：
//		分配用户虚地址，假如uiAllocType有MEM_COMMIT但没有MEM_RESERVE，则该地址范围必须是之前已经被保留的
// 引用：
//		
// ********************************************************************
#define DEBUG_DOVIRTUALALLOC 0
static LPVOID DoVirtualAlloc( 
						LPVOID lpvAddress, 
						DWORD dwSize, 
						DWORD dwAllocType, 
						DWORD dwProtect,
						DWORD * lpdwPhycialPages )
{
	DWORD dwStart = (DWORD)lpvAddress;
	DWORD dwEnd = dwStart + dwSize;
	DWORD dwSegIndex = GET_SEGMENT_INDEX( dwStart );
	LPSEGMENT lpSeg;
	LPPROCESS_SEGMENTS lpProcessSeg;
	DWORD dwError = 0;
	DWORD dwRetv = NULL;


	DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtualAlloc:lpvAddress=%x,dwSize=%d.\r\n", lpvAddress,dwSize ) );
	// 检查参数，check param
	if( (dwAllocType & MEM_PHYSICAL) && lpdwPhycialPages == NULL )
	{
        KL_SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	KL_EnterCriticalSection( &csVirtualMem );

	//ASSERT( dwSegIndex < MAX_SEGMENTS && dwSize ); //2005-01-18
//	ASSERT( dwSize );
	
	//if( dwSegIndex < MAX_SEGMENTS && dwSize )
	if( dwSize )
	{
		if( ( lpProcessSeg = Seg_FindProcessSegment( dwStart ) ) &&
			( lpSeg = lpProcessSeg->lpSeg ) )  // 查找对应的段
		{
			int iNeedPages;
			int iPages;
			int idxFirst, idxCommit, idxCommitPage;
			BOOL bReserved = FALSE;

			DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtualAlloc:lpSeg=%x.\r\n", lpSeg ) );

		    idxFirst = -1;
			//是否需要做保留工作 ？
			if( (dwAllocType & MEM_RESERVE) ||
				( lpvAddress == NULL && (dwAllocType & MEM_COMMIT) ) 
			  )
			{   // 需要去保留相应的页 to get enough free block( NULL_BLOCK )
				// 需要的页数
			    iPages = iNeedPages = ( ( dwStart + dwSize + PAGE_SIZE - 1 ) / PAGE_SIZE ) - ( dwStart / PAGE_SIZE );

				dwAllocType |= MEM_RESERVE;
				idxFirst = 0;

				if( dwSegIndex )
				{
					dwStart &= SEGMENT_MASK;  // 32M size
				}
				if( dwStart ) // 用户指定地址 ？
				{  // 是
					dwStart &= 0xffff0000; // 下舍到64k边界 round down 64k
					idxFirst = BLOCK_INDEX( dwStart );
				}
				else // 否，从1块开始（0块系统保留）
					idxFirst = 1;  // first block reserved by sys
				
				// search enough blocks
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:idxFirst=%d,iNeedPages=%d.\r\n", idxFirst, iNeedPages ) );
				//得到内存域的开始块
				idxCommit = idxFirst = Seg_GetFirstBlockPos( lpSeg, idxFirst, iNeedPages, dwStart == NULL );
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:Reserve idxFirst=%d.\r\n", idxFirst ) );
				// 保留需要的页
				if( idxFirst &&
					Seg_ReserveNeedPages( lpSeg, idxFirst, iNeedPages, (dwAllocType & MEM_AUTO_COMMIT), dwProtect ) )
				{   // now to reserve
					bReserved = TRUE;
					dwStart = idxFirst << BLOCK_SHIFT;
				}
				else
				{
					WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc: failuer to ReserveNeedPages(%d), idxFirst(%d), dwStart(0x%x).\r\n", iNeedPages, idxFirst, dwStart ) );
					dwError = ERROR_NOT_ENOUGH_MEMORY;
					goto _err_return;
				}
			}
			else if( dwStart && (dwAllocType & MEM_COMMIT) )
			{   // 直接提交物理页，to commit blocks which have reserved
				idxCommit = idxFirst = BLOCK_INDEX( dwStart );  // 开始块
				// align to page
				//需要的页数
			    iNeedPages = ( ( dwStart + dwSize + PAGE_SIZE - 1 ) / PAGE_SIZE ) - ( dwStart / PAGE_SIZE );
				dwStart &= ~PAGE_MASK;
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:dwStart=%x,idxFirst=%d.\r\n", dwStart, idxFirst ) );
                //得到该域的开始块
				idxFirst = Seg_SearchFirstBlock( lpSeg, idxFirst );
				if( idxFirst == NOT_FIND_FIRST )
				{   // 给的地址是无效的，error param
					WARNMSG( DEBUG_DOVIRTUALALLOC, ( "warn in DoVirtaulAlloc:NOT_FIND_FIRST,idxFirst=%d.\r\n", idxFirst ) );
//					ASSERT( 0 );
					dwError = ERROR_INVALID_PARAMETER;
					goto _err_return;
				}
			}
			//以下代码做提交物理页工作
			// now to commit page if need
			if( dwAllocType & MEM_COMMIT )  ////需要提交物理页吗？
			{   //需要提交物理页
				int iAllocPages;

				idxCommitPage = PAGE_INDEX( dwStart );
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:To Commit,idxCommit=%d,idxCommitPage=%d,iNeedPages=%d.\r\n", idxCommit, idxCommitPage, iNeedPages ) );
				//检视需要提交的页数并分配相应的块结构
				if( ( iAllocPages = Seg_ReviewRegion( lpSeg, idxFirst, idxCommit, idxCommitPage, iNeedPages, NULL ) ) != -1 )
				{   // 成功，提交物理页。ok , success to commit，now, alloc the physical pages for commit page
					DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc: iAllocPages=%d.\r\n", iAllocPages ) );
					if( Seg_MapPhyPages( lpProcessSeg, lpSeg, idxCommit, idxCommitPage, iNeedPages, iAllocPages,  dwAllocType, lpdwPhycialPages, dwProtect ) )
					{
						DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:success to commit=%x.\r\n", dwStart ) );
						dwRetv = dwStart;
						FlushCacheAndClearTLB(); //刷新CACHE
						goto _return;						
					}
					else
					{
						WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:failuer to MapPhyPages.\r\n" ) );
					}
				}
				else
				{
					WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:failuer to ReviewRegion.\r\n" ) );
				}
				// 发生了某些错误，做清除工作。some error happen, now clear reserved
				WARNMSG( DEBUG_DOVIRTUALALLOC, ( "Failuer to Commit.\r\n" ) );
				if( bReserved )
				{
					Seg_FreeRegion( lpSeg, idxFirst );
				}
			}
			else //不需要提交物理页，仅仅保留。no commit , only reserve
			{
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:success to reserve=%x.\r\n", dwStart ) );
				dwRetv = dwStart;
				goto _return;
			}
		}
		else
		{
		    WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc: not find the segment addr=(0x%x), index=%d, knx=%d.\r\n", dwStart, dwSegIndex, KERNEL_SEGMENT_INDEX ) );
			ASSERT( 0 );
		}
	}
_err_return:
	KL_SetLastError( dwError );
	WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:failure alloc address(%x).\r\n", dwStart ) );
_return:
	KL_LeaveCriticalSection( &csVirtualMem );
	if( dwRetv )
	    dwRetv = dwRetv | ( dwSegIndex << SEGMENT_SHIFT );
	RETAILMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:dwRetv(0x%x).\r\n", dwRetv ) );
	return (LPVOID)dwRetv;
}

// ********************************************************************
// 声明：LPVOID WINAPI KL_VirtualAlloc( 
//							LPVOID lpvAddress, 
//							DWORD dwSize, 
//							DWORD dwAllocType, 
//							DWORD dwProtect )
// 参数：
//		IN lpvAddress - 虚地址
//		IN dwSize - 需要分配的大小（以byte为单位）
//		IN dwAllocType - 分配类型，为以下值的组合：
//					MEM_RESERVE - 保留虚地址
//					MEM_COMMIT - 提交物理页
//		IN dwProtect - 保护属性
// 返回值：
//		假如成功，返回非NULL指针；否则返回NULL
// 功能描述：
//		分配用户虚地址，假如uiAllocType有MEM_COMMIT但没有MEM_RESERVE，则该地址范围必须是之前已经被保留的
// 引用：
//		系统API		
// ********************************************************************
#define DEBUG_KL_VirtualAlloc 0
LPVOID WINAPI KL_VirtualAlloc( 
							LPVOID lpvAddress, 
							DWORD dwSize, 
							DWORD dwAllocType, 
							DWORD dwProtect )
{
	DEBUGMSG( DEBUG_KL_VirtualAlloc, ( "KL_VirtualAlloc entry,lpvAddress=0x%x,dwSize=%d,dwAllocType=0x%x.\r\n", lpvAddress, dwSize, dwAllocType ) );
	if( (DWORD)lpvAddress < 0x80000000 )
		return DoVirtualAlloc( lpvAddress, dwSize, dwAllocType, dwProtect, NULL );
	else
		return NULL;
}

// ********************************************************************
// 声明：LPVOID WINAPI KC_VirtualAlloc( 
//							LPVOID lpvAddress, 
//							DWORD dwSize, 
//							DWORD dwAllocType, 
//							DWORD dwProtect )
// 参数：
//		IN lpvAddress - 虚地址
//		IN dwSize - 需要分配的大小（以byte为单位）
//		IN dwAllocType - 分配类型，为以下值的组合：
//					MEM_RESERVE - 保留虚地址
//					MEM_COMMIT - 提交物理页
//		IN dwProtect - 保护属性
// 返回值：
//		假如成功，返回非NULL指针；否则返回NULL
// 功能描述：
//		内核版，分配用户虚地址，假如uiAllocType有MEM_COMMIT但没有MEM_RESERVE，则该地址范围必须是之前已经被保留的
// 引用：
//		内核版
// ********************************************************************
#define DEBUG_KC_VirtualAlloc 0
LPVOID WINAPI KC_VirtualAlloc( 
							LPVOID lpvAddress, 
							DWORD dwSize, 
							DWORD dwAllocType, 
							DWORD dwProtect )
{
	DEBUGMSG( DEBUG_KC_VirtualAlloc, ( "KC_VirtualAlloc entry,lpvAddress=0x%x,dwSize=%d.\r\n", lpvAddress, dwSize ) );
	return DoVirtualAlloc( lpvAddress, dwSize, dwAllocType, dwProtect, NULL );
}


// ********************************************************************
// 声明：BOOL DoVirtualCopy( 
//						DWORD dwDstAddress, 
//						DWORD dwSrcAddress,
//						DWORD dwSize, 
//						DWORD dwProtect
//						)
// 参数：
//		IN dwDstAddress - 目标地址
//		IN dwSrcAddress - 源地址
//		IN dwSize - 需要拷贝的大小
//		IN dwProtect - 保护属性
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		将源地址的物理页映射到目标地址
// 引用：
//		
// ********************************************************************

#define DEBUG_VIRTUALCOPY 0
static BOOL DoVirtualCopy( 
						DWORD dwDstAddress, 
						DWORD dwSrcAddress,
						DWORD dwSize, 
						DWORD dwProtect
						)
{
	DWORD dwDstStart = dwDstAddress;
	DWORD dwDstEnd = dwDstStart + dwSize;
	DWORD dwSegIndex = GET_SEGMENT_INDEX( dwDstStart );
	LPSEGMENT lpSegDst;
	LPPROCESS_SEGMENTS lpProcessSegDst;
	DWORD dwError = 0;
	DWORD dwRetv = NULL;
	DWORD dwPhyAdr = 0;
	UINT uiNeedPages;
	
	BOOL bVirtualAdr = FALSE;
	BOOL bRetv = FALSE;

	if( dwSize == 0 || dwDstStart == 0 || dwSrcAddress == 0 || dwSegIndex >= MAX_SEGMENTS )
		goto _RET_INVALID;

	uiNeedPages = ( ( dwDstEnd + PAGE_SIZE - 1 ) / PAGE_SIZE ) - ( dwDstStart / PAGE_SIZE );

	if( dwProtect & PAGE_PHYSICAL ) // dwSrcAddress是物理地址吗 ？
	{	//是
		dwPhyAdr = (dwSrcAddress << 8) & ~(PAGE_SIZE-1); // align to page
	    if( ( dwPhyAdr & PAGE_MASK ) != ( dwDstStart & PAGE_MASK ) )  // 源与目标应该对齐页边界
			goto _RET_INVALID;

		dwProtect &= ~PAGE_PHYSICAL;
	}
	else if( IsKernelVirtualAddress( dwSrcAddress ) )  // 源是内核地址吗 ？
	{	//是
		dwPhyAdr = _GetPhysicalPageAdr( dwSrcAddress ); // 得到对应的物理地址
	    if( ( dwPhyAdr & PAGE_MASK ) != ( dwDstStart & PAGE_MASK ) ) // 源与目标应该对齐页边界
			goto _RET_INVALID;
	}
	else
	{   // 源在用户地址空间。from other virtual address
		bVirtualAdr = TRUE;
	}

	KL_EnterCriticalSection( &csVirtualMem );  // 进入冲突段

	//lpSegDst = Seg_FindSegment( dwDstStart );  // 发现目标段
	lpProcessSegDst = Seg_FindProcessSegment( dwDstStart );
	if( lpProcessSegDst )
		lpSegDst = lpProcessSegDst->lpSeg;
	else
		lpSegDst = NULL;

	if( lpSegDst )
	{	//
		UINT idxDstStartBlk;
		UINT idxDstStartPage;
		UINT idxFirstBlk;
		UINT uiNeedAllocPages;

        idxDstStartBlk = BLOCK_INDEX( dwDstAddress );
       	idxDstStartPage = PAGE_INDEX( dwDstAddress );

 	    idxFirstBlk = Seg_SearchFirstBlock( lpSegDst, idxDstStartBlk );		

		if( idxFirstBlk == NOT_FIND_FIRST )
			goto _RET_INVALID_LEAVE;

		// 检查目标区域，保证所用需要拷贝的page已被保留。	
	    uiNeedAllocPages = Seg_ReviewRegion( lpSegDst, idxFirstBlk, idxDstStartBlk, idxDstStartPage, uiNeedPages, NULL );
		if( uiNeedAllocPages != uiNeedPages )
			goto _RET_INVALID_LEAVE;
		if( bVirtualAdr )
		{   // 源在用户地址空间，from other virtual address
			LPSEGMENT lpSegSrc;
			UINT idxSrcStartBlk;
			UINT idxSrcStartPage;
			UINT idxFirstBlk;
			UINT uiAllocPages;
			UINT uiCountPages;
			
			idxSrcStartPage = PAGE_INDEX( dwSrcAddress );
			if( idxSrcStartPage != idxDstStartPage )
				goto _RET_INVALID_LEAVE;

			idxSrcStartBlk = BLOCK_INDEX( dwSrcAddress );


			lpSegSrc = Seg_FindSegment( dwSrcAddress );
			if( lpSegSrc == NULL )
				goto _RET_INVALID_LEAVE;
			
			idxFirstBlk = Seg_SearchFirstBlock( lpSegSrc, idxSrcStartBlk );
		    if( idxFirstBlk == NOT_FIND_FIRST )
			    goto _RET_INVALID_LEAVE;
			
			// 保证所用需要的page已被提交。
			
			uiAllocPages = Seg_ReviewRegion( lpSegSrc, idxFirstBlk, idxSrcStartBlk, idxSrcStartPage, uiNeedPages, &uiCountPages );
			if( uiAllocPages || uiCountPages != uiNeedPages )
				goto _RET_INVALID_LEAVE;
			bRetv = Seg_Copy( lpProcessSegDst, lpSegDst, idxDstStartBlk, idxDstStartPage, uiNeedPages, lpSegSrc, idxSrcStartBlk, dwProtect );
		}
		else
		{   // 源是物理地址，直接映射。phy address
			bRetv = Seg_MapPhyPages( lpProcessSegDst, lpSegDst, idxDstStartBlk, idxDstStartPage, uiNeedPages, uiNeedAllocPages, MEM_PHYSICAL | MEM_CONTIGUOUS, (DWORD*)dwPhyAdr, dwProtect );
		}
	}

	if( bRetv )
		FlushCacheAndClearTLB(); // 因为有重新映射，必须刷新CACHE

	KL_LeaveCriticalSection( &csVirtualMem );//离开冲突段
	return bRetv;

_RET_INVALID_LEAVE:
	KL_LeaveCriticalSection( &csVirtualMem );
_RET_INVALID:
	KL_SetLastError( ERROR_INVALID_PARAMETER );
	WARNMSG( DEBUG_VIRTUALCOPY, ( "DoVirtaulCopy:failure alloc.\r\n" ) );
	return bRetv;
}

// ********************************************************************
// 声明：BOOL WINAPI KL_VirtualCopy( 
//							 LPVOID lpvDest,
//							 LPVOID lpvSrc,
//							 DWORD cbSize,
//							 DWORD fdwProtect )
// 参数：
//		IN dwDstAddress - 目标地址
//		IN dwSrcAddress - 源地址
//		IN dwSize - 需要拷贝的大小
//		IN dwProtect - 保护属性
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		将源地址的物理页映射到目标地址
// 引用：
//		系统API	
// ********************************************************************
#define DEBUG_KL_VirtualCopy 0
BOOL WINAPI KL_VirtualCopy( 
							 LPVOID lpvDest,
							 LPVOID lpvSrc,
							 DWORD cbSize,
							 DWORD fdwProtect )
{
	DEBUGMSG( DEBUG_KL_VirtualCopy, ( "KL_VirtualAlloc entry,lpvDest=0x%x,lpvSrc=0x%x,cbSize=%d.\r\n", lpvDest, lpvSrc, cbSize ) );
	return DoVirtualCopy( (DWORD)lpvDest, (DWORD)lpvSrc, cbSize, fdwProtect ) ? TRUE : FALSE;
}

// ********************************************************************
// 声明：static BOOL DoVirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
// 参数：
//		IN lpAddress - 虚地址
//		IN dwSize - 虚内存大小
//		IN dwFreeType - 释放类型，可以为：
//				MEM_DECOMMIT - 释放一段区域的物理页
//				MEM_RELEASE - 释放整个域，dwSize必须为0
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		释放虚内存域/地址
// 引用：
//		
// ********************************************************************
static BOOL DoVirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
{
	DWORD dwStart = (DWORD)lpAddress;
	DWORD dwSegIndex;
	int idxFirstBlk, idxStartBlk, idxPage;
	int iPages;
	LPSEGMENT lpSeg;
	LPPROCESS_SEGMENTS lpProcessSeg;
	BOOL bRetv = FALSE;
	BOOL bFlush = FALSE;

	// check param
	// 检查参数是否有效
	if( lpAddress == NULL || 
		( (dwFreeType & MEM_RELEASE) && dwSize ) )
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	KL_EnterCriticalSection( &csVirtualMem );

	dwSegIndex = GET_SEGMENT_INDEX( dwStart );

	if( ( lpProcessSeg = Seg_FindProcessSegment( dwStart ) ) &&
		( lpSeg = lpProcessSeg->lpSeg ) )  // 得到地址对应的段指针
	{
		idxStartBlk = BLOCK_INDEX( dwStart );
		idxFirstBlk = Seg_SearchFirstBlock( lpSeg, idxStartBlk ); // 得到该域的第一个块
		idxPage = PAGE_INDEX( dwStart );
		if( idxFirstBlk != NOT_FIND_FIRST )  // 块有效吗 ？
		{	// 有效
			int iReserved;
			int iCountPages = 0;
			if( dwFreeType == MEM_DECOMMIT && dwSize )
			{	// 释放一段区域
                iPages = ( ( dwStart + dwSize + PAGE_SIZE - 1 ) >> PAGE_SHIFT ) - ( dwStart >> PAGE_SHIFT );
				// 统计保留的页数 和 已分配的页数
				iReserved = Seg_ReviewRegion( lpSeg, idxFirstBlk, idxStartBlk, idxPage, iPages, &iCountPages );
				if( iCountPages >= iPages )
				{
					if( iReserved != iPages )
					{   //先刷新CACHE 
						FlushCacheAndClearTLB();
						bFlush |= Seg_DecommitPages( lpProcessSeg, lpSeg, idxStartBlk, idxPage, iPages );
						bRetv = TRUE;
						goto _return;
					}
				}
				else
				{   //error
					ASSERT( 0 );
				}
			}
			else if( dwFreeType & MEM_RELEASE )
			{	//释放整个域
                if( idxPage == 0 && idxFirstBlk == idxStartBlk )
                {	// 统计保留的页数 和 已分配的页数
					iReserved = Seg_ReviewRegion( lpSeg, idxFirstBlk, idxStartBlk, 0, -1, &iCountPages );
				    if( iReserved != iCountPages )
					{
						FlushCacheAndClearTLB();
						bFlush |= Seg_DecommitPages( lpProcessSeg, lpSeg, idxStartBlk, 0, iCountPages );
					}
					// 释放该域已定位的块
					Seg_FreeRegion( lpSeg, idxStartBlk );
					bRetv = TRUE;
					goto _return;
				}
			}
		}
	}
_return:
	//调用Seg_DecommitPages之前已经调用了FlushCacheAndClearTLB，这里必要吗 ？
	if( bFlush )
        FlushCacheAndClearTLB();
	KL_LeaveCriticalSection( &csVirtualMem );
	return bRetv;
}

// ********************************************************************
// 声明：BOOL WINAPI KL_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
//		IN lpAddress - 虚地址
//		IN dwSize - 虚内存大小
//		IN dwFreeType - 释放类型，可以为：
//				MEM_DECOMMIT - 释放一段区域的物理页
//				MEM_RELEASE - 释放整个域，dwSize必须为0
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		释放虚内存域/地址
// 引用：
//		系统API	
// ********************************************************************
#define DEBUG_KL_VirtualFree 0
BOOL WINAPI KL_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
{
	DEBUGMSG( DEBUG_KL_VirtualFree, ( "KL_VirtualFree entry,lpAddress=0x%x,dwSize=0x%x.\r\n", lpAddress, dwSize ) );
	if( (DWORD)lpAddress < 0x80000000 )
		return DoVirtualFree( lpAddress, dwSize, dwFreeType );
	else
		return FALSE;
}

// ********************************************************************
// 声明：BOOL WINAPI KC_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
//		IN lpAddress - 虚地址
//		IN dwSize - 虚内存大小
//		IN dwFreeType - 释放类型，可以为：
//				MEM_DECOMMIT - 释放一段区域的物理页
//				MEM_RELEASE - 释放整个域，dwSize必须为0
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		释放虚内存域/地址
// 引用：
//		内核版
// ********************************************************************
#define DEBUG_KC_VirtualFree 0
BOOL WINAPI KC_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
{
	DEBUGMSG( DEBUG_KC_VirtualFree, ( "KC_VirtualFree entry,lpAddress=0x%x,dwSize=0x%x.\r\n", lpAddress, dwSize ) );
	return DoVirtualFree( lpAddress, dwSize, dwFreeType );
}


// ********************************************************************
// 声明：BOOL WINAPI KL_VirtualProtect( LPVOID lpvAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect )
// 参数：
//		IN lpAddress - 虚地址
//		IN dwSize - 虚内存大小
//		IN flNewProtect - 新的保护属性
//		IN lpfOldProtect - 之前的保护属性
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		重新设置内存域的页属性
// 引用：
//		系统API
// ********************************************************************
#define DEBUG_KL_VirtualProtect 0
BOOL WINAPI KL_VirtualProtect( LPVOID lpvAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect )
{
	DWORD dwStart;// = (DWORD)lpvAddress;
	DWORD dwSegIndex;// = GET_SEGMENT_INDEX( (DWORD)dwStart );
	LPSEGMENT lpSeg;
	DWORD dwError = 0;
	LPPROCESS_SEGMENTS lpProcessSeg;

	DEBUGMSG( DEBUG_KL_VirtualProtect, ( "KL_VirtualProtect entry,lpvAddress=0x%x,dwSize=0x%x.\r\n", lpvAddress, dwSize ) );

	// 检查参数
	if( dwSize == 0 || 
		lpflOldProtect == NULL ||
		lpvAddress == NULL )
	{
        KL_SetLastError(  ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	dwStart = (DWORD)lpvAddress;
	dwSegIndex = GET_SEGMENT_INDEX( (DWORD)dwStart );

	KL_EnterCriticalSection( &csVirtualMem );

	if( dwSegIndex < MAX_SEGMENTS )
	{
		if( ( lpProcessSeg = Seg_FindProcessSegment( dwStart ) ) &&
			( lpSeg = lpProcessSeg->lpSeg ) )  // 得到虚地址对应的段指针
		//if( ( lpSeg = Seg_FindSegment( dwStart ) ) ) // 得到虚地址所在的段
		{
			DWORD idxFirstBlk, idxStartBlk = BLOCK_INDEX( dwStart );
			DWORD idxStartPage;
			DWORD dwPages;
			// 2004-01-28， this is a error, i will test it
			//idxStartBlk = Seg_SearchFirstBlock( lpSeg, idxFirstBlk );
			idxFirstBlk = Seg_SearchFirstBlock( lpSeg, idxStartBlk ); // 得到该内存域的第一个块
			//if( idxStartBlk != NOT_FIND_FIRST )
			if( idxFirstBlk != NOT_FIND_FIRST )
			// 2004-01-28
			{
				idxStartPage = PAGE_INDEX( dwStart );
				dwPages = ( ( dwStart + dwSize + PAGE_SIZE - 1 ) / PAGE_SIZE ) - ( dwStart / PAGE_SIZE );
				// 扫描需要的页是否都已经提交
				if( Seg_ScanCommitRegion( lpSeg, idxFirstBlk, idxStartBlk, idxStartPage, dwPages ) )
				{	//是
					DWORD dwNewAttrib;
					//得到当前的属性
					*lpflOldProtect = GetProtectFromAttrib( lpSeg->lpBlks[idxStartBlk]->uiPages[idxStartPage] );
					dwNewAttrib = GetAttribFromProtect( flNewProtect );
					if( dwNewAttrib )
					{	//设置新的属性
					    Seg_ResetPageAttrib( lpProcessSeg, lpSeg, idxFirstBlk, idxStartBlk, idxStartPage, dwPages, dwNewAttrib );
						FlushCacheAndClearTLB(); //刷新CACHE
					}
				}
			}
			else
			{   // 虚地址无效
				//ASSERT( 0 );
				dwError = ERROR_INVALID_PARAMETER;
				WARNMSG( DEBUG_KL_VirtualProtect, ( "KL_VirtualProtect: invalid address=0x%x,dwSize=0x%x.\r\n", lpvAddress, dwSize ) );
				goto _err_return;
			}
		}
	}
_err_return:
	if( dwError )
		KL_SetLastError( dwError );
	KL_LeaveCriticalSection( &csVirtualMem );
	return dwError ? FALSE : TRUE;
}

// ********************************************************************
// 声明：LPVOID WINAPI KL_AllocPhysMem( 
//						   DWORD dwSize,
//						   DWORD fdwProtect,
//						   DWORD dwAlignmentMask,
//						   DWORD dwFlags,
//						   ULONG * pPhysicalAddress
//						   )
// 参数：
//		IN dwSize - 需要分配的大小
//		IN fdwProtect - 保护属性
//		IN dwAlignmentMask - 开始地址对齐模式
//		IN dwFlags - 标志
//		OUT pPhysicalAddress - 用于接受分配的物理地址
// 返回值：
//		假如成功，返回非NULL的虚地址，pPhysicalAddress返回对应的物理地址；否则，返回NULL
// 功能描述：
//		分配连续的物理页
// 引用：
//		系统API
// ********************************************************************
#define DEBUG_KL_AllocPhysMem 0
LPVOID WINAPI KL_AllocPhysMem( 
						   DWORD dwSize,
						   DWORD fdwProtect,
						   DWORD dwAlignmentMask,
						   DWORD dwFlags,
						   ULONG * pPhysicalAddress
						   )
{
   	void * p;
	DWORD dwPageSize = ALIGN_PAGE_UP( dwSize );
    LPVOID lpVAdr = NULL;

	DEBUGMSG( DEBUG_KL_AllocPhysMem, ( "KL_AllocPhysMem entry,dwSize=0x%x.\r\n", dwSize ) );

	KL_EnterCriticalSection( &csVirtualMem );
	// 得到连续的页
	p = Page_GetContiguous( dwPageSize / PAGE_SIZE, dwAlignmentMask );
	if( p )
	{	//
	    DWORD dwPhysicalAddress = _GetPhysicalPageAdr( (DWORD)p ); // 得到p对应的物理地址
		//将p映射到用户空间
		lpVAdr = DoVirtualAlloc( NULL, 
								dwPageSize, 
								MEM_COMMIT | MEM_RESERVE | MEM_PHYSICAL | MEM_CONTIGUOUS, 
								fdwProtect | PAGE_NOCACHE, 
								(LPVOID)dwPhysicalAddress );
        if( lpVAdr )
		{
			*pPhysicalAddress = dwPhysicalAddress;
		}
		else
		{	//失败。
			Page_ReleaseContiguous( p, dwPageSize / PAGE_SIZE );
		}
	}

	KL_LeaveCriticalSection( &csVirtualMem );
	return lpVAdr; 
}

// ********************************************************************
// 声明：BOOL WINAPI KL_FreePhysMem( LPVOID lpvAddress )
// 参数：
//		IN lpvAddress - 虚地址（用KL_AllocPhysMem分配的虚地址）
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		释放之前用 KL_AllocPhysMem 分配的地址
// 引用：
//		系统API
// ********************************************************************
#define DEBUG_KL_FreePhysMem 0
BOOL WINAPI KL_FreePhysMem( LPVOID lpvAddress )
{
	DEBUGMSG( DEBUG_KL_FreePhysMem, ( "KL_FreePhysMem entry,lpvAddress=0x%x.\r\n", lpvAddress ) );
	return DoVirtualFree( lpvAddress, 0, MEM_RELEASE );
}



