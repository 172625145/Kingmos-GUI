/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：Caret管理
版本号：2.0.0
开发时期：1999
作者：李林
修改记录：
    2003-10-07: 增加caret.hrgnClip
******************************************************/

#include <eframe.h>
#include <ecaret.h>
#include <eassert.h>
#include <eapisrv.h>
#include <winsrv.h>
#include <gdisrv.h>

#define CCS_HIDE 0
#define CCS_SHOW 1

//定义光标光标结构
typedef struct __CARETDATA
{
    HWND hwnd;	//光标所关联的窗口
    short int x;		//当前光标在窗口中客户区的开始坐标
    short int y;
    short int width;		//当前光标在窗口中客户区的长和宽度
    short int height;
    HBITMAP hBitmap;		//如果光标是一个位图，该值有效
    short int lockCount;		//锁定计数器
    WORD iBlinkTime;			//光标闪烁频率（以毫秒为单位）
    //DWORD ticks;			//无用
    WORD state;			//状态
	WORD dump;
	HRGN hrgnClip;		//当前显示的光标图形在窗口中的裁剪区
}_CARETDATA, FAR * _LPCARETDATA;

//默认数据
static _CARETDATA caretData = { 0, 0, 0, 0, 0, 0, 0, 500, 0, 0, NULL };

static void _CaretBlink( HDC hdc, BOOL bFromPaintDC );

VOID CALLBACK AutoCaretBlink( 
  HWND hwnd, 
  UINT uMsg,
  UINT idEvent,
  DWORD dwTime  );

// **************************************************
// 声明：BOOL _InitCaret( void )
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化系统裁剪区
// 引用: 
//	GWME 系统加载初始化时调用该函数
// ************************************************

BOOL _InitCaret( void )
{
	caretData.hrgnClip = WinRgn_CreateRect( 0, 0, 0, 0 );
	return (BOOL)caretData.hrgnClip;
}

// **************************************************
// 声明：void _DeInitialCaret( void )
// 参数：
// 	无
// 返回值：
//	无
// 功能描述：
//	无 _InitCaret 相反，释放之前分配的资源
// 引用: 
//	GWME 系统退出时调用该函数
// ************************************************

void _DeInitialCaret( void )
{
	WinGdi_DeleteObject( caretData.hrgnClip );
}

// **************************************************
// 声明：static void _CaretBlink( HDC hdc, BOOL bFromPaintDC )
// 参数：
// 	IN hdc - 绘图DC 句柄
//	IN bFromPaintDC - 该 hdc是否来自于 WM_PAINT 的句柄
// 返回值：
//	无
// 功能描述：
//	不断使光标闪烁
// 引用: 
//	
// ************************************************

static void _CaretBlink( HDC hdc, BOOL bFromPaintDC )
{
	HRGN hRgn;

	if( caretData.state == CCS_SHOW && bFromPaintDC == FALSE )
	{	//当前是处于显示状态并且hdc不是来自于 BeginPaint
		//将之前的显示区域选进DC
		WinGdi_SelectClipRgn( hdc, caretData.hrgnClip );
	}
	//绘制
    WinGdi_PatBlt(hdc,
               caretData.x,
               caretData.y,
               caretData.width,
               caretData.height,
               DSTINVERT);
	//反转状态
    caretData.state = 1 - caretData.state;

	if( caretData.state == CCS_SHOW )
	{	//保存当前显示区域
		hRgn = WinRgn_CreateRect( caretData.x, caretData.y, caretData.x + caretData.width, caretData.y + caretData.height );
		WinGdi_GetClipRgn( hdc, caretData.hrgnClip );
		WinRgn_Combine( caretData.hrgnClip, caretData.hrgnClip, hRgn, RGN_AND );
		WinGdi_DeleteObject( hRgn );
	}
}


// **************************************************
// 声明：BOOL WINAPI WinCaret_Create( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight )
// 参数：
// 	IN hWnd	- 窗口句柄，标明光标的拥有者
//	IN hBitmap - 光标的显示外观，如果为NULL, 则显示为矩形（不支持，必须为NULL）
//	IN nWidth - 光标的宽度
//	IN nHeight - 光标的高度
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	为窗口创建一个光标
// 引用: 
//	系统API
// ************************************************
 
BOOL WINAPI WinCaret_Create( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight )
{
    if( caretData.hwnd )
	{	//当前光标有拥有者,释放它
        WinCaret_Destroy();
	}
	//为新拥有者初始化数据
    caretData.hwnd = hWnd;
    caretData.x = 0;
    caretData.y = 0;
    caretData.width = nWidth;
    caretData.height = nHeight;
    caretData.hBitmap = hBitmap;
    caretData.state = CCS_HIDE;
    caretData.lockCount = 1;
	
	WinRgn_SetRect( caretData.hrgnClip, 0, 0, nWidth, nHeight );
	//创建一个系统 timer
	SetSysTimer( hWnd, IDCARET, 400, NULL );//AutoCaretBlink );
    return TRUE;
}

// **************************************************
// 声明：BOOL WINAPI WinCaret_Destroy( void )
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	破坏光标
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinCaret_Destroy( void )
{
    HDC hdc;
	//破坏该光标的 timer
	KillSysTimer( caretData.hwnd, IDCARET );
	//光标是否被锁住 ？
    if( caretData.lockCount <= 0 )
    {   // 没有caret is active
		//进入可能，隐含光标
        if( caretData.state == CCS_SHOW && caretData.hwnd )
        {
            hdc = WinGdi_GetClientDC( caretData.hwnd );
            _CaretBlink( hdc, FALSE );
            WinGdi_ReleaseDC( caretData.hwnd, hdc );
        }
    }
	//释放拥有者
    caretData.hwnd = NULL;
    caretData.lockCount = 1; // lock it
	// 2003-10-07
	WinRgn_SetRect( caretData.hrgnClip, 0, 0, 0, 0 );
	//
    return TRUE;
}

