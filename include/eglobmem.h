/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EGLOBMEM_H
#define __EGLOBMEM_H

typedef HANDLE              HGLOBAL;

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define GMEM_MOVEABLE   0x0001
#define GMEM_DDESHARE	0x0002
#define GMEM_ZEROINIT	0x0004
#define GPTR			0x0008
#define GHND			0x0010
#define GMEM_FIXED		0x0020


#define GlobalAlloc  Gbl_Alloc
HGLOBAL WINAPI Gbl_Alloc(
  UINT uFlags,     // 分配标志
  DWORD dwBytes   // 要分配的内存大小
);

#define GlobalDiscard  Gbl_Discard
HGLOBAL WINAPI Gbl_Discard(
  HGLOBAL hglbMem  // 全局内存句柄
);

#define GlobalFlags  Gbl_Flags
UINT WINAPI Gbl_Flags(
  HGLOBAL hMem   // 全局内存句柄
);

#define GlobalFree  Gbl_Free
HGLOBAL WINAPI Gbl_Free(
  HGLOBAL hMem   // 全局内存句柄
);

#define GlobalHandle  Gbl_Handle
HGLOBAL WINAPI Gbl_Handle(
  LPCVOID pMem   // 全局内存地址
);

#define GlobalLock  Gbl_Lock
LPVOID WINAPI Gbl_Lock(
  HGLOBAL hMem   // 全局内存句柄
);

#define GlobalReAlloc  Gbl_ReAlloc
HGLOBAL WINAPI Gbl_ReAlloc(
  HGLOBAL hMem,   // 全局内存句柄
  DWORD dwBytes, // 新块的大小
  UINT uFlags     // 重分配的标志
);

#define GlobalSize  Gbl_Size
DWORD WINAPI Gbl_Size(
  HGLOBAL hMem   // 全局内存句柄
);

#define GlobalUnlock  Gbl_Unlock
BOOL WINAPI Gbl_Unlock(
  HGLOBAL hMem   // 全局内存句柄
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EGLOBMEM_H
