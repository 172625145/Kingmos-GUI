/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __DESKTOP_H
#define __DESKTOP_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ����������й���������
void LoadDeskTop(HINSTANCE hInstance);
extern const char classStateBar[];

void DrawTransparentBitMap(HDC hdc,HBITMAP hBitmap, const RECT * lprect,UINT xOffset,UINT yOffset, DWORD clrTranspant );
#define CLR_TRANSPARENT ( RGB( 255, 0, 255 ) )

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__DESKTOP_H
