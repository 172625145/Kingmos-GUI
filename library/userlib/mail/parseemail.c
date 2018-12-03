/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

/**************************************************
文件说明：邮件解析，需要将邮件从一个文件中读出来。以正确的格式,目前只支持一些通用格式
          如Base64,Quoted-printable ，html 等格式.
版本号：1.0.0
开发时期：2003-3-12
作者：满益文
修改记录：增加了 HTML+ATTACHMENT的多功能邮件的处理，此时可能有多个边界字符串存在。2003-09-18 by 满益文
**************************************************/
#include <ewindows.h>
#include <EmailFormat.h>
#include <mailboxapi.h>
#include <BaseQuoted.h>
#include "emailflags.h"

/*************************************************
声明：BOOL CheckValidEmail( LPTSTR szTo )
参数：
IN      szTo    -邮件地址
返回：  BOOL    TRUE  有效的邮件地址
                FALSE 无效的邮件地址
功能描述：在发邮件之前要，简要地检查以下邮件地址是否有效，如果不是，则
          不会发送，并提醒USER 确认邮件地址，一般的邮件都是带@符号的字符串
		  并且邮件发送时支持多个同时发送，一般以空格或分号隔离
引用：    邮件发送前要检查
************************************************/
BOOL CheckValidEmail( LPTSTR szTo )
{
    if (StrPos( szTo, '@' ) == 0 )//find '@' successfully.
		return FALSE;
	else 
		return TRUE;
}

/*************************************************
声明：void ParseString (CHAR * szSource,CHAR *szDest)
参数：
IN      szSource    -源字符串
IN      szDest      -转换后的字符串
返回：void
功能描述：将邮件头中的一些信息解释出来，因为有些邮件头也进行了加密，一般
          用 Base64 ,Quoted-printable 两种方式来加密，解密
引用：在解释邮件头(ParseEmailHeader)时需要调用
************************************************/
void ParseString (CHAR * szSource,CHAR *szDest)
{
	int i = 0, iCur;
	int iPos = 0;
	CHAR ch,ch1,ch2;
	int iLength = strlen(szSource);
	CHAR *szEncode,*szDecode;
	while (i < iLength)
	{
		ch1 = szSource[i++];
		ch2 = szSource[i++];

		if (ch1 == '=' && ch2 == '?')
		{
			//寻找‘？’
			while ((ch = szSource[i++])!='?' && ch!=0 );//不解析字符集
			
			iCur=0;
			if (ch!=0)
			{//找到关键字，=?
				szEncode = malloc(100);
				szDecode = malloc(100);
				memset( szEncode, 0, 100);
				memset( szDecode, 0, 100);
				switch ((ch = szSource[i++]))
				{
				case 'Q':
				case 'q':
					i++;//跳过一个?
					while ((ch = szSource[i++])!='?' && ch!=0)
						szEncode[iCur++]=ch;
					DecodeQuoted(szEncode,szDecode);
					strcat(szDest,szDecode);
					i++;
                    iPos+=strlen( szDecode );
					break;
				case 'B':
				case 'b':
					i++;//跳过一个?
					while ((ch = szSource[i++])!='?' && ch!=0 )
						szEncode[iCur++]=ch;
					DecodeBase64(szEncode,szDecode);
					strcat( szDest, szDecode);
					i++;
					iPos+= strlen( szDecode );
					break;
				default:
                    //本应该不解析
					strcpy( szDest,szEncode);
				}
				free( szEncode );
    			free( szDecode );
			}
		}
		else
		{
			szDest[iPos++] = ch1;
			i--;
		}
	}
	return;
}
static void AdjustPosition( LPCTSTR szLine,int * pFilePos ,int nLineLength )
{//这是一个补充，因为在以前没有考虑到在一行中有多个描述段，因此只能这样使用，
//如果一行有多个描述符的话，使指针指向下一个描述符
	int iPos;
	int i,iLen;
	CHAR ch;

	iLen = strlen( szLine );
	iPos = StrPos( (LPTSTR)szLine,';' );
	if ( iPos == 0 )
		return;	
	i = iPos;
	while ( ch=szLine[i] && (ch == 9 || ch == 20) ) i++;
    if ( i == iLen ) return;
	if ( iPos != 0 )
		*pFilePos = *pFilePos - nLineLength + iPos ;
}
/*************************************************
声明：void GetKeyString(HANDLE hFile,int *pFilePos,LPTSTR szLine, PEmailPart part, PEmailFormat  pMail)
参数：
IN      hFile    -文件句柄
IN/OUT  pFilePos -文件当前的读写位置
IN      szLine   -字符串
IN      part     -邮件PART 的结构
IN/OUT  pMail    -邮件格式的数据结构
返回：void
功能描述：根据邮件协议，每一个的PART的部分都有写描写该PART的信息，通过这些
        信息才可以解释出该部分，详细请参考相关的协议和EmailFormat.h 中的EmailPart结构说明
************************************************/
void GetKeyString(HANDLE hFile,int *pFilePos,LPTSTR szLine, PEmailPart part, PEmailFormat  pMail)
{
	int i;
	CHAR szKey[100];
	CHAR *ALine;
    CHAR * temp= szLine;        
    memset(szKey,0,100);
	
    if (FindStringBeforeCh(szLine,58,szKey))//58 means  ':'
	{
		if (stricmp(szKey,TEXT("Content-Type"))==0)
		{
			temp+=strlen(szKey);
			temp++;
			memset(szKey,0,100);
			if (FindStringBeforeCh(temp,47,szKey)) //47 means '/'
			{
				temp+=strlen(szKey);
				temp++;
				TrimString(szKey);
				strcpy(part->type,szKey);
				memset(szKey,0,100);
				if (FindStringBeforeCh(temp,59,szKey)) //59 means ';'
				{
					TrimString(szKey);
					strcpy(part->subtype,szKey);
				}
			}
			if (stricmp(part->type,TEXT("Multipart"))==0)
			{ //means It's a boundary .
				ALine=malloc(102);
				memset(ALine,0,102);
				for (;;){
                if(ReadVirtualLine2(hFile,ALine,&i,pFilePos))
				{ 
					memset(szKey,0,100);
					if (FindStringBeforeCh(ALine,'=',szKey))
					{
						TrimString(szKey);
						if (stricmp(szKey,TEXT("boundary"))==0)
						{ 
                            if ( (i=StrPos(ALine,34))!=0)//first '"' pos.
                            {
								temp=ALine+i;
								if ((i=StrPos(temp,34))>0)
								{
									memset(szKey,0,100);
									strncpy(szKey,temp,i-1);
									if (pMail->boundary2 == NULL)
									{   //当我们判断时总是使用boundary1 和 boundary2 来作为判断的标志
										pMail->boundary2 = malloc(100);
										strcpy(pMail->boundary2,pMail->boundary1);
									    strcpy(pMail->boundary1,szKey);
									    part->param = TRUE;
									}
									else
									{
										if ( pMail->boundary3 == NULL ){
											pMail->boundary3 = malloc(100);
											//此时，boundary2 是已经被交换过一次的原boundary1,因此需要保存
											strcpy( pMail->boundary3,pMail->boundary2 );
											strcpy( pMail->boundary2,pMail->boundary1 );
											strcpy( pMail->boundary1,szKey );
										}
										part->param = TRUE;
									}
								}
							}
						}
					}
					if (StrPos( ALine,';' )==0)					
						   break;
					
				}else 
					break;
				}
				free(ALine);
			}
			else if ( stricmp( part->type,TEXT("application"))==0 || stricmp( part->type,TEXT("image"))==0 )
			{
                ALine=malloc(102);
				memset(ALine,0,102);
				for(;;){
                if(ReadVirtualLine2(hFile,ALine,&i,pFilePos))
				{ 
					memset(szKey,0,100);
					if (FindStringBeforeCh(ALine,'=',szKey))
					{
						TrimString(szKey);
						if (stricmp(szKey,TEXT("name"))==0 || stricmp( szKey,TEXT("filename"))==0)
						{ //找NAME
                            i=StrPos(ALine,34);
							temp=ALine+i;
							if (i=StrPos(temp,34))
							{
								CHAR szTempFileName[40]; 
								memset( szTempFileName, 0,40);
								strncpy(szTempFileName,temp,i-1);//get the name.
								memset( part->name,0, 40);
								ParseString( szTempFileName,part->name);
							}
						}
					}
					if ( StrPos( ALine,';')== 0 )
						break;
				}
				else
					break;
				}
				free(ALine);
			}
		}
		else if (stricmp(szKey,TEXT("Content-Transfer-Encoding"))==0)
		{
			temp+=strlen(szKey);
			temp++;
			TrimString(temp);
			strcpy(part->encoding,temp);
		}
		else if (stricmp(szKey,TEXT("Content-Disposition"))==0)
		{
			temp+=strlen(szKey);
			temp++;
			TrimString(temp);            
			strncpy(part->disposing,temp,StrPos(temp,';')-1);
			//get the name;
			ALine=malloc(102);
			memset(ALine,0,102);
			for (;;){
			if(ReadVirtualLine2(hFile,ALine,&i,pFilePos))
			{//line not including CRLF.				
				memset(szKey,0,100);
				if (FindStringBeforeCh(ALine,'=',szKey))
				{
					TrimString(szKey);
					if (stricmp(szKey,TEXT("filename"))==0 || stricmp(szKey,TEXT("name"))==0)
					{
						i=StrPos(ALine,34);
						temp=ALine+i;
						if (i=StrPos(temp,34))
						{
							CHAR szTempFileName[40]; 
							memset( szTempFileName, 0,40);
							strncpy(szTempFileName,temp,i-1);//get the name.
							memset( part->name,0, 40);
						    ParseString( szTempFileName,part->name);
						}
					}
				}
				if (i==2)
					SetFilePointer(hFile,-2,NULL,FILE_CURRENT);	
				if ( StrPos( ALine,';') == 0 )
					break;
			}
			else
				break;
			}
			free(ALine);
		}
	}	
  return ;   
}

