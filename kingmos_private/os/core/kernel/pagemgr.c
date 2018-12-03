/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：页管理模块，该模块将系统所有空闲内存以页为单位组织起来；
          这是一个最基本的内存管理模块，其他所有内存分配函数都从该
		  模块得到以页为单位的内存


版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/


#include <eframe.h>
#include <epcore.h>
#include <pagemgr.h>
#include <oemfunc.h>
#include <cpu.h>

#define FLAG_USED 1
#define FLAG_FREE 0
#define MAX_USED_COUNT 0xff


LPBYTE  lpbSysMainMem;   // 系统主内存开始位置
LPBYTE  lpbSysMainMemEnd; // 系统主内存结束位置
DWORD   dwHandleBase;  // 句柄的内存基本位置
ULONG   ulSysMainMemLength; //系统主内存大小

#ifdef INLINE_PROGRAM
//extern UINT _uiHeapSize = 0;
#endif

// 由地址和该地址所在的段得到该地址对应的索引号
#define GET_INDEX( lpSegInfo, lpvMemAdr ) ( ( (LPBYTE)(lpvMemAdr) - (lpSegInfo)->lpbFreePageStart ) / PAGE_SIZE )

// 内存配置信息
static MEMINFO memInfo;
// 空闲页链表
static LPPAGELIST lpFreePageList = NULL;
// 空闲页数
static UINT    uiFreePages = 0;

// ********************************************************************
//声明：void Page_LinkToList( LPPAGELIST lpList ) 
//参数：
//	IN lpList - 以PAGELIST结构指针表示的空闲页
//返回值：
//	无
//功能描述：
//	将一个空闲页插入空闲链表
//引用：
// ********************************************************************
static void Page_LinkToList( LPPAGELIST lpList ) 
{
	lpList->lpNext = lpFreePageList;
	lpList->lpPrev = NULL;
	if( lpFreePageList )
		lpFreePageList->lpPrev = lpList;
	lpFreePageList = lpList;
}

// ********************************************************************
//声明：void Page_UnlinkFromList( LPPAGELIST lpList )
//参数：
//	IN lpList - 以PAGELIST结构指针表示的空闲页
//返回值：
//	无
//功能描述：
//	将一个空闲页移出空闲链表
//引用：
// ********************************************************************

static void Page_UnlinkFromList( LPPAGELIST lpList )
{
	if( lpList->lpNext )
	{
		ASSERT( lpList->lpNext->lpPrev == lpList );
		lpList->lpNext->lpPrev = lpList->lpPrev;
	}
	if( lpList->lpPrev )
	{
		ASSERT( lpList->lpPrev->lpNext == lpList );
		lpList->lpPrev->lpNext = lpList->lpNext;
	}
	if( lpFreePageList == lpList )
		lpFreePageList = lpList->lpNext;
	lpList->lpNext = lpList->lpPrev = NULL;
}

// ********************************************************************
//声明：void Page_AddToFreeList( LPPAGELIST lpList )
//参数：
//	IN lpList - 以PAGELIST结构指针表示的页
//返回值：
// 无
//功能描述：
//	将一个页插入空闲链表并增加空闲页计数
//引用：
// ********************************************************************

static void Page_AddToFreeList( LPPAGELIST lpList )
{
	Page_LinkToList( lpList );
    uiFreePages++;
}

// ********************************************************************
//声明：void * Page_RemoveFromFreeList( BOOL bUpdateFreePages )
//参数：
//  IN bUpdateFreePages - 是否更新空闲页计数

//返回值：
// 无
//功能描述：
//	将一个页移出空闲链表并根据需要减少空闲页计数，在调用该函数之前必须锁住中断!!
//引用：
// ********************************************************************

static void * Page_RemoveFromFreeList( BOOL bUpdateFreePages )
{
	LPPAGELIST lpList = lpFreePageList;
	Page_UnlinkFromList( lpFreePageList );//lpList );
	if( bUpdateFreePages )
	    uiFreePages--;
	return lpList;
}

