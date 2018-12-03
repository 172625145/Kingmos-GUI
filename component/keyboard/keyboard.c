/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：键盘处理函数
版本号：1.0.0.456
开发时期：2002-03-15
作者：陈建明 Jami chen 
修改记录：
**************************************************/
#include <eframe.h>
//#include <eprivate.h>
#include <softkey.h>
#include <eptrlist.h>
#include <elstctrl.h>
#include <thirdpart.h>

// *************************************************
// 定义区域
// *************************************************
typedef struct {
	int (*Who)(HWND hWnd);
	void (*Create)(HWND hWnd);
	void (*Destroy)(HWND hWnd);
	void (*Resume)(HWND hWnd);
	int (*DoPAINT)( HWND hWnd, HDC hdc );
	int (*DoLBUTTONDOWN)( HWND hWnd, int x, int y );
	int (*DoLBUTTONUP)( HWND hWnd, int x, int y );
	int (*DoMOUSEMOVE)( HWND hWnd, int x, int y );
	BOOL (*GetKeyValue)( HWND hWnd,WORD * );
	LRESULT (*DoDefWindowProc)(HWND hWnd,UINT uMag,WPARAM wParam,LPARAM lParam);
}KEYDRV, FAR * LPKEYDRV;

typedef const KEYDRV * LPCKEYDRV;

#define KEYBOARDSTARTX  0
//efine KEYBOARDSTARTY  (320-TOOL_BAR_HEIGHT-KEYBOARDHEIGHT)
#define KEYBOARDSTARTY  (320-20-KEYBOARDHEIGHT)

#define VIRTUALSAMBOLNUM 30
struct KeyValue{
   UINT uASCIICode;
   UINT uVirtualKey;
//   UINT uScanCode;
};
static struct KeyValue VK_CharCodeTable[]=
{
   {0x00,0X0332},
   {0x01,0X0241},
   {0x02,0X0242},
   {0x03,0X0003},
   {0x04,0X0244},
   {0x05,0X0245},
   {0x06,0X0246},
   {0x07,0X0247},
   {0x08,0X0008},
   {0x09,0X0009},
   {0x0A,0X020D},
   {0x0B,0X024B},
   {0x0C,0X024C},
   {0x0D,0X000D},
   {0X0E,0X024E},
   {0x0F,0X024F},
   {0x10,0X0250},
   {0x11,0X0251},
   {0x12,0X0252},
   {0x13,0X0253},
   {0x14,0X0254},
   {0x15,0X0255},
   {0x16,0X0256},
   {0x17,0X0257},
   {0x18,0X0258},
   {0x19,0X0259},
   {0x1A,0X025A},
   {0x1B,0X02DB},
   {0x1C,0X02DC},
   {0x1D,0X02DD},
   {0X1E,0X0336},
   {0x1F,0X03BD},
   {0x20,0X0020},
   {0x21,0X0131},
   {0x22,0X01DE},
   {0x23,0X0133},
   {0x24,0X0134},
   {0x25,0X0135},
   {0x26,0X0137},
   {0x27,0X00DE},
   {0x28,0X0139},
   {0x29,0X0130},
   {0x2A,0X0138},
   {0x2B,0X01BB},
   {0x2C,0X00BC},
   {0x2D,0X00BD},
   {0X2E,0X00BE},
   {0x2F,0X00BF},
   {0x30,0X0030},
   {0x31,0X0031},
   {0x32,0X0032},
   {0x33,0X0033},
   {0x34,0X0034},
   {0x35,0X0035},
   {0x36,0X0036},
   {0x37,0X0037},
   {0x38,0X0038},
   {0x39,0X0039},
   {0x3A,0X01BA},
   {0x3B,0X00BA},
   {0x3C,0X01BC},
   {0x3D,0X00BB},
   {0X3E,0X01BE},
   {0x3F,0X01BF},
   {0x40,0X0132},
   {0x41,0X0141},
   {0x42,0X0142},
   {0x43,0X0143},
   {0x44,0X0144},
   {0x45,0X0145},
   {0x46,0X0146},
   {0x47,0X0147},
   {0x48,0X0148},
   {0x49,0X0149},
   {0x4A,0X014A},
   {0x4B,0X014B},
   {0x4C,0X014C},
   {0x4D,0X014D},
   {0X4E,0X014E},
   {0x4F,0X014F},
   {0x50,0X0150},
   {0x51,0X0151},
   {0x52,0X0152},
   {0x53,0X0153},
   {0x54,0X0154},
   {0x55,0X0155},
   {0x56,0X0156},
   {0x57,0X0157},
   {0x58,0X0158},
   {0x59,0X0159},
   {0x5A,0X015A},
   {0x5B,0X00DB},
   {0x5C,0X00DC},
   {0x5D,0X00DD},
   {0X5E,0X0136},
   {0x5F,0X01DB},
   {0x60,0X00C0},
   {0x61,0X0041},
   {0x62,0X0042},
   {0x63,0X0043},
   {0x64,0X0044},
   {0x65,0X0045},
   {0x66,0X0046},
   {0x67,0X0047},
   {0x68,0X0048},
   {0x69,0X0049},
   {0x6A,0X004A},
   {0x6B,0X004B},
   {0x6C,0X004C},
   {0x6D,0X004D},
   {0X6E,0X004E},
   {0x6F,0X004F},
   {0x70,0X0050},
   {0x71,0X0051},
   {0x72,0X0052},
   {0x73,0X0053},
   {0x74,0X0054},
   {0x75,0X0055},
   {0x76,0X0056},
   {0x77,0X0057},
   {0x78,0X0058},
   {0x79,0X0059},
   {0x7A,0X005A},
   {0x7B,0X01DB},
   {0x7C,0X01DC},
   {0x7D,0X01DD},
   {0X7E,0X01C0},
   {0x7F,0X0208},
};