/*************************************************
声明：void HandleMultiPartBody( HANDLE hFile, PEmailFormat  pMail, int CurPos)
参数：
IN      hFile    -文件句柄
IN/OUT  pMail    -邮件格式的数据结构
IN  CurPos  -得到当前邮件读取的位置
  返回：void
功能描述：在多行的解析中，注意如果是Printable 格式，可能在一行中存在多于100个字节的数据。
          要改进，一般情况下，BASE64格式不会存在多于100个字节的行。
 
调用：  被ParseEmailBody 调用，邮件体解析时,当有多个部分时要注意解析出一些不同邮件发送器产生的
        差异。
************************************************/
void HandleMultiPartBody( HANDLE hFile, PEmailFormat  pMail, int CurPos)
{
	EmailPart part;
    CHAR szLine[102];
	CHAR szKey[121];
	int nLineLength;
	int nFilePos;
    CHAR *szBoundary;
	CHAR * szBoundary1;
	CHAR * szBoundary2;
	CHAR *temp;

	PEmailAttach Attach;
    BOOL fOver,bFound,bContinue;
	nFilePos = CurPos;
	szBoundary=malloc(102);
	memset(szBoundary,0,102);
	//寻找 boundary
	szBoundary2 = malloc( 102 );
	bFound=FALSE;
	for (;;)
	{
		memset(szLine,0,102);
		bFound=FALSE;
		bContinue = FALSE;
		sprintf(szBoundary,TEXT("--%s"),pMail->boundary1);//jms mark
		if ( pMail->uiParseState == EMAIL_PARSE_BODY_IMAGE ){
			sprintf( szBoundary2,TEXT("--%s--"),pMail->boundary2 );			
		}
		//ReadVirtualLine( hFile,szLine,
		while (fOver=ReadVirtualLine2(hFile,szLine,&nLineLength,&nFilePos))//nLineLength return line length.
		{//szLine return a line without CRLF.
			if (stricmp(szBoundary,szLine)==0)
			{
				bFound=TRUE;
				break;
			}
			if ( pMail->uiParseState == EMAIL_PARSE_BODY_IMAGE ){
				if ( stricmp( szBoundary2,szLine )== 0 )
				{//BODY sub file parse over
					strcpy( szBoundary2,pMail->boundary2 );
					strcpy( pMail->boundary2,pMail->boundary1 );
					strcpy( pMail->boundary1,pMail->boundary3 );
					strcpy( pMail->boundary3,szBoundary2 );
					pMail->uiParseState = EMAIL_PARSE_ATTACHMEN ;
					//continue
					bContinue = TRUE;
					break;
				}
			}
		}
        if (!fOver)
			break;
		if ( bContinue )
			continue;
		//进入一段解析段。
		memset(szLine,0,102);
		memset(part.name,0,40);
		memset(part.type,0,20);
		memset(part.subtype,0,20);
		memset(part.disposing,0,20);
		memset(part.encoding,0,20);
		part.blank=0;//begin to parse a part.
		part.param=FALSE;//not find boundary.
		
		while (fOver=ReadVirtualLine2(hFile,szLine,&nLineLength,&nFilePos))
		{
            if (nLineLength==2 || nLineLength == 1)
			{
				part.blank++;
				if (part.blank==1)
				{
                    //judge if is a multipart second boundary text.
					if (part.param==TRUE)
						break;//means end the part parsing.
				//	memset(szLine,0,102);
				}
			    if (part.blank==2)
					break;
			}
			switch (part.blank)
			{
			case 0:
				AdjustPosition( szLine,&nFilePos,nLineLength );
				GetKeyString(hFile,&nFilePos,szLine,&part,pMail);
				break;
			case 1:
				//要么就解析出plain 格式 或者就解析出 html格式两者之一,只解释一个，如果含有两个的话则只解释第一个，第二个不再解释
				if ( pMail->BodyType == 0 && stricmp(part.type,TEXT("TEXT"))==0 && stricmp(part.disposing,TEXT("attachment"))!=0 )
				//if (stricmp(part.type,TEXT("TEXT"))==0 && stricmp(part.subtype,TEXT("plain"))==0)
				{
					int  iBodyLen = 2048;
				
					if ( stricmp( part.subtype,TEXT("plain") ) == 0 )
						pMail->BodyType = 1;
					else if ( stricmp( part.subtype,TEXT("HTM") ) == 0 || stricmp( part.subtype,TEXT("HTML") )== 0 )
					{
						pMail->BodyType = 2;
					}
					if ( pMail->Body )
						free( pMail->Body );
					pMail->Body = malloc(iBodyLen);
					if ( pMail->Body == NULL )
					{
						SetCurrentError( NOT_MEMORY_AFFORD );
						break;
					}
                         
					memset(pMail->Body,0,iBodyLen);
					
                     if (stricmp(part.encoding,TEXT("Base64")) == 0 )
					 {
						 memset(szLine,0,102);
						  while (ReadVirtualLine2(hFile,szLine,&nLineLength,&nFilePos))
						  {//base64必须严格按照换行来结束
							  if (nLineLength==2 || nLineLength == 1 )//text parse over.
							  {
								  fOver=FALSE;
								  break;
							  }
							  memset(szKey,0,121);
							  DecodeBase64(szLine,szKey);
							  // test
							 // ASSERT( strlen( pMail->Body ) < iBodyLen - 1 );
							   //

							  if ( (int)(strlen(szKey) + strlen(pMail->Body) +1 )> iBodyLen )
							  {
								  CHAR * szTemp;
								  
								  szTemp = realloc( pMail->Body, iBodyLen*2);
								  //HeapValidate( GetProcessHeap(),0,0 );
								  if ( szTemp == NULL )
								  {
									  SetCurrentError( NOT_MEMORY_AFFORD );
									  fOver=FALSE;
									  break;
								  }
								  else
								  {
									  szTemp[iBodyLen] = 0;
                                      pMail->Body = szTemp;
									  iBodyLen = 2*iBodyLen;
								  }
							  }
							  strcat(pMail->Body,szKey);	
							  // test
//							  ASSERT( strlen( pMail->Body ) < iBodyLen - 1 );
							  //
							  					}						  
					 }
					 else if (stricmp(part.encoding,TEXT("quoted-printable"))==0)
					 {						     
							 temp = malloc( 102 );
							 memset(temp,0,102);
							 memset(szLine,0,102);
							 memset( szBoundary,0,102 );
                             strcpy( szBoundary,pMail->boundary1 );
							 szBoundary1 = malloc( 102 );
							 memset( szBoundary1, 0, 102);
							 sprintf( szBoundary1, TEXT("--%s--\r\n"),pMail->boundary1);
							 sprintf(szBoundary,TEXT("--%s\r\n"),pMail->boundary1);
							 while (ReadVirtualLine(hFile,szLine,nFilePos))
							 {
                                 TrimLines2( szLine,&nLineLength );
								 nFilePos += nLineLength;
								 if (stricmp(szBoundary,szLine)==0 || stricmp(szBoundary1,szLine)==0 )
								 {
									  fOver=FALSE;
									  break;
								 }
								 memset( temp, 0, 102 );
								 DecodeQuoted( szLine,temp );
								 if ( (int)(strlen(temp) + strlen(pMail->Body)+1)>iBodyLen )
								 {
									 CHAR * szTemp;
									 szTemp = realloc( pMail->Body, iBodyLen*2);
									 if ( szTemp == NULL )
									 {
										 SetCurrentError( NOT_MEMORY_AFFORD );
										 fOver=FALSE;
										 break;
									 }
									 else
									 {
										 szTemp[iBodyLen] = 0;
										 pMail->Body = szTemp;
										 iBodyLen = 2*iBodyLen;
									 }
								 }
								 strcat(pMail->Body,temp);
								 memset(szLine,0,102);
							 }
					          free(temp);
							  free( szBoundary1 );
					 }
					 else 
					 {//其他不认识的格式:bit8,bit7等格式一律按照字符串来解释,
						 szBoundary1 = malloc( 102 );
						 sprintf( szBoundary1, TEXT("--%s--"),pMail->boundary1);
						 sprintf(szBoundary,TEXT("--%s"),pMail->boundary1);
						 while (ReadVirtualLine2(hFile,szLine,&nLineLength,&nFilePos))
						   {
							     
							      if (stricmp(szBoundary,szLine)==0 || stricmp(szBoundary1,szLine)==0 )
								 {
									  fOver=FALSE;
									  break;
								 }
                                  //if (nLineLength==2 || nLineLength == 1 )
								  //{//当遇到boundary界限时结束该段
									 // fOver=FALSE;
									//  break;
								  //}//no guaratten for limits.
								  if ( (int)(strlen(pMail->Body)+strlen(szLine)+1)>iBodyLen )
								  {
                                     CHAR * szTemp;
									 szTemp = realloc( pMail->Body, iBodyLen*2);
									 if ( szTemp == NULL )
									 {
										 SetCurrentError( NOT_MEMORY_AFFORD );
										 fOver=FALSE;
										 break;
									 }
									 else
									 {
										 szTemp[iBodyLen] = 0;
										 pMail->Body = szTemp;
										 iBodyLen = 2*iBodyLen;
									 }
								  }
								  strcat(pMail->Body,szLine);
						   }
						   free( szBoundary1 );
					 }
					 if (pMail->boundary2!=NULL){
						 strcpy(pMail->boundary1,pMail->boundary2);
					 }
					 else
					 {
						 nFilePos -= nLineLength;//一些邮件发送程序没有按照标准来分隔。
					 }
					 if ( pMail->boundary3 ){
						 CHAR szTempStr[100];
					     pMail->uiParseState = EMAIL_PARSE_BODY_IMAGE;
						 strcpy( szTempStr,pMail->boundary1);
						 strcpy( pMail->boundary1,pMail->boundary2 );
						 strcpy( pMail->boundary2,szTempStr );
					 }
					 else 
						 pMail->uiParseState = EMAIL_PARSE_ATTACHMEN;
				}
				else if (stricmp(part.type,TEXT("image"))==0 || stricmp(part.type,TEXT("application"))==0 || stricmp(part.disposing,TEXT("attachment"))==0)
				{
					  Attach=(PEmailAttach)malloc(sizeof(EmailAttach));
					  Attach->Name=malloc(40);
					  strcpy(Attach->Name,part.name);
					  if (stricmp(part.encoding,TEXT("base64"))==0)
					        Attach->coding = 1;//1 means base64 method.
					  else if (stricmp(part.encoding,TEXT("quoted-printable"))==0)
						    Attach->coding = 2;//2 means quoted-printable method.
					  else  Attach->coding = 0;//0 means text method.
					  Attach->pos = nFilePos;
					  Attach->cbyte = 0 ;
					  if ( stricmp(part.type,TEXT("image"))==0 )
						  Attach->type = BODY_ATTACH;
					  else
						  Attach->type = MAIL_ATTACH;
                      if (Attach->coding == 1)
					  {
						  int  iTrueLen ;
						  while (ReadVirtualLine2(hFile,szLine,&nLineLength,&nFilePos))
						  {
							  if (nLineLength==2 || nLineLength == 1)
							  {
								  fOver=FALSE;
								  break;
							  }
							  memset(szKey,0,121);
							  DecodeBase64(szLine,szKey);
                              //不能够使用strlen来获取解释后的字符串的长度，因为可能是二进制的数据流    
							  iTrueLen = (strlen( szLine ) )* 3/4;
							  Attach->cbyte += iTrueLen ;
						      memset( szLine,0,102);
						  }
						  Attach->cbyte = Attach->cbyte/1024 + 1;
						  
					  }else if (Attach->coding == 2)
					  {
						  temp=malloc(102);
					      memset(temp,0,102);
     					  sprintf(szBoundary,TEXT("--%s"),pMail->boundary1);
						  szBoundary1 = malloc( 102 );
						  memset(szBoundary1, 0, 102);
						  sprintf( szBoundary1,TEXT("--%s--"),pMail->boundary1);
						  while (ReadVirtualLine2(hFile,szLine,&nLineLength,&nFilePos))
						  {
							  if (stricmp(szBoundary,szLine)==0 || stricmp(szBoundary1, szLine)== 0)
							  {
								  fOver = FALSE;
								  nFilePos -=nLineLength;
								  break;
							  }
							  memset( temp,0,102 );
                              DecodeQuoted(szLine,temp);
						      Attach->cbyte +=strlen( temp )+2;	  
						  }
                          free( szBoundary1 );						  
						  free(temp);
						  Attach->cbyte = Attach->cbyte /1024 + 1;
					  } else if (Attach->coding==0)
					  {// We should just copy the string to dest until we find the boundary string
                          //测试每一行来确定是 Boundary 界限值.
						  szBoundary1 = malloc( 102 );
                          memset( szBoundary1, 0,102 );
						  sprintf(szBoundary,TEXT("--%s"),pMail->boundary1);
                          sprintf( szBoundary1,TEXT("--%s--"),pMail->boundary1);
						  while (ReadVirtualLine2(hFile,szLine,&nLineLength,&nFilePos))
						  {
							  if (stricmp(szBoundary,szLine)==0 || stricmp( szBoundary1,szLine)== 0 )
							  {
								  fOver = FALSE;
								  nFilePos -= nLineLength;
								  break;
							  }
 						      Attach->cbyte += nLineLength;
						 }						  
						 free( szBoundary1 );
						 Attach->cbyte = Attach->cbyte/1024 + 1;
					  }
					  if ( pMail->AttachCount == pMail->MaxCount )
					  {
                          pMail->MaxCount = 2*pMail->MaxCount ;
						  pMail->Attach = realloc( pMail->Attach, pMail->MaxCount*sizeof(PEmailAttach));
					  }
					  pMail->Attach[pMail->AttachCount]=Attach;
					  pMail->AttachCount++;
				}
				break;
			case 2:
				  fOver=FALSE;
			}			
			memset(szLine,0,102);
			if (fOver==FALSE)
			{
				break;
			}
		}
	}
    free(szBoundary);
}

