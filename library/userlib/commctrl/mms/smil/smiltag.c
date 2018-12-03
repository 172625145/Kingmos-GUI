/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����SMIL�ֽ�TAG�ļ�
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-04-30
���ߣ��½��� Jami
�޸ļ�¼��
**************************************************/
#include "ewindows.h"
#include "smiltag.h"
//#include "debugwnd.h"

// **************************************************
// ��������
// **************************************************

#define MAXTAGNAMELEN	32
#define MAXATTRNAMELEN	32

// ����SMIL TAG���б�ṹ
typedef struct
{
	char *tagName;
	TAGID tagID;
}SMIL_TAGLIST;

// �������о�����Ч��TAG���Ƽ���ǩID
static SMIL_TAGLIST SMIL_tagList[]={  // ��ǩ�б�
	{"SMIL",		SMILTAG_SMIL},
	{"HEAD",		SMILTAG_HEAD},
	{"BODY",		SMILTAG_BODY},
	{"META",		SMILTAG_META},
	{"LAYOUT",		SMILTAG_LAYOUT},
	{"ROOT-LAYOUT",	SMILTAG_ROOTLAYOUT},
	{"REGION",		SMILTAG_REGION},
	{"PAR",			SMILTAG_PAR},
	{"IMG",			SMILTAG_IMG},
	{"TEXT",		SMILTAG_TEXT},
	{"AUDIO",		SMILTAG_AUDIO},
	{"REF",			SMILTAG_REF},
	{NULL,			SMILTAG_NULL},
};


typedef struct
{
	char *attrName;
	ATTRID attrID;
}SMIL_ATTRLIST;

static SMIL_ATTRLIST SMIL_AttrList[]={ // ���������б�
	{"id",		SMILATTR_ID},
	{"left",	SMILATTR_LEFT},
	{"top",		SMILATTR_TOP},
	{"width",	SMILATTR_WIDTH},
	{"height",	SMILATTR_HEIGHT},
	{"fit",		SMILATTR_FIT},
	{"fill",	SMILATTR_FILL},
	{"hidden",	SMILATTR_HIDDEN},
	{"meet",	SMILATTR_MEET},
	{"slice",	SMILATTR_SLICE},
	{"src",		SMILATTR_SRC},
	{"region",	SMILATTR_REGION},
	{"alt",		SMILATTR_ALT},
	{"begin",	SMILATTR_BEGIN},
	{"end",		SMILATTR_END},
	{"dur",		SMILATTR_DUR},
	{NULL,		SMILATTR_NULL},
};


// **************************************************
// ����������
// **************************************************

static BOOL SMIL_GetTag(char **pStream,char *pTagName);
static TAGID SMIL_GetTagID(char *pTagName);

static BOOL SMIL_GetAttribute(char **pStream,char *pAttributeName);
static BOOL SMIL_GetAttrContent(char **pStream,char *pAttributeContent,DWORD dwSize);
static ATTRID SMIL_GetAttrID(char *pAttributeName);
static BOOL SMIL_GetAttributeContent(char **pStream,char *pAttrContent,DWORD dwSize);
//
//static char SMIL_GetRemberCode(char **pStream);
//int SMIL_GetRemberCodeLen(char *pStream);
//#define DEBUG_OUT



// **************************************************
// ������void SMIL_Filter(char **pStream)
// ������
// 	IN/OUT pStream -- ָ��������
// 
// ����ֵ����
// �������������˵��������е���Ч���롣
// ����: 
// **************************************************
void SMIL_Filter(char **pStream)
{  // �˵�����Ҫ���ַ�
		while(**pStream==' '||**pStream=='\r'||**pStream=='\n'||**pStream=='\t')
			*pStream+=1;
}

