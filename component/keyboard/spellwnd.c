/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：拼音键盘  拼写区类
版本号：1.0.0
开发时期：2004-05-21
作者：陈建明 Jami chen
设计规划：
		在显示拼写区的方法上，现在有两种方案
		1。显示区域的大小随拼写的条目个数与条目长度主动调节
		   到适当大小，使每一个条目有适当的间距即可，用PLAN_1来定义
		2。显示的区域大小不便，根据拼音条目的长度主动调节每一个条目
		   区域的大小，用PLAN_2来定义
修改记录：
******************************************************/
#include <ewindows.h>
#include <ecomctrl.h>
#include "resource.h"
#include <softkey.h>
#include "pysearch.h"



/***************  全局区 定义， 声明 *****************/

//#define PLAN_1
#define PLAN_2


const TCHAR classSpellArea[] = "SPELLAREA";


typedef struct SpellAreaStruct{
	SPELLLIST SpellList; // 当前的拼音拼写情况
	int iCurSpellPage; // 当前拼写的页索引
	int iCurSel; // 当前选择的拼写

	COLORREF cl_Text;  // 文本颜色
	COLORREF cl_Bk;  // 背景颜色
	COLORREF cl_SelBk; // 选择背景颜色
	COLORREF cl_SelText; //选择文本颜色

	HWND hOwner;
}SPELLAREA,*LPSPELLAREA;


#ifdef PLAN_1
#define SPACE_WIDTH  4
#endif
// ********************************************************************
// 函数声明
// ********************************************************************
ATOM RegisterSpellAreaWindow(HINSTANCE hInstance); // 注册手写区窗口类