// **************************************************
// 声明：UINT WINAPI WinCaret_GetBlinkTime( void )
// 参数：
// 	无
// 返回值：
//	返回当前光标的闪烁时间（以毫秒为单位）
// 功能描述：
//	得到当前光标的闪烁时间（以毫秒为单位）
// 引用: 
//	系统API
// ************************************************

UINT WINAPI WinCaret_GetBlinkTime( void )
{
    return caretData.iBlinkTime;
}

// **************************************************
// 声明：BOOL WINAPI WinCaret_GetPos( LPPOINT lpPoint )
// 参数：
// 	OUT lpPoint - POINT 结构指针
// 返回值：
//	假如成功，返回TRUE，lpPoint 保存光标当前的位置信息；否则，返回FALSE
// 功能描述：
//	得到光标当前的位置信息
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinCaret_GetPos( LPPOINT lpPoint )
{
    if( caretData.hwnd && lpPoint )
    {
        lpPoint->x = caretData.x;
        lpPoint->y = caretData.y;
        return TRUE;
    }
    else
        return FALSE;
}

// **************************************************
// 声明：BOOL _HideCaret( HWND hWnd, HDC hdc )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN hdc - 绘图DC句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	隐藏光标
// 引用: 
//	被 BeginPaint GetDC 等调用
// ************************************************

BOOL _HideCaret( HWND hWnd, HDC hdc )
{
    if( caretData.hwnd == hWnd )
    {	//有效的拥有者

        if( caretData.lockCount <= 0 )
        {   // 光标当前是活动的 caret is active
            // 假如可能，隐藏光标 noactive caret
            caretData.lockCount = 1;
            if( caretData.state == CCS_SHOW )
            {   // hide the caret				
				hdc = WinGdi_GetClientDC( hWnd );
				_CaretBlink( hdc, FALSE );
				WinGdi_ReleaseDC( hWnd, hdc );
            }
        }
        else   // caret is no active
            caretData.lockCount++;	//光标之前就被锁住。仅仅需要累加计数
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinCaret_Hide( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	隐藏光标
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinCaret_Hide( HWND hWnd )
{
    return _HideCaret( hWnd, NULL );
}

// **************************************************
// 声明：BOOL WINAPI WinCaret_SetBlinkTime( UINT uMSeconds )
// 参数：
// 	IN uMSeconds - 以毫秒为单位显示的
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置闪烁时间
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinCaret_SetBlinkTime( UINT uMSeconds )
{
    caretData.iBlinkTime = uMSeconds;
    return TRUE;
}

// **************************************************
// 声明：BOOL WINAPI WinCaret_SetPos( int x, int y )
// 参数：
// 	IN x - 光标新的在窗口里的位置
// 	IN y - 光标新的在窗口里的位置
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置窗口新的位置
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinCaret_SetPos( int x, int y )
{
    // 假如可能，先隐藏 hide caret if possible
    if( caretData.hwnd )
        WinCaret_Hide( caretData.hwnd );
    // set new position
    caretData.x = x;
    caretData.y = y;
    // show caret if possible
    if( caretData.hwnd )
        WinCaret_Show( caretData.hwnd );
    return TRUE;
}

// **************************************************
// 声明：BOOL _ShowCaret( HWND hWnd, HDC hdc )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN hdc - 绘图句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	显示光标
// 引用: 
//	
// ************************************************

BOOL _ShowCaret( HWND hWnd, HDC hdc )
{
    if( caretData.hwnd == hWnd )
    {
        if( caretData.lockCount >= 1 )
        {    // caret not been actived
            caretData.lockCount--;
        }
        else
            return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinCaret_Show( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	显示光标
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinCaret_Show( HWND hWnd )
{
	return _ShowCaret( hWnd, NULL );
}

// **************************************************
// 声明：void CALLBACK AutoCaretBlink( 
//					HWND hwnd, 
//					UINT uMsg,     // WM_SYSTIMER message
//					UINT idEvent,  // timer identifier
//					DWORD dwTime  )

// 参数：
// 	IN hWnd - 窗口句柄
//	IN uMsg - 消息
//	IN idEvent - ID
//	IN dwTime - 无用
// 返回值：
//	无
// 功能描述：
//  产生周期闪烁
// 引用: 
//	被　defproc.c 调用
// ************************************************

#define ABS( v ) ( (v) > 0 ? (v) : -(v) )
//void AutoCaretBlink( void )
void CALLBACK AutoCaretBlink( 
  HWND hwnd, 
  UINT uMsg,     // WM_SYSTIMER message
  UINT idEvent,  // timer identifier
  DWORD dwTime  )
{
    HDC hdc;

	ASSERT( idEvent == IDCARET );
	if( idEvent == IDCARET )
	{
		if( caretData.hwnd && caretData.lockCount <= 0 )
		{
			hdc = WinGdi_GetClientDC( caretData.hwnd );
			//ASSERT( hdc );
			_CaretBlink( hdc, FALSE );
			WinGdi_ReleaseDC( caretData.hwnd, hdc );
		}
	}
}
