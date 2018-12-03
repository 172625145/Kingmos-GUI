#ifndef __W32CFG_H
#define __W32CFG_H

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480


extern const int iDeskTopWidth;
extern const int iDeskTopHeight;
extern HBITMAP hDeskTopBitmap;
extern HBITMAP hLogoBitmap;
extern const int iDisplayOffsetX;
extern const int iDisplayOffsetY;
extern const int iDisplayWidth;
extern const int iDisplayHeight;
extern const int iTouchWidth;
extern const int iTouchHeight;
extern const int iPlane;
extern const int iBitsPerPel;
extern HWND hwndDeskTop;
extern HWND hwndDebugWnd;
extern const COLORREF rgbs[];
extern const int iRGBNum;

#define RESCHE_TIME 50

//#define WORK_PAD_HEIGHT  40
#define CL_BKLIGHT (RGB( 0x00, 0xff, 0xff ))
#endif  //__WIN32CFG_H



