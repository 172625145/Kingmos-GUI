/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EEBASE_H

#include "eframe.h"
#include "eedit.h"

#define DOT '.'

//#define INITIALEDITSIZE 64
#define INITIALEDITSIZE 64
//#define LINEHEIGHT      16
//#define CHINESEWIDTH    16

#define NORMALCOLOR      0
#define INVERTCOLOR      1
#define DISABLECOLOR     2
#define READONLYCOLOR	 3
#define TITLECOLOR		 4

#define PE_SHOWCLIENT      0x00000000
#define PE_SHOWLINEEND     0x00000001
#define PE_SHOWTEXTEND     0x00000002
#define PE_SHOWSPECIALLEN  0x00000003
#define PE_SHOWALL	       0x00000004

#define ENTERCODE        0x0D
#define BACKSPACE		 0x08

#define MOTIONLESS       0x0000
#define MOVETOEND        0x0001
#define MOVETOHOME		 0x0002
#define MOVETOLEFT       0x0003
#define MOVETORIGHT      0x0004

//  Item Struct
struct EditItemStruct
{
	// Edit Buffer
	   LPTSTR lpPDAControlBuffer; // Title + EditBuffer
	   LPTSTR lpPDAEditBuffer;   // Display edit text
	   LPTSTR lpPDASaveEditBuffer;   // save edit text

		 short cbControlBufferLen; // length of control buffer
	   short cbTitleLen;     // the length of the title
	   short cbEditBufferLen; // length of edit buffer
	   short cbEditLen;  // current length of edit text
	   short cbEditLimitLen; // maximum length of the edit text

   // display position
     RECT rect;
     DWORD dwStyle;
     short iStartPos;
     short nDisplayx;      // the x position of the text to org of the window
	   short nDisplayLine; // the line of the text to org of the window

   // Caret Position
	   short nCaretx;  // the caret x position of the window
	   short nCaretLine; // the caret line of the window
	   LPTSTR lpCaretPosition; // the address of the caret

   // Invert area
	   LPTSTR lpInvertInitial;  // the address of the Initial invert 
	   LPTSTR lpInvertStart;  // the address of the start Invert
	   LPTSTR lpInvertEnd;  //  the address of the end Invert
	   LPTSTR lpOldInvertStart; // the addrss of the old start Invert
	   LPTSTR lpOldInvertEnd;  // the address of the old end Invert


     short nTotalLine;   // the total line of the current edit control
		 UINT  fModified;			// the edit is or not to be modified
		 TCHAR chPassWordWord;  // the current edit's password char

     LPTSTR lpFormat;        // the current edit's format string
	 EDITCALLBACK *lpCallBack;

   // Input 
	   CHAR HiByte;  // the input word high byte
	   CHAR LoByte;	// the input word low byte
	   BOOL  HalfByte; // receive half byte flag

		COLORREF cl_NormalText;
		COLORREF cl_NormalBkColor;

		COLORREF cl_InvertText;
		COLORREF cl_InvertBkColor;

		COLORREF cl_DisableText;
		COLORREF cl_DisableBkColor;

		COLORREF cl_ReadOnly;
		COLORREF cl_ReadOnlyBk ;

		COLORREF cl_Title;
		COLORREF cl_TitleBk ;
	// Color Control
};
typedef struct EditItemStruct   EDITITEM;
typedef struct EditItemStruct * LPEDITITEM;

// define Multiple Item Edit Struct
struct MultiItemEditStruct
{
	LPEDITITEM lpItemList;
	short nDisplayLine;
	short xDisplayPos;
	short *nItemStartLineList;
	short nItemNum;
	short nContextBottomPos;
	short iActiveItem;

		COLORREF cl_NormalText;
		COLORREF cl_NormalBkColor;

		COLORREF cl_InvertText;
		COLORREF cl_InvertBkColor;

		COLORREF cl_DisableText;
		COLORREF cl_DisableBkColor;

		COLORREF cl_ReadOnly;
		COLORREF cl_ReadOnlyBk ;

		COLORREF cl_Title;
		COLORREF cl_TitleBk ;
};
typedef struct MultiItemEditStruct MULTIITEMEDITSTRUCT;
typedef MULTIITEMEDITSTRUCT* LPMULTIITEMEDITSTRUCT;

struct FormatItemStruct
{
    LPTSTR lpEditText;  // current item's edit text 
    short cEditTextLen; // the length of the edit text 

    LPTSTR lpTileText;  // the current item's title text
    short cTileTextLen; // the length of the title text

    TCHAR chDataStyle;   // the current item's data style
    short cLimited;     // the max length of the edit text

    short nCoordinateX;  // the x position of the current item
    short nCoordinateLine;  // the y line of the current item
};
typedef struct FormatItemStruct FORMATITEM;
typedef struct FormatItemStruct * LPFORMATITEM;

#define FORMATITEMMAXNUM  10

struct FormatAnalysisStruct
{
    short ItemNum;    // the currnet edit have item number
    short ActiveItem;  // the active item no.

    LPTSTR lpTitle;    // the current format title 
    short cTitleLen;   // the length of the format title

    // the max Item num is 10
    FORMATITEM arrayFmtItem[FORMATITEMMAXNUM];  // the item data array , the max item number is 10
};
typedef struct FormatAnalysisStruct FORMATANALYSIS;
typedef struct FormatAnalysisStruct * LPFORMATANALYSIS;



// This function for debug
void ErrorDisplay(LPCTSTR lpCaption);

// public extern area
extern int Edit_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw);
extern int Edit_GetScrollPos(HWND hWnd, int nBar);
extern BOOL Edit_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw );

// public function declare area
extern int GetTextHeight(HWND hWnd);
extern void SendNotificationMsg( HWND hWnd, WORD wNotifyCode);
extern void SetCaretToNextItem(HWND hWnd,LPEDITITEM lpEditItem);
extern void ChangeSize(HWND hWnd,LPEDITITEM lpEditItem,short nChangedLine,BOOL bRedraw);
// static function declare area

extern void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,short cbEditLen);
extern void GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,short cbEditLen);

extern void ClearEditItemStruct(LPEDITITEM lpEditItem);
extern void DrawPDAEditControl(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,DWORD dwFlag);
extern BOOL CreatePDACaret(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL SetPDACaret(HWND hWnd);
extern BOOL ShowPDACaret(HWND hWnd);
extern BOOL HidePDACaret(HWND hWnd);
extern BOOL DeletePDACaret(HWND hWnd);
extern BOOL SetPDACaretPosition(HWND hWnd,LPEDITITEM lpEditItem,short xPos,short yPos);
extern void SetPDAEditColor(HWND hWnd,HDC hdc,short nColorStyle);
extern void DrawPDAEditSpecialText(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,LPTSTR lpShowAddress,
							   short nShowPos,short nShowLine,short nShowLen,DWORD dwFlag);
extern void GetCoordinate(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpPosition,short * lpxPos,short * lpnLine,BOOL bIsMultiLine);
extern void ClearInvert(HWND hWnd,LPEDITITEM lpEditItem, BOOL bFlag);
extern BOOL InsertChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR HiByte,CHAR LoByte);
extern BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle);
extern void ClearLine(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,short xPos,short nLine);
extern void ClearTextEnd(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,short xPos,short nLine);
extern void DisplayInvertChange(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL AdjustCaretInEditItem(HWND hWnd,LPEDITITEM lpEditItem);
extern int  BackSpaceChar(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL MoveCaretToPreChar(HWND hWnd,LPEDITITEM lpEditItem);
extern int DeleteCaretChar(HWND hWnd,LPEDITITEM lpEditItem);
extern int DeleteChar(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL DeleteInvert(HWND hWnd,LPEDITITEM lpEditItem);
extern void FillPassWordChar(LPTSTR lpEditBuffer,short nFillLength,TCHAR chPasswordChar);


extern LPTSTR GetLinePosition(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpLineStart,short xPos,BOOL bIsMultiLine,short * lpRealPos);
extern short GetxStartOfLine(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress);

extern LPTSTR GetLineAddress(HWND hWnd,LPEDITITEM lpEditItem,short nLine);
extern short GetTotalLine(HWND hWnd,LPEDITITEM lpEditItem);
extern void SetInvertArea(HWND hWnd,LPEDITITEM lpEditItem);
extern void SetCaretToStart(HWND hWnd,LPEDITITEM lpEditItem);
//extern short GetWindowWidth(HWND hWnd);
extern short GetWindowWidth(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL GetPDACharWidth(HWND hWnd,LPINT lpASCIICodeWidthBuffer,LPINT lpChineseCodeWidth);
extern void SetWindowRect(HWND hWnd,LPEDITITEM lpEditItem,BOOL bRedraw);

extern void SetPDAEditVScrollRange(HWND hWnd,short nTotalLine);
extern short GetPageLine(HWND hWnd);
extern void SetPDAEditScrollPage(HWND hWnd);
extern void ScrollCaretInPDAEdit(HWND hWnd,short nScrollLine);
extern short VScrollWindowInPDAEdit(HWND hWnd,short nScrollLine);
extern void HScrollWindowInPDAEdit(HWND hWnd,short nHScrollPos);
extern void SetPDAEditVScrollPos(HWND hWnd,short nNewScrollPos);
extern void SetPDAEditHScrollPos(HWND hWnd,short nNewScrollPos);
extern void DrawLineSeparate(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,short nCurLine);
extern void JumpToNextItem(HWND hWnd,LPEDITITEM lpEditItem);
extern short GetLineLength(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress,short *lpLineWidth);
extern void ReleasePDAEdit(HWND hWnd);
extern void InsertString(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpString,int cchLen);
extern void SetCaretCoordinate(HWND hWnd,LPEDITITEM lpEditItem);
extern short GetItem(HWND hWnd,short xPos,short yPos);
short HorzMoveCaret(HWND hWnd,LPEDITITEM lpEditItem,short iHorzPos);


extern LRESULT GetCharFromPos(HWND hWnd,LPEDITITEM lpEditItem,short xPos,short yPos);
extern LRESULT SetLimitText(HWND hWnd,LPEDITITEM lpEditItem,short cchMax);
extern LRESULT GetLineLengthFromPos(HWND hWnd,LPEDITITEM lpEditItem,short ich);
extern LRESULT GetPosFromChar(HWND hWnd,LPEDITITEM lpEditItem,short wCharIndex);
extern LRESULT SetPasswordChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR ch);
extern LRESULT CopyToClipboard(HWND hWnd,LPEDITITEM lpEditItem);
extern LRESULT PasteFromClipboard(HWND hWnd,LPEDITITEM lpEditItem);
extern LRESULT SetSel(HWND hWnd,LPEDITITEM lpEditItem,short nStart,short nEnd);
extern LRESULT SetText(HWND hWnd,LPEDITITEM lpEditItem,LPCSTR lpsz);
extern BOOL InCallBackPos(HWND hWnd,LPEDITITEM lpEditItem,short xPos,short yPos);
extern void SetEditVScrollPos(HWND hWnd,short nNewScrollPos);
extern void SetEditHScrollPos(HWND hWnd,short nNewScrollPos);

extern HBRUSH GetBkBrush(HWND hWnd);


#endif  //_EEBASE_H
