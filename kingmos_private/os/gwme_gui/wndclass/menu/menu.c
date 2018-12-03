/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����menu class
�汾�ţ�1.0.0
����ʱ�ڣ�2000-04-07
���ߣ�����
�޸ļ�¼��
    5. LN, 2004-05-19, �� DEFAULTHEIGHT ��Ϊ GetSystemMetrics( SM_CYMENU )
    4. LN, 2003-08-27, add map
	3. Jami chen  2003.07.04 ����MENU�Ĵ�С����ʾ
    2. LN, 2003-06-17, ����TPM_NONOTIFY & TPM_RETURNCMD & 
	   TPM_RIGHTALIGN & TPM_CENTERALIGN & TPM_BOTTOMALIGN &
	   TPM_VCENTERALIGN
    1.LN 2003-05-06,�Ի湦��ͳ��widthȥ��2*16+4������
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <eassert.h>
#include <emenu.h>
#include <ealloc.h>
//#include <epos.h>

//#include <gwmesrv.h>

//#define _MAPPOINTER

#define MFS_UPDATE_OWNER_DRAW         0x80000000L
#define MA_RETURN    1
#define MA_SELECTED  2
#define IS_STRING( type ) ( !( (MFT_BITMAP | MFT_OWNERDRAW | MFT_SEPARATOR) & (type) ) )

//#define MENUBACKCOLOR		RGB(237,235,237)
//#define MENUTEXTCOLOR		RGB(78,81,78)
#define MENULIGHTSHADE		RGB(244,244,244)
#define MENULIGHT1SHADE		RGB(235,235,235)
#define MENUDARK1SHADE		RGB(200,200,200)
#define MENUDARKSHADE		RGB(150,150,150)
//#define MENUSELTEXTCOLOR	RGB(255,255,255)
//#define MENUSELBACKCOLOR	RGB(77,166,255)

#define EDGEWIDTH  2
#define VERTBANK   2
#define HORZBANK   0

//#define DEFAULTHEIGHT  18

// ����˵���Ŀ�ṹ
// private data struct
typedef struct __MENUITEMINFO {
    DWORD    fType;		// �˵���Ŀ������
    DWORD    fState;	// �˵���Ŀ��״̬
    DWORD    wID;		// �˵���Ŀ��ID
    HMENU    hSubMenu;	// ����еĻ����ò˵���Ŀ���Ӳ˵�
    HBITMAP  hbmpChecked;	// ����еĻ������˵���Ŀ��ѡȡʱ����ʾ��λͼ
    HBITMAP  hbmpUnchecked;	// ����еĻ������˵���Ŀû�б�ѡȡʱ����ʾ��λͼ
    DWORD    dwItemData;	// �˵���Ŀ����
    LPTSTR   dwTypeData;	// �����ͣ�fType������������
    DWORD    cch;			// ����˵���Ŀ��������Ϊ�ַ�����ָ���䳤��
	UINT     iItemHeight;		// ��Ŀ��ʾ�߶�
	struct __MENUDATA * lpOwnerMenu;	// �˵���Ŀ��ӵ����
    struct __MENUITEMINFO * lpNext;		// ��һ���˵���Ŀ
} _MENUITEM, FAR *_LPMENUITEM;

// �˵���ʼ���ṹ
typedef struct _MENUPARAM
{
    HMENU hMenu;		// �˵����		
    HWND hOwner;		// �˵�ӵ����
	UINT uiFlag;		// ��������
}MENUPARAM;

// �˵��ṹ
typedef struct __MENUDATA {
    UINT objType;            // �������� = must OBJ_MENU
	_LPMENUITEM lpDeftItem;	// ��ǰ�˵�Ĭ�ϵ���Ŀ
	_LPMENUITEM lpLastItem;	// ��ǰ�˵���Ŀ�����һ����Ŀ
	UINT iItemWidth;		// �˵���Ŀ�߶�
	UINT nCount;			// �˵���Ŀ��
    UINT active;			// ��ǰ��Ŀ�״��

	COLORREF cl_Text;		// �˵������ı�ǰ����ɫ
	COLORREF cl_TextBk;     // �˵������ı����뱳��ɫ
	COLORREF cl_Selection;	// ѡ���ı���ǰ��ɫ
	COLORREF cl_SelectionBk;  // ѡ���ı��ı���ɫ
	COLORREF cl_Disable;		//��Ч�ı���ǰ��ɫ
	COLORREF cl_DisableBk;    // ��Ч�ı��ı���ɫ

	//HANDLE hOwnerProcess;		// �˵���ӵ���߽���
	_LPMENUITEM lpOwnerItem;	// �˵�ӵ�еĵ�һ����Ŀ
	struct __MENUDATA * lpNext;	// ͬ���˵�����
}_MENUDATA, FAR *_LPMENUDATA;

static _LPMENUDATA _GetHMENUPtr( HMENU hMenu );
static _LPMENUITEM _RemoveItem( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags );
static _LPMENUITEM _At( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags, _LPMENUITEM * lppPrev );
static int _IndexOf( _LPMENUDATA lpMenu, _LPMENUITEM lpItem );
static int _Count( _LPMENUDATA lpMenu );
static _LPMENUITEM _AllocItem(void);
static void _SetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii );
static void _GetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii );
static LRESULT CALLBACK MenuWndProc( HWND, UINT, WPARAM, LPARAM );
static void CalcMenuRect( HWND hwndOwner, HMENU hMenu, LPRECT lpRect, UINT uiFlags );
static WORD DoMenuModalLoop( HWND hwnd, _LPMENUDATA lpMenuData, UINT uiFlags );

static const char strClassMenu[] = "MENU";	// �˵�����
static _LPMENUDATA lpMenuList;	// ���в˵���Դ����



BOOL WINAPI Menu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags );
BOOL WINAPI Menu_InsertItem( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii );

// **************************************************
// ������static _LPMENUDATA _GetHMENUPtr( HMENU hMenu )
// ������
//	IN hMenu - �˵����
// ����ֵ��
//	���ؾ����Ӧ�Ĳ˵�����ָ��
// ����������
//	���˵����ݽṹ����Ч�ԣ�������Ч��������ָ�룻���򣬷���NULL
// ����: 
//	
// ************************************************
static _LPMENUDATA _GetHMENUPtr( HMENU hMenu )
{
    ASSERT( hMenu );
    if( hMenu )
    {
        ASSERT( ((_LPMENUDATA)hMenu)->objType == OBJ_MENU );
        if( ((_LPMENUDATA)hMenu)->objType == OBJ_MENU )
            return (_LPMENUDATA)hMenu;	// ��Ч
    }
	SetLastError( ERROR_INVALID_PARAMETER );
    return NULL;
}

// **************************************************
// ������static VOID _SetMenuColor( _LPMENUDATA lpMenuData )
// ������
//	IN lpMenuData - �˵����ݽṹ
// ����ֵ��
//	��
// ����������
//	���ò˵�����
// ����: 
//	
// ************************************************

static VOID _SetMenuColor(_LPMENUDATA lpMenuData )
{
	lpMenuData->cl_Disable = GetSysColor(COLOR_GRAYTEXT);
	lpMenuData->cl_DisableBk = GetSysColor(COLOR_MENU);
	lpMenuData->cl_Selection = GetSysColor( COLOR_HIGHLIGHTTEXT );//MENUSELTEXTCOLOR;
	lpMenuData->cl_SelectionBk = GetSysColor( COLOR_HIGHLIGHT );
	lpMenuData->cl_Text = GetSysColor(COLOR_MENUTEXT);// MENUTEXTCOLOR;
	lpMenuData->cl_TextBk = GetSysColor(COLOR_MENU);//MENUBACKCOLOR;
}

// **************************************************
// ������static _LPMENUDATA _AllocMenuData( HANDLE hOwnerProcess )
// ������
//	IN hOwnerProcess - �˵���ӵ����
// ����ֵ��
//	����ɹ������ز˵��ṹָ�룻���򣬷���NULL
// ����������
//	���䲢��ʼ���˵�����ṹ
// ����: 
//	
// ************************************************

static _LPMENUDATA _AllocMenuData( VOID )// HANDLE hOwnerProcess )
{
    _LPMENUDATA lpMenuData = malloc( sizeof( _MENUDATA ) ); // ����ṹ

    if( lpMenuData )
    {	// ��ʼ���ṹ
  	    memset( lpMenuData, 0, sizeof( _MENUDATA ) );
        lpMenuData->objType = OBJ_MENU;
		_SetMenuColor( lpMenuData );


		//lpMenuData->cl_Disable = GetSysColor(COLOR_GRAYTEXT);
		//lpMenuData->cl_DisableBk = GetSysColor(COLOR_MENU);
		//lpMenuData->cl_Selection = GetSysColor( COLOR_HIGHLIGHTTEXT );//MENUSELTEXTCOLOR;
		//lpMenuData->cl_SelectionBk = GetSysColor( COLOR_HIGHLIGHT );
		//lpMenuData->cl_Text = GetSysColor(COLOR_MENUTEXT);// MENUTEXTCOLOR;
		//lpMenuData->cl_TextBk = GetSysColor(COLOR_MENU);//MENUBACKCOLOR;

		//lpMenuData->hOwnerProcess = hOwnerProcess;
		// �������� insert to list
		lpMenuData->lpNext = lpMenuList;
		lpMenuList = lpMenuData;
		// 
    }
    return lpMenuData;
}

