/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����Ӣ�ļ���
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-05-19
���ߣ��½��� Jami chen
�޸ļ�¼��
**************************************************/
//#include <eframe.h>
#include "ewindows.h"
#include <thirdpart.h>
#include "resource.h"
#include <softkey.h>
#include <showstyle.h>


// *******************************************************
// ����������
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
	BYTE bChar; // ����ֵ
	BYTE bVk;  // �������ֵ
	WORD wSBCCase; // ȫ�����
}KEYVALUE;

typedef struct {
    KEYVALUE  keyValue[6]; // ��ż�ֵ������
	int		iKeyNum;  // ��ֵ�ĸ���
    int     iLeft;  // ��ʼλ��
	int     iWidth; // ���Ŀ��
} KEYENTRY;

#define NUM_KEYBDS 1

#define NONEKEY			0  // �Ѿ�û���ַ���
#define PINYIN_KB		1  // �л���ƴ������
#define YINGWEN_KB		2  // �л���Ӣ�ļ���
#define FUHAO_KB		3  // �л������ż���
#define QUANJIAO		4  // ת����ȫ��ģʽ
#define HANDWRITE_KB	5  // �л�����д����


//#define BLK_HEIGHT 16
#define COLUMN_NUM 12
#define ROW_NUM    5

//static int g_RowHeightList[ROW_NUM+1] = {1,31,61,91,120};
//static int g_RowHeightList[ROW_NUM+1] = {2,22,43,63,84,104};
static int g_RowHeightList[ROW_NUM+1] = {1,18,34,50,66,82};
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {1,40,80,120,160,200,240};
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {1,30,60,90,120,150,180};
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {0,30,60,90,120,150,180,210}; // �м�ͷ��7������

