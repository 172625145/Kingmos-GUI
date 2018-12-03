/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����Ӣ������������ģ��������
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��

******************************************************/
#include <eframe.h>
#include <gwmeobj.h>
//��ģ
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
static int Phonetic_WordBitmap( HANDLE, const BYTE FAR* lpText, UINT len, _LPCHAR_METRICS );
static int Phonetic_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int Phonetic_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static LPCBYTE Phonetic_NextWord( HANDLE handle, LPCBYTE lpText );

//	��������ӿں���
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
// ������static DWORD Phonetic_Init( void )
// ������
//	��
// ����ֵ��
//	����ɹ������ض����������򣬷���NULL
// ����������
//	��ʼ��������������
// ����: 
//	��������ӿں���
// *****************************************************************
static DWORD Phonetic_Init( void )
{
	return 1;
}

// *****************************************************************
// ������static BOOL Phonetic_Deinit( DWORD dwData )
// ������
//	IN dwData - ��Phonetic_Init���صĶ�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͷŶ���
// ����: 
//	��������ӿں���
// *****************************************************************

static BOOL Phonetic_Deinit( DWORD dwData )
{
	return TRUE;
}

// *****************************************************************
// ������BOOL Phonetic_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
// ������
//	IN dwData - ��Phonetic_Init���صĶ�����
//	IN lpszPathName - �ļ�·�����������ļ���
// ����ֵ��
//	���������ļ�������������ʶ�𣬷���TRUE; ���򣬷���FALSE
// ����������
//	��װһ������
// ����: 
//	��������ӿں���
// *****************************************************************
static BOOL Phonetic_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
{
	return FALSE;
}

// *****************************************************************
// ������HANDLE Phonetic_CreateFont( DWORD dwData, const LOGFONT *lplf )
// ������
//	IN dwData - ��Phonetic_Init���صĶ�����
//	IN lplf - LOGFONT �ṹָ�룬�����߼���������
// ����ֵ��
//	����ɹ������ش򿪵��߼����������
// ����������
//	�����߼��������
// ����: 
//	��������ӿں���
// *****************************************************************
static HANDLE Phonetic_CreateFont( DWORD dwData, const LOGFONT *lplf )
{
	return (HANDLE)1;
}

// *****************************************************************
// ������BOOL Phonetic_DeleteFont( HANDLE handle )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	ɾ���߼��������
// ����: 
//	��������ӿں���
// *****************************************************************
static BOOL Phonetic_DeleteFont( HANDLE handle )
{
	return TRUE;
}

// *****************************************************************
// ������static int Phonetic_MaxHeight( HANDLE handle )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
// ����ֵ��
//	�����߼�����ĸ߶�
// ����������
//	�õ��߼�����ĸ߶�
// ����: 
//	��������ӿں���
// *****************************************************************
static int Phonetic_MaxHeight( HANDLE handle )
{
    return 8;
}

// *****************************************************************
// ������static int Phonetic_MaxWidth( HANDLE handle )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
// ����ֵ��
//	�����߼�����Ŀ��
// ����������
//	�õ��߼�����Ŀ��
// ����: 
//	��������ӿں���
// *****************************************************************
static int Phonetic_MaxWidth( HANDLE handle )
{
    return _SYSFontDrv.lpMaxWidth( handle );
}

// *****************************************************************
// ������static int Phonetic_WordHeight( HANDLE handle, WORD aWord )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
//	IN aWord - �ַ�����
// ����ֵ��
//	�����ַ��ĸ߶�
// ����������
//	�õ��ַ��ĸ߶�
// ����: 
//	��������ӿں���
// *****************************************************************
static int Phonetic_WordHeight( HANDLE handle, WORD aWord )
{
	return _SYSFontDrv.lpWordHeight( handle, aWord );
}

// *****************************************************************
// ������static int Phonetic_WordWidth( HANDLE handle, WORD aWord )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
//	IN aWord - �ַ�����
// ����ֵ��
//	�����ַ��Ŀ��
// ����������
//	�õ��ַ��Ŀ��
// ����: 
//	��������ӿں���
// *****************************************************************
static int Phonetic_WordWidth( HANDLE handle, WORD aWord )
{
	return _SYSFontDrv.lpWordWidth( handle, aWord );
}

// *****************************************************************
// ������static int Phonetic_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPBITMAPDATA lpMask )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
//	OUT lpMask - ���ڽ�����ģ�Ľṹָ��
// ����ֵ��
//	�����ַ�������ֽ��������ַ�Ϊ2����lpMask�������ַ��ĳ������ģ��ַ
// ����������
//	�õ��ַ�������ֽ��������ַ�Ϊ2�����ַ��ĳ������ģ��ַ
// ����: 
//	��������ӿں���
// *****************************************************************
static int Phonetic_WordBitmap( HANDLE handle, LPCBYTE lpText, UINT len, _LPCHAR_METRICS lpMask )
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
			return _SYSFontDrv.lpWordBitmap( handle, lpText, len, lpMask );

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
// ������static int Phonetic_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
//	IN len - ��Ҫͳ�Ƶ��ı�����
// ����ֵ��
//	�ı������س���
// ����������
//	�õ��ı������س��ȣ�����ı����ȣ�len���м���������ַ�����ͳ�Ƶ������ַ�λ��Ϊֹ��
// ����: 
//	��������ӿں���
// *****************************************************************
static int Phonetic_TextWidth( HANDLE handle, LPCBYTE lpText, int len )
{
	return _SYSFontDrv.lpTextWidth( handle, lpText, len );
}

// *****************************************************************
// ������static int Phonetic_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
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
static int Phonetic_TextHeight( HANDLE handle, LPCBYTE lpText, int aLineWidth )
{
	return _SYSFontDrv.lpTextHeight( handle, lpText, aLineWidth );
}

// *****************************************************************
// ������const BYTE FAR* Phonetic_NextWord( HANDLE handle, const BYTE FAR* lpText )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
// ����ֵ��
//	�ı�����һ���ַ���ַָ��
// ����������
//	�õ��ı�����һ���ַ���ַָ��
// ����: 
//	��������ӿں���
// *****************************************************************
static LPCBYTE Phonetic_NextWord( HANDLE handle, LPCBYTE lpText )
{
	return _SYSFontDrv.lpNextWord( handle, lpText );
}

// *****************************************************************
// ������int Phonetic_WordLength( HANDLE handle, const BYTE FAR* lpText )
// ������
//	IN handle - �� Phonetic_CreateFont ���ص��߼����������
//	IN lpText - �ı�ָ��
// ����ֵ��
//	�ı������ַ�Ϊ��λ������
// ����������
//	�õ��ı������ַ�Ϊ��λ������
// ����: 
//	��������ӿں���
// *****************************************************************
static int Phonetic_WordLength( HANDLE handle, LPCBYTE lpText )
{
	return _SYSFontDrv.lpWordLength( handle, lpText );
}
