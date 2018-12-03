/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

/**************************************************
文件说明：Email API 函数说明，通过封装，使使用者可以简单得调用，而不用
          处理POP3，SMTP 等有关的协议，所有的协议在内部处理同时邮件的接受
		  也只能在限制在内部的目录内。在我们的邮件箱里，如果用于手机则，只允许
		  单用户使用，如果是多在电脑中使用，则可以支持多个用户。
版本号：1.0.0
开发时期：2003-3-12
作者：满益文,瞿吉祥
修改记录：
   1:补加了下载后删除服务器上的邮件功能，并当邮件大于限额并且不完全下载的
   　邮件不作删除处理。(2004-12-22)
   2:修正备份已发邮件问题，默认备分5份，(2004-12-23)
   3:有一个问题：在DownLoadMail里有ParseRetr它在当邮件大于1M时就取消下载
     以避免网速慢的问题。这样在有限制大小时只下载邮件头时也被取消了，
	 所以我去掉ParseRetr里1M的限制，而在前面加MAXSIZE_MEGA开关时，不管用
	 户有没有加限制设置都限制在1M里。(2005-04-06)
**************************************************/
#include <ewindows.h>
//#include <socket.h>
#include <MailBoxApi.h>
#include <EmailFormat.h>
#include <BaseQuoted.h>
#include "EmailFlags.h"
#include "endalign.h"
/********************************  全局变量区 *************************/
#define BOUNDARY         TEXT("--NextPart_021679a")
#define MAX_BYTE 57  //根据邮件协议，用Base64编码的邮件，每一行不能超过76个字节。
#define escapeTIME 4 //每次传输事务的等时间最大值
//#define SENTMAILNAME     TEXT("\\systgem\\email\\send\\sendmlg")

#define MAXSIZE_MEGA //定义最大尺寸1M的邮件有可能完全下载

static int CurErr;
static int g_iSizeLimit = 200;
static BOOL *EmailLimitTable = NULL;
static int iLimitNum =0;
static int * CurMax;//指向当前最大可下载的邮件数
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
/********************************  全局变量区 *************************/
BOOL DelServerMailBackup( PPop3Socket pPop3, int iIndex );
BOOL GetRestrictTable(LPTSTR szBuf );
/*************************************************
声明：static BOOL Email_FileExist( LPCTSTR lpName )
参数：
IN    lpName : 需要判断的邮件文件名
返回值：  TRUE： 文件存在
          FALSE：文件不存在
功能描述：判断文件存在否
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
声明：int GetSaveTempEmailName()
参数：

返回值：返回需要保存的临时文件名序号
功能描述：获得要保存文件临时文件名，对应于控制面板上邮件设置：保留
引用：    保存已发邮件时使用
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
声明：int ParseRetr( LPTSTR szText )
参数：
IN    szText    -Retr 命令时，接受的字符串
返回值：- 当大于等于0时成功，返回值是邮件的大小
        -1 - 不可以接受       
功能描述：解析Retr时,可以要求不接受大于1M 的邮件。问题在于如果要向用户提供
          存在一个新邮件的信息，虽然不能下载.
引用：    收邮件时使用
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
	// 通过有的ISP测试，发现在回应的后面没有大小设定，所有不能判断该邮件的大小因此
	//不利于接受，
	if (j == 0 ){
	    RETAILMSG(TME,("NO SIZE interpret"));
		return 0;
	}
	//在那里没有处理好，因为如果没有把所有的数据下载下来的话，则可能会有数据阻塞，会不会出现其他的情况呢？
	return atoi(szTemp);
/*
	if ((i = atoi( szTemp))< 1024*1536  && i>40 )
		return TRUE ;
	else
		return FALSE;
		*/
}

