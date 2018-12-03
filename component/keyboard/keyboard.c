/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵�������̴�����
�汾�ţ�1.0.0.456
����ʱ�ڣ�2002-03-15
���ߣ��½��� Jami chen 
�޸ļ�¼��
**************************************************/
#include <eframe.h>
//#include <eprivate.h>
#include <softkey.h>
#include <eptrlist.h>
#include <elstctrl.h>
#include <thirdpart.h>

// *************************************************
// ��������
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
// ������������
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
// ������BOOL RegisterKeyboardClass( HINSTANCE hInst )
// ������
// 	IN hInst -- ʵ�����
// 
// ����ֵ������ע����
// ����������ע������ࡣ
// ����: 
// **************************************************
BOOL RegisterKeyboardClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = KeyboardWndProc;  // ��������̺���
    wc.cbClsExtra = sizeof(DWORD);
    wc.cbWndExtra = sizeof( LPKEYDRV ) + 8;
    wc.hInstance = hInst;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = classKEYBOARD; // ����

	return (BOOL)(RegisterClass( &wc )); // ��ϵͳע�������
}

// **************************************************
// ������static LRESULT DoSETKEYDRV( HWND hWnd, LPCKEYDRV lpdrv )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpdrv -- Ҫ����Ϊ��ǰ���̵ļ���������
// 
// ����ֵ����
// �������������ü���������
// ����: 
// **************************************************
static LRESULT DoSETKEYDRV( HWND hWnd, LPCKEYDRV lpdrv )
{
	LPCKEYDRV lpOlddrv;
	if (lpdrv == 0)
		return 0;
	lpOlddrv = (LPCKEYDRV)GetWindowLong( hWnd, 0); // �õ��ɵļ���������
	if (lpOlddrv != 0)
	{ // ԭ�����ڼ���
		lpOlddrv->Destroy(hWnd); // �ƻ�ԭ���ļ���
	}
	SetWindowLong( hWnd, 0, (LONG)lpdrv ); // �������������õ���ǰ������
	lpdrv->Create(hWnd); // �����ü���
	InvalidateRect( hWnd, 0, TRUE );  // �ػ����
	return 0;
}

// **************************************************
// ������static LRESULT DoPAINT( HWND hWnd )
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �������������Ƽ��̡�
// ����: 
// **************************************************
static LRESULT DoPAINT( HWND hWnd )
{
	PAINTSTRUCT ps;
	LPKEYDRV lpdrv = (LPKEYDRV)GetWindowLong( hWnd, 0 ); // �õ���������
	HDC hdc = BeginPaint( hWnd, &ps ); // ��ʼ����
	RECT rect;
	if( lpdrv )
	    lpdrv->DoPAINT( hWnd, hdc ); // ����ָ���ļ���
	else
	{  // ��ǰû���κμ���
		GetClientRect( hWnd, &rect ); // �õ����̴��ڵĴ�С
		FillRect( hdc, &rect, GetStockObject( LTGRAY_BRUSH ) ); // ����ɫ
	}
	EndPaint( hWnd, &ps );  // ��������
	return 0;
}

// **************************************************
// ������static LRESULT DoLBUTTONDOWN( HWND hWnd, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ�� ��
// �����������������������¡�
// ����: 
// **************************************************
static LRESULT DoLBUTTONDOWN( HWND hWnd, LPARAM lParam )
{
	LPKEYDRV lpdrv = (LPKEYDRV)GetWindowLong( hWnd, 0 ); // �õ���������

	SetCapture(hWnd); // ץס���
	lpdrv->DoLBUTTONDOWN( hWnd, (short)LOWORD( lParam ), (short)HIWORD( lParam ) ); // ָ������������Ϣ
	return 0;
}

// **************************************************
// ������static LRESULT DoLBUTTONUP( HWND hWnd, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ�� ��
// ������������������������
// ����: 
// **************************************************
static LRESULT DoLBUTTONUP( HWND hWnd, LPARAM lParam )
{
	LPKEYDRV lpdrv = (LPKEYDRV)GetWindowLong( hWnd, 0 ); // �õ���������
//	WORD key;
	
	ReleaseCapture(); // �ͷ����
	lpdrv->DoLBUTTONUP( hWnd, (short)LOWORD( lParam ), (short)HIWORD( lParam ) ); // ָ����������
	return 0;

}

