#include "eHtmView.h"
//#include "stdafx.h"
#include "HtmlControl.h"
//#include "commctrl.h"

#define ENG_WHOLE

//extern WORD iPage,iMove;
// !!!Modified By Jami chen for WCE
// char * BufferAssign(const char *pSrc);
// !!!!
TCHAR* BufferAssign(const char *pSrc);
TCHAR* BufferAssignTChar(TCHAR *pSrc);
size_t StrLen( TCHAR *string );
// !!! Modified End
DWORD HexToDword(char *ptr);
COLOR GetColor(char *lpColor);

extern BOOL DownLoadImage(HWND hWnd,LPTSTR lpImage);


static void ProcessTAGID_A(char **stream,LPCONTROLSTATE lpControlState);
static void ProcessTAGID_A_End(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_TEXT(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_IMG(HWND hWnd,char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_FORM(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_OPTION(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_SELECT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_INPUT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_SCRIPT(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_STYLE(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_PRE(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_PREEND(char **stream,LPCONTROLSTATE lpControlState);
static HHTMCTL ProcessTAGID_MAP(char **stream,LPCONTROLSTATE lpControlState);
static void ProcessTAGID_FONT(char **stream,LPCONTROLSTATE lpControlState);
static void ProcessTAGID_FONT_End(char **stream,LPCONTROLSTATE lpControlState);
static void ShowText(HDC hdc,int x,int y,TCHAR *name,DWORD cbName,RECT rect,DWORD state);
static HGIF DrawImage(HWND hWnd,HDC hdc,LPTSTR lpImage,int x0,int y0,int width,int height);
static BOOL DrawImageByHandle(HWND hWnd,HDC hdc,HGIF hGif,int x0,int y0,int width,int height);
HGIF GetImageSize(HWND hWnd,LPTSTR image,LPRECT lprect);
BOOL GetImageSizeByHandle(HWND hWnd,HGIF hGif,LPRECT lprect);
#ifdef ENG_WHOLE
static int GetCharWidthEx(HDC hdc,LPTSTR lpszStr,int *lpcchChar,int iFitWidth,int *lpchFit);
#endif

static LPMAP GetMapName(LPCONTROLSTATE lpControlState,char *pText);

//#ifndef __WCE_DEFINE
static void GetTextLenInPoint(HDC hdc,
							  LPTSTR lpszStr,
							  int cchString,
							  int MaxExtent,
							  LPINT lpnFit,
							  LPSIZE lpSize);
//#endif
struct ColorTable{
	char *colorName;
	COLOR colorValue;
}DefaultColorTable[16]={
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
	{"aqua",	0x00ffff},  //无色
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
		lpHtmlControl->name=NULL;
		lpHtmlControl->color=color;
		lpHtmlControl->widthFirstRow=0;
		lpHtmlControl->widthLastRow=0;
		lpHtmlControl->heightLine=0;
		lpHtmlControl->type =CONTROLTYPE_TEXT;
		return lpHtmlControl;
}
*/
// !!!! Delete By Jami chen for WCE end
// !!!! Modified by Jami chen for WCE
/*
char * BufferAssign(const char *pSrc)
{
	char *ptr;
	DWORD dwLen;

		if (pSrc==NULL)
			return NULL;
		dwLen=StrLen(pSrc)+1;
		ptr=(char *)malloc(dwLen);
		if (ptr==NULL)
		{
			MessageBox(NULL,L"The memory is not enough",L"memory alloc failure",MB_OK);
			return NULL;
		}
		strcpy(ptr,pSrc);
		return ptr;
}
*/
#ifdef ENG_WHOLE
static BOOL IsAlpha(TCHAR c)
{
	if ((c>='a'&&c<='z') || (c>='A'&&c<='Z'))
		return TRUE;
	return FALSE;
}
#endif
// !!!!! 
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
COLOR GetColor(char *lpColor)
{
  DWORD colorValue=0;
	if (*lpColor=='#')
	{
		colorValue=HexToDword(lpColor+1);
	}
	else
	{
		int i;
		for (i=0;i<16;i++)
		{
			if (stricmp(lpColor,DefaultColorTable[i].colorName)==0)
				colorValue = DefaultColorTable[i].colorValue;
		}
	}
	return RGB(((colorValue&0xff0000)>>16),((colorValue&0xff00)>>8),(colorValue&0xff));
}
DWORD HexToDword(char *ptr)
{
	DWORD dwValue=0;
	while(*ptr)
	{
		switch(*ptr)
		{
			case '0':
				dwValue*=16;
				dwValue+=0;
				break;
			case '1':
				dwValue*=16;
				dwValue+=1;
				break;
			case '2':
				dwValue*=16;
				dwValue+=2;
				break;
			case '3':
				dwValue*=16;
				dwValue+=3;
				break;
			case '4':
				dwValue*=16;
				dwValue+=4;
				break;
			case '5':
				dwValue*=16;
				dwValue+=5;
				break;
			case '6':
				dwValue*=16;
				dwValue+=6;
				break;
			case '7':
				dwValue*=16;
				dwValue+=7;
				break;
			case '8':
				dwValue*=16;
				dwValue+=8;
				break;
			case '9':
				dwValue*=16;
				dwValue+=9;
				break;
			case 'a':
			case 'A':
				dwValue*=16;
				dwValue+=10;
				break;
			case 'b':
			case 'B':
				dwValue*=16;
				dwValue+=11;
				break;
			case 'c':
			case 'C':
				dwValue*=16;
				dwValue+=12;
				break;
			case 'd':
			case 'D':
				dwValue*=16;
				dwValue+=13;
				break;
			case 'e':
			case 'E':
				dwValue*=16;
				dwValue+=14;
				break;
			case 'f':
			case 'F':
				dwValue*=16;
				dwValue+=15;
				break;
			default:
				return dwValue;
		}
		ptr++;
	}
	return dwValue;
}
void ShowHtmlControl(HWND hWnd,HDC hdc,HHTMCTL hHtmlCtl,RECT rect,POINT ptOrg)
{
	LPHTMLCONTROL lpHtmlControl;
//	RECT rectInter;
	DWORD state=0;

//		if (IntersectRect(&rectInter,&rect,&rcPaint)==0)
//			return ; // the two rect is not intersection
		lpHtmlControl=(LPHTMLCONTROL)hHtmlCtl;


		if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
			return;
		{
			// The Control is or Not in the Show Area,if not ,then return;
			RECT rect;
				
				GetClientRect(hWnd,&rect);
				if ((lpHtmlControl->y>(ptOrg.y+rect.bottom))||((lpHtmlControl->y+lpHtmlControl->height)<(DWORD)(rect.top+ptOrg.y)))
					return;
		}

		if (lpHtmlControl->image)
		{
			//loadImage(
/*			Rectangle(hdc,lpHtmlControl->x-ptOrg.x,
						  lpHtmlControl->y-ptOrg.y,
						  lpHtmlControl->x-ptOrg.x+lpHtmlControl->width,
						  lpHtmlControl->y-ptOrg.y+lpHtmlControl->height);
*/						  
/*			DrawImage(hWnd,
					  hdc,
					  lpHtmlControl->image,
					  lpHtmlControl->x-ptOrg.x,
					  lpHtmlControl->y-ptOrg.y,
					  lpHtmlControl->width,
					  lpHtmlControl->height
					  );
*/
			if (lpHtmlControl->hGif==NULL)
			{
				lpHtmlControl->hGif=DrawImage(hWnd,
						   hdc,
						   lpHtmlControl->image,
						   lpHtmlControl->x-ptOrg.x,
						   lpHtmlControl->y-ptOrg.y,
						   lpHtmlControl->width,
						   lpHtmlControl->height
						   );
			}
			else
			{
				DrawImageByHandle(hWnd,
						   hdc,
						   lpHtmlControl->hGif,
						   lpHtmlControl->x-ptOrg.x,
						   lpHtmlControl->y-ptOrg.y,
						   lpHtmlControl->width,
						   lpHtmlControl->height
						   );
			}
		}
		else if (lpHtmlControl->content)
		{
//			TextOut(hdc,lpHtmlControl->x,lpHtmlControl->y,lpHtmlControl->content,StrLen(lpHtmlControl->content));
			if (lpHtmlControl->color==GetBkColor(hdc))
				SetTextColor(hdc,RGB(128,128,128));
			else
				SetTextColor(hdc,lpHtmlControl->color);
//			rect.left=lpHtmlControl->x-iMove*600;
//			rect.right=rect.left+lpHtmlControl->width;
//			rect.top=lpHtmlControl->y-iPage*400;
//			rect.bottom=rect.top+lpHtmlControl->height;

//			DrawTextEx(hdc,lpHtmlControl->content,StrLen(lpHtmlControl->content),&rect,DT_WORDBREAK, NULL);
			if (lpHtmlControl->url)
				state|=SHTM_EXISTURL;
			ShowText(hdc,lpHtmlControl->x-ptOrg.x,lpHtmlControl->y-ptOrg.y,lpHtmlControl->content,StrLen(lpHtmlControl->content),rect,state);
		}
}

/**********************************************************************/
// Prease a single Control
/**********************************************************************/
LPHTMLOBJECT ParseHtmlControl(HWND hWnd,TAGID tagID,char **stream,	LPCONTROLSTATE lpControlState)
{
	LPHTMLOBJECT lpObject=NULL;
	HHTMCTL hHtmlCtl;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter Parse Html Control\r\n")));

	switch(tagID)
	{
	case TAGID_A:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_A\r\n")));
		ProcessTAGID_A(stream,lpControlState);
		ToNextTag(stream,FALSE);
		return NULL;  
	case TAGID_A+TAGID_END:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_A\r\n")));
		ProcessTAGID_A_End(stream,lpControlState);
		ToNextTag(stream,FALSE);
		return NULL;  
	case TAGID_FONT:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FONT\r\n")));
		ProcessTAGID_FONT(stream,lpControlState);
		ToNextTag(stream,FALSE);
		return NULL;
	case TAGID_FONT+TAGID_END:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FONT\r\n")));
		ProcessTAGID_FONT_End(stream,lpControlState);
		ToNextTag(stream,FALSE);
		return NULL;
	case TAGID_TEXT:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_TEXT\r\n")));
		hHtmlCtl=ProcessTAGID_TEXT(stream,lpControlState);
		break;
	case TAGID_IMG:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_IMG\r\n")));
		hHtmlCtl=ProcessTAGID_IMG(hWnd,stream,lpControlState);
		ToNextTag(stream,FALSE);
		break;
	case TAGID_BR:
	case TAGID_P:
	case TAGID_LI:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_ENTER\r\n")));
		hHtmlCtl=NULL;
		ToNextTag(stream,FALSE);
		lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));
		if (lpObject==NULL)
			return NULL;
		lpObject->lpContent=NULL;
		lpObject->type=TYPE_CR;
		lpObject->next=NULL;
		return lpObject;
	case TAGID_FORM:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FORM\r\n")));
		hHtmlCtl=ProcessTAGID_FORM(stream,lpControlState);
		ToNextTag(stream,FALSE);
		break;
	case TAGID_OPTION:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_OPTION\r\n")));
		hHtmlCtl=ProcessTAGID_OPTION(stream,lpControlState);
		ToNextTag(stream,FALSE);
		break;
	case TAGID_SELECT:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_SELECT\r\n")));
		hHtmlCtl=ProcessTAGID_SELECT(hWnd,stream,lpControlState);
//		ToNextTag(stream);
		break;
	case TAGID_INPUT:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_INPUT\r\n")));
		hHtmlCtl=ProcessTAGID_INPUT(hWnd,stream,lpControlState);
//		ToNextTag(stream);
		break;
	case TAGID_SCRIPT:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_SCRIPT\r\n")));
		hHtmlCtl=ProcessTAGID_SCRIPT(stream,lpControlState);
		ToNextTag(stream,FALSE);
		break;
	case TAGID_STYLE:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_STYLE\r\n")));
		hHtmlCtl=ProcessTAGID_STYLE(stream,lpControlState);
		ToNextTag(stream,FALSE);
		break;
	case TAGID_PRE:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_PRE\r\n")));
		hHtmlCtl=ProcessTAGID_PRE(stream,lpControlState);
		ToNextTag(stream,FALSE);
		lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));
		if (lpObject==NULL)
			return NULL;
		lpObject->lpContent=NULL;
		lpObject->type=TYPE_CR;
		lpObject->next=NULL;
		return lpObject;
//		break;
	case TAGID_PRE+TAGID_END:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_PRE+TAGID_END\r\n")));
		hHtmlCtl=ProcessTAGID_PREEND(stream,lpControlState);
		ToNextTag(stream,FALSE);
		lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));
		if (lpObject==NULL)
			return NULL;
		lpObject->lpContent=NULL;
		lpObject->type=TYPE_CR;
		lpObject->next=NULL;
		return lpObject;
	case TAGID_MAP:
		ProcessTAGID_MAP(stream,lpControlState);
		ToNextTag(stream,FALSE);
		return NULL;
//		break;
	default:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter Default\r\n")));
		ToNextTag(stream,FALSE);
		return NULL;
	}
	if (hHtmlCtl)
	{
		lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));
		if (lpObject==NULL)
			return NULL;
		lpObject->lpContent=hHtmlCtl;
		lpObject->type=TYPE_CONTROL;
		lpObject->next=NULL;
		return lpObject;
	}
	return NULL;
}
/*******************************************************************/
//  Process tag <A> 
/*******************************************************************/
static void ProcessTAGID_A(char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pPropertyContent;

	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
		return;
	while(1)
	{
		propertyID=LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
		{
			break;
		}
		LocatePropertyContent(stream,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_HREF:  // Process property "href"
			if (lpControlState->url)  // if current url is exist ,then free the old memory
				free(lpControlState->url);
			lpControlState->url=BufferAssign(pPropertyContent); //malloc a memory and copy the content
																//to the memory ,then to the current url			break;
			break;
		case PROPERTYID_NAME:  // Process property "href"
			if (lpControlState->name)  // if current url is exist ,then free the old memory
				free(lpControlState->name);
			lpControlState->name=BufferAssign(pPropertyContent); //malloc a memory and copy the content
																//to the memory ,then to the current url			break;
			break;
		}
	}
	if (pPropertyContent)
		free(pPropertyContent);
}
/*******************************************************************/
//  Process tag <A> End
/*******************************************************************/
/*******************************************************************/
//  Process tag </A> 
/*******************************************************************/
static void ProcessTAGID_A_End(char **stream,LPCONTROLSTATE lpControlState)
{
	if (lpControlState->url)  // if current url is exist ,then free the old memory
		free(lpControlState->url);
	lpControlState->url=NULL;
	if (lpControlState->name)  // if current url is exist ,then free the old memory
		free(lpControlState->name);
	lpControlState->name=NULL;
	return ;
}
/*******************************************************************/
//  Process tag </A> End
/*******************************************************************/

