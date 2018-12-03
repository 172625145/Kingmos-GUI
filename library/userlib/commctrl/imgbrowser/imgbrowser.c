/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����ͼ�������
�汾�ţ�1.0.0
����ʱ�ڣ�2004-06-07
���ߣ��½��� Jami chen
�滮��
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
#include <jpegapi.h>
#include <gif2bmp.h>
#include <imgbrowser.h>
#include <ecomctrl.h>

/***************  ȫ���� ���壬 ���� *****************/

//char   classIMGBrowser[20] = TEXT("IMGBROWSER");

#define USETHREADSHOWGIF

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

#ifdef USETHREADSHOWGIF
	HANDLE hDrawGifThread;
	BOOL bExit;
#endif
}IMGBROWSER, * LPIMGBROWSER;


#define ID_GIFSHOW	100

/******************************************************/
// ����������
/******************************************************/
ATOM RegisterIMGBrowserClass( HINSTANCE hInstance );
static LRESULT CALLBACK IMGBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void OnIMGBrowserPaint( HWND hWnd );
static LRESULT DoIMGBrowserCreate(HWND hWnd);
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetIMGFile(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);

static UINT GetImgFileType(LPTSTR lpFileName);
static void SendActiveNotify(HWND hWnd,UINT code,POINT pt);


static int GetTotalPic(HGIF hGif);
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize);
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg);
//static void ShowCurImage(HWND hWnd);
static void ShowCurImage(HWND hWnd,HDC hdc);

static int GetNextPicTime(HGIF hGif,int iIndex);

static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize);
static void DrawBitMap(HWND hWnd,HDC hdc,HBITMAP hBitmap,int x,int y);

static void ShowCurText(HWND hWnd,HDC hdc);
//static HDC CreateMemoryDC(HWND hWnd);
static HDC CreateMemoryDC(HWND hWnd, HBITMAP *lpNewBitmap);


static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);

#ifdef USETHREADSHOWGIF
static DWORD WINAPI ShowImageThread(VOID * pParam);
#endif
// ********************************************************************
// ������ATOM RegisterIMGBrowserClass( HINSTANCE hInstance );
// ������
//	IN hInstance - ����ϵͳ��ʵ�����
// ����ֵ��
// 	��
// ����������ע��ϵͳ������
// ����: 
// ********************************************************************
ATOM RegisterIMGBrowserClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)IMGBrowserWndProc;  // ������̺���
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classIMGBrowser;
	return RegisterClass(&wcex); // ��ϵͳע��������
}

// ********************************************************************
// ������static LRESULT CALLBACK IMGBrowserWndProc(HWND , UINT , WPARAM , LPARAM )
// ������
//	IN hWnd- ����ϵͳ�Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��
// ����������ϵͳ���洰�ڹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK IMGBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // �滭��Ϣ
			OnIMGBrowserPaint( hWnd );
			return 0;
		case WM_CREATE: // ������Ϣ
			return DoIMGBrowserCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			DoReleaseControl(hWnd,wParam,lParam);
			break;
		case WM_COMMAND:
			DoCommand(hWnd,wParam,lParam);
			break;
	   	case WM_LBUTTONDOWN: // ����������
			return DoLButtonDown(hWnd,wParam,lParam);
   		case WM_LBUTTONUP: // ����������
			return DoLButtonUp(hWnd,wParam,lParam);

		case WM_TIMER:
			return DoTimer(hWnd,wParam,lParam);
		case WM_SETTEXT:
			DefWindowProc( hWnd, message, wParam, lParam );
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;

		case WM_SETCTLCOLOR	: // ���ÿ�����ɫ
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // �õ�������ɫ
			return DoGetColor(hWnd,wParam,lParam);

		case IMB_SETIMGFILE:  //����Ҫ��ʾ��ͼ���ļ�
			return DoSetIMGFile(hWnd,wParam,lParam);
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // ����Ĭ����Ϣ
   }
   return FALSE;
}

// ********************************************************************
// ������static void OnIMGBrowserPaint( HWND hWnd )
// ������
//	IN hWnd- ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//	��
// ��������������ϵͳ���洰�ڻ滭����
// ����: 
// ********************************************************************
static void OnIMGBrowserPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
	LPIMGBROWSER lpIMGBrowser;
	RECT rect;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		ASSERT(lpIMGBrowser);

		GetClientRect(hWnd,&rect);
		hdc = BeginPaint( hWnd, &ps );  // ��ʼ����

