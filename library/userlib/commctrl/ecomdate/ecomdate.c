/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：日期编辑类
版本号：1.0.0
开发时期：2002-06-10
作者：
修改记录：
    2003-04-17:将DATETIME 结构改为SYSTEMTIME, BY LN
******************************************************/

#include <ewindows.h>
#include <esymbols.h>
#include <edateset.h>
#include <ecomdate.h>
#include <ecomctrl.h>

#define	YEAR_WIDTH				40
#define MONTH_WIDTH				20
#define	DAY_WIDTH				20
#define COMDATE_WIDTH			130
#define COMDATE_HIGH			120
#define DATESET_DROPDOWN		1

#define DATEOFFSET              0

typedef struct	_COMDATE
{
	HWND				hDate;
	SYSTEMTIME			dt;
	RECT				rtDrop;
	DWORD				dState;
}COMDATE, * LPCOMDATE;

const TCHAR	classCOMDATE[]="_COMDATE_";

static LRESULT CALLBACK ComDateProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

ATOM RegisterComDateClass(HINSTANCE hInstance)
{
	WNDCLASS wcex;
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ComDateProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classCOMDATE;
	return RegisterClass(&wcex);
}

static void SendNotifyMessage( HWND hWnd, const SYSTEMTIME * lpdt )
{
	struct DATETIMENMHDR	dth;
	memcpy( &( dth.dt ), lpdt, sizeof( SYSTEMTIME ) );
	dth.hdr.code = 0 ;
	dth.hdr.hwndFrom = hWnd;
	dth.hdr.idFrom = GetWindowLong( hWnd, GWL_ID );
	SendMessage( GetParent( hWnd ), WM_NOTIFY, dth.hdr.idFrom, (LPARAM)&dth );
}
/*
static void DrawBitMap(HDC hdc,HBITMAP hBitMap,RECT rect)
{
	HDC hMemoryDC;
	if(hBitMap==0)
		return;
	hMemoryDC=CreateCompatibleDC(hdc);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	BitBlt( hdc,(short)rect.left,(short)rect.top,(short)(rect.right-rect.left),
		(short)(rect.bottom-rect.top), hMemoryDC,-3,-5,
				SRCCOPY);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	DeleteDC(hMemoryDC);
}
*/

static void DoPaintComDate(HWND hWnd){
	PAINTSTRUCT				ps;
	HDC						hdc;
	LPCOMDATE				lpcd;
	TCHAR					szTemp[20];
	RECT					rt;
	HFONT hFont;
	//HBITMAP					hBmp;
	lpcd=(LPCOMDATE)GetWindowLong(hWnd,DATEOFFSET);
	ASSERT(lpcd!=NULL);
	hdc = BeginPaint(hWnd, &ps);


	//hBmp=CreateBitmap(8,6,1,1,bmpDropDown);
	//DrawBitMap(hdc,hBmp,lpcd->rtDrop);
	//DeleteObject(hBmp);
	hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL16X16) );//
	szTemp[0] = SYM_DOWNARROW;
	SetBkColor( hdc, GetSysColor(COLOR_BTNFACE) );
	SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );
	DrawText( hdc, szTemp, 1, &lpcd->rtDrop, DT_CENTER|DT_VCENTER|DT_SINGLELINE );	
	SelectObject( hdc, hFont );

	sprintf(szTemp," %4d年%2d月%2d日",lpcd->dt.wYear,lpcd->dt.wMonth,lpcd->dt.wDay);
	GetClientRect(hWnd,&rt);

	SetBkColor( hdc, GetSysColor(COLOR_WINDOW) );
	SetTextColor( hdc, GetSysColor(COLOR_WINDOWTEXT) );
	DrawText(hdc,szTemp,strlen(szTemp),&rt,DT_VCENTER|DT_SINGLELINE|DT_LEFT);

	EndPaint(hWnd, &ps);
}

static void DoDestroy(HWND hWnd){
	LPCOMDATE				lpcd;
	lpcd=(LPCOMDATE)GetWindowLong(hWnd,DATEOFFSET);
	ASSERT(lpcd!=NULL);
	free(lpcd);
}

