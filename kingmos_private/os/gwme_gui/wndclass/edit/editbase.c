/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：编辑类
版本号：1.0.0.456
开发时期：2001-12-26
作者：Jami chen 陈建明
修改记录：
	2004.07.12  修改颜色设置错误，当readme, diable的颜色与一般文本颜色不同是，会有部分
				使用一般文本的背景色，部分使用正确的颜色
**************************************************/
//#include <stdlib.h>
#include "eebase.h"
#include <eframe.h>
#include <estring.h>
#include "eclipbrd.h"
#include "ecaret.h"
#include "eGlobmem.h"
//#include <gwmesrv.h>

//#define LINE_BANK

/***********************************************************************************************/
 BOOL FormatEdit(HWND hWnd,LPEDITITEM lpEditItem,LPFORMATANALYSIS lpFmtAnalysis);
// BOOL FormatEdit(HWND hWnd,LPFORMATANALYSIS lpFmtAnalysis);
 int GetFmtTextLen(LPTSTR lpFmtBunch);
 int GetFmtNumeric(LPTSTR lpCurChar,LPTSTR lpNumericBuffer);
 void FormatEditText(HWND hWnd,LPEDITITEM lpEditItem,LPFORMATANALYSIS lpFmtAnalysis);
 void ShowFormatText(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,LPTSTR lpText,int nTextLen,int *lpnCurCharPos,int *lpnCurLine);
 SIZE GetSize(HWND hWnd,LPTSTR lpText,int cTextLen);
 int GetFmtItem(LPFORMATANALYSIS lpFmtAnalysis,LPTSTR lpCurChar);
 int CanInsertToFmtItem(LPFORMATANALYSIS lpFmtAnalysis,TCHAR HiByte,TCHAR LoByte);
/***********************************************************************************************/

// This function for debug
void ErrorDisplay(LPCTSTR lpCaption);

// public function declare area
//int GetTextHeight(HWND hWnd);
extern void SendNotificationMsg( HWND hWnd, WORD wNotifyCode);
//  function declare area
void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,int cbEditLen);
//void GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,int cbEditLen);

 void ClearEditItemStruct(LPEDITITEM lpEditItem);
 void DrawPDAEditControl(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,DWORD dwFlag);
 BOOL CreatePDACaret(HWND hWnd,LPEDITITEM lpEditItem);
 BOOL SetPDACaret(HWND hWnd);
 BOOL ShowPDACaret(HWND hWnd);
 BOOL HidePDACaret(HWND hWnd);
 BOOL DeletePDACaret(HWND hWnd);
 BOOL SetPDACaretPosition(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos);
 void SetPDAEditColor(HWND hWnd,HDC hdc,int nColorStyle);
 void DrawPDAEditSpecialText(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,LPTSTR lpShowAddress,
							   int nShowPos,int nShowLine,int nShowLen,DWORD dwStyle);
 void GetCoordinate(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpPosition,int * lpxPos,int * lpnLine,BOOL bIsMultiLine);
 void ClearInvert(HWND hWnd,LPEDITITEM lpEditItem, BOOL bFlag);
 BOOL InsertChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR HiByte,CHAR LoByte);
 BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle);
 void ClearLine(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos);
 void ClearTextEnd(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos);
 void ClearTextHead(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos);
 void DisplayInvertChange(HWND hWnd,LPEDITITEM lpEditItem);
 BOOL AdjustCaretInEditItem(HWND hWnd,LPEDITITEM lpEditItem);
 int  BackSpaceChar(HWND hWnd,LPEDITITEM lpEditItem);
 BOOL MoveCaretToPreChar(HWND hWnd,LPEDITITEM lpEditItem);
 int DeleteCaretChar(HWND hWnd,LPEDITITEM lpEditItem);
 int DeleteChar(HWND hWnd,LPEDITITEM lpEditItem);
// void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
 BOOL DeleteInvert(HWND hWnd,LPEDITITEM lpEditItem);
 void FillPassWordChar(LPTSTR lpEditBuffer,int nFillLength,TCHAR chPasswordChar);


 LPTSTR GetLinePosition(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpLineStart,int xPos,BOOL bIsMultiLine,int * lpRealPos);
 int GetxStartOfLine(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress);

 LPTSTR GetLineAddress(HWND hWnd,LPEDITITEM lpEditItem,int nLine);
 int GetTotalLine(HWND hWnd,LPEDITITEM lpEditItem);
 void SetInvertArea(HWND hWnd,LPEDITITEM lpEditItem);
 void SetCaretToStart(HWND hWnd,LPEDITITEM lpEditItem);
 int GetWindowWidth(HWND hWnd,LPEDITITEM lpEditItem);
 int GetWindowHeight(HWND hWnd);
 BOOL GetPDACharWidth(HWND hWnd,LPINT lpASCIICodeWidthBuffer,LPINT lpChineseCodeWidth);
 void SetWindowRect(HWND hWnd,LPEDITITEM lpEditItem,BOOL bRedraw);

 void SetPDAEditVScrollRange(HWND hWnd,int nTotalLine);
 int GetPageLine(HWND hWnd);
 void SetPDAEditScrollPage(HWND hWnd);
 void ScrollCaretInPDAEdit(HWND hWnd,int nScrollLine);
 void SetEditVScrollPos(HWND hWnd,int nNewScrollPos);
 void SetEditHScrollPos(HWND hWnd,int nNewScrollPos);
 void DrawLineSeparate(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int nCurLine);
// void JumpToNextItem(HWND hWnd,LPEDITITEM lpEditItem);
 int GetLineLength(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress,int *lpLineWidth);
 void InsertString(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpString,int cchLen);
 void SetCaretCoordinate(HWND hWnd,LPEDITITEM lpEditItem);


static void ShowCallBackFlag(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem);

/**************************************************
声明：void ClearEditItemStruct(LPEDITITEM lpEditItem)
参数：
	IN lpEdititem -- 编辑条目结构指针
返回值：无
功能描述：清除编辑条目结构。
引用: 
************************************************/
void ClearEditItemStruct(LPEDITITEM lpEditItem)
{  // clear Edit State
	    // Clear Edit buffer
		memset( lpEditItem->lpPDAEditBuffer, 0, lpEditItem->cbEditBufferLen );
		memset( lpEditItem->lpPDASaveEditBuffer, 0, lpEditItem->cbEditBufferLen );
		// Set display Poisition to (0,0)
		lpEditItem->nDisplayx=0;
		lpEditItem->nDisplayLine=0;
		
		// Set Caret Position to (0,0)
		lpEditItem->nCaretx=0;
		lpEditItem->nCaretLine=0;
		lpEditItem->lpCaretPosition=lpEditItem->lpPDAEditBuffer;
		
		// set Invert area to 0
		lpEditItem->lpInvertEnd=NULL;
		lpEditItem->lpInvertStart=NULL;
		lpEditItem->lpInvertInitial=NULL;
		lpEditItem->lpOldInvertEnd=NULL;
		lpEditItem->lpOldInvertStart=NULL;

		// Clear Modified flag
		lpEditItem->fModified=FALSE;

		// the default Password char is '*'
		lpEditItem->chPassWordWord='*';
		
		// No input char
		lpEditItem->HiByte=0;
    lpEditItem->LoByte=0;
    lpEditItem->HalfByte=FALSE;

//		lpEditItem->nTotalLine=-1;
}
/**************************************************
声明：void DrawPDAEditControl(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,DWORD dwFlag)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN lpEdititem -- 编辑条目结构指针
	IN dwFlag -- 绘制标志
返回值：无
功能描述：绘制编辑条目。
引用: 
************************************************/
 void DrawPDAEditControl(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,DWORD dwFlag)
{
		// display whole text from coordinate (0,0)
    DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,NULL,0,0,0,dwFlag);
