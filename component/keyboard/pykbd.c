/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����ƴ������
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-05-19
���ߣ��½��� Jami chen
�޸ļ�¼��
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

#define NONEKEY		0  // �Ѿ�û���ַ���
#define PINYIN_KB	1  // �л���ƴ������
#define YINGWEN_KB	2  // �л���Ӣ�ļ���
#define FUHAO_KB	3  // �л������ż���
#define QUANJIAO	4  // ת����ȫ��ģʽ
#define HANDWRITE_KB	5  // �л�����д����
#define PAGEUP_SEL	6	// ��ѡ���ı�������һҳ
#define PAGEDOWN_SEL	7	// ��ѡ���ı�������һҳ


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
static int g_ColumnWidthList[COLUMN_NUM + 1] = {20,59,99,139,179,220,0}; // �м�ͷ��5������
#endif
#if SELECT_NUM == 6
static int g_ColumnWidthList[COLUMN_NUM + 1] = {20,53,86,119,152,185,218}; // �м�ͷ��6������
#endif
#if SELECT_NUM == 7
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {21,49,77,105,133,161,189,217}; // �м�ͷ��7������
static int g_ColumnWidthList[COLUMN_NUM + 1] = {0,30,60,90,120,150,180,210}; // �м�ͷ��7������
#endif
#if SELECT_NUM == 8
//static int g_ColumnWidthList[COLUMN_NUM + 1] = {24,48,72,96,120,144,168,192,216}; // �м�ͷ��8������
static int g_ColumnWidthList[COLUMN_NUM + 1] = {68,84,100,116,132,148,164,180,196}; // �м�ͷ��8������
#endif

// ��������
const static KEYENTRY g_pykeys[ROW_NUM][COLUMN_NUM + 1] = {
		//��һ��
		{
			// ��ʾ����������
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
			{		{{'\x08',VK_BACK,0}},		1,    185 + STARTX,	24},
			{		 {{0,NONEKEY,0}},						   0,	  0,			0}
		},
		//������
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
		//������
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
		//������
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
/*		//�ڶ���
		{
			{		{{'a','A',0xa3e1},{'b','B',0xa3e2},{'c','C',0xa3e3}},	   3,	  0 + STARTX,	41},
			{		{{'d','D',0xa3e4},{'e','E',0xa3e5},{'f','F',0xa3e6}},	   3,     42+ STARTX,	41},
			{		{{'g','G',0xa3e7},{'h','H',0xa3e8},{'i','I',0xa3e9}},	   3,     84+ STARTX,	40},
			{		{{'j','J',0xa3ea},{'k','K',0xa3eb}},					   2,     125 + STARTX,	41},
			{		{{'l','L',0xa3ec},{'m','M',0xa3ed}},			           2,     167 + STARTX,	41},
//			{		{{'\x08',VK_BACK,0}},								       1,     210 + STARTX,	30},
			{		{{0,NONEKEY,0}},					   0,	  0,			0},
		},
		//������
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
		//������
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



//HBITMAP g_hPYBitmap = NULL;  // ��ǰ���̵�λͼ���
static HBITMAP g_hPYBitmap = NULL;  // ��ǰ���̵�λͼ���
static HBITMAP g_hInvertBitmap = NULL; // ��ǰ����λͼ�ľ��
HBITMAP g_hDisableArrow = NULL;  // ��ǰ���̵�λͼ���
//static HBITMAP g_hDisableArrow = NULL;  // ��ǰ���̵�λͼ���

// ����һ��ѡ������״̬�Ľṹ
typedef struct SelAreaStatusStruct
{
	BOOL bLeftArrowEnable; // ���ͷ�Ƿ���Ч
	BOOL bRightArrowEnable; // �Ҽ�ͷ�Ƿ���Ч	

	BOOL bLeftArrowPressed; // ���ͷ����
	BOOL bRightArrowPressed; // �Ҽ�ͷ����
}SELAREASTATUS, *LPSELAREASTATUS;

// ���µ�������Ҫ�봰�ڹҹ�
typedef struct structPY_Keyboard{
	BOOL g_bSBCCase ;
	SPELLRESULT py_SpellResult; // ��ǰ��ƴ�����
	RECT py_HitRect;  // ��ǰ������λ��
	KEYENTRY *py_lpHitKey ; // ��ǰ���µļ�
	BOOL PY_KeyBroad_Shift;  // ��ǰ��SHIFT��״̬
	BOOL PY_KeyBroad_Capital;  // ��ǰ��CAPS״̬
	int g_iBitmapIndex ;   // ��ǰʹ�õ�λͼ���к�
	BOOL g_bCursorLeft ;  // ��ǰ����Ƿ��Ѿ��뿪�˿�ʼ���µļ�
	SELAREASTATUS py_SelAreaStatus;
	HWND g_hSpell ;
}PY_KEYBOARD, *LPPY_KEYBOARD;

#if 0
static BOOL g_bSBCCase = FALSE;

static 	SPELLRESULT py_SpellResult; // ��ǰ��ƴ�����

static RECT py_HitRect={0,0,0,0};  // ��ǰ������λ��
static 	KEYENTRY *py_lpHitKey = NULL; // ��ǰ���µļ�
//static 	KEYENTRY *py_lpSelectKey = NULL;
static BOOL Eng_KeyBroad_Capital = FALSE;  // ��ǰ��CAPS״̬
//static BOOL py_KeyBroad_Shift = FALSE;  // ��ǰ��SHIFT״̬
static int g_iBitmapIndex = 0;   // ��ǰʹ�õ�λͼ���к�
static BOOL g_bCursorLeft = FALSE;  // ��ǰ����Ƿ��Ѿ��뿪�˿�ʼ���µļ�

static SELAREASTATUS py_SelAreaStatus;
static HWND g_hSpell = NULL;
#endif

// *******************************************************
// ����������
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

// ����Ӣ�ļ��̺���
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
// ������static int py_Who(HWND hWnd)
// ������
// 	��
// ����ֵ�����ص�ǰ���̵ı�ʶ��
// ����������
// ����: 
// **************************************************
static int py_Who(HWND hWnd)
{
	return 1;
}

// **************************************************
// ������static void py_Create(HWND hWnd)
// ������
// 	��
// ����ֵ����
// �����������������̡�
// ����: 
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

	py_Resume(hWnd); // �ָ�����
	ClearSpellResult(hWnd,&lpPY_Keyboard->py_SpellResult);

	if (lpPY_Keyboard->g_hSpell == NULL)
	{
		HINSTANCE hInstance;

		hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		RegisterSpellAreaWindow(hInstance);
		// ��û�д���ƴд����,������

		GetWindowRect(hWnd,&rect);


		/*
		iStartx = 0;  //��ʼ x ����
		iStarty = rect.top - SPELLWINDOW_HEIGHT; //��ʼ y ����
    hKey = CreateWindowEx(WS_EX_TOPMOST|WS_EX_INPUTWINDOW,  
						"Keyboard",
						"����",
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
		iStartx = 0;  //��ʼ x ����
		iStarty = rect.top - SPELLWINDOW_HEIGHT; //��ʼ y ����
	lpPY_Keyboard->g_hSpell = CreateWindowEx(WS_EX_INPUTWINDOW,
								classSpellArea, "", 
								WS_POPUP|WS_BORDER , 
								iStartx, iStarty, 
								SPELLWINDOW_WIDTH, 
								SPELLWINDOW_HEIGHT, 
								hWnd, (HMENU)ID_SPELL, 
								hInstance, hWnd);
*/		
		iStartx = 1;  //��ʼ x ����
		iStarty = 1; //��ʼ y ����
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
// ������static void py_Destroy(HWND hWnd)
// ������
// 	��
// ����ֵ����
// �����������ƻ����̡�
// ����: 
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
// ������static void py_Resume(HWND hWnd)
// ������
// 	��
// ����ֵ����
// �����������ָ����̡�
// ����: 
// **************************************************
static void py_Resume(HWND hWnd)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;
//	py_Count = 0;
		// ���ѡ����
		ClearSpellResult(hWnd,&lpPY_Keyboard->py_SpellResult); // ���ƴд����е�����
		SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // ����ѡ������

		// ���ƴд��
		if (lpPY_Keyboard->g_hSpell)
			SendMessage(lpPY_Keyboard->g_hSpell,SPM_CLEARSPELL,0,0);
		SetRect(&lpPY_Keyboard->py_HitRect,0,0,0,0);  // ���ü��̵İ���״̬Ϊ��
}

