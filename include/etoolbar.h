/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __TOOLBAR_H
#define __TOOLBAR_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

//#define TOOLBAR_HIGH			36
#define TOOLBAR_HIGH			20
#define TB_LEFTBUTTON_WIDTH		18
//==============================================================
typedef int						INT;

#define TBN_FIRST               (0U-700U)
#define TBN_LAST                (0U-720U)

#define CCS_TOP                 0x00000001L
#define CCS_NOMOVEY             0x00000002L
#define CCS_BOTTOM              0x00000003L
#define CCS_NORESIZE            0x00000004L
#define CCS_NOPARENTALIGN       0x00000008L
#define CCS_SYSTEMCOLOR			0x00000010L
#define CCS_ADJUSTABLE          0x00000020L
#define CCS_NODIVIDER           0x00000040L
#define CCS_VERT                0x00000080L
#define CCS_LEFT                (CCS_VERT | CCS_TOP)
#define CCS_RIGHT               (CCS_VERT | CCS_BOTTOM)
#define CCS_NOMOVEX             (CCS_VERT | CCS_NOMOVEY)

#define CCS_WHITECOLOR			0x00000100L

#define CCS_ICONIMAGE			0x00000200L  // 使用ICON


//==============================================================
typedef struct _TBBUTTON {
    int iBitmap;
    DWORD idCommand;
    BYTE fsState;
    BYTE fsStyle;
    BYTE bReserved[2];
    DWORD dwData;
    int iString;
} TBBUTTON,*PTBBUTTON,*LPTBBUTTON;
typedef struct {
    int   iButton;
    DWORD dwFlags;
} TBINSERTMARK, * LPTBINSERTMARK;

typedef struct _COLORMAP {
    COLORREF from;
    COLORREF to;
} COLORMAP, FAR* LPCOLORMAP;

typedef struct _LEFTBMPDATA {
        DWORD			cbSize;
        HWND			hWnd;
		TCHAR			szApp[20];
        UINT			uID;
        UINT			uFlags;
        UINT			uCallbackMessage;
        HBITMAP			hBmp;
		UINT			uWidth;
} LEFTBMPDATA, *PLEFTBMPDATA;
//==============================================================
 HWND WINAPI CreateToolbarEx(HWND hwnd, DWORD ws, UINT wID, int nBitmaps,
                        HINSTANCE hBMInst, UINT wBMID, TBBUTTON* lpButtons,
                        int iNumButtons, int dxButton, int dyButton,
                        int dxBitmap, int dyBitmap, UINT uStructSize);

 HBITMAP WINAPI CreateMappedBitmap(HINSTANCE hInstance, int idBitmap,
                                  UINT wFlags, LPCOLORMAP lpColorMap,
                                  int iNumMaps);

#define CMB_MASKED              0x02

#define TBSTATE_CHECKED         0x01
#define TBSTATE_PRESSED         0x02
#define TBSTATE_ENABLED         0x04
#define TBSTATE_HIDDEN          0x08
#define TBSTATE_INDETERMINATE   0x10
#define TBSTATE_WRAP            0x20
#define TBSTATE_ELLIPSES        0x40
#define TBSTATE_MARKED          0x80

#define TBSTYLE_BUTTON          0x0000
#define TBSTYLE_SEP             0x0001
#define TBSTYLE_CHECK           0x0002
#define TBSTYLE_GROUP           0x0004
#define TBSTYLE_CHECKGROUP      (TBSTYLE_GROUP | TBSTYLE_CHECK)
#define TBSTYLE_DROPDOWN        0x0008
#define TBSTYLE_AUTOSIZE        0x0010 
#define TBSTYLE_NOPREFIX        0x0020 

#define BTNS_SHOWTEXT			0x0040
#define BTNS_RIGHTBUTTON		0x0080

