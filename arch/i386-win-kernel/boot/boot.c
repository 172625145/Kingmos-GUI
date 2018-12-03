#include <eversion.h>
#include <windows.h>
#include "w32cfg.h"
//#include <w32cfg.h>
//#include <private\epalloc.h>
//#include <process.h>
//#include <cpu.h>

//extern int StartEsoft( LPARAM lParam );
extern BOOL InitWin32Eml( HINSTANCE hInstance );
//extern BOOL WINAPI Wnd_GetWindowRect( HWND hWnd, LPRECT lpRect );

//DWORD dwID;
//BOOL fExitEsoft = 0;
//HANDLE hEvent;
//extern int fStartSucess;
extern HWND hwndDeskTop;// = NULL;
//int fEnter = 0;
//int eventCount = 0;

//HWND hMoveWnd;
//int iCanMove=0;
//extern LRESULT WINAPI Msg_Send( HWND, UINT, WPARAM, LPARAM );
//extern HWND WINAPI Wnd_GetCapture(void);

//extern void WINAPI KL_DebugOutString( LPTSTR );

//BOOL IsESOFTExit( void )
//{
  //  if( fEnter == 1  && fExitEsoft == 1 )
	//	return TRUE;
	//return FALSE;
//}

extern void CPU_Init( void );
extern void CPU_Deinit( void );
extern void CPU_SetIRQEvent( MSG * lpmsg );
extern BOOL IsESOFTExit( void );

WINBASEAPI
BOOL
WINAPI
IsDebuggerPresent(
    VOID
    );

int WINAPI WinMain(
						  HINSTANCE hInstance,
						  HINSTANCE hPrevInstance,
                          LPSTR     lpCmdLine,
						  int       nCmdShow )
{
    MSG msg;
//	printf( "hello, Kingmos gwme(3.0) for linux.\r\n" );

//	printf( "build data(%s),time(%s).\r\n", __DATE__, __TIME__ );
	CPU_Init();
    if( InitWin32Eml( hInstance ) == FALSE )
		return 1;

	InitGWMEKernel();

    while( 1 )
	{
		if( GetMessage( &msg,0, 0,0 ) )
		{
			if( IsESOFTExit() )
				break;				
			if( msg.hwnd == hwndDeskTop )
			{
				if( msg.message >= WM_MOUSEFIRST &&
					msg.message <= WM_MOUSELAST )
				{
					static BOOL bMouseEnter = 0;
					short y = (short)HIWORD( msg.lParam );
					short x = (short)LOWORD( msg.lParam );
					if( x < iDisplayOffsetX || 
						y < iDisplayOffsetY ||
						x >= (iDisplayOffsetX + iDisplayWidth) ||
						y >= (iDisplayOffsetY + iDisplayHeight) )
					{
						if( bMouseEnter == TRUE )
						{
							MSG m = msg;
							m.message = WM_LBUTTONUP;
							CPU_SetIRQEvent( &msg );
							bMouseEnter = FALSE;
						}
						goto _SKIP;
					}
					else
					{
						bMouseEnter = TRUE;
					}
				}
				if( msg.message == WM_KEYDOWN )
				{
					msg.message = WM_KEYDOWN;
				}

				CPU_SetIRQEvent( &msg );		
			}
_SKIP:
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	CPU_Deinit();
	//sleep(-1);
//	printf("-1 is a incorrect argument\r\n");
//	sleep(99999);
//	printf("Gwme exit.\r\n");
	return 0;
}
/*
int WINAPI WinMain_old(
						  HINSTANCE hInstance,
						  HINSTANCE hPrevInstance,
                          LPSTR     lpCmdLine,
						  int       nCmdShow )
{
    MSG msg;

	DWORD dwVersion = GetVersion();

	if( (dwVersion & 0x80000000) != 0 ||
		(dwVersion & 0xff) < 5  )
	{
		MessageBox( NULL, "请运行在Window2000及其以上", "错误", MB_OK );
	}

    if( InitWin32Eml( hInstance ) == FALSE )
		return 1;
	CPU_Init();

//	KL_DebugOutString( "Enter Message Loop\n" );

    while( 1 )
	{
		if( GetMessage( &msg,0, 0,0 ) )
		{
			if( IsESOFTExit() )
				break;				
			if( msg.hwnd == hwndDeskTop )
			{
				if( msg.message >= WM_MOUSEFIRST &&
					msg.message <= WM_MOUSELAST )
				{
					static BOOL bMouseEnter = 0;
					short y = (short)HIWORD( msg.lParam );
					short x = (short)LOWORD( msg.lParam );
					if( x < iDisplayOffsetX || 
						y < iDisplayOffsetY ||
						x >= (iDisplayOffsetX + iDisplayWidth) ||
						y >= (iDisplayOffsetY + iDisplayHeight) )
					{
						if( bMouseEnter == TRUE )
						{
							MSG m = msg;
							m.message = WM_LBUTTONUP;
							CPU_SetIRQEvent( &msg );
							bMouseEnter = FALSE;
						}
						goto _SKIP;
					}
					else
					{
						bMouseEnter = TRUE;
					}
				}
				CPU_SetIRQEvent( &msg );		
			}
_SKIP:
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

//	KL_DebugOutString( "Platform Leave...\n" );
	CPU_Deinit();
    return 0;
}
*/






