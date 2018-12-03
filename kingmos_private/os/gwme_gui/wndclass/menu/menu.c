/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：menu class
版本号：1.0.0
开发时期：2000-04-07
作者：李林
修改记录：
    5. LN, 2004-05-19, 将 DEFAULTHEIGHT 该为 GetSystemMetrics( SM_CYMENU )
    4. LN, 2003-08-27, add map
	3. Jami chen  2003.07.04 调整MENU的大小及显示
    2. LN, 2003-06-17, 增加TPM_NONOTIFY & TPM_RETURNCMD & 
	   TPM_RIGHTALIGN & TPM_CENTERALIGN & TPM_BOTTOMALIGN &
	   TPM_VCENTERALIGN
    1.LN 2003-05-06,自绘功能统计width去掉2*16+4的限制
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

// 定义菜单项目结构
// private data struct
typedef struct __MENUITEMINFO {
    DWORD    fType;		// 菜单项目的类型
    DWORD    fState;	// 菜单项目的状态
    DWORD    wID;		// 菜单项目的ID
    HMENU    hSubMenu;	// 如果有的话，该菜单项目的子菜单
    HBITMAP  hbmpChecked;	// 如果有的话，当菜单项目被选取时所显示的位图
    HBITMAP  hbmpUnchecked;	// 如果有的话，当菜单项目没有被选取时所显示的位图
    DWORD    dwItemData;	// 菜单项目数据
    LPTSTR   dwTypeData;	// 由类型（fType）决定的数据
    DWORD    cch;			// 假如菜单项目数据类型为字符串，指定其长度
	UINT     iItemHeight;		// 项目显示高度
	struct __MENUDATA * lpOwnerMenu;	// 菜单项目的拥有者
    struct __MENUITEMINFO * lpNext;		// 下一个菜单项目
} _MENUITEM, FAR *_LPMENUITEM;

// 菜单初始化结构
typedef struct _MENUPARAM
{
    HMENU hMenu;		// 菜单句柄		
    HWND hOwner;		// 菜单拥有者
	UINT uiFlag;		// 创建参数
}MENUPARAM;

// 菜单结构
typedef struct __MENUDATA {
    UINT objType;            // 对象类型 = must OBJ_MENU
	_LPMENUITEM lpDeftItem;	// 当前菜单默认的项目
	_LPMENUITEM lpLastItem;	// 当前菜单项目的最后一个项目
	UINT iItemWidth;		// 菜单项目高度
	UINT nCount;			// 菜单项目数
    UINT active;			// 当前项目活动状况

	COLORREF cl_Text;		// 菜单正常文本前景颜色
	COLORREF cl_TextBk;     // 菜单正常文本的与背景色
	COLORREF cl_Selection;	// 选择文本的前景色
	COLORREF cl_SelectionBk;  // 选择文本的背景色
	COLORREF cl_Disable;		//无效文本的前景色
	COLORREF cl_DisableBk;    // 无效文本的背景色

	//HANDLE hOwnerProcess;		// 菜单的拥有者进程
	_LPMENUITEM lpOwnerItem;	// 菜单拥有的第一个项目
	struct __MENUDATA * lpNext;	// 同级菜单链表
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

static const char strClassMenu[] = "MENU";	// 菜单类名
static _LPMENUDATA lpMenuList;	// 所有菜单资源链表



BOOL WINAPI Menu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags );
BOOL WINAPI Menu_InsertItem( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii );

// **************************************************
// 声明：static _LPMENUDATA _GetHMENUPtr( HMENU hMenu )
// 参数：
//	IN hMenu - 菜单句柄
// 返回值：
//	返回句柄对应的菜单数据指针
// 功能描述：
//	检查菜单数据结构的有效性，假如有效，返回其指针；否则，返回NULL
// 引用: 
//	
// ************************************************
static _LPMENUDATA _GetHMENUPtr( HMENU hMenu )
{
    ASSERT( hMenu );
    if( hMenu )
    {
        ASSERT( ((_LPMENUDATA)hMenu)->objType == OBJ_MENU );
        if( ((_LPMENUDATA)hMenu)->objType == OBJ_MENU )
            return (_LPMENUDATA)hMenu;	// 有效
    }
	SetLastError( ERROR_INVALID_PARAMETER );
    return NULL;
}

// **************************************************
// 声明：static VOID _SetMenuColor( _LPMENUDATA lpMenuData )
// 参数：
//	IN lpMenuData - 菜单数据结构
// 返回值：
//	无
// 功能描述：
//	设置菜单数据
// 引用: 
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
// 声明：static _LPMENUDATA _AllocMenuData( HANDLE hOwnerProcess )
// 参数：
//	IN hOwnerProcess - 菜单的拥有者
// 返回值：
//	假如成功，返回菜单结构指针；否则，返回NULL
// 功能描述：
//	分配并初始化菜单对象结构
// 引用: 
//	
// ************************************************

static _LPMENUDATA _AllocMenuData( VOID )// HANDLE hOwnerProcess )
{
    _LPMENUDATA lpMenuData = malloc( sizeof( _MENUDATA ) ); // 分配结构

    if( lpMenuData )
    {	// 初始化结构
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
		// 加入链表 insert to list
		lpMenuData->lpNext = lpMenuList;
		lpMenuList = lpMenuData;
		// 
    }
    return lpMenuData;
}

// **************************************************
// 声明：static BOOL _FreeMenuData( _LPMENUDATA lpMenuData )
// 参数：
//	IN lpMenuData - 菜单结构指针
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	与_AllocMenuData相反，释放菜单结构
// 引用: 
//	
// ************************************************