#define TBSTYLE_TOOLTIPS        0x0100
#define TBSTYLE_WRAPABLE        0x0200
#define TBSTYLE_ALTDRAG         0x0400
#define TBSTYLE_FLAT            0x0800
#define TBSTYLE_LIST            0x1000
#define TBSTYLE_CUSTOMERASE     0x2000
#define TBSTYLE_REGISTERDROP    0x4000
#define TBSTYLE_TRANSPARENT     0x8000
#define TBSTYLE_EX_DRAWDDARROWS 0x00000001
// Custom Draw Structure
typedef struct _NMTBCUSTOMDRAW {
    NMCUSTOMDRAW nmcd;
    HBRUSH hbrMonoDither;
    HBRUSH hbrLines;
    HPEN hpenLines;

    COLORREF clrText;
    COLORREF clrMark;
    COLORREF clrTextHighlight;
    COLORREF clrBtnFace;
    COLORREF clrBtnHighlight;
    COLORREF clrHighlightHotTrack;
                                  
    RECT rcText;                  

    int nStringBkMode;
    int nHLStringBkMode;
} NMTBCUSTOMDRAW, * LPNMTBCUSTOMDRAW;


#define TBCDRF_NOEDGES              0x00010000  
#define TBCDRF_HILITEHOTTRACK       0x00020000  
#define TBCDRF_NOOFFSET             0x00040000  
#define TBCDRF_NOMARK               0x00080000  
#define TBCDRF_NOETCHEDEFFECT       0x00100000  

#define TB_ENABLEBUTTON         (WM_USER + 1)
#define TB_CHECKBUTTON          (WM_USER + 2)
#define TB_PRESSBUTTON          (WM_USER + 3)
#define TB_HIDEBUTTON           (WM_USER + 4)
#define TB_INDETERMINATE        (WM_USER + 5)
#define TB_MARKBUTTON           (WM_USER + 6)
#define TB_GETSTRING			(WM_USER+7)
#define TB_ISBUTTONENABLED      (WM_USER + 9)
#define TB_ISBUTTONCHECKED      (WM_USER + 10)
#define TB_ISBUTTONPRESSED      (WM_USER + 11)
#define TB_ISBUTTONHIDDEN       (WM_USER + 12)
#define TB_ISBUTTONINDETERMINATE (WM_USER + 13)
#define TB_ISBUTTONHIGHLIGHTED  (WM_USER + 14)
#define TB_SETSTATE             (WM_USER + 17)
#define TB_GETSTATE             (WM_USER + 18)
#define TB_ADDBITMAP            (WM_USER + 19)
typedef struct tagTBADDBITMAP {
        int             hInst;
        UINT            nID;
		LPPTRLIST		hImageList;
} TBADDBITMAP, *LPTBADDBITMAP;

#define HINST_COMMCTRL          ((HINSTANCE)-1)
#define IDB_STD_SMALL_COLOR     0
#define IDB_STD_LARGE_COLOR     1
#define IDB_VIEW_SMALL_COLOR    4
#define IDB_VIEW_LARGE_COLOR    5
#define IDB_HIST_SMALL_COLOR    8
#define IDB_HIST_LARGE_COLOR    9


#define HIST_ADDTOFAVORITES             0
#define HIST_BACK                       1
#define HIST_FAVORITES                  2
#define HIST_FORWARD                    3
#define HIST_VIEWTREE                   4

#define STD_COPY                        0
#define STD_CUT                         1
#define STD_DELETE                      2
#define STD_FILENEW                     3
#define STD_FILEOPEN                    4
#define STD_FILESAVE                    5
#define STD_FIND                        6
#define STD_HELP                        7
#define STD_PASTE                       8
#define STD_PRINT                       9
#define STD_PRINTPRE                    10
#define STD_PROPERTIES                  11
#define STD_REDOW                       12
#define STD_REPLACE                     13
#define STD_UNDO                        14

#define VIEW_LARGEICONS         0
#define VIEW_SMALLICONS         1
#define VIEW_LIST               2
#define VIEW_DETAILS            3
#define VIEW_SORTNAME           4
#define VIEW_SORTSIZE           5
#define VIEW_SORTDATE           6
#define VIEW_SORTTYPE           7
#define VIEW_PARENTFOLDER       8
#define VIEW_NETCONNECT         9
#define VIEW_NETDISCONNECT      10
#define VIEW_NEWFOLDER          11
#define VIEW_VIEWMENU           12

#define TB_ADDBUTTONSA          (WM_USER + 20)
#define TB_INSERTBUTTONA        (WM_USER + 21)

