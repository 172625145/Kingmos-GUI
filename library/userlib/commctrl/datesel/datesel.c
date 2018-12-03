/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵��������������
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-06-06
���ߣ�    ФԶ��
�޸ļ�¼��
2003-06-05: DoLButtonDown ���� SetFocus
2003-04-21: ��InvalidateRect ��ΪRRefreshWindowDays, �޸����� by LN
2003-04-17: ��DATETIME �ṹ��Ϊ SYSTEMTIME�ṹ �� by LN
******************************************************/

//----------------------------------------------------
//
//�ļ���˵��
//1.  DATESELINFO����ÿؼ�DateSel���������Ժ͹��ܣ���DoCreate���䣬��DoDestroy�ͷ�
//2.  ����������ʾ��ǰ��ָ�����꣨static show���͸�����İ�ť����(combobox)���գ�draw days and clickable��
//3.  Change year by the inc/dec button
//    Change month by selecting from its combobox
//    Change day by clicking the other days, the day is slected when click twice, 
//    ������û�(hWndUserSel)1���Ѿ�ע���֪ͨ��Ϣ(uMsgUserSel)
//4.  ���û�Click�ؼ�DateSel���ⲿ������ ��ϢWM_ACTIVATE(wParam==WA_INACTIVE)ʱ��
//    ����û�(hWndUserSel)1��ȡ��ѡ���֪ͨ��Ϣ(uMsgUserSel)
//5.  �ṩ���û����ر��ܣ��ο�edatesel.h����Ϣ
//
//----------------------------------------------------

#include <eframe.h>
#include <edatesel.h>
#include <ecombo.h>
#include <ettime.h>
#include <esymbols.h>
#include <ecomctrl.h>

#ifdef EML_DOS
#include "Dos.h"
#include <stdio.h>
#endif

#include <ymdlunar.h>

/***************  ȫ���� ���壬 ���� *****************/

//#define _MAPPOINTER
//---------------------------------------------------
static	const	TCHAR		g_pszWeek[][4]= {"��", "һ", "��", "��", "��", "��", "��"};

//---------------------------------------------------
#define YEAR_MIN		0
#define YEAR_MAX		9999

#define	DAYS_COL		7
#define	DAYS_ROW		6

#define DEF_16FONT
//#define DEF_24FONT


#ifdef DEF_16FONT

//�ؼ� λ��
#define	H_YEAR			16
#define	W_YEAR			32

#define	H_YEARCTL		18
#define	W_YEARCTL		16


#define	H_MONTH			H_YEAR
#define	W_MONTH			W_YEAR

#define	H_MONTHCTL		H_YEARCTL
#define	W_MONTHCTL		W_YEARCTL

#define YPOS_YEARMONTH	0
#define H_YEARMONTH		18

#define YPOS_LUNAR		H_YEARMONTH
#define H_LUNAR			18


#define	H_WEEK			22						//WEEK�ؼ� �߶�
#define ROW_HEIGHT		12
#define	H_DAY			(DAYS_ROW * ROW_HEIGHT)			//DAY�ؼ�  ��С�߶�

//#define	YPOS_WEEK		(3 + 20 + 1)			//WEEK�ؼ� λ��
//#define	YPOS_DAY		(YPOS_WEEK + H_WEEK)//DAY�ؼ�  λ��
#define	YPOS_WEEK		(YPOS_LUNAR + H_LUNAR + 1)//(H_WEEK + 4)//(3 + 20 + 1)			//WEEK�ؼ� λ��
#define	YPOS_DAY		(YPOS_WEEK + H_WEEK + 1)//DAY�ؼ�  λ��

#define	W_MON			63						//�¿ؼ� ���
#define	W_APP			120						//AP ��С���
#define	H_APP			(YPOS_DAY + H_DAY)		//AP ��С�߶�

#endif
#ifdef DEF_24FONT

#define	H_YEAR			24
#define	W_YEAR			48

#define	H_YEARCTL		24
#define	W_YEARCTL		18

#define	H_MONTH			H_YEAR
#define	W_MONTH			W_YEAR

#define	H_MONTHCTL		H_YEARCTL
#define	W_MONTHCTL		W_YEARCTL

#define YPOS_YEARMONTH	0
#define H_YEARMONTH		26

#define YPOS_LUNAR		(YPOS_YEARMONTH + H_YEARMONTH + 1)
#define H_LUNAR			26

//�ؼ� λ��
#define	H_WEEK			26//22						//WEEK�ؼ� �߶�
#define ROW_HEIGHT		26
#define	H_DAY			(DAYS_ROW * ROW_HEIGHT) //18)			//DAY�ؼ�  ��С�߶�

#define	YPOS_WEEK		(YPOS_LUNAR + H_LUNAR + 1)//(H_WEEK + 4)//(3 + 20 + 1)			//WEEK�ؼ� λ��
#define	YPOS_DAY		(YPOS_WEEK + H_WEEK + 1)//DAY�ؼ�  λ��

#define	W_MON			90//63						//�¿ؼ� ���
#define	W_APP			120						//AP ��С���
#define	H_APP			(YPOS_DAY + H_DAY)		//AP ��С�߶�

#endif

//��ɫ
#define	RGB_DAYSGRAY			RGB(192, 192, 192)

//�ؼ�
//#define IDC_MONTHSEL			100
#define IDC_YEAR				101
#define IDC_YEARDEC				102
#define IDC_YEARINC				103

#define IDC_MONTH				104
#define IDC_MONTHDEC			105
#define IDC_MONTHINC			106

#define IDC_GOTODAY				107
//---------------------------------------------------
//help function
#define	ARR_COUNT( arr )			(sizeof(arr)/sizeof(arr[0]))
#define	RECT_CX( prt )				((prt)->right - (prt)->left)
#define	RECT_CY( prt )				((prt)->bottom - (prt)->top)

#define AVERAGE(prt)				((RECT_CX(prt)%7) / 2)

static	BOOL	hlp_ShowYear( HWND hDlg, int nIDDlgItem, WORD wYear );
static	BOOL	hlp_ShowMonth( HWND hWnd, int nIDDlgItem, WORD wMonth);
static	WORD	hlp_GetDayWeekFromFirst( WORD wDayWeekOk, WORD wDayGet );

//---------------------------------------------------
typedef	struct	_DATESELINFO
{
	DWORD	dwDateStyle;			//
	HWND	hWndUserSel;		//�û�����ؼ�֮������ʱ ������Ϣ
	UINT	uMsgUserSel;

	WORD	wCaleYearMin;		//�� Min
	WORD	wCaleYearMax;		//�� Max

	WORD	wCaleYear;			//��
	WORD	wCaleMonth;			//��
	WORD	wCaleDaysOfMonth;	//��ǰ�µ�����
	SHORT	nPosDayFirst;		//��ǰ��1�� ��������ʾλ�ã����������ڼ�

	WORD	wCaleDaySel;		//��ǰ�� ��ǰѡ��   ������
	WORD	wFlagFont;			//ѡ�� ��ʾ���ڵ�����

	WORD	wDayFirstPrevM;		//ǰһ������ʾ�Ŀ�ʼ����
	WORD	wDayEndNextM;		//��һ������ʾ�Ľ�������

	//
	COLORREF cl_Text;
	COLORREF cl_TextBk;     //�����ı���ǰ���뱳��ɫ
	COLORREF cl_Selection;
	COLORREF cl_SelectionBk;  // ѡ���ı���ǰ���뱳��ɫ
//	COLORREF cl_Disable;
//	COLORREF cl_DisableBk;    // ��Ч�ı���ǰ���뱳��ɫ
//	COLORREF cl_ReadOnly;
//	COLORREF cl_ReadOnlyBk;   // ֻ���ı���ǰ���뱳��ɫ
	COLORREF cl_Title;
	COLORREF cl_TitleBk;      // �����ı���ǰ���뱳��ɫ

	COLORREF cl_MonthOther;

	SYSTEMTIME stToday;

} DATESELINFO, *LPDATESELINFO;

//---------------------------------------------------
static	BOOL	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem );
static	void	DrawDay( LPDATESELINFO lpInfo, HDC hdcDraw, RECT* prtDay, WORD wDay, BOOL fSel ,int iPos, BOOL bRefreshBK);
static	void	ReadyData( LPDATESELINFO lpInfo );
static	void	YearChange( HWND hWnd, LPDATESELINFO lpInfo );
static	void MonthChange( HWND hWnd, LPDATESELINFO lpInfo );
static void DoGoToday(HWND hWnd, LPDATESELINFO lpInfo);

