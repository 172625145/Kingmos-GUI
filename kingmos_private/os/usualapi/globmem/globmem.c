/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：全局内存分配
版本号：1.0.0
开发时期：2003-05-23
作者：陈建明 Jami chen
修改记录：
**************************************************/

#include <ewindows.h>
#include <eglobmem.h>
#include <usualsrv.h>

/***************  全局区 定义， 声明 *****************/

typedef struct{
	UINT uFlags;
	DWORD dwBytes;
	LPTSTR lpMem;
}GLOBALMEMDATA, *LPGLOBALMEMDATA;

// **************************************************
// 声明：HGLOBAL WINAPI Gbl_GlobalAlloc(  UINT uFlags,  DWORD dwBytes )
// 参数：
// 	IN uFlags -- 要分配的内存标志
// 	IN dwBytes -- 要分配的内存字符数
// 
// 返回值：返回内存句柄
// 功能描述：分配一块全局内存。
// 引用: 
// **************************************************
#define DEBUG_Gbl_GlobalAlloc 0
HGLOBAL WINAPI Gbl_GlobalAlloc(  UINT uFlags,  DWORD dwBytes )
{
	LPGLOBALMEMDATA lpGlobalMem;
	
	lpGlobalMem = (LPGLOBALMEMDATA)malloc(sizeof(GLOBALMEMDATA));  // 分配全局内存数据结构
	//
	
	if (lpGlobalMem == NULL)
		return NULL;  // 分配失败
	//lilin 2005-01-06,必须使用共享内存区
//	lpGlobalMem->lpMem = (LPTSTR)malloc(dwBytes);  // 分配指定字符数的内存
	lpGlobalMem->lpMem = (LPTSTR)Page_AllocMem( dwBytes, NULL, PM_SHARE );  // 分配指定字符数的内存
	DEBUGMSG( DEBUG_Gbl_GlobalAlloc, ( "Gbl_GlobalAlloc: lpMem= 0x%x.\r\n", lpGlobalMem->lpMem ) );
	//
	if (lpGlobalMem->lpMem == NULL)
	{  // 分配失败
		free(lpGlobalMem);
		return NULL;
	}
	lpGlobalMem->uFlags = uFlags;  // 设置内存标志
	lpGlobalMem->dwBytes = dwBytes;  // 设置分配字符数
	return lpGlobalMem;  // 返回句柄
}

// **************************************************
// 声明：HGLOBAL WINAPI Gbl_GlobalDiscard(  HGLOBAL hglbMem  )
// 参数：
// 	IN hglbMem -- 全局内存指针 
// 
// 返回值：无
// 功能描述：保留功能。
// 引用: 
// **************************************************
HGLOBAL WINAPI Gbl_GlobalDiscard(  HGLOBAL hglbMem  )
{
	return NULL;
}

// **************************************************
// 声明：UINT WINAPI Gbl_GlobalFlags(  HGLOBAL hMem   )
// 参数：
// 	IN hMem -- 内存句柄
// 
// 返回值：返回内存的标志
// 功能描述：得到指定全局内存的标志。
// 引用: 
// **************************************************
UINT WINAPI Gbl_GlobalFlags(  HGLOBAL hMem   )
{
	LPGLOBALMEMDATA lpGlobalMem;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;
		if (lpGlobalMem == NULL)  // 错误句柄
			return 0;
		return lpGlobalMem->uFlags;  // 返回内存的标志
}

// **************************************************
// 声明：HGLOBAL WINAPI Gbl_GlobalFree(HGLOBAL hMem )
// 参数：
// 	IN hMem -- 内存句柄
// 
// 返回值：返回无效句柄
// 功能描述：释放全局内存
// 引用: 
// **************************************************
HGLOBAL WINAPI Gbl_GlobalFree(HGLOBAL hMem )
{
	LPGLOBALMEMDATA lpGlobalMem;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;  // 得到全局内存数据结构
		if (lpGlobalMem == NULL)  // 句柄无效
			return NULL;
		if (lpGlobalMem->lpMem)  // 释放内存
		{
			//lilin -2005-01-06必须使用共享内存区
			//free(lpGlobalMem->lpMem);
			Page_FreeMem( lpGlobalMem->lpMem, lpGlobalMem->dwBytes );
			//
		}
		free(lpGlobalMem); // 释放数据结构
		return NULL; // 返回
}

