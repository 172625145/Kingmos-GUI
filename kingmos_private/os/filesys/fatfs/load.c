/******************************************************
Copyright(c) °æÈ¨ËùÓÐ£¬1998-2003Î¢Âß¼­¡£±£ÁôËùÓÐÈ¨Àû¡£
******************************************************/

/*****************************************************
ÎÄ¼þËµÃ÷£º
°æ±¾ºÅ£º1.0.0
¿ª·¢Ê±ÆÚ£º2003-4-18
×÷Õß£ºÎºº£Á
ÐÞ¸Ä¼ÇÂ¼£º
******************************************************/

#include "fatfs.h"

#ifdef	KINGMOS
#include "efsdmgr.h"

extern void InitializeFATSys( void  );
static FSDDRV  fsd;
BOOL LoadFatFileSystem( void )
{
	InitializeFATSys(  );

    fsd.lpCloseFile=FAT_CloseFile;
    fsd.lpCloseVolume=FAT_CloseVolume;
    fsd.lpCreateDirectory=FAT_CreateDirectory;
    fsd.lpCreateFile=FAT_CreateFile;
    fsd.lpDeleteAndRenameFile=FAT_DeleteAndRenameFile;
    fsd.lpDeleteFile=FAT_DeleteFile;
    fsd.lpDeviceIoControl=FAT_DeviceIoControl;
    fsd.lpFindClose=FAT_FindClose;
    fsd.lpFindFirstFile=FAT_FindFirstFile;
    fsd.lpFindNextFile=FAT_FindNextFile;
    fsd.lpFlushFileBuffers=FAT_FlushFileBuffers;
    fsd.lpGetDiskFreeSpace=FAT_GetDiskFreeSpace;
    fsd.lpGetFileAttributes=FAT_GetFileAttributes;
    fsd.lpGetFileInformationByHandle=FAT_GetFileInformationByHandle;
    fsd.lpGetFileSize=FAT_GetFileSize;
    fsd.lpGetFileTime=FAT_GetFileTime;
    fsd.lpMountDisk=FAT_MountDisk;
    fsd.lpMoveFile=FAT_MoveFile;
    fsd.lpNotify=FAT_Notify;
    fsd.lpReadFile=FAT_ReadFile;
    fsd.lpReadFileWithSeek=FAT_ReadFileWithSeek;
    fsd.lpRegisterFileSystemFunction=FAT_RegisterFileSystemFunction;
    fsd.lpRemoveDirectory=FAT_RemoveDirectory;
    fsd.lpSetEndOfFile=FAT_SetEndOfFile;
    fsd.lpSetFileAttributes=FAT_SetFileAttributes;
    fsd.lpSetFilePointer=FAT_SetFilePointer;
    fsd.lpSetFileTime=FAT_SetFileTime;
    fsd.lpUnmountDisk=FAT_UnmountDisk;
    fsd.lpWriteFile  =FAT_WriteFile;
    fsd.lpWriteFileWithSeek=FAT_WriteFileWithSeek;
    fsd.lpCopyFile=FAT_CopyFile;

	return FSDMGR_RegisterFSD( "FATFSD", &fsd);
}

/*
int MultiByteToWideChar(
UINT CodePage, 
DWORD dwFlags, 
LPCSTR lpMultiByteStr, 
int cbMultiByte, 
LPWSTR lpWideCharStr, 
int cchWideChar )
{
	int i;

	for( i=0; i<cbMultiByte; i++ ){

		lpWideCharStr[i]=lpMultiByteStr[i];
		if( i>=cchWideChar)
			break;
	}
	return i;
}




int WideCharToMultiByte(
UINT CodePage, 
DWORD dwFlags, 
LPCWSTR lpWideCharStr, 
int cchWideChar, 
LPSTR lpMultiByteStr, 
int cbMultiByte, 
LPCSTR lpDefaultChar, 
BOOL* lpUsedDefaultChar )
{
	int i;
	for( i=0; i<cchWideChar; i++ ){

		lpMultiByteStr[i]=(char)lpWideCharStr[i];
		if( i>=cbMultiByte )
			break;
	}
	return i;
}
*/
#endif
/*
int	WideStrLen( WCHAR * lpszStr)
{
	int i=0;

	for( ;*lpszStr; i++){

		lpszStr++;
	}

	return i;
}
*/