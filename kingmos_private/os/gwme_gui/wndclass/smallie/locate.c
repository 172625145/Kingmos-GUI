/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：小屏幕模式的IE浏览类分解各部分的函数
版本号：1.0.0.456
开发时期：2001-02-13
作者：陈建明 Jami
修改记录：
**************************************************/
//#include "stdafx.h"
//#include <stdio.h>
#include "Locate.h"
#include "eHtmView.h"

// **************************************************
// 定义区域
// **************************************************
typedef struct
{
	char *tagName;
	TAGID tagID;
}SMIE_TAGLIST;

static SMIE_TAGLIST SMIE_tagList[]={  // 标签列表
	{"A",			TAGID_A},
	{"ABBR",		TAGID_ABBR},
	{"ACRONYM",		TAGID_ACRONYM},
	{"ADDRESS",		TAGID_ADDRESS},
	{"APPLLET",		TAGID_APPLLET},
	{"AREA",		TAGID_AREA},
	{"B",			TAGID_B},
	{"BASE",		TAGID_BASE},
	{"BASEFONT",	TAGID_BASEFONT},
	{"BDO",			TAGID_BDO},
	{"BIG",			TAGID_BIG},
	{"BLOCKQUOTE",	TAGID_BLOCKQUOTE},
	{"BODY",		TAGID_BODY},
	{"BR",			TAGID_BR},
	{"BUTTON",		TAGID_BUTTON},
	{"CAPTION",		TAGID_CAPTION},
	{"CENTER",		TAGID_CENTER},
	{"CITE",		TAGID_CITE},
	{"CODE",		TAGID_CODE},
	{"COL",			TAGID_COL},
	{"COLGROUP",	TAGID_COLGROUP},
	{"DO",			TAGID_DO},
	{"DEL",			TAGID_DEL},
	{"DFN",			TAGID_DFN},
	{"DIR",			TAGID_DIR},
	{"DIV",			TAGID_DIV},
	{"DL",			TAGID_DL},
	{"DT",			TAGID_DT},
	{"EM",			TAGID_EM},
	{"FIELDSET",	TAGID_FIELDSET},
	{"FONT",		TAGID_FONT},
	{"FORM",		TAGID_FORM},
	{"FRAME",		TAGID_FRAME},
	{"FRAMESET",	TAGID_FRAMESET},
	{"H1",			TAGID_H1},
	{"H2",			TAGID_H2},
	{"H3",			TAGID_H3},
	{"H4",			TAGID_H4},
	{"H5",			TAGID_H5},
	{"H6",			TAGID_H6},
	{"HEAD",		TAGID_HEAD},
	{"HTML",		TAGID_HTML},
	{"HR",			TAGID_HR},
	{"IFRAME",		TAGID_IFRAME},
	{"IMG",			TAGID_IMG},
	{"INPUT",		TAGID_INPUT},
	{"INS",			TAGID_INS},
	{"ISINDEX",		TAGID_ISINDEX},
	{"KBD",			TAGID_KBD},
	{"LABEL",		TAGID_LABEL},
	{"LEGENT",		TAGID_LEGENT},
	{"LI",			TAGID_LI},
	{"LINK",		TAGID_LINK},
	{"MAP",			TAGID_MAP},
	{"MENU",		TAGID_MENU},
	{"META",		TAGID_META},
	{"NOFRAMES",	TAGID_NOFRAMES},
	{"NOSCRIPT",	TAGID_NOSCRIPT},
	{"OBJECT",		TAGID_OBJECT},
	{"OL",			TAGID_OL},
	{"OPTGROUP",	TAGID_OPTGROUP},
	{"OPTION",		TAGID_OPTION},
	{"P",			TAGID_P},
	{"PARAM",		TAGID_PARAM},
	{"PRE",			TAGID_PRE},
	{"Q",			TAGID_Q},
	{"S",			TAGID_S},
	{"SAMP",		TAGID_SAMP},
	{"SCRIPT",		TAGID_SCRIPT},
	{"SELECT",		TAGID_SELECT},
	{"SMALL",		TAGID_SMALL},
	{"SPAN",		TAGID_SPAN},
	{"STRIKE",		TAGID_STRIKE},
	{"STRONG",		TAGID_STRONG},
	{"STYLE",		TAGID_STYLE},
	{"TABLE",		TAGID_TABLE},
	{"TBODY",		TAGID_TBODY},
	{"TD",			TAGID_TD},
	{"TEXTAREA",	TAGID_TEXTAREA},
	{"TH",			TAGID_TH},
	{"THEAD",		TAGID_THEAD},
	{"TFOOT",		TAGID_TFOOT},
	{"TITLE",		TAGID_TITLE},
	{"TR",			TAGID_TR},
	{"TT",			TAGID_TT},
	{"U",			TAGID_U},
	{"UL",			TAGID_UL},
	{"VAR",			TAGID_VAR},
};


