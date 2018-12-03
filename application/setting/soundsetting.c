/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：声音属性，用户界面部分
版本号：1.0.0
开发时期：2003-06-25
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include <SliderCtrl.h>
#include "emmsys.h"

/***************  全局区 定义， 声明 *****************/
const struct dlgIDD_SoundProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_SoundProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,120,0,0,"声音属性" },
    };

static LPTSTR strSoundVolumeClassName = "SOUNDVOLUMECLASS";
static LPTSTR strSoundSoundClassName = "SOUNDSOUNDCLASS";

#define IDC_TAB		300

#define IDC_SOUNDVOLUME  400
#define IDC_SOUNDSOUND   401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET SoundPropertySheet[] = {
	{"音量","SOUNDVOLUMECLASS",IDC_SOUNDVOLUME},
//	{"声音","SOUNDSOUNDCLASS",IDC_SOUNDSOUND},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK SoundPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSoundPropertyOK(HWND hDlg);

ATOM RegisterVolumeWindow(HINSTANCE hInstance);
ATOM RegisterSoundWindow(HINSTANCE hInstance);

/***************  函数实现 *****************/

// **************************************************
// 声明：void DoSoundProperty(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：声音属性入口函数。
// 引用: 
// **************************************************
void DoSoundProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_SoundProperty,hWnd, (DLGPROC)SoundPropertyProc); // 创建声音属性对话框
}

// ********************************************************************
// 声明：static LRESULT CALLBACK SoundPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：声音属性对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK SoundPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hDlg, &ps );
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
/*			switch (wmId)
			{
				case IDC_OK:
					EndDialog(hDlg,TRUE);
					return 0;
			}	
			*/
			return 0;
		case WM_NOTIFY: // 通知消息
			return DoNotify(hDlg,wParam,lParam);
		case WM_INITDIALOG: // 初始化对话框
			return DoInitDialog(hDlg);
		case WM_CLOSE: // 关闭消息
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK消息
			DoSoundPropertyOK(hDlg);
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}

// **************************************************
// 声明：static LRESULT DoInitDialog(HWND hDlg)
// 参数：
// 	IN hDlg -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述： 初始化对话框。
// 引用: 
// **************************************************
static LRESULT DoInitDialog(HWND hDlg)
{
	HINSTANCE hInstance;
	HWND hTab;
	TCITEM tcItem;
	int numItem;
	int i,iIndex;
	CTLCOLORSTRUCT stCtlColor;

		hInstance = (HINSTANCE)GetWindowLong(hDlg,GWL_HINSTANCE); // 得到实例句柄
		// 创建TAB窗口
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0);
		// 设置当前颜色
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterVolumeWindow(hInstance); // 注册音量窗口类
		RegisterSoundWindow(hInstance); // 注册声音窗口类

		numItem = sizeof(SoundPropertySheet) / sizeof(TABSHEET); // 得到TAB条目个数
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = SoundPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入一个TAB条目
			CreateWindow(SoundPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,73,hDlg,(HMENU)SoundPropertySheet[i].id,hInstance,0); // 创建一个对应的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一个条目为当前选择
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,SoundPropertySheet[iIndex].id),SW_SHOW); // 显示条目对应的窗口
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：返回消息处理结果
// 功能描述：处理通知消息。
// 引用: 
// **************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	NMHDR   *hdr=(NMHDR   *)lParam;    
	int iIndex;
	HWND hTab;

		hTab = GetDlgItem(hWnd,IDC_TAB); // 得到TAB窗口句柄
		switch(hdr->code)
		{
			 case TCN_SELCHANGING: // 选择正在改变，改变以前
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,SoundPropertySheet[iIndex].id),SW_HIDE); // 隐藏对应窗口
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择已经改变，改变以后
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,SoundPropertySheet[iIndex].id),SW_SHOW); // 显示对应窗口
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoSoundPropertyOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：声音属性WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoSoundPropertyOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(SoundPropertySheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,SoundPropertySheet[i].id); // 得到条目对应的窗口
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // 发送OK消息
		}
		return TRUE;
}


/****************************************************************************/
//  “音量” 界面
/****************************************************************************/
#define IDC_SLIDER 201

static LRESULT CALLBACK VolumeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoSoundVolumeCreate(HWND hWnd);
static LRESULT DoVolumeOK(HWND hDlg);

