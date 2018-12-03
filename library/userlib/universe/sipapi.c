/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：处理软键盘
版本号：1.0.0
开发时期：2003-03-18
作者：陈建明 Jami chen
修改记录：
******************************************************/

#include "ewindows.h"
#include "SipApi.h"

extern const char classKEYBOARD[];

// **************************************************
// 声明：static HWND GetKeyboardWindow(void)
// 参数：
// 	无
// 返回值：返回键盘窗口的句柄
// 功能描述：得到窗口的句柄
// 引用: 
// **************************************************
static HWND GetKeyboardWindow(void)
{
	return FindWindow( classKEYBOARD, NULL ) ;  // 查找键盘窗口
}

// **************************************************
// 声明：BOOL Kingmos_SipShowIM( DWORD dwFlag )
// 参数：
// 	IN dwFlag
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：显示/隐藏软件盘
// 引用: 
// **************************************************
BOOL Kingmos_SipShowIM( DWORD dwFlag )
{
	HWND hKeyboard;
	
		hKeyboard = GetKeyboardWindow(); // 得到键盘窗口句柄
		if (hKeyboard)
		{
			if (dwFlag & SIPF_OFF)
			{ // 隐藏键盘
				ShowWindow(hKeyboard,SW_HIDE);
			}
			else
			{ // 显示键盘
				ShowWindow(hKeyboard,SW_SHOW);
			}
		}
		return TRUE;
}
