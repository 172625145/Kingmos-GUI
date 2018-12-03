/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：静态类 static class
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <estatic.h>
//#include <gwmesrv.h>

// 类名

typedef struct _STATIC_ATTRIB
{
	HANDLE   hImage;
    COLORREF cl_Text;//正常文本的前景
    COLORREF cl_TextBk;     //正常文本的前景与背景色
	COLORREF cl_Disable;    // 无效文本的前景色
	COLORREF cl_DisableBk;    // 无效文本的背景色
}STATIC_ATTRIB, * PSTATIC_ATTRIB;

static const char strClassStatic[]="STATIC";

static LRESULT CALLBACK StaticWndProc( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static void DoPAINT( HWND hwnd );
static VOID _SetStaticColor( PSTATIC_ATTRIB pAttrib );


// **************************************************
// 声明：ATOM RegisterButtonClass(HINSTANCE hInst)
// 参数：
//		IN hInst - 实例句柄
// 返回值：
//		假如成功，返回非0值；失败，返回0
// 功能描述：
//		注册按钮类
// 引用: 
//		被sysclass.c 调用
// ************************************************
ATOM RegisterStaticClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = StaticWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( PSTATIC_ATTRIB );
    wc.hInstance = hInst;
    wc.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION );
    wc.hCursor = 0;//LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = NULL;//GetStockObject( LTGRAY_BRUSH );
    wc.lpszMenuName = 0;
    wc.lpszClassName = strClassStatic;

    return (BOOL)(RegisterClass( &wc ));
}

// **************************************************
// 声明：static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
// 参数：
//	IN hdc - 显示设备句柄
//	IN lprc - RECT 结构指针，指向需要填充的矩形
//	IN clr - RGB颜色值
// 返回值：
//	无
// 功能描述：
//	用固定颜色填充举行矩形
// 引用: 
//	
// ************************************************
static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