// **************************************************
// ������TAGID SMIL_LocateTag(char **pStream)
// ������
// 	IN/OUT pStream -- ָ��������,�������µ�ָ��λ��
// 
// ����ֵ�����ص�ǰ�ı�ǩ���
// ������������λ��ǰ�ı�ǩ��
// ����: 
// **************************************************
TAGID SMIL_LocateTag(char **pStream)
{
	char pTagName[MAXTAGNAMELEN];
	TAGID tagID;
	SMIL_Filter(pStream);  // �˵���Ч�ַ�
	while(1)
	{
		if (**pStream==0)  // ����������
			return SMILTAG_NULL;
		if (**pStream=='<')
		{  // ��ǩ��ʼ���
			if (*(*pStream+1)=='!'&&*(*pStream+2)=='-'&&*(*pStream+3)=='-')
			{  // ע�Ϳ�ʼ <!--
				while(1)
				{
					if (**pStream==0)  // û���ַ���
					{
						RETAILMSG(1,(TEXT("!!!ERROR,NO > TAG")));
						return SMILTAG_NULL;
					}
					if (**pStream=='-'&&*(*pStream+1)=='-'&&*(*pStream+2)=='>')
					{  // ע�ͽ���
						*pStream+=3;  // '-->'
						break;
					}
					*pStream+=1;
				}
				continue; // ע���Ѿ����꣬���¿�ʼ������һ����ǩ
			}
			if ((*(*pStream+1)>='a'&&*(*pStream+1)<='z')||(*(*pStream+1)>='A'&&*(*pStream+1)<='Z'))
			{ // Ӣ���ַ�����ǩ��ʼ
				break;
			}
			if ((*(*pStream+1)=='/')||(*(*pStream+1)=='?')||(*(*pStream+1)=='!'))
			{ // �����ַ�
				break;
			}
			RETAILMSG(1,(TEXT("!!!ERROR,UNKNOW TAG\r\n")));
			return SMILTAG_NULL;
		}
		*pStream+=1; // ��ǰ�ַ����Ǳ�ǩ��ʼ����Ϊ��Ч�ַ�����ȥ
	}
	SMIL_GetTag(pStream,pTagName); // �õ���ǩ����
	RETAILMSG(1,(TEXT("The Current TAG is <%s>"),pTagName));
	tagID=SMIL_GetTagID(pTagName); // �õ���ǩ���
	return tagID;  // ���ر�ǩ���
}

// **************************************************
// ������void SMIL_ToNextTag(char **pStream)
// ������
// 	IN/OUT pStream -- ָ��������
// 
// ����ֵ����
// ��������������ǰָ��ָ����һ����ǩ��
// ����: 
// **************************************************
void SMIL_ToNextTag(char **pStream)
{
	while(1)
	{
		if (**pStream==0)
			return ;
		if (**pStream=='>')
		{  // current tag is end ,then next position is new tag
			*pStream+=1;  // jump '>'
			SMIL_Filter(pStream);
			break;
		}
		if (**pStream=='<')
		{ // already is a new tag
			if ((*(*pStream+1)>='a'&&*(*pStream+1)<='z')||(*(*pStream+1)>='A'&&*(*pStream+1)<='Z'))
			{
				break;
			}
			if ((*(*pStream+1)=='/')||(*(*pStream+1)=='?')||(*(*pStream+1)=='!'))
			{
				break;
			}
//			break;
		}
		*pStream+=1;
	}
}

// **************************************************
// ������static BOOL SMIL_GetTag(char **pStream,char *pTagName)
// ������
// 	IN/OUT pStream -- ָ��������
// 	OUT pTagName -- ��ű�ǩ���ƵĻ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǩ�����ơ�
// ����: 
// **************************************************
static BOOL SMIL_GetTag(char **pStream,char *pTagName)
{
	WORD wTagNameLen=0;

	*pStream+=1;  // '<'
	while(1)
	{
		if (**pStream==' '||**pStream=='>')
			break;  // ��ǩ����
		*pTagName++=**pStream;
		*pStream+=1;
		wTagNameLen++;
		if (wTagNameLen>=MAXTAGNAMELEN-1)
		{ //��ǩ����̫��
//			MessageBox(NULL,"illegal Tag Name","Error",MB_OK);
			*pTagName=0;
			return FALSE;
		}
	}
	*pTagName=0; // �ɹ�����ӽ�β��
	return TRUE;
}

