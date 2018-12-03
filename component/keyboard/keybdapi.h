/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __KEYBDSRV_H
#define __KEYBDAPI_H

#ifndef __EDEF_H
#include <edef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus




HWND WINAPI Keybd_CreateKeyboard(HINSTANCE hInstance, HWND hWnd, int yPos);
VOID WINAPI Keybd_ShowKeyboard(HWND hWnd,DWORD dwFlag);

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif   // __KEYBDSRV_H


