/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：英文键盘
版本号：1.0.0.456
开发时期：2004-05-19
作者：陈建明 Jami chen
修改记录：
**************************************************/
//#include <eframe.h>
#include "ewindows.h"
#include <thirdpart.h>
#include "resource.h"
#include <softkey.h>
#include <showstyle.h>


// *******************************************************
// 变量定义区
// *******************************************************
typedef struct {
	int (*Who)(HWND hWnd);
	void (*Create)(HWND hWnd);
	void (*Destroy)(HWND hWnd);
	void (*Resume)(HWND hWnd);
	int (*DoPAINT)( HWND hWnd, HDC hdc );
	int (*DoLBUTTONDOWN)( HWND hWnd, int x, int y );
	int (*DoLBUTTONUP)( HWND hWnd, int x, int y );
	int (*DoMOUSEMOVE)( HWND hWnd, int x, int y );
	BOOL (*GetKeyValue)(HWND hWnd, WORD * );
	LRESULT (*DoDefWindowProc)(HWND hWnd,UINT uMag,WPARAM wParam,LPARAM lParam);
}KEYDRV, FAR * LPKEYDRV;

extern HINSTANCE g_hInstance;

//#define KBDBMPWIDTH  240
//#define KBDBMPHEIGHT 120
#define KBDBMPWIDTH  KEYBOARDWIDTH
#define KBDBMPHEIGHT KEYBOARDHEIGHT

//#define KEYBOARDWIDTH  210
//#define KEYBOARDHEIGHT 84

#define ENG_PRIVATEDATA		8

#define INVERTSHOW  0x0001
#define NORMALSHOW  0x0002
#define DISABLESHOW 0x0003

#define STARTX  1

typedef struct KeyValue{
	BYTE bChar; // 键的值
	BYTE bVk;  // 键的虚键值
	WORD wSBCCase; // 全角输出
}KEYVALUE;

typedef struct {
    KEYVALUE  keyValue[6]; // 存放键值的数组
	int		iKeyNum;  // 键值的个数
    int     iLeft;  // 开始位置
	int     iWidth; // 键的宽度
} KEYENTRY;

#define NUM_KEYBDS 1

#define NONEKEY			0  // 已经没有字符了
#define PINYIN_KB		1  // 切换到拼音键盘
#define YINGWEN_KB		2  // 切换到英文键盘
#define FUHAO_KB		3  // 切换到符号键盘
#define QUANJIAO		4  // 转化到全角模式
#define HANDWRITE_KB	5  // 切换到手写键盘


//#define BLK_HEIGHT 16
#define COLUMN_NUM 12
#define ROW_NUM    5

//static int g_RowHeightList[ROW_NUM+1] = {1,31,61,91,120};
//static int g_RowHeightList[ROW_NUM+1] = {2,22,43,63,84,104};
static int g_RowHeightList[ROW_NUM+1] = {1,18,34,50,66,82};
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {1,40,80,120,160,200,240};
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {1,30,60,90,120,150,180};
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {0,30,60,90,120,150,180,210}; // 有箭头，7个区域

