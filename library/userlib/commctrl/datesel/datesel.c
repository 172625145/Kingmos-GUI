/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：日期设置类
版本号：  1.0.0
开发时期：2003-06-06
作者：    肖远钢
修改记录：
2003-06-05: DoLButtonDown 增加 SetFocus
2003-04-21: 将InvalidateRect 改为RRefreshWindowDays, 修改坐标 by LN
2003-04-17: 将DATETIME 结构改为 SYSTEMTIME结构 ， by LN
******************************************************/

//----------------------------------------------------
//
//文件解说：
//1.  DATESELINFO保存该控件DateSel的所有属性和功能，在DoCreate分配，在DoDestroy释放
//2.  主界面是显示当前或指定的年（static show）和更改年的按钮、月(combobox)、日（draw days and clickable）
//3.  Change year by the inc/dec button
//    Change month by selecting from its combobox
//    Change day by clicking the other days, the day is slected when click twice, 
//    并会给用户(hWndUserSel)1个已经注册的通知消息(uMsgUserSel)
//4.  当用户Click控件DateSel的外部，导致 消息WM_ACTIVATE(wParam==WA_INACTIVE)时，
//    会给用户(hWndUserSel)1个取消选择的通知消息(uMsgUserSel)
//5.  提供给用户的特别功能，参考edatesel.h的消息
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

/***************  全局区 定义， 声明 *****************/

//#define _MAPPOINTER
//---------------------------------------------------
static	const	TCHAR		g_pszWeek[][4]= {"日", "一", "二", "三", "四", "五", "六"};

//---------------------------------------------------
#define YEAR_MIN		0
#define YEAR_MAX		9999

#define	DAYS_COL		7
#define	DAYS_ROW		6

#define DEF_16FONT
//#define DEF_24FONT


#ifdef DEF_16FONT

//控件 位置
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


#define	H_WEEK			22						//WEEK控件 高度
#define ROW_HEIGHT		12
#define	H_DAY			(DAYS_ROW * ROW_HEIGHT)			//DAY控件  最小高度

//#define	YPOS_WEEK		(3 + 20 + 1)			//WEEK控件 位置
//#define	YPOS_DAY		(YPOS_WEEK + H_WEEK)//DAY控件  位置
#define	YPOS_WEEK		(YPOS_LUNAR + H_LUNAR + 1)//(H_WEEK + 4)//(3 + 20 + 1)			//WEEK控件 位置
#define	YPOS_DAY		(YPOS_WEEK + H_WEEK + 1)//DAY控件  位置

#define	W_MON			63						//月控件 宽度
#define	W_APP			120						//AP 最小宽度
#define	H_APP			(YPOS_DAY + H_DAY)		//AP 最小高度

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

//控件 位置
#define	H_WEEK			26//22						//WEEK控件 高度
#define ROW_HEIGHT		26
#define	H_DAY			(DAYS_ROW * ROW_HEIGHT) //18)			//DAY控件  最小高度

#define	YPOS_WEEK		(YPOS_LUNAR + H_LUNAR + 1)//(H_WEEK + 4)//(3 + 20 + 1)			//WEEK控件 位置
#define	YPOS_DAY		(YPOS_WEEK + H_WEEK + 1)//DAY控件  位置

#define	W_MON			90//63						//月控件 宽度
#define	W_APP			120						//AP 最小宽度
#define	H_APP			(YPOS_DAY + H_DAY)		//AP 最小高度

#endif

//颜色
#define	RGB_DAYSGRAY			RGB(192, 192, 192)

//控件
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
	HWND	hWndUserSel;		//用户点击控件之外区域时 发出消息
	UINT	uMsgUserSel;

	WORD	wCaleYearMin;		//年 Min
	WORD	wCaleYearMax;		//年 Max

	WORD	wCaleYear;			//年
	WORD	wCaleMonth;			//月
	WORD	wCaleDaysOfMonth;	//当前月的天数
	SHORT	nPosDayFirst;		//当前月1日 的索引显示位置，或者是星期几

	WORD	wCaleDaySel;		//当前月 当前选中   的日期
	WORD	wFlagFont;			//选择 显示日期的字体

	WORD	wDayFirstPrevM;		//前一个月显示的开始日期
	WORD	wDayEndNextM;		//后一个月显示的结束日期

	//
	COLORREF cl_Text;
	COLORREF cl_TextBk;     //正常文本的前景与背景色
	COLORREF cl_Selection;
	COLORREF cl_SelectionBk;  // 选择文本的前景与背景色