// **************************************************
// ������static BOOL _FreeMenuData( _LPMENUDATA lpMenuData )
// ������
//	IN lpMenuData - �˵��ṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	��_AllocMenuData�෴���ͷŲ˵��ṹ
// ����: 
//	
// ************************************************

static BOOL _FreeMenuData( _LPMENUDATA lpMenuData )
{
	_LPMENUDATA lpPrev, lpMenu = lpMenuList;

    lpPrev = NULL;
	while( lpMenu )
	{
		if( lpMenu == lpMenuData )
		{   // �Ƴ� remove from list
			if( lpPrev )
				lpPrev->lpNext = lpMenu->lpNext;
			else
				lpMenuList = lpMenu->lpNext;
			break;
		}
		lpPrev = lpMenu;
		lpMenu = lpMenu->lpNext;
	}	
	ASSERT( lpMenu );

	lpMenuData->objType = OBJ_NULL;	

    free( lpMenuData );  // �ͷ�
    return TRUE;
}

// **************************************************
// ������static _LPMENUITEM _AllocItem(void)
// ������
//	��
// ����ֵ��
//	����ɹ���������Ч�Ĳ˵���Ŀָ�룻���򣬷���NULL
// ����������
//	���䲢��ʼ���˵���Ŀ
// ����: 
//	
// ************************************************

static _LPMENUITEM _AllocItem(void)
{
    _LPMENUITEM lpItem;
    lpItem = malloc( sizeof( _MENUITEM ) );
    if( lpItem )
        memset( lpItem, 0, sizeof( _MENUITEM ) );
    return lpItem;
}

// **************************************************
// ������static void _GetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii )
// ������
//	IN lpItem - �˵���Ŀ
//	IN/OUT lpmii - ���ڽ��ܲ˵���Ϣ
// ����ֵ��
//	��
// ����������
//	�õ��˵���Ŀ��Ϣ
// ����: 
//	
// ************************************************

static void _GetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii )
{
	  ASSERT( lpItem && lpmii );

	  if( lpmii->fMask & MIIM_TYPE )
	  {	  // �õ���������
	      lpmii->fType = lpItem->fType;

		  if( IS_STRING( lpItem->fType ) )
		  {   // �ַ�������
#ifdef _MAPPOINTER
			  strncpy( MapPtrToProcess( (LPTSTR)lpmii->dwTypeData, GetCallerProcess() ),
				       (LPTSTR)lpItem->dwTypeData,
					   lpmii->cch );
#else
			  strncpy( (LPTSTR)lpmii->dwTypeData,
				       (LPTSTR)lpItem->dwTypeData,
					   lpmii->cch );
#endif
		  }	  //
		  else
		  {
	          lpmii->dwTypeData = 0;
		  }
	  }
	  if( lpmii->fState & MIIM_STATE )
	      lpmii->fState = lpItem->fState;	// �õ�״̬
	  if( lpmii->fMask & MIIM_ID )
	      lpmii->wID = lpItem->wID;		// �õ�ID
	  if( lpmii->fMask & MIIM_SUBMENU )	
	      lpmii->hSubMenu = lpItem->hSubMenu;	// 
	  if( lpmii->fMask & MIIM_DATA )
	      lpmii->dwItemData = lpItem->dwItemData;	//
	  if( lpmii->fMask & MIIM_CHECKMARKS )
	  {
	      lpmii->hbmpChecked = lpItem->hbmpChecked;
		  lpmii->hbmpUnchecked = lpItem->hbmpUnchecked;
	  }
}

// **************************************************
// ������static void _SetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii )
// ������
//	OUT lpItem - ���������õĲ˵�����
//	IN lpmii - ������Ҫ���õĲ˵�����
// ����ֵ��
//	��
// ����������
//	�Բ˵���Ŀ�����µ�����
// ����: 
//	
// ************************************************

static void _SetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii )
{
	  ASSERT( lpItem && lpmii );

	  if( lpmii->fMask & MIIM_TYPE )
	  {
	      lpItem->fType = lpmii->fType;
          
		  lpItem->cch = lpmii->cch;
		  if( IS_STRING( lpItem->fType ) )
		  {   // �����û��ַ���ָ�룬��Ҫ����ӳ��
#ifdef _MAPPOINTER
			  char * lpsz = MapPtrToProcess( lpmii->dwTypeData, GetCallerProcess() );
#else
			  char * lpsz = (char*)lpmii->dwTypeData;
#endif
	          // ���֮ǰ�Ѿ��з�����ַ��������ͷ���
			  if( lpItem->dwTypeData )
				  free( lpItem->dwTypeData );
			  lpItem->dwTypeData = malloc( lpmii->cch + 1 );	// �����µ�
			  strncpy( lpItem->dwTypeData, lpsz, lpmii->cch );	// ��������
			  *( (LPTSTR)lpItem->dwTypeData + lpmii->cch ) = 0;	//���ý�����
		  }
		  else
		  {
			  lpItem->dwTypeData = 0;	// ��������
			  lpItem->dwItemData = lpmii->dwItemData;	// 
		  }
	  }
	  if( lpmii->fMask & MIIM_STATE )
	      lpItem->fState = lpmii->fState;
	  if( lpmii->fMask & MIIM_ID )
	      lpItem->wID = lpmii->wID;
	  if( lpmii->fMask & MIIM_SUBMENU )
	  {		// ��Ҫ���������Ӳ˵�
		  _LPMENUDATA lpSubMenu = _GetHMENUPtr( lpmii->hSubMenu );		  
			// ������ܣ����Ƴ���
		  if( lpSubMenu )
		  {
			  ASSERT( lpSubMenu->lpOwnerItem == NULL );
			  if( lpItem->hSubMenu )
			  {   // remove it
				  _LPMENUDATA lpsm = _GetHMENUPtr( lpItem->hSubMenu );
				  if( lpsm )
					  lpsm->lpOwnerItem = NULL;
			  }
	          lpItem->hSubMenu = lpmii->hSubMenu;
			  lpSubMenu->lpOwnerItem = lpItem;	// ����ӵ����
		  }
	  }
	  if( lpmii->fMask & MIIM_DATA )
	      lpItem->dwItemData = lpmii->dwItemData;
	  if( lpmii->fMask & MIIM_CHECKMARKS )
	  {
		    lpItem->hbmpChecked = lpmii->hbmpChecked;
		    lpItem->hbmpUnchecked = lpmii->hbmpUnchecked;
	  }
}

// **************************************************
// ������static int _IndexOf( _LPMENUDATA lpMenu, _LPMENUITEM lpItem )
// ������
//	IN lpMenu - �˵��ṹָ��
//	IN lpItem - ��Ŀ�ṹָ��
// ����ֵ��
//	����������,ʧ�ܣ�����-1
// ����������
//	�õ�һ���˵����ڲ˵��е������ţ����������㣬��0Ϊ������
// ����: 
//	
// ************************************************

static int _IndexOf( _LPMENUDATA lpMenu, _LPMENUITEM lpItem )
{
    _LPMENUITEM lpTemp = lpMenu->lpLastItem;
    int i;

    ASSERT( lpTemp );
    i = 0;
    do	{
       lpTemp = lpTemp->lpNext;
       if( lpTemp == lpItem )
	         return i;	// �ҵ�������������
       i++;
    }while( lpTemp != lpMenu->lpLastItem );
    return -1;
}

// **************************************************
// ������static int _Count( _LPMENUDATA lpMenu )
// ������
//	IN lpMenu - �˵��ṹָ��
// ����ֵ��
//	���ز˵��Ĳ˵���Ŀ��
// ����������
//	�õ��˵��Ĳ˵���Ŀ��
// ����: 
//	
// ************************************************

static int _Count( _LPMENUDATA lpMenu )
{
	return lpMenu->nCount;
}

// **************************************************
// ������static _LPMENUITEM _At( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags, _LPMENUITEM * lppPrev )
// ������
//	IN lpMenu - �˵��ṹָ��
//	IN uPosition - �˵���Ŀ��λ�û�ID,������ uFlags
//	IN uFlags - ��ѯ��ʽ��
//					MF_BYPOSITION - ͨ��λ�ò��Ҳ˵���Ŀ
//					MF_BYCOMMAND - ͨ��ID���Ҳ˵���Ŀ
//	IN lppPrev - ���ڽ���ƥ����Ŀ��ǰһ����Ŀָ��
//	
// ����ֵ��
//	�����ҵ������ز˵���Ŀ; ���򣬷���NULL
// ����������
//	����λ�û�ID�õ��˵���һ����Ŀ
// ����: 
//	
// ************************************************

