/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：系统字体字模驱动程序 24x24点正阵汉字，12x24ascii
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：

******************************************************/
#include <eframe.h>
#include <gwmeobj.h>
#include <eobjtype.h>
#include "hzk24s.h"
#include "12x24.h"
#include "hzk24t.h"

static DWORD SYSFont24_Init(void);
static BOOL SYSFont24_Deinit( DWORD dwData );
static BOOL SYSFont24_InstallFont( DWORD dwData, LPCTSTR lpszPathName );
static HANDLE SYSFont24_CreateFont( DWORD dwData, const LOGFONT *lplf );
static BOOL SYSFont24_DeleteFont( HANDLE );
static int SYSFont24_MaxHeight( HANDLE );
static int SYSFont24_MaxWidth( HANDLE );
static int SYSFont24_WordLength( HANDLE, const BYTE FAR* lpText );
static int SYSFont24_WordHeight( HANDLE, WORD aWord );
static int SYSFont24_WordWidth( HANDLE, WORD aWord );
static int SYSFont24_WordMask( HANDLE, const BYTE FAR* lpText, UINT len, _LPCHAR_METRICS );
static int SYSFont24_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int SYSFont24_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static const BYTE FAR* SYSFont24_NextWord( HANDLE, const BYTE FAR* lpText );

static LPCBYTE _GetChineseMask( WORD );
//	驱动程序接口函数
const _FONTDRV FAR _SYSFont24Drv = {
                      SYSFont24_Init,
                      SYSFont24_Deinit,
	                  SYSFont24_InstallFont,
	                  SYSFont24_CreateFont,
	                  SYSFont24_DeleteFont,
                      SYSFont24_MaxHeight,
                      SYSFont24_MaxWidth,
                      SYSFont24_WordLength,
                      SYSFont24_WordHeight,
                      SYSFont24_WordWidth,
                      SYSFont24_WordMask,
                      SYSFont24_TextWidth,
                      SYSFont24_TextHeight,
                      SYSFont24_NextWord 
};


enum{
    USE_NOFONT = 0,
    USE_ROMFONT = 1,
    USE_FILEFONT = 2
};

#define MK_FARP( seg, ofs ) ( (void * )MAKELONG( ofs, seg ) )

#define IS_CHINESE( lpText ) ( (*(lpText) & 0x80) && ( *((lpText) + 1) & 0x80 ) )
#define IS_TEXT_MARK( v ) ( (v) == TEXT_SOFT_BREAK || (v) == TEXT_KEY_BREAK || (v) == TEXT_EOF )
#define _GET_CHINESE( lpText ) ( (WORD)( (((WORD)*(lpText)) << 8) | *((lpText) + 1) ) )
#define _GET_ENGLISH( lpText ) ( *(lpText) )
#define FONT_HEIGHT 24
#define CHINESE_WIDTH 24
#define ENGLISH_WIDTH 12


// *****************************************************************
// 声明：static DWORD SYSFont24_Init( void )
// 参数：
//	无
// 返回值：
//	假如成功，返回对象句柄；否则，返回NULL
// 功能描述：
//	初始化字体驱动程序
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static DWORD SYSFont24_Init( void )
{
	return 1;
}

// *****************************************************************
// 声明：static BOOL SYSFont24_Deinit( DWORD dwData )
// 参数：
//	IN dwData - 由SYSFont24_Init返回的对象句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static BOOL SYSFont24_Deinit( DWORD dwData )
{
	return 1;
}

// *****************************************************************
// 声明：BOOL SYSFont24_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
// 参数：
//	IN dwData - 由ROM8x8_Init返回的对象句柄
//	IN lpszPathName - 文件路径名（字体文件）
// 返回值：
//	假如字体文件被该驱动程序识别，返回TRUE; 否则，返回FALSE
// 功能描述：
//	安装一个字体
// 引用: 
//	驱动程序接口函数
// *****************************************************************
BOOL SYSFont24_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
{
	return FALSE;
}

// *****************************************************************
// 声明：HANDLE SYSFont24_CreateFont( DWORD dwData, const LOGFONT *lplf )
// 参数：
//	IN dwData - 由SYSFont24_Init返回的对象句柄
//	IN lplf - LOGFONT 结构指针，包含逻辑字体属性
// 返回值：
//	假如成功，返回打开的逻辑字体对象句柄
// 功能描述：
//	创建逻辑字体对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************
HANDLE SYSFont24_CreateFont( DWORD dwData, const LOGFONT *lplf )
{
	return (HANDLE)1;
}

// *****************************************************************
// 声明：BOOL SYSFont24_DeleteFont( HANDLE handle )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	删除逻辑字体对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************

BOOL SYSFont24_DeleteFont( HANDLE handle )
{
	return TRUE;
}

// *****************************************************************
// 声明：static int SYSFont24_MaxHeight( HANDLE handle )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	返回逻辑字体的高度
// 功能描述：
//	得到逻辑字体的高度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int SYSFont24_MaxHeight( HANDLE handle )
{
    return FONT_HEIGHT;
}

