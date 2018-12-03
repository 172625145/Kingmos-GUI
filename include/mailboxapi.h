/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

#ifndef _MAILBOX_API_H_
#define _MAILBOX_API_H_
#define NOT_AUTO_CLOSE_NET //打开这个开关，下载完不会关掉网络

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
#define TMAIL 1  //打开这个定义，将打邮箱应用程序里的调试信息
#define TME 1    //打开调试信息 (邮箱驱动部分）
#else
#define TME 0
#define TMAIL 0
#endif

#define szReceiveBox     TEXT("\\手机\\系统数据\\email\\Receive\\")
#define szReceiveBoxs    TEXT("\\手机\\系统数据\\email\\Receive")
#define szSentBox        TEXT("\\手机\\系统数据\\email\\Send\\")
#define szSentBoxs       TEXT("\\手机\\系统数据\\email\\Send")
#define szDraftBox       TEXT("\\手机\\系统数据\\email\\Draft\\")
#define szDraftBoxs      TEXT("\\手机\\系统数据\\email\\Draft")
#define szEmailRoot      TEXT("\\手机\\系统数据\\")
#define szEmailRoots     TEXT("\\手机\\系统数据")
#define szEmaildir       TEXT("\\手机\\系统数据\\email\\")
#define szEmaildirs      TEXT("\\手机\\系统数据\\email")
#define szEmailCfgDir    TEXT("\\手机\\系统数据\\email\\config")
#define szEmailCfgTempDir	TEXT("\\手机\\系统数据\\email\\config\\mlg")

//在emailcomm.c中用该宏是一个6临时文件的文件名，当以参数形式 新建一个邮件时，以该文件名传递，在
//一读出必要的信息后，必须马上删除该文件。
#define szECTempFileName  TEXT("\\手机\\系统数据\\email\\config\\newtemp.dxt")
#define szECTempDirectory TEXT("\\手机\\系统数据\\email\\config\\mlg")
#define szTempEmailNameInMApi  TEXT("\\手机\\系统数据\\email\\tempemail.non")//在mailboxapi.c
/******************************在mailconfig.c ***********************/
#define szMlcfgTempIDTable TEXT("\\手机\\系统数据\\Email\\config\\tempMesID.dat")    //用于判断当前油箱ID列表与已经DOWN下的邮件ID比较   
#define szMlcfgExistIDTable TEXT("\\手机\\系统数据\\Email\\config\\Receive.dat")     //保存现有邮件的ID列表的文件
#define szMlcfgConfigName  TEXT("\\手机\\系统数据\\Email\\EKeyCon.ems")
/******************************************************/


void SetCurrentError(int error);
int GetCurrentError(void); 
//列出POP3模块的数据结构和函数列表
/*****************************   Pop3 Server *****************************/
//typedef struct _Pop3Socket Pop3Socket,*PPop3Socket ;
typedef struct Pop3Socket {
        LPTSTR   m_Pop3IPAddr ;         //Pop3 Server IP地址
		int      m_Port ;
		LPTSTR   m_UserName ;       //Pop3 User 帐号
		LPTSTR   m_Password;        //密码
		LPTSTR   m_Dir;             //邮件下载定位的目录，一般是收件箱
		int   *  m_pNewIndex;       //一个新邮件的Index列表
		int      m_iNewCnt ;        //新邮件的总数
		int      m_BoxTotal;        //服务器中的邮件总数
        BOOL     m_bDownAllOrNew;   //下载新邮件还是所有邮件
#define SIZE_NOLIMIT 0
#define SIZE_200K  1
#define SIZE_500K  2
#define SIZE_1000K 3
		UINT     m_SizeLimit;
		BOOL     m_bCopyAndLoad ;  //下载时保留一份在服务器中
        SOCKET   m_Socket;
}Pop3Socket,*PPop3Socket ;

BOOL InitializePop3Socket( PPop3Socket pPop3 ) ;
BOOL DeInitializePop3Socket( PPop3Socket pPop3 );

BOOL ConnectServerAndDown( PPop3Socket pPop3 ,int * iMax);

BOOL DownLoadMail( PPop3Socket pPop3, int iIndex );
BOOL DownLoadAllMails( PPop3Socket pPop3 ); 
BOOL DownLoadNewMails( PPop3Socket pPop3 );      
/*****************************   Pop3 Server *****************************/

//列出SMTP模块的数据结构和函数列表
/*****************************  Smtp Server *****************************/
typedef struct  _AttachFile {
       CHAR *  *ppFileList ; //附件列表
	   int     iCnt ;       //附件个数
}AttachFile,* PAttachFile;

typedef struct ToAddress 
{
       CHAR * szToAddress ;  
       BOOL   bRet        ;
}ToAddress ,*PToAddress;

typedef struct ToList 
{
      ToAddress * ppTlst ; //发送地址的列表
	  int       iToCnt ; //发送地址的总数
}ToList ,* PToList ;

typedef struct  SmtpSocket {
       LPTSTR   m_SmtpIPAddr ;//SMTP 服务器地址
	   int      m_Port ;
       LPTSTR   m_UserName ;       //Smtp User 帐号
	   LPTSTR   m_Password;        //密码
	   LPTSTR   m_From       ;//邮件的来源
	   LPTSTR   m_Reply      ;//回复地址
	   ToList   m_pToList    ;//邮件的去处
	   LPTSTR   m_Data       ;//发邮件的时间
	   LPTSTR   m_Cc         ;//抄送
	   LPTSTR   m_Subject    ;//主题
	   LPTSTR   m_Body       ;//内容
	   int      m_BodyType   ;//0: means no Body. 1:means Text Body.2:means Htm body.
	   BOOL     m_bHasAttach ;//有无附件
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
