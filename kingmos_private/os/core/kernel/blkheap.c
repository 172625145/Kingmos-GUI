/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�������Ŀ�ѹ���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��

******************************************************/

#include <ewindows.h>
#include <epcore.h>
#include <epalloc.h>
#include <coresrv.h>

#define BLOCK_EXCLUSIVE		0x1		//��һ��ҳ��ʣ��Ĳ���ȫ������ÿ�Ŀ��п���,�ù���Ҫ���Ĵ�СӦ����ҳ�ɱ���

typedef struct _BLOCKHEAP
{
	USHORT wBlockSize;
	USHORT wFlag;
	LPBYTE lpFreeBlock;
#ifdef __DEBUG
	int nTotalBlock;
	int iUsed;
	//int   iMaxBlocks;
#endif
}BLOCKHEAP;

//Ԥ����Ŀ����
static BLOCKHEAP heap[] = {
    { 8,   0, 0,  },
	{ 16,  0, 0,  },
	{ 32,  0, 0,  },
	{ 64,  0, 0,  },
	{ 128, 0, 0,  },
	{ 256, 0, 0,  },
	{ 512, 0, 0,  },
	{ 1024,BLOCK_EXCLUSIVE, 0,  },
	{ 2048,BLOCK_EXCLUSIVE, 0,  },
	{ 4096,BLOCK_EXCLUSIVE, 0,  }
};
#define MAX_BLOCK_POOLS ( sizeof( heap ) / sizeof( BLOCKHEAP ) )

//��ʣ�µ��ڴ�ָ����ѵĿ�ʼ����
static int iRemainderIndex;
//ʣ�µ��ڴ�
static LPBYTE lpFree;
//ʣ�µ��ڴ��С
static int  uiFreeSize;
//����������С�Ŀ�
static int uiMinBlockSize;

//��һ��������Ӧ�Ŀ�������б�
#define INSERT_FREE_BLOCK( lpHeap, lpvBlock ) \
	do{\
	    UINT uiSave; \
		LockIRQSave( &uiSave ); \
		*( (LPBYTE*)(lpvBlock) ) = (lpHeap)->lpFreeBlock;\
		(lpHeap)->lpFreeBlock = (LPBYTE)(lpvBlock);\
		UnlockIRQRestore( &uiSave ); \
	}while(0)

//�ӿ�������б��Ƴ�һ����
#define REMOVE_FREE_BLOCK( lpHeap, lpRet ) \
	do{\
	    UINT uiSave; \
		LockIRQSave( &uiSave ); \
		if( ((lpRet) = (lpHeap)->lpFreeBlock ) != NULL ) \
		    (lpHeap)->lpFreeBlock = *( (LPBYTE*)(lpHeap)->lpFreeBlock );\
		UnlockIRQRestore( &uiSave ); \
	}while(0)

/**************************************************
������BLOCKHEAP * GetHeapPtr( UINT uiSize )
������
	uiSize - ��Ҫ����Ŀ��ڴ��С  
����ֵ��
	����ɹ���������Ч�Ŀ�ѽṹָ�룻���򣬷���NULL
����������
	������Ҫ�Ŀ��С��ϵͳ�����õ�һ��ƥ��Ŀ�ѽṹָ�룬
	�ѱ�Ӹÿ�ѽṹ������Ҫ�Ŀ�
����: 
	
************************************************/
static BLOCKHEAP * GetHeapPtr( UINT uiSize )
{
	BLOCKHEAP * lpHeap = &heap[0];
    int i;
	for( i = 0; i < MAX_BLOCK_POOLS; i++, lpHeap++ )
	{
		if( lpHeap->wBlockSize >= uiSize )
			return lpHeap;
	}
	RETAILMSG( 1, ( "error heap size:uiSize=%d.\r\n", uiSize ) );
	return NULL;
}