// ��������
const static KEYENTRY g_engkeys[2][ROW_NUM][COLUMN_NUM + 1] = {
	{
		//��һ��
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
		//�ڶ���
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
		//������
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
		//������
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
		//������
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
	{  // Сд����
		//��һ��
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
		//�ڶ���
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
		//������
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
		//������
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
		//������
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
/*		//�ڶ���
		{
			{		{{'a','A',0xa3e1},{'A','A',0xa3c1},{'b','B',0xa3e2},{'B','B',0xa3c2},{'c','C',0xa3e3},{'C','C',0xa3c3}},	   6,	  0 + STARTX,	41},
			{		{{'d','D',0xa3e4},{'D','D',0xa3c4},{'e','E',0xa3e5},{'E','E',0xa3c5},{'f','F',0xa3e6},{'F','F',0xa3c6}},	   6,     42+ STARTX,	41},
			{		{{'g','G',0xa3e7},{'G','G',0xa3c7},{'h','H',0xa3e8},{'H','H',0xa3c8},{'i','I',0xa3e9},{'I','I',0xa3c9}},	   6,     84+ STARTX,	40},
			{		{{'j','J',0xa3ea},{'J','J',0xa3ca},{'k','K',0xa3eb},{'K','K',0xa3cb}},			   4,     125 + STARTX,	41},
			{		{{'l','L',0xa3ec},{'L','L',0xa3cc},{'m','M',0xa3ed},{'M','M',0xa3cd}},			   4,     167 + STARTX,	41},
//			{		{{'\x08',VK_BACK,0}},					   1,     199 + STARTX,	39},
			{		{{0,NONEKEY,0}},					   0,	  0,			0},
		},
		//������
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
		//������
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
static HBITMAP g_hBitmap = NULL;  // ��ǰ���̵�λͼ���
static HBITMAP g_hInvertBitmap = NULL; // ��ǰ����λͼ�ľ��

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

// ���µ�������Ҫ�봰�ڹҹ�
typedef struct structENG_Keyboard{
	BOOL g_bSBCCase;
	RECT eng_HitRect;  // ��ǰ������λ��
	KEYENTRY *eng_lpHitKey; // ��ǰ���µļ�
//	KEYENTRY *eng_lpSelectKey; // ��ǰ���û�ѡ��ļ�
	BOOL Eng_KeyBroad_Shift;  // ��ǰ��SHIFT��״̬
	BOOL Eng_KeyBroad_Capital;  // ��ǰ��CAPS״̬
	int g_iBitmapIndex;   // ��ǰʹ�õ�λͼ���к�
	BOOL g_bCursorLeft;  // ��ǰ����Ƿ��Ѿ��뿪�˿�ʼ���µļ�
}ENG_KEYBOARD, *LPENG_KEYBOARD;

#if 0
static BOOL g_bSBCCase = FALSE;
static RECT eng_HitRect={0,0,0,0};  // ��ǰ������λ��
static 	KEYENTRY *eng_lpHitKey = NULL; // ��ǰ���µļ�
static 	KEYENTRY *eng_lpSelectKey = NULL; // ��ǰ���û�ѡ��ļ�
static BOOL Eng_KeyBroad_Capital = FALSE;  // ��ǰ��CAPS״̬
//static BOOL Eng_KeyBroad_Shift = FALSE;  // ��ǰ��SHIFT״̬
static int g_iBitmapIndex = 0;   // ��ǰʹ�õ�λͼ���к�
static BOOL g_bCursorLeft = FALSE;  // ��ǰ����Ƿ��Ѿ��뿪�˿�ʼ���µļ�
#endif

// *******************************************************
// ����������
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

// ����Ӣ�ļ��̺���
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
// ������static int eng_Who(HWND hWnd)
// ������
// 	��
// ����ֵ�����ص�ǰ���̵ı�ʶ��
// ����������
// ����: 
// **************************************************
static int eng_Who(HWND hWnd)
{
	return 1;
}

// **************************************************
// ������static void eng_Create(HWND hWnd)
// ������
// 	��
// ����ֵ����
// �����������������̡�
// ����: 
// **************************************************
static void eng_Create(HWND hWnd)
{
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)malloc(sizeof(ENG_KEYBOARD));
		if (lpENG_Keyboard == NULL)
			return;
		memset(lpENG_Keyboard,0,sizeof(ENG_KEYBOARD));
		lpENG_Keyboard->Eng_KeyBroad_Capital = TRUE;  // Ĭ��Ϊ��д״̬
		lpENG_Keyboard->Eng_KeyBroad_Shift = FALSE;

		SetWindowLong(hWnd,ENG_PRIVATEDATA,(LONG)lpENG_Keyboard);
		eng_Resume(hWnd); // �ָ�����
}

// **************************************************
// ������static void eng_Destroy(HWND hWnd)
// ������
// 	��
// ����ֵ����
// �����������ƻ����̡�
// ����: 
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
	{ // �Ѿ�װ�ؼ���λͼ
		DeleteObject(g_hPYBitmap); // ɾ��λͼ���
		g_hPYBitmap = NULL;
	}
*/
/*	if (g_hInvertBitmap != NULL)
	{ // �Ѿ�װ�ذ���״̬����λͼ
		DeleteObject(g_hInvertBitmap);  // ɾ��λͼ���
		g_hInvertBitmap = NULL;
	}
*/
}

// **************************************************
// ������static void eng_Resume(HWND hWnd)
// ������
// 	��
// ����ֵ����
// �����������ָ����̡�
// ����: 
// **************************************************
static void eng_Resume(HWND hWnd)
{
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return;
//	eng_Count = 0;
		SetRect(&lpENG_Keyboard->eng_HitRect,0,0,0,0);  // ���ü��̵İ���״̬Ϊ��
}
// **************************************************
// ������static LRESULT eng_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
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
static LRESULT eng_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

// **************************************************
// ������static KEYENTRY *GetKey(HWND hWnd,int x,int y,RECT *lprect)
// ������
// 	IN x -- X����
// 	IN y -- Y����
// 	OUT lprect -- ���ذ����ľ���
// 
// ����ֵ�����ص�ǰ���µļ��Ľṹָ��
// ������������������λ�õõ����µļ���ָ��
// ����: 
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
		// �õ���ǰ���µ�λ���ڼ��̵ĵڼ���
		while(1)
		{
//			if (y < (iRow+1)*BLK_HEIGHT)
			if (y < g_RowHeightList[iRow+1]) // ��ǰY�������Ƿ�����һ�е�����
				break; 
			iRow ++;
			if (iRow >=ROW_NUM)  // �Ѿ������һ��
				return NULL; // û�а����κμ�
		}

		if (lpENG_Keyboard->Eng_KeyBroad_Capital == FALSE)
		{ // Сд
			iKeyIndex = 1;
		}
		else
		{
			iKeyIndex = 0;
		}
		if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
		{
			iKeyIndex = 1 - iKeyIndex; // ȡ��
		}

		// �õ���ǰ���µ�λ���ڼ��̵�ָ���е���һ��
		while(1)
		{
			if (g_engkeys[iKeyIndex][iRow][iColumn].iKeyNum == 0)  // û���κΰ���
				return NULL;

			if ( x < g_engkeys[iKeyIndex][iRow][iColumn].iLeft + g_engkeys[iKeyIndex][iRow][iColumn].iWidth) // �Ƿ���ָ������
				break;
			
			iColumn ++;
		}
		// �Ѿ��ҵ�ָ������
		if (lprect != NULL)
		{ // �õ�ָ�������ľ���
			lprect->left = g_engkeys[iKeyIndex][iRow][iColumn].iLeft;
			lprect->right = lprect->left + g_engkeys[iKeyIndex][iRow][iColumn].iWidth;
			lprect->top = g_RowHeightList[iRow];
			lprect->bottom = g_RowHeightList[iRow+1]-1;
		}
		return ( KEYENTRY *)&g_engkeys[iKeyIndex][iRow][iColumn];  // ���ذ�����ָ��
}

#define KEYEVENTF_KEYDOWN 0

// **************************************************
// ������static int eng_DoLBUTTONDOWN( HWND hWnd, int x, int y )
// ������
// 	IN hWnd -- ���ھ��
// 	IN x -- X����
// 	IN y -- Y����
// 
// ����ֵ����
// �������������������¡�
// ����: 
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
		{ // ���ǿվ���
			hdc = GetDC( hWnd );
			ShowKeyboardrect(hWnd,hdc,lpENG_Keyboard->eng_HitRect,NORMALSHOW); // �ָ�ԭ����������ʾ
			ReleaseDC( hWnd, hdc );
			SetRect(&lpENG_Keyboard->eng_HitRect,0,0,0,0);  // ����Ϊ�վ���
		}
#if 0
		if (lpENG_Keyboard->eng_lpSelectKey)
		{
			// �����ѡ���
			int iIndex;
				
				iIndex = GetSelectIndex(x,y); // �õ���ǰ���ѡ�����������ֵ

				if (iIndex >= 0 && iIndex < 6)
				{
					// �ʵ㵽ѡ����
					if (SelectKey(hWnd,lpENG_Keyboard->eng_lpSelectKey,iIndex) == TRUE) // ѡ���
					{ // �Ѿ�ѡ��
						lpENG_Keyboard->eng_lpSelectKey = NULL; // ���ѡ���
						ClearSelectArea(hWnd);
					}
					return TRUE;
				}
		}
#endif
		// �õ�ָ��λ�õļ�
		lpKey = GetKey(hWnd,x,y,&rect);
		if (lpKey == NULL)
			return FALSE; // û�м�
/*
// ���̲������ƶ�
		if (lpKey->bVk[0] == VK_MOVE)
		{ // ��ǰ�������϶������ƶ�
			StartMoveKeyboard(hWnd,x,y); // ��ʼ�ƶ�����
			return TRUE;
		}
*/
#if 0
		if (lpENG_Keyboard->eng_lpSelectKey)
		{ // ��ǰ��ѡ���
			lpENG_Keyboard->eng_lpSelectKey = NULL; // ���ѡ���
			ClearSelectArea(hWnd);
		}
#endif
		lpENG_Keyboard->eng_HitRect = rect; // ���ð��¼��ľ���
		lpENG_Keyboard->eng_lpHitKey = lpKey; // ���ð��µļ�
		hdc = GetDC( hWnd );
		//InvertRect(hdc,&rect);
		ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW);  // ��ʾ��ǰ����Ϊ����״̬
		ReleaseDC( hWnd, hdc );
		lpENG_Keyboard->g_bCursorLeft = FALSE; // û���뿪��ǰ�İ���

		if (lpKey->iKeyNum == 1)
		{
			if (lpKey->keyValue[0].bVk == PINYIN_KB)
			{ // ��Ҫ�л���ƴ������
				eng_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_SPELL,0); // �л���ƴ������
			}
			else if (lpKey->keyValue[0].bVk == YINGWEN_KB)
			{ // ��Ҫ�л���Ӣ�ļ���
				eng_Resume(hWnd);
				// ��ǰ�Ѿ���Ӣ�ļ���
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_SPELL,0); // �л���ƴ������
			}
			else if (lpKey->keyValue[0].bVk == FUHAO_KB)
			{ // ��Ҫ�л������ż���
				eng_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_SYMBOL,0); // �л������ż���
			}
			else if (lpKey->keyValue[0].bVk == HANDWRITE_KB)
			{ // ��Ҫ�л�����д����
				eng_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_HANDWRITE,0); // �л�����д����
			}
			else if (lpKey->keyValue[0].bVk == QUANJIAO)
			{ // ��Ҫ�л���ȫ�Ǽ���
				if (lpENG_Keyboard->g_bSBCCase == FALSE)
					lpENG_Keyboard->g_bSBCCase = TRUE;
				else
				{
					lpENG_Keyboard->g_bSBCCase = FALSE;
					hdc = GetDC( hWnd );
					DrawSystemKey(hWnd,hdc,QUANJIAO);  // �ָ�ȫ�Ǽ���״̬
					ReleaseDC( hWnd, hdc );
				}
		
				eng_Resume(hWnd);
			}
			else
			{ // ��һ���
				SelectKey(hWnd,lpKey,0); // ֻ��һ����,ֱ��ѡ���һ�������
				//keybd_event(lpKey->keyValue[0].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // ����һ��������
				//keybd_event(lpKey->keyValue[0].bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
				//keybd_event(lpKey->keyValue[0].bVk,0,KEYEVENTF_KEYUP,0); // ����һ��������
			}
		}
