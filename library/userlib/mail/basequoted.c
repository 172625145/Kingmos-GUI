/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

/**************************************************
�ļ�˵����Email���ַ�ת������ ֧��Base64,Quoted-print���ַ�ʽ�ı��룬����
�汾�ţ�1.0.0
����ʱ�ڣ�2003-3-11
���ߣ�������
�޸ļ�¼��ͨ������޸ĺ�,���ԶԶԶ����Ƶ����ݽ�����ȷ�Ľ���ͼ���,������ȷ���ؽ��ܺ��
          ���ݵĴ�С,ͬʱ�����˶�Queoted-printable ����ʱ��һЩ����,�Է��ϱ�׼�����ݽ�
		  �н���,���õ�����������    2003-06-10 By ������
**************************************************/
#include <ewindows.h>
#include <BaseQuoted.h>

/********************************  Base64 ���룬���� ***********************/
const CHAR Base64Table[]={ 
	      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',  
          'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  
          'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',  
          'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  
          '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'  
        };

const CHAR Base64Pad='=';

/*************************************************
������int  EncodeBase64(CHAR *szSource,CHAR* szDest, int iLen)
������
IN szSource  -Ը�ַ���
IN szDest    -ת���ɹ��Ľ���ַ���
  ����ֵ��1  û���κ����壬���ʧ�ܵĻ���ֻ�ܵõ���Ч�Ľ�������ý����û���κ�����
	      
�������������ַ�����Base64����  
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
������CHAR ConvertBaseChar(CHAR ch)
������
IN ch  -�ַ�
����ֵ��ch  ����ת�����ַ�
������������base64�ı�׼ת���ַ�
���ã�����ʱ����DecodeBase64����   	      
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
������int DecodeBase64 (CHAR *szSource,CHAR *szDest)
������
IN szSource  -Ը�ַ���
IN szDest    -ת������ַ���
����ֵ��int  ������ȷ�����ݣ���������������ʽ������Ҫ���ܹ�������������
������������base64�ı�׼ת���ַ�������
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
	  iRet = iPackTimes*3;//���һ����������
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

/********************************  Quoted-printable ���룬���� ***********************/
const CHAR quotedTable[]={
                          '0','1','2','3',
						  '4','5','6','7',
						  '8','9','A','B',
						  'C','D','E','F',
						  '?',0,
};

/*************************************************
������CHAR *_Mywstrchr(LPCSTR lpSource,CHAR ch)
������
IN lpSource  -Ը�ַ���
IN ch        - ��Ѱ�ҵ��ַ�
����ֵ���ҵ��ַ�ch���ڵ�λ�ã�NULL û�ж�Ӧ���ַ�
������������һ�ַ���Ѱ��ch���� �ĵ�һλ�ã�û�з���NULL
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
������void EncodeQuoted(CHAR *szSource,CHAR *szDest)
������
IN szSource  -Ը�ַ���
IN szDest    -ת������ַ���
����ֵ��void 
������������Quoted-printable��ʽ�������ַ�������
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
������CHAR UPPERCASE(CHAR ch)
������
IN ch  -Ը�ַ�
����ֵ��ch  �Ĵ�д�ַ� 
�����������õ�ch �Ĵ�д��ʽ
************************************************/
CHAR UPPERCASE(CHAR ch)
{
     if (ch>96 && ch<123)
		 ch-=23;
	 return ch;
}

/*************************************************
������void DecodeQuoted(CHAR *szSource,CHAR *szDest)
������
IN szSource  -Ը�ַ���
IN szDest    -ת������ַ���
����ֵ��void
������������Quoted-printable�ķ�ʽ���ַ�������
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
