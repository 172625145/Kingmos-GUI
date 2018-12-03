/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ERGN_H
#define __ERGN_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

// Region type flags
#define ERROR               0
#define NULLREGION          1
#define SIMPLEREGION        2
#define COMPLEXREGION       3

// CombineRgn command values
#define RGN_AND             1
#define RGN_OR              2
#define RGN_XOR             3
#define RGN_DIFF            4
#define RGN_COPY            5

#define CreateRectRgn Rgn_CreateRect
HRGN WINAPI Rgn_CreateRect(int, int, int, int);

#define CreateRectRgnIndirect Rgn_CreateRectIndirect
HRGN WINAPI Rgn_CreateRectIndirect(LPCRECT);

#define CombineRgn Rgn_Combine
int WINAPI Rgn_Combine(HRGN, HRGN, HRGN, int);

#define EqualRgn Rgn_Equal
BOOL WINAPI Rgn_Equal(HRGN, HRGN);

#define OffsetRgn Rgn_Offset
int WINAPI Rgn_Offset(HRGN, int, int);

#define GetRgnBox Rgn_GetBox
int WINAPI Rgn_GetBox(HRGN, RECT FAR*);

#define RectInRegion Rgn_RectInRegion
BOOL WINAPI Rgn_RectInRegion(HRGN, const RECT * );

#define PtInRegion Rgn_PtInRegion
BOOL WINAPI Rgn_PtInRegion(HRGN, int, int);

#define SetRectRgn Rgn_SetRect
BOOL WINAPI Rgn_SetRect(HRGN, int, int, int, int );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //__ERGN_H
