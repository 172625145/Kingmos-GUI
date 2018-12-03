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
    (PFNVOID)Keybd_CreateKeyboard,  // ��������
    (PFNVOID)Keybd_ShowKeyboad,  // ��ʾ����
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
// ������int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR     lpCmdLine,int nCmdShow)
// ������
// 	IN hInstance -- ��ǰӦ�ó����ʵ�����
// 	IN hPrevInstance -- ǰһ��Ӧ�ó����ʵ�����
// 	IN lpCmdLine -- ���в���
// 	IN nCmdShow -- ��ʾ��־
// 
// ����ֵ����
// ����������Ӧ�ó�����ڲ�����
// ����: 
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
	InstallKeybdServer();  // ��װ���̷������

	RegisterKeyboardClass( 0 );  // ע����̴���

	hKeybdEvent = CreateEvent( NULL, FALSE, FALSE, "Keybd_event" );  // �����¼�

	API_SetReady(API_KEYBD);
	while( 1 )
	{
		int rv;
		rv = WaitForSingleObject( hKeybdEvent, INFINITE );  // �ȴ��¼�
		break;
	}
	
	CloseHandle( hKeybdEvent );  // �رվ��
	return 0;
}




// **************************************************
// ������VOID WINAPI Keybd_CreateKeyboard(HINSTANCE hInstance, HWND hParent, int yPos)
// ������
//		hInstance -- ʵ�����
//		hParent	--   ���ھ��
//		yPos	--	 ���ڴ�����λ��
// ����ֵ��
//	���سɹ������ļ��̴��ھ��
// �����������������̡�
// ����: 
// **************************************************
HWND WINAPI Keybd_CreateKeyboard(HINSTANCE hInstance, HWND hParent, int yPos)
{
	HWND hKey;
	int y;
	// �������̡�

	if (yPos == -1)
		y = KEYBOARDSTARTY;
	else
		y = yPos;
//    hKey = CreateWindowEx(WS_EX_TOPMOST|WS_EX_INPUTWINDOW,  
    hKey = CreateWindowEx(WS_EX_INPUTWINDOW,  
						"Keyboard",
						"����",
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

		dwStyle = GetWindowLong(hKey,GWL_STYLE);  // �õ����̵ķ��

	}
	return hKey;
}
// **************************************************
// ������VOID WINAPI Keybd_ShowKeyboad(HWND hWnd,DWORD dwFlag)
// ������
//	hWnd --���ھ��
//	dwFlag -- ��ʾ��־
// ����ֵ��
// ������������ʾ����
// ����: 
// **************************************************
VOID WINAPI Keybd_ShowKeyboad(HWND hWnd,DWORD dwFlag)
{
	DWORD dwStyle;//,dwExStyle;

	dwStyle = GetWindowLong(hWnd,GWL_STYLE);  // �õ����̵ķ��

	if (dwStyle & WS_VISIBLE)
	{  // ��ǰ�����Ѿ���ʾ
		if (dwFlag == SIP_SHOW)
			return ;
		ShowWindow(hWnd,SW_HIDE);
//		SetWindowPos(hWnd,0,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);  // ���ؼ���
	}
	else
	{
		// ��ǰû����ʾ����
		if (dwFlag == SIP_HIDE)
			return ;
//		ShowWindow(hWnd,SW_SHOW);
		ShowWindow(hWnd,SW_SHOWNORMAL);
//		SetWindowPos(hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);  // ��ʾ����
	}
}
