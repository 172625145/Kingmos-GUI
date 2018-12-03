/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����Slider�࣬ϵͳ�ಿ��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-26
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <eWindows.h>
#include <SliderCtrl.h>

/***************  ȫ���� ���壬 ���� *****************/

static const char classSliderCtrl[] = "SliderControl";

#define SLIDERHEIGHT  16
#define TICKHEIGHT	  10

#define VERTBANK			3
#define HORZBANK			10
#define SLIDERSELHEIGHT		12
#define TICKLINELENGTH		6

#define THUMBWIDTH			8
#define THUMBHEIGHT			16


#define POS_THUMBBLOCK		0x0001
#define POS_RIGHTSLIDER		0x0002
#define POS_LEFTSLIDER		0x0003
#define POS_NONE			0x0000

typedef struct{
	int iCurPos;
	int iSliderPos;
	int iTickPos;
	LONG iRangeMin;
	LONG iRangeMax;
	WORD wTicFreq;
	int iSelStart;
	int iSelEnd;
	RECT rectThumb;
	int iAutoTickNum ; 
	int iTickInc ;
	int iSliderlength ;
	int iHorzBank ;
	SIZE sizeThumb;
	HICON hArrowIcon;

	int iPageSize;
	int iLineSize;
	COLORREF cl_Text;
	COLORREF cl_Bk;
}SLIDERCTRLDATA , *LPSLIDERCTRLDATA;


// ********************************************************************
// ��������
// ********************************************************************

ATOM RegisterSliderCtrlClass(HINSTANCE hInstance);
static LRESULT CALLBACK SliderCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);


