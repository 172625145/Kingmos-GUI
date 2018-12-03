/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：gwme系统设置
版本号：3.0.0
开发时期：1999  
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <gwmeobj.h>

// like GetSystemMetrics
#define CAPTION_HEIGHT 20
#define UNDEF_VALUE -1
#define SYS_INDEX_NUM  ( sizeof(iSysSettings) / sizeof(int) )

//默认的系统度量设置
static int iSysSettings[] = {
                   UNDEF_VALUE,//#define SM_CXSCREEN             0
                   UNDEF_VALUE,//#define SM_CYSCREEN             1
                   13,//#define SM_CXVSCROLL            2
                   13,//#define SM_CYHSCROLL            3
                   CAPTION_HEIGHT,//#define SM_CYCAPTION            4
                   1,//#define SM_CXBORDER             5
                   1,//#define SM_CYBORDER             6
                   2,//#define SM_CXDLGFRAME           7
                   2,//#define SM_CYDLGFRAME           8
                   2,//#define SM_CXFRAME              9
                   2,//#define SM_CYFRAME              10
                   32,//#define SM_CXICON               11
                   32,//#define SM_CYICON               12
                   32,//#define SM_CXCURSOR             13
                   32,//#define SM_CYCURSOR             14
                   18,//#define SM_CYMENU               15
                   UNDEF_VALUE,//#define SM_CXFULLSCREEN         16
                   UNDEF_VALUE,//#define SM_CYFULLSCREEN         17
                   13,//#define SM_CYVSCROLL            18
                   13,//#define SM_CXHSCROLL            19
                   0,//#define SM_DEBUG                20
                   40,//#define SM_CXMIN                21
                   40,//#define SM_CYMIN                22
                   17,//#define SM_CXSIZE               23
                   CAPTION_HEIGHT,//#define SM_CYSIZE               24
                   40,//#define SM_CXMINTRACK           25
                   40,//#define SM_CYMINTRACK           26
                   32,//#define SM_CXICONSPACING        27
                   32,//#define SM_CYICONSPACING        28
                   FALSE,//#define SM_PENWINDOWS           29
                   UNDEF_VALUE,//#define SM_CXMAXTRACK           30
                   UNDEF_VALUE,//#define SM_CYMAXTRACK           31
                   UNDEF_VALUE,//#define SM_CXMAXIMIZED          32
                   UNDEF_VALUE,//#define SM_CYMAXIMIZED          33
                   FALSE,//#define SM_NETWORK              34
                   16,//#define SM_CXSMICON             35
                   16,//#define SM_CYSMICON             36
                   13,//#define SM_CYVTHUMB             37
                   13,//#define SM_CXHTHUMB             38
                   UNDEF_VALUE,//#define SM_CXTOUCH              39
                   UNDEF_VALUE,//#define SM_CYTOUCH              40
                   UNDEF_VALUE,//#define SM_XTOUCHOFF            41
                   UNDEF_VALUE,//#define SM_YTOUCHOFF            42
//#ifdef ARM_CPU
  //                 (800 - 240) / 2,//0,//#define SM_XVIEW                43
    //               (600 - 320) / 2 - 35,//0,//#define SM_YVIEW                44
//#else
				   0,
				   0,
				   0,		//SM_CXSYSFONT
				   0,		//SM_CYSYSFONT
//#endif
				   0,	//SM_XCENTER              47		//对话框居中 X
				   0,	//SM_YCENTER              48		//对话框居中 Y
				   0,	//SM_CXCENTER             49		//对话框居中 CX
				   0,   //SM_CYCENTER             50		//对话框居中 CY					
				   0,	//SM_CYLISTITEM           51		//定义List item默认高度
//				   0,   //SM_BITS_PERPEL          52        //系统默认的象素点格式
};

