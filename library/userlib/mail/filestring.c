/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

/**************************************************
�ļ�˵�����ļ��ַ�����������.
�汾�ţ�1.0.0
����ʱ�ڣ�2003-3-12
���ߣ�������
�޸ļ�¼��
**************************************************/
#include <ewindows.h>
#include "emailformat.h"

/*************************************************
������void TrimLines(LPTSTR szLine,int * pPos)
������
IN      szLine           -�ַ���
IN/OUT  pPos           -�õ��ø��ַ��������ַ���

  ���أ�void
������������һ�ַ����еõ�һ��Ч���У�����email �й�Э�飬���е��ʼ�����ĩβ����CRLF
���ã�    ����Ҫһ����������ʱ���ã����ܵ�������������е��ַ�û���н�������
          ע���в�ͬ�ĵط���  TrimLines  ��  TrimLines2 ������
		  ǰ���ǣ��õ� ��CRLF�����Ч���ַ���
		  �����ǣ��õ���CRLFΪ��־������һ�У�û��CRLFʱҲ��һ��
************************************************/
void TrimLines(LPTSTR szLine,int * pPos)
{
   CHAR ch;
   int  i = 0;
   for (;;)
   {
	   if ((ch = szLine[i])==13 || ch == 10 || ch==0)
		   break;
	   i++;
   }
   szLine[i] = 0;
   if  (ch == 10 )
	   i +=1;
   else if ( ch == 13)
	   i += 2;
   *pPos = i;
}

/*************************************************
������void TrimLines2(CHAR *szLine,int *piPos)
������
IN      szLine           -�ַ���
IN/OUT  piPos           - �õ��ø��ַ��������ַ���,��	��ȷ��QUOTED���뷽ʽ���ַ�����
                          Ҫ���ǲ���������ַ�
  ���أ�void
������������һ�ַ����еõ�һ��Ч���У�����email �й�Э�飬���е��ʼ�����ĩβ����CRLF��
          ͬʱ�����ڸú�����Ҫ��Խ���Quoted�������,��Ҫ�����ݵ��н��нضϣ��������һ����������
		  ����Ҫ��Ҫ�Ѻ����Quoted���ݶ��ƻ�
���ã�    ����Ҫһ����������ʱ���ã����ܵ�������������е��ַ�û���н�������ʱ
************************************************/
void TrimLines2(CHAR *szLine,int *piPos)
{
	int i = 0;
	CHAR ch;
	while (1)
	{
		if ((ch=szLine[i])==10 || ch==0 )
		{		
			break;
		}
		i++;
	}
	if ( ch == 10 )
		i++;
	if ( ch == 0 ) 
	{//˵�������ݲ���һ���������У���Ҫ���¼���,����Quoted-printable�����ݸ�ʽ
        i -=2;
		if ((ch = szLine[i]) != '=' )
		{
            if ( (ch = szLine[++i]) != '=')
		          i++;
		}
	}
	*piPos = i;
	szLine[i] = 0;
}

/*************************************************
������BOOL ReadVirtualLine(HANDLE hFile,CHAR *Aline,int CurPos)
������
IN      hFile           -�ļ����
IN/OUT  Aline           -��ȡ�ַ�����BUFFER 
IN  CurPos              -��ȡǰ ���ʼ��ļ���ǰ��λ��
���أ�BOOL  TRUE �ɹ���ȡ
            FALSE ��ȡʧ��
������������ȡһ���������ַ� ���ú�����ReadVirtualLine2�Ĳ�֮ͬ�����ڣ���ֻ��ȡһ���������ַ�
          ����ȡ�У����ϲ㺯��������ReadVirtualLine2���ȡһ��������
���ã�   
************************************************/
BOOL ReadVirtualLine(HANDLE hFile,CHAR *Aline,int iCurPos)
{
	DWORD dwRead;
	
	SetFilePointer(hFile,iCurPos,NULL,FILE_BEGIN);
	if (ReadFile(hFile,Aline,100,&dwRead,NULL)==0  || dwRead==0)
		return FALSE;
    return TRUE;
}

