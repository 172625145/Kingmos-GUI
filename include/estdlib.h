/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ESTDLIB_H
#define __ESTDLIB_H

#ifndef __EDEF_H
#include <edef.h>
#endif

#ifndef __ESTDARG_H
#include <estdarg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

void std_srand(unsigned seed);
#define srand std_srand

int std_rand(void); 
#define rand std_rand

unsigned long std_random( void );
#define random std_random

int std_toupper( int c );
#define toupper std_toupper

int std_tolower( int c );
#define tolower std_tolower

char * std_itoa( int val, char *buf, int radix );
#define itoa std_itoa

char * std_ltoa( long val, char *buf, int radix );
#define ltoa std_ltoa

char * std_ultoa( unsigned long val, char *buf, int radix );
#define ultoa std_ultoa

long std_atol( const char *str );
#define atol std_atol

int std_atoi( const char *str );
#define atoi std_atoi

unsigned int std_sprintf( char *pbuf, const char *format, ... );
#define sprintf std_sprintf

unsigned int std_vsprintf( char *pbuf, const char *format, va_list argptr );
#define vsprintf std_vsprintf

unsigned long std_strtoul( const char *pn, char **pend, int ibase );
#define strtoul std_strtoul

long std_strtol( const char *pn, char **pend, int ibase );
#define strtol std_strtol

char *std_getenv( const char *varname );
#define getenv std_getenv

INT64 std_atoll( const char *ptr );
#define atoll std_atoll

INT64 std_atoi64( const char *ptr );
#define _atoi64 std_atoi64

// errno
extern int errno;
#define ERANGE      34

// ctype
// set bit masks of character types

#define __DIGIT          0x01     // 0->9
#define __SPACE          0x02     // tab, carriage return, newline
                               // vertical tab or form feed
#define __UPPER          0x04     // 'A'-'Z'
#define __LOWER          0x08     // 'a'-'z'

#define __HEX            0x10   // hexadecimal digit
#define __CONTROL        0x20   // control character
#define __BLANK          0x40   // space char
#define __PUNCT          0x80   // punctuation character

#define __ALPHA          (__UPPER|__LOWER)  // alphabetic character

//int _isctype(int, int);
int std_isalpha(int);
#undef isalpha
#define isalpha std_isalpha

int std_isupper(int);
#undef isupper
#define isupper std_isupper

int std_islower(int);
#undef islower
#define islower std_islower

int std_isdigit(int);
#undef isdigit
#define isdigit std_isdigit

int std_isxdigit(int);
#undef isxdigit
#define isxdigit std_isxdigit

int std_isspace(int);
#undef isspace
#define isspace std_isspace

int std_ispunct(int);
#undef ispunct
#define ispunct std_ispunct

int std_isalnum(int);
#undef isalnum
#define isalnum std_isalnum

int std_isprint(int);
#undef isprint
#define isprint std_isprint

int std_isgraph(int);
#undef isgraph
#define isgraph std_isgraph

int std_iscntrl(int);
#undef iscntrl
#define iscntrl std_iscntrl

//int std_toupper(int);
//#define toupper std_toupper

//int std_tolower(int);
//#define tolower std_tolower

//int _tolower(int);
//int _toupper(int);

#ifndef __isascii
#define __isascii(_c)   ( (unsigned)(_c) < 0x80 )
#endif

#ifndef __toascii
#define __toascii(_c)   ( (_c) & 0x7f )
#endif

#ifndef isascii
#define isascii(c)  __isascii(c)
#endif

#ifndef toascii
#define toascii(c)  __toascii(c)
#endif

//


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__ESTDLIB_H
