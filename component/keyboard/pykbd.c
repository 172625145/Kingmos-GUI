/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：拼音键盘
版本号：1.0.0.456
开发时期：2004-05-19
作者：陈建明 Jami chen
修改记录：
**************************************************/
//#include <eframe.h>
#include "ewindows.h"
#include <thirdpart.h>
#include <softkey.h>
#include <showstyle.h>
#include "resource.h"
#include "lxsearch.h"
#include "pysearch.h"


extern HINSTANCE g_hInstance;
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
	BOOL (*GetKeyValue)( HWND hWnd,WORD * );
	LRESULT (*DoDefWindowProc)(HWND hWnd,UINT uMag,WPARAM wParam,LPARAM lParam);
}KEYDRV, FAR * LPKEYDRV;

//#define KEYBOARDWIDTH  240
//#define KEYBOARDHEIGHT 104

#define KBDBMPWIDTH  KEYBOARDWIDTH
#define KBDBMPHEIGHT KEYBOARDHEIGHT

#define PY_PRIVATEDATA	8
//#define KEYBOARDWIDTH  210
//#define KEYBOARDHEIGHT 84

#define INVERTSHOW  0x0001
#define NORMALSHOW  0x0002

#define ID_SPELL	103

#define STARTX  1

#define NUM_KEYBDS 1

#define NONEKEY		0  // 已经没有字符了
#define PINYIN_KB	1  // 切换到拼音键盘
#define YINGWEN_KB	2  // 切换到英文键盘
#define FUHAO_KB	3  // 切换到符号键盘
#define QUANJIAO	4  // 转化到全角模式
#define HANDWRITE_KB	5  // 切换到手写键盘
#define PAGEUP_SEL	6	// 将选择文本翻到上一页
#define PAGEDOWN_SEL	7	// 将选择文本翻到下一页


//#define BLK_HEIGHT 16

#define SELECT_NUM	8
#define COLUMN_NUM  13
#define ROW_NUM     5

#define SPELLWINDOW_WIDTH	52
#define SPELLWINDOW_HEIGHT	16

//#define LEFTARROWSTART	1
//#define LEFTARROWEND	19
//#define RIGHTARROWSTART	221
//#define RIGHTARROWEND	239
//static RECT g_rectLeftArrow = {210,1,240,26};
//static RECT g_rectRightArrow = {210,27,240,51};
static RECT g_rectLeftArrow = {53,1,66,16};
static RECT g_rectRightArrow = {196,1,209,16};

#define LEFTARROW		100
#define RIGHTARROW		101


//static int g_RowHeightList[ROW_NUM+1] = {1,31,61,91,120};
//atic int g_RowHeightList[ROW_NUM+1] = {2,27,52,78,104};
static int g_RowHeightList[ROW_NUM+1] = {1,18,34,50,66,82};
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {1,40,80,120,160,200,240};
#if SELECT_NUM == 5
static int g_ColumnWidthList[COLUMN_NUM + 1] = {20,59,99,139,179,220,0}; // 有箭头，5个区域
#endif
#if SELECT_NUM == 6
static int g_ColumnWidthList[COLUMN_NUM + 1] = {20,53,86,119,152,185,218}; // 有箭头，6个区域
#endif
#if SELECT_NUM == 7
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {21,49,77,105,133,161,189,217}; // 有箭头，7个区域
static int g_ColumnWidthList[COLUMN_NUM + 1] = {0,30,60,90,120,150,180,210}; // 有箭头，7个区域
#endif
#if SELECT_NUM == 8
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {24,48,72,96,120,144,168,192,216}; // 有箭头，8个区域
static int g_ColumnWidthList[COLUMN_NUM + 1] = {68,84,100,116,132,148,164,180,196}; // 有箭头，8个区域
#endif