// **************************************************
// ������static LRESULT DoMOUSEMOVE( HWND hWnd, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ�� ��
// ������������������ƶ���
// ����: 
// **************************************************
static LRESULT DoMOUSEMOVE( HWND hWnd, LPARAM lParam )
{
	LPKEYDRV lpdrv = (LPKEYDRV)GetWindowLong( hWnd, 0 ); // �õ���������

	lpdrv->DoMOUSEMOVE( hWnd, (short)LOWORD( lParam ), (short)HIWORD( lParam ) ); // ����������������ƶ�
	return 0;
}

// **************************************************
// ������LRESULT CALLBACK KeyboardWndProc( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hwnd -- ���ھ��
// 	IN uMsg -- ��Ϣ
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����Ϣ����
// �������������̴��ڴ�����Ϣ���̺�����
// ����: 
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
	case WM_CREATE: // �������̴���
//		if (g_bKBCreate == TRUE)  // �����Ѿ�����������Ҫ�ٴ���
//			return -1;
//		dwCreate = GetClassLong(hwnd,0);
//		if (dwCreate)
//			return -1;
//		SetClassLong(hwnd,0,TRUE);
///		g_bKBCreate = TRUE;  // ���ü����Ѿ�������Ϣ
		lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)malloc(sizeof(STRUCT_KEYBOARD));
		if (lpStruct_Keyboard == NULL)
			return -1;  // ��������ʧ��

		// ��ʼ����ǰ�ļ���
		lpStruct_Keyboard->iOldKeyboard = KB_ENGLISH;//KB_HANDWRITE;
		lpStruct_Keyboard->iCurKeyboard = KB_ENGLISH;//KB_HANDWRITE;
		SetWindowLong(hwnd,4,(LONG)lpStruct_Keyboard);

		cx=KEYBOARDWIDTH;  // ���ü��̴�С
		cy=KEYBOARDHEIGHT;
		lpcs = (LPCREATESTRUCT) lParam;  // �õ������ṹ
		dwStyle=lpcs->style; // �õ����
		if ((dwStyle&WS_CAPTION) == WS_CAPTION)
			cy+=GetSystemMetrics(SM_CYCAPTION);
//		SetWindowPos(hwnd,NULL,KEYBOARDSTARTX,KEYBOARDSTARTY,cx,cy,SWP_NOZORDER|SWP_NOREDRAW); // ���ü���λ��
		DoSETKEYDRV( hwnd, drvKeyboard[lpStruct_Keyboard->iCurKeyboard]); // ����ǰ�������õ�����
		break;
	case WM_PAINT: // ���Ƽ���
		DoPAINT( hwnd );  
		break;
	case WM_LBUTTONDOWN: // ��������������
		return DoLBUTTONDOWN( hwnd, lParam );
	case WM_LBUTTONUP: // ��������������
		return DoLBUTTONUP( hwnd, lParam );
	case WM_MOUSEMOVE: // ��������ƶ�
		if( wParam & MK_LBUTTON )
			return DoMOUSEMOVE( hwnd, lParam );
		return DefWindowProc( hwnd, uMsg, wParam, lParam );
	case KM_SETNEXTKEYBD:  // ���õ���һ������
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
	case KM_SETNAMEDKEYBD:  // ����һ����������
		DoSetNamedKeybd(hwnd,wParam,lParam);
		break;
	case KM_GOBACKBD:
		DoGoBackKB(hwnd,wParam,lParam);
		break;
  //case WM_ACTIVATE:
    //   return 0;
    case WM_WINDOWPOSCHANGING: // ������λ�øı�
#if 0
		((LPWINDOWPOS)lParam)->x = KEYBOARDSTARTX;
		if (((LPWINDOWPOS)lParam)->y < 0)
			((LPWINDOWPOS)lParam)->y = 0;
		if (((LPWINDOWPOS)lParam)->y > KEYBOARDSTARTY)
			((LPWINDOWPOS)lParam)->y = KEYBOARDSTARTY;