static LRESULT CALLBACK SpellAreaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void DoInsertKey(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelSpell(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoClearSpell(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteKey(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetKeyNum(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void ClearSpellArea(HWND hWnd);
static void ShowSpellArea(HWND hWnd,HDC hdc);

static int GetNewSelIndex(LPSPELLAREA lpSpellArea,LPTSTR lpOldSelSpell);

#ifdef PLAN_1
static void SetCurrentSel(HWND hWnd,LPSPELLAREA lpSpellArea, int iIndex, BOOL bShow);
static void AdjustWindowSize(HWND hWnd,LPSPELLAREA lpSpellArea);
static UINT GetStringWidth(HWND hWnd,LPTSTR lpString);
static int GetItemWidth(HWND hWnd,LPSPELLAREA lpSpellArea);
static int GetSpellIndex(HWND hWnd,LPSPELLAREA lpSpellArea,POINT point);
static void ShowSpellItem(HWND hWnd,HDC hdc,LPSPELLAREA lpSpellArea, int iIndex);
#endif

#ifdef PLAN_2

#endif

// ********************************************************************
// 声明：ATOM RegisterHandWriteAreaWindow(HINSTANCE hInstance)
// 参数：
//	IN hInstance - 实例句柄
// 返回值：
// 	无
// 功能描述：注册手写区窗口类
// 引用: 
// ********************************************************************
ATOM RegisterSpellAreaWindow(HINSTANCE hInstance)
{
	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)SpellAreaWndProc;  // 手写工作区过程函数
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classSpellArea;
	return RegisterClass(&wcex); // 向系统拼写区类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK SpellAreaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	不同的消息有不同的返回值，具体看消息本身
// 功能描述：应用程序窗口过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK SpellAreaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

		case WM_LBUTTONDOWN: // 处理鼠标左键按下
			return DoLButtonDown( hWnd, wParam,lParam );
		case WM_LBUTTONUP: // 处理鼠标左键弹起
			return DoLButtonUp( hWnd, wParam,lParam );
		case WM_MOUSEMOVE: // 处理鼠标移动
			return DoMouseMove( hWnd, wParam,lParam );

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
		case WM_WINDOWPOSCHANGING: // 处理窗口位置改变

			((LPWINDOWPOS)lParam)->flags |= SWP_NOACTIVATE;
			return 0;
		case WM_CREATE: // 创建消息
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏窗口
			//PostQuitMessage(0);
			break;

		case SPM_INSERTKEY: // 插入一个键到拼写窗口
			DoInsertKey(hWnd,wParam,lParam);
			break;
		case SPM_GETSELSPELL:  // 得到当前的选择拼写
			return DoGetSelSpell(hWnd,wParam,lParam);

		case SPM_CLEARSPELL:	// 清除所有的拼写
			DoClearSpell(hWnd,wParam,lParam);
			break;

		case SPM_DELETEKEY:  // 删除一个键
			DoDeleteKey(hWnd,wParam,lParam);
			break;
		case SPM_GETKEYNUM:  // 得到当前拼写区域中键的数目
			return DoGetKeyNum(hWnd,wParam,lParam);
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
//	返回0，将继续创建窗口，返回-1，则会破坏窗口*
// 功能描述：应用程序处理创建消息
// 引用: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
	LPCREATESTRUCT lpcs ;

		lpcs = (LPCREATESTRUCT) lParam;

		lpSpellArea = (LPSPELLAREA)malloc(sizeof(SPELLAREA)); // 分配手写区结构缓存
		if (lpSpellArea == NULL)
			return -1; // 返回创建窗口失败
		
		lpcs = (LPCREATESTRUCT) lParam;

		lpSpellArea->hOwner = (HWND)lpcs->lpCreateParams;
		// 设置控件颜色

//		lpSpellArea->cl_Bk = RGB(231,242,255);
		lpSpellArea->cl_Bk = RGB(240,255,240);
		lpSpellArea->cl_Text = RGB(0,0,0);

		lpSpellArea->cl_SelBk = RGB(161,205,255);
		lpSpellArea->cl_SelText = RGB(0,0,0);
		lpSpellArea->iCurSel = 0;
		lpSpellArea->iCurSpellPage = 0;

		lpSpellArea->SpellList.iKeyNum = 0;
		lpSpellArea->SpellList.iSpellNum = 0;

		SetWindowLong(hWnd,0,(LONG)lpSpellArea); // 设置结构到窗口

		ClearSpellArea(hWnd);  // 清除手写区
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	返回0
// 功能描述：应用程序处理下笔消息
// 引用: 
// ********************************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
#ifdef PLAN_1
	LPSPELLAREA lpSpellArea; 
	POINT point;
	int iIndex;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);
	    
		// 得到笔的位置
		point.x=(short)LOWORD(lParam);
		point.y=(short)HIWORD(lParam);

		iIndex = GetSpellIndex(hWnd,lpSpellArea,point);

		if (iIndex == -1)
		{
			// 没有点到任何有效的拼写条目
			return 0;
		}
		SetCurrentSel(hWnd,lpSpellArea, iIndex,TRUE); // 设置当前的选择
#endif
		return 0;
}


// ********************************************************************
// 声明：static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	返回0
// 功能描述：应用程序处理抬笔消息
// 引用: 
// ********************************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 

		if (GetCapture()!=hWnd)
			return 0; // 没有抓住鼠标
		SetCapture(0); // 释放抓住鼠标

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);
	    
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	返回0
// 功能描述：应用程序处理移动笔消息
// 引用: 
// ********************************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 

		if (GetCapture()!=hWnd) 
			return 0; // 没有抓住鼠标

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);
	    
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
	LPSPELLAREA lpSpellArea; 

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);
		
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构指针

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 设置文本颜色
			 lpSpellArea->cl_Text = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 设置文本背景色
			 lpSpellArea->cl_Bk = lpCtlColor->cl_TextBk;
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
	LPSPELLAREA lpSpellArea; 

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构指针

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 得到文本颜色
			 lpCtlColor->cl_Text = lpSpellArea->cl_Text ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 得到文本背景色
				lpCtlColor->cl_TextBk = lpSpellArea->cl_Bk ;
		 }
		 return TRUE;
}

// ********************************************************************
// 声明：static void DoPaint(HWND hWnd,HDC hdc)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//    IN hdc  - 设备句柄
// 返回值：
//	
// 功能描述：应用程序处理 WM_PAINT 消息
// 引用: 
// ********************************************************************
static void DoPaint(HWND hWnd,HDC hdc)
{
	RECT rect;

	
		GetClientRect(hWnd,&rect);
		ShowSpellArea(hWnd,hdc);
}