#define TB_DELETEBUTTON         (WM_USER + 22)
#define TB_GETBUTTON            (WM_USER + 23)
#define TB_BUTTONCOUNT          (WM_USER + 24)
#define TB_COMMANDTOINDEX       (WM_USER + 25)

#define TBSAVEPARAMS            TBSAVEPARAMSW
#define LPTBSAVEPARAMS          LPTBSAVEPARAMSW

#define TB_SAVERESTOREA         (WM_USER + 26)
#define TB_SAVERESTOREW         (WM_USER + 76)
#define TB_CUSTOMIZE            (WM_USER + 27)
#define TB_ADDSTRINGA           (WM_USER + 28)
#define TB_ADDSTRINGW           (WM_USER + 77)
#define TB_GETITEMRECT          (WM_USER + 29)
#define TB_BUTTONSTRUCTSIZE     (WM_USER + 30)
#define TB_SETBUTTONSIZE        (WM_USER + 31)
#define TB_SETBITMAPSIZE        (WM_USER + 32)
#define TB_AUTOSIZE             (WM_USER + 33)
#define TB_GETTOOLTIPS          (WM_USER + 35)
#define TB_SETTOOLTIPS          (WM_USER + 36)
#define TB_SETPARENT            (WM_USER + 37)
#define TB_SETROWS              (WM_USER + 39)
#define TB_GETROWS              (WM_USER + 40)
#define TB_SETCMDID             (WM_USER + 42)
#define TB_CHANGEBITMAP         (WM_USER + 43)
#define TB_GETBITMAP            (WM_USER + 44)
#define TB_GETBUTTONTEXTA       (WM_USER + 45)
#define TB_GETBUTTONTEXTW       (WM_USER + 75)
#define TB_REPLACEBITMAP        (WM_USER + 46)
#define TB_SETINDENT            (WM_USER + 47)
#define TB_SETIMAGELIST         (WM_USER + 48)
#define TB_GETIMAGELIST         (WM_USER + 49)
#define TB_LOADIMAGES           (WM_USER + 50)
#define TB_GETRECT              (WM_USER + 51)
#define TB_SETHOTIMAGELIST      (WM_USER + 52)
#define TB_GETHOTIMAGELIST      (WM_USER + 53)
#define TB_SETDISABLEDIMAGELIST (WM_USER + 54)
#define TB_GETDISABLEDIMAGELIST (WM_USER + 55)
#define TB_SETSTYLE             (WM_USER + 56)
#define TB_GETSTYLE             (WM_USER + 57)
#define TB_GETBUTTONSIZE        (WM_USER + 58)
#define TB_SETBUTTONWIDTH       (WM_USER + 59)
#define TB_SETMAXTEXTROWS       (WM_USER + 60)
#define TB_GETTEXTROWS          (WM_USER + 61)

#define TB_GETBUTTONTEXT        TB_GETBUTTONTEXTW
#define TB_SAVERESTORE          TB_SAVERESTOREW
#define TB_ADDSTRING            TB_ADDSTRINGW
#define TB_GETOBJECT            (WM_USER + 62)
#define TB_GETHOTITEM           (WM_USER + 71)
#define TB_SETHOTITEM           (WM_USER + 72)
#define TB_SETANCHORHIGHLIGHT   (WM_USER + 73)
#define TB_GETANCHORHIGHLIGHT   (WM_USER + 74)
#define TB_MAPACCELERATORA      (WM_USER + 78)

#define TBIMHT_AFTER      0x00000001
#define TBIMHT_BACKGROUND 0x00000002

#define TB_GETINSERTMARK        (WM_USER + 79)  
#define TB_SETINSERTMARK        (WM_USER + 80)  
#define TB_INSERTMARKHITTEST    (WM_USER + 81)  
#define TB_MOVEBUTTON           (WM_USER + 82)
#define TB_GETMAXSIZE           (WM_USER + 83)  
#define TB_SETEXTENDEDSTYLE     (WM_USER + 84)  
#define TB_GETEXTENDEDSTYLE     (WM_USER + 85)  
#define TB_GETPADDING           (WM_USER + 86)
#define TB_SETPADDING           (WM_USER + 87)
#define TB_SETINSERTMARKCOLOR   (WM_USER + 88)
#define TB_GETINSERTMARKCOLOR   (WM_USER + 89)