//    ReleaseDC(hWnd,hdc);
		return;
}
/**************************************************
声明：BOOL CreatePDACaret(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：成功返回TRUE，否则返回FALSE
功能描述：创建光标。
引用: 
************************************************/
BOOL CreatePDACaret(HWND hWnd,LPEDITITEM lpEditItem)
{
	// Create a width is 2 ,and height is LINEHEIGHT Caret
	int nTextHeight;
	DWORD dwStyle;
//		dwStyle=WindowLong(hWnd,GWL_STYLE);
        dwStyle=lpEditItem->dwStyle;
//		if (dwStyle&ES_READONLY)
//			return FALSE;

		nTextHeight=GetTextHeight(hWnd);
        // 创建光标
#ifdef CPU_68K
		CreateCaret(hWnd,NULL,1,nTextHeight);
#else
		CreateCaret(hWnd,NULL,2,nTextHeight);
#endif
		return TRUE;
}
/**************************************************
声明：BOOL SetPDACaret(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置光标。
引用: 
************************************************/
BOOL SetPDACaret(HWND hWnd)
{
	int x,y;
//  LPEDITITEM lpEditItem;
//	BOOL Result;
	int nTextHeight;
	DWORD dwStyle;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

		// get focus
//		if (GetFocus()!=hWnd) return FALSE;
// 为什么要去掉这句话，加上 ？？？  Jami chen 2003.08.07
	if (GetFocus()!=hWnd) 
		return FALSE;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到创建风格
    if (dwStyle&ES_MULTIITEMEDIT)
    {
      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // 得到多条目结构句柄
      if (lpMultiItemEditStruct==NULL) return 0;
      lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到激活条目的结构结构
    }
    else
    {
      lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // 得到编辑区结构句柄
      if (lpEditItem==NULL) return 0;
    }


	// get line height
    nTextHeight=GetTextHeight(hWnd);

	    // Get struct EDITITEM data
//		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
//    if(lpEditItem==NULL) return FALSE;

		dwStyle=lpEditItem->dwStyle;  // 得到条目风格
//		if (dwStyle&ES_READONLY)
//			return FALSE;
		// Get Caret x position
	  x=lpEditItem->nCaretx;
		// Get Caret y Position
		y=lpEditItem->nCaretLine*nTextHeight+lpEditItem->iStartPos;

//    gotoxy(0,20);
//    printf("x=%d\n",x);
//    printf("y=%d\n",y);
		// Set Caret Position
		HidePDACaret(hWnd);
		SetCaretPos(x,y);  // 设置光标位置
		ShowPDACaret(hWnd);

		// Set Success
	    return TRUE;
}
/**************************************************
声明： BOOL ShowPDACaret(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：显示光标。
引用: 
************************************************/
 BOOL ShowPDACaret(HWND hWnd)
{
	ShowCaret(hWnd);  // 显示光标
	return TRUE;
}
/**************************************************
声明： BOOL HidePDACaret(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：隐藏光标。
引用: 
************************************************/
 BOOL HidePDACaret(HWND hWnd)
{
	HideCaret(hWnd);  // 隐藏光标
	return TRUE;
}
/**************************************************
声明： BOOL DeletePDACaret(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：删除光标。
引用: 
************************************************/
 BOOL DeletePDACaret(HWND hWnd)
{
// !!! Delete By Jami chen in 2004.07.15
// 在非同步模式（PostMessage）下，收到WM_KILLFOCUS
//时GetFocus()得到的窗口句柄已经不是当前窗口
	//if (GetFocus()!=hWnd)
	//	return FALSE;
// !!! Delete End By Jami chen in 2004.07.15
	DestroyCaret();  // 破坏光标
	return TRUE;
}
/**************************************************
声明： BOOL SetPDACaretPosition(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN xPos -- x坐标
	IN yPos -- y坐标
返回值：无
功能描述：设置光标位置。
引用: 
************************************************/
 BOOL SetPDACaretPosition(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos)
{
	DWORD dwStyle;
	int nLine,nTotalLine,nCaretPosition;
	LPTSTR lpLineAddress;
	int nPointCoordinate;
	int nTextHeight;
//  int nItem;
// !!! Add By Jami chen in 2004.10.18
	LPTSTR lpNewCaret;
// !!! Add End

		// get line height
		nTextHeight=GetTextHeight(hWnd);

//    yPos-=lpEditItem->iStartPos;
		// Get Edit Control Style
		dwStyle=lpEditItem->dwStyle;
		// Get the x_coordinate from the text origin
		nPointCoordinate=xPos+lpEditItem->nDisplayx;


		if (dwStyle&ES_MULTILINE)
		{ // the edit control is multiple line
			// get caret line position from window origin
			lpEditItem->nCaretLine=yPos/nTextHeight;

			// get this window text total line
			nTotalLine=GetTotalLine(hWnd,lpEditItem);
			// get caret line position from text origin
			nLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
			// if the caret line > text total line ,then set caret line
			// to the text end
			if (nTotalLine<=nLine)
				lpEditItem->nCaretLine=nTotalLine-lpEditItem->nDisplayLine-1;

			// if the caret line out the text origin the set caret line to
			// the text first line
			if (nLine<0)
			{
				nLine=0;
				lpEditItem->nCaretLine=0-lpEditItem->nDisplayLine;
			}
			// Get text address of the caret line
			lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
			// Get Caret Position
// !!! Modified By jami chen in 2004.10.18
//			lpEditItem->lpCaretPosition=GetLinePosition(hWnd,lpEditItem,lpLineAddress,nPointCoordinate,TRUE,&nCaretPosition);
			lpNewCaret=GetLinePosition(hWnd,lpEditItem,lpLineAddress,nPointCoordinate,TRUE,&nCaretPosition);
// !!! Modified End
		}
		else
		{ // the edit control is single line
			// the caret line always in the first line
			lpEditItem->nCaretLine=0;
			// Get Caret Position
//			lpEditItem->lpCaretPosition=GetLinePosition(hWnd,lpEditItem->lpPDAEditBuffer,nPointCoordinate,FALSE,&nCaretPosition);
// !!! Modified By jami chen in 2004.10.18
//			lpEditItem->lpCaretPosition=GetLinePosition(hWnd,lpEditItem,lpEditItem->lpPDAControlBuffer,nPointCoordinate,FALSE,&nCaretPosition);
			lpNewCaret=GetLinePosition(hWnd,lpEditItem,lpEditItem->lpPDAControlBuffer,nPointCoordinate,FALSE,&nCaretPosition);
// !!! Modified End
		}

// !!! Add By Jami chen in 2004.10.18
		if (lpNewCaret == lpEditItem->lpCaretPosition)
		{
			// 光标位置没有发生改变
			return FALSE;
		}
		// 设置新的光标位置
		lpEditItem->lpCaretPosition = lpNewCaret;
// !!! Add End

		if (lpEditItem->lpCaretPosition<lpEditItem->lpPDAEditBuffer)
		{   // 设置光标到起始位置
			SetCaretToStart(hWnd,lpEditItem);
		}
		else
		{
			// Get caret x-coordinate form window origin
			lpEditItem->nCaretx=nCaretPosition-lpEditItem->nDisplayx;
		}
		return TRUE;
}
/**************************************************
声明： LPTSTR GetLinePosition(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpLineStart,int xPos,BOOL bIsMultiLine,int * lpRealPos)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpLineStart -- 指定行的起始地址
	IN xPos -- 要调整的原始坐标
	IN bIsMultiLine -- 是否是多行编辑区
	OUT lpRealPos -- 调整后的实际位置
返回值：返回指定坐标的字符指针位置
功能描述：得到指定点在指定行中的实际位置。
引用: 
************************************************/
 LPTSTR GetLinePosition(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpLineStart,int xPos,BOOL bIsMultiLine,int * lpRealPos)
{
  int lpCharWidthBuffer[128];
//   LPEDITITEM lpEditItem;
  int ChineseCharWidth;
  int nAddWidth=0;
  int CurCharWidth,nWindowWidth;
  LPSTR lpCurChar,lpPreChar;
  DWORD dwStyle;
  FORMATANALYSIS FmtAnalysis;
  int nFMTItem;
  SIZE Size;

		// get struct EDITITEM data
//		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL) return NULL;
		// get window width
    nWindowWidth=GetWindowWidth(hWnd,lpEditItem);
		// Get Edit Control Style
//		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    dwStyle=lpEditItem->dwStyle;
		// get Char Width for ASCII Code
    GetPDACharWidth(hWnd,lpCharWidthBuffer,&ChineseCharWidth);

		nAddWidth=GetxStartOfLine(hWnd,lpEditItem,lpLineStart);
		// Get the line Start Address
		lpCurChar=lpLineStart;
		lpPreChar=lpLineStart;
    if (dwStyle&ES_FORMAT)
    {  // 当前条目是格式化条目
      FormatEdit(hWnd,lpEditItem,&FmtAnalysis);  // 格式化编辑区
      nFMTItem=GetFmtItem(&FmtAnalysis,lpCurChar)-1;; // 得到格式化条目
    }
		while(TRUE)
		{
      if (dwStyle&ES_FORMAT)
      { // the current edit is a format edit
          if (lpCurChar==lpEditItem->lpPDAEditBuffer)
          {  // is must calculate the size of the format title
            Size=GetSize(hWnd,FmtAnalysis.lpTitle,
                                    FmtAnalysis.cTitleLen);
            CurCharWidth=(int)Size.cx;
            nAddWidth+=CurCharWidth;
          }
      }
			// to the text end?
			if (*lpCurChar==0x00) 
			{
				// This Line Must include Last Code 
				lpPreChar=lpCurChar;
				break;
			}
			// Is or not Multiple Line edit Control
			if (bIsMultiLine)
			{   // is or not enter code ?
				if (*lpCurChar==0x0d&&*(lpCurChar+1)==0x0a) 
				{
					lpPreChar=lpCurChar; 
					break;
				}
			}
			// Get Current Character Width
//			if (*lpCurChar<0)
			if (*lpCurChar<0 || *lpCurChar>0x7F)
			{  // current Character is a chinese
				CurCharWidth=ChineseCharWidth;
				// to next character
				lpPreChar=lpCurChar;
				lpCurChar+=2;
			}
			else
			{  // current character is english or sambol
          if ((dwStyle&ES_FORMAT)&&(*lpCurChar==FMTSEPARATE))
          { // the current edit is a format edit and the current character is system separate char
            Size=GetSize(hWnd,(FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                    (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen);
            CurCharWidth=(int)Size.cx;
						// to next item
            nFMTItem++;
						if (nFMTItem>=FORMATITEMMAXNUM)
							nFMTItem=FORMATITEMMAXNUM-1;
          }
          else
			      CurCharWidth=lpCharWidthBuffer[*lpCurChar];
				//to next character
				lpPreChar=lpCurChar;
				lpCurChar++;
			}
			// Is or not Special Position
			if (nAddWidth+CurCharWidth>xPos) break;

			if ((dwStyle&ES_AUTONEWLINE)&&bIsMultiLine)
			{
				// Is or not to the edge the window
				if (nAddWidth+CurCharWidth>nWindowWidth) break;
			}
			// add current character width
			nAddWidth+=CurCharWidth;

		}
		// return Current character Position

    if (lpRealPos)
		  *lpRealPos=nAddWidth;
		// return needed width
		return lpPreChar;
}
/**************************************************
声明： int GetxStartOfLine(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpLineAddress -- 指定行的指针
返回值：返回指定行的起始坐标
功能描述：得到指定行的起始坐标。
引用: 
************************************************/
 int GetxStartOfLine(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress)
{
	DWORD dwStyle;
	int nWindowWidth,nLineWidth;
	int xStart;

			dwStyle=lpEditItem->dwStyle;
			if (!((dwStyle&ES_CENTER)||(dwStyle&ES_RIGHT)))
			{ // this edit is left-aligns
				return 0;
			}
			nWindowWidth=GetWindowWidth(hWnd,lpEditItem); // 得到窗口宽度
		    GetLineLength(hWnd,lpEditItem,(LPCTSTR)lpLineAddress,(int *)&nLineWidth); // 得到当前行的字符宽度
	        xStart=0; 
			if (dwStyle&ES_CENTER)
			{
				xStart=(nWindowWidth-nLineWidth)/2; // 将字符显示在中间
			}
			if (dwStyle&ES_RIGHT)
			{
				xStart=nWindowWidth-nLineWidth; // 将字符显示在右边
			}
			return xStart;
}
/**************************************************
声明： LPTSTR GetLineAddress(HWND hWnd,LPEDITITEM lpEditItem,int nLine)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN nLine -- 指定行索引
返回值：返回指定行的地址
功能描述：得到指定行的地址。
引用: 
************************************************/
 LPTSTR GetLineAddress(HWND hWnd,LPEDITITEM lpEditItem,int nLine)
{
//    LPEDITITEM lpEditItem;
	DWORD dwStyle;
	LPTSTR lpCurChar,lpLineAddress;
	int nAddLen;
	int nCurLine;
  int lpCharWidthBuffer[128];
  int ChineseCharWidth,nCurCharWidth,nWindowWidth;
  int nCurCharPos=0;
  FORMATANALYSIS FmtAnalysis;
  int nFMTItem;
  SIZE Size;


		// Get struct EDITITEM data
//		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL) return NULL;
    // Get Char Width
    GetPDACharWidth(hWnd,lpCharWidthBuffer,&ChineseCharWidth);
    // Get Window Width
	// {modified by chen jian ming 2000-08-30
    //nWindowWidth=lpEditItem->rect.right-lpEditItem->rect.left;
	// } to {
	nWindowWidth=GetWindowWidth(hWnd,lpEditItem);
	// end} 2000-08-30
			// Get Edit Control Style
		dwStyle=lpEditItem->dwStyle;

		if (!(dwStyle&ES_MULTILINE))
		{  // the edit control is single line edit
			//if (nLine==0) 
				return lpEditItem->lpPDAControlBuffer;
			//else 
			//	return NULL;
		}
		// initialize variable
		lpCurChar=lpLineAddress=lpEditItem->lpPDAControlBuffer;
		nAddLen=0;
		nCurLine=0;

    if (dwStyle&ES_FORMAT)
    { // The current Edit is a format edit
      FormatEdit(hWnd,lpEditItem,&FmtAnalysis);
      nFMTItem=0;
    }
		while(TRUE)
		{
			// the current line is need line , then exit circle 
			if (nCurLine==nLine)
				break;
			if (*lpCurChar==0x00||nAddLen==(lpEditItem->cbEditLen+lpEditItem->cbTitleLen))
			{  // this is to end edit buffer
				// exit circle
        if (dwStyle&ES_FORMAT)
        {  // must calculate the last item's tile text
            Size=GetSize(hWnd,(FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                    (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen);
            nCurCharWidth=Size.cx;  // 得到字符宽度
        	  if (dwStyle&ES_AUTONEWLINE)
	          {	// 自动换行
		          if (nCurCharPos+nCurCharWidth>nWindowWidth)
              { // 需要换行
                lpLineAddress=lpCurChar;
                nCurCharPos=0;
              }
            }
        }
				break;
			}
      if (dwStyle&ES_FORMAT)
      { // Current edit is a format edit 
          if (lpCurChar==lpEditItem->lpPDAEditBuffer)
          {  // the start of the edit , mist calculate the format title first
            Size=GetSize(hWnd,FmtAnalysis.lpTitle,
                                    FmtAnalysis.cTitleLen);
            nCurCharWidth=Size.cx;
      		  if (nCurCharPos+nCurCharWidth>nWindowWidth)
            {  // 需要换行
      			   nCurLine++;
               nCurCharPos=nCurCharWidth;
               continue; 
            }
            nCurCharPos+=nCurCharWidth;
          }
      }
//			if (*lpCurChar<0)
			if (*lpCurChar<0 || *lpCurChar>0x7F)
			{  // the current character is chinese
        nCurCharWidth=ChineseCharWidth;
			}
			else
			{  // the current character is english or symbol
        if ((dwStyle&ES_FORMAT)&&(*lpCurChar==FMTSEPARATE))
        {  // the current character is system separate code
            Size=GetSize(hWnd,(FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                    (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen);
            nCurCharWidth=Size.cx;
						// to next item 
            nFMTItem++;
						if (nFMTItem>=FORMATITEMMAXNUM)
							nFMTItem=FORMATITEMMAXNUM-1;
        }
        else
        { // get current character width
            nCurCharWidth=lpCharWidthBuffer[*lpCurChar];
        }
			}
			if (*lpCurChar==0x0d&&*(lpCurChar+1)==0x0a)
			{  // this code is enter code , must display this line and to next line
				// to next char
				lpCurChar+=2;
				nAddLen+=2;
				lpLineAddress=lpCurChar;
				nCurLine++;
        nCurCharPos=0;
				// to next circle
				continue;
			}

			if (dwStyle&ES_AUTONEWLINE)
			{	 // current edit may auto newline
					if (nCurCharPos+nCurCharWidth>nWindowWidth)
					{ // to next line
						nCurLine++;
						lpLineAddress=lpCurChar;
						nCurCharPos=0;
					}
			}
			// to next char
      nCurCharPos+=nCurCharWidth;
//			if (*lpCurChar<0)
			if (*lpCurChar<0 || *lpCurChar>0x7F)
			{  // the current character is chinese
				lpCurChar+=2;
				nAddLen+=2;
			}
			else
			{  // the current character is english or symbol
				lpCurChar++;
				nAddLen++;
			}
		}
		// return current line start address
		return lpLineAddress;		
}
/**************************************************
声明： int GetTotalLine(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：返回编辑条目的总行数
功能描述：得到编辑条目的总行数。
引用: 
************************************************/
 int GetTotalLine(HWND hWnd,LPEDITITEM lpEditItem)
{
//    LPEDITITEM lpEditItem;
	DWORD dwStyle;
	LPTSTR lpCurChar;
	int nAddLen;
	int nTotalLine;
  int lpCharWidthBuffer[128];
  int ChineseCharWidth;
  int nCurCharWidth,nWindowWidth;
  int nCurCharPos=0,nCurCharLen;
  FORMATANALYSIS FmtAnalysis;
  int nFMTItem;
  SIZE Size;

	if (lpEditItem->bNeedCalcTotalLine == FALSE)
		return lpEditItem->nTotalLine;
	
	lpEditItem->bNeedCalcTotalLine = FALSE;
  // Get Char Width
    GetPDACharWidth(hWnd,lpCharWidthBuffer,&ChineseCharWidth);
    // Get Window Width
	// {modified by chen jian ming 2000-08-30
//    nWindowWidth=lpEditItem->rect.right-lpEditItem->rect.left;
	//} to {
	nWindowWidth=GetWindowWidth(hWnd,lpEditItem);
	// end} 2000-08-30
		// Get struct EDITITEM data
//		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL) return 0;
		// Get Edit Control Style
		dwStyle=lpEditItem->dwStyle;
		// get edit text length
		lpEditItem->cbEditLen=strlen(lpEditItem->lpPDAEditBuffer);

		if (!(dwStyle&ES_MULTILINE))
		{  // the edit control is single line edit , Only 1 line
			return 1;
		}
		// initialize variable
//		lpCurChar=lpEditItem->lpPDAEditBuffer;
		lpCurChar=lpEditItem->lpPDAControlBuffer;
		nAddLen=0;
		nTotalLine=1;

    if (dwStyle&ES_FORMAT)
    { // 是格式化条目
      FormatEdit(hWnd,lpEditItem,&FmtAnalysis);  // 格式化编辑区
      nFMTItem=0;
      Size=GetSize(hWnd,FmtAnalysis.lpTitle,FmtAnalysis.cTitleLen); // 得到条目尺寸
      nCurCharWidth=(int)Size.cx;  // 当前字符宽度为条目宽度
      nCurCharPos=nCurCharWidth;
    }

		while(TRUE)
		{
			if (*lpCurChar==0x00||nAddLen==(lpEditItem->cbEditLen+lpEditItem->cbTitleLen))
			{  // this is to end edit buffer
        if (dwStyle&ES_FORMAT)
        {  // 当前为格式化条目
          Size=GetSize(hWnd,(FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                  (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen);
          nCurCharWidth=(int)Size.cx;
			    if (dwStyle&ES_AUTONEWLINE)
			    {  // 可以自动换行
			      if (nCurCharPos+nCurCharWidth>nWindowWidth)
			      { // 要求换行 
				    nTotalLine++;
				    nCurCharPos=0;
			      }
			    }
        }
				// exit circle
				break;
			}
//			if (*lpCurChar<0)
			if (*lpCurChar<0 || *lpCurChar>0x7F)
			{  // the current character is chinese
        nCurCharWidth=ChineseCharWidth;
        nCurCharLen=2;
			}
			else
			{  // the current character is english or symbol
        if ((dwStyle&ES_FORMAT)&&(*lpCurChar==FMTSEPARATE))
        {  // 格式化条目分隔符
          Size=GetSize(hWnd,(FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                  (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen);
          nCurCharWidth=(int)Size.cx;
          nFMTItem++; // 下一个条目
					if (nFMTItem>=FORMATITEMMAXNUM)
						nFMTItem=FORMATITEMMAXNUM-1;
        }
        else
          nCurCharWidth=lpCharWidthBuffer[*lpCurChar];  // 得到当前字符的宽度
        nCurCharLen=1;
			}
			if (*lpCurChar==0x0d&&*(lpCurChar+1)==0x0a)
			{  // this code is enter code , to next line
				// to next char
				lpCurChar+=2;
				nAddLen+=2;
				nTotalLine++;
        nCurCharPos=0;
				// to next circle
				continue;
			}
			if (dwStyle&ES_AUTONEWLINE)
			{  // 可以自动换行
			  if (nCurCharPos+nCurCharWidth>nWindowWidth)
			  {  // 要求换行 
				nTotalLine++;
				nCurCharPos=0;
			  }
			}
			// to next char
			lpCurChar+=nCurCharLen;
			nAddLen+=nCurCharLen;
      nCurCharPos+=nCurCharWidth;
		}
		// return Total line 
//    Print(10,430,nTotalLine);
		return nTotalLine;		
}
/**************************************************
声明： void SetInvertArea(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：无
功能描述：设置编辑去的选择区域。
引用: 
************************************************/
 void SetInvertArea(HWND hWnd,LPEDITITEM lpEditItem)
{
//   LPEDITITEM lpEditItem;

		// Get struct EDITITEM data
//		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL) return;
		// save old Invert area
		lpEditItem->lpOldInvertStart=lpEditItem->lpInvertStart;
		lpEditItem->lpOldInvertEnd=lpEditItem->lpInvertEnd;

		// The Invert Initial point is or not exist
		if (lpEditItem->lpInvertInitial==NULL)
		{ // Is not exist ,then set Invert area to the caret position
			lpEditItem->lpInvertStart=lpEditItem->lpInvertEnd=lpEditItem->lpCaretPosition;
			lpEditItem->lpInvertInitial=lpEditItem->lpCaretPosition;
		}
		else
		{ // Is exist , then set new area
			if (lpEditItem->lpCaretPosition>lpEditItem->lpInvertInitial)
			{ // if caret position is behind the initial point
				// the Invert start in the initial point and the end in the caret position
				lpEditItem->lpInvertStart=lpEditItem->lpInvertInitial;
				lpEditItem->lpInvertEnd=lpEditItem->lpCaretPosition;
			}
			else
			{  // the Invert start in the caret position and the end in the initial point
				lpEditItem->lpInvertEnd=lpEditItem->lpInvertInitial;
				lpEditItem->lpInvertStart=lpEditItem->lpCaretPosition;
			}
		}
}
/**************************************************
声明：void SetPDAEditColor(HWND hWnd,HDC hdc,int nColorStyle)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN nColorStyle -- 颜色类型
返回值：无
功能描述：设置设备颜色。
引用: 
************************************************/
void SetPDAEditColor(HWND hWnd,HDC hdc,int nColorStyle)
{
	COLORREF cl_Color;
	CTLCOLORSTRUCT CtlColor;


		CtlColor.fMask = 0xffffffff;
		SendMessage(hWnd,WM_GETCTLCOLOR,0,(LPARAM)&CtlColor);
		if (nColorStyle==NORMALCOLOR)
		{ // Set normal color
			// set Text Color same with normal window
//			SetTextColor(hdc,CL_BLACK);
//			cl_Color = SendMessage(hWnd,EM_GETCOLOR,NORMALTEXTCOLOR,0);
			cl_Color = CtlColor.cl_Text;
			SetTextColor(hdc,cl_Color);
			// set Text background Color same with normal window
//			SetBkColor(hdc,CL_WHITE);
//			cl_Color = SendMessage(hWnd,EM_GETCOLOR,NORMALTEXTBKCOLOR,0);
			cl_Color = CtlColor.cl_TextBk;
			SetBkColor(hdc,cl_Color);
		}
		if (nColorStyle==INVERTCOLOR)
		{ // set invert color
			// set Text Color same with Invert Text
//			SetTextColor(hdc,CL_WHITE);
//			cl_Color = SendMessage(hWnd,EM_GETCOLOR,NORMALINVERTCOLOR,0);
			cl_Color = CtlColor.cl_Selection;
			SetTextColor(hdc,cl_Color);
			// set Text background Color same with Invert text background
//			SetBkColor(hdc,CL_BLACK);
//			cl_Color = SendMessage(hWnd,EM_GETCOLOR,NORMALINVERTBKCOLOR,0);
			cl_Color = CtlColor.cl_SelectionBk;
			SetBkColor(hdc,cl_Color);
		}
		if (nColorStyle==DISABLECOLOR)
		{ // set invert color
			cl_Color = CtlColor.cl_Disable;
			SetTextColor(hdc,cl_Color);

			cl_Color = CtlColor.cl_DisableBk;
			SetBkColor(hdc,cl_Color);
		}
		if (nColorStyle==TITLECOLOR)
		{ // set invert color
			// set Text Color same with Invert Text
			cl_Color = CtlColor.cl_Title;
			SetTextColor(hdc,cl_Color);

			cl_Color = CtlColor.cl_TitleBk;
			SetBkColor(hdc,cl_Color);
		}
		if (nColorStyle==READONLYCOLOR)
		{ // set invert color
			// set Text Color same with Invert Text
			cl_Color = CtlColor.cl_ReadOnly;
			SetTextColor(hdc,cl_Color);

			cl_Color = CtlColor.cl_ReadOnlyBk;
			SetBkColor(hdc,cl_Color);
		}
}
/**************************************************
声明： void DrawPDAEditSpecialText(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,LPTSTR lpShowAddress,
							   int nShowPos,int nShowLine,int nShowWordLen,DWORD dwFlag)

参数：
	IN hWnd : Handle of the window
	IN hdc  : handle of the device context
	IN lpEditItem : the pointer of the struct EDITITEM
	IN lpShowAddress : It is will be Showed Start Address ,if dwFlag is PE_SHOWCLIENT ,this parameter is ignore     
	IN nShowPos      : This is will be showed Start Position
	IN nShowLine     : This is will be showed start Line
	IN nShowWordLen  : This is will be showed word number,if dwFlag is PE_SHOWSPECIALLEN ,this parameter is valid,
	                other is ignore
	IN dwFlag        : you need Draw style
	    PE_SHOWCLIENT  : draw the whole text
 	   PE_SHOWLINEEND : draw the lpShowAddress from the appoint position to the end of the line
  	  PE_SHOWTEXTEND : draw the lpShowAddress from the appoint position to the end of the TEXT
   	 PE_SHOWSPECIALLEN: draw the lpShowAddress from the appoint position used the appoint len
返回值：无
功能描述：绘制指定文本。
引用: 
************************************************/
 void DrawPDAEditSpecialText(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,LPTSTR lpShowAddress,
							   int nShowPos,int nShowLine,int nShowWordLen,DWORD dwFlag)
{
	int lpCharWidthBuffer[128];
	int ChineseCharWidth;
	DWORD dwStyle;
	LPTSTR lpCurChar,lpLineAddress;
	int x,y;
	int nAddLen,nShowLen;
	int nCurLine;
	int nCurCharPos,nCurShowPos;
	int nTextHeight;
	int nWindowWidth,nCurCharWidth,nCurCharLen,nWindowHeight;
  int nFMTItem;
  FORMATANALYSIS FmtAnalysis;
  RECT rcClientRect;

		// Send Notification message EN_UPDATE to parent window
		SendNotificationMsg( hWnd,EN_UPDATE);
		// get Line Height
		nTextHeight=GetTextHeight(hWnd);
		// get window width
    nWindowWidth=GetWindowWidth(hWnd,lpEditItem);
	if (dwFlag==PE_SHOWALL)
		nWindowHeight=0x7fff;  // can show untile the end
	else
		nWindowHeight=GetWindowHeight(hWnd);
		// get Char Width for ASCII Code and chinese Code
    GetPDACharWidth(hWnd,lpCharWidthBuffer,&ChineseCharWidth);

//    RETAILMSG(1,( "!!!!!Chinese CHAR Width  = %d\r\n",ChineseCharWidth));

		// Get Edit Control Style
//		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    dwStyle=lpEditItem->dwStyle;

		// initial variable
		if (dwFlag==PE_SHOWCLIENT||dwFlag==PE_SHOWALL)
		{ // begin from the top of the window
	  	nCurLine=lpEditItem->nDisplayLine;
      lpCurChar=lpLineAddress=GetLineAddress(hWnd,lpEditItem,nCurLine);
		}
		else
		{  // begin from the special address
			if (lpShowAddress==NULL) return;
		  nCurLine=nShowLine;
			lpCurChar=lpLineAddress=lpShowAddress;
		}
//		nClearPos=(int)(nShowPos-lpEditItem->nDisplayx);
//		RETAILMSG(1,("Draw %s \r\n",lpCurChar));

		if ((dwStyle&ES_CENTER)||(dwStyle&ES_RIGHT))
		{ // 对齐方式为中间对齐或右边对齐
			lpCurChar=lpLineAddress=GetLineAddress(hWnd,lpEditItem,nCurLine); // 得到行起始位置
			nShowPos=GetxStartOfLine(hWnd,lpEditItem,lpCurChar); // 得到开始显示位置
			dwFlag=PE_SHOWTEXTEND; // 显示到文本结束
			x=nShowPos-lpEditItem->nDisplayx; // 得到开始显示位置
			y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
			ClearTextHead(hWnd,hdc,lpEditItem,x,y); // 清除文本前面空白
		}
		// Set Normal Color
		// the initial position in the invert area ,then must set the invert color
		if (lpEditItem->dwStyle&WS_DISABLED)
		{  // 当前为DISABLE
			SetPDAEditColor(hWnd,hdc,DISABLECOLOR);
		}
		else
		{
			if (lpCurChar>=lpEditItem->lpInvertStart&&lpCurChar<lpEditItem->lpInvertEnd)
			{  // 在选择范围内
				SetPDAEditColor(hWnd,hdc,INVERTCOLOR);
			}
			else if (lpCurChar<lpEditItem->lpPDAEditBuffer)
			{ // 是标题
				SetPDAEditColor(hWnd,hdc,TITLECOLOR);
			}
			else if (lpEditItem->dwStyle&ES_READONLY)
			{ // 是只读
				SetPDAEditColor(hWnd,hdc,READONLYCOLOR);
			}
			else
			{  // 一般文本
				SetPDAEditColor(hWnd,hdc,NORMALCOLOR);
			}

		}

		nAddLen=nShowLen=0;
		nCurCharPos=nShowPos;
		nCurShowPos=nShowPos;

		GetClientRect(hWnd,&rcClientRect);
		//  first hide the caret
		HidePDACaret(hWnd);
		// Clear Rewrite Area : ClearRect
/*
		switch (dwFlag)
		{
			case PE_SHOWCLIENT:
				// Clear the whole client area
				ClearTextEnd(hWnd,hdc,0,0);
				break;
			case PE_SHOWLINEEND:
				// clear the appoint line
//				ClearLine(hWnd,(nShowPos-lpEditItem->nDisplayx),(nShowLine-lpEditItem->nDisplayLine));
//				break;
			case PE_SHOWTEXTEND:
				// clear the appoint line to the end
				ClearTextEnd(hWnd,hdc,nClearPos,(int)(nShowLine-lpEditItem->nDisplayLine));
//				ClearTextEnd(hWnd,0,0);
        break;
		}
*/
    if (dwStyle&ES_FORMAT)
    {  // 是格式化条目
      FormatEdit(hWnd,lpEditItem,&FmtAnalysis);
      nFMTItem=GetFmtItem(&FmtAnalysis,lpCurChar)-1;;
    }

    while(TRUE)
		{
// Add By Jami chen in 2003.06.30
			if (lpCurChar == lpEditItem->lpPDAEditBuffer)
			{
					// display Invert area text
					x=nCurShowPos-lpEditItem->nDisplayx;
					y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
					// display this line
					if (y>=0&&y<nWindowHeight)
					{
//						RETAILMSG(1,("TextOut0 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
						TextOut(hdc,x,y,lpLineAddress,nShowLen);
					}

					// set Next Display Area : Invert Area
					lpLineAddress=lpCurChar;
					nShowLen=0;
					nCurShowPos=nCurCharPos;

					// restore normal area color
					if (!(lpEditItem->dwStyle&WS_DISABLED))
					{
// !!! modified by jami chen in 2004.7.12
//						SetPDAEditColor(hWnd,hdc,NORMALCOLOR);
						if (lpEditItem->dwStyle&ES_READONLY)
						{ // 是只读
							SetPDAEditColor(hWnd,hdc,READONLYCOLOR);
						}
						else
						{  // 一般文本
							SetPDAEditColor(hWnd,hdc,NORMALCOLOR);
						}
// !!! modified End by jami chen in 2004.7.12
					}
			}
// !!! Add End By Jami chen in  2003.06.30
			if (dwFlag==PE_SHOWSPECIALLEN&&nAddLen>=nShowWordLen)
			{  // is or not display need draw length
				x=nCurShowPos-lpEditItem->nDisplayx;
				y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
				if (y>=0&&y<nWindowHeight)
				{
//					RETAILMSG(1,("TextOut1 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
					TextOut(hdc,x,y,lpLineAddress,nShowLen);
				}
				if (dwStyle&ES_LINESEPARATE)
					DrawLineSeparate(hWnd,hdc,lpEditItem,(int)(nCurLine-lpEditItem->nDisplayLine));
				// exit circle
				break;
			}
			if (*lpCurChar==0x00||nAddLen==(lpEditItem->cbEditLen+lpEditItem->cbTitleLen))
			{  // this is to end edit text
				x=nCurShowPos-lpEditItem->nDisplayx;
				y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
				if (y>=0&&y<nWindowHeight)
		        {
//					   RETAILMSG(1,("TextOut2 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
					   TextOut(hdc,x,y,lpLineAddress,nShowLen);
					   if (dwStyle&ES_FORMAT)
						{ // display the last item tile
							ShowFormatText(hWnd,hdc,lpEditItem,
                                   (FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                   (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen,
                                   &nCurCharPos,
                                   &nCurLine);
						}

//           ClearLine(hWnd,hdc,lpEditItem,(nCurCharPos-lpEditItem->nDisplayx),nCurLine);
				        ClearTextEnd(hWnd,hdc,lpEditItem,(int)(nCurCharPos-lpEditItem->nDisplayx),y);

#ifndef LINE_BANK
						if (dwStyle&ES_LINESEPARATE)
							DrawLineSeparate(hWnd,hdc,lpEditItem,(int)(nCurLine-lpEditItem->nDisplayLine));
#endif
				}
				// exit circle
				break;
			}
//			RETAILMSG(1,( "*lpCurChar  = %d \r\n",*lpCurChar));
//			if (*lpCurChar<0)
			if (*lpCurChar<0 || *lpCurChar>0x7F)
			{ // the current character is Chinese
				nCurCharWidth=ChineseCharWidth;
//			    RETAILMSG(1,( "nCurCharWidth 000 = %d \r\n",nCurCharWidth));
				nCurCharLen=2;
			}
      else
      { // the currrent character is English and Symbol
        nCurCharWidth=lpCharWidthBuffer[*lpCurChar];
//		RETAILMSG(1,( "nCurCharWidth 111 = %d \r\n",nCurCharWidth));
        nCurCharLen=1;
      }
			if (lpEditItem->lpInvertStart!=lpEditItem->lpInvertEnd)
			{  // is exist invert area
				if (lpCurChar==lpEditItem->lpInvertStart)
				{  // this is to invert start ,enter the invert area
					// display this line start position
					x=nCurShowPos-lpEditItem->nDisplayx;
					y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;

					// display this line
					if (y>=0&&y<nWindowHeight)
					{
//						RETAILMSG(1,("TextOut3 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
						TextOut(hdc,x,y,lpLineAddress,nShowLen);
					}

					// set Next Display Area : Invert Area
					nCurShowPos=nCurCharPos;
					lpLineAddress=lpCurChar;
					nShowLen=0;

					// Set Invert area color
					SetPDAEditColor(hWnd,hdc,INVERTCOLOR);
				}
				if (lpCurChar==lpEditItem->lpInvertEnd)
				{  // this is to invert end , out the invert area

					// display Invert area text
					x=nCurShowPos-lpEditItem->nDisplayx;
					y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
					// display this line
					if (y>=0&&y<nWindowHeight)
					{
//						RETAILMSG(1,("TextOut4 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
						TextOut(hdc,x,y,lpLineAddress,nShowLen);
					}

					// set Next Display Area : Invert Area
					lpLineAddress=lpCurChar;
					nShowLen=0;
					nCurShowPos=nCurCharPos;

					// restore normal area color
					if (!(lpEditItem->dwStyle&WS_DISABLED))
					{
// !!! modified End by jami chen in 2004.7.12
//						SetPDAEditColor(hWnd,hdc,NORMALCOLOR);
						if (lpEditItem->dwStyle&ES_READONLY)
						{ // 是只读
							SetPDAEditColor(hWnd,hdc,READONLYCOLOR);
						}
						else
						{  // 一般文本
							SetPDAEditColor(hWnd,hdc,NORMALCOLOR);
						}
// !!! modified End by jami chen in 2004.7.12
					}
				}
			}
      if (dwStyle&ES_FORMAT)
      {
          if ((lpCurChar==lpEditItem->lpPDAEditBuffer)&&(dwFlag==PE_SHOWCLIENT||dwFlag==PE_SHOWALL))
          { // will display the format title text , must display the edit text
					  x=nCurShowPos-lpEditItem->nDisplayx;
					  y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
					  // display this line
						if (y>=0&&y<nWindowHeight)
						{
//						  RETAILMSG(1,("TextOut6 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
						  TextOut(hdc,x,y,lpLineAddress,nShowLen);
						}
						// display the format title
            ShowFormatText(hWnd,hdc,lpEditItem,
                                  FmtAnalysis.lpTitle,
                                  FmtAnalysis.cTitleLen,
                                  &nCurCharPos,
                                  &nCurLine);
            // set Next Display Area : format item edit text
            nCurShowPos=nCurCharPos;
					  lpLineAddress=lpCurChar;
					  nShowLen=0;
          }
          if (*lpCurChar==FMTSEPARATE)
          {// will display the next item text
						// display this item text first
					  x=nCurShowPos-lpEditItem->nDisplayx;
					  y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
					  // display this line
						if (y>=0&&y<nWindowHeight)
						{
//						  RETAILMSG(1,("TextOut7 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
						  TextOut(hdc,x,y,lpLineAddress,nShowLen);
						}

						// display this item tile text
            ShowFormatText(hWnd,hdc,lpEditItem,
                                  (FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                  (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen,
                                  &nCurCharPos,
                                  &nCurLine);

            nCurShowPos=nCurCharPos;
				    lpCurChar++;

            // set Next Display Area :  next item text
					  lpLineAddress=lpCurChar;
					  nShowLen=0;
				    nAddLen++;
            nFMTItem++;
						if (nFMTItem>=FORMATITEMMAXNUM)
							nFMTItem=FORMATITEMMAXNUM-1;
            continue;
          }
      }
			if (dwStyle&ES_MULTILINE)
			{ // the edit control is multiple line
				if (*lpCurChar==0x0d&&*(lpCurChar+1)==0x0a)
				{  // this code is enter code , must display this line and to next line
					// display this line start position
					x=nCurShowPos-lpEditItem->nDisplayx;
					y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
					if ((y>=rcClientRect.bottom)&&dwFlag!=PE_SHOWALL)
						break;

					// display this line
					if (y>=0&&y<nWindowHeight)
					{
//						RETAILMSG(1,("TextOut8 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
						TextOut(hdc,x,y,lpLineAddress,nShowLen);
					}

//          ClearLine(hWnd,hdc,lpEditItem,nCurCharPos-lpEditItem->nDisplayx,nCurLine);
          ClearLine(hWnd,hdc,lpEditItem,(int)(nCurCharPos-lpEditItem->nDisplayx),y);
					// display the separate line
          if (dwStyle&ES_LINESEPARATE)
            DrawLineSeparate(hWnd,hdc,lpEditItem,(int)(nCurLine-lpEditItem->nDisplayLine));

					// to next char
					lpCurChar+=2;
					nAddLen+=2;
					lpLineAddress=lpCurChar;

					// to next line
					nShowLen=0;
					nCurLine++;
					nCurShowPos=GetxStartOfLine(hWnd,lpEditItem,lpCurChar);
					nCurCharPos=nCurShowPos;
					// to next circle
					continue;
				}
				if (dwStyle&ES_AUTONEWLINE)
				{
					if (nCurCharPos+nCurCharWidth>nWindowWidth)
					{
						// display this line start position
						x=nCurShowPos-lpEditItem->nDisplayx;
						y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;

						// display this line
						if (y>=0&&y<nWindowHeight)
						{
//							RETAILMSG(1,("TextOut9 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
							TextOut(hdc,x,y,lpLineAddress,nShowLen);
						}

//            ClearLine(hWnd,hdc,lpEditItem,nCurCharPos,nCurLine);
              ClearLine(hWnd,hdc,lpEditItem,(int)(nCurCharPos-lpEditItem->nDisplayx),y);

						if (dwStyle&ES_LINESEPARATE)
							DrawLineSeparate(hWnd,hdc,lpEditItem,(int)(nCurLine-lpEditItem->nDisplayLine));
						// to next char
						lpLineAddress=lpCurChar;
						// to next line
						nShowLen=0;
						nCurLine++;
						nCurShowPos=GetxStartOfLine(hWnd,lpEditItem,lpCurChar);
						nCurCharPos=nCurShowPos;
					}
				}
			}
			else
			{ 
// !!! Add By Jami chen in 2003.10.23
				// is single line
// !!! modified By Jami chen in 2004.05.11
//  错误原因：nCurShowPos -- 开始显示字符的位置
//			  nCurCharPos -- 没有加入当前字符（前一个字符）的位置，即当前字符开始显示的位置
//            在这里只需要判断当前字符是否已经在屏幕外就可以了，而当前字符的屏幕位置是nCurCharPos-lpEditItem->nDisplayx
//			  而不是nCurShowPos-lpEditItem->nDisplayx + nCurCharPos。
//			  另如果使用nCurCharPos-lpEditItem->nDisplayx + nCurCharWidth，则最后一个字符（半个字符）不能显示，而且必须调用
//			  ClearTextEnd来清除行尾的字符痕迹，不加nCurCharWidth就不必调用，因为已经全部清除了

//				if ((nCurShowPos-lpEditItem->nDisplayx + nCurCharPos) > nWindowWidth)
//				if ((nCurCharPos-lpEditItem->nDisplayx + nCurCharWidth) > nWindowWidth)
				if ((nCurCharPos-lpEditItem->nDisplayx) > nWindowWidth)
// !!! modified end
				{
					x=nCurShowPos-lpEditItem->nDisplayx;
					y=(nCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
					if (y>=0&&y<nWindowHeight)
					{
	//					RETAILMSG(1,("TextOut1 (%d,%d) %s -- %d\r\n",x,y,lpLineAddress,nShowLen));						
						TextOut(hdc,x,y,lpLineAddress,nShowLen);
					}
					if (dwStyle&ES_LINESEPARATE)
						DrawLineSeparate(hWnd,hdc,lpEditItem,(short)(nCurLine-lpEditItem->nDisplayLine));
					// exit circle
					break;
				}
			}
// !!! Add End By Jami chen in 2003.10.23

			// to next char
	        nCurCharPos+=nCurCharWidth;
//			RETAILMSG(1,("nCurCharPos = %d ,nCurCharWidth = %d\r\n",nCurCharPos,nCurCharWidth));
 	       lpCurChar+=nCurCharLen;
			nShowLen+=nCurCharLen;
			nAddLen+=nCurCharLen;

		}
/*
        if( y + nTextHeight < rcClientRect.bottom )
        {
            rcClientRect.top = y+nTextHeight;
            FillRect( hdc, &rcClientRect, GetStockObject( WHITE_BRUSH ) );
        }
*/
		// show caret
		ShowPDACaret(hWnd);
// !!! add By Jami chen for have a call back style
		if (lpEditItem->dwStyle&ES_CALLBACK)
		{
			ShowCallBackFlag(hWnd,hdc,lpEditItem);
		}
// !!! add End
		return;
}
/**************************************************
声明： void ClearInvert(HWND hWnd,LPEDITITEM lpEditItem, BOOL bFlag)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN bFlag -- 重绘标志
返回值：无
功能描述：清除选择区域。
引用: 
************************************************/
 void ClearInvert(HWND hWnd,LPEDITITEM lpEditItem, BOOL bFlag)
{
    LPTSTR lpShowAddress;
//	LPEDITITEM lpEditItem;
	int nShowPos,nShowLine;
	int sizeLen;
	HDC hdc;
	DWORD dwStyle;

		 // get struct EDITITEM data
//	   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//     if (lpEditItem==NULL) return;
	   if (bFlag==TRUE)
	   {	// Initial redraw data
			// get window style
//			dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      dwStyle=lpEditItem->dwStyle;
			// get redraw start address
			lpShowAddress=lpEditItem->lpInvertStart;
			// get redraw text length
			sizeLen=lpEditItem->lpInvertEnd-lpEditItem->lpInvertStart;
			// get redraw start cordinate position
			GetCoordinate(hWnd,lpEditItem,lpShowAddress,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
	   }
		 // clear the invert area
	   lpEditItem->lpInvertInitial=NULL;
	   lpEditItem->lpInvertStart=NULL;
	   lpEditItem->lpInvertEnd=NULL;
	   lpEditItem->lpOldInvertStart=NULL;
	   lpEditItem->lpOldInvertEnd=NULL;
 	   if (bFlag==TRUE&&sizeLen!=0)
	   {// redraw the change text
		    // get hdc
	      hdc=GetDC(hWnd);
			  // redraw the change text
			  DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,lpShowAddress,
			                          nShowPos,nShowLine,sizeLen,PE_SHOWSPECIALLEN);
			  //release hdc
			  ReleaseDC(hWnd,hdc);
	   }
}
/*********************************************************************/
// get coordinate position of the appoiont address

// input :  hWnd --- handle of the window
//		    	lpPosition  --- the appoint address 
//			    lpxPos ---- will return the x-coordinate
//          lpnLine ---- will return the line
//			    bIsMultiLine --- is or not multiple line 
/*********************************************************************/
/**************************************************
声明： void GetCoordinate(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpPosition,int * lpxPos,int * lpnLine,BOOL bIsMultiLine)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpPosition -- 指定地址
	OUT lpxPos -- 存放指定地址的X坐标
	OUT lpnLine -- 存放指定地址的行索引
	IN bIsMultiLine -- 是否是多行编辑区 
返回值：无
功能描述：得到指定地址的坐标值
引用: 
************************************************/
 void GetCoordinate(HWND hWnd,LPEDITITEM lpEditItem,LPTSTR lpPosition,int * lpxPos,int * lpnLine,BOOL bIsMultiLine)
{
//	LPEDITITEM lpEditItem;
  int lpCharWidthBuffer[128];
  int ChineseCharWidth;
  int nAddWidth=0;
  int nCurCharWidth,nCurLine,nWindowWidth;
//  LPSTR lpCurChar;
  char *lpCurChar;
  DWORD dwStyle;
  FORMATANALYSIS FmtAnalysis;
  int nFMTItem=0;
  SIZE Size;

		// get struct EDITITEM data
//		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL)
//    {
//				if (lpxPos)
//					*lpxPos=0;
//				if (lpnLine)
//					*lpnLine=0;
//        return;
//    }
    // Get Window Width
     nWindowWidth=GetWindowWidth(hWnd,lpEditItem);
		// get Char Width for ASCII Code
    GetPDACharWidth(hWnd,lpCharWidthBuffer,&ChineseCharWidth);
		// Get Edit Control Style
//		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    dwStyle=lpEditItem->dwStyle;

		// Get Start Address
		lpCurChar=lpEditItem->lpPDAControlBuffer;
		nCurLine=0;

		nAddWidth=GetxStartOfLine(hWnd,lpEditItem,lpCurChar);

    if (dwStyle&ES_FORMAT)
    {  //this  edit is format edit
      FormatEdit(hWnd,lpEditItem,&FmtAnalysis);
      nFMTItem=0;
      Size=GetSize(hWnd,FmtAnalysis.lpTitle,FmtAnalysis.cTitleLen);
      nCurCharWidth=(int)Size.cx;
			nAddWidth+=nCurCharWidth;
    }
		while(TRUE)
		{
//			if (lpCurChar==lpPosition)
			if (lpCurChar >= lpPosition)
			{// is to the appoint address
				// exist circle
				break;
			}
			// The buffer is end?
			if (*lpCurChar==0x00)
			{
				break;
			}
			// Get Current Character Width
//			if ((*lpCurChar)<0)
			if (*lpCurChar<0 || *lpCurChar>0x7F)
			{  // current Character is a chinese
				nCurCharWidth=ChineseCharWidth;
			}
			else
			{  // current character is english or sambol
          if ((dwStyle&ES_FORMAT)&&((*lpCurChar)==FMTSEPARATE))
          {  // 是格式化条目分隔符
            Size=GetSize(hWnd,(FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                    (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen);
            nCurCharWidth=(int)Size.cx;
            nFMTItem++; // 下一个条目
						if (nFMTItem>=FORMATITEMMAXNUM)
							nFMTItem=FORMATITEMMAXNUM-1;
          }
          else
          {
			      nCurCharWidth=lpCharWidthBuffer[*lpCurChar];
          }
			}
			// Is or not Multiple Line edit Control
			if (bIsMultiLine)
			{   // is or not enter code ?
				if (*lpCurChar==0x0d&&*(lpCurChar+1)==0x0a)
				{
					// To Next Line
					nCurLine++;
//					nAddWidth=0;
					nAddWidth=GetxStartOfLine(hWnd,lpEditItem,lpCurChar);
					lpCurChar+=2;
					continue;
				}
			  if (dwStyle&ES_AUTONEWLINE)
			  {  // 可以自动换行
				  if (nAddWidth+nCurCharWidth>nWindowWidth)
				    {
								  // To Next Line
								  nCurLine++;
  //								nAddWidth=0;
								  nAddWidth=GetxStartOfLine(hWnd,lpEditItem,lpCurChar);
  //					  continue;
				    }
			  }
			}
			// Get Current Character Width
//			if (*lpCurChar<0)
			if (*lpCurChar<0 || *lpCurChar>0x80)
			{ // to next character
				lpCurChar+=2;
			}
			else
			{ //to next character
				lpCurChar++;
			}
			// add current character width
			nAddWidth+=nCurCharWidth;

		}
		// return Current character Position
		if (lpxPos)
			*lpxPos=nAddWidth;
		if (lpnLine)
			*lpnLine=nCurLine;
		return ;
}
/**************************************************
声明：BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle)
参数：
	IN HiByte -- 高字节
	IN LoByte -- 低字节
	IN dwStyle -- 编辑区风格
返回值：需要字符，返回TRUE，否则返回FALSE。
功能描述：选择字符是否符合当前类型的编辑区。
引用: 
************************************************/
 BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle)
{
	if (dwStyle&ES_NUMBER)
	{  // the edit control is number ,then Only Receive 0--9
		if (HiByte!=0) return FALSE;
		if (LoByte>'9'||LoByte<'0') return FALSE;
	}
	if ((dwStyle&ES_MULTILINE)==0)
	{ // if this is single line then not receive the entercode
		if (LoByte==ENTERCODE&&HiByte==0) return FALSE;
	}
	if (HiByte==0&&LoByte<0x20)
	{// not receive the character of not display
		if (LoByte!=ENTERCODE) return FALSE;
	}
	return TRUE;
}
/**************************************************
声明： BOOL InsertChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR HiByte,CHAR LoByte)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN HiByte -- 高字节
	IN LoByte -- 低字节
返回值：成功返回TRUE，否则返回FALSE
功能描述：插入字符。
引用: 
************************************************/
 BOOL InsertChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR HiByte,CHAR LoByte)
{
  int nTextLen,nRemainLen;
  int nMoveLen;
//	LPEDITITEM lpEditItem;
	DWORD dwStyle;
  BOOL bReturnCode=TRUE;
  FORMATANALYSIS FmtAnalysis;


	// get struct EDITITEM data
//	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//  if (lpEditItem==NULL) return FALSE;
//  dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    dwStyle=lpEditItem->dwStyle;


  if (dwStyle&ES_PASSWORD)
  { // the style is password
		  GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
//		GetWindowText(hWnd,lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen+1); // +1 is read end code 'null'
  }
  // Get the legnth of the text 
	nTextLen=strlen(lpEditItem->lpPDAEditBuffer);
	// get the length from the caret position to the end (include the end code)
	nMoveLen=strlen(lpEditItem->lpCaretPosition)+1;
	// get the remain length of the window
  if (dwStyle&ES_FORMAT)
  { // the style is format style
    FormatEdit(hWnd,lpEditItem,&FmtAnalysis);
    nRemainLen=CanInsertToFmtItem(&FmtAnalysis,HiByte,LoByte);
  }
  else
  {
	  nRemainLen=lpEditItem->cbEditLimitLen-nTextLen;
  }
	if (HiByte==0)
	{ // this is a english or symbol
		if (nRemainLen<1)
		{ // the remain length is not enough to place the insert char
//			MessageBeep(0xFFFFFFFFL);
			bReturnCode=FALSE;
			SendNotificationMsg( hWnd,EN_MAXTEXT);
		}
    else
    {
		  //place the insert char
		  memmove(lpEditItem->lpCaretPosition+1,lpEditItem->lpCaretPosition,nMoveLen);
		  *(lpEditItem->lpCaretPosition)=LoByte;
		  // move the caret position
		  lpEditItem->lpCaretPosition++;
//      nRemainLen--;
			lpEditItem->fModified=TRUE;
			lpEditItem->cbEditLen=strlen(lpEditItem->lpPDAEditBuffer);
			SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItem->lpPDAEditBuffer,(int)(lpEditItem->cbEditLen+1));
			lpEditItem->bNeedCalcTotalLine = TRUE;
// !!! Add By Jami chen in 2005.04.05
			SetWindowRect(hWnd,lpEditItem,TRUE);
// !!! Add End
//			SendNotificationMsg(hWnd,EN_CHANGE);
    }

	}
	else
	{ // the insert char is chinese
		if (nRemainLen<2)
		{ // the remain length is not enough to place the insert char
//			MessageBeep(0xFFFFFFFF);
			bReturnCode=FALSE;
			SendNotificationMsg( hWnd,EN_MAXTEXT);
		}
		else
		{
			  // place the insert char
			  memmove(lpEditItem->lpCaretPosition+2,lpEditItem->lpCaretPosition,nMoveLen);
			  *(lpEditItem->lpCaretPosition)=HiByte;
			  *(lpEditItem->lpCaretPosition+1)=LoByte;
			  //move caret position
			  lpEditItem->lpCaretPosition+=2;
		//    nRemainLen-=2;
			  lpEditItem->fModified=TRUE;
			  lpEditItem->cbEditLen=strlen(lpEditItem->lpPDAEditBuffer);
			  SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItem->lpPDAEditBuffer,(int)(lpEditItem->cbEditLen+1));
			  lpEditItem->bNeedCalcTotalLine = TRUE;
// !!! Add By Jami chen in 2005.04.05
			  SetWindowRect(hWnd,lpEditItem,TRUE);
// !!! Add End
//			  SendNotificationMsg(hWnd,EN_CHANGE);
		}
	}

//  SetWindowText(hWnd,lpEditItem->lpPDAEditBuffer);
//	lpEditItem->cbEditLen=strlen(lpEditItem->lpPDAEditBuffer);
//	SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItem->lpPDAEditBuffer,(int)(lpEditItem->cbEditLen+1));
  if (dwStyle&ES_PASSWORD)
  {
		// read Window text
    FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
  }
  if (nRemainLen==0)
  {
    if (dwStyle&ES_FORMAT)
    { //Is format?
      while(*lpEditItem->lpCaretPosition!=0)
      { // jump the next format item
        if (*lpEditItem->lpCaretPosition==FMTSEPARATE)
        {
          lpEditItem->lpCaretPosition++;
//          lpPDAEditState->lpCaretPosition=lpCurChar;
	          return bReturnCode;
        }
        lpEditItem->lpCaretPosition++;
      }
    }
    if (dwStyle&ES_MULTIITEMEDIT)
      SetCaretToNextItem(hWnd,lpEditItem);
//    JumpToNextItem(hWnd,lpEditItem);
  }
	// insert success
	return bReturnCode;
}

/**************************************************
声明：void ClearLine(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN lpEdititem -- 编辑条目结构指针
	IN xPos -- x坐标
	IN yPos -- y坐标
返回值：无
功能描述：清除行指定坐标以后的区域。
引用: 
************************************************/
void ClearLine(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos)
{
   RECT ClearRect;
//   HDC hdc;
   int nTextHeight;
	 int nWindowWidth;
	 HBRUSH hBrush;

	 //  get line height
	 nTextHeight=GetTextHeight(hWnd);
	 // get window width
	 nWindowWidth=(int)(lpEditItem->rect.right-lpEditItem->rect.left);

	// calculate clear rect
	ClearRect.left=xPos;
	ClearRect.top=yPos;
	ClearRect.right=nWindowWidth;
	ClearRect.bottom =ClearRect.top+nTextHeight;
	// clear ClearRect
//	hdc=GetDC(hWnd);
//	FillRect(hdc,&ClearRect,GetStockObject(WHITE_BRUSH));
	hBrush = GetBkBrush(hWnd,lpEditItem);
	FillRect(hdc,&ClearRect,hBrush);
	DeleteObject(hBrush);
//	ReleaseDC(hWnd,hdc);
}
/**************************************************
声明： void ClearTextEnd(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN lpEdititem -- 编辑条目结构指针
	IN xPos -- x坐标
	IN yPos -- y坐标
返回值：无
功能描述：清除指定坐标以后的所有区域。
引用: 
************************************************/
 void ClearTextEnd(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos)
{
   RECT ClearRect;
//   HDC hdc;
   int nTextHeight;
   HBRUSH hBrush;


//	RETAILMSG(1,(TEXT("Enter ClearTextEnd ...\r\n")));
	nTextHeight=GetTextHeight(hWnd);

//	hdc=GetDC(hWnd);
	// Get Client Rect
//	GetClientRect(hWnd,&ClientRect);
	// Clear appoint line
	ClearRect.left=xPos;
	ClearRect.top=yPos;
	ClearRect.right=lpEditItem->rect.right;

	ClearRect.bottom =ClearRect.top+nTextHeight;
//	FillRect(hdc,&ClearRect,GetStockObject(WHITE_BRUSH));
	hBrush = GetBkBrush(hWnd,lpEditItem);
	FillRect(hdc,&ClearRect,hBrush);
	// clear next line to the window end

	ClearRect.left=0;
	ClearRect.top=ClearRect.bottom;
	ClearRect.right=lpEditItem->rect.right;
//	ClearRect.bottom =lpEditItem->rect.bottom;
    ClearRect.bottom=lpEditItem->rect.bottom+lpEditItem->iStartPos;

    if (ClearRect.bottom>ClearRect.top)
	{
//	   FillRect(hdc,&ClearRect,GetStockObject(WHITE_BRUSH));
	   FillRect(hdc,&ClearRect,hBrush);
	}

	DeleteObject(hBrush);

#ifdef LINE_BANK
	// 在空白的地方绘制横线
	{
		DWORD dwStyle;

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		if (dwStyle&ES_LINESEPARATE)
		{
			int nCurLine,nBottomLine;
			int nWindowHeight;

				nWindowHeight=GetWindowHeight(hWnd);

				nCurLine = (yPos - lpEditItem->iStartPos ) / nTextHeight + lpEditItem->nDisplayLine;
				nBottomLine = nWindowHeight / nTextHeight + lpEditItem->nDisplayLine;

				for (;nCurLine < nBottomLine; nCurLine++)
				{
					DrawLineSeparate(hWnd,hdc,lpEditItem,(int)(nCurLine-lpEditItem->nDisplayLine));
				}
		}
	}
#endif
//	ReleaseDC(hWnd,hdc);
//	RETAILMSG(1,(TEXT("Leave ClearTextEnd ...\r\n")));
}

/**************************************************
声明： void ClearTextHead(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN lpEdititem -- 编辑条目结构指针
	IN xPos -- x坐标
	IN yPos -- y坐标
返回值：无
功能描述：清除指定点以前当前行的区域。
引用: 
************************************************/
 void ClearTextHead(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int xPos,int yPos)
{
   RECT ClearRect;
//   HDC hdc;
   int nTextHeight;
   HBRUSH hBrush;

   nTextHeight=GetTextHeight(hWnd);

	// Clear appoint line
	ClearRect.left=lpEditItem->rect.left;
	ClearRect.top=yPos;
	ClearRect.right= xPos;

	ClearRect.bottom =ClearRect.top+nTextHeight;
//	FillRect(hdc,&ClearRect,GetStockObject(WHITE_BRUSH));
	hBrush = GetBkBrush(hWnd,lpEditItem);
	FillRect(hdc,&ClearRect,hBrush);
	DeleteObject(hBrush);
	// clear next line to the window end
}
/**************************************************
声明： void DisplayInvertChange(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：无
功能描述：显示选择区域改变的文本。
引用: 
************************************************/
 void DisplayInvertChange(HWND hWnd,LPEDITITEM lpEditItem)
{
    LPTSTR lpShowAddress;
//	LPEDITITEM lpEditItem;
	int nShowPos,nShowLine;
	int sizeLen;
	HDC hdc;
	DWORD dwStyle;

		// get the struct EDITITEM data
//	   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//     if (lpEditItem==NULL) return;
	   // get window style
//	   dwStyle=GetWindowLong(hWnd,GWL_STYLE);
     dwStyle=lpEditItem->dwStyle;
	   if (lpEditItem->lpOldInvertStart==NULL)
	   { // from no invert to have invert ,redraw all invert area
				lpShowAddress=lpEditItem->lpInvertStart;
				sizeLen=lpEditItem->lpInvertEnd-lpEditItem->lpInvertStart;
	   }
	   else if (lpEditItem->lpInvertStart==NULL)
	   {// from exist invert to not exist invert ,redraw old invert area
				lpShowAddress=lpEditItem->lpOldInvertStart;
				sizeLen=lpEditItem->lpOldInvertEnd-lpEditItem->lpOldInvertStart;
	   }
	   else if (lpEditItem->lpInvertStart==lpEditItem->lpOldInvertStart)
	   { // the invert start address not change , so redraw the end start changed
			if (lpEditItem->lpInvertEnd>lpEditItem->lpOldInvertEnd)
			{ // Invert area to bigger
				lpShowAddress=lpEditItem->lpOldInvertEnd;
				sizeLen=lpEditItem->lpInvertEnd-lpEditItem->lpOldInvertEnd;
			}
			else
			{ // Invert area to smaller
				lpShowAddress=lpEditItem->lpInvertEnd;
				sizeLen=lpEditItem->lpOldInvertEnd-lpEditItem->lpInvertEnd;
			}
	   }
	   else if (lpEditItem->lpInvertEnd==lpEditItem->lpOldInvertEnd)
	   { // the end address is not changed ,redraw the area of the start address changed
			if (lpEditItem->lpInvertStart>lpEditItem->lpOldInvertStart)
			{ // the area to smaller
				lpShowAddress=lpEditItem->lpOldInvertStart;
				sizeLen=lpEditItem->lpInvertStart-lpEditItem->lpOldInvertStart;
			}
			else
			{ // the invert area to bigger
				lpShowAddress=lpEditItem->lpInvertStart;
				sizeLen=lpEditItem->lpOldInvertStart-lpEditItem->lpInvertStart;
			}
	   }
	   else
	   { // the start address and the end address all be chenged
			if (lpEditItem->lpInvertStart>lpEditItem->lpOldInvertStart)
			{ // the start address to behind
				lpShowAddress=lpEditItem->lpOldInvertStart;
			}
			else
			{ // the start address to front
				lpShowAddress=lpEditItem->lpInvertStart;
			}
			if (lpEditItem->lpInvertEnd>lpEditItem->lpOldInvertEnd)
			{ // the end address to behind
				sizeLen=lpEditItem->lpInvertEnd-lpShowAddress;
			}
			else
			{ // the end address to front
				sizeLen=lpEditItem->lpOldInvertEnd-lpShowAddress;
			}
	   }
	    if (sizeLen==0) return;
			// get coordinate of the  start address  to be redrawed
			GetCoordinate(hWnd,lpEditItem,lpShowAddress,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
			// redraw the wnddow
			hdc=GetDC(hWnd);
	    DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,lpShowAddress,
	                      nShowPos,nShowLine,sizeLen,PE_SHOWSPECIALLEN);
	    ReleaseDC(hWnd,hdc);
}
/**************************************************
声明：BOOL AdjustCaretInEditItem(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：光标位置有调整返回TRUE，否则返回FALSE
功能描述：调整光标位置到可见区域。
引用: 
************************************************/
BOOL AdjustCaretInEditItem(HWND hWnd,LPEDITITEM lpEditItem)
{
//	RECT ClientRect;
//	LPEDITITEM lpEditItem;
	BOOL bModified=FALSE;
	int nClientLine,nTotalLine,nWindowWidth;
//	int nTextHeight;

//  HWND hParentWnd;
	DWORD dwStyle;
//  LPARAM lParam;

		// get Text hieght
//		nTextHeight=GetTextHeight(hWnd);

		// reset thw window rect when the lines is changed
    SetWindowRect(hWnd,lpEditItem,TRUE);
    // get the struct EDITITEM data
//	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL) return FALSE;
//    dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    dwStyle=lpEditItem->dwStyle;

    if (!(dwStyle&ES_MULTIITEMEDIT))
    {
		  // get window width
		  nWindowWidth=GetWindowWidth(hWnd,lpEditItem);
		  if (lpEditItem->nCaretx<0)
		  {  // out side of the left
			  while(lpEditItem->nCaretx<0)
			  {   // adjust the display coordinate and to the caret
				  //coordinate into the client rect
				  lpEditItem->nCaretx+=HSCROLLWIDTH;
				  lpEditItem->nDisplayx-=HSCROLLWIDTH;
			  }
			  bModified=TRUE;
		  }
		  if (lpEditItem->nCaretx>nWindowWidth)
		  { // out side of the right
			  while(lpEditItem->nCaretx>nWindowWidth)
			  { // adjust the display coordinate
				  lpEditItem->nCaretx-=HSCROLLWIDTH;
				  lpEditItem->nDisplayx+=HSCROLLWIDTH;
			  }
			  bModified=TRUE;
		  }
      if (bModified)
          SetEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH));
    }
    if (!(dwStyle&ES_AUTOEXTEND)&&dwStyle&ES_MULTILINE)
    { // do process line
		    if (lpEditItem->nCaretLine<0)
		    {  // Caret position is out in client area to up
			    if (lpEditItem->nDisplayLine<=0)
			    {  // now the edit is the top
				    // set caret position the top
				    lpEditItem->nCaretLine=0;
			    }
			    else
			    {  // may move up
				    while(lpEditItem->nCaretLine<0&&lpEditItem->nDisplayLine>0)
				    { // Is move to top or the caret psition is in the client area
					    // up one line
					    lpEditItem->nCaretLine++;
					    lpEditItem->nDisplayLine--;
				    }
				    // the caret position is be changed
				    bModified=TRUE;
			    }
		    }
		    // get client total line
//		    nClientLine=ClientRect.bottom/nTextHeight-1;
		    nClientLine=GetPageLine(hWnd)-1;
		    // Get edit text total lie
		    nTotalLine=GetTotalLine(hWnd,lpEditItem);

// !!! Add By Jami chen in 2003.08.30
		// 可能会有当Edit中所有的行都可以在屏幕上显示，则将显示从第一行开始。
			if (nTotalLine <= nClientLine +1 )
			{
				if (lpEditItem->nDisplayLine != 0)
				{
					lpEditItem->nCaretLine += lpEditItem->nDisplayLine;
					lpEditItem->nDisplayLine = 0;
					bModified=TRUE;
				}
			}
// !!! Add End By Jami chen in 2003.08.30
		    if (lpEditItem->nCaretLine>nClientLine)
		    { // out side of the buttom
			    // adjust the display coordinate
			    if(lpEditItem->nCaretLine+lpEditItem->nDisplayLine>nTotalLine)
			    {
				    lpEditItem->nCaretLine=nTotalLine-(lpEditItem->nDisplayLine+1);
			    }
			    else
			    {
				    while (lpEditItem->nCaretLine>nClientLine)
				    {
						    lpEditItem->nCaretLine--;
						    lpEditItem->nDisplayLine++;
				    }
				    // the caret position is be changed
				    bModified=TRUE;
			    }
		    }
        if (bModified)
          SetEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
    }

/*    if (dwStyle&ES_MULTIITEMEDIT)
    {
            hParentWnd=GetParent(hWnd);
            lParam=MAKELPARAM(lpEditItem->nDisplayx,lpEditItem->nCaretLine);
            SendMessage(hParentWnd,MIEM_DISPLAYCHANGED,(WPARAM)hWnd,lParam);
            bModified=FALSE;
     }*/
		return bModified;
}
/**************************************************
声明： int  BackSpaceChar(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：0 -- 已经不能删除， 1 -- 正常字符删除， 2 -- 删除回车符
功能描述：回删一个字符。
引用: 
************************************************/
 int  BackSpaceChar(HWND hWnd,LPEDITITEM lpEditItem)
{

		// Move Caret to preceding char
		if (MoveCaretToPreChar(hWnd,lpEditItem))
		{
			// Delete Caret Char
			return (DeleteCaretChar(hWnd,lpEditItem));
		}
		return FALSE;
}
/**************************************************
声明： BOOL MoveCaretToPreChar(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：成功返回TRUE，否则返回FALSE。
功能描述：移动光标到前一个字符。
引用: 
************************************************/
 BOOL MoveCaretToPreChar(HWND hWnd,LPEDITITEM lpEditItem)
{
//	LPEDITITEM lpEditItem;

	// get struct EDITITEM data
//	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//  if (lpEditItem==NULL) return FALSE;

  if (lpEditItem->lpCaretPosition==lpEditItem->lpPDAEditBuffer)
	{ // Already to the top ,can not move
		return FALSE;
	}
	if (*(lpEditItem->lpCaretPosition-1)==0x0a)
	{  // the pre char is enter code
		lpEditItem->lpCaretPosition-=2;
		return TRUE;
	}
//	if (*(lpEditItem->lpCaretPosition-1)<0)
	if (*(lpEditItem->lpCaretPosition-1)<0  || *(lpEditItem->lpCaretPosition-1) > 0x7F)
	{  // the pre char is chinese
		lpEditItem->lpCaretPosition-=2;
		return TRUE;
	}
	// The Pre Char is english or symbol
	lpEditItem->lpCaretPosition--;
	return TRUE;
}
/**************************************************
声明： int DeleteCaretChar(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：0 -- 已经不能删除， 1 -- 正常字符删除， 2 -- 删除回车符
功能描述：删除当前一个字符。
引用: 
************************************************/
 int DeleteCaretChar(HWND hWnd,LPEDITITEM lpEditItem)
{
	LPTSTR lpMoveSourceAddress,lpMoveObjectAddress;
	int nMoveLen,returncode;
//	LPEDITITEM lpEditItem;
	DWORD dwStyle;


	// get struct EDITITEM data
//	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//  if (lpEditItem==NULL) return 0;
//  dwStyle=GetWindowLong(hWnd,GWL_STYLE);

  dwStyle=lpEditItem->dwStyle;
	if (*lpEditItem->lpCaretPosition==0)
	{ // Already to the End ,can not be Deletee
		return 0;
	}
	if ((dwStyle&ES_FORMAT)&&(*lpEditItem->lpCaretPosition==FMTSEPARATE))
  {
    return 0;
  }

  if (dwStyle&ES_PASSWORD)
  {
		// read Window text
		  GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
//		GetWindowText(hWnd,lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen+1); // +1 is read end code 'null'
  }
  if (*lpEditItem->lpCaretPosition==0x0d)
	{  // the pre char is enter code
		if (*(lpEditItem->lpCaretPosition + 1)==0x0a)
		{
			lpMoveSourceAddress=lpEditItem->lpCaretPosition+2;
			returncode=2;
		}
		else
		{
			lpMoveSourceAddress=lpEditItem->lpCaretPosition+1;
			returncode=1;
		}
	}
//	else if (*lpEditItem->lpCaretPosition<0)
	else if (*lpEditItem->lpCaretPosition<0 || *lpEditItem->lpCaretPosition > 0x7F)
	{  // the pre char is chinese 
		lpMoveSourceAddress=lpEditItem->lpCaretPosition+2;
		returncode=1;
	}
	else
	{// The Pre Char is english or symbol
		lpMoveSourceAddress=lpEditItem->lpCaretPosition+1;
		returncode=1;
	}
	// Move memoey to caret position
	lpMoveObjectAddress=lpEditItem->lpCaretPosition;
	// calculate Move Len 
	nMoveLen=strlen(lpMoveSourceAddress)+1;
	// move memory
	memmove(lpMoveObjectAddress,lpMoveSourceAddress,nMoveLen);

//  SetWindowText(hWnd,lpEditItem->lpPDAEditBuffer);
//	lpEditItem->cbEditLen=GetWindowTextLength(hWnd);
	lpEditItem->cbEditLen=strlen(lpEditItem->lpPDAEditBuffer);
	SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItem->lpPDAEditBuffer,(int)(lpEditItem->cbEditLen+1));
	lpEditItem->fModified=TRUE;
	lpEditItem->bNeedCalcTotalLine = TRUE;
	SendNotificationMsg(hWnd,EN_CHANGE);
  if (dwStyle&ES_PASSWORD)
  {
		// Set the password char
    FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
  }
	return returncode;
}
/**************************************************
声明：int DeleteChar(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：无
功能描述：删除字符，如果有选择区域，则删除选择区域字符，否则删除当前字符。
引用: 
************************************************/
 int DeleteChar(HWND hWnd,LPEDITITEM lpEditItem)
{
//	LPEDITITEM lpEditItem;
	HDC hdc;
	LPTSTR lpShowAddress;
	int nShowPos,nShowLine;
	DWORD dwShowFlag,dwStyle;
	int iResult;


	// get struct EDITITEM data
//	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//  if (lpEditItem==NULL) return 0;

	// get window style
//	dwStyle=GetWindowLong(hWnd,GWL_STYLE);
	  dwStyle=lpEditItem->dwStyle;
	  if (dwStyle&ES_READONLY)
		  return 0;
	  if (dwStyle&WS_DISABLED)
		  return 0;
	if (lpEditItem->lpInvertStart!=lpEditItem->lpInvertEnd)
	{ // exist Invert area and delete Invert
		// delete Invert area
		DeleteInvert(hWnd,lpEditItem);
		// set redraw parameter
		lpShowAddress=lpEditItem->lpCaretPosition;
		GetCoordinate(hWnd,lpEditItem,lpShowAddress,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
		lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
		lpEditItem->nCaretLine=nShowLine-lpEditItem->nDisplayLine;
		// adjust caret coordinate
		if (AdjustCaretInEditItem(hWnd,lpEditItem))
		{
			lpShowAddress=NULL;
			nShowPos=0;
			nShowLine=0;
			dwShowFlag=PE_SHOWCLIENT;
		}
		else
		{
			dwShowFlag=PE_SHOWTEXTEND;
		}

	}
	else
	{  // not exist invert ,then delete caret position char
		
		iResult = DeleteCaretChar(hWnd,lpEditItem);
		if (iResult == 0)
			return 0;

		if (iResult == 2)
		{ // Delete Enter Code
			dwShowFlag=PE_SHOWTEXTEND;
		}
		else
		{ // delete other code
			dwShowFlag=PE_SHOWLINEEND;
		}
		// set redraw parameter
		if (dwStyle&0x0003)
		{
				GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
				lpEditItem->nCaretLine=nShowLine-lpEditItem->nDisplayLine;
				lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
		}
// !!! Add By Jami chen in 2003.08.30
		// 可能会有当Edit中所有的行都可以在屏幕上显示，则需要重新显示
		if (AdjustCaretInEditItem(hWnd,lpEditItem))
		{
			lpShowAddress=NULL;
			nShowPos=0;
			nShowLine=0;
			dwShowFlag=PE_SHOWCLIENT;
		}
		else
// !!! Add End By Jami chen in 2003.08.30
		{
			lpShowAddress=lpEditItem->lpCaretPosition;
			nShowPos=lpEditItem->nCaretx+lpEditItem->nDisplayx;
			nShowLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
		}
//		SetWindowRect(hWnd,lpEditItem,TRUE);
  }

	SendNotificationMsg(hWnd,EN_CHANGE);

	// redraw the window
	hdc=GetDC(hWnd);
  DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,lpShowAddress,nShowPos,nShowLine,0,dwShowFlag);
	ReleaseDC(hWnd,hdc);
	// reset caret
	SetPDACaret(hWnd);
	return 0;
}
/**************************************************
声明： BOOL DeleteInvert(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：成功返回TRUE，否则返回FALSE
功能描述：删除选择区域的内容。
引用: 
************************************************/
 BOOL DeleteInvert(HWND hWnd,LPEDITITEM lpEditItem)
{
	LPTSTR lpMoveSourceAddress,lpMoveObjectAddress;
	int nMoveLen;
//	LPEDITITEM lpEditItem;
	DWORD dwStyle;
  LPTSTR lpCurChar;


	// get struct EDITITEM data
//	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//  if (lpEditItem==NULL) return FALSE;
//  dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    dwStyle=lpEditItem->dwStyle;
	
	// not exist invert area
  if (lpEditItem->lpInvertEnd==lpEditItem->lpInvertStart) return FALSE;
  
  if (dwStyle&ES_PASSWORD)
  {
		// read Window text
//		GetWindowText(hWnd,lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen+1); // +1 is read end code 'null'
		  GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
  }
  
  lpMoveSourceAddress=lpEditItem->lpInvertEnd;
	// Move memoey to caret position
	lpMoveObjectAddress=lpEditItem->lpInvertStart;
  
  if (dwStyle&ES_FORMAT)
  {  // 是格式化条目
    lpCurChar=lpMoveObjectAddress;
    while(lpCurChar!=lpMoveSourceAddress)
    {
      if (*lpCurChar==FMTSEPARATE)
      {
        *lpMoveObjectAddress=FMTSEPARATE;
         lpMoveObjectAddress++; 
      }
      lpCurChar++;
    }
  }
	// calculate Move Len 
	nMoveLen=strlen(lpMoveSourceAddress)+1;
	// move memory
	memmove(lpMoveObjectAddress,lpMoveSourceAddress,nMoveLen);
	// Set Caret Position
	lpEditItem->lpCaretPosition=lpEditItem->lpInvertStart;

//  SetWindowText(hWnd,lpEditItem->lpPDAEditBuffer);
//  lpEditItem->cbEditLen=GetWindowTextLength(hWnd);
	lpEditItem->cbEditLen=strlen(lpEditItem->lpPDAEditBuffer);
	SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItem->lpPDAEditBuffer,(int)(lpEditItem->cbEditLen+1));
	lpEditItem->fModified=TRUE;
	lpEditItem->bNeedCalcTotalLine = TRUE;
// !!! Add By Jami chen in 2005.04.05
	SetWindowRect(hWnd,lpEditItem,TRUE);
// !!! Add End
	SendNotificationMsg(hWnd,EN_CHANGE);
  if (dwStyle&ES_PASSWORD)
  {
		// read Window text
    FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
  }
	// Initial Invert Area
	lpEditItem->lpInvertStart=NULL;
	lpEditItem->lpInvertEnd=NULL;
	lpEditItem->lpInvertInitial=NULL;
	lpEditItem->lpOldInvertStart=NULL;
	lpEditItem->lpOldInvertEnd=NULL;
	return TRUE;
}
/***************************************************************************/
// Get Window Text height
// input : hWnd -- handle of the window
// return Value:
//       return window text Height
/***************************************************************************/
/*int GetTextHeight(HWND hWnd)
{
	HDC hdc;
	TEXTMETRIC TextMetric;
  int nTextHeight;
  DWORD dwStyle;
		hdc=GetDC(hWnd);
		// get current text property
		GetTextMetrics(hdc,&TextMetric);
    ReleaseDC(hWnd,hdc);
    nTextHeight=(int)TextMetric.tmHeight;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		// add a dot line height , 1 dot
    if (dwStyle&ES_LINESEPARATE)
      nTextHeight+=1;
		return nTextHeight;
} */
/**************************************************
声明： void SetCaretToStart(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：无
功能描述：设置光标到开头。
引用: 
************************************************/
 void SetCaretToStart(HWND hWnd,LPEDITITEM lpEditItem)
{
//	LPEDITITEM lpEditItem;
	int nLine,xPos;
	DWORD dwStyle;

		// get struct EDITITEM data
//		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL) return;
		// Set Caret Position to Start Address of the lpPDAEditBuffer
		lpEditItem->lpCaretPosition=lpEditItem->lpPDAEditBuffer;
		// Get Window Style
//		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    dwStyle=lpEditItem->dwStyle;
		// Get Coordinate of the Caret
		GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&xPos,&nLine,dwStyle&ES_MULTILINE);
		lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine;
		lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx;
}
/**************************************************
声明： int GetWindowWidth(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：返回窗口的宽度
功能描述：得到窗口的宽度。
引用: 
************************************************/
 int GetWindowWidth(HWND hWnd,LPEDITITEM lpEditItem)
{
  RECT ClientRect;

		GetClientRect(hWnd,&ClientRect);
		if (lpEditItem->dwStyle&ES_CALLBACK)
		{
			// because must leave a caret space : 2 dot
			return (int)ClientRect.right-2-16;
		}
		else
		{
			// because must leave a caret space : 2 dot
			return (int)ClientRect.right-2;
		}
}
/**************************************************
声明： int GetWindowHeight(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回窗口的高度
功能描述：得到窗口的高度。
引用: 
************************************************/
 int GetWindowHeight(HWND hWnd)
{
  RECT ClientRect;
    GetClientRect(hWnd,&ClientRect);
		return (int)ClientRect.bottom;
}
/**************************************************
声明： BOOL GetPDACharWidth(HWND hWnd,LPINT lpASCIICodeWidthBuffer,LPINT lpChineseCodeWidth)
参数：
	IN hWnd -- 窗口句柄
	IN lpASCIICodeWidthBuffer -- 存放ASCII的所有字符宽度，需要128个INT的缓存
	IN lpChineseCodeWidth -- 存放汉字的宽度
返回值：成功返回TRUE，否则返回FALSE
功能描述：得到字符宽度。
引用: 
************************************************/
 BOOL GetPDACharWidth(HWND hWnd,LPINT lpASCIICodeWidthBuffer,LPINT lpChineseCodeWidth)
{
  HDC hdc;
    hdc=GetDC(hWnd);
		// get ASCII code width
		if (GetCharWidth(hdc,0,0x7f,lpASCIICodeWidthBuffer)==0)
        ;
//			ErrorDisplay("GetCharWidth Error");
		// get chinese character width
		if (GetCharWidth(hdc,0xb0a1,0xb0a1,lpChineseCodeWidth)==0)
        ;
//			ErrorDisplay("GetCharWidth Error");
    ReleaseDC(hWnd,hdc);
    return TRUE;
}
/********************************************************************************************/
// Set Window Rect , this function is effect when the style is ES_AUTOEXTEN
// if the window is not ES_AUTOEXTEN ,this function is only reset the TotalLine parameter
/********************************************************************************************/
/**************************************************
声明： void SetWindowRect(HWND hWnd,LPEDITITEM lpEditItem,BOOL bRedraw)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN bRedraw -- 是否需要重绘
返回值：无
功能描述：设置窗口大小，在有ES_AUTOEXTEN时才有效。
引用: 
************************************************/
 void SetWindowRect(HWND hWnd,LPEDITITEM lpEditItem,BOOL bRedraw)
{
      int nTextHeigth;
      int nTotalLine;
//      RECT ClientRect;
//    	int nTextHeight;
//    	LPEDITITEM lpEditItem;
//      HWND hParentWnd;
//      LPARAM lParam;
      int nChangedLine;
      DWORD dwStyle ;


        HidePDACaret(hWnd);
        dwStyle=lpEditItem->dwStyle;
				// get text totel lines
        nTotalLine=GetTotalLine(hWnd,lpEditItem);
        if (nTotalLine!=lpEditItem->nTotalLine)
        {
          if (dwStyle&ES_AUTOEXTEND)
          { // May auto extend window ,then extend window
              nChangedLine=nTotalLine-lpEditItem->nTotalLine;
              lpEditItem->nTotalLine=nTotalLine;
			  lpEditItem->bNeedCalcTotalLine = FALSE;
              if (dwStyle&ES_MULTIITEMEDIT)
              {
//	            nChangedLine=nTotalLine-lpEditItem->nTotalLine;
                ChangeSize(hWnd,lpEditItem,nChangedLine,bRedraw);
              }

//              lpEditItem->nTotalLine=nTotalLine;

              nTextHeigth=GetTextHeight(hWnd);
              lpEditItem->rect.bottom=lpEditItem->rect.top+lpEditItem->nTotalLine*nTextHeigth;

          }
          else
          {  // reset the TotalLine and Set VSCROLL
            lpEditItem->nTotalLine=nTotalLine;
			lpEditItem->bNeedCalcTotalLine = FALSE;
            SetPDAEditVScrollRange(hWnd,nTotalLine);
          }
        }
        ShowPDACaret(hWnd);
}
/**************************************************
声明： void SetPDAEditVScrollRange(HWND hWnd,int nTotalLine)
参数：
	IN hWnd -- 窗口句柄
	IN nTotalLine -- 总行数
返回值：无
功能描述：设置编辑区垂直滚动范围。
引用: 
************************************************/
 void SetPDAEditVScrollRange(HWND hWnd,int nTotalLine)
{
  int nMinPos,nMaxPos;
  DWORD dwStyle;
  int nPageLine;

  	  // get window style
	  dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      if (!(dwStyle&WS_VSCROLL)) return;
	  // get page lines
      nPageLine=GetPageLine(hWnd);
      if (nTotalLine<=nPageLine)
      { // not a page ,so not need scroll
			nMinPos=0;
			nMaxPos=nPageLine-1;//-nWindowLine+1;
			Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE);
			EnableScrollBar(hWnd,SB_VERT,ESB_DISABLE_BOTH);
      }
      else
      { // set vertical scroll range
			nMinPos=0;
			nMaxPos=nTotalLine-1;//-nWindowLine+1;
//			nMaxPos=nTotalLine-nPageLine+1;
			Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE);
			EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH);
      }
}
/**************************************************
声明：void SetPDAEditScrollPage(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：设置编辑区滚动条一页的大小。
引用: 
************************************************/
void SetPDAEditScrollPage(HWND hWnd)
{
   int nPageLine;
   //int nTextHeight;
   //RECT rcClientRect;
   SCROLLINFO ScrollInfo;
   DWORD dwStyle;

			// get window style 
      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
			// get page Lines
      nPageLine=GetPageLine(hWnd);

      ScrollInfo.cbSize=sizeof(SCROLLINFO);
      ScrollInfo.fMask=SIF_PAGE|SIF_DISABLENOSCROLL;
      // Set Vertical Scroll Page
      if (dwStyle&WS_VSCROLL)
      { // set vertical scroll page
        ScrollInfo.nPage =nPageLine;
        SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);
      }

      // Set horizonta Scroll Page
      if (dwStyle&WS_HSCROLL)
      { // set horizon scroll page
          ScrollInfo.nPage =1;
          SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);
      }
}
/**************************************************
声明：void SetEditVScrollPos(HWND hWnd,int nNewScrollPos)
参数：
	IN hWnd -- 窗口句柄
	IN nNewScrollPos -- 当前新的位置
返回值：无
功能描述：设置垂直滚动条新的当前位置。
引用: 
************************************************/
void SetEditVScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
      if (!(dwStyle&WS_VSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_VERT,nNewScrollPos,TRUE);
}
/**************************************************
声明： void SetEditHScrollPos(HWND hWnd,int nNewScrollPos)
参数：
	IN hWnd -- 窗口句柄
	IN nNewScrollPos -- 当前新的位置
返回值：无
功能描述：设置水平滚动条新的当前位置。
引用: 
************************************************/
 void SetEditHScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
      if (!(dwStyle&WS_HSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_HORZ,nNewScrollPos,TRUE);
}
/**************************************************
声明： int GetPageLine(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回一页的行数
功能描述：得到一页的行数。
引用: 
************************************************/
 int GetPageLine(HWND hWnd)
{
  int nTextHeight,nPageLine;
  RECT rcClientRect;
		// get line height
    nTextHeight=GetTextHeight(hWnd);
		// get client rect
    GetClientRect(hWnd,&rcClientRect);
		// get page lines
    nPageLine=rcClientRect.bottom/nTextHeight;
    return nPageLine;
}
/**************************************************
声明： void ScrollCaretInPDAEdit(HWND hWnd,int nScrollLine)
参数：
	IN hWnd -- 窗口句柄
	IN nScrollLine -- 滚动的行数
返回值：无
功能描述：滚动光标。
引用: 
************************************************/
 void ScrollCaretInPDAEdit(HWND hWnd,int nScrollLine)
{
	LPEDITITEM lpEditItem;
//	int nClientLine;
	//RECT ClientRect;
	int nPointCoordinate,nLine,nCaretPosition;
	LPTSTR lpLineAddress;
//	int nTextHeight;

			// get line height
//	  	nTextHeight=GetTextHeight(hWnd);


		// get struct PADEDITSTATE data
	   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
     if (lpEditItem==NULL) return;
	   // calculate client lines
//	   nClientLine=GetPageLine(hWnd);
     if (nScrollLine<0)
     {  // scroll to up
				if (lpEditItem->nCaretLine==0&&lpEditItem->nDisplayLine==0) // alread to the top of the edit control
					return;
     }
     if(nScrollLine>0)
     { // scroll down
				if (lpEditItem->nCaretLine+lpEditItem->nDisplayLine>=(lpEditItem->nTotalLine-1))
					return; // alread  to the bottom of the edit control
     }
		 // set caret to new line
		lpEditItem->nCaretLine+=nScrollLine;
		// Get caret line from the text
// !!! Add By Jami chen in 2005.01.29
		if (lpEditItem->nCaretLine+lpEditItem->nDisplayLine < 0)
			lpEditItem->nCaretLine= 0 - lpEditItem->nDisplayLine ;

		if (lpEditItem->nCaretLine+lpEditItem->nDisplayLine>=(lpEditItem->nTotalLine-1))
			lpEditItem->nCaretLine = (lpEditItem->nTotalLine-1) - lpEditItem->nDisplayLine;
// !!! Add End By Jami chen in 2005.01.29
		nLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
		// get address of caret line
		lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
		// get caret xPosition from text
		nPointCoordinate=lpEditItem->nCaretx+lpEditItem->nDisplayx;
		// get caret position
		lpEditItem->lpCaretPosition=GetLinePosition(hWnd,lpEditItem,lpLineAddress,nPointCoordinate,TRUE,&nCaretPosition);
		if (lpEditItem->lpCaretPosition<lpEditItem->lpPDAEditBuffer)
		{ // the caret position on title
			SetCaretToStart(hWnd,lpEditItem);
		}
		else
		{
			// Get caret x-coordinate form window origin
			lpEditItem->nCaretx=nCaretPosition-lpEditItem->nDisplayx;
		}
		// adjust caret coordinate
		if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)
		{
			// clear invert and not redraw
			ClearInvert(hWnd,lpEditItem,FALSE);
			// redraw the windwo
//			DrawPDAEditControl(hWnd);
				InvalidateRect(hWnd,NULL,TRUE);
		}
		else
		{// clear invert and redraw
			ClearInvert(hWnd,lpEditItem,TRUE);
		}
		// reset caret
		SetPDACaret(hWnd);
}
/**************************************************
声明：static void FillPassWordChar(LPTSTR lpEditBuffer,int nFillLength,TCHAR chPassWordWord)
参数：
	IN lpEditBuffer  -- 要填充的缓存
	IN nFillLength -- 要填充的长度
	IN chPassWordWord -- 要用来填充的密码字符
返回值：无
功能描述：将缓存填充为密码字符。
引用: 
************************************************/
 void FillPassWordChar(LPTSTR lpEditBuffer,int nFillLength,TCHAR chPassWordWord)
{
	// fill the buffer
  memset(lpEditBuffer,chPassWordWord,nFillLength);
	// fill end code
  lpEditBuffer[nFillLength]=0;
}
/**************************************************
声明： void DrawLineSeparate(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int nCurLine)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN lpEdititem -- 编辑条目结构指针
	IN nCurLine -- 要画分隔线的行
返回值：无
功能描述：画分隔线。
引用: 
************************************************/
 void DrawLineSeparate(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,int nCurLine)
{
  int x0,y0,x1,y1;
//  RECT rcClientRect;
  int nTextHeight;
  HPEN hPen;
  HGDIOBJ hOldObj;
  COLORREF bkColor;
	int nWindowWidth;

		// create pen
//		bkColor=GetTextColor(hdc);
    hPen=CreatePen(PS_DOT,1,CL_BLACK);
//    hPen=CreatePen(PS_DOT,1,0);
	if( hPen == 0 )
	{
		hPen = 0;
	}
    hOldObj=SelectObject(hdc,hPen);
		// get old bkColor
    bkColor=GetBkColor(hdc);
		// set pen bkColor
    SetBkColor(hdc,CL_WHITE);
		// set line width
	nWindowWidth=(int)(lpEditItem->rect.right-lpEditItem->rect.left);
    x0=0;
    x1=nWindowWidth;
		// set line position
    nTextHeight=GetTextHeight(hWnd);
    y0=y1=lpEditItem->iStartPos+(nCurLine+1)*nTextHeight-1;
		// draw a line
    MoveToEx(hdc,x0,y0,NULL);
    LineTo(hdc,x1,y1);

		// restore the old set
    SetBkColor(hdc,bkColor);
    SelectObject(hdc,hOldObj);
		// delete the pen
    DeleteObject(hPen);
}

/**************************************************
声明： BOOL FormatEdit(HWND hWnd,LPEDITITEM lpEditItem,LPFORMATANALYSIS lpFmtAnalysis)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpFmtAnalysis -- 指向FORMATANALYSIS的指针，格式结果
返回值：成功返回TRUE，否则返回FALSE
功能描述：格式化编辑区。
引用: 
************************************************/
 BOOL FormatEdit(HWND hWnd,LPEDITITEM lpEditItem,LPFORMATANALYSIS lpFmtAnalysis)
{
  int nCount;
//	LPEDITITEM lpEditItem;
  LPTSTR lpCurChar;
  TCHAR lpNumericBuffer[128];

    lpFmtAnalysis->ItemNum=0;
		// get struct PADEDITSTATE data
//	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL) return FALSE;
    if (lpEditItem->lpFormat==NULL) return FALSE;

    // Get Title
    lpFmtAnalysis->lpTitle=lpEditItem->lpFormat;
    lpFmtAnalysis->cTitleLen=GetFmtTextLen(lpFmtAnalysis->lpTitle);

    // Get Item Content
    lpCurChar=lpEditItem->lpFormat+lpFmtAnalysis->cTitleLen;
    while(1)
    {
      if (*lpCurChar==0)
      { // The Format is end
        break;
      }
      // jump char '%'
      lpCurChar++;
      // Get Item char Limited
      nCount=GetFmtNumeric(lpCurChar,lpNumericBuffer);
      (lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ItemNum]).cLimited=(int)atoi(lpNumericBuffer);
      lpCurChar+=nCount;
      // Get Data Style
      (lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ItemNum]).chDataStyle=*lpCurChar;
      lpCurChar++;
      // Get Item Tile Text
      (lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ItemNum]).lpTileText=lpCurChar;
      nCount=GetFmtTextLen(lpCurChar);
      (lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ItemNum]).cTileTextLen=nCount;
      
      // Format a item success, to next item
      lpCurChar+=nCount;
      lpFmtAnalysis->ItemNum++;
      if (lpFmtAnalysis->ItemNum>=10) break;
    }
		// format edit text
    FormatEditText(hWnd,lpEditItem,lpFmtAnalysis);
    return TRUE;
}
/**************************************************
声明： int GetFmtTextLen(LPTSTR lpFmtBunch)
参数：
	IN lpFntBunch -- 要格式的字符串
返回值：返回当前格式标题的长度
功能描述：得到当前格式标题的长度。
引用: 
************************************************/
 int GetFmtTextLen(LPTSTR lpFmtBunch)
{
  int nCount=0;
  LPTSTR lpCurChar=lpFmtBunch;
    while(1)
    {
        if (*lpCurChar=='%'&&(*(lpCurChar+1)>='0'&&*(lpCurChar+1)<='9'))
        {
          // this is begin of the next item
          break; 
        }
        if (*lpCurChar==0)
        {  // The format bunch is end
          break;;
        }
//        if (*lpCurChar<0)
        if (*lpCurChar<0 || *lpCurChar > 0x7F) 
        {  // this is chinese 
          lpCurChar+=2;
          nCount+=2;
        }
        else
        { // this is english or symbol
          lpCurChar++;
          nCount++;
        }
    }
    return nCount;
}
/**************************************************
声明： int GetFmtNumeric(LPTSTR lpCurChar,LPTSTR lpNumericBuffer)
参数：
	IN lpCurChar -- 当前的字符串指针
	OUT lpNumericBuffer -- 存放数字的缓存
返回值：返回数字字串的长度
功能描述：得到当前字串中的数字字串。
引用: 
************************************************/
 int GetFmtNumeric(LPTSTR lpCurChar,LPTSTR lpNumericBuffer)
{
  int nCount=0;
  while(1)
  { 
		// Is numeric ?
    if (*lpCurChar<'0'||*lpCurChar>'9')
      break;
		// store this numeric and to the next character
    *lpNumericBuffer++=*lpCurChar++;
    nCount++;
  }
	//??? Is Need this :
	//    *lpNumericBuffer = 0;
  return nCount;
}
/**************************************************
声明： void FormatEditText(HWND hWnd,LPEDITITEM lpEditItem,LPFORMATANALYSIS lpFmtAnalysis)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpFmtAnalysis -- 指向FORMATANALYSIS的指针，格式结果
返回值：无
功能描述：格式化编辑内容。
引用: 
************************************************/
 void FormatEditText(HWND hWnd,LPEDITITEM lpEditItem,LPFORMATANALYSIS lpFmtAnalysis)
{
//	LPEDITITEM lpEditItem;
  LPTSTR lpCurChar;
  int ItemNum=0;
  int nCount,i;

		// get struct PADEDITSTATE data
//	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
//    if (lpEditItem==NULL) return;

		if (lpEditItem->cbEditLen==0)
		{ // 内容为空
			for (i=0;i<(lpFmtAnalysis->ItemNum-1);i++)
			{  // 初始化格式化条目内容
				lpEditItem->lpPDAEditBuffer[i]=FMTSEPARATE;
				lpEditItem->cbEditLen++;
			}
		}
    lpCurChar=lpEditItem->lpPDAEditBuffer;
    (lpFmtAnalysis->arrayFmtItem[ItemNum]).lpEditText=lpCurChar;
    nCount=0;
    while(1)
    {
        if (*lpCurChar==0||nCount>lpEditItem->cbEditLen)
        { // to the end of the edit buffer
          // set this item edit text length  
          (lpFmtAnalysis->arrayFmtItem[ItemNum]).cEditTextLen=nCount;
          // The Caret is or not in this item
          if (lpEditItem->lpCaretPosition>=(lpFmtAnalysis->arrayFmtItem[ItemNum]).lpEditText&&
              lpEditItem->lpCaretPosition<=lpCurChar)
          {
            lpFmtAnalysis->ActiveItem=ItemNum;
          }
          break;
        }
        if (*lpCurChar==FMTSEPARATE)
        { // to the next Item
          // set this item edit text length  
          (lpFmtAnalysis->arrayFmtItem[ItemNum]).cEditTextLen=nCount;
          // The Caret is or not in this item
          if (lpEditItem->lpCaretPosition>=(lpFmtAnalysis->arrayFmtItem[ItemNum]).lpEditText&&
              lpEditItem->lpCaretPosition<=lpCurChar)
          {
            lpFmtAnalysis->ActiveItem=ItemNum;
          }
          // to next item
          ItemNum++;
          // jump FMTSEPARATE
          lpCurChar++;
          (lpFmtAnalysis->arrayFmtItem[ItemNum]).lpEditText=lpCurChar;
          nCount=0;
          continue;
        }
//        if (*lpCurChar<0)
        if (*lpCurChar<0 || *lpCurChar > 0x7F) 
        {  // 是汉字
          lpCurChar+=2;
          nCount+=2;
        }
        else
        { // 是英文与符号
          lpCurChar++;
          nCount++;
        }
    }
}
/**************************************************
声明： void ShowFormatText(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,LPTSTR lpText,int cTextLen,int *lpnCurCharPos,int *lpnCurLine)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpText -- 要显示的文本
	IN ctextLen -- 要显示的文本长度
	OUT lpnCurCharPos -- 返回当前的显示位置X坐标
	OUT lpnCurLine -- 返回当前的显示位置，行数
返回值：无
功能描述：显示格式化文本。
引用: 
************************************************/
 void ShowFormatText(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem,LPTSTR lpText,int cTextLen,int *lpnCurCharPos,int *lpnCurLine)
{
  SIZE Size;
  RECT rcClientRect;
  int nFit,aDx[128];
  int nTextHeight;
//  LPEDITITEM lpEditItem;
  int x,y;
  int nCurWidth;
  COLORREF colorTextColor,colorBkColor;

      if (cTextLen==0) return;
			// get struct EDITITEM data
//      lpEditItem =(LPEDITITEM)GetWindowLong(hWnd,0);
//      if (lpEditItem==NULL) return;

			// set show clolr and save old color
      colorTextColor=GetTextColor(hdc);
      colorBkColor=GetBkColor(hdc);
	  if (!(lpEditItem->dwStyle&WS_DISABLED))
	  {
// !!! modified End by jami chen in 2004.7.12
//			SetPDAEditColor(hWnd,hdc,NORMALCOLOR);
			if (lpEditItem->dwStyle&ES_READONLY)
			{ // 是只读
				SetPDAEditColor(hWnd,hdc,READONLYCOLOR);
			}
			else
			{  // 一般文本
				SetPDAEditColor(hWnd,hdc,NORMALCOLOR);
			}
// !!! modified End by jami chen in 2004.7.12
	  }
	  else
			SetPDAEditColor(hWnd,hdc,DISABLECOLOR);
			// get the text height
      nTextHeight=GetTextHeight(hWnd);

			// get the width of the text
      GetClientRect(hWnd,&rcClientRect);
      GetTextExtentExPoint(  hdc,         // handle to device context
          lpText, // pointer to character string
          cTextLen,   // number of characters in string
          rcClientRect.right,  // maximum width for formatted string
          &nFit,    // pointer to value specifying max. number of chars.
          aDx,     // pointer to array for partial string widths
          &Size    // pointer to structure with string dimensions
      );
			// will can be show in current line
      nCurWidth=(*lpnCurCharPos+Size.cx);
      if(lpEditItem->dwStyle&ES_MULTILINE)
      {  // 是多行编辑区
        if (nCurWidth>rcClientRect.right)
        {
          *lpnCurCharPos=0;
          (*lpnCurLine)++;
          nCurWidth=Size.cx;
        }
      }
			// get show coordinate
      x=*lpnCurCharPos-lpEditItem->nDisplayx;
			y=(*lpnCurLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos;
			// display this line
			if (y>=0)
				TextOut(hdc,x,y,lpText,cTextLen);
      *lpnCurCharPos=nCurWidth;
      
			// restore the old color
      SetTextColor(hdc,colorTextColor);
      SetBkColor(hdc,colorBkColor);
}

/**************************************************
声明：SIZE GetSize(HWND hWnd,LPTSTR lpText,int cTextLen)
参数：
	IN hWnd -- 窗口句柄
	IN lpText -- 要得到尺寸的文本
	IN cTextLen -- 文本长度
返回值：返回文本的尺寸
功能描述：得到指定文本的尺寸。
引用: 
************************************************/
SIZE GetSize(HWND hWnd,LPTSTR lpText,int cTextLen)
{
  SIZE Size;
  HDC hdc;
      hdc=GetDC(hWnd);
      GetTextExtentExPoint(  hdc,         // handle to device context
          lpText, // pointer to character string
          cTextLen,   // number of characters in string
          10000,  // maximum width for formatted string
          NULL,    // pointer to value specifying max. number of chars.
          NULL,     // pointer to array for partial string widths
          &Size    // pointer to structure with string dimensions
      );
      ReleaseDC(hWnd,hdc);
      return Size;
}
/**************************************************
声明： int GetFmtItem(LPFORMATANALYSIS lpFmtAnalysis,LPTSTR lpCurChar)
参数：
	IN lpFmtAnalysis -- 指向FORMATANALYSIS的指针，格式结果
	IN lpCurChar -- 当前位置
返回值：返回格式条目的次序
功能描述：得到指定的位置在哪一个格式化条目中。
引用: 
************************************************/
 int GetFmtItem(LPFORMATANALYSIS lpFmtAnalysis,LPTSTR lpCurChar)
{
    int i;
      
      for (i=1;i<lpFmtAnalysis->ItemNum;i++)
      {  // 得到当前光标所在的条目索引
          if (lpCurChar<lpFmtAnalysis->arrayFmtItem[i].lpEditText)
            return i;
      }
      return i;
}
/**************************************************
声明： int CanInsertToFmtItem(LPFORMATANALYSIS lpFmtAnalysis,TCHAR HiByte,TCHAR LoByte)
参数：
	IN lpFmtAnalysis -- 指向FORMATANALYSIS的指针，格式结果
	IN HiByte -- 高字节
	IN LoByte -- 低字节
返回值：可以插入返回当前格式还可插入的字符数，否则返回-1
功能描述：指定字符能否插入到格式条目中。
引用: 
************************************************/
 int CanInsertToFmtItem(LPFORMATANALYSIS lpFmtAnalysis,TCHAR HiByte,TCHAR LoByte)
{
    TCHAR chDataStyle;
    int nRemainLen,i;

      // get active item data style
      chDataStyle=lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ActiveItem].chDataStyle;
      switch(chDataStyle)
      {
      case 'd':
        // Only receive numeric
          if (HiByte!=0) return -1;
          if (LoByte>'9'||LoByte<'0') return -1;
        break;
      case 'f':
        // receive numeric and dot ,this is float style 
          if (HiByte!=0) return -1;
          if (LoByte==DOT)
          {  // 是"."
              for (i=0;i<lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ActiveItem].cEditTextLen;i++)
              {
                if (lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ActiveItem].lpEditText[i]==DOT)
                  return -1;
              }
          }
          else if (LoByte>'9'||LoByte<'0') return -1;
        break;
      case 's':
        // can receive any char
        break;
      }
      nRemainLen=lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ActiveItem].cLimited-lpFmtAnalysis->arrayFmtItem[lpFmtAnalysis->ActiveItem].cEditTextLen;
      return nRemainLen;
}
/****************************************************************************************************/
// jump the catet to the next item
// Input :  hWnd ---- handle of the window
/****************************************************************************************************/
/*void JumpToNextItem(HWND hWnd,LPEDITITEM lpEditItem)
{
  DWORD dwStyle;
    // Get Window style
    dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    if (dwStyle&ES_MULTIITEMEDIT)
      SetCaretToNextItem(hWnd,lpEditItem);
}*/
/**************************************************
声明：int GetLineLength(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress,int *lpLineWidth)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpLineAddress -- 行地址
	OUT lpLineWidth -- 当前行的宽度
返回值：返回当前行的字符个数。
功能描述：得到当前行的字符个数。
引用: 
************************************************/
int GetLineLength(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpLineAddress,int *lpLineWidth)
{
	DWORD dwStyle;
	LPTSTR lpCurChar;
	int nAddLen;
  int lpCharWidthBuffer[128];
  int ChineseCharWidth,nCurCharWidth,nWindowWidth;
  int nCurCharPos=0;
  FORMATANALYSIS FmtAnalysis;
  int nFMTItem;
  SIZE Size;


    // Get Char Width
    GetPDACharWidth(hWnd,lpCharWidthBuffer,&ChineseCharWidth);
    // Get Window Width
    nWindowWidth=GetWindowWidth(hWnd,lpEditItem);
		// Get Edit Control Style
//		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    dwStyle=lpEditItem->dwStyle;
		// initialize variable
		lpCurChar=(LPTSTR)lpLineAddress;
		nAddLen=0;
    if (dwStyle&ES_FORMAT)
    {
      FormatEdit(hWnd,lpEditItem,&FmtAnalysis);
      nFMTItem=GetFmtItem(&FmtAnalysis,lpCurChar)-1;;
    }
		while(TRUE)
		{
//			if (*lpCurChar<0)
        	if (*lpCurChar<0 || *lpCurChar > 0x7F) 
			{  // the current character is chinese
        nCurCharWidth=ChineseCharWidth;
			}
			else
			{  // the current character is english or symbol
        if ((dwStyle&ES_FORMAT)&&(*lpCurChar==FMTSEPARATE))
        {  // Is formt item and the current character is system separate
            Size=GetSize(hWnd,(FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                    (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen);
            nCurCharWidth=Size.cx;
            nFMTItem++;
						if (nFMTItem>=FORMATITEMMAXNUM)
							nFMTItem=FORMATITEMMAXNUM-1;
        }
        else
        { // is normal english character
            nCurCharWidth=lpCharWidthBuffer[*lpCurChar];
        }
			}
			if (dwStyle&ES_AUTONEWLINE)
			{	  // Is auto newline
					if (nCurCharPos+nCurCharWidth>nWindowWidth)
					{
						lpLineAddress=lpCurChar;
						break;
					}
			}
			if (*lpCurChar==0x0d&&*(lpCurChar+1)==0x0a)
			{  // this code is enter code , must display this line and to next line
				// to next char
				lpCurChar+=2;
				nAddLen+=2;
				// to exit circle
				break;
			}
			if (*lpCurChar==0x00)
			{  // this is to end edit buffer
        if (dwStyle&ES_FORMAT)
        {  // 是格式化条目
            Size=GetSize(hWnd,(FmtAnalysis.arrayFmtItem[nFMTItem]).lpTileText,
                                    (FmtAnalysis.arrayFmtItem[nFMTItem]).cTileTextLen);
            nCurCharWidth=Size.cx;
			     nCurCharPos+=nCurCharWidth;
        }
				break;
			}

			// to next char
      nCurCharPos+=nCurCharWidth;
//			if (*lpCurChar<0)
	        if (*lpCurChar<0 || *lpCurChar > 0x7F) 
			{  // the current character is chinese
				lpCurChar+=2;
				nAddLen+=2;
			}
			else
			{  // the current character is english or symbol
				lpCurChar++;
				nAddLen++;
			}
		}
		// return current line start address
		if (lpLineWidth)
			*lpLineWidth=nCurCharPos;
		return nAddLen;

}
/**************************************************
声明：void ReleasePDAEdit(HWND hWnd)
参数：
	IN hWnd --窗口句柄
返回值：无
功能描述： 释放编辑区。
引用: 
************************************************/
// delete By Jami chen in 2004.06.12
/* void ReleasePDAEdit(HWND hWnd)
{
    LPEDITITEM lpEditItem;

			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return ;

	    DeletePDACaret(hWnd);  // 删除光标
	    SendNotificationMsg(hWnd,EN_KILLFOCUS); // 通知父窗口

	    	 // 释放缓存
			if (lpEditItem->lpPDASaveEditBuffer)
				free(lpEditItem->lpPDASaveEditBuffer);
			if (lpEditItem->lpPDAControlBuffer)
				free(lpEditItem->lpPDAControlBuffer);
			if (lpEditItem->lpFormat)
				free(lpEditItem->lpFormat);
			free(lpEditItem);
			SetWindowLong(hWnd,0,0);

}
*/
/************************************************************************************/
/**************************************************
声明：void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,int cbEditLen)
参数：
	IN lpSaveBuffer -- 保存数据的缓存
	IN lpBuffer -- 要保存的数据缓存
	IN cbEditLen -- 要保存的数据长度
返回值：无
功能描述：保存编辑区的文本。
引用: 
************************************************/
void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,int cbEditLen)
{
	strncpy(lpSaveBuffer,lpBuffer,cbEditLen); 
}
/**************************************************
声明：int GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,int cbEditLen)
参数：
	IN lpGetBuffer -- 存放得到数据的缓存
	IN lpBuffer -- 原始数据缓存
	IN cbEditLen -- 要得到的数据长度
返回值：返回复制文本的个数
功能描述：得到编辑区的文本。
引用: 
************************************************/
int GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,int cbEditLen)
{
// !!! modify by jami chen in 2004.05.27
//	strncpy(lpGetBuffer,lpBuffer,cbEditLen);
	if (cbEditLen == 0)
		return 0;
	strncpy(lpGetBuffer,lpBuffer,cbEditLen -1); //复制被缓存少一个字符，最后一个字符是 \0
	lpGetBuffer[cbEditLen -1] = 0 ; // 添加结尾符 \0
	return cbEditLen -1 ; // 返回复制文本的个数
// !!! modify End by jami chen in 2004.05.27
}

