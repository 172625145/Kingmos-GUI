/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
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

//��ʼ�����е� Controls
BOOL WINAPI InitThirdpartControls( void );

#ifdef CANON_DIGCMP
#include <third_canon.h>
#endif

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // __THIRDPART_H
