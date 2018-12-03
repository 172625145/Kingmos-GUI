/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __ELSTCTRL_H
#define __ELSTCTRL_H

#ifndef NOLISTVIEW
#define WM_INSERTITEM    (WM_OS)
//#define WM_ACTIVEITEM    (WM_OS+1)
#define LVN_FIRST               (0U-100U)       // listview
#define LVN_LAST                (0U-199U)

// begin_r_commctrl
#define ODT_LISTVIEW            102

#define LVS_ICON                0x0000
#define LVS_REPORT              0x0001
#define LVS_SMALLICON           0x0002
#define LVS_LIST                0x0003
#define LVS_TYPEMASK            0x0003
#define LVS_SINGLESEL           0x0004
#define LVS_SHOWSELALWAYS       0x0008
#define LVS_SORTASCENDING       0x0010
#define LVS_SORTDESCENDING      0x0020
#define LVS_SHAREIMAGELISTS     0x0040
#define LVS_NOLABELWRAP         0x0080
//#define LVS_AUTOARRANGE         0x0100
//#define LVS_EDITLABELS          0x0200
#define LVS_HORZLINE            0x0100
#define LVS_VERTLINE            0x0200
#define LVS_OWNERDATA           0x1000
#define LVS_NOSCROLL            0x2000

#define LVS_TYPESTYLEMASK       0xfc00

#define LVS_ALIGNTOP            0x0000
#define LVS_ALIGNLEFT           0x0800
#define LVS_ALIGNMASK           0x0c00

#define LVS_OWNERDRAWFIXED      0x0400
#define LVS_NOCOLUMNHEADER      0x4000
#define LVS_NOSORTHEADER        0x8000

// end_r_commctrl
#define LVM_FIRST               0x1000      // ListView messages


#define LVM_GETIMAGELIST        (LVM_FIRST + 2)
#define ListView_GetImageList(hwnd, iImageList) \
    (HIMAGELIST)SendMessage((hwnd), LVM_GETIMAGELIST, (WPARAM)(INT)(iImageList), 0L)

#define LVSIL_NORMAL            0
#define LVSIL_SMALL             1
#define LVSIL_STATE             2

#define LVM_SETIMAGELIST        (LVM_FIRST + 3)
#define ListView_SetImageList(hwnd, himl, iImageList) \
    (HIMAGELIST)(UINT)SendMessage((hwnd), LVM_SETIMAGELIST, (WPARAM)(iImageList), (LPARAM)(UINT)(HIMAGELIST)(himl))

#define LVM_GETITEMCOUNT        (LVM_FIRST + 4)
#define ListView_GetItemCount(hwnd) \
    (int)SendMessage((hwnd), LVM_GETITEMCOUNT, 0, 0L)

#define LVIF_TEXT               0x0001
#define LVIF_IMAGE              0x0002
#define LVIF_PARAM              0x0004
#define LVIF_STATE              0x0008
#define LVIF_INDENT             0x0010
#define LVIF_NORECOMPUTE        0x0800

#define LVIS_FOCUSED            0x0001
#define LVIS_SELECTED           0x0002
#define LVIS_CUT                0x0004
#define LVIS_DROPHILITED        0x0008
#define LVIS_ACTIVATING         0x0020

#define LVIS_OVERLAYMASK        0x0F00
#define LVIS_STATEIMAGEMASK     0xF000

#define INDEXTOSTATEIMAGEMASK(i) ((i) << 12)

#define tagLVITEM    _LV_ITEM
#define LVITEM       LV_ITEM


typedef struct tagLVITEMA
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
    int iIndent;
} LVITEM, FAR* LPLVITEM;



#define LPSTR_TEXTCALLBACK     ((LPSTR)-1L)

#define I_IMAGECALLBACK         (-1)

#define LVM_GETITEM            (LVM_FIRST + 5)

#define ListView_GetItem(hwnd, pitem) \
    (BOOL)SendMessage((hwnd), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)(pitem))


#define LVM_SETITEM            (LVM_FIRST + 6)

#define ListView_SetItem(hwnd, pitem) \
    (BOOL)SendMessage((hwnd), LVM_SETITEM, 0, (LPARAM)(const LV_ITEM FAR*)(pitem))


#define LVM_INSERTITEM         (LVM_FIRST + 7)

#define ListView_InsertItem(hwnd, pitem)   \
    (int)SendMessage((hwnd), LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM FAR*)(pitem))


#define LVM_DELETEITEM          (LVM_FIRST + 8)
#define ListView_DeleteItem(hwnd, i) \
    (BOOL)SendMessage((hwnd), LVM_DELETEITEM, (WPARAM)(int)(i), 0L)


#define LVM_DELETEALLITEMS      (LVM_FIRST + 9)
#define ListView_DeleteAllItems(hwnd) \
    (BOOL)SendMessage((hwnd), LVM_DELETEALLITEMS, 0, 0L)