/**************************************************
声明：LRESULT GetCharFromPos(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN xPos -- x 坐标
	IN yPos -- y 坐标
返回值：低WORD返回指定坐标所在的字符索引位置，高WORD返回该字符所在的行。
功能描述：得到指定点的字符。
引用: 
************************************************/
LRESULT GetCharFromPos(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos)
{
  	DWORD dwStyle;
	  int nLine,nTotalLine,nCaretPosition,nCharIndex;
	  LPTSTR lpLineAddress,lpPosition;
	  int nPointCoordinate;
	  int nTextHeight;
    LRESULT lResult;

      nTextHeight=GetTextHeight(hWnd);


      // Get Edit Control Style
      dwStyle=lpEditItem->dwStyle;
		  // Get the x_coordinate from the text origin
		  nPointCoordinate=xPos+lpEditItem->nDisplayx;


		  if (dwStyle&ES_MULTILINE)
		  { // the edit control is multiple line
			  // get caret line position from window origin
			  nLine=yPos/nTextHeight;

			  // get this window text total line
			  nTotalLine=GetTotalLine(hWnd,lpEditItem);
			  // get caret line position from text origin
			  nLine+=lpEditItem->nDisplayLine;
			  // if the caret line > text total line ,then set caret line
			  // to the text end
			  if (nTotalLine<=nLine)
				  nLine=nTotalLine-lpEditItem->nDisplayLine-1;

			  // if the caret line out the text origin the set caret line to
			  // the text first line
			  if (nLine<0)
			  {
				  nLine=0;
			  }
			  // Get text address of the caret line
			  lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
			  // Get Caret Position
			  lpPosition=GetLinePosition(hWnd,lpEditItem,lpLineAddress,nPointCoordinate,TRUE,&nCaretPosition);
		  }
		  else
		  { // the edit control is single line
			  // the caret line always in the first line
			  nLine=0;
			  // Get Caret Position
  			lpPosition=GetLinePosition(hWnd,lpEditItem,lpEditItem->lpPDAControlBuffer,nPointCoordinate,FALSE,&nCaretPosition);
		  }
      // Get Char Index
      nCharIndex=lpPosition-lpEditItem->lpPDAEditBuffer;
      // make Result
      lResult=MAKELRESULT( nCharIndex,nLine);
      return lResult;
}
/**************************************************
声明：LRESULT SetLimitText(HWND hWnd,LPEDITITEM lpEditItem,int cchMax)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN cchMax -- 文本限制
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置编辑条目的文本限制。
引用: 
************************************************/
LRESULT SetLimitText(HWND hWnd,LPEDITITEM lpEditItem,int cchMax)
{
	 LPTSTR lpRealloc;
   DWORD dwStyle;
   int iCaretPos;

    dwStyle=lpEditItem->dwStyle;
	if (cchMax==0)
		cchMax=INITIALEDITSIZE;
		if (!(dwStyle&ES_FORMAT))
		{
			if(lpEditItem->cbEditLen>cchMax)
				return 0;

			iCaretPos = lpEditItem->lpCaretPosition - lpEditItem->lpPDAEditBuffer; // 得到原来的关标位置
			lpEditItem->cbEditLimitLen=cchMax;
			// charge the window text can put the default buffer
			lpEditItem->cbEditBufferLen=lpEditItem->cbEditLimitLen+1;
			lpEditItem->cbControlBufferLen=lpEditItem->cbEditBufferLen+lpEditItem->cbTitleLen;

			// allocate memory
			lpRealloc=(LPTSTR)realloc((void *)lpEditItem->lpPDAControlBuffer,(ULONG)lpEditItem->cbControlBufferLen);

			if (lpRealloc==NULL)
			{  // allocate failare then free struct EDITITEM and return FALSE
				return 0;
			}
			// add { by jami-chen 2000-08-29
			lpEditItem->lpPDAControlBuffer=lpRealloc;
			// end}

			// allocate memory
			lpRealloc=(LPTSTR)realloc((void *)lpEditItem->lpPDASaveEditBuffer,(ULONG)lpEditItem->cbEditBufferLen);

			if (lpRealloc==NULL)
			{  // allocate failare then free struct EDITITEM and return FALSE
				return 0;
			}
			// add { by jami-chen 2000-08-29
			lpEditItem->lpPDASaveEditBuffer=lpRealloc;
			// end}

			lpEditItem->lpPDAEditBuffer=lpEditItem->lpPDAControlBuffer+lpEditItem->cbTitleLen;
//			Clear(lpEditItem);
// !!! Why need clear edit item context, Now Delete it By Jami chen in 2002.05.16
//			ClearEditItemStruct(lpEditItem);
// !!! Why need clear edit item context, Now Delete it End By Jami chen in 2002.05.16

			// read Window text
			if (dwStyle&ES_PASSWORD)
			{
				// read Window text
				FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
			}
			else
			{
//				GetWindowText(hWnd,lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen+1); // +1 is read end code 'null'
				GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
			}
			// Read Text Success

			// set Initial State to Edit Control
//			lpEditItem->nTotalLine=-1;
//			SetPDAEditScrollPage(hWnd);
			SetWindowRect(hWnd,lpEditItem,TRUE);
			SetEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
			SetEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH));

			// !!! Add By Jami chen in 2003.09.24
			// Reset The Caret position
