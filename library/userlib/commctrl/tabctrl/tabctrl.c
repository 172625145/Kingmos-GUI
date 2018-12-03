/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����Tab�࣬ϵͳ�ಿ��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-23
���ߣ��½��� Jami chen
�޸ļ�¼��
		2004.5. 27 ��ӹ��� 
		TCS_EX_AUTOHIDETEXT -- �Զ������ı�
		TCS_EX_PLATSHOWTEXT -- ƽ����ʾ
		TCS_EX_AUTOSIZE -- �Զ�������Ŀ�ߴ�
******************************************************/
#include <eWindows.h>
#include <TabCtrl.h>

/***************  ȫ���� ���壬 ���� *****************/

#define IMAGE_WIDTH  24

#define COLOR_UNACTIVEBK COLOR_SCROLLBARTHUMB

static const char classTabCtrl[] = "TabControl";

typedef struct{
	LPTSTR lpText;
	UINT iImage;
	DWORD dwState;
	LPARAM lParam;
	UINT iWidth;
	HIMAGELIST hImageList ;
}TABITEMDATA,*LPTABITEMDATA;

typedef struct{
	HIMAGELIST hImageList;
	LPPTRLIST lpTabData;
	int iDisplayIndex;
	int iCurSel;
	UINT iRow;
	UINT iMaxRow;
	UINT *pRowOrder; // ȷ��һ����Ŀ��ʾ����һ�У���һ��

	BOOL bScrollArrow;

	COLORREF cl_Text;
	COLORREF cl_Bk;

	int iTabItemHeight; // 
	SIZE sizeIcon; //  ICON�ĸ߶�

	HBITMAP hBackGround; // ����λͼ
}TABCTRLDATA , *LPTABCTRLDATA;


#define LEFTARROW	0x0001
#define RIGHTARROW  0x0002

#define ARROWWIDTH	10
//#define lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/  26

#define NOTINITEM		-3
#define LEFTARROWITEM	-2
#define RIGHTARROWITEM	-1

//#define HIDEITEMWIDTH	40
#define HIDEITEMWIDTH	50
// ********************************************************************
// ��������
// ********************************************************************

