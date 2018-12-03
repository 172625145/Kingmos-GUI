//typedef void * HKEY;
#include <windows.h>


//#define InterlockedIncrement  Interlock_Increment
LONG WINAPI KL_InterlockedIncrement( LPLONG lpDest )
{
	return InterlockedIncrement( lpDest );
}

//#define InterlockedDecrement  Interlock_Decrement
LONG WINAPI KL_InterlockedDecrement( LPLONG lpDest )
{
	return InterlockedDecrement( lpDest );
}

//#define InterlockedExchange   Interlock_Exchange
LONG WINAPI KL_InterlockedExchange( LPLONG lpDest, LONG lNewValue )
{
	return InterlockedExchange( lpDest, lNewValue );
}

//#define InterlockedTestExchange   Interlock_TestExchange
//LONG WINAPI InterlockedTestExchange( LPLONG lpDest, LONG lOldValue, LONG lNewValue )
//{
	//return InterlockedCompareExchange( lpDest, lNewValue, lOldValue );
//}

//#define InterlockedExchangeAdd    Interlock_ExchangeAdd
LONG WINAPI KL_InterlockedExchangeAdd( LPLONG lpDest, LONG lIncrement )
{
	return InterlockedExchangeAdd( lpDest, lIncrement );
}

//#define InterlockedCompareExchange Interlock_CompareExchange
LONG WINAPI KL_InterlockedCompareExchange( LPLONG lpDest, LONG lExchange, LONG lComperand )
{
	return (LONG)InterlockedCompareExchange( (LPVOID)lpDest, (LPVOID)lExchange, (LPVOID)lComperand );
}