/*efine LVM_GETCALLBACKMASK     (LVM_FIRST + 10)
#define ListView_GetCallbackMask(hwnd) \
    (BOOL)SendMessage((hwnd), LVM_GETCALLBACKMASK, 0, 0)

*/
/*
#define LVM_SETCALLBACKMASK     (LVM_FIRST + 11)
#define ListView_SetCallbackMask(hwnd, mask) \
    (BOOL)SendMessage((hwnd), LVM_SETCALLBACKMASK, (WPARAM)(UINT)(mask), 0)
*/

#define LVNI_ALL                0x0000
#define LVNI_FOCUSED            0x0001
#define LVNI_SELECTED           0x0002
#define LVNI_CUT                0x0004
#define LVNI_DROPHILITED        0x0008

#define LVNI_ABOVE              0x0100
#define LVNI_BELOW              0x0200
#define LVNI_TOLEFT             0x0400
#define LVNI_TORIGHT            0x0800


#define LVM_GETNEXTITEM         (LVM_FIRST + 12)
#define ListView_GetNextItem(hwnd, i, flags) \
    (int)SendMessage((hwnd), LVM_GETNEXTITEM, (WPARAM)(int)(i), MAKELPARAM((flags), 0))


#define LVFI_PARAM              0x0001
#define LVFI_STRING             0x0002
#define LVFI_PARTIAL            0x0008
#define LVFI_WRAP               0x0020
#define LVFI_NEARESTXY          0x0040

#define LV_FINDINFO    LVFINDINFO
#define tagLVFINDINFO  _LV_FINDINFOA
#define    LVFINDINFO   LV_FINDINFOA

#define LV_FINDINFO  LVFINDINFO

typedef struct tagLVFINDINFO
{
    UINT flags;
    LPCSTR psz;
    LPARAM lParam;
    POINT pt;
    UINT vkDirection;
} LVFINDINFO, FAR* LPFINDINFO;


#define LVM_FINDITEM           (LVM_FIRST + 13)

#define ListView_FindItem(hwnd, iStart, plvfi) \
    (int)SendMessage((hwnd), LVM_FINDITEM, (WPARAM)(int)(iStart), (LPARAM)(const LV_FINDINFO FAR*)(plvfi))

#define LVIR_BOUNDS             0
#define LVIR_ICON               1
#define LVIR_LABEL              2
#define LVIR_SELECTBOUNDS       3


#define LVM_GETITEMRECT         (LVM_FIRST + 14)
#define ListView_GetItemRect(hwnd, i, prc, code) \
     (BOOL)SendMessage((hwnd), LVM_GETITEMRECT, (WPARAM)(int)(i), \
           ((prc) ? (((RECT FAR *)(prc))->left = (code),(LPARAM)(RECT FAR*)(prc)) : (LPARAM)(RECT FAR*)NULL))


/*
#define LVM_SETITEMPOSITION     (LVM_FIRST + 15)
#define ListView_SetItemPosition(hwndLV, i, x, y) \
    (BOOL)SendMessage((hwndLV), LVM_SETITEMPOSITION, (WPARAM)(int)(i), MAKELPARAM((x), (y)))

*/
/*
#define LVM_GETITEMPOSITION     (LVM_FIRST + 16)
#define ListView_GetItemPosition(hwndLV, i, ppt) \
    (BOOL)SendMessage((hwndLV), LVM_GETITEMPOSITION, (WPARAM)(int)(i), (LPARAM)(POINT FAR*)(ppt))
*/

#define LVM_GETSTRINGWIDTH     (LVM_FIRST + 17)

#define ListView_GetStringWidth(hwndLV, psz) \
    (int)SendMessage((hwndLV), LVM_GETSTRINGWIDTH, 0, (LPARAM)(LPCTSTR)(psz))


#define LVHT_NOWHERE            0x0001
#define LVHT_ONITEMICON         0x0002
#define LVHT_ONITEMLABEL        0x0004
#define LVHT_ONITEMSTATEICON    0x0008
#define LVHT_ONITEM             (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON)

#define LVHT_ABOVE              0x0008
#define LVHT_BELOW              0x0010
#define LVHT_TORIGHT            0x0020
#define LVHT_TOLEFT             0x0040

#define LV_HITTESTINFO LVHITTESTINFO
#define tagLVHITTESTINFO  _LV_HITTESTINFO
#define    LVHITTESTINFO   LV_HITTESTINFO

#define LVHITTESTINFO_V1_SIZE CCSIZEOF_STRUCT(LVHITTESTINFO, iItem)

typedef struct tagLVHITTESTINFO
{
    POINT pt;
    UINT flags;
    int iItem;
    int iSubItem;    // this is was NOT in win95.  valid only for LVM_SUBITEMHITTEST
} LVHITTESTINFO, FAR* LPLVHITTESTINFO;

