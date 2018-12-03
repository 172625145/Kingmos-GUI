/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

/**************************************************
�ļ�˵����Email API ����˵����ͨ����װ��ʹʹ���߿��Լ򵥵õ��ã�������
          ����POP3��SMTP ���йص�Э�飬���е�Э�����ڲ�����ͬʱ�ʼ��Ľ���
		  Ҳֻ�����������ڲ���Ŀ¼�ڡ������ǵ��ʼ������������ֻ���ֻ����
		  ���û�ʹ�ã�����Ƕ��ڵ�����ʹ�ã������֧�ֶ���û���
�汾�ţ�1.0.0
����ʱ�ڣ�2003-3-12
���ߣ�������,�ļ���
�޸ļ�¼��
   1:���������غ�ɾ���������ϵ��ʼ����ܣ������ʼ������޶�Ҳ���ȫ���ص�
   ���ʼ�����ɾ������(2004-12-22)
   2:���������ѷ��ʼ����⣬Ĭ�ϱ���5�ݣ�(2004-12-23)
   3:��һ�����⣺��DownLoadMail����ParseRetr���ڵ��ʼ�����1Mʱ��ȡ������
     �Ա��������������⡣�����������ƴ�Сʱֻ�����ʼ�ͷʱҲ��ȡ���ˣ�
	 ������ȥ��ParseRetr��1M�����ƣ�����ǰ���MAXSIZE_MEGA����ʱ��������
	 ����û�м��������ö�������1M�(2005-04-06)
**************************************************/
#include <ewindows.h>
//#include <socket.h>
#include <MailBoxApi.h>
#include <EmailFormat.h>
#include <BaseQuoted.h>
#include "EmailFlags.h"
#include "endalign.h"
/********************************  ȫ�ֱ����� *************************/
#define BOUNDARY         TEXT("--NextPart_021679a")
#define MAX_BYTE 57  //�����ʼ�Э�飬��Base64������ʼ���ÿһ�в��ܳ���76���ֽڡ�
#define escapeTIME 4 //ÿ�δ�������ĵ�ʱ�����ֵ
//#define SENTMAILNAME     TEXT("\\systgem\\email\\send\\sendmlg")

#define MAXSIZE_MEGA //�������ߴ�1M���ʼ��п�����ȫ����

static int CurErr;
static int g_iSizeLimit = 200;
static BOOL *EmailLimitTable = NULL;
static int iLimitNum =0;
static int * CurMax;//ָ��ǰ�������ص��ʼ���
//*************************************************{
//jms add function 
BOOL fnIfServerRespondEnd(const char * szStr);
static int fnReadFromSock(SOCKET s,char FAR *buf,int len,int flags);
static int fnSendToSock(SOCKET s,char FAR *buf,int len,int flags);


//*************************************************}
void SetCurrentError(int error)
{
	CurErr=error;
}
int GetCurrentError(void)
{ 
	return CurErr;
}
/********************************  ȫ�ֱ����� *************************/
BOOL DelServerMailBackup( PPop3Socket pPop3, int iIndex );
BOOL GetRestrictTable(LPTSTR szBuf );
/*************************************************
������static BOOL Email_FileExist( LPCTSTR lpName )
������
IN    lpName : ��Ҫ�жϵ��ʼ��ļ���
����ֵ��  TRUE�� �ļ�����
          FALSE���ļ�������
�����������ж��ļ����ڷ�
************************************************/
static BOOL Email_FileExist( LPCTSTR szFile )
{
	HANDLE h;
	
     if ( (h=CreateFile(szFile,
            GENERIC_READ,
		    FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		    OPEN_EXISTING, NULL, 0)) == INVALID_HANDLE_VALUE )
		return FALSE;
	 else
	 {
         CloseHandle( h );
		 return TRUE; 
	 }
}

/*************************************************
������int GetSaveTempEmailName()
������

����ֵ��������Ҫ�������ʱ�ļ������
�������������Ҫ�����ļ���ʱ�ļ�������Ӧ�ڿ���������ʼ����ã�����
���ã�    �����ѷ��ʼ�ʱʹ��
************************************************/
static int GetSaveTempEmailName()
{
	CONFIGINFO Config;
	int    iSaveNum;
	int    i ;

	CHAR szFile[256];
	InitEmailConfig( &Config );
	GetEmailConfig( &Config );
	DeInitEmailConfig( &Config );

	iSaveNum = Config.iSaveNum;
	if ( iSaveNum == 0 ) 
		iSaveNum = 1000;//if it doesn't support Saving Function ,just set a virtual value.
	for ( i = 1; i<= iSaveNum ;i++ ){
		sprintf( szFile, TEXT("%s8888o%d.mlg"),szSentBox,i);
        if ( Email_FileExist( szFile ) )
		{
			continue;
		}
		else
		{
			break;
		}
	}
	if ( i >iSaveNum )
	{
		i = iSaveNum+1;
		sprintf( szFile ,TEXT("%s8888o%d.mlg"),szSentBox,1 );
		if ( Email_FileExist( szFile ) )
			DeleteFile( szFile );
	}
	else
	{
		sprintf( szFile ,TEXT("%s8888o%d.mlg"),szSentBox,i+1);
		if ( Email_FileExist( szFile ) )
			DeleteFile( szFile );
	}
    
	return i;
}

/*************************************************
������int ParseRetr( LPTSTR szText )
������
IN    szText    -Retr ����ʱ�����ܵ��ַ���
����ֵ��- �����ڵ���0ʱ�ɹ�������ֵ���ʼ��Ĵ�С
        -1 - �����Խ���       
��������������Retrʱ,����Ҫ�󲻽��ܴ���1M ���ʼ��������������Ҫ���û��ṩ
          ����һ�����ʼ�����Ϣ����Ȼ��������.
���ã�    ���ʼ�ʱʹ��
************************************************/
int ParseRetr( LPTSTR szText )
{
	CHAR szTemp[10];
	int i ,j ;
    CHAR ch ;
	memset( szTemp, 0, 10);
	strncpy( szTemp, szText, 3);
    
	if (stricmp( szTemp, TEXT("+OK") )!= 0 )
		return -1; 
	i = 4 ; j = 0;
	memset ( szTemp, 0, 10 );
    while ((ch = szText[i++]) && ch !=' ' && j< 8 && ch != 13 && ch != 10 )
	{
      szTemp[j++] = ch ;
	}  
	// ͨ���е�ISP���ԣ������ڻ�Ӧ�ĺ���û�д�С�趨�����в����жϸ��ʼ��Ĵ�С���
	//�����ڽ��ܣ�
	if (j == 0 ){
	    RETAILMSG(TME,("NO SIZE interpret"));
		return 0;
	}
	//������û�д���ã���Ϊ���û�а����е��������������Ļ�������ܻ��������������᲻���������������أ�
	return atoi(szTemp);
/*
	if ((i = atoi( szTemp))< 1024*1536  && i>40 )
		return TRUE ;
	else
		return FALSE;
		*/
}

/*************************************************
������BOOL ParseRecv( LPTSTR szText )
������
IN    szText    -��Ҫ���͵��ַ���
����ֵ��TRUE- �յ�+OK��Ϣ����Ч����
        FALSE-������ǰһ�ε�����
�������������ʼ�Э�������������Ӧ��Ϣ
���ã�    ÿһ������󣬶���Ҫ�ж��Ƿ���һ����Ч������
************************************************/
BOOL ParseRecv( LPTSTR szText )
{
    CHAR szTemp[6];
	memset( szTemp,0,6);
	strncpy( szTemp,szText,3);

	if (stricmp( szTemp,TEXT("+OK"))==0)
	{
		return TRUE;
	}
    SetCurrentError( UNKOWN_SOCKET_ERROR );
	return FALSE;
}
/*************************************************
������BOOL DoneCommand( SOCKET  socket )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
����ֵ��TRUE-�رճɹ�
       
������������ɲ�����Ҫ��SERVER���͹ر�����
���ã�
************************************************/
BOOL DoneCommand( SOCKET  socket )
{
     CHAR szBuf[100];
     int iBufLen;
	 memset( szBuf,0,100);
    
	 sprintf( szBuf,TEXT("QUIT%c%c"), 13, 10);
	 iBufLen = strlen( szBuf );
	 fnSendToSock(socket, szBuf, iBufLen, 0);

     if ( socket != INVALID_SOCKET )
	 {
		 int err;
		 err = closesocket( socket );
		 err = GetLastError();
	 }
	 socket = INVALID_SOCKET ;
	 return TRUE;
}

/*************************************************
������static SOCKET TCPClient(CHAR* szDest,int iPort)
������
IN  szDest    -׼�������Ŀͻ���ʱ���Է��������ĵ�ַ
IN  iPort     -�������ӵĶ˿ں�
����ֵ��SOCKET �������޵�SOCKET ���
	      
�������������ݷ����ַ �Ͷ˿ںţ����������������
���ã��κ�SOCKET ��������Ҫ���Ƚ�������
************************************************/
#ifdef HOSTNAME_WHL  //added by xyg_2004_08_30
static SOCKET TCPClient(CHAR* szDest,int iPort)
{
	SOCKADDR_IN dest_sin;
	SOCKET Client;
	
	int     iLen,i =0;
	CHAR szIP[256];
	CHAR *szBuf;
	CHAR *szTemp;
	
	HOSTENT *h;
    if ( szDest == NULL )
	{
        SetCurrentError( ERROR_NOT_CONFIG );		
		return INVALID_SOCKET;
	}
	Client = socket (AF_INET,SOCK_STREAM,0);
	
	if (  Client ==INVALID_SOCKET)
	{
		SetCurrentError( SERVER_NOT_READY );		
		return INVALID_SOCKET;
	}// SOCKADDR_IN dest_sin;
	//������Ҫ���ǽ�NAMEת��IP��ַ�����IP�Ļ�����
    memset( szIP, 0,256 );
	szBuf = malloc( 256 );
	memset( szBuf,0,256 );
	iLen = 0;
	//HeapValidate( GetProcessHeap(),0,0 );
	RETAILMSG(TME,("Before gethostbyname \r\n"));
#ifdef KINGMOS  //EML_WIN32	 modified by xyg_2004_08_30
    h = gethostbynameex( szDest ,szBuf,(DWORD *)(&iLen)); 
#else
	h = gethostbyname( szDest); 
#endif	
	//HeapValidate( GetProcessHeap(),0,0 );
	RETAILMSG(TME,("After gethostbyname \r\n"));
	if ( h==NULL )
	{
		if ( iLen== 0 ){
			SetCurrentError( INVALID_ADDRESS );
			free( szBuf );
			return INVALID_SOCKET;
		}
        free( szBuf );
		//HeapValidate( GetProcessHeap(),0,0 );
		szBuf = malloc( iLen +20);
		memset( szBuf, 0, iLen+20);
		//HeapValidate( GetProcessHeap(),0,0 );
		RETAILMSG(TME,("Before gethostbyname1 \r\n"));
#ifdef KINGMOS  //EML_WIN32	 modified by xyg_2004_08_30
		h = gethostbynameex( szDest,szBuf,(DWORD*)(&iLen) );
#else
		h = gethostbyname( szDest );
#endif
		//HeapValidate( GetProcessHeap(),0,0 );
		RETAILMSG(TME,("After gethostbyname 1\r\n"));
		if ( h == NULL )
		{
            SetCurrentError( INVALID_ADDRESS );
			free( szBuf );
			return INVALID_SOCKET;
		}
	}
	//�����ж��������Host����ν���ʶ����趨һ����ȷ������??
	//�Ƿ���Ҫ���ж����
    for (;;){
		szTemp = h->h_addr_list[i++];
		if ( szTemp == NULL )
		{
			SetCurrentError( INVALID_ADDRESS );
			free( szBuf );
			return INVALID_SOCKET;
		}
		sprintf( szIP,TEXT("%d.%d.%d.%d"),(unsigned char)szTemp[0],(unsigned char)szTemp[1],(unsigned char)szTemp[2],(unsigned char)szTemp[3]);
		
		
		dest_sin.sin_addr.s_addr =inet_addr ( szIP);
		
		//dest_sin.sin_addr.s_addr =inet_addr ( szDest );
		dest_sin.sin_family = AF_INET;
		dest_sin.sin_port = htons( iPort );
		RETAILMSG(TME,("connecting.... \r\n"));
		if (connect(Client, (struct sockaddr FAR *)&dest_sin, sizeof(dest_sin))!=0)
		{		
			RETAILMSG(TME,("connect fail.. \r\n"));
			SetCurrentError(SERVER_LINK_ERROR);
			closesocket(Client);
			free( szBuf );
			return INVALID_SOCKET;
		}
		else
			break; //got it,out
	}
	free( szBuf );
	return Client;	
}
#else //added by xyg_2004_08_30
#define CNT_IP_MAIL 10
static SOCKET TCPClient(CHAR* szDest,int iPort)
{
	SOCKADDR_IN		dest_sin;
	SOCKET			Client;
	int				i;
	HOSTENT*		pHostInfo;
#ifdef KINGMOS  //modified by xyg_2004_08_30
	BYTE			bBufHost[sizeof(HOSTENT)+CNT_IP_MAIL*sizeof(DWORD)];
#endif
	DWORD*			pdwIP;

	//
    if ( szDest == NULL )
	{
        SetCurrentError( ERROR_NOT_CONFIG );		
		return INVALID_SOCKET;
	}
	RETAILMSG(TME,("Before socket \r\n"));
	Client = socket (AF_INET,SOCK_STREAM,0);
	if (  Client ==INVALID_SOCKET)
	{
		SetCurrentError( SERVER_NOT_READY );		
		return INVALID_SOCKET;
	}// SOCKADDR_IN dest_sin;
	//������Ҫ���ǽ�NAMEת��IP��ַ�����IP�Ļ�����
	RETAILMSG(TME,("start gethostbyname \r\n"));
#ifdef KINGMOS  //EML_WIN32	 modified by xyg_2004_08_30
    pHostInfo = gethostbynameex( szDest ,bBufHost,sizeof(bBufHost)); 
    RETAILMSG(TME,("............................................. \r\n"));
#else
	pHostInfo = gethostbyname( szDest); 
#endif	
	RETAILMSG(TME,("end gethostbyname \r\n"));
	//�����ж��������Host����ν���ʶ����趨һ����ȷ������??
	//�Ƿ���Ҫ���ж����
	if (!pHostInfo)
	{
		RETAILMSG(TME,("gethostbyname fail... \r\n"));
		return INVALID_SOCKET;
	}

	for( i=0; i<CNT_IP_MAIL; i++ )
	{
		pdwIP = (DWORD*)(pHostInfo->h_addr_list[i]);
		if( pdwIP==NULL )
		{
			SetCurrentError( INVALID_ADDRESS );
			return INVALID_SOCKET;
		}
		dest_sin.sin_addr.s_addr = *pdwIP;
		//dest_sin.sin_addr.s_addr =inet_addr ( szDest );
		dest_sin.sin_family = AF_INET;
		dest_sin.sin_port = htons( (WORD)iPort );
		RETAILMSG(TME,("connecting.... \r\n"));
		if (connect(Client, (struct sockaddr FAR *)&dest_sin, sizeof(dest_sin))!=0)
		{		
			RETAILMSG(TME,("connect fail.. \r\n"));
			SetCurrentError(SERVER_LINK_ERROR);
			closesocket(Client);
			return INVALID_SOCKET;
		}
		else
			break; //got it,out
	}
	return Client;	
}
#endif  //added by xyg_2004_08_30

