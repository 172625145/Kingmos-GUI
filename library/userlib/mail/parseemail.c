/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

/**************************************************
�ļ�˵�����ʼ���������Ҫ���ʼ���һ���ļ��ж�����������ȷ�ĸ�ʽ,Ŀǰֻ֧��һЩͨ�ø�ʽ
          ��Base64,Quoted-printable ��html �ȸ�ʽ.
�汾�ţ�1.0.0
����ʱ�ڣ�2003-3-12
���ߣ�������
�޸ļ�¼�������� HTML+ATTACHMENT�Ķ๦���ʼ��Ĵ�����ʱ�����ж���߽��ַ������ڡ�2003-09-18 by ������
**************************************************/
#include <ewindows.h>
#include <EmailFormat.h>
#include <mailboxapi.h>
#include <BaseQuoted.h>
#include "emailflags.h"

/*************************************************
������BOOL CheckValidEmail( LPTSTR szTo )
������
IN      szTo    -�ʼ���ַ
���أ�  BOOL    TRUE  ��Ч���ʼ���ַ
                FALSE ��Ч���ʼ���ַ
�����������ڷ��ʼ�֮ǰҪ����Ҫ�ؼ�������ʼ���ַ�Ƿ���Ч��������ǣ���
          ���ᷢ�ͣ�������USER ȷ���ʼ���ַ��һ����ʼ����Ǵ�@���ŵ��ַ���
		  �����ʼ�����ʱ֧�ֶ��ͬʱ���ͣ�һ���Կո��ֺŸ���
���ã�    �ʼ�����ǰҪ���
************************************************/
BOOL CheckValidEmail( LPTSTR szTo )
{
    if (StrPos( szTo, '@' ) == 0 )//find '@' successfully.
		return FALSE;
	else 
		return TRUE;
}