// ********************************************************************
//声明：LPSEGMENTINFO Page_GetSeg( void * lpvMemAdr  )
//参数：
//	IN lpvMemAdr - 内存地址
//返回值：
//	假如成功，返回该地址所在的内存段信息；否则NULL
//功能描述：
//	得到一个地址所在的内存段信息，一个内存段是一个连续的能够存取的内存空间
//引用：
// ********************************************************************

static LPSEGMENTINFO Page_GetSeg( void * lpvMemAdr  )
{
	LPSEGMENTINFO lpSegInfo = memInfo.lpSectionInfo;
	DWORD dwSections = memInfo.dwSections;

	for( ; dwSections; dwSections-- )
	{
		if( (LPBYTE)lpvMemAdr >= lpSegInfo->lpbFreePageStart &&
			(LPBYTE)lpvMemAdr < lpSegInfo->lpbFreePageEnd )
		{
			return lpSegInfo;
		}
		lpSegInfo++;
	}
	return NULL;
}

// ********************************************************************
//声明：BOOL Page_Duplicate( void * lpvMemAdr )
//参数：
//	IN lpvMemAdr - 内存地址
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	增加对该内存的引用计数
//引用：
// ********************************************************************

BOOL Page_Duplicate( void * lpvMemAdr )
{
	LPSEGMENTINFO lpSegInfo;
	UINT uiSave;
	int index;

	lpvMemAdr = (LPVOID)CACHE_TO_UNCACHE( lpvMemAdr );

	lpSegInfo = Page_GetSeg( lpvMemAdr );
//	ASSERT( lpSegInfo );

	LockIRQSave( &uiSave );

	index = GET_INDEX( lpSegInfo, lpvMemAdr );
	ASSERT( lpSegInfo->lpbFreeMemMap[index] != FLAG_FREE );
	if( lpSegInfo->lpbFreeMemMap[index] < MAX_USED_COUNT ) 
	    lpSegInfo->lpbFreeMemMap[index]++;

	UnlockIRQRestore( &uiSave );

	return TRUE;
}

// ********************************************************************
//声明：BOOL Page_GetSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex )
//参数：
//	IN lpSegInfo - 内存段指针
//	IN dwIndex - 页索引号
//返回值：
//	假如成功，返回TRUE；否则FALSE
//功能描述：
//	假如一个特定的页是空闲的，则将其设为占用状态
//引用：
// ********************************************************************

static BOOL Page_GetSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex )
{
	UINT uiSave;

    LockIRQSave( &uiSave );

	ASSERT( dwIndex < lpSegInfo->dwTotalPages );
	if( lpSegInfo->lpbMemStart[dwIndex] == FLAG_FREE )
	{	//空闲的
		lpSegInfo->lpbMemStart[dwIndex] = FLAG_USED;
		Page_UnlinkFromList( (LPPAGELIST)( lpSegInfo->lpbFreePageStart + dwIndex * PAGE_SIZE ) );
		
		UnlockIRQRestore( &uiSave );
		return TRUE;
	}

    UnlockIRQRestore( &uiSave );
	return FALSE;	
}

// ********************************************************************
//声明：BOOL Page_ReleaseSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex )
//参数：
//	IN lpSegInfo - 内存段指针
//	IN dwIndex - 页索引号
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	与Page_GetSpecific相对应，假如一个特定的页是占用状态，则将其设为空闲状态
//引用：
// ********************************************************************

static BOOL Page_ReleaseSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex )
{
	UINT uiSave;

    LockIRQSave( &uiSave );

	ASSERT( dwIndex < lpSegInfo->dwTotalPages );
	ASSERT( lpSegInfo->lpbMemStart[dwIndex] == FLAG_USED );

	if( lpSegInfo->lpbMemStart[dwIndex] == FLAG_USED )
	{	// 增加到链表
		lpSegInfo->lpbMemStart[dwIndex] = FLAG_FREE;
		Page_LinkToList( (LPPAGELIST)( lpSegInfo->lpbFreePageStart + dwIndex * PAGE_SIZE ) );
	}

	UnlockIRQRestore( &uiSave );
	return TRUE;
}