#define TB_SETCOLORSCHEME       CCM_SETCOLORSCHEME  
#define TB_GETCOLORSCHEME       CCM_GETCOLORSCHEME	

#define TB_SETUNICODEFORMAT     CCM_SETUNICODEFORMAT
#define TB_GETUNICODEFORMAT     CCM_GETUNICODEFORMAT

#define TB_MAPACCELERATORW      (WM_USER + 90)  
#define TB_MAPACCELERATOR       TB_MAPACCELERATORW      

typedef struct {
    HINSTANCE       hInstOld;
    UINT            nIDOld;
    HINSTANCE       hInstNew;
    UINT            nIDNew;
    int             nButtons;
} TBREPLACEBITMAP, *LPTBREPLACEBITMAP;

#define TBBF_LARGE              0x0001

#define TB_GETBITMAPFLAGS       (WM_USER + 41)

#define TBIF_IMAGE              0x00000001
#define TBIF_TEXT               0x00000002
#define TBIF_STATE              0x00000004
#define TBIF_STYLE              0x00000008
#define TBIF_LPARAM             0x00000010
#define TBIF_COMMAND            0x00000020
#define TBIF_SIZE               0x00000040
#define TBIF_BYINDEX			0x00000080

typedef struct {
    UINT cbSize;
    DWORD dwMask;
    int idCommand;
    int iImage;
    BYTE fsState;
    BYTE fsStyle;
    WORD cx;
    DWORD lParam;
    TCHAR* pszText;
    int cchText;
} TBBUTTONINFO, *LPTBBUTTONINFO;

#define TB_GETBUTTONINFOW        (WM_USER + 63)
#define TB_SETBUTTONINFOW        (WM_USER + 64)
#define TB_GETBUTTONINFOA        (WM_USER + 65)
#define TB_SETBUTTONINFOA        (WM_USER + 66)

#define TB_GETBUTTONINFO        TB_GETBUTTONINFOW
#define TB_SETBUTTONINFO        TB_SETBUTTONINFOW

#define TB_INSERTBUTTONW        (WM_USER + 67)
#define TB_ADDBUTTONSW          (WM_USER + 68)

#define TB_HITTEST              (WM_USER + 69)
#define TB_ADDLEFTBMP			(WM_USER + 100)
#define TB_MODIFYLEFTBMP		(WM_USER + 101)
#define TB_DELETELEFTBMP		(WM_USER + 102)

#define TB_INSERTBUTTON         TB_INSERTBUTTONW
#define TB_ADDBUTTONS           TB_ADDBUTTONSW

#define TB_SETDRAWTEXTFLAGS     (WM_USER + 70)  

#define TBN_GETBUTTONINFOA      (TBN_FIRST-0)
#define TBN_GETBUTTONINFOW      (TBN_FIRST-20)
#define TBN_BEGINDRAG           (TBN_FIRST-1)
#define TBN_ENDDRAG             (TBN_FIRST-2)
#define TBN_BEGINADJUST         (TBN_FIRST-3)
#define TBN_ENDADJUST           (TBN_FIRST-4)
#define TBN_RESET               (TBN_FIRST-5)
#define TBN_QUERYINSERT         (TBN_FIRST-6)
#define TBN_QUERYDELETE         (TBN_FIRST-7)
#define TBN_TOOLBARCHANGE       (TBN_FIRST-8)
#define TBN_CUSTHELP            (TBN_FIRST-9)

#define TBN_DROPDOWN            (TBN_FIRST - 10)

#define TBN_GETOBJECT           (TBN_FIRST - 12)

typedef struct tagNMTBHOTITEM
{
    NMHDR   hdr;
    int     idOld;
    int     idNew;
    DWORD   dwFlags;
} NMTBHOTITEM, * LPNMTBHOTITEM;


#define HICF_OTHER          0x00000000
#define HICF_MOUSE          0x00000001          
#define HICF_ARROWKEYS      0x00000002          
#define HICF_ACCELERATOR    0x00000004          
#define HICF_DUPACCEL       0x00000008          
#define HICF_ENTERING       0x00000010          
#define HICF_LEAVING        0x00000020          
#define HICF_RESELECT       0x00000040          


