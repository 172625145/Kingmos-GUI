/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：按钮类
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <esymbols.h>
#include <eassert.h>
//#include <gwmesrv.h>


#define GETTYPE( dwStyle ) ( (WORD)((dwStyle)&0xf) )

#define ICON_SIZE 16
#define GSB_CHECKNORMAL 0
#define GSB_CHECKSELECT 1
#define GSB_RADIONORMAL 2
#define GSB_RADIOSELECT 3

// 按钮标志
#define BTF_DISABLEFOCUS  0x0001

typedef struct _BUTTON_ATTRIB{
    WORD btState;  // OFFSET_STATE 0
	WORD btFlag;   //	按钮标志
    HANDLE hImage;  // OFFSET_IMAGE 4
	COLORREF cl_Text;
	COLORREF cl_TextBk;     //正常文本的前景与背景色
	COLORREF cl_SelectedText;
	COLORREF cl_SelectedTextBk;     //正常文本的前景与背景色
	COLORREF cl_Disable;
	COLORREF cl_DisableBk;    // 无效文本的前景与背景色
}BUTTON_ATTRIB, * PBUTTON_ATTRIB;

static void DoOwnerDraw( HDC, HWND hWnd, DWORD state, UINT uiAction );
static void DrawCheckState( HDC, HWND hWnd, DWORD state );
static void DrawPushState( HDC, HWND hWnd, BOOL fDefault, DWORD state );
static void DrawButtonState( HDC, HWND hWnd, DWORD style, DWORD state );

static LRESULT DoPAINT( HWND hWnd );
static LRESULT DoLBUTTONDOWN( HWND hWnd );
static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y );
static LRESULT DoSETFOCUS( HWND hWnd );
static LRESULT DoSETSTATE( HWND hWnd, WPARAM wParam );
static LRESULT DoSETCHECK( HWND hWnd, WPARAM wParam );
static LRESULT WINAPI ButtonWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

// 类名
static const char strClassButton[] = "AGL_BUTTON"; 
/*
typedef struct _BUTTON_SHAPE_BRUSH
{
	HBITMAP hbmpBegin;   // button 开始部分的位图
	HBITMAP hbmpMiddle;	// button 中间部分的位图
	HBITMAP hbmpEnd;		// button 末位部分的位图

	HBRUSH hBeginBrush;   // button 开始部分的位图
	HBRUSH hMiddleBrush;	// button 中间部分的位图
	HBRUSH hEndBrush;		// button 末位部分的位图
}BUTTON_SHAPE_BRUSH, FAR * LPBUTTON_SHAPE_BRUSH;

static LPBUTTON_SHAPE_BRUSH lpNormalBrush = NULL;   // 通常显示的刷子
static LPBUTTON_SHAPE_BRUSH lpSelectBrush = NULL;	 // 选中显示的刷子
static BUTTON_SHAPE_BRUSH bsbNormalBrush;
static BUTTON_SHAPE_BRUSH bsbSelectBrush;



//  创建的button的刷子
static HBRUSH CreateScrollBarBrush( UINT id, HBITMAP *lphBitmap )
{
	//HBITMAP hBitmap;

	if( *lphBitmap == NULL )
		*lphBitmap = LoadImage( NULL, MAKEINTRESOURCE( id ), IMAGE_BITMAP, 0, 0, 0 );
	
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
*/
/*
//初始化button的刷子
static VOID InitButtonBrush( ) //LPSCRLBAR_BRUSH lpscrlBrush )
{
	if( lpNormalBrush == NULL )
	{
	    //第一次，初始化...
		bsbNormalBrush.hBeginBrush = CreateScrollBarBrush( OBM_BUTTON_NORMAL_BEGIN, &bsbNormalBrush.hbmpBegin );
		bsbNormalBrush.hMiddleBrush = CreateScrollBarBrush( OBM_BUTTON_NORMAL_MIDDLE, &&bsbNormalBrush.hbmpMiddle );
		bsbNormalBrush.hEndBrush = CreateScrollBarBrush( OBM_BUTTON_NORMAL_END, &&bsbNormalBrush.hbmpEnd );
		lpNormalBrush = &bsbNormalBrush;
	}		
	if( lpSelectBrush == NULL )
	{
	    //第一次，初始化...
		bsbSelectBrush.hBeginBrush = CreateScrollBarBrush( OBM_BUTTON_NORMAL_BEGIN, &bsbSelectBrush.hbmpBegin );
		bsbSelectBrush.hMiddleBrush = CreateScrollBarBrush( OBM_BUTTON_NORMAL_MIDDLE, &&bsbSelectBrush.hbmpMiddle );
		bsbSelectBrush.hEndBrush = CreateScrollBarBrush( OBM_BUTTON_NORMAL_END, &&bsbSelectBrush.hbmpEnd );
		lpSelectBrush = &bsbSelectBrush;
	}		
}
*/


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
// register my define class
ATOM RegisterAngelusButtonClass(HINSTANCE hInst)
{
	WNDCLASS wc;


//	InitButtonBrush();
	
	//初始化类结构
	wc.hInstance=hInst;
	wc.lpszClassName= strClassButton;
	// the proc is class function
	wc.lpfnWndProc=(WNDPROC)ButtonWndProc;
	wc.style=CS_DBLCLKS|CS_CLASSDC;
	wc.hIcon= 0;
	// 基于笔的窗口，没有光标at pen window, no cursor
	wc.hCursor= 0;//LoadCursor(NULL,IDC_ARROW);
	// to auto erase background, must set a valid brush
	// if 0, you must erase background yourself
	wc.hbrBackground = 0;
	wc.lpszMenuName=0;
	wc.cbClsExtra=0;
	// !!! it's important to save state of button, align to long
	wc.cbWndExtra= sizeof( PBUTTON_ATTRIB );
	
	return RegisterClass(&wc);
}

//#define SEND_NOTIFY( hParent, idCtrl, idNotify, hWnd ) \ 
  //      SendMessage( (hParent), WM_COMMAND, MAKELONG( (idCtrl),  (idNotify) ), (LPARAM)(hWnd) )
