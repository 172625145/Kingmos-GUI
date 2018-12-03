#include <ewindows.h>
#include <mms_clt.h>
#include "mmstrans.h"
#include <ztmsgbox.h>
#include <dialdlg.h>


#define MMSTRANS_TEST

#define DEFAULTCHARSET	0			// Ĭ�ϵĲ����ַ���
#define DEFAULTCONTENTTYPE	0X21	// Ĭ�ϵĲ�����������

static LPTSTR AllocCopyString(LPTSTR pSrc);
static BOOL GetMMSContentFromNotification(MCLT_NOTIFICATION_IND *pNotification_Ind,LPMMS_CONTENT lpMMS_Content);
static BOOL GetMMSContentFromRetrieve(MCLT_RETRIEVE_CONF *pRetrieveConf,LPMMS_CONTENT lpMMS_Content);
static BOOL	FillMMSContentToSendReq(LPMMS_CONTENT lpMMS_Content,MCLT_SEND_REQ* pPduMCltReq);
static LPTSTR GetLocalNumber(void);

// ***************************************************************************************
// ������BOOL GetNotificationFromServer(DWORD dwDataID ,LPMMS_CONTENT lpMMS_Content)
// ������
//  IN  dwDataID -- Ҫ�õ���֪ͨ��Ϣ��ID
//  OUT lpMMS_Content -- �洢֪ͨ��Ϣ�Ľṹ
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	�ӷ�������ȡ����֪ͨ��
// ***************************************************************************************
BOOL GetNotificationFromServer(DWORD dwDataID ,LPMMS_CONTENT lpMMS_Content)
{
	MCLT_NOTIFICATION_IND stMCLT_Notification_Ind;
	ERROR_INFO			InfoErr;

		memset( &InfoErr, 0, sizeof(ERROR_INFO) ); // ��մ���ṹ

		if (MCltReadNotifyInd(dwDataID,&stMCLT_Notification_Ind,&InfoErr) == FALSE) // �õ�֪ͨ��
		{
			// �õ����ŵ�֪ͨ��Ϣʧ��
			return FALSE;
		}
		// ��֪ͨ���еõ�����֪ͨ��Ϣ
		GetMMSContentFromNotification(&stMCLT_Notification_Ind,lpMMS_Content);
		
		MCltFreePdu((MCLT_PDU*)&stMCLT_Notification_Ind); // �ͷŵõ���֪ͨ��

		return TRUE; // �õ�֪ͨ���ɹ�
}


// ***************************************************************************************
// ������BOOL GetMMSFromServer(LPVOID lpData, DWORD dwSize,LPMMS_CONTENT lpMMS_Content)
// ������
//  IN  lpData -- ֪ͨ��������
//	IN  dwSize -- ֪ͨ�������ݳ���
//  OUT lpMMS_Content -- �洢�������ݵĽṹ
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	�ӷ�������ȡ��������
// ***************************************************************************************
BOOL GetMMSFromServer(LPVOID lpData, DWORD dwSize,LPMMS_CONTENT lpMMS_Content)
{
	HANDLE				hMClt;
	ERROR_INFO			InfoErr;
	MCLT_RETRIEVE_CONF	stRetrieveConf;
	LPTSTR lpMMSLocation;
	HANDLE hRasConn;


		// ����ҳ֮ǰ����Ҫ���ŵ�����
		if (DailToInternet(NULL,&hRasConn,GPRS_NETTYPE_WAP) == FALSE) // ���ŵ�INTERNET
		{
			// �������ӵ�����
#ifdef ZT_PHONE
			ZTMessageBox(NULL,"�������ӵ�����","����",MB_OK|MB_TOPMOST);
#else
			MessageBox(NULL,"�������ӵ�����","����",MB_OK|MB_TOPMOST);
#endif
			return FALSE;
		}

		// Ŀǰ�洢��֪ͨ��Ϣ���ݽ����ǵ�ַ����
		lpMMSLocation = (LPTSTR)lpData;

		memset( &InfoErr, 0, sizeof(ERROR_INFO) ); // ��մ���ṹ

		if( !(hMClt=MCltOpen( &InfoErr )) )  // �򿪲��ž��
		{
			HandDownInternet(NULL,hRasConn); // �Ҷ����������ϵ
			return FALSE;
		}


		memset( &stRetrieveConf, 0, sizeof(MCLT_RETRIEVE_CONF) ); // ��ղ������ݰ��ṹ
		if( !MCltRetrieval( hMClt, lpMMSLocation, &stRetrieveConf, &InfoErr ) )  // �õ�����
		{  // �õ�����ʧ��
			MCltClose( hMClt );
			HandDownInternet(NULL,hRasConn); // �Ҷ����������ϵ
			return FALSE;
		}

		// �Ӳ������ݰ��еõ���������
		GetMMSContentFromRetrieve(&stRetrieveConf,lpMMS_Content);
		

		MCltFreePdu( (MCLT_PDU*)&stRetrieveConf ); // �ͷŲ������ݰ��ṹ

		MCltClose( hMClt ); // �رղ��ž��

		HandDownInternet(NULL,hRasConn); // �Ҷ����������ϵ
		return TRUE;
}


