/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.
Copyright (c) 1994-2001  Micrologic electronic Corporation(微逻辑电子有限公司)

Module Name:  
	etoolbar.c
Abstract:  
	etoolbar is toolbar like toolbar on PC
Author:
	Micrologic electronic Corporation(微逻辑电子有限公司)
History:
    2001.10 base function is ok by kingkong(v1.0.0)
    2001.11 modify it and espand functions is ok by kingkong(v1.0.1)
--*/
#include <EWindows.h>
#include <EToolbar.h>
#include <EPTRLIST.H>

#include "../include/TBardef.h"

#define COLOR_BACKGROUND COLOR_SCROLLBARTHUMB

const TCHAR			classTOOLBAR[]="_CLASS_TOOLBAR_";
////////////////////////////////////////////////////////////
#define SYS_HIST_ADDTOFAVORITES             125
#define SYS_HIST_BACK                       126
#define SYS_HIST_FAVORITES                  127
#define SYS_HIST_FORWARD                    128
#define SYS_HIST_VIEWTREE                   129
#define SYS_STD_COPY                        130
#define SYS_STD_CUT                         131
#define SYS_STD_DELETE                      132
#define SYS_STD_FILENEW                     133
#define SYS_STD_FILEOPEN                    134
#define SYS_STD_FILESAVE                    135
#define SYS_STD_FIND                        136
#define SYS_STD_HELP                        137
#define SYS_STD_PASTE                       138
#define SYS_STD_PRINT                       139
#define SYS_STD_PRINTPRE                    140
#define SYS_STD_PROPERTIES                  141
#define SYS_STD_REDOW                       142
#define SYS_STD_REPLACE                     143
#define SYS_STD_UNDO                        144
static int STD_ID [] =
 {
		SYS_STD_COPY,
		SYS_STD_CUT,
		SYS_STD_DELETE,
		SYS_STD_FILENEW,
		SYS_STD_FILEOPEN,
		SYS_STD_FILESAVE,
		SYS_STD_FIND,
		SYS_STD_HELP,
		SYS_STD_PASTE,
		SYS_STD_PRINT,
		SYS_STD_PRINTPRE,
		SYS_STD_PROPERTIES,
		SYS_STD_REDOW,
		SYS_STD_REPLACE,
		SYS_STD_UNDO //15
};
static int VIEW_ID[] = 
{
		VIEW_DETAILS,
		VIEW_LARGEICONS,
		VIEW_LIST,
		VIEW_NETCONNECT,
		VIEW_NETDISCONNECT,
		VIEW_NEWFOLDER,
		VIEW_PARENTFOLDER,
		VIEW_SMALLICONS,
		VIEW_SORTDATE ,
		VIEW_SORTNAME ,
		VIEW_SORTSIZE ,
		VIEW_SORTTYPE  //12
} ;
static int HIST_ID[] =
{
		SYS_HIST_ADDTOFAVORITES,
		SYS_HIST_BACK,
		SYS_HIST_FAVORITES,
		SYS_HIST_FORWARD ,
		SYS_HIST_VIEWTREE //5
} ;
#define HINST_NUMBER			5
#define STD_NUMBER				15
#define VIEW_NUMBER				12

#define X_SPACE          2
#define Y_SPACE          1
#define INDENT_SPACE     3