static BOOL _FreeMenuData( _LPMENUDATA lpMenuData )
{
	_LPMENUDATA lpPrev, lpMenu = lpMenuList;

    lpPrev = NULL;
	while( lpMenu )
	{
		if( lpMenu == lpMenuData )
		{   // 移出 remove from list
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

    free( lpMenuData );  // 释放
    return TRUE;
}

// **************************************************
// 声明：static _LPMENUITEM _AllocItem(void)
// 参数：
//	无
// 返回值：
//	假如成功，返回有效的菜单项目指针；否则，返回NULL
// 功能描述：
//	分配并初始化菜单项目
// 引用: 
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
// 声明：static void _GetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii )
// 参数：
//	IN lpItem - 菜单项目
//	IN/OUT lpmii - 用于接受菜单信息
// 返回值：
//	无
// 功能描述：
//	得到菜单项目信息
// 引用: 
//	
// ************************************************

static void _GetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii )
{
	  ASSERT( lpItem && lpmii );

	  if( lpmii->fMask & MIIM_TYPE )
	  {	  // 得到类型数据
	      lpmii->fType = lpItem->fType;

		  if( IS_STRING( lpItem->fType ) )
		  {   // 字符串类型
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
	      lpmii->fState = lpItem->fState;	// 得到状态
	  if( lpmii->fMask & MIIM_ID )
	      lpmii->wID = lpItem->wID;		// 得到ID
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
// 声明：static void _SetItemData( _LPMENUITEM lpItem, LPMENUITEMINFO lpmii )
// 参数：
//	OUT lpItem - 接受新设置的菜单属性
//	IN lpmii - 包含需要设置的菜单属性
// 返回值：
//	无
// 功能描述：
//	对菜单项目设置新的属性
// 引用: 
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
		  {   // 包含用户字符串指针，需要重新映射
#ifdef _MAPPOINTER
			  char * lpsz = MapPtrToProcess( lpmii->dwTypeData, GetCallerProcess() );
#else
			  char * lpsz = (char*)lpmii->dwTypeData;
#endif
	          // 如果之前已经有分配的字符串，先释放它
			  if( lpItem->dwTypeData )
				  free( lpItem->dwTypeData );
			  lpItem->dwTypeData = malloc( lpmii->cch + 1 );	// 分配新的
			  strncpy( lpItem->dwTypeData, lpsz, lpmii->cch );	// 拷贝数据
			  *( (LPTSTR)lpItem->dwTypeData + lpmii->cch ) = 0;	//设置结束符
		  }
		  else
		  {
			  lpItem->dwTypeData = 0;	// 类型数据
			  lpItem->dwItemData = lpmii->dwItemData;	// 
		  }
	  }
	  if( lpmii->fMask & MIIM_STATE )
	      lpItem->fState = lpmii->fState;
	  if( lpmii->fMask & MIIM_ID )
	      lpItem->wID = lpmii->wID;
	  if( lpmii->fMask & MIIM_SUBMENU )
	  {		// 需要重新设置子菜单
		  _LPMENUDATA lpSubMenu = _GetHMENUPtr( lpmii->hSubMenu );		  
			// 假如可能，先移出它
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
			  lpSubMenu->lpOwnerItem = lpItem;	// 设置拥有者
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
// 声明：static int _IndexOf( _LPMENUDATA lpMenu, _LPMENUITEM lpItem )
// 参数：
//	IN lpMenu - 菜单结构指针
//	IN lpItem - 项目结构指针
// 返回值：
//	返回索引号,失败，返回-1
// 功能描述：
//	得到一个菜单项在菜单中的索引号（由上向下算，以0为基数）
// 引用: 
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
	         return i;	// 找到，返回索引号
       i++;
    }while( lpTemp != lpMenu->lpLastItem );
    return -1;
}

// **************************************************
// 声明：static int _Count( _LPMENUDATA lpMenu )
// 参数：
//	IN lpMenu - 菜单结构指针
// 返回值：
//	返回菜单的菜单项目数
// 功能描述：
//	得到菜单的菜单项目数
// 引用: 
//	
// ************************************************

static int _Count( _LPMENUDATA lpMenu )
{
	return lpMenu->nCount;
}

// **************************************************
// 声明：static _LPMENUITEM _At( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags, _LPMENUITEM * lppPrev )
// 参数：
//	IN lpMenu - 菜单结构指针
//	IN uPosition - 菜单项目的位置或ID,依赖于 uFlags
//	IN uFlags - 查询方式：
//					MF_BYPOSITION - 通过位置查找菜单项目
//					MF_BYCOMMAND - 通过ID查找菜单项目
//	IN lppPrev - 用于接受匹配项目的前一个项目指针
//	
// 返回值：
//	假如找到，返回菜单项目; 否则，返回NULL
// 功能描述：
//	根据位置或ID得到菜单的一个项目
// 引用: 
//	
// ************************************************

static _LPMENUITEM _At( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags, _LPMENUITEM * lppPrev )
{
    _LPMENUITEM	lpItem;
    DWORD i;

    if( (lpItem = lpMenu->lpLastItem) != 0 )
    {
	      if( uFlags )// MF_BYPOSITION
	      {	// 通过位置查找
	          i = 0;
	          do   {
		            if( lppPrev )
		                *lppPrev = lpItem;
                lpItem = lpItem->lpNext;
                if( i == uPosition )
                    return lpItem;	// 找到
                i++;
            }while( lpItem != lpMenu->lpLastItem );
	      }
	      else
	      {   // 通过ID查找 MF_BYCOMMAND
	          do   {
                if( lppPrev )
		                *lppPrev = lpItem;
		            lpItem = lpItem->lpNext;
                if( lpItem->wID == uPosition )
                    return lpItem;	// 找到
            }while( lpItem != lpMenu->lpLastItem );
        }
    }
    return 0;  // not found
}

// **************************************************
// 声明：static _LPMENUITEM _RemoveItem( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags )
// 参数：
//	IN lpMenu - 菜单指针
//	IN uPosition - 菜单项目的位置或ID,依赖于 uFlags
//	IN uFlags - 查询方式：
//					MF_BYPOSITION - 通过位置查找菜单项目
//					MF_BYCOMMAND - 通过ID查找菜单项目
// 返回值：
//	假如找到，返回非NULL的菜单项目指针；否则，返回NULL
// 功能描述：
//	移出菜单项目对象
// 引用: 
//	
// ************************************************

static _LPMENUITEM _RemoveItem( _LPMENUDATA lpMenu, DWORD uPosition, DWORD uFlags )
{
    _LPMENUITEM lpPrev, lpItem = 0;
	
    if( lpMenu  && lpMenu->lpLastItem )
    {	// 查找菜单项目
		if( _At( lpMenu, uPosition, uFlags, &lpPrev ) )
		{	// 找到
			if( lpPrev == lpMenu->lpLastItem )
			{   // 该菜单仅仅一个菜单项目 only one item
				lpItem = lpPrev;
				lpMenu->lpLastItem = lpMenu->lpDeftItem = 0;
			}
			else
			{	// save item pointer
				lpItem = lpPrev->lpNext;
				// 移出 remove link
				lpPrev->lpNext = lpPrev->lpNext->lpNext;
				if( lpItem == lpMenu->lpDeftItem && lpItem == lpMenu->lpLastItem )
					lpMenu->lpDeftItem = lpMenu->lpLastItem = lpPrev;
				else if( lpItem == lpMenu->lpDeftItem )
					lpMenu->lpDeftItem = lpItem->lpNext;
				else if( lpItem == lpMenu->lpLastItem )
					lpMenu->lpLastItem = lpPrev;
				else
				{	// 不知到的错误
					ASSERT( 0 ); // error
					lpItem = 0;
					goto REV_RET;
				}
			}
			lpItem->lpOwnerMenu = NULL;
			lpMenu->nCount--;	// 减少菜单项目的总拥有数
		}
    }
REV_RET:
    return lpItem;
}

// **************************************************
// 声明：HMENU WINAPI Menu_Create(void)
// 参数：
//	无
// 返回值：
//	假如成功，返回菜单对象句柄；否则，返回NULL
// 功能描述：
//	创建菜单对象，并返回其句柄
// 引用: 
//	系统API
// ************************************************

HMENU WINAPI Menu_Create(void)
{
    _LPMENUDATA lpMenu;

    lpMenu = _AllocMenuData();// GetCallerProcess() );
    return (HMENU)lpMenu;
}

// **************************************************
// 声明：HMENU WINAPI Menu_CreatePopup(void)
// 参数：
//	无
// 返回值：
//	假如成功，返回菜单对象句柄；否则，返回NULL
// 功能描述：
//	创建菜单对象，并返回其句柄
// 引用: 
//	系统API
// ************************************************

HMENU WINAPI Menu_CreatePopup(void)
{
    _LPMENUDATA lpMenu;

    lpMenu = _AllocMenuData();// GetCallerProcess() );
    return (HMENU)lpMenu;
}

// **************************************************
// 声明：BOOL WINAPI Menu_Destroy( HMENU hMenu )
// 参数：
//	IN hMenu - 菜单句柄
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	破坏菜单对象
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Menu_Destroy( HMENU hMenu )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );

    ASSERT( lpMenu );
    if( lpMenu )
	{
		int count = _Count( lpMenu );	// 统计菜单的项目数
		// 删除所有的菜单项目
		count--;
		while( count >= 0 )
		{
			Menu_Delete( hMenu, (DWORD)count, MF_BYPOSITION );
			count--;
		}
		_FreeMenuData( lpMenu );	//释放菜单项目
		return TRUE;
	}
    return FALSE;    
}

