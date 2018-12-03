/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：list class
版本号：1.0.0
开发时期：2003-04-07
作者：李林
修改记录：
	2005-01-26, add code, DoRESETCONTENT 没有刷新滚动条
	2004-12-21, 改变 DoLBUTTONUP & DoLBUTTONDOWN , 去掉 SetCapture功能
				因为有可能在 发LB_CLICK 后AP进入其它画面，然后又被 SetCapture

	2004-10-27, DoSETCURSEL 2004-10-27, modify, 优化
	2004-08-27, 将 _LISTDATA 的 count 成员有 short 改为 WORD
	2004-03-06, LN DoSetRedraw 之前并没有做任何事情
	2003-08-29  LN, not sent LBN_SELCHANGE when use LB_SETCURSEL
    2003-06-23, DoSETCARETINDEX 增加对Caret的隐藏
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <eassert.h>
#include <ealloc.h>
//#include <gwmesrv.h>

#define ITEMEXT 10
#define ITEM_HEIGHT 16
#define ITEM_WIDTH 8
#define AUTO_UP 0xfffe
#define AUTO_DOWN 0xfffd
#define AUTO_HOLD 0xffc

//#define _MAPPOINTER

// 定义列表框项目结构
typedef struct __LISTITEM
{
	LPSTR lpszItem;		//列表框项目数据
	DWORD dwItemData;	//列表框项目用户数据
	WORD typeData;		//列表框项目用户数据类型
	WORD state;			//当前列表项目状态
}_LISTITEM, FAR * _LPLISTITEM;

//	定义列表框结构
typedef struct  __LISTDATA
{
	_LPLISTITEM lpItems;	//列表框项目指针
	DWORD dwStyle;			//列表框风格
	WORD count;			//列表框当前项目数
	BYTE bFree;			//是否由列表框释放内容（无用）
	BYTE bDown;			//是否在画面有处理 WM_LBUTTONDOWN 消息
	
	short limit;			//列表框可容纳的最大项目数目
    short caret;			//列表框当前标注项目

    short curSel;			//列表框当前被选择项目（用于单项选择风格）use when single-selection
    short topIndex;			//列表框当前显示窗口的最顶项目

    short colWidth;			//列表框列宽
    short rowHeight;		//列表框行宽

    short colNum;			//列表框列数
    short bCombBoxList;		//是否是组合框列表风格
    HWND hCombBox;			//假如是组合框列表，该项有效，表示组合框窗口

	COLORREF cl_Text;		//正常文本的前景与背景色
	COLORREF cl_TextBk;     //
	COLORREF cl_Selection;	//选择文本的前景与背景色
	COLORREF cl_SelectionBk;  // 
	COLORREF cl_Disable;	//无效文本的前景与背景色
	COLORREF cl_DisableBk;    // 
	HANDLE hFont;
}_LISTDATA, FAR * _LPLISTDATA;

// insert before index
static _LPLISTITEM InsertItem( _LPLISTDATA, int * index );
static BOOL SetItemData( _LPLISTDATA, int index, DWORD dwData );
static BOOL SetItemTypeData( _LPLISTDATA, int index, WORD dwTypeData );
static BOOL RemoveItem( _LPLISTDATA, int index );
static BOOL ResetLimit( _LPLISTDATA, int newLimit );
static int SetItemState( _LPLISTDATA, int index, WORD state, BOOL bEnable ); 
static BOOL FreeItem( LPSTR lpstr );
static _LPLISTITEM At( _LPLISTDATA, int index );
static int Count( _LPLISTDATA );
//static BOOL Pack( _LPLISTDATA );
static void RedrawItem( _LPLISTDATA lpData, 
					   HWND hWnd, 
					   DWORD dwStyle, 
					   HDC hdc, 
					   int iFrom, 
					   int iTo,
					   BOOL bEraseBottom
					    );
static LRESULT DoADDSTRING( HWND hWnd, LPCSTR lpcstr );
static LRESULT DoINSERTSTRING( HWND hWnd, int index, LPCSTR lpcstr );
static LRESULT DoSETSEL( HWND hWnd, BOOL fSelect, int index );
static LRESULT DoSETCURSEL( HWND hWnd, int index, BOOL bNotify );
static LRESULT DoSETTOPINDEX( HWND hWnd, int index );
static LRESULT DoSELITEMRANGE( HWND hWnd, BOOL fSelect, int first, int last );
static LRESULT DoSETCARETINDEX( HWND hWnd, int index, BOOL fScroll );

static short GetItemRect( _LPLISTDATA, int index, LPRECT lpRect, DWORD style );
static int GetRows( _LPLISTDATA, DWORD dwStyle, LPCRECT lpClienRect );
static int GetCols( _LPLISTDATA, LPCRECT lpClienRect );
static int AtRow( _LPLISTDATA, DWORD dwStyle, int y );
static int AtCol( _LPLISTDATA, DWORD dwStyle, int x );
static void  DrawItemContents ( HWND hWnd, _LPLISTDATA lpListData, DWORD dwStyle, HDC hdc, int index, LPCRECT lpRect, BOOL bHasCaret, UINT uiAction );
static void DrawCaret( HDC hdc, LPCRECT lpRect );
static void SendNotify( HWND hWnd, const short code );
static BOOL SetScrollBar( HWND hWnd, int pos, int *lpNewPos );
static int ScrollRow( HWND hWnd, int code );
static int ScrollCol( HWND hWnd, int code );
static void CALLBACK TimerProc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );

static LRESULT WINAPI ListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
static LRESULT WINAPI CombBoxListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

// 列表框类名
static const char strClassListBox[] = "LISTBOX";

// **************************************************
// 声明：ATOM RegisterListBoxClass(HINSTANCE hInst)
// 参数：
//		IN hInst - 实例句柄
// 返回值：
//		假如成功，返回非0值；失败，返回0
// 功能描述：
//		注册列表框类
// 引用: 
//		被sysclass.c 调用
// ************************************************

// register my define class
ATOM RegisterListBoxClass(HINSTANCE hInst)
{
   WNDCLASS wc;

   wc.hInstance=hInst;
   wc.lpszClassName= strClassListBox;
// the proc is class function
   wc.lpfnWndProc = ListBoxWndProc;
   wc.style=CS_DBLCLKS;
   wc.hIcon= 0;
// at pen window, no cursor
   wc.hCursor= LoadCursor(NULL,IDC_ARROW);
// to auto erase background, must set a valid brush
// if 0, you must erase background yourself
   wc.hbrBackground = NULL;

   wc.lpszMenuName=NULL;
   wc.cbClsExtra=0;
// !!! it's important to save state of button, align to long
   //私有数据
   wc.cbWndExtra= sizeof( void * );

   return RegisterClass(&wc);
}

// 组合列表框类名
static const char strClassComboListBox[] = "COMBOLISTBOX";

// register my define class
// **************************************************
// 声明：ATOM RegisterComboListBoxClass(HINSTANCE hInst)
// 参数：
//		IN hInst - 实例句柄
// 返回值：
//		假如成功，返回非0值；失败，返回0
// 功能描述：
//		注册组合列表框类
// 引用: 
//		被sysclass.c 调用
// ************************************************
ATOM RegisterComboListBoxClass(HINSTANCE hInst)
{
   WNDCLASS wc;

   wc.hInstance=hInst;
   wc.lpszClassName= strClassComboListBox;
// the proc is class function
   wc.lpfnWndProc = CombBoxListBoxWndProc;
   wc.style=CS_DBLCLKS;
   wc.hIcon= 0;
// at pen window, no cursor
   wc.hCursor= LoadCursor(NULL,IDC_ARROW);
// to auto erase background, must set a valid brush
// if 0, you must erase background yourself
   wc.hbrBackground = NULL;
   wc.lpszMenuName=NULL;
   wc.cbClsExtra=0;
// !!! it's important to align to long
   ////私有数据
   wc.cbWndExtra=sizeof( void * );

   return RegisterClass(&wc);
}

// **************************************************
// 声明：static HDC GetListDC( HWND hWnd, PAINTSTRUCT * lpps, HFONT hFont )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpps - PAINTSTRUCT 结构指针
//	IN hFont - 字体句柄
// 返回值：
//	假如成功，返回DC 句柄
// 功能描述：
//	得到DC(如果 lpps 为NULL,用GetDC,否则用BeginPaint)
// 引用: 
//	
// ************************************************

static HDC GetListDC( HWND hWnd, PAINTSTRUCT * lpps, _LPLISTDATA lpListData )
{
	HDC hdc;
	if( lpps )
	{
		hdc = BeginPaint( hWnd, lpps );
	}
	else
		hdc = GetDC( hWnd );
	if( lpListData->hFont )
		SelectObject( hdc, lpListData->hFont );
	return hdc;
}

// **************************************************
// 声明：static VOID ReleaseListDC( HWND hWnd, PAINTSTRUCT * lpps, _LPLISTDATA lpListData )
// 参数：
//	IN hWnd - 窗口句柄
//	IN hdc - DC句柄
//	IN lpps - PAINTSTRUCT 结构指针
// 返回值：
//	无
// 功能描述：
//	释放DC
// 引用: 
//	
// ************************************************

static VOID ReleaseListDC( HWND hWnd, HDC hdc, PAINTSTRUCT * lpps )
{
	if( lpps )
	{
		EndPaint( hWnd, lpps );
	}
	else
		ReleaseDC( hWnd, hdc );
}

// **************************************************
// 声明：static _LPLISTITEM InsertItem( _LPLISTDATA lpListData, int * index )
// 参数：
//	IN/OUT lpListData - 列表框对象指针
//	IN/OUT index - 将列表框项目插入位置，假如为-1,表示将列表框项目插到最后并返回实际的位置
// 返回值：
//	假如成功，返回新的项目指针; 否则，返回0
// 功能描述：
//	在当前列表框位置新增一个列表项目的内存空间
// 引用: 
//	
// ************************************************

static _LPLISTITEM InsertItem( _LPLISTDATA lpListData, int * index )
{
    if(	lpListData->count == lpListData->limit )
  	{	// 没有更多的列表项目的内存空间，需要新增列表项目
		// 每次新增固定数目的项目空间以备以后使用
        if( ResetLimit( lpListData, (lpListData->limit + ITEMEXT) ) == FALSE )
            return 0;		//分配失败
    }

    if( *index == -1 )
    {	//新项目在末尾
        *index = lpListData->count;
    }
    else
    {	//新项目在列表框中间，需要移动已经存在的项目
		//[oooooooooo new oooooooold]
        memmove( lpListData->lpItems + *index + 1,
		         lpListData->lpItems + *index,
			       (lpListData->count - *index) * sizeof( _LISTITEM ) );
    }
	//初始化新的
    memset( lpListData->lpItems + *index, 0, sizeof(_LISTITEM) );
    lpListData->count++;	//更新实际项目数
	//返回新项目
    return lpListData->lpItems + *index;
}

// **************************************************
// 声明：static BOOL RemoveItem( _LPLISTDATA lpListData, int index )
// 参数：
//	IN/OUT lpListData - 列表框对象指针
//	IN index - 列表项目的索引号
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSe
// 功能描述：
//	移出列表项目,但并不减少整个列表项目的内存空间
// 引用: 
//	
// ************************************************