ATOM RegisterTabCtrlClass(HINSTANCE hInstance);
static LRESULT CALLBACK TabCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemExtra(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoAdjustRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoRemoveImage(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetPadding(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetRowCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetMinTabWidth(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeselectAll(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoHighlight(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetExtenedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetExtendedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetBackGround(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight,int xOffset,int yOffset);

// *************************************************************************/
static void ScrollArrowNeed(HWND hWnd);

static void ShowArrow(HWND hWnd,HDC hdc,int cx,DWORD dwArrowStyle);
static void ShowItem(HWND hWnd,HDC hdc,LPTABITEMDATA lpTabItemData,int cx,int width,BOOL bCurSel);

static UINT GetItemWidth(HWND hWnd , LPTABITEMDATA lpTabItemData);
static UINT GetStringWidth(HWND hWnd,LPTSTR lpString);

static int GetCurrentItem(HWND hWnd,POINTS points);
static void ScrollTab(HWND hWnd,int iNewItem);
static void SetNewCurItem(HWND hWnd,int iNewItem);
static BOOL CanRightScroll(HWND hWnd);
static  LRESULT SendNormalNotify(HWND hWnd,UINT iCode);

static void AdjustItemSize(HWND hWnd,LPTABCTRLDATA lpTabCtrlData);

// ********************************************************************
// ������ATOM RegisterTabCtrlClass(HINSTANCE hInstance)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
// ����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
// ����������ע�ᵱǰӦ�ó������
// ����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
ATOM RegisterTabCtrlClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)TabCtrlWndProc; // TAB�ര�ڹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(LONG);
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
//    wc.hbrBackground = GetSysColorBrush( COLOR_BTNFACE );//GetStockObject( LTGRAY_BRUSH );
//    wc.hbrBackground = GetStockObject( LTGRAY_BRUSH );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classTabCtrl; // TAB����

	return RegisterClass(&wc); // ע�ᴰ��
}


// ********************************************************************
// ������static LRESULT CALLBACK TabCtrlWndProc(HWND , UINT , WPARAM , LPARAM )
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��ͬ����Ϣ�в�ͬ�ķ���ֵ�����忴��Ϣ����
// �����������ļ�����ര�ڹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK TabCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // ���ƴ���
			hdc = BeginPaint(hWnd, &ps);
			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_ERASEBKGND: // ɾ������
			OnEraseBkgnd( hWnd,( HDC )wParam );
			return 0;
		case WM_COMMAND: // ��������
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_LBUTTONDOWN: // ����������
			return DoLButtonDown(hWnd,wParam,lParam);
		case WM_CREATE: // ���ڴ���
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // �����ƻ�
			return DoDestroyWindow(hWnd);
		case WM_SETCTLCOLOR	: // ������ɫ
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // �õ��ؼ���ɫ
			return DoGetColor(hWnd,wParam,lParam);
// !!! Add By Jami chen in 2004.07.19
		case WM_SYSCOLORCHANGE:
			return DoSysColorChanged(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2004.07.19

		case TCM_GETITEM: // �õ���Ŀ
			return DoGetItem(hWnd,wParam,lParam);
		case TCM_SETITEM: // ������Ŀ
			return DoSetItem(hWnd,wParam,lParam);
		case TCM_INSERTITEM: // ��TAB�ؼ��в���һ����Ŀ
			return DoInsertItem(hWnd,wParam,lParam);
		case TCM_DELETEITEM: // ɾ����Ŀ
			return DoDeleteItem(hWnd,wParam,lParam);
		case TCM_DELETEALLITEMS: // ɾ�����е���Ŀ
			return DoDeleteAllItems(hWnd,wParam,lParam);
		case TCM_GETITEMRECT: // �õ���Ŀ����
			return DoGetItemRect(hWnd,wParam,lParam);
		case TCM_GETCURSEL: // �õ���ǰѡ��
			return DoGetCurSel(hWnd,wParam,lParam);
		case TCM_SETCURSEL: // ���õ�ǰѡ��
			return DoSetCurSel(hWnd,wParam,lParam);
		case TCM_HITTEST: // ���Ե�
			return DoHitTest(hWnd,wParam,lParam);
		case TCM_SETITEMEXTRA: // ������Ŀ��չ����
			return DoSetItemExtra(hWnd,wParam,lParam);
		case TCM_ADJUSTRECT:// ��������
			return DoAdjustRect(hWnd,wParam,lParam);
		case TCM_SETITEMSIZE:// ������Ŀ�ߴ�
			return DoSetItemSize(hWnd,wParam,lParam);
		case TCM_REMOVEIMAGE:// ɾ��ͼ��
			return DoRemoveImage(hWnd,wParam,lParam);
		case TCM_SETPADDING: // ���ÿհ�
			return DoSetPadding(hWnd,wParam,lParam);
		case TCM_GETROWCOUNT:// �õ��е�����
			return DoGetRowCount(hWnd,wParam,lParam);
		case TCM_GETTOOLTIPS: // �õ�������ʾ
				return DoGetToolTips(hWnd,wParam,lParam);
		case TCM_SETTOOLTIPS: // ���ù�����ʾ
			return DoSetToolTips(hWnd,wParam,lParam);
		case TCM_GETCURFOCUS:// �õ���ǰ����
			return DoGetCurFocus(hWnd,wParam,lParam);
		case TCM_SETCURFOCUS:// ���õ�ǰ����
			return DoSetCurFocus(hWnd,wParam,lParam);
		case TCM_SETMINTABWIDTH: // ������С��TAB���
			return DoSetMinTabWidth(hWnd,wParam,lParam);
		case TCM_DESELECTALL:// �������ѡ��
			return DoDeselectAll(hWnd,wParam,lParam);
		case TCM_HIGHLIGHTITEM:// ������ʾ��Ŀ
			return DoHighlight(hWnd,wParam,lParam);
		case TCM_SETEXTENDEDSTYLE:// ������չ���
			return DoSetExtenedStyle(hWnd,wParam,lParam);
		case TCM_GETEXTENDEDSTYLE: // �õ���չ���
			return DoGetExtendedStyle(hWnd,wParam,lParam);
		case TCM_SETIMAGELIST: //����ͼ���б�
			return DoSetImageList(hWnd,wParam,lParam);

		case TCM_SETBACKGROUND: //����ͼ���б�
			return DoSetBackGround(hWnd,wParam,lParam);


		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}



// ********************************************************************
// ������static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	����0���������������ڣ�����-1������ƻ�����
// ����������Ӧ�ó���������Ϣ
// ����: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rect;	
	LPTABCTRLDATA lpTabCtrlData;

	lpTabCtrlData = (LPTABCTRLDATA)malloc(sizeof(TABCTRLDATA)); // ����TAB�ṹ
	if (lpTabCtrlData == NULL)
		return -1; // ���ܴ����ô���

	// ��ʼ���ṹ
	lpTabCtrlData->hImageList = NULL;
	lpTabCtrlData->lpTabData = (LPPTRLIST)malloc(sizeof(PTRLIST)); // ����TAB�����б�ṹ
	if (lpTabCtrlData->lpTabData == NULL)
		return -1;
	PtrListCreate(lpTabCtrlData->lpTabData, 8, 8 ) ; // ���������б�
	lpTabCtrlData->iDisplayIndex = 0;
	lpTabCtrlData->iCurSel = -1;
	lpTabCtrlData->iRow = 0;
	lpTabCtrlData->iMaxRow = 4;
	lpTabCtrlData->pRowOrder = (LPUINT)malloc(sizeof(UINT) * lpTabCtrlData->iMaxRow);  // �����д�������
	memset(lpTabCtrlData->pRowOrder,0,sizeof(UINT) * lpTabCtrlData->iMaxRow); // ��ʼ������

	lpTabCtrlData->bScrollArrow = FALSE;

	// ����TAB��ɫ
	lpTabCtrlData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);; 
//	lpTabCtrlData->cl_Bk  = RGB(0xC0,0xC0,0xC0);//GetSysColor(COLOR_UNACTIVEBK);
	lpTabCtrlData->cl_Bk  = GetSysColor(COLOR_UNACTIVEBK);

	GetClientRect(hWnd,&rect);
	lpTabCtrlData->iTabItemHeight = rect.bottom - rect.top;

	lpTabCtrlData->sizeIcon.cx = 16;  //����Ĭ��ICON�Ĵ�С
	lpTabCtrlData->sizeIcon.cy = 16;

	lpTabCtrlData->hBackGround = NULL;


	SetWindowLong(hWnd,0,(DWORD)lpTabCtrlData);
	return 0;
}
// ********************************************************************
// ������static LRESULT DoDestroyWindow(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	����TRUE���������ƻ����ڣ�����FALSE�����ƻ�����
// ����������Ӧ�ó������ƻ����ڵ���Ϣ
// ����: 
// ********************************************************************
static LRESULT DoDestroyWindow(HWND hWnd)
{
	LPTABCTRLDATA lpTabCtrlData;
	lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
	ASSERT(lpTabCtrlData);

	if (lpTabCtrlData->hImageList != NULL)
	{
		// ԭ���Ѿ�����ͼ���б�
		ImageList_Destroy(lpTabCtrlData->hImageList);
	}

	free(lpTabCtrlData);
	SetWindowLong(hWnd,0,0);
	return TRUE;
}

// ********************************************************************
// ������static LRESULT DoPaint(HWND hWnd,HDC hdc)
// ������
//	IN hWnd - ���ھ��
//    IN hdc - �豸���
// ����ֵ��
//	
// ����������WM_PAINT �������
// ����: 
// ********************************************************************
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	DWORD dwStyle,dwExStyle;
	int cx = 0,width =0,iBankWidth = 0;
	int iIndex;
	RECT rect;
	HPEN hPen;
	HBRUSH hBrush;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);

		SetTextColor(hdc,lpTabCtrlData->cl_Text); // �����ı���ɫ
		SetBkColor(hdc,lpTabCtrlData->cl_Bk); // ���ñ�����ɫ
//		hPen = CreatePen(PS_SOLID,1,lpTabCtrlData->cl_Text);
		hPen = CreatePen(PS_SOLID,1,RGB(100,100,100));
		hPen = (HPEN)SelectObject(hdc,hPen);
		hBrush = CreateSolidBrush(lpTabCtrlData->cl_Bk);
		hBrush = (HBRUSH)SelectObject(hdc,hBrush);

		GetClientRect(hWnd,&rect); // �õ����ڿͻ�����
		iBankWidth = rect.right - rect.left;
		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		
		if (dwStyle & TCS_MULTILINE)
		{// ������ʾ����
		}
		else
		{// ֻ����ʾһ��
			MoveToEx(hdc,0,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/,NULL);
			LineTo(hdc,iBankWidth,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/);
			
			MoveToEx(hdc,0,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/+1,NULL);
			LineTo(hdc,iBankWidth,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/+1);
			
			iIndex = lpTabCtrlData->iDisplayIndex;  // �õ���ʼ��ʾ������
			if (lpTabCtrlData->bScrollArrow)
			{
				// ���Ǵӵ�һ����ʼ��ʾ��Ҫ��ʾһ��������������ļ�ͷ
				ShowArrow(hWnd,hdc,cx,LEFTARROW);
				cx += ARROWWIDTH;
				iBankWidth -= 2 * ARROWWIDTH;  //�ɹ���Ŀ��ʾ�Ŀ�ȱ����ȥ���Ҽ�ͷ�Ŀ��
			}
			while(1)
			{ // ��ʾÿһ����Ŀ
				lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);
				if (lpTabItemData == NULL)
					break; //�Ѿ�û������
				if (dwExStyle & TCS_EX_AUTOHIDETEXT)
				{ // �����Զ������ı�
					if (iIndex == lpTabCtrlData->iCurSel)
						width = iBankWidth - HIDEITEMWIDTH * (lpTabCtrlData->lpTabData->count - 1);
					else
						width = HIDEITEMWIDTH;
				}
				else
				{
					width = lpTabItemData->iWidth;
				}
				
				if (lpTabCtrlData->bScrollArrow == TRUE)
				{  
					if (width > iBankWidth)
					{  //����������ʾ���һ����Ŀ
						width = iBankWidth;
					}
				}
				if (width < 0)
				{	// ������Ŀû��ʾ����Ҫ�Ҽ�ͷ
					break;
				}
				if (iIndex == lpTabCtrlData->iCurSel)
					ShowItem(hWnd,hdc,lpTabItemData,cx,width,TRUE);
				else
					ShowItem(hWnd,hdc,lpTabItemData,cx,width,FALSE);
				cx += width;
				if (!(dwExStyle & TCS_EX_AUTOHIDETEXT))
				{
					iBankWidth -=width;
				}
				iIndex ++ ;
				if (iIndex >=lpTabCtrlData->lpTabData->count)
					break;
			}
			if (lpTabCtrlData->bScrollArrow == TRUE)
			{  // ��Ҫ��ʾ�Ҽ�ͷ
				ShowArrow(hWnd,hdc,cx,RIGHTARROW);
			}
		}
		hPen = (HPEN)SelectObject(hdc,hPen); // �ָ�ԭ����
		DeleteObject(hPen); // ɾ����
		//hBrush = CreateSolidBrush(lpTabCtrlData->cl_Bk);
		hBrush = (HBRUSH)SelectObject(hdc,hBrush); // �ָ�ԭ���Ļ�ˢ
		DeleteObject(hBrush); // ɾ��ˢ��
		return 0;
}