//		FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // ������

//		if (lpIMGBrowser->hBitmap) // ����bitmap
//			DrawBitMap(hWnd,lpIMGBrowser->hMemoryDC,lpIMGBrowser->hBitmap,lpIMGBrowser->point.x,lpIMGBrowser->point.y);  // ����λͼ�ļ�
//		if (lpIMGBrowser->hGif) // ����gifͼ��
//			ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // ����GIF�ļ�

//		ShowCurText(hWnd,lpIMGBrowser->hMemoryDC);

		// ���ڴ�DC�еĻ���ˢ�µ���Ļ
		BitBlt(hdc,0,0,rect.right,rect.bottom,lpIMGBrowser->hMemoryDC,0,0,SRCCOPY);

		ShowCurText(hWnd,hdc); //��Ҫ��ʾ���ı���ʾ����Ļ

		EndPaint( hWnd, &ps ); // ��������
}


// ********************************************************************
// ������static LRESULT DoIMGBrowserCreate(HWND hWnd)
// ������
//	IN	hWnd -- ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//		���ش����Ľ��
// �����������������洴����Ϣ
// ����: 
// ********************************************************************
static LRESULT DoIMGBrowserCreate(HWND hWnd)
{
	LPIMGBROWSER lpIMGBrowser;


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
		SetWindowLong(hWnd,0,(LONG)lpIMGBrowser);

#ifdef USETHREADSHOWGIF
		lpIMGBrowser->hDrawGifThread = NULL;
		lpIMGBrowser->bExit = FALSE;
		lpIMGBrowser->hDrawGifThread = CreateThread(NULL, 0, ShowImageThread, (void *)hWnd, 0, 0 );  // �������ع����߳�
#endif
		return 0;
}


// **************************************************
// ������static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ����
// ��������������������Ϣ��
// ����: 
// **************************************************
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int wmId, wmEvent;

		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		switch (wmId)
		{
			default:
			   return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
		}

		return 0;
}



// **************************************************
// ������static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ����
// �����������ͷſؼ��ڴ档
// ����: 
// **************************************************
static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPIMGBROWSER lpIMGBrowser;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		ASSERT(lpIMGBrowser);

#ifdef USETHREADSHOWGIF
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
#else
		KillTimer(hWnd,ID_GIFSHOW); //ֹͣ��ʱ��
#endif
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
		return TRUE;
}



/**************************************************
������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(X,Y),�������
����ֵ����
��������������WM_LBUTTONDOWN��Ϣ��
����: 
************************************************/
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;

		point.x=LOWORD(lParam);
		point.y=HIWORD(lParam);
		return 0;
}


/**************************************************
������static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(X,Y),�������
����ֵ����
��������������WM_LBUTTONUP��Ϣ��
����: 
************************************************/
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;
	LPIMGBROWSER lpIMGBrowser;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		ASSERT(lpIMGBrowser);

		point.x=LOWORD(lParam);  // �õ���ǰ��
		point.y=HIWORD(lParam);

		SendActiveNotify(hWnd,NMIMG_CLICK,point);
		return 0;
}




