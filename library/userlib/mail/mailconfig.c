/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

/**************************************************
文件说明：配置邮件信息的各类函数声明。包括下载邮件时的新邮件的判断等。
          邮件配置信息，发送，接受都需要的信息,但是目前的配置是为一个单独的
		  使用者提供,不支持多用户操作
版本号：1.0.0
开发时期：2003-3-12
作者：满益文
修改记录：
**************************************************/

#include <ewindows.h>
#include "MailBoxApi.h"
#include "EmailFormat.h"
#include "emailflags.h"
#include <ereg.h>

#define  CONFIG_REGISTRY
#undef   CONFIG_REGISTRY

/*************************************************
声明：BOOL SaveConfigInfo( CONFIGINFO * pConfig )
参数：
IN szConfig    -包含配置信息的文件名
返回值：TRUE  保存成功
        FALSE 保存失败
	      
功能描述：保存配置数据，将配置数据保存到文本文件中去
调用：    被配置窗口调用
调用原则：当数据在被配置时，首先被读入，当有数据发生改变时才去更改里面的数据
          尤其在密码更改时，要求如果没有密码条，表示不改变原来的密码
		  在窗口中尽量保持密码框没数据。
************************************************/
BOOL SaveConfigInfo( CONFIGINFO * pConfig )
{
    HANDLE hFile ; 
	CHAR   szText[256];
    DWORD  dwWrite;

	if(!pConfig)
	{
        RETAILMSG(TMAIL,("ERROR!!!!!!!!!!!occur in [mailconfig.c\\saveconfiginfo()]!"));
		return FALSE; 
	}
	hFile = CreateFile(szMlcfgConfigName,
               GENERIC_WRITE,
		       FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		       OPEN_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);//if not existing ,create one.
    if ( hFile == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}
	if ( pConfig->smtpserver )
	{
		sprintf( szText,TEXT("SMTP:%s\r\n"),pConfig->smtpserver );
	    WriteFile( hFile,szText, strlen(szText),&dwWrite,NULL);
		strcpy( szText, TEXT("SPORT:25\r\n") );
	    WriteFile( hFile,szText,strlen(szText),&dwWrite,NULL);
	}
	
	if ( pConfig->pop3server )
	{
        sprintf( szText,TEXT("POP3:%s\r\n"),pConfig->pop3server );
	    WriteFile( hFile,szText, strlen(szText),&dwWrite,NULL);
		strcpy( szText, TEXT("PPORT:110\r\n") );
	    WriteFile( hFile,szText,strlen(szText),&dwWrite,NULL);
	}
	
	if ( pConfig->account )
	{
        sprintf( szText,TEXT("ACCOUNT:%s\r\n"),pConfig->account );
	    WriteFile( hFile,szText, strlen(szText),&dwWrite,NULL);
	}
	if ( pConfig->password )
	{
		sprintf( szText, TEXT("PASSWORD:%s\r\n"),pConfig->password );
	    WriteFile( hFile,szText,strlen(szText),&dwWrite,NULL);
	}
	if ( pConfig->user )
	{
		sprintf( szText,TEXT("USER:%s\r\n"),pConfig->user );
		WriteFile( hFile,szText,strlen(szText),&dwWrite,NULL);
	}
	if ( pConfig->From )
	{
		sprintf( szText,TEXT("EMAIL:%s\r\n"),pConfig->From );
		WriteFile( hFile, szText,strlen(szText),&dwWrite ,NULL);
	}
	if ( pConfig->Reply )
	{
		sprintf( szText,TEXT("REPLY:%s\r\n"),pConfig->Reply );
		WriteFile( hFile, szText,strlen(szText),&dwWrite ,NULL);
	}
	sprintf( szText,TEXT("BACKUP:%d\r\n"),pConfig->iSaveNum );
	WriteFile( hFile, szText,strlen(szText),&dwWrite,NULL );
	
	sprintf( szText,TEXT("SERVERSAVE:%d\r\n"),pConfig->bDel?1:0 );
	WriteFile( hFile, szText,strlen(szText),&dwWrite,NULL );
	
	sprintf( szText,TEXT("LIMIT:%d\r\n"),pConfig->iSizeLimit );
	WriteFile( hFile, szText,strlen(szText),&dwWrite,NULL );

	sprintf( szText,TEXT("DOWNALL:%d\r\n"),pConfig->bDownAll?1:0 );
	WriteFile( hFile, szText,strlen(szText),&dwWrite,NULL );
	
	if ( pConfig->Company )
	{
		sprintf(szText,TEXT("Company:%s\r\n"),pConfig->Company);
		WriteFile( hFile,szText,strlen(szText),&dwWrite,NULL );
	}
	sprintf( szText,TEXT("Interval:%d\r\n"),pConfig->Interval);
	WriteFile( hFile,szText,strlen(szText),&dwWrite,NULL );
	CloseHandle( hFile );
    return TRUE;
}	

