/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：进度条窗口类
版本号：1.0.0
开发时期：
作者：
修改记录：
******************************************************/

#include <ewindows.h>
#include <edef.h>

#include <eprogres.h>

typedef struct _tag_Progress_Data{
	int			iCurrent;  // 进度条的当前位置
	int			iIncrement; // 步长
	PBRANGE		Pbr;   // 范围
	COLORREF    clBarColor;  // 前景颜色
	COLORREF    clBackGround; // 背景颜色
}_PROGRESSDATA,*_LPPROGRESSDATA;


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
static SCRLBAR_BRUSH scrlBrush;

//  创建滚动棒用的刷子
static HBRUSH CreateProgressBrush( UINT id, HBITMAP *lphBitmap )
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

static SCRLBAR_BRUSH * InitProgressBrush( ) //LPSCRLBAR_BRUSH lpscrlBrush )
{
	if( lpscrl_brush == NULL )
	{
	//if( hbmpVSBackground == NULL )
	//{ //第一次，初始化...
		scrlBrush.hbrushVSBackground = CreateProgressBrush( OBM_VSCROLLBAR_BACKGROUND, &hbmpVSBackground );
	//}
	
	//if( hbrushHSBackground == NULL )
	//{ //第一次，初始化...
		scrlBrush.hbrushHSBackground = CreateProgressBrush( OBM_HSCROLLBAR_BACKGROUND, &hbmpHSBackground );
	//}

	//if( hbrushVSThumb == NULL )
	//{ //第一次，初始化...
		scrlBrush.hbrushVSThumb = CreateProgressBrush( OBM_VPROGRESS_THUMB, &hbmpVSThumb );
	//}

	//if( hbrushHSThumb == NULL )
	//{ //第一次，初始化...
		scrlBrush.hbrushHSThumb = CreateProgressBrush( OBM_HPROGRESS_THUMB, &hbmpHSThumb );
	//}
	
	//if( hbrushCorner == NULL )
	//{
		//scrlBrush.hbrushCorner = CreateProgressBrush( OBM_SCROLLBAR_CORNER, &hbmpCorner );
	//}
	//if( hbrushUpArrow == NULL )
	//{
		scrlBrush.hbrushUpArrow = CreateProgressBrush( OBM_VSCROLLBAR_UPARROW, &hbmpUpArrow );
	//}
	//if( hbrushDownArrow == NULL )
	//{
		scrlBrush.hbrushDownArrow = CreateProgressBrush( OBM_VSCROLLBAR_DOWNARROW, &hbmpDownArrow );
	//}
	//if( hbrushLeftArrow == NULL )
	//{
		scrlBrush.hbrushLeftArrow = CreateProgressBrush( OBM_HSCROLLBAR_LEFTARROW, &hbmpLeftArrow );
	//}
	//if( hbrushRightArrow == NULL )
	//{
		scrlBrush.hbrushRightArrow = CreateProgressBrush( OBM_HSCROLLBAR_RIGHTARROW, &hbmpRightArrow );
	//}
		lpscrl_brush = &scrlBrush;
	}
		
	return lpscrl_brush;
}

static BOOL DeinitProgressBrush( )//LPSCRLBAR_BRUSH lpscrlBrush )
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
	SelectObject( hdc, hBrush );
	SetBrushOrgEx( hdc, lprc->left, lprc->top, NULL );
	BitBlt( hdc, lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top, NULL, 0, 0, PATCOPY );
}

