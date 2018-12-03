/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

#ifndef _MAILBOX_API_H_
#define _MAILBOX_API_H_
#define NOT_AUTO_CLOSE_NET //��������أ������겻��ص�����

#ifdef EML_WIN32
	#include <emlsocket.h>
#else
	#include "socket.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */  
#ifndef INLINE_PROGRAM
#define _TEST_MAIL  
#endif	
#ifdef _TEST_MAIL
#define TMAIL 1  //��������壬��������Ӧ�ó�����ĵ�����Ϣ
#define TME 1    //�򿪵�����Ϣ (�����������֣�
#else
#define TME 0
#define TMAIL 0
#endif

#define szReceiveBox     TEXT("\\�ֻ�\\ϵͳ����\\email\\Receive\\")
#define szReceiveBoxs    TEXT("\\�ֻ�\\ϵͳ����\\email\\Receive")
#define szSentBox        TEXT("\\�ֻ�\\ϵͳ����\\email\\Send\\")
#define szSentBoxs       TEXT("\\�ֻ�\\ϵͳ����\\email\\Send")
#define szDraftBox       TEXT("\\�ֻ�\\ϵͳ����\\email\\Draft\\")
#define szDraftBoxs      TEXT("\\�ֻ�\\ϵͳ����\\email\\Draft")
#define szEmailRoot      TEXT("\\�ֻ�\\ϵͳ����\\")
#define szEmailRoots     TEXT("\\�ֻ�\\ϵͳ����")
#define szEmaildir       TEXT("\\�ֻ�\\ϵͳ����\\email\\")
#define szEmaildirs      TEXT("\\�ֻ�\\ϵͳ����\\email")
#define szEmailCfgDir    TEXT("\\�ֻ�\\ϵͳ����\\email\\config")
#define szEmailCfgTempDir	TEXT("\\�ֻ�\\ϵͳ����\\email\\config\\mlg")

//��emailcomm.c���øú���һ��6��ʱ�ļ����ļ��������Բ�����ʽ �½�һ���ʼ�ʱ���Ը��ļ������ݣ���
//һ������Ҫ����Ϣ�󣬱�������ɾ�����ļ���
#define szECTempFileName  TEXT("\\�ֻ�\\ϵͳ����\\email\\config\\newtemp.dxt")
#define szECTempDirectory TEXT("\\�ֻ�\\ϵͳ����\\email\\config\\mlg")
#define szTempEmailNameInMApi  TEXT("\\�ֻ�\\ϵͳ����\\email\\tempemail.non")//��mailboxapi.c
/******************************��mailconfig.c ***********************/
#define szMlcfgTempIDTable TEXT("\\�ֻ�\\ϵͳ����\\Email\\config\\tempMesID.dat")    //�����жϵ�ǰ����ID�б����Ѿ�DOWN�µ��ʼ�ID�Ƚ�   
#define szMlcfgExistIDTable TEXT("\\�ֻ�\\ϵͳ����\\Email\\config\\Receive.dat")     //���������ʼ���ID�б���ļ�
#define szMlcfgConfigName  TEXT("\\�ֻ�\\ϵͳ����\\Email\\EKeyCon.ems")
/******************************************************/


void SetCurrentError(int error);
int GetCurrentError(void); 
//�г�POP3ģ������ݽṹ�ͺ����б�
/*****************************   Pop3 Server *****************************/
//typedef struct _Pop3Socket Pop3Socket,*PPop3Socket ;
typedef struct Pop3Socket {
        LPTSTR   m_Pop3IPAddr ;         //Pop3 Server IP��ַ
		int      m_Port ;
		LPTSTR   m_UserName ;       //Pop3 User �ʺ�
		LPTSTR   m_Password;        //����
		LPTSTR   m_Dir;             //�ʼ����ض�λ��Ŀ¼��һ�����ռ���
		int   *  m_pNewIndex;       //һ�����ʼ���Index�б�
		int      m_iNewCnt ;        //���ʼ�������
		int      m_BoxTotal;        //�������е��ʼ�����
        BOOL     m_bDownAllOrNew;   //�������ʼ����������ʼ�
#define SIZE_NOLIMIT 0
#define SIZE_200K  1
#define SIZE_500K  2
#define SIZE_1000K 3
		UINT     m_SizeLimit;
		BOOL     m_bCopyAndLoad ;  //����ʱ����һ���ڷ�������
        SOCKET   m_Socket;
}Pop3Socket,*PPop3Socket ;