/*************************************************
声明：BOOL GetConfigInfo( CONFIGINFO * config )
参数：
OUT  config    -返回的配置结构
返回值：TRUE  读取成功
        FALSE 读取失败
	      
功能描述：读出配置结构信息，由于配置文件是以文本文件形式存在，目前没有加密
调用：  在初始化POP3或SMPT时，需要读取配置信息
************************************************/
BOOL GetConfigInfo( CONFIGINFO * config )
{
    HANDLE hFile;
	TCHAR *szTemp;
	TCHAR *szLine;
	TCHAR szKey[100];
	DWORD dwRead;
	int   iCurPos;
	int   iLen ;

	if(!config)
	{
        RETAILMSG(TMAIL,("ERROR!!!!!!!!!!!occur in [mailconfig.c\\GetConfigInfo()]!"));
		return FALSE;
	}	
    hFile = CreateFile(szMlcfgConfigName,
               GENERIC_READ,
		       FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		       OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, 0);//if not existing ,create one.

      if (hFile == INVALID_HANDLE_VALUE)
		 return FALSE ;
	  iCurPos = 0;
	  szLine = malloc(101);
	  memset( szLine, 0, 101);
	  while (ReadVirtualLine2( hFile, szLine, (int*)&dwRead,&iCurPos))
	  { 
	      if (dwRead == 2)
			  break;
		  memset( szKey, 0, 100);
		  if ( FindStringBeforeCh(szLine,':',szKey))
		  {
			  szTemp = szLine+strlen(szKey)+1;
			  iLen = strlen( szTemp ) + 2;
			  if (stricmp( szKey, TEXT("POP3"))==0)
			  {				  
				  config->pop3server = malloc (iLen) ;
				  strcpy(config->pop3server, szTemp);
			  }
			  else if (stricmp( szKey, TEXT("ACCOUNT"))==0)
			  {
			      config->account = malloc (iLen );
				  strcpy( config->account, szTemp);
			  }
			  else if (stricmp( szKey, TEXT("PASSWORD"))==0)
			  {
			      config->password = malloc (iLen );
				  strcpy( config->password,szTemp);
			  }
			  else if (stricmp( szKey, TEXT("EMAIL"))==0) 
			  {
				  config->From = malloc( iLen );
				  strcpy ( config->From, szTemp );
			  }
			  else if (stricmp( szKey ,TEXT("SMTP"))==0)
			  {
				  config->smtpserver = malloc( iLen );
				  strcpy ( config->smtpserver, szTemp );
			  }
			  else if (stricmp( szKey, TEXT("SPORT"))==0)
			  {
				  config->smtpport = atoi(szTemp);
			  }
			  else if (stricmp( szKey ,TEXT("PPORT"))==0)
			  {
                  config->pop3port = atoi(szTemp);
			  }
			  else if (stricmp( szKey, TEXT("USER"))==0)
			  {
				  config->user = malloc( iLen );
				  strcpy ( config->user, szTemp );
			  }
			  else if (stricmp( szKey,TEXT("DOWNALL"))== 0 )
			  {
				  config->bDownAll = (BOOL)atoi(szTemp);
			  }
			  else if (stricmp( szKey,TEXT("REPLY"))== 0 )
			  {
				  config->Reply = malloc( iLen );
				  strcpy ( config->Reply, szTemp );
			  }
			  else if (stricmp( szKey,TEXT("BACKUP"))== 0 )
			  {
				  config->iSaveNum = atoi(szTemp);
			  } 
			  else if (stricmp( szKey, TEXT("Company")) == 0 )
			  {
				  config->Company = malloc( iLen );
				  strcpy( config->Company,szTemp );
			  }
			  else if (stricmp( szKey,TEXT("Interval")) == 0 )
			  {
				  config->Interval = atoi(szTemp);				  
			  }
			  else if ( stricmp( szKey,TEXT("LIMIT")) == 0 )
				  config->iSizeLimit = atoi( szTemp );
			  else if ( stricmp( szKey,TEXT("SERVERSAVE")) == 0)
				  config->bDel = (BOOL)atoi( szTemp );
		  }
          memset(szLine,0,101);
	  }    
	  CloseHandle(hFile);
	  free(szLine);
      return TRUE ;
}
/*************************************************
声明：void  InitEmailConfig(CONFIGINFO *  config)
参数：

OUT  config    -返回的配置结构
返回： void   
功能描述：初始化配置结构
调用：    在读取配置信息之前，需要初始化该结构
************************************************/
void  InitEmailConfig(CONFIGINFO *  config)
{
	   if(!config)
	   {
          RETAILMSG(TMAIL,("ERROR!!!!!!!!!!!occur in [mailconfig.c\\InitEmailConfig()]!"));
		  return;
	   }	
       config->account = NULL;
       config->password = NULL;
	   config->pop3server = NULL;
	   config->smtpserver = NULL;
	   config->pop3port = 110;
	   config->smtpport = 25;
	   config->From = NULL;	   
	   config->Reply   = NULL;
	   config->Company = NULL;
	   config->user = NULL;
	   config->bDownAll     = FALSE;
	   config->bDel         = FALSE;
	   config->iSaveNum     = 5; //默认备分5份已发邮件
	   config->Interval     = 30;
	   config->iSizeLimit = 1;
}

