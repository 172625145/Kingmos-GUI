/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：Slider类，系统类部分
版本号：1.0.0
开发时期：2003-06-26
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <eWindows.h>
#include <SliderCtrl.h>

/***************  全局区 定义， 声明 *****************/

static const char classSliderCtrl[] = "SliderControl";

#define SLIDERHEIGHT  16
#define TICKHEIGHT	  10

#define VERTBANK			3
#define HORZBANK			10
#define SLIDERSELHEIGHT		12
#define TICKLINELENGTH		6

#define THUMBWIDTH			8
#define THUMBHEIGHT			16


#define POS_THUMBBLOCK		0x0001
#define POS_RIGHTSLIDER		0x0002
#define POS_LEFTSLIDER		0x0003
#define POS_NONE			0x0000

typedef struct{
	int iCurPos;
	int iSliderPos;
	int iTickPos;
	LONG iRangeMin;
	LONG iRangeMax;
	WORD wTicFreq;
	int iSelStart;
	int iSelEnd;
	RECT rectThumb;
	int iAutoTickNum ; 
	int iTickInc ;
	int iSliderlength ;
	int iHorzBank ;
	SIZE sizeThumb;
	HICON hArrowIcon;

	int iPageSize;
	int iLineSize;
	COLORREF cl_Text;
	COLORREF cl_Bk;
}SLIDERCTRLDATA , *LPSLIDERCTRLDATA;


// ********************************************************************
// 函数声明
// ********************************************************************

ATOM RegisterSliderCtrlClass(HINSTANCE hInstance);
static LRESULT CALLBACK SliderCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);


