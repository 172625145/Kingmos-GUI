/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：对话框管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
2004-08-27, 去掉 AP_XSTART, AP_YSTART, AP_WIDTH, AP_HEIGHT，改用系统配置
2003-10-07: LN,对模式对话框清除所有的消息 
2003-08-29：LN,增加对 hWnd被DestroyWindow的问题
2003-08-26: LN,增加对 Window Title Ptr 和 lpClasName的映射（MapPtrToProcess）
2003-05-22: LN,对 DS_CENTER 考虑 AP_XSTART, AP_YSTART, AP_WIDTH, AP_HEIGHT
2003-04-10: LN,当处理WM_INITDIALOG是无法设置焦点
           (1)增加DoActive 和 hCurCtl去保存Dialog的当前焦点
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <edialog.h>

//#include <eapisrv.h>
//#include <winsrv.h>
//#include <gdisrv.h>
//#include <epwin.h>
//#include <eobjcall.h>
//#define _MAPPOINTER

static int _RunModalLoop( HWND hWnd );
static BOOL _EnableModal( HWND hWnd );
static HWND _PreModal( HWND );
static LRESULT CALLBACK DialogWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
static const char classDIALOG[] = "DIALOG";

static BOOL DoDlgPreHandle( HWND hDlg, LPMSG lpMsg );

//#define DWL_MSGRESULT   0
//#define DWL_DLGPROC     4
//#define DWL_USER        8

//对话框私有数据结构
typedef struct {
	DWORD iResult;	
    DLGPROC lpDialogFunc;
	DWORD dwUserData;
    DWORD dwState;
    HWND  hCurCtl;  //LN:2003-04-10
}DLG_WNDEX;

//#define DWL_MSGRESULT		0
//#define DWL_DLGPROC		4
//#define DWL_USER			8
#define DWL_STATE			12
#define DWL_CURCTL			16

#define BUTTON_HEIGHT 20

// **************************************************
// 声明：ATOM RegisterDialogClass( HINSTANCE hInst )
// 参数：
// 	IN hInst - 实例句柄
// 返回值：
//	假入成功，返回注册的类原子；否则，返回0
// 功能描述：
//	注册对话框类
// 引用: 
//	当系统初始化系统类时，会调用该函数
// ************************************************

ATOM RegisterDialogClass( HINSTANCE hInst )
{
    WNDCLASS wc;
	//初始化类结构
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = DialogWndProc;
    wc.hInstance = hInst;
    wc.hIcon = NULL;//LoadIcon( NULL, IDI_APPLICATION );
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DLG_WNDEX );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;//GetStockObject( LTGRAY_BRUSH );
    wc.lpszMenuName = 0;
    wc.lpszClassName = classDIALOG;
	//注册
    return RegisterClass( &wc );
}

// **************************************************
// 声明：static HWND DoCreateDialog( 
//							HINSTANCE hInst,
//							LPCDLG_TEMPLATE lpcdt,
//							HWND hParent,
//							DLGPROC lpDialogFunc,
//							LPARAM lParamInit,
//							BOOL * lpbVisible,
//							HANDLE hCallerProcess )
// 参数：
//	IN hInst - 实例句柄
//	IN lpcdt - 对话框模板结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
//	OUT lpbVisible - 用于接受是否可视
//	IN hCallerProcess - 呼叫该功能的进程
// 返回值：
//	假入成功，返回创建的窗口句柄；否则，返回 NULL
// 功能描述：
//	创建并初始化对话框窗口
// 引用: 
//	
// ************************************************

static HWND DoCreateDialog( HINSTANCE hInst,
						   LPCDLG_TEMPLATE lpcdt,
						   HWND hParent,
						   DLGPROC lpDialogFunc,
						   LPARAM lParamInit,
						   BOOL * lpbVisible,
						   HANDLE hCallerProcess )
{
    HWND hWnd, hChild;
    LPCDLG_ITEMTEMPLATE lpcdi = (LPCDLG_ITEMTEMPLATE)(lpcdt + 1);
    int i, x, y;
    BOOL bVisible;
    const char * lpClassName;

	*lpbVisible = FALSE;
	//假如可能，映射指针
    if( lpcdt->lpcClassName )
	{
#ifdef _MAPPOINTER
        lpClassName = (const char *)MapPtrToProcess( (LPVOID)lpcdt->lpcClassName, hCallerProcess );
#else
		lpClassName = (const char *)lpcdt->lpcClassName;
#endif
	}
    else
        lpClassName = classDIALOG;

    bVisible = (lpcdt->dwStyle & WS_VISIBLE) ? 1 : 0;

	x = lpcdt->x; y = lpcdt->y; 
	if( lpcdt->dwStyle & DS_CENTER )
	{	//对话框需要居中
		RECT rect;

		if( hParent && (lpcdt->dwStyle & WS_CHILD)  )
		{	//得到父的客户区矩形
			GetClientRect( hParent, &rect );
		}
		else
		{
			rect.left = GetSystemMetrics( SM_XCENTER );//AP_XSTART;
			rect.top = GetSystemMetrics( SM_YCENTER );//AP_YSTART;
			rect.right = rect.left + GetSystemMetrics( SM_CXCENTER );//AP_WIDTH;
			rect.bottom = rect.top + GetSystemMetrics( SM_CYCENTER );//AP_HEIGHT;
		}
		x = ( (rect.right-rect.left) - lpcdt->cx ) / 2 + rect.left;
		y = ( (rect.bottom-rect.top) - lpcdt->cy ) / 2 + rect.top;
	}
	//创建对话框窗口
	hWnd = CreateWindowEx( 0, 
	                     (LPCBYTE)lpClassName,
#ifdef _MAPPOINTER
                         MapPtrToProcess( (LPVOID)lpcdt->lpcWindowName, hCallerProcess ),
#else
						 lpcdt->lpcWindowName,
#endif
                         lpcdt->dwStyle & (~WS_VISIBLE),
                         x,
                         y,
                         lpcdt->cx,
                         lpcdt->cy,
                         hParent,
                         (HMENU)lpcdt->id,
                         hInst,
                         (LPVOID)lpDialogFunc );
    if( hWnd )
    {	//创建主窗口成功
        i = lpcdt->cdit;
		//创建子窗口
		while( i )
		{
			hChild = CreateWindowEx( 		
				0,
#ifdef _MAPPOINTER
				MapPtrToProcess( (LPVOID)lpcdi->lpcClassName, hCallerProcess ),
				MapPtrToProcess( (LPVOID)lpcdi->lpcWindowName, hCallerProcess ),
#else
				lpcdi->lpcClassName,
				lpcdi->lpcWindowName,
#endif
				lpcdi->dwStyle|WS_CHILD,
				lpcdi->x,
				lpcdi->y,
				lpcdi->cx,
				lpcdi->cy,
				hWnd,
				(HMENU)lpcdi->id,
				hInst,
				lpcdi->lpParam );
			if( hChild == NULL )
			{	//其中一个子窗口失败
				DestroyWindow( hWnd );
				hWnd = NULL;
				break;
			}
			i--;
			lpcdi++;
		}

	    *lpbVisible = bVisible;
    }

    return hWnd;
}