// *****************************************************************
// 声明：static int SYSFont24_MaxWidth( HANDLE handle )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	返回逻辑字体的宽度
// 功能描述：
//	得到逻辑字体的宽度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int SYSFont24_MaxWidth( HANDLE handle )
{
    return CHINESE_WIDTH;
}

// *****************************************************************
// 声明：static int SYSFont24_WordHeight( HANDLE handle, WORD aWord )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
//	IN aWord - 字符代码
// 返回值：
//	返回字符的高度
// 功能描述：
//	得到字符的高度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int SYSFont24_WordHeight( HANDLE handle, WORD aWord )
{
    return FONT_HEIGHT;
}

// *****************************************************************
// 声明：static int SYSFont24_WordWidth( HANDLE handle, WORD aWord )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
//	IN aWord - 字符代码
// 返回值：
//	返回字符的宽度
// 功能描述：
//	得到字符的宽度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int SYSFont24_WordWidth( HANDLE handle, WORD aWord )
{
    return aWord < 0xff ? ENGLISH_WIDTH : CHINESE_WIDTH;
}

// *****************************************************************
// 声明：static int SYSFont24_WordMask( HANDLE handle, const BYTE FAR* lpText, UINT len, _LPCHAR_METRICS lpMask )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	OUT lpMask - 用于接受字模的结构指针
// 返回值：
//	返回字符代码的字节数（宽字符为2），lpMask包含该字符的长宽和字模地址
// 功能描述：
//	得到字符代码的字节数（宽字符为2）和字符的长宽和字模地址
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int SYSFont24_WordMask( HANDLE handle, LPCBYTE lpText, UINT len, _LPCHAR_METRICS lpMask )
{
#ifdef EML_DOS
    static const WORD FAR chineseMask[16] =
    { 0xfffe,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0xfffe,
      0x0000 };
    static const BYTE FAR englishMask[16] =
    { 0xfe,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0xfe,
      0x00 };

   if( IS_CHINESE( lpText ) )
   {
       lpMask->bmWidth = CHINESE_WIDTH;
       lpMask->bmHeight = FONT_HEIGHT;
       lpMask->bmWidthBytes = 2;
       lpMask->bmBits = (BYTE FAR*)chineseMask;

	   lpMask->xAdvance = CHINESE_WIDTH;
	   lpMask->yAdvance = 0;
	   lpMask->left = lpMask->top = 0;

       return 2;
   }
   else
   {
       lpMask->bmWidth = ENGLISH_WIDTH;
       lpMask->bmHeight = FONT_HEIGHT;
       lpMask->bmWidthBytes = 1;
       lpMask->bmBits = englishMask;

	   lpMask->xAdvance = ENGLISH_WIDTH;
	   lpMask->yAdvance = 0;
	   lpMask->left = lpMask->top = 0;

       return 1;
   }
#else
   //字模
   //extern const unsigned long eng12x24Mask[];
   if( IS_CHINESE( lpText ) )
   {	//汉字
       lpMask->bitmap.bmWidth = CHINESE_WIDTH;
       lpMask->bitmap.bmHeight = FONT_HEIGHT;
       lpMask->bitmap.bmWidthBytes = 3;

       lpMask->bitmap.bmBits = (LPBYTE)_GetChineseMask( _GET_CHINESE( lpText ) );

	   lpMask->xAdvance = CHINESE_WIDTH;
	   lpMask->yAdvance = 0;
	   lpMask->left = lpMask->top = 0;

       return 2;
   }
   else
   {	//ascii
       lpMask->bitmap.bmWidth = ENGLISH_WIDTH;
       lpMask->bitmap.bmHeight = FONT_HEIGHT;
       lpMask->bitmap.bmWidthBytes = 2;
       lpMask->bitmap.bmBits = (LPBYTE)( (LPBYTE)eng12x24Mask+_GET_ENGLISH( lpText )*48);// 24(H) x 12(2bytes) _ptrInRomEFont+ _GET_ENGLISH( lpText )*16;

	   lpMask->xAdvance = ENGLISH_WIDTH;
	   lpMask->yAdvance = 0;
	   lpMask->left = lpMask->top = 0;

       return 1;
   }
#endif
}

// *****************************************************************
// 声明：static int SYSFont24_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	IN len - 需要统计的文本长度
// 返回值：
//	文本的象素长度
// 功能描述：
//	得到文本的象素长度，如果文本长度（len）中间包含换行字符，则统计到换行字符位置为止。
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static int SYSFont24_TextWidth( HANDLE handle, LPCBYTE lpText, int len )
{
   int  w = 0;

   if( lpText )
   {
       while( !IS_TEXT_MARK( *lpText ) && *lpText && len )
       {	//换行 或 已到限制宽度
           if( IS_CHINESE( lpText ) )
           {	//汉字
               w += SYSFont24_WordWidth( handle, _GET_CHINESE( lpText ) );
               lpText += 2;
               if( len > 0 )
                   len -= 2;
           }
           else
           {	//ascii 
               w += SYSFont24_WordWidth( handle, _GET_ENGLISH( lpText ) );
               lpText += 1;
               if( len > 0 )
                   len--;
           }
       }
   }
   return w;

}

