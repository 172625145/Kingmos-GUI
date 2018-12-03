/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ļ�������ͨ������Ƚ�
�汾�ţ�1.0.0
����ʱ�ڣ�2003-04-29
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <edef.h>
#include <eassert.h>
#include <estring.h>

// *****************************************************************
//������BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );
//������
//	IN lpcszMask-��ͨ������ַ���
//  IN iMaskLen-��ͨ������ַ����ĳ���; ��Ϊ0��������0�������ַ���
//  IN lpcszSrc-��Ҫ�Ƚϵ��ļ���
//  IN iSrcLen-�ļ�������; ��Ϊ0��������0�������ַ���
//����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
//�����������ļ�������ͨ������Ƚ�
//����: 
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
