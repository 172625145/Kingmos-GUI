/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EMENU_H
#define __EMENU_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

// menu flags
#define MF_STRING           0x00000000L
#define MF_GRAYED			0x00000001L
#define MF_BITMAP           0x00000004L
#define MF_OWNERDRAW        0x00000100L

#define MF_SEPARATOR        0x00000800L

#define MF_BYCOMMAND        0x00000000L
#define MF_BYPOSITION       0x00000400L

#define MFT_STRING          MF_STRING
#define MFT_BITMAP          MF_BITMAP
#define MFT_OWNERDRAW       MF_OWNERDRAW
//#define MF_POPUP            0x00000010L
#define MFT_SEPARATOR       MF_SEPARATOR

#define MFS_ENABLED          0x00000000L
#define MFS_GRAYED           MF_GRAYED
#define MFS_DISABLED         0x00000002L

#define MFS_MASK            0x0000108BL		//²Ëµ¥×´Ì¬Î»

// define menu function
#define MIIM_STATE       0x00000001
#define MIIM_ID          0x00000002
#define MIIM_SUBMENU     0x00000004
#define MIIM_CHECKMARKS  0x00000008
#define MIIM_TYPE        0x00000010
#define MIIM_DATA        0x00000020

typedef struct _MENUITEMINFO
{
    DWORD    cbSize;
    DWORD    fMask;
    DWORD    fType;          // used if MIIM_TYPE
    DWORD    fState;         // used if MIIM_STATE
    DWORD    wID;            // used if MIIM_ID
    HMENU    hSubMenu;       // used if MIIM_SUBMENU
    HBITMAP  hbmpChecked;    // used if MIIM_CHECKMARKS
    HBITMAP  hbmpUnchecked;  // used if MIIM_CHECKMARKS
    DWORD    dwItemData;     // used if MIIM_DATA
    LPTSTR    dwTypeData;     // used if MIIM_TYPE
    DWORD    cch;            // used if MIIM_TYPE
}MENUITEMINFO, * PMENUITEMINFO, FAR *LPMENUITEMINFO;

#define CreateMenu Menu_Create
HMENU WINAPI Menu_Create(void);

#define CreatePopupMenu Menu_CreatePopup
HMENU WINAPI Menu_CreatePopup(void);

#define DestroyMenu Menu_Destroy
BOOL WINAPI Menu_Destroy( HMENU hMenu );

#define RemoveMenu Menu_Remove
BOOL WINAPI Menu_Remove( HMENU hMenu, DWORD uPosition, DWORD uFlags );

#define DeleteMenu Menu_Delete
BOOL WINAPI Menu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags );

#define  InsertMenu Menu_Insert
BOOL WINAPI Menu_Insert(  HMENU hMenu,  DWORD uPosition,  DWORD uFlags,  DWORD uIDNewItem,  LPCTSTR lpNewItem );

#define InsertMenuItem Menu_InsertItem
BOOL WINAPI Menu_InsertItem( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii );

#define EnableMenuItem Menu_EnableItem
BOOL WINAPI Menu_EnableItem( HMENU hMenu, DWORD uPosition, DWORD uEnable );

#define GetMenuDefaultItem Menu_GetDefaultItem
DWORD WINAPI Menu_GetDefaultItem( HMENU, DWORD fByPos, DWORD uFlags );

#define SetMenuDefaultItem Menu_SetDefaultItem
BOOL WINAPI Menu_SetDefaultItem( HMENU, DWORD uItem, DWORD fByPos );

#define SetMenuItemInfo Menu_SetItemInfo
BOOL WINAPI Menu_SetItemInfo( HMENU, DWORD uItem, BOOL fByPos, LPMENUITEMINFO lpmii );

#define GetMenuItemCount Menu_GetItemCount
int WINAPI Menu_GetItemCount( HMENU hMenu );

#define GetMenuItemID Menu_GetItemID
int WINAPI Menu_GetItemID( HMENU hMenu, int nPos );

#define GetMenuItemInfo Menu_GetItemInfo
BOOL WINAPI Menu_GetItemInfo( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii );

#define GetSubMenu Menu_GetSub
HMENU WINAPI Menu_GetSub( HMENU hMenu, int nPos );

#define TPM_LEFTALIGN   0x0000L
#define TPM_CENTERALIGN 0x0004L
#define TPM_RIGHTALIGN  0x0008L

#define TPM_TOPALIGN        0x0000L
#define TPM_VCENTERALIGN    0x0010L
#define TPM_BOTTOMALIGN     0x0020L

#define TPM_NONOTIFY    0x0080L
#define TPM_RETURNCMD   0x0100L

#define TrackPopupMenu Menu_TrackPopup
BOOL WINAPI Menu_TrackPopup( HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hwndOwner, LPCRECT lpcRect );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // __EMENU_H
