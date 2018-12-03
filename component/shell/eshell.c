/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����Ӧ�ó�����ǣ��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-03-18
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
#include <eapisrv.h>
//#include "desktop.h"
#include "shellapi.h"
#include "eshell.h"
#include "explorer.h"
#include "emmsys.h"

/***************  ȫ���� ���壬 ���� *****************/

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
// ������LRESULT CALLBACK  WinMain_Shell( HINSTANCE , HINSTANCE ,LPTSTR , int)
// ������
//	IN hInstance - Ӧ�ó���ʵ�����
//	IN hPrevInstance - ǰһ��Ӧ�ó���ʵ����� , ����
//    IN lpCmdLine   - ����Ӧ�ó���Ĳ���
//    IN nCmdShow  -  �Ƿ���Ҫ��ʾ
// ����ֵ��
// 	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// �����������õ������ֽڵĳ���
// ����: �� DoMainCode ����
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
	{ //�ж�ϵͳ�Ƿ��Ѿ�׼����
		RETAILMSG( 1, ( "wait filesys and gwe\r\n" ) );
		Sleep(200);
	}
*/
	//���ط���
	InstallServerProgram();  // ��װ�������
	LoadServerProgram(); // װ�ط������

	SetApplicationInfo(); // ����Ӧ�ó�����Ϣ

	// װ����֪��Ӧ�ó��򣬶�����ApMain\ApMain.c
#ifdef INLINE_PROGRAM	
	InstallApplication();
#endif	
/*	RETAILMSG(1, ("filetest load start \r\n"));
	LoadApplication("filetest",NULL);  // װ��filetest
	RETAILMSG(1, ("filetest loaded \r\n"));	*/
    // װ��ϵͳ�������
/*
	RETAILMSG(1, ("pic ap load start \r\n"));
	LoadApplication("pic",NULL);  // װ��pictest
	RETAILMSG(1, ("pic ap loaded \r\n"));*/
/*
	RETAILMSG(1, ("mobinonte load start \r\n"));
	LoadApplication("mobinonte",NULL);  // mobinonte
	RETAILMSG(1, ("mobinonte loaded \r\n"));*/
	
	// װ������	
	//LoadDeskTop();
	hShellEvent = CreateEvent( NULL, FALSE, FALSE, "Shell_event" );  // ����SHELL�¼�

	while( 1 )
	{
		int rv;
		rv = WaitForSingleObject( hShellEvent, INFINITE );  // �ȴ�SHELL�˳�
		break;
	}
	
	CloseHandle( hShellEvent );  // �ر��¼����

	return 0;

}

// ********************************************************************
// ������void LoadServerProgram(void)
// ������
//	��
// ����ֵ��
// 	��
// ����������ע��ϵͳ����
// ����: �� Shell ����
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
    RegisterApplication( "device",  Device_WinMain, NULL );  // ע���豸
#else
	RegisterApplication( "registry", WinMain_Registry, 0 ); // ע������
#endif
	RegisterApplication( "Desktop", WinMain_Desktop, 0 ); // ע������
	RegisterApplication( "UsualApi", WinMain_UsualApi,0 ); // ע��ͨ�ó���

	RegisterApplication( "gwme", Gwme_WinMain,0 ); // ע��GWME

	RegisterApplication( "keyboard", WinMain_KeybdApi,0 );
#endif   // INLINE_PROGRAM

}

// ********************************************************************
//������void WaitAPIReady( UINT uiAPIId )
//������
//	uiAPIId-����id 
//����ֵ��
//	�ɹ���TRUE�� ʧ�ܣ� FASE
//�����������ȴ�ϵͳ������سɹ�
//����: 
// ********************************************************************

