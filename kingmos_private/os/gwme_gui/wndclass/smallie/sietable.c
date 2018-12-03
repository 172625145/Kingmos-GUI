/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����С��Ļģʽ��IE����ദ��TABLE
�汾�ţ�1.0.0.456
����ʱ�ڣ�2001-02-16
���ߣ��½��� Jami 
�޸ļ�¼��
**************************************************/
#include "eHtmView.h"
#include "SIETable.h"
#include "Locate.h"
#include "Control.h"
#include "ViewDef.h"

//extern DWORD iPage,iMove;
// **************************************************
// ������
// **************************************************

#define PERCENTTAG  10000
//#define MAXWIDTH 10000
#define MAXHEIGHT 10000

// **************************************************
// ����������
// **************************************************
LPHTMLOBJECT SMIE_ParseHtmlHead(char **lpHtmlBuffer);
//LPHTMLOBJECT ParseTable(char **lpHtmlBuffer,DWORD width_up);
LPHTMLOBJECT SMIE_ParseTable(HWND hWnd,char **lpHtmlBuffer);
//LPTR_TABLE ParseTR(char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable,DWORD width_up);
LPTR_TABLE SMIE_ParseTR(HWND hWnd,char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable);
//LPTD_TABLE ParseTD(char **lpHtmlBuffer,DWORD width_up);
LPTD_TABLE SMIE_ParseTD(HWND hWnd,char **lpHtmlBuffer);

void SMIE_AdjustTable(LPHTMLTABLE  lpTable);
DWORD SMIE_AdjustTR(LPTR_TABLE tr);
BOOL SMIE_InsertEmptyTDInTR(LPTR_TABLE tr,DWORD iInsertCol,DWORD colSpan);



//BOOL ReCalcSize(HWND hWnd,HDC hdc,LPHTMLHEAD lpHead);
BOOL SMIE_ReCalcSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE  lpTable,DWORD iTDMaxWidth,BOOL bAdjust);
BOOL SMIE_ReCalcSize_TR(HWND hWnd,HDC hdc,LPTR_TABLE tr,LPHTMLTABLE  lpTable,BOOL bAdjust);
//BOOL ReCalcSize_TD(HDC hdc,LPTD_TABLE td,LPHTMLTABLE  lpTable);
BOOL SMIE_ReCalcSize_TD(HWND hWnd,HDC hdc,LPTD_TABLE td,DWORD iTDMaxWidth);
SIZE SMIE_ReCalcSize_Object(HWND hWnd,HDC hdc,LPHTMLOBJECT lpObject,DWORD iTDMaxWidth);

void SMIE_AdjustSize_Table(LPHTMLTABLE  lpTable);
void SMIE_AdjustSize_TR(LPTR_TABLE tr,LPHTMLTABLE  lpTable);

void SMIE_CompressSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD widthCompress,DWORD iTDMaxWidth);
void SMIE_Increase_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD iTDMaxWidth);


BOOL SMIE_ReLocatePosition(HWND hWnd,LPHTMLHEAD lpHead);
BOOL SMIE_ReLocate_Table(HWND hWnd,LPHTMLTABLE lpTable,LPDWORD lpx,LPDWORD lpy,LPDWORD lpiMaxWidth,LPDWORD iTableHeight);
BOOL SMIE_ReLocate_TR(HWND hWnd,LPTR_TABLE tr,LPHTMLTABLE  lpTable,DWORD y);
BOOL SMIE_ReLocate_TD(HWND hWnd,LPTD_TABLE td,DWORD x,DWORD y);
BOOL SMIE_ReLocate_Object(HWND hWnd,LPHTMLOBJECT lpObject,DWORD x_Start,DWORD y_Start,RECT rect);


void SMIE_ShowHtmlTable(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,POINT ptOrg);
void SMIE_ShowTR(HWND hWnd,HDC hdc,LPTR_TABLE tr,POINT ptOrg);
void SMIE_ShowTD(HWND hWnd,HDC hdc,LPTD_TABLE td,POINT ptOrg);
void SMIE_ShowBackGround(HDC hdc,RECT rect,COLORREF color);


BOOL SMIE_GetUrlInObject(LPHTMLOBJECT lpObject,DWORD x,DWORD y,LPTSTR *lppUrl);
BOOL SMIE_GetUrlInTable(LPHTMLTABLE lpTable,DWORD x,DWORD y,LPTSTR *lppUrl);
BOOL SMIE_GetUrlInTR(LPTR_TABLE tr,DWORD x,DWORD y,LPTSTR *lppUrl);
BOOL SMIE_GetUrlInTD(LPTD_TABLE td,DWORD x,DWORD y,LPTSTR *lppUrl);


void SMIE_ReleaseTable(LPHTMLTABLE lpTable);
void SMIE_ReleaseTR(LPTR_TABLE tr);
void SMIE_ReleaseTD(LPTD_TABLE td);
void SMIE_ReleaseObject(LPHTMLOBJECT lpObject);

BOOL SMIE_GetMarkPos(HHTML hHtml,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);
static BOOL SMIE_GetMarkPosInTable(LPHTMLTABLE lpTable,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);
static BOOL SMIE_GetMarkPosInTR(LPTR_TABLE tr,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);
static BOOL SMIE_GetMarkPosInTD(LPTD_TABLE td,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);
static BOOL SMIE_GetMarkPosInObject(LPHTMLOBJECT lpObject,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);

// **************************************************
// ������HHTML SMIE_ParseHtml(HWND hWnd,char *lpHtmlBuffer)
// ������
// 	IN hWnd  -- ���ھ��
// 	IN lpHtmlBuffer  -- һ��HTML���ݵĻ���
// 
// ����ֵ���ɹ�����һ���Ѿ������õ�HTML��������򷵻�NULL
// ��������������һ��HTML���档
// ����: 
// **************************************************
HHTML SMIE_ParseHtml(HWND hWnd,char *lpHtmlBuffer)
{
	char *lpPtr=lpHtmlBuffer;
	TAGID tagID;
	BOOL bIsHead=FALSE;
	LPHTMLHEAD  lpHead;
	LPHTMLOBJECT lpCurObject,lpObject;
	CONTROLSTATE structControlState;
//	DWORD width_up;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ParseHtml\r\n")));
	// ��ʼ���ؼ��ṹ
	structControlState.color=0;
	structControlState.font=NULL;
	structControlState.url=NULL;
	structControlState.name=NULL;
	structControlState.x=0;
	structControlState.y=0;
	structControlState.bPreProcess=FALSE;

	// ����һ��HTML HEADER
	lpHead=(LPHTMLHEAD)malloc(sizeof(HTMLHEAD));  
	if (lpHead==NULL)
	{  // �����ڴ�ʧ��
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// ��ʼ���ṹ
	lpHead->next=NULL;
	lpHead->width=0;
	lpHead->height=0;

	lpCurObject=lpHead->next;

//	width_up=GetSystemMetrics(SM_CXSCREEN);

	// ��ʼ����
	while(*lpPtr)
	{
		tagID=SMIE_LocateTag(&lpPtr);  // �õ���ǩ���
		if (tagID==TAGID_NULL)   // the html Buffer Is End
		{
			break;
		}

		else if(tagID==TAGID_HTML||tagID==TAGID_HTML+TAGID_END)  // <HTML> and </HTML>
		{
			SMIE_ToNextTag(&lpPtr);
			continue;
		}

		else if (tagID==TAGID_HEAD)   // <HEAD>
		{
			//bIsHead=TRUE;
			//SMIE_ToNextTag(&lpPtr);
			SMIE_ParseHtmlHead(&lpPtr);
			continue;
		}
		/*
		else if (tagID==(TAGID_HEAD+TAGID_END))  // </HEAD>
		{
			bIsHead=FALSE;
			SMIE_ToNextTag(&lpPtr);
			continue;
		}
		if (bIsHead==TRUE)  // will to be process the head content
		{
			SMIE_ToNextTag(&lpPtr);
			continue;
		}
		*/
		if (tagID==TAGID_TABLE)
		{ // Insert a Table  <TABLE>
//			lpObject=ParseTable(&lpPtr,width_up);
			lpObject=SMIE_ParseTable(hWnd,&lpPtr);  // ����һ��TABLE
			// �������õ�TABLE���뵽��ǰ������
			if (lpCurObject==NULL)
			{
				lpHead->next=lpObject;
			}
			else
			{
				lpCurObject->next=lpObject;
			}
			lpCurObject=lpObject;
		}
		else
		{// Insert a HtmlControl
			lpObject=SMIE_ParseHtmlControl(hWnd,tagID,&lpPtr,&structControlState); // ����һ��HTML����
			if (lpObject)
			{  // ������Ƶ���ǰ������
				if (lpCurObject==NULL)
				{
					lpHead->next=lpObject;
				}
				else
				{
					lpCurObject->next=lpObject;
				}
				lpCurObject=lpObject;
			}
//			SMIE_ToNextTag(&lpPtr);
		}
	}

	// �ͷſռ�
	if (structControlState.font)
		free(structControlState.font);
	if (structControlState.url)
		free(structControlState.url);
	if (structControlState.name)
		free(structControlState.name);
	return lpHead; // ����
}

// **************************************************
// ������BOOL SMIE_ReCalcSize(HWND hWnd,HDC hdc,HHTML hHtml)  
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN hHtml -- HTML���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������HTML��Ҫ�ĳߴ硣
// ����: 
// **************************************************
BOOL SMIE_ReCalcSize(HWND hWnd,HDC hdc,HHTML hHtml)  
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;
//	HFONT font;
	DWORD width_up;
	SIZE size;

	/*
#ifndef __WCE_DEFINE
	font=CreateFont(12,6,0,0,0,
					FALSE,FALSE,FALSE,
					GB2312_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH|FF_DONTCARE,
					NULL);
	font=(HFONT)SelectObject(hdc,font);
#endif
	*/

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ReCalcSize\r\n")));

	lpHead=(LPHTMLHEAD)hHtml;
	lpCurObject=lpHead->next;

/*	while(lpCurObject)
	{
		switch(lpCurObject->type)
		{
		case TYPE_TABLE:
			SMIE_ReCalcSize_Table(hdc,(LPHTMLTABLE)lpCurObject->lpContent);  // Recalc the Control Size
			break;
		case TYPE_CONTROL:
			ReCalcSize_Control(hdc,lpCurObject->lpContent,0);  // Recalc the Control Size
			break;
		default:
			break;
		}
		lpCurObject=lpCurObject->next;
	}
	*/
	width_up=GetSystemMetrics(SM_CXSCREEN);  // �õ���Ļ��С
//	width_up=700;
	size=SMIE_ReCalcSize_Object(hWnd,hdc,lpCurObject,width_up); // �õ�Ŀ��ߴ�
	// ����Ŀ��ߴ�
	lpHead->width=size.cx;
	lpHead->height=size.cy;

	SMIE_ReLocatePosition(hWnd,lpHead);  // ���¶�λ���ؼ�λ��
/*
#ifndef __WCE_DEFINE
	font=(HFONT)SelectObject(hdc,font);
	DeleteObject(font);
#endif
*/
	return TRUE; // ���سɹ�
}
// **************************************************
// ������BOOL SMIE_ReLocatePosition(HWND hWnd,LPHTMLHEAD lpHead)
// ������
//  IN hWnd -- ���ھ��
// 	IN lpHead -- HTML ͷ�ṹ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������¶�λ�����Ƶ�λ�á�
// ����: 
// **************************************************
BOOL SMIE_ReLocatePosition(HWND hWnd,LPHTMLHEAD lpHead)
{
	LPHTMLOBJECT lpCurObject;
	DWORD x,y,width;
	RECT rect;
	
	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ReLocatePosition\r\n")));

	lpCurObject=lpHead->next;

	// ��ʼ����Ϊ(0,0)
	x=y=0;
	width=775;  // ����һ������

	// ��ʼ���ṹ
	rect.left=x;
	rect.right=rect.left+width;
	rect.top=0;
//	rect.bottom=10000;
	rect.bottom=lpHead->height;
	SMIE_ReLocate_Object(hWnd,lpCurObject,0,0,rect); // ���¶�λĿ��

/*
	while(lpCurObject)
	{
		switch(lpCurObject->type)
		{
		case TYPE_TABLE:
			SMIE_ReLocate_Table(hWnd,(LPHTMLTABLE)lpCurObject->lpContent,&x,&y,&width);  // Recalc the Control Size
			break;
		case TYPE_CONTROL:
			rect.left=x;
			rect.right=rect.left+width;
			rect.top=0;
			rect.bottom=10000;
			SMIE_ReLocate_Control(hWnd,lpCurObject->lpContent,&x,&y,rect);  // Recalc the Control Size
			break;
		case TYPE_CR:  // It will be insert a CR in here
			x=0;
			y+=14;
			break;
		default:
			break;
		}
		lpCurObject=lpCurObject->next;
	}
*/
	return TRUE;
}

// **************************************************
// ������void SMIE_ShowHtml(HWND hWnd,HDC hdc,HHTML hHtml,POINT ptOrg)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN hHtml -- HTML���
// 	IN ptOrg -- ��ʾԭ��
// 
// ����ֵ����
// ������������ʾһ��HTML��
// ����: 
// **************************************************
void SMIE_ShowHtml(HWND hWnd,HDC hdc,HHTML hHtml,POINT ptOrg)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;

//	HFONT font;
	RECT rect;
/*
#ifndef __WCE_DEFINE

	font=CreateFont(12,6,0,0,0,
					FALSE,FALSE,FALSE,
					GB2312_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH|FF_DONTCARE,
					NULL);

	font=(HFONT)SelectObject(hdc,font);
#endif
*/	
	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ShowHtml\r\n")));