// 键盘数据
const static KEYENTRY g_pykeys[ROW_NUM][COLUMN_NUM + 1] = {
		//第一行
		{
			// 显示区域，无数据
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
			{		{{'\x08',VK_BACK,0}},		1,    185 + STARTX,	24},
			{		 {{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第三行
		{
			{		{{'\"',0,0}},				1,     0 + STARTX,	29},
			{		{{'a','A',0xa3c1}},			1,    29 + STARTX,	16},
			{		{{'s','S',0xa3d3}},			1,    45 + STARTX,	16},
			{		{{'d','D',0xa3c4}},			1,    61 + STARTX,	16},
			{		{{'f','F',0xa3c6}},			1,    77 + STARTX,	16},
			{		{{'g','G',0xa3c7}},			1,    93 + STARTX,	16},
			{		{{'h','H',0xa3c8}},			1,    109+ STARTX,	16},
			{		{{'j','J',0xa3ca}},			1,    125+ STARTX,	16},
			{		{{'k','K',0xa3cb}},			1,    141+ STARTX,	16},
			{		{{'l','L',0xa3cc}},			1,    157+ STARTX,	16},
			{		{{';',0,0xa3bb}},			1,    173 + STARTX,	16},
			{		{{'\xd',VK_RETURN,0}},		1,    189 + STARTX,	19},
			{		{{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//第四行
		{
//			{		{{0,VK_SHIFT,0}},			1,     0 + STARTX,	32},
			{		{{'(',0,0xa3a8}},			1,    0+ STARTX,	16},  // (
			{		{{')',0,0xa3a9}},			1,    16+ STARTX,	16},  // )
			{		{{'z','Z',0xa3da}},			1,    32 + STARTX,	16},
			{		{{'x','X',0xa3d8}},			1,    48 + STARTX,	16},
			{		{{'c','C',0xa3c3}},			1,    64 + STARTX,	16},
			{		{{'v','V',0xa3d6}},			1,    80 + STARTX,	16},
			{		{{'b','B',0xa3c2}},			1,    96+ STARTX,	16},
			{		{{'n','N',0xa3ce}},			1,    112+ STARTX,	16},
			{		{{'m','M',0xa3cd}},			1,    128+ STARTX,	16},
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
			{		{{' ',VK_SPACE,0}},		1,    80 + STARTX,  64},
			{		{{'/',0,0xa1c2}},		1,	  144 + STARTX,	16},
			{		{{0,VK_LEFT,0}},		1,    160 + STARTX,	16},
			{		{{0,VK_DOWN,0}},		1,    176 + STARTX,	16},
			{		{{0,VK_RIGHT,0}},		1,    192 + STARTX,	16},
			{		 {{0,NONEKEY,0}},		0,	  0,			0}

		},
/*		//第二行
		{
			{		{{'a','A',0xa3e1},{'b','B',0xa3e2},{'c','C',0xa3e3}},	   3,	  0 + STARTX,	41},
			{		{{'d','D',0xa3e4},{'e','E',0xa3e5},{'f','F',0xa3e6}},	   3,     42+ STARTX,	41},
			{		{{'g','G',0xa3e7},{'h','H',0xa3e8},{'i','I',0xa3e9}},	   3,     84+ STARTX,	40},
			{		{{'j','J',0xa3ea},{'k','K',0xa3eb}},					   2,     125 + STARTX,	41},
			{		{{'l','L',0xa3ec},{'m','M',0xa3ed}},			           2,     167 + STARTX,	41},
//			{		{{'\x08',VK_BACK,0}},								       1,     210 + STARTX,	30},
			{		{{0,NONEKEY,0}},					   0,	  0,			0},
		},
		//第三行
		{
			{		{{'n','N',0xa3ee},{'o','O',0xa3ef},{'p','P',0xa3f0}},	   3,	  0 + STARTX,	41},
			{		{{'q','Q',0xa3f1},{'r','R',0xa3f2},{'s','S',0xa3f3}},	   3,     42+ STARTX,	41},
			{		{{'t','T',0xa3f4},{'u','U',0xa3f5},{'v','V',0xa3f6}},	   3,     84+ STARTX,	40},
			{		{{'w','W',0xa3f7},{'x','X',0xa3f8}},			   2,     125 + STARTX,	41},
			{		{{'y','Y',0xa3f9},{'z','Z',0xa3fa}},			   2,     167 + STARTX,	41},
//			{		{{0,VK_DELETE,0}},				   1,     210 + STARTX,	30},
			{		{{'\x08',VK_BACK,0}},								       1,     210 + STARTX,	30},
//			{		{{'\xd',VK_RETURN,0}},				1,    210 + STARTX,	30},
			{		{{0,NONEKEY,0}},					   0,	  0,			0},
		},
		//第四行
		{
			{		{{',',0,0xa3ac}},			1,    0+ STARTX,	28},
//			{		{{0,0,0xa1a3}},				1,	  29 + STARTX,	29},
			{		{{'.',0,0xa1a3}},			1,	  29 + STARTX,	29},
			{		{{'?',0,0xa3bf}},			1,    59 + STARTX,	29},
			{		{{' ',VK_SPACE,0}},			1,    89 + STARTX, 29},
//			{		{{0,PINYIN_KB,0}},			1,	  0 + STARTX,	38},
			{		{{0,FUHAO_KB,0}},			1,    119+ STARTX,	29},
			{		{{0,HANDWRITE_KB,0}},		1,    149 + STARTX, 29},
			{		{{0,YINGWEN_KB,0}},			1,    179+ STARTX,	29},
			{		{{'\xd',VK_RETURN,0}},		1,    209 + STARTX,	29},
			{		{{0,NONEKEY,0}},			0,	  0,			0},
		},
*/
};


static LPTSTR lpPYKBImageFileName[1][2] = {
	{
		"/kingmos/pyimage.bmp",
		"/kingmos/pyimageinv.bmp",
	},
};



//HBITMAP g_hPYBitmap = NULL;  // 当前键盘的位图句柄
static HBITMAP g_hPYBitmap = NULL;  // 当前键盘的位图句柄
static HBITMAP g_hInvertBitmap = NULL; // 当前按下位图的句柄
HBITMAP g_hDisableArrow = NULL;  // 当前键盘的位图句柄
//static HBITMAP g_hDisableArrow = NULL;  // 当前键盘的位图句柄

// 定义一个选择区域状态的结构
typedef struct SelAreaStatusStruct
{
	BOOL bLeftArrowEnable; // 左箭头是否有效
	BOOL bRightArrowEnable; // 右箭头是否有效	

	BOOL bLeftArrowPressed; // 左箭头按下
	BOOL bRightArrowPressed; // 右箭头按下
}SELAREASTATUS, *LPSELAREASTATUS;

// 以下的内容需要与窗口挂钩
typedef struct structPY_Keyboard{
	BOOL g_bSBCCase ;
	SPELLRESULT py_SpellResult; // 当前的拼音结果
	RECT py_HitRect;  // 当前按键的位置
	KEYENTRY *py_lpHitKey ; // 当前按下的键
	BOOL PY_KeyBroad_Shift;  // 当前的SHIFT的状态
	BOOL PY_KeyBroad_Capital;  // 当前的CAPS状态
	int g_iBitmapIndex ;   // 当前使用的位图序列号
	BOOL g_bCursorLeft ;  // 当前鼠标是否已经离开了开始按下的键
	SELAREASTATUS py_SelAreaStatus;
	HWND g_hSpell ;
}PY_KEYBOARD, *LPPY_KEYBOARD;

#if 0
static BOOL g_bSBCCase = FALSE;

static 	SPELLRESULT py_SpellResult; // 当前的拼音结果

static RECT py_HitRect={0,0,0,0};  // 当前按键的位置
static 	KEYENTRY *py_lpHitKey = NULL; // 当前按下的键
//static 	KEYENTRY *py_lpSelectKey = NULL;
static BOOL Eng_KeyBroad_Capital = FALSE;  // 当前的CAPS状态
//static BOOL py_KeyBroad_Shift = FALSE;  // 当前的SHIFT状态
static int g_iBitmapIndex = 0;   // 当前使用的位图序列号
static BOOL g_bCursorLeft = FALSE;  // 当前鼠标是否已经离开了开始按下的键

static SELAREASTATUS py_SelAreaStatus;
static HWND g_hSpell = NULL;
#endif

// *******************************************************
// 函数定义区
// *******************************************************
static int py_Who(HWND hWnd);
static void py_Create(HWND hWnd);
static void py_Destroy(HWND hWnd);
static void py_Resume(HWND hWnd);
static int py_DoPAINT( HWND, HDC );
static int py_DoLBUTTONDOWN( HWND, int x, int y );
static int py_DoLBUTTONUP( HWND, int x, int y );
static int py_DoMOUSEMOVE( HWND, int x, int y );
static BOOL py_GetKeyValue( HWND hWnd,WORD * lpKey );
static void py_GetKeyRect( int row, int col, LPRECT lprect );
static LRESULT py_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

static KEYENTRY *GetKey(int x,int y,RECT *lprect);
static void DrawSystemKey(HWND hWnd,HDC hdc,BYTE bVk);

static void ShowKeyboardrect(HWND hWnd,HDC hdc,RECT rect,DWORD dwShowStyle);
static BOOL GetVirtualKeyRect(BYTE bVk, RECT *lprect);

static void ShowSelectArea(HWND hWnd);
static void ClearSelectArea(HWND hWnd);
static int GetSelectIndex(HWND hWnd,int x,int y);
static BOOL SelectKey(HWND hWnd, KEYENTRY *lpKey,int iIndex);
static BOOL ValidData(HWND hWnd);
static void GoBackDeal(HWND hWnd);
static void InputKey(HWND hWnd,KEYENTRY *lpKey);

static void ClearSpellResult(HWND hWnd,LPSPELLRESULT lpSpellResult);
static void SetSelAreaStatus(HWND hWnd,LPSPELLRESULT lpSpellResult);
static void SelectInput(HWND hWnd , int iIndex);
void DrawArrow(HWND hWnd,HDC hdc,UINT iArrowType,UINT iArrowStatus);
static LRESULT DoSpellSelChange(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSpellNoData(HWND hWnd,WPARAM wParam,LPARAM lParam);

static BOOL LoadPYKeyboardImage(HWND hWnd, BOOL bReLoad);

extern void KB_DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop);
extern void StartMoveKeyboard(HWND hWnd , int x,int y);
extern void MoveKeyboard(HWND hWnd , int x,int y);
extern void EndMoveKeyboard(HWND hWnd);
extern BOOL KeyboardMoving(HWND hWnd);

// 定义英文键盘函数
const KEYDRV pyKeyboard = {
	py_Who,
	py_Create,
	py_Destroy,
	py_Resume,
	py_DoPAINT,
	py_DoLBUTTONDOWN,
	py_DoLBUTTONUP,
	py_DoMOUSEMOVE,
    py_GetKeyValue,
	py_DefWindowProc };


// **************************************************
// 声明：static int py_Who(HWND hWnd)
// 参数：
// 	无
// 返回值：返回当前键盘的标识。
// 功能描述：
// 引用: 
// **************************************************
static int py_Who(HWND hWnd)
{
	return 1;
}

// **************************************************
// 声明：static void py_Create(HWND hWnd)
// 参数：
// 	无
// 返回值：无
// 功能描述：创建键盘。
// 引用: 
// **************************************************
static void py_Create(HWND hWnd)
{
	int iStartx, iStarty;
	RECT rect;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)malloc(sizeof(PY_KEYBOARD));
		if (lpPY_Keyboard == NULL)
			return;
		memset(lpPY_Keyboard,0,sizeof(PY_KEYBOARD));

		SetWindowLong(hWnd,PY_PRIVATEDATA,(LONG)lpPY_Keyboard);

	py_Resume(hWnd); // 恢复键盘
	ClearSpellResult(hWnd,&lpPY_Keyboard->py_SpellResult);

	if (lpPY_Keyboard->g_hSpell == NULL)
	{
		HINSTANCE hInstance;

		hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		RegisterSpellAreaWindow(hInstance);
		// 还没有创建拼写窗口,创建它

		GetWindowRect(hWnd,&rect);


		/*
		iStartx = 0;  //开始 x 坐标
		iStarty = rect.top - SPELLWINDOW_HEIGHT; //开始 y 坐标
    hKey = CreateWindowEx(WS_EX_TOPMOST|WS_EX_INPUTWINDOW,  
						"Keyboard",
						"键盘",
						WS_POPUP,//|WS_VISIBLE,//|WS_CAPTION,
						0,
						0,
						10,
						10,
						hParent,
						(HMENU)101,
						(HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE),
						NULL);
*/
//		lpPY_Keyboard->g_hSpell = CreateWindowEx(WS_EX_TOPMOST|WS_EX_INPUTWINDOW,
/*		
		iStartx = 0;  //开始 x 坐标
		iStarty = rect.top - SPELLWINDOW_HEIGHT; //开始 y 坐标
	lpPY_Keyboard->g_hSpell = CreateWindowEx(WS_EX_INPUTWINDOW,
								classSpellArea, "", 
								WS_POPUP|WS_BORDER , 
								iStartx, iStarty, 
								SPELLWINDOW_WIDTH, 
								SPELLWINDOW_HEIGHT, 
								hWnd, (HMENU)ID_SPELL, 
								hInstance, hWnd);
*/		
		iStartx = 1;  //开始 x 坐标
		iStarty = 1; //开始 y 坐标
		lpPY_Keyboard->g_hSpell = CreateWindowEx(WS_EX_INPUTWINDOW,
								classSpellArea, "", 
								WS_CHILD|WS_VISIBLE , 
								iStartx, iStarty, 
								SPELLWINDOW_WIDTH, 
								SPELLWINDOW_HEIGHT, 
								hWnd, (HMENU)ID_SPELL, 
								hInstance, hWnd);
	}
}

// **************************************************
// 声明：static void py_Destroy(HWND hWnd)
// 参数：
// 	无
// 返回值：无
// 功能描述：破坏键盘。
// 引用: 
// **************************************************
static void py_Destroy(HWND hWnd)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

		py_Resume(hWnd);

		if (lpPY_Keyboard->g_hSpell)
		{
			DestroyWindow(lpPY_Keyboard->g_hSpell);
			lpPY_Keyboard->g_hSpell = NULL;
		}
		free(lpPY_Keyboard);
		SetWindowLong(hWnd,PY_PRIVATEDATA,0);
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
// 声明：static void py_Resume(HWND hWnd)
// 参数：
// 	无
// 返回值：无
// 功能描述：恢复键盘。
// 引用: 
// **************************************************
static void py_Resume(HWND hWnd)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;
//	py_Count = 0;
		// 清空选择区
		ClearSpellResult(hWnd,&lpPY_Keyboard->py_SpellResult); // 清除拼写结果中的内容
		SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // 设置选择区域

		// 清空拼写区
		if (lpPY_Keyboard->g_hSpell)
			SendMessage(lpPY_Keyboard->g_hSpell,SPM_CLEARSPELL,0,0);
		SetRect(&lpPY_Keyboard->py_HitRect,0,0,0,0);  // 设置键盘的按键状态为空
}

// **************************************************
// 声明：static LRESULT py_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
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
static LRESULT py_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case SPN_SELCHANGE:
			DoSpellSelChange(hWnd,wParam,lParam);
			break;
		case SPN_NODATA:
			DoSpellNoData(hWnd,wParam,lParam);
			break;
		default:
			return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

// **************************************************
// 声明：static KEYENTRY *GetKey(int x,int y,RECT *lprect)
// 参数：
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 	OUT lprect -- 返回按键的矩形
// 
// 返回值：返回当前按下的键的结构指针
// 功能描述：根据坐标位置得到按下的键的指针
// 引用: 
// **************************************************
static KEYENTRY *GetKey(int x,int y,RECT *lprect)
{
	int iRow=0,iColumn=0;

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
		// 得到当前按下的位置在键盘的指定行的那一列
		while(1)
		{
			if (g_pykeys[iRow][iColumn].iKeyNum == 0)  // 没有任何按键
				return NULL;

			if ( x < g_pykeys[iRow][iColumn].iLeft + g_pykeys[iRow][iColumn].iWidth) // 是否在指定的列
				break;
			
			iColumn ++;
		}
		// 已经找到指定的列
		if (lprect != NULL)
		{ // 得到指定按键的矩形
			lprect->left = g_pykeys[iRow][iColumn].iLeft;
			lprect->right = lprect->left + g_pykeys[iRow][iColumn].iWidth;
			lprect->top = g_RowHeightList[iRow];
			lprect->bottom = g_RowHeightList[iRow+1]-1;
		}
		return (KEYENTRY *)&g_pykeys[iRow][iColumn];  // 返回按键的指针
}

#define KEYEVENTF_KEYDOWN 0

// **************************************************
// 声明：static int py_DoLBUTTONDOWN( HWND hWnd, int x, int y )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 
// 返回值：无
// 功能描述：鼠标左键按下。
// 引用: 
// **************************************************
static int py_DoLBUTTONDOWN( HWND hWnd, int x, int y )
{
	RECT rect;
	HDC hdc;
	KEYENTRY *lpKey;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;

		if (IsRectEmpty(&lpPY_Keyboard->py_HitRect) == 0)
		{ // 不是空矩形
			hdc = GetDC( hWnd );
			ShowKeyboardrect(hWnd,hdc,lpPY_Keyboard->py_HitRect,NORMALSHOW); // 恢复原来按键的显示
			ReleaseDC( hWnd, hdc );
			SetRect(&lpPY_Keyboard->py_HitRect,0,0,0,0);  // 设置为空矩形
		}

//		if (py_lpSelectKey)
		if (ValidData(hWnd) == TRUE)
		{
			// 存在有效数据，要判断是否需要作出选择
			int iIndex;
				
				iIndex = GetSelectIndex(hWnd,x,y); // 得到当前点的选择区域的索引值

				if (iIndex != -1)
				{ 
					// 笔点到选择区有效区域
					SelectInput(hWnd , iIndex);  // 选择输入
					return TRUE;
				}
		}

		// 得到指定位置的键
		lpKey = GetKey(x,y,&rect);
		if (lpKey == NULL)
			return FALSE; // 没有键
		lpPY_Keyboard->py_HitRect = rect; // 设置按下键的矩形
		lpPY_Keyboard->py_lpHitKey = lpKey; // 设置按下的键
		hdc = GetDC( hWnd );
		//InvertRect(hdc,&rect);
		ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW);  // 显示当前按键为按下状态
		ReleaseDC( hWnd, hdc );
		lpPY_Keyboard->g_bCursorLeft = FALSE; // 没有离开当前的按键

		if (lpKey->iKeyNum == 1)
		{

			if (lpKey->keyValue[0].bVk >= 'A' && lpKey->keyValue[0].bVk <= 'Z')
			{
						InputKey(hWnd,lpKey);
			}
			else if (lpKey->keyValue[0].bVk == PINYIN_KB)
			{ // 需要切换到拼音键盘
				py_Resume(hWnd);
				// 当前已经是拼音键盘
			}
			else if (lpKey->keyValue[0].bVk == YINGWEN_KB)
			{ // 需要切换到英文键盘
				py_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_ENGLISH,0); // 切换到英文键盘
			}
			else if (lpKey->keyValue[0].bVk == FUHAO_KB)
			{ // 需要切换到符号键盘
				py_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_SYMBOL,0); // 切换到符号键盘
			}
			else if (lpKey->keyValue[0].bVk == HANDWRITE_KB)
			{ // 需要切换到手写键盘
				py_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_HANDWRITE,0); // 切换到手写键盘
			}
			else if (lpKey->keyValue[0].bVk == QUANJIAO)
			{ // 需要切换到全角键盘
				if (lpPY_Keyboard->g_bSBCCase == FALSE)
					lpPY_Keyboard->g_bSBCCase = TRUE;
				else
				{
					lpPY_Keyboard->g_bSBCCase = FALSE;
					hdc = GetDC( hWnd );
					DrawSystemKey(hWnd,hdc,QUANJIAO);  // 恢复全角键的状态
					ReleaseDC( hWnd, hdc );
				}
		
				py_Resume(hWnd);
			}
			else
			{ // 是一般键
				if (ValidData(hWnd) == TRUE)
				{
					// 在选择框中有有效数据，只有VK_BACK有效
					if (lpKey->keyValue[0].bVk == VK_BACK)
					{
						// 要求退回到上一级功能
						GoBackDeal(hWnd);
					}
				}
				else
				{ // 在选择筐中没有任何有效数据，直接输出字符
					SelectKey(hWnd,lpKey,0); // 只有一个键,直接选择第一个键输出
				}
			}
		}
		else
		{
			// 当前的键下有多个键值，需要选择  , 只有是英文字符才有多键
			//InsertKeyToSpell(&lpPY_Keyboard->py_SpellResult,lpKey);
			InputKey(hWnd,lpKey);
		}
		return 0;
}

// **************************************************
// 声明：static int py_DoLBUTTONUP( HWND hWnd, int x, int y )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 
// 返回值：无效
// 功能描述：处理鼠标左键弹起。
// 引用: 
// **************************************************
static int py_DoLBUTTONUP( HWND hWnd, int x, int y )
{
	HDC hdc;
	RECT rect;

	KEYENTRY *lpKey;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;

		if (KeyboardMoving(hWnd) == TRUE)
		{  // 键盘在移动状态
			EndMoveKeyboard(hWnd); // 结束键盘移动
			return TRUE; // 返回
		}
		if (lpPY_Keyboard->g_bCursorLeft ==TRUE) // 鼠标已经离开按下的键，不要做任何处理
			return TRUE;
		if (lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed)
		{
			//左箭头被按下
			lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed = FALSE; // 恢复左箭头
//			RETAILMSG(1,(TEXT("Left Arrow Up \r\n")));
			hdc = GetDC( hWnd );
			DrawArrow(hWnd,hdc,LEFTARROW,0); // 画箭头
			ReleaseDC( hWnd, hdc );
			return 0;
		}
		if (lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed)
		{
			//右箭头被按下
			lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed = FALSE; // 恢复右箭头
//			RETAILMSG(1,(TEXT("Right Arrow Up \r\n")));
			hdc = GetDC( hWnd );
			DrawArrow(hWnd,hdc,RIGHTARROW,0); // 画箭头
			ReleaseDC( hWnd, hdc );
			return 0;
		}


		// 得到指定位置的键
		lpKey = GetKey(x,y,&rect);  // 得到键值
		if (lpKey == NULL)
			return FALSE; // 没有任何键
		if (lpPY_Keyboard->py_lpHitKey !=NULL)
		{  // 按下的键不为空
			if (lpKey->keyValue[0].bVk == QUANJIAO)
			{
				return 0;  // 全角键，不要做任何处理
			}
			hdc = GetDC( hWnd );
//			InvertRect(hdc,&lpPY_Keyboard->py_HitRect);
//			ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW);
			ShowKeyboardrect(hWnd,hdc,lpPY_Keyboard->py_HitRect,NORMALSHOW); // 恢复按下的键
			ReleaseDC( hWnd, hdc );
			SetRect(&lpPY_Keyboard->py_HitRect,0,0,0,0);  // 设置按下的键矩形为空
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

		lpPY_Keyboard->py_lpHitKey = NULL;    // 已经没有按下的键
		return 0;
}

// **************************************************
// 声明：static int py_DoMOUSEMOVE( HWND hWnd, int x, int y )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN x -- X坐标
// 	IN y -- Y坐标
// 
// 返回值：无效
// 功能描述：处理按着鼠标左键移动。
// 引用: 
// **************************************************
static int py_DoMOUSEMOVE( HWND hWnd, int x, int y )
{
	KEYENTRY *lpKey;
	RECT rect;
	HDC hdc;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;

		if (KeyboardMoving(hWnd) == TRUE)
		{  // 键盘在移动状态
			MoveKeyboard(hWnd,x,y);  // 移动键盘
			return TRUE;
		}
		if (lpPY_Keyboard->py_lpHitKey == NULL)
			return FALSE; // 没有按下的键
		// 得到指定位置的键
		lpKey = GetKey(x,y,&rect);
		if (lpKey != lpPY_Keyboard->py_lpHitKey)
		{  // 当前的键与按下的键不同
			if (lpPY_Keyboard->g_bCursorLeft == TRUE)  // 已经离开了按下的键
				return TRUE; // 不需要处理
			if (lpPY_Keyboard->py_lpHitKey->keyValue[0].bVk != QUANJIAO)
			{  // 按下的键不是全角键
//				InvertRect(hdc,&lpPY_Keyboard->py_HitRect);
				hdc = GetDC( hWnd );
				ShowKeyboardrect(hWnd,hdc,lpPY_Keyboard->py_HitRect,NORMALSHOW); // 恢复按下键的状态
				ReleaseDC( hWnd, hdc );
			}
			lpPY_Keyboard->g_bCursorLeft = TRUE; // 设置键已经离开原来按下的键
		}
		else
		{ // 当前的键与按下的键相同
			if (lpPY_Keyboard->g_bCursorLeft == FALSE) // 还没有离开按下的键
				return TRUE;
			// 鼠标离开原来按下的键后又回到原来的按键位置
			if (lpPY_Keyboard->py_lpHitKey->keyValue[0].bVk != QUANJIAO)
			{  // 按下的键不是全角键
//				InvertRect(hdc,&lpPY_Keyboard->py_HitRect);
				hdc = GetDC( hWnd );
				ShowKeyboardrect(hWnd,hdc,lpPY_Keyboard->py_HitRect,INVERTSHOW); // 显示键为按下的状态
				ReleaseDC( hWnd, hdc );	
			}
			lpPY_Keyboard->g_bCursorLeft = FALSE; // 设置键在原来按下的位置
		}
		return 0;
}

// **************************************************
// 声明：static int py_DoPAINT( HWND hWnd, HDC hdc )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值： 无
// 功能描述：绘制英文键盘。
// 引用: 
// **************************************************
static int py_DoPAINT( HWND hWnd, HDC hdc )
{
  RECT rect={0,0,KBDBMPWIDTH,KBDBMPHEIGHT};
  int iStarty;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;
		iStarty = lpPY_Keyboard->g_iBitmapIndex*KBDBMPHEIGHT ;
//  HBITMAP hBitMap;

/*	  if (g_hPYBitmap == NULL)
	  {  // 还没有加载图象句柄
		  HINSTANCE hInstance;

//		  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		  hInstance = g_hInstance;
		  //g_hPYBitmap = LoadImage( 0, "\\System\\Keyboard1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_NORMALKB), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载英文键盘位图
//		  g_hPYBitmap = LoadImage( hInstance, "\\KINGMOS\\PINYINKB.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载英文键盘位图
		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_PINYIN), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载手写键盘位图
		  if( g_hPYBitmap == 0 )
			return 0;  // 装载失败
	  }
*/
	  LoadPYKeyboardImage(hWnd,FALSE);
	  KB_DrawBitMap(hdc,(HBITMAP)g_hPYBitmap,rect,0,iStarty,SRCCOPY); // 绘制键盘
//	  DrawCaseState(hdc ,py_KeyBroad_Case);
//	  if (!IsRectEmpty(&lpPY_Keyboard->py_HitRect))
//		 InvertRect(hdc,&lpPY_Keyboard->py_HitRect);
//	  DeleteObject(hBitMap);
//	  DrawSystemKey(hWnd,hdc,VK_SHIFT); // 绘制SHIFT键状态
//	  DrawSystemKey(hWnd,hdc,VK_CAPITAL); // 绘制CAPS键状态
	  DrawSystemKey(hWnd,hdc,PINYIN_KB);  // 绘制YINGWEN_KB键的状态
//	  DrawSystemKey(hWnd,hdc,QUANJIAO);  // 绘制QIANJIAO键的状态
	  ClearSelectArea(hWnd);
	  ShowSelectArea(hWnd); // 显示选择区域
	  return 0;
}
// **************************************************
// 声明：static BOOL py_GetKeyValue(HWND hWnd, WORD * lpKey )
// 参数：
// 	OUT lpKey -- 得到当前按键的值
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到当前按键的值。
// 引用: 
// **************************************************
static BOOL py_GetKeyValue(HWND hWnd, WORD * lpKey )
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;
		if( lpPY_Keyboard->py_lpHitKey )
		{  // 有键按下
			*lpKey = lpPY_Keyboard->py_lpHitKey->keyValue[0].bChar; // 返回当前按键的值
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
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

		GetVirtualKeyRect(bVk,&rect);
		if (bVk == QUANJIAO)
		{
			if (lpPY_Keyboard->g_bSBCCase == TRUE)
				ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // 显示键为按下的状态
			else
				ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW); // 显示键为按下的状态
		}
		if (bVk == PINYIN_KB)
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

/*	  if (g_hPYBitmap == NULL)
	  {  // 还没有加载图象句柄
		  HINSTANCE hInstance;

//		  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		  hInstance = g_hInstance;
		  //g_hPYBitmap = LoadImage( 0, "\\System\\Keyboard1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_NORMALKB), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载英文键盘位图
		  g_hPYBitmap = LoadImage( hInstance, "\\KINGMOS\\PINYINKB.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载英文键盘位图
		  if( g_hPYBitmap == 0 )
			return ;  // 装载失败
	  }
*/
/*	if (g_hInvertBitmap == NULL)
	{ // 没有装载选择位图
		  //g_hInvertBitmap = LoadImage( 0, "\\System\\Keyboard2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  g_hInvertBitmap = LoadImage( (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE(IDB_pySEL), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;// 装载位图句柄
		  if( g_hInvertBitmap == 0 )
			return ;
	}
*/
    LoadPYKeyboardImage(hWnd,FALSE);
	if (dwShowStyle == NORMALSHOW)  // 通常显示
		KB_DrawBitMap(hdc,(HBITMAP)g_hPYBitmap,rect,rect.left + 0,rect.top,SRCCOPY);
	else // 选择（反白）显示
		KB_DrawBitMap(hdc,(HBITMAP)g_hInvertBitmap,rect,rect.left + 0,rect.top,SRCCOPY);
}


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
	int iDrawNum,iDrawIndex;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

		if (ValidData(hWnd) == FALSE)
			return ; // 没有需要选择的键
		hdc = GetDC(hWnd); // 得到设备句柄

		rect.top = g_RowHeightList[0];  // 得到选择区域的垂直位置
		rect.bottom = g_RowHeightList[1] - 1;

		// 已经拼写出结果，显示结果
		iDrawIndex = lpPY_Keyboard->py_SpellResult.iCurPage * SELECT_NUM; // 得到开始显示的索引
		if ((iDrawIndex + SELECT_NUM) > lpPY_Keyboard->py_SpellResult.iSpellResultNum)
			iDrawNum = lpPY_Keyboard->py_SpellResult.iSpellResultNum; // 当前不是满屏
		else
			iDrawNum = SELECT_NUM;  // 当前是满屏

		SetTextColor(hdc,RGB(0,0,0));
//		SetBkColor(hdc,RGB(231,242,255));
		SetBkColor(hdc,RGB(255,255,255));

		for (i = 0 ; i < iDrawNum ; i ++)
		{
			rect.left = g_ColumnWidthList[i];
			rect.right = g_ColumnWidthList[i + 1] - 2;
			DrawText(hdc, (char *)&lpPY_Keyboard->py_SpellResult.lpSpellResult[i + iDrawIndex],2,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);  // 2 -- 一个汉字
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
//	HPEN hPen;
//	HBRUSH hBrush;
	// 键盘一共有六个选择区域

		if (g_hPYBitmap)
		{
			hdc = GetDC(hWnd); // 得到设备句柄

	//		rect.top = 1;  // 得到选择区域的垂直位置
	//		rect.bottom = 30;
			rect.left = g_ColumnWidthList[0]; 
			rect.right = g_ColumnWidthList[SELECT_NUM]; 
			rect.top = g_RowHeightList[0];  // 得到选择区域的垂直位置
			rect.bottom = g_RowHeightList[1] -1 ;
		
			KB_DrawBitMap(hdc,(HBITMAP)g_hPYBitmap,rect,rect.left,rect.top,SRCCOPY); // 绘制键盘
			// 画左箭头
			DrawArrow(hWnd,hdc,LEFTARROW,NORMALSHOW);
			// 画右箭头
			DrawArrow(hWnd,hdc,RIGHTARROW,NORMALSHOW);
				
			ReleaseDC(hWnd,hdc);
		}
		return ;
/*
		hBrush = CreateSolidBrush(RGB(231,242,255));
//		FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH)); // 清除选择区域
		FillRect(hdc,&rect,hBrush); // 清除选择区域
		DeleteObject(hBrush);
	
		// 画左箭头
		DrawArrow(hWnd,hdc,LEFTARROW,NORMALSHOW);
		// 画右箭头
		DrawArrow(hWnd,hdc,RIGHTARROW,NORMALSHOW);

		// 画每一个区域的分隔线
		hPen = CreatePen(PS_DASH,1,RGB(0,0,0));  // 创建一个虚点的笔
		hPen = (HPEN)SelectObject(hdc,hPen);  // 选择笔到设备

		for ( i = 1; i < SELECT_NUM ; i++)
		{
			MoveToEx(hdc,g_ColumnWidthList[i],rect.top,NULL); 
			LineTo(hdc,g_ColumnWidthList[i],rect.bottom);  // 绘制线
		}

		hPen = (HPEN)SelectObject(hdc,hPen);  // 恢复原来笔
		DeleteObject(hPen); // 删除创建的笔

		ReleaseDC(hWnd,hdc);  // 释放设备句柄
*/
}


// **************************************************
// 声明：static int GetSelectIndex(HWND hWnd,int x,int y)
// 参数：
// 	IN x -- x 坐标
// 	IN y -- y 坐标
// 
// 返回值：返回指定坐标所在的索引
// 功能描述：得到当前点的选择区域的索引值。
// 引用: 
// **************************************************
static int GetSelectIndex(HWND hWnd,int x,int y)
{
	int iIndex = 0;
	int i;
	POINT pt;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return -1;

		pt.x = x;
		pt.y = y;
//		if ( x < LEFTARROWEND)
		if (PtInRect(&g_rectLeftArrow,pt))
		{
			// 笔点到左箭头
			if (lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable)  // 左箭头有效
				return LEFTARROW;
			return -1; // 左箭头无效
		}
//		else if ( x > RIGHTARROWSTART)
		else if (PtInRect(&g_rectRightArrow,pt))
		{
			// 笔点到右箭头
			if (lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable) // 右箭头有效
				return RIGHTARROW;
			return -1;  // 右箭头无效
		}
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
	
	if (lpPY_Keyboard->g_bSBCCase == FALSE)
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
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;
		if (lpKey == NULL)
			return FALSE; // 没有选择键 
		if (iIndex < 0 || iIndex >= lpKey->iKeyNum)
		{
			// 错误的索引
			return FALSE;
		}
		
		if (lpPY_Keyboard->g_bSBCCase == FALSE)
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
// 声明：static BOOL GetVirtualKeyRect(BYTE bVk, RECT *lprect)
// 参数：
// 	IN bVk -- 指定虚键
// 	OUT lprect -- 返回该虚键的位置
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到指定虚键的位置。
// 引用: 
// **************************************************
static BOOL GetVirtualKeyRect(BYTE bVk, RECT *lprect)
{
	int iRow=0,iColumn=0;
	int i;

		for (iRow = 0; iRow < ROW_NUM; iRow ++)
		{ // 得到虚键是属于哪一行
			iColumn=0;
			while(1)
			{ // 得到虚键的键的位置

				for ( i = 0; i< g_pykeys[iRow][iColumn].iKeyNum; i++)
				{ // 每一个键上可能有多个键值
					if ( bVk == g_pykeys[iRow][iColumn].keyValue[0].bVk)
					{ // 当前的键就是虚键
						if (lprect != NULL)
						{ // 指定的指针有效，得到键的位置
							lprect->left = g_pykeys[iRow][iColumn].iLeft;
							lprect->right = lprect->left + g_pykeys[iRow][iColumn].iWidth;
							lprect->top = g_RowHeightList[iRow];
							lprect->bottom = g_RowHeightList[iRow+1]-1;
						}
						return TRUE;
					}
				}
				
				iColumn ++;
				if (g_pykeys[iRow][iColumn].iKeyNum == 0)
					break; // 已经到最后一个键
			}
		}
		return FALSE; // 没有找到指定的键
}


// **************************************************
// 声明：static void ClearSpellResult(HWND hWnd,LPSPELLRESULT lpSpellResult)
// 参数：
// 	IN/OUT lpSpellResult -- 需要清除的拼音结果结构
// 
// 返回值：无
// 功能描述：清理拼音结果结构。
// 引用: 
// **************************************************
static void ClearSpellResult(HWND hWnd,LPSPELLRESULT lpSpellResult)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

		// 以下内容为选择了一个拼写后有效

		memset(lpSpellResult->lpSpellResult , 0 ,MAX_PYRESULT);
		lpSpellResult->iSelectIndex = 0;
		lpSpellResult->iSpellResultNum = 0;  // 没有拼写结果
		lpSpellResult->iCurPage = 0;

		lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable = FALSE;  // 左箭头无效
		lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = FALSE; // 右箭头无效
		lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed = FALSE; // 左箭头按下
		lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed = FALSE; // 右箭头按下

		// 画左箭头
	//	DrawArrow(hWnd,hdc,LEFTARROW,NORMALSHOW);
		// 画右箭头
	//	DrawArrow(hWnd,hdc,RIGHTARROW,NORMALSHOW);
				
	//	RETAILMSG(1,(TEXT("Clear Arrow Status \r\n")));

}


// **************************************************
// 声明：static void SetSelAreaStatus(HWND hWnd,LPSPELLRESULT lpSpellResult)
// 参数：
//	IN lpSpellResult -- 当前的拼写结果
// 返回值：无
// 功能描述：根据当前的拼写结果设置选择区域的状态。
// 说明:
//     1，在拼写状态 （还没有拼写出结果）
//			a.没有任何拼写，不需要箭头
//			b.字符小于4个，当拼音数目大于6个需要箭头
//			c.字符为4个，	当拼音数目大于4个需要箭头
//			d.字符大于4个，	当拼音数目大于3个需要箭头
//     2，在拼写结果状态，当拼写结果大于6个时需要箭头
// 引用: 
// **************************************************
static void SetSelAreaStatus(HWND hWnd,LPSPELLRESULT lpSpellResult)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;
		// 已经拼写出结果
		 if (lpSpellResult->iSpellResultNum > SELECT_NUM)
		 {
			if (lpSpellResult->iCurPage == 0)
			{
				lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable = FALSE;  // 左箭头无效
				lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = TRUE; // 右箭头有效
			}
			else
			{
				lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable = TRUE;  // 左箭头有效
				
				if (lpSpellResult->iSpellResultNum > ((lpSpellResult->iCurPage + 1) * SELECT_NUM))   // SELECT_NUM -- 有箭头后一屏只能显示5的区域
					lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = TRUE; // 还有超过一屏的数据，右箭头有效
				else
					lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = FALSE; // 没有一屏的数据，右箭头无效
			}
		 }
		 else
		 {
				lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable = FALSE;  // 左箭头无效
				lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = FALSE; // 右箭头无效
		 }
}

// **************************************************
// 声明：static void SetSelAreaStatus(HWND hWnd,LPSPELLRESULT lpSpellResult)
// 参数：
//	IN hWnd -- 窗口句柄
//  IN iIndex -- 选择索引
// 返回值：无
// 功能描述：选择输入。
// 引用: 
// **************************************************
static void SelectInput(HWND hWnd , int iIndex)
{
	int iSelectIndex;
	HDC hdc ;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

//		RETAILMSG(1,(TEXT(" Enter SelectInput\r\n")));
		// 已经拼写出结果
		if (iIndex == LEFTARROW)
		{
			// 左箭头
			if (lpPY_Keyboard->py_SpellResult.iCurPage > 0)
			{
				lpPY_Keyboard->py_SpellResult.iCurPage --; // 翻到前一页
				lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed = TRUE;
//				RETAILMSG(1,(TEXT("Left Arrow Down \r\n")));
				hdc = GetDC( hWnd );
				DrawArrow(hWnd,hdc,LEFTARROW,0); // 画箭头
				ReleaseDC( hWnd, hdc );
				SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // 设置选择区域
				ClearSelectArea(hWnd); // 清除选择区域
				ShowSelectArea(hWnd); // 显示选择区域
			}
			return ;
		}
		else if (iIndex == RIGHTARROW)
		{
			// 右箭头
			if (((lpPY_Keyboard->py_SpellResult.iCurPage + 1) * SELECT_NUM) < lpPY_Keyboard->py_SpellResult.iSpellResultNum)
			{  // 还可以往后翻
				lpPY_Keyboard->py_SpellResult.iCurPage ++; // 翻到后一页
				lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed = TRUE;
//				RETAILMSG(1,(TEXT("Right Arrow Down \r\n")));
				hdc = GetDC( hWnd );
				DrawArrow(hWnd,hdc,RIGHTARROW,0); // 画箭头
				ReleaseDC( hWnd, hdc );
				SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // 设置选择区域
				ClearSelectArea(hWnd); // 清除选择区域
				ShowSelectArea(hWnd); // 显示选择区域
			}
			return ;
		}
		// 点到选择区
//		RETAILMSG(1,(TEXT(" Enter Select a Word\r\n")));
		iSelectIndex = lpPY_Keyboard->py_SpellResult.iCurPage * SELECT_NUM + iIndex;

		if (iSelectIndex <lpPY_Keyboard->py_SpellResult.iSpellResultNum)
		{
			// 点到有结果的区域，开始输出结果
			BYTE bChar;
			WORD wOutput;

			wOutput = lpPY_Keyboard->py_SpellResult.lpSpellResult[iSelectIndex]; //  得到要输出的汉字
			bChar = (BYTE)wOutput; // 后输出低位
			keybd_event(bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息

			bChar = (BYTE)(wOutput >> 8); // 先输出高位
			keybd_event(bChar,0,KEYEVENTF_CHAR,0); // 发送一个字符消息

//			RETAILMSG(1,(TEXT(" Had Output a Word\r\n")));

			// 清空选择区
			ClearSpellResult(hWnd,&lpPY_Keyboard->py_SpellResult); // 清除拼写结果中的内容
//			RETAILMSG(1,(TEXT(" ClearSpellResult OK !!!\r\n")));
			SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // 设置选择区域
//			RETAILMSG(1,(TEXT(" SetSelAreaStatus OK !!!\r\n")));

			// 清空拼写区
			SendMessage(lpPY_Keyboard->g_hSpell,SPM_CLEARSPELL,0,0);
//			RETAILMSG(1,(TEXT(" SPM_CLEARSPELL OK !!!\r\n")));

			// 联想功能
			lpPY_Keyboard->py_SpellResult.iSpellResultNum = DoPYLXSearch(wOutput,lpPY_Keyboard->py_SpellResult.lpSpellResult,MAX_PYRESULT);
//			RETAILMSG(1,(TEXT(" Get LX OK !!!\r\n")));
			
			SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // 设置选择区域
//			RETAILMSG(1,(TEXT(" SetSelAreaStatus OK !!!\r\n")));
			ClearSelectArea(hWnd); // 清除选择区域
//			RETAILMSG(1,(TEXT(" ClearSelectArea OK !!!\r\n")));
			ShowSelectArea(hWnd); // 显示选择区域
//			RETAILMSG(1,(TEXT(" ShowSelectArea OK !!!\r\n")));


		}
		return;
}

// **************************************************
// 声明：static BOOL ValidData(HWND hWnd)
// 参数：
// 
// 返回值：当前拼写结果中有有效数据，返回TRUE，否则返回FALSE
// 功能描述：检查当前的拼写结果中是否有有效数据。
// 引用: 
// **************************************************
static BOOL ValidData(HWND hWnd)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;
	
		if (lpPY_Keyboard->py_SpellResult.iSpellResultNum > 0)
		{
			// 在拼音结果中有有效数据， 可能是联想数据
			return TRUE;
		}

		return FALSE; // 没有有效数据
}


// **************************************************
// 声明：static void GoBackDeal(HWND hWnd)
// 参数：
//	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：回到上一级操作。
// 引用: 
// **************************************************
static void GoBackDeal(HWND hWnd)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;
		SendMessage(lpPY_Keyboard->g_hSpell,SPM_DELETEKEY,0,0);
}