BOOL WaitAPIReady( UINT uiAPIId )
{
	while( API_IsReady( uiAPIId ) == FALSE )  // �ȴ�ָ��ID�ŵ�API�Ƿ��Ѿ����
	{
		RETAILMSG( 1, ( "wait api[%d] ready.\r\n", uiAPIId ) );
		Sleep(100);
	}
	return TRUE;
}
// ********************************************************************
//������void LoadServerProgram(void)
//������
//	��
//����ֵ��
//	��
//����������װ��ϵͳ����
//����: �� Shell ����
// ********************************************************************
void LoadServerProgram(void)
{
//    MSG				msg;

#ifdef INLINE_PROGRAM

#ifdef KINGMOS_KERNEL
		//LoadApplication("device",NULL);  // װ���豸
		//WaitAPIReady( API_FILESYS ); // �ȴ��ļ�ϵͳ���
	       
#else
	   API_SetReady( API_FILESYS );
	   LoadApplication("registry",NULL);  // װ��GWME
	   WaitAPIReady( API_REGISTRY ); // �ȴ�GWE���
	   
#endif

		LoadApplication("gwme",NULL);  // װ��GWME
		WaitAPIReady( API_GWE ); // �ȴ�GWE���

		LoadApplication("UsualApi",NULL);  // װ��ͨ��API	

		// !!! Add By Jami chen in 2003.09.10
		InstallShellServer( );  // ��װSHELL����
		API_SetReady(API_SHELL); // �ȴ�SHELL���
		// !!! Add End By Jami chen in 2003.09.10

		LoadApplication("keyboard", 0);
		WaitAPIReady( API_KEYBD ); // �ȴ�GWE���

		LoadApplication("Desktop",NULL);  // ��������
#else	
		//added by zb...for midp...
		//�ý����÷���ģʽ
		CreateProcess("\\kingmos\\device.exe",NULL,0,0,0,0x40000000,0,0,0,0);  // �����豸
		WaitAPIReady( API_FILESYS ); // �ȴ��ļ�ϵͳ���
//		WaitAPIReady( API_AUDIO ); // �ȴ��������

		CreateProcess("\\kingmos\\gwme.exe",NULL,0,0,0,0x40000000,0,0,0,0);  // ����GWME
		WaitAPIReady( API_GWE ); // �ȴ�GWE���

		CreateProcess("\\kingmos\\UsualApi.exe",NULL,0,0,0,0,0,0,0,0); // ����ͨ��API


		// !!! Add By Jami chen in 2003.09.10
		InstallShellServer( );  // ��װSHELL����
		API_SetReady(API_SHELL); // �ȴ�SHELL���
		
		// !!! Add End By Jami chen in 2003.09.10

		CreateProcess("\\kingmos\\keyboard.exe", "",0,0,0,0,0,0,0,0);  // ���м��̳���
		WaitAPIReady( API_KEYBD ); // �ȴ�GWE���
		
		CreateProcess("\\kingmos\\Desktop.exe",NULL,0,0,0,0,0,0,0,0); // ��������
	
		
#endif
		
}

// **************************************************
// ������static void SetApplicationInfo()
// ������
// 	��
// ����ֵ��
//	  ��
// ��������������Ӧ�ó�����Ϣ
// ����: 
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
	SetSysColors(sizeof(ChangeList) / sizeof(int),ChangeList,colorList);  // ����ϵͳ��ɫ


//	RETAILMSG( 1, ( "Load ApplicationInfo .......................\r\n" ) );
	
	//LoadSystemStyle();  // װ��ϵͳ��ʾ���

//#ifndef EML_WIN32
//	waveOutSetVolume(0,0xffffffff);  // Set Sound To max
//#endif
	
	//LoadSysTimeFormat(); // װ�ص�ǰ��ϵͳʱ���ʽ

	//SetSystemMetrics(SM_CYCAPTION,26);  // ����CAPTION�ĸ߶�

	SetSystemMetrics(SM_CXVSCROLL,12);  // ���ô�ֱ�������ĸ߶ȺͿ��
	SetSystemMetrics(SM_CYVSCROLL,1);
	
	SetSystemMetrics(SM_CXHSCROLL,1);  // ����ˮƽ�������ĸ߶ȺͿ��
	SetSystemMetrics(SM_CYHSCROLL,12);

	SetSystemMetrics(SM_CYLISTITEM,26);  // ������ʽ���и�
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
// ������VOID WINAPI Shell_SHChangeNotify(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2)
// ������
// 	IN wEventId -- �¼����
// 	IN uFlags -- ��־
// 	IN dwItem1 -- ֪ͨ����1
// 	IN dwItem2 -- ֪ͨ����2
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
VOID WINAPI Shell_SHChangeNotify(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2)
{
	switch(wEventId)
	{
		case SHCNE_MKDIR:  // ����Ŀ¼
		case SHCNE_RMDIR: // ɾ��Ŀ¼
		case SHCNE_DRIVEREMOVED: // �豸�Ƴ�
		case SHCNE_DRIVEADD: // �豸���
		case SHCNE_UPDATEDIR: // ˢ��Ŀ¼
			DoDirChanged();  // Ŀ¼�ı�
			break;
	}
}
// **************************************************
// ������static void DoDirChanged(void)
// ������
// 	��
// ����ֵ�� ��
// ��������������Ŀ¼�ı�
// ����: 
// **************************************************
static void DoDirChanged(void)
{
//	HWND hFileBrowser;

//		hFileBrowser = FindWindow(classFileBrowser,NULL);
//		if (hFileBrowser)
//		{
//			PostMessage(hFileBrowser,EXM_REFRESH,0,0);
//		}
		PostMessage(HWND_BROADCAST,WM_SHELLNOTIFY,SN_REFRESH,0);  // �㲥��Ϣ֪ͨ����Ӧ�ó���ˢ���ļ�
}