//	COLORREF cl_Disable;
//	COLORREF cl_DisableBk;    // 无效文本的前景与背景色
//	COLORREF cl_ReadOnly;
//	COLORREF cl_ReadOnlyBk;   // 只读文本的前景与背景色
	COLORREF cl_Title;
	COLORREF cl_TitleBk;      // 标题文本的前景与背景色

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
//声明：ATOM RegisterDateSelClass(HINSTANCE hInstance)
//参数：
//	IN hInstance - 应用程序的实例句柄
//返回值：
//	成功返回注册成功后标志该类的 ATOM ， 否则返回 0
//功能描述：注册应用程序类
//引用: 
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
//声明：LRESULT CALLBACK WndProc_DateSel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN message - 需要处理的消息
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：应用程序窗口处理过程
//引用: 
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
//声明：BOOL	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN idCtl - 消息参数
//  IN pDrawItem - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：WM_DRAWITEM 处理过程
//引用: 
// ********************************************************************
//#define	PAD_DRAWFOCUS
//WM_DRAWITEM的消息处理
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
	// 通用绘制
	if( pDrawItem->itemAction == ODA_DRAWENTIRE )		
	{
		//nState = 0;
		//if( pDrawItem->itemState & ODS_DISABLED )		// disable
		//else
	}
	// 选中绘制	
	else if( pDrawItem->itemAction == ODA_SELECT )	
	{
		if( pDrawItem->itemState & ODS_SELECTED )		//得到 Select
		{
//			edge = BDR_SUNKENINNER;
			DrawEdge( pDrawItem->hDC, &pDrawItem->rcItem, BDR_SUNKENINNER, BF_RECT );
		}
		else
		{
			DrawEdge( pDrawItem->hDC, &pDrawItem->rcItem, 0, BF_MIDDLE );
		}
		//else
		//	nState = 0;								//失去 Select
//#ifdef	PAD_DRAWFOCUS
//		if( pDrawItem->itemState & ODS_FOCUS )		//有焦点、没有焦点
//			fFocus = TRUE;
//#endif
	}
#ifdef	PAD_DRAWFOCUS
	//焦点操作，不用绘制
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

			hIcon = LoadImage( NULL, MAKEINTRESOURCE(IDI_GOTODAY), IMAGE_ICON, 24, 24, LR_DEFAULTSIZE ) ; // 装载应用程序资源
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
//声明：BOOL	hlp_ShowYear( HWND hWnd, int nIDDlgItem, WORD wYear)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//	IN nIDDlgItem - 控件ID
//	IN wYear - 要显示的年
//返回值：
//	返回TRUE
//功能描述：在控件上显示年
//引用: 
// ********************************************************************
static	BOOL	hlp_ShowYear( HWND hWnd, int nIDDlgItem, WORD wYear)
{
	TCHAR	pszTmp[12];
	sprintf( pszTmp, TEXT("%4d"), wYear );
	return SetWindowText( GetDlgItem( hWnd, nIDDlgItem ), pszTmp );	
}

// ********************************************************************
//声明：BOOL	hlp_ShowMonth( HWND hWnd, int nIDDlgItem, WORD wMonth)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//	IN nIDDlgItem - 控件ID
//	IN wYear - 要显示的年
//返回值：
//	返回TRUE
//功能描述：在控件上显示年
//引用: 
// ********************************************************************
static	BOOL	hlp_ShowMonth( HWND hWnd, int nIDDlgItem, WORD wMonth)
{
	TCHAR	pszTmp[12];
	sprintf( pszTmp, TEXT("%02d月"), wMonth );
	return SetWindowText( GetDlgItem( hWnd, nIDDlgItem ), pszTmp );	
}
// ********************************************************************
//声明：VOID RefreshWindowDays( HWND hWnd, RECT* prtDraw )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//	IN prtDraw - 指定的矩形范围内
//返回值：
//	无
//功能描述：重新绘制所有Days
//引用: 
// ********************************************************************
//LN:2003-04-21, 增加
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
		//绘制 日-------开始
		iModeOld = SetBkMode( hdc, TRANSPARENT );
		DrawDaysInMonth(hWnd,hdc,NULL , TRUE);
		//  绘制农历
		DrawLunar(hWnd,hdc,TRUE);

		SetBkMode( hdc, iModeOld );
		//绘制 年、月、日-------结束
		ReleaseDC( hWnd, hdc );
}
//