//			SetPDACaretPosition(hWnd,lpEditItem,0,0);
			lpEditItem->lpCaretPosition = lpEditItem->lpPDAEditBuffer + iCaretPos; // 设置原来的关标位置
			// !!! Add End By Jami  chen in 2003.09.24
			return TRUE;
		}
		else
			return 0;
}
/**************************************************
声明：LRESULT GetLineLengthFromPos(HWND hWnd,LPEDITITEM lpEditItem,int ich)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN ich -- 指定的字符索引
返回值：返回指定字符所在行的字符个数
功能描述：得到指定字符所在行的字符个数。
引用: 
************************************************/
LRESULT GetLineLengthFromPos(HWND hWnd,LPEDITITEM lpEditItem,int ich)
{
	 DWORD dwStyle;
	 LPTSTR lpCurPos;
	 int nLine;
	 int cchLineLength;
    dwStyle=lpEditItem->dwStyle;
		if (dwStyle&ES_MULTILINE)
		{ // 是多行编辑区
				if (ich==-1)
				{ // 字符位置为-1
						if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
						{ // 不存在选择区域
								// 得到光标所在的位置
								lpCurPos=lpEditItem->lpCaretPosition;
								GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE);
								lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine); // 得到行头
								cchLineLength=GetLineLength(hWnd,lpEditItem,lpCurPos,NULL); // 得到行的字符个数
						}
						else
						{  // 有选择区域
								// 得到选择起始行的非选择个数
								lpCurPos=lpEditItem->lpInvertStart;
								GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE);
								lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine);
								cchLineLength=lpEditItem->lpInvertStart-lpCurPos;

								// 得到选择结束行的非选择个数
								lpCurPos=lpEditItem->lpInvertEnd;
								GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE);
								lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine);
								cchLineLength+=GetLineLength(hWnd,lpEditItem,lpCurPos,NULL)-(lpEditItem->lpInvertEnd-lpCurPos);
						}
				}
				else
				{  // 有指定字符位置
						if (ich>lpEditItem->cbEditLen)
							ich=lpEditItem->cbEditLen;
						// 得到字符所在行的字符个数
						lpCurPos=lpEditItem->lpPDAEditBuffer+ich;
						GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE);
						lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine);
						cchLineLength=GetLineLength(hWnd,lpEditItem,lpCurPos,NULL);
				}
		}
		else
		{ // 是 单行编辑区
				cchLineLength=lpEditItem->cbEditLen; // 得到编辑区的字符个数
		}
		return (LRESULT)cchLineLength;
}
/**************************************************
声明：LRESULT GetPosFromChar(HWND hWnd,LPEDITITEM lpEditItem,int wCharIndex)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN wCharIndex -- 指定的字符索引
返回值：返回指定字符的坐标,MAKERESULT(x,y)
功能描述：得到指定字符的坐标。
引用: 
************************************************/
LRESULT GetPosFromChar(HWND hWnd,LPEDITITEM lpEditItem,int wCharIndex)
{
	 DWORD dwStyle;
	 LPTSTR lpCurPos;
	 int nLine,xPos,yPos;
	 int nTextHeight;
	 LRESULT lResult;

		dwStyle=lpEditItem->dwStyle;  // 得到条目风格
		nTextHeight=GetTextHeight(hWnd); // 得到文本高度
		if (wCharIndex>lpEditItem->cbEditLen)
			wCharIndex=lpEditItem->cbEditLen; 
		if (wCharIndex<0) 
				wCharIndex=0;
		lpCurPos=lpEditItem->lpPDAEditBuffer+wCharIndex; // 得到指定字符的位置
		GetCoordinate(hWnd,lpEditItem,lpCurPos,&xPos,&nLine,dwStyle&ES_MULTILINE); // 得到指定字符的坐标位置
		yPos=(nLine-lpEditItem->nDisplayLine)*nTextHeight+lpEditItem->iStartPos; // 得到Y坐标
		lResult=MAKELRESULT(xPos,yPos);
		return (LRESULT)lResult;
}
/**************************************************
声明：void InsertString(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpString,int cchLen)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpString -- 要插入的字符串
	IN cchLen -- 要插入的字符串的长度
返回值：无
功能描述：插入一个字符串。
引用: 
************************************************/
void InsertString(HWND hWnd,LPEDITITEM lpEditItem,LPCTSTR lpString,int cchLen)
{
	LPTSTR lpCurChar;
	CHAR LoByte,HiByte;
    int iLen;


		lpCurChar=(LPTSTR)lpString;  // 得到起始位置
    iLen=0;
		while(*lpCurChar)
		{
//			if (*lpCurChar<0)
	        if (*lpCurChar<0 || *lpCurChar > 0x7F) 
			{  // 是汉字
					HiByte=*lpCurChar;
					lpCurChar++;
					LoByte=*lpCurChar;
					lpCurChar++;
          iLen+=2;
			}
			else if (*lpCurChar==0x0d&&*(lpCurChar+1)==0x0a)
			{ // 是回车 
				HiByte=*lpCurChar;
				lpCurChar++;
				LoByte=*lpCurChar;
				lpCurChar++;
        iLen+=2;
			}
			else
			{ // 是一般字符
				LoByte=*lpCurChar;
				lpCurChar++;
				HiByte=0;
        iLen++;
			}
      if (iLen>cchLen) break;
			if (InsertChar(hWnd,lpEditItem,HiByte,LoByte)==FALSE) break;  // 插入字符
		}
}
/**************************************************
声明：void SetCaretCoordinate(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：无
功能描述：设置光标位置。
引用: 
************************************************/
void SetCaretCoordinate(HWND hWnd,LPEDITITEM lpEditItem)
{
	 int xPos,nLine;
	 DWORD dwStyle;

	  // Get struct EDITITEM data
    dwStyle=lpEditItem->dwStyle;  // 得到条目风格
    GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&xPos,&nLine,dwStyle&ES_MULTILINE); // 得到光标的坐标位置
		lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx;
		lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine;
}
/**************************************************
声明：LRESULT SetPasswordChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR ch)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN ch -- 要设置的密码字符
返回值：成功返回TRUE ， 否则返回FALSE
功能描述：设置编辑区的密码字符。
引用: 
************************************************/
LRESULT SetPasswordChar(HWND hWnd,LPEDITITEM lpEditItem,CHAR ch)
{
		if (ch==0)
		{ // 没有指定字符。清除密码输入
				lpEditItem->dwStyle&=~ES_PASSWORD;
		    GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
		}
		else
		{ // 设定密码输入字符
				lpEditItem->chPassWordWord=ch;
				lpEditItem->dwStyle|=ES_PASSWORD;
				// read Window text
				FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
		}
		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}