// **************************************************
// ������static LRESULT py_DefWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
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
// ������static KEYENTRY *GetKey(int x,int y,RECT *lprect)
// ������
// 	IN x -- X����
// 	IN y -- Y����
// 	OUT lprect -- ���ذ����ľ���
// 
// ����ֵ�����ص�ǰ���µļ��Ľṹָ��
// ������������������λ�õõ����µļ���ָ��
// ����: 
// **************************************************
static KEYENTRY *GetKey(int x,int y,RECT *lprect)
{
	int iRow=0,iColumn=0;

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
		// �õ���ǰ���µ�λ���ڼ��̵�ָ���е���һ��
		while(1)
		{
			if (g_pykeys[iRow][iColumn].iKeyNum == 0)  // û���κΰ���
				return NULL;

			if ( x < g_pykeys[iRow][iColumn].iLeft + g_pykeys[iRow][iColumn].iWidth) // �Ƿ���ָ������
				break;
			
			iColumn ++;
		}
		// �Ѿ��ҵ�ָ������
		if (lprect != NULL)
		{ // �õ�ָ�������ľ���
			lprect->left = g_pykeys[iRow][iColumn].iLeft;
			lprect->right = lprect->left + g_pykeys[iRow][iColumn].iWidth;
			lprect->top = g_RowHeightList[iRow];
			lprect->bottom = g_RowHeightList[iRow+1]-1;
		}
		return (KEYENTRY *)&g_pykeys[iRow][iColumn];  // ���ذ�����ָ��
}

#define KEYEVENTF_KEYDOWN 0

// **************************************************
// ������static int py_DoLBUTTONDOWN( HWND hWnd, int x, int y )
// ������
// 	IN hWnd -- ���ھ��
// 	IN x -- X����
// 	IN y -- Y����
// 
// ����ֵ����
// �������������������¡�
// ����: 
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
		{ // ���ǿվ���
			hdc = GetDC( hWnd );
			ShowKeyboardrect(hWnd,hdc,lpPY_Keyboard->py_HitRect,NORMALSHOW); // �ָ�ԭ����������ʾ
			ReleaseDC( hWnd, hdc );
			SetRect(&lpPY_Keyboard->py_HitRect,0,0,0,0);  // ����Ϊ�վ���
		}