static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetThumbIcon(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetRangeMin(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetRangeMax(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetTic(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTic(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetRangeMin(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetRangeMax(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoClearTics(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetSel(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetSelStart(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetSelEnd(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoGetTics(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetTicPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetNumTicks(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelStart(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelEnd(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoClearSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTicFreq(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetThumbRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetChannelRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTipSide(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam);
/*************************************************************************/
static void DrawHorzSilder(HWND hWnd, HDC hdc);
static void GetThumbRect(HWND hWnd);
static int GetCurrentPosition(HWND hWnd,POINTS points);
static BOOL SetNewCurPos(HWND hWnd,int xPos);
static void MovePageSize(HWND hWnd,int iPosition);
static void ResetSlider(HWND hWnd);

// ********************************************************************
// 声明：ATOM RegisterSliderCtrlClass(HINSTANCE hInstance)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
// 返回值：
//	成功，返回非零，不成功，返回零。
// 功能描述：注册当前应用程序的类
// 引用: 被 应用程序入口程序 调用
// ********************************************************************
ATOM RegisterSliderCtrlClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)SliderCtrlWndProc; // Slider类过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(LONG);
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classSliderCtrl; // Slider类名

	return RegisterClass(&wc); // 注册Slider类
}


// ********************************************************************
// 声明：static LRESULT CALLBACK SliderCtrlWndProc(HWND , UINT , WPARAM , LPARAM )
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	不同的消息有不同的返回值，具体看消息本身
// 功能描述：Slider类窗口过程函数
//引用: 
// ********************************************************************
static LRESULT CALLBACK SliderCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint(hWnd, &ps);
			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_ERASEBKGND: // 删除背景消息
			OnEraseBkgnd( hWnd,( HDC )wParam );
			return 0;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_LBUTTONDOWN: // 鼠标左键按下
			return DoLButtonDown(hWnd,wParam,lParam);
		case WM_MOUSEMOVE: // 鼠标移动
			return DoMouseMove(hWnd,wParam,lParam);
   	    case WM_LBUTTONUP: // 鼠标左键弹起
			// the mouse left button be released
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_CREATE: // 创建消息
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏消息
			DoDestroyWindow(hWnd);
			break;
		case WM_SETCTLCOLOR	: // 设置颜色
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // 得到颜色
			return DoGetColor(hWnd,wParam,lParam);


		case TBM_GETPOS: // 得到当前滑块的位置
			return DoGetPos(hWnd,wParam,lParam);
		case TBM_GETRANGEMIN: // 得到滑块的最小位置
			return DoGetRangeMin(hWnd,wParam,lParam);
		case TBM_GETRANGEMAX: // 得到滑块的最大位置
			return DoGetRangeMax(hWnd,wParam,lParam);
		case TBM_GETTIC: // 得到指定记号所指定的位置
			return DoGetTic(hWnd,wParam,lParam);
		case TBM_SETTIC: // 设置一个记号
			return DoSetTic(hWnd,wParam,lParam);
		case TBM_SETPOS: // 设置一个新的当前位置
			return DoSetPos(hWnd,wParam,lParam);
		case TBM_SETRANGE: // 设置一个新的滑块范围
			return DoSetRange(hWnd,wParam,lParam);
		case TBM_SETRANGEMIN: // 设置一个新的滑块最小的范围
			return DoSetRangeMin(hWnd, wParam, lParam);
		case TBM_SETRANGEMAX:// 设置一个新的滑块最大的范围
			return DoSetRangeMax(hWnd, wParam, lParam);
		case TBM_CLEARTICS:// 清除自设标记
			return DoClearTics(hWnd, wParam, lParam);
		case TBM_SETSEL: // 设置选择区域
			return DoSetSel(hWnd, wParam, lParam);
		case TBM_SETSELSTART: // 设置开始选择区域消息
			return DoSetSelStart(hWnd, wParam, lParam);
		case TBM_SETSELEND:// 设置结束选择区域
			return DoSetSelEnd(hWnd, wParam, lParam);
		case TBM_GETPTICS: // 得到各个标号的位置
			return DoGetTics(hWnd,wParam,lParam);
		case TBM_GETTICPOS: // 得到指定标号的坐标位置
			return DoGetTicPos(hWnd,wParam,lParam);
		case TBM_GETNUMTICS: // 得到标号的坐数目
			return DoGetNumTicks(hWnd,wParam,lParam);
		case TBM_GETSELSTART: // 得到选择区域的开始位置
			return DoGetSelStart(hWnd,wParam,lParam);
		case TBM_GETSELEND: // 得到选择区域的结束位置
			return DoGetSelEnd(hWnd,wParam,lParam);
		case TBM_CLEARSEL: // 清除选择区域
			return DoClearSel(hWnd,wParam,lParam);
		case TBM_SETTICFREQ: // 设置自动标号的频率
			return DoSetTicFreq(hWnd,wParam,lParam);
		case TBM_SETPAGESIZE: // 设置页尺寸
			return DoSetPageSize(hWnd,wParam,lParam);
		case TBM_GETPAGESIZE: // 得到页尺寸
			return DoGetPageSize(hWnd,wParam,lParam);
		case TBM_SETLINESIZE: // 设置行尺寸
			return DoSetLineSize(hWnd,wParam,lParam);
		case TBM_GETLINESIZE: // 得到行尺寸
			return DoGetLineSize(hWnd,wParam,lParam);
		case TBM_GETTHUMBRECT: // 得到滑块的位置
			return DoGetThumbRect(hWnd,wParam,lParam);
		case TBM_GETCHANNELRECT: // 得到滑道的位置
			return DoGetChannelRect(hWnd,wParam,lParam);
		case TBM_SETTHUMBLENGTH: // 设置滑块的长度
			return DoSetThumbLength(hWnd,wParam,lParam);
		case TBM_GETTHUMBLENGTH: // 得到滑块的长度
			return DoGetThumbLength(hWnd,wParam,lParam);
		case TBM_SETTOOLTIPS: // 设置工具提示窗口句柄
			return DoSetToolTips(hWnd,wParam,lParam);
		case TBM_GETTOOLTIPS: // 得到工具提示窗口句柄
			return DoGetToolTips(hWnd,wParam,lParam);
		case TBM_SETTIPSIDE: // 设置工具提示窗口的位置
			return DoSetTipSide(hWnd,wParam,lParam);

		case TBM_SETBUDDY: // 设置绑定窗口
			return DoSetBuddy(hWnd,wParam,lParam);
		case TBM_GETBUDDY: // 得到绑定窗口
			return DoGetBuddy(hWnd,wParam,lParam);
		
		case TBM_SETTHUMBICON: // 设置滑块的位图
			return DoSetThumbIcon(hWnd,wParam,lParam);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}



// ********************************************************************
// 声明：static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	返回0，将继续创建窗口，返回-1，则会破坏窗口
// 功能描述：应用程序处理创建消息
// 引用: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	
	LPSLIDERCTRLDATA lpSliderCtrlData;
	RECT rect;
	DWORD dwStyle;

	lpSliderCtrlData = (LPSLIDERCTRLDATA)malloc(sizeof(SLIDERCTRLDATA)); // 分配一个Slider类结构
	if (lpSliderCtrlData == NULL)
		return -1; // 不能创建该窗口
	
	GetClientRect(hWnd,&rect); // 得到客户矩形
	dwStyle = (DWORD)GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格

	// 初始化Slider类结构
	lpSliderCtrlData->iCurPos = 0;
	lpSliderCtrlData->iSliderPos = 0;
	lpSliderCtrlData->iTickPos = 0;
	lpSliderCtrlData->iRangeMin = 0;
	lpSliderCtrlData->iRangeMax = 10;
	lpSliderCtrlData->wTicFreq = 1;
	lpSliderCtrlData->iSelStart = -1;
	lpSliderCtrlData->iSelEnd = -1;
	lpSliderCtrlData->iPageSize = lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin;
	lpSliderCtrlData->iLineSize = 1;

	lpSliderCtrlData->hArrowIcon = NULL;

	if (dwStyle & TBS_NOTICKS)
	{ // 没有标记
		lpSliderCtrlData->iSliderPos = rect.top + VERTBANK;
	}
	else
	{ // 有标记
		if ((dwStyle & TBS_TOP) == 0)
		{
			// 标记在滑道的下面
			lpSliderCtrlData->iSliderPos = rect.top + VERTBANK;
			lpSliderCtrlData->iTickPos = lpSliderCtrlData->iSliderPos + SLIDERHEIGHT; 
		}
		else
		{
			// 标记在滑道的上面
			lpSliderCtrlData->iTickPos = rect.top + VERTBANK;
			lpSliderCtrlData->iSliderPos = lpSliderCtrlData->iTickPos+ TICKHEIGHT; 
		}

	}

	lpSliderCtrlData->iAutoTickNum = ((lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin + (lpSliderCtrlData->wTicFreq -1) ) / lpSliderCtrlData->wTicFreq) -1; // -1 ， 不需要画最后一条线
	lpSliderCtrlData->iTickInc = (rect.right - rect.left - HORZBANK * 2) / (lpSliderCtrlData->iAutoTickNum + 1); // 将当前的长度等样划分
	lpSliderCtrlData->iSliderlength = lpSliderCtrlData->iTickInc * (lpSliderCtrlData->iAutoTickNum + 1); // 每一份的长度 * 份数
	lpSliderCtrlData->iHorzBank = (rect.right - rect.left - lpSliderCtrlData->iSliderlength) / 2; // 总长度 - Slider的长度，然后两边等份

	lpSliderCtrlData->sizeThumb.cx = THUMBWIDTH;
	lpSliderCtrlData->sizeThumb.cy = THUMBHEIGHT;

	// 设置颜色
	lpSliderCtrlData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);;
	lpSliderCtrlData->cl_Bk  = GetSysColor(COLOR_STATIC);

	SetWindowLong(hWnd,0,(DWORD)lpSliderCtrlData); // 设置Slider类结构到窗口

	GetThumbRect(hWnd); // 得到滑块的矩形
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoDestroyWindow(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	返回TRUE，将继续破坏窗口，返回FALSE，则不破坏窗口
// 功能描述：应用程序处理破坏窗口的消息
// 引用: 
// ********************************************************************
static LRESULT DoDestroyWindow(HWND hWnd)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0);  // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		if (lpSliderCtrlData->hArrowIcon != NULL)
			DestroyIcon(lpSliderCtrlData->hArrowIcon); // 破坏箭头ICON

		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoPaint(HWND hWnd,HDC hdc)
// 参数：
//	IN hWnd - 窗口句柄
//  IN hdc - 设备句柄
// 返回值：
//	无
// 功能描述：WM_PAINT 处理过程
// 引用: 
// ********************************************************************
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;
	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		GetClientRect(hWnd,&rect); // 得到客户矩形
		SetTextColor(hdc,lpSliderCtrlData->cl_Text); // 设置前景颜色
		SetBkColor(hdc,lpSliderCtrlData->cl_Bk); // 设置背景颜色

		dwStyle = (DWORD)GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
/*
#define TBS_AUTOTICKS           0x0001
#define TBS_VERT                0x0002
#define TBS_HORZ                0x0000
#define TBS_TOP                 0x0004
#define TBS_BOTTOM              0x0000
#define TBS_LEFT                0x0004
#define TBS_RIGHT               0x0000
#define TBS_BOTH                0x0008
#define TBS_NOTICKS             0x0010
#define TBS_ENABLESELRANGE      0x0020
#define TBS_FIXEDLENGTH         0x0040
#define TBS_NOTHUMB             0x0080
#define TBS_TOOLTIPS            0x0100
*/
		if (dwStyle & TBS_VERT)
		{
			// 是垂直的滑道
			if (dwStyle & TBS_NOTICKS)
			{
			}
			else
			{
			}
		}
		else
		{ // 水平滑道
			DrawHorzSilder(hWnd, hdc); // 绘制水平滑道
		}
		return 0;
}

// ********************************************************************
// 声明：static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc)
// 参数：
//  IN hWnd - 窗口句柄
//  IN hdc - 设备句柄
// 返回值：
//	
// 功能描述：WM_ERASEBKGND 处理过程
// 引用: 
// ********************************************************************
static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	HBRUSH hBrush;
	RECT rect;
		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		hBrush = CreateSolidBrush(lpSliderCtrlData->cl_Bk); // 创建背景颜色画刷
		if (hBrush == NULL)
			return 0;
		GetClientRect(hWnd,&rect); // 得到客户矩形
		FillRect(hdc,&rect,hBrush); // 填充客户区
		DeleteObject(hBrush); // 删除画刷
		return 0;
}

// **************************************************
// 声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(X,Y),当前鼠标的位置。
// 返回值：无
// 功能描述：处理WM_LBUTTONDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINTS points;
	int iPosition;

		points.x=LOWORD(lParam);
		points.y=HIWORD(lParam);

		iPosition = GetCurrentPosition(hWnd,points); // 得到当前点的位置

		switch(iPosition)
		{
			case POS_THUMBBLOCK: // 在滑块上
				SetCapture(hWnd); // 抓住鼠标，用于拖动滑块
				break;
			case POS_RIGHTSLIDER: // 滑块右边滑道
			case POS_LEFTSLIDER: // 滑块左边滑道
				MovePageSize(hWnd,iPosition); // 移动一页
				break;
			case POS_NONE: // 空白位置
				break;
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(X,Y),当前鼠标的位置。
// 返回值：无
// 功能描述：处理WM_MOUSEMOVE消息。
// 引用: 
// **************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	short xPos,yPos;
	DWORD dwStyle;
 

	// if the mouse be capture by this window
	if (GetCapture()==hWnd)
	{ // 点在滑块上
			// Get LButton down Position
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);

			dwStyle = (DWORD)GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			if (dwStyle & TBS_VERT)
			{ // 垂直滑道
			}
			else
			{ // 水平滑道
				if (SetNewCurPos(hWnd,xPos) == TRUE) // 设置新的滑块位置
					InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
			}
	}
	return 0;
}
// **************************************************
// 声明：static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(X,Y),当前鼠标的位置。
// 返回值：无
// 功能描述：处理WM_LBUTTONUP消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	// Release capture 
	ReleaseCapture(); // 释放鼠标
	wParam++;
	lParam++;
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 指向结构CTLCOLORSTRUCT的指针
// 返回值：无
// 功能描述：处理WM_SETCTLCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		 
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 设置文本颜色
			 lpSliderCtrlData->cl_Text = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 设置文本背景颜色
			 lpSliderCtrlData->cl_Bk = lpCtlColor->cl_TextBk;
		 }
		return TRUE;
}
// **************************************************
// 声明：static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN/OUT lParam -- 指向结构CTLCOLORSTRUCT的指针
// 返回值：无
// 功能描述：处理WM_GETCTLCOLORR消息。
// 引用: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 得到文本颜色
			 lpCtlColor->cl_Text = lpSliderCtrlData->cl_Text ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 得到文本背景颜色
				lpCtlColor->cl_TextBk = lpSliderCtrlData->cl_Bk ;
		 }
		 return TRUE;
}


// ********************************************************************
// 声明：static void DrawHorzSilder(HWND hWnd, HDC hdc)
// 参数：
//    IN hWnd - 窗口句柄
//    IN hdc - 设备句柄
// 返回值：
//       无	
// 功能描述：绘制水平方向的滑道
// 引用: 
// ********************************************************************
static void DrawHorzSilder(HWND hWnd, HDC hdc)
{
		// 是水平的滑道
//	int iTickPos = -1,iSliderPos = -1;
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;
	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		GetClientRect(hWnd,&rect); // 得到客户矩形
		dwStyle = (DWORD)GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格

		// 画标记
		if (lpSliderCtrlData->iTickPos != -1)
		{
//	lpSliderCtrlData->iAutoTickNum = ((lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin + (lpSliderCtrlData->wTicFreq -1) ) / lpSliderCtrlData->wTicFreq) -1; // -1 ， 不需要画最后一条线
//	lpSliderCtrlData->iTickInc = (rect.right - rect.left - HORZBANK * 2) / (lpSliderCtrlData->iAutoTickNum + 1);
//	lpSliderCtrlData->iSliderlength = lpSliderCtrlData->iTickInc * (lpSliderCtrlData->iAutoTickNum + 1);
//	lpSliderCtrlData->iHorzBank = (rect.right - rect.left - lpSliderCtrlData->iSliderlength) / 2;
			// 画最左边的标记
			MoveToEx(hdc,rect.left + lpSliderCtrlData->iHorzBank,lpSliderCtrlData->iTickPos,NULL);
			LineTo(hdc,rect.left + lpSliderCtrlData->iHorzBank,lpSliderCtrlData->iTickPos + TICKLINELENGTH);
			// 画最右边的标记
			MoveToEx(hdc,rect.right - lpSliderCtrlData->iHorzBank ,lpSliderCtrlData->iTickPos,NULL);
			LineTo(hdc,rect.right - lpSliderCtrlData->iHorzBank,lpSliderCtrlData->iTickPos + TICKLINELENGTH);
			if (dwStyle & TBS_AUTOTICKS)
			{ // 有自动标记
				int iTickHorz = rect.left + lpSliderCtrlData->iHorzBank;
				int i;

					for (i = 1; i <= lpSliderCtrlData->iAutoTickNum ; i++)  //不需要画两端的线
					{  // 画每一个标记
						MoveToEx(hdc,iTickHorz + lpSliderCtrlData->iTickInc * i ,lpSliderCtrlData->iTickPos,NULL);
						LineTo(hdc,iTickHorz + lpSliderCtrlData->iTickInc * i,lpSliderCtrlData->iTickPos + TICKLINELENGTH);
					}
			}
		}
		// 画滑道及滑块
		if (lpSliderCtrlData->iSliderPos != -1)
		{
			if ((dwStyle & TBS_NOTHUMB) == 0)
			{
				// 需要画滑道
				if (dwStyle & TBS_ENABLESELRANGE)
				{ // 可以选择范围，即中空
					HBRUSH hBrush;
					RECT rectRange;
						// 滑道外边框
						rectRange.left = rect.left + lpSliderCtrlData->iHorzBank;
						rectRange.top = lpSliderCtrlData->iSliderPos;
						rectRange.right = rectRange.left+ lpSliderCtrlData->iSliderlength;
						rectRange.bottom = lpSliderCtrlData->iSliderPos + SLIDERSELHEIGHT;
						DrawEdge(hdc,&rectRange,BDR_SUNKENINNER,BF_RECT);
						InflateRect(&rectRange,-2,-2);
						// 滑道内空
						hBrush = GetStockObject(WHITE_BRUSH);
						FillRect(hdc,&rectRange,hBrush);
				}
				else
				{ // 不可以选择范围
					HPEN hPen;
					int iPos;
						
						hPen = CreatePen(PS_SOLID,1,RGB(150,150,150)); // 创建深灰色笔
						hPen = (HPEN)SelectObject(hdc,hPen); // 选择笔
						iPos = lpSliderCtrlData->iSliderPos + SLIDERHEIGHT /2 - 2; // 得到画线的位置
						MoveToEx(hdc,rect.left + lpSliderCtrlData->iHorzBank ,iPos,NULL);
						LineTo(hdc,rect.right -lpSliderCtrlData->iHorzBank,iPos); // 画一条线
						hPen = (HPEN)SelectObject(hdc,hPen); // 恢复笔
						DeleteObject(hPen); // 删除笔

						hPen = CreatePen(PS_SOLID,1,RGB(220,220,220)); // 创建浅灰色笔
						hPen = (HPEN)SelectObject(hdc,hPen);
						iPos += 1; // 到下一行的位置
						MoveToEx(hdc,rect.left + lpSliderCtrlData->iHorzBank ,iPos,NULL);
						LineTo(hdc,rect.right - lpSliderCtrlData->iHorzBank,iPos); // 画一条线
						hPen = (HPEN)SelectObject(hdc,hPen);
						DeleteObject(hPen); // 删除笔

						hPen = CreatePen(PS_SOLID,1,RGB(255,255,255)); // 创建白色的笔
						hPen = (HPEN)SelectObject(hdc,hPen);
						iPos += 1; // 到下一行的位置
						MoveToEx(hdc,rect.left + lpSliderCtrlData->iHorzBank ,iPos,NULL);
						LineTo(hdc,rect.right - lpSliderCtrlData->iHorzBank,iPos); // 画一条线
						hPen = (HPEN)SelectObject(hdc,hPen);
						DeleteObject(hPen); // 删除笔
				}
			}
			// 画滑块
			{
//				int iSliderBlockx = lpSliderCtrlData->iCurPos;
//				int iSliderBlocky = lpSliderCtrlData->iSliderPos;
				if (lpSliderCtrlData->hArrowIcon == NULL)
				{ // 没有指定滑块的ICON ，使用系统滑块
					HICON hIcon;

					if (dwStyle & TBS_TOP)
					{ // 滑块在上面的滑块
						hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SLIDERBLOCK_TOP ), IMAGE_ICON,THUMBWIDTH, THUMBHEIGHT, 0 ) ;
					}
					else if (dwStyle & TBS_BOTH)
					{ // 滑块两边都有的滑块
						hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SLIDERBLOCK_HORZBOTH ), IMAGE_ICON,THUMBWIDTH, THUMBHEIGHT, 0 ) ;
					}
					else 
					{ // 滑块在下面的滑块
						hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SLIDERBLOCK_BOTTOM ), IMAGE_ICON,THUMBWIDTH, THUMBHEIGHT, 0 ) ;
					}
					DrawIcon(hdc,lpSliderCtrlData->rectThumb.left,lpSliderCtrlData->rectThumb.top,hIcon); // 绘制滑块
					DestroyIcon(hIcon); // 破坏滑块
				}
				else
				{
					DrawIcon(hdc,lpSliderCtrlData->rectThumb.left,lpSliderCtrlData->rectThumb.top,lpSliderCtrlData->hArrowIcon); // 绘制滑块
				}
			}
		}
}


