/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����Updown32������
�汾�ţ�1.0.0
����ʱ�ڣ�2003-04-21
���ߣ�JAMI CHEN �½���
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <eupdown.h>
#include <elstctrl.h>

typedef struct UpDown32Struct
{
   HWND hwndBuddy;  // �󶨵Ĵ��ھ��
   WORD iBase;  // ��ѧ���� ��ֻ��Ϊ10 ���� 16
   WORD cAccels; //
   LPUDACCEL paAccels;
   long iLower;  //��Сֵ
   long iUpper;  // ���ֵ
   long iPos;  // ��ǰֵ
   int iInc;  // ����
   BOOL bUnionCode;  // �Ƿ�ΪUNICODE

   UINT uPressButton;  // ��ť״̬
}UPDOWN32,* LPUPDOWN32;

static const char classUPDOWN32[]="UPDOWN32";

#define _MAPPOINTER

#define NOARROW   0  
#define UPARROW   1
#define DOWNARROW 2

#define DEPRESS	  1
#define NOPRESS	  2


#define INVALIDNUMBER -1
#define SEPARATOR     -2

LRESULT WINAPI UpDown32WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroy(HWND hWnd);
static VOID DoPaint(HWND hWnd,HDC hdc);
//static void DrawUpDown32Arrow(HWND hWnd,HDC hdc);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoGetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetBase(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetRange(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetBase(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKillFocus(HWND hWnd, WPARAM wParam,LPARAM lParam);

// private function
static int GetBitValue(CHAR chBit,WORD iBase,BOOL bSeparator);
static void LongToChar(long iValue, CHAR *lpNumber, WORD iLen,WORD iBase,BOOL bSeparator);
static long CharToLong(CHAR *lpNumber,WORD iBase,BOOL bSeparator);
static WORD GetPosition(HWND hWnd,POINT point);
static BOOL IsInRange(LPUPDOWN32 lpUpDown32,long iPos);
static BOOL ScrollArrow(HWND hWnd,WORD iDirection);
static void SendNotify(HWND hWnd,long iPos,long iDelta);
static void DrawArrow(HWND hWnd, HDC hdc, UINT uDirection, UINT uStatus);

// **************************************************
// ������ATOM RegisterUpDownClass(HINSTANCE hInst)
// ������
// 	IN hInst -- ʵ�������
// ����ֵ������ע������
// ����������ע�����¿����ࡣ
// ����: 
// *************************************************
ATOM RegisterUpDownClass(HINSTANCE hInst)
{
   WNDCLASS wc;
// register UpDown32 class
   wc.hInstance=hInst;
   wc.lpszClassName= classUPDOWN32;
// the proc is class function
   wc.lpfnWndProc=(WNDPROC)UpDown32WndProc;
   wc.style=0;
   wc.hIcon= 0;
// at pen window, no cursor
   wc.hCursor= LoadCursor(NULL,IDC_ARROW);
// to auto erase background, must set a valid brush
// if 0, you must erase background yourself
   wc.hbrBackground = GetSysColorBrush( COLOR_BTNFACE );//GetStockObject( LTGRAY_BRUSH );
//   wc.hbrBackground = GetStockObject( COLOR_STATIC );

   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
// !!! it's important to save state of button, align to long
   wc.cbWndExtra=sizeof( long );
   return RegisterClass(&wc);
}
// **************************************************
// ������LRESULT WINAPI UpDown32WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// ������
// 	IN  hWnd -- ���ھ��
// 	IN  message -- Ҫ�������Ϣ
// 	IN  wParam -- ��Ϣ����
// 	IN  lParam -- ��Ϣ����
// ����ֵ��������Ϣ�Ĵ�������
// �������������������Ϣ��
// ����: 
// *************************************************
LRESULT WINAPI UpDown32WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
  	case WM_CREATE: // ����UPDOWN32����
           return DoCreate(hWnd,wParam,lParam);
	  case WM_PAINT:
	     	// ���ƴ���
	       hdc = BeginPaint(hWnd, &ps);  // Get hdc
	       DoPaint(hWnd,hdc);
	       EndPaint(hWnd, &ps);  //release hdc
           break;
	  case WM_LBUTTONDOWN: // ��������������
           return DoLButtonDown(hWnd,wParam,lParam);
      case WM_MOUSEMOVE: // ��������ƶ�
           return DoMouseMove(hWnd,wParam,lParam);
	    case WM_LBUTTONUP: // ��������������
           return DoLButtonUp(hWnd,wParam,lParam);
      case WM_KEYDOWN: // ���������
           return DoKeyDown(hWnd,wParam,lParam);
	  case WM_KILLFOCUS:// ����ʧȥ����
		   return DoKillFocus(hWnd,wParam,lParam);

      case UDM_GETACCEL:
           return DoGetAccel(hWnd,wParam,lParam);
      case UDM_GETBASE: // �õ���ǰ��ʹ�õ���ѧ����
           return DoGetBase(hWnd,wParam,lParam);
      case UDM_GETBUDDY: // �����뵱ǰ�ؼ��󶨵Ĵ���
           return DoGetBuddy(hWnd,wParam,lParam);
      case UDM_GETPOS: // �õ���ǰ��ֵ
           return DoGetPos(hWnd,wParam,lParam);
      case UDM_GETRANGE: // �õ����Ƶķ�Χ
           return DoGetRange(hWnd,wParam,lParam);
      case UDM_GETRANGE32: // �õ����Ʒ�Χ
           return DoGetRange32(hWnd,wParam,lParam);
      case UDM_GETUNICODEFORMAT: // �õ�UNICODE��ʽ
           return DoGetUnionCodeFormat(hWnd,wParam,lParam);
      case UDM_SETACCEL:
           return DoSetAccel(hWnd,wParam,lParam);
      case UDM_SETBASE: // �趨�µ���ѧ����
           return DoSetBase(hWnd,wParam,lParam);
      case UDM_SETBUDDY: // �趨�µİ󶨴���
           return DoSetBuddy(hWnd,wParam,lParam);
      case UDM_SETPOS: // �����µĵ�ǰֵ
           return DoSetPos(hWnd,wParam,lParam);
      case UDM_SETRANGE: // �趨�µĿؼ���Χ
           return DoSetRange(hWnd,wParam,lParam);
      case UDM_SETRANGE32: // �趨�µĿؼ���Χ
           return DoSetRange32(hWnd,wParam,lParam);
      case UDM_SETUNICODEFORMAT: // �趨�µ�unicode��ʽ
           return DoSetUnionCodeFormat(hWnd,wParam,lParam);
	  case WM_DESTROY: // �ƻ��ؼ�
		   DoDestroy(hWnd);
		   break;
      default:
   		// ����Ĭ����Ϣ   		
	       return DefWindowProc(hWnd,message,wParam,lParam);
	}
  return 0;
}

