/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����С��Ļģʽ��IE����ദ����Ʋ���
�汾�ţ�1.0.0.456
����ʱ�ڣ�2001-02-23
���ߣ��½��� JAMI
�޸ļ�¼��
	2004.05.26 �����ʾGIF�ļ������Ĳ���
**************************************************/
#include "eHtmView.h"
//#include "stdafx.h"
#include "Control.h"
//#include "commctrl.h"
#include "viewdef.h"

//#define INPUTCONTROL

// **************************************************
// ����������
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
// ��������
// **************************************************
struct ColorTable{
	char *colorName;
	COLOR colorValue;
}SMIE_DefaultColorTable[16]={
	{"black",	0x000000},  // ��ɫ
	{"green",	0x008000},  //��ɫ
	{"silver",	0xc0c0c0},  //����ɫ
	{"lime",	0x00ff00},	//�Ұ�ɫ
	{"gray",	0x808080},	//��ɫ
	{"olive",	0x808000},	//����ɫ
	{"white",	0xffffff},	//��ɫ
	{"yellow",	0xffff00},	//��ɫ
	{"maroon",	0x800000},	//��ɫ
	{"navy",	0x000080},	//����ɫ
	{"red",		0xff0000},	//��ɫ
	{"blue",	0x0000ff},	//��ɫ
	{"purple",	0x800080},	//��ɫ
	{"teal",	0x008080},	//���ɫ
	{"fuchsia",	0xff00ff},	//�Ϻ�ɫ
	{"aqua",	0x00ffff},  //ˮ��ɫ
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
// ������COLOR SMIE_GetColor(char *lpColor)
// ������
// 	IN  lpColor -- ָ������ɫ�ִ�
// 
// ����ֵ������һ��RGB����ɫֵ
// �����������õ�һ��RGB����ɫֵ
// ����: 
// **************************************************
COLOR SMIE_GetColor(char *lpColor)
{
  DWORD colorValue=0;
	if (*lpColor=='#')
	{  // ��һ������������ɫֵ
		colorValue=SMIE_HexToDword(lpColor+1);
	}
	else
	{  // ��һ����ɫ���
		int i;
		for (i=0;i<16;i++)
		{
			if (stricmp(lpColor,SMIE_DefaultColorTable[i].colorName)==0)  // �Ƚ���ɫ��
				colorValue = SMIE_DefaultColorTable[i].colorValue;
		}
	}
	return RGB(((colorValue&0xff0000)>>16),((colorValue&0xff00)>>8),(colorValue&0xff));  // ����RGB��ɫ
}
// **************************************************
// ������DWORD SMIE_HexToDword(char *ptr)
// ������
// 	IN  ptr -- һ��16���Ƶ����ִ�
// 
// ����ֵ������һ��DWORD
// ���������������ִ�ת��Ϊ����
// ����: 
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
			default:  // �������֣�����
				return dwValue;
		}
		ptr++;
	}
	return dwValue;
}
// **************************************************
// ������void SMIE_ShowHtmlControl(HWND hWnd,HDC hdc,HHTMCTL hHtmlCtl,RECT rect,POINT ptOrg)
// ������
// 	IN hWnd  -- ���ھ��
// 	IN hdc -- �豸���
// 	IN hHtmlCtl -- ���ƾ��
// 	IN rect -- ���ƾ���
// 	IN ptOrg -- ��ʾԭ��
// 
// ����ֵ����
// ������������ʾһ������
// ����: 
// **************************************************
void SMIE_ShowHtmlControl(HWND hWnd,HDC hdc,HHTMCTL hHtmlCtl,RECT rect,POINT ptOrg)
{
	LPHTMLCONTROL lpHtmlControl;
//	RECT rectInter;
	DWORD state=0;

	LPHTMLVIEW lpHtmlView;



		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return ;
//		if (IntersectRect(&rectInter,&rect,&rcPaint)==0)
//			return ; // the two rect is not intersection
		lpHtmlControl=(LPHTMLCONTROL)hHtmlCtl;  // �õ����ƽṹ

		if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
		{
			// �����ı�
			return;
		}
		{
			// The Control is or Not in the Show Area,if not ,then return;
			RECT rect;
								
				GetClientRect(hWnd,&rect);  // �õ����ھ���
				if ((lpHtmlControl->y>(DWORD)(ptOrg.y+rect.bottom))||((lpHtmlControl->y+lpHtmlControl->height)<(DWORD)(rect.top+ptOrg.y)))
				{  // ���ڴ��ھ��η�Χ֮�ڣ�����Ҫ��ʾ
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
			{  // ��û�м���ͼ��
				lpHtmlControl->hGif=SMIE_DrawImage(hWnd,  // ���ز���ʾͼ��
						   hdc,
						   lpHtmlControl->image,
						   lpHtmlControl->x-ptOrg.x,
						   lpHtmlControl->y-ptOrg.y,
						   lpHtmlControl->width,
						   lpHtmlControl->height
						   );
			}
			else
			{// ��ʾͼ��
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
			{ // ��Ҫ��ʾͼ��
				ShowImageControl(hWnd,lpHtmlControl);
			}
		}
		else if (lpHtmlControl->content)
		{  // ���ı�
			// �����ı���ɫ
			if (lpHtmlControl->color==GetBkColor(hdc))  // if the BackGroung Color is Same the foreground color
				SetTextColor(hdc,RGB(128,128,128));     //  then select other color to foregroung color
			else
				SetTextColor(hdc,lpHtmlControl->color);  
			
			if (lpHtmlControl->url)
				state|=SHTM_EXISTURL;  // ��ʾʱ��ʾ��URL���ӵı�־
			SMIE_ShowText(hWnd,hdc,(int)(lpHtmlControl->x-ptOrg.x),(int)(lpHtmlControl->y-ptOrg.y),lpHtmlControl->content,(WORD)StrLen(lpHtmlControl->content),rect,state);  // ��ʾָ���ı�
		}
}

// **************************************************
// ������LPHTMLOBJECT SMIE_ParseHtmlControl(HWND hWnd,TAGID tagID,char **stream,	LPCONTROLSTATE lpControlState)
// ������
// 	IN hWnd  -- ���ھ��
// 	IN tagID -- ���
// 	IN stream -- ������
// 	IN lpControlState -- ָ��CONTROLSTATE��ָ��
// 
// ����ֵ�����صõ�Ŀ���ָ�롣
// ���������������ؼ���
// ����: 
// **************************************************
LPHTMLOBJECT SMIE_ParseHtmlControl(HWND hWnd,TAGID tagID,char **stream,	LPCONTROLSTATE lpControlState)
{
	LPHTMLOBJECT lpObject=NULL;
	HHTMCTL hHtmlCtl;
	LPHTMLVIEW lpHtmlView;


	JAMIMSG(DBG_FUNCTION,(TEXT("Enter Parse Html Control\r\n")));

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
	if (lpHtmlView==NULL)
		return FALSE;

	switch(tagID)
	{
	case TAGID_A:  // <A>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_A\r\n")));
		SMIE_ProcessTAGID_A(stream,lpControlState); // ����<A>
		SMIE_ToNextTag(stream);  // ����һ�����
		return NULL;  
	case TAGID_A+TAGID_END: // </A>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_A\r\n")));
		SMIE_ProcessTAGID_A_End(stream,lpControlState);  // ���� </A>
		SMIE_ToNextTag(stream);  // ����һ�����
		return NULL;  
	case TAGID_FONT: // <FONT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FONT\r\n")));  
		SMIE_ProcessTAGID_FONT(stream,lpControlState); // ���� </FONT>
		SMIE_ToNextTag(stream);  // ����һ�����
		return NULL;
	case TAGID_FONT+TAGID_END:  // </FONT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FONT\r\n")));
		SMIE_ProcessTAGID_FONT_End(stream,lpControlState);  // ���� </FONT>
		SMIE_ToNextTag(stream); //����һ�����
		return NULL;

	case TAGID_TEXT:  // �ı�
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_TEXT\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_TEXT(stream,lpControlState);  // �����ı�
		break;
	case TAGID_IMG:  // <img>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_IMG\r\n")));
		if (lpHtmlView->iDisplayMode == DISPLAY_ONLYTEXT)
		{
			//ֻ��Ҫ��ʾ�ı�
			SMIE_ToNextTag(stream);  // ����һ�����
			return NULL;
		}
		//��Ҫͼ�󣬷���ͼ��
		hHtmlCtl=SMIE_ProcessTAGID_IMG(hWnd,stream,lpControlState);  // ����<img>
		SMIE_ToNextTag(stream);  // ����һ�����
		break;
	case TAGID_BR:  // <BR>
	case TAGID_P:  // <P>
	case TAGID_LI:  // <LI>
		// ��������Ҫһ���س�
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_ENTER\r\n")));
		hHtmlCtl=NULL;
		SMIE_ToNextTag(stream);  // ����һ�����
		lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));  // �����ڴ�
		if (lpObject==NULL)
			return NULL;  // ����ʧ��
		lpObject->lpContent=NULL;
		lpObject->type=TYPE_CR;  // �س�
		lpObject->next=NULL;
		return lpObject;
	case TAGID_FORM:  // <FORM>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_FORM\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_FORM(stream,lpControlState);   // ����<FORM>
		SMIE_ToNextTag(stream);// ����һ�����
		break;
	case TAGID_OPTION:  // <FONT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_OPTION\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_OPTION(stream,lpControlState);  // ����<FONT>
		SMIE_ToNextTag(stream); // ����һ�����
		break;
	case TAGID_SELECT:  // <SELECT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_SELECT\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_SELECT(hWnd,stream,lpControlState); //����<SELECT>
//		SMIE_ToNextTag(stream);
		break;
#ifdef INPUTCONTROL
	case TAGID_INPUT:  // <INPUT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_INPUT\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_INPUT(hWnd,stream,lpControlState);  // ����<INPUT>
//		SMIE_ToNextTag(stream);
		break;
#endif
	case TAGID_SCRIPT:  // <SCRIPT>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_SCRIPT\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_SCRIPT(stream,lpControlState);  // ����<SCRIPT>
		SMIE_ToNextTag(stream);  // ����һ�����
		break;
	case TAGID_STYLE:  // <STYLE>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_STYLE\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_STYLE(stream,lpControlState);  // ����<STYLE>
		SMIE_ToNextTag(stream);  // ����һ�����
		break;
	case TAGID_PRE:  // <PRE>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_PRE\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_PRE(stream,lpControlState);  // ����<PRE>
		SMIE_ToNextTag(stream); // ����һ�����
		break;
	case TAGID_PRE+TAGID_END:  // </PRE>
		JAMIMSG(DBG_CONTROL,(TEXT("Enter TAGID_PRE+TAGID_END\r\n")));
		hHtmlCtl=SMIE_ProcessTAGID_PREEND(stream,lpControlState);  // ����</PRE>
		SMIE_ToNextTag(stream); // ����һ�����
		break;
	default:
		JAMIMSG(DBG_CONTROL,(TEXT("Enter Default\r\n")));
		SMIE_ToNextTag(stream);  // ����һ�����
		return NULL;
	}
	if (hHtmlCtl)
	{  // �ɹ��õ�һ������
		lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT)); // ����һ��Ŀ��
		if (lpObject==NULL)
			return NULL; // ����ʧ��
		lpObject->lpContent=hHtmlCtl;  // ��Ŀ�����ݸ����ƾ��
		lpObject->type=TYPE_CONTROL;  // Ŀ����һ������
		lpObject->next=NULL;
		return lpObject;
	}
	return NULL;  // ʧ�ܷ���
}
// **************************************************
// ������static void SMIE_ProcessTAGID_A(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ����
// ��������������������<A>������
// ����: 
// **************************************************
static void SMIE_ProcessTAGID_A(char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pPropertyContent;

	pPropertyContent=(char *)malloc(1024);  // �����������ݵĻ���
	if (pPropertyContent==NULL)
		return;  // ����ʧ��
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
		free(pPropertyContent);  // �ͷŲ�����Ҫ�Ļ���
}