#define TBN_HOTITEMCHANGE       (TBN_FIRST - 13)
#define TBN_DRAGOUT             (TBN_FIRST - 14) 
#define TBN_DELETINGBUTTON      (TBN_FIRST - 15) 
#define TBN_GETDISPINFOA        (TBN_FIRST - 16) 
#define TBN_GETDISPINFOW        (TBN_FIRST - 17) 
#define TBN_GETINFOTIPA         (TBN_FIRST - 18)
#define TBN_GETINFOTIPW         (TBN_FIRST - 19)

typedef struct tagNMTBGETINFOTIPW
{
    NMHDR hdr;
    LPWSTR pszText;
    int cchTextMax;
    int iItem;
    LPARAM lParam;
} NMTBGETINFOTIPW, *LPNMTBGETINFOTIPW;

#define TBN_GETINFOTIP          TBN_GETINFOTIPW
#define NMTBGETINFOTIP          NMTBGETINFOTIPW
#define LPNMTBGETINFOTIP        LPNMTBGETINFOTIPW

#define TBNF_IMAGE              0x00000001
#define TBNF_TEXT               0x00000002
#define TBNF_DI_SETITEM         0x10000000

typedef struct {
    NMHDR hdr;
    DWORD dwMask;     
    int idCommand;    
    DWORD lParam;     
    int iImage;       
    LPWSTR pszText;   
    int cchText;      
} NMTBDISPINFOW, *LPNMTBDISPINFOW;

#define TBN_GETDISPINFO       TBN_GETDISPINFOW
#define NMTBDISPINFO	      NMTBDISPINFOW
#define LPNMTBDISPINFO        LPNMTBDISPINFOW

#define TBDDRET_DEFAULT         0
#define TBDDRET_NODEFAULT       1
#define TBDDRET_TREATPRESSED    2       

#define TBN_GETBUTTONINFO       TBN_GETBUTTONINFOW

#define TBNOTIFYW NMTOOLBARW

#define LPTBNOTIFYW LPNMTOOLBARW

#define TBNOTIFY       NMTOOLBAR
#define LPTBNOTIFY     LPNMTOOLBAR

typedef struct tagNMTOOLBARW {
    NMHDR   hdr;
    int     iItem;
    TBBUTTON tbButton;
    int     cchText;
    TCHAR   *pszText;
} NMTOOLBARW, FAR* LPNMTOOLBARW;
#define MAKEPOSTOMASK(CmdPosInAry) ((DWORD)1 << (31 - CmdPosInAry))
typedef struct COMMON_TOOLBAR_CREATESTRUCT{
	DWORD *pCmdAry;
	DWORD *pEnablePicIdAry;
	DWORD *pDisablePicIdAry;
	WORD wButtonNum;
	WORD wPreserve;
	DWORD dwSystemIDMask;
	DWORD dwDisableMask;
}COMMON_TOOLBAR_CREATESTRUCT, *PCOMMON_TOOLBAR_CREATESTRUCT;
HWND CreateCommonToolBar(
	HWND hWnd,
	PCOMMON_TOOLBAR_CREATESTRUCT pCreateStruct
);

HWND WINAPI CreateToolbarEx_Combo(HWND hwnd, DWORD ws, UINT wID, int nBitmaps,
                        HINSTANCE hBMInst, UINT wBMID, TBBUTTON* lpButtons,
                        int iNumButtons, int dxButton, int dyButton,
                        int dxBitmap, int dyBitmap, UINT uStructSize,LPRECT rect);


#define NMTOOLBAR               NMTOOLBARW
#define LPNMTOOLBAR             LPNMTOOLBARW
#define BTNS_AUTOSIZE							TBSTYLE_AUTOSIZE
#define BTNS_BUTTON 							TBSTYLE_BUTTON
#define BTNS_CHECK 								TBSTYLE_CHECK
#define BTNS_CHECKGROUP 						TBSTYLE_CHECKGROUP
#define BTNS_DROPDOWN 							TBSTYLE_DROPDOWN
#define BTNS_GROUP 								TBSTYLE_GROUP
#define BTNS_NOPREFIX 							TBSTYLE_NOPREFIX
#define BTNS_SEP 								TBSTYLE_SEP

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // __TOOLBAR_H
