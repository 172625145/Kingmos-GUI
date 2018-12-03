/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/
/*
说明：消息分为四个部分：
1.0 ~ WM_SYS: WIN32 API兼容部分（不可重叠区）
2.WM_SYS ~ WM_OS: Kingmos自定义系统消息（不可重叠区）
3.WM_OS ~ WM_USER: 为各种控件提供的消息（可重叠区）
4.WM_USER ~ 0xffff: 用户区
*/

#ifndef __EMESSAGE_H
#define __EMESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

typedef struct _MSG
{
    HWND        hwnd;
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    DWORD       time;
    POINT       pt;
}MSG, FAR * LPMSG;

// define Keyboard virtual key value

#define VK_UNUSED           0x00

//#define VK_CPY              0x01
//#define VK_PST              0x02
//#define VK_TRAN             0X03
//#define VK_KB               0X04
//#define VK_VISIBLE          0X05

#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#define VK_POWEROFF         0X06

// Turn between SBC Case and DBC Case
#define VK_CASE             0X07  

#define VK_BACK             0x08
#define VK_TAB				0x09
#define VK_CLEAR            0x0C
#define VK_RETURN           0x0D

#define VK_SHIFT            0x10
#define VK_CONTROL          0x11
#define VK_PAUSE            0x13

#define VK_CAPITAL          0x14

#define VK_ESCAPE           0x1B

#define VK_ACCEPT           0x1E

#define VK_PRINT            0x2A
#define VK_EXECUTE          0x2B
#define VK_SNAPSHOT         0x2C

#define VK_SPACE            0x20
#define VK_PRIOR            0x21
#define VK_NEXT             0x22
#define VK_END              0x23
#define VK_HOME             0x24
#define VK_LEFT             0x25
#define VK_UP               0x26
#define VK_RIGHT            0x27
#define VK_DOWN             0x28

#define VK_INSERT           0x2D
#define VK_DELETE           0x2E
#define VK_HELP             0x2F

#define VK_0                '0'
#define VK_1                '1'
#define VK_2                '2'
#define VK_3                '3'
#define VK_4                '4'
#define VK_5                '5'
#define VK_6                '6'
#define VK_7                '7'
#define VK_8                '8'
#define VK_9                '9'
#define VK_A                'A'
#define VK_B                'B'
#define VK_C                'C'
#define VK_D                'D'
#define VK_E                'E'
#define VK_F                'F'
#define VK_G                'G'
#define VK_H                'H'
#define VK_I                'I'
#define VK_J                'J'
#define VK_K                'K'
#define VK_L                'L'
#define VK_M                'M'
#define VK_N                'N'
#define VK_O                'O'
#define VK_P                'P'
#define VK_Q                'Q'
#define VK_R                'R'
#define VK_S                'S'
#define VK_T                'T'
#define VK_U                'U'
#define VK_V                'V'
#define VK_W                'W'
#define VK_X                'X'
#define VK_Y                'Y'
#define VK_Z                'Z'

#define VK_LWIN				0x5B
#define VK_RWIN				0x5C
#define VK_APPS				0x5D

#define VK_SLEEP			0x5F

#define VK_NUMPAD0			0x60
#define VK_NUMPAD1			0x61
#define VK_NUMPAD2			0x62
#define VK_NUMPAD3			0x63
#define VK_NUMPAD4			0x64
#define VK_NUMPAD5			0x65
#define VK_NUMPAD6			0x66
#define VK_NUMPAD7			0x67
#define VK_NUMPAD8			0x68
#define VK_NUMPAD9			0x69
#define VK_MULTIPLY			0x6A
#define VK_ADD				0x6B
#define VK_SEPARATOR		0x6C
#define VK_SUBTRACT			0x6D
#define VK_DECIMAL			0x6E
#define VK_DIVIDE			0x6F
#define VK_F1				0x70
#define VK_F2				0x71
#define VK_F3				0x72
#define VK_F4				0x73
#define VK_F5				0x74
#define VK_F6				0x75
#define VK_F7				0x76
#define VK_F8				0x77
#define VK_F9				0x78
#define VK_F10				0x79
#define VK_F11				0x7A
#define VK_F12				0x7B
#define VK_F13				0x7C
#define VK_F14				0x7D
#define VK_F15				0x7E
#define VK_F16				0x7F
#define VK_F17				0x80
#define VK_F18				0x81
#define VK_F19				0x82
#define VK_F20				0x83
#define VK_F21				0x84
#define VK_F22				0x85
#define VK_F23				0x86
#define VK_F24				0x87

