#include <ewindows.h>

BOOL WINAPI INTR_Init( UINT idInt, 
				       HANDLE hIntEvent,  // // intr event
				       LPVOID lpvData,   // transfer to OEM_InterruptEnable
				       DWORD dwSize      // transfer to OEM_InterruptEnable
				     )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	return TRUE;
}
void WINAPI INTR_Done( UINT idInt )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
}
