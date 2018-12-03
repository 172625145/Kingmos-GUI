/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __MMS_TANSLATE_H
#define __MMS_TANSLATE_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

#include "..\include\mms_content.h"

// ***************************************************************************************
// 声明：BOOL GetNotificationFromServer(DWORD dwDataID ,LPMMS_CONTENT lpMMS_Content)
// 参数：
//  IN  dwDataID -- 要得到的通知消息的ID
//  OUT lpMMS_Content -- 存储通知消息的结构
// 返回值：
//		返回当前系统中的MMS的数目(包括已经下载的和没有下载的)
// 功能描述：
//  	得到当前系统中MMS的数目
// ***************************************************************************************
BOOL GetNotificationFromServer(DWORD dwDataID ,LPMMS_CONTENT lpMMS_Content);

// ***************************************************************************************
// 声明：BOOL GetMMSFromServer(LPVOID lpData, DWORD dwSize,LPMMS_CONTENT lpMMS_Content)
// 参数：
//  IN  lpData -- 通知包的数据
//	IN  dwSize -- 通知包的数据长度
//  OUT lpMMS_Content -- 存储彩信数据的结构
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	从服务器读取彩信数据
// ***************************************************************************************
BOOL GetMMSFromServer(LPVOID lpData, DWORD dwSize,LPMMS_CONTENT lpMMS_Content);

// ***************************************************************************************
// 声明：BOOL SentMMSToServer(LPMMS_CONTENT lpMMS_Content)
// 参数：
//  IN lpMMS_Content -- 要发送的彩信数据
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	发送彩信数据到服务器
// ***************************************************************************************
BOOL SentMMSToServer(LPMMS_CONTENT lpMMS_Content);


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif // __MMS_TANSLATE_H