typedef struct
{
	char *propertyName;
	PROPERTYID propertyID;
}SMIE_PROPERTYLIST;

static SMIE_PROPERTYLIST SMIE_propertyList[]={ // 属性内容列表
	{"abbr",PROPERTYID_ABBR},
	{"accept-charset",PROPERTYID_ACCEPT_CHARSET},
	{"accept",PROPERTYID_ACCEPT},
	{"accesskey",PROPERTYID_ACCESSKEY},
	{"action",PROPERTYID_ACTION},
	{"align",PROPERTYID_ALIGN},
	{"alink",PROPERTYID_ALINK},
	{"alt",PROPERTYID_ALT},
	{"archive",PROPERTYID_ARCHIVE},
	{"axis",PROPERTYID_AXIS},
	{"background",PROPERTYID_BACKGROUND},
	{"bgcolor",PROPERTYID_BGCOLOR},
	{"border",PROPERTYID_BORDER},
	{"cellpadding",PROPERTYID_CELLPADDING},
	{"cellspacing",PROPERTYID_CELLSPACING},
	{"char",PROPERTYID_CHAR},
	{"charoff",PROPERTYID_CHAROFF},
	{"charset",PROPERTYID_CHARSET},
	{"checked",PROPERTYID_CHECKED},
	{"cite",PROPERTYID_CITE},
	{"class",PROPERTYID_CLASS},
	{"classid",PROPERTYID_CLASSID},
	{"clear",PROPERTYID_CLEAR},
	{"code",PROPERTYID_CODE},
	{"codebase",PROPERTYID_CODEBASE},
	{"codetype",PROPERTYID_CODETYPE},
	{"color",PROPERTYID_COLOR},
	{"cols",PROPERTYID_COLS},
	{"colspan",PROPERTYID_COLSPAN},
	{"compact",PROPERTYID_COMPACT},
	{"content",PROPERTYID_CONTENT},
	{"coords",PROPERTYID_COORDS},
	{"data",PROPERTYID_DATA},
	{"datetime",PROPERTYID_DATETIME},
	{"declare",PROPERTYID_DECLARE},
	{"defer",PROPERTYID_DEFER},
	{"dir",PROPERTYID_DIR},
	{"disabled",PROPERTYID_DISABLED},
	{"enctype",PROPERTYID_ENCTYPE},
	{"face",PROPERTYID_FACE},
	{"for",PROPERTYID_FOR},
	{"frame",PROPERTYID_FRAME},
	{"frameborder",PROPERTYID_FRAMEBORDER},
	{"headers",PROPERTYID_HEADERS},
	{"height",PROPERTYID_HEIGHT},
	{"href",PROPERTYID_HREF},
	{"herflang",PROPERTYID_HERFLANG},
	{"hspace",PROPERTYID_HSPACE},
	{"http-equiv",PROPERTYID_HTTP_EQUIV},
	{"id",PROPERTYID_ID},
	{"ismap",PROPERTYID_ISMAP},
	{"label",PROPERTYID_LABEL},
	{"lang",PROPERTYID_LANG},
	{"language",PROPERTYID_LANGUAGE},
	{"link",PROPERTYID_LINK},
	{"longdesc",PROPERTYID_LONGDESC},
	{"marginheight",PROPERTYID_MARGINHEIGHT},
	{"marginwidth",PROPERTYID_MARGINWIDTH},
	{"maxlength",PROPERTYID_MAXLENGTH},
	{"media",PROPERTYID_MEDIA},
	{"method",PROPERTYID_METHOD},
	{"multiple",PROPERTYID_MULTIPLE},
	{"name",PROPERTYID_NAME},
	{"nohref",PROPERTYID_NOHREF},
	{"noresize",PROPERTYID_NORESIZE},
	{"noshade",PROPERTYID_NOSHADE},
	{"nowrap",PROPERTYID_NOWRAP},
	{"object",PROPERTYID_OBJECT},
	{"onblur",PROPERTYID_ONBLUR},
	{"onchange",PROPERTYID_ONCHANGE},
	{"onclick",PROPERTYID_ONCLICK},
	{"ondblclick",PROPERTYID_ONDBLCLICK},
	{"onfocus",PROPERTYID_ONFOCUS},
	{"onkeydown",PROPERTYID_ONKEYDOWN},
	{"onkeypress",PROPERTYID_ONKEYPRESS},
	{"onkeyup",PROPERTYID_ONKEYUP},
	{"onload",PROPERTYID_ONLOAD},
	{"onmousedown",PROPERTYID_ONMOUSEDOWN},
	{"onmousemove",PROPERTYID_ONMOUSEMOVE},
	{"onmouseover",PROPERTYID_ONMOUSEOVER},
	{"onmouseup",PROPERTYID_ONMOUSEUP},
	{"onreset",PROPERTYID_ONRESET},
	{"onselect",PROPERTYID_ONSELECT},
	{"onsubmit",PROPERTYID_ONSUBMIT},
	{"onunload",PROPERTYID_ONUNLOAD},
	{"profile",PROPERTYID_PROFILE},
	{"prompt",PROPERTYID_PROMPT},
	{"readonly",PROPERTYID_READONLY},
	{"rel",PROPERTYID_REL},
	{"rev",PROPERTYID_REV},
	{"rows",PROPERTYID_ROWS},
	{"rowspan",PROPERTYID_ROWSPAN},
	{"rules",PROPERTYID_RULES},
	{"scheme",PROPERTYID_SCHEME},
	{"scope",PROPERTYID_SCOPE},
	{"scrolling",PROPERTYID_SCROLLING},
	{"selected",PROPERTYID_SELECTED},
	{"shape",PROPERTYID_SHAPE},
	{"size",PROPERTYID_SIZE},
	{"span",PROPERTYID_SPAN},
	{"src",PROPERTYID_SRC},
	{"standby",PROPERTYID_STANDBY},
	{"start",PROPERTYID_START},
	{"style",PROPERTYID_STYLE},
	{"summary",PROPERTYID_SUMMARY},
	{"tabindex",PROPERTYID_TABINDEX},
	{"target",PROPERTYID_TARGET},
	{"text",PROPERTYID_TEXT},
	{"title",PROPERTYID_TITLE},
	{"type",PROPERTYID_TYPE},
	{"usemap",PROPERTYID_USEMAP},
	{"valign",PROPERTYID_VALIGN},
	{"value",PROPERTYID_VALUE},
	{"valuetype",PROPERTYID_VALUETYPE},
	{"version",PROPERTYID_VERSION},
	{"vlink",PROPERTYID_VLINK},
	{"vspace",PROPERTYID_VSPACE},
	{"width",PROPERTYID_WIDTH},
};

