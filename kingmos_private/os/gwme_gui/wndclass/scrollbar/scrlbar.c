/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：滚动棒类(ScrollBar)
版本号：1.0.0
开发时期：1999-03-06
作者：李林
修改记录：
		LN 2004-12-21增加 自动处理鼠标消息和timer消息
        LN, 2003-07-12, 除零错
******************************************************/

#include <eframe.h>
#include <esymbols.h>
#include <eassert.h>
//#include <eapisrv.h>
#include "..\..\include\scrollbar.h"

//#include <epwin.h>
//#include <winsrv.h>
//#include <gdisrv.h>

#ifdef EML_DOS
    #include <emouse.h>
#endif

// this is all message handle proc
static LRESULT DoCREATE( HWND hWnd );
static LRESULT DoPAINT( HWND );
static LRESULT CALLBACK ScrollBarWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
static int Scrl_SetScrollState( HWND hWnd, int fnBar, UINT s, BOOL fEnable );
static int Scrl_GetScrollValue( HWND hWnd, int fnBar, int pos );
_SCROLLDATA * _GetWindowScrollBar( HWND hWnd, UINT type );

extern BOOL _GetFrameSize( SIZE FAR* lpSize, DWORD dwMainStyle );


static const char classSCROLLBAR[]="ScrollBar";

typedef struct _SCRLBAR_BRUSH
{
	HBRUSH hbrushVSBackground;
	HBRUSH hbrushVSThumb;
	HBRUSH hbrushHSBackground;
	HBRUSH hbrushHSThumb;
	HBRUSH hbrushCorner;
	HBRUSH hbrushUpArrow;
	HBRUSH hbrushDownArrow;
	HBRUSH hbrushLeftArrow;
	HBRUSH hbrushRightArrow;
}SCRLBAR_BRUSH, FAR * LPSCRLBAR_BRUSH;

static HBITMAP hbmpVSBackground = NULL;
static HBITMAP hbmpVSThumb= NULL;
static HBITMAP hbmpHSBackground = NULL;
static HBITMAP hbmpHSThumb= NULL;
static HBITMAP hbmpCorner= NULL;
static HBITMAP hbmpUpArrow= NULL;
static HBITMAP hbmpDownArrow= NULL;
static HBITMAP hbmpLeftArrow= NULL;
static HBITMAP hbmpRightArrow= NULL;

static SCRLBAR_BRUSH *lpscrl_brush = NULL;
SCRLBAR_BRUSH scrlBrush;

//  创建滚动棒用的刷子
static HBRUSH CreateScrollBarBrush( UINT id, HBITMAP *lphBitmap )
{
	//HBITMAP hBitmap;

	if( *lphBitmap == NULL )
		*lphBitmap = LoadImage( NULL, MAKEINTRESOURCE( id ), IMAGE_BITMAP, 0, 0, LR_SHARED );
	
	if( *lphBitmap )
	{
		LOGBRUSH lb;
		
		lb.lbColor = 0;
		lb.lbHatch = (LONG)*lphBitmap;
		lb.lbStyle = BS_PATTERN;
		return CreateBrushIndirect ( &lb ); 
	}
	return NULL;
}

static SCRLBAR_BRUSH * InitScrollBarBrush( ) //LPSCRLBAR_BRUSH lpscrlBrush )
{
	if( lpscrl_brush == NULL )
	{
	//if( hbmpVSBackground == NULL )
	//{ //第一次，初始化...
		scrlBrush.hbrushVSBackground = CreateScrollBarBrush( OBM_VSCROLLBAR_BACKGROUND, &hbmpVSBackground );
	//}
	
	//if( hbrushHSBackground == NULL )
	//{ //第一次，初始化...
		scrlBrush.hbrushHSBackground = CreateScrollBarBrush( OBM_HSCROLLBAR_BACKGROUND, &hbmpHSBackground );
	//}

	//if( hbrushVSThumb == NULL )
	//{ //第一次，初始化...
		scrlBrush.hbrushVSThumb = CreateScrollBarBrush( OBM_VSCROLLBAR_THUMB, &hbmpVSThumb );
	//}

	//if( hbrushHSThumb == NULL )
	//{ //第一次，初始化...
		scrlBrush.hbrushHSThumb = CreateScrollBarBrush( OBM_HSCROLLBAR_THUMB, &hbmpHSThumb );
	//}
	
	//if( hbrushCorner == NULL )
	//{
		scrlBrush.hbrushCorner = CreateScrollBarBrush( OBM_SCROLLBAR_CORNER, &hbmpCorner );
	//}
	//if( hbrushUpArrow == NULL )
	//{
		scrlBrush.hbrushUpArrow = CreateScrollBarBrush( OBM_VSCROLLBAR_UPARROW, &hbmpUpArrow );
	//}
	//if( hbrushDownArrow == NULL )
	//{
		scrlBrush.hbrushDownArrow = CreateScrollBarBrush( OBM_VSCROLLBAR_DOWNARROW, &hbmpDownArrow );
	//}
	//if( hbrushLeftArrow == NULL )
	//{
		scrlBrush.hbrushLeftArrow = CreateScrollBarBrush( OBM_HSCROLLBAR_LEFTARROW, &hbmpLeftArrow );
	//}
	//if( hbrushRightArrow == NULL )
	//{
		scrlBrush.hbrushRightArrow = CreateScrollBarBrush( OBM_HSCROLLBAR_RIGHTARROW, &hbmpRightArrow );
	//}
		lpscrl_brush = &scrlBrush;
	}
		
	return lpscrl_brush;
}

static BOOL DeinitScrollBarBrush( )//LPSCRLBAR_BRUSH lpscrlBrush )
{
	if( lpscrl_brush )
	{
		if( lpscrl_brush->hbrushVSBackground )
		{
			DeleteObject( lpscrl_brush->hbrushVSBackground );
			DeleteObject( hbmpVSBackground );
		}
		if( lpscrl_brush->hbrushHSBackground )
		{
			DeleteObject( lpscrl_brush->hbrushHSBackground );
			DeleteObject( hbmpHSBackground  );
		}
		if( lpscrl_brush->hbrushVSThumb )
		{
			DeleteObject( lpscrl_brush->hbrushVSThumb );
			DeleteObject( hbmpVSThumb );
		}
		if( lpscrl_brush->hbrushHSThumb )
		{
			DeleteObject( lpscrl_brush->hbrushHSThumb );
			DeleteObject( hbmpHSThumb );
		}
		if( lpscrl_brush->hbrushCorner )
		{
			DeleteObject( lpscrl_brush->hbrushCorner );
			DeleteObject( hbmpCorner );
		}
		if( lpscrl_brush->hbrushUpArrow )
		{
			DeleteObject( lpscrl_brush->hbrushUpArrow );
			DeleteObject( hbmpUpArrow );
		}
		if( lpscrl_brush->hbrushDownArrow )
		{
			DeleteObject( lpscrl_brush->hbrushDownArrow );
			DeleteObject( hbmpDownArrow );
		}
		if( lpscrl_brush->hbrushLeftArrow )
		{
			DeleteObject( lpscrl_brush->hbrushLeftArrow );
			DeleteObject( hbmpLeftArrow );
		}
		if( lpscrl_brush->hbrushRightArrow )
		{
			DeleteObject( lpscrl_brush->hbrushRightArrow );
			DeleteObject(  hbmpRightArrow );
		}
		lpscrl_brush = NULL;
	}
}

static void FillScrollRect( HDC hdc, RECT * lprc, HBRUSH hBrush )
{
	//SelectObject( hdc, hBrush );
	SetBrushOrgEx( hdc, lprc->left, lprc->top, NULL );
	//BitBlt( hdc, lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top, NULL, 0, 0, PATCOPY );
	FillRect( hdc, lprc, hBrush );
}

/*
VOID ResetBrushOrg( LPSCRLBAR_BRUSH lpscrlBrush, int x, int y )
{
	SetBrushOrgEx( lpscrlBrush->hbrushVSBackground = CreateScrollBarBrush( OBM_VSCROLLBAR_BACKGROUND, &hbmpVSBackground );
	lpscrlBrush->hbrushHSBackground = CreateScrollBarBrush( OBM_HSCROLLBAR_BACKGROUND, &hbmpHSBackground );
	lpscrlBrush->hbrushVSThumb = CreateScrollBarBrush( OBM_VSCROLLBAR_THUMB, &hbmpVSThumb );
	lpscrlBrush->hbrushHSThumb = CreateScrollBarBrush( OBM_HSCROLLBAR_THUMB, &hbmpHSThumb );
	lpscrlBrush->hbrushCorner = CreateScrollBarBrush( OBM_SCROLLBAR_CORNER, &hbmpCorner );
	lpscrlBrush->hbrushUpArrow = CreateScrollBarBrush( OBM_VSCROLLBAR_UPARROW, &hbmpUpArrow );
	lpscrlBrush->hbrushDownArrow = CreateScrollBarBrush( OBM_VSCROLLBAR_DOWNARROW, &hbmpDownArrow );
	lpscrlBrush->hbrushLeftArrow = CreateScrollBarBrush( OBM_HSCROLLBAR_LEFTARROW, &hbmpLeftArrow );
	lpscrlBrush->hbrushRightArrow = CreateScrollBarBrush( OBM_HSCROLLBAR_RIGHTARROW, &hbmpRightArrow );
}
*/
// **************************************************
// 声明：ATOM RegisterScrollBarClass(HINSTANCE hInst)
// 参数：
//		IN hInst - 实例句柄
// 返回值：
//		假如成功，返回非0值；失败，返回0
// 功能描述：
//		注册滚动棒类
// 引用: 
//		被sysclass.c 调用
// ************************************************

// register my define class
ATOM RegisterScrollBarClass(HINSTANCE hInst)
{
	WNDCLASS wc;
	ATOM atomClass;
//	HBITMAP hBitmap;
	
	wc.hInstance=hInst;
	wc.lpszClassName=classSCROLLBAR;
	// the proc is class function
	wc.lpfnWndProc=(WNDPROC)ScrollBarWndProc;
	wc.style=CS_DBLCLKS|CS_CLASSDC;
	wc.hIcon= 0;
	// at pen window, no cursor
	wc.hCursor= LoadCursor(NULL,IDC_ARROW);
	// to auto erase background, must set a valid brush
	// if 0, you must erase background yourself
	wc.hbrBackground = 0;
	wc.lpszMenuName=NULL;
	wc.cbClsExtra=0;
	// !!! it's important to save state of button, align to long
	wc.cbWndExtra=0;//sizeof( _SCROLLDATA );
	atomClass = RegisterClass( &wc );
	
	
	InitScrollBarBrush();
	//if( lpscrl_brush == NULL )
	//{
	//	lpscrl_brush = InitScrollBarBrush( &scrl_brush );
		//lpscrl_brush = &scrl_brush;
	//}
		
	return atomClass;
	
}

