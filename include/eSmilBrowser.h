/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ESMILBROWSER_H
#define __ESMILBROWSER_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ewindows.h"


#define SMM_LOADSMIL	40200


// 通知消息
#define NMSMIL_CLICK		7500     // 点击通知消息

typedef struct tagNMSMILBROWSER
{
    NMHDR   hdr;
    POINT   ptAction;
} NMSMILBROWSER, FAR *LPNMSMILBROWSER;


extern LPTSTR classSMILBROWSER;




#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif //__ESMILBROWSER_H