#define LVM_HITTEST             (LVM_FIRST + 18)
#define ListView_HitTest(hwndLV, pinfo) \
    (int)SendMessage((hwndLV), LVM_HITTEST, 0, (LPARAM)(LV_HITTESTINFO FAR*)(pinfo))


#define LVM_ENSUREVISIBLE       (LVM_FIRST + 19)
#define ListView_EnsureVisible(hwndLV, i, fPartialOK) \
    (BOOL)SendMessage((hwndLV), LVM_ENSUREVISIBLE, (WPARAM)(int)(i), MAKELPARAM((fPartialOK), 0))


#define LVM_SCROLL              (LVM_FIRST + 20)
#define ListView_Scroll(hwndLV, dx, dy) \
    (BOOL)SendMessage((hwndLV), LVM_SCROLL, (WPARAM)(int)dx, (LPARAM)(int)dy)


#define LVM_REDRAWITEMS         (LVM_FIRST + 21)
#define ListView_RedrawItems(hwndLV, iFirst, iLast) \
    (BOOL)SendMessage((hwndLV), LVM_REDRAWITEMS, (WPARAM)(int)iFirst, (LPARAM)(int)iLast)

/*
#define LVA_DEFAULT             0x0000
#define LVA_ALIGNLEFT           0x0001
#define LVA_ALIGNTOP            0x0002
#define LVA_SNAPTOGRID          0x0005

#define LVM_ARRANGE             (LVM_FIRST + 22)
#define ListView_Arrange(hwndLV, code) \
    (BOOL)SendMessage((hwndLV), LVM_ARRANGE, (WPARAM)(UINT)(code), 0L)

*/

#define LVM_EDITLABEL          (LVM_FIRST + 23)

#define ListView_EditLabel(hwndLV, i) \
    (HWND)SendMessage((hwndLV), LVM_EDITLABEL, (WPARAM)(int)(i), 0L)


#define LVM_GETEDITCONTROL      (LVM_FIRST + 24)
#define ListView_GetEditControl(hwndLV) \
    (HWND)SendMessage((hwndLV), LVM_GETEDITCONTROL, 0, 0L)


#define LV_COLUMN      LVCOLUMN
#define tagLVCOLUMN    _LV_COLUMN


typedef struct tagLVCOLUMNA
{
    UINT mask;
    int fmt;
    int cx;
    LPSTR pszText;
    int cchTextMax;
    int iSubItem;
    int iImage;
    int iOrder;
} LVCOLUMN, FAR* LPLVCOLUMN;



#define LVCF_FMT                0x0001
#define LVCF_WIDTH              0x0002
#define LVCF_TEXT               0x0004
#define LVCF_SUBITEM            0x0008
#define LVCF_IMAGE              0x0010
#define LVCF_ORDER              0x0020

#define LVCFMT_LEFT             0x0000
#define LVCFMT_RIGHT            0x0001
#define LVCFMT_CENTER           0x0002
#define LVCFMT_JUSTIFYMASK      0x0003
#define LVCFMT_FILE             0x0400
#define LVCFMT_IMAGE            0x0800
#define LVCFMT_BITMAP_ON_RIGHT  0x1000
#define LVCFMT_COL_HAS_IMAGES   0x8000

#define LVM_GETCOLUMN          (LVM_FIRST + 25)

#define ListView_GetColumn(hwnd, iCol, pcol) \
    (BOOL)SendMessage((hwnd), LVM_GETCOLUMN, (WPARAM)(int)(iCol), (LPARAM)(LV_COLUMN FAR*)(pcol))


#define LVM_SETCOLUMN          (LVM_FIRST + 26)

#define ListView_SetColumn(hwnd, iCol, pcol) \
    (BOOL)SendMessage((hwnd), LVM_SETCOLUMN, (WPARAM)(int)(iCol), (LPARAM)(const LV_COLUMN FAR*)(pcol))


#define LVM_INSERTCOLUMN       (LVM_FIRST + 27)

#define ListView_InsertColumn(hwnd, iCol, pcol) \
    (int)SendMessage((hwnd), LVM_INSERTCOLUMN, (WPARAM)(int)(iCol), (LPARAM)(const LV_COLUMN FAR*)(pcol))


#define LVM_DELETECOLUMN        (LVM_FIRST + 28)
#define ListView_DeleteColumn(hwnd, iCol) \
    (BOOL)SendMessage((hwnd), LVM_DELETECOLUMN, (WPARAM)(int)(iCol), 0)


#define LVM_GETCOLUMNWIDTH      (LVM_FIRST + 29)
#define ListView_GetColumnWidth(hwnd, iCol) \
    (int)SendMessage((hwnd), LVM_GETCOLUMNWIDTH, (WPARAM)(int)(iCol), 0)