/**************************************************
������static LRESULT DoSetIMGFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- UINT ͼ������
	IN lParam -- LPTSTR Ҫ��ʾ���ļ���
����ֵ����
��������������һ��ͼ���ļ�����ʾ,���Ϊ��,�����ԭ������ʾ��
����: 
************************************************/
static LRESULT DoSetIMGFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT uFileType;
	LPTSTR lpFileName;
	LPIMGBROWSER lpIMGBrowser;
	HINSTANCE hInstance;
	RECT rect;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		ASSERT(lpIMGBrowser);

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);

		GetClientRect(hWnd,&rect); // �õ����ڵĴ�С
		// Add By Jami chen in 2004.07.20
		FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // ��ձ���
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

		uFileType = (UINT)wParam;
		lpFileName = (LPTSTR)lParam;

		if (lpFileName == NULL)
		{
			//����Ҫ���κ�ͼ��
			return 0;
		}
		if (uFileType == IMG_UNKNOW)
		{
			uFileType = GetImgFileType(lpFileName);
		}
		
		switch(uFileType)
		{
			case IMG_BMPFILE:
				lpIMGBrowser->hBitmap = LoadImage( hInstance, lpFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��Ҫ��ʾ��λͼ
				GetBMPImageSize(lpIMGBrowser->hBitmap,&lpIMGBrowser->Size);
				lpIMGBrowser->point.x = ((rect.right - rect.left) - lpIMGBrowser->Size.cx ) /2; // ��ʾͼ���λ��
				lpIMGBrowser->point.y = ((rect.bottom - rect.top) - lpIMGBrowser->Size.cy ) /2 ; // ��ʾͼ���λ��
				// ��λͼ��ʾ���ڴ�DC
//				FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // ������

				if (lpIMGBrowser->hBitmap) // ����bitmap
					DrawBitMap(hWnd,lpIMGBrowser->hMemoryDC,lpIMGBrowser->hBitmap,lpIMGBrowser->point.x,lpIMGBrowser->point.y);  // ����λͼ�ļ�
//				InvalidateRect(hWnd,NULL,TRUE); // ˢ�»���
				break;
			case IMG_GIFFILE:
				lpIMGBrowser->hGif = Gif_LoadFromFile( lpFileName ) ; // װ��GIFͼ��
				if (lpIMGBrowser->hGif != NULL)
				{
					// �ɹ�װ��
					lpIMGBrowser->iIndex = 0; // gifͼ��ʹ�ã���ǰ���ڲ��ŵ�ͼ������
					lpIMGBrowser->iDelayTime = 0; // gif ͼ��ʹ�ã�������һ������ͼ����Ҫ�ȴ���ʱ��
					lpIMGBrowser->iImageNum = GetTotalPic(lpIMGBrowser->hGif); // �õ���ǰGIF��ͼ������
//					SetTimer(hWnd,ID_GIFSHOW,1,NULL);
					GetGIFImageSize(lpIMGBrowser->hGif,&lpIMGBrowser->Size);
					lpIMGBrowser->point.x = ((rect.right - rect.left) - lpIMGBrowser->Size.cx ) /2; // ��ʾͼ���λ��
					lpIMGBrowser->point.y = ((rect.bottom - rect.top) - lpIMGBrowser->Size.cy ) /2 ; // ��ʾͼ���λ��
//					if (lpIMGBrowser->iImageNum > 1)
//						DoTimer(hWnd,0,0);  // ֻ����GIF��ͼ�����һ��ʱ����Ҫ��ʾ����
//					ShowCurImage(hWnd, hdc); // ����GIF�ļ�
//					FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // ������
					ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // ����GIF�ļ�
//					ShowCurText(hWnd, hdc);
//					InvalidateRect(hWnd,NULL,TRUE); // ˢ�»���
				}
				break;
			case IMG_JPGFILE:
				lpIMGBrowser->hBitmap = JPEG_LoadByName(lpFileName,NULL);  // װ��JPEGͼ��
				if (lpIMGBrowser->hBitmap)
				{ // װ��λͼ�ɹ�
					GetBMPImageSize(lpIMGBrowser->hBitmap,&lpIMGBrowser->Size);
					//RETAILMSG(1,(TEXT("Bitmap Size [%d,%d]\r\n"),lpIMGBrowser->Size.cx,lpIMGBrowser->Size.cy));
					lpIMGBrowser->point.x = ((rect.right - rect.left) - lpIMGBrowser->Size.cx ) /2; // ��ʾͼ���λ��
					lpIMGBrowser->point.y = ((rect.bottom - rect.top) - lpIMGBrowser->Size.cy ) /2 ; // ��ʾͼ���λ��
					// ��λͼ��ʾ���ڴ�DC
//					FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // ������

					if (lpIMGBrowser->hBitmap) // ����bitmap
						DrawBitMap(hWnd,lpIMGBrowser->hMemoryDC,lpIMGBrowser->hBitmap,lpIMGBrowser->point.x,lpIMGBrowser->point.y);  // ����λͼ�ļ�
				}
				break;
		}
		InvalidateRect(hWnd,NULL,TRUE); // ˢ�»���
		return TRUE;
}