static _LPMENUITEM _At( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags, _LPMENUITEM * lppPrev )
{
    _LPMENUITEM	lpItem;
    DWORD i;

    if( (lpItem = lpMenu->lpLastItem) != 0 )
    {
	      if( uFlags )// MF_BYPOSITION
	      {	// ͨ��λ�ò���
	          i = 0;
	          do   {
		            if( lppPrev )
		                *lppPrev = lpItem;
                lpItem = lpItem->lpNext;
                if( i == uPosition )
                    return lpItem;	// �ҵ�
                i++;
            }while( lpItem != lpMenu->lpLastItem );
	      }
	      else
	      {   // ͨ��ID���� MF_BYCOMMAND
	          do   {
                if( lppPrev )
		                *lppPrev = lpItem;
		            lpItem = lpItem->lpNext;
                if( lpItem->wID == uPosition )
                    return lpItem;	// �ҵ�
            }while( lpItem != lpMenu->lpLastItem );
        }
    }
    return 0;  // not found
}

// **************************************************
// ������static _LPMENUITEM _RemoveItem( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags )
// ������
//	IN lpMenu - �˵�ָ��
//	IN uPosition - �˵���Ŀ��λ�û�ID,������ uFlags
//	IN uFlags - ��ѯ��ʽ��
//					MF_BYPOSITION - ͨ��λ�ò��Ҳ˵���Ŀ
//					MF_BYCOMMAND - ͨ��ID���Ҳ˵���Ŀ
// ����ֵ��
//	�����ҵ������ط�NULL�Ĳ˵���Ŀָ�룻���򣬷���NULL
// ����������
//	�Ƴ��˵���Ŀ����
// ����: 
//	
// ************************************************

static _LPMENUITEM _RemoveItem( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags )
{
    _LPMENUITEM lpPrev, lpItem = 0;
	
    if( lpMenu  && lpMenu->lpLastItem )
    {	// ���Ҳ˵���Ŀ
		if( _At( lpMenu, uPosition, uFlags, &lpPrev ) )
		{	// �ҵ�
			if( lpPrev == lpMenu->lpLastItem )
			{   // �ò˵�����һ���˵���Ŀ only one item
				lpItem = lpPrev;
				lpMenu->lpLastItem = lpMenu->lpDeftItem = 0;
			}
			else
			{	// save item pointer
				lpItem = lpPrev->lpNext;
				// �Ƴ� remove link
				lpPrev->lpNext = lpPrev->lpNext->lpNext;
				if( lpItem == lpMenu->lpDeftItem && lpItem == lpMenu->lpLastItem )
					lpMenu->lpDeftItem = lpMenu->lpLastItem = lpPrev;
				else if( lpItem == lpMenu->lpDeftItem )
					lpMenu->lpDeftItem = lpItem->lpNext;
				else if( lpItem == lpMenu->lpLastItem )
					lpMenu->lpLastItem = lpPrev;
				else
				{	// ��֪���Ĵ���
					ASSERT( 0 ); // error
					lpItem = 0;
					goto REV_RET;
				}
			}
			lpItem->lpOwnerMenu = NULL;
			lpMenu->nCount--;	// ���ٲ˵���Ŀ����ӵ����
		}
    }
REV_RET:
    return lpItem;
}

// **************************************************
// ������HMENU WINAPI Menu_Create(void)
// ������
//	��
// ����ֵ��
//	����ɹ������ز˵������������򣬷���NULL
// ����������
//	�����˵����󣬲���������
// ����: 
//	ϵͳAPI
// ************************************************

HMENU WINAPI Menu_Create(void)
{
    _LPMENUDATA lpMenu;

    lpMenu = _AllocMenuData();// GetCallerProcess() );
    return (HMENU)lpMenu;
}

// **************************************************
// ������HMENU WINAPI Menu_CreatePopup(void)
// ������
//	��
// ����ֵ��
//	����ɹ������ز˵������������򣬷���NULL
// ����������
//	�����˵����󣬲���������
// ����: 
//	ϵͳAPI
// ************************************************

HMENU WINAPI Menu_CreatePopup(void)
{
    _LPMENUDATA lpMenu;

    lpMenu = _AllocMenuData();// GetCallerProcess() );
    return (HMENU)lpMenu;
}

// **************************************************
// ������BOOL WINAPI Menu_Destroy( HMENU hMenu )
// ������
//	IN hMenu - �˵����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�ƻ��˵�����
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Menu_Destroy( HMENU hMenu )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );

    ASSERT( lpMenu );
    if( lpMenu )
	{
		int count = _Count( lpMenu );	// ͳ�Ʋ˵�����Ŀ��
		// ɾ�����еĲ˵���Ŀ
		count--;
		while( count >= 0 )
		{
			Menu_Delete( hMenu, (DWORD)count, MF_BYPOSITION );
			count--;
		}
		_FreeMenuData( lpMenu );	//�ͷŲ˵���Ŀ
		return TRUE;
	}
    return FALSE;    
}

// **************************************************
// ������void FreeInstanceMenu( HANDLE hOwnerProcess )
// ������
//	IN hOwnerProcess - ӵ���߽��̾��
// ����ֵ��
//	��
// ����������
//	�ͷŽ���ӵ�е����в˵�����
// ����: 
//	��GwmeServerHandler��������
// ************************************************

// call by WinSys_CloseObject
/*
void FreeInstanceMenu( HANDLE hOwnerProcess )
{
	_LPMENUDATA lpMenu = lpMenuList;

	while( lpMenu )
	{
		if( lpMenu->hOwnerProcess == hOwnerProcess )
		{   // �˵����󱻸ý���ӵ�У��õ��˵��Ķ����˵� get top level menu
			while( lpMenu->lpOwnerItem )
			{	// 
				lpMenu = lpMenu->lpOwnerItem->lpOwnerMenu;
				ASSERT( lpMenu );
			}
			// �ƻ�
			Menu_Destroy( (HMENU)lpMenu );
			// restart
			lpMenu = lpMenuList;
		}
		else
			lpMenu = lpMenu->lpNext;	// ��һ���˵�
	}
}
*/
// **************************************************
// ������BOOL WINAPI Menu_Remove( HMENU hMenu, DWORD uPosition, DWORD uFlags )
// ������
//	IN hMenu - �˵����
//	IN uPosition - �˵���Ŀ��λ�û�ID,������ uFlags
//	IN uFlags - ��ѯ��ʽ��
//					MF_BYPOSITION - ͨ��λ�ò��Ҳ˵���Ŀ
//					MF_BYCOMMAND - ͨ��ID���Ҳ˵���Ŀ
//	
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	��һ���˵���Ŀ�Ӳ˵��Ƴ�
// ����: 
//	ϵͳAPI
// ************************************************

//deletes a menu item from the specified menu
BOOL WINAPI Menu_Remove( HMENU hMenu, DWORD uPosition, DWORD uFlags )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    if( lpMenu )
    {        
        _LPMENUITEM lpItem;
		// �Ƴ�
        if( (lpItem = _RemoveItem( lpMenu, uPosition, uFlags )) != 0 )
        {	
			if( lpItem->hSubMenu )
			{   // ����ò˵������Ӳ˵���������Ӳ˵���ӵ���� remove the submenu from the item
				_LPMENUDATA lpSubMenu = _GetHMENUPtr( lpItem->hSubMenu );
				if( lpSubMenu )
				{
					lpSubMenu->lpOwnerItem = NULL;
				}
			}
			// ����ò˵���Ŀ����Ϊ�ַ��������ͷ���
            if( IS_STRING( lpItem->fType ) && lpItem->dwTypeData )
                free( lpItem->dwTypeData );
            free( lpItem );	// �ͷ���Ŀ
        }
        return (lpItem != 0);
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI Menu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags )
// ������
//	IN hMenu - �˵�������
//	IN uPosition - �˵���Ŀ��λ�û�ID,������ uFlags
//	IN uFlags - ��ѯ��ʽ��
//					MF_BYPOSITION - ͨ��λ�ò��Ҳ˵���Ŀ
//					MF_BYCOMMAND - ͨ��ID���Ҳ˵���Ŀ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ���˵�����
// ����: 
//	ϵͳAPI
// ************************************************

