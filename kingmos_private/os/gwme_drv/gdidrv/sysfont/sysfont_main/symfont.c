/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵��������������ģ��������
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��

******************************************************/
#include <eframe.h>
#include <gwmeobj.h>

//��ģ
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
static int _Symbol_WordMask( HANDLE, const BYTE FAR* lpText, UINT len, _LPCHAR_METRICS );
static int _Symbol_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int _Symbol_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static const BYTE FAR* _Symbol_NextWord( HANDLE, const BYTE FAR* lpText );

//	��������ӿں���
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
// ������static DWORD _Symbol_Init( void )
// ������
//	��
// ����ֵ��
//	����ɹ������ض����������򣬷���NULL
// ����������
//	��ʼ��������������
// ����: 
//	��������ӿں���
// *****************************************************************
static DWORD _Symbol_Init( void )
{
	return 1;
}

// *****************************************************************
// ������static BOOL _Symbol_Deinit( DWORD dwData )
// ������
//	IN dwData - ��_Symbol_Init���صĶ�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͷŶ���
// ����: 
//	��������ӿں���
// *****************************************************************
static BOOL _Symbol_Deinit( DWORD dwData )
{
	return TRUE;
}

// *****************************************************************
// ������BOOL _Symbol_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
// ������
//	IN dwData - ��_Symbol_Init���صĶ�����
//	IN lpszPathName - �ļ�·�����������ļ���
// ����ֵ��
//	���������ļ�������������ʶ�𣬷���TRUE; ���򣬷���FALSE
// ����������
//	��װһ������
// ����: 
//	��������ӿں���
// *****************************************************************
BOOL _Symbol_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
{
	return FALSE;
}

// *****************************************************************
// ������HANDLE _Symbol_CreateFont( DWORD dwData, const LOGFONT *lplf )
// ������
//	IN dwData - ��_Symbol_Init���صĶ�����
//	IN lplf - LOGFONT �ṹָ�룬�����߼���������
// ����ֵ��
//	����ɹ������ش򿪵��߼����������
// ����������
//	�����߼��������
// ����: 
//	��������ӿں���
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
		return (HANDLE)16;//Ĭ��
	return NULL;
}

// *****************************************************************
// ������BOOL _Symbol_DeleteFont( HANDLE handle )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	ɾ���߼��������
// ����: 
//	��������ӿں���
// *****************************************************************
BOOL _Symbol_DeleteFont( HANDLE handle )
{
	return TRUE;
}

// *****************************************************************
// ������static int _Symbol_MaxHeight( HANDLE handle )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
// ����ֵ��
//	�����߼�����ĸ߶�
// ����������
//	�õ��߼�����ĸ߶�
// ����: 
//	��������ӿں���
// *****************************************************************
static int _Symbol_MaxHeight( HANDLE handle )
{
    return (int)handle;
}

// *****************************************************************
// ������static int _Symbol_MaxWidth( HANDLE handle )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
// ����ֵ��
//	�����߼�����Ŀ��
// ����������
//	�õ��߼�����Ŀ��
// ����: 
//	��������ӿں���
// *****************************************************************
static int _Symbol_MaxWidth( HANDLE handle )
{
    return (int)handle;//16;
}

// *****************************************************************
// ������static int _Symbol_WordHeight( HANDLE handle, WORD aWord )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
//	IN aWord - �ַ�����
// ����ֵ��
//	�����ַ��ĸ߶�
// ����������
//	�õ��ַ��ĸ߶�
// ����: 
//	��������ӿں���
// *****************************************************************
static int _Symbol_WordHeight( HANDLE handle, WORD unused )
{
    return (int)handle;//16;
}

// *****************************************************************
// ������static int _Symbol_WordWidth( HANDLE handle, WORD aWord )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
//	IN aWord - �ַ�����
// ����ֵ��
//	�����ַ��Ŀ��
// ����������
//	�õ��ַ��Ŀ��
// ����: 
//	��������ӿں���
// *****************************************************************
static int _Symbol_WordWidth( HANDLE handle, WORD aWord )
{
    return (int)handle;//16;
}

// *****************************************************************
// ������static int _Symbol_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPBITMAPDATA lpMask )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
//	OUT lpMask - ���ڽ�����ģ�Ľṹָ��
// ����ֵ��
//	�����ַ�������ֽ��������ַ�Ϊ2����lpMask�������ַ��ĳ������ģ��ַ
// ����������
//	�õ��ַ�������ֽ��������ַ�Ϊ2�����ַ��ĳ������ģ��ַ
// ����: 
//	��������ӿں���
// *****************************************************************
static int _Symbol_WordMask( HANDLE handle, const BYTE FAR* lpText, UINT len, _LPCHAR_METRICS lpMask )
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
// ������static int _Symbol_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
//	IN len - ��Ҫͳ�Ƶ��ı�����
// ����ֵ��
//	�ı������س���
// ����������
//	�õ��ı������س��ȣ�����ı����ȣ�len���м���������ַ�����ͳ�Ƶ������ַ�λ��Ϊֹ��
// ����: 
//	��������ӿں���
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
// ������static int _Symbol_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
//	IN aLineWidth - ��Ҫͳ�Ƶ���������ȣ����Ϊ0����û�п������
// ����ֵ��
//	�ı������ظ߶�
// ����������
//	�õ��ı������ظ߶ȣ�����ı����س��� ���ڵ���aLineWidth����������߶ȣ�
//	����ı��м���������ַ�������������߶ȡ�
//	���aLineWidthΪ0����û�п�����ƣ��Ի����ַ�ȷ����һ��
// ����: 
//	��������ӿں���
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
// ������const BYTE FAR* _Symbol_NextWord( HANDLE handle, const BYTE FAR* lpText )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
// ����ֵ��
//	�ı�����һ���ַ���ַָ��
// ����������
//	�õ��ı�����һ���ַ���ַָ��
// ����: 
//	��������ӿں���
// *****************************************************************
const BYTE FAR* _Symbol_NextWord( HANDLE handle, const BYTE FAR* lpText )
{
    return (lpText + 1);
}

// *****************************************************************
// ������int _Symbol_WordLength( HANDLE handle, const BYTE FAR* lpText )
// ������
//	IN handle - �� _Symbol_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
// ����ֵ��
//	�ı������ַ�Ϊ��λ������
// ����������
//	�õ��ı������ַ�Ϊ��λ������
// ����: 
//	��������ӿں���
// *****************************************************************
int _Symbol_WordLength( HANDLE handle, const BYTE FAR* lpText )
{
     int l = 0;
     while( *lpText++ )l++;
     return l;
}
