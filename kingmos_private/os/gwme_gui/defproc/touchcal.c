#include <ewindows.h>
#include <eapisrv.h>
//#include <touch.h>
//#include "touchcal.h"
//#include <gwmeobj.h>
//#include <gwmesrv.h>

#define ID_EXITBUTTON  (WM_USER+100)
#define C_CALIBRATION_MSG_BUF_CHARS 128
#define WM_CALIBRATION (WM_USER+1)


static char *IDS_CALIBRATION_1_1_NK[2] = {"用笔点十字中心",
									         "重复直到完成"};
static char *IDS_CALIBRATION_2_1_NK[3] = {"新的校正已完成."};
		//							"点击屏幕，系统将保存当前的数据。,
		//  						"30秒后将自动退出，刚才的操作无效。"};
static char *IDS_CALIBRATION_TIMEOUT[1] = {"       剩余时间: %d 秒       "};

//extern UINT  uiCalibrationState;
static int x_cal[5], y_cal[5], x_Screen[5], y_Screen[5];  
static int bCal_over,nCount, nlinelen, nWidth, nHeight;
static HWND hWnd, hButton;
static const char szWindowClass[] = "Touch_Cal";
//static HINSTANCE hInst;

// Forward declarations of functions included in this code module:
static ATOM				MyRegisterClass	(HINSTANCE, LPCSTR);
static BOOL				InitInstance	(HINSTANCE, int);
static LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);
static VOID CalibrationXY(int x, int y);


//static VOID CALLBACK SetCalibrationXY(int x, int y);
/*
extern BOOL TouchPanelSetCalibration(
    int   cCalibrationPoints,     //@PARM The number of calibration points
    int   *pScreenXBuffer,        //@PARM List of screen X coords displayed
    int   *pScreenYBuffer,        //@PARM List of screen Y coords displayed
    int   *pUncalXBuffer,         //@PARM List of X coords collected
    int   *pUncalYBuffer          //@PARM List of Y coords collected
    );
*/
static 
void TextSize(
	HDC		hdc,
	LPSTR	pszStr,
	int		cChars,
	SIZE	*pSize)
{
	if ( !GetTextExtentExPoint(
				hdc,
				pszStr,
				cChars,
				0,	 // nMaxExtent
				NULL, // lpnFit
				NULL,
				pSize) )
		{
		ASSERT(0);
		}
	return;
}

static 
void TouchCalibrateDrawScreenText(
	HDC		hdc,
	int		cLines,
	char   **pString)
{
	int		VertSpacing = 0;
	char	buf[C_CALIBRATION_MSG_BUF_CHARS];
	int		cChars;
	int		xText, yText;
	SIZE	Size;
	int		i;
	RECT rc;// = { 0, 0, nWidth, nHeight };

    rc.left = 0;
    rc.top = 0;
    rc.right = nWidth;
    rc.bottom = nHeight;
	//	Clear the screen.
	//Rectangle(hdc, 0, 0, nWidth, nHeight);
	FillRect( hdc, &rc, GetStockObject( WHITE_BRUSH ) ); 

	//	Display each line of instructions.
	for ( i = 0; i < cLines; i++ ) 	{
		memcpy(buf, *pString, strlen(*pString)+1);
		*pString++;
		cChars = strlen(buf);
		TextSize(hdc, buf, cChars, &Size);
		xText = nWidth/2 - Size.cx/2;	//	Center horizontally
		//	If first pass through, figure vertical spacing,
		//	else just skip to the next line.
		if ( VertSpacing == 0 )	{
			VertSpacing = Size.cy + Size.cy/10;
			yText = 4;//VertSpacing;		// Skip a line at the top
			}
		else{
			yText += VertSpacing;
		}

		//	Draw this line.
		SetTextColor( hdc, CL_BLACK );
		SetBkMode( hdc, TRANSPARENT );
		ExtTextOut(
				  hdc,
				  xText, yText,
				  NULL, NULL,	 //  rectangle options
				  buf, cChars,
				  NULL);
		}

	return;
}

static void ES_TouchCalibrateUI_DrawMainScreen(
	HDC	hdc)
{
	char	   **pStringPos; 

	//	Figure out string resources based on whether keyboard is enabled.
	if( !bCal_over ) {
		pStringPos = IDS_CALIBRATION_1_1_NK;
		TouchCalibrateDrawScreenText(hdc, 2, pStringPos);
	}
	else {
		pStringPos = IDS_CALIBRATION_2_1_NK;
		TouchCalibrateDrawScreenText(hdc, 1, pStringPos);
	}
	return;
}