//		if (py_lpSelectKey)
		if (ValidData(hWnd) == TRUE)
		{
			// ������Ч���ݣ�Ҫ�ж��Ƿ���Ҫ����ѡ��
			int iIndex;
				
				iIndex = GetSelectIndex(hWnd,x,y); // �õ���ǰ���ѡ�����������ֵ

				if (iIndex != -1)
				{ 
					// �ʵ㵽ѡ������Ч����
					SelectInput(hWnd , iIndex);  // ѡ������
					return TRUE;
				}
		}

		// �õ�ָ��λ�õļ�
		lpKey = GetKey(x,y,&rect);
		if (lpKey == NULL)
			return FALSE; // û�м�
		lpPY_Keyboard->py_HitRect = rect; // ���ð��¼��ľ���
		lpPY_Keyboard->py_lpHitKey = lpKey; // ���ð��µļ�
		hdc = GetDC( hWnd );
		//InvertRect(hdc,&rect);
		ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW);  // ��ʾ��ǰ����Ϊ����״̬
		ReleaseDC( hWnd, hdc );
		lpPY_Keyboard->g_bCursorLeft = FALSE; // û���뿪��ǰ�İ���

		if (lpKey->iKeyNum == 1)
		{

			if (lpKey->keyValue[0].bVk >= 'A' && lpKey->keyValue[0].bVk <= 'Z')
			{
						InputKey(hWnd,lpKey);
			}
			else if (lpKey->keyValue[0].bVk == PINYIN_KB)
			{ // ��Ҫ�л���ƴ������
				py_Resume(hWnd);
				// ��ǰ�Ѿ���ƴ������
			}
			else if (lpKey->keyValue[0].bVk == YINGWEN_KB)
			{ // ��Ҫ�л���Ӣ�ļ���
				py_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_ENGLISH,0); // �л���Ӣ�ļ���
			}
			else if (lpKey->keyValue[0].bVk == FUHAO_KB)
			{ // ��Ҫ�л������ż���
				py_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_SYMBOL,0); // �л������ż���
			}
			else if (lpKey->keyValue[0].bVk == HANDWRITE_KB)
			{ // ��Ҫ�л�����д����
				py_Resume(hWnd);
			    PostMessage(hWnd,KM_SETNAMEDKEYBD,(WPARAM)KB_HANDWRITE,0); // �л�����д����
			}
			else if (lpKey->keyValue[0].bVk == QUANJIAO)
			{ // ��Ҫ�л���ȫ�Ǽ���
				if (lpPY_Keyboard->g_bSBCCase == FALSE)
					lpPY_Keyboard->g_bSBCCase = TRUE;
				else
				{
					lpPY_Keyboard->g_bSBCCase = FALSE;
					hdc = GetDC( hWnd );
					DrawSystemKey(hWnd,hdc,QUANJIAO);  // �ָ�ȫ�Ǽ���״̬
					ReleaseDC( hWnd, hdc );
				}
		
				py_Resume(hWnd);
			}
			else
			{ // ��һ���
				if (ValidData(hWnd) == TRUE)
				{
					// ��ѡ���������Ч���ݣ�ֻ��VK_BACK��Ч
					if (lpKey->keyValue[0].bVk == VK_BACK)
					{
						// Ҫ���˻ص���һ������
						GoBackDeal(hWnd);
					}
				}
				else
				{ // ��ѡ�����û���κ���Ч���ݣ�ֱ������ַ�
					SelectKey(hWnd,lpKey,0); // ֻ��һ����,ֱ��ѡ���һ�������
				}
			}
		}
		else
		{
			// ��ǰ�ļ����ж����ֵ����Ҫѡ��  , ֻ����Ӣ���ַ����ж��
			//InsertKeyToSpell(&lpPY_Keyboard->py_SpellResult,lpKey);
			InputKey(hWnd,lpKey);
		}
		return 0;
}

// **************************************************
// ������static int py_DoLBUTTONUP( HWND hWnd, int x, int y )
// ������
// 	IN hWnd -- ���ھ��
// 	IN x -- X����
// 	IN y -- Y����
// 
// ����ֵ����Ч
// ������������������������
// ����: 
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
		{  // �������ƶ�״̬
			EndMoveKeyboard(hWnd); // ���������ƶ�
			return TRUE; // ����
		}
		if (lpPY_Keyboard->g_bCursorLeft ==TRUE) // ����Ѿ��뿪���µļ�����Ҫ���κδ���
			return TRUE;
		if (lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed)
		{
			//���ͷ������
			lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed = FALSE; // �ָ����ͷ
//			RETAILMSG(1,(TEXT("Left Arrow Up \r\n")));
			hdc = GetDC( hWnd );
			DrawArrow(hWnd,hdc,LEFTARROW,0); // ����ͷ
			ReleaseDC( hWnd, hdc );
			return 0;
		}
		if (lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed)
		{
			//�Ҽ�ͷ������
			lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed = FALSE; // �ָ��Ҽ�ͷ
//			RETAILMSG(1,(TEXT("Right Arrow Up \r\n")));
			hdc = GetDC( hWnd );
			DrawArrow(hWnd,hdc,RIGHTARROW,0); // ����ͷ
			ReleaseDC( hWnd, hdc );
			return 0;
		}


		// �õ�ָ��λ�õļ�
		lpKey = GetKey(x,y,&rect);  // �õ���ֵ
		if (lpKey == NULL)
			return FALSE; // û���κμ�
		if (lpPY_Keyboard->py_lpHitKey !=NULL)
		{  // ���µļ���Ϊ��
			if (lpKey->keyValue[0].bVk == QUANJIAO)
			{
				return 0;  // ȫ�Ǽ�����Ҫ���κδ���
			}
			hdc = GetDC( hWnd );
//			InvertRect(hdc,&lpPY_Keyboard->py_HitRect);
//			ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW);
			ShowKeyboardrect(hWnd,hdc,lpPY_Keyboard->py_HitRect,NORMALSHOW); // �ָ����µļ�
			ReleaseDC( hWnd, hdc );
			SetRect(&lpPY_Keyboard->py_HitRect,0,0,0,0);  // ���ð��µļ�����Ϊ��
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

		lpPY_Keyboard->py_lpHitKey = NULL;    // �Ѿ�û�а��µļ�
		return 0;
}