// ********************************************************************
// 声明：static void GetThumbRect(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	无
// 功能描述：得到滑块的位置
// 引用: 
// ********************************************************************
static void GetThumbRect(HWND hWnd)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		GetClientRect(hWnd,&rect); // 得到客户矩形

		if (lpSliderCtrlData->iSliderPos != -1)
		{ // 得到滑块的矩形
			lpSliderCtrlData->rectThumb.left = lpSliderCtrlData->iHorzBank + lpSliderCtrlData->iCurPos *  lpSliderCtrlData->iSliderlength / (lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin) - lpSliderCtrlData->sizeThumb.cx / 2;
			lpSliderCtrlData->rectThumb.right = lpSliderCtrlData->rectThumb.left + lpSliderCtrlData->sizeThumb.cx;
			lpSliderCtrlData->rectThumb.top = lpSliderCtrlData->iSliderPos + (SLIDERHEIGHT - lpSliderCtrlData->sizeThumb.cy) /2;
			lpSliderCtrlData->rectThumb.bottom = lpSliderCtrlData->rectThumb.top + lpSliderCtrlData->sizeThumb.cy;
		}
}

// ********************************************************************
// 声明：static int GetCurrentPosition(HWND hWnd,POINTS points);
// 参数：
//	IN hWnd - 窗口句柄
//    IN points - 当前要得到位置的点坐标
// 返回值：
//	得到的位置。
// 功能描述：得到指定坐标的点所在的位置
// 引用: 
// ********************************************************************
static int GetCurrentPosition(HWND hWnd,POINTS points)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	POINT point;
	RECT rectSlider;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		point.x = points.x;
		point.y = points.y;
		if( PtInRect( &lpSliderCtrlData->rectThumb, point ) )
		{ // 点在滑块上
			return POS_THUMBBLOCK;
		}

		// 得到滑道的矩形		
		rectSlider.left = lpSliderCtrlData->iHorzBank;
		rectSlider.right = rectSlider.left + lpSliderCtrlData->iSliderlength;
		rectSlider.top = lpSliderCtrlData->iSliderPos;
		rectSlider.bottom = rectSlider.top + SLIDERHEIGHT;

		if( PtInRect( &rectSlider, point ) )
		{ // 点在滑道上
			if (point.x > lpSliderCtrlData->rectThumb.left)
				return POS_RIGHTSLIDER; // 点在右边滑道
			return POS_LEFTSLIDER; // 点在左边滑道
		}
		return POS_NONE; // 点在空白位置
}