#define CONTROL_FACE RGB( 0xf0, 0xf0, 0xf0 )
//默认的系统颜色设置
static DWORD dwSysColor[SYS_COLOR_NUM] = {
//  COLOR_3DDKSHADOW            0
//  Dark shadow for three-dimensional display elements. 
//  深灰-用于三维显示的阴影部分
    RGB( 0x80, 0x80, 0x80 ),//CL_BLACK,

//  COLOR_3DFACE                1 
//  COLOR_BTNFACE               1
//  Face color for three-dimensional display elements. 
//  亮灰-用于三维显示的正面部分
    CONTROL_FACE,
    
//  COLOR_3DHILIGHT             2
//  COLOR_3DHIGHLIGHT           2
//  COLOR_BTNHILIGHT            2  
//  COLOR_BTNHIGHLIGHT          2
//  Highlight color for three-dimensional display elements 
//  (for edges facing the light source.) 
//  高亮-用于三维显示的高亮部分 
	RGB( 0xff, 0xff, 0xff ),
	
//  COLOR_3DLIGHT               3
//  Light color for three-dimensional display elements 
//  (for edges facing the light source.) 
//  亮-用于三维显示的亮部分 
    RGB( 0xf8, 0xf8, 0xf8 ),//RGB( 0xf0, 0xf0, 0xf0 ),

//  COLOR_3DSHADOW              4
//  COLOR_BTNSHADOW Shadow color for three-dimensional display elements 
//  (for edges facing away from the light source). 
//  黑-用于三维显示的黑部分 
    CL_BLACK,

//  COLOR_ACTIVEBORDER    5  
//  Active window border. 
//  活动窗口的边界颜色
	CL_BLACK,

//  COLOR_ACTIVECAPTION         6
//  Active window title bar. 
//  活动窗口的标题条颜色	
    RGB( 0xc0, 0xc0, 0xc0 ),

//  COLOR_APPWORKSPACE          7
//  Background color of multiple document interface (MDI) applications. 
//  多文档活动窗口的背景
	CL_WHITE,

//  COLOR_BACKGROUND            8
//  COLOR_DESKTOP               8
//  Desktop. 
//  桌面背景
	CL_WHITE,

//  COLOR_BTNTEXT               9
//  Text on push buttons. 
//  按钮文本
    CL_BLACK,

//  COLOR_CAPTIONTEXT           10
//  Text in caption, size box, and scroll bar arrow box. 
//  标题条文本颜色
	CL_BLACK,

//  COLOR_GRAYTEXT              11
//  Grayed (disabled) text. This color is set to 0 
//  if the current display driver does not support a solid gray color. 
//  灰文本 或 失效文字	
    CL_DARKGRAY,

//  COLOR_HIGHLIGHT             12 
//  Item(s) selected in a control. 
//  高亮-在控件里被选择的项目背景
    CL_BLUE,
	
//  COLOR_HIGHLIGHTTEXT         13
//  Text of item(s) selected in a control. 
//  高亮-在控件里被选择的项目的文本    
    CL_YELLOW,

//  COLOR_INACTIVEBORDER        14
//  Inactive window border. 
//  非活动边界
    CL_BLACK,

//  COLOR_INACTIVECAPTION       15
//  Inactive window caption. 
//  非活动标题栏
	RGB( 0xe0, 0xe0, 0xe0 ),

//  COLOR_INACTIVECAPTIONTEXT   16
//  Color of text in an inactive caption. 
//  非活动标题栏的文本
    CL_DARKGRAY, 

//  COLOR_INFOBK                17
//  Background color for ToolTip controls. 
//      
    CL_YELLOW,

//  COLOR_INFOTEXT              18
//  Text color for ToolTip controls. 
//      
    CL_BLACK,

//  COLOR_MENU                  19 
//  Menu background. 
//  菜单背景    
    CL_WHITE,

//  COLOR_MENUTEXT              20
//  Text in menus. 
//  菜单文本
    CL_BLACK,

//  COLOR_SCROLLBAR             21
//  Scroll bar gray area. 
//  滚动条
    RGB( 0x00, 0x66, 0xff ),

//  COLOR_STATIC                22
//  Background color for static controls and dialogs. 
//  静态控件或对话框的背景
    CONTROL_FACE,

//  COLOR_STATICTEXT            23
//  Text color for static controls
//  静态控件文本
    CL_BLACK,

//  COLOR_WINDOW                24
//  Window background. 
//  窗口背景    
    CL_WHITE,

//  COLOR_WINDOWFRAME           25
//  Window frame. 
//  窗口框架    
    CL_BLACK,
	
//  COLOR_WINDOWTEXT            26
//  Text in windows.
//  窗口文本
    CL_BLACK,
	//scrollbar thumb
	CONTROL_FACE			
};