// **************************************************
// ������static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ����� 0 �� ���򷵻� -1��
// ���������������ؼ�������WM_CREATE��Ϣ��
// ����: 
// *************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   DWORD dwStyle;
   RECT rect;
   short x,y,cx,cy;

      lpUpDown32=malloc(sizeof(UPDOWN32)); // ����һ��UPDOWN32�Ľṹ

      if (lpUpDown32==NULL)
        return -1;

      // ��ʼ���ṹ
      lpUpDown32->hwndBuddy=NULL;
      lpUpDown32->iBase=10;
      lpUpDown32->cAccels=0;
      lpUpDown32->paAccels=NULL;
      lpUpDown32->iLower=0;
      lpUpDown32->iUpper=0;
      lpUpDown32->iPos=0;
      lpUpDown32->iInc=1;
      lpUpDown32->bUnionCode=0;
	  lpUpDown32->uPressButton = NOARROW;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
      if (dwStyle&UDS_AUTOBUDDY)
      { // �Զ��󶨴���
         lpUpDown32->hwndBuddy=GetNextWindow(hWnd,GW_HWNDPREV); // �õ��󶨴���
      }

      if (dwStyle&UDS_ALIGNRIGHT)
      { // �Ҷ���
         if (lpUpDown32->hwndBuddy)
         { // �а󶨴���
            GetWindowRect(lpUpDown32->hwndBuddy,&rect); // �õ��󶨴��ڵľ��δ���
            x=(short)rect.right+1;
            y=(short)rect.top;
            cy=(short)(rect.bottom-rect.top);

            GetWindowRect(hWnd,&rect); // �õ�UPDOWN32�Ĵ��ھ���
            cx=(short)(rect.right-rect.left);

            SetWindowPos(hWnd,NULL,x,y,cx,cy,SWP_NOZORDER|SWP_NOREDRAW); // ���ô��ھ����λ��
         }
      }
      else if (dwStyle&UDS_ALIGNLEFT)
      { // �����
         if (lpUpDown32->hwndBuddy)
         { // �а󶨴���
            GetWindowRect(hWnd,&rect);
            cx=(short)(rect.right-rect.left);

            GetWindowRect(lpUpDown32->hwndBuddy,&rect);
            x=rect.left-cx-1;
            y=(short)rect.top;
            cy=(short)(rect.bottom-rect.top);

            SetWindowPos(hWnd,NULL,x,y,cx,cy,SWP_NOZORDER|SWP_NOREDRAW); // �趨�󶨴��ڵ�λ��
         }
      }

      SetWindowLong(hWnd,0,(long)lpUpDown32); // �趨���ڵĽṹ
      return 0;
}
// **************************************************
// ������static LRESULT DoDestroy(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �����������ƻ��ؼ�������WM_DESTROY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDestroy(HWND hWnd)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;
   free(lpUpDown32); // �ͷŽṹ
   return 0;
}
// **************************************************
// ������static VOID DoPaint(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc --  �豸���
// ����ֵ����
// �������������ƴ���,����WM_PAINT��Ϣ��
// ����: 
// **************************************************
static VOID DoPaint(HWND hWnd,HDC hdc)
{
//   DrawUpDown32Arrow(hWnd,hdc);
	 DrawArrow(hWnd, hdc, UPARROW, NOPRESS);  // ����/���ͷ
	 DrawArrow(hWnd, hdc, DOWNARROW, NOPRESS); // ����/�Ҽ�ͷ��
}

