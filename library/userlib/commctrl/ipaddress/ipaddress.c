/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：IP地址输入窗口类
版本号：1.0.0
开发时期：2003-08-08
作者：
修改记录：
******************************************************/

#include <eWindows.h>
#include <eassert.h>
//#include <IPAddress.h>

#define ID_EDIT  230

#define MAX_SUBITEMLEN  4
/*
typedef struct IPAddressClassStruct
{
}IPADDRESSCLASS,* LPIPADDRESSCLASS;
*/

static const char classIPADDRESS[]="IPADDRESS";

//#define _MAPPOINTER

static LRESULT WINAPI IPAddressEditWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);


static LRESULT WINAPI IPAddressWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);
static VOID DoPaint(HWND hWnd,HDC hdc);
static void DoEditNotify(HWND hWnd,HWND hEdit,int wmEvent);
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void ToNextEdit(HWND hWnd,HWND hEdit);
static BOOL GetSubItemtext(LPTSTR lpsz,int iIndex,LPTSTR lpSubItemText,WORD wMaxlen);

// **************************************************
// 声明：ATOM RegisterIPAddressClass(HINSTANCE hInst)
// 参数：
// 	IN hInst -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册IP地址类。
// 引用: 
// **************************************************
ATOM RegisterIPAddressClass(HINSTANCE hInst)
{
   WNDCLASS wc;
// register IPAddress class
   wc.hInstance=hInst;
   wc.lpszClassName= classIPADDRESS; // 窗口类名
// the proc is class function
   wc.lpfnWndProc=(WNDPROC)IPAddressWndProc; // 窗口过程函数
   wc.style=0;
   wc.hIcon= 0;
// at pen window, no cursor
   wc.hCursor= LoadCursor(NULL,IDC_ARROW);
// to auto erase background, must set a valid brush
// if 0, you must erase background yourself
   wc.hbrBackground = GetStockObject( WHITE_BRUSH ); // 背景刷

   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
// !!! it's important to save state , align to long
   wc.cbWndExtra=sizeof( long );
   return RegisterClass(&wc); // 注册类
}
// this all message handle center
// **************************************************
// 声明：LRESULT WINAPI IPAddressWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN message -- 消息
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：返回消息的处理结果
// 功能描述：IP地址类窗口过程函数。
// 引用: 
// **************************************************
LRESULT WINAPI IPAddressWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
	    case WM_PAINT: // 绘制消息
			 // Paint this window
			 hdc = BeginPaint(hWnd, &ps);  // Get hdc
			 DoPaint(hWnd,hdc);
			 EndPaint(hWnd, &ps);  //release hdc
           break;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case ID_EDIT: // 编辑框通知消息
				case ID_EDIT + 1:
				case ID_EDIT + 2:
				case ID_EDIT + 3:
					DoEditNotify(hWnd,(HWND)lParam,wmEvent);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_SETTEXT:
			return DoSetText(hWnd,wParam,lParam); // 设置文本
		case WM_GETTEXT:
			return DoGetText(hWnd,wParam,lParam); // 得到文本
		case WM_STYLECHANGED:
			return DoStyleChanged(hWnd,wParam,lParam); // 风格改变
  		case WM_CREATE: // 创建消息
           return DoCreate(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏消息
			return DoDestroyWindow(hWnd);
      default:
	         // It's important to do this
	         // if your do'nt handle message, you must call DefwindowProc
	         return DefWindowProc(hWnd,message,wParam,lParam);
	}
  return 0;
}

// ********************************************************************
// 声明：static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	返回0，将继续创建窗口，返回-1，则会破坏窗口
// 功能描述：应用程序处理创建消息
// 引用: 
// ********************************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rectClient;
	HINSTANCE hInstance;
	HWND hEdit;
	DWORD dwWndProc;
	int i;
	int iCharWidth;
	int iEditPos = 0;
	HDC hdc;

	hdc = GetDC(hWnd);
	// get ASCII code width
	GetCharWidth(hdc,0x41,0x41,&iCharWidth);

	ReleaseDC(hWnd,hdc);

	GetClientRect(hWnd,&rectClient); // 得到客户矩形

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄

	// 创建4个输入IP地址的编辑区
	for (i = 0; i < 4; i++)
	{   // 创建编辑框
		iEditPos = 4 * iCharWidth * i;
		hEdit = CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,iEditPos,0,iCharWidth * 3+ 3,rectClient.bottom,hWnd,(HMENU)(ID_EDIT + i),hInstance,0);
		SendMessage(hEdit,EM_SETLIMITTEXT,3,0); // 设置文本限制，最大3个字符（255）
		dwWndProc = GetWindowLong(hEdit,GWL_WNDPROC); // 得到原来的编辑窗口过程函数
		SetWindowLong(hEdit,GWL_USERDATA,dwWndProc); // 设置原来的过程函数到用户数据
		SetWindowLong(hEdit,GWL_WNDPROC,(DWORD)IPAddressEditWndProc); // 设置新的过程函数
	}
