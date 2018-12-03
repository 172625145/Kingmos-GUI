/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

/**************************************************
文件说明：Email，字符转换函数 支持Base64,Quoted-print两种方式的编码，解码
版本号：1.0.0
开发时期：2003-3-11
作者：满益文
修改记录：通过多次修改后,可以对对二进制的数据进行正确的解码和加码,可以正确返回解密后的
          数据的大小,同时增加了对Queoted-printable 解码时的一些补充,对符合标准的数据进
		  行解码,并得到完整的数据    2003-06-10 By 满益文
**************************************************/
#include <ewindows.h>
#include <BaseQuoted.h>

/********************************  Base64 编码，解码 ***********************/
const CHAR Base64Table[]={ 
	      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',  
          'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  
          'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',  
          'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  
          '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'  
        };

const CHAR Base64Pad='=';

/*************************************************
声明：int  EncodeBase64(CHAR *szSource,CHAR* szDest, int iLen)
参数：
IN szSource  -愿字符串
IN szDest    -转换成功的结果字符串
  返回值：1  没有任何意义，如果失败的话，只能得到无效的结果，即该结果是没有任何意义
	      
功能描述：给字符串按Base64加密  
************************************************/
int  EncodeBase64(CHAR *szSource,CHAR* szDest,int iLen)
{     
       int iSourceLength = iLen;
	   int iPackTimes = iSourceLength / 3;
       int iSourcePos,iDestPos ;
	   int i;
	   BYTE ch1,ch2,ch3;
    
	   iSourcePos=0;
	   iDestPos=0;
 

	   while (iPackTimes-- >0)
	   {
		   ch1 = szSource[iSourcePos++];
           szDest[iDestPos++] = Base64Table[ ch1 >> 2];
		   ch2= szSource[iSourcePos++];
		   szDest[iDestPos++] = Base64Table[ (ch1 & 0x03 )<<4 | ch2>>4];
		   ch3 = szSource[iSourcePos++];
		   szDest[iDestPos++] = Base64Table[(ch2 & 0x0f)<<2 | ch3>>6];
		   szDest[iDestPos++] = Base64Table[ch3 & 0x3f];
		    
	   }
	   i = iSourceLength % 3;
	   if ( i==1 )
	   {
		   ch1 = szSource[iSourcePos++];
		   szDest[iDestPos++] = Base64Table[ch1>>2];
		   ch2 = szSource[iSourcePos++];
           szDest[iDestPos++] = Base64Table[(ch1 & 0x03) << 4]; 
           szDest[iDestPos++] = Base64Pad; 
           szDest[iDestPos++] = Base64Pad; 

	   }
	   else if ( i==2 )
	   { 
		   ch1 = szSource[iSourcePos++];
		   szDest[iDestPos++] = Base64Table[ch1>>2];
           ch2 = szSource[iSourcePos++];
           szDest[iDestPos++] = Base64Table[(ch1 & 0x03 ) << 4 | ch2>>4];
		   szDest[iDestPos++] = Base64Table[(ch2 & 0x0f)<<2];
		   szDest[iDestPos++]=Base64Pad;
	   }
	   szDest[iDestPos]='\0';
     return 1;
}

/*************************************************
声明：CHAR ConvertBaseChar(CHAR ch)
参数：
IN ch  -字符
返回值：ch  返回转换的字符
功能描述：按base64的标准转换字符
引用：解码时，被DecodeBase64调用   	      
************************************************/
CHAR ConvertBaseChar(CHAR ch)
{
	if (ch>=65 && ch<=90) return ch-=65;
	if (ch>=97 && ch<=122) return ch-=71;
	if (ch>=48 && ch<=57) return ch+=4;
	if (ch==61) return 0;
	if (ch==43) return 62;
	if (ch==47) return 63;

           	return 0;
}

/*************************************************
声明：int DecodeBase64 (CHAR *szSource,CHAR *szDest)
参数：
IN szSource  -愿字符串
IN szDest    -转换后的字符串
返回值：int  返回正确的数据，由于是数据流格式，所以要求能够正常返回数据
功能描述：按base64的标准转换字符，解码
************************************************/
int DecodeBase64 (CHAR *szSource,CHAR *szDest)
{
      int iSourceLength = strlen(szSource);
      int iDestPos,iSourcePos;
	  int iPackTimes;
	  int iRet = 0;

	  CHAR ch1,ch2,ch3,ch4;

	  iDestPos = 0;
	  iSourcePos = 0;
	  iPackTimes = iSourceLength/4;
      //iPackTimes--;//save 1 pack for end handling.
	  iRet = iPackTimes*3;//最后一个单独计算
	  while ( iPackTimes-- >0)
	  {

         ch1 = ConvertBaseChar(szSource[iSourcePos++]);
         ch2 = ConvertBaseChar(szSource[iSourcePos++]);
         ch3 = ConvertBaseChar(szSource[iSourcePos++]);
         ch4 = ConvertBaseChar(szSource[iSourcePos++]);
		
		 szDest[iDestPos++] = (ch1 <<2) | ch2>>4;
		 szDest[iDestPos++] = (ch2 <<4) | (ch3 & 0x3c )>>2;
		 szDest[iDestPos++] = ch3 <<6 | ch4;
          
	  }
	  iSourcePos -= 2;
	  if ( (ch1 = szSource[iSourcePos++])=='=' )
		  iRet-=2;
	  else	 if ( (ch1 = szSource[iSourcePos++])=='=' )
		  iRet-=1;
   //parse the end of pack
   //I think no need to check the end.
	  return iRet;
}

