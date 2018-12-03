/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __MENUSRV_H
#define __MENUSRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#undef CreateMenu
#define CreateMenu WinMenu_Create
HMENU WINAPI WinMenu_Create(void);

#undef CreatePopupMenu
#define CreatePopupMenu WinMenu_CreatePopup
HMENU WINAPI WinMenu_CreatePopup(void);

#undef DestroyMenu
#define DestroyMenu WinMenu_Destroy
BOOL WINAPI WinMenu_Destroy( HMENU hMenu );

#undef RemoveMenu
#define RemoveMenu WinMenu_Remove
BOOL WINAPI WinMenu_Remove( HMENU hMenu, DWORD uPosition, DWORD uFlags );

#undef DeleteMenu
#define DeleteMenu WinMenu_Delete
BOOL WINAPI WinMenu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags );

#undef InsertMenu
#define InsertMenu WinMenu_Insert
BOOL WINAPI WinMenu_Insert(  HMENU hMenu,  DWORD uPosition,  DWORD uFlags,  DWORD uIDNewItem,  LPCTSTR lpNewItem );

#undef InsertMenuItem 
#define InsertMenuItem WinMenu_InsertItem
BOOL WINAPI WinMenu_InsertItem( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii );

#undef EnableMenuItem
#define EnableMenuItem WinMenu_EnableItem
BOOL WINAPI WinMenu_EnableItem( HMENU hMenu, DWORD uPosition, DWORD uEnable );

#undef GetDefaultMenuItem
#define GetDefaultMenuItem WinMenu_GetDefaultItem
DWORD WINAPI WinMenu_GetDefaultItem( HMENU, DWORD fByPos, DWORD uFlags );

#undef SetDefaultMenuItem
#define SetDefaultMenuItem WinMenu_SetDefaultItem
BOOL WINAPI WinMenu_SetDefaultItem( HMENU, DWORD uItem, DWORD fByPos );

#undef SetMenuItemInfo
#define SetMenuItemInfo WinMenu_SetItemInfo
BOOL WINAPI WinMenu_SetItemInfo( HMENU, DWORD uItem, BOOL fByPos, LPMENUITEMINFO lpmii );

#undef GetMenuItemCount
#define GetMenuItemCount WinMenu_GetItemCount
int WINAPI WinMenu_GetItemCount( HMENU hMenu );

#undef GetMenuItemID
#define GetMenuItemID WinMenu_GetItemID
int WINAPI WinMenu_GetItemID( HMENU hMenu, int nPos );

#undef GetMenuItemInfo
#define GetMenuItemInfo WinMenu_GetItemInfo
BOOL WINAPI WinMenu_GetItemInfo( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii );

#undef GetSubMenu
#define GetSubMenu WinMenu_GetSub
HMENU WINAPI WinMenu_GetSub( HMENU hMenu, int nPos );

#undef TrackPopupMenu
#define TrackPopupMenu WinMenu_TrackPopup
BOOL WINAPI WinMenu_TrackPopup( HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hwndOwner, LPCRECT lpcRect );

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // __MENUSRV_H


