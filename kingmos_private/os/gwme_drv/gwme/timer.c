/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����gwme ʱ�ӹ���
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
	2004-07-02:�������Ϊ��̬���䣬��̬������ļ�Ϊ timer_static_alloc.c
    2003-04-14:���ӳ�ͻ�μ��
******************************************************/

#include <eframe.h>
#include <eevent.h>
#include <epwin.h>
#include <bheap.h>
#include <gwmeobj.h>

static int _TimerInstall(void);
static int _TimerRelease(void);
static int _TimerWho(void);
static int _TimerPeek( LPMSG, UINT );
static int _TimerPut( LPMSG );
static int _TimerRemove(void);
static UINT _TimerCount(void);

_EVENTDRV _timerEvent = {
    _TimerInstall,
    _TimerRelease,
    _TimerWho,
    _TimerPeek,
    _TimerPut,
    _TimerRemove,
    _TimerCount
};

// define timer driver

#define TEF_SIGNAL   0x1
typedef struct _TIMEREVENT
{
	struct _TIMEREVENT * lpNext;
	struct _TIMEREVENT * lpPrev;
	HANDLE hOwnerProcess;
    HWND hWnd;
    UINT msg;
    UINT uID;
    UINT uElaspe;
	UINT flag;//Used; // TEF_SIGNAL
    DWORD oldTicks;
    TIMERPROC lpTimerProc;
}TIMEREVENT;


static TIMEREVENT * lpTimerServer = NULL;
static CRITICAL_SECTION csTimerQueue;

// **************************************************
// ������static VOID AddTimer( TIMEREVENT * lpTimer )
// ������
// 	IN/OUT lpTimer - TIMEREVENT �ṹָ��
// ����ֵ��
//	��
// ����������
//	�� timer �������
// ����: 
//	
// ************************************************

static VOID AddTimer( TIMEREVENT * lpTimer )
{
	ASSERT( lpTimer->lpNext == NULL );
	if( lpTimerServer )
	{	// ���ӵ�����
		lpTimer->lpNext = lpTimerServer;//->lpNext;
		lpTimer->lpPrev = lpTimerServer->lpPrev;
		lpTimerServer->lpPrev->lpNext = lpTimer;
		lpTimerServer->lpPrev = lpTimer;

	}
	else
	{
		lpTimer->lpNext = lpTimer;
		lpTimer->lpPrev = lpTimer;
	}
	lpTimerServer = lpTimer;
}

// **************************************************
// ������static VOID RemoveTimer( TIMEREVENT * lpTimer )
// ������
// 	IN/OUT lpTimer - TIMEREVENT �ṹָ��
// ����ֵ��
//	��
// ����������
//	�� timer �Ƴ�����
// ����: 
//	
// ************************************************

static VOID RemoveTimer( TIMEREVENT * lpTimer )
{
	ASSERT( lpTimerServer && lpTimer && lpTimer->lpNext );
	if( lpTimer->lpNext == lpTimer )
	{	//����һ��
		lpTimerServer = NULL;
		lpTimer->lpNext = NULL;
	}
	else
	{	//���,�Ƴ�����
		lpTimer->lpNext->lpPrev = lpTimer->lpPrev;
		lpTimer->lpPrev->lpNext = lpTimer->lpNext;
		if( lpTimerServer == lpTimer )	//�ڶ���ͷ�� ��
			lpTimerServer = lpTimer->lpNext;	//��
	}
}


// **************************************************
// ������static TIMEREVENT * _GetWinTimer( void )
// ������
// 	��
// ����ֵ��
//	����ɹ������� TIMEREVENT �ṹָ�룻���򣬷��� NULL
// ����������
//	�õ�һ�����ڵ� timer
// ����: 
//	
// ************************************************