/*************************************************
声明：void DeInitEmailConfig(CONFIGINFO * config)
参数：

OUT  config    -返回的配置结构
返回： void   
功能描述：释放配置结构中动态分配的资源
调用：    在读取配置信息之后，需要析构该结构
************************************************/
void DeInitEmailConfig(CONFIGINFO * config)
{
	  if(!config)
	  {
         RETAILMSG(TMAIL,("ERROR!!!!!!!!!!!occur in [mailconfig.c\\DeInitEmailConfig()]!"));
	     return;
	  }	
 
     if (config->account)
		 free(config->account);
	 if (config->password)
		 free(config->password);
	 if (config->pop3server)
		 free(config->pop3server);
	 if (config->smtpserver)
		 free(config->smtpserver);
	 if (config->From)
		 free(config->From);
	 if (config->user)
		 free(config->user);
	 if (config->Reply)
		 free(config->Reply);
     if (config->Company )
		 free(config->Company);
	 
}

/*************************************************
声明：int GetNewEmailInfo( CHAR * IDList, int *pNewList )
参数：

IN  IDList    -server上的邮件头信息
OUT pNewList  -返回新邮件的邮件头
返回： int    -返回新邮件的个数   
功能描述：得到新邮件的个数和邮件头信息 ，如果是第一次使用，则建一个空文件，
          这样将把所有的服务器上的邮件当做新邮件,这里的限制是99,如果有邮件的数量超过99，则只取前99再说
		  这里有个问题时如果网络上正好有超过99个邮件，则每次只能拷贝前99个新邮件,只能分批拷出
调用：    在 FindNewEmailNum 调用
************************************************/
int GetNewEmailInfo( CHAR * IDList, int *pNewList )
{
     CHAR     *temp;
	 CHAR     buf[71];
     int      pos , index = 1;
	 int      i= 0 ;
	 HANDLE   hIDTable, hTempTable ;
     FINDSTRUCT  fs ;
	 FIELDITEM  fd ;
	 RETAILMSG(TME,(IDList));
	 pos=StrPos(IDList,10);//skip to the second line to get the message id.
     if (pos=0)
	 {
       return 0;
	 }
	 if (( hIDTable = CreateTable( szMlcfgExistIDTable,GENERIC_WRITE|GENERIC_READ,0,NULL,OPEN_ALWAYS, 1 )) == INVALID_HANDLE_VALUE )
	 {
		 SetCurrentError ( SYSTEM_ERROR_UNKOWN ); 
		 return 0 ;
	 }
	 if ((hTempTable  =  CreateTable( szMlcfgTempIDTable, GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS, 1 )) == INVALID_HANDLE_VALUE )
	 {
		 SetCurrentError ( SYSTEM_ERROR_UNKOWN ); 
		 RETAILMSG(TME,("TempID open wrong... \r\n"));
		 CloseHandle (hIDTable ) ;
		 return 0 ;
	 }
	 temp   = IDList+pos; // set position to the second line.
	 memset(buf,0,71);
     RETAILMSG(TME,(temp));
	 while ((pos=StrPos(temp,' ')) && i<100)//这里的100就是一个限制
	 {       
		  temp += pos ;
		  if ((pos = StrPos(temp,10))<2) //If it is the end of ID message.
		  {
		      break;
		  }
		  strncpy(buf,temp,pos-2);
		  //compare to Id list table
          RETAILMSG(TME,("iD: %s\r\n",buf));
		  TrimBlankStr(buf);//删除空字符
		  if ( strlen(buf) == 0 )
			  continue;
		  if ( INVALID_HANDLE_VALUE == FindFirstRecord( hIDTable, 1,buf, strlen(buf), &fs ) )
//		  if (INVALID_HANDLE_VALUE == ( FindFirstRecord ( hIDTable, 1, buf, strlen( buf ), &fs )))
		  {
              RETAILMSG(TME,("Get a new ID %d ", index));
		      pNewList [i] = index ;
			  i++;  
		  }
		  temp+=pos;
          index++;
		  fd.lpData = buf ;
		  fd.size   = 70 ;
		  AppendRecord ( hTempTable, &fd ) ;
	 }
	 CloseTable ( hIDTable ) ;
	 CloseTable ( hTempTable );
     return i ;
}