#define VK_NUMLOCK			0x90
#define VK_SCROLL			0x91

#define VK_SEMICOLON		0xBA
#define VK_EQUAL			0xBB
#define VK_COMMA			0xBC
#define VK_HYPHEN			0xBD
#define VK_PERIOD			0xBE
#define VK_SLASH			0xBF
#define VK_BACKQUOTE		0xC0

#define VK_LBRACKET			0xDB
#define VK_BACKSLASH		0xDC
#define VK_RBRACKET			0xDD
#define VK_APOSTROPHE		0xDE


#define VK_LSHIFT			0xA0
#define VK_RSHIFT			0xA1
#define VK_LCONTROL			0xA2
#define VK_RCONTROL			0xA3
#define VK_LMENU			0xA4
#define VK_RMENU			0xA5

#define	VK_EXTEND_BSLASH	0xE2
#define	VK_OEM_102			0xE2

#define VK_PROCESSKEY		0xE5

#define VK_TURN				0xF2  // 键盘切换
#define VK_MOVE				0xF3  // 移动键盘

#define VK_SEND             0xF4
#define VK_OVER             0xF5

#define VK_ATTN				0xF6
#define VK_CRSEL			0xF7
#define VK_EXSEL			0xF8
#define VK_EREOF			0xF9
#define VK_PLAY				0xFA
#define VK_ZOOM				0xFB
#define VK_NONAME			0xFC
#define VK_PA1				0xFD
#define VK_OEM_CLEAR		0xFE



#define VK_BROWSER_BACK                  0xA6
#define VK_BROWSER_FORWARD               0xA7
#define VK_BROWSER_REFRESH               0xA8
#define VK_BROWSER_STOP                  0xA9
#define VK_BROWSER_SEARCH                0xAA
#define VK_BROWSER_FAVORITES             0xAB
#define VK_BROWSER_HOME                  0xAC
#define VK_VOLUME_MUTE                   0xAD
#define VK_VOLUME_DOWN                   0xAE
#define VK_VOLUME_UP                     0xAF
#define VK_MEDIA_NEXT_TRACK              0xB0
#define VK_MEDIA_PREV_TRACK              0xB1
#define VK_MEDIA_STOP                    0xB2
#define VK_MEDIA_PLAY_PAUSE              0xB3
#define VK_LAUNCH_MAIL                   0xB4
#define VK_LAUNCH_MEDIA_SELECT           0xB5
#define VK_LAUNCH_APP1                   0xB6
#define VK_LAUNCH_APP2                   0xB7

#define VK_OFF              0xDF

#define VK_DBE_ALPHANUMERIC              0x0f0
#define VK_DBE_KATAKANA                  0x0f1
#define VK_DBE_HIRAGANA                  0x0f2
#define VK_DBE_SBCSCHAR                  0x0f3
#define VK_DBE_DBCSCHAR                  0x0f4
#define VK_DBE_ROMAN                     0x0f5
#define VK_DBE_NOROMAN                   0x0f6
#define VK_DBE_ENTERWORDREGISTERMODE     0x0f7
#define VK_DBE_ENTERIMECONFIGMODE        0x0f8
#define VK_DBE_FLUSHSTRING               0x0f9
#define VK_DBE_CODEINPUT                 0x0fa
#define VK_DBE_NOCODEINPUT               0x0fb
#define VK_DBE_DETERMINESTRING           0x0fc
#define VK_DBE_ENTERDLGCONVERSIONMODE    0x0fd

//  Window Messages
#define WM_NULL                         0x0000
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002
#define WM_MOVE                         0x0003
#define WM_SIZE                         0x0005

#define WM_ACTIVATE                     0x0006
// WM_ACTIVATE state values
#define     WA_INACTIVE     0
#define     WA_ACTIVE       1
#define     WA_CLICKACTIVE  2