static TIMEREVENT * _GetWinTimer( BOOL bRemove )
{
	TIMEREVENT * lpTimer, * lpGetTimer = NULL;

	EnterCriticalSection( &csTimerQueue );

	if( (lpTimer = lpTimerServer) )
	{
		do{
			if( lpTimer->flag & TEF_SIGNAL )
			{
				if( bRemove )
				{
					lpTimer->flag &= ~TEF_SIGNAL;
					lpTimer->oldTicks = GetTickCount();
				}
				lpGetTimer = lpTimer;
				lpTimerServer = lpTimer->lpNext;	//�Ժ����һ�ο�ʼ���
				break;
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	
	LeaveCriticalSection( &csTimerQueue );
	return lpGetTimer;

}

// **************************************************
// ������BOOL CheckWinTimerEvent( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����Ƿ��� timer �¼�����
// ����: 
//	
// ************************************************

BOOL CheckWinTimerEvent( void )
{
	BOOL bRetv = FALSE;
    DWORD diff;
    DWORD ticks;
	TIMEREVENT * lpTimer;

	EnterCriticalSection( &csTimerQueue );

	lpTimer = lpTimerServer;

	
	if( lpTimer )
	{
		ticks = GetTickCount();
		do
		{
			ASSERT( lpTimer->hWnd || lpTimer->lpTimerProc );
			//�Ƿ������źű�־
			if( (lpTimer->flag & TEF_SIGNAL) == 0 )
			{	//û�У������
				if( ticks > lpTimer->oldTicks )
					diff = ( ticks - lpTimer->oldTicks );
				else
					diff = ticks + ( ~lpTimer->oldTicks );
				if( diff >= lpTimer->uElaspe )
				{
					lpTimerServer = lpTimer;
					lpTimer->flag |= TEF_SIGNAL;					
					bRetv = TRUE; // ���ź�
					break;
				}
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}

	LeaveCriticalSection( &csTimerQueue );

	if( bRetv )
		SetSYSQueueState( ES_TIMER, TRUE );
	return bRetv;
}

// **************************************************
// ������static UINT _TimerCount(void)
// ������
// 	��
// ����ֵ��
//	���� ��ǰϵͳ�� timer ��
// ����������
//	�õ���ǰϵͳ�����õ� timer ��
// ����: 
//	
// ************************************************

static UINT _TimerCount(void)
{
    int n  = 0;
	TIMEREVENT * lpTimer;
	
	EnterCriticalSection( &csTimerQueue );	

	if( (lpTimer = lpTimerServer) )
	{
		do
		{
			n++;
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	LeaveCriticalSection( &csTimerQueue );
    return n;
}

// **************************************************
// ������static int _TimerInstall()
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ�� timer ������
// ����: 
//	
// ************************************************

static int _TimerInstall()
{
	InitializeCriticalSection( &csTimerQueue );
	lpTimerServer = NULL;
    return 1;
}

// **************************************************
// ������static int _TimerRelease()
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�� _TimerInstall �෴�� �ͷ�timer ������
// ����: 
//	
// ************************************************

static int _TimerRelease()
{
	DeleteCriticalSection( &csTimerQueue );
    return 1;
}

// **************************************************
// ������static int _TimerWho()
// ������
// 	��
// ����ֵ��
//	���� ID_TIMER
// ����������
//	�õ� ���¼���������ID
// ����: 
//	
// ************************************************

static int _TimerWho()
{
    return ID_TIMER;
}

// **************************************************
// ������static int _TimerPeek( LPMSG lpEvent, UINT fRemove )
// ������
// 	OUT lpEvent - ���ڽ������źŵ��¼�
//	IN fRemove - �Ƿ��Ƴ��ź�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�� timer ����������һ�����źŵ��¼�
// ����: 
//	
// ************************************************

static int _TimerPeek( LPMSG lpEvent, UINT fRemove )
{
	int iRetv = 0;
	TIMEREVENT * lpTimer;

	EnterCriticalSection( &csTimerQueue );

_repeat:
	lpTimer = _GetWinTimer(fRemove);
	if( lpTimer )
	{		
		if( lpTimer->hWnd )
		{
			if( !IsWindow( lpTimer->hWnd ) )
			{
				WARNMSG( 1, ("Invalid window handle:%x in timer server\r\n", lpTimer->hWnd ) );
				RemoveTimer( lpTimer );
				BlockHeap_Free( hgwmeBlockHeap, 0, lpTimer, sizeof(TIMEREVENT) );
				goto _repeat;
			}
		}
		lpEvent->message = lpTimer->msg;
		lpEvent->hwnd = lpTimer->hWnd;
		lpEvent->wParam = lpTimer->uID;
		lpEvent->lParam = (LPARAM)lpTimer->lpTimerProc;
		iRetv = 1;
	}

	LeaveCriticalSection( &csTimerQueue );
	return iRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static int _TimerPut( LPMSG lpUnused )
{
    return 0;
}

// **************************************************
// ������static int _TimerRemove()
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����ǰ���źŵ� timer ȥ���źţ��ٽ������Ƶ���һ��
// ����: 
//	
// ************************************************

static int _TimerRemove()
{
	EnterCriticalSection( &csTimerQueue );

	if( lpTimerServer )
	{
		lpTimerServer->flag &= ~TEF_SIGNAL;
		lpTimerServer = lpTimerServer->lpNext;		
	}
	LeaveCriticalSection( &csTimerQueue );

    return 1;
}

// **************************************************
// ������UINT FASTCALL _SetTimer( HWND hWnd, UINT msg, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
// ������
// 	IN hWnd - ���ھ��
//	IN msg - ��Ҫ���͵���Ϣ
//	IN id - timer ID
//	IN uElapse - ����ʱ��
//	IN lpTimerProc - timer ������������Ϊ NULL
// ����ֵ��
//	����ɹ������� ID, ʧ�ܣ����� 0
// ����������
//	�����µ� timer
// ����: 
//	
// ************************************************

UINT FASTCALL _SetTimer( HANDLE hOwnerProcess, HWND hWnd, UINT msg, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
{
	UINT uiRetv = 0;
    TIMEREVENT * lpTimer, * lpTimerUse = NULL;
	

    ASSERT( hWnd || lpTimerProc );
	if( uElapse == 0 )
		uElapse = 1;
	// EnterCri
	EnterCriticalSection( &csTimerQueue );
	//����Ƿ����и� timer
	lpTimer = lpTimerServer;
	if( lpTimer )
	{
		do{
			if( lpTimer->uID == id && lpTimer->hWnd == hWnd )
			{
				lpTimerUse = lpTimer;
				break;
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	if( lpTimerUse == NULL )
	{
		lpTimerUse = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(TIMEREVENT) );
		if( lpTimerUse )
			memset( lpTimerUse, 0, sizeof(TIMEREVENT) );
	}
	if( lpTimerUse )
	{
		lpTimerUse->hWnd = hWnd;
		lpTimerUse->msg = msg;
		lpTimerUse->uElaspe = uElapse;
		lpTimerUse->lpTimerProc = lpTimerProc;
		lpTimerUse->hOwnerProcess = hOwnerProcess;
		if( hWnd ) // the  id is valid
			lpTimerUse->uID = id;
		else
		{
			id = lpTimerUse->uID = (UINT)lpTimerUse;
		}		
		lpTimerUse->oldTicks = GetTickCount();
		uiRetv = lpTimerUse->uID;
		//timer ���·������ ��
		if( lpTimerUse->lpNext == NULL )
		{	//�ǣ�����
			AddTimer( lpTimerUse );
		}
	}

	LeaveCriticalSection( &csTimerQueue );
	return uiRetv;
}

// **************************************************
// ������UINT FASTCALL _KillTimer( HWND hWnd, UINT msg, UINT id )
// ������
// 	IN hWnd - ���ھ��
//	IN msg - ��Ҫ���͵���Ϣ
//	IN id - timer ID
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͷ� timer
// ����: 
//	
// ************************************************

UINT FASTCALL _KillTimer( HWND hWnd, UINT msg, UINT id )
{
	UINT uiRetv = FALSE;
    TIMEREVENT * lpTimer;

	EnterCriticalSection( &csTimerQueue );
	lpTimer = lpTimerServer;

	if( lpTimer )
	{
		do{
			if( 
				lpTimer->hWnd == hWnd &&
				lpTimer->msg == msg &&
				lpTimer->uID == id )
			{
				RemoveTimer( lpTimer );
				BlockHeap_Free( hgwmeBlockHeap, 0, lpTimer, sizeof(TIMEREVENT) );
				if( hWnd )
					ClearThreadQueue( 0, hWnd, msg, msg );
				uiRetv = TRUE;
				break;
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	LeaveCriticalSection( &csTimerQueue );
	
    return uiRetv;
}


// **************************************************
// ������VOID FASTCALL _ClearProcessTimer( HANDLE hProcess )
// ������
// 	IN hProcess - ���̾��
// ����ֵ��
//	��
// ����������
//	��һ�������˳�ʱ���ͷŸý��̵� timer��Դ
// ����: 
//	
// ************************************************

VOID FASTCALL _ClearProcessTimer( HANDLE hProcess )
{
    TIMEREVENT * lpTimer;

	//�����ͻ��
	EnterCriticalSection( &csTimerQueue );

	lpTimer = lpTimerServer;

	if( lpTimer )
	{
		do{
			if( lpTimer->hOwnerProcess == hProcess )
			{
				TIMEREVENT * lpSave = lpTimer->lpNext;
				//�Ƴ����ͷ�
				RemoveTimer( lpTimer );
				BlockHeap_Free( hgwmeBlockHeap, 0, lpTimer, sizeof(TIMEREVENT) );
				if( lpTimerServer == NULL )
					break;	//û���κ� timer ��
				lpTimer = lpSave;
				continue;
			}
			lpTimer = lpTimer->lpNext;
		}while( lpTimer != lpTimerServer );
	}
	LeaveCriticalSection( &csTimerQueue );	
}
