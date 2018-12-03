#include <eframe.h>
#include <eapisrv.h>
#include "shellapi.h"

///////////////////////////////////////////////////
const PFNVOID lpShellAPI[] = 
{
	NULL,
    (PFNVOID)Shell_SHChangeNotify,
};

static const DWORD dwShellArgs[] = {
	0,	
	ARG4_MAKE( DWORD, DWORD, PTR, PTR ),//VOID Shell_SHChangeNotify(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2)
};



void InstallShellServer( void )
{
    RETAILMSG(1, ("InstallShellServer...\r\n") );
	API_RegisterEx( API_SHELL,  (const PFNVOID *)lpShellAPI, dwShellArgs, sizeof( lpShellAPI ) / sizeof(PFNVOID) );
    RETAILMSG(1, ("InstallShellServer  OK!!!\r\n") );
}