/*************************************************
声明：BOOL ParseRecv( LPTSTR szText )
参数：
IN    szText    -需要解释的字符串
返回值：TRUE- 收到+OK信息，有效命令
        FALSE-不接受前一次的命令
功能描述：按邮件协议来处理命令回应信息
引用：    每一个命令后，都需要判断是否是一个有效的命令
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
声明：BOOL DoneCommand( SOCKET  socket )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
返回值：TRUE-关闭成功
       
功能描述：完成操作后要想SERVER发送关闭命令
引用：
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
声明：static SOCKET TCPClient(CHAR* szDest,int iPort)
参数：
IN  szDest    -准备建立的客户端时，对方服务器的地址
IN  iPort     -建立连接的端口号
返回值：SOCKET 返回有限的SOCKET 句柄
	      
功能描述：根据服务地址 和端口号，与服务器建立连接
引用：任何SOCKET 操作都需要首先建立连接
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
	//这里需要的是将NAME转化IP地址，如果IP的话，就
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
	//对于有多个网卡的Host，如何进行识别和设定一个正确的配置??
	//是否需要进行多个。
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
	//这里需要的是将NAME转化IP地址，如果IP的话，就
	RETAILMSG(TME,("start gethostbyname \r\n"));
#ifdef KINGMOS  //EML_WIN32	 modified by xyg_2004_08_30
    pHostInfo = gethostbynameex( szDest ,bBufHost,sizeof(bBufHost)); 
    RETAILMSG(TME,("............................................. \r\n"));
#else
	pHostInfo = gethostbyname( szDest); 
#endif	
	RETAILMSG(TME,("end gethostbyname \r\n"));
	//对于有多个网卡的Host，如何进行识别和设定一个正确的配置??
	//是否需要进行多个。
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
声明：BOOL InitializePop3Socket( PPop3Socket pPop3 )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
返回值：TRUE  初始化成功返回
        FALSE 初始化失败
	      
功能描述：初始化所有资源，要求在连接SERVER之前将需要的资源分配好
引用：在接受邮件之前，需要调用该函数来初始化
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
	    strcpy( pPop3->m_Pop3IPAddr ,config.pop3server );//如果没有转换的话，还需要解析转化成IP地址
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
声明：BOOL DeInitializePop3Socket( PPop3Socket pPop3 )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
返回值：TRUE-释放初始化时分配的资源
	      
功能描述：在完成邮件接受后要释放资源
引用：在接受邮件之后，需要调用该函数来释放资源
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
声明：BOOL AuthorizationPass( PPop3Socket pPop3 )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
返回值：TRUE-连接成功,通过权限验证
        FALSE - 连接失败，非法用户	      
功能描述：连接POP3服务器
引用：在ConnectServer 中使用
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
	  RETAILMSG(TME,("!!!!!!!!!!授权初始recv!\r\n"));
	  if (fnReadFromSock( s, szBuf, iBufLen, 0)==SOCKET_ERROR )
	  {
		  SetCurrentError( AUTH_PASS_ERROR );
		  RETAILMSG(TME,("!!!!!!!!!!授权初始失败!\r\n"));
	      return FALSE;
	  }
//	  RETAILMSG(TME,("!!!!!!!!!!授权初始完成!\r\n"));
	  RETAILMSG(TME,("!!!!!!!!!!recv=%s iBufLen = %d\r\n",szBuf,iBufLen));
	  if (!ParseRecv( szBuf ))
	  {
          SetCurrentError( AUTH_PASS_ERROR );
	      return FALSE;
	  }
	  //---------------Server Ready---------------------------
	  //transfer user to server . 
	  memset( szBuf, 0, 256); 
//	  RETAILMSG(TME,("!!!!!!!!!!发送USER!\r\n"));
	  sprintf( szBuf, TEXT("USER %s%c%c"),pPop3->m_UserName,13,10);
	  if (fnSendToSock( s,szBuf,strlen( szBuf), 0)==SOCKET_ERROR)
	  {
          SetCurrentError( AUTH_PASS_ERROR );
		  return FALSE;
	  }
	  memset( szBuf, 0, 256);
//	  RETAILMSG(TME,("!!!!!!!!!!接收user确认!\r\n"));
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
	 //密码验证
	  memset( szBuf, 0, 256); 
//	  RETAILMSG(TME,("!!!!!!!!!!发送PASS!\r\n"));
	  sprintf(szBuf,TEXT("PASS %s%c%c"),pPop3->m_Password,13,10);
	  if (fnSendToSock( s,szBuf,strlen(szBuf),0)==SOCKET_ERROR)
	  {
          SetCurrentError( AUTH_PASS_ERROR );
		  return FALSE;
	  }
//      RETAILMSG(TME,("!!!!!!!!!!接收pass确认!\r\n"));
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
声明：BOOL FindNewEmailNum(SOCKET s,int *num,int * pNewList)
参数：
IN     s    -用来接受Pop3邮件的SOCKET句柄
OUT    num  -返回新邮件的个数
OUT    pNewList - 返回新邮件的INDEX列表
返回值：TRUE-得到正确的信息
        FALSE -不能得到正确的信息
功能描述：从POP3 服务器中得到正确的邮件个数，和总邮件大小
引用：在 GetServerEmailInfo 中使用
注：当邮件的数量太多时，可能会导致内存不够，因此要么使用动态分配的方式
    或者 在移动电脑中采用数量限制的方法
************************************************/
BOOL FindNewEmailNum(SOCKET s,int *num,int * pNewList)
{
       int i,buflen , len;
	   CHAR *buf;
	   CHAR *MesID;
	   int   iMesIDLen =  1000;//初试长度为1000

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
	   //获得邮件的邮件头列表，同时得到新邮件的个数。
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
		  *num=i; //得到个数 
		  free( MesID );
	   }
	   else
	   {
		   free( MesID );
		   free( buf );
		   return FALSE;
	   }
       //在LIST 命令中可以判断出每一个邮件的大小，如果需要根据大小来决定是否接受，
	   //可以在此处理。
	   sprintf(buf,TEXT("%s %c%c"),TEXT("LIST"),13,10);
	   
	   buflen=strlen(buf);	   
	   fnSendToSock(s,buf,buflen,0);
	   memset(buf,0,1000);
	   //解释邮件数据大小，同时做出限制表明下载限制的邮件
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
声明：BOOL JudgeStat(SOCKET s,int *num,int *cbyte)
参数：
IN s    -用来接受Pop3邮件的SOCKET句柄
IN/OUT *num -成功时返回总邮件数
IN/OUT *cbyte -成功时返回总邮件字节数
返回值：TRUE-得到正确的信息
        FALSE - 不能得到正确的信息