//	{  // Clear the rect to be ReDraw
//		SMIE_ShowBackGround(hdc,rcPaint,RGB(0xff,0xff,0xff));
//	}
	GetClientRect(hWnd,&rect);  // �õ��ͻ�����
	SMIE_ShowBackGround(hdc,rect,RGB(0xff,0xff,0xff)); // ������

	lpHead=(LPHTMLHEAD)hHtml;  // �õ�HTMLͷ�ṹ
	if (lpHead==NULL)
		return;		
	lpCurObject=lpHead->next; // �õ���ǰ����


	while(lpCurObject)
	{
		switch(lpCurObject->type)
		{
		case TYPE_TABLE: // ��һ��TABLE
			SMIE_ShowHtmlTable(hWnd,hdc,(LPHTMLTABLE)lpCurObject->lpContent,ptOrg);  // Show Table
			break;
		case TYPE_CONTROL: // ��һ������
//			rect.left=0;
//			rect.right=10000;
//			rect.top=0;
			rect.bottom=10000;
			SMIE_ShowHtmlControl(hWnd,hdc,lpCurObject->lpContent,rect,ptOrg);  // Show  the Control 
			break;
		default:
			break;
		}
		lpCurObject=lpCurObject->next; // ����һ������
	}
/*
#ifndef __WCE_DEFINE
	font=(HFONT)SelectObject(hdc,font);
	DeleteObject(font);
#endif
*/
//#ifdef USE_TIMERTOSHOWIMG
	ShowImage(hWnd);
//#endif
}

// **************************************************
// ������void SMIE_ReleaseHtml(HHTML hHtml)
// ������
// 	IN hHtml -- HTML���
// 
// ����ֵ����
// �����������ͷ�HTML�ؼ���
// ����: 
// **************************************************
void SMIE_ReleaseHtml(HHTML hHtml)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;//,lpNext;
	

	if (hHtml==NULL)
		return;

	lpHead=(LPHTMLHEAD)hHtml; // �õ�ͷ�ṹ
	lpCurObject=lpHead->next; // �õ���ǰ����

	SMIE_ReleaseObject(lpCurObject); // �ͷŵ�ǰ����
	free(lpHead); // �ͷ�ͷ�ṹ

}

// **************************************************
// ������LPHTMLOBJECT SMIE_ParseHtmlHead(char **lpHtmlBuffer)
// ������
// 	IN/OUT lpHtmlBuffer -- ������
// 
// ����ֵ������һ��HTML����
// ��������������HTMLͷ����
// ����: 
// **************************************************
LPHTMLOBJECT SMIE_ParseHtmlHead(char **lpHtmlBuffer)
{
	char **lpPtr;
	TAGID tagID;

		JAMIMSG(DBG_FUNCTION,(TEXT("Enter ParseHtmlHead\r\n")));

		lpPtr=lpHtmlBuffer; // �õ���ǰָ��
/*
	while(1)
	{
		propertyID=SMIE_LocateProperty(lpPtr); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
			case PROPERTYID_WIDTH:  // Process property "width"
		}
	}
*/
	SMIE_ToNextTag(lpPtr); // ����һ����ǩ��ʼλ��

	// Get item Content
	while(**lpPtr)
	{
		tagID=SMIE_LocateTag(lpPtr); // �õ���ǩ���
		if (tagID==TAGID_HEAD+TAGID_END)  // </HEAD>
		{ // HTMLͷ����
			SMIE_ToNextTag(lpPtr); // ����һ����ǩ��ʼλ��
			break;
		}
		SMIE_ToNextTag(lpPtr);// ����һ����ǩ��ʼλ��
	}
	return NULL;
}