/*************************************************
声明：void  InitEmailFormat( PEmailFormat  pMail )
参数：
IN/OUT  pMail    -邮件格式的数据结构
返回：void
功能描述：在读一邮件之前，必须进行初始化
调用：  在读一邮件之前，必须进行初始化
************************************************/
BOOL  InitEmailFormat( PEmailFormat  pMail )
{
	 if(!pMail)
	 {
		 RETAILMSG(TMAIL,("ERROR!!!~o~~o~~o~~o~~o~~o~~o~~o~~o~,occur in ../parseemail.c/DeInitEmailFormat()"));
		 return FALSE;
	 }
	 pMail->bSent     = TRUE;
     pMail->Cc        = NULL;
     pMail->Date      = NULL;
	 pMail->From      = NULL;
	 pMail->Subject   = NULL;
	 pMail->To        = NULL;
	 pMail->boundary1 = NULL;
     pMail->ReplyTo   = NULL;
	 pMail->emailtype = NONE_TYPE;
	 pMail->DeType    = NONE_FORMAT;
	 pMail->BodyType  = 0;// 
	 //以上是对邮件头的初始化	 
	 pMail->Body=NULL;
	 pMail->Attach=(PEmailAttach *)malloc(2*sizeof(PEmailAttach));
     //初始的数量值是2。
     pMail->AttachCount=0;
	 pMail->MaxCount=2;
	 pMail->boundary2 = NULL ;
     pMail->boundary3 = NULL ;
	 pMail->uiParseState = EMAIL_PARSE_BODY ;
	 
     return TRUE;
}