// **************************************************
// 声明：static void DoDraw( HWND hwnd, HDC hdc, DWORD dwStyle )
// 参数：
//	IN hwnd - 窗口句柄
//	IN hdc - 显示设备句柄
//	IN dwStyle - 窗口风格
// 返回值：
//	无
// 功能描述：
//	用固定颜色填充举行矩形
// 引用: 
//	
// ************************************************
static void DoDraw( HWND hwnd, HDC hdc, DWORD dwStyle )
{
	HBITMAP hBitmap;
	RECT rect;
	HDC hMemDC;
	HPEN hPen;
    BITMAP bitmap;
    int i;
    DWORD dwDrawStyle;//prefix;
	PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hwnd, 0 );// 从窗口私有数据区得到控件属性

	if( (dwStyle & 0x1f) == SS_OWNERDRAW  )
    {	// 自绘风格
	    DRAWITEMSTRUCT di;
        di.CtlID = (WORD)GetWindowLong( hwnd, GWL_ID );
        di.CtlType = ODT_STATIC;
        di.hDC = hdc;
        di.hwndItem = hwnd;
        di.itemAction = ODA_DRAWENTIRE;
        di.itemData = 0;
        di.itemID = 0;
        di.itemState = ODS_DEFAULT;
        GetClientRect( hwnd, &di.rcItem );	// 得到窗口客户区矩形坐标
		// 向父窗口发送通知消息
        SendMessage( GetParent( hwnd ), WM_DRAWITEM, (WPARAM)di.CtlID, (LPARAM)&di );
        return;
    }
	GetClientRect( hwnd, &rect );// 得到窗口客户区矩形坐标
    
    if( dwStyle & SS_NOPREFIX )	// 控件包含前缀字符吗 ？例如："&Hello" 的'&'
        dwDrawStyle = DT_NOPREFIX;	// 没有前缀字符
    else
        dwDrawStyle = 0;
	dwDrawStyle |= DT_END_ELLIPSIS;	// 用省略号表示无法显示部分，例如："Hello,W..."

	switch( (dwStyle & 0x1f) )
	{
	case SS_CENTER:			// 文本居中
	    dwDrawStyle |= DT_CENTER | DT_WORDBREAK;
		break;
	case SS_LEFT:			// 文本左对齐
        dwDrawStyle	|= DT_LEFT | DT_WORDBREAK;
		break;
	case SS_RIGHT:			// 文本右对齐
		dwDrawStyle	|= DT_RIGHT | DT_WORDBREAK;
		break;
	case SS_LEFTNOWORDWRAP:		// 
		dwDrawStyle	|= DT_LEFT;
	}

	switch( dwStyle & 0x1f )
	{
	case SS_BITMAP:			// 位图风格
		hBitmap = pAttrib->hImage;
        GetObject( hBitmap, sizeof(bitmap), &bitmap );
		hMemDC =  CreateCompatibleDC( hdc );
		hBitmap = SelectObject( hMemDC, hBitmap );
        BitBlt( hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY );
        hBitmap = SelectObject( hMemDC, hBitmap );
		DeleteDC( hMemDC );
		break;
    case SS_ICON:			// 图标风格
        DrawIcon( hdc, 0, 0, pAttrib->hImage );
        break;
	case SS_BLACKFRAME:		// 黑色边框
		SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
		Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
		break;
	case SS_BLACKRECT:		// 黑色矩形
        FillRect( hdc, &rect, GetStockObject( BLACK_BRUSH ) );
        break;
	case SS_WHITEFRAME:		// 白色边框
		SelectObject( hdc, GetStockObject( WHITE_PEN ) );
        SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
		Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
        break;
	case SS_WHITERECT:		// 白色矩形
		FillRect( hdc, &rect, GetStockObject( WHITE_BRUSH ) );
        break;
	case SS_GRAYFRAME:		// 灰色边框
        hPen = CreatePen( PS_SOLID, 1, CL_LIGHTGRAY );
        hPen = SelectObject( hdc, hPen );
        SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
		Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
        hPen = SelectObject( hdc, hPen );
        DeleteObject( hPen );
        break;
	case SS_GRAYRECT:		// 灰色矩形
        FillRect( hdc, &rect, GetStockObject( GRAY_BRUSH ) );
		break;
	case SS_CENTER:
    case SS_LEFT:
	case SS_RIGHT:
	case SS_LEFTNOWORDWRAP:
		SetBkMode( hdc, TRANSPARENT );		// 设置文本的背景为透明模式
		i = GetWindowTextLength(hwnd);
		i += sizeof(TCHAR); //+0结束符
		if( i )
		{
			TCHAR szText[128];
			LPTSTR lpstr;
			if( i <= sizeof(szText) )
				lpstr = szText;
		    else
			    lpstr = (LPSTR)malloc( i );   // 需要以后改进
			if( lpstr )
			{
				i = GetWindowText( hwnd, lpstr, i );	// 得到文本
				// 设置需要显示的文本颜色
				if( dwStyle & WS_DISABLED )			// 
					SetTextColor( hdc, pAttrib->cl_Disable );
				else
				    SetTextColor( hdc, pAttrib->cl_Text );
				// 画文本
				DrawText( hdc,
					lpstr,
					i,
					&rect,
					dwDrawStyle );
				if( lpstr != szText )
				    free(lpstr);	//用malloc分配的
			}
		}
		break;
	}
}

// **************************************************
// 声明：static LRESULT DoLButtonDown( HWND hWnd ) //, short x, short y )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_LBUTTONDOWN 消息
// 引用: 
//	
// ************************************************

static LRESULT DoLButtonDown( HWND hWnd ) //, short x, short y )
{
	HWND hParent = GetParent( hWnd );
	if( hParent )
	{	// 得到控件风格
		DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		if( dwStyle & SS_NOTIFY )	// 是否发通知消息 ？
		{	// 是
			WPARAM wParam = (WPARAM)GetWindowLong( hWnd, GWL_ID ); // 控件ID
			wParam = MAKELONG( wParam, STN_CLICKED );		// 通知消息类型
			//	向父窗口发通知消息
			return SendMessage( hParent, WM_COMMAND, wParam, (LPARAM)hWnd );
		}
	}
	return 0;
}