// ********************************************************************
//声明：UINT Page_CountFreePages( void )
//参数：
//	无
//返回值：
//	返回空闲页数
//功能描述：
//	得到系统空闲页数
//引用：
// ********************************************************************

UINT Page_CountFreePages( void )
{
	return uiFreePages;
}

// ********************************************************************
//声明：BOOL Page_Lock( DWORD dwPages )
//参数：
// IN dwPages - 页数
//
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	锁住相应的页数
//引用：
// ********************************************************************

BOOL Page_Lock( DWORD dwPages )
{
	BOOL bRetv;
	UINT uiSave;

    LockIRQSave( &uiSave );

	if( uiFreePages >= dwPages )
	{	//	假如空闲页满足需要的页数，则减去需要的页数
		uiFreePages -= dwPages;
		bRetv = TRUE;
	}
	else
		bRetv = FALSE;


	UnlockIRQRestore( &uiSave );
	return bRetv;
}

// ********************************************************************
//声明：BOOL Page_Unlock( DWORD dwPages )
//参数：
//	IN dwPages - 页数 
//
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	与Page_Lock相对应，解锁相应的页数
//引用：
//
// ********************************************************************

BOOL Page_Unlock( DWORD dwPages )
{
	UINT uiSave;

    LockIRQSave( &uiSave );

    uiFreePages += dwPages;

    UnlockIRQRestore( &uiSave );

	return TRUE;
}

// ********************************************************************
//声明：void * Page_Alloc( BOOL bUpdateFreePages )
//参数：
//	IN bUpdateFreePages - 是否更新空闲页计数,如果为TRUE,则要检查 uiFreePages 是否 > 0; 如果为FALSE, 则不检查（之前用Page_Lock已经更新了uiFreePages）
//
//返回值：
//	假如成功，返回有效内存页地址；否则返回NULL
//功能描述：
//	分配一内存页
//引用：
// ********************************************************************

void * Page_Alloc( BOOL bUpdateFreePages )
{
    void * lp = NULL;
	UINT uiSave;

    LockIRQSave( &uiSave );

	// 空闲页表是否为空？
	if( (bUpdateFreePages && uiFreePages) || 
		(bUpdateFreePages == FALSE && lpFreePageList) )
	//if( lpFreePageList )
	{   // 不为空
	    LPSEGMENTINFO lpSegInfo;

		//lp = lpFreePageList;
		lp = Page_RemoveFromFreeList( bUpdateFreePages );
		//	得到该页所在的内存段
		lpSegInfo = Page_GetSeg( lp );
//		ASSERT( lpSegInfo );
		//	将该页在内存段相应的位设为占用状态
		lpSegInfo->lpbFreeMemMap[GET_INDEX( lpSegInfo, lp )] = FLAG_USED;
	}
	UnlockIRQRestore( &uiSave );
	//	如果有CACHE功能，将其转化为CACHE指针
	return lp ? (void*)UNCACHE_TO_CACHE( lp ) : NULL;
}