/*************************************************
������BOOL InitializePop3Socket( PPop3Socket pPop3 )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
����ֵ��TRUE  ��ʼ���ɹ�����
        FALSE ��ʼ��ʧ��
	      
������������ʼ��������Դ��Ҫ��������SERVER֮ǰ����Ҫ����Դ�����
���ã��ڽ����ʼ�֮ǰ����Ҫ���øú�������ʼ��
************************************************/
BOOL InitializePop3Socket( PPop3Socket pPop3 )
{
	 CONFIGINFO config;
     int        iLen ;
	 BOOL       bRet = FALSE;
	 InitEmailConfig( &config );
     GetEmailConfig(&config );
	 pPop3->m_Socket = INVALID_SOCKET ;
     if ( config.pop3server )
	 {
		iLen = strlen (config.pop3server) + 2;
	    pPop3->m_Pop3IPAddr = malloc ( iLen );
	    memset( pPop3->m_Pop3IPAddr ,0, iLen );
	    strcpy( pPop3->m_Pop3IPAddr ,config.pop3server );//���û��ת���Ļ�������Ҫ����ת����IP��ַ
     }
	 else
	 {
		 pPop3->m_Pop3IPAddr = NULL;
		 SetCurrentError ( ERROR_NOT_CONFIG );
		 //goto ErrorExit;
	 }
	 if (config.account)
	 {
		 iLen = strlen ( config.account ) + 2 ;
		 pPop3->m_UserName   = malloc ( iLen );
		 memset( pPop3->m_UserName ,0, iLen );
		 strcpy( pPop3->m_UserName, config.account );
	 }
	 else
		 pPop3->m_UserName = NULL;
	 if (config.password)
	 {
		 iLen = strlen( config.password )+2;
		 pPop3->m_Password   = malloc (iLen );
		 memset( pPop3->m_Password ,0, iLen );
		 strcpy( pPop3->m_Password, config.password );
	 }
	 else 
		 pPop3->m_Password = NULL;
     bRet = TRUE;
	 pPop3->m_Socket = INVALID_SOCKET ;

	 pPop3->m_bCopyAndLoad = config.bDel ;
	 switch( config.iSizeLimit )
	 {
	 case 1:
		 g_iSizeLimit = 200;
		 break;
	 case 2:
		 g_iSizeLimit = 500;
		 break;
#ifdef MAXSIZE_MEGA
     default:
          g_iSizeLimit = 1000;
#else
	 case 3:
		 g_iSizeLimit = 1000;
		 break;
	 default:
		 pPop3->m_SizeLimit = SIZE_NOLIMIT;
		 g_iSizeLimit = -1; 
#endif
	 }

	 pPop3->m_bDownAllOrNew = config.bDownAll;
	 pPop3->m_iNewCnt      = 0 ;
     pPop3->m_BoxTotal     = 0 ;
	 pPop3->m_pNewIndex    = NULL ;
	 pPop3->m_Port         = 110 ; //
	 DeInitEmailConfig( &config );	 
	 return bRet;
}
/*************************************************
������BOOL DeInitializePop3Socket( PPop3Socket pPop3 )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
����ֵ��TRUE-�ͷų�ʼ��ʱ�������Դ
	      
����������������ʼ����ܺ�Ҫ�ͷ���Դ
���ã��ڽ����ʼ�֮����Ҫ���øú������ͷ���Դ
************************************************/
BOOL DeInitializePop3Socket( PPop3Socket pPop3 )
{    
     if ( pPop3->m_Pop3IPAddr )
		 free( pPop3->m_Pop3IPAddr );
     if ( pPop3->m_UserName )
         free (pPop3->m_UserName );
	 if ( pPop3->m_Password )
          free (pPop3->m_Password );	 
	 if ( pPop3->m_pNewIndex )
		  free (pPop3->m_pNewIndex );
	 return TRUE ;
}

/*************************************************
������BOOL AuthorizationPass( PPop3Socket pPop3 )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
����ֵ��TRUE-���ӳɹ�,ͨ��Ȩ����֤
        FALSE - ����ʧ�ܣ��Ƿ��û�	      
��������������POP3������
���ã���ConnectServer ��ʹ��
************************************************/
BOOL AuthorizationPass( PPop3Socket pPop3 )
{
      CHAR szBuf[256];
      int iBufLen;
	  int i;
	  int iResult;
	  SOCKET  s = pPop3->m_Socket ;
	  
	  memset( szBuf,0,256);
      i = 0;
	  iBufLen = 254;
	  iResult = SUCCESS_SOCKET_ERROR;
	  RETAILMSG(TME,("!!!!!!!!!!��Ȩ��ʼrecv!\r\n"));
	  if (fnReadFromSock( s, szBuf, iBufLen, 0)==SOCKET_ERROR )
	  {
		  SetCurrentError( AUTH_PASS_ERROR );
		  RETAILMSG(TME,("!!!!!!!!!!��Ȩ��ʼʧ��!\r\n"));
	      return FALSE;
	  }
//	  RETAILMSG(TME,("!!!!!!!!!!��Ȩ��ʼ���!\r\n"));
	  RETAILMSG(TME,("!!!!!!!!!!recv=%s iBufLen = %d\r\n",szBuf,iBufLen));
	  if (!ParseRecv( szBuf ))
	  {
          SetCurrentError( AUTH_PASS_ERROR );
	      return FALSE;
	  }
	  //---------------Server Ready---------------------------
	  //transfer user to server . 
	  memset( szBuf, 0, 256); 
//	  RETAILMSG(TME,("!!!!!!!!!!����USER!\r\n"));
	  sprintf( szBuf, TEXT("USER %s%c%c"),pPop3->m_UserName,13,10);
	  if (fnSendToSock( s,szBuf,strlen( szBuf), 0)==SOCKET_ERROR)
	  {
          SetCurrentError( AUTH_PASS_ERROR );
		  return FALSE;
	  }
	  memset( szBuf, 0, 256);
//	  RETAILMSG(TME,("!!!!!!!!!!����userȷ��!\r\n"));
	  if (fnReadFromSock( s, szBuf, iBufLen, 0)==SOCKET_ERROR)
	  {
		  SetCurrentError ( AUTH_PASS_ERROR );
		  return FALSE;
	  } 
	  if (!ParseRecv( szBuf))
	  {
          SetCurrentError( AUTH_PASS_ERROR );
		  return FALSE;
	  }
	 //������֤
	  memset( szBuf, 0, 256); 
//	  RETAILMSG(TME,("!!!!!!!!!!����PASS!\r\n"));
	  sprintf(szBuf,TEXT("PASS %s%c%c"),pPop3->m_Password,13,10);
	  if (fnSendToSock( s,szBuf,strlen(szBuf),0)==SOCKET_ERROR)
	  {
          SetCurrentError( AUTH_PASS_ERROR );
		  return FALSE;
	  }
//      RETAILMSG(TME,("!!!!!!!!!!����passȷ��!\r\n"));
	  memset(szBuf,0,256);
	  if (fnReadFromSock( s, szBuf,iBufLen,0)==SOCKET_ERROR)
	  {
		  SetCurrentError( AUTH_PASS_ERROR );
		  return FALSE;
	  }      
	  if (!ParseRecv( szBuf ))
	  {
	      SetCurrentError( AUTH_PASS_ERROR );
		  return FALSE ;
	  }//-------------password pass------------------
	  return TRUE;
}