// **************************************************
// 声明：static LRESULT DoEnable( HWND hWnd, BOOL bEnable )
// 参数：
//	IN hWnd - 窗口句柄
//	IN bEnable - 控件是否处于有效状态
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_ENABLE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoEnable( HWND hWnd, BOOL bEnable )
{
	HWND hParent = GetParent( hWnd );
	if( hParent )
	{	// 得到控件风格
		DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		HDC hdc = GetDC( hWnd );
		DoDraw( hWnd, hdc, dwStyle );
		ReleaseDC( hWnd, hdc );
		if( dwStyle & SS_NOTIFY )	// 是否发通知消息 ？
		{
			WPARAM wParam = (WPARAM)GetWindowLong( hWnd, GWL_ID );// 控件ID
			// 通知消息类型
			if( bEnable )
				wParam = MAKELONG( wParam, STN_ENABLE );
			else
				wParam = MAKELONG( wParam, STN_DISABLE );
			//	向父窗口发通知消息
			return SendMessage( hParent, WM_COMMAND, wParam, (LPARAM)hWnd );
		}
	}
	return 0;
}

// **************************************************
// 声明：static LRESULT SetImage( HWND hwnd, UINT dwStyle, HANDLE hImage )
// 参数：
//	IN hWnd - 窗口句柄
//	IN dwStyle - 窗口风格
//	IN hImage - 图形句柄
// 返回值：
//	返回之前的图形句柄
// 功能描述：
//	设置控件显示映象
// 引用: 
//	
// ************************************************

static LRESULT SetImage( HWND hwnd, DWORD dwStyle, HANDLE hImage )
{
    HANDLE handle;
    //DWORD dwStyle;
	PSTATIC_ATTRIB pAttrib;

    pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hwnd, 0 );// 从窗口私有数据区得到控件属性

	handle = pAttrib->hImage;
	pAttrib->hImage = hImage;

	//dwStyle = GetWindowLong( hwnd, GWL_STYLE ); // 得到窗口风格 
	// 假如可能，重新绘制
	if( (dwStyle & WS_VISIBLE) == WS_VISIBLE )	// 可视吗 ？
	{	// 可视，绘制窗口客户区
		HDC hdc;
		// 
        hdc = GetDC( hwnd );	// 得到显示设备DC
		DoDraw( hwnd, hdc, dwStyle );	// 实际的绘制
		ReleaseDC( hwnd, hdc );	// 释放DC
	}
    return (LRESULT)handle;
}

// **************************************************
// 声明：static LRESULT DoSetImage( HWND hwnd, UINT uiType, HANDLE hImage )
// 参数：
//	IN hwnd - 窗口句柄
//	IN uiType - 图形对象的类型
//	IN hImage - 图形句柄
// 返回值：
//	假如成功，返回之前的图形句柄；否则，返回NULL
// 功能描述：
//	设置控件显示映象，处理 STM_SETICON 和 STM_SETIMAGE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoSetImage( HWND hwnd, UINT uiType, HANDLE hImage )
{
	DWORD dwStyle;

	dwStyle = GetWindowLong( hwnd, GWL_STYLE );// 得到窗口风格
	switch( uiType )
	{
	case IMAGE_BITMAP:
		if( dwStyle & SS_BITMAP )	// 映象类型与窗口风格是否一致 ？
		{	// 是
			return SetImage( hwnd, dwStyle, hImage );
		}
		break;
//	case IMAGE_CURSOR:
	case IMAGE_ICON:
		if( dwStyle & SS_ICON )	// 映象类型与窗口风格是否一致 ？
		{	// 是
			return SetImage( hwnd, dwStyle, hImage );
		}
		break;
	}
	return NULL;	// 
}

// **************************************************
// 声明：static LRESULT GetImage( HWND hwnd, UINT uiType )
// 参数：
//	IN hwnd - 窗口句柄
//	IN uiType - 图形对象的类型
// 返回值：
//	假如成功，返回当前的图形句柄；否则，返回NULL
// 功能描述：
//	得到控件显示映象
// 引用: 
//	
// ************************************************

static LRESULT GetImage( HWND hwnd, UINT uiType )
{
    return (LRESULT)((PSTATIC_ATTRIB)GetWindowLong( hwnd, 0 ))->hImage;
}

