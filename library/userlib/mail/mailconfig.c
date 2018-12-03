/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

/**************************************************
�ļ�˵���������ʼ���Ϣ�ĸ��ຯ�����������������ʼ�ʱ�����ʼ����жϵȡ�
          �ʼ�������Ϣ�����ͣ����ܶ���Ҫ����Ϣ,����Ŀǰ��������Ϊһ��������
		  ʹ�����ṩ,��֧�ֶ��û�����
�汾�ţ�1.0.0
����ʱ�ڣ�2003-3-12
���ߣ�������
�޸ļ�¼��
**************************************************/

#include <ewindows.h>
#include "MailBoxApi.h"
#include "EmailFormat.h"
#include "emailflags.h"
#include <ereg.h>

#define  CONFIG_REGISTRY
#undef   CONFIG_REGISTRY

/*************************************************
������BOOL SaveConfigInfo( CONFIGINFO * pConfig )
������
IN szConfig    -����������Ϣ���ļ���
����ֵ��TRUE  ����ɹ�
        FALSE ����ʧ��
	      
���������������������ݣ����������ݱ��浽�ı��ļ���ȥ
���ã�    �����ô��ڵ���
����ԭ�򣺵������ڱ�����ʱ�����ȱ����룬�������ݷ����ı�ʱ��ȥ�������������
          �������������ʱ��Ҫ�����û������������ʾ���ı�ԭ��������
		  �ڴ����о������������û���ݡ�
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
������BOOL GetConfigInfo( CONFIGINFO * config )
������
OUT  config    -���ص����ýṹ
����ֵ��TRUE  ��ȡ�ɹ�
        FALSE ��ȡʧ��
	      
�����������������ýṹ��Ϣ�����������ļ������ı��ļ���ʽ���ڣ�Ŀǰû�м���
���ã�  �ڳ�ʼ��POP3��SMPTʱ����Ҫ��ȡ������Ϣ
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
������void  InitEmailConfig(CONFIGINFO *  config)
������

OUT  config    -���ص����ýṹ
���أ� void   
������������ʼ�����ýṹ
���ã�    �ڶ�ȡ������Ϣ֮ǰ����Ҫ��ʼ���ýṹ
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
	   config->iSaveNum     = 5; //Ĭ�ϱ���5���ѷ��ʼ�
	   config->Interval     = 30;
	   config->iSizeLimit = 1;
}

/*************************************************
������void DeInitEmailConfig(CONFIGINFO * config)
������

OUT  config    -���ص����ýṹ
���أ� void   
�����������ͷ����ýṹ�ж�̬�������Դ
���ã�    �ڶ�ȡ������Ϣ֮����Ҫ�����ýṹ
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
������int GetNewEmailInfo( CHAR * IDList, int *pNewList )
������

IN  IDList    -server�ϵ��ʼ�ͷ��Ϣ
OUT pNewList  -�������ʼ����ʼ�ͷ
���أ� int    -�������ʼ��ĸ���   
�����������õ����ʼ��ĸ������ʼ�ͷ��Ϣ ������ǵ�һ��ʹ�ã���һ�����ļ���
          �����������еķ������ϵ��ʼ��������ʼ�,�����������99,������ʼ�����������99����ֻȡǰ99��˵
		  �����и�����ʱ��������������г���99���ʼ�����ÿ��ֻ�ܿ���ǰ99�����ʼ�,ֻ�ܷ�������
���ã�    �� FindNewEmailNum ����
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
	 while ((pos=StrPos(temp,' ')) && i<100)//�����100����һ������
	 {       
		  temp += pos ;
		  if ((pos = StrPos(temp,10))<2) //If it is the end of ID message.
		  {
		      break;
		  }
		  strncpy(buf,temp,pos-2);
		  //compare to Id list table
          RETAILMSG(TME,("iD: %s\r\n",buf));
		  TrimBlankStr(buf);//ɾ�����ַ�
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
������void RefreshIDList( int num , int * pNewList ) 
������

IN  IDList    -server�ϵ��ʼ�ͷ��Ϣ
OUT pNewList  -�������ʼ����ʼ�ͷ
���أ� int    -�������ʼ��ĸ���   
�����������õ����ʼ��ĸ������ʼ�ͷ��Ϣ ������ǵ�һ��ʹ�ã���һ�����ļ���
          �����������еķ������ϵ��ʼ��������ʼ�
���ã�    �� FindNewEmailNum ����
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
//�ʼ����ñ��浽ע���ļ���
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
������BOOL SaveConfigToReg( CONFIGINFO * pConfig )
������
IN :  pConfig  - ��Ҫ�����������Ϣ
���أ� TRUE    -����ɹ�
       FALSE   -����ʧ��
�������������µ����ñ��浽ע�����
���ã�
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
������BOOL GetConfigFromReg( CONFIGINFO * pConfig )
������
OUT :  pConfig  - ��õ��ʼ�������Ϣ
���أ� TRUE    -��ȡ�ɹ�
       FALSE   -��ȡʧ��
������������ע����л�ȡ�ʼ�[������Ϣ
���ã�
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
������void SaveEmailConfig( CONFIGINFO * pConfig )
������
OUT :  pConfig  - �����ʼ�������Ϣ
���أ� TRUE    -����ɹ�
       FALSE   -����ʧ��
��������������������Ϣ��ϵͳ��ȥ
���ã�
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
������void CreateEmailDirectory()
������
���أ� void
���������������ʼ�ϵͳ��ʱĿ¼��
���ã�
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
������BOOL GetEmailConfig( CONFIGINFO * pConfig )
������
OUT :  pConfig  - ����ʼ�������Ϣ
���أ� TRUE    -��ȡ�ɹ�
       FALSE   -��ȡʧ��
������������ϵͳ��ȡ������Ϣ
���ã�
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