#define LVSCW_AUTOSIZE              -1
#define LVSCW_AUTOSIZE_USEHEADER    -2
#define LVM_SETCOLUMNWIDTH          (LVM_FIRST + 30)

#define ListView_SetColumnWidth(hwnd, iCol, cx) \
    (BOOL)SendMessage((hwnd), LVM_SETCOLUMNWIDTH, (WPARAM)(int)(iCol), MAKELPARAM((cx), 0))

/*
#define LVM_GETHEADER               (LVM_FIRST + 31)
#define ListView_GetHeader(hwnd)\
    (HWND)SendMessage((hwnd), LVM_GETHEADER, 0, 0L)
*/
/*
#define LVM_CREATEDRAGIMAGE     (LVM_FIRST + 33)
#define ListView_CreateDragImage(hwnd, i, lpptUpLeft) \
    (HIMAGELIST)SendMessage((hwnd), LVM_CREATEDRAGIMAGE, (WPARAM)(int)(i), (LPARAM)(LPPOINT)(lpptUpLeft))
*/

#define LVM_GETVIEWRECT         (LVM_FIRST + 34)
#define ListView_GetViewRect(hwnd, prc) \
    (BOOL)SendMessage((hwnd), LVM_GETVIEWRECT, 0, (LPARAM)(RECT FAR*)(prc))

/*
#define LVM_GETTEXTCOLOR        (LVM_FIRST + 35)
#define ListView_GetTextColor(hwnd)  \
    (COLORREF)SendMessage((hwnd), LVM_GETTEXTCOLOR, 0, 0L)
*/
/*
#define LVM_SETTEXTCOLOR        (LVM_FIRST + 36)
#define ListView_SetTextColor(hwnd, clrText) \
    (BOOL)SendMessage((hwnd), LVM_SETTEXTCOLOR, 0, (LPARAM)(COLORREF)(clrText))
*/
/*
#define LVM_GETTEXTBKCOLOR      (LVM_FIRST + 37)
#define ListView_GetTextBkColor(hwnd)  \
    (COLORREF)SendMessage((hwnd), LVM_GETTEXTBKCOLOR, 0, 0L)
*/
/*
#define LVM_SETTEXTBKCOLOR      (LVM_FIRST + 38)
#define ListView_SetTextBkColor(hwnd, clrTextBk) \
    (BOOL)SendMessage((hwnd), LVM_SETTEXTBKCOLOR, 0, (LPARAM)(COLORREF)(clrTextBk))
*/

#define LVM_GETTOPINDEX         (LVM_FIRST + 39)
#define ListView_GetTopIndex(hwndLV) \
    (int)SendMessage((hwndLV), LVM_GETTOPINDEX, 0, 0)


#define LVM_GETCOUNTPERPAGE     (LVM_FIRST + 40)
#define ListView_GetCountPerPage(hwndLV) \
    (int)SendMessage((hwndLV), LVM_GETCOUNTPERPAGE, 0, 0)


#define LVM_GETORIGIN           (LVM_FIRST + 41)
#define ListView_GetOrigin(hwndLV, ppt) \
    (BOOL)SendMessage((hwndLV), LVM_GETORIGIN, (WPARAM)0, (LPARAM)(POINT FAR*)(ppt))


#define LVM_UPDATE              (LVM_FIRST + 42)
#define ListView_Update(hwndLV, i) \
    (BOOL)SendMessage((hwndLV), LVM_UPDATE, (WPARAM)i, 0L)


#define LVM_SETITEMSTATE        (LVM_FIRST + 43)
#define ListView_SetItemState(hwndLV, i, data, mask) \
{ LV_ITEM _ms_lvi;\
  _ms_lvi.stateMask = mask;\
  _ms_lvi.state = data;\
  SendMessage((hwndLV), LVM_SETITEMSTATE, (WPARAM)i, (LPARAM)(LV_ITEM FAR *)&_ms_lvi);\
}


#define LVM_GETITEMSTATE        (LVM_FIRST + 44)
#define ListView_GetItemState(hwndLV, i, mask) \
   (UINT)SendMessage((hwndLV), LVM_GETITEMSTATE, (WPARAM)i, (LPARAM)mask)

#define ListView_GetCheckState(hwndLV, i) \
   ((((UINT)(SendMessage((hwndLV), LVM_GETITEMSTATE, (WPARAM)i, LVIS_STATEIMAGEMASK))) >> 12) -1)

#define LVM_GETITEMTEXT        (LVM_FIRST + 45)

#define ListView_GetItemText(hwndLV, i, iSubItem_, pszText_, cchTextMax_) \
{ LV_ITEM _ms_lvi;\
  _ms_lvi.iSubItem = iSubItem_;\
  _ms_lvi.cchTextMax = cchTextMax_;\
  _ms_lvi.pszText = pszText_;\
  SendMessage((hwndLV), LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)(LV_ITEM FAR *)&_ms_lvi);\
}


