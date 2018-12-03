/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：MIME数据PACK，UNPACK函数
版本号：1.0.0.456
开发时期：2004-05-15
作者：陈建明 JAMI
修改记录：
**************************************************/

#ifndef KINGMOS
#include "windows.h"
#include <stdio.h>
#include "Debugwnd.h"
#else
#include <ewindows.h>
#include <estdio.h>
#endif

#include "mime_body.h"
#include <mms_clt.h>


/****************************************************
参考资料： RFC2045,RFC2047,RFC2048,RFC2049
			WAP-230-WSP-20010705-a 
数据说明：

****************************************************/



// ****************************************************
// 函数声明
// ****************************************************
static LPMIME_PART PraseMIME_PartHeader(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,UINT iPartHeaderSize);
static UINT GetPartItemID(LPDATA_DEAL lpMMS_Data,BOOL *pbOver);
static void InsertPartToBody(LPBODYDETAIL lpBodyDetail,LPMIME_PART lpMIME_Part);
static BOOL SavePartData(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,LPTSTR lpFileName,unsigned int iPartSize);

static LPTSTR UnpackContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver);
static LPTSTR UnpackMedia_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, unsigned int iValueLen);

static BOOL PackMIME_PartHeader(LPDATA_DEAL lpMMS_Data,LPMIME_PART lpMIME_Part);
static BOOL PackPartItemID(LPDATA_DEAL lpMMS_Data,UINT uID);

static void ReleasePart(LPMIME_PART lpMIME_Part);

static BOOL ReadField_name(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, SHORT_STR *lpFieldName);
static BOOL WriteContent_Type(LPDATA_DEAL lpMMS_Data,LPTSTR lpContent_Type);
static BOOL WriteField_name(LPDATA_DEAL lpMMS_Data, SHORT_STR * lpFieldName);
static LPTSTR ReadContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver);

static LPTSTR AllocCopyString(LPTSTR pSrc);