// **************************************************
// 声明：static int SendNotify( HWND hParent, UINT uiId, UINT uiNotify, HWND hWnd )
// 参数：
//	IN hParent - 父窗口
//	IN uiId - 窗口id
//	IN uiNotify - 通知消息
//	IN hWnd - 窗口句柄
// 返回值：
//	依赖于具体的消息
// 功能描述：
//	向父窗口发送通知消息
// 引用: 
//	
// ************************************************

static int SendNotify( HWND hParent, UINT uiId, UINT uiNotify, HWND hWnd )
{
    return SendMessage( hParent, WM_COMMAND, MAKELONG( uiId, uiNotify ), (LPARAM)hWnd );
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
// 声明：static void GetDrawRect(	HDC hdc, 
//									LPCRECT lpClientRect, 
//									LPRECT lpTextRect, 
//									LPRECT lpIconRect, 
//									DWORD dwStyle )
// 参数：
//	IN hdc - 显示设备句柄
//	IN lpClientRect - 矩形指针，指向窗口的客户矩形
//	OUT lpTextRect - 矩形指针，用于接受按钮的文本矩形
//	OUT lpIconRect - 矩形指针，用于接受按钮的图标矩形
//	IN dwStyle - 窗口风格
// 返回值：
//	无
// 功能描述：
//	得到按钮窗口的文本部分矩形和图标部分矩形
// 引用: 
//	
// ************************************************

static void GetDrawRect( HDC hdc, LPCRECT lpClientRect, LPRECT lpTextRect, LPRECT lpIconRect, DWORD dwStyle )
{
    int type  = GETTYPE( dwStyle );  // 得到按钮风格
    
    *lpTextRect = *lpIconRect = *lpClientRect;
    
    if( type == BS_DEFPUSHBUTTON ||
        type == BS_PUSHBUTTON ||
        (dwStyle & BS_PUSHLIKE) )
    {	// 按钮风格
        if( type == BS_DEFPUSHBUTTON )
        {	// 默认按钮风格
            InflateRect( lpTextRect, -1, -1 );
        }
        InflateRect( lpTextRect, -1, -1 );
    }
    else if( type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON ||
            type == BS_CHECKBOX || type == BS_AUTOCHECKBOX )
    {	// 单选（自动）按钮 或 复选（自动）按钮
        if( dwStyle & BS_LEFTTEXT )
        {	// 按钮文本在左面, 象：[ ButtonText <icon> ] 
            lpIconRect->left = lpIconRect->right - ICON_SIZE;
            lpTextRect->right = lpIconRect->left;// - 2;
        }
        else
        {	// 按钮文本在右面, 象：[ <icon> ButtonText ] 
            lpIconRect->right = lpIconRect->left + ICON_SIZE;
            lpTextRect->left = lpIconRect->right;// + 2;
        }
		// 设置图标的上下边
        lpIconRect->top = ( lpClientRect->bottom + lpClientRect->top) / 2 - ICON_SIZE / 2;
        lpIconRect->bottom = lpIconRect->top + ICON_SIZE;
    }
    else if( type == BS_GROUPBOX )
    {	// 组框
        TEXTMETRIC tm;
        GetTextMetrics( hdc, &tm );
 
        lpTextRect->left += 8;
        lpTextRect->right -= 8;
        lpTextRect->bottom = (short)(lpTextRect->top + tm.tmHeight);
    }
}

// **************************************************
// 声明：static void DrawRadioBox( HDC hdc, LPCRECT lprectIcon, DWORD state )
// 参数：
//	IN hdc - 显示设备句柄
//	IN lprectIcon - 画单选按钮icon的矩形
//	IN state - 单选按钮的状态
// 返回值：
//	无
// 功能描述：
//	画单选按钮
// 引用: 
//	
// ************************************************
/*
static void DrawRadioBox( HDC hdc, LPCRECT lprectIcon, DWORD state )
{
	int iOldMode;
	BYTE bIcon;

	HFONT hfont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );
    iOldMode = SetBkMode( hdc, TRANSPARENT );

    if( state & BST_CHECKED )
        bIcon = SYM_RADIO_SET;
	else
		bIcon = SYM_RADIO_NOSET;
	DrawText( hdc, (LPCTSTR)&bIcon, 1, (LPRECT)lprectIcon, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
    SetBkMode( hdc, iOldMode );
	SelectObject( hdc, hfont );
}
*/

// **************************************************
// 声明：static void DrawCheckBox( HDC hdc, LPCRECT lprectIcon, DWORD state )
// 参数：
//  IN hdc -  显示设备句柄
//	IN lprectIcon - 画icon的矩形
//	IN state - 复选按钮的状态
// 返回值：
//	无
// 功能描述：
//	画复选按钮
// 引用: 
//	
// ************************************************
/*
static void DrawCheckBox( HDC hdc, LPCRECT lprectIcon, DWORD state )
{
	int iOldMode;
	BYTE bIcon;
	HFONT hfont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );
    iOldMode = SetBkMode( hdc, TRANSPARENT );

    if( state & BST_CHECKED )
        bIcon = SYM_CHECK_SET;
	else
		bIcon = SYM_CHECK_NOSET;
	DrawText( hdc, (LPCTSTR)&bIcon, 1, (LPRECT)lprectIcon, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
	SetBkMode( hdc, iOldMode );
	SelectObject( hdc, hfont );

}
*/

// **************************************************
// 声明：static void DrawButtonState( HDC hdc, HWND hWnd, DWORD style, DWORD state )
// 参数：
//  IN hdc -  显示设备句柄
//	IN hWnd - 窗口句柄
//	IN style - 窗口风格
//	IN state - 状态
// 返回值：
//	无	
// 功能描述：
//	画按钮（包含：check button, radio button...）
// 引用: 
//	
// ************************************************
static void DrawButtonState( HDC hdc, HWND hWnd, DWORD style, DWORD state )
{
	WORD type = GETTYPE( style );
	RECT rectClient;//, rectBrush;
//	HBRUSH hBrush;
    PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 ); // 得到按钮属性结构指针
	BOOL bEnable = IsWindowEnabled( hWnd ); // 
	char strBuf[128];
	char * buf = NULL;
	int l, shift = 0;
	WORD textStyle = 0;
	POINT pts[4];
	HPEN hDarkPen = CreatePen( PS_SOLID, 1, RGB( 59, 59, 59 ) ); 
	HPEN hGrayPen = CreatePen( PS_SOLID, 1, RGB( 168, 168, 168 ) ); 
	HPEN hBlackPen = CreatePen( PS_SOLID, 1, RGB( 39, 39, 39 ) ); 

	GetClientRect( hWnd, &rectClient );  // 得到窗口客户矩形
	l = GetWindowTextLength( hWnd );	// 得到窗口文本长度
	if( l < sizeof( strBuf ) )
	{		
	    l = GetWindowText( hWnd, strBuf, sizeof( strBuf ) );	// 得到窗口文本
		buf = strBuf;
	}
	else
	{	// 窗口文本太大，动态分配一个
		l = (l + 1 )* sizeof( char ); 
		buf = malloc( l );
		if( buf )
		{
			GetWindowText( hWnd, buf, l );
		}
		else
			return;
	}


    if( state & (BST_FOCUS|BST_PUSHED|BST_CHECKED) )
    {   // 按钮有焦点 has focus
	    //DrawFocusRect( hdc, &rectText );
		//HANDLE hBrush = CreateSolidBrush( RGB( 255, 202, 92 ) );
		//FillRect( hdc, &rectClient );
		FillSolidRect( hdc, &rectClient, RGB( 255, 202, 92 ) );
	}
	else
	{
		//HANDLE hBrush = CreateSolidBrush( RGB( 84, 84, 84 ) );
		//FillRect( hdc, &rectClient );
		FillSolidRect( hdc, &rectClient, RGB( 84, 84, 84 ) );
	}

	SelectObject( hdc, GetStockObject( NULL_BRUSH ) );

	SelectObject( hdc, hDarkPen );
	Rectangle( hdc, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom );
	Rectangle( hdc, rectClient.left, rectClient.top, rectClient.right-1, rectClient.bottom );

	pts[0].x = rectClient.left + 1;
	pts[0].y = rectClient.top;

	pts[1].x = rectClient.right - 2;
	pts[1].y = rectClient.top;

	pts[2].x = rectClient.right - 2;
	pts[2].y = rectClient.top + 1;

	pts[3].x = rectClient.left;
	pts[3].y = rectClient.top + 1;

	if( state & BST_FOCUS )
	{
		SelectObject( hdc, GetStockObject(WHITE_PEN) );
	}
	else
		SelectObject( hdc, hGrayPen );
	Polyline( hdc, pts, 4 );


	pts[0].x = rectClient.left + 1;
	pts[0].y = rectClient.bottom - 3;

	pts[1].x = rectClient.right - 2;
	pts[1].y = rectClient.bottom - 3;

	pts[2].x = rectClient.right - 2;
	pts[2].y = rectClient.bottom - 2;

	pts[3].x = rectClient.left;
	pts[3].y = rectClient.bottom - 2;

	SelectObject( hdc, hBlackPen );
	Polyline( hdc, pts, 4 );


    SetBkMode( hdc, TRANSPARENT );	// 背景用透明模式
    if( state & (BST_PUSHED|BST_CHECKED) )  // highlight state( sunken or checked )
    {	//选中状态
		SetTextColor( hdc, pAttrib->cl_SelectedText );
	}
	else
	{  //通常状态
		SetTextColor( hdc, pAttrib->cl_Text );
	}

	// 缩小边框一个点宽
	InflateRect( &rectClient, -1, -1 );  //矩形内缩一个点
	// 画文本
	DrawText( hdc, buf, l, &rectClient, DT_VCENTER | DT_SINGLELINE | DT_CENTER );


	DeleteObject( hBlackPen );
	DeleteObject( hGrayPen );
	DeleteObject( hDarkPen );

	if( buf && buf != strBuf )
		free( buf );	//释放之前分配的缓冲
}
/*
static void DrawButtonState( HDC hdc, HWND hWnd, DWORD style, DWORD state )
{
	
	WORD type = GETTYPE( style );
	RECT rectClient, rectText, rectIcon;
	HBRUSH hBrush;
    PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 ); // 得到按钮属性结构指针
	BOOL bEnable = IsWindowEnabled( hWnd ); // 
	char strBuf[128];
	char * buf = NULL;
	int l, shift = 0;
	WORD textStyle = 0;

	GetClientRect( hWnd, &rectClient );  // 得到窗口客户矩形
	l = GetWindowTextLength( hWnd );	// 得到窗口文本长度
	if( l < sizeof( strBuf ) )
	{		
	    l = GetWindowText( hWnd, strBuf, sizeof( strBuf ) );	// 得到窗口文本
		buf = strBuf;
	}
	else
	{	// 窗口文本太大，动态分配一个
		l = (l + 1 )* sizeof( char ); 
		buf = malloc( l );
		if( buf )
		{
			GetWindowText( hWnd, buf, l );
		}
		else
			return;
	}

	textStyle = DT_SINGLELINE | DT_VCENTER;
	// 用背景色填充
	if( bEnable )
	    FillSolidRect( hdc, &rectClient, pAttrib->cl_TextBk );  // clear background
	else
		FillSolidRect( hdc, &rectClient, pAttrib->cl_DisableBk );  // clear background
	//	选择空刷子
	hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
    GetDrawRect( hdc, &rectClient, &rectText, &rectIcon, style );
	
    if( type == BS_DEFPUSHBUTTON ||
        type == BS_PUSHBUTTON ||
        (style & BS_PUSHLIKE) )
    {	// 按钮
		textStyle |= DT_CENTER;
		// 画边界矩形
        if( type == BS_DEFPUSHBUTTON )
        {	
            Rectangle( hdc, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom );
        }
        if( state & (BST_PUSHED|BST_CHECKED) )  // highlight state( sunken or checked )
        {	// 
            DrawEdge( hdc, &rectClient, BDR_SUNKENOUTER, BF_RECT );
            shift = 1;
        }
        else  // normal state
            DrawEdge( hdc, &rectClient, BDR_RAISEDOUTER, BF_RECT );
    }
	else if( type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON )
		DrawRadioBox( hdc, &rectIcon, state );	// 单选按钮
	else if( type == BS_CHECKBOX || type == BS_AUTOCHECKBOX )
		DrawCheckBox( hdc, &rectIcon, state );	// 复选按钮
    else if( type == BS_GROUPBOX )
    {	// 组框
		textStyle |= DT_CENTER;
        rectClient.top += (rectText.bottom - rectText.top) / 2;  
		// 画边框
        DrawEdge( hdc, &rectClient, BDR_SUNKENOUTER, BF_RECT );	//
        // 缩小边框一个点宽
		InflateRect( &rectClient, -1, -1 );
		// 画边框
        DrawEdge( hdc, &rectClient, BDR_RAISEDOUTER, BF_RECT );	//
        // 缩小边框一个点宽
        InflateRect( &rectClient, -1, -1 );        
    }

    if( (state & BST_FOCUS) && type != BS_GROUPBOX )
    {   // 按钮有焦点 has focus
	    DrawFocusRect( hdc, &rectText );
		//InflateRect( &rectText, -1, -1 ); // 2003-06-19, DEL, 这行将使文本产生抖动
	}
   	if( shift )  // move down state
	{	// 使文本产生向下移动的视觉感
        rectText.left += 1;
        rectText.top += 1;
	}
    // draw text or image
    if( style & (BS_BITMAP | BS_ICON) )
    {	// 图象位图按钮
        HANDLE hSave, hImage = pAttrib->hImage;
        if( hImage )
        {
            if( style & BS_BITMAP )
            {   // 位图 BITMAP
                HDC hMemDC = CreateCompatibleDC( hdc ); // 创建内存DC
                hSave = SelectObject( hMemDC, hImage );  // 为内存DC选择输出面
                // 贴图
				BitBlt(
                    hdc,
                    rectText.left,
                    rectText.top,
                    rectText.right - rectText.left,
                    rectText.bottom - rectText.top,
                    hMemDC,
                    0,
                    0,
                    SRCCOPY
                    ); 
				// 恢复
                hSave = SelectObject( hMemDC, hSave );
                DeleteDC( hMemDC );
            }
            else
            {  // 图标 ICON
                DrawIcon( hdc, rectText.left, rectText.top, hImage );
            }
        }
    }
    else
    {	// 文本按钮
		// 检查对齐模式并设置文本输出格式
		if( (style & BS_CENTER) == BS_CENTER )
		{
			textStyle |= DT_CENTER; // 居中
		}
		else
		{
			if( style & BS_LEFT )
			{
				textStyle &= ~DT_CENTER;
				textStyle |= DT_LEFT;  // 左对齐
			}			
			else if( style & BS_RIGHT )
			{
				textStyle &= ~DT_CENTER;
				textStyle |= DT_RIGHT;	// 右对齐
			}
		}			
		if( (style & BS_VCENTER) == BS_VCENTER )
		{
			textStyle |= DT_VCENTER;	// 垂直对齐
		}
		else
		{
			if( style & BS_TOP )
			{
				textStyle &= ~DT_VCENTER;
				textStyle |= DT_TOP;	// 上对齐
			}
			else if( style & BS_BOTTOM )
			{
				textStyle &= ~DT_VCENTER;
				textStyle |= DT_BOTTOM;	// 底对齐
			}
		}

        if( (style & BS_MULTILINE) )
            textStyle &= ~DT_SINGLELINE; // 多行文本
        // 画文本 now draw text
        if( type == BS_GROUPBOX )
        {
			;
		}
        else
            SetBkMode( hdc, TRANSPARENT );	// 背景用透明模式
        if( bEnable )
        {	// 启用模式 enable
			SetTextColor( hdc, pAttrib->cl_Text );
        }
        else
        {   // 停用模式 disable
            //SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );
			SetTextColor( hdc, pAttrib->cl_Disable );
        }
		if( (style & BS_MULTILINE) && (style & BS_VCENTER) )
		{   // 
			RECT rc = rectText;
			int cyOrg, cyNew;
			DrawText( hdc, buf, l, &rc, textStyle | DT_CALCRECT );
			cyOrg = rectText.bottom - rectText.top;
			cyNew = rc.bottom - rc.top;
			if( cyNew < cyOrg )
				rectText.top += (cyOrg - cyNew) / 2;			
		}
		// 画文本
        DrawText( hdc, buf, l, &rectText, textStyle  );
    }
	if( buf && buf != strBuf )
		free( buf );	//释放之前分配的缓冲

	SelectObject( hdc, hBrush );	//恢复
}
*/

