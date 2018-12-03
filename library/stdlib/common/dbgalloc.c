#include <edef.h>
#include <eframe.h>
#include <ealloc.h>
#include <estring.h>
#include <estdlib.h>
#include <eassert.h>

#define ALIGN_MASK   7

typedef struct _DBGMEMINFO
{
	LPBYTE lpbFileName;
	int line;
	DWORD dwThreadId;
	DWORD dwProcessId;
	LPVOID lpvRet;
}DBGMEMINFO;

void Debug_Mem_Free( void * p, char * file, int line );

#define DBGMEMSIZE ( ( sizeof( DBGMEMINFO ) + ALIGN_MASK ) & (~ALIGN_MASK) )

void * Debug_Mem_Alloc( DWORD dwSize, char * file, int line )
{
	//char buf[16];
	//int iFileLen, iLineLen, iPreFixLen;
	char * p;


	if( dwSize )
	{
		//itoa( line, buf, 10 );
		//iLineLen = strlen( buf );
		//iFileLen = strlen( file );	
		
		//iPreFixLen = iFileLen + sizeof( char ) + iLineLen + sizeof( char ) + sizeof(DWORD) + sizeof(DWORD);
		//iPreFixLen = (iPreFixLen + ALIGN_MASK) & (~ALIGN_MASK);  // align to dword
		//iPreFixLen = DBGMEMSIZE;//( sizeof( _DBGMEMINFO ) + ALIGN_MASK ) & (~ALIGN_MASK);
		
		p = (char*)Mem_Alloc( dwSize + DBGMEMSIZE );
		if( p )
		{
			DBGMEMINFO * lpmi = (DBGMEMINFO *)p;
			//strcpy( p, file );
			//strcat( p, "@" );
			//strcat( p, buf );
			
			//*( (DWORD*)(p + iPreFixLen - sizeof( DWORD) ) ) = iPreFixLen;
			//*( (DWORD*)(p + iPreFixLen - sizeof( DWORD) - sizeof(DWORD) ) ) = GetCurrentProcessId();
			lpmi->line = line;
			lpmi->lpbFileName = (LPBYTE)file;
			lpmi->dwProcessId = GetCurrentProcessId();
			lpmi->dwThreadId = GetCurrentThreadId();
			lpmi->lpvRet = ( p + DBGMEMSIZE );

			return lpmi->lpvRet;//(p + iPreFixLen);
		}
	}
	return NULL;
}

void * Debug_Mem_Realloc( void * p, DWORD dwSize, char * file, int line )
{
	//_HeapCheck();

	if( p )
	{
		DBGMEMINFO * lpmi = (DBGMEMINFO *)( (LPBYTE)p - DBGMEMSIZE );
		//DWORD iPreFixLen = *( (DWORD*)( (char*)p - sizeof(DWORD) ) );
		//DWORD dwProcessId = *( (DWORD*)( (char*)p - sizeof(DWORD) - sizeof(DWORD) ) );
		
		//ASSERT( GetCurrentProcessId() == dwProcessId );
		ASSERT( lpmi->lpvRet == p );

		//_HeapCheck();

		if( dwSize )
		{
		    //char * pr = (char*)Mem_Realloc( (char*)p - iPreFixLen, iPreFixLen+dwSize );
			char * pr = (char*)Mem_Realloc( (char*)lpmi, DBGMEMSIZE + dwSize );
			//_HeapCheck();
			if( pr )
			{
				lpmi = (DBGMEMINFO *)pr;
				lpmi->lpvRet = pr + DBGMEMSIZE;
				return lpmi->lpvRet;//pr + iPreFixLen;
			}
			else
				return NULL;
		}
		else
		{
			//_HeapCheck();
			Debug_Mem_Free( p, file, line );
			//_HeapCheck();
			return NULL;
		}
	}
	else
	{
		if( dwSize )
		{
			//_HeapCheck();
		    return Debug_Mem_Alloc( dwSize, file, line );
		}
		else
			return NULL;
	}
}

void Debug_Mem_Free( void * p, char * file, int line )
{
	if( p )
	{
		DBGMEMINFO * lpmi = (DBGMEMINFO *)( (LPBYTE)p - DBGMEMSIZE );

		//DWORD * pdw = (DWORD*)((char*)p - sizeof(DWORD));
		//DWORD iPreFixLen = *pdw;
		//char * pd = (char*)p - iPreFixLen;

		//DWORD dwProcessId = *( (DWORD*)( (char*)p - sizeof(DWORD) - sizeof(DWORD) ) );
		
		//ASSERT( GetCurrentProcessId() == dwProcessId );

		//ASSERT( (lpmi->lpvRet == p) && (lpmi->dwProcessId == GetCurrentProcessId()) );
		DEBUGMSG( !( (lpmi->lpvRet == p) && (lpmi->dwProcessId == GetCurrentProcessId()) ), ( "lpmi->lpvRet=%x,p=%x,lpmi->ProcessId=%x,CurProcId=%x", lpmi->lpvRet, p, lpmi->dwProcessId, GetCurrentProcessId() ) );

		lpmi->lpvRet = NULL;
		

		Mem_Free( lpmi );
	}
}


//BOOL Debug_Mem_Enum( LPMEMENUM lpfn, UINT uiFlag )
//{
//}
static BOOL CALLBACK _CheckThread( LPVOID lpv, LPARAM lParam )
{
	DBGMEMINFO * lpmi = (DBGMEMINFO *)lpv;

	//ASSERT( lpmi->lpvRet == lpv );
	if( lpmi->dwThreadId == (DWORD)lParam )
	{
		RETAILMSG( 1, ( "find the thread alloc mem:file=%s,line=%d.\r\n", lpmi->lpbFileName, lpmi->line ) );
	}
	return TRUE;
}

static BOOL CALLBACK _CheckProcess( LPVOID lpv, LPARAM lParam )
{
	DBGMEMINFO * lpmi = (DBGMEMINFO *)lpv;

	ASSERT( lpmi->dwProcessId == (DWORD)lParam );
	RETAILMSG( 1, ( "the process mem is not free:file=%s,line=%d, ProcesId=%x,ThreadId=%x.\r\n", lpmi->lpbFileName, lpmi->line, lpmi->dwProcessId, lpmi->dwThreadId ) );
	return TRUE;
}


void _CheckThreadAllocMem( void )
{
	;//Mem_Enum( _CheckThread, GetCurrentThreadId(), 0 );
}

void _CheckProcessAllocMem( void )
{
	;//Mem_Enum( _CheckProcess, GetCurrentProcessId(), 0 );
}