/**************************************************
������static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ��Ϣ����
����ֵ����
��������������WM_LBUTTONUP��Ϣ��
����: 
************************************************/
static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	LPIMGBROWSER lpIMGBrowser;
	RECT rect;
	
		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		ASSERT(lpIMGBrowser);

		GetClientRect(hWnd,&rect);

		KillTimer(hWnd,ID_GIFSHOW); //ֹͣ��ʱ��
		hdc = GetDC(hWnd);  // �õ��豸���
//		ShowCurImage(hWnd,hdc); // ����GIF�ļ�
		ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // ����GIF�ļ�

		// ���ڴ�DC�еĻ���ˢ�µ���Ļ
		BitBlt(hdc,0,0,rect.right,rect.bottom,lpIMGBrowser->hMemoryDC,0,0,SRCCOPY);
		ShowCurText(hWnd,hdc);
		ReleaseDC(hWnd,hdc); //�ͷ��豸�����
		
	return 0;
}
// **************************************************
// ������static void ShowCurImage(HWND hWnd, HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
//  IN hdc  -- �豸���
// ����ֵ����
// ������������ʾ��ǰ������GIFͼ��
// ����: 
// ************************************************
static void ShowCurImage(HWND hWnd, HDC hdc)
{
	LPIMGBROWSER lpIMGBrowser;
	
//	POINT ptOrg;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		ASSERT(lpIMGBrowser);

		if (lpIMGBrowser->hGif == NULL)
			return ;
		//��Ҫ��ʾ
//		GetControlPos(lpCurItem->hControl,&ptControl);
//		ptOrg.x = ptControl.x - lpIMGBrowser->point.x;
//		ptOrg.y = ptControl.y - lpIMGBrowser->point.y;
		DrawGifImage(hWnd,hdc,lpIMGBrowser->hGif,lpIMGBrowser->iIndex,&lpIMGBrowser->point);


		if (lpIMGBrowser->iImageNum > 1)
		{	//  ֻ�г���һ��ͼ���ǲ���Ҫ�趨��һ��ͼ�����ʾʱ��
			lpIMGBrowser->iDelayTime = GetNextPicTime(lpIMGBrowser->hGif,lpIMGBrowser->iIndex); // �õ�����һ��ͼ���ʱ��
			lpIMGBrowser->iIndex ++; // ��һ��ͼ��
			if (lpIMGBrowser->iIndex >= lpIMGBrowser->iImageNum)
				lpIMGBrowser->iIndex = 0; // �Ѿ������һ��ͼ��, �´ο�ʼ��ʾ��һ��ͼ��

#ifndef USETHREADSHOWGIF
			SetTimer(hWnd,ID_GIFSHOW,lpIMGBrowser->iDelayTime,NULL);  // �趨��һ��ͼ�����ʾʱ��
#endif
		}

}

// **************************************************
// ������static int GetTotalPic(HGIF hGif)
// ������
// 	IN hGif --	GIF�ļ����
// 
// ����ֵ�����ص�ǰ��GIF��ͼ�����
// ���������� �õ���ǰ��GIF��ͼ�����.
// ����: 
// **************************************************
static int GetTotalPic(HGIF hGif)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // ���ýṹ��С
		//�õ�GIF�������Ϣ
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
			return gifInfo.nIndexNum; // ���ص�ǰGIF��ͼ����
		return 0; // û�еõ�ͼ����
}

// **************************************************
// ������static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN hGif -- Ҫ��ʾ��ͼ����
//  IN iIndex -- Ҫ��ʾ��ͼ������
//  IN ptOrg -- ͼ��ʼ��ʾλ��
// 
// ����ֵ����
// ��������������һ��ͼ�ξ����ʾͼ��
// ����: 
// **************************************************
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
{
//	GIF_IMAGE gifimage;

//		gifimage.dwSize = sizeof(GIF_IMAGE);
//		Gif_GetImage(hGif,iIndex,&gifimage); // �õ���ǰ����ͼ�����Ϣ
		// ���Ƶ�ǰָ��������ͼ��
//		Gif_DrawIndex(hGif,iIndex,hdc,ptOrg->x,ptOrg->y);

//		gifimage.dwSize = sizeof(GIF_IMAGE);
//		Gif_GetImage(hGif,iIndex,&gifimage); // �õ���ǰ����ͼ�����Ϣ
		// ���Ƶ�ǰָ��������ͼ��
		Gif_DrawIndex(hGif,iIndex,hdc,ptOrg->x,ptOrg->y);
}