/*******************************************************************/
//  Process tag <FONT> 
/*******************************************************************/
static void ProcessTAGID_FONT(char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pPropertyContent;

	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
		return ;
	while(1)
	{
		propertyID=LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		LocatePropertyContent(stream,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_COLOR:  // Process property "href"
			lpControlState->color=GetColor(pPropertyContent);
		}
	}
	
	if (pPropertyContent)
		free(pPropertyContent);
}
/*******************************************************************/
//  Process tag <FONT> End
/*******************************************************************/

//  Process tag </FONT> 
/*******************************************************************/
static void ProcessTAGID_FONT_End(char **stream,LPCONTROLSTATE lpControlState)
{
	lpControlState->color=CL_BLACK;
	return ;
}
/*******************************************************************/
//  Process tag </FONT> End
/*******************************************************************/

/*******************************************************************/
//  Process tag <TEXT> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_TEXT(char **stream,LPCONTROLSTATE lpControlState)
{
//	PROPERTYID propertyID;
	char *pText=NULL;
	LPHTMLCONTROL lpHtmlControl;
	DWORD sizeText;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ProcessTAGID_TEXT\r\n")));

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));
	if (lpHtmlControl==NULL)
	{
		ToNextTag(stream,FALSE);
		return NULL;
	}

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));

	JAMIMSG(DBG_PROCTEXT,(TEXT("GetTagContentLen\r\n")));
	sizeText=GetTagContentLen(*stream,lpControlState->bPreProcess);

	pText=(char *)malloc(sizeText+2);
	if (pText==NULL)
		return NULL;

	JAMIMSG(DBG_PROCTEXT,(TEXT("GetTagContent\r\n")));
	LocateTagContent(stream,pText,sizeText,lpControlState->bPreProcess);  // Get text

	sizeText=strlen(pText);

	lpHtmlControl->x=lpControlState->x;
	lpHtmlControl->x=lpControlState->y;
	lpHtmlControl->content=BufferAssign(pText);
	lpHtmlControl->width=0;  // the width of this control
	lpHtmlControl->height=0;  // the height of this control
	lpHtmlControl->image=NULL;
	lpHtmlControl->hGif=NULL;
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->url=BufferAssign(lpControlState->url);
// !!!
	lpHtmlControl->url=BufferAssignTChar(lpControlState->url);
	lpHtmlControl->name=BufferAssignTChar(lpControlState->name);