// **************************************************
// ������static int py_DoMOUSEMOVE( HWND hWnd, int x, int y )
// ������
// 	IN hWnd -- ���ھ��
// 	IN x -- X����
// 	IN y -- Y����
// 
// ����ֵ����Ч
// �����������������������ƶ���
// ����: 
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
		{  // �������ƶ�״̬
			MoveKeyboard(hWnd,x,y);  // �ƶ�����
			return TRUE;
		}
		if (lpPY_Keyboard->py_lpHitKey == NULL)
			return FALSE; // û�а��µļ�
		// �õ�ָ��λ�õļ�
		lpKey = GetKey(x,y,&rect);
		if (lpKey != lpPY_Keyboard->py_lpHitKey)
		{  // ��ǰ�ļ��밴�µļ���ͬ
			if (lpPY_Keyboard->g_bCursorLeft == TRUE)  // �Ѿ��뿪�˰��µļ�
				return TRUE; // ����Ҫ����
			if (lpPY_Keyboard->py_lpHitKey->keyValue[0].bVk != QUANJIAO)
			{  // ���µļ�����ȫ�Ǽ�
//				InvertRect(hdc,&lpPY_Keyboard->py_HitRect);
				hdc = GetDC( hWnd );
				ShowKeyboardrect(hWnd,hdc,lpPY_Keyboard->py_HitRect,NORMALSHOW); // �ָ����¼���״̬
				ReleaseDC( hWnd, hdc );
			}
			lpPY_Keyboard->g_bCursorLeft = TRUE; // ���ü��Ѿ��뿪ԭ�����µļ�
		}
		else
		{ // ��ǰ�ļ��밴�µļ���ͬ
			if (lpPY_Keyboard->g_bCursorLeft == FALSE) // ��û���뿪���µļ�
				return TRUE;
			// ����뿪ԭ�����µļ����ֻص�ԭ���İ���λ��
			if (lpPY_Keyboard->py_lpHitKey->keyValue[0].bVk != QUANJIAO)
			{  // ���µļ�����ȫ�Ǽ�
//				InvertRect(hdc,&lpPY_Keyboard->py_HitRect);
				hdc = GetDC( hWnd );
				ShowKeyboardrect(hWnd,hdc,lpPY_Keyboard->py_HitRect,INVERTSHOW); // ��ʾ��Ϊ���µ�״̬
				ReleaseDC( hWnd, hdc );	
			}
			lpPY_Keyboard->g_bCursorLeft = FALSE; // ���ü���ԭ�����µ�λ��
		}
		return 0;
}

// **************************************************
// ������static int py_DoPAINT( HWND hWnd, HDC hdc )
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ�� ��
// ��������������Ӣ�ļ��̡�
// ����: 
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
	  {  // ��û�м���ͼ����
		  HINSTANCE hInstance;

//		  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		  hInstance = g_hInstance;
		  //g_hPYBitmap = LoadImage( 0, "\\System\\Keyboard1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_NORMALKB), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ��Ӣ�ļ���λͼ
//		  g_hPYBitmap = LoadImage( hInstance, "\\KINGMOS\\PINYINKB.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��Ӣ�ļ���λͼ
		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_PINYIN), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ����д����λͼ
		  if( g_hPYBitmap == 0 )
			return 0;  // װ��ʧ��
	  }
*/
	  LoadPYKeyboardImage(hWnd,FALSE);
	  KB_DrawBitMap(hdc,(HBITMAP)g_hPYBitmap,rect,0,iStarty,SRCCOPY); // ���Ƽ���
//	  DrawCaseState(hdc ,py_KeyBroad_Case);
//	  if (!IsRectEmpty(&lpPY_Keyboard->py_HitRect))
//		 InvertRect(hdc,&lpPY_Keyboard->py_HitRect);
//	  DeleteObject(hBitMap);
//	  DrawSystemKey(hWnd,hdc,VK_SHIFT); // ����SHIFT��״̬
//	  DrawSystemKey(hWnd,hdc,VK_CAPITAL); // ����CAPS��״̬
	  DrawSystemKey(hWnd,hdc,PINYIN_KB);  // ����YINGWEN_KB����״̬
//	  DrawSystemKey(hWnd,hdc,QUANJIAO);  // ����QIANJIAO����״̬
	  ClearSelectArea(hWnd);
	  ShowSelectArea(hWnd); // ��ʾѡ������
	  return 0;
}
// **************************************************
// ������static BOOL py_GetKeyValue(HWND hWnd, WORD * lpKey )
// ������
// 	OUT lpKey -- �õ���ǰ������ֵ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰ������ֵ��
// ����: 
// **************************************************
static BOOL py_GetKeyValue(HWND hWnd, WORD * lpKey )
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;
		if( lpPY_Keyboard->py_lpHitKey )
		{  // �м�����
			*lpKey = lpPY_Keyboard->py_lpHitKey->keyValue[0].bChar; // ���ص�ǰ������ֵ
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
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

		GetVirtualKeyRect(bVk,&rect);
		if (bVk == QUANJIAO)
		{
			if (lpPY_Keyboard->g_bSBCCase == TRUE)
				ShowKeyboardrect(hWnd,hdc,rect,INVERTSHOW); // ��ʾ��Ϊ���µ�״̬
			else
				ShowKeyboardrect(hWnd,hdc,rect,NORMALSHOW); // ��ʾ��Ϊ���µ�״̬
		}
		if (bVk == PINYIN_KB)
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

/*	  if (g_hPYBitmap == NULL)
	  {  // ��û�м���ͼ����
		  HINSTANCE hInstance;

//		  hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		  hInstance = g_hInstance;
		  //g_hPYBitmap = LoadImage( 0, "\\System\\Keyboard1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
//		  g_hPYBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_NORMALKB), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ��Ӣ�ļ���λͼ
		  g_hPYBitmap = LoadImage( hInstance, "\\KINGMOS\\PINYINKB.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��Ӣ�ļ���λͼ
		  if( g_hPYBitmap == 0 )
			return ;  // װ��ʧ��
	  }
*/
/*	if (g_hInvertBitmap == NULL)
	{ // û��װ��ѡ��λͼ
		  //g_hInvertBitmap = LoadImage( 0, "\\System\\Keyboard2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  g_hInvertBitmap = LoadImage( (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE(IDB_pySEL), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;// װ��λͼ���
		  if( g_hInvertBitmap == 0 )
			return ;
	}
*/
    LoadPYKeyboardImage(hWnd,FALSE);
	if (dwShowStyle == NORMALSHOW)  // ͨ����ʾ
		KB_DrawBitMap(hdc,(HBITMAP)g_hPYBitmap,rect,rect.left + 0,rect.top,SRCCOPY);
	else // ѡ�񣨷��ף���ʾ
		KB_DrawBitMap(hdc,(HBITMAP)g_hInvertBitmap,rect,rect.left + 0,rect.top,SRCCOPY);
}


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
	int iDrawNum,iDrawIndex;
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

		if (ValidData(hWnd) == FALSE)
			return ; // û����Ҫѡ��ļ�
		hdc = GetDC(hWnd); // �õ��豸���

		rect.top = g_RowHeightList[0];  // �õ�ѡ������Ĵ�ֱλ��
		rect.bottom = g_RowHeightList[1] - 1;

		// �Ѿ�ƴд���������ʾ���
		iDrawIndex = lpPY_Keyboard->py_SpellResult.iCurPage * SELECT_NUM; // �õ���ʼ��ʾ������
		if ((iDrawIndex + SELECT_NUM) > lpPY_Keyboard->py_SpellResult.iSpellResultNum)
			iDrawNum = lpPY_Keyboard->py_SpellResult.iSpellResultNum; // ��ǰ��������
		else
			iDrawNum = SELECT_NUM;  // ��ǰ������

		SetTextColor(hdc,RGB(0,0,0));