#endif		
		if (((LPWINDOWPOS)lParam)->flags & SWP_HIDEWINDOW)
		{
			// �����Ҫ���ش��ڣ�������ü�������resume
			DoResumeKeyboard(hwnd);
		}
		if (((LPWINDOWPOS)lParam)->flags & SWP_SHOWWINDOW)
		{
			// �����ʾ���ڣ�������ü������õ�TOPMOST
//			((LPWINDOWPOS)lParam)->flags &= ~SWP_NOZORDER; 
//			((LPWINDOWPOS)lParam)->hwnd = HWND_TOP;  // ��ʾ����
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
// ������SHORT WINAPI Keybd_VkKeyScan(TCHAR uChar)
// ������
// 	IN uChar -- ָ���ַ�
// 
// ����ֵ�������ַ�����Ӧ�����ֵ
// �����������õ��ַ�����Ӧ�����ֵ��
// ����: 
// **************************************************
SHORT WINAPI Keybd_VkKeyScan(TCHAR uChar)
{
//	if (uChar>127)
	if (uChar<0)  // char ���ַ������ܴ���127�� Build Warning 
		return 0;  // ��Ӧ�ַ�û�����
	return VK_CharCodeTable[uChar].uVirtualKey; // ���ض�Ӧ�����
}

static int Startx=0,Starty=0;  // ���̿�ʼλ��
static BOOL bMoveStart = FALSE; // ��ʼ�ƶ���־
static int xCurPos = KEYBOARDSTARTX;  // ���̵�ǰλ��
static int yCurPos = KEYBOARDSTARTY;

// **************************************************
// ������void StartMoveKeyboard(HWND hWnd , int x,int y)
// ������
// 	IN hWnd -- ���ھ��
// 	IN x -- ��ʼX����
// 	IN y -- ��ʼY����
// 
// ����ֵ����
// ������������ʼ�ƶ����̡�
// ����: 
// **************************************************
void StartMoveKeyboard(HWND hWnd , int x,int y)
{
	POINT ptCurPos;
/*	Startx=x;
	Starty=y;
	bMoveStart = TRUE;
	*/
	ptCurPos.x = x;// �õ����̿�ʼ�ƶ���λ��
	ptCurPos.y = y;
	ClientToScreen(hWnd,&ptCurPos); // �õ���Ӧ�����Ļ����
	ReleaseCapture();  // �ͷ����
	SendMessage(hWnd,WM_SYSCOMMAND,SC_MOVE,MAKELPARAM(ptCurPos.x,ptCurPos.y)); // ��ʼϵͳ�ƶ����ڴ���
	SetCapture(hWnd); // ������ɣ�ץס���
}

// **************************************************
// ������void MoveKeyboard(HWND hWnd , int x,int y)
// ������
// 	IN hWnd -- ���ھ��
// 	IN x -- X����
// 	IN y -- Y����
// 
// ����ֵ����
// �����������ƶ����̣���ʱ�������Ѿ���ϵͳ����
// ����: 
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
// ������void EndMoveKeyboard(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ���������������ƶ����̡�
// ����: 
// **************************************************
void EndMoveKeyboard(HWND hWnd)
{
	Startx=0;
	Starty=0;
	bMoveStart = FALSE; // ����ƶ���־��
}

// **************************************************
// ������BOOL KeyboardMoving(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ�����ص�ǰ�Ƿ��ƶ��ı�־��
// �����������õ���ǰ�����Ƿ����ƶ�״̬��
// ����: 
// **************************************************
BOOL KeyboardMoving(HWND hWnd)
{
	return bMoveStart; // ���ص�ǰ���̵��ƶ��ı�־��
}


// **************************************************
// ������static LRESULT DoSetNamedKeybd(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
//  IN wParam -- UINT ָ�����̵�����
//	IN lParam -- ����
// 
// ����ֵ���ޡ�
// ��������������һ��ָ���ļ��̡�
// ����: 
// **************************************************
static LRESULT DoSetNamedKeybd(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	KEYDRV *pNamedKeyboard = NULL;
	UINT uKeyBDType;
	UINT uSubKeyBDType;
	LPSTRUCT_KEYBOARD lpStruct_Keyboard;



		lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)GetWindowLong(hWnd,4);
		ASSERT(lpStruct_Keyboard);

		uKeyBDType = (UINT)	wParam;  // �õ�Ҫ���õļ��̵�����
		uSubKeyBDType = (UINT)	lParam; // ֻ�е�Ҫ�赽���ż���ʱ�����Ӽ��̹���

		switch (uKeyBDType)
		{
			case KB_SPELL:   // ƴ������
				pNamedKeyboard = &pyKeyboard;
				break;
			case KB_ENGLISH:	// Ӣ�ļ���
				pNamedKeyboard = &engKeyboard;
				break;
		}

		lpStruct_Keyboard->iOldKeyboard = lpStruct_Keyboard->iCurKeyboard;  // ����ԭ���ļ���
		lpStruct_Keyboard->iCurKeyboard = uKeyBDType;

		if (pNamedKeyboard) // �Ѿ��õ��µļ���
		{
			DoSETKEYDRV( hWnd, pNamedKeyboard);  // �����µļ���Ϊ��ǰ����
		}

		return 0;
}

