/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：日期设置类
版本号：1.0.0
开发时期：2002-03-17
作者：
修改记录：
    2003-06-05: DoLButtonDown 增加 SetFocus
    2003-04-21: 将InvalidateRect 改为RrefreshWindow, 修改坐标 by LN
    2003-04-17: 将DATETIME 结构改为 SYSTEMTIME结构 ， by LN
******************************************************/

#include <eframe.h>
#include <edateset.h>
#include <eupdown.h>
#include <ecombo.h>
#include <ettime.h>
#include <edialog.h>
#include <ecomctrl.h>

#ifdef EML_DOS
#include "Dos.h"
#include <stdio.h>
#endif

//#define _MAPPOINTER

// Varible extern area
//extern HINSTANCE hInst;

#define ID_MONTH   100
#define ID_YEAR    101
#define ID_UPDOWN  102

#define CALENDARSTARTX 10 //5//LN:2003-04-21,修改
#define CALENDARSTARTY 46
#define CALENDARWIDTH  16
#define CALENDARHEIGHT 12


typedef struct DateSetStruct
{
   WORD wYear;
   WORD wMonth;
   WORD wDay;
   WORD iWeekOfTheMonth;
} DATESETSTRUCT,*LPDATESETSTRUCT;

// Function declare area
static void DrawWeekItem(HWND hWnd,HDC hdc);
//static void Itoa(int value,char *lpString);
static void DrawDayOfTheMonth(HWND hWnd,HDC hdc);
static void InvertSelectDay(HDC hdc,LPDATESETSTRUCT lpDateSetStruct);
static void MonthChange(HWND hWnd,LPDATESETSTRUCT lpDateSetStruct,WORD wMonth);
static void YearChange(HWND hWnd,LPDATESETSTRUCT lpDateSetStruct,WORD wYear);

static const char classSETDATE[] = "SETDATE";
static LRESULT CALLBACK SetDateWndProc( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static VOID DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroy(HWND hWnd);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeltaPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam);

// Function define area
ATOM RegisterSetDateClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = SetDateWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION );
    wc.hCursor = 0;//LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_WINDOW);//(HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = classSETDATE;

    return (BOOL)(RegisterClass( &wc ));
}

static LRESULT CALLBACK SetDateWndProc( HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch (uMsg)
	{
    case WM_CREATE:
		return DoCreate(hWnd,wParam,lParam);
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DoPaint(hWnd,hdc);
		EndPaint(hWnd, &ps);
		break;
    case WM_LBUTTONDOWN:
		return DoLButtonDown(hWnd,wParam,lParam);
    case WM_COMMAND:
		return DoCommand(hWnd,wParam,lParam);
	case WM_NOTIFY:
		return DoNotify(hWnd,wParam,lParam);
	case SDM_GETDATETIME:
		return DoGetDateTime(hWnd,wParam,lParam);
	case SDM_SETDATETIME:
		return DoSetDateTime(hWnd,wParam,lParam);
	case WM_DESTROY:
		DoDestroy(hWnd);
		break;
	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

static VOID DoPaint(HWND hWnd,HDC hdc)
{
	//SetBkColor(hdc,CL_LIGHTGRAY);
	DrawWeekItem(hWnd,hdc);
	DrawDayOfTheMonth(hWnd,hdc);
}

//LN:2003-04-21, 增加
static VOID RefreshWindow( HWND hWnd )
{
	HDC hdc = GetDC( hWnd );
	DoPaint( hWnd, hdc );
	ReleaseDC( hWnd, hdc );
}
//

static void DrawWeekItem(HWND hWnd,HDC hdc)
{
	LPTSTR lpWeekItem="日一二三四五六";
	//TextOut(hdc, 5, 26, lpWeekItem, strlen(lpWeekItem));// LN:2003-04-21,删除
	TextOut(hdc, CALENDARSTARTX, 26, lpWeekItem, strlen(lpWeekItem)); // LN:2003-04-21,增加
}

static void DrawDayOfTheMonth(HWND hWnd,HDC hdc)
{
	WORD iDays,wWeek;
	int iLine=0,i;
	CHAR lpDay[10];
	HFONT hFont;
	RECT rect;
	LPDATESETSTRUCT lpDateSetStruct;
	
	lpDateSetStruct=(LPDATESETSTRUCT)GetWindowLong(hWnd,0);
	if (lpDateSetStruct==NULL)
		return;
	
	iDays=GetDayofTheMonth(lpDateSetStruct->wYear,lpDateSetStruct->wMonth);
	wWeek=lpDateSetStruct->iWeekOfTheMonth;
	
	hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_ENGLISH8X6) );
	rect.top=CALENDARSTARTY;
	rect.bottom=rect.top+8;
	for (i=1;i<=iDays;i++)
	{
		//Itoa(i,lpDay);
		itoa( i, lpDay, 10 );
		rect.left=CALENDARSTARTX+wWeek*CALENDARWIDTH;
		rect.right=rect.left+12;
		DrawText( hdc, lpDay, strlen( lpDay ), &rect, DT_CENTER|DT_NOPREFIX );
		wWeek++;
		if (wWeek>6)
		{
			wWeek=0;
			iLine++;
			rect.top=CALENDARSTARTY+iLine*CALENDARHEIGHT;
			rect.bottom=rect.top+8;
		}
	}
	SelectObject( hdc, hFont );
	InvertSelectDay(hdc,lpDateSetStruct);
}

