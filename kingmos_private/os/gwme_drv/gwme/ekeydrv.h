/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EKEYDRV_H
#define __EKEYDRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

typedef struct _KEYRECORD
{
    DWORD dwKeyValue;
	DWORD lParam;
    WORD  wState;
    WORD  wCount;
}KEYRECORD, *LPKEYRECORD;

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif         // __EKEYDRV_H
