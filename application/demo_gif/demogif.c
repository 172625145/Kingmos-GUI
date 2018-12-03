//-------


#include <ewindows.h>
#include <gif2bmp.h>


HINSTANCE hInst;
HWND hWnd;

#define TIMEINTERVAL	50
typedef struct IMGBrowserStruct{
	HDC hMemoryDC;  // Ϊ�˷�ֹ������˸��ʹ���ڴ�DC,��ֻ����PAINT
	HBITMAP hMemoryBmp;  // �����ڴ�DC��λͼ

	UINT uFileType; // ��ǰ��ʾ��ͼ������
	HGIF hGif ; // Gif ͼ��ľ��
	HBITMAP hBitmap ; // bitmap , jpeg ͼ��ľ��

	UINT iIndex; // gifͼ��ʹ�ã���ǰ���ڲ��ŵ�ͼ������
	int iDelayTime; // gif ͼ��ʹ�ã�������һ������ͼ����Ҫ�ȴ���ʱ��
	UINT iImageNum ; //gif ͼ��ʹ�ã���ǰһ���ж��ٷ�ͼ��

	POINT point ; // ��ʾͼ���λ��
	SIZE  Size ;  // ��ǰͼ��Ĵ�С

	COLORREF cl_Text;   // �ı���ɫ
	COLORREF cl_BkColor;  // ������ɫ

	HANDLE hDrawGifThread;
	BOOL bExit;
}IMGBROWSER, * LPIMGBROWSER;

static LPIMGBROWSER lpIMGBrowser = NULL;

#define Demogif_ClassName TEXT("Class_DemogifMain")
#define WM_USERGIF  8654

static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT  DoGifFile(HWND hWnd);
static LRESULT OnGifPaint( HWND hWnd );
static LRESULT DoGifCreate(HWND hWnd);
static DWORD WINAPI ShowImageThread(VOID * pParam);
static LRESULT DoGifReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void ShowCurImage(HWND hWnd, HDC hdc);
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize);
static int GetTotalPic(HGIF hGif);
static HDC CreateMemoryDC(HWND hWnd, HBITMAP *lpNewBitmap);
static int GetNextPicTime(HGIF hGif,int iIndex);
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg);

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
	wcex.hbrBackground	= NULL;//(HBRUSH)(BLACK_BRUSH);//GetStockObject(WHITE_BRUSH)
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= Demogif_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK Demogif_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;

	hWnd = FindWindow( Demogif_ClassName, NULL ) ;  // ���Ҵ����Ƿ����
	if( hWnd != 0 )
	{ // �����Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���ô��ڵ�ǰ̨
		return FALSE ;
	}

	Main_RegClass(hInstance);

	if (!Main_InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int width,heigh;
   
	hInst = hInstance;
   

    width = GetSystemMetrics(SM_CXSCREEN);
	heigh = GetSystemMetrics(SM_CYSCREEN);

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,Demogif_ClassName, NULL, WS_VISIBLE,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetForegroundWindow(hWnd);
	return TRUE;
}

static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:
			OnGifPaint(hWnd);
			break;
		case WM_CREATE:
			DoGifCreate(hWnd);
			DoGifFile(hWnd);
			break;
/*		case WM_KEYUP:
			{
				if(wParam == VK_BACK)
					DestroyWindow(hWnd);
			}
			break;
*/		case WM_USERGIF:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			DoGifReleaseControl(hWnd,wParam,lParam);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


static LRESULT  DoGifFile(HWND hWnd)
{

	LPTSTR lpFileName;
	HINSTANCE hInstance;
	RECT rect;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);

		GetClientRect(hWnd,&rect); // �õ����ڵĴ�С
		// Add By Jami chen in 2004.07.20
		FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(BLACK_BRUSH));  // ��ձ���
		// Add By Jami chen in 2004.07.20
		// ���ȹر�ԭ������ʾ
		if (lpIMGBrowser->hGif)
		{  //����GIF���
			Gif_Destroy(lpIMGBrowser->hGif);
			lpIMGBrowser->hGif = NULL;
		}
		if (lpIMGBrowser->hBitmap)
		{  //����BMP���
			DeleteObject(lpIMGBrowser->hBitmap);
			lpIMGBrowser->hBitmap = NULL;
		}
		lpIMGBrowser->uFileType = 0; //��ǰû�д��κε�ͼ��

#ifdef LINUX_KERNEL
	lpFileName = "./kingmos/mms.gif";
#else
	lpFileName = "\\kingmos\\mms.gif";