/********************************  Quoted-printable 编码，解码 ***********************/
const CHAR quotedTable[]={
                          '0','1','2','3',
						  '4','5','6','7',
						  '8','9','A','B',
						  'C','D','E','F',
						  '?',0,
};

/*************************************************
声明：CHAR *_Mywstrchr(LPCSTR lpSource,CHAR ch)
参数：
IN lpSource  -愿字符串
IN ch        - 需寻找的字符
返回值：找到字符ch所在的位置，NULL 没有对应的字符
功能描述：从一字符串寻找ch存在 的第一位置，没有返回NULL
************************************************/
CHAR *_Mywstrchr(LPCSTR lpSource,CHAR ch)
{
	unsigned int i;
	CHAR ch1;
     if (lpSource == NULL) 
		 return NULL;
	 for ( i = 0; i< strlen(lpSource);i++)
	 {
          if ((ch1 = lpSource[i]) ==  ch)
			  return (CHAR *)(lpSource + i);
	 }
     return NULL;
}

/*************************************************
声明：void EncodeQuoted(CHAR *szSource,CHAR *szDest)
参数：
IN szSource  -愿字符串
IN szDest    -转换后的字符串
返回值：void 
功能描述：用Quoted-printable方式给所给字符串加密
************************************************/
void EncodeQuoted(CHAR *szSource,CHAR *szDest)
{
     int iLength = strlen(szSource);
     CHAR szText[6];
     BYTE ch;
	 BYTE ch1,ch2;
	 int iCur=0,i=0;
	 while ( iCur < iLength)
	 {  
		 ch = szSource[iCur];
		 if (ch != 0x0d && ch != 0x0a )
		 {
			 ch1= quotedTable[ch >>4];
			 ch2= quotedTable[ch & 0x0f];
			 sprintf(szText,TEXT("=%c%c"),ch1,ch2);
		 }
		 else
			 sprintf(szText,TEXT("%c"),ch);
		 strcat(szDest,szText);
		 iCur++;
 	 }
}

/*************************************************
声明：CHAR UPPERCASE(CHAR ch)
参数：
IN ch  -愿字符
返回值：ch  的大写字符 
功能描述：得到ch 的大写形式
************************************************/
CHAR UPPERCASE(CHAR ch)
{
     if (ch>96 && ch<123)
		 ch-=23;
	 return ch;
}

/*************************************************
声明：void DecodeQuoted(CHAR *szSource,CHAR *szDest)
参数：
IN szSource  -愿字符串
IN szDest    -转换后的字符串
返回值：void
功能描述：用Quoted-printable的方式将字符串解码
************************************************/
void DecodeQuoted(CHAR *szSource,CHAR *szDest)
{
    int iCur,i;
    int iLength = strlen(szSource);
	CHAR *pos1,*pos2;
    BYTE ch,ch1,ch2;
	iCur=0;
	i=0;
	while (iCur < iLength)
	{
		if ((ch = szSource[iCur++])=='=')
		{  
			if ( (ch1 = szSource[iCur++]) == 13	|| ch == 10 )	
			{
				if ( ((ch2 = szSource[iCur++])== 13 || ch2 == 10) && ch1!=ch2)
				{
				}
				else
					iCur--;
				continue;
			} 
			iCur--;
			pos1 = _Mywstrchr(quotedTable,UPPERCASE(szSource[iCur++]));
			ch1 = pos1-quotedTable;
			pos2 = _Mywstrchr(quotedTable,UPPERCASE(szSource[iCur++]));
			ch2  = pos2-quotedTable;
			if ( pos1 && pos2 )
			{
				szDest[i++]=(ch1 <<4  | (ch2 & 0x0f));
			}
			else
				szDest[i++] = quotedTable[16];
			
		}
		else
			szDest[i++] = ch;
				
	}
}