#define WM_SETFOCUS                     0x0007
#define WM_KILLFOCUS                    0x0008
#define WM_ENABLE                       0x000A
#define WM_SETREDRAW                    0x000B
#define WM_SETTEXT                      0x000C
#define WM_GETTEXT                      0x000D
#define WM_GETTEXTLENGTH                0x000E
#define WM_PAINT                        0x000F
#define WM_CLOSE                        0x0010
#define WM_QUERYENDSESSION              0x0011
#define WM_QUIT                         0x0012
#define WM_QUERYOPEN                    0x0013
#define WM_ERASEBKGND                   0x0014
#define WM_SYSCOLORCHANGE               0x0015
#define WM_ENDSESSION                   0x0016
#define WM_SHOWWINDOW                   0x0018
#define WM_OK                           0x0019

#define WM_ACTIVATEAPP                  0x001C

#define WM_SETCURSOR                    0x0020

#define WM_MOUSEACTIVATE                0x0021
// WM_MOUSEACTIVATE return value
#define MA_ACTIVATE         1
#define MA_ACTIVATEANDEAT   2
#define MA_NOACTIVATE       3
#define MA_NOACTIVATEANDEAT 4

#define WM_DRAWITEM                     0x002B
#define WM_MEASUREITEM                  0x002C
#define WM_DELETEITEM                   0x002D
#define WM_VKEYTOITEM                   0x002E
#define WM_CHARTOITEM                   0x002F

#define WM_SETFONT                      0x0030

#define WM_WINDOWPOSCHANGING            0x0046
#define WM_WINDOWPOSCHANGED             0x0047

#define WM_NOTIFY                       0x004E
#define WM_HELP                         0x0053

#define WM_STYLECHANGING                0x007C
#define WM_STYLECHANGED                 0x007D

// WM_GETICON / WM_SETICON wParam data
#define ICON_SMALL          0
#define ICON_BIG            1

#define WM_GETICON                      0x007F
#define WM_SETICON                      0x0080

#define WM_NCCREATE                     0x0081
#define WM_NCDESTROY                    0x0082
#define WM_NCCALCSIZE                   0x0083


#define WM_NCHITTEST                    0x0084
// WM_NCHITTEST code
#define HTERROR             (-2)
#define HTTRANSPARENT       (-1)
#define HTNOWHERE           0
#define HTCLIENT            1
#define HTCAPTION           2
#define HTSYSMENU           3
#define HTGROWBOX           4
#define HTSIZE              HTGROWBOX
#define HTMENU              5
#define HTHSCROLL           6
#define HTVSCROLL           7
#define HTMINBUTTON         8
#define HTMAXBUTTON         9
#define HTLEFT              10
#define HTRIGHT             11
#define HTTOP               12
#define HTTOPLEFT           13
#define HTTOPRIGHT          14
#define HTBOTTOM            15
#define HTBOTTOMLEFT        16
#define HTBOTTOMRIGHT       17
#define HTBORDER            18
#define HTREDUCE            HTMINBUTTON
#define HTZOOM              HTMAXBUTTON
#define HTSIZEFIRST         HTLEFT
#define HTSIZELAST          HTBOTTOMRIGHT

#define HTCLOSE             20
#define HTHELP              21

// kingmos define
#define HTCLOSEBOX          100
#define HTOKBOX             101
#define HTHELPBOX           102
#define HTCLIENTEDGE        103

//#define HTOPTION            100

#define WM_NCPAINT                      0x0085
#define WM_NCACTIVATE                   0x0086
#define WM_GETDLGCODE                   0x0087

#define WM_NCMOUSEMOVE                  0x00A0
#define WM_NCLBUTTONDOWN                0x00A1
#define WM_NCLBUTTONUP                  0x00A2
#define WM_NCLBUTTONDBLCLK              0x00A3
#define WM_NCRBUTTONDOWN                0x00A4
#define WM_NCRBUTTONUP                  0x00A5
#define WM_NCRBUTTONDBLCLK              0x00A6
#define WM_NCMBUTTONDOWN                0x00A7
#define WM_NCMBUTTONUP                  0x00A8
#define WM_NCMBUTTONDBLCLK              0x00A9

// Scrollbar message
#define SBM_SETPOS                      0x00E0
#define SBM_GETPOS                      0x00E1
#define SBM_SETRANGE                    0x00E2
#define SBM_SETRANGEREDRAW              0x00E6
#define SBM_GETRANGE                    0x00E3
#define SBM_ENABLE_ARROWS               0x00E4
#define SBM_SETSCROLLINFO               0x00E9
#define SBM_GETSCROLLINFO               0x00EA