// **************************************************
// ������static int GetNextPicTime(HGIF hGif,int iIndex)
// ������
// 	IN hGif --	GIF�ļ����
//  IN iIndex -- ��ǰ��ͼ�������
// 
// ����ֵ�����ص�ǰͼ����һ��ͼ��ļ��ʱ��
// ���������� �õ���ǰͼ����һ��ͼ��ļ��ʱ��.
// ����: 
// **************************************************
static int GetNextPicTime(HGIF hGif,int iIndex)
{
	DWORD dwDelay;
	Gif_GetDelay(hGif,iIndex, &dwDelay ); // �õ���һ��ͼ��ʱ������

	return (dwDelay * 10 ) - 20; // Delay ��0.01s Ϊ��λ��ת����MS
}

// **************************************************
// ������static void DrawBitMap(HWND hWnd,HDC hdc,HBITMAP hBitmap,int x,int y)
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN x -- ��ʾλͼ��xλ��
// 	IN y -- ��ʾλͼ��yλ��
// 
// ����ֵ��
//		��	
// ��������������λͼ��
// ����: 
// **************************************************
static void DrawBitMap(HWND hWnd,HDC hdc,HBITMAP hBitmap,int x,int y)
{
	HDC hMemoryDC;
    HBITMAP hOldBitmap;
	SIZE ImgSize;
	DWORD dwStyle;
	RECT rect;

		if (hBitmap == NULL)
			return ; // û��λͼ��Ҫ��ʾ

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		GetBMPImageSize(hBitmap, &ImgSize);

		hMemoryDC=CreateCompatibleDC(hdc); // �����ڴ���
//		RETAILMSG(1,("***********************\r\n"));
//		RETAILMSG(1,("hdc = %X \r\n"),hdc);
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // ��λͼѡ���ڴ���

		if (dwStyle & IBS_IMGSTRETCH)
		{
			// ��Ҫ����
				GetClientRect(hWnd,&rect);
				// ��ʾλͼ
				StretchBlt( hdc, // handle to destination device context
					(short)0,  // x-coordinate of destination rectangle's upper-left
											 // corner
					(short)0,  // y-coordinate of destination rectangle's upper-left
											 // corner
					rect.right - rect.left , //	 �������
					rect.bottom - rect.top , //  �����߶�
					hMemoryDC,  // handle to source device context
					(short)0,   // x-coordinate of source rectangle's upper-left
											 // corner
					(short)0,   // y-coordinate of source rectangle's upper-left
											 // corner
					(short)ImgSize.cx,  // width of IMAGE rectangle
					(short)ImgSize.cy, // height of IMAGE rectangle
					SRCCOPY
					);
		}
		else
		{
			// ����Ҫ����
				// ��ʾλͼ
				BitBlt( hdc, // handle to destination device context
					(short)x,  // x-coordinate of destination rectangle's upper-left
											 // corner
					(short)y,  // y-coordinate of destination rectangle's upper-left
											 // corner
					(short)ImgSize.cx,  // width of destination rectangle
					(short)ImgSize.cy, // height of destination rectangle
					hMemoryDC,  // handle to source device context
					(short)0,   // x-coordinate of source rectangle's upper-left
											 // corner
					(short)0,   // y-coordinate of source rectangle's upper-left
											 // corner

					SRCCOPY
					);
		}

//		RETAILMSG(1,("hMemoryDC = %X , hOldBitmap = %X \r\n",hMemoryDC,hOldBitmap));
		SelectObject(hMemoryDC,hOldBitmap); // �ָ��ڴ��豸�����λͼ���
		DeleteDC(hMemoryDC); // ɾ���ڴ���
}


// **************************************************
// ������static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize)
// ������
// 	IN hBmp --	BMP�ļ����
//  OUT lpImgSize -- ����ͼ���С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ���������� �õ���ǰ��BMP��ͼ���С.
// ����: 
// **************************************************
static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize)
{
	BITMAP bitmap;

		//װ�سɹ�
		if (GetObject(hBmp,sizeof(BITMAP),&bitmap))
		{
			//�õ���Ϣ�ɹ�
			lpImgSize->cx = bitmap.bmWidth;
			lpImgSize->cy = bitmap.bmHeight;
//			RETAILMSG(1,(TEXT("Bitmap Size [%d,%d]\r\n"),bitmap.bmWidth,bitmap.bmHeight));
			return TRUE; //  ���سɹ�
		}
		return FALSE; // û�еõ�ͼ����
}



