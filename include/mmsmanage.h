/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __MMSMANAGE_H
#define __MMSMANAGE_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

typedef void * HMMS;  // һ��MMS��������ӵ�иþ���Ϳ��Է���MMS
typedef void * HMMSDATA;  // һ��MMS��Ϣ�����ݾ����ӵ�иþ�����Ϳ��ԶԸ���Ϣ������ʾ

// MMS��Ϣ�ṹ
#define INFO_FROMTO		0x0001  // ���ŵķ�����/�ռ���
#define INFO_SUBJECT	0x0002  // ���ŵı���
#define INFO_DATE		0x0004  // ���ŵ�ʱ��
#define INFO_STATUS		0x0008  // �����Ƿ��Ѷ�

#define MMS_NOTREAD		0x0001	// δ��MMS
#define MMS_READ		0x0002  // �Ѷ�MMS

typedef struct structMMSInfo
{
	UINT uSize;  // �ṹ��С��Ϊ�Ժ���չ��׼��
	UINT uMask; // �趨��Щ������Ч��

	LPTSTR lpFromTo;  // �洢������/�ռ�����Ϣ
	UINT uMaxFromTo;  // ������/�ռ��˻����С

	LPTSTR lpSubject;  // �洢������Ϣ
	UINT uMaxSubject;  // ���⻺���С

	SYSTEMTIME MMSTime;  // MMS�ķ���ʱ��

	UINT uStatus;  // ��ʾ��ǰMMS��״̬

}MMSINFO, * LPMMSINFO;

#define MMS_RECEIVE		0x0001	//	�ռ���
#define MMS_SEND		0x0002	//  ������
#define MMS_SENT		0x0003	//	�ѷ���Ϣ
#define MMS_SAVED		0x0004	//  �浵�ļ���

// ***************************************************************************************
// ������BOOL InitialMMSSystem(void)
// ������
//	��
//
// ����ֵ��
//		��
// ����������
//		��ʼ��MMSϵͳ
// ***************************************************************************************
BOOL InitialMMSSystem(void);

// ***************************************************************************************
// ������BOOL DeinitialMMSSystem(void)
// ������
//
// ����ֵ��
//		��
// ����������
//		Deinitial MMSϵͳ
// ***************************************************************************************
BOOL DeinitialMMSSystem(void);


// ***************************************************************************************
// ������HMMS OpenMMS(UINT uMMSType)
// ������
//	IN uMMSType  -- Ҫ�򿪵�MMS�����ͣ���ΪMMS_RECEIVE(�ռ���)��MMS_SEND(������),MMS_SENT(�ѷ���Ϣ),MMS_SAVED(�浵�ļ���)
// ����ֵ��
//	����һ��MMS���
// ����������
//  	��һ��MMS�����Ժ����ʹ�øþ����MMS���з���
// ***************************************************************************************
HMMS OpenMMS(UINT uMMSType);

// ***************************************************************************************
// ������void CloseMMS(HMMS hMMS)
// ������
//    hMMS -- Ҫ�رյ�MMS���
// ����ֵ��
//	��
// ����������
//  	�ر�һ��MMS�����Ժ�Ͳ��ܶ�MMS���з���
// ***************************************************************************************
void CloseMMS(HMMS hMMS);


// ***************************************************************************************
// ������UINT GetMMSCount(HMMS hMMS);
// ������
//    hMMS -- �Ѿ��򿪵�MMS���
// ����ֵ��
//		���ص�ǰϵͳ�е�MMS����Ŀ(�����Ѿ����صĺ�û�����ص�)
// ����������
//  	�õ���ǰϵͳ��MMS����Ŀ
// ***************************************************************************************
UINT GetMMSCount(HMMS hMMS);

// ***************************************************************************************
// ������UINT GetMMSCapacity(HMMS hMMS);
// ������
//    hMMS -- �Ѿ��򿪵�MMS���
// ����ֵ��
//		���ص�ǰϵͳ�е�MMS������
// ����������
//  	�õ���ǰϵͳ��MMS��������С
// ***************************************************************************************
UINT GetMMSCapacity(HMMS hMMS);

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
BOOL GetMMSInfo(HMMS hMMS, UINT uIndex,LPMMSINFO lpMMSInfo);


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
BOOL DeleteMMS(HMMS hMMS, UINT uIndex);