static void InvertSelectDay(HDC hdc,LPDATESETSTRUCT lpDateSetStruct)
{
   RECT rect;
   WORD wWeek,iLine;//,iDays;

//          iDays=GetDayofTheMonth(lpDateSetStruct->wYear,lpDateSetStruct->wMonth);

          wWeek=(lpDateSetStruct->iWeekOfTheMonth+lpDateSetStruct->wDay-1)%7;
          iLine=(lpDateSetStruct->iWeekOfTheMonth+lpDateSetStruct->wDay-1)/7;

          rect.left=CALENDARSTARTX+wWeek*CALENDARWIDTH;
          rect.right=rect.left+12;
          rect.top=CALENDARSTARTY+iLine*CALENDARHEIGHT;
          rect.bottom=rect.top+8;

          InvertRect(hdc,&rect);
}
/*
static void Itoa(int value,char *lpString)
{
   if (value>=10) 
   {
     lpString[0]=value/10+'0';
     lpString[1]=value%10+'0';
     lpString[2]=0;
   }
   else
   {
     lpString[0]=value+'0';
     lpString[1]=0;
   }
}
*/

static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   HWND hYear,hComb,hUpDown;
   LPDATESETSTRUCT lpDateSetStruct;
   SYSTEMTIME datetime;
   HINSTANCE hInst;
   int iTitleHeight = GetSystemMetrics( SM_CYCAPTION );

    lpDateSetStruct=(LPDATESETSTRUCT)malloc(sizeof(DATESETSTRUCT));
    if (lpDateSetStruct==NULL)
      return -1;

    GetDateTime(&datetime);
    lpDateSetStruct->wYear=datetime.wYear;
    lpDateSetStruct->wMonth=datetime.wMonth;
    lpDateSetStruct->wDay=datetime.wDay;

    lpDateSetStruct->iWeekOfTheMonth=GetWeekData(datetime.wYear,datetime.wMonth,1);

	hInst=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
	hYear=CreateWindow(classSTATIC,
		"",
		WS_CHILD|WS_VISIBLE|WS_BORDER,
		3,3,40,iTitleHeight,//18, //LN:2003-04-21,  修改
		hWnd,
		(HMENU)ID_YEAR,
		hInst,
		NULL);
	hUpDown=CreateWindow(classUPDOWN32,
		"",
		WS_CHILD|WS_VISIBLE|WS_BORDER|UDS_WRAP|UDS_NOTHOUSANDS|UDS_SETBUDDYINT|UDS_ARROWKEYS,
		43,3,GetSystemMetrics( SM_CXVSCROLL ),iTitleHeight,//18, //LN:2003-04-21,  修改
		hWnd,
		(HMENU)ID_UPDOWN,
		hInst,
		NULL);
	hComb=CreateWindow(classCOMBOBOX,
		"",
		WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,
		59,3,69,72,
		hWnd,
		(HMENU)ID_MONTH,
		hInst,
		NULL);