//		SetBkColor(hdc,RGB(231,242,255));
		SetBkColor(hdc,RGB(255,255,255));

		for (i = 0 ; i < iDrawNum ; i ++)
		{
			rect.left = g_ColumnWidthList[i];
			rect.right = g_ColumnWidthList[i + 1] - 2;
			DrawText(hdc, (char *)&lpPY_Keyboard->py_SpellResult.lpSpellResult[i + iDrawIndex],2,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);  // 2 -- һ������
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
//	HPEN hPen;
//	HBRUSH hBrush;
	// ����һ��������ѡ������

		if (g_hPYBitmap)
		{
			hdc = GetDC(hWnd); // �õ��豸���

	//		rect.top = 1;  // �õ�ѡ������Ĵ�ֱλ��
	//		rect.bottom = 30;
			rect.left = g_ColumnWidthList[0]; 
			rect.right = g_ColumnWidthList[SELECT_NUM]; 
			rect.top = g_RowHeightList[0];  // �õ�ѡ������Ĵ�ֱλ��
			rect.bottom = g_RowHeightList[1] -1 ;
		
			KB_DrawBitMap(hdc,(HBITMAP)g_hPYBitmap,rect,rect.left,rect.top,SRCCOPY); // ���Ƽ���
			// �����ͷ
			DrawArrow(hWnd,hdc,LEFTARROW,NORMALSHOW);
			// ���Ҽ�ͷ
			DrawArrow(hWnd,hdc,RIGHTARROW,NORMALSHOW);
				
			ReleaseDC(hWnd,hdc);
		}
		return ;
/*
		hBrush = CreateSolidBrush(RGB(231,242,255));
//		FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH)); // ���ѡ������
		FillRect(hdc,&rect,hBrush); // ���ѡ������
		DeleteObject(hBrush);
	
		// �����ͷ
		DrawArrow(hWnd,hdc,LEFTARROW,NORMALSHOW);
		// ���Ҽ�ͷ
		DrawArrow(hWnd,hdc,RIGHTARROW,NORMALSHOW);

		// ��ÿһ������ķָ���
		hPen = CreatePen(PS_DASH,1,RGB(0,0,0));  // ����һ�����ı�
		hPen = (HPEN)SelectObject(hdc,hPen);  // ѡ��ʵ��豸

		for ( i = 1; i < SELECT_NUM ; i++)
		{
			MoveToEx(hdc,g_ColumnWidthList[i],rect.top,NULL); 
			LineTo(hdc,g_ColumnWidthList[i],rect.bottom);  // ������
		}

		hPen = (HPEN)SelectObject(hdc,hPen);  // �ָ�ԭ����
		DeleteObject(hPen); // ɾ�������ı�

		ReleaseDC(hWnd,hdc);  // �ͷ��豸���
*/
}


// **************************************************
// ������static int GetSelectIndex(HWND hWnd,int x,int y)
// ������
// 	IN x -- x ����
// 	IN y -- y ����
// 
// ����ֵ������ָ���������ڵ�����
// �����������õ���ǰ���ѡ�����������ֵ��
// ����: 
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
			// �ʵ㵽���ͷ
			if (lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable)  // ���ͷ��Ч
				return LEFTARROW;
			return -1; // ���ͷ��Ч
		}
