
#include <ewindows.h>
extern LRESULT CALLBACK WinMain_P1(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
extern LRESULT CALLBACK WinMain_P2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
extern LRESULT CALLBACK WinMain_P3(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
extern LRESULT CALLBACK WinMain_P4(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
extern LRESULT CALLBACK Demogif_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
extern LRESULT CALLBACK DemoFont_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
extern LRESULT CALLBACK Democontipic_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
extern LRESULT CALLBACK DemoOverLop_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
extern int WINAPI SMIExplore(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow);
extern int WINAPI WinMain_Mobinonte( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,int nCmdShow);
LRESULT CALLBACK WinMain_Test( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow );
void LoadAllDemoAp(HINSTANCE hInstance)
{
//	RETAILMSG(1, ("WinMain_Test start ..........\r\n"));
//	WinMain_Test(hInstance, NULL, NULL, 0);
	
//	RETAILMSG(1, ("WinMain_Test start ..........\r\n"));
	
//	WinMain_Test(hInstance, NULL, NULL, 0);
#if 0
	RETAILMSG(1, ("DemoFont_WinMain start ..........\r\n"));
	DemoFont_WinMain(hInstance, NULL, NULL, 0);
		
	RETAILMSG(1, ("WinMain_P1 start ..........\r\n"));
	WinMain_P1(hInstance, NULL, NULL, 0);
	
	RETAILMSG(1, ("WinMain_P2 start ..........\r\n"));
	WinMain_P2(hInstance, NULL, NULL, 0);
	
	RETAILMSG(1, ("WinMain_P3 start ..........\r\n"));
	WinMain_P3(hInstance, NULL, NULL, 0);
	
	RETAILMSG(1, ("WinMain_P4 start ..........\r\n"));
	WinMain_P4(hInstance, NULL, NULL, 0);
	
	RETAILMSG(1, ("Demogif_WinMain start ..........\r\n"));
	Demogif_WinMain(hInstance, NULL, NULL, 0);
		
	RETAILMSG(1, ("Democontipic_WinMain start ..........\r\n"));
	Democontipic_WinMain(hInstance, NULL, NULL, 0);
	
	RETAILMSG(1, ("DemoOverLop_WinMain start ..........\r\n"));
	DemoOverLop_WinMain(hInstance, NULL, NULL, 0);

	RETAILMSG(1, ("WinMain_Mobinonte start ..........\r\n"));
	WinMain_Mobinonte(hInstance, NULL, NULL, 0);

#endif
	
#if 0	
	RETAILMSG(1, ("SMIExplore start ..........\r\n"));	
	SMIExplore(hInstance, NULL, "/mnt/kingmos/htm/kingmos.htm", 0);
#endif	
	RETAILMSG(1, ("load finish ..........\r\n"));	
}

LRESULT CALLBACK WinMain_Pic(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	LoadAllDemoAp(hInstance);
	return 0;
}