static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetThumbIcon(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetRangeMin(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetRangeMax(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetTic(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTic(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetRangeMin(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetRangeMax(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoClearTics(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetSel(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetSelStart(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetSelEnd(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoGetTics(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetTicPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetNumTicks(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelStart(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelEnd(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoClearSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTicFreq(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetThumbRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetChannelRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTipSide(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam);
/*************************************************************************/
static void DrawHorzSilder(HWND hWnd, HDC hdc);
static void GetThumbRect(HWND hWnd);
static int GetCurrentPosition(HWND hWnd,POINTS points);
static BOOL SetNewCurPos(HWND hWnd,int xPos);
static void MovePageSize(HWND hWnd,int iPosition);
static void ResetSlider(HWND hWnd);

// ********************************************************************
// ������ATOM RegisterSliderCtrlClass(HINSTANCE hInstance)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
// ����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
// ����������ע�ᵱǰӦ�ó������
// ����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
ATOM RegisterSliderCtrlClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)SliderCtrlWndProc; // Slider����̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(LONG);
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classSliderCtrl; // Slider����

	return RegisterClass(&wc); // ע��Slider��
}


// ********************************************************************
// ������static LRESULT CALLBACK SliderCtrlWndProc(HWND , UINT , WPARAM , LPARAM )
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��ͬ����Ϣ�в�ͬ�ķ���ֵ�����忴��Ϣ����
// ����������Slider�ര�ڹ��̺���
//����: 
// ********************************************************************
static LRESULT CALLBACK SliderCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint(hWnd, &ps);
			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_ERASEBKGND: // ɾ��������Ϣ
			OnEraseBkgnd( hWnd,( HDC )wParam );
			return 0;
		case WM_COMMAND: // ������Ϣ
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
		case WM_MOUSEMOVE: // ����ƶ�
			return DoMouseMove(hWnd,wParam,lParam);
   	    case WM_LBUTTONUP: // ����������
			// the mouse left button be released
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_CREATE: // ������Ϣ
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // �ƻ���Ϣ
			DoDestroyWindow(hWnd);
			break;
		case WM_SETCTLCOLOR	: // ������ɫ
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // �õ���ɫ
			return DoGetColor(hWnd,wParam,lParam);


		case TBM_GETPOS: // �õ���ǰ�����λ��
			return DoGetPos(hWnd,wParam,lParam);
		case TBM_GETRANGEMIN: // �õ��������Сλ��
			return DoGetRangeMin(hWnd,wParam,lParam);
		case TBM_GETRANGEMAX: // �õ���������λ��
			return DoGetRangeMax(hWnd,wParam,lParam);
		case TBM_GETTIC: // �õ�ָ���Ǻ���ָ����λ��
			return DoGetTic(hWnd,wParam,lParam);
		case TBM_SETTIC: // ����һ���Ǻ�
			return DoSetTic(hWnd,wParam,lParam);
		case TBM_SETPOS: // ����һ���µĵ�ǰλ��
			return DoSetPos(hWnd,wParam,lParam);
		case TBM_SETRANGE: // ����һ���µĻ��鷶Χ
			return DoSetRange(hWnd,wParam,lParam);
		case TBM_SETRANGEMIN: // ����һ���µĻ�����С�ķ�Χ
			return DoSetRangeMin(hWnd, wParam, lParam);
		case TBM_SETRANGEMAX:// ����һ���µĻ������ķ�Χ
			return DoSetRangeMax(hWnd, wParam, lParam);
		case TBM_CLEARTICS:// ���������
			return DoClearTics(hWnd, wParam, lParam);
		case TBM_SETSEL: // ����ѡ������
			return DoSetSel(hWnd, wParam, lParam);
		case TBM_SETSELSTART: // ���ÿ�ʼѡ��������Ϣ
			return DoSetSelStart(hWnd, wParam, lParam);
		case TBM_SETSELEND:// ���ý���ѡ������
			return DoSetSelEnd(hWnd, wParam, lParam);
		case TBM_GETPTICS: // �õ�������ŵ�λ��
			return DoGetTics(hWnd,wParam,lParam);
		case TBM_GETTICPOS: // �õ�ָ����ŵ�����λ��
			return DoGetTicPos(hWnd,wParam,lParam);
		case TBM_GETNUMTICS: // �õ���ŵ�����Ŀ
			return DoGetNumTicks(hWnd,wParam,lParam);
		case TBM_GETSELSTART: // �õ�ѡ������Ŀ�ʼλ��
			return DoGetSelStart(hWnd,wParam,lParam);
		case TBM_GETSELEND: // �õ�ѡ������Ľ���λ��
			return DoGetSelEnd(hWnd,wParam,lParam);
		case TBM_CLEARSEL: // ���ѡ������
			return DoClearSel(hWnd,wParam,lParam);
		case TBM_SETTICFREQ: // �����Զ���ŵ�Ƶ��
			return DoSetTicFreq(hWnd,wParam,lParam);
		case TBM_SETPAGESIZE: // ����ҳ�ߴ�
			return DoSetPageSize(hWnd,wParam,lParam);
		case TBM_GETPAGESIZE: // �õ�ҳ�ߴ�
			return DoGetPageSize(hWnd,wParam,lParam);
		case TBM_SETLINESIZE: // �����гߴ�
			return DoSetLineSize(hWnd,wParam,lParam);
		case TBM_GETLINESIZE: // �õ��гߴ�
			return DoGetLineSize(hWnd,wParam,lParam);
		case TBM_GETTHUMBRECT: // �õ������λ��
			return DoGetThumbRect(hWnd,wParam,lParam);
		case TBM_GETCHANNELRECT: // �õ�������λ��
			return DoGetChannelRect(hWnd,wParam,lParam);
		case TBM_SETTHUMBLENGTH: // ���û���ĳ���
			return DoSetThumbLength(hWnd,wParam,lParam);
		case TBM_GETTHUMBLENGTH: // �õ�����ĳ���
			return DoGetThumbLength(hWnd,wParam,lParam);
		case TBM_SETTOOLTIPS: // ���ù�����ʾ���ھ��
			return DoSetToolTips(hWnd,wParam,lParam);
		case TBM_GETTOOLTIPS: // �õ�������ʾ���ھ��
			return DoGetToolTips(hWnd,wParam,lParam);
		case TBM_SETTIPSIDE: // ���ù�����ʾ���ڵ�λ��
			return DoSetTipSide(hWnd,wParam,lParam);

		case TBM_SETBUDDY: // ���ð󶨴���
			return DoSetBuddy(hWnd,wParam,lParam);
		case TBM_GETBUDDY: // �õ��󶨴���
			return DoGetBuddy(hWnd,wParam,lParam);
		
		case TBM_SETTHUMBICON: // ���û����λͼ
			return DoSetThumbIcon(hWnd,wParam,lParam);
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
	
	LPSLIDERCTRLDATA lpSliderCtrlData;
	RECT rect;
	DWORD dwStyle;

	lpSliderCtrlData = (LPSLIDERCTRLDATA)malloc(sizeof(SLIDERCTRLDATA)); // ����һ��Slider��ṹ
	if (lpSliderCtrlData == NULL)
		return -1; // ���ܴ����ô���
	
	GetClientRect(hWnd,&rect); // �õ��ͻ�����
	dwStyle = (DWORD)GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��

	// ��ʼ��Slider��ṹ
	lpSliderCtrlData->iCurPos = 0;
	lpSliderCtrlData->iSliderPos = 0;
	lpSliderCtrlData->iTickPos = 0;
	lpSliderCtrlData->iRangeMin = 0;
	lpSliderCtrlData->iRangeMax = 10;
	lpSliderCtrlData->wTicFreq = 1;
	lpSliderCtrlData->iSelStart = -1;
	lpSliderCtrlData->iSelEnd = -1;
	lpSliderCtrlData->iPageSize = lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin;
	lpSliderCtrlData->iLineSize = 1;

	lpSliderCtrlData->hArrowIcon = NULL;

	if (dwStyle & TBS_NOTICKS)
	{ // û�б��
		lpSliderCtrlData->iSliderPos = rect.top + VERTBANK;
	}
	else
	{ // �б��
		if ((dwStyle & TBS_TOP) == 0)
		{
			// ����ڻ���������
			lpSliderCtrlData->iSliderPos = rect.top + VERTBANK;
			lpSliderCtrlData->iTickPos = lpSliderCtrlData->iSliderPos + SLIDERHEIGHT; 
		}
		else
		{
			// ����ڻ���������
			lpSliderCtrlData->iTickPos = rect.top + VERTBANK;
			lpSliderCtrlData->iSliderPos = lpSliderCtrlData->iTickPos+ TICKHEIGHT; 
		}

	}

	lpSliderCtrlData->iAutoTickNum = ((lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin + (lpSliderCtrlData->wTicFreq -1) ) / lpSliderCtrlData->wTicFreq) -1; // -1 �� ����Ҫ�����һ����
	lpSliderCtrlData->iTickInc = (rect.right - rect.left - HORZBANK * 2) / (lpSliderCtrlData->iAutoTickNum + 1); // ����ǰ�ĳ��ȵ�������
	lpSliderCtrlData->iSliderlength = lpSliderCtrlData->iTickInc * (lpSliderCtrlData->iAutoTickNum + 1); // ÿһ�ݵĳ��� * ����
	lpSliderCtrlData->iHorzBank = (rect.right - rect.left - lpSliderCtrlData->iSliderlength) / 2; // �ܳ��� - Slider�ĳ��ȣ�Ȼ�����ߵȷ�

	lpSliderCtrlData->sizeThumb.cx = THUMBWIDTH;
	lpSliderCtrlData->sizeThumb.cy = THUMBHEIGHT;

	// ������ɫ
	lpSliderCtrlData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);;
	lpSliderCtrlData->cl_Bk  = GetSysColor(COLOR_STATIC);

	SetWindowLong(hWnd,0,(DWORD)lpSliderCtrlData); // ����Slider��ṹ������

	GetThumbRect(hWnd); // �õ�����ľ���
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
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0);  // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		if (lpSliderCtrlData->hArrowIcon != NULL)
			DestroyIcon(lpSliderCtrlData->hArrowIcon); // �ƻ���ͷICON

		return 0;
}

// ********************************************************************
// ������static LRESULT DoPaint(HWND hWnd,HDC hdc)
// ������
//	IN hWnd - ���ھ��
//  IN hdc - �豸���
// ����ֵ��
//	��
// ����������WM_PAINT �������
// ����: 
// ********************************************************************
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;
	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		SetTextColor(hdc,lpSliderCtrlData->cl_Text); // ����ǰ����ɫ
		SetBkColor(hdc,lpSliderCtrlData->cl_Bk); // ���ñ�����ɫ

		dwStyle = (DWORD)GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
/*
#define TBS_AUTOTICKS           0x0001
#define TBS_VERT                0x0002
#define TBS_HORZ                0x0000
#define TBS_TOP                 0x0004
#define TBS_BOTTOM              0x0000
#define TBS_LEFT                0x0004
#define TBS_RIGHT               0x0000
#define TBS_BOTH                0x0008
#define TBS_NOTICKS             0x0010
#define TBS_ENABLESELRANGE      0x0020
#define TBS_FIXEDLENGTH         0x0040
#define TBS_NOTHUMB             0x0080
#define TBS_TOOLTIPS            0x0100
*/
		if (dwStyle & TBS_VERT)
		{
			// �Ǵ�ֱ�Ļ���
			if (dwStyle & TBS_NOTICKS)
			{
			}
			else
			{
			}
		}
		else
		{ // ˮƽ����
			DrawHorzSilder(hWnd, hdc); // ����ˮƽ����
		}
		return 0;
}

// ********************************************************************
// ������static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc)
// ������
//  IN hWnd - ���ھ��
//  IN hdc - �豸���
// ����ֵ��
//	
// ����������WM_ERASEBKGND �������
// ����: 
// ********************************************************************
static LRESULT OnEraseBkgnd(HWND  hWnd,HDC hdc)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	HBRUSH hBrush;
	RECT rect;
		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		hBrush = CreateSolidBrush(lpSliderCtrlData->cl_Bk); // ����������ɫ��ˢ
		if (hBrush == NULL)
			return 0;
		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		FillRect(hdc,&rect,hBrush); // ���ͻ���
		DeleteObject(hBrush); // ɾ����ˢ
		return 0;
}

// **************************************************
// ������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(X,Y),��ǰ����λ�á�
// ����ֵ����
// ��������������WM_LBUTTONDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINTS points;
	int iPosition;

		points.x=LOWORD(lParam);
		points.y=HIWORD(lParam);

		iPosition = GetCurrentPosition(hWnd,points); // �õ���ǰ���λ��

		switch(iPosition)
		{
			case POS_THUMBBLOCK: // �ڻ�����
				SetCapture(hWnd); // ץס��꣬�����϶�����
				break;
			case POS_RIGHTSLIDER: // �����ұ߻���
			case POS_LEFTSLIDER: // ������߻���
				MovePageSize(hWnd,iPosition); // �ƶ�һҳ
				break;
			case POS_NONE: // �հ�λ��
				break;
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(X,Y),��ǰ����λ�á�
// ����ֵ����
// ��������������WM_MOUSEMOVE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	short xPos,yPos;
	DWORD dwStyle;
 

	// if the mouse be capture by this window
	if (GetCapture()==hWnd)
	{ // ���ڻ�����
			// Get LButton down Position
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);

			dwStyle = (DWORD)GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			if (dwStyle & TBS_VERT)
			{ // ��ֱ����
			}
			else
			{ // ˮƽ����
				if (SetNewCurPos(hWnd,xPos) == TRUE) // �����µĻ���λ��
					InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
			}
	}
	return 0;
}
// **************************************************
// ������static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(X,Y),��ǰ����λ�á�
// ����ֵ����
// ��������������WM_LBUTTONUP��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	// Release capture 
	ReleaseCapture(); // �ͷ����
	wParam++;
	lParam++;
	return 0;
}
// **************************************************
// ������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ָ��ṹCTLCOLORSTRUCT��ָ��
// ����ֵ����
// ��������������WM_SETCTLCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		 
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �����ı���ɫ
			 lpSliderCtrlData->cl_Text = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �����ı�������ɫ
			 lpSliderCtrlData->cl_Bk = lpCtlColor->cl_TextBk;
		 }
		return TRUE;
}
// **************************************************
// ������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN/OUT lParam -- ָ��ṹCTLCOLORSTRUCT��ָ��
// ����ֵ����
// ��������������WM_GETCTLCOLORR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �õ��ı���ɫ
			 lpCtlColor->cl_Text = lpSliderCtrlData->cl_Text ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �õ��ı�������ɫ
				lpCtlColor->cl_TextBk = lpSliderCtrlData->cl_Bk ;
		 }
		 return TRUE;
}


// ********************************************************************
// ������static void DrawHorzSilder(HWND hWnd, HDC hdc)
// ������
//    IN hWnd - ���ھ��
//    IN hdc - �豸���
// ����ֵ��
//       ��	
// ��������������ˮƽ����Ļ���
// ����: 
// ********************************************************************
static void DrawHorzSilder(HWND hWnd, HDC hdc)
{
		// ��ˮƽ�Ļ���
//	int iTickPos = -1,iSliderPos = -1;
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;
	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		dwStyle = (DWORD)GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��

		// �����
		if (lpSliderCtrlData->iTickPos != -1)
		{
//	lpSliderCtrlData->iAutoTickNum = ((lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin + (lpSliderCtrlData->wTicFreq -1) ) / lpSliderCtrlData->wTicFreq) -1; // -1 �� ����Ҫ�����һ����
//	lpSliderCtrlData->iTickInc = (rect.right - rect.left - HORZBANK * 2) / (lpSliderCtrlData->iAutoTickNum + 1);
//	lpSliderCtrlData->iSliderlength = lpSliderCtrlData->iTickInc * (lpSliderCtrlData->iAutoTickNum + 1);
//	lpSliderCtrlData->iHorzBank = (rect.right - rect.left - lpSliderCtrlData->iSliderlength) / 2;
			// ������ߵı��
			MoveToEx(hdc,rect.left + lpSliderCtrlData->iHorzBank,lpSliderCtrlData->iTickPos,NULL);
			LineTo(hdc,rect.left + lpSliderCtrlData->iHorzBank,lpSliderCtrlData->iTickPos + TICKLINELENGTH);
			// �����ұߵı��
			MoveToEx(hdc,rect.right - lpSliderCtrlData->iHorzBank ,lpSliderCtrlData->iTickPos,NULL);
			LineTo(hdc,rect.right - lpSliderCtrlData->iHorzBank,lpSliderCtrlData->iTickPos + TICKLINELENGTH);
			if (dwStyle & TBS_AUTOTICKS)
			{ // ���Զ����
				int iTickHorz = rect.left + lpSliderCtrlData->iHorzBank;
				int i;

					for (i = 1; i <= lpSliderCtrlData->iAutoTickNum ; i++)  //����Ҫ�����˵���
					{  // ��ÿһ�����
						MoveToEx(hdc,iTickHorz + lpSliderCtrlData->iTickInc * i ,lpSliderCtrlData->iTickPos,NULL);
						LineTo(hdc,iTickHorz + lpSliderCtrlData->iTickInc * i,lpSliderCtrlData->iTickPos + TICKLINELENGTH);
					}
			}
		}
		// ������������
		if (lpSliderCtrlData->iSliderPos != -1)
		{
			if ((dwStyle & TBS_NOTHUMB) == 0)
			{
				// ��Ҫ������
				if (dwStyle & TBS_ENABLESELRANGE)
				{ // ����ѡ��Χ�����п�
					HBRUSH hBrush;
					RECT rectRange;
						// ������߿�
						rectRange.left = rect.left + lpSliderCtrlData->iHorzBank;
						rectRange.top = lpSliderCtrlData->iSliderPos;
						rectRange.right = rectRange.left+ lpSliderCtrlData->iSliderlength;
						rectRange.bottom = lpSliderCtrlData->iSliderPos + SLIDERSELHEIGHT;
						DrawEdge(hdc,&rectRange,BDR_SUNKENINNER,BF_RECT);
						InflateRect(&rectRange,-2,-2);
						// �����ڿ�
						hBrush = GetStockObject(WHITE_BRUSH);
						FillRect(hdc,&rectRange,hBrush);
				}
				else
				{ // ������ѡ��Χ
					HPEN hPen;
					int iPos;
						
						hPen = CreatePen(PS_SOLID,1,RGB(150,150,150)); // �������ɫ��
						hPen = (HPEN)SelectObject(hdc,hPen); // ѡ���
						iPos = lpSliderCtrlData->iSliderPos + SLIDERHEIGHT /2 - 2; // �õ����ߵ�λ��
						MoveToEx(hdc,rect.left + lpSliderCtrlData->iHorzBank ,iPos,NULL);
						LineTo(hdc,rect.right -lpSliderCtrlData->iHorzBank,iPos); // ��һ����
						hPen = (HPEN)SelectObject(hdc,hPen); // �ָ���
						DeleteObject(hPen); // ɾ����

						hPen = CreatePen(PS_SOLID,1,RGB(220,220,220)); // ����ǳ��ɫ��
						hPen = (HPEN)SelectObject(hdc,hPen);
						iPos += 1; // ����һ�е�λ��
						MoveToEx(hdc,rect.left + lpSliderCtrlData->iHorzBank ,iPos,NULL);
						LineTo(hdc,rect.right - lpSliderCtrlData->iHorzBank,iPos); // ��һ����
						hPen = (HPEN)SelectObject(hdc,hPen);
						DeleteObject(hPen); // ɾ����

						hPen = CreatePen(PS_SOLID,1,RGB(255,255,255)); // ������ɫ�ı�
						hPen = (HPEN)SelectObject(hdc,hPen);
						iPos += 1; // ����һ�е�λ��
						MoveToEx(hdc,rect.left + lpSliderCtrlData->iHorzBank ,iPos,NULL);
						LineTo(hdc,rect.right - lpSliderCtrlData->iHorzBank,iPos); // ��һ����
						hPen = (HPEN)SelectObject(hdc,hPen);
						DeleteObject(hPen); // ɾ����
				}
			}
			// ������
			{
//				int iSliderBlockx = lpSliderCtrlData->iCurPos;
//				int iSliderBlocky = lpSliderCtrlData->iSliderPos;
				if (lpSliderCtrlData->hArrowIcon == NULL)
				{ // û��ָ�������ICON ��ʹ��ϵͳ����
					HICON hIcon;

					if (dwStyle & TBS_TOP)
					{ // ����������Ļ���
						hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SLIDERBLOCK_TOP ), IMAGE_ICON,THUMBWIDTH, THUMBHEIGHT, 0 ) ;
					}
					else if (dwStyle & TBS_BOTH)
					{ // �������߶��еĻ���
						hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SLIDERBLOCK_HORZBOTH ), IMAGE_ICON,THUMBWIDTH, THUMBHEIGHT, 0 ) ;
					}
					else 
					{ // ����������Ļ���
						hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SLIDERBLOCK_BOTTOM ), IMAGE_ICON,THUMBWIDTH, THUMBHEIGHT, 0 ) ;
					}
					DrawIcon(hdc,lpSliderCtrlData->rectThumb.left,lpSliderCtrlData->rectThumb.top,hIcon); // ���ƻ���
					DestroyIcon(hIcon); // �ƻ�����
				}
				else
				{
					DrawIcon(hdc,lpSliderCtrlData->rectThumb.left,lpSliderCtrlData->rectThumb.top,lpSliderCtrlData->hArrowIcon); // ���ƻ���
				}
			}
		}
}