// ********************************************************************
// 声明：static void ClearSpellArea(HWND hWnd)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：清除拼写区
// 引用: 
// ********************************************************************
static void ClearSpellArea(HWND hWnd)
{
	LPSPELLAREA lpSpellArea; 
	RECT rect;
	HBRUSH hBrush;
	HDC hdc;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);

		GetClientRect(hWnd,&rect);  // 得到窗口矩形
		
		hdc = GetDC(hWnd); // 得到设备句柄

		hBrush = CreateSolidBrush(lpSpellArea->cl_Bk); //创建背景刷
		FillRect(hdc,&rect,hBrush); // 用背景色清除内存DC
		DeleteObject(hBrush); //删除画刷

		ReleaseDC(hWnd,hdc); // 释放句柄

}


// ********************************************************************
// 声明：static void DoInsertKey(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam -- 保留
//  IN lParam -- KEYENTRY * , 要插入的键的指针
// 返回值：
//	
// 功能描述：插入一个键
// 引用: 
// ********************************************************************
static void DoInsertKey(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
	KEYENTRY *pKey;
	TCHAR lpOldSelSpell[MAX_SPELLNUM];
#ifdef PLAN_1
	int iIndex;
	BOOL bShow = TRUE;
#endif

//		RETAILMSG(1,(TEXT(" Enter DoInsertKey\r\n")));
		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);

		pKey = (KEYENTRY *)lParam;
		
#ifdef PLAN_1
		if (lpSpellArea->SpellList.iKeyNum == 0)
		{  // 还没有任何键,处于隐藏状态,首先让它显示
			//ShowWindow(hWnd,SW_SHOW); // 显示拼写窗口
//			SetWindowPos(hWnd,NULL,0,0,0,0,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE);  // 不改变窗口次序，不移动窗口
			bShow = FALSE;
		}
#endif

		if (lpSpellArea->SpellList.iKeyNum == 0)  
		{  // 原来还没有任何字符
			strcpy(lpOldSelSpell,""); // 得到原来的选择拼写
		}
		else
		{
			strcpy(lpOldSelSpell,lpSpellArea->SpellList.lpSpellList[lpSpellArea->iCurSel]); // 得到原来的选择拼写
		}

		if (InsertKeyToSpell(&lpSpellArea->SpellList,pKey)) // 将一个键插入到拼写列表
		{			
#ifdef PLAN_1
			if (lpSpellArea->SpellList.iKeyNum && bShow == FALSE)
			{  // 还没有任何键,处于隐藏状态,首先让它显示
				SetWindowPos(hWnd,NULL,0,0,0,0,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE);  // 不改变窗口次序，不移动窗口
			}
			lpSpellArea->iCurSel = -1 ; //设置没有选择
			iIndex = GetNewSelIndex(lpSpellArea,lpOldSelSpell);
			SetCurrentSel(hWnd,lpSpellArea, iIndex,FALSE); // 设置第一个为当前的选择
			AdjustWindowSize(hWnd,lpSpellArea); // 根据当前的拼写条目重新调整窗口大小
#endif
#ifdef PLAN_2
			HDC hdc;

			//在这里不需要显示，因为调整窗口大小后需要重新显示
			hdc = GetDC(hWnd);  // 得到设备句柄
			ShowSpellArea(hWnd,hdc);  // 显示拼写区

			ReleaseDC(hWnd,hdc); // 释放设备句柄

			PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)0,0);
#endif
		}

//		RETAILMSG(1,(TEXT(" Leave DoInsertKey\r\n")));
}


// ********************************************************************
// 声明：static LRESULT DoGetSelSpell(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam -- INT 缓存的大小
//  OUT lParam -- LPTSTR , 用来存放拼写的缓存
// 返回值：
//	 成功返回TRUE，否则返回FLASE
// 功能描述：得到当前选择的拼写串
// 引用: 
// ********************************************************************
static LRESULT DoGetSelSpell(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
	LPTSTR pSpell;
	int iMax_Length;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);

		iMax_Length = (int)wParam;
		pSpell = (LPTSTR)lParam;

		if (lpSpellArea->iCurSel < 0 || lpSpellArea->iCurSel >= lpSpellArea->SpellList.iSpellNum)
		{
			// 无效的索引，返回
			return FALSE;
		}
		
		if ((int)strlen(lpSpellArea->SpellList.lpSpellList[lpSpellArea->iCurSel]) >= iMax_Length)
		{
			// 缓存大小太小
			return FALSE;
		}
		
		strcpy(pSpell,lpSpellArea->SpellList.lpSpellList[lpSpellArea->iCurSel]);
		return TRUE;
}

