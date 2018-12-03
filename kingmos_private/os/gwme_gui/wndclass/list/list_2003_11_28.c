/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：list class
版本号：1.0.0
开发时期：2003-04-07
作者：李林
修改记录：
     2003-08-29  LN, not sent LBN_SELCHANGE when use LB_SETCURSEL
     2003-06-23, DoSETCARETINDEX 增加对Caret的隐藏
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <eassert.h>
#include <ealloc.h>
#include <gwmesrv.h>

#define ITEMEXT 10
#define ITEM_HEIGHT 16
#define ITEM_WIDTH 8
#define AUTO_UP 0xfffe
#define AUTO_DOWN 0xfffd
#define AUTO_HOLD 0xffc

#define _MAPPOINTER

//#define CL_BLACK    RGB( 0, 0, 0 )
//#define CL_WHITE    RGB( 0xff, 0xff, 0xff )
//#define CL_DARKGRAY RGB( 0x100, 0x100, 0x100 )

typedef struct __LISTITEM
{
	LPSTR lpszItem;
	DWORD dwItemData;
	WORD typeData;
	WORD state;
}_LISTITEM, FAR * _LPLISTITEM;

typedef struct  __LISTDATA
{
	_LPLISTITEM lpItems;
	DWORD dwStyle;
	short count;
	short bFree;
	
	short limit;
    short caret;

    short curSel;// use when single-selection
    short topIndex;

    short colWidth;
    short rowHeight;

    //short rowNum;
    short colNum;
    //short fRedraw;
    short bCombBoxList;
    HWND hCombBox;

	//UINT     fclMask;       // color mask
	COLORREF cl_Text;
	COLORREF cl_TextBk;     //正常文本的前景与背景色
	COLORREF cl_Selection;
	COLORREF cl_SelectionBk;  // 选择文本的前景与背景色
	COLORREF cl_Disable;
	COLORREF cl_DisableBk;    // 无效文本的前景与背景色
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
static BOOL Pack( _LPLISTDATA );
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
//static short GetItemTextRect( _LPLISTDATA, int index, LPRECT lpRect, DWORD style );
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

static const char classLISTBOX[] = "LISTBOX";
// register my define class
ATOM RegisterListBoxClass(HINSTANCE hInst)
{
   WNDCLASS wc;
// register MyButton
   wc.hInstance=hInst;
   wc.lpszClassName= classLISTBOX;
// the proc is class function
   wc.lpfnWndProc = ListBoxWndProc;
   wc.style=CS_DBLCLKS;
   wc.hIcon= 0;
// at pen window, no cursor
   wc.hCursor= LoadCursor(NULL,IDC_ARROW);
// to auto erase background, must set a valid brush
// if 0, you must erase background yourself
   wc.hbrBackground = NULL;//GetSysColorBrush(COLOR_WINDOW);//GetStockObject( WHITE_BRUSH );

   wc.lpszMenuName=NULL;
   wc.cbClsExtra=0;
// !!! it's important to save state of button, align to long
   wc.cbWndExtra=sizeof( void * );//(_LISTDATA );

   return RegisterClass(&wc);
}

static const char classCOMBOLISTBOX[] = "COMBOLISTBOX";
// register my define class
ATOM RegisterComboListBoxClass(HINSTANCE hInst)
{
   WNDCLASS wc;
// register MyButton
   wc.hInstance=hInst;
   wc.lpszClassName= classCOMBOLISTBOX;
// the proc is class function
   wc.lpfnWndProc = CombBoxListBoxWndProc;
   wc.style=CS_DBLCLKS;
   wc.hIcon= 0;
// at pen window, no cursor
   wc.hCursor= LoadCursor(NULL,IDC_ARROW);
// to auto erase background, must set a valid brush
// if 0, you must erase background yourself
   wc.hbrBackground = NULL;//GetSysColorBrush(COLOR_WINDOW);//GetStockObject( WHITE_BRUSH );

   wc.lpszMenuName=NULL;
   wc.cbClsExtra=0;
// !!! it's important to align to long
   wc.cbWndExtra=sizeof( void * );//(_LISTDATA );sizeof( _LISTDATA );

   return RegisterClass(&wc);
}


static _LPLISTITEM InsertItem( _LPLISTDATA lpListData, int * index )
{
    if(	lpListData->count == lpListData->limit )
  	{
        if( ResetLimit( lpListData, (lpListData->limit + ITEMEXT) ) == FALSE )
            return 0;
    }

    if( *index == -1 )
    {
        *index = lpListData->count;
    }
    else
    {
        memmove( lpListData->lpItems + *index + 1,
		         lpListData->lpItems + *index,
			       (lpListData->count - *index) * sizeof( _LISTITEM ) );
    }
    memset( lpListData->lpItems + *index, 0, sizeof(_LISTITEM) );
    lpListData->count++;
    return lpListData->lpItems + *index;
}

static BOOL RemoveItem( _LPLISTDATA lpListData, int index )
{
    //LPSTR lpcstr;
    _ASSERT( index >= 0 && index < lpListData->count );

	  if( index >= 0 && index < lpListData->count )
	  {
		    lpListData->count--;
            //lpcstr = (lpListData->lpItems + index)->lpszItem;
		    memmove( lpListData->lpItems + index,
			      lpListData->lpItems + index + 1,
			      (lpListData->count - index) * sizeof( _LISTITEM ) );
		    //return lpcstr;
			return TRUE;
	  }
	  return FALSE;
}

static int DeleteItem( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData, int index )
{
    _LPLISTITEM lpItem;
	lpItem = At( lpListData, index );

    if( dwStyle & LBS_HASSTRINGS )
	{
		//LPSTR lpstr;
		
		//lpstr = RemoveItem( lpListData, index );
		

		//if( lpstr )
		//if( lpItem )
		//{   // free space
			FreeItem( lpItem->lpszItem );
		//}
	}
	else if( dwStyle & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE) )
	{
        DELETEITEMSTRUCT ds;
        //ds.itemData = RemoveItem( lpListData, index );;
		ds.itemData = lpItem->dwItemData;
		if( ds.itemData )
		{
			ds.CtlType = ODT_LISTBOX;
			ds.CtlID = (WORD)GetWindowLong( hWnd, GWL_ID );
			ds.itemID = index;
			ds.hwndItem = hWnd;
			SendMessage( GetParent( hWnd ), WM_DELETEITEM, ds.CtlID, (LPARAM)&ds );
		}
		
	}
	else
		return LB_ERR;
    
	RemoveItem( lpListData, index );
    return lpListData->count;
}

static BOOL SetItemData( _LPLISTDATA lpListData, int index, DWORD dwData )
{
    _ASSERT( index >=0 && index < lpListData->count );
    //(lpListData->lpItems + index)->lpszItem = (LPSTR)dwData;
	(lpListData->lpItems + index)->dwItemData = dwData;
    return 0;
}

static BOOL ResetLimit( _LPLISTDATA lpListData, int newLimit )
{
    _LPLISTITEM lpItem;
    if( newLimit < ITEMEXT )
        newLimit = ITEMEXT;
    if( newLimit != lpListData->limit )
    {
        lpItem = (_LPLISTITEM)realloc( lpListData->lpItems, newLimit*sizeof( _LISTITEM ) );
        if( lpItem )
        {
            lpListData->lpItems = lpItem;
            lpListData->limit = newLimit;
        }
        else
            return FALSE;
    }
    return TRUE;
}

static BOOL SetItemTypeData( _LPLISTDATA lpListData, int index, WORD dwTypeData )
{
    _ASSERT( index >=0 && index < lpListData->count );
    (lpListData->lpItems+index)->typeData = dwTypeData;
    return 0;
}