// **************************************************
// ������LPHTMLOBJECT SMIE_ParseTable(HWND hWnd,char **lpHtmlBuffer)
// ������
// 	IN hWnd -- ���ھ��
// 	IN/OUT lpHtmlBuffer -- HTML������
// 
// ����ֵ������һ�������õĿ���
// ��������������һ��TABLE��
// ����: 
// **************************************************
LPHTMLOBJECT SMIE_ParseTable(HWND hWnd,char **lpHtmlBuffer)
{
	LPHTMLOBJECT lpHtmlObject,lpControl,lpCurObject;
	LPHTMLTABLE  lpHtmlTable;
	DWORD thePart=BODY_PART;
	LPTR_TABLE lpCurHead,lpCurBody,lpCurFoot,tr;
	char **lpPtr=lpHtmlBuffer;
	PROPERTYID propertyID;
	char *pPropertyContent;
	TAGID tagID;
	DWORD i;
	CONTROLSTATE structControlState;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ParseTable\r\n")));

	// ��ʼ�����ƽṹ
	structControlState.color=0;
	structControlState.font=NULL;
	structControlState.url=NULL;
	structControlState.name=NULL;
	structControlState.x=0;
	structControlState.y=0;
	structControlState.bPreProcess=FALSE;

	// ����һ���������ݿռ�
	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
		return NULL; // �����ڴ�ʧ��

	// ����һ��Ŀ��ṹ�ռ�
	lpHtmlObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));
	if (lpHtmlObject==NULL)
	{ // �����ڴ�ʧ��
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	
	// ����һ��TABLE�ṹ�ռ�
	lpHtmlTable=(LPHTMLTABLE)malloc(sizeof(HTMLTABLE));
	if (lpHtmlTable==NULL)
	{ // ����ʧ��
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}

	// ��ʼ��Ŀ��ṹ
	lpHtmlObject->next=NULL;
	lpHtmlObject->type=TYPE_TABLE;
	lpHtmlObject->lpContent=(LPCONTENT)lpHtmlTable;

	// Initialize struct HtmlTable 
	lpHtmlTable->body=NULL;
	lpHtmlTable->foot=NULL;
	lpHtmlTable->head=NULL;
	lpHtmlTable->lpItem=NULL;
	lpHtmlTable->height=0;
	lpHtmlTable->width=0;
	lpHtmlTable->heightPreRow=0;
	lpHtmlTable->widthPreCol=0;
	lpHtmlTable->cols=0;
	lpHtmlTable->rows=0;
	lpHtmlTable->x=0;
	lpHtmlTable->y=0;
	lpHtmlTable->align=ALIGN_NONE;
	lpHtmlTable->bgColor=-1;
	lpHtmlTable->cellSpacing=2;
	lpHtmlTable->cellPadding=2;
	lpHtmlTable->bgColor=0xffffff;
	lpHtmlTable->percent=0;
	lpHtmlTable->heightSet=0;
	
	lpCurHead=lpHtmlTable->head;  // �õ���ǰTABLEͷ
	lpCurBody=lpHtmlTable->body; // �õ���ǰTABLE����
	lpCurFoot=lpHtmlTable->foot; // �õ���ǰTABLE��
	lpCurObject=lpHtmlTable->lpItem; // �õ���ǰTABLE����Ŀ

	// Get style of the table
	// ....

//	if (width_up>=PERCENTTAG)
//		width_up=0;
	while(1)
	{
		propertyID=SMIE_LocateProperty(lpPtr); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_WIDTH:  // Process property "width"
			if (SMIE_IsPercent(pPropertyContent)==TRUE)
			{
//				if (width_up)
//					lpHtmlTable->width=atoi(pPropertyContent)*width_up/100;
//				else
					lpHtmlTable->percent=PERCENTTAG+atoi(pPropertyContent);//+PERCENTTAG;
			}
			else
				lpHtmlTable->percent=atoi(pPropertyContent);
			break;
		case PROPERTYID_HEIGHT:  // Process property "Height"
			lpHtmlTable->heightSet=atoi(pPropertyContent);
			break;
		case PROPERTYID_ALIGN:  // Process property "align"
			lpHtmlTable->align=SMIE_GetAlignType(pPropertyContent);
			break;
		case PROPERTYID_BGCOLOR:// Process property "bgColor"
			lpHtmlTable->bgColor=SMIE_GetColor(pPropertyContent);
			break;
		case PROPERTYID_CELLSPACING:// Process property "cellSpacing"
			lpHtmlTable->cellSpacing=atoi(pPropertyContent);
			break;
		case PROPERTYID_CELLPADDING:// Process property "cellPadding"
			lpHtmlTable->cellPadding=atoi(pPropertyContent);
			break;
		}
	}

	JAMIMSG(DBG_FUNCTION,(TEXT("Parse Style Complete\r\n")));
	SMIE_ToNextTag(lpPtr);

	// Get item Content
	while(**lpPtr)
	{
		tagID=SMIE_NextTagIs(*lpPtr);
		if (tagID==TAGID_TABLE) // <TABLE>
		{  // This is Next TR
			break;
		}
		else if (tagID==TAGID_NULL)  // End of the buffer
		{
			break;
		}

		tagID=SMIE_LocateTag(lpPtr);
		if (tagID==TAGID_TBODY)  // <TBODY>
		{// Is Table Body
			JAMIMSG(DBG_FUNCTION,(TEXT("Parse TBody\r\n")));
			thePart=BODY_PART;
			SMIE_ToNextTag(lpPtr);
		}
		else if (tagID==TAGID_THEAD)  // <THEAD>
		{// Is Table Head
			thePart=HEAD_PART;
			SMIE_ToNextTag(lpPtr);
		}
		else if (tagID==TAGID_TFOOT)  // <TFOOT>
		{// Is Table foot
			thePart=FOOT_PART;
			SMIE_ToNextTag(lpPtr);
		}
		else if (tagID==TAGID_THEAD+TAGID_END)  // </THEAD>
		{// Is Table Body end
			thePart=BODY_PART;
			SMIE_ToNextTag(lpPtr);
		}
		else if (tagID==TAGID_TFOOT+TAGID_END)  // </TFOOT>
		{// Is Table foot end
			thePart=BODY_PART;
			SMIE_ToNextTag(lpPtr);
		}
		else if (tagID==TAGID_TR)   // <TR>
		{ // Insert a Row  
			// if thePart=BODY; Insert Body part
			// if thePart=HEAD; Insert Head part
			// if thePart=FOOT; Insert Foot part

			lpHtmlTable->rows++;  // have a new row
//			if (lpHtmlTable->width)
//				width_up=lpHtmlTable->width;
//			tr=ParseTR(lpPtr,lpHtmlTable,width_up);
			JAMIMSG(DBG_FUNCTION,(TEXT("Parse TR\r\n")));
			tr=SMIE_ParseTR(hWnd,lpPtr,lpHtmlTable);  // ����һ��TR
			if (tr)
			{
				if (thePart == BODY_PART)
				{  // ��BODY���֣���TR���뵽BODY
					if (lpCurBody==NULL)
					{
						lpHtmlTable->body=tr;
					}
					else
					{
						lpCurBody->next=tr;
					}
					lpCurBody=tr;
				}
				else if (thePart == HEAD_PART)
				{ // ��HEAD���֣���TR���뵽HEAD
					if (lpCurHead==NULL)
					{
						lpHtmlTable->head=tr;
					}
					else
					{
						lpCurHead->next=tr;
					}
					lpCurHead=tr;
				}
				else  // thePart == FOOT_PART
				{ // ��FOOT���֣���TR���뵽FOOT
					if (lpCurFoot==NULL)
					{
						lpHtmlTable->foot=tr;
					}
					else
					{
						lpCurFoot->next=tr;
					}
					lpCurFoot=tr;
				}

			}
		}
		else if (tagID==TAGID_TABLE+TAGID_END) // </TABLE>
		{  // This is Next TR
			SMIE_ToNextTag(lpPtr);
			break;
		}
		else
		{// Insert a HtmlControl
			JAMIMSG(DBG_FUNCTION,(TEXT("Parse Control\r\n")));
			lpControl=SMIE_ParseHtmlControl(hWnd,tagID,lpPtr,&structControlState);  // ����һ������
			if (lpControl)
			{  // ���뵱ǰ����
				if (lpCurObject==NULL)
				{
					lpHtmlTable->lpItem=lpControl;
				}
				else
				{
					lpCurObject->next=lpControl;
				}
				lpCurObject=lpControl;
			}
//			SMIE_ToNextTag(lpPtr);
		}
	}

	JAMIMSG(DBG_FUNCTION,(TEXT("Adjust Table\r\n")));
	SMIE_AdjustTable(lpHtmlTable); // ����TABLE

	// malloc  heightPreRow
	lpHtmlTable->heightPreRow=(LPDWORD)malloc(sizeof(DWORD)*(lpHtmlTable->rows+10)); 
	if (lpHtmlTable->heightPreRow==NULL)
	{  // �����ڴ�ʧ��
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// ��ʼ��heightPreRow
	for (i=0;i<lpHtmlTable->rows;i++)
		lpHtmlTable->heightPreRow[i]=0;

	// malloc widthPreCol
	lpHtmlTable->widthPreCol=(LPDWORD)malloc(sizeof(DWORD)*(lpHtmlTable->cols+10));
	if (lpHtmlTable->widthPreCol==NULL)
	{  // �����ڴ�ʧ��
		if (lpHtmlTable->widthPreCol)
			free(lpHtmlTable->widthPreCol);
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// ��ʼ��widthPreCol
	for (i=0;i<lpHtmlTable->cols;i++)
		lpHtmlTable->widthPreCol[i]=0;

	JAMIMSG(DBG_FUNCTION,(TEXT("Parse Table Success\r\n")));
	// �ͷŲ���Ҫ�Ŀռ�
	if (pPropertyContent)
		free(pPropertyContent);
	if (structControlState.font)
		free(structControlState.font);
	if (structControlState.url)
		free(structControlState.url);
	if (structControlState.name)
		free(structControlState.name);
	return lpHtmlObject;
}

// **************************************************
// ������LPTR_TABLE SMIE_ParseTR(HWND hWnd,char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable)
// ������
// 	IN hWnd -- ���ھ�� 
// 	IN/OUT lpHtmlBuffer -- HTML ������
// 	IN lpHtmlTable -- TABLE �ṹ
// 
// ����ֵ������һ��TR�ṹ
// ��������������TABLE �е�TR��
// ����: 
// **************************************************
LPTR_TABLE SMIE_ParseTR(HWND hWnd,char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable)
{
	LPTR_TABLE tr;
	LPTD_TABLE lpCurTD,td;
	char **lpPtr=lpHtmlBuffer;
	TAGID tagID;
	DWORD cols=0;
	PROPERTYID propertyID;
	char *pPropertyContent;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter Parse TR\r\n")));

	// ����һ���������ݿռ�
	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
	{ // �����ڴ�ʧ��
		return NULL;
	}
	// ����һ��TR�ṹ
	tr=(LPTR_TABLE)malloc(sizeof(TR_TABLE)); // malloc tr
	if (tr==NULL)  // the failure to malloc tr
	{  // �����ڴ�ʧ��
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// ��ʼ��TR�ṹ
	tr->next=NULL;
	tr->height=0;
	tr->width=0;
	tr->td=NULL;
	tr->bgColor=-1;
	lpCurTD=tr->td;  // ָ��ǰTD

//	if (width_up>=PERCENTTAG)
//		width_up=0;
	// Get style of the tr
	while(1)
	{
		propertyID=SMIE_LocateProperty(lpPtr); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_BGCOLOR:  // background color
			tr->bgColor=SMIE_GetColor(pPropertyContent);
			break;
			/*
		case PROPERTYID_WIDTH:  // Process property "width"
			if (SMIE_IsPercent(pPropertyContent)==TRUE)
			{
				if (width_up)
					tr->width=atoi(pPropertyContent)*width_up/100;
				else
					tr->width=atoi(pPropertyContent)+PERCENTTAG;
			}
			else
				tr->width=atoi(pPropertyContent);
			break;
			*/
		}
	}

	JAMIMSG(DBG_FUNCTION,(TEXT("Parse TR Style Complete\r\n")));

	SMIE_ToNextTag(lpPtr);  // ����һ����ǩλ��
	// Get td Content
	while(**lpPtr)
	{
		tagID=SMIE_NextTagIs(*lpPtr);  // �õ���һ����ǩ���
		if (tagID==TAGID_TR) // <TR>
		{  // This is Next TR
			break;
		}
		else if (tagID==TAGID_THEAD||tagID==TAGID_THEAD+TAGID_END)  //<THEAD> or </THEAD>
		{
			break;
		}
		else if (tagID==TAGID_TBODY||tagID==TAGID_TBODY+TAGID_END)  //<TBODY> or </TBODY>
		{
			break;
		}
		else if (tagID==TAGID_TFOOT||tagID==TAGID_TFOOT+TAGID_END)  //<TFOOT> or </TFOOT>
		{
			break;
		}

		tagID=SMIE_LocateTag(lpPtr);  // �õ���ǰ��ǩ���
		if (tagID==TAGID_TR+TAGID_END) // <TR>
		{  // This is Next TR
			SMIE_ToNextTag(lpPtr);  // ����һ����ǩ
			break;
		}
		if (tagID==TAGID_TD)  // <TD>
		{// Insert a TD to current Row
			cols++;  // have a new cols
//			if (tr->width)
//				width_up=tr->width;
//			td=ParseTD(lpPtr,width_up);
			JAMIMSG(DBG_FUNCTION,(TEXT("Parse TD \r\n")));
			td=SMIE_ParseTD(hWnd,lpPtr);  // ����һ��TD
			if (td!=NULL)  // have Create a new td
			{
				if (lpCurTD==NULL)  
					tr->td=td;   // the first td is NULL
				else
					lpCurTD->next=td; //the next TD is td
				lpCurTD=td;  // to the next td
			}
		}
		if (tagID==TAGID_TH)  // <TH>
		{// Insert a TD to current Row
			cols++;  // have a new cols
//			if (tr->width)
//				width_up=tr->width;
//			td=ParseTD(lpPtr,width_up);
			JAMIMSG(DBG_FUNCTION,(TEXT("Parse TD \r\n")));
			td=SMIE_ParseTD(hWnd,lpPtr);  // ����һ��TD
			if (td!=NULL)  // have Create a new td
			{
				if (lpCurTD==NULL)  
					tr->td=td;   // the first td is NULL
				else
					lpCurTD->next=td; //the next TD is td
				lpCurTD=td;  // to the next td
			}
		}
		else
		{// Insert a HtmlControl To Table
			SMIE_ToNextTag(lpPtr);  // ����һ����ǩ��λ��
//			break;
		}
	}
	// ��������
	if (lpHtmlTable->cols<cols)
		lpHtmlTable->cols=cols;
	// if the parent width is know and only one son's width is unKnow in the sons
	// then the last son's width is know 
//	if (width_up)
	if (lpHtmlTable->width)
	{  // TABLE ��ָ����� ���������TR TD �Ŀ��
		DWORD UnknowWidthNum,width;
		td=tr->td;
		UnknowWidthNum=0;
		width=0;
		while(td)
		{
			if (td->width==0)
			{
				UnknowWidthNum++;
				lpCurTD=td;
			}
			else
			{
				width+=td->width;
			}
			td=td->next;
		}
		if (UnknowWidthNum==1)
		{  // û��ָ����ȵ�TD����1�����򽫸�TD�Ŀ��Ϊʣ����
//			if (width_up>width)
//				lpCurTD->width=width_up-width;
			if (lpHtmlTable->width>width)
				lpCurTD->width=lpHtmlTable->width-width;  
		}
	}
	// �ͷ����õ��ڴ�
	if (pPropertyContent)
		free(pPropertyContent);
	return tr;  // �ɹ�����
}

// **************************************************
// ������LPTD_TABLE SMIE_ParseTD(HWND hWnd,char **lpHtmlBuffer)
// ������
// 	IN hWnd -- ���ھ�� 
// 	IN/OUT lpHtmlBuffer -- HTML ������
// 
// ����ֵ�����ط����õ�TD�ṹ
// ��������������һ��TD��
// ����: 
// **************************************************
LPTD_TABLE SMIE_ParseTD(HWND hWnd,char **lpHtmlBuffer)
{
	char **lpPtr=lpHtmlBuffer;
	LPTD_TABLE td;
	LPHTMLOBJECT lpCurObject,lpControl;
	TAGID tagID;
	CONTROLSTATE structControlState;
	PROPERTYID propertyID;
	char *pPropertyContent;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter Parse TD \r\n")));
	// �����������ݿռ�
	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
	{ // �����ڴ�ʧ��
		return NULL;
	}
	// ����һ��TD�ṹ
	td=(LPTD_TABLE)malloc(sizeof(TD_TABLE)); // malloc tr
	if (td==NULL)  // the failure to malloc tr
	{// �����ڴ�ʧ��
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// ��ʼ��TD�ṹ
	td->next=NULL;
	td->height=0;
	td->width=0;
	td->lpItem=NULL;
	td->bgColor=-1;
	td->colSpan=1;
	td->rowSpan=1;
	td->IsVirtual=FALSE;
	td->percent=0;
	td->NoWrap=FALSE;
	td->heightSet=0;


	lpCurObject=td->lpItem;  // �õ���ǰ��Ŀ��ؼ�

	// ��ʼ���ؼ�״̬�ṹ
	structControlState.color=0;
	structControlState.font=NULL;
	structControlState.url=NULL;
	structControlState.name=NULL;
	structControlState.x=0;
	structControlState.y=0;
	structControlState.bPreProcess=FALSE;

//	if (width_up>=PERCENTTAG)
//		width_up=0;
	// Get style of the tr
	while(1)
	{
		propertyID=SMIE_LocateProperty(lpPtr); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
		case PROPERTYID_BGCOLOR:  // background color
			td->bgColor=SMIE_GetColor(pPropertyContent);  // �õ���ɫֵ
			break;
		case PROPERTYID_COLSPAN: // col span
			td->colSpan=atoi(pPropertyContent);  // �õ��ߴ�
			break;
		case PROPERTYID_ROWSPAN: // row span
			td->rowSpan=atoi(pPropertyContent);  // �õ��ߴ�
			break;
		case PROPERTYID_WIDTH:  // Process property "width"
			if (SMIE_IsPercent(pPropertyContent)==TRUE)
			{  // �ǰٷֱ�����
//				if (width_up)
//					td->width=atoi(pPropertyContent)*width_up/100;
//				else
					td->percent=atoi(pPropertyContent)+PERCENTTAG; // �õ��ٷֱȣ�PERCENTTAG -- �ٷֱȱ��
			}
			else
				td->percent=atoi(pPropertyContent);  // �õ�ʵ�ʳߴ�
			break;
		case PROPERTYID_HEIGHT:  // Process property "height"
			td->heightSet=atoi(pPropertyContent);  // �õ��߶�
			break;
		case PROPERTYID_NOWRAP:  // no wrap 
			td->NoWrap=TRUE;
			break;
		}
	}

	JAMIMSG(DBG_FUNCTION,(TEXT("Parse TD Style Success\r\n")));

	SMIE_ToNextTag(lpPtr);  // ����һ����ǩ
	// Get td Content
	while(**lpPtr)
	{
		tagID=SMIE_NextTagIs(*lpPtr);  // �õ���һ����ǩ���
		if (tagID==TAGID_TD||tagID==TAGID_TH) // <TD> or <TH>
		{  // This is Next TD or TH
			break;
		}
		else if (tagID==TAGID_TR) // <TR>
		{ // This is next TR
			break;
		}
		else if (tagID==TAGID_THEAD||tagID==TAGID_THEAD+TAGID_END)  //<THEAD> or </THEAD>
		{
			break;
		}
		else if (tagID==TAGID_TBODY||tagID==TAGID_TBODY+TAGID_END)  //<TBODY> or </TBODY>
		{
			break;
		}
		else if (tagID==TAGID_TFOOT||tagID==TAGID_TFOOT+TAGID_END)  //<TFOOT> or </TFOOT>
		{
			break;
		}

		tagID=SMIE_LocateTag(lpPtr);  // �õ���ǰ�ı�ǩ���
		if (tagID==TAGID_TD+TAGID_END)  // </TD>
		{
			SMIE_ToNextTag(lpPtr);  // ����һ����ǩ
			break;
		}
		else if (tagID==TAGID_TABLE)  // <TABLE>
		{
//			if (td->width)
//			width_up=td->width;
//			lpControl=ParseTable(lpPtr,width_up);
			lpControl=SMIE_ParseTable(hWnd,lpPtr);  // ����һ��TABLE
			if (lpControl)
			{  // ����TABLE���Ƶ���ǰ����
				if (lpCurObject==NULL)
				{
					td->lpItem=lpControl;
				}
				else
				{
					lpCurObject->next=lpControl;
				}
				lpCurObject=lpControl;
			}
		}
		else
		{// Insert a HtmlControl To TD
			lpControl=SMIE_ParseHtmlControl(hWnd,tagID,lpPtr,&structControlState);  // ����һ������
			if (lpControl)
			{  // ������Ƶ���ǰ����
				if (lpCurObject==NULL)
				{
					td->lpItem=lpControl;
				}
				else
				{
					lpCurObject->next=lpControl;
				}
				lpCurObject=lpControl;
			}
		}
	}
	// �ͷŲ���Ҫ���ڴ�
	if (pPropertyContent)
		free(pPropertyContent);
	if (structControlState.font)
		free(structControlState.font);
	if (structControlState.url)
		free(structControlState.url);
	if (structControlState.name)
		free(structControlState.name);
	return td;  // �ɹ�����
}


// **************************************************
// ������BOOL SMIE_ReCalcSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE  lpTable,DWORD iTDMaxWidth,BOOL bAdjust)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpTable -- TABLE�ṹ
// 	IN iTDMaxWidth -- TD�������
// 	IN bAdjust -- �Ƿ���Ҫ���µ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������¼���TABLE�ĳߴ硣
// ����: 
// **************************************************
BOOL SMIE_ReCalcSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE  lpTable,DWORD iTDMaxWidth,BOOL bAdjust)
{
	LPTR_TABLE tr;
	LPHTMLOBJECT lpObject;
	DWORD width,height;//,widthTable;
	DWORD iRows=0,i;
//	lpHtmlTable->heightPreRow=NULL;
//	lpHtmlTable->widthPreCol=NULL;

	width=0;
	height=0;

	{  // �õ�Ĭ�ϵ�TABLE�ߴ�
		RECT rect;
			GetClientRect(hWnd,&rect);  // �õ����ڵľ��δ�С
			width=rect.right-rect.left;
			lpTable->width=width;
	}
/*
	if (lpTable->percent)
	{
		if (lpTable->percent>=PERCENTTAG)
//			lpTable->width=(lpTable->percent-PERCENTTAG)*800/100;
			lpTable->width=(lpTable->percent-PERCENTTAG)*iTDMaxWidth/100;
		else
			lpTable->width=lpTable->percent;
	}
//	else	
//		widthTable=lpTable->width;
*/

	lpTable->height=0;
	// ���¼���TABLE��HEAD
	tr=lpTable->head;  // ReCalc Size the head tr of the table
	while(tr)
	{  // ����ÿһ��TR
		SMIE_ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust);  // ���¼���TR
		lpTable->heightPreRow[iRows]=tr->height;
		iRows++;
		tr=tr->next;
	}

	// ���¼���TABLE��BODY
	tr=lpTable->body; // ReCalc Size the body tr of the table
	while(tr)
	{  // ����ÿһ��TR
		SMIE_ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust); // ���¼���TR
		lpTable->heightPreRow[iRows]=tr->height;
		iRows++;
		tr=tr->next;
	}

	// ���¼���TABLE��BODY
	tr=lpTable->foot; // ReCalc Size the foot tr of the table
	while(tr)
	{  // ����ÿһ��TR
		SMIE_ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust);  // ���¼���TR
		lpTable->heightPreRow[iRows]=tr->height;
		iRows++;
		tr=tr->next;
	}

	if (iRows>lpTable->rows)
	{
		MessageBox(NULL,TEXT("the tr row is error"),TEXT("Error"),MB_OK);
	}

/*
	for (i=0;i<lpTable->rows;i++)      // the table height is total of the row height
	{
		lpTable->heightPreRow[i]+=lpTable->cellPadding;
		height+=lpTable->heightPreRow[i]+lpTable->cellSpacing;
	}
*/
/*	AdjustSize_Table(lpTable);
	for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
	{
//		lpTable->widthPreCol[i]+=lpTable->cellPadding;
		width+=lpTable->widthPreCol[i]+lpTable->cellPadding;
	}
	width+=lpTable->cellSpacing*(lpTable->cols-1);
	*/
	/*
	if (bAdjust==TRUE)
	{
		if (lpTable->width)
		{
			for (i=0;i<lpTable->rows;i++)      // Must ReCalc the Table TR height
			{
				lpTable->heightPreRow[i]=0;
			}
			if (width>lpTable->width)
			{
				CompressSize_Table(hWnd,hdc,lpTable,width-lpTable->width,iTDMaxWidth);  // reduce the Column size
			}
			else if (width<lpTable->width)
			{
				Increase_Table(hWnd,hdc,lpTable,iTDMaxWidth);  // Increase the Column size
			}
			else
			{
				SMIE_ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
			}
		}
	}
	*/
	
	// �õ�TABLE�ĸ߶�
	height=0;
	for (i=0;i<lpTable->rows;i++)      // the table height is total of the row height
	{
//		lpTable->heightPreRow[i]+=lpTable->cellPadding;
		height+=lpTable->heightPreRow[i]+lpTable->cellPadding+lpTable->cellSpacing;
	}
