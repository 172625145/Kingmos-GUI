/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __DESKTOP_H
#define __DESKTOP_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// 这里是你的有关声明部分
void LoadDeskTop(HINSTANCE hInstance);
extern const char classStateBar[];

void DrawTransparentBitMap(HDC hdc,HBITMAP hBitmap, const RECT * lprect,UINT xOffset,UINT yOffset, DWORD clrTranspant );
#define CLR_TRANSPARENT ( RGB( 255, 0, 255 ) )

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__DESKTOP_H