// ***************************************************************************************
// ������BOOL SentMMSToServer(LPMMS_CONTENT lpMMS_Content)
// ������
//  IN lpMMS_Content -- Ҫ���͵Ĳ�������
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	���Ͳ������ݵ�������
// ***************************************************************************************
BOOL SentMMSToServer(LPMMS_CONTENT lpMMS_Content)
{
	HANDLE				hMClt;
	ERROR_INFO			InfoErr;
	MCLT_SEND_REQ		PduMCltReq;
	MCLT_SEND_CONF		PduMCltConf;
	HANDLE hRasConn;

		// ����ҳ֮ǰ����Ҫ���ŵ�����
		if (DailToInternet(NULL,&hRasConn,GPRS_NETTYPE_WAP) == FALSE) // ���ŵ�INTERNET
		{
			// �������ӵ�����
#ifdef ZT_PHONE
			ZTMessageBox(NULL,"�������ӵ�����","����",MB_OK|MB_TOPMOST);
#else
			MessageBox(NULL,"�������ӵ�����","����",MB_OK|MB_TOPMOST);
#endif
			return FALSE;
		}

		memset( &PduMCltReq, 0, sizeof(PduMCltReq) );  // ��ղ��ŷ��Ͱ�
		memset( &PduMCltConf, 0, sizeof(PduMCltConf) ); // ���ȷ�ϰ�

		// ���Ҫ���͵�����
		FillMMSContentToSendReq(lpMMS_Content,&PduMCltReq);

		memset( &InfoErr, 0, sizeof(ERROR_INFO) ); // ��մ����
		if( !(hMClt=MCltOpen( &InfoErr )) ) // �򿪲��ž��
		{  // �򿪾��ʧ��
			HandDownInternet(NULL,hRasConn); // �Ҷ����������ϵ
			return FALSE;
		}


		if( !MCltSendReq( hMClt, &PduMCltReq, &PduMCltConf, &InfoErr ) )  // ���Ͳ���
		{ // ����ʧ��
			MCltFreePdu( (MCLT_PDU*)&PduMCltReq );
			MCltClose( hMClt );
			HandDownInternet(NULL,hRasConn); // �Ҷ����������ϵ
	#ifdef MMSTRANS_TEST		
			return TRUE;
	#else
			return FALSE;
	#endif
		}

		MCltFreePdu( (MCLT_PDU*)&PduMCltConf ); // �ͷ�ȷ�ϰ�

		MCltClose( hMClt ); // �رղ��ž��
		HandDownInternet(NULL,hRasConn); // �Ҷ����������ϵ
		return TRUE;
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
			MessageBox(NULL,TEXT("The memory is not enough"),TEXT("memory alloc failure"),MB_OK|MB_TOPMOST);
			return NULL;
		}
		strcpy(ptr,pSrc);
		return ptr;
}



