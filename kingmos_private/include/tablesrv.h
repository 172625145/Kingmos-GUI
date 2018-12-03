/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __TABLESRV_H
#define __TABLESRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

HTABLE WINAPI Table_CreateByStruct( LPCREATE_TABLE lpct );//LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, UINT nFieldNum, DWORD dwTableFileSize );
BOOL WINAPI Table_Delete( LPCTSTR lpcszFileName );
BOOL WINAPI Table_Close( HTABLE hTable );
WORD WINAPI Table_SetRecordPointer( HTABLE hTable, long lOffset, UINT fMethod );
WORD WINAPI Table_NextRecordPointer( HTABLE hTable, BOOL bNext );
WORD WINAPI Table_ReadField( HTABLE hTable, UINT nField, WORD wStartPos, void * lpBuf, WORD wSizeLimit );
WORD WINAPI Table_WriteField( HTABLE hTable, UINT nField, const void * lpcvBuf, WORD wSize );
long WINAPI Table_WriteRecord( HTABLE hTable, FIELDITEM fieldItem[] );
BOOL WINAPI Table_DeleteRecord( HTABLE hTable, UINT uiRecord, DWORD dwFlag );
BOOL WINAPI Table_SortRecord( HTABLE hTable, WORD field, FIELDCOMPPROC lpfcProc );
BOOL WINAPI Table_AppendRecord( HTABLE hTable, FIELDITEM fieldItem[] );
BOOL WINAPI Table_ReadRecord( 
					HTABLE hTable,
                    void * lpvData,
                    FIELDITEM fieldItem[],
                    DWORD * lpdwSize );
BOOL WINAPI Table_InsertRecord( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] );
int WINAPI Table_CountRecord( HTABLE hTable );
BOOL WINAPI Table_GetRecordInfo( HTABLE hTable, LPRECORDINFO lpri );
BOOL WINAPI Table_GetTableInfo( LPCTSTR lpcszFileName, LPTABLEINFO lpti );
DWORD WINAPI Table_GetRecordSize( HTABLE hTable );
HANDLE WINAPI Table_FindFirst( HTABLE hTable, UINT uField, const void * lpcvFind, UINT uLen, LPFINDSTRUCT lpfs );
BOOL WINAPI Table_FindNext( HANDLE hFind, LPFINDSTRUCT lpfs );
BOOL WINAPI Table_CloseFind( HANDLE hFind );
BOOL WINAPI Table_Pack( LPCTSTR lpcszFileName, UINT uiFlag );
BOOL WINAPI Table_CloseAll( HANDLE hProcess );
UINT WINAPI Table_ReadMultiRecords( HTABLE hTable, LPMULTI_RECORD lpmultiRecord );
UINT WINAPI Table_GetUserData( HTABLE hTable, LPVOID lpUserBuf,	UINT nBufSize );
UINT WINAPI Table_SetUserData( HTABLE hTable, LPCVOID lpUserBuf, UINT nWriteSize );
BOOL WINAPI Table_Flush( HTABLE hTable );
BOOL WINAPI Table_PowerHandler( BOOL bShutDown );
BOOL WINAPI Table_PackByHandle( HANDLE hTable, UINT uiFlag );
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__TABLESRV_H