static BOOL RemoveItem( _LPLISTDATA lpListData, int index )
{
    _ASSERT( (DWORD)index < lpListData->count );
	
	//if( index >= 0 && index < lpListData->count )
	if( (DWORD)index < lpListData->count )
	{	// 索引号合法
		lpListData->count--;
		// 通过移动内存的方法移出该项目
		// [  IIIIIIII  (remove item)   IIIIIIIIIII ]
		memmove( lpListData->lpItems + index,
			lpListData->lpItems + index + 1,
			(lpListData->count - index) * sizeof( _LISTITEM ) );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：static int DeleteItem( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData, int index )
// 参数：
//	IN hWnd - 列表窗口句柄
//	IN dwStyle - 列表窗口风格
//	IN/OUT lpListData - 列表框对象指针
//	IN index - 列表项目索引号
// 返回值：
//	假如成功，返回返回当前列表框实际的列表项目数; 否则，返回LB_ERR
// 功能描述：
//	删除列表项目内容，并移出项目结构
//	返回当前列表框实际的列表项目数
// 引用: 
//	
// ************************************************

static int DeleteItem( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData, int index )
{
    _LPLISTITEM lpItem;
	lpItem = At( lpListData, index );
	
    if( dwStyle & LBS_HASSTRINGS )
	{	//列表项目为字符串风格项目内容，释放该项目内容
		FreeItem( lpItem->lpszItem );
	}
	else if( dwStyle & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE) )
	{	//列表项目为用户自绘风格，必须让用户去释放项目数据
        DELETEITEMSTRUCT ds;
		ds.itemData = lpItem->dwItemData;
		if( ds.itemData )
		{	//准备发送的数据
			ds.CtlType = ODT_LISTBOX;
			ds.CtlID = (WORD)GetWindowLong( hWnd, GWL_ID );
			ds.itemID = index;
			ds.hwndItem = hWnd;
			// 向父窗口发送数据
			SendMessage( GetParent( hWnd ), WM_DELETEITEM, ds.CtlID, (LPARAM)&ds );
		}		
	}
	else
		return LB_ERR;	//错误
    //移出项目结构
	RemoveItem( lpListData, index );
    return lpListData->count;
}

// **************************************************
// 声明：static int SetItemData( _LPLISTDATA lpListData, int index, DWORD dwData )
// 参数：
//	IN/OUT lpListData - 列表框对象指针
//	IN index - 列表项目索引号
//	IN dwData - 需要设置的数据
// 返回值：
//	假如成功，返回0; 否则，返回LB_ERR
// 功能描述：
//	设置项目用户数据
// 引用: 
//	
// ************************************************

static BOOL SetItemData( _LPLISTDATA lpListData, int index, DWORD dwData )
{
    _ASSERT( index >=0 && index < lpListData->count );
	(lpListData->lpItems + index)->dwItemData = dwData;
    return 0;
}

// **************************************************
// 声明：static BOOL ResetLimit( _LPLISTDATA lpListData, int newLimit )
// 参数：
//	IN/OUT lpListData - 列表框对象指针
//	IN newLimit - 新的项目内存空间
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	重新分配项目内存空间
// 引用: 
//	
// ************************************************

static BOOL ResetLimit( _LPLISTDATA lpListData, int newLimit )
{
    _LPLISTITEM lpItem;
	// 必须大于固定的项目数
    if( newLimit < ITEMEXT )
        newLimit = ITEMEXT;	
    if( newLimit != lpListData->limit )
    {	//重新分配
        lpItem = (_LPLISTITEM)realloc( lpListData->lpItems, newLimit*sizeof( _LISTITEM ) );
        if( lpItem )
        {	//分配成功，设置新的列表框数据
            lpListData->lpItems = lpItem;
            lpListData->limit = newLimit;
        }
        else
            return FALSE;
    }
    return TRUE;
}

// **************************************************
// 声明：static int SetItemTypeData( _LPLISTDATA lpListData, int index, WORD dwTypeData )
// 参数：
//	IN/OUT lpListData - 列表框对象指针
//	IN index - 列表项目索引号
//	IN dwTypeData - 需要设置的类型数据
// 返回值：
//	假如成功，返回0; 否则，返回LB_ERR
// 功能描述：
//	设置项目类型数据
// 引用: 
//	
// ************************************************

static int SetItemTypeData( _LPLISTDATA lpListData, int index, WORD dwTypeData )
{
    _ASSERT( index >=0 && index < lpListData->count );
    (lpListData->lpItems+index)->typeData = dwTypeData;
    return 0;
}

// **************************************************
// 声明：static int SetItemState( _LPLISTDATA lpListData, int index, WORD state, BOOL bEnable )
// 参数：
//	IN/OUT lpListData - 列表框对象指针
//	IN index - 列表项目索引号
//	IN state - 列表项目状态
//	IN bEnable - 表示设置或是清除项目状态
// 返回值：
//	假如成功，返回0; 否则，返回LB_ERR
// 功能描述：
//	设置或清除项目状态
// 引用: 
//	
// ************************************************

static int SetItemState( _LPLISTDATA lpListData, int index, WORD state, BOOL bEnable )
{
    _ASSERT( index >=0 && index < lpListData->count );

    if( bEnable )
    {	//设置
        (lpListData->lpItems+index)->state |= state;
    }
    else	//清除
        (lpListData->lpItems+index)->state &= ~state;
    return 0;
}

// **************************************************
// 声明：static BOOL FreeItem( LPSTR lpstr )
// 参数：
//	IN lpstr - 字符串指针
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	释放项目内容
// 引用: 
//	
// ************************************************

static BOOL FreeItem( LPSTR lpstr )
{
    free( lpstr );
    return TRUE;
}

// **************************************************
// 声明：static _LPLISTITEM At( _LPLISTDATA lpListData, int index )
// 参数：
//	IN lpListData - 列表框对象指针
//	IN index - 列表项目索引号
// 返回值：
//	假如成功，返回列表项目指针; 否则，返回NULL
// 功能描述：
//	得到列表项目索引号对应的列表项目指针对象
// 引用: 
//	
// ************************************************

static _LPLISTITEM At( _LPLISTDATA lpListData, int index )
{
    _ASSERT( index >= 0 && index < lpListData->count );
    return lpListData->lpItems + index;
}

// **************************************************
// 声明：static int Count( _LPLISTDATA lpListData )
// 参数：
//	IN lpListData - 列表框对象指针
// 返回值：
//	返回列表框的实际项目数
// 功能描述：
//	统计列表框的实际项目数
// 引用: 
//	
// ************************************************

static int Count( _LPLISTDATA lpListData )
{
    return lpListData->count;
}

// **************************************************
// 声明：static BOOL Pack( _LPLISTDATA lpListData )
// 参数：
//	IN lpListData - 列表框对象指针
// 返回值：
//	假如成功，返回; 否则，返回0
// 功能描述：
//	
// 引用: 
//	
// ************************************************

//static BOOL Pack( _LPLISTDATA lpListData )
//{
//    return TRUE;
//}

// **************************************************
// 声明：static LRESULT DoADDFILE( HWND hWnd, LPCSTR lpName )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpName - 文件名
// 返回值：
//	假如成功，返回增加的项目索引号; 否则，返回LB_ERR
// 功能描述：
//	中间文件名到列表框
//	暂不支持
// 引用: 
//	
// ************************************************

static LRESULT DoADDFILE( HWND hWnd, LPCSTR lpName )
{
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoADDSTRING( HWND hWnd, LPCSTR lpcstr )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpcstr - 字符串项目
// 返回值：
//	假如成功，返回新增项目索引号; 否则，假如没有内存, 返回 LB_ERRSPACE, 其它错误返回LB_ERR
// 功能描述：
//	增加一个字符串项目到末尾
//	处理 LB_ADDSTRING 消息
// 引用: 
//	
// ************************************************

static LRESULT DoADDSTRING( HWND hWnd, LPCSTR lpcstr )
{
    return DoINSERTSTRING( hWnd, -1, lpcstr );
}

// **************************************************
// 声明：static void FillSolidRect( HDC hdc, const RECT * lprc, COLORREF clr )
// 参数：
//	IN hdc - 绘图DC 
//	IN lprc - 需要填充的矩形
//	IN clr - 颜色
// 返回值：
//	无
// 功能描述：
//	填充矩形框
// 引用: 
//	
// ************************************************

static void FillSolidRect( HDC hdc, const RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

// **************************************************
// 声明：static void ReflashScrollBar( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData )
// 参数：
//	IN hWnd - 列表框窗口句柄
//	IN dwStyle - 窗口风格
//	IN lpListData - 列表框对象指针 
// 返回值：
//	假如成功，返回; 否则，返回0
// 功能描述：
//	刷新滚动棒数据和显示
// 引用: 
//	
// ************************************************

static void ReflashScrollBar( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData )
{
    SetScrollBar( hWnd, lpListData->topIndex, 0 );
}

// **************************************************
// 声明：static LRESULT DoINSERTSTRING( HWND hWnd, int index, LPCSTR lpcstr )
// 参数：
//	IN hWnd - 窗口句柄 
//	IN index - 需要插入的位置 
//	IN lpcstr - 需要插入的字符串
// 返回值：
//	假如成功，返回插入的索引号; 否则，假如没有内存, 返回LB_ERRSPACE, 其它错误, 返回LB_ERR
// 功能描述：
//	插入一个字符串项目到指定的位置
//	处理 LB_INSERTSTRING
// 引用: 
//	
// ************************************************

static LRESULT DoINSERTSTRING( HWND hWnd, int index, LPCSTR lpcstr )
{
    _LPLISTDATA lpListData;
    _LPLISTITEM lpItem;
    DWORD dwStyle;
    WORD len;
    LPSTR lp = NULL;
    int cur;    

    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//得到列表框私有数据指针
	// 得到窗口风格
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    if( index > lpListData->limit )
    {	// 错误位置
        return LB_ERR;
    }
	if( dwStyle & LBS_HASSTRINGS )
	{	// 列表框项目为字符串, 分配新的字符串内存
		len = (WORD)strlen( lpcstr );
		lp = malloc( len + 1 );
		if( lp == NULL )
			return LBN_ERRSPACE;

	}
	// 假如可能,隐含当前选择项
	cur = lpListData->curSel;
	DoSETCURSEL( hWnd, -1, FALSE );  // hide cur-sel
	// 插入
	lpItem = InsertItem( lpListData, &index );
	if( lpItem )
	{	//插入成功
		if( dwStyle & LBS_HASSTRINGS )
		{	// 如果是字符串类型, 拷贝内容
			lpItem->state = 0;
			lpItem->typeData = len;
			lpItem->lpszItem = lp;
			strcpy( lpItem->lpszItem, lpcstr );
		}
		else
		{   // 用户自定义数据类型 LBS_OWNERDRAWVARIABLE ..
			lpItem->lpszItem = NULL;
			lpItem->state = 0;
			lpItem->typeData = 0;
			lpItem->dwItemData = (DWORD)lpcstr;
		}
		if( dwStyle & LBS_OWNERDRAWVARIABLE )
		{	// 如果是自定义数据类型, 发送消息到父窗口去得到项目高度
			MEASUREITEMSTRUCT mis;

			mis.CtlID = GetWindowLong( hWnd, GWL_ID );
			mis.CtlType = ODT_LISTBOX;
			mis.itemData = (DWORD)lpcstr;
			mis.itemHeight = 0;
			mis.itemID = index;
			mis.itemWidth = 0;
			if( SendMessage( GetParent( hWnd ), WM_MEASUREITEM, mis.CtlID, (LPARAM)&mis ) )
			{
                lpItem->typeData = (short)mis.itemHeight;
			}
			else
				lpItem->typeData = 1;  // safe value
		}
		// 修正当前选择项的索引号
		if( index <= cur && !(dwStyle & LBS_MULTIPLESEL) )
			cur++;
		
		if( !(dwStyle & LBS_NOREDRAW) )
		{	// 需要重绘
			HDC hdc;
			// 得到绘图DC
			hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
			// 绘图项目
			RedrawItem( lpListData, hWnd, dwStyle, hdc, index, lpListData->count, FALSE );
			// 释放DC
			ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
			// 刷新滚动棒
			ReflashScrollBar( hWnd, dwStyle, lpListData );
		}
		// 显示选择项
		DoSETCURSEL( hWnd, cur, FALSE );  // show cur-sel
		return index;
	}
	else
	{	//插入不成功, 释放之前的数据
		if( lp )
			free( lp );
	}
	//	重新显示之前隐藏的项目
	DoSETCURSEL( hWnd, cur, FALSE );  // show cur-sel
	// 向父窗口发送通知消息
	SendNotify( hWnd, LBN_ERRSPACE );
	return LBN_ERRSPACE;
}

// **************************************************
// 声明：LRESULT DoDELETESTRING( HWND hWnd, int index )
// 参数：
//	IN hWnd - 窗口句柄 
//	IN index - 项目索引号
// 返回值：
//	假如成功，返回当前列表框项目数; 否则，返回LB_ERR
// 功能描述：
//	删除索引号对应的项目
//	处理 LB_DELETESTRING
// 引用: 
//	
// ************************************************

LRESULT DoDELETESTRING( HWND hWnd, int index )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );	//得到列表框窗口的列表框数据对象指针
    short retv = LB_ERR, cur;
    DWORD dwStyle;
	RECT rect;

    _ASSERT( lpListData );

    //if( index < lpListData->count && index >= 0 )
	if( (DWORD)index < lpListData->count )
    {	//合法的索引号值
        cur = lpListData->curSel;
		//假如有选择项或标注项,先隐含它们
        if( cur != -1 )
		    DoSETCURSEL( hWnd, -1, FALSE );  // hide cur-sel
		if( lpListData->caret != -1 )    // hide cur-caret
			DoSETCARETINDEX( hWnd, -1, FALSE );
		//得到窗口风格
		dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		//删除项目
        retv = DeleteItem( hWnd, dwStyle, lpListData, index );
		//得到窗口客户区矩形坐标
		GetClientRect( hWnd, &rect );
		// reset the topIndex
		// 重新设置当前窗口的第一个项目
		if( (dwStyle & LBS_OWNERDRAWVARIABLE) == 0 )			
		{	// 得到窗口矩形能够显示的最大行数
			int rows = GetRows( lpListData, dwStyle, &rect );
			int t = rows;
			int iTopIndex;
			if( dwStyle & LBS_MULTICOLUMN )
			{   // 多列显示风格, 得到列数
				int cols = GetCols( lpListData, &rect );
				t = t * cols;	// 当前窗口所能显示的最大项目数
			}
			if( cur < lpListData->topIndex  )
			{	//选择项目索引号比窗口的最顶项目号小
				iTopIndex = cur - 1 < 0 ? 0 : cur;
			}
			else if( cur >= lpListData->topIndex + t )
			{	//选择项目索引号比窗口的最底项目号大
				iTopIndex = cur - t;
			}
			else	//
				iTopIndex = lpListData->topIndex;
			//确定最后一页的最顶项目索引号
			t -= (lpListData->count - iTopIndex ); 
			if( t > 0  )
			{	//当前的最顶项目索引号(iTopIndex)之下 没有足够的项目显示最后一页
				t = iTopIndex - t;
				if( t < 0 )
					t = 0;	//最顶项目索引号应该为0
			}
			else
				t = iTopIndex;	//足够
            DoSETTOPINDEX( hWnd, t );
		}

		if( !(dwStyle & LBS_NOREDRAW) )
		{	// 需要重绘风格                
			HDC hdc;
			
			hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
			if( index == lpListData->count )// the bottom item
			{
				RedrawItem( lpListData, hWnd, dwStyle, hdc, index-1, lpListData->count, TRUE );
			}
			else
			    RedrawItem( lpListData, hWnd, dwStyle, hdc, index, lpListData->count, TRUE );
			ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );	//
			ReflashScrollBar( hWnd, dwStyle, lpListData );
		}

        //if( cur >= 0 )
            //DoSETCURSEL( hWnd, cur, FALSE );
    }
    return retv;
}

// **************************************************
// 声明：static LRESULT DoDIR( HWND hWnd, WORD attrib, LPCSTR lpName )
// 参数：
//	IN hWnd - 窗口句柄
//	IN attrib - 文件属性
//	IN lpName - 文件名
// 返回值：
//	假如成功，返回最后增加的项目索引号; 否则，假如没有内存, 返回LB_ERRSPACE, 其它错误返回LB_ERR
// 功能描述：
//	增加文件或文件夹中的文件到列表框
//	处理 LB_DIR
// 引用: 
//	
// ************************************************

