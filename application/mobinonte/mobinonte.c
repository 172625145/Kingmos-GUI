/*********************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
*********************************************************/

/*****************************************************
�ļ�˵����MobinonteӦ�ó���
�汾�ţ�0.0.1
����ʱ�ڣ�2005
���ߣ�liujunplus
�޸ļ�¼��
******************************************************/

////////// ͷ�ļ��� //////////
#include <ewindows.h>
#include <ecomctrl.h>

#include "mobinonte.h"

////////// defineԤ������ //////////
#define WM_RUNPROC		WM_USER+1
#define WM_CRTTHRD		WM_USER+2

#define MOBI_LIST		101		// Mobi�б�� ID

#define IDT_MOBI_TIMER	100		// MENU���붨ʱ��
////////// ȫ�ֱ����ļ��� //////////
// ȫ�ֱ���������
HWND hMainWnd = NULL;
GLOBAL_MOBI	g_Mobi;
		int	iMobiSel;
BOOL	bTimerDraw = FALSE;
BOOL	bListHide = FALSE;
static	BOOL	bCreateThread = FALSE;
static	BOOL	bLoadResourceOver = FALSE;
static	HBITMAP	hLoadBitmap = NULL;
static	CHOICE_MENU	myChoiceMenu[6];			// �����Ŀ¼������6��
static	const char g_classMOBI[] = "classMOBINONTE";
static	const char g_mainItem[ITEM_SEL_NUM][MAINMENU_SIZE] = { "videos", "music", "games", "travel", "aero", "set" };
static	RECT	rtMobiPaint = { 200, 20, 792, 56 };		// ���Ʊ�����δ�С
static	RECT	rtMobiText	= { 200, 34, 792, 56 };		// ���Ʊ�����δ�С

static	RECT	rtBitmapBkGrn= { 500,  99, 792, 430 };		// ��ϸͼƬ�İ�ɫ���α���
static	RECT	rtBitmapRect = { 534, 113, 744, 363 };		// ��ϸͼƬ�ľ���

static	RECT	rtRating = { 532, 321, 612, 341 };
static	RECT	rtPg	 = { 532, 342, 612, 362 };
static	RECT	rtPrice  = { 532, 374, 612, 394 };
static	RECT	rtValue  = { 532, 395, 750, 415 };
// ������ɫֵ
//��Mobi�������롾Menu��������ɫֵ
DWORD	dszBgColor[6]	= { RGB(255,169,0), RGB(255,126,0), RGB(255,104,136), RGB(159,109,202), RGB(96,124,220), RGB(71,149,205) };
//DWORD	dszBgColor[6]	= { RGB(255,0,0), RGB(0,0,255), RGB(0,0,255), RGB(255,255,0), RGB(255,0,255), RGB(0,255,255) };
//Menuǰ��
static	DWORD	dszMenuColor[6]	= { RGB(255,58,0),  RGB(255,50,0),  RGB(255,0,0),     RGB(136,0,179),   RGB(83,83,192),  RGB(3,104,192)  };

////////// �ⲿ���������� //////////
extern	const char classMOBILISTCTRL[14];
extern	ATOM RegisterMobiListCtrlClass(HINSTANCE hInstance);

////////// ���������� //////////
VOID	DoLoadBitmapAndShowIt( HWND hWnd, int iIndex );

//������������������������ ����ʼ ������������������������//
VOID	DoSpecListCmd( HWND hListCtrl )
{
	int			i;
	LV_ITEM		lvitem;

	for( i=5; i>=0; i-- )
	{
		memset( &lvitem,0,sizeof(lvitem) );

		lvitem.mask			= LVIF_TEXT|LVIF_PARAM;
		lvitem.iItem		= 0;
		lvitem.iSubItem		= 0;
		lvitem.pszText		= (LPSTR)g_mainItem[i];
		lvitem.lParam		= FILE_RIGHT;

		SendMessage( hListCtrl, LVM_INSERTITEM, 0, (LPARAM)&lvitem );
	}
}

// ��ʼ���б��
VOID InitDrawList( HWND hWnd )
{
	HINSTANCE	hInst;
	HWND		hListCtrl;
	CTLCOLORSTRUCT	stCtlColor;
	LV_COLUMN		lvcolumn;

	hInst = (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE );
	RegisterMobiListCtrlClass( hInst );
	hListCtrl = CreateWindow(classMOBILISTCTRL, "", LVS_NOCOLUMNHEADER|WS_VISIBLE|WS_CHILD|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_LIST|LVS_HORZLINE,
		-465, LIST_Y, LIST_W, LIST_H, hWnd, (HMENU)MOBI_LIST, hInst, NULL);	// ��������

	SendMessage( hListCtrl, LVM_SETCOLUMNWIDTH, 0, MAKELONG(44,10) ); // ����ͼ����Ŀ֮��ļ��

	// ������ɫ
	stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR;
	stCtlColor.cl_Text = GetSysColor(COLOR_WINDOWTEXT);
	stCtlColor.cl_TextBk = GetSysColor(COLOR_WINDOW);
	stCtlColor.cl_Selection = GetSysColor(COLOR_HIGHLIGHTTEXT);
	stCtlColor.cl_SelectionBk = GetSysColor(COLOR_HIGHLIGHT);
	SendMessage( hListCtrl, WM_SETCTLCOLOR, 0, (LPARAM)&stCtlColor );

	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH |LVCF_SUBITEM | LVCF_TEXT ;
	lvcolumn.fmt = LVCFMT_LEFT; // ��ͼ���ı������
	lvcolumn.pszText = "";
	lvcolumn.cchTextMax = 0;
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = LIST_ITEM_W;
	SendMessage( hListCtrl,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn );

	{
		DoSpecListCmd( hListCtrl );
		SendMessage( hListCtrl, LVM_SETSELECTIONMARK, 0, (LPARAM)0 );
	}
	EnableWindow( hListCtrl, FALSE );
}

