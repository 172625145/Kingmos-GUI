/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __ESMILBROWSER_H
#define __ESMILBROWSER_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ewindows.h"


#define SMM_LOADSMIL	40200


// ֪ͨ��Ϣ
#define NMSMIL_CLICK		7500     // ���֪ͨ��Ϣ

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
