/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/

/**************************************************
文件说明：SMIL浏览类分析文本
版本号：1.0.0.456
开发时期：2004-04-30
作者：陈建明 JAMI
修改记录：
**************************************************/
#include "esmilbrowser.h"
#include "smilstruct.h"
//#include "DebugWnd.h"
#include "parsesmil.h"
#include "smiltag.h"


// **************************************************
// 定义区域
// **************************************************
#define MAX_ATTRCONTENTLEN	512

#define DEFAULT_DELAY	3000 // 默认的幻灯片的显示时间
// **************************************************
// 函数定义区域
// **************************************************
static LPHEAD ParseHead(LPTSTR *lppPtr);
static LPBODY ParseBody(LPTSTR *lppPtr);
static LPLAYOUT ParseLayout(LPTSTR *lppPtr);
static LPROOTLAYOUT ParseRootLayout(LPTSTR *lppPtr);
static LPREGION ParseRegion(LPTSTR *lppPtr);
static LPPAR ParsePar(LPTSTR *lppPtr);
static LPIMG ParseImg(LPTSTR *lppPtr);
static LPAUDIO ParseAudio(LPTSTR *lppPtr);
static LPTEXT ParseText(LPTSTR *lppPtr);
static LPREF ParseRef(LPTSTR *lppPtr);

static UINT GetPixels(LPTSTR lpAttrContent);
static UINT GetDurTime(LPTSTR lpAttrContent);
static void AdjustTheParTime(LPPAR lpPar);

LPTSTR SMILBufferAssign(const LPTSTR pSrc);

// **************************************************


// **************************************************
// 声明：LPSMIL ParseSMIL(LPTSTR lpContent)
// 参数：
// 	IN lpContent -- 要分析的文本内容
// 
// 返回值：返回分析后的SMIL指针
// 功能描述：分析SMIL的文件内容。
// 引用: 
// **************************************************
LPSMIL ParseSMIL(LPTSTR lpContent)
{
	LPSMIL lpSmil = NULL; // SMIL 结构指针，保存当前的SMIL内容
	LPTSTR lpPtr; // 用于保存当前已经分析的指针位置
	TAGID tagID; // 标签标号
	LPHEAD lpHead; // 保存头信息
	LPBODY lpBody; // 保存BODY信息
		
		RETAILMSG(1,(TEXT("+ParseText")));

		lpSmil = (LPSMIL)malloc(sizeof(SMIL)); // 分配SMIL结构的缓存

		if (lpSmil == NULL)
		{ // 分配缓存失败
			return NULL; // 返回失败
		}
		
		lpPtr = lpContent ;// 初始化到起始位置

		// 开始分析内容
		while(1)
		{
			tagID=SMIL_LocateTag(&lpPtr);  // 得到标签标号

			if(tagID == SMILTAG_HEAD)
			{
				// 当前是<HEAD>标签
				SMIL_ToNextTag(&lpPtr); // 到下一个标签的位置，即<HEAD> </HEAD>之间的标签
				lpHead = ParseHead(&lpPtr); // 分析头信息
				lpSmil->lpHead = lpHead; // 将当前头信息添加到SMIL中
			}
			if(tagID == SMILTAG_BODY)
			{
				// 当前是<BODY>标签
				SMIL_ToNextTag(&lpPtr); // 到下一个标签的位置，即<BODY> </BODY>之间的标签
				lpBody = ParseBody(&lpPtr); // 分析头信息
				lpSmil->lpBody = lpBody; // 将当前body信息添加到SMIL中
			}
			if(tagID == SMILTAG_SMIL + SMILTAG_END)
			{
				// 当前是</SMIL>标签
				break; // 已经结束
			}
			if(tagID == SMILTAG_NULL)
			{
				// 已经没有内容了
				break;
			}
			SMIL_ToNextTag(&lpPtr);
		}
	
		RETAILMSG(1,(TEXT("-ParseText")));
		return lpSmil;
}