/*************************************************
声明：void  DeInitEmailFormat ( PEmailFormat  pMail )
参数：
IN/OUT  pMail    -邮件格式的数据结构
返回：void
功能描述：在完成读取之后，必须进行析构，释放资源
调用：  在读一邮件之后，释放所有的资源
************************************************/
void  DeInitEmailFormat ( PEmailFormat  pMail )
{
	int i;

	   if(!pMail)
	   {
		   RETAILMSG(TMAIL,("ERROR!!!~o~~o~~o~~o~~o~~o~~o~~o~~o~,occur in ../parseemail.c/DeInitEmailFormat()"));
		   return;
	   }
	   if (pMail->Cc!=NULL)
	   {
		   free(pMail->Cc);
	       pMail->Cc=NULL;
	   }
	   if (pMail->ReplyTo)
	   {
		   free( pMail->ReplyTo);
		   pMail->ReplyTo =  NULL;
	   }
	   if (pMail->From!=NULL)
	   {
		   free(pMail->From);
		   pMail->From=NULL;
	   }
	   if (pMail->To!=NULL)
	   {
		   free(pMail->To);
		   pMail->To=NULL;
	   }
	   if (pMail->Subject!=NULL)
	   {
		   free(pMail->Subject);
           pMail->Subject=NULL;
	   }
	   if (pMail->Date!=NULL)
	   {
		   free(pMail->Date);
		   pMail->Date=NULL;
	   }
	   if (pMail->boundary1!=NULL)
	   {
		   free(pMail->boundary1);
           pMail->boundary1=NULL;
	   }
	   
	   if (pMail->Body != NULL ){
		   free(pMail->Body);
		   pMail->Body = NULL;
	   }
	   
	   if ( pMail->AttachCount>0 )
	   {
		   for( i=0;i<(pMail->AttachCount ); i++)
		   {
			   if ( pMail->Attach[i] )
			   {
				   if (pMail->Attach[i]->Name )
					   free( pMail->Attach[i]->Name );
				   free( pMail->Attach[i] );
			   }
		   }
	   }
	   if ( pMail->boundary2 )
	   {
		   free( pMail->boundary2 );
		   pMail->boundary2 = NULL ;
	   }
	   if ( pMail->boundary3 ){
		   free( pMail->boundary3 );
	       pMail->boundary3 = NULL;
	   }
	   free(pMail->Attach);
	   pMail->Attach = NULL;
	   pMail->MaxCount=0;
	   pMail->AttachCount=0;
	   pMail->bSent =  TRUE;
}