// **************************************************
// 声明：void FreeInstanceMenu( HANDLE hOwnerProcess )
// 参数：
//	IN hOwnerProcess - 拥有者进程句柄
// 返回值：
//	无
// 功能描述：
//	释放进程拥有的所有菜单对象
// 引用: 
//	被GwmeServerHandler调用下来
// ************************************************

// call by WinSys_CloseObject
/*
void FreeInstanceMenu( HANDLE hOwnerProcess )
{
	_LPMENUDATA lpMenu = lpMenuList;

	while( lpMenu )
	{
		if( lpMenu->hOwnerProcess == hOwnerProcess )
		{   // 菜单对象被该进程拥有，得到菜单的顶级菜单 get top level menu
			while( lpMenu->lpOwnerItem )
			{	// 
				lpMenu = lpMenu->lpOwnerItem->lpOwnerMenu;
				ASSERT( lpMenu );
			}
			// 破坏
			Menu_Destroy( (HMENU)lpMenu );
			// restart
			lpMenu = lpMenuList;
		}
		else
			lpMenu = lpMenu->lpNext;	// 下一个菜单
	}
}
*/
// **************************************************
// 声明：BOOL WINAPI Menu_Remove( HMENU hMenu, DWORD uPosition, DWORD uFlags )
// 参数：
//	IN hMenu - 菜单句柄
//	IN uPosition - 菜单项目的位置或ID,依赖于 uFlags
//	IN uFlags - 查询方式：
//					MF_BYPOSITION - 通过位置查找菜单项目
//					MF_BYCOMMAND - 通过ID查找菜单项目
//	
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	将一个菜单项目从菜单移出
// 引用: 
//	系统API
// ************************************************

//deletes a menu item from the specified menu
BOOL WINAPI Menu_Remove( HMENU hMenu, DWORD uPosition, DWORD uFlags )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    if( lpMenu )
    {        
        _LPMENUITEM lpItem;
		// 移出
        if( (lpItem = _RemoveItem( lpMenu, uPosition, uFlags )) != 0 )
        {	
			if( lpItem->hSubMenu )
			{   // 如果该菜单项有子菜单，则清除子菜单的拥有者 remove the submenu from the item
				_LPMENUDATA lpSubMenu = _GetHMENUPtr( lpItem->hSubMenu );
				if( lpSubMenu )
				{
					lpSubMenu->lpOwnerItem = NULL;
				}
			}
			// 如果该菜单项目类型为字符串，则释放它
            if( IS_STRING( lpItem->fType ) && lpItem->dwTypeData )
                free( lpItem->dwTypeData );
            free( lpItem );	// 释放项目
        }
        return (lpItem != 0);
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Menu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags )
// 参数：
//	IN hMenu - 菜单对象句柄
//	IN uPosition - 菜单项目的位置或ID,依赖于 uFlags
//	IN uFlags - 查询方式：
//					MF_BYPOSITION - 通过位置查找菜单项目
//					MF_BYCOMMAND - 通过ID查找菜单项目
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除菜单对象
// 引用: 
//	系统API
// ************************************************

