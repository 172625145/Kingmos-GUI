/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：键盘事件管理
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
    2003-04-14:增加冲突段检查
    2003-04-11:键盘对列满但窗口却得不到
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

//用户界面设备事件接口
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
// 声明：void CALLBACK __PutKeybrdEvent( LPKEYRECORD lpRecord )
// 参数：
// 	IN lpRecord - KEYRECORD结构指针，包含键盘数据
// 返回值：
//	无
// 功能描述：
//	提供给键盘设备的回调接口
//	当键盘设备驱动程序产生键盘事件后，将其得到的键盘数据通过该
//	函数传给 GWME 系统
// 引用: 
//	被键盘设备驱动程序调用
// ************************************************

// define key/button device driver, call by interrupt server subroution
#define DEBUG_PUT_KEYBRD_EVENT 0
void CALLBACK __PutKeybrdEvent( LPKEYRECORD lpRecord )
{
	EnterCriticalSection( &csKeyQueue );
	//队列满吗？	
    if( wQueueCount < MAX_QUEUE )
    {	//没有，加入队列尾
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
	//设置系统有事件状态
	SetSYSQueueState( ES_KEY, TRUE );
}

// **************************************************
// 声明：static BOOL __GetKeybrdEvent( LPKEYRECORD lpRecord, BOOL fRemove )
// 参数：
// 	OUT lpRecord - 用于接受键盘数据
//	IN fRemove - 是否将该键盘数据从队列中移出
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从键盘事件队列中得到键盘数据
// 引用: 
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
    {	//有数据
        *lpRecord = keyQueue[wQueueHead];
        if( fRemove )
        {	//移出
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
// 声明：static UINT _KeyCount( void )
// 参数：
// 	无
// 返回值：
//	返回当前键盘队列的有多少键盘数据
// 功能描述：
//	得到当前键盘队列的键盘数据个数
// 引用: 
//	接口函数
// ************************************************

static UINT _KeyCount( void )
{
    return wQueueCount;
}

// **************************************************
// 声明：static int _KeyInstall()
// 参数：
// 	无
// 返回值：
//	假如成功，返回1；否则，返回0
// 功能描述：
//	安装并初始化键盘
// 引用: 
//	接口函数
// ************************************************

static int _KeyInstall()
{
	InitializeCriticalSection( &csKeyQueue );
    return 1;
}

// **************************************************
// 声明：static int _KeyRelease()
// 参数：
// 	无
// 返回值：
//	成功，返回0
// 功能描述：
//	释放/卸载键盘
// 引用: 
//	接口函数
// ************************************************

static int _KeyRelease()
{
	DeleteCriticalSection( &csKeyQueue );
    return 0;
}

// **************************************************
// 声明：static int _KeyWho()
// 参数：
// 	无
// 返回值：
//	返回该接口的识别码(ID)
// 功能描述：
//	得到键盘事件接口的ID
// 引用: 
//	接口函数
// ************************************************

static int _KeyWho()
{
    return ID_KEY;
}

// **************************************************
// 声明：static int _KeyPeek( LPMSG lpMsg, UINT fRemove )
// 参数：
// 	OUT lpMsg -  用于接受键盘消息
//	IN fRemove - 是否移出队列的键盘消息
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从键盘队列得到键盘事件
// 引用: 
//	接口函数
// ************************************************

static int _KeyPeek( LPMSG lpMsg, UINT fRemove )
{
    KEYRECORD  keyRecord;
	//从队列中得到数据 
    if( __GetKeybrdEvent( &keyRecord, fRemove ) )
    {	//得到
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
// 声明：static int _KeyPut( LPMSG lpUnused )
// 参数：
// 	IN lpUnused - 由用户加入的键盘事件
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	用于让用户回放键盘事件（消息）。
// 引用: 
//	接口函数
// ************************************************

static int _KeyPut( LPMSG lpUnused )
{
     return FALSE;
}

// **************************************************
// 声明：static int _KeyRemove()
// 参数：
// 	无
// 返回值：
//	返回当前队列的键盘事件数
// 功能描述：
//	移出一个最先的键盘事件
// 引用: 
//	接口函数
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
// 声明：VOID WINAPI WinEvent_KeybdEvent( 
//				 BYTE bVk, 
//				 BYTE bScan, 
//				 DWORD dwFlags, 
//				 DWORD dwExtraInfo )
// 参数：
//	IN bVk – 虚键代码
//	IN bScan – 不支持
//	IN dwFlags – 可以为以下值：
//		KEYEVENTF_KEYUP – 说明键盘被释放，否则键盘被按下
//	IN dwExtraInfo – 不支持(为0)
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	向系统输入队列投递一个键盘事件
// 引用: 
//	系统API
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
// 声明：SHORT WINAPI WinMsg_GetKeyState( int nVirtKey )
// 参数：
// 	IN nVirtKey - 虚健值
// 返回值：
//	返回 nVirtKey的键盘状态
// 功能描述：
//	得到nVirtKey的键盘状态
// 引用: 
//	系统API
// ************************************************

SHORT WINAPI WinMsg_GetKeyState( int nVirtKey )
{
	return 0;
}