// ********************************************************************
//声明：BOOL Page_Free( LPVOID lpvMemAdr  )
//参数：
// IN lpvMemAdr - 需要释放的页
//
//返回值：
// 假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	与Page_Alloc相对应，释放一个页内存。
//引用：
// ********************************************************************
#define DEBUG_Page_Free 0
BOOL Page_Free( LPVOID lpvMemAdr  )
{
	LPSEGMENTINFO lpSegInfo;

//	ASSERT( lpvMemAdr );
	//	如果有CACHE功能，将其转化为UNCACHE指针
	lpvMemAdr = (LPVOID)CACHE_TO_UNCACHE( lpvMemAdr );
	//	得到该内存所在的页
	lpSegInfo = Page_GetSeg( lpvMemAdr );

	if( lpSegInfo )
	{
	    UINT uiSave;
		int index;
		// 得到该页在内存段的索引号
        index = GET_INDEX( lpSegInfo, lpvMemAdr );
		LockIRQSave( &uiSave );

		//ASSERT( lpSegInfo->lpbFreeMemMap[index] );
		// 该内存页的引用计数是否为0
		if( lpSegInfo->lpbFreeMemMap[index] )
		{
			if( --lpSegInfo->lpbFreeMemMap[index] == 0 )
				Page_AddToFreeList( lpvMemAdr );  //是，将其加入到空闲链表
		}
		else
		{
			ERRORMSG( DEBUG_Page_Free, ( "error in Page_Free: lpvMemAdr=0x%x.\r\n", lpvMemAdr ) );
		}

		UnlockIRQRestore( &uiSave );
	}
	else
	{	// 不一定是错，有可能是代码页，也通过该函数
		;//RETAILMSG( 1, ( "failure error at Page_Free(0x%x): lpSegInfo = NULL!.\r\n", lpvMemAdr ) );
	}

	return TRUE;
}

// ********************************************************************
//声明：BOOL Page_Get( LPVOID * lppAdr, DWORD dwPages )
//参数：
//	IN lppAdr - 指向用于接收页地址的指针数组，该数组的大小应该等于dwPages -> lppAdr[dwPages]
//	IN dwPages - 需要的页数
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	得到一些页内存
//引用：
// ********************************************************************

BOOL Page_Get( LPVOID * lppAdr, DWORD dwPages )
{
	// 先锁住需要的页
	if( Page_Lock( dwPages ) )
	{	//锁住成功
		while( dwPages-- )
		{	//得到每个页
			*lppAdr++ = Page_Alloc( FALSE ); 
		}
		return TRUE;
	}    
	return FALSE;
}

// ********************************************************************
//声明：BOOL Page_Release( LPVOID * lppAdr, DWORD dwPages )
//参数：
//	IN lppAdr - 指向用于保存有页地址的指针数组，该数组的大小应该等于dwPages -> lppAdr[dwPages]
//	IN dwPages - 需要释放的页数
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	与Page_Get对应，释放一些页内存
//引用：
// ********************************************************************
BOOL Page_Release( LPVOID * lppAdr, DWORD dwPages )
{
	while( dwPages-- )
	{
		//ASSERT( *lppAdr );
		Page_Free( *lppAdr );
		lppAdr++;
	}
	return TRUE;
}