static	void	DoActiveApp( HWND hWnd );
static LRESULT	DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs );

static LRESULT CALLBACK WndProc_DateSel( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static void		DoPaint( HWND hWnd, HDC hdc, const LPRECT lprtPaint );
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroy(HWND hWnd);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDateFont(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDateStyle(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetClickOut(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam);
static int DoEraseBkgnd( HWND hWnd, HDC hdc );

static BOOL IsToday(LPDATESELINFO lpInfo,WORD wDays);
static void DrawWeek(HWND hWnd,HDC hdc, const LPRECT lprtPaint);
//static void DrawDaysInMonth(HWND hWnd,HDC hdc,const LPRECT lprtPaint);
static void DrawDaysInMonth(HWND hWnd,HDC hdc,const LPRECT lprtPaint,BOOL bRefreshBK);
static void DrawLunar(HWND hWnd,HDC hdc,BOOL bRefreshBK);
static void GetDaysRect(int iRow,int iCol,LPRECT lprcClient,LPRECT lpRect);

/******************************************************/


// ********************************************************************
//������ATOM RegisterDateSelClass(HINSTANCE hInstance)
//������
//	IN hInstance - Ӧ�ó����ʵ�����
//����ֵ��
//	�ɹ�����ע��ɹ����־����� ATOM �� ���򷵻� 0
//����������ע��Ӧ�ó�����
//����: 
// ********************************************************************
ATOM RegisterDateSelClass( HINSTANCE hInst )
{
    WNDCLASS wc;
	
    wc.style = 0;
    wc.lpfnWndProc = WndProc_DateSel;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = NULL;//(HBRUSH)GetSysColorBrush(COLOR_WINDOW);
    wc.lpszMenuName = 0;
    wc.lpszClassName = classDATESEL;
	
    return (BOOL)(RegisterClass( &wc ));
}

// ********************************************************************
//������LRESULT CALLBACK WndProc_DateSel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN message - ��Ҫ�������Ϣ
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������Ӧ�ó��򴰿ڴ������
//����: 
// ********************************************************************
static LRESULT CALLBACK WndProc_DateSel( HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	//
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			DoPaint(hWnd,hdc, &ps.rcPaint);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_ERASEBKGND:
		return DoEraseBkgnd( hWnd, (HDC)wParam );
	case WM_DRAWITEM:
		return DoDrawItem( hWnd, (UINT)wParam, (DRAWITEMSTRUCT*) lParam );
    case WM_LBUTTONDOWN:
		return DoLButtonDown(hWnd,wParam,lParam);

    case WM_ACTIVATE :
		if( wParam==WA_INACTIVE )
			DoActiveApp( hWnd );
		break;
    case WM_COMMAND:
		return DoCommand(hWnd,wParam,lParam);

	case DSM_GETDATETIME:
		return DoGetDateTime(hWnd,wParam,lParam);
	case DSM_SETDATETIME:
		return DoSetDateTime(hWnd,wParam,lParam);
	case DSM_SETDAYSFONT:
		return DoSetDateFont(hWnd,wParam,lParam);
	case DSM_SETDATESTYLE:
		return DoSetDateStyle(hWnd,wParam,lParam);
	case DSM_SETUSERSELNOTIFY:
		return DoSetClickOut(hWnd,wParam,lParam);
	case DSM_SETYEARRANGE:
		return DoSetRange(hWnd,wParam,lParam);

	case WM_SETCTLCOLOR:
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );

    case WM_CREATE:
		return DoCreate(hWnd,wParam,lParam);
	case WM_DESTROY:
		DoDestroy(hWnd);
		break;
	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

// ********************************************************************
//������BOOL	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN idCtl - ��Ϣ����
//  IN pDrawItem - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������WM_DRAWITEM �������
//����: 
// ********************************************************************
//#define	PAD_DRAWFOCUS
//WM_DRAWITEM����Ϣ����
BOOL	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
{
	UINT	edge;
#ifdef	PAD_DRAWFOCUS
	BOOL	fFocus = FALSE;
#endif

	//check
	//ASSERT( pDrawItem->CtlType==ODT_BUTTON );
//	if( pDrawItem->CtlType!=ODT_BUTTON || !(idCtl==IDC_YEARDEC || idCtl==IDC_YEARINC) )
//		return FALSE;

//	edge = BDR_RAISEDINNER;
	edge = BF_FLAT;
	// ͨ�û���
	if( pDrawItem->itemAction == ODA_DRAWENTIRE )		
	{
		//nState = 0;
		//if( pDrawItem->itemState & ODS_DISABLED )		// disable
		//else
	}
	// ѡ�л���	
	else if( pDrawItem->itemAction == ODA_SELECT )	
	{
		if( pDrawItem->itemState & ODS_SELECTED )		//�õ� Select
		{
//			edge = BDR_SUNKENINNER;
			DrawEdge( pDrawItem->hDC, &pDrawItem->rcItem, BDR_SUNKENINNER, BF_RECT );
		}
		else
		{
			DrawEdge( pDrawItem->hDC, &pDrawItem->rcItem, 0, BF_MIDDLE );
		}
		//else
		//	nState = 0;								//ʧȥ Select
//#ifdef	PAD_DRAWFOCUS
//		if( pDrawItem->itemState & ODS_FOCUS )		//�н��㡢û�н���
//			fFocus = TRUE;
//#endif
	}
#ifdef	PAD_DRAWFOCUS
	//������������û���
	else if( pDrawItem->itemAction & ODA_FOCUS )
	{
		fFocus = TRUE;
	}
#endif

//	DrawEdge( pDrawItem->hDC, &pDrawItem->rcItem, edge, BF_RECT | BF_MIDDLE );
//	DrawEdge( pDrawItem->hDC, &pDrawItem->rcItem, edge, BF_RECT );
	if (idCtl == IDC_GOTODAY)
	{
		HICON hIcon;

			hIcon = LoadImage( NULL, MAKEINTRESOURCE(IDI_GOTODAY), IMAGE_ICON, 24, 24, LR_DEFAULTSIZE ) ; // װ��Ӧ�ó�����Դ
			if (hIcon)
			{
				DrawIcon(pDrawItem->hDC,pDrawItem->rcItem.left + 1,pDrawItem->rcItem.top + 1,hIcon);
				DestroyIcon(hIcon);
			}
	}
	else
	{
		TCHAR		pszDir[2];
		HFONT		hFontOld;
		COLORREF	colOldText;
		int			iModeOld;

		pszDir[1] = 0;
		if( idCtl==IDC_YEARDEC || idCtl==IDC_MONTHDEC)
			pszDir[0] = SYM_LEFTARROW;
		else
			pszDir[0] = SYM_RIGHTARROW;

//		hFontOld  = SelectObject( pDrawItem->hDC, GetStockObject(SYSTEM_FONT_SYMBOL16X16) );//
		hFontOld  = SelectObject( pDrawItem->hDC, GetStockObject(SYSTEM_FONT_SYMBOL) );//
		iModeOld = SetBkMode( pDrawItem->hDC, TRANSPARENT );
		colOldText= SetTextColor( pDrawItem->hDC, GetSysColor(COLOR_BTNTEXT) );

		DrawText( pDrawItem->hDC, pszDir, 1, &pDrawItem->rcItem, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

		SetTextColor( pDrawItem->hDC, colOldText );
		SetBkMode( pDrawItem->hDC, iModeOld );
		SelectObject( pDrawItem->hDC, hFontOld );//
	}

#ifdef	PAD_DRAWFOCUS
	if( fFocus )
	{
		DrawFocusRect( pDrawItem->hDC, &pDrawItem->rcItem );
	}
#endif
	
	return TRUE;
}

//draw year
// ********************************************************************
//������BOOL	hlp_ShowYear( HWND hWnd, int nIDDlgItem, WORD wYear)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//	IN nIDDlgItem - �ؼ�ID
//	IN wYear - Ҫ��ʾ����
//����ֵ��
//	����TRUE
//�����������ڿؼ�����ʾ��
//����: 
// ********************************************************************
static	BOOL	hlp_ShowYear( HWND hWnd, int nIDDlgItem, WORD wYear)
{
	TCHAR	pszTmp[12];
	sprintf( pszTmp, TEXT("%4d"), wYear );
	return SetWindowText( GetDlgItem( hWnd, nIDDlgItem ), pszTmp );	
}

// ********************************************************************
//������BOOL	hlp_ShowMonth( HWND hWnd, int nIDDlgItem, WORD wMonth)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//	IN nIDDlgItem - �ؼ�ID
//	IN wYear - Ҫ��ʾ����
//����ֵ��
//	����TRUE
//�����������ڿؼ�����ʾ��
//����: 
// ********************************************************************
static	BOOL	hlp_ShowMonth( HWND hWnd, int nIDDlgItem, WORD wMonth)
{
	TCHAR	pszTmp[12];
	sprintf( pszTmp, TEXT("%02d��"), wMonth );
	return SetWindowText( GetDlgItem( hWnd, nIDDlgItem ), pszTmp );	
}
// ********************************************************************
//������VOID RefreshWindowDays( HWND hWnd, RECT* prtDraw )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//	IN prtDraw - ָ���ľ��η�Χ��
//����ֵ��
//	��
//�������������»�������Days
//����: 
// ********************************************************************
//LN:2003-04-21, ����
//redraw all days
static VOID RefreshWindowDays( HWND hWnd, RECT* prtDraw )
{
	HDC		hdc;
	int			iModeOld;

	hdc = GetDC( hWnd );
	ASSERT( hdc );
/*	if( prtDraw==NULL )
	{
		RECT	rtClient;
		RECT	rtDrawDays;

		GetClientRect( hWnd, &rtClient );
		SetRect( &rtDrawDays, rtClient.left, YPOS_DAY, rtClient.right,  rtClient.bottom );
		prtDraw = &rtDrawDays;
	}
*/
	//
//	DoPaint( hWnd, hdc, prtDraw );
		//���� ��-------��ʼ
		iModeOld = SetBkMode( hdc, TRANSPARENT );
		DrawDaysInMonth(hWnd,hdc,NULL , TRUE);
		//  ����ũ��
		DrawLunar(hWnd,hdc,TRUE);

		SetBkMode( hdc, iModeOld );
		//���� �ꡢ�¡���-------����
		ReleaseDC( hWnd, hdc );
}
//

// ********************************************************************
//������void	DoPaint( HWND hWnd, HDC hdcDraw, const LPRECT lprtPaint )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN hdcDraw - ��ͼHDC���
//  IN lprtPaint - ��ͼ�ľ���
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������WM_PAINT �������
//����: 
// ********************************************************************
void	DoPaint( HWND hWnd, HDC hdcDraw, const LPRECT lprtPaint )
{
	int			iModeOld;
	
		//
		iModeOld = SetBkMode( hdcDraw, TRANSPARENT );
		//  ����ũ��
		DrawLunar(hWnd,hdcDraw,FALSE);
		//���� week
		//
		DrawWeek(hWnd,hdcDraw, lprtPaint);
		//���� ��-------��ʼ
		DrawDaysInMonth(hWnd,hdcDraw,lprtPaint , FALSE);
		//���� �ꡢ�¡���-------����
		//
		SetBkMode( hdcDraw, iModeOld );
}

typedef	struct	_ITEMCTL
{
	LPCTSTR	pszWndClass;
	DWORD	uIDCtl;
	LPCTSTR	pszTitle;
	int		x;
	int		y;
	int		cx;
	int		cy;
	DWORD	dwStyle;
}ITEMCTL;

// ********************************************************************
//������LRESULT DoCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	���� 0
//����������WM_CREATE �������
//����: 
// ********************************************************************
static LRESULT DoCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPDATESELINFO	lpInfo;
	HINSTANCE		hInst;
	int				i;
	SYSTEMTIME		datetime;
	RECT			rtDate;
	RECT			rtWnd;

	HWND			hWndChild;
	ITEMCTL			ctlChild[] = 
	{
		//year
//		{   classBUTTON	, IDC_YEARDEC	, NULL	, 3, 4, 15, 18	, BS_OWNERDRAW	},
//		{ classSTATIC	, IDC_YEAR		, NULL	, 20, 5, 32, 16	, 0	},
//		{ classBUTTON	, IDC_YEARINC	, NULL	, 53, 4, 15, 18	, BS_OWNERDRAW	},
		{ classBUTTON	, IDC_YEARDEC	, NULL	, 3, 1, W_YEARCTL, H_YEARCTL	, BS_OWNERDRAW	},
		{ classSTATIC	, IDC_YEAR		, NULL	, (3 + W_YEARCTL + 2), 1 + (H_YEARCTL - H_YEAR)/2, W_YEAR, H_YEAR	, 0	},
		{ classBUTTON	, IDC_YEARINC	, NULL	, (3 + W_YEARCTL + 2 + W_YEAR + 2), 1, W_YEARCTL, H_YEARCTL	, BS_OWNERDRAW	},

		//month
//		{ classCOMBOBOX, IDC_MONTHSEL	, NULL		, 78, 3, W_MON,100	, CBS_DROPDOWNLIST	}
#ifdef DEF_16FONT
		{ classBUTTON	, IDC_MONTHDEC	, NULL	, 70, 1, W_MONTHCTL, H_MONTHCTL	, BS_OWNERDRAW	},
		{ classSTATIC	, IDC_MONTH		, NULL	, (70 + W_MONTHCTL + 2), 1 + (H_MONTHCTL - H_MONTH)/2, W_MONTH, H_MONTH	, 0	},
		{ classBUTTON	, IDC_MONTHINC	, NULL	, (70 + W_MONTHCTL + 2 + W_MONTH + 2), 1, W_MONTHCTL, H_MONTHCTL	, BS_OWNERDRAW	},

//		{ classBUTTON	, IDC_GOTODAY	, NULL	, 130, 26, 26, 26	, BS_OWNERDRAW	},
#endif
#ifdef DEF_24FONT
		{ classBUTTON	, IDC_MONTHDEC	, NULL	, 100, 1, W_MONTHCTL, H_MONTHCTL	, BS_OWNERDRAW	},
		{ classSTATIC	, IDC_MONTH		, NULL	, (100 + W_MONTHCTL + 2), 1 + (H_MONTHCTL - H_MONTH)/2, W_MONTH, H_MONTH	, 0	},
		{ classBUTTON	, IDC_MONTHINC	, NULL	, (100 + W_MONTHCTL + 2 + W_MONTH + 2), 1, W_MONTHCTL, H_MONTHCTL	, BS_OWNERDRAW	},

		{ classBUTTON	, IDC_GOTODAY	, NULL	, 210, 0, 26, 26	, BS_OWNERDRAW	},
#endif
	};
/*	TCHAR			pszMonthText[][8] = 
	{	TEXT(" һ��"),	TEXT(" ����"),	TEXT(" ����"),	TEXT(" ����"),	TEXT(" ����"),	TEXT(" ����"),
		TEXT(" ����"),	TEXT(" ����"),	TEXT(" ����"),	TEXT(" ʮ��"),	TEXT("ʮһ��"),	TEXT("ʮ����")
	};
*/	
    lpInfo = (LPDATESELINFO)malloc(sizeof(DATESELINFO));
    if(lpInfo==NULL)
	{
		return -1;	
	}
	memset( lpInfo, 0, sizeof(DATESELINFO) );
    GetLocalTime(&datetime);
    lpInfo->wCaleYear   = datetime.wYear;
    lpInfo->wCaleMonth  = datetime.wMonth;
    lpInfo->wCaleDaySel = datetime.wDay;
	lpInfo->wCaleYearMin= YEAR_MIN;
	lpInfo->wCaleYearMax= YEAR_MAX;
	ReadyData( lpInfo );
	//
	GetWindowRect( hWnd, &rtWnd );
	GetClientRect( hWnd, &rtDate );
	//i = 0;
	//if( RECT_CX(&rtDate) < W_APP )
	//{
	//	i = 1;
	//	rtWnd.right += (W_APP-RECT_CX(&rtDate));
	//}
	//if( RECT_CY(&rtDate) < H_APP )
	//{
	//	i = 1;
	//	rtWnd.bottom += (H_APP - RECT_CY(&rtDate));
	//}
	//if( i )
	//{
	//	SetWindowPos( hWnd, NULL, 0, 0, RECT_CX(&rtWnd), RECT_CY(&rtWnd), SWP_NOMOVE |SWP_NOZORDER );
	//}
	//GetClientRect( hWnd, &rtDate );

//	ctlChild[3].x = RECT_CX(&rtDate) - 3 - W_MON;
	//
	hInst = (HINSTANCE)GetWindowLong( hWnd,GWL_HINSTANCE );
	for( i=0; i<ARR_COUNT(ctlChild); i++ )
	{
		hWndChild = CreateWindow( ctlChild[i].pszWndClass, ctlChild[i].pszTitle, 
			ctlChild[i].dwStyle |WS_VISIBLE |WS_CHILD, ctlChild[i].x, ctlChild[i].y, 
			ctlChild[i].cx, ctlChild[i].cy,	hWnd, (HMENU)ctlChild[i].uIDCtl, hInst, NULL);
		if( !hWndChild )
		{
			return -1;
		}
	}
/*
	hWndChild = GetDlgItem( hWnd, IDC_MONTHSEL );
	SetWindowLong( hWndChild, GWL_EXSTYLE, (GetWindowLong( hWndChild, GWL_EXSTYLE ) |WS_EX_CLIENTEDGE) );
	for( i=0; i<ARR_COUNT(pszMonthText); i++ )
	{
		SendMessage( hWndChild, CB_ADDSTRING, 0, (LPARAM)pszMonthText[i] );
	}
	//
    SendMessage( hWndChild, CB_SETCURSEL, (lpInfo->wCaleMonth-1), 0 );
*/
	hlp_ShowYear( hWnd, IDC_YEAR, lpInfo->wCaleYear );
	hlp_ShowMonth( hWnd, IDC_MONTH, lpInfo->wCaleMonth );
	//
	
	//
	lpInfo->cl_Text        = GetSysColor( COLOR_WINDOWTEXT );
	lpInfo->cl_TextBk      = GetSysColor( COLOR_BACKGROUND );     //�����ı���ǰ���뱳��ɫ
	lpInfo->cl_Selection   = GetSysColor( COLOR_HIGHLIGHTTEXT );//CL_WHITE
	lpInfo->cl_SelectionBk = GetSysColor( COLOR_HIGHLIGHT );  // ѡ���ı���ǰ���뱳��ɫ
//	lpInfo->cl_Disable     = ;
//	lpInfo->cl_DisableBk   = ;    // ��Ч�ı���ǰ���뱳��ɫ
//	lpInfo->cl_ReadOnly    = ;
//	lpInfo->cl_ReadOnlyBk  = ;   // ֻ���ı���ǰ���뱳��ɫ
	lpInfo->cl_Title       = GetSysColor( COLOR_CAPTIONTEXT );//CL_WHITE
	lpInfo->cl_TitleBk     = GetSysColor( COLOR_ACTIVECAPTION );	       // �����ı���ǰ���뱳��ɫ

	lpInfo->cl_MonthOther  = RGB_DAYSGRAY;

	GetLocalTime(&lpInfo->stToday);

    SetWindowLong(hWnd,0,(DWORD)lpInfo);
    return 0;
}

// ********************************************************************
//������void	DoDestroy( HWND hWnd )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//����ֵ��
//	��
//����������WM_DESTROY �������
//����: 
// ********************************************************************
static LRESULT DoDestroy(HWND hWnd)
{
	LPDATESELINFO lpInfo;
	
	lpInfo=(LPDATESELINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return 0;
	free(lpInfo);
	return 0;
}

// ********************************************************************
//������LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��
//����������WM_LBUTTONDOWN �������
//����: 
// ********************************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDATESELINFO	lpInfo;
	POINT			point;
//	RECT			rtDrawDays;
	RECT			rtClient;
	LONG			xUnit;
	//LONG			yUnit;
	//LONG			xPos;
	//LONG			yCol;
	//LONG			xRow;
	int iRow,iCol;
	int iIndex;
	WORD			wCaleDay;
	
	lpInfo=(LPDATESELINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return 0;
	point.x=(short)LOWORD(lParam);
	point.y=(short)HIWORD(lParam);
	if (point.y<YPOS_DAY)
		return 0;
	GetClientRect( hWnd, &rtClient );
//	SetRect( &rtDrawDays, rtClient.left, YPOS_DAY, rtClient.right,  rtClient.bottom );
	//�õ������ţ�������Ƿ���Ч
	xUnit = (rtClient.right - rtClient.left)/DAYS_COL;
//	yUnit = (rtDrawDays.bottom - rtDrawDays.top)/DAYS_ROW;
	iRow  = (point.y-YPOS_DAY) / ROW_HEIGHT;
	iCol  = (point.x-rtClient.left)/ xUnit;
	iIndex  = (iRow * DAYS_COL) + iCol;
	//��֤��������ڵ���
	if( (iIndex>=lpInfo->nPosDayFirst) && (iIndex<(lpInfo->nPosDayFirst+lpInfo->wCaleDaysOfMonth)) )
	{
		wCaleDay = (iIndex - lpInfo->nPosDayFirst) + 1;
		if( wCaleDay==lpInfo->wCaleDaySel )
		{
			if( (lpInfo->dwDateStyle & DSS_USERSEL) && lpInfo->hWndUserSel )
			{
				PostMessage( lpInfo->hWndUserSel, lpInfo->uMsgUserSel, (WPARAM)hWnd, TRUE );
			}
		}
		else
		{
//			LONG		nAverage;
			HDC			hdc;
			int			iModeOld;
			HFONT		hFontOld;
			RECT		rtTmp;
			WORD		wCaleDaySelPrev;

			wCaleDaySelPrev    = lpInfo->wCaleDaySel;
			lpInfo->wCaleDaySel= wCaleDay;

			//׼������---��ǰ1��ѡ���� ���Ƴ�����������ǰѡ���� ���Ƴ�ѡ��
			hdc = GetDC( hWnd );
			ASSERT(hdc);
			iModeOld = SetBkMode( hdc, TRANSPARENT );
			if( lpInfo->wFlagFont!=DAYSFONT_DEFAULT )
			{
				if( lpInfo->wFlagFont==DAYSFONT_8X8 )
				{
					hFontOld = SelectObject( hdc, GetStockObject(SYSTEM_FONT_ENGLISH8X8) );
				}
				else if( lpInfo->wFlagFont==DAYSFONT_8X6 )
				{
					hFontOld = SelectObject( hdc, GetStockObject(SYSTEM_FONT_ENGLISH8X6) );
				}
			}

			//
			GetDaysRect(iRow,iCol,&rtClient,&rtTmp);
			DrawDay( lpInfo, hdc, &rtTmp, wCaleDay, TRUE ,iCol , TRUE);
			//��ǰ1��ѡ����--- ���Ƴ�����
			iCol = wCaleDaySelPrev + lpInfo->nPosDayFirst - 1;
			iRow = 0;
			while( iCol >= DAYS_COL )
			{
				iCol -= DAYS_COL;
				iRow ++;
			}
			//xRow = xPos;
			GetDaysRect(iRow,iCol,&rtClient,&rtTmp);
			DrawDay( lpInfo, hdc, &rtTmp, wCaleDaySelPrev, FALSE ,iCol , TRUE);

			//��ɻ���
			if( lpInfo->wFlagFont!=DAYSFONT_DEFAULT && hFontOld )
			{
				SelectObject( hdc, hFontOld );
			}
			//  ����ũ��
			DrawLunar(hWnd,hdc,TRUE);

			SetBkMode( hdc, iModeOld );
			ReleaseDC( hWnd, hdc );
		}
	}
	SetFocus( hWnd );//LN, 2003-06-05, ����
	return 0;
}

// ********************************************************************
//������LRESULT	DoCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������WM_COMMAND �������
//����: 
// ********************************************************************
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDATESELINFO	lpInfo;
	WORD			wNotify;
	WORD			wIDCtl;
//	WORD			wMonth;
	
	lpInfo=(LPDATESELINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return 0;
	wNotify = HIWORD(wParam);
	wIDCtl  = LOWORD(wParam);

	if( wNotify==BN_CLICKED )
	{
		switch( wIDCtl )
		{
		case IDC_YEARDEC:
			if( lpInfo->wCaleYear > lpInfo->wCaleYearMin )
				lpInfo->wCaleYear --;
			else
				lpInfo->wCaleYear = lpInfo->wCaleYearMax;
			YearChange( hWnd, lpInfo );
			break;
		case IDC_YEARINC:
			if( lpInfo->wCaleYear < lpInfo->wCaleYearMax )
				lpInfo->wCaleYear ++;
			else
				lpInfo->wCaleYear = lpInfo->wCaleYearMin;
			YearChange( hWnd, lpInfo );
			break;			
		case IDC_MONTHDEC:
			if( lpInfo->wCaleMonth > 1 )
				lpInfo->wCaleMonth --;
			else
			{
				if( lpInfo->wCaleYear > lpInfo->wCaleYearMin )
					lpInfo->wCaleYear --;
				else
					lpInfo->wCaleYear = lpInfo->wCaleYearMax;
				hlp_ShowYear( hWnd, IDC_YEAR, lpInfo->wCaleYear );
				lpInfo->wCaleMonth = 12;
			}
			MonthChange( hWnd, lpInfo );
			break;
		case IDC_MONTHINC:
			if( lpInfo->wCaleMonth < 12 )
				lpInfo->wCaleMonth ++;
			else
			{
				if( lpInfo->wCaleYear < lpInfo->wCaleYearMax )
					lpInfo->wCaleYear ++;
				else
					lpInfo->wCaleYear = lpInfo->wCaleYearMin;
				hlp_ShowYear( hWnd, IDC_YEAR, lpInfo->wCaleYear );
				lpInfo->wCaleMonth = 1;
			}
			MonthChange( hWnd, lpInfo );
			break;

		case IDC_GOTODAY:
			DoGoToday(hWnd, lpInfo);
			break;
		default:
			break;
		}
	}
/*
	else if( wNotify==CBN_SELCHANGE && wIDCtl==IDC_MONTHSEL )
	{
		wMonth = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0) + 1;
		if( wMonth != lpInfo->wCaleMonth )
		{
			lpInfo->wCaleMonth = wMonth;
			ReadyData( lpInfo );
			//InvalidateRect(hWnd,NULL,TRUE); //LN:2003-04-21, ɾ��
			RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, ����
		}
	}
*/
	return 0;
}

// ********************************************************************
//������WORD	hlp_GetDayWeekFromFirst( WORD wDayWeekOk, WORD wDayGet )
//������
//	IN wDayWeekOk - ÿ��1�ŵ�������
//	IN wDayGet - ָ�����µ�����
//����ֵ��
//	���� ����õ���������
//��������������ÿ��1�ŵ����������������ָ�����ӵ���������������
//����: 
// ********************************************************************
//���������1�������ڼ�������ָ�����������ڼ�
WORD	hlp_GetDayWeekFromFirst( WORD wDayWeekOk, WORD wDayGet )
{
	WORD	wDayWeekGet;

	wDayWeekGet = wDayGet;
	while( wDayWeekGet >= 8 )
	{
		wDayWeekGet -= 7;
	}
	ASSERT( (wDayWeekGet>=1) && (wDayWeekGet<(1+7)) );
	wDayWeekGet = wDayWeekOk + (wDayWeekGet - 1);
	if( wDayWeekGet >= 7 )
	{
		wDayWeekGet -= 7;
	}
	return wDayWeekGet;
}

// ********************************************************************
//������LRESULT	DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN lpccs - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������WM_SETCTLCOLOR �������
//����: 
// ********************************************************************
static LRESULT	DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	LPDATESELINFO	lpInfo;
	UINT			fMask;

	lpInfo=(LPDATESELINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return FALSE;

	fMask = lpccs->fMask;
	if( fMask & CLF_TITLECOLOR		 )
		lpInfo->cl_Title		= lpccs->cl_Title		;
	if( fMask & CLF_TITLEBKCOLOR	 )
		lpInfo->cl_TitleBk		= lpccs->cl_TitleBk		;
	if( fMask & CLF_TEXTCOLOR		 )
		lpInfo->cl_Text			= lpccs->cl_Text		;
	if( fMask & CLF_TEXTBKCOLOR		 )
		lpInfo->cl_TextBk		= lpccs->cl_TextBk		;
	if( fMask & CLF_SELECTIONCOLOR	 )
		lpInfo->cl_Selection	= lpccs->cl_Selection	;
	if( fMask & CLF_SELECTIONBKCOLOR )
		lpInfo->cl_SelectionBk	= lpccs->cl_SelectionBk	;
//	if( fMask & CLF_DISABLECOLOR	 )
//		lpInfo->cl_Disable		= lpccs->cl_Disable		;
//	if( fMask & CLF_DISABLEBKCOLOR	 )
//		lpInfo->cl_DisableBk	= lpccs->cl_DisableBk	;
//	if( fMask & CLF_READONLYCOLOR	 )
//		lpInfo->cl_ReadOnly		= lpccs->cl_ReadOnly	;
//	if( fMask & CLF_READONLYBKCOLOR	 )
//		lpInfo->cl_ReadOnlyBk	= lpccs->cl_ReadOnlyBk	;
	if( fMask )
	{
		RECT	rtRedraw;
		RECT	rtClient;
		RECT	rtDrawWeek;
		RECT	rtDrawDays;

		GetClientRect( hWnd, &rtClient );
		SetRect( &rtDrawWeek, rtClient.left, YPOS_WEEK, rtClient.right, YPOS_WEEK+H_WEEK );
		SetRect( &rtDrawDays, rtClient.left, YPOS_DAY, rtClient.right,  rtClient.bottom );
		UnionRect( &rtRedraw, &rtDrawWeek, &rtDrawDays );
		
		RefreshWindowDays( hWnd, &rtRedraw );
	}

	return TRUE;
}

// ********************************************************************
//������LRESULT DoGetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������DSM_GETDATETIME �������
//����: 
// ********************************************************************
static LRESULT DoGetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSYSTEMTIME  lpDateTime;
	LPDATESELINFO lpInfo;
	
#ifdef _MAPPOINTER
	lParam = MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
	
	lpDateTime=(LPSYSTEMTIME)lParam;
	
	lpInfo=(LPDATESELINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return FALSE;
	memset( lpDateTime, 0, sizeof(SYSTEMTIME) );
	lpDateTime->wYear=lpInfo->wCaleYear;
	lpDateTime->wMonth=lpInfo->wCaleMonth;
	lpDateTime->wDay=lpInfo->wCaleDaySel;
	lpDateTime->wDayOfWeek = hlp_GetDayWeekFromFirst( lpInfo->nPosDayFirst, lpInfo->wCaleDaySel );
	
	return TRUE;
}

// ********************************************************************
//������LRESULT DoSetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������DSM_SETDATETIME �������
//����: 
// ********************************************************************
static LRESULT DoSetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSYSTEMTIME	lpDateTime;
	LPDATESELINFO	lpInfo;
//	HWND			hCtl;
#ifdef _MAPPOINTER
	lParam = MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
	
	lpDateTime = (LPSYSTEMTIME)lParam;	
	lpInfo     = (LPDATESELINFO)GetWindowLong(hWnd,0);
    if( lpDateTime==NULL || lpInfo==NULL )
		return FALSE;
	//check lpDateTime
	if( lpDateTime->wMonth>12 || lpDateTime->wMonth<1 )
		return FALSE;
	if( lpDateTime->wYear>lpInfo->wCaleYearMax || lpDateTime->wYear<lpInfo->wCaleYearMin )
		return FALSE;
	//
    lpInfo->wCaleYear   = lpDateTime->wYear;
    lpInfo->wCaleMonth  = lpDateTime->wMonth;
	lpInfo->wCaleDaySel = lpDateTime->wDay;
	ReadyData( lpInfo );
	//
	hlp_ShowYear( hWnd, IDC_YEAR, lpInfo->wCaleYear );
	hlp_ShowMonth( hWnd, IDC_MONTH, lpInfo->wCaleMonth );
//	hCtl = GetDlgItem( hWnd, IDC_MONTHSEL );
//    SendMessage( hCtl, CB_SETCURSEL, (lpInfo->wCaleMonth-1), 0 );

	//InvalidateRect(hWnd,NULL,TRUE);//LN:2003-04-21, ɾ��
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, ����
	return TRUE;
}

// ********************************************************************
//������LRESULT DoSetDateFont(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������DSM_SETDAYSFONT �������
//����: 
// ********************************************************************
static LRESULT DoSetDateFont(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDATESELINFO	lpInfo;
	DWORD			dwFlagFont;

	dwFlagFont = (DWORD)lParam;
	lpInfo     = (LPDATESELINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	//check 
	if( dwFlagFont>2 )
		return FALSE;
	lpInfo->wFlagFont = (WORD)dwFlagFont;

	//InvalidateRect(hWnd,NULL,TRUE);//LN:2003-04-21, ɾ��
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, ����
	return TRUE;
}
// ********************************************************************
//������LRESULT DoSetDateStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������DSM_SETDATESTYLE �������
//����: 
// ********************************************************************
static LRESULT DoSetDateStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDATESELINFO	lpInfo;

	lpInfo     = (LPDATESELINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	lpInfo->dwDateStyle = (DWORD)wParam;
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, ����
	return TRUE;
}
// ********************************************************************
//������LRESULT DoSetClickOut(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������DSM_SETUSERSELNOTIFY �������
//����: 
// ********************************************************************
static LRESULT DoSetClickOut(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDATESELINFO	lpInfo;

	lpInfo     = (LPDATESELINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	//check 
	lpInfo->hWndUserSel = (HWND)wParam;
	lpInfo->uMsgUserSel = (UINT)lParam;
	return TRUE;
}
// ********************************************************************
//������LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������DSM_SETYEARRANGE �������
//����: 
// ********************************************************************
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDATESELINFO	lpInfo;
	BOOL			fReDraw;

	lpInfo     = (LPDATESELINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	//check 
	lpInfo->wCaleYearMin = LOWORD(wParam);
	lpInfo->wCaleYearMax = HIWORD(wParam);
	//
	fReDraw = FALSE;
	if( lpInfo->wCaleYear > lpInfo->wCaleYearMax )
	{
		lpInfo->wCaleYear = lpInfo->wCaleYearMax;
		fReDraw = TRUE;
	}
	else if( lpInfo->wCaleYear < lpInfo->wCaleYearMin )
	{
		lpInfo->wCaleYear = lpInfo->wCaleYearMin;
		fReDraw = TRUE;
	}
	if( fReDraw )
	{
		YearChange( hWnd, lpInfo );
	}
	return TRUE;
}

// ********************************************************************
//������void YearChange( HWND hWnd, LPDATESELINFO lpInfo )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN lpInfo - DATESEL������Ϣ
//����ֵ��
//	��
//���������������꣬�����Ƴ���
//����: 
// ********************************************************************
//cal the year data and draw year
static void YearChange( HWND hWnd, LPDATESELINFO lpInfo )
{
	ReadyData( lpInfo );
	hlp_ShowYear( hWnd, IDC_YEAR, lpInfo->wCaleYear );
	//InvalidateRect(hWnd,NULL,TRUE); //LN:2003-04-21, ɾ��
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, ����
}

// ********************************************************************
// ������void MonthChange( HWND hWnd, LPDATESELINFO lpInfo )
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN lpInfo - DATESEL������Ϣ
// ����ֵ��
//	��
// ���������������£������Ƴ���
// ����: 
// ********************************************************************
//cal the year data and draw year
static void MonthChange( HWND hWnd, LPDATESELINFO lpInfo )
{
	ReadyData( lpInfo );
	hlp_ShowMonth( hWnd, IDC_MONTH, lpInfo->wCaleMonth );
	//InvalidateRect(hWnd,NULL,TRUE); //LN:2003-04-21, ɾ��
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, ����
}


// ********************************************************************
//������void	DrawDay( LPDATESELINFO lpInfo, HDC hdcDraw, RECT* prtDay, WORD wDay, BOOL fSel ,int iPos, BOOL bRefreshBK)
//������
//  IN lpInfo - DATESEL������Ϣ
//	IN hdcDraw - ���Ƶ�HDC���
//	IN prtDay - ���Ƶľ���
//	IN wDay - ���Ƶ�����
//	IN fSel - �Ƿ�Ϊѡ��״̬
//����ֵ��
//	��
//��������������ָ��������
//����: 
// ********************************************************************
//draw the day
void	DrawDay( LPDATESELINFO lpInfo, HDC hdcDraw, RECT* prtDay, WORD wDay, BOOL fSel ,int iPos, BOOL bRefreshBK)
{
	TCHAR		szTmp[8];
	COLORREF	colOld;
	
//	HPEN		hPen;
//	HPEN		hPenOld;
	HBRUSH		hBrh;
//	HBRUSH		hBrhOld;

	if (bRefreshBK)
	{
		if( fSel )
		{
			hBrh = CreateSolidBrush( lpInfo->cl_SelectionBk );
		}
		else
		{
			hBrh = CreateSolidBrush( lpInfo->cl_TextBk );
		}
		FillRect(hdcDraw,prtDay,hBrh);
		DeleteObject( hBrh );

	}
	//
	if( fSel )
		colOld = SetTextColor( hdcDraw, lpInfo->cl_Selection );
	else
	{
		if (iPos == 0 || iPos == 6)
		{ // �����ռ�������
			colOld = SetTextColor( hdcDraw, RGB(255,0,0) );
		}
		else
		{
			colOld = SetTextColor( hdcDraw, lpInfo->cl_Text );
		}
	}
	sprintf( szTmp, TEXT("%d"), wDay );
	DrawText( hdcDraw, szTmp, -1, prtDay, DT_SINGLELINE|DT_VCENTER|DT_CENTER );
	SetTextColor( hdcDraw, colOld );

	if (IsToday(lpInfo,wDay))
	{
		// ���ƽ��յı�־
		{
			HPEN	hPen;
			HPEN	hPenOld;
			HBRUSH	hBrh;
			HBRUSH	hBrhOld;

			hPen = CreatePen( PS_SOLID, 1, lpInfo->cl_Text );
			hBrh = GetStockObject(NULL_BRUSH);
			hPenOld = SelectObject( hdcDraw, (HPEN)hPen );
			hBrhOld = SelectObject( hdcDraw, (HPEN)hBrh );
			Rectangle(hdcDraw,prtDay->left,prtDay->top,prtDay->right,prtDay->bottom);
			SelectObject( hdcDraw, (HPEN)hBrhOld );
			SelectObject( hdcDraw, (HPEN)hPenOld );
			DeleteObject( hPen );
			DeleteObject( hBrh );
		}
	}

}

// ********************************************************************
//������void	ReadyData( LPDATESELINFO lpInfo )
//������
//  IN lpInfo - DATESEL������Ϣ
//����ֵ��
//	��
//�����������������������������
//����: 
// ********************************************************************
void	ReadyData( LPDATESELINFO lpInfo )
{
	WORD	iYear;
	WORD	iMonth;
	WORD	nPosDayEnd;

	iYear  = lpInfo->wCaleYear;
	iMonth = lpInfo->wCaleMonth;
	//��ǰ��
	lpInfo->wCaleDaysOfMonth = GetDayofTheMonth( iYear, iMonth );
    lpInfo->nPosDayFirst= GetWeekData( iYear, iMonth, 1 );
	nPosDayEnd   = lpInfo->wCaleDaysOfMonth + lpInfo->nPosDayFirst - 1;
	if( lpInfo->wCaleDaySel > lpInfo->wCaleDaysOfMonth )	//attention here
	{
		lpInfo->wCaleDaySel = lpInfo->wCaleDaysOfMonth;
	}
	//ǰһ�·�
	if( lpInfo->nPosDayFirst > 0 )	//����������
	{
		if( iMonth==1 )
		{
			lpInfo->wDayFirstPrevM = GetDayofTheMonth( (WORD)(iYear-1), 12 );
		}
		else
		{
			lpInfo->wDayFirstPrevM = GetDayofTheMonth( iYear, (WORD)(iMonth-1) );
		}
		lpInfo->wDayFirstPrevM -= (lpInfo->nPosDayFirst - 1);
	}
	else
	{
		lpInfo->wDayFirstPrevM = 0;
	}
	//��һ�·�
	lpInfo->wDayEndNextM = (DAYS_COL * DAYS_ROW) - nPosDayEnd;
}

// ********************************************************************
//������void	DoActiveApp( HWND hWnd )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������WM_ACTIVATE �������
//����: 
// ********************************************************************
void	DoActiveApp( HWND hWnd )
{
	LPDATESELINFO	lpInfo;

	lpInfo = (LPDATESELINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return ;
	if( (lpInfo->dwDateStyle & DSS_USERSEL) && lpInfo->hWndUserSel )
	{
		PostMessage( lpInfo->hWndUserSel, lpInfo->uMsgUserSel, (WPARAM)hWnd, FALSE );
	}
}



// ********************************************************************
//������static BOOL IsToday(LPDATESELINFO lpInfo,WORD wDays)
//������
//  IN lpInfo - DATESEL������Ϣ
//	IN wDay - ��ǰ������
//����ֵ��
//	ָ���������ǽ��գ��򷵻�TRUE�����򷵻�FALSE
//�����������õ�ָ���������Ƿ��ǽ���
//����: 
// ********************************************************************
static BOOL IsToday(LPDATESELINFO lpInfo,WORD wDays)
{
	if ((lpInfo->stToday.wYear == lpInfo->wCaleYear) && 
		(lpInfo->stToday.wMonth == lpInfo->wCaleMonth) &&
		(lpInfo->stToday.wDay == wDays))
	{
		// �ǽ���
		return TRUE;
	}

	return FALSE;
}

// ********************************************************************
//������static void DrawWeek(HWND hWnd,HDC hdc, const LPRECT lprtPaint)
//������
//  IN hdc - �豸���
//	IN lprtPaint - Ҫ���Ƶľ���
//����ֵ��
//	��
//������������������
//����: 
// ********************************************************************
static void DrawWeek(HWND hWnd,HDC hdc, const LPRECT lprtPaint)
{
	RECT		rtTmp;
	RECT		rtClient;
	RECT		rtDrawWeek;
	LPDATESELINFO	lpInfo;

	LONG		nAverage;
	LONG		xUnit;
	LONG		xPos;
	LONG		nMax;
//	LONG		yUnit;
	COLORREF colOld;


		lpInfo = (LPDATESELINFO)GetWindowLong(hWnd,0);
		if (lpInfo==NULL)
			return ;
		GetClientRect( hWnd, &rtClient );
		SetRect( &rtDrawWeek, rtClient.left, YPOS_WEEK, rtClient.right, YPOS_WEEK+H_WEEK );
//		nAverage  = RECT_CX(&rtDrawWeek)%ARR_COUNT(g_pszWeek);
//		nAverage /= 2;
		nAverage  = AVERAGE(&rtClient);

		if( IntersectRect( &rtTmp, lprtPaint, &rtDrawWeek) )
		{
			//����
			//top and bottom
			rtTmp.top    = rtDrawWeek.top;
			rtTmp.bottom = rtDrawWeek.bottom;
			//left and right, and xPos
			xUnit = RECT_CX(&rtDrawWeek)/ARR_COUNT(g_pszWeek);
			if( rtTmp.left<(xUnit+nAverage) )
			{
				xPos = 0;
				rtTmp.left = rtDrawWeek.left + nAverage;
			}
			else
			{
				xPos = lprtPaint->left / xUnit;
				if( xPos>DAYS_COL )
					xPos = DAYS_COL;
				rtTmp.left = rtDrawWeek.left + xPos * xUnit + nAverage;
			}
			rtTmp.right = rtTmp.left + xUnit;
			//nMax
			nMax = (lprtPaint->right + xUnit -1) / xUnit;
			if( nMax>DAYS_COL )
				nMax = DAYS_COL;

			//Draw week
			colOld = SetTextColor( hdc, lpInfo->cl_Title );
			for( ; xPos<nMax; xPos++ )
			{
				if (xPos == 0 || xPos == 6)
				{
					// ������ ��������
					SetTextColor( hdc, RGB(0XFF,0,0) );
				}
				else
				{
					SetTextColor( hdc, lpInfo->cl_Text );
				}
				DrawText( hdc, g_pszWeek[xPos], -1, &rtTmp, DT_SINGLELINE|DT_VCENTER|DT_CENTER );
				//
				rtTmp.left  += xUnit;
				rtTmp.right += xUnit;
			}
			SetTextColor( hdc, colOld );
		}
}

// ********************************************************************
//������static void DrawDaysInMonth(HWND hWnd,HDC hdc,const LPRECT lprtPaint,BOOL bRefreshBK)
//������
//  IN hdc - �豸���
//	IN lprtPaint - Ҫ���Ƶľ���
//  IN bRefreshBK -- �Ƿ�Ҫˢ�±���

//����ֵ��
//	��
//��������������������е���
//����: 
// ********************************************************************
static void DrawDaysInMonth(HWND hWnd,HDC hdc,const LPRECT lprtPaint,BOOL bRefreshBK)
{

	LPDATESELINFO	lpInfo;
	
	HFONT		hFontOld;
	COLORREF	colOld;

	TCHAR		szTmp[16];
	RECT		rtTmp;
	RECT		rtClient;
	RECT		rtDrawDays;
	WORD		wDay;

	int iRow,iCol;
	int i;


		lpInfo = (LPDATESELINFO)GetWindowLong(hWnd,0);
		if (lpInfo==NULL)
			return ;
		GetClientRect( hWnd, &rtClient );
		SetRect( &rtDrawDays, rtClient.left, YPOS_DAY, rtClient.right,  rtClient.bottom );

//		nAverage  = AVERAGE(&rtClient);

		if( lprtPaint == NULL || IntersectRect( &rtTmp, lprtPaint, &rtDrawDays) )
		{
			if (lprtPaint == NULL)
			{
				rtTmp = rtDrawDays;
			}
			//���� ����
			if (bRefreshBK)
			{
				HBRUSH	hBrh;

					hBrh = GetSysColorBrush(COLOR_WINDOW);
					FillRect(hdc,&rtTmp,hBrh);
			}

			//
			hFontOld = NULL;
			if( lpInfo->wFlagFont!=DAYSFONT_DEFAULT )
			{
				if( lpInfo->wFlagFont==DAYSFONT_8X8 )
				{
					hFontOld = SelectObject( hdc, GetStockObject(SYSTEM_FONT_ENGLISH8X8) );
				}
				else if( lpInfo->wFlagFont==DAYSFONT_8X6 )
				{
					hFontOld = SelectObject( hdc, GetStockObject(SYSTEM_FONT_ENGLISH8X6) );
				}
			}
			//Draw ǰһ�·�
			if( lpInfo->dwDateStyle & DSS_SHOWMPREV )
			{
				if( lpInfo->wDayFirstPrevM )
				{
					iCol = 0;
					colOld = SetTextColor( hdc, lpInfo->cl_MonthOther );
					for( wDay=lpInfo->wDayFirstPrevM, i = 0; i<lpInfo->nPosDayFirst; wDay++ ,i ++)
					{
						GetDaysRect(0,iCol,&rtClient,&rtTmp);
						{
							sprintf( szTmp, TEXT("%d"), wDay );
							DrawText( hdc, szTmp, -1, &rtTmp, DT_SINGLELINE|DT_VCENTER|DT_CENTER );
						}
						iCol++;
					}
					SetTextColor( hdc, colOld );
				}
			}
			//Draw ��ǰ�·�
			colOld = SetTextColor( hdc, lpInfo->cl_Text );
			for( iCol = lpInfo->nPosDayFirst, wDay=1,iRow = 0; wDay<=lpInfo->wCaleDaysOfMonth; iCol++, wDay++ )
			{
				//�Ƿ��� ������ \r\n �Ĳ���
				if( iCol>=DAYS_COL )
				{
					iCol = 0;
					iRow ++;
				}
				GetDaysRect(iRow,iCol,&rtClient,&rtTmp);
				if( wDay!=lpInfo->wCaleDaySel )
					DrawDay( lpInfo, hdc, &rtTmp, wDay, FALSE ,iCol,FALSE);
				else
					DrawDay( lpInfo, hdc, &rtTmp, wDay, TRUE ,iCol,TRUE);
			}
			SetTextColor( hdc, colOld );

			//
			//Draw ��һ�·�
			if( lpInfo->dwDateStyle & DSS_SHOWMNEXT )
			{
				colOld = SetTextColor( hdc, lpInfo->cl_MonthOther );
				for( wDay=1; wDay<=lpInfo->wDayEndNextM; iCol++, wDay++ )
				{
					if( iCol>=DAYS_COL )
					{
						iCol = 0;
						iRow ++;
					}
					GetDaysRect(iRow,iCol,&rtClient,&rtTmp);
					{
						sprintf( szTmp, TEXT("%d"), wDay );
						DrawText( hdc, szTmp, -1, &rtTmp, DT_SINGLELINE|DT_VCENTER|DT_CENTER );
					}
				}
				SetTextColor( hdc, colOld );
			}
			//
			if( lpInfo->wFlagFont!=DAYSFONT_DEFAULT && hFontOld )
			{
				SelectObject( hdc, hFontOld );
			}
		}
}

/******************************************************************
������static BOOL DoEraseBkgnd( HWND hWnd, HDC hdc )
������
	IN  hWnd-���ھ��
	IN  hdc-��ͼDC���
����ֵ��
	TRUE
����������
	���� WM_ERASEBKGND ��Ϣ
*******************************************************************/
static int DoEraseBkgnd( HWND hWnd, HDC hdc )
{
	HPEN	hPen;
	HPEN	hPenOld;
	HBRUSH	hBrh;
	LPDATESELINFO	lpInfo;
	RECT rtClient,rtDraw;

		GetClientRect( hWnd, &rtClient );

		lpInfo = (LPDATESELINFO)GetWindowLong(hWnd,0);
		if (lpInfo==NULL)
			return 0;

		hPen = CreatePen( PS_SOLID, 1, lpInfo->cl_Text );
		hPenOld = SelectObject( hdc, (HPEN)hPen );
		// �������ѡ��ı���
		hBrh = GetSysColorBrush(COLOR_STATIC);
		SetRect( &rtDraw, rtClient.left, YPOS_YEARMONTH, rtClient.right,  YPOS_YEARMONTH + H_YEARMONTH);
		FillRect(hdc,&rtDraw,hBrh);
		MoveToEx( hdc, rtDraw.left, rtDraw.bottom, NULL);
		LineTo( hdc , rtDraw.right, rtDraw.bottom);
		// ��䵱��ũ���ı���
		hBrh = GetSysColorBrush(COLOR_STATIC);
		SetRect( &rtDraw, rtClient.left, YPOS_LUNAR, rtClient.right,  YPOS_LUNAR + H_LUNAR);
		FillRect(hdc,&rtDraw,hBrh);
		MoveToEx( hdc, rtDraw.left, rtDraw.bottom, NULL);
		LineTo( hdc , rtDraw.right, rtDraw.bottom);

		// ������ڵı���
		hBrh = GetSysColorBrush(COLOR_WINDOW);
		SetRect( &rtDraw, rtClient.left, YPOS_WEEK, rtClient.right,  YPOS_WEEK + H_WEEK);
		FillRect(hdc,&rtDraw,hBrh);
		MoveToEx( hdc, rtDraw.left, rtDraw.bottom, NULL);
		LineTo( hdc , rtDraw.right, rtDraw.bottom);
		// ���������ڵı���
		hBrh = GetSysColorBrush(COLOR_WINDOW);
		SetRect( &rtDraw, rtClient.left, YPOS_DAY, rtClient.right,  YPOS_DAY + H_DAY);
		FillRect(hdc,&rtDraw,hBrh);

		SelectObject( hdc, (HPEN)hPenOld );
		DeleteObject( hPen );
		return 0;
}


// ********************************************************************
//������static void DrawLunar(HWND hWnd,HDC hdc,BOOL bRefreshBK)
//������
//  IN hWnd -- ���ھ��
//  IN hdc - �豸���
//  IN bRefreshBK -- �Ƿ�Ҫˢ�±���

//����ֵ��
//	��
//��������������������е���
//����: 
// ********************************************************************
static void DrawLunar(HWND hWnd,HDC hdc,BOOL bRefreshBK)
{
	LPDATESELINFO	lpInfo;
	WORD wLunarYear,wLunarMonth,wLunarDay;
	BOOL bLeap;
	TCHAR pszLanur[32];
	RECT rtLunar;
	RECT rtClient;
#ifndef DEF_16FONT
	int iWeek;
#endif
	COLORREF colOld;
	
		lpInfo = (LPDATESELINFO)GetWindowLong(hWnd,0);
		if (lpInfo==NULL)
			return ;

		colOld = SetTextColor( hdc, lpInfo->cl_Text );
		GetClientRect( hWnd, &rtClient );

		SetRect( &rtLunar, rtClient.left, YPOS_LUNAR, rtClient.right,  YPOS_LUNAR + H_LUNAR -1);
		if (bRefreshBK)
		{
			// ��䵱��ũ���ı���
			HBRUSH hBrh;

			hBrh = GetSysColorBrush(COLOR_STATIC);
			FillRect(hdc,&rtLunar,hBrh);
		}

		// �õ���ǰѡ�����ũ��
		if( !Lunar_GetYMDFromAD( lpInfo->wCaleYear, lpInfo->wCaleMonth, lpInfo->wCaleDaySel, &wLunarYear, 
								&wLunarMonth, &wLunarDay, &bLeap ) )
		{  
			// û��������ֱ����ʾ���������ڼ�
			goto DRAWWEEK ;
		}

//		Lunar_GetNameYMD( wLunarYear, wLunarMonth, wLunarDay, pszLanur );
		Lunar_GetNameYear( wLunarYear, pszLanur );
		strcat(pszLanur ,"��");
		Lunar_GetNameMonth( wLunarMonth, pszLanur + strlen(pszLanur) );
		Lunar_GetNameDay( wLunarMonth, wLunarDay,pszLanur + strlen(pszLanur) );
		

		DrawText( hdc, pszLanur, -1, &rtLunar, DT_SINGLELINE|DT_VCENTER);

DRAWWEEK:
		// ��ʾ���ڼ�
#ifndef DEF_16FONT

		iWeek = ( lpInfo->nPosDayFirst + lpInfo->wCaleDaySel -1) % 7;

		sprintf(pszLanur,"(%s)",g_pszWeek[iWeek]);
		rtLunar.left = rtLunar.right - 48;
/*		if (iWeek == 0 || iWeek == 6)
		{
			// ������ ��������
			SetTextColor( hdc, RGB(0XFF,0,0) );
		}
		else
		{
			SetTextColor( hdc, lpInfo->cl_Text );
		}
*/
		DrawText( hdc, pszLanur, -1, &rtLunar, DT_SINGLELINE|DT_VCENTER);
#endif
		SetTextColor( hdc, colOld );
		return ;
}

// ********************************************************************
//������static void GetDaysRect(int iRow,int iCol,LPRECT lprcClient,LPRECT lpRect)
//������
//  IN iRow -- ָ����
//  IN iCol - ָ����
//  IN lprcClient -- ָ�����ڵĴ�С
//  OUT lpRect -- ����ָ����ľ���

//����ֵ��
//	��
//��������������������е���
//����: 
// ********************************************************************
static void GetDaysRect(int iRow,int iCol,LPRECT lprcClient,LPRECT lpRect)
{
	int xUnit;
	int nAverage;

		nAverage  = AVERAGE(lprcClient);
		xUnit = (lprcClient->right - lprcClient->left)/DAYS_COL;

		//����ǰѡ����--- ���Ƴ�ѡ��
		lpRect->left  = lprcClient->left + iCol * xUnit + nAverage;
		lpRect->right = lpRect->left + xUnit;
		lpRect->top   = YPOS_DAY  + iRow * ROW_HEIGHT;
		lpRect->bottom = lpRect->top  + ROW_HEIGHT;
}

// ********************************************************************
// ������static void DoGoToday(HWND hWnd, LPDATESELINFO lpInfo)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN lpInfo - DATESEL������Ϣ
// ����ֵ��
//	��
// ���������������£������Ƴ���
// ����: 
// ********************************************************************
static void DoGoToday(HWND hWnd, LPDATESELINFO lpInfo)
{
		if ((lpInfo->wCaleYear == lpInfo->stToday.wYear)&&
			(lpInfo->wCaleMonth == lpInfo->stToday.wMonth)&&
			(lpInfo->wCaleDaySel == lpInfo->stToday.wDay))
		{
			// ��ǰѡ���Ѿ��ǽ�����
			return ;
		}
		lpInfo->wCaleYear = lpInfo->stToday.wYear;
		lpInfo->wCaleMonth = lpInfo->stToday.wMonth;
		lpInfo->wCaleDaySel = lpInfo->stToday.wDay;

		ReadyData( lpInfo );
		hlp_ShowYear( hWnd, IDC_YEAR, lpInfo->wCaleYear );
		hlp_ShowMonth( hWnd, IDC_MONTH, lpInfo->wCaleMonth );
		RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, ����
}
