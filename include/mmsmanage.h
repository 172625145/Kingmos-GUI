/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __MMSMANAGE_H
#define __MMSMANAGE_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

typedef void * HMMS;  // 一个MMS管理句柄，拥有该句柄就可以访问MMS
typedef void * HMMSDATA;  // 一个MMS信息的数据句柄，拥有该句柄，就可以对该信息进行显示

// MMS信息结构
#define INFO_FROMTO		0x0001  // 彩信的发件人/收件人
#define INFO_SUBJECT	0x0002  // 彩信的标题
#define INFO_DATE		0x0004  // 彩信的时间
#define INFO_STATUS		0x0008  // 彩信是否已读

#define MMS_NOTREAD		0x0001	// 未读MMS
#define MMS_READ		0x0002  // 已读MMS

typedef struct structMMSInfo
{
	UINT uSize;  // 结构大小，为以后扩展作准备
	UINT uMask; // 设定那些项是有效的

	LPTSTR lpFromTo;  // 存储发件人/收件人信息
	UINT uMaxFromTo;  // 发件人/收件人缓存大小

	LPTSTR lpSubject;  // 存储标题信息
	UINT uMaxSubject;  // 标题缓存大小

	SYSTEMTIME MMSTime;  // MMS的发送时间

	UINT uStatus;  // 显示当前MMS的状态

}MMSINFO, * LPMMSINFO;

#define MMS_RECEIVE		0x0001	//	收件箱
#define MMS_SEND		0x0002	//  发件箱
#define MMS_SENT		0x0003	//	已发信息
#define MMS_SAVED		0x0004	//  存档文件夹

// ***************************************************************************************
// 声明：BOOL InitialMMSSystem(void)
// 参数：
//	无
//
// 返回值：
//		无
// 功能描述：
//		初始化MMS系统
// ***************************************************************************************
BOOL InitialMMSSystem(void);

// ***************************************************************************************
// 声明：BOOL DeinitialMMSSystem(void)
// 参数：
//
// 返回值：
//		无
// 功能描述：
//		Deinitial MMS系统
// ***************************************************************************************
BOOL DeinitialMMSSystem(void);


// ***************************************************************************************
// 声明：HMMS OpenMMS(UINT uMMSType)
// 参数：
//	IN uMMSType  -- 要打开的MMS的类型，分为MMS_RECEIVE(收件箱)，MMS_SEND(发件箱),MMS_SENT(已发信息),MMS_SAVED(存档文件夹)
// 返回值：
//	返回一个MMS句柄
// 功能描述：
//  	打开一个MMS服务，以后可以使用该句柄对MMS进行访问
// ***************************************************************************************
HMMS OpenMMS(UINT uMMSType);

// ***************************************************************************************
// 声明：void CloseMMS(HMMS hMMS)
// 参数：
//    hMMS -- 要关闭的MMS句柄
// 返回值：
//	无
// 功能描述：
//  	关闭一个MMS服务，以后就不能对MMS进行访问
// ***************************************************************************************
void CloseMMS(HMMS hMMS);


// ***************************************************************************************
// 声明：UINT GetMMSCount(HMMS hMMS);
// 参数：
//    hMMS -- 已经打开的MMS句柄
// 返回值：
//		返回当前系统中的MMS的数目(包括已经下载的和没有下载的)
// 功能描述：
//  	得到当前系统中MMS的数目
// ***************************************************************************************
UINT GetMMSCount(HMMS hMMS);

// ***************************************************************************************
// 声明：UINT GetMMSCapacity(HMMS hMMS);
// 参数：
//    hMMS -- 已经打开的MMS句柄
// 返回值：
//		返回当前系统中的MMS的容量
// 功能描述：
//  	得到当前系统中MMS的容量大小
// ***************************************************************************************
UINT GetMMSCapacity(HMMS hMMS);

// ***************************************************************************************
// 声明：BOOL GetMMSInfo(HMMS hMMS, UINT uIndex,LPMMSINFO lpMMSInfo);
// 参数：
//   IN  hMMS -- 已经打开的MMS句柄
//   IN  uIndex -- 要得到MMS信息的条目索引
//   OUT lpMMSInfo -- 返回指定条目的信息，包括发信人，标题，时间
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	得到指定索引的MMS的信息
// ***************************************************************************************
BOOL GetMMSInfo(HMMS hMMS, UINT uIndex,LPMMSINFO lpMMSInfo);


// ***************************************************************************************
// 声明：BOOL DeleteMMS(HMMS hMMS, UINT uIndex);
// 参数：
//   IN  hMMS -- 已经打开的MMS句柄
//   IN  uIndex -- 要删除的MMS的条目索引
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	删除指定索引的MMS
// ***************************************************************************************
BOOL DeleteMMS(HMMS hMMS, UINT uIndex);


// ***************************************************************************************
// 声明：BOOL ClearMMS(HMMS hMMS);
// 参数：
//   IN  hMMS -- 已经打开的MMS句柄
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	删除所有的MMS
// ***************************************************************************************
BOOL ClearMMS(HMMS hMMS);

