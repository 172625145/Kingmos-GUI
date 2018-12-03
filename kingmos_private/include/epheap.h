#ifndef __EPHEAP_H
#define __EPHEAP_H

//HANDLE Heap_Create( void );
//BOOL Heap_Release( HANDLE hHeap );
//int  Heap_Add( HANDLE hHeap, void *lpBlock, DWORD dwSize );

////////////////////////////
//typedef struct _BLOCK;	//changed for gcc by lilin.
struct _BLOCK;

typedef struct _BUSY {
    struct _BLOCK FAR * lpPrev;	// previous physical block (0 if none)
	struct _BLOCK FAR * lpNext;	// next physical block (0 if none)
}BUSY, FAR * LPBUSY;

typedef struct _SECTION {
    //struct _SECTION FAR * lpPrev;	// previous physical block (0 if none)
	struct _SECTION FAR * lpNext;	// next physical block (0 if none)
	ULONG ulLen;
}SECTION, FAR * LPSECTION;

typedef struct _BLOCK {
	BUSY busy;  //   BIT0 set if is this block is BUSY_FLAG 
    struct _BLOCK FAR * lpPrevFree;	// (free only) free list backward link 
    struct _BLOCK FAR * lpNextFree;	// (free only) free list forward link 
}BLOCK, FAR * LPBLOCK;

#define HEAP_VIRTUAL_ALLOC 0x80000000
typedef struct _HEAP{
	//CRITICAL_SECTION  csHeap;
	int iLockCount;

	CRITICAL_SECTION csHeap;
	DWORD dwOption;
	//DWORD dwThreadID;
	DWORD dwHeapSizeLimit;

	DWORD dwTotalReserveSize;
	DWORD dwTotalCommitSize;

	DWORD dwCurReserveSize;
	DWORD dwCurUseSize;
	LPBYTE lpbCurReserveBase;
	LPBYTE lpbCur;

	LPSECTION lpSection;

    LPBLOCK lpFirstBlock;	// Head of doubly linked block list 
    LPBLOCK lpLastBlock;		// Dummy block at end of last section
    LPBLOCK lpFreeList;		// Head of doubly linked free list

}HEAP, FAR * LPHEAP;

//#define INIT_HEAP \
//{ 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL }

//BOOL Heap_Init( LPHEAP lpHeap );
//BOOL Heap_Release( LPHEAP lpHeap );

//void * Heap_Alloc( LPHEAP lpHeap, DWORD dwFlags, DWORD dwSizeNeeded );
//BOOL Heap_Free( LPHEAP lpHeap, DWORD dwFlags, void FAR * lpvUsed );
//LPVOID Heap_ReAlloc( LPHEAP lpHeap, DWORD dwFlags, LPVOID lpMem, DWORD dwBytes );

typedef BOOL (CALLBACK * LPHEAPENUMPROC )( LPVOID lpMemBlock, LPARAM lParam );
//int Heap_Enum( LPHEAP lpHeap, LPHEAPENUMPROC lpfn, LPARAM lParam, UINT uiFlag );
HANDLE DoHeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize );
BOOL DoHeapDestroy( LPHEAP lpHeap );

/////////////////////



#endif    // __EPHEAP_H

