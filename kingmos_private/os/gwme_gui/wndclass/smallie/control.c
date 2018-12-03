/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：小屏幕模式的IE浏览类处理控制部分
版本号：1.0.0.456
开发时期：2001-02-23
作者：陈建明 JAMI
修改记录：
	2004.05.26 添加显示GIF文件动画的部分
**************************************************/
#include "eHtmView.h"
//#include "stdafx.h"
#include "Control.h"
//#include "commctrl.h"
#include "viewdef.h"

//#define INPUTCONTROL

// **************************************************
// 函数声明区
// **************************************************
//extern WORD iPage,iMove;
// !!!Modified By Jami chen for WCE
// char * BufferAssign(const char *pSrc);
// !!!!
//TCHAR* SMIE_BufferAssign(const char *pSrc);  // malloc and copy context form char to TChar
//TCHAR* SMIE_BufferAssignTChar(TCHAR *pSrc);  // malloc and copy context form TChar to TChar
size_t StrLen( TCHAR *string );
// !!! Modified End
DWORD SMIE_HexToDword(char *ptr);
COLOR SMIE_GetColor(char *lpColor);

extern BOOL DownLoadImage(HWND hWnd,HHTMCTL hHtmlCtl,LPTSTR lpImage);
extern BOOL InsertAnimationList(HWND hWnd,HHTMCTL lpHtmlControl);
extern BOOL ShowImageControl(HWND hWnd,HHTMCTL hControl);