/*************************************************
声明：void RefreshIDList( int num , int * pNewList ) 
参数：

IN  IDList    -server上的邮件头信息
OUT pNewList  -返回新邮件的邮件头
返回： int    -返回新邮件的个数   
功能描述：得到新邮件的个数和邮件头信息 ，如果是第一次使用，则建一个空文件，
          这样将把所有的服务器上的邮件当做新邮件
调用：    在 FindNewEmailNum 调用
************************************************/
void RefreshIDList( int num , int * pNewList ) 
{
	HANDLE  hTempTable ;
	int i ;
	
	if ((hTempTable  =  CreateTable( szMlcfgTempIDTable, GENERIC_WRITE|GENERIC_READ,0,NULL,OPEN_ALWAYS, 1 )) == INVALID_HANDLE_VALUE )
	{
		SetCurrentError ( SYSTEM_ERROR_UNKOWN ); 
		return ;
	}

    for ( i = num-1 ;i>=0 ;i-- )
	{
	    if ( pNewList[i] != -1 )
		{//
			RETAILMSG(TME,("Delete %d \r\n",pNewList[i]));
			SetRecordPointer( hTempTable, pNewList[i], SRP_BEGIN );
		    DeleteRecord( hTempTable );
		}
	}
	CloseTable ( hTempTable );
	//Delete orignal file rename the temp file.
    //DeleteFile( szMlcfgExistIDTable );
	if (DeleteAndRenameFile ( szMlcfgExistIDTable , szMlcfgTempIDTable ) == FALSE )
	{
       RETAILMSG(TME,("Can not delete szTempID or ExistID\r\n"));
	}
}
//邮件配置保存到注册表的键字
#define RK_CONTROLPANEL_EMAIL (TEXT("ControlPanel\\EASYBOX"))