#if 0
		else
		{
			// ��ǰ�ļ����ж����ֵ����Ҫѡ��
			lpENG_Keyboard->eng_lpSelectKey = lpKey;
			ShowSelectArea(hWnd);
		}
#endif
		return 0;
}

// **************************************************
// ������static int eng_DoLBUTTONUP( HWND hWnd, int x, int y )
// ������
// 	IN hWnd -- ���ھ��
// 	IN x -- X����
// 	IN y -- Y����
// 
// ����ֵ����Ч
// ������������������������
// ����: 
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
		{  // �������ƶ�״̬
			EndMoveKeyboard(hWnd); // ���������ƶ�
			return TRUE; // ����
		}
		if (lpENG_Keyboard->g_bCursorLeft ==TRUE) // ����Ѿ��뿪���µļ�����Ҫ���κδ���
			return TRUE;
		// �õ�ָ��λ�õļ�
		lpKey = GetKey(hWnd,x,y,&rect);  // �õ���ֵ
		if (lpKey == NULL)
			return FALSE; // û���κμ�
		if (lpENG_Keyboard->eng_lpHitKey !=NULL)
		{  // ���µļ���Ϊ��
			if (lpKey->keyValue[0].bVk == QUANJIAO)
			{
				return 0;  // ȫ�Ǽ�����Ҫ���κδ���
			}
			hdc = GetDC( hWnd );
//			InvertRect(hdc,&lpENG_Keyboard->eng_HitRect);
//			ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW);
			ShowKeyboardrect(hWnd,hdc,lpENG_Keyboard->eng_HitRect,NORMALSHOW); // �ָ����µļ�
			ReleaseDC( hWnd, hdc );
			SetRect(&lpENG_Keyboard->eng_HitRect,0,0,0,0);  // ���ð��µļ�����Ϊ��
		}

		if (lpKey->keyValue[0].bVk == VK_TURN)
		{  // ����ת����ƴ��
		   SendMessage(hWnd,KM_SETNEXTKEYBD,0,0); // ת������һ���ļ�
		}
		else if (lpKey->keyValue[0].bVk == PINYIN_KB)
		{  // ����ת����ƴ��
		   //SendMessage(hWnd,KM_SETNEXTKEY,0,0); // ת������һ���ļ�
		}
		else if (lpKey->keyValue[0].bVk == FUHAO_KB)
		{  // ����ת��������
		   //SendMessage(hWnd,KM_SETNEXTKEY,0,0); // ת������һ���ļ�
		}

		else if (lpKey->keyValue[0].bVk == VK_SHIFT)
		{ // ��ǰ����ΪSHIFT
			if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
			{  // �Ѿ�������SHIFT
				lpENG_Keyboard->Eng_KeyBroad_Shift = FALSE; // ����SHIFT����
			}
			else
			{ // ��û�а���SHIFT
				lpENG_Keyboard->Eng_KeyBroad_Shift = TRUE; // ����SHIFT����
			}
			LoadEngKeyboardImage(hWnd, TRUE);
			InvalidateRect(hWnd,NULL,TRUE); // �ػ�����
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
			{  // �Ѿ�������SHIFT
				lpENG_Keyboard->Eng_KeyBroad_Shift = FALSE; // ����SHIFT����
				LoadEngKeyboardImage(hWnd, TRUE);
				InvalidateRect(hWnd,NULL,TRUE); // �ػ�����
			}
		}

		lpENG_Keyboard->eng_lpHitKey = NULL;    // �Ѿ�û�а��µļ�
		return 0;
}