/**************************************************
������void FreeRemainder( LPBYTE lpRemainder, int iRemainderSize )
������
	lpRemainder - ʣ����ڴ�ָ��
	iRemainderSize - ʣ����ڴ��С
����ֵ��
	��
����������
	��ʣ����ڴ�ָ���
����: 
	
************************************************/
/*
static void FreeRemainder( LPBYTE lpRemainder, int iRemainderSize )
{
	int i = iRemainderIndex;	
	UINT uiSave;

	while( iRemainderSize >= uiMinBlockSize )
	{
		ASSERT( i < MAX_BLOCK_POOLS );
		if( heap[i].wBlockSize <= iRemainderSize )
		{
			INSERT_FREE_BLOCK( &heap[i], lpRemainder );
			//KL_InterlockedIncrement( &heap[i].iMaxBlocks );//2003-06-04, DEL

			lpRemainder += heap[i].wBlockSize;
            iRemainderSize -= heap[i].wBlockSize; 
#ifdef __DEBUG
			heap[i].nTotalBlock++;
#endif
		}
		i = (i + 1) % MAX_BLOCK_POOLS;
	}
	LockIRQSave( &uiSave );//2003-06-11, ADD
	
	iRemainderIndex = i;
	UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
}
*/
/**************************************************
������static void FreeToHeap( BLOCKHEAP * lpHeap, LPBYTE lpRemainder, int iRemainderSize )
������
	lpHeap - ��ѽṹָ��
	lpRemainder - ʣ����ڴ�ָ��
	iRemainderSize - ʣ����ڴ��С
����ֵ��
	��
����������
	��ʣ����ڴ澡���ָܷָ���Ŀ��
����: 
	
************************************************/
static void FreeToHeap( BLOCKHEAP * lpHeap, LPBYTE lpRemainder, int iRemainderSize )
{
	if( lpHeap )
	{
		int blockSize = (int)lpHeap->wBlockSize;
		while( iRemainderSize >= blockSize )
		{
			{
				INSERT_FREE_BLOCK( lpHeap, lpRemainder );
				//KL_InterlockedIncrement( &heap[i].iMaxBlocks );//2003-06-04, DEL
				
				lpRemainder += blockSize;
				iRemainderSize -= blockSize; 
#ifdef __DEBUG
				lpHeap->nTotalBlock++;
#endif
			}
		}
	}
	//�����ಿ�ּ���������
	if( iRemainderSize )
	{
		int i = iRemainderIndex;	
		UINT uiSave;
		
		while( iRemainderSize >= uiMinBlockSize )
		{
			ASSERT( i < MAX_BLOCK_POOLS );
			if( heap[i].wBlockSize <= iRemainderSize &&
				(heap[i].wFlag & BLOCK_EXCLUSIVE) == 0 )
			{
				INSERT_FREE_BLOCK( &heap[i], lpRemainder );
				//KL_InterlockedIncrement( &heap[i].iMaxBlocks );//2003-06-04, DEL
				
				lpRemainder += heap[i].wBlockSize;
				iRemainderSize -= heap[i].wBlockSize; 
#ifdef __DEBUG
				heap[i].nTotalBlock++;
#endif 
			}
			i = (i + 1) % MAX_BLOCK_POOLS;
		}
		LockIRQSave( &uiSave );//2003-06-11, ADD
		
		iRemainderIndex = i;
		UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
	}
}


static LPBYTE IncreaseHeap( BLOCKHEAP * lpHeap )
{
	LPBYTE lpRet = NULL;
	UINT uiSave;
	int  size = lpHeap->wBlockSize;

	//INTR_OFF();
	LockIRQSave( &uiSave );//2003-06-11, ADD
	if( uiFreeSize >= size && 
		(lpHeap->wFlag & BLOCK_EXCLUSIVE) == 0 )
	{
		lpRet = lpFree;
		lpFree += size;
		uiFreeSize -= size;
		UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
	}
	else
	{  //
		UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
		
		if( (lpRet = (LPBYTE)Page_Alloc( TRUE )) )// (UINT)&heap[0] )) )
		{
			LPBYTE lpRemainder;
			int uiRemainderSize;

			if( lpHeap->wFlag & BLOCK_EXCLUSIVE )
			{
				FreeToHeap( lpHeap, lpRet + size, PAGE_SIZE - size );
			}
			else
			{
				LockIRQSave( &uiSave ); //2003-06-11, ADD
				if( uiFreeSize < PAGE_SIZE - size )
				{
					lpRemainder = lpFree;
					uiRemainderSize = uiFreeSize;
					
					lpFree = lpRet + size;
					uiFreeSize = PAGE_SIZE - size;
				}
				else
				{
					lpRemainder = lpRet + size;
					uiRemainderSize = PAGE_SIZE - size;
				}
				UnlockIRQRestore( &uiSave ); //2003-06-11, ADD
				
				FreeToHeap( NULL, lpRemainder, uiRemainderSize );
			}
		}		
	}
	
	return lpRet;
}