//deletes a menu item from the specified menu and free mem
BOOL WINAPI Menu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );

    ASSERT( lpMenu );    
    if( lpMenu )
    {	// ���Ƴ�
        _LPMENUITEM lpItem = _RemoveItem( lpMenu, uPosition, uFlags );
        if( lpItem )
        {
            // �������Ӳ˵������ƻ��� destroy submenu if possible
            if( lpItem->hSubMenu )
			{
                Menu_Destroy( lpItem->hSubMenu );
			}
            // ����˵���ĿΪ�ַ������ͣ����ͷ��ַ��� free item from mem
            if( IS_STRING( lpItem->fType ) && lpItem->dwTypeData )
                free( lpItem->dwTypeData );
            free( lpItem );
        }
        return (lpItem != 0);
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI Menu_Insert(  HMENU hMenu,  DWORD uPosition,  DWORD uFlags,  DWORD uIDNewItem,  LPCTSTR lpNewItem )
// ������
//	IN hMenu - �˵��������
//	IN uPosition - �˵���Ŀ��λ�û�ID,������ uFlags
//	IN uFlags - ��ѯ��ʽ��
//					MF_BYPOSITION - ͨ��λ�ò��Ҳ˵���Ŀ
//					MF_BYCOMMAND - ͨ��ID���Ҳ˵���Ŀ
//					MF_STRING - �˵���Ŀ����Ϊ�ַ���
//					
//	IN uIDNewItem - �˵�ID
//  IN lpNewItem - �˵���Ŀ���ݣ������� uFlags��־
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�� �˵��в����µĲ˵���Ŀ
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Menu_Insert(  HMENU hMenu,  DWORD uPosition,  DWORD uFlags,  DWORD uIDNewItem,  LPCTSTR lpNewItem )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    if( lpMenu )
    {      
        MENUITEMINFO mii;
		//2003-08-27, Add Map,
		if( IS_STRING( uFlags ) )
		{	// �ַ�������
#ifdef _MAPPOINTER
			lpNewItem = (LPCTSTR)MapPtrToProcess( (LPVOID)lpNewItem, GetCallerProcess() );
#else
			//lpNewItem = (LPCTSTR)MapPtrToProcess( (LPVOID)lpNewItem, GetCallerProcess() );
#endif
		}
		// ��ʼ���ṹ����
        mii.cbSize = sizeof( mii );
        mii.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
        mii.fType = uFlags & (~MFS_MASK);
		mii.fState = uFlags & MFS_MASK;
        mii.wID = uIDNewItem;
		if( IS_STRING( uFlags ) )
		{
            mii.dwTypeData = (LPTSTR)lpNewItem;
			mii.dwItemData = 0;
		}
		else
		{
		    mii.dwItemData = (DWORD)lpNewItem;
			mii.fMask |= MIIM_DATA;
			mii.dwTypeData = 0;
		}
		
        if( IS_STRING(uFlags) )
			mii.cch = strlen( lpNewItem );
        else   
			mii.cch = 0;
        // ����
        return Menu_InsertItem( hMenu, uPosition, (BOOL)(uFlags & MF_BYPOSITION), &mii );
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI Menu_InsertItem( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii )
// ������
//	IN hMenu - �˵�������
//	IN uItem - ID �� λ�ã������� fByPosition
//	IN fByPosition - ����ΪTRUE, ��ζuItem��ʾλ�ã�����uItem��ʾID
//  IN lpmii - �����˵���Ϣ��MENUITEMINFO���ݽṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	����һ���˵���Ŀ
// ����: 
//	ϵͳAPI
// ************************************************
 
BOOL WINAPI Menu_InsertItem( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    ASSERT( lpmii );
    if( lpMenu && lpmii )
    {       
        _LPMENUITEM lpPrev, lpItem;
        
        lpItem = _AllocItem();	// ����˵��ṹ

        if( lpItem )
        {	// ��ʼ������
            _SetItemData( lpItem, lpmii );
			//	����ӵ����
			lpItem->lpOwnerMenu = lpMenu;
			//	
            if( lpMenu->lpLastItem )
            {	// �õ���λ��/ID�Ĳ˵���Ŀ
                if( _At( lpMenu, uItem, fByPosition ? MF_BYPOSITION : MF_BYCOMMAND, &lpPrev ) )
                {	// �ҵ�
                    lpItem->lpNext = lpPrev->lpNext;
                    lpPrev->lpNext = lpItem;
                }
                else  // put bottom
                {	// û���ҵ����ŵ��ײ�
                    lpItem->lpNext = lpMenu->lpLastItem->lpNext;
                    lpMenu->lpLastItem->lpNext = lpItem;
                    lpMenu->lpLastItem = lpItem;
                }
            }
            else    // empty menu
            {	// ��ǰ�˵�û���κβ˵���Ŀ
                lpItem->lpNext = lpItem;
                lpMenu->lpLastItem = lpMenu->lpDeftItem = lpItem;
            }
			lpMenu->nCount++;	// ��ǰ�˵�����Ŀ����
        }
        return (BOOL)(lpItem != NULL);
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI Menu_EnableItem( HMENU hMenu, DWORD uPosition, DWORD uEnable )
// ������
//	IN hMenu - �˵�����
//	IN uPosition - ID �� λ�ã������� fByPosition
//	IN uEnable - �Բ˵���Ŀ�Ĳ�����Ϊ�������ݣ�
//					MF_BYCOMMAND - ˵�� uPosition ��ʾ�˵���ĿID
//					MF_BYPOSITION - ˵�� uPosition ��ʾ�˵���ĿID
//					MF_DISABLED - ˵�� �˵���Ŀ Ӧ��Ϊ��Ч״̬
//					MF_ENABLED - ˵�� �˵���Ŀ Ӧ��Ϊ��Ч״̬
//					MF_GRAYED - ˵�� �˵���Ŀ Ӧ��Ϊ��Ч�ͻ�ɫ״̬
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���ò˵�״̬
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Menu_EnableItem( HMENU hMenu, DWORD uPosition, DWORD uEnable )
{
    BOOL retv = FALSE;
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    if( lpMenu )
    {       
        _LPMENUITEM lpItem;
        // ����
        lpItem = _At( lpMenu, uPosition, (uEnable & MF_BYPOSITION) ? MF_BYPOSITION : MF_BYCOMMAND, 0 );
        if( lpItem )
        {	// 
			lpItem->fState &= ~(MFS_DISABLED|MFS_ENABLED|MFS_GRAYED);
            lpItem->fState |= (uEnable&(MFS_DISABLED|MFS_ENABLED|MFS_GRAYED));
            retv = TRUE;
        }
    }
    return FALSE;
}

// **************************************************
// ������DWORD WINAPI Menu_GetDefaultItem( HMENU hMenu, DWORD fByPos, DWORD uFlags )
// ������
//	IN hMenu - �˵�������
//	IN fByPos - ˵�����ز˵���Ŀ��λ�û���ID.����ΪTRUE,����λ�ã����򣬷���ID
//	IN uFlags - ��֧�֣�����Ϊ0
// ����ֵ��
//	���ص�ǰĬ�ϵĲ˵���Ŀ������ʧ�ܣ�����-1
// ����������
//	�õ���ǰ�˵�Ĭ�ϵĲ˵���Ŀ�����ض�Ӧ��ֵ
// ����: 
//	ϵͳAPI
// ************************************************

DWORD WINAPI Menu_GetDefaultItem( HMENU hMenu, DWORD fByPos, DWORD uFlags )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );    
    if( lpMenu && lpMenu->lpDeftItem )
    {
        if( fByPos )
            return lpMenu->lpDeftItem->wID;	// ����IDֵ
        else
            return _IndexOf( lpMenu,lpMenu->lpDeftItem );	// ����λ��ֵ
    }
    return -1;
}

// **************************************************
// ������int WINAPI Menu_GetItemCount( HMENU hMenu )
// ������
//	IN hMenu - �˵�������
// ����ֵ��
//	����ɹ������ص�ǰ�˵���Ŀ��������ʧ�ܣ�����-1
// ����������
//	�õ��˵���ӵ�е���Ŀ��
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI Menu_GetItemCount( HMENU hMenu )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );    
    if( lpMenu )
        return _Count( lpMenu );
    return -1;
}

// **************************************************
// ������int WINAPI Menu_GetItemID( HMENU hMenu, int nPos )
// ������
//	IN hMenu - �˵������� 
//	IN nPos - �˵���Ŀλ��
// ����ֵ��
//	����ɹ�������ָ���˵���Ŀ��ID; ���򣬷���-1
// ����������
//	�õ��˵���Ŀ��ID
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI Menu_GetItemID( HMENU hMenu, int nPos )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    if( lpMenu )
    {
        _LPMENUITEM lpItem = _At( lpMenu, nPos, MF_BYPOSITION, 0 );
        return lpItem ? (int)lpItem->wID : -1;
    }
    return -1;
}