// **************************************************
// ������static int eng_DoMOUSEMOVE( HWND hWnd, int x, int y )
// ������
// 	IN hWnd -- ���ھ��
// 	IN x -- X����
// 	IN y -- Y����
// 
// ����ֵ����Ч
// �����������������������ƶ���
// ����: 
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
		{  // �������ƶ�״̬
			MoveKeyboard(hWnd,x,y);  // �ƶ�����
			return TRUE;
		}
		if (lpENG_Keyboard->eng_lpHitKey == NULL)
			return FALSE; // û�а��µļ�
		// �õ�ָ��λ�õļ�
		lpKey = GetKey(hWnd,x,y,&rect);
		if (lpKey != lpENG_Keyboard->eng_lpHitKey)
		{  // ��ǰ�ļ��밴�µļ���ͬ
			if (lpENG_Keyboard->g_bCursorLeft == TRUE)  // �Ѿ��뿪�˰��µļ�
				return TRUE; // ����Ҫ����
			if (lpENG_Keyboard->eng_lpHitKey->keyValue[0].bVk != QUANJIAO)
			{  // ���µļ�����ȫ�Ǽ�
//				InvertRect(hdc,&lpENG_Keyboard->eng_HitRect);
				hdc = GetDC( hWnd );
				ShowKeyboardrect(hWnd,hdc,lpENG_Keyboard->eng_HitRect,NORMALSHOW); // �ָ����¼���״̬
				ReleaseDC( hWnd, hdc );
			}
			lpENG_Keyboard->g_bCursorLeft = TRUE; // ���ü��Ѿ��뿪ԭ�����µļ�
		}
		else
		{ // ��ǰ�ļ��밴�µļ���ͬ
			if (lpENG_Keyboard->g_bCursorLeft == FALSE) // ��û���뿪���µļ�
				return TRUE;
			// ����뿪ԭ�����µļ����ֻص�ԭ���İ���λ��
			if (lpKey->keyValue[0].bVk != QUANJIAO)
			{  // ���µļ�����ȫ�Ǽ�
//				InvertRect(hdc,&lpENG_Keyboard->eng_HitRect);
				hdc = GetDC( hWnd );
				ShowKeyboardrect(hWnd,hdc,lpENG_Keyboard->eng_HitRect,INVERTSHOW); // ��ʾ��Ϊ���µ�״̬
				ReleaseDC( hWnd, hdc );	
			}
			lpENG_Keyboard->g_bCursorLeft = FALSE; // ���ü���ԭ�����µ�λ��
		}
		return 0;
}