// **************************************************
// 声明：static HWND DoCreateIndirectParam( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE lpcdt,
//								  HWND hParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit, 
//								  HANDLE hCallerProcess )
// 参数：
//	IN hInst - 实例句柄
//	IN lpcdt - 对话框模板结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
//	IN hCallerProcess - 呼叫该功能的进程
// 返回值：
//	假入成功，返回创建的窗口句柄；否则，返回 NULL
// 功能描述：
//	创建并初始化对话框窗口
// 引用: 
//	
// ************************************************

static HWND DoCreateIndirectParam( 
								  HINSTANCE hInst,
								  LPCDLG_TEMPLATE lpcdt,
								  HWND hParent,
								  DLGPROC lpDialogFunc,
								  LPARAM lParamInit, 
								  HANDLE hCallerProcess )
{
	BOOL bVisible;
	//创建窗口
    HWND hWnd = DoCreateDialog( hInst, lpcdt, hParent, lpDialogFunc, lParamInit, &bVisible, hCallerProcess );
	if( hWnd )
	{
        SendMessage( hWnd, WM_INITDIALOG, 0, lParamInit );

        if( bVisible )
		{
            ShowWindow( hWnd, SW_SHOWNORMAL );
		}
	}
	return hWnd;
}

// **************************************************
// 声明：HWND WINAPI Dlg_CreateIndirectParam( 
//								HINSTANCE hInst,
//                              LPCDLG_TEMPLATE lpcdt,
//                              HWND hParent,
//                              DLGPROC lpDialogFunc,
//                              LPARAM lParamInit )
// 参数：
//	IN hInst - 实例句柄
//	IN lpcdt - 对话框模板结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数

// 返回值：
//	假入成功，返回创建的窗口句柄；否则，返回 NULL
// 功能描述：
//	创建并初始化对话框窗口
// 引用: 
//	系统API
// ************************************************

HWND WINAPI Dlg_CreateIndirectParam( HINSTANCE hInst,
                                LPCDLG_TEMPLATE lpcdt,
                                HWND hParent,
                                DLGPROC lpDialogFunc,
                                LPARAM lParamInit )
{
	return DoCreateIndirectParam( hInst, lpcdt, hParent, lpDialogFunc, lParamInit, GetCallerProcess() );
}

// **************************************************
// 声明：HWND WINAPI Dlg_CreateIndirect( 
//						HINSTANCE hInst, 
//						LPCDLG_TEMPLATE lpcdt, 
//						HWND hParent, 
//						DLGPROC lpDialogFunc )
// 参数：
//	IN hInst - 实例句柄
//	IN lpcdt - 对话框模板结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数

// 返回值：
//	假入成功，返回创建的窗口句柄；否则，返回 NULL
// 功能描述：
//	不带参数创建并初始化对话框窗口
// 引用: 
//	系统API
// ************************************************

HWND WINAPI Dlg_CreateIndirect( HINSTANCE hInst, LPCDLG_TEMPLATE lpcdt, HWND hParent, DLGPROC lpDialogFunc )
{
    return DoCreateIndirectParam( hInst, lpcdt, hParent, lpDialogFunc, 0, GetCallerProcess() );
}

// **************************************************
// 声明：static LRESULT DoDialogModal( HWND hWnd, HWND hWndParent, LPARAM lParamInit, BOOL bVisible )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN hWndParent - 父窗口
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
//	IN bVisible
// 返回值：
//	返回结果值
// 功能描述：
//	处理模式对话框
// 引用: 
//	
// ************************************************
#define DEBUG_DO_DIALOG_MODAL 0
static LRESULT DoDialogModal( HWND hWnd, HWND hWndParent, LPARAM lParamInit, BOOL bVisible )
{
    HWND hwndTop;
	BOOL bEnable = FALSE;
 
	DEBUGMSG( DEBUG_DO_DIALOG_MODAL, ( "DoDialogModal：Enter.\r\n" ) );

    if( bVisible )
		ShowWindow( hWnd, SW_SHOWNORMAL );
	SendMessage( hWnd, WM_INITDIALOG, 0, lParamInit );

	if( bVisible && _EnableModal( hWnd ) )
	{
		if( (GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD) == 0 )
		    SetForegroundWindow( hWnd );
	}
    

	//是否退出 ？
	if( _EnableModal( hWnd ) )
	{	//否
		//搜索顶级窗口
		hwndTop = _PreModal( hWndParent );
		if( hwndTop && IsWindowEnabled( hwndTop ) )
		{	//无效顶级窗口
			EnableWindow( hwndTop, FALSE );
			bEnable = TRUE;
		}
		DEBUGMSG( DEBUG_DO_DIALOG_MODAL, ( "DoDialogModal:_RunModalLoop Enter.\r\n" ) );
		_RunModalLoop( hWnd );
		
		DEBUGMSG( DEBUG_DO_DIALOG_MODAL, ( "DoDialogModal:_RunModalLoop Leave.\r\n" ) );
		//隐藏窗口
		ShowWindow( hWnd, SW_HIDE );
		if( bEnable )
		{
			EnableWindow( hwndTop, TRUE );
		}
		if( hwndTop ) //&& GetActiveWindow() == hWnd )
		{
			SetActiveWindow( hwndTop );	//恢复之前的状态			
		}

	}	

	//返回结果值
	return (int)GetWindowLong( hWnd, DWL_MSGRESULT );
}