#define WM_KEYFIRST                     0x0100
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_CHAR                         0x0102
#define WM_DEADCHAR                     0x0103
#define WM_SYSKEYDOWN                   0x0104
#define WM_SYSKEYUP                     0x0105
#define WM_SYSCHAR                      0x0106
#define WM_SYSDEADCHAR                  0x0107
#define WM_KEYLAST                      0x0108

#define WM_INITDIALOG                   0x0110
#define WM_COMMAND                      0x0111


#define SC_OK           0xF160
#define SC_CONTEXTHELP  0xF170

#define WM_SYSCOMMAND                   0x0112
//WM_SYSCOMMAND
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_MINIMIZE     0xF020
#define SC_MAXIMIZE     0xF030
#define SC_NEXTWINDOW   0xF040
#define SC_PREVWINDOW   0xF050
#define SC_CLOSE        0xF060
#define SC_VSCROLL      0xF070
#define SC_HSCROLL      0xF080
#define SC_MOUSEMENU    0xF090
#define SC_KEYMENU      0xF100
#define SC_ARRANGE      0xF110
#define SC_RESTORE      0xF120
#define SC_TASKLIST     0xF130
#define SC_SCREENSAVE   0xF140
#define SC_HOTKEY       0xF150


#define WM_TIMER                        0x0113

#define WM_HSCROLL                      0x0114
#define WM_VSCROLL                      0x0115
// Scroll Bar Commands
#define SB_LINEUP           0
#define SB_LINELEFT         0
#define SB_LINEDOWN         1
#define SB_LINERIGHT        1
#define SB_PAGEUP           2
#define SB_PAGELEFT         2
#define SB_PAGEDOWN         3
#define SB_PAGERIGHT        3
#define SB_THUMBPOSITION    4
#define SB_THUMBTRACK       5
#define SB_TOP              6
#define SB_LEFT             6
#define SB_BOTTOM           7
#define SB_RIGHT            7
#define SB_ENDSCROLL        8


#define WM_SYSTIMER                     0x0118
#define IDCARET                         0x0001
#define IDSCROLLBAR                     0x0002


// WM_ENTERIDLE 's WPARAM
#define MSGF_DIALOGBOX      0
#define MSGF_MESSAGEBOX     1
#define WM_ENTERIDLE                    0x0121
#define WM_KICKIDLE                     0x0122

// static control message and notify
#define STM_SETICON                     0x0170
#define STM_GETICON                     0x0171
#define STM_SETIMAGE                    0x0172
#define STM_GETIMAGE                    0x0173

#define STN_CLICKED         0
#define STN_DBLCLK          1
#define STN_ENABLE          2
#define STN_DISABLE         3





// Key State Masks for Mouse Messages

#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010

#define WM_MOUSEFIRST                   0x0200
#define WM_MOUSEMOVE                    0x0200
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_LBUTTONDBLCLK                0x0203
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_RBUTTONDBLCLK                0x0206
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
#define WM_MBUTTONDBLCLK                0x0209
#define WM_MOUSELAST                    0x0209

#define WM_PARENTNOTIFY                 0x0210

#define WM_CAPTURECHANGED   0x0215

// 电源管理消息
#define WM_POWERBROADCAST               0x0218

#define PBT_APMQUERYSUSPEND             0x0000	//请求挂起
//#define PBT_APMQUERYSTANDBY             0x0001   //不支持

#define PBT_APMQUERYSUSPENDFAILED       0x0002	//挂起请求被拒绝失败
//#define PBT_APMQUERYSTANDBYFAILED       0x0003	//不支持

#define PBT_APMSUSPEND                  0x0004	//不支持
//#define PBT_APMSTANDBY                  0x0005	//不支持
//#define PBT_APMRESUMECRITICAL           0x0006	//不支持
#define PBT_APMRESUMESUSPEND            0x0007	//不支持
//#define PBT_APMRESUMESTANDBY            0x0008	//不支持
//#define PBTF_APMRESUMEFROMFAILURE       0x00000001	//不支持

#define PBT_APMBATTERYLOW               0x0009	//低电源消息
#define PBT_APMPOWERSTATUSCHANGE        0x000A	//当前电源状态改变

