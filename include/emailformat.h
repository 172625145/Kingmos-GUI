/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
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

//���븽��ʱҪ���������ʼ��ķֽ���ȷ�ϣ����Ҫ�����ĵ���Ϣ��boundary

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

/*˵�����ڶ�PART���֣��ֽ��ԭ���ǣ�
      ��1��ֻ��һ�����֣���û�и�����ֻ���ı�����ʱ��ֻ��boundary1����Ч�ģ�ͨ����
	       �ʼ�ͷ���ֻ���˵�������û�з���multipart��˵������ֻ��һ��boundary��ԭ��������
	  ��2���ж�����ֵ�ʱ��ͨ�����ʼ�ͷ���ֻῴ��bouandary1 ������������£�boundary1ͨ��ֻ
	       ����˵��boundary2����������boundary2 ��˵���лῴ��boundary2��������������£�
		   bouandary1�Ѿ�ʧЧ������ͨ������������boundary2 ȡ��boundary1,�ڽ����ʼ���͸�����
		   �жϡ�����������淶��һ�㲻����͡�
		   �������ʼ��ж�����ֵ�ʱ�����䵱�ʼ��庬�и��������ʼ��庬��һЩ�ļ�����ʾʱ��
		   ��HTML�ʼ��͸���ͬʱ���ڵ�ʱ�򣬾���Ҫboundary3,��ʱ��ԭ���ǣ�������������е��ʼ��弰�丽���󣬲�
		   ���������ʼ��ĸ�����������ʼ�����ʾʱ��Ҫ����Ը�������������������ȷ���Ƿ�Ҫ����ʾ���ʼ���ĸ���
  
  ����ʼ��ı����˵����ͨ�������Ϸ�ʽ���ж϶ಿ�ֵĽ���ԭ��
      �ҵ�ԭ�����ʼ�ͷ����ͨ�����ж������ʼ��ķֿ����˵�������û���ҵ�multipart�������
	       ���ʼ���һ��ֻ�����ʼ��壬�������ʼ������������Ĳ��֡�
	 ��1������multipart ����PART��ԭ����͡�
	 ��2������Text��һ���ǽ����е��ʼ��岿�ְ��ʼ�ͷ�ı��뷽ʽ���ͳ���
	 ��3��û���ҵ���������˵������Ŀǰ�İ취�ǵ�һ�ֲ�֪���ķ�ʽ��������û���ʼ��塣
          
*/
typedef struct EmailFormat 
{
	BOOL       bSent; //û�з��͵ı�־
	LPTSTR     From;
	LPTSTR     ReplyTo;
	LPTSTR     To;
	LPTSTR     Cc;
	LPTSTR     Date;
	LPTSTR     Subject;
	int        DeType;
	LPTSTR     boundary1;//�ڶ�PART���֣�һ����Ҫ���boundary ���ֽ�
	int        emailtype;   //���ʼ�ͷ�еõ����ʼ�����    
    
	EmailAttach **Attach;//This member is link of the attach.
	LPTSTR       Body;         //�ʼ���
	int          BodyType;     //Body �ж������ͣ��������ı���HTML����
	                       //0 :��ʾû���ʼ���
	                       //1: ��ʾ��plain�ı���
	                       //2: ��ʾ��HTML��
	                       //3: ��ʾ�ǷǱ�׼���ͣ���IE֧�ֵ�ͼƬ�ȣ�������Ҫ�ַ�����ʾ,��Ҫ�ϸ����Ƶ�
	int          AttachCount;     //��ǰ�ĸ�����Ŀ
	int          MaxCount;        //˵��������װ��������
	LPTSTR       boundary2;     //body�ķֽ紮
	LPTSTR       boundary3; //���body��Ϊ��δ��뱾����Ҫboudanry���綨�Ļ���Ҫ��ʹ�����Ӷ�
	                            //�������HTML��ʽ�ı��븽��һ���ͳ�ȥ�Ļ�����Ҫ���ͳ���boundary2����
	                            //�ַ����߽�
#define  EMAIL_PARSE_BODY       1
#define  EMAIL_PARSE_BODY_IMAGE 2
#define  EMAIL_PARSE_ATTACHMEN  3
	UINT         uiParseState;  //��ʾ����״̬
	                       //EMAIL_PARSE_BODY:����BODY
	                       //EMAIL_PARSE_BODY_IMAGE:����body�ĸ����ļ� ,��Ҫ���html�ļ�
	                       //EMAIL_PARSE_ATTACHMEN:����attachment.
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