// **************************************************
// ������static void DrawUpDown32Arrow(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// ����ֵ����
// �������������ƿؼ���ͷ��
// ����: 
// **************************************************
/*
static void DrawUpDown32Arrow(HWND hWnd,HDC hdc)
{
	RECT rect,rectArrow;
  DWORD dwStyle;
  HFONT hFont;
  int iOldMode;
//  COLOR bkColor;
//  extern HFONT hEngFont8x8;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE);
	if (dwStyle & WS_DISABLED)
	{
		SetTextColor(hdc,RGB(0x80,0x80,0x80));
	}
    hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_ENGLISH8X8) );//hEngFont8x8 );
	  if( dwStyle&UDS_HORZ )
	  {
          GetClientRect(hWnd,&rect);
          rectArrow.top=rect.top;
          rectArrow.bottom=rect.bottom;

          iOldMode = SetBkMode( hdc, TRANSPARENT );

          rectArrow.left=rect.left;
          rectArrow.right=rect.left+(rect.right-rect.left)/2;
          DrawText( hdc, "", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

          MoveTo(hdc,rectArrow.right,rectArrow.top);
          LineTo(hdc,rectArrow.right,rectArrow.bottom);

          rectArrow.left=rectArrow.right+1;
          rectArrow.right=rect.right;
          DrawText( hdc, "", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

          SetBkMode( hdc, iOldMode );

	  }
	  else
	  {
          GetClientRect(hWnd,&rect);
          rectArrow.left=rect.left;
          rectArrow.right=rect.right;

          iOldMode = SetBkMode( hdc, TRANSPARENT );

          rectArrow.top=rect.top;
          rectArrow.bottom=rect.top+(rect.bottom-rect.top)/2;
          DrawText( hdc, "", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

          // Sparate line
          MoveTo(hdc,rectArrow.left,rectArrow.bottom);
          LineTo(hdc,rectArrow.right,rectArrow.bottom);

          rectArrow.top=rectArrow.bottom+1;
          rectArrow.bottom=rect.bottom;
          DrawText( hdc, "", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

          SetBkMode( hdc, iOldMode );
	  }
    SelectObject( hdc, hFont );
}
*/
// **************************************************
// ������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LOWORD ��X����
// 				 HIWORD ��Y����
// ����ֵ�� ��
// ��������������WM_LBUTTONDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   POINT point;
   WORD position;
   LPUPDOWN32 lpUpDown32;
   HDC hdc;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;

    // �õ�����λ��
    point.x=(short)LOWORD(lParam);
    point.y=(short)HIWORD(lParam);

    // ���ý��㵽��ǰ�ؼ�
    if (GetFocus()!=hWnd)
		SetFocus(hWnd);
    
	// �õ����µ�λ��
    position=GetPosition(hWnd,point);
    if (position==NOARROW)
       return 0;
	lpUpDown32->uPressButton = position;

	// �ػ��ͷ
	hdc = GetDC(hWnd);
	DrawArrow(hWnd,hdc,position,DEPRESS);
	ReleaseDC(hWnd,hdc);
	// �����µ�ֵ
    ScrollArrow(hWnd,position);
	SetCapture(hWnd);
	return 0;
}
// **************************************************
// ������static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LOWORD ��X����
// 				 HIWORD ��Y����
// ����ֵ����
// ��������������WM_MOUSEMOVE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   POINT point;
   WORD position;
   LPUPDOWN32 lpUpDown32;
   HDC hdc;

    if (hWnd != GetCapture())  // �Ƿ���ץס���
		return 0;
    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;

    // �õ����λ��
    point.x=(short)LOWORD(lParam);
    point.y=(short)HIWORD(lParam);
    position=GetPosition(hWnd,point);
	if (lpUpDown32->uPressButton !=NOARROW && lpUpDown32->uPressButton != position)
	{ // λ�÷����ı䣬�ָ���ť״̬
		hdc = GetDC(hWnd);
		DrawArrow(hWnd,hdc,lpUpDown32->uPressButton,NOPRESS); // ���Ƽ�ͷ
		ReleaseDC(hWnd,hdc);
		lpUpDown32->uPressButton = NOARROW;
	}
	return 0;
}
// **************************************************
// ������static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LOWORD ��X����
// 				 HIWORD ��Y����
// ����ֵ����
// ��������������WM_LBUTTONUP��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   HDC hdc;

	ReleaseCapture(); // �ͷ����
    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;

    // �ָ���ť״̬
	hdc = GetDC(hWnd);
	DrawArrow(hWnd,hdc,lpUpDown32->uPressButton,NOPRESS); // ���ƴ���
	ReleaseDC(hWnd,hdc);
	lpUpDown32->uPressButton = NOARROW;
	return 0;
}

// **************************************************
// ������
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT nVirtKey ,���¼��������ֵ
// 	IN lParam -- ����
// ����ֵ����
// ����������������̰�����ϢWM_KEYDOWN��
// ����: 
// **************************************************
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
  DWORD dwStyle;

  	  // �ж��Ƿ���Ҫ��������Ϣ
	    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
 	   if (!(dwStyle&UDS_ARROWKEYS))
  	    return 0;
		nVirtKey = (int) wParam;    // �����ֵ

		switch(nVirtKey)
		{
			case VK_UP:// �ϼ�ͷ
		        ScrollArrow(hWnd,UPARROW);  
				break;
			case VK_DOWN:// �¼�ͷ
  		      ScrollArrow(hWnd,DOWNARROW);
				break;
			default :
				return TRUE;
		};
		return TRUE;
}