// !!! Modified End
	lpHtmlControl->color=lpControlState->color;
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->font=BufferAssign(lpControlState->font);
// !!!
	lpHtmlControl->font=BufferAssignTChar(lpControlState->font);
// !!! Modified End
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->heightLine=0;
	lpHtmlControl->type =CONTROLTYPE_TEXT;

	JAMIMSG(DBG_PROCTEXT,(TEXT("Free Memory\r\n")));
	if(pText)
		free(pText);
	JAMIMSG(DBG_PROCTEXT,(TEXT("Process Text Success\r\n")));
	return lpHtmlControl;
}
/*******************************************************************/
//  Process tag <TEXT> End
/*******************************************************************/

/*******************************************************************/
//  Process tag <IMG> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_IMG(HWND hWnd,char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pText;
	LPHTMLCONTROL lpHtmlControl;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));
	if (lpHtmlControl==NULL)
		return NULL;

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));

	pText=(char *)malloc(1024);
	if (pText==NULL)
	{
		free(lpHtmlControl);
		return NULL;
	}
	lpHtmlControl->x=lpControlState->x;
	lpHtmlControl->x=lpControlState->y;
	lpHtmlControl->width=0;  // the width of this control
	lpHtmlControl->height=0;  // the height of this control
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->url=BufferAssign(lpControlState->url);
// !!!
	lpHtmlControl->url=BufferAssignTChar(lpControlState->url);
	lpHtmlControl->name=BufferAssignTChar(lpControlState->name);
// !!! Modified End
	lpHtmlControl->color=lpControlState->color;
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->font=BufferAssign(lpControlState->font);
// !!!
	lpHtmlControl->font=BufferAssignTChar(lpControlState->font);
// !!! Modified End
	lpHtmlControl->content=NULL;
	lpHtmlControl->image=NULL;
	lpHtmlControl->hGif=NULL;
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->heightLine=0;
	lpHtmlControl->type =CONTROLTYPE_TEXT;

	while(1)
	{
		propertyID=LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		LocatePropertyContent(stream,pText,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_SRC:  // Process property "src"
			lpHtmlControl->image=BufferAssign(pText); //malloc a memory and copy the content
																//to the memory ,then to the current image
			DownLoadImage(hWnd,lpHtmlControl->image);
			break;
		case PROPERTYID_ALT:  // Process property "alt"
			lpHtmlControl->content=BufferAssign(pText); //malloc a memory and copy the content
																//to the memory ,then to the current name
			break;
		case PROPERTYID_WIDTH:  // Process property "width"
			lpHtmlControl->width=atoi(pText);//GetWidth(pText);
			lpHtmlControl->widthFirstRow=lpHtmlControl->width;
			lpHtmlControl->widthLastRow=lpHtmlControl->width;
			break;
		case PROPERTYID_HEIGHT:
			lpHtmlControl->height=atoi(pText);
			lpHtmlControl->heightLine=lpHtmlControl->height;
//			lpHtmlControl->heightLine=GetFontHeight(NULL);
//			if(lpHtmlControl->heightLine>lpHtmlControl->height)
//				lpHtmlControl->heightLine=lpHtmlControl->height;
			break;
		case PROPERTYID_USEMAP:
			lpHtmlControl->ExtControlData=(DWORD)GetMapName(lpControlState,pText);
			DeleteMap(lpControlState,(LPMAP)lpHtmlControl->ExtControlData);
			break;
		default:
			break;
		}
	}
//	if (lpHtmlControl->width==0||lpHtmlControl->height==0)
	{  // No image size,  must read from the img file
		RECT rect={0,0,0,0};
		DWORD width,height;

//			GetImageSize(hWnd,lpHtmlControl->image,&rect);
			if (lpHtmlControl->hGif==NULL)
				lpHtmlControl->hGif=GetImageSize(hWnd,lpHtmlControl->image,&rect);
			else
				GetImageSizeByHandle(hWnd,lpHtmlControl->hGif,&rect);
			width=rect.right-rect.left;
			height=rect.bottom-rect.top;
			if (width>lpHtmlControl->width)
				lpHtmlControl->width=width;
			if (height>lpHtmlControl->height)
				lpHtmlControl->height=height;
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
	}
	free(pText);
	return lpHtmlControl;
}
/*******************************************************************/
//  Process tag <IMG> End
/*******************************************************************/


/*******************************************************************/
//  Process tag <FORM> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_FORM(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
	ToNextTag(stream,FALSE);