// **************************************************
// ������BOOL WINAPI Menu_GetItemInfo( 
//								HMENU hMenu, 
//								DWORD uItem, 
//								BOOL fByPosition, 
//								LPMENUITEMINFO lpmii )
// ������
//	IN hMenu - �˵�������
//	IN uItem - �˵���Ŀ��ʶ�����庬����fByPosition������
//	IN fByPosition - ��ʾ uItem �Ǳ�ʾ�˵���Ŀλ�û��ǲ˵���Ŀ. ����ΪTRUE, ��ʾuItem����λ��
//	IN/OUT lpmii - ���ڽ��ܲ˵���Ŀ��Ϣ 
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�õ��˵���Ŀ����Ϣ
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Menu_GetItemInfo( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    ASSERT( lpmii );
    if( lpMenu && lpmii )
    {   // �õ��˵���Ŀ����  
        _LPMENUITEM lpItem = _At( lpMenu, uItem, fByPosition ? MF_BYPOSITION : MF_BYCOMMAND, 0 );
        if( lpItem )
            _GetItemData( lpItem, lpmii );	// �õ���Ϣ
        return (BOOL)(lpItem != NULL);
    }
	return FALSE;
}

// **************************************************
// ������HMENU WINAPI Menu_GetSub( HMENU hMenu, int nPos )
// ������
//	IN hMenu - �˵�������
//	IN nPos - �˵���Ŀλ��
// ����ֵ��
//	����ɹ�������ָ���˵���Ŀ��ӵ�е��Ӳ˵������򣬷���NULL
// ����������
//	�õ��˵���Ŀ��ӵ�е��Ӳ˵�
// ����: 
//	ϵͳAPI
// ************************************************

HMENU WINAPI Menu_GetSub( HMENU hMenu, int nPos )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );

    ASSERT( lpMenu );
    if( lpMenu )
	{
        _LPMENUITEM lpItem = 0;
        lpItem = _At( lpMenu, nPos, MF_BYPOSITION, 0 );  // ���Ҳ˵���Ŀ
        return lpItem ? lpItem->hSubMenu : (HMENU)NULL;
	}
    return (HMENU)NULL;
}

// **************************************************
// ������BOOL WINAPI Menu_SetDefaultItem( HMENU hMenu, DWORD uItem, DWORD fByPos )
// ������
//	IN hMenu - �˵������� 
//	IN uItem - �˵���Ŀ��ʶ�����庬���ɲ��� fByPos ������
//	IN fByPos - ����uItem�ĺ��塣����ΪTRUE,��ʾuItemΪ�˵���Ŀλ�ã����򣬱�ʾ�˵���ĿID
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ���ǰ�˵���Ĭ����Ŀ
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Menu_SetDefaultItem( HMENU hMenu, DWORD uItem, DWORD fByPos )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    _LPMENUITEM lpItem = 0;
    
    ASSERT( lpMenu );
    if( lpMenu )
    {
        lpItem = _At( lpMenu, uItem, fByPos, 0 );	// ���Ҳ˵���Ŀ
        if( lpItem )
            lpMenu->lpDeftItem = lpItem;	// ��ΪĬ��ֵ
    }
    return lpItem != 0;
}

// **************************************************
// ������BOOL WINAPI Menu_SetItemInfo( HMENU hMenu, DWORD uItem, BOOL fByPos, LPMENUITEMINFO lpmii )
//	IN hMenu - �˵�������
//	IN uItem - �˵���Ŀ��ʶ�����庬����fByPosition������
//	IN fByPos - ��ʾ uItem �Ǳ�ʾ�˵���Ŀλ�û��ǲ˵���Ŀ. ����ΪTRUE, ��ʾuItem����λ��
//	IN lpmii - ������Ҫ���õĲ˵���Ŀ��Ϣ 
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���ò˵���Ŀ����Ϣ
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Menu_SetItemInfo( HMENU hMenu, DWORD uItem, BOOL fByPos, LPMENUITEMINFO lpmii )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    _LPMENUITEM lpItem = 0;
    
    ASSERT( lpMenu );
    ASSERT( lpmii );
    if( lpMenu && lpmii )
    {	
        lpItem = _At( lpMenu, uItem, fByPos, 0 );	// ���Ҳ˵���Ŀ
        if( lpItem )
            _SetItemData( lpItem, lpmii );	// ������Ϣ
    }
    return lpItem != 0;	//
}

// **************************************************
// ������ATOM RegisterMenuClass( HINSTANCE hInst )
// ������
//	IN hInst - ʵ�����
// ����ֵ��
//	����ɹ�������ԭ�ӣ����򣬷���0
// ����������
//	ע��˵���
// ����: 
//	
// ************************************************

// define menu class
ATOM RegisterMenuClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = CS_PARENTDC | CS_DBLCLKS;
    wc.lpfnWndProc = MenuWndProc;
    wc.hInstance = hInst;
    wc.hIcon = 0;
    wc.cbWndExtra = sizeof( HMENU ) + sizeof( HWND ) + sizeof( UINT );
	wc.cbClsExtra = 0;
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = 0;//(HRGN)COLOR_BACKGROUND;
    wc.lpszMenuName = 0;
    wc.lpszClassName = strClassMenu;
    return RegisterClass( &wc );
}

// **************************************************
// ������static int GetItemPos( _LPMENUDATA lpMenu, _LPMENUITEM lpItem )
// ������
//	IN lpMenu - MENUDATA�ṹ����ָ��
//	IN lpItem - �˵���Ŀָ��
// ����ֵ��
//	���ز˵���Ŀλ��
// ����������
//	�õ��˵���Ŀ�ڲ˵��е�y����λ��
// ����: 
//	
// ************************************************

static int GetItemPos( _LPMENUDATA lpMenu, _LPMENUITEM lpItem )
{
	int i = 0;
	if( lpMenu->nCount )
	{
		_LPMENUITEM lpCurItem = lpMenu->lpLastItem->lpNext;

		i = EDGEWIDTH + VERTBANK;//lpCurItem->iItemHeight;  // Modified By Jami 2003.06.17    2 -- edge , VERTBANK -- top bank
		for( ; lpCurItem && lpCurItem != lpItem; )
		{
			i += lpCurItem->iItemHeight;
			lpCurItem = lpCurItem->lpNext;
		}
		ASSERT( lpCurItem == lpItem );
	}
	return i;
}

// **************************************************
// ������static int GetItemBound( _LPMENUDATA lpMenu, _LPMENUITEM lpItem, LPRECT lprect )
// ������
//	IN lpMenu - MENUDATA�ṹ����ָ��
//	IN lpItem - �˵���Ŀָ��
//	OUT lprect - ���ڽ��ܲ˵���Ŀ���� 
// ����ֵ��
//	����TRUE
// ����������
//	�õ��˵���Ŀ����
// ����: 
//	
// ************************************************

static BOOL GetItemBound( _LPMENUDATA lpMenu, _LPMENUITEM lpItem, LPRECT lprect )
{
	if( lpMenu->nCount )
	{
		lprect->top = GetItemPos( lpMenu, lpItem );
		lprect->bottom = lprect->top + lpItem->iItemHeight;
	}
	return TRUE;
}

// **************************************************
// ������static _LPMENUITEM GetItemPtr( _LPMENUDATA lpMenu, POINT pt )
// ������
//	IN lpMenu - MENUDATA�ṹ����ָ��
//	IN pt - �ͻ�������
// ����ֵ��
//	����ɹ������ز˵���Ŀָ�룻���򣬷���NULL
// ����������
//	�õ��������Ӧ�Ĳ˵���Ŀָ��
// ����: 
//	
// ************************************************

static _LPMENUITEM GetItemPtr( _LPMENUDATA lpMenu, POINT pt )
{
	if( lpMenu->nCount )
	{
		_LPMENUITEM lpCurItem = lpMenu->lpLastItem->lpNext;
		int n = lpMenu->nCount;
		int i;

		i = EDGEWIDTH + VERTBANK;//lpCurItem->iItemHeight;  // Modified By Jami 2003.06.17    EDGEWIDTH -- edge , VERTBANK -- top bank
		for( ; lpCurItem && n; n--)
		{
			if( i <= pt.y && pt.y < (int)(i +  lpCurItem->iItemHeight) )
				break;	// ƥ��
			i += lpCurItem->iItemHeight;
			lpCurItem = lpCurItem->lpNext;	// ��һ���˵���Ŀ
		}
		return lpCurItem;	// 
	}
	return NULL;
}

// **************************************************
// ������static void CalcMenuRect( HWND hwndOwner, HMENU hMenu, LPRECT lpRect, UINT uiFlags )
// ������
//	IN hwndOwner - ���ھ��
//	IN hMenu - �˵����
//	OUT lpRect - ���ڽ��ܲ˵��ľ���
//	IN uiFlags - ����
// ����ֵ��
//	��
// ����������
//	�õ��˵�����ʾ����
// ����: 
//	
// ************************************************

