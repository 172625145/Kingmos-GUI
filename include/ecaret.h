/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ECARET_H
#define __ECARET_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#define CreateCaret Caret_Create
BOOL WINAPI Caret_Create( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight );

#define DestroyCaret Caret_Destroy
BOOL WINAPI Caret_Destroy( void );

#define GetCaretBlinkTime Caret_GetBlinkTime
UINT WINAPI Caret_GetBlinkTime( void );

#define GetCaretPos Caret_GetPos
BOOL WINAPI Caret_GetPos( LPPOINT lpPoint );

#define HideCaret Caret_Hide
BOOL WINAPI Caret_Hide( HWND );

#define SetCaretBlinkTime Caret_SetBlinkTime
BOOL WINAPI Caret_SetBlinkTime( UINT uMSeconds );

#define SetCaretPos Caret_SetPos
BOOL WINAPI Caret_SetPos( int x, int y );

#define ShowCaret Caret_Show
BOOL WINAPI Caret_Show( HWND );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif         //__ECARET_H