/*
static void DrawButtonState( HDC hdc, HWND hWnd, DWORD style, DWORD state )
{
	WORD type = GETTYPE( style );
	RECT rectClient, rectText, rectIcon;
	HBRUSH hBrush;
	char buf[20];
	int l, shift = 0;
	WORD textStyle = 0;

	GetClientRect( hWnd, &rectClient );
	l = GetWindowText( hWnd, buf, sizeof( buf ) );

	textStyle = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
	//hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
    FillRect( hdc, &rectClient, GetStockObject( LTGRAY_BRUSH ) );  // clear background
	hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
	if( type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON ||
		type == BS_CHECKBOX || type == BS_AUTOCHECKBOX )
	{
		rectText = rectClient;
		rectIcon = rectClient;
		if( style & BS_LEFTTEXT )
		{
		    rectIcon.left = rectClient.right - ICON_SIZE;
			rectIcon.right = rectClient.right;
			rectText.right = rectIcon.left - 2;
		}
		else
		{
		    rectIcon.left = rectClient.left;
			rectIcon.right = rectIcon.left + ICON_SIZE;
			rectText.left = rectIcon.right + 2;
		}
		rectIcon.top = (rectClient.bottom + rectClient.top) / 2 - ICON_SIZE / 2;
		rectIcon.bottom = rectIcon.top + ICON_SIZE;
		if( type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON )
			DrawRadioBox( hdc, &rectIcon, state );
		else
			DrawCheckBox( hdc, &rectIcon, state );
	}
	else
	{    
		if( type == BS_DEFPUSHBUTTON ||
			type == BS_PUSHBUTTON ||
			(style & BS_PUSHLIKE) )
	    {
			if( type == BS_DEFPUSHBUTTON )
			{
				Rectangle( hdc, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom );
				InflateRect( &rectClient, -1, -1 );
			}
    		if( state & BST_PUSHED )  // highlight state
            {
				DrawEdge( hdc, &rectClient, BDR_SUNKENOUTER, BF_RECT | BF_MIDDLE );
                shift = 1;
            }
			else  // normal state
				DrawEdge( hdc, &rectClient, BDR_RAISEDOUTER, BF_RECT | BF_MIDDLE );
			InflateRect( &rectClient, -1, -1 );
            
		}
		rectText = rectClient;

	}
    if( state & BST_FOCUS )
    {   // has focus
	    DrawFocusRect( hdc, &rectText );
		InflateRect( &rectText, -1, -1 );
	}
   	if( shift )  // move down state
	{
        rectText.left += 1;
        rectText.top += 1;
	}

    if( style & BS_LEFT )
	{
		textStyle &= ~DT_CENTER;
		textStyle |= DT_LEFT;
	}
	else if( style & BS_RIGHT )
	{
		textStyle &= ~DT_CENTER;
		textStyle |= DT_RIGHT;
	}
	if( style & BS_TOP )
	{
		textStyle &= ~DT_VCENTER;
		textStyle |= DT_TOP;
	}
	else if( style & BS_BOTTOM )
	{
		textStyle &= ~DT_VCENTER;
		textStyle |= DT_BOTTOM;
	}
	if( (style & BS_MULTILINE) )
	    textStyle &= ~DT_SINGLELINE;
		    // now draw text
	//SetBkColor( hdc,GetSysColor(COLOR_3DFACE) ); 
	SetBkMode( hdc, TRANSPARENT );
	if( !IsWindowEnabled( hWnd ) )
	{	// disable
	    SetTextColor( hdc, CL_DARKGRAY );
	}
	else
	{   // enable
	    SetTextColor( hdc, CL_BLACK );
	}
   	DrawText( hdc, buf, l, &rectText, textStyle  );
	SelectObject( hdc, hBrush );
}
*/

