/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __GWMESRV_H
#define __GWMESRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <eapisrv.h>
#include <emenu.h>
#include <winsrv.h>
#include <gdisrv.h>
#include <menusrv.h>

DWORD WINAPI GwmeServerHandler( HANDLE hServer, DWORD dwServercode , DWORD dwParam, LPVOID lpParam );

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // __GWMESRV_H


