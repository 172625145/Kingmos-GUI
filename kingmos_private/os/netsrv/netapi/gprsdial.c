/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：GPRS Config API
版本号：  1.0.0
开发时期：2004-08-24
作者：    肖远钢
修改记录：
******************************************************/

//root include
#include <ewindows.h>
//"\inc_app"
#include <ras.h>
#include <gprscfg.h>


/***************  全局区 定义， 声明 *****************/
/******************************************************/


DWORD	GprsDial( DWORD dwNetworkType, HWND hWndRas, OUT HANDLE* phRasConn )
{
	DWORD		dwErr;
	DIALPARAM	stDialParam;

	//
	stDialParam.dwSize = sizeof(DIALPARAM);
	stDialParam.szEntryName[0] = 0;
	strcpy( stDialParam.szPhoneNumber, "*99#" );
	stDialParam.szUserName[0] = 0;
	stDialParam.szPassword[0] = 0;
	stDialParam.szDomain[0] = 0;

	//请添加进程互斥操作--begin

	//
	GprsCfg_SetCurNetType( dwNetworkType );
	//
	dwErr = RasDial( &stDialParam, DIALNOTIFY_WND, (LPVOID)hWndRas, phRasConn );

	//请添加进程互斥操作--end

	return dwErr;
}


BOOL	GprsCheckDial( OUT DWORD* pdwNetworkType )
{
	//1 是否有GPRS拨号上网
	if( !RasIsDevExist( TRUE, RASDT_SUB_GPRS, RASDT_CHKSUB ) )
	{
		return FALSE;
	}
	//2 得到GPRS拨号的网络类型
	GprsCfg_GetCurNetType( pdwNetworkType );
	return TRUE;
}