// **************************************************
// 声明：static int DoBoxIndirectParam( 
//							HANDLE hInst, 
//							LPDLG_TEMPLATE lpTemplate, 
//							HWND hWndParent,
//							DLGPROC lpDialogFunc, 
//							LPARAM lParamInit, 
//							HANDLE hCallerProcess )
// 参数：
//	IN hInst - 实例句柄
//	IN lpTemplate - 对话框模板结构
//	IN hWndParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
//	IN hCallerProcess - 呼叫者进程

// 返回值：
//	假入成功，返回处理结果；否则，返回 -1
// 功能描述：
//	创建并初始化对话框窗口，并进入模式处理过程
// 引用: 
//	系统API
// ************************************************

static int DoBoxIndirectParam( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit, HANDLE hCallerProcess )
{
    HWND hWnd;//, hwndTop;
    BOOL bEnable = FALSE;
    int result = -1;
	BOOL bVisible;
	MSG msg;
	//清除所有的鼠标消息
	//ClearThreadQueue( 0, NULL, WM_MOUSEFIRST,WM_MOUSELAST  );
	while( PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) )
		;
	//清除所有的键盘消息
	//ClearThreadQueue( 0, NULL, WM_KEYFIRST,WM_KEYLAST  );
	while( PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ) )
		;

	//创建对话框
    hWnd = DoCreateDialog( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit, &bVisible, hCallerProcess );

    if( hWnd )
    {	//处理模式过程
		result = DoDialogModal( hWnd, hWndParent, lParamInit, bVisible );
		//破坏窗口
		DestroyWindow( hWnd );
    }
    return result;
}

// **************************************************
// 声明：int WINAPI Dlg_BoxIndirectParam( 
//							HANDLE hInst,
//							LPDLG_TEMPLATE lpTemplate,
//							HWND hWndParent,
//							DLGPROC lpDialogFunc,
//							LPARAM lParamInit )
// 参数：
//	IN hInst - 实例句柄
//	IN lpTemplate - 对话框模板结构
//	IN hWndParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
// 返回值：
//	假入成功，返回处理结果；否则，返回 -1
// 功能描述：
//	带参数创建并初始化对话框窗口，并进入模式处理过程
// 引用: 
//	系统API
// ************************************************

int WINAPI Dlg_BoxIndirectParam( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit )
{
	return DoBoxIndirectParam( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit, GetCallerProcess() );
}

// **************************************************
// 声明：int WINAPI Dlg_BoxIndirect( 
//							HANDLE hInst,
//							LPDLG_TEMPLATE lpTemplate,
//							HWND hWndParent,
//							DLGPROC lpDialogFunc )
// 参数：
//	IN hInst - 实例句柄
//	IN lpTemplate - 对话框模板结构
//	IN hWndParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
// 返回值：
//	假入成功，返回处理结果；否则，返回 -1
// 功能描述：
//	不带参数创建并初始化对话框窗口，并进入模式处理过程
// 引用: 
//	系统API
// ************************************************

int WINAPI Dlg_BoxIndirect( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc )
{
    return DoBoxIndirectParam( hInst, lpTemplate, hWndParent, lpDialogFunc, 0, GetCallerProcess() );
}


//ex-begin
// **************************************************
// 声明：static HWND DoCreateDialogEx( HINSTANCE hInst, 
//							 LPCDLG_TEMPLATE_EX lpcdt, 
//							 HWND hParent, 
//							 DLGPROC lpDialogFunc, 
//							 LPARAM lParamInit,
//							 BOOL *lpbVisible,
//							 HANDLE hCallerProcess )
// 参数：
//	IN hInst - 实例句柄
//	IN lpcdt - 对话框模板结构（DLG_TEMPLATE_EX结构指针）
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
//	OUT lpbVisible - 用于接受是否可视
//	IN hCallerProcess - 呼叫该功能的进程
// 返回值：
//	假入成功，返回创建的窗口句柄；否则，返回 NULL
// 功能描述：
//	创建并初始化对话框窗口，与DoCreateDialog不同，包含扩展风格。
// 引用: 
//	
// ************************************************