/*
	// Get td Content
	while(**stream)
	{
		tagID=LocateTag(stream);
		if (tagID==TAGID_FORM+TAGID_END)  // </FORM>
		{
			ToNextTag(stream);
			return NULL;
		}
		ToNextTag(stream);
	}
	*/
	return NULL;
}
/*******************************************************************/
//  Process tag <FORM> End
/*******************************************************************/

/*******************************************************************/
//  Process tag <OPTION> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_OPTION(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	ToNextTag(stream,FALSE);

	// Get td Content
	while(**stream)
	{
		tagID=LocateTag(stream,FALSE);
		if (tagID==TAGID_OPTION+TAGID_END)  // </OPTION>
		{
			ToNextTag(stream,FALSE);
			return NULL;
		}
		ToNextTag(stream,FALSE);
	}
	return NULL;
}
static void GetOptionItem(char **stream,char *lpItem,DWORD cbSize)
{
	TAGID tagID;
	ToNextTag(stream,FALSE);

	// Get td Content
	while(**stream)
	{
		tagID=LocateTag(stream,FALSE);
		if (tagID==TAGID_OPTION+TAGID_END)  // </OPTION>
		{
			ToNextTag(stream,FALSE);
			return;
		}
		else if (tagID==TAGID_TEXT)
		{
			LocateTagContent(stream,lpItem,cbSize,FALSE);  // Get text
		}
		ToNextTag(stream,FALSE);
	}
	return ;
}
/*******************************************************************/
//  Process tag <OPTION> End
/*******************************************************************/

/*******************************************************************/
//  Process tag <SELECT> 
/*******************************************************************/
#define ITEMMAXLENGTH 128
static HHTMCTL ProcessTAGID_SELECT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	char lpItem[ITEMMAXLENGTH];

	PROPERTYID propertyID;
	char *pText;
	LPHTMLCONTROL lpHtmlControl;
	HWND hWnd;
	HINSTANCE hInst;
	DWORD cbItem;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));
	if (lpHtmlControl==NULL)
		return NULL;

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));

	pText=(char *)malloc(1024);
	if (pText==NULL)
	{
		free(lpHtmlControl);
		return NULL;
	}
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
		propertyID=LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		LocatePropertyContent(stream,pText,1024); // get current property ID's content
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
	hInst = (HINSTANCE)GetWindowLong(hParentWnd,GWL_HINSTANCE);
	if (lpHtmlControl->width==0)
		lpHtmlControl->width=10;
	hWnd=CreateWindow(TEXT("ComboBox"),NULL,WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,lpHtmlControl->x,lpHtmlControl->y,
				lpHtmlControl->width,lpHtmlControl->height+90,hParentWnd,0,hInst,NULL);
// !!! Modified By Jami chen for WCE End
//	hWnd=CreateWindow(WC_COMBOBOXEX,NULL,WS_VISIBLE|WS_CHILD|CBS_DROPDOWN,lpHtmlControl->x,lpHtmlControl->y,
//				lpHtmlControl->width,lpHtmlControl->height,hParentWnd,0,hInst,NULL);
//	hWnd=CreateWindow("COMBOBOX","Text",WS_VISIBLE|WS_CHILD|CBS_DROPDOWN,300,200,
//				300,120,hParentWnd,0,hInst,NULL);
	lpHtmlControl->content=(TCHAR *)hWnd;
//	MoveWindow((HWND)lpHtmlControl->content,300,200,300,16,FALSE);
	ToNextTag(stream,FALSE);

	// Get td Content
	while(**stream)
	{
		tagID=LocateTag(stream,FALSE);
		if (tagID==TAGID_SELECT+TAGID_END)  // </SELECT>
		{
			lpHtmlControl->width+=24;
			lpHtmlControl->widthFirstRow=lpHtmlControl->width;
			SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,lpHtmlControl->width,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
			SendMessage(hWnd,CB_SETCURSEL,0,0);
			ToNextTag(stream,FALSE);

			if (pText)
				free(pText);
			return lpHtmlControl;
		}
		else if (tagID==TAGID_OPTION)  // <OPTION>
		{  
		  TCHAR *lpItemContent;
			GetOptionItem(stream,lpItem,ITEMMAXLENGTH);
			cbItem=strlen(lpItem);
			if (lpHtmlControl->width<cbItem*8)
				lpHtmlControl->width=cbItem*8;
			lpItemContent=BufferAssign(lpItem);
			if (lpItemContent)
			{
				SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)lpItemContent);
				free(lpItemContent);
			}
		}
		ToNextTag(stream,FALSE);
	}
	lpHtmlControl->width+=24;
	lpHtmlControl->widthFirstRow=lpHtmlControl->width;
	SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,lpHtmlControl->width,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
	SendMessage(hWnd,CB_SETCURSEL,0,0);
	if (pText)
		free(pText);
	return lpHtmlControl;
}
/*******************************************************************/
//  Process tag <SELECT> End
/*******************************************************************/


/*******************************************************************/
//  Get Input Type
/*******************************************************************/
#define INPUT_NULL		0
#define INPUT_TEXT		1
#define INPUT_BUTTON	2
#define INPUT_PASSWORD	3
#define INPUT_CHECK		4
#define INPUT_RADIO		5
#define INPUT_SUBMIT	6
#define INPUT_RESET		7

#define MAXTYPE  7

WORD GetInputType(char *type)
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
		{
			return SupportType[i].type;
		}
	}
//	MessageBox(NULL,"UnKnow Input Type",type,MB_OK);
	return INPUT_NULL;
}
/*******************************************************************/
//  Get Input Type
/*******************************************************************/


/*******************************************************************/
//  Process tag <INPUT> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_INPUT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;

	PROPERTYID propertyID;
	char *pText;
	LPHTMLCONTROL lpHtmlControl;
	HWND hWnd;
	HINSTANCE hInst;
//	WORD cbItem;
	TCHAR *classInput=NULL;
	TCHAR *value=NULL;
	DWORD InputStyle=0;
	WORD InputType=INPUT_TEXT;
	DWORD size=0,valueLen=0;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));
	if (lpHtmlControl==NULL)
		return NULL;

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));

	pText=(char *)malloc(1024);
	if (pText==NULL)
	{
		free(lpHtmlControl);
		return NULL;
	}
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
		propertyID=LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		LocatePropertyContent(stream,pText,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_SIZE:  // Process property "width"
			size=atoi(pText);//GetWidth(pText);
			break;
		case PROPERTYID_TYPE:
			InputType=GetInputType(pText);
			break;
		case PROPERTYID_VALUE:
			value=BufferAssign(pText);
			break;
		default:
			break;
		}
	}
	switch(InputType)
	{
	case INPUT_TEXT:
		classInput=TEXT("EDIT");
		InputStyle=WS_BORDER|WS_CHILD|WS_VISIBLE;
		valueLen=0;
		if (value)
			valueLen=StrLen(value);
		if (size<valueLen)
			size=valueLen;
		lpHtmlControl->height=20;
		lpHtmlControl->heightLine=20;
		lpHtmlControl->width=size*8;
		break;
	case INPUT_PASSWORD:
		classInput=TEXT("EDIT");
		InputStyle=WS_BORDER|WS_CHILD|WS_VISIBLE|ES_PASSWORD;
		valueLen=0;
		if (value)
			valueLen=StrLen(value);
		if (size<valueLen)
			size=valueLen;
		lpHtmlControl->height=20;
		lpHtmlControl->heightLine=20;
		lpHtmlControl->width=size*8;
		break;
	case INPUT_CHECK:
		classInput=TEXT("BUTTON");
		InputStyle=WS_CHILD|WS_VISIBLE|BS_CHECKBOX;
		lpHtmlControl->width=size;
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
	case INPUT_RADIO:
		classInput=TEXT("BUTTON");
		InputStyle=WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON;
		lpHtmlControl->width=size;
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
		classInput=TEXT("BUTTON");
		InputStyle=WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON;
		lpHtmlControl->width=size;
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
		hInst=(HINSTANCE)GetWindowLong(hParentWnd,GWL_HINSTANCE);
// !!!!
//		hInst = NULL;
// !!! Modified By Jami chen for WCE
		hWnd=CreateWindow(classInput,value,InputStyle,lpHtmlControl->x,lpHtmlControl->y,
					lpHtmlControl->width,lpHtmlControl->height,hParentWnd,0,hInst,NULL);

		lpHtmlControl->content=(TCHAR *)hWnd;
	}
	if (value)
		free(value);
	ToNextTag(stream,FALSE);
	lpHtmlControl->widthFirstRow=lpHtmlControl->width;
	if (pText)
		free(pText);
	if (lpHtmlControl->content==NULL)
	{
		free(lpHtmlControl);
		return NULL;
	}
	return lpHtmlControl;
}
/*******************************************************************/
//  Process tag <INPUT> End
/*******************************************************************/


