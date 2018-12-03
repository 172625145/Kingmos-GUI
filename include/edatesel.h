/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EDATESEL_H
#define __EDATESEL_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//---------------------------------------------------
//控制消息									//wParam, lParam
//
//获取日期
#define DSM_GETDATETIME			(WM_USER+3)	//0, (SYSTEMTIME*)

//设置日期
#define DSM_SETDATETIME			(WM_USER+4)	//0, (SYSTEMTIME*)

//选中是通知用户(在失去激活 或2次单击 时)
#define	DSM_SETUSERSELNOTIFY	(WM_USER+5)	//hWnd, uMsg

//设置年范围
#define	DSM_SETYEARRANGE		(WM_USER+6)	//(Low-Min, High-Max), 0

//设置Draw Days 的字体大小
#define DSM_SETDAYSFONT			(WM_USER+7)	//0, 0/1/2(参数lParam 见下面)
#define DAYSFONT_DEFAULT		0
#define DAYSFONT_8X8			1
#define DAYSFONT_8X6			2

//设置特定功能的风格
#define DSM_SETDATESTYLE		(WM_USER+8)	//DateStyle, 0
#define DSS_SHOWMPREV			0x00000001L	//
#define DSS_SHOWMNEXT			0x00000002L	//
#define DSS_USERSEL				0x00000004L	//2次单击--选中的通知，点击非本窗口区域内--取消选择的通知(若有DSM_SETUSERSELNOTIFY)
//

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EDATESEL_H