//得到窗口风格决定的滚动条
_SCROLLDATA * _GetWindowScrollBar( HWND hWnd, UINT type )
{
	_SCROLLDATA * lpret = NULL;

	if( type == SB_CTL )
	{
		DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		if( dwStyle & SBS_VERT )
		{
			type = SB_VERT;
		}
		else
		{
			type = SB_HORZ;
		}
	}
	//else
		//type = fnBar;

	if( type == SB_VERT )
	{
		lpret = (_SCROLLDATA *)GetWindowLong( hWnd, GWL_VSCROLL );//lpws->lpvScrollBar;
	}
	else if( type == SB_HORZ )
	{
		lpret = (_SCROLLDATA *)GetWindowLong( hWnd, GWL_HSCROLL );//lpws->lphScrollBar;
	}
	if( lpret == NULL )
	{  //第一次		
		//EnterCriticalSection( &csWindow );
		
		if( type == SB_VERT )
		{
			//lpret = lpws->lpvScrollBar = BlockHeap_Alloc( hgwmeBlockHeap, BLOCKHEAP_ZERO_MEMORY, sizeof(_SCROLLDATA) );
			lpret = calloc( sizeof(_SCROLLDATA), 1 );
			SetWindowLong( hWnd, GWL_VSCROLL, (LONG)lpret );
		}
		else if( type == SB_HORZ )
		{
			//lpret = lpws->lphScrollBar = BlockHeap_Alloc( hgwmeBlockHeap, BLOCKHEAP_ZERO_MEMORY, sizeof(_SCROLLDATA) ); 
			lpret = calloc( sizeof(_SCROLLDATA), 1 );
			SetWindowLong( hWnd, GWL_HSCROLL, (LONG)lpret );
		}
		if( lpret )
			Scrl_Inital( lpret );		
		//LeaveEnterCriticalSection( &csWindow );
	}
	ASSERT( lpret || type == SB_CORNER );
	return lpret;
}


VOID _ReleaseWindowScrollBar( HWND hWnd, UINT type )
{
	_SCROLLDATA * lpret = NULL;
	//int type;

	//EnterCriticalSection( &csWindow );
	if( type == SB_CTL )
	{
		DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		if( dwStyle & SBS_VERT )
		{
			type = SB_VERT;
		}
		else
		{
			type = SB_HORZ;
		}
	}


	if( type == SB_VERT )
	{		
		//lpret = lpws->lpvScrollBar;
		lpret = (_SCROLLDATA *)SetWindowLong( hWnd, GWL_VSCROLL, 0 );//(_SCROLLDATA *)GetWindowLong( hWnd, GWL_VSCROLL );
		//lpws->lpvScrollBar = NULL;
	}
	else if( type == SB_HORZ )
	{
		lpret = (_SCROLLDATA *)SetWindowLong( hWnd, GWL_HSCROLL, 0 );//(_SCROLLDATA *)GetWindowLong( hWnd, GWL_HSCROLL );
		
		//lpret = lpws->lphScrollBar;
		//lpws->lphScrollBar = NULL;
	}
	if( lpret )
	{
	    //BlockHeap_Free( hgwmeBlockHeap, 0, lpret, sizeof(_SCROLLDATA) );
		//DeinitScrollBarBrush( (LPSCRLBAR_BRUSH)lpret->lpvUserData );
		free( lpret );
	}

	//LeaveEnterCriticalSection( &csWindow );

	//return lpret;
}


// **************************************************
// 声明：void Scrl_GetBarRect( LPRECT lprect, DWORD dwMainStyle, DWORD dwExStyle, int which )
// 参数：
// 	IN/OUT lprect - 输入窗口矩形，输出滚动条矩形 
//	IN dwMainStyle - 窗口主风格
//	IN dwExStyle - 窗口扩展风格
//	IN which - 滚动条类型，如下值：
//				SB_VERT - 垂直滚动条
//				SB_HORZ - 水平滚动条
// 返回值：
//	无
// 功能描述：
//	得到滚动条矩形
// 引用: 
//	
// ************************************************

void Scrl_GetBarRect( LPRECT lprect, DWORD dwMainStyle, DWORD dwExStyle, int which )
{
    SIZE size;
	//得到边框厚度
    _GetFrameSize( &size, dwMainStyle );
	//客户边框 ？
	if( dwExStyle & WS_EX_CLIENTEDGE )
        InflateRect( lprect, -(size.cx+1), -(size.cy+1) );
	else
		InflateRect( lprect, -size.cx, -size.cy );
	
    if( which == SB_VERT )
    {   // 垂直 vertial scroll bar
        if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
			(dwExStyle & WS_EX_TITLE) )
            lprect->top += GetSystemMetrics( SM_CYCAPTION );
        if( dwMainStyle & WS_HSCROLL )
             lprect->bottom -= GetSystemMetrics( SM_CYHSCROLL );
        lprect->left = lprect->right - GetSystemMetrics( SM_CXVSCROLL );
    }
    else if( which == SB_HORZ )
    {    // 水平horitonal scroll bar
        lprect->top = lprect->bottom - GetSystemMetrics( SM_CYHSCROLL );
        if( dwMainStyle & WS_VSCROLL )
            lprect->right -= GetSystemMetrics( SM_CXVSCROLL );
    }
    else
    {    // 右底矩形 right bottom corner
        lprect->top = lprect->bottom - GetSystemMetrics( SM_CYHSCROLL );
        lprect->left = lprect->right - GetSystemMetrics( SM_CXVSCROLL );
    }
}

// **************************************************
// 声明：static BOOL Scrl_GetScrollData( HWND hWnd, int fnBar, _LPSCROLL lpsd )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型
//	OUT lpsd - 用于接受滚动条数据
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到滚动条数据
// 引用: 
//	
// ************************************************

