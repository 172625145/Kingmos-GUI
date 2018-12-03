/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：应用程序外壳，用户界面部分
版本号：1.0.0
开发时期：2003-03-18
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <ewindows.h>
#include <eapisrv.h>
//#include "desktop.h"
#include "shellapi.h"
#include "eshell.h"
#include "explorer.h"
#include "emmsys.h"

/***************  全局区 定义， 声明 *****************/

HANDLE hShellEvent; 

/******************************************************/

extern void InstallApplication( void );
extern void InstallShellServer( void );

static void InstallServerProgram( void );

static void LoadDeskTop( void );
static void LoadServerProgram( void );
static BOOL WaitAPIReady( UINT uiAPIId );

static void SetApplicationInfo();

// ********************************************************************
// 声明：LRESULT CALLBACK  WinMain_Shell( HINSTANCE , HINSTANCE ,LPTSTR , int)
// 参数：
//	IN hInstance - 应用程序实例句柄
//	IN hPrevInstance - 前一个应用程序实例句柄 , 保留
//    IN lpCmdLine   - 调用应用程序的参数
//    IN nCmdShow  -  是否需要显示
// 返回值：
// 	成功，返回TRUE;失败，返回FALSE 
// 功能描述：得到解码字节的长度
// 引用: 被 DoMainCode 调用
// ********************************************************************
#ifdef INLINE_PROGRAM
LRESULT CALLBACK Shell_WinMain( HINSTANCE hInstance,
		            HINSTANCE hPrevInstance,
		            LPTSTR    lpCmdLine,
		            int       nCmdShow )
#else
LRESULT CALLBACK WinMain( HINSTANCE hInstance,
		            HINSTANCE hPrevInstance,
		            LPTSTR    lpCmdLine,
		            int       nCmdShow )
#endif
{
	RETAILMSG( 1, ( "Start Shell ...\r\n" ) );
/*
	while( API_IsReady( API_FILESYS ) == FALSE )
	{ //判断系统是否已经准备好
		RETAILMSG( 1, ( "wait filesys and gwe\r\n" ) );
		Sleep(200);
	}
*/
	//加载服务
	InstallServerProgram();  // 安装服务程序
	LoadServerProgram(); // 装载服务程序

	SetApplicationInfo(); // 设置应用程序信息

	// 装载已知的应用程序，定义在ApMain\ApMain.c
#ifdef INLINE_PROGRAM	
	InstallApplication();
#endif	
/*	RETAILMSG(1, ("filetest load start \r\n"));
	LoadApplication("filetest",NULL);  // 装载filetest
	RETAILMSG(1, ("filetest loaded \r\n"));	*/
    // 装载系统服务程序
/*
	RETAILMSG(1, ("pic ap load start \r\n"));
	LoadApplication("pic",NULL);  // 装载pictest
	RETAILMSG(1, ("pic ap loaded \r\n"));*/
/*
	RETAILMSG(1, ("mobinonte load start \r\n"));
	LoadApplication("mobinonte",NULL);  // mobinonte
	RETAILMSG(1, ("mobinonte loaded \r\n"));*/
	
	// 装载桌面	
	//LoadDeskTop();
	hShellEvent = CreateEvent( NULL, FALSE, FALSE, "Shell_event" );  // 创建SHELL事件

	while( 1 )
	{
		int rv;
		rv = WaitForSingleObject( hShellEvent, INFINITE );  // 等待SHELL退出
		break;
	}
	
	CloseHandle( hShellEvent );  // 关闭事件句柄

	return 0;

}

// ********************************************************************
// 声明：void LoadServerProgram(void)
// 参数：
//	无
// 返回值：
// 	无
// 功能描述：注册系统服务
// 引用: 被 Shell 调用
// ********************************************************************
void InstallServerProgram( void )
{
#ifdef INLINE_PROGRAM
    extern int CALLBACK Device_WinMain(
							 HINSTANCE hInstance,
							 HINSTANCE hPrevInstance,
							 LPSTR     lpCmdLine,
							 int       nCmdShow );


    extern LRESULT CALLBACK WinMain_Desktop(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);

	extern int CALLBACK Gwme_WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);
    extern int WINAPI WinMain_UsualApi(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow);
    extern int WINAPI WinMain_Registry(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow);


	extern LRESULT CALLBACK WinMain_KeybdApi( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow );
