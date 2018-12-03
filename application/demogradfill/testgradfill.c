// testgradfill.cpp : Defines the entry point for the application.
//

#include <ewindows.h>

// Global Variables:
static HINSTANCE hInst;								// current instance
static TCHAR szTitle[] = "渐变填充";								// The title bar text
static TCHAR szWindowClass[]="GRADFILL";// The title bar text

// Foward declarations of functions included in this code module:
static ATOM				MyRegisterClass(HINSTANCE hInstance);
static BOOL				InitInstance(HINSTANCE, int);
static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//static LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#define GRADIENT_FILL_RECT_H    0x00000000
#define GRADIENT_FILL_RECT_V    0x00000001
#define GRADIENT_FILL_TRIANGLE  0x00000002
#define GRADIENT_FILL_OP_FLAG   0x000000ff
typedef USHORT COLOR16;

typedef struct _TRIVERTEX
{
    LONG    x;
    LONG    y;
    COLOR16 Red;
    COLOR16 Green;
    COLOR16 Blue;
    COLOR16 Alpha;
}TRIVERTEX,*PTRIVERTEX,*LPTRIVERTEX;

typedef struct _GRADIENT_TRIANGLE
{
    ULONG Vertex1;
    ULONG Vertex2;
    ULONG Vertex3;
} GRADIENT_TRIANGLE,*PGRADIENT_TRIANGLE,*LPGRADIENT_TRIANGLE;
typedef struct _GRADIENT_RECT
{
    ULONG UpperLeft;
    ULONG LowerRight;
}GRADIENT_RECT,*PGRADIENT_RECT,*LPGRADIENT_RECT;
//typedef BOOL  (WINAPI *LPGradientFill)(HDC,PTRIVERTEX,ULONG,PVOID,ULONG,ULONG);
//LPGradientFill lpGradientFill;
static BOOL FillColor( HDC hdc, LPTRIVERTEX lpTriVertx, ULONG dwNumVertex, PVOID pMesh, ULONG dwNumMesh, ULONG dwMode );