static int SetItemState( _LPLISTDATA lpListData, int index, WORD state, BOOL bEnable )
{
    _ASSERT( index >=0 && index < lpListData->count );

    if( bEnable )
    {
        (lpListData->lpItems+index)->state |= state;
    }
    else
        (lpListData->lpItems+index)->state &= ~state;
    return 0;
}

static BOOL FreeItem( LPSTR lpstr )
{
    free( lpstr );
    return TRUE;
}


static _LPLISTITEM At( _LPLISTDATA lpListData, int index )
{
    _ASSERT( index >= 0 && index < lpListData->count );
	  //if( index >= 0 && index < lpListData->count )
    return lpListData->lpItems + index;
    //return 0;
}

int Count( _LPLISTDATA lpListData )
{
    return lpListData->count;
}

BOOL Pack( _LPLISTDATA lpListData )
{
    return TRUE;
}

static LRESULT DoADDFILE( HWND hWnd, LPCSTR lpName )
{
    return LB_ERR;
}

static LRESULT DoADDSTRING( HWND hWnd, LPCSTR lpcstr )
{
    return DoINSERTSTRING( hWnd, -1, lpcstr );
}

static void FillSolidRect( HDC hdc, const RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

static void ReflashScrollBar( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData )
{
//    RECT rect;
/*
    GetClientRect( hWnd, &rect );
    if( (dwStyle & LBS_MULTICOLUMN) == 0 && 
		lpListData->count > GetRows( lpListData, dwStyle, &rect ) )
        ShowScrollBar( hWnd, SB_VERT, TRUE );
    else
        ShowScrollBar( hWnd, SB_VERT, FALSE );
*/
	//if( lpListData->count > GetCols( lpListData, &rect ) )
        //ShowScrollBar( hWnd, SB_HORZ, TRUE );
    //else
        //ShowScrollBar( hWnd, SB_HORZ, FALSE );

    SetScrollBar( hWnd, lpListData->topIndex, 0 );
}

static LRESULT DoINSERTSTRING( HWND hWnd, int index, LPCSTR lpcstr )
{
    _LPLISTDATA lpListData;
    _LPLISTITEM lpItem;
    DWORD dwStyle;
    WORD len;
    LPSTR lp = NULL;
    int cur;    

    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    if( index > lpListData->limit )
    {
        return LB_ERR;
    }
	if( dwStyle & LBS_HASSTRINGS )
	{
		len = (WORD)strlen( lpcstr );
		lp = malloc( len + 1 );
		if( lp == NULL )
			return LBN_ERRSPACE;

	}
	cur = lpListData->curSel;
	DoSETCURSEL( hWnd, -1, FALSE );  // hide cur-sel
	
	lpItem = InsertItem( lpListData, &index );
	if( lpItem )
	{
		if( dwStyle & LBS_HASSTRINGS )
		{
			lpItem->state = 0;
			lpItem->typeData = len;
			lpItem->lpszItem = lp;
			strcpy( lpItem->lpszItem, lpcstr );
		}
		else
		{  //LBS_OWNERDRAWVARIABLE ..
			lpItem->lpszItem = NULL;
			lpItem->state = 0;
			lpItem->typeData = 0;
			lpItem->dwItemData = (DWORD)lpcstr;
		}
		if( dwStyle & LBS_OWNERDRAWVARIABLE )
		{
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
		
		if( index <= cur && !(dwStyle & LBS_MULTIPLESEL) )
			cur++;
		
		if( !(dwStyle & LBS_NOREDRAW) )
		{                
			HDC hdc;
			hdc = GetDC( hWnd );
			RedrawItem( lpListData, hWnd, dwStyle, hdc, index, lpListData->count, FALSE );
			ReleaseDC( hWnd, hdc );
			ReflashScrollBar( hWnd, dwStyle, lpListData );
		}
		DoSETCURSEL( hWnd, cur, FALSE );  // show cur-sel
		return index;
	}
	else
	{
		if( lp )
			free( lp );
	}
	DoSETCURSEL( hWnd, cur, FALSE );  // show cur-sel
	SendNotify( hWnd, LBN_ERRSPACE );
	return LBN_ERRSPACE;
}

LRESULT DoDELETESTRING( HWND hWnd, int index )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    short retv = LB_ERR, cur;
    DWORD dwStyle;
	RECT rect;

    _ASSERT( lpListData );

//    if( !(GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL) && lpListData->curSel >= 0 )
    if( index < lpListData->count && index >= 0 )
    {
        cur = lpListData->curSel;

        if( cur != -1 )
		    DoSETCURSEL( hWnd, -1, FALSE );  // hide cur-sel
		if( lpListData->caret != -1 )    // hide cur-caret
			DoSETCARETINDEX( hWnd, -1, FALSE );

		dwStyle = GetWindowLong( hWnd, GWL_STYLE );

        retv = DeleteItem( hWnd, dwStyle, lpListData, index );

		GetClientRect( hWnd, &rect );
		// reset the topIndex
		if( (dwStyle & LBS_OWNERDRAWVARIABLE) == 0 )			
		{			
			int rows = GetRows( lpListData, dwStyle, &rect );
			int t = rows;
			int iTopIndex;
			if( dwStyle & LBS_MULTICOLUMN )
			{				
				int cols = GetCols( lpListData, &rect );
				t = t * cols;
			}
			if( cur < lpListData->topIndex  )
			{				
				iTopIndex = cur - 1 < 0 ? 0 : cur;
			}
			else if( cur >= lpListData->topIndex + t )
			{
				iTopIndex = cur - t;// - lpListData->topIndex;
				//DoSETTOPINDEX( hWnd, cur - 1 < 0 ? 0 : cur );
			}
			else 
				iTopIndex = lpListData->topIndex;

			t -= (lpListData->count - iTopIndex );//lpListData->topIndex); 
			if( t > 0  )
			{					
				t = iTopIndex - t;//lpListData->topIndex - t;
				if( t < 0 )
					t = 0;
			}
			else
				t = iTopIndex;//lpListData->topIndex;
            //if( index == lpListData->topIndex )
            DoSETTOPINDEX( hWnd, t );
		}

        //if( cur > index )
            //cur--;
        //else if( cur == index && cur == lpListData->count )
          //  cur--;

        
/*
        if( !(dwStyle & LBS_NOREDRAW) )
		{
            ReflashScrollBar( hWnd, lpListData );
		}
*/
		if( !(dwStyle & LBS_NOREDRAW) )
		{                
			HDC hdc;
			
			hdc = GetDC( hWnd );
			if( index == lpListData->count )// the bottom item
			{
				RedrawItem( lpListData, hWnd, dwStyle, hdc, index-1, lpListData->count, TRUE );
			}
			else
			    RedrawItem( lpListData, hWnd, dwStyle, hdc, index, lpListData->count, TRUE );
			ReleaseDC( hWnd, hdc );
			ReflashScrollBar( hWnd, dwStyle, lpListData );
		}

        //if( cur >= 0 )
            //DoSETCURSEL( hWnd, cur, FALSE );
    }
    return retv;
}

static LRESULT DoDIR( HWND hWnd, WORD attrib, LPCSTR lpName )
{
    return LB_ERR;
}

static LRESULT DoFINDSTRING( HWND hWnd, int indexStart, LPCSTR lpFindStr, BOOL bExact )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    short i;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );

    _ASSERT( lpListData );
    if( indexStart == -1 )
        indexStart = 0;
    for( i = 0; i < lpListData->count; i++, indexStart++ )
    {
        if( indexStart >= lpListData->count )
            indexStart = 0;
        if( dwStyle & LBS_HASSTRINGS )
        {
            if( bExact )
			{
                if( stricmp( At( lpListData, indexStart )->lpszItem, lpFindStr ) == 0 )
                    return indexStart;
			}
			else
			{
                if( strstr( At( lpListData, indexStart )->lpszItem, lpFindStr ) )
                    return indexStart;
			}
        }
        else if( dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE) )
        {
            //if( At( lpListData, indexStart )->lpszItem == lpFindStr )
			if( At( lpListData, indexStart )->dwItemData == (DWORD)lpFindStr )
                return indexStart;
        }
        else
            break; // error
    }
    return LB_ERR;
}
/*
static LRESULT DoFINDSTRINGEXACT( HWND hWnd, short indexStart, LPCSTR lpFindStr )
{
//    return LB_ERR;
    _LPLISTDATA lpListData = (_LPLISTDATA)_Wnd_GetWndExtraDataAdr( hWnd );
    short i;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );

    _ASSERT( lpListData );
    if( indexStart == -1 )
        indexStart = 0;
    for( i = 0; i < lpListData->count; i++, indexStart++ )
    {
        if( indexStart >= lpListData->count )
            indexStart = 0;
        if( dwStyle & LBS_HASSTRINGS )
        {
            if( stricmp( At( lpListData, indexStart )->lpszItem, lpFindStr ) )
                return indexStart;
        }
        else if( dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE) )
        {
            if( At( lpListData, indexStart )->lpszItem == lpFindStr )
                return indexStart;
        }
        else
            break; // error
    }
    return LB_ERR;
}
*/

