/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <etable.h>
#include <eutable.h>
#include <eapisrv.h>

#include <epcore.h>


//typedef HTABLE (WINAPI * PTB_CREATE_EX)( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, UINT nFieldNum, DWORD dwTableFileSize );
typedef HTABLE (WINAPI * PTB_CREATE_BY_STRUCT)( LPCREATE_TABLE lpct );
HTABLE WINAPI Tb_CreateByStruct( LPCREATE_TABLE lpct )
{
#ifdef CALL_TRAP 
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_CREATE, 1 );
    cs.arg0 = (DWORD)lpct;
    return (HTABLE)CALL_SERVER( &cs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	HTABLE  retv = INVALID_HANDLE_VALUE;

	if( API_Enter( API_TABLE, TB_CREATE, &lpfn, &cs ) )
	{
		lpct = MapProcessPtr( lpct, (LPPROCESS)cs.lpvData );

		retv = ((PTB_CREATE_BY_STRUCT)lpfn)( lpct );
		API_Leave(  );
	}
	return retv;
#endif
}

HTABLE WINAPI Tb_CreateEx( LPCTSTR lpcszFileName, 
						 DWORD dwAccess, 
						 DWORD dwShareMode, 
						 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
						 DWORD dwCreate, 
						 UINT nFieldNum,
						 DWORD dwTableFileSize )
{
	CREATE_TABLE ct;
	memset( &ct, 0, sizeof( ct ) );
	ct.dwSize = sizeof(ct);
	ct.lpcszFileName = lpcszFileName;
	ct.dwAccess = dwAccess;
	ct.dwShareMode = dwShareMode;
	ct.lpSecurityAttributes = lpSecurityAttributes;
	ct.dwCreate = dwCreate;
	ct.nFieldNum = nFieldNum;
	ct.dwFileSizeLimit = dwTableFileSize;
	return Tb_CreateByStruct( &ct );
}

HTABLE WINAPI Tb_Create( LPCTSTR lpcszFileName, 
						 DWORD dwAccess, 
						 DWORD dwShareMode, 
						 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
						 DWORD dwCreate, 
						 UINT nFieldNum )
{
	return Tb_CreateEx( lpcszFileName, dwAccess, dwShareMode, lpSecurityAttributes, dwCreate, nFieldNum, 0 );
}

typedef BOOL ( WINAPI * PTB_DELETE )( LPCTSTR lpcszFileName );
BOOL WINAPI Tb_Delete( LPCTSTR lpcszFileName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_DELETE, 1 );
    cs.arg0 = (DWORD)lpcszFileName ;
    return (DWORD)CALL_SERVER( &cs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_TABLE, TB_DELETE, &lpfn, &cs ) )
	{
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );

		retv = ((PTB_DELETE)lpfn)( lpcszFileName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_CLOSE )( HTABLE hTable );
