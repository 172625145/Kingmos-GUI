/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：MMS管理系统,它主要是用于对MMS的管理,包括对四种类型的MMS的组织,
		  管理,删除,读取,添加... 等操作
		  当前版本的文件读取使用直接存区文件的方法，而不是建立一个索引的方法
		  其中保存文件的方法为保存到一个文件中，或者保存到多个文件中两个方法
		  保存到一个文件使用定义SINGLE_FILE.
		  保存到多个文件使用MULTI_FILE
版本号：1.0.0
开发时期：2004-06-03
作者：陈建明 Jami chen
修改记录：

******************************************************/
#include <EWindows.h>
#include <MMSManage.h>
#include "..\mmsbody\mime_body.h"
#include "..\include\mms_content.h"
#include "..\mmstrans\mmstrans.h"
#include <mms_clt.h>

#define MULTI_FILE
//#define SINGLE_FILE


/***************  全局区 定义， 声明 *****************/
#ifdef MULTI_FILE

#define FIELD_NUM		6
#define FIELD_STATUS		0
#define FIELD_FROMTO		1
#define FIELD_SUBJECT		2
#define FIELD_DATE			3
#define FIELD_CONTENTTYPE	4
#define FIELD_MIME			5
// MMS 数据库定义
// FIELD 0 ： DWORD  -- 定义当前数据的状态
// FIELD 1 ： 发件人 或 收件人-- 该短信有谁发送或将要发送给谁，一般保存电话号码，如果已经定义在联系本，则有用户去查找
// FIELD 2 ： 标题 -- 指出该短信的标题
// FIELD 3 ： 时间 -- MMS的发送时间，保存为一个DWORD值，从1970.1.1 00:00:00 开始计算
// FIELD 4 ： 数据类型 -- Content-type ,当前MMS的信息类型
// FIELD 5 ： 数据部分 -- 如果该MMS已经读取，则存放MMS的BODY，否则存放该MMS的通知消息

#endif

#ifdef SINGLE_FILE
// MMS 数据库定义
// FIELD 0 ： 1 BYTE 定义该彩信的类型 'R' -- 收件箱数据 , 
//									  'T' -- 发件箱数据 , 
//									  'H' -- 已发送的数据, 
//									  'S' -- 保存的数据 。
// FIELD 1 ： DWORD  -- 定义当前数据的状态
// FIELD 2 ： 发件人 或 收件人-- 该短信有谁发送或将要发送给谁，一般保存电话号码，如果已经定义在联系本，则有用户去查找
// FIELD 3 ： 标题 -- 指出该短信的标题
// FIELD 4 ： 时间 -- MMS的发送时间，保存为一个DWORD值，从1970.1.1 00:00:00 开始计算
// FIELD 5 ： 数据类型 -- Content-type ,当前MMS的信息类型
// FIELD 6 ： 数据部分 -- 如果该MMS已经读取，则存放MMS的BODY，否则存放该MMS的通知消息


#define FIELD_NUM			7		// 目前每一个记录有二个字段
#define FIELD_TYPE			0
#define FIELD_STATUS		1
#define FIELD_FROMTO		2
#define FIELD_SUBJECT		3
#define FIELD_DATE			4
#define FIELD_CONTENTTYPE	5
#define FIELD_MIME			6

#endif

#define MMS_MAXSIZE  (50 * 1024)  // 目前中国移动定义的最大彩信大小为50K

// MMS管理结构
typedef struct structMMSManage{
	UINT  uMMSType; //当前MMS的管理类型
	HTABLE hTable;
#ifdef SINGLE_FILE
	TCHAR	chType;
#endif
}MMSMANAGE, *LPMMSMANAGE;

#ifdef MULTI_FILE
#define MMS_RECEIVE_FILE	"\\Mobile\\MMS\\Receive.dat"  // 收件箱
#define MMS_SEND_FILE		"\\Mobile\\MMS\\Send.dat"     // 发件箱
#define MMS_SENT_FILE		"\\Mobile\\MMS\\Sent.dat"     // 已发信息
#define MMS_SAVED_FILE		"\\Mobile\\MMS\\Saved.dat"    // 保存信息

#define RECORDNUM_RECEIVE	20		// 收件箱可存入的最大记录条目数
#define RECORDNUM_SEND		 5		// 发件箱可存入的最大记录条目数
#define RECORDNUM_SENT		10		// 已发送可存入的最大记录条目数
#define RECORDNUM_SAVED		20		// 保存箱可存入的最大记录条目数
#endif

#ifdef SINGLE_FILE
#define MMS_DATA_FILE	"\\Mobile\\MMS\\MMS.dat"  //  彩信数据库
#define RECORDNUM			30		// 可以保存的最大记录条目数
#endif


#define TYPENUM				4       // 目前有四种不同类型的数据


#define DATA_VALID		0x80000000  // 数据有效


// ***************************************************************************************
// 函数定义
// ***************************************************************************************
static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize);
static BOOL WriteDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize);
static void TransDate(LPSYSTEMTIME lpMMSTime,DWORD dwSecs);
static LONG EncodeDateValue( SYSTEMTIME *lpSystemTime);
static BOOL	File_IsExist( LPCTSTR lpFileName );
static int SaveMessageToTable(LPMMS_CONTENT lpMMS_Content,UINT uMessageType);
static UINT GetFieldSize(HTABLE hTable,UINT uField);
static void CreateDefaultFileBox(LPCTSTR lpTableName,UINT uRecordNum);
static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName);
static void GetFileType(LPTSTR lpFullFileName,LPTSTR lpContentType);

static void ReleasePart(LPMIME_PART lpMIME_Part);
static LPTSTR BufferAllocCopy(LPTSTR pSrc);

static void WriteMessageToTable(HTABLE hTable,LPMMS_CONTENT lpMMS_Content,UINT uMessageType);

static int WINAPI SendMMSThread(LPVOID lParam );

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );


// **************************************************
// mms 监视窗口
// **************************************************
static TCHAR classMMSMonitorWindow[] = "MMSMONITORWINDOW";

static HWND g_hMonitorWnd = NULL;

#define WM_NEW_MMS_PUSH		(WM_USER + 629)

static ATOM RegisterMMSMonitorWindow(HINSTANCE hInstance);
static LRESULT CALLBACK MonitorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT NewMMSArrive(HWND hWnd,WPARAM wParam,LPARAM lParam);


// ***************************************************************************************
// 声明：HMMS OpenMMS(UINT uMMSType)
// 参数：
//	IN uMMSType -- 指定的MMS类型

