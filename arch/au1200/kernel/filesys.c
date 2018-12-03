//#include <windows.h>
//#include <stdio.h>
//#include <eframe.h>
#include <edef.h>
#include <ecore.h>
#include <efile.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "handlepackage.h"

HANDLE WINAPI File_CreateFile( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
	//return CreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );

	//"r"	Opens for reading. If the file does not exist or cannot be found, the fopen call fails.

	//"w"	Opens an empty file for writing. If the given file exists, its contents are destroyed

	//"a"	Opens for writing at the end of the file (appending) without removing the EOF marker before writing new data to the file;
			//creates the file first if it doesn¡¯t exist

	//"r+"	Opens for both reading and writing. (The file must exist.)

	//"w+"	Opens an empty file for both reading and writing. If the given file exists, its contents are destroyed.

	//"a+"  Opens for reading and appending; the appending operation includes the removal of the EOF marker before new data is
			//written to the file and the EOF marker is restored after writing is complete;
			//creates the file first if it doesn¡¯t exist.
	LPTRHANDLEPACKAGE lpvoid; 
	FILE			  *lpFile;	

//	printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	if(dwCreate == CREATE_NEW){ //Creates a new file. The function fails if the specified file already exists.
		lpFile = fopen(lpcszFileName, "r+b");
	}else if(dwCreate == CREATE_ALWAYS){ //Creates a new file. If the file exists, the function overwrites the file and clears the existing attributes. 
		lpFile = fopen(lpcszFileName, "w+b");
	}else if(dwCreate == OPEN_EXISTING){ //Opens the file. The function fails if the file does not exist. 
		if(dwAccess & GENERIC_WRITE){
			lpFile = fopen(lpcszFileName, "r+b");
		}else{
			lpFile = fopen(lpcszFileName, "rb");
		}
	}else if(dwCreate == OPEN_ALWAYS){ //Opens the file, if it exists. If the file does not exist, the function creates the file as if dwCreationDisposition were CREATE_NEW. 
		lpFile = fopen(lpcszFileName, "wb");
	}else if(dwCreate == TRUNCATE_EXISTING){  //Opens the file. Once opened, the file is truncated so that its size is zero bytes. 
		                                      //The calling process must open the file with at least GENERIC_WRITE access.
		                                      //The function fails if the file does not exist.
		if(!(dwAccess & GENERIC_WRITE)){
			lpFile = (FILE *)0;
		}
		lpFile = fopen(lpcszFileName, "a+b");
	}

	if(lpFile == (FILE *)0){
		printf("open file fail (filename:%s)", lpcszFileName);
		return (HANDLE)INVALID_HANDLE_VALUE;
	}

	lpvoid = (LPTRHANDLEPACKAGE)malloc(sizeof(HANDLEPACKAGE));
	if(lpvoid == NULL){
		printf("open file error 1(filename:%s)", lpcszFileName);
		fclose(lpFile);
		return (HANDLE)INVALID_HANDLE_VALUE;

	}else{
		lpvoid->nHandleType = FILE_HANDLE;
		lpvoid->lpv = (HANDLE)lpFile;
	}

	return (HANDLE)lpvoid;
}

BOOL WINAPI File_Read( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped )
{
	//return ReadFile( hFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)hFile; 
	BOOL  bFlag=TRUE;	
	DWORD i; 

	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	if(lptr->nHandleType != FILE_HANDLE){
		return FALSE;
	}

	i = fread(lpvBuffer, sizeof(char), dwNumToRead, (FILE *)(lptr->lpv));
	if(lpdwNumRead)
		*lpdwNumRead = (DWORD)i;
	if(i < dwNumToRead){
		if( ferror((FILE *)(lptr->lpv)) ){
			bFlag = FALSE;
		}
	}
	return bFlag;
}

