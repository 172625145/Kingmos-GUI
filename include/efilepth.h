/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#include "ewindows.h"

#ifndef __EFILEPTH_H
#define __EFILEPTH_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus 


void _splitpath( const char *path, char *drive, char *dir, char *fname, char *ext);
void _makepath( char *path, const char *drive, const char *dir, const char *fname, const char *ext );

#ifdef KINGMOS_UNICODE

//typedef unsigned short wchar_t;
void _wsplitpath( const wchar_t *path, wchar_t *drive, wchar_t *dir, wchar_t *fname, wchar_t *ext );
void _wmakepath( wchar_t *path, const wchar_t *drive, const wchar_t *dir, const wchar_t *fname, const wchar_t *ext );

#endif

#ifdef __cplusplus
}           

#endif  // __cplusplus
#endif  //__EFILEPTH_H