// **************************************************
// ������static LRESULT DoGetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ����������UDM_GETACCEL ����Ϣ������
// ����: 
// **************************************************
static LRESULT DoGetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   return 0;
}
// **************************************************
// ������static LRESULT DoGetBase(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ��ʹ�õ���ѧ����
// �����������õ���ǰ��ʹ�õ���ѧ���ƣ�����UDM_GETBASE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetBase(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;
    return lpUpDown32->iBase; // ���ص�ǰ����ѧ����
}
// **************************************************
// ������static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�������뵱ǰ�ؼ��󶨵Ĵ��ھ��
// ���������������뵱ǰ�ؼ��󶨵Ĵ��ڣ�����UDM_GETBUDDY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return NULL;
    return (LRESULT)lpUpDown32->hwndBuddy; // ���ذ󶨴���
}
// **************************************************
// ������static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ����
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ��ֵ��
// �����������õ���ǰ��ֵ������UDM_GETPOS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   CHAR lpNumber[28];
   BOOL bSeparator=TRUE;
   DWORD dwStyle;
   long iPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return -1;
    if (lpUpDown32->hwndBuddy==NULL)
       return -1;

    // �õ��Ƿ��м����
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;
    // �õ���ǰ������
    SendMessage(lpUpDown32->hwndBuddy,WM_GETTEXT,(WPARAM)28,(LPARAM)lpNumber); // �����ı�
    iPos=CharToLong(lpNumber,lpUpDown32->iBase,bSeparator);
    if (IsInRange(lpUpDown32,iPos)==FALSE)
      return -1;
    return iPos;
}
// **************************************************
// ������static LRESULT DoGetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ؿؼ��ķ�Χ��LOWORD -- ���ֵ��HIWORD -- ��Сֵ��
// �����������õ����Ƶķ�Χ������UDM_GETRANGE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0);  // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;
    return MAKELONG((WORD)lpUpDown32->iUpper,(WORD)lpUpDown32->iLower); // ���ط�Χ
}
// **************************************************
// ������static LRESULT DoGetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	OUT wParam -- LPINT lpiHight �õ����ֵ��
// 	OUT lParam -- LPINT lpiLow �õ���Сֵ��
// ����ֵ����
// �����������õ����Ʒ�Χ������UDM_GETRANGE32��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   LPINT lpiHigh,lpiLow;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // ӳ��ָ��
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

    lpiLow =(LPINT)wParam;
    lpiHigh=(LPINT)lParam;

    *lpiHigh=lpUpDown32->iUpper; // �������ֵ
    *lpiLow=lpUpDown32->iLower; // ������Сֵ

    return 0;
}
// **************************************************
// ������static LRESULT DoGetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ��UNICODE��ʽ��
// �����������õ�UNICODE��ʽ������UDM_GETUNICODEFORMAT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;
    return lpUpDown32->bUnionCode; // ����UNICODE
}
// **************************************************
// ������static LRESULT DoSetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam --  ����
// 	IN lParam --  ����
// ����ֵ��
// ����������UDM_SETACCEL��Ϣ����
// ����: 
// **************************************************
static LRESULT DoSetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   return 0;
}
// **************************************************
// ������static LRESULT DoSetBase(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- WORD nBase , Ҫ�趨����ѧ����
// 	IN lParam -- ����
// ����ֵ���ɹ�����ԭ���Ľ��ƣ����򷵻�0��
// �����������趨�µ���ѧ���ƣ�����UDM_SETBASE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetBase(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   WORD nBase,nOldBase;
   CHAR lpNumber[28];
   DWORD dwStyle;
   BOOL bSeparator=TRUE;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;
    nBase=(WORD)wParam; // �õ�����
    if (nBase!=10&&nBase!=16)
      return 0;

    nOldBase=lpUpDown32->iBase;
    if (lpUpDown32->iBase!=nBase)
    {
      // Ҫ�����µ���ѧ����
      if (dwStyle&UDS_SETBUDDYINT)
      {
         if (lpUpDown32->hwndBuddy)
         {
           SendMessage(lpUpDown32->hwndBuddy,WM_GETTEXT,(WPARAM)28,(LPARAM)lpNumber); // �õ��󶨴��ڵ��ı�
           lpUpDown32->iPos=CharToLong(lpNumber,lpUpDown32->iBase,bSeparator);
         }

         lpUpDown32->iBase=nBase; // ���ý���

         if (lpUpDown32->hwndBuddy)
         {
           LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator);
           SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // ���ð󶨴��ڵ��ı�
           InvalidateRect(lpUpDown32->hwndBuddy,NULL,TRUE); // ��Ч����
         }
      }
    }
    return nOldBase;
}
// **************************************************
// ������static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- HWND hWnd Ҫ�󶨵Ĵ��ھ����
// 	IN lParam -- ����
// ����ֵ������ԭ���İ󶨴��ھ����
// �����������趨�µİ󶨴��ڣ�����UDM_SETBUDDY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   HWND hprevWnd;
   CHAR lpNumber[28];
   BOOL bSeparator=TRUE;
   DWORD dwStyle;
   long iOldPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;
    hprevWnd=lpUpDown32->hwndBuddy; // �õ�ǰһ���󶨴���
    lpUpDown32->hwndBuddy=(HWND)wParam; // �����µİ󶨴���

    if (lpUpDown32->hwndBuddy)
    {
      SendMessage(lpUpDown32->hwndBuddy,WM_GETTEXT,(WPARAM)28,(LPARAM)lpNumber); // �õ��󶨴��ڵ��ı�
      iOldPos=lpUpDown32->iPos; 
      lpUpDown32->iPos=CharToLong(lpNumber,lpUpDown32->iBase,bSeparator); // ת��Ϊ����
      SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // ����֪ͨ��Ϣ

    }

    return (LRESULT)hprevWnd;
}
// **************************************************
// ������static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD iPos ,Ҫ���õĵ�ǰֵ��
// 	IN lParam -- ����
// ����ֵ������ԭ���ĵ�ǰֵ��
// ���������������µĵ�ǰֵ������UDM_SETPOS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   CHAR lpNumber[28];
   short iPos,iOldPos;
   DWORD dwStyle;
   BOOL bSeparator=TRUE;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    if (dwStyle&UDS_NOTHOUSANDS)
       bSeparator=FALSE;