//#define PBT_APMOEMEVENT                 0x000B
//#define PBT_APMRESUMEAUTOMATIC          0x0012

#define BROADCAST_QUERY_DENY            0xde

#define WM_CUT                          0x0300
#define WM_COPY                         0x0301
#define WM_PASTE                        0x0302
#define WM_CLEAR                        0x0303

#define WM_PRINT                        0x0317
#define WM_PRINTCLIENT                  0x0318


#define WM_SYS                          0x0400			// WM_SYS ~ WM_OS 系统区,不可重叠区

#define KM_SETNEXTKEY                   (WM_SYS+0)

// define start bar command
#define SBC_UNUSED                      (-1)
#define SBC_STARTMENU                   (-2)
#define SBC_HELP                        (-3)

#define SBC_CLOSE                       (-4)
#define SBC_USERMENU                    (-5)
#define WM_STARTBAR                     (WM_SYS+1)
#define SBM_GETSTARTMENU                (WM_SYS+2)
#define SBM_SETID                       (WM_SYS+3)
#define SBM_REMOVEITEM                  (WM_SYS+4)
#define SBM_SETSTYLE                    (WM_SYS+5)


// give shell a chance to get sys change,
// wParam  value:
// foreground window changed
#define WM_SHELLNOTIFY                  (WM_SYS+6)
// WM_SHELLNOTIFY  wParam  define
#define SN_SETFOREGROUNDWINDOW   1
#define SN_REFRESH               2


#define WM_SYSSETFOREGROUND                (WM_SYS+7)

#define STM_SEND  1		// 发送
#define STM_REPLY 2   //   回复
// wParam = STM_SEND or STM_REPLY
#define WM_SYSTHREADMSG                 (WM_SYS+8)

//消息名称

#define WM_SETCTLCOLOR		(WM_SYS+9)

// 设置控件的颜色

//参数

//wParam 
//	保留, 为0
//lParam 
//	LPCTLCOLORSTRUCT pCtlColor;
//成功，return TRUE；失败， return FALSE

#define WM_GETCTLCOLOR		(WM_SYS+10)

// 得到或者设置控件的颜色

//为网络提供的系统消息，共11个
#define WM_NET_FIRST    (WM_SYS+11)    //从
#define WM_NET_LAST     (WM_SYS+21)		

//

//参数

//wParam 
//	保留, 为0
//lParam 
//	LPCTLCOLORSTRUCT pCtlColor;
//成功，return TRUE；失败， return FALSE

typedef struct{
    UINT     cbSize; // = sizeof(CTLCOLORSTRUCT)
	UINT     fMask;
	COLORREF cl_Text;
	COLORREF cl_TextBk;     //正常文本的前景与背景色
	COLORREF cl_Selection;
	COLORREF cl_SelectionBk;  // 选择文本的前景与背景色
	COLORREF cl_Disable;
	COLORREF cl_DisableBk;    // 无效文本的前景与背景色
	COLORREF cl_ReadOnly;
	COLORREF cl_ReadOnlyBk;   // 只读文本的前景与背景色
	COLORREF cl_Title;
	COLORREF cl_TitleBk;      // 标题文本的前景与背景色
}CTLCOLORSTRUCT, *LPCTLCOLORSTRUCT;


#define CLF_TEXTCOLOR			0x00000001
#define CLF_TEXTBKCOLOR			0x00000002
#define CLF_SELECTIONCOLOR		0x00000004
#define CLF_SELECTIONBKCOLOR	0x00000008

#define CLF_DISABLECOLOR		0x00000010
#define CLF_DISABLEBKCOLOR		0x00000020

#define CLF_READONLYCOLOR		0x00000040
#define CLF_READONLYBKCOLOR		0x00000080
#define CLF_TITLECOLOR		    0x00000100
#define CLF_TITLEBKCOLOR		0x00000200
//#define CLF_USER1COLOR			0x00000400
//#define CLF_USER2COLOR			0x00000800
//#define CLF_USER3COLOR			0x00001000
//#define CLF_USER4COLOR			0x00002000


#define WM_OS                           0x0600   //( 0x600  ~ 0x4000 )

#define WM_USER                         0x4000

#define WM_APP                          0x8000


