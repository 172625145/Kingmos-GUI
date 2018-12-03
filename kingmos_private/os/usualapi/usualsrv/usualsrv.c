#include <eframe.h>
#include <eapisrv.h>
#include <eglobmem.h>
#include <eclipbrd.h>
#include <usualsrv.h>


///////////////////////////////////////////////////
const PFNVOID lpUsualAPI[] = 
{
	NULL,
    (PFNVOID)Gbl_GlobalAlloc,
    (PFNVOID)Gbl_GlobalDiscard,
    (PFNVOID)Gbl_GlobalFlags,
    (PFNVOID)Gbl_GlobalFree,
    (PFNVOID)Gbl_GlobalHandle,
    (PFNVOID)Gbl_GlobalLock,
    (PFNVOID)Gbl_GlobalReAlloc,
    (PFNVOID)Gbl_GlobalSize,
    (PFNVOID)Gbl_GlobalUnlock,

    (PFNVOID)Clip_OpenClipboard,
    (PFNVOID)Clip_CloseClipboard,
    (PFNVOID)Clip_SetClipboardData,
    (PFNVOID)Clip_GetClipboardData,
    (PFNVOID)Clip_EmptyClipboard,
	(PFNVOID)Clip_IsClipboardFormatAvailable,
};

static const DWORD dwUsualArgs[] = {
	0,	
//   Global Memory
    ARG2_MAKE( DWORD, DWORD ),//Gbl_GlobalAlloc(  UINT uFlags,  DWORD dwBytes );
    ARG1_MAKE( DWORD ),//Gbl_GlobalDiscard(  HGLOBAL hglbMem  );
    ARG1_MAKE( DWORD ),//Gbl_GlobalFlags(  HGLOBAL hMem   );
    ARG1_MAKE( DWORD ),//Gbl_GlobalFree(HGLOBAL hMem );
    ARG1_MAKE( DWORD ),//Gbl_GlobalHandle(  LPCVOID pMem  );
    ARG1_MAKE( DWORD ),//Gbl_GlobalLock(  HGLOBAL hMem   );
    ARG3_MAKE( DWORD, DWORD, DWORD ),//Gbl_GlobalReAlloc(  HGLOBAL hMem,   DWORD dwBytes, UINT uFlags );
    ARG1_MAKE( DWORD ),//Gbl_GlobalSize(  HGLOBAL hMem  );
    ARG1_MAKE( DWORD ),//Gbl_GlobalUnlock(  HGLOBAL hMem );

    ARG1_MAKE( DWORD ),//Clip_OpenClipboard(HWND hWndNewOwner);
    ARG0_MAKE( ),//Clip_CloseClipboard(void);
    ARG2_MAKE( DWORD, DWORD ),//Clip_SetClipboardData(UINT uFormat, HANDLE hMem );
    ARG1_MAKE( DWORD ),//Clip_GetClipboardData(UINT uFormat );
    ARG0_MAKE( ),//Clip_EmptyClipboard(VOID);
    ARG1_MAKE( DWORD ),//Clip_IsClipboardFormatAvailable(UINT uFormat );
};

////////////////////////////


void InstallUsualServer( void )
{
    RETAILMSG(1, ("InstallUsualServer...\r\n") );
//	API_Register( API_AUDIO,  (PFNVOID)lpAudioAPI, sizeof( lpAudioAPI ) / sizeof(PFNVOID) );
	API_RegisterEx( API_USUAL,  (const PFNVOID *)lpUsualAPI, dwUsualArgs, sizeof( lpUsualAPI ) / sizeof(PFNVOID) );
    RETAILMSG(1, ("InstallUsualServer  OK!!!\r\n") );
	API_SetReady( API_USUAL );
}

HANDLE hUsualEvent;

#ifdef INLINE_PROGRAM
int WINAPI WinMain_UsualApi(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
//	HANDLE hThd;

    RETAILMSG(1, ("Init Usual Function\r\n") );

	if( API_IsReady( API_USUAL ) )
	{
		RETAILMSG( 1, ( "the UsualApi has already load.exit!\r\n" ) );
		return -1;
	}

	InstallUsualServer();

	hUsualEvent = CreateEvent( NULL, FALSE, FALSE, "Usual_event" );

	while( 1 )
	{
		int rv;
		rv = WaitForSingleObject( hUsualEvent, INFINITE );
		break;
	}
	
	CloseHandle( hUsualEvent );
	
	return 0;
}