// **************************************************
// 声明：void DrawArrow(HWND hWnd,HDC hdc,UINT iArrowType,UINT iArrowStatus)
// 参数：
//	IN hWnd -- 窗口句柄
//	IN hdc -- 设备句柄
//  IN iArrowType -- 要绘制的箭头的类型，LEFTARROW 或者 RIGHTARROW
//  IN iArrowStatus -- 要绘制的箭头的状态， INVERTSHOW 或者 NORMALSHOW
// 返回值：无
// 功能描述：绘制箭头。
// 引用: 
// **************************************************
void DrawArrow(HWND hWnd,HDC hdc,UINT iArrowType,UINT iArrowStatus)
{
//	RECT rect;
//	COLORREF cl_Text,cl_Bk;
//	HPEN hPen;
//	HBRUSH hBrush;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

		return ;

	if (iArrowType == LEFTARROW)
	{  // 是左箭头
		// 得到左箭头的矩形位置
//		rect.top = 1-1;  // 得到选择区域的垂直位置
//		rect.bottom = 30+1;
//		rect.left = LEFTARROWSTART-1; 
//		rect.right = LEFTARROWEND+1; 
		if (lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable == FALSE)
		{
			// 左箭头是DISABLE，
//			cl_Text = CL_LIGHTGRAY; // 文本是浅灰色
//			cl_Bk = RGB(0XC0,0XC0,0XC0); // 底色是灰色
			if (g_hDisableArrow == NULL)
			{
//				HINSTANCE hInstance;

//				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				g_hDisableArrow = LoadImage( g_hInstance, MAKEINTRESOURCE(IDB_ARROWDIS), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载手写键盘位图
			}
			KB_DrawBitMap(hdc,(HBITMAP)g_hDisableArrow,g_rectLeftArrow,1,g_rectLeftArrow.top,SRCCOPY);
		}
		else
		{
			// 左箭头是ENABLE
			if (lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed)
			{  // 左箭头按下
//				cl_Text = CL_BLACK;  // 白底黑字
//				cl_Bk = CL_WHITE;
				ShowKeyboardrect(hWnd,hdc,g_rectLeftArrow,INVERTSHOW); // 恢复原来按键的显示

			}
			else
			{
//				cl_Text = CL_BLACK;  // 灰底黑字
//				cl_Bk = RGB(0XC0,0XC0,0XC0); // 底色是灰色
				ShowKeyboardrect(hWnd,hdc,g_rectLeftArrow,NORMALSHOW); // 恢复原来按键的显示
			}
		}
/*
//		hPen = CreatePen(PS_SOLID,1,cl_Text);
		hBrush = CreateSolidBrush(cl_Bk); // 创建背景刷

//		hPen = SelectObject(hdc,hPen);
		hBrush = SelectObject(hdc,hBrush); // 选择背景刷
		SetTextColor(hdc,cl_Text); // 设置文本颜色
		SetBkColor(hdc,cl_Bk); // 设置背景颜色

		Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom); // 绘制边框与底色
		InflateRect(&rect,-2,-2); // 文本矩形比边框小2点
		DrawText(hdc,"<",1,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 绘制左箭头

//		hPen = SelectObject(hdc,hPen);
		hBrush = SelectObject(hdc,hBrush); // 恢复背景刷

//		DeleteObject(hPen);
		DeleteObject(hBrush); // 删除刷子
*/
	}
	else
	{
		// 得到右箭头的矩形位置
//		rect.top = 1-1;  // 得到选择区域的垂直位置
//		rect.bottom = 30+1;
//		rect.left = RIGHTARROWSTART-1; 
//		rect.right = RIGHTARROWEND+1; 
		if (lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable == FALSE)
		{
			// 右箭头是DISABLE，
//			cl_Text = CL_LIGHTGRAY; // 文本是浅灰色
//			cl_Bk = RGB(0XC0,0XC0,0XC0); // 底色是灰色
			if (g_hDisableArrow == NULL)
			{
//				HINSTANCE hInstance;

//				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				g_hDisableArrow = LoadImage( g_hInstance, MAKEINTRESOURCE(IDB_ARROWDIS), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载手写键盘位图
			}
			KB_DrawBitMap(hdc,(HBITMAP)g_hDisableArrow,g_rectRightArrow,1,g_rectRightArrow.top,SRCCOPY);
		}
		else
		{
			// 右箭头是ENABLE
			if (lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed)
			{  // 右箭头按下
//				cl_Text = CL_BLACK;  // 白底黑字
//				cl_Bk = CL_WHITE;
				ShowKeyboardrect(hWnd,hdc,g_rectRightArrow,INVERTSHOW); // 恢复原来按键的显示
			}
			else
			{
//				cl_Text = CL_BLACK;  // 灰底黑字
//				cl_Bk = RGB(0XC0,0XC0,0XC0); // 底色是灰色
				ShowKeyboardrect(hWnd,hdc,g_rectRightArrow,NORMALSHOW); // 恢复原来按键的显示
			}
		}
/*
//		hPen = CreatePen(PS_SOLID,1,cl_Text);
		hBrush = CreateSolidBrush(cl_Bk); // 创建背景刷

//		hPen = SelectObject(hdc,hPen);
		hBrush = SelectObject(hdc,hBrush); // 选择背景刷
		SetTextColor(hdc,cl_Text); // 设置文本颜色
		SetBkColor(hdc,cl_Bk); // 设置背景颜色

		Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom); // 绘制边框与底色
		InflateRect(&rect,-2,-2); // 文本矩形比边框小2点
		DrawText(hdc,">",1,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 绘制左箭头

//		hPen = SelectObject(hdc,hPen);
		hBrush = SelectObject(hdc,hBrush); // 恢复背景刷

//		DeleteObject(hPen);
		DeleteObject(hBrush); // 删除刷子
*/
	}

}
// **************************************************
// 声明：static void InputKey(HWND hWnd,KEYENTRY *lpKey)
// 参数：
//	IN hWnd -- 窗口句柄
//  IN lpKey --输入的键
// 返回值：无
// 功能描述：绘制箭头。
// 引用: 
// **************************************************
static void InputKey(HWND hWnd,KEYENTRY *lpKey)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;
		PostMessage(lpPY_Keyboard->g_hSpell,SPM_INSERTKEY,0,(LPARAM)lpKey); // 插入一个键到SPELL窗口
		return;
}


