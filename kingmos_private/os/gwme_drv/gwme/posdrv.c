/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����������豸��Ϣ
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
    2003-04-14-���ӳ�ͻ�μ��
******************************************************/

#include <eframe.h>
#include "epos.h"
#include <eevent.h>
#include <epwin.h>
#include <gwmeobj.h>
#include <esysset.h>
#ifdef EML_DOS
#include <dos.h>
#endif

void TouchPanelCalibrateAPoint(
    int   xUnCalibrate,     //ԭʼ��x����
    int   yUnCalibrate,     //ԭʼ��y����
    int   *pCalX,     //ת�����x����
    int   *pCalY      //ת�����y����
    );

static int _GetPosEvent( POSRECORD FAR * lpRecord, UINT fRemove );
static int _PosPeek( LPMSG, UINT fRemove );
static int _PosPut( LPMSG );
static int _PosRemove(void);
static int _PosInstall(void);
static int _PosRelease(void);
static int _PosWho(void);
static UINT _PosCount(void);
static BOOL CALLBACK DoPosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo );

_EVENTDRV _posEvent = {
    _PosInstall,
    _PosRelease,
    _PosWho,
    _PosPeek,
    _PosPut,
    _PosRemove,
    _PosCount
};

#undef  MAX_QUEUE
#define MAX_QUEUE 16
static int _queueHead = 0;
static int _queueTail = 0;
static int _queueCount = 0;
static POSRECORD _curPos;
static POSRECORD _lastPos;
static POSRECORD _posQueue[MAX_QUEUE];
static CRITICAL_SECTION csPosQueue;
static LPPOS_CALLBACK lpPosCallBackFun = NULL;//��λ�豸�ص�����

BOOL bTestQueue = 0;

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