//BOOL File_Write(HANDLE hFile, LPVOID lpvBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
BOOL WINAPI File_Write( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped )
{
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)hFile; 
	BOOL  bFlag=TRUE;	
	DWORD i; 

	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	if(lptr->nHandleType != FILE_HANDLE){
		return FALSE;
	}

	i = fwrite(lpvBuffer, sizeof(char), dwNumToWrite, (FILE *)(lptr->lpv));
	if( lpdwNumWrite )
		*lpdwNumWrite = (DWORD)i;
	if(i < dwNumToWrite){
		if( ferror((FILE *)(lptr->lpv)) ){
			bFlag = FALSE;
		}
	}

	return bFlag;
}

DWORD WINAPI File_SetPointer( HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod )
{
	//return SetFilePointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)hFile; 
	int origin;

	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	
	if(lptr->nHandleType != FILE_HANDLE){
		return (DWORD)-1;
	}

	if(FILE_BEGIN == dwMoveMethod){  
		origin = SEEK_SET;
	}else if(FILE_CURRENT == dwMoveMethod){
		origin = SEEK_CUR;
	}else if(FILE_END == dwMoveMethod){
		origin = SEEK_END;
	}

	if(fseek((FILE *)(lptr->lpv), lDistanceToMove, origin)){
		return (DWORD)(-1);//INVALID_SET_FILE_POINTER;	
	}else{
		return (DWORD)ftell((FILE *)(lptr->lpv));
	}
	
	return (DWORD)(-1);//INVALID_SET_FILE_POINTER;
}


DWORD WINAPI File_GetSize( HANDLE hFile, LPDWORD lpFileSizeHigh )
{
	//return SetFilePointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)hFile; 
	long origin;
	DWORD dwSize;

	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	
	if(lptr->nHandleType != FILE_HANDLE){
		return (DWORD)-1;
	}
	origin = (DWORD)ftell((FILE *)(lptr->lpv));
	fseek( (FILE *)(lptr->lpv), 0, SEEK_END );
	dwSize =(DWORD)ftell( (FILE *)(lptr->lpv) );
	fseek( (FILE *)(lptr->lpv), origin, SEEK_SET );
	if( lpFileSizeHigh )
		*lpFileSizeHigh = 0;
	return dwSize;
	
//	return (DWORD)(-1);//INVALID_SET_FILE_POINTER;
}

BOOL WINAPI File_SetEnd( HANDLE hFile )
{
	//return SetFilePointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
	LPTRHANDLEPACKAGE lptr = (LPTRHANDLEPACKAGE)hFile; 
	long fn, cur;

	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );
	
	if(lptr->nHandleType != FILE_HANDLE){
		return (DWORD)-1;
	}
	cur = (DWORD)ftell((FILE *)(lptr->lpv));		
	fn = fileno( (FILE *)(lptr->lpv) );
	return !ftruncate( fn, cur );
}

BOOL WINAPI File_CreateDirectory(
  LPCTSTR lpPathName,                         // directory name
  LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
)
{
	return !mkdir( lpPathName, ALLPERMS );
}

BOOL WINAPI File_Delete(
  LPCTSTR lpFileName   // file name
)
{
	return !remove( lpFileName );
}


#include <glob.h>

