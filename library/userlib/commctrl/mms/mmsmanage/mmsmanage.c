/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����MMS����ϵͳ,����Ҫ�����ڶ�MMS�Ĺ���,�������������͵�MMS����֯,
		  ����,ɾ��,��ȡ,���... �Ȳ���
		  ��ǰ�汾���ļ���ȡʹ��ֱ�Ӵ����ļ��ķ����������ǽ���һ�������ķ���
		  ���б����ļ��ķ���Ϊ���浽һ���ļ��У����߱��浽����ļ�����������
		  ���浽һ���ļ�ʹ�ö���SINGLE_FILE.
		  ���浽����ļ�ʹ��MULTI_FILE
�汾�ţ�1.0.0
����ʱ�ڣ�2004-06-03
���ߣ��½��� Jami chen
�޸ļ�¼��

******************************************************/
#include <EWindows.h>
#include <MMSManage.h>
#include "..\mmsbody\mime_body.h"
#include "..\include\mms_content.h"
#include "..\mmstrans\mmstrans.h"
#include <mms_clt.h>

#define MULTI_FILE
//#define SINGLE_FILE


/***************  ȫ���� ���壬 ���� *****************/
#ifdef MULTI_FILE

#define FIELD_NUM		6
#define FIELD_STATUS		0
#define FIELD_FROMTO		1
#define FIELD_SUBJECT		2
#define FIELD_DATE			3
#define FIELD_CONTENTTYPE	4
#define FIELD_MIME			5
// MMS ���ݿⶨ��
// FIELD 0 �� DWORD  -- ���嵱ǰ���ݵ�״̬
// FIELD 1 �� ������ �� �ռ���-- �ö�����˭���ͻ�Ҫ���͸�˭��һ�㱣��绰���룬����Ѿ���������ϵ���������û�ȥ����
// FIELD 2 �� ���� -- ָ���ö��ŵı���
// FIELD 3 �� ʱ�� -- MMS�ķ���ʱ�䣬����Ϊһ��DWORDֵ����1970.1.1 00:00:00 ��ʼ����
// FIELD 4 �� �������� -- Content-type ,��ǰMMS����Ϣ����
// FIELD 5 �� ���ݲ��� -- �����MMS�Ѿ���ȡ������MMS��BODY�������Ÿ�MMS��֪ͨ��Ϣ

#endif

#ifdef SINGLE_FILE
// MMS ���ݿⶨ��
// FIELD 0 �� 1 BYTE ����ò��ŵ����� 'R' -- �ռ������� , 
//									  'T' -- ���������� , 
//									  'H' -- �ѷ��͵�����, 
//									  'S' -- ��������� ��
// FIELD 1 �� DWORD  -- ���嵱ǰ���ݵ�״̬
// FIELD 2 �� ������ �� �ռ���-- �ö�����˭���ͻ�Ҫ���͸�˭��һ�㱣��绰���룬����Ѿ���������ϵ���������û�ȥ����
// FIELD 3 �� ���� -- ָ���ö��ŵı���
// FIELD 4 �� ʱ�� -- MMS�ķ���ʱ�䣬����Ϊһ��DWORDֵ����1970.1.1 00:00:00 ��ʼ����
// FIELD 5 �� �������� -- Content-type ,��ǰMMS����Ϣ����
// FIELD 6 �� ���ݲ��� -- �����MMS�Ѿ���ȡ������MMS��BODY�������Ÿ�MMS��֪ͨ��Ϣ


#define FIELD_NUM			7		// Ŀǰÿһ����¼�ж����ֶ�
#define FIELD_TYPE			0
#define FIELD_STATUS		1
#define FIELD_FROMTO		2
#define FIELD_SUBJECT		3
#define FIELD_DATE			4
#define FIELD_CONTENTTYPE	5
#define FIELD_MIME			6

#endif

#define MMS_MAXSIZE  (50 * 1024)  // Ŀǰ�й��ƶ�����������Ŵ�СΪ50K

// MMS����ṹ
typedef struct structMMSManage{
	UINT  uMMSType; //��ǰMMS�Ĺ�������
	HTABLE hTable;
#ifdef SINGLE_FILE
	TCHAR	chType;
#endif
}MMSMANAGE, *LPMMSMANAGE;

#ifdef MULTI_FILE
#define MMS_RECEIVE_FILE	"\\Mobile\\MMS\\Receive.dat"  // �ռ���
#define MMS_SEND_FILE		"\\Mobile\\MMS\\Send.dat"     // ������
#define MMS_SENT_FILE		"\\Mobile\\MMS\\Sent.dat"     // �ѷ���Ϣ
#define MMS_SAVED_FILE		"\\Mobile\\MMS\\Saved.dat"    // ������Ϣ

#define RECORDNUM_RECEIVE	20		// �ռ���ɴ��������¼��Ŀ��
#define RECORDNUM_SEND		 5		// ������ɴ��������¼��Ŀ��
#define RECORDNUM_SENT		10		// �ѷ��Ϳɴ��������¼��Ŀ��
#define RECORDNUM_SAVED		20		// ������ɴ��������¼��Ŀ��
#endif

#ifdef SINGLE_FILE
#define MMS_DATA_FILE	"\\Mobile\\MMS\\MMS.dat"  //  �������ݿ�
#define RECORDNUM			30		// ���Ա��������¼��Ŀ��
#endif


#define TYPENUM				4       // Ŀǰ�����ֲ�ͬ���͵�����


#define DATA_VALID		0x80000000  // ������Ч


// ***************************************************************************************
// ��������
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
// mms ���Ӵ���
// **************************************************
static TCHAR classMMSMonitorWindow[] = "MMSMONITORWINDOW";

static HWND g_hMonitorWnd = NULL;

#define WM_NEW_MMS_PUSH		(WM_USER + 629)

static ATOM RegisterMMSMonitorWindow(HINSTANCE hInstance);
static LRESULT CALLBACK MonitorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT NewMMSArrive(HWND hWnd,WPARAM wParam,LPARAM lParam);


// ***************************************************************************************
// ������HMMS OpenMMS(UINT uMMSType)
// ������
//	IN uMMSType -- ָ����MMS����

// ����ֵ��
//	����һ��MMS���
// ����������
//  	��һ��MMS�����Ժ����ʹ�øþ����MMS���з���
// ***************************************************************************************
HMMS OpenMMS(UINT uMMSType)
{
	LPMMSMANAGE lpMMSManage;
	LPTSTR lpDataBaseName;
	static int iCount = 0;

		lpMMSManage = (LPMMSMANAGE)malloc(sizeof(MMSMANAGE)); // ����һ��MMS����ṹ�Ļ���

		if (lpMMSManage == NULL)
		{
			// ����ʧ��
			SetLastError(MMS_ERROR_NOMEMORY); // ϵͳ�ڴ治��
			return NULL;
		}

#ifdef MULTI_FILE
		switch(uMMSType)
		{
			case MMS_RECEIVE:	//	�ռ���
				lpDataBaseName = MMS_RECEIVE_FILE; //�õ����յ�MMS�����ݿ��ļ���
				break;
			case MMS_SEND:		//  ������
				lpDataBaseName = MMS_SEND_FILE; //�õ�δ����MMS�����ݿ��ļ���
				break;
			case MMS_SENT:		//	�ѷ���Ϣ
				lpDataBaseName = MMS_SENT_FILE; //�õ��ѷ���MMS�����ݿ��ļ���
				break;
			case MMS_SAVED:		//  �浵�ļ���
				lpDataBaseName = MMS_SAVED_FILE; //�õ��浵MMS�����ݿ��ļ���
				break;
			default: // ����ʶ��MMS����
				free(lpMMSManage);
				SetLastError(MMS_ERROR_UNKNOWMMSTYPE); // ����ʶ��MMS����
				return NULL;
		}
#endif
#ifdef SINGLE_FILE
		switch(uMMSType)
		{
			case MMS_RECEIVE:	//	�ռ���
				lpMMSManage->chType = 'R'; //�õ����յ�MMS�����ݿ��ļ���
				break;
			case MMS_SEND:		//  ������
				lpMMSManage->chType = 'T'; //�õ�δ����MMS�����ݿ��ļ���
				break;
			case MMS_SENT:		//	�ѷ���Ϣ
				lpMMSManage->chType = 'H'; //�õ��ѷ���MMS�����ݿ��ļ���
				break;
			case MMS_SAVED:		//  �浵�ļ���
				lpMMSManage->chType = 'S'; //�õ��浵MMS�����ݿ��ļ���
				break;
			default: // ����ʶ��MMS����
				free(lpMMSManage);
				SetLastError(MMS_ERROR_UNKNOWMMSTYPE); // ����ʶ��MMS����
				return NULL;
		}
	
		lpDataBaseName = MMS_DATA_FILE; //�õ��浵MMS�����ݿ��ļ���
#endif
		
		// �ڴ����ɹ�
		iCount ++;
		lpMMSManage->uMMSType = uMMSType; // ���浱ǰ������
		lpMMSManage->hTable = CreateTable(lpDataBaseName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
		if ( lpMMSManage->hTable == INVALID_HANDLE_VALUE )
		{  // �����ݿ�ʧ��
			DWORD dwError = GetLastError();
			RETAILMSG( 1, ( "OpenTable(%s):%d Times Failure\r\n",lpDataBaseName,iCount) );
//			Sleep(1000);
			free(lpMMSManage); // �ͷ��ڴ�
			SetLastError(MMS_ERROR_INVALIDTABLE);  // ��������ݿ�
			return NULL;
		}


		return (HMMS)lpMMSManage; //�ɹ�,���ؾ��
}


// ***************************************************************************************
// ������void CloseMMS(HMMS hMMS)
// ������
//    hMMS -- Ҫ�رյ�MMS���
// ����ֵ��
//	��
// ����������
//  	�ر�һ��MMS�����Ժ�Ͳ��ܶ�MMS���з���
// ***************************************************************************************
void CloseMMS(HMMS hMMS)
{
	LPMMSMANAGE lpMMSManage;

		if (hMMS == NULL)
		{  // �����MMS���
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return;
		}

		lpMMSManage = (LPMMSMANAGE)hMMS; // �õ�MMS����ṹ��ָ��

		if ( lpMMSManage->hTable != INVALID_HANDLE_VALUE )
		{
			CloseTable(lpMMSManage->hTable);
		}

		free(lpMMSManage); // �ͷ��ڴ�
}


// ***************************************************************************************
// ������UINT GetMMSCount(HMMS hMMS);
// ������
//    hMMS -- �Ѿ��򿪵�MMS���
// ����ֵ��
//		���ص�ǰϵͳ�е�MMS����Ŀ(�����Ѿ����صĺ�û�����ص�)
// ����������
//  	�õ���ǰϵͳ��MMS����Ŀ
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
		{  // �����MMS���
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return 0;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // �õ�MMS����ṹ��ָ��
#ifdef MULTI_FILE
		uCount = CountRecord(lpMMSManage->hTable);
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //���ҵ�һ����ǰ���͵�����
		if( hFind != INVALID_HANDLE_VALUE )
		{
			do{
				//do somthing with record
				uCount ++;
			}while( FindNextRecord( hFind, &fs ) );  // ������һ����ǰ���͵�����
			CloseTableFind( hFind ); // close find handle
		}
#endif

		return uCount; // ���ص�ǰ�ļ�¼��Ŀ
}

// ***************************************************************************************
// ������BOOL GetMMSInfo(HMMS hMMS, UINT uIndex,LPMMSINFO lpMMSInfo);
// ������
//   IN  hMMS -- �Ѿ��򿪵�MMS���
//   IN  uIndex -- Ҫ�õ�MMS��Ϣ����Ŀ����
//   OUT lpMMSInfo -- ����ָ����Ŀ����Ϣ�����������ˣ����⣬ʱ��
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	�õ�ָ��������MMS����Ϣ
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
		{  // �����MMS���
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return FALSE;
		}

		if (lpMMSInfo->uSize != sizeof(MMSINFO))
		{
			SetLastError(MMS_ERROR_INVALIDPARAMETER);  // �������
			return FALSE;
		}

		lpMMSManage = (LPMMSMANAGE)hMMS; // �õ�MMS����ṹ��ָ��
#ifdef MULTI_FILE
		uRecordIndex  = uIndex;
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //���ҵ�һ����ǰ���͵�����
		if( hFind != INVALID_HANDLE_VALUE )
		{
			while( 1 )
			{
				//do somthing with record
				if ((uIndex + 1) == fs.nRecord)
				{
					uRecordIndex  = fs.nRecord -1; // �Ѿ��ҵ���ǰ�ļ�¼
					break;
				}
				if (FindNextRecord( hFind, &fs ) == 0) // ������һ����ǰ���͵�����
				{
					// �Ѿ�û��������
					SetLastError(MMS_ERROR_INVALIDINDEX);
					return FALSE;
				}
			}  
			CloseTableFind( hFind ); // close find handle
		}
#endif


		if (SetRecordPointer(lpMMSManage->hTable, uRecordIndex, SRP_BEGIN) == 0xffff) // ���õ��û�ָ��������
		{
			SetLastError(MMS_ERROR_INVALIDINDEX);
			return FALSE;
		}
		
		if (lpMMSInfo->uMask & INFO_FROMTO)
		{ 
			// ��ҪFROMTO
			ReadDataField(lpMMSManage->hTable,FIELD_FROMTO,(void *)lpMMSInfo->lpFromTo,(WORD)lpMMSInfo->uMaxFromTo);  //��ȡ������/�ռ�����Ϣ
		}

		if (lpMMSInfo->uMask & INFO_SUBJECT)
		{
			ReadDataField(lpMMSManage->hTable,FIELD_SUBJECT,(void *)lpMMSInfo->lpSubject,(WORD)lpMMSInfo->uMaxSubject);  //��ȡ������Ϣ
		}

		if (lpMMSInfo->uMask & INFO_DATE)
		{
			ReadDataField(lpMMSManage->hTable,FIELD_DATE,(void *)&dwSecs,sizeof(DWORD));  //��ȡʱ����Ϣ
			TransDate(&lpMMSInfo->MMSTime,dwSecs); // ת��Ϊ��ǰ��ʱ��
		}

		if (lpMMSInfo->uMask & INFO_STATUS)
		{
			ReadDataField(lpMMSManage->hTable,FIELD_STATUS,(void *)&lpMMSInfo->uStatus,(WORD)sizeof(DWORD));  //��ȡ��Ϣ״̬
		}

		return TRUE;
}