extern KEYDRV engKeyboard;
extern KEYDRV pyKeyboard;

KEYDRV *drvKeyboard[]={(KEYDRV *)&pyKeyboard,(KEYDRV *)&engKeyboard};

static UINT VirtualKeyToChar(UINT VirtualKey);
SHORT VkKeyScan(TCHAR uChar);


typedef struct structKeyboard{
	int iOldKeyboard;
	int iCurKeyboard;
}STRUCT_KEYBOARD, *LPSTRUCT_KEYBOARD;

#if 0
//static int iOldKeyboard = KB_SPELL;
//static int iCurKeyboard = KB_SPELL;
static int iOldKeyboard = KB_HANDWRITE;//KB_SPELL;
static int iCurKeyboard = KB_HANDWRITE;//KB_SPELL;
static BOOL g_bKBCreate = FALSE;

#endif

// *************************************************
// 函数声明区域
// *************************************************
static LRESULT CALLBACK KeyboardWndProc( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static LRESULT DoSETKEYDRV( HWND hWnd, LPCKEYDRV );
static LRESULT DoPAINT( HWND );
static LRESULT DoLBUTTONDOWN( HWND, LPARAM );
static LRESULT DoLBUTTONUP( HWND, LPARAM );
static LRESULT DoMOUSEMOVE( HWND, LPARAM );
static LRESULT DoDefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

static LRESULT DoSetNamedKeybd(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoResumeKeyboard( HWND hWnd);
static LRESULT DoGoBackKB(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd,WPARAM wParam , LPARAM lParam);

static const char classKEYBOARD[] = "Keyboard";


//
// **************************************************
// 声明：BOOL RegisterKeyboardClass( HINSTANCE hInst )
// 参数：
// 	IN hInst -- 实例句柄
// 
// 返回值：返回注册结果
// 功能描述：注册键盘类。
// 引用: 
// **************************************************
BOOL RegisterKeyboardClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = KeyboardWndProc;  // 键盘类过程函数
    wc.cbClsExtra = sizeof(DWORD);
    wc.cbWndExtra = sizeof( LPKEYDRV ) + 8;
    wc.hInstance = hInst;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = classKEYBOARD; // 类名

	return (BOOL)(RegisterClass( &wc )); // 向系统注册键盘类
}

// **************************************************
// 声明：static LRESULT DoSETKEYDRV( HWND hWnd, LPCKEYDRV lpdrv )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpdrv -- 要设置为当前键盘的键盘驱动。
// 
// 返回值：无
// 功能描述：设置键盘驱动。
// 引用: 
// **************************************************
static LRESULT DoSETKEYDRV( HWND hWnd, LPCKEYDRV lpdrv )
{
	LPCKEYDRV lpOlddrv;
	if (lpdrv == 0)
		return 0;
	lpOlddrv = (LPCKEYDRV)GetWindowLong( hWnd, 0); // 得到旧的键盘驱动类
	if (lpOlddrv != 0)
	{ // 原来存在键盘
		lpOlddrv->Destroy(hWnd); // 破坏原来的键盘
	}
	SetWindowLong( hWnd, 0, (LONG)lpdrv ); // 将键盘驱动设置到当前键盘类
	lpdrv->Create(hWnd); // 创建该键盘
	InvalidateRect( hWnd, 0, TRUE );  // 重绘键盘
	return 0;
}

// **************************************************
// 声明：static LRESULT DoPAINT( HWND hWnd )
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：绘制键盘。
// 引用: 
// **************************************************
static LRESULT DoPAINT( HWND hWnd )
{
	PAINTSTRUCT ps;
	LPKEYDRV lpdrv = (LPKEYDRV)GetWindowLong( hWnd, 0 ); // 得到键盘驱动
	HDC hdc = BeginPaint( hWnd, &ps ); // 开始绘制
	RECT rect;
	if( lpdrv )
	    lpdrv->DoPAINT( hWnd, hdc ); // 绘制指定的键盘
	else
	{  // 当前没有任何键盘
		GetClientRect( hWnd, &rect ); // 得到键盘窗口的大小
		FillRect( hdc, &rect, GetStockObject( LTGRAY_BRUSH ) ); // 填充灰色
	}
	EndPaint( hWnd, &ps );  // 结束绘制
	return 0;
}

// **************************************************
// 声明：static LRESULT DoLBUTTONDOWN( HWND hWnd, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lParam -- 消息参数
// 
// 返回值： 无
// 功能描述：处理左鼠标键按下。
// 引用: 
// **************************************************
static LRESULT DoLBUTTONDOWN( HWND hWnd, LPARAM lParam )
{
	LPKEYDRV lpdrv = (LPKEYDRV)GetWindowLong( hWnd, 0 ); // 得到键盘驱动

	SetCapture(hWnd); // 抓住鼠标
	lpdrv->DoLBUTTONDOWN( hWnd, (short)LOWORD( lParam ), (short)HIWORD( lParam ) ); // 指定驱动处理消息
	return 0;
}

// **************************************************
// 声明：static LRESULT DoLBUTTONUP( HWND hWnd, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lParam -- 消息参数
// 
// 返回值： 无
// 功能描述：处理左鼠标键弹起。
// 引用: 
// **************************************************
static LRESULT DoLBUTTONUP( HWND hWnd, LPARAM lParam )
{
	LPKEYDRV lpdrv = (LPKEYDRV)GetWindowLong( hWnd, 0 ); // 得到键盘驱动
//	WORD key;
	
	ReleaseCapture(); // 释放鼠标
	lpdrv->DoLBUTTONUP( hWnd, (short)LOWORD( lParam ), (short)HIWORD( lParam ) ); // 指定驱动处理
	return 0;

}

// **************************************************
// 声明：static LRESULT DoMOUSEMOVE( HWND hWnd, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lParam -- 消息参数
// 
// 返回值： 无
// 功能描述：处理鼠标移动。
// 引用: 
// **************************************************
static LRESULT DoMOUSEMOVE( HWND hWnd, LPARAM lParam )
{
	LPKEYDRV lpdrv = (LPKEYDRV)GetWindowLong( hWnd, 0 ); // 得到键盘驱动

	lpdrv->DoMOUSEMOVE( hWnd, (short)LOWORD( lParam ), (short)HIWORD( lParam ) ); // 键盘驱动处理鼠标移动
	return 0;
}

// **************************************************
// 声明：LRESULT CALLBACK KeyboardWndProc( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hwnd -- 窗口句柄
// 	IN uMsg -- 消息
// 	IN wParam -- 参数
// 	IN lParam -- 参数
// 
// 返回值：消息返回
// 功能描述：键盘窗口处理消息过程函数。
// 引用: 
// **************************************************
LRESULT CALLBACK KeyboardWndProc( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	int cx,cy;
	DWORD dwStyle;
	LPCREATESTRUCT lpcs;
//	DWORD dwCreate;
	LPSTRUCT_KEYBOARD  lpStruct_Keyboard;

	switch( uMsg )
	{
	case WM_CREATE: // 创建键盘窗口
//		if (g_bKBCreate == TRUE)  // 键盘已经创建，不需要再创建
//			return -1;
//		dwCreate = GetClassLong(hwnd,0);
//		if (dwCreate)
//			return -1;
//		SetClassLong(hwnd,0,TRUE);
///		g_bKBCreate = TRUE;  // 设置键盘已经创建消息
		lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)malloc(sizeof(STRUCT_KEYBOARD));
		if (lpStruct_Keyboard == NULL)
			return -1;  // 创建键盘失败

		// 初始化当前的键盘
		lpStruct_Keyboard->iOldKeyboard = KB_ENGLISH;//KB_HANDWRITE;
		lpStruct_Keyboard->iCurKeyboard = KB_ENGLISH;//KB_HANDWRITE;
		SetWindowLong(hwnd,4,(LONG)lpStruct_Keyboard);

		cx=KEYBOARDWIDTH;  // 设置键盘大小
		cy=KEYBOARDHEIGHT;
		lpcs = (LPCREATESTRUCT) lParam;  // 得到创建结构
		dwStyle=lpcs->style; // 得到风格
		if ((dwStyle&WS_CAPTION) == WS_CAPTION)
			cy+=GetSystemMetrics(SM_CYCAPTION);
//		SetWindowPos(hwnd,NULL,KEYBOARDSTARTX,KEYBOARDSTARTY,cx,cy,SWP_NOZORDER|SWP_NOREDRAW); // 设置键盘位置
		DoSETKEYDRV( hwnd, drvKeyboard[lpStruct_Keyboard->iCurKeyboard]); // 将当前键盘设置到窗口
		break;
	case WM_PAINT: // 绘制键盘
		DoPAINT( hwnd );  
		break;
	case WM_LBUTTONDOWN: // 处理鼠标左键按下
		return DoLBUTTONDOWN( hwnd, lParam );
	case WM_LBUTTONUP: // 处理鼠标左键弹起
		return DoLBUTTONUP( hwnd, lParam );
	case WM_MOUSEMOVE: // 处理鼠标移动
		if( wParam & MK_LBUTTON )
			return DoMOUSEMOVE( hwnd, lParam );
		return DefWindowProc( hwnd, uMsg, wParam, lParam );
	case KM_SETNEXTKEYBD:  // 设置到下一个键盘
		//iCurKeyboard++;
		//if (iCurKeyboard>=2)
		  //iCurKeyboard=0;
		lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)GetWindowLong(hwnd,4);
		ASSERT(lpStruct_Keyboard);

		lpStruct_Keyboard->iOldKeyboard = lpStruct_Keyboard->iCurKeyboard;
		if (lpStruct_Keyboard->iCurKeyboard == KB_SPELL)
		{
			lpStruct_Keyboard->iCurKeyboard = KB_ENGLISH;
		}
		else if (lpStruct_Keyboard->iCurKeyboard == KB_ENGLISH)
		{
//			lpStruct_Keyboard->iCurKeyboard = KB_SYMBOL;
			lpStruct_Keyboard->iCurKeyboard = KB_SPELL;
		}
		else if (lpStruct_Keyboard->iCurKeyboard == KB_SYMBOL)
		{
			lpStruct_Keyboard->iCurKeyboard = KB_HANDWRITE;
		}
		else
		{
			lpStruct_Keyboard->iCurKeyboard = KB_SPELL;
		}
		DoSETKEYDRV( hwnd, drvKeyboard[lpStruct_Keyboard->iCurKeyboard]);
		break;
	case KM_SETNAMEDKEYBD:  // 设置一个命名键盘
		DoSetNamedKeybd(hwnd,wParam,lParam);
		break;
	case KM_GOBACKBD:
		DoGoBackKB(hwnd,wParam,lParam);
		break;
  //case WM_ACTIVATE:
    //   return 0;
    case WM_WINDOWPOSCHANGING: // 处理窗口位置改变
#if 0
		((LPWINDOWPOS)lParam)->x = KEYBOARDSTARTX;
		if (((LPWINDOWPOS)lParam)->y < 0)
			((LPWINDOWPOS)lParam)->y = 0;
		if (((LPWINDOWPOS)lParam)->y > KEYBOARDSTARTY)
			((LPWINDOWPOS)lParam)->y = KEYBOARDSTARTY;
#endif		
		if (((LPWINDOWPOS)lParam)->flags & SWP_HIDEWINDOW)
		{
			// 如果需要隐藏窗口，则必须让键盘首先resume
			DoResumeKeyboard(hwnd);
		}
		if (((LPWINDOWPOS)lParam)->flags & SWP_SHOWWINDOW)
		{
			// 如果显示窗口，则必须让键盘设置到TOPMOST
//			((LPWINDOWPOS)lParam)->flags &= ~SWP_NOZORDER; 
//			((LPWINDOWPOS)lParam)->hwnd = HWND_TOP;  // 显示键盘
		}

		((LPWINDOWPOS)lParam)->flags |= SWP_NOACTIVATE;
		return DoDefWindowProc(hwnd, uMsg, wParam, lParam);
//  case WM_SETFOCUS:
//	   SetFocus((HWND)wParam);
  //     return 0;
	case WM_DESTROY:
		return DoDestroyWindow(hwnd,wParam,lParam);

	default:
//		return DefWindowProc( hwnd, uMsg, wParam, lParam );

		return DoDefWindowProc(hwnd, uMsg, wParam, lParam);
	}
  return 0;
}