// ********************************************************************
// 声明：static BOOL SetNewCurPos(HWND hWnd,int xPos)
// 参数：
//    IN hWnd - 窗口句柄
//    IN xPox - 当前点的位置
// 返回值：
//	成功返回TRUE，否则返回FALSE。
// 功能描述：设置新的位置。
// 引用: 
// ********************************************************************
static BOOL SetNewCurPos(HWND hWnd,int xPos)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
//	POINT point;
//	RECT rectSlider;
	int iNewCurPos  = 0;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		if (xPos < lpSliderCtrlData->iHorzBank)
		{ // 超过最小位置，设置到最小位置
			iNewCurPos = lpSliderCtrlData->iRangeMin;
		}
		else if (xPos > (lpSliderCtrlData->iHorzBank + lpSliderCtrlData->iSliderlength))
		{ // 超过最大位置，设置到最大位置
			iNewCurPos = lpSliderCtrlData->iRangeMax;
		}
		else
		{ // 得到新的位置
			iNewCurPos = (xPos - lpSliderCtrlData->iHorzBank) * (lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin) /  lpSliderCtrlData->iSliderlength;
		}
		if (iNewCurPos != lpSliderCtrlData->iCurPos)
		{ // 位置改变
			lpSliderCtrlData->iCurPos = iNewCurPos; // 设置新的滑块位置
			GetThumbRect(hWnd); // 得到滑块的矩形
			return TRUE;
		}
		return FALSE;
}
// **************************************************
// 声明：static void MovePageSize(HWND hWnd,int iPosition)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iPosition -- 当前鼠标在滑块上的位置
// 返回值：无
// 功能描述：移动一页。
// 引用: 
// **************************************************
static void MovePageSize(HWND hWnd,int iPosition)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		switch(iPosition)
		{
			case POS_RIGHTSLIDER: // 向右移动一页
				lpSliderCtrlData->iCurPos +=lpSliderCtrlData->iPageSize;
				break;
			case POS_LEFTSLIDER: // 向左移动一页
				lpSliderCtrlData->iCurPos -=lpSliderCtrlData->iPageSize;
				break;
			default:
				return ;
		}
		if (lpSliderCtrlData->iCurPos > lpSliderCtrlData->iRangeMax)
		{ // 超过最大位置，设置到最大位置
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMax;
		}

		if (lpSliderCtrlData->iCurPos < lpSliderCtrlData->iRangeMin)
		{ // 超过最小位置，设置到最小位置
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMin;
		}
		GetThumbRect(hWnd); // 得到滑块的矩形
		InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
}