static LRESULT DoGETANCHORINDEX( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    int i;

    _ASSERT( lpListData );
    for( i = 0; i < lpListData->count; i++ )
    {
        if( At( lpListData, i )->state & ODS_SELECTED )
            return i;
    }
    return LB_ERR;
}

static LRESULT DoGETCARETINDEX( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    _ASSERT( lpListData );
    return lpListData->caret;
}

static LRESULT DoGETCOUNT( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    _ASSERT( lpListData );
    return Count( lpListData );
}

static LRESULT DoGETCURSEL( HWND hWnd )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    int i;

    _ASSERT( lpListData );

    if( dwStyle & LBS_MULTIPLESEL )
    {
        for( i = 0; i < lpListData->count; i++ )
        {
            if( At( lpListData, i )->state & ODS_FOCUS )
                return i;
        }
    }
    else
    {       // single-sel style
        return lpListData->curSel;
    }

    return LB_ERR;
}

static LRESULT DoGETHORIZONTALEXTENT( HWND hWnd )
{
    //_LPLISTDATA lpListData = (_LPLISTDATA)_Wnd_GetWndExtraDataAdr( hWnd );
    //_LPLISTITEM lpListItem;
    //short i, maxc = 0;

    //_ASSERT( lpListData );

    //for( i = 0; i < lpListData->count; i++ )
    //{
        //lpListItem = At( lpListData, i );
        //maxc = max( maxc, lpListItem->typeData );
    //}
    //return maxc * CHAR_WIDTH;
    return LB_ERR;
}

static LRESULT DoGETITEMDATA( HWND hWnd, int index )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

    _ASSERT( lpListData );
    if( index < lpListData->count )
    {
        //return (LRESULT)At( lpListData, index )->lpszItem;
		return (LRESULT)At( lpListData, index )->dwItemData;
    }
    return LB_ERR;
}

static LRESULT DoGETITEMHEIGHT( HWND hWnd, int index )
{
    DWORD dwStyle = (DWORD)GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

    _ASSERT( lpListData );

    if( index < lpListData->count )
    {
        if(  dwStyle & LBS_OWNERDRAWVARIABLE )
            return At( lpListData, index )->typeData;
        else if( index == 0 )
            return lpListData->rowHeight;
    }
    return LB_ERR;
}

static LRESULT DoGETITEMRECT( HWND hWnd, int index, LPRECT lpRect )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

    _ASSERT( lpListData );

    GetClientRect( hWnd, lpRect );
    if( GetItemRect( lpListData , index, lpRect, GetWindowLong( hWnd, GWL_STYLE ) ) )
        return 0;
    return LB_ERR;
}

static LRESULT DoGETLOCALE( HWND hWnd )
{
    return LB_ERR;
}

static LRESULT DoGETSEL( HWND hWnd, int index )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

    _ASSERT( lpListData );

    if( index < lpListData->count )
    {
        return At( lpListData, index )->state & ODS_SELECTED;
    }
    return LB_ERR;
}

static LRESULT DoGETSELCOUNT( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    int i, n = 0;

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {
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

static LRESULT DoGETSELITEMS( HWND hWnd, int maxItems, LPINT lpArray )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    int i, m = 0;

    _ASSERT( lpListData );

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {
        for( i = 0; i < lpListData->count && m < maxItems ; i++ )
        {
            if( At( lpListData, i )->state & ODS_SELECTED )
            {
                *lpArray++ = i;
                m++;
            }
        }
        return m;
    }
    return LB_ERR;
}

static LRESULT DoGETTEXT( HWND hWnd, int index, void * lpBuf )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    _LPLISTITEM lpItem;

    _ASSERT( lpListData );
    if( index < lpListData->count )
    {
        lpItem = At( lpListData, index );
		if( (DWORD)GetWindowLong( hWnd, GWL_STYLE ) & LBS_HASSTRINGS )
        {
            strcpy( lpBuf, lpItem->lpszItem );
            return lpItem->typeData;
        }
        else
        {
            //memcpy( lpBuf, &lpItem->lpszItem, 4 );
			memcpy( lpBuf, &lpItem->dwItemData, 4 );
            return 4;
        }
    }
    return LB_ERR;
}

static LRESULT DoGETTEXTLEN( HWND hWnd, int index )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

    _ASSERT( lpListData );
    if( index < lpListData->count )
    {
        if( (DWORD)GetWindowLong( hWnd, GWL_STYLE ) & LBS_HASSTRINGS )
            return At( lpListData, index )->typeData;
    }
    return LB_ERR;
}

static LRESULT DoGETTOPINDEX( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

    _ASSERT( lpListData );
    return lpListData->topIndex;
}

static LRESULT DoRESETCONTENT( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
//    DELETEITEMSTRUCT ds;
//    LPSTR lpstr;
    DWORD dwStyle = (DWORD)GetWindowLong( hWnd, GWL_STYLE );
    int i, fOwner;

    _ASSERT( lpListData );
    fOwner = (int)(dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE));

    for( i = lpListData->count - 1; i >= 0; i-- )
    {
        //static short DeleteItem( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData, short index )
		DeleteItem( hWnd, dwStyle, lpListData, i );
/*
		lpstr = RemoveItem( lpListData, i );
        ds.CtlType = ODT_LISTBOX;
        ds.CtlID = (WORD)GetWindowLong( hWnd, GWL_ID );
        ds.itemID = i;
        ds.hwndItem = hWnd;
        ds.itemData = (DWORD)lpstr;
        if( dwStyle & LBS_HASSTRINGS )
        {
            FreeItem( lpstr );
        }
        else if( fOwner )
            SendMessage( GetParent( hWnd ), WM_DELETEITEM, ds.CtlID, (LPARAM)&ds );
*/
    }
    lpListData->caret = -1;
    lpListData->curSel = -1;
	lpListData->topIndex = 0;
	lpListData->limit = 0;
	if( !fOwner )
	{
		InvalidateRect( hWnd, NULL, FALSE );
		UpdateWindow( hWnd );
	}

    return 0;
}

static LRESULT DoSELECTSTRING( HWND hWnd, int indexStart, LPCSTR lpcstr )
{
    indexStart = DoFINDSTRING( hWnd, indexStart, lpcstr, FALSE );
    if( indexStart != LB_ERR )
    {
         DoSETSEL( hWnd, TRUE, indexStart );
    }
    return indexStart;
}

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

static LRESULT DoSETANCHORINDEX( HWND hWnd, int index )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

    DoSELITEMRANGE( hWnd, FALSE, 0, lpListData->count-1 );
    DoSETSEL( hWnd, TRUE, index );
    return 0;
}