//		else if ( x > RIGHTARROWSTART)
		else if (PtInRect(&g_rectRightArrow,pt))
		{
			// �ʵ㵽�Ҽ�ͷ
			if (lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable) // �Ҽ�ͷ��Ч
				return RIGHTARROW;
			return -1;  // �Ҽ�ͷ��Ч
		}
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
	
	if (lpPY_Keyboard->g_bSBCCase == FALSE)
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
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;
		if (lpKey == NULL)
			return FALSE; // û��ѡ��� 
		if (iIndex < 0 || iIndex >= lpKey->iKeyNum)
		{
			// ���������
			return FALSE;
		}
		
		if (lpPY_Keyboard->g_bSBCCase == FALSE)
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
// ������static BOOL GetVirtualKeyRect(BYTE bVk, RECT *lprect)
// ������
// 	IN bVk -- ָ�����
// 	OUT lprect -- ���ظ������λ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ�ָ�������λ�á�
// ����: 
// **************************************************
static BOOL GetVirtualKeyRect(BYTE bVk, RECT *lprect)
{
	int iRow=0,iColumn=0;
	int i;

		for (iRow = 0; iRow < ROW_NUM; iRow ++)
		{ // �õ������������һ��
			iColumn=0;
			while(1)
			{ // �õ�����ļ���λ��

				for ( i = 0; i< g_pykeys[iRow][iColumn].iKeyNum; i++)
				{ // ÿһ�����Ͽ����ж����ֵ
					if ( bVk == g_pykeys[iRow][iColumn].keyValue[0].bVk)
					{ // ��ǰ�ļ��������
						if (lprect != NULL)
						{ // ָ����ָ����Ч���õ�����λ��
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
					break; // �Ѿ������һ����
			}
		}
		return FALSE; // û���ҵ�ָ���ļ�
}


// **************************************************
// ������static void ClearSpellResult(HWND hWnd,LPSPELLRESULT lpSpellResult)
// ������
// 	IN/OUT lpSpellResult -- ��Ҫ�����ƴ������ṹ
// 
// ����ֵ����
// ��������������ƴ������ṹ��
// ����: 
// **************************************************
static void ClearSpellResult(HWND hWnd,LPSPELLRESULT lpSpellResult)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;

		// ��������Ϊѡ����һ��ƴд����Ч

		memset(lpSpellResult->lpSpellResult , 0 ,MAX_PYRESULT);
		lpSpellResult->iSelectIndex = 0;
		lpSpellResult->iSpellResultNum = 0;  // û��ƴд���
		lpSpellResult->iCurPage = 0;

		lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable = FALSE;  // ���ͷ��Ч
		lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = FALSE; // �Ҽ�ͷ��Ч
		lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed = FALSE; // ���ͷ����
		lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed = FALSE; // �Ҽ�ͷ����

		// �����ͷ
	//	DrawArrow(hWnd,hdc,LEFTARROW,NORMALSHOW);
		// ���Ҽ�ͷ
	//	DrawArrow(hWnd,hdc,RIGHTARROW,NORMALSHOW);
				
	//	RETAILMSG(1,(TEXT("Clear Arrow Status \r\n")));

}


// **************************************************
// ������static void SetSelAreaStatus(HWND hWnd,LPSPELLRESULT lpSpellResult)
// ������
//	IN lpSpellResult -- ��ǰ��ƴд���
// ����ֵ����
// �������������ݵ�ǰ��ƴд�������ѡ�������״̬��
// ˵��:
//     1����ƴд״̬ ����û��ƴд�������
//			a.û���κ�ƴд������Ҫ��ͷ
//			b.�ַ�С��4������ƴ����Ŀ����6����Ҫ��ͷ
//			c.�ַ�Ϊ4����	��ƴ����Ŀ����4����Ҫ��ͷ
//			d.�ַ�����4����	��ƴ����Ŀ����3����Ҫ��ͷ
//     2����ƴд���״̬����ƴд�������6��ʱ��Ҫ��ͷ
// ����: 
// **************************************************
static void SetSelAreaStatus(HWND hWnd,LPSPELLRESULT lpSpellResult)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;
		// �Ѿ�ƴд�����
		 if (lpSpellResult->iSpellResultNum > SELECT_NUM)
		 {
			if (lpSpellResult->iCurPage == 0)
			{
				lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable = FALSE;  // ���ͷ��Ч
				lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = TRUE; // �Ҽ�ͷ��Ч
			}
			else
			{
				lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable = TRUE;  // ���ͷ��Ч
				
				if (lpSpellResult->iSpellResultNum > ((lpSpellResult->iCurPage + 1) * SELECT_NUM))   // SELECT_NUM -- �м�ͷ��һ��ֻ����ʾ5������
					lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = TRUE; // ���г���һ�������ݣ��Ҽ�ͷ��Ч
				else
					lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = FALSE; // û��һ�������ݣ��Ҽ�ͷ��Ч
			}
		 }
		 else
		 {
				lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable = FALSE;  // ���ͷ��Ч
				lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable = FALSE; // �Ҽ�ͷ��Ч
		 }
}

// **************************************************
// ������static void SetSelAreaStatus(HWND hWnd,LPSPELLRESULT lpSpellResult)
// ������
//	IN hWnd -- ���ھ��
//  IN iIndex -- ѡ������
// ����ֵ����
// ����������ѡ�����롣
// ����: 
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
		// �Ѿ�ƴд�����
		if (iIndex == LEFTARROW)
		{
			// ���ͷ
			if (lpPY_Keyboard->py_SpellResult.iCurPage > 0)
			{
				lpPY_Keyboard->py_SpellResult.iCurPage --; // ����ǰһҳ
				lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed = TRUE;
//				RETAILMSG(1,(TEXT("Left Arrow Down \r\n")));
				hdc = GetDC( hWnd );
				DrawArrow(hWnd,hdc,LEFTARROW,0); // ����ͷ
				ReleaseDC( hWnd, hdc );
				SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // ����ѡ������
				ClearSelectArea(hWnd); // ���ѡ������
				ShowSelectArea(hWnd); // ��ʾѡ������
			}
			return ;
		}
		else if (iIndex == RIGHTARROW)
		{
			// �Ҽ�ͷ
			if (((lpPY_Keyboard->py_SpellResult.iCurPage + 1) * SELECT_NUM) < lpPY_Keyboard->py_SpellResult.iSpellResultNum)
			{  // ����������
				lpPY_Keyboard->py_SpellResult.iCurPage ++; // ������һҳ
				lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed = TRUE;
//				RETAILMSG(1,(TEXT("Right Arrow Down \r\n")));
				hdc = GetDC( hWnd );
				DrawArrow(hWnd,hdc,RIGHTARROW,0); // ����ͷ
				ReleaseDC( hWnd, hdc );
				SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // ����ѡ������
				ClearSelectArea(hWnd); // ���ѡ������
				ShowSelectArea(hWnd); // ��ʾѡ������
			}
			return ;
		}
		// �㵽ѡ����
//		RETAILMSG(1,(TEXT(" Enter Select a Word\r\n")));
		iSelectIndex = lpPY_Keyboard->py_SpellResult.iCurPage * SELECT_NUM + iIndex;

		if (iSelectIndex <lpPY_Keyboard->py_SpellResult.iSpellResultNum)
		{
			// �㵽�н�������򣬿�ʼ������
			BYTE bChar;
			WORD wOutput;

			wOutput = lpPY_Keyboard->py_SpellResult.lpSpellResult[iSelectIndex]; //  �õ�Ҫ����ĺ���
			bChar = (BYTE)wOutput; // �������λ
			keybd_event(bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ

			bChar = (BYTE)(wOutput >> 8); // �������λ
			keybd_event(bChar,0,KEYEVENTF_CHAR,0); // ����һ���ַ���Ϣ

//			RETAILMSG(1,(TEXT(" Had Output a Word\r\n")));

			// ���ѡ����
			ClearSpellResult(hWnd,&lpPY_Keyboard->py_SpellResult); // ���ƴд����е�����
//			RETAILMSG(1,(TEXT(" ClearSpellResult OK !!!\r\n")));
			SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // ����ѡ������
//			RETAILMSG(1,(TEXT(" SetSelAreaStatus OK !!!\r\n")));

			// ���ƴд��
			SendMessage(lpPY_Keyboard->g_hSpell,SPM_CLEARSPELL,0,0);
//			RETAILMSG(1,(TEXT(" SPM_CLEARSPELL OK !!!\r\n")));

			// ���빦��
			lpPY_Keyboard->py_SpellResult.iSpellResultNum = DoPYLXSearch(wOutput,lpPY_Keyboard->py_SpellResult.lpSpellResult,MAX_PYRESULT);
//			RETAILMSG(1,(TEXT(" Get LX OK !!!\r\n")));
			
			SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // ����ѡ������
//			RETAILMSG(1,(TEXT(" SetSelAreaStatus OK !!!\r\n")));
			ClearSelectArea(hWnd); // ���ѡ������
//			RETAILMSG(1,(TEXT(" ClearSelectArea OK !!!\r\n")));
			ShowSelectArea(hWnd); // ��ʾѡ������
//			RETAILMSG(1,(TEXT(" ShowSelectArea OK !!!\r\n")));


		}
		return;
}

// **************************************************
// ������static BOOL ValidData(HWND hWnd)
// ������
// 
// ����ֵ����ǰƴд���������Ч���ݣ�����TRUE�����򷵻�FALSE
// ������������鵱ǰ��ƴд������Ƿ�����Ч���ݡ�
// ����: 
// **************************************************
static BOOL ValidData(HWND hWnd)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return FALSE;
	
		if (lpPY_Keyboard->py_SpellResult.iSpellResultNum > 0)
		{
			// ��ƴ�����������Ч���ݣ� ��������������
			return TRUE;
		}

		return FALSE; // û����Ч����
}