// ********************************************************************
// ********************************************************************
// ********************************************************************
// 声明：static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//  IN wParam = 0  
//  IN lParam = 0
// 返回值：
//	返回当前滑块的位置
// 功能描述：得到当前滑块的位置，处理TBM_GETPOS消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		return lpSliderCtrlData->iCurPos; // 返回当前的位置
}
// ********************************************************************
// 声明：static LRESULT DoSetThumbIcon(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//    IN hWnd - 窗口句柄
//    IN wParam = MAKEWPARAM(cx,cy)  指定位图的大小
//    IN lParam = (HICON)hIcon       要设定的位图句柄
// 返回值：
//	成功返回TRUE，否则返回FALSE
// 功能描述：设置滑块的位图，处理TBM_SETTHUMBICON消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetThumbIcon(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	HICON hIcon;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		hIcon = (HICON)lParam; // 得到滑块的ICON句柄
		if (hIcon)
		{
			if (lpSliderCtrlData->hArrowIcon)
				DestroyIcon(lpSliderCtrlData->hArrowIcon); // 破坏原来的ICON句柄
			lpSliderCtrlData->hArrowIcon = hIcon;
			lpSliderCtrlData->sizeThumb.cx = LOWORD(wParam); // 得到ICON的大小
			lpSliderCtrlData->sizeThumb.cy = LOWORD(wParam);
			GetThumbRect(hWnd); // 得到滑块的矩形
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
			return TRUE;
		}
		return FALSE;
}