BOOL KHeap_Init( void )
{
    int i;

    uiMinBlockSize = heap[0].wBlockSize;
#ifdef __DEBUG
		heap[0].nTotalBlock = 0;
#endif

	for( i = 1; i < MAX_BLOCK_POOLS; i++ )
	{
		uiMinBlockSize = min( heap[i].wBlockSize, uiMinBlockSize );
#ifdef __DEBUG
		heap[i].nTotalBlock = 0;
		heap[i].iUsed = 0;
#endif
         
	}
	uiFreeSize = 0;
	lpFree = NULL;
	iRemainderIndex = 0;
	return TRUE;

}

#ifdef __CHECK_MEM
int startCheckMem = 1;
int startCheckMemSize = 1;
UINT nCheckMemMinSize = 32;
UINT nCheckMemMaxSize = 64;
#endif

#ifdef __CHECK_MEM
LPVOID _KHeap_Alloc( UINT uiSize, const char * lpszFile, int line )
#else
LPVOID KHeap_Alloc( UINT uiSize )
#endif
{
	void * lpRet;
	BLOCKHEAP * lpHeap;

#ifdef __CHECK_MEM
	if( startCheckMem )
	{
		if( startCheckMemSize == 0 || 
			( uiSize > nCheckMemMinSize &&
			  uiSize <= nCheckMemMaxSize) )
			RETAILMSG( 1, ("_KHeap_Alloc:uiSize=%d,f=%s,l=%d,Proc=%s.\r\n", uiSize, lpszFile, line, lpCurThread->lpCurProcess->lpszApplicationName ) );
	}
#endif

    lpHeap = GetHeapPtr( uiSize );
//	ASSERT( lpHeap );
	//INTR_OFF();
	REMOVE_FREE_BLOCK( lpHeap, lpRet ); 

	if( lpRet == NULL )
	{
		if( (lpRet = IncreaseHeap( lpHeap )) )
		{
#ifdef __DEBUG
			lpHeap->nTotalBlock++;
#endif

			;//KL_InterlockedIncrement( &lpHeap->iMaxBlocks );
		}
		else
		{
			RETAILMSG( 1, ( "error in KHeap_Alloc: no enough memory!\r\n" ) );
			return NULL;
		}
	}
	//KL_InterlockedIncrement( &lpHeap->iUsed );//2003-06-04, DEL

#ifdef __DEBUG
	memset( lpRet, 0xCCCCCCCC, uiSize );
	lpHeap->iUsed++;
#endif


	//INTR_ON();
	// TEST
#ifdef VIRTUAL_MEM
    if( lpRet != NULL && ( (DWORD)lpRet & 0x8C000000) != 0x8C000000 )
	{
		RETAILMSG( 1, ("error in KHeap_Free: invalid ptr(0x%x)", lpRet ) );
	}
#endif
	//
	return lpRet;
}

#ifdef __CHECK_MEM
BOOL _KHeap_Free( LPVOID lpvBlock, UINT uiSize, const char * lpszFile, int line )
#else
BOOL KHeap_Free( LPVOID lpvBlock, UINT uiSize )
#endif
{
	//ASSERT( id == *( (LPBYTE)lpvBlock ) );
	BLOCKHEAP * lpHeap;

#ifdef __CHECK_MEM
	if( startCheckMem )
	{
		if( startCheckMemSize == 0 || 
			( uiSize > nCheckMemMinSize &&
			  uiSize <= nCheckMemMaxSize) )
					RETAILMSG( 1, ("KHeap_Free:uiSize=%d,f=%s,l=%d,Proc=%s.\r\n", uiSize, lpszFile, line, lpCurThread->lpCurProcess->lpszApplicationName ) );
	}
#endif

#ifdef VIRTUAL_MEM
	if( ( (DWORD)lpvBlock & 0x8C000000 ) != 0x8C000000 )
	{
		RETAILMSG( 1, ( "error in KHeap_Free: invalid ptr(0x%x), size(%d).\r\n", lpvBlock, uiSize ) );
		return FALSE;
	}
#endif

    lpHeap = GetHeapPtr( uiSize );
//    ASSERT( lpHeap );

    INSERT_FREE_BLOCK( lpHeap, lpvBlock );

#ifdef __DEBUG
	lpHeap->iUsed--;
#endif

	//ASSERT( heap[id].lpFreeBlock != NULL );

	//KL_InterlockedDecrement( &lpHeap->iUsed );//2003-06-04, DEL

	//ASSERT( heap[id].iUsed >= 0 );
    return TRUE;
}