// ***************************************************************************************
// ������static BOOL GetMMSContentFromNotification(MCLT_NOTIFICATION_IND *pNotification_Ind,LPMMS_CONTENT lpMMS_Content)
// ������
//	IN pNotification_Ind -- �õ���֪ͨ�����ݽṹ
//  OUT  lpMMS_Content -- Ҫ�����Ĳ������ݽṹ
// ����ֵ��
//		�ɹ�����TRUE�� ���򷵻�FALSE
// ����������
//  	��֪ͨ���еõ�����֪ͨ��Ϣ
// ***************************************************************************************
static BOOL GetMMSContentFromNotification(MCLT_NOTIFICATION_IND *pNotification_Ind,LPMMS_CONTENT lpMMS_Content)
{
	SYSTEMTIME stCurTime;

		// �õ�������
		if (pNotification_Ind->dwMask & MCLT_MASK_From)
		{
			// �� FROM ��
			lpMMS_Content->lpFromTo = AllocCopyString(pNotification_Ind->stFrom.stEncodedStr.pszString);
		}
		else
		{
			// �� FROM �� �� ��֪�ӺͶ�������ʾ��...��
			lpMMS_Content->lpFromTo = AllocCopyString("...");
		}

		// �õ�����
		if (pNotification_Ind->dwMask & MCLT_MASK_Subject)
		{
			// �� ����
			lpMMS_Content->lpSubject = AllocCopyString(pNotification_Ind->stSubject.pszString);
		}
		else
		{
			// �� ����
			lpMMS_Content->lpSubject = AllocCopyString("������");
		}

		// �õ���ǰ��ʱ��
		GetLocalTime(&stCurTime);
		VarMake_Date(&lpMMS_Content->dwDate,&stCurTime);
	
		// �õ������������� , ��֪ͨ���в��������ŵ���������
		lpMMS_Content->lpContent_Type = NULL;

		// ����֪ͨ�������ݣ� Ŀǰֻ�������ŵĵ�ַ
		lpMMS_Content->lpData = AllocCopyString(pNotification_Ind->pszX_Mms_Content_Location);
		lpMMS_Content->iDataSize = strlen(lpMMS_Content->lpData) + 1;  // ������β��

		return TRUE;
}