/*
	hEdit = CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,32,0,27,rectClient.bottom,hWnd,ID_EDIT + 1,hInstance,0);
	SendMessage(hEdit,EM_SETLIMITTEXT,3,0);
	dwWndProc = GetWindowLong(hEdit,GWL_WNDPROC);
	SetWindowLong(hEdit,GWL_USERDATA,dwWndProc);
	SetWindowLong(hEdit,GWL_WNDPROC,(DWORD)IPAddressEditWndProc);
	
	hEdit = CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,64,0,27,rectClient.bottom,hWnd,ID_EDIT + 2,hInstance,0);
	SendMessage(hEdit,EM_SETLIMITTEXT,3,0);
	dwWndProc = GetWindowLong(hEdit,GWL_WNDPROC);
	SetWindowLong(hEdit,GWL_USERDATA,dwWndProc);
	SetWindowLong(hEdit,GWL_WNDPROC,(DWORD)IPAddressEditWndProc);
	
	hEdit = CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,96,0,27,rectClient.bottom,hWnd,ID_EDIT + 3,hInstance,0);
	SendMessage(hEdit,EM_SETLIMITTEXT,3,0);
	dwWndProc = GetWindowLong(hEdit,GWL_WNDPROC);
	SetWindowLong(hEdit,GWL_USERDATA,dwWndProc);
	SetWindowLong(hEdit,GWL_WNDPROC,(DWORD)IPAddressEditWndProc);
*/
    return 0;
}
// ********************************************************************
// 声明：static LRESULT DoDestroyWindow(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	返回TRUE，将继续破坏窗口，返回FALSE，则不破坏窗口
// 功能描述：应用程序处理破坏窗口的消息
// 引用: 
// ********************************************************************
static LRESULT DoDestroyWindow(HWND hWnd)
{
	return TRUE;
}
// ********************************************************************
// 声明：static VOID DoPaint(HWND hWnd,HDC hdc)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//    IN hdc  - 设备句柄
// 返回值：
//	
// 功能描述：应用程序处理 WM_PAINT 消息
// 引用: 
// ********************************************************************
static VOID DoPaint(HWND hWnd,HDC hdc)
{
  BOOL bEnable;
  int iDotPos = 0;
  int iCharWidth;
		// get ASCII code width
		GetCharWidth(hdc,0x41,0x41,&iCharWidth);
		// get window style
		bEnable = IsWindowEnabled(hWnd);
		SetBkColor(hdc,GetSysColor(COLOR_WINDOW)); // 设置背景颜色
		if (bEnable == FALSE)
			SetTextColor(hdc,GetSysColor(COLOR_GRAYTEXT)); // 设置颜色

		// 绘制三个分隔点
		iDotPos += 3 * iCharWidth + 1;
		TextOut(hdc,iDotPos,0 ,".",1);
		iDotPos += 4 * iCharWidth;
		TextOut(hdc,iDotPos,0 ,".",1);
		iDotPos += 4 * iCharWidth;
		TextOut(hdc,iDotPos,0 ,".",1);
}

// ********************************************************************
// 声明：static void ToNextEdit(HWND hWnd,HWND hEdit)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//    IN hEdit  - 当前活动的编辑区
// 返回值：
//	
// 功能描述：将激活下一个编辑区
// 引用: 
// ********************************************************************
static void ToNextEdit(HWND hWnd,HWND hEdit)
{
	UINT idCur;
	HWND hNewEdit;

		idCur = GetWindowLong(hEdit,GWL_ID); // 得到当前编辑框的ID号

		if (idCur == ID_EDIT + 3)
		{
			// Had Last Edit
			return ;
		}
		idCur ++ ; // 到下一个编辑框
		hNewEdit = GetDlgItem(hWnd,idCur); // 得到窗口句柄
		SetFocus(hNewEdit); // 设置焦点
}

// ********************************************************************
// 声明：static void DoEditNotify(HWND hWnd,HWND hEdit,int wmEvent)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//    IN hEdit  - 当前活动的编辑区
//    IN wmEvent - 编辑区发回的通知代码
// 返回值：
//	
// 功能描述：处理编辑区的通知消息
// 引用: 
// ********************************************************************
static void DoEditNotify(HWND hWnd,HWND hEdit,int wmEvent)
{
	if (wmEvent == EN_MAXTEXT)
	{ // 已经输满
		ToNextEdit(hWnd,hEdit); // 跳到下一个编辑框
	}
}

