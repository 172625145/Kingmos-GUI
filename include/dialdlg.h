/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef __DIALDLG_H
#define __DIALDLG_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// 这里是你的有关声明部分
#include <gprscfg.h>

BOOL DailToInternet(HWND hWnd,HANDLE *pRasConn,DWORD dwDailNetworkType);
BOOL HandDownInternet(HWND hWnd, HANDLE hRasConn);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //__DIALDLG_H