// **************************************************
// ������static void GoBackDeal(HWND hWnd)
// ������
//	IN hWnd -- ���ھ��
// ����ֵ����
// �����������ص���һ��������
// ����: 
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
// ������void DrawArrow(HWND hWnd,HDC hdc,UINT iArrowType,UINT iArrowStatus)
// ������
//	IN hWnd -- ���ھ��
//	IN hdc -- �豸���
//  IN iArrowType -- Ҫ���Ƶļ�ͷ�����ͣ�LEFTARROW ���� RIGHTARROW
//  IN iArrowStatus -- Ҫ���Ƶļ�ͷ��״̬�� INVERTSHOW ���� NORMALSHOW
// ����ֵ����
// �������������Ƽ�ͷ��
// ����: 
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
	{  // �����ͷ
		// �õ����ͷ�ľ���λ��
//		rect.top = 1-1;  // �õ�ѡ������Ĵ�ֱλ��
//		rect.bottom = 30+1;
//		rect.left = LEFTARROWSTART-1; 
//		rect.right = LEFTARROWEND+1; 
		if (lpPY_Keyboard->py_SelAreaStatus.bLeftArrowEnable == FALSE)
		{
			// ���ͷ��DISABLE��
//			cl_Text = CL_LIGHTGRAY; // �ı���ǳ��ɫ
//			cl_Bk = RGB(0XC0,0XC0,0XC0); // ��ɫ�ǻ�ɫ
			if (g_hDisableArrow == NULL)
			{
//				HINSTANCE hInstance;

//				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				g_hDisableArrow = LoadImage( g_hInstance, MAKEINTRESOURCE(IDB_ARROWDIS), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ����д����λͼ
			}
			KB_DrawBitMap(hdc,(HBITMAP)g_hDisableArrow,g_rectLeftArrow,1,g_rectLeftArrow.top,SRCCOPY);
		}
		else
		{
			// ���ͷ��ENABLE
			if (lpPY_Keyboard->py_SelAreaStatus.bLeftArrowPressed)
			{  // ���ͷ����
//				cl_Text = CL_BLACK;  // �׵׺���
//				cl_Bk = CL_WHITE;
				ShowKeyboardrect(hWnd,hdc,g_rectLeftArrow,INVERTSHOW); // �ָ�ԭ����������ʾ

			}
			else
			{
//				cl_Text = CL_BLACK;  // �ҵ׺���
//				cl_Bk = RGB(0XC0,0XC0,0XC0); // ��ɫ�ǻ�ɫ
				ShowKeyboardrect(hWnd,hdc,g_rectLeftArrow,NORMALSHOW); // �ָ�ԭ����������ʾ
			}
		}
/*
//		hPen = CreatePen(PS_SOLID,1,cl_Text);
		hBrush = CreateSolidBrush(cl_Bk); // ��������ˢ

//		hPen = SelectObject(hdc,hPen);
		hBrush = SelectObject(hdc,hBrush); // ѡ�񱳾�ˢ
		SetTextColor(hdc,cl_Text); // �����ı���ɫ
		SetBkColor(hdc,cl_Bk); // ���ñ�����ɫ

		Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom); // ���Ʊ߿����ɫ
		InflateRect(&rect,-2,-2); // �ı����αȱ߿�С2��
		DrawText(hdc,"<",1,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE); // �������ͷ

//		hPen = SelectObject(hdc,hPen);
		hBrush = SelectObject(hdc,hBrush); // �ָ�����ˢ

//		DeleteObject(hPen);
		DeleteObject(hBrush); // ɾ��ˢ��
*/
	}
	else
	{
		// �õ��Ҽ�ͷ�ľ���λ��
//		rect.top = 1-1;  // �õ�ѡ������Ĵ�ֱλ��
//		rect.bottom = 30+1;
//		rect.left = RIGHTARROWSTART-1; 
//		rect.right = RIGHTARROWEND+1; 
		if (lpPY_Keyboard->py_SelAreaStatus.bRightArrowEnable == FALSE)
		{
			// �Ҽ�ͷ��DISABLE��
//			cl_Text = CL_LIGHTGRAY; // �ı���ǳ��ɫ
//			cl_Bk = RGB(0XC0,0XC0,0XC0); // ��ɫ�ǻ�ɫ
			if (g_hDisableArrow == NULL)
			{
//				HINSTANCE hInstance;

//				hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
				g_hDisableArrow = LoadImage( g_hInstance, MAKEINTRESOURCE(IDB_ARROWDIS), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ����д����λͼ
			}
			KB_DrawBitMap(hdc,(HBITMAP)g_hDisableArrow,g_rectRightArrow,1,g_rectRightArrow.top,SRCCOPY);
		}
		else
		{
			// �Ҽ�ͷ��ENABLE
			if (lpPY_Keyboard->py_SelAreaStatus.bRightArrowPressed)
			{  // �Ҽ�ͷ����
//				cl_Text = CL_BLACK;  // �׵׺���
//				cl_Bk = CL_WHITE;
				ShowKeyboardrect(hWnd,hdc,g_rectRightArrow,INVERTSHOW); // �ָ�ԭ����������ʾ
			}
			else
			{
//				cl_Text = CL_BLACK;  // �ҵ׺���
//				cl_Bk = RGB(0XC0,0XC0,0XC0); // ��ɫ�ǻ�ɫ
				ShowKeyboardrect(hWnd,hdc,g_rectRightArrow,NORMALSHOW); // �ָ�ԭ����������ʾ
			}
		}
/*
//		hPen = CreatePen(PS_SOLID,1,cl_Text);
		hBrush = CreateSolidBrush(cl_Bk); // ��������ˢ

//		hPen = SelectObject(hdc,hPen);
		hBrush = SelectObject(hdc,hBrush); // ѡ�񱳾�ˢ
		SetTextColor(hdc,cl_Text); // �����ı���ɫ
		SetBkColor(hdc,cl_Bk); // ���ñ�����ɫ

		Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom); // ���Ʊ߿����ɫ
		InflateRect(&rect,-2,-2); // �ı����αȱ߿�С2��
		DrawText(hdc,">",1,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE); // �������ͷ

//		hPen = SelectObject(hdc,hPen);
		hBrush = SelectObject(hdc,hBrush); // �ָ�����ˢ

//		DeleteObject(hPen);
		DeleteObject(hBrush); // ɾ��ˢ��
*/
	}

}
// **************************************************
// ������static void InputKey(HWND hWnd,KEYENTRY *lpKey)
// ������
//	IN hWnd -- ���ھ��
//  IN lpKey --����ļ�
// ����ֵ����
// �������������Ƽ�ͷ��
// ����: 
// **************************************************
static void InputKey(HWND hWnd,KEYENTRY *lpKey)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return;
		PostMessage(lpPY_Keyboard->g_hSpell,SPM_INSERTKEY,0,(LPARAM)lpKey); // ����һ������SPELL����
		return;
}


