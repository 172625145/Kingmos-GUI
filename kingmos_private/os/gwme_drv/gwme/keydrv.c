/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵���������¼�����
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
    2003-04-14:���ӳ�ͻ�μ��
    2003-04-11:���̶�����������ȴ�ò���
******************************************************/

#include <eframe.h>
#include "ekeydrv.h"

#include <eevent.h>
//#include <epalloc.h>
#include <epwin.h>

#undef  MAX_QUEUE
#define MAX_QUEUE 32
#define KEY_CAPITAL 0x00000001
#define KEY_SHIFT   0x00000002
#define KEY_CTRL    0x00000004
#define KEY_ALT     0x00000008

static WORD wQueueHead = 0;
static WORD wQueueTail = 0;
static WORD wQueueCount = 0;
static KEYRECORD keyQueue[MAX_QUEUE];
static LPKEYRECORD lpPrev;
static DWORD dwKeyboardState = 0;
static CRITICAL_SECTION csKeyQueue;

static int _KeyInstall( void );
static int _KeyRelease( void );
static int _KeyWho( void );
static int _KeyPeek( LPMSG lpMsg, UINT fRemove );
static int _KeyPut( LPMSG lpUnused );
static int _KeyRemove( void );
static UINT _KeyCount( void );

//�û������豸�¼��ӿ�
_EVENTDRV _keyEvent = {
    _KeyInstall,
    _KeyRelease,
    _KeyWho,
    _KeyPeek,
    _KeyPut,
    _KeyRemove,
    _KeyCount
};

// **************************************************
// ������void CALLBACK __PutKeybrdEvent( LPKEYRECORD lpRecord )
// ������
// 	IN lpRecord - KEYRECORD�ṹָ�룬������������
// ����ֵ��
//	��
// ����������
//	�ṩ�������豸�Ļص��ӿ�
//	�������豸����������������¼��󣬽���õ��ļ�������ͨ����
//	�������� GWME ϵͳ
// ����: 
//	�������豸�����������
// ************************************************

// define key/button device driver, call by interrupt server subroution
#define DEBUG_PUT_KEYBRD_EVENT 0
void CALLBACK __PutKeybrdEvent( LPKEYRECORD lpRecord )
{
	EnterCriticalSection( &csKeyQueue );
	//��������	
    if( wQueueCount < MAX_QUEUE )
    {	//û�У��������β
		keyQueue[wQueueTail] = *lpRecord;
		lpPrev = (keyQueue + wQueueTail);
		if( ++wQueueTail >= MAX_QUEUE )
			wQueueTail = 0;
		wQueueCount++;
    }
	else
	{
		ERRORMSG( DEBUG_PUT_KEYBRD_EVENT, ( "key queue is full, wQueueCount=%d.\r\n", wQueueCount ) );
	}

	LeaveCriticalSection( &csKeyQueue ); 
	//����ϵͳ���¼�״̬
	SetSYSQueueState( ES_KEY, TRUE );
}

// **************************************************
// ������static BOOL __GetKeybrdEvent( LPKEYRECORD lpRecord, BOOL fRemove )
// ������
// 	OUT lpRecord - ���ڽ��ܼ�������
//	IN fRemove - �Ƿ񽫸ü������ݴӶ������Ƴ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�Ӽ����¼������еõ���������
// ����: 
//	
// ************************************************

static BOOL __GetKeybrdEvent( LPKEYRECORD lpRecord, BOOL fRemove )
{
#ifdef EML_DOS
     static DWORD dwKey = -1;
     if( dwKey == -1 )
     {
        if( bioskey( 1 ) )
        {
            lpRecord->wState = WM_KEYDOWN;
            dwKey = lpRecord->dwKeyValue = bioskey( 0 );
            if( fRemove )
                dwKey = -1;
            return TRUE;
        }
     }
     else
     {
            lpRecord->wState = WM_KEYDOWN;
            lpRecord->dwKeyValue = dwKey;
            if( fRemove )
                dwKey = -1;
            return TRUE;
     }
     return FALSE;
#else

	BOOL bRetv = FALSE;

    EnterCriticalSection( &csKeyQueue );

    if( wQueueCount )
    {	//������
        *lpRecord = keyQueue[wQueueHead];
        if( fRemove )
        {	//�Ƴ�
            if( ++wQueueHead >= MAX_QUEUE )
                wQueueHead = 0;
            wQueueCount--;
        }
        bRetv = TRUE;
    }

	LeaveCriticalSection( &csKeyQueue );

	return bRetv;
#endif
}

