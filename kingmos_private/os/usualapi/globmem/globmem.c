/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����ȫ���ڴ����
�汾�ţ�1.0.0
����ʱ�ڣ�2003-05-23
���ߣ��½��� Jami chen
�޸ļ�¼��
**************************************************/

#include <ewindows.h>
#include <eglobmem.h>
#include <usualsrv.h>

/***************  ȫ���� ���壬 ���� *****************/

typedef struct{
	UINT uFlags;
	DWORD dwBytes;
	LPTSTR lpMem;
}GLOBALMEMDATA, *LPGLOBALMEMDATA;

// **************************************************
// ������HGLOBAL WINAPI Gbl_GlobalAlloc(  UINT uFlags,  DWORD dwBytes )
// ������
// 	IN uFlags -- Ҫ������ڴ��־
// 	IN dwBytes -- Ҫ������ڴ��ַ���
// 
// ����ֵ�������ڴ���
// ��������������һ��ȫ���ڴ档
// ����: 
// **************************************************
#define DEBUG_Gbl_GlobalAlloc 0
HGLOBAL WINAPI Gbl_GlobalAlloc(  UINT uFlags,  DWORD dwBytes )
{
	LPGLOBALMEMDATA lpGlobalMem;
	
	lpGlobalMem = (LPGLOBALMEMDATA)malloc(sizeof(GLOBALMEMDATA));  // ����ȫ���ڴ����ݽṹ
	//
	
	if (lpGlobalMem == NULL)
		return NULL;  // ����ʧ��
	//lilin 2005-01-06,����ʹ�ù����ڴ���
//	lpGlobalMem->lpMem = (LPTSTR)malloc(dwBytes);  // ����ָ���ַ������ڴ�
	lpGlobalMem->lpMem = (LPTSTR)Page_AllocMem( dwBytes, NULL, PM_SHARE );  // ����ָ���ַ������ڴ�
	DEBUGMSG( DEBUG_Gbl_GlobalAlloc, ( "Gbl_GlobalAlloc: lpMem= 0x%x.\r\n", lpGlobalMem->lpMem ) );
	//
	if (lpGlobalMem->lpMem == NULL)
	{  // ����ʧ��
		free(lpGlobalMem);
		return NULL;
	}
	lpGlobalMem->uFlags = uFlags;  // �����ڴ��־
	lpGlobalMem->dwBytes = dwBytes;  // ���÷����ַ���
	return lpGlobalMem;  // ���ؾ��
}

// **************************************************
// ������HGLOBAL WINAPI Gbl_GlobalDiscard(  HGLOBAL hglbMem  )
// ������
// 	IN hglbMem -- ȫ���ڴ�ָ�� 
// 
// ����ֵ����
// �����������������ܡ�
// ����: 
// **************************************************
HGLOBAL WINAPI Gbl_GlobalDiscard(  HGLOBAL hglbMem  )
{
	return NULL;
}

// **************************************************
// ������UINT WINAPI Gbl_GlobalFlags(  HGLOBAL hMem   )
// ������
// 	IN hMem -- �ڴ���
// 
// ����ֵ�������ڴ�ı�־
// �����������õ�ָ��ȫ���ڴ�ı�־��
// ����: 
// **************************************************
UINT WINAPI Gbl_GlobalFlags(  HGLOBAL hMem   )
{
	LPGLOBALMEMDATA lpGlobalMem;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;
		if (lpGlobalMem == NULL)  // ������
			return 0;
		return lpGlobalMem->uFlags;  // �����ڴ�ı�־
}

// **************************************************
// ������HGLOBAL WINAPI Gbl_GlobalFree(HGLOBAL hMem )
// ������
// 	IN hMem -- �ڴ���
// 
// ����ֵ��������Ч���
// �����������ͷ�ȫ���ڴ�
// ����: 
// **************************************************
HGLOBAL WINAPI Gbl_GlobalFree(HGLOBAL hMem )
{
	LPGLOBALMEMDATA lpGlobalMem;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;  // �õ�ȫ���ڴ����ݽṹ
		if (lpGlobalMem == NULL)  // �����Ч
			return NULL;
		if (lpGlobalMem->lpMem)  // �ͷ��ڴ�
		{
			//lilin -2005-01-06����ʹ�ù����ڴ���
			//free(lpGlobalMem->lpMem);
			Page_FreeMem( lpGlobalMem->lpMem, lpGlobalMem->dwBytes );
			//
		}
		free(lpGlobalMem); // �ͷ����ݽṹ
		return NULL; // ����
}