//默认的系统刷子
static HBRUSH hSysColorBrush[SYS_COLOR_NUM];


// **************************************************
// 声明：int WINAPI WinSys_SetMetrics( int nIndex, int iNewValue )
// 参数：
// 	IN nIndex - 索引值（标示那一个度量）
// 	IN iNewValue - 新的度量值
// 返回值：
//	假如成功，返回老的值；否则，返回0
// 功能描述：
//	设置度量
// 引用: 
//	系统API
// ************************************************


int WINAPI WinSys_SetMetrics( int nIndex, int iNewValue )
{
	int v = 0;
	if( (UINT)nIndex < SYS_INDEX_NUM )
	{
		v = iSysSettings[nIndex];
		iSysSettings[nIndex] = iNewValue;
	}
	else
	{
		WARNMSG( 1, ("error at WinSys_SetMetrics: invalid index value=%d.\r\n", nIndex ) );
    	SetLastError( ERROR_INVALID_PARAMETER );		
	}
	return v;
}

// **************************************************
// 声明：int WINAPI WinSys_GetMetrics( int nIndex )
// 参数：
// 	IN nIndex - 索引值（标示那一个度量）
// 返回值：
//	假如成功，返回老的值；否则，返回0
// 功能描述：
//	得到度量	
// 引用: 
//	系统API
// ************************************************

int WINAPI WinSys_GetMetrics( int nIndex )
{
	if( (UINT)nIndex < SYS_INDEX_NUM )
	{
		return iSysSettings[nIndex];
	}
	WARNMSG( 1, ("error at WinSys_GetMetrics:invalid index value=%d.\r\n", nIndex ) );
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}


// **************************************************
// 声明：DWORD WINAPI WinSys_GetColor(int nIndex)
// 参数：
// 	IN nIndex - 索引值（标示那一个颜色）
// 返回值：
//	假如成功，返回老的值；否则，返回0
// 功能描述：
//	得到系统颜色
// 引用: 
//	系统API
// ************************************************

DWORD WINAPI WinSys_GetColor( int nIndex)
{
	if( (UINT)nIndex < SYS_COLOR_NUM )
		return dwSysColor[nIndex];
	else
	{
	    WARNMSG( 1, ("error at WinSys_GetColor:invalid index value=%d.\r\n", nIndex ) );
    	SetLastError( ERROR_INVALID_PARAMETER );
	}
	return 0;
}

// **************************************************
// 声明：BOOL WINAPI WinSys_SetColors(
//				  int cElements, 
//				  const int *lpaElements, 
//                  const COLORREF *lpaRgbValues )
// 参数：
// 	IN cElements - 需要设置的颜色数量
// 	IN lpaElements - 需要设置的颜色索引组
// 	IN lpaRgbValues - COLORREF指针，需要设置的颜色值
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置新的系统颜色
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinSys_SetColors(
				  int cElements, 
				  const int *lpaElements, 
                  const COLORREF *lpaRgbValues )
{
	extern BOOL _SetSysSolidBrushColor( HBRUSH hBrush , COLORREF color );
	int i;

	for( i = 0; i < cElements; i++ )
	{
	    if( (UINT)*lpaElements  < SYS_COLOR_NUM )
		{
		    dwSysColor[*lpaElements] = *lpaRgbValues;
			_SetSysSolidBrushColor( hSysColorBrush[*lpaElements], *lpaRgbValues );
		}
		else
		{
	        WARNMSG( 1, ("error at WinSys_SetColors:invalid index(%d) value=%d.\r\n", i, *lpaElements ) );
    	    SetLastError( ERROR_INVALID_PARAMETER );
    	    break;
		}
		lpaElements++;
		lpaRgbValues++;
	}
	return i;
}

