/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __EABOUT_H
#define __EABOUT_H

#include "ewindows.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ����������й���������

static const struct dlgIDD_ABOUT{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[4];
}dlgIDD_ABOUT = {
    { WS_EX_CLOSEBOX|WS_EX_NOMOVE,0x90000000L|WS_CAPTION,4,30,70,210,180,0,0,"����" },
    {
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,0,50,200,20,0xFFFF,classSTATIC,"�޹���Kingmos 2.0.0 ",0 },
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,0,75,200,20,0xFFFF,classSTATIC,"����΢�߼��������޹�˾",0 },
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,0,100,200,20,0xFFFF,classSTATIC,"��Ȩ����(C)1998-2003",0 },
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,0,125,200,20,0xFFFF,classSTATIC,"��������Ȩ��",0 },
    } };


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EABOUT_H
