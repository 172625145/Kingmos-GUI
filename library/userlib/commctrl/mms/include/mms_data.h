/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __MMS_DATA_H
#define __MMS_DATA_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

// �ýṹ���ڵõ������õ�ǰ�������ݵĽṹ
typedef struct structMMS_Content{
	LPTSTR lpFromTo; // �����˻��ռ���
	LPTSTR lpSubject; //  ����
	DWORD  dwDate;  // ʱ��
	LPTSTR lpContent_Type;  // �������� 
	LPBYTE lpData; // ���ݲ���
	int		iDataSize; // ���ݳ���
}MMS_CONTENT, *LPMMS_CONTENT;


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif // __MMS_DATA_H