/*
	width=0;
	for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
	{
//		lpTable->widthPreCol[i]+=lpTable->cellPadding;
		width+=lpTable->widthPreCol[i]+lpTable->cellPadding;
	}
	width+=lpTable->cellSpacing*(lpTable->cols-1);
*/
	/*********************/
	// calc the Object

	lpObject=lpTable->lpItem; // ReCalc Size the Control of the table
	if (lpObject)
	{
		SIZE size;

			size=SMIE_ReCalcSize_Object(hWnd,hdc,lpObject,lpTable->width);
			height+= (DWORD)size.cy;
	}

	/*********************/

//	if (lpTable->width==0)
	// �õ�TABLE�Ŀ��
	lpTable->width=width;
/*
 if (lpTable->width!=0 && lpTable->width < width)
	{
//     This condition, what do it.
//		lpTable->width=width;
	}
*/
//	if (lpTable->height==0)
	// ��TABLE�����ø߶ȱȽ�
	if (lpTable->heightSet<height)
		lpTable->height=height;
	else
		lpTable->height=lpTable->heightSet;
	return TRUE;  // �ɹ�����
}

// **************************************************
// ������void SMIE_AdjustTable(LPHTMLTABLE  lpTable)
// ������
// 	IN lpTable -- TABLE�ṹ
// 
// ����ֵ����
// ��������������TABLE��
// ����: 
// **************************************************
void SMIE_AdjustTable(LPHTMLTABLE  lpTable)
{
	LPTR_TABLE tr;
	DWORD iCol=0;

	tr=lpTable->head;  // ReCalc Size the head tr of the table
	iCol=SMIE_AdjustTR(tr);  // ����TR
	if (iCol>lpTable->cols) 
		lpTable->cols=iCol;  // �����������

	tr=lpTable->body; // ReCalc Size the body tr of the table
	iCol=SMIE_AdjustTR(tr);  // ����TR
	if (iCol>lpTable->cols)
		lpTable->cols=iCol;// �����������

	tr=lpTable->foot; // ReCalc Size the foot tr of the table
	iCol=SMIE_AdjustTR(tr);  // ����TR
	if (iCol>lpTable->cols)
		lpTable->cols=iCol; // �����������


}

// **************************************************
// ������DWORD SMIE_AdjustTR(LPTR_TABLE tr)
// ������
// 	IN tr -- TR�ṹ
// 
// ����ֵ�� ���ظ�TR������������(TD)
// ��������������TR��
// ����: 
// **************************************************
DWORD SMIE_AdjustTR(LPTR_TABLE tr)
{
	LPTR_TABLE curTR;
	LPTD_TABLE td;
	DWORD iMaxCol,iCol;
//	DWORD iMaxRow,iRow;
	DWORD i;

	iMaxCol=iCol=0;
	while(tr)
	{  //����ÿһ��TR
		td=tr->td;
		iCol=0;
		while(td)
		{  //����ÿһ��Td
			if (td->rowSpan>1)
			{  // TD��Խ����һ��TR
				// Insert a Empty TD in the next TR
				curTR=tr;
				for (i=0;i<td->rowSpan-1;i++)
				{
					if (curTR==NULL)
					{
						td->rowSpan=i;
						break;
					}
					curTR=curTR->next;
					SMIE_InsertEmptyTDInTR(curTR,iCol,td->colSpan);  // ����һ����TD
				}
			}
			iCol+=td->colSpan;  // ��TD��ռ�е�����
			td=td->next;  // ��һ��TD
		}
		if (iCol>iMaxCol)
			iMaxCol=iCol;  // �õ���������
		iCol=0;
		tr=tr->next;  //��һ��TR
	}
	return iMaxCol; //����������������
}

// **************************************************
// ������BOOL SMIE_InsertEmptyTDInTR(LPTR_TABLE tr,DWORD iInsertCol,DWORD colSpan)
// ������
// 	IN tr -- TR�ṹ
// 	IN iInsertCol -- Ҫ�����λ��
// 	IN colSpan -- Ҫ�����TD��ռ������
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// ������������ָ����TR�в���һ����TD��
// ����: 
// **************************************************
BOOL SMIE_InsertEmptyTDInTR(LPTR_TABLE tr,DWORD iInsertCol,DWORD colSpan)
{
	DWORD iCol;
	LPTD_TABLE td,newTD,pretd;

		if (tr==NULL)
			return TRUE;
		td=tr->td;
		
		iCol=0;
		pretd=td;
		// �ҵ�Ҫ�����λ��
		while(td)
		{
			if (iCol==iInsertCol)
				break;
			pretd=td;
			td=td->next;
			iCol++;
		}
		// ����һ��TD�ṹ
		newTD=(LPTD_TABLE)malloc(sizeof(TD_TABLE));
		if (newTD==NULL)
		{  // ����ʧ��
			MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
			return FALSE;
		}

		// ��ʼ���ṹ
		memset(newTD,0,sizeof(TD_TABLE));

		// ����Ҫ����TD������
		newTD->IsVirtual=TRUE;
		newTD->rowSpan=1;
		newTD->colSpan=colSpan;

		// �����µ�TD
		newTD->next=td;
		if (iCol==0)
		{
			tr->td=newTD;
		}
		else
		{
			pretd->next=newTD;
		}

		return TRUE;  // �ɹ�����
}


// **************************************************
// ������void SMIE_CompressSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD widthCompress,DWORD iTDMaxWidth)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpTable -- TABLE�ṹ
// 	IN widthCompress -- Ҫѹ���Ŀ��
// 	IN iTDMaxWidth -- TD�������
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
void SMIE_CompressSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD widthCompress,DWORD iTDMaxWidth)
{
	DWORD width,tdWidthCompress,widthPreCompresss;
	DWORD i;
	LPDWORD lpMinWidth;
	LPDWORD lpBackup;
	LPTR_TABLE tr;
	LPTD_TABLE td;
	DWORD iCol;

	if (lpTable->cols==0)
		return;
	// ����һ������������ÿһ��TD���ܴﵽ����С���
	lpMinWidth=(LPDWORD)malloc(lpTable->cols*sizeof(DWORD));
	if (lpMinWidth==NULL)
	{ // ����ʧ��
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return ;
	}
	// ����һ������������ԭ���Ŀ��
	lpBackup=(LPDWORD)malloc(lpTable->cols*sizeof(DWORD));
	if (lpBackup==NULL)
	{  // ����ʧ��
		free(lpMinWidth);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return ;
	}
	// ����ԭ���Ŀ�ȣ����õ�ÿһ��TD����С���
	memset(lpMinWidth,0,lpTable->cols*sizeof(DWORD));
	while(1)
	{
		// ����ԭ���Ŀ��
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			lpBackup[i]=lpTable->widthPreCol[i];
		}
		// �õ�ÿһ��TD����С���
		tr=lpTable->body;
		while(tr)
		{
			td=tr->td;
			iCol=0;
			while(td)
			{
				if (td->NoWrap==TRUE)
				{  // ���������NoWrap������С���Ϊ�趨�Ŀ�ȣ�����Ϊ0
					if (lpMinWidth[iCol]<td->width)
						lpMinWidth[iCol]=td->width;
				}
				iCol+=td->colSpan;
				td=td->next;
			}
			tr=tr->next;
		}
		// �õ�TABLE��������ѹ�����
		width=0;
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			width+=lpTable->widthPreCol[i]-lpMinWidth[i];  // �õ�ÿһ��TD��ѹ���Ŀ�ȣ���ǰ��� - ��С���
		}

		if (width==0)
		{  // ������ѹ�����˳�
			free(lpMinWidth);
			free(lpBackup);
			return;
		}
		// ��ʼѹ��
		for (i=0;i<lpTable->cols;i++)		
		{
			// �õ�ÿһ��TD��Ҫѹ���Ŀ�ȣ�����ѹ�����ƽ�����䵽ÿһ��TD
			tdWidthCompress=widthCompress*(lpTable->widthPreCol[i]-lpMinWidth[i])/width;
			if (lpTable->widthPreCol[i]<tdWidthCompress)
				lpTable->widthPreCol[i]=0;
			else
				lpTable->widthPreCol[i]-=tdWidthCompress;
		}
		
		// ���¼���TABLE�Ŀ��
		SMIE_ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
		// �ٴι��˵�һЩ����ѹ����TD�����¼�����С�Ŀ��
		for (i=0;i<lpTable->cols;i++)		
		{
			tdWidthCompress=widthCompress*(lpBackup[i]-lpMinWidth[i])/width;
			if (lpTable->widthPreCol[i]==0||(lpBackup[i]-lpTable->widthPreCol[i])<tdWidthCompress)
				lpMinWidth[i]=lpTable->widthPreCol[i];  
			else
				lpMinWidth[i]=0;
		}
		//�鿴�Ƿ���Ҫѹ��
		for (i=0;;i++)
		{
			if (i==lpTable->cols)
			{  // ����Ҫ��ѹ�����˳�
				free(lpMinWidth);
				free(lpBackup);
				return ;
			}
			if (lpMinWidth[i]!=0)
				break;
		}
		// �鿴�Ƿ���ѹ��
		for (i=0;;i++)
		{
			if (i==lpTable->cols)
			{  // ������ѹ�����˳�
				free(lpMinWidth);
				free(lpBackup);
				return ;
			}
			if (lpBackup[i]!=lpTable->widthPreCol[i])
				break;
		}
		if (width<widthPreCompresss)  // ���ѹ�����С�ڿ�ѹ���Ŀ�ȣ��˳�
			break;
		
		// �õ�TABLE���ܿ��
		width=0;
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			width+=lpTable->widthPreCol[i];
		}
		width+=lpTable->cellSpacing*(lpTable->cols-1);
		
		widthPreCompresss=width;  // ����TABLE���
		if (width<lpTable->width)
			break;

		widthCompress=width-lpTable->width;  // ���µõ�ѹ�����
	}
	free(lpMinWidth);
	free(lpBackup);
	// ��ɷ���
}

// **************************************************
// ������void SMIE_Increase_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD iTDMaxWidth)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpTable -- TABLE�ṹ
// 	IN iTDMaxWidth -- TD�������
// 
// ����ֵ����
// �����������Ŵ�TABLE�Ŀ��
// ����: 
// **************************************************
void SMIE_Increase_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD iTDMaxWidth)
{
	DWORD width,widthCompress,tdWidthCompress;
	DWORD i;

	if (lpTable->cols==0)
		return;
	// �õ���ǰTABLE�Ŀ��
	width=0;
	for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
	{
		width+=lpTable->widthPreCol[i];
	}
	width+=lpTable->cellSpacing*(lpTable->cols-1);

	// �õ�Ҫ�Ŵ�Ŀ��
	widthCompress=lpTable->width-width;

	if (width!=0)
	{  // �õ���ǰTABLE�Ŀ�Ȳ�Ϊ0
		// ����ÿһ��TDԭ���Ĵ�С����������Ŵ�ĳߴ�
		for (i=0;i<lpTable->cols;i++)		
		{
			tdWidthCompress=widthCompress*lpTable->widthPreCol[i]/width;
			lpTable->widthPreCol[i]+=tdWidthCompress;
		}
	}
	else
	{  // ��ǰTABLE�Ŀ��Ϊ0
		// ƽ������ÿһ��TD�Ŀ��
		widthCompress /=lpTable->cols;
		for (i=0;i<lpTable->cols;i++)		
		{
			lpTable->widthPreCol[i]+=widthCompress;
		}
	}	
//	SMIE_ReCalcSize_Table(hdc,lpTable,lpTable->width,FALSE);
	// ���¼���TABLE�Ĵ�С
	SMIE_ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
}