static void SMIE_ProcessTAGID_A(char **stream,LPCONTROLSTATE lpControlState);
static void SMIE_ProcessTAGID_A_End(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_TEXT(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_IMG(HWND hWnd,char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_FORM(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_OPTION(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_SELECT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_INPUT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_SCRIPT(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_STYLE(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_PRE(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL SMIE_ProcessTAGID_PREEND(char **stream,LPCONTROLSTATE lpControlState);
static void SMIE_ProcessTAGID_FONT(char **stream,LPCONTROLSTATE lpControlState);
static void SMIE_ProcessTAGID_FONT_End(char **stream,LPCONTROLSTATE lpControlState);
static void SMIE_ShowText(HWND hWnd,HDC hdc,int x,int y,TCHAR *content,WORD cbName,RECT rect,DWORD state);

//static HGIF SMIE_DrawImage(HWND hWnd,HDC hdc,LPTSTR lpImage,int x0,int y0,int width,int height);
//static BOOL SMIE_DrawImageByHandle(HWND hWnd,HDC hdc,HGIF hGif,int x0,int y0,int width,int height);
static HGIF SMIE_GetImageSize(HWND hWnd,LPTSTR image,LPRECT lprect);
static BOOL SMIE_GetImageSizeByHandle(HWND hWnd,HGIF hGif,LPRECT lprect);

static BOOL SMIE_IsAlpha(TCHAR c);

//#ifndef __WCE_DEFINE
static void SMIE_GetTextLenInPoint(HDC hdc,
							  LPTSTR lpszStr,
							  int cchString,
							  int MaxExtent,
							  LPINT lpnFit,
							  LPSIZE lpSize);
static int SMIE_GetCharWidthEx(HDC hdc,LPTSTR lpszStr,int *lpcchChar,int iFitWidth,int *lpchFit);
//#endif
// **************************************************
// 定义区域
// **************************************************
struct ColorTable{
	char *colorName;
	COLOR colorValue;
}SMIE_DefaultColorTable[16]={
	{"black",	0x000000},  // 黑色
	{"green",	0x008000},  //绿色
	{"silver",	0xc0c0c0},  //银白色
	{"lime",	0x00ff00},	//灰白色
	{"gray",	0x808080},	//灰色
	{"olive",	0x808000},	//茶青色
	{"white",	0xffffff},	//白色
	{"yellow",	0xffff00},	//黄色
	{"maroon",	0x800000},	//栗色
	{"navy",	0x000080},	//藏青色
	{"red",		0xff0000},	//红色
	{"blue",	0x0000ff},	//兰色
	{"purple",	0x800080},	//紫色
	{"teal",	0x008080},	//深灰色
	{"fuchsia",	0xff00ff},	//紫红色
	{"aqua",	0x00ffff},  //水绿色
};
// !!!! Delete By Jami chen for WCE
/*
HHTMCTL CreateHtmlControl(	POINT point,
						  NAME  name,
						  IMAGE image,	
						  URL   url,
						  COLOR color)
{

	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));
		if (lpHtmlControl==NULL)
			return NULL;
		memset(lpHtmlControl,0,sizeof(HTMLCONTROL));
		lpHtmlControl->x=point.x;
		lpHtmlControl->x=point.y;
		lpHtmlControl->content=BufferAssign(name);
		lpHtmlControl->image=BufferAssign(image);
		lpHtmlControl->url=BufferAssign(url);
		lpHtmlControl->color=color;
		lpHtmlControl->widthFirstRow=0;
		lpHtmlControl->widthLastRow=0;
		lpHtmlControl->heightLine=0;
		lpHtmlControl->type =CONTROLTYPE_TEXT;
		return lpHtmlControl;
}
*/
// !!!! Delete By Jami chen for WCE end
TCHAR* BufferAssign(const char *pSrc)
{
	TCHAR *ptr;
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
#ifdef KINGMOS_UNICODE
		//strcpy(ptr,pSrc);
		mbstowcs( ptr, pSrc, dwLen);
#else
		strcpy(ptr,pSrc);
#endif
		return ptr;
}
TCHAR* BufferAssignTChar(TCHAR *pSrc)
{
	TCHAR *ptr;
	DWORD dwLen;

		if (pSrc==NULL)
			return NULL;
		dwLen=StrLen(pSrc)+1;
		ptr=(TCHAR *)malloc(dwLen*sizeof(TCHAR));
		if (ptr==NULL)
		{
			MessageBox(NULL,TEXT("The memory is not enough"),TEXT("memory alloc failure"),MB_OK);
			return NULL;
		}
#ifdef KINGMOS_UNICODE
		wcscpy(ptr,pSrc);
#else
		strcpy(ptr,pSrc);
#endif
		return ptr;
}
// !!!! Modified end
// **************************************************
// 声明：COLOR SMIE_GetColor(char *lpColor)
// 参数：
// 	IN  lpColor -- 指定的颜色字串
// 
// 返回值：返回一个RGB的颜色值
// 功能描述：得到一个RGB的颜色值
// 引用: 
// **************************************************
COLOR SMIE_GetColor(char *lpColor)
{
  DWORD colorValue=0;
	if (*lpColor=='#')
	{  // 是一个立即数的颜色值
		colorValue=SMIE_HexToDword(lpColor+1);
	}
	else
	{  // 是一个颜色标号
		int i;
		for (i=0;i<16;i++)
		{
			if (stricmp(lpColor,SMIE_DefaultColorTable[i].colorName)==0)  // 比较颜色表
				colorValue = SMIE_DefaultColorTable[i].colorValue;
		}
	}
	return RGB(((colorValue&0xff0000)>>16),((colorValue&0xff00)>>8),(colorValue&0xff));  // 返回RGB颜色
}
// **************************************************
// 声明：DWORD SMIE_HexToDword(char *ptr)
// 参数：
// 	IN  ptr -- 一个16进制的数字串
// 
// 返回值：返回一个DWORD
// 功能描述：根据字串转化为数字
// 引用: 
// **************************************************
DWORD SMIE_HexToDword(char *ptr)
{
	DWORD dwValue=0;
	while(*ptr)
	{
		switch(*ptr)
		{
			case '0':  // 0
				dwValue*=16;
				dwValue+=0;
				break;
			case '1':  // 1
				dwValue*=16;
				dwValue+=1;
				break;
			case '2':  // 2
				dwValue*=16;
				dwValue+=2;
				break;
			case '3':  // 3
				dwValue*=16;
				dwValue+=3;
				break;
			case '4':  // 4
				dwValue*=16;
				dwValue+=4;
				break;
			case '5':  // 5
				dwValue*=16;
				dwValue+=5;
				break;
			case '6':  // 6
				dwValue*=16;
				dwValue+=6;
				break;
			case '7':  // 7
				dwValue*=16;
				dwValue+=7;
				break;
			case '8':  // 8
				dwValue*=16;
				dwValue+=8;
				break;
			case '9':  // 9
				dwValue*=16;
				dwValue+=9;
				break;
			case 'a':  // a , A
			case 'A':
				dwValue*=16;
				dwValue+=10;
				break;
			case 'b':  // b, B
			case 'B':
				dwValue*=16;
				dwValue+=11;
				break;
			case 'c':  // c, C
			case 'C':
				dwValue*=16;
				dwValue+=12;
				break;
			case 'd':  // d, D
			case 'D':
				dwValue*=16;
				dwValue+=13;
				break;
			case 'e':  // e, E
			case 'E':
				dwValue*=16;
				dwValue+=14;
				break;
			case 'f':  // f, F
			case 'F':
				dwValue*=16;
				dwValue+=15;
				break;
			default:  // 不是数字，返回
				return dwValue;
		}
		ptr++;
	}
	return dwValue;
}
// **************************************************
// 声明：void SMIE_ShowHtmlControl(HWND hWnd,HDC hdc,HHTMCTL hHtmlCtl,RECT rect,POINT ptOrg)
// 参数：
// 	IN hWnd  -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN hHtmlCtl -- 控制句柄
// 	IN rect -- 控制矩形
// 	IN ptOrg -- 显示原点
// 
// 返回值：无
// 功能描述：显示一个控制
// 引用: 
// **************************************************
void SMIE_ShowHtmlControl(HWND hWnd,HDC hdc,HHTMCTL hHtmlCtl,RECT rect,POINT ptOrg)
{
	LPHTMLCONTROL lpHtmlControl;
//	RECT rectInter;
	DWORD state=0;

	LPHTMLVIEW lpHtmlView;



		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return ;
//		if (IntersectRect(&rectInter,&rect,&rcPaint)==0)
//			return ; // the two rect is not intersection
		lpHtmlControl=(LPHTMLCONTROL)hHtmlCtl;  // 得到控制结构

		if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
		{
			// 不是文本
			return;
		}
		{
			// The Control is or Not in the Show Area,if not ,then return;
			RECT rect;
								
				GetClientRect(hWnd,&rect);  // 得到窗口矩形
				if ((lpHtmlControl->y>(DWORD)(ptOrg.y+rect.bottom))||((lpHtmlControl->y+lpHtmlControl->height)<(DWORD)(rect.top+ptOrg.y)))
				{  // 不在窗口矩形范围之内，不需要显示
					return;
				}
		}

		if (lpHtmlControl->image)
		{
			//ShowImage
/*			Rectangle(hdc,lpHtmlControl->x-ptOrg.x,
						  lpHtmlControl->y-ptOrg.y,
						  lpHtmlControl->x-ptOrg.x+lpHtmlControl->width,
						  lpHtmlControl->y-ptOrg.y+lpHtmlControl->height);
*/
/*
			if (lpHtmlControl->hGif==NULL)
			{  // 还没有加载图象
				lpHtmlControl->hGif=SMIE_DrawImage(hWnd,  // 加载并显示图象
						   hdc,
						   lpHtmlControl->image,
						   lpHtmlControl->x-ptOrg.x,
						   lpHtmlControl->y-ptOrg.y,
						   lpHtmlControl->width,
						   lpHtmlControl->height
						   );
			}
			else
			{// 显示图象
				SMIE_DrawImageByHandle(hWnd,
						   hdc,
						   lpHtmlControl->hGif,
						   lpHtmlControl->x-ptOrg.x,
						   lpHtmlControl->y-ptOrg.y,
						   lpHtmlControl->width,
						   lpHtmlControl->height
						   );
			}
*/
//			if (lpHtmlView->iDisplayMode == DISPLAY_TEXTIMAGE)
			if (lpHtmlView->iDisplayMode == DISPLAY_TEXTIMAGE || lpHtmlView->iFileStyle == IMAGE_FILE)
			{ // 需要显示图象
				ShowImageControl(hWnd,lpHtmlControl);
			}
		}
		else if (lpHtmlControl->content)
		{  // 是文本
			// 设置文本颜色
			if (lpHtmlControl->color==GetBkColor(hdc))  // if the BackGroung Color is Same the foreground color
				SetTextColor(hdc,RGB(128,128,128));     //  then select other color to foregroung color
			else
				SetTextColor(hdc,lpHtmlControl->color);  
			
			if (lpHtmlControl->url)
				state|=SHTM_EXISTURL;  // 显示时显示有URL联接的标志
			SMIE_ShowText(hWnd,hdc,(int)(lpHtmlControl->x-ptOrg.x),(int)(lpHtmlControl->y-ptOrg.y),lpHtmlControl->content,(WORD)StrLen(lpHtmlControl->content),rect,state);  // 显示指定文本
		}
}

// **************************************************
// 声明：LPHTMLOBJECT SMIE_ParseHtmlControl(HWND hWnd,TAGID tagID,char **stream,	LPCONTROLSTATE lpControlState)
// 参数：
// 	IN hWnd  -- 窗口句柄
// 	IN tagID -- 标号
// 	IN stream -- 数据流
// 	IN lpControlState -- 指向CONTROLSTATE的指针
// 
// 返回值：返回得到目标的指针。
// 功能描述：分析控件。
// 引用: 
// **************************************************
LPHTMLOBJECT SMIE_ParseHtmlControl(HWND hWnd,TAGID tagID,char **stream,	LPCONTROLSTATE lpControlState)
{
	LPHTMLOBJECT lpObject=NULL;
	HHTMCTL hHtmlCtl;
	LPHTMLVIEW lpHtmlView;


	JAMIMSG(DBG_FUNCTION,(TEXT("Enter Parse Html Control\r\n")));

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
	if (lpHtmlView==NULL)
		return FALSE;

	switch(tagID)
	{
	case TAGID_A:  // <A>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_A\r\n")));
		SMIE_ProcessTAGID_A(stream,lpControlState); // 处理<A>
		SMIE_ToNextTag(stream);  // 到下一个标号
		return NULL;  
	case TAGID_A+TAGID_END: // </A>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_A\r\n")));
		SMIE_ProcessTAGID_A_End(stream,lpControlState);  // 处理 </A>
		SMIE_ToNextTag(stream);  // 到下一个标号
		return NULL;  
	case TAGID_FONT: // <FONT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FONT\r\n")));  
		SMIE_ProcessTAGID_FONT(stream,lpControlState); // 处理 </FONT>
		SMIE_ToNextTag(stream);  // 到下一个标号
		return NULL;
	case TAGID_FONT+TAGID_END:  // </FONT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FONT\r\n")));
		SMIE_ProcessTAGID_FONT_End(stream,lpControlState);  // 处理 </FONT>
		SMIE_ToNextTag(stream); //到下一个标号
		return NULL;

	case TAGID_TEXT:  // 文本
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_TEXT\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_TEXT(stream,lpControlState);  // 处理文本
		break;
	case TAGID_IMG:  // <img>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_IMG\r\n")));
		if (lpHtmlView->iDisplayMode == DISPLAY_ONLYTEXT)
		{
			//只需要显示文本
			SMIE_ToNextTag(stream);  // 到下一个标号
			return NULL;
		}
		//需要图象，分析图象
		hHtmlCtl=SMIE_ProcessTAGID_IMG(hWnd,stream,lpControlState);  // 处理<img>
		SMIE_ToNextTag(stream);  // 到下一个标号
		break;
	case TAGID_BR:  // <BR>
	case TAGID_P:  // <P>
	case TAGID_LI:  // <LI>
		// 这里是需要一个回车
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_ENTER\r\n")));
		hHtmlCtl=NULL;
		SMIE_ToNextTag(stream);  // 到下一个标号
		lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));  // 分配内存
		if (lpObject==NULL)
			return NULL;  // 分配失败
		lpObject->lpContent=NULL;
		lpObject->type=TYPE_CR;  // 回车
		lpObject->next=NULL;
		return lpObject;
	case TAGID_FORM:  // <FORM>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FORM\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_FORM(stream,lpControlState);   // 处理<FORM>
		SMIE_ToNextTag(stream);// 到下一个标号
		break;
	case TAGID_OPTION:  // <FONT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_OPTION\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_OPTION(stream,lpControlState);  // 处理<FONT>
		SMIE_ToNextTag(stream); // 到下一个标号
		break;
	case TAGID_SELECT:  // <SELECT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_SELECT\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_SELECT(hWnd,stream,lpControlState); //处理<SELECT>
//		SMIE_ToNextTag(stream);
		break;
#ifdef INPUTCONTROL
	case TAGID_INPUT:  // <INPUT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_INPUT\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_INPUT(hWnd,stream,lpControlState);  // 处理<INPUT>
//		SMIE_ToNextTag(stream);
		break;
#endif
	case TAGID_SCRIPT:  // <SCRIPT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_SCRIPT\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_SCRIPT(stream,lpControlState);  // 处理<SCRIPT>
		SMIE_ToNextTag(stream);  // 到下一个标号
		break;
	case TAGID_STYLE:  // <STYLE>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_STYLE\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_STYLE(stream,lpControlState);  // 处理<STYLE>
		SMIE_ToNextTag(stream);  // 到下一个标号
		break;
	case TAGID_PRE:  // <PRE>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_PRE\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_PRE(stream,lpControlState);  // 处理<PRE>
		SMIE_ToNextTag(stream); // 到下一个标号
		break;
	case TAGID_PRE+TAGID_END:  // </PRE>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_PRE+TAGID_END\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_PREEND(stream,lpControlState);  // 处理</PRE>
		SMIE_ToNextTag(stream); // 到下一个标号
		break;
	default:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter Default\r\n")));
		SMIE_ToNextTag(stream);  // 到下一个标号
		return NULL;
	}
	if (hHtmlCtl)
	{  // 成功得到一个控制
		lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT)); // 分配一个目标
		if (lpObject==NULL)
			return NULL; // 分配失败
		lpObject->lpContent=hHtmlCtl;  // 给目标内容赋控制句柄
		lpObject->type=TYPE_CONTROL;  // 目标是一个控制
		lpObject->next=NULL;
		return lpObject;
	}
	return NULL;  // 失败返回
}
// **************************************************
// 声明：static void SMIE_ProcessTAGID_A(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：无
// 功能描述：分析处理标号<A>的数据
// 引用: 
// **************************************************
static void SMIE_ProcessTAGID_A(char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pPropertyContent;

	pPropertyContent=(char *)malloc(1024);  // 分配属性内容的缓存
	if (pPropertyContent==NULL)
		return;  // 分配失败
	while(1)
	{
		propertyID=SMIE_LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
		{
			break;
		}
		SMIE_LocatePropertyContent(stream,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_HREF:  // Process property "href"
			if (lpControlState->url)  // if current url is exist ,then free the old memory
				free(lpControlState->url);
//			lpControlState->url=SMIE_BufferAssign(pPropertyContent); //malloc a memory and copy the content
			lpControlState->url=BufferAssign(pPropertyContent); //malloc a memory and copy the content
																//to the memory ,then to the current url			break;
			break;
		case PROPERTYID_NAME:  // Process property "name"
			if (lpControlState->name)  // if current url is exist ,then free the old memory
				free(lpControlState->name);
//			lpControlState->name=SMIE_BufferAssign(pPropertyContent); //malloc a memory and copy the content
			lpControlState->name=BufferAssign(pPropertyContent); //malloc a memory and copy the content
																//to the memory ,then to the current url			break;
			break;
		}
	}
	if (pPropertyContent)
		free(pPropertyContent);  // 释放不再需要的缓存
}

// **************************************************
// 声明：static void SMIE_ProcessTAGID_A_End(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：无
// 功能描述：分析处理标号</A>
// 引用: 
// **************************************************
static void SMIE_ProcessTAGID_A_End(char **stream,LPCONTROLSTATE lpControlState)
{
	if (lpControlState->url)  // if current url is exist ,then free the old memory
		free(lpControlState->url);
	lpControlState->url=NULL;
	if (lpControlState->name)  // if current url is exist ,then free the old memory
		free(lpControlState->name);
	lpControlState->name=NULL;
	return ;
}


// **************************************************
// 声明：static void SMIE_ProcessTAGID_FONT(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：无
// 功能描述：分析处理标号<FONT>
// 引用: 
// **************************************************
static void SMIE_ProcessTAGID_FONT(char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pPropertyContent;

	pPropertyContent=(char *)malloc(1024);  // 分配内存
	if (pPropertyContent==NULL)
		return ; // 分配失败
	while(1)
	{
		propertyID=SMIE_LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		SMIE_LocatePropertyContent(stream,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_COLOR:  // Process property "color"
			lpControlState->color=SMIE_GetColor(pPropertyContent);
		}
	}
	
	if (pPropertyContent)
		free(pPropertyContent);  // 释放不需要的内存
}

// **************************************************
// 声明：static void SMIE_ProcessTAGID_FONT_End(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：无
// 功能描述：分析处理标号</FONT>
// 引用: 
// **************************************************
static void SMIE_ProcessTAGID_FONT_End(char **stream,LPCONTROLSTATE lpControlState)
{
	lpControlState->color=CL_BLACK;  // 恢复到默认颜色
	return ;
}

// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_TEXT(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理文本数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_TEXT(char **stream,LPCONTROLSTATE lpControlState)
{
//	PROPERTYID propertyID;
	char *pText=NULL;
	LPHTMLCONTROL lpHtmlControl;
	DWORD sizeText;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ProcessTAGID_TEXT\r\n")));

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));  // 分配一个控制
	if (lpHtmlControl==NULL)
		return NULL;  // 分配失败

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // 初始化结构

	JAMIMSG(DBG_PROCTEXT,(TEXT("GetTagContentLen\r\n")));
	sizeText=SMIE_GetTagContentLen(*stream,lpControlState->bPreProcess);  // 得到标签内容长度

	pText=(char *)malloc(sizeText);  // 分配内容的缓存
	if (pText==NULL)
		return NULL;

	JAMIMSG(DBG_PROCTEXT,(TEXT("GetTagContent\r\n")));
	SMIE_LocateTagContent(stream,pText,sizeText,lpControlState->bPreProcess);  // Get text

	// 得到控制的位置
	lpHtmlControl->x=lpControlState->x;
	lpHtmlControl->x=lpControlState->y;
//	lpHtmlControl->content=SMIE_BufferAssign(pText);
	lpHtmlControl->content=BufferAssign(pText);  // 复制文本内容
	lpHtmlControl->width=0;  // the width of this control
	lpHtmlControl->height=0;  // the height of this control
	lpHtmlControl->image=NULL;
//	lpHtmlControl->hGif=NULL;
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->url=SMIE_BufferAssign(lpControlState->url);
// !!!
//	lpHtmlControl->url=SMIE_BufferAssignTChar(lpControlState->url);
	lpHtmlControl->url=BufferAssignTChar(lpControlState->url);  // 复制指向的URL
//	lpHtmlControl->name=SMIE_BufferAssignTChar(lpControlState->name);
	lpHtmlControl->name=BufferAssignTChar(lpControlState->name); // 复制标号
// !!! Modified End
	lpHtmlControl->color=lpControlState->color;  // 得到颜色
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->font=SMIE_BufferAssign(lpControlState->font);
// !!!
//	lpHtmlControl->font=SMIE_BufferAssignTChar(lpControlState->font);
	lpHtmlControl->font=BufferAssignTChar(lpControlState->font);  // 复制字体
// !!! Modified End
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->heightLine=0;
	lpHtmlControl->type =CONTROLTYPE_TEXT;

	JAMIMSG(DBG_PROCTEXT,(TEXT("Free Memory\r\n")));
	if(pText)
		free(pText);  // 释放不需要的内存
	JAMIMSG(DBG_PROCTEXT,(TEXT("Process Text Success\r\n")));
	return lpHtmlControl;  // 返回控制句柄
}

// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_IMG(HWND hWnd,char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理<img>数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_IMG(HWND hWnd,char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pText;
	LPHTMLCONTROL lpHtmlControl;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));  // 分配一个控制
	if (lpHtmlControl==NULL)
		return NULL;  // 分配失败

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // 初始化结构

	pText=(char *)malloc(1024);  // 分配一个文本内存
	if (pText==NULL)
	{  // 分配失败
		free(lpHtmlControl);
		return NULL;
	}
	lpHtmlControl->x=lpControlState->x;
	lpHtmlControl->x=lpControlState->y;
	lpHtmlControl->width=0;  // the width of this control
	lpHtmlControl->height=0;  // the height of this control
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->url=SMIE_BufferAssign(lpControlState->url);
// !!!
//	lpHtmlControl->url=SMIE_BufferAssignTChar(lpControlState->url);
	lpHtmlControl->url=BufferAssignTChar(lpControlState->url);  // 复制URL
