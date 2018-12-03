/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�������Դ���
�汾�ţ�1.0.0
����ʱ�ڣ�2001-01-04
���ߣ�����
�޸ļ�¼��
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
//������void EdbgOutputDebugString( const unsigned char *lpszFormat, ... )
//������
//	IN lpszFormat-�ַ�����ʽ
//����ֵ����
//������������ʽ���ַ���������Կ����
//          ֧����������
//            %u = unsigned
//            %d = int
//            %c = char
//            %s = string
//            %x = 4-bit hex number
//            %B = 8-bit hex number
//            %H = 16-bit hex number
//            %X = 32-bit hex number
//����: 
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
//������void pOutputByte( unsigned char c )
//������
//	IN c-�ַ�
//����ֵ����
//�������������Կ����һ���ַ�
//����: 
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
//������void OutputNumHex( unsigned long n, long depth )
//������
//	IN n-��Ҫ�������ֵ
//  IN depth-��Ҫ�������ֵ�ĳ��ȣ��� 08 �� 0008
//����ֵ����
//�������������16������ֵ
//����: 
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
//������void OutputNumDecimal( unsigned long n )
//������
//	IN n-��Ҫ�������ֵ
//����ֵ����
//�������������10������ֵ
//����: 
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
//������void OutputString( const char *lpsz )
//������
//	IN lpsz-��Ҫ������ַ���ָ��
//����ֵ����
//��������������ַ���
//����: 
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
