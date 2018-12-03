/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#ifndef __ECLIPBRD_H
#define __ECLIPBRD_H

#define CF_TEXT			1
#define CF_FILE			2

#define OpenClipboard  Clip_Open
BOOL WINAPI Clip_Open(HWND hWndNewOwner);

#define CloseClipboard  Clip_Close
BOOL WINAPI Clip_Close(void);

#define SetClipboardData  Clip_SetData
HANDLE WINAPI Clip_SetData(UINT uFormat, HANDLE hMem );

#define GetClipboardData  Clip_GetData
HANDLE WINAPI Clip_GetData(UINT uFormat );

#define EmptyClipboard  Clip_Empty
BOOL WINAPI Clip_Empty(VOID);

#define IsClipboardFormatAvailable  Clip_IsFormatAvailable
BOOL WINAPI Clip_IsFormatAvailable(UINT uFormat );


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__ECLIPBRD_H
