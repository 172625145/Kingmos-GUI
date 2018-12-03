/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：程序管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
     LN:2003.05.07, 修改MODULE 结构
******************************************************/

#include <eframe.h>
#include <eprogram.h>
#include <efile.h>

#include <epalloc.h>
#include <epheap.h>
#include <eprocess.h>
#include <epcore.h>
#include <coresrv.h>


// ********************************************************************
//声明： PMODULE Module_Init( 
//								LPPROCESS lpOwner, 
//								LPCTSTR lpszApplicationName,
//								PCMODULE_DRV lpmd,
//								UINT uiCreateFlag )
//参数：
//	IN lpOwner - 进程结构指针，指向拥有者
//	IN lpszApplicationName - 程序名
//	IN lpmd - MODULE_DRV结构指针，模块驱动接口
//	IN uiCreateFlag - 创建标志
//返回值：
//	假如成功，返回MODULE结构指针；否则，返回NULL
//功能描述：
//	分配并初始化MODULE结构指针
//引用：
//	LoadApplication & CreateProcess 用它去初始化模块
// ********************************************************************
PMODULE Module_Init( LPPROCESS lpOwner, LPCTSTR lpszApplicationName, PCMODULE_DRV lpmd, UINT uiCreateFlag )
{
	PMODULE pModule = KHeap_Alloc( sizeof( MODULE ) );
	if( pModule )
	{	//分配一个句柄	
		pModule->hModule = Handle_Alloc( lpOwner, pModule, OBJ_MODULE );
		if( pModule->hModule )
		{
			//初始化模块
			KC_InitializeCriticalSection( &pModule->csModule );
			pModule->lpmd = lpmd;
			pModule->hModuleData = lpmd->lpCreateModule( lpszApplicationName, lpOwner->hProcess, uiCreateFlag );
			if( pModule->hModuleData )
				return pModule;
		}
		//失败，清除...
		if( pModule->hModule )
			Handle_Free( pModule->hModule, TRUE );
		KHeap_Free( pModule, sizeof( MODULE ) );
	}
	return NULL;
}

// ********************************************************************
//声明：void Module_DeInit( PMODULE lpModule ) 
//参数：
//	IN lpModule - MODULE结构指针，为之前用Module_Init返回的
//返回值：
//	无
//功能描述：
//	与Module_Init对应，该功能释放模块
//引用：
// ********************************************************************
void Module_DeInit( PMODULE lpModule )
{	
	lpModule->lpmd->lpFreeModule( lpModule->hModuleData );
	Handle_Free( lpModule->hModule, TRUE );
	KL_DeleteCriticalSection( &lpModule->csModule );
	KHeap_Free( lpModule, sizeof( MODULE ) );
}

// ********************************************************************
//声明：FARPROC WINAPI KL_GetProcAddress( HMODULE hModule, LPCTSTR lpProcName ) 
//参数：
//	IN hModule - 模块句柄
//	IN lpProcName - 函数名
//返回值：
//	假如成功，返回模块的函数调用地址；否则，返回NULL
//功能描述：
//	得到模块的函数调用地址
//引用：
// ********************************************************************
FARPROC WINAPI KL_GetProcAddress( HMODULE hModule, LPCTSTR lpProcName )
{
	PMODULE pModule = HandleToPtr( hModule, OBJ_MODULE );
	if( pModule )
	{	// 有效
		return (FARPROC)pModule->lpmd->lpGetProcAddress( pModule->hModuleData,  lpProcName );
	}
	else
        KL_SetLastError( ERROR_INVALID_PARAMETER );
	return NULL;
}

