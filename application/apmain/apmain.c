#include <ewindows.h>
#include <Eassert.h>
#include <eapisrv.h>
#include <eucore.h>

int WINAPI WinMain_Backup(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow);
int WINAPI WinMain_test(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);

int WINAPI WinMain_DemoAlpha(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);

int WINAPI WinMain_DemoNormal(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);

int WINAPI WinMain_DemoRgnWindow(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);
LRESULT CALLBACK WinMain_DemoFont( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow );
LRESULT CALLBACK WinMain_Bounce( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow );

LRESULT CALLBACK WinMain_Stex3d( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow );
LRESULT CALLBACK WinMain_Gears( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow );

LRESULT CALLBACK WinMain_DemoLayer( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow );
LRESULT CALLBACK WinMain_DemoGPS( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow );
int WINAPI WinMain_GradFill(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);
void InstallApplication( void )
{	
    //RegisterApplication( "backup", WinMain_Backup ,0 );
//	RegisterApplication( "test", WinMain_test ,0 );
	RegisterApplication( "demo_alpha", WinMain_DemoAlpha ,0 );
	RegisterApplication( "demo_normal", WinMain_DemoNormal ,0 );
	RegisterApplication( "demo_rgnwin", WinMain_DemoRgnWindow ,0 );
	RegisterApplication( "demo_font1", WinMain_DemoFont ,0 );
//	RegisterApplication( "demo_openglBounce", WinMain_Bounce ,0 );
//	RegisterApplication( "demo_Stex3d", WinMain_Stex3d ,0 );
	RegisterApplication( "demo_Gears", WinMain_Gears ,0 );
	RegisterApplication( "demo_Layered", WinMain_DemoLayer ,0 );
	RegisterApplication( "demo_gps", WinMain_DemoGPS ,0 );
	RegisterApplication( "demo_gradfill", WinMain_GradFill ,0 );
	

}
