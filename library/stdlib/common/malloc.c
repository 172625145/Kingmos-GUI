#include <eframe.h>

void * Mem_Alloc( DWORD dwSizeNeeded )
{	
	void * p = NULL;
	HANDLE hHeap = GetProcessHeap();
	if( hHeap )
	{
		//EnterCriticalLock( hHeap );
		//_HeapCheck();
	    p = Heap_Alloc( hHeap, 0, dwSizeNeeded );
		//_HeapCheck();
		//LeaveCriticalLock( hHeap );
	}
	return p;
}

void Mem_Free( void FAR * lpvUsed )
{
	HANDLE hHeap = GetProcessHeap();
	if( hHeap )
	{
        //EnterCriticalLock( hHeap );				
		
	    Heap_Free( hHeap, 0, lpvUsed );
		//LeaveCriticalLock( hHeap );
	}
}

void * Mem_Realloc( void *p, DWORD dwResize )
{
	void * pRetv = NULL;
	HANDLE hHeap = GetProcessHeap();
	if( hHeap )
	{
	    //EnterCriticalLock( hHeap );
		pRetv = Heap_ReAlloc( hHeap, 0, p, dwResize );
        //LeaveCriticalLock( hHeap );
	}
	return pRetv;
}
/*
BOOL Mem_Enum( LPMEMENUM lpfn, LPARAM lParam, UINT uiFlag )
{
	HANDLE hHeap = GetProcessHeap();
	if( hHeap )
	{
		return Heap_Enum( hHeap, lpfn, lParam, uiFlag );
	}
	return FALSE;
}
*/


void * Mem_Calloc( DWORD num, DWORD size )
{
	void *ptr;

	size *= num;
	ptr = Mem_Alloc( size );
	if( ptr )
	    memset( ptr, 0, size );
	return ptr;	
}
