/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：ctype
版本号：1.0.0
开发时期：2003-06-24
作者：
修改记录：
******************************************************/
#include <edef.h>
#include <estdlib.h>

static const unsigned char _ctype[256] = 
{
	    __CONTROL,               /* 00 (NUL) */
        __CONTROL,               /* 01 (SOH) */
        __CONTROL,               /* 02 (STX) */
        __CONTROL,               /* 03 (ETX) */
        __CONTROL,               /* 04 (EOT) */
        __CONTROL,               /* 05 (ENQ) */
        __CONTROL,               /* 06 (ACK) */
        __CONTROL,               /* 07 (BEL) */
        __CONTROL,               /* 08 (BS)  */
        __SPACE+__CONTROL,        /* 09 (HT)  */
        __SPACE+__CONTROL,        /* 0A (LF)  */
        __SPACE+__CONTROL,        /* 0B (VT)  */
        __SPACE+__CONTROL,        /* 0C (FF)  */
        __SPACE+__CONTROL,        /* 0D (CR)  */
        __CONTROL,               /* 0E (SI)  */
        __CONTROL,               /* 0F (SO)  */
        __CONTROL,               /* 10 (DLE) */
        __CONTROL,               /* 11 (DC1) */
        __CONTROL,               /* 12 (DC2) */
        __CONTROL,               /* 13 (DC3) */
        __CONTROL,               /* 14 (DC4) */
        __CONTROL,               /* 15 (NAK) */
        __CONTROL,               /* 16 (SYN) */
        __CONTROL,               /* 17 (ETB) */
        __CONTROL,               /* 18 (CAN) */
        __CONTROL,               /* 19 (EM)  */
        __CONTROL,               /* 1A (SUB) */
        __CONTROL,               /* 1B (ESC) */
        __CONTROL,               /* 1C (FS)  */
        __CONTROL,               /* 1D (GS)  */
        __CONTROL,               /* 1E (RS)  */
        __CONTROL,               /* 1F (US)  */
        __SPACE+__BLANK,          /* 20 SPACE */
        __PUNCT,                 /* 21 !     */
        __PUNCT,                 /* 22 "     */
        __PUNCT,                 /* 23 #     */
        __PUNCT,                 /* 24 $     */
        __PUNCT,                 /* 25 %     */
        __PUNCT,                 /* 26 &     */
        __PUNCT,                 /* 27 '     */
        __PUNCT,                 /* 28 (     */
        __PUNCT,                 /* 29 )     */
        __PUNCT,                 /* 2A *     */
        __PUNCT,                 /* 2B +     */
        __PUNCT,                 /* 2C ,     */
        __PUNCT,                 /* 2D -     */
        __PUNCT,                 /* 2E .     */
        __PUNCT,                 /* 2F /     */
        __DIGIT+__HEX,            /* 30 0     */
        __DIGIT+__HEX,            /* 31 1     */
        __DIGIT+__HEX,            /* 32 2     */
        __DIGIT+__HEX,            /* 33 3     */
        __DIGIT+__HEX,            /* 34 4     */
        __DIGIT+__HEX,            /* 35 5     */
        __DIGIT+__HEX,            /* 36 6     */
        __DIGIT+__HEX,            /* 37 7     */
        __DIGIT+__HEX,            /* 38 8     */
        __DIGIT+__HEX,            /* 39 9     */
        __PUNCT,                 /* 3A :     */
        __PUNCT,                 /* 3B ;     */
        __PUNCT,                 /* 3C <     */
        __PUNCT,                 /* 3D =     */
        __PUNCT,                 /* 3E >     */
        __PUNCT,                 /* 3F ?     */
        __PUNCT,                 /* 40 @     */
        __UPPER+__HEX,            /* 41 A     */
        __UPPER+__HEX,            /* 42 B     */
        __UPPER+__HEX,            /* 43 C     */
        __UPPER+__HEX,            /* 44 D     */
        __UPPER+__HEX,            /* 45 E     */
        __UPPER+__HEX,            /* 46 F     */
        __UPPER,                 /* 47 G     */
        __UPPER,                 /* 48 H     */
        __UPPER,                 /* 49 I     */
        __UPPER,                 /* 4A J     */
        __UPPER,                 /* 4B K     */
        __UPPER,                 /* 4C L     */
        __UPPER,                 /* 4D M     */
        __UPPER,                 /* 4E N     */
        __UPPER,                 /* 4F O     */
        __UPPER,                 /* 50 P     */
        __UPPER,                 /* 51 Q     */
        __UPPER,                 /* 52 R     */
        __UPPER,                 /* 53 S     */
        __UPPER,                 /* 54 T     */
        __UPPER,                 /* 55 U     */
        __UPPER,                 /* 56 V     */
        __UPPER,                 /* 57 W     */
        __UPPER,                 /* 58 X     */
        __UPPER,                 /* 59 Y     */
        __UPPER,                 /* 5A Z     */
        __PUNCT,                 /* 5B [     */
        __PUNCT,                 /* 5C \     */
        __PUNCT,                 /* 5D ]     */
        __PUNCT,                 /* 5E ^     */
        __PUNCT,                 /* 5F __     */
        __PUNCT,                 /* 60 `     */
        __LOWER+__HEX,            /* 61 a     */
        __LOWER+__HEX,            /* 62 b     */
        __LOWER+__HEX,            /* 63 c     */
        __LOWER+__HEX,            /* 64 d     */
        __LOWER+__HEX,            /* 65 e     */
        __LOWER+__HEX,            /* 66 f     */
        __LOWER,                 /* 67 g     */
        __LOWER,                 /* 68 h     */
        __LOWER,                 /* 69 i     */
        __LOWER,                 /* 6A j     */
        __LOWER,                 /* 6B k     */
        __LOWER,                 /* 6C l     */
        __LOWER,                 /* 6D m     */
        __LOWER,                 /* 6E n     */
        __LOWER,                 /* 6F o     */
        __LOWER,                 /* 70 p     */
        __LOWER,                 /* 71 q     */
        __LOWER,                 /* 72 r     */
        __LOWER,                 /* 73 s     */
        __LOWER,                 /* 74 t     */
        __LOWER,                 /* 75 u     */
        __LOWER,                 /* 76 v     */
        __LOWER,                 /* 77 w     */
        __LOWER,                 /* 78 x     */
        __LOWER,                 /* 79 y     */
        __LOWER,                 /* 7A z     */
        __PUNCT,                 /* 7B {     */
        __PUNCT,                 /* 7C |     */
        __PUNCT,                 /* 7D }     */
        __PUNCT,                 /* 7E ~     */
        __CONTROL,               /* 7F (DEL) */
        /* and the rest are 0... */
};