// ********************************************************************
//声明：void	DoPaint( HWND hWnd, HDC hdcDraw, const LPRECT lprtPaint )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN hdcDraw - 绘图HDC句柄
//  IN lprtPaint - 绘图的矩形
//返回值：
//	消息处理后返回的结果
//功能描述：WM_PAINT 处理过程
//引用: 
// ********************************************************************
void	DoPaint( HWND hWnd, HDC hdcDraw, const LPRECT lprtPaint )
{
	int			iModeOld;
	
		//
		iModeOld = SetBkMode( hdcDraw, TRANSPARENT );
		//  绘制农历
		DrawLunar(hWnd,hdcDraw,FALSE);
		//绘制 week
		//
		DrawWeek(hWnd,hdcDraw, lprtPaint);
		//绘制 日-------开始
		DrawDaysInMonth(hWnd,hdcDraw,lprtPaint , FALSE);
		//绘制 年、月、日-------结束
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
//声明：LRESULT DoCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	返回 0
//功能描述：WM_CREATE 处理过程
//引用: 
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
	{	TEXT(" 一月"),	TEXT(" 二月"),	TEXT(" 三月"),	TEXT(" 四月"),	TEXT(" 五月"),	TEXT(" 六月"),
		TEXT(" 七月"),	TEXT(" 八月"),	TEXT(" 九月"),	TEXT(" 十月"),	TEXT("十一月"),	TEXT("十二月")
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
	lpInfo->cl_TextBk      = GetSysColor( COLOR_BACKGROUND );     //正常文本的前景与背景色
	lpInfo->cl_Selection   = GetSysColor( COLOR_HIGHLIGHTTEXT );//CL_WHITE
	lpInfo->cl_SelectionBk = GetSysColor( COLOR_HIGHLIGHT );  // 选择文本的前景与背景色
//	lpInfo->cl_Disable     = ;
//	lpInfo->cl_DisableBk   = ;    // 无效文本的前景与背景色
//	lpInfo->cl_ReadOnly    = ;
//	lpInfo->cl_ReadOnlyBk  = ;   // 只读文本的前景与背景色
	lpInfo->cl_Title       = GetSysColor( COLOR_CAPTIONTEXT );//CL_WHITE
	lpInfo->cl_TitleBk     = GetSysColor( COLOR_ACTIVECAPTION );	       // 标题文本的前景与背景色

	lpInfo->cl_MonthOther  = RGB_DAYSGRAY;

	GetLocalTime(&lpInfo->stToday);

    SetWindowLong(hWnd,0,(DWORD)lpInfo);
    return 0;
}

// ********************************************************************
//声明：void	DoDestroy( HWND hWnd )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//返回值：
//	无
//功能描述：WM_DESTROY 处理过程
//引用: 
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
//声明：LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	无
//功能描述：WM_LBUTTONDOWN 处理过程
//引用: 
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
	//得到索引号，并检查是否有效
	xUnit = (rtClient.right - rtClient.left)/DAYS_COL;
//	yUnit = (rtDrawDays.bottom - rtDrawDays.top)/DAYS_ROW;
	iRow  = (point.y-YPOS_DAY) / ROW_HEIGHT;
	iCol  = (point.x-rtClient.left)/ xUnit;
	iIndex  = (iRow * DAYS_COL) + iCol;
	//保证点击的是在当月
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

			//准备绘制---将前1个选择项 绘制成正常，将当前选择项 绘制成选择
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
			//将前1个选择项--- 绘制成正常
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

			//完成绘制
			if( lpInfo->wFlagFont!=DAYSFONT_DEFAULT && hFontOld )
			{
				SelectObject( hdc, hFontOld );
			}
			//  绘制农历
			DrawLunar(hWnd,hdc,TRUE);

			SetBkMode( hdc, iModeOld );
			ReleaseDC( hWnd, hdc );
		}
	}
	SetFocus( hWnd );//LN, 2003-06-05, 增加
	return 0;
}

// ********************************************************************
//声明：LRESULT	DoCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：WM_COMMAND 处理过程
//引用: 
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
			//InvalidateRect(hWnd,NULL,TRUE); //LN:2003-04-21, 删除
			RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, 增加
		}
	}
*/
	return 0;
}