/*************************************************
声明：void ParseEmailHeader( HANDLE hFile, PEmailFormat  pMail,int *pdwCurPos )
参数：
IN      hFile    -文件句柄
IN/OUT  pMail    -邮件格式的数据结构
IN/OUT  pdwCurPos-得到当前邮件读取的位置
  返回：void
功能描述：读取邮件的头结构
调用：  被ParseEmail 调用，邮件头的解析的结果是解析邮件体和附件的基础
************************************************/
void ParseEmailHeader(HANDLE hFile, PEmailFormat  pMail,int *pdwCurPos)
{
//以每次一行的方式来读,遇到空行表示头解析结束
//当前pdwCurPos的值为0,有写软件支持多于100个字节的行，如OUTLOOK 的主题等都可以支持100
//当群发时，邮件地址的可能的写法有很多种，分隔符也可能有几种。
	CHAR * szHeaderPart;//每次读100个字，假设行中不超过100 Bytes
    CHAR ch;
    CHAR *szKey;
 
	int i=0;
	int cur=0;//记载当前的行开始位置。
    int FilePos=0;
    szHeaderPart=malloc(102);
	memset(szHeaderPart,0,102);
	szKey=malloc(100);
	//memset(szKey,0,100);
	//尽量读100个BYTE 来解析邮件头，如果不够，则需要再来，有多的需要记住
	//邮件头结束位置。

	while (ReadVirtualLine(hFile,szHeaderPart,FilePos))
	{
        //开始新一行的解析。
		i=0;
		cur=0;
		memset(szKey,0,100);
		if (( ch=szHeaderPart[cur])==13 || ch ==10 || ch==0 )//后一条判断为以防万一
		{
			if ( ch == 10 )
				FilePos+=1;//正常情况下认为邮件的行结束符为CRLF.
			else
				FilePos+=2;
			break;//退出邮件头解析.
		}
		//开始解析该行。由于很多邮件发送器的的没有完全按照CRLF的格式来结尾，所以需要特别处理
		//分为两种情况下来处理即要么CRLF，要么是LF(10)
		ch=szHeaderPart[cur++];
		while(1)
		{
			if (ch!=':')
			{	 
			//一旦发现':'表示需要解析该行，并记下关键字	
				szKey[i++]=ch;
				ch=szHeaderPart[cur++];
				if ( ch==13 || ch == 10 || ch==0 )
				{
				    //直到行结束时没有发现':'，表示行不会进行解析，返回
					if ( ch == 13 )
						cur+=1;// 跳过0x0a 
					FilePos+=cur;
					memset(szHeaderPart,0,102);
					break;
				}
			}
			else
			{
				while( 1 )
				{	//读出内容，一般LTrim 空白字符
					if ((szHeaderPart[cur]==0x20) || (szHeaderPart[cur]==0x09))
					{
						cur++;
						continue;
					}
					break;
				}
				//关键子记下
				if (stricmp(szKey,TEXT("From"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						//一般的邮件行结束都是0x13 0x10,但是如果是其他的文本可能在文件结束
						//时没有结束符，则可能陷入循环，因此加上0判断
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==10 || szHeaderPart[cur]==0  ) 
							break;
						szKey[i++]=szHeaderPart[cur++];
					}					
					if (!pMail->From)
					{
						//动态分配
						pMail->From=(LPTSTR)malloc(i+1);
						memset(pMail->From,0,i+1);
						ParseString(szKey,pMail->From);
					}
					if (szHeaderPart[cur] == 10 )
						cur+=1;
					else
						cur+=2;//skip CRLF.
					FilePos+=cur;
					memset(szHeaderPart,0,102);
					break;
				}
				if (stricmp(szKey,TEXT("Reply-To"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						//一般的邮件行结束都是0x10d 0x0a,但是如果是其他的文本可能在文件结束
						//时没有结束符，则可能陷入循环，因此加上0判断
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==10 || szHeaderPart[cur]==0  ) 
							break;
						szKey[i++]=szHeaderPart[cur++];						
					}					
					if (!pMail->ReplyTo)
					{
						//动态分配
						pMail->ReplyTo=(LPTSTR)malloc(i+1);
						memset(pMail->ReplyTo,0,i+1);
						ParseString(szKey,pMail->ReplyTo);
					}
					if (szHeaderPart[cur] == 10 )
						cur+=1;
					else
						cur+=2;//skip CRLF.
					FilePos+=cur;
					memset(szHeaderPart,0,102);
					break;
				}
				if (stricmp(szKey,TEXT("To"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==10 || szHeaderPart[cur]==0 ) 
							break;
						szKey[i++]=szHeaderPart[cur++];						
					}					
					if (!pMail->To)//has already got.
					{
						pMail->To=malloc(i+1);
						memset(pMail->To,0,i+1);
						ParseString(szKey,pMail->To);
					}
					if (szHeaderPart[cur] == 10 )
						cur+=1;
					else
						cur+=2;//skip CRLF.
					FilePos+=cur;
					memset(szHeaderPart,0,102);
					break;
				}
				//看是否是MLG标志
				if (stricmp(szKey,TEXT("MLG_Sent"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==10 || szHeaderPart[cur]==0 ) 
							break;
						szKey[i++]=szHeaderPart[cur++];						
					}
					if ( '0' == szKey[i] )
                          pMail->bSent = FALSE;
					if (szHeaderPart[cur] == 10 )
						cur+=1;
					else
						cur+=2;//skip CRLF.
					FilePos+=cur;
					memset(szHeaderPart,0,102);
					break;
				}
				if (stricmp(szKey,TEXT("Date"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==10 || szHeaderPart[cur]==0) 
							break;
						szKey[i++]=szHeaderPart[cur++];						
					}					
					if (!pMail->Date)//has already got.
					{
						pMail->Date=malloc(i+1);
						memset(pMail->Date,0,i+1);
						ParseString(szKey,pMail->Date);
					}
					if (szHeaderPart[cur] == 10 )
						cur+=1;
					else
						cur+=2;//skip CRLF.
                    FilePos+=cur;
					memset(szHeaderPart,0,102);
					break;
				}
				if (stricmp(szKey,TEXT("Subject"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==10 || szHeaderPart[cur]==0) 
							break;
						szKey[i++]=szHeaderPart[cur++];						
					}					
					if (!pMail->Subject)//has already got.
					{
						pMail->Subject=malloc(i+1);
						memset(pMail->Subject,0,i+1);
						ParseString(szKey,pMail->Subject);
					}
					if (szHeaderPart[cur] == 10 )
						cur+=1;
					else
						cur+=2;//skip CRLF.
					FilePos+=cur;					
					memset(szHeaderPart,0,102);
					break;
				}				
				if (stricmp(szKey,TEXT("Cc"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==0 || szHeaderPart[cur]==0) 
							break;	
							szKey[i++]=szHeaderPart[cur++];						
					}					
					if (pMail->Cc!=NULL)//has already got.
					{
						pMail->Cc=malloc(i+1);
						memset(pMail->Cc,0,i+1);
						ParseString(szKey,pMail->Cc);
					}
					if (szHeaderPart[cur] == 10 )
						cur+=1;
					else
						cur+=2;//skip CRLF.
					FilePos+=cur;					
					memset(szHeaderPart,0,102);
					break;					
				}
				if (stricmp(szKey,TEXT("Content-Transfer-Encoding"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==0 || szHeaderPart[cur]==0) 
							break;	
							szKey[i++]=szHeaderPart[cur++];						
					}
					if (stricmp(szKey,TEXT("quoted-printable"))==0)
						pMail->DeType=Quoted;
					else
						if (stricmp(szKey,TEXT("base64"))==0)
							pMail->DeType=Base64;
					if (szHeaderPart[cur] == 10 )
						cur+=1;
					else
						cur+=2;//skip CRLF.
					FilePos+=cur;
					memset(szHeaderPart,0,102);
					break;					
				}
				if (stricmp(szKey,TEXT("Content-Type")) == 0 )
				{//read next line to get boundary flags;
					//***************************************************************
					i=0;
					memset(szKey,0,100);
					while ( (ch=szHeaderPart[cur++])!=0x0a && (ch!=0) && ch!=0x2f)
					{
						szKey[i++]=ch;
					}
					if (ch==0x2f)//寻找‘/’
					{
						if (stricmp(szKey,TEXT("TEXT"))==0)
						{
							pMail->emailtype=TEXT_TYPE;
						}
						else if (stricmp(szKey,TEXT("multipart"))==0)
						{
								pMail->emailtype=MULTIPART_TYPE;
						}
						else
						{
								pMail->emailtype=NONE_TYPE;
						}
						
						//等待读出所有的数据后，不解析子类型，因为不管是那种格式，都必须以0x0a结尾，所以是没有问题的
						i = 0;
						memset( szKey, 0,100 );
						while((ch=szHeaderPart[cur++])!=0x0a && ch!=0 && ch!= ';' && i< 99 )szKey[i++] = ch;//等待换行
						if ( stricmp( szKey,TEXT("html"))== 0 || stricmp( szKey,TEXT("htm"))== 0 )
							pMail->BodyType = 2; //means HTML format.
						else if ( stricmp(szKey,TEXT("plain")) == 0 )
							pMail->BodyType = 1;
						FilePos+=cur;
						memset(szHeaderPart,0,102);
						//find boundary for parsing the multipart.
						cur=0;
						while (ReadVirtualLine(hFile,szHeaderPart,FilePos))
						{
							
							//判断是否如果是 ';' 终结符,必须确认该字符后在行结束前是否有有效数据
							if ( ch == ';' ) //判断有效数据是否在下一行或在该行结束处
							{
								BOOL  bContinue = FALSE;
								while (1)
								{
									ch = szHeaderPart[cur];
									if ( ch ==13 || ch == 10 || ch ==0x20 || ch ==0x09  )
									{
										if ( ch == 13 || ch == 10 )
										{
											bContinue = TRUE;
											if ( ch == 13 )
												cur +=2;
											else 
												cur++;
											break;
										}
										cur++;
										continue;
									}
									break;
								}
                                if( bContinue )
								{
									FilePos += cur;
									memset( szHeaderPart,0,102 );
									cur = 0;
									continue;
								}
							}

							while(1)
							{	//读出内容，一般LTrim 空白字符
								if ((szHeaderPart[cur])==0x20 || (szHeaderPart[cur]==0x09 ))
								{
									cur++;
									continue;
								}
								break;
							}
							//寻找boundary
							memset(szKey,0,100);
							for (i=0;i<8;i++)
								szKey[i]=szHeaderPart[cur++];//在此没有判断,要防止异常发生
							if (stricmp(szKey,TEXT("boundary"))==0)
							{
								//获得boundary 值
								while ((ch=szHeaderPart[cur++])!=0x0D && ch!=0x0A && ch!=0x22 && ch!=0);
								if (ch==0x22)
								{				  
									//line end parsing.
									i=0;
									memset(szKey,0,100);
									while(ch=szHeaderPart[cur++])
									{//直到发现'='为止
										if (ch==13 || ch == 10)
										{
											if ( ch==10)
												cur--;
											break;
										}
										if (ch==0x22)
											break;
										szKey[i++]=ch;
									}
									if (ch==0x22)
									{
										//memset(szKey,0,100);
										pMail->boundary1=(CHAR *)malloc(i+1);
										memset(pMail->boundary1,0,i+1);
										strcpy(pMail->boundary1,szKey);
									}
								}
							}
							else
							{//因为在前面有一个循环找boundary.
								cur-=8;
							}
							//After getting the boundary
							while((ch=szHeaderPart[cur++])!=0x0a  && (ch!=0) && ch!=';' );
							FilePos+=cur;
							memset(szHeaderPart,0,102);
							cur=0;
							if ( ch != ';' )
								break;
						}
						
						break;//read the  file return FALSE BREAK;
						
						
					}
					//没有发现0x5c '/'
					cur++;
					FilePos+=cur;
					memset(szHeaderPart,0,102);
                    //*********************************************************************
					//删除空白字符。
					break;
				}
				//比较结果没有合适的关键字。
				while ((ch=szHeaderPart[cur++])!=0x0a && ch!=0);
				FilePos+=cur;				
				memset(szHeaderPart,0,102);
		   		break;				
		  }
		}
		}
		free(szHeaderPart);
		free(szKey);
		*pdwCurPos=FilePos;
      return;
}