#define LVM_SETITEMTEXT        (LVM_FIRST + 46)

#define ListView_SetItemText(hwndLV, i, iSubItem_, pszText_) \
{ LV_ITEM _ms_lvi;\
  _ms_lvi.iSubItem = iSubItem_;\
  _ms_lvi.pszText = pszText_;\
  SendMessage((hwndLV), LVM_SETITEMTEXT, (WPARAM)i, (LPARAM)(LV_ITEM FAR *)&_ms_lvi);\
}

// these flags only apply to LVS_OWNERDATA listviews in report or list mode
#define LVSICF_NOINVALIDATEALL  0x00000001
#define LVSICF_NOSCROLL         0x00000002

#define LVM_SETITEMCOUNT        (LVM_FIRST + 47)
#define ListView_SetItemCount(hwndLV, cItems) \
  SendMessage((hwndLV), LVM_SETITEMCOUNT, (WPARAM)cItems, 0)

#define ListView_SetItemCountEx(hwndLV, cItems, dwFlags) \
  SendMessage((hwndLV), LVM_SETITEMCOUNT, (WPARAM)cItems, (LPARAM)dwFlags)

typedef int (CALLBACK *PFNLVCOMPARE)(LPARAM, LPARAM, LPARAM);


#define LVM_SORTITEMS           (LVM_FIRST + 48)
#define ListView_SortItems(hwndLV, _pfnCompare, _lPrm) \
  (BOOL)SendMessage((hwndLV), LVM_SORTITEMS, (WPARAM)(LPARAM)_lPrm, \
  (LPARAM)(PFNLVCOMPARE)_pfnCompare)

/*
#define LVM_SETITEMPOSITION32   (LVM_FIRST + 49)
#define ListView_SetItemPosition32(hwndLV, i, x, y) \
{ POINT ptNewPos = {x,y}; \
    SendMessage((hwndLV), LVM_SETITEMPOSITION32, (WPARAM)(int)(i), (LPARAM)&ptNewPos); \
}
*/

#define LVM_GETSELECTEDCOUNT    (LVM_FIRST + 50)
#define ListView_GetSelectedCount(hwndLV) \
    (UINT)SendMessage((hwndLV), LVM_GETSELECTEDCOUNT, 0, 0L)


#define LVM_GETITEMSPACING      (LVM_FIRST + 51)
#define ListView_GetItemSpacing(hwndLV, fSmall) \
        (DWORD)SendMessage((hwndLV), LVM_GETITEMSPACING, fSmall, 0L)


#define LVM_GETISEARCHSTRING   (LVM_FIRST + 52)

#define ListView_GetISearchString(hwndLV, lpsz) \
        (BOOL)SendMessage((hwndLV), LVM_GETISEARCHSTRING, 0, (LPARAM)(LPTSTR)lpsz)

#define LVM_SETICONSPACING      (LVM_FIRST + 53)
// -1 for cx and cy means we'll use the default (system settings)
// 0 for cx or cy means use the current setting (allows you to change just one param)
#define ListView_SetIconSpacing(hwndLV, cx, cy) \
        (DWORD)SendMessage((hwndLV), LVM_SETICONSPACING, 0, MAKELONG(cx,cy))


/*
#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)   // optional wParam == mask
#define ListView_SetExtendedListViewStyle(hwndLV, dw)\
        (DWORD)SendMessage((hwndLV), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw)

#define ListView_SetExtendedListViewStyleEx(hwndLV, dwMask, dw)\
        (DWORD)SendMessage((hwndLV), LVM_SETEXTENDEDLISTVIEWSTYLE, dwMask, dw)
*/
/*
#define LVM_GETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 55)
#define ListView_GetExtendedListViewStyle(hwndLV)\
        (DWORD)SendMessage((hwndLV), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0)

#define LVS_EX_GRIDLINES        0x00000001
#define LVS_EX_SUBITEMIMAGES    0x00000002
#define LVS_EX_CHECKBOXES       0x00000004
#define LVS_EX_TRACKSELECT      0x00000008
#define LVS_EX_HEADERDRAGDROP   0x00000010
#define LVS_EX_FULLROWSELECT    0x00000020 // applies to report mode only
#define LVS_EX_ONECLICKACTIVATE 0x00000040
#define LVS_EX_TWOCLICKACTIVATE 0x00000080
#define LVS_EX_FLATSB           0x00000100
#define LVS_EX_REGIONAL         0x00000200
#define LVS_EX_INFOTIP          0x00000400 // listview does InfoTips for you
#define LVS_EX_UNDERLINEHOT     0x00000800
#define LVS_EX_UNDERLINECOLD    0x00001000
#define LVS_EX_MULTIWORKAREAS   0x00002000
*/
#define LVM_GETSUBITEMRECT      (LVM_FIRST + 56)
#define ListView_GetSubItemRect(hwnd, iItem, iSubItem, code, prc) \
        (BOOL)SendMessage((hwnd), LVM_GETSUBITEMRECT, (WPARAM)(int)(iItem), \
                ((prc) ? ((((LPRECT)(prc))->top = iSubItem), (((LPRECT)(prc))->left = code), (LPARAM)(prc)) : (LPARAM)(LPRECT)NULL))

