/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

/**************************************************
文件说明：Email，邮件调用的外部接口，及一些公共接口，当外部AP调用时
         只需要初始化一个数据结构，EmailFormat 然后再调用 SendEmailTo即可

版本号：1.0.0
开发时期：2003-5-10
作者：满益文
修改记录：在讨论需要外部调用后，更改了EasyBox的架构，改用命令行参数调用的方式来
          打开邮件或读邮件，新建邮件 2003-6-20 by 满益文
**************************************************/

#include "ewindows.h"
#include "EmailFormat.h"
#include "MailBoxApi.h"
BOOL SaveTempFile(PEmailFormat pEmailFormat );
extern void CreateEmailDirectory();
/*************************************************
声明：void OpenEmail( CHAR *szTempFile )
参数：
IN  szTempFile   - 邮件文件名
返回：void
功能描述：　打开邮件对话框
调用：   
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
	 CreateProcess("\\程序\\readbox.exe",szTempFile,0,0,
		          0,0,0,0,0,0 );
		RETAILMSG(TME,("After CreateProcess %s \r\n",szTempFile));
	}
 
//	RETAILMSG(TME,("Before LoadApplication2\r\n"));
//    LoadApplication("ReadBox",szTempFile);
#endif
}

/*************************************************
声明：BOOL SendEmailTo( PEmailFormat  pEmailFormat )
参数：
IN  pEmailFormat   - 邮件数据结构，用来初始化新邮件对话框
返回：void
功能描述：　发送新邮件或回复等时调用
调用：   
************************************************/
BOOL SendEmailTo( PEmailFormat  pEmailFormat )
{
	BOOL bp = FALSE;
	
	CreateEmailDirectory();
	if ( pEmailFormat ){
	    bp = TRUE;
	}
//	PROCESS_INFORMATION pi;
//保存成一个临时文件用来表示初试的文件内容,首先获得一个唯一的字符串
//然后再将数据保存，在NewBox打开后读取后必须进行删除
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
		CreateProcess("\\程序\\newbox.exe",szECTempFileName,0,0,
		          0,0,0,0,0,0 );
	  else
        CreateProcess("\\程序\\newbox.exe",0,0,0,
		          0,0,0,0,0,0 );
		}
	RETAILMSG(TME,("Before LoadApplication4\r\n"));

#endif 
	return TRUE;

}

/*************************************************
声明：BOOL SaveTempFile(PEmailFormat pEmailFormat )
参数：
IN  pEmailFormat   - 保存邮件到临时文件中，将文件名作为参数传给
返回：void
功能描述：　发送新邮件或回复等时调用
调用：   
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
	 {   //这是一个很不符规范的操作，因为本来EmailFormat 是用来读邮件数据文件的结构
		 //由于目前新建或打开的操作所需要的数据基本相同，固保留使用，要注意的是是
		 //附件的结构中的数据表现有所不同：这里只需要设置其中的Attach->Name及附件的总数
		 //其中Name表示的是，用户要传送的附件的全路径名.
		 //在正常的读取中(草稿箱)是不需要显示附件的,因为我们的原则是只草稿箱是不能保存附件的
		 //主要为其实AP的服务提供的接口.如:IE,记事本等.但是为了安全起见,我们将保存至我们的临时位置.
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
			 //获得附件的文件名,同时要保证所有的附件的文件名要求不相同
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
声明：BOOL ReadTempFile(PEmailFormat pMail,CHAR * szFileName)
参数：
OUT   pMail       - 返回的邮件的数据格式
IN    szFileName  - 保存邮件数据的临时文件
返回值：BOOL TRUE   读数据成功
             FALSE  读数据失败 
功能描述：
引用： 从临时文件中读出邮件信息并删除临时文件
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
		TrimLines(szLine,&iLineLen);//不含有多余的结尾符
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
					//发现有不正常的现象，比如没有按照正常的流程操作
					break;
				}
			}
			else
			{//读出关键字后，在进行数据的比较得到数据
               	//关键子记下
				if (stricmp(szKey,TEXT("To"))==0)
				{						 
					i=0;
					memset(szKey,0,100);
					while(1)
					{
						//一般的邮件行结束都是0x13 0x10,但是如果是其他的文本可能在文件结束
						//时没有结束符，则可能陷入循环，因此加上0判断
						if (szLine[cur]==0  ) 
							break;
						szKey[i++]=szLine[cur++];
					}					
					if (!pMail->To)
					{
						//动态分配
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
						//时没有结束符，则可能陷入循环，因此加上0判断
						if (szLine[cur]==0  ) 
							break;
						szKey[i++]=szLine[cur++];
					}					
					if (!pMail->Subject)
					{
						//动态分配
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
						//时没有结束符，则可能陷入循环，因此加上0判断
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
						//一般的邮件行结束都是0x13 0x10,但是如果是其他的文本可能在文件结束
						//时没有结束符，则可能陷入循环，因此加上0判断
						if (szLine[cur]==0  ) 
							break;
						szKey[i++]=szLine[cur++];
					}					
						//动态分配
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
						//时没有结束符，则可能陷入循环，因此加上0判断
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
声明：BOOL  CreateAttachList( HWND  hListView, DWORD dwStatus )
参数：
IN  hListView    - 附件ListView的句柄
IN  dwStatus     - 附件ListView的风格
返回：BOOK  TRUE 成功
            FALSE 失败   
功能描述：创建附件ListView的列
引用：在CreateControls_New调用
************************************************/
BOOL  CreateAttachList( HWND  hListView, DWORD dwStatus )
{
	short			i;
	TCHAR           szHeader[2][10]={"文件名","大小"};
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
声明：void InsertListViewItem(HWND hListView,short iItem, short itemSub,
							   CHAR* lpItem,short iImage )

参数：
IN        hListView  - 列式框
IN        iItem      - 行索引
IN        itemSub    - 子项索引
IN        lpItem     - 添加项的字符串
IN        iImage     - 添加项的图象索引  
返回值：  void
功能描述：添加每一个主项
引用：    当添加邮件项时，要求这样先添加
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
//定位配备路径
		if (Email_MergeFullFileName(lpstrDir,lpFullName,"*.*") == FALSE)
			goto DELETE_END;
		hFind=FindFirstFile(lpstrDir, &FindFileData);  // 查找第一个文件
		if( hFind == INVALID_HANDLE_VALUE) 
		{// 文件不存在
			bRet =RemoveDirectory(lpFullName);
			goto DELETE_END;
		}
		while(TRUE)
		{  // 文件存在
			if(FindFileData.cFileName[0]=='.')
			{  // 该文件是一个表示当前目录是子目录的文件，跳过。
			}
			else
			{
				if (Email_MergeFullFileName(lpstrDir,lpFullName,FindFileData.cFileName) == FALSE)
					goto DELETE_END;
				if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
				{
					// 查找到的是一个目录
					bRet = Email_DeleteDirectory(lpstrDir);
				}
				else
				{
					// 查找到的是一个文件
					bRet = DeleteFile(lpstrDir);
					if (bRet == FALSE)
					{
						goto DELETE_END;
					}
				}
			}
			if( FindNextFile(hFind,&FindFileData)==0 )  //查找下一个文件
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
