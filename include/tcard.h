/***************************************************
Copyright (c) 所有，1998-2003 微逻辑(R)。保留所有权利。
***************************************************/

/**************************************************
文件说明：TCARD 库的头文件声明
版本号：  1.0.0
开发时期：2004-06-24
卡片应用：包含对 simcard, 本地日志卡片, MMS(彩信)的读取
修改记录：
**************************************************/

#ifndef __TCARD_H
#define __TCARD_H


#ifndef __TELEPHONE_H
#include <telephone.h>
#endif

#include <mmsmanage.h>

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

// 定义 Mobile 信息类型（短信、彩信、电话）
#define MAX_PHONELEN	(MAX_PHONE_LEN+4)	// 定义电话号码的最大长度,包含'\0'，并对齐到4
//#define MAX_NUMBERLEN	(MAX_PHONE_LEN+1)	// 定义电话号码的最大长度,包含'\0'，并对齐到4
#define MAX_SMGLEN		(160+4)			    // 定义短消息的最大长度，包含'\0'，并对齐到4
//#define MAX_CONTENTLEN	(MAX_SMGLEN+1)	// 定义短消息的最大长度,包含'\0'，并对齐到4
#define MAX_NAMELEN		(MAX_NAME_LEN+4)	// 定义姓名的最大长度,包含'\0'，并对齐到4
#define MAX_PINLEN		(6+2)				// 定义PIN的最大长度,包含'\0'，并对齐到4
#define MAX_CAPTION

// 定义SIM卡电话本基本信息结构
typedef struct _SIMPHONE_ITEM{
	UINT		nSize;					// = sizeof(SIMPHONE_ITEM)
	UINT		nPosIndex;				// 从什么位置开始查找，初始化为1
	TCHAR		szNumber[MAX_PHONELEN];	// 电话号码（(假如有的话)）
	TCHAR		szName[MAX_NAMELEN];	// 姓名或用于显示的说明文本(假如有的话)
}SIMPHONE_ITEM, FAR * LPSIMPHONE_ITEM;


//短消息的标志状态值
#define SMSF_RECV_UNREAD	0x0			//短消息已接收但还未被读取
#define SMSF_RECV_READED    0x1			//短消息已接收并被读取
#define SMSF_STO_UNSENT     0x2			//短消息已保存但未被发送
#define SMSF_STO_SENT		0x3			//短消息已保存且已发送
typedef struct _SMS_RECORD{
    UINT		nSize;					// = sizeof(SMS_RECORD)
	SYSTEMTIME	sysTime;				// 时间 
	UINT		uFlag;					// 短消息的标志状态值
	UINT		nPosIndex;				// 从什么位置开始查找，初始化为1
	TCHAR		szNumber[MAX_PHONELEN];	// 电话号码（(假如有的话)）
	TCHAR		szContent[MAX_SMGLEN];	// 内容
	TCHAR		szName[MAX_NAMELEN];	// 姓名或用于显示的说明文本(假如有的话)
}SMS_RECORD, FAR * LPSMS_RECORD;
typedef SMS_RECORD SIMSMS_ITEM;
typedef LPSMS_RECORD LPSIMSMS_ITEM;

// 定义MMS基本信息结构
typedef struct _MMS_FIND_ITEM{
    UINT		nSize;					// = sizeof(SIMSMS_ITEM)
	MMSINFO		MMSInfo;				// 短消息的类行
	UINT		nPosIndex;				// 从什么位置开始查找，初始化为1
}MMS_FIND_ITEM, FAR * LPMMS_FIND_ITEM;

// 定义日志记录基本信息结构
typedef struct _LOG_RECORD{
    UINT		nSize;					// = sizeof(LOG_ITEM)
	UINT		nLogType;				// 记录类型（指示打入、打出、未接）
	UINT		nPosIndex;				// 从什么位置开始查找，初始化为1 //
	SYSTEMTIME	sysTime;				// 开始时间 
	DWORD		dwTalkTime;				// 通话时间（秒）
	TCHAR		szNumber[MAX_PHONELEN];	// 电话号码(假如有的话) 
	TCHAR		szName[MAX_SMGLEN];		// 内容
	BOOL		bRead;					// 是否已经读
}LOG_RECORD, FAR * LPLOG_RECORD;

// 定义日志增加记录基本信息结构 
typedef struct _LOG_SAVE_ITEM{
    UINT		nSize;					// = sizeof(LOG_ADD_ITEM)
	UINT		nLogType;				// 记录类型（指示打入、打出、未接）
	SYSTEMTIME	sysTime;				// 开始时间
	DWORD		dwTalkTime;				// 假如有的话，通话时间（秒）
	TCHAR		szNumber[MAX_PHONELEN];	// 电话号码
}LOG_SAVE_ITEM, FAR * LPLOG_SAVE_ITEM;