// **************************************************
// 声明：void DoOwnerDraw( HDC hdc, HWND hWnd, DWORD state, UINT uiAction )
// 参数：
//  IN hdc -  显示设备句柄
//	IN hWnd - 窗口句柄
//	IN state - 按钮状态
//	IN uiAction - 当前采取的行动，包含：
//			ODA_DRAWENTIRE - 控件的整个区域需要被绘制. 
//			ODA_FOCUS - 控件失去或得到键盘输入焦点. 参数 state 标明了其是否失去或得到. 
//			ODA_SELECT - 选择状态改变. 参数 state 标明了新的状态

// 返回值：
//	无
// 功能描述：
//	用户自绘处理
// 引用: 
//	
// ************************************************

// if window's style  is BS_OWNERDRAW, send message to owner window
static void DoOwnerDraw( HDC hdc, HWND hWnd, DWORD state, UINT uiAction )
{
	DRAWITEMSTRUCT drawItem;

	// 得到控件的客户区
	GetClientRect( hWnd, &drawItem.rcItem );
	// 初始化数据
	drawItem.CtlType = ODT_BUTTON;
	drawItem.CtlID = (WORD)GetWindowLong( hWnd, GWL_ID );
	drawItem.itemID = 0;
	drawItem.itemAction = uiAction;
	drawItem.hwndItem = hWnd;
	drawItem.hDC = hdc; 
	drawItem.itemData = 0;
	drawItem.itemState = 0;
	// 确定当前的状态
	if( state & ( BST_CHECKED | BST_PUSHED ) )
		drawItem.itemState |= ODS_SELECTED;
	if( state & BST_FOCUS )
		drawItem.itemState |= ODS_FOCUS;
	if( !IsWindowEnabled( hWnd ) )
		drawItem.itemState |= ODS_DISABLED;
	// 发送消息给父窗口
	SendMessage( GetParent(hWnd), WM_DRAWITEM, GetWindowLong( hWnd, GWL_ID ), (LPARAM)&drawItem );
}

