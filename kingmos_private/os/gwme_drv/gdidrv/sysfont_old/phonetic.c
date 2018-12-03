/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：英文语音字体字模驱动程序
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：

******************************************************/
#include <eframe.h>
#include <gwmeobj.h>
//字模
static const HUGE BYTE PhoneticMask[]=
{
	0X00,0X00,0X00,0X00,0X00,0X66,0X99,0X19,0X7E,0X98,0X99,0X66,0X00,0X00,0X00,0X00 ,
	0X00,0X00,0X00,0X00,0X38,0X44,0X02,0X02,0X02,0X02,0X44,0X38,0X00,0X00,0X00,0X00 ,
	0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X24,0X24,0X42,0X42,0X00,0X00,0X00,0X00,0X00 ,
	0X00,0X00,0X00,0X00,0X00,0X3C,0X42,0X02,0X7E,0X42,0X42,0X3C,0X00,0X00,0X00,0X00 ,
	0X00,0X00,0X00,0X00,0X00,0X3C,0X42,0X42,0X30,0X42,0X42,0X3C,0X00,0X00,0X00,0X00 ,
	0X00,0X00,0X00,0X00,0X5C,0X62,0X42,0X42,0X42,0X02,0X02,0X42,0X42,0X42,0X3C,0X00 ,
	0X00,0X00,0X00,0X18,0X24,0X42,0X42,0X7E,0X42,0X42,0X24,0X18,0X00,0X00,0X00,0X00 ,
	0X00,0X38,0X05,0X0E,0X32,0X46,0X1A,0X22,0X42,0X44,0X38,0X00,0X00,0X00,0X00,0X00 ,
	0X00,0X06,0X09,0X09,0X08,0X08,0X08,0X08,0X08,0X08,0X08,0X08,0X08,0X48,0X30,0X00 ,
	0X00,0X00,0X00,0X00,0X00,0XFC,0X08,0X30,0X08,0X04,0X04,0X04,0X84,0X84,0X78,0X00 ,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X08,0X08,0X08,0X08,0X00,0X00 ,	
};

static DWORD Phonetic_Init(void);
static BOOL Phonetic_Deinit( DWORD dwData );
static BOOL Phonetic_InstallFont( DWORD dwData, LPCTSTR lpszPathName );
static HANDLE Phonetic_CreateFont( DWORD dwData, const LOGFONT *lplf );
static BOOL Phonetic_DeleteFont( HANDLE );
static int Phonetic_MaxHeight( HANDLE );
static int Phonetic_MaxWidth( HANDLE );
static int Phonetic_WordLength( HANDLE, const BYTE FAR* lpText );
static int Phonetic_WordHeight( HANDLE, WORD aWord );
static int Phonetic_WordWidth( HANDLE, WORD aWord );
static int Phonetic_WordBitmap( HANDLE, const BYTE FAR* lpText, _LPCHAR_METRICS );
static int Phonetic_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int Phonetic_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static LPCBYTE Phonetic_NextWord( HANDLE handle, LPCBYTE lpText );

//	驱动程序接口函数
const _FONTDRV FAR _PhoneticDrv = {
                      Phonetic_Init,
                      Phonetic_Deinit,
	                  Phonetic_InstallFont,
	                  Phonetic_CreateFont,
	                  Phonetic_DeleteFont,
                      Phonetic_MaxHeight,
                      Phonetic_MaxWidth,
                      Phonetic_WordLength,
                      Phonetic_WordHeight,
                      Phonetic_WordWidth,
                      Phonetic_WordBitmap,
                      Phonetic_TextWidth,
                      Phonetic_TextHeight,
                      Phonetic_NextWord 
};


//extern const _FONTDRV FAR _SYSFontDrv;
extern const _FONTDRV FAR * _lpSYSFontDrv;
#define _SYSFontDrv (*_lpSYSFontDrv)

// define all function


// *****************************************************************
// 声明：static DWORD Phonetic_Init( void )
// 参数：
//	无
// 返回值：
//	假如成功，返回对象句柄；否则，返回NULL
// 功能描述：
//	初始化字体驱动程序
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static DWORD Phonetic_Init( void )
{
	return 1;
}

// *****************************************************************
// 声明：static BOOL Phonetic_Deinit( DWORD dwData )
// 参数：
//	IN dwData - 由Phonetic_Init返回的对象句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static BOOL Phonetic_Deinit( DWORD dwData )
{
	return TRUE;
}

// *****************************************************************
// 声明：BOOL Phonetic_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
// 参数：
//	IN dwData - 由Phonetic_Init返回的对象句柄
//	IN lpszPathName - 文件路径名（字体文件）
// 返回值：
//	假如字体文件被该驱动程序识别，返回TRUE; 否则，返回FALSE
// 功能描述：
//	安装一个字体
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static BOOL Phonetic_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
{
	return FALSE;
}