/*************************************************
������BOOL ReadVirtualLine2(HANDLE hFile,LPTSTR ALine,int * pnLineLength,int * pFilePos)
������
IN      hFile           -�ļ����
IN/OUT  pnLineLength    -���е����ݵ�λ��
IN  CurPos  -�õ���ǰ�ʼ���ȡ��λ��
  ���أ�void
�����������ڶ��еĽ����У�ע�������Printable ��ʽ��������һ���д��ڶ���100���ֽڵ����ݡ�
          Ҫ�Ľ���һ������£�BASE64��ʽ������ڶ���100���ֽڵ��С�
���ã�  ���ʼ�Э�����ʱ�����õ��˺���
************************************************/
BOOL ReadVirtualLine2(HANDLE hFile,LPTSTR szALine,int * pnLineLength,int * pFilePos)
{
    DWORD dwRead;
	int iPos = 0;
		
	SetFilePointer( hFile, *pFilePos, NULL, FILE_BEGIN);
	if (ReadFile( hFile, szALine, 100, &dwRead, NULL)==0  ||  dwRead==0)
		return FALSE;
	TrimLines( szALine, &iPos);
	*pnLineLength = iPos;
	*pFilePos = *pFilePos + iPos;
    return TRUE;
}

/*************************************************
������BOOL FindStringBeforeCh(LPTSTR lpString,CHAR key,LPTSTR szKey)
������
IN      lpString        -Ը�ַ���
IN      key             -�ؼ���
OUT     szKey           -�õ���Ҫ���ַ���
���أ�void
���������� ���ֹؼ���֮ǰ���ַ���
���ã�   ���ʼ����������� ����
************************************************/
BOOL FindStringBeforeCh(LPTSTR lpString,CHAR key,LPTSTR szKey)
{
	int i = 0;
	CHAR ch;
    BOOL bFound = FALSE;
    while (ch = lpString[i++] )
	{
          if (ch == key )
		  {
			  bFound = TRUE;
			  break;
		  }
	}
    if ( bFound )
	{
		i-=1;
        strncpy( szKey, lpString, i);
		return TRUE;
	}
	else 
		return FALSE;
}
/*************************************************
������void TrimString(LPTSTR szLine)
������
IN/OUT      szLine        -Դ�ַ���
���أ�void
�������������ַ�����ߵ��ַ�ȫ��Skip 
���ã�   ���ʼ����������� ����
************************************************/
void TrimString(LPTSTR szLine)//trim left blank char
{
	int i = 0 ;
	int index = 0;
	CHAR ch;
	CHAR ALine[512];
	memset(ALine,0,512);
   for (;;)//trim left blank char.
   {
       if ((ch = szLine[i])==0x09 || ch==0x20  )
	   {
		   i++;
		   continue;
	   }
	   break;
   }
   for (;;)
   { 
       if ((ch = szLine[i])!=0)
	   {
		   ALine[index++]=ch;
	       i++;           
	   }
	   else
		   break;
   }
   strcpy( szLine, ALine);
}

/*************************************************
������int StrPos(CHAR * szLine,CHAR ch)
������
IN     szLine           -Ը�ַ���
IN      ch             -�ؼ���
���أ�int     ���ظ��ַ��ڴ��еĵ�һ��λ�� 0 ��ʾ������
���������� ���ֹؼ����ڴ��е�λ��
���ã�   
************************************************/
int StrPos(CHAR * szLine,CHAR ch)
{
    UINT i;
    CHAR c;
	if ( szLine == NULL ) return 0;
	for (i=0;i<strlen( szLine);i++)
	{
		if ((c = szLine[i])==ch)
			return (++i);
	}
    return 0;
}

