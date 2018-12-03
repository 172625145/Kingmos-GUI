/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：文件名（含通配符）比较
版本号：1.0.0
开发时期：2003-04-29
作者：李林
修改记录：
******************************************************/

#include <edef.h>
#include <eassert.h>
#include <estring.h>

// *****************************************************************
//声明：BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );
//参数：
//	IN lpcszMask-含通配符的字符串
//  IN iMaskLen-含通配符的字符串的长度; 当为0，必须是0结束的字符串
//  IN lpcszSrc-需要比较的文件名
//  IN iSrcLen-文件名长度; 当为0，必须是0结束的字符串
//返回值：
//	成功，返回TRUE;失败，返回FALSE
//功能描述：文件名（含通配符）比较
//引用: 
// *****************************************************************


BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen )
{
	TCHAR cMask;
	if( iMaskLen == 0 )
		iMaskLen = strlen( lpcszMask );
	if( iSrcLen == 0 )
		iSrcLen = strlen( lpcszSrc );

    while( iMaskLen && iSrcLen )
	{
        if( (cMask = *lpcszMask) == '*' )
		{
            lpcszMask++;
			iMaskLen--;
	
			if( iMaskLen && *lpcszMask == '.' )
			{ //filename is match , skip to ext name
	            lpcszMask++;
			    iMaskLen--;
				while( iSrcLen )
				{
					if( *lpcszSrc == '.' )
					{
						lpcszSrc++;
						iSrcLen--;
						break;
					}
					lpcszSrc++;
					iSrcLen--;
				}
				if( iSrcLen == 0 )
					goto _return;

				//continue;
			}
			
            if( iMaskLen ) 
			{
                while( iSrcLen )
				{
					if( FileNameCompare( lpcszMask, iMaskLen, lpcszSrc++, iSrcLen-- ) )
                        return TRUE;
                }
                return FALSE;
            }
            return TRUE;
        }
		else if( cMask == '?' || cMask == *lpcszSrc )
		{
			;  // continue next char
		}
		else if( cMask >= 'A' && cMask <= 'Z' )
		{
			if( ((int)cMask - (int)*lpcszSrc) != ('A' - 'a') )
				return FALSE;
		}
		else if( cMask >= 'a' && cMask <= 'z' )
		{
			if( ((int)cMask - (int)*lpcszSrc) != ('a' - 'A') )
				return FALSE;
		}
		else
			return FALSE;
		
        iMaskLen--;
        lpcszMask++;
        iSrcLen--;
        lpcszSrc++;
    }

_return:
    if( !iMaskLen && !iSrcLen )
        return TRUE;

    if( !iMaskLen )
        return FALSE;
    else
	{
		while( iMaskLen-- ) 
		{
            if( *lpcszMask++ != '*' )
                return FALSE;
		}
    }
    return TRUE;
}
