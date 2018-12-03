/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵������Ϣ�ַ�����
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
	2004-12-08, _CallThreadWindowProc ���Ӷ� ���ڵ��ж�
    2003.07.07: LN in DispatchMessage, ȥ����PowerOff�Ĵ���
    2003.05.30: LN ���� WINS_INUSERPROC �� ����
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <ekeybrd.h>
#include <eassert.h>
#include <eapisrv.h>
#include <epwin.h>
#include <bheap.h>
#include <gwmeobj.h>

#include <winsrv.h>
#include <gdisrv.h>
#include <eobjcall.h>

static BOOL CALLBACK _PostAllTopLevel( HWND, LPARAM );
static BOOL TranslatePosMsg( LPMSG lpMsg );

static HWND GetChildWindow( HWND hWNd, POINT pt );
static BOOL HandlePosEvent( LPMSG lpMsg, BOOL fEnable );
LRESULT _CallWindowProc( _LPWINDATA lpws, WNDPROC lpCallProc, UINT message, WPARAM wParam, LPARAM lParam );

///////////////////////////////////////////////////////////
// private proc define end

typedef struct __THREADMSG
{
	MSG msg;
	LRESULT lResult;
	DWORD dwThreadIDOfSender;
	UINT uFlag;   // 0 , usable, 1 sended, 2 handled by the reveier
}THREADMSG, FAR * LPTHREADMSG;

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

static LPTHREADMSG _AllocThreadMsgData( DWORD * lpID )
{
	return (LPTHREADMSG)( *lpID = (DWORD)BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(THREADMSG) ) );
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