static LRESULT DoDIR( HWND hWnd, WORD attrib, LPCSTR lpName )
{
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoFINDSTRING( HWND hWnd, int indexStart, LPCSTR lpFindStr, BOOL bExact )
// 参数：
//	IN hWnd - 窗口句柄 
//	IN indexStart - 搜索的起始索引号 
//	IN lpFindStr - 需要匹配的字符串 
//	IN bExact - 是否精确匹配(精确匹配意味其长度也要匹配)
// 返回值：
//	假如成功，返回项目索引号; 否则，返回LB_ERR
// 功能描述：
//	搜索匹配的项目
//	处理 LB_FINDSTRING 和 LB_FINDSTRINGEXACT
// 引用: 
//	
// ************************************************

static LRESULT DoFINDSTRING( HWND hWnd, int indexStart, LPCSTR lpFindStr, BOOL bExact )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//得到列表框窗口的列表框数据对象指针
    short i;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );//得到列表框风格

    _ASSERT( lpListData );
    if( indexStart == -1 )
        indexStart = 0;	//从0开始
	// 从indexStart开始搜索所有的项目
    for( i = 0; i < lpListData->count; i++, indexStart++ )
    {
        if( indexStart >= lpListData->count )
            indexStart = 0;	// 如果没有搜索完所有的项目,从第一个开始
        if( dwStyle & LBS_HASSTRINGS )
        {	// 字符串类型
            if( bExact )
			{	// 精确匹配
                if( stricmp( At( lpListData, indexStart )->lpszItem, lpFindStr ) == 0 )
                    return indexStart;
			}
			else
			{	// 查找字符串
                if( strstr( At( lpListData, indexStart )->lpszItem, lpFindStr ) )
                    return indexStart;
			}
        }
        else if( dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE) )
        {	// 用户数据
			if( At( lpListData, indexStart )->dwItemData == (DWORD)lpFindStr )
                return indexStart;
        }
        else
            break; // error
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETANCHORINDEX( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回列表框被选择项目中的第一个项目索引; 否则，返回LB_ERR
//	
// 功能描述：
//	得到列表框被选择项目中的第一个项目索引（用于多行选择）
//	处理 LB_GETANCHORINDEX 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETANCHORINDEX( HWND hWnd )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    int i;

    _ASSERT( lpListData );
    for( i = 0; i < lpListData->count; i++ )
    {
        if( At( lpListData, i )->state & ODS_SELECTED )
            return i;		//找到
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETCARETINDEX( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回列表框被选择项目中的最后一个项目索引; 否则，返回LB_ERR
// 功能描述：
//	得到列表框被选择项目中的最后一个项目索引（用于多行选择）
//	处理 LB_GETCARETINDEX
// 引用: 
//	
// ************************************************

static LRESULT DoGETCARETINDEX( HWND hWnd )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    return lpListData->caret;
}

// **************************************************
// 声明：static LRESULT DoGETCOUNT( HWND hWnd )
// 参数：
//	IN  hWnd - 窗口句柄
// 返回值：
//	假如成功，返回列表框的项目总数; 否则，返回LB_ERR
// 功能描述：
//	得到列表框的项目总数
//	处理 LB_GETCOUNT
// 引用: 
//	
// ************************************************

static LRESULT DoGETCOUNT( HWND hWnd )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    return Count( lpListData );
}

// **************************************************
// 声明：static LRESULT DoGETCURSEL( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回列表框被选择项目中的有焦点的项目索引; 否则，返回LB_ERR
//	
// 功能描述：
//	如果列表框是多行选择风格，得到列表框被选择项目中的有焦点的项目索引
//	如果列表框是单行选择风格，得到列表框被选择项目的项目索引
//	处理 LB_GETCURSEL 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETCURSEL( HWND hWnd )
{	// 得到窗口风格
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    //得到列表框窗口的列表框数据对象指针
	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    int i;

    _ASSERT( lpListData );

    if( dwStyle & LBS_MULTIPLESEL )
    {	// 多行选择风格
        for( i = 0; i < lpListData->count; i++ )
        {	
            if( At( lpListData, i )->state & ODS_FOCUS )
                return i;	//
        }
    }
    else
    {   // 单行选择 single-sel style
        return lpListData->curSel;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETHORIZONTALEXTENT( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回列表框的水平宽度; 否则，返回LB_ERR
//	
// 功能描述：
//	得到之前调用 LB_SETHORIZONTALEXTENT 设置的水平宽度
//	处理 LB_GETHORIZONTALEXTENT 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETHORIZONTALEXTENT( HWND hWnd )
{
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETITEMDATA( HWND hWnd, int index )
// 参数：
//	IN hWnd - 窗口句柄
//  IN index - 项目索引
// 返回值：
//	假如成功，返回列表框项目的项目数据; 否则，返回LB_ERR
// 功能描述：
//	得到列表框项目的项目数据
//	处理 LB_GETITEMDATA 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETITEMDATA( HWND hWnd, int index )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );
    if( (DWORD)index < lpListData->count )
    {	// 有效的索引
		return (LRESULT)At( lpListData, index )->dwItemData;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETITEMHEIGHT( HWND hWnd, int index )
// 参数：
//	IN hWnd - 窗口句柄
//  IN index - 项目索引
// 返回值：
//	假如成功，返回列表框项目的项目高度; 否则，返回LB_ERR
// 功能描述：
//	得到列表框项目的项目高度（ 列表框必须有 LBS_OWNERDRAWVARIABLE 风格；否则index必须为0 ）
//	处理 LB_GETITEMHEIGHT 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETITEMHEIGHT( HWND hWnd, int index )
{	// 得到窗口风格
    DWORD dwStyle = (DWORD)GetWindowLong( hWnd, GWL_STYLE );
    //得到列表框窗口的列表框数据对象指针
	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );
    if( (DWORD)index < lpListData->count )
    {	
        if(  dwStyle & LBS_OWNERDRAWVARIABLE )
            return At( lpListData, index )->typeData;
        else if( index == 0 )
            return lpListData->rowHeight;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETITEMRECT( HWND hWnd, int index, LPRECT lpRect )
// 参数：
//	IN hWnd - 窗口句柄 
//	IN index - 项目索引 
//	OUT lpRect - 用于接受项目矩形
// 返回值：
//	假如成功，返回0，lpRect返回项目矩形坐标; 否则，返回LB_ERR
// 功能描述：
//	得到项目矩形坐标
//	处理 LB_GETITEMRECT 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETITEMRECT( HWND hWnd, int index, LPRECT lpRect )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );

    GetClientRect( hWnd, lpRect );
    if( GetItemRect( lpListData , index, lpRect, GetWindowLong( hWnd, GWL_STYLE ) ) )
        return 0;
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETLOCALE( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回; 否则，返回LB_ERR
// 功能描述：
//	不支持
// 引用: 
//	
// ************************************************

static LRESULT DoGETLOCALE( HWND hWnd )
{
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETSEL( HWND hWnd, int index )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 项目索引
// 返回值：
//	假如成功，返回当前列表框被选择的项目; 否则，返回 LB_ERR
// 功能描述：
//	得到列表框被选择的项目
//	处理 LB_GETSEL 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETSEL( HWND hWnd, int index )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );

    if( (DWORD)index < lpListData->count )
    {
        return At( lpListData, index )->state & ODS_SELECTED;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETSELCOUNT( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回当前列表框被选择的项目数; 否则，返回 LB_ERR
// 功能描述：
//	得到列表框被选择的项目数（用于多行选择）
//	处理 LB_GETSELCOUNT 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETSELCOUNT( HWND hWnd )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    int i, n = 0;

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {	//多行风格
        _ASSERT( lpListData );
        for( i = 0; i < lpListData->count; i++ )
        {
            if( At( lpListData, i )->state & ODS_SELECTED )
                n++;
        }
        return n;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETSELITEMS( HWND hWnd, int maxItems, LPINT lpArray )
// 参数：
//	IN hWnd - 窗口句柄
//	IN maxItems - 需要返回的最大项目数
//	IN lpArray - 用于接受项目索引号的数组
// 返回值：
//	假如成功，返回实际拷贝的项目索引数; 否则，返回 LB_ERR
// 功能描述：
//	得到列表框中当前处于被选择状态的项目数,适用于有 LBS_MULTIPLESEL 风格的列表框
//	处理 LB_GETSELITEMS 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETSELITEMS( HWND hWnd, int maxItems, LPINT lpArray )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    int i, m = 0;

    _ASSERT( lpListData );

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {	//
        for( i = 0; i < lpListData->count && m < maxItems ; i++ )
        {
            if( At( lpListData, i )->state & ODS_SELECTED )
            {	// 符合条件
                *lpArray++ = i;
                m++;
            }
        }
        return m;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETTEXT( HWND hWnd, int index, void * lpBuf )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 项目索引
//	OUT lpBuf - 用于接受项目文本的缓存
// 返回值：
//	假如成功，返回实际拷贝的内容长度; 否则，返回 LB_ERR
// 功能描述：
//	得到指定项目文本内容
//	处理 LB_GETTEXT 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETTEXT( HWND hWnd, int index, void * lpBuf )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _LPLISTITEM lpItem;

    _ASSERT( lpListData );
    if( (DWORD)index < lpListData->count )
    {
        lpItem = At( lpListData, index );
		if( (DWORD)GetWindowLong( hWnd, GWL_STYLE ) & LBS_HASSTRINGS )
        {	// 是字符串内容
            strcpy( lpBuf, lpItem->lpszItem );
            return lpItem->typeData;
        }
        else
        {	// 用户自定义内容，拷贝其设定的值
			memcpy( lpBuf, &lpItem->dwItemData, 4 );
            return 4;
        }
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETTEXTLEN( HWND hWnd, int index )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 项目索引
// 返回值：
//	假如成功，返回项目字符串内容长度; 否则，返回 LB_ERR
// 功能描述：
//	得到指定项目字符串内容长度
//	处理 LB_GETTEXTLEN 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETTEXTLEN( HWND hWnd, int index )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    if( (DWORD)index < lpListData->count )
    {
        if( (DWORD)GetWindowLong( hWnd, GWL_STYLE ) & LBS_HASSTRINGS )
            return At( lpListData, index )->typeData;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoGETTOPINDEX( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回列表框窗口的第一条项目的索引值; 否则，返回 LB_ERR
// 功能描述：
//	得到列表框窗口的第一条项目的索引值
//	处理 LB_GETTOPINDEX 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGETTOPINDEX( HWND hWnd )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );
    return lpListData->topIndex;
}

// **************************************************
// 声明：static LRESULT DoRESETCONTENT( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	假如成功，返回0; 否则，返回 LB_ERR
// 功能描述：
//	清除列表框的所有项目
//	处理 LB_RESETCONTENT 消息
// 引用: 
//	
// ************************************************

static LRESULT DoRESETCONTENT( HWND hWnd )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    DWORD dwStyle = (DWORD)GetWindowLong( hWnd, GWL_STYLE );
    int i, fOwner;

    _ASSERT( lpListData );
	//是否自绘风格
    fOwner = (int)(dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE));
	//删除每一个项目
    for( i = lpListData->count - 1; i >= 0; i-- )
    {
		DeleteItem( hWnd, dwStyle, lpListData, i );
    }
    lpListData->caret = -1;
    lpListData->curSel = -1;
	lpListData->topIndex = 0;
	lpListData->limit = 0;
	if( !fOwner )
	{	// 非自绘风格，更新窗口
		InvalidateRect( hWnd, NULL, FALSE );
		UpdateWindow( hWnd );
	}
	//2005-01-26, add code
	ReflashScrollBar( hWnd, dwStyle, lpListData );
	//2005-01-26

    return 0;
}

// **************************************************
// 声明：static LRESULT DoSELECTSTRING( HWND hWnd, int indexStart, LPCSTR lpcstr )
// 参数：
//	IN hWnd - 窗口句柄
//	IN indexStart - 项目索引
//	IN lpcstr - 需要匹配的字符串
// 返回值：
//	假如成功，返回设置的项目索引号
// 功能描述：
//	将字符串匹配的项目设为被选择项
//	处理 LB_SELECTSTRING 消息
// 引用: 
//	
// ************************************************

static LRESULT DoSELECTSTRING( HWND hWnd, int indexStart, LPCSTR lpcstr )
{
    indexStart = DoFINDSTRING( hWnd, indexStart, lpcstr, FALSE );
    if( indexStart != LB_ERR )
    {
         DoSETSEL( hWnd, TRUE, indexStart );
    }
    return indexStart;
}

// **************************************************
// 声明：static LRESULT DoSELITEMRANGE( HWND hWnd, BOOL fSelect, int first, int last )
// 参数：
//	IN hWnd - 窗口句柄
//	IN fSelect - 是否将项目设为选择状态或是清除其选择状态
//	IN first - 起始项目索引
//	IN last - 结束项目索引
// 返回值：
//	假如成功，返回0；否则，返回 LB_ERR
// 功能描述：
//	将从 first开始 到 last 结束的项目设为选择状态，适用于有 LBS_MULTIPLESEL 风格的列表框
//	处理 LB_SELITEMRANGE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoSELITEMRANGE( HWND hWnd, BOOL fSelect, int first, int last )
{
    int i;

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {
        for( i = first; i <= last; i++ )
            DoSETSEL( hWnd, fSelect, i );
        return 0;
    }
    else
        return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoSETANCHORINDEX( HWND hWnd, int index )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 项目索引
// 返回值：
//	返回0
// 功能描述：
//	设置列表框的 ANCHOR 项
//	处理 LB_SETANCHORINDEX 消息
// 引用: 
//	
// ************************************************

static LRESULT DoSETANCHORINDEX( HWND hWnd, int index )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    DoSELITEMRANGE( hWnd, FALSE, 0, lpListData->count-1 );
    DoSETSEL( hWnd, TRUE, index );
    return 0;
}

// **************************************************
// 声明：static LRESULT DoSETCARETINDEX( HWND hWnd, int index, BOOL fScroll )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 项目索引
//	IN fScroll - 滚动功能。假如为FALSE, 则将滚动该项目直到其完全可视；否则，局部可视也行
// 返回值：
//	假如成功，返回0；否则，返回 LB_ERR
// 功能描述：
//	设置列表框的 CARET 项
//	处理 LB_SETCARETINDEX 消息
// 引用: 
//	
// ************************************************

static LRESULT DoSETCARETINDEX( HWND hWnd, int index, BOOL fScroll )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    RECT rect, clientRect;
    HDC hdc;
    DWORD dwStyle;
	int iRowNum;

    _ASSERT( lpListData );
	//得到客户区坐标
    GetClientRect( hWnd, &clientRect );
	//得到窗口风格
    dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//得到窗口所能容纳的行数
	iRowNum = GetRows( lpListData, dwStyle, &clientRect );

    if( index != lpListData->caret &&
		index >= -1 &&
        index < lpListData->count )
    {	// 有效并需要重新设置
		if( index >= 0 ) 
        {   // 调整项目直到其可视
			rect = clientRect;
			//得到项目矩形坐标
			GetItemRect( lpListData, index, &rect, dwStyle );
			if( !IntersectRect( &rect, &rect, &clientRect ) )	//可视吗 ？
			{	//不可视
                if( dwStyle & LBS_MULTICOLUMN )
                {	//如果列表框是多列风格，则滚动列
                    ScrollCol( hWnd,
						index / iRowNum -
						lpListData->topIndex / iRowNum );
                }
                else
                {	//单列，滚动行
                    if( index < lpListData->topIndex )
                        ScrollRow( hWnd, (index - lpListData->topIndex) );
                    else
                        ScrollRow( hWnd, (index - (lpListData->topIndex+iRowNum-1) ) );
                }
			}
        }
		// 假如可能，重新设置 并绘制 caret
        if( lpListData->caret != index )
        {	//分配绘图DC
            hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
            if( lpListData->caret != -1 )
            {   // 存在老的caret, 清除它  remove old caret
				rect = clientRect;
                GetItemRect( lpListData, lpListData->caret, &rect, dwStyle );
                DrawItemContents ( hWnd, lpListData, dwStyle, hdc, lpListData->caret, &rect, FALSE, ODA_SELECT );
            }
			if( index >= 0 && (dwStyle & (LBS_OWNERDRAWVARIABLE|LBS_OWNERDRAWFIXED)) == 0 )
			{   // 显示新的 caret show new
				rect = clientRect;
				GetItemRect( lpListData, index, &rect, dwStyle );
				DrawCaret( hdc, &rect );
			}
			//设置 caret
            lpListData->caret = index;
			//释放绘图DC
            ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
        }
        return 0;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoSETCOLUMNWIDTH( HWND hWnd, int cxWidth )
// 参数：
//	IN hWnd - 窗口句柄
//	IN cxWidth - 新的列宽
// 返回值：
//	假如成功，返回0；否则，返回 LB_ERR
// 功能描述：
//	设置列表框的列宽
//	处理 LB_SETCOLUMNWIDTH 消息，适用于有 LBS_MULTICOLUMN 风格的列表框
// 引用: 
//	
// ************************************************

static LRESULT DoSETCOLUMNWIDTH( HWND hWnd, int cxWidth )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    RECT rect;

    if( (GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTICOLUMN) && cxWidth > 0 )
    {	//有效
        lpListData->colWidth = cxWidth;
		//得到窗口的客户区
        GetClientRect( hWnd, &rect );
        //新的列数
		lpListData->colNum = GetCols( lpListData, &rect );
		//无效窗口客户区
        InvalidateRect( hWnd, 0, TRUE );
        return 0;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoSETCOUNT( HWND hWnd, short count )
// 参数：
//	IN hWnd - 窗口句柄
//	IN count - 新的项目数
// 返回值：
//	假如成功，返回0；否则，假如没有内存，返回LB_ERRSPACE, 其它错误返回 LB_ERR
// 功能描述：
//	设置列表框的可以容纳的项目数
//	处理 LB_SETCOUNT 消息，适用于有 LBS_NODATA 和非 LBS_HASSTRINGS 风格的列表框
// 引用: 
//	
// ************************************************

static LRESULT DoSETCOUNT( HWND hWnd, short count )
{	//得到窗口风格
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    if( (dwStyle & LBS_NODATA) && (dwStyle & LBS_HASSTRINGS)==0 )
    {	//得到列表框窗口的列表框数据对象指针
        lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
        _ASSERT( lpListData );
        if( count != Count( lpListData ) )
        {	//需要重设
            if( ResetLimit( lpListData, count ) == FALSE )
            {	//失败，向父发送LBN_ERRSPACE消息
                SendNotify( hWnd, LBN_ERRSPACE );
                return LB_ERRSPACE;
            }
        }
        return 0;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static void ShowSelectIndex( _LPLISTDATA lpListData, 
//					  HWND hWnd, 
//					  int index, 
//					  HDC hdc, 
//					  LPCRECT lpcrect, 
//					  DWORD dwStyle, 
//					  BOOL fShow )

// 参数：
//	IN lpListData - 列表框对象指针
//	IN hWnd - 窗口句柄
//	IN index - 项目号
//	IN hdc - 绘图DC
//	IN lpcrect - 列表框窗口矩形
//	IN dwStyle - 列表框风格
//	IN fShow - 是否设置或清除选择状态
// 返回值：
//	无
// 功能描述：
//	设置列表框的可以容纳的项目数
//	处理 LB_SETCOUNT 消息，适用于有 LBS_NODATA 和非 LBS_HASSTRINGS 风格的列表框
// 引用: 
//	
// ************************************************

static void ShowSelectIndex( _LPLISTDATA lpListData, 
					  HWND hWnd, 
					  int index, 
					  HDC hdc, 
					  LPCRECT lpcrect, 
					  DWORD dwStyle, 
					  BOOL fShow )
{
    RECT rect = *lpcrect;
    _ASSERT( index >= 0 );
    SetItemState( lpListData, index, ODS_SELECTED, fShow );
	//得到项目矩形
	GetItemRect( lpListData, index, &rect, dwStyle );
	//画项目内容
    DrawItemContents ( hWnd, lpListData, dwStyle, hdc, index, &rect, index == lpListData->caret, ODA_SELECT );
}

// **************************************************
// 声明：static LRESULT DoSETCURSEL( HWND hWnd, int index, BOOL bNotify )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 列表框项目索引号
//	iN bNotify - 是否向父窗口发通知消息
// 返回值：
//	假如成功，返回被设置的索引值; 否则，返回LB_ERR
// 功能描述：
//	将index设置为当前的选择项目，适用于非多行选择风格的列表框
//	处理 LB_SETCURSEL
// 引用: 
//	
// ************************************************

static LRESULT DoSETCURSEL( HWND hWnd, int index, BOOL bNotify )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    HDC hdc;
    _LPLISTDATA lpListData;
    RECT rect;
    if( (dwStyle & LBS_MULTIPLESEL)==0 && index >= -1 )
    {	//得到列表框窗口的列表框数据对象指针
        lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
        _ASSERT( lpListData );
		// 2004-10-27, modify, 优化
        //if( index < lpListData->count )
		if( index < lpListData->count &&
			index != lpListData->curSel )
		//
        {	//合法
			//得到窗口的客户区坐标
            GetClientRect( hWnd, &rect );
			//分配绘图DC
            hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
            //假如可能，隐含当前的 hide cursel if possible
            if( lpListData->curSel != -1 )
                ShowSelectIndex( lpListData, hWnd, lpListData->curSel, hdc, &rect, dwStyle, FALSE );
			//显示新的
            if( index >= 0 )
                ShowSelectIndex( lpListData, hWnd, index, hdc, &rect, dwStyle, TRUE );
			//设置新的选择项
            lpListData->curSel = index;
			//释放绘图DC
            ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
		}
        return index;
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoSETHORIZONTALEXTENT( HWND hWnd, short cx )
// 参数：
//	IN hWnd - 窗口句柄
//	IN cx - 水平宽度
// 返回值：
//	假如成功，返回; 否则，返回LB_ERR
// 功能描述：
//	处理 LB_SETHORIZONTALEXTENT
// 引用: 
//	
// ************************************************

static LRESULT DoSETHORIZONTALEXTENT( HWND hWnd, short cx )
{
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoSETITEMDATA( HWND hWnd, short index, DWORD dwData )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 列表框项目索引号
//	IN dwData - 项目数据
// 返回值：
//	假如成功，返回0; 否则，返回LB_ERR
// 功能描述：
//	设置项目数据
//	处理 LB_SETITEMDATA
// 引用: 
//	
// ************************************************

static LRESULT DoSETITEMDATA( HWND hWnd, short index, DWORD dwData )
{	//得到窗口风格
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
	//得到列表框窗口的列表框数据对象指针
	lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	_ASSERT( lpListData );
	//if( index < lpListData->count && index >=0 )
	if( (DWORD)index < lpListData->count )
		return SetItemData( lpListData, index, dwData );	//
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoSETITEMHEIGHT( HWND hWnd, short index, short height )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 列表框项目索引号
//	IN height - 新的项目高度
// 返回值：
//	假如成功，返回0; 否则，返回LB_ERR
// 功能描述：
//	设置项目的显示高度
//	处理 LB_SETITEMHEIGHT
// 引用: 
//	
// ************************************************

static LRESULT DoSETITEMHEIGHT( HWND hWnd, short index, short height )
{	//得到窗口风格
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    //RECT rect;
	//得到列表框窗口的列表框数据对象指针
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
    {	//如果是可变风格,设置到index指向的项目中
       //if( index < lpListData->count && index >=0 )
		if( (DWORD)index < lpListData->count )
           return SetItemTypeData( lpListData, index, (WORD)height );
    }
    else
    {
        if( height > 0 && index == 0 )
        {	//高度是统一的
            lpListData->rowHeight = height;
            //GetClientRect( hWnd, &rect );
            InvalidateRect( hWnd, 0, TRUE );
            return 0;
        }
    }
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoSELITEMRANGEEX( HWND hWnd, short first, short last )
// 参数：
//	IN hWnd - 窗口句柄
//	IN first - 起始项目索引号
//	IN last - 结束项目索引号
// 返回值：
//	假如成功，返回0; 否则，返回LB_ERR
// 功能描述：
//	将指定的项目范围设为选择状态( 具有 LBS_MULTIPLESEL 风格有效 )
//	处理 LB_SELITEMRANGEEX 消息
// 引用: 
//	
// ************************************************

static LRESULT DoSELITEMRANGEEX( HWND hWnd, short first, short last )
{	//
    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {	//
        int i;
        if( first < last )
        {
            for( i = first; i < last; i++ )
                DoSETSEL( hWnd, TRUE, i );
        }
        else
        {
            for( i = last; i < first; i++ )
                DoSETSEL( hWnd, FALSE, i );
        }
        return 0;
    }
    else
        return LB_ERR;    
}

// **************************************************
// 声明：
// 参数：
//	IN 
// 返回值：
//	假如成功，返回; 否则，返回0
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static LRESULT DoSETLOCALE( HWND hWnd, WORD idLocale )
{
    return LB_ERR;
} 

// **************************************************
// 声明：static LRESULT DoSETSEL( HWND hWnd, BOOL fSelect, int index )
// 参数：
//	IN hWnd - 窗口句柄
//	IN fSelect - 是否设置为选择状态或非
//	IN index - 项目索引号
// 返回值：
//	假如成功，返回0; 否则，返回LB_ERR
// 功能描述：
//	将指定的项目设为选择状态, 假如 index = -1, 则选择所有或清除所有.( 具有 LBS_MULTIPLESEL 风格有效 )
//	处理 LB_SETSEL 消息
// 引用: 
//	
// ************************************************

static LRESULT DoSETSEL( HWND hWnd, BOOL fSelect, int index )
{	//得到窗口风格
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    HDC hdc;
    short i, retv = LB_ERR;
    RECT rect;
	//得到列表框窗口的列表框数据对象指针
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    if( dwStyle & LBS_MULTIPLESEL )
    {	//多重风格
        //if( index >= 0 && index < lpListData->count )
		if( (DWORD)index < lpListData->count )
        {	//有效的索引值
            retv = SetItemState( lpListData, index, ODS_SELECTED, fSelect );
            SendNotify( hWnd, LBN_SELCHANGE );
			//分配绘图DC
            hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
			//得到窗口客户矩形
            GetClientRect( hWnd, &rect );
			//得到项目矩形
			GetItemRect( lpListData, index, &rect, dwStyle );
			//画项目内容
            DrawItemContents ( hWnd, lpListData, dwStyle, hdc, index, &rect, lpListData->caret == index, ODA_SELECT );
            //释放DC
			ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
        }
        else if( index <= -1 )
        {	//对所有项目做设置或清除工作
            for( i = 0; i < lpListData->count; i++ )
            {
                retv = SetItemState( lpListData, i, ODS_SELECTED, fSelect );
                SendNotify( hWnd, LBN_SELCHANGE );
            }
            InvalidateRect( hWnd, 0, TRUE );
        }
    }
    return retv;
}

// **************************************************
// 声明：static LRESULT DoSETTABSTOPS( HWND hWnd, short num, short * lpArray )
// 参数：
//	IN hWnd - 窗口句柄
//	IN num - 
// 返回值：
//	假如成功，返回; 否则，返回0
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static LRESULT DoSETTABSTOPS( HWND hWnd, short num, short * lpArray )
{
    return LB_ERR;
}

// **************************************************
// 声明：static LRESULT DoSETTOPINDEX( HWND hWnd, int index )
// 参数：
//	IN hWnd - 窗口句柄
//	IN index - 项目索引号
// 返回值：
//	假如成功，返回0; 否则，返回LB_ERR
// 功能描述：
//	设置当前列表框窗口的第一个项目
//	处理 LB_SETTOPINDEX 
// 引用: 
//	
// ************************************************

static LRESULT DoSETTOPINDEX( HWND hWnd, int index )
{
    _LPLISTDATA lpListData;
    //得到列表框窗口的列表框数据对象指针
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    //if( index >= 0 && index < lpListData->count && index != lpListData->topIndex )
	if( (DWORD)index < lpListData->count && index != lpListData->topIndex )
    {	//设置并无效客户区
        lpListData->topIndex = index;
		SetScrollBar( hWnd, lpListData->topIndex, 0 );
        InvalidateRect( hWnd, 0, TRUE );
        return 0;
    }
    return LB_ERR;
}


// **************************************************
// 声明：static VOID _SetListColor( _LPLISTDATA lpListData )
// 参数：
//	IN lpListData - 静态类数据结构
// 返回值：
//	无
// 功能描述：
//	设置窗口显示颜色
// 引用: 
//	
// ************************************************

static VOID _SetListColor( _LPLISTDATA lpListData )
{
	//设置显示颜色
	lpListData->cl_Disable = GetSysColor(COLOR_GRAYTEXT);
	lpListData->cl_DisableBk = GetSysColor(COLOR_WINDOW);
	lpListData->cl_Selection = GetSysColor(COLOR_HIGHLIGHTTEXT);
	lpListData->cl_SelectionBk = GetSysColor(COLOR_HIGHLIGHT);
	lpListData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);
	lpListData->cl_TextBk = GetSysColor(COLOR_WINDOW);

}

// **************************************************
// 声明：static _LPLISTDATA _DoInitListData( HWND hWnd, LPCREATESTRUCT lpcs )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpcs - 窗口创建结构指针
// 返回值：
//	假如成功，返回 _LISTDATA结构指针; 否则，返回 NULL
// 功能描述：
//	分配并初始化 _LISTDATA结构指针
// 引用: 
//	
// ************************************************

static _LPLISTDATA _DoInitListData( HWND hWnd, LPCREATESTRUCT lpcs )
{
    _LPLISTDATA lpListData;
    //TEXTMETRIC tm;
//    HDC hdc;
    RECT rect;
	//分配一个内存结构	
	lpListData = (_LPLISTDATA)malloc( sizeof( _LISTDATA ) );

	if( lpListData )
	{
		memset( lpListData, 0, sizeof( _LISTDATA ) );
        ResetLimit( lpListData, 0 );

		lpListData->dwStyle = lpcs->style;
		lpcs->style &= ~(WS_VSCROLL | WS_HSCROLL);

		if( lpcs->style & LBS_OWNERDRAWFIXED )
		{	//拥有者自定义风格
			MEASUREITEMSTRUCT mis;
			mis.CtlID = (UINT)lpcs->hMenu;
			mis.CtlType = ODT_LISTBOX;
			mis.itemData = 0;
			mis.itemHeight = 0;
			mis.itemID = 0;
			mis.itemWidth = 0;
			//想父窗口得到行高
			if( SendMessage( lpcs->hParent, WM_MEASUREITEM, mis.CtlID, (LPARAM)&mis ) )
			{
                lpListData->rowHeight = (short)mis.itemHeight;
			}	
		}
		//
		if( lpListData->rowHeight == 0 )
		{	//得到系统默认高度
			//hdc = GetDC( hWnd );
			//GetTextMetrics( hdc, &tm );
			//ReleaseDC( hWnd, hdc );
			//(short)tm.tmHeight + 4;
			lpListData->rowHeight = GetSystemMetrics(SM_CYLISTITEM);//GetSystemMetrics(SM_CYCAPTION);
		}
		//初始化其它参数
        lpListData->colWidth = lpcs->cx;
        lpListData->caret = -1;
        lpListData->curSel = -1;

        lpListData->bCombBoxList= FALSE;
        lpListData->hCombBox = NULL;
		//设置显示颜色
		_SetListColor( lpListData );
		//lpListData->cl_Disable = GetSysColor(COLOR_GRAYTEXT);
		//lpListData->cl_DisableBk = GetSysColor(COLOR_WINDOW);
		//lpListData->cl_Selection = GetSysColor(COLOR_HIGHLIGHTTEXT);
		//lpListData->cl_SelectionBk = GetSysColor(COLOR_HIGHLIGHT);
		//lpListData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);
		//lpListData->cl_TextBk = GetSysColor(COLOR_WINDOW);
		//根据当前窗口坐标得到能显示的列数
        GetClientRect( hWnd, &rect );
        lpListData->colNum = GetCols( lpListData, &rect );

		SetWindowLong( hWnd, 0, (long)lpListData );
		SetWindowLong( hWnd, GWL_STYLE, lpcs->style );
	}
	return lpListData;
}

// **************************************************
// 声明：static LRESULT DoCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpcs - 窗口创建结构指针
// 返回值：
//	假如成功，返回0; 否则，返回-1
// 功能描述：
//	处理  WM_CREATE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
{	//
    _LPLISTDATA lpListData = _DoInitListData( hWnd, lpcs );
	if( lpListData )
	{
		return 0;
	}
	return -1;
}

// **************************************************
// 声明：static LRESULT DoCREATECombBoxList( HWND hWnd, LPCREATESTRUCT lpcs )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpcs - 窗口创建结构指针
// 返回值：
//	假如成功，返回0; 否则，返回-1
// 功能描述：
//	处理  WM_CREATE 消息
// 引用: 
//	
// ************************************************
static LRESULT DoCREATECombBoxList( HWND hWnd, LPCREATESTRUCT lpcs )
{
    _LPLISTDATA lpListData = _DoInitListData( hWnd, lpcs );
	if( lpListData )
	{
        lpListData->bCombBoxList=TRUE;
        lpListData->hCombBox = lpcs->lpCreateParams;
		return 0;
	}
	return -1;
}

// **************************************************
// 声明：static LRESULT DoDestroy( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_DESTROY 消息
// 引用: 
//	
// ************************************************

static LRESULT DoDestroy( HWND hWnd )
{
    _LPLISTDATA lpListData;
    HWND hParent;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//得到列表框窗口的列表框数据对象指针
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    hParent = GetParent( hWnd );
	//释放所有的项目
    while( lpListData->count )
    {
		DeleteItem( hWnd, dwStyle, lpListData, lpListData->count-1 );
    }
    free( lpListData->lpItems );
    free( lpListData );
    return 0;
}

// **************************************************
// 声明：static int AtRow( _LPLISTDATA lpListData, DWORD dwStyle, int y )
// 参数：
//	IN lpListData - 列表框对象指针 
//	IN dwStyle - 风格
//	IN y - y坐标
// 返回值：
//	返回y所在的行数
// 功能描述：
//	得到y所在的行数
// 引用: 
//	
// ************************************************

static int AtRow( _LPLISTDATA lpListData, DWORD dwStyle, int y )
{
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
	{	//可变行高
		int cy = 0;
		int indexStart; 
		_LPLISTITEM lpItem;
		
		if( lpListData->count )
		{	//有项目
			indexStart = lpListData->topIndex;
			//
			do{
				lpItem = At( lpListData, indexStart );
				cy += (int)lpItem->typeData;
				if( cy >= y )
					break;
				indexStart++;
			}while( indexStart < lpListData->count );
			if( indexStart < lpListData->count )
				return indexStart - lpListData->topIndex;
			else if( lpListData->count )
				return lpListData->count - lpListData->topIndex - 1;
		}
		return 0;
	}
	else
	{	//固定行高
		if( y >= 0 )
			return y / lpListData->rowHeight;
		else
			return -1 + y / lpListData->rowHeight;
	}
}

// **************************************************
// 声明：static int AtCol( _LPLISTDATA lpListData, DWORD dwStyle, int x )
// 参数：
//	IN lpListData - 列表框对象指针 
//	IN dwStyle - 窗口风格
//	IN x - x坐标
// 返回值：
//	返回列数
// 功能描述：
//	得到 x 所在的列数
// 引用: 
//	
// ************************************************

static int AtCol( _LPLISTDATA lpListData, DWORD dwStyle, int x )
{
    if( lpListData->colWidth > 0 )
	{
		if( x >= 0 )
			return x / lpListData->colWidth;
		else
			return -1 + x / lpListData->colWidth;
	}
	else
		return 0;
}

// **************************************************
// 声明：static int GetRows( _LPLISTDATA lpListData, DWORD dwStyle, LPCRECT lpClientRect )
// 参数：
//	IN lpListData - 列表框对象指针 
//	IN dwStyle - 窗口风格
//	IN lpClientRect - 客户矩形
// 返回值：
//	返回客户矩形所能容纳的行数
// 功能描述：
//	得到客户矩形所能容纳的行数
// 引用: 
//	
// ************************************************

static int GetRows( _LPLISTDATA lpListData, DWORD dwStyle, LPCRECT lpClientRect )
{
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
	{	//可变行高
		int cy = 0;
		int indexStart; 
		_LPLISTITEM lpItem;

		if( lpListData->count )
		{	//有项目
			int y = lpClientRect->bottom;
			
			indexStart = lpListData->topIndex;
			do{
				lpItem = At( lpListData, indexStart );
				cy += (int)lpItem->typeData;
				if( cy >= y )
					break;
				indexStart++;
			}while( indexStart < lpListData->count );
			if( indexStart < lpListData->count )		
				return indexStart - lpListData->topIndex + 1;
			else
				return indexStart - lpListData->topIndex;
		}
		else
			return 1;
	}
	else
	{	//不可变行数
		if( lpListData->rowHeight > 0 )
			return lpClientRect->bottom / lpListData->rowHeight;
		else
			return 1;
	}
}

// **************************************************
// 声明：static int GetCols( _LPLISTDATA lpListData, LPCRECT lpClientRect )
// 参数：
//	IN lpListData - 列表框对象指针 
//	IN lpClientRect - 客户矩形
// 返回值：
//	返回客户矩形所能容纳的列数
// 功能描述：
//	得到客户矩形所能容纳的列数
// 引用: 
//	
// ************************************************

static int GetCols( _LPLISTDATA lpListData, LPCRECT lpClientRect )
{
    if( lpListData->colWidth > 0 )
	    return lpClientRect->right / lpListData->colWidth;
	else
		return 1;
}

// **************************************************
// 声明：static short GetItemRect( _LPLISTDATA lpListData, int index, LPRECT lpRect, DWORD dwStyle )
// 参数：
//	IN lpListData - 列表框对象指针 
//	IN index - 项目索引
//	IN/OUT lpRect - 输入窗口矩形, 接受项目矩形
//	IN dwStyle - 窗口风格
// 返回值：
//	返回1
// 功能描述：
//	得到项目矩形
// 引用: 
//	
// ************************************************

static short GetItemRect( _LPLISTDATA lpListData, int index, LPRECT lpRect, DWORD dwStyle )
{
    int crows, row, col;

    if( dwStyle & LBS_MULTICOLUMN )
    {	//多列风格
        crows = GetRows( lpListData, dwStyle, lpRect );
        col = (index - lpListData->topIndex) / crows;
        row = (index - lpListData->topIndex) % crows;
        lpRect->left = col * lpListData->colWidth;
        lpRect->right = lpRect->left + lpListData->colWidth;
        lpRect->top = row * lpListData->rowHeight;
	    lpRect->bottom = lpRect->top + lpListData->rowHeight;
        return 1;
    }
    else
    {	//单列
		if( dwStyle & LBS_OWNERDRAWVARIABLE )
		{	//可变行高
	        int y = 0;
			int indexStart; 
			_LPLISTITEM lpItem;
			//从topIndex开始索引
			indexStart = lpListData->topIndex;
			if( index < lpListData->topIndex )
			{
				indexStart--;
				do{
					lpItem = At( lpListData, indexStart );
					y += (int)lpItem->typeData;
					indexStart--;
				}while( indexStart >= index );
			    lpRect->top = -y;
			    lpRect->bottom = -y + (int)lpItem->typeData;
			}
			else if( index > lpListData->topIndex )
			{
				do{
					lpItem = At( lpListData, indexStart );
					y += (int)lpItem->typeData;
					indexStart++;
				}while( indexStart <= index );
			    lpRect->top = y - lpItem->typeData;
			    lpRect->bottom = y;
			}
			else
			{
				lpItem = At( lpListData, indexStart );
			    lpRect->top = 0;
			    lpRect->bottom = lpItem->typeData;
			}			
		}
		else
		{	//固定行高
			lpRect->top = (index-lpListData->topIndex) * lpListData->rowHeight;
			lpRect->bottom = lpRect->top + lpListData->rowHeight;
		}
        return 1;
    }
}

// **************************************************
// 声明：static void DrawCaret( HDC hdc, LPCRECT lpRect )
// 参数：
//	IN hdc - 绘图DC
//	IN lpRect - 矩形坐标
// 返回值：
//	无
// 功能描述：
//	绘制焦点显示效果
// 引用: 
//	
// ************************************************

static void DrawCaret( HDC hdc, LPCRECT lpRect )
{
    DrawFocusRect( hdc, lpRect );
}

// **************************************************
// 声明：static void DrawItemContents ( HWND hWnd, 
//                              _LPLISTDATA lpListData,
//							  DWORD dwStyle, 
//							  HDC hdc, 
//							  int index, 
//							  LPCRECT lpRect, 
//							  BOOL bHasCaret,
//							  UINT uiAction )
//
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpListData - 列表框对象指针
//	IN dwStyle - 窗口风格
//	IN hdc - DC句柄
//	IN index - 项目索引
//	IN lpRect - 项目矩形
//	IN bHasCaret - 项目是否有 Caret 属性
//	IN uiAction - 当前的动作
// 返回值：
//	无
// 功能描述：
//	绘制项目
// 引用: 
//	
// ************************************************

static void DrawItemContents ( HWND hWnd, 
                              _LPLISTDATA lpListData,
							  DWORD dwStyle, 
							  HDC hdc, 
							  int index, 
							  LPCRECT lpRect, 
							  BOOL bHasCaret,
							  UINT uiAction )
{
	_LPLISTITEM lpItem = At( lpListData, index );
	if( lpItem  )
    {
        if( dwStyle & (LBS_OWNERDRAWVARIABLE | LBS_OWNERDRAWFIXED) )
		{	//自绘风格
			DRAWITEMSTRUCT dis;
			HWND hParent;
			
			dis.CtlID = GetWindowLong( hWnd, GWL_ID );
			dis.CtlType = ODT_LISTBOX;
			dis.itemData = lpItem->dwItemData;
			dis.itemID = index;
		
			hParent = GetParent(hWnd);			

			dis.hDC = hdc;
			dis.hwndItem = hWnd;
			dis.itemAction = uiAction;
			dis.itemState = lpItem->state;
			// 2005-09-22, add
			if( bHasCaret )
				dis.itemState |= ODS_FOCUS;
			//
			dis.rcItem =  *lpRect;
			//向父发绘制项目消息
			SendMessage( hParent, WM_DRAWITEM, dis.CtlID, (LPARAM)&dis );
		}
		else
		{	// string list
			TEXTMETRIC tm;
			int top;
			if( bHasCaret )
			{
				DrawCaret( hdc,lpRect );
			}
			//设置文本显示颜色
			if( lpItem->state & (ODS_DISABLED|ODS_GRAYED) )
			{
				SetTextColor( hdc, lpListData->cl_Disable );
				SetBkColor( hdc, lpListData->cl_DisableBk );
			}
			else if( lpItem->state & ODS_SELECTED )
			{   // selected item color
				SetTextColor( hdc, lpListData->cl_Selection );
				SetBkColor( hdc, lpListData->cl_SelectionBk );
			}
			else
			{
				SetBkColor( hdc, lpListData->cl_TextBk );//CL_WHITE );
				SetTextColor( hdc, lpListData->cl_Text );//CL_BLACK );
			}
			GetTextMetrics( hdc, &tm );
			top = lpRect->top + ( (lpRect->bottom - lpRect->top) - tm.tmHeight ) / 2; //垂直居中
			
			//绘制项目文本
			ExtTextOut( hdc,
				lpRect->left, top,
				ETO_CLIPPED | ETO_OPAQUE,
				lpRect,
				lpItem->lpszItem, lpItem->typeData,
				0 );
			if( bHasCaret )
			{
				DrawCaret( hdc, lpRect );
			}
		}
    }
    else
    {
		FillSolidRect( hdc, lpRect, lpListData->cl_TextBk );
    }
}

// **************************************************
// 声明：static void SendNotify( HWND hWnd, const short code )
// 参数：
//	IN hWnd - 窗口句柄
//	IN code - 通知代码
// 返回值：
//	无
// 功能描述：
//	向窗口发送通知消息
// 引用: 
//	
// ************************************************

static void SendNotify( HWND hWnd, const short code )
{
    DWORD wParam;
  	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//得到列表框窗口的列表框数据对象指针

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_NOTIFY )
    {	//
        wParam = GetWindowLong( hWnd, GWL_ID );
        wParam = MAKELONG( wParam, code );
        if (lpListData->bCombBoxList==TRUE)               
           SendMessage( lpListData->hCombBox, WM_COMMAND, wParam, (LPARAM)hWnd );
        else
           SendMessage( GetParent( hWnd ), WM_COMMAND, wParam, (LPARAM)hWnd );
    }
}

// **************************************************
// 声明：static BOOL SetScrollBar( HWND hWnd, int pos, int *lpNewPos )
// 参数：
//	IN hWnd - 窗口句柄  
//	IN pos - 滚动棒位置 
//	IN lpNewPos - 用于接受新的位置
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	设置滚动棒位置
// 引用: 
//	
// ************************************************

static BOOL SetScrollBar( HWND hWnd, int pos, int *lpNewPos )
{	//得到列表框窗口的列表框数据对象指针
	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    SCROLLINFO si;
    RECT rect;
	DWORD dwStyle;
    short rows,t, cols;

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	GetClientRect( hWnd, &rect );
    rows = GetRows( lpListData, dwStyle, &rect  );
    if( lpListData->dwStyle & WS_VSCROLL )
    {   //有垂直滚动棒
        si.cbSize = sizeof( si );
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        si.nMin = 0;
        t = lpListData->count-1;
        si.nMax = max( t, 0 );
        si.nPage = rows;
        if( pos < 0 )
            pos = 0;
        if( pos >= lpListData->count - rows )
            pos = si.nMax;
        si.nPos = pos;
		// 2005-01-26 < to <=
        if( si.nMin <= si.nMax )
        {
            SetScrollInfo( hWnd, SB_VERT, &si, TRUE );
            if( lpNewPos )
                *lpNewPos = pos;
            return TRUE;
        }
    }
    
	else if( lpListData->dwStyle & WS_HSCROLL )
    {	//有水平滚动棒
        cols = GetCols( lpListData, &rect );
        si.cbSize = sizeof( si );
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        si.nMin = 0;
        si.nMax = lpListData->count / (cols * rows);
        si.nPage = cols;
        if( pos < 0 )
            pos = 0;
        if( pos >= si.nMax )
            pos = si.nMax;
        si.nPos = pos;
			// 2005-01-26 < to <=
        if( si.nMin <= si.nMax )
        {
            SetScrollInfo( hWnd, SB_HORZ, &si, TRUE );
            if( lpNewPos )
                *lpNewPos = pos;
            return TRUE;
        }
    }
    return FALSE;
}

// **************************************************
// 声明：static int ScrollCol( HWND hWnd, int code  )
// 参数：
//	IN hWnd - 窗口句柄
//	IN code - 列数
// 返回值：
//	返回0
// 功能描述：
//	滚动列
// 引用: 
//	
// ************************************************

static int ScrollCol( HWND hWnd, int code  )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	RECT rect;
	int iRowNum;
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//得到窗口矩形
	GetClientRect( hWnd, &rect );
	//行数
	iRowNum = GetRows( lpListData, dwStyle, &rect );
    if( SetScrollBar( hWnd, code + lpListData->topIndex / iRowNum, &code ) )
    {
        code *= iRowNum;
        if( code != lpListData->topIndex )
            DoSETTOPINDEX( hWnd, code );
    }
    return 0;
}

// **************************************************
// 声明：static int ScrollRow( HWND hWnd, int code  )
// 参数：
//	IN hWnd - 窗口句柄
//	IN code - 行数
// 返回值：
//	返回0
// 功能描述：
//	滚动行数
// 引用: 
//	
// ************************************************

static int ScrollRow( HWND hWnd, int code  )
{	//得到列表框窗口的列表框数据对象指针
	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	RECT rect;
	int iRowNum;
	DWORD dwStyle;
	//得到窗口风格
    dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//得到客户区矩形
	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpListData, dwStyle, &rect );
    code = code + lpListData->topIndex;
    if( SetScrollBar( hWnd, code, &code ) )
    {	
        if( code != lpListData->topIndex )
        {
            if( code >= lpListData->count - iRowNum )
                code = lpListData->count - iRowNum;
            if( code != lpListData->topIndex )
                DoSETTOPINDEX( hWnd, code );
        }
    }
    return 0;
}

// **************************************************
// 声明：static void RedrawItem( _LPLISTDATA lpListData, 
//					   HWND hWnd, 
//					   DWORD dwStyle, 
//					   HDC hdc, 
//					   int iFrom, 
//					   int iTo,
//					   BOOL bEraseBottom
//					    )
// 参数：
//	IN lpListData - 列表框对象指针 
//	IN hWnd - 窗口句柄 
//	IN dwStyle - 窗口风格
//	IN hdc - 图形DC
//	IN iFrom - 开始项目
//	IN iTo - 结束项目
//	IN bEraseBottom - 是否清除窗口底部区域
// 返回值：
//	无
// 功能描述：
//	绘制项目
// 引用: 
//	
// ************************************************

static void RedrawItem( _LPLISTDATA lpListData, 
					   HWND hWnd, 
					   DWORD dwStyle, 
					   HDC hdc, 
					   int iFrom, 
					   int iTo,
					   BOOL bEraseBottom
					    )
{
	int i, yBottom, nEmptyItem = 0;
	RECT rcClip, rect, rcClient;
	HFONT hOldFont = NULL;
	//得到客户区
	GetClientRect( hWnd, &rcClient );
	if( iFrom < lpListData->topIndex )
		iFrom = lpListData->topIndex;
	yBottom = rcClient.top;
	if( lpListData->hFont )
	{
		hOldFont = SelectObject( hdc, lpListData->hFont );
	}
	//绘制每一个项目
	for( i = iFrom; i < iTo; i++ )
    {
		rect = rcClient;
		//得到项目显示区域
		GetItemRect( lpListData, i, &rect, dwStyle );
		yBottom = rect.bottom;
		//得到裁剪区
        if( IntersectRect( &rcClip, &rect, &rcClient ) )
		    DrawItemContents( hWnd, lpListData, dwStyle, hdc, i, &rect, lpListData->caret == i, ODA_DRAWENTIRE );
        else
		{
            bEraseBottom = FALSE;
			break;
		}
    }
	if( bEraseBottom )
	{	//清除余下背景
		rect = rcClient;
		rect.top = yBottom;
		if( rect.top < rect.bottom )
		{
			FillSolidRect( hdc, &rect, lpListData->cl_TextBk );
		}		
	}
	if( lpListData->hFont )
	{
		SelectObject( hdc, hOldFont );
	}
}

// **************************************************
// 声明：static LRESULT DoPAINT( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	绘制客户区
//	处理 WM_PAINT 消息
// 引用: 
//	
// ************************************************

static LRESULT DoPAINT( HWND hWnd )
{
	_LPLISTDATA lpData;
	HDC hdc;
	PAINTSTRUCT ps;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//得到列表框窗口的列表框数据对象指针
	lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	//得到绘图DC
	hdc = GetListDC( hWnd, &ps, lpData );//BeginPaint( hWnd, &ps );
	
	RedrawItem( lpData, hWnd, dwStyle, hdc, lpData->topIndex, lpData->count, TRUE );
    //释放DC
	//EndPaint( hWnd, &ps );
	ReleaseListDC( hWnd, hdc, &ps );
    return 0;
}

// **************************************************
// 声明：static LRESULT DoLBUTTONDOWN( HWND hWnd, short x, short y, BOOL bNotify )
// 参数：
//	IN hWnd - 窗口句柄
//	IN x - 鼠标x坐标
//	IN y - 鼠标y坐标
//	IN bNotify - 是否发通知消息给父窗口
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_LBUTTONDOWN 消息
// 引用: 
//	
// ************************************************

static LRESULT DoLBUTTONDOWN( HWND hWnd, short x, short y, BOOL bNotify )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	_LPLISTDATA lpData;
    short h, col, row, i, iRowNum;
	RECT rect;

    if( GetFocus() != hWnd )
        SetFocus( hWnd );
	//得到列表框窗口的列表框数据对象指针
    lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	_ASSERT( lpData );
	//得到窗口客户区
	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpData, dwStyle, &rect );

    col = AtCol( lpData, dwStyle, x );
    row = AtRow( lpData, dwStyle, y );
    i = col * iRowNum + row;
    h = lpData->topIndex + i;
    if( h >= 0 )
    {
		// 假如可能，发送通知消息 send notify message
		if(  dwStyle & LBS_NOTIFY )
		{	
			LBNOTIFY lbNotify, * plbNotify;
			DWORD dwParam;

			plbNotify = &lbNotify;
#ifdef _MAPPOINTER
		    plbNotify = (LBNOTIFY*)MapPtrToProcess( plbNotify, GetCurrentProcess() );
#endif
			//初始化通知结构数据
			plbNotify->hwndFrom = hWnd;
			if( h < lpData->count )
			{
				plbNotify->iItem = h;
				plbNotify->uiState = (lpData->lpItems+h)->state;
				plbNotify->dwItemData = (lpData->lpItems+h)->dwItemData;
			}
			else
			{
				plbNotify->iItem = -1;
			}

			dwParam = GetWindowLong( hWnd, GWL_ID );
			dwParam = MAKELONG( dwParam, LBN_CLICK );
			//
			SendMessage( GetParent(hWnd), WM_COMMAND, dwParam, (LPARAM)plbNotify );
		}

        if( dwStyle & LBS_MULTIPLESEL )
            DoSETSEL( hWnd, !DoGETSEL( hWnd, h ), h );
        else if( h != lpData->curSel )
		{	//改变当前选择项 并发通知消息
            if( bNotify )
		        SendNotify( hWnd, LBN_SELCHANGE );
            DoSETCURSEL( hWnd, h, bNotify );
            DoSETCARETINDEX( hWnd, h, TRUE );
		}
    }
	//抓住鼠标事件/消息
    //SetCapture( hWnd ); //2004-12-21,ln, 因为有可能在 发LB_CLICK 后进入其它画面，移到 MOUSEMOVE
	lpData->bDown = 1;
    return 0;
}

// **************************************************
// 声明：static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y, BOOL bNotify )
// 参数：
//	IN hWnd - 窗口句柄
//	IN x - 鼠标x坐标
//	IN y - 鼠标y坐标
//	IN bNotify - 是否发通知消息给父窗口
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_LBUTTONDOWN 消息
// 引用: 
//	
// ************************************************

static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y, BOOL bNotify )
{
	_LPLISTDATA lpData;

	lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    //if( GetCapture() == hWnd )
	if( lpData->bDown == 1 )
    {   //  ComboBox 需要在这里加 Notify.
        if( bNotify )
		    SendNotify( hWnd, LBN_SELCHANGE );
        //ReleaseCapture();
		lpData->bDown = 0;
    }
	
    return 0;
}

// **************************************************
// 声明：static LRESULT DoMOUSEMOVE( HWND hWnd, WPARAM wParam, short x, short y, BOOL bNotify )
// 参数：
//	IN hWnd - 窗口句柄
//  IN wParam - WM_MOUSEMOVE 消息的 wParam 消息
//	IN x - 鼠标x坐标
//	IN y - 鼠标y坐标
//	IN bNotify - 是否发通知消息给父窗口
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_MOUSEMOVE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoMOUSEMOVE( HWND hWnd, WPARAM wParam, short x, short y, BOOL bNotify )
{
    int pos, iRowNum;
    _LPLISTDATA lpData;
	RECT rect;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );	//得到窗口风格

	//得到列表框窗口的列表框数据对象指针
    lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	//得到窗口客户区坐标
	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpData, dwStyle, &rect );
	//是否鼠标移动时按下了左按钮
    if( (wParam & MK_LBUTTON) )
    {	//鼠标移动时按下了左按钮
		//得到当前鼠标所在的项目
        pos = (short)DoGETTOPINDEX( hWnd ) + AtRow( lpData, dwStyle, y );
        if( (dwStyle & LBS_MULTICOLUMN) )
        {	//列表框为多列风格
            pos += AtCol( lpData, dwStyle, x ) * iRowNum;
        }
        if( pos >= 0 )                                   // modify 2000-05-18 by ln
        {	//
			if( dwStyle & LBS_MULTIPLESEL )
				return DoSETCARETINDEX( hWnd, pos, TRUE );
			else
			{	//单项选择风格
				DoSETCURSEL( hWnd, pos, bNotify );
				return DoSETCARETINDEX( hWnd, pos, TRUE );
			}
        }
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoLBUTTONDBLCLK( HWND hWnd )
// 参数：
//	IN hWnd - 窗口风格
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_LBUTTONDBLCLK 消息
// 引用: 
//	
// ************************************************

static LRESULT DoLBUTTONDBLCLK( HWND hWnd )
{
    WPARAM wParam;

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_NOTIFY )
    {	//需要发送通知消息
        wParam = GetWindowLong( hWnd, GWL_ID );
        wParam = MAKELONG( wParam, LBN_DBLCLK );
        PostMessage( GetParent( hWnd ), WM_COMMAND, wParam, (LPARAM)hWnd );
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoCHAR( HWND hWnd, WPARAM wParam, LPARAM lParam )
// 参数：
//	IN hWnd - 窗口句柄
//	IN wParam - WM_CHAR 消息的wParam参数
//	IN lParam - WM_CHAR 消息的lParam参数
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_CHAR 消息
// 引用: 
//	
// ************************************************

static LRESULT DoCHAR( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lrs = -1;
	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    // support late
    //if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_WANTKEYBOARDINPUT )
    //{
        //lrs = SendMessage( hWnd, WM_VKEYTOITEM, MAKELONG( wParam, lpListData->caret ), (LPARAM)hWnd );
    //}
    if( lrs == -1 )
    {    // do default action
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoKEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
// 参数：
//	IN hWnd - 窗口句柄
//	IN wParam - WM_KEYDOWN 消息的wParam参数
//	IN lParam - WM_KEYDOWN 消息的lParam参数
// 返回值：
//	返回0
// 功能描述：
//	处理 WM_KEYDOWN 消息
// 引用: 
//	
// ************************************************

static LRESULT DoKEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lrs = -1;
	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );
    // update late
    //if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_WANTKEYBOARDINPUT )
    //{
        //lrs = SendMessage( hWnd, WM_VKEYTOITEM, MAKELONG( wParam, lpListData->caret ), (LPARAM)hWnd );
    //}
    if( lrs == -1 )  // do default action
    {
        short rows = 0;
        short cols = 0;

        switch( wParam )
        {
        case VK_UP:
            rows = -1;
            break;
        case VK_DOWN:
            rows = 1;
            break;
//        case VK_PGUP:
//            rows = -((short)lpListData->rowNum);
//            break;
//        case VK_PGDN
//            rows = (short)lpListData->rowNum;
//            break;
        case VK_LEFT:
            cols = -1;
            break;
        case VK_RIGHT:
            cols = 1;
            break;
		case VK_RETURN:
			SendMessage( GetParent(hWnd), WM_COMMAND, (WPARAM)1, 0 );
			break;
        }
        if( rows )
        {	//同列的行项目改变，设置新的选择项
			if(  lpListData->curSel + rows < 0 )
				DoSETCURSEL( hWnd, 0, TRUE );
			else
                DoSETCURSEL( hWnd, lpListData->curSel + rows, TRUE );
            DoSETCARETINDEX( hWnd, lpListData->curSel, TRUE );
        }
        else if( cols )
            ScrollCol( hWnd, cols );//同行的列项目改变，设置新的选择项
    }
    else if( lrs >= 0 )  // lrs is index of item, do default action at index of lrs
    {
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoSetRedraw( HWND hWnd, BOOL bRedraw )
// 参数：
//	IN hWnd - 窗口句柄
//	IN bRedraw - 是否设置重绘标志
// 返回值：
//	返回0
// 功能描述：
//	当项目改变（新增、删除等）是否从绘改变
//	处理 WM_SETREDRAW 消息
// 引用: 
//	
// ************************************************

static LRESULT DoSetRedraw( HWND hWnd, BOOL bRedraw )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    //	得到窗口风格
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//
    if( bRedraw )
        dwStyle &= ~LBS_NOREDRAW;
    else
        dwStyle |= LBS_NOREDRAW;
	//2004-03-06 add
	SetWindowLong( hWnd, GWL_STYLE, dwStyle );
    return 0;
}

// **************************************************
// 声明：static LRESULT DoHSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
// 参数：
//	IN hWnd - 窗口风格
//	IN code - 滚动方法
//	IN pos - 滚动数
//	IN hwndCtrl - 为NULL
// 返回值：
//	返回0
// 功能描述：
//	水平滚动
//	处理 WM_HSCROLL 消息
// 引用: 
//	
// ************************************************

static LRESULT DoHSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    RECT rect;
    int cols, i;
	int iRowNum;
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//得到窗口风格
	GetClientRect( hWnd, &rect );
	//得到客户区能显示的行数
	iRowNum = GetRows( lpListData, dwStyle, &rect );
	//得到客户区能显示的列数
    cols = lpListData->count / iRowNum;
    i = lpListData->topIndex / iRowNum;

    switch( code )
    {
    case SB_BOTTOM:		//到底
        ScrollCol( hWnd, cols - i );
        break;
    case SB_ENDSCROLL:	//
        break;
    case SB_LINEDOWN:	//下一列
        ScrollCol( hWnd, 1 );
        break;
    case SB_LINEUP:		//上一列
        ScrollCol( hWnd, -1 );
        break;
    case SB_PAGEDOWN:	//下一页
        ScrollCol( hWnd, lpListData->colNum );
        break;
    case SB_PAGEUP:		//上一页
        ScrollCol( hWnd, (short)-lpListData->colNum );
        break;
    case SB_THUMBPOSITION:		//
        break;
    case SB_THUMBTRACK:
        ScrollCol( hWnd, pos - i );
        break;
    case SB_TOP:		//到顶
        ScrollCol( hWnd, -i );
        break;
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoHSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
// 参数：
//	IN hWnd - 窗口风格
//	IN code - 滚动方法
//	IN pos - 滚动数
//	IN hwndCtrl - 为NULL
// 返回值：
//	返回0
// 功能描述：
//	垂直滚动
//	处理 WM_VSCROLL 消息
// 引用: 
//	
// ************************************************

static LRESULT DoVSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    RECT rect;
    int rows, i;
	//得到窗口风格
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//得到客户区矩形
    GetClientRect( hWnd, &rect );
	//得到客户区能显示的行数
	rows = GetRows( lpListData, dwStyle, &rect );

    switch( code )
    {
    case SB_BOTTOM:			//到底
        ScrollRow( hWnd, lpListData->count );
        break;
    case SB_ENDSCROLL:
        break;
    case SB_LINEDOWN:		//下一行
        ScrollRow( hWnd, 1 );
        break;
    case SB_LINEUP:			//上一行
        ScrollRow( hWnd, -1 );
        break;
    case SB_PAGEDOWN:		//下一页
        ScrollRow( hWnd, rows );
        break;	
    case SB_PAGEUP:			//上一页
        ScrollRow( hWnd, (short)-rows );
        break;
    case SB_THUMBPOSITION:
//        i = DoGETTOPINDEX( hWnd );
//        if( i != pos )
//        {
            //DoSETTOPINDEX( hWnd, pos );
            //SetScrollBar( hWnd, pos, 0, 0 );
        //}
        break;
    case SB_THUMBTRACK:
        i = (short)DoGETTOPINDEX( hWnd );
        if( i != pos )
        {
            ScrollRow( hWnd, pos - i );
//            DoSETTOPINDEX( hWnd, pos );
//            SetScrollBar( hWnd, pos, 0 );
        }
        break;
    case SB_TOP:			//到顶
        i = (short)DoGETTOPINDEX( hWnd );
        ScrollRow( hWnd, -i );
        break;
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoNCCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
// 参数：
//	IN hWnd - 窗口句炳
//	IN lpcs - 创建结构
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	处理 WM_NCCREATE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoNCCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
{
    if( ( lpcs->style & LBS_MULTICOLUMN ) &&
        ( lpcs->style & WS_VSCROLL ) )
    {	//不能有该风格
        lpcs->style &= ~WS_VSCROLL;
    }
    else if( ( lpcs->style & LBS_MULTICOLUMN ) == 0 &&
             ( lpcs->style & WS_HSCROLL ) )
    {	//不能有该风格
        lpcs->style &= ~WS_HSCROLL;
    }
    if( (lpcs->style & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE)) == 0 )
        lpcs->style |= LBS_HASSTRINGS;
	if( (lpcs->style & LBS_OWNERDRAWVARIABLE) &&
		(lpcs->style & LBS_MULTICOLUMN) )
	{
		lpcs->style &= ~LBS_OWNERDRAWVARIABLE;
	}
    SetWindowLong( hWnd, GWL_STYLE, lpcs->style );
    return DefWindowProc( hWnd, WM_NCCREATE, 0, (LPARAM)lpcs );
}

// **************************************************
// 声明：static LRESULT WINAPI DoKILLFOCUS( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	当失去焦点时处理该过程
//	处理 WM_KILLFOCUS 消息
// 引用: 
//	
// ************************************************

static LRESULT WINAPI DoKILLFOCUS( HWND hWnd )
{	//得到列表框窗口的列表框数据对象指针
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    HDC hdc;
    RECT rect;
	DWORD dwStyle;

    if( lpListData->caret != - 1 )
    {	//得到绘图DC
        hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
		//得到窗口客户区矩形
        GetClientRect( hWnd, &rect );
		//得到项目矩形
        GetItemRect( lpListData, lpListData->caret, &rect, GetWindowLong( hWnd, GWL_STYLE ) );
		//得到窗口风格
		dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		//画项目
        DrawItemContents ( hWnd, lpListData, dwStyle, hdc, lpListData->caret, &rect, FALSE, ODA_FOCUS );
        lpListData->caret = -1;
		//释放DC
        ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
    }
	//通知父窗口
    SendNotify( hWnd, LBN_KILLFOCUS );
    
    return 0;
}


// **************************************************
// 声明：static LRESULT WINAPI DoSETFOCUS( HWND hWnd )
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	返回0
// 功能描述：
//	当失去焦点时处理该过程
//	处理 WM_KILLFOCUS 消息
// 引用: 
//	
// ************************************************

static LRESULT WINAPI DoSETFOCUS( HWND hWnd )
{	//得到列表框窗口的列表框数据对象指针
	/*
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    HDC hdc;
    RECT rect;
	DWORD dwStyle;

	
	if( lpListData->caret != - 1 )
		return 0;  // nothing to do	
	//

	lpListData->caret = lpListData->curSel;  //// 2005-09-22, add code by lilin
    if( lpListData->caret != -1 )
    {	//得到绘图DC
		
        hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
		//得到窗口客户区矩形
        GetClientRect( hWnd, &rect );
		//得到项目矩形
        GetItemRect( lpListData, lpListData->caret, &rect, GetWindowLong( hWnd, GWL_STYLE ) );
		//得到窗口风格
		dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		//画项目
        DrawItemContents ( hWnd, lpListData, dwStyle, hdc, lpListData->caret, &rect, TRUE, ODA_FOCUS );
        //lpListData->caret = -1;
		//释放DC
        ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
    }
	*/
	//通知父窗口
    SendNotify( hWnd, LBN_SETFOCUS );
    
    return 0;
}

// **************************************************
// 声明：static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpccs - 控件结构，包含颜色值
// 返回值：
//	成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	设置控件颜色,处理 WM_SETCTLCOLOR 消息
// 引用: 
//	
// ************************************************


static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{	//得到列表框窗口的列表框数据对象指针
	    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
		if( lpccs->fMask & CLF_TEXTCOLOR )
		{
			lpListData->cl_Text = lpccs->cl_Text;	//通常文本颜色
		}
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		{
			lpListData->cl_TextBk = lpccs->cl_TextBk;	//通常文本背景颜色
		}
		if( lpccs->fMask & CLF_SELECTIONCOLOR )
		{
			lpListData->cl_Selection = lpccs->cl_Selection;	//选择项文本颜色
		}
		if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
		{
			lpListData->cl_SelectionBk = lpccs->cl_SelectionBk;	//选择项背景颜色
		}
		if( lpccs->fMask & CLF_DISABLECOLOR )
		{
			lpListData->cl_Disable = lpccs->cl_Disable;	//无效文本颜色
		}
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
		{
			lpListData->cl_DisableBk = lpccs->cl_DisableBk;	//无效背景颜色
		}

		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// 参数：
//	IN hWnd - 窗口句柄
//	IN lpccs - 控件结构，包含颜色值
// 返回值：
//	成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	得到控件颜色，处理 WM_GETCTLCOLOR 消息
// 引用: 
//	
// ************************************************

static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{	//得到列表框窗口的列表框数据对象指针
		_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
		if( lpccs->fMask & CLF_TEXTCOLOR )
		{
			lpccs->cl_Text = lpListData->cl_Text;	//通常文本颜色
		}
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		{
			lpccs->cl_TextBk = lpListData->cl_TextBk;//通常文本背景颜色
		}
		if( lpccs->fMask & CLF_SELECTIONCOLOR )
		{
			lpccs->cl_Selection = lpListData->cl_Selection;//选择项文本颜色
		}
		if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
		{
			lpccs->cl_SelectionBk = lpListData->cl_SelectionBk;//选择项背景颜色
		}
		if( lpccs->fMask & CLF_DISABLECOLOR )
		{
			lpccs->cl_Disable = lpListData->cl_Disable;	//无效文本颜色
		}
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
		{
			lpccs->cl_DisableBk = lpListData->cl_DisableBk;//无效背景颜色
		}
		
		return TRUE;
	}
	return FALSE;	
}

// **************************************************
// 声明：static LRESULT DoSysColorChange( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	返回 0
// 功能描述：
//	处理系统颜色改变
// 引用: 
// ************************************************

static LRESULT DoSysColorChange( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

	_SetListColor( lpListData );
	return 0;
}

// **************************************************
// 声明：static LRESULT DoFontChange( HWND hWnd, WPARAM wParam, LPARAM lParam )
// 参数：
// 	IN hWnd - 窗口句柄
// 	IN wParam - WPARAM 参数(字体句柄)
// 	IN lParam - LPARAM 参数
// 返回值：
//	返回 0
// 功能描述：
//	处理字体改变
// 引用: 
// ************************************************

static LRESULT DoFontChange( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	DWORD dwStyle;

	lpListData->hFont = (HANDLE)wParam;
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	if(!( dwStyle & LBS_OWNERDRAWFIXED ))
	{
		HFONT hOldFont = NULL;
	    TEXTMETRIC tm;
		HDC hdc;

			hdc = GetDC( hWnd );
			if( lpListData->hFont )
			{
				hOldFont = SelectObject( hdc, lpListData->hFont );
			}
			GetTextMetrics( hdc, &tm );
			if( lpListData->hFont )
			{
				SelectObject( hdc, hOldFont );
			}
			ReleaseDC( hWnd, hdc );
			lpListData->rowHeight = (short)tm.tmHeight + 4;
	}
	return 0;
}


// **************************************************
// 声明：static LRESULT WINAPI ListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// 参数：
//  IN hWnd - 窗口句柄
//	IN message - 消息
//	IN wParam - 第一个消息
//	IN lParam - 第二个消息
// 返回值：
//	依赖于具体的消息
// 功能描述：
//	按钮窗口处理总入口
// 引用: 
//	
// ************************************************

static LRESULT WINAPI ListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch( message )
	{
	case WM_PAINT:
		return DoPAINT( hWnd );
    case WM_LBUTTONDOWN:
        return DoLBUTTONDOWN( hWnd, LOWORD( lParam ), HIWORD( lParam ), TRUE );
    case WM_LBUTTONUP:
        return DoLBUTTONUP( hWnd, LOWORD( lParam ), HIWORD( lParam ), FALSE );
    case WM_MOUSEMOVE:
        return DoMOUSEMOVE( hWnd, wParam, LOWORD(lParam), HIWORD(lParam), TRUE );
    case WM_LBUTTONDBLCLK:
        return DoLBUTTONDBLCLK( hWnd );
    case WM_VSCROLL:
        return DoVSCROLL( hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam );
    case WM_HSCROLL:
        return DoHSCROLL( hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam );
    case  WM_KILLFOCUS:
        return DoKILLFOCUS( hWnd );
    case WM_SETFOCUS:
        //SendNotify( hWnd, LBN_SETFOCUS );
		return DoSETFOCUS( hWnd );  // 2005-09-22 , add
        //break;
    case WM_KEYDOWN:
        return DoKEYDOWN( hWnd, wParam, lParam );
    case WM_CHAR:
        return DoCHAR( hWnd, wParam, lParam );
    case LB_ADDFILE:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoADDFILE( hWnd, (LPCSTR)lParam );
    case LB_ADDSTRING:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
		return DoADDSTRING( hWnd, (LPCSTR)lParam );
    case LB_DELETESTRING:
        return DoDELETESTRING( hWnd, (short)wParam );
    case LB_DIR:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoDIR( hWnd, (WORD)wParam, (LPCSTR)lParam );
    case LB_FINDSTRING:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoFINDSTRING( hWnd, (short)wParam, (LPCSTR)lParam, FALSE );
    case LB_FINDSTRINGEXACT:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoFINDSTRING( hWnd, (short)wParam, (LPCSTR)lParam, TRUE );  // exact find 
    case LB_GETANCHORINDEX:
        return DoGETANCHORINDEX( hWnd );
    case LB_GETCARETINDEX:
        return DoGETCARETINDEX( hWnd );
    case LB_GETCOUNT:
        return DoGETCOUNT( hWnd );
    case LB_GETCURSEL:
        return DoGETCURSEL( hWnd );
    case LB_GETHORIZONTALEXTENT:
        return DoGETHORIZONTALEXTENT( hWnd );
    case LB_GETITEMDATA:
        return DoGETITEMDATA( hWnd, (short)wParam );
    case LB_GETITEMHEIGHT:
        return DoGETITEMHEIGHT( hWnd, (short)wParam );
    case LB_GETITEMRECT:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoGETITEMRECT( hWnd, (short)wParam, (LPRECT)lParam );
    case LB_GETLOCALE:
        return DoGETLOCALE( hWnd );
    case LB_GETSEL:
        return DoGETSEL( hWnd, (short)wParam );
    case LB_GETSELCOUNT:
        return DoGETSELCOUNT( hWnd );
    case LB_GETSELITEMS:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoGETSELITEMS( hWnd, (short)wParam, (LPINT)lParam );
    case LB_GETTEXT:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoGETTEXT( hWnd, (short)wParam, (LPSTR)lParam );
    case LB_GETTEXTLEN:
        return DoGETTEXTLEN( hWnd, (short)wParam );
    case LB_GETTOPINDEX:
        return DoGETTOPINDEX( hWnd );
    case LB_INSERTSTRING:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoINSERTSTRING( hWnd, (short)wParam, (LPCSTR)lParam );
    case LB_RESETCONTENT:
        return DoRESETCONTENT( hWnd );
    case LB_SELECTSTRING:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoSELECTSTRING( hWnd, (short)wParam, (LPCSTR)lParam );
    case LB_SELITEMRANGE:
        return DoSELITEMRANGE( hWnd, (BOOL)wParam, LOWORD( lParam ), HIWORD( lParam ) );
    case LB_SELITEMRANGEEX:
        return DoSELITEMRANGEEX( hWnd, (short)wParam, (short)lParam );
    case LB_SETANCHORINDEX:
        return DoSETANCHORINDEX( hWnd, (short)wParam );
    case LB_SETCARETINDEX:
        return DoSETCARETINDEX( hWnd, (short)wParam, (BOOL)lParam );
    case LB_SETCOLUMNWIDTH:
        return DoSETCOLUMNWIDTH( hWnd, (short)wParam );
    case LB_SETCOUNT:
        return DoSETCOUNT( hWnd, (short)wParam );
    case LB_SETCURSEL:
        return DoSETCURSEL( hWnd, (short)wParam, FALSE );
    case LB_SETHORIZONTALEXTENT:
        return DoSETHORIZONTALEXTENT( hWnd, (short)wParam );
    case LB_SETITEMDATA:
        return DoSETITEMDATA( hWnd, (short)wParam, (DWORD)lParam );
    case LB_SETITEMHEIGHT:
        return DoSETITEMHEIGHT( hWnd, (short)wParam, (short)lParam );
    case LB_SETLOCALE:
        return DoSETLOCALE( hWnd, (WORD)wParam );
    case LB_SETSEL:
        return DoSETSEL( hWnd, (BOOL)wParam, (short)lParam );
    case LB_SETTABSTOPS:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoSETTABSTOPS( hWnd, (short)wParam, (short*)lParam  );
    case LB_SETTOPINDEX:
        return DoSETTOPINDEX( hWnd, (short)wParam );
    case WM_SETREDRAW:
        return DoSetRedraw( hWnd, (BOOL)wParam );
    case WM_SETCTLCOLOR:
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:
		return DoGetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hWnd );
	case WM_SETFONT:
		return DoFontChange( hWnd, wParam, lParam );
    case WM_NCCREATE:
        return DoNCCREATE( hWnd, (LPCREATESTRUCT)lParam );
    case WM_CREATE:
		return DoCREATE( hWnd, (LPCREATESTRUCT)lParam );
    case WM_DESTROY:
        return DoDestroy( hWnd );
	//2005-09-20, add for WS_GROUP by lilin
	case DLGC_WANTALLKEYS:
		return DLGC_WANTALLKEYS;
	//

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT WINAPI CombBoxListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// 参数：
//  IN hWnd - 窗口句柄
//	IN message - 消息
//	IN wParam - 第一个消息
//	IN lParam - 第二个消息
// 返回值：
//	依赖于具体的消息
// 功能描述：
//	按钮窗口处理总入口
// 引用: 
//	
// ************************************************

static LRESULT WINAPI CombBoxListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch( message )
	{
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_LBUTTONDOWN:
        return DoLBUTTONDOWN( hWnd, LOWORD( lParam ), HIWORD( lParam ), FALSE );
    case WM_LBUTTONUP:
        return DoLBUTTONUP( hWnd, LOWORD( lParam ), HIWORD( lParam ), TRUE );
    case WM_MOUSEMOVE:
        return DoMOUSEMOVE( hWnd, wParam, LOWORD(lParam), HIWORD(lParam), FALSE );
    case WM_ACTIVATEAPP:
        return 1;
    case WM_CREATE:
		return DoCREATECombBoxList( hWnd, (LPCREATESTRUCT)lParam );
	default:
		return ListBoxWndProc( hWnd, message, wParam, lParam );
	}
	return 0;
}


// **************************************************
// 声明：static LRESULT WINAPI CombBoxListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// 参数：
//  IN hWnd - 窗口句柄
//	IN message - 消息
//	IN wParam - 第一个消息
//	IN lParam - 第二个消息
// 返回值：
//	依赖于具体的消息
// 功能描述：
//	按钮窗口处理总入口
// 引用: 
//	
// ************************************************
/*
static LRESULT WINAPI CombBoxListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch( message )
	{
	case WM_PAINT:
		return DoPAINT( hWnd );
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_LBUTTONDOWN:
        return DoLBUTTONDOWN( hWnd, LOWORD( lParam ), HIWORD( lParam ), FALSE );
    case WM_LBUTTONUP:
        return DoLBUTTONUP( hWnd, LOWORD( lParam ), HIWORD( lParam ), TRUE );
    case WM_MOUSEMOVE:
        return DoMOUSEMOVE( hWnd, wParam, LOWORD(lParam), HIWORD(lParam), FALSE );
    case WM_LBUTTONDBLCLK:
        return DoLBUTTONDBLCLK( hWnd );
    case WM_VSCROLL:
        return DoVSCROLL( hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam );
    case WM_HSCROLL:
        return DoHSCROLL( hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam );
    case  WM_KILLFOCUS:
        return DoKILLFOCUS( hWnd );
    case WM_SETFOCUS:
        SendNotify( hWnd, LBN_SETFOCUS );
        break;
    case WM_KEYDOWN:
        return DoKEYDOWN( hWnd, wParam, lParam );
    case WM_CHAR:
        return DoCHAR( hWnd, wParam, lParam );
    case LB_ADDFILE:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoADDFILE( hWnd, (LPCSTR)lParam );
    case LB_ADDSTRING:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
		return DoADDSTRING( hWnd, (LPCSTR)lParam );
    case LB_DELETESTRING:
        return DoDELETESTRING( hWnd, (short)wParam );
    case LB_DIR:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoDIR( hWnd, (WORD)wParam, (LPCSTR)lParam );
    case LB_FINDSTRING:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoFINDSTRING( hWnd, (short)wParam, (LPCSTR)lParam, FALSE );
    case LB_FINDSTRINGEXACT:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoFINDSTRING( hWnd, (short)wParam, (LPCSTR)lParam, TRUE );
    case LB_GETANCHORINDEX:
        return DoGETANCHORINDEX( hWnd );
    case LB_GETCARETINDEX:
        return DoGETCARETINDEX( hWnd );
    case LB_GETCOUNT:
        return DoGETCOUNT( hWnd );
    case LB_GETCURSEL:
        return DoGETCURSEL( hWnd );
    case LB_GETHORIZONTALEXTENT:
        return DoGETHORIZONTALEXTENT( hWnd );
    case LB_GETITEMDATA:
        return DoGETITEMDATA( hWnd, (short)wParam );
    case LB_GETITEMHEIGHT:
        return DoGETITEMHEIGHT( hWnd, (short)wParam );
    case LB_GETITEMRECT:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoGETITEMRECT( hWnd, (short)wParam, (LPRECT)lParam );
    case LB_GETLOCALE:
        return DoGETLOCALE( hWnd );
    case LB_GETSEL:
        return DoGETSEL( hWnd, (short)wParam );
    case LB_GETSELCOUNT:
        return DoGETSELCOUNT( hWnd );
    case LB_GETSELITEMS:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoGETSELITEMS( hWnd, (short)wParam, (LPINT)lParam );
    case LB_GETTEXT:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoGETTEXT( hWnd, (short)wParam, (LPSTR)lParam );
    case LB_GETTEXTLEN:
        return DoGETTEXTLEN( hWnd, (short)wParam );
    case LB_GETTOPINDEX:
        return DoGETTOPINDEX( hWnd );
    case LB_INSERTSTRING:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoINSERTSTRING( hWnd, (short)wParam, (LPCSTR)lParam );
    case LB_RESETCONTENT:
        return DoRESETCONTENT( hWnd );
    case LB_SELECTSTRING:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoSELECTSTRING( hWnd, (short)wParam, (LPCSTR)lParam );
    case LB_SELITEMRANGE:
        return DoSELITEMRANGE( hWnd, (BOOL)wParam, LOWORD( lParam ), HIWORD( lParam ) );
    case LB_SELITEMRANGEEX:
        return DoSELITEMRANGEEX( hWnd, (short)wParam, (short)lParam );
    case LB_SETANCHORINDEX:
        return DoSETANCHORINDEX( hWnd, (short)wParam );
    case LB_SETCARETINDEX:
        return DoSETCARETINDEX( hWnd, (short)wParam, (BOOL)lParam );
    case LB_SETCOLUMNWIDTH:
        return DoSETCOLUMNWIDTH( hWnd, (short)wParam );
    case LB_SETCOUNT:
        return DoSETCOUNT( hWnd, (short)wParam );
    case LB_SETCURSEL:
        return DoSETCURSEL( hWnd, (short)wParam, FALSE );
    case LB_SETHORIZONTALEXTENT:
        return DoSETHORIZONTALEXTENT( hWnd, (short)wParam );
    case LB_SETITEMDATA:
        return DoSETITEMDATA( hWnd, (short)wParam, (DWORD)lParam );
    case LB_SETITEMHEIGHT:
        return DoSETITEMHEIGHT( hWnd, (short)wParam, (short)lParam );
    case LB_SETLOCALE:
        return DoSETLOCALE( hWnd, (WORD)wParam );
    case LB_SETSEL:
        return DoSETSEL( hWnd, (BOOL)wParam, (short)lParam );
    case LB_SETTABSTOPS:
#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );
#endif
        return DoSETTABSTOPS( hWnd, (short)wParam, (short*)lParam  );
    case LB_SETTOPINDEX:
        return DoSETTOPINDEX( hWnd, (short)wParam );
    case WM_SETREDRAW:
        return DoSetRedraw( hWnd, (BOOL)wParam );
    case WM_ACTIVATEAPP:
        return 1;
    case WM_SETCTLCOLOR:
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:
		return DoGetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hWnd );
    case WM_NCCREATE:
        return DoNCCREATE( hWnd, (LPCREATESTRUCT)lParam );
    case WM_CREATE:
		return DoCREATECombBoxList( hWnd, (LPCREATESTRUCT)lParam );
    case WM_DESTROY:
        return DoDestroy( hWnd );
    default:

	// It's important to do this
	// if your do'nt handle message, you must call DefWindowProc
	// 重要提示：假如你不需要处理消息，将该消息交由系统窗口默认处理函数去处理
	// 

        return DefWindowProc( hWnd, message, wParam, lParam );
    }
    return 0;
}
*/