//	lpHtmlControl->name=SMIE_BufferAssignTChar(lpControlState->name);
	lpHtmlControl->name=BufferAssignTChar(lpControlState->name); // 复制名称
// !!! Modified End
	lpHtmlControl->color=lpControlState->color;  // 得到颜色
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->font=SMIE_BufferAssign(lpControlState->font);
// !!!
//	lpHtmlControl->font=SMIE_BufferAssignTChar(lpControlState->font);
	lpHtmlControl->font=BufferAssignTChar(lpControlState->font);  // 复制字体
// !!! Modified End
	lpHtmlControl->content=NULL;
	lpHtmlControl->image=NULL;
//	lpHtmlControl->hGif=NULL;
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->heightLine=0;
	lpHtmlControl->type =CONTROLTYPE_TEXT;

	InsertAnimationList(hWnd,lpHtmlControl);

	while(1)
	{
		propertyID=SMIE_LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		SMIE_LocatePropertyContent(stream,pText,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_SRC:  // Process property "src"
//			lpHtmlControl->image=SMIE_BufferAssign(pText); //malloc a memory and copy the content
			lpHtmlControl->image=BufferAssign(pText); //malloc a memory and copy the content
																//to the memory ,then to the current image
			DownLoadImage(hWnd,lpHtmlControl,lpHtmlControl->image);  // 下载指定图象
			break;
		case PROPERTYID_ALT:  // Process property "alt"
//			lpHtmlControl->content=SMIE_BufferAssign(pText); //malloc a memory and copy the content
			lpHtmlControl->content=BufferAssign(pText); //malloc a memory and copy the content
																//to the memory ,then to the current name
			break;
		case PROPERTYID_WIDTH:  // Process property "width"
			lpHtmlControl->width=atoi(pText);//GetWidth(pText);
			lpHtmlControl->widthFirstRow=lpHtmlControl->width;
			lpHtmlControl->widthLastRow=lpHtmlControl->width;
			break;
		case PROPERTYID_HEIGHT:  // Process property "height"
			lpHtmlControl->height=atoi(pText);
			lpHtmlControl->heightLine=lpHtmlControl->height;
//			lpHtmlControl->heightLine=SMIE_GetFontHeight(hWnd,NULL);
//			if(lpHtmlControl->heightLine>lpHtmlControl->height)
//				lpHtmlControl->heightLine=lpHtmlControl->height;
			break;
		default:
			break;
		}
	}
	if (lpHtmlControl->width==0||lpHtmlControl->height==0)
	{  // No image size,  must read from the img file
/*
		HANDLE img_file;
		char lpFileContent[200];
		DWORD dwReadLen;

			img_file=CreateFile(lpHtmlControl->image,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
			if (img_file!=INVALID_HANDLE_VALUE)
			{
				ReadFile(img_file,lpFileContent,200,&dwReadLen,NULL);
				if (lpFileContent[0]=='G'&&lpFileContent[1]=='I'&&lpFileContent[2]=='F')
				{
					lpHtmlControl->width=*(unsigned short *)&lpFileContent[6];
					lpHtmlControl->height=*(unsigned short *)&lpFileContent[8];
				}
				CloseHandle(img_file);
			}
*/
/*		RECT rect={0,0,0,0};
			//// 得到图象的大小
			if (lpHtmlControl->hGif==NULL)
				lpHtmlControl->hGif=SMIE_GetImageSize(hWnd,lpHtmlControl->image,&rect);  
			else
				SMIE_GetImageSizeByHandle(hWnd,lpHtmlControl->hGif,&rect);
			lpHtmlControl->width=rect.right-rect.left;
			lpHtmlControl->height=rect.bottom-rect.top;		
*/
	}
	free(pText);  // 释放不需要的内存

	// Add By Jami chen in 2004.05.26 for gif Animation
	InsertAnimationList(hWnd,lpHtmlControl);
	return lpHtmlControl;  // 返回控制句柄
}


// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_FORM(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理<FORM>数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_FORM(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
	SMIE_ToNextTag(stream);  // 到下一个标号
