/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __MMS_STRUCT_H
#define __MMS_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

typedef struct MMS_PDUStruct{
	int dwSize; // ���ݵĳ���
	int dwDealedSize; // �Ѿ������˵����ݳ���
	LPBYTE lpData; // ���ݻ���
}MMS_DATA, *LPMMS_DATA;


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__MMS_STRUCT_H