// ********************************************************************
//声明：WORD	hlp_GetDayWeekFromFirst( WORD wDayWeekOk, WORD wDayGet )
//参数：
//	IN wDayWeekOk - 每月1号的星期数
//	IN wDayGet - 指定该月的日期
//返回值：
//	返回 计算得到的星期数
//功能描述：根据每月1号的星期数，计算该月指定日子的星期数，并返回
//引用: 
// ********************************************************************
//根据这个月1号是星期几，计算指定日子是星期几
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
//声明：LRESULT	DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN lpccs - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：WM_SETCTLCOLOR 处理过程
//引用: 
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
//声明：LRESULT DoGetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：DSM_GETDATETIME 处理过程
//引用: 
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
//声明：LRESULT DoSetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：DSM_SETDATETIME 处理过程
//引用: 
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

	//InvalidateRect(hWnd,NULL,TRUE);//LN:2003-04-21, 删除
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, 增加
	return TRUE;
}

// ********************************************************************
//声明：LRESULT DoSetDateFont(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：DSM_SETDAYSFONT 处理过程
//引用: 
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

	//InvalidateRect(hWnd,NULL,TRUE);//LN:2003-04-21, 删除
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, 增加
	return TRUE;
}
// ********************************************************************
//声明：LRESULT DoSetDateStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：DSM_SETDATESTYLE 处理过程
//引用: 
// ********************************************************************
static LRESULT DoSetDateStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDATESELINFO	lpInfo;

	lpInfo     = (LPDATESELINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	lpInfo->dwDateStyle = (DWORD)wParam;
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, 增加
	return TRUE;
}
// ********************************************************************
//声明：LRESULT DoSetClickOut(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：DSM_SETUSERSELNOTIFY 处理过程
//引用: 
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
//声明：LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：DSM_SETYEARRANGE 处理过程
//引用: 
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
//声明：void YearChange( HWND hWnd, LPDATESELINFO lpInfo )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN lpInfo - DATESEL所有信息
//返回值：
//	无
//功能描述：更改年，并绘制出来
//引用: 
// ********************************************************************
//cal the year data and draw year
static void YearChange( HWND hWnd, LPDATESELINFO lpInfo )
{
	ReadyData( lpInfo );
	hlp_ShowYear( hWnd, IDC_YEAR, lpInfo->wCaleYear );
	//InvalidateRect(hWnd,NULL,TRUE); //LN:2003-04-21, 删除
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, 增加
}

// ********************************************************************
// 声明：void MonthChange( HWND hWnd, LPDATESELINFO lpInfo )
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN lpInfo - DATESEL所有信息
// 返回值：
//	无
// 功能描述：更改月，并绘制出来
// 引用: 
// ********************************************************************
//cal the year data and draw year
static void MonthChange( HWND hWnd, LPDATESELINFO lpInfo )
{
	ReadyData( lpInfo );
	hlp_ShowMonth( hWnd, IDC_MONTH, lpInfo->wCaleMonth );
	//InvalidateRect(hWnd,NULL,TRUE); //LN:2003-04-21, 删除
	RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, 增加
}