#ifdef KINGMOS_KERNEL
    RegisterApplication( "device",  Device_WinMain, NULL );  // 注册设备
#else
	RegisterApplication( "registry", WinMain_Registry, 0 ); // 注册桌面
#endif
	RegisterApplication( "Desktop", WinMain_Desktop, 0 ); // 注册桌面
	RegisterApplication( "UsualApi", WinMain_UsualApi,0 ); // 注册通用程序

	RegisterApplication( "gwme", Gwme_WinMain,0 ); // 注册GWME

	RegisterApplication( "keyboard", WinMain_KeybdApi,0 );
#endif   // INLINE_PROGRAM

}

// ********************************************************************
//声明：void WaitAPIReady( UINT uiAPIId )
//参数：
//	uiAPIId-服务id 
//返回值：
//	成功：TRUE； 失败： FASE
//功能描述：等待系统服务加载成功
//引用: 
// ********************************************************************

BOOL WaitAPIReady( UINT uiAPIId )
{
	while( API_IsReady( uiAPIId ) == FALSE )  // 等待指定ID号的API是否已经完成
	{
		RETAILMSG( 1, ( "wait api[%d] ready.\r\n", uiAPIId ) );
		Sleep(100);
	}
	return TRUE;
}
// ********************************************************************
//声明：void LoadServerProgram(void)
//参数：
//	无
//返回值：
//	无
//功能描述：装载系统服务
//引用: 被 Shell 调用
// ********************************************************************
void LoadServerProgram(void)
{
//    MSG				msg;

#ifdef INLINE_PROGRAM

#ifdef KINGMOS_KERNEL
		//LoadApplication("device",NULL);  // 装载设备
		//WaitAPIReady( API_FILESYS ); // 等待文件系统完成
	       
#else
	   API_SetReady( API_FILESYS );
	   LoadApplication("registry",NULL);  // 装载GWME
	   WaitAPIReady( API_REGISTRY ); // 等待GWE完成
	   
#endif

		LoadApplication("gwme",NULL);  // 装载GWME
		WaitAPIReady( API_GWE ); // 等待GWE完成

		LoadApplication("UsualApi",NULL);  // 装载通用API	

		// !!! Add By Jami chen in 2003.09.10
		InstallShellServer( );  // 安装SHELL服务
		API_SetReady(API_SHELL); // 等待SHELL完成
		// !!! Add End By Jami chen in 2003.09.10

		LoadApplication("keyboard", 0);
		WaitAPIReady( API_KEYBD ); // 等待GWE完成

		LoadApplication("Desktop",NULL);  // 运行桌面
#else	
		//added by zb...for midp...
		//该进程用服务模式
		CreateProcess("\\kingmos\\device.exe",NULL,0,0,0,0x40000000,0,0,0,0);  // 运行设备
		WaitAPIReady( API_FILESYS ); // 等待文件系统完成
//		WaitAPIReady( API_AUDIO ); // 等待声音完成

		CreateProcess("\\kingmos\\gwme.exe",NULL,0,0,0,0x40000000,0,0,0,0);  // 运行GWME
		WaitAPIReady( API_GWE ); // 等待GWE完成

		CreateProcess("\\kingmos\\UsualApi.exe",NULL,0,0,0,0,0,0,0,0); // 运行通用API


		// !!! Add By Jami chen in 2003.09.10
		InstallShellServer( );  // 安装SHELL服务
		API_SetReady(API_SHELL); // 等待SHELL完成
		
		// !!! Add End By Jami chen in 2003.09.10

		CreateProcess("\\kingmos\\keyboard.exe", "",0,0,0,0,0,0,0,0);  // 运行键盘程序
		WaitAPIReady( API_KEYBD ); // 等待GWE完成
		
		CreateProcess("\\kingmos\\Desktop.exe",NULL,0,0,0,0,0,0,0,0); // 运行桌面
	
		
#endif
		
}

