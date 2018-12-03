/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：toolbar 按钮类，系统类部分
版本号：1.0.0
开发时期：2004-08-25
作者：陈建明 Jami chen
修改记录：
		2004。08。25 -- 增加支持ICON的功能
******************************************************/
#include <EWindows.h>
#include <EToolbar.h>

#include "../include/TBardef.h"
#include "Etbimage.h"

#define COLOR_BACKGROUND COLOR_SCROLLBARTHUMB

//word align
static const BYTE bmpDropDown[] = {
    0x00,0x00,
	0x82,0x00,
	0xc6,0x00,
	0xee,0x00
    //0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    //0X07,0X00,0X00,0X00,0X00,0X00,0X07,
    //0X07,0X07,0X00,0X00,0X00,0X07,0X07,
    //0X07,0X07,0X07,0X00,0X07,0X07,0X07,
   };
static const BYTE bmpDropUp[] = {
	0xee,0x00,
	0xc6,0x00,
	0x82,0x00,
	0x00,0x00
    //0X07,0X07,0X07,0X00,0X07,0X07,0X07,
    //0X07,0X07,0X00,0X00,0X00,0X07,0X07,
    //0X07,0X00,0X00,0X00,0X00,0X00,0X07,
    //0X00,0X00,0X00,0X00,0X00,0X00,0X00,
   };

#define WHITE_TOOL_BAR  //工具条背景是白色的
//#define SYSBK_TOOL_BAR    //工具条背景是系统颜色的
const TCHAR classTBBUTTON[]="_CLASS_TBBUTTON_";

#define	DROPDOWN_WIDTH									10
#define SEP_WIDTH										10

