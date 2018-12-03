/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：块堆管理
版本号：1.0.0
开发时期：2003-09-03
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

typedef struct BLOCK_HEAP
{
	int nBlockCount;
    int iRemainderIndex;
    LPBYTE lpFree;
    int uiFreeSize;
    int uiMaxBlockSize;
	HANDLE hThis;
	HANDLE hProcessHeap;
//	LPUINT lpAllocList;
	CRITICAL_SECTION cs; 

	BLOCK_DATA block[1];
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
#define ALIGN_SIZE( uiSize ) ( ( (uiSize) + 3) & (~3) )


static LPBLOCK_DATA BlockHeap_GetPtr( LPBLOCK_HEAP lpbh, UINT uiSize )
{
	//BLOCKHEAP * lpHeap = &heap[0];
    int i;
	int count = lpbh->nBlockCount;
	LPBLOCK_DATA lpbd = lpbh->block;

	for( i = 0; i < count; i++, lpbd++ )
	{
		if( lpbd->wBlockSize >= uiSize )
			return lpbd;
	}
	RETAILMSG( 1, ( "error heap size:uiSize=%d.\r\n", uiSize ) );
	return NULL;
}

static void FreeRemainder( LPBLOCK_HEAP lpbh, LPBYTE lpRemainder, UINT iRemainderSize )
{
	int i = lpbh->iRemainderIndex;
	LPBLOCK_DATA lpbd = &lpbh->block[i];
	UINT uiMinBlockSize = lpbd->wBlockSize;

//	UINT uiSave;

	while( iRemainderSize >= uiMinBlockSize )
	{
		ASSERT( i < lpbh->nBlockCount );
		if( lpbd->wBlockSize <= iRemainderSize )
		{
			INSERT_FREE_BLOCK( lpbd, lpRemainder );
			//KL_InterlockedIncrement( &heap[i].iMaxBlocks );//2003-06-04, DEL

			lpRemainder += lpbd->wBlockSize;
            iRemainderSize -= lpbd->wBlockSize; 
		}
		if( ++i == lpbh->nBlockCount )
		{
			i = 0;
			lpbd = lpbh->block;
		}
		else
			lpbd++;
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

	if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
	{
		EnterCriticalSection( &lpbh->cs );
	}

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
			if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
			{
				LeaveCriticalSection( &lpbh->cs );
			}
#ifdef LINUX_KERNEL
			lpRet = (LPBYTE)malloc( ALLOC_PAGE_SIZE );
#else
			lpRet = (LPBYTE)HeapAlloc( lpbh->hProcessHeap, 0, ALLOC_PAGE_SIZE );
#endif

			if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
			{
				EnterCriticalSection( &lpbh->cs );
			}
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

	if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
	{
		LeaveCriticalSection( &lpbh->cs );
	}
	
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
    UINT i;
    UINT bhSize;
	LPBLOCK_HEAP lpbh;
	LPUINT lpui;
	UINT nRealCount;
	UINT uiSize[32];
	HANDLE hProcessHeap = NULL;
    
	if( nCount == 0 || nCount > 32 )
		return NULL;
	for( lpui = lpuiSize, i = 0; i < nCount; i++, lpui++ )
	{
		if( *lpui > ALLOC_PAGE_SIZE )
			return NULL;
	}
	// 将lpuiSize的值由小到大排例
	nRealCount = SortAndCompressSize( uiSize, lpuiSize, nCount );

	bhSize = ( nRealCount - 1 ) * sizeof( BLOCK_DATA ) + sizeof( BLOCK_HEAP );
#ifdef LINUX_KERNEL
	lpbh = malloc( bhSize );
	memset( lpbh, 0, bhSize );
#else
	lpbh = HeapAlloc( hProcessHeap = GetProcessHeap(), HEAP_ZERO_MEMORY, bhSize );
#endif
	if( lpbh )
	{
		lpbh->hProcessHeap = hProcessHeap;
		lpbh->nBlockCount = nRealCount;
		lpbh->hThis = (HANDLE)PTR_TO_HANDLE( lpbh );
		lpui = uiSize;
		//DEBUGMSG( 1,("CrearteBlock,lpbh=0x%x,hThis=0x%x,h2p=0x%x.\r\n",lpbh,lpbh->hThis,HANDLE_TO_PTR(lpbh->hThis)));

		for( i = 0; i < nRealCount; i++, lpui++ )
		{
			lpbh->block[i].wBlockSize = *lpui; 
		}
		lpbh->uiMaxBlockSize = uiSize[nRealCount-1];
		InitializeCriticalSection( &lpbh->cs );
	    return lpbh->hThis;	
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
			LPBLOCK_DATA lpbd = lpbh->block;
		    for( i = 0; i < lpbh->nBlockCount; i++, lpbd++ )
			{
				WARNMSG( lpbd->nCount, ( "error in BlockHeap_Destroy: not all block(%d) is freed, block size(%d).\r\n", lpbd->nCount, lpbd->wBlockSize ) );
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
#ifdef LINUX_KERNEL
		free( lpbh );
#else
		HeapFree( hProcessHeap, 0, lpbh );
#endif
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
//	DEBUGMSG(1, ("DoAlloc,handle=0x%x,ptr=0x%x.\r\n",handle,lpbh));

	if( lpbh && lpbh->hThis == handle )
	{
		LPBLOCK_DATA lpbd;
		
		lpbd = BlockHeap_GetPtr( lpbh, uiSize );

		ASSERT( lpbd );
		//INTR_OFF();
		if( lpbd )
		{
			if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
			{
				EnterCriticalSection( &lpbh->cs );
			}

			REMOVE_FREE_BLOCK( lpbd, lpRet );

			if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
			{
				LeaveCriticalSection( &lpbh->cs );
			}
			
			if( lpRet == NULL )
			{
				if( (lpRet = BlockHeap_Increase( lpbh, uiFlags, lpbd->wBlockSize )) )
				{
					;//KL_InterlockedIncrement( &lpHeap->iMaxBlocks );
				}
				else
					return NULL;
			}
			//KL_InterlockedIncrement( &lpHeap->iUsed );//2003-06-04, DEL
			
#ifdef __DEBUG
			memset( lpRet, 0xCCCCCCCC, uiSize );
            InterlockedIncrement( &lpbd->nCount );
#endif
			
		}
		
		
		//INTR_ON();
	}

	return lpRet;
	
}

LPVOID WINAPI BlockHeap_Alloc( HANDLE handle, UINT uiFlags, UINT uiSize )
{
	LPBYTE lpui;

//	DEBUGMSG( 1, ("Alloc, Handle=0x%x.\r\n",handle));
	if( uiFlags & BLOCKHEAP_RESIZABLE )
	    uiSize += 4;
	lpui = DoBlockHeapAlloc( handle, uiFlags, uiSize );
//	DEBUGMSG( 1,("Alloc001.\r\n"));
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
	
	if( lpbh && lpbh->hThis == handle && lpvBlock )
	{		
		LPBLOCK_DATA lpbd = BlockHeap_GetPtr( lpbh, uiSize );
		ASSERT( lpbd );
		if( lpbd )
		{
			if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
			{
				EnterCriticalSection( &lpbh->cs );
			}
			
			INSERT_FREE_BLOCK( lpbd, lpvBlock );
			
			
			if( !(uiFlags & BLOCKHEAP_NO_SERIALIZE) )
			{
				LeaveCriticalSection( &lpbh->cs );
			}

#ifdef __DEBUG
			// test code
			if( lpbd->nCount <= 0 )
			{
				lpbd->nCount = lpbd->nCount;
			}
			//
			ASSERT( lpbd->nCount > 0 );
		    InterlockedDecrement( &lpbd->nCount );
#endif
			return TRUE;
		}
	}
	return FALSE;
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

void TestBlockHeap( void )
{
	int bContinue = 1;
	int nsocket;
//	int index;
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