//    iPos=LOWORD(wParam);
    iPos=LOWORD(lParam);
    if (IsInRange(lpUpDown32,iPos)==FALSE) // ��ǰ��λ���Ƿ��ڿ��Ʒ�Χ��
       return 0;

    iOldPos=(short)lpUpDown32->iPos;
    lpUpDown32->iPos=iPos;

    if (lpUpDown32->hwndBuddy)
    {
      if (dwStyle&UDS_SETBUDDYINT)
      {
         LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator); // ����ת��Ϊ�ִ�
         SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // ���ð󶨴��ڵ��ı�

         InvalidateRect(lpUpDown32->hwndBuddy,NULL,TRUE); // ��Ч����
      }

    }
    SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // ����֪ͨ��Ϣ
    return iOldPos;
}
// **************************************************
// ������static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam --  LOWORD iIpper, ���ֵ
// 				  HIWORD iLower, ��Сֵ
// ����ֵ����
// ���������趨�µĿؼ���Χ������UDM_SETRANGE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   CHAR lpNumber[28];
   short iUpper,iLower;
   DWORD dwStyle;
   BOOL bSeparator=TRUE;
   long iOldPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;

    iUpper=LOWORD(lParam); // �õ����ֵ
    iLower=HIWORD(lParam); // �õ���Сֵ

    lpUpDown32->iUpper=iUpper; // ���÷�Χ
    lpUpDown32->iLower=iLower;

    if (IsInRange(lpUpDown32,lpUpDown32->iPos)==FALSE) // ��ǰλ���Ƿ��ڷ�Χ��
    {
      iOldPos=lpUpDown32->iPos;
      lpUpDown32->iPos=lpUpDown32->iLower;
      if (dwStyle&UDS_SETBUDDYINT)
      { // ���ð󶨴��ڵ�����
        if (lpUpDown32->hwndBuddy)
        {
          LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator); // ������ת��Ϊ�ִ�
          SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // ���ð󶨴��ڵ��ı�
        }
      }
      SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // ����֪ͨ��Ϣ
    }
    if (lpUpDown32->iUpper>lpUpDown32->iLower)
      lpUpDown32->iInc=1;
    else
      lpUpDown32->iInc=-1;

    return 0;
}
// **************************************************
// ������static LRESULT DoSetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- iLow, ��Сֵ
// 	IN lParam -- iHigh ,���ֵ
// ����ֵ����
// �����������趨�µĿؼ���Χ������UDM_SETRANGE32��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   CHAR lpNumber[28];
   long iHigh,iLow;
   DWORD dwStyle;
   BOOL bSeparator=TRUE;
   long iOldPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;

    iHigh=(long)lParam; // �õ����ֵ
    iLow =(long)wParam; // �õ���Сֵ

    lpUpDown32->iUpper=iHigh;
    lpUpDown32->iLower=iLow;

    if (IsInRange(lpUpDown32,lpUpDown32->iPos)==FALSE) // �жϵ�ǰλ���Ƿ��ڷ�Χ��
    { // ���ڷ�Χ��
      iOldPos=lpUpDown32->iPos;
      lpUpDown32->iPos=lpUpDown32->iLower; // ���õ�ǰλ��Ϊ��Сֵ
      if (dwStyle&UDS_SETBUDDYINT)
      {
        if (lpUpDown32->hwndBuddy)
        {
          LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator); // ����ת��Ϊ�ִ�
          SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // ���ð󶨴����ı�
        }
      }
      SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // ����֪ͨ��Ϣ
    }
    if (lpUpDown32->iUpper>lpUpDown32->iLower)
      lpUpDown32->iInc=1;
    else
      lpUpDown32->iInc=-1;
    return 0;
}
// **************************************************
// ������static LRESULT DoSetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- BOOL bUnionCode,Ҫ�趨��unicode��ʽ
// 	IN lParam -- ����
// ����ֵ������ԭ����unicode��ʽ
// �����������趨�µ�unicode��ʽ������UDM_SETUNICODEFORMAT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   BOOL bUnionCode,bPrev;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return 0;

    bUnionCode=(BOOL)wParam; // �õ�UNICODE
    bPrev=lpUpDown32->bUnionCode; // �õ�ǰһ��UNICODE
    lpUpDown32->bUnionCode=bUnionCode; // ����UNICODE

    return bPrev;
}