// ********************************************************************
// ������static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc)
// ������
//	IN hWnd - ���ھ��
//    IN hdc - �豸���
// ����ֵ��
//	
// ����������WM_ERASEBKGND �������
// ����: 
// ********************************************************************
static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc)
{
	LPTABCTRLDATA lpTabCtrlData;
	HBRUSH hBrush;
	RECT rect;
		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0);  // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);

		hBrush = CreateSolidBrush(lpTabCtrlData->cl_Bk); // �õ�����ˢ
		if (hBrush == NULL)
			return 0;
		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		FillRect(hdc,&rect,hBrush); // ���ͻ�����
		DeleteObject(hBrush); // ɾ����ˢ
		return 0;
}

// **************************************************
// ������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(x,y),��ǰ����λ��
// ����ֵ����
// ��������������WM_LBUTTONDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINTS points;
	int iNewItem;

		// �õ����λ��
		points.x=LOWORD(lParam);
		points.y=HIWORD(lParam);

		iNewItem = GetCurrentItem(hWnd,points); // �õ������ڵ���Ŀ
		switch(iNewItem)
		{
			case NOTINITEM:
				return 0; // û�е����κ���Ŀ��
			case LEFTARROWITEM:
			case RIGHTARROWITEM:// �������Ҽ�ͷ��
				ScrollTab(hWnd,iNewItem); // ����TAB
				break;
			default: // ������Ŀ��
				SetNewCurItem(hWnd,iNewItem); // �����µ���ĿΪ��ǰѡ����Ŀ
				break;
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTLCOLORSTRUCT��Ҫ���õ���ɫֵ
// ����ֵ����
// �������������ô�����ɫ������WM_SETCTLCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);
		
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹָ��

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �����ı���ɫ
			 lpTabCtrlData->cl_Text = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �����ı�����ɫ
			 lpTabCtrlData->cl_Bk = lpCtlColor->cl_TextBk;
		 }
		return TRUE;
}
// **************************************************
// ������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTLCOLORSTRUCT����ŵõ�����ɫֵ
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰ���ڵ���ɫ������WM_GETCTLCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹָ��

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �õ��ı���ɫ
			 lpCtlColor->cl_Text = lpTabCtrlData->cl_Text ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �õ��ı�����ɫ
				lpCtlColor->cl_TextBk = lpTabCtrlData->cl_Bk ;
		 }
		 return TRUE;
}

// !!! Add By Jami chen in 2004.07.19
// **************************************************
// ������static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�� ��
// ����������ϵͳ��ɫ�ı䣬����WM_SYSCOLORHANGED��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);

		lpTabCtrlData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);; 
//		lpTabCtrlData->cl_Bk  = RGB(0xC0,0xC0,0xC0);//GetSysColor(COLOR_STATIC);
		lpTabCtrlData->cl_Bk  = GetSysColor(COLOR_UNACTIVEBK);
	
		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
// !!! Add End By Jami chen in 2004.07.19