// **************************************************
// ������BOOL SMIE_ReCalcSize_TR(HWND hWnd,HDC hdc,LPTR_TABLE tr,LPHTMLTABLE  lpTable,BOOL bAdjust)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN tr -- TR�ṹ
// 	IN lpTable -- TR����TABLE�ṹ
// 	IN bAdjust -- �Ƿ���Ҫ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������¼���TR�ĳߴ硣
// ����: 
// **************************************************
BOOL SMIE_ReCalcSize_TR(HWND hWnd,HDC hdc,LPTR_TABLE tr,LPHTMLTABLE  lpTable,BOOL bAdjust)
{
	LPTD_TABLE td;
	DWORD width,height;
	DWORD iCols=0;
	DWORD iTDMaxWidth,tdWidth;
//	DWORD i;

		width=0;
		height=0;

//		if (tr->width)
//			iTDMaxWidth=tr->width;
//		else
		// �õ�TABLE�����ߴ�
		iTDMaxWidth=lpTable->width;
//		if (tr->width>=PERCENTTAG)
//			tr->width=(tr->width-PERCENTTAG)*iTDMaxWidth/100;
/*
		td=tr->td;
		while(td)
		{
			if (td->percent)
			{
				if (td->percent>=PERCENTTAG)
				{
					td->width=(td->percent-PERCENTTAG)*iTDMaxWidth/100;
				}
				else
				{
					td->width=td->percent;
				}
			}
			td=td->next;
		}
*/
		td=tr->td;
//		tdWidth=iTDMaxWidth;  // add by Jami in 2001.07.03
		while(td)
		{
			// ���¼���ÿһ��TD�ĳߴ�
/*
			if (bAdjust==FALSE)
			{
				tdWidth=0;
				for (i=iCols;i<iCols+td->colSpan;i++)
					tdWidth+=lpTable->widthPreCol[i];

				if (td->width==tdWidth)
				{  // Not need ReCalc Size
					if (td->rowSpan==1)
					{
						if (height<td->height)  // The Heght is max height of all td with the tr
							height=td->height;  
					}
					width+=tdWidth;       // the width of the tr is total width all td with the tr
					if (iTDMaxWidth>tdWidth)
						iTDMaxWidth-=tdWidth;
					iCols+=td->colSpan;
					td=td->next;
					continue;
				}
				if(td->NoWrap==FALSE)
					td->width=tdWidth;
*/
/*
				else if (td->percent)
				{
					if (td->percent<PERCENTTAG)
					{
						td->width=td->percent;
						tdWidth=td->width;
					}
				}
*/
//			}

//			else
//			{
			tdWidth=iTDMaxWidth;  
//			}
			SMIE_ReCalcSize_TD(hWnd,hdc,td,tdWidth);  // Recalc the td Size
//			if (td->rowSpan==1)
//			{
//				if (height<td->height)  // The Heght is max height of all td with the tr
			height+=td->height;  
//			}
/*
			if (td->colSpan==1)
			{
				if (lpTable->widthPreCol[iCols]<td->width)
					lpTable->widthPreCol[iCols]=td->width;
				tdWidth=lpTable->widthPreCol[iCols];       // the width of the tr is total width all td with the tr
			}
			else
			{
				tdWidth=0;
				for (i=iCols;i<iCols+td->colSpan;i++)
					tdWidth+=lpTable->widthPreCol[i];
				if (tdWidth<td->width)
				{
//					lpTable->widthPreCol[iCols+td->colSpan-1]+=td->width-tdWidth; // the last col width is free width
					tdWidth=td->width;
				}
			}
			width+=tdWidth;       // the width of the tr is total width all td with the tr
			if (iTDMaxWidth>tdWidth)
				iTDMaxWidth-=tdWidth;
			iCols+=td->colSpan;
			td=td->next;
		*/
			td=td->next;  // ����һ��TD
		}
		//�õ�TR�Ŀ�Ⱥ͸߶�
		width=tdWidth;       // the width of the tr is total width all td with the tr
		tr->width=width;
		tr->height=height;
		return TRUE; // �ɹ�����
}
// **************************************************
// ������BOOL SMIE_ReCalcSize_TD(HWND hWnd,HDC hdc,LPTD_TABLE td,DWORD iTDMaxWidth)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN td -- TD�ṹ
// 	IN iTDMaxWidth -- TD�������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������¼���TD�ĳߴ硣
// ����: 
// **************************************************
BOOL SMIE_ReCalcSize_TD(HWND hWnd,HDC hdc,LPTD_TABLE td,DWORD iTDMaxWidth)
{
/*
	LPHTMLOBJECT lpObject;
	DWORD width,height,widthTD;
	DWORD iLastRowHeight=0,ControlHeight;
	BOOL IsNewRow;
	DWORD iRemainWidth;

	width=0;
	widthTD=0;
	height=0;
	IsNewRow=FALSE;
	iRemainWidth=iTDMaxWidth;

	lpObject=td->lpItem;

	if (iTDMaxWidth==0)
		return FALSE;

	while(lpObject)
	{
		switch(lpObject->type)
		{
		case TYPE_TABLE:
			SMIE_ReCalcSize_Table(hdc,(LPHTMLTABLE)lpObject->lpContent);  // Recalc the Table Size
			width=((LPHTMLTABLE)(lpObject->lpContent))->width;    // Get Table width
//			if (iLastRowHeight<((LPHTMLTABLE)(lpObject->lpContent))->height)  // Get Table 
			iLastRowHeight=((LPHTMLTABLE)(lpObject->lpContent))->height;   // Get Table Height
			if (widthTD<width)		// charge the table width and the TD width
				widthTD=width;
			height+=iLastRowHeight;   //the TD height must add the table height
			iLastRowHeight=0;         // this line height is zero (will enter a new line)
//			IsNewRow=TRUE;
			break;
		case TYPE_CONTROL:
//			ReCalcSize_Control(hdc,lpObject->lpContent,iRemainWidth);  // Recalc the Control Size
			ReCalcSize_Control(hdc,lpObject->lpContent,0);  // Recalc the Control Size
			width=((LPHTMLCONTROL)(lpObject->lpContent))->width;   // Get Control width
			if (iRemainWidth>width)			// if this control can display in this line
			{  // can show complete in this line
				iRemainWidth-=width;   // get new free width in this line
				widthTD+=width;		   // this TD width must add this control width	
				ControlHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;  // get the control height
			}
			else // must show to the next line
			{
				ControlHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;  // get the control height pre line
				while(iRemainWidth<width)  // can't display in this line ,then display in next line
				{
					iRemainWidth+=iTDMaxWidth;   // the total width with lines
					ControlHeight+=((LPHTMLCONTROL)(lpObject->lpContent))->height;  // add a control line height
				}
				iRemainWidth-=width;  // get new free width in the last line
				width=iTDMaxWidth;  // the control width is Maxwidth
				widthTD=width;      // the TD width is Control width
			}
			if(iLastRowHeight<ControlHeight)   // if the current line height is small with the control line height
				iLastRowHeight=ControlHeight;  // set new line height
			break;
		case TYPE_CR:   // Is a CR
			if (iLastRowHeight)  // Exist line height
				height+=iLastRowHeight;   
			else  // Is a empty line
				height+=14;
			iLastRowHeight=0;
			IsNewRow=FALSE;
//			IsNewRow=TRUE;
//			iRemainWidth=iTDMaxWidth;
			break;
		default:
			MessageBox(NULL,"Have Unknow TYPE Exist","Error",MB_OK);
			break;
		}
		lpObject=lpObject->next;
	}

	height+=iLastRowHeight;

	if (td->width==0)
		td->width=widthTD;
	if (td->height==0)
		td->height=height;
	return TRUE;
	*/
	SIZE size;
	LPHTMLOBJECT lpObject;

	if (td->IsVirtual==TRUE)  // ����һ�����TD������Ҫ���ǳߴ�
		return TRUE;

	lpObject=td->lpItem;

//	if (td->width)
//		iTDMaxWidth=td->width;

	td->height=0;

	size=SMIE_ReCalcSize_Object(hWnd,hdc,lpObject,iTDMaxWidth);  // ���¼���Ŀ��ĳߴ�
	td->width=iTDMaxWidth;
//	if (td->width<size.cx)
//		td->width=size.cx;
	// �õ�TD�ĳߴ�
	if (td->heightSet<(DWORD)size.cy)
		td->height=(DWORD)size.cy;
	else
		td->height=td->heightSet;
	return TRUE;  // �ɹ�����

}


// **************************************************
// ������SIZE SMIE_ReCalcSize_Object(HWND hWnd,HDC hdc,LPHTMLOBJECT lpObject,DWORD iTDMaxWidth)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpObject -- Ŀ����ƵĽṹ
// 	IN iTDMaxWidth -- �����
// 
// ����ֵ������Ŀ����Ƶĳߴ�
// �������������¼���Ŀ����Ƶĳߴ�
// ����: 
// **************************************************
SIZE SMIE_ReCalcSize_Object(HWND hWnd,HDC hdc,LPHTMLOBJECT lpObject,DWORD iTDMaxWidth)
{
	DWORD width,height,widthTD,widthLine;
	DWORD iLastRowHeight=0;//,ControlHeight;
	BOOL IsNewRow;
	DWORD iCurPos;
	SIZE size={0,0};
	RECT rect;
	DWORD iHeightLine;
	HHTMCTL lpPreControl;
	BOOL isTextControl;

	
	iHeightLine=SMIE_GetFontHeight(hWnd,NULL);// �õ�����ĸ߶�
	width=0;	  // current object width
	widthTD=0;	  // the all line max width
	widthLine=0;  // current line width
	height=0;
	IsNewRow=FALSE;

//	iRemainWidth=iTDMaxWidth;
	iCurPos=0;
	lpPreControl=NULL;


	//if (iRemainWidth==0)
	//	iRemainWidth=MAXWIDTH;

	while(lpObject)
	{ // ����ÿһ�����Ƶĳߴ�
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // ��ǰ������һ��TABLE
			height+=iLastRowHeight;   
			SMIE_ReCalcSize_Table(hWnd,hdc,(LPHTMLTABLE)lpObject->lpContent,iTDMaxWidth,TRUE);  // Recalc the Table Size
			width=((LPHTMLTABLE)(lpObject->lpContent))->width;    // Get Table width
//			if (iLastRowHeight<((LPHTMLTABLE)(lpObject->lpContent))->height)  // Get Table 
			iLastRowHeight=((LPHTMLTABLE)(lpObject->lpContent))->height;   // Get Table Height
			if (widthTD<width)		// charge the table width and the TD width
				widthTD=width;
			height+=iLastRowHeight;   //the TD height must add the table height
			iLastRowHeight=0;         // this line height is zero (will enter a new line)
			widthLine=0;
//			IsNewRow=TRUE;
			lpPreControl=NULL;
			break;
		case TYPE_CONTROL: // ��ǰ������һ��һ�����
//			ReCalcSize_Control(hdc,lpObject->lpContent,iRemainWidth);  // Recalc the Control Size
//			ReCalcSize_Control(hdc,lpObject->lpContent,0);  // Recalc the Control Size
			GetClientRect(hWnd,&rect); // �õ����ڵĳߴ�
			widthLine=rect.right-rect.left;
			rect.left=0;
			rect.top=0;
//			if (iTDMaxWidth==0)
//				rect.right=rect.left+MAXWIDTH;
//			else
//			if (iTDMaxWidth!=0)
//				rect.right=rect.left+iTDMaxWidth;
			rect.bottom=MAXHEIGHT;

			isTextControl = IsTextControl((LPHTMLCONTROL)(lpObject->lpContent));

			if (isTextControl == FALSE)
			{
				height+=iLastRowHeight;   //the TD height must add the table height
				iCurPos = 0;
				iLastRowHeight = 0;
			}

			SMIE_ReCalcSize_Control(hWnd,hdc,(HHTMCTL)lpObject->lpContent,iCurPos,rect);  // Recalc the Control Size

			iLastRowHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;
			if (isTextControl == FALSE)
			{
				height+=iLastRowHeight;   //the TD height must add the table height
				iCurPos = 0;
				iLastRowHeight = 0;
			}
			else
			{
				height+=(iLastRowHeight - ((LPHTMLCONTROL)(lpObject->lpContent))->heightLine);   //the TD height must add the table height
				if (((LPHTMLCONTROL)(lpObject->lpContent))->height<=((LPHTMLCONTROL)(lpObject->lpContent))->heightLine)
				{
					iCurPos += ((LPHTMLCONTROL)(lpObject->lpContent))->widthLastRow;
				}
				else
				{
					iCurPos = ((LPHTMLCONTROL)(lpObject->lpContent))->widthLastRow;
					// Add By Jami chen in 2005.03.01
					iLastRowHeight = ((LPHTMLCONTROL)(lpObject->lpContent))->heightLine; // ���һ���ı��ĸ߶�
				}
			}
/*
			iLastRowHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;
//			widthLine=((LPHTMLCONTROL)(lpObject->lpContent))->width;
			height+=iLastRowHeight;   //the TD height must add the table height
			iLastRowHeight=0;
*/
/*
			if (((LPHTMLCONTROL)(lpObject->lpContent))->image)
			{
				if (((LPHTMLCONTROL)(lpObject->lpContent))->width<(rect.right-iCurPos))
				{
					iCurPos+=((LPHTMLCONTROL)(lpObject->lpContent))->widthLastRow;
					widthLine=iCurPos;
				}
				else
				{
					height+=iLastRowHeight;
					iCurPos=((LPHTMLCONTROL)(lpObject->lpContent))->widthLastRow;
					widthLine=((LPHTMLCONTROL)(lpObject->lpContent))->width;
				}
			}
			else
			{
				if (((LPHTMLCONTROL)(lpObject->lpContent))->height<=((LPHTMLCONTROL)(lpObject->lpContent))->heightLine)
				{
					iCurPos+=((LPHTMLCONTROL)(lpObject->lpContent))->widthLastRow;
					widthLine=iCurPos;
				}
				else
				{
					if (lpPreControl)
					{
						height+=((LPHTMLCONTROL)(lpObject->lpContent))->height-((LPHTMLCONTROL)(lpObject->lpContent))->heightLine+iLastRowHeight-((LPHTMLCONTROL)lpPreControl)->heightLine;
					}
					else
					{

						height+=((LPHTMLCONTROL)(lpObject->lpContent))->height-((LPHTMLCONTROL)(lpObject->lpContent))->heightLine;//+iLastRowHeight;
					}

					iCurPos=((LPHTMLCONTROL)(lpObject->lpContent))->widthLastRow;
					widthLine=((LPHTMLCONTROL)(lpObject->lpContent))->width;
				}
			}
			ControlHeight=((LPHTMLCONTROL)(lpObject->lpContent))->heightLine;
			if(iLastRowHeight<ControlHeight)   // if the current line height is small with the control line height
				iLastRowHeight=ControlHeight;  // set new line height
				*/
/*
			width=((LPHTMLCONTROL)(lpObject->lpContent))->width;   // Get Control width
			if (iRemainWidth>width||iTDMaxWidth==0)			// if this control can display in this line
			{  // can show complete in this line
				iRemainWidth-=width;   // get new free width in this line
				widthLine+=width;		   // this TD width must add this control width	
				ControlHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;  // get the control height
			}
			else // must show to the next line
			{
				ControlHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;  // get the control height pre line
				while(iRemainWidth<width)  // can't display in this line ,then display in next line
				{
					iRemainWidth+=iTDMaxWidth;   // the total width with lines
					ControlHeight+=((LPHTMLCONTROL)(lpObject->lpContent))->height;  // add a control line height
				}
				iRemainWidth-=width;  // get new free width in the last line
				widthLine=iRemainWidth;
				width=iTDMaxWidth;  // the control width is Maxwidth
				widthTD=width;      // the TD width is Control width
			}
*/
//			if(iLastRowHeight<ControlHeight)   // if the current line height is small with the control line height
//				iLastRowHeight=ControlHeight;  // set new line height
			if (widthLine>widthTD)
				widthTD=widthLine;
			lpPreControl=lpObject->lpContent;
			break;
		case TYPE_CR:   // Is a CR
//			if (iLastRowHeight)  // Exist line height
			height+=iLastRowHeight;   // �������һ�еĸ߶�
//			else  // Is a empty line
//				height+=14;
			iLastRowHeight=iHeightLine;  // �������һ�еĸ߶�Ϊһ������߶�
			IsNewRow=FALSE;
//			IsNewRow=TRUE;
//			iRemainWidth=iTDMaxWidth;
			iCurPos=0;
			widthLine=0;
			lpPreControl=NULL;
			break;
		case TYPE_HEAD:   // Only say This is a Head
			break;
		default:
			MessageBox(NULL,TEXT("Have Unknow TYPE Exist"),TEXT("Error"),MB_OK);
			break;
		}
		lpObject=lpObject->next;
	}

	height+=iLastRowHeight;
