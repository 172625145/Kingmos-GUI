/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EEBASE_H

#include "eframe.h"
#include "eedit.h"

#define DITHERING_DEAL  // 需要抖动处理

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

		 int cbControlBufferLen; // length of control buffer
	   int cbTitleLen;     // the length of the title
	   int cbEditBufferLen; // length of edit buffer
	   int cbEditLen;  // current length of edit text
	   int cbEditLimitLen; // maximum length of the edit text

   // display position
     RECT rect;
     DWORD dwStyle;
     int iStartPos;
     int nDisplayx;      // the x position of the text to org of the window
	   int nDisplayLine; // the line of the text to org of the window

   // Caret Position
	   int nCaretx;  // the caret x position of the window
	   int nCaretLine; // the caret line of the window
	   LPTSTR lpCaretPosition; // the address of the caret

   // Invert area
	   LPTSTR lpInvertInitial;  // the address of the Initial invert 
	   LPTSTR lpInvertStart;  // the address of the start Invert
	   LPTSTR lpInvertEnd;  //  the address of the end Invert
	   LPTSTR lpOldInvertStart; // the addrss of the old start Invert
	   LPTSTR lpOldInvertEnd;  // the address of the old end Invert


     int nTotalLine;   // the total line of the current edit control
	 int bNeedCalcTotalLine; // need recalc the line of the edit control
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
	int nDisplayLine;
	int xDisplayPos;
	int *nItemStartLineList;
	int nItemNum;
	int nContextBottomPos;
	int iActiveItem;

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
    int cEditTextLen; // the length of the edit text 

    LPTSTR lpTileText;  // the current item's title text
    int cTileTextLen; // the length of the title text

    TCHAR chDataStyle;   // the current item's data style
    int cLimited;     // the max length of the edit text

    int nCoordinateX;  // the x position of the current item
    int nCoordinateLine;  // the y line of the current item
};
typedef struct FormatItemStruct FORMATITEM;
typedef struct FormatItemStruct * LPFORMATITEM;

#define FORMATITEMMAXNUM  10

struct FormatAnalysisStruct
{
    int ItemNum;    // the currnet edit have item number
    int ActiveItem;  // the active item no.

    LPTSTR lpTitle;    // the current format title 
    int cTitleLen;   // the length of the format title

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
extern void ChangeSize(HWND hWnd,LPEDITITEM lpEditItem,int nChangedLine,BOOL bRedraw);
// static function declare area

extern void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,int cbEditLen);
extern int GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,int cbEditLen);

extern void ClearEditItemStruct(LPEDITITEM lpEditItem);
extern void DrawPDAEditControl(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,DWORD dwFlag);
extern BOOL CreatePDACaret(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL SetPDACaret(HWND hWnd);
extern BOOL ShowPDACaret(HWND hWnd);
extern BOOL HidePDACaret(HWND hWnd);
extern BOOL DeletePDACaret(HWND hWnd);
extern BOOL SetPDACaretPosition(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos);
extern void SetPDAEditColor(HWND hWnd,HDC hdc,int nColorStyle);
extern void DrawPDAEditSpecialText(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,LPTSTR lpShowAddress,
							   int nShowPos,int nShowLine,int nShowLen,DWORD dwFlag);
extern void GetCoordinate(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpPosition,int * lpxPos,int * lpnLine,BOOL bIsMultiLine);
extern void ClearInvert(HWND hWnd,LPEDITITEM lpEditItem, BOOL bFlag);
extern BOOL InsertChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR HiByte,CHAR LoByte);
extern BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle);
extern void ClearLine(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int nLine);
extern void ClearTextEnd(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int nLine);
extern void DisplayInvertChange(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL AdjustCaretInEditItem(HWND hWnd,LPEDITITEM lpEditItem);
extern int  BackSpaceChar(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL MoveCaretToPreChar(HWND hWnd,LPEDITITEM lpEditItem);
extern int DeleteCaretChar(HWND hWnd,LPEDITITEM lpEditItem);
extern int DeleteChar(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL DeleteInvert(HWND hWnd,LPEDITITEM lpEditItem);
extern void FillPassWordChar(LPTSTR lpEditBuffer,int nFillLength,TCHAR chPasswordChar);


extern LPTSTR GetLinePosition(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpLineStart,int xPos,BOOL bIsMultiLine,int * lpRealPos);
extern int GetxStartOfLine(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress);

extern LPTSTR GetLineAddress(HWND hWnd,LPEDITITEM lpEditItem,int nLine);
extern int GetTotalLine(HWND hWnd,LPEDITITEM lpEditItem);
extern void SetInvertArea(HWND hWnd,LPEDITITEM lpEditItem);
extern void SetCaretToStart(HWND hWnd,LPEDITITEM lpEditItem);
//extern int GetWindowWidth(HWND hWnd);
extern int GetWindowWidth(HWND hWnd,LPEDITITEM lpEditItem);
extern BOOL GetPDACharWidth(HWND hWnd,LPINT lpASCIICodeWidthBuffer,LPINT lpChineseCodeWidth);
extern void SetWindowRect(HWND hWnd,LPEDITITEM lpEditItem,BOOL bRedraw);

extern void SetPDAEditVScrollRange(HWND hWnd,int nTotalLine);
extern int GetPageLine(HWND hWnd);
extern void SetPDAEditScrollPage(HWND hWnd);
extern void ScrollCaretInPDAEdit(HWND hWnd,int nScrollLine);
//extern int VScrollWindowInPDAEdit(HWND hWnd,int nScrollLine);
//extern void HScrollWindowInPDAEdit(HWND hWnd,int nHScrollPos);
//extern void SetPDAEditVScrollPos(HWND hWnd,int nNewScrollPos);
//extern void SetPDAEditHScrollPos(HWND hWnd,int nNewScrollPos);
extern void DrawLineSeparate(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int nCurLine);
extern void JumpToNextItem(HWND hWnd,LPEDITITEM lpEditItem);
extern int GetLineLength(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress,int *lpLineWidth);
//extern void ReleasePDAEdit(HWND hWnd);
extern void InsertString(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpString,int cchLen);
extern void SetCaretCoordinate(HWND hWnd,LPEDITITEM lpEditItem);
extern int GetItem(HWND hWnd,int xPos,int yPos);
int HorzMoveCaret(HWND hWnd,LPEDITITEM lpEditItem,int iHorzPos);


extern LRESULT GetCharFromPos(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos);
extern LRESULT SetLimitText(HWND hWnd,LPEDITITEM lpEditItem,int cchMax);
extern LRESULT GetLineLengthFromPos(HWND hWnd,LPEDITITEM lpEditItem,int ich);
extern LRESULT GetPosFromChar(HWND hWnd,LPEDITITEM lpEditItem,int wCharIndex);
extern LRESULT SetPasswordChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR ch);
extern LRESULT CopyToClipboard(HWND hWnd,LPEDITITEM lpEditItem);
extern LRESULT PasteFromClipboard(HWND hWnd,LPEDITITEM lpEditItem);
extern LRESULT SetSel(HWND hWnd,LPEDITITEM lpEditItem,int nStart,int nEnd);
extern LRESULT SetText(HWND hWnd,LPEDITITEM lpEditItem,LPCSTR lpsz);
extern BOOL InCallBackPos(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos);
extern void SetEditVScrollPos(HWND hWnd,int nNewScrollPos);
extern void SetEditHScrollPos(HWND hWnd,int nNewScrollPos);

//extern HBRUSH GetBkBrush(HWND hWnd);
extern HBRUSH GetBkBrush(HWND hWnd,LPEDITITEM lpEditItem);

#endif  //_EEBASE_H
