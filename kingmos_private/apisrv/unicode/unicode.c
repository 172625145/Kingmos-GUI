/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����unicode ת��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-05-16
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <enls.h>
#include "unicode.h"

typedef struct _CODE_PAGE_INFO
{
	UINT uiCodePageID;
	DWORD dwcpHandle;
	const CODE_PAGE_DRV * pCodePageDrv;
}CODE_PAGE_INFO, * PCODE_PAGE_INFO;

extern const CODE_PAGE_DRV gb2312Drv;
extern const CODE_PAGE_DRV utf8Drv;
extern const CODE_PAGE_DRV CP_936Drv;
extern const CODE_PAGE_DRV CP_1252Drv;
static CODE_PAGE_INFO cpTable[] = 
{
	{ CP_GB2312, 1, &gb2312Drv },
	{ 936, 1, &CP_936Drv },
	{ 1252, 1, &CP_1252Drv },
	{ CP_UTF8, 1, &utf8Drv },
	{ 0 , 0 }
};

// ********************************************************************
//������PCODE_PAGE GetCodePage( UINT uiCodePage )
//������
//	IN uiCodePage - ����ҳ
//����ֵ��
//	�����
//�������������ݴ���ҳ�õ���Ӧ�Ĵ����
//����: 
// ********************************************************************

static const CODE_PAGE_INFO * GetCodePage( UINT uiCodePage )
{
	int i;
	PCODE_PAGE_INFO pcp = cpTable;

	for( i = 0; i < sizeof( cpTable ) / sizeof( CODE_PAGE_INFO ); i++, pcp++ )
	{
		if( pcp->uiCodePageID == uiCodePage )
			return pcp;
	}
	return NULL;
}


// ********************************************************************
//������int WinCP_MultiByteToWideChar( UINT uiCodePage,
//                               DWORD dwFlags,
//                               LPCSTR lpMultiByteStr,
//                               int cbMultiByte,
//                               LPWSTR lpWideCharStr, 
//						         int cchWideChar
//						       );
//������
//	IN uiCodePage - ����ҳ
//	IN dwFlags - ���ã�����Ϊ0
//	IN lpMultiByteStr - ��Ҫת�����ַ���ָ��
//	IN cbMultiByte - lpMultiByteStr ָ�����Ҫת�����ַ����������Ϊ-1����ʾ lpMultiByteStr ��0����
//	IN lpWideCharStr - ���ܻ���
//	IN cchWideChar - lpWideCharStr ָ�򻺴�Ĵ�С�����Ϊ0����ʾ�ú�������������Ҫ�Ľ��ܻ���Ĵ�С��û��ʵ�ʵ�ת�����̣�
//����ֵ��
//	����ɹ�����cchWideChar��0������ʵ��д�� lpWideCharStr �Ŀ��ַ���������0������
//	����ɹ�����cchWideCharΪ0��������Ҫ�Ľ��ܿ��ַ�����С
//����������
//	��һ���ַ���ʽת��Ϊ unicode ��ʽ
//����: 
// ********************************************************************

int WINAPI WinCP_MultiByteToWideChar(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPWSTR lpWideCharStr, 
						int cchWideChar 
						)
{
	const CODE_PAGE_INFO * pcp = GetCodePage( uiCodePage );

	if( pcp )
	{
		if( cchWideChar )
		{
			if( cbMultiByte == -1 )
				cbMultiByte = strlen( lpMultiByteStr ) + 1;
			if( cbMultiByte )
			    return pcp->pCodePageDrv->lpGetUnicode( pcp->dwcpHandle, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar );
		}
		else
		{	//�õ���Ҫ�Ļ�����ַ���
			return pcp->pCodePageDrv->lpCountBufferSize( pcp->dwcpHandle, 
				                                         (LPCVOID)lpMultiByteStr, 
														 cbMultiByte, 
														 FALSE );
		}
	}
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}

// ********************************************************************
//������int WINAPI WinCP_WideCharToMultiByte(
//						UINT uiCodePage, 
//						DWORD dwFlags, 
//						LPCWSTR lpWideCharStr, 
//						int cchWideChar, 
//						LPSTR lpMultiByteStr, 
//						int cbMultiByte, 
//						LPCSTR lpDefaultChar, 
//						BOOL * lpUsedDefaultChar 
//						)
//������
//	IN uiCodePage - ����ҳ
//	IN dwFlags - ���ã�����Ϊ0
//	IN lpWideCharStr - ��Ҫת���� unicode �ַ�ָ��
//	IN cchWideChar - lpWideCharStr ָ�����Ҫת�����ַ����������Ϊ-1����ʾ lpWideCharStr ��0����
//	IN lpMultiByteStr - ���ܻ���
//	IN cbMultiByte - lpMultiByteStr ָ�򻺴�Ĵ�С�����Ϊ0����ʾ�ú�������������Ҫ�Ľ��ܻ���Ĵ�С��û��ʵ�ʵ�ת�����̣�
//	IN lpDefaultChar - ���һ���ַ��޷�ת�����ø�ָ��ָ�����ַ�����
//	IN lpUsedDefaultChar - ����ָʾ lpDefaultChar ��־�Ƿ���ת���������ù���ֵΪ TRUE/FALSE
//����ֵ��
//	����ɹ�����cbMultiByte��0������ʵ��д�� lpMultiByteStr���ֽ���������0������
//	����ɹ�����cbMultiByteΪ0��������Ҫ�Ľ��ܻ����С
//����������
//	�� unicode �ַ�ת��Ϊָ����һ���ַ���ʽ
//����: 
// ********************************************************************

int WINAPI WinCP_WideCharToMultiByte(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCWSTR lpWideCharStr, 
						int cchWideChar, 
						LPSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPCSTR lpDefaultChar, 
						BOOL * lpUsedDefaultChar 
						)
{
	const CODE_PAGE_INFO * pcp = GetCodePage( uiCodePage );

	if( pcp )
	{
		if( cbMultiByte )
		{
			if( cchWideChar == -1 )
				cchWideChar = wcslen( lpWideCharStr ) + 1;
			if( cchWideChar )
			    return pcp->pCodePageDrv->lpGetChar( pcp->dwcpHandle, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, NULL, NULL );
		}
		else
		{	//�õ���Ҫ�Ļ����ַ���
			return pcp->pCodePageDrv->lpCountBufferSize( pcp->dwcpHandle, lpWideCharStr, cchWideChar, TRUE );
		}
	}
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}