static BOOL Scrl_GetScrollData( HWND hWnd, int fnBar, _LPSCROLL lpsd )
{
	
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//int type;
    //_LPWINDATA lpws = _GetHWNDPtr(hWnd);
	_LPSCROLLDATA p;
	p = _GetWindowScrollBar( hWnd, fnBar );
	
	if( p || fnBar == SB_CORNER )
	{	//得到私有数据
		//_LPSCROLLDATA p = (_LPSCROLLDATA)(lpws+1);
		if( fnBar == SB_CTL )
		{	//为滚动条窗口
			
			lpsd->sd = *p;
			GetClientRect( hWnd, &lpsd->rect );
			lpsd->fnBar = (dwStyle & SBS_VERT) ? SB_VERT : SB_HORZ;
		}
		else
		{	//窗口内部滚动条
			DWORD dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );

			GetWindowRect( hWnd, &lpsd->rect );
			OffsetRect( &lpsd->rect, -lpsd->rect.left, -lpsd->rect.top );
			//得到滚动条矩形
			Scrl_GetBarRect( &lpsd->rect, dwStyle, dwExStyle, fnBar );
			//	2004-09-06 , modify
			//p = _GetWindowScrollBar( lpws, fnBar );
			if( fnBar != SB_CORNER )
				lpsd->sd = *p;
			//if( fnBar == SB_VERT )
			//	lpsd->sd = lpws->vScrollBar;
			//else if( fnBar == SB_HORZ )
			//	lpsd->sd = lpws->hScrollBar;
			// 2004-09-06
			lpsd->fnBar = fnBar;
		}
		//ResetBrushOrg( lpsd->sd.lpvUserData, lpsd->rect.left, lpsd->rect.top );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：static int Scrl_GetDrawPos( _LPSCROLL lpsd, int pos[6] )
// 参数：
//	IN lpsd - 滚动条数据对象指针
//	OUT pos[6] - 用于接受滚动条显示位置数据
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到滚动条各部分显示位置数据
// 引用: 
//	
// ************************************************

static int Scrl_GetDrawPos( _LPSCROLL lpsd, int pos[6] )
{
	RECT rect = lpsd->rect;
	long l, nSize;
	int arrowSize = 0, pSize, offset;

    if(lpsd->fnBar == SB_VERT)
    {	//垂直
    	nSize =  rect.bottom-rect.top;
        offset = rect.top;
    }
    else if(lpsd->fnBar == SB_HORZ)
    {	//水平
        nSize = rect.right-rect.left;
        offset = rect.left;
    }
	else   // error
		return FALSE;

	if( lpsd->fnBar == SB_VERT )
	{	//得到宽度
		arrowSize = GetSystemMetrics( SM_CYVSCROLL );
	}
    else if( lpsd->fnBar == SB_HORZ )
	{	//得到高度
		arrowSize = GetSystemMetrics( SM_CXHSCROLL );
	}
	//得到上下/左右 块显示位置
	pos[0] = 0;
	pos[1] = arrowSize;

	pos[4] = (short)(nSize - arrowSize);
	pos[5] = nSize;

	l = nSize - arrowSize * 2;
	if( lpsd->sd.nMin < lpsd->sd.nMax )
	    pSize = (int)( lpsd->sd.nPage * l / (lpsd->sd.nMax-lpsd->sd.nMin+1) );
	else
		pSize = l;
	pSize = min( pSize, l );
	pSize = max( MIN_THUMBSIZE, pSize );
	//得到滑动块显示位置
    if( lpsd->sd.nPos + lpsd->sd.nPage >= lpsd->sd.nMax + 1  )
	{	//滑动块超出下位置
		pos[3] = (short)(nSize - arrowSize);
		pos[2] = (short)(pos[3] - pSize);
	}
	else if( lpsd->sd.nPos <= lpsd->sd.nMin )
	{	//滑动块超出上位置
		pos[2] = pos[1];
		pos[3] = pos[2] + pSize;
	}
	else
	{	//最大/最小值有效吗 ？
		if( lpsd->sd.nMin < lpsd->sd.nMax )
		{	//有效
	        pos[2] = (short)(arrowSize + (lpsd->sd.nPos - lpsd->sd.nMin) * l / (lpsd->sd.nMax-lpsd->sd.nMin+1));
			pos[3] = pos[2] + pSize;
			if( pos[3] > pos[4] )
			{
				pos[3] = pos[4];
				pos[2] = pos[3] - pSize;
			}
			if( pos[2] < pos[1] )
				pos[2] = pos[1];
		}
		else
		{
			pos[2] = pos[1];
	        pos[3] = pos[2] + pSize;//
		}
	}

    for( l = 0; l < 6; l++ )
        pos[(int)l] += offset;
	return TRUE;
}

// **************************************************
// 声明：static BOOL Scrl_GetThumbRect( _LPSCROLL lpsd, LPRECT lprect )
// 参数：
//	IN lpsd - 滚动条数据对象指针
//	OUT lprect - 用于接受滑动块矩形
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到滑动块矩形
// 引用: 
//	
// ************************************************

static BOOL Scrl_GetThumbRect( _LPSCROLL lpsd, LPRECT lprect )
{
	int drawPos[6];

    *lprect = lpsd->rect;
    Scrl_GetDrawPos( lpsd, drawPos );

	if( lpsd->fnBar == SB_VERT )
	{	//垂直
		lprect->top = drawPos[2];
		lprect->bottom = drawPos[3];
	}
	else if( lpsd->fnBar == SB_HORZ )
	{	//水平
		lprect->left = drawPos[2];
		lprect->right = drawPos[3];
	}
	return TRUE;
}

// **************************************************
// 声明：static int Scrl_DrawThumbBox( HDC hdc, int fnBar, UINT uiState, LPCRECT lprcOld, LPCRECT lprcNew )
// 参数：
// 	IN hdc - 显示DC
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//						
//	IN uiState - 状态
//	IN lprcOld - 之前的显示矩形
//	IN lprcNew - 新的显示矩形
// 返回值：
//	返回0
// 功能描述：
//	画滚动条的滑块
// 引用: 
//	
// ************************************************
/* orgin
static int Scrl_DrawThumbBox( HDC hdc, int fnBar, UINT uiState, LPCRECT lprcOld, LPCRECT lprcNew )
{
    HBRUSH hBrush;
	RECT rect;

	if( (uiState & ESB_DISABLE_BOTH) == ESB_DISABLE_BOTH )
	{	//无效状态
		//联合矩形
		if( lprcOld && lprcNew )
			UnionRect( &rect, lprcOld, lprcNew );
		else if( lprcOld )
			rect = *lprcOld;
		else
			rect = *lprcNew;
		//
		FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
		return 0;
	}
	//清除背景
    if( lprcOld && lprcNew )
    {
		if( SubtractRect( &rect, lprcOld, lprcNew ) )
		{	// 清除老矩形 clear old rect

			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
		}
		else
		{   // 清除老矩形 clear old rect
			rect = *lprcOld;

			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
		}
    }

    if( fnBar == SB_VERT )
	{	//垂直
		if( (lprcNew->bottom - lprcNew->top) < MIN_THUMBSIZE )
		{
			rect = *lprcNew;
			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
			return 0;
		}
	}
	else
	{   // 水平 SB_HORZ
		if( (lprcNew->right - lprcNew->left) < MIN_THUMBSIZE )
		{
			rect = *lprcNew;
			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
			return 0;
		}
	}
    {
		int iOldMode;
		int xyMid;
		HFONT hFont;
		BYTE bThumb;
        RECT rect = *lprcNew;
		//画滑块
		hBrush = GetSysColorBrush( COLOR_SCROLLBARTHUMB );//COLOR_3DFACE );
        hBrush = SelectObject( hdc, hBrush );

        Rectangle( hdc, lprcNew->left, lprcNew->top, lprcNew->right, lprcNew->bottom );

        hBrush = SelectObject( hdc, hBrush );

		// 选择符号字体 draw text 
		hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );

	    iOldMode = SetBkMode( hdc, TRANSPARENT );
		
		if( fnBar == SB_VERT )
		{	//垂直
			xyMid = (lprcNew->bottom + lprcNew->top) / 2;
			rect.top = xyMid - 8;
			rect.bottom = xyMid + 8;
			bThumb = SYM_SCROLL_VTHUMB;
		}
		else
		{	//水平
			xyMid = (lprcNew->left + lprcNew->right) / 2;
			rect.left = xyMid - 8;
			rect.right = xyMid + 8;
			bThumb = SYM_SCROLL_HTHUMB;
		}

		SetTextColor( hdc, CL_WHITE );

	    DrawText( hdc, (LPCTSTR)&bThumb, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

		SetTextColor( hdc, CL_DARKGRAY );
		if( fnBar == SB_VERT )
		{
			rect.top++;
			rect.bottom++;
		}
		else
		{
			rect.left++;
			rect.right++;
		}
		DrawText( hdc, (LPCTSTR)&bThumb, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

	    SetBkMode( hdc, iOldMode );

		SelectObject( hdc, hFont );
    }
    return 0;
}
*/
/*
static HBRUSH hbrushVSBackground = NULL;
static HBRUSH hbrushVSThumb= NULL;
static HBRUSH hbrushHSBackground = NULL;
static HBRUSH hbrushHSThumb= NULL;
static HBRUSH hbrushCorner= NULL;
static HBRUSH hbrushUpArrow= NULL;
static HBRUSH hbrushDownArrow= NULL;
static HBRUSH hbrushLeftArrow= NULL;
static HBRUSH hbrushRightArrow= NULL;
*/
static int Scrl_DrawThumbBox( HDC hdc, _LPSCROLL lpscrl, LPCRECT lprcOld, LPCRECT lprcNew )
{
//    HBRUSH hBrush;
	RECT rect;	
	_LPSCROLLDATA lpsd = &lpscrl->sd;
	UINT fnBar = lpscrl->fnBar;
	UINT uiState = lpsd->state;
	if( (uiState & ESB_DISABLE_BOTH) == ESB_DISABLE_BOTH )
	{	//无效状态
		//联合矩形
		if( lprcOld && lprcNew )
			UnionRect( &rect, lprcOld, lprcNew );
		else if( lprcOld )
			rect = *lprcOld;
		else
			rect = *lprcNew;
		//
		if( fnBar == SB_VERT )
		    FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushVSBackground );
		else
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushHSBackground );

		return 0;
	}
	//清除背景
    if( lprcOld && lprcNew )
    {
		if( SubtractRect( &rect, lprcOld, lprcNew ) )
		{	// 清除老矩形 clear old rect

			//FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
			if( fnBar == SB_VERT )
				FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushVSBackground );
			else
				FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushHSBackground );

		}
		else
		{   // 清除老矩形 clear old rect
			rect = *lprcOld;

			//FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
			if( fnBar == SB_VERT )
				FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushVSBackground );
			else
				FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushHSBackground );

		}
    }

    if( fnBar == SB_VERT )
	{	//垂直
		if( (lprcNew->bottom - lprcNew->top) < MIN_THUMBSIZE )
		{
			rect = *lprcNew;
			//FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushVSBackground );

			return 0;
		}
	}
	else
	{   // 水平 SB_HORZ
		if( (lprcNew->right - lprcNew->left) < MIN_THUMBSIZE )
		{
			rect = *lprcNew;
			//FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushHSBackground );

			return 0;
		}
	}
    {
		rect = *lprcNew;
		if( fnBar == SB_VERT )
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushVSThumb );
		else
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)(lpsd->lpvUserData))->hbrushHSThumb );
		/*
		int iOldMode;
		int xyMid;
		HFONT hFont;
		BYTE bThumb;
        RECT rect = *lprcNew;
		//画滑块
		hBrush = GetSysColorBrush( COLOR_SCROLLBARTHUMB );//COLOR_3DFACE );
        hBrush = SelectObject( hdc, hBrush );

        Rectangle( hdc, lprcNew->left, lprcNew->top, lprcNew->right, lprcNew->bottom );

        hBrush = SelectObject( hdc, hBrush );

		// 选择符号字体 draw text 
		hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );

	    iOldMode = SetBkMode( hdc, TRANSPARENT );
		
		if( fnBar == SB_VERT )
		{	//垂直
			xyMid = (lprcNew->bottom + lprcNew->top) / 2;
			rect.top = xyMid - 8;
			rect.bottom = xyMid + 8;
			bThumb = SYM_SCROLL_VTHUMB;
		}
		else
		{	//水平
			xyMid = (lprcNew->left + lprcNew->right) / 2;
			rect.left = xyMid - 8;
			rect.right = xyMid + 8;
			bThumb = SYM_SCROLL_HTHUMB;
		}

		SetTextColor( hdc, CL_WHITE );

	    DrawText( hdc, (LPCTSTR)&bThumb, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

		SetTextColor( hdc, CL_DARKGRAY );
		if( fnBar == SB_VERT )
		{
			rect.top++;
			rect.bottom++;
		}
		else
		{
			rect.left++;
			rect.right++;
		}
		DrawText( hdc, (LPCTSTR)&bThumb, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

	    SetBkMode( hdc, iOldMode );

		SelectObject( hdc, hFont );
		*/
    }
    return 0;
}

// **************************************************
// 声明：static int Scrl_DrawScrollBar( _LPSCROLL lpsd, HDC hdc )
// 参数：
//	IN lpsd - 滚动条数据对象指针
// 	IN hdc - 显示DC
// 返回值：
//	返回0
// 功能描述：
//	画滚动条
// 引用: 
//	
// ************************************************

/*  orgin
static int Scrl_DrawScrollBar( _LPSCROLL lpsd, HDC hdc )
{
	RECT rect;
	int drawPos[6];
    HFONT hFont;
    int iOldMode;
	BYTE bArrow;

	if( lpsd->fnBar == SB_CORNER )
	{	//右下角
		FillRect( hdc, &lpsd->rect, GetSysColorBrush(COLOR_3DFACE) );
	}
	else
	{	//显示矩形
		rect = lpsd->rect;
		// 选择符号字体
		hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );//
		//得到显示位置
		Scrl_GetDrawPos( lpsd, drawPos );
	
		if( lpsd->fnBar == SB_VERT )
		{	//垂直
			rect.top = drawPos[1];
			rect.bottom = drawPos[4];

#ifdef ZT_PHONE
			{
				HBRUSH hBrush = GetSysColorBrush(COLOR_SCROLLBAR);
				hBrush = SelectObject( hdc, hBrush );
				Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
				SelectObject( hdc, hBrush );
			}
#else
			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
#endif

			// 画上箭头 draw up arrow
			rect.top = drawPos[0];   
			rect.bottom = drawPos[1];
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );		
			iOldMode = SetBkMode( hdc, TRANSPARENT );			
			if( lpsd->sd.state & ESB_DISABLE_UP )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );
			bArrow = SYM_UPARROW;
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

			// 画下箭头 draw down arrow
			rect.top = drawPos[4];
			rect.bottom = drawPos[5];
			SetBkMode( hdc, OPAQUE );
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );			
			if( lpsd->sd.state & ESB_DISABLE_DOWN )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );			
			bArrow = SYM_DOWNARROW;
			SetBkMode( hdc, TRANSPARENT );
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

			// draw thumb
			rect.top = drawPos[2];
			rect.bottom = drawPos[3];
			SetBkMode( hdc, iOldMode );
			Scrl_DrawThumbBox( hdc, SB_VERT, lpsd->sd.state, 0, &rect );
		}
		else if( lpsd->fnBar == SB_HORZ )
		{	//水平
			rect.left = drawPos[1];
			rect.right = drawPos[4];
#ifdef ZT_PHONE
			{
				HBRUSH hBrush = GetSysColorBrush(COLOR_SCROLLBAR);
				hBrush = SelectObject( hdc, hBrush );
				Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
				SelectObject( hdc, hBrush );
			}
#else
			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
#endif
			
			rect.left = drawPos[0];
			rect.right = drawPos[1];
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
			
			iOldMode = SetBkMode( hdc, TRANSPARENT );
			//画左箭头
			if( lpsd->sd.state & ESB_DISABLE_LEFT )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );
			bArrow = SYM_LEFTARROW;
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			rect.left = drawPos[4];
			rect.right = drawPos[5];
			SetBkMode( hdc, OPAQUE );
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
			
			if( lpsd->sd.state & ESB_DISABLE_RIGHT )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );
			//画右箭头			
			bArrow = SYM_RIGHTARROW;
			SetBkMode( hdc, TRANSPARENT );
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			SetBkMode( hdc, iOldMode );
			
			rect.left = drawPos[2];
			rect.right = drawPos[3];
			Scrl_DrawThumbBox( hdc, SB_HORZ, lpsd->sd.state, 0, &rect );
		}
		SelectObject( hdc, hFont );
	}
	return 0;
}
*/

