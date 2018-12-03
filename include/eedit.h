/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EEDIT_H
#define __EEDIT_H

#include "eWindows.h"

#define ES_LEFT             0x0000L
#define ES_CENTER           0x0001L
#define ES_RIGHT            0x0002L
#define ES_MULTILINE        0x0004L
#define ES_UPPERCASE        0x0008L
#define ES_LOWERCASE        0x0010L
#define ES_PASSWORD         0x0020L

//#define ES_AUTOVSCROLL      0x0040L
#define ES_LINESEPARATE     0x0040L

//#define ES_AUTOHSCROLL      0x0080L
#define ES_AUTOEXTEND       0x0080L

#define ES_NOHIDESEL        0x0100L

//#define ES_OEMCONVERT       0x0400L
#define ES_MULTIITEMEDIT        0x0400L

#define ES_READONLY         0x0800L

//#define ES_WANTRETURN       0x1000L
#define ES_AUTONEWLINE      0x1000L

#define ES_NUMBER           0x2000L

#define ES_FORMAT           0x4000L

#define ES_CALLBACK           0x8000L
/*
 * Edit Control Messages
 */
#define EM_GETSEL               0x00B0
#define EM_SETSEL               0x00B1
#define EM_GETRECT              0x00B2
#define EM_SETRECT              0x00B3
#define EM_SETRECTNP            0x00B4
#define EM_SCROLL               0x00B5
#define EM_LINESCROLL           0x00B6
#define EM_SCROLLCARET          0x00B7
#define EM_GETMODIFY            0x00B8
#define EM_SETMODIFY            0x00B9
#define EM_GETLINECOUNT         0x00BA
#define EM_LINEINDEX            0x00BB
#define EM_SETHANDLE            0x00BC
#define EM_GETHANDLE            0x00BD
#define EM_GETTHUMB             0x00BE
#define EM_LINELENGTH           0x00C1
#define EM_REPLACESEL           0x00C2
#define EM_GETLINE              0x00C4
#define EM_LIMITTEXT            0x00C5
#define EM_CANUNDO              0x00C6
#define EM_UNDO                 0x00C7
#define EM_FMTLINES             0x00C8
#define EM_LINEFROMCHAR         0x00C9
#define EM_SETTABSTOPS          0x00CB
#define EM_SETPASSWORDCHAR      0x00CC
#define EM_EMPTYUNDOBUFFER      0x00CD
#define EM_GETFIRSTVISIBLELINE  0x00CE
#define EM_SETREADONLY          0x00CF
#define EM_SETWORDBREAKPROC     0x00D0
#define EM_GETWORDBREAKPROC     0x00D1
#define EM_GETPASSWORDCHAR      0x00D2

#define EM_SETMARGINS           0x00D3
#define EM_GETMARGINS           0x00D4
#define EM_SETLIMITTEXT         EM_LIMITTEXT   
#define EM_GETLIMITTEXT         0x00D5
#define EM_POSFROMCHAR          0x00D6
#define EM_CHARFROMPOS          0x00D7


#define EM_SETIMESTATUS         0x00D8
#define EM_GETIMESTATUS         0x00D9


#define EM_GETCARETPOS        (WM_OS+100)
#define EM_SETCARETPOS        (WM_OS+101)
#define EN_UPPAGE             (WM_OS+102)
#define EN_DOWNPAGE           (WM_OS+103)
#define EN_END                (WM_OS+104)
#define EN_HOME               (WM_OS+105)
#define EN_LEFT               (WM_OS+106)
#define EN_UPLINE             (WM_OS+107)
#define EN_RIGHT              (WM_OS+108)
#define EN_DOWNLINE           (WM_OS+109)
#define MIEM_SIZECHANGED      (WM_OS+110)
#define MIEM_SETXDISPLAYPOS   (WM_OS+111)
#define MIEM_DISPLAYCHANGED   (WM_OS+112)
#define EM_SETCARETTONEXTITEM (WM_OS+113)
#define EM_GETITEM						(WM_OS+114)
#define EM_EMPTY  						(WM_OS+115)
#define EM_INSERTITEM					(WM_OS+116)
#define EM_SETITEM            (WM_OS+117)
#define EM_DELETEITEM         (WM_OS+118)
#define EM_GETITEMTEXT		  (WM_OS+119)
#define EM_SETITEMTEXT		  (WM_OS+120)	
#define EM_GETITEMTEXTLENGTH  (WM_OS+121)
#define EM_SETITEMCALLBACK    (WM_OS+122)
/*
	Parameter:
		wParam = (int)iStart; // if iStart = -1, the Start form Text Begin
		lParam = (LPTSTR)lpSearchString;
	return :
		The position of the find string begin, -1 is not find
*/
#define EM_SEARCHSTRING		  (WM_OS+123)

/*
	Parameter:
		wParam = (int)iItem; 
		lParam = 0;
	return :
		The Style of the special item 
*/
#define EM_GETITEMSTYLE		  (WM_OS+126)
/*
	Parameter:
		wParam = (int)iItem; 
		lParam = dwStyle;
	return :
		none
*/
#define EM_SETITEMSTYLE		  (WM_OS+127)	

/*
	Parameter:
		wParam = 0; // if iStart = -1, the Start form Text Begin
		lParam = 0;
	return :
		success , return the index of the item , else return -1
*/
#define EM_GETACTIVEITEM		 (WM_OS+124)


// Edit Control Notification Codes
#define EN_SETFOCUS         0x0100
#define EN_KILLFOCUS        0x0200
#define EN_CHANGE           0x0300
#define EN_UPDATE           0x0400
#define EN_ERRSPACE         0x0500
#define EN_MAXTEXT          0x0501
#define EN_HSCROLL          0x0601
#define EN_VSCROLL          0x0602
#define EN_ACTIVEITEMCHANGE 0x0700

#define HSCROLLWIDTH 8
#define FMTSEPARATE  0x01

struct PDAEditPropertyStruct
{
	LPTSTR lpTitle;
	short  cLimited;
    LPTSTR lpFormat;
} ;


typedef struct PDAEditPropertyStruct PDAEDITPROPERTY;
typedef struct PDAEditPropertyStruct * LPPDAEDITPROPERTY;

struct EditItemPropertyStruct
{
    LPTSTR lpTitle;
    LPTSTR lpItemText;
    DWORD  dwItemStyle;
    short  cLimited;
    LPTSTR lpFormat;
};

typedef struct EditItemPropertyStruct EDITITEMPROPERTY;
typedef struct EditItemPropertyStruct * LPEDITITEMPROPERTY;

struct MultiItemEditPropertyStruct
{
    short nItemNum;
    LPEDITITEMPROPERTY lpEditItemProperty;
};

typedef struct MultiItemEditPropertyStruct MULTIITEMEDITPROPERTY;
typedef struct MultiItemEditPropertyStruct * LPMULTIITEMEDITPROPERTY;

typedef void (CALLBACK * EDITCALLBACK)(HWND hWnd,short nItem);

#endif   //__EEDIT_H