BOOL InitializePop3Socket( PPop3Socket pPop3 ) ;
BOOL DeInitializePop3Socket( PPop3Socket pPop3 );

BOOL ConnectServerAndDown( PPop3Socket pPop3 ,int * iMax);

BOOL DownLoadMail( PPop3Socket pPop3, int iIndex );
BOOL DownLoadAllMails( PPop3Socket pPop3 ); 
BOOL DownLoadNewMails( PPop3Socket pPop3 );      
/*****************************   Pop3 Server *****************************/

//�г�SMTPģ������ݽṹ�ͺ����б�
/*****************************  Smtp Server *****************************/
typedef struct  _AttachFile {
       CHAR *  *ppFileList ; //�����б�
	   int     iCnt ;       //��������
}AttachFile,* PAttachFile;

typedef struct ToAddress 
{
       CHAR * szToAddress ;  
       BOOL   bRet        ;
}ToAddress ,*PToAddress;

typedef struct ToList 
{
      ToAddress * ppTlst ; //���͵�ַ���б�
	  int       iToCnt ; //���͵�ַ������
}ToList ,* PToList ;

typedef struct  SmtpSocket {
       LPTSTR   m_SmtpIPAddr ;//SMTP ��������ַ
	   int      m_Port ;
       LPTSTR   m_UserName ;       //Smtp User �ʺ�
	   LPTSTR   m_Password;        //����
	   LPTSTR   m_From       ;//�ʼ�����Դ
	   LPTSTR   m_Reply      ;//�ظ���ַ
	   ToList   m_pToList    ;//�ʼ���ȥ��
	   LPTSTR   m_Data       ;//���ʼ���ʱ��
	   LPTSTR   m_Cc         ;//����
	   LPTSTR   m_Subject    ;//����
	   LPTSTR   m_Body       ;//����
	   int      m_BodyType   ;//0: means no Body. 1:means Text Body.2:means Htm body.
	   BOOL     m_bHasAttach ;//���޸���
	   AttachFile m_AttachList;
	   SOCKET   m_Socket ;
}SmtpSocket ,*PSmtpSocket;
BOOL  InitializeSmtpSocket( PSmtpSocket pSmtp );
void  DeInitializeSmtpSocket( PSmtpSocket pSmtp );
void DeInitToAddress( PSmtpSocket pSmtp );
BOOL InitToAddress( PSmtpSocket pSmtp ,LPTSTR szTo);
BOOL  SendEmail( PSmtpSocket pSmtp  );

/*****************************  Smtp Server *****************************/

/***************************** Config Info ******************************/
typedef struct ConfigInfo {
	LPTSTR pop3server;
	int    pop3port;
	LPTSTR smtpserver;
	int    smtpport;
	LPTSTR account;
	LPTSTR password;

	LPTSTR user;
	LPTSTR Company;
	LPTSTR From;
	LPTSTR Reply;
    
	int    Interval;
	int    iSaveNum;
	BOOL bDel;
	BOOL bDownAll;
	int  iSizeLimit;
}CONFIGINFO,* PConfigInfo;
BOOL SocketStartUp(DWORD hiVersion,DWORD loVersion);
void SocketCleanUp( );
void ErrorConvertStr(int error,LPTSTR szDest);
BOOL  SaveTempEmailFile( PSmtpSocket pSmtp, UINT  uiFlags ,CHAR *szDefault );
/***************************** Config Info ******************************/
void DeInitEmailConfig(CONFIGINFO * config);
void  InitEmailConfig(CONFIGINFO *  config);
BOOL GetEmailConfig( CONFIGINFO * pConfig );
void SaveEmailConfig( CONFIGINFO * pConfig );
void RefreshIDList( int num , int * pNewList ) ;
int GetNewEmailInfo( CHAR * IDList, int *pNewList );
int CheckAndDownLoad();
/***************************** Config Info ******************************/
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //_MAILBOX_API_H_