// ********************************************************************
// 声明：static LRESULT DoClearSpell(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam -- 保留
//  IN lParam -- 保留
// 返回值：
//	 成功返回TRUE，否则返回FLASE
// 功能描述：清除所有的拼写串
// 引用: 
// ********************************************************************
static LRESULT DoClearSpell(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
//	LPTSTR pSpell;
//	int iMax_Length;

//		RETAILMSG(1,(TEXT(" Enter DoClearSpell\r\n")));
		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);

		lpSpellArea->SpellList.iKeyNum = 0; // 当前的键的数目为0
		lpSpellArea->SpellList.iSpellNum = 0;  // 当前的拼写组合为0
#ifdef PLAN_1
		AdjustWindowSize(hWnd,lpSpellArea);  // 调整窗口的大小
#endif
#ifdef PLAN_2
		{
			HDC hdc;

			//在这里不需要显示，因为调整窗口大小后需要重新显示
			hdc = GetDC(hWnd);  // 得到设备句柄
			ShowSpellArea(hWnd,hdc);  // 显示拼写区

			ReleaseDC(hWnd,hdc); // 释放设备句柄

			if (lpSpellArea->SpellList.iSpellNum )
				PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)0,0);
			else
				SendMessage(lpSpellArea->hOwner,SPN_NODATA,(WPARAM)0,0);
		}
#endif

//		RETAILMSG(1,(TEXT(" Leave DoClearSpell\r\n")));
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoDeleteKey(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam -- 保留
//  IN lParam -- 保留
// 返回值：
//	 成功返回TRUE，否则返回FLASE
// 功能描述：删除一个键
// 引用: 
// ********************************************************************
static LRESULT DoDeleteKey(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
#ifdef PLAN_1
	TCHAR lpOldSelSpell[MAX_SPELLNUM];
	int iIndex;
#endif
//		RETAILMSG(1,(TEXT(" Enter DoDeleteKey\r\n")));

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);


#ifdef PLAN_1
		if (lpSpellArea->SpellList.iKeyNum == 0)  
		{  // 原来还没有任何字符
// !!! modify by jami chen in 2004.11.03
//			strcpy(lpOldSelSpell,""); // 得到原来的选择拼写
			AdjustWindowSize(hWnd,lpSpellArea); //重新调整窗口大小
			return 0;
// !!! modify End by jami chen in 2004.11.03
		}
		else
		{
			strcpy(lpOldSelSpell,lpSpellArea->SpellList.lpSpellList[lpSpellArea->iCurSel]); // 得到原来的选择拼写
		}
#endif
#ifdef PLAN_2
		if (lpSpellArea->SpellList.iKeyNum == 0)  
		{  // 原来还没有任何字符
			SendMessage(lpSpellArea->hOwner,SPN_NODATA,(WPARAM)0,0);
			return 0;
		}
#endif
		lpSpellArea->SpellList.iKeyNum --; // 减少一个键
		lpSpellArea->SpellList.pKey[lpSpellArea->SpellList.iKeyNum] = NULL; // 清除最后一个键

		if (lpSpellArea->SpellList.iKeyNum)
		{  // 还有键存在，需要重新拼写
			ReSpellList(&lpSpellArea->SpellList);

#ifdef PLAN_1
			iIndex = GetNewSelIndex(lpSpellArea,lpOldSelSpell);
			SetCurrentSel(hWnd,lpSpellArea, iIndex,FALSE); // 设置第一个为当前的选择
#endif
		}
		else
		{
			// 已经没有键了
			lpSpellArea->SpellList.iSpellNum = 0;  // 清空拼写区
		}
		
#ifdef PLAN_1
		AdjustWindowSize(hWnd,lpSpellArea); //重新调整窗口大小
#endif
#ifdef PLAN_2
		{
			HDC hdc;

			//在这里不需要显示，因为调整窗口大小后需要重新显示
			hdc = GetDC(hWnd);  // 得到设备句柄
			ShowSpellArea(hWnd,hdc);  // 显示拼写区

			ReleaseDC(hWnd,hdc); // 释放设备句柄

//			PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)0,0);
			if (lpSpellArea->SpellList.iSpellNum )
				PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)0,0);
			else
				SendMessage(lpSpellArea->hOwner,SPN_NODATA,(WPARAM)0,0);
		}