/*************************************************
������BOOL FindNewEmailNum(SOCKET s,int *num,int * pNewList)
������
IN     s    -��������Pop3�ʼ���SOCKET���
OUT    num  -�������ʼ��ĸ���
OUT    pNewList - �������ʼ���INDEX�б�
����ֵ��TRUE-�õ���ȷ����Ϣ
        FALSE -���ܵõ���ȷ����Ϣ
������������POP3 �������еõ���ȷ���ʼ������������ʼ���С
���ã��� GetServerEmailInfo ��ʹ��
ע�����ʼ�������̫��ʱ�����ܻᵼ���ڴ治�������Ҫôʹ�ö�̬����ķ�ʽ
    ���� ���ƶ������в����������Ƶķ���
************************************************/
BOOL FindNewEmailNum(SOCKET s,int *num,int * pNewList)
{
       int i,buflen , len;
	   CHAR *buf;
	   CHAR *MesID;
	   int   iMesIDLen =  1000;//���Գ���Ϊ1000

//	   DWORD iTimeDot;

	   buf = malloc( 1000 );
	   if ( buf == NULL )
		   return FALSE;
	   sprintf(buf,TEXT("%s %c%c"),TEXT("UIDL"),13,10);
	   buflen=strlen(buf);
	   //HeapValidate( GetProcessHeap(),0,0 );
	   RETAILMSG(TME,("before ############## uidl.... \r\n"));
       fnSendToSock(s,buf,buflen,0);
	   //HeapValidate( GetProcessHeap(),0,0 );
	   RETAILMSG(TME,("after ############## uidl.... \r\n"));
	   buflen=999;
	   MesID = (CHAR*)malloc( 1000 );
	   if ( MesID == NULL )
		   return FALSE;
	   memset( MesID , 0, 1000 ) ; 
	   memset( buf, 0, 1000 );
	   i=1;
	   len = 0 ;
	   //����ʼ����ʼ�ͷ�б�ͬʱ�õ����ʼ��ĸ�����
	   //HeapValidate( GetProcessHeap(),0,0 );
//	   iTimeDot = GetTickCount();
//       RETAILMSG(TME,("!!!!!!!!!Uidl--Now time= %d\r\n",iTimeDot));
	   RETAILMSG(TME,("Recv Uidl ############## ....uidl \r\n"));
       while ( fnReadFromSock( s, buf, 999, 0 )>0 )
	   {
		   //HeapValidate( GetProcessHeap(),0,0 );
            len = strlen (buf); 
			if ( (len +(int)(strlen(MesID))+1) >= iMesIDLen )
			{
				MesID = realloc( MesID, 2*iMesIDLen );
				MesID[iMesIDLen] = 0;
			    iMesIDLen = 2* iMesIDLen ;
			}
            strcat( MesID, buf);
			if ( len < 3 || (buf[len-3] == '.' && buf[len-2]==13 ) )
				break;
			memset( buf, 0,1000 );
			RETAILMSG(TME,("Get NewID ############## ....uidl \r\n"));
	   }
	   if ( MesID[0] == '+' ) //+OK existing.
	   {
          i = GetNewEmailInfo( MesID, pNewList );
		  *num=i; //�õ����� 
		  free( MesID );
	   }
	   else
	   {
		   free( MesID );
		   free( buf );
		   return FALSE;
	   }
       //��LIST �����п����жϳ�ÿһ���ʼ��Ĵ�С�������Ҫ���ݴ�С�������Ƿ���ܣ�
	   //�����ڴ˴���
	   sprintf(buf,TEXT("%s %c%c"),TEXT("LIST"),13,10);
	   
	   buflen=strlen(buf);	   
	   fnSendToSock(s,buf,buflen,0);
	   memset(buf,0,1000);
	   //�����ʼ����ݴ�С��ͬʱ�������Ʊ����������Ƶ��ʼ�
	   MesID = (CHAR*)malloc( 800 );
	   memset( MesID,0, 800 );
	   buflen = 1000;
	   if ( fnReadFromSock(s,MesID,798,0) >0)
	   {
		   RETAILMSG(TME,("!!!!!!!!!list recv 1= %s\r\n",MesID));
		   if ( ParseRecv( MesID ) ){
			   if(fnIfServerRespondEnd(MesID))
				   goto ListCMDOver;
			   memset( MesID , 0,800 );
			   RETAILMSG(TME,("!!!!!!!!!!list recv no only line!!!!!!!!!\r\n"));
			   while ( fnReadFromSock(s,MesID,798,0) >0 )
			   {
				   RETAILMSG(TME,("!!!!!!!!!list recv 2= %s\r\n",MesID));
				   if ( ( (int)strlen(MesID)+ (int)strlen( buf )  ) > (buflen-1)  )
				   {
					   CHAR * szTempStr ;
					   szTempStr = realloc( buf,2*buflen);
					   if ( szTempStr == NULL ){
						   free( MesID );
						   free( buf );
						   SetCurrentError( NOT_MEMORY_AFFORD );
						   return FALSE;
					   }
					   buf = szTempStr;
					   buf[buflen] = 0;
					   buflen *= 2;              
				   }
				   RETAILMSG(TME,("%s",MesID));
				   strcat( buf,MesID );
//				   ch = MesID[strlen(MesID)-3];
//				   if ( ch == '.' )
//					   break;
			       if(fnIfServerRespondEnd(MesID))
				          break;				   
				   memset(MesID,0,800);
			   }//while end
ListCMDOver:
			   GetRestrictTable( buf );
		   }//if ( Parse  end
	   }//if end
	   free( MesID );
	   //need check.
	   free( buf );
	   return TRUE;
}	   
/*************************************************
������BOOL JudgeStat(SOCKET s,int *num,int *cbyte)
������
IN s    -��������Pop3�ʼ���SOCKET���
IN/OUT *num -�ɹ�ʱ�������ʼ���
IN/OUT *cbyte -�ɹ�ʱ�������ʼ��ֽ���
����ֵ��TRUE-�õ���ȷ����Ϣ
        FALSE - ���ܵõ���ȷ����Ϣ
������������POP3 �������еõ���ȷ���ʼ������������ʼ���С
���ã���ConnectServer ��ʹ��
************************************************/
BOOL JudgeStat(SOCKET s,int *num,int *cbyte)
{
	   int i,buflen;
	   CHAR buf[100];
	   CHAR mailInfo[20];
	   CHAR ch;
	   //STAT����õ��ʼ����������ֽ���
	   //Ӧ���磺"+OK 3 12648"����������3�⣬�ܴ�СΪ12648�ֽ�
	   sprintf(buf,TEXT("%s%c%c"),TEXT("STAT"),13,10);
	   
	   i=0;
	   buflen=strlen(buf);
	   if (fnSendToSock(s,buf,buflen,0)==SOCKET_ERROR)
	   {
		  SetCurrentError(UNKOWN_SOCKET_ERROR);
		  return FALSE;
	   }
	   memset(buf,0,100);
	   buflen=99;
	   if (fnReadFromSock(s,buf,buflen,0)==SOCKET_ERROR)
	  {
		  SetCurrentError(UNKOWN_SOCKET_ERROR);
		  return FALSE;
	  }	   
	   
	   if (!ParseRecv(buf))
	   {
           SetCurrentError(PARSEMAIL_SOCKET_ERROR);
		   return FALSE;
	   }
	   i=4;
	   buflen=0;
       memset(mailInfo,0,20);
	   while ((ch=buf[i]) && ch!=0x20 && ch!=13 && i<20) 
	   {
		   mailInfo[buflen++]=ch;
		   i++;
	   }
	   if ( mailInfo[0] !='0' && (*num=atoi(mailInfo))==0 )
	   {
           SetCurrentError(PARSEMAIL_SOCKET_ERROR);
		   return FALSE;
	   }
	   i++;
	   buflen=0;
	   memset(mailInfo,0,20);
	   while((ch=buf[i]) && ch!=13 && ch!=0x20 && i<20)
	   {
		   mailInfo[buflen++]=ch;
		   i++;
	   }
	   *cbyte=atoi(mailInfo);
	   return TRUE;
}
/*************************************************
������BOOL GetServerEmailInfo( PPop3Socket pPop3 )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
����ֵ��TRUE- �����ȷ����Ϣ
        FALSE - ���ܵõ���ȷ����Ϣ      
��������������ʼ����������ʼ���Ϣ�����ʼ�ͷ���ʼ��б������ʼ���С���
���ã��ڽ����ʼ�ǰ��Ҫ��õ����ʼ�����Ϣ�ʹ�С�ȣ�USER����ֻ�������ʼ�
************************************************/
BOOL GetServerEmailInfo( PPop3Socket pPop3 )
{
	SOCKET s = pPop3->m_Socket ;
    int   iTotal = 0 ;
	int   iCBytes = 0 ;
	int   iNewList[100];
	int   iNewNum = 0 ;
	int   m ;

	//�õ����������ʼ����ʹ�С��
	if (!JudgeStat(s,&iTotal,&iCBytes))
	{
        SetCurrentError( FORMAT_SOCKET_ERROR );
		return FALSE ;
	}
	RETAILMSG(TME,("After JudgeStat ############## uidl.... \r\n"));
    pPop3->m_BoxTotal   = iTotal ;
    if ( iTotal == 0 )
	{
	    SetCurrentError( EMAIL_NOTEXIST );
		return FALSE ;
	}
	if ( !FindNewEmailNum( s, (int*)&iNewNum, (int*)&iNewList ))
	{
	    SetCurrentError( FORMAT_SOCKET_ERROR );
		return FALSE ;
    }
	RETAILMSG(TME,("after FindNewEmailNum ############## .... \r\n"));
	pPop3->m_iNewCnt  = iNewNum ;
	if ( !pPop3->m_bDownAllOrNew && iNewNum == 0 )
	{
		SetCurrentError( NEWMAIL_NOTEXIST );
		return FALSE;
	}
	pPop3->m_pNewIndex = (int * )malloc (sizeof(int)* (iNewNum+1 ));
	//�����Ƿ������memcpy�ķ�ʽ
	for ( m = 0 ;m<iNewNum ;m++ ){
	    RETAILMSG(TME,("mailList :%d \r\n",iNewList[m]));
		pPop3->m_pNewIndex[m] = iNewList[m];
	}
	return TRUE ;
}

/*************************************************
������BOOL ConnectServerAndDown( PPop3Socket pPop3 )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
IN/OUT Max  -��ʱ������ʾ�ռ���ֻ�ж��ٿռ�����ʱΪ�����ص��ʼ�������û������Ϊ-1   
����ֵ��TRUE-���ӳɹ�
        FALSE - ����ʧ��	      
��������������POP3�������Ի�ȡ�ʼ�
���ã�����Ҫ��SERVER�����ʼ�ʱ������Ҫ���øú������ӣ�ǰ��������
      ���������Ϣ����SOCKET �Ѿ���ʼ���ˡ�
************************************************/
BOOL ConnectServerAndDown( PPop3Socket pPop3 ,int * Max)
{
	 SendMessage( FindWindow(NULL,"�����ʼ�"),MM_DOWNLOAD,(WPARAM)0,(LPARAM)0 );
	 //HeapValidate( GetProcessHeap(),0,0 );
	 CurMax = Max;//qu add 1/17/2004
	 RETAILMSG(TME,("!!!!!!!!!!!!TcpClient start...\r\n"));
	 pPop3->m_Socket = TCPClient( pPop3->m_Pop3IPAddr ,pPop3->m_Port );
	 if (pPop3->m_Socket == INVALID_SOCKET )
	 {
         RETAILMSG(TME,("!!!!!!!!!!!TcpClient failed....\r\n"));
		 return FALSE ;
	 }
	 RETAILMSG(TME,("!!!!!!!!!!!!TcpClient success...\r\n"));
	 if( !AuthorizationPass( pPop3 ))
	 {
		 DoneCommand(pPop3->m_Socket);
		 return FALSE ;
	 }
	 RETAILMSG(TME,("!!!!!!!!!!!!AuthorizationPass success...\r\n"));
	 SendMessage( FindWindow(NULL,"�����ʼ�"),MM_DOWNLOAD,(WPARAM)1,(LPARAM)1 );
	 if (!GetServerEmailInfo( pPop3 ))
	 {
		 DoneCommand( pPop3->m_Socket );
		 RETAILMSG(TME,("!!!!!!!!!!!!GetServerEmailInfo fail!...\r\n"));
		 return FALSE ;
     }
	 RETAILMSG(TME,("!!!!!!!!!!!!!!!Begin Download emails"));
	 if ( pPop3->m_bDownAllOrNew )
		 DownLoadAllMails( pPop3 ) ; //�������е��ʼ�
	 else
         DownLoadNewMails( pPop3 );  //�������ʼ�
     DoneCommand( pPop3->m_Socket );
    
	 if ( EmailLimitTable != NULL )
		 free( EmailLimitTable );
	 EmailLimitTable = NULL;
	 return TRUE;
}

BOOL  LimitMail( int iIndex )
{
	int i = 0;
	if ( EmailLimitTable == NULL )
		return FALSE;
	while ( EmailLimitTable[i] && i< iIndex ){
		if ( EmailLimitTable[i++] == iIndex )
			return TRUE;
	}
    return FALSE;
}
/*************************************************
������BOOL DownLoadMail( PPop3Socket pPop3, int iIndex )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
IN iIndex   -׼�����ص��ʼ���Index ;
����ֵ��TRUE-���سɹ�
        FALSE- ����ʧ��
       
������������������Ҫ���ʼ�,���ĵ��ʼ��������������⣬ϣ����
���ã�
************************************************/
BOOL DownLoadMail( PPop3Socket pPop3, int iIndex )
{
	SOCKET s = pPop3->m_Socket ;
    HANDLE hFile;
    CHAR * szText;
    int n;
	DWORD dwWrite;
	CHAR szFileName[100];
	CHAR ch;
    int iBufLen;
	CHAR szBuf[100];
	BOOL bOver,bDel;
    CHAR   szTimeStr[10];
	
    RETAILMSG(TME,("!!!!!!!!!!���ص�%d����\r\n",iIndex));
	//�ڴ���Ҫ֪ͨ�ϲ�apĿǰ��״̬	
//    SendMessage( FindWindow(NULL,"�����ʼ�"),MM_DOWNLOAD,2,(LPARAM)iIndex );
//����Ϊ�����д�����ʼ���������ʾ����������ʵ���������������Ұ���һ�зŵ����������ȥ�ˡ�

	//�ж��Ƿ������������ش�С���ʼ�
	bDel = LimitMail(iIndex);
	if ( !bDel )
	    sprintf(szBuf,TEXT("%s %d%c%c"),TEXT("RETR"),iIndex,13,10);
	else
		sprintf( szBuf,TEXT("%s %d 1%c%c"),TEXT("TOP"),iIndex,13,10);
    
	iBufLen = strlen( szBuf );
	RETAILMSG(TME,("!!!!!!!!!!%s iBufLen = %d\r\n",szBuf,iBufLen));
	if (fnSendToSock( s, szBuf,iBufLen, 0)==SOCKET_ERROR)
	{
	   RETAILMSG(TME,("!!!!!!!!!!Send failed\r\n"));
       SetCurrentError(UNKOWN_SOCKET_ERROR);
	   return FALSE;
	}
	memset(szBuf,0,100);
	iBufLen=99;
	
    if (fnReadFromSock( s, szBuf,iBufLen, 0)==SOCKET_ERROR)
	{
		RETAILMSG(TME,("!!!!!!!!!!recv string failed\r\n"));
		SetCurrentError(UNKOWN_SOCKET_ERROR);
		return FALSE;
	}
	RETAILMSG(TME,("Parse string:%s",szBuf));
	if (ParseRetr( szBuf )<0)
	{
	   RETAILMSG(TME,("Parse Retr error!"));
	   SetCurrentError( UNKOWN_SOCKET_ERROR );
       return FALSE;
	}
    SendMessage( FindWindow(NULL,"�����ʼ�"),MM_DOWNLOAD,2,(LPARAM)iIndex );
	RETAILMSG(TME,("DOWN........... wait!"));
	szText=malloc(4098);
    iBufLen=4096;
    memset(szText,0,4098);
    //when move to esoft ,change the dir to esofe
	//  "\\system\\email\\receive\\filename.mlg"
	GetTimeStr(szTimeStr);

	//������������һ����־��ʶ���Ƿ������������ݣ�t����,f������.
	if(!bDel)
	  sprintf( szFileName,TEXT("%s%snt%d.mlg"),szReceiveBox,szTimeStr,iIndex);
	else
	  sprintf( szFileName,TEXT("%s%snf%d.mlg"),szReceiveBox,szTimeStr,iIndex);
    hFile = CreateFile(szFileName,
            GENERIC_WRITE,
		    FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		    CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);//if not existing ,create one.
    if (hFile == INVALID_HANDLE_VALUE )
	{
	   free( szText);
	   SetCurrentError( NOT_DISK_SPACE );
	   //SetCurrentError ( FILE_ERROR_EXCEPTION );
	   RETAILMSG(TME,(" FILE_ERROR_EXCEPTION gens "));
	   return FALSE;
	}
	bOver = FALSE;
//	while (fnReadFromSock( s,szText,iBufLen,0)>0 )
	while (1)
	{
		CHAR * pText ;

        if(fnReadFromSock( s,szText,iBufLen,0)==SOCKET_ERROR)
		{
			//��������ʱ���ִ���ɾ��������ݲ��������ʼ�
			free(szText);
			CloseHandle(hFile);
//			DeleteFile(szFileName);
//			SetCurrentError( READ_ERROR_RETRY );
			MessageBox(NULL,"���չ���,������!","���ʼ�",0);
			return FALSE;
		}
		iBufLen = strlen(szText);
		n = iBufLen - 5;
		pText = szText + n ;
		if (n>=0)
		{
            if ((ch=szText[n++])==13 && (ch=szText[n++])==10
				&& (ch=szText[n++])=='.' && (ch=szText[n++])==13
				&& (ch=szText[n++])==10)
			{		
				iBufLen = iBufLen - 5;
			    bOver = TRUE;
			}
		}
		else 
			break;
		WriteFile(hFile, szText,(DWORD)iBufLen, &dwWrite,NULL);
		
		iBufLen = strlen(szText);
	    if (bOver ==TRUE)
		{
			break; 
		}
		memset(szText,0,4098);
		iBufLen = 4096;
	}

	//�ʼ�������Ϊ���غ�����������������ʼ��Ĵ�С�������޶���ɾ����
	//�ʼ�
	if(pPop3->m_bCopyAndLoad)
	  if(!bDel)
	    DelServerMailBackup(pPop3,iIndex);

    CloseHandle(hFile);
    free(szText);      
	return TRUE;    
}