// **************************************************
// ������static TAGID SMIL_GetTagID(char *pTagName)
// ������
// 	IN pTagName -- ��ǩ����
// 
// ����ֵ�����ر�ǩ�ı��
// �����������õ���ǩ�ı�š�
// ����: 
// **************************************************
static TAGID SMIL_GetTagID(char *pTagName)
{
	int i;
	TAGID tagID_End=0;
	if (*pTagName=='/')
	{
		pTagName++;
		tagID_End=SMILTAG_END;
	}

	for (i=0;;i++)
	{
		if (SMIL_tagList[i].tagName == NULL) // �Ѿ�û�б�ǩ��
			break; 
		if (stricmp(SMIL_tagList[i].tagName,pTagName)==0) // �Ƚϵ�ǰ��ǩ���ǩ�б��еı�ǩ�Ƚ�
			return (SMIL_tagList[i].tagID+tagID_End); // �ҵ���ǰ��ǩ�����ر�ǩ�ı��
	}
	return SMILTAG_UNKNOW;
}

// **************************************************
// ������ATTRID SMIL_LocateAttribute(char **pStream)
// ������
// 	IN/OUT pStream -- ָ�����������������µ�λ��
// 
// ����ֵ�����ص�ǰλ�õ����Ա��
// ������������λ��ǰ�����Ա�š�
// ����: 
// **************************************************
ATTRID SMIL_LocateAttribute(char **pStream)
{
	char pAttributeName[MAXATTRNAMELEN];
	ATTRID attrID;

		SMIL_Filter(pStream); // �˵���Ч����

		if (**pStream=='>'||**pStream==0)
			return 0;  // ��ǩ����
		SMIL_GetAttribute(pStream,pAttributeName);  // �õ���ǩ��������
		attrID=SMIL_GetAttrID(pAttributeName);  // �õ���ǩ���Ա��
		return attrID; // ���ر�ǩ���Ա��
}

// **************************************************
// ������static BOOL SMIL_GetAttribute(char **pStream,char *pAttributeName)
// ������
// 	IN/OUT pStream -- ָ��������
// 	OUT pAttributeName -- ��ű�ǩ������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ��������ơ�
// ����: 
// **************************************************
static BOOL SMIL_GetAttribute(char **pStream,char *pAttributeName)
{
	WORD wAttrNameLen=0;

	while(1)
	{
		if (**pStream==' '||**pStream=='='||**pStream=='>')
			break; // �����ָ������˳���
		*pAttributeName++=**pStream;
		*pStream+=1;
		wAttrNameLen++;
		if (wAttrNameLen>=MAXATTRNAMELEN-1)
		{ // ����̫���˳�
			while(1)
			{
				if (**pStream=='>') // ����ָ���ƶ�����ǩ����
					break;
				*pStream+=1;
			}
			RETAILMSG(1,(TEXT("!!! ERROR Attribute <%s>\r\n"),pAttributeName));
			*pAttributeName=0;
			return FALSE;
		}
	}
	*pAttributeName=0; // ��ɣ���ӽ�β��
	return TRUE;
}

// **************************************************
// ������BOOL SMIL_LocateAttributeContent(char **pStream,char *pAttrContent,DWORD dwSize)
// ������
// 	IN/OUT pStream -- ָ��������
// 	OUT pAttrContent -- ����������ݵĻ���
// 	IN dwSize -- �����С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������λ��ǰ��ǩ���Ե����ݡ�
// ����: 
// **************************************************
BOOL SMIL_LocateAttributeContent(char **pStream,char *pAttrContent,DWORD dwSize)
{
	SMIL_GetAttributeContent(pStream,pAttrContent,dwSize); // �õ���������
	return TRUE;
}

