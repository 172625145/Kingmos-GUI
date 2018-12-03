/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EALLOC_H
#define __EALLOC_H

#ifndef __EDEF_H
#include <edef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus


void Mem_Free( void FAR* usedp );
void * Mem_Alloc( DWORD sizeneeded );
void * Mem_Realloc( void *old, DWORD sz );
void * Mem_Calloc( DWORD num, DWORD size );

#ifdef DEBUG_ALLOC

void Debug_Mem_Free( void FAR* usedp, char * file, int len );
void * Debug_Mem_Alloc( DWORD sizeneeded, char * file, int len );
void * Debug_Mem_Realloc( void *old, DWORD sz, char * file, int len );

#define free( p ) Debug_Mem_Free( (p), __FILE__, __LINE__ )
#define malloc( sz ) Debug_Mem_Alloc( (sz), __FILE__, __LINE__ )
#define realloc( p, sz ) Debug_Mem_Realloc( (p), (sz), __FILE__, __LINE__ )
#define calloc Debug_Mem_Calloc

#else

#define free Mem_Free
#define malloc Mem_Alloc
#define realloc Mem_Realloc
#define calloc Mem_Calloc

#endif

#ifdef __cplusplus
}
#endif  // __cplusplus


#endif   // __EALLOC_H