// **************************************************
// ������static LRESULT DoGoBackKB(HWND hWnd, WPARAM wParam, LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
//  IN wParam -- ����
//	IN lParam -- ����
// 
// ����ֵ���ޡ�
// ��������������һ��ָ���ļ��̡�
// ����: 
// **************************************************
static LRESULT DoGoBackKB(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KEYDRV *pNamedKeyboard = NULL;
	UINT uKeyBDType;
	LPSTRUCT_KEYBOARD lpStruct_Keyboard;



		lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)GetWindowLong(hWnd,4);
		ASSERT(lpStruct_Keyboard);

		uKeyBDType = lpStruct_Keyboard->iOldKeyboard; // ��Ҫ���õļ���
		lpStruct_Keyboard->iOldKeyboard = lpStruct_Keyboard->iCurKeyboard;  // ����ԭ���ļ���
		lpStruct_Keyboard->iCurKeyboard = uKeyBDType;

		switch (uKeyBDType)
		{
			case KB_SPELL:   // ƴ������
				pNamedKeyboard = &pyKeyboard;
				break;
			case KB_ENGLISH:	// Ӣ�ļ���
				pNamedKeyboard = &engKeyboard;
				break;
		}

		if (pNamedKeyboard) // �Ѿ��õ��µļ���
		{
			DoSETKEYDRV( hWnd, pNamedKeyboard);  // �����µļ���Ϊ��ǰ����
		}

		return 0;
}


// **************************************************
// ������static LRESULT DoDefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hwnd -- ���ھ��
// 	IN uMsg -- ��Ϣ
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ��������������Ĭ����Ϣ��
// ����: 
// **************************************************
static LRESULT DoDefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LPCKEYDRV lpdrv;

	lpdrv = (LPCKEYDRV)GetWindowLong( hWnd, 0); // �õ��ɵļ���������
	if (lpdrv)
	{
		return lpdrv->DoDefWindowProc( hWnd, uMsg, wParam, lParam ); // ���ü���Ĭ����Ϣ����
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


// **************************************************
// ������static LRESULT DoResumeKeyboard( HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpdrv -- Ҫ����Ϊ��ǰ���̵ļ���������
// 
// ����ֵ����
// �������������ü���������
// ����: 
// **************************************************
static LRESULT DoResumeKeyboard( HWND hWnd)
{
	LPCKEYDRV lpdrv;
	lpdrv = (LPCKEYDRV)GetWindowLong( hWnd, 0); // �õ��ɵļ���������
	if (lpdrv != 0)
	{ // ԭ�����ڼ���
		lpdrv->Resume(hWnd); // �ƻ�ԭ���ļ���
	}
	return 0;
}


// **************************************************
// ������static LRESULT DoDestroyWindow(HWND hWnd,WPARAM wParam , LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �����������ƻ����̡�
// ����: 
// **************************************************
static LRESULT DoDestroyWindow(HWND hWnd,WPARAM wParam , LPARAM lParam)
{
	LPCKEYDRV lpOlddrv;
	LPSTRUCT_KEYBOARD  lpStruct_Keyboard;

	lpOlddrv = (LPCKEYDRV)GetWindowLong( hWnd, 0); // �õ��ɵļ���������
	if (lpOlddrv != 0)
	{ // ԭ�����ڼ���
		lpOlddrv->Destroy(hWnd); // �ƻ�ԭ���ļ���
	}

	SetWindowLong( hWnd, 0, (LONG)0); // �������������õ���ǰ������

	lpStruct_Keyboard = (LPSTRUCT_KEYBOARD)GetWindowLong(hWnd,4);
	ASSERT(lpStruct_Keyboard);
	free(lpStruct_Keyboard);

	SetWindowLong( hWnd, 4, (LONG)0); // �������������õ���ǰ������

	return 0;
}


int GetSystemStyle()
{
	return 0;
}