// ***************************************************************************************
// 声明：HMMSDATA LoadMMSData(HMMS hMMS, UINT uIndex);
// 参数：
//   IN  hMMS -- 已经打开的MMS句柄
//   IN  uIndex -- 要装载的MMS的条目索引
// 返回值：
//		成功返回MMS数据句柄，否则返回NULL
// 功能描述：
//  	装载指定索引的MMS数据
// ***************************************************************************************
HMMSDATA LoadMMSData(HMMS hMMS, UINT uIndex);


// ***************************************************************************************
// 声明：BOOL ReleaseMMSData(HMMS hMMS, HMMSDATA hMMSData);
// 参数：
//   IN  hMMS -- 已经打开的MMS句柄
//   IN  hMMSData -- 要释放的MMS数据句柄
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	释放的MMS数据
// ***************************************************************************************
BOOL ReleaseMMSData(HMMS hMMS, HMMSDATA hMMSData);


// 当调用程序失败，则调用GetLastError , 得到当前的错误类型

#define MMS_ERROR_SUCCESS			0x0000		// 没有错误
#define MMS_ERROR_NOMEMORY			0x3000		// 系统内存不足
#define MMS_ERROR_UNKNOWMMSTYPE		0x3001		// 不认识的MMS类型
#define MMS_ERROR_INVALIDHANDLE		0x3002		// 无效句柄
#define MMS_ERROR_INVALIDTABLE		0x3003		// 错误打开数据库
#define MMS_ERROR_INVALIDINDEX		0x3004		// 无效索引
#define MMS_ERROR_OVERFLOWSIZE		0x3005		// 彩信数据太大
#define MMS_ERROR_FULL				0x3006		// 当前记录已经满
#define MMS_ERROR_INVALIDPARAMETER	0x3007		// 无效参数
#define MMS_ERROR_TYPE				0x3008		// 错误的彩信类型
#define MMS_ERROR_ATTACHFLOW		0x3009		// 附件尺寸溢出
#define MMS_ERROR_READMMSFAILURE	0x300A		// 读取彩信数据失败


#define MAX_ATTACHNUM  6    // 用户最多可以添加的附件数

// uMask 定义
#define CM_MASK_TO		  0x0001    // 有发件人信息，lpTo 参数有效
#define CM_MASK_TEXT	  0x0002    // 有文本添加，lpText 参数有效
#define CM_MASK_ATTACH    0x0004    // 有要添加的附件，uAttachNum 和 lpAttachFile 参数有效

// 用来创建一个新的彩信的结构
typedef struct CreateMessageStruct{
	UINT uSize;  // 结构大小，为以后扩展作准备
	UINT uMask; // 设定那些项是有效的

	LPTSTR lpTo;  // 收件人信息
	LPTSTR lpText;  // 要发送的文本信息

	UINT uAttachNum; // 要添加的附件的个数
	LPTSTR lpAttachFile[MAX_ATTACHNUM]; // 用户要添加的附件的文件名

}CREATEMESSAGE,*LPCREATEMESSAGE;


// ***************************************************************************************
// 声明：HMMSDATA OrganizeMMSData(LPCREATEMESSAGE lpCreateMessage);
// 参数：
//   IN  lpCreateMessage -- 要组织的MMS数据结构
// 返回值：
//		成功返回MMS数据句柄，否则返回NULL
// 功能描述：
//  	组织要创建的MMS数据
// ***************************************************************************************
HMMSDATA OrganizeMMSData(LPCREATEMESSAGE lpCreateMessage);

// ***************************************************************************************
// 声明：int SaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex)
// 参数：
//   IN  hMMSData -- 要保存的MMS数据句柄
//   IN  uMMSType -- 要将数据保存到那一个信箱中
//   IN  iIndex -- 当前要保存的索引，如果是-1，则添加一条新记录
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	保存当前创建的彩信
// ***************************************************************************************
//BOOL SaveMMSMessage(HMMSDATA hMMSData);  // 保存当前创建的彩信
int SaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex);

// ***************************************************************************************
// 声明：BOOL SendMMSMessage(HMMSDATA hMMSData)
// 参数：
//   IN  hMMSData -- 要发送的MMS数据句柄
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	发送当前创建的彩信
// ***************************************************************************************
BOOL SendMMSMessage(HMMSDATA hMMSData);  // 发送当前创建的彩信

// ***************************************************************************************
// 声明：int SendAndSaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex)
// 参数：
//   IN  hMMSData -- 要发送并保存的MMS数据句柄
//   IN  uMMSType -- 要将数据保存到那一个信箱中
//   IN  iIndex -- 当前要保存的索引，如果是-1，则添加一条新记录
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	发送并保存当前创建的彩信
// ***************************************************************************************
int SendAndSaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex);


// ***************************************************************************************
// 声明：UINT GetMMSCount(HMMS hMMS);
// 参数：
//    hMMS -- 已经打开的MMS句柄
// 返回值：
//		返回当前系统中的MMS的数目(包括已经下载的和没有下载的)
// 功能描述：
//  	得到当前系统中MMS的数目
// ***************************************************************************************
UINT GetMMSCount(HMMS hMMS);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__MMSMANAGE_H
