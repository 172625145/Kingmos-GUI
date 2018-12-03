/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：符号字体字模驱动程序
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：

******************************************************/
#include <eframe.h>
#include <gwmeobj.h>

//字模
#include "symmask16x16.c"
#include "symmask24x24.c"
//extern const BYTE __bSymbolsFontMask16x16[];
//extern const BYTE __bSymbolsFontMask16x16[];

static DWORD _Symbol_Init(void);
static BOOL _Symbol_Deinit( DWORD dwData );
static BOOL _Symbol_InstallFont( DWORD dwData, LPCTSTR lpszPathName );
static HANDLE _Symbol_CreateFont( DWORD dwData, const LOGFONT *lplf );
static BOOL _Symbol_DeleteFont( HANDLE );
static int _Symbol_MaxHeight( HANDLE );
static int _Symbol_MaxWidth( HANDLE );
static int _Symbol_WordLength( HANDLE, const BYTE FAR* lpText );
static int _Symbol_WordHeight( HANDLE, WORD aWord );
static int _Symbol_WordWidth( HANDLE, WORD aWord );
static int _Symbol_WordMask( HANDLE, const BYTE FAR* lpText, _LPCHAR_METRICS );
static int _Symbol_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int _Symbol_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static const BYTE FAR* _Symbol_NextWord( HANDLE, const BYTE FAR* lpText );

//	驱动程序接口函数
const _FONTDRV FAR _SymbolDrv = {
                      _Symbol_Init,
                      _Symbol_Deinit,
	                  _Symbol_InstallFont,
	                  _Symbol_CreateFont,
	                  _Symbol_DeleteFont,
                      _Symbol_MaxHeight,
                      _Symbol_MaxWidth,
                      _Symbol_WordLength,
                      _Symbol_WordHeight,
                      _Symbol_WordWidth,
                      _Symbol_WordMask,
                      _Symbol_TextWidth,
                      _Symbol_TextHeight,
                      _Symbol_NextWord 
};

#define IS_TEXT_MARK( v ) ( (v) == TEXT_SOFT_BREAK || (v) == TEXT_KEY_BREAK || (v) == TEXT_EOF )

// define all function

// *****************************************************************
// 声明：static DWORD _Symbol_Init( void )
// 参数：
//	无
// 返回值：
//	假如成功，返回对象句柄；否则，返回NULL
// 功能描述：
//	初始化字体驱动程序
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static DWORD _Symbol_Init( void )
{
	return 1;
}

// *****************************************************************
// 声明：static BOOL _Symbol_Deinit( DWORD dwData )
// 参数：
//	IN dwData - 由_Symbol_Init返回的对象句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static BOOL _Symbol_Deinit( DWORD dwData )
{
	return TRUE;
}

// *****************************************************************
// 声明：BOOL _Symbol_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
// 参数：
//	IN dwData - 由_Symbol_Init返回的对象句柄
//	IN lpszPathName - 文件路径名（字体文件）
// 返回值：
//	假如字体文件被该驱动程序识别，返回TRUE; 否则，返回FALSE
// 功能描述：
//	安装一个字体
// 引用: 
//	驱动程序接口函数
// *****************************************************************
BOOL _Symbol_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
{
	return FALSE;
}

// *****************************************************************
// 声明：HANDLE _Symbol_CreateFont( DWORD dwData, const LOGFONT *lplf )
// 参数：
//	IN dwData - 由_Symbol_Init返回的对象句柄
//	IN lplf - LOGFONT 结构指针，包含逻辑字体属性
// 返回值：
//	假如成功，返回打开的逻辑字体对象句柄
// 功能描述：
//	创建逻辑字体对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************
HANDLE _Symbol_CreateFont( DWORD dwData, const LOGFONT *lplf )
{
	if( lplf )
	{
		if(	lplf->lfWeight == 16 &&
			lplf->lfHeight == 16 )
		{
			return (HANDLE)16;
		}
		if(	lplf->lfWeight == 24 &&
			lplf->lfHeight == 24 )
		{
			return (HANDLE)24;
		}
	}
	else
		return (HANDLE)16;//默认
	return NULL;
}

// *****************************************************************
// 声明：BOOL _Symbol_DeleteFont( HANDLE handle )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	删除逻辑字体对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************
BOOL _Symbol_DeleteFont( HANDLE handle )
{
	return TRUE;
}

// *****************************************************************
// 声明：static int _Symbol_MaxHeight( HANDLE handle )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	返回逻辑字体的高度
// 功能描述：
//	得到逻辑字体的高度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int _Symbol_MaxHeight( HANDLE handle )
{
    return (int)handle;
}