功能描述：从POP3 服务器中得到正确的邮件个数，和总邮件大小
引用：在ConnectServer 中使用
************************************************/
BOOL JudgeStat(SOCKET s,int *num,int *cbyte)
{
	   int i,buflen;
	   CHAR buf[100];
	   CHAR mailInfo[20];
	   CHAR ch;
	   //STAT命令得到邮件总数和总字节数
	   //应答如："+OK 3 12648"　邮箱里有3封，总大小为12648字节
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
声明：BOOL GetServerEmailInfo( PPop3Socket pPop3 )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
返回值：TRUE- 获得正确的信息
        FALSE - 不能得到正确的信息      
功能描述：获得邮件服务器中邮件信息，如邮件头，邮件列表数，邮件大小情况
引用：在接受邮件前，要求得到新邮件的信息和大小等，USER可以只下载新邮件
************************************************/
BOOL GetServerEmailInfo( PPop3Socket pPop3 )
{
	SOCKET s = pPop3->m_Socket ;
    int   iTotal = 0 ;
	int   iCBytes = 0 ;
	int   iNewList[100];
	int   iNewNum = 0 ;
	int   m ;

	//得到邮箱里总邮件数和大小。
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
	//测试是否可以用memcpy的方式
	for ( m = 0 ;m<iNewNum ;m++ ){
	    RETAILMSG(TME,("mailList :%d \r\n",iNewList[m]));
		pPop3->m_pNewIndex[m] = iNewList[m];
	}
	return TRUE ;
}

/*************************************************
声明：BOOL ConnectServerAndDown( PPop3Socket pPop3 )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
IN/OUT Max  -入时用来表示收件箱只有多少空件，出时为新下载的邮件数，如没下载完为-1   
返回值：TRUE-连接成功
        FALSE - 连接失败	      
功能描述：连接POP3服务器以获取邮件
引用：在需要向SERVER请求邮件时，首先要调用该函数连接，前提条件是
      获得配置信息，且SOCKET 已经初始化了。
************************************************/
BOOL ConnectServerAndDown( PPop3Socket pPop3 ,int * Max)
{
	 SendMessage( FindWindow(NULL,"下载邮件"),MM_DOWNLOAD,(WPARAM)0,(LPARAM)0 );
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
	 SendMessage( FindWindow(NULL,"下载邮件"),MM_DOWNLOAD,(WPARAM)1,(LPARAM)1 );
	 if (!GetServerEmailInfo( pPop3 ))
	 {
		 DoneCommand( pPop3->m_Socket );
		 RETAILMSG(TME,("!!!!!!!!!!!!GetServerEmailInfo fail!...\r\n"));
		 return FALSE ;
     }
	 RETAILMSG(TME,("!!!!!!!!!!!!!!!Begin Download emails"));
	 if ( pPop3->m_bDownAllOrNew )
		 DownLoadAllMails( pPop3 ) ; //下载所有的邮件
	 else
         DownLoadNewMails( pPop3 );  //下载新邮件
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
声明：BOOL DownLoadMail( PPop3Socket pPop3, int iIndex )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
IN iIndex   -准备下载的邮件的Index ;
返回值：TRUE-下载成功
        FALSE- 下载失败
       
功能描述：下载所需要的邮件,本文的邮件的命名存在问题，希更正
引用：
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
	
    RETAILMSG(TME,("!!!!!!!!!!下载第%d封信\r\n",iIndex));
	//在此需要通知上层ap目前的状态	
//    SendMessage( FindWindow(NULL,"下载邮件"),MM_DOWNLOAD,2,(LPARAM)iIndex );
//　因为发生有错误的邮件，导至提示下载数大于实际下载数，所以我把上一行放到下面解析后去了。

	//判断是否是受限制下载大小的邮件
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
    SendMessage( FindWindow(NULL,"下载邮件"),MM_DOWNLOAD,2,(LPARAM)iIndex );
	RETAILMSG(TME,("DOWN........... wait!"));
	szText=malloc(4098);
    iBufLen=4096;
    memset(szText,0,4098);
    //when move to esoft ,change the dir to esofe
	//  "\\system\\email\\receive\\filename.mlg"
	GetTimeStr(szTimeStr);

	//在这里我增加一个标志，识别是否是完整的内容，t完整,f非完整.
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
			//当读内容时出现错误，删除这封内容不完整的邮件
			free(szText);
			CloseHandle(hFile);
//			DeleteFile(szFileName);
//			SetCurrentError( READ_ERROR_RETRY );
			MessageBox(NULL,"接收故障,请重试!","收邮件",0);
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

	//邮件设置里为下载后服务器不保留及本邮件的大小不超过限额则删本次
	//邮件
	if(pPop3->m_bCopyAndLoad)
	  if(!bDel)
	    DelServerMailBackup(pPop3,iIndex);

    CloseHandle(hFile);
    free(szText);      
	return TRUE;    
}

/*************************************************
声明：BOOL DownLoadAllMails( PPop3Socket pPop3 )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
返回值：TRUE-下载成功
        FALSE- 下载失败
       
功能描述：下载所需要的邮件
引用：
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

		 iTotalIDList[i] = -1 ;//表示该邮件已经被down下
	 }
	 //更新邮件头的列表
	 RefreshIDList( pPop3->m_BoxTotal, iTotalIDList ) ;
	 free( iTotalIDList );
	 RETAILMSG(TME,("Download over "));
     return bRet;
}

/*************************************************
声明：int DownLoadNewMails( PPop3Socket pPop3 )
参数：
IN pPop3    -用来接受Pop3邮件的SOCKET句柄
IN iIndex   -准备下载的邮件的Index ;
返回值：TRUE-下载成功
        FALSE- 下载失败
       
功能描述：下载新邮件，
引用：
************************************************/
int DownLoadNewMails( PPop3Socket pPop3 )
{
	 int i,iNum ;
	 iNum = min(*CurMax,pPop3->m_iNewCnt);
	 if(pPop3->m_iNewCnt > *CurMax)
		 *CurMax = -1;
	 else *CurMax = pPop3->m_iNewCnt;

	 RETAILMSG(TME,("!!!!!!!!!!!!!将要下载%d封邮件\r\n",iNum));
	 for (i = 0 ;i< iNum ; i++ )
	 {
		 RETAILMSG(TME,("!!!!!!!!!!!!!下载%d封邮件\r\n",i));
		 if ( !DownLoadMail( pPop3, pPop3->m_pNewIndex[i] ))
		 {
			 SetCurrentError ( UNKOWN_SOCKET_ERROR ) ;
			 break ;
		 }
		 pPop3->m_pNewIndex[i] = -1 ;//表示该邮件已经被down下
	 }
	 //更新邮件头的列表
	 RefreshIDList( pPop3->m_iNewCnt, pPop3->m_pNewIndex ) ;
     return pPop3->m_iNewCnt;	 
}

/*************************************************
声明：void ErrorConvertStr(int error,LPTSTR dest)
参数：
IN error    -错误代码
IN szDest   -返回的错误信息
返回值：void
功能描述：根据错误代码，返回错误信息
引用：    当操作有误时，返回错误信息字符串
************************************************/
void ErrorConvertStr(int error,LPTSTR szDest)
{
	   RETAILMSG(TME,("error :%d \r\n",error));
       switch (error)
	   {
	   case TO_NOT_KNOWN:
		   strcpy(szDest,TEXT("发送数据出错"));
		   break;
	   case MESSAGE_NOT_EXIST:
		   strcpy(szDest,TEXT("删除文件出错"));
		   break;
       case SERVER_NOT_READY:
		   strcpy(szDest,TEXT("服务器没有准备"));
		   break;
	   case SERVER_LINK_ERROR:
		   strcpy( szDest,TEXT("网络服务不存在"));
		   break;
	   case AUTH_PASS_ERROR:
		   strcpy(szDest,TEXT("授权没有被通过"));
		   break;
	   case UNKOWN_SOCKET_ERROR:
		   strcpy(szDest,TEXT("异常发生"));
		   break;
	   case PARSEMAIL_SOCKET_ERROR:
		   strcpy(szDest,TEXT("不明邮件错误"));
		   break;
	   case SERVER_TIME_OUT:
		   strcpy(szDest,TEXT("服务器超时错误"));
	       break;
	   case NOT_MEMORY_AFFORD:
		   strcpy( szDest,TEXT("没有足够的内存"));
		   break;
	   case NOT_DISK_SPACE:
		   strcpy( szDest, TEXT("磁盘空间不足") );
		   break;
	   case INVALID_EMAIL_ADDRESS:
		   strcpy( szDest ,TEXT("无效的邮件地址"));
		   break;
	   case ERROR_CONFIG_SMTP:
		   strcpy(szDest, TEXT("请重新配置邮箱"));
		   break;
	   case ERROR_NOT_CONFIG:
		   strcpy( szDest, TEXT("请先配置邮件收发器"));
		   break;
	   case FILE_ERROR_EXCEPTION:
		   strcpy( szDest, TEXT("文件操作异常!"));
		   break;
	   case FORMAT_SOCKET_ERROR:
		   strcpy ( szDest, TEXT("错误的命令格式"));
		   break;
	   case EMAIL_NOTEXIST:
		   strcpy ( szDest, TEXT("没有邮件!"));
		   break;
	   case NEWMAIL_NOTEXIST:
		   strcpy ( szDest, TEXT("没有新邮件存在!"));
		   break;
	   case INVALID_ADDRESS:
           strcpy ( szDest, TEXT("无效的服务器配置"));
		   break;
	   case ADDRESS_NOT_ACCEPT:
		   strcpy( szDest,TEXT("请提交正确的地址"));
		   break;
	   case SERVER_NOT_SUPPORT:
		   strcpy( szDest, TEXT("没有提供该项服务"));
		   break;
	   case USER_INVALID:
		   strcpy( szDest, TEXT("无效用户名"));
		   break;
	   case ADDRESS_USER_INVALID:
		   strcpy( szDest,TEXT("用户地址设置无效"));
		   break;
	   case SMTP_USER_INVALID:
           strcpy( szDest, TEXT("无效SMTP用户名"));
		   break;
	   case SMTP_PASSWORD_INVALID:
           strcpy( szDest, TEXT("无效SMTP密码"));
		   break;
	   case INVALID_VALUE_FILE:
		   strcpy( szDest, TEXT("文件操作失败"));
		   break;
//	   case READ_ERROR_RETRY:
//		   strcpy(szDest,TEXT("接收故障,请重试!"));
		 
	   default:
		   strcpy( szDest, TEXT("服务中断,请重试!"));
	   }
}

/*************************************************
声明：BOOL SocketStartUp(DWORD hiVersion,DWORD loVersion)
参数：
IN hiVersion    -高位版本号
IN loVersion    -低位版本号
返回值：void
功能描述：初始化SOCKET，调用API WSAStartup来初始化
引用：    所有的有关SOCKET对TCP/IP的操作都必须先SOCKET 初始化
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
声明：void SocketCleanUp( )
参数：没有
返回值：void
功能描述：完成SOCKET操作后，必须释放资源
引用：    简单地调用SOCKET API函数来释放资源
************************************************/
void SocketCleanUp( )
{ 
	  WSACleanup( ) ;
}


/******************************** SMTP operation Function ****************************/
/*************************************************
声明：BOOL InitializeSmtpSocket( PSmtpSocket pSmtp )
参数：pSmtp       - 用于发送邮件的SMTP句柄
返回值：BOOL    TRUE  - 得到正确的配置信息
                FALSE - 不能得到正确的配置信息
功能描述：初始化SMTP配置信息分配资源
引用：    在发送邮件之前必须这样做
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
声明：void  DeInitializeSmtpSocket( PSmtpSocket pSmtp )
参数：pSmtp       - 用于发送邮件的SMTP句柄
返回值：void
功能描述： 发送邮件完成后，或失败后必须释放资源
引用：     如果没有作好，可能存在资源泄露
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
声明：BOOL ParseOkErr( LPTSTR szBuf, short flags)
参数：
IN      szBuf       - 接收到的字符串
IN      flags       - 表明是那些阶段的字符串
返回值：BOOL TRUE  得到回应，可以继续
             FALSE 没有得到回应，或者发送错误
功能描述： 解释收到的字符串注意参考Smtp协议，详细请参考MIME和SMTP 及POP3格式的文档
引用：    收到命令字符串时，都必须调用
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
	case 4://SMTP验证时表示是一个正确的值
		while( i<3 )
		{
			if ( szBuf[i] == '3' && szBuf[i+1] == '3' && szBuf[i+2] == '4' )
				return TRUE;
			i++;
		}
		break;
	case 5://SMTP验证时表示是一个密码正确后，可以使用SMTP发送邮件了
		//RETAILMSG(TME,("password: %s ",szBuf));
		while( i<3 )
		{
			if ( szBuf[i] == '2' && szBuf[i+1] == '3' && szBuf[i+2] =='5' )
				return TRUE;
			i++;
		}
		break;
	case 6://helo验证时表示是一个密码正确后，可以使用SMTP发送邮件了
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
声明：BOOL CanSendEmail( Socket  s )
参数：
IN      s       - 用于发送邮件的SMTP SOCKET 句柄
返回值：BOOL TRUE  得到回应，可以可送
             FALSE 没有得到回应，或者发送错误
功能描述： 按 邮件协议规范来，一旦连接成功后，服务器会发送一些信息，USER 必须把数据接受完
           才可以继续
引用：  在SendEmail 被调用
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
声明：BOOL  SendEmailHeader( PSmtpSocket pSmtp )
参数：
IN      pSmtp       - 用于发送邮件的SMTP句柄
返回值：BOOL   TRUE -发送邮件头完成
               FALSE-发送邮件失败
功能描述： 在发送邮件数据前，必须发送From ，To 等基本信息给SERVER
引用：     被 SendEmail 调用
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
	   RETAILMSG(TME,("!!!!!!!!!!将发送HELO命令!\r\n"));
       if( fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!发送HELO命令失败!\r\n"));
//		   RETAILMSG(TME,("!!!!!!!!!!recv string=%s!!!!!!!!!\r\n",szStr));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   RETAILMSG(TME,("!!!!!!!!!!将接收HELO命令回应!\r\n"));
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!接收失败!\r\n"));
		   return FALSE;
	   }

       if ( !ParseOkErr( szBuf , 6 ) )
	   {
		   SetCurrentError(USER_INVALID);
		   RETAILMSG(TME,("!!!!!!!!!!解析失败!\r\n"));
		   return FALSE;
	   }
       //针对一些特定的ISP服务商要求密码验证
       memset( szBuf, 0, iBufLen );
       sprintf( szBuf,TEXT("AUTH LOGIN%c%c"),13,10);
	   iBufLen = strlen( szBuf );
       if( fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!失败1!\r\n"));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!失败2!\r\n"));
		   return FALSE;
	   }
       if ( !ParseOkErr( szBuf , 4 ) )
	   {
		   //如果是不要求验证的ISP的话，就不需要继续鉴别了，直接发送邮件可以了。
	   }
	   else
	   {
       //要求用户名
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
		   RETAILMSG(TME,("!!!!!!!!!!失败3!\r\n"));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!失败4!\r\n"));
		   return FALSE;
	   }
       if ( !ParseOkErr( szBuf , 4 ) )
	   {
		   SetCurrentError(SMTP_USER_INVALID);
		   RETAILMSG(TME,("!!!!!!!!!!失败5!\r\n"));
		   return FALSE;
	   }
	   //要求密码
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
		   RETAILMSG(TME,("!!!!!!!!!!失败6!\r\n"));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!失败7!\r\n"));
		   return FALSE;
	   }
       if ( !ParseOkErr( szBuf , 5) )
	   {
		   SetCurrentError(SMTP_PASSWORD_INVALID);
		   RETAILMSG(TME,("!!!!!!!!!!失败8!\r\n"));
		   return FALSE;
	   }
	   }
	   //针对一些特定的ISP服务商要求密码验证

	   memset( szBuf, 0,256 );
	   sprintf( szBuf,TEXT("MAIL FROM:%s%c%c"),pSmtp->m_From,13,10);
		   
	   iBufLen = strlen( szBuf );
	   
	   if( fnSendToSock( sc,szBuf, iBufLen, 0)==SOCKET_ERROR)
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!失败9!\r\n"));
		   return FALSE;
	   }
	   iBufLen = 256;
	   memset( szBuf, 0, 256);
	   if (fnReadFromSock( sc, szBuf, iBufLen, 0 ) == SOCKET_ERROR )
	   {
		   SetCurrentError(SERVER_NOT_READY);
		   RETAILMSG(TME,("!!!!!!!!!!失败10!\r\n"));
		   return FALSE;
	   }
	   //To see if  the name is valid.
	   if ( !ParseOkErr( szBuf , 2 ) )
	   {
		   SetCurrentError(ADDRESS_USER_INVALID);
		   RETAILMSG(TME,("!!!!!!!!!!recv string=%s!!!!!!!!!\r\n",szBuf));
		   RETAILMSG(TME,("!!!!!!!!!!失败11!\r\n"));
		   return FALSE;
	   }
	   //We can repeat any times. 
	   i = 0 ;
	   bRetVal = FALSE;
	   //将所有的邮件地址告诉服务器，如果所有的地址是错误的，则不发送
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
	   if ( bRetVal )  // 说明至少一个邮件已经被发送出去了
		   return TRUE ;
	   else 
	   {
		   RETAILMSG(TME,("!!!!!!!!!!最后失败!\r\n"));
		   return FALSE;
	   }
}

/*************************************************
声明：void EncodeEmailBody(CHAR *dest,CHAR *source)
参数：
OUT      dest    - 加密后的字符串
IN       sourse  - 加密前的源字符串
返回值： void
功能描述： 用BASE64对邮件体进行加密，按一行不超过76个字符排列
引用：     被 SaveTempEmailFile 调用
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
	//必须保证格式严格按照每行７６个字来排列。所以应该每次读５７个字。
	//由于是解释BODY所以仅仅用strcat就可以了，不需要使用流操作
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
//最后一行不添加0x0d,0x0a，因为在之后会作。
	}
    free( Part1 );
	free( Part2 );

}

/*************************************************
声明：void SaveAttachFile( HANDLE hFile, CHAR *szFile ,CHAR * szBoundary ,int iFlags)
参数：
IN      hFile    -临时邮件文件句柄
IN      szFile   -附件文件名
IN      szBoundary-在附件中的boundary
IN      iFlags   - 1:表示当附件是不是属于下载文件，或可超连接文件
                   0:属于正常附件
返回值：void
功能描述： 当有多个邮件附件需要添加时，一个一个地加
引用：     当把当前邮件保存到临时文件时，被 SaveTempEmailFile 调用
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
   //添加附件标准格式。
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
  //添加附件的文件内容
   
   Part1=malloc(58);
   Part2=malloc(80);

   memset(Part1,0,58);
   memset(Part2,0,80);
   //如果dwRead<57表示文件读完了。
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
声明：BOOL  SaveTempEmailFile( PSmtpSocket pSmtp ,UINT  uiFlags,CHAR *szDefault )
参数：
IN      pSmtp       - 用于发送邮件的SMTP句柄
IN      uiFlags     - 将邮件保存到某个邮箱中去
                      BOX_REC - 0 保留
					  BOX_SET - 1 保存到发件箱去
					  BOX_DRAFT- 2 保存到草稿箱去
返回值：BOOL   TRUE - 保存成临时邮件
               FALSE- 失败
功能描述： 在发邮件前，要加密，排列数据等操作，首先把数据加密成一定的格式，
           保存在一个临时的文件里。
引用：     被 SendEmail 调用
************************************************/
BOOL  SaveTempEmailFile( PSmtpSocket pSmtp, UINT  uiFlags, CHAR * szDefault )
{
     HANDLE    hFile;
     CHAR   *  szEmailText;  
	 CHAR   *  szDestBody;  //加密调用的
	 DWORD       dwWrite;
	 int       iLen;
	 BOOL      bAttach;
	 CHAR   *  szTemp , *szEmailName;
	 int       i;
	 CHAR      szBoundary1[50], szBoundary2[50];
	 CHAR      szStr[12];
	 CHAR      szTimeStr[10];
	 SYSTEMTIME st;
	 //保存在一个临时的文件里

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
		 { //这里需要考虑如何设计保存到已发邮件的问题
			 //Get Send mail name.
    		 CONFIGINFO con;
	    	 InitEmailConfig( &con );
		     GetEmailConfig( &con );
             if(con.iSaveNum==0)
				 return FALSE;
			 sprintf( szEmailName, TEXT("%s8888o%d.mlg"),szSentBox,GetSaveTempEmailName()); //keep the filename the same
		 }
		 break;
	 default://REC_BOX保存临时邮件
         strcpy( szEmailName,szTempEmailNameInMApi );
	 }
     hFile = CreateFile( szEmailName,
		                     GENERIC_READ|GENERIC_WRITE, 0, NULL, 
							 CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL ); 
	 RETAILMSG(TME,("!!!!!!!!!!临时文件=%s!\r\n",szEmailName));
     free( szEmailName );
	 if (hFile == INVALID_HANDLE_VALUE)
	 {
		 int ierr;
		 ierr = GetLastError();
		 RETAILMSG(TME,("!!!!!!!!!!保存临时文件失败!error = %d\r\n",ierr));
	     SetCurrentError( FILE_ERROR_EXCEPTION );
		 return FALSE ;
	 }
	 szEmailText = malloc( 100 );
     szTemp = malloc( 100 );
     //确认有无附件,有附件的情况下和没有附件的情况 使boundary不一样的处理办法
	 if (pSmtp->m_bHasAttach)
		 bAttach  =  TRUE;
	 else
		 bAttach  =  FALSE;
	
	 //获得邮件地址
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
	 //设置回复
	 TrimBlankStr(pSmtp->m_Reply);
     if ( pSmtp->m_Reply && strlen( pSmtp->m_Reply)>0 )
	 {
		 sprintf( szEmailText,TEXT("Reply-To:%s%c%c"),pSmtp->m_Reply,13,10 );
	     WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 }
	 //获得目的地
	 strcpy(szEmailText,"To:");
	 
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 for ( i = 0 ; i < pSmtp->m_pToList.iToCnt ;i++ )
	 {
		 sprintf( szEmailText, TEXT("%s;"), pSmtp->m_pToList.ppTlst[i] );
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
		 //获得主题
	 }
	 sprintf(szEmailText,TEXT("%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 if ( uiFlags != 0 )
	 {//如果不是 0 
		 sprintf(szEmailText,TEXT("MLG_Sent:0%c%c"),13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);		 
	 }
	 if ( uiFlags == 1 )
	 {//记录发送的时间
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
     }//获得抄送

	 if ( pSmtp->m_Cc )
	 {
		 sprintf(szEmailText,TEXT("Cc:%s%c%c"),pSmtp->m_Cc,13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     }
	 //获得时间,好象EMAIL服务器会自动给邮件附上时间值.
     //sprintf(szEmailText,TEXT("Date:%s%c%c"),TEXT("Date: Wed, 9 Oct 2002 12:02:33 +0800"),13,10);
	 //WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);

	 //得到版本号

     sprintf(szEmailText,TEXT("MIME-Version: 1.0%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 //
     sprintf(szEmailText,TEXT("Content-Type: multipart/mixed;%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     sprintf(szEmailText,TEXT("		boundary=%c%s%c%c%c"),0x22,szBoundary1,0x22,13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
     
	 sprintf(szEmailText,TEXT("X-Priority: 3%c%c"),13,10);
	 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 
	 //为保证微软能识别该邮件，特意保留
	 //判断有无附件
	 
     if (bAttach)
	 {
		 sprintf(szEmailText,TEXT("%c%c--%s%c%c"),13,10,szBoundary1,13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
		 sprintf(szEmailText,TEXT("Content-Type: multipart/alternative;%c%c"),13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
		 sprintf(szEmailText,TEXT("		boundary=%c%s%c%c%c"),0x22,szBoundary2,0x22,13,10);
		 WriteFile(hFile,szEmailText,strlen(szEmailText),&dwWrite,NULL);
	 }
	 //邮件体格式
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
	 //在中文简体版中 一般是gb2312，但是实际上可以完全不对其解释
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
		 //内存不足
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
//在邮件体之后不需要加上boundary.
 //邮件附件格式。
     if (bAttach)
	 {//经过测试发现，OutLook对多个附件的情况是：boundary是
	  //--boundary
	  //--boundary
	  //--boundary--直到最后一个附件产生。
		 //uiFlags == 2 means it is a html file,it will not include attach file,maybe the future version
		 //will afford this service.
		 if ( uiFlags != 2 ) { 
		 for ( i =0 ; i < pSmtp->m_AttachList.iCnt;i++)
		 {//根据附件的名称 ，保存一个附件到邮件文件中去
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
声明：BOOL SendTempEmailFile( PSmtpSocket  pSmtp )
参数：
IN      pSmtp       - 用于发送邮件的SMTP句柄
返回值：BOOL    TRUE -发送完全成功
                FALSE-发送失败
功能描述： 当一切都准备就绪了，就可以发送
引用：     在要完全封装的情况下，为了节省内存的话，如果采用将所有内容和附件都首先
           保存在一个临时文件里，如果发送完成后，就删除该文件，同时保存一份流在
		   发件箱，以备份，因为有可能邮件发不出去，而导致数据丢失。
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
//发送DATA命令表示开始发送邮件数据
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
	//采用文件存储的方式，保留最近发送过的邮件，发送时，直接把邮件文件发送过去。
	//--------------------------------------------
	hFile = CreateFile(szTempEmailNameInMApi, GENERIC_READ | GENERIC_WRITE , 0, 0, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );//==NULL)
	RETAILMSG(TME,("!!!!!!!!!!建立文件=%s\r\n",szTempEmailNameInMApi));
    if (hFile == INVALID_HANDLE_VALUE){
		int ierr;
    	ierr = GetLastError();
        RETAILMSG(TME,("!!!!!!!!!!建立文件失败!ierr = %d\r\n",ierr));
		RETAILMSG(TME,("!!!!!!!!!!文件=%s!\r\n",szTempEmailNameInMApi));
	    SetCurrentError ( FILE_ERROR_EXCEPTION );
		return FALSE;
	}
	szEmailText=malloc( 1024 );   //每次发送多少个字节，以达到快捷的目的，但是太多的话，会否阻塞呢?
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
	SendMessage( FindWindow(NULL,"发送邮件Flash"),MM_DOWNLOAD,(WPARAM)0,(LPARAM)2 );
	free(szEmailText);
    return TRUE;
	//当发送完成一个邮件时，需要将临时邮件删除，同时拷贝一份到发件箱
	//这儿要求一个算法，来计算如何命名
}

/*************************************************
声明：void DeleteTempEmail( )
参数：
返回值：void
功能描述： 在邮件完全发送完毕后，要求删除临时邮件
引用：    在SendEmail 中使用
************************************************/
void DeleteTempEmail( )
{
    DeleteFile( szTempEmailNameInMApi );
}

/*************************************************
声明：BOOL  SendEmail( PSmtpSocket pSmtp ,LPTSTR szEmailName )
参数：
IN      pSmtp       - 用于发送邮件的SMTP句柄
返回值：void
功能描述： 当一切都准备就绪了，就可以发送
引用：     在要完全封装的情况下，为了节省内存的话，如果采用将所有内容和附件都首先
           保存在一个临时文件里，如果发送完成后，就删除该文件，同时保存一份流在
		   发件箱，以备份，因为有可能邮件发不出去，而导致数据丢失。
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
    RETAILMSG(TME,("!!!!!!!!!!TCPClient建立连接成功!\r\n"));
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
声明：BOOL CheckEmailAddress( LPTSTR szTo ,DWORD * pdwCnt)
参数：
IN       szTo        - 发送邮件的地址的字符串
OUT      pdwCnt      - 返回有效的邮件地址数
返回值： TRUE       -  完整的To队列
         FALSE      -  无效的地址存在
功能描述： 检查邮件To地址是否完全有效,原则是：不能多余５８个字节，含有邮件特别字符@
           这是一个比较严格的验证函数，不允许在';'之间存在空字符存在
引用：     在CheckEmailAddress中调用
************************************************/
BOOL CheckEmailAddress( LPTSTR szTo ,DWORD * pdwCnt )
{
    CHAR * szTemp;
	CHAR   szOneTo[60];
	int iPos ;
	int iToCnt =0 ;
	BOOL bRetVal = FALSE;
    //以 ';'来隔离不同邮件的地址 
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
		{//最后一字符串，有可能是没有字符，也可能是一串空串，也可能是一个邮件地址
			if (iPos>58 )
			{ 
				SetCurrentError( INVALID_EMAIL_ADDRESS );
				break;
			}
			//看该字符串是否空或无效的
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
声明：BOOL InitToAddress( PSmtpSocket pSmtp )
参数：
IN      pSmtp       - 用于发送邮件的SMTP句柄
返回值： TRUE       -完整的To队列
         FALSE      -不完整的To队列
功能描述： 在发送邮件前，要将To封装成队列，以后可以使用
           首先检查地址是否完整
引用：     发送邮件前需要检查并确认有效地址
************************************************/
BOOL InitToAddress( PSmtpSocket pSmtp ,LPTSTR szTo)
{  
	DWORD dwToCnt ;// 在移植程序到Kingmos Platbuilder时，一些类型不一致
	int  iToCnt = 0;
    CHAR szOneTo[256]; //一个邮件的地址长度不能超过60个字节
    CHAR * szTemp;
	int    i, iPos;
	//在我的程序存在代码浪费？？？？	
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
			pSmtp->m_pToList.ppTlst[i].bRet = FALSE; //当服务器表明是一个有效的地址时，置为TRUE
			iPos = StrPos( szTemp, ';' );
			szTemp = szTemp + iPos ;				
			//增加一个地址
		}
		else 
		{
			TrimString( szTemp );
			pSmtp->m_pToList.ppTlst[i].szToAddress = malloc( strlen(szTemp)+2);
			strcpy( pSmtp->m_pToList.ppTlst[i].szToAddress, szTemp );
			pSmtp->m_pToList.ppTlst[i].bRet = FALSE; //当服务器表明是一个有校的地址时，置为TRUE
		}
		memset( szOneTo ,0,256 );
		i++;
	}
	return TRUE;
}

/*************************************************
声明：void DeInitToAddress( PSmtpSocket pSmtp )
参数：
IN      pSmtp       - 用于发送邮件的SMTP句柄
返回值： void
功能描述： 发送邮件完成后，要求能够释放资源，如果没有的话，则要求在SMTP结构
　　　  　 析构时释放，要求代码能够完善

  引用：     发送邮件前需要检查并确认有效地址
************************************************/
void DeInitToAddress( PSmtpSocket pSmtp )
{
	int i;
	//要求一致
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
声明：int CheckAndDownLoad()
参数：
返回值： int  返回新邮件的个数.
功能描述： 用于随时监控检测新邮件，并同时下载
引用：  用于系统监控
注意：当系统和用户同时在使用时，应该使用资源互斥,比如记录MesID的数据表等
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
		iNew = DownLoadNewMails( &Pop3 );  //下载新邮件
	
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
	//删除文件失败
	if ( !ParseRecv( szTemp ) )
	{
		SetCurrentError( MESSAGE_NOT_EXIST );
	    bRet = FALSE;
		//删除文件失败
	}

	free( szTemp );
	return bRet;
    
}
/*************************************************
声明：BOOL GetRestrictTable(LPTSTR szBuf )
参数：
IN szBuf    -已接受的Pop3 SERVER的应答消息(不包括+OK)
返回值：TRUE-成功
        FALSE - 失败，
功能描述：根据发送pop3的LIST命令后，得到server的应答创立限制表(不包括+OK)
　　　　　限制表，邮件尺寸大于最大值（如200K等）
引用：
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
声明：BOOL fnIfServerRespondEnd(const char * string )
参数：
IN szStr  -	 缓冲
返回值：是以句点结尾返回true,否则为false.
功能描述：判断是否是以'.'(句点)结尾，一般服务器以句点结束一个事务。
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
声明：static int fnReadFromSock(SOCKET s,char FAR *buf,int len,int flags);
参数：
IN szStr  -	 缓冲
返回值：是以句点结尾返回true,否则为false.
功能描述：从socket里读数据   //recv( s, szBuf, iBufLen, 0)
***/
static int fnReadFromSock(SOCKET s,char FAR *buf,int len,int flags)
{
   fd_set fdread;
   struct timeval times;
   
   FD_ZERO(&fdread);
   FD_SET(s,&fdread);
   times.tv_sec = escapeTIME;
   times.tv_usec =1;
//   RETAILMSG(TME,("!!!!!!!!!!!读\r\n"));
   if(select(0,&fdread,NULL,NULL,&times)==SOCKET_ERROR)
   {
//	   RETAILMSG(TME,("!!!!!!!!!!!读超时\r\n"));
       return SOCKET_ERROR;
   }
   //  RETAILMSG(TME,("!!!!!!!!!!!select end\r\n"));
     return recv(s,buf,len,flags);
}
/****************************************************************
声明：static int fnReadFromSock(SOCKET s,char FAR *buf,int len,int flags);
参数：
IN szStr  -	 缓冲
返回值：是以句点结尾返回true,否则为false.
功能描述：从socket里读数据   //recv( s, szBuf, iBufLen, 0)
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
   RETAILMSG(TME,("!!!!!!!!!!!写\r\n"));
   if(select(0,NULL,&fdwrite,NULL,&times)==SOCKET_ERROR)
   {
	   RETAILMSG(TME,("!!!!!!!!!!!写超时\r\n"));
       return SOCKET_ERROR;
   }
   RETAILMSG(TME,("!!!!!!!!!!!select end\r\n"));
*/
     return send(s,buf,len,flags);
}