// ********************************************************************
//声明：DWORD WINAPI KL_GetModuleFileName( 
//										HMODULE hModule,
//										LPTSTR lpFileName, 
//										DWORD nSize )
//参数：
//	IN hModule - 模块句柄
//  OUT lpFileName - 字符串指针，用于接受hModule模块的文件名
//  IN nSize - lpFileName指向的内存长度
//返回值：
//	返回拷贝到lpFileName的字符数
//功能描述：
//	得到模块文件名
//引用：
//	系统API
// ********************************************************************
DWORD WINAPI KL_GetModuleFileName( 
							   HMODULE hModule,
							   LPTSTR lpFileName, 
							   DWORD nSize )
{
	PMODULE pModule;
	if( hModule )
	{
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	else
	{	//用当前进程模块
		pModule = GetAPICallerProcessPtr()->pModule;
	}

	if( pModule )
	{
		LPCTSTR lpszfn = pModule->lpmd->lpGetModuleFileName( pModule->hModuleData );
		if( lpszfn )
		{
			strncpy( lpFileName, lpszfn, nSize );
			return strlen( lpFileName );
		}
	}
	else
        KL_SetLastError( ERROR_INVALID_PARAMETER );

	return 0;
}

// ********************************************************************
//声明： HMODULE WINAPI KL_LoadLibraryEx( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags )
//参数：
//	IN lpFileName – 需要加载的模块文件名
//	IN hFile – 文件句柄（保留，必须为NULL）
//	IN dwFlags – 保留，必须为0
//返回值：
//	假如成功，返回有效的句柄；否则，返回NULL
//功能描述：
//	加载一个模块到当前进程空间
//引用：
//	系统API
// ********************************************************************
HMODULE WINAPI KL_LoadLibraryEx( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags )
{
#ifdef INLINE_PROGRAM
	return NULL;
#else
	extern const MODULE_DRV ElfModule;
	extern const MODULE_DRV qemd;
    extern DWORD WINAPI KL_GetFileAttributes( LPCTSTR lpcszFileName );
	extern BOOL IsQEModuleExist( LPCTSTR lpcszApName );
	BOOL bIsELF = FALSE;	
/*

	if( hFile == NULL )
	{
		if( IsQEModuleExist( lpFileName ) == FALSE )
		{   // qe-module is not existing,  elf format
			bIsELF = TRUE;
		}

	}
	else
*/
		bIsELF = TRUE;
	if( bIsELF )
	{	// elf文件格式
		DWORD dwInitFlag = -1;// 先设为无效
		int len = strlen( lpFileName );
		//是 .exe 文件 或是 .so文件
		if( len >= 5 && stricmp( &lpFileName[len-4], ".exe" ) == 0 )
			dwInitFlag = CREATE_EXE; // .exe 文件
		else if( len >= 4 && stricmp( &lpFileName[len-3], ".so" ) == 0 )
		    dwInitFlag = CREATE_LIBRARY; // .so 文件
		else
		{	// 是否文件名有别的类型
			while( len )
			{
				if( lpFileName[len-1] == '.' )
					break;
				len--;
			}
			// 如果没有，默认该文件为 .so文件类型
			if( len == 0 )
			{  // no ext, default is .so
				dwInitFlag = CREATE_LIBRARY;
			}
		}
		// dwInitFlag 标志是否有效
		if( dwInitFlag != -1 )
		{
			PMODULE pModule = Module_Init( GetAPICallerProcessPtr(), lpFileName, &ElfModule, dwInitFlag ); 
			if( pModule )
			{
				if( dwInitFlag == CREATE_EXE )
				{	//将可执行模块装入内存
				    ElfModule.lpLoadModule( pModule->hModuleData, pModule->hModule, NULL, LF_LOAD_TO_MEM );
				}
				else //将动态连接库装入当前进程空间
					ElfModule.lpLoadModule( pModule->hModuleData, pModule->hModule, NULL, LF_DEFLOAD );

				return pModule->hModule;
			}
		}
		else
	        KL_SetLastError( ERROR_INVALID_PARAMETER );

	}
#endif 
	return NULL;

}

// ********************************************************************
//声明： BOOL WINAPI KL_FreeLibrary( HMODULE hModule )
//参数：
//	IN hModule - 模块句柄
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	与KL_LoadLibraryEx对应，该功能释放模块
//引用：
//	系统API
// ********************************************************************
BOOL WINAPI KL_FreeLibrary( HMODULE hModule )
{
	PMODULE pModule = NULL;
	// 句柄是否有效
	if( hModule )
	{
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	if( pModule )
	{
		Module_DeInit( pModule ); // 释放模块
		return TRUE;
	}
	else
		KL_SetLastError( ERROR_INVALID_PARAMETER );
	return FALSE;
}

// ********************************************************************
//声明： HMODULE WINAPI KL_GetModuleHandle( LPCTSTR lpModuleName )
//参数：
//	IN lpModuleName - 模块名（必须为NULL，这是就等于返回当前进程的模块句柄）
//返回值：
//	模块句柄
//功能描述：
//	得到已经被加载到进程空间的模块
//引用：
// ********************************************************************
#define DEBUG_GET_MODULE_HANDLE 0
HMODULE WINAPI KL_GetModuleHandle( LPCTSTR lpModuleName )
{
	DEBUGMSG( DEBUG_GET_MODULE_HANDLE, ( "KL_GetModuleHandle=0x%x.\r\n", lpModuleName ? NULL : GetAPICallerProcessPtr()->pModule->hModule ) );
	if( lpModuleName )
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return NULL;
	}
	else
		return GetAPICallerProcessPtr()->pModule->hModule;
}

// ********************************************************************
//声明： HANDLE WINAPI KL_GetModuleFileHandle( HMODULE hModule )
//参数：
//	IN hModule - 模块句柄
//返回值：
//	文件句柄
//功能描述：
//	得到模块所打开的文件句柄
//引用：
//	系统API
// ********************************************************************
#define DEBUG_GET_MODULE_FILE_HANDLE 0
HANDLE WINAPI KL_GetModuleFileHandle( HMODULE hModule )
{
	PMODULE pModule;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	// 检查合法性
	if( hModule )
	{   // 有效的模块
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	else
	{   // 当前进程模块
		pModule = GetAPICallerProcessPtr()->pModule;
	}

	if( pModule )
	{   // 返回文件句柄
		hFile = pModule->lpmd->lpGetModuleFileHandle( pModule->hModuleData );
		if( hFile == NULL )
		{
			hFile = INVALID_HANDLE_VALUE;
		}
		else
		{   // 锁住句柄
			KL_EnterCriticalSection( &pModule->csModule );
		}
	}
	else
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_GET_MODULE_FILE_HANDLE, ( "error at KL_GetModuleFileHandle invalid hModule(0x%x).\r\n", hModule ) );
	}
	DEBUGMSG( DEBUG_GET_MODULE_FILE_HANDLE, ( "KL_GetModuleFileHandle return (0x%x).\r\n", hFile ) );

	return hFile;
}

