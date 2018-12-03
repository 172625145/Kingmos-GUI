/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __MMS_STRUCT_H
#define __MMS_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

typedef struct MMS_PDUStruct{
	int dwSize; // 数据的长度
	int dwDealedSize; // 已经处理了的数据长度
	LPBYTE lpData; // 数据缓存
}MMS_DATA, *LPMMS_DATA;


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__MMS_STRUCT_H