/*************************************************
������void ParseString (CHAR * szSource,CHAR *szDest)
������
IN      szSource    -Դ�ַ���
IN      szDest      -ת������ַ���
���أ�void
�������������ʼ�ͷ�е�һЩ��Ϣ���ͳ�������Ϊ��Щ�ʼ�ͷҲ�����˼��ܣ�һ��
          �� Base64 ,Quoted-printable ���ַ�ʽ�����ܣ�����
���ã��ڽ����ʼ�ͷ(ParseEmailHeader)ʱ��Ҫ����
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
			//Ѱ�ҡ�����
			while ((ch = szSource[i++])!='?' && ch!=0 );//�������ַ���
			
			iCur=0;
			if (ch!=0)
			{//�ҵ��ؼ��֣�=?
				szEncode = malloc(100);
				szDecode = malloc(100);
				memset( szEncode, 0, 100);
				memset( szDecode, 0, 100);
				switch ((ch = szSource[i++]))
				{
				case 'Q':
				case 'q':
					i++;//����һ��?
					while ((ch = szSource[i++])!='?' && ch!=0)
						szEncode[iCur++]=ch;
					DecodeQuoted(szEncode,szDecode);
					strcat(szDest,szDecode);
					i++;
                    iPos+=strlen( szDecode );
					break;
				case 'B':
				case 'b':
					i++;//����һ��?
					while ((ch = szSource[i++])!='?' && ch!=0 )
						szEncode[iCur++]=ch;
					DecodeBase64(szEncode,szDecode);
					strcat( szDest, szDecode);
					i++;
					iPos+= strlen( szDecode );
					break;
				default:
                    //��Ӧ�ò�����
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
{//����һ�����䣬��Ϊ����ǰû�п��ǵ���һ�����ж�������Σ����ֻ������ʹ�ã�
//���һ���ж���������Ļ���ʹָ��ָ����һ��������
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
������void GetKeyString(HANDLE hFile,int *pFilePos,LPTSTR szLine, PEmailPart part, PEmailFormat  pMail)
������
IN      hFile    -�ļ����
IN/OUT  pFilePos -�ļ���ǰ�Ķ�дλ��
IN      szLine   -�ַ���
IN      part     -�ʼ�PART �Ľṹ
IN/OUT  pMail    -�ʼ���ʽ�����ݽṹ
���أ�void
���������������ʼ�Э�飬ÿһ����PART�Ĳ��ֶ���д��д��PART����Ϣ��ͨ����Щ
        ��Ϣ�ſ��Խ��ͳ��ò��֣���ϸ��ο���ص�Э���EmailFormat.h �е�EmailPart�ṹ˵��
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
									{   //�������ж�ʱ����ʹ��boundary1 �� boundary2 ����Ϊ�жϵı�־
										pMail->boundary2 = malloc(100);
										strcpy(pMail->boundary2,pMail->boundary1);
									    strcpy(pMail->boundary1,szKey);
									    part->param = TRUE;
									}
									else
									{
										if ( pMail->boundary3 == NULL ){
											pMail->boundary3 = malloc(100);
											//��ʱ��boundary2 ���Ѿ���������һ�ε�ԭboundary1,�����Ҫ����
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
						{ //��NAME
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
������void HandleMultiPartBody( HANDLE hFile, PEmailFormat  pMail, int CurPos)
������
IN      hFile    -�ļ����
IN/OUT  pMail    -�ʼ���ʽ�����ݽṹ
IN  CurPos  -�õ���ǰ�ʼ���ȡ��λ��
  ���أ�void
�����������ڶ��еĽ����У�ע�������Printable ��ʽ��������һ���д��ڶ���100���ֽڵ����ݡ�
          Ҫ�Ľ���һ������£�BASE64��ʽ������ڶ���100���ֽڵ��С�
 
���ã�  ��ParseEmailBody ���ã��ʼ������ʱ,���ж������ʱҪע�������һЩ��ͬ�ʼ�������������
        ���졣
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
	//Ѱ�� boundary
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
		//����һ�ν����Ρ�
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
				//Ҫô�ͽ�����plain ��ʽ ���߾ͽ����� html��ʽ����֮һ,ֻ����һ����������������Ļ���ֻ���͵�һ�����ڶ������ٽ���
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
						  {//base64�����ϸ��ջ���������
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
					 {//��������ʶ�ĸ�ʽ:bit8,bit7�ȸ�ʽһ�ɰ����ַ���������,
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
								  //{//������boundary����ʱ�����ö�
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
						 nFilePos -= nLineLength;//һЩ�ʼ����ͳ���û�а��ձ�׼���ָ���
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
                              //���ܹ�ʹ��strlen����ȡ���ͺ���ַ����ĳ��ȣ���Ϊ�����Ƕ����Ƶ�������    
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
                          //����ÿһ����ȷ���� Boundary ����ֵ.
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
������void  InitEmailFormat( PEmailFormat  pMail )
������
IN/OUT  pMail    -�ʼ���ʽ�����ݽṹ
���أ�void
�����������ڶ�һ�ʼ�֮ǰ��������г�ʼ��
���ã�  �ڶ�һ�ʼ�֮ǰ��������г�ʼ��
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
	 //�����Ƕ��ʼ�ͷ�ĳ�ʼ��	 
	 pMail->Body=NULL;
	 pMail->Attach=(PEmailAttach *)malloc(2*sizeof(PEmailAttach));
     //��ʼ������ֵ��2��
     pMail->AttachCount=0;
	 pMail->MaxCount=2;
	 pMail->boundary2 = NULL ;
     pMail->boundary3 = NULL ;
	 pMail->uiParseState = EMAIL_PARSE_BODY ;
	 
     return TRUE;
}

/*************************************************
������void  DeInitEmailFormat ( PEmailFormat  pMail )
������
IN/OUT  pMail    -�ʼ���ʽ�����ݽṹ
���أ�void
��������������ɶ�ȡ֮�󣬱�������������ͷ���Դ
���ã�  �ڶ�һ�ʼ�֮���ͷ����е���Դ
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
������void ParseEmailHeader( HANDLE hFile, PEmailFormat  pMail,int *pdwCurPos )
������
IN      hFile    -�ļ����
IN/OUT  pMail    -�ʼ���ʽ�����ݽṹ
IN/OUT  pdwCurPos-�õ���ǰ�ʼ���ȡ��λ��
  ���أ�void
������������ȡ�ʼ���ͷ�ṹ
���ã�  ��ParseEmail ���ã��ʼ�ͷ�Ľ����Ľ���ǽ����ʼ���͸����Ļ���
************************************************/
void ParseEmailHeader(HANDLE hFile, PEmailFormat  pMail,int *pdwCurPos)
{
//��ÿ��һ�еķ�ʽ����,�������б�ʾͷ��������
//��ǰpdwCurPos��ֵΪ0,��д���֧�ֶ���100���ֽڵ��У���OUTLOOK ������ȶ�����֧��100
//��Ⱥ��ʱ���ʼ���ַ�Ŀ��ܵ�д���кܶ��֣��ָ���Ҳ�����м��֡�
	CHAR * szHeaderPart;//ÿ�ζ�100���֣��������в�����100 Bytes
    CHAR ch;
    CHAR *szKey;
 
	int i=0;
	int cur=0;//���ص�ǰ���п�ʼλ�á�
    int FilePos=0;
    szHeaderPart=malloc(102);
	memset(szHeaderPart,0,102);
	szKey=malloc(100);
	//memset(szKey,0,100);
	//������100��BYTE �������ʼ�ͷ���������������Ҫ�������ж����Ҫ��ס
	//�ʼ�ͷ����λ�á�

	while (ReadVirtualLine(hFile,szHeaderPart,FilePos))
	{
        //��ʼ��һ�еĽ�����
		i=0;
		cur=0;
		memset(szKey,0,100);
		if (( ch=szHeaderPart[cur])==13 || ch ==10 || ch==0 )//��һ���ж�Ϊ�Է���һ
		{
			if ( ch == 10 )
				FilePos+=1;//�����������Ϊ�ʼ����н�����ΪCRLF.
			else
				FilePos+=2;
			break;//�˳��ʼ�ͷ����.
		}
		//��ʼ�������С����ںܶ��ʼ��������ĵ�û����ȫ����CRLF�ĸ�ʽ����β��������Ҫ�ر���
		//��Ϊ���������������ҪôCRLF��Ҫô��LF(10)
		ch=szHeaderPart[cur++];
		while(1)
		{
			if (ch!=':')
			{	 
			//һ������':'��ʾ��Ҫ�������У������¹ؼ���	
				szKey[i++]=ch;
				ch=szHeaderPart[cur++];
				if ( ch==13 || ch == 10 || ch==0 )
				{
				    //ֱ���н���ʱû�з���':'����ʾ�в�����н���������
					if ( ch == 13 )
						cur+=1;// ����0x0a 
					FilePos+=cur;
					memset(szHeaderPart,0,102);
					break;
				}
			}
			else
			{
				while( 1 )
				{	//�������ݣ�һ��LTrim �հ��ַ�
					if ((szHeaderPart[cur]==0x20) || (szHeaderPart[cur]==0x09))
					{
						cur++;
						continue;
					}
					break;
				}
				//�ؼ��Ӽ���
				if (stricmp(szKey,TEXT("From"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						//һ����ʼ��н�������0x13 0x10,����������������ı��������ļ�����
						//ʱû�н����������������ѭ������˼���0�ж�
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==10 || szHeaderPart[cur]==0  ) 
							break;
						szKey[i++]=szHeaderPart[cur++];
					}					
					if (!pMail->From)
					{
						//��̬����
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
						//һ����ʼ��н�������0x10d 0x0a,����������������ı��������ļ�����
						//ʱû�н����������������ѭ������˼���0�ж�
						if (szHeaderPart[cur]==13 || szHeaderPart[cur]==10 || szHeaderPart[cur]==0  ) 
							break;
						szKey[i++]=szHeaderPart[cur++];						
					}					
					if (!pMail->ReplyTo)
					{
						//��̬����
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
				//���Ƿ���MLG��־
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
					if (ch==0x2f)//Ѱ�ҡ�/��
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
						
						//�ȴ��������е����ݺ󣬲����������ͣ���Ϊ���������ָ�ʽ����������0x0a��β��������û�������
						i = 0;
						memset( szKey, 0,100 );
						while((ch=szHeaderPart[cur++])!=0x0a && ch!=0 && ch!= ';' && i< 99 )szKey[i++] = ch;//�ȴ�����
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
							
							//�ж��Ƿ������ ';' �ս��,����ȷ�ϸ��ַ������н���ǰ�Ƿ�����Ч����
							if ( ch == ';' ) //�ж���Ч�����Ƿ�����һ�л��ڸ��н�����
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
							{	//�������ݣ�һ��LTrim �հ��ַ�
								if ((szHeaderPart[cur])==0x20 || (szHeaderPart[cur]==0x09 ))
								{
									cur++;
									continue;
								}
								break;
							}
							//Ѱ��boundary
							memset(szKey,0,100);
							for (i=0;i<8;i++)
								szKey[i]=szHeaderPart[cur++];//�ڴ�û���ж�,Ҫ��ֹ�쳣����
							if (stricmp(szKey,TEXT("boundary"))==0)
							{
								//���boundary ֵ
								while ((ch=szHeaderPart[cur++])!=0x0D && ch!=0x0A && ch!=0x22 && ch!=0);
								if (ch==0x22)
								{				  
									//line end parsing.
									i=0;
									memset(szKey,0,100);
									while(ch=szHeaderPart[cur++])
									{//ֱ������'='Ϊֹ
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
							{//��Ϊ��ǰ����һ��ѭ����boundary.
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
					//û�з���0x5c '/'
					cur++;
					FilePos+=cur;
					memset(szHeaderPart,0,102);
                    //*********************************************************************
					//ɾ���հ��ַ���
					break;
				}
				//�ȽϽ��û�к��ʵĹؼ��֡�
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
������void ParseEmailBody(HANDLE hFile,PEmailFormat  pMail,int * pCurPos)
������
IN      hFile    -�ļ����
IN/OUT  pMail    -�ʼ���ʽ�����ݽṹ
IN/OUT  pCurPos  -�õ���ǰ�ʼ���ȡ��λ��
  ���أ�void
������������ȡ�ʼ���ṹ�����ʼ��ĸ����б�
          ���ʼ���Ľ����зֳ�����������EmailFormat.h��������������
		  �����ʼ�ͷ�е�emailtype����ȡ�ʼ�������
���ã�  ��ParseEmail ���ã��ʼ������
************************************************/
void ParseEmailBody(HANDLE hFile,PEmailFormat  pMail,int * pCurPos)
{
   int i,iBodyLen = 3073,iTempLen;
   CHAR *szLine;
   DWORD dwRead;
   CHAR ch;
   CHAR *szTemp;
   int FilePos=*pCurPos;
   //�����ʼ���ʱ��Ҫ��boundary Ϊ��־
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
	  //���ӽ����ذ��ı�����ȥ  �ȿ�500���ֹ�����
	   if (pMail->Body != NULL)
		   free( pMail->Body );
	   pMail->Body=malloc( iBodyLen );
	   memset( pMail->Body,0,iBodyLen );
	  
	   if (pMail->DeType==Quoted)
	   {//û�п��ǽ������ı������������3K�ֽڡ�
		   SetFilePointer(hFile,*pCurPos,NULL,FILE_BEGIN);
		   szLine = malloc( 3073 );
		   iTempLen = 3073;
		   szLine[0] = 0;		   
		   while (ReadFile(hFile,(LPVOID)pMail->Body,3072,&dwRead,NULL) )
		   {
	   	   // ����� Quoted �����������н�����
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
//���ڲ��ÿ���Ҫ��Ҫʶ��boundary�����Կ���ֱ������ȡ
		  }
		  free(szTemp);
		  free(szLine);
          //�����BASE64��ֻ��һ��һ�еؽ��н�����
	   }else
	   {//���ڲ���ʶ������ݣ�����Ҫ��򵥵Ŀ���
           ReadFile(hFile,(LPVOID)pMail->Body,3073,&dwRead,NULL);
		   //Copy  the text to content.
           //FilePos+=i;
	   }
	   *pCurPos=FilePos;
	   return ;
   }
	if (pMail->emailtype==MULTIPART_TYPE)
	{//��ӵĽ�����
        HandleMultiPartBody( hFile, pMail, FilePos);
	}   
   return ;
}

/*************************************************
������BOOL  ParseEmail( LPTSTR szFileName, PEmailFormat  pMail )
������
IN  szFileName  - �ʼ��ļ���
IN/OUT  pMail    -�ʼ���ʽ�����ݽṹ
���أ�BOOL TRUE  ������ȷ
           FALSE ����ʧ��
��������������һ���������ʼ�
���ã�    �������ʼ���ʽ�壬����ʼ��֮�󣬲ſ��Խ��У������ڲ���ҪʱҪ������
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
������void ConvertEmailTime( LPTSTR lpDest, LPSYSTEMTIME   lst )
������
OUT   lpDest       -ת�����ʱ���ʽ 
IN    lst          -�Ѿ���õ�ϵͳʱ��
���أ�void
������������ϵͳʱ��ת��Ϊ�����ʼ�ʱ�䣬
          ע������Ҫ��һ��ϵͳʱ��ͻ��ʱ��
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
������BOOL EmailTimeConvert(LPTSTR lpDest,LPTSTR lpSource)
������
OUT   lpDest       -ת�����ʱ���ʽ 
IN    lpSource     -ת��ǰ��ʱ���ʽ
���أ�BOOL TRUE  ������ȷ
           FALSE ����ʧ��
����������Ҫ��ʱ���ʽһ��Ҫ���ջ���INTERNET�ʼ�Э��ı�׼ʱ�������㣬���򲻸�����
          ������Ϊ�ض��İ汾������׼ʱ��ת��Ϊ���ĸ�ʽ����ʱ��ľ����ʽ����Ҫ���
		  ��ͬ�ĸ�����ȷ��,�������������ǿ�ѡ�ģ�����Ҫ���ܹ�����
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
     if ( StrPos( lpSource,',' ) )//���������ǿ�ѡ��
		 bDayOfWeek = TRUE;
	 memset( szSecond, 0, 4);
	 memset( szMiniter, 0, 4);
	 memset( szHour, 0, 4);
	 memset( szTimes, 0,4 );
	 if ( bDayOfWeek ){
	 strncpy( szTemp,lpSource,3);
	 index = 3;	 
	 if (stricmp( szTemp, TEXT("SUN")) == 0)
		 strcpy( szWeek, TEXT("������"));
     else if (stricmp( szTemp, TEXT("mon")) == 0)
		 strcpy( szWeek, TEXT("����һ"));
	 else if (stricmp( szTemp, TEXT("Tue")) ==0)
		 strcpy( szWeek, TEXT("���ڶ�"));
	 else if (stricmp( szTemp, TEXT("Wed"))==0)
		 strcpy( szWeek, TEXT("������"));
	 else if (stricmp( szTemp, TEXT("Thu"))==0)
		 strcpy( szWeek,TEXT("������"));
	 else if (stricmp( szTemp, TEXT("Fri"))==0)
		 strcpy(  szWeek, TEXT("������"));
	 else if (stricmp( szTemp, TEXT("Sat"))==0)
		 strcpy( szWeek, TEXT("������"));
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
	 //���ͳ�ʱ����ֵ
     while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09)) index++;
	 i = 0;
     while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09 && ch!=':')
	 {
         szHour[i++] = ch;      
		 index++;
	 }
     //���ͳ���
	 index++;
	 while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09) && ch == ':') index++;
	 i = 0;
     while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09 && ch!=':')
	 {
         szMiniter[i++] = ch;      
		 index++;
	 }
	 //���ͳ���
	 index++;
     while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09) && ch == ':') index++;
	 i = 0;
     while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09 && ch!=':')
	 {
         szSecond[i++] = ch;      
		 index++;
	 }
/*     //���ͳ�ʱ��
     while ((ch = lpSource[index]) && (ch!='+') && ch !='-' ) index++;
	 i = 0;
     index++;
	 while ((ch = lpSource[index]) && ch!=0x20 && ch!=0x09 && ch!=':')
	 {
         szSecond[i++] = ch;      
		 index++;
	 }
*/  //�����Ҫ�����������ڵĻ����������ɵؼ��ϸ�ֵ
	 sprintf( lpDest, TEXT("%s-%s-%s %s:%s"), szYear, szMonth, szDay,szHour,szMiniter );
	 
	 while ((ch = lpSource[index]) && (ch==0x20 || ch==0x09)) index++;
	 //not have the hour miniter,second.
	 return TRUE;
}     

/*************************************************
������BOOL SaveAttachAs( PEmailFormat  pMail , int index, LPTSTR lpEmailName , LPTSTR lpAttachName )
������
IN    pMail        -��ʾ�ʼ������ݽṹ
IN    index        -��ʾҪ����ĸ�������ֵ
IN    lpEmailName  -�ʼ����ļ���
IN    lpAttachName -Ҫ����ĸ���������
���أ�BOOL TRUE  �����ɹ�
           FALSE ����ʧ��
�������������ݸ�������Ϣ����ȡ����������Ҫ�����ʼ���֮ǰ�ܹ����͵������������Ϣ��PEmailFormat������
          һ����������Ϣ�У���С�����֣��ͽ��뷽ʽ�����ʼ��е�λ�õ���Ϣ��ֻҪ�ṩ����������Ϣ��������
		  ������
���ã� ��Ҫ���渽��ʱ�������˺���
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
	 int          iRealLen ;//����Base64�������ʱ����д��������ʱ����
	                        //��0�����ݣ����Ե�д����ʱ�������Ǵ���Base64
							//��ʱ��Ҫ��֤��ȫ������д�룬
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
                  break;//�����ƺ�����һ�����⣺��ΪBASE64����ʱ�����㶼�Ჹ�ϣ��ճ����㣬�����ڽ���ʱ�Ƿ����ʶ������أ�
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
������BOOL GetEmailHeaderInfo(LPTSTR szFileName, PEmailFormat  pMail)
������
IN    szFileName   -�ʼ���
IN    pMail        -��ʾ�ʼ������ݽṹ
���أ�BOOL TRUE  �����ɹ�
           FALSE ����ʧ��
�������������ݸ�������Ϣ����ȡ����������Ҫ�����ʼ���֮ǰ�ܹ����͵������������Ϣ��PEmailFormat������
          һ����������Ϣ�У���С�����֣��ͽ��뷽ʽ�����ʼ��е�λ�õ���Ϣ��ֻҪ�ṩ����������Ϣ��������
		  ������
���ã� ��Ҫ���渽��ʱ�������˺���
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
������BOOL FindEmailAddressFromStr( LPCTSTR szSource ,LPTSTR szDest )
������
IN    szSource       - Դ�ַ���
OUT   szDest         - Ŀ�Ĵ�,�Ѿ��õ����ʼ���ַ
����  BOOL   TRUE    �ҵ�һ����Ч�ĵ�ַ
             FALSE   û���ҵ���Ч��ַ
************************************************/
BOOL FindEmailAddressFromStr( LPCTSTR szSource ,LPTSTR szDest )
{
	int   i,j;
	CHAR  ch;
	i = StrPos( (LPTSTR)szSource,'@' );
	if (i > 0 )
	{
        //�ҵ���һ���ַ�
		i--;
		while ( i-- )
		{
			ch = szSource[i];
			if ( ch ==' ' || ch == 0x09 || ch == 34 || ch == '<' )
				break;//�ҵ���һ��.
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
������BOOL IsDraftBox( LPCTSTR szFileName )
������
IN      szFileName       - һ���ʼ������ĵ�ַ
����ֵ�� BOOL  TRUE ��DRAFT�ʼ�
               FALSE����DRAFT�ʼ�
���������� �ж��Ƿ� draft �ʼ�
���ã�  
************************************************/
BOOL IsDraftBox( LPCTSTR szFileName )
{
	BOOL bRet = FALSE;
	if (strstr( szFileName,"\\receive\\" ))
		bRet = TRUE;
	return bRet;
}

