/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/
/*
OEM �Զ������
*/

#ifndef __EOEMKEY_H
#define __EOEMKEY_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

//nclude <emessage.h>

//#define VK_LMENU     VK_LMENU		//��ͬ
#define VK_INFO      VK_HELP
//#define VK_DELETE    VK_DELETE   //��ͬ
#define VK_ZOOM_INC  VK_F2
#define VK_ZOOM_DEC  VK_F3
#define VK_SET       VK_RETURN
//#define VK_LEFT      VK_LEFT		//��ͬ
//#define VK_TOP       VK_TOP		//��ͬ
//#define VK_RIGHT     VK_RIGHT		//��ͬ
//#define VK_DOWN      VK_DOWN		//��ͬ
//#define VK_PRINT     VK_PRINT		//��ͬ
#ifndef VK_CANCEL
#define VK_CANCEL    VK_ESCAPE
#endif

#ifndef VK_POWER
#define VK_POWER     VK_F4
#endif

#ifndef VK_BACKUP
#define VK_BACKUP    VK_F5
#endif

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // __EOEMKEY_H