/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����ϵͳ״̬��
�汾�ţ�1.0.0
����ʱ�ڣ�2005-09-21
���ߣ�����
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
#include "desktop.h"
/***************  ȫ���� ���壬 ���� *****************/

const char classStateBar[] = "STATE_BAR";
static HBITMAP hACPower;
static HBITMAP hdrHome;
static HBITMAP hBattery[4];



static LRESULT CALLBACK StateBarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// ********************************************************************
// ������static ATOM RegisterDesktopClass( HINSTANCE hInstance );
// ������
//	IN hInstance - ����ϵͳ��ʵ�����
// ����ֵ��
// 	��
// ����������ע��ϵͳ������
// ����: 
// ********************************************************************
static ATOM RegisterStateBarClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;	

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)StateBarWndProc;  // ������̺���
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject( BLACK_BRUSH );//
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classStateBar;
	return RegisterClass(&wcex); // ��ϵͳע��������
}

BOOL InitStateBar( HINSTANCE hInstance )
{
	RegisterStateBarClass( hInstance );
//#ifdef LINUX_KERNEL
	hdrHome = LoadImage( 0, "./kingmos/desktop/hdr_home.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
	hACPower = LoadImage( 0, "./kingmos/desktop/hdr_ac.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
	hBattery[0] = LoadImage( 0, "./kingmos/desktop/hdr_bttry0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
	hBattery[1] = LoadImage( 0, "./kingmos/desktop/hdr_bttry1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
	hBattery[2] = LoadImage( 0, "./kingmos/desktop/hdr_bttry2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
	hBattery[3] = LoadImage( 0, "./kingmos/desktop/hdr_bttry3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
    hBattery[4] = LoadImage( 0, "./kingmos/desktop/hdr_bttry4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
//#else
//	hdrHome = LoadImage( 0, ".\\kingmos\\hdr_home.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
//	hACPower = LoadImage( 0, ".\\kingmos\\hdr_ac.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
//	hBattery[0] = LoadImage( 0, ".\\kingmos\\hdr_bttry0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
//	hBattery[1] = LoadImage( 0, ".\\kingmos\\hdr_bttry1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
//	hBattery[2] = LoadImage( 0, ".\\kingmos\\hdr_bttry2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
//	hBattery[3] = LoadImage( 0, ".\\kingmos\\hdr_bttry3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
//	hBattery[4] = LoadImage( 0, ".\\kingmos\\hdr_bttry4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��λͼ
//#endif
	return TRUE;
}

// **************************************************
// ������static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN rect -- λͼ��С
// 	IN xOffset -- Xƫ��
// 	IN yOffset -- Yƫ��
// 	IN dwRop -- ����ģʽ
// 
// ����ֵ����
// ��������������λͼ��
// ����: 
// **************************************************
void DrawTransparentBitMap(HDC hdc,HBITMAP hBitmap, const RECT * lprect,UINT xOffset,UINT yOffset, DWORD clrTranspant )
{
		HDC hMemoryDC;
        HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // ����һ�����ݵ��ڴ���
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // ��Ҫ���Ƶ�λͼ���õ��ڴ�����
		TransparentBlt( hdc, // handle to destination device context
			(short)lprect->left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)lprect->top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			(short)xOffset,   // x-coordinate of source rectangle's upper-left
									 // corner
			(short)yOffset,   // y-coordinate of source rectangle's upper-left
									 // corner

			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			clrTranspant
			);

		SelectObject(hMemoryDC,hOldBitmap); // �ָ��ڴ���
		DeleteDC(hMemoryDC); // ɾ�����
}

// ********************************************************************
// ������static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc )
// ������
//	IN hWnd -- ����ϵͳ�Ĵ��ھ��
//	IN hdc -- ����ϵͳ�Ļ滭���
// ����ֵ��
//	��
// ��������������ϵͳ���洰�ڱ���ˢ�¹���
// ����: 
// ********************************************************************
/*
static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc )
{
	RECT rect;
	BITMAP bitmap;


	

	if (g_hDesktopBitmap == NULL)
	{  // û��װ��ǽֽ
//		RETAILMSG(1,("***** Start Load Bitmap File \r\n"));
		g_hDesktopBitmap = LoadImage( 0, lpDeskTopName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��ǽֽ
//		RETAILMSG(1,("***** Load Bitmap Success \r\n"));
		if( g_hDesktopBitmap == NULL )
		{  // װ��ǽֽʧ��
			return ;
		}
	}

	GetObject(g_hDesktopBitmap,sizeof(bitmap),&bitmap);  // �õ�λͼ�ṹ
	// �õ�λͼ��С
	rect.left = 0;
	rect.right = 640;
	rect.top = 0;
	rect.bottom = 480 ;
	
	DrawBitMap( hdc, g_hDesktopBitmap, rect, 0, 0 ,SRCCOPY);  // ����λͼ

}
*/
// ********************************************************************
// ������static void DoDraw( HWND hWnd, HDC hdc )
// ������
//	IN hWnd- ���ھ��
// ����ֵ��
//	��
// ���������������ڻ滭����
// ����: 
// ********************************************************************
static void DoDraw( HWND hWnd, HDC hdc )
{
	//�����
	const RECT rcBattery = { 570, 5, 570 + 65, 5 + 40 };
	const RECT rcHome = { 5, 5, 5 + 40, 5 + 40 };

	DrawTransparentBitMap( hdc, hdrHome, &rcHome, 0, 0, RGB(255, 0, 255) );
	DrawTransparentBitMap( hdc, hBattery[0], &rcBattery, 0, 0, RGB(255, 0, 255) );	
	
}

// ********************************************************************
// ������static void OnPaint( HWND hWnd )
// ������
//	IN hWnd- ���ھ��
// ����ֵ��
//	��
// ���������������ڻ滭����
// ����: 
// ********************************************************************
static void OnPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
	
	hdc = BeginPaint( hWnd, &ps );  // ��ʼ����
	DoDraw( hWnd, hdc ); 
	EndPaint( hWnd, &ps ); // ��������
}

// ********************************************************************
// ������static LRESULT CALLBACK DesktopWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK StateBarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // �滭��Ϣ
			OnPaint( hWnd );
			return 0;
		//case WM_ERASEBKGND:  // ���Ʊ���
			//OnDesktopEraseBkgnd( hWnd,( HDC )wParam );
		//	return 0;
/*		case WM_LBUTTONDOWN: // �����ť���£���TOUCH�ʰ���
			DoDesktopLButtonDown(hWnd,wParam,lParam);
			return 0;
		case WM_LBUTTONUP:// �����ť���𣬻�TOUCH���뿪
			DoDesktopLButtonUp(hWnd,wParam,lParam);
			return 0;
		case WM_TIMER: // ��ʱ��
			DoDesktopTimer(hWnd,wParam,lParam);
			return 0;
		case WM_WINDOWPOSCHANGING: // ����λ�øı�
			return DoWindowPosChanging(hWnd,wParam,lParam);
//            return 0;
		case WM_CREATE: // ������Ϣ
			DoDesktopCreate(hWnd);
			return 0;
		case WM_CLOSE:  // �ر���Ϣ
			DoDesktopClose(hWnd);
			return 0;
		case WM_DESTROY: // �ƻ���Ϣ
			PostQuitMessage(0);
			return 0;
*/
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // ����Ĭ����Ϣ
   }
   return FALSE;
}