/*
UINT MapVirtualKey(UINT uKeyCode, UINT fuMapType)
{
//   UINT uKeyCode;           
//   UINT fuMapType;          
     switch(fuMapType)
     {
         case 0: // Virtual Key into scan code
                 break;
         case 1: // Scan code into Virtual Key
                 break;
         case 2: // Virtual Key  into ASCII Code
                 return VirtualKeyToChar(uKeyCode);
         default:
                 return uKeyCode;
     }
     return 0;
}

UINT VirtualKeyToChar(UINT VirtualKey)
{
  int i;
   
   for (i=0;i<128;i++)
   {
	   if (VK_CharCodeTable[i].uVirtualKey==VirtualKey)
		   return VK_CharCodeTable[i].uASCIICode;
   }
   return -1;
}
*/

// **************************************************
// 声明：SHORT WINAPI Keybd_VkKeyScan(TCHAR uChar)
// 参数：
// 	IN uChar -- 指定字符
// 
// 返回值：返回字符所对应的虚键值
// 功能描述：得到字符所对应的虚键值。
// 引用: 
// **************************************************
SHORT WINAPI Keybd_VkKeyScan(TCHAR uChar)
{
//	if (uChar>127)
	if (uChar<0)  // char 型字符不可能大于127， Build Warning 
		return 0;  // 对应字符没有虚键
	return VK_CharCodeTable[uChar].uVirtualKey; // 返回对应的虚键
}

