#include <edef.h>
#include <ecore.h>
#include <sysintr.h>
//#include <ethread.h>
#include <oemfunc.h>
#include <eapisrv.h>

extern void InstallMouseDevice( void );
extern void InstallKeyboardDevice( void );
extern void xygRas_AddCOM( void );

//BOOL CALLBACK _MyInitDefaultDevice( void )
//{
//	int i=0;
//}
// call by system.c

BOOL OEM_InitDefaultDevice( void )
{
	extern BOOL LoadFatFileSystem( void );
    // to register your device here
	//OEM_InterruptEnable( SYSINTR_RESCHED, 0, 0 );
	//CreateThread( NULL, 0, _MyInitDefaultDevice, 0, 0, 0 ); 
	//CreateEvent( NULL, FALSE, 0, 0 );

    //InstallKeyboardDevice();
	//LoadFatFileSystem();

	//Load_ATA_Driver();


	//_LoadSRAMDISK();
	//_LoadMULTDISK();

	//
//	xygRas_AddCOM( );

    return TRUE;
}
