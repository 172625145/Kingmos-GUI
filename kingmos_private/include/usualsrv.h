/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef __USUALSRV_H__
#define __USUALSRV_H__

//#include "eglobmem.h"

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */ 

HGLOBAL WINAPI Gbl_GlobalAlloc(  UINT uFlags,  DWORD dwBytes );
HGLOBAL WINAPI Gbl_GlobalDiscard(  HGLOBAL hglbMem  );
UINT WINAPI Gbl_GlobalFlags(  HGLOBAL hMem   );
HGLOBAL WINAPI Gbl_GlobalFree(HGLOBAL hMem );
HGLOBAL WINAPI Gbl_GlobalHandle(  LPCVOID pMem  );
LPVOID WINAPI Gbl_GlobalLock(  HGLOBAL hMem   );
HGLOBAL WINAPI Gbl_GlobalReAlloc(  HGLOBAL hMem,   DWORD dwBytes, UINT uFlags );
DWORD WINAPI Gbl_GlobalSize(  HGLOBAL hMem  );
BOOL WINAPI Gbl_GlobalUnlock(  HGLOBAL hMem );

BOOL WINAPI Clip_OpenClipboard(HWND hWndNewOwner);
BOOL WINAPI Clip_CloseClipboard(void);
HANDLE WINAPI Clip_SetClipboardData(UINT uFormat, HANDLE hMem );
HANDLE WINAPI Clip_GetClipboardData(UINT uFormat );
BOOL WINAPI Clip_EmptyClipboard(VOID);
BOOL WINAPI Clip_IsClipboardFormatAvailable(UINT uFormat);

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif  //__USUALSRV_H__