/*************************************************
������BOOL DownLoadAllMails( PPop3Socket pPop3 )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
����ֵ��TRUE-���سɹ�
        FALSE- ����ʧ��
       
������������������Ҫ���ʼ�
���ã�
************************************************/
BOOL DownLoadAllMails( PPop3Socket pPop3 )
{
     int i ,iNum;
	 BOOL bRet = TRUE;
	 int iLength = (sizeof(int) * (pPop3->m_BoxTotal+ 1)) ;
	 int *iTotalIDList = (int *)malloc ( iLength );
	 if(!iTotalIDList)
        return FALSE;
	 memset( iTotalIDList ,0, iLength );
	 iNum = min(*CurMax,pPop3->m_BoxTotal);
	 if(pPop3->m_BoxTotal > *CurMax)
		 *CurMax = -1;
	 else *CurMax = pPop3->m_BoxTotal;

	 RETAILMSG(TME,("all email num %d ",pPop3->m_BoxTotal ));
     for (i = 0 ;iNum ; i++ )
	 {
		 RETAILMSG(TME,("be DownLoading number %d  ",i ));
		 
		 if ( !DownLoadMail( pPop3, i+1 ))
		 {
			 //SetCurrentError ( UNKOWN_SOCKET_ERROR ) ;
			 bRet = FALSE;
			 break ;
		 }

		 iTotalIDList[i] = -1 ;//��ʾ���ʼ��Ѿ���down��
	 }
	 //�����ʼ�ͷ���б�
	 RefreshIDList( pPop3->m_BoxTotal, iTotalIDList ) ;
	 free( iTotalIDList );
	 RETAILMSG(TME,("Download over "));
     return bRet;
}

/*************************************************
������int DownLoadNewMails( PPop3Socket pPop3 )
������
IN pPop3    -��������Pop3�ʼ���SOCKET���
IN iIndex   -׼�����ص��ʼ���Index ;
����ֵ��TRUE-���سɹ�
        FALSE- ����ʧ��
       
�����������������ʼ���
���ã�
************************************************/
int DownLoadNewMails( PPop3Socket pPop3 )
{
	 int i,iNum ;
	 iNum = min(*CurMax,pPop3->m_iNewCnt);
	 if(pPop3->m_iNewCnt > *CurMax)
		 *CurMax = -1;
	 else *CurMax = pPop3->m_iNewCnt;

	 RETAILMSG(TME,("!!!!!!!!!!!!!��Ҫ����%d���ʼ�\r\n",iNum));
	 for (i = 0 ;i< iNum ; i++ )
	 {
		 RETAILMSG(TME,("!!!!!!!!!!!!!����%d���ʼ�\r\n",i));
		 if ( !DownLoadMail( pPop3, pPop3->m_pNewIndex[i] ))
		 {
			 SetCurrentError ( UNKOWN_SOCKET_ERROR ) ;
			 break ;
		 }
		 pPop3->m_pNewIndex[i] = -1 ;//��ʾ���ʼ��Ѿ���down��
	 }
	 //�����ʼ�ͷ���б�
	 RefreshIDList( pPop3->m_iNewCnt, pPop3->m_pNewIndex ) ;
     return pPop3->m_iNewCnt;	 
}

/*************************************************
������void ErrorConvertStr(int error,LPTSTR dest)
������
IN error    -�������
IN szDest   -���صĴ�����Ϣ
����ֵ��void
�������������ݴ�����룬���ش�����Ϣ
���ã�    ����������ʱ�����ش�����Ϣ�ַ���
************************************************/
void ErrorConvertStr(int error,LPTSTR szDest)
{
	   RETAILMSG(TME,("error :%d \r\n",error));
       switch (error)
	   {
	   case TO_NOT_KNOWN:
		   strcpy(szDest,TEXT("�������ݳ���"));
		   break;
	   case MESSAGE_NOT_EXIST:
		   strcpy(szDest,TEXT("ɾ���ļ�����"));
		   break;
       case SERVER_NOT_READY:
		   strcpy(szDest,TEXT("������û��׼��"));
		   break;
	   case SERVER_LINK_ERROR:
		   strcpy( szDest,TEXT("������񲻴���"));
		   break;
	   case AUTH_PASS_ERROR:
		   strcpy(szDest,TEXT("��Ȩû�б�ͨ��"));
		   break;
	   case UNKOWN_SOCKET_ERROR:
		   strcpy(szDest,TEXT("�쳣����"));
		   break;
	   case PARSEMAIL_SOCKET_ERROR:
		   strcpy(szDest,TEXT("�����ʼ�����"));
		   break;
	   case SERVER_TIME_OUT:
		   strcpy(szDest,TEXT("��������ʱ����"));
	       break;
	   case NOT_MEMORY_AFFORD:
		   strcpy( szDest,TEXT("û���㹻���ڴ�"));
		   break;
	   case NOT_DISK_SPACE:
		   strcpy( szDest, TEXT("���̿ռ䲻��") );
		   break;
	   case INVALID_EMAIL_ADDRESS:
		   strcpy( szDest ,TEXT("��Ч���ʼ���ַ"));
		   break;
	   case ERROR_CONFIG_SMTP:
		   strcpy(szDest, TEXT("��������������"));
		   break;
	   case ERROR_NOT_CONFIG:
		   strcpy( szDest, TEXT("���������ʼ��շ���"));
		   break;
	   case FILE_ERROR_EXCEPTION:
		   strcpy( szDest, TEXT("�ļ������쳣!"));
		   break;
	   case FORMAT_SOCKET_ERROR:
		   strcpy ( szDest, TEXT("����������ʽ"));
		   break;
	   case EMAIL_NOTEXIST:
		   strcpy ( szDest, TEXT("û���ʼ�!"));
		   break;
	   case NEWMAIL_NOTEXIST:
		   strcpy ( szDest, TEXT("û�����ʼ�����!"));
		   break;
	   case INVALID_ADDRESS:
           strcpy ( szDest, TEXT("��Ч�ķ���������"));
		   break;
	   case ADDRESS_NOT_ACCEPT:
		   strcpy( szDest,TEXT("���ύ��ȷ�ĵ�ַ"));
		   break;
	   case SERVER_NOT_SUPPORT:
		   strcpy( szDest, TEXT("û���ṩ�������"));
		   break;
	   case USER_INVALID:
		   strcpy( szDest, TEXT("��Ч�û���"));
		   break;
	   case ADDRESS_USER_INVALID:
		   strcpy( szDest,TEXT("�û���ַ������Ч"));
		   break;
	   case SMTP_USER_INVALID:
           strcpy( szDest, TEXT("��ЧSMTP�û���"));
		   break;
	   case SMTP_PASSWORD_INVALID:
           strcpy( szDest, TEXT("��ЧSMTP����"));
		   break;
	   case INVALID_VALUE_FILE:
		   strcpy( szDest, TEXT("�ļ�����ʧ��"));
		   break;
//	   case READ_ERROR_RETRY:
//		   strcpy(szDest,TEXT("���չ���,������!"));
		 
	   default:
		   strcpy( szDest, TEXT("�����ж�,������!"));
	   }
}

/*************************************************
������BOOL SocketStartUp(DWORD hiVersion,DWORD loVersion)
������
IN hiVersion    -��λ�汾��
IN loVersion    -��λ�汾��
����ֵ��void
������������ʼ��SOCKET������API WSAStartup����ʼ��
���ã�    ���е��й�SOCKET��TCP/IP�Ĳ�����������SOCKET ��ʼ��
************************************************/
BOOL SocketStartUp(DWORD hiVersion,DWORD loVersion)
{
    WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( loVersion,hiVersion );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return FALSE;
	}
	

	if ( LOBYTE( wsaData.wVersion ) != loVersion ||
        HIBYTE( wsaData.wVersion ) != hiVersion)
	{
		
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return FALSE; 
	}
	return TRUE;
}

/*************************************************
������void SocketCleanUp( )
������û��
����ֵ��void
�������������SOCKET�����󣬱����ͷ���Դ
���ã�    �򵥵ص���SOCKET API�������ͷ���Դ
************************************************/
void SocketCleanUp( )
{ 
	  WSACleanup( ) ;
}


/******************************** SMTP operation Function ****************************/
/*************************************************
������BOOL InitializeSmtpSocket( PSmtpSocket pSmtp )
������pSmtp       - ���ڷ����ʼ���SMTP���
����ֵ��BOOL    TRUE  - �õ���ȷ��������Ϣ
                FALSE - ���ܵõ���ȷ��������Ϣ
������������ʼ��SMTP������Ϣ������Դ
���ã�    �ڷ����ʼ�֮ǰ����������
************************************************/
BOOL InitializeSmtpSocket( PSmtpSocket pSmtp )
{
	 CONFIGINFO config;
     int     iLen ;
	
	 InitEmailConfig( &config );
     if (GetEmailConfig(&config ) == FALSE )
	 {
		 SetCurrentError ( ERROR_NOT_CONFIG );
		 DeInitEmailConfig(&config);
		 return FALSE ;
     }
	 pSmtp->m_Socket = INVALID_SOCKET ;
	 if ( !config.smtpserver)
	 {
	     SetCurrentError ( ERROR_CONFIG_SMTP );
		 pSmtp->m_SmtpIPAddr = NULL;
		 goto SmtpFail ;
	 }
	 else
	 {
         iLen = strlen (config.smtpserver) +2 ;
		 pSmtp->m_SmtpIPAddr = malloc ( iLen );
		 memset(pSmtp->m_SmtpIPAddr , 0, iLen );
		 strcpy( pSmtp->m_SmtpIPAddr , config.smtpserver );

     }

	 if ( config.Reply )
	 {
		 iLen = strlen( config.Reply ) +2;
		 pSmtp->m_Reply = malloc (iLen);
		 memset( pSmtp->m_Reply , 0, iLen );
		 strcpy( pSmtp->m_Reply , config.Reply );
	 }
	 else
		 pSmtp->m_Reply = NULL ;
	 if ( config.account )
	 {
		 iLen = strlen( config.account ) +2;
		 pSmtp->m_UserName = malloc (iLen);
		 memset( pSmtp->m_UserName , 0, iLen );
		 strcpy( pSmtp->m_UserName , config.account );
	 }
	 else
		 pSmtp->m_UserName = NULL ;

	 if ( config.password )
	 {
		 iLen = strlen( config.password ) +2;
		 pSmtp->m_Password = malloc (iLen);
		 memset( pSmtp->m_Password , 0, iLen );
		 strcpy( pSmtp->m_Password , config.password );
	 }
	 else
		 pSmtp->m_Password = NULL ;


	 pSmtp->m_bHasAttach = FALSE; 
	 pSmtp->m_AttachList.iCnt = 0 ;
     pSmtp->m_AttachList.ppFileList = NULL ;
	 
	 if ( config.From )
	 {
		 iLen = strlen( config.From ) + 2;
		 pSmtp->m_From = malloc( iLen );
		 memset(pSmtp->m_From, 0 , iLen ) ;
		 strcpy( pSmtp->m_From , config.From );
	 }
	 else
		 pSmtp->m_From = NULL ;
     pSmtp->m_pToList.iToCnt = 0;
	 pSmtp->m_pToList.ppTlst = NULL;
	 pSmtp->m_Data    = NULL;
	 pSmtp->m_Body    = NULL;
	 pSmtp->m_BodyType = 0;
	 pSmtp->m_Subject = NULL;
	 pSmtp->m_Cc      = NULL;
	 pSmtp->m_Port    = 25 ; //smtp port 	 
	 DeInitEmailConfig( &config );	 
	 return TRUE ;
SmtpFail:
	 DeInitEmailConfig( &config );
	 return FALSE;
}

