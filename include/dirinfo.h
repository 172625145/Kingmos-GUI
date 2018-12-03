/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
#ifndef _EDIRINFO_H
#define _EDIRINFO_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// ����������й���������

#include <ewindows.h>


#define DI_SEARCHSUBDIR	0X00000001	// ������Ŀ¼
#define DI_STATISTICDIR	0X00000002	// ͳ��Ŀ¼�ĸ���

typedef struct DirectoryInfoStruct{
	DWORD	dwSize;
	LPTSTR	lpCurDir;
	DWORD	dwNumber; // ��Ŀ¼���ļ��ĸ���
	DWORD	dwDirSize; // ��Ŀ¼���ļ����ܵĴ�С
	DWORD	dwFlag; // �õ�Ŀ¼��Ϣ�ı�־

}DIRECTORYINFO, * LPDIRECTORYINFO;

BOOL GetDirectoryInfo(LPDIRECTORYINFO lpDirectoryInfo);


#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //_EDIRINFO_H

