/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：注册表 OEM 输入输出
版本号：1.0.0
开发时期：2004-07-27
作者：李林
修改记录：
******************************************************/

#include <ewindows.h>
#include <emlos.h>
#include <oemfunc.h>

//将注册表保存到或恢复从OEM设备
//假如你想支持该功能，按以下步骤：
//1.实现一个功能函数，如  BOOL OEM_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
//2.对全局变量付值        extern LPOEM_REGISTRYIO lpOEM_RegistryIO=OEM_RegistryIO;

// ********************************************************************
//声明：BOOL OEM_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
//参数：
//	IN/OUT lpDataBuf - 用于保存或写入的数据
//  IN nNumberOfBytes - lpDataBuf 的数据量
//	IN dwFlag - 以下类型的组合:
//                   REGIO_BEGIN - 第一次操作
//                   REGIO_READ -  读
//                   REGIO_WRITE - 写
//                   REGIO_END -   结束操作
//返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
//功能描述：
//	输入输出注册表
//引用:
//	系统API
// ********************************************************************

BOOL OEM_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
{
	static HANDLE hFile = INVALID_HANDLE_VALUE;
	//是否开始IO ?
	if( dwFlag & REGIO_BEGIN )
	{	//是
		hFile = Win32_CreateFile( "kmsreg.dat", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL );
	}

	if( hFile && hFile != INVALID_HANDLE_VALUE )
	{
		DWORD dwRW;
		if( dwFlag & REGIO_WRITE )
			Win32_WriteFile( hFile, lpDataBuf, nNumberOfBytes, &dwRW, NULL );
		else if( dwFlag & REGIO_READ )
			Win32_ReadFile( hFile, lpDataBuf, nNumberOfBytes, &dwRW, NULL );			
	}
	//是否结束IO ?
	if( dwFlag & REGIO_END )
	{	//是
		 Win32_CloseHandle( hFile ); 
		 hFile = INVALID_HANDLE_VALUE;
	}
	return TRUE;
}

extern LPOEM_REGISTRYIO lpOEM_RegistryIO=OEM_RegistryIO;