//deletes a menu item from the specified menu and free mem
BOOL WINAPI Menu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );

    ASSERT( lpMenu );    
    if( lpMenu )
    {	// 先移出
        _LPMENUITEM lpItem = _RemoveItem( lpMenu, uPosition, uFlags );
        if( lpItem )
        {
            // 假如有子菜单，先破坏它 destroy submenu if possible
            if( lpItem->hSubMenu )
			{
                Menu_Destroy( lpItem->hSubMenu );
			}
            // 假如菜单项目为字符串类型，先释放字符串 free item from mem
            if( IS_STRING( lpItem->fType ) && lpItem->dwTypeData )
                free( lpItem->dwTypeData );
            free( lpItem );
        }
        return (lpItem != 0);
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Menu_Insert(  HMENU hMenu,  DWORD uPosition,  DWORD uFlags,  DWORD uIDNewItem,  LPCTSTR lpNewItem )
// 参数：
//	IN hMenu - 菜单句柄对象
//	IN uPosition - 菜单项目的位置或ID,依赖于 uFlags
//	IN uFlags - 查询方式：
//					MF_BYPOSITION - 通过位置查找菜单项目
//					MF_BYCOMMAND - 通过ID查找菜单项目
//					MF_STRING - 菜单项目内容为字符串
//					
//	IN uIDNewItem - 菜单ID
//  IN lpNewItem - 菜单项目内容，依赖于 uFlags标志
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	在 菜单中插入新的菜单项目
// 引用: 
//	系统API
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
		{	// 字符串类型
#ifdef _MAPPOINTER
			lpNewItem = (LPCTSTR)MapPtrToProcess( (LPVOID)lpNewItem, GetCallerProcess() );
#else
			//lpNewItem = (LPCTSTR)MapPtrToProcess( (LPVOID)lpNewItem, GetCallerProcess() );
#endif
		}
		// 初始化结构数据
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
        // 插入
        return Menu_InsertItem( hMenu, uPosition, (BOOL)(uFlags & MF_BYPOSITION), &mii );
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Menu_InsertItem( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii )
// 参数：
//	IN hMenu - 菜单对象句柄
//	IN uItem - ID 或 位置，依赖于 fByPosition
//	IN fByPosition - 假如为TRUE, 意味uItem表示位置；否则，uItem表示ID
//  IN lpmii - 包含菜单信息的MENUITEMINFO数据结构指针
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	插入一个菜单项目
// 引用: 
//	系统API
// ************************************************
 
BOOL WINAPI Menu_InsertItem( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    ASSERT( lpmii );
    if( lpMenu && lpmii )
    {       
        _LPMENUITEM lpPrev, lpItem;
        
        lpItem = _AllocItem();	// 分配菜单结构

        if( lpItem )
        {	// 初始化数据
            _SetItemData( lpItem, lpmii );
			//	设置拥有者
			lpItem->lpOwnerMenu = lpMenu;
			//	
            if( lpMenu->lpLastItem )
            {	// 得到该位置/ID的菜单项目
                if( _At( lpMenu, uItem, fByPosition ? MF_BYPOSITION : MF_BYCOMMAND, &lpPrev ) )
                {	// 找到
                    lpItem->lpNext = lpPrev->lpNext;
                    lpPrev->lpNext = lpItem;
                }
                else  // put bottom
                {	// 没有找到，放到底部
                    lpItem->lpNext = lpMenu->lpLastItem->lpNext;
                    lpMenu->lpLastItem->lpNext = lpItem;
                    lpMenu->lpLastItem = lpItem;
                }
            }
            else    // empty menu
            {	// 当前菜单没有任何菜单项目
                lpItem->lpNext = lpItem;
                lpMenu->lpLastItem = lpMenu->lpDeftItem = lpItem;
            }
			lpMenu->nCount++;	// 当前菜单的项目总数
        }
        return (BOOL)(lpItem != NULL);
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Menu_EnableItem( HMENU hMenu, DWORD uPosition, DWORD uEnable )
// 参数：
//	IN hMenu - 菜单对象
//	IN uPosition - ID 或 位置，依赖于 fByPosition
//	IN uEnable - 对菜单项目的操作，为以下内容：
//					MF_BYCOMMAND - 说明 uPosition 表示菜单项目ID
//					MF_BYPOSITION - 说明 uPosition 表示菜单项目ID
//					MF_DISABLED - 说明 菜单项目 应该为无效状态
//					MF_ENABLED - 说明 菜单项目 应该为有效状态
//					MF_GRAYED - 说明 菜单项目 应该为有效和灰色状态
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	设置菜单状态
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Menu_EnableItem( HMENU hMenu, DWORD uPosition, DWORD uEnable )
{
    BOOL retv = FALSE;
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    if( lpMenu )
    {       
        _LPMENUITEM lpItem;
        // 查找
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
// 声明：DWORD WINAPI Menu_GetDefaultItem( HMENU hMenu, DWORD fByPos, DWORD uFlags )
// 参数：
//	IN hMenu - 菜单对象句柄
//	IN fByPos - 说明返回菜单项目的位置或是ID.假如为TRUE,返回位置；否则，返回ID
//	IN uFlags - 不支持，必须为0
// 返回值：
//	返回当前默认的菜单项目；假如失败，返回-1
// 功能描述：
//	得到当前菜单默认的菜单项目并返回对应的值
// 引用: 
//	系统API
// ************************************************

DWORD WINAPI Menu_GetDefaultItem( HMENU hMenu, DWORD fByPos, DWORD uFlags )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );    
    if( lpMenu && lpMenu->lpDeftItem )
    {
        if( fByPos )
            return lpMenu->lpDeftItem->wID;	// 返回ID值
        else
            return _IndexOf( lpMenu,lpMenu->lpDeftItem );	// 返回位置值
    }
    return -1;
}

// **************************************************
// 声明：int WINAPI Menu_GetItemCount( HMENU hMenu )
// 参数：
//	IN hMenu - 菜单对象句柄
// 返回值：
//	假如成功，返回当前菜单项目数；假如失败，返回-1
// 功能描述：
//	得到菜单所拥有的项目数
// 引用: 
//	系统API
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
// 声明：int WINAPI Menu_GetItemID( HMENU hMenu, int nPos )
// 参数：
//	IN hMenu - 菜单对象句柄 
//	IN nPos - 菜单项目位置
// 返回值：
//	假如成功，返回指定菜单项目的ID; 否则，返回-1
// 功能描述：
//	得到菜单项目的ID
// 引用: 
//	系统API
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
// 声明：BOOL WINAPI Menu_GetItemInfo( 
//								HMENU hMenu, 
//								DWORD uItem, 
//								BOOL fByPosition, 
//								LPMENUITEMINFO lpmii )
// 参数：
//	IN hMenu - 菜单对象句柄
//	IN uItem - 菜单项目标识（具体含义由fByPosition决定）
//	IN fByPosition - 表示 uItem 是表示菜单项目位置或是菜单项目. 假如为TRUE, 表示uItem代表位置
//	IN/OUT lpmii - 用于接受菜单项目信息 
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	得到菜单项目的信息
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Menu_GetItemInfo( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    
    ASSERT( lpMenu );
    ASSERT( lpmii );
    if( lpMenu && lpmii )
    {   // 得到菜单项目对象  
        _LPMENUITEM lpItem = _At( lpMenu, uItem, fByPosition ? MF_BYPOSITION : MF_BYCOMMAND, 0 );
        if( lpItem )
            _GetItemData( lpItem, lpmii );	// 得到信息
        return (BOOL)(lpItem != NULL);
    }
	return FALSE;
}

// **************************************************
// 声明：HMENU WINAPI Menu_GetSub( HMENU hMenu, int nPos )
// 参数：
//	IN hMenu - 菜单对象句柄
//	IN nPos - 菜单项目位置
// 返回值：
//	假如成功，返回指定菜单项目所拥有的子菜单；否则，返回NULL
// 功能描述：
//	得到菜单项目所拥有的子菜单
// 引用: 
//	系统API
// ************************************************

HMENU WINAPI Menu_GetSub( HMENU hMenu, int nPos )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );

    ASSERT( lpMenu );
    if( lpMenu )
	{
        _LPMENUITEM lpItem = 0;
        lpItem = _At( lpMenu, nPos, MF_BYPOSITION, 0 );  // 查找菜单项目
        return lpItem ? lpItem->hSubMenu : (HMENU)NULL;
	}
    return (HMENU)NULL;
}

// **************************************************
// 声明：BOOL WINAPI Menu_SetDefaultItem( HMENU hMenu, DWORD uItem, DWORD fByPos )
// 参数：
//	IN hMenu - 菜单对象句柄 
//	IN uItem - 菜单项目标识（具体含义由参数 fByPos 决定）
//	IN fByPos - 决定uItem的含义。假如为TRUE,表示uItem为菜单项目位置；否则，表示菜单项目ID
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到当前菜单的默认项目
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Menu_SetDefaultItem( HMENU hMenu, DWORD uItem, DWORD fByPos )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    _LPMENUITEM lpItem = 0;
    
    ASSERT( lpMenu );
    if( lpMenu )
    {
        lpItem = _At( lpMenu, uItem, fByPos, 0 );	// 查找菜单项目
        if( lpItem )
            lpMenu->lpDeftItem = lpItem;	// 设为默认值
    }
    return lpItem != 0;
}