#define DEBUG_DO_CREATE_DIALOG_EX 0
static HWND DoCreateDialogEx( HINSTANCE hInst, 
							 LPCDLG_TEMPLATE_EX lpcdt, 
							 HWND hParent, 
							 DLGPROC lpDialogFunc, 
							 LPARAM lParamInit,
							 BOOL *lpbVisible,
							 HANDLE hCallerProcess )
{
    HWND hWnd, hChild;
    LPCDLG_ITEMTEMPLATE_EX lpcdi = (LPCDLG_ITEMTEMPLATE_EX)(lpcdt + 1);
    int i, x, y;
    BOOL bVisible;
    const char * lpClassName;

	*lpbVisible = FALSE;
	//映射指针到调用者进程
	if( lpcdt->lpcClassName ) 
	{
#ifdef _MAPPOINTER		
        lpClassName = (const char *)MapPtrToProcess( (LPVOID)lpcdt->lpcClassName, hCallerProcess );
#else
		lpClassName = lpcdt->lpcClassName;
#endif
	}
    else
        lpClassName = classDIALOG;
    bVisible = (lpcdt->dwStyle & WS_VISIBLE) ? 1 : 0;

	x = lpcdt->x; y = lpcdt->y; 
	if( lpcdt->dwStyle & DS_CENTER )
	{
		RECT rect;
		//计算显示矩形和位置
		if( hParent && (lpcdt->dwStyle & WS_CHILD)  )
		{
			GetClientRect( hParent, &rect );
		}
		else
		{
			//rect.left = AP_XSTART;
			//rect.top = AP_YSTART;
			//rect.right = rect.left + AP_WIDTH;
			//rect.bottom = rect.top + AP_HEIGHT;
			rect.left = GetSystemMetrics( SM_XCENTER );//AP_XSTART;
			rect.top = GetSystemMetrics( SM_YCENTER );//AP_YSTART;
			rect.right = rect.left + GetSystemMetrics( SM_CXCENTER );//AP_WIDTH;
			rect.bottom = rect.top + GetSystemMetrics( SM_CYCENTER );//AP_HEIGHT;

		}
		x = ( (rect.right-rect.left) - lpcdt->cx ) / 2 + rect.left;
		y = ( (rect.bottom-rect.top) - lpcdt->cy ) / 2 + rect.top;
	}
	DEBUGMSG( DEBUG_DO_CREATE_DIALOG_EX, ( "DoCreateDialogEx: Create Main Window.\r\n" ) );
	//创建主窗口
	hWnd = CreateWindowEx( lpcdt->dwExStyle, 
		                 (LPCBYTE)lpClassName,
#ifdef _MAPPOINTER
                         MapPtrToProcess( (LPVOID)lpcdt->lpcWindowName, hCallerProcess ),
#else
						 lpcdt->lpcWindowName,
#endif
                         lpcdt->dwStyle & (~WS_VISIBLE),
                         x,
                         y,
                         lpcdt->cx,
                         lpcdt->cy,
                         hParent,
                         (HMENU)lpcdt->id,//hMenu,
                         hInst,
                         (LPVOID)lpDialogFunc );	
    if( hWnd )
    {
        //创建子窗口
		i = lpcdt->cdit;
		DEBUGMSG( DEBUG_DO_CREATE_DIALOG_EX, ( "DoCreateDialogEx: Create Sub Window( cdit=%d ).\r\n", i ) );
		while( i )
		{
			hChild = CreateWindowEx( 
				lpcdi->dwExStyle, 
#ifdef _MAPPOINTER
				MapPtrToProcess( (LPVOID)lpcdi->lpcClassName, hCallerProcess ),
				MapPtrToProcess( (LPVOID)lpcdi->lpcWindowName, hCallerProcess ),
#else
				lpcdi->lpcClassName,
				lpcdi->lpcWindowName,
#endif
				lpcdi->dwStyle|WS_CHILD,
				lpcdi->x,
				lpcdi->y,
				lpcdi->cx,
				lpcdi->cy,
				hWnd,
				(HMENU)lpcdi->id,
				hInst,
				lpcdi->lpParam );
			if( hChild == NULL )
			{	//其中一个子窗口失败
				DestroyWindow( hWnd );
				hWnd = NULL;
				DEBUGMSG( DEBUG_DO_CREATE_DIALOG_EX, ( "DoCreateDialogEx: Create Sub Window(class=%s,title=%s) Failure( cdit=%d ).\r\n", lpcdi->lpcClassName ? lpcdi->lpcClassName : "", lpcdi->lpcWindowName ? lpcdi->lpcWindowName : "" ) );
				break;
			}
			i--;
			lpcdi++;
		}		
	    *lpbVisible = bVisible;
    }

    return hWnd;
}

// **************************************************
// 声明：static HWND DoCreateIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpcdt,
//								  HWND hParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit, 
//								  HANDLE hCallerProcess )
// 参数：
//	IN hInst - 实例句柄
//	IN lpcdt - 对话框模板扩展结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
//	IN hCallerProcess - 呼叫该功能的进程
// 返回值：
//	假入成功，返回创建的窗口句柄；否则，返回 NULL
// 功能描述：
//	带参数创建并初始化对话框窗口（包含窗口的扩展分格）
// 引用: 
//	
// ************************************************
static HWND DoCreateIndirectParamEx( HINSTANCE hInst, 
									LPCDLG_TEMPLATE_EX lpcdt, 
									HWND hParent, 
									DLGPROC lpDialogFunc, 
									LPARAM lParamInit,
									HANDLE hCallerProcess )
{
	BOOL bVisible;
    HWND hWnd = DoCreateDialogEx( hInst, lpcdt, hParent, lpDialogFunc, lParamInit, &bVisible, hCallerProcess );
	if( hWnd )
	{
        SendMessage( hWnd, WM_INITDIALOG, 0, lParamInit );

        if( bVisible )
		{	//显示窗口
            ShowWindow( hWnd, SW_SHOWNORMAL );
		}
	}
	return hWnd;
}

// **************************************************
// 声明：HWND WINAPI Dlg_CreateIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpcdt,
//								  HWND hParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit
//								  )
// 参数：
//	IN hInst - 实例句柄
//	IN lpcdt - 对话框模板扩展结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
// 返回值：
//	假入成功，返回创建的窗口句柄；否则，返回 NULL
// 功能描述：
//	带参数创建并初始化对话框窗口（包含窗口的扩展分格）
// 引用: 
//	系统API
// ************************************************

HWND WINAPI Dlg_CreateIndirectParamEx( HINSTANCE hInst, 
										  LPCDLG_TEMPLATE_EX lpcdt, 
										  HWND hParent, 
										  DLGPROC lpDialogFunc, 
										  LPARAM lParamInit )
{
	return DoCreateIndirectParamEx( hInst, lpcdt, hParent, lpDialogFunc, lParamInit, GetCallerProcess() );
}

// **************************************************
// 声明：HWND WINAPI Dlg_CreateIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpcdt,
//								  HWND hParent,
//								  DLGPROC lpDialogFunc
//								  )
// 参数：
//	IN hInst - 实例句柄
//	IN lpcdt - 对话框模板扩展结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
// 返回值：
//	假入成功，返回创建的窗口句柄；否则，返回 NULL
// 功能描述：
//	创建并初始化对话框窗口（包含窗口的扩展分格）
// 引用: 
//	系统API
// ************************************************