BOOL WINAPI Tb_Close( HTABLE hTable )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_CLOSE, 1 );
    cs.arg0 = (DWORD)hTable ;
    return (DWORD)CALL_SERVER( &cs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_TABLE, TB_CLOSE, &lpfn, &cs ) )
	{
		retv = ((PTB_CLOSE)lpfn)( hTable );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef WORD ( WINAPI * PTB_SETRECORDPOINTER )( HTABLE hTable, long lOffset, UINT fMethod );
WORD WINAPI Tb_SetRecordPointer( HTABLE hTable, long lOffset, UINT fMethod )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_SETRECORDPOINTER, 3 );
    cs.arg0 = (DWORD)hTable;
    return (WORD)CALL_SERVER( &cs, lOffset, fMethod );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	WORD  retv = 0xffff;

	if( API_Enter( API_TABLE, TB_SETRECORDPOINTER, &lpfn, &cs ) )
	{
		retv = ((PTB_SETRECORDPOINTER)lpfn)( hTable, lOffset, fMethod );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef WORD ( WINAPI * PTB_NEXTRECORDPOINTER )( HTABLE hTable, BOOL bNext );

WORD WINAPI Tb_NextRecordPointer( HTABLE hTable, BOOL bNext )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_NEXTRECORDPOINTER, 2 );
    cs.arg0 = (DWORD)hTable;
    return (WORD)CALL_SERVER( &cs, bNext );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	WORD  retv = 0xffff;

	if( API_Enter( API_TABLE, TB_NEXTRECORDPOINTER, &lpfn, &cs ) )
	{
		retv = ((PTB_NEXTRECORDPOINTER)lpfn)( hTable, bNext );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef WORD ( WINAPI * PTB_READFIELD )( HTABLE hTable, UINT nField, WORD wStartPos, void * lpBuf, WORD wSizeLimit );
WORD WINAPI Tb_ReadField( HTABLE hTable, 
						  UINT nField, 
						  WORD wStartPos,
						  void * lpBuf,
						  WORD wSizeLimit )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_READFIELD, 5 );
    cs.arg0 = (DWORD)hTable;
    return (WORD)CALL_SERVER( &cs, nField, wStartPos, lpBuf, wSizeLimit );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	WORD  retv = 0;

	if( API_Enter( API_TABLE, TB_READFIELD, &lpfn, &cs ) )
	{
		lpBuf = MapProcessPtr( lpBuf, (LPPROCESS)cs.lpvData );

		retv = ((PTB_READFIELD)lpfn)( hTable, nField, wStartPos, lpBuf, wSizeLimit );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef WORD ( WINAPI * PTB_WRITEFIELD )( HTABLE hTable, UINT nField, const void * lpcvBuf, WORD wSize );
WORD WINAPI Tb_WriteField( HTABLE hTable, UINT nField, const void * lpcvBuf, WORD wSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_WRITEFIELD, 4 );
    cs.arg0 = (DWORD)hTable;
    return (WORD)CALL_SERVER( &cs, nField, lpcvBuf, wSize );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	WORD  retv = 0;

	if( API_Enter( API_TABLE, TB_WRITEFIELD, &lpfn, &cs ) )
	{
		lpcvBuf = MapProcessPtr( lpcvBuf, (LPPROCESS)cs.lpvData );

		retv = ((PTB_WRITEFIELD)lpfn)( hTable, nField, lpcvBuf, wSize );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef long ( WINAPI * PTB_WRITERECORD )( HTABLE hTable, FIELDITEM fieldItem[] );
long WINAPI Tb_WriteRecord( HTABLE hTable, FIELDITEM fieldItem[] )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_WRITERECORD, 2 );
    cs.arg0 = (DWORD)hTable;
    return (DWORD)CALL_SERVER( &cs, fieldItem );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	long  retv = 0;

	if( API_Enter( API_TABLE, TB_WRITERECORD, &lpfn, &cs ) )
	{
		fieldItem = MapProcessPtr( fieldItem, (LPPROCESS)cs.lpvData );

		retv = ((PTB_WRITERECORD)lpfn)( hTable, fieldItem );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_DELETERECORD )( HTABLE hTable, UINT uiRecord, DWORD dwFlag );
BOOL WINAPI Tb_DeleteRecord( HTABLE hTable, UINT uiRecord, DWORD dwFlag )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_DELETERECORD, 3 );
    cs.arg0 = (DWORD)hTable;
    return (DWORD)CALL_SERVER( &cs, uiRecord, dwFlag );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_DELETERECORD, &lpfn, &cs ) )
	{
		retv = ((PTB_DELETERECORD)lpfn)( hTable, uiRecord, dwFlag );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_SORTRECORD )( HTABLE hTable, WORD field, FIELDCOMPPROC lpfcProc );
