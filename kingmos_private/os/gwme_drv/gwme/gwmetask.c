#include <eframe.h>
#include <epwin.h>


typedef struct _GWME_TASK_DATA
{
	struct _GWME_TASK_DATA * lpNext;
	DWORD dwThreadId;
	LPGWEDATA lpgweData;
}GWME_TASK_DATA, FAR * LPGWME_TASK_DATA;

static LPGWME_TASK_DATA lpTaskList;
static CRITICAL_SECTION  csTaskList;
//#define GetThreadGWEPtr( dwThreadId )  ( (LPGWEDATA)TlsGetThreadValue( (dwThreadId), TLS_MSGQUEUE ) )
//#define SetThreadGWEPtr( dwThreadId, lpgwe )  ( TlsSetThreadValue( (dwThreadId), TLS_MSGQUEUE, (lpgwe) ) )
BOOL _InitGwmeTaskList( void )
{
	lpTaskList = NULL;
	InitializeCriticalSection( &csTaskList );
	return TRUE;
}

VOID _DeinitTaskList( void )
{
	DeleteCriticalSection( &csTaskList );
}

VOID SetThreadGWEPtr( DWORD dwThreadId, LPGWEDATA lpgweData )
{
	LPGWME_TASK_DATA lpgwmeTask, lpgwmePrev;

	EnterCriticalSection( &csTaskList );
	lpgwmePrev = lpgwmeTask = lpTaskList;
	while( lpgwmeTask )
	{
		if( lpgwmeTask->dwThreadId == dwThreadId )
		{
			if( lpgweData )
			{
				lpgwmeTask->lpgweData = lpgweData;
				break;
			}
			else
			{  //free
				if( lpgwmePrev == lpgwmeTask )
				{ //at head
					lpTaskList = lpgwmeTask->lpNext;
				}
				else
				{
					lpgwmePrev->lpNext = lpgwmeTask->lpNext;
				}
				free( lpgwmeTask );
				goto _return;				
			}
		}
		lpgwmePrev = lpgwmeTask;
		lpgwmeTask = lpgwmeTask->lpNext;
	}

	if( !lpgwmeTask )
	{	//not find
		if( lpgweData )
		{	// need alloc a new
			lpgwmeTask = (LPGWME_TASK_DATA)malloc( sizeof(GWME_TASK_DATA) );
			if( lpgwmeTask )
			{
				memset( lpgwmeTask, 0, sizeof(GWME_TASK_DATA) );
				lpgwmeTask->lpNext = lpTaskList;			
				lpTaskList = lpgwmeTask;
				lpgwmeTask->lpgweData = lpgweData;
				lpgwmeTask->dwThreadId = dwThreadId;
			}
		}
	}

_return:
	LeaveCriticalSection( &csTaskList );
}

LPGWEDATA GetThreadGWEPtr( DWORD dwThreadId )
{
	LPGWME_TASK_DATA lpgwmeTask;

	EnterCriticalSection( &csTaskList );
	lpgwmeTask = lpTaskList;
	while( lpgwmeTask )
	{
		if( lpgwmeTask->dwThreadId == dwThreadId )
			break;
		lpgwmeTask = lpgwmeTask->lpNext;
	}
	LeaveCriticalSection( &csTaskList );
	if( lpgwmeTask )
		return lpgwmeTask->lpgweData;
	return NULL;
}