static int Scrl_DrawScrollBar( _LPSCROLL lpsd, HDC hdc )
{
	RECT rect;
	int drawPos[6];
//    HFONT hFont;
//    int iOldMode;
//	BYTE bArrow;

	/*
static HBRUSH hbrushVSBackground = NULL;
static HBRUSH hbrushVSThumb= NULL;
static HBRUSH hbrushHSBackground = NULL;
static HBRUSH hbrushHSThumb= NULL;
static HBRUSH hbrushCorner= NULL;
static HBRUSH hbrushUpArrow= NULL;
static HBRUSH hbrushDownArrow= NULL;
static HBRUSH hbrushLeftArrow= NULL;
static HBRUSH hbrushRightArrow= NULL;
	*/
	if( lpsd->fnBar == SB_CORNER )
	{	//右下角
		FillScrollRect( hdc, &lpsd->rect, GetSysColorBrush(COLOR_3DFACE) );//((LPSCRLBAR_BRUSH)lpsd->sd.lpvUserData)->hbrushCorner );
	}
	else
	{	//显示矩形
		rect = lpsd->rect;
		// 选择符号字体
		//hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );//
		//得到显示位置
		Scrl_GetDrawPos( lpsd, drawPos );
	
		if( lpsd->fnBar == SB_VERT )
		{	//垂直
			rect.top = drawPos[1];
			rect.bottom = drawPos[4];

			
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)lpsd->sd.lpvUserData)->hbrushVSBackground );//GetSysColorBrush(COLOR_SCROLLBAR) );

			// 画上箭头 draw up arrow
			
			rect.top = drawPos[0];   
			rect.bottom = drawPos[1];
			/*
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );		
			iOldMode = SetBkMode( hdc, TRANSPARENT );			
			if( lpsd->sd.state & ESB_DISABLE_UP )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );
			bArrow = SYM_UPARROW;
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			*/
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)lpsd->sd.lpvUserData)->hbrushUpArrow );

			// 画下箭头 draw down arrow
			rect.top = drawPos[4];
			rect.bottom = drawPos[5];
			/*
			SetBkMode( hdc, OPAQUE );
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );			
			if( lpsd->sd.state & ESB_DISABLE_DOWN )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );			
			bArrow = SYM_DOWNARROW;
			SetBkMode( hdc, TRANSPARENT );
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			*/
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)lpsd->sd.lpvUserData)->hbrushDownArrow );

			// draw thumb
			rect.top = drawPos[2];
			rect.bottom = drawPos[3];
			//SetBkMode( hdc, iOldMode );
			//Scrl_DrawThumbBox( hdc, SB_VERT, lpsd->sd.state, 0, &rect );
			Scrl_DrawThumbBox( hdc, lpsd, 0, &rect );
		}
		else if( lpsd->fnBar == SB_HORZ )
		{	//水平
			rect.left = drawPos[1];
			rect.right = drawPos[4];
/*
#ifdef ZT_PHONE
			{
				HBRUSH hBrush = GetSysColorBrush(COLOR_SCROLLBAR);
				hBrush = SelectObject( hdc, hBrush );
				Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
				SelectObject( hdc, hBrush );
			}
#else
			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
#endif
			*/
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)lpsd->sd.lpvUserData)->hbrushHSBackground );
			
			rect.left = drawPos[0];
			rect.right = drawPos[1];

			//Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)lpsd->sd.lpvUserData)->hbrushLeftArrow );
			
/*
			iOldMode = SetBkMode( hdc, TRANSPARENT );
			//画左箭头
			if( lpsd->sd.state & ESB_DISABLE_LEFT )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );
			bArrow = SYM_LEFTARROW;
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			rect.left = drawPos[4];
			rect.right = drawPos[5];
			SetBkMode( hdc, OPAQUE );
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
			
			if( lpsd->sd.state & ESB_DISABLE_RIGHT )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );
			//画右箭头			
			bArrow = SYM_RIGHTARROW;
			SetBkMode( hdc, TRANSPARENT );
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			SetBkMode( hdc, iOldMode );
			*/
			rect.left = drawPos[4];
			rect.right = drawPos[5];

			FillScrollRect( hdc, &rect, ((LPSCRLBAR_BRUSH)lpsd->sd.lpvUserData)->hbrushLeftArrow );
			//Scrl_DrawThumbBox( hdc, SB_HORZ, lpsd->sd.state, 0, &rect );

			rect.left = drawPos[2];
			rect.right = drawPos[3];

			Scrl_DrawThumbBox( hdc, lpsd, 0, &rect );
		}
		//SelectObject( hdc, hFont );
	}
	return 0;
}


// **************************************************
// 声明：static int Scrl_DrawScrollBar( _LPSCROLL lpsd, HDC hdc )
// 参数：
//	IN lpsd - 滚动条数据对象指针
// 	IN hdc - 显示DC
// 返回值：
//	返回0
// 功能描述：
//	画滚动条
// 引用: 
//	
// ************************************************
/*
static int Scrl_DrawScrollBar( _LPSCROLL lpsd, HDC hdc )
{
	RECT rect;
	int drawPos[6];
    HFONT hFont;
    int iOldMode;
	BYTE bArrow;
	static HANDLE hdcHScrollBar = NULL;
	static HANDLE hdcVScrollBar;
	static BITMAP bmpHInfo, bmpVInfo;

	if( hdcHScrollBar == NULL )
	{ //第一次，初始化...
		HANDLE hBitmap = LoadImage( NULL, MAKEINTRESOURCE( OBM_HSCROLLBAR_BACKGROUND ), IMAGE_BITMAP, 0, 0, 0 );

		if( hBitmap )
		{
			hdcHScrollBar =  CreateCompatibleDC( hdc );
			SelectObject( hdcHScrollBar, hBitmap );
			GetObject( hBitmap, sizeof(bmpHInfo), &bmpHInfo );
		}
	}

	if( hdcVScrollBar == NULL )
	{ //第一次，初始化...
		HANDLE hBitmap = LoadImage( NULL, MAKEINTRESOURCE( OBM_VSCROLLBAR_BACKGROUND ), IMAGE_BITMAP, 0, 0, 0 );

		if( hBitmap )
		{
			hdcVScrollBar =  CreateCompatibleDC( hdc );
			SelectObject( hdcVScrollBar, hBitmap );
			GetObject( hBitmap, sizeof(bmpVInfo), &bmpVInfo );
		}
	}


	if( lpsd->fnBar == SB_CORNER )
	{	//右下角
		FillRect( hdc, &lpsd->rect, GetSysColorBrush(COLOR_3DFACE) );
	}
	else
	{	//显示矩形
		int len;

		rect = lpsd->rect;
		// 选择符号字体
		//hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );//
		//得到显示位置
		Scrl_GetDrawPos( lpsd, drawPos );
	
		if( lpsd->fnBar == SB_VERT )
		{	//垂直
			int cyImage, cyClient;

			rect.top = drawPos[0];
			rect.bottom = drawPos[5];

			cyImage = bmpVInfo.bmHeight;
			cyClient = rect.bottom - rect.top;

			if( cyImage == cyClient )
			{
				BitBlt( hdc, rect.left, rect.top, bmpVInfo.bmWidth, bmpVInfo.bmHeight, hdcVScrollBar, 0, 0, SRCCOPY );
			}
			else if( cyImage < cyClient )
			{
				int yStart = rect.top;
				int cymidSize;
				cyImage --;  //去掉尾的一行
				//cyImage = MIN( cyImage, cyClient );
				//画前段
				BitBlt( hdc, rect.left, yStart, bmpVInfo.bmWidth, cyImage, hdcVScrollBar, 0, 0, SRCCOPY );
				yStart += cyImage;
				cyClient -= cyImage;
				//cyClient -= cyImage;
				//cyImage--;
				//画中间
				cymidSize = cyImage - 1;
				for( ; yStart + cyImage < rect.bottom; yStart += cymidSize )
				{
					BitBlt( hdc, rect.left, yStart, bmpVInfo.bmWidth, cymidSize, hdcVScrollBar, 0, 1, SRCCOPY );
					cyClient -= cymidSize;
				}
				//画末段
				BitBlt( hdc, rect.left, yStart, bmpVInfo.bmWidth, cyClient, hdcVScrollBar, 0, bmpVInfo.bmHeight - cyClient, SRCCOPY );

			}
			else
			{
				BitBlt( hdc, rect.left, rect.top, bmpVInfo.bmWidth, cyClient - 1, hdcVScrollBar, 0, 0, SRCCOPY );
				BitBlt( hdc, rect.left, rect.top + cyClient - 1, bmpVInfo.bmWidth, 1, hdcVScrollBar, 0, cyImage - 1, SRCCOPY );
			}
			
			Scrl_DrawThumbBox( hdc, SB_VERT, lpsd->sd.state, 0, &rect );
		}
		else if( lpsd->fnBar == SB_HORZ )
		{	//水平
			rect.left = drawPos[1];
			rect.right = drawPos[4];

#ifdef ZT_PHONE
			{
				HBRUSH hBrush = GetSysColorBrush(COLOR_SCROLLBAR);
				hBrush = SelectObject( hdc, hBrush );
				Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
				SelectObject( hdc, hBrush );
			}
#else
			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
#endif
			
			rect.left = drawPos[0];
			rect.right = drawPos[1];
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
			
			iOldMode = SetBkMode( hdc, TRANSPARENT );
			//画左箭头
			if( lpsd->sd.state & ESB_DISABLE_LEFT )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );
			bArrow = SYM_LEFTARROW;
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			rect.left = drawPos[4];
			rect.right = drawPos[5];
			SetBkMode( hdc, OPAQUE );
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
			
			if( lpsd->sd.state & ESB_DISABLE_RIGHT )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );
			//画右箭头			
			bArrow = SYM_RIGHTARROW;
			SetBkMode( hdc, TRANSPARENT );
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			SetBkMode( hdc, iOldMode );
			
			rect.left = drawPos[2];
			rect.right = drawPos[3];
			Scrl_DrawThumbBox( hdc, SB_HORZ, lpsd->sd.state, 0, &rect );
		}
//		SelectObject( hdc, hFont );
	}
	return 0;
}
*/

