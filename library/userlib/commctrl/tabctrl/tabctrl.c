/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：Tab类，系统类部分
版本号：1.0.0
开发时期：2003-06-23
作者：陈建明 Jami chen
修改记录：
		2004.5. 27 添加功能 
		TCS_EX_AUTOHIDETEXT -- 自动隐藏文本
		TCS_EX_PLATSHOWTEXT -- 平面显示
		TCS_EX_AUTOSIZE -- 自动调整条目尺寸
******************************************************/
#include <eWindows.h>
#include <TabCtrl.h>

/***************  全局区 定义， 声明 *****************/

#define IMAGE_WIDTH  24

#define COLOR_UNACTIVEBK COLOR_SCROLLBARTHUMB

static const char classTabCtrl[] = "TabControl";

typedef struct{
	LPTSTR lpText;
	UINT iImage;
	DWORD dwState;
	LPARAM lParam;
	UINT iWidth;
	HIMAGELIST hImageList ;
}TABITEMDATA,*LPTABITEMDATA;

typedef struct{
	HIMAGELIST hImageList;
	LPPTRLIST lpTabData;
	int iDisplayIndex;
	int iCurSel;
	UINT iRow;
	UINT iMaxRow;
	UINT *pRowOrder; // 确定一个条目显示在哪一行，哪一列

	BOOL bScrollArrow;

	COLORREF cl_Text;
	COLORREF cl_Bk;

	int iTabItemHeight; // 
	SIZE sizeIcon; //  ICON的高度

	HBITMAP hBackGround; // 背景位图
}TABCTRLDATA , *LPTABCTRLDATA;


#define LEFTARROW	0x0001
#define RIGHTARROW  0x0002

#define ARROWWIDTH	10
//#define lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/  26

#define NOTINITEM		-3
#define LEFTARROWITEM	-2
#define RIGHTARROWITEM	-1

//#define HIDEITEMWIDTH	40
#define HIDEITEMWIDTH	50
// ********************************************************************
// 函数声明
// ********************************************************************