// **************************************************
// ������static long CharToLong(CHAR *lpNumber,WORD iBase,BOOL bSeparator)
// ������
// 	IN lpNumber -- Ҫת�����ַ���
// 	IN iBase -- Ҫת�����ַ�������ѧ����
// 	IN bSeparator  -- �ַ�������û�м����
// ����ֵ������ת�����������֡�
// �������������ַ���ת��Ϊ���֡�
// ����: 
// **************************************************
static long CharToLong(CHAR *lpNumber,WORD iBase,BOOL bSeparator)
{
  long iResult,iBitValue;
  WORD i;
  BOOL bNegative=FALSE;

     iResult=0;
     i=0;

     if (lpNumber[i]=='-')
     { // �Ǹ���
       bNegative=TRUE;
       i++;
     }
     else if (lpNumber[i]=='+')
     { // ������
       i++;
     }
     for (;lpNumber[i]!=0;i++)
     { // �õ���ǰ���ַ�
        iBitValue=GetBitValue(lpNumber[i],iBase,bSeparator);
        if (iBitValue==SEPARATOR)
          continue; // �Ƿ���
        if (iBitValue==INVALIDNUMBER)
          break; // �Ƿ�����
        iResult=iResult*iBase+iBitValue; // �õ���ǰ��ֵ
     }
     if (bNegative)
       iResult*=-1; //�õ�����
     return iResult;
}
// **************************************************
// ������static void LongToChar(long iValue, CHAR *lpNumber, WORD iLen,WORD iBase,BOOL bSeparator)
// ������
// 	IN iValue -- Ҫת��������
// 	OUT lpNumber -- �����洢ת������ַ����Ļ���
// 	IN iLen -- ����Ĵ�С
// 	IN iBase -- Ҫת�����ַ�������ѧ����
// 	IN bSeparator  -- �ַ�������û�м����
// ����ֵ����
// ����������������ת��Ϊ�ַ���
// ����: 
// **************************************************
static void LongToChar(long iValue, CHAR *lpNumber, WORD iLen,WORD iBase,BOOL bSeparator)
{
  CHAR chTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  CHAR chTemp[32];
  BOOL bNegative=FALSE;
  WORD i=0,j=0;
  long iBitValue;

     if (iValue<0)
     { // �Ǹ���
        bNegative=TRUE;
        iValue*=-1;
     }
     while(iValue)
     { // ��������
       if (!(i%3) && i != 0)
       { // ��ӷָ���
          if (bSeparator)
            chTemp[i++]=',';
       }
       iBitValue=iValue%iBase; // �õ���ǰ������
       chTemp[i++]=chTable[iBitValue]; // �õ��ַ�
       iValue/=iBase;
     }
     if (i)
     {
       if (bNegative)
         lpNumber[j++]='-'; // �Ǹ���
       while(i)
       {
         lpNumber[j++]=chTemp[--i]; // ���Ƶ�ǰ���ִ�
       }
     }
     else
     {
         lpNumber[j++]='0';
     }
     lpNumber[j]=0;
     return;
}
// **************************************************
// ������static int GetBitValue(CHAR chBit,WORD iBase,BOOL bSeparator)
// ������
// 	IN chBit -- Ҫת�����ַ�
// 	IN iBase -- ���ֽ���
// 	IN bSeparator -- �Ƿ��м����
// ����ֵ���ɹ�����ת�������֣����򷵻�INVALIDNUMBER��
// ������������ָ���ַ�ת��Ϊ����
// ����: 
// **************************************************
static int GetBitValue(CHAR chBit,WORD iBase,BOOL bSeparator)
{
       switch(chBit)
       {
          case '0':  // ����
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
                    return chBit-'0';
          case 'A': // 16��������
          case 'B':
          case 'C':
          case 'D':
          case 'E':
          case 'F':
                    if (iBase==16)
                    {
                       return 10+chBit-'A';
                    }
                    break;
          case 'a': // 16��������
          case 'b':
          case 'c':
          case 'd':
          case 'e':
          case 'f':
                    if (iBase==16)
                    {
                       return 10+chBit-'a';
                    }
                    break;
          case ',': // �ָ���
                   if (bSeparator)
                      return SEPARATOR;
       }
       return INVALIDNUMBER; // �����ַ��ǷǷ��ַ�
}
// **************************************************
// ������static WORD GetPosition(HWND hWnd,POINT point)
// ������
// 	IN hWnd -- ���ھ��
// 	IN point -- �������
// ����ֵ�����ص��λ�á�
// �����������õ�ָ���ĵ��ڿؼ���λ�á�
// ����: 
// **************************************************
static WORD GetPosition(HWND hWnd,POINT point)
{
  RECT rect,rectUpArrow,rectDownArrow;
  DWORD dwStyle;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    GetClientRect(hWnd,&rect);
	  if( dwStyle&UDS_HORZ )
	  { // ˮƽ����
// !!! Add By  Jami chen in 2004.07.15
		  if (dwStyle&UDS_VERTSHOW)
		  {
		  	  // �õ����ͷ��λ��
			  rectUpArrow.left=rect.left;
			  rectUpArrow.right=rect.right;
			  rectUpArrow.top=rect.top;
			  rectUpArrow.bottom=rect.top+(rect.bottom-rect.top)/2;
			  // �õ��Ҽ�ͷ��λ��
			  rectDownArrow.left=rect.left;
			  rectDownArrow.right=rect.right;
			  rectDownArrow.top=rectUpArrow.bottom+1;
			  rectDownArrow.bottom=rect.bottom;
		  }
		  else
// !!! Add End By  Jami chen in 2004.07.15
		  {
	  		  // �õ����ͷ��λ��
			  rectUpArrow.top=rect.top;
			  rectUpArrow.bottom=rect.bottom;
			  rectUpArrow.left=rect.left;
			  rectUpArrow.right=rect.left+(rect.right-rect.left)/2;
			  // �õ��Ҽ�ͷ��λ��
			  rectDownArrow.top=rect.top;
			  rectDownArrow.bottom=rect.bottom;
			  rectDownArrow.left=rectUpArrow.right+1;
			  rectDownArrow.right=rect.right;
		  }
	  }
	  else
	  { // ��ֱ����
// !!! Add By  Jami chen in 2004.07.15
		  if (dwStyle&UDS_HORZSHOW)
		  {
		  	  // �õ��ϼ�ͷ��λ��
			  rectUpArrow.top=rect.top;
			  rectUpArrow.bottom=rect.bottom;
			  rectUpArrow.left=rect.left;
			  rectUpArrow.right=rect.left+(rect.right-rect.left)/2;
			  // �õ��¼�ͷ��λ��
			  rectDownArrow.top=rect.top;
			  rectDownArrow.bottom=rect.bottom;
			  rectDownArrow.left=rectUpArrow.right+1;
			  rectDownArrow.right=rect.right;
		  }
		  else
// !!! Add End By  Jami chen in 2004.07.15
		  {
		  	  // �õ��ϼ�ͷ��λ��
			  rectUpArrow.left=rect.left;
			  rectUpArrow.right=rect.right;
			  rectUpArrow.top=rect.top;
			  rectUpArrow.bottom=rect.top+(rect.bottom-rect.top)/2;
			  // �õ��¼�ͷ��λ��
			  rectDownArrow.left=rect.left;
			  rectDownArrow.right=rect.right;
			  rectDownArrow.top=rectUpArrow.bottom+1;
			  rectDownArrow.bottom=rect.bottom;
		  }
	  }
    if( PtInRect( &rectUpArrow, point ) )
       return UPARROW; // ��ǰ�����ϼ�ͷ
    if( PtInRect( &rectDownArrow, point ) )
       return DOWNARROW; // ��ǰ�����¼�ͷ
    return NOARROW;
}

