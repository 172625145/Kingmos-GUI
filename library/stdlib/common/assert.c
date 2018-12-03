/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：调试处理
版本号：1.0.0
开发时期：2001-01-04
作者：李林
修改记录：
******************************************************/

#include <ewindows.h>
#include <estdarg.h>

#ifndef INLINE_AP

static void pOutputByte(unsigned char c);

static void OutputNumHex(unsigned long n,long depth);
static void OutputNumDecimal(unsigned long n);
static void OutputString(const char *s);
static void pFlushDebug( void );

static char * szSprintf;

BOOL _InitDebug(void)
{
	return 1;
}
//extern unsigned int _out_stream( unsigned char *pbuf, const unsigned char *format, va_list vl );

#ifdef EML_WIN32
#define MAX_DEBUG_BUFFER 1024
static char strFormat[MAX_DEBUG_BUFFER];
static int indexFormat = 0;
#endif


static void VL_EdbgOutputDebugString( const char * lpszFormat,  va_list vl )
{
    unsigned char   c;
    //va_list         vl;
    
    //va_start( vl, lpszFormat );
   
    while( *lpszFormat )
	{
        c = *lpszFormat++;
        switch (c) { 
		case (unsigned char)'%':
            c = *lpszFormat++;
            switch (c) { 
			case 'x':
                OutputNumHex(va_arg(vl, unsigned long), 0);
                break;
			case 'B':
                OutputNumHex(va_arg(vl, unsigned long), 2);
                break;
			case 'H':
                OutputNumHex(va_arg(vl, unsigned long), 4);
                break;
			case 'X':
                OutputNumHex(va_arg(vl, unsigned long), 8);
                break;
			case 'd': 
				{
					long    l;
					
					l = va_arg(vl, long);
					if (l < 0) 
					{ 
						pOutputByte('-');
						l = - l;
					}
					OutputNumDecimal((unsigned long)l);
				}
                break;
			case 'u':
                OutputNumDecimal(va_arg(vl, unsigned long));
                break;
			case 's':
                OutputString(va_arg(vl, char *));
                break;
			case '%':
                pOutputByte('%');
                break;
			case 'c':
                c = va_arg(vl, unsigned char);
                pOutputByte(c);
                break;
                
			default:
                pOutputByte(' ');
                break;
            }
            break;
            case '\n':
				pOutputByte('\r');
				// fall through
            default:
				pOutputByte(c);
        }
    }
    
//    va_end(vl);
}


// ********************************************************************
//声明：void EdbgOutputDebugString( const unsigned char *lpszFormat, ... )
//参数：
//	IN lpszFormat-字符串格式
//返回值：无
//功能描述：格式化字符串并向调试口输出
//          支持以下类型
//            %u = unsigned
//            %d = int
//            %c = char
//            %s = string
//            %x = 4-bit hex number
//            %B = 8-bit hex number
//            %H = 16-bit hex number
//            %X = 32-bit hex number
//引用: 
// ********************************************************************

void EdbgOutputDebugString( const char * lpszFormat, ... )
{
    va_list         vl;

    va_start( vl, lpszFormat );
    
	VL_EdbgOutputDebugString( lpszFormat, vl );
	pFlushDebug();

    va_end(vl);
}

void EdbgOutputWarnString( const char *sz, ... )
{

	extern void WarnSound( void );
    va_list         vl;
    
    va_start( vl, sz );

	VL_EdbgOutputDebugString( sz, vl );
	pFlushDebug();

	va_end(vl);

	/*
	static int in = 0;
    
	int i;
    
    in++;	

	if( in == 1 )
	{
        va_list         vl;
		va_start(vl, sz);
		if( (i = _out_stream( strFormat, sz, vl )) != 0 )
		{
			strFormat[i] = 0;
			DbgOutString( strFormat );
		}
	}

	in--;
	*/
#ifdef EML_WIN32
	{
		//extern void WarnSound( void );
		//WarnSound();	
	}
#endif
}



// ********************************************************************
//声明：void pOutputByte( unsigned char c )
//参数：
//	IN c-字符
//返回值：无
//功能描述：调试口输出一个字符
//引用: 
// ********************************************************************

static void pOutputByte( unsigned char c )
{
#ifdef EML_WIN32
	if( indexFormat < (MAX_DEBUG_BUFFER-1) )
	{
	    strFormat[indexFormat] = c;
		indexFormat++;
	}
	else
	{
		strFormat[indexFormat] = 0;
		DbgOutString( strFormat );
		indexFormat = 0;
	}
#else
	char str[2];
	str[0] = c;
	str[1] = 0;
	DbgOutString( str );
#endif

}

static void pFlushDebug( void )
{
#ifdef EML_WIN32
	strFormat[indexFormat] = 0;
	DbgOutString( strFormat );
	indexFormat = 0;
#endif
}

// ********************************************************************
//声明：void OutputNumHex( unsigned long n, long depth )
//参数：
//	IN n-需要输出的数值
//  IN depth-需要输出的数值的长度，如 08 或 0008
//返回值：无
//功能描述：输出16进制数值
//引用: 
// ********************************************************************

static void OutputNumHex( unsigned long n, long depth )
{
    if( depth )
        depth--;    
    if ( ( n & ~0xf ) || depth )
	{
        OutputNumHex( n >> 4, depth );
        n &= 0xf;
    }    
    if( n < 10 )
        pOutputByte( (unsigned char)(n + '0') );
	else
		pOutputByte( (unsigned char)(n - 10 + 'A') );
}


// ********************************************************************
//声明：void OutputNumDecimal( unsigned long n )
//参数：
//	IN n-需要输出的数值
//返回值：无
//功能描述：输出10进制数值
//引用: 
// ********************************************************************

static void OutputNumDecimal( unsigned long n )
{
    if( n >= 10 )
	{
        OutputNumDecimal( n / 10 );
        n %= 10;
    }
    pOutputByte( (unsigned char)(n + '0') );
}

// ********************************************************************
//声明：void OutputString( const char *lpsz )
//参数：
//	IN lpsz-需要输出的字符串指针
//返回值：无
//功能描述：输出字符串
//引用: 
// ********************************************************************
static void OutputString( const char *lpsz )
{
	if( lpsz )
	{
		while( *lpsz )
		{		
			if ( *lpsz == '\n' )
			{
				pOutputByte('\r');
			}
			pOutputByte( *lpsz++ );
		}
	}
}


void __AssertFail( char *__msg, char *__cond, char *__file, int __line, char * __notify )
{	
	if( __notify )
	{
		EdbgOutputDebugString( "%s", __notify );
	}

	EdbgOutputDebugString(__msg, __cond, __file, __line);
}
/*
void TestAssert( void )
{
	ASSERT( 0 );
}
*/
#endif
