/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：时间编辑类
版本号：1.0.0
开发时期：2002-06-10
作者：李林
修改记录：
    2003-04-17:将DATETIME 结构改为SYSTEMTIME
******************************************************/

#include <ewindows.h>
#include <edateset.h>
#include <ecomdate.h>
#include <esymbols.h>
#include <eComCtrl.h>
#include <etimefmt.h>

// Delete By Jami chen in 2003.08.05
/*
static const BYTE bmpDropDown[] = 
{
    0XFF,0,
    0X01,0,
    0X83,0,
    0XC7,0,
    0XEF,0,
    0XFF,0
};
static const BYTE bmpDropUp[] = 
{
    0XFF,0,
    0XEF,0,
    0XC7,0,
    0X83,0,
    0X01,0,
    0XFF,0
};
*/
// Delete End By Jami chen in 2003.08.05
typedef struct _EDITTIME
{
	SYSTEMTIME				dt;
	int						iIndex;
//	int						iButton;
	//DWORD					style;

// !!! Add By Jami chen in 2003.08.05
	COLORREF    clTextColor;
	COLORREF    clBackGround;

	COLORREF    clSelColor;
	COLORREF    clSelBackGround;

	COLORREF    clDisableColor;
	COLORREF    clDisableBackGround;
// !!! Add End By Jami chen in 2003.08.05
// !!! Add By Jami chen in 2003.09.09
	UINT fModified ;
	UINT fTimer;	
// !!! Add End By Jami chen in 2003.09.09
// !!! Add By Jami chen in 2003.09.09
	DWORD iTimeFormat;
// !!! Add End By Jami chen in 2003.09.09
}EDITTIME, * LPEDITTIME;

#define IDC_UPDOWN  203

#define	CHAR_WIDTH					9
#define ARROWWIDTH					16
#define TIMEWIDTH					18
#define SPECWIDTH					9
#define DATAOFFSET                  0

#define SEL_HOUR		0
#define SEL_MINUTE		1
#define SEL_SECOND		2
#define SEL_TIMEFORMAT	3

static const TCHAR	classEDITTIME[]="__EDIT_TIME__";

static LRESULT CALLBACK EditTimeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// !!! Add By Jami chen In 2003.08.01
static LRESULT DoETNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DrawEditTime( HWND hWnd, HDC hdc );
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
// !!! Add End By Jami chen In 2003.08.01

// !!! Add By Jami chen in 2003.09.09
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam);
// !!! Add End By Jami chen in 2003.09.09