#define EMAIL_POP3SERVER  (TEXT("POP3"))
#define EMAIL_SMTPSERVER  (TEXT("SMTP"))
#define EMAIL_POP3PORT    (TEXT("PPORT"))
#define EMAIL_SMTPPORT    (TEXT("PSMTP"))

#define EMAIL_ACCOUNT     (TEXT("Account"))
#define EMAIL_PASSWORD    (TEXT("Password"))
#define EMAIL_USER        (TEXT("User"))
#define EMAIL_Company     (TEXT("Company"))
#define EMAIL_EMAIL       (TEXT("mailAddr"))
#define EMAIL_REPLY       (TEXT("ReplyAddr"))
#define EMAIL_DOWNALL     (TEXT("DownAll"))
#define EMAIL_INTERVAL    (TEXT("Interval"))
#define EMAIL_BACKUP      (TEXT("Backup"))
#define EMAIL_SERVERSAVE  (TEXT("SERVERSAVE"))
#define EMAIL_SIZELIMIT   (TEXT("SIZELIMIT"))
/*************************************************
声明：BOOL SaveConfigToReg( CONFIGINFO * pConfig )
参数：
IN :  pConfig  - 需要保存的设置信息
返回： TRUE    -保存成功
       FALSE   -保存失败
功能描述：将新的配置保存到注册表中
调用：
************************************************/
BOOL SaveConfigToReg( CONFIGINFO * pConfig )
{
    HKEY    hKey;
	BOOL    bRet = FALSE;
    DWORD   dwDisp;
    int     iLen;
	CHAR    ch ;
    
	ch = 0;
	if ( RegCreateKeyEx( HKEY_SOFTWARE_ROOT,RK_CONTROLPANEL_EMAIL,0,NULL,0,
                        0, NULL, &hKey, &dwDisp)==ERROR_SUCCESS )
	{
		if ( pConfig->pop3server )
		{	iLen = strlen( pConfig->pop3server ) + 1;
		    RegSetValueEx( hKey,EMAIL_POP3SERVER,NULL,REG_SZ,(LPBYTE)pConfig->pop3server,iLen );
		}
		else 
			RegSetValueEx( hKey,EMAIL_POP3SERVER,NULL,REG_SZ,(LPBYTE)&ch,1 );
		
		RegSetValueEx( hKey,EMAIL_POP3PORT,NULL,REG_DWORD,(LPBYTE)&pConfig->pop3port,sizeof(pConfig->pop3port) );
		
		if ( pConfig->smtpserver )
		{
			iLen = strlen( pConfig->smtpserver ) +1;
			RegSetValueEx( hKey,EMAIL_SMTPSERVER,NULL,REG_SZ,(LPBYTE)pConfig->smtpserver,iLen );
		}
		else
            RegSetValueEx( hKey,EMAIL_SMTPSERVER,NULL,REG_SZ,(LPBYTE)&ch,1 );
		
		RegSetValueEx( hKey,EMAIL_POP3PORT,NULL,REG_DWORD,(LPBYTE)&pConfig->smtpport,sizeof(pConfig->smtpport ) );
	
		if ( pConfig->account )
		{
			iLen = strlen( pConfig->account ) + 1;
			RegSetValueEx( hKey,EMAIL_ACCOUNT	,NULL,REG_SZ,(LPBYTE)pConfig->account,iLen );
		}
		else
			RegSetValueEx( hKey,EMAIL_ACCOUNT	,NULL,REG_SZ,(LPBYTE)&ch,1 );
		
		if ( pConfig->password )
		{
			iLen = strlen( pConfig->password ) + 1;
			RegSetValueEx( hKey,EMAIL_PASSWORD	,NULL,REG_SZ,(LPBYTE)pConfig->password,iLen );
		}
		else
			RegSetValueEx( hKey,EMAIL_PASSWORD	,NULL,REG_SZ,(LPBYTE)&ch,1 );
		
		if ( pConfig->user )
		{
			iLen = strlen( pConfig->user ) + 1;
			RegSetValueEx( hKey,EMAIL_USER,NULL,REG_SZ,(LPBYTE)pConfig->user,iLen );
		}
		else
			RegSetValueEx( hKey,EMAIL_USER,NULL,REG_SZ,(LPBYTE)&ch,1 );
		if ( pConfig->Company )
		{
			iLen = strlen( pConfig->Company ) + 1;
			RegSetValueEx( hKey,EMAIL_Company,NULL,REG_SZ,(LPBYTE)pConfig->Company,iLen );
		}
		else
            RegSetValueEx( hKey,EMAIL_Company,NULL,REG_SZ,(LPBYTE)&ch,1 );
		if ( pConfig->From )
		{
		    iLen = strlen( pConfig->From ) + 1;
			RegSetValueEx( hKey,EMAIL_EMAIL,NULL,REG_SZ,(LPBYTE)pConfig->From,iLen );
		}
		else
			RegSetValueEx( hKey,EMAIL_EMAIL,NULL,REG_SZ,(LPBYTE)&ch,1 );

		if ( pConfig->Reply )
		{
			iLen = strlen( pConfig->Reply ) + 1;
			RegSetValueEx( hKey,EMAIL_REPLY,NULL,REG_SZ,(LPBYTE)pConfig->Reply,strlen(pConfig->Reply)+1 );
		}
        else
			RegSetValueEx( hKey,EMAIL_REPLY,NULL,REG_SZ,(LPBYTE)&ch,1 );
       
		RegSetValueEx( hKey,EMAIL_INTERVAL,NULL,REG_DWORD,(LPBYTE)&pConfig->Interval,sizeof(pConfig->Interval) );
		RegSetValueEx( hKey,EMAIL_DOWNALL,NULL,REG_DWORD,(LPBYTE)&pConfig->bDownAll,sizeof(pConfig->bDownAll) );
		RegSetValueEx( hKey,EMAIL_BACKUP,NULL,REG_DWORD,(LPBYTE)&pConfig->iSaveNum,sizeof(pConfig->iSaveNum) );
		RegSetValueEx( hKey,EMAIL_SIZELIMIT,NULL,REG_DWORD,(LPBYTE)&pConfig->iSizeLimit,sizeof(pConfig->iSizeLimit) );
        RegSetValueEx( hKey,EMAIL_SERVERSAVE,NULL,REG_DWORD,(LPBYTE)&pConfig->bDel,sizeof(pConfig->bDel) );
	    bRet = TRUE;
		RegCloseKey( hKey );
	}
	return bRet;
}