/*
typedef struct _FILE_FIND_DATA 
{ 
	DWORD dwFileAttributes; 
	FILETIME ftCreationTime; 
	FILETIME ftLastAccessTime; 
	FILETIME ftLastWriteTime; 
	DWORD nFileSizeHigh; 
	DWORD nFileSizeLow; 
	DWORD dwOID; 
	TCHAR cFileName[MAX_PATH]; 
}FILE_FIND_DATA, * PFILE_FIND_DATA, FAR * LPFILE_FIND_DATA; 
*/
/*
typedef struct
{
    size_t gl_pathc;    // Count of paths matched so far  
    char **gl_pathv;    // List of matched pathnames.  
    size_t gl_offs;     // Slots to reserve in `gl_pathv'.  
} glob_t;
*/
/*
S_ISREG(m)
is it a regular file? 
S_ISDIR(m)
directory? 
S_ISCHR(m)
character device? 
S_ISBLK(m)
block device? 
S_ISFIFO(m)
fifo? 
S_ISLNK(m)
symbolic link? (Not in POSIX.1-1996.) 
S_ISSOCK(m)
socket? (Not in POSIX.1-1996.) 

*/
static BOOL GetFindData( glob_t * lpglob, int pos, FILE_FIND_DATA * pfd )
{
	struct stat statBuf;
	memset( pfd, 0, sizeof( FILE_FIND_DATA ) );
	if( lstat( lpglob->gl_pathv[pos], &statBuf ) == 0 )
	{	//success
		char *pszFile;
		pfd->dwFileAttributes = 0;
		if( S_ISREG(statBuf.st_mode) )
		{
			pfd->dwFileAttributes |= FILE_ATTRIBUTE_NORMAL;
		}
		
		if(S_ISDIR(statBuf.st_mode))
		{
			pfd->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
		}
		if(!(statBuf.st_mode & S_IWUSR))
		{
			pfd->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
		}
		pfd->ftCreationTime.dwLowDateTime = statBuf.st_ctime;
		pfd->ftLastAccessTime.dwLowDateTime = statBuf.st_atime;
		pfd->ftLastWriteTime.dwLowDateTime = statBuf.st_mtime;
		pfd->nFileSizeLow =  statBuf.st_size;
		pszFile = strrchr(lpglob->gl_pathv[pos], '/');
		if(pszFile)
		{
			strncpy( pfd->cFileName, pszFile + 1, sizeof(pfd->cFileName) );
			printf("%s(%s)\r\n", pfd->cFileName, ((pfd->dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? "rdonly" : "normal"));
		}
		else
		{
			strncpy( pfd->cFileName, lpglob->gl_pathv[pos], sizeof(pfd->cFileName) );
			printf("%s(%s)\r\n", pfd->cFileName, ((pfd->dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? "rdonly" : "normal"));
		}
//		printf("find:%s ", lpglob->gl_pathv[pos]);
		return TRUE;
	}
	return FALSE;
}

HANDLE WINAPI File_FindFirst( LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd )
{
	glob_t * lpglob = (glob_t *) malloc( sizeof(glob_t) );
	int retv;
	
	if( lpglob )
	{
		retv = glob( lpcszFileSpec, 0, 0, lpglob );
		if( retv == 0 )
		{	//success
			if( GetFindData( lpglob, 0, pfd ) )
			{
				lpglob->gl_offs++;
				return lpglob;
			}
			globfree(lpglob);
		}
	}
	return INVALID_HANDLE_VALUE;
}


#define FindNextFile File_FindNext
BOOL WINAPI File_FindNext( HANDLE hFind, FILE_FIND_DATA * pfd )
{
	glob_t * lpglob = (glob_t *)hFind;

	if( lpglob && lpglob != INVALID_HANDLE_VALUE )
	{
		if( lpglob->gl_offs < lpglob->gl_pathc )
		{
			if( GetFindData( lpglob, lpglob->gl_offs, pfd ) )
			{
				lpglob->gl_offs++;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL WINAPI File_FindClose( HANDLE hFind )
{
	glob_t * lpglob = (glob_t *)hFind;

	if( lpglob && lpglob != INVALID_HANDLE_VALUE )
	{
		lpglob->gl_offs = 0;
		globfree(lpglob);
		return TRUE;
	}
	return FALSE;
}


BOOL WINAPI File_RemoveDirectory( LPCTSTR lpcszPathName )
{
	int iRet;
	iRet = rmdir(lpcszPathName);
	return iRet ? FALSE : TRUE;
}

BOOL WINAPI File_SetAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )
{
	int iRet;
	mode_t mode = 0;
	mode |= S_IRUSR | S_IWUSR;
	
	if(dwFileAttributes & FILE_ATTRIBUTE_READONLY)
	{ 
		mode &= ~S_IWUSR;
	}
	iRet = chmod(lpcszFileName, mode);
	return iRet ? FALSE : TRUE;
}
