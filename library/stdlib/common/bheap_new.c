/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：块堆管理,用在提供服务的模块里的动态分配，
		  释放时，将块按大小放到块管理容器，分配时，首先从
          块管理容器分配，如果没有分配到，则从系统分配
版本号：1.0.0
开发时期：2004-07-05
作者：李林
修改记录：
******************************************************/

#include <ewindows.h>
#include <bheap.h>

#include <eobjlist.h>

typedef struct _BLOCK_DATA
{
	USHORT wBlockSize;
	USHORT wReserve;//
#ifdef __DEBUG
	LONG   nCount;
#endif
	LPBYTE lpFreeBlock;
}BLOCK_DATA, FAR * LPBLOCK_DATA;

#define DEF_INC_BLOCKS 8
#define BHF_VARIABLE          0x1
typedef struct BLOCK_HEAP
{
	BYTE nTotalBlockCount;	//总的块对象 = nAllocBlockCount + nFreeBlockCount
	BYTE nAllocBlockCount;	//已使用块对象
	BYTE nFreeBlockCount;	//空闲块对象
    BYTE iRemainderIndex;
    LPBYTE lpFree;
    int uiFreeSize;
    UINT uiMaxBlockSize;
	HANDLE hThis;
	HANDLE hProcessHeap;
	UINT uOption;	//
//	LPUINT lpAllocList;
	CRITICAL_SECTION cs; 

	//BLOCK_DATA block[1];
	LPBLOCK_DATA lpBlock;
}BLOCK_HEAP, FAR * LPBLOCK_HEAP;

//#define MAX_BLOCK_POOLS ( sizeof( heap ) / sizeof( BLOCKHEAP ) ) 


#define INSERT_FREE_BLOCK( lpHeap, lpvBlock ) \
	do{\
	    *( (LPBYTE*)(lpvBlock) ) = (lpHeap)->lpFreeBlock;\
		(lpHeap)->lpFreeBlock = (LPBYTE)(lpvBlock);\
	}while(0)

#define REMOVE_FREE_BLOCK( lpHeap, lpRet ) \
	do{\
	    if( ((lpRet) = (lpHeap)->lpFreeBlock ) != NULL ) \
		    (lpHeap)->lpFreeBlock = *( (LPBYTE*)(lpHeap)->lpFreeBlock );\
	}while(0)

 //align to 4 bytes;
#define ALIGN_SIZE( uiSize ) ( ( (uiSize) + 7) & (~7) )

static LPBLOCK_DATA BlockHeap_AllocBlockObject( LPBLOCK_HEAP lpbh, UINT uiSize )
{
	//LPBLOCK_DATA = HeapAlloc( lpbh->hProcessHeap, HEAP_ZERO_MEMORY, sizeof( BLOCK_HEAP ) );
_again:
	if( lpbh->nFreeBlockCount )
	{
		LPBLOCK_DATA lpblkEnd, lpCur = lpbh->lpBlock;
		int count = lpbh->nAllocBlockCount;
		int cur;
		//得到快对应的位置（按块大小排序）
		for( cur = 0; cur < count; cur++, lpCur++ )
		{
			if( uiSize < lpCur->wBlockSize )
				break;
		}
		//移动数据
		lpblkEnd = lpCur + count;
		for( ;cur < count; count--, lpblkEnd-- )
		{
			*lpblkEnd = *(lpblkEnd - 1);
		}
		memset( lpCur, 0, sizeof( BLOCK_DATA ) );
		lpCur->wBlockSize = uiSize;
		lpbh->nAllocBlockCount++;
		lpbh->nFreeBlockCount--;
		lpbh->uiMaxBlockSize = MAX( uiSize, lpbh->uiMaxBlockSize );

		return lpCur;
	}
	else
	{	//没有足够的空闲位置，需要重新分配
		LPBLOCK_DATA lpBlock = HeapReAlloc( lpbh->hProcessHeap, 0, lpbh->lpBlock, (lpbh->nTotalBlockCount + DEF_INC_BLOCKS) * sizeof(BLOCK_DATA) );
		if( lpBlock )
		{	//分配成功
			lpbh->lpBlock = lpBlock;
			lpbh->nTotalBlockCount += DEF_INC_BLOCKS;
			lpbh->nFreeBlockCount = DEF_INC_BLOCKS;
			goto _again;
		}
	}
	return NULL;
}