ATOM RegisterTabCtrlClass(HINSTANCE hInstance);
static LRESULT CALLBACK TabCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemExtra(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoAdjustRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoRemoveImage(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetPadding(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetRowCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetMinTabWidth(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeselectAll(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoHighlight(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetExtenedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetExtendedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetBackGround(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight,int xOffset,int yOffset);

// *************************************************************************/
static void ScrollArrowNeed(HWND hWnd);

static void ShowArrow(HWND hWnd,HDC hdc,int cx,DWORD dwArrowStyle);
static void ShowItem(HWND hWnd,HDC hdc,LPTABITEMDATA lpTabItemData,int cx,int width,BOOL bCurSel);

static UINT GetItemWidth(HWND hWnd , LPTABITEMDATA lpTabItemData);
static UINT GetStringWidth(HWND hWnd,LPTSTR lpString);

static int GetCurrentItem(HWND hWnd,POINTS points);
static void ScrollTab(HWND hWnd,int iNewItem);
static void SetNewCurItem(HWND hWnd,int iNewItem);
static BOOL CanRightScroll(HWND hWnd);
static  LRESULT SendNormalNotify(HWND hWnd,UINT iCode);

static void AdjustItemSize(HWND hWnd,LPTABCTRLDATA lpTabCtrlData);

// ********************************************************************
// 声明：ATOM RegisterTabCtrlClass(HINSTANCE hInstance)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
// 返回值：
//	成功，返回非零，不成功，返回零。
// 功能描述：注册当前应用程序的类
// 引用: 被 应用程序入口程序 调用
// ********************************************************************
ATOM RegisterTabCtrlClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)TabCtrlWndProc; // TAB类窗口过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(LONG);
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
//    wc.hbrBackground = GetSysColorBrush( COLOR_BTNFACE );//GetStockObject( LTGRAY_BRUSH );
//    wc.hbrBackground = GetStockObject( LTGRAY_BRUSH );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classTabCtrl; // TAB类名

	return RegisterClass(&wc); // 注册窗口
}


// ********************************************************************
// 声明：static LRESULT CALLBACK TabCtrlWndProc(HWND , UINT , WPARAM , LPARAM )
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	不同的消息有不同的返回值，具体看消息本身
// 功能描述：文件浏览类窗口过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK TabCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // 绘制窗口
			hdc = BeginPaint(hWnd, &ps);
			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_ERASEBKGND: // 删除背景
			OnEraseBkgnd( hWnd,( HDC )wParam );
			return 0;
		case WM_COMMAND: // 窗口命令
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
		case WM_CREATE: // 窗口创建
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // 窗口破坏
			return DoDestroyWindow(hWnd);
		case WM_SETCTLCOLOR	: // 设置颜色
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // 得到控件颜色
			return DoGetColor(hWnd,wParam,lParam);
// !!! Add By Jami chen in 2004.07.19
		case WM_SYSCOLORCHANGE:
			return DoSysColorChanged(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2004.07.19

		case TCM_GETITEM: // 得到条目
			return DoGetItem(hWnd,wParam,lParam);
		case TCM_SETITEM: // 设置条目
			return DoSetItem(hWnd,wParam,lParam);
		case TCM_INSERTITEM: // 在TAB控件中插入一个条目
			return DoInsertItem(hWnd,wParam,lParam);
		case TCM_DELETEITEM: // 删除条目
			return DoDeleteItem(hWnd,wParam,lParam);
		case TCM_DELETEALLITEMS: // 删除所有的条目
			return DoDeleteAllItems(hWnd,wParam,lParam);
		case TCM_GETITEMRECT: // 得到条目矩形
			return DoGetItemRect(hWnd,wParam,lParam);
		case TCM_GETCURSEL: // 得到当前选择
			return DoGetCurSel(hWnd,wParam,lParam);
		case TCM_SETCURSEL: // 设置当前选择
			return DoSetCurSel(hWnd,wParam,lParam);
		case TCM_HITTEST: // 测试点
			return DoHitTest(hWnd,wParam,lParam);
		case TCM_SETITEMEXTRA: // 设置条目扩展数据
			return DoSetItemExtra(hWnd,wParam,lParam);
		case TCM_ADJUSTRECT:// 调整矩形
			return DoAdjustRect(hWnd,wParam,lParam);
		case TCM_SETITEMSIZE:// 设置条目尺寸
			return DoSetItemSize(hWnd,wParam,lParam);
		case TCM_REMOVEIMAGE:// 删除图象
			return DoRemoveImage(hWnd,wParam,lParam);
		case TCM_SETPADDING: // 设置空白
			return DoSetPadding(hWnd,wParam,lParam);
		case TCM_GETROWCOUNT:// 得到行的总数
			return DoGetRowCount(hWnd,wParam,lParam);
		case TCM_GETTOOLTIPS: // 得到工具提示
				return DoGetToolTips(hWnd,wParam,lParam);
		case TCM_SETTOOLTIPS: // 设置工具提示
			return DoSetToolTips(hWnd,wParam,lParam);
		case TCM_GETCURFOCUS:// 得到当前焦点
			return DoGetCurFocus(hWnd,wParam,lParam);
		case TCM_SETCURFOCUS:// 设置当前焦点
			return DoSetCurFocus(hWnd,wParam,lParam);
		case TCM_SETMINTABWIDTH: // 设置最小的TAB宽度
			return DoSetMinTabWidth(hWnd,wParam,lParam);
		case TCM_DESELECTALL:// 清除所有选择
			return DoDeselectAll(hWnd,wParam,lParam);
		case TCM_HIGHLIGHTITEM:// 高亮显示条目
			return DoHighlight(hWnd,wParam,lParam);
		case TCM_SETEXTENDEDSTYLE:// 设置扩展风格
			return DoSetExtenedStyle(hWnd,wParam,lParam);
		case TCM_GETEXTENDEDSTYLE: // 得到扩展风格
			return DoGetExtendedStyle(hWnd,wParam,lParam);
		case TCM_SETIMAGELIST: //设置图象列表
			return DoSetImageList(hWnd,wParam,lParam);

		case TCM_SETBACKGROUND: //设置图象列表
			return DoSetBackGround(hWnd,wParam,lParam);


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
	RECT rect;	
	LPTABCTRLDATA lpTabCtrlData;

	lpTabCtrlData = (LPTABCTRLDATA)malloc(sizeof(TABCTRLDATA)); // 分配TAB结构
	if (lpTabCtrlData == NULL)
		return -1; // 不能创建该窗口

	// 初始化结构
	lpTabCtrlData->hImageList = NULL;
	lpTabCtrlData->lpTabData = (LPPTRLIST)malloc(sizeof(PTRLIST)); // 分配TAB数据列表结构
	if (lpTabCtrlData->lpTabData == NULL)
		return -1;
	PtrListCreate(lpTabCtrlData->lpTabData, 8, 8 ) ; // 创建数据列表
	lpTabCtrlData->iDisplayIndex = 0;
	lpTabCtrlData->iCurSel = -1;
	lpTabCtrlData->iRow = 0;
	lpTabCtrlData->iMaxRow = 4;
	lpTabCtrlData->pRowOrder = (LPUINT)malloc(sizeof(UINT) * lpTabCtrlData->iMaxRow);  // 分配行次序数组
	memset(lpTabCtrlData->pRowOrder,0,sizeof(UINT) * lpTabCtrlData->iMaxRow); // 初始化数组

	lpTabCtrlData->bScrollArrow = FALSE;

	// 设置TAB颜色
	lpTabCtrlData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);; 
//	lpTabCtrlData->cl_Bk  = RGB(0xC0,0xC0,0xC0);//GetSysColor(COLOR_UNACTIVEBK);
	lpTabCtrlData->cl_Bk  = GetSysColor(COLOR_UNACTIVEBK);

	GetClientRect(hWnd,&rect);
	lpTabCtrlData->iTabItemHeight = rect.bottom - rect.top;

	lpTabCtrlData->sizeIcon.cx = 16;  //设置默认ICON的大小
	lpTabCtrlData->sizeIcon.cy = 16;

	lpTabCtrlData->hBackGround = NULL;


	SetWindowLong(hWnd,0,(DWORD)lpTabCtrlData);
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
	LPTABCTRLDATA lpTabCtrlData;
	lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
	ASSERT(lpTabCtrlData);

	if (lpTabCtrlData->hImageList != NULL)
	{
		// 原来已经存在图象列表
		ImageList_Destroy(lpTabCtrlData->hImageList);
	}

	free(lpTabCtrlData);
	SetWindowLong(hWnd,0,0);
	return TRUE;
}

// ********************************************************************
// 声明：static LRESULT DoPaint(HWND hWnd,HDC hdc)
// 参数：
//	IN hWnd - 窗口句柄
//    IN hdc - 设备句柄
// 返回值：
//	
// 功能描述：WM_PAINT 处理过程
// 引用: 
// ********************************************************************
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	DWORD dwStyle,dwExStyle;
	int cx = 0,width =0,iBankWidth = 0;
	int iIndex;
	RECT rect;
	HPEN hPen;
	HBRUSH hBrush;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);

		SetTextColor(hdc,lpTabCtrlData->cl_Text); // 设置文本颜色
		SetBkColor(hdc,lpTabCtrlData->cl_Bk); // 设置背景颜色
//		hPen = CreatePen(PS_SOLID,1,lpTabCtrlData->cl_Text);
		hPen = CreatePen(PS_SOLID,1,RGB(100,100,100));
		hPen = (HPEN)SelectObject(hdc,hPen);
		hBrush = CreateSolidBrush(lpTabCtrlData->cl_Bk);
		hBrush = (HBRUSH)SelectObject(hdc,hBrush);

		GetClientRect(hWnd,&rect); // 得到窗口客户矩形
		iBankWidth = rect.right - rect.left;
		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		
		if (dwStyle & TCS_MULTILINE)
		{// 可以显示多行
		}
		else
		{// 只能显示一行
			MoveToEx(hdc,0,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/,NULL);
			LineTo(hdc,iBankWidth,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/);
			
			MoveToEx(hdc,0,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/+1,NULL);
			LineTo(hdc,iBankWidth,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/+1);
			
			iIndex = lpTabCtrlData->iDisplayIndex;  // 得到开始显示的索引
			if (lpTabCtrlData->bScrollArrow)
			{
				// 不是从第一个开始显示，要显示一个可以向左滚动的箭头
				ShowArrow(hWnd,hdc,cx,LEFTARROW);
				cx += ARROWWIDTH;
				iBankWidth -= 2 * ARROWWIDTH;  //可供条目显示的宽度必须除去左右箭头的宽度
			}
			while(1)
			{ // 显示每一个条目
				lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);
				if (lpTabItemData == NULL)
					break; //已经没有数据
				if (dwExStyle & TCS_EX_AUTOHIDETEXT)
				{ // 可以自动隐藏文本
					if (iIndex == lpTabCtrlData->iCurSel)
						width = iBankWidth - HIDEITEMWIDTH * (lpTabCtrlData->lpTabData->count - 1);
					else
						width = HIDEITEMWIDTH;
				}
				else
				{
					width = lpTabItemData->iWidth;
				}
				
				if (lpTabCtrlData->bScrollArrow == TRUE)
				{  
					if (width > iBankWidth)
					{  //不能完整显示最后一个条目
						width = iBankWidth;
					}
				}
				if (width < 0)
				{	// 还有条目没显示，需要右箭头
					break;
				}
				if (iIndex == lpTabCtrlData->iCurSel)
					ShowItem(hWnd,hdc,lpTabItemData,cx,width,TRUE);
				else
					ShowItem(hWnd,hdc,lpTabItemData,cx,width,FALSE);
				cx += width;
				if (!(dwExStyle & TCS_EX_AUTOHIDETEXT))
				{
					iBankWidth -=width;
				}
				iIndex ++ ;
				if (iIndex >=lpTabCtrlData->lpTabData->count)
					break;
			}
			if (lpTabCtrlData->bScrollArrow == TRUE)
			{  // 需要显示右箭头
				ShowArrow(hWnd,hdc,cx,RIGHTARROW);
			}
		}
		hPen = (HPEN)SelectObject(hdc,hPen); // 恢复原来笔
		DeleteObject(hPen); // 删除笔
		//hBrush = CreateSolidBrush(lpTabCtrlData->cl_Bk);
		hBrush = (HBRUSH)SelectObject(hdc,hBrush); // 恢复原来的画刷
		DeleteObject(hBrush); // 删除刷子
		return 0;
}

