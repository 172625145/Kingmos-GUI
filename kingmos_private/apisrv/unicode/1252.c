/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����unicode-1252 ת��
�汾�ţ�1.0.0
����ʱ�ڣ�2005-10-05
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <unicode.h>

static DWORD Create( DWORD dwReserver );
static BOOL Delete( DWORD dwHandle );
//utf8->unicode
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar );
// unicode->utf8
static int Get1252( DWORD dwHandle,
			   LPCWSTR lpWideCharStr, 
			   int cchWideChar, 
			   LPSTR lpMultiByteStr, 
			   int cbMultiByte, 
			   LPCSTR lpDefaultChar, 
			   BOOL * lpUsedDefaultChar );
static int CountBufferSize( DWORD dwHandle,
				     LPCVOID lpStr,
				     int cbCountLen,
			         BOOL IsWideChar );


const CODE_PAGE_DRV CP_1252Drv = 
{
	Create,
	Delete,
	GetUnicode,
	Get1252,
	CountBufferSize
};

static DWORD Create( DWORD dwReserver )
{
	return 1;
}

static BOOL Delete( DWORD dwHandle )
{
	return TRUE;
}

//1252 -> unicode
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar )
{
	int cchSaveWideChar = cchWideChar;

	while( cbMultiByte && cchWideChar )
	{
		*lpWideCharStr++ = *lpMultiByteStr++;
		cchWideChar--;
		cbMultiByte--;
	}

	return cchSaveWideChar - cchWideChar;
}

// unicode -> 1252
static int Get1252( DWORD dwHandle,
			   LPCWSTR lpWideCharStr, 
			   int cchWideChar, 
			   LPSTR lpMultiByteStr, 
			   int cbMultiByte, 
			   LPCSTR lpDefaultChar, 
			   BOOL * lpUsedDefaultChar )
{
	int cbSaveMultiByte = cbMultiByte;


	while( cbMultiByte && cchWideChar )
	{		
		WCHAR wc = *lpWideCharStr++;
		cchWideChar--;

		if( wc <= 0xff )
		{	////��һ�ַ�
			*lpMultiByteStr++ = (BYTE)wc;
			cbMultiByte--;
		}
		else
		{  // �޷�ʶ��
			*lpMultiByteStr++ = (BYTE)'?';
			cbMultiByte--;
		}
	}
	return cbSaveMultiByte - cbMultiByte;
}

//ͳ����Ҫ���ַ���
static int CountBufferSize( DWORD dwHandle,
				     LPCVOID lpStr,
				     int cbCountLen,
			         BOOL IsWideChar )
{
	return cbCountLen;
}

