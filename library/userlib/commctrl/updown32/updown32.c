/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：Updown32窗口类
版本号：1.0.0
开发时期：2003-04-21
作者：JAMI CHEN 陈建明
修改记录：
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <eupdown.h>
#include <elstctrl.h>

typedef struct UpDown32Struct
{
   HWND hwndBuddy;  // 绑定的窗口句柄
   WORD iBase;  // 数学进制 ，只能为10 或者 16
   WORD cAccels; //
   LPUDACCEL paAccels;
   long iLower;  //最小值
   long iUpper;  // 最大值
   long iPos;  // 当前值
   int iInc;  // 步长
   BOOL bUnionCode;  // 是否为UNICODE

   UINT uPressButton;  // 按钮状态
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
// 声明：ATOM RegisterUpDownClass(HINSTANCE hInst)
// 参数：
// 	IN hInst -- 实例句柄。
// 返回值：返回注册结果。
// 功能描述：注册上下控制类。
// 引用: 
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
// 声明：LRESULT WINAPI UpDown32WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN  hWnd -- 窗口句柄
// 	IN  message -- 要处理的消息
// 	IN  wParam -- 消息参数
// 	IN  lParam -- 消息参数
// 返回值：返回消息的处理结果。
// 功能描述：处理控制消息。
// 引用: 
// *************************************************
LRESULT WINAPI UpDown32WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
  	case WM_CREATE: // 创建UPDOWN32窗口
           return DoCreate(hWnd,wParam,lParam);
	  case WM_PAINT:
	     	// 绘制窗口
	       hdc = BeginPaint(hWnd, &ps);  // Get hdc
	       DoPaint(hWnd,hdc);
	       EndPaint(hWnd, &ps);  //release hdc
           break;
	  case WM_LBUTTONDOWN: // 处理鼠标左键按下
           return DoLButtonDown(hWnd,wParam,lParam);
      case WM_MOUSEMOVE: // 处理鼠标移动
           return DoMouseMove(hWnd,wParam,lParam);
	    case WM_LBUTTONUP: // 处理鼠标左键弹起
           return DoLButtonUp(hWnd,wParam,lParam);
      case WM_KEYDOWN: // 处理键按下
           return DoKeyDown(hWnd,wParam,lParam);
	  case WM_KILLFOCUS:// 处理失去焦点
		   return DoKillFocus(hWnd,wParam,lParam);

      case UDM_GETACCEL:
           return DoGetAccel(hWnd,wParam,lParam);
      case UDM_GETBASE: // 得到当前所使用的数学进制
           return DoGetBase(hWnd,wParam,lParam);
      case UDM_GETBUDDY: // 返回与当前控件绑定的窗口
           return DoGetBuddy(hWnd,wParam,lParam);
      case UDM_GETPOS: // 得到当前的值
           return DoGetPos(hWnd,wParam,lParam);
      case UDM_GETRANGE: // 得到控制的范围
           return DoGetRange(hWnd,wParam,lParam);
      case UDM_GETRANGE32: // 得到控制范围
           return DoGetRange32(hWnd,wParam,lParam);
      case UDM_GETUNICODEFORMAT: // 得到UNICODE格式
           return DoGetUnionCodeFormat(hWnd,wParam,lParam);
      case UDM_SETACCEL:
           return DoSetAccel(hWnd,wParam,lParam);
      case UDM_SETBASE: // 设定新的数学进制
           return DoSetBase(hWnd,wParam,lParam);
      case UDM_SETBUDDY: // 设定新的绑定窗口
           return DoSetBuddy(hWnd,wParam,lParam);
      case UDM_SETPOS: // 设置新的当前值
           return DoSetPos(hWnd,wParam,lParam);
      case UDM_SETRANGE: // 设定新的控件范围
           return DoSetRange(hWnd,wParam,lParam);
      case UDM_SETRANGE32: // 设定新的控件范围
           return DoSetRange32(hWnd,wParam,lParam);
      case UDM_SETUNICODEFORMAT: // 设定新的unicode格式
           return DoSetUnionCodeFormat(hWnd,wParam,lParam);
	  case WM_DESTROY: // 破坏控件
		   DoDestroy(hWnd);
		   break;
      default:
   		// 处理默认消息   		
	       return DefWindowProc(hWnd,message,wParam,lParam);
	}
  return 0;
}

