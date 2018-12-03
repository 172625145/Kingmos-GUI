//#include <windows.h>
#include <edef.h>

// folling function has bug, we just use it now only  alex!!!!....
LONG WINAPI Interlock_Increment( LPLONG lpDest )
{
	//return InterlockedIncrement( lpDest );
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	*lpDest += 1;//++;// = *lpDest + 1;
	return *lpDest;
}

LONG WINAPI Interlock_Decrement( LPLONG lpDest )
{
	//return InterlockedDecrement( lpDest );
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	*lpDest -= 1;// = *lpDest -1 ;
	return *lpDest;
}

LONG WINAPI Interlock_Exchange( LPLONG lpDest, LONG lNewValue )
{
	//return InterlockedExchange(lpDest, lNewValue);
	LONG i;

	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	i = *lpDest;
	*lpDest = lNewValue;
	return i;
}

LONG WINAPI Interlock_CompareExchange( LPLONG lpDest, LONG lExchange, LONG lComperand )
{
	//return (LONG)InterlockedCompareExchange( (PVOID)lpDest, (PVOID)lExchange, (PVOID)lComperand);
	LONG i;

	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	i = *lpDest;
	if( *lpDest == lComperand){
		*lpDest = lExchange;
	}

	return i;
}