// **************************************************
// 声明：BOOL WINAPI Menu_SetItemInfo( HMENU hMenu, DWORD uItem, BOOL fByPos, LPMENUITEMINFO lpmii )
//	IN hMenu - 菜单对象句柄
//	IN uItem - 菜单项目标识（具体含义由fByPosition决定）
//	IN fByPos - 表示 uItem 是表示菜单项目位置或是菜单项目. 假如为TRUE, 表示uItem代表位置
//	IN lpmii - 用于需要设置的菜单项目信息 
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	设置菜单项目的信息
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Menu_SetItemInfo( HMENU hMenu, DWORD uItem, BOOL fByPos, LPMENUITEMINFO lpmii )
{
    _LPMENUDATA lpMenu = _GetHMENUPtr( hMenu );
    _LPMENUITEM lpItem = 0;
    
    ASSERT( lpMenu );
    ASSERT( lpmii );
    if( lpMenu && lpmii )
    {	
        lpItem = _At( lpMenu, uItem, fByPos, 0 );	// 查找菜单项目
        if( lpItem )
            _SetItemData( lpItem, lpmii );	// 设置信息
    }
    return lpItem != 0;	//
}

// **************************************************
// 声明：ATOM RegisterMenuClass( HINSTANCE hInst )
// 参数：
//	IN hInst - 实例句柄
// 返回值：
//	假如成功，返回原子；否则，返回0
// 功能描述：
//	注册菜单类
// 引用: 
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
// 声明：static int GetItemPos( _LPMENUDATA lpMenu, _LPMENUITEM lpItem )
// 参数：
//	IN lpMenu - MENUDATA结构对象指针
//	IN lpItem - 菜单项目指针
// 返回值：
//	返回菜单项目位置
// 功能描述：
//	得到菜单项目在菜单中的y坐标位置
// 引用: 
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
// 声明：static int GetItemBound( _LPMENUDATA lpMenu, _LPMENUITEM lpItem, LPRECT lprect )
// 参数：
//	IN lpMenu - MENUDATA结构对象指针
//	IN lpItem - 菜单项目指针
//	OUT lprect - 用于接受菜单项目矩形 
// 返回值：
//	返回TRUE
// 功能描述：
//	得到菜单项目矩形
// 引用: 
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
// 声明：static _LPMENUITEM GetItemPtr( _LPMENUDATA lpMenu, POINT pt )
// 参数：
//	IN lpMenu - MENUDATA结构对象指针
//	IN pt - 客户点坐标
// 返回值：
//	假如成功，返回菜单项目指针；否则，返回NULL
// 功能描述：
//	得到点坐标对应的菜单项目指针
// 引用: 
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
				break;	// 匹配
			i += lpCurItem->iItemHeight;
			lpCurItem = lpCurItem->lpNext;	// 下一个菜单项目
		}
		return lpCurItem;	// 
	}
	return NULL;
}

