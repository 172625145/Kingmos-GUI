#include <edef.h>
#include <eframe.h>
#include <ealloc.h>
#include <estring.h>
#include <estdlib.h>
#include <eassert.h>

////////////////////////

VOID Debug_CriticalSection_Enter( LPCRITICAL_SECTION lpCriticalSection, char * lpFile, int iFileLine )
{
	lpCriticalSection->lpFileName = lpFile;
	lpCriticalSection->iFileLine = iFileLine;
#ifdef ARM_CPU
	lpCriticalSection->dwTickCount = GetTickCount();//OEM_GetSysTickCount();
	 //GetTickCount();
#else
	lpCriticalSection->dwTickCount = GetTickCount();
#endif
	CriticalSection_Enter( lpCriticalSection );
}
//////////////////////