static LPBLOCK_DATA BlockHeap_GetPtr( LPBLOCK_HEAP lpbh, UINT uiSize )
{
	//BLOCKHEAP * lpHeap = &heap[0];
    int i;
	int count = lpbh->nAllocBlockCount;
	LPBLOCK_DATA lpblk = lpbh->lpBlock;

	for( i = 0; i < count; i++, lpblk++ )
	{
		if( lpblk->wBlockSize >= uiSize )
		{
			if( lpbh->uOption & BHF_VARIABLE )
			{	//可变
				if( lpblk->wBlockSize < uiSize + 8 )
					return lpblk;
				//没有符合要求的，需要重新增加一个块描述对象
				lpblk = BlockHeap_AllocBlockObject( lpbh, uiSize );
				return lpblk;
			}
			else
				return lpblk;	//不可变，符合条件
		}
	}
	//没有找到合适的
	if( lpbh->uOption & BHF_VARIABLE )
	{  //可变块
		return BlockHeap_AllocBlockObject( lpbh, uiSize );
	}

	RETAILMSG( 1, ( "error heap size:uiSize=%d.\r\n", uiSize ) );
	return NULL;
}

static void FreeRemainder( LPBLOCK_HEAP lpbh, LPBYTE lpRemainder, UINT iRemainderSize )
{
	int i = lpbh->iRemainderIndex;
	LPBLOCK_DATA lpblk = lpbh->lpBlock;
	UINT uiMinBlockSize = lpblk->wBlockSize;

//	UINT uiSave;

	while( iRemainderSize >= uiMinBlockSize )
	{
		ASSERT( i < lpbh->nAllocBlockCount );
		if( lpblk->wBlockSize <= iRemainderSize )
		{
			INSERT_FREE_BLOCK( lpblk, lpRemainder );
			//KL_InterlockedIncrement( &heap[i].iMaxBlocks );//2003-06-04, DEL

			lpRemainder += lpblk->wBlockSize;
            iRemainderSize -= lpblk->wBlockSize; 
		}
		if( ++i == lpbh->nAllocBlockCount )
		{
			i = 0;
			lpblk = lpbh->lpBlock;
		}
		else
			lpblk++;
		//i = (i + 1) % MAX_BLOCK_POOLS;
	}

	//LockIRQSave( &uiSave );//2003-06-11, ADD
	
	lpbh->iRemainderIndex = i;
	//INTR_ON();
	//UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
}

#define ALLOC_PAGE_SIZE   4096

static LPBYTE BlockHeap_Increase( LPBLOCK_HEAP lpbh, UINT uiFlags, int size )
{
	LPBYTE lpRet = NULL;

	//if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
	//{
	//	EnterCriticalSection( &lpbh->cs );
	//}

	if( lpbh->uiFreeSize >= size )
	{
		lpRet = lpbh->lpFree;
		lpbh->lpFree += size;
		lpbh->uiFreeSize -= size;
	}
	else
	{  //
		if( size <= ALLOC_PAGE_SIZE )
		{
			//if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
			//{
			//	LeaveCriticalSection( &lpbh->cs );
			//}
			lpRet = (LPBYTE)HeapAlloc( lpbh->hProcessHeap, 0, ALLOC_PAGE_SIZE );

			//if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
			//{
			//	EnterCriticalSection( &lpbh->cs );
			//}
			if(  lpRet )
			{
				LPBYTE lpRemainder;
				int uiRemainderSize;
				UINT uiFreeBytes = ALLOC_PAGE_SIZE;// - 8;
				
				//INTR_OFF();
				//			LockIRQSave( &uiSave ); //2003-06-11, ADD
	//			ASSERT( size < ALLOC_PAGE_SIZE );
//				{  // must size <= ALLOC_PAGE_SIZE - 8
	//				*( (LPUINT*)lpRet ) = lpbh->lpAllocList;
	//				lpbh->lpAllocList = (LPUINT)lpRet;
	//				lpRet = (LPBYTE)lpRet + 8; 
	//			}
				if( lpbh->uiFreeSize < (int)(uiFreeBytes - size) )
				{
					lpRemainder = lpbh->lpFree;
					uiRemainderSize = lpbh->uiFreeSize;
					
					lpbh->lpFree = lpRet + size;
					lpbh->uiFreeSize = uiFreeBytes - size;
				}
				else
				{
					lpRemainder = lpRet + size;
					uiRemainderSize = uiFreeBytes - size;
				}
				//INTR_ON();
				//			UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
				
				FreeRemainder( lpbh, lpRemainder, uiRemainderSize );
			}
		}
		else
		{
			RETAILMSG( 1, ( "invalid blockheap size(%d).\r\n", size ) );
//			return NULL;//
		}
	}

	//if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
	//{
	//	LeaveCriticalSection( &lpbh->cs );
	//}
	
	return lpRet;
}

