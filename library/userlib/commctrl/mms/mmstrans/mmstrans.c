#include <ewindows.h>
#include <mms_clt.h>
#include "mmstrans.h"
#include <ztmsgbox.h>
#include <dialdlg.h>


#define MMSTRANS_TEST

#define DEFAULTCHARSET	0			// 默认的彩信字符集
#define DEFAULTCONTENTTYPE	0X21	// 默认的彩信内容类型

static LPTSTR AllocCopyString(LPTSTR pSrc);
static BOOL GetMMSContentFromNotification(MCLT_NOTIFICATION_IND *pNotification_Ind,LPMMS_CONTENT lpMMS_Content);
static BOOL GetMMSContentFromRetrieve(MCLT_RETRIEVE_CONF *pRetrieveConf,LPMMS_CONTENT lpMMS_Content);
static BOOL	FillMMSContentToSendReq(LPMMS_CONTENT lpMMS_Content,MCLT_SEND_REQ* pPduMCltReq);
static LPTSTR GetLocalNumber(void);

// ***************************************************************************************
// 声明：BOOL GetNotificationFromServer(DWORD dwDataID ,LPMMS_CONTENT lpMMS_Content)
// 参数：
//  IN  dwDataID -- 要得到的通知消息的ID
//  OUT lpMMS_Content -- 存储通知消息的结构
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	从服务器读取彩信通知包
// ***************************************************************************************
BOOL GetNotificationFromServer(DWORD dwDataID ,LPMMS_CONTENT lpMMS_Content)
{
	MCLT_NOTIFICATION_IND stMCLT_Notification_Ind;
	ERROR_INFO			InfoErr;

		memset( &InfoErr, 0, sizeof(ERROR_INFO) ); // 清空错误结构

		if (MCltReadNotifyInd(dwDataID,&stMCLT_Notification_Ind,&InfoErr) == FALSE) // 得到通知包
		{
			// 得到彩信的通知消息失败
			return FALSE;
		}
		// 从通知包中得到彩信通知消息
		GetMMSContentFromNotification(&stMCLT_Notification_Ind,lpMMS_Content);
		
		MCltFreePdu((MCLT_PDU*)&stMCLT_Notification_Ind); // 释放得到的通知包

		return TRUE; // 得到通知包成功
}


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
BOOL GetMMSFromServer(LPVOID lpData, DWORD dwSize,LPMMS_CONTENT lpMMS_Content)
{
	HANDLE				hMClt;
	ERROR_INFO			InfoErr;
	MCLT_RETRIEVE_CONF	stRetrieveConf;
	LPTSTR lpMMSLocation;
	HANDLE hRasConn;


		// 打开网页之前首先要拨号到网络
		if (DailToInternet(NULL,&hRasConn,GPRS_NETTYPE_WAP) == FALSE) // 拨号到INTERNET
		{
			// 不能连接到网络
#ifdef ZT_PHONE
			ZTMessageBox(NULL,"不能连接到网络","错误",MB_OK|MB_TOPMOST);
#else
			MessageBox(NULL,"不能连接到网络","错误",MB_OK|MB_TOPMOST);
#endif
			return FALSE;
		}

		// 目前存储的通知消息数据仅仅是地址而已
		lpMMSLocation = (LPTSTR)lpData;

		memset( &InfoErr, 0, sizeof(ERROR_INFO) ); // 清空错误结构

		if( !(hMClt=MCltOpen( &InfoErr )) )  // 打开彩信句柄
		{
			HandDownInternet(NULL,hRasConn); // 挂断与网络的联系
			return FALSE;
		}


		memset( &stRetrieveConf, 0, sizeof(MCLT_RETRIEVE_CONF) ); // 清空彩信数据包结构
		if( !MCltRetrieval( hMClt, lpMMSLocation, &stRetrieveConf, &InfoErr ) )  // 得到彩信
		{  // 得到彩信失败
			MCltClose( hMClt );
			HandDownInternet(NULL,hRasConn); // 挂断与网络的联系
			return FALSE;
		}

		// 从彩信数据包中得到彩信数据
		GetMMSContentFromRetrieve(&stRetrieveConf,lpMMS_Content);
		

		MCltFreePdu( (MCLT_PDU*)&stRetrieveConf ); // 释放彩信数据包结构

		MCltClose( hMClt ); // 关闭彩信句柄

		HandDownInternet(NULL,hRasConn); // 挂断与网络的联系
		return TRUE;
}


