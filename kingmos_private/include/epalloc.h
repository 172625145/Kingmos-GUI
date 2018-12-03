#ifndef __EPALLOC_H
#define __EPALLOC_H

//#define _KERNEL_SIZE (40*1024*sizeof(int))

void * _kalloc( size_t sz );
void _kfree( void * p );
void * _krealloc( void *pold, size_t sz );

void * WINAPI KL_Alloc( DWORD dwSize, DWORD * lpdwRealSize, UINT uiFlag );
BOOL WINAPI KL_Free( LPVOID lpvMem, ULONG uLen );
void * WINAPI KL_AllocPage( void );
BOOL WINAPI KL_FreePage( void * lpv );
BOOL WINAPI KL_InitMem( void );

//#define __CHECK_MEM

#ifdef __CHECK_MEM

	LPVOID _KHeap_Alloc( UINT uiSize, const char * lpszFile, int line );
	BOOL _KHeap_Free( LPVOID lpv, UINT uiSize, const char * lpszFile, int line );
	LPVOID _KHeap_AllocString( UINT uiSize, const char * lpszFile, int line );
	BOOL _KHeap_FreeString( LPVOID lpv, const char * lpszFile, int line );
	#define KHeap_Alloc( uiSize ) _KHeap_Alloc( (uiSize), __FILE__, __LINE__ )
	#define KHeap_Free( lpv, uiSize ) _KHeap_Free( (lpv), (uiSize), __FILE__, __LINE__ )
	#define KHeap_AllocString( uiSize ) _KHeap_AllocString( (uiSize), __FILE__, __LINE__ )
	#define KHeap_FreeString( lpv ) _KHeap_FreeString( (lpv), __FILE__, __LINE__ )

#else

	LPVOID KHeap_Alloc( UINT uiSize );
	BOOL KHeap_Free( LPVOID lpv, UINT uiSize );
	LPVOID KHeap_AllocString( UINT uiSize );
	BOOL KHeap_FreeString( LPVOID lpv );

#endif

#endif  // __EPALLOC_H