static LRESULT CALLBACK	TBButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL OnTBButtonCreate(HWND hWnd);
static void DoTBButtonLButtonDown(HWND hWnd,WPARAM wParam,int xPos,int yPos);
static void DoTBButtonLButtonUp(HWND hWnd,WPARAM wParam,int xPos,int yPos);
static void DoTBButtonPaint(HWND hWnd);
static HBITMAP GetBitmapOfButton(HWND hWnd,BYTE state);
static BOOL ResizeToolbar(HWND hWnd);
static void DrawBitMap(HDC hdc,HBITMAP hBitMap,RECT rect, int xOffset, int yOffset);
static void DoTBButtonDestroy(HWND hWnd);
static BOOL DoTBButtonSetData(HWND hWnd,LPTBBUTTONINFO lpti);
static BOOL DoTBButtonGetData(HWND hWnd,LPTBBUTTONINFO lpti);
static void DrawTBIcon(HWND hWnd,HDC hdc,HICON hIcon);
//===============================================================================
static HBITMAP GetBitmapOfButton(HWND hWnd,BYTE state){
	_LPTOOLBARDATA				lptd;
	_LPTBBUTTONDATA				lptbbd;
//	LPPTRLIST					hImageList;
	HIMAGELIST					hImageList;
	UINT iCount;

	lptd=(_LPTOOLBARDATA)GetWindowLong(GetParent(hWnd),0);
	ASSERT(lptd!=NULL);
	if(state&TBSTATE_ENABLED)
		hImageList=lptd->hImgList;
	else
		hImageList=lptd->hDisImgList;
	lptbbd=(_LPTBBUTTONDATA)GetWindowLong(hWnd,0);
	ASSERT(lptbbd!=NULL);
	iCount = ImageList_GetImageCount(hImageList);

//	if((lptbbd->iImage<0)||(hImageList->count<=lptbbd->iImage))
//		return 0;
	if((lptbbd->iImage<0)||(iCount<=(UINT)lptbbd->iImage))
		return 0;
//	return PtrListAt(hImageList,lptbbd->iImage);
	return (HBITMAP)ImageList_GetIcon(hImageList,lptbbd->iImage,0);
}
static BOOL ResizeToolbar(HWND hWnd){
	SendMessage(GetParent(hWnd),TB_AUTOSIZE,0,0);
	return TRUE;
}
//===============================================================================
static BOOL OnTBButtonCreate(HWND hWnd){
	_LPTBBUTTONDATA			lptd;
	lptd=(_LPTBBUTTONDATA)malloc(sizeof(_TBBUTTONDATA));
	if(lptd==NULL)
		return FALSE;
	memset(lptd,0,sizeof(_TBBUTTONDATA));
	lptd->fsState=TBSTATE_ENABLED;
	SetWindowLong(hWnd,0,(long)lptd);
	return TRUE;
}
static void DoTBButtonDestroy(HWND hWnd){
	_LPTBBUTTONDATA			lptd;
	lptd=(_LPTBBUTTONDATA)GetWindowLong(hWnd,0);
	ASSERT(lptd!=NULL);
//	if(lptd==NULL)
//		return ;
	if(lptd->pszText!=NULL)
		free(lptd->pszText);
	free(lptd);
}
static void DoTBButtonLButtonDown(HWND hWnd,WPARAM wParam,int xPos,int yPos)
{
	_LPTBBUTTONDATA			lptd;
	POINT					pt;
	RECT					rt;
	
//	RETAILMSG(1,(TEXT("LBUTTONDOWN\r\n")));
	lptd=(_LPTBBUTTONDATA)GetWindowLong(hWnd,0);
	ASSERT(lptd!=NULL);
	if ( (lptd->fsState & TBSTATE_ENABLED ) == 0)
		return;
	pt.x=xPos;
	pt.y=yPos;
	GetClientRect(hWnd,&rt);
	if(PtInRect(&rt,pt)==TRUE){
		if(lptd->fsStyle&BTNS_SEP)
			return ;
		if(lptd->fsStyle&BTNS_DROPDOWN){
			rt.left=rt.right-DROPDOWN_WIDTH;
			if(PtInRect(&rt,pt)==TRUE){
				NMTOOLBARW				nmtb;
				HDC						hdc;
				nmtb.pszText=NULL;
				nmtb.hdr.hwndFrom=hWnd;
				nmtb.hdr.idFrom=lptd->idCommand;
				nmtb.iItem=nmtb.hdr.idFrom;
				if(lptd->pszText!=NULL){
					nmtb.pszText=(TCHAR*)malloc(sizeof(TCHAR)*(lptd->cchText+1));
					if(nmtb.pszText==NULL)
						return ;
					strcpy(nmtb.pszText,lptd->pszText);
					nmtb.cchText=lptd->cchText;
				}
				nmtb.tbButton.fsState=lptd->fsState;
				nmtb.tbButton.fsStyle=lptd->fsStyle;
				nmtb.tbButton.iBitmap=lptd->iImage;
				nmtb.tbButton.idCommand=lptd->idCommand;
				nmtb.tbButton.dwData=lptd->lParam;
				hdc=GetDC(hWnd);
				InvertRect(hdc,&rt);
				SendMessage(GetParent(hWnd),WM_NOTIFY,TBN_DROPDOWN,(LPARAM)&nmtb);
				InvertRect(hdc,&rt);
				ReleaseDC(hWnd,hdc);
				if(nmtb.pszText!=NULL)
					free(nmtb.pszText);
				return ;
			}
		}
	}
	SetCapture(hWnd);
	lptd->fsState|=TBSTATE_PRESSED;
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE);
//	RETAILMSG(1,(TEXT("LBUTTONDOWN OK\r\n")));
}
static void DoTBButtonLButtonUp(HWND hWnd,WPARAM wParam,int xPos,int yPos){
	_LPTBBUTTONDATA			lptd;
	POINT					pt;
	RECT					rt;

//	RETAILMSG(1,(TEXT("LBUTTONUP\r\n")));
	lptd=(_LPTBBUTTONDATA)GetWindowLong(hWnd,0);
	ASSERT(lptd!=NULL);
	if ( (lptd->fsState & TBSTATE_ENABLED ) == 0)
		return;
	ReleaseCapture();
	pt.x=xPos;
	pt.y=yPos;
	GetClientRect(hWnd,&rt);
//	if(PtInRect(&rt,pt)==TRUE)
	if(TRUE)
	{
		if(lptd->fsState&TBSTATE_PRESSED){
			lptd->fsState&=~TBSTATE_PRESSED;
			if(lptd->fsStyle&BTNS_CHECK){
				if(lptd->fsState&TBSTATE_CHECKED)
					lptd->fsState&=~TBSTATE_CHECKED;
				else
					lptd->fsState|=TBSTATE_CHECKED;
				if(lptd->fsState&BTNS_GROUP){
					lptd->fsState|=TBSTATE_PRESSED;
					//SendMessage(GetParent(hWnd),...;//update the group of the button
				}
			}
			GetClientRect(hWnd,&rt);
			InvalidateRect(hWnd,&rt,TRUE);
//			PostMessage(GetParent(hWnd),WM_COMMAND,MAKELONG(lptd->idCommand,0),(LPARAM)hWnd);
//			RETAILMSG(1,(TEXT("Send Command [%d]\r\n"),lptd->idCommand));
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKELONG(lptd->idCommand,0),(LPARAM)hWnd);
		}
	}
	else
	{
		lptd->fsState&=~TBSTATE_PRESSED;
		GetClientRect(hWnd,&rt);
		InvalidateRect(hWnd,&rt,TRUE);
		RETAILMSG(1,(TEXT("LBUTTONUP (%d,%d)Invalidate\r\n"),pt.x,pt.y));
	}