/**************************************************
声明：LRESULT SetSel(HWND hWnd,LPEDITITEM lpEditItem,int nStart,int nEnd)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN nStart -- 选择区域的开始
	IN nEnd -- 选择区域的结束
返回值：无
功能描述：设置选择区域。
引用: 
************************************************/
LRESULT SetSel(HWND hWnd,LPEDITITEM lpEditItem,int nStart,int nEnd)
{
		ClearInvert(hWnd,lpEditItem,TRUE);
// !!! Modified By Jami chen in 2002.04.24
//  if the nStart == -1 then clear invert
//		if (nStart<0)
//			nStart=0;
		if (nStart<0)
			return 0;
// !!! modified end By Jami chen in 2002.04.24
		if (nStart>lpEditItem->cbEditLen)
			nStart=lpEditItem->cbEditLen;
// !!! Modified By Jami chen in 2002.04.24
//		if (nEnd<0)
//			nEnd=0;
		if (nEnd<0)
			nEnd=0x7fff;
// !!! modified end By Jami chen in 2002.04.24
		if (nEnd>lpEditItem->cbEditLen)
			nEnd=lpEditItem->cbEditLen;

		if (nStart>nEnd)
		{  // 开始位置大于结束位置
			lpEditItem->lpInvertStart=lpEditItem->lpPDAEditBuffer+nEnd;
			lpEditItem->lpInvertEnd=lpEditItem->lpPDAEditBuffer+nStart;
		}
		else
		{ // 开始位置小于等于结束位置
			lpEditItem->lpInvertStart=lpEditItem->lpPDAEditBuffer+nStart;
			lpEditItem->lpInvertEnd=lpEditItem->lpPDAEditBuffer+nEnd;
		}
		DisplayInvertChange(hWnd,lpEditItem);  // 显示选择改变
    return 0;
}