static int Startx=0,Starty=0;  // 键盘开始位置
static BOOL bMoveStart = FALSE; // 开始移动标志
static int xCurPos = KEYBOARDSTARTX;  // 键盘当前位置
static int yCurPos = KEYBOARDSTARTY;

// **************************************************
// 声明：void StartMoveKeyboard(HWND hWnd , int x,int y)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN x -- 开始X坐标
// 	IN y -- 开始Y坐标
// 
// 返回值：无
// 功能描述：开始移动键盘。
// 引用: 
// **************************************************
void StartMoveKeyboard(HWND hWnd , int x,int y)
{
	POINT ptCurPos;
/*	Startx=x;
	Starty=y;
	bMoveStart = TRUE;
	*/
	ptCurPos.x = x;// 得到键盘开始移动的位置
	ptCurPos.y = y;
	ClientToScreen(hWnd,&ptCurPos); // 得到对应点的屏幕坐标
	ReleaseCapture();  // 释放鼠标
	SendMessage(hWnd,WM_SYSCOMMAND,SC_MOVE,MAKELPARAM(ptCurPos.x,ptCurPos.y)); // 开始系统移动窗口处理
	SetCapture(hWnd); // 处理完成，抓住鼠标
}

// **************************************************
// 声明：void MoveKeyboard(HWND hWnd , int x,int y)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 
// 返回值：无
// 功能描述：移动键盘，暂时不处理，已经有系统处理。
// 引用: 
// **************************************************
void MoveKeyboard(HWND hWnd , int x,int y)
{
/*	if (bMoveStart == FALSE)
		return ;
	if (Starty == y)
		return ;
	yCurPos += y - Starty;
	Starty = y;
//	SetWindowPos(hWnd,NULL,xCurPos,yCurPos,0,0,SWP_NOSIZE|SWP_NOZORDER);
	ReleaseCapture();
	SendMessage(hWnd,WM_SYSCOMMAND,SC_MOVE,MAKELPARAM(x+xCurPos,yCurPos));
	SetCapture(hWnd);
*/
}
// **************************************************
// 声明：void EndMoveKeyboard(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：结束移动键盘。
// 引用: 
// **************************************************
void EndMoveKeyboard(HWND hWnd)
{
	Startx=0;
	Starty=0;
	bMoveStart = FALSE; // 清楚移动标志。
}