HWND WINAPI Dlg_CreateIndirectEx( HINSTANCE hInst, LPCDLG_TEMPLATE_EX lpTemplate, HWND hParent, DLGPROC lpDialogFunc )
{
	return DoCreateIndirectParamEx( hInst, lpTemplate, hParent, lpDialogFunc, 0, GetCallerProcess() );
}

// **************************************************
// 声明：int DoBoxIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpTemplate,
//								  HWND hWndParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit,
//								  HANDLE hCallerProcess
//								  )
// 参数：
//	IN hInst - 实例句柄
//	IN lpTemplate - 对话框模板扩展结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
//	IN hCallerProcess - 呼叫者进程
// 返回值：
//	假入成功，返回对话框返回的值；否则，返回 -1
// 功能描述：
//	1).带参数创建并初始化对话框窗口（包含窗口的扩展分格）
//	2).进入模式处理
// 引用: 
//	
// ************************************************

static int DoBoxIndirectParamEx( HANDLE hInst, 
								LPDLG_TEMPLATE_EX lpTemplate, 
								HWND hWndParent, 
								DLGPROC lpDialogFunc, 
								LPARAM lParamInit, 
								HANDLE hCallerProcess )
{
    HWND hWnd;
    BOOL bEnable = FALSE;
    int result = -1;
	BOOL bVisible;
	MSG msg;
	//先清除在线程队列中的鼠标消息和键盘消息
	//清除所有的鼠标消息
	//ClearThreadQueue( 0, NULL, WM_MOUSEFIRST,WM_MOUSELAST  );
	while( PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) )
		;
	//清除所有的键盘消息
	//ClearThreadQueue( 0, NULL, WM_KEYFIRST,WM_KEYLAST  );
	while( PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ) )
		;
	//创建并初始化窗口
	hWnd = DoCreateDialogEx( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit, &bVisible, hCallerProcess );

    if( hWnd )
    {	//处理模式
	    result = DoDialogModal( hWnd, hWndParent, lParamInit, bVisible );
		DestroyWindow( hWnd );    
	}
    return result;
}

// **************************************************
// 声明：int WINAPI Dlg_BoxIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpTemplate,
//								  HWND hWndParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit
//								  )
// 参数：
//	IN hInst - 实例句柄
//	IN lpTemplate - 对话框模板扩展结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
//	IN lParamInit - 传递给 WM_INITDIALOG 消息的LPARAM参数
// 返回值：
//	假入成功，返回对话框返回的值；否则，返回 -1
// 功能描述：
//	1).带参数创建并初始化对话框窗口（包含窗口的扩展分格）
//	2).进入模式处理
// 引用: 
//	系统API
// ************************************************

int WINAPI Dlg_BoxIndirectParamEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit )
{
	return DoBoxIndirectParamEx( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit, GetCallerProcess() );
}

// **************************************************
// 声明：int WINAPI Dlg_BoxIndirectEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpTemplate,
//								  HWND hWndParent,
//								  DLGPROC lpDialogFunc,
//								  )
// 参数：
//	IN hInst - 实例句柄
//	IN lpTemplate - 对话框模板扩展结构
//	IN hParent - 对话框的父窗口
//	IN lpDialogFunc - 对话框回调函数
// 返回值：
//	假入成功，返回对话框返回的值；否则，返回 -1
// 功能描述：
//	1).创建并初始化对话框窗口（包含窗口的扩展分格）
//	2).进入模式处理
// 引用: 
//	系统API
// ************************************************
int WINAPI Dlg_BoxIndirectEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc )
{	
	return DoBoxIndirectParamEx( hInst, lpTemplate, hWndParent, lpDialogFunc, 0, GetCallerProcess() );
}

//ex-end

// **************************************************
// 声明：BOOL WINAPI Dlg_End( HWND hDlg, int nResult )
// 参数：
// 	IN hDlg - 对话框窗口句柄
//	IN nResult - 传递给对话框的返回代码
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	结束对话框模式处理并设置对话框返回代码
// 引用: 
//	系统API 
// ************************************************

BOOL WINAPI Dlg_End( HWND hDlg, int nResult )
{
	//设置返回代码
    SetWindowLong( hDlg, DWL_MSGRESULT, nResult );
	//设置返回标志
	SetWindowLong( hDlg, DWL_STATE, 1 );   // set end flag
	//这里，有可能对话框在  GetMessage 里等待而没有出来，
	//给对话框发该消息，目的是让其从GetMessage 里出来处理退出过程
	// get a no oprate message to notify dlg message loop to exit;
	PostMessage( hDlg, WM_NULL, 0, 0 );
    return TRUE;
}

// **************************************************
// 声明：int WINAPI Dlg_GetCtrlID( HWND hwndCtl )
// 参数：
// 	IN hwndCtl - 控件窗口句柄
// 返回值：
//	假入成功，返回控件ID；否则，返回0
// 功能描述：
//	得到控件/窗口ID
// 引用: 
//	系统API
// ************************************************

int WINAPI Dlg_GetCtrlID( HWND hwndCtl )
{
    return (int)GetWindowLong( hwndCtl, GWL_ID );
}

// **************************************************
// 声明：HWND WINAPI Dlg_GetItem( HWND hDlg, int nID )
// 参数：
// 	IN hDlg - 对话框窗口句柄
//	IN nID - 子窗口ID
// 返回值：
//	假入成功，返回子窗口窗口句柄；否则，返回NULL
// 功能描述：
//	由控件ID得到对话框子窗口窗口句柄
// 引用: 
//	系统API
// ************************************************

HWND WINAPI Dlg_GetItem( HWND hDlg, int nID )
{
    HWND hWnd = GetWindow( hDlg, GW_CHILD );	//得到第一个子窗口

    while( hWnd )
    {
        if( GetWindowLong( hWnd, GWL_ID ) == nID )
            return hWnd;
		//得到下一个子窗口
        hWnd = GetWindow( hWnd, GW_HWNDNEXT );
    }
    return hWnd;
}

