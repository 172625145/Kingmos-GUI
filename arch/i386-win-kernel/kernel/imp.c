#include <edef.h> //windows.h>
#include <ecore.h>
#include <eapisrv.h>
#include <eobjcall.h>
#include <stdarg.h>

LRESULT KL_ImplementCallBack4( LPCALLBACKDATA lpcd, ... )
{
	va_list         vl;
	UINT arg1, arg2, arg3;
	
	va_start(vl, lpcd);

	arg1 = va_arg(vl, int);
	arg2 = va_arg(vl, int);
	arg3 = va_arg(vl, int);

	printf( "KL_ImplementCallBack4.\r\n" );

	return lpcd->lpfn( lpcd->dwArg0, arg1, arg2, arg3 );	
}