// **************************************************
// 声明：static LPHEAD ParseHead(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的HEAD指针
// 功能描述 分析<HEAD>的内容。
// 引用: 
// **************************************************
static LPHEAD ParseHead(LPTSTR *lppPtr)
{
	LPHEAD lpHead; // 保存头信息
	LPLAYOUT lpLayout; // 保存LAYOUT信息
	TAGID tagID; // 当前标签的标号
	
		RETAILMSG(1,(TEXT("+ParseHead\r\n")));
		lpHead = (LPHEAD)malloc(sizeof(HEAD)); //  分配HEAD结构指针
		if (lpHead == NULL)
		{
			// 分配内存失败
			RETAILMSG(1,(TEXT("!!!ERROR MALLOC FAILURE IN [ParseHead]\r\n")));
			return NULL;
		}

		// 开始分析HEAD内容
		// HEAD 的内容包括 META， LAYOUT， ROOT-LAYOUT
		while(1)
		{
			tagID=SMIL_LocateTag(lppPtr);  // 得到标签标号

			if(tagID == SMILTAG_NULL)
			{
				// 已经没有内容了
				RETAILMSG(1,(TEXT("!!!ERROR IN <HEAD> TAG ,CAN'T OCCUR NULL TAG\r\n")));
				break;
			}
			if(tagID == SMILTAG_HEAD + SMILTAG_END)
			{
				// 当前是</HEAD>标签
				break; // HEAD 的内容已经结束
			}
			if(tagID == SMILTAG_META)
			{
				// 当前是<META>标签
			}
			if(tagID == SMILTAG_LAYOUT)
			{
				// 当前是<LAYOUT>标签
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置，即<LAYOUT> </LAYOUT>之间的标签
				lpLayout = ParseLayout(lppPtr); // 分析LAYOUT的内容
				lpHead ->lpLayout = lpLayout;  // 设置LAYOUT的结构到HEAD
			}
			if(tagID == SMILTAG_ROOTLAYOUT)
			{
				// 当前是<ROOT-LAYOUT>标签
			}
			SMIL_ToNextTag(lppPtr);
		}

		RETAILMSG(1,(TEXT("-ParseHead\r\n")));
		return lpHead;
}


// **************************************************
// 声明：static LPBODY ParseBody(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的BODY指针
// 功能描述 分析<BODY>的内容。
// 引用: 
// **************************************************
static LPBODY ParseBody(LPTSTR *lppPtr)
{
	LPBODY lpBody; // 保存头信息
	LPPAR lpPar; // 存放PAR的信息
	TAGID tagID; // 当前标签的标号
	
		RETAILMSG(1,(TEXT("+ParseBody\r\n")));
		lpBody = (LPBODY)malloc(sizeof(BODY)); //  分配BODY结构指针
		if (lpBody == NULL)
		{
			// 分配内存失败
			RETAILMSG(1,(TEXT("!!!ERROR MALLOC FAILURE IN [ParseBody]\r\n")));
			return NULL;
		}
		// 初始化lpBody结构
		lpBody->lpPar = NULL;

		// 开始分析BODY内容
		// BODY 的内容包括 PAR
		while(1)
		{
			tagID=SMIL_LocateTag(lppPtr);  // 得到标签标号

			if(tagID == SMILTAG_NULL)
			{
				// 已经没有内容了
				RETAILMSG(1,(TEXT("!!!ERROR IN <BODY> TAG ,CAN'T OCCUR NULL TAG\r\n")));
				break;
			}
			if(tagID == SMILTAG_BODY + SMILTAG_END)
			{
				// 当前是</BODY>标签
				break; // BODY 的内容已经结束
			}
			if(tagID == SMILTAG_PAR)
			{
				// 当前是<PAR>标签
				lpPar = ParsePar(lppPtr); // 分析一个幻灯片
				if (lpPar != NULL) 
				{
					// 将lpPar 插入到lpBody中
					if (lpBody->lpPar)
					{
						LPPAR lpCurrent; // 存放PAR的信息
						lpCurrent = lpBody->lpPar; // 得到第一个幻灯片
						while(lpCurrent) // 当前幻灯片有效
						{
							if (lpCurrent->next == NULL)
							{
								lpCurrent->next = lpPar; // 已经没有幻灯片，将当前分析得到的幻灯片插入到后面
								break;
							}
							else
							{
								lpCurrent = lpCurrent->next; // 指向下一个幻灯片
							}
						}
					}
					else
					{
						lpBody->lpPar = lpPar;
					}
				}
			}
			SMIL_ToNextTag(lppPtr);
		}

		RETAILMSG(1,(TEXT("-ParseBody\r\n")));
		return lpBody;
}