static void CalcMenuRect( HWND hwndOwner, HMENU hMenu, LPRECT lpRect, UINT uiFlags )
{
	static int nCharWidth = -1;
	int i, count;
	UINT maxy, maxx;
    _LPMENUDATA lpMenu;
	MEASUREITEMSTRUCT mis;
	_LPMENUITEM lpItem = NULL;
	int cyItem = GetSystemMetrics( SM_CYMENU ); 

	if( nCharWidth == -1 )
	{
		HDC hdc = GetDC( NULL );
		GetCharWidth( hdc, 'a', 'a', &nCharWidth );
		ReleaseDC( NULL, hdc );
	}

	lpMenu = _GetHMENUPtr( hMenu );	// �õ������Ӧ�Ķ���ָ��
	//count = lpMenu->nCount;
	if( (count = lpMenu->nCount) )
	    lpItem = lpMenu->lpLastItem->lpNext; //��һ��

	if( lpMenu->iItemWidth == 0 )
	    maxx = 1;//2 * 8 + 4;
	else
		maxx = lpMenu->iItemWidth;
	maxy = 1;
	
	for( i = 0; i < count; i++ )
	{
		if( (lpItem->fType & MFT_OWNERDRAW) )
		{	// �Ի���
		    if( (lpItem->fState & MFS_UPDATE_OWNER_DRAW) == 0 )
			{	// �Ӳ˵��Ĵ���ӵ���ߵõ��Զ�������
				mis.CtlID = 0;
				mis.CtlType = ODT_MENU;
				mis.itemData = lpItem->dwItemData;
				mis.itemHeight = cyItem;//16;
				mis.itemID = lpItem->wID;
				mis.itemWidth = 16;
				SendMessage( hwndOwner, WM_MEASUREITEM, 0, (LPARAM)&mis );
				lpItem->fState |= MFS_UPDATE_OWNER_DRAW;
				lpItem->iItemHeight = mis.itemHeight;
				maxx = MAX( maxx, mis.itemWidth );	// �õ����Ŀ��
			}
		}
		else if( IS_STRING( lpItem->fType ) )
		{	// �ַ������
			UINT ex;
			lpItem->iItemHeight = cyItem;  // ��Ĭ�ϸ߶� default height
			ex = 2 * 16 + 4 + lpItem->cch * nCharWidth;//8;	
			maxx = MAX( maxx, ex );	// �õ����Ŀ��
		}
		else
		{
			lpItem->iItemHeight = cyItem;  // ��Ĭ�ϸ߶� default height
		}
		maxy += lpItem->iItemHeight;
		lpItem = lpItem->lpNext;
	}
	// ���÷���ֵ
	lpRect->left = 0;
	lpRect->top = 0;
	lpRect->right = maxx + 2 * EDGEWIDTH;      // modified By Jami chen 2003.06.18
	//lpRect->bottom = maxy ;
	lpRect->bottom = maxy + 2 * EDGEWIDTH + 2 * VERTBANK; // modified By Jami chen 2003.06.18
	lpMenu->iItemWidth = maxx;

}

// **************************************************
// ������static int GetFormatString( _LPMENUITEM lpItem, char * lpbuf )
// ������
//	IN lpItem - �˵���Ŀָ��
//	OUT lpbuf - ���ڽ��ܸ�ʽ�����ַ���
// ����ֵ��
//	�����ַ������ܳ���
// ����������
//	��ʽ���˵���Ŀ�ַ���
// ����: 
//	
// ************************************************

static int GetFormatString( _LPMENUITEM lpItem, char * lpbuf )
{
    int retv;
    strncpy( lpbuf, lpItem->dwTypeData, 29 );
    retv = (int)lpItem->cch;
    if( lpItem->hSubMenu )
    {
        strcat( lpbuf, " " );	//�Ӳ˵��ļ�ͷ ���磺 "menu item    -> "
        retv += 2;
    }
    return retv;
}

// **************************************************
// ������static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
// ������
//	IN hdc - ��ͼDC���
//	IN lprc - ��Ҫ���ľ���
//	IN clr - ��ɫֵ
// ����ֵ��
//	��
// ����������
//	������
// ����: 
//	
// ************************************************

static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

// **************************************************
// ������static void DrawMenuItem( 
//						HWND hwnd, 
//						_LPMENUDATA lpMenu, 
//						_LPMENUITEM lpItem,
//						HDC hdc,						 
//						LPCRECT lpClipRect, 
//						BOOL bDefault )

// ������
//	IN hwnd - ���ھ�� 
//	IN lpMenu - �˵�����ָ��
//	IN lpItem - �˵���Ŀָ��
//	IN hdc - ��ͼDC���
//	IN lpClipRect - �˵�����ָ��
//	IN bDefault - ��ʾ�ò˵���Ŀ�Ƿ�Ϊ��ǰĬ�ϲ˵���
// ����ֵ��
//	��
// ����������
//	���˵���Ŀ
// ����: 
//	
// ************************************************

static void DrawMenuItem( HWND hwnd, _LPMENUDATA lpMenu, 
						 _LPMENUITEM lpItem,
						 HDC hdc,						 
						 LPCRECT lpClipRect, 
						 BOOL bDefault )
{
    RECT rect = *lpClipRect;
    char buf[32];

    if( lpItem->fType & MFT_OWNERDRAW )
	{	// �Ի���
		DRAWITEMSTRUCT dis;
		dis.CtlID = 0;
		dis.CtlType = ODT_MENU;
		dis.hDC = hdc;
		dis.hwndItem = (HWND)lpMenu;//hwnd;//lpItem->wID;
		dis.itemAction = ODA_DRAWENTIRE;
		dis.itemData = lpItem->dwItemData;
		dis.itemID = lpItem->wID;			
		dis.itemState = lpItem->fState;
		dis.rcItem = *lpClipRect;

		dis.itemState = 0;
		//(MFS_DISABLED|MFS_ENABLED|MFS_GRAYED)
		if( lpItem->fState & MFS_DISABLED )
			dis.itemState |= ODS_DISABLED;
		if( lpItem->fState & MFS_GRAYED )
			dis.itemState |= ODS_GRAYED;
		if( bDefault )
			dis.itemState |= ODS_DEFAULT|ODS_SELECTED;
		// �õ��˵���Ŀ�ľ��α߽�
	    GetItemBound( lpMenu, lpItem, &dis.rcItem );
		// ��������Ϣ
		SendMessage( GetParent(hwnd), WM_DRAWITEM, 0, (LPARAM)&dis );
		return;
	}
	else
	{   // �õ��˵���Ŀ�ľ��α߽�
	   GetItemBound( lpMenu, lpItem, &rect );

       if( lpItem->fType & MFT_SEPARATOR )
	   {	// �ò˵���Ŀ��һ���ָ���
		   HPEN hPen;
		   // ������ѡ���
		   hPen = CreatePen(PS_SOLID,1,MENUDARKSHADE);
		   hPen = SelectObject(hdc,hPen);
		   // ���û������
		   MoveToEx( hdc, rect.left+4, (rect.top + rect.bottom)/2, NULL );
		   // ������
		   LineTo( hdc, rect.right-4, (rect.top + rect.bottom)/2 );
		   hPen = SelectObject(hdc,hPen);
		   DeleteObject(hPen);
			//
		   hPen = CreatePen(PS_SOLID,1,MENULIGHTSHADE);
		   hPen = SelectObject(hdc,hPen);
		   MoveToEx( hdc, rect.left+4, (rect.top + rect.bottom)/2+1, NULL );
		   // ������
		   LineTo( hdc, rect.right-4, (rect.top + rect.bottom)/2+1 );
		   hPen = SelectObject(hdc,hPen);
		   DeleteObject(hPen);

	   }
       else if( IS_STRING( lpItem->fType ) )
       {	// ��Ŀ���ַ���
		   COLORREF dwOldTextColor;
		   
		   if( lpItem->fState & (MFS_GRAYED|MFS_DISABLED) )
		   {	// ��ǰ״̬Ϊ��/��Ч
			   FillSolidRect( hdc, &rect, lpMenu->cl_TextBk );
			   dwOldTextColor = SetTextColor( hdc, lpMenu->cl_Disable );
		   }
		   else if( bDefault )
		   {	// Ĭ����Ŀ
			   FillSolidRect( hdc, &rect, lpMenu->cl_SelectionBk );
			   dwOldTextColor = SetTextColor( hdc, lpMenu->cl_Selection );
		   }
		   else
		   {	// ��ͨ��Ŀ
			   FillSolidRect( hdc, &rect, lpMenu->cl_TextBk );
			   dwOldTextColor = SetTextColor( hdc, lpMenu->cl_Text );
		   }

		   rect.left += 16;
		   rect.right -= 16;

		   SetBkMode( hdc, TRANSPARENT );
		   //	��ʾ�ı�
           DrawText( hdc, buf, GetFormatString( lpItem, buf ), &rect, DT_SINGLELINE | DT_VCENTER );
		   SetTextColor( hdc, dwOldTextColor );	// �ָ�
       }
    }
}

// **************************************************
// ������static void DrawMenu( HWND hwnd, HDC hdc )
// ������
//	IN hwnd - ���ھ��
//	IN hdc - ��ͼDC
// ����ֵ��
//	��
// ����������
//	���˵�
// ����: 
//	
// ************************************************

