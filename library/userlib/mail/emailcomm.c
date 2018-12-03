/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

/**************************************************
�ļ�˵����Email���ʼ����õ��ⲿ�ӿڣ���һЩ�����ӿڣ����ⲿAP����ʱ
         ֻ��Ҫ��ʼ��һ�����ݽṹ��EmailFormat Ȼ���ٵ��� SendEmailTo����

�汾�ţ�1.0.0
����ʱ�ڣ�2003-5-10
���ߣ�������
�޸ļ�¼����������Ҫ�ⲿ���ú󣬸�����EasyBox�ļܹ������������в������õķ�ʽ��
          ���ʼ�����ʼ����½��ʼ� 2003-6-20 by ������
**************************************************/

#include "ewindows.h"
#include "EmailFormat.h"
#include "MailBoxApi.h"
BOOL SaveTempFile(PEmailFormat pEmailFormat );
extern void CreateEmailDirectory();
/*************************************************
������void OpenEmail( CHAR *szTempFile )
������
IN  szTempFile   - �ʼ��ļ���
���أ�void
���������������ʼ��Ի���
���ã�   
************************************************/
void OpenEmail( CHAR *szTempFile )
{
	CreateEmailDirectory();
#ifdef INLINE_PROGRAM
	RETAILMSG(TME,("Before LoadApplication1\r\n"));
	LoadApplication("ReadBox",szTempFile);
#else
{    
	 STARTUPINFO  si;
	 PROCESS_INFORMATION pi;

	 RETAILMSG(TME,("Before CreateProcess %s \r\n",szTempFile));
	 CreateProcess("\\����\\readbox.exe",szTempFile,0,0,
		          0,0,0,0,0,0 );
		RETAILMSG(TME,("After CreateProcess %s \r\n",szTempFile));
	}
 
//	RETAILMSG(TME,("Before LoadApplication2\r\n"));
//    LoadApplication("ReadBox",szTempFile);
#endif
}

/*************************************************
������BOOL SendEmailTo( PEmailFormat  pEmailFormat )
������
IN  pEmailFormat   - �ʼ����ݽṹ��������ʼ�����ʼ��Ի���
���أ�void
�������������������ʼ���ظ���ʱ����
���ã�   
************************************************/
BOOL SendEmailTo( PEmailFormat  pEmailFormat )
{
	BOOL bp = FALSE;
	
	CreateEmailDirectory();
	if ( pEmailFormat ){
	    bp = TRUE;
	}
//	PROCESS_INFORMATION pi;
//�����һ����ʱ�ļ�������ʾ���Ե��ļ�����,���Ȼ��һ��Ψһ���ַ���
//Ȼ���ٽ����ݱ��棬��NewBox�򿪺��ȡ��������ɾ��
	if ( bp )
		if ( !SaveTempFile( pEmailFormat ) )
			return FALSE;
#ifdef INLINE_PROGRAM
	RETAILMSG(TME,("Before LoadApplication3\r\n"));
	if  ( bp )
		LoadApplication("NewBox",szECTempFileName);
	else 
		LoadApplication( "NewBox",NULL);
#else
	RETAILMSG(TME,("Before LoadApplication4\r\n"));
		{
			STARTUPINFO  si;
	  PROCESS_INFORMATION pi;
	 if ( bp) 
		CreateProcess("\\����\\newbox.exe",szECTempFileName,0,0,
		          0,0,0,0,0,0 );
	  else
        CreateProcess("\\����\\newbox.exe",0,0,0,
		          0,0,0,0,0,0 );
		}
	RETAILMSG(TME,("Before LoadApplication4\r\n"));

#endif 
	return TRUE;

}