// 返回值：
//	返回一个MMS句柄
// 功能描述：
//  	打开一个MMS服务，以后可以使用该句柄对MMS进行访问
// ***************************************************************************************
HMMS OpenMMS(UINT uMMSType)
{
	LPMMSMANAGE lpMMSManage;
	LPTSTR lpDataBaseName;
	static int iCount = 0;

		lpMMSManage = (LPMMSMANAGE)malloc(sizeof(MMSMANAGE)); // 分配一个MMS管理结构的缓存

		if (lpMMSManage == NULL)
		{
			// 分配失败
			SetLastError(MMS_ERROR_NOMEMORY); // 系统内存不足
			return NULL;
		}

#ifdef MULTI_FILE
		switch(uMMSType)
		{
			case MMS_RECEIVE:	//	收件箱
				lpDataBaseName = MMS_RECEIVE_FILE; //得到接收到MMS的数据库文件名
				break;
			case MMS_SEND:		//  发件箱
				lpDataBaseName = MMS_SEND_FILE; //得到未发送MMS的数据库文件名
				break;
			case MMS_SENT:		//	已发信息
				lpDataBaseName = MMS_SENT_FILE; //得到已发送MMS的数据库文件名
				break;
			case MMS_SAVED:		//  存档文件夹
				lpDataBaseName = MMS_SAVED_FILE; //得到存档MMS的数据库文件名
				break;
			default: // 不认识的MMS类型
				free(lpMMSManage);
				SetLastError(MMS_ERROR_UNKNOWMMSTYPE); // 不认识的MMS类型
				return NULL;
		}
#endif
#ifdef SINGLE_FILE
		switch(uMMSType)
		{
			case MMS_RECEIVE:	//	收件箱
				lpMMSManage->chType = 'R'; //得到接收到MMS的数据库文件名
				break;
			case MMS_SEND:		//  发件箱
				lpMMSManage->chType = 'T'; //得到未发送MMS的数据库文件名
				break;
			case MMS_SENT:		//	已发信息
				lpMMSManage->chType = 'H'; //得到已发送MMS的数据库文件名
				break;
			case MMS_SAVED:		//  存档文件夹
				lpMMSManage->chType = 'S'; //得到存档MMS的数据库文件名
				break;
			default: // 不认识的MMS类型
				free(lpMMSManage);
				SetLastError(MMS_ERROR_UNKNOWMMSTYPE); // 不认识的MMS类型
				return NULL;
		}
	
		lpDataBaseName = MMS_DATA_FILE; //得到存档MMS的数据库文件名
#endif
		
		// 内存分配成功
		iCount ++;
		lpMMSManage->uMMSType = uMMSType; // 保存当前的类型
		lpMMSManage->hTable = CreateTable(lpDataBaseName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
		if ( lpMMSManage->hTable == INVALID_HANDLE_VALUE )
		{  // 打开数据库失败
			DWORD dwError = GetLastError();
			RETAILMSG( 1, ( "OpenTable(%s):%d Times Failure\r\n",lpDataBaseName,iCount) );
//			Sleep(1000);
			free(lpMMSManage); // 释放内存
			SetLastError(MMS_ERROR_INVALIDTABLE);  // 错误打开数据库
			return NULL;
		}


		return (HMMS)lpMMSManage; //成功,返回句柄
}


// ***************************************************************************************
// 声明：void CloseMMS(HMMS hMMS)
// 参数：
//    hMMS -- 要关闭的MMS句柄
// 返回值：
//	无
// 功能描述：
//  	关闭一个MMS服务，以后就不能对MMS进行访问
// ***************************************************************************************
void CloseMMS(HMMS hMMS)
{
	LPMMSMANAGE lpMMSManage;

		if (hMMS == NULL)
		{  // 错误的MMS句柄
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return;
		}

		lpMMSManage = (LPMMSMANAGE)hMMS; // 得到MMS管理结构的指针

		if ( lpMMSManage->hTable != INVALID_HANDLE_VALUE )
		{
			CloseTable(lpMMSManage->hTable);
		}

		free(lpMMSManage); // 释放内存
}


// ***************************************************************************************
// 声明：UINT GetMMSCount(HMMS hMMS);
// 参数：
//    hMMS -- 已经打开的MMS句柄
// 返回值：
//		返回当前系统中的MMS的数目(包括已经下载的和没有下载的)
// 功能描述：
//  	得到当前系统中MMS的数目
// ***************************************************************************************
UINT GetMMSCount(HMMS hMMS)
{
	LPMMSMANAGE lpMMSManage;
	UINT uCount = 0;
#ifdef SINGLE_FILE
	FINDSTRUCT fs;
	HANDLE hFind;
#endif

		if (hMMS == NULL)
		{  // 错误的MMS句柄
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return 0;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // 得到MMS管理结构的指针
#ifdef MULTI_FILE
		uCount = CountRecord(lpMMSManage->hTable);
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //查找第一个当前类型的数据
		if( hFind != INVALID_HANDLE_VALUE )
		{
			do{
				//do somthing with record
				uCount ++;
			}while( FindNextRecord( hFind, &fs ) );  // 查找下一个当前类型的数据
			CloseTableFind( hFind ); // close find handle
		}
#endif

		return uCount; // 返回当前的记录数目
}

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
BOOL GetMMSInfo(HMMS hMMS, UINT uIndex,LPMMSINFO lpMMSInfo)
{
	LPMMSMANAGE lpMMSManage;
	UINT dwSecs;
	UINT uRecordIndex;
#ifdef SINGLE_FILE
	FINDSTRUCT fs;
	HANDLE hFind;
#endif

		if (hMMS == NULL)
		{  // 错误的MMS句柄
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return FALSE;
		}

		if (lpMMSInfo->uSize != sizeof(MMSINFO))
		{
			SetLastError(MMS_ERROR_INVALIDPARAMETER);  // 错误参数
			return FALSE;
		}

		lpMMSManage = (LPMMSMANAGE)hMMS; // 得到MMS管理结构的指针
#ifdef MULTI_FILE
		uRecordIndex  = uIndex;
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //查找第一个当前类型的数据
		if( hFind != INVALID_HANDLE_VALUE )
		{
			while( 1 )
			{
				//do somthing with record
				if ((uIndex + 1) == fs.nRecord)
				{
					uRecordIndex  = fs.nRecord -1; // 已经找到当前的记录
					break;
				}
				if (FindNextRecord( hFind, &fs ) == 0) // 查找下一个当前类型的数据
				{
					// 已经没有数据了
					SetLastError(MMS_ERROR_INVALIDINDEX);
					return FALSE;
				}
			}  
			CloseTableFind( hFind ); // close find handle
		}
#endif


		if (SetRecordPointer(lpMMSManage->hTable, uRecordIndex, SRP_BEGIN) == 0xffff) // 设置到用户指定的索引
		{
			SetLastError(MMS_ERROR_INVALIDINDEX);
			return FALSE;
		}
		
		if (lpMMSInfo->uMask & INFO_FROMTO)
		{ 
			// 需要FROMTO
			ReadDataField(lpMMSManage->hTable,FIELD_FROMTO,(void *)lpMMSInfo->lpFromTo,(WORD)lpMMSInfo->uMaxFromTo);  //读取发件人/收件人信息
		}

		if (lpMMSInfo->uMask & INFO_SUBJECT)
		{
			ReadDataField(lpMMSManage->hTable,FIELD_SUBJECT,(void *)lpMMSInfo->lpSubject,(WORD)lpMMSInfo->uMaxSubject);  //读取标题信息
		}

		if (lpMMSInfo->uMask & INFO_DATE)
		{
			ReadDataField(lpMMSManage->hTable,FIELD_DATE,(void *)&dwSecs,sizeof(DWORD));  //读取时间信息
			TransDate(&lpMMSInfo->MMSTime,dwSecs); // 转换为当前的时间
		}

		if (lpMMSInfo->uMask & INFO_STATUS)
		{
			ReadDataField(lpMMSManage->hTable,FIELD_STATUS,(void *)&lpMMSInfo->uStatus,(WORD)sizeof(DWORD));  //读取信息状态
		}

		return TRUE;
}

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
BOOL DeleteMMS(HMMS hMMS, UINT uIndex)
{
	LPMMSMANAGE lpMMSManage;
	UINT uRecordIndex;
#ifdef SINGLE_FILE
	FINDSTRUCT fs;
	HANDLE hFind;
#endif

		if (hMMS == NULL)
		{  // 错误的MMS句柄
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return FALSE;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // 得到MMS管理结构的指针

#ifdef MULTI_FILE
		uRecordIndex  = uIndex;
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //查找第一个当前类型的数据
		if( hFind != INVALID_HANDLE_VALUE )
		{
			while( 1 )
			{
				//do somthing with record
				if ((uIndex + 1) == fs.nRecord)
				{
					uRecordIndex  = fs.nRecord -1; // 已经找到当前的记录
					break;
				}
				if (FindNextRecord( hFind, &fs ) == 0) // 查找下一个当前类型的数据
				{
					// 已经没有数据了
					return FALSE;
				}
			}  
			CloseTableFind( hFind ); // close find handle
		}
#endif

		SetRecordPointer(lpMMSManage->hTable, uRecordIndex, SRP_BEGIN); // 设置到用户指定的索引

		DeleteRecord(lpMMSManage->hTable); //删除当前的记录


		return TRUE;
}
// ***************************************************************************************
// 声明：BOOL ClearMMS(HMMS hMMS);
// 参数：
//   IN  hMMS -- 已经打开的MMS句柄
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	删除所有的MMS
// ***************************************************************************************
BOOL ClearMMS(HMMS hMMS)
{
	LPMMSMANAGE lpMMSManage;
	UINT nCount;
		if (hMMS == NULL)
		{  // 错误的MMS句柄
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return FALSE;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // 得到MMS管理结构的指针

		nCount = GetMMSCount(hMMS);
		while(nCount)
		{
			SetRecordPointer(lpMMSManage->hTable, nCount - 1, SRP_BEGIN); // 设置到最后一条记录的索引
			DeleteRecord(lpMMSManage->hTable); // 删除最后一条记录
			nCount --; // 删除前一条记录
		}
		return TRUE;
}
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
HMMSDATA LoadMMSData(HMMS hMMS, UINT uIndex)
{
	LPMMSMANAGE lpMMSManage;
	LPMMS_CONTENT lpMMS_Content = NULL;
	UINT uRecordIndex;
	UINT uFieldSize;
	UINT uStatus;
#ifdef SINGLE_FILE
	FINDSTRUCT fs;
	HANDLE hFind;
#endif

		if (hMMS == NULL)
		{  // 错误的MMS句柄
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return NULL;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // 得到MMS管理结构的指针

		// 首先得到当前的状态,如果是一个未读的MMS,则先去得到当前的MMS,然后在装载当前的MMS
#ifdef MULTI_FILE
		uRecordIndex  = uIndex;
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //查找第一个当前类型的数据
		if( hFind != INVALID_HANDLE_VALUE )
		{
			while( 1 )
			{
				//do somthing with record
				if ((uIndex + 1) == fs.nRecord)
				{
					uRecordIndex  = fs.nRecord -1; // 已经找到当前的记录
					break;
				}
				if (FindNextRecord( hFind, &fs ) == 0) // 查找下一个当前类型的数据
				{
					// 已经没有数据了
					return FALSE;
				}
			}  
			CloseTableFind( hFind ); // close find handle
		}
#endif

		SetRecordPointer(lpMMSManage->hTable, uRecordIndex, SRP_BEGIN); // 设置到用户指定的索引

		lpMMS_Content = (LPMMS_CONTENT)malloc(sizeof(MMS_CONTENT)); // 分配一个MMS数据的结构缓存
		if (lpMMS_Content == NULL)
		{ // 分配失败
			SetLastError(MMS_ERROR_NOMEMORY); // 系统内存不足
			goto ERROR_RETURN;
		}

		// 初始化结构
		memset(lpMMS_Content,0,sizeof(MMS_CONTENT));  // 清空缓存

		ReadDataField(lpMMSManage->hTable,FIELD_STATUS,(void *)&uStatus,(WORD)sizeof(DWORD));  //读取信息状态

		if ((uStatus & 0xffff) == MMS_NOTREAD)
		{  // 当前数据还没有读取
			if (lpMMSManage->uMMSType == MMS_RECEIVE)
			{ // 当前是收件箱，该MMS还没有读取,先读取MMS		
				LPTSTR lpData;

					// 首先读取通知消息的数据
					uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_MIME);
					lpData = (LPVOID)malloc(uFieldSize);

					if (lpData == NULL)
					{ // 分配失败
						SetLastError(MMS_ERROR_NOMEMORY); // 系统内存不足
						goto ERROR_RETURN;
					}
					ReadDataField(lpMMSManage->hTable,FIELD_MIME,(void *)lpData,(WORD)uFieldSize);  //读取MMS通知包的数据

					if (GetMMSFromServer(lpData, uFieldSize,lpMMS_Content) == FALSE)  // 读取彩信数据
					{
						// 得到彩信数据失败
						SetLastError(MMS_ERROR_READMMSFAILURE); // 系统内存不足
						goto ERROR_RETURN;
					}
					// 保存当前得到的彩信到文件
					WriteMessageToTable(lpMMSManage->hTable,lpMMS_Content,MMS_RECEIVE);
					
					return (HMMSDATA)lpMMS_Content; // 返回得到的彩信
			}
			else
			{
				// 其他类型的彩信
				uStatus &= 0xffff0000;
			    uStatus |=MMS_READ; // 设置彩信已读
			    WriteDataField(lpMMSManage->hTable,FIELD_STATUS,(void *)&uStatus,(WORD)sizeof(DWORD));  //读取信息状态
			}
		}


		// 需要FROMTO
		uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_FROMTO);
		lpMMS_Content->lpFromTo = (LPTSTR)malloc(uFieldSize);
		if (lpMMS_Content->lpFromTo == NULL)
		{ // 分配失败
			SetLastError(MMS_ERROR_NOMEMORY); // 系统内存不足
			goto ERROR_RETURN;
		}
		ReadDataField(lpMMSManage->hTable,FIELD_FROMTO,(void *)lpMMS_Content->lpFromTo,(WORD)uFieldSize);  //读取发件人/收件人信息

		uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_SUBJECT);
		lpMMS_Content->lpSubject = (LPTSTR)malloc(uFieldSize);
		if (lpMMS_Content->lpSubject == NULL)
		{ // 分配失败
			SetLastError(MMS_ERROR_NOMEMORY); // 系统内存不足
			goto ERROR_RETURN;
		}
		ReadDataField(lpMMSManage->hTable,FIELD_SUBJECT,(void *)lpMMS_Content->lpSubject,(WORD)uFieldSize);  //读取标题信息

		ReadDataField(lpMMSManage->hTable,FIELD_DATE,(void *)&lpMMS_Content->dwDate,sizeof(DWORD));  //读取时间信息

		//  读取彩信数据类型
		uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_CONTENTTYPE);
		if (uFieldSize != 0)
		{
			lpMMS_Content->lpContent_Type = (LPTSTR)malloc(uFieldSize);
			if (lpMMS_Content->lpContent_Type == NULL)
			{ // 分配失败
				SetLastError(MMS_ERROR_NOMEMORY); // 系统内存不足
				goto ERROR_RETURN;
			}
			ReadDataField(lpMMSManage->hTable,FIELD_CONTENTTYPE,(void *)lpMMS_Content->lpContent_Type,(WORD)uFieldSize);  //读取内容类型
		}
		else
		{
			lpMMS_Content->lpContent_Type = NULL;
		}

		//  读取彩信数据
		uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_MIME);
		lpMMS_Content->iDataSize = uFieldSize;
		lpMMS_Content->lpData = (LPVOID)malloc(lpMMS_Content->iDataSize);
		if (lpMMS_Content->lpData == NULL)
		{ // 分配失败
			SetLastError(MMS_ERROR_NOMEMORY); // 系统内存不足
			goto ERROR_RETURN;
		}
		ReadDataField(lpMMSManage->hTable,FIELD_MIME,(void *)lpMMS_Content->lpData,(WORD)lpMMS_Content->iDataSize);  //读取MMS数据

		return (HMMSDATA)lpMMS_Content;

ERROR_RETURN:

		if (lpMMS_Content)
		{
			if (lpMMS_Content->lpContent_Type)
				free(lpMMS_Content->lpContent_Type);  // 释放内容类型缓存

			if (lpMMS_Content->lpData)
				free(lpMMS_Content->lpData);  // 释放数据缓存

			if (lpMMS_Content->lpFromTo)
				free(lpMMS_Content->lpFromTo);   // 释放发件人缓存

			if (lpMMS_Content->lpSubject)
				free(lpMMS_Content->lpSubject);   // 释放标题缓存

			free(lpMMS_Content);  // 释放数据结构缓存
		}
		return NULL;
}

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
BOOL ReleaseMMSData(HMMS hMMS, HMMSDATA hMMSData)
{
	LPMMSMANAGE lpMMSManage;
	LPMMS_CONTENT lpMMS_Content;

		if (hMMS == NULL || hMMSData == NULL)
		{  // 错误的MMS句柄
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return FALSE;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // 得到MMS管理结构的指针
		lpMMS_Content = (LPMMS_CONTENT)hMMSData; // 得到MMS数据结构指针

		if (lpMMS_Content)
		{
			if (lpMMS_Content->lpContent_Type)
				free(lpMMS_Content->lpContent_Type);  // 释放内容类型缓存

			if (lpMMS_Content->lpData)
				free(lpMMS_Content->lpData);  // 释放数据缓存

			if (lpMMS_Content->lpFromTo)
				free(lpMMS_Content->lpFromTo);   // 释放发件人缓存

			if (lpMMS_Content->lpSubject)
				free(lpMMS_Content->lpSubject);   // 释放标题缓存

			free(lpMMS_Content);  // 释放数据结构缓存
		}

		return TRUE;
}

// ***************************************************************************************
// 声明：static void TransDate(LPSYSTEMTIME lpMMSTime,DWORD dwDate)
// 参数：
//	OUT  lpMMSTime -- 得到当前的时间
//  IN  dwDate -- 从1970.01.01 00:00:00 开始的秒数
// 返回值：
//		无
// 功能描述：
//		转换为当前的时间
// ***************************************************************************************
static void TransDate(LPSYSTEMTIME lpMMSTime,DWORD dwSecs)
{
	UINT iDays; 
		// 初始化起始时间 1970 - 01 - 01 , 00:00:00
		lpMMSTime->wYear = 1970;
		lpMMSTime->wMonth = 1;
		lpMMSTime->wDay = 1;
		lpMMSTime->wHour = 0;
		lpMMSTime->wMinute = 0;
		lpMMSTime->wSecond = 0;
		lpMMSTime->wMilliseconds = 0;



		iDays = dwSecs / (3600 * 24) ; // 得到从1970-01-01 开始计算起的天数
		dwSecs %= (3600 * 24) ; // 得到除去天数后的秒数，用来计算时，分，秒

		DateAddDay(lpMMSTime,iDays); // 得到当前的日期

		// 得到当前的时间
		lpMMSTime->wHour = (WORD)dwSecs / 3600; // 得到小时数
		dwSecs %= 3600;  // 得到除去小时后的秒数

		lpMMSTime->wMinute = (WORD)dwSecs / 60; // 得到分数
		dwSecs %= 60;  // 得到除去分数后的秒数

		lpMMSTime->wSecond = (WORD)dwSecs; // 得到秒数

}

// ***************************************************************************************
// 声明：BOOL InitialMMSSystem(HINSTANCE hInstance)
// 参数：
// IN hInstance -- 当前进行初始化彩信的实例句柄
//
// 返回值：
//		无
// 功能描述：
//		初始化MMS系统
// ***************************************************************************************
BOOL InitialMMSSystem(void)
{
	HINSTANCE hInstance;
//	INITLOCK(); // 初始化临界变量
	
	if (g_hMonitorWnd)
		return TRUE; // 已经初始化完成

	RETAILMSG( 1, ( "Initialize folder\r\n") );
	// 初始化数据库
	if (File_IsExist( "\\Mobile" ) == FALSE )
	{
		// Mobile 目录不存在，创建Mobile
		CreateDirectory("\\Mobile",NULL);  // 创建目录
	}

	if (File_IsExist( "\\Mobile\\MMS" ) == FALSE )
	{
		// \Mobile\mms 目录不存在，创建Mobile\mms
		CreateDirectory("\\Mobile\\MMS",NULL);  // 创建目录
	}

	if (File_IsExist( "\\Mobile\\temp" ) == FALSE )
	{
		// \Mobile\temp 目录不存在，创建Mobile\temp
		CreateDirectory("\\Mobile\\temp",NULL);  // 创建目录
	}

	RETAILMSG( 1, ( "Initialize folder OK !!!\r\n") );
#ifdef MULTI_FILE
	if (File_IsExist( MMS_RECEIVE_FILE ) == FALSE )
	{
		// 收件箱不存在
		CreateDefaultFileBox(MMS_RECEIVE_FILE,RECORDNUM_RECEIVE); // 创建默认收件箱
	}

	if (File_IsExist( MMS_SEND_FILE ) == FALSE )
	{
		// 发件箱不存在
		CreateDefaultFileBox(MMS_SEND_FILE,RECORDNUM_SEND); // 创建默认收件箱
	}

	if (File_IsExist( MMS_SENT_FILE ) == FALSE )
	{
		// 发件箱不存在
		CreateDefaultFileBox(MMS_SENT_FILE,RECORDNUM_SENT); // 创建默认收件箱
	}

	if (File_IsExist( MMS_SAVED_FILE ) == FALSE )
	{
		// 发件箱不存在
		CreateDefaultFileBox(MMS_SAVED_FILE,RECORDNUM_SAVED); // 创建默认收件箱
	}
#endif

#ifdef SINGLE_FILE
	if (File_IsExist( MMS_DATA_FILE ) == FALSE )
	{
		// 数据库不存在
		RETAILMSG( 1, ( "Need CreateDefaultFileBox ...\r\n") );
		CreateDefaultFileBox(MMS_DATA_FILE,RECORDNUM); // 创建默认收件箱
		RETAILMSG( 1, ( "CreateDefaultFileBox OK !!!\r\n") );
	}
#endif

//	CreateThread( NULL, 0, SendMMSThread, NULL, 0, NULL );

	hInstance = GetModuleHandle(NULL);

	RegisterMMSMonitorWindow(hInstance);
	// 创建一个彩信接收监测窗口
	g_hMonitorWnd = CreateWindow( 
		          classMMSMonitorWindow, 
				  "",
				  WS_POPUP,//|WS_CAPTION|WS_SYSMENU,
		          0, 0, 1, 1,
				  0,
				  0,
				  hInstance,
				  0 );
  
	if (g_hMonitorWnd == NULL)
		return FALSE;

	MCltSetPushCallback( g_hMonitorWnd, WM_NEW_MMS_PUSH ); // 设置彩信通知回调窗口和消息

	return TRUE;
}

// ***************************************************************************************
// 声明：BOOL DeinitialMMSSystem(void)
// 参数：
//
// 返回值：
//		无
// 功能描述：
//		Deinitial MMS系统
// ***************************************************************************************
BOOL DeinitialMMSSystem(void)
{
	return TRUE;
}

// ***************************************************************************************
// 声明：void GetNewMessage(LPMMS_CONTENT lpMMS_Content)
// 参数：
//	IN lpMMS_Content -- 得到的新的彩信
//
// 返回值：
//		无
// 功能描述：
//		得到一个新的彩信
// ***************************************************************************************
void GetNewMessage(LPMMS_CONTENT lpMMS_Content)
{
	// 得到一个新的短消息

	SaveMessageToTable(lpMMS_Content,MMS_RECEIVE);
}

// ***************************************************************************************
// 声明：static void SaveMessageToTable(LPMMS_CONTENT lpMMS_Content,UINT uMessageType)
// 参数：
//	IN lpMMS_Content -- 要保存的彩信
//	IN bNewMessage -- 是否是新的彩信
// 返回值：
//		无
// 功能描述：
//		保存一个的彩信
// ***************************************************************************************
int SaveMessageToTable(LPMMS_CONTENT lpMMS_Content,UINT uMessageType)
{
	UINT uStatus = 0;
	HTABLE hTable = NULL;
//	UINT uRecordIndex;
	FIELDITEM fieldItem[FIELD_NUM];
	static TCHAR lpData[0XE1F];
	int iIndex;

//		RETAILMSG(1,(TEXT("SaveMessageToTable \r\n")));
		// 数据规划		
		//  Field 0 -- |  Flag  | Field 0 Len | Field 1 Len | ... ... | Field n Len | 
		//  Field 1 -- |Field 0 Data | Field 1 Data | ... | Field n Data |

		//  其中 Flag 为当前条目的标志, DWORD (4 BYTE)
		//  flag --  bit 31  -- 当前数据是否有效
		//			 bit 15 -- bit 0 -- 当前记录的状态
		//  Field 0 Len ... Field n Len 为各个字段的长度 每个字段为一个WORD (2 BYTE)
		//  Field 0 Data  ...  Field n Data  为各个字段的数据

#ifdef SINGLE_FILE
		// 彩信类型
		switch(uMessageType)
		{
			case MMS_RECEIVE:	//	收件箱
				fieldItem[FIELD_TYPE].lpData = "R";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SEND:		//  发件箱
				fieldItem[FIELD_TYPE].lpData = "T";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SENT:		//	已发信息
				fieldItem[FIELD_TYPE].lpData = "H";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SAVED:		//  存档文件夹
				fieldItem[FIELD_TYPE].lpData = "S";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			default: // 不认识的MMS类型
				SetLastError(MMS_ERROR_UNKNOWMMSTYPE); // 不认识的MMS类型
				return;
		}
#endif

		// 彩信状态
		uStatus |= MMS_NOTREAD ; //设置MMS未读
		fieldItem[FIELD_STATUS].lpData = &uStatus;
		fieldItem[FIELD_STATUS].size = sizeof(DWORD);


		// 写入字段 0 -- 发件人/收件人
		fieldItem[FIELD_FROMTO].lpData = lpMMS_Content->lpFromTo;
		fieldItem[FIELD_FROMTO].size = strlen(lpMMS_Content->lpFromTo) + 1; // 包含结尾符

		// 写入字段 1 --  标题
		fieldItem[FIELD_SUBJECT].lpData = lpMMS_Content->lpSubject;
		fieldItem[FIELD_SUBJECT].size = strlen(lpMMS_Content->lpSubject) + 1; // 包含结尾符

		// 写入字段 2 --  时间
		fieldItem[FIELD_DATE].lpData = &lpMMS_Content->dwDate;
		fieldItem[FIELD_DATE].size = sizeof(DWORD); 

		// 写入字段 3 --  内容类型
		fieldItem[FIELD_CONTENTTYPE].lpData = lpMMS_Content->lpContent_Type;
		if (lpMMS_Content->lpContent_Type == NULL)
		{
			fieldItem[FIELD_CONTENTTYPE].size = 0; // 包含结尾符
		}
		else
		{
			fieldItem[FIELD_CONTENTTYPE].size = strlen(lpMMS_Content->lpContent_Type) + 1; // 包含结尾符
		}

		// 写入字段 4 --  数据

		fieldItem[FIELD_MIME].lpData = lpMMS_Content->lpData;
		fieldItem[FIELD_MIME].size = lpMMS_Content->iDataSize; // 包含结尾符

#ifdef MULTI_FILE
		switch(uMessageType)
		{
			case MMS_RECEIVE:	//	收件箱
				hTable = CreateTable(MMS_RECEIVE_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				break;
			case MMS_SEND:		//  发件箱
				hTable = CreateTable(MMS_SEND_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				break;
			case MMS_SENT:		//	已发信息
				hTable = CreateTable(MMS_SENT_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				break;
			case MMS_SAVED:		//  存档文件夹
				hTable = CreateTable(MMS_SAVED_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				break;
		}
#endif

#ifdef SINGLE_FILE
		hTable = CreateTable(MMS_DATA_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
#endif
		if ( hTable == INVALID_HANDLE_VALUE )
		{  // 打开数据库失败
			SetLastError(MMS_ERROR_INVALIDTABLE);
			return -1;
		}
		AppendRecord( hTable, (FIELDITEM *)&fieldItem);
		
		iIndex = CountRecord(hTable);
		CloseTable(hTable);

		return iIndex;
}

// ***************************************************************************************
// 声明：static BOOL	File_IsExist( LPCTSTR lpFileName )
// 参数：
//	IN lpFileName -- 要测试的文件名
// 返回值：
//		如果文件存在返回TRUE，否则返回FALSE
// 功能描述：
//		测试一个文件或文件夹是否存在
// ***************************************************************************************
static BOOL	File_IsExist( LPCTSTR lpFileName )
{
	HANDLE			hFindFile;
	FILE_FIND_DATA	FileData;

	hFindFile = FindFirstFile( lpFileName, &FileData );
	if( hFindFile!=INVALID_HANDLE_VALUE )
	{
		FindClose( hFindFile );
		return TRUE;
	}
	return FALSE;
}



// ***************************************************************************************
// 声明：static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
// 参数：
//	IN hTable -- 当前已经打开的的数据库句柄
//	IN uField -- 指定字段
//  OUT lpData -- 数据缓存
//	IN wDataSize --	数据缓存大小
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//		读取数据库当前记录的指定FIELD数据
// ***************************************************************************************
static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
{
	WORD wReadSize;
		wReadSize = ReadField(hTable,uField + 1,0,lpData,wDataSize);  //读取数据
		return TRUE;
}
// ***************************************************************************************
// 声明：static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
// 参数：
//	IN hTable -- 当前已经打开的的数据库句柄
//	IN uField -- 指定字段
//  OUT lpData -- 数据缓存
//	IN wDataSize --	数据缓存大小
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//		读取数据库当前记录的指定FIELD数据
// ***************************************************************************************
static BOOL WriteDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
{
		WriteField(hTable,uField + 1,lpData,wDataSize);  //写入数据
		return TRUE;
}


// ***************************************************************************************
// 声明：static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
// 参数：
//	IN hTable -- 当前已经打开的的数据库句柄
//	IN uField -- 指定字段
//  OUT lpData -- 数据缓存
//	IN wDataSize --	数据缓存大小
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//		读取数据库当前记录的指定FIELD数据
// ***************************************************************************************
static UINT GetFieldSize(HTABLE hTable,UINT uField)
{
	TCHAR lpTemp[sizeof(RECORDINFO) + FIELD_NUM * sizeof(DWORD)];
	RECORDINFO*		lprdInfo = NULL;
	UINT uFieldSize;

		lprdInfo = (LPRECORDINFO)lpTemp; // 分配记录信息结构
		lprdInfo->uiMask = RIM_DELETED | RIM_RECORDSIZE | RIM_FIELDSIZE;
		if( !GetRecordInfo(hTable, lprdInfo) )
		{
			// 得到信息失败
			SetLastError(MMS_ERROR_INVALIDINDEX);  // 无效索引
			return 0;
		}
		uFieldSize = lprdInfo->uiFieldSize[uField];

		return uFieldSize;
}

// ***************************************************************************************
// 声明：static void CreateDefaultFileBox(LPCTSTR lpTableName,UINT uRecordNum)
// 参数：
//	IN lpTableName -- 要创建的的数据库名
//  IN uRecordNum -- 要创建的默认的记录数目
// 返回值：
//		如果文件存在返回TRUE，否则返回FALSE
// 功能描述：
//		创建默认的数据库
// ***************************************************************************************
static void CreateDefaultFileBox(LPCTSTR lpTableName,UINT uRecordNum)
{
	HTABLE hTable;
	UINT  uFileSize;

		uFileSize  = uRecordNum * MMS_MAXSIZE ;
//		RETAILMSG( 1, ( "CreateDefaultFileBox ...\r\n" ) );
		hTable = CreateTableEx(lpTableName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FIELD_NUM,uFileSize); // 创建一个新的数据库
//		hTable = CreateTable(lpTableName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FIELD_NUM); // 创建一个新的数据库
		if ( hTable == INVALID_HANDLE_VALUE )
		{  // 打开数据库失败
			SetLastError(MMS_ERROR_INVALIDTABLE);
			return ;
		}

		RETAILMSG( 1, ( "App OK %d\r\n",CountRecord(hTable)) );
		CloseTable(hTable); // 关闭数据库
		RETAILMSG( 1, ( "CreateDefaultFileBox OK\r\n" ) );

/*		{
			// test
			int i;

			for (i = 0; i < 100; i++)
			{
				hTable = CreateTable(lpTableName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				if ( hTable == INVALID_HANDLE_VALUE )
				{  // 打开数据库失败
					RETAILMSG( 1, ( "OpenTable(%s):%d Times Failure\r\n",lpTableName,i) );
					Sleep(1000);
					SetLastError(MMS_ERROR_INVALIDTABLE);
				//	return ;
				}
				else
					CloseTable(hTable); // 关闭数据库
			}
		}
		*/
}

// ***************************************************************************************
// 声明：HMMSDATA OrganizeMMSData(LPCREATEMESSAGE lpCreateMessage);
// 参数：
//   IN  lpCreateMessage -- 要组织的MMS数据结构
// 返回值：
//		成功返回MMS数据句柄，否则返回NULL
// 功能描述：
//  	组织要创建的MMS数据
// ***************************************************************************************
HMMSDATA OrganizeMMSData(LPCREATEMESSAGE lpCreateMessage)
{
	BODYDETAIL BodyDetail;
	LPMIME_PART lpMIME_Part = NULL;
	LPMIME_PART lpPrePart = NULL;
	DATA_DEAL MMS_Data;
	LPMMS_CONTENT lpMMS_Content = NULL;
	int i;
	static TCHAR lpPart_Location[MAX_PATH];
	static TCHAR lpPart_ID[64];
	static TCHAR lpPart_Desc[64];
	static TCHAR lpPart_Type[64];

	SYSTEMTIME stCurTime;

		GetLocalTime(&stCurTime);

		lpMMS_Content = (LPMMS_CONTENT)malloc(sizeof(MMS_CONTENT));
		if (lpMMS_Content == NULL)
		{
			SetLastError(MMS_ERROR_NOMEMORY);
			goto OG_ERROR_RETURN;
		}
		memset(lpMMS_Content,0,sizeof(MMS_CONTENT));

		lpMMS_Content->lpContent_Type = (LPTSTR)malloc(strlen("application/vnd.wap.multipart.related") + 1);
		if (lpMMS_Content->lpContent_Type == NULL)
		{
			SetLastError(MMS_ERROR_NOMEMORY);
			goto OG_ERROR_RETURN;
		}
		strcpy(lpMMS_Content->lpContent_Type,"application/vnd.wap.multipart.related");

		lpMMS_Content->lpFromTo = malloc(strlen(lpCreateMessage->lpTo) + 1);
		if (lpMMS_Content->lpFromTo == NULL)
		{
			SetLastError(MMS_ERROR_NOMEMORY);
			goto OG_ERROR_RETURN;
		}
		strcpy(lpMMS_Content->lpFromTo,lpCreateMessage->lpTo);   // 发件人/收件人

		lpMMS_Content->lpSubject = malloc(strlen(lpCreateMessage->lpText) + 1);
		if (lpMMS_Content->lpSubject == NULL)
		{
			SetLastError(MMS_ERROR_NOMEMORY);
			goto OG_ERROR_RETURN;
		}
		strcpy(lpMMS_Content->lpSubject,lpCreateMessage->lpText);  // 标题

//		DWORD dwDate;  // 彩信日期
		lpMMS_Content->dwDate = EncodeDateValue(&stCurTime);

		lpMMS_Content->lpData = (LPBYTE)malloc(MMS_MAXSIZE);
		if (lpMMS_Content->lpData == NULL)
		{
			// 分配内存失败
			SetLastError(MMS_ERROR_NOMEMORY);
			goto OG_ERROR_RETURN;
		}

		BodyDetail.iPartNum = lpCreateMessage->uAttachNum;  // 得到当前有多少个附件
		

		MMS_Data.dwLenTotal = MMS_MAXSIZE;
		MMS_Data.dwLenDealed = 0;
		MMS_Data.lpData = lpMMS_Content->lpData;

		BodyDetail.lpMIME_Part = NULL;  
		for (i = 0; i < BodyDetail.iPartNum ; i++)
		{
			lpMIME_Part = (LPMIME_PART)malloc( sizeof(MIME_PART)  ); // 分配一个空间来存放当前的PART
			if (lpMIME_Part == NULL)
			{
					SetLastError(MMS_ERROR_NOMEMORY);
					goto OG_ERROR_RETURN;
			}

			memset(lpMIME_Part,0,sizeof(MIME_PART)); // 初始化结构
			if (BodyDetail.lpMIME_Part == NULL)
				BodyDetail.lpMIME_Part = lpMIME_Part;   // 设置第一个PART

			if (lpPrePart)
				lpPrePart->next = lpMIME_Part;  // 将当前part连接到最后一个PART

			strcpy(lpMIME_Part->lpPath, lpCreateMessage->lpAttachFile[i]);

			GetPureFileName(lpMIME_Part->lpPath,lpPart_Location);  // 得到文件名
			lpMIME_Part->lpContent_Location = BufferAllocCopy(lpPart_Location); // 设置文件名

			sprintf(lpPart_ID,"Kingmos%d",i + 1);  // 得到ID
			lpMIME_Part->lpContent_ID = BufferAllocCopy(lpPart_ID);

			GetFileType(lpMIME_Part->lpPath,lpPart_Type);//得到内容类型
			lpMIME_Part->lpContent_Type = BufferAllocCopy(lpPart_Type);

			sprintf(lpPart_Desc,"Kingmos file %d",i + 1);
			lpMIME_Part->lpContent_Description = BufferAllocCopy(lpPart_Desc); // 设置说明

			lpMIME_Part->dwDate = EncodeDateValue(&stCurTime);

			lpMIME_Part->lpContent_Url = BufferAllocCopy("www.mlg.com");  // 设置URL

			lpMIME_Part->uMask = 0xff;
			lpMIME_Part->next = NULL;
			lpPrePart = lpMIME_Part; 
		}

		if (MIME_Pack(&MMS_Data,&BodyDetail) == FALSE)
		{
			SetLastError(MMS_ERROR_ATTACHFLOW);
			goto OG_ERROR_RETURN;
		}
		lpMMS_Content->lpData = MMS_Data.lpData;
		lpMMS_Content->iDataSize = MMS_Data.dwLenDealed;

		if (lpMIME_Part)
			ReleasePart(lpMIME_Part);   // 释放

		return lpMMS_Content;

OG_ERROR_RETURN:

		if (lpMMS_Content)
		{
			if (lpMMS_Content->lpContent_Type)
				free(lpMMS_Content->lpContent_Type);  // 释放内容类型缓存

			if (lpMMS_Content->lpData)
				free(lpMMS_Content->lpData);  // 释放数据缓存

			if (lpMMS_Content->lpFromTo)
				free(lpMMS_Content->lpFromTo);   // 释放发件人缓存

			if (lpMMS_Content->lpSubject)
				free(lpMMS_Content->lpSubject);   // 释放标题缓存

			free(lpMMS_Content);  // 释放数据结构缓存
		}

		if (lpMIME_Part)
			ReleasePart(lpMIME_Part);   // 释放

		return NULL;

}

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
int SaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex)
{
	LPMMS_CONTENT lpMMS_Content;

	lpMMS_Content = (LPMMS_CONTENT)hMMSData;
	if (lpMMS_Content == NULL)
	{
		SetLastError(MMS_ERROR_INVALIDHANDLE);
		return -1;
	}
	if (iIndex == -1)
	{
		return SaveMessageToTable(lpMMS_Content,uMMSType);
	}
	else
	{
		// 将要替换原来的索引记录
		HTABLE hTable;
		UINT uRecordIndex;
#ifdef SINGLE_FILE
		FINDSTRUCT fs;
		HANDLE hFind;
#endif
#ifdef MULTI_FILE
		switch(uMMSType)
		{
			case MMS_RECEIVE:	//	收件箱
				hTable = CreateTable(MMS_RECEIVE_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				break;
			case MMS_SEND:		//  发件箱
				hTable = CreateTable(MMS_SEND_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				break;
			case MMS_SENT:		//	已发信息
				hTable = CreateTable(MMS_SENT_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				break;
			case MMS_SAVED:		//  存档文件夹
				hTable = CreateTable(MMS_SAVED_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
				break;
		}
#endif

#ifdef SINGLE_FILE
		hTable = CreateTable(MMS_DATA_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // 打开数据库
#endif
		if ( hTable == INVALID_HANDLE_VALUE )
		{  // 打开数据库失败
			SetLastError(MMS_ERROR_INVALIDTABLE);
			return -1;
		}
#ifdef MULTI_FILE
		uRecordIndex  = iIndex;
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //查找第一个当前类型的数据
		if( hFind != INVALID_HANDLE_VALUE )
		{
			while( 1 )
			{
				//do somthing with record
				if ((iIndex + 1) == fs.nRecord)
				{
					uRecordIndex  = fs.nRecord -1; // 已经找到当前的记录
					break;
				}
				if (FindNextRecord( hFind, &fs ) == 0) // 查找下一个当前类型的数据
				{
					// 已经没有数据了
					SetLastError(MMS_ERROR_INVALIDINDEX);
					return FALSE;
				}
			}  
			CloseTableFind( hFind ); // close find handle
		}
#endif

		WriteMessageToTable(hTable,lpMMS_Content,MMS_RECEIVE);
		CloseTable(hTable);
	}
	return iIndex;
}

// ***************************************************************************************
// 声明：BOOL SendMMSMessage(HMMSDATA hMMSData)
// 参数：
//   IN  hMMSData -- 要发送的MMS数据句柄
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	发送当前创建的彩信
// ***************************************************************************************
BOOL SendMMSMessage(HMMSDATA hMMSData)
{

	LPMMS_CONTENT lpMMS_Content;

	lpMMS_Content = (LPMMS_CONTENT)hMMSData;
	if (lpMMS_Content == NULL)
	{
		SetLastError(MMS_ERROR_INVALIDHANDLE);
		return FALSE;
	}
//	SaveMessageToTable(lpMMS_Content,MMS_SEND);
	if (SentMMSToServer(lpMMS_Content) == TRUE)  // 发送彩信
	{
		// 发送成功
		SaveMessageToTable(lpMMS_Content,MMS_SENT); // 将已经发送的彩信存入到已发件箱
	}

	return TRUE;
}

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
int SendAndSaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex)
{
	iIndex = SaveMMSMessage(hMMSData,uMMSType,iIndex);
	SendMMSMessage(hMMSData);
	return iIndex;
}


// **************************************************
// 声明：static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName)
// 参数：
// 	IN lpFullFileName -- 全文件名，包含路径
// 	OUT lpPureFileName -- 纯文件名，不包含路径
// 
// 返回值：无
// 功能描述：从一个全路径的文件名中提取一个不含路径的文件名
// 引用: 
// **************************************************
static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName)
{
	const char *ptr = lpFullFileName;
	int length;

//		ptr=path;
		// Search '.'
		// 将指针指向文件名结尾
		while(*ptr)
		{
			ptr++;
		}
		// Get filename
		length=0;
		while(ptr>lpFullFileName)
		{ // 还没有到达文件头
			ptr--;  // not include '\' and '.'
			if (*ptr=='\\')
			{  // 遇到文件路径分隔
				ptr++;  // not includ符e '\' 
				break; // 退出，后面的就是不包含路径的文件名
			}
			length++;
		}
		if (lpPureFileName)
		{
			strcpy(lpPureFileName,ptr); // 复制纯文件名到缓存
		}
}


// **************************************************
// 声明：static void GetFileType(LPTSTR lpFullFileName,LPTSTR lpContentType)
// 参数：
// 	IN lpFullFileName -- 全文件名，包含路径
// 	OUT lpContentType -- 文件的类型
// 
// 返回值：无
// 功能描述：从一个的文件名得到该文件的文件类型
// 引用: 
// **************************************************
#define MMSTYPE_UNKNOWFILE	0
#define MMSTYPE_TEXTFILE	100
#define MMSTYPE_BMPFILE		101
#define MMSTYPE_GIFFILE		102
#define MMSTYPE_JPGFILE		103
#define MMSTYPE_MP3FILE		104
#define MMSTYPE_3GPFILE		105
#define MMSTYPE_AMRFILE		106
#define MMSTYPE_MIDFILE		107
#define MMSTYPE_WAVFILE		108
#define MMSTYPE_SMILFILE	109

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

const static FILETYPE tabFileType[] = {
	{ "*.txt" , MMSTYPE_TEXTFILE },
	{ "*.bmp" , MMSTYPE_BMPFILE },
	{ "*.jpg" , MMSTYPE_JPGFILE },
	{ "*.gif" , MMSTYPE_GIFFILE },
	{ "*.mp3" , MMSTYPE_MP3FILE },
	{ "*.3gp" , MMSTYPE_3GPFILE },
	{ "*.amr" , MMSTYPE_AMRFILE },
	{ "*.mid" , MMSTYPE_MIDFILE },
	{ "*.wav" , MMSTYPE_WAVFILE },
	{ "*.smil", MMSTYPE_SMILFILE },
};

static void GetFileType(LPTSTR lpFullFileName,LPTSTR lpContentType)
{
	int iFileTypeNum;
	int i;
	UINT idFileType = MMSTYPE_UNKNOWFILE;

		iFileTypeNum = sizeof (tabFileType) / sizeof (FILETYPE); // 得到当前的已知类型数目
		for (i = 0 ; i < iFileTypeNum; i++ )
		{
			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), lpFullFileName, strlen(lpFullFileName)) == TRUE)
			{ // 比较当前文件是否是已知文件
				idFileType = tabFileType[i].iFileType;  // 使用已知文件的图标
				break;
			}
		}

		switch(idFileType)
		{
			case MMSTYPE_TEXTFILE:
				strcpy(lpContentType,"text/plain");
				return;
			case MMSTYPE_GIFFILE:
				strcpy(lpContentType,"image/gif");
				return;
			case MMSTYPE_JPGFILE:
				strcpy(lpContentType,"image/jpeg");
				return;
			case MMSTYPE_SMILFILE:
				strcpy(lpContentType,"application/smil");
				return;
		}
		strcpy(lpContentType,"*.*");
}


// **************************************************
// 声明：static void ReleasePart(LPMIME_PART lpMIME_Part)
// 参数：
//	IN lpMIME_Part -- 要释放的PART结构指针
// 
// 返回值：无
// 功能描述：释放PART空间。
// 引用: 
// **************************************************
static void ReleasePart(LPMIME_PART lpMIME_Part)
{
	LPMIME_PART lpNextPart = NULL;
		
		while(1)
		{

			if (lpMIME_Part == NULL)
				return ;

			lpNextPart = lpMIME_Part->next;

			if (lpMIME_Part->lpContent_Description)
				free(lpMIME_Part->lpContent_Description); //释放Content - Description
			
			if (lpMIME_Part->lpContent_ID)
				free(lpMIME_Part->lpContent_ID);  //释放content - ID

			if (lpMIME_Part->lpContent_Type)
				free(lpMIME_Part->lpContent_Type);  // 释放content - type

			if (lpMIME_Part->lpContent_Url)
				free(lpMIME_Part->lpContent_Url);   // 释放 content url

			if (lpMIME_Part->lpContent_Location)
				free(lpMIME_Part->lpContent_Location);  // 释放 conten location

			free(lpMIME_Part); // 释放part结构

			lpMIME_Part = lpNextPart;
		}
}



// **************************************************
// 声明：static LPTSTR BufferAllocCopy(LPTSTR pSrc)
// 参数：
//	IN pSrc -- 原始字串
// 
// 返回值：新的字串
// 功能描述：分配一个新的空间并复制原始串的内容。
// 引用: 
// **************************************************
static LPTSTR BufferAllocCopy(LPTSTR pSrc)
{
	TCHAR *ptr;
	DWORD dwLen;

		if (pSrc==NULL)
			return NULL;
		dwLen=strlen(pSrc)+1;
		ptr=(TCHAR *)malloc(dwLen*sizeof(TCHAR));
		if (ptr==NULL)
		{
			MessageBox(NULL,TEXT("The memory is not enough"),TEXT("memory alloc failure"),MB_OK);
			return NULL;
		}
		strcpy(ptr,pSrc);
		return ptr;
}



// **************************************************
// 声明：static int WINAPI  SendMMSThread(LPVOID lParam)
// 参数：
//	
// 返回值：发送成功返回TRUE，否则返回FALSE。
// 功能描述：发送彩信线程。
// 引用: 
// **************************************************
static int WINAPI SendMMSThread(LPVOID lParam )
{
	LPMMS_CONTENT lpMMS_Content;
	UINT uIndex ,iCount;
	HMMS hMMS;

	while(1)
	{
		hMMS = OpenMMS(MMS_SEND); // 打开要发送的彩信数据库
		iCount = GetMMSCount(hMMS); // 得到数据库中要发送的彩信数量

		uIndex = 0;
		while (uIndex < iCount)
		{ 
			// 有要发送的彩信存在
			lpMMS_Content = (LPMMS_CONTENT)LoadMMSData(hMMS,0); // 得到第一条记录的数据
			if (lpMMS_Content != NULL)
			{
//				if (StartSendMMSMessage(lpMMS_Content) == TRUE)
				if (SentMMSToServer(lpMMS_Content) == TRUE)  // 发送彩信
				{
					// 发送成功
					DeleteMMS(hMMS, uIndex); // 从送件箱中删除已经发送成功的彩信
					SaveMessageToTable(lpMMS_Content,MMS_SENT); // 将已经发送的彩信存入到已发件箱
				}
				ReleaseMMSData(hMMS,(HMMSDATA)lpMMS_Content); // 释放装载的数据
			}
			else
			{
				uIndex ++;  // 发送下一条记录
			}
		}

		CloseMMS(hMMS); // 关闭数据库

		Sleep(1000 * 6);  // 一分钟查询一次
	}
	return TRUE;
}


// **************************************************
// mms 监视窗口
// **************************************************

// **************************************************
// 声明：static ATOM RegisterMMSMonitorWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册应用程序窗口类。
// 引用: 
// **************************************************
static ATOM RegisterMMSMonitorWindow(HINSTANCE hInstance)
{
	WNDCLASS wc;

    wc.style = 0;//CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) MonitorWndProc; // 应用程序过程函数
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL; // 装载应用程序图标
    wc.hCursor = 0;
    wc.hbrBackground = NULL;//(HBRUSH) GetStockObject(GRAY_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = classMMSMonitorWindow; // 应用程序窗口类名

	return RegisterClass(&wc); // 注册类
}


// **************************************************
// 声明：static LRESULT CALLBACK MonitorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	不同的消息有不同的返回值，具体看消息本身
// 功能描述：应用程序窗口过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK MonitorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			EndPaint(hWnd, &ps);
			break;
		case WM_NEW_MMS_PUSH:
			NewMMSArrive(hWnd,wParam,lParam); // 有彩信到达
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// **************************************************
// 声明：static LRESULT NewMMSArrive(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
// 返回值：
//	无
// 功能描述：窗口得到一个彩信的通知消息
// 引用: 
// ********************************************************************
static LRESULT NewMMSArrive(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMMS_CONTENT lpMMS_Content = NULL;
	DWORD dwDataID;

		lpMMS_Content = (LPMMS_CONTENT)malloc(sizeof(MMS_CONTENT)); // 分配一个MMS数据的结构缓存
		if (lpMMS_Content == NULL)
		{ // 分配失败
			SetLastError(MMS_ERROR_NOMEMORY); // 系统内存不足
			return 0;
		}
		memset((LPVOID)lpMMS_Content,0,sizeof(MMS_CONTENT)); // 清除结构内容

		dwDataID = wParam;

		if (GetNotificationFromServer(dwDataID ,lpMMS_Content) == TRUE)
		{
			SaveMessageToTable(lpMMS_Content,MMS_RECEIVE);
		}

		if (lpMMS_Content)
		{
			if (lpMMS_Content->lpContent_Type)
				free(lpMMS_Content->lpContent_Type);  // 释放内容类型缓存

			if (lpMMS_Content->lpData)
				free(lpMMS_Content->lpData);  // 释放数据缓存

			if (lpMMS_Content->lpFromTo)
				free(lpMMS_Content->lpFromTo);   // 释放发件人缓存

			if (lpMMS_Content->lpSubject)
				free(lpMMS_Content->lpSubject);   // 释放标题缓存

			free(lpMMS_Content);  // 释放数据结构缓存
		}
		return 0;
}


// ***************************************************************************************
// 声明：static void WriteMessageToTable(HTABLE hTable,LPMMS_CONTENT lpMMS_Content,UINT uMessageType)
// 参数：
//	IN hTable -- 当前打开的TABLE
//	IN lpMMS_Content -- 要保存的彩信
//	IN bNewMessage -- 是否是新的彩信
// 返回值：
//		无
// 功能描述：
//		保存一个的彩信
// ***************************************************************************************
static void WriteMessageToTable(HTABLE hTable,LPMMS_CONTENT lpMMS_Content,UINT uMessageType)
{
	UINT uStatus = 0;
	FIELDITEM fieldItem[FIELD_NUM];
	static TCHAR lpData[0XE1F];

//		RETAILMSG(1,(TEXT("WriteMessageToTable \r\n")));
		// 数据规划		
		//  Field 0 -- |  Flag  | Field 0 Len | Field 1 Len | ... ... | Field n Len | 
		//  Field 1 -- |Field 0 Data | Field 1 Data | ... | Field n Data |

		//  其中 Flag 为当前条目的标志, DWORD (4 BYTE)
		//  flag --  bit 31  -- 当前数据是否有效
		//			 bit 15 -- bit 0 -- 当前记录的状态
		//  Field 0 Len ... Field n Len 为各个字段的长度 每个字段为一个WORD (2 BYTE)
		//  Field 0 Data  ...  Field n Data  为各个字段的数据

#ifdef SINGLE_FILE
		// 彩信类型
		switch(uMessageType)
		{
			case MMS_RECEIVE:	//	收件箱
				fieldItem[FIELD_TYPE].lpData = "R";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SEND:		//  发件箱
				fieldItem[FIELD_TYPE].lpData = "T";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SENT:		//	已发信息
				fieldItem[FIELD_TYPE].lpData = "H";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SAVED:		//  存档文件夹
				fieldItem[FIELD_TYPE].lpData = "S";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			default: // 不认识的MMS类型
				SetLastError(MMS_ERROR_UNKNOWMMSTYPE); // 不认识的MMS类型
				return;
		}
#endif

		// 彩信状态
		uStatus |= MMS_READ ; //设置MMS未读
		fieldItem[FIELD_STATUS].lpData = &uStatus;
		fieldItem[FIELD_STATUS].size = sizeof(DWORD);


		// 写入字段 0 -- 发件人/收件人
		fieldItem[FIELD_FROMTO].lpData = lpMMS_Content->lpFromTo;
		fieldItem[FIELD_FROMTO].size = strlen(lpMMS_Content->lpFromTo) + 1; // 包含结尾符

		// 写入字段 1 --  标题
		fieldItem[FIELD_SUBJECT].lpData = lpMMS_Content->lpSubject;
		fieldItem[FIELD_SUBJECT].size = strlen(lpMMS_Content->lpSubject) + 1; // 包含结尾符

		// 写入字段 2 --  时间
		fieldItem[FIELD_DATE].lpData = &lpMMS_Content->dwDate;
		fieldItem[FIELD_DATE].size = sizeof(DWORD); 

		// 写入字段 3 --  内容类型
		fieldItem[FIELD_CONTENTTYPE].lpData = lpMMS_Content->lpContent_Type;
		if (lpMMS_Content->lpContent_Type == NULL)
		{
			fieldItem[FIELD_CONTENTTYPE].size = 0; // 包含结尾符
		}
		else
		{
			fieldItem[FIELD_CONTENTTYPE].size = strlen(lpMMS_Content->lpContent_Type) + 1; // 包含结尾符
		}

		// 写入字段 4 --  数据

		fieldItem[FIELD_MIME].lpData = lpMMS_Content->lpData;
		fieldItem[FIELD_MIME].size = lpMMS_Content->iDataSize; // 包含结尾符

		WriteRecord( hTable, (FIELDITEM *)&fieldItem);

}


// **************************************************
// 声明：static LONG EncodeDateValue( SYSTEMTIME *lpSystemTime)
// 参数：
//	IN lpSystemTime -- 要进行编码的时间
// 
// 返回值：成功编码后的LONG数据
// 功能描述：编码一个DATE的数据。
// 引用: 
// **************************************************
static LONG EncodeDateValue( SYSTEMTIME *lpSystemTime)
{
	SYSTEMTIME curDate ;
	DWORD dwSecs = 0; // 得到从1970 - 01 - 01 , 00:00:00 开始的秒数
	int iDays;

		// 初始化起始时间 1970 - 01 - 01 , 00:00:00
		RETAILMSG(1,(TEXT(" Current Date = %d-%02d-%02d %02d:%02d:%02d\r\n"),lpSystemTime->wYear,lpSystemTime->wMonth,lpSystemTime->wDay,lpSystemTime->wHour,lpSystemTime->wMinute,lpSystemTime->wSecond));

		curDate.wYear = 1970;
		curDate.wMonth = 1;
		curDate.wDay = 1;
		curDate.wHour = 0;
		curDate.wMinute = 0;
		curDate.wSecond = 0;
		curDate.wMilliseconds = 0;



		iDays = DateDiff(&curDate, lpSystemTime); // 得到从1970-01-01 开始计算起的天数
		dwSecs = iDays * (3600 * 24) ; // 得到所有天数的总共秒数

		// 得到当前的时间
		dwSecs += lpSystemTime->wHour * 3600;  // 加上小时所需要的秒数

		dwSecs += lpSystemTime->wMinute * 60;  // 得到除去分数后的秒数

		dwSecs += lpSystemTime->wSecond; // 加上秒数
		
		return dwSecs; 
}


// ***************************************************************************************
// 声明：UINT GetMMSCapacity(HMMS hMMS)
// 参数：
//    hMMS -- 已经打开的MMS句柄
// 返回值：
//		返回当前系统中的MMS的容量
// 功能描述：
//  	得到当前系统中MMS的容量
// ***************************************************************************************
UINT GetMMSCapacity(HMMS hMMS)
{
	LPMMSMANAGE lpMMSManage;

		if (hMMS == NULL)
		{  // 错误的MMS句柄
			RETAILMSG(1,(TEXT("hMMS == NULL \r\n")));
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return 0;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // 得到MMS管理结构的指针
		switch(lpMMSManage->uMMSType)
		{
			case MMS_RECEIVE:	//	收件箱
				return RECORDNUM_RECEIVE;
			case MMS_SEND:		//  发件箱
				return RECORDNUM_SEND;
			case MMS_SENT:		//	已发信息
				return RECORDNUM_SENT;
			case MMS_SAVED:		//  存档文件夹
				return RECORDNUM_SAVED;
			default: // 不认识的MMS类型
				return 0;
		}
}