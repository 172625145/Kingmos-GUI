/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __FINDDATA_H
#define __FINDDATA_H

#include "eWindows.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define IDC_OK		101
#define IDC_FINDTEXT	102
#define IDC_CANCEL		103

// 这里是你的有关声明部分

const struct dlgFindTemplate{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[4];
}dlgFindTemplate = {
    { WS_EX_NOMOVE,0x90000000L|WS_CAPTION,4,30,80,210,100,0,0,"查找" },
    {
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,10,16,40,20,0xFFFF,classSTATIC,"名称:",0 },
        { WS_EX_CLIENTEDGE,WS_CHILD|WS_VISIBLE,55,16,140,20,IDC_FINDTEXT,"EDIT","",0 },
        { 0,WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,30,45,50,20,IDC_OK,"BUTTON","查找",0 },
        { 0,WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,100,45,50,20,IDC_CANCEL,"BUTTON","取消",0 },
    } };

const struct dlgShowPropertyTemplate{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[2];
}dlgShowPropertyTemplate = {
    { WS_EX_NOMOVE|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,2,30,80,210,158,0,0,"属性" },
    {
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,10,10,40,20,0xFFFF,classSTATIC,"名称:",0 },
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,10,35,40,20,0xFFFF,classSTATIC,"类型:",0 },
//        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,10,64,40,20,0xFFFF,classSTATIC,"大小:",0 },
//        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,10,88,72,20,0xFFFF,classSTATIC,"创建时间:",0 },
//        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,10,112,72,20,0xFFFF,classSTATIC,"修改时间:",0 },
    } };


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__FINDDATA_H
