/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：SMIL分解TAG文件
版本号：1.0.0.456
开发时期：2004-04-30
作者：陈建明 Jami
修改记录：
**************************************************/
#include "ewindows.h"
#include "smiltag.h"
//#include "debugwnd.h"

// **************************************************
// 定义区域
// **************************************************

#define MAXTAGNAMELEN	32
#define MAXATTRNAMELEN	32

// 定义SMIL TAG的列表结构
typedef struct
{
	char *tagName;
	TAGID tagID;
}SMIL_TAGLIST;

// 在这里列举了有效的TAG名称及标签ID
static SMIL_TAGLIST SMIL_tagList[]={  // 标签列表
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

static SMIL_ATTRLIST SMIL_AttrList[]={ // 属性内容列表
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
// 函数声明区
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
// 声明：void SMIL_Filter(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：无
// 功能描述：过滤掉数据流中的无效代码。
// 引用: 
// **************************************************
void SMIL_Filter(char **pStream)
{  // 滤掉不需要的字符
		while(**pStream==' '||**pStream=='\r'||**pStream=='\n'||**pStream=='\t')
			*pStream+=1;
}

// **************************************************
// 声明：TAGID SMIL_LocateTag(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流,并返回新的指针位置
// 
// 返回值：返回当前的标签标号
// 功能描述：定位当前的标签。
// 引用: 
// **************************************************
TAGID SMIL_LocateTag(char **pStream)
{
	char pTagName[MAXTAGNAMELEN];
	TAGID tagID;
	SMIL_Filter(pStream);  // 滤掉无效字符
	while(1)
	{
		if (**pStream==0)  // 数据流结束
			return SMILTAG_NULL;
		if (**pStream=='<')
		{  // 标签开始标记
			if (*(*pStream+1)=='!'&&*(*pStream+2)=='-'&&*(*pStream+3)=='-')
			{  // 注释开始 <!--
				while(1)
				{
					if (**pStream==0)  // 没有字符了
					{
						RETAILMSG(1,(TEXT("!!!ERROR,NO > TAG")));
						return SMILTAG_NULL;
					}
					if (**pStream=='-'&&*(*pStream+1)=='-'&&*(*pStream+2)=='>')
					{  // 注释结束
						*pStream+=3;  // '-->'
						break;
					}
					*pStream+=1;
				}
				continue; // 注释已经读完，从新开始搜索下一个标签
			}
			if ((*(*pStream+1)>='a'&&*(*pStream+1)<='z')||(*(*pStream+1)>='A'&&*(*pStream+1)<='Z'))
			{ // 英文字符，标签开始
				break;
			}
			if ((*(*pStream+1)=='/')||(*(*pStream+1)=='?')||(*(*pStream+1)=='!'))
			{ // 特殊字符
				break;
			}
			RETAILMSG(1,(TEXT("!!!ERROR,UNKNOW TAG\r\n")));
			return SMILTAG_NULL;
		}
		*pStream+=1; // 当前字符不是标签开始，作为无效字符，舍去
	}
	SMIL_GetTag(pStream,pTagName); // 得到标签名称
	RETAILMSG(1,(TEXT("The Current TAG is <%s>"),pTagName));
	tagID=SMIL_GetTagID(pTagName); // 得到标签标号
	return tagID;  // 返回标签标号
}

// **************************************************
// 声明：void SMIL_ToNextTag(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：无
// 功能描述：将当前指针指到下一个标签。
// 引用: 
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
// 声明：static BOOL SMIL_GetTag(char **pStream,char *pTagName)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pTagName -- 存放标签名称的缓存
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到标签的名称。
// 引用: 
// **************************************************
static BOOL SMIL_GetTag(char **pStream,char *pTagName)
{
	WORD wTagNameLen=0;

	*pStream+=1;  // '<'
	while(1)
	{
		if (**pStream==' '||**pStream=='>')
			break;  // 标签结束
		*pTagName++=**pStream;
		*pStream+=1;
		wTagNameLen++;
		if (wTagNameLen>=MAXTAGNAMELEN-1)
		{ //标签长度太大
//			MessageBox(NULL,"illegal Tag Name","Error",MB_OK);
			*pTagName=0;
			return FALSE;
		}
	}
	*pTagName=0; // 成功，添加结尾符
	return TRUE;
}

// **************************************************
// 声明：static TAGID SMIL_GetTagID(char *pTagName)
// 参数：
// 	IN pTagName -- 标签名称
// 
// 返回值：返回标签的标号
// 功能描述：得到标签的标号。
// 引用: 
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
		if (SMIL_tagList[i].tagName == NULL) // 已经没有标签了
			break; 
		if (stricmp(SMIL_tagList[i].tagName,pTagName)==0) // 比较当前标签与标签列表中的标签比较
			return (SMIL_tagList[i].tagID+tagID_End); // 找到当前标签，返回标签的标号
	}
	return SMILTAG_UNKNOW;
}