#define LVM_SUBITEMHITTEST      (LVM_FIRST + 57)
#define ListView_SubItemHitTest(hwnd, plvhti) \
        (int)SendMessage((hwnd), LVM_SUBITEMHITTEST, 0, (LPARAM)(LPLVHITTESTINFO)(plvhti))

#define LVM_SETCOLUMNORDERARRAY (LVM_FIRST + 58)
#define ListView_SetColumnOrderArray(hwnd, iCount, pi) \
        (BOOL)SendMessage((hwnd), LVM_SETCOLUMNORDERARRAY, (WPARAM)iCount, (LPARAM)(LPINT)pi)

#define LVM_GETCOLUMNORDERARRAY (LVM_FIRST + 59)
#define ListView_GetColumnOrderArray(hwnd, iCount, pi) \
        (BOOL)SendMessage((hwnd), LVM_GETCOLUMNORDERARRAY, (WPARAM)iCount, (LPARAM)(LPINT)pi)

#define LVM_SETHOTITEM  (LVM_FIRST + 60)
#define ListView_SetHotItem(hwnd, i) \
        (int)SendMessage((hwnd), LVM_SETHOTITEM, (WPARAM)i, 0)

#define LVM_GETHOTITEM  (LVM_FIRST + 61)
#define ListView_GetHotItem(hwnd) \
        (int)SendMessage((hwnd), LVM_GETHOTITEM, 0, 0)
/*
#define LVM_SETHOTCURSOR  (LVM_FIRST + 62)
#define ListView_SetHotCursor(hwnd, hcur) \
        (HCURSOR)SendMessage((hwnd), LVM_SETHOTCURSOR, 0, (LPARAM)hcur)
*/
/*
#define LVM_GETHOTCURSOR  (LVM_FIRST + 63)
#define ListView_GetHotCursor(hwnd) \
        (HCURSOR)SendMessage((hwnd), LVM_GETHOTCURSOR, 0, 0)
*/
#define LVM_APPROXIMATEVIEWRECT (LVM_FIRST + 64)
#define ListView_ApproximateViewRect(hwnd, iWidth, iHeight, iCount) \
        (DWORD)SendMessage((hwnd), LVM_APPROXIMATEVIEWRECT, iCount, MAKELPARAM(iWidth, iHeight))

/*
#define LV_MAX_WORKAREAS         16                          
#define LVM_SETWORKAREAS         (LVM_FIRST + 65)
#define ListView_SetWorkAreas(hwnd, nWorkAreas, prc) \
    (BOOL)SendMessage((hwnd), LVM_SETWORKAREAS, (WPARAM)(int)nWorkAreas, (LPARAM)(RECT FAR*)(prc))
*/
/*
#define LVM_GETWORKAREAS        (LVM_FIRST + 70)
#define ListView_GetWorkAreas(hwnd, nWorkAreas, prc) \
    (BOOL)SendMessage((hwnd), LVM_GETWORKAREAS, (WPARAM)(int)nWorkAreas, (LPARAM)(RECT FAR*)(prc))
*/
/*
#define LVM_GETNUMBEROFWORKAREAS  (LVM_FIRST + 73)
#define ListView_GetNumberOfWorkAreas(hwnd, pnWorkAreas) \
    (BOOL)SendMessage((hwnd), LVM_GETNUMBEROFWORKAREAS, 0, (LPARAM)(UINT *)(pnWorkAreas))
*/

#define LVM_GETSELECTIONMARK    (LVM_FIRST + 66)
#define ListView_GetSelectionMark(hwnd) \
    (int)SendMessage((hwnd), LVM_GETSELECTIONMARK, 0, 0)

#define LVM_SETSELECTIONMARK    (LVM_FIRST + 67)
#define ListView_SetSelectionMark(hwnd, i) \
    (int)SendMessage((hwnd), LVM_SETSELECTIONMARK, 0, (LPARAM)i)

