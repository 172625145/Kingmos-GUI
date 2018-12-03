//typedef void * HKEY;
#include <windows.h>

#include <cpu.h>



void OEM_GetRealTime( SYSTEMTIME FAR * lpst )
{
    GetLocalTime( lpst );
}

BOOL OEM_SetRealTime( const SYSTEMTIME FAR * lpst )
{
    return SetLocalTime( lpst );
}

extern VOID CPU_SetRTCTime( DWORD dwSeconds );
BOOL OEM_SetAlarmTime( const SYSTEMTIME FAR * lpst )
{
	SYSTEMTIME stCurTime;
	FILETIME ft;
	ULARGE_INTEGER u64set, u64cur;

	SystemTimeToFileTime( lpst, &ft );
	u64set.HighPart = ft.dwHighDateTime;
	u64set.LowPart = ft.dwLowDateTime;


	GetLocalTime( &stCurTime );
	SystemTimeToFileTime( &stCurTime, &ft );
	u64cur.HighPart = ft.dwHighDateTime;
	u64cur.LowPart = ft.dwLowDateTime;

	if( u64cur.QuadPart < u64set.QuadPart )
		CPU_SetRTCTime( (DWORD)(u64set.QuadPart - u64cur.QuadPart) / 10000000 );
	else
		CPU_SetRTCTime( 0 );

    return FALSE;
}

DWORD OEM_GetTickCount( void )
{
    return GetTickCount();
}

DWORD OEM_TimeToJiffies( DWORD dwMilliseconds, DWORD dwNanoseconds )
{
    return dwMilliseconds / RESCHED_PERIOD;
}
