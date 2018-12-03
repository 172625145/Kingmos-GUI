/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

#ifndef _EMAILFORMAT_H_
#define _EMAILFORMAT_H_


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */  

#define MM_DOWNLOAD (WM_USER+101)

typedef struct _EmailPart {
        CHAR type[20];
		CHAR subtype[20];
		CHAR encoding[20];
		CHAR disposing[20];
		BOOL param;
		CHAR name[40];
		int blank;
}EmailPart,*PEmailPart;

//解码附件时要根据整个邮件的分界来确认，因此要保留的的信息有boundary

typedef struct _EmailAttach
{
        LPTSTR    Name;
		int       cbyte;
		int       coding;
		int       pos;
#define MAIL_ATTACH 0
#define BODY_ATTACH 1
		UINT      type;
}EmailAttach,*PEmailAttach ;

/*说明：在多PART部分，分界的原则是：
      例1：只有一个部分，如没有附件，只有文本内容时，只有boundary1是有效的，通常在
	       邮件头部分会有说明，如果没有发现multipart的说明，则只按一个boundary的原则来解析
	  例2：有多个部分的时候，通常在邮件头部分会看到bouandary1 ，在这种情况下，boundary1通常只
	       用来说明boundary2的声明，在boundary2 的说明中会看到boundary2串，在这种情况下，
		   bouandary1已经失效，我们通常的做法是用boundary2 取代boundary1,在进行邮件体和附件的
		   判断。遇到不满足规范的一般不予解释。
		   附：在邮件有多个部分的时候，尤其当邮件体含有附件，即邮件体含有一些文件以显示时：
		   如HTML邮件和附件同时存在的时候，就需要boundary3,此时的原则是：当解释完成所有的邮件体及其附件后，才
		   解释整个邮件的附件。因此在邮件的显示时，要求针对附件的类型来做处理。既确定是否要求显示出邮件体的附件
  
  针对邮件的编码的说明：通常按以上方式来判断多部分的解释原则
      我的原则：在邮件头部分通常会有对整个邮件的分块进行说明，如果没有找到multipart，则表明
	       该邮件是一个只含有邮件体，不存在邮件附件等其他的部分。
	 例1：发现multipart 按多PART的原则解释。
	 例2：发现Text，一般是将所有的邮件体部分按邮件头的编码方式解释出来
	 例3：没有找到以上两种说明，我目前的办法是当一种不知道的方式来处理，即没有邮件体。
          
*/
typedef struct EmailFormat 
{
	BOOL       bSent; //没有发送的标志
	LPTSTR     From;
	LPTSTR     ReplyTo;
	LPTSTR     To;
	LPTSTR     Cc;
	LPTSTR     Date;
	LPTSTR     Subject;
	int        DeType;
	LPTSTR     boundary1;//在多PART部分，一定需要多个boundary 来分界
	int        emailtype;   //从邮件头中得到的邮件类型    
    
	EmailAttach **Attach;//This member is link of the attach.
	LPTSTR       Body;         //邮件体
	int          BodyType;     //Body 有多种类型，可能有文本和HTML类型
	                       //0 :表示没有邮件体
	                       //1: 表示是plain文本型
	                       //2: 表示是HTML型
	                       //3: 表示是非标准类型，如IE支持的图片等，不在需要字符串显示,需要严格限制的
	int          AttachCount;     //当前的附件数目
	int          MaxCount;        //说明最多可以装几个附件
	LPTSTR       boundary2;     //body的分界串
	LPTSTR       boundary3; //如果body作为多段代码本身需要boudanry来界定的话，要求使用其子段
	                            //如果当将HTML格式文本与附件一起发送出去的话。需要解释出波boundary2的子
	                            //字符串边界
#define  EMAIL_PARSE_BODY       1
#define  EMAIL_PARSE_BODY_IMAGE 2
#define  EMAIL_PARSE_ATTACHMEN  3
	UINT         uiParseState;  //表示解释状态
	                       //EMAIL_PARSE_BODY:解释BODY
	                       //EMAIL_PARSE_BODY_IMAGE:解释body的附属文件 ,主要针对html文件
	                       //EMAIL_PARSE_ATTACHMEN:解释attachment.
}EmailFormat, *PEmailFormat;

BOOL  InitEmailFormat( PEmailFormat  pMail );
void  DeInitEmailFormat ( PEmailFormat  pMail );
BOOL  ParseEmail( LPTSTR szFileName, PEmailFormat  pMail );
BOOL SaveAttachAs( PEmailFormat  pMail , int index, LPTSTR lpEmailName , LPTSTR lpAttachName );

BOOL EmailTimeConvert(LPTSTR lpDest,LPTSTR lpSource);
void ConvertEmailTime( LPTSTR lpDest, LPSYSTEMTIME   lst );
BOOL GetEmailHeaderInfo(LPTSTR szFileName, PEmailFormat  pMail);
void GetFileNameEx( LPTSTR lpPath, LPTSTR lpName,int * piType );
BOOL ReadVirtualLine2(HANDLE hFile,LPTSTR szALine,int * pnLineLength,int * pFilePos);
int StrPos(CHAR * szLine,CHAR ch);
BOOL FindStringBeforeCh(LPTSTR lpString,CHAR key,LPTSTR szKey);
void TrimString(LPTSTR szLine);
BOOL ReadVirtualLine(HANDLE hFile,CHAR *Aline,int iCurPos);
void TrimLines2(CHAR *szLine,int *piPos);
BOOL CheckValidEmail( LPTSTR szTo );
void ParseString (CHAR * szSource,CHAR *szDest);
void TrimLines(LPTSTR szLine,int * pPos);
//ap call
BOOL SendEmailTo( PEmailFormat  pEmailFormat );
void OpenEmail( CHAR *szTempFile );
BOOL ReadTempFile(PEmailFormat pMail,CHAR * szFileName);
BOOL  CreateAttachList( HWND  hListView, DWORD dwStatus );
void InsertListViewItem(HWND hListView,short iItem, short itemSub,
							   CHAR* lpItem,short iImage );
void GetTimeStr(CHAR *szStr);
BOOL FindEmailAddressFromStr( LPCTSTR szSource ,LPTSTR szDest );
BOOL IsDraftBox( LPCTSTR szFileName );
BOOL Email_ClearTempDirectory();
void TrimBlankStr( CHAR * lpStr );
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif //_EMAILFORMAT_H_