// **************************************************
// 声明：static void CalcMenuRect( HWND hwndOwner, HMENU hMenu, LPRECT lpRect, UINT uiFlags )
// 参数：
//	IN hwndOwner - 窗口句柄
//	IN hMenu - 菜单句柄
//	OUT lpRect - 用于接受菜单的矩形
//	IN uiFlags - 保留
// 返回值：
//	无
// 功能描述：
//	得到菜单的显示矩形
// 引用: 
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

	lpMenu = _GetHMENUPtr( hMenu );	// 得到句柄对应的对象指针
	//count = lpMenu->nCount;
	if( (count = lpMenu->nCount) )
	    lpItem = lpMenu->lpLastItem->lpNext; //第一个

	if( lpMenu->iItemWidth == 0 )
	    maxx = 1;//2 * 8 + 4;
	else
		maxx = lpMenu->iItemWidth;
	maxy = 1;
	
	for( i = 0; i < count; i++ )
	{
		if( (lpItem->fType & MFT_OWNERDRAW) )
		{	// 自绘风格
		    if( (lpItem->fState & MFS_UPDATE_OWNER_DRAW) == 0 )
			{	// 从菜单的窗口拥有者得到自定义坐标
				mis.CtlID = 0;
				mis.CtlType = ODT_MENU;
				mis.itemData = lpItem->dwItemData;
				mis.itemHeight = cyItem;//16;
				mis.itemID = lpItem->wID;
				mis.itemWidth = 16;
				SendMessage( hwndOwner, WM_MEASUREITEM, 0, (LPARAM)&mis );
				lpItem->fState |= MFS_UPDATE_OWNER_DRAW;
				lpItem->iItemHeight = mis.itemHeight;
				maxx = MAX( maxx, mis.itemWidth );	// 得到最大的宽度
			}
		}
		else if( IS_STRING( lpItem->fType ) )
		{	// 字符串风格
			UINT ex;
			lpItem->iItemHeight = cyItem;  // 用默认高度 default height
			ex = 2 * 16 + 4 + lpItem->cch * nCharWidth;//8;	
			maxx = MAX( maxx, ex );	// 得到最大的宽度
		}
		else
		{
			lpItem->iItemHeight = cyItem;  // 用默认高度 default height
		}
		maxy += lpItem->iItemHeight;
		lpItem = lpItem->lpNext;
	}
	// 设置返回值
	lpRect->left = 0;
	lpRect->top = 0;
	lpRect->right = maxx + 2 * EDGEWIDTH;      // modified By Jami chen 2003.06.18
	//lpRect->bottom = maxy ;
	lpRect->bottom = maxy + 2 * EDGEWIDTH + 2 * VERTBANK; // modified By Jami chen 2003.06.18
	lpMenu->iItemWidth = maxx;

}

// **************************************************
// 声明：static int GetFormatString( _LPMENUITEM lpItem, char * lpbuf )
// 参数：
//	IN lpItem - 菜单项目指针
//	OUT lpbuf - 用于接受格式化的字符串
// 返回值：
//	返回字符串的总长度
// 功能描述：
//	格式化菜单项目字符串
// 引用: 
//	
// ************************************************

static int GetFormatString( _LPMENUITEM lpItem, char * lpbuf )
{
    int retv;
    strncpy( lpbuf, lpItem->dwTypeData, 29 );
    retv = (int)lpItem->cch;
    if( lpItem->hSubMenu )
    {
        strcat( lpbuf, " " );	//子菜单的箭头 例如： "menu item    -> "
        retv += 2;
    }
    return retv;
}

// **************************************************
// 声明：static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
// 参数：
//	IN hdc - 绘图DC句柄
//	IN lprc - 需要填充的矩形
//	IN clr - 颜色值
// 返回值：
//	无
// 功能描述：
//	填充矩形
// 引用: 
//	
// ************************************************

static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

// **************************************************
// 声明：static void DrawMenuItem( 
//						HWND hwnd, 
//						_LPMENUDATA lpMenu, 
//						_LPMENUITEM lpItem,
//						HDC hdc,						 
//						LPCRECT lpClipRect, 
//						BOOL bDefault )

// 参数：
//	IN hwnd - 窗口句柄 
//	IN lpMenu - 菜单对象指针
//	IN lpItem - 菜单项目指针
//	IN hdc - 绘图DC句柄
//	IN lpClipRect - 菜单矩形指针
//	IN bDefault - 表示该菜单项目是否为当前默认菜单项
// 返回值：
//	无
// 功能描述：
//	画菜单项目
// 引用: 
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
	{	// 自绘风格
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
		// 得到菜单项目的矩形边界
	    GetItemBound( lpMenu, lpItem, &dis.rcItem );
		// 发绘制消息
		SendMessage( GetParent(hwnd), WM_DRAWITEM, 0, (LPARAM)&dis );
		return;
	}
	else
	{   // 得到菜单项目的矩形边界
	   GetItemBound( lpMenu, lpItem, &rect );

       if( lpItem->fType & MFT_SEPARATOR )
	   {	// 该菜单项目是一个分割线
		   HPEN hPen;
		   // 创建并选择笔
		   hPen = CreatePen(PS_SOLID,1,MENUDARKSHADE);
		   hPen = SelectObject(hdc,hPen);
		   // 设置绘制起点
		   MoveToEx( hdc, rect.left+4, (rect.top + rect.bottom)/2, NULL );
		   // 画黑线
		   LineTo( hdc, rect.right-4, (rect.top + rect.bottom)/2 );
		   hPen = SelectObject(hdc,hPen);
		   DeleteObject(hPen);
			//
		   hPen = CreatePen(PS_SOLID,1,MENULIGHTSHADE);
		   hPen = SelectObject(hdc,hPen);
		   MoveToEx( hdc, rect.left+4, (rect.top + rect.bottom)/2+1, NULL );
		   // 画亮线
		   LineTo( hdc, rect.right-4, (rect.top + rect.bottom)/2+1 );
		   hPen = SelectObject(hdc,hPen);
		   DeleteObject(hPen);

	   }
       else if( IS_STRING( lpItem->fType ) )
       {	// 项目是字符串
		   COLORREF dwOldTextColor;
		   
		   if( lpItem->fState & (MFS_GRAYED|MFS_DISABLED) )
		   {	// 当前状态为灰/无效
			   FillSolidRect( hdc, &rect, lpMenu->cl_TextBk );
			   dwOldTextColor = SetTextColor( hdc, lpMenu->cl_Disable );
		   }
		   else if( bDefault )
		   {	// 默认项目
			   FillSolidRect( hdc, &rect, lpMenu->cl_SelectionBk );
			   dwOldTextColor = SetTextColor( hdc, lpMenu->cl_Selection );
		   }
		   else
		   {	// 普通项目
			   FillSolidRect( hdc, &rect, lpMenu->cl_TextBk );
			   dwOldTextColor = SetTextColor( hdc, lpMenu->cl_Text );
		   }

		   rect.left += 16;
		   rect.right -= 16;

		   SetBkMode( hdc, TRANSPARENT );
		   //	显示文本
           DrawText( hdc, buf, GetFormatString( lpItem, buf ), &rect, DT_SINGLELINE | DT_VCENTER );
		   SetTextColor( hdc, dwOldTextColor );	// 恢复
       }
    }
}