/*************************************************
声明：BOOL GetConfigFromReg( CONFIGINFO * pConfig )
参数：
OUT :  pConfig  - 获得的邮件配置信息
返回： TRUE    -获取成功
       FALSE   -获取失败
功能描述：从注册表中获取邮件[配置信息
调用：
************************************************/
BOOL GetConfigFromReg( CONFIGINFO * pConfig )
{
	HKEY    hKey;
	BOOL    bRet = FALSE;
    DWORD   dwDisp;
	CHAR    szTemp[100];
	DWORD   dwRead;
	DWORD   dwType;
	if ( RegCreateKeyEx( HKEY_SOFTWARE_ROOT,RK_CONTROLPANEL_EMAIL,0,NULL,0,
		0, NULL, &hKey, &dwDisp)==ERROR_SUCCESS )
	{
        memset( szTemp ,0,100 );
	    dwRead = 99;
		dwType = REG_SZ;
        if ( ERROR_SUCCESS == RegQueryValueEx( hKey,EMAIL_POP3SERVER,NULL,(LPDWORD)&dwType,(LPBYTE)&szTemp,&dwRead ) )
		{
			pConfig->pop3server = malloc( dwRead );
			strcpy( pConfig->pop3server, szTemp );
		}
		dwType = REG_DWORD;
		RegQueryValueEx( hKey,EMAIL_POP3PORT,NULL,(LPDWORD)&dwType,(LPBYTE)&pConfig->pop3port,&dwRead );
		memset( szTemp ,0,100 );
		dwRead = 99;
		dwType = REG_SZ;
		if ( ERROR_SUCCESS == RegQueryValueEx( hKey,EMAIL_SMTPSERVER,NULL,(LPDWORD)&dwType,(LPBYTE)&szTemp[0],&dwRead ) )
		{
			pConfig->smtpserver = malloc (dwRead );
			strcpy( pConfig->smtpserver,szTemp );
		}
		
		memset( szTemp ,0,100 );
		dwRead = 99;
		if ( ERROR_SUCCESS == RegQueryValueEx( hKey,EMAIL_PASSWORD,NULL,(LPDWORD)&dwType,(LPBYTE)&szTemp[0],&dwRead ) )
		{
			pConfig->password = malloc (dwRead );
			strcpy( pConfig->password,szTemp );
		}

		memset( szTemp ,0,100 );
		dwRead = 99;
		if ( ERROR_SUCCESS == RegQueryValueEx( hKey,EMAIL_ACCOUNT,NULL,(LPDWORD)&dwType,(LPBYTE)&szTemp[0],&dwRead ) )
		{
			pConfig->account = malloc (dwRead );
			strcpy( pConfig->account,szTemp );
		}

		dwType = REG_DWORD;
		RegQueryValueEx( hKey,EMAIL_POP3PORT,NULL,(LPDWORD)&dwType,(LPBYTE)&pConfig->smtpport,&dwRead );
		memset( szTemp ,0,100 );
		dwRead = 99;
		dwType = REG_SZ;
		if ( ERROR_SUCCESS  == RegQueryValueEx( hKey,EMAIL_USER,NULL,(LPDWORD)&dwType,(LPBYTE)&szTemp[0],&dwRead ) )
		{
           pConfig->user = malloc (dwRead );
			strcpy( pConfig->user,szTemp );
		}
        memset( szTemp ,0,100 );
		dwRead = 99;
		
		if ( ERROR_SUCCESS  == RegQueryValueEx( hKey,EMAIL_Company,NULL,(LPDWORD)&dwType,(LPBYTE)&szTemp[0],&dwRead ) )
		{
			pConfig->Company = malloc (dwRead );
			strcpy( pConfig->Company,szTemp );
		}
        memset( szTemp ,0,100 );
		dwRead = 99;
		if ( ERROR_SUCCESS  == RegQueryValueEx( hKey,EMAIL_EMAIL,NULL,(LPDWORD)&dwType,(LPBYTE)&szTemp[0],&dwRead ) )
		{
			pConfig->From = malloc( dwRead );
			strcpy( pConfig->From,szTemp );
		}
		memset( szTemp ,0,100 );
		dwRead = 99;
		if ( ERROR_SUCCESS  == RegQueryValueEx( hKey,EMAIL_REPLY,NULL,(LPDWORD)&dwType,(LPBYTE)&szTemp,&dwRead ) )
		{
			pConfig->Reply = malloc( dwRead );
			strcpy( pConfig->Reply,szTemp );
		}
		dwType = REG_DWORD;
		dwRead = 4;
		RegQueryValueEx( hKey,EMAIL_INTERVAL,NULL,(LPDWORD)&dwType,(LPBYTE)&pConfig->Interval,&dwRead );
		RegQueryValueEx( hKey,EMAIL_DOWNALL,NULL,(LPDWORD)&dwType,(LPBYTE)&pConfig->bDownAll,&dwRead );
		RegQueryValueEx( hKey,EMAIL_BACKUP,NULL,(LPDWORD)&dwType,(LPBYTE)&pConfig->iSaveNum,&dwRead );
		RegQueryValueEx( hKey,EMAIL_SERVERSAVE,NULL,(LPDWORD)&dwType,(LPBYTE)&pConfig->bDel,&dwRead );	
		RegQueryValueEx( hKey,EMAIL_SIZELIMIT,NULL,(LPDWORD)&dwType,(LPBYTE)&pConfig->iSizeLimit,&dwRead );
		RegCloseKey( hKey );
		bRet = TRUE;
	}

    return bRet;
}

