// 实现 OPENGL 中需要但是系统没有实现的函数
#ifndef __MLG_GLTHIRD
#define __MLG_GLTHIRD


#ifdef  __cplusplus
extern "C" {
#endif

#include <ewindows.h>
#include <egdi.h>

#define MAXPNAMELEN				32  /* max product name length (including NULL) */
#define MAX_JOYSTICKOEMVXDNAME	260 /* max oem vxd name length (including NULL) */
typedef struct tagJOYCAPSA {
    WORD    wMid;                /* manufacturer ID */
    WORD    wPid;                /* product ID */
    CHAR    szPname[MAXPNAMELEN];/* product name (NULL terminated string) */
    UINT    wXmin;               /* minimum x position value */
    UINT    wXmax;               /* maximum x position value */
    UINT    wYmin;               /* minimum y position value */
    UINT    wYmax;               /* maximum y position value */
    UINT    wZmin;               /* minimum z position value */
    UINT    wZmax;               /* maximum z position value */
    UINT    wNumButtons;         /* number of buttons */
    UINT    wPeriodMin;          /* minimum message period when captured */
    UINT    wPeriodMax;          /* maximum message period when captured */
    UINT    wRmin;               /* minimum r position value */
    UINT    wRmax;               /* maximum r position value */
    UINT    wUmin;               /* minimum u (5th axis) position value */
    UINT    wUmax;               /* maximum u (5th axis) position value */
    UINT    wVmin;               /* minimum v (6th axis) position value */
    UINT    wVmax;               /* maximum v (6th axis) position value */
    UINT    wCaps;	 	 /* joystick capabilites */
    UINT    wMaxAxes;	 	 /* maximum number of axes supported */
    UINT    wNumAxes;	 	 /* number of axes in use */
    UINT    wMaxButtons;	 /* maximum number of buttons supported */
    CHAR    szRegKey[MAXPNAMELEN];/* registry key */
    CHAR    szOEMVxD[MAX_JOYSTICKOEMVXDNAME]; /* OEM VxD in use */
} JOYCAPS, *PJOYCAPS, *NPJOYCAPS, *LPJOYCAPS;

// 返回值
#define JOYERR_NOERROR        (0)                  /* no error */


typedef struct joyinfoex_tag {
    DWORD dwSize;		 /* size of structure */
    DWORD dwFlags;		 /* flags to indicate what to return */
    DWORD dwXpos;                /* x position */
    DWORD dwYpos;                /* y position */
    DWORD dwZpos;                /* z position */
    DWORD dwRpos;		 /* rudder/4th axis position */
    DWORD dwUpos;		 /* 5th axis position */
    DWORD dwVpos;		 /* 6th axis position */
    DWORD dwButtons;             /* button states */
    DWORD dwButtonNumber;        /* current button number pressed */
    DWORD dwPOV;                 /* point of view state */
    DWORD dwReserved1;		 /* reserved for communication between winmm & driver */
    DWORD dwReserved2;		 /* reserved for future expansion */
} JOYINFOEX, *PJOYINFOEX, NEAR *NPJOYINFOEX, FAR *LPJOYINFOEX;


typedef	UINT	MMRESULT;
typedef	UINT	UINT_PTR;
/*
// 外部定义的函数 *****>>>>>*****>>>>>*****>>>>>*****>>>>>*****>>>>>*****>>>>>
extern	DWORD	GetGlyphOutline( HDC hdc, UINT uChar, UINT uFormat, LPGLYPHMETRICS lpgm, DWORD cbBuffer, LPVOID lpvBuffer, CONST MAT2 *lpmat2 );
extern	UINT	GetPaletteEntries( HPALETTE hpal, UINT iStartIndex, UINT nEntries, PALETTEENTRY *lppe );
extern	UINT	GetNearestPaletteIndex( HPALETTE hpal, COLORREF crColor );
extern	HPALETTE CreatePalette( CONST LOGPALETTE *lplgpl );
extern	HDC		CreateIC( LPCTSTR lpszDriver, LPCTSTR lpszDevice, LPCTSTR lpszOutput, int iVoid );
extern	char	*strtok( char *strToken, const char *strDelimit );
extern	HCURSOR SetCursor( HCURSOR hCursor );
extern	BOOL	SetCursorPos( int X, int Y );
extern	BOOL	GetCursorPos( LPPOINT lpPoint );
extern	LONG	ChangeDisplaySettings( LPDEVMODEA lpDevMode, DWORD dwflags );
extern	BOOL	EnumDisplaySettings( LPCTSTR lpszDeviceName, DWORD iModeNum, LPDEVMODEA lpDevMode );
extern	MMRESULT joyGetDevCaps( UINT_PTR uJoyID, LPJOYCAPS pjc, UINT cbjc );
extern	MMRESULT joyGetPosEx( UINT uJoyID, LPJOYINFOEX pji );
extern	MMRESULT joyReleaseCapture( UINT uJoyID );
extern	MMRESULT joySetThreshold( UINT uJoyID, UINT uThreshold );
extern	MMRESULT joySetCapture( HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged );
extern	BOOL	GdiFlush( VOID );
extern	BOOL	AppendMenu( HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCTSTR lpNewItem );
extern	BOOL	ModifyMenu( HMENU hMnu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCTSTR lpNewItem );
extern	BOOL	AdjustWindowRect( LPRECT lpRect, DWORD dwStyle, BOOL bMenu );
extern	UINT	RealizePalette( HDC hdc );
extern	HPALETTE SelectPalette( HDC hdc, HPALETTE hpal, BOOL bForceBackground );
extern	BOOL	UnrealizeObject( HGDIOBJ hgdiobj );
extern	UINT	GetSystemPaletteEntries( HDC hdc, UINT iStartIndex, UINT nEntries, PALETTEENTRY *lppe );
extern	UINT	SetPaletteEntries( HPALETTE hpal, UINT iStart, UINT cEntries, CONST PALETTEENTRY *lppe );
// 外部定义的函数 *****<<<<<*****<<<<<*****<<<<<*****<<<<<*****<<<<<*****<<<<<
*/


#ifdef  __cplusplus
}
#endif

#endif