// ��ͨ��ʼ��������ѭ����
DWORD WINAPI CommonSetMobinonteSource( LPVOID lpVoid )
{
	int			iTime, iSuccess=0;	// ѭ������
	HINSTANCE	hInst;
	HWND		hWnd = (HWND)lpVoid;

	hInst = (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE );

	for( iTime=1; iTime<ITEM_SEL_NUM; iTime++ )
	{
		strcpy( g_Mobi.strMobinonte[iTime].szTitle, g_mainItem[iTime] );
#ifdef LINUX_KERNEL
		sprintf( g_Mobi.strMobinonte[iTime].szBigBkground,	"./kingmos/flash/resource/%s/00%d.bmp", g_mainItem[iTime], 7 );		// ���ڴ󱳾�-------7
		sprintf( g_Mobi.strMobinonte[iTime].szMobinonte,	"./kingmos/flash/resource/%s/00%d.bmp", g_mainItem[iTime], 1 );		// MobinonteͼƬ----1
#else
		sprintf( g_Mobi.strMobinonte[iTime].szBigBkground,	"\\����\\flash\\resource\\%s\\00%d.bmp", g_mainItem[iTime], 7 );		// ���ڴ󱳾�-------7
		sprintf( g_Mobi.strMobinonte[iTime].szMobinonte,	"\\����\\flash\\resource\\%s\\00%d.bmp", g_mainItem[iTime], 1 );		// MobinonteͼƬ----1

#endif

		g_Mobi.hbitmapMobinonte[iTime].hBigBkground		=	LoadImage( hInst, g_Mobi.strMobinonte[iTime].szBigBkground,		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
		g_Mobi.hbitmapMobinonte[iTime].hMobinonte		=	LoadImage( hInst, g_Mobi.strMobinonte[iTime].szMobinonte,		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

		g_Mobi.hbitmapMobinonte[iTime].hSelBackgroundColor = CreateSolidBrush( dszBgColor[iTime] );
		g_Mobi.hbitmapMobinonte[iTime].hSelForegroundColor = CreateSolidBrush( dszMenuColor[iTime] );
		if ( g_Mobi.hbitmapMobinonte[iTime].hBigBkground		&&
			g_Mobi.hbitmapMobinonte[iTime].hMobinonte 			&&
			g_Mobi.hbitmapMobinonte[iTime].hSelForegroundColor	&&
			g_Mobi.hbitmapMobinonte[iTime].hSelBackgroundColor )
		{
			continue;
		}
		else
		{
			iSuccess = 1;
			break;
		}
	}
	bLoadResourceOver = TRUE;
	return iSuccess;
}

VOID LoadThread( HWND hWnd )
{
	HANDLE hLoadResource;
	hLoadResource = CreateThread( NULL, 0, CommonSetMobinonteSource, (LPVOID)hWnd, 0, 0 );
	if (hLoadResource)
	{
		CloseHandle(hLoadResource);
	}
}

// �Ż���ʼ��
//BOOL OptimizeLoadResource( HINSTANCE hInst, HWND hWnd )
BOOL OptimizeLoadResource( HINSTANCE hInst )
{
	BOOL	bSuccess = TRUE;

	strcpy( g_Mobi.strMobinonte[0].szTitle, g_mainItem[0] );
#ifdef LINUX_KERNEL
	sprintf( g_Mobi.strMobinonte[0].szBigBkground,	"./kingmos/flash/resource/%s/00%d.bmp", g_mainItem[0], 7 );		// ���ڴ󱳾�-------7
	sprintf( g_Mobi.strMobinonte[0].szMobinonte,	"./kingmos/flash/resource/%s/00%d.bmp", g_mainItem[0], 1 );		// MobinonteͼƬ----1
#else
	sprintf( g_Mobi.strMobinonte[0].szBigBkground,	"\\����\\flash\\resource\\%s\\00%d.bmp", g_mainItem[0], 7 );		// ���ڴ󱳾�-------7
	sprintf( g_Mobi.strMobinonte[0].szMobinonte,	"\\����\\flash\\resource\\%s\\00%d.bmp", g_mainItem[0], 1 );		// MobinonteͼƬ----1
#endif

	g_Mobi.hbitmapMobinonte[0].hBigBkground		=	LoadImage( hInst, g_Mobi.strMobinonte[0].szBigBkground,		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	g_Mobi.hbitmapMobinonte[0].hMobinonte		=	LoadImage( hInst, g_Mobi.strMobinonte[0].szMobinonte,		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

	g_Mobi.hbitmapMobinonte[0].hSelBackgroundColor = CreateSolidBrush( dszBgColor[0] );
	g_Mobi.hbitmapMobinonte[0].hSelForegroundColor = CreateSolidBrush( dszMenuColor[0] );
	if ( g_Mobi.hbitmapMobinonte[0].hBigBkground		&&
		g_Mobi.hbitmapMobinonte[0].hMobinonte 			&&
		g_Mobi.hbitmapMobinonte[0].hSelForegroundColor	&&
		g_Mobi.hbitmapMobinonte[0].hSelBackgroundColor )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


// �����һ�γ�ʼ��
BOOL FirstSetMobinonteSource( HINSTANCE hInst )
{
	BOOL bCommon = FALSE;
//	HINSTANCE hInst = (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE );

	memset( &g_Mobi, 0, sizeof(g_Mobi) );

	g_Mobi.uSize = sizeof(g_Mobi);
	{	// ���ò��ֳ�ʼ��
#ifdef LINUX_KERNEL
		sprintf( g_Mobi.cCommonMobi.szArrow, "./kingmos/flash/resource/006.ico" );		// ��ͷ-------------6
#else
		sprintf( g_Mobi.cCommonMobi.szArrow, "\\����\\flash\\resource\\006.ico" );		// ��ͷ-------------6
#endif
		g_Mobi.cCommonMobi.hArrow = (HICON)LoadImage( hInst, g_Mobi.cCommonMobi.szArrow, IMAGE_ICON, 0, 0, LR_LOADFROMFILE );
		g_Mobi.cCommonMobi.hWhiteBrush = CreateSolidBrush( RGB(255,255,255) );				// ������ɫ�Ļ�ˢ
		g_Mobi.cCommonMobi.hPen = CreatePen( PS_NULL, 1, RGB(255,255,255) );				// ������ɫ�Ļ���
		g_Mobi.cCommonMobi.hRedPen = CreatePen( PS_SOLID, 5, RGB(255,0,0) );				// ������ɫ����
	}

//	bCommon = CommonSetMobinonteSource( hInst, hWnd );	// ѭ����ʼ��
	bCommon = OptimizeLoadResource( hInst );	// ѭ����ʼ��

	memset( &myChoiceMenu, 0, sizeof(myChoiceMenu) );
	if( g_Mobi.cCommonMobi.hArrow		&& 
		g_Mobi.cCommonMobi.hWhiteBrush	&&
		g_Mobi.cCommonMobi.hPen			&&
		g_Mobi.cCommonMobi.hRedPen		&& bCommon )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ��ͨ������ͷ���Դ
VOID CommonReleaseMobinonteSource( )
{
	int iItem;

	for( iItem=0; iItem<ITEM_SEL_NUM; iItem++ )
	{
		if (g_Mobi.hbitmapMobinonte[iItem].hBigBkground)
		{
			DeleteObject(g_Mobi.hbitmapMobinonte[iItem].hBigBkground);
		}
		if (g_Mobi.hbitmapMobinonte[iItem].hMobinonte)
		{
			DeleteObject(g_Mobi.hbitmapMobinonte[iItem].hMobinonte);
		}
		if (g_Mobi.hbitmapMobinonte[iItem].hSelBackgroundColor)
		{
			DeleteObject(g_Mobi.hbitmapMobinonte[iItem].hSelBackgroundColor);
		}
		if (g_Mobi.hbitmapMobinonte[iItem].hSelForegroundColor)
		{
			DeleteObject(g_Mobi.hbitmapMobinonte[iItem].hSelForegroundColor);
		}
		g_Mobi.hbitmapMobinonte[iItem].hSelBackgroundColor	=
		g_Mobi.hbitmapMobinonte[iItem].hSelForegroundColor	= NULL;
		g_Mobi.hbitmapMobinonte[iItem].hBigBkground			=
		g_Mobi.hbitmapMobinonte[iItem].hMobinonte			= NULL;
		
	}
}

// �ͷ����е���Դ
VOID LastReleaseMobinonteSource( )
{
	if (g_Mobi.cCommonMobi.hArrow)		// ICON ͼƬ
	{
		DestroyIcon( g_Mobi.cCommonMobi.hArrow );
	}
	if (g_Mobi.cCommonMobi.hWhiteBrush)	// ��ɫ��ˢ
	{
		DeleteObject( g_Mobi.cCommonMobi.hWhiteBrush );
	}
	if (g_Mobi.cCommonMobi.hPen)		// ��ɫ����
	{
		DeleteObject( g_Mobi.cCommonMobi.hPen );
	}
	if (g_Mobi.cCommonMobi.hRedPen)		// ��ɫ����
	{
		DeleteObject( g_Mobi.cCommonMobi.hRedPen );
	}
	
	g_Mobi.cCommonMobi.hPen		=
	g_Mobi.cCommonMobi.hRedPen	= NULL;
	g_Mobi.cCommonMobi.hArrow		= NULL;
	g_Mobi.cCommonMobi.hWhiteBrush	= NULL;

	if (hLoadBitmap)
	{
		DeleteObject( hLoadBitmap );
		hLoadBitmap = NULL;
	}
	CommonReleaseMobinonteSource( );
}

VOID DoMobiEraseBkgnd( HWND hWnd, HDC hDc )
{

	HDC		hMemDC = CreateCompatibleDC( hDc );

	if (hMemDC)
	{
		HWND	hListCtrl = GetDlgItem( hWnd, MOBI_LIST );
		if ( g_Mobi.hbitmapMobinonte[iMobiSel].hBigBkground		&&
			g_Mobi.hbitmapMobinonte[iMobiSel].hSelBackgroundColor	&&
			g_Mobi.hbitmapMobinonte[iMobiSel].hMobinonte )
		{
			SelectObject( hMemDC, g_Mobi.hbitmapMobinonte[iMobiSel].hBigBkground );	// �󱳾�
			BitBlt( hDc, WINDOW_X_S, WINDOW_Y_S, WINDOW_X_E, WINDOW_Y_E, hMemDC, 0, 0, SRCCOPY );

			SelectObject( hDc, g_Mobi.cCommonMobi.hPen );
			SelectObject( hDc, g_Mobi.hbitmapMobinonte[iMobiSel].hSelBackgroundColor );	// Mobinonte ����
			RoundRect( hDc, -5, rtMobiPaint.top, rtMobiPaint.right, rtMobiPaint.bottom, 10, 10 );

			SelectObject( hMemDC, g_Mobi.hbitmapMobinonte[iMobiSel].hMobinonte );	// Mobinonte ǰ��
			BitBlt( hDc, MOBI_H_MOBI_X, MOBI_H_MOBI_Y, 119, 28, hMemDC, 0, 0, SRCCOPY );

		}

		DeleteDC( hMemDC );
	}
}

// ���������Ϣ���ϡ��°�������Ȼ�󷵻�ѡ���Ľ��
int		DoMobiKeyDown( HWND hWnd, WPARAM wParam )
{
	int		iAll=0, iSel=0;
	HWND	hListCtrl;

	hListCtrl = GetDlgItem( hWnd, MOBI_LIST );
	iAll = SendMessage( hListCtrl, LVM_GETITEMCOUNT, NULL, NULL );		// ����
	iSel = SendMessage( hListCtrl, LVM_GETSELECTIONMARK, NULL, NULL );

	if( wParam == VK_UP )
	{
		iSel --;
		iSel = (iSel+iAll)%iAll;
	}
	else if( wParam == VK_DOWN )
	{
		iSel ++;
		iSel = (iSel+iAll)%iAll;
	}
	return iSel;
}

// �������ѡ����Ŀ����
VOID	DoMoniDrawItem( HWND hWnd, int iItem )
{
	char	szText[80];
	char	szChoiceText[256], szSpec[] = "��";
	HDC		hDc = GetDC( hWnd );
	HDC		hMemDC = CreateCompatibleDC( hDc );
	int		i;
	static	HWND	hBitmap = NULL;
	BOOL	bLoadBitmap = FALSE;

	szChoiceText[0] = 0;
	for( i=0; i<6; i++ )
	{
		if (myChoiceMenu[i].bValid)
		{
			strcat( szChoiceText, myChoiceMenu[i].szText );
			strcat( szChoiceText, szSpec );
			continue;
		}
		else
		{
			break;
		}
	}
	{	// ���б���п�������
		LV_ITEM	lvItem;
		HWND	hListCtrl = GetDlgItem( hWnd, MOBI_LIST );
		int		iIndex = SendMessage( hListCtrl, LVM_GETSELECTIONMARK, NULL, NULL );	// ��ȡ��ǰ�Ľ����ڵڣ���

		memset( &lvItem, 0, sizeof(lvItem) );
		lvItem.mask = LVIF_TEXT|LVIF_PARAM;	// ��ȡ����
		lvItem.pszText = szText;	// ���ֻ���
		lvItem.iItem = iIndex;
		lvItem.iSubItem = 0;
		SendMessage(hListCtrl, LVM_GETITEM, NULL, (LPARAM)&lvItem);
		if (szText[0])
		{
			strcat( szChoiceText, szText );

			if (lvItem.lParam&FILE_BITMAP)	// ��ͼƬ
			{
				DoLoadBitmapAndShowIt( hWnd, iIndex );
				bLoadBitmap = TRUE;
			}
		}
	}

	if (hMemDC)
	{
		if( g_Mobi.hbitmapMobinonte[iMobiSel].hSelBackgroundColor )
		{
			SelectObject( hDc, g_Mobi.cCommonMobi.hPen );
			SelectObject( hDc, g_Mobi.hbitmapMobinonte[iMobiSel].hSelBackgroundColor );	// Mobinonte ����
			RoundRect( hDc, rtMobiPaint.left, rtMobiPaint.top, rtMobiPaint.right, rtMobiPaint.bottom, 10, 10 );
		}
		SetTextColor( hDc, GetSysColor(COLOR_HIGHLIGHTTEXT) );
		SetBkMode( hDc, TRANSPARENT );
		DrawText( hDc, szChoiceText, strlen(szChoiceText), &rtMobiText, DT_LEFT|DT_VCENTER );	// ��ʾ��[Mobinonte]��
		if(bLoadBitmap)	// ��Ҫ��ʾ���ص�ͼƬ
		{	// ������ϸͼƬ
			SelectObject( hDc, g_Mobi.cCommonMobi.hPen );
			SelectObject( hDc, g_Mobi.cCommonMobi.hWhiteBrush );	// Mobinonte ����
			RoundRect( hDc, rtBitmapBkGrn.left, rtBitmapBkGrn.top, rtBitmapBkGrn.right, rtBitmapBkGrn.bottom, 15, 15 );
			if (hLoadBitmap)
			{	// ����ͼƬ�ɹ�
				SelectObject( hMemDC, hLoadBitmap );
				BitBlt( hDc, rtBitmapRect.left, rtBitmapRect.top, rtBitmapRect.right-rtBitmapRect.left, rtBitmapRect.bottom-rtBitmapRect.top, hMemDC, 0, 0, SRCCOPY );
			}
			else
			{	// ����û��ͼƬ��Ч��
				SelectObject( hDc, (HGDIOBJ)g_Mobi.cCommonMobi.hRedPen );
				MoveToEx( hDc, rtBitmapRect.left, rtBitmapRect.top, 0 );
				LineTo( hDc, rtBitmapRect.right, rtBitmapRect.top );		// �� ��
				MoveToEx( hDc, rtBitmapRect.left, rtBitmapRect.top, 0 );
				LineTo( hDc, rtBitmapRect.left, rtBitmapRect.bottom );		// �� |
				MoveToEx( hDc, rtBitmapRect.left, rtBitmapRect.bottom, 0 );
				LineTo( hDc, rtBitmapRect.right, rtBitmapRect.bottom );		// �¡�
				MoveToEx( hDc, rtBitmapRect.right, rtBitmapRect.bottom, 0 );
				LineTo( hDc, rtBitmapRect.right, rtBitmapRect.top );		// ��|
				MoveToEx( hDc, rtBitmapRect.left, rtBitmapRect.top, 0 );
				LineTo( hDc, rtBitmapRect.right, rtBitmapRect.bottom );		// �Խ���"\"
				MoveToEx( hDc, rtBitmapRect.right, rtBitmapRect.top, 0 );
				LineTo( hDc, rtBitmapRect.left, rtBitmapRect.bottom );		// �Խ���"/"
			}
			{	// ������ʾ������Ϣ
				if (iMobiSel==0)		// Videos
				{	// ר��
					SetTextColor( hDc, RGB(255,159,0) );
					DrawText( hDc, "RATING", 7, &rtRating, DT_LEFT|DT_VCENTER );
					DrawText( hDc, "PRICE", 7, &rtPrice, DT_LEFT|DT_VCENTER );
					SetTextColor( hDc, RGB(255,53,0) );
					DrawText( hDc, "PG��", 7, &rtPg, DT_LEFT|DT_VCENTER );
					DrawText( hDc, "$6.99", 7, &rtValue, DT_LEFT|DT_VCENTER );
				}
				else if(iMobiSel==1)	// Music
				{
					SetTextColor( hDc, RGB(255,159,0) );
					DrawText( hDc, "ARTIST", 7, &rtPrice, DT_LEFT|DT_VCENTER );
					SetTextColor( hDc, RGB(255,53,0) );
					DrawText( hDc, szText, sizeof(szText), &rtValue, DT_LEFT|DT_VCENTER|DT_SINGLELINE );
				}
			}
		}
		else
		{	// �ô󱳾�������ϸ����
			if( g_Mobi.hbitmapMobinonte[iMobiSel].hBigBkground )
			{
				SelectObject( hMemDC, g_Mobi.hbitmapMobinonte[iMobiSel].hBigBkground );	// �󱳾�
				BitBlt( hDc, rtBitmapBkGrn.left, rtBitmapBkGrn.top, rtBitmapBkGrn.right-rtBitmapBkGrn.left, rtBitmapBkGrn.bottom-rtBitmapBkGrn.top, hMemDC, rtBitmapBkGrn.left, rtBitmapBkGrn.top, SRCCOPY );
//				BitBlt( hDc, LIST_X, LIST_Y, LIST_W, LIST_H, hMemDC, LIST_X, LIST_Y, SRCCOPY );
			}
		}
		DeleteDC( hMemDC );
	}

	ReleaseDC( hWnd, hDc );
}

// ���ļ��������ر�Ĵ�������"MLG.bmp"������ʾ��MLG.bmp���������ʾ"MLG"
VOID	DoSpecialCmdText( char *lpszFileText )
{
	if (lpszFileText)
	{
		int i, iLen, iIndex;
		char *lpTemp = lpszFileText;

		iIndex = 0;
		iLen = strlen(lpszFileText);

		for( i=0; i<iLen; i++,lpTemp++ )
		{
			if (*lpTemp=='.')
			{
				iIndex = i;
			}
		}
		if (iIndex>0)
		{
			*(lpszFileText+iIndex) = 0;
		}
	}
}

// ��ѡ������ر�Ĵ�����ӡ�All Movies�����ߡ�All Songs��
VOID	DoSpecAddItem( HWND hListCtrl, int iMobiSel )
{
	LV_ITEM		lvItem;
	char		szText[15];

	szText[0] = 0;

	memset(&lvItem, 0, sizeof(lvItem));
	lvItem.mask = LVIF_TEXT|LVIF_PARAM;
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	lvItem.lParam = FILE_RIGHT;

	switch( iMobiSel )
	{
		case 0:	// Vides
			{
				strcpy( szText, "All Movies" );
				lvItem.pszText = szText;
				SendMessage( hListCtrl, LVM_INSERTITEM, 0, (LPARAM)&lvItem );
			}
			break;
		case 1:	// Music
			{
				strcpy( szText, "All Songs" );
				lvItem.pszText = szText;
				SendMessage( hListCtrl, LVM_INSERTITEM, 0, (LPARAM)&lvItem );

				strcpy( szText, "Now Playing" );
				lvItem.iItem = 0xFFFF;
				lvItem.pszText = szText;
				SendMessage( hListCtrl, LVM_INSERTITEM, 0, (LPARAM)&lvItem );
			}
			break;
		default:
			return ;
	}
}

// ����Ŀ¼������ֵָ���Ƿ����ļ�
BOOL	DoLoadDirectoryOrFile( HWND hListCtrl, char *lpPath )
{
	BOOL			bRet = FALSE, bDel = FALSE;
	FILE_FIND_DATA	FindFileData;
	HANDLE			hFind;
	LV_ITEM			lvItem;

	hFind = FindFirstFile( lpPath, &FindFileData );
	if( hFind == INVALID_HANDLE_VALUE ) 
	{
		return bRet;
	}
	else
	{
		bDel = TRUE;	// Ӧ��ɾ���б�������
	}

	while( TRUE )
	{
		if( FindFileData.cFileName[0]=='.'||
			(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN) )
		{	// һ����ļ��ṹ�ǣ�"."��".."
			;
		}
		else
		{	// ���ļ������б��
			char szFileText[80];
			bRet = TRUE;
			if (bDel)
			{
				bDel = FALSE;
				SendMessage( hListCtrl, LVM_DELETEALLITEMS, 0, 0 );
			}
			memset(&lvItem, 0, sizeof(lvItem));
			lvItem.mask = LVIF_TEXT|LVIF_PARAM;
			strncpy( szFileText, FindFileData.cFileName, sizeof(szFileText)-1 );	// ���ֻ���
			lvItem.iItem = 0;
			lvItem.iSubItem = 0;
			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				lvItem.pszText = szFileText;
				lvItem.lParam = FILE_RIGHT;
			}
			else
			{
				DoSpecialCmdText( szFileText );
				lvItem.pszText = szFileText;
				lvItem.lParam = FILE_DISABLE|FILE_BITMAP;
			}
			SendMessage( hListCtrl, LVM_INSERTITEM, 0, (LPARAM)&lvItem );
		}

		if( FindNextFile(hFind,&FindFileData)==0 )  //������һ���ļ�
		{ // �Ѿ�û���ļ���
			// �ж��Ƿ��ǡ�Videos���͡�Music����ѡ����ΪҪ������ӡ�All Movies����All Songs��
			if ( (iMobiSel==0||iMobiSel==1) && myChoiceMenu[0].bValid && (!myChoiceMenu[1].bValid) )
			{	// �ж�Ŀǰ�����Լ�Ŀ¼��ѡ�񣬴�ʱҪ��ӡ�All Movies����All Songs��
				DoSpecAddItem( hListCtrl, iMobiSel );
			}
			break;
		}
	}
	FindClose(hFind);  // ���ҽ���
	return bRet;
}

// ���⹦�ܣ������ƶ�����Ҫ������ϸͼƬ
// BOOL bDraw:��WM_PAINT�н��л���
VOID	DoLoadBitmapAndShowIt( HWND hWnd, int iIndex )
{
	if (hLoadBitmap==NULL)
	{
#ifdef LINUX_KERNEL
		char	szFilePath[256], szFileSpec[]="/", szText[80];
#else
		char	szFilePath[256], szFileSpec[]="\\", szText[80];
#endif
		HINSTANCE	hInst = (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE );
		HWND hListCtrl = GetDlgItem( hWnd, MOBI_LIST );
		LV_ITEM	lvItem;

		szText[0] = 0;
		memset( &lvItem, 0, sizeof(lvItem) );
		lvItem.mask = LVIF_TEXT|LVIF_PARAM;	// ��ȡ����
		lvItem.pszText = szText;	// ���ֻ���
		lvItem.iItem = iIndex;
		lvItem.iSubItem = 0;
		SendMessage(hListCtrl, LVM_GETITEM, NULL, (LPARAM)&lvItem);
		if (szText[0])
		{
			int i;
			szFilePath[0] = 0;
#ifdef LINUX_KERNEL
			strcpy( szFilePath, "./kingmos/flash/" );
#else
			strcpy( szFilePath, "\\����\\flash\\" );
#endif
			for( i=0; i<6; i++ )
			{
				if (myChoiceMenu[i].bValid)
				{
					strcat( szFilePath, myChoiceMenu[i].szText );
					strcat( szFilePath, szFileSpec );
					continue;
				}
				else
				{
					break;
				}
			}
			if (lvItem.lParam&FILE_BITMAP)	// ��ͼƬ
			{
				// forѭ����ȡ��ǰ�ļ�·�� -- szFilePath
				strcat( szFilePath, szText );
				strcat( szFilePath, ".bmp" );
				hLoadBitmap = (HBITMAP)LoadImage( hInst, szFilePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
			}
		}
	}
}

// ����ϵͳ�ļ�����Ϣ
VOID	DoCmdKeyMsg( HWND hWnd, WPARAM wParam )
{
	char	szFilePath[256];
	HWND hListCtrl = GetDlgItem( hWnd, MOBI_LIST );
	if (bListHide)
	{
		bListHide = FALSE;	// ��ʾ�Ѿ���������Ϊ��ʾ
	}

	if ( (wParam==VK_UP)||(wParam == VK_DOWN) )
	{
		int iSel;
		if (hLoadBitmap)
		{	// �ͷ�ͼ����
			DeleteObject( hLoadBitmap );
			hLoadBitmap = NULL;
		}

		iSel = DoMobiKeyDown( hWnd, wParam );
		SendMessage( hListCtrl, LVM_SETSELECTIONMARK, 0, iSel );

		if ( (!myChoiceMenu[0].bValid)&&(iMobiSel!=iSel) )
		{	// �����˵��н���ѡ��
			iMobiSel = iSel;
			InvalidateRect( hListCtrl, NULL, FALSE );
			InvalidateRect( hWnd, NULL, FALSE );
		}
		else
		{
//			DoLoadBitmapAndShowIt( hWnd, iSel );
			DoMoniDrawItem( hWnd, iMobiSel );
		}
		return ;
	}
	else
	{
		int		iIndex;
		char	szText[80];
		LV_ITEM	lvItem;

		iIndex = SendMessage( hListCtrl, LVM_GETSELECTIONMARK, NULL, NULL );	// ��ȡ��ǰ�Ľ����ڵڣ���
		memset(&lvItem, 0, sizeof(lvItem));
		lvItem.mask = LVIF_TEXT|LVIF_PARAM;	// ��ȡ����
		lvItem.pszText = szText;	// ���ֻ���
		lvItem.iItem = iIndex;
		lvItem.iSubItem = 0;
		SendMessage(hListCtrl, LVM_GETITEM, NULL, (LPARAM)&lvItem);

		if ( (wParam==VK_RIGHT||wParam==VK_RETURN)&&(lvItem.lParam&FILE_RIGHT) )
		{	// ����Ŀ¼
			int		i;
#ifdef LINUX_KERNEL
			char	szSep[] = "/";
#else
			char	szSep[] = "\\";
#endif

			if (hLoadBitmap)
			{	// �ͷ�ͼ����
				DeleteObject( hLoadBitmap );
				hLoadBitmap = NULL;
			}
			szFilePath[0] = 0;
#ifdef LINUX_KERNEL
			strcpy( szFilePath, "./kingmos/flash" );
#else
			strcpy( szFilePath, "\\����\\flash" );
#endif
			for( i=0; i<6; i++ )
			{	// Ŀǰָ������Ŀ¼Ϊ����
				if (myChoiceMenu[i].bValid)
				{
					strcat( szFilePath, szSep );	// "/"�ָ�����
					strcat( szFilePath, myChoiceMenu[i].szText );
					continue;
				}
				else
				{
					break;
				}
			}

			strcat( szFilePath, szSep );
			strcat( szFilePath, szText );
#ifdef LINUX_KERNEL
			strcat( szFilePath, "/*.*" );
#else
			strcat( szFilePath, "\\*.*" );
#endif
			myChoiceMenu[i].bValid = TRUE;
			// ����szFilePathĿ¼�����е��ļ�
			if( DoLoadDirectoryOrFile( hListCtrl, szFilePath) )
			{
				myChoiceMenu[i].iOldSel = iIndex;
				myChoiceMenu[i].bValid = TRUE;
				strcpy( myChoiceMenu[i].szText, szText );
				SendMessage( hListCtrl, LVM_SETSELECTIONMARK, 0, 0 );	// Ĭ�ϵ�һ��
			}
			else
			{	// ��Ŀ¼����ȥ����Ϊû���ļ�
				myChoiceMenu[i].bValid = FALSE;
				return ;
			}
//			DoLoadBitmapAndShowIt( hWnd, iIndex );
			DoMoniDrawItem( hWnd, iMobiSel );

		}
		else if ( wParam==VK_LEFT )
		{	// ����Ŀ¼
			int i;
			if( !myChoiceMenu[0].bValid )
			{
				return ;
			}
			if (hLoadBitmap)
			{	// �ͷ�ͼ����
				DeleteObject( hLoadBitmap );
				hLoadBitmap = NULL;
			}
			if ( myChoiceMenu[0].bValid && !myChoiceMenu[1].bValid )
			{
				myChoiceMenu[0].bValid = FALSE;
				SendMessage( hListCtrl, LVM_DELETEALLITEMS, 0, 0 );
				DoSpecListCmd( hListCtrl );
				SendMessage( hListCtrl, LVM_SETSELECTIONMARK, 0, (LPARAM)myChoiceMenu[0].iOldSel );	// Ĭ�ϵ�һ��
				return ;
			}
#ifdef LINUX_KERNEL
			strcpy( szFilePath, "./kingmos/flash" );	// ���²���
#else
			strcpy( szFilePath, "\\����\\flash" );	// ���²���
#endif
			for( i=0; (i<6)&&(myChoiceMenu[i+1].bValid); i++ )
			{
#ifdef LINUX_KERNEL
				strcat( szFilePath, "/" );
#else
				strcat( szFilePath, "\\" );
#endif
				strcat( szFilePath, myChoiceMenu[i].szText );
			}
			myChoiceMenu[i].bValid = FALSE;
#ifdef LINUX_KERNEL
			strcat( szFilePath, "/*.*" );
#else
			strcat( szFilePath, "\\*.*" );
#endif
			DoLoadDirectoryOrFile( hListCtrl, szFilePath );
			SendMessage( hListCtrl, LVM_SETSELECTIONMARK, 0, (LPARAM)myChoiceMenu[i].iOldSel );	// Ĭ�ϵ�һ��
			DoMoniDrawItem( hWnd, iMobiSel );
		}
		else if( wParam == VK_LMENU )
		{
			ShowWindow( hListCtrl, SW_HIDE );
			SetTimer( hWnd, IDT_MOBI_TIMER, 50, NULL );
			UpdateWindow( hWnd );
		}
		else if ( wParam==VK_BACK )
		{
			DestroyWindow( hWnd );
		}
	}
}

// ���� WM_TIMER ��Ϣ
extern int Move_X;
VOID DoMobiTimer( HWND hWnd )
{
	BLENDFUNCTION_EX bAlpha = { AC_SRC_OVER, 0, 0xC8, AC_SRC_ALPHA };
	static	int iStep = 40;
	static	int	Mobi_X = -465;
	HWND	hListCtrl = GetDlgItem( hWnd, MOBI_LIST );
	HDC		hDc = GetDC( hListCtrl );
	HDC		hMemDC	= CreateCompatibleDC( hDc );
	HBITMAP hMemBitmap = CreateCompatibleBitmap( hDc, LIST_W, LIST_H );
	HDC		hBgDC	= CreateCompatibleDC( hDc );

	KillTimer( hWnd, IDT_MOBI_TIMER );

	bAlpha.rgbTransparent = RGB(0,0,0);
	bTimerDraw = TRUE;
	SelectObject( hBgDC, g_Mobi.hbitmapMobinonte[iMobiSel].hBigBkground );
	SelectObject( hMemDC, hMemBitmap );
	bListHide = TRUE;	// ������������
	SetWindowPos( hListCtrl, HWND_TOP, 0, LIST_Y, LIST_W, LIST_H, SWP_NOSIZE | SWP_SHOWWINDOW );

	SendMessage( hListCtrl, WM_GETMOVIEDC, (WPARAM)hMemDC, NULL );

	for( ; ; )
	{
		HDC		hTempDC = CreateCompatibleDC( hDc );
		HBITMAP hTempBitmap = CreateCompatibleBitmap( hDc, LIST_W, LIST_H );
		SelectObject( hTempDC, hTempBitmap );
		Mobi_X = Mobi_X + iStep/2 +1;
//		Mobi_X = Mobi_X+5;
		Move_X = Mobi_X;
		if (Mobi_X>0)
		{
			Mobi_X = 0;
			Move_X = 0;
		}
		// set to targer xy

		BitBlt( hTempDC, 0-Mobi_X, 0, LIST_W+Mobi_X, LIST_H, hBgDC, 0, LIST_Y, SRCCOPY );
		AlphaBlendEx( hTempDC, 0, 0, LIST_W, LIST_H, hMemDC, 0, 0, LIST_W, LIST_H, &bAlpha );
		BitBlt( hDc, Mobi_X, 0, LIST_W, LIST_H, hTempDC, 0, 0, SRCCOPY );
		//UpdateWindow( hListCtrl );
//		iStep = iStep + 2;	// ���ڼ��ٶ�
		if (iStep>0)
			iStep--;

		DeleteObject( hTempBitmap );
		DeleteDC( hTempDC );

//		if (Mobi_X+iStep>=LIST_X)
		if (Mobi_X>=LIST_X)
		{
			Move_X = 0;
			if (!bCreateThread)
			{
				bCreateThread = TRUE;
				PostMessage( hWnd, WM_CRTTHRD, NULL, NULL );	// �����߳�
			}
			Mobi_X = -465;
			iStep = 40;
			break;
		}
	}

	bTimerDraw = FALSE;
	DeleteDC( hBgDC );
	DeleteObject( hMemBitmap );
	DeleteDC( hMemDC );
	ReleaseDC( hListCtrl, hDc );
}

/*************************************************
������static LRESULT CALLBACK MobileProc( HWND hWnd, 
				UINT uMsg, WPARAM wParam, LPARAM lParam )
������
IN	HWND	hWnd	 - ���ھ��
IN	UINT	uMsg	 - ���ھ��
IN	WPARAM	wParam	 - ���� wParam
IN	LPARAM	lParam	 - ���� lParam
����ֵ��0
��������������绰���ã��绰���ֻ�
************************************************/
// �绰��������
static LRESULT CALLBACK MobinonteProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static	BOOL	bKeyVoid = TRUE;	// ����̫�ఴ��
	switch (uMsg)
	{
		case WM_PAINT:
			{
				HDC			hdc;
				PAINTSTRUCT ps;

				hdc = BeginPaint( hWnd, &ps );
				DoMoniDrawItem( hWnd, iMobiSel );
				EndPaint( hWnd, &ps );
			}
			return 0;
		case WM_ERASEBKGND:
			{
				DoMobiEraseBkgnd( hWnd, (HDC)wParam );
			}
			return 0;
		case WM_TIMER:
			{
				DoMobiTimer( hWnd );
			}
			return 0;
		case WM_KEYUP:
			{
				if (bLoadResourceOver)
					DoCmdKeyMsg( hWnd, wParam );
			}
			return 0;
		case WM_CREATE:
			{
//				BOOL bCreateRet;
				hMainWnd = hWnd;
				
//				bCreateRet = FirstSetMobinonteSource( hWnd );

				InitDrawList( hWnd );
				iMobiSel = 0;
//				if(bCreateRet)
				{
					SetTimer( hWnd, IDT_MOBI_TIMER, 50, NULL );
					return 0;
				}
//				else
//				{
//					return -1;
//				}
			}
			return 0;
		case WM_CRTTHRD:
			{
				LoadThread( hWnd );
			}
			return 0;
		case WM_RUNPROC:
			{
				SetForegroundWindow( hWnd );
			}
			return 0;
		case WM_DESTROY:
			{
				LastReleaseMobinonteSource( );
				PostQuitMessage( 0 );
			}
			return 0;
		default :
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

/*************************************************
������static ATOM RegisterTelClass( HINSTANCE hInstance )
������
IN hInstance -��������
����ֵ������һ��ԭ�ӱ���
����������ע��һ�����ڴ�����
************************************************/
static ATOM RegisterMobilnonteClass( HINSTANCE hInst )
{
	WNDCLASS	wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)MobinonteProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(long);
	wc.hInstance		= hInst;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= g_classMOBI;

	return RegisterClass(&wc);
}
#if 0
// ע���ź���
static HANDLE MutexInit( HINSTANCE hInstance )
{
	HANDLE	hMutex;
	hMutex = CreateMutex( NULL, FALSE, "MOBINONTE" );
	if (hMutex)
	{
		DWORD dLastError = GetLastError( );
		if ( dLastError==ERROR_ALREADY_EXISTS )
		{
			HWND hWnd = 0;
			hWnd = FindWindow( g_classMOBI, NULL );
			if( hWnd!=0 )
			{
				PostMessage( hWnd, WM_RUNPROC, 0, 0 );
			}
			CloseHandle( hMutex );
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
	return hMutex;
}

// �ͷ��ź���
static VOID MutexDeInit( HANDLE hMutex )
{
	ReleaseMutex(hMutex);
	CloseHandle( hMutex );
}
#endif
/*************************************************
������LRESULT CALLBACK WinMain_Mobinonte( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )

������
IN hInstance       -��������
IN hPrevInstance   -ǰһ�������������ѱ�����
IN lpCmdLine       -������
IN nCmdShow        -��ʼ��ʱ����ʾ��񣬳���ֵSW_HIDE ,SW_SHOW
����ֵ��  int TRUE -��ȷ����AP
����������Ӧ�ó�����ں���
************************************************/
#ifndef INLINE_PROGRAM
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,int nCmdShow)
#else
int WINAPI WinMain_Mobinonte( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,int nCmdShow)
#endif
{
	MSG	msg;
#if 0	
	HANDLE hMutex = MutexInit(hInstance);
	if (!hMutex )
	{
		return FALSE;
	}

#endif

	InitCommonControls( );
	RegisterMobilnonteClass( hInstance );

	FirstSetMobinonteSource( hInstance );	// �Ż�

//	hMainWnd = CreateWindowEx( WS_EX_TITLE|WS_EX_NOMOVE|WS_EX_CLOSEBOX, g_classMOBI, 
	CreateWindowEx( WS_EX_NOMOVE, g_classMOBI, 
		"Mobinonte", WS_POPUP|WS_VISIBLE|WS_CLIPCHILDREN, WINDOW_X_S, WINDOW_Y_S, WINDOW_X_E, WINDOW_Y_E, 0, NULL, hInstance, NULL );
#if 0
	if ( !hMainWnd )
	{
		MutexDeInit( hMutex );
		return FALSE;
	}
#endif
	while ( GetMessage(&msg, 0, 0, 0) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
#if 0	
	MutexDeInit( hMutex );
#endif	
	return TRUE;
}