// **************************************************
// 声明：static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回 0 ， 否则返回 -1。
// 功能描述：创建控件，处理WM_CREATE消息。
// 引用: 
// *************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   DWORD dwStyle;
   RECT rect;
   short x,y,cx,cy;

      lpUpDown32=malloc(sizeof(UPDOWN32)); // 分配一个UPDOWN32的结构

      if (lpUpDown32==NULL)
        return -1;

      // 初始化结构
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

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
      if (dwStyle&UDS_AUTOBUDDY)
      { // 自动绑定窗口
         lpUpDown32->hwndBuddy=GetNextWindow(hWnd,GW_HWNDPREV); // 得到绑定窗口
      }

      if (dwStyle&UDS_ALIGNRIGHT)
      { // 右对齐
         if (lpUpDown32->hwndBuddy)
         { // 有绑定窗口
            GetWindowRect(lpUpDown32->hwndBuddy,&rect); // 得到绑定窗口的矩形窗口
            x=(short)rect.right+1;
            y=(short)rect.top;
            cy=(short)(rect.bottom-rect.top);

            GetWindowRect(hWnd,&rect); // 得到UPDOWN32的窗口矩形
            cx=(short)(rect.right-rect.left);

            SetWindowPos(hWnd,NULL,x,y,cx,cy,SWP_NOZORDER|SWP_NOREDRAW); // 设置窗口句柄的位置
         }
      }
      else if (dwStyle&UDS_ALIGNLEFT)
      { // 左对齐
         if (lpUpDown32->hwndBuddy)
         { // 有绑定窗口
            GetWindowRect(hWnd,&rect);
            cx=(short)(rect.right-rect.left);

            GetWindowRect(lpUpDown32->hwndBuddy,&rect);
            x=rect.left-cx-1;
            y=(short)rect.top;
            cy=(short)(rect.bottom-rect.top);

            SetWindowPos(hWnd,NULL,x,y,cx,cy,SWP_NOZORDER|SWP_NOREDRAW); // 设定绑定窗口的位置
         }
      }

      SetWindowLong(hWnd,0,(long)lpUpDown32); // 设定窗口的结构
      return 0;
}
// **************************************************
// 声明：static LRESULT DoDestroy(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：破坏控件，处理WM_DESTROY消息。
// 引用: 
// **************************************************
static LRESULT DoDestroy(HWND hWnd)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;
   free(lpUpDown32); // 释放结构
   return 0;
}
// **************************************************
// 声明：static VOID DoPaint(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc --  设备句柄
// 返回值：无
// 功能描述：绘制窗口,处理WM_PAINT消息。
// 引用: 
// **************************************************
static VOID DoPaint(HWND hWnd,HDC hdc)
{
//   DrawUpDown32Arrow(hWnd,hdc);
	 DrawArrow(hWnd, hdc, UPARROW, NOPRESS);  // 画上/左箭头
	 DrawArrow(hWnd, hdc, DOWNARROW, NOPRESS); // 画下/右箭头。
}

// **************************************************
// 声明：static void DrawUpDown32Arrow(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 返回值：无
// 功能描述：绘制控件箭头。
// 引用: 
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
// 声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LOWORD ，X坐标
// 				 HIWORD ，Y坐标
// 返回值： 无
// 功能描述：处理WM_LBUTTONDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   POINT point;
   WORD position;
   LPUPDOWN32 lpUpDown32;
   HDC hdc;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;

    // 得到坐标位置
    point.x=(short)LOWORD(lParam);
    point.y=(short)HIWORD(lParam);

    // 设置焦点到当前控件
    if (GetFocus()!=hWnd)
		SetFocus(hWnd);
    
	// 得到按下的位置
    position=GetPosition(hWnd,point);
    if (position==NOARROW)
       return 0;
	lpUpDown32->uPressButton = position;

	// 重绘箭头
	hdc = GetDC(hWnd);
	DrawArrow(hWnd,hdc,position,DEPRESS);
	ReleaseDC(hWnd,hdc);
	// 设置新的值
    ScrollArrow(hWnd,position);
	SetCapture(hWnd);
	return 0;
}
// **************************************************
// 声明：static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LOWORD ，X坐标
// 				 HIWORD ，Y坐标
// 返回值：无
// 功能描述：处理WM_MOUSEMOVE消息。
// 引用: 
// **************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   POINT point;
   WORD position;
   LPUPDOWN32 lpUpDown32;
   HDC hdc;

    if (hWnd != GetCapture())  // 是否有抓住鼠标
		return 0;
    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;

    // 得到鼠标位置
    point.x=(short)LOWORD(lParam);
    point.y=(short)HIWORD(lParam);
    position=GetPosition(hWnd,point);
	if (lpUpDown32->uPressButton !=NOARROW && lpUpDown32->uPressButton != position)
	{ // 位置发生改变，恢复按钮状态
		hdc = GetDC(hWnd);
		DrawArrow(hWnd,hdc,lpUpDown32->uPressButton,NOPRESS); // 绘制箭头
		ReleaseDC(hWnd,hdc);
		lpUpDown32->uPressButton = NOARROW;
	}
	return 0;
}
// **************************************************
// 声明：static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LOWORD ，X坐标
// 				 HIWORD ，Y坐标
// 返回值：无
// 功能描述：处理WM_LBUTTONUP消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   HDC hdc;

	ReleaseCapture(); // 释放鼠标
    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;

    // 恢复按钮状态
	hdc = GetDC(hWnd);
	DrawArrow(hWnd,hdc,lpUpDown32->uPressButton,NOPRESS); // 绘制窗口
	ReleaseDC(hWnd,hdc);
	lpUpDown32->uPressButton = NOARROW;
	return 0;
}