static LRESULT DoSETCARETINDEX( HWND hWnd, int index, BOOL fScroll )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    RECT rect, clientRect;
//    short rows;
    HDC hdc;
    DWORD dwStyle;
	int iRowNum;

    _ASSERT( lpListData );

    GetClientRect( hWnd, &clientRect );
    dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	iRowNum = GetRows( lpListData, dwStyle, &clientRect );//lpListData->rowNum;

    //if( DoSETSEL( hWnd, TRUE, index ) )
    if( index != lpListData->caret &&
        //index >= 0 &&                                // 2003-06-23-DEL  
		index >= -1 &&                                 // 2003-06-23-ADD
        index < lpListData->count )
    {
        //rect = clientRect; // 2003-06-23-DEL  
        //GetItemRect( lpListData, index, &rect, dwStyle );// 2003-06-23-DEL  
        //if( index >= 0 && !IntersectRect( &rect, &rect, &clientRect ) ) // 2003-06-23-DEL  
		if( index >= 0 ) // 2003-06-23-ADD
        {   // scroll to visible
			rect = clientRect;// 2003-06-23-ADD
			GetItemRect( lpListData, index, &rect, dwStyle );// 2003-06-23-ADD
			if( !IntersectRect( &rect, &rect, &clientRect ) )// 2003-06-23-ADD
			{				
                if( dwStyle & LBS_MULTICOLUMN )
                {
                    ScrollCol( hWnd,
						index / iRowNum -
						lpListData->topIndex / iRowNum );
                }
                else
                {
                    if( index < lpListData->topIndex )
                        ScrollRow( hWnd, (index - lpListData->topIndex) );
                    else
                        ScrollRow( hWnd, (index - (lpListData->topIndex+iRowNum-1) ) );
                }
			}
        }
        if( lpListData->caret != index )
        {
            hdc = GetDC( hWnd );
            if( lpListData->caret != -1 )
            {   // remove old caret
				rect = clientRect;
                GetItemRect( lpListData, lpListData->caret, &rect, dwStyle );
                DrawItemContents ( hWnd, lpListData, dwStyle, hdc, lpListData->caret, &rect, FALSE, ODA_SELECT );
            }
			if( index >= 0 && (dwStyle & (LBS_OWNERDRAWVARIABLE|LBS_OWNERDRAWFIXED)) == 0 )
			{   // show new
				rect = clientRect;
				GetItemRect( lpListData, index, &rect, dwStyle );
				DrawCaret( hdc, &rect );
			}

            lpListData->caret = index;
            ReleaseDC( hWnd, hdc );
        }
        return 0;
    }
    return LB_ERR;
}

static LRESULT DoSETCOLUMNWIDTH( HWND hWnd, int cxWidth )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    RECT rect;

    if( (GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTICOLUMN) && cxWidth > 0 )
    {
        lpListData->colWidth = cxWidth;
        GetClientRect( hWnd, &rect );
        //lpData->rowNum = GetRows( lpData, &rect );
        lpListData->colNum = GetCols( lpListData, &rect );
        InvalidateRect( hWnd, 0, TRUE );
        return 0;
    }
    return LB_ERR;
}

static LRESULT DoSETCOUNT( HWND hWnd, short count )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    if( (dwStyle & LBS_NODATA) && (dwStyle & LBS_HASSTRINGS)==0 )
    {
        lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
        _ASSERT( lpListData );
        if( count != Count( lpListData ) )
        {
            if( ResetLimit( lpListData, count ) == FALSE )
            {
                SendNotify( hWnd, LBN_ERRSPACE );
                return LB_ERRSPACE;
            }
        }
        return 0;
    }
    return LB_ERR;
}

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
    //GetItemTextRect( lpListData, index, &rect, dwStyle );
	GetItemRect( lpListData, index, &rect, dwStyle );
    DrawItemContents ( hWnd, lpListData, dwStyle, hdc, index, &rect, index == lpListData->caret, ODA_SELECT );
}

static LRESULT DoSETCURSEL( HWND hWnd, int index, BOOL bNotify )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    HDC hdc;
    _LPLISTDATA lpListData;
    RECT rect;
    if( (dwStyle & LBS_MULTIPLESEL)==0 && index >= -1 )
    {
        lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
        _ASSERT( lpListData );

        if( index < lpListData->count )
        {
            GetClientRect( hWnd, &rect );
            hdc = GetDC( hWnd );
            // hide cursel if possible
            if( lpListData->curSel != -1 )
                ShowSelectIndex( lpListData, hWnd, lpListData->curSel, hdc, &rect, dwStyle, FALSE );

            if( index >= 0 )
                ShowSelectIndex( lpListData, hWnd, index, hdc, &rect, dwStyle, TRUE );

            lpListData->curSel = index;
            ReleaseDC( hWnd, hdc );
// LN, 2003-08-29 not sent LBN_SELCHANGE when use LB_SETCURSEL
// 		    if( bNotify )
//			    SendNotify( hWnd, LBN_SELCHANGE ); 
		}
        return index;
/*

            if( lpListData->curSel != -1 )// hide cursel
                SetItemState( lpListData, lpListData->curSel, ODS_SELECTED, FALSE );
            SetItemState( lpListData, index, ODS_SELECTED, TRUE ); // show cursel

            hdc = GetDC( hWnd );
            if( lpListData->curSel != -1 )// hide cursel
            {
                GetClientRect( hWnd, &rect );
                GetItemTextRect( lpListData, lpListData->curSel, &rect, dwStyle );
                DrawItemContents ( hdc, At( lpListData, lpListData->curSel ), &rect, lpListData->curSel == lpListData->caret );
            }
            GetClientRect( hWnd, &rect );
            GetItemTextRect( lpListData, index, &rect, dwStyle );
            DrawItemContents ( hdc, At( lpListData, index ), &rect, lpListData->caret == index );

            lpListData->curSel = index;
            ReleaseDC( hWnd, hdc );
            return 0;
*/
    }
    return LB_ERR;
}

static LRESULT DoSETHORIZONTALEXTENT( HWND hWnd, short cx )
{
    return LB_ERR;
}

static LRESULT DoSETITEMDATA( HWND hWnd, short index, DWORD dwData )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    //if( (dwStyle & LBS_HASSTRINGS)==0 )
    {
        lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
        _ASSERT( lpListData );
        if( index < lpListData->count && index >=0 )
            return SetItemData( lpListData, index, dwData );
    }
    return LB_ERR;
}

static LRESULT DoSETITEMHEIGHT( HWND hWnd, short index, short height )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    RECT rect;

    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    _ASSERT( lpListData );
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
    {
       if( index < lpListData->count && index >=0 )
           return SetItemTypeData( lpListData, index, (WORD)height );
    }
    else
    {
        if( height > 0 && index == 0 )
        {
            lpListData->rowHeight = height;
            GetClientRect( hWnd, &rect );
            //lpListData->rowNum = GetRows( lpListData, dwStyle, &rect );
            InvalidateRect( hWnd, 0, TRUE );
            return 0;
        }
    }
    return LB_ERR;
}