// **************************************************
// 声明：BOOL KeyboardMoving(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：返回当前是否移动的标志。
// 功能描述：得到当前键盘是否在移动状态。
// 引用: 
// **************************************************
BOOL KeyboardMoving(HWND hWnd)
{
	return bMoveStart; // 返回当前键盘的移动的标志。
}


// **************************************************
// 声明：static LRESULT DoSetNamedKeybd(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN wParam -- UINT 指定键盘的类型
//	IN lParam -- 保留
// 
// 返回值：无。
// 功能描述：设置一个指定的键盘。
// 引用: 
// **************************************************
static LRESULT DoSetNamedKeybd(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	KEYDRV *pNamedKeyboard = NULL;
	UINT uKeyBDType;
	UINT uSubKeyBDType;
	LPSTRUCT_KEYBOARD lpStruct_Keyboard;



		lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)GetWindowLong(hWnd,4);
		ASSERT(lpStruct_Keyboard);

		uKeyBDType = (UINT)	wParam;  // 得到要设置的键盘的类型
		uSubKeyBDType = (UINT)	lParam; // 只有当要设到符号键盘时才有子键盘功能

		switch (uKeyBDType)
		{
			case KB_SPELL:   // 拼音键盘
				pNamedKeyboard = &pyKeyboard;
				break;
			case KB_ENGLISH:	// 英文键盘
				pNamedKeyboard = &engKeyboard;
				break;
		}

		lpStruct_Keyboard->iOldKeyboard = lpStruct_Keyboard->iCurKeyboard;  // 保留原来的键盘
		lpStruct_Keyboard->iCurKeyboard = uKeyBDType;

		if (pNamedKeyboard) // 已经得到新的键盘
		{
			DoSETKEYDRV( hWnd, pNamedKeyboard);  // 设置新的键盘为当前键盘
		}

		return 0;
}