// ***************************************************************************************
// 声明：BOOL SentMMSToServer(LPMMS_CONTENT lpMMS_Content)
// 参数：
//  IN lpMMS_Content -- 要发送的彩信数据
// 返回值：
//		成功返回TRUE，否则返回FALSE
// 功能描述：
//  	发送彩信数据到服务器
// ***************************************************************************************
BOOL SentMMSToServer(LPMMS_CONTENT lpMMS_Content)
{
	HANDLE				hMClt;
	ERROR_INFO			InfoErr;
	MCLT_SEND_REQ		PduMCltReq;
	MCLT_SEND_CONF		PduMCltConf;
	HANDLE hRasConn;

		// 打开网页之前首先要拨号到网络
		if (DailToInternet(NULL,&hRasConn,GPRS_NETTYPE_WAP) == FALSE) // 拨号到INTERNET
		{
			// 不能连接到网络
#ifdef ZT_PHONE
			ZTMessageBox(NULL,"不能连接到网络","错误",MB_OK|MB_TOPMOST);
#else
			MessageBox(NULL,"不能连接到网络","错误",MB_OK|MB_TOPMOST);
#endif
			return FALSE;
		}

		memset( &PduMCltReq, 0, sizeof(PduMCltReq) );  // 清空彩信发送包
		memset( &PduMCltConf, 0, sizeof(PduMCltConf) ); // 清空确认包

		// 填充要发送的数据
		FillMMSContentToSendReq(lpMMS_Content,&PduMCltReq);

		memset( &InfoErr, 0, sizeof(ERROR_INFO) ); // 清空错误包
		if( !(hMClt=MCltOpen( &InfoErr )) ) // 打开彩信句柄
		{  // 打开句柄失败
			HandDownInternet(NULL,hRasConn); // 挂断与网络的联系
			return FALSE;
		}


		if( !MCltSendReq( hMClt, &PduMCltReq, &PduMCltConf, &InfoErr ) )  // 发送彩信
		{ // 发送失败
			MCltFreePdu( (MCLT_PDU*)&PduMCltReq );
			MCltClose( hMClt );
			HandDownInternet(NULL,hRasConn); // 挂断与网络的联系
	#ifdef MMSTRANS_TEST		
			return TRUE;
	#else
			return FALSE;
	#endif
		}

		MCltFreePdu( (MCLT_PDU*)&PduMCltConf ); // 释放确认包

		MCltClose( hMClt ); // 关闭彩信句柄
		HandDownInternet(NULL,hRasConn); // 挂断与网络的联系
		return TRUE;
}


// ***************************************************************************************
// 声明：static LPTSTR AllocCopyString(LPTSTR pSrc)
// 参数：
//  IN  pSrc -- 复制的字串
// 返回值：
//		返回新的字串
// 功能描述：
//  	分配一个空间并复制当前字串到新的缓存中
// ***************************************************************************************
static LPTSTR AllocCopyString(LPTSTR pSrc)
{
	LPTSTR ptr;
	DWORD dwLen;

		if (pSrc==NULL)
			return NULL;
		dwLen=strlen(pSrc)+1;
		ptr=(TCHAR *)malloc(dwLen*sizeof(TCHAR));
		if (ptr==NULL)
		{
			MessageBox(NULL,TEXT("The memory is not enough"),TEXT("memory alloc failure"),MB_OK|MB_TOPMOST);
			return NULL;
		}
		strcpy(ptr,pSrc);
		return ptr;
}



// ***************************************************************************************
// 声明：static BOOL GetMMSContentFromNotification(MCLT_NOTIFICATION_IND *pNotification_Ind,LPMMS_CONTENT lpMMS_Content)
// 参数：
//	IN pNotification_Ind -- 得到的通知包数据结构
//  OUT  lpMMS_Content -- 要被填充的彩信数据结构
// 返回值：
//		成功返回TRUE， 否则返回FALSE
// 功能描述：
//  	从通知包中得到彩信通知消息
// ***************************************************************************************
static BOOL GetMMSContentFromNotification(MCLT_NOTIFICATION_IND *pNotification_Ind,LPMMS_CONTENT lpMMS_Content)
{
	SYSTEMTIME stCurTime;

		// 得到发件人
		if (pNotification_Ind->dwMask & MCLT_MASK_From)
		{
			// 有 FROM 项
			lpMMS_Content->lpFromTo = AllocCopyString(pNotification_Ind->stFrom.stEncodedStr.pszString);
		}
		else
		{
			// 无 FROM 项 ， 不知从和而来，显示“...”
			lpMMS_Content->lpFromTo = AllocCopyString("...");
		}

		// 得到主题
		if (pNotification_Ind->dwMask & MCLT_MASK_Subject)
		{
			// 有 主题
			lpMMS_Content->lpSubject = AllocCopyString(pNotification_Ind->stSubject.pszString);
		}
		else
		{
			// 无 主题
			lpMMS_Content->lpSubject = AllocCopyString("无主题");
		}

		// 得到当前的时间
		GetLocalTime(&stCurTime);
		VarMake_Date(&lpMMS_Content->dwDate,&stCurTime);
	
		// 得到彩信内容类型 , 在通知包中不包含彩信的内容类型
		lpMMS_Content->lpContent_Type = NULL;

		// 保存通知包的数据， 目前只保留彩信的地址
		lpMMS_Content->lpData = AllocCopyString(pNotification_Ind->pszX_Mms_Content_Location);
		lpMMS_Content->iDataSize = strlen(lpMMS_Content->lpData) + 1;  // 包括结尾符

		return TRUE;
}