//	RETAILMSG(1,(TEXT("LBUTTONUP OK\r\n")));
}
static void DrawBitMap(HDC hdc,HBITMAP hBitMap,RECT rect, int xOffset, int yOffset)
{
	HDC hMemoryDC;
	if(hBitMap==0)
		return;
	hMemoryDC=CreateCompatibleDC(hdc);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	BitBlt( hdc, (short)rect.left,(short)rect.top,(short)(rect.right-rect.left),
		(short)(rect.bottom-rect.top),hMemoryDC,(short)xOffset,(short)yOffset,SRCCOPY);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	DeleteDC(hMemoryDC);
}

					
static void DrawTBIcon(HWND hWnd,HDC hdc,HICON hIcon)
{
	_LPTOOLBARDATA				lptd;
	int x,y;
	RECT rt;

		lptd=(_LPTOOLBARDATA)GetWindowLong(GetParent(hWnd),0);
		ASSERT(lptd!=NULL);

		GetClientRect(hWnd,&rt);

		x = (rt.right - lptd->dxBitmap) / 2; // 显示在中央
		y = (rt.bottom - lptd->dyBitmap) / 2; // 显示在中央

		DrawIcon(hdc,x,y,hIcon);
}

//居中画竖分隔线  ||
static void DrawSepOfButton(HWND hWnd,HDC hdc){
	RECT					rt;
	GetClientRect(hWnd,&rt);
	MoveTo(hdc,(rt.right-rt.left)/2-1,rt.top+2);
	LineTo(hdc,(rt.right-rt.left)/2-1,rt.bottom-2);
	MoveTo(hdc,(rt.right-rt.left)/2+1,rt.top+2);
	LineTo(hdc,(rt.right-rt.left)/2+1,rt.bottom-2);
}
//画上下箭头 |▼   and |▲
static void DrawDropDown(HWND hWnd,HDC hdc,RECT rt,BOOL isDown){
	HBITMAP						hBmpDropDown;
	//int cy;
	rt.left=rt.right-DROPDOWN_WIDTH;
	//上下箭头位图
	if(isDown==TRUE){
		//hBmpDropDown=CreateBitmap(7,4,1,8,bmpDropDown);
		hBmpDropDown=CreateBitmap(7,4,1,1,bmpDropDown);
	}else{
		//hBmpDropDown=CreateBitmap(7,4,1,8,bmpDropUp);
		hBmpDropDown=CreateBitmap(7,4,1,1,bmpDropUp);
	}
	//画一条竖线
	MoveTo(hdc,rt.left,rt.top+2);
	LineTo(hdc,rt.left,rt.bottom-2);
	//DrawBitMap(hdc,hBmpDropDown,rt,-2,-7);
	rt.left+=2;
	rt.top += ( (rt.bottom - rt.top) - 4 ) / 2;
	rt.bottom = rt.top + 4;
	DrawBitMap(hdc,hBmpDropDown,rt, 0,0);
	DeleteObject(hBmpDropDown);
}
static void DoTBButtonPaint(HWND hWnd){
	HDC						hdc;
    PAINTSTRUCT				ps;
	_LPTBBUTTONDATA			lptd;
	_LPTOOLBARDATA			lptool;
	RECT					rt;
	HBITMAP					hBmp;
	HICON					hIcon;
	int						iWidth=0;
	DWORD					dwToolBarStyle;
	BOOL					bExistImage = FALSE;

	lptd=(_LPTBBUTTONDATA)GetWindowLong(hWnd,0);
	ASSERT(lptd!=NULL);
	lptool=(_LPTOOLBARDATA)GetWindowLong(GetParent(hWnd),0);
	ASSERT(lptool!=NULL);
//	if(lptd->fsState&TBSTATE_HIDDEN){
//		ShowWindow(hWnd,SW_HIDE);
//		return ;
//	}
	hdc = BeginPaint( hWnd, &ps );
	dwToolBarStyle = GetWindowLong( GetParent(hWnd), GWL_STYLE); // 得到工具条的风格
	if(  dwToolBarStyle & CCS_SYSTEMCOLOR ){
		GetClientRect(hWnd, &rt);
	}else{
		if(lptd->fsStyle&BTNS_SEP){
			//仅仅画分隔线
			DrawSepOfButton(hWnd,hdc);
			EndPaint(hWnd,&ps);
			return ;
		}
		if (dwToolBarStyle & CCS_ICONIMAGE)
		{
			hIcon = (HICON)GetBitmapOfButton(hWnd, lptd->fsState);
			if (hIcon)
				bExistImage = TRUE;
		}
		else
		{
			hBmp = GetBitmapOfButton(hWnd, lptd->fsState);
			if (hBmp)
				bExistImage = TRUE;
		}
		if(lptd->fsStyle & BTNS_AUTOSIZE){
			//宽度等于文本+image
			if((dwToolBarStyle & TBSTYLE_LIST)
				&&(lptd->fsStyle&BTNS_SHOWTEXT)){
				GetCharWidth(hdc,'A','A',&iWidth);
			}
			if(lptd->cx < (iWidth+lptool->dxBitmap)){
				//改变窗口宽度
				lptd->cx=iWidth*(lptd->cchText+1)+lptool->dxBitmap;
				ResizeToolbar(hWnd);
			}
		}
		GetClientRect(hWnd,&rt);
		// 首先要画边框并清楚内容
		if(lptd->fsState&TBSTATE_PRESSED){
			//按下
			//rt.top+=1;
			DrawEdge(hdc,&rt,BDR_SUNKENOUTER,BF_RECT|BF_MIDDLE);
//			DrawEdge(hdc,&rt,EDGE_SUNKEN,BF_RECT|BF_MIDDLE);
			//DrawEdge(hdc,&rt,EDGE_SUNKEN,BF_RECT);
//			InflateRect( &rt, -2, -2 );
			InflateRect( &rt, -1, -1 );
			//rt.top+=1;
			//rt.left+=1;
		}else if(!(dwToolBarStyle&TBSTYLE_FLAT)){
			//默认情况-抬起效果状态
			DrawEdge(hdc,&rt,BDR_RAISEDOUTER,BF_RECT|BF_MIDDLE);
			//DrawEdge(hdc,&rt,BDR_RAISED,BF_RECT|BF_MIDDLE);
//			InflateRect( &rt, -2, -2 );
			InflateRect( &rt, -1, -1 );
		}
		if(lptd->fsStyle & BTNS_DROPDOWN){
			if(GetWindowLong(GetParent(hWnd), GWL_STYLE) & CCS_TOP)
				DrawDropDown(hWnd,hdc,rt,TRUE);
			else
				DrawDropDown(hWnd,hdc,rt,FALSE);
			rt.right -= DROPDOWN_WIDTH;
		}
//		if(hBmp != 0)
		if(bExistImage != 0)
		{
			if(lptd->fsStyle&BTNS_DROPDOWN)
			{	//有下箭头
				//DrawBitMap(hdc,hBmp,rt,-2,-1);
				if (dwToolBarStyle & CCS_ICONIMAGE)
					DrawTBIcon(hWnd,hdc,hIcon);
				else
					DrawBitMap(hdc,hBmp,rt,0,0);
			}
			else
			{
#ifdef WHITE_TOOL_BAR
				if (!(dwToolBarStyle & CCS_ICONIMAGE))
				{
					if( !(lptd->fsState&TBSTATE_PRESSED) )
					{
						HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255) );
						ASSERT (hPen != NULL);
						hPen = (HPEN)SelectObject(hdc, hPen);
						Rectangle (hdc, rt.left, rt.top, rt.right, rt.bottom);
						hPen = (HPEN)SelectObject(hdc, hPen);
						ASSERT (hPen != NULL);
						DeleteObject(hPen);
					}
				}
#endif
				//DrawBitMap(hdc,hBmp,rt,-2,-2);
				if (dwToolBarStyle & CCS_ICONIMAGE)
					DrawTBIcon(hWnd,hdc,hIcon);
				else
					DrawBitMap(hdc,hBmp,rt,0,0);
			}
//			rt.left+=lptool->dxBitmap+4;
			rt.left+=lptool->dxBitmap;
		}else{
#ifdef  SYSBK_TOOL_BAR
			FillRect(hdc,&rt,GetSysColorBrush(COLOR_BACKGROUND)); // 首先要清除背景
#endif
//			rt.left+=4;
		}
		if((GetWindowLong(GetParent(hWnd),GWL_STYLE)&TBSTYLE_LIST)&&
			(lptd->fsStyle&BTNS_SHOWTEXT)){
#ifdef WHITE_TOOL_BAR
			SetBkMode(hdc, TRANSPARENT);
#endif
#ifdef  SYSBK_TOOL_BAR
			SetBkMode(hdc, TRANSPARENT);
#endif
#ifdef GRAY_TOOL_BAR
			SetBkColor(hdc,CL_LIGHTGRAY);
#endif
			if(lptd->pszText!=NULL)
				DrawText(hdc,lptd->pszText,strlen(lptd->pszText),&rt,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}
	}
    EndPaint( hWnd, &ps );
}
static BOOL DoTBButtonSetData(HWND hWnd,LPTBBUTTONINFO lpti){
	_LPTBBUTTONDATA			lptd;
	RECT					rt;
	lptd=(_LPTBBUTTONDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	if(lpti==NULL)
		return FALSE;
	if(lpti->dwMask&TBIF_COMMAND){
		lptd->idCommand=lpti->idCommand;
		SetWindowLong(hWnd,GWL_ID,(long)lpti->idCommand);
	}
	if(lpti->dwMask&TBIF_IMAGE){
		lptd->iImage=lpti->iImage;
	}
	if(lpti->dwMask&TBIF_LPARAM)
		lptd->lParam=lpti->lParam;
	if(lpti->dwMask&TBIF_SIZE){
		lptd->cx=lpti->cx;
		ResizeToolbar(hWnd);
	}
// !!! modified by jami chen in 2004.08.28
//	if(lpti->dwMask&TBIF_STATE)
//		lptd->fsState=lpti->fsState;
	if(lpti->dwMask&TBIF_STATE)
	{
		UINT oldState = lptd->fsState;
		lptd->fsState=lpti->fsState;
		if( (lptd->fsState & TBSTATE_HIDDEN) && 
			(oldState & TBSTATE_HIDDEN) == 0 )
		{
			ShowWindow( hWnd, SW_HIDE );
		}
		else if( (lptd->fsState & TBSTATE_HIDDEN) == 0 &&
			     (oldState & TBSTATE_HIDDEN) )
		{
			ShowWindow( hWnd, SW_SHOW );
		}
	}
// !!! modified End 
	if(lpti->dwMask&TBIF_STYLE){
		lptd->fsStyle=lpti->fsStyle;
	}
	if(lpti->dwMask&TBIF_TEXT){
		if(lptd->pszText!=NULL)
			free(lptd->pszText);
		lptd->pszText=(TCHAR*)malloc(sizeof(TCHAR)*(lpti->cchText+1));
		if(lptd->pszText==NULL)
			return FALSE;
		strcpy(lptd->pszText,lpti->pszText);
		lptd->cchText=lpti->cchText;
	}
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE);
	return TRUE;
}
static BOOL DoTBButtonGetData(HWND hWnd,LPTBBUTTONINFO lpti){
	_LPTBBUTTONDATA			lptd;
	lptd=(_LPTBBUTTONDATA)GetWindowLong(hWnd,0);
	ASSERT(lptd!=NULL);
	if(lpti==NULL)
		return FALSE;
	if(lpti->dwMask&TBIF_COMMAND)
		lpti->idCommand=lptd->idCommand;
	if(lpti->dwMask&TBIF_IMAGE)
		lpti->iImage=lptd->iImage;
	if(lpti->dwMask&TBIF_LPARAM)
		lpti->lParam=lptd->lParam;
	if(lpti->dwMask&TBIF_SIZE)
		lpti->cx=lptd->cx;
	if(lpti->dwMask&TBIF_STATE)
		lpti->fsState=lptd->fsState;
	if(lpti->dwMask&TBIF_STYLE)
		lpti->fsStyle=lptd->fsStyle;
	if(lpti->dwMask&TBIF_TEXT){
		if(lpti->cchText<lptd->cchText)
			return FALSE;
		strcpy(lpti->pszText,lptd->pszText);
		lpti->cchText=lptd->cchText;
	}
	return TRUE;
}
BOOL SetTBButtonInfo(HWND hWnd,LPTBBUTTONINFO lpti){
//	return SendMessage(hWnd,TBBM_SETDATA,(WPARAM)lpti,0);
	return DoTBButtonSetData( hWnd, lpti );
}
BOOL GetTBButtonInfo(HWND hWnd,LPTBBUTTONINFO lpti){
//	return SendMessage(hWnd,TBBM_GETDATA,(WPARAM)lpti,0);
	return DoTBButtonGetData( hWnd, lpti );
}
static void DoTBButtonEraseBkgnd(HWND hWnd,HDC hdc ){
	HBRUSH				hBrush;
	RECT				rt;
//	if(GetWindowLong(GetParent(hWnd),GWL_STYLE)&CCS_SYSTEMCOLOR)
	if(GetWindowLong(GetParent(hWnd),GWL_STYLE)&(CCS_SYSTEMCOLOR|CCS_ICONIMAGE))
	{
		GetClientRect(hWnd,&rt);
		hBrush=CreateSolidBrush(GetSysColor(COLOR_BACKGROUND));
		FillRect(hdc,&rt,hBrush);
		DeleteObject(hBrush);
	}
	else if(GetWindowLong(GetParent(hWnd),GWL_STYLE)&CCS_WHITECOLOR)
	{
		GetClientRect(hWnd,&rt);
		hBrush = GetStockObject( WHITE_BRUSH ) ;
		FillRect(hdc,&rt,hBrush);
	}
	else
	{
		DefWindowProc(hWnd, WM_ERASEBKGND, (WPARAM)hdc, 0);
	}
}
static LRESULT CALLBACK	TBButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
	case WM_CREATE:
		if(OnTBButtonCreate(hWnd)==FALSE)
			return 1;
		return 0;
	case WM_PAINT:
		DoTBButtonPaint(hWnd);
		return 0;
	case WM_ERASEBKGND:
		DoTBButtonEraseBkgnd(hWnd,(HDC)wParam);
		return 0;
	case TBBM_SETDATA:
		return DoTBButtonSetData(hWnd,(LPTBBUTTONINFO)wParam);
	case TBBM_GETDATA:
		return DoTBButtonGetData(hWnd,(LPTBBUTTONINFO)wParam);
	case WM_LBUTTONDOWN:
		DoTBButtonLButtonDown(hWnd,wParam,LOWORD(lParam),HIWORD(lParam));
		return 0;
	case WM_LBUTTONUP:
		DoTBButtonLButtonUp(hWnd,wParam,LOWORD(lParam),HIWORD(lParam));
		return 0;
