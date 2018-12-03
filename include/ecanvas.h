/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __ECANVAS_H
#define __ECANVAS_H

#ifndef __EDEF_H
#include "edef.h"
#endif

extern ATOM RegisterCanvasClass( HINSTANCE hInst );
extern const char classCANVAS[];


#define CM_SETPROPERTY  0x4000
#define CM_SETDRAWMODE  0x4001
#define CM_GETDRAWMODE  0x4002
#define CM_SETLINEWIDTH 0x4003
#define CM_GETLINEWIDTH 0x4004
#define CM_SETFILLMODE  0x4005
#define CM_GETFILLMODE  0x4006
#define CM_SETCOLOR     0x4007
#define CM_GETCOLOR     0x4008
#define CM_SETBKCOLOR   0x4009
#define CM_GETBKCOLOR   0x400A
#define CM_GETZOOMWINDOW 0x400B
#define CM_GETMODIFYFLAG 0x400C
#define CM_SETMODIFYFLAG 0x400D
/*
Parameter:
	wParam = 0
	lParam = (LPPOINT)lpPoint;
return :
	success RGB value, failure -1
*/
#define CM_GETPIXEL		 0x400E

typedef struct{
	NMHDR hdr;
	UINT iMouseStatus;
	int iDrawMode;
	POINT ptAction;
}NMCANVAS,* LPNMCANVAS;

typedef struct{
	NMHDR hdr;
	COLORREF crColor;
	POINT ptAction;
}NMGETCOLOR,* LPNMGETCOLOR;
typedef struct{
	NMHDR hdr;
	POINT ptAction;
}NMMAGNIFIER,* LPNMMAGNIFIER;

#define CN_MOUSECHANGED  0x4100
#define CN_GETPOINTCOLOR 0x4101
#define CN_MAGNIFIER	 0X4102

// Draw Mode
#define NODRAWMODE     -1
#define PENMODE         0
#define LINEMODE        1
#define RECTMODE        2
#define CIRCLEMODE      3
#define RUBBERMODE      4
#define SELECTMODE      5
#define INPUTCHARMODE   6
#define GETCOLORMODE    7
#define ZOOMMODE		8
#define AIRBRUSHMODE	9
#define FILLAREAMODE	10
#define MAXMODE         10

// Fill Mode
#define FILL_EMPTY      0
#define FILL_BACKGROUND 1
#define FILL_FOREGROUND 2


#endif  // __ECANVAS_H

#ifndef __MAGNIFIER_H

#define MM_ASSOCIATE   0x4300

#endif  //__MAGNIFIER_H
