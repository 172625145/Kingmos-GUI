/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：网络服务的管理 主文件
版本号：  1.0.0
开发时期：2004-05-09
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <eapisrv.h>


/***************  全局区 定义， 声明 *****************/
extern	BOOL	SMgr_Init( );
extern	void	SMgr_Deinit( );
extern	BOOL	SMgr_RegAPI( );

static	BOOL	API_WaitReady( UINT uiAPIId, DWORD dwWaitMS, LPCSTR pszInfo );

/******************************************************/


// ********************************************************************
//功能描述：Services 主函数入口
// ********************************************************************
#ifdef	INLINE_PROGRAM
LRESULT CALLBACK WinMain_NetSrv(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#else
LRESULT CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#endif
{
	HANDLE	hEvtNetSrv;

	if( !API_IsReady( API_TCPIP ) )
	{
		//注册API
		if( !SMgr_RegAPI( ) )
		{
			return 0;
		}
		//等待文件系统
		API_WaitReady( API_FILESYS, -1, "WinMain_NetSrv" );

		//初始化
		if( !SMgr_Init( ) )
		{
			return 0;
		}

		//开始工作
		hEvtNetSrv = CreateEvent( NULL, FALSE, FALSE, "Event_Net_Services" );
		API_SetReady( API_TCPIP );

		//开始监测任何事件
		while( 1 )
		{
			WaitForSingleObject( hEvtNetSrv, INFINITE );
			//break;
		}
		CloseHandle( hEvtNetSrv );

		//
		SMgr_Deinit( );
	}

	return 0;
}


BOOL	API_WaitReady( UINT uiAPIId, DWORD dwWaitSec, LPCSTR pszInfo )
{
	DWORD		dwTick;

	// 等待指定ID号的API是否已经完成
	dwTick = GetTickCount();
	while( 1 )
	{
		//
		if( API_IsReady( uiAPIId ) )
		{
			return TRUE;
		}
		RETAILMSG( 1, ( "(%s): wait api [%d] ready!\r\n", pszInfo, uiAPIId ) );
		//
		if( dwWaitSec!=-1 )
		{
			if( ((GetTickCount()-dwTick)/1000)>=dwWaitSec )
			{
				return FALSE;
			}
		}
		Sleep(100);
	}
	return FALSE;
}
