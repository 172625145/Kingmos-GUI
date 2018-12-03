#include <windows.h>

// folling function has bug, we just use it now only  alex!!!!....
LONG WINAPI Interlock_Increment( LPLONG lpDest )
{
	return InterlockedIncrement( lpDest );
}

LONG WINAPI Interlock_Decrement( LPLONG lpDest )
{
	return InterlockedDecrement( lpDest );
}

LONG WINAPI Interlock_Exchange( LPLONG lpDest, LONG lNewValue )
{
	return InterlockedExchange(lpDest, lNewValue);
}

LONG WINAPI Interlock_CompareExchange( LPLONG lpDest, LONG lExchange, LONG lComperand )
{
	return (LONG)InterlockedCompareExchange( (PVOID)lpDest, (PVOID)lExchange, (PVOID)lComperand);
}



