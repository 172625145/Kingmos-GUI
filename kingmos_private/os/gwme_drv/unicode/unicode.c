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

static CODE_PAGE_INFO cpTable[] = 
{
	{ CP_GB2312, 1, &gb2312Drv },
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

	for( i = 0; i < sizeof( cpTable ) / sizeof( CODE_PAGE_INFO ); i++ )
	{
		if( pcp->uiCodePageID == uiCodePage )
			return pcp;
	}
	return NULL;
}


// ********************************************************************
//������int MultiByteToWideChar( UINT uiCodePage,
//                               DWORD dwFlags,
//                               LPCSTR lpMultiByteStr,
//                               int cbMultiByte,
//                               LPWSTR lpWideCharStr, 
//						         int cchWideChar
//						       );
//������
//	IN uiCodePage - ����ҳ
//����ֵ��
//	�����
//�������������ݴ���ҳ�õ���Ӧ�Ĵ����
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
		{
			return pcp->pCodePageDrv->lpCountBufferSize( pcp->dwcpHandle, 
				                                         (LPCVOID)lpMultiByteStr, 
														 cbMultiByte, 
														 FALSE );
		}
	}
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}

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
		{
			return pcp->pCodePageDrv->lpCountBufferSize( pcp->dwcpHandle, lpWideCharStr, cchWideChar, TRUE );
		}
	}
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}