// ********************************************************************
//声明：void	DrawDay( LPDATESELINFO lpInfo, HDC hdcDraw, RECT* prtDay, WORD wDay, BOOL fSel ,int iPos, BOOL bRefreshBK)
//参数：
//  IN lpInfo - DATESEL所有信息
//	IN hdcDraw - 绘制的HDC句柄
//	IN prtDay - 绘制的矩形
//	IN wDay - 绘制的数字
//	IN fSel - 是否为选中状态
//返回值：
//	无
//功能描述：绘制指定的日子
//引用: 
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
		{ // 星期日及星期六
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
		// 绘制今日的标志
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
//声明：void	ReadyData( LPDATESELINFO lpInfo )
//参数：
//  IN lpInfo - DATESEL所有信息
//返回值：
//	无
//功能描述：计算好这个月所需的数据
//引用: 
// ********************************************************************
void	ReadyData( LPDATESELINFO lpInfo )
{
	WORD	iYear;
	WORD	iMonth;
	WORD	nPosDayEnd;

	iYear  = lpInfo->wCaleYear;
	iMonth = lpInfo->wCaleMonth;
	//当前月
	lpInfo->wCaleDaysOfMonth = GetDayofTheMonth( iYear, iMonth );
    lpInfo->nPosDayFirst= GetWeekData( iYear, iMonth, 1 );
	nPosDayEnd   = lpInfo->wCaleDaysOfMonth + lpInfo->nPosDayFirst - 1;
	if( lpInfo->wCaleDaySel > lpInfo->wCaleDaysOfMonth )	//attention here
	{
		lpInfo->wCaleDaySel = lpInfo->wCaleDaysOfMonth;
	}
	//前一月份
	if( lpInfo->nPosDayFirst > 0 )	//不是星期日
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
	//后一月份
	lpInfo->wDayEndNextM = (DAYS_COL * DAYS_ROW) - nPosDayEnd;
}

// ********************************************************************
//声明：void	DoActiveApp( HWND hWnd )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//返回值：
//	消息处理后返回的结果
//功能描述：WM_ACTIVATE 处理过程
//引用: 
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
//声明：static BOOL IsToday(LPDATESELINFO lpInfo,WORD wDays)
//参数：
//  IN lpInfo - DATESEL所有信息
//	IN wDay - 当前的日期
//返回值：
//	指定的日期是今日，则返回TRUE，否则返回FALSE
//功能描述：得到指定的日期是否是今日
//引用: 
// ********************************************************************
static BOOL IsToday(LPDATESELINFO lpInfo,WORD wDays)
{
	if ((lpInfo->stToday.wYear == lpInfo->wCaleYear) && 
		(lpInfo->stToday.wMonth == lpInfo->wCaleMonth) &&
		(lpInfo->stToday.wDay == wDays))
	{
		// 是今日
		return TRUE;
	}

	return FALSE;
}

// ********************************************************************
//声明：static void DrawWeek(HWND hWnd,HDC hdc, const LPRECT lprtPaint)
//参数：
//  IN hdc - 设备句柄
//	IN lprtPaint - 要绘制的矩形
//返回值：
//	无
//功能描述：绘制星期
//引用: 
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
			//计算
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
					// 星期日 与星期六
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
//声明：static void DrawDaysInMonth(HWND hWnd,HDC hdc,const LPRECT lprtPaint,BOOL bRefreshBK)
//参数：
//  IN hdc - 设备句柄
//	IN lprtPaint - 要绘制的矩形
//  IN bRefreshBK -- 是否要刷新背景

//返回值：
//	无
//功能描述：绘制这个月中的天
//引用: 
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
			//绘制 背景
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
			//Draw 前一月份
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
			//Draw 当前月份
			colOld = SetTextColor( hdc, lpInfo->cl_Text );
			for( iCol = lpInfo->nPosDayFirst, wDay=1,iRow = 0; wDay<=lpInfo->wCaleDaysOfMonth; iCol++, wDay++ )
			{
				//是否换行 类似于 \r\n 的操作
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
			//Draw 后一月份
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
声明：static BOOL DoEraseBkgnd( HWND hWnd, HDC hdc )
参数：
	IN  hWnd-窗口句柄
	IN  hdc-绘图DC句柄
返回值：
	TRUE
功能描述：
	处理 WM_ERASEBKGND 消息
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
		// 填充年月选择的背景
		hBrh = GetSysColorBrush(COLOR_STATIC);
		SetRect( &rtDraw, rtClient.left, YPOS_YEARMONTH, rtClient.right,  YPOS_YEARMONTH + H_YEARMONTH);
		FillRect(hdc,&rtDraw,hBrh);
		MoveToEx( hdc, rtDraw.left, rtDraw.bottom, NULL);
		LineTo( hdc , rtDraw.right, rtDraw.bottom);
		// 填充当天农历的背景
		hBrh = GetSysColorBrush(COLOR_STATIC);
		SetRect( &rtDraw, rtClient.left, YPOS_LUNAR, rtClient.right,  YPOS_LUNAR + H_LUNAR);
		FillRect(hdc,&rtDraw,hBrh);
		MoveToEx( hdc, rtDraw.left, rtDraw.bottom, NULL);
		LineTo( hdc , rtDraw.right, rtDraw.bottom);

		// 填充星期的背景
		hBrh = GetSysColorBrush(COLOR_WINDOW);
		SetRect( &rtDraw, rtClient.left, YPOS_WEEK, rtClient.right,  YPOS_WEEK + H_WEEK);
		FillRect(hdc,&rtDraw,hBrh);
		MoveToEx( hdc, rtDraw.left, rtDraw.bottom, NULL);
		LineTo( hdc , rtDraw.right, rtDraw.bottom);
		// 填充具体日期的背景
		hBrh = GetSysColorBrush(COLOR_WINDOW);
		SetRect( &rtDraw, rtClient.left, YPOS_DAY, rtClient.right,  YPOS_DAY + H_DAY);
		FillRect(hdc,&rtDraw,hBrh);

		SelectObject( hdc, (HPEN)hPenOld );
		DeleteObject( hPen );
		return 0;
}


// ********************************************************************
//声明：static void DrawLunar(HWND hWnd,HDC hdc,BOOL bRefreshBK)
//参数：
//  IN hWnd -- 窗口句柄
//  IN hdc - 设备句柄
//  IN bRefreshBK -- 是否要刷新背景

//返回值：
//	无
//功能描述：绘制这个月中的天
//引用: 
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
			// 填充当天农历的背景
			HBRUSH hBrh;

			hBrh = GetSysColorBrush(COLOR_STATIC);
			FillRect(hdc,&rtLunar,hBrh);
		}

		// 得到当前选择天的农历
		if( !Lunar_GetYMDFromAD( lpInfo->wCaleYear, lpInfo->wCaleMonth, lpInfo->wCaleDaySel, &wLunarYear, 
								&wLunarMonth, &wLunarDay, &bLeap ) )
		{  
			// 没有阴历，直接显示今天是星期几
			goto DRAWWEEK ;
		}

