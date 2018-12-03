/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __MIME_BODY_H
#define __MIME_BODY_H

#ifndef KINGMOS
#include "windows.h"
#else
#include <ewindows.h>
#endif

//#include "..\include\MMSStruct.h"
#include <mms_clt.h>

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

// 在这里定义一个MIME 的一个具体的部分（即一个多媒体的文件）
// ****************************************************
// 预定义区域
// ****************************************************
#define PARTITEM_UNKNOW			0x0000  // 不认识的ID
#define PARTITEM_CONTENT_URL	0x0001  // X-Wap-Content-URI
#define PARTITEM_CONTENT_ID		0x0002  // Content_ID
#define PARTITEM_CONTENT_DES	0x0004  // Content Description 
#define PARTITEM_CONTENT_LOCAL	0x0008  // Content location
#define PARTITEM_DATE			0x0010  // 日期


typedef struct MIME_PartStruct{
	UINT uMask; //字段屏蔽码
	LPTSTR lpContent_Type; // 当前文件的类型
	LPTSTR lpContent_ID; // 当前部分的标识号
	LPTSTR lpContent_Description ; // 当前部分的标识描述
	LPTSTR lpContent_Location; // 当前部分的文件名
	LPTSTR lpContent_Url; // 当前部分的URL
	DWORD  dwDate;  // 当前PART的时间

	// 关于数据部分
	UINT uPos; // 内容开始位置
	UINT uLen; // 内容长度


	TCHAR lpPath[MAX_PATH]; // 当前文件保存到本地后的全路径文件名
	struct MIME_PartStruct *next; // 指向下一个PART的指针
}MIME_PART,*LPMIME_PART;

// 在这里定义一个MIME Body的结构，来存放MIME的内容


typedef struct BodyDetailStruct{
		int iPartNum; // 在这里一共包含有几个部分
		LPMIME_PART lpMIME_Part; // 指向具体的MIME 内容
}BODYDETAIL, *LPBODYDETAIL;


typedef struct MIME_BodyStruct{
		int iDataSize; // 在这里一共包含有几个部分
		LPBYTE lpData; // 指向具体的MIME 内容
}MIME_BODY, *LPMIME_BODY;


LPBODYDETAIL MIME_UnPack(LPDATA_DEAL lpMMS_Data,BOOL *pbOver);
BOOL MIME_Pack(LPDATA_DEAL lpMMS_Data,LPBODYDETAIL lpBodyDetail);
void MIME_Release(LPBODYDETAIL lpBodyDetail);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__MIME_BODY_H