// **************************************************
// ������static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �����������õ���Ŀ������TCM_GETITEM��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ������Ŀ������TCM_SETITEM��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// ********************************************************************
// ������static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//    IN wParam = (WPARAM) (int) iItem; ָ��Ҫ�����λ�á�
//    IN lParam = (LPARAM) (const LPTCITEM) pitem;  Ҫ������Ŀ�����ݡ�
// ����ֵ��
//	�ɹ����ز�����Ŀ������ֵ�����򷵻�-1��
// ������������TAB�ؼ��в���һ����Ŀ������TCM_INSERTITEM��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	LPTCITEM lpTCItem;
	int iItem;

		lpTCItem = (LPTCITEM)lParam; // �õ���Ŀ�ṹ����
		if (lpTCItem == NULL)
			return -1;
		iItem = (int)wParam;
		if (iItem < 0)
			return -1;
		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);

		lpTabItemData = (LPTABITEMDATA)malloc(sizeof(TABITEMDATA)); // ����һ����Ŀ����
		if (lpTabItemData == NULL)
			return -1;
		
		if (lpTCItem->mask & TCIF_TEXT)
		{ // �ṹ�ı���Ч
			lpTabItemData->lpText = (LPTSTR)malloc(strlen(lpTCItem->pszText) + 1); // ����һ������������ı�
			strcpy(lpTabItemData->lpText,lpTCItem->pszText); // �����ı�
		}
		else
		{
			lpTabItemData->lpText = NULL; // û���ı�
		}
		
		if (lpTCItem->mask & TCIF_IMAGE)
		{ // ͼ����Ч
			lpTabItemData->iImage = lpTCItem->iImage; // ����ͼ��
		}
		else
		{ // û��ͼ��
			lpTabItemData->iImage = -1;
		}
		if (lpTCItem->mask & TCIF_PARAM)
		{ // ������Ч
			lpTabItemData->lParam = lpTCItem->lParam; // ���ò���
		}
		else
		{ // û�в���
			lpTabItemData->lParam = 0;
		}
		lpTabItemData->dwState = 0;

		lpTabItemData->iWidth = GetItemWidth(hWnd,lpTabItemData); // �õ���Ŀ���
//		RETAILMSG(1,("The Tab item < %s> Width = %d \r\n",lpTabItemData->lpText,lpTabItemData->iWidth));

		if (iItem > lpTabCtrlData->lpTabData->count)
			iItem = lpTabCtrlData->lpTabData->count;
//		RETAILMSG(1,("Insert %d Data \r\n",iItem));
		if (PtrListAtInsert(lpTabCtrlData->lpTabData,iItem,lpTabItemData) ==TRUE) // ������Ŀ
//		if (PtrListInsert(lpTabCtrlData->lpTabData,lpTabItemData) ==TRUE)
		{
			// ����ɹ�
			DWORD dwExStyle;
//			RETAILMSG(1,("Insert success \r\n"));
			if (lpTabCtrlData->iCurSel == -1) // û��ѡ����Ŀ
				lpTabCtrlData->iCurSel = iItem; // ���õ�ǰ��ĿΪѡ����Ŀ
			// !!! Add By Jami chen for adjust the item width

			dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
			if (TCS_EX_AUTOSIZE & dwExStyle)
			{
				// ����TAB��Ŀ�Ŀ��
				AdjustItemSize(hWnd,lpTabCtrlData);
			}
			// !!! Add End
			ScrollArrowNeed(hWnd);// �趨�Ƿ���Ҫ������ͷ
			return iItem; // ���ز�����Ŀ��λ��
		}