typedef struct
{
	char *RemberCode;
	TCHAR ReadCode;
}SMIE_REMBERCODELIST;

static SMIE_REMBERCODELIST SMIE_remberCodeList[]={  // 记忆符号列表
	{"amp",		(TCHAR)38},		//	1
	{"laquo",	(TCHAR)171},		//	2
	{"laquo",	(TCHAR)187},		//	3
	{"raquo",	(TCHAR)166},		//	4
	{"Aacute",	(TCHAR)193},		//	5
	{"Acirc",	(TCHAR)194},		//	6
	{"Auml",	(TCHAR)196},		//	7
	{"Agrave",	(TCHAR)192},		//	8
	{"Aring",	(TCHAR)197},		//	9
	{"Atilde",	(TCHAR)195},		//	10
	{"AElig",	(TCHAR)198},		//	11
	{"Ccedil",	(TCHAR)199},		//	12
	{"Eacute",	(TCHAR)201},		//	13
	{"Ecirc",	(TCHAR)202},		//	14
	{"Euml",	(TCHAR)203},		//	15
	{"Egrave",	(TCHAR)200},		//	16
	{"ETH",		(TCHAR)208},		//	17
	{"Iacute",	(TCHAR)205},		//	18	
	{"Icicr",	(TCHAR)206},		//	19
	{"Iuml",	(TCHAR)207},		//	20
	{"Igrave",	(TCHAR)204},		//	21
	{"Ntilde",	(TCHAR)209},		//	22
	{"Oacute",	(TCHAR)211},		//	23
	{"Ocirc",	(TCHAR)212},		//	24
	{"Ouml",	(TCHAR)214},		//	25
	{"Ograve",	(TCHAR)210},		//	26
	{"Oslash",	(TCHAR)216},		//	27
	{"Otilde",	(TCHAR)213},		//	28
	{"THORN",	(TCHAR)222},		//	29
	{"Uacute",	(TCHAR)218},		//	30
	{"Ucirc",	(TCHAR)219},		//	31
	{"Uuml",	(TCHAR)220},		//	32
	{"Ugrave",	(TCHAR)217},		//	33
	{"Yacute",	(TCHAR)221},		//	34
	{"cent",	(TCHAR)162},		//	35
	{"reg",		(TCHAR)174},		//	36
	{"copy",	(TCHAR)169},		//	37
	{"curren",	(TCHAR)164},		//	38
	{"deg",		(TCHAR)176},		//	39
	{"divide",	(TCHAR)247},		//	40
	{"ordf",	(TCHAR)170},		//	41
	{"frac12",	(TCHAR)189},		//	42
	{"frac14",	(TCHAR)188},		//	43
	{"frac34",	(TCHAR)190},		//	44
	{"gt",		(TCHAR)62},		//	45
	{"iexcl",	(TCHAR)161},		//	46
	{"iquest",	(TCHAR)191},		//	47
	{"lt",		(TCHAR)60},		//	48
	{"ordm",	(TCHAR)186},		//	49
	{"micro",	(TCHAR)181},		//	50
	{"middot",	(TCHAR)183},		//	51
	{"times",	(TCHAR)215},		//	52
	{"not",		(TCHAR)172},		//	53
	{"nbsp",	(TCHAR)160},		//	54
	{"para",	(TCHAR)182},		//	55
	{"plusmn",	(TCHAR)177},		//	56
	{"pound",	(TCHAR)163},		//	57
	{"quot",	(TCHAR)134},		//	58
	{"sect",	(TCHAR)167},		//	59
	{"aacute",	(TCHAR)225},		//	60	
	{"acirc",	(TCHAR)226},		//	61
	{"auml",	(TCHAR)228},		//	62
	{"agrave",	(TCHAR)224},		//	63
	{"aring",	(TCHAR)229},		//	64
	{"atilde",	(TCHAR)227},		//	65
	{"aelig",	(TCHAR)230},		//	66
	{"ccedil",	(TCHAR)231},		//	67
	{"eacute",	(TCHAR)233},		//	68
	{"ecirc",	(TCHAR)234},		//	69
	{"euml",	(TCHAR)235},		//	70
	{"egrave",	(TCHAR)232},		//	71
	{"eth",		(TCHAR)240},		//	72
	{"iacute",	(TCHAR)237},		//	73
	{"icirc",	(TCHAR)238},		//	74
	{"iuml",	(TCHAR)239},		//	75
	{"igrave",	(TCHAR)236},		//	76
	{"ntilde",	(TCHAR)241},		//	77	
	{"oacute",	(TCHAR)243},		//	78
	{"ocirc",	(TCHAR)244},		//	79
	{"ouml",	(TCHAR)246},		//	80
	{"ograve",	(TCHAR)242},		//	81
	{"oslash",	(TCHAR)248},		//	82
	{"otilde",	(TCHAR)245},		//	83
	{"szlig",	(TCHAR)223},		//	84
	{"thorn",	(TCHAR)254},		//	85
	{"uacute",	(TCHAR)250},		//	86
	{"ucirc",	(TCHAR)251},		//	87	
	{"uuml",	(TCHAR)252},		//	88
	{"ugrave",	(TCHAR)249},		//	89
	{"yacute",	(TCHAR)253},		//	90
	{"yuml",	(TCHAR)255},		//	91
	{"shy",		(TCHAR)173},		//	92
	{"acute",	(TCHAR)180},		//	93
	{"cedil",	(TCHAR)184},		//	94
	{"uml",		(TCHAR)168},		//	95
	{"macr",	(TCHAR)175},		//	96
	{"sup1",	(TCHAR)185},		//	97
	{"sup2",	(TCHAR)178},		//	98
	{"sup3",	(TCHAR)179},		//	99	
	{"yen",		(TCHAR)165},		//	100
};