// **************************************************
// 声明：
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT nVirtKey ,按下键的虚键键值
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理键盘按键消息WM_KEYDOWN。
// 引用: 
// **************************************************
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
  DWORD dwStyle;

  	  // 判断是否需要处理按键消息
	    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
 	   if (!(dwStyle&UDS_ARROWKEYS))
  	    return 0;
		nVirtKey = (int) wParam;    // 虚键键值

		switch(nVirtKey)
		{
			case VK_UP:// 上箭头
		        ScrollArrow(hWnd,UPARROW);  
				break;
			case VK_DOWN:// 下箭头
  		      ScrollArrow(hWnd,DOWNARROW);
				break;
			default :
				return TRUE;
		};
		return TRUE;
}


// **************************************************
// 声明：static LRESULT DoGetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：UDM_GETACCEL 该消息保留。
// 引用: 
// **************************************************
static LRESULT DoGetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   return 0;
}
// **************************************************
// 声明：static LRESULT DoGetBase(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前所使用的数学进制
// 功能描述：得到当前所使用的数学进制，处理UDM_GETBASE消息。
// 引用: 
// **************************************************
static LRESULT DoGetBase(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;
    return lpUpDown32->iBase; // 返回当前的数学进制
}
// **************************************************
// 声明：static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回与当前控件绑定的窗口句柄
// 功能描述：返回与当前控件绑定的窗口，处理UDM_GETBUDDY消息。
// 引用: 
// **************************************************
static LRESULT DoGetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return NULL;
    return (LRESULT)lpUpDown32->hwndBuddy; // 返回绑定窗口
}
// **************************************************
// 声明：static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄。
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前的值。
// 功能描述：得到当前的值，处理UDM_GETPOS消息。
// 引用: 
// **************************************************
static LRESULT DoGetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   CHAR lpNumber[28];
   BOOL bSeparator=TRUE;
   DWORD dwStyle;
   long iPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return -1;
    if (lpUpDown32->hwndBuddy==NULL)
       return -1;

    // 得到是否有间隔符
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;
    // 得到当前的内容
    SendMessage(lpUpDown32->hwndBuddy,WM_GETTEXT,(WPARAM)28,(LPARAM)lpNumber); // 设置文本
    iPos=CharToLong(lpNumber,lpUpDown32->iBase,bSeparator);
    if (IsInRange(lpUpDown32,iPos)==FALSE)
      return -1;
    return iPos;
}
// **************************************************
// 声明：static LRESULT DoGetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回控件的范围，LOWORD -- 最大值，HIWORD -- 最小值。
// 功能描述：得到控制的范围，处理UDM_GETRANGE消息。
// 引用: 
// **************************************************
static LRESULT DoGetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0);  // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;
    return MAKELONG((WORD)lpUpDown32->iUpper,(WORD)lpUpDown32->iLower); // 返回范围
}
// **************************************************
// 声明：static LRESULT DoGetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	OUT wParam -- LPINT lpiHight 得到最大值。
// 	OUT lParam -- LPINT lpiLow 得到最小值。
// 返回值：无
// 功能描述：得到控制范围，处理UDM_GETRANGE32消息。
// 引用: 
// **************************************************
static LRESULT DoGetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   LPINT lpiHigh,lpiLow;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // 映射指针
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

    lpiLow =(LPINT)wParam;
    lpiHigh=(LPINT)lParam;

    *lpiHigh=lpUpDown32->iUpper; // 设置最大值
    *lpiLow=lpUpDown32->iLower; // 设置最小值

    return 0;
}
// **************************************************
// 声明：static LRESULT DoGetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前的UNICODE格式。
// 功能描述：得到UNICODE格式，处理UDM_GETUNICODEFORMAT消息。
// 引用: 
// **************************************************
static LRESULT DoGetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;
    return lpUpDown32->bUnionCode; // 返回UNICODE
}
// **************************************************
// 声明：static LRESULT DoSetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam --  保留
// 	IN lParam --  保留
// 返回值：
// 功能描述：UDM_SETACCEL消息保留
// 引用: 
// **************************************************
static LRESULT DoSetAccel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   return 0;
}
// **************************************************
// 声明：static LRESULT DoSetBase(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- WORD nBase , 要设定的数学进制
// 	IN lParam -- 保留
// 返回值：成功返回原来的进制，否则返回0。
// 功能描述：设定新的数学进制，处理UDM_SETBASE消息。
// 引用: 
// **************************************************
static LRESULT DoSetBase(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   WORD nBase,nOldBase;
   CHAR lpNumber[28];
   DWORD dwStyle;
   BOOL bSeparator=TRUE;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;
    nBase=(WORD)wParam; // 得到进制
    if (nBase!=10&&nBase!=16)
      return 0;

    nOldBase=lpUpDown32->iBase;
    if (lpUpDown32->iBase!=nBase)
    {
      // 要设置新的数学进制
      if (dwStyle&UDS_SETBUDDYINT)
      {
         if (lpUpDown32->hwndBuddy)
         {
           SendMessage(lpUpDown32->hwndBuddy,WM_GETTEXT,(WPARAM)28,(LPARAM)lpNumber); // 得到绑定窗口的文本
           lpUpDown32->iPos=CharToLong(lpNumber,lpUpDown32->iBase,bSeparator);
         }

         lpUpDown32->iBase=nBase; // 设置进制

         if (lpUpDown32->hwndBuddy)
         {
           LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator);
           SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // 设置绑定窗口的文本
           InvalidateRect(lpUpDown32->hwndBuddy,NULL,TRUE); // 无效窗口
         }
      }
    }
    return nOldBase;
}
// **************************************************
// 声明：static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- HWND hWnd 要绑定的窗口句柄。
// 	IN lParam -- 保留
// 返回值：返回原来的绑定窗口句柄。
// 功能描述：设定新的绑定窗口，处理UDM_SETBUDDY消息。
// 引用: 
// **************************************************
static LRESULT DoSetBuddy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   HWND hprevWnd;
   CHAR lpNumber[28];
   BOOL bSeparator=TRUE;
   DWORD dwStyle;
   long iOldPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;
    hprevWnd=lpUpDown32->hwndBuddy; // 得到前一个绑定窗口
    lpUpDown32->hwndBuddy=(HWND)wParam; // 设置新的绑定窗口

    if (lpUpDown32->hwndBuddy)
    {
      SendMessage(lpUpDown32->hwndBuddy,WM_GETTEXT,(WPARAM)28,(LPARAM)lpNumber); // 得到绑定窗口的文本
      iOldPos=lpUpDown32->iPos; 
      lpUpDown32->iPos=CharToLong(lpNumber,lpUpDown32->iBase,bSeparator); // 转化为数字
      SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // 发送通知消息

    }

    return (LRESULT)hprevWnd;
}
// **************************************************
// 声明：static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD iPos ,要设置的当前值。
// 	IN lParam -- 保留
// 返回值：返回原来的当前值。
// 功能描述：设置新的当前值，处理UDM_SETPOS消息。
// 引用: 
// **************************************************
static LRESULT DoSetPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   CHAR lpNumber[28];
   short iPos,iOldPos;
   DWORD dwStyle;
   BOOL bSeparator=TRUE;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    if (dwStyle&UDS_NOTHOUSANDS)
       bSeparator=FALSE;

