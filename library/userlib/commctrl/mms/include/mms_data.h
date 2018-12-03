/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __MMS_DATA_H
#define __MMS_DATA_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

// 该结构用于得到或设置当前彩信数据的结构
typedef struct structMMS_Content{
	LPTSTR lpFromTo; // 发件人或收件人
	LPTSTR lpSubject; //  标题
	DWORD  dwDate;  // 时间
	LPTSTR lpContent_Type;  // 内容类型 
	LPBYTE lpData; // 数据部分
	int		iDataSize; // 数据长度
}MMS_CONTENT, *LPMMS_CONTENT;


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif // __MMS_DATA_H