// ********************************************************************
//声明： BOOL WINAPI KL_ReleaseModuleFileHandle( HMODULE hModule, HANDLE hFile )
//参数：
//	IN hModule - 模块句柄
//	IN hFile - 文件句柄
//返回值：
//	成功，返回TRUE; 失败，返回FALSE
//功能描述：
//	释放该模块的句柄(解锁)
//引用：
//	系统API
// ********************************************************************
#define DEBUG_RELEASE_MODULE_FILE_HANDLE 0
BOOL WINAPI KL_ReleaseModuleFileHandle( HMODULE hModule, HANDLE hFile )
{
	BOOL bRetv = FALSE;
	PMODULE pModule;
	
	// 检查合法性
	if( hModule )
	{   // 有效的模块
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	else
	{   // 当前进程模块
		pModule = GetAPICallerProcessPtr()->pModule;
	}

	if( pModule )
	{   // 返回文件句柄
		HANDLE hf;
		hf = pModule->lpmd->lpGetModuleFileHandle( pModule->hModuleData );
		if( hf == hFile )
		{	// 释放句柄
			KL_LeaveCriticalSection( &pModule->csModule );
			bRetv = TRUE;
		}
	}

	if( bRetv == FALSE )
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG( DEBUG_RELEASE_MODULE_FILE_HANDLE, ( "error at KL_ReleaseModuleFileHandle invalid hModule(0x%x),hFile(0x%x).\r\n", hModule, hFile ) );
	}

	return bRetv;
}

// ********************************************************************
//声明： DWORD WINAPI KL_GetModuleSectionOffset( HMODULE hModule, LPCTSTR lpcszSectionName )
//参数：
//	IN hModule - 模块句柄
//  IN lpcszSectionName - 段名
//返回值：
//	假如成功，返回模块的段在模块文件里的偏移；否则，返回-1
//功能描述：
//	得到模块的段在模块文件里的偏移，这里段是指模块文件里数据组织形态
//引用：
//	系统API
// ********************************************************************
DWORD WINAPI KL_GetModuleSectionOffset( HMODULE hModule, LPCTSTR lpcszSectionName )
{
	PMODULE pModule;

	// 检查合法性

	if( hModule )
	{   // 有效的模块
	    pModule = HandleToPtr( hModule, OBJ_MODULE );
	}
	else
	{   // 当前进程模块
		pModule = GetAPICallerProcessPtr()->pModule;
	}

	if( pModule )
	{
		return pModule->lpmd->lpGetModuleSectionOffset( pModule->hModuleData, lpcszSectionName );
	}
	else
		KL_SetLastError( ERROR_INVALID_PARAMETER );
	return -1;
}

// ********************************************************************
//声明：VOID WINAPI KL_FreeLibraryAndExitThread( HMODULE hModule, DWORD dwExitCode ) 
//参数：
//	IN hModule - 模块句柄
//	IN dwExitCode - 退出代码
//返回值：
//	
//功能描述：
//	
//引用：
// ********************************************************************
VOID WINAPI KL_FreeLibraryAndExitThread( HMODULE hModule, DWORD dwExitCode )
{
	KL_ExitThread(0);
}

// ********************************************************************
//声明： 
//参数：
//	IN
//返回值：
//	
//功能描述：
//	
//引用：
// ********************************************************************
BOOL WINAPI KL_DisableThreadLibraryCalls( HMODULE hModule )
{
	return FALSE;
}


////////////////////////////////////////////