// ***************************************************************************************
// ������BOOL DeleteMMS(HMMS hMMS, UINT uIndex);
// ������
//   IN  hMMS -- �Ѿ��򿪵�MMS���
//   IN  uIndex -- Ҫɾ����MMS����Ŀ����
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	ɾ��ָ��������MMS
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
		{  // �����MMS���
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return FALSE;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // �õ�MMS����ṹ��ָ��

#ifdef MULTI_FILE
		uRecordIndex  = uIndex;
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //���ҵ�һ����ǰ���͵�����
		if( hFind != INVALID_HANDLE_VALUE )
		{
			while( 1 )
			{
				//do somthing with record
				if ((uIndex + 1) == fs.nRecord)
				{
					uRecordIndex  = fs.nRecord -1; // �Ѿ��ҵ���ǰ�ļ�¼
					break;
				}
				if (FindNextRecord( hFind, &fs ) == 0) // ������һ����ǰ���͵�����
				{
					// �Ѿ�û��������
					return FALSE;
				}
			}  
			CloseTableFind( hFind ); // close find handle
		}
#endif

		SetRecordPointer(lpMMSManage->hTable, uRecordIndex, SRP_BEGIN); // ���õ��û�ָ��������

		DeleteRecord(lpMMSManage->hTable); //ɾ����ǰ�ļ�¼


		return TRUE;
}
// ***************************************************************************************
// ������BOOL ClearMMS(HMMS hMMS);
// ������
//   IN  hMMS -- �Ѿ��򿪵�MMS���
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	ɾ�����е�MMS
// ***************************************************************************************
BOOL ClearMMS(HMMS hMMS)
{
	LPMMSMANAGE lpMMSManage;
	UINT nCount;
		if (hMMS == NULL)
		{  // �����MMS���
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return FALSE;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // �õ�MMS����ṹ��ָ��

		nCount = GetMMSCount(hMMS);
		while(nCount)
		{
			SetRecordPointer(lpMMSManage->hTable, nCount - 1, SRP_BEGIN); // ���õ����һ����¼������
			DeleteRecord(lpMMSManage->hTable); // ɾ�����һ����¼
			nCount --; // ɾ��ǰһ����¼
		}
		return TRUE;
}
// ***************************************************************************************
// ������HMMSDATA LoadMMSData(HMMS hMMS, UINT uIndex);
// ������
//   IN  hMMS -- �Ѿ��򿪵�MMS���
//   IN  uIndex -- Ҫװ�ص�MMS����Ŀ����
// ����ֵ��
//		�ɹ�����MMS���ݾ�������򷵻�NULL
// ����������
//  	װ��ָ��������MMS����
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
		{  // �����MMS���
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return NULL;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // �õ�MMS����ṹ��ָ��

		// ���ȵõ���ǰ��״̬,�����һ��δ����MMS,����ȥ�õ���ǰ��MMS,Ȼ����װ�ص�ǰ��MMS
#ifdef MULTI_FILE
		uRecordIndex  = uIndex;
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //���ҵ�һ����ǰ���͵�����
		if( hFind != INVALID_HANDLE_VALUE )
		{
			while( 1 )
			{
				//do somthing with record
				if ((uIndex + 1) == fs.nRecord)
				{
					uRecordIndex  = fs.nRecord -1; // �Ѿ��ҵ���ǰ�ļ�¼
					break;
				}
				if (FindNextRecord( hFind, &fs ) == 0) // ������һ����ǰ���͵�����
				{
					// �Ѿ�û��������
					return FALSE;
				}
			}  
			CloseTableFind( hFind ); // close find handle
		}
#endif

		SetRecordPointer(lpMMSManage->hTable, uRecordIndex, SRP_BEGIN); // ���õ��û�ָ��������

		lpMMS_Content = (LPMMS_CONTENT)malloc(sizeof(MMS_CONTENT)); // ����һ��MMS���ݵĽṹ����
		if (lpMMS_Content == NULL)
		{ // ����ʧ��
			SetLastError(MMS_ERROR_NOMEMORY); // ϵͳ�ڴ治��
			goto ERROR_RETURN;
		}

		// ��ʼ���ṹ
		memset(lpMMS_Content,0,sizeof(MMS_CONTENT));  // ��ջ���

		ReadDataField(lpMMSManage->hTable,FIELD_STATUS,(void *)&uStatus,(WORD)sizeof(DWORD));  //��ȡ��Ϣ״̬

		if ((uStatus & 0xffff) == MMS_NOTREAD)
		{  // ��ǰ���ݻ�û�ж�ȡ
			if (lpMMSManage->uMMSType == MMS_RECEIVE)
			{ // ��ǰ���ռ��䣬��MMS��û�ж�ȡ,�ȶ�ȡMMS		
				LPTSTR lpData;

					// ���ȶ�ȡ֪ͨ��Ϣ������
					uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_MIME);
					lpData = (LPVOID)malloc(uFieldSize);

					if (lpData == NULL)
					{ // ����ʧ��
						SetLastError(MMS_ERROR_NOMEMORY); // ϵͳ�ڴ治��
						goto ERROR_RETURN;
					}
					ReadDataField(lpMMSManage->hTable,FIELD_MIME,(void *)lpData,(WORD)uFieldSize);  //��ȡMMS֪ͨ��������

					if (GetMMSFromServer(lpData, uFieldSize,lpMMS_Content) == FALSE)  // ��ȡ��������
					{
						// �õ���������ʧ��
						SetLastError(MMS_ERROR_READMMSFAILURE); // ϵͳ�ڴ治��
						goto ERROR_RETURN;
					}
					// ���浱ǰ�õ��Ĳ��ŵ��ļ�
					WriteMessageToTable(lpMMSManage->hTable,lpMMS_Content,MMS_RECEIVE);
					
					return (HMMSDATA)lpMMS_Content; // ���صõ��Ĳ���
			}
			else
			{
				// �������͵Ĳ���
				uStatus &= 0xffff0000;
			    uStatus |=MMS_READ; // ���ò����Ѷ�
			    WriteDataField(lpMMSManage->hTable,FIELD_STATUS,(void *)&uStatus,(WORD)sizeof(DWORD));  //��ȡ��Ϣ״̬
			}
		}


		// ��ҪFROMTO
		uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_FROMTO);
		lpMMS_Content->lpFromTo = (LPTSTR)malloc(uFieldSize);
		if (lpMMS_Content->lpFromTo == NULL)
		{ // ����ʧ��
			SetLastError(MMS_ERROR_NOMEMORY); // ϵͳ�ڴ治��
			goto ERROR_RETURN;
		}
		ReadDataField(lpMMSManage->hTable,FIELD_FROMTO,(void *)lpMMS_Content->lpFromTo,(WORD)uFieldSize);  //��ȡ������/�ռ�����Ϣ

		uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_SUBJECT);
		lpMMS_Content->lpSubject = (LPTSTR)malloc(uFieldSize);
		if (lpMMS_Content->lpSubject == NULL)
		{ // ����ʧ��
			SetLastError(MMS_ERROR_NOMEMORY); // ϵͳ�ڴ治��
			goto ERROR_RETURN;
		}
		ReadDataField(lpMMSManage->hTable,FIELD_SUBJECT,(void *)lpMMS_Content->lpSubject,(WORD)uFieldSize);  //��ȡ������Ϣ

		ReadDataField(lpMMSManage->hTable,FIELD_DATE,(void *)&lpMMS_Content->dwDate,sizeof(DWORD));  //��ȡʱ����Ϣ

		//  ��ȡ������������
		uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_CONTENTTYPE);
		if (uFieldSize != 0)
		{
			lpMMS_Content->lpContent_Type = (LPTSTR)malloc(uFieldSize);
			if (lpMMS_Content->lpContent_Type == NULL)
			{ // ����ʧ��
				SetLastError(MMS_ERROR_NOMEMORY); // ϵͳ�ڴ治��
				goto ERROR_RETURN;
			}
			ReadDataField(lpMMSManage->hTable,FIELD_CONTENTTYPE,(void *)lpMMS_Content->lpContent_Type,(WORD)uFieldSize);  //��ȡ��������
		}
		else
		{
			lpMMS_Content->lpContent_Type = NULL;
		}

		//  ��ȡ��������
		uFieldSize = GetFieldSize(lpMMSManage->hTable,FIELD_MIME);
		lpMMS_Content->iDataSize = uFieldSize;
		lpMMS_Content->lpData = (LPVOID)malloc(lpMMS_Content->iDataSize);
		if (lpMMS_Content->lpData == NULL)
		{ // ����ʧ��
			SetLastError(MMS_ERROR_NOMEMORY); // ϵͳ�ڴ治��
			goto ERROR_RETURN;
		}
		ReadDataField(lpMMSManage->hTable,FIELD_MIME,(void *)lpMMS_Content->lpData,(WORD)lpMMS_Content->iDataSize);  //��ȡMMS����

		return (HMMSDATA)lpMMS_Content;