// **************************************************
// 声明：static LPLAYOUT ParseLayout(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的LAYOUT指针
// 功能描述 分析<LAYOUT>的内容。
// 引用: 
// **************************************************
static LPLAYOUT ParseLayout(LPTSTR *lppPtr)
{
	LPLAYOUT lpLayout; // 保存LAYOUT信息
	LPREGION lpRegion; // 保存REGION信息
	LPROOTLAYOUT lpRootLayout; // 保存ROOT-LAYOUT信息
	TAGID tagID; // 当前标签的标号
	
		RETAILMSG(1,(TEXT("+ParseLayout\r\n")));
		lpLayout = (LPLAYOUT)malloc(sizeof(LAYOUT)); //  分配LAYOUT结构指针
		if (lpLayout == NULL)
		{
			// 分配内存失败
			RETAILMSG(1,(TEXT("!!!ERROR MALLOC FAILURE IN [ParseLayout]\r\n")));
			return NULL;
		}
		// 初始化lpLayout结构
		lpLayout->lpRegion = NULL;

		// 开始分析LAYOUT内容
		// LAYOUT 的内容包括 REGION ， ROOT-LAYOUT
		while(1)
		{
			tagID=SMIL_LocateTag(lppPtr);  // 得到标签标号

			if(tagID == SMILTAG_NULL)
			{
				// 已经没有内容了
				RETAILMSG(1,(TEXT("!!!ERROR IN <LAYOUT> TAG ,CAN'T OCCUR NULL TAG\r\n")));
				break;
			}
			if(tagID == SMILTAG_LAYOUT + SMILTAG_END)
			{
				// 当前是</LAYOUT>标签
				break; // LAYOUT 的内容已经结束
			}
			else if(tagID == SMILTAG_ROOTLAYOUT)
			{
				// 当前是<ROOT-LAYOUT>标签
				lpRootLayout = ParseRootLayout(lppPtr); // 分析ROOT-LAYOUT的内容

				// 将lpRegion 插入到lpLayout的第一个
				lpLayout->lpRootLayout = lpRootLayout;
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置
			}
			else if(tagID == SMILTAG_REGION)
			{
				// 当前是<REGION>标签
				lpRegion = ParseRegion(lppPtr); // 分析REGION的内容

				// 将lpRegion 插入到lpLayout的第一个
				lpRegion->next = lpLayout->lpRegion;
				lpLayout->lpRegion = lpRegion;
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置
			}
			else
			{ // 当前标签是无效标签
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置
			}
		}

		RETAILMSG(1,(TEXT("-ParseLayout\r\n")));
		return lpLayout;
}