// *****************************************************************
// 声明：HANDLE Phonetic_CreateFont( DWORD dwData, const LOGFONT *lplf )
// 参数：
//	IN dwData - 由Phonetic_Init返回的对象句柄
//	IN lplf - LOGFONT 结构指针，包含逻辑字体属性
// 返回值：
//	假如成功，返回打开的逻辑字体对象句柄
// 功能描述：
//	创建逻辑字体对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static HANDLE Phonetic_CreateFont( DWORD dwData, const LOGFONT *lplf )
{
	return (HANDLE)1;
}

// *****************************************************************
// 声明：BOOL Phonetic_DeleteFont( HANDLE handle )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	删除逻辑字体对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static BOOL Phonetic_DeleteFont( HANDLE handle )
{
	return TRUE;
}

// *****************************************************************
// 声明：static int Phonetic_MaxHeight( HANDLE handle )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	返回逻辑字体的高度
// 功能描述：
//	得到逻辑字体的高度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int Phonetic_MaxHeight( HANDLE handle )
{
    return 8;
}

// *****************************************************************
// 声明：static int Phonetic_MaxWidth( HANDLE handle )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	返回逻辑字体的宽度
// 功能描述：
//	得到逻辑字体的宽度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int Phonetic_MaxWidth( HANDLE handle )
{
    return _SYSFontDrv.lpMaxWidth( handle );
}

// *****************************************************************
// 声明：static int Phonetic_WordHeight( HANDLE handle, WORD aWord )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
//	IN aWord - 字符代码
// 返回值：
//	返回字符的高度
// 功能描述：
//	得到字符的高度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int Phonetic_WordHeight( HANDLE handle, WORD aWord )
{
	return _SYSFontDrv.lpWordHeight( handle, aWord );
}

// *****************************************************************
// 声明：static int Phonetic_WordWidth( HANDLE handle, WORD aWord )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
//	IN aWord - 字符代码
// 返回值：
//	返回字符的宽度
// 功能描述：
//	得到字符的宽度
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int Phonetic_WordWidth( HANDLE handle, WORD aWord )
{
	return _SYSFontDrv.lpWordWidth( handle, aWord );
}

// *****************************************************************
// 声明：static int Phonetic_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPBITMAPDATA lpMask )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	OUT lpMask - 用于接受字模的结构指针
// 返回值：
//	返回字符代码的字节数（宽字符为2），lpMask包含该字符的长宽和字模地址
// 功能描述：
//	得到字符代码的字节数（宽字符为2）和字符的长宽和字模地址
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int Phonetic_WordBitmap( HANDLE handle, LPCBYTE lpText, _LPCHAR_METRICS lpMask )
{
  int iPosition;

    switch(*lpText)
	{
		case '@':
			iPosition=0;
			break;
		case '#':
			iPosition=1;
			break;
		case '$':
			iPosition=2;
			break;
		case '%':
			iPosition=3;
			break;
		case '^':
			iPosition=4;
			break;
		case '*':
			iPosition=5;
			break;
		case '?':
			iPosition=6;
			break;
		case '|':
			iPosition=7;
			break;
		case '-':
			iPosition=8;
			break;
		case '/':
			iPosition=9;
			break;
		case '&':
			iPosition=10;
			break;
		default:
			return _SYSFontDrv.lpWordBitmap( handle, lpText, lpMask );

	}
    iPosition*=16;

    lpMask->bitmap.bmWidth = 8;
    lpMask->bitmap.bmHeight = 16;
    lpMask->bitmap.bmWidthBytes = 1;
    lpMask->bitmap.bmBits = (BYTE *)&PhoneticMask[iPosition];

	lpMask->xAdvance = 8;
	lpMask->yAdvance = 0;
	lpMask->left = lpMask->top = 0;
    return 1;
}

// *****************************************************************
// 声明：static int Phonetic_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	IN len - 需要统计的文本长度
// 返回值：
//	文本的象素长度
// 功能描述：
//	得到文本的象素长度，如果文本长度（len）中间包含换行字符，则统计到换行字符位置为止。
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int Phonetic_TextWidth( HANDLE handle, LPCBYTE lpText, int len )
{
	return _SYSFontDrv.lpTextWidth( handle, lpText, len );
}

// *****************************************************************
// 声明：static int Phonetic_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
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
static int Phonetic_TextHeight( HANDLE handle, LPCBYTE lpText, int aLineWidth )
{
	return _SYSFontDrv.lpTextHeight( handle, lpText, aLineWidth );
}

// *****************************************************************
// 声明：const BYTE FAR* Phonetic_NextWord( HANDLE handle, const BYTE FAR* lpText )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
// 返回值：
//	文本的下一个字符地址指针
// 功能描述：
//	得到文本的下一个字符地址指针
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static LPCBYTE Phonetic_NextWord( HANDLE handle, LPCBYTE lpText )
{
	return _SYSFontDrv.lpNextWord( handle, lpText );
}

// *****************************************************************
// 声明：int Phonetic_WordLength( HANDLE handle, const BYTE FAR* lpText )
// 参数：
//	IN handle - 由 Phonetic_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
// 返回值：
//	文本中以字符为单位的数量
// 功能描述：
//	得到文本中以字符为单位的数量
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static int Phonetic_WordLength( HANDLE handle, LPCBYTE lpText )
{
	return _SYSFontDrv.lpWordLength( handle, lpText );
}