static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoTimeFmtChange(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void AutoAdjustSize(HWND hWnd);

ATOM RegisterEditTimeClass(HINSTANCE hInstance)
{
	WNDCLASS wcex;
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)EditTimeProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classEDITTIME;
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

// !!! Delete By Jami chen in 2003.08.05
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
// !!! Delete End By Jami chen in 2003.08.05

static BOOL OnCreateEditTime(HWND hWnd)
{
	LPEDITTIME				lpet;
	HINSTANCE hInstance;
	HWND hUpDown;
	RECT rectClient;
    BOOL bEnable;
	DWORD dwStyle;


	lpet=(LPEDITTIME)malloc(sizeof(EDITTIME));
	if(lpet==NULL)
		return FALSE;

	dwStyle = GetWindowLong(hWnd,GWL_STYLE);
	memset( lpet, 0, sizeof(EDITTIME) );
	GetDateTime(&(lpet->dt));
	lpet->iIndex = -1;
//	lpet->iButton = -1;

// !!! Add By Jami chen in 2003.08.05
	lpet->clTextColor = GetSysColor(COLOR_WINDOWTEXT);
	lpet->clBackGround = GetSysColor(COLOR_WINDOW);

	lpet->clSelColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
	lpet->clSelBackGround = GetSysColor(COLOR_HIGHLIGHT);
// !!! Add End By Jami chen in 2003.08.05

// !!! Add By Jami chen in 2003.08.30
	lpet->clDisableColor = GetSysColor(COLOR_GRAYTEXT);
	lpet->clDisableBackGround = GetSysColor(COLOR_WINDOW);
// !!! Add End By Jami chen in 2003.08.30

// !!! Add By Jami chen in 2003.09.09
	lpet->fModified = FALSE;
	lpet->fTimer = FALSE;	
// !!! Add End By Jami chen in 2003.09.09

	SetWindowLong(hWnd,DATAOFFSET,(long)lpet);

// !!! Add By Jami chen In 2003.08.01
	GetClientRect(hWnd, &rectClient);
	InitCommonControls();
	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
	if (dwStyle & ET_HORZUDS)
		hUpDown = CreateWindow(classUPDOWN32,  "",WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_HORZSHOW,rectClient.right - ARROWWIDTH,0,ARROWWIDTH,rectClient.bottom,hWnd,(HMENU)IDC_UPDOWN,hInstance,0);
	else
		hUpDown = CreateWindow(classUPDOWN32,  "",WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT ,rectClient.right - ARROWWIDTH,0,ARROWWIDTH,rectClient.bottom,hWnd,(HMENU)IDC_UPDOWN,hInstance,0);
	SendMessage(hUpDown,UDM_SETRANGE32,(WPARAM)0x80000000,(LPARAM)0x7fffffff);
	SendMessage(hUpDown,UDM_SETPOS,0,0);
// !!! Add End By Jami chen In 2003.08.01

// !!! Add By Jami chen in 2003 . 08.30
//  if the Control is disable , then the child control must be disable
	bEnable = (dwStyle & WS_DISABLED) ? FALSE : TRUE; 
	EnableWindow(hUpDown,bEnable);
// !!! Add End By Jami chen in 2003 . 08.30

// !!! Add By Jami chen in 2003.09.09
	if(dwStyle&ET_RUNCLOCK)
	{
		SetTimer(hWnd,1,1000,NULL);
		lpet->fTimer = TRUE;	
	}
// !!! Add End By Jami chen in 2003.09.09

// !!! Add By Jami chen in 2004.07.15
	lpet->iTimeFormat = GetSysTimeFormat();
	if (dwStyle & ET_AUTOSIZE)
	{
		// 需要自动调整窗口大小
		AutoAdjustSize(hWnd);
	}
// !!! Add End By Jami chen in 2004.07.15
	return TRUE;
}
// !!! delete By Jami chen in 2003.08.05
/*
static void DrawEditTime( HWND hWnd, HDC hdc )
{
	LPEDITTIME				lpet;
	RECT					rt;
	TCHAR					szTemp[15];
//	HBITMAP					hBmp,hBmpUp;
//	HFONT                   hFont;

	lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
	ASSERT(lpet!=NULL);
	if(lpet->dt.wHour<10)
		sprintf(szTemp,"0%1d",lpet->dt.wHour);
	else
		sprintf(szTemp,"%2d",lpet->dt.wHour);
	if(lpet->dt.wMinute<10)
		sprintf(&(szTemp[2]),":0%1d",lpet->dt.wMinute);
	else
		sprintf(&(szTemp[2]),":%2d",lpet->dt.wMinute);
	if( !(ET_NOSECOND&GetWindowLong( hWnd, GWL_STYLE ) ) )
	{
// !!! Modified By jami chen in 2003.08.05
//		if(lpet->dt.wSecond<10)
//			sprintf(&(szTemp[5]),":0%1d %s",lpet->dt.wSecond,lpet->dt.wHour>=12?"PM":"AM");
//		else
//			sprintf(&(szTemp[5]),":%2d %s",lpet->dt.wSecond,lpet->dt.wHour>=12?"PM":"AM");
		sprintf(&(szTemp[5]),":%02d",lpet->dt.wSecond);
// !!! Modified End By Jami chen in 2003.08.05
	}

	GetClientRect(hWnd,&rt);
	DrawText(hdc,szTemp,strlen(szTemp),&rt,DT_VCENTER|DT_SINGLELINE|DT_LEFT);
// !!! Delete By Jami chen In 2003.08.01
/ *
    hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL16X16) );//
	SetBkColor( hdc, GetSysColor(COLOR_BTNFACE) );
	SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );

	rt.top=(rt.bottom-rt.top-16)/2;
	rt.bottom=rt.top+16;
	rt.left=rt.right-15;
	//hBmp=CreateBitmap(8,6,1,1,bmpDropDown);
	//DrawBitMap(hdc,hBmp,rt);
	//DeleteObject(hBmp);
	szTemp[0] = SYM_DOWNARROW;
	DrawText( hdc, szTemp, 1, &rt, DT_CENTER|DT_VCENTER|DT_SINGLELINE );	


	rt.right=rt.left;
	rt.left-=15;
	//hBmpUp=CreateBitmap(8,6,1,1,bmpDropUp);
	//DrawBitMap(hdc,hBmpUp,rt);
	//DeleteObject(hBmpUp);
	szTemp[0] = SYM_UPARROW;
	DrawText( hdc, szTemp, 1, &rt, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

	SelectObject( hdc, hFont );
	* /
// !!! Delete End By Jami chen In 2003.08.01

	GetClientRect(hWnd,&rt);
	if((lpet->iIndex >= 0)&&(lpet->iIndex <= 2)){
		rt.left+=(CHAR_WIDTH*3)*lpet->iIndex-4*lpet->iIndex;
		rt.right=rt.left+(CHAR_WIDTH*2);
		InvertRect(hdc,&rt);
	}
// !!! Delete By Jami chen In 2003.08.01
/ *
	GetClientRect(hWnd,&rt);
	if(lpet->iButton==0){
		rt.left=rt.right-15;
		InvertRect(hdc,&rt);
	}else if(lpet->iButton==1){
		rt.left=rt.right-30;
		rt.right-=15;
		InvertRect(hdc,&rt);
	}
	* /
// !!! Delete End By Jami chen In 2003.08.01
}
*/
// !!! delete End By Jami chen in 2003.08.05

static void RefreshEditTime( HWND hWnd )
{
	HDC hdc = GetDC( hWnd );
	RECT rt;

	GetClientRect( hWnd, &rt );
//	FillRect( hdc, &rt, GetStockObject(WHITE_BRUSH) );
	DrawEditTime( hWnd, hdc );
	ReleaseDC( hWnd, hdc );
}

static void DoPaintEditTime( HWND hWnd )
{
	PAINTSTRUCT				ps;
	HDC hdc;

	hdc = BeginPaint(hWnd, &ps);

	DrawEditTime( hWnd, hdc );

	EndPaint(hWnd, &ps);
}

static void DoETLButtonDown(HWND hWnd,WORD xPos,WORD yPos){
	LPEDITTIME				lpet;
	RECT					rt;
	POINT					pt;
	DWORD                   dwStyle;
	pt.x=xPos;
	pt.y=yPos;
	lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
	ASSERT(lpet!=NULL);

	if (GetFocus() != hWnd)
	{
		SetFocus(hWnd);
	}
	GetClientRect(hWnd,&rt);
	if(PtInRect(&rt,pt)==FALSE)
		return ;
	SetCapture(hWnd);

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );

// !!! delete By Jami chen in 2003.09.09
//	if(dwStyle&ET_RUNCLOCK)
//		KillTimer(hWnd,1);
// !!! delete End By Jami chen in 2003.09.09

// !!! delete By Jami chen in 2003.08.01
/*
	if(xPos > (rt.right-15)){
		lpet->iButton=0;
		if(lpet->iIndex==0){
			if( lpet->dt.wHour == 0 ){
				DateAddDay( &(lpet->dt), -1 );
				lpet->dt.wHour = 23;
			}else
				lpet->dt.wHour -- ;
		}else if(lpet->iIndex==1){
			if( lpet->dt.wMinute == 0 ){
				DateAddHour( &(lpet->dt) , -1 );
				lpet->dt.wMinute = 59 ;
			}else
				lpet->dt.wMinute -- ;
		}else if(lpet->iIndex==2){
			if( lpet->dt.wSecond == 0 ){
				DateAddMin( &(lpet->dt) , -1 );
				lpet->dt.wSecond = 59 ;
			}else
				lpet->dt.wSecond -- ;
		}
		SendNotifyMessage( hWnd, &lpet->dt );
		//InvalidateRect(hWnd,&rt,TRUE);
//		RefreshEditTime( hWnd );
//		return ;
	}
	else if(xPos > (rt.right-30) ){
		lpet->iButton=1;
		if(lpet->iIndex==0){
			if( lpet->dt.wHour == 23 ){
				DateAddDay( &(lpet->dt), 1 );
				lpet->dt.wHour = 0;
			}else
				lpet->dt.wHour ++ ;
		}else if(lpet->iIndex==1){
			if( lpet->dt.wMinute == 59 ){
				DateAddHour( &(lpet->dt) , 1 );
				lpet->dt.wMinute = 0 ;
			}else
				lpet->dt.wMinute ++ ;
		}else if(lpet->iIndex==2){
			if( lpet->dt.wSecond == 59 ){
				DateAddMin( &(lpet->dt) , 1 );
				lpet->dt.wSecond = 0 ;
			}else
				lpet->dt.wSecond ++ ;
		}
		SendNotifyMessage( hWnd, &lpet->dt );
//		InvalidateRect(hWnd,&rt,TRUE);
//		RefreshEditTime( hWnd );
//		return ;
	}
	else{
*/
// !!! delete By Jami chen in 2003.08.01
	{
		int iPos;

		iPos = rt.left+TIMEWIDTH + SPECWIDTH; // The Hour Pos
//		lpet->iButton=-1;
//		if(xPos < (rt.left+CHAR_WIDTH *2) ){
		if( xPos < iPos )
		{
			lpet->iIndex=SEL_HOUR;
		}
		else
		{

			iPos += TIMEWIDTH + SPECWIDTH; // The Munute Pos
	//		if((xPos> (rt.left+CHAR_WIDTH*3-4))&&(xPos <(rt.left + CHAR_WIDTH *5))){
			if( xPos < iPos)
			{
				lpet->iIndex=SEL_MINUTE;
			}
			else
			{   // second
				if( dwStyle & ET_NOSECOND ) 
				{ // No Second
					// !!! Add By Jami chen in 2004.07.15
					if (lpet->iTimeFormat & TIMEFMT_12HOUR)
					{
						// 当前显示的是12小时制，所以当前位置的时间格式区
						lpet->iIndex = SEL_TIMEFORMAT;
					}
					else
					// !!! Add End By Jami chen in 2004.07.15
						lpet->iIndex = SEL_MINUTE;
				}
				else
				{  // 要显示秒
					// !!! Add By Jami chen in 2004.07.15
					if (lpet->iTimeFormat & TIMEFMT_12HOUR)
					{  
						// 当前显示的是12小时制
						iPos += TIMEWIDTH; // The Second Pos
						if( xPos < iPos)
						{  // 当前位置是秒
							lpet->iIndex=SEL_SECOND;
						}
						else
							lpet->iIndex = SEL_TIMEFORMAT; // 当前位置是时间格式
					}
					else
					// !!! Add End By Jami chen in 2004.07.15
						lpet->iIndex=SEL_SECOND;
				}
			}
		}
	}
	//InvalidateRect(hWnd,&rt,TRUE);
	RefreshEditTime( hWnd );
}