/*************************************************
������void GetFileNameEx( LPTSTR lpPath, LPTSTR lpName,int *piExt )
������
IN     lpPath          -�ļ�����·��
OUT    lpName          -���ص��ļ���
OUT    *piExt          -��չ����ʾ��
���أ�void
���������� Ѱ���ļ�����·���е��ļ���
���ã�   
************************************************/
void GetFileNameEx( LPTSTR lpPath, LPTSTR lpName,int *piExt )
{
	int iPos, iLen,i;
	CHAR  ch;
	CHAR  szExt[5];
	BOOL  bFind = FALSE;
	memset( szExt, 0,5 );
	iLen = iPos = strlen( lpPath );
	iPos--;
    if ( piExt )
	{
		*piExt = 0;
		for ( i = 0;i< 5,iPos>=0;i++)
		{
			if ( (ch=lpPath[iPos--])=='.')
			{
				bFind = TRUE;
				iPos +=2;
				break;
			}		
		}
	
	if ( bFind )
	{
		strncpy( szExt,lpPath+iPos,iLen-iPos);
		if ( stricmp( szExt,TEXT("bmp"))==0 ) 
			*piExt = 1;
		else if ( stricmp( szExt,TEXT("TXT"))==0 ) 
			*piExt = 2;
		else if ( stricmp( szExt,TEXT("MP3"))==0 )
			*piExt = 3;
		else if ( stricmp( szExt,TEXT("htm"))== 0 )
			*piExt = 4;
		else if ( stricmp( szExt,TEXT("mlg")) == 0 )
			*piExt = 5;
		else if ( stricmp( szExt,TEXT("html"))== 0 )
			*piExt = 4;
		else if ( stricmp( szExt,TEXT("exe"))== 0 )
			*piExt = 6;
		else if ( stricmp( szExt,TEXT("gif"))== 0 )
			*piExt = 7;
		else if ( stricmp( szExt,TEXT("bmp"))== 0 )
			*piExt = 8;
		else 
			*piExt = 0;

	   bFind = FALSE;
	}
	}
	if ( lpName==NULL ){ return ;}
	while ( iPos >= 0 )
	{
		if ( (ch = lpPath[iPos]) == '\\' )
		{
			bFind = TRUE;
			break;
		}
		iPos-- ;
	}
	if ( iPos != -1 )
	{
		iLen = iLen - iPos - 1;
	}
	else
	{		
		iLen = iLen - iPos;
	}
	iPos++;
	if ( iLen )
	      strncpy( lpName, lpPath + iPos, iLen );
	else
		lpName[0] = 0;
    
}

/*************************************************
������void GetTimeStr(CHAR *szStr)
������
IN  szStr - �õ���ʱ���йص��ַ���,ע�ⳤ��Ҫ������10Bytes
���أ�void
�������������õ���ʱ���йص��ַ�������ҪΪ�����ʼ�����
���ã�   
************************************************/
void GetTimeStr(CHAR *szStr )
{//��֤�������㹻��Ϊ10
    SYSTEMTIME st ;
	GetLocalTime( &st );
	//�ʼ�����ԭ��������ʱ���ټ�������ֵ,���������ų�ͬ����ԭ��,���������Ļ��������ų���Щ���
	sprintf( szStr,TEXT("%02d%02d%02d%02d"),st.wMonth,st.wDay,st.wHour,st.wSecond);
}

void TrimBlankStr( CHAR * lpStr )
{
	int iLen ;
      int i;
	  if ( lpStr == NULL )
		  return ;
	  iLen = strlen(lpStr);
	  for (i =iLen-1;i>=0;i--)
	  {
		  if (lpStr[i]==32 || lpStr[i]==9 )
			  continue;
		  else
			  break;
	  }
	  if ( i == -1)
	  {
		  lpStr[0] = 0;
	      return;
	  }
	  
	  lpStr[i+1] = 0;
	  iLen = strlen( lpStr );
	  for (i=0;i<iLen;i++ )
	  {
		  if ( lpStr[i]==32 || lpStr[i]==9 )
			  continue;
		  else
			  break;
	  }
	  if ( i!= 0 ){
		  memcpy( lpStr,lpStr+i,iLen - i );
	      lpStr[iLen-i] = 0;
	  }
}