/**************************************************
声明：LRESULT SetText(HWND hWnd,LPEDITITEM lpEditItem,LPCSTR lpsz)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpsz -- 要设置的文本
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置编辑文本。
引用: 
************************************************/
LRESULT SetText(HWND hWnd,LPEDITITEM lpEditItem,LPCSTR lpsz)
{
//  int cTextlen;
  DWORD dwStyle;
  DWORD cTextlen;

		cTextlen=strlen(lpsz);
//		if (cTextlen > 0x7fff)
//			return FALSE;
		if (cTextlen>(DWORD)lpEditItem->cbEditLimitLen)
		{
			// add { by jami-chen 2000-08-29
			if (SetLimitText(hWnd,lpEditItem,(int)(cTextlen+5))==FALSE)
			// end }
				return FALSE;
		}
		lpEditItem->cbEditLen=(SHORT)cTextlen;
// !!! Modified By Jami chen 2003.06.30
//		GetEditText(lpEditItem->lpPDAEditBuffer,lpsz,(int)(lpEditItem->cbEditLen+1));
//		SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItem->lpPDAEditBuffer,(int)(lpEditItem->cbEditLen+1));
		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		if (dwStyle&ES_PASSWORD)
		{
				// read Window text
			FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
		}
		else
		{
			GetEditText(lpEditItem->lpPDAEditBuffer,lpsz,(int)(lpEditItem->cbEditLen+1));
		}
		SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpsz,(int)(lpEditItem->cbEditLen+1));