static void DoETButtonUp(HWND hWnd,WORD xPos,WORD yPos){
//	RECT					rt;
//	POINT					pt;
	LPEDITTIME				lpet;
	DWORD                   dwStyle = GetWindowLong( hWnd, GWL_STYLE );

	lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
	ASSERT(lpet!=NULL);
// !!! Delete By Jami chen in 2003.08.05
/*
	if(lpet->iButton>-1)
		lpet->iButton=-1;
	pt.x=xPos;
	pt.y=yPos;
	GetClientRect(hWnd,&rt);
	if(PtInRect(&rt,pt)==FALSE){
*/
// !!! Delete By Jami chen in 2003.08.05
//		lpet->iButton=-1;
//		lpet->iIndex=-1;
		ReleaseCapture();
// !!! delete By Jami chen in 2003.09.09
//		if(dwStyle&ET_RUNCLOCK)
//		{
//			SetTimer(hWnd,1,1000,NULL);
//		}
// !!! delete end By Jami chen in 2003.09.09
//!!! Delete 	}
	//InvalidateRect(hWnd,&rt,TRUE);
	RefreshEditTime( hWnd );
}
static void DoGetTime(HWND hWnd,LPSYSTEMTIME lpdt,BOOL isSet){
	LPEDITTIME				lpet;
	lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
	ASSERT(lpet!=NULL);
	if(isSet==TRUE){
//		lpet->dt = *lpdt ;
		lpet->dt.wHour = lpdt->wHour;
		lpet->dt.wMinute = lpdt->wMinute;
		lpet->dt.wSecond = lpdt->wSecond;
		//InvalidateRect( hWnd, NULL, FALSE );
		RefreshEditTime( hWnd );
	}else{
//		*lpdt = lpet->dt ;
		lpdt->wHour = lpet->dt.wHour;
		lpdt->wMinute = lpet->dt.wMinute;
		lpdt->wSecond = lpet->dt.wSecond;
	}
}