//		Lunar_GetNameYMD( wLunarYear, wLunarMonth, wLunarDay, pszLanur );
		Lunar_GetNameYear( wLunarYear, pszLanur );
		strcat(pszLanur ,"年");
		Lunar_GetNameMonth( wLunarMonth, pszLanur + strlen(pszLanur) );
		Lunar_GetNameDay( wLunarMonth, wLunarDay,pszLanur + strlen(pszLanur) );
		

		DrawText( hdc, pszLanur, -1, &rtLunar, DT_SINGLELINE|DT_VCENTER);

DRAWWEEK:
		// 显示星期几
#ifndef DEF_16FONT

		iWeek = ( lpInfo->nPosDayFirst + lpInfo->wCaleDaySel -1) % 7;

		sprintf(pszLanur,"(%s)",g_pszWeek[iWeek]);
		rtLunar.left = rtLunar.right - 48;
/*		if (iWeek == 0 || iWeek == 6)
		{
			// 星期日 与星期六
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
//声明：static void GetDaysRect(int iRow,int iCol,LPRECT lprcClient,LPRECT lpRect)
//参数：
//  IN iRow -- 指定行
//  IN iCol - 指定列
//  IN lprcClient -- 指定窗口的大小
//  OUT lpRect -- 返回指定天的矩形

//返回值：
//	无
//功能描述：绘制这个月中的天
//引用: 
// ********************************************************************
static void GetDaysRect(int iRow,int iCol,LPRECT lprcClient,LPRECT lpRect)
{
	int xUnit;
	int nAverage;

		nAverage  = AVERAGE(lprcClient);
		xUnit = (lprcClient->right - lprcClient->left)/DAYS_COL;

		//将当前选择项--- 绘制成选择
		lpRect->left  = lprcClient->left + iCol * xUnit + nAverage;
		lpRect->right = lpRect->left + xUnit;
		lpRect->top   = YPOS_DAY  + iRow * ROW_HEIGHT;
		lpRect->bottom = lpRect->top  + ROW_HEIGHT;
}

// ********************************************************************
// 声明：static void DoGoToday(HWND hWnd, LPDATESELINFO lpInfo)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN lpInfo - DATESEL所有信息
// 返回值：
//	无
// 功能描述：更改月，并绘制出来
// 引用: 
// ********************************************************************
static void DoGoToday(HWND hWnd, LPDATESELINFO lpInfo)
{
		if ((lpInfo->wCaleYear == lpInfo->stToday.wYear)&&
			(lpInfo->wCaleMonth == lpInfo->stToday.wMonth)&&
			(lpInfo->wCaleDaySel == lpInfo->stToday.wDay))
		{
			// 当前选择已经是今天了
			return ;
		}
		lpInfo->wCaleYear = lpInfo->stToday.wYear;
		lpInfo->wCaleMonth = lpInfo->stToday.wMonth;
		lpInfo->wCaleDaySel = lpInfo->stToday.wDay;

		ReadyData( lpInfo );
		hlp_ShowYear( hWnd, IDC_YEAR, lpInfo->wCaleYear );
		hlp_ShowMonth( hWnd, IDC_MONTH, lpInfo->wCaleMonth );
		RefreshWindowDays( hWnd, NULL );//LN:2003-04-21, 增加
}
