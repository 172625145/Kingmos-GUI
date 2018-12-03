/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

/**************************************************
文件说明：文件字符串操作函数.
版本号：1.0.0
开发时期：2003-3-12
作者：满益文
修改记录：
**************************************************/
#include <ewindows.h>
#include "emailformat.h"

/*************************************************
声明：void TrimLines(LPTSTR szLine,int * pPos)
参数：
IN      szLine           -字符串
IN/OUT  pPos           -得到该该字符串的行字符数

  返回：void
功能描述：从一字符串中得到一有效的行，根据email 有关协议，所有的邮件的行末尾都是CRLF
调用：    当需要一行完整的行时调用，可能的问题是如果该行的字符没有行结束符，
          注意有不同的地方：  TrimLines  和  TrimLines2 的区别，
		  前者是，得到 除CRLF外的有效行字符串
		  后者是，得到以CRLF为标志的完整一行，没有CRLF时也是一行
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
声明：void TrimLines2(CHAR *szLine,int *piPos)
参数：
IN      szLine           -字符串
IN/OUT  piPos           - 得到该该字符串的行字符数,或	正确的QUOTED编码方式的字符串。
                          要求是不含多余的字符
  返回：void
功能描述：从一字符串中得到一有效的行，根据email 有关协议，所有的邮件的行末尾都是CRLF，
          同时，由于该函数主要针对解析Quoted码的数据,需要对数据的行进行截断，如果不是一个完整的行
		  但是要求不要把后面的Quoted数据段破坏
调用：    当需要一行完整的行时调用，可能的问题是如果该行的字符没有行结束符，时
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
	{//说明该数据不是一行真正的行，需要重新鉴别,根据Quoted-printable的数据格式
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
声明：BOOL ReadVirtualLine(HANDLE hFile,CHAR *Aline,int CurPos)
参数：
IN      hFile           -文件句柄
IN/OUT  Aline           -读取字符串的BUFFER 
IN  CurPos              -读取前 ，邮件文件当前的位置
返回：BOOL  TRUE 成功读取
            FALSE 读取失败
功能描述：读取一定数量的字符 ，该函数与ReadVirtualLine2的不同之处在于，它只读取一定数量的字符
          不截取行，由上层函数处理，而ReadVirtualLine2则读取一完整的行
调用：   
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
声明：BOOL ReadVirtualLine2(HANDLE hFile,LPTSTR ALine,int * pnLineLength,int * pFilePos)
参数：
IN      hFile           -文件句柄
IN/OUT  pnLineLength    -该行的数据的位置
IN  CurPos  -得到当前邮件读取的位置
  返回：void
功能描述：在多行的解析中，注意如果是Printable 格式，可能在一行中存在多于100个字节的数据。
          要改进，一般情况下，BASE64格式不会存在多于100个字节的行。
调用：  按邮件协议操作时可能用到此函数
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
声明：BOOL FindStringBeforeCh(LPTSTR lpString,CHAR key,LPTSTR szKey)
参数：
IN      lpString        -愿字符串
IN      key             -关键字
OUT     szKey           -得到需要的字符串
返回：void
功能描述： 发现关键字之前的字符串
调用：   在邮件解析过程中 调用
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
声明：void TrimString(LPTSTR szLine)
参数：
IN/OUT      szLine        -源字符串
返回：void
功能描述：将字符串左边的字符全部Skip 
调用：   在邮件解析过程中 调用
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
声明：int StrPos(CHAR * szLine,CHAR ch)
参数：
IN     szLine           -愿字符串
IN      ch             -关键字
返回：int     返回该字符在串中的第一个位置 0 表示不存在
功能描述： 发现关键字在串中的位置
调用：   
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
声明：void GetFileNameEx( LPTSTR lpPath, LPTSTR lpName,int *piExt )
参数：
IN     lpPath          -文件完整路径
OUT    lpName          -返回的文件名
OUT    *piExt          -扩展名标示号
返回：void
功能描述： 寻找文件完整路径中的文件名
调用：   
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
声明：void GetTimeStr(CHAR *szStr)
参数：
IN  szStr - 得到与时间有关的字符串,注意长度要有至少10Bytes
返回：void
功能描述：　得到与时间有关的字符串，主要为保存邮件而生
调用：   
************************************************/
void GetTimeStr(CHAR *szStr )
{//保证数据有足够长为10
    SYSTEMTIME st ;
	GetLocalTime( &st );
	//邮件命名原则是月日时分再加上索引值,基本可以排除同名的原则,如过加上年的话，可以排除这些情况
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