// **************************************************
// 声明：static void SetApplicationInfo()
// 参数：
// 	无
// 返回值：
//	  无
// 功能描述：设置应用程序信息
// 引用: 
// **************************************************
static void SetApplicationInfo()
{
	int ChangeList[] =
	{
		COLOR_HIGHLIGHTTEXT,
		COLOR_HIGHLIGHT,
		COLOR_BTNHILIGHT,
		COLOR_3DDKSHADOW,
		COLOR_3DLIGHT,
		COLOR_BTNSHADOW,
		COLOR_CAPTIONTEXT

	};
	COLORREF colorList[] =
	{
		RGB(255,255,255),
		RGB(77,166,255),
		RGB(210, 210, 210),
		RGB(64, 64, 64 ),
		RGB(255, 255, 255),
		RGB(128, 128, 128),
		RGB(0xff, 0xff, 0xff)
	};
	SetSysColors(sizeof(ChangeList) / sizeof(int),ChangeList,colorList);  // 设置系统颜色


//	RETAILMSG( 1, ( "Load ApplicationInfo .......................\r\n" ) );
	
	//LoadSystemStyle();  // 装载系统显示风格

//#ifndef EML_WIN32
//	waveOutSetVolume(0,0xffffffff);  // Set Sound To max
//#endif
	
	//LoadSysTimeFormat(); // 装载当前的系统时间格式

	//SetSystemMetrics(SM_CYCAPTION,26);  // 设置CAPTION的高度

	SetSystemMetrics(SM_CXVSCROLL,12);  // 设置垂直滚动条的高度和宽度
	SetSystemMetrics(SM_CYVSCROLL,1);
	
	SetSystemMetrics(SM_CXHSCROLL,1);  // 设置水平滚动条的高度和宽度
	SetSystemMetrics(SM_CYHSCROLL,12);

	SetSystemMetrics(SM_CYLISTITEM,26);  // 设置列式的行高
	SetSystemMetrics( SM_CYCAPTION, 26 );
	SetSystemMetrics( SM_CXDLGFRAME, 3 );
	SetSystemMetrics( SM_CYDLGFRAME, 3 );
	SetSystemMetrics( SM_CXFRAME, 3 );
	SetSystemMetrics( SM_CYFRAME, 3 );
	
	SetSystemMetrics( SM_CYSIZE, 25 );
	SetSystemMetrics( SM_CXSIZE, 23 );
	

}



/*******************************************************************************/
/*******************************************************************************/
// SHChangeNotify
/*******************************************************************************/
static void DoDirChanged(void);

// **************************************************
// 声明：VOID WINAPI Shell_SHChangeNotify(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2)
// 参数：
// 	IN wEventId -- 事件标号
// 	IN uFlags -- 标志
// 	IN dwItem1 -- 通知参数1
// 	IN dwItem2 -- 通知参数2
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
VOID WINAPI Shell_SHChangeNotify(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2)
{
	switch(wEventId)
	{
		case SHCNE_MKDIR:  // 创建目录
		case SHCNE_RMDIR: // 删除目录
		case SHCNE_DRIVEREMOVED: // 设备移除
		case SHCNE_DRIVEADD: // 设备添加
		case SHCNE_UPDATEDIR: // 刷新目录
			DoDirChanged();  // 目录改变
			break;
	}
}
// **************************************************
// 声明：static void DoDirChanged(void)
// 参数：
// 	无
// 返回值： 无
// 功能描述：处理目录改变
// 引用: 
// **************************************************
static void DoDirChanged(void)
{
//	HWND hFileBrowser;

//		hFileBrowser = FindWindow(classFileBrowser,NULL);
//		if (hFileBrowser)
//		{
//			PostMessage(hFileBrowser,EXM_REFRESH,0,0);
//		}
		PostMessage(HWND_BROADCAST,WM_SHELLNOTIFY,SN_REFRESH,0);  // 广播消息通知所有应用程序刷新文件
}

