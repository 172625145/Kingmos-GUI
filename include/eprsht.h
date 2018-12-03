/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef _EPRSHT_H_
#define _EPRSHT_H_

#ifndef DUMMYUNIONNAME
#ifdef NONAMELESSUNION
#define DUMMYUNIONNAME   u
#define DUMMYUNIONNAME2  u2
#define DUMMYUNIONNAME3  u3
#else
#define DUMMYUNIONNAME
#define DUMMYUNIONNAME2
#define DUMMYUNIONNAME3
#endif
#endif // DUMMYUNIONNAME

#ifdef __cplusplus
extern "C" {
#endif


#ifndef SNDMSG
#ifdef __cplusplus
#define SNDMSG ::SendMessage
#else
#define SNDMSG SendMessage
#endif
#endif // ifndef SNDMSG

#define MAXPROPPAGES            100

struct _PSP;
typedef struct _PSP FAR* HPROPSHEETPAGE;

#ifndef MIDL_PASS
struct _PROPSHEETPAGEA;
struct _PROPSHEETPAGEW;
#endif

typedef UINT (CALLBACK FAR * LPFNPSPCALLBACK)(HWND hwnd, UINT uMsg, struct _PROPSHEETPAGE FAR *ppsp);


#define PSP_DEFAULT             0x0000
#define PSP_DLGINDIRECT         0x0001
#define PSP_USEHICON            0x0002
//#define PSP_USEICONID           0x0004
#define PSP_USETITLE            0x0008
#define PSP_RTLREADING          0x0010

#define PSP_HASHELP             0x0020
#define PSP_USEREFPARENT        0x0040
#define PSP_USECALLBACK         0x0080
#define PSP_PREMATURE           0x0400

#define PSP_HIDEHEADER			0x0800

#define PSPCB_RELEASE           1
#define PSPCB_CREATE            2

typedef  LPCDLG_TEMPLATE LPCDLGTEMPLATE ;
typedef struct _PROPSHEETPAGE {
        DWORD           dwSize;
        DWORD           dwFlags;
        HINSTANCE       hInstance;
//        union {
//            LPCWSTR          pszTemplate;
            LPCDLGTEMPLATE  pResource;
//        }DUMMYUNIONNAME;
//        union {
            HICON       hIcon;
//            LPCWSTR      pszIcon;
//        }DUMMYUNIONNAME2;
        TCHAR           *pszTitle;
        DLGPROC         pfnDlgProc;
        LPARAM          lParam;
        LPFNPSPCALLBACK pfnCallback;
        UINT FAR * pcRefParent;
} PROPSHEETPAGE, FAR *LPPROPSHEETPAGE;
typedef const PROPSHEETPAGE FAR *LPCPROPSHEETPAGE;

#define PSH_DEFAULT               0x0000
#define PSH_PROPTITLE             0x0001
#define PSH_USEHICON              0x0002
//#define PSH_USEICONID             0x0004
#define PSH_PROPSHEETPAGE         0x0008
#define PSH_WIZARDHASFINISH       0x0010
#define PSH_WIZARD                0x0020
//#define PSH_USEPSTARTPAGE         0x0040
#define PSH_NOAPPLYNOW            0x0080
#define PSH_USECALLBACK           0x0100
#define PSH_HASHELP               0x0200
#define PSH_MODELESS              0x0400
#define PSH_RTLREADING            0x0800
#define PSH_WIZARDCONTEXTHELP     0x1000
#define PSH_MAXIMIZE              0x2000

// Property sheet bit specific to PPC
#define PSH_NOCANCELBUTTON    0x80000000

typedef int (CALLBACK *PFNPROPSHEETCALLBACK)(HWND, UINT, LPARAM);

typedef struct _PROPSHEETHEADER {
        DWORD           dwSize;
        DWORD           dwFlags;
        HWND            hwndParent;
        HINSTANCE       hInstance;
//        union {
            HICON       hIcon;
//            LPCWSTR     pszIcon;
//        }DUMMYUNIONNAME;
        LPCWSTR         pszCaption;


        UINT            nPages;
//        union {
            UINT        nStartPage;
//            LPCWSTR     pStartPage;
//        }DUMMYUNIONNAME2;
        union {
            LPCPROPSHEETPAGE ppsp;
            HPROPSHEETPAGE FAR *phpage;
        }DUMMYUNIONNAME3;
        PFNPROPSHEETCALLBACK pfnCallback;
} PROPSHEETHEADER, FAR *LPPROPSHEETHEADER;
typedef const PROPSHEETHEADER FAR *LPCPROPSHEETHEADER;


#define PSCB_INITIALIZED  1
#define PSCB_PRECREATE    2
//#ifdef WPC_NEW  Marking new for WPC
#define PSCB_GETVERSION   3
#define PSCB_GETTITLE     4
#define PSCB_GETLINKTEXT  5
//endif

HPROPSHEETPAGE WINAPI CreatePropertySheetPage(LPCPROPSHEETPAGE);
BOOL           WINAPI DestroyPropertySheetPage(HPROPSHEETPAGE);
int            WINAPI PropertySheet(LPCPROPSHEETHEADER);

typedef BOOL (CALLBACK FAR * LPFNADDPROPSHEETPAGE)(HPROPSHEETPAGE, LPARAM);
typedef BOOL (CALLBACK FAR * LPFNADDPROPSHEETPAGES)(LPVOID, LPFNADDPROPSHEETPAGE, LPARAM);


typedef struct _PSHNOTIFY
{
    NMHDR hdr;
    LPARAM lParam;
} PSHNOTIFY, FAR *LPPSHNOTIFY;


#define PSN_FIRST               (0U-200U)
#define PSN_LAST                (0U-299U)


#define PSN_SETACTIVE           (PSN_FIRST-0)
#define PSN_KILLACTIVE          (PSN_FIRST-1)
// #define PSN_VALIDATE            (PSN_FIRST-1)
#define PSN_APPLY               (PSN_FIRST-2)
#define PSN_RESET               (PSN_FIRST-3)
// #define PSN_CANCEL              (PSN_FIRST-3)
#define PSN_HELP                (PSN_FIRST-5)
#define PSN_WIZBACK             (PSN_FIRST-6)
#define PSN_WIZNEXT             (PSN_FIRST-7)
#define PSN_WIZFINISH           (PSN_FIRST-8)
#define PSN_QUERYCANCEL         (PSN_FIRST-9)

#define PSNRET_NOERROR              0
#define PSNRET_INVALID              1
#define PSNRET_INVALID_NOCHANGEPAGE 2


#define PSM_SETCURSEL           (WM_USER + 101)
#define PropSheet_SetCurSel(hDlg, hpage, index) \
        SNDMSG(hDlg, PSM_SETCURSEL, (WPARAM)index, (LPARAM)hpage)


#define PSM_REMOVEPAGE          (WM_USER + 102)
#define PropSheet_RemovePage(hDlg, index, hpage) \
        SNDMSG(hDlg, PSM_REMOVEPAGE, index, (LPARAM)hpage)


#define PSM_ADDPAGE             (WM_USER + 103)
#define PropSheet_AddPage(hDlg, hpage) \
        SNDMSG(hDlg, PSM_ADDPAGE, 0, (LPARAM)hpage)


#define PSM_CHANGED             (WM_USER + 104)
#define PropSheet_Changed(hDlg, hwnd) \
        SNDMSG(hDlg, PSM_CHANGED, (WPARAM)hwnd, 0L)


#define PSM_RESTARTWINDOWS      (WM_USER + 105)
#define PropSheet_RestartWindows(hDlg) \
        SNDMSG(hDlg, PSM_RESTARTWINDOWS, 0, 0L)


#define PSM_REBOOTSYSTEM        (WM_USER + 106)
#define PropSheet_RebootSystem(hDlg) \
        SNDMSG(hDlg, PSM_REBOOTSYSTEM, 0, 0L)


#define PSM_CANCELTOCLOSE       (WM_USER + 107)
#define PropSheet_CancelToClose(hDlg) \
        PostMessage(hDlg, PSM_CANCELTOCLOSE, 0, 0L)


#define PSM_QUERYSIBLINGS       (WM_USER + 108)
#define PropSheet_QuerySiblings(hDlg, wParam, lParam) \
        SNDMSG(hDlg, PSM_QUERYSIBLINGS, wParam, lParam)


#define PSM_UNCHANGED           (WM_USER + 109)
#define PropSheet_UnChanged(hDlg, hwnd) \
        SNDMSG(hDlg, PSM_UNCHANGED, (WPARAM)hwnd, 0L)


#define PSM_APPLY               (WM_USER + 110)
#define PropSheet_Apply(hDlg) \
        SNDMSG(hDlg, PSM_APPLY, 0, 0L)

#define PSM_SETTITLEW           (WM_USER + 120)

#define PSM_SETTITLE            PSM_SETTITLEW

#define PropSheet_SetTitle(hDlg, wStyle, lpszText)\
        SNDMSG(hDlg, PSM_SETTITLE, wStyle, (LPARAM)(LPCTSTR)lpszText)


#define PSM_SETWIZBUTTONS       (WM_USER + 112)
#define PropSheet_SetWizButtons(hDlg, dwFlags) \
        PostMessage(hDlg, PSM_SETWIZBUTTONS, 0, (LPARAM)dwFlags)



#define PSWIZB_BACK             0x00000001
#define PSWIZB_NEXT             0x00000002
#define PSWIZB_FINISH           0x00000004
#define PSWIZB_DISABLEDFINISH   0x00000008


#define PSM_PRESSBUTTON         (WM_USER + 113)
#define PropSheet_PressButton(hDlg, iButton) \
        PostMessage(hDlg, PSM_PRESSBUTTON, (WPARAM)iButton, 0)


#define PSBTN_BACK              0
#define PSBTN_NEXT              1
#define PSBTN_FINISH            2
#define PSBTN_OK                3
#define PSBTN_APPLYNOW          4
#define PSBTN_CANCEL            5
#define PSBTN_HELP              6
#define PSBTN_MAX               6



#define PSM_SETCURSELID         (WM_USER + 114)
#define PropSheet_SetCurSelByID(hDlg, id) \
        SNDMSG(hDlg, PSM_SETCURSELID, 0, (LPARAM)id)


#define PSM_SETFINISHTEXTW      (WM_USER + 121)

#define PSM_SETFINISHTEXT       PSM_SETFINISHTEXTW

#define PropSheet_SetFinishText(hDlg, lpszText) \
        SNDMSG(hDlg, PSM_SETFINISHTEXT, 0, (LPARAM)lpszText)


#define PSM_GETTABCONTROL       (WM_USER + 116)
#define PropSheet_GetTabControl(hDlg) \
        (HWND)SNDMSG(hDlg, PSM_GETTABCONTROL, 0, 0)

#define PSM_ISDIALOGMESSAGE     (WM_USER + 117)
#define PropSheet_IsDialogMessage(hDlg, pMsg) \
        (BOOL)SNDMSG(hDlg, PSM_ISDIALOGMESSAGE, 0, (LPARAM)pMsg)

#define PSM_GETCURRENTPAGEHWND  (WM_USER + 118)
#define PropSheet_GetCurrentPageHwnd(hDlg) \
        (HWND)SNDMSG(hDlg, PSM_GETCURRENTPAGEHWND, 0, 0L)

#define ID_PSRESTARTWINDOWS     0x2
#define ID_PSREBOOTSYSTEM       (ID_PSRESTARTWINDOWS | 0x1)


#define WIZ_CXDLG               276
#define WIZ_CYDLG               140

#define WIZ_CXBMP               80

#define WIZ_BODYX               92
#define WIZ_BODYCX              184

#define PROP_SM_CXDLG           212
#define PROP_SM_CYDLG           188

#define PROP_MED_CXDLG          227
#define PROP_MED_CYDLG          215

#define PROP_LG_CXDLG           252
#define PROP_LG_CYDLG           218



#ifdef __cplusplus
}
#endif

#endif  // _EPRSHT_H_