static BOOL OnCreate(HWND hWnd){
	LPCOMDATE			lpcd;
	HINSTANCE			hIns;
	RECT				rtClient,rtWindow;

	lpcd=(LPCOMDATE)malloc(sizeof(COMDATE));
	if(lpcd==NULL)
		return FALSE;
	memset( lpcd, 0, sizeof(COMDATE) );
	SetWindowLong(hWnd,DATEOFFSET,(long)lpcd);
	hIns=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
	GetWindowRect(hWnd,&rtWindow);
	lpcd->hDate=CreateWindow(classSETDATE,"",WS_POPUP|WS_BORDER,rtWindow.left,rtWindow.bottom,
		COMDATE_WIDTH,COMDATE_HIGH,hWnd,NULL,hIns,NULL);
	if(lpcd->hDate==0){
		free( lpcd );  //LN:2004-04-18
		return FALSE;
	}
	GetClientRect(hWnd,&rtClient);
	lpcd->rtDrop=rtClient;
	lpcd->rtDrop.left=lpcd->rtDrop.right-20;
	//SetWindowLong(hWnd,DATEOFFSET,(long)lpcd);//LN:2004-04-18,多余

	GetDateTime(&(lpcd->dt));
	//InvalidateRect(hWnd,&rtClient,TRUE);//LN:2004-04-18，多余
	return TRUE;
}

static void RetractPopupSetDate(HWND hWnd){
	LPCOMDATE				lpcd;
	RECT					rt;
	lpcd=(LPCOMDATE)GetWindowLong(hWnd,DATEOFFSET);
	ASSERT(lpcd!=NULL);
	SendMessage(lpcd->hDate,SDM_GETDATETIME,0,(LPARAM)&(lpcd->dt));
	SendNotifyMessage( hWnd, &lpcd->dt );
	ShowWindow(lpcd->hDate,SW_HIDE);
	lpcd->dState&=~DATESET_DROPDOWN;
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE);
}

static void PopupSetDate(HWND hWnd){
	LPCOMDATE				lpcd;
	MSG						msg;
	RECT					rect,rcClient;

	lpcd=(LPCOMDATE)GetWindowLong(hWnd,DATEOFFSET);
	ASSERT(lpcd!=NULL);

    GetWindowRect(hWnd,&rect);
    GetClientRect(hWnd,&rcClient);
	if(rect.bottom>200)
	  SetWindowPos(lpcd->hDate,0, rect.left, rect.top-120,0,0,
			SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOACTIVATE);
	else
	  SetWindowPos(lpcd->hDate,0, rect.left, rect.bottom,0,0,
			SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOACTIVATE);

	SendMessage(lpcd->hDate,SDM_SETDATETIME,0,(LPARAM)&(lpcd->dt));

    while(lpcd->dState&DATESET_DROPDOWN){
		if (GetMessage( &msg, 0, 0, 0 )==TRUE){
			if (msg.message==WM_CLOSE){
				RetractPopupSetDate(hWnd);
				break;
			}
			if (msg.message==WM_LBUTTONDOWN||
				msg.message==WM_NCLBUTTONDOWN){
				GetWindowRect(lpcd->hDate,&rect);
				if (PtInRect(&rect,msg.pt)==0){
					RetractPopupSetDate(hWnd);
					break;
				}
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
}

static BOOL DoLButtonDown(HWND hWnd,int xPos,int yPos){
	LPCOMDATE				lpcd;
	RECT					rt;
	HDC						hdc;
	lpcd=(LPCOMDATE)GetWindowLong(hWnd,DATEOFFSET);
	ASSERT(lpcd!=NULL);
	lpcd->dState|=DATESET_DROPDOWN;
	GetClientRect(hWnd,&rt);
	hdc=GetDC(hWnd);
	InvertRect(hdc,&rt);
	PopupSetDate(hWnd);
	InvertRect(hdc,&rt);
	ReleaseDC(hWnd,hdc);
	return TRUE;
}

static void DoGetSetDateTime(HWND hWnd,LPSYSTEMTIME lpdt,BOOL isSet){
	LPCOMDATE					lpcd;
	lpcd=(LPCOMDATE)GetWindowLong(hWnd,DATEOFFSET);
	ASSERT(lpcd!=NULL);
	if(isSet==TRUE){
		memcpy(&(lpcd->dt),lpdt,sizeof(SYSTEMTIME));
//		SendNotifyMessage( hWnd, lpcd->dt );
	}else{
		memcpy(lpdt,&(lpcd->dt),sizeof(SYSTEMTIME));
	}
}

static LRESULT CALLBACK ComDateProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		if(OnCreate(hWnd)==FALSE)
			return -1;
		return 0;
	case WM_PAINT:
		DoPaintComDate(hWnd);
		return 0;
   	case WM_LBUTTONDOWN:
		DoLButtonDown(hWnd,LOWORD(lParam),HIWORD(lParam));
		return 0;
	case CDM_GETDATETIME:
		DoGetSetDateTime(hWnd,(LPSYSTEMTIME)lParam,FALSE);
		return 0;
	case CDM_SETDATETIME:
		DoGetSetDateTime(hWnd,(LPSYSTEMTIME)lParam,TRUE);
		return 0;
	case WM_DESTROY:
		DoDestroy(hWnd);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
