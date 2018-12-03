//typedef void * HKEY;
#include <windows.h>
//int InterruptEnable = 0;

BOOL __TRY( void * lp, int len )
{
	register int v;
//	register DWORD * p;
	register int * lpadr = (int*)lp;

	//Interlock_Exchange( &iTryData, 1 );
	//p = lpCurThread->lpdwTLS+TLS_TRY;	
	//*p = 1;

	len >>= ( sizeof( int ) >> 1 );  // align 4bytes

	v = 0;
	__try{
		while( len && *lpadr )
		{
			v += *lpadr++;
			len--;
		}
	}__except( 1 ){
		return FALSE;
	}
	//v = *p;
	//*p = 0;
	return TRUE;
}