static void DoETDestroy(HWND hWnd){
	LPEDITTIME				lpet;
	lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
	ASSERT(lpet!=NULL);
	free(lpet);
}

static void DoETTimer(HWND hWnd){
	LPEDITTIME				lpet;
//	SYSTEMTIME				dt;
	lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
	ASSERT(lpet!=NULL);
// !!! Delete By Jami chen in 2003.08.05
//	GetDateTime(&dt);
//	lpet->dt.wSecond=dt.wSecond;
// !!! Delete End By Jami chen in 2003.08.05
// !!! Modified By Jami chen in 2003.09.11
/*
	lpet->dt.wSecond++;
	if(lpet->dt.wSecond==60){
		lpet->dt.wSecond=0;
		lpet->dt.wMinute++;
		SendNotifyMessage( hWnd, &lpet->dt );
	}
	if(lpet->dt.wMinute==60){
		lpet->dt.wMinute=0;
		lpet->dt.wHour++;
	}
	if(lpet->dt.wHour==24)
	{
		lpet->dt.wHour=0;
		DateAddDay( &(lpet->dt), 1 );
	}
*/
	// if the Timer is Valid , then the Time Must same with system time
	GetDateTime(&(lpet->dt));
// !!! Modified By Jami chen in 2003.09.11
	//InvalidateRect(hWnd,NULL,TRUE);
	RefreshEditTime( hWnd );
}