// ********************************************************************
// 声明：static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc)
// 参数：
//	IN hWnd - 窗口句柄
//    IN hdc - 设备句柄
// 返回值：
//	
// 功能描述：WM_ERASEBKGND 处理过程
// 引用: 
// ********************************************************************
static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc)
{
	LPTABCTRLDATA lpTabCtrlData;
	HBRUSH hBrush;
	RECT rect;
		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0);  // 得到TAB结构
		ASSERT(lpTabCtrlData);

		hBrush = CreateSolidBrush(lpTabCtrlData->cl_Bk); // 得到背景刷
		if (hBrush == NULL)
			return 0;
		GetClientRect(hWnd,&rect); // 得到客户矩形
		FillRect(hdc,&rect,hBrush); // 填充客户矩形
		DeleteObject(hBrush); // 删除画刷
		return 0;
}

// **************************************************
// 声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(x,y),当前鼠标的位置
// 返回值：无
// 功能描述：处理WM_LBUTTONDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINTS points;
	int iNewItem;

		// 得到点的位置
		points.x=LOWORD(lParam);
		points.y=HIWORD(lParam);

		iNewItem = GetCurrentItem(hWnd,points); // 得到点所在的条目
		switch(iNewItem)
		{
			case NOTINITEM:
				return 0; // 没有点在任何条目上
			case LEFTARROWITEM:
			case RIGHTARROWITEM:// 点在左右箭头上
				ScrollTab(hWnd,iNewItem); // 滚动TAB
				break;
			default: // 点在条目上
				SetNewCurItem(hWnd,iNewItem); // 设置新的条目为当前选择条目
				break;
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPCTLCOLORSTRUCT，要设置的颜色值
// 返回值：无
// 功能描述：设置窗口颜色，处理WM_SETCTLCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);
		
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构指针

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 设置文本颜色
			 lpTabCtrlData->cl_Text = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 设置文本背景色
			 lpTabCtrlData->cl_Bk = lpCtlColor->cl_TextBk;
		 }
		return TRUE;
}
// **************************************************
// 声明：static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPCTLCOLORSTRUCT，存放得到的颜色值
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到当前窗口的颜色，处理WM_GETCTLCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构指针

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 得到文本颜色
			 lpCtlColor->cl_Text = lpTabCtrlData->cl_Text ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 得到文本背景色
				lpCtlColor->cl_TextBk = lpTabCtrlData->cl_Bk ;
		 }
		 return TRUE;
}

// !!! Add By Jami chen in 2004.07.19
// **************************************************
// 声明：static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值： 无
// 功能描述：系统颜色改变，处理WM_SYSCOLORHANGED消息。
// 引用: 
// **************************************************
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);

		lpTabCtrlData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);; 
//		lpTabCtrlData->cl_Bk  = RGB(0xC0,0xC0,0xC0);//GetSysColor(COLOR_STATIC);
		lpTabCtrlData->cl_Bk  = GetSysColor(COLOR_UNACTIVEBK);
	
		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
// !!! Add End By Jami chen in 2004.07.19

