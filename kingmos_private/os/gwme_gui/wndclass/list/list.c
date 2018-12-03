/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����list class
�汾�ţ�1.0.0
����ʱ�ڣ�2003-04-07
���ߣ�����
�޸ļ�¼��
	2005-01-26, add code, DoRESETCONTENT û��ˢ�¹�����
	2004-12-21, �ı� DoLBUTTONUP & DoLBUTTONDOWN , ȥ�� SetCapture����
				��Ϊ�п����� ��LB_CLICK ��AP�����������棬Ȼ���ֱ� SetCapture

	2004-10-27, DoSETCURSEL 2004-10-27, modify, �Ż�
	2004-08-27, �� _LISTDATA �� count ��Ա�� short ��Ϊ WORD
	2004-03-06, LN DoSetRedraw ֮ǰ��û�����κ�����
	2003-08-29  LN, not sent LBN_SELCHANGE when use LB_SETCURSEL
    2003-06-23, DoSETCARETINDEX ���Ӷ�Caret������
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

// �����б����Ŀ�ṹ
typedef struct __LISTITEM
{
	LPSTR lpszItem;		//�б����Ŀ����
	DWORD dwItemData;	//�б����Ŀ�û�����
	WORD typeData;		//�б����Ŀ�û���������
	WORD state;			//��ǰ�б���Ŀ״̬
}_LISTITEM, FAR * _LPLISTITEM;

//	�����б��ṹ
typedef struct  __LISTDATA
{
	_LPLISTITEM lpItems;	//�б����Ŀָ��
	DWORD dwStyle;			//�б����
	WORD count;			//�б��ǰ��Ŀ��
	BYTE bFree;			//�Ƿ����б���ͷ����ݣ����ã�
	BYTE bDown;			//�Ƿ��ڻ����д��� WM_LBUTTONDOWN ��Ϣ
	
	short limit;			//�б������ɵ������Ŀ��Ŀ
    short caret;			//�б��ǰ��ע��Ŀ

    short curSel;			//�б��ǰ��ѡ����Ŀ�����ڵ���ѡ����use when single-selection
    short topIndex;			//�б��ǰ��ʾ���ڵ����Ŀ

    short colWidth;			//�б���п�
    short rowHeight;		//�б���п�

    short colNum;			//�б������
    short bCombBoxList;		//�Ƿ�����Ͽ��б���
    HWND hCombBox;			//��������Ͽ��б�������Ч����ʾ��Ͽ򴰿�

	COLORREF cl_Text;		//�����ı���ǰ���뱳��ɫ
	COLORREF cl_TextBk;     //
	COLORREF cl_Selection;	//ѡ���ı���ǰ���뱳��ɫ
	COLORREF cl_SelectionBk;  // 
	COLORREF cl_Disable;	//��Ч�ı���ǰ���뱳��ɫ
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

// �б������
static const char strClassListBox[] = "LISTBOX";

// **************************************************
// ������ATOM RegisterListBoxClass(HINSTANCE hInst)
// ������
//		IN hInst - ʵ�����
// ����ֵ��
//		����ɹ������ط�0ֵ��ʧ�ܣ�����0
// ����������
//		ע���б����
// ����: 
//		��sysclass.c ����
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
   //˽������
   wc.cbWndExtra= sizeof( void * );

   return RegisterClass(&wc);
}

// ����б������
static const char strClassComboListBox[] = "COMBOLISTBOX";

// register my define class
// **************************************************
// ������ATOM RegisterComboListBoxClass(HINSTANCE hInst)
// ������
//		IN hInst - ʵ�����
// ����ֵ��
//		����ɹ������ط�0ֵ��ʧ�ܣ�����0
// ����������
//		ע������б����
// ����: 
//		��sysclass.c ����
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
   ////˽������
   wc.cbWndExtra=sizeof( void * );

   return RegisterClass(&wc);
}

// **************************************************
// ������static HDC GetListDC( HWND hWnd, PAINTSTRUCT * lpps, HFONT hFont )
// ������
//	IN hWnd - ���ھ��
//	IN lpps - PAINTSTRUCT �ṹָ��
//	IN hFont - ������
// ����ֵ��
//	����ɹ�������DC ���
// ����������
//	�õ�DC(��� lpps ΪNULL,��GetDC,������BeginPaint)
// ����: 
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
// ������static VOID ReleaseListDC( HWND hWnd, PAINTSTRUCT * lpps, _LPLISTDATA lpListData )
// ������
//	IN hWnd - ���ھ��
//	IN hdc - DC���
//	IN lpps - PAINTSTRUCT �ṹָ��
// ����ֵ��
//	��
// ����������
//	�ͷ�DC
// ����: 
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
// ������static _LPLISTITEM InsertItem( _LPLISTDATA lpListData, int * index )
// ������
//	IN/OUT lpListData - �б�����ָ��
//	IN/OUT index - ���б����Ŀ����λ�ã�����Ϊ-1,��ʾ���б����Ŀ�嵽��󲢷���ʵ�ʵ�λ��
// ����ֵ��
//	����ɹ��������µ���Ŀָ��; ���򣬷���0
// ����������
//	�ڵ�ǰ�б��λ������һ���б���Ŀ���ڴ�ռ�
// ����: 
//	
// ************************************************

static _LPLISTITEM InsertItem( _LPLISTDATA lpListData, int * index )
{
    if(	lpListData->count == lpListData->limit )
  	{	// û�и�����б���Ŀ���ڴ�ռ䣬��Ҫ�����б���Ŀ
		// ÿ�������̶���Ŀ����Ŀ�ռ��Ա��Ժ�ʹ��
        if( ResetLimit( lpListData, (lpListData->limit + ITEMEXT) ) == FALSE )
            return 0;		//����ʧ��
    }

    if( *index == -1 )
    {	//����Ŀ��ĩβ
        *index = lpListData->count;
    }
    else
    {	//����Ŀ���б���м䣬��Ҫ�ƶ��Ѿ����ڵ���Ŀ
		//[oooooooooo new oooooooold]
        memmove( lpListData->lpItems + *index + 1,
		         lpListData->lpItems + *index,
			       (lpListData->count - *index) * sizeof( _LISTITEM ) );
    }
	//��ʼ���µ�
    memset( lpListData->lpItems + *index, 0, sizeof(_LISTITEM) );
    lpListData->count++;	//����ʵ����Ŀ��
	//��������Ŀ
    return lpListData->lpItems + *index;
}

// **************************************************
// ������static BOOL RemoveItem( _LPLISTDATA lpListData, int index )
// ������
//	IN/OUT lpListData - �б�����ָ��
//	IN index - �б���Ŀ��������
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSe
// ����������
//	�Ƴ��б���Ŀ,���������������б���Ŀ���ڴ�ռ�
// ����: 
//	
// ************************************************