/*
	// Get td Content
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);
		if (tagID==TAGID_FORM+TAGID_END)  // </FORM>
		{
			SMIE_ToNextTag(stream);
			return NULL;
		}
		SMIE_ToNextTag(stream);
	}
	*/
	return NULL;
}

// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_OPTION(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理<OPTION>数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_OPTION(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	SMIE_ToNextTag(stream);

	// 暂时不处理<option>及其包含的内容,直到〈/OPTION〉，
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // 得到标签标号
		if (tagID==TAGID_OPTION+TAGID_END)  // </OPTION>
		{
			SMIE_ToNextTag(stream);  // 到下一个标签
			return NULL;
		}
		SMIE_ToNextTag(stream);  // 到下一个标签
	}
	return NULL;
}
// **************************************************
// 声明：static void SMIE_GetOptionItem(char **stream,char *lpItem,WORD cbSize)
// 参数：
// 	IN/OUT stream -- 数据流
// 	OUT lpItem -- 得到条目内容的缓存
// 	IN cbSize -- 缓存大小
// 
// 返回值：无
// 功能描述：得到OPTION条目的内容
// 引用: 
// **************************************************
static void SMIE_GetOptionItem(char **stream,char *lpItem,WORD cbSize)
{
	TAGID tagID;
	SMIE_ToNextTag(stream);  // 到下一个标签标号

	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // 得到标签标号
		if (tagID==TAGID_OPTION+TAGID_END)  // </OPTION>
		{
			SMIE_ToNextTag(stream); // 到下一个标签标号
			return;
		}
		else if (tagID==TAGID_TEXT)
		{// 是文本
			SMIE_LocateTagContent(stream,lpItem,cbSize,FALSE);  // Get text
		}
		SMIE_ToNextTag(stream);  // 到下一个标签标号
	}
	return ;
}

// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_SELECT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN hParentWnd -- 窗口句柄
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理<SELECT>数据
// 引用: 
// **************************************************
#ifdef INPUTCONTROL
#define ITEMMAXLENGTH 128
static HHTMCTL SMIE_ProcessTAGID_SELECT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	char lpItem[ITEMMAXLENGTH];

	PROPERTYID propertyID;
	char *pText;
	LPHTMLCONTROL lpHtmlControl;
	HWND hWnd;
	HINSTANCE hInst;
	WORD cbItem;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));  // 分配一个控制
	if (lpHtmlControl==NULL)
		return NULL;  // 分配失败

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // 初始化结构

	pText=(char *)malloc(1024);  // 分配一个文本内存
	if (pText==NULL)
	{ // 分配失败
		free(lpHtmlControl);
		return NULL;
	}
	// 给结构赋值
	lpHtmlControl->x=lpControlState->x;
	lpHtmlControl->x=lpControlState->y;
	lpHtmlControl->width=0;  // the width of this control
	lpHtmlControl->url=NULL;
	lpHtmlControl->name=NULL;
	lpHtmlControl->color=0;
	lpHtmlControl->font=NULL;
	lpHtmlControl->content=NULL;
	lpHtmlControl->image=NULL;
	lpHtmlControl->hGif=NULL;
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->type =CONTROLTYPE_SELECT;

	lpHtmlControl->height=24;  // the height of this control
	lpHtmlControl->heightLine=24;

	while(1)
	{
		propertyID=SMIE_LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		SMIE_LocatePropertyContent(stream,pText,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_SIZE:  // Process property "width"
			lpHtmlControl->width=atoi(pText);//GetWidth(pText);
			break;
		default:
			break;
		}
	}
// !!! Modified By Jami chen for WCE
//	hInst=(HINSTANCE)GetWindowLong(hParentWnd,GWL_HINSTANCE);
//	hWnd=CreateWindow("ComboBox",NULL,WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,lpHtmlControl->x,lpHtmlControl->y,
//				lpHtmlControl->width,lpHtmlControl->height+90,hParentWnd,0,hInst,NULL);
// !!!!
	hInst = GetWindowLong(hParentWnd,GWL_HINSTANCE);  // 得到实例句柄
	if (lpHtmlControl->width==0)
		lpHtmlControl->width=10;
	// 创建一个COMBOBOX窗口
	hWnd=CreateWindow(TEXT("ComboBox"),NULL,WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,lpHtmlControl->x,lpHtmlControl->y,
				lpHtmlControl->width,lpHtmlControl->height+90,hParentWnd,0,hInst,NULL);
// !!! Modified By Jami chen for WCE End
//	hWnd=CreateWindow(WC_COMBOBOXEX,NULL,WS_VISIBLE|WS_CHILD|CBS_DROPDOWN,lpHtmlControl->x,lpHtmlControl->y,
//				lpHtmlControl->width,lpHtmlControl->height,hParentWnd,0,hInst,NULL);
//	hWnd=CreateWindow("COMBOBOX","Text",WS_VISIBLE|WS_CHILD|CBS_DROPDOWN,300,200,
//				300,120,hParentWnd,0,hInst,NULL);
	lpHtmlControl->content=(TCHAR *)hWnd;  // 将窗口句柄赋给控制
//	MoveWindow((HWND)lpHtmlControl->content,300,200,300,16,FALSE);
	SMIE_ToNextTag(stream);  // 到下一个标签

	// 得到SELECT的内容
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // 得到标签标号
		if (tagID==TAGID_SELECT+TAGID_END)  // </SELECT>
		{  // 结束
			lpHtmlControl->width+=24;
			lpHtmlControl->widthFirstRow=lpHtmlControl->width;
			// 设置窗口位置
			SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,lpHtmlControl->width,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
			SendMessage(hWnd,CB_SETCURSEL,0,0);// 设置当前选择条
			SMIE_ToNextTag(stream);  // 到下一个标签标号

			if (pText)
				free(pText);  // 释放不需要的内存
			return lpHtmlControl; // 返回控制句柄
		}
		else if (tagID==TAGID_OPTION)  // <OPTION>
		{   // 标签一个选项
		  TCHAR *lpItemContent;
			GetOptionItem(stream,lpItem,ITEMMAXLENGTH);  // 得到标签内容
			cbItem=strlen(lpItem); // 得到条目尺寸
			if (lpHtmlControl->width<cbItem*8)
				lpHtmlControl->width=cbItem*8; // 设置控制宽度
//			lpItemContent=SMIE_BufferAssign(lpItem);
			lpItemContent=BufferAssign(lpItem);  // 复制条目内容
			if (lpItemContent)
			{
				SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)lpItemContent);  // 添加一条内容到窗口
				free(lpItemContent);  // 释放条目内存
			}
		}
		SMIE_ToNextTag(stream);  // 到下一个标签
	}
	lpHtmlControl->width+=24;
	lpHtmlControl->widthFirstRow=lpHtmlControl->width;
	// 设置窗口位置
	SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,lpHtmlControl->width,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
	SendMessage(hWnd,CB_SETCURSEL,0,0);  // 设置当前选择条目
	if (pText)
		free(pText);  // 释放不需要的内存
	return lpHtmlControl; // 返回控制句柄
}
#else
#define ITEMMAXLENGTH 128
static HHTMCTL SMIE_ProcessTAGID_SELECT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	// 在这里不处理SELECT ，故不处理其及其以后的内容直到</SELECT>
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // 得到标签标号
		if (tagID==TAGID_SELECT+TAGID_END)  // </SELECT>
		{
			SMIE_ToNextTag(stream);  // 到下一个标号
			return NULL;
		}
		SMIE_ToNextTag(stream);  // 到下一个标号
	}
	return NULL;
}
#endif


#define INPUT_NULL		0
#define INPUT_TEXT		1
#define INPUT_BUTTON	2
#define INPUT_PASSWORD	3
#define INPUT_CHECK		4
#define INPUT_RADIO		5
#define INPUT_SUBMIT	6
#define INPUT_RESET		7

#define MAXTYPE  7

// **************************************************
// 声明：WORD SMIE_GetInputType(char *type)
// 参数：
// 	IN type -- 要得到输入类型的字串
// 
// 返回值：返回当前的输入类型
// 功能描述：得到指定字串的输入类型。
// 引用: 
// **************************************************
WORD SMIE_GetInputType(char *type)
{
	int i;
	struct{
		char *name;
		WORD type;
	}SupportType[]={
		{"text",INPUT_TEXT},
		{"button",INPUT_BUTTON},
		{"password",INPUT_PASSWORD},
		{"check box",INPUT_CHECK},
		{"radio",INPUT_RADIO},
		{"submit",INPUT_SUBMIT},
		{"reset",INPUT_RESET},
	};
	for (i=0;i<MAXTYPE;i++)
	{
		if (strcmp(type,SupportType[i].name)==0)
		{ // 比较匹配的输入类型
			return SupportType[i].type;
		}
	}
//	MessageBox(NULL,"UnKnow Input Type",type,MB_OK);
	return INPUT_NULL;  // 不认识的类型，返回失败
}


