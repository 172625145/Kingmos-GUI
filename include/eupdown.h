/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EUPDOWN_H
#define __EUPDOWN_H

#ifndef __EFRAME_H
#include <eframe.h>
#endif //__EFRAME_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

typedef struct {
   UINT nSec;
   UINT nInc;
}UDACCEL, * LPUDACCEL;

typedef struct _NM_UPDOWN {
    NMHDR hdr;
    int   iPos;
    int   iDelta;
} NMUPDOWN, * LPNMUPDOWN;



#define UDM_GETACCEL             0x4801
#define UDM_GETBASE              0x4802
#define UDM_GETBUDDY             0x4803
#define UDM_GETPOS               0x4804
#define UDM_GETRANGE             0x4805
#define UDM_GETRANGE32           0x4806
#define UDM_GETUNICODEFORMAT     0x4807
#define UDM_SETACCEL             0x4808
#define UDM_SETBASE              0x4809
#define UDM_SETBUDDY             0x480A
#define UDM_SETPOS               0x480B
#define UDM_SETRANGE             0x480C
#define UDM_SETRANGE32           0x480D
#define UDM_SETUNICODEFORMAT     0x480E

#define UDN_DELTAPOS             0x4810


#define UDS_HORZ                 0x00000001
#define UDS_WRAP                 0x00000002
#define UDS_ARROWKEYS            0x00000004
#define UDS_SETBUDDYINT          0x00000008
#define UDS_NOTHOUSANDS          0x00000010
#define UDS_AUTOBUDDY            0x00000020
#define UDS_ALIGNRIGHT           0x00000040
#define UDS_ALIGNLEFT            0x00000080
#define UDS_HORZSHOW             0x00000100  // 垂直控制水平显示
#define UDS_VERTSHOW             0x00000100  // 水平控制垂直显示

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__EUPDOWN_H




