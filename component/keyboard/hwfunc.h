/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __HWFUNC_H
#define __HWFUNC_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus 

// 辩识系统已经有结果
// WPARAM -- INT 得到已经辩识的汉字个数
// LPARAM -- LPTSTR  得到辩识的汉字缓存地址
#define KM_RECOGNIZE	(WM_USER + 1033)


// **************************************************
// 声明：BOOL InitialHWFunction(HWND hWnd)
// 参数：
//	IN hWnd -- 系统将要把辩识的结果发送的窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：初始化辩识系统。
// 引用: 
// **************************************************
BOOL InitialHWFunction(HWND hWnd);

// **************************************************
// 声明：BOOL DeinitialHWFunction(void)
// 参数：
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：停止辩识系统。
// 引用: 
// **************************************************
BOOL DeinitialHWFunction(void);

// **************************************************
// 声明：BOOL StartHWFunction(void)
// 参数：
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：开始辩识功能。
// 引用: 
// **************************************************
BOOL StartHWFunction(void);

// **************************************************
// 声明：BOOL StopHWFunction(void)
// 参数：
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：停止辩识功能。
// 引用: 
// **************************************************
BOOL StopHWFunction(void);

#ifdef __cplusplus
}           
#endif  // __cplusplus
#endif  //__HWFUNC_H
