#include <eframe.h>
#include <gwmeobj.h>

extern DWORD CALLBACK GWDI_DisplayEnter( UINT msg, DWORD dwParam, LPVOID lParam );
extern DWORD CALLBACK GWDI_KeyEnter( UINT msg, DWORD dwParam, LPVOID lParam );
extern DWORD CALLBACK GWDI_MouseEnter( UINT msg, DWORD dwParam, LPVOID lParam );

BOOL _InitDefaultGwdi( void )
{
	lpGwdiDisplayEnter = GWDI_DisplayEnter;
	lpGwdiPosEnter = GWDI_MouseEnter;
	lpGwdiKeyEnter = GWDI_KeyEnter;
    return TRUE;
}

