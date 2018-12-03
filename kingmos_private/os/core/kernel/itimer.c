#include <edef.h> 
#include <epcore.h>

void ItRealFunction( DWORD dwData )
{
	LPTHREAD lpThread = (LPTHREAD)dwData;
	DWORD dwInterval;

	dwInterval = lpThread->dwItRealIncr;
	if( dwInterval )
    {
		DWORD dwTimeout = dwJiffies + dwInterval;
		lpThread->realTimer.dwExpires = dwTimeout;
		_AddTimerList( &lpThread->realTimer );
	}
}