// ********************************************************************
// 声明：static LRESULT DoSpellSelChange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam -- 改变后的选择索引
//  IN lParam -- 保留
// 返回值：
//	
// 功能描述：拼写区的选择发生改变
// 引用: 
// ********************************************************************
static LRESULT DoSpellSelChange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	TCHAR lpSpell[MAX_SPELLNUM];
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return 0;

		if (SendMessage(lpPY_Keyboard->g_hSpell,SPM_GETSELSPELL,MAX_SPELLNUM,(LPARAM)lpSpell) == TRUE)
		{
			// 成功得到拼写字符，现在开始进行拼写，得到汉字
			memset( lpPY_Keyboard->py_SpellResult.lpSpellResult , 0 , MAX_PYRESULT * sizeof(WORD) ); // 清空结果缓存
			lpPY_Keyboard->py_SpellResult.iSpellResultNum = PYSpell(lpSpell,lpPY_Keyboard->py_SpellResult.lpSpellResult,MAX_PYRESULT);
			
			lpPY_Keyboard->py_SpellResult.iCurPage = 0;  // 设置到第一页
			SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // 设置选择区域
			ClearSelectArea(hWnd); // 清除选择区域
			ShowSelectArea(hWnd); // 显示选择区域

		}
		return 0;
}



// ********************************************************************
// 声明：static LRESULT DoSpellNoData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN wParam -- 保留
//  IN lParam -- 保留
// 返回值：
//	
// 功能描述：拼写窗口已经没有数据
// 引用: 
// ********************************************************************
static LRESULT DoSpellNoData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return 0;

		lpPY_Keyboard->py_SpellResult.iSpellResultNum = 0;  // 设置拼写结果为0

		SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // 设置选择区域
		ClearSelectArea(hWnd); // 清除选择区域
		return 0;
}