static LRESULT CALLBACK EditTimeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD dwStyle;

	switch (message)
	{
	case WM_PAINT:
		DoPaintEditTime(hWnd);
		return 0;
	case WM_TIMER:
		DoETTimer(hWnd);
		return 0;
   	case WM_LBUTTONDOWN:
		DoETLButtonDown(hWnd,LOWORD(lParam),HIWORD(lParam));
		return 0;
	case WM_LBUTTONUP:
		DoETButtonUp(hWnd,LOWORD(lParam),HIWORD(lParam));
		return 0;
// !!! Add By Jami chen In 2003.08.01
	case WM_KILLFOCUS:
		DoKillFocus(hWnd,wParam,lParam);
		return 0;
	case WM_NOTIFY:
		DoETNotify(hWnd,wParam,lParam);
		return 0;
	case WM_SETCTLCOLOR	:
		return DoSetColor(hWnd,wParam,lParam);
	case WM_GETCTLCOLOR	:
		return DoGetColor(hWnd,wParam,lParam);
// !!! Add By Jami chen In 2003.08.01

	case ETM_GETTIME:
		DoGetTime(hWnd,(LPSYSTEMTIME)lParam,FALSE);
		return 0;
	case ETM_SETTIME:
		DoGetTime(hWnd,(LPSYSTEMTIME)lParam,TRUE);
		return 0;
// !!! Add By Jami chen in 2003.09.09
	case ETM_SETMODIFY:
		DoSetModify(hWnd,wParam,lParam);
		return 0;
	case ETM_GETMODIFY:
		return DoGetModify(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2003.09.09
// !!! Add By Jami chen in 2004.07.15
	case ETM_TIMEFMTCHG:  // 系统时间格式发生改变
		return DoTimeFmtChange(hWnd,wParam,lParam);
// !!! Add By Jami chen in 2004.07.15
	case WM_CREATE:
		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		if(OnCreateEditTime(hWnd)==FALSE)
			return -1;
// !!! delete By Jami chen in 2003.09.09
//		if(GetWindowLong(hWnd,GWL_STYLE)&ET_RUNCLOCK)
//			SetTimer(hWnd,1,1000,NULL);
// !!! delete End By Jami chen in 2003.09.09
		return 0;
	case WM_DESTROY:
		if(GetWindowLong(hWnd,GWL_STYLE)&ET_RUNCLOCK)
			KillTimer(hWnd,1);
		DoETDestroy(hWnd);
		return 0;

// !!! Add By Jami chen in 2004.07.19
		case WM_SYSCOLORCHANGE:
			return DoSysColorChanged(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2004.07.19
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}


// !!! Add By Jami chen In 2003.08.01
static LRESULT DoETNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMUPDOWN lpnmUpDown;
	LPEDITTIME				lpet;

		lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
		ASSERT(lpet!=NULL);

		lpnmUpDown = (LPNMUPDOWN)lParam;
		if (lpnmUpDown == NULL)
		{
			return 0;
		}
		switch(lpnmUpDown->hdr.code)
		{
			case UDN_DELTAPOS:
// !!! modified By Jami chen in 2003 08 29
//				if (lpnmUpDown->iDelta > 0)
				if (lpnmUpDown->iDelta < 0)  // if Up Arrow then Time Add
// !!! modified By Jami chen in 2003 08 29
				{
					if(lpet->iIndex==SEL_HOUR){
						if( lpet->dt.wHour == 0 ){
							DateAddDay( &(lpet->dt), -1 );
							lpet->dt.wHour = 23;
						}else
							lpet->dt.wHour -- ;
// !!! Add By Jami chen in 2003.09.09
						lpet->fModified = TRUE;
// !!! Add End By Jami chen in 2003.09.09
					}else if(lpet->iIndex==SEL_MINUTE){
						if( lpet->dt.wMinute == 0 ){
							DateAddHour( &(lpet->dt) , -1 );
							lpet->dt.wMinute = 59 ;
						}else
							lpet->dt.wMinute -- ;
// !!! Add By Jami chen in 2003.09.09
						lpet->fModified = TRUE;
// !!! Add End By Jami chen in 2003.09.09
					}else if(lpet->iIndex==SEL_SECOND){
						if( lpet->dt.wSecond == 0 ){
							DateAddMin( &(lpet->dt) , -1 );
							lpet->dt.wSecond = 59 ;
						}else
							lpet->dt.wSecond -- ;
// !!! Add By Jami chen in 2003.09.09
						lpet->fModified = TRUE;
// !!! Add End By Jami chen in 2003.09.09
					}
// !!! Add By Jami chen in 2004.07.15
					else if(lpet->iIndex==SEL_TIMEFORMAT){
						if( lpet->dt.wHour >=12 ){
							lpet->dt.wHour -= 12 ;
						}else
							lpet->dt.wHour += 12 ;
						lpet->fModified = TRUE;
					}
// !!! Add By Jami chen in 2004.07.15
					RefreshEditTime( hWnd );
					SendNotifyMessage( hWnd, &lpet->dt );
				}
				else
				{
					if(lpet->iIndex==SEL_HOUR){
						if( lpet->dt.wHour == 23 ){
							DateAddDay( &(lpet->dt), 1 );
							lpet->dt.wHour = 0;
						}else
							lpet->dt.wHour ++ ;
// !!! Add By Jami chen in 2003.09.09
						lpet->fModified = TRUE;
// !!! Add End By Jami chen in 2003.09.09
					}else if(lpet->iIndex==SEL_MINUTE){
						if( lpet->dt.wMinute == 59 ){
							DateAddHour( &(lpet->dt) , 1 );
							lpet->dt.wMinute = 0 ;
						}else
							lpet->dt.wMinute ++ ;
// !!! Add By Jami chen in 2003.09.09
						lpet->fModified = TRUE;
// !!! Add End By Jami chen in 2003.09.09
					}else if(lpet->iIndex==SEL_SECOND){
						if( lpet->dt.wSecond == 59 ){
							DateAddMin( &(lpet->dt) , 1 );
							lpet->dt.wSecond = 0 ;
						}else
							lpet->dt.wSecond ++ ;
// !!! Add By Jami chen in 2003.09.09
						lpet->fModified = TRUE;
// !!! Add End By Jami chen in 2003.09.09
					}
// !!! Add By Jami chen in 2004.07.15
					else if(lpet->iIndex==SEL_TIMEFORMAT){
						if( lpet->dt.wHour >=12 ){
							lpet->dt.wHour -= 12 ;
						}else
							lpet->dt.wHour += 12 ;
						lpet->fModified = TRUE;
					}
// !!! Add By Jami chen in 2004.07.15
					RefreshEditTime( hWnd );
					SendNotifyMessage( hWnd, &lpet->dt );
				}
// !!! Add By Jami chen in 2003.09.09
				if (lpet->fModified == TRUE)
				{
					if(lpet->fTimer)
					{
						KillTimer(hWnd,1);
						lpet->fTimer = FALSE;
					}
				}
// !!! Add End By Jami chen in 2003.09.09

				break;
			case NM_KILLFOCUS:
				DoKillFocus(hWnd,0,0);
				break;
			default:
				break;
		}
		return 0;
}
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPEDITTIME				lpet;
	HWND hwndFocus;
	HWND hUpDown;

	hUpDown = GetDlgItem(hWnd,IDC_UPDOWN);
	hwndFocus = (HWND)wParam;
	if (hwndFocus == hUpDown || hwndFocus == hWnd)
	{
		return 0;
	}

	lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
	ASSERT(lpet!=NULL);

	if (lpet->iIndex != -1)
	{
		lpet->iIndex = -1;
		RefreshEditTime(hWnd);
	}
	return 0;
}
// !!! Add End By Jami chen In 2003.08.01