int _isctype( int c, int mask )
{
	if( c <= 0xff )
		return _ctype[c] & mask;
	return 0;
}

int std_isalpha( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & __ALPHA;
	return 0;
}

int std_isupper( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & __UPPER;
	return 0;
}

int std_islower( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & __LOWER;
	return 0;
}

int std_isdigit( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & __DIGIT;
	return 0;
}

int std_isxdigit( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & (__DIGIT|__HEX);
	return 0;
}

int std_isspace( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & (__SPACE);
	return 0;
}

int std_ispunct( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & (__PUNCT);
	return 0;
}

int std_isalnum( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & (__DIGIT|__ALPHA);
	return 0;
}

int std_isprint( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & (__BLANK|__PUNCT|__UPPER|__LOWER|__DIGIT);
	return 0;   
}

int std_isgraph( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & (__PUNCT|__UPPER|__LOWER|__DIGIT);
	return 0;	
}

int std_iscntrl( int c )
{
	if( c <= 0xff )
	    return _ctype[c] & __CONTROL;
	return 0;	
}

int std_toupper( int c )
{
	if( c <= 0xff )
	{
		return (_ctype[c] & __LOWER) ?  ( c-'a'+'A' ) : c;
	}
	return c;	
}


int std_tolower( int c )
{
	if( c <= 0xff )
	{
		return (_ctype[c] & __UPPER) ?  ( c-'A'+'a' ) : c;
	}
	return c;	
}