// **************************************************
// 声明：UINT WINAPI Dlg_GetItemText( HWND hDlg, int nID, LPSTR lpstr, int nMaxCount )
// 参数：
// 	IN hDlg - 对话框窗口句柄
//	IN nID - 子窗口ID
//	IN/OUT lpstr - 用于接受文本的缓存
//	IN nMaxCount - lpstr缓存的大小
// 返回值：
//	假如成功，返回拷贝到lpstr里的字符数（不含0结束符）；否则，返回0
// 功能描述：
//	得到对话框控件文本
// 引用: 
//	系统API
// ************************************************

UINT WINAPI Dlg_GetItemText( HWND hDlg, int nID, LPSTR lpstr, int nMaxCount )
{
    HWND hWnd = Dlg_GetItem( hDlg, nID );
    if( hWnd )
        return (UINT)SendMessage( hWnd, WM_GETTEXT, (WPARAM)nMaxCount, (LPARAM)lpstr );
    else
        return 0;
}

// **************************************************
// 声明：LONG WINAPI Dlg_SendItemMessage( HWND hDlg, int nID, UINT msg, WPARAM wParam, LPARAM lParam )
// 参数：
// 	IN hDlg - 对话框句柄
//	IN nID  - 子窗口ID
//	IN msg - 需要发送的消息
//	IN wParam - WPARAM 消息参数
//	IN lParam - LPARAM 消息参数
// 返回值：
//	假入成功，返回该消息对应的返回值；否则，返回0
// 功能描述：
//	向对话框控件发送消息
// 引用: 
//	系统API
// ************************************************

LONG WINAPI Dlg_SendItemMessage( HWND hDlg, int nID, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HWND hWnd = Dlg_GetItem( hDlg, nID );
    _ASSERT( hWnd );
    if( hWnd )
        return SendMessage( hWnd, msg, wParam, lParam );
    else
        return 0;
}

// **************************************************
// 声明：BOOL WINAPI Dlg_SetItemText( HWND hDlg, int nID, LPCTSTR lpstr )
// 参数：
// 	IN hDlg - 对话框窗口句柄
//	IN nID - 子窗口/控件ID
//	IN lpstr - 需要设置的子窗口/控件的文本
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置子窗口/控件的仓库文本
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Dlg_SetItemText( HWND hDlg, int nID, LPCTSTR lpstr )
{
    HWND hWnd = Dlg_GetItem( hDlg, nID );
    if( hWnd )
        return (BOOL)SendMessage( hWnd, WM_SETTEXT, 0, (LPARAM)lpstr );
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Dlg_SetItemInt( HWND hDlg, int nID, UINT uValue, BOOL bSigned )
// 参数：
// 	IN hDlg - 对话框窗口句柄
//	IN nID - 子窗口/控件ID
//	IN uValue - 数字值
//	IN bSigned - 说明nValue是代符号(为TRUE)或是不带符号的(为FALSE)
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将值 uValue 转化为字符串文本
//	将字符串文本设置为子窗口/控件ID的窗口文本
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Dlg_SetItemInt( HWND hDlg, int nID, UINT uValue, BOOL bSigned )
{
	HWND	hCtl;
	TCHAR	pszTmp[20];

	hCtl = Dlg_GetItem( hDlg, nID );
	if( hCtl )
	{	//有效的 窗口
		//将值 uValue 转化为字符串文本
		if( bSigned )
			sprintf( pszTmp, TEXT("%d"), uValue );
		else
			sprintf( pszTmp, TEXT("%u"), uValue );
		//设置窗口文本
		return SetWindowText( hCtl, pszTmp );
	}
	return FALSE;
}

// **************************************************
// 声明：UINT WINAPI Dlg_GetItemInt( HWND hDlg, int nID, BOOL *lpTranslated, BOOL bSigned )
// 参数：
// 	IN hDlg - 对话框窗口句柄
//	IN nID - 子窗口/控件ID
//	OUT lpTranslated - 用于接受是否转换成功
//	IN bSigned - 说明返回值是代符号(为TRUE)或是不带符号的(为FALSE)
// 返回值：
//	假入成功，lpTranslated的值为TRUE；否则，lpTranslated的值为FALSE
// 功能描述：
//	得到子窗口/控件的窗口文本
//	将得到的窗口文本转化为数字值
// 引用: 
//	系统API
// ************************************************

UINT WINAPI Dlg_GetItemInt( HWND hDlg, int nID, BOOL *lpTranslated, BOOL bSigned )
{
	TCHAR	pszNum[20];
	HWND	hCtl;
	UINT uiRetv = 0;

	if( lpTranslated )
		*lpTranslated =FALSE;
	hCtl = Dlg_GetItem( hDlg, nID );
	//有效窗口 ？
	if( hCtl )
	{	//有效，得到其长度
		UINT len = SendMessage( hCtl, WM_GETTEXTLENGTH, 0, 0 );
		if( len <= 19 )
		{	//快速方法
			SendMessage( hCtl, WM_GETTEXT, 20, (LPARAM)pszNum );
			if( bSigned )
			    uiRetv = (UINT)atoi( pszNum );	//带符号
			else
				uiRetv = (UINT)strtoul( pszNum, NULL, 10 );	//不带符号
	        if( lpTranslated )
		       *lpTranslated = TRUE;
		}
		else
		{	//慢速方法，先分配要求大小的指针
			TCHAR * p = malloc( (len + 1)* sizeof(TCHAR) );
			if( p )
			{	//得到文本
				SendMessage( hCtl, WM_GETTEXT, (len + 1)* sizeof(TCHAR), (LPARAM)p );
				if( bSigned )
					uiRetv = (UINT)atoi( p );
				else
					uiRetv = (UINT)strtoul( p, NULL, 10 );				
				free( p );
	            if( lpTranslated )
		           *lpTranslated = TRUE;
			}
		}
	}
	return uiRetv;
}

// **************************************************
// 声明：static BOOL _EnableModal( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	假入继续模式处理，返回TRUE；否则，返回FALSE
// 功能描述：
//	判断对话快框窗口是否需要继续进行模式处理
// 引用: 
//	
// ************************************************

static BOOL _EnableModal( HWND hWnd )
{   
	return ( IsWindow( hWnd ) && !GetWindowLong( hWnd, DWL_STATE ) );
}

// **************************************************
// 声明：static int _RunModalLoop( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	假入成功，返回模式处理返回值；否则，返回0
// 功能描述：
//	处理模式对话框过程
// 引用: 
//	
// ************************************************

#define DEBUG_MODAL_LOOP 0
static int _RunModalLoop( HWND hWnd )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    BOOL bIdle = (BOOL)!(dwStyle & DS_NOIDLEMSG);
    MSG msg;
    BOOL bContinue = TRUE;
    HWND hWndParent = GetParent( hWnd );
    ULONG ulIdleCount = 0;

    DEBUGMSG( DEBUG_MODAL_LOOP, ( "_RunModalLoop: entry.\r\n" ) );
	SetCapture( NULL );
	//循环处理消息直到退出
	while( bContinue )
    {	//得到当前模式状态
        bContinue = _EnableModal( hWnd );// enable continue modal loop
		//判断是否有消息
        while( bContinue && bIdle && !PeekMessage( &msg, 0, 0, 0, PM_NOREMOVE ) )
        {	//没有消息，是否需要发送 IDLE 消息 ？
            if( !(dwStyle & DS_NOIDLEMSG) && hWndParent && ulIdleCount == 0 )
            {	//是
                SendMessage( hWndParent, WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)hWnd );
            }
            bIdle = SendMessage( hWnd, WM_KICKIDLE, MSGF_DIALOGBOX, ulIdleCount++ );
			bContinue = _EnableModal( hWnd );
        }
		//
        while( bContinue )
        {	//判断是否有消息
            if( GetMessage( &msg, 0, 0, 0 ) )
            {	//有
                TranslateMessage( &msg );
				// 2005-09-20， 增加对 WS_GROUP 的处理
				if( DoDlgPreHandle( hWnd, &msg ) == FALSE )
					DispatchMessage( &msg );
				//
            }
            else
            {	//异常退出
                ASSERT( 0 );
                PostQuitMessage(0);
                return -1;
            }
			//处理完消息，检查是否有更多的消息 或是否退出模式处理
            bContinue = _EnableModal( hWnd );
			if( bContinue && PeekMessage( &msg, 0, 0, 0, PM_NOREMOVE ) )
				continue;
			else
				break;
        };

        if( !(dwStyle & DS_NOIDLEMSG) )
        {
            bIdle = TRUE;
            ulIdleCount = 0;
        }
    }
	DEBUGMSG( DEBUG_MODAL_LOOP, ( "_RunModalLoop: leave.\r\n" ) );
	return (int)GetWindowLong( hWnd, DWL_MSGRESULT );
}