/*************************************************
������void  DeInitializeSmtpSocket( PSmtpSocket pSmtp )
������pSmtp       - ���ڷ����ʼ���SMTP���
����ֵ��void
���������� �����ʼ���ɺ󣬻�ʧ�ܺ�����ͷ���Դ
���ã�     ���û�����ã����ܴ�����Դй¶
************************************************/
void  DeInitializeSmtpSocket( PSmtpSocket pSmtp )
{
	   if ( pSmtp->m_UserName )
		   free( pSmtp->m_UserName );
	   if ( pSmtp->m_Password )
		   free( pSmtp->m_Password );
	   if ( pSmtp->m_Cc )
		   free( pSmtp->m_Cc);
	   if ( pSmtp->m_Body )
		   free( pSmtp->m_Body );
	   if ( pSmtp->m_SmtpIPAddr )
		   free ( pSmtp->m_SmtpIPAddr );
	   if ( pSmtp->m_From )
		   free (pSmtp->m_From );
       if ( pSmtp->m_Reply )
		   free (pSmtp->m_Reply );
	   if (pSmtp->m_pToList.iToCnt )
	   {
		   int i;
	       for ( i = 0; i < pSmtp->m_pToList.iToCnt ; i++ )
			    free( pSmtp->m_pToList.ppTlst[i].szToAddress );
		   free( pSmtp->m_pToList.ppTlst );
		   pSmtp->m_pToList.ppTlst = NULL;
		   pSmtp->m_pToList.iToCnt = 0;
	   }
	   if  (pSmtp->m_Data )
		   free( pSmtp->m_Data );
       if (pSmtp->m_Subject)
		   free ( pSmtp->m_Subject );
	   
       if ( pSmtp->m_bHasAttach )
	   {
		   int i ;
		   for ( i=0 ;i<pSmtp->m_AttachList.iCnt ; i++ )
		   {
              free ( pSmtp->m_AttachList.ppFileList[i] );
		   }
		   pSmtp->m_bHasAttach = FALSE;
		   free (pSmtp->m_AttachList.ppFileList );
	   }
}

/*************************************************
������BOOL ParseOkErr( LPTSTR szBuf, short flags)
������
IN      szBuf       - ���յ����ַ���
IN      flags       - ��������Щ�׶ε��ַ���
����ֵ��BOOL TRUE  �õ���Ӧ�����Լ���
             FALSE û�еõ���Ӧ�����߷��ʹ���
���������� �����յ����ַ���ע��ο�SmtpЭ�飬��ϸ��ο�MIME��SMTP ��POP3��ʽ���ĵ�
���ã�    �յ������ַ���ʱ�����������
************************************************/
BOOL ParseOkErr( LPTSTR szBuf, short flags)
{
	int i=0;
	
    switch ( flags )
	{
	case 1:
		while ( i < 3 )
		{
			
			if ( szBuf[i] == '2' && szBuf[i+1] == '2' && szBuf[i+2] == '0')
				return TRUE;
			i++;
		}
        break;//the first touch.
	case 2:
        
		while (i<3)
		{
			
			if ( szBuf[i] == '2' && szBuf[i+1] == '5' && szBuf[i+2] == '0')
				return TRUE;
			i++;
		}
		break;
	case 3:
		while ( i < 3 )
		{
			
			if ( szBuf[i] == '3' && szBuf[i+1] == '5' && szBuf[i+2] == '4')
				return TRUE;
			i++;
		}
		break;
	case 4://SMTP��֤ʱ��ʾ��һ����ȷ��ֵ
		while( i<3 )
		{
			if ( szBuf[i] == '3' && szBuf[i+1] == '3' && szBuf[i+2] == '4' )
				return TRUE;
			i++;
		}
		break;
	case 5://SMTP��֤ʱ��ʾ��һ��������ȷ�󣬿���ʹ��SMTP�����ʼ���
		//RETAILMSG(TME,("password: %s ",szBuf));
		while( i<3 )
		{
			if ( szBuf[i] == '2' && szBuf[i+1] == '3' && szBuf[i+2] =='5' )
				return TRUE;
			i++;
		}
		break;
	case 6://helo��֤ʱ��ʾ��һ��������ȷ�󣬿���ʹ��SMTP�����ʼ���
		//RETAILMSG(TME,("helo: %s",szBuf));
		while( i<3 )
		{//document said ,server doesn't receive the request,must answer with 521.
			if ( szBuf[i] == '5' && szBuf[i+1] == '0' && szBuf[i+2] =='1' )
				return FALSE;
			i++;
		}
		return TRUE;
		break;
	}
		

  return FALSE;
}
/*************************************************
������BOOL CanSendEmail( Socket  s )
������
IN      s       - ���ڷ����ʼ���SMTP SOCKET ���
����ֵ��BOOL TRUE  �õ���Ӧ�����Կ���
             FALSE û�еõ���Ӧ�����߷��ʹ���
���������� �� �ʼ�Э��淶����һ�����ӳɹ��󣬷������ᷢ��һЩ��Ϣ��USER ��������ݽ�����
           �ſ��Լ���
���ã�  ��SendEmail ������
************************************************/
BOOL CanSendEmail( SOCKET  s )
{
	CHAR  szBuf[256] ;
    int   iBufLen = 256;
	
	memset(szBuf,0,256);
    RETAILMSG(TME,("Send mail Judge \r\n"));
	if (fnReadFromSock( s, szBuf, iBufLen, 0)==SOCKET_ERROR)
	{
		SetCurrentError( SERVER_NOT_READY );
		return FALSE;
	}
	//parse the recv...
	RETAILMSG(TME,(" %s \r\n",szBuf));
	if (!ParseOkErr(szBuf,1))
	{
		SetCurrentError(SERVER_NOT_SUPPORT);
		return FALSE;
	}
	return TRUE;
}

static CHAR * GetEmailAddress( CHAR* szTo )
{
	int iPos;
     if ( (iPos=StrPos( szTo,' ')) == 0 )
		 return szTo ;
     return (CHAR*)(szTo + iPos);
}

/*************************************************
������BOOL  SendEmailHeader( PSmtpSocket pSmtp )
������
IN      pSmtp       - ���ڷ����ʼ���SMTP���
����ֵ��BOOL   TRUE -�����ʼ�ͷ���
               FALSE-�����ʼ�ʧ��
���������� �ڷ����ʼ�����ǰ�����뷢��From ��To �Ȼ�����Ϣ��SERVER
���ã�     �� SendEmail ����
************************************************/
BOOL  SendEmailHeader( PSmtpSocket pSmtp )
{    
	   CHAR szBuf[256];
	   int  iBufLen = 256 ;
	   int  i ;
	   SOCKET    sc = pSmtp->m_Socket ;
       BOOL  bRetVal ;
	   
	   memset( szBuf, 0, iBufLen );
       sprintf( szBuf,TEXT("HELO %s%c%c"),pSmtp->m_SmtpIPAddr,13,10);
	   iBufLen = strlen( szBuf );
	   RETAILMSG(TME,("!!!!!!!!!!������HELO����!\r\n"));
       if( fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!����HELO����ʧ��!\r\n"));
//		   RETAILMSG(TME,("!!!!!!!!!!recv string=%s!!!!!!!!!\r\n",szStr));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   RETAILMSG(TME,("!!!!!!!!!!������HELO�����Ӧ!\r\n"));
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!����ʧ��!\r\n"));
		   return FALSE;
	   }

       if ( !ParseOkErr( szBuf , 6 ) )
	   {
		   SetCurrentError(USER_INVALID);
		   RETAILMSG(TME,("!!!!!!!!!!����ʧ��!\r\n"));
		   return FALSE;
	   }
       //���һЩ�ض���ISP������Ҫ��������֤
       memset( szBuf, 0, iBufLen );
       sprintf( szBuf,TEXT("AUTH LOGIN%c%c"),13,10);
	   iBufLen = strlen( szBuf );
       if( fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��1!\r\n"));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��2!\r\n"));
		   return FALSE;
	   }
       if ( !ParseOkErr( szBuf , 4 ) )
	   {
		   //����ǲ�Ҫ����֤��ISP�Ļ����Ͳ���Ҫ���������ˣ�ֱ�ӷ����ʼ������ˡ�
	   }
	   else
	   {
       //Ҫ���û���
	   memset( szBuf, 0, iBufLen );
	   EncodeBase64( pSmtp->m_UserName,szBuf,strlen(pSmtp->m_UserName) );
       iBufLen = strlen( szBuf );
	   szBuf[iBufLen++] = 13;
	   szBuf[iBufLen++] = 10;
	   szBuf[iBufLen] = 0;
	   //sprintf( szBuf,TEXT("%c%c"),13,10);
	   //iBufLen = strlen( szBuf );
       if( fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��3!\r\n"));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��4!\r\n"));
		   return FALSE;
	   }
       if ( !ParseOkErr( szBuf , 4 ) )
	   {
		   SetCurrentError(SMTP_USER_INVALID);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��5!\r\n"));
		   return FALSE;
	   }
	   //Ҫ������
	   memset( szBuf, 0, iBufLen );
	   EncodeBase64( pSmtp->m_Password,szBuf,strlen(pSmtp->m_Password) );
       iBufLen = strlen( szBuf );
	   szBuf[iBufLen++] = 13;
	   szBuf[iBufLen++] = 10;
	   szBuf[iBufLen] = 0;
	   //sprintf( szBuf,TEXT("%c%c"),13,10);
	   //iBufLen = strlen( szBuf );
       if( fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��6!\r\n"));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��7!\r\n"));
		   return FALSE;
	   }
       if ( !ParseOkErr( szBuf , 5) )
	   {
		   SetCurrentError(SMTP_PASSWORD_INVALID);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��8!\r\n"));
		   return FALSE;
	   }
	   }
	   //���һЩ�ض���ISP������Ҫ��������֤

	   memset( szBuf, 0,256 );
	   sprintf( szBuf,TEXT("MAIL FROM:%s%c%c"),pSmtp->m_From,13,10);
		   
	   iBufLen = strlen( szBuf );
	   
	   if( fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��9!\r\n"));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��10!\r\n"));
		   return FALSE;
	   }
	   //To see if  the name is valid.
	   if ( !ParseOkErr( szBuf , 2 ) )
	   {
		   SetCurrentError(ADDRESS_USER_INVALID);
		   RETAILMSG(TME,("!!!!!!!!!!recv string=%s!!!!!!!!!\r\n",szBuf));
		   RETAILMSG(TME,("!!!!!!!!!!ʧ��11!\r\n"));
		   return FALSE;
	   }
	   //We can repeat any times. 
	   i = 0 ;
	   bRetVal = FALSE;
	   //�����е��ʼ���ַ���߷�������������еĵ�ַ�Ǵ���ģ��򲻷���
	   while (i<pSmtp->m_pToList.iToCnt)
	   {
		   CHAR *szPointer ;
		   szPointer = GetEmailAddress( pSmtp->m_pToList.ppTlst[i].szToAddress );
		   sprintf( szBuf,TEXT("RCPT TO:%s%c%c"),szPointer,13,10);
		   iBufLen = strlen(szBuf);
		   if (fnSendToSock( sc,szBuf, iBufLen, 0) == SOCKET_ERROR)
		   {
			   SetCurrentError( SERVER_NOT_READY);
			   break;
		   }
		   iBufLen = 256;
		   memset(szBuf, 0, 256);
		   if (fnReadFromSock( sc,  szBuf, iBufLen, 0)==SOCKET_ERROR)
		   {
			   SetCurrentError( SERVER_NOT_READY);
			   break;
		   }
		   
		   if (!ParseOkErr( szBuf, 2))
		   {
		       pSmtp->m_pToList.ppTlst[i].bRet = FALSE;	
			   SetCurrentError( ADDRESS_NOT_ACCEPT );
		   }
		   else
		   {
			   pSmtp->m_pToList.ppTlst[i].bRet = TRUE;
			   bRetVal  = TRUE;
		   }
		   i++;
	   }
	   if ( bRetVal )  // ˵������һ���ʼ��Ѿ������ͳ�ȥ��
		   return TRUE ;
	   else 
	   {
		   RETAILMSG(TME,("!!!!!!!!!!���ʧ��!\r\n"));
		   return FALSE;
	   }
}

/*************************************************
������void EncodeEmailBody(CHAR *dest,CHAR *source)
������
OUT      dest    - ���ܺ���ַ���
IN       sourse  - ����ǰ��Դ�ַ���
����ֵ�� void
���������� ��BASE64���ʼ�����м��ܣ���һ�в�����76���ַ�����
���ã�     �� SaveTempEmailFile ����
************************************************/
void EncodeEmailBody(CHAR *dest,CHAR *source)
{
	int    nLength;
	int    i,times;
	CHAR * Part1,*Part2;
	nLength = strlen(source);
    Part1 = malloc(58);
	Part2 = malloc(80);
	memset( Part2, 0, 80);
	memset( Part1, 0, 58);
    times = nLength/MAX_BYTE;
	//���뱣֤��ʽ�ϸ���ÿ�У������������С�����Ӧ��ÿ�ζ��������֡�
	//�����ǽ���BODY���Խ�����strcat�Ϳ����ˣ�����Ҫʹ��������
    for ( i = 0; i < times; i++ )
    {
		strncpy( Part1, source + i*57, 57);
	    EncodeBase64( Part1, Part2,57);
		Part2[76] = 13;
		Part2[77] = 10;
		Part2[78] = 0;
		strcat( dest, Part2);
		memset( Part2, 0, 80);
		memset( Part1, 0, 58);
	}
	if ((times = nLength%MAX_BYTE) > 0)
	{
		strncpy( Part1, source+i*57, times);
		EncodeBase64( Part1, Part2,times);
		strcat( dest, Part2);
		times = strlen( dest );
//���һ�в����0x0d,0x0a����Ϊ��֮�������
	}
    free( Part1 );
	free( Part2 );

}