// ********************************************************************
// ������static void GetThumbRect(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	��
// �����������õ������λ��
// ����: 
// ********************************************************************
static void GetThumbRect(HWND hWnd)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		GetClientRect(hWnd,&rect); // �õ��ͻ�����

		if (lpSliderCtrlData->iSliderPos != -1)
		{ // �õ�����ľ���
			lpSliderCtrlData->rectThumb.left = lpSliderCtrlData->iHorzBank + lpSliderCtrlData->iCurPos *  lpSliderCtrlData->iSliderlength / (lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin) - lpSliderCtrlData->sizeThumb.cx / 2;
			lpSliderCtrlData->rectThumb.right = lpSliderCtrlData->rectThumb.left + lpSliderCtrlData->sizeThumb.cx;
			lpSliderCtrlData->rectThumb.top = lpSliderCtrlData->iSliderPos + (SLIDERHEIGHT - lpSliderCtrlData->sizeThumb.cy) /2;
			lpSliderCtrlData->rectThumb.bottom = lpSliderCtrlData->rectThumb.top + lpSliderCtrlData->sizeThumb.cy;
		}
}

// ********************************************************************
// ������static int GetCurrentPosition(HWND hWnd,POINTS points);
// ������
//	IN hWnd - ���ھ��
//    IN points - ��ǰҪ�õ�λ�õĵ�����
// ����ֵ��
//	�õ���λ�á�
// �����������õ�ָ������ĵ����ڵ�λ��
// ����: 
// ********************************************************************
static int GetCurrentPosition(HWND hWnd,POINTS points)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	POINT point;
	RECT rectSlider;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		point.x = points.x;
		point.y = points.y;
		if( PtInRect( &lpSliderCtrlData->rectThumb, point ) )
		{ // ���ڻ�����
			return POS_THUMBBLOCK;
		}

		// �õ������ľ���		
		rectSlider.left = lpSliderCtrlData->iHorzBank;
		rectSlider.right = rectSlider.left + lpSliderCtrlData->iSliderlength;
		rectSlider.top = lpSliderCtrlData->iSliderPos;
		rectSlider.bottom = rectSlider.top + SLIDERHEIGHT;

		if( PtInRect( &rectSlider, point ) )
		{ // ���ڻ�����
			if (point.x > lpSliderCtrlData->rectThumb.left)
				return POS_RIGHTSLIDER; // �����ұ߻���
			return POS_LEFTSLIDER; // ������߻���
		}
		return POS_NONE; // ���ڿհ�λ��
}