// **************************************************
// ������static void SMIE_ProcessTAGID_A_End(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ����
// ��������������������</A>
// ����: 
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
// ������static void SMIE_ProcessTAGID_FONT(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ����
// ��������������������<FONT>
// ����: 
// **************************************************
static void SMIE_ProcessTAGID_FONT(char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pPropertyContent;

	pPropertyContent=(char *)malloc(1024);  // �����ڴ�
	if (pPropertyContent==NULL)
		return ; // ����ʧ��
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
		free(pPropertyContent);  // �ͷŲ���Ҫ���ڴ�
}

// **************************************************
// ������static void SMIE_ProcessTAGID_FONT_End(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ����
// ��������������������</FONT>
// ����: 
// **************************************************
static void SMIE_ProcessTAGID_FONT_End(char **stream,LPCONTROLSTATE lpControlState)
{
	lpControlState->color=CL_BLACK;  // �ָ���Ĭ����ɫ
	return ;
}

// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_TEXT(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// �������������������ı�����
// ����: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_TEXT(char **stream,LPCONTROLSTATE lpControlState)
{
//	PROPERTYID propertyID;
	char *pText=NULL;
	LPHTMLCONTROL lpHtmlControl;
	DWORD sizeText;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ProcessTAGID_TEXT\r\n")));

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));  // ����һ������
	if (lpHtmlControl==NULL)
		return NULL;  // ����ʧ��

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // ��ʼ���ṹ

	JAMIMSG(DBG_PROCTEXT,(TEXT("GetTagContentLen\r\n")));
	sizeText=SMIE_GetTagContentLen(*stream,lpControlState->bPreProcess);  // �õ���ǩ���ݳ���

	pText=(char *)malloc(sizeText);  // �������ݵĻ���
	if (pText==NULL)
		return NULL;

	JAMIMSG(DBG_PROCTEXT,(TEXT("GetTagContent\r\n")));
	SMIE_LocateTagContent(stream,pText,sizeText,lpControlState->bPreProcess);  // Get text

	// �õ����Ƶ�λ��
	lpHtmlControl->x=lpControlState->x;
	lpHtmlControl->x=lpControlState->y;