/*************************************************
������BOOL SaveTempFile(PEmailFormat pEmailFormat )
������
IN  pEmailFormat   - �����ʼ�����ʱ�ļ��У����ļ�����Ϊ��������
���أ�void
�������������������ʼ���ظ���ʱ����
���ã�   
************************************************/
BOOL SaveTempFile(PEmailFormat pEmailFormat )
{
	HANDLE hFile;
	DWORD  dwWrite;
	int    iLen;

	if ( (hFile = CreateFile( szECTempFileName,
		                     GENERIC_READ|GENERIC_WRITE, 0, NULL, 
							 CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL )) == INVALID_HANDLE_VALUE ) 
	    return FALSE;
	 RETAILMSG(TME,("CreateFile TRUE in SaveTempFile\r\n"));
     if ( pEmailFormat->To )
	 {
		 iLen = strlen( pEmailFormat->To );
		 WriteFile( hFile,"To:",3,&dwWrite,NULL );
		 WriteFile( hFile,pEmailFormat->To,iLen,&dwWrite,NULL);
		 WriteFile( hFile,"\r\n",2,&dwWrite,NULL );
	 }
	 if ( pEmailFormat->Subject )
	 {
		 iLen = strlen( pEmailFormat->Subject );
		 WriteFile( hFile,"Subject:",8,&dwWrite,NULL );
		 WriteFile( hFile,pEmailFormat->Subject,iLen,&dwWrite,NULL);
		 WriteFile( hFile,"\r\n",2,&dwWrite,NULL );
	 }
	 if ( pEmailFormat->Cc )
	 {
		 iLen = strlen( pEmailFormat->Cc );
		 WriteFile( hFile,"Cc:",3,&dwWrite,NULL );
		 WriteFile( hFile,pEmailFormat->Cc,iLen,&dwWrite,NULL);
		 WriteFile( hFile,"\r\n",2,&dwWrite,NULL );
	 }
	 
	 if ( pEmailFormat->AttachCount )
	 {   //����һ���ܲ����淶�Ĳ�������Ϊ����EmailFormat ���������ʼ������ļ��Ľṹ
		 //����Ŀǰ�½���򿪵Ĳ�������Ҫ�����ݻ�����ͬ���̱���ʹ�ã�Ҫע�������
		 //�����Ľṹ�е����ݱ���������ͬ������ֻ��Ҫ�������е�Attach->Name������������
		 //����Name��ʾ���ǣ��û�Ҫ���͵ĸ�����ȫ·����.
		 //�������Ķ�ȡ��(�ݸ���)�ǲ���Ҫ��ʾ������,��Ϊ���ǵ�ԭ����ֻ�ݸ����ǲ��ܱ��渽����
		 //��ҪΪ��ʵAP�ķ����ṩ�Ľӿ�.��:IE,���±���.����Ϊ�˰�ȫ���,���ǽ����������ǵ���ʱλ��.
		 CHAR *szFileName;
		 CHAR *szPathName;
		 int i;
		 
		 szFileName = malloc( 256 );
		 szPathName = malloc( 256 );
		 WriteFile( hFile,"AttachList:",11,&dwWrite,NULL);
		 for (i = 0;i<pEmailFormat->AttachCount;i++)
		 { 
			 memset( szFileName,0,256 );
			 memset( szPathName,0,256 );
			 //��ø������ļ���,ͬʱҪ��֤���еĸ������ļ���Ҫ����ͬ
             GetFileNameEx( pEmailFormat->Attach[i]->Name,szFileName,NULL );
			 sprintf( szPathName,TEXT("%s\\%s"),szECTempDirectory,szFileName );
             if ( !CopyFile( pEmailFormat->Attach[i]->Name,szPathName,FALSE ) )
			 {
				 CloseHandle( hFile );
				 free( szFileName );
				 free( szPathName );
				 return FALSE;
			 }
			 iLen = strlen( szPathName );
			 WriteFile( hFile,szPathName,iLen,&dwWrite,NULL);
			 WriteFile( hFile,";",1,&dwWrite,NULL);
		 }
		 free( szFileName );
		 free( szPathName );
		 WriteFile( hFile,"\r\n",2,&dwWrite,NULL ); 
	 }
	 if ( pEmailFormat->BodyType || pEmailFormat->Body )
	 {
		 CHAR   temp[6];
		 int    iCurPos = 0;
		 DWORD  dwWriteLen = 90;
		 WriteFile( hFile,"BodyType:",9,&dwWrite,NULL);
		 sprintf( temp,"%d\r\n", pEmailFormat->BodyType);
		 WriteFile( hFile,temp,3,&dwWrite,NULL);
		 //dwWriteLen = (iLen - 90)>0 ? 90:iLen;
		 //while ( iCurPos != iLen ){
		 WriteFile( hFile,"Body:",5,&dwWrite,NULL );
		 if( pEmailFormat->Body ){
			 iLen = strlen( pEmailFormat->Body );
			 WriteFile( hFile,pEmailFormat->Body/*+iCurPos,dwWriteLen*/, iLen,&dwWrite,NULL);
		 }
		 else
		 {
			 WriteFile(hFile,"  ",1,&iLen,NULL );
		 }
		 //WriteFile( hFile,"\r\n",2,&dwWrite,NULL );
		 //iCurPos +=dwWrite;
		 //dwWriteLen = ( (iLen-iCurPos) > 90 )?90:(iLen - iCurPos);
		 
		 
	 }
	 RETAILMSG(TME,("CloseFile in SaveTempFile\r\n"));
	 CloseHandle( hFile );
	 return TRUE;
}