/*
#define LVM_SETHOVERTIME        (LVM_FIRST + 71)
#define ListView_SetHoverTime(hwndLV, dwHoverTimeMs)\
        (DWORD)SendMessage((hwndLV), LVM_SETHOVERTIME, 0, dwHoverTimeMs)
*/
/*
#define LVM_GETHOVERTIME        (LVM_FIRST + 72)
#define ListView_GetHoverTime(hwndLV)\
        (DWORD)SendMessage((hwndLV), LVM_GETHOVERTIME, 0, 0)
*/
/*
#define LVM_SETTOOLTIPS       (LVM_FIRST + 74)
#define ListView_SetToolTips(hwndLV, hwndNewHwnd)\
        (HWND)SendMessage((hwndLV), LVM_SETTOOLTIPS, hwndNewHwnd, 0)
*/
/*
#define LVM_GETTOOLTIPS       (LVM_FIRST + 78)
#define ListView_GetToolTips(hwndLV)\
        (HWND)SendMessage((hwndLV), LVM_GETTOOLTIPS, 0, 0)
*/
/*
typedef struct tagLVBKIMAGE
{
    ULONG ulFlags;              // LVBKIF_*
    HBITMAP hbm;
    LPSTR pszImage;
    UINT cchImageMax;
    int xOffsetPercent;
    int yOffsetPercent;
} LVBKIMAGE, FAR *LPLVBKIMAGE;

#define LVBKIF_SOURCE_NONE      0x00000000
#define LVBKIF_SOURCE_HBITMAP   0x00000001
#define LVBKIF_SOURCE_URL       0x00000002
#define LVBKIF_SOURCE_MASK      0x00000003
#define LVBKIF_STYLE_NORMAL     0x00000000
#define LVBKIF_STYLE_TILE       0x00000010
#define LVBKIF_STYLE_MASK       0x00000010

#define LVM_SETBKIMAGE         (LVM_FIRST + 68)
#define LVM_GETBKIMAGE         (LVM_FIRST + 69)


#define ListView_SetBkImage(hwnd, plvbki) \
    (BOOL)SendMessage((hwnd), LVM_SETBKIMAGE, 0, (LPARAM)plvbki)

#define ListView_GetBkImage(hwnd, plvbki) \
    (BOOL)SendMessage((hwnd), LVM_GETBKIMAGE, 0, (LPARAM)plvbki)

*/
#define NM_LISTVIEW     NMLISTVIEW
#define tagNMLISTVIEW   _NM_LISTVIEW
#define  LPNMLISTVIEW   LPNM_LISTVIEW

typedef struct _NMLISTVIEW
{
    NMHDR   hdr;
    int     iItem;
    int     iSubItem;
    UINT    uNewState;
    UINT    uOldState;
    UINT    uChanged;
    POINT   ptAction;
    LPARAM  lParam;
} NMLISTVIEW, FAR *LPNMLISTVIEW;


// NMITEMACTIVATE is used instead of NMLISTVIEW in IE >= 0x400
// therefore all the fields are the same except for extra uKeyFlags
// they are used to store key flags at the time of the single click with
// delayed activation - because by the time the timer goes off a user may
// not hold the keys (shift, ctrl) any more
typedef struct tagNMITEMACTIVATE
{
    NMHDR   hdr;
    int     iItem;
    int     iSubItem;
    UINT    uNewState;
    UINT    uOldState;
    UINT    uChanged;
    POINT   ptAction;
    LPARAM  lParam;
    UINT    uKeyFlags;
} NMITEMACTIVATE, FAR *LPNMITEMACTIVATE;

// key flags stored in uKeyFlags
#define LVKF_ALT       0x0001
#define LVKF_CONTROL   0x0002
#define LVKF_SHIFT     0x0004


#define NMLVCUSTOMDRAW_V3_SIZE CCSIZEOF_STRUCT(NMLVCUSTOMDRW, clrTextBk)

typedef struct tagNMCUSTOMDRAWINFO
{
    NMHDR hdr;
    DWORD dwDrawStage;
    HDC hdc;
    RECT rc;
    DWORD dwItemSpec;  // this is control specific, but it's how to specify an item.  valid only with CDDS_ITEM bit set
    UINT  uItemState;
    LPARAM lItemlParam;
} NMCUSTOMDRAW, FAR * LPNMCUSTOMDRAW;

typedef struct tagNMLVCUSTOMDRAW
{
    NMCUSTOMDRAW nmcd;
    COLORREF clrText;
    COLORREF clrTextBk;
    int iSubItem;
} NMLVCUSTOMDRAW, *LPNMLVCUSTOMDRAW;

typedef struct tagNMLVCACHEHINT
{
    NMHDR   hdr;
    int     iFrom;
    int     iTo;
} NMLVCACHEHINT, FAR *LPNMLVCACHEHINT;

#define LPNM_CACHEHINT  LPNMLVCACHEHINT
#define PNM_CACHEHINT   LPNMLVCACHEHINT
#define NM_CACHEHINT    NMLVCACHEHINT

typedef struct tagNMLVFINDITEM
{
    NMHDR   hdr;
    int     iStart;
    LVFINDINFO lvfi;
} NMLVFINDITEM, FAR *LPNMLVFINDITEM;

#define PNM_FINDITEM    LPNMLVFINDITEM
#define LPNM_FINDITEM   LPNMLVFINDITEM
#define NM_FINDITEM     NMLVFINDITEM