// ********************************************************************
// ������static BOOL SetNewCurPos(HWND hWnd,int xPos)
// ������
//    IN hWnd - ���ھ��
//    IN xPox - ��ǰ���λ��
// ����ֵ��
//	�ɹ�����TRUE�����򷵻�FALSE��
// ���������������µ�λ�á�
// ����: 
// ********************************************************************
static BOOL SetNewCurPos(HWND hWnd,int xPos)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
//	POINT point;
//	RECT rectSlider;
	int iNewCurPos  = 0;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		if (xPos < lpSliderCtrlData->iHorzBank)
		{ // ������Сλ�ã����õ���Сλ��
			iNewCurPos = lpSliderCtrlData->iRangeMin;
		}
		else if (xPos > (lpSliderCtrlData->iHorzBank + lpSliderCtrlData->iSliderlength))
		{ // �������λ�ã����õ����λ��
			iNewCurPos = lpSliderCtrlData->iRangeMax;
		}
		else
		{ // �õ��µ�λ��
			iNewCurPos = (xPos - lpSliderCtrlData->iHorzBank) * (lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin) /  lpSliderCtrlData->iSliderlength;
		}
		if (iNewCurPos != lpSliderCtrlData->iCurPos)
		{ // λ�øı�
			lpSliderCtrlData->iCurPos = iNewCurPos; // �����µĻ���λ��
			GetThumbRect(hWnd); // �õ�����ľ���
			return TRUE;
		}
		return FALSE;
}
// **************************************************
// ������static void MovePageSize(HWND hWnd,int iPosition)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iPosition -- ��ǰ����ڻ����ϵ�λ��
// ����ֵ����
// �����������ƶ�һҳ��
// ����: 
// **************************************************
static void MovePageSize(HWND hWnd,int iPosition)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		switch(iPosition)
		{
			case POS_RIGHTSLIDER: // �����ƶ�һҳ
				lpSliderCtrlData->iCurPos +=lpSliderCtrlData->iPageSize;
				break;
			case POS_LEFTSLIDER: // �����ƶ�һҳ
				lpSliderCtrlData->iCurPos -=lpSliderCtrlData->iPageSize;
				break;
			default:
				return ;
		}
		if (lpSliderCtrlData->iCurPos > lpSliderCtrlData->iRangeMax)
		{ // �������λ�ã����õ����λ��
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMax;
		}

		if (lpSliderCtrlData->iCurPos < lpSliderCtrlData->iRangeMin)
		{ // ������Сλ�ã����õ���Сλ��
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMin;
		}
		GetThumbRect(hWnd); // �õ�����ľ���
		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
}