// **************************************************
// 声明：static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：得到条目，处理TCM_GETITEM消息。
// 引用: 
// **************************************************
static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 设置条目，处理TCM_SETITEM消息。
// 引用: 
// **************************************************
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//    IN wParam = (WPARAM) (int) iItem; 指定要插入的位置。
//    IN lParam = (LPARAM) (const LPTCITEM) pitem;  要插入条目的内容。
// 返回值：
//	成功返回插入条目的索引值，否则返回-1。
// 功能描述：在TAB控件中插入一个条目，处理TCM_INSERTITEM消息。
// 引用: 
// ********************************************************************
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	LPTCITEM lpTCItem;
	int iItem;

		lpTCItem = (LPTCITEM)lParam; // 得到条目结构参数
		if (lpTCItem == NULL)
			return -1;
		iItem = (int)wParam;
		if (iItem < 0)
			return -1;
		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);

		lpTabItemData = (LPTABITEMDATA)malloc(sizeof(TABITEMDATA)); // 分配一个条目数据
		if (lpTabItemData == NULL)
			return -1;
		
		if (lpTCItem->mask & TCIF_TEXT)
		{ // 结构文本有效
			lpTabItemData->lpText = (LPTSTR)malloc(strlen(lpTCItem->pszText) + 1); // 分配一个缓存来存放文本
			strcpy(lpTabItemData->lpText,lpTCItem->pszText); // 复制文本
		}
		else
		{
			lpTabItemData->lpText = NULL; // 没有文本
		}
		
		if (lpTCItem->mask & TCIF_IMAGE)
		{ // 图象有效
			lpTabItemData->iImage = lpTCItem->iImage; // 设置图象
		}
		else
		{ // 没有图象
			lpTabItemData->iImage = -1;
		}
		if (lpTCItem->mask & TCIF_PARAM)
		{ // 参数有效
			lpTabItemData->lParam = lpTCItem->lParam; // 设置参数
		}
		else
		{ // 没有参数
			lpTabItemData->lParam = 0;
		}
		lpTabItemData->dwState = 0;

		lpTabItemData->iWidth = GetItemWidth(hWnd,lpTabItemData); // 得到条目宽度
//		RETAILMSG(1,("The Tab item < %s> Width = %d \r\n",lpTabItemData->lpText,lpTabItemData->iWidth));

		if (iItem > lpTabCtrlData->lpTabData->count)
			iItem = lpTabCtrlData->lpTabData->count;
//		RETAILMSG(1,("Insert %d Data \r\n",iItem));
		if (PtrListAtInsert(lpTabCtrlData->lpTabData,iItem,lpTabItemData) ==TRUE) // 插入条目
//		if (PtrListInsert(lpTabCtrlData->lpTabData,lpTabItemData) ==TRUE)
		{
			// 插入成功
			DWORD dwExStyle;
//			RETAILMSG(1,("Insert success \r\n"));
			if (lpTabCtrlData->iCurSel == -1) // 没有选择条目
				lpTabCtrlData->iCurSel = iItem; // 设置当前条目为选择条目
			// !!! Add By Jami chen for adjust the item width

			dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
			if (TCS_EX_AUTOSIZE & dwExStyle)
			{
				// 调整TAB条目的宽度
				AdjustItemSize(hWnd,lpTabCtrlData);
			}
			// !!! Add End
			ScrollArrowNeed(hWnd);// 设定是否需要滚动箭头
			return iItem; // 返回插入条目的位置
		}