/*************************************************
声明：void SaveEmailConfig( CONFIGINFO * pConfig )
参数：
OUT :  pConfig  - 保存邮件配置信息
返回： TRUE    -保存成功
       FALSE   -保存失败
功能描述：保存配置信息到系统中去
调用：
************************************************/
void SaveEmailConfig( CONFIGINFO * pConfig )
{
#ifdef  CONFIG_REGISTRY
	SaveConfigToReg( pConfig );
#else
	SaveConfigInfo( pConfig );
#endif
}
/*************************************************
声明：void CreateEmailDirectory()
参数：
返回： void
功能描述：创建邮件系统临时目录。
调用：
************************************************/
void CreateEmailDirectory()
{
    DWORD d;
	if ( (d = GetFileAttributes( szEmailRoots )) == -1 || (d | FILE_ATTRIBUTE_DIRECTORY) == 0  )
		CreateDirectory( szEmailRoots,NULL );
	if ( (d = GetFileAttributes( szEmaildirs )) == -1 || (d | FILE_ATTRIBUTE_DIRECTORY) == 0  )
		CreateDirectory( szEmaildirs,NULL );
	if ( (d = GetFileAttributes( szEmailCfgDir )) == -1 || (d | FILE_ATTRIBUTE_DIRECTORY) == 0  )
		CreateDirectory( szEmailCfgDir,NULL );
	if ( (d = GetFileAttributes( szSentBoxs )) == -1 || (d | FILE_ATTRIBUTE_DIRECTORY) == 0  )
		CreateDirectory( szSentBoxs,NULL );
	if ( (d = GetFileAttributes( szReceiveBoxs )) == -1 || (d | FILE_ATTRIBUTE_DIRECTORY) == 0  )
		CreateDirectory( szReceiveBoxs,NULL );
	if ( (d = GetFileAttributes( szDraftBoxs )) == -1 || (d | FILE_ATTRIBUTE_DIRECTORY) == 0  )
		CreateDirectory( szDraftBoxs,NULL );
    if ( (d = GetFileAttributes( szEmailCfgTempDir )) == -1 || (d | FILE_ATTRIBUTE_DIRECTORY) == 0  )
		CreateDirectory( szEmailCfgTempDir,NULL );
}

/*************************************************
声明：BOOL GetEmailConfig( CONFIGINFO * pConfig )
参数：
OUT :  pConfig  - 获得邮件配置信息
返回： TRUE    -获取成功
       FALSE   -获取失败
功能描述：从系统获取配置信息
调用：
************************************************/
BOOL GetEmailConfig( CONFIGINFO * pConfig )
{
	CreateEmailDirectory();
#ifdef  CONFIG_REGISTRY   
   return GetConfigFromReg( pConfig );	
#else
   return GetConfigInfo( pConfig );
#endif
}

