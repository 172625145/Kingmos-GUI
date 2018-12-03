/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EOBJLIST_H
#define __EOBJLIST_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

typedef struct _OBJLIST
{
    DWORD  objType;  // the three member must same 
	struct _OBJLIST  *lpNext;   // in pen, brush, font, bitmap, region, palette
	struct _OBJLIST  *lpPrev;
	ULONG  ulOwner;   // instance handle or thread handle
	int    iRefCount;  // referent count
}OBJLIST, FAR * LPOBJLIST;


BOOL FASTCALL ObjList_Init( LPOBJLIST * lppList, LPOBJLIST lpObj, DWORD objType, ULONG ulOwner );
BOOL FASTCALL ObjList_Add( LPOBJLIST * lppList, LPOBJLIST lpObj );
BOOL FASTCALL ObjList_Remove( LPOBJLIST * lppList, LPOBJLIST lpObj );
// add referent count 
//int FASTCALL ObjList_AddRef( LPOBJLIST lpObj );
//int FASTCALL ObjList_AddRefCountNum( LPOBJLIST lpObj, int iAddNum );
// dev referent count
//int FASTCALL ObjList_DecRef( LPOBJLIST lpObj );
//int FASTCALL ObjList_DecRefCountNum( LPOBJLIST lpObj, int iDecNum );

int FASTCALL ObjList_Check( LPOBJLIST * lppList );

typedef BOOL ( CALLBACK * DELOBJPROC ) ( LPOBJLIST lpObj, LPARAM lParam );

BOOL FASTCALL ObjList_Delete( LPOBJLIST * lppList, ULONG ulOwner, DELOBJPROC lpDelProc, LPARAM lParam );

// format:
// FR PP PPPP PPPP
//#define HANDLE_BASE( h ) ( ( (DWORD)&(h) ) & 0xFF000000 )
//#define PTR_TO_HANDLE( ptr ) ( ( ( random() << 24 ) & 0x0F000000 ) | 0xC0000000 | ( (DWORD)( ((DWORD)ptr)&0x03FFFFFF ) >> 2 ) )
//#define HANDLE_TO_PTR( h ) ( ( (DWORD)h & 0xF0000000 ) == 0xC0000000 ? ( ( (DWORD)h & 0x00FFFFFF ) << 2 ) | HANDLE_BASE(h) : 0 )

#define HANDLE_PTR_MASK 0x1FFFFFFC

#ifdef INLINE_PROGRAM
	#define HANDLE_BASE( h ) ( ( (DWORD)&(h) ) & 0xE0000000 )
#else
	#define HANDLE_BASE( h ) (0)
#endif

//#define PTR_TO_HANDLE( ptr ) (HANDLE)( ( random() & 0x3 ) |  ( (DWORD)ptr & HANDLE_PTR_MASK ) | 0x40000000 )
//#define HANDLE_TO_PTR( h ) ( ( (DWORD)h & 0xE0000000 ) == 0x40000000 ? ( ( (DWORD)h & HANDLE_PTR_MASK ) | HANDLE_BASE(h) ) : 0 )
#define PTR_TO_HANDLE( ptr ) ((HANDLE)( ptr ))
#define HANDLE_TO_PTR( h ) (  ( (int) (h) ) == -1 ? 0 : (h) )

#ifdef __cplusplus
}
#endif  // __cplusplus


#endif   //__EOBJLIST_H