BOOL WINAPI Tb_SortRecord( HTABLE hTable, WORD field, FIELDCOMPPROC lpfcProc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_SORTRECORD, 3 );
    cs.arg0 = (DWORD)hTable;
    return (DWORD)CALL_SERVER( &cs, field, lpfcProc );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_SORTRECORD, &lpfn, &cs ) )
	{
		retv = ((PTB_SORTRECORD)lpfn)( hTable, field, lpfcProc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_APPENDRECORD )( HTABLE hTable, FIELDITEM fieldItem[] );
BOOL WINAPI Tb_AppendRecord( HTABLE hTable, FIELDITEM fieldItem[] )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_APPENDRECORD, 2 );
    cs.arg0 = (DWORD)hTable;
    return (DWORD)CALL_SERVER( &cs, fieldItem );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_APPENDRECORD, &lpfn, &cs ) )
	{
		fieldItem = MapProcessPtr( fieldItem, (LPPROCESS)cs.lpvData );

		retv = ((PTB_APPENDRECORD)lpfn)( hTable, fieldItem );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_READRECORD )( 
					HTABLE hTable,
                    void * lpvData,
                    FIELDITEM fieldItem[],
                    DWORD * lpdwSize );

BOOL WINAPI Tb_ReadRecord( 
					HTABLE hTable,
                    void * lpvData,
                    FIELDITEM fieldItem[],
                    DWORD * lpdwSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_READRECORD, 4 );
    cs.arg0 = (DWORD)hTable;
    return (DWORD)CALL_SERVER( &cs, lpvData, fieldItem, lpdwSize );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_READRECORD, &lpfn, &cs ) )
	{
		lpvData = MapProcessPtr( lpvData, (LPPROCESS)cs.lpvData );
		fieldItem = MapProcessPtr( fieldItem, (LPPROCESS)cs.lpvData );
		lpdwSize = MapProcessPtr( lpdwSize, (LPPROCESS)cs.lpvData );

		retv = ((PTB_READRECORD)lpfn)( hTable, lpvData, fieldItem, lpdwSize );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_INSERTRECORD )( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] );
BOOL WINAPI Tb_InsertRecord( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_INSERTRECORD, 3 );
    cs.arg0 = (DWORD)hTable;
    return (DWORD)CALL_SERVER( &cs, nPos, fieldItem );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_INSERTRECORD, &lpfn, &cs ) )
	{
		fieldItem = MapProcessPtr( fieldItem, (LPPROCESS)cs.lpvData );

		retv = ((PTB_INSERTRECORD)lpfn)( hTable, nPos, fieldItem );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PTB_COUNTRECORD )( HTABLE hTable );
int WINAPI Tb_CountRecord( HTABLE hTable )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_COUNTRECORD, 1 );
    cs.arg0 = (DWORD)hTable ;
    return (DWORD)CALL_SERVER( &cs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	int retv = 0;

	if( API_Enter( API_TABLE, TB_COUNTRECORD, &lpfn, &cs ) )
	{
		retv = ((PTB_COUNTRECORD)lpfn)( hTable );
		API_Leave(  );
	}
	return retv;
#endif
}
///////////
typedef BOOL ( WINAPI * PTB_GETRECORDINFO )( HTABLE hTable, LPRECORDINFO lpri );
BOOL WINAPI Tb_GetRecordInfo( HTABLE hTable, LPRECORDINFO lpri )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_GETRECORDINFO, 2 );
    cs.arg0 = (DWORD)hTable;
    return (DWORD)CALL_SERVER( &cs, lpri );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_GETRECORDINFO, &lpfn, &cs ) )
	{
		lpri = MapProcessPtr( lpri, (LPPROCESS)cs.lpvData );

		retv = ((PTB_GETRECORDINFO)lpfn)( hTable, lpri );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_GETTABLEINFO )( LPCTSTR lpcszFileName, LPTABLEINFO lpti );
BOOL WINAPI Tb_GetTableInfo( LPCTSTR lpcszFileName, LPTABLEINFO lpti )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_GETTABLEINFO, 2 );
    cs.arg0 = (DWORD)lpcszFileName;
    return (DWORD)CALL_SERVER( &cs, lpti );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_GETTABLEINFO, &lpfn, &cs ) )
	{
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );
		lpti = MapProcessPtr( lpti, (LPPROCESS)cs.lpvData );

		retv = ((PTB_GETTABLEINFO)lpfn)( lpcszFileName, lpti );
		API_Leave(  );
	}
	return retv;