// **************************************************
// ������static UINT _KeyCount( void )
// ������
// 	��
// ����ֵ��
//	���ص�ǰ���̶��е��ж��ټ�������
// ����������
//	�õ���ǰ���̶��еļ������ݸ���
// ����: 
//	�ӿں���
// ************************************************

static UINT _KeyCount( void )
{
    return wQueueCount;
}

// **************************************************
// ������static int _KeyInstall()
// ������
// 	��
// ����ֵ��
//	����ɹ�������1�����򣬷���0
// ����������
//	��װ����ʼ������
// ����: 
//	�ӿں���
// ************************************************

static int _KeyInstall()
{
	InitializeCriticalSection( &csKeyQueue );
    return 1;
}

// **************************************************
// ������static int _KeyRelease()
// ������
// 	��
// ����ֵ��
//	�ɹ�������0
// ����������
//	�ͷ�/ж�ؼ���
// ����: 
//	�ӿں���
// ************************************************

static int _KeyRelease()
{
	DeleteCriticalSection( &csKeyQueue );
    return 0;
}

// **************************************************
// ������static int _KeyWho()
// ������
// 	��
// ����ֵ��
//	���ظýӿڵ�ʶ����(ID)
// ����������
//	�õ������¼��ӿڵ�ID
// ����: 
//	�ӿں���
// ************************************************

static int _KeyWho()
{
    return ID_KEY;
}

// **************************************************
// ������static int _KeyPeek( LPMSG lpMsg, UINT fRemove )
// ������
// 	OUT lpMsg -  ���ڽ��ܼ�����Ϣ
//	IN fRemove - �Ƿ��Ƴ����еļ�����Ϣ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�Ӽ��̶��еõ������¼�
// ����: 
//	�ӿں���
// ************************************************

static int _KeyPeek( LPMSG lpMsg, UINT fRemove )
{
    KEYRECORD  keyRecord;
	//�Ӷ����еõ����� 
    if( __GetKeybrdEvent( &keyRecord, fRemove ) )
    {	//�õ�
		if( keyRecord.wState & KEYEVENTF_CHAR )
			lpMsg->message = WM_CHAR;
        else if( keyRecord.wState & KEYEVENTF_KEYUP )
			lpMsg->message = WM_KEYUP;
		else
            lpMsg->message = WM_KEYDOWN;
		lpMsg->wParam = keyRecord.dwKeyValue;
		lpMsg->lParam = keyRecord.lParam;
		
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������static int _KeyPut( LPMSG lpUnused )
// ������
// 	IN lpUnused - ���û�����ļ����¼�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�������û��طż����¼�����Ϣ����
// ����: 
//	�ӿں���
// ************************************************

static int _KeyPut( LPMSG lpUnused )
{
     return FALSE;
}

// **************************************************
// ������static int _KeyRemove()
// ������
// 	��
// ����ֵ��
//	���ص�ǰ���еļ����¼���
// ����������
//	�Ƴ�һ�����ȵļ����¼�
// ����: 
//	�ӿں���
// ************************************************

static int _KeyRemove()
{
    if( wQueueCount )
    {
        if( ++wQueueHead >= MAX_QUEUE )
            wQueueHead = 0;
        wQueueCount--;
    }
    return wQueueCount;
}


// **************************************************
// ������VOID WINAPI WinEvent_KeybdEvent( 
//				 BYTE bVk, 
//				 BYTE bScan, 
//				 DWORD dwFlags, 
//				 DWORD dwExtraInfo )
// ������
//	IN bVk �C �������
//	IN bScan �C ��֧��
//	IN dwFlags �C ����Ϊ����ֵ��
//		KEYEVENTF_KEYUP �C ˵�����̱��ͷţ�������̱�����
//	IN dwExtraInfo �C ��֧��(Ϊ0)
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ϵͳ�������Ͷ��һ�������¼�
// ����: 
//	ϵͳAPI
// ************************************************

VOID WINAPI WinEvent_KeybdEvent( 
				 BYTE bVk, 
				 BYTE bScan, 
				 DWORD dwFlags, 
				 DWORD dwExtraInfo )
{
	KEYRECORD kc;

	kc.dwKeyValue = bVk;
	kc.wState = (WORD)dwFlags;
	kc.wCount = 1;
	kc.lParam = dwExtraInfo;

	__PutKeybrdEvent( &kc );
}

// **************************************************
// ������SHORT WINAPI WinMsg_GetKeyState( int nVirtKey )
// ������
// 	IN nVirtKey - �齡ֵ
// ����ֵ��
//	���� nVirtKey�ļ���״̬
// ����������
//	�õ�nVirtKey�ļ���״̬
// ����: 
//	ϵͳAPI
// ************************************************

SHORT WINAPI WinMsg_GetKeyState( int nVirtKey )
{
	return 0;
}
