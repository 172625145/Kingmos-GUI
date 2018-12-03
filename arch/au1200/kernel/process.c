//#include <windows.h>

//#define KINGMOS_GWME_PROCESS_ID     0xFF094530
//#define KINGMOS_GWME_PROCESS_HANDLE ((HANDLE)0xFF094531)
#include <edef.h>
#include <ecore.h>
#include "handlepackage.h"

HANDLE WINAPI Process_GetCaller( void )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return NULL;//GetCurrentProcess();//KINGMOS_GWME_PROCESS_HANDLE;
}

HANDLE WINAPI Process_GetCurrent( void )
{
	//return NULL;//GetCurrentProcess();
	LPTRHANDLEPACKAGE lpPackage;
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
			
	lpPackage = malloc( sizeof(HANDLEPACKAGE) );  // when free
	if( lpPackage )
	{
		lpPackage->lpv = (HANDLE)getpid();
		lpPackage->nHandleType = PROCESS_HANDLE;
	}
	printf( "call leave %s, %d.\r\n", __FILE__, __LINE__ );
	return lpPackage;
}

DWORD WINAPI Process_GetId( HANDLE hProcess )
{
	//return GetProcessId();//KINGMOS_GWME_PROCESS_ID;
	LPTRHANDLEPACKAGE lpPackage = (LPTRHANDLEPACKAGE)hProcess;

	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	if( lpPackage && lpPackage->nHandleType == PROCESS_HANDLE )
	{
		printf( "call leave0 %s, %d.\r\n", __FILE__, __LINE__ );
		return lpPackage->lpv;
	}
	printf( "call leave1 %s, %d.\r\n", __FILE__, __LINE__ );
	return 0;
}

DWORD WINAPI Process_GetCurrentId( void )
{
	//GetCurrentProcessId();//KINGMOS_GWME_PROCESS_ID;
	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return (DWORD)getpid();
}

HANDLE WINAPI Process_Open( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
{
	//return NULL;//OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);//KINGMOS_GWME_PROCESS_HANDLE;
	LPTRHANDLEPACKAGE lpPackage;
			
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	lpPackage = malloc( sizeof(HANDLEPACKAGE) );	// when free 
	if( lpPackage )
	{
		lpPackage->lpv = (HANDLE)dwProcessId;
		lpPackage->nHandleType = PROCESS_HANDLE;
	}
	//printf( "call leave %s, %d.\r\n", __FILE__, __LINE__ );

	return lpPackage;
}

LPVOID WINAPI Process_MapPtr( LPVOID lpv, HANDLE hProcess )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	return lpv;
}

LPVOID WINAPI Process_UnMapPtr( LPVOID lpv )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return lpv;
}


//HANDLE WINAPI Process_GetHeap( void )
//{
//}