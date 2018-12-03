/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */


// scrollbar data

typedef struct __SCROLLBAR{
    short nMin; // log pos
    short nMax; // log pos
    short nPage; // log pos
    short nPos;
    short nTrackPos;
    WORD state;
	VOID * lpvUserData;  //OEM 自定义的数据
}_SCROLLDATA, FAR * _LPSCROLLDATA;


typedef struct __SCROLL{
    _SCROLLDATA sd;
    RECT rect;
    short fnBar;
}_SCROLL, FAR * _LPSCROLL;

#define OFF_MIN        0
#define OFF_MAX        2
#define OFF_PAGE       4
#define OFF_POS        6
#define OFF_TRACKPOS   8
#define OFF_STATE     10
#define MIN_THUMBSIZE 14
#define SBS_TRACKTHUMB 0x1000

static BOOL Scrl_GetScrollData( HWND hWnd, int fnBar, _LPSCROLL );
static BOOL Scrl_SendScrollMsg( HWND, int fnBar, UINT dwMsg, WPARAM, LPARAM );
static void Scrl_SetScrollValue( HWND, int fnBar, int offset, int v );
static int Scrl_GetScrollValue( HWND, int fnBar, int offset );
static int Scrl_GetDrawPos( _LPSCROLL, int pos[6] );
static int Scrl_DrawScrollBar( _LPSCROLL, HDC );
static HDC Scrl_GetScrollDC( HWND, int fnBar );
BOOL Scrl_Inital( _LPSCROLLDATA lpsd );
int Scrl_ShowThumbBox( _LPSCROLL, HDC, BOOL );
void Scrl_GetBarRect( LPRECT lprect, DWORD dwMainStyle, DWORD dwExStyle, int which );
static BOOL Scrl_GetThumbRect( _LPSCROLL lpsd, LPRECT lprect );
int Scrl_RedrawScrollBar( HWND hWnd, HDC hdc, int fnBar );
static BOOL Scrl_GetRect( _LPSCROLL, int fBar, LPRECT );
// this is all message handle proc
LRESULT Scrl_DoLBUTTONDOWN( HWND hWnd, int fnBar, int x, int y );

//LRESULT Scrl_DoSETSCROLLINFO( HWND hWnd, int fnBar, const void * lpsi, BOOL fRedraw );
LRESULT Scrl_DoSETSCROLLINFO( HWND hWnd, int fnBar, LPCSCROLLINFO lpsi,  BOOL fRedraw );
LRESULT Scrl_DoGETSCROLLINFO( HWND hWnd, int fnBar, void * lpsi );
//LRESULT Scrl_DoSETPOS( HWND hWnd, int fnBar, int nPos, BOOL fRedraw );
//LRESULT Scrl_DoGETPOS( HWND hWnd, int fnBar );
//LRESULT Scrl_DoGETRANGE( HWND hWnd, int fnBar, LPINT lpnMinPos, LPINT lpnMaxPos );
//LRESULT Scrl_DoSETRANGE( HWND hWnd, int fnBar, int nMinPos, int nMaxPos, BOOL fRedraw );

VOID _ReleaseWindowScrollBar( HWND hWnd, UINT type );
_SCROLLDATA * _GetWindowScrollBar( HWND hWnd, UINT type );

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // __SCROLLBAR_H