// *****************************************************************
// 声明：static int SYSFont24_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
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
static int SYSFont24_TextHeight( HANDLE handle, LPCBYTE lpText, int aLineWidth )
{
    int h = 0, w = 0;

    if( lpText )
    {
        h = FONT_HEIGHT;	//默认高度
        do  {
            if( IS_CHINESE( lpText ) )
            {	//汉字
                if( aLineWidth > 0 && w + CHINESE_WIDTH >= aLineWidth )
                {	//超过宽度
                    h += FONT_HEIGHT;
                    w = 0;
                }
                else
                    w += CHINESE_WIDTH;
				//汉字字符字节数为2 ，下一个字符
                lpText += 2;	
            }
            else
            {  //ascii字符 english font
                if( (aLineWidth > 0 && w + ENGLISH_WIDTH >= aLineWidth) ||
                    IS_TEXT_MARK( *lpText ) )
                {	//超过宽度 或 有换行字符
                    h += FONT_HEIGHT;
                    w = 0;
                }
                else
                    w += ENGLISH_WIDTH;
                lpText++;
            }
        }while( *lpText );
    }
    return h;
}

// *****************************************************************
// 声明：const BYTE FAR* SYSFont24_NextWord( HANDLE handle, const BYTE FAR* lpText )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
// 返回值：
//	文本的下一个字符地址指针
// 功能描述：
//	得到文本的下一个字符地址指针
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static LPCBYTE SYSFont24_NextWord( HANDLE handle, LPCBYTE lpText )
{
    return IS_CHINESE( lpText ) ? (lpText + 2) : (lpText + 1);
}

// *****************************************************************
// 声明：int SYSFont24_WordLength( HANDLE handle, const BYTE FAR* lpText )
// 参数：
//	IN handle - 由 SYSFont24_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
// 返回值：
//	文本中以字符为单位的数量
// 功能描述：
//	得到文本中以字符为单位的数量
// 引用: 
//	驱动程序接口函数
// *****************************************************************
int SYSFont24_WordLength( HANDLE handle, LPCBYTE lpText )
{
     int l = 0;
     if( lpText )
     {
         while( *lpText )
         {
             if( IS_CHINESE( lpText ) )
			 {
                 lpText += 2;
				 //l+= 2;
			 }
             else
                 lpText++;//l++;
             //lpText++;
			 l++;
         }
     }
     return l;
}

// *****************************************************************
// 声明：static LPCBYTE _GetChineseMask( WORD aWord )
// 参数：
//	IN aWord - 汉字代码
// 返回值：
//	该字符的字模
// 功能描述：
//	得到汉字字模
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static LPCBYTE _GetChineseMask( WORD aWord )
{
    const static BYTE undefMask[72] = {
        0x00,0x00,0x00,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x40,0x00,0x02,
        0x00,0x00,0x00
    };

#ifdef EML_DOS
    long offset;
    BYTE hi = (BYTE)(aWord >> 8);
    BYTE lo = (BYTE)aWord;

    offset = ( ( (long)hi - 0xa1L )*94L + (long)lo - 0xa1L ) << 5;  // * 32
/*
    if( _ptrInXMSFont )
    {
        XMSSeek( _ptrInXMSFont, Offset, XMS_BEG );
        XMSRead( _ptrInXMSFont, Mask, 16, 1 );
    }
    else if( _ptrInFileFont )
    {
        fseek( _ptrInFileFont, Offset, SEEK_SET );
        fread( Mask, 32, 1, _ptrInFileFont );
    }
*/
    return (BYTE FAR*)undefMask;

#else
	//包含汉字字模的数组
    //extern const unsigned char hzk24sMask[];

    long offset;
    BYTE hi = (BYTE)(aWord >> 8);
    BYTE lo = (BYTE)aWord;
	//得到相对偏移
    //offset = ( ( (long)hi - 0xa1L )*94L + (long)lo - 0xa1L ) << 5;  // * 32
	if( hi < 0xb0 )
	{
	    offset = ( ( (long)hi - 0xa1L )*94L + (long)lo - 0xa1L ) * 72;  
		if( offset <= sizeof(hzk24tMask) - 72 )
			return (LPCBYTE)( (LPBYTE)hzk24tMask+offset);
		else
			return undefMask;
	}
	else
	{
		offset = ( ( (long)hi - 0xb0L )*94L + (long)lo - 0xa1L ) * 72;
		if( offset <= sizeof(hzk24sMask) - 72 )
			return (LPCBYTE)( (LPBYTE)hzk24sMask+offset);
		else
			return undefMask;
	}
  
    
#endif
}
