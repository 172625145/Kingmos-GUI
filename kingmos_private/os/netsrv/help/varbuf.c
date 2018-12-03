/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ɱ�BUFFER����
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-06-10
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
#ifdef XYG_PC_PRJ
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#else
#include <ewindows.h>

#endif
#include "varbuf.h"
#include "pcfile.h"

/***************  ȫ���� ���壬 ���� *****************/
/******************************************************/


// ********************************************************************
// ������
// ������
//	IN lpDealData-����������ݻ��崦��ṹ
//	IN dwFlag-�����־
//	IN dwLenGrow-ÿ�������ĳ���
//	IN dwLenMaxAlloc-����������ݵĳ���
//	IN dwLenTotal-��ǰ��������ݵĳ���
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// �������������� ���ݻ��崦��ṹ��BUFFER
// ����: 
// ********************************************************************
BOOL	VarBuf_Alloc( DATA_DEAL* lpDealData, DWORD dwFlag, DWORD dwLenGrow, DWORD dwLenMaxAlloc, DWORD dwLenTotal )
{
	//����
	lpDealData->lpData = (LPBYTE)malloc( dwLenTotal );
	if( !lpDealData->lpData )
	{
		return FALSE;
	}
	//��ʼ��
	if( dwFlag & DDF_ZEROALLOC )
	{
		memset( lpDealData->lpData, 0, dwLenTotal );
	}
	lpDealData->dwFlag = dwFlag;
	if( !dwLenGrow || dwLenGrow<DDGROW_DEFAULT )
	{
		lpDealData->dwLenGrow = DDGROW_DEFAULT;
	}
	else
	{
		lpDealData->dwLenGrow = dwLenGrow;
	}
	lpDealData->dwLenMaxAlloc = dwLenMaxAlloc;
	lpDealData->dwLenTotal = dwLenTotal;

	lpDealData->dwLenDealed = 0;
	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN lpDealData-����������ݻ��崦��ṹ
//	IN dwLenGrow-ÿ�������ĳ���
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// �����������Զ��ط��� ���ݻ��崦��ṹ��BUFFER
// ����: 
// ********************************************************************
BOOL	VarBuf_Grow( DATA_DEAL* lpDealData, DWORD dwLenGrow )
{
	LPBYTE			lpData;
	DWORD			dwLenTotal;

	//�������
	if( !(lpDealData->dwFlag & DDF_GROW) )
	{
		return FALSE;
	}

	//
	//1. ����ռ�
	//
	//���Ƚ������ķ���
	if( dwLenGrow < lpDealData->dwLenGrow )
	{
		dwLenTotal = lpDealData->dwLenGrow + lpDealData->dwLenTotal;
	}
	else
	{
		dwLenTotal = dwLenGrow + lpDealData->dwLenTotal;
	}
	if( lpDealData->dwFlag & DDF_MAXALLOC  )
	{
		//�������Ļ������ǲ��ǿ��� ѡ���ʵ��ķ���
		if( dwLenTotal > lpDealData->dwLenMaxAlloc )
		{
			if( dwLenGrow < lpDealData->dwLenGrow )
			{
				return FALSE;
			}
			//ѡ������ʵ��ķ���
			dwLenTotal = dwLenGrow + lpDealData->dwLenTotal;
			if( dwLenTotal > lpDealData->dwLenMaxAlloc )
			{
				return FALSE;
			}
		}
	}
	//
	//2. ����
	//
	lpData = (LPBYTE)malloc( dwLenTotal );
	if( !lpData )
	{
		return FALSE;
	}
	if( lpDealData->dwFlag & DDF_ZEROALLOC )
	{
		memset( lpData, 0, dwLenTotal );
	}


	//
	//3. ����
	//
	if( lpDealData->lpData )
	{
		memcpy( lpData, lpDealData->lpData, lpDealData->dwLenTotal );
		free( lpDealData->lpData );
	}
	lpDealData->lpData = lpData;
	lpDealData->dwLenTotal = dwLenTotal;

	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN lpDealData-����������ݻ��崦��ṹ
// ����ֵ��
//	
// �����������ͷ� ���ݻ��崦��ṹ��BUFFER
// ����: 
// ********************************************************************
void	VarBuf_Free( DATA_DEAL* lpDealData )
{
	if( lpDealData->lpData )
	{
		free( lpDealData->lpData );
	}
	lpDealData->lpData = NULL;
	lpDealData->dwLenDealed = 0;
	lpDealData->dwLenTotal = 0;
}

BOOL	VarBuf_ReadFile( LPCTSTR pszFileName, OUT DATA_DEAL* lpDealDataRecv )
{
	HANDLE			hFile;
	DWORD			dwSize;
	DWORD			dwTmp;

	//���ļ�
	hFile = ver_CreateFile( pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
	if( hFile==INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}
	//��ȡ�ļ���С
	dwSize = ver_GetFileSize( hFile, NULL );
	if( !dwSize )
	{
		ver_CloseHandle( hFile );
		return FALSE;
	}
	//alloc buffer
	if( !VarBuf_Alloc( lpDealDataRecv, DDF_GROW, 0, 0, dwSize ) )
	{
		ver_CloseHandle( hFile );
		//��ʾʧ��
		return FALSE;
	}
	//��ȡ�ļ�
	if( !ver_ReadFile( hFile, lpDealDataRecv->lpData, lpDealDataRecv->dwLenTotal, &dwTmp, NULL ) )
	{
		ver_CloseHandle( hFile );
		VarBuf_Free( lpDealDataRecv );
		//��ʾʧ��
		return FALSE;
	}
	ver_CloseHandle( hFile );
	lpDealDataRecv->dwLenTotal = dwTmp;
	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN lpDealData-����������ݻ��崦��ṹ
//	OUT pbRead-�����������������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ���������������ݻ��崦��ṹ��BUFFER�У����� BYTE ���͵�����
// ����: 
// ********************************************************************
BOOL	VarDec_Byte( DATA_DEAL* lpDealData, OUT BYTE* pbRead )
{
	//�������
	if( lpDealData->dwLenDealed >= lpDealData->dwLenTotal )
	{
		return FALSE;
	}
	//��ȡ���ݣ����ƶ�ָ�뵽��һ��λ��
	*pbRead = *(lpDealData->lpData+lpDealData->dwLenDealed);
	lpDealData->dwLenDealed++;
	return TRUE;
}
// ********************************************************************
// ������
// ������
//	IN lpDealData-����������ݻ��崦��ṹ
//	OUT pbRead-�����������������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ���������������ݻ��崦��ṹ��BUFFER�У����� BYTE ���͵����ݣ������Զ��ƶ�
// ����: 
// ********************************************************************
BOOL	VarPeek_Byte( DATA_DEAL* lpDealData, OUT BYTE* pbRead )
{
	//�������
	if( lpDealData->dwLenDealed >= lpDealData->dwLenTotal )
	{
		return FALSE;
	}
	*pbRead = *(lpDealData->lpData+lpDealData->dwLenDealed);
	return TRUE;
}
// ********************************************************************
// ������
// ������
//	IN OUT lpDealData-����������ݻ��崦��ṹ
//	IN bWrite-ָ���������������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ���������������ݻ��崦��ṹ��BUFFER�У����� BYTE ���͵�����
// ����: 
// ********************************************************************
BOOL	VarEnc_Byte( DATA_DEAL* lpDealData, BYTE bWrite )
{
	//�������
	if( (lpDealData->dwLenDealed+1) > lpDealData->dwLenTotal )
	{
		if( !VarBuf_Grow(lpDealData, 1) )
		{
			//��ʾʧ��
			return FALSE;
		}
	}
	//��ȡ���ݣ����ƶ�ָ�뵽��һ��λ��
	*(lpDealData->lpData+lpDealData->dwLenDealed) = bWrite;
	lpDealData->dwLenDealed++;
	return TRUE;
}
// ********************************************************************
// ������
// ������
//	IN OUT lpDealData-����������ݻ��崦��ṹ
//	IN pBufWrite-ָ���������������
//	IN dwLenWrite-ָ����������ݳ���
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ���������������ݻ��崦��ṹ��BUFFER�У����� BUFFER ���͵�����
// ����: 
// ********************************************************************
BOOL	VarEnc_Buffer( DATA_DEAL* lpDealData, LPBYTE pBufWrite, DWORD dwLenWrite )
{
	//�������
	if( dwLenWrite==0 )
	{
		return TRUE;
	}
	if( (lpDealData->dwLenDealed+dwLenWrite) > lpDealData->dwLenTotal )
	{
		if( !VarBuf_Grow(lpDealData, dwLenWrite) )
		{
			//��ʾʧ��
			return FALSE;
		}
	}
	//
	memcpy( lpDealData->lpData+lpDealData->dwLenDealed, pBufWrite, dwLenWrite );
	lpDealData->dwLenDealed += dwLenWrite;
	return TRUE;
}
