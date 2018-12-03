/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����MIME����PACK��UNPACK����
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-05-15
���ߣ��½��� JAMI
�޸ļ�¼��
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
�ο����ϣ� RFC2045,RFC2047,RFC2048,RFC2049
			WAP-230-WSP-20010705-a 
����˵����

****************************************************/



// ****************************************************
// ��������
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
// ������LPBODYDETAIL MIME_UnPack(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
// ������
//	IN lpMMS_Data -- Ҫ�����MMS���ݽṹ
//	OUT pbOver -- �����Ѿ�û�����ݵı�־
// 
// ����ֵ���ɹ�����MIME�Ľṹ�����򷵻�NULL
// �������������MIME���֡�
// ����: 
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

	
//		bPartNum = ReadByte(lpMMS_Data,pbOver); // �õ���ǰMIME��������PART��Ŀ
		bRet = VarDec_Byte(lpMMS_Data,&bPartNum);// �õ���ǰMIME��������PART��Ŀ
		if (bPartNum== 0) // û���κε�PART����
			return NULL; // û��ý������

		lpBodyDetail = (LPBODYDETAIL)malloc(sizeof(BODYDETAIL)); // ����һ��MIME BODY �ṹ
		if (lpBodyDetail == NULL)
		{  // �����ڴ�ʧ��
			RETAILMSG(1,(TEXT("!!! ERROR malloc in MIME_UnPack \r\n")));
			return NULL;
		}
		// ��ʼ���ṹ
		lpBodyDetail->iPartNum = bPartNum;
		lpBodyDetail->lpMIME_Part = NULL;

		for ( i = 0; i < bPartNum; i++)
		{
			// �õ��������ֵ�����

			// �õ�Content header size
			// ��PART�Ŀ�ʼ��������PART HEADER �ĳ��ȣ���������PART���ݵĳ���
//			iPartHeaderSize = ReadUnsignedIntegers(lpMMS_Data,pbOver); // �õ�Header�ĳ���
			VarDec_UintVar(lpMMS_Data,&iPartHeaderSize);// �õ�Header�ĳ���
//			iPartSize = ReadUnsignedIntegers(lpMMS_Data,pbOver); // �õ����ݵĳ���
			VarDec_UintVar(lpMMS_Data,&iPartSize);// �õ����ݵĳ���
			
			// �õ�Content header
			lpMIME_Part = PraseMIME_PartHeader(lpMMS_Data,pbOver,iPartHeaderSize);
			if (lpMIME_Part == NULL)
			{
				RETAILMSG(1,(TEXT("!!!!Prase MIME Part Error \r\n")));
				goto ERROR_RETURN;
			}
			lpMIME_Part->uLen = iPartSize; // �õ����ݵĳ���
			lpMIME_Part->uPos = lpMMS_Data->dwLenDealed;  // �õ����ݵ�λ��
			lpMMS_Data->dwLenDealed += iPartSize;  // ����һ��PART��λ��

			// �õ�Content
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
// ������static LPMIME_PART PraseMIME_PartHeader(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,UINT iPartHeaderSize)
// ������
//	IN lpMMS_Data -- Ҫ�����MMS���ݽṹ
//	OUT pbOver -- �����Ѿ�û�����ݵı�־
// 
// ����ֵ���ɹ�����MIME��PARTͷ�Ľṹ�����򷵻�NULL
// �������������MIME��PARTͷ��Ϣ��
// ����: 
// **************************************************
static LPMIME_PART PraseMIME_PartHeader(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,UINT iPartHeaderSize)
{
	LPMIME_PART lpMIME_Part;
	UINT ItemID;
	DATA_DEAL MMS_PartHeader;
	BOOL bRet;

		// ��ʼ��Ҫ���������
		MMS_PartHeader.dwLenDealed = 0;
		MMS_PartHeader.dwLenTotal = iPartHeaderSize;
		MMS_PartHeader.lpData = lpMMS_Data->lpData + lpMMS_Data->dwLenDealed;
		MMS_PartHeader.dwFlag = DDF_NOWRITE;
		MMS_PartHeader.dwLenGrow = 0;
		MMS_PartHeader.dwLenMaxAlloc = 0;

//		dwOldPos = lpMMS_Data->dwLenDealed; // ������ǰ��λ�ã���������PartHeader�Ƿ�������
		lpMIME_Part = (LPMIME_PART)malloc(sizeof(MIME_PART)); // ����һ��MIME��PART�ṹ

		if (lpMIME_Part == NULL)
		{ // �����ڴ�ʧ��
			RETAILMSG(1,(TEXT("!!! Error Malloc in PraseMIME_PartHeader \r\n")));
			return NULL;
		}

		// ��ʼ���ṹ
		lpMIME_Part->lpContent_ID = NULL;
		lpMIME_Part->lpContent_Type = NULL;
		lpMIME_Part->lpContent_Location = NULL;
		lpMIME_Part->lpContent_Description = NULL;
		lpMIME_Part->lpContent_Url = NULL;

		lpMIME_Part->next = NULL;

		// PART ITEM �Ŀ�ʼ�� Content_Type
//		lpMIME_Part->lpContent_Type = ReadContent_Type(&MMS_PartHeader,pbOver);
		lpMIME_Part->lpContent_Type = UnpackContent_Type(&MMS_PartHeader,pbOver);

		while(1)
		{
			// Read Parameter
//			iSize = lpMMS_Data->dwLenDealed - dwOldPos ; // �õ��Ѿ���ȡ��������

//			if (iSize == iPartHeaderSize)
//				break; // �����Ѿ���ȡ���
			if (MMS_PartHeader.dwLenDealed >= MMS_PartHeader.dwLenTotal)
			{  // �����Ѿ��������
				RETAILMSG(1,(TEXT("!!! Error PART HEADER Size overflow \r\n")));
				break; // �����Ѿ���ȡ̫�࣬��ɴ���
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

		lpMMS_Data->dwLenDealed += iPartHeaderSize; // �����Ѿ��������
		return lpMIME_Part;
}


// **************************************************
// ������static BOOL SavePartData(LPDATA_DEAL lpMMS_Data,BOOL *pbOver,LPTSTR lpFileName,unsigned int iPartSize)
// ������
//	IN lpMMS_Data -- Ҫ�����MMS���ݽṹ
//	OUT pbOver -- �����Ѿ�û�����ݵı�־
//  IN lpFileName -- Ҫ�����ݱ�����ļ���
//  IN iPartSize -- Ҫ��������ݵĳ���
// 
// ����ֵ���ɹ�����MIME�Ľṹ�����򷵻�NULL
// �������������MIME���֡�
// ����: 
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
// ������static UINT GetPartItemID(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
// ������
//	IN lpMMS_Data -- Ҫ�����MMS���ݽṹ
//	OUT pbOver -- �����Ѿ�û�����ݵı�־
// 
// ����ֵ���ɹ�����MIME��PART��ID�����򷵻�PARTITEM_UNKNOW
// �������������MIME��PART��ID��
// ����: 
// **************************************************
static UINT GetPartItemID(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
{
	SHORT_STR PartItemID;

		ReadField_name(lpMMS_Data,pbOver,&PartItemID );
//		if (PartItemID.bMask == MASK_INTEGRES)
		if (PartItemID.fIsString == FALSE)
		{  // ��ǰ��Item��һ����֪�ı�ʶ
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
			// ��ǰ��Item��һ����չ�ı�ʶ
			if (stricmp(TEXT("Content-ID"),PartItemID.pszString) == 0)
			{
				// ��Content-ID
				return PARTITEM_CONTENT_ID;
			}
			if (stricmp(TEXT("Content-Description"),PartItemID.pszString) == 0)
			{
				// ��Content-Description
				return PARTITEM_CONTENT_DES;
			}
		}

		return PARTITEM_UNKNOW;
}

// **************************************************
// ������static void InsertPartToBody(LPBODYDETAIL lpBodyDetail,LPMIME_PART lpMIME_Part)
// ������
//	IN lpBodyDetail -- MIME��Body�ṹ
//	OUT lpMIME_Part -- Ҫ�����PART
// 
// ����ֵ����
// ������������һ��PART�Ľṹ���뵽MIME Body �ṹ��
// ����: 
// **************************************************
static void InsertPartToBody(LPBODYDETAIL lpBodyDetail,LPMIME_PART lpMIME_Part)
{
	LPMIME_PART lpCurPart;

		if (lpBodyDetail->lpMIME_Part == NULL)
		{  // ��ǰ��û��
			lpBodyDetail->lpMIME_Part = lpMIME_Part;
			return;
		}
		else
		{
			// �Ѿ�����PART����
			lpCurPart = lpBodyDetail->lpMIME_Part;
			while(1)
			{
				if (lpCurPart->next == NULL)
				{
					// �����Ѿ�û������
					lpCurPart->next = lpMIME_Part;
					break;
				}
				lpCurPart = lpCurPart->next; // ����һ��PART
			}
		}
}



// **************************************************
// ������BOOL MIME_Pack(LPDATA_DEAL lpMMS_Data,LPBODYDETAIL lpBodyDetail)
// ������
//	OUT lpMMS_Data -- ������ű������MMS���ݽṹ
//	IN lpBodyDetail -- Ҫ�����MIME����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������MIME���ֽ��б��롣
// ����: 
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
		{  // �����ڴ�ʧ��
			RETAILMSG(1,(TEXT("!!! ERROR Parameter in MIME_Pack \r\n")));
			return FALSE;
		}
//		WriteByte(lpMMS_Data,(BYTE)lpBodyDetail->iPartNum); // д�뵱ǰ��Ҫ������ļ���Ŀ
		VarEnc_Byte(lpMMS_Data,(BYTE)lpBodyDetail->iPartNum);// д�뵱ǰ��Ҫ������ļ���Ŀ

		MMS_Temp.dwFlag = DDF_GROW;
		MMS_Temp.dwLenTotal = 256 ; 
		MMS_Temp.lpData = (BYTE *)malloc(MMS_Temp.dwLenTotal);
		if (MMS_Temp.lpData == NULL)
		{
			// �����ڴ�ʧ��
			return FALSE;
		}
		memset(MMS_Temp.lpData, 0, MMS_Temp.dwLenTotal);
		MMS_Temp.dwLenDealed = 0;
		MMS_Temp.dwLenGrow = DDGROW_DEFAULT;

		lpMIME_Part = lpBodyDetail->lpMIME_Part; //�õ���һ���ļ���λ��
		for ( i = 0; i < lpBodyDetail->iPartNum; i++)
		{
			// ����������ֵ�����

			// �õ�Content header size
			// ��PART�Ŀ�ʼ��������PART HEADER �ĳ��ȣ���������PART���ݵĳ���
			// �����ڱ�����ǰ��֪PART HEADER�ĳ��ȣ����Ƚ� PART HEADER���뵽һ����ʱ�ļ�
//			iPartHeaderSize = ReadUnsignedIntegers(lpMMS_Data,pbOver); // �õ�Header�ĳ���
//			iPartSize = ReadUnsignedIntegers(lpMMS_Data,pbOver); // �õ����ݵĳ���
			MMS_Temp.dwLenDealed = 0;
			
			// �õ�Content header
			bResult = PackMIME_PartHeader(&MMS_Temp,lpMIME_Part);
			iPartHeaderSize = MMS_Temp.dwLenDealed; //�Ѿ������ĳ���

			// ��Ҫ������ļ�
			hFile = CreateFile( lpMIME_Part->lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
			if( hFile == INVALID_HANDLE_VALUE )
			{
				bResult = FALSE;
				goto ERROR_RETURN;
			}
			iPartSize = GetFileSize(hFile,0); 

			// д��PART HEADER�ĳ���
//			WriteUnsignedIntegers(lpMMS_Data,iPartHeaderSize);
			VarEnc_UintVar(lpMMS_Data,iPartHeaderSize);

			// д��PART ���ݵĳ���
//			WriteUnsignedIntegers(lpMMS_Data,iPartSize);
			VarEnc_UintVar(lpMMS_Data,iPartSize);

			// д��PART HEADER
			// �������ǽ���ʱ�����ڵ����ݸ��Ƶ���ǰ����

			if ((lpMMS_Data->dwLenTotal - lpMMS_Data->dwLenDealed) < MMS_Temp.dwLenDealed)
			{
				//  ����ռ䲻��
				bResult = FALSE;
				CloseHandle(hFile); // �ر��ļ����
				goto ERROR_RETURN;
			}

			// ��������
			memcpy((lpMMS_Data->lpData + lpMMS_Data->dwLenDealed ),MMS_Temp.lpData,MMS_Temp.dwLenDealed);
			lpMMS_Data->dwLenDealed += MMS_Temp.dwLenDealed;

			// д��PART ����

			if ((lpMMS_Data->dwLenTotal - lpMMS_Data->dwLenDealed) < iPartSize)
			{
				//  ����ռ䲻��
				bResult = FALSE;
				CloseHandle(hFile); // �ر��ļ����
				goto ERROR_RETURN;
			}
//			SavePartData(lpMMS_Data,pbOver,lpMIME_Part->lpPath,iPartSize);
			lpData = lpMMS_Data->lpData + lpMMS_Data->dwLenDealed;
			ReadFile(hFile,lpData,iPartSize,&dwReadLen,NULL); // ����ǰ�ļ������ݱ��뵽�ڴ�
			CloseHandle(hFile); // �ر��ļ����
			lpMMS_Data->dwLenDealed += iPartSize;

			lpMIME_Part = lpMIME_Part->next; //��ʼ��һ��PART
		}

ERROR_RETURN:
		// Will Release the alloc memory space
		free(MMS_Temp.lpData); //�ͷ���ʱ����
		return bResult;
}


// **************************************************
// ������static BOOL PackMIME_PartHeader(LPDATA_DEAL lpMMS_Data,LPMIME_PART lpMIME_Part)
// ������
//	OUT lpMMS_Data -- ������ű������MMS���ݽṹ
//	IN lpMIME_Part -- Ҫ�����MIME PART����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������MIME PART���ֽ��б��롣
// ����: 
// **************************************************
static BOOL PackMIME_PartHeader(LPDATA_DEAL lpMMS_Data,LPMIME_PART lpMIME_Part)
{

		// PART ITEM �Ŀ�ʼ�� Content_Type
		WriteContent_Type(lpMMS_Data,lpMIME_Part->lpContent_Type);

		if (lpMIME_Part->uMask & PARTITEM_CONTENT_URL)
		{ // �� ��content_url��
			PackPartItemID(lpMMS_Data,PARTITEM_CONTENT_URL); // ����ID
//			WriteTextString(lpMMS_Data,lpMIME_Part->lpContent_Url); // д������URL
			VarEnc_TextStr(lpMMS_Data,lpMIME_Part->lpContent_Url);
		}
		if (lpMIME_Part->uMask & PARTITEM_CONTENT_ID)
		{ // �ǡ�content ID��
			PackPartItemID(lpMMS_Data,PARTITEM_CONTENT_ID); // ����ID
//			WriteTextString(lpMMS_Data,lpMIME_Part->lpContent_ID); //д������ID
			VarEnc_TextStr(lpMMS_Data,lpMIME_Part->lpContent_ID);
		}
		if (lpMIME_Part->uMask & PARTITEM_CONTENT_DES)
		{ // �ǡ�content Description��
			PackPartItemID(lpMMS_Data,PARTITEM_CONTENT_DES);  // ����ID
//			WriteTextString(lpMMS_Data,lpMIME_Part->lpContent_Description); //д������˵��
			VarEnc_TextStr(lpMMS_Data,lpMIME_Part->lpContent_Description);
		}
		if (lpMIME_Part->uMask & PARTITEM_CONTENT_LOCAL)
		{  // ���ļ���
			PackPartItemID(lpMMS_Data,PARTITEM_CONTENT_LOCAL);  // ����ID
//			WriteTextString(lpMMS_Data,lpMIME_Part->lpContent_Location); //д���ļ���
			VarEnc_TextStr(lpMMS_Data,lpMIME_Part->lpContent_Location);
		}
		if (lpMIME_Part->uMask & PARTITEM_DATE)
		{  //���ļ�����
			PackPartItemID(lpMMS_Data,PARTITEM_DATE); // ����ID
			//WriteDateValue(lpMMS_Data,lpMIME_Part->dwDate);
			VarEnc_Date(lpMMS_Data,lpMIME_Part->dwDate);
		}
		return TRUE;
}


// **************************************************
// ������static BOOL PackPartItemID(LPDATA_DEAL lpMMS_Data,UINT uID)
// ������
//	OUT lpMMS_Data -- ������ű������MMS���ݽṹ
//	IN uID -- Ҫ�������ĿID
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������MIME��PART��ID��
// ����: 
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
// ������void MIME_Release(LPBODYDETAIL lpBodyDetail)
// ������
//	IN lpBodyDetail -- Ҫ�ͷŵ�MMS body�ṹָ��
// 
// ����ֵ����
// �����������ͷ�MMS body�ռ䡣
// ����: 
// **************************************************
void MIME_Release(LPBODYDETAIL lpBodyDetail)
{
	LPMIME_PART lpNext;

	while (lpBodyDetail->lpMIME_Part)
	{
		lpNext = lpBodyDetail->lpMIME_Part->next; // �õ���һ��PART
		ReleasePart(lpBodyDetail->lpMIME_Part); // �ͷ�PART�ռ�
		lpBodyDetail->lpMIME_Part = lpNext; // ָ����һ��PART
	}

	free(lpBodyDetail); // �ͷ�DETAIL�ռ�
}

// **************************************************
// ������static void ReleasePart(LPMIME_PART lpMIME_Part)
// ������
//	IN lpMIME_Part -- Ҫ�ͷŵ�PART�ṹָ��
// 
// ����ֵ����
// �����������ͷ�PART�ռ䡣
// ����: 
// **************************************************
static void ReleasePart(LPMIME_PART lpMIME_Part)
{
	if (lpMIME_Part == NULL)
		return ;

	if (lpMIME_Part->lpContent_Description)
		free(lpMIME_Part->lpContent_Description); //�ͷ�Content - Description
	
	if (lpMIME_Part->lpContent_ID)
		free(lpMIME_Part->lpContent_ID);  //�ͷ�content - ID

	if (lpMIME_Part->lpContent_Type)
		free(lpMIME_Part->lpContent_Type);  // �ͷ�content - type

	if (lpMIME_Part->lpContent_Url)
		free(lpMIME_Part->lpContent_Url);   // �ͷ� content url

	if (lpMIME_Part->lpContent_Location)
		free(lpMIME_Part->lpContent_Location);  // �ͷ� conten location

	free(lpMIME_Part); // �ͷ�part�ṹ
}

// **************************************************
// ������static LPTSTR UnpackContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
// ������
//	IN lpMMS_Data -- Ҫ�����MMS���ݽṹ
//	OUT pbOver -- �����Ѿ�û�����ݵı�־
// 
// ����ֵ�����ص�ǰ��Ϣ����������
// �����������õ���Ϣ���������͡�
// ��ע�� �ο����� -- WAP-230-WSP-20010705-a  8.4.2.24
//		 Content_Type ��ֵ�����ָ�ʽ��ɣ�һ������֪ý�����͵Ķ̸�ʽ��δ֪ý�����͵�ͨ����ʽ
// ����: 
// **************************************************
static LPTSTR UnpackContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
{
	BYTE bFirstValue;
	int iValueLen;
	LPTSTR lpContentType;

//		bFirstValue = GetByte(lpMMS_Data,pbOver); // �õ����ݵĵ�һ���ַ�
		VarPeek_Byte(lpMMS_Data,&bFirstValue); // �õ����ݵĵ�һ���ַ�		
		// ��һ���ַ��ĸ�ʽ���� -- WAP-230-WSP-20010705-a 8.4.1.2

		if (bFirstValue < 31)
		{  // ��ǰ��ֵ�������ݵĳ���
			//ReadByte(lpMMS_Data,pbOver); // �������ж�ȡ���BYTE
			VarDec_Byte(lpMMS_Data,(BYTE *)&bFirstValue);// �õ����ݳ���
			iValueLen = bFirstValue ; // �õ����ݳ���
		}
		else if (bFirstValue == 31)
		{ // ������������һUnsigned int �͵����ݳ���
//			ReadByte(lpMMS_Data,pbOver); // �������ж�ȡ���BYTE
			VarDec_Byte(lpMMS_Data,(BYTE *)&bFirstValue);// �������ж�ȡ���BYTE
//			iValueLen = ReadUnsignedIntegers(lpMMS_Data,pbOver); // �õ����ݳ���
			VarDec_UintVar(lpMMS_Data,&iValueLen);
		}
		else
		{
			lpContentType = ReadContent_Type(lpMMS_Data,pbOver);
			return lpContentType;
		}
		
		// ����Ҫ�õ�CONTENT TYPE VALUE �Ķ���  -- Media-type ����
		// �ο����� WAP-230-WSP-20010705-a  8.4.2.24

		lpContentType = UnpackMedia_Type(lpMMS_Data,pbOver,iValueLen);// ��� Media Type
		RETAILMSG(1,(TEXT("Content is general-form success\r\n")));
		return lpContentType;
}


// **************************************************
// ������static LPTSTR UnpackMedia_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, unsigned int iValueLen)
// ������
//	IN lpMMS_Data -- Ҫ�����MMS���ݽṹ
//	OUT pbOver -- �����Ѿ�û�����ݵı�־
//	IN iValueLen -- ��ǰMedia_Type�ĳ���
// 
// ����ֵ�����ص�ǰ��Ϣ����������
// �����������õ���Ϣ���������͡�
// ��ע�� �ο����� -- WAP-230-WSP-20010705-a  8.4.2.24 -- Media-type ����		 
// ����: 
// **************************************************
static LPTSTR UnpackMedia_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, unsigned int iValueLen)
{
	DWORD dwOldPos;
	LPTSTR lpContentType;


		dwOldPos = lpMMS_Data->dwLenDealed; // ������ǰ��λ�ã���������MEDIA TYPE ���Ƿ�������
		lpContentType = ReadContent_Type(lpMMS_Data,pbOver); //�õ�CONTENT-TYPE
		if (lpContentType == NULL)
		{
			RETAILMSG(1,(TEXT("!!! Error Media Type \r\n")));
			return NULL;
		}

		RETAILMSG(1,(TEXT("Content is general-form [%s]\r\n"),lpContentType));
		if ((lpMMS_Data->dwLenDealed - dwOldPos) == iValueLen)
		{
			// �����Ѿ�������ɣ�û�в���
			return lpContentType;
		}
		// ���в�����Ҫ����,������ʱ���������
		lpMMS_Data->dwLenDealed = dwOldPos + iValueLen;  // ������Ҫ���������

		return lpContentType; // ���سɹ�
}


// **************************************************
// ������static BOOL ReadField_name(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, SHORT_STR *lpFieldName)
// ������
//	IN lpMMS_Data -- Ҫ�����MMS���ݽṹ
//	OUT pbOver -- �����Ѿ�û�����ݵı�־
//	OUT lpFieldName -- ���صõ�������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ȡһ��Field_name�͵����ݡ�
// ��ע����ȡ���ݺ󣬵�ǰ������ָ��ָ����һ��BYTE
//	Field-name = Token-text | Well-known-field-name
//	Well-known-field-name = Short-integer
// ����: 
// **************************************************
static BOOL ReadField_name(LPDATA_DEAL lpMMS_Data,BOOL *pbOver, SHORT_STR *lpFieldName)
{
	BYTE bType;
//	bType = GetByte(lpMMS_Data,pbOver);
	VarPeek_Byte(lpMMS_Data,&bType); // �õ����ݵĵ�һ���ַ�		
	if (bType & 0x80)
	{
		lpFieldName->fIsString = FALSE;
//		lpFieldName->bIntegres = ReadByte(lpMMS_Data,pbOver); // ��ȡ Short-integer �͵�TYPE
		VarDec_ShortInt(lpMMS_Data,&lpFieldName->bVal); // ��ȡ Short-integer �͵�TYPE
		RETAILMSG(1,(TEXT(" Int = [%d]\r\n"),lpFieldName->bVal));
	}
	else
	{
		lpFieldName->fIsString = TRUE;
//		lpFieldName->lpString= ReadTextString(lpMMS_Data,pbOver); // ��ȡExtension-Media�͵�TYPE
		VarDec_TextStr(lpMMS_Data,&lpFieldName->pszString);
		RETAILMSG(1,(TEXT(" String = [%s]\r\n"),lpFieldName->pszString));
	}
	return TRUE;
}


// **************************************************
// ������static BOOL WriteContent_Type(LPDATA_DEAL lpMMS_Data,LPTSTR lpContent_Type)
// ������
//	OUT lpMMS_Data -- ������ű������MMS���ݽṹ
//	IN lpContent_Type -- Ҫ���б����Content-Type
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������������������ݡ�
// ��ע�� �ο����� -- WAP-230-WSP-20010705-a  8.4.2.24
//		 Content_Type ��ֵ�����ָ�ʽ��ɣ�һ������֪ý�����͵Ķ̸�ʽ��δ֪ý�����͵�ͨ����ʽ
// ����: 
// **************************************************
static BOOL WriteContent_Type(LPDATA_DEAL lpMMS_Data,LPTSTR lpContent_Type)
{
	LPTSTR lpContentType = NULL;

		// ��ָ������������
		CNTTYPE_ASSIGN stCntTypeAssign;

		if (strlen(lpContent_Type) >= LEN_WSP_CNTTYPE_NAME)
		{
			// �϶�������֪�Ĳ�����������
			return VarEnc_TextStr(lpMMS_Data,lpContent_Type);
		}
		else
		{
			strcpy(stCntTypeAssign.szName,lpContent_Type);
			//  ���ҵ�ǰ���������Ƿ�Ϊ��֪������
			if (VarFind_CntTypeAssign( CNTTYPEFIND_NUM_BYNAME, &stCntTypeAssign ) == TRUE)
			{
				// �Ѿ��鵽������֪�Ĳ�����������
				return VarEnc_ShortInt(lpMMS_Data,(BYTE)stCntTypeAssign.dwNum);
			}
			else
			{
				// ������֪�Ĳ�����������
				return VarEnc_TextStr(lpMMS_Data,lpContent_Type);
			}
		}

		return 0;
}

// **************************************************
// ������static BOOL WriteField_name(LPDATA_DEAL lpMMS_Data, LPUN_SHORT_LPTSTR lpUn_short_lptstr)
// ������
//	OUT lpMMS_Data -- ������ű������MMS���ݽṹ
//	IN lpFieldName -- Ҫ���б�������ݣ����INT��Ч��bMask = MASK_INTEGRES������bMask = MASK_STRING
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������һ��Field_name�͵����ݡ�
// ��ע��
//	Field-name = Token-text | Well-known-field-name
//	Well-known-field-name = Short-integer
// ����: 
// **************************************************
static BOOL WriteField_name(LPDATA_DEAL lpMMS_Data, SHORT_STR * lpFieldName)
{
	if (lpFieldName->fIsString == FALSE)
	{
//		WriteByte(lpMMS_Data,lpUn_short_lptstr->bIntegres); // д�� Short-integer �͵�TYPE
		VarEnc_ShortInt(lpMMS_Data,(BYTE)lpFieldName->bVal);// д�� Short-integer �͵�TYPE
		RETAILMSG(1,(TEXT(" Int = [%d]\r\n"),lpFieldName->bVal));
	}
	else
	{
//		WriteTextString(lpMMS_Data,lpUn_short_lptstr->lpString); // д��Extension-Media�͵�TYPE
		VarEnc_TextStr(lpMMS_Data,lpFieldName->pszString); // д��Extension-Media�͵�TYPE
		RETAILMSG(1,(TEXT(" String = [%s]\r\n"),lpFieldName->pszString));
	}
	return TRUE;
}


// **************************************************
// ������static LPTSTR ReadContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
// ������
//	IN lpMMS_Data -- Ҫ�����MMS���ݽṹ
//	OUT pbOver -- �����Ѿ�û�����ݵı�־
// 
// ����ֵ������CONTENT ������
// �����������õ�CONTENT �����͡�
// ��ע�� �ο����� -- WAP-230-WSP-20010705-a  8.4.2.24
//		 Content_Type ��ֵ�����ָ�ʽ��ɣ�һ������֪ý�����͵Ķ̸�ʽ��δ֪ý�����͵�ͨ����ʽ
// ����: 
//		(Well-known-media | Extension-Media)
// **************************************************
static LPTSTR ReadContent_Type(LPDATA_DEAL lpMMS_Data,BOOL *pbOver)
{
	BYTE bFirstValue;
	LPTSTR lpContentType = NULL;

//		bFirstValue = GetByte(lpMMS_Data,pbOver); // �õ����ݵĵ�һ���ַ�
		VarPeek_Byte(lpMMS_Data,&bFirstValue); // �õ����ݵĵ�һ���ַ�		
		if (bFirstValue >= 0x80)
		{
			// ��һ����֪�Ķ̸�ʽ��ý������,������֪������
			CNTTYPE_ASSIGN stCntTypeAssign;
			BYTE bContentType;

//			ReadByte(lpMMS_Data,pbOver); // �������ж�ȡ���BYTE
			VarDec_ShortInt(lpMMS_Data,&bContentType);// �������ж�ȡ���BYTE
//			bContentType = bFirstValue & 0x7F;
			//lpContentType = GetWell_Know_Type(bContentType);
	
			stCntTypeAssign.dwNum = bContentType;
			//  ������֪������
			if (VarFind_CntTypeAssign( CNTTYPEFIND_NAME_BYNUM, &stCntTypeAssign ) == TRUE)
			{
				// �Ѿ��鵽
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
			// ��һ���� '\0'Ϊ���������ִ�
//			lpContentType = ReadTextString(lpMMS_Data,pbOver);
			VarDec_TextStr(lpMMS_Data,&lpContentType);
			RETAILMSG(1,(TEXT("Content is Text String [%s]\r\n"),lpContentType));
			return lpContentType;
		}

		return NULL; // 
}

// ***************************************************************************************
// ������static LPTSTR AllocCopyString(LPTSTR pSrc)
// ������
//  IN  pSrc -- ���Ƶ��ִ�
// ����ֵ��
//		�����µ��ִ�
// ����������
//  	����һ���ռ䲢���Ƶ�ǰ�ִ����µĻ�����
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