/*************************************************
声明：void ParseEmailBody(HANDLE hFile,PEmailFormat  pMail,int * pCurPos)
参数：
IN      hFile    -文件句柄
IN/OUT  pMail    -邮件格式的数据结构
IN/OUT  pCurPos  -得到当前邮件读取的位置
  返回：void
功能描述：读取邮件体结构，和邮件的附件列表
          在邮件体的解析中分成三部分正如EmailFormat.h中所描述的那样
		  根据邮件头中的emailtype来读取邮件的内容
调用：  被ParseEmail 调用，邮件体解析
************************************************/
void ParseEmailBody(HANDLE hFile,PEmailFormat  pMail,int * pCurPos)
{
   int i,iBodyLen = 3073,iTempLen;
   CHAR *szLine;
   DWORD dwRead;
   CHAR ch;
   CHAR *szTemp;
   int FilePos=*pCurPos;
   //解析邮件体时，要以boundary 为标志
   if (pMail->emailtype == NONE_TYPE){
	   RETAILMSG(TME,("NONO_TYPE"));
       if ( pMail->Body!= NULL)
		   free( pMail->Body );
	   pMail->Body = malloc( iBodyLen );
	   memset( pMail->Body,0, iBodyLen );
	   pMail->BodyType = 1;
	   SetFilePointer(hFile,FilePos,NULL,FILE_BEGIN);
	   ReadFile( hFile,pMail->Body,3000,&dwRead,NULL );
	   return ;
   }
   if (pMail->emailtype==TEXT_TYPE)
   {
	  //不加解析地把文本拷过去  先拷500个字过来。
	   if (pMail->Body != NULL)
		   free( pMail->Body );
	   pMail->Body=malloc( iBodyLen );
	   memset( pMail->Body,0,iBodyLen );
	  
	   if (pMail->DeType==Quoted)
	   {//没有考虑将整个文本考下来，最多3K字节。
		   SetFilePointer(hFile,*pCurPos,NULL,FILE_BEGIN);
		   szLine = malloc( 3073 );
		   iTempLen = 3073;
		   szLine[0] = 0;		   
		   while (ReadFile(hFile,(LPVOID)pMail->Body,3072,&dwRead,NULL) )
		   {
	   	   // 如果是 Quoted 可以整个进行解析。
			   if ( (int)(strlen( pMail->Body )+ strlen(szLine)+1)>iTempLen )
			   {
				   CHAR * h;
				   h = realloc( szLine,iTempLen*2);
				   if ( h )
				   {
				       szLine = h;
					   szLine[iTempLen] = 0;
					   iTempLen *=2;
				   }
				   else
					   break;
			   }
			   strcat( szLine, pMail->Body );
			   memset( pMail->Body, 0,iBodyLen);
			   if( dwRead< 3072 || iTempLen >40*1024)
				   break;
		   }
		   
		   if ( iBodyLen <iTempLen )
		   {
			   pMail->Body = realloc( pMail->Body,iTempLen);
		       iBodyLen = iTempLen ;
			   memset( pMail->Body, 0,iBodyLen );
		   }
		   DecodeQuoted( szLine,pMail->Body );
		   free( szLine );		   
	   }
	   else if (pMail->DeType==Base64)
	   {
          szLine=malloc(102);
		  szTemp=malloc(100);
		  memset(szLine,0,102);
		  while (ReadVirtualLine(hFile,szLine,FilePos))
		  {
			  i=0;
            while ((ch=szLine[i++])!=0x0D && ch!=0);
			szLine[--i]=0;
			i+=2;
            memset(szTemp,0,100);
			DecodeBase64(szLine,szTemp);
			if ( (int)(strlen(pMail->Body) + strlen(szTemp)+1)>iBodyLen )
			{
			     CHAR *szCheck;
				   szCheck = realloc(pMail->Body,iBodyLen*2);
				   if ( szCheck == NULL)
				       break;
				   pMail->Body = szCheck;
				   pMail->Body[iBodyLen] = 0;
                   iBodyLen *=2;
			}
			strcat(pMail->Body,szTemp);
			memset(szLine,0,102);
			FilePos+=i;
//由于不用考虑要不要识别boundary，所以可以直接来读取
		  }
		  free(szTemp);
		  free(szLine);
          //如果是BASE64则只能一行一行地进行解析。
	   }else
	   {//对于不能识别的数据，可以要求简单的拷贝
           ReadFile(hFile,(LPVOID)pMail->Body,3073,&dwRead,NULL);
		   //Copy  the text to content.
           //FilePos+=i;
	   }
	   *pCurPos=FilePos;
	   return ;
   }
	if (pMail->emailtype==MULTIPART_TYPE)
	{//最复杂的解析。
        HandleMultiPartBody( hFile, pMail, FilePos);
	}   
   return ;
}

/*************************************************
声明：BOOL  ParseEmail( LPTSTR szFileName, PEmailFormat  pMail )
参数：
IN  szFileName  - 邮件文件名
IN/OUT  pMail    -邮件格式的数据结构
返回：BOOL TRUE  解释正确
           FALSE 解释失败
功能描述：解释一封完整的邮件
调用：    必须在邮件格式体，被初始化之后，才可以进行，并且在不需要时要析构掉
************************************************/
BOOL  ParseEmail( LPTSTR szFileName, PEmailFormat  pMail )
{
     HANDLE   hFile;
	 int   iCurPos;
     
	 iCurPos = 0 ;
	 hFile=CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL );
	 if (hFile == INVALID_HANDLE_VALUE )
	 {
//	     SetCurrentError( INVALID_VALUE_FILE );
		 return FALSE ;
	 }
	 ParseEmailHeader(hFile,pMail,&iCurPos) ;
	
	 ParseEmailBody( hFile, pMail, &iCurPos) ;
	 
	 CloseHandle( hFile );
	 return TRUE ;
}