//	if (widthTD==498)
//		size.cx=0;
	// ����Ŀ����Ƶĳߴ�	
	size.cx=widthTD;
	size.cy=height;
	return size; // �ɹ�����
}
// **************************************************
// ������void SMIE_AdjustSize_Table(LPHTMLTABLE  lpTable)
// ������
// 	IN lpTable -- TABLE�ṹ
// 
// ����ֵ����
// �������������µ���TABLE�ĳߴ硣
// ����: 
// **************************************************
void SMIE_AdjustSize_Table(LPHTMLTABLE  lpTable)
{
	LPTR_TABLE tr;

	tr=lpTable->head;  // ReCalc Size the head tr of the table
	SMIE_AdjustSize_TR(tr,lpTable);

	tr=lpTable->body; // ReCalc Size the body tr of the table
	SMIE_AdjustSize_TR(tr,lpTable);

	tr=lpTable->foot; // ReCalc Size the foot tr of the table
	SMIE_AdjustSize_TR(tr,lpTable);

}

// **************************************************
// ������void SMIE_AdjustSize_TR(LPTR_TABLE tr,LPHTMLTABLE  lpTable)
// ������
// 	IN tr -- TR�ṹ
// 	IN lpTable -- TABLE�ṹ
// 
// ����ֵ����
// �������������µ���TR�ĳߴ硣
// ����: 
// **************************************************
void SMIE_AdjustSize_TR(LPTR_TABLE tr,LPHTMLTABLE  lpTable)
{
	LPTD_TABLE td;
	DWORD iRow,iHeight=0;
	DWORD iCols;//,tdWidth;
	DWORD i;

	iRow=0;
	while(tr)
	{  // ����ÿһ��TR�ĳߴ�
		td=tr->td; //�õ�TR�ĵ�һ��TD
		iCols=0;
		while(td)
		{   // ����ÿһ��TD
			if (td->rowSpan>1)
			{  // ��TD��Խ����һ��TR
				iHeight=0;
				for (i=iRow;i<iRow+td->rowSpan;i++)
				{  // ����TD���ܸ߶�
					iHeight+=lpTable->heightPreRow[i];
				}
				if (iHeight<td->height)  // �ܸ߶�С��TD���趨����������һ�еĸ߶�
					lpTable->heightPreRow[iRow+td->rowSpan-1]+=td->height-iHeight;  // the last row is the ramain height
			}
/*
			if (td->colSpan>1)
			{
				tdWidth=0;
				for (i=iCols;i<iCols+td->colSpan;i++)
					tdWidth+=lpTable->widthPreCol[i];
				if ((td->percent!=0&&td->percent<PERCENTTAG)||tdWidth==0)
				{
					if (tdWidth<td->width)
					{
						lpTable->widthPreCol[iCols+td->colSpan-1]+=td->width-tdWidth; // the last col width is free width
					}
				}
			}
*/
			iCols+=td->colSpan;  // ����������
			td=td->next; // ָ����һ��TD
		}
		tr=tr->next;  //ָ����һ��TR
		iRow++;
	}
}


// **************************************************
// ������BOOL SMIE_ReLocate_Table(HWND hWnd,LPHTMLTABLE lpTable,LPDWORD lpx,LPDWORD lpy,LPDWORD lpiMaxWidth,LPDWORD iTableHeight)
// ������
//  IN hWnd -- ���ھ��
// 	IN lpTable -- TABLE�ṹ
// 	IN/OUT lpx -- TABLE��Ŀ�ʼXλ�ã���������һ�����Ƶ�λ��
// 	IN/OUT lpy -- TABLE��Ŀ�ʼYλ�ã���������һ�����Ƶ�λ��
// 	IN/OUT lpiMaxWidth  -- ����
// 	IN/OUT iTableHeight -- TABLE�ĸ߶�
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������¶�λTABLE��
// ����: 
// **************************************************
BOOL SMIE_ReLocate_Table(HWND hWnd,LPHTMLTABLE lpTable,LPDWORD lpx,LPDWORD lpy,LPDWORD lpiMaxWidth,LPDWORD iTableHeight)
{
	LPTR_TABLE tr;
	LPHTMLOBJECT lpObject;
	DWORD x,y;
	RECT rect;
/*
	if (lpTable->align==ALIGN_LEFT)
	{
		lpTable->x=*lpx;
		lpTable->y=*lpy;
		*lpx+=lpTable->width;
		*lpiMaxWidth=*lpiMaxWidth-lpTable->width;
		if (*iTableHeight<lpTable->height)
			*iTableHeight=lpTable->height;
	}
	else if (lpTable->align==ALIGN_RIGHT)
	{
		lpTable->x=*lpx+*lpiMaxWidth-lpTable->width;
		lpTable->y=*lpy;
		*lpiMaxWidth=*lpiMaxWidth-lpTable->width;
//		*lpx+=lpTable->width;
		if (*iTableHeight<lpTable->height)
			*iTableHeight=lpTable->height;
	}
	else
*/
	{
		lpTable->x=*lpx;  // �趨TABLE�Ŀ�ʼλ��
		lpTable->y=*lpy;
//		if (*iTableHeight>lpTable->height)
//			*lpy+=*iTableHeight;
//		else
		*lpy+=lpTable->height;  // ������һ�����Ƶ�λ��
		*iTableHeight=0;
	}


	x=lpTable->x;
	y=lpTable->y;
	tr=lpTable->head;  // ReLocate the head tr of the table
	while(tr)
	{
		SMIE_ReLocate_TR(hWnd,tr,lpTable,y);  // ���¶�λTR
		y+=tr->height;    // �õ���һ��TR��λ��
		y+=lpTable->cellSpacing;
		tr=tr->next;    // ָ����һ��TR
	}

	tr=lpTable->body; // ReLocate the body tr of the table
	while(tr)
	{
		SMIE_ReLocate_TR(hWnd,tr,lpTable,y);  // ���¶�λTR
		y+=tr->height;    // �õ���һ��TR��λ��
		y+=lpTable->cellSpacing;
		tr=tr->next;    // ָ����һ��TR
	}

	tr=lpTable->foot; // ReLocate the foot tr of the table
	while(tr)
	{
		SMIE_ReLocate_TR(hWnd,tr,lpTable,y);  // ���¶�λTR
		y+=tr->height;    // �õ���һ��TR��λ��
		y+=lpTable->cellSpacing;
		tr=tr->next;   // ָ����һ��TR
	}

	lpObject=lpTable->lpItem; // ReLocate the Control of the table

	rect.left=lpTable->x;
	rect.right=rect.left+lpTable->width;
	rect.top=lpTable->y;
	rect.bottom=rect.top+lpTable->height;
	SMIE_ReLocate_Object(hWnd,lpObject,x,y,rect);  // ���¶�λ����

	return TRUE;
}


// **************************************************
// ������BOOL SMIE_ReLocate_TR(hWnd,LPTR_TABLE tr,LPHTMLTABLE  lpTable,DWORD y)
// ������
//  IN hWnd -- ���ھ��
// 	IN tr -- TR�ṹ
// 	IN lpTable -- TABLE�ṹ
// 	IN y -- ��ǰTR�Ŀ�ʼY����
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// �������������¶�λTR��λ�á�
// ����: 
// **************************************************
BOOL SMIE_ReLocate_TR(HWND hWnd,LPTR_TABLE tr,LPHTMLTABLE  lpTable,DWORD y)
{
	DWORD x;
	LPTD_TABLE td;
	DWORD iCols=0;
	//int i;

	x=lpTable->x;  // �õ�TR��X����
	tr->x=x;  // ����TR������
	tr->y=y;
//	y=y;


		td=tr->td;  //�õ���һ��TD
		while(td)
		{  // ��λÿһ��TD
			SMIE_ReLocate_TD(hWnd,td,x,y);  // Renew Locate the td Position
/*			for (i=0;i<td->colSpan;i++)
			{
				x+=lpTable->widthPreCol[iCols];
				x+=lpTable->cellSpacing;
				iCols++;
			}
			*/
			y+=td->height;  // �õ���һ��TD������
			td=td->next;  // �õ���һ��TD
		}
		return TRUE;
}

// **************************************************
// ������BOOL SMIE_ReLocate_TD(hWnd,LPTD_TABLE td,DWORD x_Start,DWORD y_Start)
// ������
//  IN hWnd -- ���ھ��
// 	IN td -- TD�ṹ
//	 IN x_Start -- TD�Ŀ�ʼX����
//	 IN y_Start -- TD�Ŀ�ʼY����
// 
// ����ֵ���ɹ�����TRUE,���򷵻�FALSE
// �������������¶�λTD��λ�á�
// ����: 
// **************************************************
BOOL SMIE_ReLocate_TD(HWND hWnd,LPTD_TABLE td,DWORD x_Start,DWORD y_Start)
{
	LPHTMLOBJECT lpObject;
	DWORD x,y;
//	DWORD width;
	DWORD iTableHeight=0;
	DWORD iLastRowHeight=0;
	RECT rect;


	if (td->IsVirtual==TRUE)  // ��TD��һ����TD������Ҫ���д���
		return TRUE;
	lpObject=td->lpItem;  // ָ���һ������


	x=x_Start;
	y=y_Start;

	td->x=x;  // ����TD�Ŀ�ʼλ��
	td->y=y;

	// ����TD�������Ŀ��Ƶķ�Χ
	rect.left=td->x;
	rect.top=td->y;
	rect.right=td->x+td->width;
	rect.bottom=td->y+td->height;
	SMIE_ReLocate_Object(hWnd,lpObject,x_Start,y_Start,rect);  // ���¶�λ����
	return TRUE; // �ɹ�����
}

// **************************************************
// ������BOOL SMIE_ReLocate_Object(HWND hWnd,LPHTMLOBJECT lpObject,DWORD x_Start,DWORD y_Start,RECT rect)
// ������
//	IN hWnd -- ���ھ��
// 	IN lpObject -- Ŀ����ƽṹ
// 	IN x_Start -- Ŀ����ƵĿ�ʼX����
// 	IN y_Start -- Ŀ����ƵĿ�ʼY����
// 	IN rect -- ���Ƶķ�Χ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������¶�λĿ����ơ�
// ����: 
// **************************************************
BOOL SMIE_ReLocate_Object(HWND hWnd,LPHTMLOBJECT lpObject,DWORD x_Start,DWORD y_Start,RECT rect)
{
	DWORD x,y;
	DWORD width;
	DWORD iTableHeight=0;
	DWORD iLastRowHeight=0,iCurTabHeight=0;
	DWORD iHeightLine;
	HHTMCTL lpPreControl;
	BOOL isTextControl;

	x=x_Start;  // �õ���ʼ����
	y=y_Start;
	
	iHeightLine=SMIE_GetFontHeight(hWnd,NULL);  // �õ�����߶�
	width=rect.right-rect.left;  // �õ����Ƶ������
//	if (width==0)
//		return TRUE;
	lpPreControl=NULL;
	while(lpObject)
	{  // ��λÿһ������
		switch(lpObject->type)
		{
		case TYPE_TABLE: // Ŀ�������һ��TABLE

			if (iLastRowHeight)
			{
				x=x_Start;
				y+=iLastRowHeight;  // ��λ���ϴο��Ƶ�����
			}

			SMIE_ReLocate_Table(hWnd,(LPHTMLTABLE)lpObject->lpContent,&x,&y,&width,&iCurTabHeight);  // Recalc the Control Size 
			iLastRowHeight=0;  
			lpPreControl=NULL;
			break;
		case TYPE_CONTROL:  // Ŀ�������һ��һ�����
			if (width==0)
				break;
			isTextControl = IsTextControl((LPHTMLCONTROL)(lpObject->lpContent));

			if (isTextControl == FALSE)
			{
				x=x_Start;
				y+=iLastRowHeight;  // ��λ���ϴο��Ƶ�����
				iLastRowHeight=0;
			}
//			SMIE_ReLocate_Control(hWnd,lpObject->lpContent,&x,&y,rect,lpPreControl);  // Recalc the Control Size
			SMIE_ReLocate_Control(hWnd,(HHTMCTL)lpObject->lpContent,&x,&y,rect,iLastRowHeight);  // Recalc the Control Size
			if(iLastRowHeight<((LPHTMLCONTROL)(lpObject->lpContent))->heightLine)
				iLastRowHeight=((LPHTMLCONTROL)(lpObject->lpContent))->heightLine;
//			iLastRowHeight=0;
			lpPreControl=lpObject->lpContent;  // ������ǰ���ƣ����ܻ�Ӱ�쵽�������ƵĶ�λ
			if (isTextControl == FALSE)
			{
				y+=iLastRowHeight;   //the TD height must add the table height
				x=x_Start;
				iLastRowHeight=0;
			}
			break;
		case TYPE_CR:  // Ŀ�������һ���س�
//			if (iLastRowHeight)
			y+=iLastRowHeight;  // ��λ���ϴο��Ƶ��·�
//			else
//				y+=14;
			x=x_Start;
			iLastRowHeight=iHeightLine;
			lpPreControl=NULL;
			break;
		default:
			break;
		}
		lpObject=lpObject->next;  // �õ���һ������
	}
	return TRUE;
}