// **************************************************
// ������static int eng_DoPAINT( HWND hWnd, HDC hdc )
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ�� ��
// ��������������Ӣ�ļ��̡�
// ����: 
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
	  {  // ��û�м���ͼ����
		  HINSTANCE hInstance;

//		  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		  hInstance = g_hInstance;
		  //g_hPYBitmap = LoadImage( 0, "\\System\\Keyboard1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_NORMALKB), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ��Ӣ�ļ���λͼ
//		  g_hPYBitmap = LoadImage( hInstance, "\\KINGMOS\\ENGKB.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��Ӣ�ļ���λͼ
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_PINYIN), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ����д����λͼ
		  g_hBitmap = LoadImage( 0, "\\����\\eng1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hBitmap == 0 )
			return 0;  // װ��ʧ��
	  }
*/
	  LoadEngKeyboardImage(hWnd, FALSE);

	  KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,0,iStarty,SRCCOPY); // ���Ƽ���

	  DrawSystemKey(hWnd,hdc,VK_SHIFT);  // �ָ�ȫ�Ǽ���״̬
	  DrawSystemKey(hWnd,hdc,VK_CAPITAL);  // �ָ�ȫ�Ǽ���״̬
	  return 0;
}
// **************************************************
// ������void KB_DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN rect -- λͼ����
// 	IN xOffset -- X����ƫ��
// 	IN yOffset -- Y����ƫ��
// 	IN dwRop -- ����ģʽ
// 
// ����ֵ����
// ��������������λͼ��
// ����: 
// **************************************************
void KB_DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
{
		HDC hMemoryDC;
    HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc);  // ��������DC
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // ��Ҫ���Ƶ�λͼѡ��DC
		// ����λͼ
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

		SelectObject(hMemoryDC,hOldBitmap); // �ָ�λͼ���
		DeleteDC(hMemoryDC); // ɾ��DC
}
// **************************************************
// ������static BOOL eng_GetKeyValue(HWND hWnd, WORD * lpKey )
// ������
// 	OUT lpKey -- �õ���ǰ������ֵ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰ������ֵ��
// ����: 
// **************************************************
static BOOL eng_GetKeyValue(HWND hWnd, WORD * lpKey )
{
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return FALSE;
	if( lpENG_Keyboard->eng_lpHitKey )
	{  // �м�����
		*lpKey = lpENG_Keyboard->eng_lpHitKey->keyValue[0].bChar; // ���ص�ǰ������ֵ
		return TRUE;
	}
	else
		return FALSE; // ����ʧ��
}


// **************************************************
// ������static void DrawSystemKey(HWND hWnd,HDC hdc,BYTE bVk)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN bVk -- ϵͳ���ļ�ֵ
// 
// ����ֵ����
// ��������������ϵͳ����״̬��
// ����: 
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
				ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // ��ʾ��Ϊ���µ�״̬
			else
				ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW); // ��ʾ��Ϊ���µ�״̬
		}
		if (bVk == VK_CAPITAL)
		{
			if (lpENG_Keyboard->Eng_KeyBroad_Capital == TRUE)
				ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // ��ʾ��Ϊ���µ�״̬
			else
				ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW); // ��ʾ��Ϊ���µ�״̬
		}
		if (bVk == VK_SHIFT)
		{
			if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
				ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // ��ʾ��Ϊ���µ�״̬
			else
				ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW); // ��ʾ��Ϊ���µ�״̬
		}
		if (bVk == YINGWEN_KB)
		{
			ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // ��ʾ��Ϊ���µ�״̬
		}
		
		return;

}

