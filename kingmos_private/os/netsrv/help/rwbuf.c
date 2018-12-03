/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����RW BUF ����
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-05-17
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
//"\inc_local"
#include "rwbuf.h"


/***************  ȫ���� ���壬 ���� *****************/

/******************************************************/


// ********************************************************************
// ������
// ������
//	IN lpRWBuf-Ҫ������RWBUFFER
// ����ֵ��
//	
// �������������ý��ջ���
// ����: 
// ********************************************************************
BOOL	RWBuf_Alloc( RWBUFFER* lpRWBuf, DWORD nLenNew, DWORD nLenMin )
{
	LPBYTE		pAlloc;

	//��ʼ����
	EnterCriticalSection( &lpRWBuf->csBufRW );
	if( lpRWBuf->pBufRW )
	{
		if( (nLenNew<nLenMin) || (nLenNew==lpRWBuf->dwLenRW) )
		{
			//����Ѿ����䣬�����䳬����͵��ߣ����߷����뵱ǰ��С��ͬ����ֱ�ӷ���
			LeaveCriticalSection( &lpRWBuf->csBufRW );
			return TRUE;
		}
		pAlloc = (LPBYTE)realloc( lpRWBuf->pBufRW, nLenNew );
	}
	else
	{
		pAlloc = (LPBYTE)malloc( nLenNew );
	}
	//���������Ϣ
	if( pAlloc )
	{
		lpRWBuf->pBufRW = pAlloc;
		lpRWBuf->dwLenRW = nLenNew;

		lpRWBuf->dwCntRW = 0;
		lpRWBuf->dwReadRW = 0;
		lpRWBuf->dwWriteRW = 0;

		LeaveCriticalSection( &lpRWBuf->csBufRW );
		return TRUE;
	}
	else
	{
		LeaveCriticalSection( &lpRWBuf->csBufRW );
		return FALSE;
	}
}

// ********************************************************************
// ������
// ������
//	IN lpRWBuf-Ҫ������RWBUFFER
// ����ֵ��
//	
// �����������ͷ�RWBUFFER
// ����: 
// ********************************************************************
void	RWBuf_Free( RWBUFFER* lpRWBuf )
{
	//�ͷ�
	if( lpRWBuf->pBufRW )
	{
		free( lpRWBuf->pBufRW );
		lpRWBuf->pBufRW = NULL;
	}
	//
	DeleteCriticalSection( &lpRWBuf->csBufRW );
}