#endif
//		RETAILMSG(1,(TEXT(" Leave DoDeleteKey\r\n")));
		return 0;
}
		
// ********************************************************************
// 声明：static LRESULT DoGetKeyNum(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam -- 保留
//  IN lParam -- 保留
// 返回值：
//	 成功返回当前拼写窗口中键的数目，否则返回-1
// 功能描述：得到当前拼写区域中键的数目
// 引用: 
// ********************************************************************
static LRESULT DoGetKeyNum(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);

		return lpSpellArea->SpellList.iKeyNum;
}

#ifdef PLAN_1

// ********************************************************************
// 声明：static void SetCurrentSel(HWND hWnd,LPSPELLAREA lpSpellArea, int iIndex,BOOL bShow)
// 参数：
//	IN hWnd -- 应用程序的窗口句柄
//  IN lpSpellArea -- 当前的拼写结构
//  IN iIndex -- 要将当前选择设置到的指定索引
// 返回值：
//		无
// 功能描述：设置当前的选择
// 引用: 
// ********************************************************************
static void SetCurrentSel(HWND hWnd,LPSPELLAREA lpSpellArea, int iIndex,BOOL bShow)
{
//	HWND hParent;
	int iOldSel;
	HDC hdc;

		if (iIndex < 0 || iIndex >= lpSpellArea->SpellList.iSpellNum)
		{
			// 无效的索引，返回
			return;
		}
		iOldSel = lpSpellArea->iCurSel; // 得到原来的选择
		lpSpellArea->iCurSel = iIndex; // 设置当前的选择

		if (bShow)
		{
			// 显示当前的选择条目
			hdc = GetDC(hWnd);  //得到设备句柄
			if (iOldSel != -1)
				ShowSpellItem(hWnd,hdc,lpSpellArea, iOldSel); // 恢复原来的选择显示
			ShowSpellItem(hWnd,hdc,lpSpellArea, lpSpellArea->iCurSel); // 显示当前的选择
			ReleaseDC(hWnd,hdc);  // 释放设备句柄
		}

//		hParent = GetParent(hWnd); // 得到父窗口句柄
		// 通知父窗口当前的选择发生改变
//		PostMessage(hParent,SPN_SELCHANGE,(WPARAM)iIndex,0);

		PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)iIndex,0);
}


// ********************************************************************
// 声明：static void AdjustWindowSize(HWND hWnd,LPSPELLAREA lpSpellArea)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN lpSpellArea -- 当前的拼写结构
// 返回值：
//		无
// 功能描述：根据当前的拼写条目重新调整窗口大小
// 引用: 
// 说明：条目的显示方法为
//		| (2 point) ITEM1 (2 point) ITEM2 ... ITEMn (2 point) |
//      |<--  Item 1 Area  (1) | (1) Item 2 ... Item n ---->  |
// ********************************************************************
static void AdjustWindowSize(HWND hWnd,LPSPELLAREA lpSpellArea)
{
	int iWindowWidth = 0;
	int iItemWidth = 0, iWindowHeight;
	RECT rect;
//	HWND hParent;

//		RETAILMSG(1,(TEXT(" Enter AdjustWindowSize\r\n")));
		iItemWidth = GetItemWidth(hWnd,lpSpellArea); // 得到第一个条目的宽度，如果已经没有任何的键，则返回0
//		RETAILMSG(1,(TEXT(" 1\r\n")));

		if (iItemWidth == 0)
		{
			// 已经没有任何的键，隐藏窗口
//			RETAILMSG(1,(TEXT(" 2\r\n")));
			ShowWindow(hWnd,SW_HIDE);

//			RETAILMSG(1,(TEXT(" 3\r\n")));
			//hParent = GetParent(hWnd); 
			// 通知父窗口这边已经没有数据了
//			RETAILMSG(1,(TEXT(" 4 = %x\r\n"),hParent));
			//SendMessage(hParent,SPN_NODATA,0,0);
//			RETAILMSG(1,(TEXT(" Leave AdjustWindowSize\r\n")));
			SendMessage(lpSpellArea->hOwner,SPN_NODATA,0,0);
			return ;
		}

//		RETAILMSG(1,(TEXT(" 6\r\n")));
		GetWindowRect(hWnd,&rect);  // 得到窗口矩形
//		RETAILMSG(1,(TEXT(" 7\r\n")));

		iWindowHeight = rect.bottom - rect.top;  // 窗口高度不变

		iWindowWidth = (iItemWidth + SPACE_WIDTH ) * lpSpellArea->SpellList.iSpellNum + SPACE_WIDTH; // 一共有 n +1 个间隔 

		iWindowWidth += 2; // 窗口边框

//		RETAILMSG(1,(TEXT(" 8\r\n")));
		SetWindowPos(hWnd,NULL,0,0,iWindowWidth,iWindowHeight,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);  // 不改变窗口次序，不移动窗口
//		RETAILMSG(1,(TEXT(" Leave AdjustWindowSize\r\n")));
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

// ********************************************************************
// 声明：static void ShowSpellArea(HWND hWnd,HDC hdc)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN hdc -- 设备句柄
// 返回值：
//	
// 功能描述：显示拼写区.
// 引用: 
// 说明：条目的显示方法为
//		| (2 point) ITEM1 (2 point) ITEM2 ... ITEMn (2 point) |
//      |<--  Item 1 Area  (1) | (1) Item 2 ... Item n ---->  |
// ********************************************************************
static void ShowSpellArea(HWND hWnd,HDC hdc)
{
	LPSPELLAREA lpSpellArea; 
	RECT rect;
	int i;
	HBRUSH hBrush;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);

		GetClientRect(hWnd,&rect);  // 得到窗口矩形

		// 清除背景
		hBrush = CreateSolidBrush(lpSpellArea->cl_Bk);
		FillRect(hdc,&rect,hBrush);
		DeleteObject(hBrush);
		
		// 开始绘制条目
		for( i = 0; i < lpSpellArea->SpellList.iSpellNum ; i++)
		{
			ShowSpellItem(hWnd,hdc,lpSpellArea, i);
		}
}