// **************************************************
// 声明：static LRESULT DoGoBackKB(HWND hWnd, WPARAM wParam, LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN wParam -- 保留
//	IN lParam -- 保留
// 
// 返回值：无。
// 功能描述：设置一个指定的键盘。
// 引用: 
// **************************************************
static LRESULT DoGoBackKB(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KEYDRV *pNamedKeyboard = NULL;
	UINT uKeyBDType;
	LPSTRUCT_KEYBOARD lpStruct_Keyboard;



		lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)GetWindowLong(hWnd,4);
		ASSERT(lpStruct_Keyboard);

		uKeyBDType = lpStruct_Keyboard->iOldKeyboard; // 将要设置的键盘
		lpStruct_Keyboard->iOldKeyboard = lpStruct_Keyboard->iCurKeyboard;  // 保留原来的键盘
		lpStruct_Keyboard->iCurKeyboard = uKeyBDType;

		switch (uKeyBDType)
		{
			case KB_SPELL:   // 拼音键盘
				pNamedKeyboard = &pyKeyboard;
				break;
			case KB_ENGLISH:	// 英文键盘
				pNamedKeyboard = &engKeyboard;
				break;
		}

		if (pNamedKeyboard) // 已经得到新的键盘
		{
			DoSETKEYDRV( hWnd, pNamedKeyboard);  // 设置新的键盘为当前键盘
		}

		return 0;
}