/*************************************************
声明：void ConvertEmailTime( LPTSTR lpDest, LPSYSTEMTIME   lst )
参数：
OUT   lpDest       -转换后的时间格式 
IN    lst          -已经获得的系统时间
返回：void
功能描述：将系统时间转化为网络邮件时间，
          注：必须要求一个系统时间和获得时区
************************************************/
void ConvertEmailTime( LPTSTR lpDest, LPSYSTEMTIME   lst )
{
	CHAR szTemp[20];
    lpDest[0] = 0;
	szTemp[4] = 0;
	strcpy( lpDest, TEXT("Date: "));
	switch( lst->wDayOfWeek )
	{
	case 0:
       strcat( lpDest, "Sun,");   break;
    case 1:
		strcat( lpDest, "Mon,");  break;
	case 2:
		strcat( lpDest, "Tue,");  break;
	case 3:
		strcat( lpDest, "Wed,");  break;
	case 4:
		strcat( lpDest, "Thu,");  break;
	case 5:
		strcat( lpDest, "Fri, "); break;
	case 6:
		strcat( lpDest, "Sat,");  break;
	}
	memset( szTemp,0,20 );
    itoa( lst->wDay, szTemp,10 );
	strcat( lpDest, szTemp );
	switch ( lst->wMonth )
	{
	case 1:
		strcat( lpDest, " Jan ");break;
	case 2:
		strcat( lpDest," Feb ");break;
	case 3:
		strcat( lpDest, " Mar ");break;
	case 4:
		strcat(lpDest, " Apr ");break;
	case 5:
		strcat( lpDest, " May ");break;
	case 6:
		strcat( lpDest, " Jun ");break;
	case 7:
		strcat( lpDest, " Jul ");break;
	case 8:
		strcat( lpDest, " Aug ");break;
	case 9:
		strcat( lpDest, " Sep ");break;
	case 10:
		strcat( lpDest, " Oct ");break;
	case 11:
		strcat( lpDest, " Nov ");break;
	case 12:
		strcat( lpDest, " Dec ");break;
	}    
	sprintf( szTemp, "%d %d:%d:%d",lst->wYear,lst->wHour,lst->wMinute,lst->wSecond );
	strcat( lpDest, szTemp );
	//Get local Time Zone ,Add to it
    sprintf( szTemp, " +0800");
    strcat( lpDest, szTemp );
}
/*************************************************
声明：BOOL EmailTimeConvert(LPTSTR lpDest,LPTSTR lpSource)
参数：
OUT   lpDest       -转换后的时间格式 
IN    lpSource     -转换前的时间格式
返回：BOOL TRUE  解释正确
           FALSE 解释失败
功能描述：要求时间格式一定要按照基于INTERNET邮件协议的标准时间来计算，否则不给解释
          给函数为特定的版本，将标准时间转化为中文格式，在时间的具体格式上需要针对
		  不同的个性来确定,由于星期日期是可选的，所以要求能够处理
************************************************/
BOOL EmailTimeConvert(LPTSTR lpDest,LPTSTR lpSource)
{
     CHAR szTemp[4];
	 CHAR szWeek[10];
	 CHAR ch;
     CHAR szHour[4],szMiniter[4],szSecond[4],szDay[4],szMonth[4],szYear[6],szTimes[4];
     int  index=0, i;
	 BOOL bDayOfWeek = FALSE;
	 //source has been trimmed left.
	 memset(szTemp,0,4);
	 if (lpSource == NULL )
          return FALSE;
     if ( StrPos( lpSource,',' ) )//星期日期是可选的
		 bDayOfWeek = TRUE;
	 memset( szSecond, 0, 4);
	 memset( szMiniter, 0, 4);
	 memset( szHour, 0, 4);
	 memset( szTimes, 0,4 );
	 if ( bDayOfWeek ){
	 strncpy( szTemp,lpSource,3);
	 index = 3;	 
	 if (stricmp( szTemp, TEXT("SUN")) == 0)
		 strcpy( szWeek, TEXT("星期天"));
     else if (stricmp( szTemp, TEXT("mon")) == 0)
		 strcpy( szWeek, TEXT("星期一"));
	 else if (stricmp( szTemp, TEXT("Tue")) ==0)
		 strcpy( szWeek, TEXT("星期二"));
	 else if (stricmp( szTemp, TEXT("Wed"))==0)
		 strcpy( szWeek, TEXT("星期三"));
	 else if (stricmp( szTemp, TEXT("Thu"))==0)
		 strcpy( szWeek,TEXT("星期四"));
	 else if (stricmp( szTemp, TEXT("Fri"))==0)
		 strcpy(  szWeek, TEXT("星期五"));
	 else if (stricmp( szTemp, TEXT("Sat"))==0)
		 strcpy( szWeek, TEXT("星期六"));
     else return FALSE;
	 index++;//skip ','
	 }
	 while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09)) index++;
     if (ch==0)
		 return FALSE;
	 memset( szDay, 0, 4 );
	 i = 0;
	 while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09)
	 {
         szDay[i++] = ch;      
		 index++;
	 }
	 while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09)) index++;
	 memset( szMonth, 0, 4 );
	 i = 0;
     while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09)
	 {
         szMonth[i++] = ch;      
		 index++;
	 }
	 if (stricmp( szMonth, TEXT("Jan")) == 0) 
		 strcpy( szMonth, TEXT("1"));
	 else if (stricmp( szMonth, TEXT("Feb"))==0) 
		 strcpy( szMonth, TEXT("2"));
	 else if (stricmp(szMonth, TEXT("Mar") )==0) 
		 strcpy( szMonth, TEXT("3"));
	 else if (stricmp( szMonth, TEXT("Apr") )==0 ) 
		 strcpy( szMonth, TEXT("4"));
	 else if (stricmp( szMonth, TEXT("May") ) ==0 ) 
		 strcpy( szMonth, TEXT("5") );
	 else if (stricmp( szMonth, TEXT("Jun")) == 0 ) 
		 strcpy( szMonth, TEXT("6") );
	 else if (stricmp( szMonth, TEXT("Jul") ) == 0 ) 
		 strcpy( szMonth, TEXT("7"));
	 else if (stricmp( szMonth, TEXT("Aug") ) == 0 ) 
		 strcpy( szMonth, TEXT("8"));
	 else if (stricmp( szMonth, TEXT("Sep") ) == 0 ) 
		 strcpy( szMonth, TEXT("9"));
	 else if (stricmp( szMonth, TEXT("Oct") ) == 0 ) 
		 strcpy( szMonth, TEXT("10") );
	 else if (stricmp( szMonth, TEXT("Nov") ) == 0 ) 
		 strcpy( szMonth, TEXT("11") );
	 else if (stricmp( szMonth, TEXT("Dec") ) == 0 ) 
		 strcpy( szMonth, TEXT("12"));
	 else return FALSE;

    while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09)) index++;
	 memset( szYear, 0, 6);
	 i = 0;
     while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09)
	 {
         szYear[i++] = ch;      
		 index++;
	 }
	 //解释出时分秒值
     while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09)) index++;
	 i = 0;
     while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09 && ch!=':')
	 {
         szHour[i++] = ch;      
		 index++;
	 }
     //解释出分
	 index++;
	 while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09) && ch == ':') index++;
	 i = 0;
     while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09 && ch!=':')
	 {
         szMiniter[i++] = ch;      
		 index++;
	 }
	 //解释出秒
	 index++;
     while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09) && ch == ':') index++;
	 i = 0;
     while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09 && ch!=':')
	 {
         szSecond[i++] = ch;      
		 index++;
	 }
/*     //解释出时区
     while ((ch = lpSource[index]) && (ch!='+') && ch !='-' ) index++;
	 i = 0;
     index++;
	 while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09 && ch!=':')
	 {
         szSecond[i++] = ch;      
		 index++;
	 }
*/  //如果需要加上星期日期的话，可以自由地加上该值
	 sprintf( lpDest, TEXT("%s-%s-%s %s:%s"), szYear, szMonth, szDay,szHour,szMiniter );
	 
	 while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09)) index++;
	 //not have the hour miniter,second.
	 return TRUE;
}     

