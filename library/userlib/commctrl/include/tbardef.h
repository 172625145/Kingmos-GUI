#ifndef _H_TBARDEF_2001_11_19_
#define _H_TBARDEF_2001_11_19_

#define TBBM_SETDATA				( WM_USER + 20 )
#define TBBM_GETDATA				( WM_USER + 30 )

typedef struct {
//    UINT cbSize;
//    DWORD dwMask;
    int idCommand;
    int iImage;
    BYTE fsState;
    BYTE fsStyle;
    WORD cx;
    DWORD lParam;
    TCHAR* pszText;
    int cchText;
} _TBBUTTONDATA, *_LPTBBUTTONDATA;

typedef struct _tag_TOOLBARDATA{
	LPPTRLIST			lpButtonList;
//	LPPTRLIST			hImgList;
//	LPPTRLIST			hDisImgList;
	HIMAGELIST			hImgList;
	HIMAGELIST			hDisImgList;
	LPPTRLIST			lpStringList;
//	LPPTRLIST			lpSystemImages ;
//================================================================
//	TBINSERTMARK*		InsertMark;//the Insertmark define below
	DWORD				dwExStyle;//Extended style
	DWORD				dwStyle;//the style of the toolbar windows
//	DWORD				dwDTFlags ;//the draw text style
//	DWORD				dwMask; //the mask of the DTFlags
//	DWORD				dwBpFlags;//the Bitmap Flags
//	INT					iHot;//the hot button
	int					iIndent;//the indentation of the first button in the toolbar
	int					iMaxRows ;//max row of the tool bar,it may EQV buttons or one
	WORD				cRows ;// the row of the toolbar
	WORD				cx;//the padding of the toolbar
	WORD				cy;//the padding of the toolbar
	WORD				dxButton;//the width of the button
	WORD				dyButton;//the hight of the button
	WORD				dxBitmap ;//the width of the button's image
	WORD				dyBitmap ;//the hight of the button's image
	WORD                dump;
	COLORREF            cl_Bk;  //±³¾°ÑÕÉ«
//================================================================
}_TOOLBARDATA,*_LPTOOLBARDATA;

typedef struct _tag_TBBUTTONSTRUCT{
	HWND				hWnd;
	int					iCommand;
}_TOOLBARBUTTON,*_LPTOOLBARBUTTON;

BOOL SetTBButtonInfo(HWND hWnd,LPTBBUTTONINFO lpti);
BOOL GetTBButtonInfo(HWND hWnd,LPTBBUTTONINFO lpti);
HWND CreateTBButton(HWND hWnd,LPTBBUTTON lpbt,RECT	rt);
ATOM RegisterTBButtonClass(HINSTANCE hInstance);

#endif //_H_TBARDEF_2001_11_19_
