#include <ewindows.h>
#include <oemfunc.h>

int OEM_EnumExtensionDRAM( LPMEM_SEGMENT lpSegs, UINT uiMaxSegNum )
{
	return 0;
}

int OEM_PowerOff( DWORD dwReserve )
{
	extern int CPU_PowerOff( DWORD dwReserve );
	CPU_PowerOff(dwReserve);
	return 0;
}
