/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
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
  UINT uFlags,     // �����־
  DWORD dwBytes   // Ҫ������ڴ��С
);

#define GlobalDiscard  Gbl_Discard
HGLOBAL WINAPI Gbl_Discard(
  HGLOBAL hglbMem  // ȫ���ڴ���
);

#define GlobalFlags  Gbl_Flags
UINT WINAPI Gbl_Flags(
  HGLOBAL hMem   // ȫ���ڴ���
);

#define GlobalFree  Gbl_Free
HGLOBAL WINAPI Gbl_Free(
  HGLOBAL hMem   // ȫ���ڴ���
);

#define GlobalHandle  Gbl_Handle
HGLOBAL WINAPI Gbl_Handle(
  LPCVOID pMem   // ȫ���ڴ��ַ
);

#define GlobalLock  Gbl_Lock
LPVOID WINAPI Gbl_Lock(
  HGLOBAL hMem   // ȫ���ڴ���
);

#define GlobalReAlloc  Gbl_ReAlloc
HGLOBAL WINAPI Gbl_ReAlloc(
  HGLOBAL hMem,   // ȫ���ڴ���
  DWORD dwBytes, // �¿�Ĵ�С
  UINT uFlags     // �ط���ı�־
);

#define GlobalSize  Gbl_Size
DWORD WINAPI Gbl_Size(
  HGLOBAL hMem   // ȫ���ڴ���
);

#define GlobalUnlock  Gbl_Unlock
BOOL WINAPI Gbl_Unlock(
  HGLOBAL hMem   // ȫ���ڴ���
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EGLOBMEM_H