ERROR_RETURN:

		if (lpMMS_Content)
		{
			if (lpMMS_Content->lpContent_Type)
				free(lpMMS_Content->lpContent_Type);  // �ͷ��������ͻ���

			if (lpMMS_Content->lpData)
				free(lpMMS_Content->lpData);  // �ͷ����ݻ���

			if (lpMMS_Content->lpFromTo)
				free(lpMMS_Content->lpFromTo);   // �ͷŷ����˻���

			if (lpMMS_Content->lpSubject)
				free(lpMMS_Content->lpSubject);   // �ͷű��⻺��

			free(lpMMS_Content);  // �ͷ����ݽṹ����
		}
		return NULL;
}

// ***************************************************************************************
// ������BOOL ReleaseMMSData(HMMS hMMS, HMMSDATA hMMSData);
// ������
//   IN  hMMS -- �Ѿ��򿪵�MMS���
//   IN  hMMSData -- Ҫ�ͷŵ�MMS���ݾ��
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	�ͷŵ�MMS����
// ***************************************************************************************
BOOL ReleaseMMSData(HMMS hMMS, HMMSDATA hMMSData)
{
	LPMMSMANAGE lpMMSManage;
	LPMMS_CONTENT lpMMS_Content;

		if (hMMS == NULL || hMMSData == NULL)
		{  // �����MMS���
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return FALSE;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // �õ�MMS����ṹ��ָ��
		lpMMS_Content = (LPMMS_CONTENT)hMMSData; // �õ�MMS���ݽṹָ��

		if (lpMMS_Content)
		{
			if (lpMMS_Content->lpContent_Type)
				free(lpMMS_Content->lpContent_Type);  // �ͷ��������ͻ���

			if (lpMMS_Content->lpData)
				free(lpMMS_Content->lpData);  // �ͷ����ݻ���

			if (lpMMS_Content->lpFromTo)
				free(lpMMS_Content->lpFromTo);   // �ͷŷ����˻���

			if (lpMMS_Content->lpSubject)
				free(lpMMS_Content->lpSubject);   // �ͷű��⻺��

			free(lpMMS_Content);  // �ͷ����ݽṹ����
		}

		return TRUE;
}

// ***************************************************************************************
// ������static void TransDate(LPSYSTEMTIME lpMMSTime,DWORD dwDate)
// ������
//	OUT  lpMMSTime -- �õ���ǰ��ʱ��
//  IN  dwDate -- ��1970.01.01 00:00:00 ��ʼ������
// ����ֵ��
//		��
// ����������
//		ת��Ϊ��ǰ��ʱ��
// ***************************************************************************************
static void TransDate(LPSYSTEMTIME lpMMSTime,DWORD dwSecs)
{
	UINT iDays; 
		// ��ʼ����ʼʱ�� 1970 - 01 - 01 , 00:00:00
		lpMMSTime->wYear = 1970;
		lpMMSTime->wMonth = 1;
		lpMMSTime->wDay = 1;
		lpMMSTime->wHour = 0;
		lpMMSTime->wMinute = 0;
		lpMMSTime->wSecond = 0;
		lpMMSTime->wMilliseconds = 0;



		iDays = dwSecs / (3600 * 24) ; // �õ���1970-01-01 ��ʼ�����������
		dwSecs %= (3600 * 24) ; // �õ���ȥ���������������������ʱ���֣���

		DateAddDay(lpMMSTime,iDays); // �õ���ǰ������

		// �õ���ǰ��ʱ��
		lpMMSTime->wHour = (WORD)dwSecs / 3600; // �õ�Сʱ��
		dwSecs %= 3600;  // �õ���ȥСʱ�������

		lpMMSTime->wMinute = (WORD)dwSecs / 60; // �õ�����
		dwSecs %= 60;  // �õ���ȥ�����������

		lpMMSTime->wSecond = (WORD)dwSecs; // �õ�����

}

// ***************************************************************************************
// ������BOOL InitialMMSSystem(HINSTANCE hInstance)
// ������
// IN hInstance -- ��ǰ���г�ʼ�����ŵ�ʵ�����
//
// ����ֵ��
//		��
// ����������
//		��ʼ��MMSϵͳ
// ***************************************************************************************
BOOL InitialMMSSystem(void)
{
	HINSTANCE hInstance;
//	INITLOCK(); // ��ʼ���ٽ����
	
	if (g_hMonitorWnd)
		return TRUE; // �Ѿ���ʼ�����

	RETAILMSG( 1, ( "Initialize folder\r\n") );
	// ��ʼ�����ݿ�
	if (File_IsExist( "\\Mobile" ) == FALSE )
	{
		// Mobile Ŀ¼�����ڣ�����Mobile
		CreateDirectory("\\Mobile",NULL);  // ����Ŀ¼
	}

	if (File_IsExist( "\\Mobile\\MMS" ) == FALSE )
	{
		// \Mobile\mms Ŀ¼�����ڣ�����Mobile\mms
		CreateDirectory("\\Mobile\\MMS",NULL);  // ����Ŀ¼
	}

	if (File_IsExist( "\\Mobile\\temp" ) == FALSE )
	{
		// \Mobile\temp Ŀ¼�����ڣ�����Mobile\temp
		CreateDirectory("\\Mobile\\temp",NULL);  // ����Ŀ¼
	}

	RETAILMSG( 1, ( "Initialize folder OK !!!\r\n") );
#ifdef MULTI_FILE
	if (File_IsExist( MMS_RECEIVE_FILE ) == FALSE )
	{
		// �ռ��䲻����
		CreateDefaultFileBox(MMS_RECEIVE_FILE,RECORDNUM_RECEIVE); // ����Ĭ���ռ���
	}

	if (File_IsExist( MMS_SEND_FILE ) == FALSE )
	{
		// �����䲻����
		CreateDefaultFileBox(MMS_SEND_FILE,RECORDNUM_SEND); // ����Ĭ���ռ���
	}

	if (File_IsExist( MMS_SENT_FILE ) == FALSE )
	{
		// �����䲻����
		CreateDefaultFileBox(MMS_SENT_FILE,RECORDNUM_SENT); // ����Ĭ���ռ���
	}

	if (File_IsExist( MMS_SAVED_FILE ) == FALSE )
	{
		// �����䲻����
		CreateDefaultFileBox(MMS_SAVED_FILE,RECORDNUM_SAVED); // ����Ĭ���ռ���
	}
#endif

#ifdef SINGLE_FILE
	if (File_IsExist( MMS_DATA_FILE ) == FALSE )
	{
		// ���ݿⲻ����
		RETAILMSG( 1, ( "Need CreateDefaultFileBox ...\r\n") );
		CreateDefaultFileBox(MMS_DATA_FILE,RECORDNUM); // ����Ĭ���ռ���
		RETAILMSG( 1, ( "CreateDefaultFileBox OK !!!\r\n") );
	}
#endif

//	CreateThread( NULL, 0, SendMMSThread, NULL, 0, NULL );

	hInstance = GetModuleHandle(NULL);

	RegisterMMSMonitorWindow(hInstance);
	// ����һ�����Ž��ռ�ⴰ��
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

	MCltSetPushCallback( g_hMonitorWnd, WM_NEW_MMS_PUSH ); // ���ò���֪ͨ�ص����ں���Ϣ

	return TRUE;
}

// ***************************************************************************************
// ������BOOL DeinitialMMSSystem(void)
// ������
//
// ����ֵ��
//		��
// ����������
//		Deinitial MMSϵͳ
// ***************************************************************************************
BOOL DeinitialMMSSystem(void)
{
	return TRUE;
}

// ***************************************************************************************
// ������void GetNewMessage(LPMMS_CONTENT lpMMS_Content)
// ������
//	IN lpMMS_Content -- �õ����µĲ���
//
// ����ֵ��
//		��
// ����������
//		�õ�һ���µĲ���
// ***************************************************************************************
void GetNewMessage(LPMMS_CONTENT lpMMS_Content)
{
	// �õ�һ���µĶ���Ϣ

	SaveMessageToTable(lpMMS_Content,MMS_RECEIVE);
}

// ***************************************************************************************
// ������static void SaveMessageToTable(LPMMS_CONTENT lpMMS_Content,UINT uMessageType)
// ������
//	IN lpMMS_Content -- Ҫ����Ĳ���
//	IN bNewMessage -- �Ƿ����µĲ���
// ����ֵ��
//		��
// ����������
//		����һ���Ĳ���
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
		// ���ݹ滮		
		//  Field 0 -- |  Flag  | Field 0 Len | Field 1 Len | ... ... | Field n Len | 
		//  Field 1 -- |Field 0 Data | Field 1 Data | ... | Field n Data |

		//  ���� Flag Ϊ��ǰ��Ŀ�ı�־, DWORD (4 BYTE)
		//  flag --  bit 31  -- ��ǰ�����Ƿ���Ч
		//			 bit 15 -- bit 0 -- ��ǰ��¼��״̬
		//  Field 0 Len ... Field n Len Ϊ�����ֶεĳ��� ÿ���ֶ�Ϊһ��WORD (2 BYTE)
		//  Field 0 Data  ...  Field n Data  Ϊ�����ֶε�����

#ifdef SINGLE_FILE
		// ��������
		switch(uMessageType)
		{
			case MMS_RECEIVE:	//	�ռ���
				fieldItem[FIELD_TYPE].lpData = "R";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SEND:		//  ������
				fieldItem[FIELD_TYPE].lpData = "T";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SENT:		//	�ѷ���Ϣ
				fieldItem[FIELD_TYPE].lpData = "H";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SAVED:		//  �浵�ļ���
				fieldItem[FIELD_TYPE].lpData = "S";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			default: // ����ʶ��MMS����
				SetLastError(MMS_ERROR_UNKNOWMMSTYPE); // ����ʶ��MMS����
				return;
		}
#endif

		// ����״̬
		uStatus |= MMS_NOTREAD ; //����MMSδ��
		fieldItem[FIELD_STATUS].lpData = &uStatus;
		fieldItem[FIELD_STATUS].size = sizeof(DWORD);


		// д���ֶ� 0 -- ������/�ռ���
		fieldItem[FIELD_FROMTO].lpData = lpMMS_Content->lpFromTo;
		fieldItem[FIELD_FROMTO].size = strlen(lpMMS_Content->lpFromTo) + 1; // ������β��

		// д���ֶ� 1 --  ����
		fieldItem[FIELD_SUBJECT].lpData = lpMMS_Content->lpSubject;
		fieldItem[FIELD_SUBJECT].size = strlen(lpMMS_Content->lpSubject) + 1; // ������β��

		// д���ֶ� 2 --  ʱ��
		fieldItem[FIELD_DATE].lpData = &lpMMS_Content->dwDate;
		fieldItem[FIELD_DATE].size = sizeof(DWORD); 

		// д���ֶ� 3 --  ��������
		fieldItem[FIELD_CONTENTTYPE].lpData = lpMMS_Content->lpContent_Type;
		if (lpMMS_Content->lpContent_Type == NULL)
		{
			fieldItem[FIELD_CONTENTTYPE].size = 0; // ������β��
		}
		else
		{
			fieldItem[FIELD_CONTENTTYPE].size = strlen(lpMMS_Content->lpContent_Type) + 1; // ������β��
		}

		// д���ֶ� 4 --  ����

		fieldItem[FIELD_MIME].lpData = lpMMS_Content->lpData;
		fieldItem[FIELD_MIME].size = lpMMS_Content->iDataSize; // ������β��

#ifdef MULTI_FILE
		switch(uMessageType)
		{
			case MMS_RECEIVE:	//	�ռ���
				hTable = CreateTable(MMS_RECEIVE_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				break;
			case MMS_SEND:		//  ������
				hTable = CreateTable(MMS_SEND_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				break;
			case MMS_SENT:		//	�ѷ���Ϣ
				hTable = CreateTable(MMS_SENT_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				break;
			case MMS_SAVED:		//  �浵�ļ���
				hTable = CreateTable(MMS_SAVED_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				break;
		}
#endif

#ifdef SINGLE_FILE
		hTable = CreateTable(MMS_DATA_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
#endif
		if ( hTable == INVALID_HANDLE_VALUE )
		{  // �����ݿ�ʧ��
			SetLastError(MMS_ERROR_INVALIDTABLE);
			return -1;
		}
		AppendRecord( hTable, (FIELDITEM *)&fieldItem);
		
		iIndex = CountRecord(hTable);
		CloseTable(hTable);

		return iIndex;
}

// ***************************************************************************************
// ������static BOOL	File_IsExist( LPCTSTR lpFileName )
// ������
//	IN lpFileName -- Ҫ���Ե��ļ���
// ����ֵ��
//		����ļ����ڷ���TRUE�����򷵻�FALSE
// ����������
//		����һ���ļ����ļ����Ƿ����
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
// ������static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
// ������
//	IN hTable -- ��ǰ�Ѿ��򿪵ĵ����ݿ���
//	IN uField -- ָ���ֶ�
//  OUT lpData -- ���ݻ���
//	IN wDataSize --	���ݻ����С
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//		��ȡ���ݿ⵱ǰ��¼��ָ��FIELD����
// ***************************************************************************************
static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
{
	WORD wReadSize;
		wReadSize = ReadField(hTable,uField + 1,0,lpData,wDataSize);  //��ȡ����
		return TRUE;
}
// ***************************************************************************************
// ������static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
// ������
//	IN hTable -- ��ǰ�Ѿ��򿪵ĵ����ݿ���
//	IN uField -- ָ���ֶ�
//  OUT lpData -- ���ݻ���
//	IN wDataSize --	���ݻ����С
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//		��ȡ���ݿ⵱ǰ��¼��ָ��FIELD����
// ***************************************************************************************
static BOOL WriteDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
{
		WriteField(hTable,uField + 1,lpData,wDataSize);  //д������
		return TRUE;
}


// ***************************************************************************************
// ������static BOOL ReadDataField(HTABLE hTable,UINT uField,LPVOID lpData,WORD wDataSize)
// ������
//	IN hTable -- ��ǰ�Ѿ��򿪵ĵ����ݿ���
//	IN uField -- ָ���ֶ�
//  OUT lpData -- ���ݻ���
//	IN wDataSize --	���ݻ����С
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//		��ȡ���ݿ⵱ǰ��¼��ָ��FIELD����
// ***************************************************************************************
static UINT GetFieldSize(HTABLE hTable,UINT uField)
{
	TCHAR lpTemp[sizeof(RECORDINFO) + FIELD_NUM * sizeof(DWORD)];
	RECORDINFO*		lprdInfo = NULL;
	UINT uFieldSize;

		lprdInfo = (LPRECORDINFO)lpTemp; // �����¼��Ϣ�ṹ
		lprdInfo->uiMask = RIM_DELETED | RIM_RECORDSIZE | RIM_FIELDSIZE;
		if( !GetRecordInfo(hTable, lprdInfo) )
		{
			// �õ���Ϣʧ��
			SetLastError(MMS_ERROR_INVALIDINDEX);  // ��Ч����
			return 0;
		}
		uFieldSize = lprdInfo->uiFieldSize[uField];

		return uFieldSize;
}

// ***************************************************************************************
// ������static void CreateDefaultFileBox(LPCTSTR lpTableName,UINT uRecordNum)
// ������
//	IN lpTableName -- Ҫ�����ĵ����ݿ���
//  IN uRecordNum -- Ҫ������Ĭ�ϵļ�¼��Ŀ
// ����ֵ��
//		����ļ����ڷ���TRUE�����򷵻�FALSE
// ����������
//		����Ĭ�ϵ����ݿ�
// ***************************************************************************************
static void CreateDefaultFileBox(LPCTSTR lpTableName,UINT uRecordNum)
{
	HTABLE hTable;
	UINT  uFileSize;

		uFileSize  = uRecordNum * MMS_MAXSIZE ;
//		RETAILMSG( 1, ( "CreateDefaultFileBox ...\r\n" ) );
		hTable = CreateTableEx(lpTableName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FIELD_NUM,uFileSize); // ����һ���µ����ݿ�
//		hTable = CreateTable(lpTableName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FIELD_NUM); // ����һ���µ����ݿ�
		if ( hTable == INVALID_HANDLE_VALUE )
		{  // �����ݿ�ʧ��
			SetLastError(MMS_ERROR_INVALIDTABLE);
			return ;
		}

		RETAILMSG( 1, ( "App OK %d\r\n",CountRecord(hTable)) );
		CloseTable(hTable); // �ر����ݿ�
		RETAILMSG( 1, ( "CreateDefaultFileBox OK\r\n" ) );

/*		{
			// test
			int i;

			for (i = 0; i < 100; i++)
			{
				hTable = CreateTable(lpTableName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				if ( hTable == INVALID_HANDLE_VALUE )
				{  // �����ݿ�ʧ��
					RETAILMSG( 1, ( "OpenTable(%s):%d Times Failure\r\n",lpTableName,i) );
					Sleep(1000);
					SetLastError(MMS_ERROR_INVALIDTABLE);
				//	return ;
				}
				else
					CloseTable(hTable); // �ر����ݿ�
			}
		}
		*/
}

// ***************************************************************************************
// ������HMMSDATA OrganizeMMSData(LPCREATEMESSAGE lpCreateMessage);
// ������
//   IN  lpCreateMessage -- Ҫ��֯��MMS���ݽṹ
// ����ֵ��
//		�ɹ�����MMS���ݾ�������򷵻�NULL
// ����������
//  	��֯Ҫ������MMS����
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
		strcpy(lpMMS_Content->lpFromTo,lpCreateMessage->lpTo);   // ������/�ռ���

		lpMMS_Content->lpSubject = malloc(strlen(lpCreateMessage->lpText) + 1);
		if (lpMMS_Content->lpSubject == NULL)
		{
			SetLastError(MMS_ERROR_NOMEMORY);
			goto OG_ERROR_RETURN;
		}
		strcpy(lpMMS_Content->lpSubject,lpCreateMessage->lpText);  // ����

//		DWORD dwDate;  // ��������
		lpMMS_Content->dwDate = EncodeDateValue(&stCurTime);

		lpMMS_Content->lpData = (LPBYTE)malloc(MMS_MAXSIZE);
		if (lpMMS_Content->lpData == NULL)
		{
			// �����ڴ�ʧ��
			SetLastError(MMS_ERROR_NOMEMORY);
			goto OG_ERROR_RETURN;
		}

		BodyDetail.iPartNum = lpCreateMessage->uAttachNum;  // �õ���ǰ�ж��ٸ�����
		

		MMS_Data.dwLenTotal = MMS_MAXSIZE;
		MMS_Data.dwLenDealed = 0;
		MMS_Data.lpData = lpMMS_Content->lpData;

		BodyDetail.lpMIME_Part = NULL;  
		for (i = 0; i < BodyDetail.iPartNum ; i++)
		{
			lpMIME_Part = (LPMIME_PART)malloc( sizeof(MIME_PART)  ); // ����һ���ռ�����ŵ�ǰ��PART
			if (lpMIME_Part == NULL)
			{
					SetLastError(MMS_ERROR_NOMEMORY);
					goto OG_ERROR_RETURN;
			}

			memset(lpMIME_Part,0,sizeof(MIME_PART)); // ��ʼ���ṹ
			if (BodyDetail.lpMIME_Part == NULL)
				BodyDetail.lpMIME_Part = lpMIME_Part;   // ���õ�һ��PART

			if (lpPrePart)
				lpPrePart->next = lpMIME_Part;  // ����ǰpart���ӵ����һ��PART

			strcpy(lpMIME_Part->lpPath, lpCreateMessage->lpAttachFile[i]);

			GetPureFileName(lpMIME_Part->lpPath,lpPart_Location);  // �õ��ļ���
			lpMIME_Part->lpContent_Location = BufferAllocCopy(lpPart_Location); // �����ļ���

			sprintf(lpPart_ID,"Kingmos%d",i + 1);  // �õ�ID
			lpMIME_Part->lpContent_ID = BufferAllocCopy(lpPart_ID);

			GetFileType(lpMIME_Part->lpPath,lpPart_Type);//�õ���������
			lpMIME_Part->lpContent_Type = BufferAllocCopy(lpPart_Type);

			sprintf(lpPart_Desc,"Kingmos file %d",i + 1);
			lpMIME_Part->lpContent_Description = BufferAllocCopy(lpPart_Desc); // ����˵��

			lpMIME_Part->dwDate = EncodeDateValue(&stCurTime);

			lpMIME_Part->lpContent_Url = BufferAllocCopy("www.mlg.com");  // ����URL

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
			ReleasePart(lpMIME_Part);   // �ͷ�

		return lpMMS_Content;

OG_ERROR_RETURN:

		if (lpMMS_Content)
		{
			if (lpMMS_Content->lpContent_Type)
				free(lpMMS_Content->lpContent_Type);  // �ͷ��������ͻ���

			if (lpMMS_Content->lpData)
				free(lpMMS_Content->lpData);  // �ͷ����ݻ���

			if (lpMMS_Content->lpFromTo)
				free(lpMMS_Content->lpFromTo);   // �ͷŷ����˻���

			if (lpMMS_Content->lpSubject)
				free(lpMMS_Content->lpSubject);   // �ͷű��⻺��

			free(lpMMS_Content);  // �ͷ����ݽṹ����
		}

		if (lpMIME_Part)
			ReleasePart(lpMIME_Part);   // �ͷ�

		return NULL;

}

// ***************************************************************************************
// ������int SaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex)
// ������
//   IN  hMMSData -- Ҫ�����MMS���ݾ��
//   IN  uMMSType -- Ҫ�����ݱ��浽��һ��������
//   IN  iIndex -- ��ǰҪ����������������-1�������һ���¼�¼
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	���浱ǰ�����Ĳ���
// ***************************************************************************************
//BOOL SaveMMSMessage(HMMSDATA hMMSData);  // ���浱ǰ�����Ĳ���
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
		// ��Ҫ�滻ԭ����������¼
		HTABLE hTable;
		UINT uRecordIndex;
#ifdef SINGLE_FILE
		FINDSTRUCT fs;
		HANDLE hFind;
#endif
#ifdef MULTI_FILE
		switch(uMMSType)
		{
			case MMS_RECEIVE:	//	�ռ���
				hTable = CreateTable(MMS_RECEIVE_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				break;
			case MMS_SEND:		//  ������
				hTable = CreateTable(MMS_SEND_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				break;
			case MMS_SENT:		//	�ѷ���Ϣ
				hTable = CreateTable(MMS_SENT_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				break;
			case MMS_SAVED:		//  �浵�ļ���
				hTable = CreateTable(MMS_SAVED_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
				break;
		}
#endif

#ifdef SINGLE_FILE
		hTable = CreateTable(MMS_DATA_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FIELD_NUM); // �����ݿ�
#endif
		if ( hTable == INVALID_HANDLE_VALUE )
		{  // �����ݿ�ʧ��
			SetLastError(MMS_ERROR_INVALIDTABLE);
			return -1;
		}
#ifdef MULTI_FILE
		uRecordIndex  = iIndex;
#endif

#ifdef SINGLE_FILE
		hFind = FindFirstRecord( lpMMSManage->hTable, 1, &lpMMSManage->chType, 1, &fs ); //���ҵ�һ����ǰ���͵�����
		if( hFind != INVALID_HANDLE_VALUE )
		{
			while( 1 )
			{
				//do somthing with record
				if ((iIndex + 1) == fs.nRecord)
				{
					uRecordIndex  = fs.nRecord -1; // �Ѿ��ҵ���ǰ�ļ�¼
					break;
				}
				if (FindNextRecord( hFind, &fs ) == 0) // ������һ����ǰ���͵�����
				{
					// �Ѿ�û��������
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
// ������BOOL SendMMSMessage(HMMSDATA hMMSData)
// ������
//   IN  hMMSData -- Ҫ���͵�MMS���ݾ��
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	���͵�ǰ�����Ĳ���
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
	if (SentMMSToServer(lpMMS_Content) == TRUE)  // ���Ͳ���
	{
		// ���ͳɹ�
		SaveMessageToTable(lpMMS_Content,MMS_SENT); // ���Ѿ����͵Ĳ��Ŵ��뵽�ѷ�����
	}

	return TRUE;
}

// ***************************************************************************************
// ������int SendAndSaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex)
// ������
//   IN  hMMSData -- Ҫ���Ͳ������MMS���ݾ��
//   IN  uMMSType -- Ҫ�����ݱ��浽��һ��������
//   IN  iIndex -- ��ǰҪ����������������-1�������һ���¼�¼
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	���Ͳ����浱ǰ�����Ĳ���
// ***************************************************************************************
int SendAndSaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex)
{
	iIndex = SaveMMSMessage(hMMSData,uMMSType,iIndex);
	SendMMSMessage(hMMSData);
	return iIndex;
}


// **************************************************
// ������static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName)
// ������
// 	IN lpFullFileName -- ȫ�ļ���������·��
// 	OUT lpPureFileName -- ���ļ�����������·��
// 
// ����ֵ����
// ������������һ��ȫ·�����ļ�������ȡһ������·�����ļ���
// ����: 
// **************************************************
static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName)
{
	const char *ptr = lpFullFileName;
	int length;

//		ptr=path;
		// Search '.'
		// ��ָ��ָ���ļ�����β
		while(*ptr)
		{
			ptr++;
		}
		// Get filename
		length=0;
		while(ptr>lpFullFileName)
		{ // ��û�е����ļ�ͷ
			ptr--;  // not include '\' and '.'
			if (*ptr=='\\')
			{  // �����ļ�·���ָ�
				ptr++;  // not includ��e '\' 
				break; // �˳�������ľ��ǲ�����·�����ļ���
			}
			length++;
		}
		if (lpPureFileName)
		{
			strcpy(lpPureFileName,ptr); // ���ƴ��ļ���������
		}
}


// **************************************************
// ������static void GetFileType(LPTSTR lpFullFileName,LPTSTR lpContentType)
// ������
// 	IN lpFullFileName -- ȫ�ļ���������·��
// 	OUT lpContentType -- �ļ�������
// 
// ����ֵ����
// ������������һ�����ļ����õ����ļ����ļ�����
// ����: 
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

		iFileTypeNum = sizeof (tabFileType) / sizeof (FILETYPE); // �õ���ǰ����֪������Ŀ
		for (i = 0 ; i < iFileTypeNum; i++ )
		{
			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), lpFullFileName, strlen(lpFullFileName)) == TRUE)
			{ // �Ƚϵ�ǰ�ļ��Ƿ�����֪�ļ�
				idFileType = tabFileType[i].iFileType;  // ʹ����֪�ļ���ͼ��
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
	LPMIME_PART lpNextPart = NULL;
		
		while(1)
		{

			if (lpMIME_Part == NULL)
				return ;

			lpNextPart = lpMIME_Part->next;

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

			lpMIME_Part = lpNextPart;
		}
}



// **************************************************
// ������static LPTSTR BufferAllocCopy(LPTSTR pSrc)
// ������
//	IN pSrc -- ԭʼ�ִ�
// 
// ����ֵ���µ��ִ�
// ��������������һ���µĿռ䲢����ԭʼ�������ݡ�
// ����: 
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
// ������static int WINAPI  SendMMSThread(LPVOID lParam)
// ������
//	
// ����ֵ�����ͳɹ�����TRUE�����򷵻�FALSE��
// �������������Ͳ����̡߳�
// ����: 
// **************************************************
static int WINAPI SendMMSThread(LPVOID lParam )
{
	LPMMS_CONTENT lpMMS_Content;
	UINT uIndex ,iCount;
	HMMS hMMS;

	while(1)
	{
		hMMS = OpenMMS(MMS_SEND); // ��Ҫ���͵Ĳ������ݿ�
		iCount = GetMMSCount(hMMS); // �õ����ݿ���Ҫ���͵Ĳ�������

		uIndex = 0;
		while (uIndex < iCount)
		{ 
			// ��Ҫ���͵Ĳ��Ŵ���
			lpMMS_Content = (LPMMS_CONTENT)LoadMMSData(hMMS,0); // �õ���һ����¼������
			if (lpMMS_Content != NULL)
			{
//				if (StartSendMMSMessage(lpMMS_Content) == TRUE)
				if (SentMMSToServer(lpMMS_Content) == TRUE)  // ���Ͳ���
				{
					// ���ͳɹ�
					DeleteMMS(hMMS, uIndex); // ���ͼ�����ɾ���Ѿ����ͳɹ��Ĳ���
					SaveMessageToTable(lpMMS_Content,MMS_SENT); // ���Ѿ����͵Ĳ��Ŵ��뵽�ѷ�����
				}
				ReleaseMMSData(hMMS,(HMMSDATA)lpMMS_Content); // �ͷ�װ�ص�����
			}
			else
			{
				uIndex ++;  // ������һ����¼
			}
		}

		CloseMMS(hMMS); // �ر����ݿ�

		Sleep(1000 * 6);  // һ���Ӳ�ѯһ��
	}
	return TRUE;
}


// **************************************************
// mms ���Ӵ���
// **************************************************

// **************************************************
// ������static ATOM RegisterMMSMonitorWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��Ӧ�ó��򴰿��ࡣ
// ����: 
// **************************************************
static ATOM RegisterMMSMonitorWindow(HINSTANCE hInstance)
{
	WNDCLASS wc;

    wc.style = 0;//CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) MonitorWndProc; // Ӧ�ó�����̺���
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL; // װ��Ӧ�ó���ͼ��
    wc.hCursor = 0;
    wc.hbrBackground = NULL;//(HBRUSH) GetStockObject(GRAY_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = classMMSMonitorWindow; // Ӧ�ó��򴰿�����

	return RegisterClass(&wc); // ע����
}


// **************************************************
// ������static LRESULT CALLBACK MonitorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��ͬ����Ϣ�в�ͬ�ķ���ֵ�����忴��Ϣ����
// ����������Ӧ�ó��򴰿ڹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK MonitorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			EndPaint(hWnd, &ps);
			break;
		case WM_NEW_MMS_PUSH:
			NewMMSArrive(hWnd,wParam,lParam); // �в��ŵ���
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// **************************************************
// ������static LRESULT NewMMSArrive(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//  IN wParam  - ��Ϣ����
//  IN lParam  - ��Ϣ����
// ����ֵ��
//	��
// �������������ڵõ�һ�����ŵ�֪ͨ��Ϣ
// ����: 
// ********************************************************************
static LRESULT NewMMSArrive(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMMS_CONTENT lpMMS_Content = NULL;
	DWORD dwDataID;

		lpMMS_Content = (LPMMS_CONTENT)malloc(sizeof(MMS_CONTENT)); // ����һ��MMS���ݵĽṹ����
		if (lpMMS_Content == NULL)
		{ // ����ʧ��
			SetLastError(MMS_ERROR_NOMEMORY); // ϵͳ�ڴ治��
			return 0;
		}
		memset((LPVOID)lpMMS_Content,0,sizeof(MMS_CONTENT)); // ����ṹ����

		dwDataID = wParam;

		if (GetNotificationFromServer(dwDataID ,lpMMS_Content) == TRUE)
		{
			SaveMessageToTable(lpMMS_Content,MMS_RECEIVE);
		}

		if (lpMMS_Content)
		{
			if (lpMMS_Content->lpContent_Type)
				free(lpMMS_Content->lpContent_Type);  // �ͷ��������ͻ���

			if (lpMMS_Content->lpData)
				free(lpMMS_Content->lpData);  // �ͷ����ݻ���

			if (lpMMS_Content->lpFromTo)
				free(lpMMS_Content->lpFromTo);   // �ͷŷ����˻���

			if (lpMMS_Content->lpSubject)
				free(lpMMS_Content->lpSubject);   // �ͷű��⻺��

			free(lpMMS_Content);  // �ͷ����ݽṹ����
		}
		return 0;
}


// ***************************************************************************************
// ������static void WriteMessageToTable(HTABLE hTable,LPMMS_CONTENT lpMMS_Content,UINT uMessageType)
// ������
//	IN hTable -- ��ǰ�򿪵�TABLE
//	IN lpMMS_Content -- Ҫ����Ĳ���
//	IN bNewMessage -- �Ƿ����µĲ���
// ����ֵ��
//		��
// ����������
//		����һ���Ĳ���
// ***************************************************************************************
static void WriteMessageToTable(HTABLE hTable,LPMMS_CONTENT lpMMS_Content,UINT uMessageType)
{
	UINT uStatus = 0;
	FIELDITEM fieldItem[FIELD_NUM];
	static TCHAR lpData[0XE1F];

//		RETAILMSG(1,(TEXT("WriteMessageToTable \r\n")));
		// ���ݹ滮		
		//  Field 0 -- |  Flag  | Field 0 Len | Field 1 Len | ... ... | Field n Len | 
		//  Field 1 -- |Field 0 Data | Field 1 Data | ... | Field n Data |

		//  ���� Flag Ϊ��ǰ��Ŀ�ı�־, DWORD (4 BYTE)
		//  flag --  bit 31  -- ��ǰ�����Ƿ���Ч
		//			 bit 15 -- bit 0 -- ��ǰ��¼��״̬
		//  Field 0 Len ... Field n Len Ϊ�����ֶεĳ��� ÿ���ֶ�Ϊһ��WORD (2 BYTE)
		//  Field 0 Data  ...  Field n Data  Ϊ�����ֶε�����

#ifdef SINGLE_FILE
		// ��������
		switch(uMessageType)
		{
			case MMS_RECEIVE:	//	�ռ���
				fieldItem[FIELD_TYPE].lpData = "R";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SEND:		//  ������
				fieldItem[FIELD_TYPE].lpData = "T";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SENT:		//	�ѷ���Ϣ
				fieldItem[FIELD_TYPE].lpData = "H";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			case MMS_SAVED:		//  �浵�ļ���
				fieldItem[FIELD_TYPE].lpData = "S";
				fieldItem[FIELD_TYPE].size = 1;
				break;
			default: // ����ʶ��MMS����
				SetLastError(MMS_ERROR_UNKNOWMMSTYPE); // ����ʶ��MMS����
				return;
		}
#endif

		// ����״̬
		uStatus |= MMS_READ ; //����MMSδ��
		fieldItem[FIELD_STATUS].lpData = &uStatus;
		fieldItem[FIELD_STATUS].size = sizeof(DWORD);


		// д���ֶ� 0 -- ������/�ռ���
		fieldItem[FIELD_FROMTO].lpData = lpMMS_Content->lpFromTo;
		fieldItem[FIELD_FROMTO].size = strlen(lpMMS_Content->lpFromTo) + 1; // ������β��

		// д���ֶ� 1 --  ����
		fieldItem[FIELD_SUBJECT].lpData = lpMMS_Content->lpSubject;
		fieldItem[FIELD_SUBJECT].size = strlen(lpMMS_Content->lpSubject) + 1; // ������β��

		// д���ֶ� 2 --  ʱ��
		fieldItem[FIELD_DATE].lpData = &lpMMS_Content->dwDate;
		fieldItem[FIELD_DATE].size = sizeof(DWORD); 

		// д���ֶ� 3 --  ��������
		fieldItem[FIELD_CONTENTTYPE].lpData = lpMMS_Content->lpContent_Type;
		if (lpMMS_Content->lpContent_Type == NULL)
		{
			fieldItem[FIELD_CONTENTTYPE].size = 0; // ������β��
		}
		else
		{
			fieldItem[FIELD_CONTENTTYPE].size = strlen(lpMMS_Content->lpContent_Type) + 1; // ������β��
		}

		// д���ֶ� 4 --  ����

		fieldItem[FIELD_MIME].lpData = lpMMS_Content->lpData;
		fieldItem[FIELD_MIME].size = lpMMS_Content->iDataSize; // ������β��

		WriteRecord( hTable, (FIELDITEM *)&fieldItem);

}


// **************************************************
// ������static LONG EncodeDateValue( SYSTEMTIME *lpSystemTime)
// ������
//	IN lpSystemTime -- Ҫ���б����ʱ��
// 
// ����ֵ���ɹ�������LONG����
// ��������������һ��DATE�����ݡ�
// ����: 
// **************************************************
static LONG EncodeDateValue( SYSTEMTIME *lpSystemTime)
{
	SYSTEMTIME curDate ;
	DWORD dwSecs = 0; // �õ���1970 - 01 - 01 , 00:00:00 ��ʼ������
	int iDays;

		// ��ʼ����ʼʱ�� 1970 - 01 - 01 , 00:00:00
		RETAILMSG(1,(TEXT(" Current Date = %d-%02d-%02d %02d:%02d:%02d\r\n"),lpSystemTime->wYear,lpSystemTime->wMonth,lpSystemTime->wDay,lpSystemTime->wHour,lpSystemTime->wMinute,lpSystemTime->wSecond));

		curDate.wYear = 1970;
		curDate.wMonth = 1;
		curDate.wDay = 1;
		curDate.wHour = 0;
		curDate.wMinute = 0;
		curDate.wSecond = 0;
		curDate.wMilliseconds = 0;



		iDays = DateDiff(&curDate, lpSystemTime); // �õ���1970-01-01 ��ʼ�����������
		dwSecs = iDays * (3600 * 24) ; // �õ������������ܹ�����

		// �õ���ǰ��ʱ��
		dwSecs += lpSystemTime->wHour * 3600;  // ����Сʱ����Ҫ������

		dwSecs += lpSystemTime->wMinute * 60;  // �õ���ȥ�����������

		dwSecs += lpSystemTime->wSecond; // ��������
		
		return dwSecs; 
}


// ***************************************************************************************
// ������UINT GetMMSCapacity(HMMS hMMS)
// ������
//    hMMS -- �Ѿ��򿪵�MMS���
// ����ֵ��
//		���ص�ǰϵͳ�е�MMS������
// ����������
//  	�õ���ǰϵͳ��MMS������
// ***************************************************************************************
UINT GetMMSCapacity(HMMS hMMS)
{
	LPMMSMANAGE lpMMSManage;

		if (hMMS == NULL)
		{  // �����MMS���
			RETAILMSG(1,(TEXT("hMMS == NULL \r\n")));
			SetLastError(MMS_ERROR_INVALIDHANDLE);
			return 0;
		}
		lpMMSManage = (LPMMSMANAGE)hMMS; // �õ�MMS����ṹ��ָ��
		switch(lpMMSManage->uMMSType)
		{
			case MMS_RECEIVE:	//	�ռ���
				return RECORDNUM_RECEIVE;
			case MMS_SEND:		//  ������
				return RECORDNUM_SEND;
			case MMS_SENT:		//	�ѷ���Ϣ
				return RECORDNUM_SENT;
			case MMS_SAVED:		//  �浵�ļ���
				return RECORDNUM_SAVED;
			default: // ����ʶ��MMS����
				return 0;
		}
}