// **************************************************
// 声明：static void PaintSmoothProgress(HWND hWnd,HDC hdc,RECT rt,int iPercent,BOOL isNomal)
// 参数：
// 	IN hWnd  -- 窗口句柄
// 	IN hdc  -- 设备句柄
// 	IN rt  -- RECT 要绘制的矩形大小
// 	IN iPercent  -- 当前的百分比
// 	IN isNormal  -- 是否按正常状态绘制，是 -- 水平方向绘制，否 -- 垂直方向绘制
// 返回值：无
// 功能描述：绘制光滑进度条
// 引用: 
// ************************************************/
static void PaintSmoothProgress(HWND hWnd,HDC hdc,RECT rt,int iPercent,BOOL isNomal)
{
	
	HBRUSH				hBrush;
	_LPPROGRESSDATA			_ppgd;
	RECT rectDraw;
	
	_ppgd = (_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	
	rectDraw = rt;
	//		FillRect(hdc,&rt,GetStockObject(WHITE_BRUSH));
	if(isNomal==TRUE)
	{   // 水平进度条
		// 绘制进度
		//清背景
		RECT rcThumb;
		FillScrollRect( hdc, &rt, lpscrl_brush->hbrushHSBackground );

		//hBrush=CreateSolidBrush(_ppgd->clBarColor);
		//FillRect(hdc,&rectDraw,hBrush);
		//DeleteObject(hBrush);
		// 画头
		rectDraw.right = rectDraw.left + 1;
		FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushLeftArrow );

		//thumb
		rectDraw.left = rectDraw.right;
		rectDraw.right=rectDraw.left+(iPercent*(rt.right-rt.left-2))/100;
		rcThumb = rectDraw;
		rcThumb.top++;
		rcThumb.bottom--;
		FillScrollRect( hdc, &rcThumb, lpscrl_brush->hbrushHSThumb );
		
		// 绘制剩余的背景
		rectDraw.left = rectDraw.right;
		rectDraw.right=rt.right-1;
		//hBrush=CreateSolidBrush(_ppgd->clBackGround);
		//FillRect(hdc,&rectDraw,hBrush);
		//DeleteObject(hBrush);
		//FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushHSBackground );

		//尾
		rectDraw.left = rectDraw.right;
		rectDraw.right = rt.right;

		FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushRightArrow );


	}
	else
	{  // 垂直进度条
		// 绘制进度
		RECT rcThumb;
		//清背景
		FillScrollRect( hdc, &rt, lpscrl_brush->hbrushVSBackground );

		
		// 画头
		rectDraw.bottom = rectDraw.top + 1;
		FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushUpArrow );
		
		//rectDraw.top = rectDraw.bottom;
		rectDraw.top=rectDraw.top+((100-iPercent)*(rt.bottom-rt.top-2))/100;
		rectDraw.bottom = rt.bottom - 1;

		rcThumb = rectDraw;
		rcThumb.left++;
		rcThumb.right--;
		FillScrollRect( hdc, &rcThumb, lpscrl_brush->hbrushVSThumb );

		//hBrush=CreateSolidBrush(_ppgd->clBarColor);
		//FillRect(hdc,&rt,hBrush);
		//DeleteObject(hBrush);
		
		// 绘制剩余的背景
		rectDraw.bottom = rectDraw.top;
		rectDraw.top=rt.top;
		
		//hBrush=CreateSolidBrush(_ppgd->clBackGround);
		//FillRect(hdc,&rt,hBrush);
		//DeleteObject(hBrush);
		rectDraw.top = rt.bottom - 1;;
		rectDraw.bottom = rt.bottom;

		FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushRightArrow );

	}
}
// **************************************************
// 声明：static void PaintNomalProgress(HDC hdc,RECT rt,int iPercent,int iInc,BOOL isNomal)
// 参数：
// 	IN hdc  -- 设备句柄
// 	IN rt  -- RECT 要绘制的矩形大小
// 	IN iPercent  -- 当前的百分比
// 	IN iInc -- 每一格的增长的值
// 	IN isNormal  -- 是否按正常状态绘制，是 -- 水平方向绘制，否 -- 垂直方向绘制
// 返回值： 无
// 功能描述：绘制通用进度条，为一格格的显示。
// 引用: 
// **************************************************
static void PaintNomalProgress(HDC hdc,RECT rt,int iPercent,int iInc,BOOL isNomal)
{
	HBRUSH				hBrush;
	int					i;
	RECT				rtBox;
	
	// 绘制背景
	FillRect(hdc,&rt,GetStockObject(WHITE_BRUSH));
	memcpy(&rtBox,&rt,sizeof(RECT));
	if(isNomal==TRUE)
	{  // 水平进度条
		// 绘制进度
		rt.right=rt.left+(iPercent*(rt.right-rt.left))/100;
		hBrush=CreateSolidBrush(CL_DARKBLUE);
		rtBox.right=rt.left;
		for(i=0;i<rt.right-iInc;i+=iInc)
		{// 绘制一个步长
			rtBox.left=rtBox.right+1;
			rtBox.right=rtBox.left+iInc-1;
			FillRect(hdc,&rtBox,hBrush);
		}
		DeleteObject(hBrush);
	}
	else
	{  // 垂直进度条
		// 绘制进度
		rt.top=rt.top+((100-iPercent)*(rt.bottom-rt.top))/100;
		hBrush=CreateSolidBrush(CL_DARKBLUE);
		rtBox.bottom=rt.top;
		for(i=0;i<rt.bottom-iInc;i+=iInc)
		{// 绘制一个步长
			rtBox.top=rtBox.bottom+1;
			rtBox.bottom=rtBox.top+iInc-1;
			FillRect(hdc,&rtBox,hBrush);
		}
		DeleteObject(hBrush);
	}
}
// **************************************************
// 声明：static void OnPaintProgress(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：绘制进度条
// 引用: 
// **************************************************
static void OnPaintProgress(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	int						iPercent;
	DWORD					style;
	RECT					rt;
	HDC						hdc;
	PAINTSTRUCT				ps;
	hdc = BeginPaint( hWnd, &ps );
	style=GetWindowLong(hWnd,GWL_STYLE);
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	GetClientRect(hWnd,&rt);
	iPercent=(_ppgd->iCurrent*100)/(_ppgd->Pbr.iHigh-_ppgd->Pbr.iLow); // 得到百分比
	if(style&PBS_SMOOTH)
	{ // 显示光滑进度条
		if(style&PBS_VERTICAL)
			PaintSmoothProgress(hWnd,hdc,rt,iPercent,FALSE); // 显示垂直光滑进度条
		else
			PaintSmoothProgress(hWnd,hdc,rt,iPercent,TRUE);  // 显示水平光滑进度条
	}
	else
	{ // 显示块状进度条
		if(style&PBS_VERTICAL)
			PaintNomalProgress(hdc,rt,iPercent,_ppgd->iIncrement,FALSE); // 显示垂直块状进度条
		else
			PaintNomalProgress(hdc,rt,iPercent,_ppgd->iIncrement,TRUE); // 显示水平块状进度条
	}
	EndPaint( hWnd, &ps );
	return ;
}
// **************************************************
// 声明：static BOOL OnCreateProgress(HWND hWnd)
// 参数：
// 	IN hWnd --  窗口句柄
// 返回值： 成功返回TRUE，否则返回FALSE。
// 功能描述：创建进度条
// 引用: 
// **************************************************
static BOOL OnCreateProgress(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	_ppgd=(_LPPROGRESSDATA)malloc(sizeof(_PROGRESSDATA)); // 分配进度条结构
	ASSERT(_ppgd!=NULL);
	memset( _ppgd, 0, sizeof(_PROGRESSDATA) );
	if(_ppgd==NULL)
		return FALSE;
	// 初始化数据
	_ppgd->iIncrement=10;
	_ppgd->iCurrent=0;
	_ppgd->Pbr.iLow=0;
	_ppgd->Pbr.iHigh=100;
//	_ppgd->clBarColor = CL_DARKBLUE;
	_ppgd->clBarColor = GetSysColor(COLOR_HIGHLIGHT);
//	_ppgd->clBackGround = CL_WHITE;
	_ppgd->clBackGround = GetSysColor(COLOR_WINDOW);

	SetWindowLong(hWnd,GWL_USERDATA,(long)_ppgd); // 设置窗口数据
	return TRUE;
}
// **************************************************
// 声明：static int DoSetRange(HWND hWnd,int nMinRange,int nMaxRange)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nMinRange -- 范围最小值
// 	IN nMaxRange -- 范围最大值
// 返回值：返回旧的范围 LOWORD -- 最小值， HIWORD -- 最大值
// 功能描述：设置进度条范围，处理PBM_SETRANGE消息。
// 引用: 
// **************************************************
static int DoSetRange(HWND hWnd,int nMinRange,int nMaxRange)
{
	_LPPROGRESSDATA			_ppgd;
	DWORD					dOld;
	RECT					rt;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	dOld=MAKELONG(_ppgd->Pbr.iLow,_ppgd->Pbr.iHigh);
	// 设置进度条的范围
	_ppgd->Pbr.iLow=nMinRange; // 最小值
	_ppgd->Pbr.iHigh=nMaxRange; // 最大值
	_ppgd->iCurrent=0; // 当前值为0
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE); // 无效窗口
	return dOld;
}
// **************************************************
// 声明：static int DoSetPos(HWND hWnd,int nNewPos)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nNewPos -- 要设置的新的当前值
// 返回值：返回旧的值
// 功能描述：设置当前位置 ，处理PBM_SETPOS消息
// 引用: 
// **************************************************
static int DoSetPos(HWND hWnd,int nNewPos)
{
	_LPPROGRESSDATA			_ppgd;
	RECT					rt;
	int						iOld;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	if((nNewPos<_ppgd->Pbr.iLow)||(nNewPos>_ppgd->Pbr.iHigh))
		return 0; // 要设置的值无效
	iOld=_ppgd->iCurrent;
	_ppgd->iCurrent=nNewPos; // 设置新的当前值
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE); // 无效窗口
	return iOld; // 返回旧的值
}
// **************************************************
// 声明：static int DoDeltaPos(HWND hWnd,int nIncrement)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nIncrement -- 要增加的值
// 返回值：返回旧的当前值
// 功能描述：在当前值加上指定的增加值，处理PBM_DELTAPOS消息。
// 引用: 
// **************************************************
static int DoDeltaPos(HWND hWnd,int nIncrement)
{
	_LPPROGRESSDATA			_ppgd;
	RECT					rt;
	int						iOldCurrent;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	
	iOldCurrent = _ppgd->iCurrent; // 得到当前的值
	if(((_ppgd->iCurrent+nIncrement)<_ppgd->Pbr.iLow)||((_ppgd->iCurrent+nIncrement)>_ppgd->Pbr.iHigh))
		return 0; // 增加增量后数据无效
	_ppgd->iCurrent+=nIncrement; // 增加要增加的值
	GetClientRect(hWnd,&rt); 
	InvalidateRect(hWnd,&rt,TRUE); // 无效窗口
	return iOldCurrent;
}
// **************************************************
// 声明：static int DoSetStep(HWND hWnd,int nStepInc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nStepInc -- 步长
// 返回值：返回原来的步长。
// 功能描述：设置每一步的增加值，处理PBM_SETSTEP消息。
// 引用: 
// **************************************************
static int DoSetStep(HWND hWnd,int nStepInc)
{
	_LPPROGRESSDATA			_ppgd;
	RECT					rt;
	int						iInc;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA); // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	iInc=_ppgd->iIncrement; // 得增量到
	_ppgd->iIncrement=nStepInc; // 设置新的增量
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE); // 无效窗口
	return iInc;
}
// **************************************************
// 声明：static int DoStepIt(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：返回原来的位置
// 功能描述：增加一个步长，处理PBM_STEPIT消息
// 引用: 
// **************************************************
static int DoStepIt(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	RECT					rt;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	if(((_ppgd->iCurrent+_ppgd->iIncrement)<_ppgd->Pbr.iLow)||((_ppgd->iCurrent+_ppgd->iIncrement)>_ppgd->Pbr.iHigh))
		return 0; // 增加一个增量后无效
	_ppgd->iCurrent+=_ppgd->iIncrement; // 增加一个增量后无效
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE); // 无效窗口
	return (_ppgd->iCurrent-_ppgd->iIncrement); // 返回原来的当前值
}
// **************************************************
// 声明：static int DoGetRange(HWND hWnd,BOOL fWhitchLimit,PPBRANGE ppBRange)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN fWhitchLimit -- 指定要返回那一个限制值，
// 						TRUE -- 返回最小值。
// 						FALSE -- 返回最大值。
// 	OUT ppBRange -- 要返回当前进度条的返回，如果为NULL ，则只返回fWhitchLimit指定的值。
// 返回值：返回fWhitchLimit指定的限制值。
// 功能描述：得到当前的进度条的范围，处理PBM_GETRANGE消息。
// 引用: 
// **************************************************
static int DoGetRange(HWND hWnd,BOOL fWhitchLimit,PPBRANGE ppBRange)
{
	_LPPROGRESSDATA			_ppgd;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	ppBRange->iHigh=_ppgd->Pbr.iHigh; // 设置最大值
	ppBRange->iLow=_ppgd->Pbr.iLow; // 设置最小值
	if (fWhitchLimit == TRUE)
		return _ppgd->Pbr.iLow; // 返回最小值
	else
		return _ppgd->Pbr.iHigh; // 返回最大值
}
// **************************************************
// 声明：static UINT DoGetPos(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：返回进度条的当前值。
// 功能描述：得到进度条的当前值，处理PBM_GETPOS消息。
// 引用: 
// **************************************************
static UINT DoGetPos(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	return _ppgd->iCurrent; // 返回当前值
}
// **************************************************
// 声明：static LRESULT SetBarColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN  hWnd -- 窗口句柄
// 	IN  wParam -- 保留
// 	IN  lParam -- COLORREF 要设置的颜色。
// 返回值：返回原来的颜色。
// 功能描述：设置BAR的颜色，处理PBM_SETBARCOLOR消息
// 引用: 
// **************************************************
static LRESULT SetBarColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	_LPPROGRESSDATA			_ppgd;
	COLORREF oldColor;

		_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
		ASSERT(_ppgd!=NULL);
		oldColor = _ppgd->clBarColor;
		_ppgd->clBarColor = (COLORREF)lParam; // 设置进度条的颜色
		return oldColor;
}
// **************************************************
// 声明：static LRESULT SetBKColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN  hWnd -- 窗口句柄
// 	IN  wParam -- 保留
// 	IN  lParam -- COLORREF 要设置的背景颜色。
// 返回值：返回原有的背景颜色。
// 功能描述：设置BAR的背景颜色，处理PBM_SETBKCOLOR消息。
// 引用: 
// **************************************************
static LRESULT SetBKColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	_LPPROGRESSDATA			_ppgd;
	COLORREF oldColor;

		_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
		ASSERT(_ppgd!=NULL);
		oldColor = _ppgd->clBackGround; // 得到原来的背景色
		_ppgd->clBackGround = (COLORREF)lParam; // 设置背景色
		return oldColor; // 返回原来的颜色
}

