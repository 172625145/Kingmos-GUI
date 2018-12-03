#include <Ewindows.h>
#include <Edef.h>

#include "eTrackbar.h"

#define HUMB_HIGH							7
#define HUMB_WIDTH							8
#define CHANNEL_HIGH						10
#define SET_RANGE_ALL						1
#define SET_RANGE_MAX						2
#define SET_RANGE_MIN						3

typedef struct _tag_Trackbar_Data{
	int			iCurrent;
	int			iInc;
	int			iPageSize;
	int			iLow;
	int			iHigh;
	BOOL		fTic;
	BOOL		fTics;
}_TRACKBARDATA,*_LPTRACKBARDATA;
static const BYTE bmpHumb[] = {
    0X07,0X07,0X07,0X04,0X04,0X07,0X07,0X07,
    0X07,0X07,0X04,0X04,0X04,0X04,0X07,0X07,
    0XFF,0X04,0X04,0X04,0X04,0X04,0X04,0XFF,
    0X04,0X04,0X04,0X04,0X04,0X04,0X04,0X04,
    0X04,0X04,0X04,0X04,0X04,0X04,0X04,0X04,
    0X04,0X04,0X04,0X04,0X04,0X04,0X04,0X04,
    0X04,0X04,0X04,0X04,0X04,0X04,0X04,0X04,
   };
const char classTRACKBAR[]="mglctls_trackbar";
//=======================================================================
static BOOL GetChannelRect(HWND hWnd,RECT *rt){
	RECT				prt;
	GetClientRect(hWnd,&prt);
	memcpy(rt,&prt,sizeof(RECT));
	rt->top=prt.top;
	rt->bottom=rt->top+CHANNEL_HIGH;
	return TRUE;
}
static BOOL GetHumbRect(HWND hWnd,RECT* rt,_LPTRACKBARDATA lptd){
	RECT				prt;
	GetChannelRect(hWnd,rt);
	GetClientRect(hWnd,&prt);
	rt->left=rt->left+(rt->right-rt->left)*lptd->iCurrent/(lptd->iHigh-lptd->iLow);
	if(rt->left<prt.left)
		rt->left=prt.left;
	else if((rt->left+HUMB_WIDTH)>prt.right)
		rt->left=prt.right-HUMB_WIDTH;
	rt->right=rt->left+HUMB_WIDTH;
	rt->top=rt->bottom-2;
	rt->bottom=rt->top+HUMB_HIGH+2;
	return TRUE;
}
static BOOL GetTicRect(HWND hWnd,RECT *rt){
	return TRUE;
}
static void DoPaintChannel(HWND hWnd,HDC hdc,_LPTRACKBARDATA lptd){
	RECT				rt;
	GetChannelRect(hWnd,&rt);
//	DrawEdge(hdc,&rt,BDR_SUNKENINNER,BF_RECT|BF_MIDDLE);
//	rt.top+=2;
	FillRect(hdc,&rt,GetStockObject(LTGRAY_BRUSH));
	MoveTo(hdc,rt.left,rt.bottom);
	LineTo(hdc,rt.right,rt.bottom);
}
static void DoPaintTic(HWND hWnd,HDC hdc,_LPTRACKBARDATA lptd){
}
	