enum{
	CARD_NULL = 0,
	CARD_SIMPHONE = 0x1,	//SIM Card 电话本
	CARD_SIMSMS,			//SIM Card 短消息
	CARD_RECENT,			//	  0x3			//近来
	CARD_OUT,				//    0x4			//打出
	CARD_IN,				//    0x5			//打入
	CARD_UNRECEIVE,			//    0x6			//未接
	CARD_ADDRESSBOOK,		//    0x7
	CARD_MMSRECEIVE,		//    0x8			// 彩信收件箱
	CARD_MMSSEND,			//    0x8			// 彩信发件箱
	CARD_MMSSENT,			//    0x9			// 彩信已发
	CARD_MMSSAVE,			//    0xA			// 彩信保存
	CARD_SMSRECEIVE,		//短信收件箱
	CARD_SMSSEND,			//短信发件箱
	CARD_SMSSENT,			//短信已发信息
	CARD_SMSSAVE,			//短信存档信息
	MAX_CARDS
};

//统计卡片用的数据结构
typedef struct _TCARD_INFO
{
	UINT uiSize;		//必须 = sizeof(TCARD_INFO)
	UINT nTotalRecords;
	UINT nFreeRecords;
	UINT nUsedRecords;
}TCARD_INFO, FAR * LPTCARD_INFO;

//搜索内容类型
#define SRM_NAME		1
#define SRM_PHONECODE	2
#define SRM_CONTENT		3
#define SRM_SMSSTATE	4	// SIM 卡短信状态
//
//卡片调用界面，一个新的卡片必须实现以下功能
typedef struct _TCARDDRV
{	//打开,返回hCardOpenObj
	HANDLE (*lpInit)( DWORD dwContext );
	VOID (*lpDeinit)( HANDLE hInitHandle );
	HANDLE (*lpOpen)( HANDLE hInitHandle );
	//关闭
	BOOL (*lpClose)( HANDLE hCardOpenObj );
	//查找符合要求的第一条记录
    BOOL   (*lpFindFirstRecord)( HANDLE hCardOpenObj, LPCTSTR lpszSearchMask, UINT uiMaskType, LPVOID lpvIDData );
	//查找符合要求下一条记录
	BOOL (*lpFindNextRecord)(  HANDLE hCardOpenObj, LPCTSTR lpszSearchMask, UINT uiMaskType, LPVOID lpvIDData, UINT uMethod );
	//删除一条记录
	BOOL (*lpDeleteRecord)( HANDLE hCardOpenObj, UINT index );
	//保存一条记录
	BOOL (*lpSaveRecord)( HANDLE hCardOpenObj, UINT nSavePosIndex, LPCVOID lpvIDData, UINT * lpRealSavePosIndex );
	BOOL (*lpGetInfo)( HANDLE hCardOpenObj, LPTCARD_INFO lptci );
	BOOL (*lpGetRecordInfo)( HANDLE hCardOpenObj, UINT nPosIndex, LPVOID lpvIDData );
	//与卡片的交互动作,比如刷新，排序等等
	BOOL (*lpIoControl)( HANDLE hCardOpenObj, UINT uMsg, WPARAM wParam, LPARAM lParam );
}TCARDDRV, FAR * LPTCARDDRV;

//初始化卡片
BOOL TCARD_Init( VOID );

VOID TCARD_Deinit( VOID );

//打开卡片
BOOL TCARD_Open( VOID );
//关闭卡片
BOOL TCARD_Close( VOID );

//搜索匹配的记录
BOOL TCARD_FindFirstRecord(
							   UINT uiCardID, 
							   LPCTSTR lpszSearchMask,
							   UINT uiMaskType,
							   LPVOID lpvIDData
							  );
//查找下一条记录
#define FNRM_NEXT    0			//向下查
#define FNRM_PREV    0x1		//向上查

BOOL TCARD_FindNextRecord( 
							 UINT uiCardID, 
							 LPCTSTR lpszSearchMask,
							 UINT uiMaskType,
							 LPVOID lpvIDData,
							 UINT uMethod
							 );
//删除一条记录
BOOL TCARD_DeleteRecord(   
						   UINT uiCardID, 
						   UINT index
						  );
//保存一条记录，如果nPosIndex为-1，以为不关位置（由卡片自己决定）
// lpOutIndex 用于接受实际保存的位置，可以为NULL
BOOL TCARD_SaveRecord(  
						 UINT uiCardID,
						 UINT nPosIndex,
						 LPCVOID lpvIDData,
						 UINT * lpOutIndex
						);

//统计卡片的信息
BOOL TCARD_GetInfo(
					 UINT uiCardID,
					 LPTCARD_INFO lptci
					);

//得到一条记录的信息
BOOL TCARD_GetRecordInfo(
					 UINT uiCardID,
					 UINT nPosIndex,
					 LPVOID lpvIDInfo
					);

//与卡片的交互动作,比如刷新，排序等等

BOOL TCARD_IoControl( UINT uiCardID, UINT uMsg, WPARAM wParam, LPARAM lParam );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif		//__TCARD_H