// **************************************************
// ������static BOOL SMIL_GetAttrContentContent(char **pStream,char *pAttrContent,DWORD dwSize)
// ������
// 	IN/OUT pStream -- ָ��������
// 	OUT pAttrContent -- ����������ݵĻ���
// 	IN dwSize -- �����С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ����Ե����ݡ�
// ����: 
// **************************************************
static BOOL SMIL_GetAttributeContent(char **pStream,char *pAttrContent,DWORD dwSize)
{
	WORD wAttributeContentLen=0;
	BOOL bIsQuotationMark=FALSE;

	while(1)
	{
		if (**pStream=='=')
		{
			*pStream+=1;  // jump '='
			break;
		}
		if (**pStream==' ')
		{
			*pStream+=1;  // jump ' '
			continue;
		}
		if (**pStream=='>')
		{  // ��ǩ����
			*pAttrContent=0;
			return TRUE;
		}
		*pStream+=1;
	}
	while(1)
	{
		if (**pStream==' ')
		{  // �����ո�
			*pStream+=1;  // jump ' '
			continue;
		}
		break;
	}
	if (**pStream=='\"')
	{ // ������
		bIsQuotationMark=TRUE;
		*pStream+=1;
	}
	while(1)
	{
		if (**pStream=='>')
			break;  // ��ǩ����
		if (bIsQuotationMark==FALSE&&**pStream==' ')
			break; // �������ţ��ո����
		if (bIsQuotationMark==TRUE&&**pStream=='\"')
		{ // ���Ž���
			*pStream+=1;
			break;
		}
		*pAttrContent++=**pStream;
		*pStream+=1;
		wAttributeContentLen++;
		if (wAttributeContentLen>=dwSize-1)
		{ // ���Դ�С���������С��ʧ��
			RETAILMSG(1,(TEXT("Property Content Buffer Overflow")));
			*pAttrContent=0;
			return FALSE;
		}
	}
	*pAttrContent=0;  // �ɹ�����ӽ�β��
	return TRUE;
}
// **************************************************
// ������static PROPERTYID SMIL_GetAttrID(char *pAttributeName)
// ������
// 	IN pAttributeName -- ���Ե�����
// 
// ����ֵ���������Եı��
// �����������õ����Ա�š�
// ����: 
// **************************************************
static ATTRID SMIL_GetAttrID(char *pAttributeName)
{
	int i;
	for (i=0;;i++)
	{ //�������б��в��Ҷ�Ӧ�ı��
		if (SMIL_AttrList[i].attrName == NULL)
		{
			// �Ѿ�û��������Ŀ
			break;
		}
		if (stricmp(SMIL_AttrList[i].attrName,pAttributeName)==0)
			return (SMIL_AttrList[i].attrID);
	}
	return SMILATTR_UNKNOW; // û���ҵ���Ӧ�ı��
}

