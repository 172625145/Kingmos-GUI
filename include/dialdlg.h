/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
#ifndef __DIALDLG_H
#define __DIALDLG_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// ����������й���������
#include <gprscfg.h>

BOOL DailToInternet(HWND hWnd,HANDLE *pRasConn,DWORD dwDailNetworkType);
BOOL HandDownInternet(HWND hWnd, HANDLE hRasConn);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //__DIALDLG_H