/*************************************************
������void SaveAttachFile( HANDLE hFile, CHAR *szFile ,CHAR * szBoundary ,int iFlags)
������
IN      hFile    -��ʱ�ʼ��ļ����
IN      szFile   -�����ļ���
IN      szBoundary-�ڸ����е�boundary
IN      iFlags   - 1:��ʾ�������ǲ������������ļ�����ɳ������ļ�
                   0:������������
����ֵ��void
���������� ���ж���ʼ�������Ҫ���ʱ��һ��һ���ؼ�
���ã�     ���ѵ�ǰ�ʼ����浽��ʱ�ļ�ʱ���� SaveTempEmailFile ����
************************************************/
void SaveAttachFile( HANDLE hFile, CHAR *szFile ,CHAR * szBoundary,int iFlags )
{
   HANDLE hAttachFile;
   CHAR * Part1,*Part2;
   CHAR *szTemp;
   DWORD dwWrite,dwRead;
   BOOL  bRet ;
   CHAR szFileName[256];
   int  iFileNameLen;
   
   hAttachFile=CreateFile(szFile, GENERIC_READ  , 0, 0, 
		          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );//==NULL)
       
   if (hAttachFile==INVALID_HANDLE_VALUE)
	   return;
   //continue if file exists.
   szTemp=malloc(100);
   //��Ӹ�����׼��ʽ��
   memset( szFileName, 0,256 );
   GetFileNameEx( szFile, szFileName,&iFileNameLen );
   sprintf(szTemp,TEXT("%c%c--%s%c%c"),13,10,szBoundary,13,10);
   bRet = WriteFile(hFile,szTemp,strlen(szTemp),&dwWrite,NULL);
   {
	   int iExt;
	   CHAR szExt[4];
	   GetFileNameEx( szFile,NULL,&iExt);
   if ( iFlags )
   {	   
       if ( iExt == 7 )
		   strcpy( szExt,"gif");
	   else if ( iExt == 8 )
		   strcpy( szExt,"bmp");
	   sprintf(szTemp,TEXT("Content-Type: image/%s;%c%c"),szExt,13,10 );
   }
   else
	   sprintf(szTemp,TEXT("Content-Type: application/x-msdownload;%c%c"),13,10);
   }
   bRet = WriteFile(hFile,szTemp,strlen(szTemp),&dwWrite,NULL);
   sprintf(szTemp,TEXT("	name=%c%s%c%c%c"),0x22,szFileName,0x22,13,10);
   bRet = WriteFile(hFile,szTemp,strlen(szTemp),&dwWrite,NULL);
   sprintf(szTemp,TEXT("Content-Transfer-Encoding: base64%c%c"),13,10);
   bRet = WriteFile(hFile,szTemp,strlen(szTemp),&dwWrite,NULL);
   sprintf(szTemp,TEXT("Content-Disposition: attachment;%c%c"),13,10);
   WriteFile(hFile,szTemp,strlen(szTemp),&dwWrite,NULL);
   sprintf(szTemp,TEXT("		name=%c%s%c%c%c%c%c"),0x22,szFileName,0x22,13,10,13,10);
   WriteFile(hFile,szTemp,strlen(szTemp),&dwWrite,NULL);
  //��Ӹ������ļ�����
   
   Part1=malloc(58);
   Part2=malloc(80);

   memset(Part1,0,58);
   memset(Part2,0,80);
   //���dwRead<57��ʾ�ļ������ˡ�
   while (ReadFile( hAttachFile, Part1, 57, &dwRead, NULL) && dwRead == 57)
   {
       EncodeBase64( Part1, Part2,dwRead);
	   //sprintf( szTemp, TEXT("%s%c%c"), Part2, 13, 10);
	   WriteFile( hFile, Part2, 76, &dwWrite, NULL);
	   sprintf( szTemp,TEXT("%c%c"),13,10);
       WriteFile( hFile, szTemp, 2, &dwWrite, NULL);
	   memset( Part2, 0, 80);
	   memset( Part1, 0, 58);
   }
   if ( dwRead>0 ){
	   int iEnd = (dwRead/3 + (dwRead%3?1:0))*4;   
	   EncodeBase64( Part1, Part2,dwRead);
	   WriteFile( hFile, Part2,iEnd, &dwWrite, NULL);
	   sprintf( szTemp, TEXT("%c%c"),13, 10);
	   WriteFile( hFile, szTemp, 2, &dwWrite, NULL);
	}

   free( szTemp );
   free( Part2  ) ;
   free( Part1  );
   CloseHandle( hAttachFile );
}

