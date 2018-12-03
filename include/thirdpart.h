/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __THIRDPART_H
#define __THIRDPART_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

typedef struct _INIT_THIRDPART_CONTROLSEX 
{
    DWORD dwSize;  // = sizeof(INIT_THIRDPART_CONTROLSEX) 
    DWORD dwClassMask;
} INIT_THIRDPART_CONTROLSEX , *LPINIT_THIRDPART_CONTROLSEX ;

BOOL WINAPI InitThirdpartControlsEx( LPINIT_THIRDPART_CONTROLSEX lpitc );

//初始化所有的 Controls
BOOL WINAPI InitThirdpartControls( void );

#ifdef CANON_DIGCMP
#include <third_canon.h>
#endif

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // __THIRDPART_H