// **************************************************
// ������void SMIE_ShowHtmlTable(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,POINT ptOrg)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpTable -- TABLE�ṹ
// 	IN ptOrg  -- ��ʾԭ��
// 
// ����ֵ����
// ������������ʾTABLE��
// ����: 
// **************************************************
void SMIE_ShowHtmlTable(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,POINT ptOrg)
{
	LPTR_TABLE tr;
	LPHTMLOBJECT lpObject;
	COLORREF bgOldColor;


//	if (lpTable->x>rcPaint.right||lpTable->y>rcPaint.bottom)
//	{  // the rcPaint in the over the table
//		return;
//	}

//
	{
		RECT rect;
			
			GetClientRect(hWnd,&rect);  // �õ����ھ���
//			if (((lpTable->y-ptOrg.y)>rect.bottom)||((lpTable->y-ptOrg.y+lpTable->height)<rect.top))
			if ((lpTable->y>(DWORD)(ptOrg.y+rect.bottom))||((lpTable->y+lpTable->height)<(DWORD)(rect.top+ptOrg.y)))
				return;  // ��ǰTABLE���ڴ����ڣ�����Ҫ��ʾ�����ء�
	}
	if (lpTable->bgColor!=-1)
	{
		// �����ñ�����ɫ����������ɫ
/*
		HBRUSH hBrush=(HBRUSH)CreateSolidBrush(lpTable->bgColor);
			hBrush=(HBRUSH)SelectObject(hdc,hBrush);
			SetTextColor(hdc,lpTable->bgColor);
			
			  (hdc,lpTable->x-iMove*600,
						  lpTable->y-iPage*400,
						  lpTable->x+lpTable->width-iMove*600,
						  lpTable->y+lpTable->height-iPage*400);
			hBrush=(HBRUSH)SelectObject(hdc,hBrush);
			DeleteObject(hBrush);
			bgOldColor=SetBkColor(hdc,lpTable->bgColor);
			*/
		RECT rect;
		// �õ�TABLE����ʾ����
		rect.left=lpTable->x-ptOrg.x;
		rect.top=lpTable->y-ptOrg.y;
		rect.right=lpTable->x-ptOrg.x+lpTable->width;
		rect.bottom=lpTable->y-ptOrg.y+lpTable->height;
		SMIE_ShowBackGround(hdc,rect,lpTable->bgColor);   // ������
		bgOldColor=SetBkColor(hdc,lpTable->bgColor);  // �����豸�ı���
	}
//	
	tr=lpTable->head;  // ReLocate the head tr of the table
	while(tr)
	{
		SMIE_ShowTR(hWnd,hdc,tr,ptOrg);  // ��ʾTR
		tr=tr->next;
	}

	tr=lpTable->body; // ReLocate the body tr of the table
	while(tr)
	{
		SMIE_ShowTR(hWnd,hdc,tr,ptOrg); // ��ʾTR
		tr=tr->next;
	}

	tr=lpTable->foot; // ReLocate the foot tr of the table
	while(tr)
	{
		SMIE_ShowTR(hWnd,hdc,tr,ptOrg); // ��ʾTR
		tr=tr->next;
	}

	lpObject=lpTable->lpItem; // ReLocate the Control of the table
	while(lpObject)
	{
		if (lpObject->type==TYPE_CONTROL)
		{
			RECT rect;

			// �õ����Ƶľ���
			rect.left=lpTable->x-ptOrg.x;
			rect.right=rect.left+lpTable->width;
			rect.top=lpTable->y-ptOrg.y;
			rect.bottom=rect.top+lpTable->height;

			SMIE_ShowHtmlControl(hWnd,hdc,lpObject->lpContent,rect,ptOrg); // ��ʾ����
		}
		lpObject=lpObject->next;  // �õ���һ������
	}
	if (lpTable->bgColor!=-1)
	{  // �ָ��豸�ı���
		SetBkColor(hdc,bgOldColor);
	}

}

// **************************************************
// ������void SMIE_ShowTR(HWND hWnd,HDC hdc,LPTR_TABLE tr,POINT ptOrg)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN tr -- TR�ṹ
// 	IN ptOrg  -- ��ʾԭ��
// 
// ����ֵ����
// ������������ʾTR��
// ����: 
// **************************************************
void SMIE_ShowTR(HWND hWnd,HDC hdc,LPTR_TABLE tr,POINT ptOrg)
{
	LPTD_TABLE td;
	COLORREF bgOldColor;

		{
			RECT rect;
				
				GetClientRect(hWnd,&rect);   // �õ����ھ���
//				if (((tr->y-ptOrg.y)>rect.bottom)||((tr->y-ptOrg.y+tr->height)<rect.top))
				if ((tr->y>(DWORD)(ptOrg.y+rect.bottom))||((tr->y+tr->height)<(DWORD)(rect.top+ptOrg.y)))
					return;  //��ǰTR������ʾ��Χ֮�ڣ�����Ҫ��ʾ����
		}

		if (tr->bgColor!=-1)
		{  // ��Ҫ���ñ���
/*
			HBRUSH hBrush=(HBRUSH)CreateSolidBrush(tr->bgColor);
				hBrush=(HBRUSH)SelectObject(hdc,hBrush);
				SetTextColor(hdc,tr->bgColor);
				Rectangle(hdc,tr->x-iMove*600,
							  tr->y-iPage*400,
							  tr->x+tr->width-iMove*600,
							  tr->y+tr->height-iPage*400);
				hBrush=(HBRUSH)SelectObject(hdc,hBrush);
				DeleteObject(hBrush);
				bgOldColor=SetBkColor(hdc,tr->bgColor);
*/
		RECT rect;
		// �õ�TR�ķ�Χ
		rect.left=tr->x-ptOrg.x;
		rect.top=tr->y-ptOrg.y;
		rect.right=tr->x-ptOrg.x+tr->width;
		rect.bottom=tr->y-ptOrg.y+tr->height;
		SMIE_ShowBackGround(hdc,rect,tr->bgColor);  // ��ʾ����
		bgOldColor=SetBkColor(hdc,tr->bgColor);  // �����豸����ı���

		}
		td=tr->td;  //�õ���һ��TD
		while(td)
		{  
			SMIE_ShowTD(hWnd,hdc,td,ptOrg);  // Renew Locate the td Position
			td=td->next;  // �õ���һ��TD
		}
		if (tr->bgColor!=-1)
		{  // �ָ��豸����
			SetBkColor(hdc,bgOldColor);
		}
}

// **************************************************
// ������void SMIE_ShowTD(HWND hWnd,HDC hdc,LPTD_TABLE td,POINT ptOrg)
// ������
// 	IN hWnd  -- ���ھ��
// 	IN hdc -- �豸���
// 	IN td -- TD�ṹ
// 	IN ptOrg -- ��ʾԭ��
// 
// ����ֵ����
// ������������ʾTD��
// ����: 
// **************************************************
void SMIE_ShowTD(HWND hWnd,HDC hdc,LPTD_TABLE td,POINT ptOrg)
{
	LPHTMLOBJECT lpObject;

	COLORREF bgOldColor;
	RECT rect;

	{
		RECT rect;
			
			GetClientRect(hWnd,&rect);  // �õ����ڿͻ�����
//			if (((td->y-ptOrg.y)>rect.bottom)||((td->y-ptOrg.y+td->height)<rect.top))
			if ((td->y>(DWORD)(ptOrg.y+rect.bottom))||((td->y+td->height)<(DWORD)(rect.top+ptOrg.y)))
				return; // ��ǰTD������ʾ��Χ�ڣ�����Ҫ��ʾ����
	}

	if (td->IsVirtual==TRUE)  // ��ǰTD����ģ�����Ҫ��ʾ
		return ;

//	GetClientRect(hWnd,&rect);
	// �õ�TD�ķ�Χ
	rect.left=td->x-ptOrg.x;
	rect.top=td->y-ptOrg.y;
	rect.right=td->x-ptOrg.x+td->width;
	rect.bottom=td->y-ptOrg.y+td->height;

	if (td->bgColor!=-1)
	{  // ��ʾ����
		SMIE_ShowBackGround(hdc,rect,td->bgColor);
		bgOldColor=SetBkColor(hdc,td->bgColor); // �����豸����
	}
	lpObject=td->lpItem;  // �õ���һ������

	while(lpObject)
	{  // ��ʾ��������
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // ��ǰ������һ��TABLE
			SMIE_ShowHtmlTable(hWnd,hdc,(LPHTMLTABLE)lpObject->lpContent,ptOrg);  // ��ʾTABLE
			break;
		case TYPE_CONTROL:  // ��һ��һ�����
			SMIE_ShowHtmlControl(hWnd,hdc,lpObject->lpContent,rect,ptOrg);  // Show Html Control 
			break;
		default:
			break;
		}
		lpObject=lpObject->next;  // �õ���һ������
	}
	if (td->bgColor!=-1)
	{  // �ָ��豸����
		SetBkColor(hdc,bgOldColor);
	}
}


// **************************************************
// ������void SMIE_ReleaseTable(LPHTMLTABLE lpTable)
// ������
// 	IN lpTable -- TABLE�ṹ
// 
// ����ֵ����
// �����������ͷ�TABLE�ṹ��
// ����: 
// **************************************************
void SMIE_ReleaseTable(LPHTMLTABLE lpTable)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	tr=lpTable->head;  // Release the head tr of the table
	while(tr)
	{
		nextTR=tr->next;  // �õ���һ��TR
		SMIE_ReleaseTR(tr);  // �ͷ�TR
		tr=nextTR;
	}

	tr=lpTable->body; // Release the body tr of the table
	while(tr)
	{
		nextTR=tr->next;  // �õ���һ��TR
		SMIE_ReleaseTR(tr); // �ͷ�TR
		tr=nextTR;
	}

	tr=lpTable->foot; // Release the foot tr of the table
	while(tr)
	{
		nextTR=tr->next;   // �õ���һ��TR
		SMIE_ReleaseTR(tr); // �ͷ�TR
		tr=nextTR;
	}

	lpObject=lpTable->lpItem; // Release the Control of the table
	SMIE_ReleaseObject(lpObject);  // �ͷſ���

	if (lpTable->heightPreRow!=0)
		free(lpTable->heightPreRow);  // �ͷŸ߶�����
	if (lpTable->widthPreCol!=0)
		free(lpTable->widthPreCol);   // �ͷſ������
	free(lpTable);  // �ͷ�TABLE�ṹ
	return ;
}

// **************************************************
// ������void SMIE_ReleaseTR(LPTR_TABLE tr)
// ������
// 	IN tr  -- TR�ṹ
// 
// ����ֵ����
// �����������ͷ�TR�ṹ
// ����: 
// **************************************************
void SMIE_ReleaseTR(LPTR_TABLE tr)
{
	LPTD_TABLE td,lpNext;
//	COLORREF bgOldColor;

		td=tr->td;  // �õ���һ��TD
		while(td)
		{
			lpNext=td->next;  // �õ���һ��TD
			SMIE_ReleaseTD(td);  // �ͷ�TD
			td=lpNext;
		}
		free(tr);  // �ͷ�TR�ṹ
}

// **************************************************
// ������void SMIE_ReleaseTD(LPTD_TABLE td)
// ������
// 	IN td  -- TD�ṹ
// 
// ����ֵ����
// �����������ͷ�TD�ṹ
// ����: 
// **************************************************
void SMIE_ReleaseTD(LPTD_TABLE td)
{
	LPHTMLOBJECT lpObject;
//	LPTD_TABLE lpNext;

//	while(td)
//	{
//		lpNext=td->next;
		lpObject=td->lpItem;  // �õ���һ������
		if (lpObject)
			SMIE_ReleaseObject(lpObject);  // �ͷſ���
		free(td);  // �ͷ�TD�ṹ
//		td=lpNext;
//	}
}

// **************************************************
// ������void SMIE_ReleaseObject(LPHTMLOBJECT lpObject)
// ������
// 	IN lpObject -- Ŀ����ƽṹ
// 
// ����ֵ����
// �����������ͷ�Ŀ����ƽṹ��
// ����: 
// **************************************************
void SMIE_ReleaseObject(LPHTMLOBJECT lpObject)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;  // �õ���һ��Ŀ��
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // ��ǰĿ����TABLE
			SMIE_ReleaseTable((LPHTMLTABLE)lpObject->lpContent);  // Release Table
			break;
		case TYPE_CONTROL:  // ��ǰĿ����һ������
			SMIE_ReleaseControl(lpObject->lpContent);  // Release Control 
			break;
		default:
			break;
		}
		free(lpObject);  // �ͷ�Ŀ��
		lpObject=lpNext;  // ָ����һ��Ŀ��
	}
}


// **************************************************
// ������void SMIE_ShowBackGround(HDC hdc,RECT rect,COLORREF color)
// ������
// 	IN  hdc -- �豸���
// 	IN  rect -- Ҫ���ľ���
// 	IN  color -- ָ���ı�����ɫ
// 
// ����ֵ����
// ������������ʾ������
// ����: 
// **************************************************
void SMIE_ShowBackGround(HDC hdc,RECT rect,COLORREF color)
{
	HBRUSH hBrush=(HBRUSH)CreateSolidBrush(color); // ��ָ������ɫ����ˢ��
//		hBrush=(HBRUSH)SelectObject(hdc,hBrush);
//		if (color)
		FillRect(hdc,&rect,hBrush);  // ��ˢ��������
 		DeleteObject(hBrush);  // ɾ��ˢ��
}



// **************************************************
// ������BOOL SMIE_GetUrl(HHTML hHtml,DWORD x,DWORD y,LPTSTR *lppUrl)
// ������
// 	IN hHtml  -- HTML���
// 	IN x  -- ָ����X����
// 	IN y  -- ָ����Y����
// 	OUT lppUrl -- ����ָ������ҳ��ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰ����ҳ��ָ��λ�õĿ�����ָ�����ַ��
// ����: 
// **************************************************
BOOL SMIE_GetUrl(HHTML hHtml,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;
	
	lpHead=(LPHTMLHEAD)hHtml;  // �õ�HEAD

	if (lpHead==NULL)
		return FALSE;
	lpCurObject=lpHead->next;  // �õ���ǰĿ��

	return SMIE_GetUrlInObject(lpCurObject,x,y,lppUrl);  // �ڵ�ǰĿ���еõ�ָ��λ�õ���ַ
}


