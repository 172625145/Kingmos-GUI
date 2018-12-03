/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __MMS_TANSLATE_H
#define __MMS_TANSLATE_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

#include "..\include\mms_content.h"

// ***************************************************************************************
// ������BOOL GetNotificationFromServer(DWORD dwDataID ,LPMMS_CONTENT lpMMS_Content)
// ������
//  IN  dwDataID -- Ҫ�õ���֪ͨ��Ϣ��ID
//  OUT lpMMS_Content -- �洢֪ͨ��Ϣ�Ľṹ
// ����ֵ��
//		���ص�ǰϵͳ�е�MMS����Ŀ(�����Ѿ����صĺ�û�����ص�)
// ����������
//  	�õ���ǰϵͳ��MMS����Ŀ
// ***************************************************************************************
BOOL GetNotificationFromServer(DWORD dwDataID ,LPMMS_CONTENT lpMMS_Content);

// ***************************************************************************************
// ������BOOL GetMMSFromServer(LPVOID lpData, DWORD dwSize,LPMMS_CONTENT lpMMS_Content)
// ������
//  IN  lpData -- ֪ͨ��������
//	IN  dwSize -- ֪ͨ�������ݳ���
//  OUT lpMMS_Content -- �洢�������ݵĽṹ
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	�ӷ�������ȡ��������
// ***************************************************************************************
BOOL GetMMSFromServer(LPVOID lpData, DWORD dwSize,LPMMS_CONTENT lpMMS_Content);

// ***************************************************************************************
// ������BOOL SentMMSToServer(LPMMS_CONTENT lpMMS_Content)
// ������
//  IN lpMMS_Content -- Ҫ���͵Ĳ�������
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	���Ͳ������ݵ�������
// ***************************************************************************************
BOOL SentMMSToServer(LPMMS_CONTENT lpMMS_Content);


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif // __MMS_TANSLATE_H

