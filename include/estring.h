/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ESTRING_H
#define __ESTRING_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

SIZE_T str_len( const char *string );
#define strlen str_len

char *str_ncpy( char *strDest, const char *strSource, SIZE_T count );
#define strncpy str_ncpy

char *str_cpy( char *strDest, const char *strSource );
#define strcpy str_cpy

char *str_cat( char *strDest, const char *strSource );
#define strcat str_cat

char *str_ncat( char *strDest, const char *strSource, SIZE_T count );
#define strncat str_ncat


int str_cmp( const char *string1, const char *string2 );
#define strcmp str_cmp

int str_ncmp( const char *string1, const char *string2, SIZE_T count );
#define strncmp str_ncmp

int str_icmp( const char *string1, const char *string2 );
#define stricmp   str_icmp
#define _stricmp  str_icmp

int str_nicmp( const char *string1, const char *string2, SIZE_T count );
#define strnicmp   str_nicmp
#define _strnicmp  str_nicmp

char *str_chr( const char *s1, int c );
#define strchr str_chr

char *str_rchr( const char *s1, int c );
#define strrchr str_rchr

char * str_strstr( const char * str1, const char * str2 );
#define strstr   str_strstr

char * str_istr( const char * str1, const char * str2 );
#define stristr str_stristr

char *str_strdup( const char *str );
#define strdup str_strdup
#define _strdup str_strdup

void *mem_cpy( void *dest, const void *src, SIZE_T count );
#define memcpy  mem_cpy

void *mem_move( void *dest, const void *src, SIZE_T count );
#define memmove mem_move

void *mem_set( void *dest, int c, SIZE_T count );
#define memset mem_set

int mem_cmp( const void *buf1, const void *buf2, SIZE_T count );
#define memcmp mem_cmp

void *mem_chr( const void *buf, int c, SIZE_T count );
#define memchr mem_chr



SIZE_T wcs_len( const WCHAR_T *string );
#define wcslen wcs_len

WCHAR_T *wcs_ncpy( WCHAR_T *strDest, const WCHAR_T *strSource, SIZE_T count );
#define wcsncpy wcs_ncpy

WCHAR_T *wcs_cpy( WCHAR_T *strDest, const WCHAR_T *strSource );
#define wcscpy wcs_cpy

WCHAR_T *wcs_cat( WCHAR_T *strDest, const WCHAR_T *strSource );
#define wcscat wcs_cat

WCHAR_T *wcs_ncat( WCHAR_T *strDest, const WCHAR_T *strSource, SIZE_T count );
#define wcsncat wcs_ncat

int wcs_cmp( const WCHAR_T *string1, const WCHAR_T *string2 );
#define wcscmp wcs_cmp

int wcs_ncmp( const WCHAR_T *string1, const WCHAR_T *string2, SIZE_T count );
#define wcsncmp wcs_ncmp

int wcs_icmp( const WCHAR_T *string1, const WCHAR_T *string2 );
#define wcsicmp   wcs_icmp
#define _wcsicmp  wcs_icmp

int wcs_nicmp( const WCHAR_T *string1, const WCHAR_T *string2, SIZE_T count );
#define wcsnicmp   wcs_nicmp
#define _wcsnicmp  wcs_nicmp

WCHAR_T * wcs_strstr( const WCHAR_T *string1, const WCHAR_T *string2 );
#define wcsstr   wcs_strstr

WCHAR_T * wcs_stristr( const WCHAR_T *string1, const WCHAR_T *string2 );
#define wcsistr   wcs_stristr

WCHAR_T *wcs_chr( const WCHAR_T *s1, int c );
#define wcschr wcs_chr

WCHAR_T *wcs_rchr( const WCHAR_T *s1, int c );
#define wcsrchr wcs_rchr
// 

#ifdef __cplusplus
}
#endif  //__cplusplus


#endif  //__ESTRING_H