static BOOL RemoveItem( _LPLISTDATA lpListData, int index )
{
    _ASSERT( (DWORD)index < lpListData->count );
	
	//if( index >= 0 && index < lpListData->count )
	if( (DWORD)index < lpListData->count )
	{	// �����źϷ�
		lpListData->count--;
		// ͨ���ƶ��ڴ�ķ����Ƴ�����Ŀ
		// [  IIIIIIII  (remove item)   IIIIIIIIIII ]
		memmove( lpListData->lpItems + index,
			lpListData->lpItems + index + 1,
			(lpListData->count - index) * sizeof( _LISTITEM ) );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������static int DeleteItem( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData, int index )
// ������
//	IN hWnd - �б��ھ��
//	IN dwStyle - �б��ڷ��
//	IN/OUT lpListData - �б�����ָ��
//	IN index - �б���Ŀ������
// ����ֵ��
//	����ɹ������ط��ص�ǰ�б��ʵ�ʵ��б���Ŀ��; ���򣬷���LB_ERR
// ����������
//	ɾ���б���Ŀ���ݣ����Ƴ���Ŀ�ṹ
//	���ص�ǰ�б��ʵ�ʵ��б���Ŀ��
// ����: 
//	
// ************************************************

static int DeleteItem( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData, int index )
{
    _LPLISTITEM lpItem;
	lpItem = At( lpListData, index );
	
    if( dwStyle & LBS_HASSTRINGS )
	{	//�б���ĿΪ�ַ��������Ŀ���ݣ��ͷŸ���Ŀ����
		FreeItem( lpItem->lpszItem );
	}
	else if( dwStyle & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE) )
	{	//�б���ĿΪ�û��Ի��񣬱������û�ȥ�ͷ���Ŀ����
        DELETEITEMSTRUCT ds;
		ds.itemData = lpItem->dwItemData;
		if( ds.itemData )
		{	//׼�����͵�����
			ds.CtlType = ODT_LISTBOX;
			ds.CtlID = (WORD)GetWindowLong( hWnd, GWL_ID );
			ds.itemID = index;
			ds.hwndItem = hWnd;
			// �򸸴��ڷ�������
			SendMessage( GetParent( hWnd ), WM_DELETEITEM, ds.CtlID, (LPARAM)&ds );
		}		
	}
	else
		return LB_ERR;	//����
    //�Ƴ���Ŀ�ṹ
	RemoveItem( lpListData, index );
    return lpListData->count;
}

// **************************************************
// ������static int SetItemData( _LPLISTDATA lpListData, int index, DWORD dwData )
// ������
//	IN/OUT lpListData - �б�����ָ��
//	IN index - �б���Ŀ������
//	IN dwData - ��Ҫ���õ�����
// ����ֵ��
//	����ɹ�������0; ���򣬷���LB_ERR
// ����������
//	������Ŀ�û�����
// ����: 
//	
// ************************************************

static BOOL SetItemData( _LPLISTDATA lpListData, int index, DWORD dwData )
{
    _ASSERT( index >=0 && index < lpListData->count );
	(lpListData->lpItems + index)->dwItemData = dwData;
    return 0;
}

// **************************************************
// ������static BOOL ResetLimit( _LPLISTDATA lpListData, int newLimit )
// ������
//	IN/OUT lpListData - �б�����ָ��
//	IN newLimit - �µ���Ŀ�ڴ�ռ�
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���·�����Ŀ�ڴ�ռ�
// ����: 
//	
// ************************************************

static BOOL ResetLimit( _LPLISTDATA lpListData, int newLimit )
{
    _LPLISTITEM lpItem;
	// ������ڹ̶�����Ŀ��
    if( newLimit < ITEMEXT )
        newLimit = ITEMEXT;	
    if( newLimit != lpListData->limit )
    {	//���·���
        lpItem = (_LPLISTITEM)realloc( lpListData->lpItems, newLimit*sizeof( _LISTITEM ) );
        if( lpItem )
        {	//����ɹ��������µ��б������
            lpListData->lpItems = lpItem;
            lpListData->limit = newLimit;
        }
        else
            return FALSE;
    }
    return TRUE;
}

// **************************************************
// ������static int SetItemTypeData( _LPLISTDATA lpListData, int index, WORD dwTypeData )
// ������
//	IN/OUT lpListData - �б�����ָ��
//	IN index - �б���Ŀ������
//	IN dwTypeData - ��Ҫ���õ���������
// ����ֵ��
//	����ɹ�������0; ���򣬷���LB_ERR
// ����������
//	������Ŀ��������
// ����: 
//	
// ************************************************

static int SetItemTypeData( _LPLISTDATA lpListData, int index, WORD dwTypeData )
{
    _ASSERT( index >=0 && index < lpListData->count );
    (lpListData->lpItems+index)->typeData = dwTypeData;
    return 0;
}

// **************************************************
// ������static int SetItemState( _LPLISTDATA lpListData, int index, WORD state, BOOL bEnable )
// ������
//	IN/OUT lpListData - �б�����ָ��
//	IN index - �б���Ŀ������
//	IN state - �б���Ŀ״̬
//	IN bEnable - ��ʾ���û��������Ŀ״̬
// ����ֵ��
//	����ɹ�������0; ���򣬷���LB_ERR
// ����������
//	���û������Ŀ״̬
// ����: 
//	
// ************************************************

static int SetItemState( _LPLISTDATA lpListData, int index, WORD state, BOOL bEnable )
{
    _ASSERT( index >=0 && index < lpListData->count );

    if( bEnable )
    {	//����
        (lpListData->lpItems+index)->state |= state;
    }
    else	//���
        (lpListData->lpItems+index)->state &= ~state;
    return 0;
}

// **************************************************
// ������static BOOL FreeItem( LPSTR lpstr )
// ������
//	IN lpstr - �ַ���ָ��
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�ͷ���Ŀ����
// ����: 
//	
// ************************************************

static BOOL FreeItem( LPSTR lpstr )
{
    free( lpstr );
    return TRUE;
}

// **************************************************
// ������static _LPLISTITEM At( _LPLISTDATA lpListData, int index )
// ������
//	IN lpListData - �б�����ָ��
//	IN index - �б���Ŀ������
// ����ֵ��
//	����ɹ��������б���Ŀָ��; ���򣬷���NULL
// ����������
//	�õ��б���Ŀ�����Ŷ�Ӧ���б���Ŀָ�����
// ����: 
//	
// ************************************************

static _LPLISTITEM At( _LPLISTDATA lpListData, int index )
{
    _ASSERT( index >= 0 && index < lpListData->count );
    return lpListData->lpItems + index;
}

// **************************************************
// ������static int Count( _LPLISTDATA lpListData )
// ������
//	IN lpListData - �б�����ָ��
// ����ֵ��
//	�����б���ʵ����Ŀ��
// ����������
//	ͳ���б���ʵ����Ŀ��
// ����: 
//	
// ************************************************

static int Count( _LPLISTDATA lpListData )
{
    return lpListData->count;
}

// **************************************************
// ������static BOOL Pack( _LPLISTDATA lpListData )
// ������
//	IN lpListData - �б�����ָ��
// ����ֵ��
//	����ɹ�������; ���򣬷���0
// ����������
//	
// ����: 
//	
// ************************************************

//static BOOL Pack( _LPLISTDATA lpListData )
//{
//    return TRUE;
//}

// **************************************************
// ������static LRESULT DoADDFILE( HWND hWnd, LPCSTR lpName )
// ������
//	IN hWnd - ���ھ��
//	IN lpName - �ļ���
// ����ֵ��
//	����ɹ����������ӵ���Ŀ������; ���򣬷���LB_ERR
// ����������
//	�м��ļ������б��
//	�ݲ�֧��
// ����: 
//	
// ************************************************

static LRESULT DoADDFILE( HWND hWnd, LPCSTR lpName )
{
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoADDSTRING( HWND hWnd, LPCSTR lpcstr )
// ������
//	IN hWnd - ���ھ��
//	IN lpcstr - �ַ�����Ŀ
// ����ֵ��
//	����ɹ�������������Ŀ������; ���򣬼���û���ڴ�, ���� LB_ERRSPACE, �������󷵻�LB_ERR
// ����������
//	����һ���ַ�����Ŀ��ĩβ
//	���� LB_ADDSTRING ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoADDSTRING( HWND hWnd, LPCSTR lpcstr )
{
    return DoINSERTSTRING( hWnd, -1, lpcstr );
}

// **************************************************
// ������static void FillSolidRect( HDC hdc, const RECT * lprc, COLORREF clr )
// ������
//	IN hdc - ��ͼDC 
//	IN lprc - ��Ҫ���ľ���
//	IN clr - ��ɫ
// ����ֵ��
//	��
// ����������
//	�����ο�
// ����: 
//	
// ************************************************

static void FillSolidRect( HDC hdc, const RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

// **************************************************
// ������static void ReflashScrollBar( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData )
// ������
//	IN hWnd - �б�򴰿ھ��
//	IN dwStyle - ���ڷ��
//	IN lpListData - �б�����ָ�� 
// ����ֵ��
//	����ɹ�������; ���򣬷���0
// ����������
//	ˢ�¹��������ݺ���ʾ
// ����: 
//	
// ************************************************

static void ReflashScrollBar( HWND hWnd, DWORD dwStyle, _LPLISTDATA lpListData )
{
    SetScrollBar( hWnd, lpListData->topIndex, 0 );
}

// **************************************************
// ������static LRESULT DoINSERTSTRING( HWND hWnd, int index, LPCSTR lpcstr )
// ������
//	IN hWnd - ���ھ�� 
//	IN index - ��Ҫ�����λ�� 
//	IN lpcstr - ��Ҫ������ַ���
// ����ֵ��
//	����ɹ������ز����������; ���򣬼���û���ڴ�, ����LB_ERRSPACE, ��������, ����LB_ERR
// ����������
//	����һ���ַ�����Ŀ��ָ����λ��
//	���� LB_INSERTSTRING
// ����: 
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

    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//�õ��б��˽������ָ��
	// �õ����ڷ��
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    if( index > lpListData->limit )
    {	// ����λ��
        return LB_ERR;
    }
	if( dwStyle & LBS_HASSTRINGS )
	{	// �б����ĿΪ�ַ���, �����µ��ַ����ڴ�
		len = (WORD)strlen( lpcstr );
		lp = malloc( len + 1 );
		if( lp == NULL )
			return LBN_ERRSPACE;

	}
	// �������,������ǰѡ����
	cur = lpListData->curSel;
	DoSETCURSEL( hWnd, -1, FALSE );  // hide cur-sel
	// ����
	lpItem = InsertItem( lpListData, &index );
	if( lpItem )
	{	//����ɹ�
		if( dwStyle & LBS_HASSTRINGS )
		{	// ������ַ�������, ��������
			lpItem->state = 0;
			lpItem->typeData = len;
			lpItem->lpszItem = lp;
			strcpy( lpItem->lpszItem, lpcstr );
		}
		else
		{   // �û��Զ����������� LBS_OWNERDRAWVARIABLE ..
			lpItem->lpszItem = NULL;
			lpItem->state = 0;
			lpItem->typeData = 0;
			lpItem->dwItemData = (DWORD)lpcstr;
		}
		if( dwStyle & LBS_OWNERDRAWVARIABLE )
		{	// ������Զ�����������, ������Ϣ��������ȥ�õ���Ŀ�߶�
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
		// ������ǰѡ�����������
		if( index <= cur && !(dwStyle & LBS_MULTIPLESEL) )
			cur++;
		
		if( !(dwStyle & LBS_NOREDRAW) )
		{	// ��Ҫ�ػ�
			HDC hdc;
			// �õ���ͼDC
			hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
			// ��ͼ��Ŀ
			RedrawItem( lpListData, hWnd, dwStyle, hdc, index, lpListData->count, FALSE );
			// �ͷ�DC
			ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
			// ˢ�¹�����
			ReflashScrollBar( hWnd, dwStyle, lpListData );
		}
		// ��ʾѡ����
		DoSETCURSEL( hWnd, cur, FALSE );  // show cur-sel
		return index;
	}
	else
	{	//���벻�ɹ�, �ͷ�֮ǰ������
		if( lp )
			free( lp );
	}
	//	������ʾ֮ǰ���ص���Ŀ
	DoSETCURSEL( hWnd, cur, FALSE );  // show cur-sel
	// �򸸴��ڷ���֪ͨ��Ϣ
	SendNotify( hWnd, LBN_ERRSPACE );
	return LBN_ERRSPACE;
}

// **************************************************
// ������LRESULT DoDELETESTRING( HWND hWnd, int index )
// ������
//	IN hWnd - ���ھ�� 
//	IN index - ��Ŀ������
// ����ֵ��
//	����ɹ������ص�ǰ�б����Ŀ��; ���򣬷���LB_ERR
// ����������
//	ɾ�������Ŷ�Ӧ����Ŀ
//	���� LB_DELETESTRING
// ����: 
//	
// ************************************************

LRESULT DoDELETESTRING( HWND hWnd, int index )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    short retv = LB_ERR, cur;
    DWORD dwStyle;
	RECT rect;

    _ASSERT( lpListData );

    //if( index < lpListData->count && index >= 0 )
	if( (DWORD)index < lpListData->count )
    {	//�Ϸ���������ֵ
        cur = lpListData->curSel;
		//������ѡ������ע��,����������
        if( cur != -1 )
		    DoSETCURSEL( hWnd, -1, FALSE );  // hide cur-sel
		if( lpListData->caret != -1 )    // hide cur-caret
			DoSETCARETINDEX( hWnd, -1, FALSE );
		//�õ����ڷ��
		dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		//ɾ����Ŀ
        retv = DeleteItem( hWnd, dwStyle, lpListData, index );
		//�õ����ڿͻ�����������
		GetClientRect( hWnd, &rect );
		// reset the topIndex
		// �������õ�ǰ���ڵĵ�һ����Ŀ
		if( (dwStyle & LBS_OWNERDRAWVARIABLE) == 0 )			
		{	// �õ����ھ����ܹ���ʾ���������
			int rows = GetRows( lpListData, dwStyle, &rect );
			int t = rows;
			int iTopIndex;
			if( dwStyle & LBS_MULTICOLUMN )
			{   // ������ʾ���, �õ�����
				int cols = GetCols( lpListData, &rect );
				t = t * cols;	// ��ǰ����������ʾ�������Ŀ��
			}
			if( cur < lpListData->topIndex  )
			{	//ѡ����Ŀ�����űȴ��ڵ����Ŀ��С
				iTopIndex = cur - 1 < 0 ? 0 : cur;
			}
			else if( cur >= lpListData->topIndex + t )
			{	//ѡ����Ŀ�����űȴ��ڵ������Ŀ�Ŵ�
				iTopIndex = cur - t;
			}
			else	//
				iTopIndex = lpListData->topIndex;
			//ȷ�����һҳ�����Ŀ������
			t -= (lpListData->count - iTopIndex ); 
			if( t > 0  )
			{	//��ǰ�����Ŀ������(iTopIndex)֮�� û���㹻����Ŀ��ʾ���һҳ
				t = iTopIndex - t;
				if( t < 0 )
					t = 0;	//���Ŀ������Ӧ��Ϊ0
			}
			else
				t = iTopIndex;	//�㹻
            DoSETTOPINDEX( hWnd, t );
		}

		if( !(dwStyle & LBS_NOREDRAW) )
		{	// ��Ҫ�ػ���                
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
// ������static LRESULT DoDIR( HWND hWnd, WORD attrib, LPCSTR lpName )
// ������
//	IN hWnd - ���ھ��
//	IN attrib - �ļ�����
//	IN lpName - �ļ���
// ����ֵ��
//	����ɹ�������������ӵ���Ŀ������; ���򣬼���û���ڴ�, ����LB_ERRSPACE, �������󷵻�LB_ERR
// ����������
//	�����ļ����ļ����е��ļ����б��
//	���� LB_DIR
// ����: 
//	
// ************************************************

static LRESULT DoDIR( HWND hWnd, WORD attrib, LPCSTR lpName )
{
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoFINDSTRING( HWND hWnd, int indexStart, LPCSTR lpFindStr, BOOL bExact )
// ������
//	IN hWnd - ���ھ�� 
//	IN indexStart - ��������ʼ������ 
//	IN lpFindStr - ��Ҫƥ����ַ��� 
//	IN bExact - �Ƿ�ȷƥ��(��ȷƥ����ζ�䳤��ҲҪƥ��)
// ����ֵ��
//	����ɹ���������Ŀ������; ���򣬷���LB_ERR
// ����������
//	����ƥ�����Ŀ
//	���� LB_FINDSTRING �� LB_FINDSTRINGEXACT
// ����: 
//	
// ************************************************

static LRESULT DoFINDSTRING( HWND hWnd, int indexStart, LPCSTR lpFindStr, BOOL bExact )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    short i;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );//�õ��б����

    _ASSERT( lpListData );
    if( indexStart == -1 )
        indexStart = 0;	//��0��ʼ
	// ��indexStart��ʼ�������е���Ŀ
    for( i = 0; i < lpListData->count; i++, indexStart++ )
    {
        if( indexStart >= lpListData->count )
            indexStart = 0;	// ���û�����������е���Ŀ,�ӵ�һ����ʼ
        if( dwStyle & LBS_HASSTRINGS )
        {	// �ַ�������
            if( bExact )
			{	// ��ȷƥ��
                if( stricmp( At( lpListData, indexStart )->lpszItem, lpFindStr ) == 0 )
                    return indexStart;
			}
			else
			{	// �����ַ���
                if( strstr( At( lpListData, indexStart )->lpszItem, lpFindStr ) )
                    return indexStart;
			}
        }
        else if( dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE) )
        {	// �û�����
			if( At( lpListData, indexStart )->dwItemData == (DWORD)lpFindStr )
                return indexStart;
        }
        else
            break; // error
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETANCHORINDEX( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ��������б��ѡ����Ŀ�еĵ�һ����Ŀ����; ���򣬷���LB_ERR
//	
// ����������
//	�õ��б��ѡ����Ŀ�еĵ�һ����Ŀ���������ڶ���ѡ��
//	���� LB_GETANCHORINDEX ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETANCHORINDEX( HWND hWnd )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    int i;

    _ASSERT( lpListData );
    for( i = 0; i < lpListData->count; i++ )
    {
        if( At( lpListData, i )->state & ODS_SELECTED )
            return i;		//�ҵ�
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETCARETINDEX( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ��������б��ѡ����Ŀ�е����һ����Ŀ����; ���򣬷���LB_ERR
// ����������
//	�õ��б��ѡ����Ŀ�е����һ����Ŀ���������ڶ���ѡ��
//	���� LB_GETCARETINDEX
// ����: 
//	
// ************************************************

static LRESULT DoGETCARETINDEX( HWND hWnd )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    return lpListData->caret;
}

// **************************************************
// ������static LRESULT DoGETCOUNT( HWND hWnd )
// ������
//	IN  hWnd - ���ھ��
// ����ֵ��
//	����ɹ��������б�����Ŀ����; ���򣬷���LB_ERR
// ����������
//	�õ��б�����Ŀ����
//	���� LB_GETCOUNT
// ����: 
//	
// ************************************************

static LRESULT DoGETCOUNT( HWND hWnd )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    return Count( lpListData );
}

// **************************************************
// ������static LRESULT DoGETCURSEL( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ��������б��ѡ����Ŀ�е��н������Ŀ����; ���򣬷���LB_ERR
//	
// ����������
//	����б���Ƕ���ѡ���񣬵õ��б��ѡ����Ŀ�е��н������Ŀ����
//	����б���ǵ���ѡ���񣬵õ��б��ѡ����Ŀ����Ŀ����
//	���� LB_GETCURSEL ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETCURSEL( HWND hWnd )
{	// �õ����ڷ��
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    //�õ��б�򴰿ڵ��б�����ݶ���ָ��
	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    int i;

    _ASSERT( lpListData );

    if( dwStyle & LBS_MULTIPLESEL )
    {	// ����ѡ����
        for( i = 0; i < lpListData->count; i++ )
        {	
            if( At( lpListData, i )->state & ODS_FOCUS )
                return i;	//
        }
    }
    else
    {   // ����ѡ�� single-sel style
        return lpListData->curSel;
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETHORIZONTALEXTENT( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ��������б���ˮƽ���; ���򣬷���LB_ERR
//	
// ����������
//	�õ�֮ǰ���� LB_SETHORIZONTALEXTENT ���õ�ˮƽ���
//	���� LB_GETHORIZONTALEXTENT ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETHORIZONTALEXTENT( HWND hWnd )
{
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETITEMDATA( HWND hWnd, int index )
// ������
//	IN hWnd - ���ھ��
//  IN index - ��Ŀ����
// ����ֵ��
//	����ɹ��������б����Ŀ����Ŀ����; ���򣬷���LB_ERR
// ����������
//	�õ��б����Ŀ����Ŀ����
//	���� LB_GETITEMDATA ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETITEMDATA( HWND hWnd, int index )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );
    if( (DWORD)index < lpListData->count )
    {	// ��Ч������
		return (LRESULT)At( lpListData, index )->dwItemData;
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETITEMHEIGHT( HWND hWnd, int index )
// ������
//	IN hWnd - ���ھ��
//  IN index - ��Ŀ����
// ����ֵ��
//	����ɹ��������б����Ŀ����Ŀ�߶�; ���򣬷���LB_ERR
// ����������
//	�õ��б����Ŀ����Ŀ�߶ȣ� �б������� LBS_OWNERDRAWVARIABLE ��񣻷���index����Ϊ0 ��
//	���� LB_GETITEMHEIGHT ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETITEMHEIGHT( HWND hWnd, int index )
{	// �õ����ڷ��
    DWORD dwStyle = (DWORD)GetWindowLong( hWnd, GWL_STYLE );
    //�õ��б�򴰿ڵ��б�����ݶ���ָ��
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
// ������static LRESULT DoGETITEMRECT( HWND hWnd, int index, LPRECT lpRect )
// ������
//	IN hWnd - ���ھ�� 
//	IN index - ��Ŀ���� 
//	OUT lpRect - ���ڽ�����Ŀ����
// ����ֵ��
//	����ɹ�������0��lpRect������Ŀ��������; ���򣬷���LB_ERR
// ����������
//	�õ���Ŀ��������
//	���� LB_GETITEMRECT ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETITEMRECT( HWND hWnd, int index, LPRECT lpRect )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );

    GetClientRect( hWnd, lpRect );
    if( GetItemRect( lpListData , index, lpRect, GetWindowLong( hWnd, GWL_STYLE ) ) )
        return 0;
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETLOCALE( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ�������; ���򣬷���LB_ERR
// ����������
//	��֧��
// ����: 
//	
// ************************************************

static LRESULT DoGETLOCALE( HWND hWnd )
{
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETSEL( HWND hWnd, int index )
// ������
//	IN hWnd - ���ھ��
//	IN index - ��Ŀ����
// ����ֵ��
//	����ɹ������ص�ǰ�б��ѡ�����Ŀ; ���򣬷��� LB_ERR
// ����������
//	�õ��б��ѡ�����Ŀ
//	���� LB_GETSEL ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETSEL( HWND hWnd, int index )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );

    if( (DWORD)index < lpListData->count )
    {
        return At( lpListData, index )->state & ODS_SELECTED;
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETSELCOUNT( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ������ص�ǰ�б��ѡ�����Ŀ��; ���򣬷��� LB_ERR
// ����������
//	�õ��б��ѡ�����Ŀ�������ڶ���ѡ��
//	���� LB_GETSELCOUNT ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETSELCOUNT( HWND hWnd )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    int i, n = 0;

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {	//���з��
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
// ������static LRESULT DoGETSELITEMS( HWND hWnd, int maxItems, LPINT lpArray )
// ������
//	IN hWnd - ���ھ��
//	IN maxItems - ��Ҫ���ص������Ŀ��
//	IN lpArray - ���ڽ�����Ŀ�����ŵ�����
// ����ֵ��
//	����ɹ�������ʵ�ʿ�������Ŀ������; ���򣬷��� LB_ERR
// ����������
//	�õ��б���е�ǰ���ڱ�ѡ��״̬����Ŀ��,�������� LBS_MULTIPLESEL �����б��
//	���� LB_GETSELITEMS ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETSELITEMS( HWND hWnd, int maxItems, LPINT lpArray )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    int i, m = 0;

    _ASSERT( lpListData );

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTIPLESEL )
    {	//
        for( i = 0; i < lpListData->count && m < maxItems ; i++ )
        {
            if( At( lpListData, i )->state & ODS_SELECTED )
            {	// ��������
                *lpArray++ = i;
                m++;
            }
        }
        return m;
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETTEXT( HWND hWnd, int index, void * lpBuf )
// ������
//	IN hWnd - ���ھ��
//	IN index - ��Ŀ����
//	OUT lpBuf - ���ڽ�����Ŀ�ı��Ļ���
// ����ֵ��
//	����ɹ�������ʵ�ʿ��������ݳ���; ���򣬷��� LB_ERR
// ����������
//	�õ�ָ����Ŀ�ı�����
//	���� LB_GETTEXT ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETTEXT( HWND hWnd, int index, void * lpBuf )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _LPLISTITEM lpItem;

    _ASSERT( lpListData );
    if( (DWORD)index < lpListData->count )
    {
        lpItem = At( lpListData, index );
		if( (DWORD)GetWindowLong( hWnd, GWL_STYLE ) & LBS_HASSTRINGS )
        {	// ���ַ�������
            strcpy( lpBuf, lpItem->lpszItem );
            return lpItem->typeData;
        }
        else
        {	// �û��Զ������ݣ��������趨��ֵ
			memcpy( lpBuf, &lpItem->dwItemData, 4 );
            return 4;
        }
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoGETTEXTLEN( HWND hWnd, int index )
// ������
//	IN hWnd - ���ھ��
//	IN index - ��Ŀ����
// ����ֵ��
//	����ɹ���������Ŀ�ַ������ݳ���; ���򣬷��� LB_ERR
// ����������
//	�õ�ָ����Ŀ�ַ������ݳ���
//	���� LB_GETTEXTLEN ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETTEXTLEN( HWND hWnd, int index )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
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
// ������static LRESULT DoGETTOPINDEX( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ��������б�򴰿ڵĵ�һ����Ŀ������ֵ; ���򣬷��� LB_ERR
// ����������
//	�õ��б�򴰿ڵĵ�һ����Ŀ������ֵ
//	���� LB_GETTOPINDEX ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGETTOPINDEX( HWND hWnd )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    _ASSERT( lpListData );
    return lpListData->topIndex;
}

// **************************************************
// ������static LRESULT DoRESETCONTENT( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ�������0; ���򣬷��� LB_ERR
// ����������
//	����б���������Ŀ
//	���� LB_RESETCONTENT ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoRESETCONTENT( HWND hWnd )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    DWORD dwStyle = (DWORD)GetWindowLong( hWnd, GWL_STYLE );
    int i, fOwner;

    _ASSERT( lpListData );
	//�Ƿ��Ի���
    fOwner = (int)(dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE));
	//ɾ��ÿһ����Ŀ
    for( i = lpListData->count - 1; i >= 0; i-- )
    {
		DeleteItem( hWnd, dwStyle, lpListData, i );
    }
    lpListData->caret = -1;
    lpListData->curSel = -1;
	lpListData->topIndex = 0;
	lpListData->limit = 0;
	if( !fOwner )
	{	// ���Ի��񣬸��´���
		InvalidateRect( hWnd, NULL, FALSE );
		UpdateWindow( hWnd );
	}
	//2005-01-26, add code
	ReflashScrollBar( hWnd, dwStyle, lpListData );
	//2005-01-26

    return 0;
}

// **************************************************
// ������static LRESULT DoSELECTSTRING( HWND hWnd, int indexStart, LPCSTR lpcstr )
// ������
//	IN hWnd - ���ھ��
//	IN indexStart - ��Ŀ����
//	IN lpcstr - ��Ҫƥ����ַ���
// ����ֵ��
//	����ɹ����������õ���Ŀ������
// ����������
//	���ַ���ƥ�����Ŀ��Ϊ��ѡ����
//	���� LB_SELECTSTRING ��Ϣ
// ����: 
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
// ������static LRESULT DoSELITEMRANGE( HWND hWnd, BOOL fSelect, int first, int last )
// ������
//	IN hWnd - ���ھ��
//	IN fSelect - �Ƿ���Ŀ��Ϊѡ��״̬���������ѡ��״̬
//	IN first - ��ʼ��Ŀ����
//	IN last - ������Ŀ����
// ����ֵ��
//	����ɹ�������0�����򣬷��� LB_ERR
// ����������
//	���� first��ʼ �� last ��������Ŀ��Ϊѡ��״̬���������� LBS_MULTIPLESEL �����б��
//	���� LB_SELITEMRANGE ��Ϣ
// ����: 
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
// ������static LRESULT DoSETANCHORINDEX( HWND hWnd, int index )
// ������
//	IN hWnd - ���ھ��
//	IN index - ��Ŀ����
// ����ֵ��
//	����0
// ����������
//	�����б��� ANCHOR ��
//	���� LB_SETANCHORINDEX ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoSETANCHORINDEX( HWND hWnd, int index )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

    DoSELITEMRANGE( hWnd, FALSE, 0, lpListData->count-1 );
    DoSETSEL( hWnd, TRUE, index );
    return 0;
}

// **************************************************
// ������static LRESULT DoSETCARETINDEX( HWND hWnd, int index, BOOL fScroll )
// ������
//	IN hWnd - ���ھ��
//	IN index - ��Ŀ����
//	IN fScroll - �������ܡ�����ΪFALSE, �򽫹�������Ŀֱ������ȫ���ӣ����򣬾ֲ�����Ҳ��
// ����ֵ��
//	����ɹ�������0�����򣬷��� LB_ERR
// ����������
//	�����б��� CARET ��
//	���� LB_SETCARETINDEX ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoSETCARETINDEX( HWND hWnd, int index, BOOL fScroll )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    RECT rect, clientRect;
    HDC hdc;
    DWORD dwStyle;
	int iRowNum;

    _ASSERT( lpListData );
	//�õ��ͻ�������
    GetClientRect( hWnd, &clientRect );
	//�õ����ڷ��
    dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//�õ������������ɵ�����
	iRowNum = GetRows( lpListData, dwStyle, &clientRect );

    if( index != lpListData->caret &&
		index >= -1 &&
        index < lpListData->count )
    {	// ��Ч����Ҫ��������
		if( index >= 0 ) 
        {   // ������Ŀֱ�������
			rect = clientRect;
			//�õ���Ŀ��������
			GetItemRect( lpListData, index, &rect, dwStyle );
			if( !IntersectRect( &rect, &rect, &clientRect ) )	//������ ��
			{	//������
                if( dwStyle & LBS_MULTICOLUMN )
                {	//����б���Ƕ��з���������
                    ScrollCol( hWnd,
						index / iRowNum -
						lpListData->topIndex / iRowNum );
                }
                else
                {	//���У�������
                    if( index < lpListData->topIndex )
                        ScrollRow( hWnd, (index - lpListData->topIndex) );
                    else
                        ScrollRow( hWnd, (index - (lpListData->topIndex+iRowNum-1) ) );
                }
			}
        }
		// ������ܣ��������� ������ caret
        if( lpListData->caret != index )
        {	//�����ͼDC
            hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
            if( lpListData->caret != -1 )
            {   // �����ϵ�caret, �����  remove old caret
				rect = clientRect;
                GetItemRect( lpListData, lpListData->caret, &rect, dwStyle );
                DrawItemContents ( hWnd, lpListData, dwStyle, hdc, lpListData->caret, &rect, FALSE, ODA_SELECT );
            }
			if( index >= 0 && (dwStyle & (LBS_OWNERDRAWVARIABLE|LBS_OWNERDRAWFIXED)) == 0 )
			{   // ��ʾ�µ� caret show new
				rect = clientRect;
				GetItemRect( lpListData, index, &rect, dwStyle );
				DrawCaret( hdc, &rect );
			}
			//���� caret
            lpListData->caret = index;
			//�ͷŻ�ͼDC
            ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
        }
        return 0;
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoSETCOLUMNWIDTH( HWND hWnd, int cxWidth )
// ������
//	IN hWnd - ���ھ��
//	IN cxWidth - �µ��п�
// ����ֵ��
//	����ɹ�������0�����򣬷��� LB_ERR
// ����������
//	�����б����п�
//	���� LB_SETCOLUMNWIDTH ��Ϣ���������� LBS_MULTICOLUMN �����б��
// ����: 
//	
// ************************************************

static LRESULT DoSETCOLUMNWIDTH( HWND hWnd, int cxWidth )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    RECT rect;

    if( (GetWindowLong( hWnd, GWL_STYLE ) & LBS_MULTICOLUMN) && cxWidth > 0 )
    {	//��Ч
        lpListData->colWidth = cxWidth;
		//�õ����ڵĿͻ���
        GetClientRect( hWnd, &rect );
        //�µ�����
		lpListData->colNum = GetCols( lpListData, &rect );
		//��Ч���ڿͻ���
        InvalidateRect( hWnd, 0, TRUE );
        return 0;
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoSETCOUNT( HWND hWnd, short count )
// ������
//	IN hWnd - ���ھ��
//	IN count - �µ���Ŀ��
// ����ֵ��
//	����ɹ�������0�����򣬼���û���ڴ棬����LB_ERRSPACE, �������󷵻� LB_ERR
// ����������
//	�����б��Ŀ������ɵ���Ŀ��
//	���� LB_SETCOUNT ��Ϣ���������� LBS_NODATA �ͷ� LBS_HASSTRINGS �����б��
// ����: 
//	
// ************************************************

static LRESULT DoSETCOUNT( HWND hWnd, short count )
{	//�õ����ڷ��
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    if( (dwStyle & LBS_NODATA) && (dwStyle & LBS_HASSTRINGS)==0 )
    {	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
        lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
        _ASSERT( lpListData );
        if( count != Count( lpListData ) )
        {	//��Ҫ����
            if( ResetLimit( lpListData, count ) == FALSE )
            {	//ʧ�ܣ��򸸷���LBN_ERRSPACE��Ϣ
                SendNotify( hWnd, LBN_ERRSPACE );
                return LB_ERRSPACE;
            }
        }
        return 0;
    }
    return LB_ERR;
}

// **************************************************
// ������static void ShowSelectIndex( _LPLISTDATA lpListData, 
//					  HWND hWnd, 
//					  int index, 
//					  HDC hdc, 
//					  LPCRECT lpcrect, 
//					  DWORD dwStyle, 
//					  BOOL fShow )

// ������
//	IN lpListData - �б�����ָ��
//	IN hWnd - ���ھ��
//	IN index - ��Ŀ��
//	IN hdc - ��ͼDC
//	IN lpcrect - �б�򴰿ھ���
//	IN dwStyle - �б����
//	IN fShow - �Ƿ����û����ѡ��״̬
// ����ֵ��
//	��
// ����������
//	�����б��Ŀ������ɵ���Ŀ��
//	���� LB_SETCOUNT ��Ϣ���������� LBS_NODATA �ͷ� LBS_HASSTRINGS �����б��
// ����: 
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
	//�õ���Ŀ����
	GetItemRect( lpListData, index, &rect, dwStyle );
	//����Ŀ����
    DrawItemContents ( hWnd, lpListData, dwStyle, hdc, index, &rect, index == lpListData->caret, ODA_SELECT );
}

// **************************************************
// ������static LRESULT DoSETCURSEL( HWND hWnd, int index, BOOL bNotify )
// ������
//	IN hWnd - ���ھ��
//	IN index - �б����Ŀ������
//	iN bNotify - �Ƿ��򸸴��ڷ�֪ͨ��Ϣ
// ����ֵ��
//	����ɹ������ر����õ�����ֵ; ���򣬷���LB_ERR
// ����������
//	��index����Ϊ��ǰ��ѡ����Ŀ�������ڷǶ���ѡ������б��
//	���� LB_SETCURSEL
// ����: 
//	
// ************************************************

static LRESULT DoSETCURSEL( HWND hWnd, int index, BOOL bNotify )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    HDC hdc;
    _LPLISTDATA lpListData;
    RECT rect;
    if( (dwStyle & LBS_MULTIPLESEL)==0 && index >= -1 )
    {	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
        lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
        _ASSERT( lpListData );
		// 2004-10-27, modify, �Ż�
        //if( index < lpListData->count )
		if( index < lpListData->count &&
			index != lpListData->curSel )
		//
        {	//�Ϸ�
			//�õ����ڵĿͻ�������
            GetClientRect( hWnd, &rect );
			//�����ͼDC
            hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
            //������ܣ�������ǰ�� hide cursel if possible
            if( lpListData->curSel != -1 )
                ShowSelectIndex( lpListData, hWnd, lpListData->curSel, hdc, &rect, dwStyle, FALSE );
			//��ʾ�µ�
            if( index >= 0 )
                ShowSelectIndex( lpListData, hWnd, index, hdc, &rect, dwStyle, TRUE );
			//�����µ�ѡ����
            lpListData->curSel = index;
			//�ͷŻ�ͼDC
            ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
		}
        return index;
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoSETHORIZONTALEXTENT( HWND hWnd, short cx )
// ������
//	IN hWnd - ���ھ��
//	IN cx - ˮƽ���
// ����ֵ��
//	����ɹ�������; ���򣬷���LB_ERR
// ����������
//	���� LB_SETHORIZONTALEXTENT
// ����: 
//	
// ************************************************

static LRESULT DoSETHORIZONTALEXTENT( HWND hWnd, short cx )
{
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoSETITEMDATA( HWND hWnd, short index, DWORD dwData )
// ������
//	IN hWnd - ���ھ��
//	IN index - �б����Ŀ������
//	IN dwData - ��Ŀ����
// ����ֵ��
//	����ɹ�������0; ���򣬷���LB_ERR
// ����������
//	������Ŀ����
//	���� LB_SETITEMDATA
// ����: 
//	
// ************************************************

static LRESULT DoSETITEMDATA( HWND hWnd, short index, DWORD dwData )
{	//�õ����ڷ��
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
	lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	_ASSERT( lpListData );
	//if( index < lpListData->count && index >=0 )
	if( (DWORD)index < lpListData->count )
		return SetItemData( lpListData, index, dwData );	//
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoSETITEMHEIGHT( HWND hWnd, short index, short height )
// ������
//	IN hWnd - ���ھ��
//	IN index - �б����Ŀ������
//	IN height - �µ���Ŀ�߶�
// ����ֵ��
//	����ɹ�������0; ���򣬷���LB_ERR
// ����������
//	������Ŀ����ʾ�߶�
//	���� LB_SETITEMHEIGHT
// ����: 
//	
// ************************************************

static LRESULT DoSETITEMHEIGHT( HWND hWnd, short index, short height )
{	//�õ����ڷ��
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    //RECT rect;
	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
    {	//����ǿɱ���,���õ�indexָ�����Ŀ��
       //if( index < lpListData->count && index >=0 )
		if( (DWORD)index < lpListData->count )
           return SetItemTypeData( lpListData, index, (WORD)height );
    }
    else
    {
        if( height > 0 && index == 0 )
        {	//�߶���ͳһ��
            lpListData->rowHeight = height;
            //GetClientRect( hWnd, &rect );
            InvalidateRect( hWnd, 0, TRUE );
            return 0;
        }
    }
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoSELITEMRANGEEX( HWND hWnd, short first, short last )
// ������
//	IN hWnd - ���ھ��
//	IN first - ��ʼ��Ŀ������
//	IN last - ������Ŀ������
// ����ֵ��
//	����ɹ�������0; ���򣬷���LB_ERR
// ����������
//	��ָ������Ŀ��Χ��Ϊѡ��״̬( ���� LBS_MULTIPLESEL �����Ч )
//	���� LB_SELITEMRANGEEX ��Ϣ
// ����: 
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
// ������
// ������
//	IN 
// ����ֵ��
//	����ɹ�������; ���򣬷���0
// ����������
//	
// ����: 
//	
// ************************************************

static LRESULT DoSETLOCALE( HWND hWnd, WORD idLocale )
{
    return LB_ERR;
} 

// **************************************************
// ������static LRESULT DoSETSEL( HWND hWnd, BOOL fSelect, int index )
// ������
//	IN hWnd - ���ھ��
//	IN fSelect - �Ƿ�����Ϊѡ��״̬���
//	IN index - ��Ŀ������
// ����ֵ��
//	����ɹ�������0; ���򣬷���LB_ERR
// ����������
//	��ָ������Ŀ��Ϊѡ��״̬, ���� index = -1, ��ѡ�����л��������.( ���� LBS_MULTIPLESEL �����Ч )
//	���� LB_SETSEL ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoSETSEL( HWND hWnd, BOOL fSelect, int index )
{	//�õ����ڷ��
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    _LPLISTDATA lpListData;
    HDC hdc;
    short i, retv = LB_ERR;
    RECT rect;
	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    if( dwStyle & LBS_MULTIPLESEL )
    {	//���ط��
        //if( index >= 0 && index < lpListData->count )
		if( (DWORD)index < lpListData->count )
        {	//��Ч������ֵ
            retv = SetItemState( lpListData, index, ODS_SELECTED, fSelect );
            SendNotify( hWnd, LBN_SELCHANGE );
			//�����ͼDC
            hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
			//�õ����ڿͻ�����
            GetClientRect( hWnd, &rect );
			//�õ���Ŀ����
			GetItemRect( lpListData, index, &rect, dwStyle );
			//����Ŀ����
            DrawItemContents ( hWnd, lpListData, dwStyle, hdc, index, &rect, lpListData->caret == index, ODA_SELECT );
            //�ͷ�DC
			ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
        }
        else if( index <= -1 )
        {	//��������Ŀ�����û��������
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
// ������static LRESULT DoSETTABSTOPS( HWND hWnd, short num, short * lpArray )
// ������
//	IN hWnd - ���ھ��
//	IN num - 
// ����ֵ��
//	����ɹ�������; ���򣬷���0
// ����������
//	
// ����: 
//	
// ************************************************

static LRESULT DoSETTABSTOPS( HWND hWnd, short num, short * lpArray )
{
    return LB_ERR;
}

// **************************************************
// ������static LRESULT DoSETTOPINDEX( HWND hWnd, int index )
// ������
//	IN hWnd - ���ھ��
//	IN index - ��Ŀ������
// ����ֵ��
//	����ɹ�������0; ���򣬷���LB_ERR
// ����������
//	���õ�ǰ�б�򴰿ڵĵ�һ����Ŀ
//	���� LB_SETTOPINDEX 
// ����: 
//	
// ************************************************

static LRESULT DoSETTOPINDEX( HWND hWnd, int index )
{
    _LPLISTDATA lpListData;
    //�õ��б�򴰿ڵ��б�����ݶ���ָ��
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    _ASSERT( lpListData );
    //if( index >= 0 && index < lpListData->count && index != lpListData->topIndex )
	if( (DWORD)index < lpListData->count && index != lpListData->topIndex )
    {	//���ò���Ч�ͻ���
        lpListData->topIndex = index;
		SetScrollBar( hWnd, lpListData->topIndex, 0 );
        InvalidateRect( hWnd, 0, TRUE );
        return 0;
    }
    return LB_ERR;
}


// **************************************************
// ������static VOID _SetListColor( _LPLISTDATA lpListData )
// ������
//	IN lpListData - ��̬�����ݽṹ
// ����ֵ��
//	��
// ����������
//	���ô�����ʾ��ɫ
// ����: 
//	
// ************************************************

static VOID _SetListColor( _LPLISTDATA lpListData )
{
	//������ʾ��ɫ
	lpListData->cl_Disable = GetSysColor(COLOR_GRAYTEXT);
	lpListData->cl_DisableBk = GetSysColor(COLOR_WINDOW);
	lpListData->cl_Selection = GetSysColor(COLOR_HIGHLIGHTTEXT);
	lpListData->cl_SelectionBk = GetSysColor(COLOR_HIGHLIGHT);
	lpListData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);
	lpListData->cl_TextBk = GetSysColor(COLOR_WINDOW);

}

// **************************************************
// ������static _LPLISTDATA _DoInitListData( HWND hWnd, LPCREATESTRUCT lpcs )
// ������
//	IN hWnd - ���ھ��
//	IN lpcs - ���ڴ����ṹָ��
// ����ֵ��
//	����ɹ������� _LISTDATA�ṹָ��; ���򣬷��� NULL
// ����������
//	���䲢��ʼ�� _LISTDATA�ṹָ��
// ����: 
//	
// ************************************************

static _LPLISTDATA _DoInitListData( HWND hWnd, LPCREATESTRUCT lpcs )
{
    _LPLISTDATA lpListData;
    //TEXTMETRIC tm;
//    HDC hdc;
    RECT rect;
	//����һ���ڴ�ṹ	
	lpListData = (_LPLISTDATA)malloc( sizeof( _LISTDATA ) );

	if( lpListData )
	{
		memset( lpListData, 0, sizeof( _LISTDATA ) );
        ResetLimit( lpListData, 0 );

		lpListData->dwStyle = lpcs->style;
		lpcs->style &= ~(WS_VSCROLL | WS_HSCROLL);

		if( lpcs->style & LBS_OWNERDRAWFIXED )
		{	//ӵ�����Զ�����
			MEASUREITEMSTRUCT mis;
			mis.CtlID = (UINT)lpcs->hMenu;
			mis.CtlType = ODT_LISTBOX;
			mis.itemData = 0;
			mis.itemHeight = 0;
			mis.itemID = 0;
			mis.itemWidth = 0;
			//�븸���ڵõ��и�
			if( SendMessage( lpcs->hParent, WM_MEASUREITEM, mis.CtlID, (LPARAM)&mis ) )
			{
                lpListData->rowHeight = (short)mis.itemHeight;
			}	
		}
		//
		if( lpListData->rowHeight == 0 )
		{	//�õ�ϵͳĬ�ϸ߶�
			//hdc = GetDC( hWnd );
			//GetTextMetrics( hdc, &tm );
			//ReleaseDC( hWnd, hdc );
			//(short)tm.tmHeight + 4;
			lpListData->rowHeight = GetSystemMetrics(SM_CYLISTITEM);//GetSystemMetrics(SM_CYCAPTION);
		}
		//��ʼ����������
        lpListData->colWidth = lpcs->cx;
        lpListData->caret = -1;
        lpListData->curSel = -1;

        lpListData->bCombBoxList= FALSE;
        lpListData->hCombBox = NULL;
		//������ʾ��ɫ
		_SetListColor( lpListData );
		//lpListData->cl_Disable = GetSysColor(COLOR_GRAYTEXT);
		//lpListData->cl_DisableBk = GetSysColor(COLOR_WINDOW);
		//lpListData->cl_Selection = GetSysColor(COLOR_HIGHLIGHTTEXT);
		//lpListData->cl_SelectionBk = GetSysColor(COLOR_HIGHLIGHT);
		//lpListData->cl_Text = GetSysColor(COLOR_WINDOWTEXT);
		//lpListData->cl_TextBk = GetSysColor(COLOR_WINDOW);
		//���ݵ�ǰ��������õ�����ʾ������
        GetClientRect( hWnd, &rect );
        lpListData->colNum = GetCols( lpListData, &rect );

		SetWindowLong( hWnd, 0, (long)lpListData );
		SetWindowLong( hWnd, GWL_STYLE, lpcs->style );
	}
	return lpListData;
}

// **************************************************
// ������static LRESULT DoCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
// ������
//	IN hWnd - ���ھ��
//	IN lpcs - ���ڴ����ṹָ��
// ����ֵ��
//	����ɹ�������0; ���򣬷���-1
// ����������
//	����  WM_CREATE ��Ϣ
// ����: 
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
// ������static LRESULT DoCREATECombBoxList( HWND hWnd, LPCREATESTRUCT lpcs )
// ������
//	IN hWnd - ���ھ��
//	IN lpcs - ���ڴ����ṹָ��
// ����ֵ��
//	����ɹ�������0; ���򣬷���-1
// ����������
//	����  WM_CREATE ��Ϣ
// ����: 
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
// ������static LRESULT DoDestroy( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	���� WM_DESTROY ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoDestroy( HWND hWnd )
{
    _LPLISTDATA lpListData;
    HWND hParent;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    hParent = GetParent( hWnd );
	//�ͷ����е���Ŀ
    while( lpListData->count )
    {
		DeleteItem( hWnd, dwStyle, lpListData, lpListData->count-1 );
    }
    free( lpListData->lpItems );
    free( lpListData );
    return 0;
}

// **************************************************
// ������static int AtRow( _LPLISTDATA lpListData, DWORD dwStyle, int y )
// ������
//	IN lpListData - �б�����ָ�� 
//	IN dwStyle - ���
//	IN y - y����
// ����ֵ��
//	����y���ڵ�����
// ����������
//	�õ�y���ڵ�����
// ����: 
//	
// ************************************************

static int AtRow( _LPLISTDATA lpListData, DWORD dwStyle, int y )
{
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
	{	//�ɱ��и�
		int cy = 0;
		int indexStart; 
		_LPLISTITEM lpItem;
		
		if( lpListData->count )
		{	//����Ŀ
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
	{	//�̶��и�
		if( y >= 0 )
			return y / lpListData->rowHeight;
		else
			return -1 + y / lpListData->rowHeight;
	}
}

// **************************************************
// ������static int AtCol( _LPLISTDATA lpListData, DWORD dwStyle, int x )
// ������
//	IN lpListData - �б�����ָ�� 
//	IN dwStyle - ���ڷ��
//	IN x - x����
// ����ֵ��
//	��������
// ����������
//	�õ� x ���ڵ�����
// ����: 
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
// ������static int GetRows( _LPLISTDATA lpListData, DWORD dwStyle, LPCRECT lpClientRect )
// ������
//	IN lpListData - �б�����ָ�� 
//	IN dwStyle - ���ڷ��
//	IN lpClientRect - �ͻ�����
// ����ֵ��
//	���ؿͻ������������ɵ�����
// ����������
//	�õ��ͻ������������ɵ�����
// ����: 
//	
// ************************************************

static int GetRows( _LPLISTDATA lpListData, DWORD dwStyle, LPCRECT lpClientRect )
{
    if( dwStyle & LBS_OWNERDRAWVARIABLE )
	{	//�ɱ��и�
		int cy = 0;
		int indexStart; 
		_LPLISTITEM lpItem;

		if( lpListData->count )
		{	//����Ŀ
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
	{	//���ɱ�����
		if( lpListData->rowHeight > 0 )
			return lpClientRect->bottom / lpListData->rowHeight;
		else
			return 1;
	}
}

// **************************************************
// ������static int GetCols( _LPLISTDATA lpListData, LPCRECT lpClientRect )
// ������
//	IN lpListData - �б�����ָ�� 
//	IN lpClientRect - �ͻ�����
// ����ֵ��
//	���ؿͻ������������ɵ�����
// ����������
//	�õ��ͻ������������ɵ�����
// ����: 
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
// ������static short GetItemRect( _LPLISTDATA lpListData, int index, LPRECT lpRect, DWORD dwStyle )
// ������
//	IN lpListData - �б�����ָ�� 
//	IN index - ��Ŀ����
//	IN/OUT lpRect - ���봰�ھ���, ������Ŀ����
//	IN dwStyle - ���ڷ��
// ����ֵ��
//	����1
// ����������
//	�õ���Ŀ����
// ����: 
//	
// ************************************************

static short GetItemRect( _LPLISTDATA lpListData, int index, LPRECT lpRect, DWORD dwStyle )
{
    int crows, row, col;

    if( dwStyle & LBS_MULTICOLUMN )
    {	//���з��
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
    {	//����
		if( dwStyle & LBS_OWNERDRAWVARIABLE )
		{	//�ɱ��и�
	        int y = 0;
			int indexStart; 
			_LPLISTITEM lpItem;
			//��topIndex��ʼ����
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
		{	//�̶��и�
			lpRect->top = (index-lpListData->topIndex) * lpListData->rowHeight;
			lpRect->bottom = lpRect->top + lpListData->rowHeight;
		}
        return 1;
    }
}

// **************************************************
// ������static void DrawCaret( HDC hdc, LPCRECT lpRect )
// ������
//	IN hdc - ��ͼDC
//	IN lpRect - ��������
// ����ֵ��
//	��
// ����������
//	���ƽ�����ʾЧ��
// ����: 
//	
// ************************************************

static void DrawCaret( HDC hdc, LPCRECT lpRect )
{
    DrawFocusRect( hdc, lpRect );
}

// **************************************************
// ������static void DrawItemContents ( HWND hWnd, 
//                              _LPLISTDATA lpListData,
//							  DWORD dwStyle, 
//							  HDC hdc, 
//							  int index, 
//							  LPCRECT lpRect, 
//							  BOOL bHasCaret,
//							  UINT uiAction )
//
// ������
//	IN hWnd - ���ھ��
//	IN lpListData - �б�����ָ��
//	IN dwStyle - ���ڷ��
//	IN hdc - DC���
//	IN index - ��Ŀ����
//	IN lpRect - ��Ŀ����
//	IN bHasCaret - ��Ŀ�Ƿ��� Caret ����
//	IN uiAction - ��ǰ�Ķ���
// ����ֵ��
//	��
// ����������
//	������Ŀ
// ����: 
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
		{	//�Ի���
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
			//�򸸷�������Ŀ��Ϣ
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
			//�����ı���ʾ��ɫ
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
			top = lpRect->top + ( (lpRect->bottom - lpRect->top) - tm.tmHeight ) / 2; //��ֱ����
			
			//������Ŀ�ı�
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
// ������static void SendNotify( HWND hWnd, const short code )
// ������
//	IN hWnd - ���ھ��
//	IN code - ֪ͨ����
// ����ֵ��
//	��
// ����������
//	�򴰿ڷ���֪ͨ��Ϣ
// ����: 
//	
// ************************************************

static void SendNotify( HWND hWnd, const short code )
{
    DWORD wParam;
  	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );//�õ��б�򴰿ڵ��б�����ݶ���ָ��

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
// ������static BOOL SetScrollBar( HWND hWnd, int pos, int *lpNewPos )
// ������
//	IN hWnd - ���ھ��  
//	IN pos - ������λ�� 
//	IN lpNewPos - ���ڽ����µ�λ��
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���ù�����λ��
// ����: 
//	
// ************************************************

static BOOL SetScrollBar( HWND hWnd, int pos, int *lpNewPos )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    SCROLLINFO si;
    RECT rect;
	DWORD dwStyle;
    short rows,t, cols;

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	GetClientRect( hWnd, &rect );
    rows = GetRows( lpListData, dwStyle, &rect  );
    if( lpListData->dwStyle & WS_VSCROLL )
    {   //�д�ֱ������
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
    {	//��ˮƽ������
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
// ������static int ScrollCol( HWND hWnd, int code  )
// ������
//	IN hWnd - ���ھ��
//	IN code - ����
// ����ֵ��
//	����0
// ����������
//	������
// ����: 
//	
// ************************************************

static int ScrollCol( HWND hWnd, int code  )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	RECT rect;
	int iRowNum;
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//�õ����ھ���
	GetClientRect( hWnd, &rect );
	//����
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
// ������static int ScrollRow( HWND hWnd, int code  )
// ������
//	IN hWnd - ���ھ��
//	IN code - ����
// ����ֵ��
//	����0
// ����������
//	��������
// ����: 
//	
// ************************************************

static int ScrollRow( HWND hWnd, int code  )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
	_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	RECT rect;
	int iRowNum;
	DWORD dwStyle;
	//�õ����ڷ��
    dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//�õ��ͻ�������
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
// ������static void RedrawItem( _LPLISTDATA lpListData, 
//					   HWND hWnd, 
//					   DWORD dwStyle, 
//					   HDC hdc, 
//					   int iFrom, 
//					   int iTo,
//					   BOOL bEraseBottom
//					    )
// ������
//	IN lpListData - �б�����ָ�� 
//	IN hWnd - ���ھ�� 
//	IN dwStyle - ���ڷ��
//	IN hdc - ͼ��DC
//	IN iFrom - ��ʼ��Ŀ
//	IN iTo - ������Ŀ
//	IN bEraseBottom - �Ƿ�������ڵײ�����
// ����ֵ��
//	��
// ����������
//	������Ŀ
// ����: 
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
	//�õ��ͻ���
	GetClientRect( hWnd, &rcClient );
	if( iFrom < lpListData->topIndex )
		iFrom = lpListData->topIndex;
	yBottom = rcClient.top;
	if( lpListData->hFont )
	{
		hOldFont = SelectObject( hdc, lpListData->hFont );
	}
	//����ÿһ����Ŀ
	for( i = iFrom; i < iTo; i++ )
    {
		rect = rcClient;
		//�õ���Ŀ��ʾ����
		GetItemRect( lpListData, i, &rect, dwStyle );
		yBottom = rect.bottom;
		//�õ��ü���
        if( IntersectRect( &rcClip, &rect, &rcClient ) )
		    DrawItemContents( hWnd, lpListData, dwStyle, hdc, i, &rect, lpListData->caret == i, ODA_DRAWENTIRE );
        else
		{
            bEraseBottom = FALSE;
			break;
		}
    }
	if( bEraseBottom )
	{	//������±���
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
// ������static LRESULT DoPAINT( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	���ƿͻ���
//	���� WM_PAINT ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoPAINT( HWND hWnd )
{
	_LPLISTDATA lpData;
	HDC hdc;
	PAINTSTRUCT ps;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
	lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	//�õ���ͼDC
	hdc = GetListDC( hWnd, &ps, lpData );//BeginPaint( hWnd, &ps );
	
	RedrawItem( lpData, hWnd, dwStyle, hdc, lpData->topIndex, lpData->count, TRUE );
    //�ͷ�DC
	//EndPaint( hWnd, &ps );
	ReleaseListDC( hWnd, hdc, &ps );
    return 0;
}

// **************************************************
// ������static LRESULT DoLBUTTONDOWN( HWND hWnd, short x, short y, BOOL bNotify )
// ������
//	IN hWnd - ���ھ��
//	IN x - ���x����
//	IN y - ���y����
//	IN bNotify - �Ƿ�֪ͨ��Ϣ��������
// ����ֵ��
//	����0
// ����������
//	���� WM_LBUTTONDOWN ��Ϣ
// ����: 
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
	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	_ASSERT( lpData );
	//�õ����ڿͻ���
	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpData, dwStyle, &rect );

    col = AtCol( lpData, dwStyle, x );
    row = AtRow( lpData, dwStyle, y );
    i = col * iRowNum + row;
    h = lpData->topIndex + i;
    if( h >= 0 )
    {
		// ������ܣ�����֪ͨ��Ϣ send notify message
		if(  dwStyle & LBS_NOTIFY )
		{	
			LBNOTIFY lbNotify, * plbNotify;
			DWORD dwParam;

			plbNotify = &lbNotify;
#ifdef _MAPPOINTER
		    plbNotify = (LBNOTIFY*)MapPtrToProcess( plbNotify, GetCurrentProcess() );
#endif
			//��ʼ��֪ͨ�ṹ����
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
		{	//�ı䵱ǰѡ���� ����֪ͨ��Ϣ
            if( bNotify )
		        SendNotify( hWnd, LBN_SELCHANGE );
            DoSETCURSEL( hWnd, h, bNotify );
            DoSETCARETINDEX( hWnd, h, TRUE );
		}
    }
	//ץס����¼�/��Ϣ
    //SetCapture( hWnd ); //2004-12-21,ln, ��Ϊ�п����� ��LB_CLICK ������������棬�Ƶ� MOUSEMOVE
	lpData->bDown = 1;
    return 0;
}

// **************************************************
// ������static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y, BOOL bNotify )
// ������
//	IN hWnd - ���ھ��
//	IN x - ���x����
//	IN y - ���y����
//	IN bNotify - �Ƿ�֪ͨ��Ϣ��������
// ����ֵ��
//	����0
// ����������
//	���� WM_LBUTTONDOWN ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y, BOOL bNotify )
{
	_LPLISTDATA lpData;

	lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    //if( GetCapture() == hWnd )
	if( lpData->bDown == 1 )
    {   //  ComboBox ��Ҫ������� Notify.
        if( bNotify )
		    SendNotify( hWnd, LBN_SELCHANGE );
        //ReleaseCapture();
		lpData->bDown = 0;
    }
	
    return 0;
}

// **************************************************
// ������static LRESULT DoMOUSEMOVE( HWND hWnd, WPARAM wParam, short x, short y, BOOL bNotify )
// ������
//	IN hWnd - ���ھ��
//  IN wParam - WM_MOUSEMOVE ��Ϣ�� wParam ��Ϣ
//	IN x - ���x����
//	IN y - ���y����
//	IN bNotify - �Ƿ�֪ͨ��Ϣ��������
// ����ֵ��
//	����0
// ����������
//	���� WM_MOUSEMOVE ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoMOUSEMOVE( HWND hWnd, WPARAM wParam, short x, short y, BOOL bNotify )
{
    int pos, iRowNum;
    _LPLISTDATA lpData;
	RECT rect;
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );	//�õ����ڷ��

	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    lpData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
	//�õ����ڿͻ�������
	GetClientRect( hWnd, &rect );
	iRowNum = GetRows( lpData, dwStyle, &rect );
	//�Ƿ�����ƶ�ʱ��������ť
    if( (wParam & MK_LBUTTON) )
    {	//����ƶ�ʱ��������ť
		//�õ���ǰ������ڵ���Ŀ
        pos = (short)DoGETTOPINDEX( hWnd ) + AtRow( lpData, dwStyle, y );
        if( (dwStyle & LBS_MULTICOLUMN) )
        {	//�б��Ϊ���з��
            pos += AtCol( lpData, dwStyle, x ) * iRowNum;
        }
        if( pos >= 0 )                                   // modify 2000-05-18 by ln
        {	//
			if( dwStyle & LBS_MULTIPLESEL )
				return DoSETCARETINDEX( hWnd, pos, TRUE );
			else
			{	//����ѡ����
				DoSETCURSEL( hWnd, pos, bNotify );
				return DoSETCARETINDEX( hWnd, pos, TRUE );
			}
        }
    }
    return 0;
}

// **************************************************
// ������static LRESULT DoLBUTTONDBLCLK( HWND hWnd )
// ������
//	IN hWnd - ���ڷ��
// ����ֵ��
//	����0
// ����������
//	���� WM_LBUTTONDBLCLK ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoLBUTTONDBLCLK( HWND hWnd )
{
    WPARAM wParam;

    if( GetWindowLong( hWnd, GWL_STYLE ) & LBS_NOTIFY )
    {	//��Ҫ����֪ͨ��Ϣ
        wParam = GetWindowLong( hWnd, GWL_ID );
        wParam = MAKELONG( wParam, LBN_DBLCLK );
        PostMessage( GetParent( hWnd ), WM_COMMAND, wParam, (LPARAM)hWnd );
    }
    return 0;
}

// **************************************************
// ������static LRESULT DoCHAR( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
//	IN hWnd - ���ھ��
//	IN wParam - WM_CHAR ��Ϣ��wParam����
//	IN lParam - WM_CHAR ��Ϣ��lParam����
// ����ֵ��
//	����0
// ����������
//	���� WM_CHAR ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoCHAR( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lrs = -1;
	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
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
// ������static LRESULT DoKEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
//	IN hWnd - ���ھ��
//	IN wParam - WM_KEYDOWN ��Ϣ��wParam����
//	IN lParam - WM_KEYDOWN ��Ϣ��lParam����
// ����ֵ��
//	����0
// ����������
//	���� WM_KEYDOWN ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoKEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lrs = -1;
	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
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
        {	//ͬ�е�����Ŀ�ı䣬�����µ�ѡ����
			if(  lpListData->curSel + rows < 0 )
				DoSETCURSEL( hWnd, 0, TRUE );
			else
                DoSETCURSEL( hWnd, lpListData->curSel + rows, TRUE );
            DoSETCARETINDEX( hWnd, lpListData->curSel, TRUE );
        }
        else if( cols )
            ScrollCol( hWnd, cols );//ͬ�е�����Ŀ�ı䣬�����µ�ѡ����
    }
    else if( lrs >= 0 )  // lrs is index of item, do default action at index of lrs
    {
    }
    return 0;
}

// **************************************************
// ������static LRESULT DoSetRedraw( HWND hWnd, BOOL bRedraw )
// ������
//	IN hWnd - ���ھ��
//	IN bRedraw - �Ƿ������ػ��־
// ����ֵ��
//	����0
// ����������
//	����Ŀ�ı䣨������ɾ���ȣ��Ƿ�ӻ�ı�
//	���� WM_SETREDRAW ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoSetRedraw( HWND hWnd, BOOL bRedraw )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    //	�õ����ڷ��
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
// ������static LRESULT DoHSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
// ������
//	IN hWnd - ���ڷ��
//	IN code - ��������
//	IN pos - ������
//	IN hwndCtrl - ΪNULL
// ����ֵ��
//	����0
// ����������
//	ˮƽ����
//	���� WM_HSCROLL ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoHSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    RECT rect;
    int cols, i;
	int iRowNum;
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//�õ����ڷ��
	GetClientRect( hWnd, &rect );
	//�õ��ͻ�������ʾ������
	iRowNum = GetRows( lpListData, dwStyle, &rect );
	//�õ��ͻ�������ʾ������
    cols = lpListData->count / iRowNum;
    i = lpListData->topIndex / iRowNum;

    switch( code )
    {
    case SB_BOTTOM:		//����
        ScrollCol( hWnd, cols - i );
        break;
    case SB_ENDSCROLL:	//
        break;
    case SB_LINEDOWN:	//��һ��
        ScrollCol( hWnd, 1 );
        break;
    case SB_LINEUP:		//��һ��
        ScrollCol( hWnd, -1 );
        break;
    case SB_PAGEDOWN:	//��һҳ
        ScrollCol( hWnd, lpListData->colNum );
        break;
    case SB_PAGEUP:		//��һҳ
        ScrollCol( hWnd, (short)-lpListData->colNum );
        break;
    case SB_THUMBPOSITION:		//
        break;
    case SB_THUMBTRACK:
        ScrollCol( hWnd, pos - i );
        break;
    case SB_TOP:		//����
        ScrollCol( hWnd, -i );
        break;
    }
    return 0;
}

// **************************************************
// ������static LRESULT DoHSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
// ������
//	IN hWnd - ���ڷ��
//	IN code - ��������
//	IN pos - ������
//	IN hwndCtrl - ΪNULL
// ����ֵ��
//	����0
// ����������
//	��ֱ����
//	���� WM_VSCROLL ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoVSCROLL( HWND hWnd, short code, short pos, HWND hwndCtrl )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    RECT rect;
    int rows, i;
	//�õ����ڷ��
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//�õ��ͻ�������
    GetClientRect( hWnd, &rect );
	//�õ��ͻ�������ʾ������
	rows = GetRows( lpListData, dwStyle, &rect );

    switch( code )
    {
    case SB_BOTTOM:			//����
        ScrollRow( hWnd, lpListData->count );
        break;
    case SB_ENDSCROLL:
        break;
    case SB_LINEDOWN:		//��һ��
        ScrollRow( hWnd, 1 );
        break;
    case SB_LINEUP:			//��һ��
        ScrollRow( hWnd, -1 );
        break;
    case SB_PAGEDOWN:		//��һҳ
        ScrollRow( hWnd, rows );
        break;	
    case SB_PAGEUP:			//��һҳ
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
    case SB_TOP:			//����
        i = (short)DoGETTOPINDEX( hWnd );
        ScrollRow( hWnd, -i );
        break;
    }
    return 0;
}

// **************************************************
// ������static LRESULT DoNCCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
// ������
//	IN hWnd - ���ھ��
//	IN lpcs - �����ṹ
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���� WM_NCCREATE ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoNCCREATE( HWND hWnd, LPCREATESTRUCT lpcs )
{
    if( ( lpcs->style & LBS_MULTICOLUMN ) &&
        ( lpcs->style & WS_VSCROLL ) )
    {	//�����и÷��
        lpcs->style &= ~WS_VSCROLL;
    }
    else if( ( lpcs->style & LBS_MULTICOLUMN ) == 0 &&
             ( lpcs->style & WS_HSCROLL ) )
    {	//�����и÷��
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
// ������static LRESULT WINAPI DoKILLFOCUS( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	��ʧȥ����ʱ����ù���
//	���� WM_KILLFOCUS ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT WINAPI DoKILLFOCUS( HWND hWnd )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
    HDC hdc;
    RECT rect;
	DWORD dwStyle;

    if( lpListData->caret != - 1 )
    {	//�õ���ͼDC
        hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
		//�õ����ڿͻ�������
        GetClientRect( hWnd, &rect );
		//�õ���Ŀ����
        GetItemRect( lpListData, lpListData->caret, &rect, GetWindowLong( hWnd, GWL_STYLE ) );
		//�õ����ڷ��
		dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		//����Ŀ
        DrawItemContents ( hWnd, lpListData, dwStyle, hdc, lpListData->caret, &rect, FALSE, ODA_FOCUS );
        lpListData->caret = -1;
		//�ͷ�DC
        ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
    }
	//֪ͨ������
    SendNotify( hWnd, LBN_KILLFOCUS );
    
    return 0;
}


// **************************************************
// ������static LRESULT WINAPI DoSETFOCUS( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	��ʧȥ����ʱ����ù���
//	���� WM_KILLFOCUS ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT WINAPI DoSETFOCUS( HWND hWnd )
{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
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
    {	//�õ���ͼDC
		
        hdc = GetListDC( hWnd, NULL, lpListData );//GetDC( hWnd );
		//�õ����ڿͻ�������
        GetClientRect( hWnd, &rect );
		//�õ���Ŀ����
        GetItemRect( lpListData, lpListData->caret, &rect, GetWindowLong( hWnd, GWL_STYLE ) );
		//�õ����ڷ��
		dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		//����Ŀ
        DrawItemContents ( hWnd, lpListData, dwStyle, hdc, lpListData->caret, &rect, TRUE, ODA_FOCUS );
        //lpListData->caret = -1;
		//�ͷ�DC
        ReleaseListDC( hWnd, hdc, NULL );//ReleaseDC( hWnd, hdc );
    }
	*/
	//֪ͨ������
    SendNotify( hWnd, LBN_SETFOCUS );
    
    return 0;
}

// **************************************************
// ������static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// ������
//	IN hWnd - ���ھ��
//	IN lpccs - �ؼ��ṹ��������ɫֵ
// ����ֵ��
//	�ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���ÿؼ���ɫ,���� WM_SETCTLCOLOR ��Ϣ
// ����: 
//	
// ************************************************


static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
	    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
		if( lpccs->fMask & CLF_TEXTCOLOR )
		{
			lpListData->cl_Text = lpccs->cl_Text;	//ͨ���ı���ɫ
		}
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		{
			lpListData->cl_TextBk = lpccs->cl_TextBk;	//ͨ���ı�������ɫ
		}
		if( lpccs->fMask & CLF_SELECTIONCOLOR )
		{
			lpListData->cl_Selection = lpccs->cl_Selection;	//ѡ�����ı���ɫ
		}
		if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
		{
			lpListData->cl_SelectionBk = lpccs->cl_SelectionBk;	//ѡ�������ɫ
		}
		if( lpccs->fMask & CLF_DISABLECOLOR )
		{
			lpListData->cl_Disable = lpccs->cl_Disable;	//��Ч�ı���ɫ
		}
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
		{
			lpListData->cl_DisableBk = lpccs->cl_DisableBk;	//��Ч������ɫ
		}

		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// ������
//	IN hWnd - ���ھ��
//	IN lpccs - �ؼ��ṹ��������ɫֵ
// ����ֵ��
//	�ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�õ��ؼ���ɫ������ WM_GETCTLCOLOR ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{	//�õ��б�򴰿ڵ��б�����ݶ���ָ��
		_LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );
		if( lpccs->fMask & CLF_TEXTCOLOR )
		{
			lpccs->cl_Text = lpListData->cl_Text;	//ͨ���ı���ɫ
		}
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		{
			lpccs->cl_TextBk = lpListData->cl_TextBk;//ͨ���ı�������ɫ
		}
		if( lpccs->fMask & CLF_SELECTIONCOLOR )
		{
			lpccs->cl_Selection = lpListData->cl_Selection;//ѡ�����ı���ɫ
		}
		if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
		{
			lpccs->cl_SelectionBk = lpListData->cl_SelectionBk;//ѡ�������ɫ
		}
		if( lpccs->fMask & CLF_DISABLECOLOR )
		{
			lpccs->cl_Disable = lpListData->cl_Disable;	//��Ч�ı���ɫ
		}
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
		{
			lpccs->cl_DisableBk = lpListData->cl_DisableBk;//��Ч������ɫ
		}
		
		return TRUE;
	}
	return FALSE;	
}

// **************************************************
// ������static LRESULT DoSysColorChange( HWND hWnd )
// ������
// 	IN hWnd - ���ھ��
// ����ֵ��
//	���� 0
// ����������
//	����ϵͳ��ɫ�ı�
// ����: 
// ************************************************

static LRESULT DoSysColorChange( HWND hWnd )
{
    _LPLISTDATA lpListData = (_LPLISTDATA)GetWindowLong( hWnd, 0 );

	_SetListColor( lpListData );
	return 0;
}

// **************************************************
// ������static LRESULT DoFontChange( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
// 	IN hWnd - ���ھ��
// 	IN wParam - WPARAM ����(������)
// 	IN lParam - LPARAM ����
// ����ֵ��
//	���� 0
// ����������
//	��������ı�
// ����: 
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
// ������static LRESULT WINAPI ListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// ������
//  IN hWnd - ���ھ��
//	IN message - ��Ϣ
//	IN wParam - ��һ����Ϣ
//	IN lParam - �ڶ�����Ϣ
// ����ֵ��
//	�����ھ������Ϣ
// ����������
//	��ť���ڴ��������
// ����: 
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
// ������static LRESULT WINAPI CombBoxListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// ������
//  IN hWnd - ���ھ��
//	IN message - ��Ϣ
//	IN wParam - ��һ����Ϣ
//	IN lParam - �ڶ�����Ϣ
// ����ֵ��
//	�����ھ������Ϣ
// ����������
//	��ť���ڴ��������
// ����: 
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
// ������static LRESULT WINAPI CombBoxListBoxWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// ������
//  IN hWnd - ���ھ��
//	IN message - ��Ϣ
//	IN wParam - ��һ����Ϣ
//	IN lParam - �ڶ�����Ϣ
// ����ֵ��
//	�����ھ������Ϣ
// ����������
//	��ť���ڴ��������
// ����: 
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
	// ��Ҫ��ʾ�������㲻��Ҫ������Ϣ��������Ϣ����ϵͳ����Ĭ�ϴ�����ȥ����
	// 

        return DefWindowProc( hWnd, message, wParam, lParam );
    }
    return 0;
}
*/