static UINT SortAndCompressSize( LPUINT lpuiDestSize, LPUINT lpuiSrcSize, UINT nCount )
{
	UINT nMin, nMinPos;
	UINT n;
	UINT uiValue;
	LPUINT lpui, lpuiStart; //
    UINT k;
	memcpy( lpuiDestSize, lpuiSrcSize, sizeof( UINT ) * nCount );
	//nStart = 0;

	k = nCount;
	lpuiStart = lpuiDestSize;
	while( k > 1 )
	{
		nMinPos = 0;
	    lpui = lpuiStart;

		nMin = *lpui++;
		n = 1;
		for( ; n < k; n++, lpui++ )
		{
			if( nMin > *lpui )
			{
				nMin = *lpui;
				nMinPos = n;
			}
		}
		uiValue = *lpuiStart;
		*lpuiStart = nMin;
		lpuiStart[nMinPos] = uiValue;
		k--;
		lpuiStart++;
	}
	//去掉相同的size值
	lpui = lpuiDestSize;
	nMin = *lpui;
	nMin = ALIGN_SIZE( nMin );
	*lpui++ = nMin;
	lpuiStart = lpui;
    for( k = 1; k < nCount; k++ )
	{
		UINT v = *lpuiStart;
        v = ALIGN_SIZE( v );
		ASSERT( nMin <= v );
		if( nMin != v )
		{
			*lpui++ = nMin = v;
		}
		lpuiStart++;
	}
	return lpui - lpuiDestSize;
}

HANDLE WINAPI BlockHeap_Create( LPUINT lpuiSize, UINT nCount )
{
	LPBLOCK_HEAP lpbh;
	UINT nRealCount;
	UINT uiSize[32];
	HANDLE hProcessHeap;
	LPUINT lpui;

	//UINT bhSize;
	if( lpuiSize && nCount )
	{	//不可变块对象
		UINT i;

		
		if( nCount == 0 || nCount > 32 )
			return NULL;
		for( lpui = lpuiSize, i = 0; i < nCount; i++, lpui++ )
		{
			if( *lpui > ALLOC_PAGE_SIZE )
				return NULL;
		}
		// 将lpuiSize的值由小到大排例
		nRealCount = SortAndCompressSize( uiSize, lpuiSize, nCount );
	}
	else
		nRealCount = DEF_INC_BLOCKS;
		
//	bhSize = nRealCount  * sizeof( BLOCK_DATA ) + sizeof( BLOCK_HEAP );
	lpbh = HeapAlloc( hProcessHeap = GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof( BLOCK_HEAP ) );
	if( lpbh )
	{			
		BLOCK_DATA * lpBlock = HeapAlloc( hProcessHeap, HEAP_ZERO_MEMORY, nRealCount * sizeof( BLOCK_DATA ) );
		if( lpBlock )
		{
			lpbh->lpBlock = lpBlock;
			lpbh->hProcessHeap = hProcessHeap;
			lpbh->nTotalBlockCount = nRealCount;
			lpbh->hThis = (HANDLE)PTR_TO_HANDLE( lpbh );
			if( lpuiSize && nCount )
			{	//不可变块对象
				UINT i;
				lpui = uiSize;				
				for( i = 0; i < nRealCount; i++, lpui++ )
				{
					lpbh->lpBlock[i].wBlockSize = *lpui; 
				}
				lpbh->uiMaxBlockSize = uiSize[nRealCount-1];
				lpbh->nAllocBlockCount = nRealCount;
			}
			else
			{
				lpbh->uOption = BHF_VARIABLE;
				lpbh->nFreeBlockCount = nRealCount;
			}

			InitializeCriticalSection( &lpbh->cs );
			return lpbh->hThis;	
		}
		else
		{
			HeapFree( hProcessHeap, 0, lpbh );
		}
	}
	return NULL;
}

BOOL WINAPI BlockHeap_Destroy( HANDLE handle )
{
	LPBLOCK_HEAP lpbh = (LPBLOCK_HEAP)HANDLE_TO_PTR( handle );
	BOOL bRetv = FALSE;

	if( lpbh && lpbh->hThis == handle )
	{
//		LPUINT lpList; 
		HANDLE hProcessHeap;

		EnterCriticalSection( &lpbh->cs );

#ifdef __DEBUG
		// check
		{
			int i;
			LPBLOCK_DATA lpblk = lpbh->lpBlock;
		    for( i = 0; i < lpbh->nAllocBlockCount; i++, lpblk++ )
			{
				WARNMSG( lpblk->nCount, ( "error in BlockHeap_Destroy: not all block(%d) is freed, block size(%d).\r\n", lpblk->nCount, lpblk->wBlockSize ) );
			}
		}
#endif
		hProcessHeap = lpbh->hProcessHeap;

	//	lpList = lpbh->lpAllocList; 
	//	while( lpList )
	//	{
	//		LPUINT lpNext = *(LPUINT*)lpList;
	//		HeapFree( hProcessHeap, 0, lpList );
	//		lpList = lpNext;
	//	}
		lpbh->hThis = NULL;
		HeapFree( hProcessHeap, 0, lpbh->lpBlock );
		HeapFree( hProcessHeap, 0, lpbh );
		//return TRUE;
		bRetv = TRUE;

	    LeaveCriticalSection( &lpbh->cs );
	    DeleteCriticalSection( &lpbh->cs );
	}

	return FALSE;
}

