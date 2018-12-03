#ifndef __EPFS_H
#define __EPFS_H

typedef struct __FILEDATA {
	    //OBJLIST obj;
        //DWORD objType;   // must is  OBJ_FILE
        HVOL hVol;      // volume data
        //HANDLE hProc;
		//struct __FILEDATA FAR * lpNext;
        PFILE  pFile;   // file data
}_FILEDATA, FAR * _LPFILEDATA;
//_LPFILEDATA _GetHFILEPtr( HANDLE hFile );

BOOL _File_Close( HFILE hFile );   // call by CloseHandle

typedef struct __FINDDATA {
	OBJLIST obj;
    //DWORD objType;   // must is  OBJ_FINDFILE
    HVOL hVol;  // volume data
    //HANDLE hProc;
	//struct __FINDDATA FAR * lpNext;
    PFIND  pFind;   // find data
}_FINDDATA, FAR * _LPFINDDATA;
_LPFINDDATA _GetHFINDPtr( HANDLE hFind );

typedef struct __FSDDSK {
    HANDLE hFile;  // handle of opennig device driver
    DEVICE_INFO di;
    FSD_DISK_DATA fdi;
    LPFSDDRV lpfsd;
}_FSDDSK, FAR * _LPFSDDSK;

typedef struct __VOLUME {
    struct __VOLUME * lpNext;
    _LPFSDDSK lpdsk;
    LPTSTR lpszVolName;   // vol name
    PVOL pVol;   // user data
}_VOLUME, * _PVOLUME, FAR * _LPVOLUME;

//_LPVOLUME _FSMGR_FindVolume(LPCTSTR lpszVolName, UINT uNameLen);
_LPVOLUME _FileMgr_FindVolume( LPCTSTR lpcszVolName, UINT uNameLen );


#endif  // __EPFS_H