// **************************************************
// 声明：static void DrawMenu( HWND hwnd, HDC hdc )
// 参数：
//	IN hwnd - 窗口句柄
//	IN hdc - 绘图DC
// 返回值：
//	无
// 功能描述：
//	画菜单
// 引用: 
//	
// ************************************************

static void DrawMenu( HWND hwnd, HDC hdc )
{
    _LPMENUDATA lpMenu;
    _LPMENUITEM lpItem;
    HMENU hMenu;
    int count, i;
    RECT rect;

	hMenu = (HMENU)GetWindowLong( hwnd, 0 );	// 从窗口用户数据区得到菜单句柄
    lpMenu = _GetHMENUPtr( hMenu );	// 由菜单句柄得到菜单对象指针

    GetClientRect( hwnd, &rect );	// 得到菜单矩形指针
	InflateRect(&rect,(0-EDGEWIDTH),(0-EDGEWIDTH));  // Add By Jami chen in 2003.06.17 for will to Draw Edge
    if( lpMenu && lpMenu->lpLastItem )
    {	// 
		count = lpMenu->nCount;//菜单项目总数；//GetMenuItemCount( hMenu );
        lpItem = lpMenu->lpLastItem->lpNext;
        for( i = 0; i < count; i++ )
        {	// 画每一个菜单项目
			DrawMenuItem( hwnd, lpMenu, lpItem, hdc, &rect, lpItem == lpMenu->lpDeftItem );
            lpItem = lpItem->lpNext;
        }
    }
}

// **************************************************
// 声明：static _LPMENUITEM DoPosChange( HWND hwnd, short x, short y )
// 参数：
//	IN hwnd - 菜单句柄
//	IN x - 当前鼠标在菜单窗口区域的x坐标
//	IN y - 当前鼠标在菜单窗口区域的y坐标
// 返回值：
//	假如命中，返回鼠标所在的菜单条目；否则，返回NULL
// 功能描述：
//  将鼠标命中的菜单条目设为当前默认项目	
// 引用: 
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
	// 得到客户区矩形
    GetClientRect( hwnd, &rect );
	// 除去边界厚度
	InflateRect(&rect,(0-EDGEWIDTH),(0-EDGEWIDTH));  // Add By Jami chen in 2003.06.17 for will to Draw Edge

    hMenu = (HMENU)GetWindowLong( hwnd, 0 );
    lpMenu = _GetHMENUPtr( hMenu );

    if( PtInRect( &rect, pt ) )
    {
		lpItem = GetItemPtr( lpMenu, pt );	// 得到命中项
    }
    if( lpItem != lpMenu->lpDeftItem )
    {	// 得到绘图DC
        hdc = GetDC( hwnd );
        // 重绘当前的默认项为普通状态 reset old hilight item
        if( lpMenu->lpDeftItem )
			DrawMenuItem( hwnd, lpMenu, lpMenu->lpDeftItem, hdc, &rect, FALSE );
        // 设置当前命中项为默认项目
		lpMenu->lpDeftItem = lpItem;
        // 刷新默认项 new hilight item if possible
        if( lpItem )
			DrawMenuItem( hwnd, lpMenu, lpItem, hdc, &rect, TRUE );
        ReleaseDC( hwnd, hdc );	//
    }
    return lpItem;
}

// **************************************************
// 声明：static LRESULT DoCreate( HWND hwnd, LPCREATESTRUCT lpcs )
// 参数：
//	IN hwnd - 窗口句柄
//	IN lpcs - 创建结构
// 返回值：
//	返回0
// 功能描述：
//	初始化窗口的菜单数据
// 引用: 
//	处理 WM_CREATE 消息
// ************************************************

