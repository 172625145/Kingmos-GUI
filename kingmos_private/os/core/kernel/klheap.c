/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵��������ѹ���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
    2003-05-14: ��API���ø�Ϊ Kernerl����, By ln
    2003-05-13: ���� ����δ���, By ln
    2003-05-12: HeapAlloc ���� HEAP_ZERO_MEMORY , By ln
    2003-05-09: LN, ���ѵı������Ƹ�Ϊ��ѡ������, By ln
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

// ����8bytes
#define ALIGNMASK (7)//sizeof(char*)<=2? 1: (sizeof(char*)<=4? 3: 7))

// 
static void _InsertToFree( LPHEAP, LPBLOCK );
static void _RemoveFromFree( LPHEAP, LPBLOCK );
static BOOL Heap_AddToSection( LPHEAP lpHeap, LPSECTION lp, ULONG ulLen );

// �������������
#define EnterCriticalLock( lpHeap, bSerial ) \
	do{ \
 	    if( (bSerial) && ( (lpHeap)->dwOption & HEAP_NO_SERIALIZE ) == 0 ) \
		{ \
			KL_EnterCriticalSection( &(lpHeap)->csHeap );\
		} \
	}while(0);

// ����������뿪
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
������BOOL Heap_Init( LPHEAP lpHeap, DWORD dwOption, ULONG ulInitialSize, ULONG ulLimit )
������
	lpHeap - HEAP �ṹָ�룬���ڱ������Ϣ
	dwOption - ���ӵĹ��ܣ�����Ϊ����ֵ��
		HEAP_NO_SERIALIZE - �Ժ��HeapAlloc, HeapFree�ȹ��ܲ���Ҫ���л������������
		HEAP_VIRTUAL_ALLOC - ��ϵͳ�ڴ�ռ�����ڴ�ķ�������ֵ˵����VirtualAlloc���ܴ�ϵͳ���䣻
		                     ������DoAllocPageMem���ܴ�ϵͳ����
	ulInitialSize - ��ʼ���ѳߴ�
	ulLimit - �ѳߴ��������
����ֵ��
	����ɹ���TRUE�����򣬷���FALSE
����������
	��ʼ��һ���ѣ��Ա������Ժ�ĶѲ���
����: 
	
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
		{   // Ԥ���ڴ�
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
		{   // �ύ�����ڴ�
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
������HANDLE DoHeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize )
������
	flOptions - ���ӵĹ��ܣ�����Ϊ����ֵ��
		HEAP_NO_SERIALIZE - �Ժ��HeapAlloc, HeapFree�ȹ��ܲ���Ҫ���л������������
		HEAP_VIRTUAL_ALLOC - ��ϵͳ�ڴ�ռ�����ڴ�ķ�������ֵ˵����VirtualAlloc���ܴ�ϵͳ���䣻
		                     ������DoAllocPageMem���ܴ�ϵͳ����
	dwInitialSize - ��ʼ���ѳߴ�
	dwMaximumSize - �ѳߴ��������
����ֵ��
	����ɹ�����Ч�ľ�������򣬷���NULL
����������
	��ʼ��һ���ѣ��Ա������Ժ�ĶѲ���
����: 
	process.c�øú���ȥ��ʼ�����̶�
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
������BOOL WINAPI DoHeapDestroy( HANDLE hHeap )
������
	hHeap - �Ѿ����Ϊ֮ǰ����KL_HeapCreate�����صľ����
����ֵ��
	����ɹ�������TRUE; ���򣬷���FALSE
����������
	�ƻ��ѵĲ�������
����: 
	�ں˵���

************************************************/

BOOL DoHeapDestroy( LPHEAP lpHeap )
{
//	LPHEAP lpHeap = (LPHEAP)hHeap;

//   1.��Ϊ �û��ѵ�����ָ��Ҳ�����û������ָ��Ҳ�п��ܱ��ƻ���
//		������������ڵ���KL_HeapDestroy�п��ܳ����쳣���������ﲻ��ȥ�ͷ� heap
//   2.Seg ���������ͷ����е��û��ռ䣨�����û��ѣ�
//   3.�ں����ͷŽ���ʱ����øú���

	// ���뻥���		
	if( lpHeap )
	{
		if( (lpHeap->dwOption & HEAP_NO_SERIALIZE) == 0 )
			KL_DeleteCriticalSection( &lpHeap->csHeap );
	       // �ͷŷ���Ķ���Ϣ�ṹ		
		KHeap_Free( lpHeap, sizeof( HEAP ) );
		return TRUE;
	}
	return FALSE;
}


/**************************************************
������HANDLE WINAPI KL_HeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize )
������
	flOptions - ���ӵĹ��ܣ�����Ϊ����ֵ��
		HEAP_NO_SERIALIZE - �Ժ��HeapAlloc, HeapFree�ȹ��ܲ���Ҫ���л������������
	dwInitialSize - ��ʼ���ѳߴ�
	dwMaximumSize - �ѳߴ��������
����ֵ��
	����ɹ�����Ч�ľ�������򣬷���NULL
����������
	��ʼ��һ���ѣ��Ա������Ժ�ĶѲ���
����: 
	ϵͳAPI����
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
������static BOOL Heap_AddToSection( LPHEAP lpHeap, LPSECTION lp, ULONG ulLen )
������
	lpHeap - HEAP �ṹָ�룬ָ�����Ϣ
	lpsc - SECTION �ṹָ�룬������ӵ����ڴ��
	ulLen - ���ڴ�γߴ�
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE
����������
	��һ�����ڴ�μ��������
����: 
	
************************************************/
static BOOL Heap_AddToSection( LPHEAP lpHeap, LPSECTION lpsc, ULONG ulLen )
{
	// ��ʼ��SECTION �ṹ
	lpsc->ulLen = ulLen;
	lpsc->lpNext = lpHeap->lpSection;
	// ���������
	lpHeap->lpSection = lpsc;
	return TRUE;
}

/**************************************************
������void * GetSystemMem( LPHEAP lpHeap, DWORD dwSize, DWORD * lpdwRealSize )
������
	lpHeap - HEAP �ṹָ�룬ָ�����Ϣ
	dwSize - ��Ҫ���ڴ��С
	lpdwRealSize - ���ص���ʵ�ڴ��С�� > dwSize )
����ֵ��
	����ɹ������ط�NULL���ڴ�ָ�룻���򣬷���NULL
����������
	��Ȼ����û���㹻���ڴ�飬���øú�����ϵͳ�ڴ�����ڴ�飬��������������
 
����: 
    Heap_Alloc, Heap_Realloc	
************************************************/
#define DEBUG_GETSYSTEMMEM 0
static void * GetSystemMem( LPHEAP lpHeap, DWORD dwSize, DWORD * lpdwRealSize )
{
	//RETAILMSG( 1, ( "GetSystemMem:lpHeap=%x,dwSize=%d,ProcessId=%d.\r\n", lpHeap, dwSize, KL_GetCurrentProcessId() ) );

#ifdef VIRTUAL_MEM
	// ���ϵͳ��MMU�������øöδ���
    if( lpHeap->dwOption & HEAP_VIRTUAL_ALLOC )
	{   // ��VirtualAllocȥ�����ڴ�
		void * p = NULL;
		DWORD dwCommitSize;
		UINT uiNewSectionSize = 0;
		
		*lpdwRealSize = 0;
		
		dwCommitSize = (dwSize + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));  // align to page
		
		if( lpHeap->dwCurUseSize + dwCommitSize > lpHeap->dwCurReserveSize )
		{   // ��ǰ����δ�ύ�����������ڴ��� С�� ��Ҫ���ڴ�ߴ磬��������µ��ڴ���/��
			//dwSize += sizeof(SECTION);
			if( lpHeap->dwHeapSizeLimit )
			{   // ����������ѵĴ�С���ƣ������
				RETAILMSG( 1, ( "error: GetSystemMem:Heap reach size limite(%d).\r\n", lpHeap->dwHeapSizeLimit ) );
				return NULL;
			}
			else
			{	// û�гߴ�����,����һ���µ�
				DWORD dwReserveSize = dwSize + sizeof(SECTION);
				// ���뵽ALLOC_SIZE��С
				dwReserveSize = (dwReserveSize + ALLOC_SIZE - 1) & (~(ALLOC_SIZE - 1));// align to 64k
				// �ȱ����ڴ���
				lpHeap->lpbCurReserveBase = KL_VirtualAlloc( 0, dwReserveSize, MEM_RESERVE, PAGE_READWRITE );
				if( lpHeap->lpbCurReserveBase == NULL )
				{   //�����ڴ���ʧ��
					WARNMSG( DEBUG_GETSYSTEMMEM, ( "error: GetSystemMem: can't to reserve space size(%d).\r\n", dwReserveSize ) );
					dwReserveSize = 0;
				}
				else
					uiNewSectionSize = sizeof(SECTION);
				// ��ʼ���µ��ڴ���
				lpHeap->lpbCur = lpHeap->lpbCurReserveBase;
				lpHeap->dwCurReserveSize = dwReserveSize;
				lpHeap->dwCurUseSize = 0;
				lpHeap->dwTotalReserveSize += dwReserveSize;
			}
		}
		
		if( lpHeap->dwHeapSizeLimit && 
			lpHeap->dwCurUseSize == NULL )
			uiNewSectionSize = sizeof(SECTION);  // Ϊsection�����ռ�
		// ��ʵ����Ҫ�ύ/������ڴ�
		dwCommitSize = (dwSize + uiNewSectionSize + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));
		// �Ϸ����
		if( lpHeap->dwCurUseSize + dwCommitSize <= lpHeap->dwCurReserveSize )
		{   // �ύ/�����µ��ڴ��
			p = KL_VirtualAlloc( lpHeap->lpbCur, dwCommitSize, MEM_COMMIT, PAGE_READWRITE );
			if( p )
			{
#ifdef __DEBUG
				memset( p, 0xCCCCCCCC, dwCommitSize );
#endif
				
				if( uiNewSectionSize )
				{  // ��Ҫ������������
					Heap_AddToSection( lpHeap, (LPSECTION)p, lpHeap->dwCurReserveSize );
					p = (LPBLOCK)( (DWORD)p + uiNewSectionSize );
				}
				// ��ȥ�ѷ�����ڴ��С
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
	{   // ��DoAllocPageMemȥ�����ڴ�
		LPBYTE p = NULL;
		*lpdwRealSize = 0;
		dwSize += sizeof(SECTION);  // ��ʵ����Ҫ������ڴ��С
		
		p = DoAllocPageMem( dwSize, lpdwRealSize, 0 );
		if( p )
		{   // ������������
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
������static BOOL Heap_MemCheck( LPHEAP lpHeap )
������
	lpHeap - HEAP �ṹָ�룬ָ�����Ϣ
����ֵ��
	����ɹ�������TRUE; ���򣬷���FALSE
����������
	�����Ƿ�Ϸ�

����: 
	
************************************************/

static BOOL Heap_MemCheck( LPHEAP lpHeap )
{
    LPBLOCK lpBlock, lpFirstBlock;

	// ���뻥���
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

	// �뿪�����
	LeaveCriticalLock( lpHeap, TRUE );

	return TRUE;
}

/**************************************************
������BOOL WINAPI KL_HeapDestroy( HANDLE hHeap )
������
	hHeap - �Ѿ����Ϊ֮ǰ����KL_HeapCreate�����صľ����
����ֵ��
	����ɹ�������TRUE; ���򣬷���FALSE
����������
	�ƻ���
����: 
************************************************/

BOOL WINAPI KL_HeapDestroy( HANDLE hHeap )
{
	LPHEAP lpHeap = (LPHEAP)hHeap;

#ifdef VIRTUAL_MEM

	// ���ϵͳ��MMU�������øöδ���

    if( lpHeap->dwOption & HEAP_VIRTUAL_ALLOC )
	{   // // ��VirtualFreeȥ�ͷ��ڴ�
		LPSECTION lps, lpNext;

		// ���뻥���
		EnterCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add
		
		RETAILMSG( 1, ( "Heap_Release:lpHeap=%x.lpbBase=%x.\r\n", lpHeap, lpHeap->lpbCurReserveBase ) );
		// �ͷ����е��ڴ��
		lps = lpHeap->lpSection;
		while( lps )
		{
			lpNext = lps->lpNext;
			KL_VirtualFree( lps, 0, MEM_RELEASE );
			lps = lpNext;
		}
		// �뿪�����
		LeaveCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add
		
		return DoHeapDestroy( lpHeap );
		
//		if( (lpHeap->dwOption & HEAP_NO_SERIALIZE) == 0 )
//			KL_DeleteCriticalSection( &lpHeap->csHeap );
        // �ͷŷ���Ķ���Ϣ�ṹ		
//		KHeap_Free( lpHeap, sizeof( HEAP ) );
		
//		return TRUE;
	}

	else

#endif

	{   // ��DoFreePageMemȥ�ͷ��ڴ�		
		LPSECTION lps, lpNext;
		// ���뻥���
		EnterCriticalLock( lpHeap, TRUE );
		// �ͷ����е��ڴ��
		lps = lpHeap->lpSection;
		while( lps )
		{
			lpNext = lps->lpNext;
			DoFreePageMem( lps, lps->ulLen );
			lps = lpNext;
		}
		// �뿪�����
		LeaveCriticalLock( lpHeap, TRUE );
//		if( (lpHeap->dwOption & HEAP_NO_SERIALIZE) == 0 )
//			KL_DeleteCriticalSection( &lpHeap->csHeap );
		
//		KHeap_Free( lpHeap, sizeof( HEAP ) );
		return DoHeapDestroy( lpHeap );		
		
//		return TRUE;
	}

}


/**************************************************
������void _InsertToFree( LPHEAP lpHeap, LPBLOCK p )
������
	lpHeap - HEAP�ṹָ��
	p - �ڴ��
����ֵ��
	��
����������
	��һ���ڴ�����ѿ��������Ա��Ժ�ʹ��
����: 
************************************************/

static void _InsertToFree( LPHEAP lpHeap, LPBLOCK p )
{
	p->lpPrevFree = 0;
    if ( (p->lpNextFree = lpHeap->lpFreeList) != 0 )
        lpHeap->lpFreeList->lpPrevFree = p;
    lpHeap->lpFreeList = p;
}

/**************************************************
������void _RemoveFromFree( LPHEAP lpHeap, LPBLOCK p )
������
	lpHeap - HEAP�ṹָ��
	p - �ڴ��
����ֵ��
	��
����������
	��һ���ڴ���Ƴ��ѿ��������Ա�ʹ��
����: 
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
������void * WINAPI KL_HeapAlloc( HANDLE hHeap, DWORD dwFlags, DWORD dwSizeNeeded )
������
	hHeap - �Ѿ��
	dwFlags - ���ܣ�����Ϊ����ֵ��
		HEAP_NO_SERIALIZE - ����Ҫ���л������������
	dwSizeNeeded - ��Ҫ������ڴ��С
����ֵ��
	����ɹ������ط�NULL��ָ��; ���򷵻�NULL
����������
	�Ӷ������һ���ڴ��
����:
	ϵͳAPI 
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

    // �����С
	dwSizeNeeded = ((dwSizeNeeded + ALIGNMASK) & ~ALIGNMASK) + sizeof(BUSY);

    if ( dwSizeNeeded < sizeof(BLOCK) ) 
		dwSizeNeeded = sizeof(BLOCK);

    lpBigBlock = 0;	// ���ڼ�¼ > dwSize ����С��һ�����п�
    dwBigSize = 0;	// ���ڼ�¼ lpBigBlock�Ĵ�С

    //���뻥���
    EnterCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add

	DEBUGMSG( DEBUG_HEAPALLOC, ( "+++M(0x%x,lock=%d,dwOption=0x%x)M+++.\r\n", KL_GetCurrentThreadId(), lpHeap->iLockCount, lpHeap->dwOption ) );

    // ���Ҷѿ�������-�����д���dwSizeNeeded�Ŀ��п���õ�һ���ʵ��Ŀ��п�
    for ( p = lpHeap->lpFreeList; p; p = p->lpNextFree )
    {
	    if ( (dwSize = (char*)p->busy.lpNext - (char*)p) >= dwSizeNeeded )
        {   // ���п�ĳߴ������Ҫ�ĳߴ�
	        if ( dwSize < dwSizeNeeded + dwSizeNeeded ) 
			{   // ����ÿ��п�ĳߴ�С����Ҫ�ĳߴ������������Ҫ���˳�����
				break;
			}
			// ���п�ĳߴ�̫�����������������Ŀǰ�õ��Ŀ��п�����С��һ�飩
			// ���䱣���� lpBigBlock, dwBigSize��
	        if ( !lpBigBlock || dwSize < dwBigSize )
            {
                lpBigBlock = p; 
				dwBigSize = dwSize;
            }
        }
    }
    // �Ƿ��Ѿ��õ����������Ŀ��п�
    if ( p == NULL && (p = lpBigBlock) == NULL )
    {   //û�У���Ҫ��ϵͳ�����ڴ�ȥ�������ڴ棬dwRealSize���ڱ���ʵ�ʵõ����ڴ���С

		p = GetSystemMem( lpHeap, dwSizeNeeded+sizeof(BUSY), (DWORD*)&dwRealSize );

	    if ( p == NULL )
        {  // �޷���ϵͳ�����ڴ棬�˳�
			DEBUGMSG( DEBUG_HEAPALLOC, ( "---M(0x%x)M---.\r\n", KL_GetCurrentThreadId() ) );
			LeaveCriticalLock( lpHeap, TRUE );
	        goto M_RETNULL;
        }

		ASSERT( dwRealSize >= dwSizeNeeded+sizeof(BUSY) );

	    dwRealSize &= ~ALIGNMASK;

	    if ( lpHeap->lpLastBlock &&
	         (char*)p == ((char*)lpHeap->lpLastBlock + sizeof(BUSY)) )
        {   // �µ��ڴ����֮ǰ��������ڴ���������ģ����ǽ������Ϊһ�����

	        p = lpHeap->lpLastBlock;
	        dwRealSize += sizeof(BUSY);
	        /********************************************************
		    Check if previous block is free and combine it now.
	        ********************************************************/
	        // ���ǰһ�����ǿ��еģ��������µ�������һ��
			if ( (x = p->busy.lpPrev) != 0 && !IS_BUSY(x) )
            {
		        _RemoveFromFree( lpHeap, x );  // �ӿ��������Ƴ�
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

		// ����һ����С��BLOCK�ṹ�����ڼ�¼���ڴ�εĽ���

	    p->busy.lpNext = lpHeap->lpLastBlock =
	        (LPBLOCK)((char*)p + dwRealSize - sizeof(BUSY));
	    lpHeap->lpLastBlock->busy.lpPrev = p;
		lpHeap->lpLastBlock->busy.lpNext = (LPBLOCK)(BUSY_FLAG);
        // ���µĶβ����������
	    _InsertToFree( lpHeap, p);
    }
    // p ָ����п�
    if ( (char*)p->busy.lpNext-(char*)p-dwSizeNeeded > sizeof(BLOCK) )
    {
		// ���pָ��Ŀ��п�ϴ����pָ��Ŀ��п�ָ��幻���ڴ�飬����ʣ�µķ����������

	    x = (LPBLOCK)((char*)p + dwSizeNeeded);
	    p->busy.lpNext->busy.lpPrev = x;
	    x->busy.lpPrev = p;
	    x->busy.lpNext = p->busy.lpNext;
	    p->busy.lpNext = (LPBLOCK)((char*)x + BUSY_FLAG + ALLOC_FLAG );
	    _RemoveFromFree( lpHeap, p );
	    _InsertToFree( lpHeap, x );
    }
    else
    {   //��p�ӿ��������Ƴ�
	    _RemoveFromFree( lpHeap, p );
	    p->busy.lpNext = (LPBLOCK)((char*)p->busy.lpNext+BUSY_FLAG +ALLOC_FLAG);
    }

	//�뿪�����
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
������BOOL IsPtrValid( void FAR * lpvUsed )
������
	lpHeap - �Ѿ��ָ��
	lpvUsed - ��Ҫ�����ָ�루�ѷ�����ڴ��ַ��
����ֵ��
	����ɹ�������TRUE; ���򷵻�FALSE
����������
	�����ָ���Ƿ�Ϸ�
����:
	
************************************************/
#define DEBUG_IsPtrValid 1
static BOOL IsPtrValid( LPCVOID lpvUsed, LPBLOCK * lppRet, LPBLOCK * lpxRet )
{
	LPBLOCK p, x;

    p = (LPBLOCK)((char*)lpvUsed - sizeof(BUSY));
    if ( !IS_BUSY(p) )
    {   // ��Ч��ָ��
		WARNMSG( DEBUG_IsPtrValid, ( "error0: invalid ptr(0x%x),(p=0x%x).\r\n" , lpvUsed, p ) );
        KL_SetLastError( ERROR_INVALID_PARAMETER );
        //goto ret;
		return FALSE;
    }
    x = (LPBLOCK)((char*)p->busy.lpNext-(BUSY_FLAG+ALLOC_FLAG) );
    if ( !x || x->busy.lpPrev != p )
    {   // ��Ч��ָ��
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
������BOOL WINAPI KL_HeapFree( HANDLE hHeap, DWORD dwFlags, void FAR * lpvUsed )
������
	hHeap - �Ѿ��
	dwFlags - ���ܣ�����Ϊ����ֵ��
		HEAP_NO_SERIALIZE - ����Ҫ���л������������
	lpvUsed - ֮ǰ������ڴ��ַ
����ֵ��
	����ɹ�������TRUE; ���򷵻�FALSE
����������
	�ͷ�֮ǰ��KL_HeapAlloc ��KL_HeapRealloc���ص�һ���ڴ��
����:
	ϵͳAPI 
************************************************/
#define DEBUG_HEAPFREE 0
BOOL WINAPI KL_HeapFree( HANDLE hHeap, DWORD dwFlags, void FAR * lpvUsed )
{
    LPBLOCK p, x;
	BOOL bRetv = FALSE;
	LPHEAP lpHeap = (LPHEAP)hHeap;

	ASSERT_NOTIFY( lpvUsed, "Invalid param at _Free:" );

    if ( !lpvUsed ) return FALSE;

	//���뻥���

	EnterCriticalLock( lpHeap, TRUE );
	
	DEBUGMSG( DEBUG_HEAPALLOC, ( "+++F(0x%x,lock=%d)F+++.\r\n", KL_GetCurrentThreadId(), lpHeap->iLockCount ) );
/*	2004-12-03 remoeve to IsPtrValid 
    p = (LPBLOCK)((char*)lpvUsed - sizeof(BUSY));
    if ( !IS_BUSY(p) )
    {   // ��Ч��ָ��
		WARNMSG( DEBUG_HEAPFREE, ( "error0: invalid ptr(0x%x),(p=0x%x) when free.\r\n" , lpvUsed, p ) );
        KL_SetLastError( ERROR_INVALID_PARAMETER );
        goto ret;
    }
    x = (LPBLOCK)((char*)p->busy.lpNext-(BUSY_FLAG+ALLOC_FLAG) );
    if ( !x || x->busy.lpPrev != p )
    {   // ��Ч��ָ��
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
	{ // �����һ����Ҳ�ǿ��еģ����������
		_RemoveFromFree( lpHeap, x );
		p->busy.lpNext = x->busy.lpNext;
		x->busy.lpNext->busy.lpPrev = p; 
	}
	
    if ( (x = p->busy.lpPrev) != 0 && !IS_BUSY(x) ) 
	{ // ���ǰһ����Ҳ�ǿ��еģ����������
		_RemoveFromFree( lpHeap, x );
		x->busy.lpNext = p->busy.lpNext;
		p->busy.lpNext->busy.lpPrev = x;
		p = x; 
	}

    //���ÿ����ѿ�������
    _InsertToFree( lpHeap, p );

	bRetv = TRUE;

ret:
	//�뿪�����
    DEBUGMSG( DEBUG_HEAPALLOC, ( "---F(0x%x)F---.\r\n", KL_GetCurrentThreadId() ) );
    LeaveCriticalLock( lpHeap, TRUE );
	
	return bRetv;
}

/**************************************************
������LPVOID WINAPI KL_HeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpOldPtr, DWORD nNewAllocSize )
������
	hHeap - �Ѿ��
	dwFlags - ���ܣ�����Ϊ����ֵ��
		HEAP_NO_SERIALIZE - ����Ҫ���л������������
	lpOldPtr - ֮ǰ�Ѿ�������ڴ档�����NULL����ͬ��KL_HeapAlloc
	nNewAllocSize - ��Ҫ���·�����ڴ��С�� �����0,��ͬ��KL_HeapFree
����ֵ��
	����ɹ������ط�NULL��ָ��; ���򷵻�NULL������ı�֮ǰ��ָ��
����������
	�Ӷ������·���һ���ڴ��
����:
	ϵͳAPI 
************************************************/
#define DEBUG_HEAPREALLOC 0
LPVOID WINAPI KL_HeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpOldPtr, DWORD nNewAllocSize )
{
    register void *newp = 0;
    LPBLOCK p, next, x;
    register DWORD oldsz;
	LPHEAP lpHeap = (LPHEAP)hHeap;
	DWORD dwOldSizeSave = 0;
	
	// �����NULL����ͬ��KL_HeapAlloc
    if ( !lpOldPtr )
        return KL_HeapAlloc( lpHeap, dwFlags, nNewAllocSize );
	// �����0,��ͬ��KL_HeapFree
    if ( !nNewAllocSize )
    {
        KL_HeapFree( lpHeap, dwFlags, lpOldPtr );
        return 0;
    }

    // ����
    nNewAllocSize = (nNewAllocSize + ALIGNMASK) & ~ALIGNMASK;
    
	//���뻥���
	EnterCriticalLock( lpHeap, TRUE );

	/* 2004-12-03 remove to IsPtrValid
    // ���Ϸ���
    p = (LPBLOCK)((char*)lpOldPtr - sizeof(BUSY));
    if ( !IS_BUSY(p) ) 
	{// ��Ч��ָ��
        KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_HEAPREALLOC, ( "error: realloc: invalid ptr(0x%x).\r\n", p ) );
		goto ret;
	}
    next = (LPBLOCK)((char*)p->busy.lpNext-(BUSY_FLAG+ALLOC_FLAG) );
    if ( !next || next->busy.lpPrev != p ) 
	{// ��Ч��ָ��
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

	
	//�õ��ߴ磨������ͷ��
    dwOldSizeSave = oldsz = (char *)next - (char *)lpOldPtr;
	
    if ( oldsz >= nNewAllocSize ) 
	{  
		register DWORD sizeleft;
        // �µ��ڴ���ҪС��֮ǰ������ϵ��ڴ�飬����ֻ������ϵ��ڴ��
resize:
		sizeleft = oldsz - nNewAllocSize;  //�ͷ�ĳЩbytes
		if ( sizeleft > sizeof(BLOCK) ) 
		{   // ��������������
			x = (LPBLOCK)((char*)next - sizeleft);
			x->busy.lpNext = next;
			x->busy.lpPrev = p;
			p->busy.lpNext = (LPBLOCK)( (char*)x+(BUSY_FLAG+ALLOC_FLAG) );
			next->busy.lpPrev = x;
			_InsertToFree( lpHeap, x );
			if ( !IS_BUSY(next) ) 
			{   //�������һ�����п�ĵ�ַ�����������������
				_RemoveFromFree( lpHeap, next );
				x->busy.lpNext = next->busy.lpNext;
				next->busy.lpNext->busy.lpPrev = x; 
			}
		}
		newp = lpOldPtr; 
	}
	
    else if ( !IS_BUSY(next) && (x=next->busy.lpNext) != 0 &&
		(oldsz = (char*)x - (char*)lpOldPtr) >= nNewAllocSize ) 
	{  //��һ�����ǿ��п鲢�����С������Ҫ�ĳߴ�
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
		//ǰһ�����ǿ��п鲢�����С������Ҫ�ĳߴ�
		_RemoveFromFree( lpHeap, x );
		x->busy.lpNext = (LPBLOCK)( (char*)next + (BUSY_FLAG+ALLOC_FLAG) );
		next->busy.lpPrev = x;
		from = (char*)p + sizeof(BUSY);
		to   = (char*)x + sizeof(BUSY);
		n = (char *)next - (char *)lpOldPtr;
        // ��������
		memmove( to, from, n );
		p = x;
		lpOldPtr = (char*)p + sizeof(BUSY);
		goto resize; 
	}
    else 
	{
		// ��Ҫһ����ȫ�µĿ�
		LeaveCriticalLock( lpHeap, TRUE );//LN:2003-05-13, Add

        // ���·���
		if ( !(newp = KL_HeapAlloc( lpHeap, dwFlags, nNewAllocSize ) ) ) //;Mem_Alloc( nNewAllocSize )) ) 
			return 0;
		// ��������
		memmove( newp, lpOldPtr, (char *)next - (char *)lpOldPtr );
		// �ͷ��ϵ�
		KL_HeapFree( lpHeap, dwFlags, lpOldPtr );
		return newp; 
	}
	
ret:

	//�뿪�����

	LeaveCriticalLock( lpHeap, TRUE );

	if( dwOldSizeSave < nNewAllocSize && (dwFlags & HEAP_ZERO_MEMORY ) )
	{   // �����ڵ��ڴ�������Ϊ0
		memset( (char*)newp + dwOldSizeSave, 0, nNewAllocSize - dwOldSizeSave );
	}
    return newp;
}

/**************************************************
������int Heap_Enum( LPHEAP lpHeap, LPHEAPENUMPROC lpfn, LPARAM lParam, UINT uiFlag )

������
	lpHeap - HEAP�ṹָ��
	lpfn - ö�ٹ���
	lParam - ���ݸ�ö�ٹ��ܵĲ���
	uiFlag - ����
����ֵ��
	����ɹ�������TRUE; ���򷵻�FALSE
����������
	ö�������ѷ���Ŀ�
����:
	
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
������BOOL WINAPI KL_HeapValidate( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem )

������
	hHeap -�Ѿ��������HeapCreate���صľ����
	dwFlags �C ���ԣ������ǣ�
		HEAP_NO_SERIALIZE �C ˵�������жѲ���ʱ���ؽ��л���
	lpMem �C ����������ΪNULL
����ֵ��
	����ɹ�������TRUE; ���򷵻�FALSE
����������
	�����Ƿ���Ч
����:
	ϵͳAPI	
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
//�ڲ�ʹ��
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