// **************************************************
// 声明：static void RefreshButton( HWND hWnd, PBUTTON_ATTRIB pAttrib )
// 参数：
//	IN hWnd - 窗口句柄
//	IN pAttrib - 控件属性结构
// 返回值：
//	无
// 功能描述：
//	刷新控件的显示面
// 引用: 
//	
// ************************************************

static void RefreshButton( HWND hWnd, PBUTTON_ATTRIB pAttrib )
{
	HDC hdc = GetDC( hWnd );  // 向系统申请显示DC
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE ); // 得到窗口风格

	if( pAttrib == NULL )
		pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );  // 从窗口私有数据区得到控件属性

	switch( dwStyle & 0xf )
	{
	case BS_OWNERDRAW:		// 自绘风格
		DoOwnerDraw( hdc, hWnd, pAttrib->btState, ODA_DRAWENTIRE );
		break;
	default:
		DrawButtonState( hdc, hWnd, dwStyle, pAttrib->btState );
	}

	ReleaseDC( hWnd, hdc );  // 释放DC
}

// **************************************************
// 声明：static LRESULT DoPAINT( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	处理WM_PAINT消息
// 引用: 
//	
// ************************************************

static LRESULT DoPAINT( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT ps;
	DWORD state;
	DWORD dwStyle;
	DWORD s;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

	// get draw dc
	hdc = BeginPaint( hWnd, &ps );// 向系统申请显示DC
	// get myself state from extra window bytes, see RegisterMyButton
	state = pAttrib->btState;//
	// get window style
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );	// 窗口风格

	s = dwStyle & 0xf;
	switch( s )
	{
	case BS_OWNERDRAW:		// 自绘风格
		DoOwnerDraw( hdc, hWnd, state, ODA_DRAWENTIRE );
		break;
	default:
		DrawButtonState( hdc, hWnd, dwStyle, state );
	}
    EndPaint( hWnd, &ps );	// 释放DC
	return 0;
}

// **************************************************
// 声明：static LRESULT DoLBUTTONDOWN( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_LBUTTONDOWN 消息
// 引用: 
//	
// ************************************************

static LRESULT DoLBUTTONDOWN( HWND hWnd )
{
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性
    //DWORD dwStyle;
	if( (pAttrib->btFlag & BTF_DISABLEFOCUS) == 0 )	
	{
		if( GetFocus() != hWnd )
			SetFocus( hWnd );	// 设置本窗口为输入焦点
	}
	
	// highlight state if possible
	//2004-05-20, not use remove lilin
    //dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//

    // if possible, set button state
    DoSETSTATE( hWnd, TRUE );	// 画高亮状态
	// capture mouse
	SetCapture(hWnd);	// 抓住点输入
	return 0;
}