//		RETAILMSG(1,("Insert Failure \r\n"));
		return -1;
}
// **************************************************
// 声明：static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：删除条目，处理TCM_DELETEITEM消息。
// 引用: 
// **************************************************
static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：删除所有条目，处理TCM_DELETEALLITEMS消息。
// 引用: 
// **************************************************
static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 得到条目矩形 ， 处理TCM_GETITEMRECT消息。
// 引用: 
// **************************************************
static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：返回当前的选择
// 功能描述： 得到当前选择，处理TCM_GETCURSEL消息。
// 引用: 
// **************************************************
static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);
		return lpTabCtrlData->iCurSel; // 返回当前的选择
}
// **************************************************
// 声明：static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：设置当前选择，处理TCM_SETCURSEL消息。
// 引用: 
// **************************************************
static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iItem;

		iItem = (int)wParam;  // 得到新的条目
		SetNewCurItem(hWnd,iItem); // 设置新的条目为当前选择条目
		return 0;
}
// **************************************************
// 声明：static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 测试点，处理TCM_HITTEST消息。
// 引用: 
// **************************************************
static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetItemExtra(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 设置条目扩展数据，处理TCM_SETITEMEXTRA消息。
// 引用: 
// **************************************************
static LRESULT DoSetItemExtra(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoAdjustRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 调整矩形，处理TCM_ADJUSTRECT消息。
// 引用: 
// **************************************************
static LRESULT DoAdjustRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetItemSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 设置条目尺寸，处理TCM_SETITEMSIZE消息。
// 引用: 
// **************************************************
static LRESULT DoSetItemSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoRemoveImage(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：删除图象，处理TCM_REMOVEIMAGE消息。
// 引用: 
// **************************************************
static LRESULT DoRemoveImage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetPadding(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 设置空白，处理TCM_SETPADDING消息。
// 引用: 
// **************************************************
static LRESULT DoSetPadding(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoGetRowCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 得到行的总数，处理TCM_GETROWCOUNT消息。
// 引用: 
// **************************************************
static LRESULT DoGetRowCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：得到工具提示，处理TCM_GETTOOLTIPS消息。
// 引用: 
// **************************************************
static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 设置工具提示，处理TCM_SETTOOLTIPS消息。
// 引用: 
// **************************************************
static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoGetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：得到当前焦点，处理TCM_GETCURFOCUS消息。
// 引用: 
// **************************************************
static LRESULT DoGetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 设置当前焦点，处理TCM_SETCURFOCUS消息。
// 引用: 
// **************************************************
static LRESULT DoSetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetMinTabWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 设置最小的TAB宽度，处理TCM_SETMINTABWIDTH消息。
// 引用: 
// **************************************************
static LRESULT DoSetMinTabWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoDeselectAll(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 清除所有选择，处理TCM_DESELECTALL消息。
// 引用: 
// **************************************************
static LRESULT DoDeselectAll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoHighlight(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 高亮显示条目，处理TCM_HIGHLIGHTITEM消息。
// 引用: 
// **************************************************
static LRESULT DoHighlight(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoSetExtenedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 设置扩展风格，处理TCM_SETEXTENDEDSTYLE消息。
// 引用: 
// **************************************************
static LRESULT DoSetExtenedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// 声明：static LRESULT DoGetExtendedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 得到扩展风格，处理TCM_GETEXTENDEDSTYLE消息。
// 引用: 
// **************************************************
static LRESULT DoGetExtendedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


// **************************************************
// 声明：static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- (HIMAGELIST) himl 图象列表句柄
// 
// 返回值：无
// 功能描述： 得到扩展风格，处理TCM_GETEXTENDEDSTYLE消息。
// 引用: 
// **************************************************
static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;
	HIMAGELIST hOldImageList;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);
		
		hOldImageList = lpTabCtrlData->hImageList; // 得到旧的图象列表

		lpTabCtrlData->hImageList = (HIMAGELIST)lParam; // 设置新的图象列表

		ImageList_GetIconSize(lpTabCtrlData->hImageList,&lpTabCtrlData->sizeIcon.cx,&lpTabCtrlData->sizeIcon.cy); // 得到ICON的大小


		return (LRESULT)hOldImageList; //返回旧的图象列表
}



// **************************************************
// 声明：static LRESULT DoSetBackGround(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- (HIMAGELIST) himl 图象列表句柄
// 
// 返回值：无
// 功能描述： 得到扩展风格，处理TCM_GETEXTENDEDSTYLE消息。
// 引用: 
// **************************************************
static LRESULT DoSetBackGround(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;
	HBITMAP hBackGround,hOldBackGround;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);

		hOldBackGround = lpTabCtrlData->hBackGround; // 得到原来的背景位图

		hBackGround = (HBITMAP)lParam; // 得到新的背景位图
	
		lpTabCtrlData->hBackGround = hBackGround; // 设置新的背景位图
	
		return (LRESULT)hOldBackGround; //返回原来的背景位图
}

// **************************************************
// 声明：static void ShowArrow(HWND hWnd,HDC hdc,int cx,DWORD dwArrowStyle)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN cx -- 箭头位置
// 	IN dwArrowStyle -- 箭头风格
// 
// 返回值：无
// 功能描述：显示箭头。
// 引用: 
// **************************************************
static void ShowArrow(HWND hWnd,HDC hdc,int cx,DWORD dwArrowStyle)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);

		if (dwArrowStyle == LEFTARROW)
		{ // 显示左箭头
			Rectangle(hdc,cx,0,cx+ARROWWIDTH,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/); // 绘制箭头矩形
			TextOut(hdc,cx+1,1,"<",1); // 显示箭头
		}
		else if (dwArrowStyle == RIGHTARROW)
		{ // 显示右箭头
			Rectangle(hdc,cx,0,cx+ARROWWIDTH,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/); // 绘制箭头矩形
			TextOut(hdc,cx+1,1,">",1); // 显示箭头
		}
}

// **************************************************
// 声明：static void ShowItem(HWND hWnd,HDC hdc,LPTABITEMDATA lpTabItemData,int cx,int width,BOOL bCurSel)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpTabItemData -- 条目数据结构
// 	IN cx -- 条目位置
// 	IN width --条目宽度
// 	IN bCurSel -- 是否是当前条目
// 
// 返回值：无
// 功能描述：显示条目。
// 引用: 
// **************************************************
static void ShowItem(HWND hWnd,HDC hdc,LPTABITEMDATA lpTabItemData,int cx,int width,BOOL bCurSel)
{
	RECT rect;
	HBRUSH hBrush;
	LPTABCTRLDATA lpTabCtrlData;
	int iTop = 0;
	DWORD dwExStyle,dwStyle;
	COLORREF cl_Bk;
	HICON hIcon;
	int iItemWidth; //有内容的条目宽度
	UINT uFormat;
	

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构

		ASSERT(lpTabCtrlData);		
		dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		if (dwExStyle & TCS_EX_PLATSHOWTEXT)
		{ // 可以自动隐藏文本

			if (bCurSel)
//				cl_Bk = RGB(255,255,255); // 是选择条目
				cl_Bk = GetSysColor(COLOR_WINDOW); // 是选择条目
			else
				cl_Bk = lpTabCtrlData->cl_Bk;  // 不是选择条目

			// Add By Jami chen in 2004.08.14
			if (cx > 0)
			{
				cx --; // 左边界与前一个条目的右边界重合
				width ++; 
			}
			// Add End By Jami chen in 2004.08.14

			// 绘制矩形框
			Rectangle(hdc,cx,iTop,cx+width,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/); // 绘制圆角矩形
			// 得到当前条目的矩形

			rect.left = cx + 1;
			rect.right = cx + width - 2;
			rect.top = iTop + 1;
			rect.bottom = rect.top + lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/ - 2;

			if (lpTabCtrlData->hBackGround && bCurSel)
			{
				DrawBitmap(hdc,lpTabCtrlData->hBackGround,rect.left,iTop,width-2,lpTabCtrlData->iTabItemHeight-1,rect.left,iTop);
				SetBkMode(hdc,TRANSPARENT);
			}
			else
			{
				hBrush = CreateSolidBrush(cl_Bk);
				FillRect(hdc,&rect,hBrush); // 清除条目下部的线条
				DeleteObject(hBrush);
			}

			// 设置要显示的内容的位置
			if (dwStyle & (TCS_FORCEICONLEFT | TCS_FORCELABELLEFT))
			{  //ICON 显示在左边
//				rect.left = cx + 1;
//				rect.right = cx + width - 2;
			}
			else
			{  // ICON 和文本显示在中央
				if (dwExStyle & TCS_EX_AUTOHIDETEXT)
				{// 隐藏文本，仅仅显示图象
					if (bCurSel)
						iItemWidth = GetItemWidth(hWnd,lpTabItemData); // 得到条目内容的宽度
					else
						iItemWidth = lpTabCtrlData->sizeIcon.cx; // 仅显示图标

					rect.left += (width - iItemWidth)/2; // 将内容设到中央
					rect.right = rect.left + iItemWidth;
				}
				else
				{
					iItemWidth = GetItemWidth(hWnd,lpTabItemData); // 得到条目内容的宽度
					rect.left += (lpTabItemData->iWidth- iItemWidth)/2; // 将文本设到中央
					rect.right = rect.left + iItemWidth - 2;
				}
			}

			if (lpTabItemData->iImage != -1)
			{
				if (lpTabCtrlData->hImageList)
				{
					int cx,cy;

					hIcon = ImageList_GetIcon(lpTabCtrlData->hImageList,lpTabItemData->iImage,0); // 装载指定的图标
					if (hIcon == NULL)
						return ;

					// 设置ICON的显示位置
					cx = rect.left;
					cy = ((rect.bottom - rect.top ) - lpTabCtrlData->sizeIcon.cy) / 2 + rect.top; // 在垂直位置上将ICON显示在控件的中间

					DrawIconEx(hdc,
							cx,
							cy,
							hIcon,
							0,
							0,
							0,
							0,
							DI_NORMAL);
					
					rect.left += lpTabCtrlData->sizeIcon.cx + 1; // 必须要有间隔
				}
			}

			// 开始显示文本
			if (dwStyle & (TCS_FORCEICONLEFT))
			{
				// 要求将文本显示在中央
				uFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER ;
			}
			else
			{
				// 要求文本紧接着ICON显示
				uFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER ;
			}
			if (bCurSel)
			{ // 如果是当前条目，则显示文本，否则已经隐藏文本
				if (lpTabItemData->lpText)
				{
					cl_Bk = SetBkColor(hdc,cl_Bk); // 设置新的背景颜色
					//TextOut(hdc,cx+1,iTop+2,lpTabItemData->lpText,strlen(lpTabItemData->lpText)); // 绘制条目文本
					DrawText(hdc, lpTabItemData->lpText,strlen(lpTabItemData->lpText), &rect, uFormat);//绘制条目文本
					SetBkColor(hdc,cl_Bk); // 恢复旧的背景颜色
				}
			}
			else
			{
				//不是选择条目
				if (!(dwExStyle & TCS_EX_AUTOHIDETEXT))
				{
					//如果是隐藏文本，则不显示文本，否则显示文本
					if (lpTabItemData->lpText)
					{
						cl_Bk = SetBkColor(hdc,cl_Bk); // 设置新的背景颜色
						//TextOut(hdc,cx+1,iTop+2,lpTabItemData->lpText,strlen(lpTabItemData->lpText)); // 绘制条目文本
						DrawText(hdc, lpTabItemData->lpText,strlen(lpTabItemData->lpText), &rect, uFormat);//绘制条目文本
						SetBkColor(hdc,cl_Bk); // 恢复旧的背景颜色
					}
				}
			}
		}
		else
		{

			if (bCurSel)
			{ // 是当前选择，会显示的比较高
				iTop = 2;
			}
			else
			{ // 是一般条目
				iTop = 4;
			}
	//		RoundRect(hdc,cx,0,cx+width,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/+4,8,8);
			RoundRect(hdc,cx,iTop,cx+width,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/+4,8,8); // 绘制圆角矩形
			rect.left = cx;
			rect.top = lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/;
			rect.right = cx + width;
			rect.bottom = lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/ + 4;

			hBrush = CreateSolidBrush(lpTabCtrlData->cl_Bk);
			FillRect(hdc,&rect,hBrush); // 清除条目下部的线条
			DeleteObject(hBrush);
	//		if (lpTabItemData->iImage)
	//			TextOut(hdc,cx+3,3,lpTabItemData->lpText,strlen(lpTabItemData->lpText));
			if (lpTabItemData->lpText)
				TextOut(hdc,cx+3,3 + iTop,lpTabItemData->lpText,strlen(lpTabItemData->lpText)); // 绘制条目文本
			if (bCurSel == FALSE)
			{ // 如果不是当前选择，则下端有横线
				MoveToEx(hdc,cx,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/,NULL);
				LineTo(hdc,cx + width,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/);
			}
		}
}

// **************************************************
// 声明：static UINT GetItemWidth(HWND hWnd , LPTABITEMDATA lpTabItemData)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpTabItemData -- 条目数据结构
// 
// 返回值：返回条目的宽度
// 功能描述：得到指定条目的宽度
// 引用: 
// **************************************************
static UINT GetItemWidth(HWND hWnd , LPTABITEMDATA lpTabItemData)
{
	UINT iWidth = 0;

		if (lpTabItemData == NULL)
			return 0; // 条目数据无效
		iWidth += 6;// the edge and bank

		if (lpTabItemData->iImage != -1)
		{
			iWidth += IMAGE_WIDTH; // image width
			iWidth += 1;  // bank the image and text
		}
		if (lpTabItemData->lpText != NULL)
		{
			iWidth += GetStringWidth(hWnd,lpTabItemData->lpText); // 文本宽度
		}

		return iWidth; // 返回条目宽度
}

// **************************************************
// 声明：static UINT GetStringWidth(HWND hWnd,LPTSTR lpString)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpString -- 指定字串
// 
// 返回值：返回字串的宽度
// 功能描述：得到字串的宽度。
// 引用: 
// **************************************************
static UINT GetStringWidth(HWND hWnd,LPTSTR lpString)
{
	HDC hdc;
    int lpASCIICodeWidthBuffer[128];
    int ChineseCharWidth;
	UINT iWidth = 0;

		if (lpString == NULL)
			return 0;
		hdc = GetDC(hWnd); // 得到设备句柄
		if (GetCharWidth(hdc,0,0x7f,lpASCIICodeWidthBuffer)==0) // 得到ASCII字符的宽度
			goto GETSTRINGWIDTH_END;
		// get chinese character width
		if (GetCharWidth(hdc,0xb0a1,0xb0a1,&ChineseCharWidth)==0) // 得到汉字的宽度
			goto GETSTRINGWIDTH_END;

		while(*lpString)
		{ // 计算每一个字符的宽度并相加
			if (*lpString<0 || *lpString > 0x7f)
			{  // current Character is a chinese
				iWidth += ChineseCharWidth;
				lpString+=2;
			}
			else
			{  // current character is english or sambol
				iWidth +=lpASCIICodeWidthBuffer[*lpString];;
				lpString ++;
			}
		}
GETSTRINGWIDTH_END:
		ReleaseDC(hWnd,hdc); // 释放DC
		return iWidth; // 返回宽度
}

// **************************************************
// 声明：static void ScrollArrowNeed(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：确定两头的滚动箭头是否需要。
// 引用: 
// **************************************************
static void ScrollArrowNeed(HWND hWnd)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	DWORD dwStyle, dwExStyle;
	int width = 0, iBankWidth = 0;
	int iIndex;
	RECT rect;
	BOOL bRightArrow = FALSE;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);
	
		GetClientRect(hWnd,&rect);
		iBankWidth = rect.right - rect.left;
		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		if (dwStyle & TCS_MULTILINE)
		{// 可以显示多行,不会有滚动箭头存在
			lpTabCtrlData->bScrollArrow = FALSE;
		}
		else
		{// 只能显示一行
			if (dwExStyle & TCS_EX_AUTOHIDETEXT)
			{  //自动隐藏文本的控件不需要箭头
				lpTabCtrlData->bScrollArrow = FALSE;
				return ;
			}

			iIndex = lpTabCtrlData->iDisplayIndex; 
			while(1)
			{
//				RETAILMSG(1,("PtrlistAt0 %d \r\n",iIndex));
				lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);
				if (lpTabItemData == NULL)
					break; //已经没有数据
				
				width +=lpTabItemData->iWidth;
				iIndex ++ ;
				if (iIndex >=lpTabCtrlData->lpTabData->count)
					break;
			}
			if (iBankWidth >= width)
			{  // 不需要显示右箭头
				lpTabCtrlData->bScrollArrow = FALSE;
			}
			else
			{  // 需要显示右箭头
				lpTabCtrlData->bScrollArrow = TRUE;
			}
		}
		return ;
}