// **************************************************
// 声明：HGLOBAL WINAPI Gbl_GlobalHandle(  LPCVOID pMem  )
// 参数：
// 	IN pMem -- 指定内存
// 
// 返回值： 返回内存句柄
// 功能描述：保留功能
// 引用: 
// **************************************************
HGLOBAL WINAPI Gbl_GlobalHandle(  LPCVOID pMem  )
{
	return NULL;
}

// **************************************************
// 声明：LPVOID WINAPI Gbl_GlobalLock(  HGLOBAL hMem   )
// 参数：
// 	IN hMem -- 内存句柄
// 
// 返回值： 返回全局内存指针
// 功能描述：锁定全局内存。
// 引用: 
// **************************************************
#define DEBUG_Gbl_GlobalLock 0
LPVOID WINAPI Gbl_GlobalLock(  HGLOBAL hMem   )
{
	LPGLOBALMEMDATA lpGlobalMem;
	LPVOID lpMapPtr;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;  // 得到全局内存数据结构
		if (lpGlobalMem == NULL)  // 句柄无效
			return NULL;
		lpMapPtr = MapPtrToProcess( (LPVOID)lpGlobalMem->lpMem, GetCurrentProcess() );   // 映射指针
		DEBUGMSG( DEBUG_Gbl_GlobalLock, ( "Gbl_GlobalLock: lpMem= 0x%x, MapPtr=0x%x.\r\n", lpGlobalMem->lpMem, lpMapPtr ) );
		return lpMapPtr;  // 返回已经映射的指针
}

// **************************************************
// 声明：HGLOBAL WINAPI Gbl_GlobalReAlloc(  HGLOBAL hMem,   DWORD dwBytes, UINT uFlags )
// 参数：
// 	IN hMem -- 已经存在的内存句柄
//	 IN dwBytes -- 要分配的字符数
//	 IN uFlags -- 新的内存的标志
// 
// 返回值：返回新分配的内存句柄
// 功能描述：重新分配一块全局内存。
// 引用: 
// **************************************************
HGLOBAL WINAPI Gbl_GlobalReAlloc(  HGLOBAL hMem,   DWORD dwBytes, UINT uFlags )
{
	LPGLOBALMEMDATA lpGlobalMem;
	LPTSTR lpNewMem;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;  // 得到原来的数据结构
		if (lpGlobalMem == NULL) // 句柄无效
			return NULL;
		{
			//2005-01-06 必须使用共享内存区
		   // lpNewMem = realloc(lpGlobalMem->lpMem,dwBytes);  // 重新分配一块内存
	    	lpNewMem = (LPTSTR)Page_AllocMem( dwBytes, NULL, PM_SHARE );  // 分配指定字符数的内存
	    	//
		}
		if (lpNewMem == NULL)  // 分配失败
			return NULL;
		//2005-01-06 必须使用共享内存区
		Page_FreeMem( lpGlobalMem->lpMem, lpGlobalMem->dwBytes );
		//
		lpGlobalMem->lpMem = lpNewMem;  // 设置新的内存
		lpGlobalMem->dwBytes = dwBytes;  // 设置新的字符个数
		lpGlobalMem->uFlags = uFlags;  // 设置新的内存标志

		return lpGlobalMem;  // 返回内存句柄
}

// **************************************************
// 声明：DWORD WINAPI Gbl_GlobalSize(  HGLOBAL hMem  )
// 参数：
// 	IN hMem -- 内存句柄
// 
// 返回值： 返回指定全局内存的内存大小
// 功能描述：得到指定全局内存的内存大小。
// 引用: 
// **************************************************
DWORD WINAPI Gbl_GlobalSize(  HGLOBAL hMem  )
{
	LPGLOBALMEMDATA lpGlobalMem;

		lpGlobalMem = (LPGLOBALMEMDATA)hMem;  // 得到全局内存数据结构
		if (lpGlobalMem == NULL)  // 句柄无效
			return 0;
		return lpGlobalMem->dwBytes;  // 返回内存大小
}

// **************************************************
// 声明：BOOL WINAPI Gbl_GlobalUnlock(  HGLOBAL hMem )
// 参数：
// 	IN hMem -- 全局内存句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：解除锁定全局内存。
// 引用: 
// **************************************************
BOOL WINAPI Gbl_GlobalUnlock(  HGLOBAL hMem )
{
	return TRUE;
}