//		RETAILMSG(1,("Insert Failure \r\n"));
		return -1;
}
// **************************************************
// ������static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ����������ɾ����Ŀ������TCM_DELETEITEM��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ����������ɾ��������Ŀ������TCM_DELETEALLITEMS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� �õ���Ŀ���� �� ����TCM_GETITEMRECT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ�����ص�ǰ��ѡ��
// ���������� �õ���ǰѡ�񣬴���TCM_GETCURSEL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);
		return lpTabCtrlData->iCurSel; // ���ص�ǰ��ѡ��
}
// **************************************************
// ������static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �������������õ�ǰѡ�񣬴���TCM_SETCURSEL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iItem;

		iItem = (int)wParam;  // �õ��µ���Ŀ
		SetNewCurItem(hWnd,iItem); // �����µ���ĿΪ��ǰѡ����Ŀ
		return 0;
}
// **************************************************
// ������static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ���Ե㣬����TCM_HITTEST��Ϣ��
// ����: 
// **************************************************
static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoSetItemExtra(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ������Ŀ��չ���ݣ�����TCM_SETITEMEXTRA��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetItemExtra(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoAdjustRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� �������Σ�����TCM_ADJUSTRECT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoAdjustRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoSetItemSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ������Ŀ�ߴ磬����TCM_SETITEMSIZE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetItemSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoRemoveImage(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ����������ɾ��ͼ�󣬴���TCM_REMOVEIMAGE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoRemoveImage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoSetPadding(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ���ÿհף�����TCM_SETPADDING��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetPadding(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoGetRowCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� �õ��е�����������TCM_GETROWCOUNT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetRowCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �����������õ�������ʾ������TCM_GETTOOLTIPS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ���ù�����ʾ������TCM_SETTOOLTIPS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoGetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �����������õ���ǰ���㣬����TCM_GETCURFOCUS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoSetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ���õ�ǰ���㣬����TCM_SETCURFOCUS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetCurFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoSetMinTabWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ������С��TAB��ȣ�����TCM_SETMINTABWIDTH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetMinTabWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoDeselectAll(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� �������ѡ�񣬴���TCM_DESELECTALL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDeselectAll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoHighlight(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ������ʾ��Ŀ������TCM_HIGHLIGHTITEM��Ϣ��
// ����: 
// **************************************************
static LRESULT DoHighlight(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoSetExtenedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ������չ��񣬴���TCM_SETEXTENDEDSTYLE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetExtenedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
// **************************************************
// ������static LRESULT DoGetExtendedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� �õ���չ��񣬴���TCM_GETEXTENDEDSTYLE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetExtendedStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


// **************************************************
// ������static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- (HIMAGELIST) himl ͼ���б���
// 
// ����ֵ����
// ���������� �õ���չ��񣬴���TCM_GETEXTENDEDSTYLE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;
	HIMAGELIST hOldImageList;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);
		
		hOldImageList = lpTabCtrlData->hImageList; // �õ��ɵ�ͼ���б�

		lpTabCtrlData->hImageList = (HIMAGELIST)lParam; // �����µ�ͼ���б�

		ImageList_GetIconSize(lpTabCtrlData->hImageList,&lpTabCtrlData->sizeIcon.cx,&lpTabCtrlData->sizeIcon.cy); // �õ�ICON�Ĵ�С


		return (LRESULT)hOldImageList; //���ؾɵ�ͼ���б�
}



// **************************************************
// ������static LRESULT DoSetBackGround(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- (HIMAGELIST) himl ͼ���б���
// 
// ����ֵ����
// ���������� �õ���չ��񣬴���TCM_GETEXTENDEDSTYLE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetBackGround(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTABCTRLDATA lpTabCtrlData;
	HBITMAP hBackGround,hOldBackGround;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);

		hOldBackGround = lpTabCtrlData->hBackGround; // �õ�ԭ���ı���λͼ

		hBackGround = (HBITMAP)lParam; // �õ��µı���λͼ
	
		lpTabCtrlData->hBackGround = hBackGround; // �����µı���λͼ
	
		return (LRESULT)hOldBackGround; //����ԭ���ı���λͼ
}

// **************************************************
// ������static void ShowArrow(HWND hWnd,HDC hdc,int cx,DWORD dwArrowStyle)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN cx -- ��ͷλ��
// 	IN dwArrowStyle -- ��ͷ���
// 
// ����ֵ����
// ������������ʾ��ͷ��
// ����: 
// **************************************************
static void ShowArrow(HWND hWnd,HDC hdc,int cx,DWORD dwArrowStyle)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);

		if (dwArrowStyle == LEFTARROW)
		{ // ��ʾ���ͷ
			Rectangle(hdc,cx,0,cx+ARROWWIDTH,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/); // ���Ƽ�ͷ����
			TextOut(hdc,cx+1,1,"<",1); // ��ʾ��ͷ
		}
		else if (dwArrowStyle == RIGHTARROW)
		{ // ��ʾ�Ҽ�ͷ
			Rectangle(hdc,cx,0,cx+ARROWWIDTH,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/); // ���Ƽ�ͷ����
			TextOut(hdc,cx+1,1,">",1); // ��ʾ��ͷ
		}
}

// **************************************************
// ������static void ShowItem(HWND hWnd,HDC hdc,LPTABITEMDATA lpTabItemData,int cx,int width,BOOL bCurSel)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpTabItemData -- ��Ŀ���ݽṹ
// 	IN cx -- ��Ŀλ��
// 	IN width --��Ŀ���
// 	IN bCurSel -- �Ƿ��ǵ�ǰ��Ŀ
// 
// ����ֵ����
// ������������ʾ��Ŀ��
// ����: 
// **************************************************
static void ShowItem(HWND hWnd,HDC hdc,LPTABITEMDATA lpTabItemData,int cx,int width,BOOL bCurSel)
{
	RECT rect;
	HBRUSH hBrush;
	LPTABCTRLDATA lpTabCtrlData;
	int iTop = 0;
	DWORD dwExStyle,dwStyle;
	COLORREF cl_Bk;
	HICON hIcon;
	int iItemWidth; //�����ݵ���Ŀ���
	UINT uFormat;
	

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ

		ASSERT(lpTabCtrlData);		
		dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		if (dwExStyle & TCS_EX_PLATSHOWTEXT)
		{ // �����Զ������ı�

			if (bCurSel)
//				cl_Bk = RGB(255,255,255); // ��ѡ����Ŀ
				cl_Bk = GetSysColor(COLOR_WINDOW); // ��ѡ����Ŀ
			else
				cl_Bk = lpTabCtrlData->cl_Bk;  // ����ѡ����Ŀ

			// Add By Jami chen in 2004.08.14
			if (cx > 0)
			{
				cx --; // ��߽���ǰһ����Ŀ���ұ߽��غ�
				width ++; 
			}
			// Add End By Jami chen in 2004.08.14

			// ���ƾ��ο�
			Rectangle(hdc,cx,iTop,cx+width,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/); // ����Բ�Ǿ���
			// �õ���ǰ��Ŀ�ľ���

			rect.left = cx + 1;
			rect.right = cx + width - 2;
			rect.top = iTop + 1;
			rect.bottom = rect.top + lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/ - 2;

			if (lpTabCtrlData->hBackGround && bCurSel)
			{
				DrawBitmap(hdc,lpTabCtrlData->hBackGround,rect.left,iTop,width-2,lpTabCtrlData->iTabItemHeight-1,rect.left,iTop);
				SetBkMode(hdc,TRANSPARENT);
			}
			else
			{
				hBrush = CreateSolidBrush(cl_Bk);
				FillRect(hdc,&rect,hBrush); // �����Ŀ�²�������
				DeleteObject(hBrush);
			}

			// ����Ҫ��ʾ�����ݵ�λ��
			if (dwStyle & (TCS_FORCEICONLEFT | TCS_FORCELABELLEFT))
			{  //ICON ��ʾ�����
//				rect.left = cx + 1;
//				rect.right = cx + width - 2;
			}
			else
			{  // ICON ���ı���ʾ������
				if (dwExStyle & TCS_EX_AUTOHIDETEXT)
				{// �����ı���������ʾͼ��
					if (bCurSel)
						iItemWidth = GetItemWidth(hWnd,lpTabItemData); // �õ���Ŀ���ݵĿ��
					else
						iItemWidth = lpTabCtrlData->sizeIcon.cx; // ����ʾͼ��

					rect.left += (width - iItemWidth)/2; // �������赽����
					rect.right = rect.left + iItemWidth;
				}
				else
				{
					iItemWidth = GetItemWidth(hWnd,lpTabItemData); // �õ���Ŀ���ݵĿ��
					rect.left += (lpTabItemData->iWidth- iItemWidth)/2; // ���ı��赽����
					rect.right = rect.left + iItemWidth - 2;
				}
			}

			if (lpTabItemData->iImage != -1)
			{
				if (lpTabCtrlData->hImageList)
				{
					int cx,cy;

					hIcon = ImageList_GetIcon(lpTabCtrlData->hImageList,lpTabItemData->iImage,0); // װ��ָ����ͼ��
					if (hIcon == NULL)
						return ;

					// ����ICON����ʾλ��
					cx = rect.left;
					cy = ((rect.bottom - rect.top ) - lpTabCtrlData->sizeIcon.cy) / 2 + rect.top; // �ڴ�ֱλ���Ͻ�ICON��ʾ�ڿؼ����м�

					DrawIconEx(hdc,
							cx,
							cy,
							hIcon,
							0,
							0,
							0,
							0,
							DI_NORMAL);
					
					rect.left += lpTabCtrlData->sizeIcon.cx + 1; // ����Ҫ�м��
				}
			}

			// ��ʼ��ʾ�ı�
			if (dwStyle & (TCS_FORCEICONLEFT))
			{
				// Ҫ���ı���ʾ������
				uFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER ;
			}
			else
			{
				// Ҫ���ı�������ICON��ʾ
				uFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER ;
			}
			if (bCurSel)
			{ // ����ǵ�ǰ��Ŀ������ʾ�ı��������Ѿ������ı�
				if (lpTabItemData->lpText)
				{
					cl_Bk = SetBkColor(hdc,cl_Bk); // �����µı�����ɫ
					//TextOut(hdc,cx+1,iTop+2,lpTabItemData->lpText,strlen(lpTabItemData->lpText)); // ������Ŀ�ı�
					DrawText(hdc, lpTabItemData->lpText,strlen(lpTabItemData->lpText), &rect, uFormat);//������Ŀ�ı�
					SetBkColor(hdc,cl_Bk); // �ָ��ɵı�����ɫ
				}
			}
			else
			{
				//����ѡ����Ŀ
				if (!(dwExStyle & TCS_EX_AUTOHIDETEXT))
				{
					//����������ı�������ʾ�ı���������ʾ�ı�
					if (lpTabItemData->lpText)
					{
						cl_Bk = SetBkColor(hdc,cl_Bk); // �����µı�����ɫ
						//TextOut(hdc,cx+1,iTop+2,lpTabItemData->lpText,strlen(lpTabItemData->lpText)); // ������Ŀ�ı�
						DrawText(hdc, lpTabItemData->lpText,strlen(lpTabItemData->lpText), &rect, uFormat);//������Ŀ�ı�
						SetBkColor(hdc,cl_Bk); // �ָ��ɵı�����ɫ
					}
				}
			}
		}
		else
		{

			if (bCurSel)
			{ // �ǵ�ǰѡ�񣬻���ʾ�ıȽϸ�
				iTop = 2;
			}
			else
			{ // ��һ����Ŀ
				iTop = 4;
			}
	//		RoundRect(hdc,cx,0,cx+width,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/+4,8,8);
			RoundRect(hdc,cx,iTop,cx+width,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/+4,8,8); // ����Բ�Ǿ���
			rect.left = cx;
			rect.top = lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/;
			rect.right = cx + width;
			rect.bottom = lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/ + 4;

			hBrush = CreateSolidBrush(lpTabCtrlData->cl_Bk);
			FillRect(hdc,&rect,hBrush); // �����Ŀ�²�������
			DeleteObject(hBrush);
	//		if (lpTabItemData->iImage)
	//			TextOut(hdc,cx+3,3,lpTabItemData->lpText,strlen(lpTabItemData->lpText));
			if (lpTabItemData->lpText)
				TextOut(hdc,cx+3,3 + iTop,lpTabItemData->lpText,strlen(lpTabItemData->lpText)); // ������Ŀ�ı�
			if (bCurSel == FALSE)
			{ // ������ǵ�ǰѡ�����¶��к���
				MoveToEx(hdc,cx,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/,NULL);
				LineTo(hdc,cx + width,lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/);
			}
		}
}

// **************************************************
// ������static UINT GetItemWidth(HWND hWnd , LPTABITEMDATA lpTabItemData)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpTabItemData -- ��Ŀ���ݽṹ
// 
// ����ֵ��������Ŀ�Ŀ��
// �����������õ�ָ����Ŀ�Ŀ��
// ����: 
// **************************************************
static UINT GetItemWidth(HWND hWnd , LPTABITEMDATA lpTabItemData)
{
	UINT iWidth = 0;

		if (lpTabItemData == NULL)
			return 0; // ��Ŀ������Ч
		iWidth += 6;// the edge and bank

		if (lpTabItemData->iImage != -1)
		{
			iWidth += IMAGE_WIDTH; // image width
			iWidth += 1;  // bank the image and text
		}
		if (lpTabItemData->lpText != NULL)
		{
			iWidth += GetStringWidth(hWnd,lpTabItemData->lpText); // �ı����
		}

		return iWidth; // ������Ŀ���
}

// **************************************************
// ������static UINT GetStringWidth(HWND hWnd,LPTSTR lpString)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpString -- ָ���ִ�
// 
// ����ֵ�������ִ��Ŀ��
// �����������õ��ִ��Ŀ�ȡ�
// ����: 
// **************************************************
static UINT GetStringWidth(HWND hWnd,LPTSTR lpString)
{
	HDC hdc;
    int lpASCIICodeWidthBuffer[128];
    int ChineseCharWidth;
	UINT iWidth = 0;

		if (lpString == NULL)
			return 0;
		hdc = GetDC(hWnd); // �õ��豸���
		if (GetCharWidth(hdc,0,0x7f,lpASCIICodeWidthBuffer)==0) // �õ�ASCII�ַ��Ŀ��
			goto GETSTRINGWIDTH_END;
		// get chinese character width
		if (GetCharWidth(hdc,0xb0a1,0xb0a1,&ChineseCharWidth)==0) // �õ����ֵĿ��
			goto GETSTRINGWIDTH_END;

		while(*lpString)
		{ // ����ÿһ���ַ��Ŀ�Ȳ����
			if (*lpString<0 || *lpString > 0x7f)
			{  // current Character is a chinese
				iWidth += ChineseCharWidth;
				lpString+=2;
			}
			else
			{  // current character is english or sambol
				iWidth +=lpASCIICodeWidthBuffer[*lpString];;
				lpString ++;
			}
		}
GETSTRINGWIDTH_END:
		ReleaseDC(hWnd,hdc); // �ͷ�DC
		return iWidth; // ���ؿ��
}

// **************************************************
// ������static void ScrollArrowNeed(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������ȷ����ͷ�Ĺ�����ͷ�Ƿ���Ҫ��
// ����: 
// **************************************************
static void ScrollArrowNeed(HWND hWnd)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	DWORD dwStyle, dwExStyle;
	int width = 0, iBankWidth = 0;
	int iIndex;
	RECT rect;
	BOOL bRightArrow = FALSE;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);
	
		GetClientRect(hWnd,&rect);
		iBankWidth = rect.right - rect.left;
		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		if (dwStyle & TCS_MULTILINE)
		{// ������ʾ����,�����й�����ͷ����
			lpTabCtrlData->bScrollArrow = FALSE;
		}
		else
		{// ֻ����ʾһ��
			if (dwExStyle & TCS_EX_AUTOHIDETEXT)
			{  //�Զ������ı��Ŀؼ�����Ҫ��ͷ
				lpTabCtrlData->bScrollArrow = FALSE;
				return ;
			}

			iIndex = lpTabCtrlData->iDisplayIndex; 
			while(1)
			{
//				RETAILMSG(1,("PtrlistAt0 %d \r\n",iIndex));
				lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);
				if (lpTabItemData == NULL)
					break; //�Ѿ�û������
				
				width +=lpTabItemData->iWidth;
				iIndex ++ ;
				if (iIndex >=lpTabCtrlData->lpTabData->count)
					break;
			}
			if (iBankWidth >= width)
			{  // ����Ҫ��ʾ�Ҽ�ͷ
				lpTabCtrlData->bScrollArrow = FALSE;
			}
			else
			{  // ��Ҫ��ʾ�Ҽ�ͷ
				lpTabCtrlData->bScrollArrow = TRUE;
			}
		}
		return ;
}