//	lpHtmlControl->content=SMIE_BufferAssign(pText);
	lpHtmlControl->content=BufferAssign(pText);  // �����ı�����
	lpHtmlControl->width=0;  // the width of this control
	lpHtmlControl->height=0;  // the height of this control
	lpHtmlControl->image=NULL;
//	lpHtmlControl->hGif=NULL;
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->url=SMIE_BufferAssign(lpControlState->url);
// !!!
//	lpHtmlControl->url=SMIE_BufferAssignTChar(lpControlState->url);
	lpHtmlControl->url=BufferAssignTChar(lpControlState->url);  // ����ָ���URL
//	lpHtmlControl->name=SMIE_BufferAssignTChar(lpControlState->name);
	lpHtmlControl->name=BufferAssignTChar(lpControlState->name); // ���Ʊ��
// !!! Modified End
	lpHtmlControl->color=lpControlState->color;  // �õ���ɫ
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->font=SMIE_BufferAssign(lpControlState->font);
// !!!
//	lpHtmlControl->font=SMIE_BufferAssignTChar(lpControlState->font);
	lpHtmlControl->font=BufferAssignTChar(lpControlState->font);  // ��������
// !!! Modified End
	lpHtmlControl->widthFirstRow=0;
	lpHtmlControl->widthLastRow=0;
	lpHtmlControl->heightLine=0;
	lpHtmlControl->type =CONTROLTYPE_TEXT;

	JAMIMSG(DBG_PROCTEXT,(TEXT("Free Memory\r\n")));
	if(pText)
		free(pText);  // �ͷŲ���Ҫ���ڴ�
	JAMIMSG(DBG_PROCTEXT,(TEXT("Process Text Success\r\n")));
	return lpHtmlControl;  // ���ؿ��ƾ��
}

// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_IMG(HWND hWnd,char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN hWnd -- ���ھ��
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������<img>����
// ����: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_IMG(HWND hWnd,char **stream,LPCONTROLSTATE lpControlState)
{
	PROPERTYID propertyID;
	char *pText;
	LPHTMLCONTROL lpHtmlControl;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));  // ����һ������
	if (lpHtmlControl==NULL)
		return NULL;  // ����ʧ��

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // ��ʼ���ṹ

	pText=(char *)malloc(1024);  // ����һ���ı��ڴ�
	if (pText==NULL)
	{  // ����ʧ��
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
	lpHtmlControl->url=BufferAssignTChar(lpControlState->url);  // ����URL
//	lpHtmlControl->name=SMIE_BufferAssignTChar(lpControlState->name);
	lpHtmlControl->name=BufferAssignTChar(lpControlState->name); // ��������
// !!! Modified End
	lpHtmlControl->color=lpControlState->color;  // �õ���ɫ
// !!! Modified By Jami chen for WCE
//	lpHtmlControl->font=SMIE_BufferAssign(lpControlState->font);
// !!!
//	lpHtmlControl->font=SMIE_BufferAssignTChar(lpControlState->font);
	lpHtmlControl->font=BufferAssignTChar(lpControlState->font);  // ��������
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
			DownLoadImage(hWnd,lpHtmlControl,lpHtmlControl->image);  // ����ָ��ͼ��
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
			//// �õ�ͼ��Ĵ�С
			if (lpHtmlControl->hGif==NULL)
				lpHtmlControl->hGif=SMIE_GetImageSize(hWnd,lpHtmlControl->image,&rect);  
			else
				SMIE_GetImageSizeByHandle(hWnd,lpHtmlControl->hGif,&rect);
			lpHtmlControl->width=rect.right-rect.left;
			lpHtmlControl->height=rect.bottom-rect.top;		
*/
	}
	free(pText);  // �ͷŲ���Ҫ���ڴ�

	// Add By Jami chen in 2004.05.26 for gif Animation
	InsertAnimationList(hWnd,lpHtmlControl);
	return lpHtmlControl;  // ���ؿ��ƾ��
}


// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_FORM(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������<FORM>����
// ����: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_FORM(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
	SMIE_ToNextTag(stream);  // ����һ�����
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
// ������static HHTMCTL SMIE_ProcessTAGID_OPTION(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������<OPTION>����
// ����: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_OPTION(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	SMIE_ToNextTag(stream);

	// ��ʱ������<option>�������������,ֱ����/OPTION����
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // �õ���ǩ���
		if (tagID==TAGID_OPTION+TAGID_END)  // </OPTION>
		{
			SMIE_ToNextTag(stream);  // ����һ����ǩ
			return NULL;
		}
		SMIE_ToNextTag(stream);  // ����һ����ǩ
	}
	return NULL;
}
// **************************************************
// ������static void SMIE_GetOptionItem(char **stream,char *lpItem,WORD cbSize)
// ������
// 	IN/OUT stream -- ������
// 	OUT lpItem -- �õ���Ŀ���ݵĻ���
// 	IN cbSize -- �����С
// 
// ����ֵ����
// �����������õ�OPTION��Ŀ������
// ����: 
// **************************************************
static void SMIE_GetOptionItem(char **stream,char *lpItem,WORD cbSize)
{
	TAGID tagID;
	SMIE_ToNextTag(stream);  // ����һ����ǩ���

	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // �õ���ǩ���
		if (tagID==TAGID_OPTION+TAGID_END)  // </OPTION>
		{
			SMIE_ToNextTag(stream); // ����һ����ǩ���
			return;
		}
		else if (tagID==TAGID_TEXT)
		{// ���ı�
			SMIE_LocateTagContent(stream,lpItem,cbSize,FALSE);  // Get text
		}
		SMIE_ToNextTag(stream);  // ����һ����ǩ���
	}
	return ;
}

// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_SELECT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN hParentWnd -- ���ھ��
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������<SELECT>����
// ����: 
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

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));  // ����һ������
	if (lpHtmlControl==NULL)
		return NULL;  // ����ʧ��

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // ��ʼ���ṹ

	pText=(char *)malloc(1024);  // ����һ���ı��ڴ�
	if (pText==NULL)
	{ // ����ʧ��
		free(lpHtmlControl);
		return NULL;
	}
	// ���ṹ��ֵ
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
	hInst = GetWindowLong(hParentWnd,GWL_HINSTANCE);  // �õ�ʵ�����
	if (lpHtmlControl->width==0)
		lpHtmlControl->width=10;
	// ����һ��COMBOBOX����
	hWnd=CreateWindow(TEXT("ComboBox"),NULL,WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,lpHtmlControl->x,lpHtmlControl->y,
				lpHtmlControl->width,lpHtmlControl->height+90,hParentWnd,0,hInst,NULL);
// !!! Modified By Jami chen for WCE End
//	hWnd=CreateWindow(WC_COMBOBOXEX,NULL,WS_VISIBLE|WS_CHILD|CBS_DROPDOWN,lpHtmlControl->x,lpHtmlControl->y,
//				lpHtmlControl->width,lpHtmlControl->height,hParentWnd,0,hInst,NULL);
//	hWnd=CreateWindow("COMBOBOX","Text",WS_VISIBLE|WS_CHILD|CBS_DROPDOWN,300,200,
//				300,120,hParentWnd,0,hInst,NULL);
	lpHtmlControl->content=(TCHAR *)hWnd;  // �����ھ����������
//	MoveWindow((HWND)lpHtmlControl->content,300,200,300,16,FALSE);
	SMIE_ToNextTag(stream);  // ����һ����ǩ

	// �õ�SELECT������
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // �õ���ǩ���
		if (tagID==TAGID_SELECT+TAGID_END)  // </SELECT>
		{  // ����
			lpHtmlControl->width+=24;
			lpHtmlControl->widthFirstRow=lpHtmlControl->width;
			// ���ô���λ��
			SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,lpHtmlControl->width,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
			SendMessage(hWnd,CB_SETCURSEL,0,0);// ���õ�ǰѡ����
			SMIE_ToNextTag(stream);  // ����һ����ǩ���

			if (pText)
				free(pText);  // �ͷŲ���Ҫ���ڴ�
			return lpHtmlControl; // ���ؿ��ƾ��
		}
		else if (tagID==TAGID_OPTION)  // <OPTION>
		{   // ��ǩһ��ѡ��
		  TCHAR *lpItemContent;
			GetOptionItem(stream,lpItem,ITEMMAXLENGTH);  // �õ���ǩ����
			cbItem=strlen(lpItem); // �õ���Ŀ�ߴ�
			if (lpHtmlControl->width<cbItem*8)
				lpHtmlControl->width=cbItem*8; // ���ÿ��ƿ��
//			lpItemContent=SMIE_BufferAssign(lpItem);
			lpItemContent=BufferAssign(lpItem);  // ������Ŀ����
			if (lpItemContent)
			{
				SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)lpItemContent);  // ���һ�����ݵ�����
				free(lpItemContent);  // �ͷ���Ŀ�ڴ�
			}
		}
		SMIE_ToNextTag(stream);  // ����һ����ǩ
	}
	lpHtmlControl->width+=24;
	lpHtmlControl->widthFirstRow=lpHtmlControl->width;
	// ���ô���λ��
	SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,lpHtmlControl->width,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
	SendMessage(hWnd,CB_SETCURSEL,0,0);  // ���õ�ǰѡ����Ŀ
	if (pText)
		free(pText);  // �ͷŲ���Ҫ���ڴ�
	return lpHtmlControl; // ���ؿ��ƾ��
}
#else
#define ITEMMAXLENGTH 128
static HHTMCTL SMIE_ProcessTAGID_SELECT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	// �����ﲻ����SELECT ���ʲ������估���Ժ������ֱ��</SELECT>
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // �õ���ǩ���
		if (tagID==TAGID_SELECT+TAGID_END)  // </SELECT>
		{
			SMIE_ToNextTag(stream);  // ����һ�����
			return NULL;
		}
		SMIE_ToNextTag(stream);  // ����һ�����
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
// ������WORD SMIE_GetInputType(char *type)
// ������
// 	IN type -- Ҫ�õ��������͵��ִ�
// 
// ����ֵ�����ص�ǰ����������
// �����������õ�ָ���ִ����������͡�
// ����: 
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
		{ // �Ƚ�ƥ�����������
			return SupportType[i].type;
		}
	}
//	MessageBox(NULL,"UnKnow Input Type",type,MB_OK);
	return INPUT_NULL;  // ����ʶ�����ͣ�����ʧ��
}


#ifdef INPUTCONTROL
// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_INPUT(HWND hParentWnd,char **stream,LPCONTROLSTATE lpControlState)
// ������
// ������
// 	IN hParentWnd -- ���ھ��
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������<INPUT>����
// ����: 
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

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL));  // ����һ������
	if (lpHtmlControl==NULL)
		return NULL;  // ����ʧ��

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // ��ʼ���ṹ

	pText=(char *)malloc(1024);  // ����һ���ı�
	if (pText==NULL)
	{ // ����ʧ��
		free(lpHtmlControl);
		return NULL;
	}
	// ���ṹ��ֵ
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
			InputType=GetInputType(pText);  // �õ���������
			break;
		case PROPERTYID_VALUE:  // Process property "value"
//			value=SMIE_BufferAssign(pText);
			value=BufferAssign(pText);  // �õ��������͵�����
			break;
		default:
			break;
		}
	}
	switch(InputType)
	{
	case INPUT_TEXT:  // ��һ���ı�����
		classInput=TEXT("EDIT");  // Ҫ����һ���༭��
		InputStyle=WS_BORDER|WS_CHILD|WS_VISIBLE;  // ���ñ༭���ķ��
		valueLen=0;
		if (value)
			valueLen=StrLen(value);
		if (size<valueLen)
			size=valueLen;
		lpHtmlControl->height=20;  // ���ñ༭���Ĵ�С
		lpHtmlControl->heightLine=20;
		lpHtmlControl->width=size*8;
		break;
	case INPUT_PASSWORD:  // ��һ���������봰��
		classInput=TEXT("EDIT");// Ҫ����һ���༭��
		InputStyle=WS_BORDER|WS_CHILD|WS_VISIBLE|ES_PASSWORD; // ���ñ༭���ķ��
		valueLen=0;
		if (value)
			valueLen=StrLen(value);
		if (size<valueLen)
			size=valueLen;
		lpHtmlControl->height=20;  // ���ñ༭���Ĵ�С
		lpHtmlControl->heightLine=20;
		lpHtmlControl->width=size*8;
		break;
	case INPUT_CHECK:  // ��һ��ѡ�񴰿�
		classInput=TEXT("BUTTON"); // Ҫ����һ����ť
		InputStyle=WS_CHILD|WS_VISIBLE|BS_CHECKBOX;  // ���ð�ť�ķ��
		lpHtmlControl->width=size;  //���ð�ť�Ĵ�С
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
	case INPUT_RADIO:  // ��һ����ѡ��
		classInput=TEXT("BUTTON");// Ҫ����һ����ť
		InputStyle=WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON; // ���ð�ť�ķ��
		lpHtmlControl->width=size; //���ð�ť�Ĵ�С
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
		// һ��һ��İ���
		classInput=TEXT("BUTTON"); // Ҫ����һ����ť
		InputStyle=WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON; // ���ð�ť�ķ��
		lpHtmlControl->width=size; //���ð�ť�Ĵ�С
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
		hInst=(HINSTANCE)GetWindowLong(hParentWnd,GWL_HINSTANCE);  // �õ�ʵ�����
// !!!!
//		hInst = NULL;
// !!! Modified By Jami chen for WCE
		// ����ָ���Ĵ���
		hWnd=CreateWindow(classInput,value,InputStyle,lpHtmlControl->x,lpHtmlControl->y,
					lpHtmlControl->width,lpHtmlControl->height,hParentWnd,0,hInst,NULL);

		lpHtmlControl->content=(TCHAR *)hWnd;  // �����ھ�����ø�����
	}
	if (value)  // �ͷŲ���Ҫ���ڴ�
		free(value);
	SMIE_ToNextTag(stream);  // ����һ����ǩ���
	lpHtmlControl->widthFirstRow=lpHtmlControl->width;
	if (pText) // �ͷŲ���Ҫ���ڴ�
		free(pText);
	if (lpHtmlControl->content==NULL)
	{ // ����ʧ�ܣ������ʧ��
		free(lpHtmlControl);
		return NULL;
	}
	return lpHtmlControl;  // ���ؿ��ƾ��
}
#endif


// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_SCRIPT(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������<SCRIPT>����
// ����: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_SCRIPT(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	SMIE_ToNextTag(stream);

	//����û�д���SCRIPT����������������ֱ��</SCRIPT>
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // �õ���ǩ���
		if (tagID==TAGID_SCRIPT+TAGID_END)  // </SCRIPT>
		{
			SMIE_ToNextTag(stream);  // ����һ����ǩ���
			return NULL;
		}
		SMIE_ToNextTag(stream);  // ����һ����ǩ���
	}
	return NULL;
}

// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_STYLE(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������<STYLE>����
// ����: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_STYLE(char **stream,LPCONTROLSTATE lpControlState)
{
	TAGID tagID;
	SMIE_ToNextTag(stream);

	// ��ʱ������STYLE���򲻴�����������ֱ��</STYLE>
	while(**stream)
	{
		tagID=SMIE_LocateTag(stream);  // �õ���ǩ���
		if (tagID==TAGID_STYLE+TAGID_END)  // </STYLE>
		{
			SMIE_ToNextTag(stream);  // ����һ����ǩ���
			return NULL;
		}
		SMIE_ToNextTag(stream);  // ����һ����ǩ���
	}
	return NULL;
}

// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_PRE(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������<PRE>����
// ����: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_PRE(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
//	SMIE_ToNextTag(stream);

	// Get td Content
	lpControlState->bPreProcess=TRUE;  // ����PRE��־
	return NULL;
}

// **************************************************
// ������static HHTMCTL SMIE_ProcessTAGID_PREEND(char **stream,LPCONTROLSTATE lpControlState)
// ������
// 	IN/OUT stream -- ������
// 	IN/OUT lpControlState -- ָ��ǰ����״̬��ָ��
// 
// ����ֵ������һ�����ƾ����ʧ�ܷ���0��
// ������������������</PRE>����
// ����: 
// **************************************************
static HHTMCTL SMIE_ProcessTAGID_PREEND(char **stream,LPCONTROLSTATE lpControlState)
{
//	TAGID tagID;
//	SMIE_ToNextTag(stream);

	// Get td Content
	lpControlState->bPreProcess=FALSE;  // ���PRE��־
	return NULL;
}

// **************************************************
// ������BOOL SMIE_ReCalcSize_Control(HWND hWnd,HDC hdc,HHTMCTL lpControl,DWORD iStart,RECT rect)  
// ������
//  IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpControl -- CONTROL���
// 	IN iStart -- ���ƵĿ�ʼλ��
// 	IN rect -- �������ڷ�Χ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������¼�����ƵĴ�С��
// ����: 
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

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
		lpHtmlControl=(LPHTMLCONTROL)lpControl; // �õ����ƽṹָ��

#ifdef INPUTCONTROL

		if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
		{  // �����ı����ƣ����������
			WORD newWidth;
			newWidth=lpHtmlControl->widthFirstRow;
			if (newWidth>(rect.right-rect.left-iStart))
			{// ���µõ����
				newWidth=(rect.right-rect.left-iStart);
			}
			lpHtmlControl->width=newWidth; // �������ÿ��
			// ���贰��
			if (lpHtmlControl->type==CONTROLTYPE_SELECT)
				SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,newWidth,lpHtmlControl->height+90,SWP_NOZORDER|SWP_NOMOVE);
			else
				SetWindowPos((HWND)lpHtmlControl->content,NULL,0,0,newWidth,lpHtmlControl->height,SWP_NOZORDER|SWP_NOMOVE);
			// �õ���С
			size.cx=newWidth;
			size.cy=newWidth;
	//		lpHtmlControl->widthFirstRow=newWidth;
			lpHtmlControl->widthLastRow=newWidth;
			return TRUE;  // ���سɹ�
		}