// ********************************************************************
// 声明：static LRESULT DoGetRangeMin(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0  
//    IN lParam = 0
// 返回值：
//	返回滑块的最小位置
// 功能描述：得到滑块的最小位置，处理TBM_GETRANGEMIN消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetRangeMin(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		return lpSliderCtrlData->iRangeMin; // 得到滑道的最小值
}
// ********************************************************************
// 声明：static LRESULT DoGetRangeMax(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0  
//    IN lParam = 0
// 返回值：
//	返回滑块的最大位置
// 功能描述：得到滑块的最大位置，处理TBM_GETRANGEMAX消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetRangeMax(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		return lpSliderCtrlData->iRangeMax; // 得到滑道的最大值
}

// ********************************************************************
// 声明：static LRESULT DoGetTic(HWND hWnd,WPARAM wParam,LPARAM lParam);
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (WORD) iTick; 
//    IN lParam = 0
// 返回值：
//	返回指定记号所指定的位置
// 功能描述：得到指定记号所指定的位置，处理TBM_GETTIC消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetTic(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iPos;
	short iTick;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		iTick = (short)wParam; // 得到标记的索引号
		if (iTick < 0)
			return -1;
		if (iTick >= lpSliderCtrlData->iAutoTickNum)
			return -1;
		iPos = lpSliderCtrlData->iRangeMin + (iTick+1) * lpSliderCtrlData->iTickInc; // 得到指定标号的位置
		return iPos; // 返回指定标号的位置
}
// ********************************************************************
// 声明：static LRESULT DoSetTic(HWND hWnd,WPARAM wParam,LPARAM lParam);
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = (LONG)lPosition;
// 返回值：
//	成功返回TRUE，否则返回FALSE。
// 功能描述：设置一个记号，处理TBM_SETTIC消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetTic(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}