// ********************************************************************
// 声明：static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	
// 功能描述：处理WM_SETTEXT消息
// 引用: 
// ********************************************************************
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
	LPCTSTR lpsz;
	TCHAR lpSubItemText[MAX_SUBITEMLEN];
	HWND hEdit;
	int i;

		lpsz= (LPCTSTR)lParam; // address of window-text string
		for (i = 0; i < 4; i++)
		{
			if (GetSubItemtext((LPTSTR)lpsz,i,lpSubItemText,MAX_SUBITEMLEN) == FALSE) // 得到每个子窗口的字串
				break;
			hEdit = GetDlgItem(hWnd,ID_EDIT + i); // 得到对应的窗口句柄
			SetWindowText(hEdit,lpSubItemText); // 设置内容
		}
		return TRUE;
}
// ********************************************************************
// 声明：static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	
// 功能描述：处理WM_SETTEXT消息
// 引用: 
// ********************************************************************
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
	LPTSTR lpsz;
//	WORD wMaxLen;
	TCHAR lpSubItemText[MAX_SUBITEMLEN];
	HWND hEdit;
	int i;

		lpsz= (LPTSTR)lParam; // address of window-text string
		for (i = 0; i < 4; i++)
		{
			if (i != 0)
			{
				strcat(lpsz,(const char *)"."); // 添加分隔符
			}
			hEdit = GetDlgItem(hWnd,ID_EDIT + i); // 得到指定编辑框的窗口句柄
			GetWindowText(hEdit,lpSubItemText,MAX_SUBITEMLEN); // 得到内容
			strcat(lpsz,lpSubItemText); // 加到用户字串中
		}
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	
// 功能描述：处理WM_SETTEXT消息
// 引用: 
// ********************************************************************
static BOOL GetSubItemtext(LPTSTR lpsz,int iIndex,LPTSTR lpSubItemText,WORD wMaxlen)
{
	int iCurIndex =0;
	int iCurLen = 0;
	
	// 得到指定索引的文本开始位置
	while(iCurIndex != iIndex)
	{
		if (*lpsz == '.') // 是分隔符
			iCurIndex ++; // 到下一个索引
		lpsz ++;
	}
	while(1)
	{
		if (iCurLen >= wMaxlen -1)
			break; // 非法长度
		if (*lpsz >'9' || *lpsz < '0')
			break; // 非法数据
		*lpSubItemText++ = *lpsz++; // 得到数据
		iCurLen ++;
	}
	*lpSubItemText = 0;
	return TRUE;
}

// **************************************************
// 声明：static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：无
// 功能描述：处理窗口风格改变的消息。
// 引用: 
// **************************************************
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  BOOL bEnable;
	HWND hEdit;
	int i;

		// get window style
		bEnable = IsWindowEnabled(hWnd); // 得到窗口的状态


		for (i = 0; i < 4; i++)
		{
			hEdit = GetDlgItem(hWnd,ID_EDIT + i);
			EnableWindow(hEdit,bEnable); // 将每个编辑框的状态设置成当前窗口的状态
		}

		return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam);
}


// **************************************************
// 声明：static LRESULT DoEditKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：无
// 功能描述：编辑区处理键盘输入消息。
// 引用: 
// **************************************************
static LRESULT DoEditKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;

		nVirtKey = (int) wParam;    // virtual-key code
		//RETAILMSG(1,(" Get Keydown = %x \r\n",nVirtKey));
		if (nVirtKey == VK_PERIOD)
		{ // 当前输入的是一个点
			ToNextEdit(GetParent(hWnd),hWnd); // 跳到下一个编辑区
		}
		return 0;
}

// **************************************************
// 声明：static LRESULT WINAPI IPAddressEditWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN message -- 消息代码
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：返回消息处理结果
// 功能描述：IP地址处理编辑去过程函数。
// 引用: 
// **************************************************
static LRESULT WINAPI IPAddressEditWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	WNDPROC lpPrevWndFunc;
	lpPrevWndFunc = (WNDPROC)GetWindowLong(hWnd,GWL_USERDATA);
	switch( message )
	{
		case WM_KEYDOWN: // 处理键盘输入消息
			DoEditKeyDown(hWnd,wParam,lParam); 
			break;
	}
	return CallWindowProc(lpPrevWndFunc,hWnd,message,wParam,lParam); // 呼叫原来的窗口过程函数
}
/**********************************************************************************/