// **************************************************
// 声明：static LRESULT DoGetImage( HWND hwnd, UINT uiType )
// 参数：
//	IN hwnd - 窗口句柄
//	IN uiType - 图形对象的类型
// 返回值：
//	假如成功，返回当前的图形句柄；否则，返回NULL
// 功能描述：
//	得到控件显示映象，处理 STM_GETICON 和 STM_GETIMAGE 消息
// 引用: 
//	
// ************************************************
static LRESULT DoGetImage( HWND hwnd, UINT uiType )
{    
	DWORD dwStyle;

	dwStyle = GetWindowLong( hwnd, GWL_STYLE );// 得到窗口风格
	switch( uiType )
	{
	case IMAGE_BITMAP:
		if( dwStyle & SS_BITMAP )// 映象类型与窗口风格是否一致 ？
		{
			return GetImage( hwnd, uiType );
		}
		break;
//	case IMAGE_CURSOR:
	case IMAGE_ICON:
		if( dwStyle & SS_ICON )	// 映象类型与窗口风格是否一致 ？
		{
			return GetImage( hwnd, uiType );
		}
		break;
	}
	return NULL;
}

// **************************************************
// 声明：static void DoPAINT( HWND hwnd )
// 参数：
//	IN hwnd - 窗口句柄
// 返回值：
//	无
// 功能描述：
//	绘制客户区，处理 WM_PAINT 消息
// 引用: 
//	
// ************************************************
static void DoPAINT( HWND hwnd )
{
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint( hwnd, &ps );	// 得到绘制DC
	DoDraw( hwnd, hdc, GetWindowLong( hwnd, GWL_STYLE ) );
	EndPaint( hwnd, &ps );	// 释放DC
}

// **************************************************
// 声明：static void DoEraseBkground( HWND hWnd, HDC hdc, DWORD dwStyle )
// 参数：
//	IN hWnd - 窗口句柄
//	IN hdc - DC句柄
// 返回值：
//	无
// 功能描述：
//	绘制客户区背景
// 引用: 
//	
// ************************************************
static void DoEraseBkground( HWND hWnd, HDC hdc, DWORD dwStyle  )
{
	RECT rcClient;
	PSTATIC_ATTRIB psa = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );
	//DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	
	GetClientRect( hWnd, &rcClient );
	//FillRect( (HDC)wParam, &rcClient, GetSysColorBrush(COLOR_STATIC) );
	if( dwStyle & WS_DISABLED )
		FillSolidRect( hdc, &rcClient, psa->cl_DisableBk );
	else
		FillSolidRect( hdc, &rcClient, psa->cl_TextBk );
}

// **************************************************
// 声明：static void RedrawWindow( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	无
// 功能描述：
//	重新绘制客户区
// 引用: 
//	
// ************************************************

static void RedrawWindow( HWND hWnd )
{
	HDC hdc = GetDC( hWnd );
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	DoEraseBkground( hWnd, hdc, dwStyle );
	DoDraw( hWnd, hdc, dwStyle );
	ReleaseDC( hWnd, hdc );
}

// **************************************************
// 声明：static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpccs - 控件结构，包含颜色值
// 返回值：
//	成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	设置控件颜色,处理 WM_SETCTLCOLOR 消息
// 引用: 
//	
// ************************************************
static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
		PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );

		if( lpccs->fMask & CLF_TEXTCOLOR )
		{
			pAttrib->cl_Text = lpccs->cl_Text;	// 有效文本颜色
		}
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		{
			pAttrib->cl_TextBk = lpccs->cl_TextBk;	// 有效文本背景颜色
		}
		if( lpccs->fMask & CLF_DISABLECOLOR )
			pAttrib->cl_Disable = lpccs->cl_Disable;	// 无效文本颜色 
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
			pAttrib->cl_DisableBk = lpccs->cl_DisableBk;	// 无效文本背景颜色
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpccs - 控件结构，包含颜色值
// 返回值：
//	成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	得到控件颜色，处理 WM_GETCTLCOLOR 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
		PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );

		if( lpccs->fMask & CLF_TEXTCOLOR )	// 有效文本颜色
			lpccs->cl_Text = pAttrib->cl_Text;
		if( lpccs->fMask & CLF_TEXTBKCOLOR )	// 有效文本背景颜色
			lpccs->cl_TextBk = pAttrib->cl_TextBk;
		if( lpccs->fMask & CLF_DISABLECOLOR )	// 无效文本颜色
			lpccs->cl_Disable = pAttrib->cl_Disable;
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )	// 无效文本背景颜色
			 lpccs->cl_DisableBk = pAttrib->cl_DisableBk;
		return TRUE;
	}
	return FALSE;	
}