// **************************************************
// ������static int GetCurrentItem(HWND hWnd,POINTS points)
// ������
// 	IN hWnd -- ���ھ��
// 	IN points -- ���λ��
// 
// ����ֵ��������Ŀ������
// �����������õ������ڵ���Ŀ��
// ����: 
// **************************************************
static int GetCurrentItem(HWND hWnd,POINTS points)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	DWORD dwStyle;
	DWORD dwExStyle;
	int width = 0, iBankWidth = 0,cx = 0;
	int iIndex;
	RECT rect;
	BOOL bRightArrow = FALSE;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);
	
		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		iBankWidth = rect.right - rect.left;
		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		dwExStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
		if (dwStyle & TCS_MULTILINE)
		{// ������ʾ����
		}
		else
		{// ֻ����ʾһ��
			if (dwExStyle & TCS_EX_AUTOHIDETEXT)
			{
				iIndex = 0;
				while(1)
				{
					lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex); // �õ���Ŀ���ݽṹ
					if (lpTabItemData == NULL)
						break; //�Ѿ�û������
					
					if (dwExStyle & TCS_EX_AUTOHIDETEXT)
					{ // �����Զ������ı�
						if (iIndex == lpTabCtrlData->iCurSel)
							width = iBankWidth - HIDEITEMWIDTH * (lpTabCtrlData->lpTabData->count - 1);
						else
							width = HIDEITEMWIDTH;
					}
					else
					{
						width = lpTabItemData->iWidth;
					}
					cx += width;
					if (cx > points.x)
						return iIndex; // �ڵ�ǰ����Ŀ�ϣ�������Ŀ������
					iIndex ++ ; // ��һ����Ŀ
					if (iIndex >=lpTabCtrlData->lpTabData->count)
						break; // �Ѿ�û����Ŀ��
				}
			}
			else
			{
				if (points.y > lpTabCtrlData->iTabItemHeight /*TABITEMHEIGHT*/)
					return NOTINITEM;
				if (lpTabCtrlData->bScrollArrow)
				{ // �����м�ͷ
					if (points.x < ARROWWIDTH)
						return LEFTARROWITEM; // ���ͷλ��
					if (points.x > iBankWidth - ARROWWIDTH)
						return RIGHTARROWITEM; // �Ҽ�ͷλ��
					cx = ARROWWIDTH;
				}
				iIndex = lpTabCtrlData->iDisplayIndex;  // �õ���ʼ��ʾ������
				while(1)
				{
	//				RETAILMSG(1,("PtrlistAt1 %d \r\n",iIndex));
					lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex); // �õ���Ŀ���ݽṹ
					if (lpTabItemData == NULL)
						break; //�Ѿ�û������
					
					cx +=lpTabItemData->iWidth;
					if (cx > points.x)
						return iIndex; // �ڵ�ǰ����Ŀ�ϣ�������Ŀ������
					iIndex ++ ; // ��һ����Ŀ
					if (iIndex >=lpTabCtrlData->lpTabData->count)
						break; // �Ѿ�û����Ŀ��
				}
			}
		}
		return NOTINITEM; // ����û����Ŀ
}
// **************************************************
// ������static void ScrollTab(HWND hWnd,int iNewItem)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iNewItem -- ��ͷ����
// 
// ����ֵ����
// ����������������Ŀ��
// ����: 
// **************************************************
static void ScrollTab(HWND hWnd,int iNewItem)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);
		switch(iNewItem)
		{
			case LEFTARROWITEM: // ���ͷ
				if (lpTabCtrlData->iDisplayIndex > 0)
					lpTabCtrlData->iDisplayIndex --; // ��ʾǰһ����Ŀ
				break;
			case RIGHTARROWITEM: // �Ҽ�ͷ
				if (CanRightScroll(hWnd) == TRUE)
					lpTabCtrlData->iDisplayIndex ++;
				break;
			default:
				return;
		}
		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
}
// **************************************************
// ������static void SetNewCurItem(HWND hWnd,int iNewItem)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iNewItem -- �µ���Ŀ
// 
// ����ֵ����
// ���������������µĵ�ǰ��Ŀ��
// ����: 
// **************************************************
static void SetNewCurItem(HWND hWnd,int iNewItem)
{
	LPTABCTRLDATA lpTabCtrlData;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);

		if (lpTabCtrlData->iCurSel == iNewItem)
			return ; // û�иı䵱ǰѡ��
		if (SendNormalNotify(hWnd,TCN_SELCHANGING) == TRUE)
		{
			//��ֹ�����µĵ�ǰ��Ŀ
			return ;
		}
		lpTabCtrlData->iCurSel = iNewItem; // �����µĵ�ǰѡ��
		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		SendNormalNotify(hWnd,TCN_SELCHANGE); // ֪ͨ�����ڵ�ǰѡ��ı�
}