// **************************************************
// 声明：static int Scrl_DrawScrollBar( _LPSCROLL lpsd, HDC hdc )
// 参数：
//	IN lpsd - 滚动条数据对象指针
// 	IN hdc - 显示DC
// 返回值：
//	返回0
// 功能描述：
//	画滚动条
// 引用: 
//	
// ************************************************

//static BITMAP bmpHInfo, bmpVInfo;
/*
static int Scrl_DrawScrollBar( _LPSCROLL lpsd, HDC hdc )
{
	RECT rect;
	int drawPos[6];
    HFONT hFont;
    int iOldMode;
	BYTE bArrow;

	if( lpsd->fnBar == SB_CORNER )
	{	//右下角
		FillRect( hdc, &lpsd->rect, GetSysColorBrush(COLOR_3DFACE) );
	}
	else
	{	//显示矩形
		rect = lpsd->rect;
		// 选择符号字体
		hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );//
		//得到显示位置
		Scrl_GetDrawPos( lpsd, drawPos );
	
		if( lpsd->fnBar == SB_VERT )
		{	//垂直
			rect.top = drawPos[1];
			rect.bottom = drawPos[4];

#ifdef ZT_PHONE
			{
				HBRUSH hBrush = GetSysColorBrush(COLOR_SCROLLBAR);
				hBrush = SelectObject( hdc, hBrush );
				Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
				SelectObject( hdc, hBrush );
			}
#else
			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
#endif

			// 画上箭头 draw up arrow
			rect.top = drawPos[0];   
			rect.bottom = drawPos[1];
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );		
			iOldMode = SetBkMode( hdc, TRANSPARENT );			
			if( lpsd->sd.state & ESB_DISABLE_UP )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );
			bArrow = SYM_UPARROW;
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

			// 画下箭头 draw down arrow
			rect.top = drawPos[4];
			rect.bottom = drawPos[5];
			SetBkMode( hdc, OPAQUE );
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );			
			if( lpsd->sd.state & ESB_DISABLE_DOWN )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );			
			bArrow = SYM_DOWNARROW;
			SetBkMode( hdc, TRANSPARENT );
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

			// draw thumb
			rect.top = drawPos[2];
			rect.bottom = drawPos[3];
			SetBkMode( hdc, iOldMode );
			Scrl_DrawThumbBox( hdc, SB_VERT, lpsd->sd.state, 0, &rect );
		}
		else if( lpsd->fnBar == SB_HORZ )
		{	//水平
			rect.left = drawPos[1];
			rect.right = drawPos[4];
#ifdef ZT_PHONE
			{
				HBRUSH hBrush = GetSysColorBrush(COLOR_SCROLLBAR);
				hBrush = SelectObject( hdc, hBrush );
				Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
				SelectObject( hdc, hBrush );
			}
#else
			FillRect( hdc, &rect, GetSysColorBrush(COLOR_SCROLLBAR) );
#endif
			
			rect.left = drawPos[0];
			rect.right = drawPos[1];
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
			
			iOldMode = SetBkMode( hdc, TRANSPARENT );
			//画左箭头
			if( lpsd->sd.state & ESB_DISABLE_LEFT )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );
			bArrow = SYM_LEFTARROW;
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			rect.left = drawPos[4];
			rect.right = drawPos[5];
			SetBkMode( hdc, OPAQUE );
			Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
			
			if( lpsd->sd.state & ESB_DISABLE_RIGHT )
				SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_LIGHTGRAY );
			else
				SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );//CL_BLACK );
			//画右箭头			
			bArrow = SYM_RIGHTARROW;
			SetBkMode( hdc, TRANSPARENT );
			DrawText( hdc, (LPCTSTR)&bArrow, 1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
			SetBkMode( hdc, iOldMode );
			
			rect.left = drawPos[2];
			rect.right = drawPos[3];
			Scrl_DrawThumbBox( hdc, SB_HORZ, lpsd->sd.state, 0, &rect );
		}
		SelectObject( hdc, hFont );
	}
	return 0;
}
*/



// **************************************************
// 声明：int Scrl_RedrawScrollBar( HWND hWnd, HDC hdc, int fnBar )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN hdc - 显示DC句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//						
// 返回值：
//	返回0
// 功能描述：
//	画滚动条
// 引用: 
//	
// ************************************************

int Scrl_RedrawScrollBar( HWND hWnd, HDC hdc, int fnBar )
{
    _SCROLL sd;
	//得到滚动条数据
	Scrl_GetScrollData( hWnd, fnBar, &sd );
	//画
	Scrl_DrawScrollBar( &sd, hdc );

	return 0;
}

// **************************************************
// 声明：static void Scrl_TranslatePos( HWND hWnd, int fnBar, short * lpx, short * lpy )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	IN/OUT lpx - 输入客户x坐标，输出窗口x坐标
//	IN/OUT lpy - 输入客户y坐标，输出窗口y坐标
// 返回值：
//	返回0
// 功能描述：
//	将客户坐标转成窗口坐标
// 引用: 
//	
// ************************************************

static void Scrl_TranslatePos( HWND hWnd, int fnBar, short * lpx, short * lpy )
{
    POINT pt;
    RECT rect;
    if( fnBar != SB_CTL )
    {
        if( lpx )
            pt.x = *lpx;
        if( lpy )
            pt.y = *lpy;
		//先转化为屏幕坐标
        ClientToScreen( hWnd, &pt );
		//得到窗口的屏幕矩形
        GetWindowRect( hWnd, &rect );
		//转化
        if( lpx )
            *lpx = (short)(pt.x - rect.left);
        if( lpy )
            *lpy = (short)(pt.y - rect.top);
    }
}

// **************************************************
// 声明：LRESULT Scrl_DoLBUTTONDOWN( HWND hWnd, int fnBar, int x, int y )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	IN x - 当前鼠标x位置
//	IN y - 当前鼠标y位置
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_LBUTTONDOWN 消息
// 引用: 
//	
// ************************************************
#define SLEEP_TICK 200
LRESULT Scrl_DoLBUTTONDOWN( HWND hWnd, int fnBar, int x, int y )
{
	_SCROLL si;
	int posv[6], pos, oldPos, i;
	short nx, ny;
	HDC hdc;
    HBRUSH hBrush;
	RECT rect, oldRect;
	MSG msg;
	UINT dwMsg;
	BOOL edgeMsg = FALSE;
    WORD ncode;
	BOOL bHasTimer = FALSE;	
	DWORD dwTickCount;
	//得到滚动条数据
	Scrl_GetScrollData( hWnd, fnBar, &si );
    rect = si.rect;
	//得到显示位置
	Scrl_GetDrawPos( &si, posv );
	pos = (si.fnBar==SB_VERT) ? y : x;
	//得到鼠标在滚动条上的位置信息（i）
	for( i = 0; i < 5; i++ )
		if( pos >= posv[i] && pos < posv[i+1] )
			break;
	
	if( si.fnBar == SB_VERT )
	{	//垂直
		rect.top = posv[i];
		rect.bottom = posv[i+1];
		dwMsg = WM_VSCROLL;
    }
    else
    {	//水平
		rect.left = posv[i];
		rect.right = posv[i+1];
		dwMsg = WM_HSCROLL;
    }
	if( i == 0 || i == 4 )
    {	//在上/左 下/右 箭头区域
        ncode = (i == 0) ? SB_LINEUP : SB_LINEDOWN;
    }
	else if( i == 1 || i == 3 )
    {	//在上页  下页区域
        ncode = (i == 1 ) ? SB_PAGEUP : SB_PAGEDOWN;
    }
	else if( i == 2 )
    {	//在滑快
        ncode = SB_THUMBTRACK;
    }
    if( (si.sd.state & ESB_DISABLE_LTUP) && 
        (ncode == SB_LINEUP || ncode == SB_PAGEUP || ncode == SB_THUMBTRACK ) )
    {	//无效上
        return 0;
    }
    else if ( (si.sd.state & ESB_DISABLE_RTDN) && 
              (ncode == SB_LINEDOWN || ncode == SB_PAGEDOWN || ncode == SB_THUMBTRACK ) )
    {	//无效下
        return 0;
    }

	if( i == 2 )
    {	//设置跟踪状态
        Scrl_SetScrollState( hWnd, fnBar, SBS_TRACKTHUMB, TRUE );
    }

	oldRect = rect;
	oldPos = si.sd.nPos;
	//抓住鼠标
	SetCapture( hWnd );

    if( ncode != SB_THUMBTRACK )
        Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( ncode, 0 ), (LPARAM)hWnd );
	//得到显示DC
    hdc = Scrl_GetScrollDC( hWnd, fnBar );
	hBrush = (HBRUSH)SelectObject( hdc, GetStockObject( LTGRAY_BRUSH ) );

#ifdef EML_DOS
    _ShowMouse();