//    SendMessage(hYear,WM_SETTEXT,0,(LPARAM)"2000");
    SendMessage(hUpDown,UDM_SETBUDDY,(WPARAM)hYear,0);
    SendMessage(hUpDown,UDM_SETRANGE,(WPARAM)0,(LPARAM)MAKELONG(2099,1980));
    SendMessage(hUpDown,UDM_SETPOS,(WPARAM)0,(LPARAM)MAKELONG(lpDateSetStruct->wYear,0));

//    SendMessage(hUpDown,UDM_SETBASE,(WPARAM)16,(LPARAM)0);

    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"一月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"二月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"三月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"四月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"五月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"六月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"七月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"八月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"九月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"十月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"十一月");
    SendMessage(hComb,CB_ADDSTRING,0,(LPARAM)"十二月");

    SendMessage(hComb,CB_SETCURSEL,(lpDateSetStruct->wMonth-1),0);


    SetWindowLong(hWnd,0,(DWORD)lpDateSetStruct);

    return 0;
}

static LRESULT DoDestroy(HWND hWnd)
{
	LPDATESETSTRUCT lpDateSetStruct;
	
	lpDateSetStruct=(LPDATESETSTRUCT)GetWindowLong(hWnd,0);
	if (lpDateSetStruct==NULL)
		return 0;
	   free(lpDateSetStruct);
	   return 0;
}

static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;
	int iRow,iCol,wDay,iDaysOfTheMonth;
	
	LPDATESETSTRUCT lpDateSetStruct;
	HDC hdc;
	
	lpDateSetStruct=(LPDATESETSTRUCT)GetWindowLong(hWnd,0);
	if (lpDateSetStruct==NULL)
		return 0;
	point.x=(short)LOWORD(lParam);
	point.y=(short)HIWORD(lParam);
	
	if (point.y<CALENDARSTARTY)
		return 0;
	
	iCol=(point.x-CALENDARSTARTX)/CALENDARWIDTH;
	iRow=(point.y-CALENDARSTARTY)/CALENDARHEIGHT;
	
	if (iCol>6)
		return 0;
	
	wDay=iRow*7+iCol+1-lpDateSetStruct->iWeekOfTheMonth;
	
	iDaysOfTheMonth=GetDayofTheMonth(lpDateSetStruct->wYear,lpDateSetStruct->wMonth);
	
	if (wDay>=1&&wDay<=iDaysOfTheMonth&&wDay!=lpDateSetStruct->wDay)
	{
		hdc=GetDC(hWnd);
		InvertSelectDay(hdc,lpDateSetStruct);
		lpDateSetStruct->wDay=wDay;
		InvertSelectDay(hdc,lpDateSetStruct);
		ReleaseDC(hWnd,hdc);
	}
	SetFocus( hWnd );//LN, 2003-06-05, 增加
	return 0;
}

static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDATESETSTRUCT lpDateSetStruct;
	NMHDR   *hdr=(NMHDR   *)lParam;
	
	lpDateSetStruct=(LPDATESETSTRUCT)GetWindowLong(hWnd,0);
	if (lpDateSetStruct==NULL)
		return 0;
	switch(hdr->code)
	{
    case UDN_DELTAPOS:
	    DoDeltaPos(hWnd,wParam,lParam);
		break;
	}
	return 0;
}

static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT id,iNotifiedMsg;
	HWND hItemWnd;
	LPDATESETSTRUCT lpDateSetStruct;
	WORD wMonth;
	
	lpDateSetStruct=(LPDATESETSTRUCT)GetWindowLong(hWnd,0);
	if (lpDateSetStruct==NULL)
		return 0;
	
	hItemWnd=(HWND)lParam;
	id=LOWORD(wParam);
	iNotifiedMsg=(UINT)HIWORD(wParam);
	
	if (id==ID_MONTH)
	{
		switch(iNotifiedMsg)
		{
		case CBN_SELCHANGE:
			wMonth=SendMessage(hItemWnd,CB_GETCURSEL,0,0)+1;
			if (wMonth!=lpDateSetStruct->wMonth)
			{
				MonthChange(hWnd,lpDateSetStruct,wMonth);
			}
			break;
		default:
			break;
		}
	}
	return 0;
}

