#include <windows.h>
#include <eversion.h>

#include <w32cfg.h>

HBITMAP hDeskTopBitmap=0;
HBITMAP hLogoBitmap=0;

const int iDisplayOffsetX=101;
const int iDisplayOffsetY=59;


const int iDisplayWidth=DISPLAY_WIDTH;
const int iDisplayHeight=DISPLAY_HEIGHT;
const int iDeskTopWidth=DISPLAY_WIDTH + 200;//322;//DISPLAY_WIDTH + 60;//380;
const int iDeskTopHeight=DISPLAY_HEIGHT + 120;//492;//515;//DISPLAY_HEIGHT + 80;//280;
const int iTouchWidth=DISPLAY_WIDTH;
const int iTouchHeight=DISPLAY_HEIGHT;
const int iPlane=1;

const int iBitsPerPel=BPP;

HWND hwndDebugWnd;

const COLORREF rgbs[] = {  
	RGB(00,00,00),
	RGB(00,00,00),
	RGB(88,88,88),
//RGB(101,101,101),
	RGB(108,108,108),
//RGB(119,119,119),
	RGB(134,134,134),
//	RGB(145,145,145),
	RGB(153,153,153),
//	RGB(164,164,164),
	RGB(170,170,170),
//	RGB(174,174,174),
	RGB(180,180,180),
//	RGB(184,184,184),
	RGB(192,192,192),
//	RGB(199,199,199),
	RGB(206,206,206),
//	RGB(216,216,216),
	RGB(225,225,225),
//	RGB(235,235,235),
	RGB(240,240,240),
//	RGB(245,245,245),
	RGB(250,250,250),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(50,50,50),
	RGB(11,11,11),
	RGB(11,11,11)
};	
const int iRGBNum = (sizeof(rgbs) / sizeof(COLORREF));