// **************************************************
// 声明：ATTRID SMIL_LocateAttribute(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流，并返回新的位置
// 
// 返回值：返回当前位置的属性标号
// 功能描述：定位当前的属性标号。
// 引用: 
// **************************************************
ATTRID SMIL_LocateAttribute(char **pStream)
{
	char pAttributeName[MAXATTRNAMELEN];
	ATTRID attrID;

		SMIL_Filter(pStream); // 滤掉无效代码

		if (**pStream=='>'||**pStream==0)
			return 0;  // 标签结束
		SMIL_GetAttribute(pStream,pAttributeName);  // 得到标签属性名称
		attrID=SMIL_GetAttrID(pAttributeName);  // 得到标签属性标号
		return attrID; // 返回标签属性标号
}

// **************************************************
// 声明：static BOOL SMIL_GetAttribute(char **pStream,char *pAttributeName)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pAttributeName -- 存放标签的名称
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到属性名称。
// 引用: 
// **************************************************
static BOOL SMIL_GetAttribute(char **pStream,char *pAttributeName)
{
	WORD wAttrNameLen=0;

	while(1)
	{
		if (**pStream==' '||**pStream=='='||**pStream=='>')
			break; // 碰到分隔符，退出。
		*pAttributeName++=**pStream;
		*pStream+=1;
		wAttrNameLen++;
		if (wAttrNameLen>=MAXATTRNAMELEN-1)
		{ // 长度太大，退出
			while(1)
			{
				if (**pStream=='>') // 将流指针移动到标签结束
					break;
				*pStream+=1;
			}
			RETAILMSG(1,(TEXT("!!! ERROR Attribute <%s>\r\n"),pAttributeName));
			*pAttributeName=0;
			return FALSE;
		}
	}
	*pAttributeName=0; // 完成，添加结尾符
	return TRUE;
}

// **************************************************
// 声明：BOOL SMIL_LocateAttributeContent(char **pStream,char *pAttrContent,DWORD dwSize)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pAttrContent -- 存放属性内容的缓存
// 	IN dwSize -- 缓存大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：定位当前标签属性的内容。
// 引用: 
// **************************************************
BOOL SMIL_LocateAttributeContent(char **pStream,char *pAttrContent,DWORD dwSize)
{
	SMIL_GetAttributeContent(pStream,pAttrContent,dwSize); // 得到属性内容
	return TRUE;
}