static LRESULT DoDeltaPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMUPDOWN lpnmUpDown=(LPNMUPDOWN)lParam;
	LPDATESETSTRUCT lpDateSetStruct;
	WORD wYear;
	
	lpDateSetStruct=(LPDATESETSTRUCT)GetWindowLong(hWnd,0);
	if (lpDateSetStruct==NULL)
		return FALSE;
	
	wYear=lpnmUpDown->iPos;
	YearChange(hWnd,lpDateSetStruct,wYear);
	
	return TRUE;
}

static LRESULT DoGetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSYSTEMTIME  lpDateTime;
	LPDATESETSTRUCT lpDateSetStruct;

#ifdef _MAPPOINTER
	lParam = MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

	lpDateTime=(LPSYSTEMTIME)lParam;

	lpDateSetStruct=(LPDATESETSTRUCT)GetWindowLong(hWnd,0);
    if (lpDateSetStruct==NULL)
       return FALSE;

	lpDateTime->wYear=lpDateSetStruct->wYear;
	lpDateTime->wMonth=lpDateSetStruct->wMonth;
	lpDateTime->wDay=lpDateSetStruct->wDay;
	lpDateTime->wDayOfWeek =lpDateSetStruct->iWeekOfTheMonth;

	return TRUE;
}

static LRESULT DoSetDateTime(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSYSTEMTIME  lpDateTime;
	LPDATESETSTRUCT lpDateSetStruct;
	HWND hComb,hUpDown;

#ifdef _MAPPOINTER
	lParam = MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

	lpDateTime=(LPSYSTEMTIME)lParam;

	lpDateSetStruct=(LPDATESETSTRUCT)GetWindowLong(hWnd,0);
    if (lpDateSetStruct==NULL)
       return FALSE;

	lpDateSetStruct->wYear=lpDateTime->wYear;
	lpDateSetStruct->wMonth=lpDateTime->wMonth;
	lpDateSetStruct->wDay=lpDateTime->wDay;
    lpDateSetStruct->iWeekOfTheMonth=GetWeekData(lpDateSetStruct->wYear,lpDateSetStruct->wMonth,1);

	hUpDown=GetDlgItem(hWnd,ID_UPDOWN);
    SendMessage(hUpDown,UDM_SETPOS,(WPARAM)0,(LPARAM)MAKELONG(lpDateSetStruct->wYear,0));
	hComb=GetDlgItem(hWnd,ID_MONTH);
    SendMessage(hComb,CB_SETCURSEL,(lpDateSetStruct->wMonth-1),0);
	//InvalidateRect(hWnd,NULL,TRUE);//LN:2003-04-21, 删除
	RefreshWindow( hWnd );//LN:2003-04-21, 增加
	return TRUE;
}

static void MonthChange(HWND hWnd,LPDATESETSTRUCT lpDateSetStruct,WORD wMonth)
{
     WORD iDaysOfTheMonth;
     lpDateSetStruct->wMonth=wMonth;
     lpDateSetStruct->iWeekOfTheMonth=GetWeekData(lpDateSetStruct->wYear,lpDateSetStruct->wMonth,1);
     iDaysOfTheMonth=GetDayofTheMonth(lpDateSetStruct->wYear,lpDateSetStruct->wMonth);
     if (lpDateSetStruct->wDay>iDaysOfTheMonth)
       lpDateSetStruct->wDay=iDaysOfTheMonth;
     //InvalidateRect(hWnd,NULL,TRUE);//LN:2003-04-21, 删除
	 RefreshWindow( hWnd );//LN:2003-04-21, 增加
}

static void YearChange(HWND hWnd,LPDATESETSTRUCT lpDateSetStruct,WORD wYear)
{
	WORD iDaysOfTheMonth;
	lpDateSetStruct->wYear=wYear;
	lpDateSetStruct->iWeekOfTheMonth=GetWeekData(lpDateSetStruct->wYear,lpDateSetStruct->wMonth,1);
	iDaysOfTheMonth=GetDayofTheMonth(lpDateSetStruct->wYear,lpDateSetStruct->wMonth);
	if (lpDateSetStruct->wDay>iDaysOfTheMonth)
		lpDateSetStruct->wDay=iDaysOfTheMonth;
	//InvalidateRect(hWnd,NULL,TRUE); //LN:2003-04-21, 删除
	RefreshWindow( hWnd );//LN:2003-04-21, 增加
}