// **************************************************
// 函数声明区
// **************************************************

static BOOL SMIE_GetTag(char **pStream,char *pTagName);
static BOOL SMIE_GetTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess);
static TAGID SMIE_GetTagID(char *pTagName);

static BOOL SMIE_GetProperty(char **pStream,char *pPropertyName);
static BOOL SMIE_GetPropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize);
static PROPERTYID SMIE_GetPropertyID(char *pPropertyName);

static char SMIE_GetRemberCode(char **pStream);
int SMIE_GetRemberCodeLen(char *pStream);
//#define DEBUG_OUT

#ifdef DEBUG_OUT
extern HANDLE out_file;
static void SMIE_Output(char *pTitle,char *pContent,BOOL bReturn);
#endif


// **************************************************
// 声明：void SMIE_Filter(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：无
// 功能描述：过滤掉数据流中的无效代码。
// 引用: 
// **************************************************
void SMIE_Filter(char **pStream)
{  // 滤掉不需要的字符
		while(**pStream==' '||**pStream=='\r'||**pStream=='\n'||**pStream=='\t')
			*pStream+=1;
}

// **************************************************
// 声明：TAGID SMIE_LocateTag(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：返回当前的标签标号
// 功能描述：定位当前的标签。
// 引用: 
// **************************************************
TAGID SMIE_LocateTag(char **pStream)
{
	char pTagName[MAXTAGNAMELEN];
	TAGID tagID;
	SMIE_Filter(pStream);  // 滤掉无效字符
	while(1)
	{
		if (**pStream==0)  // 数据流结束
			return TAGID_NULL;
		if (**pStream=='<')
		{  // 标签开始
			if (*(*pStream+1)=='!'&&*(*pStream+2)=='-'&&*(*pStream+3)=='-')
			{  // 注释开始 <!--
				while(1)
				{
					if (**pStream==0)  // 没有字符了
						return TAGID_NULL;
					if (**pStream=='-'&&*(*pStream+1)=='-'&&*(*pStream+2)=='>')
					{  // 注释结束
						*pStream+=3;  // '-->'
						break;
					}
					*pStream+=1;
				}
//				*pStream+=1;  // '>'
				/*
				while(1)
				{
					if (**pStream!='\n'&&**pStream!='\r'&&**pStream!=' '&&**pStream!='\t')  // if has a CR ,then jump this CR
						break;
					*pStream+=1;
				}
				*/
				SMIE_Filter(pStream);  // 滤掉无效字符
				continue;
			}
			if ((*(*pStream+1)>='a'&&*(*pStream+1)<='z')||(*(*pStream+1)>='A'&&*(*pStream+1)<='Z'))
			{ // 英文字符，标签开始
				break;
			}
			if ((*(*pStream+1)=='/')||(*(*pStream+1)=='?')||(*(*pStream+1)=='!'))
			{ // 特殊字符
				break;
			}
			return TAGID_TEXT;  // 不是指定标签，为文本内容
		}
		else
			return TAGID_TEXT;  // 文本内容
	}
	SMIE_GetTag(pStream,pTagName); // 得到标签名称
#ifdef DEBUG_OUT
	Output("",pTagName,FALSE);
#endif
	tagID=SMIE_GetTagID(pTagName); // 得到标签标号
#ifdef DEBUG_OUT
	{
		char Buffer[16];
		sprintf(Buffer,"%04d",tagID);
		Output("   tagID=",Buffer,TRUE);
	}
#endif
	return tagID;  // 返回标签标号
}