#endif
	if( ncode != SB_THUMBTRACK )
	{
		bHasTimer = TRUE;	    
	}
	dwTickCount = GetTickCount();
	//进入模式处理状态
	// do modal loop
	while( GetCapture() == hWnd )
	{
		BOOL bHandleTimer = FALSE;
		
		if( bHasTimer )
			SetSysTimer( hWnd, IDSCROLLBAR, SLEEP_TICK, NULL );
		//得到消息
        if( GetMessage( &msg, 0, 0, 0 ) )
		{	//处理消息
			if( bHasTimer )
			    KillSysTimer( hWnd, IDSCROLLBAR );
			if( msg.message == WM_LBUTTONUP )
			{
#ifdef EML_DOS
    _HideMouse();
#endif
                if( i == 1 || i == 3 )
                {
//                    InvertRect( hdc, &rect );
                }

				if( i == 0 || i == 4 )
                {
//				    DrawEdge( hdc, &rect, BDR_RAISEDOUTER, BF_RECT );
                }
#ifdef EML_DOS
    _ShowMouse();
#endif
				if( i == 2 )
				{	// clear background
//#ifdef EML_DOS
//    _HideMouse();
//#endif
//                    Scrl_DrawThumbBox( hdc, fnBar, &oldRect, FALSE );
//#ifdef EML_DOS
//    _ShowMouse();
//#endif
					//画滑快
                    Scrl_SetScrollState( hWnd, fnBar, SBS_TRACKTHUMB, FALSE );
					Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_THUMBPOSITION, oldPos ), (LPARAM)hWnd );
				}
                Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, (WPARAM)MAKELONG( SB_ENDSCROLL, 0 ), (LPARAM)hWnd );
				if( i == 2 )
				{
                    // redraw thumb box
                    Scrl_GetScrollData( hWnd, fnBar, &si );
	                Scrl_GetDrawPos( &si, posv );
                	if( si.fnBar == SB_VERT )
	                {
		                rect.top = posv[i];
		                rect.bottom = posv[i+1];
					}
					else if( si.fnBar == SB_HORZ )
					{
		                rect.left = posv[i];
		                rect.right = posv[i+1];
					}
#ifdef EML_DOS
    _HideMouse();
#endif
                    //Scrl_DrawThumbBox( hdc, si.fnBar, si.sd.state, &oldRect, &rect );  //2000.12.08
					Scrl_DrawThumbBox( hdc, &si, &oldRect, &rect );  //2000.12.08
#ifdef EML_DOS
    _ShowMouse();
#endif
				}
				break;
			}
			else if( msg.message == WM_MOUSEMOVE )
			{
				if( i == 2 )
				{	// thumb box
					if( si.fnBar == SB_VERT )
					{
                        ny = HIWORD( msg.lParam );
                        Scrl_TranslatePos( hWnd, fnBar, 0, &ny );

						ny = ny - y;
						// draw thumb box
					    rect.top = posv[2] + ny;
						rect.bottom = posv[3] + ny;

						if( rect.bottom <= posv[4] && rect.top >= posv[1] )
						{
#ifdef EML_DOS
    _HideMouse();
#endif
						    // clear background
							// draw thumb box
                           //Scrl_DrawThumbBox( hdc, fnBar, si.sd.state, &oldRect, &rect );
							Scrl_DrawThumbBox( hdc, &si, &oldRect, &rect );
#ifdef EML_DOS
    _ShowMouse();
#endif
							oldRect = rect;
							{  // LN, 2003-07-12  error: ( posv[4] - posv[1] - (posv[3]-posv[2]) ) == 0
								int diff = ( posv[4] - posv[1] - (posv[3]-posv[2]) );
								if( diff )
							        pos = si.sd.nMin + (rect.top-posv[1]) * (si.sd.nMax-si.sd.nMin-si.sd.nPage+1) / diff;
								else
								{  // diff = 0
									;// nothing to do 
								}
							}
							if( pos != oldPos )
							{
							    Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_THUMBTRACK, pos ), (LPARAM)hWnd );
							    oldPos = pos;
							}
							edgeMsg = FALSE;
						}
						else if( edgeMsg == FALSE )
						{
							if( rect.bottom > posv[4] )
							{
								pos = si.sd.nMax - si.sd.nPage + 1;
							    Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_THUMBTRACK, pos ), (LPARAM)hWnd );
								Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_BOTTOM, 0 ), (LPARAM)hWnd );
								oldPos = pos;
							}
							else
							{
							    pos = si.sd.nMin;
								Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_THUMBTRACK, si.sd.nMin ), (LPARAM)hWnd );
								Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_TOP, 0 ), (LPARAM)hWnd );
								oldPos = pos;
							}
							edgeMsg = TRUE;
						}
					}
					else if( si.fnBar == SB_HORZ )
					{
                        nx = LOWORD( msg.lParam );
                        Scrl_TranslatePos( hWnd, fnBar, &nx, 0 );

						nx = nx - x;

					    rect.left = posv[2] + nx;
						rect.right = posv[3] + nx;
						if( rect.right <= posv[4] && rect.left >= posv[1] )
						{
#ifdef EML_DOS
    _HideMouse();
#endif
							// clear background//                          
							// draw thumb box
                            //Scrl_DrawThumbBox( hdc, fnBar, si.sd.state, &oldRect, &rect );
							Scrl_DrawThumbBox( hdc,  &si, &oldRect, &rect );
#ifdef EML_DOS
    _ShowMouse();
#endif
							oldRect = rect;
							{  // LN, 2003-07-12  error: ( posv[4] - posv[1] - (posv[3]-posv[2]) ) == 0
							   //pos = si.sd.nMin + (rect.left-posv[1]) * (si.sd.nMax-si.sd.nMin-si.sd.nPage+1) / ( posv[4] - posv[1] - (posv[3]-posv[2]) );
								int diff = ( posv[4] - posv[1] - (posv[3]-posv[2]) );
								if( diff )
								{
									pos = si.sd.nMin + (rect.left-posv[1]) * (si.sd.nMax-si.sd.nMin-si.sd.nPage+1) / diff;
								}
								else
								{
									; // nothing to do
								}
							}

							if( pos != oldPos )
							{
							    Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_THUMBTRACK, pos ), (LPARAM)hWnd );
							    oldPos = pos;
							}
							edgeMsg = FALSE;
						}
						else if( edgeMsg == FALSE )
						{
							if( rect.right > posv[4] )
							{
								pos = si.sd.nMax - si.sd.nPage + 1;
								Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_THUMBTRACK, pos ), (LPARAM)hWnd );
								Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_BOTTOM, 0 ), (LPARAM)hWnd );
								oldPos = pos;
							}
							else
							{
							    pos = si.sd.nMin;
								Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_THUMBTRACK, pos ), (LPARAM)hWnd );
								Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( SB_TOP, 0 ), (LPARAM)hWnd );
								oldPos = pos;
							}
							edgeMsg = TRUE;
						}
					}
				}
				else
				{
					if( GetTickCount() - dwTickCount >= SLEEP_TICK )
					{	//足够的延迟
						bHandleTimer = TRUE;
						dwTickCount = GetTickCount();
					}
				}
			}
			else if( msg.message == WM_SYSTIMER && msg.wParam == IDSCROLLBAR )
			{
     			if( i != 2 )
       			{   // up arrow or down arrow or page
					bHandleTimer = TRUE;
    			}
			}
			if( bHandleTimer )
			{	//2004-12-21增加 自动处理鼠标消息和timer消息 
				Scrl_GetScrollData( hWnd, fnBar, &si );
				Scrl_GetDrawPos( &si, posv );
				//if( pos >= posv[2] && pos < posv[3] )
				if( ( ncode == SB_PAGEDOWN && pos <= posv[3] ) ||
					( ncode == SB_PAGEUP && pos >= posv[2] ) )
				{
					bHasTimer = FALSE;
				}
				else
					Scrl_SendScrollMsg( hWnd, fnBar, dwMsg, MAKELONG( ncode, 0 ), (LPARAM)hWnd );
			}
			else if( !(msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) )
			{
				DispatchMessage( &msg );
			}
		}
		else
		{   // idel handle
//			PostMessage( hWnd, 0x118, 0, 0 );
            ASSERT( 0 );
		}
	}

    KillSysTimer( hWnd, IDSCROLLBAR );
#ifdef EML_DOS
    _HideMouse();
#endif
    SelectObject( hdc, hBrush );
	ReleaseDC( hWnd, hdc );
	ReleaseCapture();
    return 0;
}

// **************************************************
// 声明：LRESULT Scrl_DoSETSCROLLINFO( HWND hWnd, int fnBar, LPCSCROLLINFO lpsi,  BOOL fRedraw )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	IN lpsi - 包含滚动条信息的结构指针
//	IN fRedraw - 是否重绘
// 返回值：
//	返回0
// 功能描述：
//	配置滚动条数据
// 引用: 
//	
// ************************************************

#define SCRL_DEBUG 0
LRESULT Scrl_DoSETSCROLLINFO( HWND hWnd, int fnBar, LPCSCROLLINFO lpsi,  BOOL fRedraw )
{
	HDC hdc;
	_SCROLL srd;
	BOOL fChange = FALSE;
	RECT rect, newRect;
	int nPos, nPage;
	int nLimit;
	//得到滚动条对象数据
    Scrl_GetScrollData( hWnd, fnBar ,&srd );
	Scrl_GetThumbRect( &srd, &rect );

	if( lpsi->fMask & SIF_RANGE )
	{	//设置滚动条范围数据
		if( lpsi->nMin >= lpsi->nMax )
		    goto _HIDE_OR_DISABLE;
        if( srd.sd.nMin != lpsi->nMin )
		{
			RETAILMSG( SCRL_DEBUG, (TEXT("Scrl:nMin=%d\r\n"), lpsi->nMin ));
            Scrl_SetScrollValue( hWnd, fnBar, OFF_MIN, lpsi->nMin );
			fChange = TRUE;
		}
		if( srd.sd.nMax != lpsi->nMax )
		{
			RETAILMSG( SCRL_DEBUG, (TEXT("Scrl:nMax=%d\r\n"), lpsi->nMax ));
		    Scrl_SetScrollValue( hWnd, fnBar, OFF_MAX, lpsi->nMax );
    	    fChange = TRUE;
		}
		nPage = srd.sd.nPage;
		nLimit = lpsi->nMax - lpsi->nMin + 1;
        // if possible, change page and pos size
        if( !(lpsi->fMask & SIF_PAGE) && 
            nPage > nLimit )
        {
			nPage = nLimit;
			RETAILMSG( SCRL_DEBUG, (TEXT("Scrl:change nPage=%d\r\n"), nPage ));
		    Scrl_SetScrollValue( hWnd, fnBar, OFF_PAGE, nPage );
    	    fChange = TRUE;
        }

		nLimit = lpsi->nMax - MAX( nPage - 1, 0);

        if( !(lpsi->fMask & SIF_POS) && 
            ( srd.sd.nPos > nLimit || srd.sd.nPos < lpsi->nMin ) )
        {
			if( srd.sd.nPos > nLimit )
		        Scrl_SetScrollValue( hWnd, fnBar, OFF_POS, nLimit );
			else
				Scrl_SetScrollValue( hWnd, fnBar, OFF_POS, lpsi->nMin );
    	    fChange = TRUE;
        }

	}
	if( lpsi->fMask & SIF_PAGE )
	{	//设置页数据
        if( lpsi->fMask & SIF_RANGE )
        {
            if( lpsi->nPage >= (UINT)(lpsi->nMax - lpsi->nMin + 1) )
		        goto _HIDE_OR_DISABLE;	
        }
        else
        {
            if( lpsi->nPage >= (UINT)(srd.sd.nMax - srd.sd.nMin + 1) )
		        goto _HIDE_OR_DISABLE;	
        }

        if( (UINT)srd.sd.nPage != lpsi->nPage )
		{
			RETAILMSG( SCRL_DEBUG, (TEXT("Scrl:nPage=%d\r\n"), lpsi->nPage ));
		    Scrl_SetScrollValue( hWnd, fnBar, OFF_PAGE, lpsi->nPage );
		    fChange = TRUE;
		}
	}
	if( lpsi->fMask & SIF_POS )
	{
		//设置位置数据
		nPos = lpsi->nPos;
		Scrl_GetScrollData( hWnd, fnBar ,&srd );

		nPage = srd.sd.nPage;
		nLimit = srd.sd.nMax - MAX( nPage - 1, 0);

	    if( nPos > nLimit )
		    nPos = nLimit;
	    if( nPos < srd.sd.nMin )
			nPos = srd.sd.nMin;

		if( nPos != srd.sd.nPos )
		{
			RETAILMSG( SCRL_DEBUG, (TEXT("Scrl:nPos=%d\r\n"), nPos ));
			Scrl_SetScrollValue( hWnd, fnBar, OFF_POS, nPos );
			fChange = TRUE;
		}
	}

	if( fRedraw && 
        (srd.sd.state & SBS_TRACKTHUMB) == 0 && 
        fChange )
	{    // 重绘 redraw
		if( fnBar != SB_CTL )
		{	
			DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
			if( ( fnBar == SB_HORZ && (dwStyle & WS_HSCROLL) == 0 ) ||
				( fnBar == SB_VERT && (dwStyle & WS_VSCROLL) == 0 ) )
			{
				ShowScrollBar( hWnd, fnBar, TRUE );
				goto _RET;
			}
		}
		hdc = Scrl_GetScrollDC( hWnd, fnBar );
        // get new thumb rect
        Scrl_GetScrollData( hWnd, fnBar ,&srd );
		Scrl_GetThumbRect( &srd, &newRect );
        // show thumb
		//Scrl_DrawThumbBox( hdc, fnBar, srd.sd.state, &rect, &newRect );
		Scrl_DrawThumbBox( hdc, &srd, &rect, &newRect );

		ReleaseDC( hWnd, hdc );
	}
_RET:
	return Scrl_GetScrollValue( hWnd, fnBar, OFF_POS );
//处理无效或隐含情况
_HIDE_OR_DISABLE:
	RETAILMSG( SCRL_DEBUG, (TEXT("Scrl:Hide or disable\r\n") ));
    if( fnBar != SB_CTL )
	{
		if( lpsi->fMask & SIF_DISABLENOSCROLL )
			EnableScrollBar( hWnd, fnBar, ESB_DISABLE_BOTH );	        
		else
			ShowScrollBar( hWnd, fnBar, FALSE );			
	}
	return Scrl_GetScrollValue( hWnd, fnBar, OFF_POS );
}

