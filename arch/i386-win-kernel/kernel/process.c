#include <windows.h>

//#define KINGMOS_GWME_PROCESS_ID     0xFF094530
//#define KINGMOS_GWME_PROCESS_HANDLE ((HANDLE)0xFF094531)

HANDLE WINAPI Process_GetCaller( void )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return NULL;//GetCurrentProcess();//KINGMOS_GWME_PROCESS_HANDLE;
}

HANDLE WINAPI Process_GetCurrent( void )
{
	return GetCurrentProcess();
}

DWORD WINAPI Process_GetId( HANDLE hProcess )
{
	return -1;//GetProcessId( hProcess );//KINGMOS_GWME_PROCESS_ID;
}

DWORD WINAPI Process_GetCurrentId( void )
{
	return GetCurrentProcessId();
}

HANDLE WINAPI Process_Open( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
{
	return OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
}

LPVOID WINAPI Process_MapPtr( LPVOID lpv, HANDLE hProcess )
{
	return lpv;
}

LPVOID WINAPI Process_UnMapPtr( LPVOID lpv )
{
	return lpv;
}

//HANDLE WINAPI Process_GetHeap( void )
//{
//}