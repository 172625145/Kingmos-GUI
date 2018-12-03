#ifndef __H_EXPLORE__2001_9_19_
#define __H_EXPLORE__2001_9_19_

extern const char classEXPLORE[];

#define	EXPM_SETITEM					( WM_USER + 1 )
#define EXPM_GETITEM					( WM_USER + 2 )
#define EXPM_UPDATE						( WM_USER + 3 )
#define EXPM_GETLISTVIEWITEM			( WM_USER + 4 )

#define EXP_AP									1
#define EXP_UPTOOLBAR							2
#define EXP_DOWNTOOLBAR							3
#define EXP_LISTVIEW							4
#define EXP_EXTEND								5
#define EXP_INITDIR								6
#define EXP_CURRENTDIR							7


//================================================
#define IDC_ICONTOOLBARUP							399
#define IDC_ICONTOOLBARDOWN							400
#define IDC_LISTVIEW								401

#define   ITEM_INTERVAL_X					25
#define   ITEM_INTERVAL_Y					10
#define   MAX_EXTEND_NAME					10
#define   MAX_NAME							100
#define   MAX_AP_NAME						10

#define   FILE_SORT_NAME					0
#define   FILE_SORT_SIZE					1
#define   FILE_SORT_TYPE					2
#define   FILE_SORT_TIME					3


#define EXPS_OWNERDRAW				0x00000001L
#define EXPS_OPENEXPLORE			0x00000002L
#define EXPS_LISTVIEWICON			0x00000004L

//the user use WM_COMMAND wParam must biger than 100 ;
//for debug
ATOM	RegistFileExplore(HINSTANCE hInstance);
HWND	CreateFileExplore(HWND hWnd,DWORD style,TCHAR *lpstrAp,TCHAR *lpstrExtend,TCHAR *lpstrInitDir);
#endif //__H_EXPLORE__2001_9_19_