// **************************************************
// ������static void ShowKeyboardrect(HWND hWnd,HDC hdc,RECT rect,DWORD dwShowStyle)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN rect -- Ҫ��ʾ�ľ���
// 	IN dwShowStyle -- ��ʾ״̬
// 
// ����ֵ����
// ������������ʾ���̵�ָ�������λͼ��
// ����: 
// **************************************************
static void ShowKeyboardrect(HWND hWnd,HDC hdc,RECT rect,DWORD dwShowStyle)
{
//  int iStarty; // �õ���ǰ���̵���ʼλ��

/*	  if (g_hBitmap == NULL)
	  {  // ��û�м���ͼ����
		  HINSTANCE hInstance;

//		  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		  hInstance = g_hInstance;
		  //g_hPYBitmap = LoadImage( 0, "\\System\\Keyboard1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
	//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_NORMALKB), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ��Ӣ�ļ���λͼ
//		  g_hPYBitmap = LoadImage( hInstance, "\\KINGMOS\\ENGKB.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��Ӣ�ļ���λͼ
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_PINYIN), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ����д����λͼ
		  g_hBitmap = LoadImage( 0, "\\����\\eng1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hBitmap == 0 )
			return ;  // װ��ʧ��
	  }
*/
/*	if (g_hInvertBitmap == NULL)
	{ // û��װ��ѡ��λͼ
		  //g_hInvertBitmap = LoadImage( 0, "\\System\\Keyboard2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  g_hInvertBitmap = LoadImage( (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE(IDB_ENGSEL), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;// װ��λͼ���
		  if( g_hInvertBitmap == 0 )
			return ;
	}
*/
/*	if (dwShowStyle == NORMALSHOW)  // ͨ����ʾ
		iStarty = 0;  // ��һ��������һ�����
	else // ѡ�񣨷��ף���ʾ
		iStarty = KBDBMPHEIGHT;  // �ڶ���������һ�����
	
	KB_DrawBitMap(hdc,(HBITMAP)g_hPYBitmap,rect,rect.left + 0,rect.top + iStarty,SRCCOPY);
	*/
/*	iStarty = 0;
	if (dwShowStyle == NORMALSHOW)  // ͨ����ʾ
		KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,rect.left + 0,rect.top + iStarty,SRCCOPY);
//		iStarty = 0;  // ��һ��������һ�����
	else // ѡ�񣨷��ף���ʾ
		//iStarty = KBDBMPHEIGHT;  // �ڶ���������һ�����	
		KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,rect.left + 0,rect.top + iStarty,NOTSRCCOPY);
*/
	LoadEngKeyboardImage(hWnd, FALSE);
	if (dwShowStyle == NORMALSHOW)  // ͨ����ʾ
		KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,rect.left + 0,rect.top,SRCCOPY);
	else // ѡ�񣨷��ף���ʾ
		KB_DrawBitMap(hdc,(HBITMAP)g_hInvertBitmap,rect,rect.left + 0,rect.top,SRCCOPY);
}