// ********************************************************************
//声明：LPVOID Page_GetContiguous( DWORD dwPages, DWORD dwAlignmentMask )
//参数：
//	IN dwPages - 需要分配的页数
//	IN dwAlignmentMask - 页开始地址对齐mask
//返回值：
//	假如成功，返回非NULL的内存地址；否则返回NULL
//功能描述：
//	得到连续的页内存
//引用：
// ********************************************************************
LPVOID Page_GetContiguous( DWORD dwPages, DWORD dwAlignmentMask )
{
	LPVOID lpvRet = NULL;
	// 锁住需要的页
	if( Page_Lock( dwPages ) )
	{	// 
		LPSEGMENTINFO lpSegInfo = memInfo.lpSectionInfo;
		LPSEGMENTINFO lpSegInfoEnd = lpSegInfo + memInfo.dwSections;
        // 遍历所有的内存段，得到一块连续的内存
		for( ; lpSegInfo < lpSegInfoEnd; lpSegInfo++ )
		{
			LPBYTE lpbMemMapS = lpSegInfo->lpbFreeMemMap;
			LPBYTE lpbMemMapE = lpbMemMapS + lpSegInfo->dwTotalPages - dwPages;
			LPBYTE lpbPageStart = lpSegInfo->lpbFreePageStart;
			DWORD dwIndexBegin = 0;
			// 查找当前内存段
			for( ; lpbMemMapS <= lpbMemMapE; lpbMemMapS++, dwIndexBegin++, lpbPageStart += PAGE_SIZE )
			{	// 该页是否是空闲的或是否对齐地址
				if( *lpbMemMapS ||
					( (DWORD)lpbPageStart & dwAlignmentMask )
					)
				{	// 占用或没有对齐
					continue;
				}
				else
				{	// 符合条件，从该页向下遍历其它需要的内存页
					LPBYTE lpbFirst = lpbMemMapS;
					LPBYTE lpbEnd = lpbFirst + dwPages;
					DWORD dwCurIndex = dwIndexBegin;
					for( ; lpbFirst < lpbEnd; lpbFirst++, dwCurIndex++ )
					{	// 是否是空闲 或 分配成功？
						if( *lpbFirst ||
							Page_GetSpecific( lpSegInfo, dwCurIndex ) == FALSE )
						{   // 不成功，释放之前已分配的内存页 							
							DWORD n = dwIndexBegin;
							for( ; n < dwCurIndex; n++ )
							{
								Page_ReleaseSpecific( lpSegInfo, n );
							}
							// 重新设定遍历开始位置
							lpbMemMapS = lpbFirst + 1;
							dwIndexBegin = dwCurIndex + 1;
							lpbPageStart = lpSegInfo->lpbFreePageStart + dwIndexBegin * PAGE_SIZE;
							break;
						}
					}
					if( lpbFirst == lpbEnd )
					{   // 已经得到所有需要的内存页 //i get all page needed
						lpvRet = lpbPageStart;
						goto _RET;
					}
				}
			}
			// 不成功，解锁
			Page_Unlock( dwPages );
		}
	}
_RET:
	//	如果有CACHE功能，将其转化为CACHE指针

	return lpvRet ? (LPVOID)UNCACHE_TO_CACHE(lpvRet) : NULL;
}

// ********************************************************************
//声明：BOOL Page_ReleaseContiguous( LPVOID lpAdr, DWORD dwPages )
//参数：
//	IN lpAdr - 需要释放的页开始地址
//	IN dwPages - 需要释放的页数
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	与Page_GetContiguous对应，释放连续的页内存
//引用：
// ********************************************************************
BOOL Page_ReleaseContiguous( LPVOID lpAdr, DWORD dwPages )
{
	for( ; dwPages; dwPages-- )
	{
		//ASSERT( lpAdr );
		Page_Free( lpAdr );
		//	下一页地址
		lpAdr = (LPVOID)( (DWORD)lpAdr + PAGE_SIZE ); 
	}
	return TRUE;
}

