/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EPOS_H
#define __EPOS_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */


// independent interface of pointer device 

// define state of posrecord struct
#define PS_LBUTTON 0x0001           
#define PS_MBUTTON 0x0002
#define PS_RBUTTON 0x0004
typedef struct _POSRECORD
{
    short x;
    short y;
    WORD wState;
}POSRECORD, * LPPOSRECORD;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //__EPOS_H