// !!! Modified End By Jami chen in 2003.06.30
		lpEditItem->bNeedCalcTotalLine = TRUE;

// !!! Add By Jami chen in 2005.04.05
		SetWindowRect(hWnd,lpEditItem,TRUE);
// !!! Add End

		SendNotificationMsg(hWnd,EN_CHANGE);

//      SetWindowRect(hWnd,lpEditItem,TRUE);
/*    
        SetPDAEditVScrollRange(hWnd,lpEditItem->nTotalLine);
		SetEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
		SetEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH));

		lpEditItem->nDisplayx=0;
      lpEditItem->nDisplayLine=0;
		  SetPDACaretPosition(hWnd,lpEditItem,0,0);
      AdjustCaretInEditItem(hWnd,lpEditItem);
		  // set caret position
		  SetPDACaret(hWnd);

      // show caret
      ShowPDACaret(hWnd);

			InvalidateRect(hWnd,NULL,TRUE);
*/			
	  return TRUE;
}

/**************************************************
声明：LRESULT CopyToClipboard(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：成功返回拷贝字符个数，否则返回0
功能描述：拷贝选择文本到剪切板
引用: 
************************************************/
LRESULT CopyToClipboard(HWND hWnd,LPEDITITEM lpEditItem)
{
  int cchInvertLen;
  HGLOBAL hData ;

		if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
		{  // 没有选择区域
			return 0;
		}
		cchInvertLen=lpEditItem->lpInvertEnd-lpEditItem->lpInvertStart;

		if (cchInvertLen>0)
		{
//			LPTSTR lpClipboard;
//			OpenPDAClipboard();
//			EmptyPDAClipboard();
//			SetPDAClipboardData(lpEditItem->lpInvertStart,cchInvertLen);
//			ClosePDAClipboard();
/*
			OpenClipboard(hWnd);
			lpClipboard = (LPTSTR)malloc(cchInvertLen +1);
			if (lpClipboard )
			{
				memmove(lpClipboard,lpEditItem->lpInvertStart,cchInvertLen);
				lpClipboard[cchInvertLen] = 0;
				SetClipboardData(CF_TEXT,lpClipboard);
			}
			CloseClipboard();
			*/
			if (OpenClipboard(hWnd)) // 打开剪切板
			{
				EmptyClipboard(); // 清空剪切板
				hData = GlobalAlloc(GMEM_MOVEABLE , (cchInvertLen +1));  // 分配空间
				if (hData != NULL)
				{
					LPSTR pszData = (LPSTR) GlobalLock(hData); // 锁定地址
					memmove(pszData ,lpEditItem->lpInvertStart,cchInvertLen);
					pszData[cchInvertLen] = 0;
					GlobalUnlock(hData);
					SetClipboardData(CF_TEXT, hData); // 设置数据
				}
				CloseClipboard();  // 关闭剪切板
			}
		}
		else
			return 0;
		return cchInvertLen;
}