// !!! Add By Jami chen In 2003.08.05
static void DrawEditTime( HWND hWnd, HDC hdc )
{
	LPEDITTIME				lpet;
	RECT					rt;
	TCHAR					szTemp[15];
	HBRUSH hBrush;
// !!! Add By Jami chen in 2003 . 08 30
	DWORD dwStyle;
// !!! Add End By Jami chen in 2003 . 08 30

	lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
	ASSERT(lpet!=NULL);

// !!! Add By Jami chen in 2003 . 08 30
	dwStyle = GetWindowLong(hWnd,GWL_STYLE);
// !!! Add End By Jami chen in 2003 . 08 30

	GetClientRect(hWnd,&rt);
	hBrush = CreateSolidBrush(lpet->clBackGround);
	FillRect( hdc, &rt, hBrush );
	DeleteObject(hBrush);

	SetBkMode( hdc, TRANSPARENT ) ;

	// Show Hour
// !!! Add By Jami chen in 2004.07.15
	if (lpet->iTimeFormat & TIMEFMT_12HOUR)
	{
		// 12小时制
		int iTime = lpet->dt.wHour%12;
		if (iTime == 0)
			iTime = 12;
		sprintf(szTemp,"%02d",iTime);
	}
	else
// !!! Add End By Jami chen in 2004.07.15
		sprintf(szTemp,"%02d",lpet->dt.wHour);

	rt.right = rt.left + TIMEWIDTH;
// !!! Add By Jami chen in 2003 . 08 30
	if (dwStyle & WS_DISABLED)
	{
			hBrush = CreateSolidBrush(lpet->clDisableBackGround);
			FillRect( hdc, &rt, hBrush );
			DeleteObject(hBrush);
			SetTextColor(hdc,lpet->clDisableColor);
	}
	else
	{
// !!! Add End By Jami chen in 2003 . 08 30
		if (lpet->iIndex == SEL_HOUR)
		{
			hBrush = CreateSolidBrush(lpet->clSelBackGround);
			FillRect( hdc, &rt, hBrush );
			DeleteObject(hBrush);

			SetTextColor(hdc,lpet->clSelColor);		
		}
		else
		{
			SetTextColor(hdc,lpet->clTextColor);
		}
	}
	DrawText(hdc,szTemp,strlen(szTemp),&rt,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	rt.left += TIMEWIDTH;

	// Show SEPARATE CODE ':'
	rt.right = rt.left + SPECWIDTH;
	SetTextColor(hdc,lpet->clTextColor);
	szTemp[0] = ':';
	szTemp[1] = 0;
	DrawText(hdc,szTemp,strlen(szTemp),&rt,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	rt.left += SPECWIDTH;

	// Show Minnte
	sprintf(szTemp,"%02d",lpet->dt.wMinute);

	rt.right = rt.left + TIMEWIDTH;
// !!! Add By Jami chen in 2003 . 08 30
	if (dwStyle & WS_DISABLED)
	{
			hBrush = CreateSolidBrush(lpet->clDisableBackGround);
			FillRect( hdc, &rt, hBrush );
			DeleteObject(hBrush);
			SetTextColor(hdc,lpet->clDisableColor);
	}
	else
	{
// !!! Add End By Jami chen in 2003 . 08 30
		if (lpet->iIndex == SEL_MINUTE)
		{
			hBrush = CreateSolidBrush(lpet->clSelBackGround);
			FillRect( hdc, &rt, hBrush );
			DeleteObject(hBrush);

			SetTextColor(hdc,lpet->clSelColor);		
		}
		else
		{
			SetTextColor(hdc,lpet->clTextColor);
		}
	}
	DrawText(hdc,szTemp,strlen(szTemp),&rt,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	rt.left += TIMEWIDTH;

	if( !(ET_NOSECOND&GetWindowLong( hWnd, GWL_STYLE ) ) )
	{
		// Show SEPARATE CODE ':'
		rt.right = rt.left + SPECWIDTH;
		SetTextColor(hdc,lpet->clTextColor);
		szTemp[0] = ':';
		szTemp[1] = 0;
		DrawText(hdc,szTemp,strlen(szTemp),&rt,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
		rt.left += SPECWIDTH;

		// Show Second
		sprintf(szTemp,"%02d",lpet->dt.wSecond);

		rt.right = rt.left + TIMEWIDTH;
// !!! Add By Jami chen in 2003 . 08 30
		if (dwStyle & WS_DISABLED)
		{
				hBrush = CreateSolidBrush(lpet->clDisableBackGround);
				FillRect( hdc, &rt, hBrush );
				DeleteObject(hBrush);
				SetTextColor(hdc,lpet->clDisableColor);
		}
		else
		{
// !!! Add End By Jami chen in 2003 . 08 30
			if (lpet->iIndex == SEL_SECOND)
			{
				hBrush = CreateSolidBrush(lpet->clSelBackGround);
				FillRect( hdc, &rt, hBrush );
				DeleteObject(hBrush);

				SetTextColor(hdc,lpet->clSelColor);		
			}
			else
			{
				SetTextColor(hdc,lpet->clTextColor);
			}
		}
		DrawText(hdc,szTemp,strlen(szTemp),&rt,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	}

//	如果是12小时制，需要显示AM 或 PM
// !!! Add By Jami chen in 2004.07.15
	if (lpet->iTimeFormat & TIMEFMT_12HOUR)
	{
		// 12小时制, 需要显示 AM 或着 PM
		rt.left += TIMEWIDTH;
//		rt.left += SPECWIDTH; // 在时间与时间格式间有一个间隙
		rt.right = rt.left + TIMEWIDTH;
		if (dwStyle & WS_DISABLED)
		{
				hBrush = CreateSolidBrush(lpet->clDisableBackGround);
				FillRect( hdc, &rt, hBrush );
				DeleteObject(hBrush);
				SetTextColor(hdc,lpet->clDisableColor);
		}
		else
		{
			if (lpet->iIndex == SEL_TIMEFORMAT)
			{
				hBrush = CreateSolidBrush(lpet->clSelBackGround);
				FillRect( hdc, &rt, hBrush );
				DeleteObject(hBrush);

				SetTextColor(hdc,lpet->clSelColor);		
			}
			else
			{
				SetTextColor(hdc,lpet->clTextColor);
			}
		}
		if (lpet->dt.wHour < 12 )
			strcpy(szTemp,"AM");
		else
			strcpy(szTemp,"PM");
		DrawText(hdc,szTemp,strlen(szTemp),&rt,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	}
// !!! Add End By Jami chen in 2004.07.15

}
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
		
	LPEDITTIME				lpet;

		lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
		ASSERT(lpet!=NULL);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {
			 lpet->clTextColor = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 {
			 lpet->clBackGround = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 {
			 lpet->clSelColor = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {
			 lpet->clSelBackGround = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {
				lpet->clDisableColor = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {
				lpet->clDisableBackGround = lpCtlColor->cl_DisableBk;
		 }
		 InvalidateRect(hWnd,NULL,TRUE);
		 return TRUE;
}
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPEDITTIME				lpet;

		lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
		ASSERT(lpet!=NULL);
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {
			 lpCtlColor->cl_Text = lpet->clTextColor ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 {
				lpCtlColor->cl_TextBk = lpet->clBackGround ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 {
				lpCtlColor->cl_Selection = lpet->clSelColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {
				lpCtlColor->cl_SelectionBk = lpet->clSelBackGround;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {
			lpCtlColor->cl_Disable= lpet->clDisableColor ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {
			lpCtlColor->cl_DisableBk = lpet->clDisableBackGround ;
		 }
		return TRUE;
}

// !!! Add End By Jami chen In 2003.08.05

// !!! Add  By Jami chen In 2003.08.30
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  BOOL bEnable;
  DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);
  HWND hUpDown;

		hUpDown = GetDlgItem(hWnd,IDC_UPDOWN);
		bEnable = (dwStyle & WS_DISABLED) ? FALSE : TRUE; 
		EnableWindow(hUpDown,bEnable);
		return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam);
}
// !!! Add  End By Jami chen In 2003.08.30

// !!! Add By Jami chen in 2003.09.09
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT fModified;
	LPEDITTIME				lpet;

		lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
		ASSERT(lpet!=NULL);

		fModified = (UINT)wParam;
		lpet->fModified = fModified;

		return 0;
}
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPEDITTIME				lpet;

		lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
		ASSERT(lpet!=NULL);

		return lpet->fModified;
}
// !!! Add End By Jami chen in 2003.09.09

// !!! Add By Jami chen in 2004.07.15
// **************************************************
// 声明：static LRESULT DoTimeFmtChange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 参数消息
// 	IN lParam -- 参数消息
// 
// 返回值：返回消息处理结果
// 功能描述：处理系统时间格式发生改变的消息。
// 引用: 
// **************************************************
static LRESULT DoTimeFmtChange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPEDITTIME				lpet;
	DWORD dwStyle;

		lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
		ASSERT(lpet!=NULL);

		lpet->iTimeFormat = GetSysTimeFormat(); // 重新得到时间格式

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		if (dwStyle & ET_AUTOSIZE)
		{
			// 需要自动调整窗口大小
			AutoAdjustSize(hWnd);
		}

		RefreshEditTime( hWnd ); // 重新显示时间

		return 0;
}

// **************************************************
// 声明：static void AutoAdjustSize(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：自动调整窗口大小。
// 引用: 
// **************************************************
static void AutoAdjustSize(HWND hWnd)
{
	RECT rectWindow,rectClient;
	LPEDITTIME				lpet;
	int iWidth,iHeight;
	TEXTMETRIC TextMetric;
	int nTextHeight;
	DWORD dwStyle;
	HDC hdc;
	HWND hUpDown;



		lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
		ASSERT(lpet!=NULL);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);

		GetWindowRect(hWnd,&rectWindow); // 得到窗口大小
		GetClientRect(hWnd,&rectClient); // 得到窗口客户区大小
		
		iWidth = (rectWindow.right - rectWindow.left) - rectClient.right; // 得到边框的宽度
		iHeight = (rectWindow.bottom - rectWindow.top) - rectClient.bottom; // 得到边框的高度

		hdc=GetDC(hWnd);
		// get current text property
		GetTextMetrics(hdc,&TextMetric);  
	    ReleaseDC(hWnd,hdc);
	    nTextHeight=(int)TextMetric.tmHeight;

		iHeight += nTextHeight; //得到窗口的高度

//					时          :			分         				
		iWidth += TIMEWIDTH + SPECWIDTH + TIMEWIDTH ;

		if (!(dwStyle & ET_NOSECOND))
		{
			// 有秒		 
			//           :			秒
			iWidth += SPECWIDTH + TIMEWIDTH ;
		}

		if (lpet->iTimeFormat & TIMEFMT_12HOUR)
		{
			// 是12小时制
			iWidth += TIMEWIDTH ;
		}
			
		iWidth += ARROWWIDTH ;

		SetWindowPos(hWnd,NULL,0,0,iWidth,iHeight,SWP_NOMOVE | SWP_NOZORDER);

		GetClientRect(hWnd,&rectClient); // 得到窗口客户区大小
		hUpDown = GetDlgItem(hWnd,IDC_UPDOWN);
		// 调整UPDOWN控件的位置
		SetWindowPos(hUpDown,NULL,rectClient.right - ARROWWIDTH,0,ARROWWIDTH,rectClient.bottom,SWP_NOZORDER);

}
// !!! Add By Jami chen in 2004.07.15

// !!! Add By Jami chen in 2004.07.19
// **************************************************
// 声明：static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值： 无
// 功能描述：系统颜色改变，处理WM_SYSCOLORHANGED消息。
// 引用: 
// **************************************************
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPEDITTIME				lpet;

		lpet=(LPEDITTIME)GetWindowLong(hWnd,DATAOFFSET);
		ASSERT(lpet!=NULL);


		lpet->clTextColor = GetSysColor(COLOR_WINDOWTEXT);
		lpet->clBackGround = GetSysColor(COLOR_WINDOW);

		lpet->clSelColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		lpet->clSelBackGround = GetSysColor(COLOR_HIGHLIGHT);
		lpet->clDisableColor = GetSysColor(COLOR_GRAYTEXT);
		lpet->clDisableBackGround = GetSysColor(COLOR_WINDOW);
	
		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
// !!! Add End By Jami chen in 2004.07.19