//    iPos=LOWORD(wParam);
    iPos=LOWORD(lParam);
    if (IsInRange(lpUpDown32,iPos)==FALSE) // 当前的位置是否在控制范围内
       return 0;

    iOldPos=(short)lpUpDown32->iPos;
    lpUpDown32->iPos=iPos;

    if (lpUpDown32->hwndBuddy)
    {
      if (dwStyle&UDS_SETBUDDYINT)
      {
         LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator); // 数字转化为字串
         SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // 设置绑定窗口的文本

         InvalidateRect(lpUpDown32->hwndBuddy,NULL,TRUE); // 无效窗口
      }

    }
    SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // 发送通知消息
    return iOldPos;
}
// **************************************************
// 声明：static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam --  LOWORD iIpper, 最大值
// 				  HIWORD iLower, 最小值
// 返回值：无
// 能描述：设定新的控件范围，处理UDM_SETRANGE消息。
// 引用: 
// **************************************************
static LRESULT DoSetRange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   CHAR lpNumber[28];
   short iUpper,iLower;
   DWORD dwStyle;
   BOOL bSeparator=TRUE;
   long iOldPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;

    iUpper=LOWORD(lParam); // 得到最大值
    iLower=HIWORD(lParam); // 得到最小值

    lpUpDown32->iUpper=iUpper; // 设置范围
    lpUpDown32->iLower=iLower;

    if (IsInRange(lpUpDown32,lpUpDown32->iPos)==FALSE) // 当前位置是否在范围内
    {
      iOldPos=lpUpDown32->iPos;
      lpUpDown32->iPos=lpUpDown32->iLower;
      if (dwStyle&UDS_SETBUDDYINT)
      { // 设置绑定窗口的数字
        if (lpUpDown32->hwndBuddy)
        {
          LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator); // 将数字转化为字串
          SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // 设置绑定窗口的文本
        }
      }
      SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // 发送通知消息
    }
    if (lpUpDown32->iUpper>lpUpDown32->iLower)
      lpUpDown32->iInc=1;
    else
      lpUpDown32->iInc=-1;

    return 0;
}
// **************************************************
// 声明：static LRESULT DoSetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- iLow, 最小值
// 	IN lParam -- iHigh ,最大值
// 返回值：无
// 功能描述：设定新的控件范围，处理UDM_SETRANGE32消息。
// 引用: 
// **************************************************
static LRESULT DoSetRange32(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   CHAR lpNumber[28];
   long iHigh,iLow;
   DWORD dwStyle;
   BOOL bSeparator=TRUE;
   long iOldPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    if (dwStyle&UDS_NOTHOUSANDS)
      bSeparator=FALSE;

    iHigh=(long)lParam; // 得到最大值
    iLow =(long)wParam; // 得到最小值

    lpUpDown32->iUpper=iHigh;
    lpUpDown32->iLower=iLow;

    if (IsInRange(lpUpDown32,lpUpDown32->iPos)==FALSE) // 判断当前位置是否在范围内
    { // 不在范围内
      iOldPos=lpUpDown32->iPos;
      lpUpDown32->iPos=lpUpDown32->iLower; // 设置当前位置为最小值
      if (dwStyle&UDS_SETBUDDYINT)
      {
        if (lpUpDown32->hwndBuddy)
        {
          LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator); // 数字转化为字串
          SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // 设置绑定窗口文本
        }
      }
      SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // 发送通知消息
    }
    if (lpUpDown32->iUpper>lpUpDown32->iLower)
      lpUpDown32->iInc=1;
    else
      lpUpDown32->iInc=-1;
    return 0;
}
// **************************************************
// 声明：static LRESULT DoSetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- BOOL bUnionCode,要设定的unicode格式
// 	IN lParam -- 保留
// 返回值：返回原来的unicode格式
// 功能描述：设定新的unicode格式，处理UDM_SETUNICODEFORMAT消息。
// 引用: 
// **************************************************
static LRESULT DoSetUnionCodeFormat(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPUPDOWN32 lpUpDown32;
   BOOL bUnionCode,bPrev;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return 0;

    bUnionCode=(BOOL)wParam; // 得到UNICODE
    bPrev=lpUpDown32->bUnionCode; // 得到前一个UNICODE
    lpUpDown32->bUnionCode=bUnionCode; // 设置UNICODE

    return bPrev;
}