#ifdef INPUTCONTROL
// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_INPUT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 参数：
// 	IN hParentWnd -- 窗口句柄
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理<INPUT>数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_INPUT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;

	PROPERTYID propertyID;
	char *pText;
	LPHTMLCONTROL lpHtmlControl;
	HWND hWnd;
	HINSTANCE hInst;
	WORD cbItem;
	TCHAR *classInput=NULL;
	TCHAR *value=NULL;
	DWORD InputStyle=0;
	WORD InputType=INPUT_TEXT;
	WORD size=0,valueLen=0;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));  // 分配一个控制
	if (lpHtmlControl==NULL)
		return NULL;  // 分配失败

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // 初始化结构

	pText=(char *)malloc(1024);  // 分配一个文本
	if (pText==NULL)
	{ // 分配失败
		free(lpHtmlControl);
		return NULL;
	}
	// 给结构赋值
	lpHtmlControl->x=lpControlState->x;
	lpHtmlControl->x=lpControlState->y;
	lpHtmlControl->width=0;  // the width of this control
	lpHtmlControl->url=NULL;
	lpHtmlControl->name=NULL;
	lpHtmlControl->color=0;
	lpHtmlControl->font=NULL;
	lpHtmlControl->content=NULL;
	lpHtmlControl->image=NULL;
	lpHtmlControl->hGif=NULL;
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->type =CONTROLTYPE_INPUT;

	lpHtmlControl->height=24;  // the height of this control
	lpHtmlControl->heightLine=24;


	while(1)
	{
		propertyID=SMIE_LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		SMIE_LocatePropertyContent(stream,pText,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_SIZE:  // Process property "size"
			size=atoi(pText);//GetWidth(pText);
			break;
		case PROPERTYID_TYPE: // Process property "type"
			InputType=GetInputType(pText);  // 得到输入类型
			break;
		case PROPERTYID_VALUE:  // Process property "value"
//			value=SMIE_BufferAssign(pText);
			value=BufferAssign(pText);  // 得到输入类型的数据
			break;
		default:
			break;
		}
	}
	switch(InputType)
	{
	case INPUT_TEXT:  // 是一个文本输入
		classInput=TEXT("EDIT");  // 要创建一个编辑区
		InputStyle=WS_BORDER|WS_CHILD|WS_VISIBLE;  // 设置编辑区的风格
		valueLen=0;
		if (value)
			valueLen=StrLen(value);
		if (size<valueLen)
			size=valueLen;
		lpHtmlControl->height=20;  // 设置编辑区的大小
		lpHtmlControl->heightLine=20;
		lpHtmlControl->width=size*8;
		break;
	case INPUT_PASSWORD:  // 是一个密码输入窗口
		classInput=TEXT("EDIT");// 要创建一个编辑区
		InputStyle=WS_BORDER|WS_CHILD|WS_VISIBLE|ES_PASSWORD; // 设置编辑区的风格
		valueLen=0;
		if (value)
			valueLen=StrLen(value);
		if (size<valueLen)
			size=valueLen;
		lpHtmlControl->height=20;  // 设置编辑区的大小
		lpHtmlControl->heightLine=20;
		lpHtmlControl->width=size*8;
		break;
	case INPUT_CHECK:  // 是一个选择窗口
		classInput=TEXT("BUTTON"); // 要创建一个按钮
		InputStyle=WS_CHILD|WS_VISIBLE|BS_CHECKBOX;  // 设置按钮的风格
		lpHtmlControl->width=size;  //设置按钮的大小
		valueLen=0;
		if (value)
		{	
			free(value);
			value=NULL;
			lpHtmlControl->width=16;
//			valueLen=StrLen(value);
		}
//		if (lpHtmlControl->width<valueLen*8+8)
//			lpHtmlControl->width=valueLen*8+8;
		break;
	case INPUT_RADIO:  // 是一个单选框
		classInput=TEXT("BUTTON");// 要创建一个按钮
		InputStyle=WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON; // 设置按钮的风格
		lpHtmlControl->width=size; //设置按钮的大小
		valueLen=0;
		if (value)
		{
			free(value);
			value=NULL;
			lpHtmlControl->width=16;
		}
//			valueLen=StrLen(value);
//		if (lpHtmlControl->width<valueLen*8+8)
//			lpHtmlControl->width=valueLen*8+8;
		break;
	case INPUT_BUTTON:
	case INPUT_SUBMIT:
	case INPUT_RESET:
		// 一个一般的按键
		classInput=TEXT("BUTTON"); // 要创建一个按钮
		InputStyle=WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON; // 设置按钮的风格
		lpHtmlControl->width=size; //设置按钮的大小
		valueLen=0;
		if (value)
			valueLen=StrLen(value);
		if (lpHtmlControl->width<valueLen*8+8)
			lpHtmlControl->width=valueLen*8+8;
		break;
	case INPUT_NULL:
		classInput=NULL;
		break;
	}
	if (classInput)
	{
// !!! Modified By Jami chen for WCE
		hInst=(HINSTANCE)GetWindowLong(hParentWnd,GWL_HINSTANCE);  // 得到实例句柄
// !!!!
//		hInst = NULL;
// !!! Modified By Jami chen for WCE
		// 创建指定的窗口
		hWnd=CreateWindow(classInput,value,InputStyle,lpHtmlControl->x,lpHtmlControl->y,
					lpHtmlControl->width,lpHtmlControl->height,hParentWnd,0,hInst,NULL);

		lpHtmlControl->content=(TCHAR *)hWnd;  // 将窗口句柄设置给控制
	}
	if (value)  // 释放不需要的内存
		free(value);
	SMIE_ToNextTag(stream);  // 到下一个标签标号
	lpHtmlControl->widthFirstRow=lpHtmlControl->width;
	if (pText) // 释放不需要的内存
		free(pText);
	if (lpHtmlControl->content==NULL)
	{ // 内容失败，则控制失败
		free(lpHtmlControl);
		return NULL;
	}
	return lpHtmlControl;  // 返回控制句柄
}
#endif


// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_SCRIPT(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理<SCRIPT>数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_SCRIPT(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	SMIE_ToNextTag(stream);

	//现在没有处理SCRIPT，不处理他的内容直到</SCRIPT>
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // 得到标签标号
		if (tagID==TAGID_SCRIPT+TAGID_END)  // </SCRIPT>
		{
			SMIE_ToNextTag(stream);  // 到下一个标签标号
			return NULL;
		}
		SMIE_ToNextTag(stream);  // 到下一个标签标号
	}
	return NULL;
}

// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_STYLE(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理<STYLE>数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_STYLE(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	SMIE_ToNextTag(stream);

	// 暂时不处理STYLE，则不处理他的内容直到</STYLE>
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // 得到标签标号
		if (tagID==TAGID_STYLE+TAGID_END)  // </STYLE>
		{
			SMIE_ToNextTag(stream);  // 到下一个标签标号
			return NULL;
		}
		SMIE_ToNextTag(stream);  // 到下一个标签标号
	}
	return NULL;
}

// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_PRE(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理<PRE>数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_PRE(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
//	SMIE_ToNextTag(stream);

	// Get td Content
	lpControlState->bPreProcess=TRUE;  // 设置PRE标志
	return NULL;
}

// **************************************************
// 声明：static HHTMCTL SMIE_ProcessTAGID_PREEND(char **stream,LPCONTROLSTATE lpControlState)
// 参数：
// 	IN/OUT stream -- 数据流
// 	IN/OUT lpControlState -- 指向当前控制状态的指针
// 
// 返回值：返回一个控制句柄，失败返回0。
// 功能描述：分析处理</PRE>数据
// 引用: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_PREEND(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
//	SMIE_ToNextTag(stream);

	// Get td Content
	lpControlState->bPreProcess=FALSE;  // 清除PRE标志
	return NULL;
}

// **************************************************
// 声明：BOOL SMIE_ReCalcSize_Control(HWND hWnd,HDC hdc,HHTMCTL lpControl,DWORD iStart,RECT rect)  
// 参数：
//  IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpControl -- CONTROL句柄
// 	IN iStart -- 控制的开始位置
// 	IN rect -- 控制所在范围
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：重新计算控制的大小。
// 引用: 
// **************************************************
BOOL SMIE_ReCalcSize_Control(HWND hWnd,HDC hdc,HHTMCTL lpControl,DWORD iStart,RECT rect)  // Recalc the Control Size
{
	LPHTMLCONTROL lpHtmlControl;
	SIZE size={0,0};
//	RECT rect;
	TCHAR *content;
	WORD cbName,iHeightLine;
	int cbChar=0;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return FALSE;
		lpHtmlControl=(LPHTMLCONTROL)lpControl; // 得到控制结构指针

#ifdef INPUTCONTROL

		if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
		{  // 不是文本控制，是输入控制
			WORD newWidth;
			newWidth=lpHtmlControl->widthFirstRow;
			if (newWidth>(rect.right-rect.left-iStart))
			{// 重新得到宽度
				newWidth=(rect.right-rect.left-iStart);
			}
			lpHtmlControl->width=newWidth; // 重新设置宽度
			// 重设窗口
			if (lpHtmlControl->type==CONTROLTYPE_SELECT)
				SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,newWidth,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
			else
				SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,newWidth,lpHtmlControl->height,SWP_NOZORDER|SWP_NOMOVE);
			// 得到大小
			size.cx=newWidth;
			size.cy=newWidth;
	//		lpHtmlControl->widthFirstRow=newWidth;
			lpHtmlControl->widthLastRow=newWidth;
			return TRUE;  // 返回成功
		}