// **************************************************
// ������static BOOL CanRightScroll(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���������ҹ�����TRUE�����򷵻�FALSE��
// �����������ж��Ƿ�������ҹ���
// ����: 
// **************************************************
static BOOL CanRightScroll(HWND hWnd)
{
	LPTABCTRLDATA lpTabCtrlData;
	LPTABITEMDATA lpTabItemData;
	DWORD dwStyle;
	int width = 0, iBankWidth = 0;
	int iIndex;
	RECT rect;
	BOOL bRightArrow = FALSE;

		lpTabCtrlData = (LPTABCTRLDATA)GetWindowLong(hWnd,0); // �õ�TAB�ṹ
		ASSERT(lpTabCtrlData);
	
		if (lpTabCtrlData->bScrollArrow == FALSE)
			return FALSE; // û�м�ͷ�����ܹ�
		GetClientRect(hWnd,&rect); // �õ����ڿͻ�����
		iBankWidth = rect.right - rect.left;
		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��

		iIndex = lpTabCtrlData->iDisplayIndex;  // �õ���ʼ��ʾ������
		while(1)
		{ // 
//			RETAILMSG(1,("PtrlistAt2 %d \r\n",iIndex));
			lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);// �õ���ǰ��Ŀ����
			if (lpTabItemData == NULL)
				break; //�Ѿ�û������
			
			width +=lpTabItemData->iWidth; // ������Ŀ���
			iIndex ++ ; // ��һ����Ŀ
			if (iIndex >=lpTabCtrlData->lpTabData->count)
				break; // �Ѿ�û����Ŀ
		}
		if (iBankWidth > width)
		{  // ����Ҫ�����ҹ���
			return FALSE;
		}
		else
		{  // ��Ҫ�����ҹ���
			return TRUE;
		}
}