/*******************************************************************/
//  Process tag <SCRIPT> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_SCRIPT(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	ToNextTag(stream,FALSE);

	// Get td Content
	while(**stream)
	{
		tagID=LocateTag(stream,FALSE);
		if (tagID==TAGID_SCRIPT+TAGID_END)  // </OPTION>
		{
			ToNextTag(stream,FALSE);
			return NULL;
		}
		ToNextTag(stream,FALSE);
	}
	return NULL;
}
/*******************************************************************/
//  Process tag <SCRIPT> End
/*******************************************************************/

/*******************************************************************/
//  Process tag <STYLE> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_STYLE(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	ToNextTag(stream,FALSE);

	// Get td Content
	while(**stream)
	{
		tagID=LocateTag(stream,FALSE);
		if (tagID==TAGID_STYLE+TAGID_END)  // </STYLE>
		{
			ToNextTag(stream,FALSE);
			return NULL;
		}
		ToNextTag(stream,FALSE);
	}
	return NULL;
}
/*******************************************************************/
//  Process tag <STYLE> End
/*******************************************************************/

/*******************************************************************/
//  Process tag <PRE> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_PRE(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
//	ToNextTag(stream);

	// Get td Content
	lpControlState->bPreProcess=TRUE;
	return NULL;
}
/*******************************************************************/
//  Process tag <PRE> End
/*******************************************************************/

/*******************************************************************/
//  Process tag </PRE> 
/*******************************************************************/
static HHTMCTL ProcessTAGID_PREEND(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
//	ToNextTag(stream);

	// Get td Content
	lpControlState->bPreProcess=FALSE;
	return NULL;
}
/*******************************************************************/
//  Process tag </PRE> End
/*******************************************************************/

/*******************************************************************/
//  Process tag <MAP> 
/*******************************************************************/
static LPMAPAREA GetMapArea(char **stream);
static void InsertMapArea(LPMAP lpMap,LPMAPAREA lpMapArea);
static void DeleteMapArea(LPMAP lpMap,LPMAPAREA lpMapArea);
static void DestroyMapArea(lpMapArea);
static SHAPE GetAreaShape(char *pPropertyContent);
static BOOL GetAreaRect(char *pPropertyContent,LPRECT lprect);
static HHTMCTL ProcessTAGID_MAP(char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pPropertyContent;
	LPMAP lpMap;
	TAGID tagID;

	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
		return NULL;
	while(1)
	{
		propertyID=LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		LocatePropertyContent(stream,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_NAME:  // Process property "width"
			lpMap=CreateMap(pPropertyContent);
			if (lpMap)
				InsertMap(lpControlState,lpMap);
			break;
		default:
			break;
		}
	}

	ToNextTag(stream,FALSE);

	// Get Map area Content
	while(**stream)
	{
		tagID=LocateTag(stream,FALSE);
		if (tagID==TAGID_MAP+TAGID_END)  // </MAP>
		{
			break;
		}
		else if (tagID==TAGID_AREA)  // <AREA>
		{  
		  LPMAPAREA lpMapArea;
			lpMapArea=GetMapArea(stream);
			InsertMapArea(lpMap,lpMapArea);
		}
		ToNextTag(stream,FALSE);
	}
	return NULL;
}
LPMAP CreateMap(char *lpMapName)
{
	LPMAP lpMap;

	lpMap=(LPMAP)malloc(sizeof(MAP));
	if (lpMap==NULL)
		return lpMap;
	lpMap->lpMapArea=NULL;
	lpMap->name=BufferAssign(lpMapName);
	lpMap->next=NULL;
	return lpMap;
}
void InsertMap(LPCONTROLSTATE lpControlState,LPMAP lpMap)
{
	LPMAP lpCurMap;
	if (lpControlState->lpMap==NULL)
	{
		lpControlState->lpMap=lpMap;
		return;
	}
	lpCurMap=lpControlState->lpMap;
	while(lpCurMap->next)
	{
		lpCurMap=lpCurMap->next;
	}
	lpCurMap->next=lpMap;
	return ;
}
// delete the map form the controlstate map list
void DeleteMap(LPCONTROLSTATE lpControlState,LPMAP lpMap)
{
	LPMAP lpCurMap,lpPreMap=NULL;
	if (lpControlState->lpMap==NULL||lpMap==NULL)
	{
		return;
	}
	lpCurMap=lpControlState->lpMap;
	while(lpCurMap)
	{
		if (lpCurMap==lpMap)
		{
			if (lpPreMap==NULL)
				lpControlState->lpMap=lpCurMap->next;
			else
				lpPreMap->next=lpCurMap->next;
			return ;
		}
		lpPreMap=lpCurMap;
		lpCurMap=lpCurMap->next;
	}
	return ;
}
// Destroy this map and free the memory
void DestroyMap(LPMAP lpMap)
{
	LPMAPAREA lpMapArea,lpNextMapArea;

	if (lpMap==NULL)
		return;
	lpMapArea=lpMap->lpMapArea;
	while(lpMapArea)
	{
		lpNextMapArea=lpMapArea->next;
		DestroyMapArea(lpMapArea);
		lpMapArea=lpNextMapArea;
	}
	if (lpMap->name)
		free(lpMap->name);
	free(lpMap);
	return ;
}
static LPMAPAREA GetMapArea(char **stream)
{
	PROPERTYID propertyID;
	char *pPropertyContent;
	LPMAPAREA lpMapArea;

	lpMapArea=(LPMAPAREA)malloc(sizeof(MAPAREA));
	if (lpMapArea==NULL)
		return NULL;
	memset(lpMapArea,0,sizeof(MAPAREA));
	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
		return NULL;
	while(1)
	{
		propertyID=LocateProperty(stream); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		LocatePropertyContent(stream,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_SHAPE:  // Process property "width"
			lpMapArea->shape=GetAreaShape(pPropertyContent);
			break;
		case PROPERTYID_HREF:  // Process property "href"
			if (lpMapArea->url)  // if current url is exist ,then free the old memory
				free(lpMapArea->url);
			lpMapArea->url=BufferAssign(pPropertyContent); //malloc a memory and copy the content
																//to the memory ,then to the current url			break;
			break;
		case PROPERTYID_COORDS:  // Process property "href"
			if (lpMapArea->shape==SHAPE_RECT)
				GetAreaRect(pPropertyContent,&lpMapArea->rect);
			break;
		default:
			break;
		}
	}

	ToNextTag(stream,FALSE);
	return lpMapArea;
}
static void InsertMapArea(LPMAP lpMap,LPMAPAREA lpMapArea)
{
	LPMAPAREA lpCurArea;

	if (lpMap->lpMapArea==NULL)
	{
		lpMap->lpMapArea=lpMapArea;
		return ;
	}
	lpCurArea=lpMap->lpMapArea;
	while(lpCurArea->next)
	{
		lpCurArea=lpCurArea->next;
	}
	lpCurArea->next=lpMapArea;
	return;
}
static void DeleteMapArea(LPMAP lpMap,LPMAPAREA lpMapArea)
{
	LPMAPAREA lpCurArea;
	LPMAPAREA lpPreArea;

	if (lpMap->lpMapArea==NULL)
	{
		return ;
	}
	lpCurArea=lpMap->lpMapArea;
	lpPreArea=lpCurArea;
	while(lpCurArea)
	{
		if (lpCurArea==lpMapArea)
		{
			lpPreArea->next=lpCurArea->next;
		}
		lpPreArea=lpCurArea;
		lpCurArea=lpCurArea->next;
	}
	return;
}
static void DestroyMapArea(LPMAPAREA lpMapArea)
{
	if (lpMapArea==NULL)
		return;
	if (lpMapArea->url)
		free(lpMapArea->url);
	free(lpMapArea);
	return;
}