#endif

}

typedef DWORD ( WINAPI * PTB_GETRECORDSIZE )( HTABLE hTable );
DWORD WINAPI Tb_GetRecordSize( HTABLE hTable )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_GETRECORDSIZE, 1 );
    cs.arg0 = (DWORD)hTable ;
    return (DWORD)CALL_SERVER( &cs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = 0;

	if( API_Enter( API_TABLE, TB_GETRECORDSIZE, &lpfn, &cs ) )
	{
		retv = ((PTB_GETRECORDSIZE)lpfn)( hTable );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HANDLE ( WINAPI * PTB_FINDFIRST )( HTABLE hTable, UINT uField, const void * lpcvFind, UINT uLen, LPFINDSTRUCT lpfs );
HANDLE WINAPI Tb_FindFirst( HTABLE hTable, 
						    UINT uField, 
							const void * lpcvFind, 
							UINT uLen, 
							LPFINDSTRUCT lpfs )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_FINDFIRST, 5 );
    cs.arg0 = (DWORD)hTable;
    return (HANDLE)CALL_SERVER( &cs, uField, lpcvFind, uLen, lpfs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	HANDLE retv = INVALID_HANDLE_VALUE;

	if( API_Enter( API_TABLE, TB_FINDFIRST, &lpfn, &cs ) )
	{
		lpcvFind = MapProcessPtr( lpcvFind, (LPPROCESS)cs.lpvData );
		lpfs = MapProcessPtr( lpfs, (LPPROCESS)cs.lpvData );

		retv = ((PTB_FINDFIRST)lpfn)( hTable, uField, lpcvFind, uLen, lpfs );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_FINDNEXT )( HANDLE hFind, LPFINDSTRUCT lpfs );
BOOL WINAPI Tb_FindNext( HANDLE hFind, LPFINDSTRUCT lpfs )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_FINDNEXT, 2 );
    cs.arg0 = (DWORD)hFind;
    return (DWORD)CALL_SERVER( &cs, lpfs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_FINDNEXT, &lpfn, &cs ) )
	{
		lpfs = MapProcessPtr( lpfs, (LPPROCESS)cs.lpvData );

		retv = ((PTB_FINDNEXT)lpfn)( hFind, lpfs );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_CLOSEFIND )( HANDLE hFind );
BOOL WINAPI Tb_CloseFind( HANDLE hFind )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_CLOSEFIND, 1 );
    cs.arg0 = (DWORD)hFind ;
    return (DWORD)CALL_SERVER( &cs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_CLOSEFIND, &lpfn, &cs ) )
	{
		retv = ((PTB_CLOSEFIND)lpfn)( hFind );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_PACK )( LPCTSTR lpcszFileName, UINT uiFlag );
BOOL WINAPI Tb_Pack( LPCTSTR lpcszFileName, UINT uiFlag )
{
#ifdef CALL_TRAP
	CALLTRAP cs;
	
	cs.apiInfo = CALL_API( API_TABLE, TB_PACK, 2 );
	cs.arg0 = (DWORD)lpcszFileName;
	return (DWORD)CALL_SERVER( &cs, uiFlag );
#else
	
	FARPROC lpfn;
	
	CALLSTACK cs;
	BOOL retv = FALSE;
	
	if( API_Enter( API_TABLE, TB_PACK, &lpfn, &cs ) )
	{
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );
		
		retv = ((PTB_PACK)lpfn)( lpcszFileName, uiFlag );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_PACKBYHANDLE )( HANDLE hTable, UINT uiFlag );