/**************************************************
声明：LRESULT PasteFromClipboard(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：无
功能描述：粘贴剪切板数据到当前编辑区。
引用: 
************************************************/
LRESULT PasteFromClipboard(HWND hWnd,LPEDITITEM lpEditItem)
{
	LPCTSTR lpClipboard;
//	int cchLen;

		DeleteInvert(hWnd,lpEditItem);  // 删除选择
/*		OpenPDAClipboard();
		lpClipboard=GetPDAClipboardData();
		if (lpClipboard==NULL)
		{
			ClosePDAClipboard();
			return 0;
		}
		cchLen=GetPDAClipboardDataLen();
		InsertString(hWnd,lpEditItem,lpClipboard,cchLen);
		ClosePDAClipboard();
*/
//		OpenClipboard(hWnd);
		if (OpenClipboard(hWnd))  // 打开剪切板
		{
			HGLOBAL hData = GetClipboardData(CF_TEXT); // 得到数据
			if (hData != NULL)
			{
				lpClipboard = (LPSTR) GlobalLock(hData);  // 锁定缓存
				if (lpClipboard==NULL)
				{
					CloseClipboard();  // 关闭剪切板
					return 0;
				}
				InsertString(hWnd,lpEditItem,lpClipboard,strlen(lpClipboard)); // 插入剪切板数据
				GlobalUnlock(hData);
			}
			CloseClipboard();  // 关闭剪切板
		}

		SetCaretCoordinate(hWnd,lpEditItem);  // 设置光标位置
		return 0;
}
/**************************************************
声明：int HorzMoveCaret(HWND hWnd,LPEDITITEM lpEditItem,int iHorzPos)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN iHorzPos -- 水平移动标志
返回值：成功返回TRUE， 否则返回FLASE
功能描述：水平移动光标。
引用: 
************************************************/
int HorzMoveCaret(HWND hWnd,LPEDITITEM lpEditItem,int iHorzPos)
{
	LPSTR lpLineAddress;
	int wLineWidth,wLineLength;
	int nShowPos,nShowLine,nLine;
	DWORD dwStyle;
	
    dwStyle=lpEditItem->dwStyle;  // get this item style

	switch(iHorzPos)
	{
		case MOVETOEND:  // to the end
			// Get caret line from the text
			nLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
			// get current line start address
//			lpLineAddress=GetLineAddress(hWnd,lpEditItem,lpEditItem->nCaretLine);
			lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
			// get current line character numbers and pixel width
			wLineLength=GetLineLength(hWnd,lpEditItem,lpLineAddress,&wLineWidth);
			// current position if or not in the end
			if (lpLineAddress+wLineLength!=lpEditItem->lpCaretPosition)
			{  // not in the end
				// move current position to the end
				lpEditItem->lpCaretPosition=lpLineAddress+wLineLength;
				// the end character is or not "Return "
				if (*(lpEditItem->lpCaretPosition-2)==0x0d&&*(lpEditItem->lpCaretPosition-1)==0x0a) 
				{  // is "Return" ,then must to before the return
					lpEditItem->lpCaretPosition-=2;
				}
//				lpLineAddress=GetLineAddress(hWnd,lpEditItem,lpEditItem->nCaretLine);
				// set the Caret new position
				lpEditItem->nCaretx=GetxStartOfLine(hWnd,lpEditItem,lpLineAddress)+wLineWidth;
				// set and show caret
				SetPDACaret(hWnd);
			}
			break;
		case MOVETOHOME:  // to the line home
			// Get caret line from the text
			nLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
			// get the current line start address
//			lpLineAddress=GetLineAddress(hWnd,lpEditItem,lpEditItem->nCaretLine);
			lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
			// if or not in title line
			if (lpLineAddress<=lpEditItem->lpPDAEditBuffer)
			{  // is in the title line 
				// set caret to the item start
				SetCaretToStart(hWnd,lpEditItem);
				// set and show new caret
				SetPDACaret(hWnd);
				break;
			}
			if (lpLineAddress!=lpEditItem->lpCaretPosition)
			{  //  not in the home 
				// set caret position to the line start address
				lpEditItem->lpCaretPosition=lpLineAddress;
				lpEditItem->nCaretx=GetxStartOfLine(hWnd,lpEditItem,lpLineAddress);
				SetPDACaret(hWnd);
			}
			break;
		case MOVETOLEFT: // to the left character
// !!! Modified By jami  in 2003.05.22
// !!!		-- can jump to the prev line when had in the start of this line  !!!
/*
			// Get caret line from the text
			nLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
			// get current line start address
//			lpLineAddress=GetLineAddress(hWnd,lpEditItem,lpEditItem->nCaretLine);
			lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
			// if in the  line start address
			if (lpLineAddress!=lpEditItem->lpCaretPosition)
			{  // not in the line start address
				// move caret to the prior character
				if (MoveCaretToPreChar(hWnd,lpEditItem))
				{  // move character success
					if (lpLineAddress==lpEditItem->lpCaretPosition)
					{  // is already to the start address
						// set caret to the home
						// Modified By Jami chen 2002.4.22 --> the home position is not in the 0 position
						//lpEditItem->nCaretx=0;
						// to
						lpEditItem->nCaretx=0-lpEditItem->nDisplayx;
						// Modified End By Jami chen 2002.04.22
					}
					else
					{
						// get the caret new position
						GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
						lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
					}
					// set and show caret position
					SetPDACaret(hWnd);
				}
			}
*/
// !!! modified to
			// move caret to the prior character
			if (MoveCaretToPreChar(hWnd,lpEditItem))
			{  // move character success
				// get the caret new position
				GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
				lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
				lpEditItem->nCaretLine=nShowLine-lpEditItem->nDisplayLine;
//				lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
				// set and show caret position
				SetPDACaret(hWnd);
			}
// !!! end modified
			break;
		case MOVETORIGHT:  // to the right character
// !!! Modified By jami  in 2003.05.22
// !!!		-- can jump to the next line when had in the end of this line  !!!
/*
			// if the next cahracter is "return" ,then already to the end of the line 
			if (*(lpEditItem->lpCaretPosition)==0x0d&&*(lpEditItem->lpCaretPosition+1)==0x0a) 
				break;
			// Get caret line from the text
			nLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
			// get current line start address
//			lpLineAddress=GetLineAddress(hWnd,lpEditItem,lpEditItem->nCaretLine);
			lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
			// get current line character numbers and pixel width
			wLineLength=GetLineLength(hWnd,lpEditItem,lpLineAddress,&wLineWidth);
			if (lpLineAddress+wLineLength!=lpEditItem->lpCaretPosition)
			{  // not to the end of the line
				    // to the next character
					if (*lpEditItem->lpCaretPosition<0)  // is chinese
						lpEditItem->lpCaretPosition+=2;
					else  // is english
						lpEditItem->lpCaretPosition++;
					// get caret new position
					GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
					lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
					lpEditItem->nCaretLine=nShowLine-lpEditItem->nDisplayLine;
					// set and show caret
					SetPDACaret(hWnd);
			}
*/
// !!! Modified to 
			if (*lpEditItem->lpCaretPosition == 0)
				break; // had in the end of this text
//			if (*lpEditItem->lpCaretPosition<0)  // is chinese
			if (*lpEditItem->lpCaretPosition<0 || *lpEditItem->lpCaretPosition > 0x7F)  // is chinese
				lpEditItem->lpCaretPosition+=2;
			else  // is english
			{
				if (*(lpEditItem->lpCaretPosition)==0x0d&&*(lpEditItem->lpCaretPosition+1)==0x0a) 
					lpEditItem->lpCaretPosition+=2;  // is CR then must jump 2 byte
				else
					lpEditItem->lpCaretPosition++;
			}
			// get caret new position
			GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
			lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
			lpEditItem->nCaretLine=nShowLine-lpEditItem->nDisplayLine;
			// set and show caret
			SetPDACaret(hWnd);
// !!! end modified 
			break;
		default:
			return 0;
	}
	return 1;
}

/**************************************************
声明：BOOL InCallBackPos(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN xPos -- x坐标
	IN yPos -- y坐标
返回值：是回调标志上，返回TRUE， 否则返回FALSE
功能描述：指定点是否在回调标志上。
引用: 
************************************************/
BOOL InCallBackPos(HWND hWnd,LPEDITITEM lpEditItem,int xPos,int yPos)
{
	int iWindowWidth,nTextHeight;

	nTextHeight=GetTextHeight(hWnd);
	if (yPos<0||yPos>=nTextHeight)
	{  // Must in first line
		return FALSE;
	}
	iWindowWidth=GetWindowWidth(hWnd,lpEditItem);  // 得到窗口宽度
	if (xPos>iWindowWidth)
		return TRUE;
	return FALSE;
 
}
/**************************************************
声明：static void ShowCallBackFlag(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN lpEdititem -- 编辑条目结构指针
返回值：无
功能描述：显示回调标志。
引用: 
************************************************/
static void ShowCallBackFlag(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem)
{
	int x,y;

		x=lpEditItem->nDisplayx+GetWindowWidth(hWnd,lpEditItem);
		y=lpEditItem->iStartPos;
		
		Line(hdc,x+4,y+5,x+12,y+5);
		Line(hdc,x+5,y+6,x+11,y+6);
		Line(hdc,x+6,y+7,x+10,y+7);
		Line(hdc,x+7,y+8,x+9, y+8);
		Line(hdc,x+8,y+9,x+8, y+9);
}

/**************************************************
声明：HBRUSH GetBkBrush(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
返回值：返回背景刷子
功能描述：得到背景刷子。
引用: 
************************************************/
HBRUSH GetBkBrush(HWND hWnd,LPEDITITEM lpEditItem)
{
	HBRUSH hBrush;
	COLORREF cl_BkColor;
	CTLCOLORSTRUCT CtlColor;
//	DWORD dwStyle;

//		cl_BkColor = SendMessage(hWnd,EM_GETCOLOR,NORMALTEXTBKCOLOR,0);
		
//		dwStyle = GetWindowLong(hWnd,GWL_STYLE);

		// 得到背景颜色
// !!! modified by jami chen in 2004.07.12
		if (lpEditItem)
		{
			if (!(lpEditItem->dwStyle&WS_DISABLED))
			{
				if (lpEditItem->dwStyle&ES_READONLY)
				{ // 是只读
					CtlColor.fMask = CLF_READONLYBKCOLOR; 
					SendMessage(hWnd,WM_GETCTLCOLOR,0,(LPARAM)&CtlColor);

					cl_BkColor = CtlColor.cl_ReadOnlyBk;
				}
				else
				{  // 一般文本
					CtlColor.fMask = CLF_TEXTBKCOLOR ; 
					SendMessage(hWnd,WM_GETCTLCOLOR,0,(LPARAM)&CtlColor);

					cl_BkColor = CtlColor.cl_TextBk;
				}
			}
			else
			{  // DISABLE 文本北京
				CtlColor.fMask = CLF_DISABLEBKCOLOR ; 
				SendMessage(hWnd,WM_GETCTLCOLOR,0,(LPARAM)&CtlColor);

				cl_BkColor = CtlColor.cl_DisableBk;
			}
		}
		else
		{ // 一般文本背景
			CtlColor.fMask = CLF_TEXTBKCOLOR ; 
			SendMessage(hWnd,WM_GETCTLCOLOR,0,(LPARAM)&CtlColor);

			cl_BkColor = CtlColor.cl_TextBk;
		}
// !!! modified by jami chen in 2004.07.12

		hBrush=CreateSolidBrush(cl_BkColor); // 创建背景画刷
		return hBrush;
}

/**************************************************
声明：int Edit_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
参数：
	IN hWnd -- 窗口句柄
	IN nBar -- 是垂直滚动条还是水平滚动条
	IN nPos -- 当前位置
	IN bRedraw -- 重绘标志
返回值：无
功能描述：设置滚动条当前位置。
引用: 
************************************************/
int Edit_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS|SIF_DISABLENOSCROLL;
			si.nPos=nPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // 设置滑块的位置
			return 0;
}
/**************************************************
声明：int Edit_GetScrollPos(HWND hWnd, int nBar)
参数：
	IN hWnd -- 窗口句柄
	IN nBar -- 是垂直滚动条还是水平滚动条
返回值：返回当前位置
功能描述：得到滚动条当前位置。
引用: 
************************************************/
int Edit_GetScrollPos(HWND hWnd, int nBar)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS|SIF_DISABLENOSCROLL;
			GetScrollInfo(hWnd,nBar,&si);  // 得到滑块的位置
			return si.nPos;
}

/**************************************************
声明：BOOL Edit_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
参数：
	IN hWnd -- 窗口句柄
	IN nBar -- 是垂直滚动条还是水平滚动条
	IN nMinPos -- 最小位置
	IN nMaxPos -- 最大位置
	IN bRedraw -- 重绘标志
返回值：无
功能描述：设置滚动条范围。
引用: 
************************************************/
BOOL Edit_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_RANGE|SIF_DISABLENOSCROLL;
			si.nMin=nMinPos;
			si.nMax=nMaxPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // 设置滚动条的范围
			return TRUE;
}

