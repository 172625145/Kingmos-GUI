/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
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

// �����ﶨ��һ��MIME ��һ������Ĳ��֣���һ����ý����ļ���
// ****************************************************
// Ԥ��������
// ****************************************************
#define PARTITEM_UNKNOW			0x0000  // ����ʶ��ID
#define PARTITEM_CONTENT_URL	0x0001  // X-Wap-Content-URI
#define PARTITEM_CONTENT_ID		0x0002  // Content_ID
#define PARTITEM_CONTENT_DES	0x0004  // Content Description 
#define PARTITEM_CONTENT_LOCAL	0x0008  // Content location
#define PARTITEM_DATE			0x0010  // ����


typedef struct MIME_PartStruct{
	UINT uMask; //�ֶ�������
	LPTSTR lpContent_Type; // ��ǰ�ļ�������
	LPTSTR lpContent_ID; // ��ǰ���ֵı�ʶ��
	LPTSTR lpContent_Description ; // ��ǰ���ֵı�ʶ����
	LPTSTR lpContent_Location; // ��ǰ���ֵ��ļ���
	LPTSTR lpContent_Url; // ��ǰ���ֵ�URL
	DWORD  dwDate;  // ��ǰPART��ʱ��

	// �������ݲ���
	UINT uPos; // ���ݿ�ʼλ��
	UINT uLen; // ���ݳ���


	TCHAR lpPath[MAX_PATH]; // ��ǰ�ļ����浽���غ��ȫ·���ļ���
	struct MIME_PartStruct *next; // ָ����һ��PART��ָ��
}MIME_PART,*LPMIME_PART;

// �����ﶨ��һ��MIME Body�Ľṹ�������MIME������


typedef struct BodyDetailStruct{
		int iPartNum; // ������һ�������м�������
		LPMIME_PART lpMIME_Part; // ָ������MIME ����
}BODYDETAIL, *LPBODYDETAIL;


typedef struct MIME_BodyStruct{
		int iDataSize; // ������һ�������м�������
		LPBYTE lpData; // ָ������MIME ����
}MIME_BODY, *LPMIME_BODY;


LPBODYDETAIL MIME_UnPack(LPDATA_DEAL lpMMS_Data,BOOL *pbOver);
BOOL MIME_Pack(LPDATA_DEAL lpMMS_Data,LPBODYDETAIL lpBodyDetail);
void MIME_Release(LPBODYDETAIL lpBodyDetail);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__MIME_BODY_H