static void DoPaintText(HWND hWnd,HDC hdc,_LPTRACKBARDATA lptd){
	RECT				rt;
	TCHAR				szText[10];
	GetClientRect(hWnd,&rt);
	rt.left=(rt.right-40)/2;
	rt.right=rt.left+40;
	sprintf(szText,"%2d%s",lptd->iCurrent*100/(lptd->iHigh-lptd->iLow),"%");
	SetBkMode(hdc,TRANSPARENT);
	DrawText(hdc,szText,strlen(szText),&rt,DT_CENTER);
	SetBkMode(hdc,OPAQUE);
}
static void DrawBitMap(HDC hdc,HBITMAP hBitMap,RECT rect){
	HDC hMemoryDC;
	if(hBitMap==0)
		return;
	hMemoryDC=CreateCompatibleDC(hdc);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	BitBlt( hdc,(short)rect.left,(short)rect.top,(short)(rect.right-rect.left),
		(short)(rect.bottom-rect.top), hMemoryDC,0,0,
				SRCCOPY);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	DeleteDC(hMemoryDC);
}
static void DoPaintHumb(HWND hWnd,HDC hdc,_LPTRACKBARDATA lptd){
	RECT				rt;
//	HBRUSH				hBrush;
	HBITMAP				hBmp;
	GetHumbRect(hWnd,&rt,lptd);
	hBmp=CreateBitmap(8,7,1,8,bmpHumb);
	DrawBitMap(hdc,hBmp,rt);
	DeleteObject(hBmp);
//	hBrush=CreateSolidBrush(CL_DARKBLUE);
//	FillRect(hdc,&rt,hBrush);
//	DeleteObject(hBrush);
}
static BOOL OnCreateTrackbar(HWND hWnd){
	_LPTRACKBARDATA					lptd;
	lptd=(_LPTRACKBARDATA)malloc(sizeof(_TRACKBARDATA));
	if(lptd==NULL)
		return FALSE;
	memset( lptd, 0, sizeof(_TRACKBARDATA) );
	lptd->iCurrent=0;
	lptd->iHigh=100;
	lptd->iInc=10;
	lptd->iLow=0;
	SetWindowLong(hWnd,GWL_USERDATA,(long)lptd);
	return TRUE;
}
static void OnPaintTrackbar(HWND hWnd){
	_LPTRACKBARDATA				lptd;
	HDC							hdc;
	PAINTSTRUCT					ps;
	hdc = BeginPaint( hWnd, &ps );

	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);

	DoPaintText(hWnd,hdc,lptd);
	DoPaintHumb(hWnd,hdc,lptd);
	
	EndPaint( hWnd, &ps );
	return ;
}
static void OnDestroyTrackbar(HWND hWnd){
	_LPTRACKBARDATA				lptd;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	free(lptd);
}
static int DoGetPos(HWND hWnd){
	_LPTRACKBARDATA				lptd;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	return lptd->iCurrent;
}
static int DoGetRangeMin(HWND hWnd){
	_LPTRACKBARDATA				lptd;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	return lptd->iLow;
}
static int DoGetRangeMax(HWND hWnd){
	_LPTRACKBARDATA				lptd;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	return lptd->iHigh;
}
static int DoGetTic(HWND hWnd,int iTic){
	_LPTRACKBARDATA				lptd;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	if((iTic<0)||(iTic>(lptd->iHigh-lptd->iLow)/lptd->iInc))
		return -1;
	return (iTic*lptd->iInc);
}
static BOOL DoSetTic(HWND hWnd,int iPosition){
	_LPTRACKBARDATA				lptd;
	RECT						rt;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	if((iPosition>lptd->iHigh)||(iPosition<lptd->iLow))
		return FALSE;
	lptd->iInc=iPosition;
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE);
	return TRUE;
}
static  int DoSetPos(HWND hWnd,BOOL fPosition,int iPosition){
	_LPTRACKBARDATA				lptd;
	RECT						rt;
	NMHDR						nmh;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	if(iPosition<lptd->iLow)
		lptd->iCurrent=lptd->iLow;
	else if(iPosition>lptd->iHigh)
		lptd->iCurrent=lptd->iHigh;
	else
		lptd->iCurrent=iPosition;
//	if(fPosition==TRUE){
		GetClientRect(hWnd,&rt);
		InvalidateRect(hWnd,&rt,TRUE);
//	}//=====================================================
	nmh.hwndFrom=hWnd;
	nmh.idFrom=GetWindowLong(hWnd,GWL_ID);
	nmh.code=NM_RELEASEDCAPTURE;
	SendMessage(GetParent(hWnd),WM_NOTIFY,GetWindowLong(hWnd,GWL_ID),(LPARAM)&nmh);
	return lptd->iCurrent;
}
static void DoSetSel(HWND hWnd,BOOL fRedraw,int iMin,int iMax){
	_LPTRACKBARDATA				lptd;
	RECT						rt;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	lptd->iLow=iMin;
	lptd->iHigh=iMax;
	if(fRedraw==TRUE){
		GetClientRect(hWnd,&rt);
		InvalidateRect(hWnd,&rt,TRUE);
	}
}
static void DoLButtonDown(HWND hWnd,short xPos,short yPos){
	_LPTRACKBARDATA				lptd;
	RECT						rt;
	int							iPos;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	GetClientRect(hWnd,&rt);
	iPos=rt.left+(rt.right-rt.left)*lptd->iCurrent/(lptd->iHigh-lptd->iLow);
	if(xPos>iPos){
		DoSetPos(hWnd,TRUE,lptd->iCurrent+lptd->iInc);
	}else if(xPos<iPos){
		DoSetPos(hWnd,TRUE,lptd->iCurrent-lptd->iInc);
	}
}
static void DoMouseMove(HWND hWnd,WPARAM wParam,short xPos,short yPos){
	_LPTRACKBARDATA				lptd;
	RECT						rt;
	WORD						iPos;
	if(wParam!=MK_LBUTTON)
		return ;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	if(xPos<=0){
		DoSetPos(hWnd,TRUE,0);
		return ;
	}
	GetClientRect(hWnd,&rt);
	iPos=(short)(xPos*(lptd->iHigh-lptd->iLow)/(rt.right-rt.left));
	DoSetPos(hWnd,TRUE,iPos);
}
static void DoSetRange(HWND hWnd,BOOL fRedraw,int nMin,int nMax,int iFild){
	_LPTRACKBARDATA				lptd;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	if(iFild==SET_RANGE_ALL){
		lptd->iHigh=nMax;
		lptd->iLow=nMin;
	}else if(iFild==SET_RANGE_MAX){
		lptd->iHigh=nMax;
	}else{
		lptd->iLow=nMin;
	}
	DoSetPos(hWnd,fRedraw,lptd->iCurrent);
}
static void DoClearTics(HWND hWnd,BOOL fRedraw){
	_LPTRACKBARDATA				lptd;
	RECT						rt;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);
	lptd->fTics=FALSE;
	if(fRedraw==TRUE){
		GetClientRect(hWnd,&rt);
		InvalidateRect(hWnd,&rt,TRUE);
	}
}
static void OnEraseBkGnd(HWND hWnd,HDC hdc){
	_LPTRACKBARDATA				lptd;
	RECT						rt;
	lptd=(_LPTRACKBARDATA)GetWindowLong(hWnd,GWL_USERDATA);
	ASSERT(lptd!=NULL);

	GetClientRect(hWnd,&rt);
	FillRect(hdc,&rt,GetStockObject(0));
	DoPaintChannel(hWnd,hdc,lptd);
	DoPaintTic(hWnd,hdc,lptd);
}
static LRESULT CALLBACK	TrackbarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

	switch(message){
	case WM_CREATE:
		if(OnCreateTrackbar(hWnd)==FALSE)
			return -1;
		return 0;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		DoLButtonDown(hWnd,LOWORD(lParam),HIWORD(lParam));
		return 0;
	case WM_MOUSEMOVE:
		DoMouseMove(hWnd,wParam,LOWORD(lParam),HIWORD(lParam));
		return 0;
	case WM_LBUTTONUP:
		ReleaseCapture();
//		DoLButtonUp(hWnd,LOWORD(lParam),HIWORD
		return 0;
	case TBM_GETPOS:
		return DoGetPos(hWnd);
	case TBM_GETRANGEMIN:
		return DoGetRangeMin(hWnd);
	case TBM_GETRANGEMAX:
		return DoGetRangeMax(hWnd);
	case TBM_GETTIC:
		return DoGetTic(hWnd,wParam);
	case TBM_SETTIC:
		return DoSetTic(hWnd,lParam);
	case TBM_SETPOS:
		return DoSetPos(hWnd,(BOOL)wParam,lParam);
	case TBM_SETRANGE:
		DoSetRange(hWnd,(BOOL)wParam,LOWORD(lParam),HIWORD(lParam),SET_RANGE_ALL);
		return 0;
	case TBM_SETRANGEMIN:
		DoSetRange(hWnd,(BOOL)wParam,0,lParam,SET_RANGE_MAX);
		return 0;
	case TBM_SETRANGEMAX:
		DoSetRange(hWnd,(BOOL)wParam,lParam,0,SET_RANGE_MIN);
		return 0;
	case TBM_CLEARTICS:
		DoClearTics(hWnd,(BOOL)wParam);
		return 0;
	case TBM_SETSEL:
		DoSetSel(hWnd,(BOOL)wParam,LOWORD(lParam),HIWORD(lParam));
		return 0;
//	case TBM_SETSELSTART:
//	case TBM_SETSELEND:
	case TBM_GETPTICS:
//	case TBM_GETTICPOS:
	case TBM_GETNUMTICS:
	case TBM_GETSELSTART:
	case TBM_GETSELEND:
	case TBM_CLEARSEL:
	case TBM_SETTICFREQ:
	case TBM_SETPAGESIZE:
	case TBM_GETPAGESIZE:
	case TBM_SETLINESIZE:
	case TBM_GETLINESIZE:
	case TBM_GETTHUMBRECT:
	case TBM_GETCHANNELRECT:
	case TBM_SETTHUMBLENGTH:
	case TBM_GETTHUMBLENGTH:

	case TBM_SETBUDDY:
	case TBM_GETBUDDY:

	case WM_PAINT:
		OnPaintTrackbar(hWnd);
		return 0;
	case WM_ERASEBKGND:
		OnEraseBkGnd(hWnd,(HDC)wParam);
		return 0;
	case WM_DESTROY:
		OnDestroyTrackbar(hWnd);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
ATOM RegisterTrackbarClass(HINSTANCE hInstance)
{
	WNDCLASS    wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc		= (WNDPROC)TrackbarProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= GetStockObject( WHITE_BRUSH );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classTRACKBAR;

	return RegisterClass(&wc);
}