// **************************************************
// 声明：static BOOL SMIL_GetAttrContentContent(char **pStream,char *pAttrContent,DWORD dwSize)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pAttrContent -- 存放属性内容的缓存
// 	IN dwSize -- 缓存大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到属性的内容。
// 引用: 
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
		{  // 标签结束
			*pAttrContent=0;
			return TRUE;
		}
		*pStream+=1;
	}
	while(1)
	{
		if (**pStream==' ')
		{  // 跳过空格
			*pStream+=1;  // jump ' '
			continue;
		}
		break;
	}
	if (**pStream=='\"')
	{ // 是引号
		bIsQuotationMark=TRUE;
		*pStream+=1;
	}
	while(1)
	{
		if (**pStream=='>')
			break;  // 标签结束
		if (bIsQuotationMark==FALSE&&**pStream==' ')
			break; // 不在引号，空格结束
		if (bIsQuotationMark==TRUE&&**pStream=='\"')
		{ // 引号结束
			*pStream+=1;
			break;
		}
		*pAttrContent++=**pStream;
		*pStream+=1;
		wAttributeContentLen++;
		if (wAttributeContentLen>=dwSize-1)
		{ // 属性大小超过缓存大小，失败
			RETAILMSG(1,(TEXT("Property Content Buffer Overflow")));
			*pAttrContent=0;
			return FALSE;
		}
	}
	*pAttrContent=0;  // 成功，添加结尾符
	return TRUE;
}
// **************************************************
// 声明：static PROPERTYID SMIL_GetAttrID(char *pAttributeName)
// 参数：
// 	IN pAttributeName -- 属性的名称
// 
// 返回值：返回属性的标号
// 功能描述：得到属性标号。
// 引用: 
// **************************************************
static ATTRID SMIL_GetAttrID(char *pAttributeName)
{
	int i;
	for (i=0;;i++)
	{ //从属性列表中查找对应的标号
		if (SMIL_AttrList[i].attrName == NULL)
		{
			// 已经没有属性条目
			break;
		}
		if (stricmp(SMIL_AttrList[i].attrName,pAttributeName)==0)
			return (SMIL_AttrList[i].attrID);
	}
	return SMILATTR_UNKNOW; // 没有找到对应的标号
}