static SHAPE GetAreaShape(char *pPropertyContent)
{
	struct ShapeType{
		char *lpShapeType;
		SHAPE shape;
	}ShapeTypeTable[]={
		{"circ",SHAPE_CIRCLE},
		{"circle",SHAPE_CIRCLE},
		{"poly",SHAPE_POLY},
		{"polygon",SHAPE_POLY},
		{"rect",SHAPE_RECT},
		{"rectangle",SHAPE_RECT},
		{NULL,SHAPE_NULL},
	};
	int i=0;
	
	while(1)
	{
		if (ShapeTypeTable[i].lpShapeType==NULL)
			return SHAPE_NULL;
		if (stricmp(pPropertyContent,ShapeTypeTable[i].lpShapeType)==0)
			return ShapeTypeTable[i].shape;
		i++;
	}
}
static BOOL GetAreaRect(char *pPropertyContent,LPRECT lprect)
{
	int i;
	int num[4]={0,0,0,0};

	//for (i==0;i<4;i++)
	i=0;
	while(1)
	{
		if (*pPropertyContent>='0' && *pPropertyContent<='9')
		{
			num[i]=atoi(pPropertyContent);
			i++;
			while(*pPropertyContent>='0' && *pPropertyContent<='9')
				pPropertyContent++;
		}
		if (*pPropertyContent==0)
			break;
		pPropertyContent++;
	}
	lprect->left=	num[0];
	lprect->top=	num[1];
	lprect->right=	num[2];
	lprect->bottom=	num[3];
	return TRUE;
}
static LPMAP GetMapName(LPCONTROLSTATE lpControlState,char *pText)
{
	LPMAP lpCurMap;
	if (lpControlState->lpMap==NULL)
	{
		return NULL;
	}
	lpCurMap=lpControlState->lpMap;
	while(lpCurMap)
	{													//		|----> +1 will be jump this
		if (stricmp(lpCurMap->name,(pText+1))==0)  //(pText+1)  #name
		{
			return lpCurMap;
		}
		lpCurMap=lpCurMap->next;
	}
	return NULL;
}
/***************************************************************************************/
// Get Url In Map
//  if the point is in the current control ,the control is a map,then return the url 
//  from the area
//  else return false
/***************************************************************************************/
BOOL GetUrlInMap(DWORD ExtData,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPMAP lpMap;
	LPMAPAREA lpMapArea;
	POINT pt;

		lpMap=(LPMAP)ExtData;
		if (lpMap==NULL)
			return FALSE;
		lpMapArea=lpMap->lpMapArea;
		pt.x=x;
		pt.y=y;
		while(lpMapArea)
		{
			if (PtInRect(&lpMapArea->rect,pt)!=0)
			{
				// the pt in the current area
				*lppUrl=BufferAssignTChar(lpMapArea->url);
				return TRUE;
			}
			lpMapArea=lpMapArea->next;
		}
		return FALSE;
}

/*******************************************************************/
//  Process tag <MAP> End
/*******************************************************************/

/*******************************************************************/
//  ReCalc size of the Control
/*******************************************************************/
BOOL ReCalcSize_Control(HDC hdc,HHTMCTL lpControl,DWORD iStart,RECT rect)  // Recalc the Control Size
{
	LPHTMLCONTROL lpHtmlControl;
	SIZE size={0,0};
//	RECT rect;
	CONTENT content;
	int cbName,iHeightLine;
	int cbChar=0;

	lpHtmlControl=(LPHTMLCONTROL)lpControl;

	if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
	{
		DWORD newWidth;
		newWidth=lpHtmlControl->widthFirstRow;
		if (newWidth>(rect.right-rect.left-iStart))
		{
			newWidth=(rect.right-rect.left-iStart);
		}
		lpHtmlControl->width=newWidth;
		if (lpHtmlControl->type==CONTROLTYPE_SELECT)
			SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,newWidth,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
		else
			SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,newWidth,lpHtmlControl->height,SWP_NOZORDER|SWP_NOMOVE);
		size.cx=newWidth;
		size.cy=newWidth;
//		lpHtmlControl->widthFirstRow=newWidth;
		lpHtmlControl->widthLastRow=newWidth;
		return TRUE;
	}
	if (lpHtmlControl->image)
		return TRUE;