// **************************************************
// 声明：static void OnDestroyProgress(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：破坏进度条，处理WM_DESTROY消息。
// 引用: 
// **************************************************
static void OnDestroyProgress(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // 得到进度条数据
	ASSERT(_ppgd!=NULL);
	free(_ppgd); // 释放数据结构
}
// **************************************************
// 声明：static LRESULT CALLBACK ProgressProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
// 	IN  hWnd -- 窗口句柄
// 	IN  message -- 要处理的消息
// 	IN  wParam -- 消息参数
// 	IN  lParam -- 消息参数
// 返回值：返回消息的处理结果。
// 功能描述：处理进度条消息。
// 引用: 
// **************************************************
static LRESULT CALLBACK	ProgressProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PAINT:  // 绘制进度条
			OnPaintProgress(hWnd);
			return 0;
		case PBM_SETRANGE: // 设置范围
			return DoSetRange(hWnd,LOWORD(lParam),HIWORD(lParam));
		case PBM_SETPOS: // 设置当前位置
			return DoSetPos(hWnd,wParam);
		case PBM_DELTAPOS: // 增加一个增量
			return DoDeltaPos(hWnd,wParam);
		case PBM_SETSTEP: // 增加一步
			return DoSetStep(hWnd,wParam);
		case PBM_STEPIT: // 设置增量
			return DoStepIt(hWnd);
	//	case PBM_SETRANGE32:
		case PBM_GETRANGE: // 得到范围
			return DoGetRange(hWnd,(BOOL)wParam,(PPBRANGE)lParam);
		case PBM_GETPOS: // 得到当前位置
			return DoGetPos(hWnd);
		case PBM_SETBARCOLOR: // 设置进度条颜色
			return SetBarColor(hWnd,wParam,lParam);
		case PBM_SETBKCOLOR: // 设置背景色
			return SetBKColor(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏窗口
			OnDestroyProgress(hWnd);
			return 0;
		case WM_CREATE: // 创建进度条
			if(OnCreateProgress(hWnd)==FALSE)
				return -1;
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



// **************************************************
// 声明：ATOM RegisterProgressClass(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 返回值：返回注册结果。
// 功能描述：注册进度条的类名。
// 引用: 
// **************************************************
ATOM RegisterProgressClass(HINSTANCE hInstance)
{
	WNDCLASS    wc;

	InitProgressBrush();

	wc.style			= CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc		= (WNDPROC)ProgressProc; // 设置过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance; // 设置实例句柄
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= GetStockObject( WHITE_BRUSH );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classPROGRESS; // 设置进度条的类名



	return RegisterClass(&wc);
}