// ********************************************************************
// 声明：static void ShowSpellItem(HWND hWnd,HDC hdc,LPSPELLAREA lpSpellArea)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN hdc -- 设备句柄
//  IN lpSpellArea  -- 拼写结构
//	IN iIndex  -- 当前要显示的条目索引
// 返回值：
//	
// 功能描述：显示拼写条目.
// 引用: 
// ********************************************************************
static void ShowSpellItem(HWND hWnd,HDC hdc,LPSPELLAREA lpSpellArea, int iIndex)
{
	RECT rect;
	int iItemWidth; 

		
		if (iIndex == lpSpellArea->iCurSel)
		{
			// 是选择条目
			SetTextColor(hdc,lpSpellArea->cl_SelText);
			SetBkColor(hdc,lpSpellArea->cl_SelBk);
		}
		else
		{
			// 不是选择条目
			SetTextColor(hdc,lpSpellArea->cl_Text);
			SetBkColor(hdc,lpSpellArea->cl_Bk);
		}

		GetClientRect(hWnd,&rect);  // 得到窗口矩形
		rect.left += SPACE_WIDTH ; // 先空出最前面的空白

		iItemWidth = GetItemWidth(hWnd,lpSpellArea); // 得到条目的宽度

		rect.left += iIndex  * (iItemWidth + SPACE_WIDTH);
		rect.right = rect.left + iItemWidth; // 得到条目的右边界
		DrawText(hdc,lpSpellArea->SpellList.lpSpellList[iIndex],lpSpellArea->SpellList.iKeyNum,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 绘制条目文本

}
// ********************************************************************
// 声明：static int GetSpellIndex(HWND hWnd,LPSPELLAREA lpSpellArea,POINT point)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN lpSpellArea -- 当前的拼写结构
//  IN point -- 当前下笔的点
// 返回值：
//		无
// 功能描述：得到当前下笔的拼写索引
// 引用: 
// 说明：条目的显示方法为
//		| (2 point) ITEM1 (2 point) ITEM2 ... ITEMn (2 point) |
//      |<--  Item 1 Area  (1) | (1) Item 2 ... Item n ---->  |
// ********************************************************************
static int GetSpellIndex(HWND hWnd,LPSPELLAREA lpSpellArea,POINT point)
{
	int i;
	int iItemWidth,iCurPos = 0; 


		iCurPos += SPACE_WIDTH ; // 先加上最前面的空白

		iItemWidth = GetItemWidth(hWnd,lpSpellArea); // 得到第一个条目的宽度
		
		// 开始绘制条目
		for( i = 0; i < lpSpellArea->SpellList.iSpellNum ; i++)
		{
			iCurPos += iItemWidth + SPACE_WIDTH; // 得到当前条目的右边界
			if (point.x < iCurPos)
			{
				// 下笔在当前条目
				return i;
			}
		}
		return -1; // 没有下笔在任何条目
}


// ********************************************************************
// 声明：static int GetItemWidth(HWND hWnd,LPSPELLAREA lpSpellArea)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN lpSpellArea -- 当前的拼写结构
// 返回值：
//		无
// 功能描述：得到当前条目的宽度
// 引用: 
// ********************************************************************
static int GetItemWidth(HWND hWnd,LPSPELLAREA lpSpellArea)
{
	int iItemWidth = 0;

		if (lpSpellArea->SpellList.iKeyNum == 0)		
		{
			// 当前的拼写中没有任何字符，隐藏窗口
			return 0; // 返回
		}
		if (lpSpellArea->SpellList.iSpellNum == 0)		
		{
			// 如果当前有键，而没有拼写，则表明发生错误，清除内容，并隐藏窗口
			lpSpellArea->SpellList.iKeyNum = 0;
			return 0; // 返回
		}

		iItemWidth = GetStringWidth(hWnd,lpSpellArea->SpellList.lpSpellList[0]); // 得到第一个条目的宽度
		return iItemWidth;
}

// ********************************************************************
// 声明：static int GetNewSelIndex(LPSPELLAREA lpSpellArea,LPTSTR lpOldSelSpell)
// 参数：
//  IN lpSpellArea -- 当前的拼写结构
//  IN lpOldSelSpell -- 原来的选择拼写
// 返回值：
//		无
// 功能描述：得到当前最新的选择，要求找到一个与原来选择最近似的第一个选择
// 引用: 
// ********************************************************************
static int GetNewSelIndex(LPSPELLAREA lpSpellArea,LPTSTR lpOldSelSpell)
{
	int iCmplen ;
	int i;

		iCmplen = strlen(lpOldSelSpell); //得到原来拼写的长度

		if (iCmplen == 0)
			return 0; // 原来没有拼写，设置当前选择为0

		if (iCmplen > lpSpellArea->SpellList.iKeyNum)
			iCmplen = lpSpellArea->SpellList.iKeyNum;  //当前拼写的长度比原来的小

		for (i = 0; i < lpSpellArea->SpellList.iSpellNum ; i++)
		{
			if (strncmp(lpOldSelSpell,lpSpellArea->SpellList.lpSpellList[i],iCmplen) == 0) //与原来的选择拼写比较
			{
				// 与原来的选择有相同的字符，使用当前索引为当前选择
				return i;
			}
		}

		return 0; //没有找到匹配的字串，设置当前选择为0
}
#endif

#ifdef PLAN_2
// ********************************************************************
// 声明：static void ShowSpellArea(HWND hWnd,HDC hdc)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN hdc -- 设备句柄
// 返回值：
//	
// 功能描述：显示拼写区.
// 引用: 
// 说明：条目的显示方法为
//		| (2 point) ITEM1 (2 point) ITEM2 ... ITEMn (2 point) |
//      |<--  Item 1 Area  (1) | (1) Item 2 ... Item n ---->  |
// ********************************************************************
static void ShowSpellArea(HWND hWnd,HDC hdc)
{
	LPSPELLAREA lpSpellArea; 
	RECT rect;
//	int i;
	HBRUSH hBrush;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // 得到手写区结构
		ASSERT(lpSpellArea);

		GetClientRect(hWnd,&rect);  // 得到窗口矩形

		// 清除背景
		hBrush = CreateSolidBrush(lpSpellArea->cl_Bk);
		FillRect(hdc,&rect,hBrush);
		DeleteObject(hBrush);
		
		if (lpSpellArea->SpellList.iSpellNum)
		{
			// 开始绘制条目
			SetTextColor(hdc,lpSpellArea->cl_Text);
			SetBkColor(hdc,lpSpellArea->cl_Bk);
			DrawText(hdc,lpSpellArea->SpellList.lpSpellList[0],lpSpellArea->SpellList.iKeyNum,&rect,DT_VCENTER | DT_SINGLELINE); // 绘制条目文本
		}
}
#endif