// ********************************************************************
// ********************************************************************
// ********************************************************************
// ������static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//  IN wParam = 0  
//  IN lParam = 0
// ����ֵ��
//	���ص�ǰ�����λ��
// �����������õ���ǰ�����λ�ã�����TBM_GETPOS��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		return lpSliderCtrlData->iCurPos; // ���ص�ǰ��λ��
}
// ********************************************************************
// ������static LRESULT DoSetThumbIcon(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//    IN hWnd - ���ھ��
//    IN wParam = MAKEWPARAM(cx,cy)  ָ��λͼ�Ĵ�С
//    IN lParam = (HICON)hIcon       Ҫ�趨��λͼ���
// ����ֵ��
//	�ɹ�����TRUE�����򷵻�FALSE
// �������������û����λͼ������TBM_SETTHUMBICON��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetThumbIcon(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	HICON hIcon;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		hIcon = (HICON)lParam; // �õ������ICON���
		if (hIcon)
		{
			if (lpSliderCtrlData->hArrowIcon)
				DestroyIcon(lpSliderCtrlData->hArrowIcon); // �ƻ�ԭ����ICON���
			lpSliderCtrlData->hArrowIcon = hIcon;
			lpSliderCtrlData->sizeThumb.cx = LOWORD(wParam); // �õ�ICON�Ĵ�С
			lpSliderCtrlData->sizeThumb.cy = LOWORD(wParam);
			GetThumbRect(hWnd); // �õ�����ľ���
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
			return TRUE;
		}
		return FALSE;
}