BOOL WINAPI Tb_PackByHandle( HANDLE hTable, UINT uiFlag )
{
#ifdef CALL_TRAP
	CALLTRAP cs;
	cs.apiInfo = CALL_API( API_TABLE, TB_PACKBYHANDLE, 2 );
	cs.arg0 = (DWORD)hTable;
	return (DWORD)CALL_SERVER( &cs, uiFlag );
#else
	
	FARPROC lpfn;
	
	CALLSTACK cs;
	BOOL retv = FALSE;
	
	if( API_Enter( API_TABLE, TB_PACKBYHANDLE, &lpfn, &cs ) )
	{
		retv = ((PTB_PACKBYHANDLE)lpfn)( hTable, uiFlag );
		API_Leave(  );
	}
	return retv;
#endif
}

/*
typedef BOOL ( CALLBACK * PTB_CLOSEALL )( HANDLE hProcess );
BOOL CALLBACK Tb_CloseAll( HANDLE hProcess )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_CLOSEALL, 1 );
    cs.arg0 = (DWORD)hProcess ;
    return (DWORD)CALL_SERVER( &cs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_TABLE, TB_CLOSEALL, &lpfn, &cs ) )
	{
		retv = ((PTB_CLOSEALL)lpfn)( hProcess );
		API_Leave(  );
	}
	return retv;
#endif
}
*/

typedef BOOL ( WINAPI * PTB_ReadMultiRecords )( HTABLE hTable, LPMULTI_RECORD lpmultiRecord );
UINT WINAPI Tb_ReadMultiRecords( HTABLE hTable, LPMULTI_RECORD lpmultiRecord )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_READMULTIRECORDS, 2 );
    cs.arg0 = (DWORD)hTable ;
    return (DWORD)CALL_SERVER( &cs, lpmultiRecord );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	UINT retv = 0;

	if( API_Enter( API_TABLE, TB_READMULTIRECORDS, &lpfn, &cs ) )
	{
		retv = ((PTB_ReadMultiRecords)lpfn)( hTable, lpmultiRecord );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_GETUSERDATA )( HTABLE hTable, LPVOID lpUserBuf, UINT nBufSize );
UINT WINAPI Tb_GetUserData( 
					HTABLE hTable,					
                    LPVOID lpUserBuf,
					UINT nBufSize
                   )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_GETUSERDATA, 3 );
    cs.arg0 = (DWORD)hTable ;
    return (DWORD)CALL_SERVER( &cs, lpUserBuf, nBufSize );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	UINT retv = 0;

	if( API_Enter( API_TABLE, TB_GETUSERDATA, &lpfn, &cs ) )
	{
		retv = ((PTB_GETUSERDATA)lpfn)( hTable, lpUserBuf, nBufSize );
		API_Leave();
	}
	return retv;
#endif
}


typedef BOOL ( WINAPI * PTB_SETUSERDATA )( HTABLE hTable, LPCVOID lpUserBuf, UINT nBufSize );
UINT WINAPI Tb_SetUserData( 
					HTABLE hTable,					
                    LPCVOID lpUserBuf,
					UINT nBufSize
                   )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_SETUSERDATA, 3 );
    cs.arg0 = (DWORD)hTable ;
    return (DWORD)CALL_SERVER( &cs, lpUserBuf, nBufSize );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	UINT retv = 0;

	if( API_Enter( API_TABLE, TB_SETUSERDATA, &lpfn, &cs ) )
	{
		retv = ((PTB_SETUSERDATA)lpfn)( hTable, lpUserBuf, nBufSize );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PTB_FLUSH )( HTABLE hTable );
BOOL WINAPI Tb_Flush( HTABLE hTable )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_TABLE, TB_FLUSH, 1 );
    cs.arg0 = (DWORD)hTable ;
    return (DWORD)CALL_SERVER( &cs );
#else

	FARPROC lpfn;

	CALLSTACK cs;
	BOOL retv = 0;

	if( API_Enter( API_TABLE, TB_FLUSH, &lpfn, &cs ) )
	{
		retv = ((PTB_FLUSH)lpfn)( hTable );
		API_Leave();
	}
	return retv;
#endif
}
