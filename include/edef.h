/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EDEF_H
#define __EDEF_H

#ifndef __EVERSION_H
#include <eversion.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

// define MS DOS Complier & X86 CPU
#ifdef __MSDC__X86__
    #define VOID                void
    #define HUGE                _huge
    #define FAR                 _far
    #define NEAR                _near
    #define PASCAL              _pascal
    #define CDECL               _cdecl
    #define WINAPI              _far _pascal
    #define CALLBACK            _far _pascal
    #define FASTCALL            _far _pascal
	#define _PACKED_
    #define _ARMCC_PACKED_
#endif    /*__MSDC__X86__*/

// define MS WINDOWS Complier & X86 CPU
#ifdef __MSWC__X86__
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              __stdcall
    #define CDECL               _cdecl
    #define WINAPI              __stdcall 
    #define CALLBACK            __stdcall
    #define FASTCALL            __stdcall
	typedef          __int64        INT64;
	typedef          unsigned __int64    UINT64;
	#define _PACKED_
    #define _ARMCC_PACKED_
#endif   /*__MSWC__X86__*/

// define MS WINDOWS Complier & ARM CPU
#ifdef __MSWC__ARM__
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              __stdcall
    #define CDECL               _cdecl
    #define WINAPI              __stdcall 
    #define CALLBACK            __stdcall
    #define FASTCALL            __stdcall
	typedef          __int64        INT64;
	typedef          unsigned __int64    UINT64;
    #define _PACKED_
	#define _ARMCC_PACKED_
#endif   /*__MSWC__ARM__*/

// define GNU Complier & ARM CPU
#ifdef __GNUC__ARM__
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              
    #define CDECL               
    #define WINAPI               
    #define CALLBACK            
    #define FASTCALL            
	typedef   long long        INT64;
	typedef   unsigned long long   UINT64;
    #define _PACKED_		__attribute__((packed))
	#define _ARMCC_PACKED_
#endif   // __GNUC__ARM__

#ifdef __GNUC__MIPS__
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              
    #define CDECL               
    #define WINAPI               
    #define CALLBACK            
    #define FASTCALL            
	typedef   long long        INT64;
	typedef   unsigned long long   UINT64;
    #define _PACKED_		__attribute__((packed))
	#define _ARMCC_PACKED_
#endif   // __GNUC__MIPS__


	

#ifdef __ARMCC__ARM__

    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              
    #define CDECL               
    #define WINAPI               
    #define CALLBACK            
    #define FASTCALL            
	typedef   long long        INT64;
	typedef   unsigned long long   UINT64;
	#define _PACKED_
    #define _ARMCC_PACKED_           __packed
#endif

// define GNU Complier & I386 CPU
#ifdef __GNUC__I386__
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              __attribute__((stdcall))
    #define CDECL               __attribute__((cdecl))
    #define WINAPI              __attribute__((stdcall)) 
    #define CALLBACK            __attribute__((stdcall))
    #define FASTCALL            __attribute__((stdcall))
#endif   // __GNUC__I386__
/*
#ifdef EML_DOS
    #define VOID                void
    #define HUGE                _huge
    #define FAR                 _far
    #define NEAR                _near
    #define PASCAL              _pascal
    #define CDECL               _cdecl
    #define WINAPI              _far _pascal
    #define CALLBACK            _far _pascal
    #define FASTCALL            _far _pascal
#endif    // CPU_X86

#ifdef CPU_68K
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL
    #define CDECL               _cdecl
    #define WINAPI
    #define CALLBACK
    #define FASTCALL
#endif   // CPU_68K

#ifdef EML_WIN32
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              __stdcall
    #define CDECL               _cdecl
    #define WINAPI              __stdcall 
    #define CALLBACK            __stdcall
    #define FASTCALL            __stdcall
#endif   // EML_WIN32

#ifdef CPU_ARM
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              __stdcall
    #define CDECL               _cdecl
    #define WINAPI              __stdcall 
    #define CALLBACK            __stdcall
    #define FASTCALL            __stdcall
#endif   // CPU_ARM

#ifdef CPU_I386
    #define VOID                void
    #define HUGE
    #define FAR
    #define NEAR
    #define PASCAL              __stdcall
    #define CDECL               _cdecl
    #define WINAPI              __stdcall 
    #define CALLBACK            __stdcall
    #define FASTCALL            __stdcall
#endif   // CPU_I386
*/

typedef void *       HANDLE;
typedef HANDLE FAR * LPHANDLE;

typedef HANDLE       HGDIOBJ;
typedef HANDLE       HFILE;
#define DEFINE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ FAR* name


typedef int                 BOOL;
#define FALSE               0
#define TRUE                1

#define CONST               const

#ifndef USE_PLATFORM_DEFINE
typedef unsigned int        size_t;
typedef unsigned long		time_t;
typedef unsigned short       wchar_t;
#endif

typedef unsigned int        SIZE_T;
typedef unsigned short      WCHAR_T;


typedef char                CHAR;
typedef unsigned char       UCHAR;
typedef char                TCHAR;
typedef unsigned short      WCHAR;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned int        UINT;

typedef          char       INT8;
typedef          short      INT16;
typedef          int        INT32;

typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;

typedef signed long         LONG;
typedef unsigned long       ULONG;
typedef short               SHORT;
typedef unsigned short      USHORT;