// ***************************************************************************************
// ������static BOOL GetMMSContentFromRetrieve(MCLT_RETRIEVE_CONF *pRetrieveConf,LPMMS_CONTENT lpMMS_Content)
// ������
//	IN pRetrieveConf -- �õ��Ĳ������ݰ����ݽṹ
//  OUT  lpMMS_Content -- Ҫ�����Ĳ������ݽṹ
// ����ֵ��
//		�ɹ�����TRUE�� ���򷵻�FALSE
// ����������
//  	�Ӳ������ݰ��еõ���������
// ***************************************************************************************
static BOOL GetMMSContentFromRetrieve(MCLT_RETRIEVE_CONF *pRetrieveConf,LPMMS_CONTENT lpMMS_Content)
{
			// �õ�������
		if (pRetrieveConf->dwMask & MCLT_MASK_From)
		{
			// �� FROM ��
			lpMMS_Content->lpFromTo = AllocCopyString(pRetrieveConf->stFrom.stEncodedStr.pszString);
		}
		else
		{
			// �� FROM �� �� ��֪�ӺͶ�������ʾ��...��
			lpMMS_Content->lpFromTo = AllocCopyString("...");
		}

		// �õ�����
		if (pRetrieveConf->dwMask & MCLT_MASK_Subject)
		{
			// �� ����
			lpMMS_Content->lpSubject = AllocCopyString(pRetrieveConf->stSubject.pszString);
		}
		else
		{
			// �� ����
			lpMMS_Content->lpSubject = AllocCopyString("������");
		}

		// �õ�����ʱ��
		
		if (pRetrieveConf->dwMask & MCLT_MASK_Date)
		{
			// �з���ʱ��
			lpMMS_Content->dwDate = pRetrieveConf->dwDate;
		}
		else
		{
			 // û�з���ʱ��
			SYSTEMTIME stCurTime;

				GetLocalTime(&stCurTime); // �õ���ǰʱ��
				VarMake_Date(&lpMMS_Content->dwDate,&stCurTime);
		}

		// �õ���ǰ���ŵ�����

		if (pRetrieveConf->dwMask & MCLT_MASK_CntType)
		{
			if (pRetrieveConf->stContent_Type.stType.fIsString == TRUE)
			{ // ���ִ�
				lpMMS_Content->lpContent_Type = AllocCopyString(pRetrieveConf->stContent_Type.stType.pszString);
			}
			else
			{
				// ��һ����֪�ı���
				CNTTYPE_ASSIGN stCntTypeAssign;
	
					stCntTypeAssign.dwNum = pRetrieveConf->stContent_Type.stType.dwVal;
					//  ������֪������
					if (VarFind_CntTypeAssign( CNTTYPEFIND_NAME_BYNUM, &stCntTypeAssign ) == TRUE)
					{
						// �Ѿ��鵽
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
		
		// �õ���������
		if (pRetrieveConf->dwMask & MCLT_MASK_Body)
		{
			// �в�������
			lpMMS_Content->iDataSize = pRetrieveConf->stBodyGen.dwLenBody; // �õ����ݳ���
			lpMMS_Content->lpData = malloc(lpMMS_Content->iDataSize); // ���������ݵĻ���
			if (lpMMS_Content->lpData == NULL)
			{
				// ���仺��ʧ��
				lpMMS_Content->iDataSize = 0;
			}
			else
			{
				memcpy(lpMMS_Content->lpData,pRetrieveConf->stBodyGen.pBody,lpMMS_Content->iDataSize); // ��������
			}
		}
		else
		{
			// û�в�������
			lpMMS_Content->lpData = NULL;
			lpMMS_Content->iDataSize = 0;
		}

		return TRUE;
}
// ***************************************************************************************
// ������static BOOL	FillMMSContentToSendReq(LPMMS_CONTENT lpMMS_Content,MCLT_SEND_REQ* pPduMCltReq)
// ������
//  IN  lpMMS_Content -- Ҫ���͵Ĳ������ݽṹ
//	OUT pPduMCltReq -- Ҫ�����ķ��Ͱ��ṹ
// ����ֵ��
//		�ɹ�����TRUE�� ���򷵻�FALSE
// ����������
//  	��䵱ǰ�Ĳ������ݵ����Ͱ��ṹ��
// ***************************************************************************************
static BOOL	FillMMSContentToSendReq(LPMMS_CONTENT lpMMS_Content,MCLT_SEND_REQ* pPduMCltReq)
{
	pPduMCltReq->dwMask = 0; 
	pPduMCltReq->bX_Mms_Message_Type = MType_send_req; // ��һ�����Ͳ��ŵ����ݰ�

	// from ������
	pPduMCltReq->stFrom.stEncodedStr.dwCharset = DEFAULTCHARSET; // ����ΪĬ���ַ���
	pPduMCltReq->stFrom.stEncodedStr.pszString = GetLocalNumber(); // �õ������绰����
	pPduMCltReq->dwMask |= MCLT_MASK_From;

	// to �ռ���
	pPduMCltReq->stTo.dwCharset = DEFAULTCHARSET; // ����ΪĬ���ַ���
	pPduMCltReq->stTo.pszString = AllocCopyString(lpMMS_Content->lpFromTo); //���öԷ��ĵ绰����
	pPduMCltReq->dwMask |= MCLT_MASK_To;

	// stSubject ����
	pPduMCltReq->stSubject.dwCharset = DEFAULTCHARSET; // ����ΪĬ���ַ���
	pPduMCltReq->stSubject.pszString = AllocCopyString(lpMMS_Content->lpSubject); //��������
	pPduMCltReq->dwMask |= MCLT_MASK_Subject;

	// stContent_Type ������������
	if (lpMMS_Content->lpContent_Type == NULL)
	{
		// û��ָ������
		pPduMCltReq->stContent_Type.stType.fIsString = FALSE;
		pPduMCltReq->stContent_Type.stType.dwVal = DEFAULTCONTENTTYPE;
	}
	else
	{
		// ��ָ������������
		CNTTYPE_ASSIGN stCntTypeAssign;

			if (strlen(lpMMS_Content->lpContent_Type) >= LEN_WSP_CNTTYPE_NAME)
			{
				// �϶�������֪�Ĳ�����������
				pPduMCltReq->stContent_Type.stType.fIsString = TRUE;
				pPduMCltReq->stContent_Type.stType.pszString = AllocCopyString(lpMMS_Content->lpContent_Type);
			}
			else
			{
				strcpy(stCntTypeAssign.szName,lpMMS_Content->lpContent_Type);
				//  ���ҵ�ǰ���������Ƿ�Ϊ��֪������
				if (VarFind_CntTypeAssign( CNTTYPEFIND_NUM_BYNAME, &stCntTypeAssign ) == TRUE)
				{
					// �Ѿ��鵽������֪�Ĳ�����������
					pPduMCltReq->stContent_Type.stType.fIsString = FALSE;
					pPduMCltReq->stContent_Type.stType.dwVal = stCntTypeAssign.dwNum;
				}
				else
				{
					// ������֪�Ĳ�����������
					pPduMCltReq->stContent_Type.stType.fIsString = TRUE;
					pPduMCltReq->stContent_Type.stType.pszString = AllocCopyString(lpMMS_Content->lpContent_Type);
				}
			}
	}
	pPduMCltReq->dwMask |= MCLT_MASK_CntType;


	// ��������

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
// ������static LPTSTR GetLocalNumber(void)
// ������
//		��
// ����ֵ��
//		���ر����ĵ绰����
// ����������
//  	�õ������ĵ绰���롣
// ***************************************************************************************
static LPTSTR GetLocalNumber(void)
{
	return AllocCopyString("13510994325");
}