// **************************************************
// 声明：static LPROOTLAYOUT ParseRootLayout(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的ROOT-LAYOUT指针
// 功能描述 分析ROOT-LAYOUT的内容。
// 引用: 
// **************************************************
static LPROOTLAYOUT ParseRootLayout(LPTSTR *lppPtr)
{
	LPROOTLAYOUT lpRootLayout;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseROOT-LAYOUT\r\n")));
		lpRootLayout = (LPROOTLAYOUT)malloc(sizeof(ROOTLAYOUT)); // 分配REGION结构的内存
		if (lpRootLayout == NULL)
		{
			// 分配内存失败
			return NULL;
		}
		// 初始化LPROOTLAYOUT 结构
		lpRootLayout->width = SMIL_DEFAILTWIDTH; // 当前SMIL的宽度，以点为单位
		lpRootLayout->height = SMIL_DEFAILTHEIGHT; // 当前SMIL的高度，以点为单位、

		// 开始分析 ROOT-LAYOUT 的 ATTRIBUTE
		// ROOT-LAYOUT 的 ATTRIBUTE 包括 width,height
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // 得到标签的一个属性
			if (idAttr == 0)
			{ // 标签结束
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_WIDTH:
					RETAILMSG(1,(TEXT("current ATTR is \"WIDTH\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRootLayout->width = GetPixels(lpAttrContent);
					break;
				case SMILATTR_HEIGHT:
					RETAILMSG(1,(TEXT("current ATTR is \"HEIGHT\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRootLayout->height = GetPixels(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseROOT-LAYOUT\r\n")));
		return lpRootLayout;
}

// **************************************************
// 声明：static LPREGION ParseRegion(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的REGION指针
// 功能描述 分析REGION的内容。
// 引用: 
// **************************************************
static LPREGION ParseRegion(LPTSTR *lppPtr)
{
	LPREGION lpRegion;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseRegion\r\n")));
		lpRegion = (LPREGION)malloc(sizeof(REGION)); // 分配REGION结构的内存
		if (lpRegion == NULL)
		{
			// 分配内存失败
			return NULL;
		}
		// 初始化REGION 结构
		lpRegion->id = NULL;
		lpRegion->top = 0; // 当前region离最顶部的距离，以点为单位
		lpRegion->left = 0; // 当前region离最左边的距离，以点为单位
		lpRegion->width = 0; // 当前region的宽度，以点为单位
		lpRegion->height = 0; // 当前region 的高度，以点为单位、
		lpRegion->fit = 0;
		lpRegion->fill = 0;
		lpRegion->hidden = 0;
		lpRegion->meet = 0;
		lpRegion->slice = 0;
		lpRegion->next = NULL; 

		// 开始分析 LAYOUT 的 ATTRIBUTE
		// LAYOUT 的 ATTRIBUTE 包括 id,top,left,width,height,fit,fill,hidden, meet, slice
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // 得到标签的一个属性
			if (idAttr == 0)
			{ // 标签结束
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_ID:
					RETAILMSG(1,(TEXT("current ATTR is \"ID\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->id = SMILBufferAssign(lpAttrContent);
					break;
				case SMILATTR_TOP:
					RETAILMSG(1,(TEXT("current ATTR is \"TOP\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->top = GetPixels(lpAttrContent);
					break;
				case SMILATTR_LEFT:
					RETAILMSG(1,(TEXT("current ATTR is \"LEFT\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->left = GetPixels(lpAttrContent);
					break;
				case SMILATTR_WIDTH:
					RETAILMSG(1,(TEXT("current ATTR is \"WIDTH\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->width = GetPixels(lpAttrContent);
					break;
				case SMILATTR_HEIGHT:
					RETAILMSG(1,(TEXT("current ATTR is \"HEIGHT\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->height = GetPixels(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseRegion\r\n")));
		return lpRegion;

}



// **************************************************
// 声明：static LPPAR ParsePar(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的PAR指针
// 功能描述 分析<PAR>的内容。
// 引用: 
// **************************************************
static LPPAR ParsePar(LPTSTR *lppPtr)
{
	LPPAR lpPar; // 保存PAR信息
	TAGID tagID; // 当前标签的标号

	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];
	
		RETAILMSG(1,(TEXT("+ParsePar\r\n")));
		lpPar = (LPPAR)malloc(sizeof(PAR)); //  分配PAR结构指针
		if (lpPar == NULL)
		{
			// 分配内存失败
			RETAILMSG(1,(TEXT("!!!ERROR MALLOC FAILURE IN [ParsePar]\r\n")));
			return NULL;
		}
		// 初始化lpLayout结构
		lpPar->next = NULL;
		lpPar->lpImg = NULL; // 图象结构
		lpPar->lpAudio = NULL; // 声音结构
		lpPar->lpText = NULL; // 文本结构
		lpPar->lpRef = NULL; // REF结构

		lpPar->startTime = 0 ; // 进入当前幻灯片就开始播放
		lpPar->durTime = DEFAULT_DELAY ; // 默认每一幅幻灯片延迟3s
		lpPar->endTime = 0 ; // 默认幻灯结束后立即结束
//		lpPar->durTime = -1 ; // 延续时间无限长
//		lpPar->endTime = -1 ; // 延续时间无限长

		// 开始分析 PAR 的 ATTRIBUTE
		// PAR 的 ATTRIBUTE 包括 begin, end , dur

		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // 得到标签的一个属性
			if (idAttr == 0)
			{ // 标签结束
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_BEGIN:
					RETAILMSG(1,(TEXT("current ATTR is \"BEGIN\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpPar->startTime = GetDurTime(lpAttrContent);
					break;
				case SMILATTR_END:
					RETAILMSG(1,(TEXT("current ATTR is \"BEGIN\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpPar->endTime = GetDurTime(lpAttrContent);
					break;
				case SMILATTR_DUR:
					RETAILMSG(1,(TEXT("current ATTR is \"BEGIN\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpPar->durTime = GetDurTime(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}


		SMIL_ToNextTag(lppPtr); // 到下一个标签的位置

		// 开始分析PAR内容
		// PAR 的内容包括 img,audio,text,ref
		while(1)
		{
			tagID=SMIL_LocateTag(lppPtr);  // 得到标签标号

			if(tagID == SMILTAG_NULL)
			{
				// 已经没有内容了
				RETAILMSG(1,(TEXT("!!!ERROR IN <LAYOUT> TAG ,CAN'T OCCUR NULL TAG\r\n")));
				break;
			}
			if(tagID == SMILTAG_PAR + SMILTAG_END)
			{
				// 当前是</PAR>标签
				break; // PAR 的内容已经结束
			}
			else if(tagID == SMILTAG_IMG)
			{
				// 当前是<IMG>标签
				LPIMG lpImg;
				lpImg = ParseImg(lppPtr); // 分析IMG的内容

				// 将lpImg 插入到lpPar
				lpPar->lpImg = lpImg;
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置
			}
			else if(tagID == SMILTAG_AUDIO)
			{
				// 当前是<AUDIO>标签
				LPAUDIO lpAudio;
				lpAudio = ParseAudio(lppPtr); // 分析AUDIO的内容

				// 将lpAudio 插入到lpPar
				lpPar->lpAudio = lpAudio;
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置
			}
			else if(tagID == SMILTAG_TEXT)
			{
				// 当前是<TEXT>标签
				LPTEXT lpText;
				lpText = ParseText(lppPtr); // 分析TEXT的内容

				// 将lpText 插入到lpPar
				lpPar->lpText = lpText;
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置
			}
			else if(tagID == SMILTAG_REF)
			{
				// 当前是<REF>标签
				LPREF lpRef;
				lpRef = ParseRef(lppPtr); // 分析REF的内容

				// 将lpRef 插入到lpPar
				lpPar->lpRef = lpRef;
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置
			}
			else
			{ // 当前标签是无效标签
				SMIL_ToNextTag(lppPtr); // 到下一个标签的位置
			}
		}

		AdjustTheParTime(lpPar); // 调整幻灯片时间
		RETAILMSG(1,(TEXT("-ParsePar\r\n")));
		return lpPar;
}


// **************************************************
// 声明：static LPIMG ParseImg(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的IMG指针
// 功能描述 分析IMG的内容。
// 引用: 
// **************************************************
static LPIMG ParseImg(LPTSTR *lppPtr)
{
	LPIMG lpImg;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseImg\r\n")));
		lpImg = (LPIMG)malloc(sizeof(IMG)); // 分配IMG结构的内存
		if (lpImg == NULL)
		{
			// 分配内存失败
			return NULL;
		}
		// 初始化IMG 结构
		lpImg->lpRegion = NULL;
		lpImg->lpAlt = NULL;
		lpImg->lpSrc = NULL;
		lpImg->next = NULL;

		// 开始分析 IMG 的 ATTRIBUTE
		// IMG 的 ATTRIBUTE 包括 src, region, alt
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // 得到标签的一个属性
			if (idAttr == 0)
			{ // 标签结束
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_SRC:
					RETAILMSG(1,(TEXT("current ATTR is \"SRC\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpImg->lpSrc = SMILBufferAssign(lpAttrContent);
					break;
				case SMILATTR_REGION:
					RETAILMSG(1,(TEXT("current ATTR is \"REGION\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpImg->lpRegion = SMILBufferAssign(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseImg\r\n")));
		return lpImg;

}


// **************************************************
// 声明：static LPAUDIO ParseAudio(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的AUDIO指针
// 功能描述 分析AUDIO的内容。
// 引用: 
// **************************************************
static LPAUDIO ParseAudio(LPTSTR *lppPtr)
{
	LPAUDIO lpAudio;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseAudio\r\n")));
		lpAudio = (LPAUDIO)malloc(sizeof(AUDIO)); // 分配AUDIO结构的内存
		if (lpAudio == NULL)
		{
			// 分配内存失败
			return NULL;
		}
		// 初始化AUDIO 结构
		lpAudio->lpSrc = NULL;
		lpAudio->lpAlt = NULL;
		lpAudio->next = NULL;

		// 开始分析 AUDIO 的 ATTRIBUTE
		// AUDIO 的 ATTRIBUTE 包括 src, alt
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // 得到标签的一个属性
			if (idAttr == 0)
			{ // 标签结束
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_SRC:
					RETAILMSG(1,(TEXT("current ATTR is \"SRC\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpAudio->lpSrc = SMILBufferAssign(lpAttrContent);
					break;
				case SMILATTR_ALT:
					RETAILMSG(1,(TEXT("current ATTR is \"REGION\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
//					lpText->lpRegion = SMILBufferAssign(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseAudio\r\n")));
		return lpAudio;

}


// **************************************************
// 声明：static LPTEXT ParseText(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的TEXT指针
// 功能描述 分析TEXT的内容。
// 引用: 
// **************************************************
static LPTEXT ParseText(LPTSTR *lppPtr)
{
	LPTEXT lpText;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseText\r\n")));
		lpText = (LPTEXT)malloc(sizeof(TEXT)); // 分配TEXT结构的内存
		if (lpText == NULL)
		{
			// 分配内存失败
			return NULL;
		}
		// 初始化TEXT 结构
		lpText->lpAlt = NULL;
		lpText->lpRegion = NULL;
		lpText->lpSrc = NULL;
		lpText->next = NULL;

		// 开始分析 TEXT 的 ATTRIBUTE
		// TEXT 的 ATTRIBUTE 包括 src, region, alt
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // 得到标签的一个属性
			if (idAttr == 0)
			{ // 标签结束
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_SRC:
					RETAILMSG(1,(TEXT("current ATTR is \"SRC\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpText->lpSrc = SMILBufferAssign(lpAttrContent);
					break;
				case SMILATTR_REGION:
					RETAILMSG(1,(TEXT("current ATTR is \"REGION\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpText->lpRegion = SMILBufferAssign(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseText\r\n")));
		return lpText;

}


// **************************************************
// 声明：static LPREF ParseRef(LPTSTR *lppPtr)
// 参数：
// 	IN/OUT lppPtr -- 指向当前要分析的文本内容，并返回最新的指针
// 
// 返回值：返回分析后的REF指针
// 功能描述 分析REF的内容。
// 引用: 
// **************************************************
static LPREF ParseRef(LPTSTR *lppPtr)
{
	LPREF lpRef;

		RETAILMSG(1,(TEXT("+ParseRef\r\n")));
		lpRef = (LPREF)malloc(sizeof(REF)); // 分配REF结构的内存
		if (lpRef == NULL)
		{
			// 分配内存失败
			return NULL;
		}
		// 初始化REF 结构

		// 开始分析 REF 的 ATTRIBUTE
		// REF 的 ATTRIBUTE 包括 src, alt
		while(1)
		{
			break;
		}

		RETAILMSG(1,(TEXT("-ParseRef\r\n")));
		return lpRef;

}


// **************************************************
// 声明：static UINT GetPixels(LPTSTR lpAttrContent)
// 参数：
// 	IN lpAttrContent -- 包含当前点数的文本内容
// 
// 返回值：返回当前的点数
// 功能描述：得到当前的点数。
// 引用: 
// **************************************************
static UINT GetPixels(LPTSTR lpAttrContent)
{
	return atoi(lpAttrContent); // 将文本转化为数字
}

// **************************************************
// 声明：static UINT GetPixels(LPTSTR lpAttrContent)
// 参数：
// 	IN lpAttrContent -- 包含当前点数的文本内容
// 
// 返回值：返回当前要延续的时间
// 功能描述：得到当前要延续的时间。
// 引用: 
// **************************************************
static UINT GetDurTime(LPTSTR lpAttrContent)
{
	LPTSTR lpUnit;

		lpUnit = lpAttrContent; 
		while(1)
		{
			if (*lpUnit > '9' || *lpUnit < '0')
				break;
			lpUnit++;
		}
		if (strnicmp(lpUnit,"s",1) == 0)
		{
			// 单位是秒
			return atoi(lpAttrContent) * 1000; // 将文本转化为数字
		}
		else if (strnicmp(lpUnit,"ms",2) == 0)
		{
			// 单位是毫秒
			return atoi(lpAttrContent); // 将文本转化为数字
		}
		else
		{
			// 不认识的单位
			return DEFAULT_DELAY; // 返回默认值
		}
}


// **************************************************
// 声明：LPTSTR SMILBufferAssign(const LPTSTR pSrc)
// 参数：
// 	IN pSrc -- 要复制的文本串
// 
// 返回值：成功返回新创建的文本串，否则返回NULL
// 功能描述：创建一个与原来文本一样大的新的文本串并将原来的文本复制到新串。
// 引用: 
// **************************************************
LPTSTR SMILBufferAssign(const LPTSTR pSrc)
{
	LPTSTR ptr;
	DWORD dwLen;

		if (pSrc==NULL)
			return NULL;
		dwLen=strlen(pSrc)+1;
		ptr=(TCHAR *)malloc(dwLen*sizeof(TCHAR));
		if (ptr==NULL)
		{
			MessageBox(NULL,TEXT("The memory is not enough"),TEXT("memory alloc failure"),MB_OK);
			return NULL;
		}
		strcpy(ptr,pSrc);
		return ptr;
}


// **************************************************
// 声明：static void AdjustTheParTime(LPPAR lpPar)
// 参数：
// 	IN lpPar -- 当前的幻灯片数据
// 
// 返回值：无
// 功能描述：调整当前幻灯片的时间。
// 引用: 
// **************************************************
static void AdjustTheParTime(LPPAR lpPar)
{
	// 注意： 1。endTime 必须大于 startTime
	//		  2。endTime 必须 >= startTime + durTime

	if (lpPar->durTime == -1 && lpPar->endTime == -1)
	{ // 当前幻灯将一直延续下去
		return ; 
	}
	if (lpPar->durTime == -1)
	{
		// 延续时间错误,需要调整
		if (lpPar->endTime < lpPar->startTime)
		{
			// 结束时间错误,调整结束时间
			lpPar->endTime = lpPar->startTime;
		}
		// 调整延续时间
		lpPar->durTime = lpPar->endTime - lpPar->startTime;

		// 调整成功
		return ;
	}

	if (lpPar->endTime < lpPar->durTime + lpPar->startTime)
	{ // 结束时间错误,调整结束时间
		lpPar->endTime = lpPar->durTime + lpPar->startTime;
	}

	return ;
}