// **************************************************
// 声明：LRESULT Scrl_DoGETSCROLLINFO( HWND hWnd, int fnBar, void * lp )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	OUT lp - 用于接受滚动条数据的 SCROLLINFO 结构指针
// 返回值：
//	返回0
// 功能描述：
//	得到滚动条数据
// 引用: 
//	
// ************************************************

LRESULT Scrl_DoGETSCROLLINFO( HWND hWnd, int fnBar, void * lp )
{
	_SCROLL srl;
    LPSCROLLINFO lpsi = (LPSCROLLINFO)lp;
	//得到滚动条数据
    Scrl_GetScrollData( hWnd, fnBar, &srl );

	if( lpsi->fMask & SIF_PAGE )
		lpsi->nPage = srl.sd.nPage;
	if( lpsi->fMask & SIF_POS )
		lpsi->nPos = srl.sd.nPos;
	if( lpsi->fMask & SIF_RANGE )
	{
		lpsi->nMax = srl.sd.nMax;
		lpsi->nMin = srl.sd.nMin;
	}
	return TRUE;
}

// **************************************************
// 声明：static LRESULT Scrl_DoSETPOS( HWND hWnd, int fnBar, int nPos, BOOL fRedraw )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	IN nPos - 位置数据
//	IN fRedraw - 是否重绘
// 返回值：
//	返回0
// 功能描述：
//	设置滚动条位置
// 引用: 
//	
// ************************************************

static LRESULT Scrl_DoSETPOS( HWND hWnd, int fnBar, int nPos, BOOL fRedraw )
{
	HDC hdc = 0;
	_SCROLL srl;
	int i, state, fUpdate = FALSE;
    RECT rcOld, rcNew;
	//得到滚动条数据
	Scrl_GetScrollData( hWnd, fnBar, &srl );

	state = srl.sd.state;

	// 检查有效性 check valid
	if( nPos + srl.sd.nPage > srl.sd.nMax + 1 )
		nPos = srl.sd.nMax - srl.sd.nPage + 1;
	if( nPos < srl.sd.nMin )
		nPos = srl.sd.nMin;
	// set new pos
	i = srl.sd.nPos;

	if( fRedraw && 
        (state & SBS_TRACKTHUMB) == 0 && 
        i != nPos )
	{
        fUpdate = TRUE;
       	Scrl_GetThumbRect( &srl, &rcOld );
	}
	//设置值
	Scrl_SetScrollValue( hWnd, fnBar, OFF_POS, nPos );

	if( fUpdate )
	{
	    Scrl_GetScrollData( hWnd, fnBar, &srl );
       	Scrl_GetThumbRect( &srl, &rcNew );
		//得到绘图DC
        hdc = Scrl_GetScrollDC( hWnd, fnBar );
		//绘制
       	//Scrl_DrawThumbBox( hdc, srl.fnBar, srl.sd.state, &rcOld, &rcNew );
		Scrl_DrawThumbBox( hdc, &srl, &rcOld, &rcNew );
		ReleaseDC( hWnd, hdc );
	}
	return i;
}

// **************************************************
// 声明：static LRESULT Scrl_DoGETPOS( HWND hWnd, int fnBar )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
// 返回值：
//	返回滑快位置数据
// 功能描述：
//	得到滑快位置数据
// 引用: 
//	
// ************************************************

static LRESULT Scrl_DoGETPOS( HWND hWnd, int fnBar )
{
	return Scrl_GetScrollValue( hWnd, fnBar, OFF_POS );
}

// **************************************************
// 声明：static LRESULT Scrl_DoGETRANGE( HWND hWnd, int fnBar, LPINT lpnMinPos, LPINT lpnMaxPos )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	OUT lpnMinPos - 用于接受范围的小值
//	OUT lpnMaxPos - 用于接受范围的大值
// 返回值：
//	返回0
// 功能描述：
//	得到滚动条范围
// 引用: 
//	
// ************************************************

static LRESULT Scrl_DoGETRANGE( HWND hWnd, int fnBar, LPINT lpnMinPos, LPINT lpnMaxPos )
{
	_SCROLL srl;

	//得到滚动条数据
	Scrl_GetScrollData( hWnd, fnBar, &srl );

	*lpnMinPos = srl.sd.nMin;
	*lpnMaxPos = srl.sd.nMax;
	return 0;
}

// **************************************************
// 声明：static LRESULT Scrl_DoSETRANGE( HWND hWnd, int fnBar, int nMinPos, int nMaxPos, BOOL fRedraw )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	IN nMinPos - 最小值
//	IN nMaxPos - 最大值
//	IN fRedraw - 是否重绘
// 返回值：
//	返回0
// 功能描述：
//	设置滚动条范围
// 引用: 
//	
// ************************************************

static LRESULT Scrl_DoSETRANGE( HWND hWnd, int fnBar, int nMinPos, int nMaxPos, BOOL fRedraw )
{
	HDC hdc = 0;
	_SCROLL srl;
	int nPos, nPage, state;
    int fUpdate = FALSE;
    RECT rcOld, rcNew;

	if( nMinPos > nMaxPos )
		return 0;
	//得到滚动条数据
	Scrl_GetScrollData( hWnd, fnBar, &srl );
    state = srl.sd.state;
	if( fRedraw && (state & SBS_TRACKTHUMB) == 0 )
	{	//
        fUpdate = TRUE;
	    Scrl_GetThumbRect( &srl, &rcOld );
	}
	
	nPos = srl.sd.nPos;
	nPage = srl.sd.nPage;
	//设置范围
	Scrl_SetScrollValue( hWnd, fnBar, OFF_MIN, nMinPos );
	Scrl_SetScrollValue( hWnd, fnBar, OFF_MAX, nMaxPos );
	//检查范围
	if( nPage > nMaxPos - nMinPos )
		nPage=nMaxPos-nMinPos;
	if( nPos < nMinPos )
		Scrl_SetScrollValue( hWnd, fnBar, OFF_POS, nMinPos );
	else if( nPos + nPage > nMaxPos + 1 )
		Scrl_SetScrollValue( hWnd, fnBar, OFF_POS, nMaxPos - nPage + 1 );

	if( fUpdate )
	{
	    Scrl_GetScrollData( hWnd, fnBar, &srl );
        Scrl_GetThumbRect( &srl, &rcNew );

        hdc = Scrl_GetScrollDC( hWnd, fnBar );
   	    //Scrl_DrawThumbBox( hdc, srl.fnBar, srl.sd.state, &rcOld, &rcNew );
		Scrl_DrawThumbBox( hdc,  &srl, &rcOld, &rcNew );
		ReleaseDC( hWnd, hdc );
	}
	return nPos;
}

// **************************************************
// 声明：static BOOL Scrl_SendScrollMsg( HWND hWnd, int fnBar, UINT dwMsg, WPARAM wParam, LPARAM lParam )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	IN dwMsg - 消息
//	IN wParam - 消息附带的 WPARAM 参数
//	IN lParam - 消息附带的 LPARAM 参数
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	向父发送 通知消息
// 引用: 
//	
// ************************************************

static BOOL Scrl_SendScrollMsg( HWND hWnd, int fnBar, UINT dwMsg, WPARAM wParam, LPARAM lParam )
{
    if( fnBar == SB_CTL )
    {
        hWnd = GetParent( hWnd );
        return SendMessage( hWnd, dwMsg, wParam, lParam );
    }
    else
        return SendMessage( hWnd, dwMsg, wParam, 0 );
}

// **************************************************
// 声明：static HDC Scrl_GetScrollDC( HWND hWnd, int fnBar )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
// 返回值：
//	假入成功，返回DC句柄
// 功能描述：
//	得到绘图DC
// 引用: 
//	
// ************************************************

static HDC Scrl_GetScrollDC( HWND hWnd, int fnBar )
{
    if( fnBar == SB_CTL )
        return GetDC( hWnd );
    else
        return GetWindowDC( hWnd );
}

// **************************************************
// 声明：static int Scrl_SetScrollState( HWND hWnd, int fnBar, UINT s, BOOL fEnable )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	IN s - 状态值
//	IN fEnable - 设置或清除
// 返回值：
//	假入成功，返回老的状态值；否则，返回0
// 功能描述：
//	设置滚动条状态
// 引用: 
//	
// ************************************************

static int Scrl_SetScrollState( HWND hWnd, int fnBar, UINT s, BOOL fEnable )
{
    //_LPWINDATA lpws = _GetHWNDPtr(hWnd);
	short * p = (short*)_GetWindowScrollBar( hWnd, fnBar );
	if( p )
	{
		//short * p = NULL;
		int pos = OFF_STATE;
		int old;
		
		//if( fnBar == SB_CTL )
		//	p = (short*)(lpws+1);
		//else
		//{
			//2004-09-06- modify
		//	p = (short*)_GetWindowScrollBar( lpws, fnBar );
			//if( fnBar == SB_VERT )
			//	p = (short*)&lpws->vScrollBar;
			//else if( fnBar == SB_HORZ )
			//	p = (short*)&lpws->hScrollBar;
			//
		//}
		// pos is byte pos, but p is short pointer
		//if( p )
		{
			pos >>= 1;
			old = *(p+pos);
			if( fEnable )
				*(p+pos) |= s;
			else
				*(p+pos) &= ~s;
			return old;
		}
	}
	return 0;
}

// **************************************************
// 声明：static void Scrl_SetScrollValue( HWND hWnd, int fnBar, int pos, int v )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	int pos - 在结构中的位置偏移值
//	int v - 需要设置的值
// 返回值：
//	无
// 功能描述：
//	设置值
// 引用: 
//	
// ************************************************