/*
void ES_TouchCalibrateUI_WaitForConfirmation(
	HANDLE	hevt,
	HDC		hdcConfirmation)
{
	int		TimeOut;
	DWORD	dwEventTimeOut;

	//	Wait here until TouchCalibrateUI_HandleUserInputMessage signals.
	//	If the keyboard is not enabled, we wait until a tap or for 30 seconds
	//	to decide what to do.  If there is a tap, the calibration is accepted.
	//	If we time out after 30 seconds, the calibration is rejected.

	//	If Keyboard is enabled, set timeout to INFINITE.
	//	If not, set the timeout to 1 sec and display a count down.
	dwEventTimeOut = UseEnterEsc() ? INFINITE : 1000;

	//	Assume a 1 sec timeout.  If the timeout is infinite, it won't
	//	matter.
	for ( TimeOut = 30 ; TimeOut >= 0 ; TimeOut--)
		{
		char	buf[C_CALIBRATION_MSG_BUF_CHARS];
		char	bufFormat[C_CALIBRATION_MSG_BUF_CHARS];
		int		cChars;
		int		xText, yText;
		SIZE	Size;

		WaitForSingleObject(hevt, dwEventTimeOut);

		//	If no longer in the confirming state (for whatever reason)
		//	we're done.
		if ( *s_ptcs != TCS_CONFIRMING )
			{
			break;
			}

		// Display countdown.
		memcpy(bufFormat, IDS_CALIBRATION_TIMEOUT, sizeof(IDS_CALIBRATION_TIMEOUT));
		wsprintf(buf, bufFormat, TimeOut);
		cChars = _tcslen(buf);
		TextSize (hdcConfirmation, buf, cChars, &Size);
		xText = GetSystemMetrics(SM_CXSCREEN)/2 - Size.cx/2;	//	Center
		yText = (Size.cy + Size.cy/10)*10;						//	Set line.
		ExtTextOut(
				  hdcConfirmation,
				  xText, yText,
				  NULL, NULL, // rectangle options
				  buf, cChars,
				  NULL);
		}

	return;
} */

//int CALLBACK WinMain_touchCal(HINSTANCE hInstance,
//					HINSTANCE hPrevInstance,
//					LPTSTR    lpCmdLine,
//					int       nCmdShow)