// ********************************************************************
// ������static LRESULT DoGetRangeMin(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0  
//    IN lParam = 0
// ����ֵ��
//	���ػ������Сλ��
// �����������õ��������Сλ�ã�����TBM_GETRANGEMIN��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetRangeMin(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		return lpSliderCtrlData->iRangeMin; // �õ���������Сֵ
}
// ********************************************************************
// ������static LRESULT DoGetRangeMax(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0  
//    IN lParam = 0
// ����ֵ��
//	���ػ�������λ��
// �����������õ���������λ�ã�����TBM_GETRANGEMAX��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetRangeMax(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		return lpSliderCtrlData->iRangeMax; // �õ����������ֵ
}

// ********************************************************************
// ������static LRESULT DoGetTic(HWND hWnd,WPARAM wParam,LPARAM lParam);
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (WORD) iTick; 
//    IN lParam = 0
// ����ֵ��
//	����ָ���Ǻ���ָ����λ��
// �����������õ�ָ���Ǻ���ָ����λ�ã�����TBM_GETTIC��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetTic(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iPos;
	short iTick;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		iTick = (short)wParam; // �õ���ǵ�������
		if (iTick < 0)
			return -1;
		if (iTick >= lpSliderCtrlData->iAutoTickNum)
			return -1;
		iPos = lpSliderCtrlData->iRangeMin + (iTick+1) * lpSliderCtrlData->iTickInc; // �õ�ָ����ŵ�λ��
		return iPos; // ����ָ����ŵ�λ��
}
// ********************************************************************
// ������static LRESULT DoSetTic(HWND hWnd,WPARAM wParam,LPARAM lParam);
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = (LONG)lPosition;
// ����ֵ��
//	�ɹ�����TRUE�����򷵻�FALSE��
// ��������������һ���Ǻţ�����TBM_SETTIC��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetTic(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}