#ifdef __CHECK_MEM
LPVOID _KHeap_AllocString( UINT uiSize, const char * lpszFile, int line )
#else
LPVOID KHeap_AllocString( UINT uiSize )
#endif
{
	UINT nSize = ( uiSize + sizeof( UINT ) + 7 ) & (~7); // 2003-10-09, align to 8
	UINT * p;//( uiSize + sizeof( UINT ) );

#ifdef __CHECK_MEM
	if( startCheckMem )
	{
		if( startCheckMemSize == 0 || 
			( nSize > nCheckMemMinSize &&
			  nSize <= nCheckMemMaxSize) )
		RETAILMSG( 1, ("_KHeap_AllocString:uiSize=%d,f=%s,l=%d.\r\n", nSize, lpszFile, line ) );
	}
#endif
	p = (UINT*)KHeap_Alloc( nSize );


	if( p ) 
	{
		*p = nSize;//uiSize + sizeof( UINT );
		p++;
	}
	return p;
}

#ifdef __CHECK_MEM
BOOL  _KHeap_FreeString( LPVOID lpv, const char * lpszFile, int line )
#else
BOOL  KHeap_FreeString( LPVOID lpv )
#endif
{
	UINT * p = (UINT*)lpv - 1;

#ifdef __CHECK_MEM
	if( startCheckMem )
	{
		if( startCheckMemSize == 0 || 
			( *p > nCheckMemMinSize &&
			  *p <= nCheckMemMaxSize) )
		RETAILMSG( 1, ("_KHeap_FreeString:s=%d,f=%s,l=%d.\r\n", *p, lpszFile, line ) );
	}
#endif

	if( *p )
	{
		return KHeap_Free( p, *p );
	}
	return FALSE;
}

void KHeap_Check( void )
{
#ifdef __DEBUG

	UINT uiSave;
	int i;
	DWORD dwTotalSize = 0;

	LockIRQSave( &uiSave );

	for( i = 0; i < MAX_BLOCK_POOLS; i++ )
	{
		RETAILMSG( 1, ( "kheap size(%d),nTotalBlock(%d),Used(%d), sum=(%d).\r\n", heap[i].wBlockSize, heap[i].nTotalBlock, heap[i].iUsed,  heap[i].wBlockSize * heap[i].nTotalBlock ) );
		dwTotalSize += heap[i].wBlockSize * heap[i].nTotalBlock;
	}
	RETAILMSG( 1, ( "kheap dwTotalSize=%d.\r\n", dwTotalSize ) );	

	UnlockIRQRestore( &uiSave );

#endif
}

/*
void TestKHeap( void )
{
	int bContinue = 1;
	int nsocket;
	int index;
	int nFreeCount = 0;
	int nAllocCount = 0;
	int i;


	LPBYTE lpv[20];
	UINT   id[20];
	i = 0;
	
    KHeap_InitBlockHeap();
	memset( id, 0xffffffff, sizeof( id ) );
	while( bContinue )
	{
		if( (rand() % 20) <= 9 )
		{   //alloc
			nsocket = rand() % 20;

			if( id[nsocket] == -1 )
			{
				index = rand() % MAX_BLOCK_POOLS;//n % MAX_BLOCK_POOLS;//

				lpv[nsocket] = KHeap_AllocMemBlock( index );
			    id[nsocket] = index;
			    memset( lpv[nsocket], index, heap[index].wBlockSize );
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
						KHeap_FreeMemBlock( id[n], lpv[n] );
						id[n] = -1;
					}
				}
			}
			else
			{
				nsocket = rand() % 20;
				if( id[nsocket] != -1 )
				{
					KHeap_FreeMemBlock( id[nsocket], lpv[nsocket] );
					id[nsocket] = -1;
					nFreeCount++;
				}
			}
		}
		i++;
	}
}

*/
