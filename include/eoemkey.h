/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/
/*
OEM 自定义键盘
*/

#ifndef __EOEMKEY_H
#define __EOEMKEY_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

//nclude <emessage.h>

//#define VK_LMENU     VK_LMENU		//相同
#define VK_INFO      VK_HELP
//#define VK_DELETE    VK_DELETE   //相同
#define VK_ZOOM_INC  VK_F2
#define VK_ZOOM_DEC  VK_F3
#define VK_SET       VK_RETURN
//#define VK_LEFT      VK_LEFT		//相同
//#define VK_TOP       VK_TOP		//相同
//#define VK_RIGHT     VK_RIGHT		//相同
//#define VK_DOWN      VK_DOWN		//相同
//#define VK_PRINT     VK_PRINT		//相同
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