// ********************************************************************
// 声明：static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)lPosition;
// 返回值：
//	没有返回值。
// 功能描述：设置一个新的当前位置，处理TBM_SETPOS消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iPos;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;
		iPos = (LONG)lParam;

		if (iPos < lpSliderCtrlData->iRangeMin)
			iPos = lpSliderCtrlData->iRangeMin; // 当前位置小于最小值，设置到最小值
		if (iPos > lpSliderCtrlData->iRangeMax)
			iPos = lpSliderCtrlData->iRangeMax; // 当前位置大于最大值，设置到最大值

		lpSliderCtrlData->iCurPos = iPos; // 设置当前位置
		GetThumbRect(hWnd); // 得到滑块矩形
		if (bRedraw == TRUE)
		{ // 要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)MAKELONG(lMin,lMax);
// 返回值：
//	没有返回值。
// 功能描述：设置一个新的滑块范围，处理TBM_SETRANGE消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMax,iMin;
	BOOL bRedraw;
 
		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;
		iMax = (LONG)HIWORD(lParam);
		iMin = (LONG)LOWORD(lParam);

		lpSliderCtrlData->iRangeMax = iMax; // 设置最大值
		lpSliderCtrlData->iRangeMin = iMin; // 设置最小值

		if (lpSliderCtrlData->iCurPos > lpSliderCtrlData->iRangeMax)
		{ // 当前位置大于最大值
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMax; // 设置到最大值
			GetThumbRect(hWnd); // 得到滑块矩形
		}

		if (lpSliderCtrlData->iCurPos < lpSliderCtrlData->iRangeMin)
		{ // 当前位置小于最小值
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMin; // 设置到最小值
			GetThumbRect(hWnd); // 得到滑块矩形
		}
		ResetSlider(hWnd); // 重设Slider

		if (bRedraw == TRUE)
		{ // 需要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoSetRangeMin(HWND hWnd, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (bool)bRedraw; 
//    IN lParam = lMin;
// 返回值：
//	没有返回值。
// 功能描述：设置一个新的滑块最小的范围，处理TBM_SETRANGEMIN消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetRangeMin(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMin;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;
		iMin = (LONG)lParam;

		lpSliderCtrlData->iRangeMin = iMin; // 设置最小值

		if (lpSliderCtrlData->iCurPos < lpSliderCtrlData->iRangeMin)
		{ // 当前位置小于最小值
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMin; // 将当前位置设置到最小值
			GetThumbRect(hWnd); // 得到滑块矩形
		}
		ResetSlider(hWnd); // 重设Slider

		if (bRedraw == TRUE)
		{ // 需要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoSetRangeMax(HWND hWnd, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (bool)bRedraw; 
//    IN lParam = lMax;
// 返回值：
//	没有返回值。
// 功能描述：设置一个新的滑块最大的范围，处理TBM_SETRANGEMAX消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetRangeMax(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMax;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;
		iMax = (LONG)lParam;

		lpSliderCtrlData->iRangeMax = iMax; // 设置最大值

		if (lpSliderCtrlData->iCurPos > lpSliderCtrlData->iRangeMax)
		{ // 当前位置大于最大值
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMax; // 设置当前位置到最大值
			GetThumbRect(hWnd); // 得到滑块的矩形
		}
		ResetSlider(hWnd); // 重设Slider

		if (bRedraw == TRUE)
		{ // 需要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoClearTics(HWND hWnd, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (bool)bRedraw; 
//    IN lParam = 0;
// 返回值：
//	没有返回值。
// 功能描述：清除自设标记，处理TBM_CLEARTICS消息。
// 引用: 
// ********************************************************************
static LRESULT DoClearTics(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;

		if (bRedraw == TRUE)
		{ // 需要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoSetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)MAKELONG(lMin,lMax);
// 返回值：
//	没有返回值。
// 功能描述：设置选择区域，处理TBM_SETSEL消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMax,iMin;
	BOOL bRedraw;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格

		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		bRedraw = (BOOL)wParam;
		iMax = (LONG)HIWORD(lParam);
		iMin = (LONG)LOWORD(lParam);

		lpSliderCtrlData->iSelStart = iMin; // 设置选择最小值
		lpSliderCtrlData->iSelEnd = iMax; // 设置选择最大值

		if (bRedraw == TRUE)
		{ // 需要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoSetSelStart(HWND hWnd, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)lMin;
// 返回值：
//	没有返回值。
// 功能描述：设置开始选择区域，处理TBM_SETSELSTART消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetSelStart(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMin;
	BOOL bRedraw;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		bRedraw = (BOOL)wParam;
		iMin = (LONG)lParam;

		lpSliderCtrlData->iSelStart = iMin; // 设置选择最小值

		if (bRedraw == TRUE)
		{ // 需要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoSetSelEnd(HWND hWnd, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)lMax;
// 返回值：
//	没有返回值。
// 功能描述：设置结束选择区域，处理TBM_SETSELEND消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetSelEnd(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMax;
	BOOL bRedraw;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		bRedraw = (BOOL)wParam;
		iMax = (LONG)lParam;

		lpSliderCtrlData->iSelEnd = iMax; // 设置选择的最大值

		if (bRedraw == TRUE)
		{ // 需要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoGetTics(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = 0;
// 返回值：
//	成功返回一个存放各个标号的位置的数组。否则返回NULL.
// 功能描述：得到各个标号的位置，处理TBM_GETPTICS消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetTics(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoGetTicPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (WORD)iTick; 
//    IN lParam = 0;
// 返回值：
//	成功返回一个指定标号的坐标位置.否则返回-1。
// 功能描述：得到指定标号的坐标位置，处理TBM_GETTICPOS消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetTicPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	short iTick;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0);  // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		iTick = (short)wParam;
		if (iTick < 0)
			return -1;
		if (iTick >= lpSliderCtrlData->iAutoTickNum -1 )
			return -1;

		return -1;
}


// ********************************************************************
// 声明：static LRESULT DoGetNumTicks(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = 0;
// 返回值：
//	成功返回标号的数目.
// 功能描述：得到标号的坐数目，处理TBM_GETNUMTICS消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetNumTicks(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		return lpSliderCtrlData->iAutoTickNum + 2; // 返回标记的个数
}

// ********************************************************************
// 声明：static LRESULT DoGetSelStart(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = 0;
// 返回值：
//	成功返回选择区域的开始位置.
// 功能描述：得到选择区域的开始位置，处理TBM_GETSELSTART消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetSelStart(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		return lpSliderCtrlData->iSelStart; // 返回当前选择的最小值
}

// ********************************************************************
// 声明：static LRESULT DoGetSelEnd(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = 0;
// 返回值：
//	成功返回选择区域的结束位置.
// 功能描述：得到选择区域的结束位置，处理TBM_GETSELEND消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetSelEnd(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		return lpSliderCtrlData->iSelEnd; // 得到选择的最小值
}

// ********************************************************************
// 声明：static LRESULT DoClearSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (BOOL) fRedraw; 
//    IN lParam = 0;
// 返回值：
//	无
// 功能描述：清除选择区域，处理TBM_CLEARSEL消息。
// 引用: 
// ********************************************************************
static LRESULT DoClearSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		bRedraw = (BOOL)wParam;
		// 清除选择
		lpSliderCtrlData->iSelStart = -1;
		lpSliderCtrlData->iSelEnd = -1;

		if (bRedraw == TRUE)
		{ // 需要重绘窗口
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoSetTicFreq(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (BOOL) fRedraw; 
//    IN lParam = 0;
// 返回值：
//	无
// 功能描述：设置自动标号的频率，处理TBM_SETTICFREQ消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetTicFreq(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;
	WORD wTicFreq;
	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (!(dwStyle & TBS_AUTOTICKS))
			return 0;

		GetClientRect(hWnd,&rect); //得到客户矩形
		wTicFreq = (WORD)wParam;
		
		lpSliderCtrlData ->wTicFreq = wTicFreq; // 设置自动标号的频率
		ResetSlider(hWnd); // 重设Slider

		InvalidateRect(hWnd,NULL,TRUE); // 无效窗口

		return 0;
}


// ********************************************************************
// 声明：static LRESULT DoSetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = (LONG)lPageSize;
// 返回值：
//	无
// 功能描述：设置页尺寸，处理TBM_SETPAGESIZE消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int lPageSize;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		
		lPageSize = (int)lParam;

		lpSliderCtrlData->iPageSize = lPageSize;  // 设置页尺寸

		return 0;
}


// ********************************************************************
// 声明：static LRESULT DoGetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = 0;
// 返回值：
//	返回页尺寸.
// 功能描述：得到页尺寸，处理TBM_GETPAGESIZE消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		
		return lpSliderCtrlData->iPageSize ;  // 得到当前的页尺寸
}


// ********************************************************************
// 声明：static LRESULT DoSetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = (LONG)lLineSize;
// 返回值：
//	
// 功能描述：设置行尺寸，处理TBM_SETLINESIZE消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int lLineSize;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		
		lLineSize = (int)lParam;

		lpSliderCtrlData->iLineSize = lLineSize;  // 设置行尺寸

		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoGetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = 0;
// 返回值：
//	返回行尺寸
// 功能描述：得到行尺寸，处理TBM_GETLINESIZE消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		
		return lpSliderCtrlData->iLineSize ;  // 得到行尺寸
}


// ********************************************************************
// 声明：static LRESULT DoGetThumbRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    OUT lParam = (LPRECT)lpRect; //存放滑块位置的矩形
// 返回值：
//	无
// 功能描述：得到滑块的位置
// 引用: 
// ********************************************************************
static LRESULT DoGetThumbRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	LPRECT lpRect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		
		lpRect = (LPRECT)lParam;
		if (lpRect == NULL)
			return 0;

		*lpRect = lpSliderCtrlData->rectThumb; // 得到滑块的矩形
		return 0; 

}

// ********************************************************************
// 声明：static LRESULT DoGetChannelRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = (LPRECT)lpRect; //存放滑道位置的矩形
// 返回值：
//	无
// 功能描述：得到滑道的位置，处理TBM_GETCHANNELRECT消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetChannelRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	LPRECT lpRect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);
		
		lpRect = (LPRECT)lParam;
		if (lpRect == NULL)
			return 0;
		
		//lpRect->left = lpSliderCtrlData->
		return 0; 
}


// ********************************************************************
// 声明：static LRESULT DoSetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (UINT)iLength; 
//    IN lParam = 0; 
// 返回值：
//	无
// 功能描述：设置滑块的长度，处理TBM_SETTHUMBLENGTH消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoGetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (UINT)iLength; 
//    IN lParam = 0; 
// 返回值：
//	返回滑块的长度
// 功能描述：得到滑块的长度，处理TBM_GETTHUMBLENGTH消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


// ********************************************************************
// 声明：static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (hWnd)hToolTips; 
//    IN lParam = 0; 
// 返回值：
//	无
// 功能描述：设置工具提示窗口句柄，处理TBM_SETTOOLTIPS消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


// ********************************************************************
// 声明：static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = 0; 
//    IN lParam = 0; 
// 返回值：
//	返回工具提示窗口句柄
// 功能描述：得到工具提示窗口句柄，处理TBM_GETTOOLTIPS消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoSetTipSide(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (int)iLocation; 
//    IN lParam = 0; 
// 返回值：
//	返回上一次的位置
// 功能描述：设置工具提示窗口的位置，处理TBM_SETTIPSIDE消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetTipSide(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


// ********************************************************************
// 声明：static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (int)iLocation; 
//    IN lParam = (HWND)hBuddy; 
// 返回值：
// 	返回上一次的绑定的窗口
// 功能描述：设置绑定窗口，处理TBM_SETBUDDY消息。
// 引用: 
// ********************************************************************
static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN wParam = (int)iLocation; 
//    IN lParam = 0; 
// 返回值：
//	返回绑定的窗口
// 功能描述：得到绑定窗口，处理TBM_GETBUDDY消息。
// 引用: 
// ********************************************************************
static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}




// **************************************************
// 声明：static void ResetSlider(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：重设滑块。
// 引用: 
// **************************************************
static void ResetSlider(HWND hWnd)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // 得到Slider类结构
		ASSERT(lpSliderCtrlData);

		GetClientRect(hWnd,&rect); // 得到客户矩形

		lpSliderCtrlData->iAutoTickNum = ((lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin + (lpSliderCtrlData->wTicFreq -1) ) / lpSliderCtrlData->wTicFreq) -1; // -1 ， 不需要画最后一条线
		lpSliderCtrlData->iTickInc = (rect.right - rect.left - HORZBANK * 2) / (lpSliderCtrlData->iAutoTickNum + 1); // 将当前的长度等样划分
		lpSliderCtrlData->iSliderlength = lpSliderCtrlData->iTickInc * (lpSliderCtrlData->iAutoTickNum + 1); // 每一份的长度 * 份数
		lpSliderCtrlData->iHorzBank = (rect.right - rect.left - lpSliderCtrlData->iSliderlength) / 2; // 总长度 - Slider的长度，然后两边等份
}