// queue event

#define QS_KEY              0x0001
#define QS_MOUSEMOVE        0x0002
#define QS_MOUSEBUTTON      0x0004
#define QS_POSTMESSAGE      0x0008
#define QS_TIMER            0x0010
#define QS_PAINT            0x0020
#define QS_SENDMESSAGE      0x0040
#define QS_HOTKEY           0x0080
#define QS_ALLPOSTMESSAGE   0x0100

#define QS_MOUSE           (QS_MOUSEMOVE | QS_MOUSEBUTTON)

#define QS_INPUT           (QS_MOUSE | QS_KEY )

#define QS_ALLEVENTS       (QS_INPUT         | \
                            QS_POSTMESSAGE   | \
                            QS_TIMER         | \
                            QS_PAINT         | \
                            QS_HOTKEY)

#define QS_ALLINPUT        (QS_INPUT         | \
                            QS_POSTMESSAGE   | \
                            QS_TIMER         | \
                            QS_PAINT         | \
                            QS_HOTKEY        | \
                            QS_SENDMESSAGE)

#define QS_EVENT_MASK       0xffff

#define PostMessage Msg_Post
BOOL WINAPI Msg_Post( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

#define PostThreadMessage Msg_PostThread
BOOL WINAPI Msg_PostThread( DWORD idThread, UINT uMsg, WPARAM wParam, LPARAM lParam );

#define GetMessage Msg_Get
BOOL WINAPI Msg_Get( LPMSG lpMsg, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax );

//PeekMessage Options
#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PeekMessage Msg_Peek
BOOL WINAPI Msg_Peek( LPMSG lpMsg, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax, UINT wRemoveMsg );

#define PostQuitMessage Msg_PostQuit
void WINAPI Msg_PostQuit( int nExitCode );

#define DispatchMessage Msg_Dispatch
LRESULT WINAPI Msg_Dispatch( const MSG FAR * lpMsg );

#define TranslateMessage Msg_Translate
BOOL WINAPI Msg_Translate( const MSG FAR * lpMsg );

#define SendMessage Msg_Send
LRESULT WINAPI Msg_Send( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#define SMTO_NORMAL         0x0000
#define SMTO_BLOCK          0x0001
#define SendMessageTimeout Msg_SendTimeout
LRESULT WINAPI Msg_SendTimeout( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, PDWORD lpdwResult );

#define WaitMessage Msg_Wait
BOOL WINAPI Msg_Wait( void );

#define MOUSEEVENTF_MOVE        0x0001 // mouse move
#define MOUSEEVENTF_LEFTDOWN    0x0002 // left button down
#define MOUSEEVENTF_LEFTUP      0x0004 // left button up
#define MOUSEEVENTF_RIGHTDOWN   0x0008 // right button down 
#define MOUSEEVENTF_RIGHTUP     0x0010 // right button up 
#define MOUSEEVENTF_MIDDLEDOWN  0x0020 // middle button down
#define MOUSEEVENTF_MIDDLEUP    0x0040 // middle button up
#define MOUSEEVENTF_WHEEL       0x0800 // wheel button rolled
#define MOUSEEVENTF_ABSOLUTE    0x8000 // absolute move

#define mouse_event MouseEvent
VOID WINAPI MouseEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo );

#define KEYEVENTF_EXTENDEDKEY 0x0001
#define KEYEVENTF_KEYUP       0x0002
#define KEYEVENTF_CHAR        0x0004

#define keybd_event KeybdEvent
VOID WINAPI KeybdEvent( BYTE bVk, BYTE bScan, DWORD dwFlags, DWORD dwExtraInfo );


#define HWND_BROADCAST  ((HWND)0x0000ffff)

#define MsgWaitForMultipleObjects Msg_WaitForMultipleObjects
DWORD WINAPI Msg_WaitForMultipleObjects(
									   DWORD nCount,
									   LPHANDLE pHandles,
									   BOOL fWaitAll,
									   DWORD dwMilliseconds,
									   DWORD dwWakeMask
									   );

#define MessageBeep Msg_Beep
BOOL WINAPI Msg_Beep( UINT uType );

#define GetKeyState Msg_GetKeyState
SHORT WINAPI Msg_GetKeyState( int nVirtKey );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // __EMESSAGE_H