// ********************************************************************
//声明：BOOL Page_InitSysPages( void )
//参数：
//	无
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	当内核启动时，初始化系统内存页，在调用该函数之前lpbSysMainMem，ulSysMainMemLength必须先初始化
//引用：
// ********************************************************************
BOOL Page_InitSysPages( void )
{
	extern void OEM_WriteDebugString( LPCTSTR lpszStr );

    int k, iSegNum;	
	MEM_SEGMENT seg[MAX_SEGMENTS];
	char buf[16];

	// 输出调试信息 - begin
	OEM_WriteDebugString( "Page_InitSysPages:lpbSysMainMem=0x" );
	itoa( (DWORD)lpbSysMainMem, buf, 16 );
	OEM_WriteDebugString( buf );
	OEM_WriteDebugString( ",ulSysMainMemLength=" );
	itoa( (DWORD)ulSysMainMemLength, buf, 10 );
	OEM_WriteDebugString( buf );
	OEM_WriteDebugString( "\r\n" );
	// - end
	
	//	如果带MMU,先刷新CACHE
#ifdef VIRTUAL_MEM
	FlushCacheAndClearTLB();
#endif
	// 初始化句柄基本地址
    dwHandleBase = (DWORD)lpbSysMainMem & 0xE0000000;
	lpbSysMainMemEnd = lpbSysMainMem + ulSysMainMemLength;
	//初始化memInfo 信息
	memInfo.lpbMainMemStart = (LPBYTE)CACHE_TO_UNCACHE( lpbSysMainMem );
	memInfo.lpbMainMemEnd = memInfo.lpbMainMemStart + ulSysMainMemLength;
	memInfo.lpbMainMemFreeStart = memInfo.lpbMainMemStart;
	memInfo.dwMainMemFreeSize = ulSysMainMemLength;
	//建立第一个内存段结构（用主内存）
	memInfo.lpSectionInfo = (LPSEGMENTINFO)ALIGN_DWORD( (DWORD)memInfo.lpbMainMemFreeStart );
	//初始化第一个内存段
	seg[0].lpbStart = (LPBYTE)( memInfo.lpSectionInfo + MAX_SEGMENTS );
	seg[0].dwSize = memInfo.dwMainMemFreeSize - ( seg[0].lpbStart - memInfo.lpbMainMemFreeStart );
	//得到系统其它附加内存段数
	iSegNum = OEM_EnumExtensionDRAM( seg+1, MAX_SEGMENTS-1 );
	iSegNum += 1;  // 总的内存段数
	//初始化每一个内存段
	for( k = 0; k < iSegNum; k++ )
	{
		LPBYTE lpbPageStart, lpbPageEnd;
		DWORD dwPages;

		memInfo.lpSectionInfo[k].lpbMemStart = seg[0].lpbStart;
		memInfo.lpSectionInfo[k].dwMemSize = seg[0].dwSize;
		memInfo.lpSectionInfo[k].lpbFreeMemMap = memInfo.lpSectionInfo[k].lpbMemStart;//
        // 该段页开始位置
		lpbPageStart = (LPBYTE)ALIGN_PAGE_UP( (DWORD)memInfo.lpSectionInfo[k].lpbMemStart );
		// 该段页结束位置
		lpbPageEnd = (LPBYTE)ALIGN_PAGE_DOWN( (DWORD)(memInfo.lpSectionInfo[k].lpbMemStart + memInfo.lpSectionInfo[k].dwMemSize) );//.lpbMemStart + 
		// 该段页数
		dwPages = (lpbPageEnd - lpbPageStart) / PAGE_SIZE;
		// 该段空闲页开始位置
		memInfo.lpSectionInfo[k].lpbFreePageStart =  (LPBYTE)ALIGN_PAGE_UP( (DWORD)(memInfo.lpSectionInfo[k].lpbFreeMemMap + dwPages) );
        // 该段空闲页结束位置
		memInfo.lpSectionInfo[k].lpbFreePageEnd = lpbPageEnd; 
		// 该段总的可用页数
	    memInfo.lpSectionInfo[k].dwTotalPages = (memInfo.lpSectionInfo[k].lpbFreePageEnd - memInfo.lpSectionInfo[k].lpbFreePageStart) / PAGE_SIZE;
		// 将页属性设为空闲状态
		memset( memInfo.lpSectionInfo[k].lpbFreeMemMap, 0, memInfo.lpSectionInfo[k].dwTotalPages );
	}

	memInfo.dwSections = iSegNum;

    // 将所有内存段的空闲页加入空闲链表。now , add free pages to pagelist	
	for( k = 0; k < iSegNum ; k++ )
	{
		LPBYTE lpbPageStart = memInfo.lpSectionInfo[k].lpbFreePageStart;
		LPBYTE lpbPageEnd = memInfo.lpSectionInfo[k].lpbFreePageEnd;
		while( lpbPageStart < lpbPageEnd )
		{
		    Page_AddToFreeList( (LPPAGELIST)lpbPageStart );
		    lpbPageStart += PAGE_SIZE;
		}
	}
	//
	return TRUE;
}

// ********************************************************************
//声明：BOOL _InitSysMem( void )
//参数：
//	无
//返回值：
//	假如成功，返回TRUE；否则返回FALSE
//功能描述：
//	当内核启动时，初始化系统内存页，在调用该函数之前lpbSysMainMem，ulSysMainMemLength必须先初始化
//引用：
// ********************************************************************
BOOL _InitSysMem( void )
{
    return Page_InitSysPages();
}
