#ifndef __PAGEMGR_H
#define __PAGEMGR_H

#define FLAG_USED 1
#define FLAG_FREE 0

#define ALIGN_PAGE_UP( v ) ( ( (v) + PAGE_SIZE - 1 ) & ~(PAGE_SIZE-1) )
#define ALIGN_PAGE_DOWN( v ) ( (v) & ~(PAGE_SIZE-1) )
#define ALIGN_DWORD( v ) ( ( (v) + sizeof(DWORD) - 1 ) & ~( sizeof(DWORD) - 1 ) )

//LPBYTE  lpbSysMainMem;// = _mem;
//extern ULONG   ulSysMainMemLength;// = MEM_SIZE;
//extern UINT    _uiHeapSize;//;

typedef struct _PAGELIST
{
	struct _PAGELIST FAR * lpNext;
	struct _PAGELIST FAR * lpPrev;
}PAGELIST, FAR * LPPAGELIST;

//#define MAX_SEGMENTS 16

typedef struct _SEGMENTINFO
{
	LPBYTE lpbMemStart;
	DWORD  dwMemSize;
	LPBYTE lpbFreeMemMap;
	LPBYTE lpbFreePageStart;
	LPBYTE lpbFreePageEnd;
	DWORD  dwTotalPages;
}SEGMENTINFO, FAR * LPSEGMENTINFO;

typedef struct _MEMINFO
{
	LPBYTE lpbMainMemStart;
	LPBYTE lpbMainMemEnd;
	LPBYTE lpbMainMemFreeStart;
	DWORD  dwMainMemFreeSize;
	DWORD  dwTotalFreePages;
	DWORD  dwSections;
	LPSEGMENTINFO lpSectionInfo;
}MEMINFO, FAR * LPMEMINFO;

//void Page_LinkToList( LPPAGELIST lpList );
//void Page_UnlinkFromList( LPPAGELIST lpList );
//void Page_AddToFreeList( LPPAGELIST lpList );
//void Page_RemoveFromFreeList( LPPAGELIST lpList, BOOL bUpdateFreePages );
BOOL Page_SetMapFlag( void * lpvMemAdr, BYTE bFlag );
BOOL Page_Lock( DWORD dwPages );
BOOL Page_Unlock( DWORD dwPages );
void * Page_Alloc( BOOL bUpdateFreePages );
BOOL Page_Free( LPVOID lpvMemAdr  );
//BOOL Page_GetSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex );
//BOOL Page_ReleaseSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex );
BOOL Page_Get( LPVOID * lppAdr, DWORD dwPages );
BOOL Page_Release( LPVOID * lppAdr, DWORD dwPages );
LPVOID Page_GetContiguous( DWORD dwPages, DWORD dwAlignmentMask );




#endif  //__PAGEMGR_H