#endif

		if (lpHtmlControl->image)
		{  // 是图象
			// 如果得到的宽度为0，则默认大小为16*16
// !!! delete By Jami chen in 2004.09.20
			// 当图象的尺寸为0时，下载完该图象后会自动调整图象大小
/*			if (lpHtmlControl->width==0)
				lpHtmlControl->width=16;
			if (lpHtmlControl->height==0)
				lpHtmlControl->height=16;
*/
			return TRUE;  // 返回成功
		}

	//	if (lpHtmlControl->width!=0 && lpHtmlControl->height!=0 )  // not need calc the size of the control name
	//		return TRUE;

		// 是文本控制
		content=lpHtmlControl->content;
		lpHtmlControl->widthFirstRow=0;
		lpHtmlControl->widthLastRow=0;
		lpHtmlControl->height=0;
		lpHtmlControl->width=0;
		lpHtmlControl->heightLine=0;

		if (content==NULL)
			return TRUE; // 文本内容为空
		cbName=StrLen(lpHtmlControl->content);  // 得到文本长度
		iHeightLine=SMIE_GetFontHeight(hWnd,lpHtmlControl->font); // 得到字体高度
		lpHtmlControl->heightLine=iHeightLine;
		while(1)
		{
	//		GetTextExtentExPoint(hdc,         // handle to device context
			// 得到在指定宽度下文本的总高度
				SMIE_GetTextLenInPoint(hdc,  
			  content, // pointer to character string
			  cbName,   // number of characters in string
			  rect.right-iStart,  // maximum width for formatted string
			  &cbChar,    // pointer to value specifying max. number of 
							   // chars.
	//		  NULL,     // pointer to array for partial string widths
			  &size// pointer to structure with string dimensions
			);	
			if (lpHtmlControl->widthFirstRow==0)
				lpHtmlControl->widthFirstRow=size.cx;

			if (*(content+cbChar)==0x0d&&*(content+cbChar+1)==0x0a)
			{  // 回车
				cbChar+=2;
			}
			else if (*(content+cbChar)==0x0a)
			{  // 回车
				cbChar+=1;
			}

			if (cbChar==cbName||(cbChar==0 && iStart == (WORD)rect.left))
				break;  // 文本结束
			content+=cbChar;  // 得到下一行的位置
			cbName-=cbChar;
			lpHtmlControl->height+=iHeightLine;  // 加上一行的高度
			iStart=(WORD)rect.left;
			lpHtmlControl->width=rect.right-rect.left; // 当前宽度为指定范围的宽度
	//		x=rect.left;
	//		y+=size.cy;
		}
		lpHtmlControl->height+=iHeightLine;  // 设置新的高度
		lpHtmlControl->widthLastRow=size.cx;
		if (lpHtmlControl->width==0)
			lpHtmlControl->width=size.cx;  // 设置新的宽度

		return TRUE;
}

/***************************************************************************************/
// Renew Locate control position
// Parameter:
// lpTable : the pointer of the control
// lpx     : the pointer of the control start x,then will return the next x position
// lpy     : the pointer of the control start y ,then will return the next y position

//  The controls may have some condition:
//  eg1:
//       +---------------------------------------------------------------------+
//       |        +---------++---------++-------------------------------------+|
//       |        |         ||         ||                                     ||
//       |        |         ||         ||                                     ||
//       |        |  Img    ||  Img    ||           Img                       ||
//       |        |         ||         ||                                     ||
//       | abcdefg+---------++---------++-------------------------------------+|
//       +---------------------------------------------------------------------+
// eg2:
//       +---------------------------------------------------------------------+
//       |        +---------+                                                  |
//       |        |         |                                                  |
//       |        |  img    |                                                  |
//       |        |         |                                                  |
//       |        |         |                                                  |
//       | abcdefg+---------+abcdffssfsdfhshfjkhsdkfhfhfhsdjkhjdhhfskfhsdfhsksd|
//       | dfjsdfjskdfhshfs                                                    |
//       +---------------------------------------------------------------------+
/***************************************************************************************/
// **************************************************
// 声明：BOOL SMIE_ReLocate_Control(HWND hwnd,HHTMCTL lpControl,LPDWORD lpx,LPDWORD lpy,RECT rect,DWORD iRowHeight)
// 参数：
// 	IN lpControl -- 控制句柄
// 	IN/OUT lpx -- 当前控制的起始位置，并返回下一个控制的位置
// 	IN/OUT lpy -- 当前控制的起始位置，并返回下一个控制的位置
// 	IN rect -- 当前控制的范围
// 	IN iRowHeight -- 当前控制所在行的高度
// 
// 返回值： 成功返回 TRUE，否则返回FALSE
// 功能描述：重新定位当前控制。
// 引用: 
// **************************************************
//BOOL ReLocate_Control(HHTMCTL lpControl,LPWORD lpx,LPWORD lpy,RECT rect,HHTMCTL lpPreControl)
BOOL SMIE_ReLocate_Control(HWND hWnd,HHTMCTL lpControl,LPDWORD lpx,LPDWORD lpy,RECT rect,DWORD iRowHeight)
{
	LPHTMLCONTROL lpHtmlControl;//,lpPreHtmlControl;
	LPHTMLVIEW lpHtmlView;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
	if (lpHtmlView==NULL)
		return FALSE;

	lpHtmlControl=(LPHTMLCONTROL)lpControl;
//	lpPreHtmlControl=(LPHTMLCONTROL)lpPreControl;
	if (lpHtmlControl->image)
	{
//		if (lpHtmlView->iDisplayMode == DISPLAY_ONLYTEXT)
		if (lpHtmlView->iDisplayMode == DISPLAY_ONLYTEXT && lpHtmlView->iFileStyle == HTML_FILE)
		{ // 不要显示图象
			return TRUE;
		}
	}

	lpHtmlControl->x=*lpx;  // 设置当前控制的起始位置
	lpHtmlControl->y=*lpy;
/*
//	*lpx=0;
	*lpy+=lpHtmlControl->height;
*/
	if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
	{  // 不是文本
#ifdef INPUTCONTROL
		if (lpHtmlControl->content)
		{  // 重新设置输入窗口的开始位置
//			MoveWindow((HWND)lpHtmlControl->content,lpHtmlControl->x,lpHtmlControl->y,lpHtmlControl->width,lpHtmlControl->height+90,FALSE);
			SetWindowPos((HWND)lpHtmlControl->content,NULL,lpHtmlControl->x,lpHtmlControl->y,0,0,SWP_NOZORDER|SWP_NOSIZE);
		}
		*lpx+=lpHtmlControl->width;
#endif
		return TRUE;
	}
//	return TRUE;  // 成功返回，以下内容无效

	if (lpHtmlControl->image)
	{
		lpHtmlControl->y=*lpy;
		*lpx+=lpHtmlControl->widthLastRow;
	}
	else
	{
//		if (iRowHeight!=0)
//		{
//			lpHtmlControl->y=*lpy+iRowHeight-lpHtmlControl->heightLine;
//		}
		if (lpHtmlControl->height==lpHtmlControl->heightLine)
			*lpx+=lpHtmlControl->widthLastRow;
		else
		{
			*lpx=rect.left+lpHtmlControl->widthLastRow;
			*lpy+=lpHtmlControl->height-lpHtmlControl->heightLine;
		}
	}
	return TRUE;
}


// **************************************************
// 声明：static void SMIE_ShowText(HWND hWnd,HDC hdc,int x,int y,TCHAR *content,WORD cbName,RECT rect,DWORD state)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN x -- 显示位置
// 	IN y -- 显示位置
// 	IN content -- 要显示的字串
// 	IN cbName -- 显示字串长度
// 	IN rect -- 显示范围
// 	IN state -- 显示文本是否带URL连接
// 
// 返回值：无
// 功能描述：显示文本。
// 引用: 
// **************************************************
static void SMIE_ShowText(HWND hWnd,HDC hdc,int x,int y,TCHAR *content,WORD cbName,RECT rect,DWORD state)
{
	int cbChar=0;
	SIZE size;
	WORD iHeightLine=SMIE_GetFontHeight(hWnd,NULL);
	RECT rcScreen;

//	char *string="abcd efgh ijk lmnf hgdf";
//	content=string;
//	cbName=StrLen(string);
//	y+=1;  // stay a line
//	x+=1;
	GetClientRect(hWnd,&rcScreen);  // 得到窗口客户区域
	if (y>rcScreen.bottom) // 不在显示范围之内，不需要显示
		return;

	while(1)
	{
//		GetTextExtentExPoint(hdc,         // handle to device context
		// 得到每一行可显示的字符个数
		SMIE_GetTextLenInPoint(hdc,
		  content, // pointer to character string
		  cbName,   // number of characters in string
		  rect.right-x,  // maximum width for formatted string
		  &cbChar,    // pointer to value specifying max. number of 
						   // chars.
//		  NULL,     // pointer to array for partial string widths
		  &size// pointer to structure with string dimensions
		);
		if ((y + iHeightLine) > 0)
		{// 输出文本
			ExtTextOut(hdc,
				x,
				y,
				ETO_CLIPPED,
				&rect,
				content,
				cbChar,
				NULL);
			if (state&SHTM_EXISTURL)
			{// This is a Url，需要显示一条下划线
				MoveTo(hdc,x,y+iHeightLine-1);
				LineTo(hdc,x+size.cx,y+iHeightLine-1);
			}
		}
		if (*(content+cbChar)==0x0d&&*(content+cbChar+1)==0x0a)
		{ // 是回车
			cbChar+=2;
		}
		else if (*(content+cbChar)==0x0a)
		{ // 是回车
			cbChar+=1;
		}
//		if (cbChar==cbName||cbChar==0)
		if (cbChar==cbName||(cbChar==0 && x == (WORD)rect.left))
			break;  // 文本结束
		content+=cbChar;  // 到下一行的位置
		cbName-=cbChar;
		x=rect.left;
//		y+=size.cy;
		y+=iHeightLine;
		if (y>rcScreen.bottom)  
			break; // 已经不在屏幕显示范围之内
	}
}


