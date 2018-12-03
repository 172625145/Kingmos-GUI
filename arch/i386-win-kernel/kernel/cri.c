#include <windows.h>


VOID WINAPI CriticalSection_Initialize( LPCRITICAL_SECTION lpCriticalSection )
{
	InitializeCriticalSection( lpCriticalSection );
}

VOID WINAPI CriticalSection_Delete( LPCRITICAL_SECTION lpCriticalSection )
{
	DeleteCriticalSection( lpCriticalSection );
}

VOID WINAPI CriticalSection_Leave( LPCRITICAL_SECTION lpCriticalSection )
{
	LeaveCriticalSection( lpCriticalSection );
}

VOID WINAPI CriticalSection_Enter( LPCRITICAL_SECTION lpCriticalSection )
{
	EnterCriticalSection( lpCriticalSection );
}

BOOL WINAPI CriticalSection_TryEnter( LPCRITICAL_SECTION lpCriticalSection )
{
	return FALSE;//return TryEnterCriticalSection( lpCriticalSection );
}