static void DrawMenu( HWND hwnd, HDC hdc )
{
    _LPMENUDATA lpMenu;
    _LPMENUITEM lpItem;
    HMENU hMenu;
    int count, i;
    RECT rect;

	hMenu = (HMENU)GetWindowLong( hwnd, 0 );	// �Ӵ����û��������õ��˵����
    lpMenu = _GetHMENUPtr( hMenu );	// �ɲ˵�����õ��˵�����ָ��

    GetClientRect( hwnd, &rect );	// �õ��˵�����ָ��
	InflateRect(&rect,(0-EDGEWIDTH),(0-EDGEWIDTH));  // Add By Jami chen in 2003.06.17 for will to Draw Edge
    if( lpMenu && lpMenu->lpLastItem )
    {	// 
		count = lpMenu->nCount;//�˵���Ŀ������//GetMenuItemCount( hMenu );
        lpItem = lpMenu->lpLastItem->lpNext;
        for( i = 0; i < count; i++ )
        {	// ��ÿһ���˵���Ŀ
			DrawMenuItem( hwnd, lpMenu, lpItem, hdc, &rect, lpItem == lpMenu->lpDeftItem );
            lpItem = lpItem->lpNext;
        }
    }
}

// **************************************************
// ������static _LPMENUITEM DoPosChange( HWND hwnd, short x, short y )
// ������
//	IN hwnd - �˵����
//	IN x - ��ǰ����ڲ˵����������x����
//	IN y - ��ǰ����ڲ˵����������y����
// ����ֵ��
//	�������У�����������ڵĲ˵���Ŀ�����򣬷���NULL
// ����������
//  ��������еĲ˵���Ŀ��Ϊ��ǰĬ����Ŀ	
// ����: 
//	
// ************************************************

static _LPMENUITEM DoPosChange( HWND hwnd, short x, short y )	//
{
    RECT rect;
    POINT pt;
    HMENU hMenu;
    HDC hdc;
    _LPMENUDATA lpMenu;
	_LPMENUITEM lpItem = NULL;

    pt.y = y;
    pt.x = x;
	// �õ��ͻ�������
    GetClientRect( hwnd, &rect );
	// ��ȥ�߽���
	InflateRect(&rect,(0-EDGEWIDTH),(0-EDGEWIDTH));  // Add By Jami chen in 2003.06.17 for will to Draw Edge

    hMenu = (HMENU)GetWindowLong( hwnd, 0 );
    lpMenu = _GetHMENUPtr( hMenu );

    if( PtInRect( &rect, pt ) )
    {
		lpItem = GetItemPtr( lpMenu, pt );	// �õ�������
    }
    if( lpItem != lpMenu->lpDeftItem )
    {	// �õ���ͼDC
        hdc = GetDC( hwnd );
        // �ػ浱ǰ��Ĭ����Ϊ��ͨ״̬ reset old hilight item
        if( lpMenu->lpDeftItem )
			DrawMenuItem( hwnd, lpMenu, lpMenu->lpDeftItem, hdc, &rect, FALSE );
        // ���õ�ǰ������ΪĬ����Ŀ
		lpMenu->lpDeftItem = lpItem;
        // ˢ��Ĭ���� new hilight item if possible
        if( lpItem )
			DrawMenuItem( hwnd, lpMenu, lpItem, hdc, &rect, TRUE );
        ReleaseDC( hwnd, hdc );	//
    }
    return lpItem;
}

// **************************************************
// ������static LRESULT DoCreate( HWND hwnd, LPCREATESTRUCT lpcs )
// ������
//	IN hwnd - ���ھ��
//	IN lpcs - �����ṹ
// ����ֵ��
//	����0
// ����������
//	��ʼ�����ڵĲ˵�����
// ����: 
//	���� WM_CREATE ��Ϣ
// ************************************************