////////////////////////////////////////////////////////////
static LRESULT CALLBACK	ToolBarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL OnToolBarCreate(HWND hWnd);
static void DoToolBarDestroy(HWND hWnd);
static void DoToolBarCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoToolBarNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoToolBarPaint(HWND hWnd);
static void DoToolBarEraseBkgnd(HWND hWnd,HDC hdc);
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
//================================================================================
static int DoTB_AddBitmap(HWND hWnd,int nButtons, LPTBADDBITMAP  lptbab );
static BOOL DoTB_AddButton(HWND hWnd,int uNumButtons, LPTBBUTTON lpButtons); 
static int DoTB_AddString(HWND hWnd,int iNum, TCHAR * szText);
static void DoTB_AutoSize(HWND hWnd);
static int DoTB_ButtonCount(HWND hWnd);
static BOOL DoTB_ChangeBitmap(HWND hWnd,int idButton,WORD iBitmap);
static BOOL DoTB_CheckButton(HWND hWnd,int idButton,BOOL fCheck);
static int DoTB_CommandToIndex(HWND hWnd,int idButton);
static BOOL DoTB_DeleteButton(HWND hWnd,int iIndex);
static BOOL DoTB_EnableButton(HWND hWnd,int idButton,BOOL fEnable);
static int DoTB_GetBitmap(HWND hWnd,int idButton);
static BOOL DoTB_PressButton(HWND hWnd,int idButton,BOOL fPress);
static int DoTB_SetButtonInfo(HWND hWnd,int idButton,LPTBBUTTONINFO lptbbi);
static BOOL DoTB_SetCmdId(HWND hWnd,int iIndex,int idButton);
//static LPPTRLIST DoTB_SetDisabledImageList(HWND hWnd,LPPTRLIST himlNewDisabled);
//static LPPTRLIST DoTB_SetImageList(HWND hWnd,LPPTRLIST  hImageList);
static HIMAGELIST DoTB_SetDisabledImageList(HWND hWnd,LPPTRLIST himlNewDisabled);
static HIMAGELIST DoTB_SetImageList(HWND hWnd,LPPTRLIST  hImageList);
//================================================================================
static BOOL OnToolBarCreate(HWND hWnd){
	_LPTOOLBARDATA					lptd;
	lptd=(_LPTOOLBARDATA)malloc(sizeof(_TOOLBARDATA));
	if(lptd==NULL)
		return FALSE;
	memset( lptd, 0, sizeof(_TOOLBARDATA) );
/*
	lptd->hDisImgList=(LPPTRLIST)malloc(sizeof(PTRLIST));
	if(lptd->hDisImgList==NULL)
		return FALSE;
	PtrListCreate(lptd->hDisImgList,5,5);
	
	lptd->hImgList=(LPPTRLIST)malloc(sizeof(PTRLIST));
	if(lptd->hImgList==NULL)
		return FALSE;
	PtrListCreate(lptd->hImgList,5,5);
*/
/*	lptd->hDisImgList=ImageList_Create(16,16,ILC_COLOR8,5,5);
	if(lptd->hDisImgList==NULL)
		return FALSE;
	
	lptd->hImgList=ImageList_Create(16,16,ILC_COLOR8,5,5);
	if(lptd->hImgList==NULL)
		return FALSE;*/
	lptd->hDisImgList = NULL;
	lptd->hImgList = NULL;
	lptd->lpButtonList=(LPPTRLIST)malloc(sizeof(PTRLIST));
	if(lptd->lpButtonList==NULL)
		return FALSE;
	PtrListCreate(lptd->lpButtonList,5,5);

	lptd->lpStringList=(LPPTRLIST)malloc(sizeof(PTRLIST));
	if(lptd->lpStringList==NULL)
		return FALSE;
	PtrListCreate(lptd->lpStringList,5,5);
	
	lptd->iIndent= INDENT_SPACE;//3;
	lptd->cx= X_SPACE;//2;
	lptd->cy= Y_SPACE;//2;
//	lptd->cl_Bk = CL_WHITE;
	lptd->cl_Bk = GetSysColor(COLOR_BACKGROUND);

	SetWindowLong(hWnd, 0, (long)lptd);
	return TRUE;
}
static void DoToolBarDestroy(HWND hWnd){
	_LPTOOLBARDATA					lptd;
//	HBITMAP							hBmp;
//	int								i;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);

	PtrListFreeAll(lptd->lpButtonList);
	PtrListDestroy(lptd->lpButtonList);
	free(lptd->lpButtonList);

	PtrListFreeAll(lptd->lpStringList);
	PtrListDestroy(lptd->lpStringList);
	free(lptd->lpStringList);

//	PtrListFreeAll(lptd->hDisImgList);
//	for(i=0;i<lptd->hDisImgList->count;i++){
//		hBmp=(HBITMAP)PtrListAt(lptd->hDisImgList,i);
//		DeleteObject(hBmp);
//	} 
	ImageList_Destroy(lptd->hDisImgList);

//	for(i=0;i<lptd->hImgList->count;i++){
//		hBmp=(HBITMAP)PtrListAt(lptd->hImgList,i);
//		DeleteObject(hBmp);
//	}
//	PtrListFreeAll(lptd->hImgList);
//	PtrListDestroy(lptd->hImgList);
//	free(lptd->hImgList);
	ImageList_Destroy(lptd->hImgList);

	free(lptd);
}
static void DoToolBarCommand(HWND hWnd,WPARAM wParam,LPARAM lParam){
	SendMessage(GetParent(hWnd),WM_COMMAND,wParam,(LPARAM)hWnd);
}
static void DoToolBarNotify(HWND hWnd,WPARAM wParam,LPARAM lParam){
	if(wParam==TBN_DROPDOWN){
		SendMessage(GetParent(hWnd),WM_NOTIFY,TBN_DROPDOWN,lParam);
	}
}
static void DoToolBarPaint(HWND hWnd){
	HDC hMemDC;
    PAINTSTRUCT ps;
	RECT rt;
	hMemDC = BeginPaint( hWnd, &ps );
	GetClientRect(hWnd,&rt);
	if(GetWindowLong(hWnd,GWL_STYLE)&CCS_TOP){
		MoveTo(hMemDC,rt.left,rt.bottom-1);
//		LineTo(hMemDC,rt.right,rt.bottom-1);
	}else{
		MoveTo(hMemDC,rt.left,rt.top);
//		LineTo(hMemDC,rt.right,rt.top);
	}
    EndPaint( hWnd, &ps );
	return ;
}
static void DoToolBarEraseBkgnd( HWND hWnd, HDC hdc )
{
	HBRUSH					hBrush;
	RECT					rt;
	_LPTOOLBARDATA					lptd;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);

	GetClientRect(hWnd,&rt);
	hBrush=CreateSolidBrush( lptd->cl_Bk );
	FillRect(hdc,&rt,hBrush);
	DeleteObject(hBrush);
/*
	if(GetWindowLong(hWnd,GWL_STYLE)&CCS_SYSTEMCOLOR){
		GetClientRect(hWnd,&rt);
		hBrush=CreateSolidBrush(GetSysColor(COLOR_BACKGROUND));
		FillRect(hdc,&rt,hBrush);
		DeleteObject(hBrush);
	}else if(GetWindowLong(hWnd,GWL_STYLE)&CCS_WHITECOLOR){
		GetClientRect(hWnd,&rt);
		hBrush=GetStockObject( WHITE_BRUSH );
		FillRect(hdc,&rt,hBrush);
	}else{
		DefWindowProc(hWnd, WM_ERASEBKGND, (WPARAM)hdc, 0);
	}
*/
}
//=========================================================================================
static int AddStandardBitmap(_LPTOOLBARDATA lptd,int id){
	return -1;
}
static int DoTB_AddBitmap(HWND hWnd,int nButtons, LPTBADDBITMAP  lptbab ){
	_LPTOOLBARDATA					lptd;
//	int								i;
//	HBITMAP							hBitmap;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	if((lptbab->hInst==IDB_STD_SMALL_COLOR)||(lptbab->hInst==IDB_VIEW_SMALL_COLOR)){
		return AddStandardBitmap(lptd,lptbab->hInst);
	}
