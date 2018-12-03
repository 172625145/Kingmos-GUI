/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EDIALOG_H
#define __EDIALOG_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

typedef struct _DLG_TEMPLATE
{
    DWORD dwStyle;    
    WORD  cdit; // number of control item
    short x;
    short y;
    short cx;
    short cy;
    WORD id;
    LPCSTR lpcClassName;
    LPCSTR lpcWindowName;
}DLG_TEMPLATE, * PDLG_TEMPLATE, FAR * LPDLG_TEMPLATE;
typedef const DLG_TEMPLATE * LPCDLG_TEMPLATE;

typedef struct _DLG_ITEMTEMPLATE
{
    DWORD dwStyle;
    short x;
    short y;
    short cx;
    short cy;
    WORD id;
    LPCSTR lpcClassName;
    LPCSTR lpcWindowName;
    LPVOID lpParam;
}DLG_ITEMTEMPLATE, * PDLG_ITEMTEMPLATE, FAR * LPDLG_ITEMTEMPLATE;
typedef const DLG_ITEMTEMPLATE * LPCDLG_ITEMTEMPLATE;

typedef struct _DLG_TEMPLATE_EX
{
	DWORD dwExStyle;
    DWORD dwStyle;    
    WORD  cdit; // number of control item
    short x;
    short y;
    short cx;
    short cy;
    WORD id;
    LPCSTR lpcClassName;
    LPCSTR lpcWindowName;
}DLG_TEMPLATE_EX, * PDLG_TEMPLATE_EX, FAR * LPDLG_TEMPLATE_EX;
typedef const DLG_TEMPLATE_EX * LPCDLG_TEMPLATE_EX;

typedef struct _DLG_ITEMTEMPLATE_EX
{
    DWORD dwExStyle;
    DWORD dwStyle;
    short x;
    short y;
    short cx;
    short cy;
    WORD id;
    LPCSTR lpcClassName;
    LPCSTR lpcWindowName;
    LPVOID lpParam;
}DLG_ITEMTEMPLATE_EX, * PDLG_ITEMTEMPLATE_EX, FAR * LPDLG_ITEMTEMPLATE_EX;
typedef const DLG_ITEMTEMPLATE_EX * LPCDLG_ITEMTEMPLATE_EX;

// 系统ID 
// 0~50 use by gwme
// 50~100 use by system control
// 100~   use by user
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define IDCLOSE             8
#define IDHELP              9
//

#define DS_CENTER           0x01L
#define DS_SYSMODAL         0x02L  
#define DS_SETFONT          0x40L
#define DS_MODALFRAME       0x80L
#define DS_NOIDLEMSG        0x100L

//对话框数据
#define DWL_MSGRESULT   0
#define DWL_DLGPROC     4
#define DWL_USER        8

// 处理键盘的协议代码
#define DLGC_WANTARROWS     0x0001      //控件需要方向键盘消息
#define DLGC_WANTTAB        0x0002      //控件需要TAB键盘消息
#define DLGC_WANTALLKEYS    0x0004      //控件需要所有键盘消息
#define DLGC_WANTMESSAGE    0x0004      //控件需要消息
#define DLGC_HASSETSEL      0x0008      //控件需要处理 EM_SETSEL
#define DLGC_DEFPUSHBUTTON  0x0010      //默认的 pushbutton
#define DLGC_UNDEFPUSHBUTTON 0x0020     //非默认的 pushbutton
#define DLGC_RADIOBUTTON    0x0040      // Radio button
#define DLGC_WANTCHARS      0x0080      // 需要 WM_CHAR messages
#define DLGC_STATIC         0x0100      // Static item: don't include 
#define DLGC_BUTTON         0x2000      // Button item: can be checked

#define LB_CTLCODE          0L


#define CreateDialogIndirect Dlg_CreateIndirect
HWND WINAPI Dlg_CreateIndirect( HINSTANCE hInst, LPCDLG_TEMPLATE lpTemplate, HWND hParent, DLGPROC lpDialogFunc );

#define CreateDialogIndirectParam Dlg_CreateIndirectParam
HWND WINAPI Dlg_CreateIndirectParam( HINSTANCE hInst, LPCDLG_TEMPLATE lpName, HWND hParent, DLGPROC lpDialogFunc, LPARAM lParamInit );

#define DialogBoxIndirect Dlg_BoxIndirect
int WINAPI Dlg_BoxIndirect( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc );

#define DialogBoxIndirectParam Dlg_BoxIndirectParam
int WINAPI Dlg_BoxIndirectParam( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit );

// ex
#define CreateDialogIndirectEx Dlg_CreateIndirectEx
HWND WINAPI Dlg_CreateIndirectEx( HINSTANCE hInst, LPCDLG_TEMPLATE_EX lpTemplate, HWND hParent, DLGPROC lpDialogFunc );

#define CreateDialogIndirectParamEx Dlg_CreateIndirectParamEx
HWND WINAPI Dlg_CreateIndirectParamEx( HINSTANCE hInst, LPCDLG_TEMPLATE_EX lpTemplate, HWND hParent, DLGPROC lpDialogFunc, LPARAM lParamInit );

#define DialogBoxIndirectEx Dlg_BoxIndirectEx
int WINAPI Dlg_BoxIndirectEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc );

#define DialogBoxIndirectParamEx Dlg_BoxIndirectParamEx
int WINAPI Dlg_BoxIndirectParamEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit );
// ex-end


#define EndDialog Dlg_End
BOOL WINAPI Dlg_End( HWND hDlg, int nResult );

#define GetDlgCtrlID Dlg_GetCtrlID
int WINAPI Dlg_GetCtrlID( HWND hwndCtl );

#define GetDlgItem Dlg_GetItem
HWND WINAPI Dlg_GetItem( HWND hDlg, int nID );

#define GetDlgItemText Dlg_GetItemText
UINT WINAPI Dlg_GetItemText( HWND hDlg, int nID, LPTSTR lpstr, int nMaxCount );

#define SetDlgItemText Dlg_SetItemText
BOOL WINAPI Dlg_SetItemText( HWND hDlg, int nIDDlgItem, LPCTSTR lpString );

#define SendDlgItemMessage Dlg_SendItemMessage
LONG WINAPI Dlg_SendItemMessage( HWND hDlg, int nID, UINT msg, WPARAM wParam, LPARAM lParam );

#define	IsDlgButtonChecked( hWnd, id )		SendMessage( GetDlgItem((hWnd), (id)), BM_GETCHECK, 0, 0 )
#define	CheckDlgButton( hWnd, id, nChk )	SendMessage( GetDlgItem((hWnd), (id)), BM_SETCHECK, nChk, 0 )

#define SetDlgItemInt Dlg_SetItemInt
BOOL WINAPI Dlg_SetItemInt( HWND hDlg, int nID, UINT uValue, BOOL bSigned );

#define GetDlgItemInt Dlg_GetItemInt
UINT WINAPI Dlg_GetItemInt( HWND hDlg, int nID, BOOL *lpTranslated, BOOL bSigned );

#define GetNextDlgGroupItem Dlg_GetNextGroupItem
HWND WINAPI Dlg_GetNextGroupItem(
								HWND hDlg,  // 父窗口
								HWND hCtl,  // 当前控件
								BOOL bPrevious  // 向前或是向后搜..
								);



#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L


#define MB_TOPMOST                  0x00040000L


#define MessageBox Dlg_MessageBox
int WINAPI Dlg_MessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif   // __EDIALOG_H