static LRESULT DoCreate( HWND hwnd, LPCREATESTRUCT lpcs )
{
	SetWindowLong( hwnd, 0, (LONG)((MENUPARAM*)lpcs->lpCreateParams)->hMenu );
	SetWindowLong( hwnd, 4, (LONG)((MENUPARAM*)lpcs->lpCreateParams)->hOwner );
	SetWindowLong( hwnd, 8, (LONG)((MENUPARAM*)lpcs->lpCreateParams)->uiFlag );
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
	{
		HMENU hMenu;
		_LPMENUDATA lpMenu;
		
		hMenu = (HMENU)GetWindowLong( hWnd, 0 );
		lpMenu = _GetHMENUPtr( hMenu );
		
		if( lpMenu && lpMenu->lpLastItem )
		{
			if( lpccs->fMask & CLF_TEXTCOLOR )
				lpMenu->cl_Text = lpccs->cl_Text;	// �ı���ɫ
			if( lpccs->fMask & CLF_TEXTBKCOLOR )
				lpMenu->cl_TextBk = lpccs->cl_TextBk;	// �ı�����
			if( lpccs->fMask & CLF_SELECTIONCOLOR )
				lpMenu->cl_Selection = lpccs->cl_Selection;	// ��ѡ��Ĳ˵���Ŀ�ı�
			if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
				lpMenu->cl_SelectionBk = lpccs->cl_SelectionBk;	// ��ѡ��Ĳ˵���Ŀ�ı�����
			if( lpccs->fMask & CLF_DISABLECOLOR )
				lpMenu->cl_Disable = lpccs->cl_Disable;	// ��Ч�˵���Ŀ�ı�
			if( lpccs->fMask & CLF_DISABLEBKCOLOR )
				lpMenu->cl_DisableBk = lpccs->cl_DisableBk;	// ��Ч�Ĳ˵���Ŀ�ı�����		
			return TRUE;
		}
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
	{
		HMENU hMenu;
		_LPMENUDATA lpMenu;
		
		hMenu = (HMENU)GetWindowLong( hWnd, 0 );
		lpMenu = _GetHMENUPtr( hMenu );

		if( lpccs->fMask & CLF_TEXTCOLOR )
		    lpccs->cl_Text = lpMenu->cl_Text;	// �ı���ɫ
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		    lpccs->cl_TextBk = lpMenu->cl_TextBk;	// �ı�����
		if( lpccs->fMask & CLF_SELECTIONCOLOR )
		    lpccs->cl_Selection = lpMenu->cl_Selection;// ��ѡ��Ĳ˵���Ŀ�ı�
		if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
		    lpccs->cl_SelectionBk = lpMenu->cl_SelectionBk;// ��ѡ��Ĳ˵���Ŀ�ı�����
		if( lpccs->fMask & CLF_DISABLECOLOR )
		    lpccs->cl_Disable = lpMenu->cl_Disable;	// ��Ч�˵���Ŀ�ı�
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
		    lpccs->cl_DisableBk = lpMenu->cl_DisableBk;// ��Ч�Ĳ˵���Ŀ�ı�����
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
	HMENU hMenu;
	_LPMENUDATA lpMenu;
	
	hMenu = (HMENU)GetWindowLong( hWnd, 0 );
	lpMenu = _GetHMENUPtr( hMenu );
	_SetMenuColor( lpMenu );
	return 0;
}

// **************************************************
// ������static LRESULT DoEraseBkgnd( HWND hWnd,HDC hdc )
// ������
//	IN hWnd - ���ھ��
//	IN hdc - ��ͼDC
// ����ֵ��
//	����TRUE
// ����������
//	���ƴ��ڱ���
// ����: 
//	���� WM_ERASEBKGND ��Ϣ
// ************************************************

static LRESULT DoEraseBkgnd( HWND hWnd,HDC hdc )
{
	RECT rect;
    _LPMENUDATA lpMenu;
    HMENU hMenu;
	HPEN hPen;
	
	hMenu = (HMENU)GetWindowLong( hWnd, 0 );
    lpMenu = _GetHMENUPtr( hMenu );
	// �õ��ͻ���	
	GetClientRect(hWnd,&rect);
	// ��䱳��ɫ
	FillSolidRect( hdc, &rect, lpMenu->cl_TextBk );
	// �����
	hPen = CreatePen(PS_SOLID,1,MENULIGHTSHADE);
	hPen = SelectObject(hdc,hPen);
	MoveToEx( hdc,rect.right - 1, rect.top, NULL );
	LineTo( hdc,  rect.left, rect.top);
	LineTo( hdc, rect.left, rect.bottom - 1);
	hPen = SelectObject(hdc,hPen);
	DeleteObject(hPen);
	
	hPen = CreatePen(PS_SOLID,1,MENULIGHT1SHADE);
	hPen = SelectObject(hdc,hPen);
	MoveToEx( hdc,rect.right - 2, rect.top-1, NULL );
	LineTo( hdc,  rect.left-1, rect.top-1);
	LineTo( hdc, rect.left-1, rect.bottom - 2);
	hPen = SelectObject(hdc,hPen);
	DeleteObject(hPen);
	
	hPen = CreatePen(PS_SOLID,1,MENUDARKSHADE);
	hPen = SelectObject(hdc,hPen);
	MoveToEx( hdc,rect.right -1 , rect.top, NULL );
	LineTo( hdc,  rect.right -1 , rect.bottom - 1);
	LineTo( hdc, rect.left, rect.bottom - 1);
	hPen = SelectObject(hdc,hPen);
	DeleteObject(hPen);
	
	hPen = CreatePen(PS_SOLID,1,MENUDARK1SHADE);
	hPen = SelectObject(hdc,hPen);
	MoveToEx( hdc,rect.right -2 , rect.top-1, NULL );
	LineTo( hdc,  rect.right -2 , rect.bottom - 2);
	LineTo( hdc, rect.left-1, rect.bottom - 2);
	hPen = SelectObject(hdc,hPen);
	DeleteObject(hPen);
	return TRUE;
}

// **************************************************
// ������static LRESULT WINAPI MenuWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
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

static LRESULT CALLBACK MenuWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HDC hdc;
    PAINTSTRUCT ps;
	_LPMENUITEM lpItem;
    _LPMENUDATA lpMenuData;
    RECT rect;

    switch( msg )
    {
	case WM_PAINT:		// ���ƿͻ���
		hdc = BeginPaint( hwnd, &ps );
		DrawMenu( hwnd, hdc );
		EndPaint( hwnd, &ps );
		break;
	case WM_ERASEBKGND:		// ���Ʊ���
		DoEraseBkgnd( hwnd,( HDC )wParam );
		return 0;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
		// ��������±���Ϣ
		lpItem = DoPosChange( hwnd, LOWORD( lParam ), HIWORD( lParam ) );//, &index );
		if( msg == WM_LBUTTONDOWN )
		{
			if( lpItem == 0 )
			{
				POINT pt;
				
				pt.x = (short)LOWORD( lParam );
				pt.y = (short)HIWORD( lParam );
				ClientToScreen( hwnd, &pt );
				
				
				lpMenuData = _GetHMENUPtr( (HMENU)GetWindowLong( hwnd, 0 ) );
				// set quit modal flag
				lpMenuData->active = MA_RETURN;
				//mouse_event( MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0 );
				//mouse_event( MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0 );
			}
		}
		break;
	case WM_LBUTTONUP:		// �������̧����Ϣ 
		DoPosChange( hwnd, LOWORD( lParam ), HIWORD( lParam ) );
		lpMenuData = _GetHMENUPtr( (HMENU)GetWindowLong( hwnd, 0 ) );
		if( lpMenuData->lpDeftItem )
		{
			if( lpMenuData->lpDeftItem->hSubMenu )
			{
				GetWindowRect( hwnd, &rect );
				if( TrackPopupMenu( lpMenuData->lpDeftItem->hSubMenu,
					TPM_LEFTALIGN,
					rect.left + 16,
					rect.top + GetItemPos( lpMenuData, lpMenuData->lpDeftItem ),//index + 1 ),
					0,
					GetParent(hwnd),
					0 ) == MA_SELECTED )
				{
					lpMenuData->active = MA_SELECTED;
					break;
				}
			}
			else
			{
				lpMenuData->active = MA_SELECTED;
			}
		}
		break;
	case WM_SYSKEYDOWN:		// ϵͳ������Ϣ
		lpMenuData = _GetHMENUPtr( (HMENU)GetWindowLong( hwnd, 0 ) );
		// set quit modal flag
		lpMenuData->active = MA_RETURN;
		PostMessage( (HWND)GetWindowLong( hwnd, 4 ), msg, wParam, lParam );
		break;
	case WM_SETCTLCOLOR:	// ���ÿؼ���ɫ
		return DoSetCtlColor( hwnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:	// �õ��ؼ���ɫ
		return DoGetCtlColor( hwnd, (LPCTLCOLORSTRUCT)lParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hwnd );
	case WM_CREATE:			// ����ʱ��ʼ������
		return DoCreate( hwnd, (LPCREATESTRUCT)lParam );		

	//2005-09-20, add for WS_GROUP by lilin
	case DLGC_WANTALLKEYS:
		return DLGC_WANTALLKEYS;
	//

	default:			// ����Ĭ�ϴ���
	// It's important to do this
	// if your do'nt handle message, you must call DefWindowProc
	// ��Ҫ��ʾ�������㲻��Ҫ������Ϣ��������Ϣ����ϵͳ����Ĭ�ϴ�����ȥ����
	// 
		return DefWindowProc( hwnd, msg, wParam, lParam );
    }
    return 0;
}

// **************************************************
// ������static WORD DoMenuModalLoop( HWND hwnd, _LPMENUDATA lpMenuData, UINT uiFlags )
// ������
//	IN hwnd - ���ھ��
//	IN lpMenuData - �˵���������
//	IN uiFlags - ����
// ����ֵ��
//	����
// ����������
//	
// ����: 
//	
// ************************************************

static WORD DoMenuModalLoop( HWND hwnd, _LPMENUDATA lpMenuData, UINT uiFlags )
{
    MSG msg;
    int retv;
    HWND hOldCapture;
	HWND hOldFocus;
	// ���ô���λ��
	SetWindowPos( hwnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE );	
	// �õ���ǰ���㴰��
    hOldFocus = SetFocus( hwnd );
	// �õ���ǰӵ�����Ĵ���
    hOldCapture = SetCapture( hwnd );
	// 
    lpMenuData->active = 0;
    while( lpMenuData->active == 0 )
    {
        retv = GetMessage( &msg, 0, 0, 0 );
        ASSERT( retv );
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
	// �˵������˳����ָ�
    SetCapture( hOldCapture );
	SetFocus( hOldFocus );
    return lpMenuData->active == MA_RETURN ? FALSE : TRUE;
}

// **************************************************
// ������BOOL WINAPI Menu_TrackPopup( HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hwndOwner, LPCRECT lpcRect )
// ������
//	IN hMenu - �˵����
//	IN uFlags - ��־��������
//				TPM_RETURNCMD - ����ֵΪ�˵�ѡ�� ID
//				TPM_RIGHTALIGN - �Ҷ���
//				TPM_CENTERALIGN - ˮƽ����
//				TPM_BOTTOMALIGN - �׶���
//				TPM_VCENTERALIGN - ��ֱ����
//	IN x - �˵���ʾλ�õ�x�����(��Ļ����)
//	IN y - �˵���ʾλ�õ�y�����(��Ļ����)
//	IN nReserved - ������Ϊ0
//	IN hwndOwner - �ò˵���ӵ����
//	IN lpcRect - ����
// ����ֵ��
//	���� uFlags ��TPM_RETURNCMD ���ҳɹ�������ѡ��Ĳ˵���ĿID;���򣬷���0
//	���� uFlags û��TPM_RETURNCMD ���ҳɹ�������TRUE; ���򣬷���FALSE
// ����������
//	��ʾ�˵����ݲ������û���ѡ��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Menu_TrackPopup( HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hwndOwner, LPCRECT lpcRect )
{
    HWND hwnd;
    RECT rect;
    BOOL retv = FALSE;
    _LPMENUDATA lpMenuData;

    lpMenuData = _GetHMENUPtr( hMenu );

    if( lpMenuData && hwndOwner )
    {
        MENUPARAM mp;
        mp.hMenu = hMenu;
        mp.hOwner = hwndOwner;
		mp.uiFlag = uFlags;
		//	ȷ���˵�����
        CalcMenuRect( hwndOwner, hMenu, &rect, uFlags );

		if( uFlags & TPM_RIGHTALIGN )
			x -= rect.right;	// �Ҷ���
		else if( uFlags & TPM_CENTERALIGN )
			x -= rect.right / 2;	// ˮƽ����
		if( uFlags & TPM_BOTTOMALIGN )
			y -= rect.bottom;	// �׶���
		else if( uFlags & TPM_VCENTERALIGN )
			y -= rect.bottom / 2;	// ��ֱ����
		// �����˵�����
        hwnd = CreateWindowEx( WS_EX_TOPMOST, strClassMenu,
                   "",
                   WS_POPUP ,
                   x, y,
                   rect.right,
                   rect.bottom,
                   hwndOwner,
                   0,
                   (HANDLE)GetWindowLong( hwndOwner, GWL_HINSTANCE ),
                   (LPVOID)&mp );
        if( hwnd )
        {	// �����û���ѡ��
            retv = DoMenuModalLoop( hwnd, lpMenuData, uFlags );
			// �˳������������
            DestroyWindow( hwnd );
			if( (( uFlags & TPM_NONOTIFY ) == 0) && retv == TRUE)
			{	// ��ӵ���߷�֪ͨ��Ϣ
				PostMessage( hwndOwner, WM_COMMAND, lpMenuData->lpDeftItem->wID, 0 );
			}
			if( (uFlags & TPM_RETURNCMD) && retv == TRUE )
			{
				retv = lpMenuData->lpDeftItem->wID;
			}
        }
    }
    return retv;
}