// ********************************************************************
// ������static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)lPosition;
// ����ֵ��
//	û�з���ֵ��
// ��������������һ���µĵ�ǰλ�ã�����TBM_SETPOS��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iPos;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;
		iPos = (LONG)lParam;

		if (iPos < lpSliderCtrlData->iRangeMin)
			iPos = lpSliderCtrlData->iRangeMin; // ��ǰλ��С����Сֵ�����õ���Сֵ
		if (iPos > lpSliderCtrlData->iRangeMax)
			iPos = lpSliderCtrlData->iRangeMax; // ��ǰλ�ô������ֵ�����õ����ֵ

		lpSliderCtrlData->iCurPos = iPos; // ���õ�ǰλ��
		GetThumbRect(hWnd); // �õ��������
		if (bRedraw == TRUE)
		{ // Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)MAKELONG(lMin,lMax);
// ����ֵ��
//	û�з���ֵ��
// ��������������һ���µĻ��鷶Χ������TBM_SETRANGE��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMax,iMin;
	BOOL bRedraw;
 
		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;
		iMax = (LONG)HIWORD(lParam);
		iMin = (LONG)LOWORD(lParam);

		lpSliderCtrlData->iRangeMax = iMax; // �������ֵ
		lpSliderCtrlData->iRangeMin = iMin; // ������Сֵ

		if (lpSliderCtrlData->iCurPos > lpSliderCtrlData->iRangeMax)
		{ // ��ǰλ�ô������ֵ
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMax; // ���õ����ֵ
			GetThumbRect(hWnd); // �õ��������
		}

		if (lpSliderCtrlData->iCurPos < lpSliderCtrlData->iRangeMin)
		{ // ��ǰλ��С����Сֵ
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMin; // ���õ���Сֵ
			GetThumbRect(hWnd); // �õ��������
		}
		ResetSlider(hWnd); // ����Slider

		if (bRedraw == TRUE)
		{ // ��Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoSetRangeMin(HWND hWnd, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (bool)bRedraw; 
//    IN lParam = lMin;
// ����ֵ��
//	û�з���ֵ��
// ��������������һ���µĻ�����С�ķ�Χ������TBM_SETRANGEMIN��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetRangeMin(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMin;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;
		iMin = (LONG)lParam;

		lpSliderCtrlData->iRangeMin = iMin; // ������Сֵ

		if (lpSliderCtrlData->iCurPos < lpSliderCtrlData->iRangeMin)
		{ // ��ǰλ��С����Сֵ
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMin; // ����ǰλ�����õ���Сֵ
			GetThumbRect(hWnd); // �õ��������
		}
		ResetSlider(hWnd); // ����Slider

		if (bRedraw == TRUE)
		{ // ��Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoSetRangeMax(HWND hWnd, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (bool)bRedraw; 
//    IN lParam = lMax;
// ����ֵ��
//	û�з���ֵ��
// ��������������һ���µĻ������ķ�Χ������TBM_SETRANGEMAX��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetRangeMax(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMax;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;
		iMax = (LONG)lParam;

		lpSliderCtrlData->iRangeMax = iMax; // �������ֵ

		if (lpSliderCtrlData->iCurPos > lpSliderCtrlData->iRangeMax)
		{ // ��ǰλ�ô������ֵ
			lpSliderCtrlData->iCurPos = lpSliderCtrlData->iRangeMax; // ���õ�ǰλ�õ����ֵ
			GetThumbRect(hWnd); // �õ�����ľ���
		}
		ResetSlider(hWnd); // ����Slider

		if (bRedraw == TRUE)
		{ // ��Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoClearTics(HWND hWnd, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (bool)bRedraw; 
//    IN lParam = 0;
// ����ֵ��
//	û�з���ֵ��
// ������������������ǣ�����TBM_CLEARTICS��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoClearTics(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		bRedraw = (BOOL)wParam;

		if (bRedraw == TRUE)
		{ // ��Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoSetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)MAKELONG(lMin,lMax);
// ����ֵ��
//	û�з���ֵ��
// ��������������ѡ�����򣬴���TBM_SETSEL��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMax,iMin;
	BOOL bRedraw;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��

		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		bRedraw = (BOOL)wParam;
		iMax = (LONG)HIWORD(lParam);
		iMin = (LONG)LOWORD(lParam);

		lpSliderCtrlData->iSelStart = iMin; // ����ѡ����Сֵ
		lpSliderCtrlData->iSelEnd = iMax; // ����ѡ�����ֵ

		if (bRedraw == TRUE)
		{ // ��Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}

// ********************************************************************
// ������static LRESULT DoSetSelStart(HWND hWnd, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)lMin;
// ����ֵ��
//	û�з���ֵ��
// �������������ÿ�ʼѡ�����򣬴���TBM_SETSELSTART��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetSelStart(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMin;
	BOOL bRedraw;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		bRedraw = (BOOL)wParam;
		iMin = (LONG)lParam;

		lpSliderCtrlData->iSelStart = iMin; // ����ѡ����Сֵ

		if (bRedraw == TRUE)
		{ // ��Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoSetSelEnd(HWND hWnd, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (bool)bRedraw; 
//    IN lParam = (LONG)lMax;
// ����ֵ��
//	û�з���ֵ��
// �������������ý���ѡ�����򣬴���TBM_SETSELEND��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetSelEnd(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int iMax;
	BOOL bRedraw;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		bRedraw = (BOOL)wParam;
		iMax = (LONG)lParam;

		lpSliderCtrlData->iSelEnd = iMax; // ����ѡ������ֵ

		if (bRedraw == TRUE)
		{ // ��Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}

// ********************************************************************
// ������static LRESULT DoGetTics(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = 0;
// ����ֵ��
//	�ɹ�����һ����Ÿ�����ŵ�λ�õ����顣���򷵻�NULL.
// �����������õ�������ŵ�λ�ã�����TBM_GETPTICS��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetTics(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

// ********************************************************************
// ������static LRESULT DoGetTicPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (WORD)iTick; 
//    IN lParam = 0;
// ����ֵ��
//	�ɹ�����һ��ָ����ŵ�����λ��.���򷵻�-1��
// �����������õ�ָ����ŵ�����λ�ã�����TBM_GETTICPOS��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetTicPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	short iTick;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0);  // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		iTick = (short)wParam;
		if (iTick < 0)
			return -1;
		if (iTick >= lpSliderCtrlData->iAutoTickNum -1 )
			return -1;

		return -1;
}


// ********************************************************************
// ������static LRESULT DoGetNumTicks(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = 0;
// ����ֵ��
//	�ɹ����ر�ŵ���Ŀ.
// �����������õ���ŵ�����Ŀ������TBM_GETNUMTICS��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetNumTicks(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		return lpSliderCtrlData->iAutoTickNum + 2; // ���ر�ǵĸ���
}

// ********************************************************************
// ������static LRESULT DoGetSelStart(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = 0;
// ����ֵ��
//	�ɹ�����ѡ������Ŀ�ʼλ��.
// �����������õ�ѡ������Ŀ�ʼλ�ã�����TBM_GETSELSTART��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetSelStart(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		return lpSliderCtrlData->iSelStart; // ���ص�ǰѡ�����Сֵ
}

// ********************************************************************
// ������static LRESULT DoGetSelEnd(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = 0;
// ����ֵ��
//	�ɹ�����ѡ������Ľ���λ��.
// �����������õ�ѡ������Ľ���λ�ã�����TBM_GETSELEND��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetSelEnd(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		return lpSliderCtrlData->iSelEnd; // �õ�ѡ�����Сֵ
}

// ********************************************************************
// ������static LRESULT DoClearSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (BOOL) fRedraw; 
//    IN lParam = 0;
// ����ֵ��
//	��
// �������������ѡ�����򣬴���TBM_CLEARSEL��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoClearSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;
	BOOL bRedraw;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (!(dwStyle & TBS_ENABLESELRANGE))
			return 0;

		bRedraw = (BOOL)wParam;
		// ���ѡ��
		lpSliderCtrlData->iSelStart = -1;
		lpSliderCtrlData->iSelEnd = -1;

		if (bRedraw == TRUE)
		{ // ��Ҫ�ػ洰��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return 0;
}

// ********************************************************************
// ������static LRESULT DoSetTicFreq(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (BOOL) fRedraw; 
//    IN lParam = 0;
// ����ֵ��
//	��
// ���������������Զ���ŵ�Ƶ�ʣ�����TBM_SETTICFREQ��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetTicFreq(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	DWORD dwStyle;
	WORD wTicFreq;
	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (!(dwStyle & TBS_AUTOTICKS))
			return 0;

		GetClientRect(hWnd,&rect); //�õ��ͻ�����
		wTicFreq = (WORD)wParam;
		
		lpSliderCtrlData ->wTicFreq = wTicFreq; // �����Զ���ŵ�Ƶ��
		ResetSlider(hWnd); // ����Slider

		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����

		return 0;
}


// ********************************************************************
// ������static LRESULT DoSetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = (LONG)lPageSize;
// ����ֵ��
//	��
// ��������������ҳ�ߴ磬����TBM_SETPAGESIZE��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int lPageSize;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		
		lPageSize = (int)lParam;

		lpSliderCtrlData->iPageSize = lPageSize;  // ����ҳ�ߴ�

		return 0;
}


// ********************************************************************
// ������static LRESULT DoGetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = 0;
// ����ֵ��
//	����ҳ�ߴ�.
// �����������õ�ҳ�ߴ磬����TBM_GETPAGESIZE��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetPageSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		
		return lpSliderCtrlData->iPageSize ;  // �õ���ǰ��ҳ�ߴ�
}


// ********************************************************************
// ������static LRESULT DoSetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = (LONG)lLineSize;
// ����ֵ��
//	
// ���������������гߴ磬����TBM_SETLINESIZE��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	int lLineSize;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		
		lLineSize = (int)lParam;

		lpSliderCtrlData->iLineSize = lLineSize;  // �����гߴ�

		return 0;
}