#endif

		if (lpHtmlControl->image)
		{  // ��ͼ��
			// ����õ��Ŀ��Ϊ0����Ĭ�ϴ�СΪ16*16
// !!! delete By Jami chen in 2004.09.20
			// ��ͼ��ĳߴ�Ϊ0ʱ���������ͼ�����Զ�����ͼ���С
/*			if (lpHtmlControl->width==0)
				lpHtmlControl->width=16;
			if (lpHtmlControl->height==0)
				lpHtmlControl->height=16;
*/
			return TRUE;  // ���سɹ�
		}

	//	if (lpHtmlControl->width!=0 && lpHtmlControl->height!=0 )  // not need calc the size of the control name
	//		return TRUE;

		// ���ı�����
		content=lpHtmlControl->content;
		lpHtmlControl->widthFirstRow=0;
		lpHtmlControl->widthLastRow=0;
		lpHtmlControl->height=0;
		lpHtmlControl->width=0;
		lpHtmlControl->heightLine=0;

		if (content==NULL)
			return TRUE; // �ı�����Ϊ��
		cbName=StrLen(lpHtmlControl->content);  // �õ��ı�����
		iHeightLine=SMIE_GetFontHeight(hWnd,lpHtmlControl->font); // �õ�����߶�
		lpHtmlControl->heightLine=iHeightLine;
		while(1)
		{
	//		GetTextExtentExPoint(hdc,         // handle to device context
			// �õ���ָ��������ı����ܸ߶�
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
			{  // �س�
				cbChar+=2;
			}
			else if (*(content+cbChar)==0x0a)
			{  // �س�
				cbChar+=1;
			}

			if (cbChar==cbName||(cbChar==0 && iStart == (WORD)rect.left))
				break;  // �ı�����
			content+=cbChar;  // �õ���һ�е�λ��
			cbName-=cbChar;
			lpHtmlControl->height+=iHeightLine;  // ����һ�еĸ߶�
			iStart=(WORD)rect.left;
			lpHtmlControl->width=rect.right-rect.left; // ��ǰ���Ϊָ����Χ�Ŀ��
	//		x=rect.left;
	//		y+=size.cy;
		}
		lpHtmlControl->height+=iHeightLine;  // �����µĸ߶�
		lpHtmlControl->widthLastRow=size.cx;
		if (lpHtmlControl->width==0)
			lpHtmlControl->width=size.cx;  // �����µĿ��

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
// ������BOOL SMIE_ReLocate_Control(HWND hwnd,HHTMCTL lpControl,LPDWORD lpx,LPDWORD lpy,RECT rect,DWORD iRowHeight)
// ������
// 	IN lpControl -- ���ƾ��
// 	IN/OUT lpx -- ��ǰ���Ƶ���ʼλ�ã���������һ�����Ƶ�λ��
// 	IN/OUT lpy -- ��ǰ���Ƶ���ʼλ�ã���������һ�����Ƶ�λ��
// 	IN rect -- ��ǰ���Ƶķ�Χ
// 	IN iRowHeight -- ��ǰ���������еĸ߶�
// 
// ����ֵ�� �ɹ����� TRUE�����򷵻�FALSE
// �������������¶�λ��ǰ���ơ�
// ����: 
// **************************************************
//BOOL ReLocate_Control(HHTMCTL lpControl,LPWORD lpx,LPWORD lpy,RECT rect,HHTMCTL lpPreControl)
BOOL SMIE_ReLocate_Control(HWND hWnd,HHTMCTL lpControl,LPDWORD lpx,LPDWORD lpy,RECT rect,DWORD iRowHeight)
{
	LPHTMLCONTROL lpHtmlControl;//,lpPreHtmlControl;
	LPHTMLVIEW lpHtmlView;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
	if (lpHtmlView==NULL)
		return FALSE;

	lpHtmlControl=(LPHTMLCONTROL)lpControl;
//	lpPreHtmlControl=(LPHTMLCONTROL)lpPreControl;
	if (lpHtmlControl->image)
	{
//		if (lpHtmlView->iDisplayMode == DISPLAY_ONLYTEXT)
		if (lpHtmlView->iDisplayMode == DISPLAY_ONLYTEXT && lpHtmlView->iFileStyle == HTML_FILE)
		{ // ��Ҫ��ʾͼ��
			return TRUE;
		}
	}

	lpHtmlControl->x=*lpx;  // ���õ�ǰ���Ƶ���ʼλ��
	lpHtmlControl->y=*lpy;
/*
//	*lpx=0;
	*lpy+=lpHtmlControl->height;
*/
	if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
	{  // �����ı�
#ifdef INPUTCONTROL
		if (lpHtmlControl->content)
		{  // �����������봰�ڵĿ�ʼλ��
//			MoveWindow((HWND)lpHtmlControl->content,lpHtmlControl->x,lpHtmlControl->y,lpHtmlControl->width,lpHtmlControl->height+90,FALSE);
			SetWindowPos((HWND)lpHtmlControl->content,NULL,lpHtmlControl->x,lpHtmlControl->y,0,0,SWP_NOZORDER|SWP_NOSIZE);
		}
		*lpx+=lpHtmlControl->width;
#endif
		return TRUE;
	}
//	return TRUE;  // �ɹ����أ�����������Ч

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
// ������static void SMIE_ShowText(HWND hWnd,HDC hdc,int x,int y,TCHAR *content,WORD cbName,RECT rect,DWORD state)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN x -- ��ʾλ��
// 	IN y -- ��ʾλ��
// 	IN content -- Ҫ��ʾ���ִ�
// 	IN cbName -- ��ʾ�ִ�����
// 	IN rect -- ��ʾ��Χ
// 	IN state -- ��ʾ�ı��Ƿ��URL����
// 
// ����ֵ����
// ������������ʾ�ı���
// ����: 
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
	GetClientRect(hWnd,&rcScreen);  // �õ����ڿͻ�����
	if (y>rcScreen.bottom) // ������ʾ��Χ֮�ڣ�����Ҫ��ʾ
		return;

	while(1)
	{
//		GetTextExtentExPoint(hdc,         // handle to device context
		// �õ�ÿһ�п���ʾ���ַ�����
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
		{// ����ı�
			ExtTextOut(hdc,
				x,
				y,
				ETO_CLIPPED,
				&rect,
				content,
				cbChar,
				NULL);
			if (state&SHTM_EXISTURL)
			{// This is a Url����Ҫ��ʾһ���»���
				MoveTo(hdc,x,y+iHeightLine-1);
				LineTo(hdc,x+size.cx,y+iHeightLine-1);
			}
		}
		if (*(content+cbChar)==0x0d&&*(content+cbChar+1)==0x0a)
		{ // �ǻس�
			cbChar+=2;
		}
		else if (*(content+cbChar)==0x0a)
		{ // �ǻس�
			cbChar+=1;
		}
//		if (cbChar==cbName||cbChar==0)
		if (cbChar==cbName||(cbChar==0 && x == (WORD)rect.left))
			break;  // �ı�����
		content+=cbChar;  // ����һ�е�λ��
		cbName-=cbChar;
		x=rect.left;
//		y+=size.cy;
		y+=iHeightLine;
		if (y>rcScreen.bottom)  
			break; // �Ѿ�������Ļ��ʾ��Χ֮��
	}
}