// ***************************************************************************************
// ������BOOL ClearMMS(HMMS hMMS);
// ������
//   IN  hMMS -- �Ѿ��򿪵�MMS���
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	ɾ�����е�MMS
// ***************************************************************************************
BOOL ClearMMS(HMMS hMMS);

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
HMMSDATA LoadMMSData(HMMS hMMS, UINT uIndex);


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
BOOL ReleaseMMSData(HMMS hMMS, HMMSDATA hMMSData);


// �����ó���ʧ�ܣ������GetLastError , �õ���ǰ�Ĵ�������

#define MMS_ERROR_SUCCESS			0x0000		// û�д���
#define MMS_ERROR_NOMEMORY			0x3000		// ϵͳ�ڴ治��
#define MMS_ERROR_UNKNOWMMSTYPE		0x3001		// ����ʶ��MMS����
#define MMS_ERROR_INVALIDHANDLE		0x3002		// ��Ч���
#define MMS_ERROR_INVALIDTABLE		0x3003		// ��������ݿ�
#define MMS_ERROR_INVALIDINDEX		0x3004		// ��Ч����
#define MMS_ERROR_OVERFLOWSIZE		0x3005		// ��������̫��
#define MMS_ERROR_FULL				0x3006		// ��ǰ��¼�Ѿ���
#define MMS_ERROR_INVALIDPARAMETER	0x3007		// ��Ч����
#define MMS_ERROR_TYPE				0x3008		// ����Ĳ�������
#define MMS_ERROR_ATTACHFLOW		0x3009		// �����ߴ����
#define MMS_ERROR_READMMSFAILURE	0x300A		// ��ȡ��������ʧ��


#define MAX_ATTACHNUM  6    // �û���������ӵĸ�����

// uMask ����
#define CM_MASK_TO		  0x0001    // �з�������Ϣ��lpTo ������Ч
#define CM_MASK_TEXT	  0x0002    // ���ı���ӣ�lpText ������Ч
#define CM_MASK_ATTACH    0x0004    // ��Ҫ��ӵĸ�����uAttachNum �� lpAttachFile ������Ч

// ��������һ���µĲ��ŵĽṹ
typedef struct CreateMessageStruct{
	UINT uSize;  // �ṹ��С��Ϊ�Ժ���չ��׼��
	UINT uMask; // �趨��Щ������Ч��

	LPTSTR lpTo;  // �ռ�����Ϣ
	LPTSTR lpText;  // Ҫ���͵��ı���Ϣ

	UINT uAttachNum; // Ҫ��ӵĸ����ĸ���
	LPTSTR lpAttachFile[MAX_ATTACHNUM]; // �û�Ҫ��ӵĸ������ļ���

}CREATEMESSAGE,*LPCREATEMESSAGE;


// ***************************************************************************************
// ������HMMSDATA OrganizeMMSData(LPCREATEMESSAGE lpCreateMessage);
// ������
//   IN  lpCreateMessage -- Ҫ��֯��MMS���ݽṹ
// ����ֵ��
//		�ɹ�����MMS���ݾ�������򷵻�NULL
// ����������
//  	��֯Ҫ������MMS����
// ***************************************************************************************
HMMSDATA OrganizeMMSData(LPCREATEMESSAGE lpCreateMessage);

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
int SaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex);

// ***************************************************************************************
// ������BOOL SendMMSMessage(HMMSDATA hMMSData)
// ������
//   IN  hMMSData -- Ҫ���͵�MMS���ݾ��
// ����ֵ��
//		�ɹ�����TRUE�����򷵻�FALSE
// ����������
//  	���͵�ǰ�����Ĳ���
// ***************************************************************************************
BOOL SendMMSMessage(HMMSDATA hMMSData);  // ���͵�ǰ�����Ĳ���

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
int SendAndSaveMMSMessage(HMMSDATA hMMSData,UINT uMMSType,int iIndex);


// ***************************************************************************************
// ������UINT GetMMSCount(HMMS hMMS);
// ������
//    hMMS -- �Ѿ��򿪵�MMS���
// ����ֵ��
//		���ص�ǰϵͳ�е�MMS����Ŀ(�����Ѿ����صĺ�û�����ص�)
// ����������
//  	�õ���ǰϵͳ��MMS����Ŀ
// ***************************************************************************************
UINT GetMMSCount(HMMS hMMS);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__MMSMANAGE_H