/*************************************************
������BOOL ReadTempFile(PEmailFormat pMail,CHAR * szFileName)
������
OUT   pMail       - ���ص��ʼ������ݸ�ʽ
IN    szFileName  - �����ʼ����ݵ���ʱ�ļ�
����ֵ��BOOL TRUE   �����ݳɹ�
             FALSE  ������ʧ�� 
����������
���ã� ����ʱ�ļ��ж����ʼ���Ϣ��ɾ����ʱ�ļ�
************************************************/
BOOL ReadTempFile(PEmailFormat pMail,CHAR * szFileName)
{
	HANDLE hFile;
	DWORD  dwRead;
	CHAR   szLine[102],szKey[102];
	int    iLineLen,iPos = 0;
    int    i = 0,cur = 0;
	CHAR   ch;
    BOOL   bOver = FALSE;
	if ( (hFile = CreateFile( szFileName,
		GENERIC_READ|GENERIC_WRITE, 0, NULL, 
							 OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL )) == INVALID_HANDLE_VALUE )
							 return FALSE;
	RETAILMSG(TME,("CreateFile in ReadTempFile\r\n"));
	memset( szLine, 0, 102 );
	memset( szKey,0, 102 );
	while( ReadVirtualLine( hFile,szLine,iPos) )
	{
		TrimLines(szLine,&iLineLen);//�����ж���Ľ�β��
		i = 0;
		cur= 0;
        ch = szLine[cur++];
		
		memset(szKey,0,102);
		while ( 1 )
		{
			if ( ch !=':' )
			{
				szKey[i++] = ch;
				ch=szLine[cur++];
				if ( ch == 0 )
				{
					//�����в����������󣬱���û�а������������̲���
					break;
				}
			}
			else
			{//�����ؼ��ֺ��ڽ������ݵıȽϵõ�����
               	//�ؼ��Ӽ���
				if (stricmp(szKey,TEXT("To"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						//һ����ʼ��н�������0x13 0x10,����������������ı��������ļ�����
						//ʱû�н����������������ѭ������˼���0�ж�
						if (szLine[cur]==0  ) 
							break;
						szKey[i++]=szLine[cur++];
					}					
					if (!pMail->To)
					{
						//��̬����
						pMail->To=(LPTSTR)malloc(i+1);
						memset(pMail->To,0,i+1);
						ParseString(szKey,pMail->To);
					}
					memset(szLine,0,102);
					break;
				}
				if (stricmp(szKey,TEXT("Subject"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						//ʱû�н����������������ѭ������˼���0�ж�
						if (szLine[cur]==0  ) 
							break;
						szKey[i++]=szLine[cur++];
					}					
					if (!pMail->Subject)
					{
						//��̬����
						pMail->Subject=(LPTSTR)malloc(i+1);
						memset(pMail->Subject,0,i+1);
						ParseString(szKey,pMail->Subject);
					}
					memset(szLine,0,102);
					break;
				}
				
				if (stricmp(szKey,TEXT("AttachList"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						//ʱû�н����������������ѭ������˼���0�ж�
						if ( (ch = szLine[cur])==';'|| ch == 0) 
						{
							PEmailAttach p;
							
							if ( strlen( szKey ) == 0 )
								break;
							p = malloc( sizeof(EmailAttach) );
							TrimBlankStr(szKey);
							
							p->Name = malloc( sizeof( szKey )+2);
							strcpy( p->Name,szKey);
							
							if ( pMail->AttachCount == pMail->MaxCount )
							{
								pMail->Attach = realloc( pMail->Attach,sizeof(EmailAttach)*(pMail->AttachCount + 2));
								pMail->MaxCount +=2;
							}
							pMail->Attach[pMail->AttachCount++] = p;
							memset( szKey,0, 102 );
							if ( ch == 0 )
								break;
							cur++;
							i = 0;
						}
						szKey[i++]=szLine[cur++];
					}					
					memset(szLine,0,102);
					break;
				}
				if ( stricmp( szKey,TEXT("BodyType") )== 0)
				{
                    i=0;
					memset(szKey,0,100);
					while(1)
					{
						//һ����ʼ��н�������0x13 0x10,����������������ı��������ļ�����
						//ʱû�н����������������ѭ������˼���0�ж�
						if (szLine[cur]==0  ) 
							break;
						szKey[i++]=szLine[cur++];
					}					
						//��̬����
					pMail->BodyType = atoi( szKey );
					memset(szLine,0,102 );
					break;
				}
				if (stricmp(szKey,TEXT("Body"))==0)
				{						
					int iBodyLen = 2048;
					i=0;
					iPos += 5;
					memset(szKey,0,102);
					if ( pMail->Body )
						free( pMail->Body );
					pMail->Body = malloc( iBodyLen );
					memset( pMail->Body,0,iBodyLen );
					memset( szKey,0, 102 );
					SetFilePointer( hFile,iPos,NULL,FILE_BEGIN);
					while( ReadFile( hFile,szKey,100,&dwRead,NULL) && dwRead!= 0)
					{
						//ʱû�н����������������ѭ������˼���0�ж�
						if ( (int)(strlen( szKey) + strlen( pMail->Body)+1) > iBodyLen )
						{
							if ( iBodyLen > 200*1024 )
							{
								bOver = TRUE;
								break;
							}	
							pMail->Body = realloc( pMail->Body,iBodyLen*2);
							pMail->Body[iBodyLen] = 0;
							iBodyLen *=2;
														
						}
						strcat( pMail->Body,szKey );
						memset( szKey,0, 102 );
					}
					bOver = TRUE;
					break;
				}
			}

		}
		if ( bOver )
			break;		
		iPos +=iLineLen;
	}
	RETAILMSG(TME,("CloseFile in ReadTempFile\r\n"));
	CloseHandle( hFile );
    return TRUE;    
}

/*************************************************
������BOOL  CreateAttachList( HWND  hListView, DWORD dwStatus )
������
IN  hListView    - ����ListView�ľ��
IN  dwStatus     - ����ListView�ķ��
���أ�BOOK  TRUE �ɹ�
            FALSE ʧ��   
������������������ListView����
���ã���CreateControls_New����
************************************************/
BOOL  CreateAttachList( HWND  hListView, DWORD dwStatus )
{
	short			i;
	TCHAR           szHeader[2][10]={"�ļ���","��С"};
	LV_COLUMN		lvcolumn;
	short           column= 1;
	DWORD           dwWinAttr;
	short			columnwidth[]={240};
    
	dwWinAttr=GetWindowLong( hListView,GWL_STYLE );
	dwWinAttr &=~LVS_TYPEMASK;
	dwWinAttr |=dwStatus;
	SetWindowLong(hListView,GWL_STYLE, dwWinAttr );
	for ( i=0; i<column; i++ )  {
		lvcolumn.mask = LVCF_FMT | LVCF_WIDTH |LVCF_SUBITEM | LVCF_TEXT ;
		if(i>0)
			lvcolumn.fmt = LVCFMT_LEFT ;
		else
			lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE;

		lvcolumn.pszText = szHeader[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = columnwidth[i];     // width of column.
		lvcolumn.cchTextMax =strlen (szHeader[i]);
		lvcolumn.iImage = 1 ;
		
		if( SendMessage(hListView,LVM_INSERTCOLUMN,i,(LPARAM)&lvcolumn)==-1 ){
			return FALSE;
		}
	}
	
	return TRUE;
}

/*************************************************
������void InsertListViewItem(HWND hListView,short iItem, short itemSub,
							   CHAR* lpItem,short iImage )

������
IN        hListView  - ��ʽ��
IN        iItem      - ������
IN        itemSub    - ��������
IN        lpItem     - �������ַ���
IN        iImage     - ������ͼ������  
����ֵ��  void
�������������ÿһ������
���ã�    ������ʼ���ʱ��Ҫ�����������
************************************************/
void InsertListViewItem(HWND hListView,short iItem, short itemSub,
							   CHAR* lpItem,short iImage )
{
	LV_ITEM				lvitem;
	
	lvitem.mask			= LVIF_TEXT | LVIF_PARAM |LVIF_IMAGE ;
	lvitem.iItem		= iItem;     // the item index,where the item shows. 
	lvitem.iSubItem		= itemSub;
	lvitem.pszText		= lpItem;
	lvitem.cchTextMax	= strlen( lpItem );
	lvitem.lParam		= iItem;
	lvitem.iImage       = iImage ;

	SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem );
}

//copy from Jami 2003-09-12
static BOOL Email_MergeFullFileName(LPTSTR lpFullName,LPCTSTR lpPath,LPCTSTR lpFileName)
{
	if (lpPath != NULL)
		strcpy(lpFullName,lpPath);
	if (strcmp(lpFullName,"\\") == 0)
	{
		if ((strlen(lpFullName) + strlen(lpFileName)) >= MAX_PATH)
			return FALSE;
		strcat(lpFullName,lpFileName);
	}
	else
	{
		if ((strlen(lpFullName) + strlen(lpFileName) +1) >= MAX_PATH)
			return FALSE;
		strcat(lpFullName,"\\");
		strcat(lpFullName,lpFileName);
	}
	return TRUE;
}

static BOOL Email_DeleteDirectory(LPTSTR lpFullName)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	TCHAR						*lpstrDir;
	BOOL bRet = TRUE;

		lpstrDir = (LPTSTR)malloc(MAX_PATH);

		if (lpstrDir == NULL)
			return FALSE;
//��λ�䱸·��
		if (Email_MergeFullFileName(lpstrDir,lpFullName,"*.*") == FALSE)
			goto DELETE_END;
		hFind=FindFirstFile(lpstrDir, &FindFileData);  // ���ҵ�һ���ļ�
		if( hFind == INVALID_HANDLE_VALUE) 
		{// �ļ�������
			bRet =RemoveDirectory(lpFullName);
			goto DELETE_END;
		}
		while(TRUE)
		{  // �ļ�����
			if(FindFileData.cFileName[0]=='.')
			{  // ���ļ���һ����ʾ��ǰĿ¼����Ŀ¼���ļ���������
			}
			else
			{
				if (Email_MergeFullFileName(lpstrDir,lpFullName,FindFileData.cFileName) == FALSE)
					goto DELETE_END;
				if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
				{
					// ���ҵ�����һ��Ŀ¼
					bRet = Email_DeleteDirectory(lpstrDir);
				}
				else
				{
					// ���ҵ�����һ���ļ�
					bRet = DeleteFile(lpstrDir);
					if (bRet == FALSE)
					{
						goto DELETE_END;
					}
				}
			}
			if( FindNextFile(hFind,&FindFileData)==0 )  //������һ���ļ�
			{
				FindClose(hFind);
//				free(lpstrDir);
				bRet =RemoveDirectory(lpFullName);
				goto DELETE_END;
			}
		}

DELETE_END:
		if (lpstrDir)
			free(lpstrDir);
		return bRet;
}
//copy from Jami 2003-09-12

BOOL Email_ClearTempDirectory()
{
	Email_DeleteDirectory(szECTempDirectory);
	return TRUE;
}