// **************************************************
// ������void SMIE_ReleaseControl(HHTMCTL lpControl)
// ������
// 	IN lpControl -- ���ƾ�� 
// 
// ����ֵ����
// ���������� �ͷſ��Ƶ��ڴ档
// ����: 
// **************************************************
void SMIE_ReleaseControl(HHTMCTL lpControl)
{
	LPHTMLCONTROL lpHtmlControl;

	lpHtmlControl=(LPHTMLCONTROL)lpControl;

	if (lpHtmlControl==NULL)
		return ;
	if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
	{ // �����ı�
		if (lpHtmlControl->content!=0)
		{// �ƻ�����
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
// ������WORD SMIE_GetFontHeight(HWND hWnd,TCHAR *font)
// ������
//  IN hWnd -- ���ھ��
// 	IN font -- ��������
// 
// ����ֵ������ָ������ĸ߶�
// �����������õ�ָ������ĸ߶�
// ����: 
// **************************************************
WORD SMIE_GetFontHeight(HWND hWnd,TCHAR *font)
{// Ŀǰֻ��һ�����壬�Ҹ߶�Ϊ16
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return 24;
		

		return lpHtmlView->iTextHeight;
}




// **************************************************
// ������BOOL SMIE_GetUrlInControl(HHTMCTL lpControl,DWORD x,DWORD y,LPTSTR *lppUrl)
// ������
// 	IN lpControl -- ���ƾ��
// 	IN  x -- ָ��λ��
// 	IN  y -- ָ��λ��
// 	OUT lppUrl -- ���صõ���URL
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// �����������ڿ��Ʒ�Χ֮�ڵõ�ָ��λ�ð�����URL��
// ����: 
// **************************************************
BOOL SMIE_GetUrlInControl(HHTMCTL lpControl,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)lpControl;
		if (lpHtmlControl->x<=x && x<=lpHtmlControl->x+lpHtmlControl->width)
		{
			if (lpHtmlControl->y<=y && y<=lpHtmlControl->y+lpHtmlControl->height)
			{ // ָ��λ���ڵ�ǰ����֮�ڣ����ص�ǰ���Ƶ�URL
//				*lppUrl=SMIE_BufferAssignTChar(lpHtmlControl->url);
				*lppUrl=BufferAssignTChar(lpHtmlControl->url);
				return TRUE;
			}
		}
		return FALSE;
}

//#ifndef __WCE_DEFINE

// **************************************************
// ������static void SMIE_GetTextLenInPoint(HDC hdc, LPTSTR lpszStr, int cchString, int MaxExtent, LPINT lpnFit, LPSIZE lpSize)
// ������
// 	IN hdc -- �豸���
// 	IN lpszStr -- ָ���ִ�
// 	IN cchString -- �ִ��ĳ���
// 	IN MaxExtent -- ���Ŀ��
// 	OUT lpnFit -- �����������ַ�����
// 	OUT lpSize -- ����ָ���ִ�����Ҫ�ĳߴ�
// 
// ����ֵ����
// �����������õ�ָ���ִ��ĳߴ硣
// ����: 
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

		GetCharWidth(hdc,0x0000,0x007f,widthChar);  // �õ�����Ӣ���ַ��Ŀ��
		GetCharWidth(hdc,'��','��',&widthChinese);  // �õ����ֵĿ��
		while(*lpszStr)
		{
			if (*lpszStr==0x0d&&*(lpszStr+1)==0x0a)
			{  // �س�
//				cchChar+=2;
				break;
			}
			if (*lpszStr==0x0a)
			{ // �ǻس�
//				cchChar+=1;
				break;
			}
			if (*lpszStr&0x80)
			{ // �Ǻ���
				widthCurChar=widthChinese;
				cchCurChar=2;
			}
			else
			{ // �õ�Ӣ���ַ����
//				widthCurChar=widthChar[*lpszStr];
//				cchCurChar=1;
//				widthCurChar=GetCharWidthEx(lpszStr,&cchCurChar);
				widthCurChar=SMIE_GetCharWidthEx(hdc,lpszStr,&cchCurChar,MaxExtent,&chFit);
			}
			if ((widthLine+widthCurChar)>MaxExtent)
			{ // ���ϵ�ǰ�ַ����Ƿ񳬹������
				if (widthLine==0)
				{// ��0�У�����������
					cchChar+=chFit;
					widthLine+=MaxExtent;
					lpszStr+=chFit;
				}
				break; // �˳�
			}
			// ����һ���ַ�
			cchChar+=cchCurChar;
			widthLine+=widthCurChar;
			lpszStr+=cchCurChar;
		}
		// ����ָ���Ĳ���
		*lpnFit=cchChar;
		lpSize->cx=widthLine;
		lpSize->cy=14;
		return;
}
// **************************************************
// ������static int SMIE_GetCharWidthEx(HDC hdc,LPTSTR lpszStr,int *lpcchChar,int iFitWidth,int *lpchFit)
// ������
// 	IN hdc -- �豸���
// 	IN lpszStr -- ָ���ַ���
// 	OUT lpcchChar -- ��ǰ���ʵ��ַ�����
// 	IN iFitWidth -- ָ�����
// 	OUT lpchFit -- ���ؿ���ָ����ȷ��µ��ַ�����
// 
// ����ֵ������Ӣ�ĵ��ʵĿ��
// �����������õ�һ��Ӣ�ĵ��ʵĳ��ȺͿ�ȡ�
// ����: 
// **************************************************
static int SMIE_GetCharWidthEx(HDC hdc,LPTSTR lpszStr,int *lpcchChar,int iFitWidth,int *lpchFit)
{
	int  widthChar[128];
	int  wordWidth=0;

		*lpcchChar=0;
		*lpchFit=0;
		if (!(*lpszStr>=0&&*lpszStr<=0x7f))
			return 0; // ����ASCII�ַ�
		GetCharWidth(hdc,0x0000,0x007f,widthChar);	// �õ�Ӣ���ַ����
		if(SMIE_IsAlpha(*lpszStr)==0)
		{  // ����Ӣ���ַ�
			*lpcchChar=1;
			*lpchFit=1;
			return widthChar[*lpszStr];  // ���ص����ַ��Ŀ��
		}
		while(SMIE_IsAlpha(*lpszStr))
		{ // ��Ӣ���ַ�
			*lpcchChar+=1;// �����м��ϸ��ַ�
			if (wordWidth<iFitWidth)
				*lpchFit+=1;// ���Է���ָ����Χ��

			wordWidth+=widthChar[*lpszStr];  // ��Ӹ��ַ��Ŀ��
			lpszStr++;
		}
		return wordWidth;  // ���ص��ʵĿ��
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
//		GetCharWidth32(hdc,'��','��',&widthChinese);
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
// ������static BOOL SMIE_IsAlpha(TCHAR c)
// ������
// 	IN c -- ָ���ַ�
// 
// ����ֵ�� ��Ӣ���ַ����򷵻�TRUE�����򷵻�FALSE
// ����������ָ���ַ��Ƿ�ΪӢ���ַ���
// ����: 
// **************************************************
static BOOL SMIE_IsAlpha(TCHAR c)
{
	if ((c>='a'&&c<='z') || (c>='A'&&c<='Z'))
		return TRUE; // ��Ӣ���ַ�������TRUE
	return FALSE; // ����Ӣ���ַ�������FALSE
}

extern LPTSTR SMIE_GetFullImage(HWND hWnd,LPTSTR lpImage);

// **************************************************
// ������static HGIF SMIE_GetImageSize(HWND hWnd,LPTSTR image,LPRECT lprect)
// ������
// 	IN hWnd -- ���ھ��
// 	IN image -- ָ����ͼ���ļ���
// 	OUT lprect -- ����ͼ��Ĵ�С
// 
// ����ֵ������ͼ����
// �����������õ�ͼ��Ĵ�С
// ����: 
// **************************************************
static HGIF SMIE_GetImageSize(HWND hWnd,LPTSTR image,LPRECT lprect)
{
	LPTSTR lpFullImage;
	HGIF hGif;

// !!! Add By Jami chen in 2003.08.07
		if (image == NULL)  
			return NULL; // �ļ���Ϊ��
// !!! Add End By Jami chen in 2003.08.07
		lpFullImage=SMIE_GetFullImage(hWnd,image);  // �õ�ȫ·���ļ���
		if (lpFullImage==NULL)
			return FALSE;
//		hGif = LoadGif( lpFullImage ) ;
		hGif = Gif_LoadFromFile( lpFullImage ) ;  // װ��ͼ��
		if (hGif==NULL)
		{  // װ��ͼ��ʧ��
			if (lpFullImage) // �ͷ��ڴ�
				free(lpFullImage);
			return FALSE; // ����ʧ��
		}
//		GetGif(hGif,lprect);
		Gif_GetRect( hGif,lprect );  // �õ�ͼ���С
//		DestroyGif( hGif ) ;
		if (lpFullImage) // �ͷ��ڴ�
			free(lpFullImage);
		return hGif;
}
// **************************************************
// ������static BOOL SMIE_GetImageSizeByHandle(HWND hWnd,HGIF hGif,LPRECT lprect)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hGif -- ָ����ͼ����
// 	OUT lprect -- ����ͼ��Ĵ�С
// 
// ����ֵ������ͼ����
// �����������õ�ͼ��Ĵ�С
// ����: 
// **************************************************
static BOOL SMIE_GetImageSizeByHandle(HWND hWnd,HGIF hGif,LPRECT lprect)
{
		if (hGif==NULL)
		{  // ͼ����Ϊ��
			return FALSE;
		}
//		GetGif(hGif,lprect);
		Gif_GetRect( hGif,lprect );  // �õ�ͼ���С
		return TRUE; // ���سɹ�
}

// **************************************************
// ������BOOL SMIE_GetMarkPosInControl(HHTMCTL lpControl,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// ������
// 	IN lpControl -- ���ƾ��
// 	OUT lpxPos -- ���ر�ŵ�λ��
// 	OUT lpyPos -- ���ر�ŵ�λ��
// 	IN lpMark -- ָ���ı��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������ڿ����еõ���ŵ�λ�á�
// ����: 
// **************************************************
BOOL SMIE_GetMarkPosInControl(HHTMCTL lpControl,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)lpControl;  // �õ����ƽṹ
		if (lpHtmlControl->name==NULL)
			return FALSE;
		if (stricmp(lpMark,lpHtmlControl->name)==0)
		{ // �Ƚ�ָ���ı�����뵱ǰ���Ƶ�����һ��
			// ���ص�ǰ���Ƶ�λ��
			*lpxPos=lpHtmlControl->x;
			*lpyPos=lpHtmlControl->y;
			return TRUE;  // ���سɹ�
		}
		return FALSE;  // ����ʧ��
}


// !!! Add By Jami chen in 2003.09.09
// **************************************************
// ������LPHTMLOBJECT SMIE_ControlLoadImage(HWND hWnd,LPTSTR lpImageFile)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpImageFile -- ͼ���ļ�
// 
// ����ֵ������Ŀ����
// ����������װ��ͼ��CONTROL
// ����: 
// **************************************************
LPHTMLOBJECT SMIE_ControlLoadImage(HWND hWnd,LPTSTR lpImageFile)
{
	LPHTMLCONTROL lpHtmlControl;
	LPHTMLOBJECT lpObject=NULL;

	lpHtmlControl=(LPHTMLCONTROL)malloc(sizeof(HTMLCONTROL)); // ����һ��CONTROL
	if (lpHtmlControl==NULL)
		return NULL; // ����ʧ��

	memset(lpHtmlControl,0,sizeof(HTMLCONTROL));  // ��ʼ��CONTROL�ṹ

	// ��CONTROL�ṹ��ֵ
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

			// �õ�ͼ��Ĵ�С
			if (lpHtmlControl->hGif==NULL)
				lpHtmlControl->hGif=SMIE_GetImageSize(hWnd,lpHtmlControl->image,&rect);
			else
				SMIE_GetImageSizeByHandle(hWnd,lpHtmlControl->hGif,&rect);
			lpHtmlControl->width=rect.right-rect.left;
			lpHtmlControl->height=rect.bottom-rect.top;		
*/
	}
	lpObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));  // ����һ��Ŀ��ṹ
	if (lpObject==NULL)
	{
		return NULL;  // ����ʧ��
	}
	lpObject->lpContent=lpHtmlControl;  // ����ǰ���Ƹ���Ŀ��
	lpObject->type=TYPE_CONTROL;
	lpObject->next=NULL;

	InsertAnimationList(hWnd,lpHtmlControl);
	DownLoadImage(hWnd,lpHtmlControl,lpHtmlControl->image);  // ����ָ��ͼ��
	return lpObject;  // ����Ŀ��
}
// !!! Add By Jami chen in 2003.09.09