/*	if(nButtons<lptbab->hImageList->count)
		return -1;
	for(i=0;i<lptd->hImgList->count;i++){
		hBitmap=(HBITMAP)PtrListAt(lptbab->hImageList,i);
		if(hBitmap==0)
			return -1;
		PtrListInsert(lptd->hImgList,hBitmap);
	}
	return lptd->hImgList->count-nButtons;
	*/
	//ImageList_Merge(lptd->hImgList,-1,lptbab->hImageList,0,16,16);
	return ImageList_GetImageCount(lptd->hImgList);
}
static BOOL GetNewTBButtonRect(HWND hWnd,RECT *rt,BYTE fsStyle){
	_LPTOOLBARDATA						lptd;
	_LPTOOLBARBUTTON					lptb;
	POINT								pt;
	RECT								parentrt;
	int									i,ileftMax=-1,irightMax=-1;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	for(i=0;i<lptd->lpButtonList->count;i++){
		if(fsStyle&BTNS_RIGHTBUTTON){
			irightMax=i;
		}else{
			ileftMax=i;
		}
	}
	if(fsStyle&BTNS_RIGHTBUTTON){
		if(irightMax==-1){
			GetClientRect(hWnd,rt);
			rt->right-=lptd->iIndent;
			return TRUE;
		}
		lptb=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,irightMax);
		ASSERT(lptb!=NULL);
		GetClientRect(hWnd,&parentrt);
		GetClientRect(lptb->hWnd,rt);
		pt.x=rt->right;
		pt.y=rt->top;
		ClientToScreen(lptb->hWnd,&pt);
		ScreenToClient(hWnd,&pt);
		rt->right=rt->left-lptd->cx;
		rt->left=parentrt.left;
	}else{
		if(ileftMax==-1){
			GetClientRect(hWnd,rt);
			rt->left+=lptd->iIndent;
			return TRUE;
		}
		lptb=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,ileftMax);
		ASSERT(lptb!=NULL);
		GetClientRect(hWnd,&parentrt);
		GetClientRect(lptb->hWnd,rt);
		pt.x=rt->left;
		pt.y=rt->top;
		ClientToScreen(lptb->hWnd,&pt);
		ScreenToClient(hWnd,&pt);
		rt->left=pt.x+(rt->right-rt->left)+lptd->cx;
		rt->right=parentrt.right;
	}
	return TRUE;
}
static BOOL DoTB_AddButton(HWND hWnd,int uNumButtons, LPTBBUTTON lpButtons){
	_LPTOOLBARDATA					lptd;
	int								i;
	RECT							rt;
	_LPTOOLBARBUTTON				lpButton;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	if(uNumButtons<1)
		return FALSE;
	if(lpButtons==NULL)
		return FALSE;
	for(i=0;i<uNumButtons;i++){
		lpButton=(_LPTOOLBARBUTTON)malloc(sizeof(_TOOLBARBUTTON));
		if(lpButton==NULL)
			return FALSE;
		memset( lpButton, 0, sizeof(_TOOLBARBUTTON) );
		GetNewTBButtonRect(hWnd,&rt,lpButtons[i].fsStyle);
		lpButton->hWnd=CreateTBButton(hWnd,&(lpButtons[i]),rt);
		if(lpButton->hWnd==0)
			return FALSE;
		lpButton->iCommand=lpButtons[i].idCommand;
		PtrListInsert(lptd->lpButtonList,lpButton);
	}
	DoTB_AutoSize(hWnd);
	return TRUE;
} 
static int DoTB_AddString(HWND hWnd,int iNum, TCHAR * szText){
	_LPTOOLBARDATA					lptd;
	TCHAR							*lpstr;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	if(szText==NULL)
		return -1;
	lpstr=(TCHAR*)malloc(sizeof(TCHAR)*(strlen(szText)+1));
	if(lpstr==NULL)
		return -1;
	strcpy(lpstr,szText);
	PtrListInsert(lptd->lpStringList,lpstr);
	return lptd->lpStringList->count-1;
}
static void DoTB_AutoSize(HWND hWnd){
	_LPTOOLBARDATA					lptd;
	int								i;
	_LPTOOLBARBUTTON				lpButton;
	RECT							rt,rrt;
	int								iHigh;
	_LPTBBUTTONDATA					lptb;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	GetClientRect(hWnd,&rt);
	rt.left+=lptd->iIndent;
	rt.top+=lptd->cy;
	rt.bottom-=lptd->cy;
	//===========================================================
	GetClientRect(hWnd,&rrt);
	rrt.right-=lptd->iIndent;
	rrt.top+=lptd->cy;
	rrt.bottom-=lptd->cy;
	//===========================================================
	iHigh=rt.bottom-rt.top;
	for(i=0;i<lptd->lpButtonList->count;i++){
		lpButton=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,i);
		lptb=(_LPTBBUTTONDATA)GetWindowLong(lpButton->hWnd, 0);
		ASSERT(lptb!=NULL);
		if(lptb->fsStyle&BTNS_RIGHTBUTTON){
			MoveWindow(lpButton->hWnd,rrt.right-lptb->cx,rrt.top,lptb->cx,iHigh,TRUE);
			rrt.right=rrt.right-lptb->cx-lptd->cx;
		}else{
			MoveWindow(lpButton->hWnd,rt.left,rt.top,lptb->cx,iHigh,TRUE);
			rt.left+=lptb->cx+lptd->cx;
		}
	}
}
static int DoTB_ButtonCount(HWND hWnd){
	_LPTOOLBARDATA					lptd;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	return lptd->lpButtonList->count;
}
static BOOL DoTB_ChangeBitmap(HWND hWnd,int idButton,WORD iBitmap){
	_LPTOOLBARDATA					lptd;
	_LPTOOLBARBUTTON				lpButton;
	TBBUTTONINFO					ti;
	int								i;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
//	if(iBitmap>=lptd->hImgList->count)
//		return FALSE;
	if(iBitmap>=ImageList_GetImageCount(lptd->hImgList))
		return FALSE;
	for(i=0;i<lptd->lpButtonList->count;i++){
		lpButton=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,i);
		ASSERT(lpButton!=NULL);
		if(lpButton->iCommand==idButton){
			ti.dwMask=TBIF_IMAGE;
			ti.iImage=iBitmap;
			SetTBButtonInfo(lpButton->hWnd,&ti);
			return TRUE;
		}
	}
	return FALSE;
}
static	BOOL SetToolBarButtonState(HWND hWnd,int idButton,DWORD dwMask,
								   BOOL isHave,DWORD State){
	_LPTOOLBARDATA					lptd;
	_LPTOOLBARBUTTON				lpButton;
	TBBUTTONINFO					ti;
	int								i;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	for(i=0;i<lptd->lpButtonList->count;i++){
		lpButton=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,i);
		ASSERT(lpButton!=NULL);
		if(lpButton->iCommand==idButton){
			_LPTBBUTTONDATA lpButtonData;
			lpButtonData=(_LPTBBUTTONDATA)GetWindowLong(lpButton->hWnd, 0);
			ti.dwMask=dwMask;

			if(isHave==TRUE)
			{
				//ti.fsState|=State; old
// !!! Add By Jami chen in 2004.09.18
				if (((BYTE)State & lpButtonData->fsState) == (BYTE)State)
				{  // 要设置的状态与原来的状态一样，不需要进行设置
					return TRUE; 
				}
// !!! Add End By Jami chen in 2004.09.18
//				ti.fsState=(BYTE)State;//new modified by tanphei
				ti.fsState= (BYTE)State | lpButtonData->fsState;//new modified by lilin - 2004-08-12
			}
			else
			{
// !!! Add By Jami chen in 2004.09.18
				if (((BYTE)State & lpButtonData->fsState) == 0)
				{  // 要设置的状态与原来的状态一样，不需要进行设置
					return TRUE; 
				}
// !!! Add End By Jami chen in 2004.09.18
				//ti.fsState &= ~State; old
//				ti.fsState=0;//modified by tanphei 2003,4.21
				ti.fsState = lpButtonData->fsState & (BYTE)(~State );//new modified by lilin - 2004-08-12
			}
			SetTBButtonInfo(lpButton->hWnd, &ti);
			return TRUE;
		}
	}
	return FALSE;
}
static BOOL DoTB_CheckButton(HWND hWnd,int idButton,BOOL fCheck){
	return SetToolBarButtonState(hWnd,idButton,TBIF_STATE,fCheck,TBSTATE_CHECKED);
}
static int DoTB_CommandToIndex(HWND hWnd,int idButton){
	_LPTOOLBARDATA					lptd;
	_LPTOOLBARBUTTON				lpButton;
	int								i;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	for(i=0;i<lptd->lpButtonList->count;i++){
		lpButton=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,i);
		ASSERT(lpButton!=NULL);
		if(lpButton->iCommand==idButton){
			return i;
		}
	}
	return -1;
}
static BOOL DoTB_DeleteButton(HWND hWnd,int iIndex){
	_LPTOOLBARDATA					lptd;
	_LPTOOLBARBUTTON				lpButton;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	if(iIndex>=lptd->lpButtonList->count)
		return FALSE;
	lpButton=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,iIndex);
	ASSERT(lpButton!=NULL);
	DestroyWindow(lpButton->hWnd);
	PtrListAtFree(lptd->lpButtonList,iIndex);
	PtrListPack(lptd->lpButtonList);
	DoTB_AutoSize(hWnd);
	return TRUE;
}
static BOOL DoTB_EnableButton(HWND hWnd,int idButton,BOOL fEnable){
	return SetToolBarButtonState(hWnd,idButton,TBIF_STATE,fEnable,TBSTATE_ENABLED);
}
static BOOL DoTB_HideButton(HWND hWnd,int idButton,BOOL fEnable)
{
    return SetToolBarButtonState(hWnd, idButton, TBIF_STATE, fEnable,TBSTATE_HIDDEN );
}
static int DoTB_GetBitmap(HWND hWnd,int idButton){
	_LPTOOLBARDATA					lptd;
	_LPTOOLBARBUTTON				lpButton;
	TBBUTTONINFO					ti;
	int								i;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	for(i=0;i<lptd->lpButtonList->count;i++){
		lpButton=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,i);
		ASSERT(lpButton!=NULL);
		if(lpButton->iCommand==idButton){
			ti.dwMask=TBIF_IMAGE;
			GetTBButtonInfo(lpButton->hWnd,&ti);
			return ti.iImage;
		}
	}
	return -1;
}
static BOOL DoTB_PressButton(HWND hWnd,int idButton,BOOL fPress){
	return SetToolBarButtonState(hWnd,idButton,TBIF_STATE,fPress,TBSTATE_ENABLED);
}
static int DoTB_SetButtonInfo(HWND hWnd,int idButton,LPTBBUTTONINFO lptbbi){
	_LPTOOLBARDATA					lptd;
	_LPTOOLBARBUTTON				lpButton;
	int								i;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	if(lptbbi==NULL)
		return FALSE;
	for(i=0;i<lptd->lpButtonList->count;i++){
		lpButton=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,i);
		ASSERT(lpButton!=NULL);
		if(lpButton->iCommand==idButton){
			SetTBButtonInfo(lpButton->hWnd,lptbbi);
			return 1;
		}
	}
	return 0;
}
static BOOL DoTB_SetCmdId(HWND hWnd,int iIndex,int idButton){
	_LPTOOLBARDATA					lptd;
	_LPTOOLBARBUTTON				lpButton;
	TBBUTTONINFO					ti;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd,0);
	ASSERT(lptd!=NULL);
	if(iIndex>=lptd->lpButtonList->count)
		return FALSE;
	lpButton=(_LPTOOLBARBUTTON)PtrListAt(lptd->lpButtonList,iIndex);
	ASSERT(lpButton!=NULL);
	ti.dwMask=TBIF_COMMAND;
	ti.idCommand=idButton;
	SetTBButtonInfo(lpButton->hWnd,&ti);
	return FALSE;
}
//static LPPTRLIST DoTBImageList(HWND hWnd,LPPTRLIST hImgeList,BOOL fImage){
static HIMAGELIST DoTBImageList(HWND hWnd,LPPTRLIST hImgeList,BOOL fImage)
{
	_LPTOOLBARDATA					lptd;
//	LPPTRLIST						hImge;
	HIMAGELIST						hImge;

	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);

	if(fImage==TRUE)
	{
		hImge=lptd->hImgList;
		lptd->hImgList=hImgeList;
		hImgeList = hImge ;
	}else
	{
		hImge=lptd->hDisImgList;
		lptd->hDisImgList=hImgeList ;
		hImgeList=hImge ;
	}
	return hImgeList;
}
//static LPPTRLIST DoTB_SetDisabledImageList(HWND hWnd,LPPTRLIST himlNewDisabled)
static HIMAGELIST DoTB_SetDisabledImageList(HWND hWnd,LPPTRLIST himlNewDisabled)
{
	return DoTBImageList(hWnd,himlNewDisabled,FALSE);
}
//static LPPTRLIST DoTB_SetImageList(HWND hWnd,LPPTRLIST  hImageList)
static HIMAGELIST DoTB_SetImageList(HWND hWnd,LPPTRLIST  hImageList)
{
	return DoTBImageList(hWnd,hImageList,TRUE);
}
static int  LoadSystemImage( HWND hWnd, int iID[], int iTotal )
{
	_LPTOOLBARDATA		lptd;
	HBITMAP				hBitmap ;
	int					i;
	UINT iCount ;
	lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
	ASSERT(lptd!=NULL);
	iCount = ImageList_GetImageCount(lptd->hImgList);
//	for( i = 0 ; i < lptd->hImgList->count ; i ++ )
	for( i = 0 ; i < (int)iCount ; i ++ )
	{
//		hBitmap = ( HBITMAP )PtrListAt( lptd->hImgList, i ) ;
		hBitmap = ( HBITMAP )ImageList_GetIcon(lptd->hImgList,i,0) ;
		ASSERT( hBitmap != NULL ) ;
		DeleteObject( hBitmap ) ;
	}
	for( i = 0 ; i < iTotal ; i ++ )
	{
		int			id ;
		id = iID[i] ;
		hBitmap = LoadImage( NULL, MAKEINTRESOURCE( id ), IMAGE_BITMAP,
			16, 16, 0 ) ;
		if( hBitmap != 0 )
//			PtrListInsert( lptd->hImgList, hBitmap ) ;
			ImageList_Add( lptd->hImgList, hBitmap ,NULL) ;
	}
//	return lptd->hImgList->count ;
	return ImageList_GetImageCount(lptd->hImgList);
}
static int DoTB_LoadImages( HWND hWnd, WPARAM iBitmapID, LPARAM hIns )
{
	switch( iBitmapID )
	{
	case IDB_HIST_LARGE_COLOR:
	case IDB_HIST_SMALL_COLOR:
		return LoadSystemImage( hWnd, HIST_ID, HINST_NUMBER ) ;
	case IDB_STD_LARGE_COLOR:
	case IDB_STD_SMALL_COLOR:
		return LoadSystemImage( hWnd, STD_ID, STD_NUMBER ) ;
	case IDB_VIEW_LARGE_COLOR:
	case IDB_VIEW_SMALL_COLOR:
		return LoadSystemImage( hWnd, VIEW_ID, VIEW_NUMBER ) ;
	}
	return 0 ;
}