/*************************************************
声明：BOOL SaveAttachAs( PEmailFormat  pMail , int index, LPTSTR lpEmailName , LPTSTR lpAttachName )
参数：
IN    pMail        -表示邮件的数据结构
IN    index        -表示要保存的附件索引值
IN    lpEmailName  -邮件的文件名
IN    lpAttachName -要保存的附件的名字
返回：BOOL TRUE  操作成功
           FALSE 操作失败
功能描述：根据附件的信息来提取出附件来，要求在邮件打开之前能够解释的完整，这个信息在PEmailFormat中描述
          一个附件的信息有：大小，名字，和解码方式，在邮件中的位置等信息，只要提供了完整的信息，可以提
		  出附件
引用： 需要保存附件时，操作此函数
************************************************/
BOOL SaveAttachAs( PEmailFormat  pMail , int index, LPTSTR lpEmailName , LPTSTR lpAttachName )
{
     PEmailAttach pAttach ;
	 HANDLE       hEmailFile ,hAttachFile;
	 CHAR         szLine[102],szTemp[102];
     int          iLineLength ;
	 int          iPos ;
     CHAR *       szBoundary ;
     CHAR *       szBoundary1;
	 int          iRealLen ;//当把Base64翻译出来时由于写二进制码时出现
	                        //有0的数据，所以当写数据时，尤其是处理Base64
							//的时候，要保证完全的数据写入，
     if (index >= pMail->AttachCount )
		 return FALSE ;
	 pAttach = pMail->Attach[index];
	 iPos =  pAttach->pos ;
     hEmailFile = CreateFile( lpEmailName, GENERIC_READ | GENERIC_WRITE , 0, 0, 
					  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	 if ( hEmailFile == INVALID_HANDLE_VALUE ) 
		 return FALSE;
	 hAttachFile = CreateFile( lpAttachName, GENERIC_READ | GENERIC_WRITE , 0, 0, 
					  CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );//==NULL)
	 if ( hAttachFile == INVALID_HANDLE_VALUE )	
	 {
		 CloseHandle( hEmailFile );
		 return FALSE ;
	 }
	 szBoundary=malloc(100);
	 SetFilePointer(hEmailFile,pAttach->pos,NULL,FILE_BEGIN);
	 switch ( pAttach->coding )
	 {
	 case 0: //TEXT
         memset( szLine, 0, 102 );
		 sprintf( szBoundary, TEXT("--%s"),pMail->boundary1 );
		 szBoundary1 = (CHAR*)malloc( 102 );
		 memset( szBoundary1, 0,102 );
		 sprintf( szBoundary1,TEXT("--%s--"),pMail->boundary1);
		 while (ReadVirtualLine2(hEmailFile,szLine,&iLineLength,&iPos))
		  {
              if (stricmp(szBoundary,szLine)==0 || stricmp( szBoundary1,szLine)== 0 )
			  {
				  break;
			  }
			  WriteFile(hAttachFile,szLine,strlen(szLine),(DWORD*)(&iLineLength),NULL);
			  WriteFile(hAttachFile,TEXT("\r\n"),2,(DWORD*)(&iLineLength),NULL);
         }
		 free ( szBoundary1 );
		 break;
	 case 1:// Base64 
		 memset( szLine ,0,102 );
		 while (ReadVirtualLine2(hEmailFile,szLine,&iLineLength,&iPos))
		 {
			 if (iLineLength==2 || iLineLength == 1)
			 {				
				 break;
			 }
			 memset(szTemp,0,102);
			 iRealLen = DecodeBase64( szLine, szTemp );
             if(!WriteFile(hAttachFile,szTemp,iRealLen,(DWORD*)(&iLineLength),NULL))
                  break;//这里似乎存在一点问题：因为BASE64加密时，不足都会补上，凑成运算，所以在解码时是否可以识别出来呢？
			 memset(szLine,0,102);
		 }
		 break;
	 case 2: //quoted-printable
     	  memset(szTemp,0,102);
		  memset(szLine,0,102);
          
		  sprintf(szBoundary,TEXT("--%s\r\n"),pMail->boundary1);
		  szBoundary1 = malloc( 102 );
		  memset( szBoundary1, 0, 102);
		  sprintf( szBoundary1, TEXT("--%s--\r\n"),pMail->boundary1 );
		            
		  while (ReadVirtualLine(hEmailFile,szLine,iPos))
		  {
			  TrimLines2( szLine,&iLineLength );
              iPos +=iLineLength;
              if (stricmp(szBoundary,szLine)==0 || stricmp(szBoundary1,szLine)==0 )
			  {
				  break;
			  }
			  memset( szTemp, 0,102 );
			  DecodeQuoted(szLine,szTemp);
			  WriteFile(hAttachFile,szTemp,strlen(szTemp),(DWORD *)(&iLineLength),NULL);
              memset(szLine,0,102);
		  }
          free( szBoundary1 );
		 break;
	 }	
	 free(szBoundary);
	 CloseHandle( hAttachFile );
	 CloseHandle( hEmailFile );
	 return TRUE;
}

/*************************************************
声明：BOOL GetEmailHeaderInfo(LPTSTR szFileName, PEmailFormat  pMail)
参数：
IN    szFileName   -邮件名
IN    pMail        -表示邮件的数据结构
返回：BOOL TRUE  操作成功
           FALSE 操作失败
功能描述：根据附件的信息来提取出附件来，要求在邮件打开之前能够解释的完整，这个信息在PEmailFormat中描述
          一个附件的信息有：大小，名字，和解码方式，在邮件中的位置等信息，只要提供了完整的信息，可以提
		  出附件
引用： 需要保存附件时，操作此函数
************************************************/
BOOL GetEmailHeaderInfo(LPTSTR szFileName, PEmailFormat  pMail)
{
	HANDLE hFile;
    int iCurPos=0;
	hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL );
    
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
     ParseEmailHeader(hFile,pMail,&iCurPos);
	 CloseHandle( hFile );
     return TRUE;
}

/*************************************************
声明：BOOL FindEmailAddressFromStr( LPCTSTR szSource ,LPTSTR szDest )
参数：
IN    szSource       - 源字符串
OUT   szDest         - 目的串,已经得到的邮件地址
返回  BOOL   TRUE    找到一个有效的地址
             FALSE   没有找到有效地址
************************************************/
BOOL FindEmailAddressFromStr( LPCTSTR szSource ,LPTSTR szDest )
{
	int   i,j;
	CHAR  ch;
	i = StrPos( (LPTSTR)szSource,'@' );
	if (i > 0 )
	{
        //找到第一个字符
		i--;
		while ( i-- )
		{
			ch = szSource[i];
			if ( ch ==' ' || ch == 0x09 || ch == 34 || ch == '<' )
				break;//找到第一个.
		}
        if ( i < 0 ) 
			i = 0 ;
		else
			i++;
		j = 0;
        ch = szSource[i];
		while ( ch )
		{
			if ( ch != ' ' && ch != 0x09 && ch !=34 && ch != '>' )
			{
               szDest[j++] = ch;
			}
			else 
				break;
            ch = szSource[++i];
		}
		szDest[j] = 0;
        return TRUE;
	}
	else
		return FALSE;
}

/*************************************************
声明：BOOL IsDraftBox( LPCTSTR szFileName )
参数：
IN      szFileName       - 一个邮件完整的地址
返回值： BOOL  TRUE 是DRAFT邮件
               FALSE不是DRAFT邮件
功能描述： 判断是否 draft 邮件
引用：  
************************************************/
BOOL IsDraftBox( LPCTSTR szFileName )
{
	BOOL bRet = FALSE;
	if (strstr( szFileName,"\\receive\\" ))
		bRet = TRUE;
	return bRet;
}