// ********************************************************************
// ������static LRESULT DoSpellSelChange(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam -- �ı���ѡ������
//  IN lParam -- ����
// ����ֵ��
//	
// ����������ƴд����ѡ�����ı�
// ����: 
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
			// �ɹ��õ�ƴд�ַ������ڿ�ʼ����ƴд���õ�����
			memset( lpPY_Keyboard->py_SpellResult.lpSpellResult , 0 , MAX_PYRESULT * sizeof(WORD) ); // ��ս������
			lpPY_Keyboard->py_SpellResult.iSpellResultNum = PYSpell(lpSpell,lpPY_Keyboard->py_SpellResult.lpSpellResult,MAX_PYRESULT);
			
			lpPY_Keyboard->py_SpellResult.iCurPage = 0;  // ���õ���һҳ
			SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // ����ѡ������
			ClearSelectArea(hWnd); // ���ѡ������
			ShowSelectArea(hWnd); // ��ʾѡ������

		}
		return 0;
}



// ********************************************************************
// ������static LRESULT DoSpellNoData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam -- ����
//  IN lParam -- ����
// ����ֵ��
//	
// ����������ƴд�����Ѿ�û������
// ����: 
// ********************************************************************
static LRESULT DoSpellNoData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPY_KEYBOARD  lpPY_Keyboard;

		lpPY_Keyboard = (LPPY_KEYBOARD)GetWindowLong(hWnd,PY_PRIVATEDATA);
		if (lpPY_Keyboard == NULL)
			return 0;

		lpPY_Keyboard->py_SpellResult.iSpellResultNum = 0;  // ����ƴд���Ϊ0

		SetSelAreaStatus(hWnd,&lpPY_Keyboard->py_SpellResult); // ����ѡ������
		ClearSelectArea(hWnd); // ���ѡ������
		return 0;
}

// **************************************************
// ������static BOOL LoadPYKeyboardImage(HWND hWnd, BOOL bReLoad)
// ������
//	IN hWnd -- ���ھ��
//	IN bReLoad -- �Ƿ���Ҫ����װ�� 
// ����ֵ���ɹ�״̬����TRUE�����򷵻�FALSE
// ����������װ�ص�ǰ�ļ���ͼ��
// ����: 
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
		{  // ��û�м���ͼ����
		  g_hPYBitmap = LoadImage( 0, lpPYKBImageFileName[iStyleIndex][0], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hPYBitmap == 0 )
			return FALSE;  // װ��ʧ��
		}
		if (g_hInvertBitmap == NULL)
		{  // ��û�м���ͼ����
		  g_hInvertBitmap = LoadImage( 0, lpPYKBImageFileName[iStyleIndex][1], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ;
		  if( g_hInvertBitmap == 0 )
			return FALSE;  // װ��ʧ��
		}

		return TRUE;
}