// **************************************************
// 声明：static LRESULT DoSysColorChange( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	返回 0
// 功能描述：
//	处理系统颜色改变
// 引用: 
// ************************************************

static LRESULT DoSysColorChange( HWND hWnd )
{
    PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );

	_SetStaticColor( pAttrib );
	return 0;
}

// **************************************************
// 声明：static VOID _SetStaticColor( PSTATIC_ATTRIB pAttrib )
// 参数：
//	IN pAttrib - 静态类数据结构
// 返回值：
//	无
// 功能描述：
//	设置窗口显示颜色
// 引用: 
//	
// ************************************************

static VOID _SetStaticColor( PSTATIC_ATTRIB pAttrib )
{
	pAttrib->cl_Text = GetSysColor( COLOR_STATICTEXT );
	pAttrib->cl_TextBk = GetSysColor( COLOR_STATIC );
	pAttrib->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
	pAttrib->cl_DisableBk = pAttrib->cl_TextBk;
}

// **************************************************
// 声明：static LRESULT DoCreate( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功，返回0; 否则，返回-1
// 功能描述：
//	初始化窗口数据。处理 WM_CREATE 消息
// 引用: 
//	
// ************************************************
static LRESULT DoCreate( HWND hWnd )
{	// 分配控件私有结构
	PSTATIC_ATTRIB pAttrib = malloc( sizeof(STATIC_ATTRIB) );
	if( pAttrib )
	{
		memset( pAttrib, 0, sizeof(STATIC_ATTRIB) );
		_SetStaticColor( pAttrib );
		//pAttrib->cl_Text = GetSysColor( COLOR_STATICTEXT );
		//pAttrib->cl_TextBk = GetSysColor( COLOR_STATIC );
		//pAttrib->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
		//pAttrib->cl_DisableBk = pAttrib->cl_TextBk;
		SetWindowLong( hWnd, 0, (LONG)pAttrib );
		return 0;  // 成功
	}
	return -1;  // 失败
}

// **************************************************
// 声明：static LRESULT DoDestroy( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	破坏窗口数据。处理 WM_DESTROY 消息
// 引用: 
//	
// ************************************************
static LRESULT DoDestroy( HWND hWnd )
{	// 释放控件私有结构（之前在 DoCreate 里创建）
	PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );
	free( pAttrib );
	return 0;
}

// **************************************************
// 声明：LRESULT WINAPI StaticWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
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

LRESULT CALLBACK StaticWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch( uMsg )
	{
	case WM_PAINT:			// 绘制客户区消息
		DoPAINT( hwnd );
		return 0;
    case WM_LBUTTONDOWN:			// 左鼠标按下消息
        return DoLButtonDown( hwnd );//, LOWORD( lParam ), HIWORD( lParam ) );
    case WM_ENABLE:
        return DoEnable( hwnd, (BOOL)wParam );
	case STM_SETICON:	
        return DoSetImage( hwnd, IMAGE_ICON, (HANDLE)wParam );
	case STM_SETIMAGE:
        return DoSetImage( hwnd, wParam, (HANDLE)lParam );
	case STM_GETICON:
		return DoGetImage( hwnd, IMAGE_ICON );
	case STM_GETIMAGE:
		return DoGetImage( hwnd, wParam );
    case WM_SETTEXT:
        DefWindowProc( hwnd, uMsg, wParam, lParam );
        //InvalidateRect( hwnd, 0, TRUE );  // 无效客户区，重绘
		RedrawWindow( hwnd );
        return 0;
	case WM_ERASEBKGND:		// 清除客户区背景
		{			
			DoEraseBkground( hwnd, (HDC)wParam, GetWindowLong( hwnd, GWL_STYLE ) );
		}
		return 1;
    case WM_SETCTLCOLOR:
		return DoSetCtlColor( hwnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:
		return DoGetCtlColor( hwnd, (LPCTLCOLORSTRUCT)lParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hwnd );
	case WM_CREATE:
		return DoCreate( hwnd );
	case WM_DESTROY:
		return DoDestroy( hwnd );
	default:
	// It's important to do this
	// if your do'nt handle message, you must call DefWindowProc
	// 重要提示：假如你不需要处理消息，将该消息交由系统窗口默认处理函数去处理
	// 
    	return DefWindowProc( hwnd, uMsg, wParam, lParam );	// 系统默认处理
	}
	return 0;
}
