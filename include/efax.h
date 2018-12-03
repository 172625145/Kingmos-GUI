/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/

#ifndef __EFAX_H
#define __EFAX_H

#ifndef VC386
#include <econtact.h>
#endif

#ifdef _cpluscplus
extern "C" {
#endif


#define FLOAD_SUCCESS 0
#define FLOAD_CRFILFAILE 1
#define FLOAD_CRDIRFAILE 2
#define FLOAD_NOENOGHMEM 3
#define FLOAD_READFILFAIL 4
#define FLOAD_WRITFILFAIL 5
#define FLOAD_ERROR_DATA 6

#define BH_MODEL 1//传送位图句柄
#define TB_MODEL 2//传送文本缓冲区
#define TF_MODEL 3//传送文本文件
#define SUBJECT_LEN 20

#ifdef VC386
#define PHONENUMBER_LEN 20
#endif
extern LPCTSTR g_szInterChangeFile;
typedef struct _FAXGUIDE{
	TCHAR szSubject[SUBJECT_LEN];//主题
	TCHAR szFaxNumber[PHONENUMBER_LEN];//对方传真号
	PVOID pData;//数据，可以是位图句柄、文本缓冲区指针、文本文件名。装入时必须指明
	DWORD dwDataSize;//数据大小，装入时必须指明
	DWORD dwDataMode;//数据格式,可以是与pDate分别相对应的BH_MODE、TB_MODEL、TF_MODEL之一。装入时必须指明
}FAXGUIDE, *PFAXGUIDE;

DWORD LoadFax(PFAXGUIDE pFaxGuide);

#ifdef _cpluscplus
}	//cpluscplus
#endif


#endif