static LRESULT DoSELITEMRANGEEX( HWND hWnd, short first, short last )
{  
    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {
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

static LRESULT DoSETLOCALE( HWND hWnd, WORD idLocale )
{
    return LB_ERR;
} 

static LRESULT DoSETSEL( HWND hWnd, BOOL fSelect, int index )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    HDC hdc;
    short i, retv = LB_ERR;
    RECT rect;

    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    _ASSERT( lpListData );
    if( dwStyle & LBS_MULTIPLESEL )
    {
        if( index >= 0 && index < lpListData->count )
        {
            retv = SetItemState( lpListData, index, ODS_SELECTED, fSelect );
            SendNotify( hWnd, LBN_SELCHANGE );

            hdc = GetDC( hWnd );
            GetClientRect( hWnd, &rect );
            //GetItemTextRect( lpListData, index, &rect, dwStyle );
			GetItemRect( lpListData, index, &rect, dwStyle );
            DrawItemContents ( hWnd, lpListData, dwStyle, hdc, index, &rect, lpListData->caret == index, ODA_SELECT );
            ReleaseDC( hWnd, hdc );
        }
        else if( index < -1 )
        {
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

static LRESULT DoSETTABSTOPS( HWND hWnd, short num, short * lpArray )
{
    return LB_ERR;
}

static LRESULT DoSETTOPINDEX( HWND hWnd, int index )
{
    _LPLISTDATA lpListData;
    
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    _ASSERT( lpListData );
    if( index >= 0 && index < lpListData->count && index != lpListData->topIndex )
    {
        lpListData->topIndex = index;
        InvalidateRect( hWnd, 0, TRUE );
        return 0;
    }
    return LB_ERR;
}

static _LPLISTDATA _DoInitListData( HWND hWnd, LPCREATESTRUCT lpcs )
{
    _LPLISTDATA lpListData;
    TEXTMETRIC tm;
    HDC hdc;
    RECT rect;
	
	lpListData = (_LPLISTDATA)malloc( sizeof( _LISTDATA ) );
    //ListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

	if( lpListData )
	{
		memset( lpListData, 0, sizeof( _LISTDATA ) );
        ResetLimit( lpListData, 0 );

		lpListData->dwStyle = lpcs->style;
		lpcs->style &= ~(WS_VSCROLL | WS_HSCROLL);

		if( lpcs->style & LBS_OWNERDRAWFIXED )
		{
			MEASUREITEMSTRUCT mis;
			mis.CtlID = (UINT)lpcs->hMenu;
			mis.CtlType = ODT_LISTBOX;
			mis.itemData = 0;
			mis.itemHeight = 0;
			mis.itemID = 0;
			mis.itemWidth = 0;
			if( SendMessage( lpcs->hParent, WM_MEASUREITEM, mis.CtlID, (LPARAM)&mis ) )
			{
                lpListData->rowHeight = (short)mis.itemHeight;
			}			
		}

		if( lpListData->rowHeight == 0 )
		{
			hdc = GetDC( hWnd );
			GetTextMetrics( hdc, &tm );
			ReleaseDC( hWnd, hdc );
			lpListData->rowHeight = (short)tm.tmHeight + 4;
		}

        lpListData->colWidth = lpcs->cx;
        lpListData->caret = -1;
        lpListData->curSel = -1;
        //lpListData->fRedraw = TRUE;

        lpListData->bCombBoxList= FALSE;
        lpListData->hCombBox = NULL;

		lpListData->cl_Disable = GetSysColor(COLOR_GRAYTEXT);
		lpListData->cl_DisableBk = GetSysColor(COLOR_WINDOW);
		lpListData->cl_Selection = GetSysColor(COLOR_HIGHLIGHTTEXT);
		lpListData->cl_SelectionBk = GetSysColor(COLOR_HIGHLIGHT);
		lpListData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);
		lpListData->cl_TextBk = GetSysColor(COLOR_WINDOW);

        GetClientRect( hWnd, &rect );

        //lpListData->rowNum = GetRows( lpListData, lpcs->style, &rect );
        lpListData->colNum = GetCols( lpListData, &rect );

		SetWindowLong( hWnd, 0, (long)lpListData );
		SetWindowLong( hWnd, GWL_STYLE, lpcs->style );
	}
	return lpListData;
}

static LRESULT DoCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
{
    _LPLISTDATA lpListData = _DoInitListData( hWnd, lpcs );
	if( lpListData )
	{
		return 0;
	}
	return -1;
}

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

/*
static LRESULT DoCREATECombBoxList( HWND hWnd, LPCREATESTRUCT lpcs )
{
    _LPLISTDATA lpListData;
    TEXTMETRIC tm;
    HDC hdc;
    RECT rect;

//    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
	lpListData = (_LPLISTDATA)malloc( sizeof( _LISTDATA ) );
	if( lpListData )
	{
		memset( lpListData, 0, sizeof( _LISTDATA ) );
        ResetLimit( lpListData, 0 );
        hdc = GetDC( hWnd );
        GetTextMetrics( hdc, &tm );
        ReleaseDC( hWnd, hdc );
        lpListData->rowHeight = (short)tm.tmHeight;
        lpListData->colWidth = lpcs->cx;
        lpListData->caret = -1;
        lpListData->curSel = -1;
        //lpListData->fRedraw = TRUE;

        lpListData->bCombBoxList=TRUE;
        lpListData->hCombBox = lpcs->lpCreateParams;

        GetClientRect( hWnd, &rect );
        //lpListData->rowNum = GetRows( lpListData, lpcs->style, &rect );
        lpListData->colNum = GetCols( lpListData, &rect );
        SetWindowLong( hWnd, 0, (long)lpListData );
		return 0;
	}

	return -1;
}
*/

static LRESULT DoDestroy( HWND hWnd )
{
    _LPLISTDATA lpListData;
    //DELETEITEMSTRUCT dis;
    HWND hParent;

    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
//    _ASSERT( lpListData );

    hParent = GetParent( hWnd );
    //dis.CtlType = ODT_LISTBOX;
    //dis.CtlID = (WORD)GetWindowLong( hWnd, GWL_ID );
    //dis.hwndItem = hWnd;

    while( lpListData->count )
    {
        //static short DeleteItem( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData, short index )
		DeleteItem( hWnd, dwStyle, lpListData, lpListData->count-1 );
/*
		dis.itemID = lpListData->count-1;
         if( dwStyle & LBS_HASSTRINGS )
             DeleteItem( lpListData, dis.itemID );
         else
         {   // send message to owner
             dis.itemData = (DWORD)RemoveItem( lpListData, dis.itemID );
             SendMessage( hParent, WM_DELETEITEM, (WPARAM)dis.CtlID, (LPARAM)&dis );
         }
*/
    }
    free( lpListData->lpItems );
    free( lpListData );
    return 0;
}

static int AtRow( _LPLISTDATA lpListData, DWORD dwStyle, int y )
{
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
	{
		int cy = 0;
		int indexStart; 
		_LPLISTITEM lpItem;
		
		if( lpListData->count )
		{
			indexStart = lpListData->topIndex;
			
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
	{
		if( y >= 0 )
			return y / lpListData->rowHeight;
		else
			return -1 + y / lpListData->rowHeight;
	}
}

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

static int GetRows( _LPLISTDATA lpListData, DWORD dwStyle, LPCRECT lpClientRect )
{
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
	{
		int cy = 0;
		int indexStart; 
		_LPLISTITEM lpItem;

		if( lpListData->count )
		{
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
	{
		if( lpListData->rowHeight > 0 )
			return lpClientRect->bottom / lpListData->rowHeight;
		else
			return 1;
	}
}

static int GetCols( _LPLISTDATA lpListData, LPCRECT lpClientRect )
{
    if( lpListData->colWidth > 0 )
	    return lpClientRect->right / lpListData->colWidth;
	else
		return 1;
}
/*
static short GetItemTextRect( _LPLISTDATA lpListData, int index, LPRECT lpRect, DWORD style )
{
    int crows, row, col;

    if( style & LBS_MULTICOLUMN )
    {
        crows = GetRows( lpListData, lpRect );
        col = (index - lpListData->topIndex) / crows;
        row = (index - lpListData->topIndex) % crows;
        lpRect->left = col * lpListData->colWidth;
        lpRect->right = lpRect->left + lpListData->colWidth;
        lpRect->top = row * lpListData->rowHeight;
		lpRect->bottom = lpRect->top + lpListData->rowHeight;
        return 1;
    }
    else
    {
		//        lpRect->left = 0;
		//        lpRect->right = lpListData->colWidth;
        lpRect->top = (index-lpListData->topIndex) * lpListData->rowHeight;
		lpRect->bottom = lpRect->top + lpListData->rowHeight;
        return 1;
    }
}
*/
static short GetItemRect( _LPLISTDATA lpListData, int index, LPRECT lpRect, DWORD dwStyle )
{
    int crows, row, col;

    if( dwStyle & LBS_MULTICOLUMN )
    {
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
    {
//        lpRect->left = 0;
//        lpRect->right = lpListData->colWidth;
		if( dwStyle & LBS_OWNERDRAWVARIABLE )
		{
	        int y = 0;
			int indexStart; 
			_LPLISTITEM lpItem;

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
				//dn = 1;
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
		{
			lpRect->top = (index-lpListData->topIndex) * lpListData->rowHeight;
			lpRect->bottom = lpRect->top + lpListData->rowHeight;
		}
        return 1;
    }
}

static void DrawCaret( HDC hdc, LPCRECT lpRect )
{
    DrawFocusRect( hdc, lpRect );
}

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
		{
			//MEASUREITEMSTRUCT mis;
			DRAWITEMSTRUCT dis;
//			int id;
			HWND hParent;
			
			dis.CtlID = GetWindowLong( hWnd, GWL_ID );
			dis.CtlType = ODT_LISTBOX;
			//if( dwStyle & LBS_HASSTRINGS )
			    //dis.itemData = (DWORD)lpItem->lpszItem;
			//else
			dis.itemData = lpItem->dwItemData;
			dis.itemID = index;
		
			hParent = GetParent(hWnd);			

			dis.hDC = hdc;
			dis.hwndItem = hWnd;
			dis.itemAction = uiAction;
			dis.itemState = lpItem->state;
			dis.rcItem =  *lpRect;
			SendMessage( hParent, WM_DRAWITEM, dis.CtlID, (LPARAM)&dis );
		}
		else
		{  // string list
			if( bHasCaret )
			{
				DrawCaret( hdc,lpRect );
			}
			if( lpItem->state & (ODS_DISABLED|ODS_GRAYED) )
			{
//				if( lpListData->fclMask & CLF_DISABLECOLOR )
				    SetTextColor( hdc, lpListData->cl_Disable );
				//else
				  //  SetTextColor( hdc, GetSysColor(COLOR_GRAYTEXT) );//CL_DARKGRAY );

//				if( lpListData->fclMask & CLF_DISABLEBKCOLOR )
				    SetBkColor( hdc, lpListData->cl_DisableBk );
//				else
//				    SetBkColor( hdc, GetSysColor(COLOR_WINDOW) );//CL_DARKGRAY );
			}
			else if( lpItem->state & ODS_SELECTED )
			{   // selected item color
//				if( lpListData->fclMask & CLF_SELECTIONCOLOR )
				    SetTextColor( hdc, lpListData->cl_Selection );
//				else
//				    SetTextColor( hdc, GetSysColor(COLOR_HIGHLIGHTTEXT) );//CL_DARKGRAY );

//				if( lpListData->fclMask & CLF_DISABLEBKCOLOR )
				    SetBkColor( hdc, lpListData->cl_SelectionBk );
//				else
//				    SetBkColor( hdc, GetSysColor(COLOR_WINDOW) );//CL_DARKGRAY );

//				SetBkColor( hdc, CL_BLACK );
//				SetTextColor( hdc, CL_WHITE );
			}
			else
			{
				SetBkColor( hdc, lpListData->cl_TextBk );//CL_WHITE );
				SetTextColor( hdc, lpListData->cl_Text );//CL_BLACK );

//				SetBkColor( hdc, CL_WHITE );
//				SetTextColor( hdc, CL_BLACK );
			}
			ExtTextOut( hdc,
				lpRect->left, lpRect->top,
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
        //FillRect( hdc, lpRect, GetStockObject( WHITE_BRUSH ) );
		FillSolidRect( hdc, lpRect, lpListData->cl_TextBk );
    }
}

static void SendNotify( HWND hWnd, const short code )
{
    DWORD wParam;
  	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_NOTIFY )
    {
        wParam = GetWindowLong( hWnd, GWL_ID );
        wParam = MAKELONG( wParam, code );
        if (lpListData->bCombBoxList==TRUE)               
           SendMessage( lpListData->hCombBox, WM_COMMAND, wParam, (LPARAM)hWnd );
        else
           SendMessage( GetParent( hWnd ), WM_COMMAND, wParam, (LPARAM)hWnd );
    }
}

static BOOL SetScrollBar( HWND hWnd, int pos, int *lpNewPos )
{
	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    SCROLLINFO si;
    RECT rect;
	DWORD dwStyle;
    short rows,t, cols;

//    _ASSERT( lpListData );
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	GetClientRect( hWnd, &rect );
    rows = GetRows( lpListData, dwStyle, &rect  );//lpListData->rowNum;
    if( lpListData->dwStyle & WS_VSCROLL )
    {   
//        rows = (rect.bottom+lpListData->rowHeight-1) / lpListData->rowHeight;
//        if( lpRows )
//            *lpRows = rows;
        
        si.cbSize = sizeof( si );
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        si.nMin = 0;
        t = lpListData->count-1;
        si.nMax = max( t, 0 );
        si.nPage = rows;//min( rows, si.nMax );
        if( pos < 0 )
            pos = 0;
        if( pos >= lpListData->count - rows )
            pos = si.nMax;
        si.nPos = pos;
        if( si.nMin < si.nMax )
        {
            SetScrollInfo( hWnd, SB_VERT, &si, TRUE );
            if( lpNewPos )
                *lpNewPos = pos;
            return TRUE;
        }
    }
    
	else if( lpListData->dwStyle & WS_HSCROLL )
    {
        cols = GetCols( lpListData, &rect );
        si.cbSize = sizeof( si );
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        si.nMin = 0;
        si.nMax = lpListData->count / (cols * rows);//rows;//lpListData->rowNum;
        si.nPage = cols;//1;// min( si.nMax, 1 );
        if( pos < 0 )
            pos = 0;
        if( pos >= si.nMax )
            pos = si.nMax;
        si.nPos = pos;

        if( si.nMin < si.nMax )
        {
            SetScrollInfo( hWnd, SB_HORZ, &si, TRUE );
            if( lpNewPos )
                *lpNewPos = pos;
            return TRUE;
        }
    }
    return FALSE;
}

static int ScrollCol( HWND hWnd, int code  )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
//    int index;
	RECT rect;
	int iRowNum;
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );

	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpListData, dwStyle, &rect );


    if( SetScrollBar( hWnd, code + lpListData->topIndex / iRowNum, &code ) )
    {
        code *= iRowNum;//lpListData->rowNum;
        if( code != lpListData->topIndex )
            DoSETTOPINDEX( hWnd, code );
    }
    return 0;
}

static int ScrollRow( HWND hWnd, int code  )
{
	  _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
//    short rows;
//    BOOL retv;
	  RECT rect;
	  int iRowNum;
	  DWORD dwStyle;

//    _ASSERT( lpListData );
    dwStyle = GetWindowLong( hWnd, GWL_STYLE );
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
    //}
    return 0;
}

//static void CALLBACK TimerProc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime )
//{
    //PostMessage( hWnd, 0x118, idEvent, 0 );
//}


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

	GetClientRect( hWnd, &rcClient );
	if( iFrom < lpListData->topIndex )
		iFrom = lpListData->topIndex;
	//ASSERT( iFrom < iTo );
	yBottom = rcClient.top;//rect.top;
	for( i = iFrom; i < iTo; i++ )
    {
        //GetItemTextRect( lpListData, i, &rect, dwStyle );
		rect = rcClient;
		GetItemRect( lpListData, i, &rect, dwStyle );
		yBottom = rect.bottom;
        if( IntersectRect( &rcClip, &rect, &rcClient ) )
		    DrawItemContents( hWnd, lpListData, dwStyle, hdc, i, &rect, lpListData->caret == i, ODA_DRAWENTIRE );
        else
		{
            bEraseBottom = FALSE;
			break;
		}
    }
	if( bEraseBottom )
	{
		rect = rcClient;
		rect.top = yBottom;
		if( rect.top < rect.bottom )
		{
			//FillRect( hdc, &rect, (HBRUSH)GetClassLong( hWnd, GCL_HBRBACKGROUND ) );
			FillSolidRect( hdc, &rect, lpListData->cl_TextBk );
		}		
	}
}

static LRESULT DoPAINT( HWND hWnd )
{
	_LPLISTDATA lpData;
	HDC hdc;
	PAINTSTRUCT ps;
	//RECT rect, clientRect, tempRect;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );

	lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
//	_ASSERT( lpData );    
	hdc = BeginPaint( hWnd, &ps );
	RedrawItem( lpData, hWnd, dwStyle, hdc, lpData->topIndex, lpData->count, TRUE );
/*
	for( i = lpData->topIndex; i < lpData->count; i++ )
    {
        rect = clientRect;
        GetItemTextRect( lpData, i, &rect, dwStyle );
        if( IntersectRect( &tempRect, &rect, &clientRect ) )
            DrawItemContents ( hdc, At( lpData, i ), &rect, lpData->caret == i );
        else
            break;
    }
*/
    EndPaint( hWnd, &ps );
    return 0;
}

static LRESULT DoLBUTTONDOWN( HWND hWnd, short x, short y, BOOL bNotify )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	_LPLISTDATA lpData;
    short h, col, row, i, iRowNum;
	RECT rect;

    if( GetFocus() != hWnd )
        SetFocus( hWnd );

    lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
	_ASSERT( lpData );

	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpData, dwStyle, &rect );

    col = AtCol( lpData, dwStyle, x );
    row = AtRow( lpData, dwStyle, y );
    i = col * iRowNum + row;
    h = lpData->topIndex + i;
    if( h >= 0 )
    {
   // send notify message
		if(  dwStyle & LBS_NOTIFY )
		{
			LBNOTIFY lbNotify, * plbNotify;
			DWORD dwParam;

			plbNotify = &lbNotify;
#ifdef _MAPPOINTER
		    plbNotify = (LBNOTIFY*)MapPtrToProcess( plbNotify, GetCurrentProcess() );
#endif
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
			//LBN_CLICK
			//PostMessage( GetParent( hWnd ), WM_COMMAND, wParam, (LPARAM)hWnd );
		}

        if( dwStyle & LBS_MULTIPLESEL )
            DoSETSEL( hWnd, !DoGETSEL( hWnd, h ), h );
        else if( h != lpData->curSel )
		{
// 2003-08-29, ADD
            if( bNotify )
		        SendNotify( hWnd, LBN_SELCHANGE );
            DoSETCURSEL( hWnd, h, bNotify );
            DoSETCARETINDEX( hWnd, h, TRUE );
		}
    }
    SetCapture( hWnd );
    return 0;
}

static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y, BOOL bNotify )
{
    if( GetCapture() == hWnd )
    {   //  ComboBox 需要在这里加 Notify.
        if( bNotify )
		    SendNotify( hWnd, LBN_SELCHANGE );
        ReleaseCapture();
    }
    return 0;
}


static LRESULT DoMOUSEMOVE( HWND hWnd, WPARAM wParam, short x, short y, BOOL bNotify )
{
    int pos, iRowNum;
    _LPLISTDATA lpData;
	RECT rect;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

//	  _ASSERT( lpData );

	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpData, dwStyle, &rect );

    if( (wParam & MK_LBUTTON) )
    {
        pos = (short)DoGETTOPINDEX( hWnd ) + AtRow( lpData, dwStyle, y );
        if( (dwStyle & LBS_MULTICOLUMN) )
        {
            pos += AtCol( lpData, dwStyle, x ) * iRowNum;
        }
//        if( (dwStyle & LBS_MULTICOLUMN) == 0  ||       // modify 2000-05-18 by ln
//             (pos < lpData->rowNum && pos >= 0 ) )     // modify 2000-05-18 by ln
        if( pos >= 0 )                                   // modify 2000-05-18 by ln
        {                                                // modify 2000-05-18 by ln
//            pos += (short)DoGETTOPINDEX( hWnd ) +      // modify 2000-05-18 by ln
//                   AtCol( lpData, x ) * lpData->rowNum;// modify 2000-05-18 by ln
//            if( pos >= 0 )                             // modify 2000-05-18 by ln
            {
                if( dwStyle & LBS_MULTIPLESEL )
                    return DoSETCARETINDEX( hWnd,
                                            pos,//(pos - 1),
                                            TRUE );
                else
                {
                    DoSETCURSEL( hWnd, pos, bNotify );
                    return DoSETCARETINDEX( hWnd, pos, TRUE );
                }
            }
        }
    }
    return 0;
}