static LPVOID DoBlockHeapAlloc( HANDLE handle, UINT uiFlags, UINT uiSize )
{
	LPBLOCK_HEAP lpbh = (LPBLOCK_HEAP)HANDLE_TO_PTR( handle );
	void * lpRet = NULL;


    uiSize = ALIGN_SIZE( uiSize );
	if( lpbh && lpbh->hThis == handle )
	{
		LPBLOCK_DATA lpblk;

		if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
		{
			EnterCriticalSection( &lpbh->cs );
		}

		
		lpblk = BlockHeap_GetPtr( lpbh, uiSize );

		ASSERT( lpblk );
		//INTR_OFF();
		if( lpblk )
		{

			REMOVE_FREE_BLOCK( lpblk, lpRet );

			if( lpRet == NULL )
			{
				if( (lpRet = BlockHeap_Increase( lpbh, uiFlags, lpblk->wBlockSize )) )
				{
					;//KL_InterlockedIncrement( &lpHeap->iMaxBlocks );
				}
				else
				{	//失败
					goto _ret;//return NULL;
				}
			}
			//KL_InterlockedIncrement( &lpHeap->iUsed );//2003-06-04, DEL
			
			if( uiFlags & BLOCKHEAP_ZERO_MEMORY )
				memset( lpRet, 0, uiSize );
			else
			{

#ifdef __DEBUG
				memset( lpRet, 0xCCCCCCCC, uiSize );
#endif
			}
#ifdef __DEBUG
			lpblk->nCount++;
#endif
			
		}
_ret:		
		if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
		{
			LeaveCriticalSection( &lpbh->cs );
		}
		
		//INTR_ON();
	}

	return lpRet;
	
}

LPVOID WINAPI BlockHeap_Alloc( HANDLE handle, UINT uiFlags, UINT uiSize )
{
	LPBYTE lpui;

	if( uiFlags & BLOCKHEAP_RESIZABLE )
	    uiSize += 4;
	lpui = DoBlockHeapAlloc( handle, uiFlags, uiSize );
	if( lpui )
	{
		if( uiFlags & BLOCKHEAP_RESIZABLE )
		{
		    *( (LPUINT)lpui ) = uiSize;
		    lpui += 4;
		}
	}
	else
	{
		RETAILMSG( 1, ( "error in BlockHeap_Alloc: not alloc a block, handle(0x%x),uiFlags(0x%x),uiSize(%d)", handle,uiFlags,uiSize ) );
	}
	return lpui;
}

static BOOL DoBlockHeapFree( HANDLE handle, UINT uiFlags, LPVOID lpvBlock, UINT uiSize )
{
	LPBLOCK_HEAP lpbh = (LPBLOCK_HEAP)HANDLE_TO_PTR( handle );
	BOOL bRetv = FALSE;
	
    uiSize = ALIGN_SIZE( uiSize );
	if( lpbh && lpbh->hThis == handle && lpvBlock )
	{	
		LPBLOCK_DATA lpblk;
		if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
		{
			EnterCriticalSection( &lpbh->cs );
		}

		lpblk = BlockHeap_GetPtr( lpbh, uiSize );
		ASSERT( lpblk );
		if( lpblk )
		{
			
			INSERT_FREE_BLOCK( lpblk, lpvBlock );
			
			
#ifdef __DEBUG
			// test code
			if( lpblk->nCount <= 0 )
			{
				lpblk->nCount = lpblk->nCount;
			}
			//
			ASSERT( lpblk->nCount > 0 );
		    lpblk->nCount--;
#endif
			bRetv = TRUE;
			//return TRUE;
		}
		if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
		{
			LeaveCriticalSection( &lpbh->cs );
		}
	}
	return bRetv;
}

