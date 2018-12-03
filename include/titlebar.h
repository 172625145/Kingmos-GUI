/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __TITLEBAR_H
#define __TITLEBAR_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

extern const TCHAR	classTitleBar[];
//标题条控制风格
//不显示电池
#define TBCS_BATTERY            0x0001
//不显示时间
#define TBCS_TIME               0x0002

//显示关闭按钮，如果有该风格，则点击该按钮后，会给父窗口发 
// Message = WM_COMMAND
// wParam = IDCANCEL
// lParam = 该标题条窗口句柄
#define TBCS_CANCEL_BOX             0x0004
#define TBCS_OK_BOX                0x0008
#define TBCS_HELP_BOX              0x0010

//在用之前，必须先调用 InitThirdpartControls 或 InitThirdpartControlsEx
#define CreateTitleBar TitleBar_Create
HWND WINAPI TitleBar_Create( 
					 LPTSTR lpszTitle,
					 DWORD dwTitleBarStyle,
					 int x,			//在父窗口的开始 x 坐标
					 int y,			//在父窗口的开始 y 坐标
					 HWND hParent	//父窗口句柄
					);

#ifdef __cplusplus
}
#endif      // __cplusplus

#endif  //__TITLEBAR_H