static LRESULT DoLBUTTONDBLCLK( HWND hWnd )
{
    WPARAM wParam;

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_NOTIFY )
    {
        wParam = GetWindowLong( hWnd, GWL_ID );
        wParam = MAKELONG( wParam, LBN_DBLCLK );
        PostMessage( GetParent( hWnd ), WM_COMMAND, wParam, (LPARAM)hWnd );
    }
    return 0;
}

static LRESULT DoCHAR( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lrs = -1;
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
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

static LRESULT DoKEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lrs = -1;
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );

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
        }
        if( rows )
        {
//            ScrollRow( hWnd, rows );
			if(  lpListData->curSel + rows < 0 )
				DoSETCURSEL( hWnd, 0, TRUE );
			else
                DoSETCURSEL( hWnd, lpListData->curSel + rows, TRUE );
            DoSETCARETINDEX( hWnd, lpListData->curSel, TRUE );
        }
        else if( cols )
            ScrollCol( hWnd, cols );
    }
    else if( lrs >= 0 )  // lrs is index of item, do default action at index of lrs
    {
    }
    return 0;
}

static LRESULT DoSetRedraw( HWND hWnd, BOOL bRedraw )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );

    if( bRedraw )
        dwStyle &= ~LBS_NOREDRAW;
    else
        dwStyle |= LBS_NOREDRAW;
    //lpListData->fRedraw = bRedraw;
    return 0;
}