// *****************************************************************
// 声明：static int _Symbol_MaxWidth( HANDLE handle )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	返回逻辑字体的宽度
// 功能描述：
//	得到逻辑字体的宽度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int _Symbol_MaxWidth( HANDLE handle )
{
    return (int)handle;//16;
}

// *****************************************************************
// 声明：static int _Symbol_WordHeight( HANDLE handle, WORD aWord )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
//	IN aWord - 字符代码
// 返回值：
//	返回字符的高度
// 功能描述：
//	得到字符的高度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int _Symbol_WordHeight( HANDLE handle, WORD unused )
{
    return (int)handle;//16;
}

// *****************************************************************
// 声明：static int _Symbol_WordWidth( HANDLE handle, WORD aWord )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
//	IN aWord - 字符代码
// 返回值：
//	返回字符的宽度
// 功能描述：
//	得到字符的宽度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int _Symbol_WordWidth( HANDLE handle, WORD aWord )
{
    return (int)handle;//16;
}

// *****************************************************************
// 声明：static int _Symbol_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPBITMAPDATA lpMask )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	OUT lpMask - 用于接受字模的结构指针
// 返回值：
//	返回字符代码的字节数（宽字符为2），lpMask包含该字符的长宽和字模地址
// 功能描述：
//	得到字符代码的字节数（宽字符为2）和字符的长宽和字模地址
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int _Symbol_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPCHAR_METRICS lpMask )
{
    lpMask->bitmap.bmWidth = (int)handle;//16;
    lpMask->bitmap.bmHeight = (int)handle;//16;
	if( (int)handle == 16 )
	{
		lpMask->bitmap.bmWidthBytes = 2;
		lpMask->bitmap.bmBits = (LPBYTE)&__bSymbolsFontMask16x16[( (*lpText) - '!' ) << 5];
	}
	else if( (int)handle == 24 )
	{
		lpMask->bitmap.bmWidthBytes = 3;		
		lpMask->bitmap.bmBits = (LPBYTE)&__bSymbolsFontMask24x24[( (*lpText) - '!' ) * 72];
	}
	lpMask->xAdvance = (int)handle;
	lpMask->yAdvance = 0;
	lpMask->left = lpMask->top = 0;
    
    return 1;
}

// *****************************************************************
// 声明：static int _Symbol_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	IN len - 需要统计的文本长度
// 返回值：
//	文本的象素长度
// 功能描述：
//	得到文本的象素长度，如果文本长度（len）中间包含换行字符，则统计到换行字符位置为止。
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int _Symbol_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
{
   int  w = 0;

   if( lpText )
   {
	   while( !IS_TEXT_MARK( *lpText ) && *lpText && len )
       {
           w += (int)handle;//16;
           lpText++;
           if( len > 0 )
               len--;
       }
   }
   return w;
}

// *****************************************************************
// 声明：static int _Symbol_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	IN aLineWidth - 需要统计的象素最大宽度，如果为0，则没有宽度限制
// 返回值：
//	文本的象素高度
// 功能描述：
//	得到文本的象素高度，如果文本象素长度 大于等于aLineWidth则增加字体高度；
//	如果文本中间包含换行字符，则增加字体高度。
//	如果aLineWidth为0，则没有宽度限制，以换行字符确定下一行
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int _Symbol_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
{
    int h = 0, w = 0;

    if( lpText )
    {
        h = (int)handle;//16;
        do
        {
            if( (aLineWidth > 0 && w + (int)handle >= aLineWidth) ||
                IS_TEXT_MARK( *lpText ) )
            {
                h += (int)handle;//8;
                w = 0;
            }
            else
                w += (int)handle;//8;
            lpText++;
        }while( *lpText );
    }
    return h;
}

// *****************************************************************
// 声明：const BYTE FAR* _Symbol_NextWord( HANDLE handle, const BYTE FAR* lpText )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
// 返回值：
//	文本的下一个字符地址指针
// 功能描述：
//	得到文本的下一个字符地址指针
// 引用: 
//	驱动程序接口函数
// *****************************************************************
const BYTE FAR* _Symbol_NextWord( HANDLE handle, const BYTE FAR* lpText )
{
    return (lpText + 1);
}

// *****************************************************************
// 声明：int _Symbol_WordLength( HANDLE handle, const BYTE FAR* lpText )
// 参数：
//	IN handle - 由 _Symbol_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
// 返回值：
//	文本中以字符为单位的数量
// 功能描述：
//	得到文本中以字符为单位的数量
// 引用: 
//	驱动程序接口函数
// *****************************************************************
int _Symbol_WordLength( HANDLE handle, const BYTE FAR* lpText )
{
     int l = 0;
     while( *lpText++ )l++;
     return l;
}