// ***************************************************************************************
// 声明：static BOOL GetMMSContentFromRetrieve(MCLT_RETRIEVE_CONF *pRetrieveConf,LPMMS_CONTENT lpMMS_Content)
// 参数：
//	IN pRetrieveConf -- 得到的彩信数据包数据结构
//  OUT  lpMMS_Content -- 要被填充的彩信数据结构
// 返回值：
//		成功返回TRUE， 否则返回FALSE
// 功能描述：
//  	从彩信数据包中得到彩信数据
// ***************************************************************************************
static BOOL GetMMSContentFromRetrieve(MCLT_RETRIEVE_CONF *pRetrieveConf,LPMMS_CONTENT lpMMS_Content)
{
			// 得到发件人
		if (pRetrieveConf->dwMask & MCLT_MASK_From)
		{
			// 有 FROM 项
			lpMMS_Content->lpFromTo = AllocCopyString(pRetrieveConf->stFrom.stEncodedStr.pszString);
		}
		else
		{
			// 无 FROM 项 ， 不知从和而来，显示“...”
			lpMMS_Content->lpFromTo = AllocCopyString("...");
		}

		// 得到主题
		if (pRetrieveConf->dwMask & MCLT_MASK_Subject)
		{
			// 有 主题
			lpMMS_Content->lpSubject = AllocCopyString(pRetrieveConf->stSubject.pszString);
		}
		else
		{
			// 无 主题
			lpMMS_Content->lpSubject = AllocCopyString("无主题");
		}

		// 得到发送时间
		
		if (pRetrieveConf->dwMask & MCLT_MASK_Date)
		{
			// 有发送时间
			lpMMS_Content->dwDate = pRetrieveConf->dwDate;
		}
		else
		{
			 // 没有发送时间
			SYSTEMTIME stCurTime;

				GetLocalTime(&stCurTime); // 得到当前时间
				VarMake_Date(&lpMMS_Content->dwDate,&stCurTime);
		}

		// 得到当前彩信的类型

		if (pRetrieveConf->dwMask & MCLT_MASK_CntType)
		{
			if (pRetrieveConf->stContent_Type.stType.fIsString == TRUE)
			{ // 是字串
				lpMMS_Content->lpContent_Type = AllocCopyString(pRetrieveConf->stContent_Type.stType.pszString);
			}
			else
			{
				// 是一个已知的变量
				CNTTYPE_ASSIGN stCntTypeAssign;
	
					stCntTypeAssign.dwNum = pRetrieveConf->stContent_Type.stType.dwVal;
					//  查找已知的类型
					if (VarFind_CntTypeAssign( CNTTYPEFIND_NAME_BYNUM, &stCntTypeAssign ) == TRUE)
					{
						// 已经查到
						lpMMS_Content->lpContent_Type = AllocCopyString(stCntTypeAssign.szName);
					}
					else
					{
						lpMMS_Content->lpContent_Type = NULL;
					}
			}
		}
		else
		{
			lpMMS_Content->lpContent_Type = NULL;
		}
		
		// 得到彩信内容
		if (pRetrieveConf->dwMask & MCLT_MASK_Body)
		{
			// 有彩信内容
			lpMMS_Content->iDataSize = pRetrieveConf->stBodyGen.dwLenBody; // 得到数据长度
			lpMMS_Content->lpData = malloc(lpMMS_Content->iDataSize); // 分配存放内容的缓存
			if (lpMMS_Content->lpData == NULL)
			{
				// 分配缓存失败
				lpMMS_Content->iDataSize = 0;
			}
			else
			{
				memcpy(lpMMS_Content->lpData,pRetrieveConf->stBodyGen.pBody,lpMMS_Content->iDataSize); // 复制数据
			}
		}
		else
		{
			// 没有彩信内容
			lpMMS_Content->lpData = NULL;
			lpMMS_Content->iDataSize = 0;
		}

		return TRUE;
}
// ***************************************************************************************
// 声明：static BOOL	FillMMSContentToSendReq(LPMMS_CONTENT lpMMS_Content,MCLT_SEND_REQ* pPduMCltReq)
// 参数：
//  IN  lpMMS_Content -- 要发送的彩信数据结构
//	OUT pPduMCltReq -- 要被填充的发送包结构
// 返回值：
//		成功返回TRUE， 否则返回FALSE
// 功能描述：
//  	填充当前的彩信数据到发送包结构。
// ***************************************************************************************
static BOOL	FillMMSContentToSendReq(LPMMS_CONTENT lpMMS_Content,MCLT_SEND_REQ* pPduMCltReq)
{
	pPduMCltReq->dwMask = 0; 
	pPduMCltReq->bX_Mms_Message_Type = MType_send_req; // 是一个发送彩信的数据包

	// from 发件人
	pPduMCltReq->stFrom.stEncodedStr.dwCharset = DEFAULTCHARSET; // 设置为默认字符集
	pPduMCltReq->stFrom.stEncodedStr.pszString = GetLocalNumber(); // 得到本机电话号码
	pPduMCltReq->dwMask |= MCLT_MASK_From;

	// to 收件人
	pPduMCltReq->stTo.dwCharset = DEFAULTCHARSET; // 设置为默认字符集
	pPduMCltReq->stTo.pszString = AllocCopyString(lpMMS_Content->lpFromTo); //设置对方的电话号码
	pPduMCltReq->dwMask |= MCLT_MASK_To;

	// stSubject 主题
	pPduMCltReq->stSubject.dwCharset = DEFAULTCHARSET; // 设置为默认字符集
	pPduMCltReq->stSubject.pszString = AllocCopyString(lpMMS_Content->lpSubject); //设置主题
	pPduMCltReq->dwMask |= MCLT_MASK_Subject;

	// stContent_Type 彩信内容类型
	if (lpMMS_Content->lpContent_Type == NULL)
	{
		// 没有指定类型
		pPduMCltReq->stContent_Type.stType.fIsString = FALSE;
		pPduMCltReq->stContent_Type.stType.dwVal = DEFAULTCONTENTTYPE;
	}
	else
	{
		// 有指定的内容类型
		CNTTYPE_ASSIGN stCntTypeAssign;

			if (strlen(lpMMS_Content->lpContent_Type) >= LEN_WSP_CNTTYPE_NAME)
			{
				// 肯定不是已知的彩信内容类型
				pPduMCltReq->stContent_Type.stType.fIsString = TRUE;
				pPduMCltReq->stContent_Type.stType.pszString = AllocCopyString(lpMMS_Content->lpContent_Type);
			}
			else
			{
				strcpy(stCntTypeAssign.szName,lpMMS_Content->lpContent_Type);
				//  查找当前彩信类型是否为已知的类型
				if (VarFind_CntTypeAssign( CNTTYPEFIND_NUM_BYNAME, &stCntTypeAssign ) == TRUE)
				{
					// 已经查到，是已知的彩信内容类型
					pPduMCltReq->stContent_Type.stType.fIsString = FALSE;
					pPduMCltReq->stContent_Type.stType.dwVal = stCntTypeAssign.dwNum;
				}
				else
				{
					// 不是已知的彩信内容类型
					pPduMCltReq->stContent_Type.stType.fIsString = TRUE;
					pPduMCltReq->stContent_Type.stType.pszString = AllocCopyString(lpMMS_Content->lpContent_Type);
				}
			}
	}
	pPduMCltReq->dwMask |= MCLT_MASK_CntType;


	// 彩信数据

	pPduMCltReq->stBodyGen.dwLenBody = lpMMS_Content->iDataSize;
	pPduMCltReq->stBodyGen.pBody = malloc(pPduMCltReq->stBodyGen.dwLenBody);
	if (pPduMCltReq->stBodyGen.pBody)
	{
		pPduMCltReq->stBodyGen.wFlag = BODYF_ALLOC;
		memcpy(	pPduMCltReq->stBodyGen.pBody,lpMMS_Content->lpData,pPduMCltReq->stBodyGen.dwLenBody);
		pPduMCltReq->dwMask |= MCLT_MASK_Body;
	}


	return TRUE;
}


/*

#define	MCLT_MASK_From				0x80000000
#define	MCLT_MASK_To				0x40000000
#define	MCLT_MASK_Cc				0x20000000
#define	MCLT_MASK_Bcc				0x10000000
#define	MCLT_MASK_Subject			0x08000000
#define	MCLT_MASK_Date				0x08000000

#define	MCLT_MASK_MClass			0x04000000
#define	MCLT_MASK_Priority			0x02000000
#define	MCLT_MASK_Expiry			0x01000000

#define	MCLT_MASK_Response_Status	0x00800000

#define	MCLT_MASK_CntType			0x00400000
#define	MCLT_MASK_CntLocation		0x00200000
#define	MCLT_MASK_Body				0x00100000

#define	MCLT_MASK_MSize				0x00080000
  */


// ***************************************************************************************
// 声明：static LPTSTR GetLocalNumber(void)
// 参数：
//		无
// 返回值：
//		返回本机的电话号码
// 功能描述：
//  	得到本机的电话号码。
// ***************************************************************************************
static LPTSTR GetLocalNumber(void)
{
	return AllocCopyString("13510994325");
}