BOOL WINAPI Sys_TouchCalibrate( void )
{
//	extern HINSTANCE hgwmeInstance;
	MSG msg;	
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);//hgwmeInstance;
	bCal_over = 0;

	while( API_IsReady( API_FILESYS ) == FALSE ||
		   API_IsReady( API_GWE ) == FALSE )
	{ //判断系统是否已经准备好
		RETAILMSG( 1, ( "wait filesys and gwe\r\n" ) );
		Sleep(200);
	}

	// Perform application initialization:
	if (!InitInstance (hInstance, SW_SHOWNORMAL )) 
	{
		return FALSE;
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

ATOM RegisterTouchCalClass( HINSTANCE hInstance )
{
	WNDCLASS	wc;
	
	//hInst = hInstance;
    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= NULL;
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//MyRegisterClass(hInstance, szWindowClass);
//	RETAILMSG( 1, ( "************** x=%d,y=%d.\r\n************",  GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) ) );
	hWnd = CreateWindowEx( WS_EX_TOPMOST, szWindowClass, NULL, WS_VISIBLE,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
//	hWnd = CreateWindowEx( 0, szWindowClass, NULL, WS_VISIBLE | WS_BORDER,
//		0, 0, AP_XSTART + AP_WIDTH, AP_YSTART + AP_HEIGHT, NULL, NULL, hInstance, NULL);


	if (!hWnd)
	{	
		return FALSE;
	}


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

static void draw_cross(HDC hdc)
{
	POINT points[2];
	HPEN hPen, hOldPen;

	//EdbgOutputDebugString( "draw_cross.\r\n", nCount );

	ES_TouchCalibrateUI_DrawMainScreen(hdc);


	if( !bCal_over ) {

		hPen = CreatePen(PS_SOLID, 3, RGB(0xff,0,0));//GetStockObject( BLACK_PEN );
		hOldPen = (HPEN)SelectObject(hdc, hPen);

		points[0].x = x_Screen[nCount] - nlinelen; 
		points[1].x = x_Screen[nCount] + nlinelen; 
		points[0].y = y_Screen[nCount];
		points[1].y = y_Screen[nCount];
		//RETAILMSG( 1, ( "Polyline++.\r\n" ) );
		Polyline(hdc, points, 2);
		//RETAILMSG( 1, ( "Polyline--.\r\n" ) );
		points[0].x = x_Screen[nCount];
		points[1].x = x_Screen[nCount];
		points[0].y = y_Screen[nCount] - nlinelen;
		points[1].y = y_Screen[nCount] + nlinelen;
		Polyline(hdc, points, 2);

		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
	}
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message) 
	{
	//case WM_LBUTTONDOWN:
	//case WM_MOUSEMOVE:
	//case WM_LBUTTONUP:
		//CalibrationXY( (short)HIWORD( lParam ), (short)LOWORD( lParam ) );
	//	break;
	//case WM_CALIBRATION:
	case WM_LBUTTONDOWN:
		CalibrationXY( (short)LOWORD( lParam ), (short)HIWORD( lParam ) );
		return 0;
	case WM_CREATE:
		{
			//uiCalibrationState = 0x02;  //CalibrationDown;
			//RETAILMSG( 1, ( "Enter Calibration State(%d).\r\n", CALIBRATION_DOWN ) );
//			if( lpGwdiPosEnter )
//			    lpGwdiPosEnter( GWDI_CALIBRATION, CALIBRATION_DOWN, SetCalibrationXY );
			Sys_SetCalibrateWindow( hWnd );

			nWidth =  GetSystemMetrics(SM_CXSCREEN);
			nHeight = GetSystemMetrics(SM_CYSCREEN);
			nlinelen = (nWidth > nHeight ) ? nHeight /20 : nWidth /20;
		
			x_Screen[0] = (nWidth / 2 );      y_Screen[0] = (nHeight / 2 );
			x_Screen[1] = (nWidth / 5 );      y_Screen[1] = (nHeight / 5 );
			x_Screen[2] = (nWidth / 5 );      y_Screen[2] = (nHeight / 5 ) * 4;
			x_Screen[3] = (nWidth / 5 ) * 4;  y_Screen[3] = (nHeight / 5 ) * 4;
			x_Screen[4] = (nWidth / 5 ) * 4;  y_Screen[4] = (nHeight / 5 );
			nCount = 0;

//			hButton = CreateWindow("button", "Exit", WS_VISIBLE | WS_CHILD,
//				nWidth/2 - 25, nHeight/2 - 20, 50, 40, hWnd, 
//				(HMENU)ID_EXITBUTTON, GetWindowLong( hWnd, GWL_HINSTANCE ), NULL);
//			EnableWindow(hButton, FALSE);
//			ShowWindow(hButton, SW_HIDE);
		}
			break;
		//case WM_COMMAND:
			//if( LOWORD(wParam) == ID_EXITBUTTON ) {
				//add for arm by kingkong 2002.4.12
			//	RECT	rt ;
			//	hdc = GetDC( hWnd ) ;
			//	GetClientRect( hWnd, &rt ) ;
			//	FillRect( hdc, &rt, GetStockObject( BLACK_BRUSH ) ) ;
			//	ReleaseDC( hWnd, hdc ) ;
				
				//---------------------------------
			//	DestroyWindow(hWnd);
			//}
			//break;

		case WM_PAINT:
		{	
			RECT rt;

			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);
			draw_cross(hdc);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// the fun called by touch-mdd
/*
static VOID CALLBACK SetCalibrationXY(int x, int y)
{ 

    ASSERT( hWnd );
	if( !hWnd )
		return;
	RETAILMSG( 1, ( "SetCalibrationXY:x=%d,y=%d.\r\n", x, y ) );

	PostMessage( hWnd, WM_CALIBRATION, 0, MAKELONG( x, y ) );
	//CalibrationXY( x, y );
}
*/
static VOID CalibrationXY(int x, int y)	
{
	int i;

    x_cal[nCount] = x;	y_cal[nCount] = y;
	nCount++;

	EdbgOutputDebugString( "count:%d\r\n", nCount );
	if( nCount == 5 ) {
		for( i = 0; i<5; i++ ) {
			EdbgOutputDebugString("x[%d]=%d, y[%d]=%d \r\n", i, x_cal[i], i, y_cal[i]);
		}
		for( i = 0; i<5; i++ ) {
			EdbgOutputDebugString("x_b[%d]=%d, y_b[%d]=%d \r\n", i, x_Screen[i], i, y_Screen[i]);
		}

		//if( !TouchPanelSetCalibration(5, x_Screen, y_Screen, x_cal, y_cal) ) {
		if( !Sys_SetCalibratePoints(5, x_Screen, y_Screen, x_cal, y_cal) ) {
			nCount = 0;
		}
		else {
			//uiCalibrationState = 0;//CalibrationInactive;
//			if( lpGwdiPosEnter )
//			    lpGwdiPosEnter( GWDI_CALIBRATION, CALIBRATION_INACIVE, 0 );  //完成
				Sys_SetCalibrateWindow( NULL );			

			bCal_over = 1;
			//EnableWindow(hButton, TRUE);
			//ShowWindow(hButton, SW_SHOW);
			DestroyWindow(hWnd);
			return; 
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}