int WINAPI WinMain_GradFill(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HWND hWnd;
//	HACCEL hAccelTable;
//	HMODULE  hModule;

	//hModule = LoadLibrary( "Msimg32.dll" );
	//lpGradientFill = (LPGradientFill)GetProcAddress( hModule, "GradientFill" );
	// Initialize global strings
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadString(hInstance, IDC_TESTGRADFILL, szWindowClass, MAX_LOADSTRING);

	hWnd = FindWindow( szWindowClass, NULL ) ;  // 查找当前程序是否已经运行
	if( hWnd != 0 )
	{ // 已经运行
		SetForegroundWindow( hWnd ) ; // 设置窗口到最前面
		return FALSE ;
	}

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

//	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TESTGRADFILL);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
	//	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wcex;

	//wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_TESTGRADFILL);
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCSTR)IDC_TESTGRADFILL;
	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClass(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowEx(WS_EX_CLOSEBOX, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE,
      0, 0, 300, 400, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
//	TCHAR szHello[MAX_LOADSTRING];
//	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...

			GetClientRect(hWnd, &rt);
			//DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);


			{
				TRIVERTEX        vert[2] ;
				GRADIENT_RECT    gRect;
				vert [0] .x      = 400;
				vert [0] .y      = 0;
				vert [0] .Red    = 0x0000;
				vert [0] .Green  = 0x0000;
				vert [0] .Blue   = 0x0000;
				vert [0] .Alpha  = 0x0000;
				
				vert [1] .x      = 500;
				vert [1] .y      = 32; 
				vert [1] .Red    = 0x0000;
				vert [1] .Green  = 0x0000;
				vert [1] .Blue   = 0xff00;
				vert [1] .Alpha  = 0x0000;
				
				gRect.UpperLeft  = 0;
				gRect.LowerRight = 1;
				//lpGradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);//GRADIENT_FILL_RECT_H);
			}
			{
				TRIVERTEX        vert [8] ;
				GRADIENT_TRIANGLE    gTRi[8];


				vert [0] .x       =  0;
				vert [0] .y       =  0;
				vert [0] .Red     =  0x0000;
				vert [0] .Green   =  0x0000;
				vert [0] .Blue    =  0x0000;
				vert [0] .Alpha   =  0x0000;
				
				vert [1] .x       =  200;
				vert [1] .y       =  100;
				vert [1] .Red     =  0x0000;
				vert [1] .Green   =  0x0000;
				vert [1] .Blue    =  0xFF00;
				vert [1] .Alpha   =  0x0000;
				
				vert [2] .x       =  200;
				vert [2] .y       =  200; 
				vert [2] .Red     =  0x0000;
				vert [2] .Green   =  0x0000;
				vert [2] .Blue    =  0xFF00;
				vert [2] .Alpha   =  0x0000;

				vert [3] .x       =  300;
				vert [3] .y       =  300;
				vert [3] .Red     =  0xff00;
				vert [3] .Green   =  0xff00;
				vert [3] .Blue    =  0x00ff;//ff00;//0xff00;
				vert [3] .Alpha   =  0x0000;

				vert [4] .x       =  200;
				vert [4] .y       =  400;
				vert [4] .Red     =  0x00FF;
				vert [4] .Green   =  0xff00;
				vert [4] .Blue    =  0x00ff;//ff00;//0xff00;
				vert [4] .Alpha   =  0x0000;

				vert [5] .x       =  100;
				vert [5] .y       =  500;
				vert [5] .Red     =  0x00FF;
				vert [5] .Green   =  0x00ff;
				vert [5] .Blue    =  0xff00;//ff00;//0xff00;
				vert [5] .Alpha   =  0x0000;
				
				vert [6] .x       =  0;
				vert [6] .y       =  300;
				vert [6] .Red     =  0x00FF;
				vert [6] .Green   =  0xff00;
				vert [6] .Blue    =  0x00ff;//ff00;//0xff00;
				vert [6] .Alpha   =  0x0000;


				
				gTRi[0].Vertex1   = 0;
				gTRi[0].Vertex2   = 1;
				gTRi[0].Vertex3   = 2;
					
				gTRi[1].Vertex1   = 1;
				gTRi[1].Vertex2   = 2;
				gTRi[1].Vertex3   = 3;

				gTRi[2].Vertex1   = 2;
				gTRi[2].Vertex2   = 3;
				gTRi[2].Vertex3   = 4;

				gTRi[3].Vertex1   = 3;
				gTRi[3].Vertex2   = 4;
				gTRi[3].Vertex3   = 5;

				FillColor(hdc,vert,7,&gTRi,4,GRADIENT_FILL_TRIANGLE);
				

				vert [0] .x       +=  300;
				vert [1] .x       +=  300;
				vert [2] .x       +=  300;
				vert [3] .x       +=  300;
				vert [4] .x       +=  300;
				vert [5] .x       +=  300;	
				vert [6] .x       +=  300;

//				gTRi[0].Vertex1   = 3;
//				gTRi[0].Vertex2   = 4;
//				gTRi[0].Vertex3   = 5;


//				lpGradientFill(hdc,vert,7,&gTRi,4,GRADIENT_FILL_TRIANGLE);
				//MoveToEx( hdc, 0, 0, NULL );
				//LineTo( hdc, 500, 500 );
			}

			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


typedef POINT VECTOR;
typedef VECTOR * LPVECTOR;

typedef struct _LINE_GRADIENT
{
	HDC hdc;

	int left;
	int top;

	int tarNumerator;
	int arDenominator;
	int tagNumerator;
	int agDenominator;
	int tabNumerator;
	int abDenominator;
	int taalphaNumerator;
	int aalphaDenominator;

	int arNumerator;
	int agNumerator;
	int abNumerator;


	int brNumerator;
	int bgNumerator;
	int bbNumerator;
	int balphaNumerator;

	int aalphaNumerator;

}LINE_GRADIENT, FAR * LPLINE_GRADIENT;
typedef BOOL (*LPLINE_CALL_BACK)( VOID * hLineData, int x0, int y, int x1 );

VOID Glyph_Fill( VOID * hGlyph, LPLINE_CALL_BACK lpLineCallBack, VOID * hLineData );
VOID * Glyph_Begin( LPVECTOR lpvt, LPBYTE lpTag, int num );
VOID Glyph_End( VOID * h );

//static int yTotal = 0;
BOOL LineGradient( VOID * hLineData, int x0, int y, int x1 )
{
	LPLINE_GRADIENT lpLineData = (LPLINE_GRADIENT)hLineData;
	int xl = min( x0, x1 );
	int xr = max( x0, x1 );

	int dy = y - lpLineData->top;
	int dx = xl - lpLineData->left;
	int tarNumerator = lpLineData->tarNumerator - dy * lpLineData->brNumerator + dx * lpLineData->arNumerator;	
	int tagNumerator = lpLineData->tagNumerator - dy * lpLineData->bgNumerator + dx * lpLineData->agNumerator;
	int tabNumerator = lpLineData->tabNumerator - dy * lpLineData->bbNumerator + dx * lpLineData->abNumerator;
	int taalphaNumerator = lpLineData->taalphaNumerator - dy * lpLineData->balphaNumerator + dx * lpLineData->aalphaNumerator;

	int arDenominator = lpLineData->arDenominator;
	int agDenominator = lpLineData->agDenominator;
	int abDenominator = lpLineData->abDenominator;

	
	int aalphaDenominator = lpLineData->aalphaDenominator;
	int arNumerator = lpLineData->arNumerator;
	int agNumerator = lpLineData->agNumerator;
	int abNumerator = lpLineData->abNumerator;

	HDC hdc = lpLineData->hdc;
	//int dir = x0 < x1 ? 1 : -1;
	//int dx = x0 - x1;

//	return 0;
	//dx = abs( dx );
	//if( dx > 200 )
	//{
	//	dx = dx;
	//}
	//yTotal++;
	//return 0;
	

	while( xl != xr )
	{
		COLORREF r = ( tarNumerator / arDenominator );// >> 8;
		COLORREF g = ( tagNumerator / agDenominator );// >> 8;
		COLORREF b = ( tabNumerator / abDenominator );// >> 8;
		COLORREF rgb = RGB( r, g, b );
		UINT  alpha = ( taalphaNumerator / aalphaDenominator );// >> 8;	
					
		SetPixel( hdc, xl, y, rgb );
				
		xl ++;//= dir;
		tarNumerator += arNumerator;
		tagNumerator += agNumerator;
		tabNumerator += abNumerator;
	}
	return TRUE;
}


BOOL FillColor( HDC hdc, LPTRIVERTEX lpTriVertx, ULONG dwNumVertex, PVOID pMesh, ULONG dwNumMesh, ULONG dwMode )
{
	//yTotal = 0;
	if( dwMode == GRADIENT_FILL_TRIANGLE )
	{		
		int l, r, t, b;
		int arDenominator, arNumerator, brNumerator;
		int agDenominator, agNumerator, bgNumerator;
		int abDenominator, abNumerator, bbNumerator;
		int aalphaDenominator, aalphaNumerator, balphaNumerator;
		GRADIENT_TRIANGLE * lpTriangle = (GRADIENT_TRIANGLE *)pMesh;
		

		while( dwNumMesh )
		{
			VOID * hGlyph;

			int x0 = lpTriVertx[lpTriangle->Vertex1].x;
			int y0 = lpTriVertx[lpTriangle->Vertex1].y;
			int zr0 = lpTriVertx[lpTriangle->Vertex1].Red >> 8;
			int zg0 = lpTriVertx[lpTriangle->Vertex1].Green >> 8;
			int zb0 = lpTriVertx[lpTriangle->Vertex1].Blue >> 8;
			int zalpha0 = lpTriVertx[lpTriangle->Vertex1].Alpha >> 8;
			
			int x1 = lpTriVertx[lpTriangle->Vertex2].x;
			int y1 = lpTriVertx[lpTriangle->Vertex2].y;
			int zr1 = lpTriVertx[lpTriangle->Vertex2].Red >> 8;
			int zg1 = lpTriVertx[lpTriangle->Vertex2].Green >> 8;
			int zb1 = lpTriVertx[lpTriangle->Vertex2].Blue >> 8;
			int zalpha1 = lpTriVertx[lpTriangle->Vertex2].Alpha >> 8;
			
			int x2 = lpTriVertx[lpTriangle->Vertex3].x;
			int y2 = lpTriVertx[lpTriangle->Vertex3].y;
			int zr2 = lpTriVertx[lpTriangle->Vertex3].Red >> 8;
			int zg2 = lpTriVertx[lpTriangle->Vertex3].Green >> 8;
			int zb2 = lpTriVertx[lpTriangle->Vertex3].Blue >> 8;
			int zalpha2 = lpTriVertx[lpTriangle->Vertex3].Alpha >> 8;


			int trn;
			int tgn;	
			int tbn;	
			int talphan;	

			int tarNumerator_s;
			int tagNumerator_s;
			int tabNumerator_s;
			int taalphaNumerator_s;

			int lineRed;//arNumerator * xl;
			int lineGreen;//agNumerator * xl;
			int lineBlue;//abNumerator * xl;
			int lineAlpha;//abNumerator * xl;

			VECTOR vt[4];
			
			
			arDenominator = ( y2 - y0 ) * ( x1 - x0 ) - ( y1 - y0 ) * ( x2 - x0 );
			if( arDenominator == 0 )
				return TRUE;
			arNumerator = ( y2 - y0 ) * ( zr1 - zr0 ) - ( y1 - y0 ) * ( zr2 - zr0 );
			brNumerator = ( x2 - x0 ) * ( zr1 - zr0 ) - ( x1 - x0 ) * ( zr2 - zr0 );
			
			agDenominator = ( y2 - y0 ) * ( x1 - x0 ) - ( y1 - y0 ) * ( x2 - x0 );
			if( agDenominator == 0 )
				return TRUE;
			agNumerator = ( y2 - y0 ) * ( zg1 - zg0 ) - ( y1 - y0 ) * ( zg2 - zg0 );
			bgNumerator = ( x2 - x0 ) * ( zg1 - zg0 ) - ( x1 - x0 ) * ( zg2 - zg0 );
			
			abDenominator = ( y2 - y0 ) * ( x1 - x0 ) - ( y1 - y0 ) * ( x2 - x0 );
			if( abDenominator == 0 )
				return TRUE;
			abNumerator = ( y2 - y0 ) * ( zb1 - zb0 ) - ( y1 - y0 ) * ( zb2 - zb0 );
			bbNumerator = ( x2 - x0 ) * ( zb1 - zb0 ) - ( x1 - x0 ) * ( zb2 - zb0 );
			
			aalphaDenominator = ( y2 - y0 ) * ( x1 - x0 ) - ( y1 - y0 ) * ( x2 - x0 );
			if( abDenominator == 0 )
				return TRUE;
			aalphaNumerator = ( y2 - y0 ) * ( zalpha1 - zalpha0 ) - ( y1 - y0 ) * ( zalpha2 - zalpha0 );
			balphaNumerator = ( x2 - x0 ) * ( zalpha1 - zalpha0 ) - ( x1 - x0 ) * ( zalpha2 - zalpha0 );

			t = min( y0, y1 );
			t = min( t, y2 );
			
			b = max( y0, y1 );
			b = max( b, y2 );
			
			l = min( x0, x1 );
			l = min( t, x2 );
			
			r = max( x0, x1 );
			r = max( b, x2 );

			vt[0].x = x0;
			vt[0].y = y0;

			vt[1].x = x1;
			vt[1].y = y1;

			vt[2].x = x2;
			vt[2].y = y2;

			vt[3].x = x0;
			vt[3].y = y0;


			hGlyph = Glyph_Begin( vt, NULL, 3 );
			

			/*  原理
			while( t < b )
			{
				int xl = l;
				while( xl < r )
				{
					COLORREF r = ( arNumerator * (  xl - x0 ) - brNumerator * ( t - y0 ) ) / arDenominator + zr0;
					COLORREF g = ( agNumerator * (  xl - x0 ) - bgNumerator * ( t - y0 ) ) / agDenominator + zg0;
					COLORREF b = ( abNumerator * (  xl - x0 ) - bbNumerator * ( t - y0 ) ) / abDenominator + zb0;
					COLORREF rgb = RGB( r, g, b );
					
					SetPixel( hdc, xl, t, rgb );
					xl++;
				}
				t++;
			}
			*/
			/*  优化算法 */
			 trn = -arNumerator * x0  - brNumerator *  t + brNumerator * y0;
			 tgn = -agNumerator * x0  - bgNumerator *  t + bgNumerator * y0;	
			 tbn = -abNumerator * x0  - bbNumerator *  t + bbNumerator * y0;	
			 talphan = -aalphaNumerator * x0  - balphaNumerator *  t + balphaNumerator * y0;	

			 tarNumerator_s = arNumerator * l;
			 tagNumerator_s = agNumerator * l;
			 tabNumerator_s = abNumerator * l;
			 taalphaNumerator_s = aalphaNumerator * l;

			 lineRed = tarNumerator_s + trn + arDenominator * zr0;//arNumerator * xl;
			 lineGreen = tagNumerator_s + tgn + agDenominator * zg0;//agNumerator * xl;
			 lineBlue = tabNumerator_s + tbn + abDenominator * zb0;//abNumerator * xl;
			 lineAlpha = taalphaNumerator_s + talphan + aalphaDenominator * zalpha0;//abNumerator * xl;

			//while( t < b )
			{
				int xl = l;
				//int trn = -arNumerator * x0  - brNumerator *  t + brNumerator * y0;				
				int tarNumerator = lineRed;//tarNumerator_s + trn + arDenominator * zr0;//arNumerator * xl;
				//int tgn = -agNumerator * x0  - bgNumerator *  t + bgNumerator * y0;	
				int tagNumerator = lineGreen;//tagNumerator_s + tgn + agDenominator * zg0;//agNumerator * xl;
				//int tbn = -abNumerator * x0  - bbNumerator *  t + bbNumerator * y0;	
				int tabNumerator = lineBlue;//tabNumerator_s + tbn + abDenominator * zb0;//abNumerator * xl;
				int taalphaNumerator = lineAlpha;//tabNumerator_s + tbn + abDenominator * zb0;//abNumerator * xl;

				//二维点是否在一个二维三角形内
				//A(x1,y1),B(x2,y2),c(x3,y3),要求的点v(x4,y4)
				//计算U,V,判断U,V是否在[0,1]之间比较好
				//U=(x4-x1)/(x2-x1)
				//V=(x4-x1)/(x3-x1),
				//当然,出现x3-x1或x2-x1等于0的情况时,可以用y坐标计算,然后判断u,v在集合[0,1]之间的话,
				//点就在三角形内				
				LINE_GRADIENT lineData;

				lineData.arDenominator = arDenominator;				
				lineData.agDenominator = agDenominator;				
				lineData.abDenominator = abDenominator;
				lineData.aalphaDenominator = aalphaDenominator;

				lineData.tarNumerator = tarNumerator;
				lineData.tagNumerator = tagNumerator;
				lineData.tabNumerator = tabNumerator;
				lineData.taalphaNumerator = taalphaNumerator;


				lineData.arNumerator = arNumerator;
				lineData.agNumerator = agNumerator;
				lineData.abNumerator = abNumerator;
				lineData.aalphaNumerator = aalphaNumerator;

				lineData.brNumerator = brNumerator;
				lineData.bgNumerator = bgNumerator;
				lineData.bbNumerator = bbNumerator;
				lineData.balphaNumerator = balphaNumerator;

				lineData.top = t;
				lineData.left = l;


				lineData.hdc = hdc;


				Glyph_Fill( hGlyph, LineGradient, &lineData );
				
/*
				while( xl < r )
				{
					COLORREF r = ( tarNumerator / arDenominator );// >> 8;
					COLORREF g = ( tagNumerator / agDenominator );// >> 8;
					COLORREF b = ( tabNumerator / abDenominator );// >> 8;
					COLORREF rgb = RGB( r, g, b );
					UINT  alpha = ( taalphaNumerator / aalphaDenominator );// >> 8;	

	
					//if( bInside ) //PtIsInside( xl, t, x0, y0, x1, y1, x2, y2 ) )
					{
						SetPixel( hdc, xl, t, rgb );
					}

					xl++;
					tarNumerator += arNumerator;
					tagNumerator += agNumerator;
					tabNumerator += abNumerator;
					taalphaNumerator += aalphaNumerator;
				}
*/

				t++;

				lineRed -= brNumerator;
				lineGreen -= bgNumerator;
				lineBlue -= bbNumerator;
				lineAlpha -= balphaNumerator;
			}			

			Glyph_End( hGlyph );
			
			lpTriangle++;
			dwNumMesh--;
		}
	}
	return TRUE;
}



