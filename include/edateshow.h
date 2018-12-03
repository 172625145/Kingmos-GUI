/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EDATESHOW_H
#define __EDATESHOW_H

#include <edatesel.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//---------------------------------------------------
//以下的控制消息是发给  DateShow的消息，与发给DateSel的消息，有相同定义。
//
//控制消息									//wParam, lParam
//
//获取日期
//DSM_GETDATETIME-----参考 edatesel.h 中的消息和其参数设置

//设置日期
//DSM_SETDATETIME-----参考 edatesel.h 中的消息和其参数设置

//选中是通知用户(在失去激活 或2次单击 时)
//DSM_SETUSERSELNOTIFY-参考 edatesel.h 中的消息和其参数设置

//设置年范围
//DSM_SETYEARRANGE----参考 edatesel.h 中的消息和其参数设置

//设置DateSel的消息
//DSM_SETDAYSFONT-----参考 edatesel.h 中的消息和其参数设置
//DSM_SETDATESTYLE----参考 edatesel.h 中的消息和其参数设置

#define DSM_SETDATESIZE			(WM_USER+20)	//(cx, cy), 0
#define DSM_GETMODIFY			(WM_USER+21)	//0, 0,  (returned val: fGetModified)
#define DSM_SETMODIFY			(WM_USER+22)	//fSetModified, 0

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EDATESHOW_H
