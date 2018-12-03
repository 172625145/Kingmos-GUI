//typedef void * HKEY;
#include <windows.h>
#include "sysmem.h"
#include <emlos.h>

DWORD * lpdwFirstPTE = 0;
DWORD * lpdwSecondPTE = 0;
DWORD * lpdwSecondPhyPTE = 0;


DWORD GetAttribFromProtect( DWORD dwProtect )
{
    //KL_SetLastError( ERROR_INVALID_PARAMETER );
    return 0;
}

DWORD GetProtectFromAttrib( DWORD dwPageAttrib )
{
	//KL_SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}

DWORD GetPhyPageFlagMask( void )
{
	return PGF_MASK;
}

DWORD GetPhyPageAdrMask( void )
{
	return PG_PHYS_ADDR_MASK;
}

VOID FreeCPUPTS( VOID * lpPTS )
{
	//ASSERT( (DWORD)lpPTS == 0xffff0000 );
}

VOID * AllocCPUPTS( VOID )
{
	return (VOID *)0xffff0000;
}