// **************************************************
// ������HGLOBAL WINAPI Gbl_GlobalHandle(  LPCVOID pMem  )
// ������
// 	IN pMem -- ָ���ڴ�
// 
// ����ֵ�� �����ڴ���
// ������������������
// ����: 
// **************************************************
HGLOBAL WINAPI Gbl_GlobalHandle(  LPCVOID pMem  )
{
	return NULL;
}

// **************************************************
// ������LPVOID WINAPI Gbl_GlobalLock(  HGLOBAL hMem   )
// ������
// 	IN hMem -- �ڴ���
// 
// ����ֵ�� ����ȫ���ڴ�ָ��
// ��������������ȫ���ڴ档
// ����: 
// **************************************************
#define DEBUG_Gbl_GlobalLock 0
LPVOID WINAPI Gbl_GlobalLock(  HGLOBAL hMem   )
{
	LPGLOBALMEMDATA lpGlobalMem;
	LPVOID lpMapPtr;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;  // �õ�ȫ���ڴ����ݽṹ
		if (lpGlobalMem == NULL)  // �����Ч
			return NULL;
		lpMapPtr = MapPtrToProcess( (LPVOID)lpGlobalMem->lpMem, GetCurrentProcess() );   // ӳ��ָ��
		DEBUGMSG( DEBUG_Gbl_GlobalLock, ( "Gbl_GlobalLock: lpMem= 0x%x, MapPtr=0x%x.\r\n", lpGlobalMem->lpMem, lpMapPtr ) );
		return lpMapPtr;  // �����Ѿ�ӳ���ָ��
}

// **************************************************
// ������HGLOBAL WINAPI Gbl_GlobalReAlloc(  HGLOBAL hMem,   DWORD dwBytes, UINT uFlags )
// ������
// 	IN hMem -- �Ѿ����ڵ��ڴ���
//	 IN dwBytes -- Ҫ������ַ���
//	 IN uFlags -- �µ��ڴ�ı�־
// 
// ����ֵ�������·�����ڴ���
// �������������·���һ��ȫ���ڴ档
// ����: 
// **************************************************
HGLOBAL WINAPI Gbl_GlobalReAlloc(  HGLOBAL hMem,   DWORD dwBytes, UINT uFlags )
{
	LPGLOBALMEMDATA lpGlobalMem;
	LPTSTR lpNewMem;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;  // �õ�ԭ�������ݽṹ
		if (lpGlobalMem == NULL) // �����Ч
			return NULL;
		{
			//2005-01-06 ����ʹ�ù����ڴ���
		   // lpNewMem = realloc(lpGlobalMem->lpMem,dwBytes);  // ���·���һ���ڴ�
	    	lpNewMem = (LPTSTR)Page_AllocMem( dwBytes, NULL, PM_SHARE );  // ����ָ���ַ������ڴ�
	    	//
		}
		if (lpNewMem == NULL)  // ����ʧ��
			return NULL;
		//2005-01-06 ����ʹ�ù����ڴ���
		Page_FreeMem( lpGlobalMem->lpMem, lpGlobalMem->dwBytes );
		//
		lpGlobalMem->lpMem = lpNewMem;  // �����µ��ڴ�
		lpGlobalMem->dwBytes = dwBytes;  // �����µ��ַ�����
		lpGlobalMem->uFlags = uFlags;  // �����µ��ڴ��־

		return lpGlobalMem;  // �����ڴ���
}

// **************************************************
// ������DWORD WINAPI Gbl_GlobalSize(  HGLOBAL hMem  )
// ������
// 	IN hMem -- �ڴ���
// 
// ����ֵ�� ����ָ��ȫ���ڴ���ڴ��С
// �����������õ�ָ��ȫ���ڴ���ڴ��С��
// ����: 
// **************************************************
DWORD WINAPI Gbl_GlobalSize(  HGLOBAL hMem  )
{
	LPGLOBALMEMDATA lpGlobalMem;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;  // �õ�ȫ���ڴ����ݽṹ
		if (lpGlobalMem == NULL)  // �����Ч
			return 0;
		return lpGlobalMem->dwBytes;  // �����ڴ��С
}

// **************************************************
// ������BOOL WINAPI Gbl_GlobalUnlock(  HGLOBAL hMem )
// ������
// 	IN hMem -- ȫ���ڴ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������������ȫ���ڴ档
// ����: 
// **************************************************
BOOL WINAPI Gbl_GlobalUnlock(  HGLOBAL hMem )
{
	return TRUE;
}