// **************************************************
// 声明：static BOOL LoadPYKeyboardImage(HWND hWnd, BOOL bReLoad)
// 参数：
//	IN hWnd -- 窗口句柄
//	IN bReLoad -- 是否需要重新装载 
// 返回值：成功状态返回TRUE，否则返回FALSE
// 功能描述：装载当前的键盘图象。
// 引用: 
// **************************************************
static BOOL LoadPYKeyboardImage(HWND hWnd, BOOL bReLoad)
{
	int iCurStyle;
	int iStyleIndex = 0;
//	int iKeyImageIndex,iKeyInvImageIndex;
	HINSTANCE hInstance;


		if (bReLoad == FALSE)
		{
			if (g_hPYBitmap && g_hInvertBitmap)
			{
				return TRUE;
			}
		}

		hInstance = g_hInstance;


		iCurStyle = GetSystemStyle();

		if (iCurStyle == 0)
			iStyleIndex = 0;
		else
			iStyleIndex = 0;

		if (bReLoad == TRUE)
		{
			if (g_hPYBitmap)
			{
				DeleteObject(g_hPYBitmap);
				g_hPYBitmap = NULL;
			}
			if (g_hInvertBitmap)
			{
				DeleteObject(g_hInvertBitmap);
				g_hInvertBitmap = NULL;
			}
		}

		if (g_hPYBitmap == NULL)
		{  // 还没有加载图象句柄
		  g_hPYBitmap = LoadImage( 0, lpPYKBImageFileName[iStyleIndex][0], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hPYBitmap == 0 )
			return FALSE;  // 装载失败
		}
		if (g_hInvertBitmap == NULL)
		{  // 还没有加载图象句柄
		  g_hInvertBitmap = LoadImage( 0, lpPYKBImageFileName[iStyleIndex][1], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hInvertBitmap == 0 )
			return FALSE;  // 装载失败
		}

		return TRUE;
}