// ********************************************************************
// ������
// ������
//	IN/OUT lpRWBuf-Ҫ������RWBUFFER
//	IN pData-Ҫ���������
//	IN/OUT pdwLenData-ָ��pData�ĳ��ȣ�����ȡ�������ݵĳ���
//	IN dwFlagWrite-ָ������ı�־
// ����ֵ��
//	
// ������������������
// ����: 
// ********************************************************************
BOOL	RWBuf_WriteData( RWBUFFER* lpRWBuf, LPBYTE pData, IN OUT DWORD* pdwLenData, DWORD dwFlagWrite )
{
	LPBYTE		pOffset;
	DWORD		dwLenAll;
	DWORD		nLen_W;

	//�������
	dwLenAll = *pdwLenData;
	if( !pData )
	{
		return FALSE;
	}
	if( !dwLenAll )
	{
		//����Ҫ ��������
		return TRUE;
	}
	//
	EnterCriticalSection( &lpRWBuf->csBufRW );
	//��ȡʣ��ռ�
	nLen_W = lpRWBuf->dwLenRW - lpRWBuf->dwCntRW;
	if( !nLen_W )
	{
		//û��ʣ��ռ� ��������
		LeaveCriticalSection( &lpRWBuf->csBufRW );
		return FALSE;
	}
	//����ɱ���Ŀռ� �Ƿ��㹻
	pOffset = pData;
	if( dwLenAll > nLen_W )
	{
		if( dwFlagWrite & RWF_W_ENOUGH )
		{
			//û���㹻ʣ��ռ� ��������
			LeaveCriticalSection( &lpRWBuf->csBufRW );
			return FALSE;
		}
		dwLenAll = nLen_W;
	}
	*pdwLenData = dwLenAll;

	//�洢����1��
	if( lpRWBuf->dwWriteRW >= lpRWBuf->dwReadRW )
	{
		//�жϵ�1�εĳ���
		nLen_W = (lpRWBuf->dwLenRW - lpRWBuf->dwWriteRW);
		if( nLen_W > dwLenAll )
		{
			nLen_W = dwLenAll;
		}
		//��������
		memcpy( lpRWBuf->pBufRW+lpRWBuf->dwWriteRW, pOffset, nLen_W );
		pOffset += nLen_W;
		dwLenAll -= nLen_W;
		//���½�����Ϣ
		lpRWBuf->dwCntRW += nLen_W;
		lpRWBuf->dwWriteRW += nLen_W;
		if( lpRWBuf->dwWriteRW >= lpRWBuf->dwLenRW )
		{
			lpRWBuf->dwWriteRW -= lpRWBuf->dwLenRW;
		}
	}
	//�洢����2��
	if( dwLenAll )
	{
		//��������
		memcpy( lpRWBuf->pBufRW+lpRWBuf->dwWriteRW, pOffset, dwLenAll );
		//���½�����Ϣ
		lpRWBuf->dwCntRW += dwLenAll;
		lpRWBuf->dwWriteRW += dwLenAll;
		if( lpRWBuf->dwWriteRW >= lpRWBuf->dwLenRW )
		{
			lpRWBuf->dwWriteRW -= lpRWBuf->dwLenRW;
		}
	}
	//
	LeaveCriticalSection( &lpRWBuf->csBufRW );

	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN/OUT lpRWBuf-Ҫ������RWBUFFER
//	OUT pData-Ҫ�������ȡ����
//	IN/OUT pdwLenData-ָ��pData�ĳ��ȣ������� ��ȡ���ݵĳ���
// ����ֵ��
//	
// ������������ȡ����
// ����: 
// ********************************************************************
BOOL	RWBuf_ReadData( RWBUFFER* lpRWBuf, LPBYTE pData, IN OUT DWORD* pdwLenData )
{
	LPBYTE		pOffset;
	DWORD		dwLenAll;
	DWORD		nLen_R;

	//�������
	dwLenAll = *pdwLenData;
	if( !pData || !dwLenAll )
	{
		//����Ҫ ��ȡ����
		return FALSE;
	}
	//
	EnterCriticalSection( &lpRWBuf->csBufRW );
	//ȷ��Ҫ��ȡ�ĳ���
	if( dwLenAll > lpRWBuf->dwCntRW )
	{
		dwLenAll = lpRWBuf->dwCntRW;
	}
	if( dwLenAll==0 )
	{
		//û�����ݿɶ�
		LeaveCriticalSection( &lpRWBuf->csBufRW );
		return FALSE;
	}
	pOffset = pData;
	*pdwLenData = dwLenAll;

	//��ȡ���ݴӵ�1��
	if( lpRWBuf->dwWriteRW <= lpRWBuf->dwReadRW )
	{
		//�жϵ�1�εĳ���
		nLen_R = (lpRWBuf->dwLenRW - lpRWBuf->dwReadRW);
		if( nLen_R > dwLenAll )
		{
			nLen_R = dwLenAll;
		}
		//��������
		memcpy( pOffset, lpRWBuf->pBufRW+lpRWBuf->dwReadRW, nLen_R );
		pOffset += nLen_R;
		dwLenAll -= nLen_R;
		//���½�����Ϣ
		lpRWBuf->dwCntRW -= nLen_R;
		lpRWBuf->dwReadRW += nLen_R;
		if( lpRWBuf->dwReadRW >= lpRWBuf->dwLenRW )
		{
			lpRWBuf->dwReadRW -= lpRWBuf->dwLenRW;
		}
	}
	//��ȡ���ݴӵ�2��
	if( dwLenAll )
	{
		//��������
		memcpy( pOffset, lpRWBuf->pBufRW+lpRWBuf->dwReadRW, dwLenAll );
		//���½�����Ϣ
		lpRWBuf->dwCntRW -= dwLenAll;
		lpRWBuf->dwReadRW += dwLenAll;
		if( lpRWBuf->dwReadRW >= lpRWBuf->dwLenRW )
		{
			lpRWBuf->dwReadRW -= lpRWBuf->dwLenRW;
		}
	}
	//
	LeaveCriticalSection( &lpRWBuf->csBufRW );

	return TRUE;
}


DWORD	RWBuf_GetCntLeft( RWBUFFER* lpRWBuf, BOOL fUseCri )
{
	DWORD			dwLenLeft;

	//
	if( fUseCri )
	{
		EnterCriticalSection( &lpRWBuf->csBufRW );
	}
	//
	dwLenLeft = lpRWBuf->dwLenRW - lpRWBuf->dwCntRW;
	//
	if( fUseCri )
	{
		LeaveCriticalSection( &lpRWBuf->csBufRW );
	}
	return dwLenLeft;
}


// ********************************************************************
// ������
// ������
//	IN lpRWBuf-Ҫ������RWBUFFER
// ����ֵ��
//	
// �������������ý��ջ���
// ����: 
// ********************************************************************
BOOL	RWBuf_SetReadOffset( RWBUFFER* lpRWBuf, DWORD dwReadOffset, BOOL fUseCri )
{
	//
	if( fUseCri )
	{
		EnterCriticalSection( &lpRWBuf->csBufRW );
	}
	//
	if( dwReadOffset>lpRWBuf->dwCntRW )
	{
		dwReadOffset = lpRWBuf->dwCntRW;
	}
	//
	lpRWBuf->dwCntRW -= dwReadOffset;
	lpRWBuf->dwReadRW += dwReadOffset;
	if( lpRWBuf->dwReadRW >= lpRWBuf->dwLenRW )
	{
		lpRWBuf->dwReadRW -= lpRWBuf->dwLenRW;
	}
	//
	if( fUseCri )
	{
		LeaveCriticalSection( &lpRWBuf->csBufRW );
	}
	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN lpRWBuf-Ҫ������RWBUFFER
// ����ֵ��
//	
// ������������ȡƫ�ƺ� ����λ�� �� ���еĳ���
// ����: 
// ********************************************************************
BOOL	RWBuf_GetReadBuf( RWBUFFER* lpRWBuf, DWORD dwReadOffset, LPBYTE* ppBufRead, DWORD* pdwLenRead )
{
	if( dwReadOffset>lpRWBuf->dwCntRW )
	{
		*ppBufRead = NULL;
		*pdwLenRead = 0;
		return FALSE;
	}
	//��ȡλ��
	dwReadOffset += lpRWBuf->dwReadRW;
	if( dwReadOffset >= lpRWBuf->dwLenRW )
	{
		dwReadOffset -= lpRWBuf->dwLenRW;
	}
	*ppBufRead = lpRWBuf->pBufRW + dwReadOffset;
	if( dwReadOffset>lpRWBuf->dwWriteRW )
	{
		*pdwLenRead = lpRWBuf->dwLenRW - dwReadOffset;
	}
	else
	{
		*pdwLenRead = lpRWBuf->dwWriteRW - dwReadOffset;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////

// ********************************************************************
// ������
// ������
//	IN lpRWBuf2-Ҫ������RWBUF2
// ����ֵ��
//	
// �������������ý��ջ���
// ����: 
// ********************************************************************
BOOL	RWBuf2_Alloc( RWBUF2* lpRWBuf2, DWORD nLenNew, DWORD nLenMin )
{
	LPBYTE		pAlloc;

	//��ʼ����
	if( lpRWBuf2->pBufRW )
	{
		if( (nLenNew<nLenMin) || (nLenNew==lpRWBuf2->dwLenRW) )
		{
			//����Ѿ����䣬�����䳬����͵��ߣ����߷����뵱ǰ��С��ͬ����ֱ�ӷ���
			return TRUE;
		}
		pAlloc = (LPBYTE)realloc( lpRWBuf2->pBufRW, nLenNew );
	}
	else
	{
		pAlloc = (LPBYTE)malloc( nLenNew );
	}
	//���������Ϣ
	if( pAlloc )
	{
		lpRWBuf2->pBufRW = pAlloc;
		lpRWBuf2->dwLenRW = nLenNew;

		lpRWBuf2->dwCntRW = 0;
		lpRWBuf2->dwReadRW = 0;
		lpRWBuf2->dwWriteRW = 0;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ********************************************************************
// ������
// ������
//	IN lpRWBuf2-Ҫ������RWBUF2
// ����ֵ��
//	
// �����������ͷ�RWBUF2
// ����: 
// ********************************************************************
void	RWBuf2_Free( RWBUF2* lpRWBuf2 )
{
	//�ͷ�
	if( lpRWBuf2->pBufRW )
	{
		free( lpRWBuf2->pBufRW );
		lpRWBuf2->pBufRW = NULL;
	}
}


// ********************************************************************
// ������
// ������
//	IN/OUT lpRWBuf2-Ҫ������RWBUF2
//	IN pData-Ҫ���������
//	IN/OUT pdwLenData-ָ��pData�ĳ��ȣ�����ȡ�������ݵĳ���
//	IN dwFlagWrite-ָ������ı�־
// ����ֵ��
//	
// ������������������
// ����: 
// ********************************************************************
BOOL	RWBuf2_WriteData( RWBUF2* lpRWBuf2, LPBYTE pData, IN OUT DWORD* pdwLenData, DWORD dwFlagWrite )
{
	LPBYTE		pOffset;
	DWORD		dwLenAll;
	DWORD		nLen_W;

	//�������
	dwLenAll = *pdwLenData;
	if( !pData )
	{
		return FALSE;
	}
	if( !dwLenAll )
	{
		//����Ҫ ��������
		return TRUE;
	}
	//��ȡʣ��ռ�
	nLen_W = lpRWBuf2->dwLenRW - lpRWBuf2->dwCntRW;
	if( !nLen_W )
	{
		//û��ʣ��ռ� ��������
		return FALSE;
	}
	//����ɱ���Ŀռ� �Ƿ��㹻
	pOffset = pData;
	if( dwLenAll > nLen_W )
	{
		if( dwFlagWrite & RWF_W_ENOUGH )
		{
			//û���㹻ʣ��ռ� ��������
			return FALSE;
		}
		dwLenAll = nLen_W;
	}
	*pdwLenData = dwLenAll;

	//�洢����1��---
	if( lpRWBuf2->dwWriteRW >= lpRWBuf2->dwReadRW )
	{
		//�жϵ�1�εĳ���
		nLen_W = (lpRWBuf2->dwLenRW - lpRWBuf2->dwWriteRW);
		if( nLen_W > dwLenAll )
		{
			nLen_W = dwLenAll;
		}
		//��������
		memcpy( lpRWBuf2->pBufRW+lpRWBuf2->dwWriteRW, pOffset, nLen_W );
		pOffset += nLen_W;
		dwLenAll -= nLen_W;
		//���½�����Ϣ
		lpRWBuf2->dwCntRW += nLen_W;
		lpRWBuf2->dwWriteRW += nLen_W;
		if( lpRWBuf2->dwWriteRW >= lpRWBuf2->dwLenRW )
		{
			lpRWBuf2->dwWriteRW -= lpRWBuf2->dwLenRW;
		}
	}
	//�洢����2��---
	if( dwLenAll )
	{
		//��������
		memcpy( lpRWBuf2->pBufRW+lpRWBuf2->dwWriteRW, pOffset, dwLenAll );
		//���½�����Ϣ
		lpRWBuf2->dwCntRW += dwLenAll;
		lpRWBuf2->dwWriteRW += dwLenAll;
		if( lpRWBuf2->dwWriteRW >= lpRWBuf2->dwLenRW )
		{
			lpRWBuf2->dwWriteRW -= lpRWBuf2->dwLenRW;
		}
	}

	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN/OUT lpRWBuf2-Ҫ������RWBUF2
//	OUT pData-Ҫ�������ȡ����
//	IN/OUT pdwLenData-ָ��pData�ĳ��ȣ������� ��ȡ���ݵĳ���
// ����ֵ��
//	
// ������������ȡ����
// ����: 
// ********************************************************************
BOOL	RWBuf2_ReadData( RWBUF2* lpRWBuf2, LPBYTE pData, IN OUT DWORD* pdwLenData )
{
	LPBYTE		pOffset;
	DWORD		dwLenAll;
	DWORD		nLen_R;

	//�������
	dwLenAll = *pdwLenData;
	if( !pData || !dwLenAll )
	{
		//����Ҫ ��ȡ����
		return FALSE;
	}
	//ȷ��Ҫ��ȡ�ĳ���
	if( dwLenAll > lpRWBuf2->dwCntRW )
	{
		dwLenAll = lpRWBuf2->dwCntRW;
	}
	if( dwLenAll==0 )
	{
		//û�����ݿɶ�
		return FALSE;
	}
	pOffset = pData;
	*pdwLenData = dwLenAll;

	//��ȡ���ݴӵ�1��
	if( lpRWBuf2->dwWriteRW <= lpRWBuf2->dwReadRW )
	{
		//�жϵ�1�εĳ���
		nLen_R = (lpRWBuf2->dwLenRW - lpRWBuf2->dwReadRW);
		if( nLen_R > dwLenAll )
		{
			nLen_R = dwLenAll;
		}
		//��������
		memcpy( pOffset, lpRWBuf2->pBufRW+lpRWBuf2->dwReadRW, nLen_R );
		pOffset += nLen_R;
		dwLenAll -= nLen_R;
		//���½�����Ϣ
		lpRWBuf2->dwCntRW -= nLen_R;
		lpRWBuf2->dwReadRW += nLen_R;
		if( lpRWBuf2->dwReadRW >= lpRWBuf2->dwLenRW )
		{
			lpRWBuf2->dwReadRW -= lpRWBuf2->dwLenRW;
		}
	}
	//��ȡ���ݴӵ�2��
	if( dwLenAll )
	{
		//��������
		memcpy( pOffset, lpRWBuf2->pBufRW+lpRWBuf2->dwReadRW, dwLenAll );
		//���½�����Ϣ
		lpRWBuf2->dwCntRW -= dwLenAll;
		lpRWBuf2->dwReadRW += dwLenAll;
		if( lpRWBuf2->dwReadRW >= lpRWBuf2->dwLenRW )
		{
			lpRWBuf2->dwReadRW -= lpRWBuf2->dwLenRW;
		}
	}

	return TRUE;
}



