/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����������
�汾�ţ�2.0.0
����ʱ�ڣ�2004-01-11
���ߣ�����
�޸ļ�¼��    
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
//������static int KL_CompareSystemTime( const struct _SYSTEMTIME * lpst1, 
//										const struct _SYSTEMTIME * lpst2 )
//������
//	IN lpst1 - _SYSTEMTIME �ṹָ��
//	IN lpst2 - _SYSTEMTIME �ṹָ��
//����ֵ��
//	1: lpst1 > lpst2; 0: lpst1 = lpst2; -1 lpst1 < lpst2
//����������
//	�Ƚ�ʱ��Ĵ�С
//����: 
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

//�ں� alarm �߳�
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
//������BOOL InitAlarmMgr( void )
//������
//	��
//����ֵ��
//	TRUR/FALSE
//����������
//	��ʼ��ϵͳ alarm ������
//����: 
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


//����ϵͳ alarmʱ��ʹ����¼�
//��� 
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
 