// ********************************************************************
// ������static LRESULT DoGetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = 0;
// ����ֵ��
//	�����гߴ�
// �����������õ��гߴ磬����TBM_GETLINESIZE��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetLineSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		
		return lpSliderCtrlData->iLineSize ;  // �õ��гߴ�
}


// ********************************************************************
// ������static LRESULT DoGetThumbRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    OUT lParam = (LPRECT)lpRect; //��Ż���λ�õľ���
// ����ֵ��
//	��
// �����������õ������λ��
// ����: 
// ********************************************************************
static LRESULT DoGetThumbRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	LPRECT lpRect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		
		lpRect = (LPRECT)lParam;
		if (lpRect == NULL)
			return 0;

		*lpRect = lpSliderCtrlData->rectThumb; // �õ�����ľ���
		return 0; 

}

// ********************************************************************
// ������static LRESULT DoGetChannelRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = (LPRECT)lpRect; //��Ż���λ�õľ���
// ����ֵ��
//	��
// �����������õ�������λ�ã�����TBM_GETCHANNELRECT��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetChannelRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;
	LPRECT lpRect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);
		
		lpRect = (LPRECT)lParam;
		if (lpRect == NULL)
			return 0;
		
		//lpRect->left = lpSliderCtrlData->
		return 0; 
}


// ********************************************************************
// ������static LRESULT DoSetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (UINT)iLength; 
//    IN lParam = 0; 
// ����ֵ��
//	��
// �������������û���ĳ��ȣ�����TBM_SETTHUMBLENGTH��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

// ********************************************************************
// ������static LRESULT DoGetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (UINT)iLength; 
//    IN lParam = 0; 
// ����ֵ��
//	���ػ���ĳ���
// �����������õ�����ĳ��ȣ�����TBM_GETTHUMBLENGTH��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetThumbLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


// ********************************************************************
// ������static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (hWnd)hToolTips; 
//    IN lParam = 0; 
// ����ֵ��
//	��
// �������������ù�����ʾ���ھ��������TBM_SETTOOLTIPS��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


// ********************************************************************
// ������static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = 0; 
//    IN lParam = 0; 
// ����ֵ��
//	���ع�����ʾ���ھ��
// �����������õ�������ʾ���ھ��������TBM_GETTOOLTIPS��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetToolTips(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

// ********************************************************************
// ������static LRESULT DoSetTipSide(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (int)iLocation; 
//    IN lParam = 0; 
// ����ֵ��
//	������һ�ε�λ��
// �������������ù�����ʾ���ڵ�λ�ã�����TBM_SETTIPSIDE��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetTipSide(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}


// ********************************************************************
// ������static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (int)iLocation; 
//    IN lParam = (HWND)hBuddy; 
// ����ֵ��
// 	������һ�εİ󶨵Ĵ���
// �������������ð󶨴��ڣ�����TBM_SETBUDDY��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

// ********************************************************************
// ������static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN wParam = (int)iLocation; 
//    IN lParam = 0; 
// ����ֵ��
//	���ذ󶨵Ĵ���
// �����������õ��󶨴��ڣ�����TBM_GETBUDDY��Ϣ��
// ����: 
// ********************************************************************
static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}




// **************************************************
// ������static void ResetSlider(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �������������軬�顣
// ����: 
// **************************************************
static void ResetSlider(HWND hWnd)
{
	LPSLIDERCTRLDATA lpSliderCtrlData;

	RECT rect;

		lpSliderCtrlData = (LPSLIDERCTRLDATA)GetWindowLong(hWnd,0); // �õ�Slider��ṹ
		ASSERT(lpSliderCtrlData);

		GetClientRect(hWnd,&rect); // �õ��ͻ�����

		lpSliderCtrlData->iAutoTickNum = ((lpSliderCtrlData->iRangeMax - lpSliderCtrlData->iRangeMin + (lpSliderCtrlData->wTicFreq -1) ) / lpSliderCtrlData->wTicFreq) -1; // -1 �� ����Ҫ�����һ����
		lpSliderCtrlData->iTickInc = (rect.right - rect.left - HORZBANK * 2) / (lpSliderCtrlData->iAutoTickNum + 1); // ����ǰ�ĳ��ȵ�������
		lpSliderCtrlData->iSliderlength = lpSliderCtrlData->iTickInc * (lpSliderCtrlData->iAutoTickNum + 1); // ÿһ�ݵĳ��� * ����
		lpSliderCtrlData->iHorzBank = (rect.right - rect.left - lpSliderCtrlData->iSliderlength) / 2; // �ܳ��� - Slider�ĳ��ȣ�Ȼ�����ߵȷ�
}