// **************************************************
// 声明：static long CharToLong(CHAR *lpNumber,WORD iBase,BOOL bSeparator)
// 参数：
// 	IN lpNumber -- 要转化的字符串
// 	IN iBase -- 要转化的字符串的数学进制
// 	IN bSeparator  -- 字符串中有没有间隔符
// 返回值：返回转化出来的数字。
// 功能描述：将字符串转化为数字。
// 引用: 
// **************************************************
static long CharToLong(CHAR *lpNumber,WORD iBase,BOOL bSeparator)
{
  long iResult,iBitValue;
  WORD i;
  BOOL bNegative=FALSE;

     iResult=0;
     i=0;

     if (lpNumber[i]=='-')
     { // 是负数
       bNegative=TRUE;
       i++;
     }
     else if (lpNumber[i]=='+')
     { // 是正数
       i++;
     }
     for (;lpNumber[i]!=0;i++)
     { // 得到当前的字符
        iBitValue=GetBitValue(lpNumber[i],iBase,bSeparator);
        if (iBitValue==SEPARATOR)
          continue; // 是风格符
        if (iBitValue==INVALIDNUMBER)
          break; // 非法数字
        iResult=iResult*iBase+iBitValue; // 得到当前的值
     }
     if (bNegative)
       iResult*=-1; //得到负数
     return iResult;
}
// **************************************************
// 声明：static void LongToChar(long iValue, CHAR *lpNumber, WORD iLen,WORD iBase,BOOL bSeparator)
// 参数：
// 	IN iValue -- 要转化的数字
// 	OUT lpNumber -- 用来存储转化后的字符串的缓存
// 	IN iLen -- 缓存的大小
// 	IN iBase -- 要转化的字符串的数学进制
// 	IN bSeparator  -- 字符串中有没有间隔符
// 返回值：无
// 功能描述：将数字转化为字符串
// 引用: 
// **************************************************
static void LongToChar(long iValue, CHAR *lpNumber, WORD iLen,WORD iBase,BOOL bSeparator)
{
  CHAR chTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  CHAR chTemp[32];
  BOOL bNegative=FALSE;
  WORD i=0,j=0;
  long iBitValue;

     if (iValue<0)
     { // 是负数
        bNegative=TRUE;
        iValue*=-1;
     }
     while(iValue)
     { // 还有数字
       if (!(i%3) && i != 0)
       { // 添加分隔符
          if (bSeparator)
            chTemp[i++]=',';
       }
       iBitValue=iValue%iBase; // 得到当前的数字
       chTemp[i++]=chTable[iBitValue]; // 得到字符
       iValue/=iBase;
     }
     if (i)
     {
       if (bNegative)
         lpNumber[j++]='-'; // 是负数
       while(i)
       {
         lpNumber[j++]=chTemp[--i]; // 复制当前的字串
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
// 声明：static int GetBitValue(CHAR chBit,WORD iBase,BOOL bSeparator)
// 参数：
// 	IN chBit -- 要转化的字符
// 	IN iBase -- 数字进制
// 	IN bSeparator -- 是否有间隔符
// 返回值：成功返回转化后数字，否则返回INVALIDNUMBER。
// 功能描述：将指定字符转化为数字
// 引用: 
// **************************************************
static int GetBitValue(CHAR chBit,WORD iBase,BOOL bSeparator)
{
       switch(chBit)
       {
          case '0':  // 数字
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
          case 'A': // 16进制数字
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
          case 'a': // 16进制数字
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
          case ',': // 分隔符
                   if (bSeparator)
                      return SEPARATOR;
       }
       return INVALIDNUMBER; // 其他字符是非法字符
}
// **************************************************
// 声明：static WORD GetPosition(HWND hWnd,POINT point)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN point -- 点的坐标
// 返回值：返回点的位置。
// 功能描述：得到指定的点在控件的位置。
// 引用: 
// **************************************************
static WORD GetPosition(HWND hWnd,POINT point)
{
  RECT rect,rectUpArrow,rectDownArrow;
  DWORD dwStyle;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    GetClientRect(hWnd,&rect);
	  if( dwStyle&UDS_HORZ )
	  { // 水平控制
// !!! Add By  Jami chen in 2004.07.15
		  if (dwStyle&UDS_VERTSHOW)
		  {
		  	  // 得到左箭头的位置
			  rectUpArrow.left=rect.left;
			  rectUpArrow.right=rect.right;
			  rectUpArrow.top=rect.top;
			  rectUpArrow.bottom=rect.top+(rect.bottom-rect.top)/2;
			  // 得到右箭头的位置
			  rectDownArrow.left=rect.left;
			  rectDownArrow.right=rect.right;
			  rectDownArrow.top=rectUpArrow.bottom+1;
			  rectDownArrow.bottom=rect.bottom;
		  }
		  else
// !!! Add End By  Jami chen in 2004.07.15
		  {
	  		  // 得到左箭头的位置
			  rectUpArrow.top=rect.top;
			  rectUpArrow.bottom=rect.bottom;
			  rectUpArrow.left=rect.left;
			  rectUpArrow.right=rect.left+(rect.right-rect.left)/2;
			  // 得到右箭头的位置
			  rectDownArrow.top=rect.top;
			  rectDownArrow.bottom=rect.bottom;
			  rectDownArrow.left=rectUpArrow.right+1;
			  rectDownArrow.right=rect.right;
		  }
	  }
	  else
	  { // 垂直控制
// !!! Add By  Jami chen in 2004.07.15
		  if (dwStyle&UDS_HORZSHOW)
		  {
		  	  // 得到上箭头的位置
			  rectUpArrow.top=rect.top;
			  rectUpArrow.bottom=rect.bottom;
			  rectUpArrow.left=rect.left;
			  rectUpArrow.right=rect.left+(rect.right-rect.left)/2;
			  // 得到下箭头的位置
			  rectDownArrow.top=rect.top;
			  rectDownArrow.bottom=rect.bottom;
			  rectDownArrow.left=rectUpArrow.right+1;
			  rectDownArrow.right=rect.right;
		  }
		  else
// !!! Add End By  Jami chen in 2004.07.15
		  {
		  	  // 得到上箭头的位置
			  rectUpArrow.left=rect.left;
			  rectUpArrow.right=rect.right;
			  rectUpArrow.top=rect.top;
			  rectUpArrow.bottom=rect.top+(rect.bottom-rect.top)/2;
			  // 得到下箭头的位置
			  rectDownArrow.left=rect.left;
			  rectDownArrow.right=rect.right;
			  rectDownArrow.top=rectUpArrow.bottom+1;
			  rectDownArrow.bottom=rect.bottom;
		  }
	  }
    if( PtInRect( &rectUpArrow, point ) )
       return UPARROW; // 当前点在上箭头
    if( PtInRect( &rectDownArrow, point ) )
       return DOWNARROW; // 当前点在下箭头
    return NOARROW;
}

// **************************************************
// 声明：static BOOL IsInRange(LPUPDOWN32 lpUpDown32,long iPos)
// 参数：
// 	IN LPUPDOWN32 lpUpDown32 -- 控件结构
// 	IN iPos -- 要测试的值
// 返回值：在控件范围内，返回TRUE，否则返回FALSE。
// 功能描述：判断指定的值是不是在控件范围内。
// 引用: 
// **************************************************
static BOOL IsInRange(LPUPDOWN32 lpUpDown32,long iPos)
{
    if (lpUpDown32->iUpper>lpUpDown32->iLower)
    {  
       if ((iPos>lpUpDown32->iUpper)||(iPos<lpUpDown32->iLower))
         return FALSE; // 当前点不在范围内
    }
    else
    {
       if ((iPos<lpUpDown32->iUpper)||(iPos>lpUpDown32->iLower))
         return FALSE; // 当前点不在范围内
    }
    return TRUE;
}

// **************************************************
// 声明：static BOOL ScrollArrow(HWND hWnd,WORD iDirection)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iDirection  -- 箭头方向
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：滚动箭头
// 引用: 
// **************************************************
static BOOL ScrollArrow(HWND hWnd,WORD iDirection)
{
  DWORD dwStyle;
  LPUPDOWN32 lpUpDown32;
  CHAR lpNumber[28];
  BOOL bSeparator=TRUE;
  long iOldPos;

    lpUpDown32=(LPUPDOWN32)GetWindowLong(hWnd,0); // 得到UPDOWN32的结构
    if (lpUpDown32==NULL)
       return FALSE;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    if (dwStyle&UDS_NOTHOUSANDS)
       bSeparator=FALSE;

    iOldPos=lpUpDown32->iPos;
    if (lpUpDown32->hwndBuddy!=NULL)
    { // 有绑定窗口
      SendMessage(lpUpDown32->hwndBuddy,WM_GETTEXT,(WPARAM)27,(LPARAM)lpNumber); // 得到绑定窗口的文本
      lpUpDown32->iPos=CharToLong(lpNumber,lpUpDown32->iBase,bSeparator); // 将字串转化为数字
    }
    if (IsInRange(lpUpDown32,lpUpDown32->iPos)==FALSE) // 当前位置是否在范围内
       lpUpDown32->iPos=lpUpDown32->iLower;
    if (iDirection==UPARROW)
    { // 往上走
      if (lpUpDown32->iPos==lpUpDown32->iUpper)
      { // 已经到最大值
         if (dwStyle&UDS_WRAP)
           lpUpDown32->iPos=lpUpDown32->iLower;
         else
           return FALSE;
      }
      else
        lpUpDown32->iPos+=lpUpDown32->iInc; // 增加一个增量
    }
    else
    { // 往下走
      if (lpUpDown32->iPos==lpUpDown32->iLower)
      { // 已经到最小值
         if (dwStyle&UDS_WRAP)
           lpUpDown32->iPos=lpUpDown32->iUpper;
         else
           return FALSE;
      }
      else
        lpUpDown32->iPos-=lpUpDown32->iInc; // 减去一个增量
    }

    if (dwStyle&UDS_SETBUDDYINT)
    { // 需要设置绑定窗口
       if (lpUpDown32->hwndBuddy!=NULL)
       {
         LongToChar(lpUpDown32->iPos,lpNumber,28,lpUpDown32->iBase,bSeparator); // 数字转化为字串
         SendMessage(lpUpDown32->hwndBuddy,WM_SETTEXT,(WPARAM)0,(LPARAM)lpNumber); // 设置绑定窗口的文本
         InvalidateRect(lpUpDown32->hwndBuddy,NULL,TRUE); // 无效绑定窗口
       }
    }
    SendNotify(hWnd,lpUpDown32->iPos,lpUpDown32->iPos-iOldPos); // 发送通知消息
    return TRUE;
}

// **************************************************
// 声明：static void SendNotify(HWND hWnd,long iPos,long iDelta)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iPos -- 当前位置
// 	IN iDelta -- 改变步长
// 返回值：无
// 功能描述：发送通知消息。
// 引用: 
// **************************************************
static void SendNotify(HWND hWnd,long iPos,long iDelta)
{
	NMUPDOWN nmUpDown;
	HWND hParent;
	// 设置通知消息结构
	nmUpDown.hdr.hwndFrom=hWnd;
	nmUpDown.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
	nmUpDown.hdr.code=UDN_DELTAPOS;
	nmUpDown.iPos=iPos;
	nmUpDown.iDelta=iDelta;
	hParent=GetParent(hWnd); // 得到父窗口句柄
	SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmUpDown); // 发送通知消息
}


// **************************************************
// 声明：static void DrawArrow(HWND hWnd, HDC hdc, UINT uDirection, UINT uStatus)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN uDirection -- 箭头方向
// 	IN uStatus -- 箭头状态
// 返回值：无
// 功能描述：绘制箭头。
// 引用: 
// **************************************************
static void DrawArrow(HWND hWnd, HDC hdc, UINT uDirection, UINT uStatus)
{
   RECT rect,rectArrow;
   DWORD dwStyle;
   HFONT hFont;
   int iOldMode;

	  dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
	  if (dwStyle & WS_DISABLED)
	  { // DISABLE风格
//		SetTextColor(hdc,RGB(0x80,0x80,0x80));
		SetTextColor(hdc,GetSysColor(COLOR_GRAYTEXT));
	  }
	  hFont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_ENGLISH8X8) );//hEngFont8x8 ); // 得到字体
      GetClientRect(hWnd,&rect); // 得到客户矩形
      iOldMode = SetBkMode( hdc, TRANSPARENT );
	  if( dwStyle&UDS_HORZ )
	  {  // 水平箭头
		  if (uDirection == UPARROW)
		  { // 左箭头
// !!! Add By  Jami chen in 2004.07.15
			if (dwStyle&UDS_VERTSHOW)
			{
			  // 得到左箭头的位置
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
			{  // 箭头按下
				DrawEdge( hdc, &rectArrow, BDR_SUNKENOUTER, BF_RECT );
			}
			else  
			{  // 箭头没有按下
				DrawEdge( hdc, &rectArrow, BDR_RAISEDOUTER, BF_RECT );
			}
			InflateRect( &rectArrow, -1, -1 );

			DrawText( hdc, "\x11", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

		  }
		  else
		  { // 右箭头
// !!! Add By  Jami chen in 2004.07.15
			if (dwStyle&UDS_VERTSHOW)
			{
			  // 得到右箭头的位置
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
			{ // 箭头按下
				DrawEdge( hdc, &rectArrow, BDR_SUNKENOUTER, BF_RECT );
			}
			else  
			{ // 箭头没有按下
				DrawEdge( hdc, &rectArrow, BDR_RAISEDOUTER, BF_RECT );
			}
	        InflateRect( &rectArrow, -1, -1 );
          
			DrawText( hdc, "\x10", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

		  }
	  }
	  else
	  {  // 垂直箭头
          GetClientRect(hWnd,&rect);
		  if (uDirection == UPARROW)
		  {   // 上箭头
// !!! Add By  Jami chen in 2004.07.15
			  if (dwStyle&UDS_HORZSHOW)
			  {
		  		  // 得到上箭头的位置
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
			  { //箭头按下
				DrawEdge( hdc, &rectArrow, BDR_SUNKENOUTER, BF_RECT );
			  }
			  else  
			  { // 箭头没有按下
			 	DrawEdge( hdc, &rectArrow, BDR_RAISEDOUTER, BF_RECT );
			  }
			  InflateRect( &rectArrow, -1, -1 );

			  DrawText( hdc, "\x1e", 1, &rectArrow, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
		  }
		  else
		  {  
              // 下箭头
// !!! Add By  Jami chen in 2004.07.15
			  if (dwStyle&UDS_HORZSHOW)
			  {
				  // 得到下箭头的位置
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
			  {  // 箭头按下
				DrawEdge( hdc, &rectArrow, BDR_SUNKENOUTER, BF_RECT );
			  }
			  else  
			  { // 箭头没有按下
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
// 声明：static LRESULT DoKillFocus(HWND hWnd, WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：控件失去焦点。
// 引用: 
// **************************************************
static LRESULT DoKillFocus(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
	NMHDR nmHdr;

		nmHdr.code = NM_KILLFOCUS;
		nmHdr.hwndFrom = hWnd;
		nmHdr.idFrom = (UINT)GetWindowLong(hWnd,GWL_ID); // 得到窗口代码
		SendMessage(GetParent(hWnd),WM_NOTIFY,(WPARAM)nmHdr.idFrom,(LPARAM)&nmHdr); // 发送通知消息
		return 0;
}