//	case WM_MOUSEMOVE:
//		if (GetCapture() == hWnd)
//			RETAILMSG(1,(TEXT("MOUSEMOVE (%d,%d)\r\n"),LOWORD(lParam),HIWORD(lParam)));
//		return 0;
	case WM_DESTROY:
		DoTBButtonDestroy(hWnd);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
ATOM RegisterTBButtonClass(HINSTANCE hInstance)
{
	WNDCLASS    wc;

	wc.style			= CS_DBLCLKS;
	wc.lpfnWndProc		= (WNDPROC)TBButtonProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(LPVOID);
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
//	wc.hbrBackground	= (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classTBBUTTON;

	return RegisterClass(&wc);
}
HWND CreateTBButton(HWND hWnd,LPTBBUTTON lpbt,RECT	rt){
	HINSTANCE					hInstance;
	HWND						hTBButton;
	TBBUTTONINFO				ti;
	_LPTOOLBARDATA				lptd;
	int							iWidth=0;
	ti.pszText = NULL;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
	ti.dwMask=TBIF_COMMAND|TBIF_IMAGE|TBIF_LPARAM|TBIF_STATE|TBIF_SIZE|TBIF_STYLE;
	ti.fsState=lpbt->fsState;
	ti.fsStyle=lpbt->fsStyle;
	if((lpbt->iString>=0)&&(lpbt->iString<lptd->lpStringList->count)){
		TCHAR				*lpstr;
		ti.dwMask|=TBIF_TEXT;
		ti.pszText=(TCHAR*)malloc(sizeof(TCHAR)*(strlen((TCHAR*)PtrListAt(
			lptd->lpStringList,lpbt->iString))+1));
		if(ti.pszText==NULL)
			return 0;
		lpstr=(TCHAR*)PtrListAt(lptd->lpStringList,lpbt->iString);
		strcpy(ti.pszText,lpstr);
		ti.cchText=strlen(ti.pszText);
	}
	if(lpbt->fsStyle&BTNS_AUTOSIZE){
		HDC				hdc;
		hdc=GetDC(hWnd);
		GetCharWidth(hdc,'A','A',&iWidth);
		ReleaseDC(hWnd,hdc);
	}
// !!! Modified By Jami chen in 2004.09.02
/*
	if(lpbt->iBitmap>=0)
		iWidth=iWidth*(ti.cchText+1)+lptd->dxBitmap;
	else
		iWidth=iWidth*(ti.cchText+1);
*/
	if(lpbt->iBitmap>=0)
		iWidth=iWidth*(ti.cchText) + 4 + lptd->dxBitmap;
	else
		iWidth=iWidth*(ti.cchText) + 4;
// !!! Modified End By Jami chen in 2004.09.02
	if(iWidth<lptd->dxButton)
		iWidth=lptd->dxButton;
	ti.cx=iWidth;
	if(lpbt->fsStyle&BTNS_DROPDOWN)
		ti.cx+=DROPDOWN_WIDTH;
	if(lpbt->fsStyle&BTNS_SEP)
		ti.cx=SEP_WIDTH;
	ti.idCommand=lpbt->idCommand;
	ti.iImage=lpbt->iBitmap;
	ti.lParam=lpbt->dwData;
	if(lpbt->fsStyle&BTNS_RIGHTBUTTON){
//		hTBButton=CreateWindow(classTBBUTTON,"",WS_VISIBLE|WS_CHILD,
//			rt.right-iWidth,rt.top,iWidth,
//			rt.bottom-rt.top,hWnd,NULL,hInstance,NULL);
		hTBButton=CreateWindow(classTBBUTTON,"",WS_VISIBLE|WS_CHILD,
			rt.right-iWidth,rt.top,iWidth,
			lptd->dyButton,hWnd,NULL,hInstance,NULL);
	}else{
//		hTBButton=CreateWindow(classTBBUTTON,"",WS_VISIBLE|WS_CHILD,
//			rt.left,rt.top,iWidth,
//			rt.bottom-rt.top,hWnd,NULL,hInstance,NULL);
		hTBButton=CreateWindow(classTBBUTTON,"",WS_VISIBLE|WS_CHILD,
			rt.left,rt.top,iWidth,
			lptd->dyButton,hWnd,NULL,hInstance,NULL);
	}
	if(hTBButton==0)
		return 0;
	SetTBButtonInfo(hTBButton,&ti);
	if(ti.pszText!=NULL)
		free(ti.pszText);
	return hTBButton;
}