// **************************************************
// ������static BOOL IsInRange(LPUPDOWN32 lpUpDown32,long iPos)
// ������
// 	IN LPUPDOWN32 lpUpDown32 -- �ؼ��ṹ
// 	IN iPos -- Ҫ���Ե�ֵ
// ����ֵ���ڿؼ���Χ�ڣ�����TRUE�����򷵻�FALSE��
// �����������ж�ָ����ֵ�ǲ����ڿؼ���Χ�ڡ�
// ����: 
// **************************************************
static BOOL IsInRange(LPUPDOWN32 lpUpDown32,long iPos)
{
    if (lpUpDown32->iUpper>lpUpDown32->iLower)
    {  
       if ((iPos>lpUpDown32->iUpper)||(iPos<lpUpDown32->iLower))
         return FALSE; // ��ǰ�㲻�ڷ�Χ��
    }
    else
    {
       if ((iPos<lpUpDown32->iUpper)||(iPos>lpUpDown32->iLower))
         return FALSE; // ��ǰ�㲻�ڷ�Χ��
    }
    return TRUE;
}

// **************************************************
// ������static BOOL ScrollArrow(HWND hWnd,WORD iDirection)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iDirection  -- ��ͷ����
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ����������������ͷ
// ����: 
// **************************************************
static BOOL ScrollArrow(HWND hWnd,WORD iDirection)
{
  DWORD dwStyle;
  LPUPDOWN32 lpUpDown32;
  CHAR lpNumber[28];
  BOOL bSeparator=TRUE;
  long iOldPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // �õ�UPDOWN32�Ľṹ
    if (lpUpDown32==NULL)
       return FALSE;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    if (dwStyle&UDS_NOTHOUSANDS)
       bSeparator=FALSE;

    iOldPos=lpUpDown32->iPos;
    if (lpUpDown32->hwndBuddy!=NULL)
    { // �а󶨴���
      SendMessage(lpUpDown32->hwndBuddy,WM_GETTEXT,(WPARAM)27,(LPARAM)lpNumber); // �õ��󶨴��ڵ��ı�
      lpUpDown32->iPos=CharToLong(lpNumber,lpUpDown32->iBase,bSeparator); // ���ִ�ת��Ϊ����
    }
    if (IsInRange(lpUpDown32,lpUpDown32->iPos)==FALSE) // ��ǰλ���Ƿ��ڷ�Χ��
       lpUpDown32->iPos=lpUpDown32->iLower;
    if (iDirection==UPARROW)
    { // ������
      if (lpUpDown32->iPos==lpUpDown32->iUpper)
      { // �Ѿ������ֵ
         if (dwStyle&UDS_WRAP)
           lpUpDown32->iPos=lpUpDown32->iLower;
         else
           return FALSE;
      }
      else
        lpUpDown32->iPos+=lpUpDown32->iInc; // ����һ������
    }
    else
    { // ������
      if (lpUpDown32->iPos==lpUpDown32->iLower)
      { // �Ѿ�����Сֵ
         if (dwStyle&UDS_WRAP)
           lpUpDown32->iPos=lpUpDown32->iUpper;
         else
           return FALSE;
      }
      else
        lpUpDown32->iPos-=lpUpDown32->iInc; // ��ȥһ������
    }

    if (dwStyle&UDS_SETBUDDYINT)
    { // ��Ҫ���ð󶨴���
       if (lpUpDown32->hwndBuddy!=NULL)
       {
         LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator); // ����ת��Ϊ�ִ�
         SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // ���ð󶨴��ڵ��ı�
         InvalidateRect(lpUpDown32->hwndBuddy,NULL,TRUE); // ��Ч�󶨴���
       }
    }
    SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // ����֪ͨ��Ϣ
    return TRUE;
}

// **************************************************
// ������static void SendNotify(HWND hWnd,long iPos,long iDelta)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iPos -- ��ǰλ��
// 	IN iDelta -- �ı䲽��
// ����ֵ����
// ��������������֪ͨ��Ϣ��
// ����: 
// **************************************************
static void SendNotify(HWND hWnd,long iPos,long iDelta)
{
	NMUPDOWN nmUpDown;
	HWND hParent;
	// ����֪ͨ��Ϣ�ṹ
	nmUpDown.hdr.hwndFrom=hWnd;
	nmUpDown.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
	nmUpDown.hdr.code=UDN_DELTAPOS;
	nmUpDown.iPos=iPos;
	nmUpDown.iDelta=iDelta;
	hParent=GetParent(hWnd); // �õ������ھ��
	SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmUpDown); // ����֪ͨ��Ϣ
}