static LRESULT DoHSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    RECT rect;
    int cols, i;
	int iRowNum;
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );

//    _ASSERT( lpListData );
    //GetClientRect( hWnd, &rect );

	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpListData, dwStyle, &rect );

    cols = lpListData->count / iRowNum;//(rect.bottom +lpListData->rowHeight-1) / lpListData->rowHeight;
    i = lpListData->topIndex / iRowNum;

    switch( code )
    {
    case SB_BOTTOM:
        ScrollCol( hWnd, cols - i );
        break;
    case SB_ENDSCROLL:
        break;
    case SB_LINEDOWN:
        ScrollCol( hWnd, 1 );
        break;
    case SB_LINEUP:
        ScrollCol( hWnd, -1 );
        break;
    case SB_PAGEDOWN:
        ScrollCol( hWnd, lpListData->colNum );
        break;
    case SB_PAGEUP:
        ScrollCol( hWnd, (short)-lpListData->colNum );
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
        ScrollCol( hWnd, pos - i );
//        i = (short)DoGETTOPINDEX( hWnd );
//        if( i != pos )
//        {
            //DoSETTOPINDEX( hWnd, pos );
            //SetScrollBar( hWnd, pos, 0 );
        //}
        break;
    case SB_TOP:
        ScrollCol( hWnd, -i );
        break;
    }
    return 0;
}

