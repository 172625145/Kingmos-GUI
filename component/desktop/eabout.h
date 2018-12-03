/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EABOUT_H
#define __EABOUT_H

#include "ewindows.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// 这里是你的有关声明部分

static const struct dlgIDD_ABOUT{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[4];
}dlgIDD_ABOUT = {
    { WS_EX_CLOSEBOX|WS_EX_NOMOVE,0x90000000L|WS_CAPTION,4,30,70,210,180,0,0,"关于" },
    {
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,0,50,200,20,0xFFFF,classSTATIC,"巨果·Kingmos 2.0.0 ",0 },
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,0,75,200,20,0xFFFF,classSTATIC,"深圳微逻辑电子有限公司",0 },
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,0,100,200,20,0xFFFF,classSTATIC,"版权所有(C)1998-2003",0 },
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,0,125,200,20,0xFFFF,classSTATIC,"保留所有权利",0 },
    } };


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EABOUT_H