// **************************************************
// 声明：void SMIE_ReleaseControl(HHTMCTL lpControl)
// 参数：
// 	IN lpControl -- 控制句柄 
// 
// 返回值：无
// 功能描述： 释放控制的内存。
// 引用: 
// **************************************************
void SMIE_ReleaseControl(HHTMCTL lpControl)
{
	LPHTMLCONTROL lpHtmlControl;

	lpHtmlControl=(LPHTMLCONTROL)lpControl;

	if (lpHtmlControl==NULL)
		return ;
	if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
	{ // 不是文本
		if (lpHtmlControl->content!=0)
		{// 破坏窗口
			DestroyWindow((HWND)lpHtmlControl->content);
//			SendMessage((HWND)lpHtmlControl->content,WM_CLOSE,0,0);
		}
	}
	else
	{
		if (lpHtmlControl->content!=0)
		{  // free content
			free(lpHtmlControl->content);
			lpHtmlControl->content=NULL;
		}
		if (lpHtmlControl->image!=0)
		{  // free image
			free(lpHtmlControl->image);
			lpHtmlControl->image=NULL;
		}
/*		if (lpHtmlControl->hGif)
		{
//			DestroyGif(lpHtmlControl->hGif);
			Gif_Destroy(lpHtmlControl->hGif);
			lpHtmlControl->hGif=NULL;
		}
*/
		if (lpHtmlControl->url!=0)
		{  // free url
			free(lpHtmlControl->url);
			lpHtmlControl->url=NULL;
		}
		if (lpHtmlControl->name!=0)
		{  // free url
			free(lpHtmlControl->name);
			lpHtmlControl->name=NULL;
		}
		if (lpHtmlControl->font!=0)
		{  // free font
			free(lpHtmlControl->font);
			lpHtmlControl->font=NULL;
		}
	}
	// free control
	free(lpHtmlControl);
	lpHtmlControl=NULL;
}


// **************************************************
// 声明：WORD SMIE_GetFontHeight(HWND hWnd,TCHAR *font)
// 参数：
//  IN hWnd -- 窗口句柄
// 	IN font -- 字体名称
// 
// 返回值：返回指定字体的高度
// 功能描述：得到指定字体的高度
// 引用: 
// **************************************************
WORD SMIE_GetFontHeight(HWND hWnd,TCHAR *font)
{// 目前只有一个字体，且高度为16
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return 24;
		

		return lpHtmlView->iTextHeight;
}




// **************************************************
// 声明：BOOL SMIE_GetUrlInControl(HHTMCTL lpControl,DWORD x,DWORD y,LPTSTR *lppUrl)
// 参数：
// 	IN lpControl -- 控制句柄
// 	IN  x -- 指定位置
// 	IN  y -- 指定位置
// 	OUT lppUrl -- 返回得到的URL
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：在控制范围之内得到指定位置包含的URL。
// 引用: 
// **************************************************
BOOL SMIE_GetUrlInControl(HHTMCTL lpControl,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)lpControl;
		if (lpHtmlControl->x<=x && x<=lpHtmlControl->x+lpHtmlControl->width)
		{
			if (lpHtmlControl->y<=y && y<=lpHtmlControl->y+lpHtmlControl->height)
			{ // 指定位置在当前控制之内，返回当前控制的URL
//				*lppUrl=SMIE_BufferAssignTChar(lpHtmlControl->url);
				*lppUrl=BufferAssignTChar(lpHtmlControl->url);
				return TRUE;
			}
		}
		return FALSE;
}

//#ifndef __WCE_DEFINE

// **************************************************
// 声明：static void SMIE_GetTextLenInPoint(HDC hdc, LPTSTR lpszStr, int cchString, int MaxExtent, LPINT lpnFit, LPSIZE lpSize)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpszStr -- 指定字串
// 	IN cchString -- 字串的长度
// 	IN MaxExtent -- 最大的宽度
// 	OUT lpnFit -- 满足条件的字符个数
// 	OUT lpSize -- 返回指定字串所需要的尺寸
// 
// 返回值：无
// 功能描述：得到指定字串的尺寸。
// 引用: 
// **************************************************
static void SMIE_GetTextLenInPoint(HDC hdc,
							  LPTSTR lpszStr,
							  int cchString,
							  int MaxExtent,
							  LPINT lpnFit,
							  LPSIZE lpSize)
{
	int  widthChar[128];
	int  widthChinese;
	int  widthLine=0;
	int  cchChar=0;
	int  widthCurChar=0,cchCurChar,chFit=0;

		GetCharWidth(hdc,0x0000,0x007f,widthChar);  // 得到所有英文字符的宽度
		GetCharWidth(hdc,'啊','啊',&widthChinese);  // 得到汉字的宽度
		while(*lpszStr)
		{
			if (*lpszStr==0x0d&&*(lpszStr+1)==0x0a)
			{  // 回车
//				cchChar+=2;
				break;
			}
			if (*lpszStr==0x0a)
			{ // 是回车
//				cchChar+=1;
				break;
			}
			if (*lpszStr&0x80)
			{ // 是汉字
				widthCurChar=widthChinese;
				cchCurChar=2;
			}
			else
			{ // 得到英文字符宽度
//				widthCurChar=widthChar[*lpszStr];
//				cchCurChar=1;
//				widthCurChar=GetCharWidthEx(lpszStr,&cchCurChar);
				widthCurChar=SMIE_GetCharWidthEx(hdc,lpszStr,&cchCurChar,MaxExtent,&chFit);
			}
			if ((widthLine+widthCurChar)>MaxExtent)
			{ // 加上当前字符，是否超过最大宽度
				if (widthLine==0)
				{// 第0行，设置行属性
					cchChar+=chFit;
					widthLine+=MaxExtent;
					lpszStr+=chFit;
				}
				break; // 退出
			}
			// 到下一个字符
			cchChar+=cchCurChar;
			widthLine+=widthCurChar;
			lpszStr+=cchCurChar;
		}
		// 返回指定的参数
		*lpnFit=cchChar;
		lpSize->cx=widthLine;
		lpSize->cy=14;
		return;
}
// **************************************************
// 声明：static int SMIE_GetCharWidthEx(HDC hdc,LPTSTR lpszStr,int *lpcchChar,int iFitWidth,int *lpchFit)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpszStr -- 指定字符串
// 	OUT lpcchChar -- 当前单词的字符个数
// 	IN iFitWidth -- 指定宽度
// 	OUT lpchFit -- 返回可在指定宽度放下的字符个数
// 
// 返回值：返回英文单词的宽度
// 功能描述：得到一个英文单词的长度和宽度。
// 引用: 
// **************************************************
static int SMIE_GetCharWidthEx(HDC hdc,LPTSTR lpszStr,int *lpcchChar,int iFitWidth,int *lpchFit)
{
	int  widthChar[128];
	int  wordWidth=0;

		*lpcchChar=0;
		*lpchFit=0;
		if (!(*lpszStr>=0&&*lpszStr<=0x7f))
			return 0; // 不是ASCII字符
		GetCharWidth(hdc,0x0000,0x007f,widthChar);	// 得到英文字符宽度
		if(SMIE_IsAlpha(*lpszStr)==0)
		{  // 不是英文字符
			*lpcchChar=1;
			*lpchFit=1;
			return widthChar[*lpszStr];  // 返回单个字符的宽度
		}
		while(SMIE_IsAlpha(*lpszStr))
		{ // 是英文字符
			*lpcchChar+=1;// 单词中加上该字符
			if (wordWidth<iFitWidth)
				*lpchFit+=1;// 可以放在指定范围内

			wordWidth+=widthChar[*lpszStr];  // 添加该字符的宽度
			lpszStr++;
		}
		return wordWidth;  // 返回单词的宽度
}
/*
#else

int GetTCharWidth(HDC hdc,TCHAR chChar)
{
	int cbChar;
	SIZE size;
	
	GetTextExtentExPoint(hdc,         // handle to device context
		  &chChar, // pointer to character string
		  1,   // number of characters in string
		  100,  // maximum width for formatted string
		  &cbChar,    // pointer to value specifying max. number of 
						   // chars.
		  NULL,     // pointer to array for partial string widths
		  &size// pointer to structure with string dimensions
		);	
	return size.cx;
}
static void SMIE_GetTextLenInPoint(HDC hdc,
							  LPTSTR lpszStr,
							  int cchString,
							  int MaxExtent,
							  LPINT lpnFit,
							  LPSIZE lpSize)
{
	int widthChar[256];
	int  widthChinese;
	int  widthLine=0;
	int  cchChar=0;
	int  widthCurChar=0,cchCurChar;
	TCHAR chChar;

//		GetCharWidth32(hdc,0x0000,0x007f,widthChar);
//		GetCharWidth32(hdc,'啊','啊',&widthChinese);
//		JAMIMSG(DBG_FUNCTION,(TEXT("Enter GetTextLenInPoint\r\n")));
//		JAMIMSG(DBG_FUNCTION,(TEXT("GetTCharWidth Success\r\n")));

		while(*lpszStr)
		{
			if (*lpszStr==0x0d&&*(lpszStr+1)==0x0a)
			{
				cchChar+=2;
				break;
			}
			if (*lpszStr==0x0a)
			{
				cchChar+=1;
				break;
			}
			widthCurChar=GetTCharWidth(hdc,*lpszStr);
			cchCurChar=1;
			if ((widthLine+widthCurChar)>MaxExtent)
			{
				break;
			}
			cchChar+=cchCurChar;
			widthLine+=widthCurChar;
			lpszStr+=cchCurChar;
		}
		*lpnFit=cchChar;
		lpSize->cx=widthLine;
		lpSize->cy=14;

//		JAMIMSG(DBG_FUNCTION,(TEXT("GetTextLenInPoint Success\r\n")));
		return;
}
*/
//#endif
size_t StrLen( TCHAR *string )
{
#ifdef KINGMOS_UNICODE
	return wcslen( string );
#else
	return strlen( string );
#endif
}

// **************************************************
// 声明：static BOOL SMIE_IsAlpha(TCHAR c)
// 参数：
// 	IN c -- 指定字符
// 
// 返回值： 是英文字符，则返回TRUE，否则返回FALSE
// 功能描述：指定字符是否为英文字符。
// 引用: 
// **************************************************
static BOOL SMIE_IsAlpha(TCHAR c)
{
	if ((c>='a'&&c<='z') || (c>='A'&&c<='Z'))
		return TRUE; // 是英文字符，返回TRUE
	return FALSE; // 不是英文字符，返回FALSE
}