static LPTHREADMSG _GetThreadMsgPtr( DWORD dwID )
{
	return (LPTHREADMSG)dwID;
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

static void _ReleaseThreadMsgPtr( LPTHREADMSG lptm )
{
	BlockHeap_Free( hgwmeBlockHeap, 0, lptm, sizeof(THREADMSG) );
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

LRESULT WINAPI WinMsg_Dispatch( const MSG * lpMsg )
{
    ASSERT( lpMsg->hwnd );
    if( lpMsg->hwnd )
	{
	    _LPWINDATA lpws = _GetHWNDPtr( lpMsg->hwnd );
		if( lpws )
		{
            //MapPtrParam( message, &wParam, &lParam );
			if( lpMsg->message == WM_SYSTIMER && lpMsg->wParam == IDCARET )
			{
				extern void CALLBACK AutoCaretBlink( 
					HWND hwnd, 
					UINT uMsg,     // WM_SYSTIMER message
					UINT idEvent,  // timer identifier
					DWORD dwTime  );
				AutoCaretBlink( lpMsg->hwnd, WM_SYSTIMER, IDCARET, 0 );
				return 0;
			}
			else
				return _CallWindowProc( lpws, NULL, lpMsg->message, lpMsg->wParam, lpMsg->lParam );
		}
	}
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

BOOL WINAPI WinMsg_Get( MSG FAR* lpMsg, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax )
{
    BOOL retv = TRUE;

    while( 1 )
    {
        while( PeekMessage( lpMsg, hWnd, wMsgFiltenMin, wMsgFiltenMax, PM_REMOVE ) == FALSE )
        { 
            WinMsg_Wait();
        }
        if( lpMsg->message == WM_QUIT )
        {
            retv = FALSE;
        }
        break;
    }
    return retv;
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

BOOL WINAPI WinMsg_Peek( MSG FAR* lpMsgUser, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax, UINT wRemoveMsg )
{
    POINT pt;
    int retv;
	_MSGDATA msgData;
	LPMSG lpMsg = &msgData.msg;

	retv = FALSE;

_REPEAT_PEEK:    

    memset( &msgData, 0, sizeof( msgData ) );
    if( CheckThreadQueue( &msgData, hWnd, wMsgFiltenMin, wMsgFiltenMax, wRemoveMsg, 1 ) )
    {
        if( msgData.eventType & QS_KEY )
		{
			ASSERT( lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST );
            lpMsg->hwnd = GetFocus();
		}
        else if( msgData.eventType & QS_MOUSE )
        {
			ASSERT( lpMsg->message >= WM_MOUSEFIRST && lpMsg->message <= WM_MOUSELAST );
			pt = lpMsg->pt;
            if( GetCapture() == NULL )
            {
                int flag;
                if( ( flag = IsWindowEnabled( lpMsg->hwnd ) ) )
                {
                    HWND hs = lpMsg->hwnd;
                    lpMsg->hwnd = GetChildWindow( lpMsg->hwnd, pt );
                    if( lpMsg->hwnd == 0 )
                    {
                        if( wRemoveMsg == PM_NOREMOVE )
                            CheckThreadQueue( &msgData, hWnd, wMsgFiltenMin, wMsgFiltenMax, PM_REMOVE, 1 );
                        goto _REPEAT_PEEK;
                    }
                }
                if( HandlePosEvent( lpMsg, flag ) == FALSE )
                {  // the message has been handle myself, continue to peek next message
                    if( wRemoveMsg == PM_NOREMOVE )
                        CheckThreadQueue( &msgData, hWnd, wMsgFiltenMin, wMsgFiltenMax, PM_REMOVE, 1 );
                    goto _REPEAT_PEEK;
                }       
            }
            else  //
            {
				
				lpMsg->hwnd = GetCapture();
				// 2003-09-30-ADD-begin
				if( lpMsg->message == WM_LBUTTONDOWN || 
					lpMsg->message == WM_RBUTTONDOWN )
				{
					if( !IsForegroundThread()  )
					{
						Win_SetForeground( lpMsg->hwnd );
					}
				}
				// 2003-09-30-ADD-end
                
				Win_ScreenToClient( lpMsg->hwnd, &pt );
                lpMsg->lParam = MAKELONG( pt.x, pt.y );
            }
        }
		else if( lpMsg->message == WM_SYSTHREADMSG )
		{
			if( lpMsg->wParam == STM_SEND )
			{	//�������̷߳��͵����߳�
				LPTHREADMSG lptm = _GetThreadMsgPtr( lpMsg->lParam );
				
				if( lptm )
				{
					extern VOID WINAPI _WinSys_SetMessageEvent( DWORD dwThreadId );
				    lptm->lResult = WinMsg_Send( lptm->msg.hwnd, lptm->msg.message, lptm->msg.wParam, lptm->msg.lParam );
				    lptm->uFlag = 2;
					//֪ͨ�����߳�
					//_WinSys_PostThreadMessage( lptm->dwThreadIDOfSender, WM_SYSTHREADMSG, STM_REPLY, lpMsg->lParam );
					//2005-03-15, ���¼�����߳�
					//��������Ϣ����Ϊ�������̲߳����������Ϣ����
					_WinSys_SetMessageEvent(lptm->dwThreadIDOfSender);
					//
				}
			}
            if( wRemoveMsg == PM_NOREMOVE )
                 CheckThreadQueue( &msgData, hWnd, wMsgFiltenMin, wMsgFiltenMax, PM_REMOVE, 1 );

			goto _REPEAT_PEEK;
		}
		*lpMsgUser = *lpMsg;
        retv = TRUE;
    }
//PK_RET:
    return retv;
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

BOOL _Msg_PutSoftKeybrdEvent( BYTE bVirtualKey, WORD wCode, DWORD dwState )
{
    HWND hWnd;
    hWnd = GetFocus();
    if( hWnd )
    {
        if( bVirtualKey )
        {   // it is virtual key
            // put key down
			keybd_event( bVirtualKey, 0, 0,  0 );
        }
        if( wCode )
        {// put char message
			keybd_event( bVirtualKey, 0, KEYEVENTF_CHAR,  0 );			
        }
        if( bVirtualKey )
        {   // it is virtual key
            // put key down
			keybd_event( bVirtualKey, 0, KEYEVENTF_KEYUP,  0 );
        }
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

BOOL WINAPI WinMsg_Post(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    MSG msg;

    if( message == WM_PAINT || message == WM_NCPAINT )
        return FALSE;
    msg.message = message;
    msg.wParam = wParam;
    msg.lParam = lParam;
    if( hWnd == HWND_BROADCAST )
    {   // post message to all top level

		MSG msg;

		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;		
		
		DoEnumTopLevelWindow( _PostAllTopLevel, (LPARAM)&msg, NULL );
		return TRUE;
    }
    else
    {
        AddMsgToThreadQueue( hWnd, message, wParam, lParam, QS_POSTMESSAGE );
        return TRUE;
    }
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

static BOOL _CallThreadWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT uiTimeout, UINT fuFlags, LPDWORD lpdwRetv )
{
	DWORD dwMsgID;
    UINT uiTickCount;
	MSG msg;
	LPTHREADMSG lptm;
	BOOL lret = FALSE;

	lptm = _AllocThreadMsgData( &dwMsgID );
	if( lptm  )
	{
		UINT uErrorState = 0;  // û�д���
		DWORD dwWait;
		lptm->msg.hwnd = hWnd;
		lptm->msg.message = message;//WM_SYSTHREADMSG;
		lptm->msg.wParam = wParam;//dwMsgID;
		lptm->msg.lParam = lParam;
		lptm->dwThreadIDOfSender = GetCurrentThreadId();
		lptm->uFlag = 1;
		lptm->lResult = 0;

		AddMsgToThreadQueue( hWnd, WM_SYSTHREADMSG, STM_SEND, dwMsgID, QS_SENDMESSAGE );

		if( uiTimeout != INFINITE )
		    uiTickCount = GetTickCount();
        while( lptm->uFlag != 2 )
		{	//�ȴ���Ϣ
			dwWait = _WinSys_WaitMessage( uiTimeout );
			
			if( !IsWindow(hWnd) )
			{
				uErrorState = -1; //error
				break;
			}
			//
			if( dwWait == WAIT_OBJECT_0 )
			{   //����Ϣ
				if( lptm->uFlag == 2 )
				{   // �����꣬������Ч
					break;
				}
				else
				{   // �����Ϣ
					if( fuFlags == SMTO_NORMAL )
					{   // ����������Ϣ
						while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
						{
							TranslateMessage( &msg );
							DispatchMessage( &msg );
							if( lptm->uFlag == 2 )
								break;
						}
					}
				}
				if( lptm->uFlag != 2 )
				{
					if( uiTimeout != INFINITE )
					{   // ���� timeout ֵ
						DWORD uiCurTick = GetTickCount();
						DWORD dwDiff;
						if( uiCurTick >= uiTickCount )
							dwDiff = uiCurTick - uiTickCount;
						else
							dwDiff = uiCurTick + (~uiTickCount);  //����
						if( dwDiff >= uiTimeout )
						{	// timeout
							uErrorState = 1;
							break;
						}
						//�õ��µ� timeout
						uiTimeout -= dwDiff;
					}
				}
			}
			else if( dwWait == WAIT_TIMEOUT )
			{
				uErrorState = 1;
				break;  // timeout
			}
			else
			{
				uErrorState = -1;
				break;  // ����
			}
		}		

		if( uErrorState == 0 )
			lret = TRUE;	//�ɹ�
		else if( uErrorState == 1 )
		{	//timeout
			SetLastError( 0 );
		}

		if( lpdwRetv )
		{
			if( uErrorState == 0 )
				*lpdwRetv = lptm->lResult; //�ɹ�
			else
				*lpdwRetv  = 0;	//����
		}
		_ReleaseThreadMsgPtr( lptm );

	}
	return lret;
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

LRESULT _CallWindowProc( _LPWINDATA lpws, WNDPROC lpCallProc, UINT message, WPARAM wParam, LPARAM lParam )
{
	LRESULT lr; 
	BOOL bCallBack = FALSE;
	HANDLE hProcess;

#ifdef __DEBUG
#endif
	if( lpws->lpClass->hOwnerProcess != hgwmeProcess )
	{   //  ���Ǳ�ĳ����ע��
		bCallBack = TRUE;
		hProcess = lpws->lpClass->hOwnerProcess;
	}
	else if( lpCallProc )
	{   // ����ϵͳע���࣬ �������Ǳ��û����� CallWindowProc
		if( lpCallProc != lpws->lpClass->wc.lpfnWndProc )
		{
			bCallBack = TRUE;
		    hProcess = lpws->hOwnerProcess;
		}
		else
		{
			;
		}
	}
	else if( lpws->uState & WINS_INUSERPROC )
	{   // ����ϵͳע���࣬ �������Ǳ��û����� SetWindowLong ȥ���� Ϊ �û���WndProc
		bCallBack = TRUE;
		hProcess = lpws->hOwnerProcess;
		
	}	

	if( !lpCallProc )
		lpCallProc = lpws->lpfnWndProc;
    
	lpws->nSendCount++;
    if( bCallBack )
	{
		CALLBACKDATA cd;

		ASSERT( hProcess && lpCallProc );
		
		cd.hProcess = hProcess;
		cd.lpfn = (FARPROC)lpCallProc;
		cd.dwArg0 = (DWORD)lpws->hThis;

	    lr = Sys_ImplementCallBack4( &cd, message, wParam, lParam );
	}
	else
	{
		//ASSERT( lpCallProc );
	    lr = lpCallProc( lpws->hThis, message, wParam, lParam );
	}

	lpws->nSendCount--;

#ifdef __DEBUG
#endif

	return lr;
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

BOOL WINAPI WinMsg_SendNotify( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    _LPWINDATA lpws;

	if( hWnd == HWND_BROADCAST )
	{
		MSG msg;
		
		if( message != WM_PAINT )
		{
			msg.message = message;
			msg.wParam = wParam;
			msg.lParam = lParam;			
			
			DoEnumTopLevelWindow( _PostAllTopLevel, (LPARAM)&msg, NULL );
			return 1;
		}
		return 0;
	}

    lpws = _GetHWNDPtr( hWnd );
    if( lpws && message != WM_PAINT )
	{
		if( lpws->dwThreadID == GetCurrentThreadId() )
		{			
			return _CallWindowProc( lpws, NULL, message, wParam, lParam );            
		}
		else
		{
			return AddMsgToThreadQueue( hWnd, message, wParam, lParam, QS_SENDMESSAGE );
		}
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


BOOL WINAPI WinMsg_PostThread( DWORD dwThreadID, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return _WinSys_PostThreadMessage( dwThreadID, uMsg, wParam, lParam );
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

void WINAPI WinMsg_PostQuit( int nExitCode )
{
    _WinSys_PostThreadMessage( GetCurrentThreadId(), WM_QUIT, 0, 0 );
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

BOOL WINAPI WinMsg_Wait( void )
{
	if( WAIT_OBJECT_0 == _WinSys_WaitMessage(INFINITE) )
		return TRUE;
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

LRESULT WINAPI WinMsg_SendTimeout(
  HWND hWnd,            
  UINT message,
  WPARAM wParam,        
  LPARAM lParam,        
  UINT fuFlags,         // send options
  UINT uiTimeout,        // time-out duration
  LPDWORD lpdwResult // return value for synchronous call
)
{
	LRESULT lret = 0;
    _LPWINDATA lpws;
	MSG msg;

	if( hWnd == HWND_BROADCAST )
	{
		if( message != WM_PAINT )
		{
			msg.message = message;
			msg.wParam = wParam;
			msg.lParam = lParam;

			DoEnumTopLevelWindow( _PostAllTopLevel, (LPARAM)&msg, NULL );
			return 1;
		}
		return 0;
	}

    lpws = _GetHWNDPtr( hWnd );
    if( lpws && message != WM_PAINT )
	{
		DWORD dwThreadID;
		DWORD dwResult;

		dwThreadID = GetCurrentThreadId();

		if( dwThreadID == lpws->dwThreadID )
		{	
			dwResult = _CallWindowProc( lpws, NULL, message, wParam, lParam );//Wnd_SendMessage( hWnd, uiMsg, wParam, lParam );
			if( lpdwResult )
				*lpdwResult = dwResult;
			dwResult = TRUE;
		}
		else
		{
			dwResult = _CallThreadWindowProc( hWnd, message, wParam, lParam, uiTimeout, fuFlags, lpdwResult );
		}
		return dwResult;	
	}
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

LRESULT WINAPI WinMsg_Send( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    _LPWINDATA lpws;
	LRESULT lr = 0;

    if( message == WM_PAINT )
        return FALSE;

    //RETAILMSG( 1, ( "c1\r\n" ) );

    if( hWnd == HWND_BROADCAST )
    {   // send message to all top level
		MSG msg;

		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;		
		
		DoEnumTopLevelWindow( _PostAllTopLevel, (LPARAM)&msg, NULL );
		return 1;
    }

	//RETAILMSG( 1, ( "c2\r\n" ) );

    lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
		//RETAILMSG( 1, ( "c3\r\n" ) );
		if( lpws->dwThreadID == GetCurrentThreadId() )
		{
			//RETAILMSG( 1, ( "c4\r\n" ) );
			lr = _CallWindowProc( lpws, NULL, message, wParam, lParam );
			//RETAILMSG( 1, ( "c5\r\n" ) );
		}
		else 
		{
		    // different thread
			//RETAILMSG( 1, ( "c6\r\n" ) );
			WinMsg_SendTimeout( hWnd, message, wParam, lParam, SMTO_NORMAL, INFINITE, (LPDWORD)&lr );
		
		}
		return lr;
    }
    return lr;
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

static BOOL CALLBACK _PostAllTopLevel( HWND hWnd, LPARAM lParam )
{
    LPMSG p = (LPMSG)lParam;

	ASSERT( p->message != WM_PAINT );
	AddMsgToThreadQueue( hWnd, p->message, p->wParam, p->lParam, QS_POSTMESSAGE );
    return TRUE;
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

static BOOL HandlePosEvent( LPMSG lpMsg, BOOL fEnable )
{

    int hitValue, fActive;
    HWND hwndTopLevel;
    HWND hwnd;
    WPARAM wParam;
    POINT pt;
	DWORD dwExStyle = Win_GetLong( lpMsg->hwnd, GWL_EXSTYLE );
	

    lpMsg->lParam = MAKELONG( lpMsg->pt.x, lpMsg->pt.y );
    if( fEnable )
	{
        hitValue = (int)WinMsg_Send( lpMsg->hwnd, WM_NCHITTEST, 0, lpMsg->lParam );
		if( hitValue == HTERROR )
			return FALSE;
	}
    else
    {   // disable
		if( lpMsg->message == WM_LBUTTONDOWN || 
			lpMsg->message == WM_RBUTTONDOWN )
		{
			if( !IsForegroundThread()  )
			{
				Win_SetForeground( lpMsg->hwnd );
			}
		}
		
		return FALSE;
    }

    if( lpMsg->message == WM_LBUTTONDOWN &&
		(dwExStyle & WS_EX_INPUTWINDOW) == 0 )
    {
        hwndTopLevel = GetTopLevelParent( lpMsg->hwnd );

        if( (GetWindowLong( lpMsg->hwnd, GWL_STYLE ) & WS_CHILD) && (hwnd = GetParent( lpMsg->hwnd )) != 0 )
        {    // hwnd is child window
            wParam = Win_GetLong( lpMsg->hwnd, GWL_ID );
            // send notify message to parent
            WinMsg_Send( hwnd, WM_PARENTNOTIFY, MAKELONG( WM_LBUTTONDOWN, wParam ), lpMsg->lParam );
        }
        fActive = (int)WinMsg_Send( 
			                        lpMsg->hwnd,
			                        WM_MOUSEACTIVATE,
			                        (WPARAM)hwndTopLevel,
			                        (LPARAM)MAKELONG( hitValue, lpMsg->message ) );
		//2004-09-09, delete, ��ʱû��
        //WinMsg_Send( lpMsg->hwnd, WM_SETCURSOR, (WPARAM)lpMsg->hwnd, MAKELONG( hitValue, lpMsg->message ) );
		//

        if( fActive == MA_ACTIVATE || fActive == MA_ACTIVATEANDEAT )
        {   // active window if possible
			if( !IsForegroundThread() || Win_GetActive() != hwndTopLevel )
			{
                    Win_SetForeground( hwndTopLevel );
			}
        }
        // discard mouse message or not
        if( fActive == MA_NOACTIVATEANDEAT || fActive == MA_ACTIVATEANDEAT )
        {   // discard mouse message
            lpMsg->message = WM_NULL;
            return FALSE; // discard message
        }
    }

//HANDLE_HIT:
    if( hitValue != HTCLIENT )
    {   // translate to not client message
        lpMsg->message = lpMsg->message - WM_MOUSEFIRST + WM_NCMOUSEMOVE;
        lpMsg->wParam = hitValue;
    }
    else   // client mouse
    {
        pt = lpMsg->pt;
        Win_ScreenToClient( lpMsg->hwnd, &pt );
        lpMsg->lParam = MAKELONG( pt.x, pt.y );
    }
    return TRUE;
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

static HWND GetChildWindow( HWND hWnd, POINT pt )
{
    HWND hwndHas = 0;
    POINT ptc;
	RECT  rc;

    while( hWnd != hwndHas )
    {
        hwndHas = hWnd;
        ptc = pt;
        ScreenToClient( hWnd, &ptc );
	    GetClientRect( hWnd, &rc );
	    if( PtInRect( &rc, ptc ) )
		{
			hWnd = ChildWindowFromPointEx( hWnd, ptc.x, ptc.y, CWP_SKIPDISABLED|CWP_SKIPINVISIBLE );//Wnd_ChildFromPoint( hWnd, ptc, CWP_SKIPDISABLED|CWP_SKIPINVISIBLE );
			if( hWnd == 0 )
				return 0;
		}
		else
			break;
        //ASSERT( hWnd );
    }
    return hwndHas;
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

DWORD WINAPI WinMsg_WaitForMultipleObjects(
									   DWORD nCount,
									   LPHANDLE pHandles,
									   BOOL fWaitAll,
									   DWORD dwMilliseconds,
									   DWORD dwWakeMask
									   )
{
	UINT uiSize = (nCount + 1) * sizeof( LPHANDLE );
	LPHANDLE pMsgHandles = BlockHeap_AllocString( hgwmeBlockHeap, 0, uiSize );
	LPGWEDATA lpgwe = GetGWEPtr( GetCurrentThreadId() );
	DWORD dwRetv = WAIT_FAILED;

	if( pMsgHandles && lpgwe )
	{
		memcpy( pMsgHandles, pHandles, nCount * sizeof(LPHANDLE) );
		pMsgHandles[nCount] = lpgwe->msgQueue.hWaitEvent;

		while( 1 )
		{
			dwRetv = WaitForMultipleObjects( nCount + 1, pMsgHandles, fWaitAll, dwMilliseconds );
		    if( dwRetv == (WAIT_OBJECT_0 + nCount) )
			{				
				if(  lpgwe->msgQueue.dwEventMask & dwWakeMask )
					break;
			}
			else
				break;
		}
		BlockHeap_FreeString( hgwmeBlockHeap, 0, pMsgHandles );
	}
	return dwRetv;
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

BOOL WINAPI WinMsg_Beep( UINT uType )
{
	//return MessageBeep(uType);
	return 1;
}
