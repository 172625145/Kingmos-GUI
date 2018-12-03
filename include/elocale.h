/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：
版本号：  1.0.0
开发时期：2003-03-06
作者：    周兵
修改记录：
******************************************************/

#ifndef _KINGMOS_INC_LOCALE
#define _KINGMOS_INC_LOCALE

#ifdef  __cplusplus
extern "C" {
#endif


#define	_CRTIMP
#define __cdecl
#define _CRTAPI1

typedef char CCHAR;          
typedef DWORD LCID;         
typedef PDWORD PLCID;       
typedef WORD   LANGID;      

typedef DWORD LCTYPE;


/* define NULL pointer value */

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

/* Locale categories */

#define LC_ALL          0
#define LC_COLLATE      1
#define LC_CTYPE        2
#define LC_MONETARY     3
#define LC_NUMERIC      4
#define LC_TIME         5

#define LC_MIN          LC_ALL
#define LC_MAX          LC_TIME

/* Locale convention structure */

#ifndef _LCONV_DEFINED
struct lconv {
        char *decimal_point;
        char *thousands_sep;
        char *grouping;
        char *int_curr_symbol;
        char *currency_symbol;
        char *mon_decimal_point;
        char *mon_thousands_sep;
        char *mon_grouping;
        char *positive_sign;
        char *negative_sign;
        char int_frac_digits;
        char frac_digits;
        char p_cs_precedes;
        char p_sep_by_space;
        char n_cs_precedes;
        char n_sep_by_space;
        char p_sign_posn;
        char n_sign_posn;
        };
#define _LCONV_DEFINED
#endif

/* ANSI: char lconv members default is CHAR_MAX which is compile time
   dependent. Defining and using _charmax here causes CRT startup code
   to initialize lconv members properly */

#ifdef  _CHAR_UNSIGNED
//extern int _charmax;
//extern __inline int __dummy() { return _charmax; }
#endif

/* function prototypes */

//_CRTIMP char * __cdecl setlocale(int, const char *);
//_CRTIMP struct lconv * __cdecl localeconv(void);

LCID THREAD_GetThreadLocale(void);
#define GetThreadLocale THREAD_GetThreadLocale


char * local_setlocale(int, const char *);
#define setlocale local_setlocale

int local_GetLocaleInfo(
  LCID Locale,      // locale identifier
  LCTYPE LCType,    // information type
  LPTSTR lpLCData,  // information buffer
  int cchData       // size of buffer
);

#define GetLocaleInfo local_GetLocaleInfo

#ifdef  __cplusplus
}
#endif


#endif  /* _INC_LOCALE */