static LRESULT DoCreate( HWND hwnd, LPCREATESTRUCT lpcs )
{
	SetWindowLong( hwnd, 0, (LONG)((MENUPARAM*)lpcs->lpCreateParams)->hMenu );
	SetWindowLong( hwnd, 4, (LONG)((MENUPARAM*)lpcs->lpCreateParams)->hOwner );
	SetWindowLong( hwnd, 8, (LONG)((MENUPARAM*)lpcs->lpCreateParams)->uiFlag );
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
	{
		HMENU hMenu;
		_LPMENUDATA lpMenu;
		
		hMenu = (HMENU)GetWindowLong( hWnd, 0 );
		lpMenu = _GetHMENUPtr( hMenu );
		
		if( lpMenu && lpMenu->lpLastItem )
		{
			if( lpccs->fMask & CLF_TEXTCOLOR )
				lpMenu->cl_Text = lpccs->cl_Text;	// 文本颜色
			if( lpccs->fMask & CLF_TEXTBKCOLOR )
				lpMenu->cl_TextBk = lpccs->cl_TextBk;	// 文本背景
			if( lpccs->fMask & CLF_SELECTIONCOLOR )
				lpMenu->cl_Selection = lpccs->cl_Selection;	// 被选择的菜单项目文本
			if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
				lpMenu->cl_SelectionBk = lpccs->cl_SelectionBk;	// 被选择的菜单项目文本背景
			if( lpccs->fMask & CLF_DISABLECOLOR )
				lpMenu->cl_Disable = lpccs->cl_Disable;	// 无效菜单项目文本
			if( lpccs->fMask & CLF_DISABLEBKCOLOR )
				lpMenu->cl_DisableBk = lpccs->cl_DisableBk;	// 无效的菜单项目文本背景		
			return TRUE;
		}
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
	{
		HMENU hMenu;
		_LPMENUDATA lpMenu;
		
		hMenu = (HMENU)GetWindowLong( hWnd, 0 );
		lpMenu = _GetHMENUPtr( hMenu );

		if( lpccs->fMask & CLF_TEXTCOLOR )
		    lpccs->cl_Text = lpMenu->cl_Text;	// 文本颜色
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		    lpccs->cl_TextBk = lpMenu->cl_TextBk;	// 文本背景
		if( lpccs->fMask & CLF_SELECTIONCOLOR )
		    lpccs->cl_Selection = lpMenu->cl_Selection;// 被选择的菜单项目文本
		if( lpccs->fMask & CLF_SELECTIONBKCOLOR )
		    lpccs->cl_SelectionBk = lpMenu->cl_SelectionBk;// 被选择的菜单项目文本背景
		if( lpccs->fMask & CLF_DISABLECOLOR )
		    lpccs->cl_Disable = lpMenu->cl_Disable;	// 无效菜单项目文本
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
		    lpccs->cl_DisableBk = lpMenu->cl_DisableBk;// 无效的菜单项目文本背景
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
	HMENU hMenu;
	_LPMENUDATA lpMenu;
	
	hMenu = (HMENU)GetWindowLong( hWnd, 0 );
	lpMenu = _GetHMENUPtr( hMenu );
	_SetMenuColor( lpMenu );
	return 0;
}

// **************************************************
// 声明：static LRESULT DoEraseBkgnd( HWND hWnd,HDC hdc )
// 参数：
//	IN hWnd - 窗口句柄
//	IN hdc - 绘图DC
// 返回值：
//	返回TRUE
// 功能描述：
//	绘制窗口背景
// 引用: 
//	处理 WM_ERASEBKGND 消息
// ************************************************

static LRESULT DoEraseBkgnd( HWND hWnd,HDC hdc )
{
	RECT rect;
    _LPMENUDATA lpMenu;
    HMENU hMenu;
	HPEN hPen;
	
	hMenu = (HMENU)GetWindowLong( hWnd, 0 );
    lpMenu = _GetHMENUPtr( hMenu );
	// 得到客户区	
	GetClientRect(hWnd,&rect);
	// 填充背景色
	FillSolidRect( hdc, &rect, lpMenu->cl_TextBk );
	// 画外框
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
// 声明：static LRESULT WINAPI MenuWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
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

static LRESULT CALLBACK MenuWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HDC hdc;
    PAINTSTRUCT ps;
	_LPMENUITEM lpItem;
    _LPMENUDATA lpMenuData;
    RECT rect;

    switch( msg )
    {
	case WM_PAINT:		// 绘制客户区
		hdc = BeginPaint( hwnd, &ps );
		DrawMenu( hwnd, hdc );
		EndPaint( hwnd, &ps );
		break;
	case WM_ERASEBKGND:		// 绘制背景
		DoEraseBkgnd( hwnd,( HDC )wParam );
		return 0;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
		// 处理鼠标下笔消息
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
	case WM_LBUTTONUP:		// 处理鼠标抬笔消息 
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
	case WM_SYSKEYDOWN:		// 系统键盘消息
		lpMenuData = _GetHMENUPtr( (HMENU)GetWindowLong( hwnd, 0 ) );
		// set quit modal flag
		lpMenuData->active = MA_RETURN;
		PostMessage( (HWND)GetWindowLong( hwnd, 4 ), msg, wParam, lParam );
		break;
	case WM_SETCTLCOLOR:	// 设置控件颜色
		return DoSetCtlColor( hwnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:	// 得到控件颜色
		return DoGetCtlColor( hwnd, (LPCTLCOLORSTRUCT)lParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hwnd );
	case WM_CREATE:			// 创建时初始化窗口
		return DoCreate( hwnd, (LPCREATESTRUCT)lParam );		

	//2005-09-20, add for WS_GROUP by lilin
	case DLGC_WANTALLKEYS:
		return DLGC_WANTALLKEYS;
	//

	default:			// 窗口默认处理
	// It's important to do this
	// if your do'nt handle message, you must call DefWindowProc
	// 重要提示：假如你不需要处理消息，将该消息交由系统窗口默认处理函数去处理
	// 
		return DefWindowProc( hwnd, msg, wParam, lParam );
    }
    return 0;
}

// **************************************************
// 声明：static WORD DoMenuModalLoop( HWND hwnd, _LPMENUDATA lpMenuData, UINT uiFlags )
// 参数：
//	IN hwnd - 窗口句柄
//	IN lpMenuData - 菜单对象数据
//	IN uiFlags - 无用
// 返回值：
//	返回
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static WORD DoMenuModalLoop( HWND hwnd, _LPMENUDATA lpMenuData, UINT uiFlags )
{
    MSG msg;
    int retv;
    HWND hOldCapture;
	HWND hOldFocus;
	// 设置窗口位置
	SetWindowPos( hwnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE );	
	// 得到当前焦点窗口
    hOldFocus = SetFocus( hwnd );
	// 得到当前拥有鼠标的窗口
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
	// 菜单窗口退出，恢复
    SetCapture( hOldCapture );
	SetFocus( hOldFocus );
    return lpMenuData->active == MA_RETURN ? FALSE : TRUE;
}

// **************************************************
// 声明：BOOL WINAPI Menu_TrackPopup( HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hwndOwner, LPCRECT lpcRect )
// 参数：
//	IN hMenu - 菜单句柄
//	IN uFlags - 标志，包含：
//				TPM_RETURNCMD - 返回值为菜单选择 ID
//				TPM_RIGHTALIGN - 右对齐
//				TPM_CENTERALIGN - 水平居中
//				TPM_BOTTOMALIGN - 底对齐
//				TPM_VCENTERALIGN - 垂直居中
//	IN x - 菜单显示位置的x坐标点(屏幕坐标)
//	IN y - 菜单显示位置的y坐标点(屏幕坐标)
//	IN nReserved - 保留，为0
//	IN hwndOwner - 该菜单的拥有者
//	IN lpcRect - 忽略
// 返回值：
//	假如 uFlags 有TPM_RETURNCMD 并且成功，返回选择的菜单项目ID;否则，返回0
//	假如 uFlags 没有TPM_RETURNCMD 并且成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	显示菜单内容并跟踪用户的选择
// 引用: 
//	系统API
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
		//	确定菜单矩形
        CalcMenuRect( hwndOwner, hMenu, &rect, uFlags );

		if( uFlags & TPM_RIGHTALIGN )
			x -= rect.right;	// 右对齐
		else if( uFlags & TPM_CENTERALIGN )
			x -= rect.right / 2;	// 水平居中
		if( uFlags & TPM_BOTTOMALIGN )
			y -= rect.bottom;	// 底对齐
		else if( uFlags & TPM_VCENTERALIGN )
			y -= rect.bottom / 2;	// 垂直居中
		// 创建菜单窗口
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
        {	// 跟踪用户的选择
            retv = DoMenuModalLoop( hwnd, lpMenuData, uFlags );
			// 退出，做清除工作
            DestroyWindow( hwnd );
			if( (( uFlags & TPM_NONOTIFY ) == 0) && retv == TRUE)
			{	// 向拥有者发通知消息
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