// **************************************************
// ������BOOL SMIE_GetUrlInObject(LPHTMLOBJECT lpObject,DWORD x,DWORD y,LPTSTR *lppUrl)
// ������
// 	IN lpObject -- ָ����Ŀ��
// 	IN x  -- ָ����X����
// 	IN y  -- ָ����Y����
// 	OUT lppUrl -- ����ָ������ҳ��ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ�ָ����Ŀ����ָ��λ�õĿ�����ָ�����ַ��
// ����: 
// **************************************************
BOOL SMIE_GetUrlInObject(LPHTMLOBJECT lpObject,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;  // �õ���һ��Ŀ��
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // ��ǰĿ����TABLE
			if(SMIE_GetUrlInTable((LPHTMLTABLE)lpObject->lpContent,x,y,lppUrl)==TRUE)  // ��TABLE�еõ�
			{  // ָ��λ���ڵ�ǰTABLE�����سɹ�
				return TRUE;
			}
			break;
		case TYPE_CONTROL:  // ��ǰĿ����һ������
			if (SMIE_GetUrlInControl((HHTMCTL)lpObject->lpContent,x,y,lppUrl)==TRUE) // ��CONTROL�в���
			{  // ָ��λ���ڵ�ǰCONTROL�����سɹ�
				return TRUE;
			}
			break;
		default:
			break;
		}
		lpObject=lpNext;  // ָ����һ��Ŀ��
	}
	return FALSE;  // ����ʧ��
}


// **************************************************
// ������BOOL SMIE_GetUrlInTable(LPHTMLTABLE lpTable,DWORD x,DWORD y,LPTSTR *lppUrl)
// ������
// 	IN lpTable -- TABLE�ṹ
// 	IN x  -- ָ����X����
// 	IN y  -- ָ����Y����
// 	OUT lppUrl -- ����ָ������ҳ��ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ�ָ����TABLE��ָ��λ�õĿ�����ָ�����ַ��
// ����: 
// **************************************************
BOOL SMIE_GetUrlInTable(LPHTMLTABLE lpTable,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	if (lpTable->x>x || x>lpTable->x+lpTable->width)
		return FALSE;  // ָ��λ�ò���TABLE��

	if (lpTable->y>y || y>lpTable->y+lpTable->height)
		return FALSE;  // ָ��λ�ò���TABLE��

	tr=lpTable->head;  // Search the head tr of the table
	while(tr)
	{
		nextTR=tr->next;  // �õ���һ��TR
		if (SMIE_GetUrlInTR(tr,x,y,lppUrl)==TRUE)  // ��TR�в���
		{  // ��ǰλ���ڵ�ǰTR�У����سɹ�
			return TRUE;
		}
		tr=nextTR;  // ָ����һ��TR
	}

	tr=lpTable->body; // Search the body tr of the table
	while(tr)
	{
		nextTR=tr->next; // �õ���һ��TR
		if (SMIE_GetUrlInTR(tr,x,y,lppUrl)==TRUE)  // ��TR�в���
		{  // ��ǰλ���ڵ�ǰTR�У����سɹ�
			return TRUE;
		}
		tr=nextTR;  // ָ����һ��TR
	}

	tr=lpTable->foot; // Search the foot tr of the table
	while(tr)
	{
		nextTR=tr->next;  // �õ���һ��TR
		if (SMIE_GetUrlInTR(tr,x,y,lppUrl)==TRUE)  // ��TR�в���
		{  // ��ǰλ���ڵ�ǰTR�У����سɹ�
			return TRUE;
		}
		tr=nextTR;  // ָ����һ��TR
	}

	lpObject=lpTable->lpItem; // Search the Control of the table
	if (SMIE_GetUrlInObject(lpObject,x,y,lppUrl)==TRUE)  // ��CONTROL�в���
	{   // ��ǰλ���ڵ�ǰCONTROL�У����سɹ�
		return TRUE;
	}

	return FALSE;  // ����ʧ��  
}


// **************************************************
// ������BOOL SMIE_GetUrlInTR(LPTR_TABLE tr,DWORD x,DWORD y,LPTSTR *lppUrl)
// ������
// 	IN tr -- TR�ṹ
// 	IN x  -- ָ����X����
// 	IN y  -- ָ����Y����
// 	OUT lppUrl -- ����ָ������ҳ��ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ�ָ����TR��ָ��λ�õĿ�����ָ�����ַ��
// ����: 
// **************************************************
BOOL SMIE_GetUrlInTR(LPTR_TABLE tr,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPTD_TABLE td,lpNext;

	// !!! delete by jami chen in 2002.01.09 ,because the td have colSpan and rowSpan >1, then
	// the td->width > tr->witdh and td->height > tr->height ,so ...
//		if (tr->x>x || x>tr->x+tr->width)
//			return FALSE;

//		if (tr->y>y || y>tr->y+tr->height)
//			return FALSE;
	// !!! delete end by Jami chen 2002.01.09
		td=tr->td; //  �õ���һ��TD
		while(td)
		{
			lpNext=td->next;   // �õ���һ��TD
			if (SMIE_GetUrlInTD(td,x,y,lppUrl)==TRUE)  // ��TD�в���
			{  // ָ��λ���ڵ�ǰTD�У����سɹ�
				return TRUE;
			}
			td=lpNext;  // ָ����һ��TD
		}
		return FALSE;  // ����ʧ��
}


// **************************************************
// ������BOOL SMIE_GetUrlInTD(LPTD_TABLE td,DWORD x,DWORD y,LPTSTR *lppUrl)
// ������
// 	IN td -- TD�ṹ
// 	IN x  -- ָ����X����
// 	IN y  -- ָ����Y����
// 	OUT lppUrl -- ����ָ������ҳ��ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ�ָ����TD��ָ��λ�õĿ�����ָ�����ַ��
// ����: 
// **************************************************
BOOL SMIE_GetUrlInTD(LPTD_TABLE td,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLOBJECT lpObject;

	if (td->x>x || x>td->x+td->width)
		return FALSE;  // ָ��λ�ò��ڵ�ǰTD��

	if (td->y>y || y>td->y+td->height)
		return FALSE;  // ָ��λ�ò��ڵ�ǰTD��
	lpObject=td->lpItem;  // �õ�Ŀ��
	if (lpObject)
	{
		if (SMIE_GetUrlInObject(lpObject,x,y,lppUrl)==TRUE)  // ��ָ��Ŀ���в���
		{  // ָ��λ���ڵ�ǰĿ���У����سɹ�
			return TRUE;
		}
	}
	return FALSE;  // ����ʧ��
}

// **************************************************
// ������BOOL SMIE_GetHtmlSize(HHTML hHtml,LPSIZE lpSize)
// ������
// 	IN hHtml -- HTML���
// 	OUT lpSize -- ���ص�ǰHTML�ĳߴ�
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰHTML�ĳߴ�
// ����: 
// **************************************************
BOOL SMIE_GetHtmlSize(HHTML hHtml,LPSIZE lpSize)
{
	LPHTMLHEAD lpHead;

		if(lpSize==NULL)  // Test the Parameter Correct
			return FALSE;
		lpHead=(LPHTMLHEAD)hHtml;

		lpSize->cx=lpHead->width;  // �õ���ǰHTML�ĳߴ�
		lpSize->cy=lpHead->height;

		return TRUE;
}
// **************************************************
// ������BOOL SMIE_GetMarkPos(HHTML hHtml,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// ������
// 	IN  hHtml  -- HTML���
// 	OUT lpxPos -- ��ű��X�����ָ��
// 	OUT lpyPos -- ��ű��Y�����ָ��
// 	IN  lpMark -- ָ���ı��
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// ������������ָ����HTML�в���ָ����ǵ�λ�á�
// ����: 
// **************************************************
BOOL SMIE_GetMarkPos(HHTML hHtml,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;
	
	lpHead=(LPHTMLHEAD)hHtml;  // �õ�HTML��ͷ

	if (lpHead==NULL)
		return FALSE;
	lpCurObject=lpHead->next;  // �õ���ǰĿ��

	return SMIE_GetMarkPosInObject(lpCurObject,lpxPos,lpyPos,lpMark);  // ��Ŀ���в��ұ��
}
// **************************************************
// ������BOOL SMIE_GetMarkPosInTable(LPHTMLTABLE lpTable,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// ������
// 	IN  lpTable -- TABLE�ṹ
// 	OUT lpxPos -- ��ű��X�����ָ��
// 	OUT lpyPos -- ��ű��Y�����ָ��
// 	IN  lpMark -- ָ���ı��
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// ������������ָ����TABLE�в���ָ����ǵ�λ�á�
// ����: 
// **************************************************
BOOL SMIE_GetMarkPosInTable(LPHTMLTABLE lpTable,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	tr=lpTable->head;  // Search the head tr of the table
	while(tr)
	{
		nextTR=tr->next;  // �õ���һ��TR
		if (SMIE_GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)  // ��TR�в���
		{  // �Ѿ��ҵ������سɹ�
			return TRUE;
		}
		tr=nextTR;  // ָ����һ��TR
	}

	tr=lpTable->body; // Search the body tr of the table
	while(tr)
	{
		nextTR=tr->next;  // �õ���һ��TR
		if (SMIE_GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)  // ��TR�в���
		{  // �Ѿ��ҵ������سɹ�
			return TRUE;
		}
		tr=nextTR;  // ָ����һ��TR
	}

	tr=lpTable->foot; // Search the foot tr of the table
	while(tr)
	{
		nextTR=tr->next; // �õ���һ��TR
		if (SMIE_GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)  // ��TR�в���
		{  // �Ѿ��ҵ������سɹ�
			return TRUE;
		}
		tr=nextTR;   // ָ����һ��TR
	}

	lpObject=lpTable->lpItem; // Search the Control of the table
	if (SMIE_GetMarkPosInObject(lpObject,lpxPos,lpyPos,lpMark)==TRUE)  // ��Ŀ���в���
	{  // �Ѿ��ҵ������سɹ�
		return TRUE;
	}

	return FALSE;  // ����ʧ��
}
// **************************************************
// ������BOOL SMIE_GetMarkPosInTR(LPTR_TABLE tr,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// ������
// 	IN  tr -- TR�ṹ
// 	OUT lpxPos -- ��ű��X�����ָ��
// 	OUT lpyPos -- ��ű��Y�����ָ��
// 	IN  lpMark -- ָ���ı��
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// ������������ָ����TR�в���ָ����ǵ�λ�á�
// ����: 
// **************************************************
BOOL SMIE_GetMarkPosInTR(LPTR_TABLE tr,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPTD_TABLE td,lpNext;

		td=tr->td;  // �õ���һ��TD
		while(td)
		{
			lpNext=td->next;  // �õ���һ��TD
			if (SMIE_GetMarkPosInTD(td,lpxPos,lpyPos,lpMark)==TRUE)  // ��TD�в���
			{  // �Ѿ��ҵ����ɹ�����
				return TRUE;
			}
			td=lpNext;  // ָ����һ��TD
		}
		return FALSE;  // ����ʧ��
}
// **************************************************
// ������BOOL SMIE_GetMarkPosInTD(LPTD_TABLE td,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// ������
// 	IN  td -- TD�ṹ
// 	OUT lpxPos -- ��ű��X�����ָ��
// 	OUT lpyPos -- ��ű��Y�����ָ��
// 	IN  lpMark -- ָ���ı��
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// ������������ָ����TD�в���ָ����ǵ�λ�á�
// ����: 
// **************************************************
BOOL SMIE_GetMarkPosInTD(LPTD_TABLE td,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLOBJECT lpObject;

	lpObject=td->lpItem;  // �õ�Ŀ��
	if (lpObject)
	{
		if (SMIE_GetMarkPosInObject(lpObject,lpxPos,lpyPos,lpMark)==TRUE)  // ��Ŀ���в���
		{ // �Ѿ��鵽�����سɹ�
			return TRUE;
		}
	}
	return FALSE;  // ����ʧ��
}
// **************************************************
// ������BOOL SMIE_GetMarkPosInObject(LPHTMLOBJECT lpObject,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// ������
// 	IN  lpObject -- OBJECT�ṹ
// 	OUT lpxPos -- ��ű��X�����ָ��
// 	OUT lpyPos -- ��ű��Y�����ָ��
// 	IN  lpMark -- ָ���ı��
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// ������������ָ����Ŀ���в���ָ����ǵ�λ�á�
// ����: 
// **************************************************
BOOL SMIE_GetMarkPosInObject(LPHTMLOBJECT lpObject,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;  // �õ���һ��Ŀ��
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // ��ǰĿ����TABLE
			if(SMIE_GetMarkPosInTable((LPHTMLTABLE)lpObject->lpContent,lpxPos,lpyPos,lpMark)==TRUE)  // ��TABLE�в���
			{  // �Ѿ��ҵ������سɹ�
				return TRUE;
			}
			break;
		case TYPE_CONTROL:  // ��ǰĿ����CONTROL
			if (SMIE_GetMarkPosInControl((LPHTMLTABLE)lpObject->lpContent,lpxPos,lpyPos,lpMark)==TRUE)  // ��CONTROL�в���
			{  // �Ѿ��ҵ������سɹ�
				return TRUE;
			}
			break;
		default:
			break;
		}
		lpObject=lpNext;  // ָ����һ��Ŀ��
	}
	return FALSE;  // ����ʧ��
}

// !!! Add By Jami chen in 2003.09.09
// **************************************************
// ������HHTML SMIE_LoadImage(HWND hWnd,LPTSTR lpImageFile)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpImageFile -- Ҫװ�ص�ͼ���ļ�
// 
// ����ֵ������һ��HTML���
// ����������װ��һ��ͼ���ļ�����ǰHTML�С�
// ����: 
// **************************************************
HHTML SMIE_LoadImage(HWND hWnd,LPTSTR lpImageFile)
{
	LPHTMLHEAD  lpHead;

		lpHead=(LPHTMLHEAD)malloc(sizeof(HTMLHEAD));  // ����һ��HTMLͷ�ṹ
		if (lpHead==NULL)
		{
			MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
			return NULL;
		}
		lpHead->next=SMIE_ControlLoadImage(hWnd,lpImageFile);  // װ��ͼ���ļ���CONTROL��
		if (lpHead->next)
		{
			SIZE size;
			GetControlSize(hWnd,lpHead->next->lpContent,&size);
			lpHead->width = size.cx;  // �õ���ǰͼ��ĳߴ�
			lpHead->height = size.cy;
		}
		else
		{
			// û�гɹ�����ͼ��
			lpHead->width = 0;  
			lpHead->height = 0;
		}
		return lpHead;  // ���ؾ��
}
// !!! Add By Jami chen in 2003.09.09