typedef short               ATOM;
typedef LONG WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;
typedef char FAR*           LPSTR;
typedef const char FAR*     LPCSTR;
typedef char FAR*           LPTSTR;
typedef const char FAR*     LPCTSTR;

typedef WCHAR *             LPWSTR;
typedef const WCHAR *       LPCWSTR;

typedef BYTE FAR*           LPBYTE;
typedef const BYTE FAR*     LPCBYTE;
typedef int FAR*            LPINT;
typedef unsigned int FAR*   LPUINT;
typedef WORD FAR*           LPWORD;
typedef const WORD FAR*     LPCWORD;
typedef unsigned short FAR * LPUSHORT;
typedef long FAR*           LPLONG;
typedef unsigned long FAR*  LPULONG;
typedef DWORD FAR*          LPDWORD;
typedef const DWORD FAR*    LPCDWORD;
typedef void FAR*           LPVOID;
typedef const void FAR*     LPCVOID;

typedef unsigned char *     PUCHAR;
typedef BYTE *              PBYTE;
typedef const BYTE *        PCBYTE;
typedef int *               PINT;
typedef WORD *              PWORD;
typedef unsigned short *    PUSHORT;
typedef const unsigned short * PCWORD;
typedef long *              PLONG;
typedef unsigned long *     PULONG;

typedef DWORD *             PDWORD;
typedef void *              PVOID;
typedef const void *        PCVOID;

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    INT64 QuadPart;
} LARGE_INTEGER;

typedef LARGE_INTEGER *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    };
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    UINT64 QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER *PULARGE_INTEGER;


typedef void (*PFNVOID)();


typedef DWORD   COLORREF;
typedef DWORD   *LPCOLORREF;


#ifndef	IN
#define	IN
#endif

#ifndef	OUT
#define	OUT
#endif

#ifndef	OPTIONAL
#define	OPTIONAL
#endif

#define	DWORD_4B(dw)				( (BYTE)(dw>>(24)) )
#define	DWORD_3B(dw)				( (BYTE)(dw>>(16)) )
#define	DWORD_2B(dw)				( (BYTE)(dw>>(8)) )
#define	DWORD_1B(dw)				( (BYTE)(dw) )
#define	MAKEDWORD4(b1,b2,b3,b4)		( (DWORD)( b1 | ((DWORD)b2<<8) | ((DWORD)b3<<16) | ((DWORD)b4<<24) ) )

DEFINE_HANDLE(HWND);
DEFINE_HANDLE(HMENU);
DEFINE_HANDLE(HINSTANCE);
DEFINE_HANDLE(HTASK);
DEFINE_HANDLE(HDC);
DEFINE_HANDLE(HRGN);
DEFINE_HANDLE(HICON);
DEFINE_HANDLE(HCURSOR);
DEFINE_HANDLE(HBRUSH);
//DEFINE_HANDLE(HANDLE);
DEFINE_HANDLE(HBITMAP);
DEFINE_HANDLE(HPEN);
DEFINE_HANDLE(HFONT);
//DEFINE_HANDLE(HGDIOBJ);
//DEFINE_HANDLE(HFILE);
DEFINE_HANDLE(HTABLE);
//DEFINE_HANDLE(HTHREAD);
DEFINE_HANDLE(HMODULE);
DEFINE_HANDLE(HKEY);
DEFINE_HANDLE(HPALETTE);
typedef HKEY * PHKEY;

#define INVALID_HANDLE_VALUE ((HANDLE)(-1))
 
typedef LRESULT (CALLBACK* WNDPROC)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef VOID (CALLBACK* TIMERPROC)(HWND hWnd, UINT msg, UINT idEvent, DWORD dwTickCount);
typedef BOOL (CALLBACK* DLGPROC)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef int ( WINAPI * WINMAINPROC)( HINSTANCE, HINSTANCE, LPTSTR, int );

typedef int (CALLBACK *FARPROC)();

#undef  NULL
#define NULL                0

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)((WORD)(w) >> 8))

#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))


#define	MAKEDWORD(lo,hi)			( (DWORD)( lo | ((DWORD)hi<<16) ) )
#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)))
#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))

#define MAKEWPARAM (WPARAM)MAKELONG
#define MAKELPARAM (LPARAM)MAKELONG
#define MAKELRESULT (LRESULT)MAKELONG

//#define MAKEPOINTS(l)       (*((POINTS FAR *)&(l)))
#undef  MAX
#define MAX(v1, v2) ((v1) > (v2) ? (v1) : (v2))

#undef  MIN
#define MIN(v1, v2) ((v1) < (v2) ? (v1) : (v2))

#undef  max
#define max(v1, v2) ((v1) > (v2) ? (v1) : (v2))

#undef  min
#define min(v1, v2) ((v1) < (v2) ? (v1) : (v2))

#define TEXT( str )  ( str )

//
#define MINCHAR     0x80        
#define MAXCHAR     0x7f        
#define MINSHORT    0x8000      
#define MAXSHORT    0x7fff      
#define MINLONG     0x80000000  
#define MAXLONG     0x7fffffff  
#define MAXBYTE     0xff        
#define MAXWORD     0xffff      
#define MAXDWORD    0xffffffff  
//

#define TEXT_PREFIX_WORD '&'
#define TEXT_EOF 0
#define TEXT_SOFT_BREAK '\r'
#define TEXT_KEY_BREAK  '\n'

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EDEF_H