static DWORD GetVolume(void);


// **************************************************
// 声明：static ATOM RegisterVolumeWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册音量窗口类。
// 引用: 
// **************************************************
static ATOM RegisterVolumeWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strSoundVolumeClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)VolumeWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK VolumeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：声音属性音量对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK VolumeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"Volume",6);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoSoundVolumeCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoVolumeOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoSoundVolumeCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建声音音量属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoSoundVolumeCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hSlider;
	HICON hIcon;
	DWORD dwVolumeLevel,iVolumeLevelNum;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄

	// 创建标记
	CreateWindow(classSTATIC,"低",WS_CHILD | WS_VISIBLE ,10,20,20,18,hWnd,(HMENU)106,hInstance,0);
//	hSlider = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|WS_VISIBLE | TBS_AUTOTICKS |TBS_HORZ |TBS_TOP,50,20,100,50,hWnd,(HMENU)IDC_SLIDER,(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),0);
	// 创建调节音量的滑块
	hSlider = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|WS_VISIBLE | TBS_AUTOTICKS |TBS_HORZ |TBS_TOP,30,20,140,50,hWnd,(HMENU)IDC_SLIDER,hInstance,0);
	hIcon = (HICON)LoadImage( (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE( IDI_ARROW ), IMAGE_ICON,16, 16, 0 ) ;
	SendMessage(hSlider,TBM_SETTHUMBICON,MAKELONG(16,16),(LPARAM)hIcon); // 设置滑块图标

	iVolumeLevelNum = 7;
	SendMessage(hSlider,TBM_SETRANGE,FALSE,MAKELONG(0,iVolumeLevelNum -1)); // 设置范围

	dwVolumeLevel = GetVolume(); // 得到当前音量
	SendMessage(hSlider,TBM_SETPOS,TRUE,dwVolumeLevel); // 设置当前音量

	SendMessage(hSlider,TBM_SETPAGESIZE,0,1); // 设置页尺寸

	CreateWindow(classSTATIC,"高",WS_CHILD | WS_VISIBLE ,173,20,20,18,hWnd,(HMENU)106,hInstance,0);

	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoVolumeOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：音量WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoVolumeOK(HWND hDlg)
{
	HWND hSlider;
	DWORD dwVolume,dwVolumeLevel;

		hSlider = GetDlgItem(hDlg,IDC_SLIDER); // 得到滑块窗口句柄
		if (hSlider)
		{
			dwVolumeLevel = SendMessage(hSlider,TBM_GETPOS,0,0); // 得到当前音量位置
			dwVolume=dwVolumeLevel*0x22222222+0x11111111; // 得到当前音量
			RETAILMSG(1,(TEXT("will to be set Volume is %x\r\n"),dwVolume));

#ifndef EML_WIN32
			waveOutSetVolume(0,dwVolume); // 设置当前音量
#endif

		}
		return TRUE;
}
// ********************************************************************
// 声明：static DWORD GetVolume(void)
// 参数：
// 返回值：
// 	返回当前的音量大小
// 功能描述：得到当前的音量大小
// 引用: 
// ********************************************************************
static DWORD GetVolume(void)
{
	DWORD dwVolume=0,dwVolumeToSet;

#ifndef EML_WIN32
	waveOutGetVolume(0,&dwVolume); // 得到当前音量
#else
	dwVolume = 0x77777777;
#endif

	RETAILMSG(1,(TEXT("The current Volume is %x\r\n"),dwVolume));

	dwVolumeToSet = dwVolume & 0x0000E000;		// get the kernel volume bit 13-15
	dwVolumeToSet = (dwVolumeToSet >> 13) ;	   // move bit 13-15 to 0-2 

	return dwVolumeToSet;
}


/****************************************************************************/
//  “声音” 界面
/****************************************************************************/
static LRESULT CALLBACK SoundWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoSoundOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterSoundWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册声音窗口类。
// 引用: 
// **************************************************
static ATOM RegisterSoundWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strSoundSoundClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)SoundWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 得到背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 向系统注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK SoundWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：声音属性声音对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK SoundWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			TextOut(hdc,10,10,"Sound",5);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			break;
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoSoundOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoSoundOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：声音WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoSoundOK(HWND hDlg)
{
		return TRUE;
}