// **************************************************
// 声明：static HWND _PreModal( HWND hWndParent )
// 参数：
// 	IN hWndParent - 窗口句柄
// 返回值：
//	返回前一个模式窗口
// 功能描述：
//	得到前一个模式窗口
// 引用: 
//	
// ************************************************

static HWND _PreModal( HWND hWndParent )
{
    HWND hWnd = hWndParent;
    while( hWnd && ( GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD ) )
        hWnd = GetParent( hWnd );
    return hWnd;
}

// **************************************************
// 声明：static HWND FindFocusWindow( HWND hWndDialog )
// 参数：
// 	IN hWndDialog - 窗口句柄
// 返回值：
//	假入成功，返回需要设置焦点的窗口句柄；否则，返回NULL
// 功能描述：
//	发现需要设置焦点的窗口句柄
// 引用: 
//	
// ************************************************

static HWND FindFocusWindow( HWND hWndDialog )
{
	HWND hChild = GetWindow( hWndDialog, GW_CHILD );

	while( hChild )
	{
		DWORD dwStyle = GetWindowLong( hChild, GWL_STYLE );
        if( (dwStyle & WS_VISIBLE) && 
			(dwStyle & WS_DISABLED ) == 0 &&
			(dwStyle & WS_TABSTOP ) )
			break;
		hChild = GetWindow( hChild, GW_HWNDNEXT );		
	}
	return hChild;
}

// **************************************************
// 声明：static LRESULT DoActive( HWND hWnd, WPARAM wParam )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN wParam - WM_ACTIVATE 消息的 WPARAM 参数
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_ACTIVATE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoActive( HWND hWnd, WPARAM wParam )
{
	if( LOWORD( wParam ) == WA_INACTIVE )
	{	//需要使窗口为非活动状态
		HWND hFocus = GetFocus();
		
		if( hFocus && GetParent( hFocus ) == hWnd )
			SetWindowLong( hWnd, DWL_CURCTL, (LONG)hFocus );
		else
			SetWindowLong( hWnd, DWL_CURCTL, NULL );
	}
	else if( LOWORD( wParam ) == WA_ACTIVE )
	{	//需要使窗口为活动状态
		HWND hFocus = (HWND)GetWindowLong( hWnd, DWL_CURCTL );
		
		if( hFocus == NULL )
			hFocus = FindFocusWindow( hWnd );	//预设一个

		if( hFocus )
			SetFocus( hFocus );
	}
	return 0;
}

// **************************************************
// 声明：LRESULT WINAPI DialogWndProc(
//									HWND hWnd,
//									UINT msg,
//									WPARAM wParam,
//									LPARAM lParam)
// 参数：
//  IN hWnd - 窗口句柄
//	IN msg - 消息
//	IN wParam - 第一个消息
//	IN lParam - 第二个消息
// 返回值：
//	依赖于具体的消息
// 功能描述：
//	对话框窗口处理总入口
// 引用: 
//	
// ************************************************