typedef struct tagNMLVODSTATECHANGE
{
    NMHDR hdr;
    int iFrom;
    int iTo;
    UINT uNewState;
    UINT uOldState;
} NMLVODSTATECHANGE, FAR *LPNMLVODSTATECHANGE;

#define PNM_ODSTATECHANGE   LPNMLVODSTATECHANGE
#define LPNM_ODSTATECHANGE  LPNMLVODSTATECHANGE
#define NM_ODSTATECHANGE    NMLVODSTATECHANGE


#define LVN_ITEMCHANGING        (LVN_FIRST-0)
#define LVN_ITEMCHANGED         (LVN_FIRST-1)
#define LVN_INSERTITEM          (LVN_FIRST-2)
#define LVN_DELETEITEM          (LVN_FIRST-3)
#define LVN_DELETEALLITEMS      (LVN_FIRST-4)
#define LVN_BEGINLABELEDITA     (LVN_FIRST-5)
#define LVN_BEGINLABELEDITW     (LVN_FIRST-75)
//#define LVN_ENDLABELEDITA       (LVN_FIRST-6)
//#define LVN_ENDLABELEDITW       (LVN_FIRST-76)
#define LVN_ENDLABELEDIT       (LVN_FIRST-6)
//#define LVN_ENDLABELEDITW       (LVN_FIRST-76)
#define LVN_COLUMNCLICK         (LVN_FIRST-8)
#define LVN_BEGINDRAG           (LVN_FIRST-9)
#define LVN_BEGINRDRAG          (LVN_FIRST-11)

#define LVN_ODCACHEHINT         (LVN_FIRST-13)
#define LVN_ODFINDITEMA         (LVN_FIRST-52)
#define LVN_ODFINDITEMW         (LVN_FIRST-79)

#define LVN_ITEMACTIVATE        (LVN_FIRST-14)
#define LVN_ODSTATECHANGED      (LVN_FIRST-15)



#define LVN_HOTTRACK            (LVN_FIRST-21)

#define LVN_GETDISPINFO        (LVN_FIRST-50)
#define LVN_SETDISPINFO        (LVN_FIRST-51)

//====== Generic WM_NOTIFY notification codes =================================
#define NM_FIRST                (0U-  0U)       // generic to all controls
#define NM_LAST                 (0U- 99U)

#define NM_OUTOFMEMORY          (NM_FIRST-1)
#define NM_CLICK                (NM_FIRST-2)    // uses NMCLICK struct
#define NM_DBLCLK               (NM_FIRST-3)
#define NM_RETURN               (NM_FIRST-4)
#define NM_RCLICK               (NM_FIRST-5)    // uses NMCLICK struct
#define NM_RDBLCLK              (NM_FIRST-6)
#define NM_SETFOCUS             (NM_FIRST-7)
#define NM_KILLFOCUS            (NM_FIRST-8)
#define NM_CUSTOMDRAW           (NM_FIRST-12)
#define NM_HOVER                (NM_FIRST-13)
#define NM_NCHITTEST            (NM_FIRST-14)   // uses NMMOUSE struct
#define NM_KEYDOWN              (NM_FIRST-15)   // uses NMKEY struct
#define NM_RELEASEDCAPTURE      (NM_FIRST-16)
#define NM_SETCURSOR            (NM_FIRST-17)   // uses NMMOUSE struct
#define NM_CHAR                 (NM_FIRST-18)   // uses NMCHAR struct


#define LVIF_DI_SETITEM         0x1000

#define LV_DISPINFO    NMLVDISPINFO

#define tagLVDISPINFO   _LV_DISPINFO
#define NMLVDISPINFO    LV_DISPINFO

#define LV_DISPINFO     NMLVDISPINFO

typedef struct tagLVDISPINFO {
    NMHDR hdr;
    LVITEM item;
} NMLVDISPINFO, FAR *LPNMLVDISPINFO;


#define LVN_KEYDOWN             (LVN_FIRST-55)

#define LV_KEYDOWN              NMLVKEYDOWN
#define tagLVKEYDOWN            _LV_KEYDOWN

typedef struct tagLVKEYDOWN
{
    NMHDR hdr;
    WORD wVKey;
    UINT flags;
} NMLVKEYDOWN, FAR *LPNMLVKEYDOWN;

#define LVN_MARQUEEBEGIN        (LVN_FIRST-56)

typedef struct tagNMLVGETINFOTIP
{
    NMHDR hdr;
    DWORD dwFlags;
    LPSTR pszText;
    int cchTextMax;
    int iItem;
    int iSubItem;
    LPARAM lParam;
} NMLVGETINFOTIP, *LPNMLVGETINFOTIP;


#define LVGIT_UNFOLDED  0x0001

#define LVN_GETINFOTIP          (LVN_FIRST-57)

#endif 
#endif   //__ELSTCTRL_H
