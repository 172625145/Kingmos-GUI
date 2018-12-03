#ifndef __COREHEAP_H
#define __COREHEAP_H

//HANDLE CoreHeap_Create( void );
//BOOL CoreHeap_Release( HANDLE hHeap );
//int  CoreHeap_Add( HANDLE hHeap, void *lpBlock, DWORD dwSize );

#include <epheap.h>

BOOL CoreHeap_Init( LPHEAP lpHeap );
BOOL CoreHeap_Release( LPHEAP lpHeap );
#define HEAP_NO_SERIALIZE   1
#define HEAP_ZERO_MEMORY    2 
void * CoreHeap_Alloc( LPHEAP lpHeap, DWORD dwFlags, DWORD dwSizeNeeded );
BOOL CoreHeap_Free( LPHEAP lpHeap, DWORD dwFlags, void FAR * lpvUsed );
LPVOID CoreHeap_ReAlloc( LPHEAP lpHeap, DWORD dwFlags, LPVOID lpMem, DWORD dwBytes );

//typedef BOOL (CALLBACK * LPHEAPENUMPROC )( LPVOID lpMemBlock, LPARAM lParam );
int CoreHeap_Enum( LPHEAP lpHeap, LPHEAPENUMPROC lpfn, LPARAM lParam, UINT uiFlag );


/////////////////////



#endif    // __COREHEAP_H