extern LPTSTR SMIE_GetFullImage(HWND hWnd,LPTSTR lpImage);

// **************************************************
// 声明：static HGIF SMIE_GetImageSize(HWND hWnd,LPTSTR image,LPRECT lprect)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN image -- 指定的图象文件名
// 	OUT lprect -- 返回图象的大小
// 
// 返回值：返回图象句柄
// 功能描述：得到图象的大小
// 引用: 
// **************************************************
static HGIF SMIE_GetImageSize(HWND hWnd,LPTSTR image,LPRECT lprect)
{
	LPTSTR lpFullImage;
	HGIF hGif;

// !!! Add By Jami chen in 2003.08.07
		if (image == NULL)  
			return NULL; // 文件名为空
// !!! Add End By Jami chen in 2003.08.07
		lpFullImage=SMIE_GetFullImage(hWnd,image);  // 得到全路径文件名
		if (lpFullImage==NULL)
			return FALSE;
//		hGif = LoadGif( lpFullImage ) ;
		hGif = Gif_LoadFromFile( lpFullImage ) ;  // 装载图象
		if (hGif==NULL)
		{  // 装载图象失败
			if (lpFullImage) // 释放内存
				free(lpFullImage);
			return FALSE; // 返回失败
		}
//		GetGif(hGif,lprect);
		Gif_GetRect( hGif,lprect );  // 得到图象大小
//		DestroyGif( hGif ) ;
		if (lpFullImage) // 释放内存
			free(lpFullImage);
		return hGif;
}
// **************************************************
// 声明：static BOOL SMIE_GetImageSizeByHandle(HWND hWnd,HGIF hGif,LPRECT lprect)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hGif -- 指定的图象句柄
// 	OUT lprect -- 返回图象的大小
// 
// 返回值：返回图象句柄
// 功能描述：得到图象的大小
// 引用: 
// **************************************************
static BOOL SMIE_GetImageSizeByHandle(HWND hWnd,HGIF hGif,LPRECT lprect)
{
		if (hGif==NULL)
		{  // 图象句柄为空
			return FALSE;
		}
//		GetGif(hGif,lprect);
		Gif_GetRect( hGif,lprect );  // 得到图象大小
		return TRUE; // 返回成功
}

// **************************************************
// 声明：BOOL SMIE_GetMarkPosInControl(HHTMCTL lpControl,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// 参数：
// 	IN lpControl -- 控制句柄
// 	OUT lpxPos -- 返回标号的位置
// 	OUT lpyPos -- 返回标号的位置
// 	IN lpMark -- 指定的标号
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：在控制中得到标号的位置。
// 引用: 
// **************************************************
BOOL SMIE_GetMarkPosInControl(HHTMCTL lpControl,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)lpControl;  // 得到控制结构
		if (lpHtmlControl->name==NULL)
			return FALSE;
		if (stricmp(lpMark,lpHtmlControl->name)==0)
		{ // 比较指定的标号是与当前控制的名称一致
			// 返回当前控制的位置
			*lpxPos=lpHtmlControl->x;
			*lpyPos=lpHtmlControl->y;
			return TRUE;  // 返回成功
		}
		return FALSE;  // 返回失败
}


// !!! Add By Jami chen in 2003.09.09
// **************************************************
// 声明：LPHTMLOBJECT SMIE_ControlLoadImage(HWND hWnd,LPTSTR lpImageFile)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpImageFile -- 图象文件
// 
// 返回值：返回目标句柄
// 功能描述：装载图象到CONTROL
// 引用: 
// **************************************************
LPHTMLOBJECT SMIE_ControlLoadImage(HWND hWnd,LPTSTR lpImageFile)
{
	LPHTMLCONTROL lpHtmlControl;
	LPHTMLOBJECT lpObject=NULL;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL)); // 分配一个CONTROL
	if (lpHtmlControl==NULL)
		return NULL; // 分配失败

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // 初始化CONTROL结构

	// 给CONTROL结构赋值
	lpHtmlControl->x=0;
	lpHtmlControl->x=0;
	lpHtmlControl->width=0;  // the width of this control
	lpHtmlControl->height=0;  // the height of this control
	lpHtmlControl->url=NULL;
	lpHtmlControl->name=NULL;
	lpHtmlControl->color=0;
	lpHtmlControl->font=0;
	lpHtmlControl->content=NULL;
	lpHtmlControl->image=NULL;
//	lpHtmlControl->hGif=NULL;
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->heightLine=0;
	lpHtmlControl->type =CONTROLTYPE_TEXT;

	lpHtmlControl->image=BufferAssign(lpImageFile); //malloc a memory and copy the content
	if (lpHtmlControl->width==0||lpHtmlControl->height==0)
	{  // No image size,  must read from the img file
/*		RECT rect={0,0,0,0};

			// 得到图象的大小
			if (lpHtmlControl->hGif==NULL)
				lpHtmlControl->hGif=SMIE_GetImageSize(hWnd,lpHtmlControl->image,&rect);
			else
				SMIE_GetImageSizeByHandle(hWnd,lpHtmlControl->hGif,&rect);
			lpHtmlControl->width=rect.right-rect.left;
			lpHtmlControl->height=rect.bottom-rect.top;		
*/
	}
	lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));  // 分配一个目标结构
	if (lpObject==NULL)
	{
		return NULL;  // 分配失败
	}
	lpObject->lpContent=lpHtmlControl;  // 将当前控制赋给目标
	lpObject->type=TYPE_CONTROL;
	lpObject->next=NULL;

	InsertAnimationList(hWnd,lpHtmlControl);
	DownLoadImage(hWnd,lpHtmlControl,lpHtmlControl->image);  // 下载指定图象
	return lpObject;  // 返回目标
}
// !!! Add By Jami chen in 2003.09.09


// **************************************************
// 声明：LPTSTR GetControlImg(HHTMCTL hControl)
// 参数：
// 	IN hControl -- 控制句柄
// 
// 返回值： 成功返回控件的图象文件名，否则返回NULL
// 功能描述：返回当前控制的图象文件名。
// 引用: 
// **************************************************
LPTSTR GetControlImg(HHTMCTL hControl)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)hControl;  // 得到控制结构
		if (lpHtmlControl == NULL )
			return NULL;
		return lpHtmlControl->image;  // 返回图象
}

// **************************************************
// 声明：BOOL GetControlPos(HHTMCTL hControl,LPPOINT ppoint)
// 参数：
// 	IN hControl -- 控制句柄
//  OUT ppoint -- 返回当前控件的位置
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：得到当前控制的位置。
// 引用: 
// **************************************************
BOOL GetControlPos(HHTMCTL hControl,LPPOINT ppoint)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)hControl;  // 得到控制结构
		if (lpHtmlControl == NULL )
			return FALSE;
		ppoint->x = lpHtmlControl->x;
		ppoint->y = lpHtmlControl->y;
		return TRUE;  // 返回图象
}


// **************************************************
// 声明：BOOL SetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize)
// 参数：
//  IN hWnd -- 窗口句柄
// 	IN hControl -- 控制句柄
//  IN lpImageSize -- 要设置的图象大小
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：设置当前控制的尺寸。
// 引用: 
// **************************************************
BOOL SetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize)
{
	LPHTMLCONTROL lpHtmlControl;

		if (lpImageSize == NULL)
			return FALSE; //参数错误
		lpHtmlControl=(LPHTMLCONTROL)hControl;  // 得到控制结构
		if (lpHtmlControl == NULL )
			return FALSE;

		if (lpHtmlControl->width==0||lpHtmlControl->height==0)
		{
			//当前控件还没有设置尺寸,设置新的尺寸
			lpHtmlControl->width = lpImageSize->cx;
			lpHtmlControl->height = lpImageSize->cy;
			if (lpHtmlControl->image)
				lpHtmlControl->heightLine = lpHtmlControl->height;

			// 重新调整控件位置
			PostMessage(hWnd,HM_READJUSTSIZE,0,0);
		}
		return TRUE;
}

// **************************************************
// 声明：BOOL GetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize)
// 参数：
//  IN hWnd -- 窗口句柄
// 	IN hControl -- 控制句柄
//  OUT lpImageSize -- 要得到的图象大小
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：设置当前控制的尺寸。
// 引用: 
// **************************************************
BOOL GetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize)
{
	LPHTMLCONTROL lpHtmlControl;

		if (lpImageSize == NULL)
			return FALSE; //参数错误
		lpHtmlControl=(LPHTMLCONTROL)hControl;  // 得到控制结构
		if (lpHtmlControl == NULL )
			return FALSE;

		//当前控件还没有设置尺寸,设置新的尺寸
		lpImageSize->cx = lpHtmlControl->width;
		lpImageSize->cy = lpHtmlControl->height;

		return TRUE;
}


// **************************************************
// 声明：BOOL IsTextControl(HHTMCTL hControl)
// 参数：
// 	IN hControl -- 控制句柄
// 
// 返回值： 是文本控件则返回TRUE，否则返回FALSE
// 功能描述：判断当前控件是否是文本控件。
// 引用: 
// **************************************************
BOOL IsTextControl(HHTMCTL hControl)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)hControl;  // 得到控制结构
		if (lpHtmlControl == NULL )
			return FALSE;

		if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
			return FALSE;  // 不是文本，图象控件

		if (lpHtmlControl->image)
			return FALSE;  // 是图象控件

		if (lpHtmlControl->content == NULL)
			return FALSE;

		return TRUE;  // 是文本控件
}