//	if (lpHtmlControl->width!=0 && lpHtmlControl->height!=0 )  // not need calc the size of the control name
//		return TRUE;

	content=lpHtmlControl->content;
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->height=0;
	lpHtmlControl->width=0;
	lpHtmlControl->heightLine=0;

	if (content==NULL)
		return TRUE;
	cbName=StrLen(lpHtmlControl->content);
	iHeightLine=GetFontHeight(lpHtmlControl->font);
	lpHtmlControl->heightLine=iHeightLine;
	while(1)
	{
//		GetTextExtentExPoint(hdc,         // handle to device context
			GetTextLenInPoint(hdc,
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

//		if (*name==0x0d&&*(name+1)==0x0a)
		if (*(content+cbChar)==0x0d&&*(content+cbChar+1)==0x0a)
		{
			cbChar+=2;
		}
		else if (*content==0x0a)
		{
			cbChar+=1;
		}

		if (cbChar==cbName||(iStart==(DWORD)rect.left&&cbChar==0))
			break;
		content+=cbChar;
		cbName-=cbChar;
		lpHtmlControl->height+=iHeightLine;
		iStart=rect.left;
		lpHtmlControl->width=rect.right-rect.left;
//		x=rect.left;
//		y+=size.cy;
	}
	lpHtmlControl->height+=iHeightLine;
	lpHtmlControl->widthLastRow=size.cx;
	if (lpHtmlControl->width==0)
		lpHtmlControl->width=size.cx;

	return TRUE;
}
/*******************************************************************/
//  ReCalc size the Control End
/*******************************************************************/

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
//BOOL ReLocate_Control(HHTMCTL lpControl,LPWORD lpx,LPWORD lpy,RECT rect,HHTMCTL lpPreControl)
BOOL ReLocate_Control(HHTMCTL lpControl,LPDWORD lpx,LPDWORD lpy,RECT rect,DWORD iRowHeight)
{
	LPHTMLCONTROL lpHtmlControl;//,lpPreHtmlControl;
	static WORD times=0;

	lpHtmlControl=(LPHTMLCONTROL)lpControl;
//	lpPreHtmlControl=(LPHTMLCONTROL)lpPreControl;

	lpHtmlControl->x=*lpx;
	lpHtmlControl->y=*lpy;
	if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
	{
		if (lpHtmlControl->content)
		{
//			MoveWindow((HWND)lpHtmlControl->content,lpHtmlControl->x,lpHtmlControl->y,lpHtmlControl->width,lpHtmlControl->height+90,FALSE);
			SetWindowPos((HWND)lpHtmlControl->content,NULL,lpHtmlControl->x,lpHtmlControl->y,0,0,SWP_NOZORDER|SWP_NOSIZE);
		}
		*lpx+=lpHtmlControl->width;
		return TRUE;
	}

	if (lpHtmlControl->image)
	{
		if ((*lpx+lpHtmlControl->width)>(DWORD)rect.right)
			return FALSE;
		lpHtmlControl->y=*lpy;
//		*lpx+=lpHtmlControl->widthLastRow;
		*lpx+=lpHtmlControl->width;
	}
	else
	{
		if (iRowHeight!=0)
		{
			lpHtmlControl->y=*lpy+iRowHeight-lpHtmlControl->heightLine;
		}
		if (lpHtmlControl->height==lpHtmlControl->heightLine)
		{
			if ((*lpx+lpHtmlControl->width)>(DWORD)rect.right)
				return FALSE;
			*lpx+=lpHtmlControl->widthLastRow;
		}
		else
		{
			*lpx=rect.left+lpHtmlControl->widthLastRow;
			*lpy+=lpHtmlControl->height-lpHtmlControl->heightLine;
		}
/*
		if (lpPreHtmlControl)
		{
			if (lpPreHtmlControl->image)
			{
				if (lpPreHtmlControl->height>lpHtmlControl->heightLine)
				{
					lpHtmlControl->y+=lpPreHtmlControl->height-lpHtmlControl->heightLine;
				}
			}
		}
*/
//		*lpy=lpHtmlControl->y+lpHtmlControl->height-lpHtmlControl->heightLine;
	}
//	if (lpHtmlControl->height>lpHtmlControl->heightLine)
//		*lpy+=lpHtmlControl->height-lpHtmlControl->heightLine;
	/*
	while(1)
	{
		if (*lpx>rect.right)
		{
			*lpy+=lpHtmlControl->height;
			*lpx-=(rect.right-rect.left);
		}
		else
			break;
	}*/
//	*lpy+=lpHtmlControl->height-16;
	return TRUE;
}
/***************************************************************************************/
// Renew Locate Control position End
/***************************************************************************************/


/***************************************************************************************/
// Show a text from (x,y) in rect
//  hdc --> handle of dc
//  x,y --> the position of start show
//  name --> the string to be show 
//  cbName --> the length to be show string
//  rect -->  show in the rectangle  
//	state --> the show state (eg,align_left ,align_right ...)
/***************************************************************************************/
static void ShowText(HDC hdc,int x,int y,TCHAR *name,DWORD cbName,RECT rect,DWORD state)
{
	DWORD cbChar=0;
	SIZE size;
	DWORD iHeightLine=GetFontHeight(NULL);

//	char *string="abcd efgh ijk lmnf hgdf";
//	name=string;
//	cbName=StrLen(string);
//	y+=1;  // stay a line
//	x+=1;
	while(1)
	{
//		GetTextExtentExPoint(hdc,         // handle to device context
		GetTextLenInPoint(hdc,
		  name, // pointer to character string
		  cbName,   // number of characters in string
		  rect.right-x,  // maximum width for formatted string
		  &cbChar,    // pointer to value specifying max. number of 
						   // chars.
//		  NULL,     // pointer to array for partial string widths
		  &size// pointer to structure with string dimensions
		);	
		if ((y + iHeightLine) > 0)
		{
			ExtTextOut(hdc,
				x,
				y,
				ETO_CLIPPED,
				&rect,
				name,
				cbChar,
				NULL);
			if (state&SHTM_EXISTURL)
			{// This is a Url
				MoveTo(hdc,x,y+iHeightLine-1);
				LineTo(hdc,x+size.cx,y+iHeightLine-1);
			}
		}
//		if (*name==0x0d&&*(name+1)==0x0a)
		if (*(name+cbChar)==0x0d&&*(name+cbChar+1)==0x0a)
		{
			cbChar+=2;
		}
		else if (*name==0x0a)
		{
			cbChar+=1;
		}
		if (cbChar==cbName||(x==rect.left && cbChar==0))
			break;
		name+=cbChar;
		cbName-=cbChar;
		x=rect.left;
//		y+=size.cy;
		y+=iHeightLine;
	}
}
/***************************************************************************************/
// Show Text End
/***************************************************************************************/


/***************************************************************************************/
// Release Control
/***************************************************************************************/
void ReleaseControl(HHTMCTL lpControl)
{
	LPHTMLCONTROL lpHtmlControl;

	lpHtmlControl=(LPHTMLCONTROL)lpControl;

	if (lpHtmlControl==NULL)
		return ;
	if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
	{
		if (lpHtmlControl->content!=0)
		{
			DestroyWindow((HWND)lpHtmlControl->content);
//			SendMessage((HWND)lpHtmlControl->content,WM_CLOSE,0,0);
		}
	}
	else
	{
		if (lpHtmlControl->content!=0)
		{  // free name
			free(lpHtmlControl->content);
			lpHtmlControl->content=NULL;
		}
		if (lpHtmlControl->image!=0)
		{  // free image
			free(lpHtmlControl->image);
			lpHtmlControl->image=NULL;
		}
		if (lpHtmlControl->hGif)
		{
//			DestroyGif(lpHtmlControl->hGif);
			Gif_Destroy(lpHtmlControl->hGif);
			lpHtmlControl->hGif=NULL;
		}
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
		if (lpHtmlControl->ExtControlData!=0)
		{
			DestroyMap((LPMAP)lpHtmlControl->ExtControlData);
			lpHtmlControl->ExtControlData=0;
		}
	}
	// free control
	free(lpHtmlControl);
	lpHtmlControl=NULL;
}
/***************************************************************************************/
// Release Control End
/***************************************************************************************/


/***************************************************************************************/
// Get current Font Height
/***************************************************************************************/
DWORD GetFontHeight(TCHAR *font)
{
	return 16;
}
/***************************************************************************************/
// Get current Font Height End
/***************************************************************************************/