static BOOL __PutPointerEvent( LPPOSRECORD lpRecord )
{
	BOOL bRetv = FALSE;
	if( lpRecord->x == _curPos.x &&
		lpRecord->y == _curPos.y &&
		lpRecord->wState == _curPos.wState )
		return FALSE;

    EnterCriticalSection( &csPosQueue );

    _curPos = *lpRecord;
 
    if( _queueCount < MAX_QUEUE )
    {
        _posQueue[_queueTail] = _curPos;
        if( ++_queueTail>= MAX_QUEUE )
            _queueTail = 0;
        _queueCount++;
		bRetv = TRUE;
    }
	else
	{
		RETAILMSG( 1, ( "mouse queue is full, _queueCount=%d.\r\n", _queueCount ) );

		// TEST code
		{
			extern void LockCSWindow( void );
			extern void UnlockCSWindow( void );
			
			DWORD dwForeThreadID;
			bTestQueue = TRUE;
			dwForeThreadID = LockForegroundThreadId(); //LN, 2003-06-03, ADD
			UnlockForegroundThreadId();
			RETAILMSG( 1, ( "UnlockForegroundThreadId.\r\n" ) );
			LockCSWindow();
			UnlockCSWindow();			
			RETAILMSG( 1, ( "UnlockCSWindow.\r\n" ) );
			{
				HRGN hClip;
			__LockRepaintRgn( &hClip );
			__UnlockRepaintRgn();
			RETAILMSG( 1, ( "UnlockRepaintRgn.\r\n" ) );
			}
			
		}
		//

	}

	LeaveCriticalSection( &csPosQueue );

	SetSYSQueueState( ES_MOUSE, TRUE );

    return bRetv;
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

int _GetPosEvent( POSRECORD FAR * lpRecord, UINT fRemove )
{
	EnterCriticalSection( &csPosQueue );


    if( _queueCount == 0 )
    {
        *lpRecord = _curPos;
    }
    else
    {
        *lpRecord = _posQueue[_queueHead];
        if( fRemove == PM_REMOVE )
        {
            if( ++_queueHead >= MAX_QUEUE )
                _queueHead = 0;
            _queueCount--;
        }
    }
	LeaveCriticalSection( &csPosQueue );
    return 0;
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

static UINT _PosCount(void)
{
    return _queueCount;
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

static int _PosPeek( LPMSG lpEventData, UINT fRemove )
{
    POSRECORD _PosRecord;

	// TEST
	if( bTestQueue )
	{
		RETAILMSG( 1, ( "_PosQueueCount = %d, fRemove=%d.\r\n", _queueCount, fRemove ) );
		bTestQueue = FALSE;
	}
	//

    _GetPosEvent( &_PosRecord, fRemove );

    if( _PosRecord.wState == 0 && _lastPos.wState != 0 )
    {
        lpEventData->message = WM_LBUTTONUP;
        lpEventData->wParam = _PosRecord.wState;
		lpEventData->pt.x = _PosRecord.x;
		lpEventData->pt.y = _PosRecord.y;

    }
    else if( _PosRecord.wState != 0 && _lastPos.wState == 0 )
    {
        lpEventData->message = WM_LBUTTONDOWN;
        lpEventData->wParam = _PosRecord.wState;
		lpEventData->pt.x = _PosRecord.x;
		lpEventData->pt.y = _PosRecord.y;
    }
    else if( _PosRecord.x != _lastPos.x ||
            _PosRecord.y != _lastPos.y )
    {
        lpEventData->message = WM_MOUSEMOVE;
        lpEventData->wParam = _PosRecord.wState;
		lpEventData->pt.x = _PosRecord.x;
		lpEventData->pt.y = _PosRecord.y;

    }
    else
    {
        if( _queueCount > 1 )
             _GetPosEvent( &_PosRecord, PM_REMOVE );
        return 0;
    }
    if( fRemove == PM_REMOVE )
        _lastPos = _PosRecord;
    return 1;
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

static int _PosInstall()
{
	InitializeCriticalSection( &csPosQueue );

    return 1;
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

static int _PosRelease()
{
	DeleteCriticalSection( &csPosQueue );
    return 1;
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

static int _PosWho()
{
    return ID_POINTER;
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

static int _PosPut( LPMSG lp )
{
    return 0;
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

static int _PosRemove()
{
    return 0;
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

static BOOL CALLBACK DoPosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo )
{
	POSRECORD pos;

	if( dwFlags & MOUSEEVENTF_ABSOLUTE )
	{
	    pos.x = (short)dx;
	    pos.y = (short)dy;
		if( dwFlags & MOUSEEVENTF_LEFTDOWN )
		{
			pos.wState = PS_LBUTTON;
		}
		else if( dwFlags & MOUSEEVENTF_LEFTUP )
			pos.wState = 0;
		else
			pos.wState = 0;
		__PutPointerEvent( &pos );
		return TRUE;
	}
	return FALSE;
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

BOOL CALLBACK HandlePosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo )
{
	int xCal, yCal;

	TouchPanelCalibrateAPoint( dx, dy, &xCal, &yCal );  
	//	RETAILMSG( 1, ( "origin x(%d),origin y(%d), xCal(%d)/4=(%d), yCal(%d)/4=(%d).\r\n", dx, dy, xCal, xCal/4, yCal, yCal/4 ) );
	xCal /= 4; yCal /= 4;
	if( lpPosCallBackFun )	// ���ûص�����
		return lpPosCallBackFun( dwFlags, xCal, yCal, dwData, dwExtraInfo );
	else  // ����Ĭ�ϵ�
	    return DoPosEvent( dwFlags, xCal, yCal, dwData, dwExtraInfo );
}

// **************************************************
// ������LPPOS_CALLBACK _SetPosEventCallBack( LPPOS_CALLBACK lpNewCallBackFun )
// ������
// 	IN lpNewCallBackFun - ��λ�豸�ص�����
// ����ֵ��
//	����ɹ�������֮ǰ�õĻص����������򣬷��� NULL
// ����������
//	�����µĶ�λ�豸�ص�����
// ����: 
//	
// ************************************************

LPPOS_CALLBACK _SetPosEventCallBack( LPPOS_CALLBACK lpNewCallBackFun )
{
	LPPOS_CALLBACK lpOldFun = lpPosCallBackFun;
	if( lpNewCallBackFun )
	{
		lpPosCallBackFun = lpNewCallBackFun;
		if( lpOldFun )
		    return lpOldFun;
		else
			return (LPPOS_CALLBACK)DoPosEvent;  //����ϵͳĬ�ϵ�
	}
	else
		return NULL;	
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

VOID WINAPI WinEvent_MouseEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo )
{
	DoPosEvent(dwFlags,  dx,  dy,  dwData,  dwExtraInfo);
}