static LRESULT DoVSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    RECT rect;
    int rows, i;
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	

//    _ASSERT( lpListData );
    GetClientRect( hWnd, &rect );
	rows = GetRows( lpListData, dwStyle, &rect );

//    rows = (rect.bottom +lpListData->rowHeight-1) / lpListData->rowHeight;
    //rows = lpListData->rowNum;

    switch( code )
    {
    case SB_BOTTOM:
        ScrollRow( hWnd, lpListData->count );
        break;
    case SB_ENDSCROLL:
        break;
    case SB_LINEDOWN:
        ScrollRow( hWnd, 1 );
        break;
    case SB_LINEUP:
        ScrollRow( hWnd, -1 );
        break;
    case SB_PAGEDOWN:
        ScrollRow( hWnd, rows );
        break;
    case SB_PAGEUP:
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
    case SB_TOP:
        i = (short)DoGETTOPINDEX( hWnd );
        ScrollRow( hWnd, -i );
        break;
    }
    return 0;
}

LRESULT DoNCCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
{
    if( ( lpcs->style & LBS_MULTICOLUMN ) &&
        ( lpcs->style & WS_VSCROLL ) )
    {
        lpcs->style &= ~WS_VSCROLL;
    }
    else if( ( lpcs->style & LBS_MULTICOLUMN ) == 0 &&
             ( lpcs->style & WS_HSCROLL ) )
    {
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

static LRESULT WINAPI DoKILLFOCUS( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//_Wnd_GetWndExtraDataAdr( hWnd );
    HDC hdc;
    RECT rect;
	DWORD dwStyle;

//    _ASSERT( lpListData );
    if( lpListData->caret != - 1 )
    {
        hdc = GetDC( hWnd );
        GetClientRect( hWnd, &rect );
        GetItemRect( lpListData, lpListData->caret, &rect, GetWindowLong( hWnd, GWL_STYLE ) );
		dwStyle = GetWindowLong( hWnd, GWL_STYLE );
        DrawItemContents ( hWnd, lpListData, dwStyle, hdc, lpListData->caret, &rect, FALSE, ODA_FOCUS );
        lpListData->caret = -1;
        ReleaseDC( hWnd, hdc );

//        DoSETCARETINDEX( hWnd, -1, FALSE );
    }
    SendNotify( hWnd, LBN_KILLFOCUS );
    
    return 0;
}

static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
	    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
		if( lpccs->fMask & CLF_TEXTCOLOR )
		{
//			lpListData->fMask |= CLF_TEXTCOLOR;
			lpListData->cl_Text = lpccs->cl_Text;
		}
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		{
//			lpListData->fMask |= CLF_TEXTBKCOLOR;
			lpListData->cl_TextBk = lpccs->cl_TextBk;
		}
		if( lpccs->fMask & CLF_SELECTIONCOLOR )
		{
//			lpListData->fMask |= CLF_SELECTIONCOLOR;
			lpListData->cl_Selection = lpccs->cl_Selection;
		}
		if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
		{
//			lpListData->fMask |= CLF_SELECTIONBKCOLOR;
			lpListData->cl_SelectionBk = lpccs->cl_SelectionBk;
		}

		if( lpccs->fMask & CLF_DISABLECOLOR )
		{
//			lpListData->fMask |= CLF_DISABLECOLOR;
			lpListData->cl_Disable = lpccs->cl_Disable;
		}
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
		{
//			lpListData->fMask |= CLF_DISABLEBKCOLOR;
			lpListData->cl_DisableBk = lpccs->cl_DisableBk;
		}

		return TRUE;
	}
	return FALSE;
}

static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
	    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
		if( lpccs->fMask & CLF_TEXTCOLOR )
		{
//			if( lpListData->fclMask & CLF_TEXTCOLOR )
			    lpccs->cl_Text = lpListData->cl_Text;
//		    else
//			    lpccs->cl_Text = GetSysColor( COLOR_WINDOWTEXT );
		}
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		{
//			if( lpListData->fMask & CLF_TEXTBKCOLOR )
			    lpccs->cl_TextBk = lpListData->cl_TextBk;
//		    else
//			    lpccs->cl_TextBk = GetSysColor( COLOR_WINDOW );
		}
        //
		if( lpccs->fMask & CLF_SELECTIONCOLOR )
		{
//			if( lpListData->fMask & CLF_SELECTIONCOLOR )
			    lpccs->cl_Selection = lpListData->cl_Selection;
//		    else
//			    lpccs->cl_Selection = GetSysColor( COLOR_HIGHLIGHTTEXT );
		}

		if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
		{
//			if( lpListData->fMask & CLF_SELECTIONBKCOLOR )
			    lpccs->cl_SelectionBk = lpListData->cl_SelectionBk;
//		    else
//			    lpccs->cl_SelectionBk = GetSysColor( COLOR_HIGHLIGHT );
		}
        //
		if( lpccs->fMask & CLF_DISABLECOLOR )
		{
//			if( lpListData->fMask & CLF_DIABLECOLOR )
			    lpccs->cl_Disable = lpListData->cl_Disable;
//		    else
//			    lpccs->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
		}
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
		{
//			if( lpListData->fMask & CLF_DIABLEBKCOLOR )
			    lpccs->cl_DisableBk = lpListData->cl_DisableBk;
//		    else
//			    lpccs->cl_DisableBk = GetSysColor( COLOR_WINDOW );
		}

		return TRUE;
	}
	return FALSE;	
}

static LRESULT WINAPI ListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch( message )
	{
	case WM_PAINT:
		return DoPAINT( hWnd );
    case WM_LBUTTONDOWN:
		RETAILMSG( 1, ("DOWN.\r\n" ) );
        return DoLBUTTONDOWN( hWnd, LOWORD( lParam ), HIWORD( lParam ), TRUE );
    case WM_LBUTTONUP:
		RETAILMSG( 1, ("UP.\r\n" ) );
        return DoLBUTTONUP( hWnd, LOWORD( lParam ), HIWORD( lParam ), FALSE );
    case WM_MOUSEMOVE:
		RETAILMSG( 1, ("MOVE.\r\n" ) );
        return DoMOUSEMOVE( hWnd, wParam, LOWORD(lParam), HIWORD(lParam), TRUE );
    case WM_LBUTTONDBLCLK:
		RETAILMSG( 1, ("CLICK.\r\n" ) );
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
/*
    case WM_CHARTOITEM:
        return DoCHARTOITEM(
    case WM_DELETEITEM      :
        return DoDELETEITEM(
    case WM_VKEYTOITEM:
        return DoVKEYTOITEM(
*/
    case WM_SETREDRAW:
        return DoSetRedraw( hWnd, (BOOL)wParam );
    case WM_SETCTLCOLOR:
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:
		return DoGetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_NCCREATE:
        return DoNCCREATE( hWnd, (LPCREATESTRUCT)lParam );
    case WM_CREATE:
		return DoCREATE( hWnd, (LPCREATESTRUCT)lParam );
    case WM_DESTROY:
        return DoDestroy( hWnd );
    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }
    return 0;
}

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
/*
    case WM_CHARTOITEM:
        return DoCHARTOITEM(
    case WM_DELETEITEM      :
        return DoDELETEITEM(
    case WM_VKEYTOITEM:
        return DoVKEYTOITEM(
*/
    case WM_SETREDRAW:
        return DoSetRedraw( hWnd, (BOOL)wParam );
    case WM_ACTIVATEAPP:
        return 1;
    case WM_SETCTLCOLOR:
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:
		return DoGetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_NCCREATE:
        return DoNCCREATE( hWnd, (LPCREATESTRUCT)lParam );
    case WM_CREATE:
		return DoCREATECombBoxList( hWnd, (LPCREATESTRUCT)lParam );
    case WM_DESTROY:
        return DoDestroy( hWnd );
    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }
    return 0;
}