// **************************************************
// 声明：void SMIE_ToNextTag(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：无
// 功能描述：将当前指针指到下一个标签。
// 引用: 
// **************************************************
void SMIE_ToNextTag(char **pStream)
{
//	char pTagName[MAXTAGNAMELEN];
//	TAGID tagID;
	while(1)
	{
		if (**pStream==0)
			return ;
		if (**pStream=='>')
		{  // current tag is end ,then next position is new tag
			*pStream+=1;  // jump '>'
/*
			while(1)
			{
				if (**pStream!='\n'&&**pStream!='\r'&&**pStream!=' '&&**pStream!='\t')  // if has a CR or bank,then jump this CR and bank
					break;
				*pStream+=1;
			}
			*/
			SMIE_Filter(pStream);
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
// 声明：TAGID SMIE_NextTagIs(char *pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：返回下一个标签的标号
// 功能描述：得到下一个标签标号。
// 引用: 
// **************************************************
TAGID SMIE_NextTagIs(char *pStream)
{
	char pTagName[MAXTAGNAMELEN];
	TAGID tagID;
	SMIE_Filter(&pStream);
	while(1)
	{
		if (*pStream==0)
			return 0;
		if (*pStream=='<')
		{  // 标签开始
			if (*(pStream+1)=='!'&&*(pStream+2)=='-'&&*(pStream+3)=='-')
			{  // 注释开始 <!--
				while(1)
				{
					if (*pStream==0)
						return TAGID_NULL;
					if (*pStream=='-'&&*(pStream+1)=='-'&&*(pStream+2)=='>')
					{ //注释结束
						pStream+=3;  // '-->'
						break;
					}
					pStream+=1;
				}
/*
				while(1)
				{
					if (*pStream!='\n'&&*pStream!='\r'&&*pStream==' '&&*pStream=='\t')  // if has a CR ,then jump this CR
						break;
					pStream+=1;
				}
*/
				SMIE_Filter(&pStream);
				continue;
			}
			if ((*(pStream+1)>='a'&&*(pStream+1)<='z')||(*(pStream+1)>='A'&&*(pStream+1)<='Z'))
			{  // 英文字符
				break;
			}
			if ((*(pStream+1)=='/')||(*(pStream+1)=='?')||(*(pStream+1)=='!'))
			{ // 特殊符号
				break;
			}
			return TAGID_TEXT;  // 文本标签
		}
		else
			return TAGID_TEXT;  // 文本标签
//		pStream+=1;
	}
	SMIE_GetTag(&pStream,pTagName);  // 得到标签名称
	tagID=SMIE_GetTagID(pTagName); // 得到标签标号
	return tagID; // 返回标签标号
}

// **************************************************
// 声明：BOOL SMIE_LocateTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
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
BOOL SMIE_LocateTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
{
	SMIE_GetTagContent(pStream,pTagContent,dwSize,bPreProcess);  // 得到标签内容
#ifdef DEBUG_OUT
	if (pTagContent[0]!=0)
		Output(" Content = ",pTagContent,TRUE);
#endif
//	JAMIMSG(DBG_FUNCTION,(TEXT("LocateTagContent=\"%s\"\r\n"),pTagContent));
	return TRUE;
}

// **************************************************
// 声明：PROPERTYID SMIE_LocateProperty(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：返回定位的属性标号
// 功能描述：定位当前的属性标号。
// 引用: 
// **************************************************
PROPERTYID SMIE_LocateProperty(char **pStream)
{
	char pPropertyName[MAXPROPERTYNAMELEN];
	PROPERTYID propertyID;

//		while(**pStream==' '||**pStream=='\r'||**pStream=='\n'||**pStream=='\t')
//			*pStream+=1;
		SMIE_Filter(pStream); // 滤掉无效代码

		if (**pStream=='>'||**pStream==0)
			return 0;  // 标签结束
		SMIE_GetProperty(pStream,pPropertyName);  // 得到标签属性名称
#ifdef DEBUG_OUT
		Output("	",pPropertyName,FALSE);
#endif
		propertyID=SMIE_GetPropertyID(pPropertyName);  // 得到标签属性标号
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
// 声明：BOOL SMIE_LocatePropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pPropertyContent -- 存放属性内容的缓存
// 	IN dwSize -- 缓存大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：定位当前标签属性的内容。
// 引用: 
// **************************************************
BOOL SMIE_LocatePropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
{
	SMIE_GetPropertyContent(pStream,pPropertyContent,dwSize); // 得到属性内容
#ifdef DEBUG_OUT
	Output(" ::: ",pPropertyContent,TRUE);
#endif
	return TRUE;
}

// **************************************************
// 声明：static BOOL SMIE_GetTag(char **pStream,char *pTagName)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pTagName -- 存放标签名称的缓存
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到标签的名称。
// 引用: 
// **************************************************
static BOOL SMIE_GetTag(char **pStream,char *pTagName)
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
// 声明：static BOOL SMIE_GetTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
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
static BOOL SMIE_GetTagContent(char **pStream,char *pTagContent,DWORD dwSize,BOOL bPreProcess)
{
  DWORD dwTagContentLen=0;
/*
	if (**pStream!='>')
	{
		*pTagContent=0;
		return FALSE;
	}
	*pStream+=1;  // jump '>'
*/
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
			*pTagContent++=SMIE_GetRemberCode(pStream); // 得到记忆字符
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
// 声明：DWORD SMIE_GetTagContentLen(char *pStream,BOOL bPreProcess)
// 参数：
// 	IN/OUT pStream -- 指定数据流
//	 IN bPreProcess -- 是否需要预处理
// 
// 返回值：返回标签内容的长度
// 功能描述：得到标签内容的长度。
// 引用: 
// **************************************************
DWORD SMIE_GetTagContentLen(char *pStream,BOOL bPreProcess)
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
			SMIE_GetRemberCode(&pStream);
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
// 声明：static TAGID SMIE_GetTagID(char *pTagName)
// 参数：
// 	IN pTagName -- 标签名称
// 
// 返回值：返回标签的标号
// 功能描述：得到标签的标号。
// 引用: 
// **************************************************
static TAGID SMIE_GetTagID(char *pTagName)
{
	int i;
	TAGID tagID_End=0;
	if (*pTagName=='/')
	{
		pTagName++;
		tagID_End=TAGID_END;
	}

	for (i=0;i<88;i++)
	{
		if (stricmp(SMIE_tagList[i].tagName,pTagName)==0)
			return (SMIE_tagList[i].tagID+tagID_End);
	}
	return TAGID_UNKNOW;
}

// **************************************************
// 声明：static BOOL SMIE_GetProperty(char **pStream,char *pPropertyName)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pPropertyName -- 存放标签的名称
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到属性名称。
// 引用: 
// **************************************************
static BOOL SMIE_GetProperty(char **pStream,char *pPropertyName)
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
// 声明：static BOOL SMIE_GetPropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 	OUT pPropertyContent -- 存放属性内容的缓存
// 	IN dwSize -- 缓存大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到属性的内容。
// 引用: 
// **************************************************
static BOOL SMIE_GetPropertyContent(char **pStream,char *pPropertyContent,DWORD dwSize)
{
	WORD wPropertyContentLen=0;
	BOOL bIsQuotationMark=FALSE;
	BOOL bIsDoubleQuotation=FALSE;

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
		bIsDoubleQuotation = TRUE; // 是双引号
		*pStream+=1;
	}
	if (**pStream=='\'')
	{ // 是单引号
		bIsQuotationMark=TRUE;
		bIsDoubleQuotation = FALSE; // 是双引号
		*pStream+=1;
	}
	while(1)
	{
		if (**pStream=='>')
			break;  // 标签结束
		if (bIsQuotationMark==FALSE&&**pStream==' ')
			break; // 不在引号，空格结束
		if (bIsQuotationMark==TRUE&&bIsDoubleQuotation == TRUE&&**pStream=='\"')
		{ // 引号结束
			*pStream+=1;
			break;
		}
		if (bIsQuotationMark==TRUE&&bIsDoubleQuotation == FALSE&&**pStream=='\'')
		{ // 引号结束
			*pStream+=1;
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
// 声明：static PROPERTYID SMIE_GetPropertyID(char *pPropertyName)
// 参数：
// 	IN pPropertyName -- 属性的名称
// 
// 返回值：返回属性的标号
// 功能描述：得到属性标号。
// 引用: 
// **************************************************
static PROPERTYID SMIE_GetPropertyID(char *pPropertyName)
{
	int i;
	for (i=0;i<118;i++)
	{ //从属性列表中查找对应的标号
		if (stricmp(SMIE_propertyList[i].propertyName,pPropertyName)==0)
			return (SMIE_propertyList[i].propertyID);
	}
	return PROPERTY_UNKNOW; // 没有找到对应的标号
}

// **************************************************
// 声明：WORD SMIE_GetAlignType(char *pAlign)
// 参数：
// 	IN pAlign -- 对齐的名称
// 
// 返回值：返回对齐方式
// 功能描述：得到对齐方式。
// 引用: 
// **************************************************
WORD SMIE_GetAlignType(char *pAlign)
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
// 声明：static char SMIE_GetRemberCode(char **pStream)
// 参数：
// 	IN/OUT pStream -- 指定数据流
// 
// 返回值：返回记忆字符
// 功能描述：得到记忆字符。
// 引用: 
// **************************************************
static char SMIE_GetRemberCode(char **pStream)
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
		lenRemberCode=SMIE_GetRemberCodeLen(*pStream); // 得到符号长度
		if (lenRemberCode)
		{
			for (i=0;i<100;i++)
			{  // 从列表中查找对应的记忆字符
				if (strncmp((*pStream),SMIE_remberCodeList[i].RemberCode,lenRemberCode)==0)
				{
					realCode=SMIE_remberCodeList[i].ReadCode;
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
// 声明：int SMIE_GetRemberCodeLen(char *pStream)
// 参数：
// 	IN pStream -- 指定数据流
// 
// 返回值：返回记忆符号的长度
// 功能描述：得到记忆符号的长度。
// 引用: 
// **************************************************
int SMIE_GetRemberCodeLen(char *pStream)
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
// 声明：BOOL SMIE_IsPercent(char *string)
// 参数：
// 	IN string -- 指定字符串
// 
// 返回值：是一个百分比字串返回TRUE，否则返回FALSE
// 功能描述：判断一个指定字串是否是一个百分比字串。
// 引用: 
// **************************************************
BOOL SMIE_IsPercent(char *string)
{
	while(*string)
	{
		if (*string=='%') // 有百分比符号，返回TRUE
			return TRUE;
		string++;
	}
	return FALSE; // 没有找到百分比符号
}
#ifdef DEBUG_OUT
// **************************************************
// 声明：static void SMIE_Output(char *pTitle,char *pContent,BOOL bReturn)
// 参数：
// 	IN pTitle -- 输出标题
// 	IN pContent -- 输出内容
//     IN bReturn -- 是否需要回车符
// 
// 返回值：无
// 功能描述：调试输出内容到文件。
// 引用: 
// **************************************************
static void SMIE_Output(char *pTitle,char *pContent,BOOL bReturn)
{
	char Buffer[1024];
	DWORD dwWriteLen;

		sprintf(Buffer,"%s%s",pTitle,pContent);
		WriteFile(out_file,Buffer,strlen(Buffer),&dwWriteLen,NULL);
		if (bReturn)
			WriteFile(out_file,"\r\n",2,&dwWriteLen,NULL);
}
#endif

// **************************************************
// 声明：WORD GetFrameStyle(char *pFrame)
// 参数：
// 	IN pFrame -- 当前的FRAME文本
// 
// 返回值：返回当前的FRAME的类型
// 功能描述：得到当前的FRAME的类型。
// 引用: 
// **************************************************
WORD GetFrameStyle(char *pFrame)
{
#define FRAMESTYLENUM 9
	struct {
		char *frameText;
		WORD frameStyle;
	}FrameTable[]={
		{"void", FRAME_VOID},
		{"above",FRAME_ABOVE},
		{"below",FRAME_BELOW},
		{"hsides",FRAME_HSIDES},
		{"vsides",FRAME_VSIDES},
		{"lhs",FRAME_LHS},
		{"rhs",FRAME_RHS},
		{"border",FRAME_BORDER},
		{"box",FRAME_BOX},
	};
	int i;
	for (i=0;i<FRAMESTYLENUM;i++)
	{
		if (stricmp(pFrame,FrameTable[i].frameText)==0)
			return FrameTable[i].frameStyle;
	}
	return FRAME_VOID;
}

// **************************************************
// 声明：WORD GetRulesStyle(char *pRules)
// 参数：
// 	IN pRules -- 当前的RULES文本
// 
// 返回值：返回当前的RULES的类型
// 功能描述：得到当前的RULES的类型。
// 引用: 
// **************************************************
WORD GetRulesStyle(char *pRules)
{
#define RULESSTYLENUM 5
	struct {
		char *rulesText;
		WORD rulesStyle;
	}RulesTable[]={
		{"none", RULES_NONE},
		{"groups",RULES_GROUP},
		{"rows",RULES_ROWS},
		{"cols",RULES_COLS},
		{"all",RULES_ALL},
	};
	int i;
	for (i=0;i<RULESSTYLENUM;i++)
	{
		if (stricmp(pRules,RulesTable[i].rulesText)==0)
			return RulesTable[i].rulesStyle;
	}
	return RULES_NONE;
}