// **************************************************
// ������static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize)
// ������
// 	IN hGif --	GIF�ļ����
//  OUT lpImgSize -- ����ͼ���С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ���������� �õ���ǰ��GIF��ͼ���С.
// ����: 
// **************************************************
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




// **************************************************
// ������static UINT GetImgFileType(LPTSTR lpFileName)
// ������
// 	IN lpFileName -- Ҫ�ж�ͼ�����͵��ļ���
// 
// ����ֵ����
// �����������õ��ļ����͡�
// ����: 
// **************************************************

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

const static FILETYPE tabFileType[] = {
	{ "*.bmp" , IMG_BMPFILE },
	{ "*.jpg" , IMG_JPGFILE },
	{ "*.gif" , IMG_GIFFILE },
};

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );;

static UINT GetImgFileType(LPTSTR lpFileName)
{

	int iFileTypeNum,i;

		iFileTypeNum = sizeof (tabFileType) / sizeof (FILETYPE); // �õ���ǰ����֪������Ŀ
		for (i = 0 ; i < iFileTypeNum; i++ )
		{
			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), lpFileName, strlen(lpFileName)) == TRUE)
			{ // �Ƚϵ�ǰ�ļ��Ƿ�����֪�ļ�
				return tabFileType[i].iFileType;  // ʹ����֪�ļ���ͼ��
			}
		}
		return 0;
}


/**************************************************
������static void SendActiveNotify(HWND hWnd,UINT code,POINT pt)
������
	IN hWnd -- ���ھ��
	IN code -- Ҫ���͵�֪ͨ��Ϣ����
	IN pt -- ��ǰ����λ��
����ֵ����
��������������һ��֪ͨ��Ϣ��������
����: 
************************************************/
static void SendActiveNotify(HWND hWnd,UINT code,POINT pt)
{
	NMIMGBROWSER nmImg;
	HWND hParent;

		// ���ýṹ����
		nmImg.hdr.hwndFrom=hWnd;
		nmImg.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmImg.hdr.code=code;
		
		nmImg.ptAction=pt;
		hParent=GetParent(hWnd); // �õ������ھ��
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmImg); // ֪ͨ������
}


/**************************************************
������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTLCOLORSTRUCT ָ��CTLCOLORSTRUCT�ṹ��ָ��
����ֵ����
�������������ÿؼ���ɫ������WM_SETCTLCOLOR��Ϣ��
����: 
************************************************/
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
	LPIMGBROWSER lpIMGBrowser;

		 lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		 ASSERT(lpIMGBrowser);
		
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �����ı���ɫ
			 lpIMGBrowser->cl_Text = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �����ı�����ɫ
				lpIMGBrowser->cl_BkColor = lpCtlColor->cl_TextBk;
		 }
		 InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
/**************************************************
������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTLCOLORSTRUCT ָ��CTLCOLORSTRUCT�ṹ��ָ��
����ֵ����
�����������õ��ؼ���ɫ������WM_GETCTLCOLOR��Ϣ��
����: 
************************************************/
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPIMGBROWSER lpIMGBrowser;

		 lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		 ASSERT(lpIMGBrowser);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // �õ��ı���ɫ
			 lpCtlColor->cl_Text = lpIMGBrowser->cl_Text ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �õ��ı�����ɫ
				lpCtlColor->cl_TextBk = lpIMGBrowser->cl_BkColor ;
		 }
		 return TRUE;
}