// **************************************************
// 声明：static int GetCurrentItem(HWND hWnd,POINTS points)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN points -- 点的位置
// 
// 返回值：返回条目的索引
// 功能描述：得到点所在的条目。
// 引用: 
// **************************************************
static int GetCurrentItem(HWND hWnd,POINTS points)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	DWORD dwStyle;
	DWORD dwExStyle;
	int width = 0, iBankWidth = 0,cx = 0;
	int iIndex;
	RECT rect;
	BOOL bRightArrow = FALSE;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);
	
		GetClientRect(hWnd,&rect); // 得到客户矩形
		iBankWidth = rect.right - rect.left;
		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		if (dwStyle & TCS_MULTILINE)
		{// 可以显示多行
		}
		else
		{// 只能显示一行
			if (dwExStyle & TCS_EX_AUTOHIDETEXT)
			{
				iIndex = 0;
				while(1)
				{
					lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex); // 得到条目数据结构
					if (lpTabItemData == NULL)
						break; //已经没有数据
					
					if (dwExStyle & TCS_EX_AUTOHIDETEXT)
					{ // 可以自动隐藏文本
						if (iIndex == lpTabCtrlData->iCurSel)
							width = iBankWidth - HIDEITEMWIDTH * (lpTabCtrlData->lpTabData->count - 1);
						else
							width = HIDEITEMWIDTH;
					}
					else
					{
						width = lpTabItemData->iWidth;
					}
					cx += width;
					if (cx > points.x)
						return iIndex; // 在当前的条目上，返回条目的索引
					iIndex ++ ; // 下一个条目
					if (iIndex >=lpTabCtrlData->lpTabData->count)
						break; // 已经没有条目了
				}
			}
			else
			{
				if (points.y > lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/)
					return NOTINITEM;
				if (lpTabCtrlData->bScrollArrow)
				{ // 允许有箭头
					if (points.x < ARROWWIDTH)
						return LEFTARROWITEM; // 左箭头位置
					if (points.x > iBankWidth - ARROWWIDTH)
						return RIGHTARROWITEM; // 右箭头位置
					cx = ARROWWIDTH;
				}
				iIndex = lpTabCtrlData->iDisplayIndex;  // 得到开始显示的索引
				while(1)
				{
	//				RETAILMSG(1,("PtrlistAt1 %d \r\n",iIndex));
					lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex); // 得到条目数据结构
					if (lpTabItemData == NULL)
						break; //已经没有数据
					
					cx +=lpTabItemData->iWidth;
					if (cx > points.x)
						return iIndex; // 在当前的条目上，返回条目的索引
					iIndex ++ ; // 下一个条目
					if (iIndex >=lpTabCtrlData->lpTabData->count)
						break; // 已经没有条目了
				}
			}
		}
		return NOTINITEM; // 返回没有条目
}
// **************************************************
// 声明：static void ScrollTab(HWND hWnd,int iNewItem)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iNewItem -- 箭头类型
// 
// 返回值：无
// 功能描述：滚动条目。
// 引用: 
// **************************************************
static void ScrollTab(HWND hWnd,int iNewItem)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);
		switch(iNewItem)
		{
			case LEFTARROWITEM: // 左箭头
				if (lpTabCtrlData->iDisplayIndex > 0)
					lpTabCtrlData->iDisplayIndex --; // 显示前一个条目
				break;
			case RIGHTARROWITEM: // 右箭头
				if (CanRightScroll(hWnd) == TRUE)
					lpTabCtrlData->iDisplayIndex ++;
				break;
			default:
				return;
		}
		InvalidateRect(hWnd,NULL,TRUE); // 无效矩形
}
// **************************************************
// 声明：static void SetNewCurItem(HWND hWnd,int iNewItem)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iNewItem -- 新的条目
// 
// 返回值：无
// 功能描述：设置新的当前条目。
// 引用: 
// **************************************************
static void SetNewCurItem(HWND hWnd,int iNewItem)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);

		if (lpTabCtrlData->iCurSel == iNewItem)
			return ; // 没有改变当前选择
		if (SendNormalNotify(hWnd,TCN_SELCHANGING) == TRUE)
		{
			//阻止设置新的当前条目
			return ;
		}
		lpTabCtrlData->iCurSel = iNewItem; // 设置新的当前选择
		InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		SendNormalNotify(hWnd,TCN_SELCHANGE); // 通知父窗口当前选择改变
}