// **************************************************
// 声明：static LRESULT DoMouseMove( HWND hWnd, short x, short y )
// 参数：
//	IN hWnd - 窗口句柄
//	IN x - 当前点输入设备在控件的客户区x坐标 
//	IN y - 当前点输入设备在控件的客户区y坐标
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_MOUSEMOVE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoMouseMove( HWND hWnd, short x, short y )
{
    RECT rect;
    DWORD state;
    int type;
    POINT pt;
    PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

    type = GETTYPE( GetWindowLong( hWnd, GWL_STYLE ) );    // 得到控件类型

    pt.x = x;
    pt.y = y;

    if( GetCapture() == hWnd )	// 之前是否已经抓住点输入设备 ？
    {	// 是
        GetClientRect( hWnd, &rect );	// 得到控件客户区矩形

        state = pAttrib->btState;

        if( PtInRect( &rect, pt ) )	// 点输入设备在控件客户区吗 ？
        {	//在控件客户区
            if( (state & BST_PUSHED) ==  0 )	// 当前控件状态为按下状态吗 ？
                DoSETSTATE( hWnd, TRUE );	// 不是，改变状态
        }
        else
        {	// 不在控件客户区
            if( state & BST_PUSHED )	// // 当前控件状态为按下状态吗 ？
                DoSETSTATE( hWnd, FALSE );// 不是，改变状态
        }
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoMouseMove( HWND hWnd, short x, short y )
// 参数：
//	IN hWnd - 窗口句柄
//	IN x - 当前点输入设备在控件的客户区x坐标 
//	IN y - 当前点输入设备在控件的客户区y坐标
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_LBUTTONUP 消息
// 引用: 
//	
// ************************************************

//extern BOOL _Wnd_IsClass( HWND hWnd, LPCBYTE lpcName );
static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y )
{
	DWORD dwStyle;
	DWORD s, dw;
    RECT rect;
    POINT pt;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

    pt.x = x; pt.y = y;

	if( GetCapture() != hWnd )	// 之前是否已经抓住点输入设备 ？
		return 0;	// 否，返回
	// release mouse
    ReleaseCapture();	// 释放点输入设备

    s = pAttrib->btState;
	DoSETSTATE( hWnd, FALSE );  // 设置按钮为释放状态
    GetClientRect( hWnd, &rect );		//// 得到控件客户区矩形
	
    if( PtInRect( &rect, pt ) )
    {   
	    dwStyle = GetWindowLong( hWnd, GWL_STYLE );	// 得到控件风格
	    if( GETTYPE(dwStyle) == BS_AUTOCHECKBOX )	// 自动复选按钮吗 ？
	    {	// 是，反转状态
			DoSETCHECK( hWnd, (s & BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED );
	    }
        else if( GETTYPE(dwStyle) == BS_AUTORADIOBUTTON && 
                 (s & BST_CHECKED) == FALSE )	// 自动单选按钮吗 并且 当前状态为非选择状吗？
        {	// 是
            HWND hParent = GetParent( hWnd );	//得到按钮的父窗口句柄
            if( hParent )
            {	// 
                HWND hNext = GetWindow( hParent, GW_CHILD );//得到父窗口的第一个子窗口
				// 遍历父窗口的所有为自动单选按钮风格的子窗口
				// 如果其中的一个的状态为选择状态，则将其设为非选择状态
                while( hNext )
                {
					TCHAR strClass[sizeof( strClassButton )];
					strClass[0] = 0;
					GetClassName( hNext, strClass, sizeof( strClassButton ) );	// 得到类名
                    if( stricmp( strClass, strClassButton ) == 0 )	// 是按钮类吗 ？
                    {  /// 是 yes , is button
                        dwStyle = GetWindowLong( hNext, GWL_STYLE );	// 得到按钮风格
                        if( GETTYPE(dwStyle) == BS_AUTORADIOBUTTON )	// 自动单选按钮吗 ? 
                        {	// 是
							PBUTTON_ATTRIB pa = (PBUTTON_ATTRIB)GetWindowLong( hNext, 0 ); // 从窗口私有数据区得到控件属性
                            s = pa->btState;
                            if( s & BST_CHECKED )
                            {	// 为选择状态
                                DoSETCHECK( hNext, BST_UNCHECKED );// 设为非选择状态
                                break;
                            }
                        }
                    }
                    hNext = GetWindow( hNext, GW_HWNDNEXT );	// 下一个窗口
                }
            }
            DoSETCHECK( hWnd, BST_CHECKED );// 将当前按钮设为选择状态
        }

	    dw = GetWindowLong( hWnd, GWL_ID );
		SendNotify( GetParent(hWnd), dw, BN_CLICKED, hWnd );	// 想父发送通知消息
    }

	return 0;
}

// **************************************************
// 声明：static LRESULT DoSETFOCUS( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_SETFOCUS 消息
// 引用: 
//	
// ************************************************
static LRESULT DoSETFOCUS( HWND hWnd )
{
	DWORD s, dwStyle;
//    int type;
	HDC hdc;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

	if( pAttrib->btFlag & BTF_DISABLEFOCUS )
		return 0;

	// get current state at extra window bytes
	s = pAttrib->btState;//GetWindowLong( hWnd, OFFSET_STATE );
	s |= BST_FOCUS;  // set focus flag
    // set new state at extra window bytes
	pAttrib->btState = (WORD)s;

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );	// 得到窗口风格	

    if( GETTYPE( dwStyle ) != BS_GROUPBOX )	// 组框 ？
    {   // 否 get draw dc
        hdc = GetDC( hWnd );	// 从系统得到显示DC
        if( GETTYPE( dwStyle ) ==  BS_OWNERDRAW )	// 自绘
            DoOwnerDraw( hdc, hWnd, s, ODA_FOCUS );	// 
        else
        {            
            //RECT rect, textRect, iconRect;
            //GetClientRect( hWnd, &rect );	// 得到客户区
            //GetDrawRect( hdc, &rect, &textRect, &iconRect, dwStyle );        
            //DrawFocusRect( hdc, &textRect );	// 画焦点
			DrawButtonState( hdc, hWnd, dwStyle, (WORD)s );
        }
        ReleaseDC( hWnd, hdc );	// 释放DC
    }

	if( dwStyle & BS_NOTIFY )	// 假如可能， 向父发通知消息
	    SendNotify( GetParent(hWnd), GetWindowLong(hWnd, GWL_ID), BN_SETFOCUS, hWnd );
	return 0;
}

// **************************************************
// 声明：static LRESULT DoKILLFOCUS( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_KILLFOCUS 消息
// 引用: 
//	
// ************************************************

static LRESULT DoKILLFOCUS( HWND hWnd )
{
	DWORD s, dwStyle;
	HDC hdc;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性


	// get current state
	s = pAttrib->btState;
	s &= ~BST_FOCUS;
	// set new state to extra window bytes
	pAttrib->btState = (WORD)s;

    dwStyle = GetWindowLong( hWnd, GWL_STYLE ); // 得到窗口风格

	
    if( GETTYPE( dwStyle ) != BS_GROUPBOX )	// 组框 ？
    {
        // get draw dc
        hdc = GetDC( hWnd );// 从系统得到显示DC
        // get window style
        if( GETTYPE( dwStyle ) ==  BS_OWNERDRAW )	// 自绘
            DoOwnerDraw( hdc, hWnd, s, ODA_FOCUS );
        else
        {
            //RECT rect, textRect, iconRect;
            
            //GetClientRect( hWnd, &rect );// 得到客户区
            //GetDrawRect( hdc, &rect, &textRect, &iconRect, dwStyle );
            //DrawFocusRect( hdc, &textRect );
			DrawButtonState( hdc, hWnd, dwStyle, (WORD)s );
        }
        ReleaseDC( hWnd, hdc );	// 释放DC
    }
	
	if( dwStyle & BS_NOTIFY )	// 假如可能， 向父发通知消息
	    SendNotify( GetParent(hWnd), GetWindowLong(hWnd, GWL_ID), BN_KILLFOCUS, hWnd );

	return 0;
 }

// **************************************************
// 声明：static LRESULT DoSETSTATE( HWND hWnd, WARAM wParam )
// 参数：
//	IN hWnd - 窗口句柄
//	IN wParam - 新的状态. 假如为TRUE,设置为选择状态；否则为非选择状态
// 返回值：
//	返回0
// 功能描述：
//	设置按钮状态，也是 BM_SETSTATE 消息的处理函数
// 引用: 
//	
// ************************************************

static LRESULT DoSETSTATE( HWND hWnd, WPARAM wParam )
{
	HDC hdc;
	DWORD dwStyle;
	DWORD state;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );// 得到窗口风格

    {
        // get current state
        state = pAttrib->btState;
        if( wParam )
            state |= BST_PUSHED;	//按下状态
        else
            state &= ~BST_PUSHED;
        // set new state
		pAttrib->btState = (WORD)state;
        // get draw dc
        hdc = GetDC( hWnd );		// 从系统得到显示DC

        if( GETTYPE( dwStyle ) ==  BS_OWNERDRAW )	// 自绘
            DoOwnerDraw( hdc, hWnd, state, ODA_SELECT );
        else
            DrawButtonState( hdc, hWnd, dwStyle, state );
        
        ReleaseDC( hWnd, hdc );// 释放DC
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoSETCHECK( HWND hWnd, WARAM wParam )
// 参数：
//	IN hWnd - 窗口句柄
//	IN wParam - 新的状态. 假如为TRUE,设置为选择状态；否则为非选择状态
// 返回值：
//	返回0
// 功能描述：
//	设置按钮状态，也是 BM_SETCHECK 消息的处理函数
// 引用: 
//	
// ************************************************

static LRESULT DoSETCHECK( HWND hWnd, WPARAM wParam )
{
	HDC hdc;
	DWORD dwStyle;
	DWORD state;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

	// get current state
	state = pAttrib->btState;

    if( wParam & BST_CHECKED )
        state |= BST_CHECKED;//选择状态
    else
	    state &= ~BST_CHECKED;
	// set new state
	pAttrib->btState = (WORD)state;

	// get draw dc
	hdc = GetDC( hWnd );// 从系统得到显示DC
	// get window style
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );// 得到窗口风格

	if( GETTYPE( dwStyle ) ==  BS_OWNERDRAW )	// 自绘
		DoOwnerDraw( hdc, hWnd, state, ODA_SELECT );
	else
		DrawButtonState( hdc, hWnd, dwStyle, state );

	ReleaseDC( hWnd, hdc );// 释放DC
	return 0;
}

// **************************************************
// 声明：static VOID _SetButtonColor( PBUTTON_ATTRIB pAttrib )
// 参数：
//	IN pAttrib - 按钮类数据结构
// 返回值：
//	无
// 功能描述：
//	设置窗口显示颜色
// 引用: 
//	
// ************************************************

static VOID _SetButtonColor( PBUTTON_ATTRIB pAttrib )
{
	pAttrib->cl_Text = GetSysColor( COLOR_BTNTEXT );
	pAttrib->cl_TextBk = GetSysColor( COLOR_BTNFACE );
	
	pAttrib->cl_SelectedText = GetSysColor( COLOR_BTNTEXT );
	pAttrib->cl_SelectedTextBk = GetSysColor( COLOR_BTNFACE );

	pAttrib->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
	pAttrib->cl_DisableBk = GetSysColor( COLOR_BTNFACE );
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
{
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)malloc( sizeof(BUTTON_ATTRIB) );	// 分配控件属性结构
	if( pAttrib )
	{	//初始化
		memset( pAttrib, 0, sizeof(BUTTON_ATTRIB) );
		//pAttrib->cl_Text = GetSysColor( COLOR_BTNTEXT );
		//pAttrib->cl_TextBk = GetSysColor( COLOR_BTNFACE );
		//pAttrib->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
		//pAttrib->cl_DisableBk = GetSysColor( COLOR_BTNFACE );
		_SetButtonColor( pAttrib );

		SetWindowLong( hWnd, 0, (LONG)pAttrib );// 设置控件属性指针到窗口私有数据区
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
{
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性
	free( pAttrib );	//释放
	return 0;
}

// **************************************************
// 声明：static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpccs - 控件结构，包含颜色值
// 返回值：
//	成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	设置控件颜色
// 引用: 
//	
// ************************************************

static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
		PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

		if( lpccs->fMask & CLF_TEXTCOLOR )	// 文本
			pAttrib->cl_Text = lpccs->cl_Text;
		if( lpccs->fMask & CLF_TEXTBKCOLOR )	// 文本背景
			pAttrib->cl_TextBk = lpccs->cl_TextBk;
		if( lpccs->fMask & CLF_DISABLECOLOR )	// 无效文本
			pAttrib->cl_Disable = lpccs->cl_Disable;
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )	// 无效文本背景
			pAttrib->cl_DisableBk = lpccs->cl_DisableBk;
        RefreshButton( hWnd, pAttrib );	// 重绘
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
//	得到控件颜色
// 引用: 
//	
// ************************************************

static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
		PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

		if( lpccs->fMask & CLF_TEXTCOLOR )	// 文本
			lpccs->cl_Text = pAttrib->cl_Text;
		if( lpccs->fMask & CLF_TEXTBKCOLOR )	// 文本背景
			lpccs->cl_TextBk = pAttrib->cl_TextBk;
		if( lpccs->fMask & CLF_DISABLECOLOR )	// 无效文本
			lpccs->cl_Disable = pAttrib->cl_Disable;
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )	// 无效文本背景
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
    PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性

	_SetButtonColor( pAttrib );
	return 0;
}