/*

// **************************************************
// 声明：BOOL SMIL_LocateTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
// 参数：
// 	IN/OUT pStream -- 指定数据流
//	 OUT pTagContent -- 用来要存放标签内容的缓存
//	 IN dwSize -- 缓存大小
//	 IN bPreProcess -- 是否需要预处理
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：定位标签内容。
// 引用: 
// **************************************************
BOOL SMIL_LocateTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
{
	SMIL_GetTagContent(pStream,pTagContent,dwSize,bPreProcess);  // 得到标签内容
#ifdef DEBUG_OUT
	if (pTagContent[0]!=0)
		Output(" Content = ",pTagContent,TRUE);
#endif
//	JAMIMSG(DBG_FUNCTION,(TEXT("LocateTagContent=\"%s\"\r\n"),pTagContent));
	return TRUE;
}

// **************************************************
// 声明：ATTRID SMIL_LocateProperty(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：返回定位的属性标号
// 功能描述：定位当前的属性标号。
// 引用: 
// **************************************************
ATTRID SMIL_LocateProperty(char **pStream)
{
	char pPropertyName[MAXPROPERTYNAMELEN];
	ATTRID propertyID;

//		while(**pStream==' '||**pStream=='\r'||**pStream=='\n'||**pStream=='\t')
//			*pStream+=1;
		SMIL_Filter(pStream); // 滤掉无效代码

		if (**pStream=='>'||**pStream==0)
			return 0;  // 标签结束
		SMIL_GetProperty(pStream,pPropertyName);  // 得到标签属性名称
#ifdef DEBUG_OUT
		Output("	",pPropertyName,FALSE);
#endif
		propertyID=SMIL_GetPropertyID(pPropertyName);  // 得到标签属性标号
#ifdef DEBUG_OUT
		{
			char Buffer[16];
			sprintf(Buffer,"%04d )",propertyID);
			Output(" (",Buffer,FALSE);
		}
#endif
		return propertyID; // 返回标签属性标号
}

// **************************************************
// 声明：BOOL SMIL_LocatePropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pPropertyContent -- 存放属性内容的缓存
// 	IN dwSize -- 缓存大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：定位当前标签属性的内容。
// 引用: 
// **************************************************
BOOL SMIL_LocatePropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
{
	SMIL_GetPropertyContent(pStream,pPropertyContent,dwSize); // 得到属性内容
#ifdef DEBUG_OUT
	Output(" ::: ",pPropertyContent,TRUE);
#endif
	return TRUE;
}

// **************************************************
// 声明：static BOOL SMIL_GetTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
// 参数：
// 	IN/OUT pStream -- 指定数据流
//	 OUT pTagContent -- 用来要存放标签内容的缓存
//	 IN dwSize -- 缓存大小
//	 IN bPreProcess -- 是否需要预处理
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：得到标签内容。
// 引用: 
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
		{  // 标签开始
			if ((*(*pStream+1)>='a'&&*(*pStream+1)<='z')||(*(*pStream+1)>='A'&&*(*pStream+1)<='Z'))
			{ // 英文字符
				break;
			}
			if ((*(*pStream+1)=='/')||(*(*pStream+1)=='?')||(*(*pStream+1)=='!'))
			{ // 特殊符号
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
		{ // 记忆字符
			*pTagContent++=SMIL_GetRemberCode(pStream); // 得到记忆字符
		}
		else 
		{ // 一般字符
			*pTagContent++=**pStream;
			*pStream+=1;
		}
		dwTagContentLen++;
		if (dwTagContentLen>=dwSize-1)
		{ // 长度超过缓存大小
		//	MessageBox(NULL,"Tag Content Buffer Overflow","Error",MB_OK);
			*pTagContent=0;
			return FALSE;
		}
	}
	*pTagContent=0; // 成功，添加结尾符
	JAMIMSG(DBG_FUNCTION,(TEXT("Exit GetTagContent\r\n")));
	return TRUE;
}
#define MAXPARAGRAPH  1024
// **************************************************
// 声明：DWORD SMIL_GetTagContentLen(char *pStream,BOOL bPreProcess)
// 参数：
// 	IN/OUT pStream -- 指定数据流
//	 IN bPreProcess -- 是否需要预处理
// 
// 返回值：返回标签内容的长度
// 功能描述：得到标签内容的长度。
// 引用: 
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
		{ // 标签开始符号
			if ((*(pStream+1)>='a'&&*(pStream+1)<='z')||(*(pStream+1)>='A'&&*(pStream+1)<='Z'))
			{ // 英文字符
				break;
			}
			if ((*(pStream+1)=='/')||(*(pStream+1)=='?')||(*(pStream+1)=='!'))
			{ // 指定的特殊符号
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
		{ // 是记忆码
			SMIL_GetRemberCode(&pStream);
		}
		else 
		{ // 是一般字符
			pStream+=1;
		}
		dwTagContentLen++;
	}
	return dwTagContentLen+1;  // will save the text and end code (\0)
}

// **************************************************
// 声明：static BOOL SMIL_GetAttribute(char **pStream,char *pPropertyName)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pPropertyName -- 存放标签的名称
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到属性名称。
// 引用: 
// **************************************************
static BOOL SMIL_GetProperty(char **pStream,char *pPropertyName)
{
	WORD wPropertyNameLen=0;

	while(1)
	{
		if (**pStream==' '||**pStream=='='||**pStream=='>')
			break; // 碰到分隔符，退出。
		*pPropertyName++=**pStream;
		*pStream+=1;
		wPropertyNameLen++;
		if (wPropertyNameLen>=MAXPROPERTYNAMELEN-1)
		{ // 长度太大，退出
			while(1)
			{
				if (**pStream=='>') // 将流指针移动到标签结束
					break;
				*pStream+=1;
			}
			//MessageBox(NULL,"illegal Property Name","Error",MB_OK);
			*pPropertyName=0;
			return FALSE;
		}
	}
	*pPropertyName=0; // 完成，添加结尾符
	return TRUE;
}
// **************************************************
// 声明：static BOOL SMIL_GetPropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pPropertyContent -- 存放属性内容的缓存
// 	IN dwSize -- 缓存大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到属性的内容。
// 引用: 
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
		{  // 标签结束
			*pPropertyContent=0;
			return TRUE;
		}
		*pStream+=1;
	}
	while(1)
	{
		if (**pStream==' ')
		{  // 跳过空格
			*pStream+=1;  // jump ' '
			continue;
		}
		break;
	}
	if (**pStream=='\"')
	{ // 是引号
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
			break;  // 标签结束
		if (bIsQuotationMark==FALSE&&**pStream==' ')
			break; // 不在引号，空格结束
		if (bIsQuotationMark==TRUE&&**pStream=='\"')
		{ // 引号结束
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
		{ // 属性大小超过缓存大小，失败
//			MessageBox(NULL,"Property Content Buffer Overflow","Error",MB_OK);
			*pPropertyContent=0;
			return FALSE;
		}
	}
	*pPropertyContent=0;  // 成功，添加结尾符
	return TRUE;
}
// **************************************************
// 声明：static ATTRID SMIL_GetPropertyID(char *pPropertyName)
// 参数：
// 	IN pPropertyName -- 属性的名称
// 
// 返回值：返回属性的标号
// 功能描述：得到属性标号。
// 引用: 
// **************************************************
static PROPERTYID SMIL_GetPropertyID(char *pPropertyName)
{
	int i;
	for (i=0;i<118;i++)
	{ //从属性列表中查找对应的标号
		if (stricmp(SMIL_propertyList[i].propertyName,pPropertyName)==0)
			return (SMIL_propertyList[i].propertyID);
	}
	return PROPERTY_UNKNOW; // 没有找到对应的标号
}

// **************************************************
// 声明：WORD SMIL_GetAlignType(char *pAlign)
// 参数：
// 	IN pAlign -- 对齐的名称
// 
// 返回值：返回对齐方式
// 功能描述：得到对齐方式。
// 引用: 
// **************************************************
WORD SMIL_GetAlignType(char *pAlign)
{
#define ALIGNTYPENUM 7
	struct {
		char *AlignText;
		WORD Align;
	}AlignTable[]={  // 对齐名称 -- 对齐方式标号 列表
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
	{ // 从列表中查找对应的对齐方式
		if (stricmp(pAlign,AlignTable[i].AlignText)==0)
			return AlignTable[i].Align;
	}
	return ALIGN_NONE; // 不认识的对齐方式
}


// **************************************************
// 声明：static char SMIL_GetRemberCode(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：返回记忆字符
// 功能描述：得到记忆字符。
// 引用: 
// **************************************************
static char SMIL_GetRemberCode(char **pStream)
{
	char realCode;
	int i,lenRemberCode;

	realCode=**pStream;
	*pStream+=1;  // jump '&'
	if (**pStream == '#')
	{  // 是一个立即数
		realCode=atoi(*pStream+1);
	}
	else
	{  // 是一个记忆符号
		lenRemberCode=SMIL_GetRemberCodeLen(*pStream); // 得到符号长度
		if (lenRemberCode)
		{
			for (i=0;i<100;i++)
			{  // 从列表中查找对应的记忆字符
				if (strncmp((*pStream),SMIL_remberCodeList[i].RemberCode,lenRemberCode)==0)
				{
					realCode=SMIL_remberCodeList[i].ReadCode;
					break;
				}
			}
			*pStream+=lenRemberCode+1;
		}
	}
	if (realCode&0x80)  // 不可显示字符，用空格代替
		realCode=' ';  // for  the  current state ,can't show a char with >0x80
	return realCode; //返回找到的字符
}

// **************************************************
// 声明：int SMIL_GetRemberCodeLen(char *pStream)
// 参数：
// 	IN pStream -- 指定数据流
// 
// 返回值：返回记忆符号的长度
// 功能描述：得到记忆符号的长度。
// 引用: 
// **************************************************
int SMIL_GetRemberCodeLen(char *pStream)
{
	int len=0;

	while(*pStream)
	{
		if (*pStream==';')
		{  // ；为记忆符号的必要结尾符
			return len;
		}
		pStream++;
		len++;
	}
	return 0;  // 没有找到结尾符，错误
}
// **************************************************
// 声明：BOOL SMIL_IsPercent(char *string)
// 参数：
// 	IN string -- 指定字符串
// 
// 返回值：是一个百分比字串返回TRUE，否则返回FALSE
// 功能描述：判断一个指定字串是否是一个百分比字串。
// 引用: 
// **************************************************
BOOL SMIL_IsPercent(char *string)
{
	while(*string)
	{
		if (*string=='%') // 有百分比符号，返回TRUE
			return TRUE;
		string++;
	}
	return FALSE; // 没有找到百分比符号
}


*/