// **************************************************
// 声明：static BOOL CanRightScroll(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：可以向右滚返回TRUE，否则返回FALSE。
// 功能描述：判断是否可以向右滚。
// 引用: 
// **************************************************
static BOOL CanRightScroll(HWND hWnd)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	DWORD dwStyle;
	int width = 0, iBankWidth = 0;
	int iIndex;
	RECT rect;
	BOOL bRightArrow = FALSE;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // 得到TAB结构
		ASSERT(lpTabCtrlData);
	
		if (lpTabCtrlData->bScrollArrow == FALSE)
			return FALSE; // 没有箭头，不能滚
		GetClientRect(hWnd,&rect); // 得到窗口客户矩形
		iBankWidth = rect.right - rect.left;
		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格

		iIndex = lpTabCtrlData->iDisplayIndex;  // 得到开始显示的索引
		while(1)
		{ // 
//			RETAILMSG(1,("PtrlistAt2 %d \r\n",iIndex));
			lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);// 得到当前条目数据
			if (lpTabItemData == NULL)
				break; //已经没有数据
			
			width +=lpTabItemData->iWidth; // 加上条目宽度
			iIndex ++ ; // 下一个条目
			if (iIndex >=lpTabCtrlData->lpTabData->count)
				break; // 已经没有条目
		}
		if (iBankWidth > width)
		{  // 不需要再向右滚动
			return FALSE;
		}
		else
		{  // 需要再向右滚动
			return TRUE;
		}
}

