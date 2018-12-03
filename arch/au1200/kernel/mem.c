//#include <stdio.h> //windows.h>
#include <edef.h>
#include <stdlib.h>
void * Mem_Alloc( DWORD dwSizeNeeded )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return malloc( dwSizeNeeded );
}

void Mem_Free( void FAR * lpvUsed )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	free( lpvUsed );
}

void * Mem_Realloc( void *p, DWORD dwResize )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return realloc( p, dwResize );
}

//BOOL WINAPI Heap_Free( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
//{
//}

//LPVOID WINAPI Heap_Alloc( HANDLE hHeap, DWORD dwFlags, ULONG dwBytes )
//{
//}

void * Mem_Calloc( DWORD num, DWORD size )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return calloc( num, size );
}