#if 0
// **************************************************
// ������static void ShowSelectArea(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ������������ʾ���̵�ѡ������
// ����: 
// **************************************************
static void ShowSelectArea(HWND hWnd)
{
	int i;
	HDC hdc;
	RECT rect;
	int pStartx[7] = {1,40,80,120,160,200,240};
	// ����һ��������ѡ������
	LPENG_KEYBOARD  lpENG_Keyboard;

		lpENG_Keyboard = (LPENG_KEYBOARD)GetWindowLong(hWnd,ENG_PRIVATEDATA);
		if (lpENG_Keyboard == NULL)
			return;

		if (lpENG_Keyboard->eng_lpSelectKey == NULL)
			return ; // û����Ҫѡ��ļ�
		hdc = GetDC(hWnd); // �õ��豸���

		SetTextColor(hdc,RGB(0,0,0));
		SetBkColor(hdc,RGB(231,242,255));

		rect.top = g_RowHeightList[0];  // �õ�ѡ������Ĵ�ֱλ��
		rect.bottom = g_RowHeightList[1] - 1 ;

		for (i = 0 ; i < lpENG_Keyboard->eng_lpSelectKey->iKeyNum ; i ++)
		{
			rect.left = g_ColumnWidthList[i];
			rect.right = g_ColumnWidthList[i + 1] - 2;
			DrawText(hdc,&lpENG_Keyboard->eng_lpSelectKey->keyValue[i].bChar,1,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		ReleaseDC(hWnd,hdc);  // �ͷ��豸���
}
// **************************************************
// ������static void ClearSelectArea(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������������̵�ѡ������
// ����: 
// **************************************************
static void ClearSelectArea(HWND hWnd)
{
//	int i;
	HDC hdc;
	RECT rect;
	// ����һ��������ѡ������

//		hdc = GetDC(hWnd); // �õ��豸���

//		rect.top = 1;  // �õ�ѡ������Ĵ�ֱλ��
//		rect.bottom = 30;

//		for (i = 0 ; i < COLUMN_NUM ; i ++)
//		{
//			rect.left = g_ColumnWidthList[i];
//			rect.right = g_ColumnWidthList[i + 1] - 2;
//			FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
//		}
//		ReleaseDC(hWnd,hdc);  // �ͷ��豸���

		if (g_hBitmap)
		{
			hdc = GetDC(hWnd); // �õ��豸���

	//		rect.top = 1;  // �õ�ѡ������Ĵ�ֱλ��
	//		rect.bottom = 30;
			rect.left = g_ColumnWidthList[0]; 
			rect.right = g_ColumnWidthList[7]; 
			rect.top = g_RowHeightList[0];  // �õ�ѡ������Ĵ�ֱλ��
			rect.bottom = g_RowHeightList[1] -1 ;
		
			KB_DrawBitMap(hdc,(HBITMAP)g_hBitmap,rect,rect.left,rect.top,SRCCOPY); // ���Ƽ���
			ReleaseDC(hWnd,hdc);
		}

}

// **************************************************
// ������static int GetSelectIndex(int x,int y)
// ������
// 	IN x -- x ����
// 	IN y -- y ����
// 
// ����ֵ������ָ���������ڵ�����
// �����������õ���ǰ���ѡ�����������ֵ��
// ����: 
// **************************************************
static int GetSelectIndex(int x,int y)
{
	int iIndex = 0;
	int i;
		if (y < 0 || (y >= g_RowHeightList[1])) // ��ǰY�������Ƿ�����һ�е�����
			return -1; // ��ǰ���겻�ڵ�һ��
		// �õ���ǰ���µ�λ���ڼ��̵�ָ���е���һ��
		for (i = 0 ; i < COLUMN_NUM ; i ++)
		{
			if ( x < g_ColumnWidthList[i + 1])
				return i ;  // x �����ڵ�ǰ������
		}
		return -1;
}
#endif

// **************************************************
// ������static BOOL SelectKey(HWND hWnd, KEYENTRY *lpKey,int iIndex)
// ������
// 	IN hWnd -- ���ھ��
//  IN lpKey --Ҫ����ļ�
// 	IN iIndex -- Ҫ����ļ���������
// 
// ����ֵ����
// ����������ѡ��ָ�������ļ���
// ����: 
// **************************************************
/*
static BOOL SelectKey(HWND hWnd, KEYENTRY *lpKey,int iIndex)
{
	if (lpKey == NULL)
		return FALSE; // û��ѡ��� 
	if (iIndex < 0 || iIndex >= lpKey->iKeyNum)
	{
		// ���������
		return FALSE;
	}
	
	if (lpENG_Keyboard->g_bSBCCase == FALSE)
	{  // �������ַ�
		if (lpKey->keyValue[iIndex].bVk) // ��Ҫ������
			keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // ����һ��������
		if (lpKey->keyValue[iIndex].bChar) // ���ַ���Ҫ���
			keybd_event(lpKey->keyValue[iIndex].bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
		if (lpKey->keyValue[iIndex].bVk) // ��Ҫ������
			keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYUP,0); // ����һ��������
	}
	else
	{
		// ���ȫ���ַ�
		if (lpKey->keyValue[iIndex].wSBCCase) 
		{ // ��ȫ���ַ���Ҫ���
			BYTE bChar;

			bChar = (BYTE)(lpKey->keyValue[iIndex].wSBCCase >> 8); // �������λ
			keybd_event(bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
			bChar = (BYTE)lpKey->keyValue[iIndex].wSBCCase; // �������λ
			keybd_event(bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
		}
		else
		{
			// û��ȫ���ַ�������������ַ�
			if (lpKey->keyValue[iIndex].bVk) // ��Ҫ������
				keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // ����һ��������
			if (lpKey->keyValue[iIndex].bChar) // ���ַ���Ҫ���
				keybd_event(lpKey->keyValue[iIndex].bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
			if (lpKey->keyValue[iIndex].bVk) // ��Ҫ������
				keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYUP,0); // ����һ��������
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
			return FALSE; // û��ѡ��� 
		if (iIndex < 0 || iIndex >= lpKey->iKeyNum)
		{
			// ���������
			return FALSE;
		}
		
		if (lpENG_Keyboard->g_bSBCCase == FALSE)
		{  // �������ַ�
			if (lpKey->keyValue[iIndex].bVk == 0 && lpKey->keyValue[iIndex].bChar == 0)
			{
				// û�а���ַ���������� �����ȫ���ַ�
				if (lpKey->keyValue[iIndex].wSBCCase) 
				{ // ��ȫ���ַ���Ҫ���
					BYTE bChar;

					bChar = (BYTE)(lpKey->keyValue[iIndex].wSBCCase >> 8); // �������λ
					keybd_event(bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
					bChar = (BYTE)lpKey->keyValue[iIndex].wSBCCase; // �������λ
					keybd_event(bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
				}
			}
			else
			{
				if (lpKey->keyValue[iIndex].bVk) // ��Ҫ������
					keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // ����һ��������
				if (lpKey->keyValue[iIndex].bChar) // ���ַ���Ҫ���
					keybd_event(lpKey->keyValue[iIndex].bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
				if (lpKey->keyValue[iIndex].bVk) // ��Ҫ������
					keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYUP,0); // ����һ��������
			}
		}
		else
		{
			// ���ȫ���ַ�
			if (lpKey->keyValue[iIndex].wSBCCase) 
			{ // ��ȫ���ַ���Ҫ���
				BYTE bChar;

				bChar = (BYTE)(lpKey->keyValue[iIndex].wSBCCase >> 8); // �������λ
				keybd_event(bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
				bChar = (BYTE)lpKey->keyValue[iIndex].wSBCCase; // �������λ
				keybd_event(bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
			}
			else
			{
				// û��ȫ���ַ�������������ַ�
				if (lpKey->keyValue[iIndex].bVk) // ��Ҫ������
					keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYDOWN,0x80000000);  // ����һ��������
				if (lpKey->keyValue[iIndex].bChar) // ���ַ���Ҫ���
					keybd_event(lpKey->keyValue[iIndex].bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ
				if (lpKey->keyValue[iIndex].bVk) // ��Ҫ������
					keybd_event(lpKey->keyValue[iIndex].bVk,0,KEYEVENTF_KEYUP,0); // ����һ��������
			}
		}

		return TRUE;
}

// **************************************************
// ������static BOOL GetVirtualKeyRect(HWND hWnd,BYTE bVk, RECT *lprect)
// ������
// 	IN bVk -- ָ�����
// 	OUT lprect -- ���ظ������λ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ�ָ�������λ�á�
// ����: 
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
		{ // Сд
			iKeyIndex = 1;
		}
		else
		{
			iKeyIndex = 0;
		}
		if (lpENG_Keyboard->Eng_KeyBroad_Shift == TRUE)
		{
			iKeyIndex = 1 - iKeyIndex; // ȡ��
		}

		for (iRow = 0; iRow < ROW_NUM; iRow ++)
		{ // �õ������������һ��
			iColumn=0;
			while(1)
			{ // �õ�����ļ���λ��

				for ( i = 0; i< g_engkeys[iKeyIndex][iRow][iColumn].iKeyNum; i++)
				{ // ÿһ�����Ͽ����ж����ֵ
					if ( bVk == g_engkeys[iKeyIndex][iRow][iColumn].keyValue[0].bVk)
					{ // ��ǰ�ļ��������
						if (lprect != NULL)
						{ // ָ����ָ����Ч���õ�����λ��
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
					break; // �Ѿ������һ����
			}
		}
		return FALSE; // û���ҵ�ָ���ļ�
}

// **************************************************
// ������static BOOL LoadEngKeyboardImage(HWND hWnd, BOOL bReLoad)
// ������
//	IN hWnd -- ���ھ��
//	IN bReLoad -- �Ƿ���Ҫ����װ�� 
// ����ֵ���ɹ�״̬����TRUE�����򷵻�FALSE
// ����������װ�ص�ǰ�ļ���ͼ��
// ����: 
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
		{ // Сд
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
			iKeyIndex = 1 - iKeyIndex; // ȡ��
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
		{  // ��û�м���ͼ����
		  g_hBitmap = LoadImage( 0, lpKBImageFileName[iStyleIndex][iKeyImageIndex], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hBitmap == 0 )
			return FALSE;  // װ��ʧ��
		}
		if (g_hInvertBitmap == NULL)
		{  // ��û�м���ͼ����
		  g_hInvertBitmap = LoadImage( 0, lpKBImageFileName[iStyleIndex][iKeyInvImageIndex], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hInvertBitmap == 0 )
			return FALSE;  // װ��ʧ��
		}

		return TRUE;
}
