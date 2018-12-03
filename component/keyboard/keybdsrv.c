#include <ewindows.h>
#include <eapisrv.h>
#include "keybdsrv.h"
#include <thirdpart.h>
#include "smartpen.h"
#include <softkey.h>

#define KEYBOARDSTARTX  0
//#define KEYBOARDSTARTY  (320-TOOL_BAR_HEIGHT-KEYBOARDHEIGHT)
#define KEYBOARDSTARTY  (320-20-KEYBOARDHEIGHT)

HINSTANCE g_hInstance = NULL;
extern BOOL RegisterKeyboardClass( HINSTANCE hInst );


HWND WINAPI Keybd_CreateKeyboard(HINSTANCE hInstance, HWND hParent, int yPos);
VOID WINAPI Keybd_ShowKeyboad(HWND hWnd,DWORD dwFlag);
extern SHORT WINAPI Keybd_VkKeyScan(TCHAR uChar);
extern UINT WINAPI Keybd_MapVirtualKey(UINT uCode, UINT uMapType);
extern void WINAPI Keybd_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp);


///////////////////////////////////////////////////
const PFNVOID lpKeybdAPI[] = 
{
	NULL,
    (PFNVOID)Keybd_CreateKeyboard,  // 创建键盘
    (PFNVOID)Keybd_ShowKeyboad,  // 显示键盘
	(PFNVOID)Keybd_VkKeyScan,
	(PFNVOID)Keybd_MapVirtualKey, 
	(PFNVOID)Keybd_SetKeyState,
};

static const DWORD dwKeybdArgs[] = {
	0,	
	ARG3_MAKE( DWORD, DWORD, DWORD),//HWND Keybd_CreateKeyboard(HINSTASNCE hInstance, HWND hParent, int yPos)
	ARG2_MAKE( DWORD,DWORD),//void Keybd_ShowKeyboad(HWND hWnd,DWORD dwFlag)
	ARG1_MAKE( DWORD),//SHORT WINAPI Keybd_VkKeyScan(TCHAR uChar)
	ARG2_MAKE( DWORD,DWORD),//UINT WINAPI Keybd_MapVirtualKey(UINT uCode, UINT uMapType)
	ARG2_MAKE( DWORD,DWORD),//void WINAPI Keybd_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp)
};



void InstallKeybdServer( void )
{
    RETAILMSG(1, ("InstallKeybdServer...\r\n") );
	API_RegisterEx( API_KEYBD,  (const  PFNVOID *)lpKeybdAPI, dwKeybdArgs, sizeof( lpKeybdAPI ) / sizeof(PFNVOID) );
    RETAILMSG(1, ("InstallKeybdServer  OK!!!\r\n") );
}



// **************************************************
// 声明：int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR     lpCmdLine,int nCmdShow)
// 参数：
// 	IN hInstance -- 当前应用程序的实例句柄
// 	IN hPrevInstance -- 前一个应用程序的实例句柄
// 	IN lpCmdLine -- 运行参数
// 	IN nCmdShow -- 显示标志
// 
// 返回值：无
// 功能描述：应用程序入口参数。
// 引用: 
// **************************************************
#ifdef INLINE_PROGRAM
int WINAPI WinMain_KeybdApi(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#else
LRESULT CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
	HANDLE hKeybdEvent;

    RETAILMSG(1, ("InitKeybd...\r\n") );
	g_hInstance = hInstance;
	InstallKeybdServer();  // 安装键盘服务程序

	RegisterKeyboardClass( 0 );  // 注册键盘窗口

	hKeybdEvent = CreateEvent( NULL, FALSE, FALSE, "Keybd_event" );  // 创建事件

	API_SetReady(API_KEYBD);
	while( 1 )
	{
		int rv;
		rv = WaitForSingleObject( hKeybdEvent, INFINITE );  // 等待事件
		break;
	}
	
	CloseHandle( hKeybdEvent );  // 关闭句柄
	return 0;
}




// **************************************************
// 声明：VOID WINAPI Keybd_CreateKeyboard(HINSTANCE hInstance, HWND hParent, int yPos)
// 参数：
//		hInstance -- 实例句柄
//		hParent	--   窗口句柄
//		yPos	--	 窗口创建的位置
// 返回值：
//	返回成功创建的键盘窗口句柄
// 功能描述：创建键盘。
// 引用: 
// **************************************************
HWND WINAPI Keybd_CreateKeyboard(HINSTANCE hInstance, HWND hParent, int yPos)
{
	HWND hKey;
	int y;
	// 创建键盘。

	if (yPos == -1)
		y = KEYBOARDSTARTY;
	else
		y = yPos;
//    hKey = CreateWindowEx(WS_EX_TOPMOST|WS_EX_INPUTWINDOW,  
    hKey = CreateWindowEx(WS_EX_INPUTWINDOW,  
						"Keyboard",
						"键盘",
//						WS_POPUP|WS_VISIBLE,//|WS_CAPTION,
						WS_CHILD,//|WS_CAPTION,
						KEYBOARDSTARTX,
						y,//KEYBOARDSTARTY,
						KEYBOARDWIDTH,
						KEYBOARDHEIGHT,
						hParent,
						(HMENU)0XFFFF,
						hInstance,
						NULL);
	{
	DWORD dwStyle;//,dwExStyle;

		dwStyle = GetWindowLong(hKey,GWL_STYLE);  // 得到键盘的风格

	}
	return hKey;
}
// **************************************************
// 声明：VOID WINAPI Keybd_ShowKeyboad(HWND hWnd,DWORD dwFlag)
// 参数：
//	hWnd --窗口句柄
//	dwFlag -- 显示标志
// 返回值：
// 功能描述：显示键盘
// 引用: 
// **************************************************
VOID WINAPI Keybd_ShowKeyboad(HWND hWnd,DWORD dwFlag)
{
	DWORD dwStyle;//,dwExStyle;

	dwStyle = GetWindowLong(hWnd,GWL_STYLE);  // 得到键盘的风格

	if (dwStyle & WS_VISIBLE)
	{  // 当前键盘已经显示
		if (dwFlag == SIP_SHOW)
			return ;
		ShowWindow(hWnd,SW_HIDE);
//		SetWindowPos(hWnd,0,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);  // 隐藏键盘
	}
	else
	{
		// 当前没有显示键盘
		if (dwFlag == SIP_HIDE)
			return ;
//		ShowWindow(hWnd,SW_SHOW);
		ShowWindow(hWnd,SW_SHOWNORMAL);
//		SetWindowPos(hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);  // 显示键盘
	}
}