// **************************************************
// 声明：static  LRESULT SendNormalNotify(HWND hWnd,UINT iCode)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iCode -- 要发送的通知消息
// 
// 返回值：返回消息结果
// 功能描述：发送指定的通知消息给父窗口。
// 引用: 
// **************************************************
static  LRESULT SendNormalNotify(HWND hWnd,UINT iCode)
{
	HWND hParent;
	NMHDR hdr;

		hdr.hwndFrom=hWnd;
		hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID); // 得到窗口ID
		hdr.code=iCode;
		hParent=GetParent(hWnd); // 得到父窗口句柄
		return SendMessage(hParent,WM_NOTIFY,(WPARAM)hdr.idFrom,(LPARAM)&hdr); // 发送通知消息
}


// !!! Add By Jami chen for adjust the item width
// **************************************************
// 声明：static void AdjustItemSize(HWND hWnd,LPTABCTRLDATA lpTabCtrlData)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpTabCtrlData -- TAB控件结构指针
// 
// 返回值：无
// 功能描述：调整TAB条目的宽度。
// 引用: 
// **************************************************
				
static void AdjustItemSize(HWND hWnd,LPTABCTRLDATA lpTabCtrlData)
{
	int iControlWidth = 0;
	int iBankWidth = 0, iAddWidth;
	RECT rect;
	int iItemNum,iIndex;
	LPTABITEMDATA lpTabItemData;

		// 计算所有条目的宽度
		iIndex = 0;
		while(1)
		{ // 显示每一个条目
			lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);
			if (lpTabItemData == NULL)
				break; //已经没有数据
			lpTabItemData->iWidth = GetItemWidth(hWnd,lpTabItemData); // 得到条目宽度
			iControlWidth += lpTabItemData->iWidth;
			iIndex ++; //下一个条目
		}

		GetClientRect(hWnd,&rect); // 得到窗口客户矩形
		iBankWidth = rect.right - rect.left; // 得到窗口的宽度
		
		if (iBankWidth < iControlWidth)
		{
			// 当前控件所有条目的宽度大于窗口宽度，无法在调整宽度
			return ;
		}
		iBankWidth -= iControlWidth; //得到还剩余的宽度
		iItemNum = lpTabCtrlData->lpTabData->count;
		// 平均分配给所有的条目
		iAddWidth =  iBankWidth / iItemNum;
		iIndex = 0;
		while(1)
		{ // 显示每一个条目
			lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);
			if (lpTabItemData == NULL)
				break; //已经没有数据
			if (iIndex == ( iItemNum -1 ))
			{  // 最后一个条目，将最后剩下的宽度全部加上
				lpTabItemData->iWidth += iBankWidth ; // 得到条目宽度
			}
			else
			{
				// 不是最后一个，增加平均宽度
				lpTabItemData->iWidth += iAddWidth ; // 得到条目宽度
			}

			iBankWidth -= iAddWidth;
			iIndex ++; //下一个条目
		}
}			
// !!! Add End


// **************************************************
// 声明：static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight,int xOffset,int yOffset)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBitmap -- 位图句柄
// 	IN x -- 显示X坐标
// 	IN y -- 显示Y坐标
// 	IN nWidth -- 位图宽度
// 	IN nHeight -- 位图高度
// 
// 返回值：无
// 功能描述：绘制位图。
// 引用: 
// **************************************************
static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight,int xOffset,int yOffset)
{
	HDC hCompDC=CreateCompatibleDC(hdc );  // 创建内存DC
	HBITMAP hOldMap;

	hOldMap=(HBITMAP)SelectObject(hCompDC,(HGDIOBJ)hBitmap);  // 选择当前位图到内存DC
	BitBlt(hdc, x, y, nWidth, nHeight, hCompDC,xOffset,yOffset, SRCCOPY); // 绘制位图
	SelectObject(hCompDC,hOldMap); // 恢复内存DC的位图
	DeleteDC(hCompDC); // 删除内存DC
}
