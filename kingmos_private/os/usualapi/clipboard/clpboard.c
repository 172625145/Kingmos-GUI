/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵�������а崦����
�汾�ţ�1.0.0.456
����ʱ�ڣ�2001-02-26
���ߣ� �½��� Jami
�޸ļ�¼��
**************************************************/
#include <ewindows.h>
#include <eclipbrd.h>
#include <eptrlist.h>
#include <eglobmem.h>
// **************************************************
// ��������
// **************************************************
#define CLIPBOARDMAXLEN  1024

static TCHAR lpClipboard[CLIPBOARDMAXLEN];
static short cClipboardTextLen=0;
static TCHAR fOpenClipboard=FALSE;


/*
void OpenPDAClipboard(void)
{
	fOpenClipboard=TRUE;
}
void ClosePDAClipboard(void)
{
	fOpenClipboard=FALSE;
}
short SetPDAClipboardData(LPCTSTR lpBuffer,short cchBufLen)
{
	if (fOpenClipboard==FALSE)
		return 0;
	if (cchBufLen>CLIPBOARDMAXLEN)
		cchBufLen=CLIPBOARDMAXLEN;
	memcpy(lpClipboard,lpBuffer,cchBufLen);
	cClipboardTextLen=cchBufLen;
	return cchBufLen;
}
LPCTSTR GetPDAClipboardData(void)
{
	if (fOpenClipboard==FALSE)
		return NULL;
	return lpClipboard;
}
short GetPDAClipboardDataLen(void)
{
	if (fOpenClipboard==FALSE)
		return 0;
	return cClipboardTextLen;
}
void EmptyPDAClipboard(void)
{
	if (fOpenClipboard==FALSE)
		return;
	memset(lpClipboard,0,CLIPBOARDMAXLEN);
	cClipboardTextLen=0;
}
*/

/************************************************************************************/
// The Compatible Microsoft API Interface
/************************************************************************************/
#define STANDFORMATNUM 6
static LPPTRLIST ptrClipDataList = NULL;
static HWND hOwner = NULL;
static UINT g_iMaxItemNum = STANDFORMATNUM;


// **************************************************
// ������BOOL WINAPI Clip_OpenClipboard(HWND hWndNewOwner)
// ������
// 	IN hWndNewOwner -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������򿪼��а塣
// ����: 
// **************************************************
BOOL WINAPI Clip_OpenClipboard(HWND hWndNewOwner)
{
	UINT i;

	if (ptrClipDataList == NULL)
	{  // ���а�������б���δ��ʼ��
		ptrClipDataList = (LPPTRLIST)malloc(sizeof(PTRLIST));  // �����ڴ�
		if (ptrClipDataList == NULL) 
			return FALSE;  // ����ʧ��
		PtrListCreate(ptrClipDataList,(ccIndex)g_iMaxItemNum,(ccIndex)16);  // ����ָ���б�
		for (i = 0; i< g_iMaxItemNum; i++)
			PtrListInsert(ptrClipDataList, NULL);  // ��������Ϊ�յ�ָ��
	}
	if (hWndNewOwner == NULL)  // ���ھ������
		return FALSE;
	if (hOwner != NULL)  // ��ǰ���а廹������ʹ��
		return FALSE;
	hOwner = hWndNewOwner;  // ���ô򿪴��ھ��
	return TRUE;
}
// **************************************************
// ������BOOL WINAPI Clip_CloseClipboard(void)
// ������
//	��
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// �����������رռ��а塣
// ����: 
// **************************************************
BOOL WINAPI Clip_CloseClipboard(void)
{
	hOwner = NULL;  // ���ô򿪴���Ϊ��
	return TRUE;
}
// **************************************************
// ������HANDLE WINAPI Clip_SetClipboardData(UINT uFormat, HANDLE hMem )
// ������
// 	IN uFormat -- ���ݸ�ʽ
// 	IN hMem -- ���ݾ�����þ���� GlobalAlloc ����
// 
// ����ֵ���ɹ��������õ����ݾ�������򷵻�NULL
// ��������������ָ����ʽ�����ݡ�
// ����: 
// **************************************************
HANDLE WINAPI Clip_SetClipboardData(UINT uFormat, HANDLE hMem )
{
	void *item;
//	DWORD dwDataSize;
//	HGLOBAL hNewData ;

	if (hMem == NULL)
		return NULL;

//	dwDataSize = GlobalSize(  (HGLOBAL) hMem  );
//	if (dwDataSize == NULL)
//		return NULL;
	
//	hNewData = GlobalAlloc(GMEM_MOVEABLE , dwDataSize);
//	if (hNewData != NULL)
//	{
//			LPSTR pszOldData = (LPSTR) GlobalLock(hMem);
//			LPSTR pszNewData = (LPSTR) GlobalLock(hNewData);
//			memcpy(pszNewData,  pszOldData,dwDataSize);
//			GlobalUnlock(hMem);
//			GlobalUnlock(hNewData);
//			GlobalFree(hMem);
//	}
//	else
//		return NULL;

	item = PtrListAt( ptrClipDataList,uFormat);  // �õ�ԭ������
	if (item)  // �����Ѿ�����
//		free(item);
		GlobalFree(item);  // �ͷ�ԭ��������
//	PtrListAtPut( ptrClipDataList,uFormat, hNewData );
//	return hNewData;
	PtrListAtPut( ptrClipDataList,uFormat, hMem );  // ����ָ��������
	return hMem;
}
// **************************************************
// ������HANDLE WINAPI Clip_GetClipboardData(UINT uFormat )
// ������
// 	IN uFormat -- ָ�������ݸ�ʽ
// 
// ����ֵ������ָ����ʽ������
// �����������õ�ָ�����ݵĸ�ʽ��
// ����: 
// **************************************************
HANDLE WINAPI Clip_GetClipboardData(UINT uFormat )
{
	return PtrListAt( ptrClipDataList,uFormat);
}
// **************************************************
// ������BOOL WINAPI Clip_EmptyClipboard(VOID)
// ������
// 	��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ռ��а����ݡ�
// ����: 
// **************************************************
BOOL WINAPI Clip_EmptyClipboard(VOID)
{
	void *item;
	UINT i;

		for (i = 0; i< g_iMaxItemNum; i++)
		{
			item = PtrListAt( ptrClipDataList,i);  // �õ�ָ����ʽ������
			if (item)
//				free(item);
				GlobalFree(item);  // �ͷ�ԭ��������
//			PtrListInsert(ptrClipDataList, NULL);
			PtrListAtPut( ptrClipDataList,i, NULL);  // ����ָ����ʽ������ΪNULL
		}
		return TRUE;
}

// **************************************************
// ������BOOL WINAPI Clip_IsClipboardFormatAvailable(UINT uFormat)
// ������
// 	IN uFormat -- ָ����ʽ
// 
// ����ֵ��������Ч����TRUE�����򷵻�FALSE
// �����������ж�ָ����ʽ�������Ƿ���Ч��
// ����: 
// **************************************************
BOOL WINAPI Clip_IsClipboardFormatAvailable(UINT uFormat)
{
	LPVOID lpData;
	
		if (ptrClipDataList == NULL)  // û�м��а�����
			return FALSE; // ������Ч
		lpData = PtrListAt( ptrClipDataList,uFormat);  // �õ�ָ����ʽ����
		if (lpData) // ���ݴ���
			return TRUE; // ����������Ч
		else 
			return FALSE; // ����������Ч
}