// **************************************************
// ������LPTSTR GetControlImg(HHTMCTL hControl)
// ������
// 	IN hControl -- ���ƾ��
// 
// ����ֵ�� �ɹ����ؿؼ���ͼ���ļ��������򷵻�NULL
// �������������ص�ǰ���Ƶ�ͼ���ļ�����
// ����: 
// **************************************************
LPTSTR GetControlImg(HHTMCTL hControl)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)hControl;  // �õ����ƽṹ
		if (lpHtmlControl == NULL )
			return NULL;
		return lpHtmlControl->image;  // ����ͼ��
}

// **************************************************
// ������BOOL GetControlPos(HHTMCTL hControl,LPPOINT ppoint)
// ������
// 	IN hControl -- ���ƾ��
//  OUT ppoint -- ���ص�ǰ�ؼ���λ��
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰ���Ƶ�λ�á�
// ����: 
// **************************************************
BOOL GetControlPos(HHTMCTL hControl,LPPOINT ppoint)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)hControl;  // �õ����ƽṹ
		if (lpHtmlControl == NULL )
			return FALSE;
		ppoint->x = lpHtmlControl->x;
		ppoint->y = lpHtmlControl->y;
		return TRUE;  // ����ͼ��
}


// **************************************************
// ������BOOL SetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize)
// ������
//  IN hWnd -- ���ھ��
// 	IN hControl -- ���ƾ��
//  IN lpImageSize -- Ҫ���õ�ͼ���С
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// �������������õ�ǰ���Ƶĳߴ硣
// ����: 
// **************************************************
BOOL SetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize)
{
	LPHTMLCONTROL lpHtmlControl;

		if (lpImageSize == NULL)
			return FALSE; //��������
		lpHtmlControl=(LPHTMLCONTROL)hControl;  // �õ����ƽṹ
		if (lpHtmlControl == NULL )
			return FALSE;

		if (lpHtmlControl->width==0||lpHtmlControl->height==0)
		{
			//��ǰ�ؼ���û�����óߴ�,�����µĳߴ�
			lpHtmlControl->width = lpImageSize->cx;
			lpHtmlControl->height = lpImageSize->cy;
			if (lpHtmlControl->image)
				lpHtmlControl->heightLine = lpHtmlControl->height;

			// ���µ����ؼ�λ��
			PostMessage(hWnd,HM_READJUSTSIZE,0,0);
		}
		return TRUE;
}

// **************************************************
// ������BOOL GetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize)
// ������
//  IN hWnd -- ���ھ��
// 	IN hControl -- ���ƾ��
//  OUT lpImageSize -- Ҫ�õ���ͼ���С
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// �������������õ�ǰ���Ƶĳߴ硣
// ����: 
// **************************************************
BOOL GetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize)
{
	LPHTMLCONTROL lpHtmlControl;

		if (lpImageSize == NULL)
			return FALSE; //��������
		lpHtmlControl=(LPHTMLCONTROL)hControl;  // �õ����ƽṹ
		if (lpHtmlControl == NULL )
			return FALSE;

		//��ǰ�ؼ���û�����óߴ�,�����µĳߴ�
		lpImageSize->cx = lpHtmlControl->width;
		lpImageSize->cy = lpHtmlControl->height;

		return TRUE;
}


// **************************************************
// ������BOOL IsTextControl(HHTMCTL hControl)
// ������
// 	IN hControl -- ���ƾ��
// 
// ����ֵ�� ���ı��ؼ��򷵻�TRUE�����򷵻�FALSE
// �����������жϵ�ǰ�ؼ��Ƿ����ı��ؼ���
// ����: 
// **************************************************
BOOL IsTextControl(HHTMCTL hControl)
{
	LPHTMLCONTROL lpHtmlControl;

		lpHtmlControl=(LPHTMLCONTROL)hControl;  // �õ����ƽṹ
		if (lpHtmlControl == NULL )
			return FALSE;

		if (lpHtmlControl->type!=CONTROLTYPE_TEXT)
			return FALSE;  // �����ı���ͼ��ؼ�

		if (lpHtmlControl->image)
			return FALSE;  // ��ͼ��ؼ�

		if (lpHtmlControl->content == NULL)
			return FALSE;

		return TRUE;  // ���ı��ؼ�
}