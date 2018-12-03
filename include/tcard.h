/***************************************************
Copyright (c) ���У�1998-2003 ΢�߼�(R)����������Ȩ����
***************************************************/

/**************************************************
�ļ�˵����TCARD ���ͷ�ļ�����
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-06-24
��ƬӦ�ã������� simcard, ������־��Ƭ, MMS(����)�Ķ�ȡ
�޸ļ�¼��
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

// ���� Mobile ��Ϣ���ͣ����š����š��绰��
#define MAX_PHONELEN	(MAX_PHONE_LEN+4)	// ����绰�������󳤶�,����'\0'�������뵽4
//#define MAX_NUMBERLEN	(MAX_PHONE_LEN+1)	// ����绰�������󳤶�,����'\0'�������뵽4
#define MAX_SMGLEN		(160+4)			    // �������Ϣ����󳤶ȣ�����'\0'�������뵽4
//#define MAX_CONTENTLEN	(MAX_SMGLEN+1)	// �������Ϣ����󳤶�,����'\0'�������뵽4
#define MAX_NAMELEN		(MAX_NAME_LEN+4)	// ������������󳤶�,����'\0'�������뵽4
#define MAX_PINLEN		(6+2)				// ����PIN����󳤶�,����'\0'�������뵽4
#define MAX_CAPTION

// ����SIM���绰��������Ϣ�ṹ
typedef struct _SIMPHONE_ITEM{
	UINT		nSize;					// = sizeof(SIMPHONE_ITEM)
	UINT		nPosIndex;				// ��ʲôλ�ÿ�ʼ���ң���ʼ��Ϊ1
	TCHAR		szNumber[MAX_PHONELEN];	// �绰���루(�����еĻ�)��
	TCHAR		szName[MAX_NAMELEN];	// ������������ʾ��˵���ı�(�����еĻ�)
}SIMPHONE_ITEM, FAR * LPSIMPHONE_ITEM;


//����Ϣ�ı�־״ֵ̬
#define SMSF_RECV_UNREAD	0x0			//����Ϣ�ѽ��յ���δ����ȡ
#define SMSF_RECV_READED    0x1			//����Ϣ�ѽ��ղ�����ȡ
#define SMSF_STO_UNSENT     0x2			//����Ϣ�ѱ��浫δ������
#define SMSF_STO_SENT		0x3			//����Ϣ�ѱ������ѷ���
typedef struct _SMS_RECORD{
    UINT		nSize;					// = sizeof(SMS_RECORD)
	SYSTEMTIME	sysTime;				// ʱ�� 
	UINT		uFlag;					// ����Ϣ�ı�־״ֵ̬
	UINT		nPosIndex;				// ��ʲôλ�ÿ�ʼ���ң���ʼ��Ϊ1
	TCHAR		szNumber[MAX_PHONELEN];	// �绰���루(�����еĻ�)��
	TCHAR		szContent[MAX_SMGLEN];	// ����
	TCHAR		szName[MAX_NAMELEN];	// ������������ʾ��˵���ı�(�����еĻ�)
}SMS_RECORD, FAR * LPSMS_RECORD;
typedef SMS_RECORD SIMSMS_ITEM;
typedef LPSMS_RECORD LPSIMSMS_ITEM;

// ����MMS������Ϣ�ṹ
typedef struct _MMS_FIND_ITEM{
    UINT		nSize;					// = sizeof(SIMSMS_ITEM)
	MMSINFO		MMSInfo;				// ����Ϣ������
	UINT		nPosIndex;				// ��ʲôλ�ÿ�ʼ���ң���ʼ��Ϊ1
}MMS_FIND_ITEM, FAR * LPMMS_FIND_ITEM;

// ������־��¼������Ϣ�ṹ
typedef struct _LOG_RECORD{
    UINT		nSize;					// = sizeof(LOG_ITEM)
	UINT		nLogType;				// ��¼���ͣ�ָʾ���롢�����δ�ӣ�
	UINT		nPosIndex;				// ��ʲôλ�ÿ�ʼ���ң���ʼ��Ϊ1 //
	SYSTEMTIME	sysTime;				// ��ʼʱ�� 
	DWORD		dwTalkTime;				// ͨ��ʱ�䣨�룩
	TCHAR		szNumber[MAX_PHONELEN];	// �绰����(�����еĻ�) 
	TCHAR		szName[MAX_SMGLEN];		// ����
	BOOL		bRead;					// �Ƿ��Ѿ���
}LOG_RECORD, FAR * LPLOG_RECORD;

// ������־���Ӽ�¼������Ϣ�ṹ 
typedef struct _LOG_SAVE_ITEM{
    UINT		nSize;					// = sizeof(LOG_ADD_ITEM)
	UINT		nLogType;				// ��¼���ͣ�ָʾ���롢�����δ�ӣ�
	SYSTEMTIME	sysTime;				// ��ʼʱ��
	DWORD		dwTalkTime;				// �����еĻ���ͨ��ʱ�䣨�룩
	TCHAR		szNumber[MAX_PHONELEN];	// �绰����
}LOG_SAVE_ITEM, FAR * LPLOG_SAVE_ITEM;


enum{
	CARD_NULL = 0,
	CARD_SIMPHONE = 0x1,	//SIM Card �绰��
	CARD_SIMSMS,			//SIM Card ����Ϣ
	CARD_RECENT,			//	  0x3			//����
	CARD_OUT,				//    0x4			//���
	CARD_IN,				//    0x5			//����
	CARD_UNRECEIVE,			//    0x6			//δ��
	CARD_ADDRESSBOOK,		//    0x7
	CARD_MMSRECEIVE,		//    0x8			// �����ռ���
	CARD_MMSSEND,			//    0x8			// ���ŷ�����
	CARD_MMSSENT,			//    0x9			// �����ѷ�
	CARD_MMSSAVE,			//    0xA			// ���ű���
	CARD_SMSRECEIVE,		//�����ռ���
	CARD_SMSSEND,			//���ŷ�����
	CARD_SMSSENT,			//�����ѷ���Ϣ
	CARD_SMSSAVE,			//���Ŵ浵��Ϣ
	MAX_CARDS
};

//ͳ�ƿ�Ƭ�õ����ݽṹ
typedef struct _TCARD_INFO
{
	UINT uiSize;		//���� = sizeof(TCARD_INFO)
	UINT nTotalRecords;
	UINT nFreeRecords;
	UINT nUsedRecords;
}TCARD_INFO, FAR * LPTCARD_INFO;

//������������
#define SRM_NAME		1
#define SRM_PHONECODE	2
#define SRM_CONTENT		3
#define SRM_SMSSTATE	4	// SIM ������״̬
//
//��Ƭ���ý��棬һ���µĿ�Ƭ����ʵ�����¹���
typedef struct _TCARDDRV
{	//��,����hCardOpenObj
	HANDLE (*lpInit)( DWORD dwContext );
	VOID (*lpDeinit)( HANDLE hInitHandle );
	HANDLE (*lpOpen)( HANDLE hInitHandle );
	//�ر�
	BOOL (*lpClose)( HANDLE hCardOpenObj );
	//���ҷ���Ҫ��ĵ�һ����¼
    BOOL   (*lpFindFirstRecord)( HANDLE hCardOpenObj, LPCTSTR lpszSearchMask, UINT uiMaskType, LPVOID lpvIDData );
	//���ҷ���Ҫ����һ����¼
	BOOL (*lpFindNextRecord)(  HANDLE hCardOpenObj, LPCTSTR lpszSearchMask, UINT uiMaskType, LPVOID lpvIDData, UINT uMethod );
	//ɾ��һ����¼
	BOOL (*lpDeleteRecord)( HANDLE hCardOpenObj, UINT index );
	//����һ����¼
	BOOL (*lpSaveRecord)( HANDLE hCardOpenObj, UINT nSavePosIndex, LPCVOID lpvIDData, UINT * lpRealSavePosIndex );
	BOOL (*lpGetInfo)( HANDLE hCardOpenObj, LPTCARD_INFO lptci );
	BOOL (*lpGetRecordInfo)( HANDLE hCardOpenObj, UINT nPosIndex, LPVOID lpvIDData );
	//�뿨Ƭ�Ľ�������,����ˢ�£�����ȵ�
	BOOL (*lpIoControl)( HANDLE hCardOpenObj, UINT uMsg, WPARAM wParam, LPARAM lParam );
}TCARDDRV, FAR * LPTCARDDRV;

//��ʼ����Ƭ
BOOL TCARD_Init( VOID );

VOID TCARD_Deinit( VOID );

//�򿪿�Ƭ
BOOL TCARD_Open( VOID );
//�رտ�Ƭ
BOOL TCARD_Close( VOID );

//����ƥ��ļ�¼
BOOL TCARD_FindFirstRecord(
							   UINT uiCardID, 
							   LPCTSTR lpszSearchMask,
							   UINT uiMaskType,
							   LPVOID lpvIDData
							  );
//������һ����¼
#define FNRM_NEXT    0			//���²�
#define FNRM_PREV    0x1		//���ϲ�

BOOL TCARD_FindNextRecord( 
							 UINT uiCardID, 
							 LPCTSTR lpszSearchMask,
							 UINT uiMaskType,
							 LPVOID lpvIDData,
							 UINT uMethod
							 );
//ɾ��һ����¼
BOOL TCARD_DeleteRecord(   
						   UINT uiCardID, 
						   UINT index
						  );
//����һ����¼�����nPosIndexΪ-1����Ϊ����λ�ã��ɿ�Ƭ�Լ�������
// lpOutIndex ���ڽ���ʵ�ʱ����λ�ã�����ΪNULL
BOOL TCARD_SaveRecord(  
						 UINT uiCardID,
						 UINT nPosIndex,
						 LPCVOID lpvIDData,
						 UINT * lpOutIndex
						);

//ͳ�ƿ�Ƭ����Ϣ
BOOL TCARD_GetInfo(
					 UINT uiCardID,
					 LPTCARD_INFO lptci
					);

//�õ�һ����¼����Ϣ
BOOL TCARD_GetRecordInfo(
					 UINT uiCardID,
					 UINT nPosIndex,
					 LPVOID lpvIDInfo
					);

//�뿨Ƭ�Ľ�������,����ˢ�£�����ȵ�

BOOL TCARD_IoControl( UINT uiCardID, UINT uMsg, WPARAM wParam, LPARAM lParam );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif		//__TCARD_H