// **************************************************
// 声明：static LRESULT DoDefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hwnd -- 窗口句柄
// 	IN uMsg -- 消息
// 	IN wParam -- 参数
// 	IN lParam -- 参数
// 
// 返回值：无
// 功能描述：处理默认消息。
// 引用: 
// **************************************************
static LRESULT DoDefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LPCKEYDRV lpdrv;

	lpdrv = (LPCKEYDRV)GetWindowLong( hWnd, 0); // 得到旧的键盘驱动类
	if (lpdrv)
	{
		return lpdrv->DoDefWindowProc( hWnd, uMsg, wParam, lParam ); // 调用键盘默认消息处理
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


// **************************************************
// 声明：static LRESULT DoResumeKeyboard( HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpdrv -- 要设置为当前键盘的键盘驱动。
// 
// 返回值：无
// 功能描述：设置键盘驱动。
// 引用: 
// **************************************************
static LRESULT DoResumeKeyboard( HWND hWnd)
{
	LPCKEYDRV lpdrv;
	lpdrv = (LPCKEYDRV)GetWindowLong( hWnd, 0); // 得到旧的键盘驱动类
	if (lpdrv != 0)
	{ // 原来存在键盘
		lpdrv->Resume(hWnd); // 破坏原来的键盘
	}
	return 0;
}


// **************************************************
// 声明：static LRESULT DoDestroyWindow(HWND hWnd,WPARAM wParam , LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：破坏键盘。
// 引用: 
// **************************************************
static LRESULT DoDestroyWindow(HWND hWnd,WPARAM wParam , LPARAM lParam)
{
	LPCKEYDRV lpOlddrv;
	LPSTRUCT_KEYBOARD  lpStruct_Keyboard;

	lpOlddrv = (LPCKEYDRV)GetWindowLong( hWnd, 0); // 得到旧的键盘驱动类
	if (lpOlddrv != 0)
	{ // 原来存在键盘
		lpOlddrv->Destroy(hWnd); // 破坏原来的键盘
	}

	SetWindowLong( hWnd, 0, (LONG)0); // 将键盘驱动设置到当前键盘类

	lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)GetWindowLong(hWnd,4);
	ASSERT(lpStruct_Keyboard);
	free(lpStruct_Keyboard);

	SetWindowLong( hWnd, 4, (LONG)0); // 将键盘驱动设置到当前键盘类

	return 0;
}


int GetSystemStyle()
{
	return 0;
}
