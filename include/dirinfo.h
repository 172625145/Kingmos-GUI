/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef _EDIRINFO_H
#define _EDIRINFO_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// 这里是你的有关声明部分

#include <ewindows.h>


#define DI_SEARCHSUBDIR	0X00000001	// 查找子目录
#define DI_STATISTICDIR	0X00000002	// 统计目录的个数

typedef struct DirectoryInfoStruct{
	DWORD	dwSize;
	LPTSTR	lpCurDir;
	DWORD	dwNumber; // 该目录下文件的个数
	DWORD	dwDirSize; // 该目录下文件的总的大小
	DWORD	dwFlag; // 得到目录信息的标志

}DIRECTORYINFO, * LPDIRECTORYINFO;

BOOL GetDirectoryInfo(LPDIRECTORYINFO lpDirectoryInfo);


#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //_EDIRINFO_H