// **************************************************
// 声明：static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPCTLCOLORSTRUCT，要设置的颜色值
// 返回值：无
// 功能描述：设置窗口颜色，处理WM_SETCTLCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if ( lParam )
	{		
		LPCTLCOLORSTRUCT lpCtlColor;
		_LPTOOLBARDATA		lptd;
		
		lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
		
		lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构指针		
		
		if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		{ // 设置文本背景色
			lptd->cl_Bk = lpCtlColor->cl_TextBk;
		}
		return TRUE;
	}
	return FALSE;
}


static LRESULT CALLBACK	ToolBarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message){
	case WM_COMMAND:
		DoToolBarCommand(hWnd,wParam,lParam);
		return 0;
	case WM_NOTIFY:
		DoToolBarNotify(hWnd,wParam,lParam);
		return 0;
	case WM_PAINT:
		DoToolBarPaint(hWnd);
		return 0;
	case WM_ERASEBKGND:
		DoToolBarEraseBkgnd(hWnd,(HDC)wParam);
		return 0;
//=================================================================================
	case TB_ADDBITMAP:
		return DoTB_AddBitmap(hWnd,(int)wParam,(LPTBADDBITMAP)lParam);
	case TB_ADDBUTTONS:
		return DoTB_AddButton(hWnd,(int)wParam,(LPTBBUTTON)lParam); 
	case TB_ADDSTRING:
		return DoTB_AddString(hWnd,(int)wParam,(TCHAR*)lParam);
	case TB_AUTOSIZE:
		DoTB_AutoSize(hWnd);
		return 0;
	case TB_BUTTONCOUNT:
		return DoTB_ButtonCount(hWnd);
	case TB_CHANGEBITMAP:
		return DoTB_ChangeBitmap(hWnd,(int)wParam,LOWORD(lParam));
	case TB_CHECKBUTTON:
		return DoTB_CheckButton(hWnd,(int)wParam,LOWORD(lParam));
	case TB_COMMANDTOINDEX:
		return DoTB_CommandToIndex(hWnd,(int)wParam);
	case TB_DELETEBUTTON:
		return DoTB_DeleteButton(hWnd,(int)wParam);
	case TB_ENABLEBUTTON:
		return DoTB_EnableButton(hWnd,(int)wParam,LOWORD(lParam));
	case TB_GETBITMAP:
		return DoTB_GetBitmap(hWnd,(int)wParam);
	case TB_HIDEBUTTON:
		return DoTB_HideButton(hWnd, (int)wParam, (BOOL)lParam );
//	case TB_GETBUTTON :
//		return DoTB_GetButton(hWnd,wParam,(TBBUTTON*)lParam);
//	case TB_GETBUTTONINFO:
//		return DoGetButtonInfo(hWnd,(INT)wParam,(LPTBBUTTONINFO)lParam);
//	case TB_GETBUTTONSIZE:
//		return DoGetButtonSize(hWnd);
//	case TB_GETBUTTONTEXT :
//		return DoGetButtonText(hWnd,wParam,(LPSTR)lParam);
//	case TB_GETDISABLEDIMAGELIST:
//		return DoGetDisabledImageList(hWnd);
//		static HANDLE DoGetDisabledImageList(HWND hWnd);
//	case TB_GETEXTENDEDSTYLE:
//		return DoGetExtendedStyle(hWnd);
//	case TB_GETIMAGELIST:
//		return (long)DoGetImageList(hWnd);
//	case TB_GETINSERTMARK:
//		return DoGetInsertMark(hWnd,(LPTBINSERTMARK)lParam);
//		static BOOL DoGetInsertMark(HWND hWnd,LPTBINSERTMARK lptbim);
//	case TB_GETITEMRECT :
//		return DoGetItemRect(hWnd,wParam,(LPRECT)lParam);
//	case TB_GETPADDING:
//		return DoGetPadding(hWnd);
//	case TB_GETRECT:
//		return DoGetRect(hWnd,(INT)wParam,(LPRECT)lParam,TRUE);
//	case TB_GETROWS:
//		return DoGetRows(hWnd);
//	case TB_GETSTATE :
//		return DoGetState(hWnd,wParam);
//	case TB_GETSTRING:
//		return DoGetString(hWnd,HIWORD (wParam) ,LOWORD (wParam) ,(LPTSTR) lParam);
//	case TB_GETSTYLE:
//		return DoGetStyle(hWnd);
//	case TB_GETTEXTROWS:
//		return DoGetTextRows(hWnd);
//	case TB_HIDEBUTTON :
//		return DoHideButton(hWnd,wParam,LOWORD(lParam));
//		static BOOL DoHideButton(HWND hWnd,DWORD idButton,BOOL fShow);
//	case TB_INDETERMINATE :
//		return DoIndeterminate(hWnd,wParam,LOWORD(lParam));
//	case TB_INSERTBUTTON :
//		return DoInsertButton(hWnd,wParam,(TBBUTTON*)lParam);
//	case TB_ISBUTTONCHECKED :
//		return DoIsButtonChecked(hWnd,wParam);
//	case TB_ISBUTTONENABLED :
//		return DoIsButtonEnabled(hWnd,wParam);
//	case TB_ISBUTTONHIDDEN :
//		return DoIsButtonHidden(hWnd,wParam);
//		static BOOL DoIsButtonHidden(HWND hWnd,DWORD idButton);
//	case TB_ISBUTTONPRESSED :
//		return DoIsButtonPressed(hWnd,wParam);
	case TB_PRESSBUTTON :
		return DoTB_PressButton(hWnd,(int)wParam,LOWORD(lParam));
//	case TB_SETBITMAPSIZE :
	case TB_SETBUTTONINFO:
		return DoTB_SetButtonInfo(hWnd,(int)wParam,(LPTBBUTTONINFO)lParam);
//	case TB_SETBUTTONSIZE :
//		return DoSetButtonSize(hWnd,LOWORD(lParam),HIWORD(lParam));
	case TB_SETCMDID :
		return DoTB_SetCmdId(hWnd,(UINT) wParam,(UINT)lParam);
	case TB_SETDISABLEDIMAGELIST:
		return (long)DoTB_SetDisabledImageList(hWnd,(LPPTRLIST)lParam);
	case TB_SETIMAGELIST:
		return (long)DoTB_SetImageList(hWnd,(LPPTRLIST)lParam);
//	case TB_SETINDENT:
//		return DoSetIndent(hWnd,(INT)wParam);
//	case TB_SETINSERTMARK:
//		DoSetInsertMark(hWnd,(LPTBINSERTMARK)lParam);
//		break;
//	case TB_SETPADDING:
//		return DoSetPadding(hWnd,LOWORD(lParam),HIWORD(lParam));
//	case TB_SETPARENT :
//		DoTB_SetParent(hWnd,(HWND)wParam);
//		break;
//	case TB_SETROWS :
//		DoTB_SetRows(hWnd,LOWORD(wParam),HIWORD(wParam),(LPRECT)lParam);
//		break;
//	case TB_SETSTATE :
//		return DoTB_SetState(hWnd,wParam,LOWORD(lParam));
//===============================================================================
//	case TB_SETSTYLE:
//		DoTB_SetStyle(hWnd,(DWORD)lParam);
//		return 0;
//	case TB_SETEXTENDEDSTYLE:
//		return (long)DoTB_SetExtendedStyle(hWnd,(DWORD)lParam);
//	case TB_BUTTONSTRUCTSIZE:
//		return DoButtonStructSize(hWnd,wParam);
//		static void DoButtonStructSize(HWND hWnd,DWORD cb);
//	case TB_GETBITMAPFLAGS :
//		return DoGetBitmapFlags(hWnd);
//		static DWORD DoGetBitmapFlags(hWnd);
//	case TB_GETCOLORSCHEME:
//		return DoGetColorScheme(hWnd,(LPCOLORSCHEME)lParam);
//		static BOOL DoGetColorScheme(HWND hWnd,LPCOLORSCHEME lpcs);
//	case TB_GETINSERTMARKCOLOR:
//		return DoGetInsertMarkColor(hWnd);
//		static COLORREF DoGetInsertMarkColor(HWND hWnd);
//	case TB_GETMAXSIZE:
//		return DoGetMaxSize(hWnd,(LPSIZE)lParam);
//	case TB_GETHOTIMAGELIST:
//		return DoGetHotImageList(hWnd);
//		static HANDLE DoGetHotImageList(HWND hWnd);
//	case TB_GETHOTITEM:
//		return DoGetHotItem(hWnd);
//		static int DoGetHotItem(HWND hWnd);
//	case TB_GETOBJECT:
//	case TB_GETTOOLTIPS:
//	case TB_GETUNICODEFORMAT:
//	case TB_HIGHLIGHTBUTTON:
//	case TB_HITTEST:
//		return DoHitTest(hWnd,(LPPOINT)lParam);
//		static int DoHitTest(HWND hWnd,LPPOINT pptHitTest);
//	case TB_INSERTMARKHITTEST:
//	case TB_ISBUTTONHIGHLIGHTED:
//	case TB_ISBUTTONINDETERMINATE :
	case TB_LOADIMAGES://===================================================
		return DoTB_LoadImages(hWnd,wParam, lParam);
//	case TB_MAPACCELERATOR:
//	case TB_MARKBUTTON:
//	case TB_MOVEBUTTON:
//	case TB_REPLACEBITMAP:
//	case TB_SAVERESTORE:
//	case TB_SETANCHORHIGHLIGHT:
//	case TB_SETBUTTONWIDTH:
//		return DoSetButtonWidth(hWnd,LOWORD(lParam),HIWORD(lParam));
//		static BOOL DoSetButtonWidth(HWND hWnd,WORD cxMin,WORD cxMax );
//	case TB_SETCOLORSCHEME:
//	case TB_SETDRAWTEXTFLAGS:
//		return DoSetDrawTextFlags(hWnd,(DWORD)wParam,(DWORD)lParam);
//	case TB_SETHOTIMAGELIST:
//	case TB_SETHOTITEM:
//		return DoSetHotItem(hWnd,(INT)wParam);
//		static int DoSetHotItem(HWND hWnd,INT iHot);
//	case TB_SETINSERTMARKCOLOR:
//	case TB_SETMAXTEXTROWS:
//		return DoSetMaxTextRows(hWnd,(INT) wParam);
//		static BOOL DoSetMaxTextRows(HWND hWnd,INT iMaxRows);
//	case TB_SETTOOLTIPS:
//	case TB_SETUNICODEFORMAT:
//	case TB_CUSTOMIZE:
//		return Customize(hWnd);
//		static void Customize(HWND hWnd);
//	case TB_GETANCHORHIGHLIGHT:
//		return DoGetAnchorHighlight(hWnd);
//		static BOOL DoGetAnchorHighlight(HWND hWnd);
//=============this message add by kingkong======================
//	case TB_ADDLEFTBMP:
//		DoAddLeftBmp(hWnd,(TBBUTTON*)lParam);
//		return 0;
//	case TB_MODIFYLEFTBMP:
//		DoModifyLeftBmp(hWnd,(TBBUTTON*)lParam);
//		return 0;
//	case TB_DELETELEFTBMP:
//		DoDeleteLeftBmp(hWnd,(TBBUTTON*)lParam);
//		return 0;
///===============end add message by kingkong=====================
	case WM_SETCTLCOLOR	: // 设置颜色
		return DoSetColor(hWnd,wParam,lParam);
// !!! Add By Jami chen in 2004.09.01
	case WM_SYSCOLORCHANGE:
		return DoSysColorChanged(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2004.09.01
	case WM_CREATE:
		if(OnToolBarCreate(hWnd)==FALSE)
			return 1;
		return 0;
	case WM_DESTROY:
		DoToolBarDestroy(hWnd);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
ATOM RegisterTOOLBARClass(HINSTANCE hInstance)
{
	WNDCLASS    wc;

	RegisterTBButtonClass(hInstance);  // 如果需要TOOLBAR ,则必须要求有TBButton

	wc.style			= CS_DBLCLKS;
	wc.lpfnWndProc		= (WNDPROC)ToolBarProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof( LPVOID );
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;//(HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classTOOLBAR;

	return RegisterClass(&wc);
}
HWND WINAPI CreateToolbarEx(HWND hwnd, DWORD ws, UINT wID, int nBitmaps,
                        HINSTANCE hBMInst, UINT wBMID, TBBUTTON* lpButtons,
                        int iNumButtons, int dxButton, int dyButton,
                        int dxBitmap, int dyBitmap, UINT uStructSize){
	HWND hToolbar;
	RECT rt;
	int i;
	_LPTOOLBARDATA  lptd;
	void *ptr;
	RegisterTOOLBARClass((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE));
//	RegisterTBButtonClass((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE));
	GetClientRect(hwnd,&rt);

	if(ws&CCS_TOP){
		hToolbar=CreateWindowEx(WS_EX_TOPMOST,classTOOLBAR,"",ws,rt.left,rt.top,rt.right-rt.left,TOOLBAR_HIGH,
						hwnd,NULL,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
	}else{
		hToolbar=CreateWindowEx(WS_EX_TOPMOST,classTOOLBAR,"",ws,rt.left,rt.bottom-TOOLBAR_HIGH,rt.right-rt.left,TOOLBAR_HIGH,
						hwnd,NULL,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
	}
	if(hToolbar==0){
		EdbgOutputDebugString("=====CreateWindow error:%d=========\r\n",GetLastError());
		return 0;
	}
	lptd=(_LPTOOLBARDATA)GetWindowLong(hToolbar, 0);
	for(i=0;i<iNumButtons;i++){
		ptr=malloc(sizeof(TBBUTTON));
		if(ptr==NULL)
			return FALSE;
		memcpy(ptr,&(lpButtons[i]),sizeof(TBBUTTON));
		PtrListInsert(lptd->lpButtonList,ptr);
	}
	lptd->dwStyle=ws;
	
	lptd->iIndent=INDENT_SPACE;//3;
	lptd->cx=X_SPACE;//3;
	lptd->cy=Y_SPACE;//2;

	lptd->dxButton=dxButton;
	lptd->dyButton=dyButton;
	lptd->dxBitmap=dxBitmap;
	lptd->dyBitmap=dyBitmap;
	SetWindowLong(hToolbar,GWL_ID,wID);
	return hToolbar;
}
HWND CreateCommonToolBar(
	HWND hWnd,
	PCOMMON_TOOLBAR_CREATESTRUCT pCreateStruct
)
{
	HWND hToolBar;
	int i;
	TBBUTTON TB;
	HBITMAP  hBitmap;
	HIMAGELIST hImageList;
	int        nIcoNum = (int)pCreateStruct->wButtonNum;
	DWORD dwValue = 0x80000000;
	HINSTANCE hInstanceTemp;
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	
	hToolBar=CreateToolbarEx(hWnd, CCS_TOP|WS_VISIBLE|WS_CHILD|TBSTYLE_FLAT|TBSTYLE_LIST,
		0, 0, NULL, 0, NULL, 0, 20, 20, 16, 16, 0);
	if(hToolBar==0){
		EdbgOutputDebugString("=========Create ToolBar is Failure===========\r\n");
		return NULL;
	}
	hImageList = ImageList_Create( 16, 16, ILC_COLOR4, nIcoNum, 2);
	if(hImageList == NULL){
		return NULL;
	}
	for (i = 0; i < nIcoNum; i++)
	{
		if ( (pCreateStruct->dwSystemIDMask << i) & dwValue)
			hInstanceTemp = 0;
		else
			hInstanceTemp = hInstance;
		hBitmap = (HBITMAP)LoadImage(hInstanceTemp, MAKEINTRESOURCE(pCreateStruct->pEnablePicIdAry[i]), IMAGE_BITMAP, 16, 16, 0);
		if (hBitmap == NULL)
		{
			ImageList_Destroy(hImageList);
			return NULL;
		}
		ImageList_Add(hImageList, hBitmap, 0);
	}
	SendMessage(hToolBar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);

	hImageList = ImageList_Create( 16, 16, ILC_COLOR4, nIcoNum, 2);
	for (i = 0; i < nIcoNum; i++)
	{
		if ( (pCreateStruct->dwSystemIDMask << i) & dwValue)
			hInstanceTemp = 0;
		else
			hInstanceTemp = hInstance;

		if ( pCreateStruct->pDisablePicIdAry[i] == 0 )
			hBitmap = NULL;
		else
		{
			hBitmap = (HBITMAP)LoadImage(hInstanceTemp, MAKEINTRESOURCE(pCreateStruct->pDisablePicIdAry[i]), IMAGE_BITMAP, 16, 16, 0);
			if (hBitmap == NULL)
			{
				ImageList_Destroy(hImageList);
				return NULL;
			}
		}
		ImageList_Add(hImageList, hBitmap, 0);
	}
	SendMessage(hToolBar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hImageList);
	

	for ( i = 0 ; i < nIcoNum ; i++)
	{
		memset(&TB, 0, sizeof(TB));
		TB.iString  = i;
		TB.iBitmap  = i;
		TB.idCommand = pCreateStruct->pCmdAry[i];
		TB.fsState = TBSTATE_ENABLED ;
		SendMessage(hToolBar, TB_ADDBUTTONS, 1, (LPARAM)&TB);
	}
	
	for (i = 0; i < nIcoNum; i ++)
	{
		if ( (pCreateStruct->dwDisableMask << i) & dwValue)
			SendMessage(hToolBar, TB_ENABLEBUTTON, (WPARAM)pCreateStruct->pCmdAry[i], MAKELONG(FALSE, 0));
	}
	
	return hToolBar;
}


HWND WINAPI CreateToolbarEx_Combo(HWND hwnd, DWORD ws, UINT wID, int nBitmaps,
                        HINSTANCE hBMInst, UINT wBMID, TBBUTTON* lpButtons,
                        int iNumButtons, int dxButton, int dyButton,
                        int dxBitmap, int dyBitmap, UINT uStructSize,LPRECT lprect)
{
	HWND hToolbar;
//	RECT rt;
	int i;
	_LPTOOLBARDATA  lptd;
	void *ptr;
	RegisterTOOLBARClass((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE));
//	RegisterTBButtonClass((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE));
//	GetClientRect(hwnd,&rt);

	if(ws&CCS_TOP){
		hToolbar=CreateWindowEx(WS_EX_TOPMOST,classTOOLBAR,"",ws,lprect->left,lprect->top,lprect->right-lprect->left,lprect->bottom - lprect->top,
						hwnd,NULL,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
	}else{
		hToolbar=CreateWindowEx(WS_EX_TOPMOST,classTOOLBAR,"",ws,lprect->left,lprect->bottom-TOOLBAR_HIGH,lprect->right-lprect->left,lprect->bottom - lprect->top,
						hwnd,NULL,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
	}
	if(hToolbar==0){
		EdbgOutputDebugString("=====CreateWindow error:%d=========\r\n",GetLastError());
		return 0;
	}
	lptd=(_LPTOOLBARDATA)GetWindowLong(hToolbar, 0);
	for(i=0;i<iNumButtons;i++){
		ptr=malloc(sizeof(TBBUTTON));
		if(ptr==NULL)
			return FALSE;
		memcpy(ptr,&(lpButtons[i]),sizeof(TBBUTTON));
		PtrListInsert(lptd->lpButtonList,ptr);
	}
	lptd->dwStyle=ws;
	//lptd->iIndent=3;
	//lptd->cx=3;
	//lptd->cy=2;
	lptd->iIndent=INDENT_SPACE;//3;
	lptd->cx=X_SPACE;//3;
	lptd->cy=Y_SPACE;//2;

	lptd->dxButton=dxButton;
	lptd->dyButton=dyButton;
	lptd->dxBitmap=dxBitmap;
	lptd->dyBitmap=dyBitmap;
	SetWindowLong(hToolbar,GWL_ID,wID);
	return hToolbar;
}


// !!! Add By Jami chen in 2004.09.01
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
	_LPTOOLBARDATA					lptd;

//		RETAILMSG(1,(TEXT("toolbar:DoSysColorChanged ...\r\n")));
		lptd=(_LPTOOLBARDATA)GetWindowLong(hWnd, 0);
		ASSERT(lptd!=NULL);

		lptd->cl_Bk = GetSysColor(COLOR_BACKGROUND);
		InvalidateRect(hWnd,NULL,TRUE);
//		RETAILMSG(1,(TEXT("toolbar:DoSysColorChanged OK\r\n")));
		return TRUE;
}
// !!! Add End By Jami chen in 2004.09.01
