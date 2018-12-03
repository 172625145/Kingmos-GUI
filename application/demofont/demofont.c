//-------


#include <ewindows.h>


static HINSTANCE hInst;
static HWND hWnd;
static HDC hdctemp;
static HBITMAP hBitmap;
static char fontstr[5][100]; 
static HFONT hHanFont;
static HFONT hGb2312Font;

#define DemoFont_ClassName TEXT("Class_DemoFontMain")
#define WM_USERFONT 9888

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void OnTime(HWND hWnd);
static void LoadString(LPTSTR filename);

static	ATOM	Main_RegClass(HINSTANCE hInstance)
{
	WNDCLASS	wcex;

	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)Main_WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(hInstance, IDC_ARROW);
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= DemoFont_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK DemoFont_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;
	LOGFONT lf;
	hWnd = FindWindow( DemoFont_ClassName, NULL ) ;  // 查找窗口是否存在
	if( hWnd != 0 )
	{ // 窗口已经存在
		SetForegroundWindow( hWnd ) ; // 设置窗口到前台
		return FALSE ;
	}

	Main_RegClass(hInstance);

	if (!Main_InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	//字体
	memset( &lf, 0, sizeof(lf) );
	lf.lfCharSet = HANGUL_CHARSET;
	lf.lfHeight = 64;
	lf.lfWidth = 64;
	hHanFont = CreateFontIndirect( &lf ); 
	RETAILMSG(1, ("hHanFont:0x%x \r\n ...........", hHanFont));
	lf.lfCharSet = GB2312_CHARSET;
	lf.lfHeight = 48;
	lf.lfWidth = 48;
	hGb2312Font = CreateFontIndirect( &lf ); 
	RETAILMSG(1, ("hGb2312Font:0x%x \r\n ...........", hGb2312Font));
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	RETAILMSG(1, ("end.\r\n"));	

	return msg.wParam;
}

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int width,heigh;
   
	hInst = hInstance;
   

    width = GetSystemMetrics(SM_CXSCREEN);
	heigh = GetSystemMetrics(SM_CYSCREEN);

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,DemoFont_ClassName, NULL, WS_VISIBLE,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		return FALSE;
	}
	SetForegroundWindow(hWnd);
//	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;


	switch (message) 
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_CREATE:
			hdc = GetDC(hWnd);
			{
				RECT rect = {0,0,600,400};

				hdctemp = CreateCompatibleDC(NULL);
				hBitmap = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
				SelectObject(hdctemp, (HGDIOBJ)hBitmap);
				FillRect(hdctemp,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));	
			}
			ReleaseDC( hWnd, hdc );
#ifdef LINUX_KERNEL
			LoadString("./kingmos/font.txt");
#else
			LoadString("/kingmos/font.txt");
#endif
			PostMessage(hWnd,WM_USERFONT,0,0);
			break;
		case WM_USERFONT:
			OnTime(hWnd);
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			DeleteDC(hdctemp);
			DeleteObject(hBitmap);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

#define STRFILE 500
static void LoadString(LPTSTR filename)
{
	HANDLE fileHandle;
	int i,num = 0,bufnum = 0;
	DWORD dwLength;
	char buf[STRFILE];

	fileHandle = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE,0);
	if(fileHandle == INVALID_HANDLE_VALUE)
	{	
		for(i = 0;i<5;i++)
			fontstr[i][0] = '\0';
		return;
	}
	ReadFile(fileHandle,buf,STRFILE,&dwLength,NULL);
	
	for(i = 0;i<5;i++)
	{
		num = 0;
		while((buf[bufnum] != 10) && (num < 100))
		{
			fontstr[i][num] = buf[bufnum];
			num++;
			bufnum++;
		}
		bufnum++;
		fontstr[i][--num] = '\0';
	}
	CloseHandle(fileHandle);
}

static void OnTime(HWND hWnd)
{
	HDC hdc;
	int i = 1,j;
	RECT rect = {0,0,600,400};
	HFONT hOld;
	
	
	char ft[] = "中文繁體";
	char jt[] = "中文简体";
	char eng[] = "abcdefghijklmn";
	char kr[] = "伙己傈磊 荤捞磊伙己傈磊荤捞滚扼磊";
	char jp[] = "平素はイメージステーションをご利用いた";
	
	hdc = GetDC( hWnd );
//	RETAILMSG(1, ("textout 0123456789 \r\n ..........."));
//	TextOut(hdc,0, 0, "1234567890", 10);

	TextOut(hdc,0,80*3,eng,sizeof(eng)-1);		
	
	hOld = SelectObject( hdc, hHanFont );
	TextOut(hdc,0,80*4,kr,strlen(kr));
	SelectObject( hdc, hOld );

	hOld = SelectObject( hdc, hGb2312Font );
	TextOut(hdc,0,80*1,ft,strlen(ft));
	SelectObject( hdc, hOld );

	hOld = SelectObject( hdc, hGb2312Font );
	TextOut(hdc,0,80*2,jt,strlen(jt));
	SelectObject( hdc, hOld );
	
	hOld = SelectObject( hdc, hGb2312Font );
	TextOut(hdc,0,80*5,jp,strlen(jp));
	SelectObject( hdc, hOld );
	
	/*	
		
	for(j = 1;j < 12;j++)
	{
		if(i<6)
		{
			if(i == 4)
			{
				hOld = SelectObject( hdctemp, hHanFont );
				TextOut(hdctemp,120,60*i,fontstr[i-1],strlen(fontstr[i-1]));
				SelectObject( hdctemp, hOld );
			}
			else if(i == 3)
			{
				TextOut(hdctemp,120,60*i,fontstr[i-1],strlen(fontstr[i-1]));
			}
			else
			{
				hOld = SelectObject( hdctemp, hGb2312Font );
				TextOut(hdctemp,120,60*i,fontstr[i-1],strlen(fontstr[i-1]));
				SelectObject( hdctemp, hOld );
			}
		}
		else
			FillRect(hdctemp,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		BitBlt(hdc, 120,40,600, 400, hdctemp,0,0,SRCCOPY);
		
		i++;
		if(i > 6)
		{
			i = 1;
		}
		Sleep(500);
	}
*/
	ReleaseDC( hWnd, hdc );
	Sleep(5000);

}