// 设置焦点
static LRESULT DoEnableFocus( HWND hWnd, BOOL bEnable )
{
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// 从窗口私有数据区得到控件属性
	if( bEnable )
	{	//enable
		pAttrib->btFlag &= ~BTF_DISABLEFOCUS;
	}
	else
	{	//disbale
		pAttrib->btFlag |= BTF_DISABLEFOCUS;
	}
	return 0;
}

// 处理 WM_KEYDOWN 消息
static int DoKeyDown( HWND hWnd,     //
			    WPARAM wParam,
				LPARAM lParam )
{
	if( wParam == VK_RETURN )
	{   //回车键
		// if possible, set button state
		DoSETSTATE( hWnd, TRUE );	// 画高亮状态
		SendNotify( GetParent(hWnd), GetWindowLong( hWnd, GWL_ID ), BN_CLICKED, hWnd );	// 向父发送通知消息
	}
	return 0;
}

// **************************************************
// 声明：LRESULT WINAPI ButtonWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
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
LRESULT WINAPI ButtonWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	DWORD s;
	PBUTTON_ATTRIB pAttrib;
	switch( message )
	{
	case WM_PAINT:			// 系统绘制消息
		return DoPAINT( hWnd );
	case WM_LBUTTONDBLCLK:			// 左鼠标双击消息
	case WM_LBUTTONDOWN:			// 左鼠标按下消息
		return DoLBUTTONDOWN( hWnd );
    case WM_MOUSEMOVE:				// 鼠标移动消息
        if( wParam & MK_LBUTTON )
            return DoMouseMove( hWnd, LOWORD( lParam ), HIWORD( lParam ) );
         else
             break;
	case WM_LBUTTONUP:				// 左鼠标释放消息
		return DoLBUTTONUP( hWnd, LOWORD( lParam ), HIWORD( lParam ) );
	case WM_SETFOCUS:				// 设置焦点
		return DoSETFOCUS( hWnd );
	case WM_KILLFOCUS:				// 释放焦点
		return DoKILLFOCUS( hWnd );
	case BM_GETCHECK:					// 得到选择状态
		s = GetWindowLong( hWnd, GWL_STYLE );
		s &= 0x0f;
		if( s == BS_RADIOBUTTON ||
			s == BS_CHECKBOX || 
			s == BS_AUTOCHECKBOX ||
			s == BS_AUTORADIOBUTTON ||
			s == BS_AUTO3STATE ||
			s == BS_3STATE )
		{
			pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );
			return pAttrib->btState & 0x03;//return (GetWindowLong( hWnd, OFFSET_STATE ) & 0x03 );
		}
		// if this is pushbutton
		return 0;
	case BM_SETCHECK:				// 设置选择状态
		return DoSETCHECK( hWnd, wParam );
	case BM_GETSTATE:				
		pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );
		return pAttrib->btState;
	case BM_SETSTATE:
		return DoSETSTATE( hWnd, wParam );
	case BM_SETSTYLE:				// 设置风格
		s = GetWindowLong( hWnd, GWL_STYLE );
		s &= 0xffff0000l;
		s |= LOWORD( wParam );
		SetWindowLong( hWnd, GWL_STYLE, s );
		if( lParam )
		{  // to redraw button, I invalidate entire client
			//InvalidateRect( hWnd, 0, TRUE );
			RefreshButton( hWnd, NULL );
		}
		return 0;
    case BM_SETIMAGE:			// 设置显示图片
		pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );
        s = (DWORD)pAttrib->hImage;
        pAttrib->hImage = (HANDLE)lParam;
		RefreshButton( hWnd, NULL );	//重绘
        return s;     
    case BM_GETIMAGE:
		pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );
        return (DWORD)pAttrib->hImage;
	case WM_SETTEXT:
		s = DefWindowProc(hWnd,message,wParam,lParam);
		RefreshButton( hWnd, NULL );	//重绘
		return s;
	case WM_ENABLE:
		RefreshButton( hWnd, NULL );	//重绘
		return 0;
	case WM_KEYDOWN:
		return DoKeyDown( hWnd, wParam, lParam );
    case WM_SETCTLCOLOR:		// 设置显示颜色
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:
		return DoGetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
	case BM_ENABLEFOCUS:		//关闭或打开按钮的焦点功能
		return DoEnableFocus( hWnd, wParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hWnd );
    case WM_CREATE:				// 窗口创建初始化
		return DoCreate( hWnd );
    case WM_DESTROY:			// 破坏
		return DoDestroy( hWnd );

	}
	// It's important to do this
	// if your do'nt handle message, you must call DefWindowProc
	// 重要提示：假如你不需要处理消息，将该消息交由系统窗口默认处理函数去处理
	// 
	return DefWindowProc(hWnd,message,wParam,lParam);
}