/*************************************************
������BOOL  SaveTempEmailFile( PSmtpSocket pSmtp ,UINT  uiFlags,CHAR *szDefault )
������
IN      pSmtp       - ���ڷ����ʼ���SMTP���
IN      uiFlags     - ���ʼ����浽ĳ��������ȥ
                      BOX_REC - 0 ����
					  BOX_SET - 1 ���浽������ȥ
					  BOX_DRAFT- 2 ���浽�ݸ���ȥ
����ֵ��BOOL   TRUE - �������ʱ�ʼ�
               FALSE- ʧ��
���������� �ڷ��ʼ�ǰ��Ҫ���ܣ��������ݵȲ��������Ȱ����ݼ��ܳ�һ���ĸ�ʽ��
           ������һ����ʱ���ļ��
���ã�     �� SendEmail ����
************************************************/
BOOL  SaveTempEmailFile( PSmtpSocket pSmtp, UINT  uiFlags, CHAR * szDefault )
{
     HANDLE    hFile;
     CHAR   *  szEmailText;  
	 CHAR   *  szDestBody;  //���ܵ��õ�
	 DWORD       dwWrite;
	 int       iLen;
	 BOOL      bAttach;
	 CHAR   *  szTemp , *szEmailName;
	 int       i;
	 CHAR      szBoundary1[50], szBoundary2[50];
	 CHAR      szStr[12];
	 CHAR      szTimeStr[10];
	 SYSTEMTIME st;
	 //������һ����ʱ���ļ���

	 memset(szBoundary1, 0, 50);
	 memset(szBoundary2, 0, 50);
	 strcpy(szBoundary1, BOUNDARY );
	 strcpy(szBoundary2, BOUNDARY );
	 i = GetTickCount();
	 memset(szStr,0, 12);
	 itoa(i, szStr, 16 );
	 strcat(szBoundary1, szStr);
	 memset(szStr,0,12);
	 i+=69;
	 itoa(i, szStr,16 );
	 strcat(szBoundary2, szStr );
	 szEmailName = malloc ( 100 );
	 GetTimeStr(szTimeStr);	 	 
     switch ( uiFlags )
	 {//DRAFT_BOX
	 case 2:
		 if ( szDefault )
			 sprintf( szEmailName, TEXT("%s%so.mlg"),szDraftBox,szDefault);  
		 else
			 sprintf( szEmailName, TEXT("%s%so.mlg"),szDraftBox,szTimeStr);  
		 break;
	 case 1://SET_BOX
		 { //������Ҫ���������Ʊ��浽�ѷ��ʼ�������
			 //Get Send mail name.
    		 CONFIGINFO con;
	    	 InitEmailConfig( &con );
		     GetEmailConfig( &con );
             if(con.iSaveNum==0)
				 return FALSE;
			 sprintf( szEmailName, TEXT("%s8888o%d.mlg"),szSentBox,GetSaveTempEmailName()); //keep the filename the same
		 }
		 break;
	 default://REC_BOX������ʱ�ʼ�
         strcpy( szEmailName,szTempEmailNameInMApi );
	 }
     hFile = CreateFile( szEmailName,
		                     GENERIC_READ|GENERIC_WRITE, 0, NULL, 
							 CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL ); 
	 RETAILMSG(TME,("!!!!!!!!!!��ʱ�ļ�=%s!\r\n",szEmailName));
     free( szEmailName );
	 if (hFile == INVALID_HANDLE_VALUE)
	 {
		 int ierr;
		 ierr = GetLastError();
		 RETAILMSG(TME,("!!!!!!!!!!������ʱ�ļ�ʧ��!error = %d\r\n",ierr));
	     SetCurrentError( FILE_ERROR_EXCEPTION );
		 return FALSE ;
	 }
	 szEmailText = malloc( 100 );
     szTemp = malloc( 100 );
     //ȷ�����޸���,�и���������º�û�и�������� ʹboundary��һ���Ĵ���취
	 if (pSmtp->m_bHasAttach)
		 bAttach  =  TRUE;
	 else
		 bAttach  =  FALSE;
	
	 //����ʼ���ַ
     if (pSmtp->m_From )
	 {
		 CONFIGINFO con;
		 InitEmailConfig( &con );
		 GetEmailConfig( &con );
		 TrimBlankStr( con.user );
		 if ( con.user && strlen( con.user )!= 0 )
			 sprintf(szEmailText,TEXT("From:%c%s%c <%s>%c%c"),34,con.user,34,pSmtp->m_From,13,10);
		 else
			 sprintf(szEmailText,TEXT("From:<%s>%c%c"),pSmtp->m_From,13,10);
            
		 WriteFile(hFile,szEmailText,strlen( szEmailText ),&dwWrite,NULL);
		 DeInitEmailConfig( &con );
     }
	 //���ûظ�
	 TrimBlankStr(pSmtp->m_Reply);
     if ( pSmtp->m_Reply && strlen( pSmtp->m_Reply)>0 )
	 {
		 sprintf( szEmailText,TEXT("Reply-To:%s%c%c"),pSmtp->m_Reply,13,10 );
	     WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 }
	 //���Ŀ�ĵ�
	 strcpy(szEmailText,"To:");
	 
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 for ( i = 0 ; i < pSmtp->m_pToList.iToCnt ;i++ )
	 {
		 sprintf( szEmailText, TEXT("%s;"), pSmtp->m_pToList.ppTlst[i] );
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
		 //�������
	 }
	 sprintf(szEmailText,TEXT("%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 if ( uiFlags != 0 )
	 {//������� 0 
		 sprintf(szEmailText,TEXT("MLG_Sent:0%c%c"),13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);		 
	 }
	 if ( uiFlags == 1 )
	 {//��¼���͵�ʱ��
        CHAR szData[30];
		int  iStrLen ;
		memset( szData, 0, 30 );
		GetLocalTime( &st );
		ConvertEmailTime( szData, &st );
		iStrLen = strlen( szData );
		szData[iStrLen++] = 13;
		szData[iStrLen++] = 10;
		szData[iStrLen] = 0;
	    WriteFile(hFile,szData,strlen(szData),&dwWrite,NULL);		 
	 }
	 if( pSmtp->m_Subject )
	 {
		 sprintf(szEmailText,TEXT("Subject:%s%c%c"),pSmtp->m_Subject,13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     }//��ó���

	 if ( pSmtp->m_Cc )
	 {
		 sprintf(szEmailText,TEXT("Cc:%s%c%c"),pSmtp->m_Cc,13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     }
	 //���ʱ��,����EMAIL���������Զ����ʼ�����ʱ��ֵ.
     //sprintf(szEmailText,TEXT("Date:%s%c%c"),TEXT("Date: Wed, 9 Oct 2002 12:02:33 +0800"),13,10);
	 //WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);

	 //�õ��汾��

     sprintf(szEmailText,TEXT("MIME-Version: 1.0%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 //
     sprintf(szEmailText,TEXT("Content-Type: multipart/mixed;%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     sprintf(szEmailText,TEXT("		boundary=%c%s%c%c%c"),0x22,szBoundary1,0x22,13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     
	 sprintf(szEmailText,TEXT("X-Priority: 3%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 
	 //Ϊ��֤΢����ʶ����ʼ������Ᵽ��
	 //�ж����޸���
	 
     if (bAttach)
	 {
		 sprintf(szEmailText,TEXT("%c%c--%s%c%c"),13,10,szBoundary1,13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
		 sprintf(szEmailText,TEXT("Content-Type: multipart/alternative;%c%c"),13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
		 sprintf(szEmailText,TEXT("		boundary=%c%s%c%c%c"),0x22,szBoundary2,0x22,13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 }
	 //�ʼ����ʽ
     if (bAttach)
		 sprintf(szEmailText,TEXT("%c%c--%s%c%c"),13,10,szBoundary2,13,10);
	 else
		 sprintf(szEmailText,TEXT("%c%c--%s%c%c"),13,10,szBoundary1,13,10);
	 
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     if ( pSmtp->m_BodyType == 2 ){
         sprintf(szEmailText,TEXT("Content-Type: text/html;%c%c"),13,10);
	 }
	 else{
		 sprintf(szEmailText,TEXT("Content-Type: text/plain;%c%c"),13,10);
	 }
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 //�����ļ������ һ����gb2312������ʵ���Ͽ�����ȫ���������
     sprintf(szEmailText,TEXT(" charset=%cgb2312%c%c%c"),0x22,0x22,13,10);//reserve for chinese.
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     sprintf(szEmailText,TEXT("Content-Transfer-Encoding: base64%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	
	 sprintf(szEmailText,TEXT("%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 
	 iLen = strlen( pSmtp->m_Body ) * 2;
	 szDestBody = malloc( iLen+2);
	 if ( szDestBody == NULL )
	 {
	     CloseHandle( hFile );
		 //�ڴ治��
		 return FALSE;
	 }
	 memset( szDestBody ,0, iLen +2 ) ;
     if ( iLen > 0 )
	 {
		 EncodeEmailBody( szDestBody, pSmtp->m_Body );
		 // sprintf(szEmailText,TEXT("%s%c%c"),Destbody,13,10);
		 WriteFile(hFile,szDestBody,strlen(szDestBody),&dwWrite,NULL);
	 }
	 sprintf(szEmailText,TEXT("%c%c%c%c"),13,10,13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 if (bAttach)
	 	 sprintf(szEmailText,TEXT("--%s--%c%c"),szBoundary2,13,10);
	 else
		 sprintf(szEmailText,TEXT("--%s--%c%c"),szBoundary1,13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 
     free ( szDestBody );
//���ʼ���֮����Ҫ����boundary.
 //�ʼ�������ʽ��
     if (bAttach)
	 {//�������Է��֣�OutLook�Զ������������ǣ�boundary��
	  //--boundary
	  //--boundary
	  //--boundary--ֱ�����һ������������
		 //uiFlags == 2 means it is a html file,it will not include attach file,maybe the future version
		 //will afford this service.
		 if ( uiFlags != 2 ) { 
		 for ( i =0 ; i < pSmtp->m_AttachList.iCnt;i++)
		 {//���ݸ��������� ������һ���������ʼ��ļ���ȥ
			 if ( pSmtp->m_BodyType == 3 )
				 SaveAttachFile(hFile,pSmtp->m_AttachList.ppFileList[i],szBoundary1,1);
			 else
				 SaveAttachFile(hFile,pSmtp->m_AttachList.ppFileList[i],szBoundary1,0);
		 }
		 sprintf(szTemp,TEXT("%c%c--%s--%c%c"),13,10,szBoundary1,13,10);
		 WriteFile(hFile,szTemp,strlen(szTemp),&dwWrite,NULL);
		 }
	 }     

	 free(szTemp);
	 free(szEmailText);
	 CloseHandle(hFile);
     return TRUE;	 
}

/*************************************************
������BOOL SendTempEmailFile( PSmtpSocket  pSmtp )
������
IN      pSmtp       - ���ڷ����ʼ���SMTP���
����ֵ��BOOL    TRUE -������ȫ�ɹ�
                FALSE-����ʧ��
���������� ��һ�ж�׼�������ˣ��Ϳ��Է���
���ã�     ��Ҫ��ȫ��װ������£�Ϊ�˽�ʡ�ڴ�Ļ���������ý��������ݺ͸���������
           ������һ����ʱ�ļ�����������ɺ󣬾�ɾ�����ļ���ͬʱ����һ������
		   �����䣬�Ա��ݣ���Ϊ�п����ʼ�������ȥ�����������ݶ�ʧ��
************************************************/
BOOL SendTempEmailFile( PSmtpSocket  pSmtp )
{
	 HANDLE   hFile ;
     CHAR     szBuf[256];
     int      iBufLen ;
	 SOCKET   sc = pSmtp->m_Socket;
	 DWORD    dwRead;
     CHAR   * szEmailText;
	 iBufLen = 256;
//����DATA�����ʾ��ʼ�����ʼ�����
	 sprintf(szBuf,TEXT("DATA %c%c"),13,10);
	 iBufLen = strlen( szBuf );
	 
	 if (fnSendToSock( sc, szBuf, iBufLen, 0) == SOCKET_ERROR)
	 {
		 SetCurrentError( SERVER_NOT_READY);
		 return FALSE;
	 }
	 
	 iBufLen=256;
	 memset( szBuf, 0, 256);
	if ( fnReadFromSock( sc, szBuf, iBufLen, 0)==SOCKET_ERROR)
	{
		SetCurrentError( SERVER_NOT_READY);
		return FALSE;
	}			
	if (!ParseOkErr(szBuf,3))
	{
		SetCurrentError(TO_NOT_KNOWN);
		return FALSE;
	}
    //Send Mail begins
	//�����ļ��洢�ķ�ʽ������������͹����ʼ�������ʱ��ֱ�Ӱ��ʼ��ļ����͹�ȥ��
	//--------------------------------------------
	hFile = CreateFile(szTempEmailNameInMApi, GENERIC_READ | GENERIC_WRITE , 0, 0, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );//==NULL)
	RETAILMSG(TME,("!!!!!!!!!!�����ļ�=%s\r\n",szTempEmailNameInMApi));
    if (hFile == INVALID_HANDLE_VALUE){
		int ierr;
    	ierr = GetLastError();
        RETAILMSG(TME,("!!!!!!!!!!�����ļ�ʧ��!ierr = %d\r\n",ierr));
		RETAILMSG(TME,("!!!!!!!!!!�ļ�=%s!\r\n",szTempEmailNameInMApi));
	    SetCurrentError ( FILE_ERROR_EXCEPTION );
		return FALSE;
	}
	szEmailText=malloc( 1024 );   //ÿ�η��Ͷ��ٸ��ֽڣ��Դﵽ��ݵ�Ŀ�ģ�����̫��Ļ������������?
	memset( szEmailText, 0, 1024 );
	while (ReadFile(hFile,szEmailText,1000,&dwRead,NULL) && dwRead>0)
	{
		if ( fnSendToSock(sc,szEmailText,dwRead,0) < 0 ){
		    SetCurrentError( SERVER_NOT_READY );
            CloseHandle( hFile );    
			return FALSE;
		}

  	}	
	sprintf( szBuf, TEXT("%c%c%c%c%c"), 13, 10, 0x2E, 13, 10);
	iBufLen = strlen( szBuf );
	CloseHandle( hFile );
	if (fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	{
		SetCurrentError( SERVER_NOT_READY);
		return FALSE;
	}
	memset( szBuf, 0, 256);
	iBufLen = 256;
	if (fnReadFromSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	{
		SetCurrentError(SERVER_NOT_READY);
		return FALSE;
	}
	
	if (!ParseOkErr(szBuf,2))
	{
		SetCurrentError(SERVE_NOT_RECEIVE);
		return FALSE;
	}
	SendMessage( FindWindow(NULL,"�����ʼ�Flash"),MM_DOWNLOAD,(WPARAM)0,(LPARAM)2 );
	free(szEmailText);
    return TRUE;
	//���������һ���ʼ�ʱ����Ҫ����ʱ�ʼ�ɾ����ͬʱ����һ�ݵ�������
	//���Ҫ��һ���㷨���������������
}

/*************************************************
������void DeleteTempEmail( )
������
����ֵ��void
���������� ���ʼ���ȫ������Ϻ�Ҫ��ɾ����ʱ�ʼ�
���ã�    ��SendEmail ��ʹ��
************************************************/
void DeleteTempEmail( )
{
    DeleteFile( szTempEmailNameInMApi );
}

/*************************************************
������BOOL  SendEmail( PSmtpSocket pSmtp ,LPTSTR szEmailName )
������
IN      pSmtp       - ���ڷ����ʼ���SMTP���
����ֵ��void
���������� ��һ�ж�׼�������ˣ��Ϳ��Է���
���ã�     ��Ҫ��ȫ��װ������£�Ϊ�˽�ʡ�ڴ�Ļ���������ý��������ݺ͸���������
           ������һ����ʱ�ļ�����������ɺ󣬾�ɾ�����ļ���ͬʱ����һ������
		   �����䣬�Ա��ݣ���Ϊ�п����ʼ�������ȥ�����������ݶ�ʧ��
************************************************/
BOOL CheckSmtpSender ( PSmtpSocket pSmtp );
BOOL  SendEmail( PSmtpSocket pSmtp )
{
    BOOL   bRetVal = TRUE;
	
    pSmtp->m_Socket = TCPClient( pSmtp->m_SmtpIPAddr ,pSmtp->m_Port );
    if (pSmtp->m_Socket == INVALID_SOCKET )
	{
        SetCurrentError( SERVER_LINK_ERROR );
	    bRetVal = FALSE ;
	}
    RETAILMSG(TME,("!!!!!!!!!!TCPClient�������ӳɹ�!\r\n"));
    if (bRetVal && !CanSendEmail( pSmtp->m_Socket ))
	{
        SetCurrentError( SERVER_LINK_ERROR );//????
		bRetVal = FALSE;
	}
	RETAILMSG(TME,("Send EmailHeader .. \r\n"));
//#define _SMTPTEST
#ifdef _SMTPTEST
	
	if ( CheckSmtpSender( pSmtp ) == FALSE )
	{
		SetCurrentError( ERROR_NOT_CONFIG );
		return FALSE;
	}
	else
		return TRUE;
#else

    if ( bRetVal && !SendEmailHeader( pSmtp ))
	{
		bRetVal = FALSE;
	}
	RETAILMSG(TME,("After SendEmailHeader .. %d\r\n",bRetVal ));
    if ( bRetVal )
		SaveTempEmailFile( pSmtp, 0 ,NULL);
	if ( bRetVal )
	{
		RETAILMSG(TME,("SendTemp &&&&&&&&&&&&&&& EmailFile ...\r\n"));
		bRetVal = SendTempEmailFile( pSmtp );		
		RETAILMSG(TME,("After SendTemp &&&&&&&&&&&&&&& EmailFile ...\r\n"));
		DeleteTempEmail(); 
	}
#endif
	DoneCommand( pSmtp->m_Socket );
	return bRetVal ;   
}

/*************************************************
������BOOL CheckEmailAddress( LPTSTR szTo ,DWORD * pdwCnt)
������
IN       szTo        - �����ʼ��ĵ�ַ���ַ���
OUT      pdwCnt      - ������Ч���ʼ���ַ��
����ֵ�� TRUE       -  ������To����
         FALSE      -  ��Ч�ĵ�ַ����
���������� ����ʼ�To��ַ�Ƿ���ȫ��Ч,ԭ���ǣ����ܶ��ࣵ�����ֽڣ������ʼ��ر��ַ�@
           ����һ���Ƚ��ϸ����֤��������������';'֮����ڿ��ַ�����
���ã�     ��CheckEmailAddress�е���
************************************************/
BOOL CheckEmailAddress( LPTSTR szTo ,DWORD * pdwCnt )
{
    CHAR * szTemp;
	CHAR   szOneTo[60];
	int iPos ;
	int iToCnt =0 ;
	BOOL bRetVal = FALSE;
    //�� ';'�����벻ͬ�ʼ��ĵ�ַ 
	*pdwCnt = 0 ;
	szTemp = szTo ;
	memset( szOneTo, 0, 60);
	while ( 1 )
	{
		iPos = StrPos( szTemp ,';');
		if (iPos>0 && iPos<58 ){
			if( FindStringBeforeCh( szTemp ,';', szOneTo ))
			{
				if( !CheckValidEmail(szOneTo) )
				{
					SetCurrentError( INVALID_EMAIL_ADDRESS );
					break;
				}
				iToCnt ++;			   
			}
			else
			{
				SetCurrentError( INVALID_EMAIL_ADDRESS );
				break;
			}
		}
		else
		{//���һ�ַ������п�����û���ַ���Ҳ������һ���մ���Ҳ������һ���ʼ���ַ
			if (iPos>58 )
			{ 
				SetCurrentError( INVALID_EMAIL_ADDRESS );
				break;
			}
			//�����ַ����Ƿ�ջ���Ч��
			if ( (iPos = strlen(szTemp)) && iPos< 58 ){
				TrimString(szTemp); //???
				if (!CheckValidEmail( szTemp ))
					SetCurrentError( INVALID_EMAIL_ADDRESS );
				else
				{
					iToCnt++;
					bRetVal = TRUE;
				}
				break;
			}
			if ( iPos >=58 )
			    SetCurrentError( INVALID_EMAIL_ADDRESS);
			else 
			{
			    SetCurrentError( SUCCESS_SOCKET_ERROR );
				bRetVal = TRUE;
			}
			break;
		}
		memset( szOneTo, 0, 60 );
		szTemp+=iPos;
	}
	*pdwCnt = iToCnt ;
	return bRetVal;
}
/*************************************************
������BOOL InitToAddress( PSmtpSocket pSmtp )
������
IN      pSmtp       - ���ڷ����ʼ���SMTP���
����ֵ�� TRUE       -������To����
         FALSE      -��������To����
���������� �ڷ����ʼ�ǰ��Ҫ��To��װ�ɶ��У��Ժ����ʹ��
           ���ȼ���ַ�Ƿ�����
���ã�     �����ʼ�ǰ��Ҫ��鲢ȷ����Ч��ַ
************************************************/
BOOL InitToAddress( PSmtpSocket pSmtp ,LPTSTR szTo)
{  
	DWORD dwToCnt ;// ����ֲ����Kingmos Platbuilderʱ��һЩ���Ͳ�һ��
	int  iToCnt = 0;
    CHAR szOneTo[256]; //һ���ʼ��ĵ�ַ���Ȳ��ܳ���60���ֽ�
    CHAR * szTemp;
	int    i, iPos;
	//���ҵĳ�����ڴ����˷ѣ�������	
	if (!CheckEmailAddress( szTo ,&dwToCnt ) || dwToCnt==0)
	{
	    SetCurrentError( INVALID_EMAIL_ADDRESS );
		return FALSE ;
	}
	iToCnt = (int)dwToCnt;
	pSmtp->m_pToList.iToCnt = iToCnt ;
	pSmtp->m_pToList.ppTlst = (PToAddress )malloc (sizeof(ToAddress)*iToCnt);
    memset( szOneTo, 0, 256 );
    szTemp = szTo ;
	i = 0 ;
	while ( i < iToCnt ){
		
		if (FindStringBeforeCh( szTemp,';', szOneTo))
		{	
			TrimString( szOneTo );
			pSmtp->m_pToList.ppTlst[i].szToAddress = malloc( strlen(szOneTo)+2);
			strcpy( pSmtp->m_pToList.ppTlst[i].szToAddress, szOneTo );
			pSmtp->m_pToList.ppTlst[i].bRet = FALSE; //��������������һ����Ч�ĵ�ַʱ����ΪTRUE
			iPos = StrPos( szTemp, ';' );
			szTemp = szTemp + iPos ;				
			//����һ����ַ
		}
		else 
		{
			TrimString( szTemp );
			pSmtp->m_pToList.ppTlst[i].szToAddress = malloc( strlen(szTemp)+2);
			strcpy( pSmtp->m_pToList.ppTlst[i].szToAddress, szTemp );
			pSmtp->m_pToList.ppTlst[i].bRet = FALSE; //��������������һ����У�ĵ�ַʱ����ΪTRUE
		}
		memset( szOneTo ,0,256 );
		i++;
	}
	return TRUE;
}

/*************************************************
������void DeInitToAddress( PSmtpSocket pSmtp )
������
IN      pSmtp       - ���ڷ����ʼ���SMTP���
����ֵ�� void
���������� �����ʼ���ɺ�Ҫ���ܹ��ͷ���Դ�����û�еĻ�����Ҫ����SMTP�ṹ
������  �� ����ʱ�ͷţ�Ҫ������ܹ�����

  ���ã�     �����ʼ�ǰ��Ҫ��鲢ȷ����Ч��ַ
************************************************/
void DeInitToAddress( PSmtpSocket pSmtp )
{
	int i;
	//Ҫ��һ��
	if (  pSmtp->m_pToList.iToCnt  && pSmtp->m_pToList.ppTlst )
    { 
		for (i =0; i< pSmtp->m_pToList.iToCnt ;i++ )
			free( pSmtp->m_pToList.ppTlst[i].szToAddress ) ;
		free (pSmtp->m_pToList.ppTlst) ;
		pSmtp->m_pToList.ppTlst = NULL;
	}
	pSmtp->m_pToList.iToCnt = 0;
}

/*************************************************
������int CheckAndDownLoad()
������
����ֵ�� int  �������ʼ��ĸ���.
���������� ������ʱ��ؼ�����ʼ�����ͬʱ����
���ã�  ����ϵͳ���
ע�⣺��ϵͳ���û�ͬʱ��ʹ��ʱ��Ӧ��ʹ����Դ����,�����¼MesID�����ݱ��
************************************************/
int CheckAndDownLoad()
{
	Pop3Socket Pop3;
    int  iNew = 0;
    
	SocketStartUp( 1,1 );
	if (!InitializePop3Socket( &Pop3 ) )
	{
        SocketCleanUp();
		return 0;
	}
	Pop3.m_Socket = TCPClient( Pop3.m_Pop3IPAddr ,Pop3.m_Port );
	if (Pop3.m_Socket == INVALID_SOCKET )
	{
		SetCurrentError( SERVER_NOT_READY );
		goto DownEnd;
	}
	if( !AuthorizationPass( &Pop3 ))
		goto DownEnd;
	if (GetServerEmailInfo( &Pop3 ))
		iNew = DownLoadNewMails( &Pop3 );  //�������ʼ�
	
	DoneCommand( Pop3.m_Socket );
DownEnd:
	DeInitializePop3Socket( &Pop3 );
	SocketCleanUp();
	return iNew;
}

BOOL DelServerMailBackup( PPop3Socket pPop3, int iIndex )
{
	CHAR * szTemp;
	BOOL  bRet = TRUE;
	szTemp = (CHAR* )malloc ( 100 );
	if ( szTemp == NULL )
		return FALSE;
	sprintf( szTemp,"DELE %d\r\n",iIndex );
	if ( SOCKET_ERROR == fnSendToSock( pPop3->m_Socket,szTemp,strlen( szTemp ), NULL ) )
	{
		free( szTemp );
		return NULL;
	}
	memset( szTemp,0, 100 );
	if ( fnReadFromSock( pPop3->m_Socket,szTemp,99,0 )== SOCKET_ERROR )
	{
		free( szTemp );
		return FALSE;
	}
	//ɾ���ļ�ʧ��
	if ( !ParseRecv( szTemp ) )
	{
		SetCurrentError( MESSAGE_NOT_EXIST );
	    bRet = FALSE;
		//ɾ���ļ�ʧ��
	}

	free( szTemp );
	return bRet;
    
}
/*************************************************
������BOOL GetRestrictTable(LPTSTR szBuf )
������
IN szBuf    -�ѽ��ܵ�Pop3 SERVER��Ӧ����Ϣ(������+OK)
����ֵ��TRUE-�ɹ�
        FALSE - ʧ�ܣ�
�������������ݷ���pop3��LIST����󣬵õ�server��Ӧ�������Ʊ�(������+OK)
�������������Ʊ��ʼ��ߴ�������ֵ����200K�ȣ�
���ã�
************************************************/
BOOL GetRestrictTable(LPTSTR szBuf )
{
	
	int i;
	int n = 0;
    int iPos1 = 0;
	int iPos2 = 0;
	CHAR * szTempStr = szBuf;
	int iIndex = 1;
	CHAR szSizeStr[10];
	if ( g_iSizeLimit == -1 || szBuf == NULL )
		return TRUE;
	if ( EmailLimitTable )
		free( EmailLimitTable );
    EmailLimitTable = malloc( 100*sizeof(BOOL) );
	memset( EmailLimitTable,0, 100*sizeof(BOOL));
	iLimitNum = 100;
//    while ( iIndex++ < 99 )
	while ( iIndex < 99 ) //jms rid ++
	{
		iPos1 = StrPos( szTempStr,' ');
		iPos2 = StrPos( szTempStr,'\r');
		memset( szSizeStr,0,10 );
		if ( iPos1 && iPos2 )
		{
			i = min( (iPos2-iPos1),9 );
            strncpy( szSizeStr,szTempStr +iPos1-1 ,iPos2-iPos1 );
			TrimBlankStr( szSizeStr );
			if ( atoi( szSizeStr )/1024 > g_iSizeLimit )
			{
                EmailLimitTable[n++] = iIndex;
			}
			szTempStr +=iPos2;
			iIndex++;
		}
		else
			break;
        
		if ( iPos1 = StrPos(szTempStr,'\n') )
			szTempStr+=iPos1;
		else
			break;
	}
    
	return TRUE;
}
//======================== send garbage email ===================
BOOL CheckSmtpSender ( PSmtpSocket pSmtp )
{
    CHAR szBuf[256];
    int  iBufLen;

	sprintf( szBuf,"EHLO %c%c",13,10 );
	if ( fnSendToSock( pSmtp->m_Socket,szBuf,strlen( szBuf ), 0 )< 0 )
        return FALSE;
	memset ( szBuf,0,256 );

	if ( fnReadFromSock( pSmtp->m_Socket,szBuf,255,0 )< 0)
		return FALSE;
	if ( ParseOkErr( szBuf,2 ) == FALSE )   
		return FALSE;
/*	sprintf( szBuf,TEXT("250-SIZE 230430%c%c"),13,10);
    
	if ( send( pSmtp->m_Socket,szBuf,strlen( szBuf ),0 )< 0 )
		return FALSE;
	memset( szBuf,0,256 );
	if  ( recv( pSmtp->m_Socket,szBuf,255,0 ) < 0 )
		return FALSE;
	if ( !ParseOkErr( szBuf,2 ) )
        return FALSE;
  */
	sprintf( szBuf,"MAIL FROM:<%s>%c%c",pSmtp->m_From,13,10);
	if ( fnSendToSock( pSmtp->m_Socket,szBuf,strlen( szBuf ),0 )< 0 )
		return FALSE;
	memset( szBuf,0,256 );
	if ( fnReadFromSock( pSmtp->m_Socket,szBuf,255,0 )< 0 )
		return FALSE;
	if ( !ParseOkErr( szBuf,2 ) )
        return FALSE;

	sprintf( szBuf,"RCPT TO:<myw@mlg.com.cn>%c%c",13,10);
	if ( fnSendToSock( pSmtp->m_Socket,szBuf,strlen( szBuf ),0 )< 0 )
		return FALSE;
	memset( szBuf,0,256 );
	if ( fnReadFromSock( pSmtp->m_Socket,szBuf,255,0 )< 0 )
		return FALSE;
	if ( !ParseOkErr( szBuf,2 ) )
        return FALSE;

    sprintf(szBuf,TEXT("DATA %c%c"),13,10);
	iBufLen = strlen( szBuf );
	 
	 if (fnSendToSock( pSmtp->m_Socket, szBuf, iBufLen, 0) == SOCKET_ERROR)   
       return FALSE;
	 memset( szBuf,0, 256 );
    if ( fnReadFromSock( pSmtp->m_Socket,szBuf,255,0 )< 0 )
		return FALSE;
	if ( !ParseOkErr( szBuf,3 ) )
        return FALSE;
	strcpy( szBuf,"Dear roby , your test is wonderful") ;
	if ( fnSendToSock( pSmtp->m_Socket,szBuf,strlen( szBuf ),0 )< 0 )
		return FALSE;
	sprintf( szBuf,"%c%c.%c%c",13,10,13,10);
	if ( fnSendToSock( pSmtp->m_Socket,szBuf,strlen( szBuf ),0 )< 0 )
		return FALSE;
	memset( szBuf,0, 256 );
    if ( fnReadFromSock( pSmtp->m_Socket,szBuf,255,0 )< 0 )
		return FALSE;
	if ( !ParseOkErr( szBuf,2 ) )
		return FALSE;
	sprintf( szBuf,"QUIT %c%c",13,10 );
	fnSendToSock( pSmtp->m_Socket,szBuf,256,0 );
    return TRUE;
}
/****************************************************************
������BOOL fnIfServerRespondEnd(const char * string )
������
IN szStr  -	 ����
����ֵ�����Ծ���β����true,����Ϊfalse.
�����������ж��Ƿ�����'.'(���)��β��һ��������Ծ�����һ������
***/
BOOL fnIfServerRespondEnd(const char * szStr)
{
	char ch;
	int i;
	i = strlen(szStr);
	ch = szStr[i-3];
	RETAILMSG(TME,("!!!!!!!!!!recv string=%s!!!!!!!!!\r\n",szStr));
	if(ch == '.')
	{
		RETAILMSG(TME,("!!!!!!!!!!recv end string=%s!!!!!!!!!\r\n",szStr));
		return TRUE;
	}
	return FALSE;
}
/****************************************************************
������static int fnReadFromSock(SOCKET s,char FAR *buf,int len,int flags);
������
IN szStr  -	 ����
����ֵ�����Ծ���β����true,����Ϊfalse.
������������socket�������   //recv( s, szBuf, iBufLen, 0)
***/
static int fnReadFromSock(SOCKET s,char FAR *buf,int len,int flags)
{
   fd_set fdread;
   struct timeval times;
   
   FD_ZERO(&fdread);
   FD_SET(s,&fdread);
   times.tv_sec = escapeTIME;
   times.tv_usec =1;
//   RETAILMSG(TME,("!!!!!!!!!!!��\r\n"));
   if(select(0,&fdread,NULL,NULL,&times)==SOCKET_ERROR)
   {
//	   RETAILMSG(TME,("!!!!!!!!!!!����ʱ\r\n"));
       return SOCKET_ERROR;
   }
   //  RETAILMSG(TME,("!!!!!!!!!!!select end\r\n"));
     return recv(s,buf,len,flags);
}
/****************************************************************
������static int fnReadFromSock(SOCKET s,char FAR *buf,int len,int flags);
������
IN szStr  -	 ����
����ֵ�����Ծ���β����true,����Ϊfalse.
������������socket�������   //recv( s, szBuf, iBufLen, 0)
***/
static int fnSendToSock(SOCKET s,char FAR *buf,int len,int flags)
{
//   fd_set fdwrite;
//   struct timeval times;

/*
   FD_ZERO(&fdwrite);
   FD_SET(s,&fdwrite);
   times.tv_sec = escapeTIME;
   times.tv_usec =1;
   RETAILMSG(TME,("!!!!!!!!!!!д\r\n"));
   if(select(0,NULL,&fdwrite,NULL,&times)==SOCKET_ERROR)
   {
	   RETAILMSG(TME,("!!!!!!!!!!!д��ʱ\r\n"));
       return SOCKET_ERROR;
   }
   RETAILMSG(TME,("!!!!!!!!!!!select end\r\n"));
*/
     return send(s,buf,len,flags);
}
