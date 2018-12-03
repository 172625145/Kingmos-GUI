/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：响铃管理
版本号：2.0.0
开发时期：2004-01-11
作者：李林
修改记录：    
******************************************************/

#include <eframe.h>
#include <eapisrv.h>
#include <epcore.h>
#include <coresrv.h>
#include <oemfunc.h>
#include <sysintr.h>

static DWORD WINAPI AlarmThreadProc( LPVOID lParam );
static HANDLE hIntrAlarmEvent;
static HANDLE hUserAlarmEvent;
static SYSTEMTIME stCurAlarmTime;
static CRITICAL_SECTION csAlarm;

// **************************************************
//声明：static int KL_CompareSystemTime( const struct _SYSTEMTIME * lpst1, 
//										const struct _SYSTEMTIME * lpst2 )
//参数：
//	IN lpst1 - _SYSTEMTIME 结构指针
//	IN lpst2 - _SYSTEMTIME 结构指针
//返回值：
//	1: lpst1 > lpst2; 0: lpst1 = lpst2; -1 lpst1 < lpst2
//功能描述：
//	比较时间的大小
//引用: 
// ************************************************

static int KL_CompareSystemTime( const struct _SYSTEMTIME * lpst1, const struct _SYSTEMTIME * lpst2 )
{
	if( lpst1->wYear > lpst2->wYear )
		return 1;
	else if( lpst1->wYear < lpst2->wYear )
		return -1;

	if( lpst1->wMonth > lpst2->wMonth )
		return 1;
	else if( lpst1->wMonth < lpst2->wMonth )
		return -1;

	if( lpst1->wDay > lpst2->wDay )
		return 1;
	else if( lpst1->wDay < lpst2->wDay )
		return -1;

	if( lpst1->wHour > lpst2->wHour )
		return 1;
	else if( lpst1->wHour < lpst2->wHour )
		return -1;

	if( lpst1->wMinute > lpst2->wMinute )
		return 1;
	else if( lpst1->wMinute < lpst2->wMinute )
		return -1;

	if( lpst1->wSecond > lpst2->wSecond )
		return 1;
	else if( lpst1->wSecond < lpst2->wSecond )
		return -1;

	if( lpst1->wMilliseconds > lpst2->wMilliseconds )
		return 1;
	else if( lpst1->wMilliseconds < lpst2->wMilliseconds )
		return -1;

	return 0;
}

//内核 alarm 线程
static DWORD WINAPI AlarmThreadProc( LPVOID lParam )
{
	while(1)
	{
		SYSTEMTIME st;
		KL_WaitForSingleObject( hIntrAlarmEvent, INFINITE );
		
		if (hUserAlarmEvent) {
			OEM_GetRealTime(&st);
			//KL_SystemTimeToFileTime(&st,&ft);
			//KL_SystemTimeToFileTime(&CurAlarmTime,&ft2);
			//if (dwNKAlarmResolutionMSec < MIN_NKALARMRESOLUTION_MSEC)
				//dwNKAlarmResolutionMSec = MIN_NKALARMRESOLUTION_MSEC;
			//else if (dwNKAlarmResolutionMSec > MAX_NKALARMRESOLUTION_MSEC)
				//dwNKAlarmResolutionMSec = MAX_NKALARMRESOLUTION_MSEC;
			//ft3.dwLowDateTime = dwNKAlarmResolutionMSec * 10000;        // convert to 100 ns units
			//ft3.dwHighDateTime = 0;
			//sub64_64_64(&ft2,&ft3,&ft3);
			KL_EnterCriticalSection( &csAlarm );
			if ( KL_CompareSystemTime( &st,&stCurAlarmTime ) >= 0 ) 
			{
				HANDLE h = hUserAlarmEvent;
				hUserAlarmEvent = NULL;
				KL_SetEvent(h);
			} 
			else
			{
				OEM_SetAlarmTime( &stCurAlarmTime );
			}
			KL_LeaveCriticalSection( &csAlarm );
		}
	}
}

// **************************************************
//声明：BOOL InitAlarmMgr( void )
//参数：
//	无
//返回值：
//	TRUR/FALSE
//功能描述：
//	初始化系统 alarm 管理器
//引用: 
// ************************************************

BOOL InitAlarmMgr( void )
{
	hIntrAlarmEvent = KL_CreateEvent( NULL, FALSE, 0, NULL );
	memset( &stCurAlarmTime, 0xff, sizeof(stCurAlarmTime) );

	KC_InitializeCriticalSection( &csAlarm );
	KL_CreateThread( NULL, 0, AlarmThreadProc, 0, 0, NULL );
	KL_IntrInit( SYSINTR_RTC_ALARM, hIntrAlarmEvent, NULL, 0 );
	
	return TRUE;
}


//设置系统 alarm时间和触发事件
//如果 
//    HANDLE hAlarm,
    //LPSYSTEMTIME lpst


BOOL WINAPI KL_SetAlarmInfo( LPALARM_INFO lpai ) //HANDLE hAlarmEvent, LPSYSTEMTIME lpstAlarm ) 
{
    if( lpai && lpai->uiSize == sizeof(ALARM_INFO) )
	{	
		KL_EnterCriticalSection( &csAlarm );
		
		hUserAlarmEvent = lpai->hAlarmEvent;
		stCurAlarmTime = lpai->stAlarmTime;
		KL_SetEvent( hIntrAlarmEvent );

		KL_LeaveCriticalSection( &csAlarm );		
	}
	else if( lpai == NULL )
	{	//clear
		KL_EnterCriticalSection( &csAlarm );

		hUserAlarmEvent = NULL;
		memset( &stCurAlarmTime, 0xffffffff, sizeof(stCurAlarmTime) );
		KL_SetEvent( hIntrAlarmEvent );

		KL_LeaveCriticalSection( &csAlarm );
	}
	else
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void WINAPI KL_RefreshAlarm( DWORD dwFlag ) 
{
    KL_SetEvent( hIntrAlarmEvent );
}
 