// **************************************************
// ������static  LRESULT SendNormalNotify(HWND hWnd,UINT iCode)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iCode -- Ҫ���͵�֪ͨ��Ϣ
// 
// ����ֵ��������Ϣ���
// ��������������ָ����֪ͨ��Ϣ�������ڡ�
// ����: 
// **************************************************
static  LRESULT SendNormalNotify(HWND hWnd,UINT iCode)
{
	HWND hParent;
	NMHDR hdr;

		hdr.hwndFrom=hWnd;
		hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID); // �õ�����ID
		hdr.code=iCode;
		hParent=GetParent(hWnd); // �õ������ھ��
		return SendMessage(hParent,WM_NOTIFY,(WPARAM)hdr.idFrom,(LPARAM)&hdr); // ����֪ͨ��Ϣ
}


// !!! Add By Jami chen for adjust the item width
// **************************************************
// ������static void AdjustItemSize(HWND hWnd,LPTABCTRLDATA lpTabCtrlData)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpTabCtrlData -- TAB�ؼ��ṹָ��
// 
// ����ֵ����
// ��������������TAB��Ŀ�Ŀ�ȡ�
// ����: 
// **************************************************
				
static void AdjustItemSize(HWND hWnd,LPTABCTRLDATA lpTabCtrlData)
{
	int iControlWidth = 0;
	int iBankWidth = 0, iAddWidth;
	RECT rect;
	int iItemNum,iIndex;
	LPTABITEMDATA lpTabItemData;

		// ����������Ŀ�Ŀ��
		iIndex = 0;
		while(1)
		{ // ��ʾÿһ����Ŀ
			lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);
			if (lpTabItemData == NULL)
				break; //�Ѿ�û������
			lpTabItemData->iWidth = GetItemWidth(hWnd,lpTabItemData); // �õ���Ŀ���
			iControlWidth += lpTabItemData->iWidth;
			iIndex ++; //��һ����Ŀ
		}

		GetClientRect(hWnd,&rect); // �õ����ڿͻ�����
		iBankWidth = rect.right - rect.left; // �õ����ڵĿ��
		
		if (iBankWidth < iControlWidth)
		{
			// ��ǰ�ؼ�������Ŀ�Ŀ�ȴ��ڴ��ڿ�ȣ��޷��ڵ������
			return ;
		}
		iBankWidth -= iControlWidth; //�õ���ʣ��Ŀ��
		iItemNum = lpTabCtrlData->lpTabData->count;
		// ƽ����������е���Ŀ
		iAddWidth =  iBankWidth / iItemNum;
		iIndex = 0;
		while(1)
		{ // ��ʾÿһ����Ŀ
			lpTabItemData = PtrListAt(lpTabCtrlData->lpTabData,iIndex);
			if (lpTabItemData == NULL)
				break; //�Ѿ�û������
			if (iIndex == ( iItemNum -1 ))
			{  // ���һ����Ŀ�������ʣ�µĿ��ȫ������
				lpTabItemData->iWidth += iBankWidth ; // �õ���Ŀ���
			}
			else
			{
				// �������һ��������ƽ�����
				lpTabItemData->iWidth += iAddWidth ; // �õ���Ŀ���
			}

			iBankWidth -= iAddWidth;
			iIndex ++; //��һ����Ŀ
		}
}			
// !!! Add End


// **************************************************
// ������static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight,int xOffset,int yOffset)
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN x -- ��ʾX����
// 	IN y -- ��ʾY����
// 	IN nWidth -- λͼ���
// 	IN nHeight -- λͼ�߶�
// 
// ����ֵ����
// ��������������λͼ��
// ����: 
// **************************************************
static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight,int xOffset,int yOffset)
{
	HDC hCompDC=CreateCompatibleDC(hdc );  // �����ڴ�DC
	HBITMAP hOldMap;

	hOldMap=(HBITMAP)SelectObject(hCompDC,(HGDIOBJ)hBitmap);  // ѡ��ǰλͼ���ڴ�DC
	BitBlt(hdc, x, y, nWidth, nHeight, hCompDC,xOffset,yOffset, SRCCOPY); // ����λͼ
	SelectObject(hCompDC,hOldMap); // �ָ��ڴ�DC��λͼ
	DeleteDC(hCompDC); // ɾ���ڴ�DC
}
