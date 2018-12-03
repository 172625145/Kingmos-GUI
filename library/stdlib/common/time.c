#include <eframe.h>
//#include <estring.h>
//#include <eassert.h>
#include <ettime.h>
//#include <oemfunc.h>

#define ORIGINYEAR  1980  //valid date: 1980.1---2116.1
#define JAN1WEEK    1   //1980.1.1/

static BOOL IsValidTime( const SYSTEMTIME *lpst ) 
{
	if( lpst->wDay >= 1 &&
		lpst->wDay <= 31 &&
		lpst->wHour >= 0 &&
		lpst->wHour <= 24 &&
		lpst->wMinute >= 0 &&
		lpst->wMinute <= 60 &&
		lpst->wMonth >= 1 &&
		lpst->wMonth <= 12 &&
		lpst->wYear >= ORIGINYEAR )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return TRUE;
	}
	return FALSE;
}

LONG WINAPI Time_CompareFile( const FILETIME * lpft1, const FILETIME * lpft2 )
{
    if( lpft1->dwLowDateTime < lpft2->dwLowDateTime )
        return -1;
    else if( lpft1->dwLowDateTime > lpft2->dwLowDateTime )
        return 1;
    else
    {
        if( lpft1->dwHighDateTime < lpft2->dwHighDateTime )
            return -1;
        else if( lpft1->dwHighDateTime > lpft2->dwHighDateTime )
            return 1;
        else
            return 0;
    }
}


static const BYTE bMonthTable[12]=    {31,28,31,30,31,30,31,31,30,31,30,31};
static const BYTE bLeapMonthTable[12]={31,29,31,30,31,30,31,31,30,31,30,31};

static void GetYMDHMS( LPSYSTEMTIME lpst, DWORD dwTotalSeconds )
{
	unsigned int    ms, sec, min, hour, day, month, year, leap;
	const BYTE * lpbMonth;
	
    ms = 0;
	sec = dwTotalSeconds % 60;
	min = dwTotalSeconds / 60;
    hour = min / 60;
    day =  hour / 24;

    lpst->wMilliseconds = ms;
    lpst->wSecond = sec;
    lpst->wMinute = min % 60;
    lpst->wHour = hour % 24;

    lpst->wDayOfWeek = (day + JAN1WEEK) % 7;

    year = 1980;//ORIGINYEAR;
    while (1) {
        leap = IsLeap( (WORD)year );
        if (day < 365+leap)
            break;
        day -= 365+leap;
        year++;
    }
    lpst->wYear = year;
	if( IsLeap( (WORD)year ) )
		lpbMonth = bLeapMonthTable;
	else
		lpbMonth = bMonthTable;

    for( month=0; month<12; month++) 
	{
		if( day < lpbMonth[month] )
            break;
		day -= lpbMonth[month];
    }

    lpst->wDay = day+1;
    lpst->wMonth = month+1;
}

static DWORD GetTotalSeconds( const SYSTEMTIME * lpst )
{
    unsigned int min, day, month, year;
    unsigned int cur_time;
	//int leap;
    const BYTE * lpbMonth;
	
    
	day = 0;
    for (year=ORIGINYEAR; year<lpst->wYear; year++) 
	{
        day += 365 + IsLeap( (WORD)year );
    }
    //leap = isleap(year) ? 1 : 0;

	if( IsLeap( (WORD)year ) )
		lpbMonth = bLeapMonthTable;
	else
		lpbMonth = bMonthTable;

    for (month=0; month<(unsigned int)(lpst->wMonth-1); month++) 
	{
		day += lpbMonth[month];
    }
    day += lpst->wDay - 1;
	
    min = (day * 24 + lpst->wHour) * 60 + lpst->wMinute;
	
	cur_time = min * 60 + lpst->wSecond;
	//v_pRTCReg->rcnr = cur_time;
	return cur_time;	
}

BOOL WINAPI Time_FileToSystem( const FILETIME *lpft, LPSYSTEMTIME lpst )
{
	GetYMDHMS( lpst, lpft->dwLowDateTime );
    return TRUE;
}

BOOL WINAPI Time_FileToLocalFile( const FILETIME *lpft, LPFILETIME lpftLocal )
{
    *lpftLocal = *lpft;
    return TRUE;
}

VOID WINAPI Time_GetLocal( LPSYSTEMTIME lpst )
{
	//OEM_GetRealTime( lpst );
	Sys_GetSystemTime( lpst );
}

BOOL WINAPI Time_SetLocal( const SYSTEMTIME *lpst )
{	
	//OEM_SetRealTime(lpst);
    if( IsValidTime( lpst ) )
	    return Sys_SetSystemTime( lpst );
	return FALSE;
}

BOOL WINAPI Time_LocalFileToFile( const FILETIME *lpftLocal, LPFILETIME lpft )
{
    *lpft  = *lpftLocal;
    return TRUE;
}

BOOL WINAPI Time_SystemToFile( const SYSTEMTIME *lpst, LPFILETIME lpft )
{
	// check param
	if( IsValidTime( lpst ) )
	{
        lpft->dwHighDateTime = lpft->dwLowDateTime = 0;
	    lpft->dwLowDateTime = GetTotalSeconds( lpst );
        return TRUE;
	}
	return FALSE;
}


/*
typedef struct _TIME_ZONE_INFORMATION {
    LONG lBias;
    TCHAR szStandardName[32];
    SYSTEMTIME stStandardDate;
    LONG lStandardBias;
    TCHAR szDaylightName[32];
    SYSTEMTIME stDaylightDate;
    LONG lDaylightBias;
} TIME_ZONE_INFORMATION, FAR * LPTIME_ZONE_INFORMATION;
*/
DWORD WINAPI Time_GetZoneInformation ( LPTIME_ZONE_INFORMATION lpTimeZoneInformation )
{
    memset( lpTimeZoneInformation, 0, sizeof( TIME_ZONE_INFORMATION ) );
    return TIME_ZONE_ID_STANDARD;

}

void WINAPI Time_SetDaylight( DWORD dst )
{
}

BOOL WINAPI Time_SetZoneInformation ( const TIME_ZONE_INFORMATION *lpTimeZoneInformation )
{
    return TRUE;
}