// 键盘数据
const static KEYENTRY g_engkeys[2][ROW_NUM][COLUMN_NUM + 1] = {
	{
		//第一行
		{
			{		{{'`',0,0xa1a2}},			1,     0 + STARTX,	20},  // `
			{		{{'1',0,0xa3b1}},			1,     20 + STARTX,	16},
			{		{{'2',0,0xa3b2}},			1,    36 + STARTX,	16},
			{		{{'3',0,0xa3b3}},			1,    52 + STARTX,	16},
			{		{{'4',0,0xa3b4}},			1,    68 + STARTX,	16},
			{		{{'5',0,0xa3b5}},			1,    84 + STARTX,	16},
			{		{{'6',0,0xa3b6}},			1,    100 + STARTX,	16},
			{		{{'7',0,0xa3b7}},			1,    116+ STARTX,	16},
			{		{{'8',0,0xa3b8}},			1,    132+ STARTX,	16},
			{		{{'9',0,0xa3b9}},			1,    148+ STARTX,	16},
			{		{{'0',0,0xa3b0}},			1,    164+ STARTX,	16},
			{		{{'\x08',VK_BACK,0}},		1,    180 + STARTX,	28},
			{		 {{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第二行
		{
//			{		{{'\t',VK_TAB,0}}, 			1,     0 + STARTX,	25},
			{		{{'/',0,0xa1c2}},			1,	  0 + STARTX,	25},
			{		{{'Q','Q',0xa3d1}},			1,    25 + STARTX,	16},
			{		{{'W','W',0xa3d7}},			1,    41 + STARTX,	16},
			{		{{'E','E',0xa3c5}},			1,    57 + STARTX,	16},
			{		{{'R','R',0xa3d2}},			1,    73 + STARTX,	16},
			{		{{'T','T',0xa3d4}},			1,    89 + STARTX,	16},
			{		{{'Y','Y',0xa3d9}},			1,    105+ STARTX,	16},
			{		{{'U','U',0xa3d5}},			1,    121+ STARTX,	16},
			{		{{'I','I',0xa3c9}},			1,    137+ STARTX,	16},
			{		{{'O','O',0xa3cf}},			1,    153+ STARTX,	16},
			{		{{'P','P',0xa3d0}},			1,    169+ STARTX,	16},
			{		{{'\\',0,0xa3dc}},			1,    185+ STARTX,	24},
			{		 {{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第三行
		{
			{		{{0,VK_CAPITAL,0}},			1,     0 + STARTX,	29},
			{		{{'A','A',0xa3c1}},			1,    29 + STARTX,	16},
			{		{{'S','S',0xa3d3}},			1,    45 + STARTX,	16},
			{		{{'D','D',0xa3c4}},			1,    61 + STARTX,	16},
			{		{{'F','F',0xa3c6}},			1,    77 + STARTX,	16},
			{		{{'G','G',0xa3c7}},			1,    93 + STARTX,	16},
			{		{{'H','H',0xa3c8}},			1,    109+ STARTX,	16},
			{		{{'J','J',0xa3ca}},			1,    125+ STARTX,	16},
			{		{{'K','K',0xa3cb}},			1,    141+ STARTX,	16},
			{		{{'L','L',0xa3cc}},			1,    157+ STARTX,	16},
			{		{{';',0,0xa3bb}},			1,    173 + STARTX,	16},
			{		{{'\xd',VK_RETURN,0}},		1,    189 + STARTX,	19},
			{		{{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第四行
		{
			{		{{0,VK_SHIFT,0}},			1,     0 + STARTX,	32},
			{		{{'Z','Z',0xa3da}},			1,    32 + STARTX,	16},
			{		{{'X','X',0xa3d8}},			1,    48 + STARTX,	16},
			{		{{'C','C',0xa3c3}},			1,    64 + STARTX,	16},
			{		{{'V','V',0xa3d6}},			1,    80 + STARTX,	16},
			{		{{'B','B',0xa3c2}},			1,    96+ STARTX,	16},
			{		{{'N','N',0xa3ce}},			1,    112+ STARTX,	16},
			{		{{'M','M',0xa3cd}},			1,    128+ STARTX,	16},
			{		{{',',0,0xa3ac}},			1,    144 + STARTX,	16},
			{		{{'.',0,0xa1a3}},			1,	  160 + STARTX,	16},
			{		{{0,VK_UP,0}},				1,    176 + STARTX,	16},
			{		{{'\'',0,0}},				1,    192 + STARTX,	16},
			{		 {{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第五行
		{
			{		{{0,VK_TURN,0}},		1,    0+ STARTX,	16},
			{		{{'_',0,0xa3df}},		1,	  16 + STARTX,	16},  // _
			{		{{'=',0,0xa3bd}},		1,    32 + STARTX,	16},  // =
			{		{{'[',0,0xa3db}},		1,	  48 + STARTX,	16},  // [
			{		{{']',0,0xa3dd}},		1,	  64 + STARTX,	16},  // ]
			{		{{' ',VK_SPACE,0}},		1,    80 + STARTX,  80},
//			{		{{'/',0,0xa1c2}},		1,	  144 + STARTX,	16},
			{		{{0,VK_LEFT,0}},		1,    160 + STARTX,	16},
			{		{{0,VK_DOWN,0}},		1,    176 + STARTX,	16},
			{		{{0,VK_RIGHT,0}},		1,    192 + STARTX,	16},
			{		 {{0,NONEKEY,0}},		0,	  0,			0}

		},
	},
	{  // 小写键盘
		//第一行
		{
			{		{{'~',0,0xa1ab}},			1,    0 + STARTX,	20},  // ~
			{		{{'!',0,0xa3a1}},			1,    20 + STARTX,	16},
			{		{{'@',0,0xa3c0}},			1,    36 + STARTX,	16},
			{		{{'#',0,0xa3a3}},			1,	  52 + STARTX,	16},  // #
			{		{{'$',0,0xa1e7}},			1,    68 + STARTX,	16},  // $
			{		{{'%',0,0xa3a5}},			1,    84 + STARTX,	16},  // %
			{		{{'^',0,0xa3de}},			1,    100 + STARTX,	16},  // ^
			{		{{'&',0,0xa3a6}},			1,    116+ STARTX,	16},  // &
			{		{{'*',0,0xa1c1}},			1,	  132+ STARTX,	16},
			{		{{'(',0,0xa3a8}},			1,    148+ STARTX,	16},  // (
			{		{{')',0,0xa3a9}},			1,    164+ STARTX,	16},  // )
			{		{{'\x08',VK_BACK,0}},		1,    180 + STARTX,	28},
			{		 {{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第二行
		{
//			{		{{'\t',VK_TAB,0}}, 			1,     0 + STARTX,	25},
			{		{{'?',0,0xa3bf}},			1,    0 + STARTX,	25},
			{		{{'q','Q',0xa3d1}},			1,    25 + STARTX,	16},
			{		{{'w','W',0xa3d7}},			1,    41 + STARTX,	16},
			{		{{'e','E',0xa3c5}},			1,    57 + STARTX,	16},
			{		{{'r','R',0xa3d2}},			1,    73 + STARTX,	16},
			{		{{'t','T',0xa3d4}},			1,    89 + STARTX,	16},
			{		{{'y','Y',0xa3d9}},			1,    105+ STARTX,	16},
			{		{{'u','U',0xa3d5}},			1,    121+ STARTX,	16},
			{		{{'i','I',0xa3c9}},			1,    137+ STARTX,	16},
			{		{{'o','O',0xa3cf}},			1,    153+ STARTX,	16},
			{		{{'p','P',0xa3d0}},			1,    169+ STARTX,	16},
			{		{{'\\',0,0xa3dc}},			1,    185+ STARTX,	24},
			{		 {{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第三行
		{
			{		{{0,VK_CAPITAL,0}},			1,     0 + STARTX,	29},
			{		{{'a','A',0xa3c1}},			1,    29 + STARTX,	16},
			{		{{'s','S',0xa3d3}},			1,    45 + STARTX,	16},
			{		{{'d','D',0xa3c4}},			1,    61 + STARTX,	16},
			{		{{'f','F',0xa3c6}},			1,    77 + STARTX,	16},
			{		{{'g','G',0xa3c7}},			1,    93 + STARTX,	16},
			{		{{'h','H',0xa3c8}},			1,    109+ STARTX,	16},
			{		{{'j','J',0xa3ca}},			1,    125+ STARTX,	16},
			{		{{'k','K',0xa3cb}},			1,    141+ STARTX,	16},
			{		{{'l','L',0xa3cc}},			1,    157+ STARTX,	16},
			{		{{':',0,0xa3ba}},			1,	  173 + STARTX,	16},
			{		{{'\xd',VK_RETURN,0}},		1,    189 + STARTX,	19},
			{		{{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第四行
		{
			{		{{0,VK_SHIFT,0}},			1,     0 + STARTX,	32},
			{		{{'z','Z',0xa3da}},			1,    32 + STARTX,	16},
			{		{{'x','X',0xa3d8}},			1,    48 + STARTX,	16},
			{		{{'c','C',0xa3c3}},			1,    64 + STARTX,	16},
			{		{{'v','V',0xa3d6}},			1,    80 + STARTX,	16},
			{		{{'b','B',0xa3c2}},			1,    96+ STARTX,	16},
			{		{{'n','N',0xa3ce}},			1,    112+ STARTX,	16},
			{		{{'m','M',0xa3cd}},			1,    128+ STARTX,	16},
			{		{{'<',0,0xa1b4}},			1,    144 + STARTX,	16},  // <
			{		{{'>',0,0xa1b5}},			1,    160 + STARTX,	16},  // >
			{		{{0,VK_UP,0}},				1,    176 + STARTX,	16},
			{		{{'\"',0,0}},				1,    192 + STARTX,	16},
			{		 {{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第五行
		{
			{		{{0,VK_TURN,0}},		1,    0+ STARTX,	16},
			{		{{'-',0,0xa3aD}},		1,    16 + STARTX,	16},
			{		{{'+',0,0xa3aB}},		1,    32 + STARTX,	16},
			{		{{'{',0,0xa3fb}},		1,	  48 + STARTX,	16},  // {
			{		{{'}',0,0xa3fd}},		1,	  64 + STARTX,	16},  // }
			{		{{' ',VK_SPACE,0}},		1,    80 + STARTX,  80},
//			{		{{'?',0,0xa3bf}},		1,    144 + STARTX,	24},
			{		{{0,VK_LEFT,0}},		1,    160 + STARTX,	16},
			{		{{0,VK_DOWN,0}},		1,    176 + STARTX,	16},
			{		{{0,VK_RIGHT,0}},		1,    192 + STARTX,	16},
			{		 {{0,NONEKEY,0}},		0,	  0,			0}

		},
	},
/*		//第二行
		{
			{		{{'a','A',0xa3e1},{'A','A',0xa3c1},{'b','B',0xa3e2},{'B','B',0xa3c2},{'c','C',0xa3e3},{'C','C',0xa3c3}},	   6,	  0 + STARTX,	41},
			{		{{'d','D',0xa3e4},{'D','D',0xa3c4},{'e','E',0xa3e5},{'E','E',0xa3c5},{'f','F',0xa3e6},{'F','F',0xa3c6}},	   6,     42+ STARTX,	41},
			{		{{'g','G',0xa3e7},{'G','G',0xa3c7},{'h','H',0xa3e8},{'H','H',0xa3c8},{'i','I',0xa3e9},{'I','I',0xa3c9}},	   6,     84+ STARTX,	40},
			{		{{'j','J',0xa3ea},{'J','J',0xa3ca},{'k','K',0xa3eb},{'K','K',0xa3cb}},			   4,     125 + STARTX,	41},
			{		{{'l','L',0xa3ec},{'L','L',0xa3cc},{'m','M',0xa3ed},{'M','M',0xa3cd}},			   4,     167 + STARTX,	41},
//			{		{{'\x08',VK_BACK,0}},					   1,     199 + STARTX,	39},
			{		{{0,NONEKEY,0}},					   0,	  0,			0},
		},
		//第三行
		{
			{		{{'n','N',0xa3ee},{'N','N',0xa3ce},{'o','O',0xa3ef},{'O','O',0xa3cf},{'p','P',0xa3f0},{'P','P',0xa3d0}},	   6,	  0 + STARTX,	41},
			{		{{'q','Q',0xa3f1},{'Q','Q',0xa3d1},{'r','R',0xa3f2},{'R','R',0xa3d2},{'s','S',0xa3f3},{'S','S',0xa3d3}},	   6,     42+ STARTX,	41},
			{		{{'t','T',0xa3f4},{'T','T',0xa3d4},{'u','U',0xa3f5},{'U','U',0xa3d5},{'v','V',0xa3f6},{'V','V',0xa3d6}},	   6,     84+ STARTX,	40},
			{		{{'w','W',0xa3f7},{'W','W',0xa3d7},{'x','X',0xa3f8},{'X','X',0xa3d8}},			   4,     125 + STARTX,	41},
			{		{{'y','Y',0xa3f9},{'Y','Y',0xa3d9},{'z','Z',0xa3fa},{'Z','Z',0xa3da}},			   4,     167 + STARTX,	41},
//			{		{{0,VK_DELETE,0}},				   1,     199 + STARTX,	39},
			{		{{'\x08',VK_BACK,0}},								       1,     210 + STARTX,	30},
			{		{{0,NONEKEY,0}},					   0,	  0,			0},
		},
		//第四行
		{
			{		{{',',0,0xa3ac}},			1,    0+ STARTX,	28},
//			{		{{0,0,0xa1a3}},				1,	  29 + STARTX,	29},
			{		{{'.',0,0xa1a3}},			1,	  29 + STARTX,	29},
			{		{{'?',0,0xa3bf}},			1,    59 + STARTX,	29},
			{		{{' ',VK_SPACE,0}},			1,    89 + STARTX,  29},
//			{		{{0,PINYIN_KB,0}},			1,	  89 + STARTX,	29},
			{		{{0,FUHAO_KB,0}},			1,    119+ STARTX,	29},
			{		{{0,HANDWRITE_KB,0}},		1,    149 + STARTX, 29},
			{		{{0,YINGWEN_KB,0}},			1,    179+ STARTX,	29},
//			{		{{'\xd',VK_RETURN,0}},		1,     199 + STARTX,	39},
			{		{{'\xd',VK_RETURN,0}},		1,    209 + STARTX,	29},
			{		{{0,NONEKEY,0}},			0,	  0,			0},
		},
*/
};



//extern HBITMAP g_hPYBitmap;
//extern HBITMAP g_hDisableArrow;
static HBITMAP g_hBitmap = NULL;  // 当前键盘的位图句柄
static HBITMAP g_hInvertBitmap = NULL; // 当前按下位图的句柄

static LPTSTR lpKBImageFileName[1][4] = {
	{
		"./kingmos/EngbUpperCase.bmp",
		"./kingmos/EngbUpperCaseInv.bmp",
		"./kingmos/EngbLowerCase.bmp",
		"./kingmos/EngbLowerCaseInv.bmp",
	},
};

static RECT g_rectLeftArrow = {210,1,240,26};
static RECT g_rectRightArrow = {210,27,240,51};

// 以下的内容需要与窗口挂钩
typedef struct structENG_Keyboard{
	BOOL g_bSBCCase;
	RECT eng_HitRect;  // 当前按键的位置
	KEYENTRY *eng_lpHitKey; // 当前按下的键
//	KEYENTRY *eng_lpSelectKey; // 当前供用户选择的键
	BOOL Eng_KeyBroad_Shift;  // 当前的SHIFT的状态
	BOOL Eng_KeyBroad_Capital;  // 当前的CAPS状态
	int g_iBitmapIndex;   // 当前使用的位图序列号
	BOOL g_bCursorLeft;  // 当前鼠标是否已经离开了开始按下的键
}ENG_KEYBOARD, *LPENG_KEYBOARD;

#if 0
static BOOL g_bSBCCase = FALSE;
static RECT eng_HitRect={0,0,0,0};  // 当前按键的位置
static 	KEYENTRY *eng_lpHitKey = NULL; // 当前按下的键
static 	KEYENTRY *eng_lpSelectKey = NULL; // 当前供用户选择的键
static BOOL Eng_KeyBroad_Capital = FALSE;  // 当前的CAPS状态
//static BOOL Eng_KeyBroad_Shift = FALSE;  // 当前的SHIFT状态
static int g_iBitmapIndex = 0;   // 当前使用的位图序列号
static BOOL g_bCursorLeft = FALSE;  // 当前鼠标是否已经离开了开始按下的键
#endif

// *******************************************************
// 函数定义区
// *******************************************************
static int eng_Who(HWND hWnd);
static void eng_Create(HWND hWnd);
static void eng_Destroy(HWND hWnd);
static void eng_Resume(HWND hWnd);
static int eng_DoPAINT( HWND, HDC );
static int eng_DoLBUTTONDOWN( HWND, int x, int y );
static int eng_DoLBUTTONUP( HWND, int x, int y );
static int eng_DoMOUSEMOVE( HWND, int x, int y );
static BOOL eng_GetKeyValue(HWND hWnd, WORD * lpKey );
static void eng_GetKeyRect( int row, int col, LPRECT lprect );
static LRESULT eng_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

//static KEYENTRY *GetKey(int x,int y,RECT *lprect);
static KEYENTRY *GetKey(HWND hWnd,int x,int y,RECT *lprect);
static void DrawSystemKey(HWND hWnd,HDC hdc,BYTE bVk);

static void ShowKeyboardrect(HWND hWnd,HDC hdc,RECT rect,DWORD dwShowStyle);
static BOOL GetVirtualKeyRect(HWND hWnd,BYTE bVk, RECT *lprect);

//static void ShowSelectArea(HWND hWnd);
//static void ClearSelectArea(HWND hWnd);
//static int GetSelectIndex(int x,int y);
static BOOL SelectKey(HWND hWnd, KEYENTRY *lpKey,int iIndex);
static BOOL LoadEngKeyboardImage(HWND hWnd, BOOL bReLoad);


void KB_DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop);
extern void StartMoveKeyboard(HWND hWnd , int x,int y);
extern void MoveKeyboard(HWND hWnd , int x,int y);
extern void EndMoveKeyboard(HWND hWnd);
extern BOOL KeyboardMoving(HWND hWnd);

//#define LEFTARROW		100
//#define RIGHTARROW		101
//static void DrawArrow(HWND hWnd,HDC hdc,UINT iArrowType,UINT iArrowStatus);

// 定义英文键盘函数
const KEYDRV engKeyboard = {
	eng_Who,
	eng_Create,
	eng_Destroy,
	eng_Resume,
	eng_DoPAINT,
	eng_DoLBUTTONDOWN,
	eng_DoLBUTTONUP,
	eng_DoMOUSEMOVE,
    eng_GetKeyValue,
	eng_DefWindowProc };


// **************************************************
// 声明：static int eng_Who(HWND hWnd)
// 参数：
// 	无
// 返回值：返回当前键盘的标识。
// 功能描述：
// 引用: 
// **************************************************
static int eng_Who(HWND hWnd)
{
	return 1;
}

// **************************************************
// 声明：static void eng_Create(HWND hWnd)
// 参数：
// 	无
// 返回值：无
// 功能描述：创建键盘。
// 引用: 
// **************************************************
static void eng_Create(HWND hWnd)
{
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)malloc(sizeof(ENG_KEYBOARD));
		if (lpENG_Keyboard == NULL)
			return;
		memset(lpENG_Keyboard,0,sizeof(ENG_KEYBOARD));
		lpENG_Keyboard->Eng_KeyBroad_Capital = TRUE;  // 默认为大写状态
		lpENG_Keyboard->Eng_KeyBroad_Shift = FALSE;

		SetWindowLong(hWnd,ENG_PRIVATEDATA,(LONG)lpENG_Keyboard);
		eng_Resume(hWnd); // 恢复键盘
}

// **************************************************
// 声明：static void eng_Destroy(HWND hWnd)
// 参数：
// 	无
// 返回值：无
// 功能描述：破坏键盘。
// 引用: 
// **************************************************
static void eng_Destroy(HWND hWnd)
{
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return;
		free(lpENG_Keyboard);
		SetWindowLong(hWnd,ENG_PRIVATEDATA,0);
/*
	if (g_hPYBitmap != NULL)
	{ // 已经装载键盘位图
		DeleteObject(g_hPYBitmap); // 删除位图句柄
		g_hPYBitmap = NULL;
	}
*/
/*	if (g_hInvertBitmap != NULL)
	{ // 已经装载按下状态键盘位图
		DeleteObject(g_hInvertBitmap);  // 删除位图句柄
		g_hInvertBitmap = NULL;
	}
*/
}

// **************************************************
// 声明：static void eng_Resume(HWND hWnd)
// 参数：
// 	无
// 返回值：无
// 功能描述：恢复键盘。
// 引用: 
// **************************************************
static void eng_Resume(HWND hWnd)
{
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return;
//	eng_Count = 0;
		SetRect(&lpENG_Keyboard->eng_HitRect,0,0,0,0);  // 设置键盘的按键状态为空
}
// **************************************************
// 声明：static LRESULT eng_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
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
static LRESULT eng_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

// **************************************************
// 声明：static KEYENTRY *GetKey(HWND hWnd,int x,int y,RECT *lprect)
// 参数：
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 	OUT lprect -- 返回按键的矩形
// 
// 返回值：返回当前按下的键的结构指针
// 功能描述：根据坐标位置得到按下的键的指针
// 引用: 
// **************************************************
static KEYENTRY *GetKey(HWND hWnd,int x,int y,RECT *lprect)
{
	int iRow=0,iColumn=0;
	int iKeyIndex;
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return NULL;

		if (y < 0)
			return NULL;
		// 得到当前按下的位置在键盘的第几行
		while(1)
		{
//			if (y < (iRow+1)*BLK_HEIGHT)
			if (y < g_RowHeightList[iRow+1]) // 当前Y坐标在是否在下一行的数据
				break; 
			iRow ++;
			if (iRow >=ROW_NUM)  // 已经到最后一行
				return NULL; // 没有按到任何键
		}

		if (lpENG_Keyboard->Eng_KeyBroad_Capital == FALSE)
		{ // 小写
			iKeyIndex = 1;
		}
		else
		{
			iKeyIndex = 0;
		}
		if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
		{
			iKeyIndex = 1 - iKeyIndex; // 取反
		}

		// 得到当前按下的位置在键盘的指定行的那一列
		while(1)
		{
			if (g_engkeys[iKeyIndex][iRow][iColumn].iKeyNum == 0)  // 没有任何按键
				return NULL;

			if ( x < g_engkeys[iKeyIndex][iRow][iColumn].iLeft + g_engkeys[iKeyIndex][iRow][iColumn].iWidth) // 是否在指定的列
				break;
			
			iColumn ++;
		}
		// 已经找到指定的列
		if (lprect != NULL)
		{ // 得到指定按键的矩形
			lprect->left = g_engkeys[iKeyIndex][iRow][iColumn].iLeft;
			lprect->right = lprect->left + g_engkeys[iKeyIndex][iRow][iColumn].iWidth;
			lprect->top = g_RowHeightList[iRow];
			lprect->bottom = g_RowHeightList[iRow+1]-1;
		}
		return ( KEYENTRY *)&g_engkeys[iKeyIndex][iRow][iColumn];  // 返回按键的指针
}

#define KEYEVENTF_KEYDOWN 0

// **************************************************
// 声明：static int eng_DoLBUTTONDOWN( HWND hWnd, int x, int y )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 
// 返回值：无
// 功能描述：鼠标左键按下。
// 引用: 
// **************************************************
static int eng_DoLBUTTONDOWN( HWND hWnd, int x, int y )
{
	RECT rect;
	HDC hdc;
	KEYENTRY *lpKey;
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return FALSE;

		if (IsRectEmpty(&lpENG_Keyboard->eng_HitRect) == 0)
		{ // 不是空矩形
			hdc = GetDC( hWnd );
			ShowKeyboardrect(hWnd,hdc,lpENG_Keyboard->eng_HitRect,NORMALSHOW); // 恢复原来按键的显示
			ReleaseDC( hWnd, hdc );
			SetRect(&lpENG_Keyboard->eng_HitRect,0,0,0,0);  // 设置为空矩形
		}
#if 0
		if (lpENG_Keyboard->eng_lpSelectKey)
		{
			// 如果有选择键
			int iIndex;
				
				iIndex = GetSelectIndex(x,y); // 得到当前点的选择区域的索引值

				if (iIndex >= 0 && iIndex < 6)
				{
					// 笔点到选择区
					if (SelectKey(hWnd,lpENG_Keyboard->eng_lpSelectKey,iIndex) == TRUE) // 选择键
					{ // 已经选择
						lpENG_Keyboard->eng_lpSelectKey = NULL; // 清除选择键
						ClearSelectArea(hWnd);
					}
					return TRUE;
				}
		}
#endif
		// 得到指定位置的键
		lpKey = GetKey(hWnd,x,y,&rect);
		if (lpKey == NULL)
			return FALSE; // 没有键
/*
// 键盘不可以移动
		if (lpKey->bVk[0] == VK_MOVE)
		{ // 当前按键是拖动键盘移动
			StartMoveKeyboard(hWnd,x,y); // 开始移动键盘
			return TRUE;
		}
*/
#if 0
		if (lpENG_Keyboard->eng_lpSelectKey)
		{ // 当前有选择键
			lpENG_Keyboard->eng_lpSelectKey = NULL; // 清除选择键
			ClearSelectArea(hWnd);
		}
#endif
		lpENG_Keyboard->eng_HitRect = rect; // 设置按下键的矩形
		lpENG_Keyboard->eng_lpHitKey = lpKey; // 设置按下的键
		hdc = GetDC( hWnd );
		//InvertRect(hdc,&rect);
		ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW);  // 显示当前按键为按下状态
		ReleaseDC( hWnd, hdc );
		lpENG_Keyboard->g_bCursorLeft = FALSE; // 没有离开当前的按键

		if (lpKey->iKeyNum == 1)
		{
			if (lpKey->keyValue[0].bVk == PINYIN_KB)
			{ // 需要切换到拼音键盘
				eng_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_SPELL,0); // 切换到拼音键盘
			}
			else if (lpKey->keyValue[0].bVk == YINGWEN_KB)
			{ // 需要切换到英文键盘
				eng_Resume(hWnd);
				// 当前已经是英文键盘
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_SPELL,0); // 切换到拼音键盘
			}
			else if (lpKey->keyValue[0].bVk == FUHAO_KB)
			{ // 需要切换到符号键盘
				eng_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_SYMBOL,0); // 切换到符号键盘
			}
			else if (lpKey->keyValue[0].bVk == HANDWRITE_KB)
			{ // 需要切换到手写键盘
				eng_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_HANDWRITE,0); // 切换到手写键盘
			}
			else if (lpKey->keyValue[0].bVk == QUANJIAO)
			{ // 需要切换到全角键盘
				if (lpENG_Keyboard->g_bSBCCase == FALSE)
					lpENG_Keyboard->g_bSBCCase = TRUE;
				else
				{
					lpENG_Keyboard->g_bSBCCase = FALSE;
					hdc = GetDC( hWnd );
					DrawSystemKey(hWnd,hdc,QUANJIAO);  // 恢复全角键的状态
					ReleaseDC( hWnd, hdc );
				}
		
				eng_Resume(hWnd);
			}
			else
			{ // 是一般键
				SelectKey(hWnd,lpKey,0); // 只有一个键,直接选择第一个键输出
				//keybd_event(lpKey->keyValue[0].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // 发送一个键按下
				//keybd_event(lpKey->keyValue[0].bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
				//keybd_event(lpKey->keyValue[0].bVk,0,KEYEVENTF_KEYUP,0); // 发送一个键弹起
			}
		}
#if 0
		else
		{
			// 当前的键下有多个键值，需要选择
			lpENG_Keyboard->eng_lpSelectKey = lpKey;
			ShowSelectArea(hWnd);
		}
#endif
		return 0;
}

// **************************************************
// 声明：static int eng_DoLBUTTONUP( HWND hWnd, int x, int y )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 
// 返回值：无效
// 功能描述：处理鼠标左键弹起。
// 引用: 
// **************************************************
static int eng_DoLBUTTONUP( HWND hWnd, int x, int y )
{
	HDC hdc;
	RECT rect;

	KEYENTRY *lpKey;
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return FALSE;

		if (KeyboardMoving(hWnd) == TRUE)
		{  // 键盘在移动状态
			EndMoveKeyboard(hWnd); // 结束键盘移动
			return TRUE; // 返回
		}
		if (lpENG_Keyboard->g_bCursorLeft ==TRUE) // 鼠标已经离开按下的键，不要做任何处理
			return TRUE;
		// 得到指定位置的键
		lpKey = GetKey(hWnd,x,y,&rect);  // 得到键值
		if (lpKey == NULL)
			return FALSE; // 没有任何键
		if (lpENG_Keyboard->eng_lpHitKey !=NULL)
		{  // 按下的键不为空
			if (lpKey->keyValue[0].bVk == QUANJIAO)
			{
				return 0;  // 全角键，不要做任何处理
			}
			hdc = GetDC( hWnd );
//			InvertRect(hdc,&lpENG_Keyboard->eng_HitRect);
//			ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW);
			ShowKeyboardrect(hWnd,hdc,lpENG_Keyboard->eng_HitRect,NORMALSHOW); // 恢复按下的键
			ReleaseDC( hWnd, hdc );
			SetRect(&lpENG_Keyboard->eng_HitRect,0,0,0,0);  // 设置按下的键矩形为空
		}

		if (lpKey->keyValue[0].bVk == VK_TURN)
		{  // 键盘转换到拼音
		   SendMessage(hWnd,KM_SETNEXTKEYBD,0,0); // 转换到下一个文件
		}
		else if (lpKey->keyValue[0].bVk == PINYIN_KB)
		{  // 键盘转换到拼音
		   //SendMessage(hWnd,KM_SETNEXTKEY,0,0); // 转换到下一个文件
		}
		else if (lpKey->keyValue[0].bVk == FUHAO_KB)
		{  // 键盘转换到符号
		   //SendMessage(hWnd,KM_SETNEXTKEY,0,0); // 转换到下一个文件
		}

		else if (lpKey->keyValue[0].bVk == VK_SHIFT)
		{ // 当前按键为SHIFT
			if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
			{  // 已经按下了SHIFT
				lpENG_Keyboard->Eng_KeyBroad_Shift = FALSE; // 设置SHIFT弹起
			}
			else
			{ // 还没有按下SHIFT
				lpENG_Keyboard->Eng_KeyBroad_Shift = TRUE; // 设置SHIFT按下
			}
			LoadEngKeyboardImage(hWnd, TRUE);
			InvalidateRect(hWnd,NULL,TRUE); // 重画键盘
			return 0;
		}
		else if (lpKey->keyValue[0].bVk == VK_CAPITAL)
		{
			if (lpENG_Keyboard->Eng_KeyBroad_Capital == FALSE)
			{
				lpENG_Keyboard->Eng_KeyBroad_Capital = TRUE;
			}
			else
			{
				lpENG_Keyboard->Eng_KeyBroad_Capital = FALSE;
			}
			LoadEngKeyboardImage(hWnd, TRUE);
			InvalidateRect(hWnd,NULL,TRUE);
		}
		else
		{
			if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
			{  // 已经按下了SHIFT
				lpENG_Keyboard->Eng_KeyBroad_Shift = FALSE; // 设置SHIFT弹起
				LoadEngKeyboardImage(hWnd, TRUE);
				InvalidateRect(hWnd,NULL,TRUE); // 重画键盘
			}
		}

		lpENG_Keyboard->eng_lpHitKey = NULL;    // 已经没有按下的键
		return 0;
}

// **************************************************
// 声明：static int eng_DoMOUSEMOVE( HWND hWnd, int x, int y )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 
// 返回值：无效
// 功能描述：处理按着鼠标左键移动。
// 引用: 
// **************************************************
static int eng_DoMOUSEMOVE( HWND hWnd, int x, int y )
{
	KEYENTRY *lpKey;
	RECT rect;
	HDC hdc;
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return FALSE;

		if (KeyboardMoving(hWnd) == TRUE)
		{  // 键盘在移动状态
			MoveKeyboard(hWnd,x,y);  // 移动键盘
			return TRUE;
		}
		if (lpENG_Keyboard->eng_lpHitKey == NULL)
			return FALSE; // 没有按下的键
		// 得到指定位置的键
		lpKey = GetKey(hWnd,x,y,&rect);
		if (lpKey != lpENG_Keyboard->eng_lpHitKey)
		{  // 当前的键与按下的键不同
			if (lpENG_Keyboard->g_bCursorLeft == TRUE)  // 已经离开了按下的键
				return TRUE; // 不需要处理
			if (lpENG_Keyboard->eng_lpHitKey->keyValue[0].bVk != QUANJIAO)
			{  // 按下的键不是全角键
//				InvertRect(hdc,&lpENG_Keyboard->eng_HitRect);
				hdc = GetDC( hWnd );
				ShowKeyboardrect(hWnd,hdc,lpENG_Keyboard->eng_HitRect,NORMALSHOW); // 恢复按下键的状态
				ReleaseDC( hWnd, hdc );
			}
			lpENG_Keyboard->g_bCursorLeft = TRUE; // 设置键已经离开原来按下的键
		}
		else
		{ // 当前的键与按下的键相同
			if (lpENG_Keyboard->g_bCursorLeft == FALSE) // 还没有离开按下的键
				return TRUE;
			// 鼠标离开原来按下的键后又回到原来的按键位置
			if (lpKey->keyValue[0].bVk != QUANJIAO)
			{  // 按下的键不是全角键
//				InvertRect(hdc,&lpENG_Keyboard->eng_HitRect);
				hdc = GetDC( hWnd );
				ShowKeyboardrect(hWnd,hdc,lpENG_Keyboard->eng_HitRect,INVERTSHOW); // 显示键为按下的状态
				ReleaseDC( hWnd, hdc );	
			}
			lpENG_Keyboard->g_bCursorLeft = FALSE; // 设置键在原来按下的位置
		}
		return 0;
}

// **************************************************
// 声明：static int eng_DoPAINT( HWND hWnd, HDC hdc )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值： 无
// 功能描述：绘制英文键盘。
// 引用: 
// **************************************************
static int eng_DoPAINT( HWND hWnd, HDC hdc )
{
  RECT rect={0,0,KBDBMPWIDTH,KBDBMPHEIGHT};
  LPENG_KEYBOARD  lpENG_Keyboard;
  int iStarty;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return 0;
	  iStarty = lpENG_Keyboard->g_iBitmapIndex*KBDBMPHEIGHT ;
//  HBITMAP hBitMap;

/*	  if (g_hBitmap == NULL)
	  {  // 还没有加载图象句柄
		  HINSTANCE hInstance;

//		  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		  hInstance = g_hInstance;
		  //g_hPYBitmap = LoadImage( 0, "\\System\\Keyboard1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_NORMALKB), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载英文键盘位图
//		  g_hPYBitmap = LoadImage( hInstance, "\\KINGMOS\\ENGKB.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载英文键盘位图
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_PINYIN), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载手写键盘位图
		  g_hBitmap = LoadImage( 0, "\\程序\\eng1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hBitmap == 0 )
			return 0;  // 装载失败
	  }
*/
	  LoadEngKeyboardImage(hWnd, FALSE);

	  KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,0,iStarty,SRCCOPY); // 绘制键盘

	  DrawSystemKey(hWnd,hdc,VK_SHIFT);  // 恢复全角键的状态
	  DrawSystemKey(hWnd,hdc,VK_CAPITAL);  // 恢复全角键的状态
	  return 0;
}
// **************************************************
// 声明：void KB_DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBitmap -- 位图句柄
// 	IN rect -- 位图矩形
// 	IN xOffset -- X坐标偏移
// 	IN yOffset -- Y坐标偏移
// 	IN dwRop -- 绘制模式
// 
// 返回值：无
// 功能描述：绘制位图。
// 引用: 
// **************************************************
void KB_DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
{
		HDC hMemoryDC;
    HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc);  // 创建兼容DC
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // 将要绘制的位图选择到DC
		// 绘制位图
		BitBlt( hdc, // handle to destination device context
			(short)rect.left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)rect.top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(rect.right-rect.left),  // width of destination rectangle
			(short)(rect.bottom-rect.top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			(short)xOffset,   // x-coordinate of source rectangle's upper-left
									 // corner
			(short)yOffset,   // y-coordinate of source rectangle's upper-left
									 // corner

			dwRop
			);

		SelectObject(hMemoryDC,hOldBitmap); // 恢复位图句柄
		DeleteDC(hMemoryDC); // 删除DC
}
// **************************************************
// 声明：static BOOL eng_GetKeyValue(HWND hWnd, WORD * lpKey )
// 参数：
// 	OUT lpKey -- 得到当前按键的值
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到当前按键的值。
// 引用: 
// **************************************************
static BOOL eng_GetKeyValue(HWND hWnd, WORD * lpKey )
{
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return FALSE;
	if( lpENG_Keyboard->eng_lpHitKey )
	{  // 有键按下
		*lpKey = lpENG_Keyboard->eng_lpHitKey->keyValue[0].bChar; // 返回当前按键的值
		return TRUE;
	}
	else
		return FALSE; // 返回失败
}


// **************************************************
// 声明：static void DrawSystemKey(HWND hWnd,HDC hdc,BYTE bVk)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN bVk -- 系统键的键值
// 
// 返回值：无
// 功能描述：绘制系统键的状态。
// 引用: 
// **************************************************
static void DrawSystemKey(HWND hWnd,HDC hdc,BYTE bVk)
{
	RECT rect;
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return;

		GetVirtualKeyRect(hWnd,bVk,&rect);
		if (bVk == QUANJIAO)
		{
			if (lpENG_Keyboard->g_bSBCCase == TRUE)
				ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // 显示键为按下的状态
			else
				ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW); // 显示键为按下的状态
		}
		if (bVk == VK_CAPITAL)
		{
			if (lpENG_Keyboard->Eng_KeyBroad_Capital == TRUE)
				ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // 显示键为按下的状态
			else
				ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW); // 显示键为按下的状态
		}
		if (bVk == VK_SHIFT)
		{
			if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
				ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // 显示键为按下的状态
			else
				ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW); // 显示键为按下的状态
		}
		if (bVk == YINGWEN_KB)
		{
			ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // 显示键为按下的状态
		}
		
		return;

}

// **************************************************
// 声明：static void ShowKeyboardrect(HWND hWnd,HDC hdc,RECT rect,DWORD dwShowStyle)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN rect -- 要显示的矩形
// 	IN dwShowStyle -- 显示状态
// 
// 返回值：无
// 功能描述：显示键盘的指定区域的位图。
// 引用: 
// **************************************************
static void ShowKeyboardrect(HWND hWnd,HDC hdc,RECT rect,DWORD dwShowStyle)
{
//  int iStarty; // 得到当前键盘的起始位置

/*	  if (g_hBitmap == NULL)
	  {  // 还没有加载图象句柄
		  HINSTANCE hInstance;

//		  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		  hInstance = g_hInstance;
		  //g_hPYBitmap = LoadImage( 0, "\\System\\Keyboard1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
	//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_NORMALKB), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载英文键盘位图
//		  g_hPYBitmap = LoadImage( hInstance, "\\KINGMOS\\ENGKB.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载英文键盘位图
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_PINYIN), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载手写键盘位图
		  g_hBitmap = LoadImage( 0, "\\程序\\eng1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hBitmap == 0 )
			return ;  // 装载失败
	  }
*/
/*	if (g_hInvertBitmap == NULL)
	{ // 没有装载选择位图
		  //g_hInvertBitmap = LoadImage( 0, "\\System\\Keyboard2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  g_hInvertBitmap = LoadImage( (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE(IDB_ENGSEL), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;// 装载位图句柄
		  if( g_hInvertBitmap == 0 )
			return ;
	}
*/
/*	if (dwShowStyle == NORMALSHOW)  // 通常显示
		iStarty = 0;  // 第一个键盘是一般键盘
	else // 选择（反白）显示
		iStarty = KBDBMPHEIGHT;  // 第二个键盘是一般键盘
	
	KB_DrawBitMap(hdc,(HBITMAP)g_hPYBitmap,rect,rect.left + 0,rect.top + iStarty,SRCCOPY);
	*/
/*	iStarty = 0;
	if (dwShowStyle == NORMALSHOW)  // 通常显示
		KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,rect.left + 0,rect.top + iStarty,SRCCOPY);
//		iStarty = 0;  // 第一个键盘是一般键盘
	else // 选择（反白）显示
		//iStarty = KBDBMPHEIGHT;  // 第二个键盘是一般键盘	
		KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,rect.left + 0,rect.top + iStarty,NOTSRCCOPY);
*/
	LoadEngKeyboardImage(hWnd, FALSE);
	if (dwShowStyle == NORMALSHOW)  // 通常显示
		KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,rect.left + 0,rect.top,SRCCOPY);
	else // 选择（反白）显示
		KB_DrawBitMap(hdc,(HBITMAP)g_hInvertBitmap,rect,rect.left + 0,rect.top,SRCCOPY);
}

#if 0
// **************************************************
// 声明：static void ShowSelectArea(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：显示键盘的选择区域。
// 引用: 
// **************************************************
static void ShowSelectArea(HWND hWnd)
{
	int i;
	HDC hdc;
	RECT rect;
	int pStartx[7] = {1,40,80,120,160,200,240};
	// 键盘一共有六个选择区域
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return;

		if (lpENG_Keyboard->eng_lpSelectKey == NULL)
			return ; // 没有需要选择的键
		hdc = GetDC(hWnd); // 得到设备句柄

		SetTextColor(hdc,RGB(0,0,0));
		SetBkColor(hdc,RGB(231,242,255));

		rect.top = g_RowHeightList[0];  // 得到选择区域的垂直位置
		rect.bottom = g_RowHeightList[1] - 1 ;

		for (i = 0 ; i < lpENG_Keyboard->eng_lpSelectKey->iKeyNum ; i ++)
		{
			rect.left = g_ColumnWidthList[i];
			rect.right = g_ColumnWidthList[i + 1] - 2;
			DrawText(hdc,&lpENG_Keyboard->eng_lpSelectKey->keyValue[i].bChar,1,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		ReleaseDC(hWnd,hdc);  // 释放设备句柄
}
// **************************************************
// 声明：static void ClearSelectArea(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：清除键盘的选择区域。
// 引用: 
// **************************************************
static void ClearSelectArea(HWND hWnd)
{
//	int i;
	HDC hdc;
	RECT rect;
	// 键盘一共有六个选择区域

//		hdc = GetDC(hWnd); // 得到设备句柄

//		rect.top = 1;  // 得到选择区域的垂直位置
//		rect.bottom = 30;

//		for (i = 0 ; i < COLUMN_NUM ; i ++)
//		{
//			rect.left = g_ColumnWidthList[i];
//			rect.right = g_ColumnWidthList[i + 1] - 2;
//			FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
//		}
//		ReleaseDC(hWnd,hdc);  // 释放设备句柄

		if (g_hBitmap)
		{
			hdc = GetDC(hWnd); // 得到设备句柄

	//		rect.top = 1;  // 得到选择区域的垂直位置
	//		rect.bottom = 30;
			rect.left = g_ColumnWidthList[0]; 
			rect.right = g_ColumnWidthList[7]; 
			rect.top = g_RowHeightList[0];  // 得到选择区域的垂直位置
			rect.bottom = g_RowHeightList[1] -1 ;
		
			KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,rect.left,rect.top,SRCCOPY); // 绘制键盘
			ReleaseDC(hWnd,hdc);
		}

}

// **************************************************
// 声明：static int GetSelectIndex(int x,int y)
// 参数：
// 	IN x -- x 坐标
// 	IN y -- y 坐标
// 
// 返回值：返回指定坐标所在的索引
// 功能描述：得到当前点的选择区域的索引值。
// 引用: 
// **************************************************
static int GetSelectIndex(int x,int y)
{
	int iIndex = 0;
	int i;
		if (y < 0 || (y >= g_RowHeightList[1])) // 当前Y坐标在是否在下一行的数据
			return -1; // 当前坐标不在第一行
		// 得到当前按下的位置在键盘的指定行的那一列
		for (i = 0 ; i < COLUMN_NUM ; i ++)
		{
			if ( x < g_ColumnWidthList[i + 1])
				return i ;  // x 坐标在当前索引下
		}
		return -1;
}
#endif

// **************************************************
// 声明：static BOOL SelectKey(HWND hWnd, KEYENTRY *lpKey,int iIndex)
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN lpKey --要输出的键
// 	IN iIndex -- 要输出的键的索引号
// 
// 返回值：无
// 功能描述：选择指定索引的键。
// 引用: 
// **************************************************
/*
static BOOL SelectKey(HWND hWnd, KEYENTRY *lpKey,int iIndex)
{
	if (lpKey == NULL)
		return FALSE; // 没有选择键 
	if (iIndex < 0 || iIndex >= lpKey->iKeyNum)
	{
		// 错误的索引
		return FALSE;
	}
	
	if (lpENG_Keyboard->g_bSBCCase == FALSE)
	{  // 输出半角字符
		if (lpKey->keyValue[iIndex].bVk) // 需要输出虚键
			keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // 发送一个键按下
		if (lpKey->keyValue[iIndex].bChar) // 有字符需要输出
			keybd_event(lpKey->keyValue[iIndex].bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
		if (lpKey->keyValue[iIndex].bVk) // 需要输出虚键
			keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYUP,0); // 发送一个键弹起
	}
	else
	{
		// 输出全角字符
		if (lpKey->keyValue[iIndex].wSBCCase) 
		{ // 有全角字符需要输出
			BYTE bChar;

			bChar = (BYTE)(lpKey->keyValue[iIndex].wSBCCase >> 8); // 先输出高位
			keybd_event(bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
			bChar = (BYTE)lpKey->keyValue[iIndex].wSBCCase; // 后输出低位
			keybd_event(bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
		}
		else
		{
			// 没有全角字符输出，输出半角字符
			if (lpKey->keyValue[iIndex].bVk) // 需要输出虚键
				keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // 发送一个键按下
			if (lpKey->keyValue[iIndex].bChar) // 有字符需要输出
				keybd_event(lpKey->keyValue[iIndex].bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
			if (lpKey->keyValue[iIndex].bVk) // 需要输出虚键
				keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYUP,0); // 发送一个键弹起
		}
	}
	return TRUE;
}
*/
static BOOL SelectKey(HWND hWnd, KEYENTRY *lpKey,int iIndex)
{
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return FALSE;
		if (lpKey == NULL)
			return FALSE; // 没有选择键 
		if (iIndex < 0 || iIndex >= lpKey->iKeyNum)
		{
			// 错误的索引
			return FALSE;
		}
		
		if (lpENG_Keyboard->g_bSBCCase == FALSE)
		{  // 输出半角字符
			if (lpKey->keyValue[iIndex].bVk == 0 && lpKey->keyValue[iIndex].bChar == 0)
			{
				// 没有半角字符可以输出， 则输出全角字符
				if (lpKey->keyValue[iIndex].wSBCCase) 
				{ // 有全角字符需要输出
					BYTE bChar;

					bChar = (BYTE)(lpKey->keyValue[iIndex].wSBCCase >> 8); // 先输出高位
					keybd_event(bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
					bChar = (BYTE)lpKey->keyValue[iIndex].wSBCCase; // 后输出低位
					keybd_event(bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
				}
			}
			else
			{
				if (lpKey->keyValue[iIndex].bVk) // 需要输出虚键
					keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // 发送一个键按下
				if (lpKey->keyValue[iIndex].bChar) // 有字符需要输出
					keybd_event(lpKey->keyValue[iIndex].bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
				if (lpKey->keyValue[iIndex].bVk) // 需要输出虚键
					keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYUP,0); // 发送一个键弹起
			}
		}
		else
		{
			// 输出全角字符
			if (lpKey->keyValue[iIndex].wSBCCase) 
			{ // 有全角字符需要输出
				BYTE bChar;

				bChar = (BYTE)(lpKey->keyValue[iIndex].wSBCCase >> 8); // 先输出高位
				keybd_event(bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
				bChar = (BYTE)lpKey->keyValue[iIndex].wSBCCase; // 后输出低位
				keybd_event(bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
			}
			else
			{
				// 没有全角字符输出，输出半角字符
				if (lpKey->keyValue[iIndex].bVk) // 需要输出虚键
					keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // 发送一个键按下
				if (lpKey->keyValue[iIndex].bChar) // 有字符需要输出
					keybd_event(lpKey->keyValue[iIndex].bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息
				if (lpKey->keyValue[iIndex].bVk) // 需要输出虚键
					keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYUP,0); // 发送一个键弹起
			}
		}

		return TRUE;
}

// **************************************************
// 声明：static BOOL GetVirtualKeyRect(HWND hWnd,BYTE bVk, RECT *lprect)
// 参数：
// 	IN bVk -- 指定虚键
// 	OUT lprect -- 返回该虚键的位置
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到指定虚键的位置。
// 引用: 
// **************************************************
static BOOL GetVirtualKeyRect(HWND hWnd,BYTE bVk, RECT *lprect)
{
	int iRow=0,iColumn=0;
	int i;
	int iKeyIndex = 0;
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return FALSE;
		if (lpENG_Keyboard->Eng_KeyBroad_Capital == FALSE)
		{ // 小写
			iKeyIndex = 1;
		}
		else
		{
			iKeyIndex = 0;
		}
		if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
		{
			iKeyIndex = 1 - iKeyIndex; // 取反
		}

		for (iRow = 0; iRow < ROW_NUM; iRow ++)
		{ // 得到虚键是属于哪一行
			iColumn=0;
			while(1)
			{ // 得到虚键的键的位置

				for ( i = 0; i< g_engkeys[iKeyIndex][iRow][iColumn].iKeyNum; i++)
				{ // 每一个键上可能有多个键值
					if ( bVk == g_engkeys[iKeyIndex][iRow][iColumn].keyValue[0].bVk)
					{ // 当前的键就是虚键
						if (lprect != NULL)
						{ // 指定的指针有效，得到键的位置
							lprect->left = g_engkeys[iKeyIndex][iRow][iColumn].iLeft;
							lprect->right = lprect->left + g_engkeys[iKeyIndex][iRow][iColumn].iWidth;
							lprect->top = g_RowHeightList[iRow];
							lprect->bottom = g_RowHeightList[iRow+1]-1;
						}
						return TRUE;
					}
				}
				
				iColumn ++;
				if (g_engkeys[iKeyIndex][iRow][iColumn].iKeyNum == 0)
					break; // 已经到最后一个键
			}
		}
		return FALSE; // 没有找到指定的键
}

// **************************************************
// 声明：static BOOL LoadEngKeyboardImage(HWND hWnd, BOOL bReLoad)
// 参数：
//	IN hWnd -- 窗口句柄
//	IN bReLoad -- 是否需要重新装载 
// 返回值：成功状态返回TRUE，否则返回FALSE
// 功能描述：装载当前的键盘图象。
// 引用: 
// **************************************************
static BOOL LoadEngKeyboardImage(HWND hWnd, BOOL bReLoad)
{
	int iCurStyle;
	int iStyleIndex = 0;
	LPENG_KEYBOARD  lpENG_Keyboard;
	int iKeyImageIndex,iKeyInvImageIndex,iKeyIndex;
	HINSTANCE hInstance;


		if (bReLoad == FALSE)
		{
			if (g_hBitmap && g_hInvertBitmap)
			{
				return TRUE;
			}
		}

		hInstance = g_hInstance;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return FALSE;

		if (lpENG_Keyboard->Eng_KeyBroad_Capital == FALSE)
		{ // 小写
//			iKeyImageIndex = 2;
//			iKeyInvImageIndex = 3;
			iKeyIndex = 1;
		}
		else
		{
//			iKeyImageIndex = 0;
//			iKeyInvImageIndex = 1;
			iKeyIndex = 0;
		}

		if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
		{
			iKeyIndex = 1 - iKeyIndex; // 取反
		}
		iKeyImageIndex = (iKeyIndex * 2);
		iKeyInvImageIndex = (iKeyIndex * 2) + 1;

		iCurStyle = GetSystemStyle();

		if (iCurStyle == 0)
			iStyleIndex = 0;
		else
			iStyleIndex = 0;

		if (bReLoad == TRUE)
		{
			if (g_hBitmap)
			{
				DeleteObject(g_hBitmap);
				g_hBitmap = NULL;
			}
			if (g_hInvertBitmap)
			{
				DeleteObject(g_hInvertBitmap);
				g_hInvertBitmap = NULL;
			}
		}

		if (g_hBitmap == NULL)
		{  // 还没有加载图象句柄
		  g_hBitmap = LoadImage( 0, lpKBImageFileName[iStyleIndex][iKeyImageIndex], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hBitmap == 0 )
			return FALSE;  // 装载失败
		}
		if (g_hInvertBitmap == NULL)
		{  // 还没有加载图象句柄
		  g_hInvertBitmap = LoadImage( 0, lpKBImageFileName[iStyleIndex][iKeyInvImageIndex], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hInvertBitmap == 0 )
			return FALSE;  // 装载失败
		}

		return TRUE;
}