/*

// **************************************************
// ������BOOL SMIL_LocateTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
// ������
// 	IN/OUT pStream -- ָ��������
//	 OUT pTagContent -- ����Ҫ��ű�ǩ���ݵĻ���
//	 IN dwSize -- �����С
//	 IN bPreProcess -- �Ƿ���ҪԤ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ������������λ��ǩ���ݡ�
// ����: 
// **************************************************
BOOL SMIL_LocateTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
{
	SMIL_GetTagContent(pStream,pTagContent,dwSize,bPreProcess);  // �õ���ǩ����
#ifdef DEBUG_OUT
	if (pTagContent[0]!=0)
		Output(" Content = ",pTagContent,TRUE);
#endif
//	JAMIMSG(DBG_FUNCTION,(TEXT("LocateTagContent=\"%s\"\r\n"),pTagContent));
	return TRUE;
}

// **************************************************
// ������ATTRID SMIL_LocateProperty(char **pStream)
// ������
// 	IN/OUT pStream -- ָ��������
// 
// ����ֵ�����ض�λ�����Ա��
// ������������λ��ǰ�����Ա�š�
// ����: 
// **************************************************
ATTRID SMIL_LocateProperty(char **pStream)
{
	char pPropertyName[MAXPROPERTYNAMELEN];
	ATTRID propertyID;

//		while(**pStream==' '||**pStream=='\r'||**pStream=='\n'||**pStream=='\t')
//			*pStream+=1;
		SMIL_Filter(pStream); // �˵���Ч����

		if (**pStream=='>'||**pStream==0)
			return 0;  // ��ǩ����
		SMIL_GetProperty(pStream,pPropertyName);  // �õ���ǩ��������
#ifdef DEBUG_OUT
		Output("	",pPropertyName,FALSE);
#endif
		propertyID=SMIL_GetPropertyID(pPropertyName);  // �õ���ǩ���Ա��
#ifdef DEBUG_OUT
		{
			char Buffer[16];
			sprintf(Buffer,"%04d )",propertyID);
			Output(" (",Buffer,FALSE);
		}
#endif
		return propertyID; // ���ر�ǩ���Ա��
}

// **************************************************
// ������BOOL SMIL_LocatePropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
// ������
// 	IN/OUT pStream -- ָ��������
// 	OUT pPropertyContent -- ����������ݵĻ���
// 	IN dwSize -- �����С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������λ��ǰ��ǩ���Ե����ݡ�
// ����: 
// **************************************************
BOOL SMIL_LocatePropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
{
	SMIL_GetPropertyContent(pStream,pPropertyContent,dwSize); // �õ���������
#ifdef DEBUG_OUT
	Output(" ::: ",pPropertyContent,TRUE);
#endif
	return TRUE;
}

// **************************************************
// ������static BOOL SMIL_GetTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
// ������
// 	IN/OUT pStream -- ָ��������
//	 OUT pTagContent -- ����Ҫ��ű�ǩ���ݵĻ���
//	 IN dwSize -- �����С
//	 IN bPreProcess -- �Ƿ���ҪԤ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// �����������õ���ǩ���ݡ�
// ����: 
// **************************************************
static BOOL SMIL_GetTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
{
  DWORD dwTagContentLen=0;
	JAMIMSG(DBG_FUNCTION,(TEXT("Enter GetTagContent\r\n")));
	JAMIMSG(DBG_FUNCTION,(TEXT("pTagContent=%08X\r\n"),pTagContent));
	JAMIMSG(DBG_FUNCTION,(TEXT("dwSize=%d\r\n"),dwSize));
	while(1)
	{
		if (**pStream==0)
			break;
		if (**pStream=='<')
		{  // ��ǩ��ʼ
			if ((*(*pStream+1)>='a'&&*(*pStream+1)<='z')||(*(*pStream+1)>='A'&&*(*pStream+1)<='Z'))
			{ // Ӣ���ַ�
				break;
			}
			if ((*(*pStream+1)=='/')||(*(*pStream+1)=='?')||(*(*pStream+1)=='!'))
			{ // �������
				break;
			}
		}
		if (bPreProcess==FALSE && (**pStream == '\r'||**pStream == '\n'))
		{  // normal , we must ignore the CR
			*pStream+=1;
			while(**pStream==' '||**pStream=='\t')  // jump the title space
				*pStream+=1;
			continue;
		}
		if (**pStream == '&')
		{ // �����ַ�
			*pTagContent++=SMIL_GetRemberCode(pStream); // �õ������ַ�
		}
		else 
		{ // һ���ַ�
			*pTagContent++=**pStream;
			*pStream+=1;
		}
		dwTagContentLen++;
		if (dwTagContentLen>=dwSize-1)
		{ // ���ȳ��������С
		//	MessageBox(NULL,"Tag Content Buffer Overflow","Error",MB_OK);
			*pTagContent=0;
			return FALSE;
		}
	}
	*pTagContent=0; // �ɹ�����ӽ�β��
	JAMIMSG(DBG_FUNCTION,(TEXT("Exit GetTagContent\r\n")));
	return TRUE;
}
#define MAXPARAGRAPH  1024
// **************************************************
// ������DWORD SMIL_GetTagContentLen(char *pStream,BOOL bPreProcess)
// ������
// 	IN/OUT pStream -- ָ��������
//	 IN bPreProcess -- �Ƿ���ҪԤ����
// 
// ����ֵ�����ر�ǩ���ݵĳ���
// �����������õ���ǩ���ݵĳ��ȡ�
// ����: 
// **************************************************
DWORD SMIL_GetTagContentLen(char *pStream,BOOL bPreProcess)
{
  DWORD dwTagContentLen=0;
  DWORD bCut=FALSE;
	while(1)
	{
		if (*pStream==0)
			break;
		if (*pStream=='<')
		{ // ��ǩ��ʼ����
			if ((*(pStream+1)>='a'&&*(pStream+1)<='z')||(*(pStream+1)>='A'&&*(pStream+1)<='Z'))
			{ // Ӣ���ַ�
				break;
			}
			if ((*(pStream+1)=='/')||(*(pStream+1)=='?')||(*(pStream+1)=='!'))
			{ // ָ�����������
				break;
			}
		}
		if (bPreProcess==FALSE && (*pStream == '\r'||*pStream == '\n'))
		{  // normal , we must ignore the CR
			pStream+=1;
			while(*pStream==' '||*pStream=='\t')  // jump the title space
				pStream+=1;
			continue;
		}
		if ((dwTagContentLen>=MAXPARAGRAPH) && (*pStream == '\r'||*pStream == '\n'))
		{  // normal , we must ignore the CR
			bCut=TRUE;
			pStream+=1;
			continue;
		}
		if (bCut==TRUE)
			break;
		if (*pStream == '&')
		{ // �Ǽ�����
			SMIL_GetRemberCode(&pStream);
		}
		else 
		{ // ��һ���ַ�
			pStream+=1;
		}
		dwTagContentLen++;
	}
	return dwTagContentLen+1;  // will save the text and end code (\0)
}

// **************************************************
// ������static BOOL SMIL_GetAttribute(char **pStream,char *pPropertyName)
// ������
// 	IN/OUT pStream -- ָ��������
// 	OUT pPropertyName -- ��ű�ǩ������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ��������ơ�
// ����: 
// **************************************************
static BOOL SMIL_GetProperty(char **pStream,char *pPropertyName)
{
	WORD wPropertyNameLen=0;

	while(1)
	{
		if (**pStream==' '||**pStream=='='||**pStream=='>')
			break; // �����ָ������˳���
		*pPropertyName++=**pStream;
		*pStream+=1;
		wPropertyNameLen++;
		if (wPropertyNameLen>=MAXPROPERTYNAMELEN-1)
		{ // ����̫���˳�
			while(1)
			{
				if (**pStream=='>') // ����ָ���ƶ�����ǩ����
					break;
				*pStream+=1;
			}
			//MessageBox(NULL,"illegal Property Name","Error",MB_OK);
			*pPropertyName=0;
			return FALSE;
		}
	}
	*pPropertyName=0; // ��ɣ���ӽ�β��
	return TRUE;
}
// **************************************************
// ������static BOOL SMIL_GetPropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
// ������
// 	IN/OUT pStream -- ָ��������
// 	OUT pPropertyContent -- ����������ݵĻ���
// 	IN dwSize -- �����С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ����Ե����ݡ�
// ����: 
// **************************************************
static BOOL SMIL_GetPropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
{
	WORD wPropertyContentLen=0;
	BOOL bIsQuotationMark=FALSE;

	while(1)
	{
		if (**pStream=='=')
		{
			*pStream+=1;  // jump '='
			break;
		}
		if (**pStream==' ')
		{
//			*pPropertyContent=0;
//			return TRUE;
			*pStream+=1;  // jump ' '
			continue;
		}
		if (**pStream=='>')
		{  // ��ǩ����
			*pPropertyContent=0;
			return TRUE;
		}
		*pStream+=1;
	}
	while(1)
	{
		if (**pStream==' ')
		{  // �����ո�
			*pStream+=1;  // jump ' '
			continue;
		}
		break;
	}
	if (**pStream=='\"')
	{ // ������
		bIsQuotationMark=TRUE;
//		*pPropertyContent++=**pStream;
		*pStream+=1;
//		wPropertyContentLen++;
//		if (wPropertyContentLen>=dwSize)
//		{
//			MessageBox(NULL,"Property Content Buffer Overflow","Error",MB_OK);
//			*pPropertyContent=0;
//			return FALSE;
//		}
	}
	while(1)
	{
		if (**pStream=='>')
			break;  // ��ǩ����
		if (bIsQuotationMark==FALSE&&**pStream==' ')
			break; // �������ţ��ո����
		if (bIsQuotationMark==TRUE&&**pStream=='\"')
		{ // ���Ž���
//			*pPropertyContent++=**pStream;
			*pStream+=1;
//			wPropertyContentLen++;
//			if (wPropertyContentLen>=dwSize)
//			{
//				MessageBox(NULL,"Property Content Buffer Overflow","Error",MB_OK);
//				*pPropertyContent=0;
//				return FALSE;
//			}
			break;
		}
		*pPropertyContent++=**pStream;
		*pStream+=1;
		wPropertyContentLen++;
		if (wPropertyContentLen>=dwSize-1)
		{ // ���Դ�С���������С��ʧ��
//			MessageBox(NULL,"Property Content Buffer Overflow","Error",MB_OK);
			*pPropertyContent=0;
			return FALSE;
		}
	}
	*pPropertyContent=0;  // �ɹ�����ӽ�β��
	return TRUE;
}
// **************************************************
// ������static ATTRID SMIL_GetPropertyID(char *pPropertyName)
// ������
// 	IN pPropertyName -- ���Ե�����
// 
// ����ֵ���������Եı��
// �����������õ����Ա�š�
// ����: 
// **************************************************
static PROPERTYID SMIL_GetPropertyID(char *pPropertyName)
{
	int i;
	for (i=0;i<118;i++)
	{ //�������б��в��Ҷ�Ӧ�ı��
		if (stricmp(SMIL_propertyList[i].propertyName,pPropertyName)==0)
			return (SMIL_propertyList[i].propertyID);
	}
	return PROPERTY_UNKNOW; // û���ҵ���Ӧ�ı��
}

// **************************************************
// ������WORD SMIL_GetAlignType(char *pAlign)
// ������
// 	IN pAlign -- ���������
// 
// ����ֵ�����ض��뷽ʽ
// �����������õ����뷽ʽ��
// ����: 
// **************************************************
WORD SMIL_GetAlignType(char *pAlign)
{
#define ALIGNTYPENUM 7
	struct {
		char *AlignText;
		WORD Align;
	}AlignTable[]={  // �������� -- ���뷽ʽ��� �б�
		{"left", ALIGN_LEFT},
		{"right",ALIGN_RIGHT},
		{"center",ALIGN_CENTER},
		{"top",ALIGN_TOP},
		{"middle",ALIGN_MIDDLE},
		{"bottom",ALIGN_BOTTOM},
		{"justify",ALIGN_JUSTIFY},
	};
	int i;
	for (i=0;i<ALIGNTYPENUM;i++)
	{ // ���б��в��Ҷ�Ӧ�Ķ��뷽ʽ
		if (stricmp(pAlign,AlignTable[i].AlignText)==0)
			return AlignTable[i].Align;
	}
	return ALIGN_NONE; // ����ʶ�Ķ��뷽ʽ
}


// **************************************************
// ������static char SMIL_GetRemberCode(char **pStream)
// ������
// 	IN/OUT pStream -- ָ��������
// 
// ����ֵ�����ؼ����ַ�
// �����������õ������ַ���
// ����: 
// **************************************************
static char SMIL_GetRemberCode(char **pStream)
{
	char realCode;
	int i,lenRemberCode;

	realCode=**pStream;
	*pStream+=1;  // jump '&'
	if (**pStream == '#')
	{  // ��һ��������
		realCode=atoi(*pStream+1);
	}
	else
	{  // ��һ���������
		lenRemberCode=SMIL_GetRemberCodeLen(*pStream); // �õ����ų���
		if (lenRemberCode)
		{
			for (i=0;i<100;i++)
			{  // ���б��в��Ҷ�Ӧ�ļ����ַ�
				if (strncmp((*pStream),SMIL_remberCodeList[i].RemberCode,lenRemberCode)==0)
				{
					realCode=SMIL_remberCodeList[i].ReadCode;
					break;
				}
			}
			*pStream+=lenRemberCode+1;
		}
	}
	if (realCode&0x80)  // ������ʾ�ַ����ÿո����
		realCode=' ';  // for  the  current state ,can't show a char with >0x80
	return realCode; //�����ҵ����ַ�
}

// **************************************************
// ������int SMIL_GetRemberCodeLen(char *pStream)
// ������
// 	IN pStream -- ָ��������
// 
// ����ֵ�����ؼ�����ŵĳ���
// �����������õ�������ŵĳ��ȡ�
// ����: 
// **************************************************
int SMIL_GetRemberCodeLen(char *pStream)
{
	int len=0;

	while(*pStream)
	{
		if (*pStream==';')
		{  // ��Ϊ������ŵı�Ҫ��β��
			return len;
		}
		pStream++;
		len++;
	}
	return 0;  // û���ҵ���β��������
}
// **************************************************
// ������BOOL SMIL_IsPercent(char *string)
// ������
// 	IN string -- ָ���ַ���
// 
// ����ֵ����һ���ٷֱ��ִ�����TRUE�����򷵻�FALSE
// �����������ж�һ��ָ���ִ��Ƿ���һ���ٷֱ��ִ���
// ����: 
// **************************************************
BOOL SMIL_IsPercent(char *string)
{
	while(*string)
	{
		if (*string=='%') // �аٷֱȷ��ţ�����TRUE
			return TRUE;
		string++;
	}
	return FALSE; // û���ҵ��ٷֱȷ���
}


*/