// **************************************************
// ������static void DrawArrow(HWND hWnd, HDC hdc, UINT uDirection, UINT uStatus)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN uDirection -- ��ͷ����
// 	IN uStatus -- ��ͷ״̬
// ����ֵ����
// �������������Ƽ�ͷ��
// ����: 
// **************************************************
static void DrawArrow(HWND hWnd, HDC hdc, UINT uDirection, UINT uStatus)
{
   RECT rect,rectArrow;
   DWORD dwStyle;
   HFONT hFont;
   int iOldMode;

	  dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
	  if (dwStyle & WS_DISABLED)
	  { // DISABLE���
//		SetTextColor(hdc,RGB(0x80,0x80,0x80));
		SetTextColor(hdc,GetSysColor(COLOR_GRAYTEXT));
	  }
	  hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_ENGLISH8X8) );//hEngFont8x8 ); // �õ�����
      GetClientRect(hWnd,&rect); // �õ��ͻ�����
      iOldMode = SetBkMode( hdc, TRANSPARENT );
	  if( dwStyle&UDS_HORZ )
	  {  // ˮƽ��ͷ
		  if (uDirection == UPARROW)
		  { // ���ͷ
// !!! Add By  Jami chen in 2004.07.15
			if (dwStyle&UDS_VERTSHOW)
			{
			  // �õ����ͷ��λ��
				rectArrow.left=rect.left;
				rectArrow.right=rect.right;
				rectArrow.top=rect.top;
				rectArrow.bottom=rect.top+(rect.bottom-rect.top)/2;
			}
			else
// !!! Add End By  Jami chen in 2004.07.15
			{
				rectArrow.top=rect.top;
				rectArrow.bottom=rect.bottom;
				rectArrow.left=rect.left;
				rectArrow.right=rect.left+(rect.right-rect.left)/2;
			}
			
			if( uStatus == DEPRESS  )  // highlight state( sunken or checked )
			{  // ��ͷ����
				DrawEdge( hdc, &rectArrow, BDR_SUNKENOUTER, BF_RECT );
			}
			else  
			{  // ��ͷû�а���
				DrawEdge( hdc, &rectArrow, BDR_RAISEDOUTER, BF_RECT );
			}
			InflateRect( &rectArrow, -1, -1 );

			DrawText( hdc, "\x11", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

		  }
		  else
		  { // �Ҽ�ͷ
// !!! Add By  Jami chen in 2004.07.15
			if (dwStyle&UDS_VERTSHOW)
			{
			  // �õ��Ҽ�ͷ��λ��
				rectArrow.left=rect.left;
				rectArrow.right=rect.right;
				rectArrow.top=rect.top+(rect.bottom-rect.top)/2;
				rectArrow.bottom=rect.bottom;
			}
			else
// !!! Add End By  Jami chen in 2004.07.15
			{
				rectArrow.top=rect.top;
				rectArrow.bottom=rect.bottom;
				rectArrow.left=rect.left+(rect.right-rect.left)/2;
				rectArrow.right=rect.right;
			}

			if( uStatus == DEPRESS  )  // highlight state( sunken or checked )
			{ // ��ͷ����
				DrawEdge( hdc, &rectArrow, BDR_SUNKENOUTER, BF_RECT );
			}
			else  
			{ // ��ͷû�а���
				DrawEdge( hdc, &rectArrow, BDR_RAISEDOUTER, BF_RECT );
			}
	        InflateRect( &rectArrow, -1, -1 );
          
			DrawText( hdc, "\x10", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

		  }
	  }
	  else
	  {  // ��ֱ��ͷ
          GetClientRect(hWnd,&rect);
		  if (uDirection == UPARROW)
		  {   // �ϼ�ͷ
// !!! Add By  Jami chen in 2004.07.15
			  if (dwStyle&UDS_HORZSHOW)
			  {
		  		  // �õ��ϼ�ͷ��λ��
				  rectArrow.top=rect.top;
				  rectArrow.bottom=rect.bottom;
				  rectArrow.left=rect.left;
				  rectArrow.right=rect.left+(rect.right-rect.left)/2;
			  }
			  else
// !!! Add End By  Jami chen in 2004.07.15
			  {
				  rectArrow.left=rect.left;
				  rectArrow.right=rect.right;
				  rectArrow.top=rect.top;
				  rectArrow.bottom=rect.top+(rect.bottom-rect.top)/2;
			  }

			  if( uStatus == DEPRESS  )  // highlight state( sunken or checked )
			  { //��ͷ����
				DrawEdge( hdc, &rectArrow, BDR_SUNKENOUTER, BF_RECT );
			  }
			  else  
			  { // ��ͷû�а���
			 	DrawEdge( hdc, &rectArrow, BDR_RAISEDOUTER, BF_RECT );
			  }
			  InflateRect( &rectArrow, -1, -1 );

			  DrawText( hdc, "\x1e", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
		  }
		  else
		  {  
              // �¼�ͷ
// !!! Add By  Jami chen in 2004.07.15
			  if (dwStyle&UDS_HORZSHOW)
			  {
				  // �õ��¼�ͷ��λ��
				  rectArrow.top=rect.top;
				  rectArrow.bottom=rect.bottom;
				  rectArrow.left=rect.left+(rect.right-rect.left)/2;;
				  rectArrow.right=rect.right;
			  }
			  else
// !!! Add End By  Jami chen in 2004.07.15
			  {
				  rectArrow.left=rect.left;
				  rectArrow.right=rect.right;
				  rectArrow.top=rect.top+(rect.bottom-rect.top)/2;
				  rectArrow.bottom=rect.bottom;
			  }

			  if( uStatus == DEPRESS  )  // highlight state( sunken or checked )
			  {  // ��ͷ����
				DrawEdge( hdc, &rectArrow, BDR_SUNKENOUTER, BF_RECT );
			  }
			  else  
			  { // ��ͷû�а���
			 	DrawEdge( hdc, &rectArrow, BDR_RAISEDOUTER, BF_RECT );
			  }
			  InflateRect( &rectArrow, -1, -1 );

			  DrawText( hdc, "\x1f", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
		  }
	  }
      SetBkMode( hdc, iOldMode );
      SelectObject( hdc, hFont );
}

// **************************************************
// ������static LRESULT DoKillFocus(HWND hWnd, WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// �����������ؼ�ʧȥ���㡣
// ����: 
// **************************************************
static LRESULT DoKillFocus(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
	NMHDR nmHdr;

		nmHdr.code = NM_KILLFOCUS;
		nmHdr.hwndFrom = hWnd;
		nmHdr.idFrom = (UINT)GetWindowLong(hWnd,GWL_ID); // �õ����ڴ���
		SendMessage(GetParent(hWnd),WM_NOTIFY,(WPARAM)nmHdr.idFrom,(LPARAM)&nmHdr); // ����֪ͨ��Ϣ
		return 0;
}