// **************************************************
// 声明：LPBODYDETAIL MIME_UnPack(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
// 参数：
//	IN lpMMS_Data -- 要解包的MMS数据结构
//	OUT pbOver -- 返回已经没有数据的标志
// 
// 返回值：成功返回MIME的结构，否则返回NULL
// 功能描述：解包MIME部分。
// 引用: 
// **************************************************
LPBODYDETAIL MIME_UnPack(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
{
	BYTE bPartNum,i;
	DWORD iPartSize;
	unsigned int iPartHeaderSize;
//	TCHAR lpFileName[MAX_PATH];
	LPMIME_PART lpMIME_Part;
	LPBODYDETAIL lpBodyDetail;
	BOOL bRet;

	
//		bPartNum = ReadByte(lpMMS_Data,pbOver); // 得到当前MIME所包含的PART数目
		bRet = VarDec_Byte(lpMMS_Data,&bPartNum);// 得到当前MIME所包含的PART数目
		if (bPartNum== 0) // 没有任何的PART数据
			return NULL; // 没有媒体数据

		lpBodyDetail = (LPBODYDETAIL)malloc(sizeof(BODYDETAIL)); // 分配一个MIME BODY 结构
		if (lpBodyDetail == NULL)
		{  // 分配内存失败
			RETAILMSG(1,(TEXT("!!! ERROR malloc in MIME_UnPack \r\n")));
			return NULL;
		}
		// 初始化结构
		lpBodyDetail->iPartNum = bPartNum;
		lpBodyDetail->lpMIME_Part = NULL;

		for ( i = 0; i < bPartNum; i++)
		{
			// 得到各个部分的数据

			// 得到Content header size
			// 在PART的开始，首先是PART HEADER 的长度，接下来是PART内容的长度
//			iPartHeaderSize = ReadUnsignedIntegers(lpMMS_Data,pbOver); // 得到Header的长度
			VarDec_UintVar(lpMMS_Data,&iPartHeaderSize);// 得到Header的长度
//			iPartSize = ReadUnsignedIntegers(lpMMS_Data,pbOver); // 得到内容的长度
			VarDec_UintVar(lpMMS_Data,&iPartSize);// 得到内容的长度
			
			// 得到Content header
			lpMIME_Part = PraseMIME_PartHeader(lpMMS_Data,pbOver,iPartHeaderSize);
			if (lpMIME_Part == NULL)
			{
				RETAILMSG(1,(TEXT("!!!!Prase MIME Part Error \r\n")));
				goto ERROR_RETURN;
			}
			lpMIME_Part->uLen = iPartSize; // 得到内容的长度
			lpMIME_Part->uPos = lpMMS_Data->dwLenDealed;  // 得到内容的位置
			lpMMS_Data->dwLenDealed += iPartSize;  // 到下一个PART的位置

			// 得到Content
//			sprintf(lpMIME_Part->lpPath,"%s\\%s",TEXT("D:\\TEST\\DECODE"),lpMIME_Part->lpFileName);

//			SavePartData(lpMMS_Data,pbOver,lpMIME_Part->lpPath,iPartSize);

			InsertPartToBody(lpBodyDetail,lpMIME_Part);
		}

		return lpBodyDetail;

ERROR_RETURN:
		// Will Release the alloc memory space
		return NULL;
}

// **************************************************
// 声明：static LPMIME_PART PraseMIME_PartHeader(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,UINT iPartHeaderSize)
// 参数：
//	IN lpMMS_Data -- 要解包的MMS数据结构
//	OUT pbOver -- 返回已经没有数据的标志
// 
// 返回值：成功返回MIME的PART头的结构，否则返回NULL
// 功能描述：解包MIME的PART头信息。
// 引用: 
// **************************************************
static LPMIME_PART PraseMIME_PartHeader(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,UINT iPartHeaderSize)
{
	LPMIME_PART lpMIME_Part;
	UINT ItemID;
	DATA_DEAL MMS_PartHeader;
	BOOL bRet;

		// 初始化要处理的数据
		MMS_PartHeader.dwLenDealed = 0;
		MMS_PartHeader.dwLenTotal = iPartHeaderSize;
		MMS_PartHeader.lpData = lpMMS_Data->lpData + lpMMS_Data->dwLenDealed;
		MMS_PartHeader.dwFlag = DDF_NOWRITE;
		MMS_PartHeader.dwLenGrow = 0;
		MMS_PartHeader.dwLenMaxAlloc = 0;

//		dwOldPos = lpMMS_Data->dwLenDealed; // 保留当前的位置，用来计算PartHeader是否还有内容
		lpMIME_Part = (LPMIME_PART)malloc(sizeof(MIME_PART)); // 分配一个MIME的PART结构

		if (lpMIME_Part == NULL)
		{ // 分配内存失败
			RETAILMSG(1,(TEXT("!!! Error Malloc in PraseMIME_PartHeader \r\n")));
			return NULL;
		}

		// 初始化结构
		lpMIME_Part->lpContent_ID = NULL;
		lpMIME_Part->lpContent_Type = NULL;
		lpMIME_Part->lpContent_Location = NULL;
		lpMIME_Part->lpContent_Description = NULL;
		lpMIME_Part->lpContent_Url = NULL;

		lpMIME_Part->next = NULL;

		// PART ITEM 的开始是 Content_Type
//		lpMIME_Part->lpContent_Type = ReadContent_Type(&MMS_PartHeader,pbOver);
		lpMIME_Part->lpContent_Type = UnpackContent_Type(&MMS_PartHeader,pbOver);

		while(1)
		{
			// Read Parameter
//			iSize = lpMMS_Data->dwLenDealed - dwOldPos ; // 得到已经读取的数据数

//			if (iSize == iPartHeaderSize)
//				break; // 数据已经读取完成
			if (MMS_PartHeader.dwLenDealed >= MMS_PartHeader.dwLenTotal)
			{  // 数据已经处理完成
				RETAILMSG(1,(TEXT("!!! Error PART HEADER Size overflow \r\n")));
				break; // 数据已经读取太多，造成错误
			}

			ItemID = GetPartItemID(&MMS_PartHeader,pbOver);
			switch(ItemID)
			{
				case PARTITEM_CONTENT_URL: // X-Wap-Content-URI
//					lpMIME_Part->lpContent_Url = ReadTextString(&MMS_PartHeader,pbOver);
					bRet = VarDec_TextStr(&MMS_PartHeader,&lpMIME_Part->lpContent_Url);
					break;
				case PARTITEM_CONTENT_ID: // Content-ID
//					lpMIME_Part->lpContent_ID = ReadTextString(&MMS_PartHeader,pbOver);
					bRet = VarDec_TextStr(&MMS_PartHeader,&lpMIME_Part->lpContent_ID);
					break;
				case PARTITEM_CONTENT_DES: // Content-Description
//					lpMIME_Part->lpContent_Description = ReadTextString(&MMS_PartHeader,pbOver);
					bRet = VarDec_TextStr(&MMS_PartHeader,&lpMIME_Part->lpContent_Description);
					break;
				case PARTITEM_CONTENT_LOCAL: // FileName
//					lpMIME_Part->lpContent_Location = ReadTextString(&MMS_PartHeader,pbOver);
					bRet = VarDec_TextStr(&MMS_PartHeader,&lpMIME_Part->lpContent_Location);
					break;
				case PARTITEM_DATE: // Date
//					lpMIME_Part->dwDate = ReadDateValue(&MMS_PartHeader,pbOver);
					bRet = VarDec_Date(&MMS_PartHeader,&lpMIME_Part->dwDate);
					break;
				default:
					RETAILMSG(1,(TEXT("!!! ERROR , UNKNOW ID IN MIME PART \r\n")));
					break;
			}

			if (bRet == FALSE)
				break;
		}

		lpMMS_Data->dwLenDealed += iPartHeaderSize; // 数据已经处理过了
		return lpMIME_Part;
}


// **************************************************
// 声明：static BOOL SavePartData(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,LPTSTR lpFileName,unsigned int iPartSize)
// 参数：
//	IN lpMMS_Data -- 要解包的MMS数据结构
//	OUT pbOver -- 返回已经没有数据的标志
//  IN lpFileName -- 要将内容保存的文件名
//  IN iPartSize -- 要保存的内容的长度
// 
// 返回值：成功返回MIME的结构，否则返回NULL
// 功能描述：解包MIME部分。
// 引用: 
// **************************************************
static BOOL SavePartData(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,LPTSTR lpFileName,unsigned int iPartSize)
{

	HANDLE hFile;
	DWORD dwWriteLen;
	LPTSTR lpData;

	hFile = CreateFile( lpFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}
	lpData = lpMMS_Data->lpData + lpMMS_Data->dwLenDealed;
	WriteFile(hFile,lpData,iPartSize,&dwWriteLen,NULL);
	CloseHandle(hFile);

	lpMMS_Data->dwLenDealed += iPartSize;
	return TRUE;
}


// **************************************************
// 声明：static UINT GetPartItemID(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
// 参数：
//	IN lpMMS_Data -- 要解包的MMS数据结构
//	OUT pbOver -- 返回已经没有数据的标志
// 
// 返回值：成功返回MIME的PART的ID，否则返回PARTITEM_UNKNOW
// 功能描述：解包MIME的PART的ID。
// 引用: 
// **************************************************
static UINT GetPartItemID(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
{
	SHORT_STR PartItemID;

		ReadField_name(lpMMS_Data,pbOver,&PartItemID );
//		if (PartItemID.bMask == MASK_INTEGRES)
		if (PartItemID.fIsString == FALSE)
		{  // 当前的Item是一个已知的标识
			switch(PartItemID.bVal)
			{
				case 0x12 : // Date
					return PARTITEM_DATE;
				case 0x0E : // FileName
					return PARTITEM_CONTENT_LOCAL;
				case 0x30 : // X-Wap-Content-URI
					return PARTITEM_CONTENT_URL;
			}
		}
		else
		{
			// 当前的Item是一个扩展的标识
			if (stricmp(TEXT("Content-ID"),PartItemID.pszString) == 0)
			{
				// 是Content-ID
				return PARTITEM_CONTENT_ID;
			}
			if (stricmp(TEXT("Content-Description"),PartItemID.pszString) == 0)
			{
				// 是Content-Description
				return PARTITEM_CONTENT_DES;
			}
		}

		return PARTITEM_UNKNOW;
}

// **************************************************
// 声明：static void InsertPartToBody(LPBODYDETAIL lpBodyDetail,LPMIME_PART lpMIME_Part)
// 参数：
//	IN lpBodyDetail -- MIME的Body结构
//	OUT lpMIME_Part -- 要插入的PART
// 
// 返回值：无
// 功能描述：将一个PART的结构插入到MIME Body 结构。
// 引用: 
// **************************************************
static void InsertPartToBody(LPBODYDETAIL lpBodyDetail,LPMIME_PART lpMIME_Part)
{
	LPMIME_PART lpCurPart;

		if (lpBodyDetail->lpMIME_Part == NULL)
		{  // 当前还没有
			lpBodyDetail->lpMIME_Part = lpMIME_Part;
			return;
		}
		else
		{
			// 已经存在PART部分
			lpCurPart = lpBodyDetail->lpMIME_Part;
			while(1)
			{
				if (lpCurPart->next == NULL)
				{
					// 后面已经没有数据
					lpCurPart->next = lpMIME_Part;
					break;
				}
				lpCurPart = lpCurPart->next; // 到下一个PART
			}
		}
}



// **************************************************
// 声明：BOOL MIME_Pack(LPDATA_DEAL lpMMS_Data,LPBODYDETAIL lpBodyDetail)
// 参数：
//	OUT lpMMS_Data -- 用来存放编码完成MMS数据结构
//	IN lpBodyDetail -- 要编码的MIME内容
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：对MIME部分进行编码。
// 引用: 
// **************************************************
BOOL MIME_Pack(LPDATA_DEAL lpMMS_Data,LPBODYDETAIL lpBodyDetail)
{
	BYTE i;
	DWORD iPartSize;
	int iPartHeaderSize;
//	TCHAR lpFileName[MAX_PATH];
	LPMIME_PART lpMIME_Part;
//	LPBODYDETAIL lpBodyDetail;
	DATA_DEAL MMS_Temp;
	HANDLE hFile;
	BOOL bResult = TRUE;
	DWORD dwReadLen;
	LPBYTE lpData = NULL;

	
		if (lpBodyDetail == NULL)
		{  // 分配内存失败
			RETAILMSG(1,(TEXT("!!! ERROR Parameter in MIME_Pack \r\n")));
			return FALSE;
		}
//		WriteByte(lpMMS_Data,(BYTE)lpBodyDetail->iPartNum); // 写入当前的要加入的文件数目
		VarEnc_Byte(lpMMS_Data,(BYTE)lpBodyDetail->iPartNum);// 写入当前的要加入的文件数目

		MMS_Temp.dwFlag = DDF_GROW;
		MMS_Temp.dwLenTotal = 256 ; 
		MMS_Temp.lpData = (BYTE *)malloc(MMS_Temp.dwLenTotal);
		if (MMS_Temp.lpData == NULL)
		{
			// 分配内存失败
			return FALSE;
		}
		memset(MMS_Temp.lpData, 0, MMS_Temp.dwLenTotal);
		MMS_Temp.dwLenDealed = 0;
		MMS_Temp.dwLenGrow = DDGROW_DEFAULT;

		lpMIME_Part = lpBodyDetail->lpMIME_Part; //得到第一个文件的位置
		for ( i = 0; i < lpBodyDetail->iPartNum; i++)
		{
			// 编码各个部分的数据

			// 得到Content header size
			// 在PART的开始，首先是PART HEADER 的长度，接下来是PART内容的长度
			// 由于在编码以前不知PART HEADER的长度，首先将 PART HEADER编码到一个临时文件
//			iPartHeaderSize = ReadUnsignedIntegers(lpMMS_Data,pbOver); // 得到Header的长度
//			iPartSize = ReadUnsignedIntegers(lpMMS_Data,pbOver); // 得到内容的长度
			MMS_Temp.dwLenDealed = 0;
			
			// 得到Content header
			bResult = PackMIME_PartHeader(&MMS_Temp,lpMIME_Part);
			iPartHeaderSize = MMS_Temp.dwLenDealed; //已经编码后的长度

			// 打开要编码的文件
			hFile = CreateFile( lpMIME_Part->lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
			if( hFile == INVALID_HANDLE_VALUE )
			{
				bResult = FALSE;
				goto ERROR_RETURN;
			}
			iPartSize = GetFileSize(hFile,0); 

			// 写入PART HEADER的长度
//			WriteUnsignedIntegers(lpMMS_Data,iPartHeaderSize);
			VarEnc_UintVar(lpMMS_Data,iPartHeaderSize);

			// 写入PART 内容的长度
//			WriteUnsignedIntegers(lpMMS_Data,iPartSize);
			VarEnc_UintVar(lpMMS_Data,iPartSize);

			// 写入PART HEADER
			// 现在我们将临时缓存内的数据复制到当前缓存

			if ((lpMMS_Data->dwLenTotal - lpMMS_Data->dwLenDealed) < MMS_Temp.dwLenDealed)
			{
				//  缓存空间不足
				bResult = FALSE;
				CloseHandle(hFile); // 关闭文件句柄
				goto ERROR_RETURN;
			}

			// 复制数据
			memcpy((lpMMS_Data->lpData + lpMMS_Data->dwLenDealed ),MMS_Temp.lpData,MMS_Temp.dwLenDealed);
			lpMMS_Data->dwLenDealed += MMS_Temp.dwLenDealed;

			// 写入PART 内容

			if ((lpMMS_Data->dwLenTotal - lpMMS_Data->dwLenDealed) < iPartSize)
			{
				//  缓存空间不足
				bResult = FALSE;
				CloseHandle(hFile); // 关闭文件句柄
				goto ERROR_RETURN;
			}
//			SavePartData(lpMMS_Data,pbOver,lpMIME_Part->lpPath,iPartSize);
			lpData = lpMMS_Data->lpData + lpMMS_Data->dwLenDealed;
			ReadFile(hFile,lpData,iPartSize,&dwReadLen,NULL); // 将当前文件的内容编码到内存
			CloseHandle(hFile); // 关闭文件句柄
			lpMMS_Data->dwLenDealed += iPartSize;

			lpMIME_Part = lpMIME_Part->next; //开始下一个PART
		}

ERROR_RETURN:
		// Will Release the alloc memory space
		free(MMS_Temp.lpData); //释放临时缓存
		return bResult;
}


// **************************************************
// 声明：static BOOL PackMIME_PartHeader(LPDATA_DEAL lpMMS_Data,LPMIME_PART lpMIME_Part)
// 参数：
//	OUT lpMMS_Data -- 用来存放编码完成MMS数据结构
//	IN lpMIME_Part -- 要编码的MIME PART内容
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：对MIME PART部分进行编码。
// 引用: 
// **************************************************
static BOOL PackMIME_PartHeader(LPDATA_DEAL lpMMS_Data,LPMIME_PART lpMIME_Part)
{

		// PART ITEM 的开始是 Content_Type
		WriteContent_Type(lpMMS_Data,lpMIME_Part->lpContent_Type);

		if (lpMIME_Part->uMask & PARTITEM_CONTENT_URL)
		{ // 是 “content_url”
			PackPartItemID(lpMMS_Data,PARTITEM_CONTENT_URL); // 编码ID
//			WriteTextString(lpMMS_Data,lpMIME_Part->lpContent_Url); // 写入内容URL
			VarEnc_TextStr(lpMMS_Data,lpMIME_Part->lpContent_Url);
		}
		if (lpMIME_Part->uMask & PARTITEM_CONTENT_ID)
		{ // 是“content ID”
			PackPartItemID(lpMMS_Data,PARTITEM_CONTENT_ID); // 编码ID
//			WriteTextString(lpMMS_Data,lpMIME_Part->lpContent_ID); //写入内容ID
			VarEnc_TextStr(lpMMS_Data,lpMIME_Part->lpContent_ID);
		}
		if (lpMIME_Part->uMask & PARTITEM_CONTENT_DES)
		{ // 是“content Description”
			PackPartItemID(lpMMS_Data,PARTITEM_CONTENT_DES);  // 编码ID
//			WriteTextString(lpMMS_Data,lpMIME_Part->lpContent_Description); //写入内容说明
			VarEnc_TextStr(lpMMS_Data,lpMIME_Part->lpContent_Description);
		}
		if (lpMIME_Part->uMask & PARTITEM_CONTENT_LOCAL)
		{  // 是文件名
			PackPartItemID(lpMMS_Data,PARTITEM_CONTENT_LOCAL);  // 编码ID
//			WriteTextString(lpMMS_Data,lpMIME_Part->lpContent_Location); //写入文件名
			VarEnc_TextStr(lpMMS_Data,lpMIME_Part->lpContent_Location);
		}
		if (lpMIME_Part->uMask & PARTITEM_DATE)
		{  //是文件日期
			PackPartItemID(lpMMS_Data,PARTITEM_DATE); // 编码ID
			//WriteDateValue(lpMMS_Data,lpMIME_Part->dwDate);
			VarEnc_Date(lpMMS_Data,lpMIME_Part->dwDate);
		}
		return TRUE;
}


// **************************************************
// 声明：static BOOL PackPartItemID(LPDATA_DEAL lpMMS_Data,UINT uID)
// 参数：
//	OUT lpMMS_Data -- 用来存放编码完成MMS数据结构
//	IN uID -- 要编码的条目ID
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：编码MIME的PART的ID。
// 引用: 
// **************************************************
static BOOL PackPartItemID(LPDATA_DEAL lpMMS_Data,UINT uID)
{
	SHORT_STR PartItemID;
	BOOL bResult ;

		switch(uID)
		{
			case PARTITEM_DATE:
//				PartItemID.bMask = MASK_INTEGRES;
				PartItemID.fIsString = FALSE;
				PartItemID.bVal = 0x12;
				break;
			case PARTITEM_CONTENT_LOCAL:
//				PartItemID.bMask = MASK_INTEGRES;
				PartItemID.fIsString = FALSE;
				PartItemID.bVal = 0x0E;
				break;
			case PARTITEM_CONTENT_URL:
//				PartItemID.bMask = MASK_INTEGRES;
				PartItemID.fIsString = FALSE;
				PartItemID.bVal = 0x30;
				break;
			case PARTITEM_CONTENT_ID:
//				PartItemID.bMask = MASK_STRING;
				PartItemID.fIsString = TRUE;
				PartItemID.pszString = "Content-ID";
				break;
			case PARTITEM_CONTENT_DES:
//				PartItemID.bMask = MASK_STRING;
				PartItemID.fIsString = TRUE;
				PartItemID.pszString = "Content-Description";
				break;
		}


		bResult = WriteField_name(lpMMS_Data,&PartItemID );
		return bResult;
}


// **************************************************
// 声明：void MIME_Release(LPBODYDETAIL lpBodyDetail)
// 参数：
//	IN lpBodyDetail -- 要释放的MMS body结构指针
// 
// 返回值：无
// 功能描述：释放MMS body空间。
// 引用: 
// **************************************************
void MIME_Release(LPBODYDETAIL lpBodyDetail)
{
	LPMIME_PART lpNext;

	while (lpBodyDetail->lpMIME_Part)
	{
		lpNext = lpBodyDetail->lpMIME_Part->next; // 得到下一个PART
		ReleasePart(lpBodyDetail->lpMIME_Part); // 释放PART空间
		lpBodyDetail->lpMIME_Part = lpNext; // 指向下一个PART
	}

	free(lpBodyDetail); // 释放DETAIL空间
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
	if (lpMIME_Part == NULL)
		return ;

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
}

// **************************************************
// 声明：static LPTSTR UnpackContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
// 参数：
//	IN lpMMS_Data -- 要解包的MMS数据结构
//	OUT pbOver -- 返回已经没有数据的标志
// 
// 返回值：返回当前消息的内容类型
// 功能描述：得到消息的内容类型。
// 备注： 参考资料 -- WAP-230-WSP-20010705-a  8.4.2.24
//		 Content_Type 的值有两种格式组成，一种是已知媒体类型的短格式和未知媒体类型的通常格式
// 引用: 
// **************************************************
static LPTSTR UnpackContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
{
	BYTE bFirstValue;
	int iValueLen;
	LPTSTR lpContentType;

//		bFirstValue = GetByte(lpMMS_Data,pbOver); // 得到内容的第一个字符
		VarPeek_Byte(lpMMS_Data,&bFirstValue); // 得到内容的第一个字符		
		// 第一个字符的格式定义 -- WAP-230-WSP-20010705-a 8.4.1.2

		if (bFirstValue < 31)
		{  // 当前的值就是数据的长度
			//ReadByte(lpMMS_Data,pbOver); // 从数据中读取这个BYTE
			VarDec_Byte(lpMMS_Data,(BYTE *)&bFirstValue);// 得到内容长度
			iValueLen = bFirstValue ; // 得到内容长度
		}
		else if (bFirstValue == 31)
		{ // 接下来紧跟着一Unsigned int 型的数据长度
//			ReadByte(lpMMS_Data,pbOver); // 从数据中读取这个BYTE
			VarDec_Byte(lpMMS_Data,(BYTE *)&bFirstValue);// 从数据中读取这个BYTE
//			iValueLen = ReadUnsignedIntegers(lpMMS_Data,pbOver); // 得到内容长度
			VarDec_UintVar(lpMMS_Data,&iValueLen);
		}
		else
		{
			lpContentType = ReadContent_Type(lpMMS_Data,pbOver);
			return lpContentType;
		}
		
		// 现在要得到CONTENT TYPE VALUE 的定义  -- Media-type 部分
		// 参考资料 WAP-230-WSP-20010705-a  8.4.2.24

		lpContentType = UnpackMedia_Type(lpMMS_Data,pbOver,iValueLen);// 解包 Media Type
		RETAILMSG(1,(TEXT("Content is general-form success\r\n")));
		return lpContentType;
}


// **************************************************
// 声明：static LPTSTR UnpackMedia_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, unsigned int iValueLen)
// 参数：
//	IN lpMMS_Data -- 要解包的MMS数据结构
//	OUT pbOver -- 返回已经没有数据的标志
//	IN iValueLen -- 当前Media_Type的长度
// 
// 返回值：返回当前消息的内容类型
// 功能描述：得到消息的内容类型。
// 备注： 参考资料 -- WAP-230-WSP-20010705-a  8.4.2.24 -- Media-type 部分		 
// 引用: 
// **************************************************
static LPTSTR UnpackMedia_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, unsigned int iValueLen)
{
	DWORD dwOldPos;
	LPTSTR lpContentType;


		dwOldPos = lpMMS_Data->dwLenDealed; // 保留当前的位置，用来计算MEDIA TYPE 下是否还有内容
		lpContentType = ReadContent_Type(lpMMS_Data,pbOver); //得到CONTENT-TYPE
		if (lpContentType == NULL)
		{
			RETAILMSG(1,(TEXT("!!! Error Media Type \r\n")));
			return NULL;
		}

		RETAILMSG(1,(TEXT("Content is general-form [%s]\r\n"),lpContentType));
		if ((lpMMS_Data->dwLenDealed - dwOldPos) == iValueLen)
		{
			// 数据已经处理完成，没有参数
			return lpContentType;
		}
		// 还有参数需要处理,我们暂时不处理参数
		lpMMS_Data->dwLenDealed = dwOldPos + iValueLen;  // 调过不要处理的数据

		return lpContentType; // 返回成功
}


// **************************************************
// 声明：static BOOL ReadField_name(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, SHORT_STR *lpFieldName)
// 参数：
//	IN lpMMS_Data -- 要解包的MMS数据结构
//	OUT pbOver -- 返回已经没有数据的标志
//	OUT lpFieldName -- 返回得到的内容
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：读取一个Field_name型的数据。
// 备注：读取数据后，当前的数据指针指向下一个BYTE
//	Field-name = Token-text | Well-known-field-name
//	Well-known-field-name = Short-integer
// 引用: 
// **************************************************
static BOOL ReadField_name(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, SHORT_STR *lpFieldName)
{
	BYTE bType;
//	bType = GetByte(lpMMS_Data,pbOver);
	VarPeek_Byte(lpMMS_Data,&bType); // 得到内容的第一个字符		
	if (bType & 0x80)
	{
		lpFieldName->fIsString = FALSE;
//		lpFieldName->bIntegres = ReadByte(lpMMS_Data,pbOver); // 读取 Short-integer 型的TYPE
		VarDec_ShortInt(lpMMS_Data,&lpFieldName->bVal); // 读取 Short-integer 型的TYPE
		RETAILMSG(1,(TEXT(" Int = [%d]\r\n"),lpFieldName->bVal));
	}
	else
	{
		lpFieldName->fIsString = TRUE;
//		lpFieldName->lpString= ReadTextString(lpMMS_Data,pbOver); // 读取Extension-Media型的TYPE
		VarDec_TextStr(lpMMS_Data,&lpFieldName->pszString);
		RETAILMSG(1,(TEXT(" String = [%s]\r\n"),lpFieldName->pszString));
	}
	return TRUE;
}


// **************************************************
// 声明：static BOOL WriteContent_Type(LPDATA_DEAL lpMMS_Data,LPTSTR lpContent_Type)
// 参数：
//	OUT lpMMS_Data -- 用来存放编码完成MMS数据结构
//	IN lpContent_Type -- 要进行编码的Content-Type
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：编码内容类型数据。
// 备注： 参考资料 -- WAP-230-WSP-20010705-a  8.4.2.24
//		 Content_Type 的值有两种格式组成，一种是已知媒体类型的短格式和未知媒体类型的通常格式
// 引用: 
// **************************************************
static BOOL WriteContent_Type(LPDATA_DEAL lpMMS_Data,LPTSTR lpContent_Type)
{
	LPTSTR lpContentType = NULL;

		// 有指定的内容类型
		CNTTYPE_ASSIGN stCntTypeAssign;

		if (strlen(lpContent_Type) >= LEN_WSP_CNTTYPE_NAME)
		{
			// 肯定不是已知的彩信内容类型
			return VarEnc_TextStr(lpMMS_Data,lpContent_Type);
		}
		else
		{
			strcpy(stCntTypeAssign.szName,lpContent_Type);
			//  查找当前彩信类型是否为已知的类型
			if (VarFind_CntTypeAssign( CNTTYPEFIND_NUM_BYNAME, &stCntTypeAssign ) == TRUE)
			{
				// 已经查到，是已知的彩信内容类型
				return VarEnc_ShortInt(lpMMS_Data,(BYTE)stCntTypeAssign.dwNum);
			}
			else
			{
				// 不是已知的彩信内容类型
				return VarEnc_TextStr(lpMMS_Data,lpContent_Type);
			}
		}

		return 0;
}

// **************************************************
// 声明：static BOOL WriteField_name(LPDATA_DEAL lpMMS_Data, LPUN_SHORT_LPTSTR lpUn_short_lptstr)
// 参数：
//	OUT lpMMS_Data -- 用来存放编码完成MMS数据结构
//	IN lpFieldName -- 要进行编码的内容，如果INT有效，bMask = MASK_INTEGRES，否则bMask = MASK_STRING
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：编码一个Field_name型的数据。
// 备注：
//	Field-name = Token-text | Well-known-field-name
//	Well-known-field-name = Short-integer
// 引用: 
// **************************************************
static BOOL WriteField_name(LPDATA_DEAL lpMMS_Data, SHORT_STR * lpFieldName)
{
	if (lpFieldName->fIsString == FALSE)
	{
//		WriteByte(lpMMS_Data,lpUn_short_lptstr->bIntegres); // 写入 Short-integer 型的TYPE
		VarEnc_ShortInt(lpMMS_Data,(BYTE)lpFieldName->bVal);// 写入 Short-integer 型的TYPE
		RETAILMSG(1,(TEXT(" Int = [%d]\r\n"),lpFieldName->bVal));
	}
	else
	{
//		WriteTextString(lpMMS_Data,lpUn_short_lptstr->lpString); // 写入Extension-Media型的TYPE
		VarEnc_TextStr(lpMMS_Data,lpFieldName->pszString); // 写入Extension-Media型的TYPE
		RETAILMSG(1,(TEXT(" String = [%s]\r\n"),lpFieldName->pszString));
	}
	return TRUE;
}


// **************************************************
// 声明：static LPTSTR ReadContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
// 参数：
//	IN lpMMS_Data -- 要解包的MMS数据结构
//	OUT pbOver -- 返回已经没有数据的标志
// 
// 返回值：返回CONTENT 的类型
// 功能描述：得到CONTENT 的类型。
// 备注： 参考资料 -- WAP-230-WSP-20010705-a  8.4.2.24
//		 Content_Type 的值有两种格式组成，一种是已知媒体类型的短格式和未知媒体类型的通常格式
// 引用: 
//		(Well-known-media | Extension-Media)
// **************************************************
static LPTSTR ReadContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
{
	BYTE bFirstValue;
	LPTSTR lpContentType = NULL;

//		bFirstValue = GetByte(lpMMS_Data,pbOver); // 得到内容的第一个字符
		VarPeek_Byte(lpMMS_Data,&bFirstValue); // 得到内容的第一个字符		
		if (bFirstValue >= 0x80)
		{
			// 是一个已知的短格式的媒体内容,返回已知的内容
			CNTTYPE_ASSIGN stCntTypeAssign;
			BYTE bContentType;

//			ReadByte(lpMMS_Data,pbOver); // 从数据中读取这个BYTE
			VarDec_ShortInt(lpMMS_Data,&bContentType);// 从数据中读取这个BYTE
//			bContentType = bFirstValue & 0x7F;
			//lpContentType = GetWell_Know_Type(bContentType);
	
			stCntTypeAssign.dwNum = bContentType;
			//  查找已知的类型
			if (VarFind_CntTypeAssign( CNTTYPEFIND_NAME_BYNUM, &stCntTypeAssign ) == TRUE)
			{
				// 已经查到
				lpContentType = AllocCopyString(stCntTypeAssign.szName);
				RETAILMSG(1,(TEXT("Content is Well know Type [%s]\r\n"),lpContentType));
			}
			else
			{
				lpContentType = NULL;
				RETAILMSG(1,(TEXT("Content is Well know Type [NULL]\r\n")));
			}
			return  lpContentType;
		}
		else
		{
			// 是一个以 '\0'为结束符的字串
//			lpContentType = ReadTextString(lpMMS_Data,pbOver);
			VarDec_TextStr(lpMMS_Data,&lpContentType);
			RETAILMSG(1,(TEXT("Content is Text String [%s]\r\n"),lpContentType));
			return lpContentType;
		}

		return NULL; // 
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
			MessageBox(NULL,TEXT("The memory is not enough"),TEXT("memory alloc failure"),MB_OK);
			return NULL;
		}
		strcpy(ptr,pSrc);
		return ptr;
}