BOOL WINAPI BlockHeap_Free( HANDLE handle, UINT uiFlags, LPVOID lpvBlock, UINT uiSize  )
{
	if( lpvBlock )
	{
		if( uiFlags & BLOCKHEAP_RESIZABLE )
		{	        
		    lpvBlock = (LPBYTE)lpvBlock - 4;
		    uiSize = *((LPUINT)lpvBlock); 
		}
	    return DoBlockHeapFree( handle, uiFlags, lpvBlock, uiSize );
	}
	else
	{
		WARNMSG( 1, ( "error in BlockHeap_Free: null ptr.\r\n" ) );
		return FALSE;
	}
}

static HANDLE hDefBlockHeap = NULL;
//BOOL WINAPI BLK_Create( VOID )
//{
//	if( hDefBlockHeap == NULL )
//	    hDefBlockHeap = BlockHeap_Create( NULL, NULL );
//	return (hDefBlockHeap != NULL);
//}

//初始化/创建块堆
BOOL WINAPI BLK_Create( VOID )
{
	if( hDefBlockHeap == NULL )
		hDefBlockHeap = BlockHeap_Create( NULL, NULL );
	return hDefBlockHeap != 0;
}

//破坏/释放块堆
BOOL WINAPI BLK_Destroy( VOID )
{
	if( hDefBlockHeap )
	{
		BlockHeap_Destroy( hDefBlockHeap );
		hDefBlockHeap = NULL;
	}
	return TRUE;
}

//从块堆分配一个块
LPVOID WINAPI BLK_Alloc( UINT uiFlags, UINT uiSize )
{
	//if( hDefBlockHeap == NULL )
//		hDefBlockHeap = BlockHeap_Create( NULL, NULL );

	if( hDefBlockHeap )
		return BlockHeap_Alloc( hDefBlockHeap, uiFlags, uiSize );
	else
		return NULL;
}

//释放一个块
BOOL WINAPI BLK_Free( UINT uiFlags, LPVOID lpvBlock, UINT uiSize )
{
	if( hDefBlockHeap )
		return BlockHeap_Free( hDefBlockHeap, uiFlags, lpvBlock, uiSize );
	else
		return FALSE;
}


/*
LPVOID WINAPI BlockHeap_AllocString( HANDLE handle, UINT uiFlags, UINT uiSize )
{
    UINT * p;

	uiSize += sizeof( UINT );
	
	p = (UINT*)DoBlockHeapAlloc( handle, uiFlags, uiSize );
	if( p )
	{
		*p = uiSize;
		p++;
	}
	return p;
}

BOOL WINAPI BlockHeap_FreeString( HANDLE handle, UINT uiFlags, LPVOID lpv )
{
	if( lpv )
	{
		UINT * p = (UINT*)lpv - 1;
		if( *p )
		{
			return DoBlockHeapFree( handle, uiFlags, p, *p );
		}
	}
	return FALSE;
}
*/



// 这是一个测试函数
#ifdef TEST_BLOCKHEAP
void TestBlockHeap( void )
{
	int bContinue = 1;
	int nsocket;

	int nFreeCount = 0;
	int nAllocCount = 0;
	int i;


	LPBYTE lpv[20];
	UINT   id[20];
    HANDLE hHeap;
	UINT uiSize[] = { 64, 12, 5, 15, 18, 34, 78, 280, 320, 5, 400 }; 
	
	i = 0;
	
//    KHeap_InitBlockHeap();
	memset( id, 0xffffffff, sizeof( id ) );
	
	hHeap = BlockHeap_Create( uiSize, sizeof(uiSize) / sizeof( UINT ) );

	while( bContinue )
	{
		if( (rand() % 20) <= 9 )
		{   //alloc
			nsocket = rand() % 20;

			if( id[nsocket] == -1 )
			{
				int size = rand() % 500;

				lpv[nsocket] = BlockHeap_Alloc( hHeap, 0, size );
				
			    id[nsocket] = size;
			    if( lpv[nsocket] )
				    memset( lpv[nsocket], nsocket, size );
				nAllocCount++;				
			}
		}
		else
		{   // free
			if( rand() % 500 == 0 )
			{  // free all
				int n;
				for( n = 0; n < 20; n++ )
				{
					if( id[n] != -1 )
					{
						BlockHeap_Free( hHeap, 0, lpv[n], id[n] );
						id[n] = -1;
					}
				}
			}
			else
			{
				nsocket = rand() % 20;
				if( id[nsocket] != -1 )
				{
					BlockHeap_Free( hHeap, 0, lpv[nsocket], id[nsocket] );
					id[nsocket] = -1;
					nFreeCount++;
				}
			}
		}
		i++;
	}
	BlockHeap_Destroy( hHeap );

}
#endif