/***************************************************************************************/
// Get Control Url
//  if the point is in the current control ,then return current url
//  else return false
/***************************************************************************************/
BOOL GetUrlInControl(HHTMCTL lpControl,int x,int y,LPTSTR *lppUrl)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)lpControl;
		if (lpHtmlControl->x<=x && x<=(int)(lpHtmlControl->x+lpHtmlControl->width))
		{
			if (lpHtmlControl->y<=y && y<=(int)(lpHtmlControl->y+lpHtmlControl->height))
			{
				if (lpHtmlControl->ExtControlData!=0)
				{  // the image is associate with a map
					return GetUrlInMap(lpHtmlControl->ExtControlData,x,y,lppUrl);
				}
				else
				{
					*lppUrl=BufferAssignTChar(lpHtmlControl->url);
					return TRUE;
				}
			}
		}
		return FALSE;
}

//#ifndef __WCE_DEFINE

static void GetTextLenInPoint(HDC hdc,
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

		GetCharWidth(hdc,0x0000,0x007f,widthChar);
		GetCharWidth(hdc,'啊','啊',&widthChinese);
		while(*lpszStr)
		{
			if (*lpszStr==0x0d&&*(lpszStr+1)==0x0a)
			{
//				cchChar+=2;
				break;
			}
			if (*lpszStr==0x0a)
			{
//				cchChar+=1;
				break;
			}
			if (*lpszStr&0x80)
			{
				widthCurChar=widthChinese;
				cchCurChar=2;
			}
			else
			{
//				widthCurChar=widthChar[*lpszStr];
//				cchCurChar=1;
#ifdef ENG_WHOLE
				widthCurChar=GetCharWidthEx(hdc,lpszStr,&cchCurChar,MaxExtent,&chFit);
#else
				widthCurChar=widthChar[*lpszStr];
				cchCurChar=1;
#endif
			}
			if ((widthLine+widthCurChar)>MaxExtent)
			{
#ifdef ENG_WHOLE
				if (widthLine==0)
				{
					cchChar+=chFit;
					widthLine+=MaxExtent;
					lpszStr+=chFit;
				}
#endif
				break;
			}
			cchChar+=cchCurChar;
			widthLine+=widthCurChar;
			lpszStr+=cchCurChar;
		}
		*lpnFit=cchChar;
		lpSize->cx=widthLine;
		lpSize->cy=14;
		return;
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
static void GetTextLenInPoint(HDC hdc,
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

//extern LPTSTR GetFullUrl(HWND hWnd,LPTSTR lpUrl);
extern LPTSTR GetFullImage(HWND hWnd,LPTSTR lpImage);

static HGIF DrawImage(HWND hWnd,HDC hdc,LPTSTR lpImage,int x0,int y0,int width,int height)
{
	HGIF hGif;
	RECT Rect;
	LPTSTR lpFullImage;

		lpFullImage=GetFullImage(hWnd,lpImage);
		if (lpFullImage==NULL)
			return FALSE;
//		hGif = LoadGif( lpFullImage ) ;
		hGif = Gif_LoadFromFile( lpFullImage ) ;
		if (hGif==NULL)
		{
			goto ERROR_END;
		}
		if (width==0||height==0)
		{
//			GetGif(hGif,&Rect);
			Gif_GetRect( hGif,&Rect );
			width=Rect.right;
			height=Rect.bottom;
			if (width<10 && height)
				goto ERROR_END;
		}
//		DrawGif(
		Gif_Draw(
		  hdc, // handle to destination device context
		  x0,  // x-coordinate of destination rectangle's upper-left 
					   // corner
		  y0,  // y-coordinate of destination rectangle's upper-left 
					   // corner
		  width,  // width of destination rectangle
		  height, // height of destination rectangle
		  hGif,  // handle to source device context
		  0,   // x-coordinate of source rectangle's upper-left 
					   // corner
		  0,   // y-coordinate of source rectangle's upper-left 
					   // corner
		  SRCCOPY  // raster operation code
		);
ERROR_END:
//		if (hGif)
//			DestroyGif( hGif ) ;
		if (lpFullImage)
			free(lpFullImage);
//		return TRUE;
		return hGif;
}
static BOOL DrawImageByHandle(HWND hWnd,HDC hdc,HGIF hGif,int x0,int y0,int width,int height)
{
	RECT Rect;
		if (hGif==NULL)
		{
			return FALSE;
		}
		if (width==0||height==0)
		{
//			GetGif(hGif,&Rect);
			Gif_GetRect( hGif,&Rect );
			width=Rect.right;
			height=Rect.bottom;
			if (width<10 && height)
				return FALSE;
		}
//		DrawGif(
		Gif_Draw(
		  hdc, // handle to destination device context
		  x0,  // x-coordinate of destination rectangle's upper-left 
					   // corner
		  y0,  // y-coordinate of destination rectangle's upper-left 
					   // corner
		  width,  // width of destination rectangle
		  height, // height of destination rectangle
		  hGif,  // handle to source device context
		  0,   // x-coordinate of source rectangle's upper-left 
					   // corner
		  0,   // y-coordinate of source rectangle's upper-left 
					   // corner
		  SRCCOPY  // raster operation code
		);
		return TRUE;
}

HGIF GetImageSize(HWND hWnd,LPTSTR image,LPRECT lprect)
{
	LPTSTR lpFullImage;
	HGIF hGif;

		lpFullImage=GetFullImage(hWnd,image);
		if (lpFullImage==NULL)
			return FALSE;
//		hGif = LoadGif( lpFullImage ) ;
		hGif = Gif_LoadFromFile( lpFullImage ) ;
		if (hGif==NULL)
		{
			return FALSE;
		}
//		GetGif(hGif,lprect);
		Gif_GetRect( hGif,lprect );
//		DestroyGif( hGif ) ;
		free(lpFullImage);
		return hGif;
}
BOOL GetImageSizeByHandle(HWND hWnd,HGIF hGif,LPRECT lprect)
{
		if (hGif==NULL)
		{
			return FALSE;
		}
//		GetGif(hGif,lprect);
		Gif_GetRect( hGif,lprect );
		return TRUE;
}
BOOL GetMarkPosInControl(HHTMCTL lpControl,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)lpControl;
		if (lpHtmlControl->name==NULL)
			return FALSE;
		if (stricmp(lpMark,lpHtmlControl->name)==0)
		{
			*lpxPos=lpHtmlControl->x;
			*lpyPos=lpHtmlControl->y;
			return TRUE;
		}
		return FALSE;
}

#ifdef ENG_WHOLE
static int GetCharWidthEx(HDC hdc,LPTSTR lpszStr,int *lpcchChar,int iFitWidth,int *lpchFit)
{
	int  widthChar[128];
	int  wordWidth=0;

		*lpcchChar=0;
		*lpchFit=0;
		if (!(*lpszStr>=0&&*lpszStr<=0x7f))
			return 0;
		GetCharWidth(hdc,0x0000,0x007f,widthChar);	
		if(IsAlpha(*lpszStr)==0)
		{
			*lpcchChar=1;
			*lpchFit=1;
			return widthChar[*lpszStr];
		}
		while(IsAlpha(*lpszStr))
		{
			*lpcchChar+=1;
			if (wordWidth<iFitWidth)
				*lpchFit+=1;

			wordWidth+=widthChar[*lpszStr];
			lpszStr++;
		}
		return wordWidth;
}
#endif
