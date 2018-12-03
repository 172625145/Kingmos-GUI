/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：剪切板处理函数
版本号：1.0.0.456
开发时期：2001-02-26
作者： 陈建明 Jami
修改记录：
**************************************************/
#include <ewindows.h>
#include <eclipbrd.h>
#include <eptrlist.h>
#include <eglobmem.h>
// **************************************************
// 定义区域
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
// 声明：BOOL WINAPI Clip_OpenClipboard(HWND hWndNewOwner)
// 参数：
// 	IN hWndNewOwner -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：打开剪切板。
// 引用: 
// **************************************************
BOOL WINAPI Clip_OpenClipboard(HWND hWndNewOwner)
{
	UINT i;

	if (ptrClipDataList == NULL)
	{  // 剪切板的数据列表尚未初始化
		ptrClipDataList = (LPPTRLIST)malloc(sizeof(PTRLIST));  // 分配内存
		if (ptrClipDataList == NULL) 
			return FALSE;  // 分配失败
		PtrListCreate(ptrClipDataList,(ccIndex)g_iMaxItemNum,(ccIndex)16);  // 创建指针列表
		for (i = 0; i< g_iMaxItemNum; i++)
			PtrListInsert(ptrClipDataList, NULL);  // 插入内容为空的指针
	}
	if (hWndNewOwner == NULL)  // 窗口句柄错误
		return FALSE;
	if (hOwner != NULL)  // 当前剪切板还有人在使用
		return FALSE;
	hOwner = hWndNewOwner;  // 设置打开窗口句柄
	return TRUE;
}
// **************************************************
// 声明：BOOL WINAPI Clip_CloseClipboard(void)
// 参数：
//	无
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：关闭剪切板。
// 引用: 
// **************************************************
BOOL WINAPI Clip_CloseClipboard(void)
{
	hOwner = NULL;  // 设置打开窗口为空
	return TRUE;
}
// **************************************************
// 声明：HANDLE WINAPI Clip_SetClipboardData(UINT uFormat, HANDLE hMem )
// 参数：
// 	IN uFormat -- 数据格式
// 	IN hMem -- 数据句柄，该句柄有 GlobalAlloc 分配
// 
// 返回值：成功返回设置的数据句柄，否则返回NULL
// 功能描述：设置指定格式的数据。
// 引用: 
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

	item = PtrListAt( ptrClipDataList,uFormat);  // 得到原来数据
	if (item)  // 数据已经存在
//		free(item);
		GlobalFree(item);  // 释放原来的数据
//	PtrListAtPut( ptrClipDataList,uFormat, hNewData );
//	return hNewData;
	PtrListAtPut( ptrClipDataList,uFormat, hMem );  // 设置指定的数据
	return hMem;
}
// **************************************************
// 声明：HANDLE WINAPI Clip_GetClipboardData(UINT uFormat )
// 参数：
// 	IN uFormat -- 指定的数据格式
// 
// 返回值：返回指定格式的数据
// 功能描述：得到指定数据的格式。
// 引用: 
// **************************************************
HANDLE WINAPI Clip_GetClipboardData(UINT uFormat )
{
	return PtrListAt( ptrClipDataList,uFormat);
}
// **************************************************
// 声明：BOOL WINAPI Clip_EmptyClipboard(VOID)
// 参数：
// 	无
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：清空剪切板数据。
// 引用: 
// **************************************************
BOOL WINAPI Clip_EmptyClipboard(VOID)
{
	void *item;
	UINT i;

		for (i = 0; i< g_iMaxItemNum; i++)
		{
			item = PtrListAt( ptrClipDataList,i);  // 得到指定格式的数据
			if (item)
//				free(item);
				GlobalFree(item);  // 释放原来的数据
//			PtrListInsert(ptrClipDataList, NULL);
			PtrListAtPut( ptrClipDataList,i, NULL);  // 设置指定格式的数据为NULL
		}
		return TRUE;
}

// **************************************************
// 声明：BOOL WINAPI Clip_IsClipboardFormatAvailable(UINT uFormat)
// 参数：
// 	IN uFormat -- 指定格式
// 
// 返回值：数据有效返回TRUE，否则返回FALSE
// 功能描述：判断指定格式的数据是否有效。
// 引用: 
// **************************************************
BOOL WINAPI Clip_IsClipboardFormatAvailable(UINT uFormat)
{
	LPVOID lpData;
	
		if (ptrClipDataList == NULL)  // 没有剪切板数据
			return FALSE; // 返回无效
		lpData = PtrListAt( ptrClipDataList,uFormat);  // 得到指定格式数据
		if (lpData) // 数据存在
			return TRUE; // 返回数据有效
		else 
			return FALSE; // 返回数据无效
}