static void Scrl_SetScrollValue( HWND hWnd, int fnBar, int pos, int v )
{
    //_LPWINDATA lpws = _GetHWNDPtr(hWnd);
	short * p = (short*)_GetWindowScrollBar( hWnd, fnBar );
	//if( lpws )
	if( p )
	{
		//short * p;
		//
		//if( fnBar == SB_CTL )
		//	p = (short*)(lpws+1);
		//else
		//{	//2004-09-06- modify
		//	p = (short*)_GetWindowScrollBar( lpws, fnBar );
			//if( fnBar == SB_VERT )
			//	p = (short*)&lpws->vScrollBar;
			//else if( fnBar == SB_HORZ )
			//	p = (short*)&lpws->hScrollBar;
		//}
		// pos is byte pos, but p is short pointer
		pos >>= 1;
		*(p+pos) = v;
	}
}

// **************************************************
// 声明：static int Scrl_GetScrollValue( HWND hWnd, int fnBar, int pos )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条类型，为下值：
//						SB_VERT - 垂直
//						SB_HORT - 水平
//	int pos - 在结构中的位置偏移值
// 返回值：
//	假入成功，返回值
// 功能描述：
//	得到值
// 引用: 
//	
// ************************************************

static int Scrl_GetScrollValue( HWND hWnd, int fnBar, int pos )
{
	short * p = (short*)_GetWindowScrollBar( hWnd, fnBar );
    //_LPWINDATA lpws = _GetHWNDPtr(hWnd);
	//if( lpws )
	if( p )
	{
		//short * p;
		
		//if( fnBar == SB_CTL )
		//	p = (short*)(lpws+1);
		//else
		//{
			//2004-09-06- modify
		//	p = (short*)_GetWindowScrollBar( lpws, fnBar );
			//if( fnBar == SB_VERT )
			//	p = (short*)&lpws->vScrollBar;
			//else if( fnBar == SB_HORZ )
			//	p = (short*)&lpws->hScrollBar;
			//
		//}
		// pos is byte pos, but p is short pointer
		pos >>= 1;
		return *(p+pos);
	}
	return 0;
}

// **************************************************
// 声明：BOOL Scrl_Inital( _LPSCROLLDATA p )
// 参数：
// 	IN p - _SCROLLDATA 结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化 _SCROLLDATA 结构指针
// 引用: 
//	
// ************************************************

static BOOL Scrl_Inital( _LPSCROLLDATA p )
{
//	LPSCRLBAR_BRUSH lpscrlBrush ;
	p->nMin = 0;
	p->nMax = 100;
    p->nPage = 0;
    p->nPos = 0;
    p->nTrackPos = 1;
    p->state = 0;
	//p->lpvUserData = calloc( 1, sizeof( SCRLBAR_BRUSH ) );
	//if( p->lpvUserData )
	//{
		p->lpvUserData = InitScrollBarBrush();
	//}
    return TRUE;
}


// **************************************************
// 声明：static LRESULT DoCREATE( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	假入成功，返回0；否则，返回-1
// 功能描述：
//	处理 WM_CREATE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoCREATE( HWND hWnd )
{
    //_LPWINDATA lpws = _GetHWNDPtr(hWnd);
	//if( lpws )
	{
	//	_LPSCROLLDATA p = (_LPSCROLLDATA)(lpws+1);
		_LPSCROLLDATA p = _GetWindowScrollBar( hWnd, SB_CTL );
		if( p )		
		    Scrl_Inital( p );
		return 0;
	}
	return -1;
}

// **************************************************
// 声明：static LRESULT DoPAINT( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	假入成功，返回0
// 功能描述：
//	处理 WM_PAINT 消息	
// 引用: 
//	
// ************************************************

static LRESULT DoPAINT( HWND hWnd )
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint( hWnd, &ps );
    Scrl_RedrawScrollBar( hWnd, hdc, SB_CTL );

	EndPaint( hWnd, &ps );
	return 0;
}

// **************************************************
// 声明：LRESULT WINAPI ScrollBarWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// 参数：
//  IN hWnd - 窗口句柄
//	IN message - 消息
//	IN wParam - 第一个消息
//	IN lParam - 第二个消息
// 返回值：
//	依赖于具体的消息
// 功能描述：
//	按钮窗口处理总入口
// 引用: 
//	
// ************************************************
// this all message handle center
static LRESULT CALLBACK ScrollBarWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch( message )
	{
	case WM_PAINT:
		return DoPAINT( hWnd );
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		return Scrl_DoLBUTTONDOWN( hWnd, SB_CTL, (short)LOWORD( lParam ), (short)HIWORD( lParam ) );
	case SBM_SETSCROLLINFO:
		return Scrl_DoSETSCROLLINFO( hWnd, SB_CTL, (LPSCROLLINFO)lParam, (BOOL)wParam );
	case SBM_GETSCROLLINFO:
		return Scrl_DoGETSCROLLINFO( hWnd, SB_CTL, (LPSCROLLINFO)lParam );
	case SBM_GETPOS:
		return Scrl_DoGETPOS( hWnd, SB_CTL );
	case SBM_SETPOS:
		return Scrl_DoSETPOS( hWnd, SB_CTL, (int)wParam, (BOOL)lParam );
	case SBM_GETRANGE:
		return Scrl_DoGETRANGE( hWnd, SB_CTL, (LPINT)wParam, (LPINT)lParam );
	case SBM_SETRANGE:
		return Scrl_DoSETRANGE( hWnd, SB_CTL, wParam, lParam, FALSE );
	case SBM_SETRANGEREDRAW:
		return Scrl_DoSETRANGE( hWnd, SB_CTL, wParam, lParam, TRUE );
    case SBM_ENABLE_ARROWS:
        return EnableScrollBar( hWnd, SB_CTL, wParam );
	case WM_STYLECHANGING:
		{
			STYLESTRUCT * lpss = (STYLESTRUCT *)lParam;
			lpss->styleOld &= ~(WS_VSCROLL | WS_HSCROLL);
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
		
	case WM_DESTROY:
		_ReleaseWindowScrollBar( hWnd, SB_CTL );
		return 0;
	case WM_CREATE:
		return DoCREATE( hWnd );
	case WM_NCCREATE:
		{
			LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
			lpcs->style &= ~(WS_VSCROLL | WS_HSCROLL);
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
	//2005-09-20, add for WS_GROUP by lilin
	case DLGC_WANTALLKEYS:
		return DLGC_WANTALLKEYS;
	//

	}

	return DefWindowProc(hWnd,message,wParam,lParam);
}


// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int WINAPI Wnd_GetScrollInfo(HWND hWnd, int fnBar, LPSCROLLINFO lpsi )
{
	return Scrl_DoGETSCROLLINFO( hWnd, fnBar, lpsi );
//    if( fnBar == SB_CTL )
//	{  
  //      return (int)WinMsg_Send( hWnd, SBM_GETSCROLLINFO, (WPARAM)0, (LONG)lpsi );
	//}
    //else
	//{
//        
//	}
}


// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int WINAPI Wnd_SetScrollInfo(HWND hWnd, int fnBar, LPCSCROLLINFO lpcsi, BOOL fRedraw)
{
	return Scrl_DoSETSCROLLINFO( hWnd, fnBar, lpcsi, fRedraw );
/*
    if( fnBar == SB_CTL )
	{  //// map lpcsi ??????????????????
        return (int)WinMsg_Send( hWnd, SBM_SETSCROLLINFO, fRedraw, (LPARAM)lpcsi );
	}
    else
	{
//		HANDLE hCallerProcess = GetCallerProcess();
//		if( hCallerProcess )
//			lpcsi = MapPtrToProcess( (LPVOID)lpcsi, hCallerProcess );
        return Scrl_DoSETSCROLLINFO( hWnd, fnBar, lpcsi, fRedraw );
	}
*/
}

// **************************************************
// 声明：BOOL WINAPI EnableScrollBar(HWND hWnd, UINT fnBar, UINT wArrows )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN fnBar - 滚动条
//						SB_VERT - 垂直
//						SB_HORT - 水平
//						SB_BOTH - 垂直和滚动
//	IN wArrows - 箭头类别
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	有效滚动条方向箭头
// 引用: 
//	
// ************************************************

BOOL WINAPI Wnd_EnableScrollBar(HWND hWnd, UINT fnBar, UINT wArrows )
{
    HDC hdc;
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	BOOL bSet = TRUE;
	UINT uiRedraw = 0;

	if( wArrows == ESB_ENABLE_BOTH )
	{
		wArrows = ESB_DISABLE_BOTH;
		bSet = FALSE;
	}	

	if( (fnBar == SB_BOTH || fnBar == SB_VERT) && (dwStyle & WS_VSCROLL) )
	{
	    Scrl_SetScrollState( hWnd, SB_VERT, wArrows, bSet );
		uiRedraw |= 0x01;
	}
	if( (fnBar == SB_BOTH || fnBar == SB_HORZ) && (dwStyle & WS_HSCROLL) )
	{
		Scrl_SetScrollState( hWnd, SB_HORZ, wArrows, bSet );
		uiRedraw |= 0x02;
	}

	if( uiRedraw )
	{	//重绘
		hdc = GetWindowDC( hWnd );

		if( uiRedraw & 0x01 )
			Scrl_RedrawScrollBar( hWnd, hdc, SB_VERT );
		if( uiRedraw & 0x02 )
			Scrl_RedrawScrollBar( hWnd, hdc, SB_HORZ );

		ReleaseDC( hWnd, hdc );
	}
    return TRUE;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL WINAPI Wnd_ShowScrollBar( HWND hWnd, int fnBar, BOOL bShow )
{
    //_LPWINDATA lpws = _GetHWNDPtr( hWnd );

    if( fnBar == SB_CTL )
        ShowWindow( hWnd, bShow );
    else
    {
        BOOL fAdd;
		DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );//lpws->dwMainStyle;
		DWORD dwOldStyle = dwStyle;
        if( bShow )
        {
            if( fnBar == SB_VERT )
                dwStyle |= WS_VSCROLL;
            else if( fnBar == SB_HORZ )
                dwStyle |= WS_HSCROLL;
            else if( fnBar == SB_BOTH )
                dwStyle |= WS_HSCROLL | WS_VSCROLL;
            fAdd = FALSE;
        }
        else
        {
            if( fnBar == SB_VERT )
                dwStyle &= ~WS_VSCROLL;
            else if( fnBar == SB_HORZ )
                dwStyle &= ~WS_HSCROLL;
            else if( fnBar == SB_BOTH )
                dwStyle &= ~(WS_HSCROLL|WS_VSCROLL);
            fAdd = TRUE;
        }
        if( dwStyle != dwOldStyle )
		{
			SetWindowLong( hWnd, GWL_STYLE, dwStyle );
		}
    }
    return TRUE;
}