// **************************************************
// 声明：HBRUSH WINAPI WinSys_GetColorBrush( int nIndex )
// 参数：
// 	IN nIndex - 索引值（标示那一种颜色刷子）
// 返回值：
//	假如成功，返回刷子句柄；否则，返回NULL
// 功能描述：
//	得到系统刷子
// 引用: 
//	系统API
// ************************************************

HBRUSH WINAPI WinSys_GetColorBrush( int nIndex )
{
	if( (UINT)nIndex < SYS_COLOR_NUM )
		return hSysColorBrush[nIndex];
	else
	{
        WARNMSG( 1, ("error at WinSys_GetColorBrush:invalid index value=%d.\r\n", nIndex ) );
	    SetLastError( ERROR_INVALID_PARAMETER );
	}
	return NULL;
}

// **************************************************
// 声明：BOOL _InitSysSetting( void )
// 参数：
// 	无
// 返回值：
//	进入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化系统设置
// 引用: 
//	当gwme系统启动时，回调用该函数
// ************************************************

extern HBRUSH _CreateSysBrush( COLORREF color, UINT uiStyle );
extern HBRUSH _DeleteSysSolidBrush( HBRUSH hBrush );

BOOL _InitSysSetting( void )
{
	extern BOOL _GetSysFontSize( LPSIZE lpSize );
	SIZE size;
	int i;
	//初始化系统刷子
	for( i = 0; i < SYS_COLOR_NUM; i++ )
	{
		hSysColorBrush[i] = _CreateSysBrush( WinSys_GetColor( i ), BS_SOLID );
		if( hSysColorBrush[i] == NULL )
			return FALSE;
	}
	//初始化系统度量
	iSysSettings[SM_CXMAXTRACK] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYMAXTRACK] = lpDisplayBitmap->bmHeight;
	iSysSettings[SM_CXMAXIMIZED] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYMAXIMIZED] = lpDisplayBitmap->bmHeight;
	iSysSettings[SM_CXTOUCH] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYTOUCH] = lpDisplayBitmap->bmHeight;
	iSysSettings[SM_XTOUCHOFF] = 0;
    iSysSettings[SM_YTOUCHOFF] = 0;
	iSysSettings[SM_CXFULLSCREEN] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYFULLSCREEN] = lpDisplayBitmap->bmHeight;
	iSysSettings[SM_CXSCREEN] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYSCREEN] = lpDisplayBitmap->bmHeight;

	iSysSettings[SM_CXCENTER] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYCENTER] = lpDisplayBitmap->bmHeight;

	if( _GetSysFontSize( &size ) )
	{
		iSysSettings[SM_CYCAPTION] = size.cy + 4;
		iSysSettings[SM_CYSIZE] = iSysSettings[SM_CYCAPTION];
		iSysSettings[SM_CXSIZE] = size.cx * 2 + 1;
		iSysSettings[SM_CYMENU] = size.cy + 2;
		iSysSettings[SM_CXSYSFONT] = size.cx;
		iSysSettings[SM_CYSYSFONT] = size.cy;
		iSysSettings[SM_CYLISTITEM] = size.cy + 4;
		
	}

	//
	return TRUE;
}

// **************************************************
// 声明：void _DeInitSysColorBrush( void )
// 参数：
//    无
// 返回值：
//	无
// 功能描述：
//	当gwme系统退出时,回调用该函数
// 引用: 
// ************************************************

void _DeInitSysColorBrush( void )
{
	int i;
	//这部分代码可以不去做，因为gwme系统退出时也意外做操作系统需要关掉
	//因此不需要去做_DeleteSysSolidBrush的代码
	//但为了代码的完整性（分配-释放）以及不系统性能的影响微不足道，我保留该代码。
	for( i = 0; i < SYS_COLOR_NUM; i++ )
	{
		_DeleteSysSolidBrush( hSysColorBrush[i] );
	}
}