#endif

	if (lpFileName == NULL)
	{
		//����Ҫ���κ�ͼ��
		return 0;
	}
	
	lpIMGBrowser->hGif = Gif_LoadFromFile( lpFileName ) ; // װ��GIFͼ��
	if (lpIMGBrowser->hGif != NULL)
	{
		// �ɹ�װ��
		lpIMGBrowser->iIndex = 0; // gifͼ��ʹ�ã���ǰ���ڲ��ŵ�ͼ������
		lpIMGBrowser->iDelayTime = 0; // gif ͼ��ʹ�ã�������һ������ͼ����Ҫ�ȴ���ʱ��
		lpIMGBrowser->iImageNum = GetTotalPic(lpIMGBrowser->hGif); // �õ���ǰGIF��ͼ������
		GetGIFImageSize(lpIMGBrowser->hGif,&lpIMGBrowser->Size);
		lpIMGBrowser->point.x = ((rect.right - rect.left) - lpIMGBrowser->Size.cx ) /2; // ��ʾͼ���λ��
		lpIMGBrowser->point.y = ((rect.bottom - rect.top) - lpIMGBrowser->Size.cy ) /2 ; // ��ʾͼ���λ��
		ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // ����GIF�ļ�
	}
	InvalidateRect(hWnd,NULL,TRUE); // ˢ�»���
	return TRUE;
}


static LRESULT OnGifPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
	RECT rect;

	GetClientRect(hWnd,&rect);
	hdc = BeginPaint( hWnd, &ps );  // ��ʼ����

	// ���ڴ�DC�еĻ���ˢ�µ���Ļ
	BitBlt(hdc,0,0,rect.right,rect.bottom,lpIMGBrowser->hMemoryDC,0,0,SRCCOPY);

//	ShowCurText(hWnd,hdc); //��Ҫ��ʾ���ı���ʾ����Ļ

	EndPaint( hWnd, &ps ); // ��������
	return 0;
}

static LRESULT DoGifCreate(HWND hWnd)
{
	lpIMGBrowser = (LPIMGBROWSER)malloc(sizeof(IMGBROWSER)); // ����IMG������ṹָ��
	if (lpIMGBrowser == NULL)
	{  // �����ڴ�ʧ��,����ʧ��
		return -1;
	}
	
	lpIMGBrowser->hMemoryBmp = NULL;
	lpIMGBrowser->hMemoryDC = CreateMemoryDC(hWnd,&lpIMGBrowser->hMemoryBmp); // �����ڴ�DC
	if (lpIMGBrowser->hMemoryDC == NULL)
	{
		// �����ڴ�DCʧ��
		return -1;
	}

	lpIMGBrowser->uFileType = 0; // ��ǰ��ʾ��ͼ������
	lpIMGBrowser->hGif = NULL ; // Gif ͼ��ľ��
	lpIMGBrowser->hBitmap = NULL ; // bitmap , jpeg ͼ��ľ��

	lpIMGBrowser->iIndex = 0 ; // gifͼ��ʹ�ã���ǰ���ڲ��ŵ�ͼ������
	lpIMGBrowser->iDelayTime = 0 ; // gif ͼ��ʹ�ã�������һ������ͼ����Ҫ�ȴ���ʱ��
	lpIMGBrowser->iImageNum = 0 ; //gif ͼ��ʹ�ã���ǰһ���ж��ٷ�ͼ��

	lpIMGBrowser->point.x = 0 ; // ��ʾͼ���λ��
	lpIMGBrowser->point.y = 0 ; // ��ʾͼ���λ��

	lpIMGBrowser->cl_Text = GetSysColor(COLOR_WINDOWTEXT);
	lpIMGBrowser->cl_BkColor = GetSysColor(COLOR_WINDOW);
	//SetWindowLong(hWnd,0,(LONG)lpIMGBrowser);

	lpIMGBrowser->hDrawGifThread = NULL;
	lpIMGBrowser->bExit = FALSE;
	lpIMGBrowser->hDrawGifThread = CreateThread(NULL, 0, ShowImageThread, (void *)hWnd, 0, 0 );  // �������ع����߳�
	return 0;
}
static DWORD WINAPI ShowImageThread(VOID * pParam)
{
	HWND hWnd;
	
		hWnd = (HWND)pParam;  // �õ�����
		if (hWnd == NULL)
			return FALSE;

		while(1)
		{
			if (lpIMGBrowser->bExit == TRUE) 
				break; // �߳�Ҫ�˳�
			if (lpIMGBrowser->iImageNum > 1)
			{
				lpIMGBrowser->iDelayTime -= TIMEINTERVAL; // ��ȥ�Ѿ��ȴ���ʱ��
				if (lpIMGBrowser->iDelayTime <= 0)
				{
					ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // ����GIF�ļ�
					InvalidateRect(hWnd,NULL,TRUE); // ˢ�»���
				}
			}
			Sleep(TIMEINTERVAL);
		}

		lpIMGBrowser->bExit = FALSE; // �˳��̳߳ɹ�
		SendMessage(hWnd,WM_USERGIF,0,0);
		return 0;	
}