LRESULT CALLBACK DialogWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    LPCREATESTRUCT lpcs;
    DLGPROC lpDialogFunc=0;
    BOOL retv = TRUE;

    lpDialogFunc = (DLGPROC)GetWindowLong( hWnd, DWL_DLGPROC );
    if( lpDialogFunc )
    {	//让用户处理函数先处理
/*
		_LPWINDATA lpws = _GetHWNDPtr( hWnd );
		if( lpws )
		{
			if( lpws->hinst != hgwmeInstance )
			{	//跨进程
				CALLBACKDATA cd;
				cd.hProcess = lpws->hOwnerProcess;
				cd.lpfn = (FARPROC)lpDialogFunc;
				cd.dwArg0 = (DWORD)hWnd;
				retv = Sys_ImplementCallBack4( &cd, msg, wParam, lParam );
			}
			else
				retv = lpDialogFunc( hWnd, msg, wParam, lParam );
		}
		else
			return 0;
		*/
		retv = lpDialogFunc( hWnd, msg, wParam, lParam );
		
		if( IsWindow( hWnd ) )
		{
			if( retv && ( msg != WM_INITDIALOG ) )
			{
				return GetWindowLong( hWnd, DWL_MSGRESULT );	//用户已处理， 不需要做更多的处理
			}
		}
		else
		{	//无效的窗口
			return 0;
		}

    }
	//对话框默认处理
    switch( msg )
    {
    case WM_COMMAND:
       if( HIWORD( wParam ) == BN_CLICKED &&
           ( LOWORD( wParam ) == IDOK || LOWORD( wParam ) == IDCANCEL ) )
           Dlg_End( hWnd, LOWORD( wParam ) );
       break;
	case WM_ERASEBKGND:		//擦出背景
		{
			RECT rcClient;

			GetClientRect( hWnd, &rcClient );
			FillRect( (HDC)wParam, &rcClient, GetSysColorBrush(COLOR_3DFACE) );
			return 0;
		}
	case WM_ACTIVATE:		//窗口激活
		return DoActive( hWnd, wParam );
    case WM_CLOSE:			//窗口关闭
        Dlg_End( hWnd, 0 );
        break;
    case WM_CREATE:
        lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong( hWnd, DWL_DLGPROC, (DWORD)lpcs->lpCreateParams );
        SetWindowLong( hWnd, DWL_MSGRESULT, 0 );
		SetWindowLong( hWnd, DWL_STATE, 0 );
		SetWindowLong( hWnd, DWL_CURCTL, 0 );

        break;
    case WM_INITDIALOG:
        if( retv == TRUE )
        {    // set focus control
            if( wParam )
                SetFocus( (HWND)wParam );			
        }
		SetWindowLong( hWnd, DWL_MSGRESULT, 0 );
        break;
    default:
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }
    return 0;
}

// 2005-09-20， 增加对 WS_GROUP 的处理
HWND WINAPI Dlg_GetNextGroupItem(
								HWND hDlg,  // 父窗口
								HWND hCtl,  // 当前控件
								BOOL bPrevious  // 向前或是向后搜..
								)
{
	HWND hNewCtrl = hCtl;
	HWND hStartCtrl = hCtl;
	BOOL bIsInvert = FALSE;
	if( hDlg )
	{
		if( hCtl )
		{
			while( 1 )
			{
				if( bPrevious )
				{
					hNewCtrl = GetWindow( hStartCtrl, GW_HWNDPREV );
				}
				else
				{
					hNewCtrl = GetWindow( hStartCtrl, GW_HWNDNEXT );
				}
				if( hNewCtrl )
				{  //发现
					DWORD dwStyle = GetWindowLong( hNewCtrl, GWL_STYLE );
					if( (dwStyle & WS_VISIBLE) &&
						(dwStyle & WS_DISABLED) == 0 )
					{   //可见并且可能的
						if( ( dwStyle & WS_GROUP ) == 0 )
						{	//符合条件
							if( !bIsInvert )
							    return hNewCtrl;
						}
						else
						{   //新的 group 开始，需要反转方向，得到第一个 WS_GROUP
							if( bIsInvert )
							{ //已经反转过一次,不需要再做..
								return hStartCtrl;
							}
							else
							{  //反转一次
								bPrevious = !bPrevious;
								bIsInvert = TRUE;
								hStartCtrl = hCtl;
								continue;
							}
						}
					}					
				}
				else  //找到没有
				{
					if( bIsInvert == TRUE )
					{//已经反转过一次
						return hStartCtrl;
					}
					//反转一次
					bIsInvert = TRUE;
					bPrevious = !bPrevious;
					hStartCtrl = hCtl;
					continue;
				}
				hStartCtrl = hNewCtrl;  //下一个
			}
		}
	}
	return hNewCtrl;
}

// 2005-09-20， 增加对 WS_GROUP 的处理
static BOOL DoDlgPreHandle( 
						   HWND hDlg,      // 控件所在的对话框
						   LPMSG lpMsg     // 消息结构
						   )
{
	WPARAM wParam;
	switch( lpMsg->message )
	{
	case WM_KEYDOWN:
		wParam = lpMsg->wParam;
		if( wParam == VK_LEFT  ||
			wParam == VK_RIGHT ||
			wParam == VK_UP ||
			wParam == VK_DOWN )
		{	//方向键
			if( IsChild( hDlg, lpMsg->hwnd ) )
			{
				LRESULT lret = SendMessage( lpMsg->hwnd, WM_GETDLGCODE, 0, 0 );
				HWND hCtrl;
				if( lret != DLGC_WANTALLKEYS ||
					lret != DLGC_WANTARROWS )
				{	// control 不需要处理这些消息
					BOOL bPrevious = ( wParam == VK_LEFT || wParam == VK_UP );
					hCtrl = GetNextDlgGroupItem( hDlg, lpMsg->hwnd, bPrevious );
					if( hCtrl )
					{   // 设置新的焦点
						SetFocus( hCtrl );
						//SendMessage( hCtrl, WM_SETFOCUS, (WPARAM)lpMsg->hwnd, 0 );
						return TRUE;
					}
				}
			}
		}
		break;
	}
	return FALSE;
}