// **************************************************
// ������static void ShowCurText(HWND hWnd, HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
//  IN hdc  -- �豸���
// ����ֵ����
// ������������ʾ��ǰϵͳ���ı���
// ����: 
// ************************************************
static void ShowCurText(HWND hWnd, HDC hdc)
{
	LPIMGBROWSER lpIMGBrowser;
	DWORD dwStyle;
	LPTSTR lpText;
	int iTextSize;
	RECT rect;
	DWORD dwDTStyle;

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);

		if (!(dwStyle & IBS_SHOWTEXT))
		{
			// ����Ҫ��ʾ�ı�
			return ;
		}
		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		ASSERT(lpIMGBrowser);

		iTextSize = GetWindowTextLength(hWnd);  // �õ��ı��ĳ���

		if (iTextSize <= 0)
		{
			// û���ı�
			return ;
		}
		
		lpText = (LPTSTR)malloc(iTextSize + 1); // ���䴰���ı��Ļ���

		if (lpText == NULL)
		{
			// ���仺��ʧ��
			return ;
		}

		GetWindowText(hWnd,lpText,iTextSize +1); // �õ��ı�

		GetClientRect(hWnd,&rect);  // �õ����ڿͻ���
		dwDTStyle = 0;

		if (dwStyle & IBS_TEXTLEFT)
		{  // �ı������
			dwDTStyle |= DT_LEFT;
		}
		else if (dwStyle & IBS_TEXTRIGHT)
		{  // �ı��Ҷ���
			dwDTStyle |= DT_RIGHT;
		}
		else
		{  // �ı�����
			dwDTStyle |= DT_CENTER;
		}

		if (dwStyle & IBS_SINGLELINE)
		{
			// ������ʾ
			if (dwStyle & IBS_TEXTTOP)
			{  // �ı��϶���
				dwDTStyle |= DT_TOP;
			}
			else if (dwStyle & IBS_TEXTBOTTOM)
			{  // �ı��¶���
				dwDTStyle |= DT_BOTTOM;
			}
			else
			{  // �ı�����
				dwDTStyle |= DT_VCENTER;
			}
		}
		else
		{
			// ������ʾ
			if (dwStyle & IBS_TEXTTOP)
			{  // �ı��϶���
				// ����Ҫ���κ��£�DrawText Ĭ�ϴ����濪ʼ��ʾ
			}
			else 
			{
				int iWindowHeight,iTextHeight;

				iWindowHeight = rect.bottom - rect.top; // ���ȵõ����ڸ߶�
				DrawText(hdc,lpText,-1,&rect,dwDTStyle | DT_CALCRECT); // ������ʾ�ı���Ҫ�ĸ߶�
				iTextHeight = rect.bottom - rect.top; // �õ��ı��߶�
				GetClientRect(hWnd,&rect);  // �õ����ڿͻ���
				if (dwStyle & IBS_TEXTBOTTOM)
				{  // �ı��¶���
					rect.top = (iWindowHeight - iTextHeight); // �õ���ʾ�ı��ľ��ζ���
				}
				else
				{  // �ı�����
					rect.top = (iWindowHeight - iTextHeight) / 2; // �õ���ʾ�ı��ľ��ζ���
				}

				rect.bottom = rect.top + iTextHeight; // �õ���ʾ�ı��ľ��εײ�
			}
		}

		if (dwStyle & IBS_TRANSPARENT)
		{
			// ͸����ʾ
			SetBkMode(hdc,TRANSPARENT);
		}
		
		SetTextColor(hdc,lpIMGBrowser->cl_Text);
		SetBkColor(hdc,lpIMGBrowser->cl_BkColor);
		
		// ��ʾ�ı�
		DrawText(hdc,lpText,-1,&rect,dwDTStyle); 

}

// ********************************************************************
// ������static HDC CreateMemoryDC(HWND hWnd, HBITMAP *lpNewBitmap)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
//	OUT lpNewBitmap -- �����´�����λͼ���
// ����ֵ��
//	
// ��������������һ���ڴ��豸���
// ����: 
// ********************************************************************
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

#ifdef USETHREADSHOWGIF

// **************************************************
// ������DWORD WINAPI ShowImageThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// ����������ͼ����ʾ�̡߳�
// ����: 
// **************************************************
static DWORD WINAPI ShowImageThread(VOID * pParam)
{
	HWND hWnd;
	LPIMGBROWSER lpIMGBrowser;
	
//		RETAILMSG( 1, ( "ShowImageThread ...\r\n" ) );
		hWnd = (HWND)pParam;  // �õ�����
		if (hWnd == NULL)
			return FALSE;
		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // �õ�IMG�����ṹָ��
		ASSERT(lpIMGBrowser);
//		RETAILMSG( 1, ( "lpIMGBrowser = %x \r\n",lpIMGBrowser ) );
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
		return 0;	
}
#endif