static LRESULT DoGifReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		if (lpIMGBrowser->hDrawGifThread)
		{ // ������һ���滭�߳�
			int iWaitTime = 2000;
			lpIMGBrowser->bExit = TRUE; // Ҫ���߳��˳�
			Sleep(TIMEINTERVAL);
			// �ȴ��߳��˳���2�����̻߳�û���˳�������ǿ���˳�
			while(1)
			{
				if (lpIMGBrowser->bExit == FALSE)
					break; // �߳��Ѿ��˳�
				if (iWaitTime <= 0)
					break; // ʱ�䵽
				iWaitTime -= TIMEINTERVAL;
				Sleep(TIMEINTERVAL);
			}
			CloseHandle(lpIMGBrowser->hDrawGifThread);
		}
		// �ͷ�����
		// ���ȹر�ԭ������ʾ
		if (lpIMGBrowser->hGif)
		{  //����GIF���
			Gif_Destroy(lpIMGBrowser->hGif);
			lpIMGBrowser->hGif = NULL;
		}
		if (lpIMGBrowser->hBitmap)
		{  //����BMP���
			DeleteObject(lpIMGBrowser->hBitmap);
			lpIMGBrowser->hBitmap = NULL;
		}
		lpIMGBrowser->uFileType = 0; //��ǰû�д��κε�ͼ��

		if (lpIMGBrowser->hMemoryDC)
			DeleteDC(lpIMGBrowser->hMemoryDC);

		if (lpIMGBrowser->hMemoryBmp)
			DeleteObject(lpIMGBrowser->hMemoryBmp);

		free(lpIMGBrowser);		// �ͷ�����ṹ
		lpIMGBrowser = NULL;
		return TRUE;
}

static void ShowCurImage(HWND hWnd, HDC hdc)
{
		if (lpIMGBrowser->hGif == NULL)
			return ;
		//��Ҫ��ʾ
		DrawGifImage(hWnd,hdc,lpIMGBrowser->hGif,lpIMGBrowser->iIndex,&lpIMGBrowser->point);


		if (lpIMGBrowser->iImageNum > 1)
		{	//  ֻ�г���һ��ͼ���ǲ���Ҫ�趨��һ��ͼ�����ʾʱ��
			lpIMGBrowser->iDelayTime = GetNextPicTime(lpIMGBrowser->hGif,lpIMGBrowser->iIndex); // �õ�����һ��ͼ���ʱ��
			lpIMGBrowser->iIndex ++; // ��һ��ͼ��
			if (lpIMGBrowser->iIndex >= lpIMGBrowser->iImageNum)
			{
				lpIMGBrowser->iIndex = 0; // �Ѿ������һ��ͼ��, �´ο�ʼ��ʾ��һ��ͼ��
				lpIMGBrowser->bExit = TRUE;//�˳�
			}
		}

}
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // ���ýṹ��С
		//�õ�GIF�������Ϣ
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
		{
			// �õ���Ϣ�ɹ�
			lpImgSize->cx = gifInfo.nWidth;
			lpImgSize->cy = gifInfo.nHeight;
			return TRUE; // ���ص�ǰGIF��ͼ����
		}
		return FALSE; // û�еõ�ͼ����
}

static int GetTotalPic(HGIF hGif)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // ���ýṹ��С
		//�õ�GIF�������Ϣ
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
			return gifInfo.nIndexNum; // ���ص�ǰGIF��ͼ����
		return 0; // û�еõ�ͼ����
}
static HDC CreateMemoryDC(HWND hWnd, HBITMAP *lpNewBitmap)
{
	HDC hdc;
	HDC hMemDC;
	HBITMAP hMemBitMap;
	RECT rect;
		
		if (lpNewBitmap == NULL )
			return NULL;
		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		hdc = GetDC(hWnd); // �õ��豸���
		hMemDC=CreateCompatibleDC(hdc); // ���������ڴ��豸���
		hMemBitMap=CreateCompatibleBitmap(hdc,(rect.right-rect.left),(rect.bottom-rect.top)); // ��������λͼ
		ReleaseDC(hWnd,hdc); /// �ͷ��豸���
		if (hMemBitMap == NULL)
		{ // ��������λͼʧ��
			DeleteDC(hMemDC);
			hMemDC = NULL;
			return NULL;
		}
		*lpNewBitmap = hMemBitMap;  // �ڴ�DC�е��ڴ�λͼ��Ҫ���Լ�ɾ��
		hMemBitMap = SelectObject(hMemDC,hMemBitMap); // ѡ��λͼ���ڴ��豸���

		FillRect(hMemDC,&rect,GetStockObject(WHITE_BRUSH));  // ������
		return hMemDC; // ���سɹ��������ڴ�DC
}
static int GetNextPicTime(HGIF hGif,int iIndex)
{
	DWORD dwDelay;
	Gif_GetDelay(hGif,iIndex, &dwDelay ); // �õ���һ��ͼ��ʱ������

	return (dwDelay * 10 ) - 20; // Delay ��0.01s Ϊ��λ��ת����MS
}
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
{
		// ���Ƶ�ǰָ��������ͼ��
		Gif_DrawIndex(hGif,iIndex,hdc,ptOrg->x,ptOrg->y);
}

