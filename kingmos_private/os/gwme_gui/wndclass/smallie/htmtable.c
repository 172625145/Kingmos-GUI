#include "eHtmView.h"
//#include "HtmTable.h"
//#include "HtmlLocate.h"
//#include "HtmlControl.h"
#include "SIETable.h"
#include "Locate.h"
#include "Control.h"
#include "ViewDef.h"

//extern DWORD iPage,iMove;

#define PERCENTTAG  10000
//#define MAXWIDTH 10000
#define MAXHEIGHT 20000

LPHTMLOBJECT ParseHtmlHead(char **lpHtmlBuffer);
//LPHTMLOBJECT ParseTable(char **lpHtmlBuffer,DWORD width_up);
LPHTMLOBJECT ParseTable(HWND hWnd,char **lpHtmlBuffer);
//LPTR_TABLE ParseTR(char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable,DWORD width_up);
LPTR_TABLE ParseTR(HWND hWnd,char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable);
//LPTD_TABLE ParseTD(char **lpHtmlBuffer,DWORD width_up);
LPTD_TABLE ParseTD(HWND hWnd,char **lpHtmlBuffer);

void AdjustTable(LPHTMLTABLE  lpTable);
DWORD AdjustTR(LPTR_TABLE tr);
BOOL InsertEmptyTDInTR(LPTR_TABLE tr,DWORD iInsertCol,DWORD colSpan);



//BOOL ReCalcSize(HWND hWnd,HDC hdc,LPHTMLHEAD lpHead);
BOOL ReCalcSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE  lpTable,DWORD iTDMaxWidth,BOOL bAdjust);
BOOL ReCalcSize_TR(HWND hWnd,HDC hdc,LPTR_TABLE tr,LPHTMLTABLE  lpTable,BOOL bAdjust);
//BOOL ReCalcSize_TD(HDC hdc,LPTD_TABLE td,LPHTMLTABLE  lpTable);
BOOL ReCalcSize_TD(HWND hWnd,HDC hdc,LPTD_TABLE td,DWORD iTDMaxWidth);
SIZE ReCalcSize_Object(HWND hWnd,HDC hdc,LPHTMLOBJECT lpObject,DWORD iTDMaxWidth);

void AdjustSize_Table(LPHTMLTABLE  lpTable);
void AdjustSize_TR(LPTR_TABLE tr,LPHTMLTABLE  lpTable);

void CompressSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD widthCompress,DWORD iTDMaxWidth);
void Increase_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD iTDMaxWidth);


BOOL ReLocatePosition(HWND hWnd,LPHTMLHEAD lpHead);
BOOL ReLocate_Table(HWND hWnd,LPHTMLTABLE lpTable,LPDWORD lpx,LPDWORD lpy,LPDWORD lpiMaxWidth,LPDWORD iTableHeight);
BOOL ReLocate_TR(HWND hWnd,LPTR_TABLE tr,LPHTMLTABLE  lpTable,DWORD y);
BOOL ReLocate_TD(HWND hWnd,LPTD_TABLE td,DWORD x,DWORD y);
BOOL ReLocate_Object(HWND hWnd,LPHTMLOBJECT lpObject,DWORD x_Start,DWORD y_Start,RECT rect);


//void ShowHtmlTable(HDC hdc,LPHTMLTABLE lpTable,RECT rcPaint);
//void ShowTR(HDC hdc,LPTR_TABLE tr,RECT rcPaint);
//void ShowTD(HDC hdc,LPTD_TABLE td,RECT rcPaint);
void ShowHtmlTable(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,POINT ptOrg);
void ShowTR(HWND hWnd,HDC hdc,LPTR_TABLE tr,POINT ptOrg);
void ShowTD(HWND hWnd,HDC hdc,LPTD_TABLE td,POINT ptOrg);
void ShowBackGround(HDC hdc,RECT rect,COLORREF color);


BOOL GetUrlInObject(LPHTMLOBJECT lpObject,DWORD x,DWORD y,LPTSTR *lppUrl);
BOOL GetUrlInTable(LPHTMLTABLE lpTable,DWORD x,DWORD y,LPTSTR *lppUrl);
BOOL GetUrlInTR(LPTR_TABLE tr,DWORD x,DWORD y,LPTSTR *lppUrl);
BOOL GetUrlInTD(LPTD_TABLE td,DWORD x,DWORD y,LPTSTR *lppUrl);


void ReleaseTable(LPHTMLTABLE lpTable);
void ReleaseTR(LPTR_TABLE tr);
void ReleaseTD(LPTD_TABLE td);
void ReleaseObject(LPHTMLOBJECT lpObject);


BOOL GetMarkPos(HHTML hHtml,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);
static BOOL GetMarkPosInTable(LPHTMLTABLE lpTable,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);
static BOOL GetMarkPosInTR(LPTR_TABLE tr,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);
static BOOL GetMarkPosInTD(LPTD_TABLE td,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);
static BOOL GetMarkPosInObject(LPHTMLOBJECT lpObject,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);

void DisplayRect(LPTSTR title,int x,int y,int width,int height)
{
	EdbgOutputDebugString( title);
	EdbgOutputDebugString( "%d,%d,%d,%d,%d,%d",x,y,x+width,y+height,width,height);
}

BOOL ChecktdIntr(LPTR_TABLE tr,LPTD_TABLE td)
{
	if ( td->x < tr->x )
		return FALSE;
	if ( td->y < tr->y )
		return FALSE;
	if ( (td->x + td->width) > (tr->x + tr->width) )
		return FALSE;
	if ( (td->y + td->height) > (tr->y + tr->height) )
		return FALSE;

	return TRUE;
}


HHTML ParseHtml(HWND hWnd,char *lpHtmlBuffer)
{
	char *lpPtr=lpHtmlBuffer;
	TAGID tagID;
	BOOL bIsHead=FALSE;
	LPHTMLHEAD  lpHead;
	LPHTMLOBJECT lpCurObject,lpObject;
	CONTROLSTATE structControlState;
//	DWORD width_up;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ParseHtml\r\n")));
	structControlState.color=0;
	structControlState.font=NULL;
	structControlState.url=NULL;
	structControlState.name=NULL;
	structControlState.x=0;
	structControlState.y=0;
	structControlState.bPreProcess=FALSE;
//	structControlState.lpMap=NULL;

	lpHead=(LPHTMLHEAD)malloc(sizeof(HTMLHEAD));
	if (lpHead==NULL)
	{
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	lpHead->next=NULL;
	lpHead->width=0;
	lpHead->height=0;

	lpCurObject=lpHead->next;

//	width_up=GetSystemMetrics(SM_CXSCREEN);

	while(*lpPtr)
	{
		tagID=SMIE_LocateTag(&lpPtr);
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
			//ToNextTag(&lpPtr);
			ParseHtmlHead(&lpPtr);
			continue;
		}
		/*
		else if (tagID==(TAGID_HEAD+TAGID_END))  // </HEAD>
		{
			bIsHead=FALSE;
			ToNextTag(&lpPtr);
			continue;
		}
		if (bIsHead==TRUE)  // will to be process the head content
		{
			ToNextTag(&lpPtr);
			continue;
		}
		*/
		if (tagID==TAGID_TABLE)
		{ // Insert a Table  <TABLE>
//			lpObject=ParseTable(&lpPtr,width_up);
			lpObject=ParseTable(hWnd,&lpPtr);
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
			lpObject=SMIE_ParseHtmlControl(hWnd,tagID,&lpPtr,&structControlState);
			if (lpObject)
			{
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
//			ToNextTag(&lpPtr);
		}
	}

	if (structControlState.font)
		free(structControlState.font);
	if (structControlState.url)
		free(structControlState.url);
	if (structControlState.name)
		free(structControlState.name);
//	if (structControlState.lpMap)
//		DestroyMap(structControlState.lpMap);
	return lpHead;
}

/*
ReCalc the Html Size 
*/
BOOL ReCalcSize(HWND hWnd,HDC hdc,HHTML hHtml)  
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;
//	HFONT font;
	DWORD width_up;
	SIZE size;
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

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ReCalcSize\r\n")));

	lpHead=(LPHTMLHEAD)hHtml;
	if (lpHead==NULL)
		return FALSE;
	lpCurObject=lpHead->next;

/*	while(lpCurObject)
	{
		switch(lpCurObject->type)
		{
		case TYPE_TABLE:
			ReCalcSize_Table(hdc,(LPHTMLTABLE)lpCurObject->lpContent);  // Recalc the Control Size
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
//	width_up=GetSystemMetrics(SM_CXSCREEN);
	GetClientRect(hWnd,&rect);
	width_up=rect.right-rect.left;//GetSystemMetrics(SM_CXSCREEN);
//	width_up=1700;
	size=ReCalcSize_Object(hWnd,hdc,lpCurObject,width_up);
	lpHead->width=size.cx;
	lpHead->height=size.cy;

	ReLocatePosition(hWnd,lpHead);
/*
#ifndef __WCE_DEFINE
	font=(HFONT)SelectObject(hdc,font);
	DeleteObject(font);
#endif
*/
	return TRUE;
}
/***************************************************************************************/
// ReNew Locate the Control Position
// The Function will Renew locate all control position
/***************************************************************************************/
BOOL ReLocatePosition(HWND hWnd,LPHTMLHEAD lpHead)
{
	LPHTMLOBJECT lpCurObject;
//	DWORD x,y,width;
	RECT rect;
	
	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ReLocatePosition\r\n")));

	lpCurObject=lpHead->next;

//	x=y=0;
//	width=775;
	GetClientRect(hWnd,&rect);

//	rect.left=x;
//	rect.right=rect.left+width;
//	rect.top=0;
//	rect.bottom=10000;
	rect.bottom=lpHead->height;
	ReLocate_Object(hWnd,lpCurObject,0,0,rect);

/*
	while(lpCurObject)
	{
		switch(lpCurObject->type)
		{
		case TYPE_TABLE:
			ReLocate_Table((LPHTMLTABLE)lpCurObject->lpContent,&x,&y,&width);  // Recalc the Control Size
			break;
		case TYPE_CONTROL:
			rect.left=x;
			rect.right=rect.left+width;
			rect.top=0;
			rect.bottom=10000;
			ReLocate_Control(lpCurObject->lpContent,&x,&y,rect);  // Recalc the Control Size
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
/***************************************************************************************/
// ReNew Locate the Control Position End 
/***************************************************************************************/

/***************************************************************************************/
// Show Html Buffer
/***************************************************************************************/
void ShowHtml(HWND hWnd,HDC hdc,HHTML hHtml,POINT ptOrg)
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
//		ShowBackGround(hdc,rcPaint,RGB(0xff,0xff,0xff));
//	}
	GetClientRect(hWnd,&rect);
	ShowBackGround(hdc,rect,RGB(0xff,0xff,0xff));

	lpHead=(LPHTMLHEAD)hHtml;
	if (lpHead==NULL)
		return;		
	lpCurObject=lpHead->next;



	while(lpCurObject)
	{
		switch(lpCurObject->type)
		{
		case TYPE_TABLE:
			ShowHtmlTable(hWnd,hdc,(LPHTMLTABLE)lpCurObject->lpContent,ptOrg);  // Show Table
			break;
		case TYPE_CONTROL:
			GetClientRect(hWnd,&rect);
//			rect.left=0;
//			rect.right=10000;
//			rect.top=0;
			rect.bottom=20000;
			SMIE_ShowHtmlControl(hWnd,hdc,lpCurObject->lpContent,rect,ptOrg);  // Show  the Control 
			break;
		default:
			break;
		}
		lpCurObject=lpCurObject->next;
	}
/*
#ifndef __WCE_DEFINE
	font=(HFONT)SelectObject(hdc,font);
	DeleteObject(font);
#endif
*/
}
/***************************************************************************************/
// Show Html Buffer End
/***************************************************************************************/

/***************************************************************************************/
// Release Html Buffer 
/***************************************************************************************/
void ReleaseHtml(HHTML hHtml)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;//,lpNext;
	

	if (hHtml==NULL)
		return;

	lpHead=(LPHTMLHEAD)hHtml;
	lpCurObject=lpHead->next;

	ReleaseObject(lpCurObject);
	free(lpHead);

}
/***************************************************************************************/
// Release Html Buffer End
/***************************************************************************************/

/***************************************************************************************/
// The Function about Head
/***************************************************************************************/
LPHTMLOBJECT ParseHtmlHead(char **lpHtmlBuffer)
{
	char **lpPtr;
	TAGID tagID;

		JAMIMSG(DBG_FUNCTION,(TEXT("Enter ParseHtmlHead\r\n")));

		lpPtr=lpHtmlBuffer;
/*
	while(1)
	{
		propertyID=LocateProperty(lpPtr); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
		switch(propertyID)
		{
			case PROPERTYID_WIDTH:  // Process property "width"
		}
	}
*/
		SMIE_ToNextTag(lpPtr);

	// Get item Content
	while(**lpPtr)
	{
		tagID=SMIE_LocateTag(lpPtr);
		if (tagID==TAGID_HEAD+TAGID_END)  // </HEAD>
		{
			SMIE_ToNextTag(lpPtr);
			break;
		}
		SMIE_ToNextTag(lpPtr);
	}
	return NULL;
}
/***************************************************************************************/
// The Function about Head End
/***************************************************************************************/

/***************************************************************************************/
// The Function about Table and the child
/***************************************************************************************/

/***************************************************************************************/
// Parse the Whole Table
/***************************************************************************************/
//LPHTMLOBJECT ParseTable(char **lpHtmlBuffer,DWORD width_up)
LPHTMLOBJECT ParseTable(HWND hWnd,char **lpHtmlBuffer)
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

	structControlState.color=0;
	structControlState.font=NULL;
	structControlState.url=NULL;
	structControlState.name=NULL;
	structControlState.x=0;
	structControlState.y=0;
//	structControlState.lpMap=NULL;
	structControlState.bPreProcess=FALSE;

	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
		return NULL;

	lpHtmlObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));
	if (lpHtmlObject==NULL)
	{
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	lpHtmlTable=(LPHTMLTABLE)malloc(sizeof(HTMLTABLE));
	if (lpHtmlTable==NULL)
	{
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}

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
	lpHtmlTable->frame=0;
	lpHtmlTable->rules=0;
	
	lpCurHead=lpHtmlTable->head;
	lpCurBody=lpHtmlTable->body;
	lpCurFoot=lpHtmlTable->foot;
	lpCurObject=lpHtmlTable->lpItem;

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
		case PROPERTYID_FRAME:// Process property "cellPadding"
			lpHtmlTable->frame=GetFrameStyle(pPropertyContent);
			break;
		case PROPERTYID_RULES:
			lpHtmlTable->rules=GetRulesStyle(pPropertyContent);
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
		{// Is Table Body
			thePart=HEAD_PART;
			SMIE_ToNextTag(lpPtr);
		}
		else if (tagID==TAGID_TFOOT)  // <TFOOT>
		{// Is Table Body
			thePart=FOOT_PART;
			SMIE_ToNextTag(lpPtr);
		}
		else if (tagID==TAGID_THEAD+TAGID_END)  // </THEAD>
		{// Is Table Body
			thePart=BODY_PART;
			SMIE_ToNextTag(lpPtr);
		}
		else if (tagID==TAGID_TFOOT+TAGID_END)  // </TFOOT>
		{// Is Table Body
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
			tr=ParseTR(hWnd,lpPtr,lpHtmlTable);
			if (tr)
			{
				if (thePart == BODY_PART)
				{
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
				{
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
				{
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
			lpControl=SMIE_ParseHtmlControl(hWnd,tagID,lpPtr,&structControlState);
			if (lpControl)
			{
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
//			ToNextTag(lpPtr);
		}
	}

	JAMIMSG(DBG_FUNCTION,(TEXT("Adjust Table\r\n")));
	AdjustTable(lpHtmlTable);

	// malloc widthPreCol and heightPreRow
	lpHtmlTable->heightPreRow=(LPDWORD)malloc(sizeof(DWORD)*(lpHtmlTable->rows+10));
	if (lpHtmlTable->heightPreRow==NULL)
	{
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	for (i=0;i<lpHtmlTable->rows;i++)
		lpHtmlTable->heightPreRow[i]=0;

	lpHtmlTable->widthPreCol=(LPDWORD)malloc(sizeof(DWORD)*(lpHtmlTable->cols+10));
	if (lpHtmlTable->widthPreCol==NULL)
	{
		if (lpHtmlTable->widthPreCol)
			free(lpHtmlTable->widthPreCol);
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	for (i=0;i<lpHtmlTable->cols;i++)
		lpHtmlTable->widthPreCol[i]=0;

	JAMIMSG(DBG_FUNCTION,(TEXT("Parse Table Success\r\n")));
	if (pPropertyContent)
		free(pPropertyContent);
	if (structControlState.font)
		free(structControlState.font);
	if (structControlState.url)
		free(structControlState.url);
	if (structControlState.name)
		free(structControlState.name);
//	if (structControlState.lpMap)
//		DestroyMap(structControlState.lpMap);
	return lpHtmlObject;
}
/***************************************************************************************/
// Parse the Whole Table
/***************************************************************************************/

/***************************************************************************************/
// Parse the TR of the Table (a row of the table)
/***************************************************************************************/
//LPTR_TABLE ParseTR(char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable,DWORD width_up)
LPTR_TABLE ParseTR(HWND hWnd,char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable)
{
	LPTR_TABLE tr;
	LPTD_TABLE lpCurTD,td;
	char **lpPtr=lpHtmlBuffer;
	TAGID tagID;
	DWORD cols=0;
	PROPERTYID propertyID;
	char *pPropertyContent;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter Parse TR\r\n")));

	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
	{
		return NULL;
	}
	tr=(LPTR_TABLE)malloc(sizeof(TR_TABLE)); // malloc tr
	if (tr==NULL)  // the failure to malloc tr
	{
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}

	tr->next=NULL;
	tr->height=0;
	tr->width=0;
	tr->td=NULL;
	tr->bgColor=-1;
	lpCurTD=tr->td;

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
		case PROPERTYID_BGCOLOR:
			tr->bgColor=SMIE_GetColor(pPropertyContent);
			break;
			/*
		case PROPERTYID_WIDTH:  // Process property "width"
			if (IsPercent(pPropertyContent)==TRUE)
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

	SMIE_ToNextTag(lpPtr);
	// Get td Content
	while(**lpPtr)
	{
		tagID=SMIE_NextTagIs(*lpPtr);
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

		tagID=SMIE_LocateTag(lpPtr);
		if (tagID==TAGID_TR+TAGID_END) // <TR>
		{  // This is Next TR
			SMIE_ToNextTag(lpPtr);
			break;
		}
		if (tagID==TAGID_TD)  // <TD>
		{// Insert a TD to current Row
			cols++;  // have a new cols
//			if (tr->width)
//				width_up=tr->width;
//			td=ParseTD(lpPtr,width_up);
			JAMIMSG(DBG_FUNCTION,(TEXT("Parse TD \r\n")));
			td=ParseTD(hWnd,lpPtr);
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
			td=ParseTD(hWnd,lpPtr);
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
			SMIE_ToNextTag(lpPtr);
//			break;
		}
	}
	if (lpHtmlTable->cols<cols)
		lpHtmlTable->cols=cols;
	// if the parent width is know and only one son's width is unKnow in the sons
	// then the last son's width is know 
//	if (width_up)
	if (lpHtmlTable->width)
	{
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
		{
//			if (width_up>width)
//				lpCurTD->width=width_up-width;
			if (lpHtmlTable->width>width)
				lpCurTD->width=lpHtmlTable->width-width;
		}
	}
	if (pPropertyContent)
		free(pPropertyContent);
	return tr;
}
/***************************************************************************************/
// Parse the TR of the Table (a row of the table) End
/***************************************************************************************/

/***************************************************************************************/
// Parse the TD of the Table (a col of the Row) 
/***************************************************************************************/
//TD_TABLE ParseTD(char **lpHtmlBuffer,DWORD width_up)
LPTD_TABLE ParseTD(HWND hWnd,char **lpHtmlBuffer)
{
	char **lpPtr=lpHtmlBuffer;
	LPTD_TABLE td;
	LPHTMLOBJECT lpCurObject,lpControl;
	TAGID tagID;
	CONTROLSTATE structControlState;
	PROPERTYID propertyID;
	char *pPropertyContent;

	JAMIMSG(DBG_FUNCTION,(TEXT("Enter Parse TD \r\n")));
	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
	{
		return NULL;
	}
	td=(LPTD_TABLE)malloc(sizeof(TD_TABLE)); // malloc tr
	if (td==NULL)  // the failure to malloc tr
	{
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}

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


	lpCurObject=td->lpItem;

	structControlState.color=0;
	structControlState.font=NULL;
	structControlState.url=NULL;
	structControlState.name=NULL;
	structControlState.x=0;
	structControlState.y=0;
	structControlState.bPreProcess=FALSE;
//	structControlState.lpMap=NULL;

//	if (width_up>=PERCENTTAG)
//		width_up=0;
	// Get style of the tr
	while(1)
	{
		propertyID=SMIE_LocateProperty(lpPtr); // get property ID
		if (propertyID==PROPERTYID_NULL)  // have no property ID
			break;
		switch(propertyID)
		{
		case PROPERTYID_BGCOLOR:
			SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
			td->bgColor=SMIE_GetColor(pPropertyContent);
			break;
		case PROPERTYID_COLSPAN:
			SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
			td->colSpan=atoi(pPropertyContent);
			break;
		case PROPERTYID_ROWSPAN:
			SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
			td->rowSpan=atoi(pPropertyContent);
			break;
		case PROPERTYID_WIDTH:  // Process property "width"
			SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
			if (SMIE_IsPercent(pPropertyContent)==TRUE)
			{
//				if (width_up)
//					td->width=atoi(pPropertyContent)*width_up/100;
//				else
					td->percent=atoi(pPropertyContent)+PERCENTTAG;
			}
			else
				td->percent=atoi(pPropertyContent);
			break;
		case PROPERTYID_HEIGHT:  // Process property "width"
			SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
			td->heightSet=atoi(pPropertyContent);
			break;
		case PROPERTYID_NOWRAP:
			td->NoWrap=TRUE;
			break;
		default:
			SMIE_LocatePropertyContent(lpPtr,pPropertyContent,1024); // get current property ID's content
			break;
		}
	}

	JAMIMSG(DBG_FUNCTION,(TEXT("Parse TD Style Success\r\n")));

	SMIE_ToNextTag(lpPtr);
	// Get td Content
	while(**lpPtr)
	{
		tagID=SMIE_NextTagIs(*lpPtr);
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

		tagID=SMIE_LocateTag(lpPtr);
		if (tagID==TAGID_TD+TAGID_END)  // </TD>
		{
			SMIE_ToNextTag(lpPtr);
			break;
		}
		else if (tagID==TAGID_TABLE)  // <TABLE>
		{
//			if (td->width)
//			width_up=td->width;
//			lpControl=ParseTable(lpPtr,width_up);
			lpControl=ParseTable(hWnd,lpPtr);
			if (lpControl)
			{
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
			lpControl=SMIE_ParseHtmlControl(hWnd,tagID,lpPtr,&structControlState);
			if (lpControl)
			{
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
	if (pPropertyContent)
		free(pPropertyContent);
	if (structControlState.font)
		free(structControlState.font);
	if (structControlState.url)
		free(structControlState.url);
	if (structControlState.name)
		free(structControlState.name);
//	if (structControlState.lpMap)
//		DestroyMap(structControlState.lpMap);
	return td;
}

/***************************************************************************************/
// Parse the TD of the Table (a col of the Row) End
/***************************************************************************************/

/***************************************************************************************/
// ReCalc the size of the Table
/***************************************************************************************/
BOOL ReCalcSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE  lpTable,DWORD iTDMaxWidth,BOOL bAdjust)
{
	LPTR_TABLE tr;
	LPHTMLOBJECT lpObject;
	DWORD width,height;//,widthTable;
	DWORD iRows=0,i;
//	lpHtmlTable->heightPreRow=NULL;
//	lpHtmlTable->widthPreCol=NULL;

	width=0;
	height=0;

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

	lpTable->height=0;
	
	tr=lpTable->head;  // ReCalc Size the head tr of the table
	while(tr)
	{
		ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust);
		lpTable->heightPreRow[iRows]=tr->height;
		iRows++;
		tr=tr->next;
	}

	tr=lpTable->body; // ReCalc Size the body tr of the table
	while(tr)
	{
		ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust);
		lpTable->heightPreRow[iRows]=tr->height;
		iRows++;
		tr=tr->next;
	}

	tr=lpTable->foot; // ReCalc Size the foot tr of the table
	while(tr)
	{
		ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust);
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
	AdjustSize_Table(lpTable);
	for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
	{
//		lpTable->widthPreCol[i]+=lpTable->cellPadding;
		width+=lpTable->widthPreCol[i]+lpTable->cellPadding;
	}
	width+=lpTable->cellSpacing*(lpTable->cols-1);
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
				ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
			}
		}
	}
	height=0;
	for (i=0;i<lpTable->rows;i++)      // the table height is total of the row height
	{
//		lpTable->heightPreRow[i]+=lpTable->cellPadding;
		height+=lpTable->heightPreRow[i]+lpTable->cellPadding+lpTable->cellSpacing;
	}
	width=0;
	for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
	{
//		lpTable->widthPreCol[i]+=lpTable->cellPadding;
		width+=lpTable->widthPreCol[i]+lpTable->cellPadding;
	}
	width+=lpTable->cellSpacing*(lpTable->cols-1);
	/*********************/
	// calc the Object
	lpObject=lpTable->lpItem; // ReCalc Size the Control of the table
	if (width==0 && height==0)
	{
		width=1;
		height=1;
	}
	{
		SIZE size;

			size=ReCalcSize_Object(hWnd,hdc,lpObject,lpTable->width);
			height+=size.cy;
			if (width<(DWORD)size.cx)
				width=size.cx;
	}
	/*********************/

	if (lpTable->width==0)
		lpTable->width=width;
	if (lpTable->width!=0 && lpTable->width < width)
	{
//     This condition, what do it.
	    //the table min width is "width",so must to set the table width is "width"
		lpTable->width=width;
	}

//	if (lpTable->height==0)
	if (lpTable->heightSet<height)
		lpTable->height=height;
	else
		lpTable->height=lpTable->heightSet;
	return TRUE;
}
/***************************************************************************************/
// ReCalc the size of the Table End
/***************************************************************************************/

/***************************************************************************************/
// Adjust Table
/***************************************************************************************/
void AdjustTable(LPHTMLTABLE  lpTable)
{
	LPTR_TABLE tr;
	DWORD iCol=0;

	tr=lpTable->head;  // ReCalc Size the head tr of the table
	iCol=AdjustTR(tr);
	if (iCol>lpTable->cols)
		lpTable->cols=iCol;

	tr=lpTable->body; // ReCalc Size the body tr of the table
	iCol=AdjustTR(tr);
	if (iCol>lpTable->cols)
		lpTable->cols=iCol;

	tr=lpTable->foot; // ReCalc Size the foot tr of the table
	iCol=AdjustTR(tr);
	if (iCol>lpTable->cols)
		lpTable->cols=iCol;


}
/***************************************************************************************/
// Adjust Table End
/***************************************************************************************/
DWORD AdjustTR(LPTR_TABLE tr)
{
	LPTR_TABLE curTR;
	LPTD_TABLE td;
	DWORD iMaxCol,iCol;
//	DWORD iMaxRow,iRow;
	DWORD i;

	iMaxCol=iCol=0;
	while(tr)
	{
		td=tr->td;
		iCol=0;
		while(td)
		{
			if (td->rowSpan>1)
			{
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
					InsertEmptyTDInTR(curTR,iCol,td->colSpan);
				}
			}
			iCol+=td->colSpan;
			td=td->next;
		}
		if (iCol>iMaxCol)
			iMaxCol=iCol;
		iCol=0;
		tr=tr->next;
	}
	return iMaxCol;
}

/***************************************************************************************/
// Insert an Empty Td to the TR
/***************************************************************************************/
BOOL InsertEmptyTDInTR(LPTR_TABLE tr,DWORD iInsertCol,DWORD colSpan)
{
	DWORD iCol;
	LPTD_TABLE td,newTD,pretd;

		if (tr==NULL)
			return TRUE;
		td=tr->td;
		
		iCol=0;
		pretd=td;
		while(td)
		{
			if (iCol==iInsertCol)
				break;
			pretd=td;
			td=td->next;
			iCol++;
		}
		newTD=(LPTD_TABLE)malloc(sizeof(TD_TABLE));
		if (newTD==NULL)
		{
			MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
			return FALSE;
		}

		memset(newTD,0,sizeof(TD_TABLE));

		newTD->IsVirtual=TRUE;
		newTD->rowSpan=1;
		newTD->colSpan=colSpan;

		newTD->next=td;
		if (iCol==0)
		{
			tr->td=newTD;
		}
		else
		{
			pretd->next=newTD;
		}

		return TRUE;
}
/***************************************************************************************/
// Adjust Table End
/***************************************************************************************/


/***************************************************************************************/
// Compress the table column size
/***************************************************************************************/
void CompressSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD widthCompress,DWORD iTDMaxWidth)
{
	DWORD width,tdWidthCompress,widthPreCompresss=0,curWidth;
	DWORD i;
	LPDWORD lpMinWidth;
	LPDWORD lpBackup;
	LPTR_TABLE tr;
	LPTD_TABLE td;
	DWORD iCol;

	if (lpTable->cols==0)
		return;
	lpMinWidth=(LPDWORD)malloc(lpTable->cols*sizeof(DWORD));
	if (lpMinWidth==NULL)
	{
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return ;
	}
	lpBackup=(LPDWORD)malloc(lpTable->cols*sizeof(DWORD));
	if (lpBackup==NULL)
	{
		free(lpMinWidth);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return ;
	}
	memset(lpMinWidth,0,lpTable->cols*sizeof(DWORD));
	while(1)
	{
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			lpBackup[i]=lpTable->widthPreCol[i];
		}
		tr=lpTable->body;
		while(tr)
		{
			td=tr->td;
			iCol=0;
			while(td)
			{
				if (td->NoWrap==TRUE)
				{
					if (lpMinWidth[iCol]<td->width)
						lpMinWidth[iCol]=td->width;
				}
				iCol+=td->colSpan;
				td=td->next;
			}
			tr=tr->next;
		}
		width=0;
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			width+=lpTable->widthPreCol[i]-lpMinWidth[i];
		}

		if (width==0)
		{
//			free(lpMinWidth);
//			free(lpBackup);
//			return;
			goto PROCESS_END;
		}
		for (i=0;i<lpTable->cols;i++)		
		{
			tdWidthCompress=widthCompress*(lpTable->widthPreCol[i]-lpMinWidth[i])/width;
			if (lpTable->widthPreCol[i]<tdWidthCompress)
				lpTable->widthPreCol[i]=0;
			else
				lpTable->widthPreCol[i]-=tdWidthCompress;
		}
		
		ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
		for (i=0;i<lpTable->cols;i++)		
		{
			tdWidthCompress=widthCompress*(lpBackup[i]-lpMinWidth[i])/width;
			if (lpTable->widthPreCol[i]==0||(lpBackup[i]-lpTable->widthPreCol[i])<tdWidthCompress)
				lpMinWidth[i]=lpTable->widthPreCol[i];
			else
				lpMinWidth[i]=0;
		}
		for (i=0;;i++)
		{
			if (i==lpTable->cols)
			{
//				free(lpMinWidth);
//				free(lpBackup);
//				return ;
				goto PROCESS_END;
			}
			if (lpMinWidth[i]!=0)
				break;
		}
//		curWidth=0;
		for (i=0;;i++)
		{
			if (i==lpTable->cols)
			{// if the all td is not compress,then the table width will be expand
				goto PROCESS_END;
//				iTDMaxWidth=curWidth;
//				ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
//				free(lpMinWidth);
//				free(lpBackup);
//				return ;
			}
			if (lpBackup[i]!=lpTable->widthPreCol[i])
				break;
//			curWidth+=lpTable->widthPreCol[i];
		}
		if (width<widthPreCompresss)
			break;
		width=0;
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			width+=lpTable->widthPreCol[i];
		}
		width+=lpTable->cellSpacing*(lpTable->cols-1);
		
		widthPreCompresss=width;
		if (width<lpTable->width)
			break;

		widthCompress=width-lpTable->width;
	}
PROCESS_END:
	curWidth=0;
	for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
	{
		curWidth+=lpTable->widthPreCol[i];
	}
	iTDMaxWidth=curWidth;
	ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
	free(lpMinWidth);
	free(lpBackup);
}
/***************************************************************************************/
// Compress the table column size End
/***************************************************************************************/

/***************************************************************************************/
// Increase the table column size
/***************************************************************************************/
void Increase_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD iTDMaxWidth)
{
	DWORD width,widthCompress,tdWidthCompress;
	DWORD i;

	if (lpTable->cols==0)
		return;
	width=0;
	for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
	{
		width+=lpTable->widthPreCol[i];
	}
	width+=lpTable->cellSpacing*(lpTable->cols-1);

	widthCompress=lpTable->width-width;

	if (width!=0)
	{
		for (i=0;i<lpTable->cols;i++)		
		{
			tdWidthCompress=widthCompress*lpTable->widthPreCol[i]/width;
			lpTable->widthPreCol[i]+=tdWidthCompress;
		}
	}
	else
	{
		widthCompress /=lpTable->cols;
		for (i=0;i<lpTable->cols;i++)		
		{
			lpTable->widthPreCol[i]+=widthCompress;
		}
	}	
//	ReCalcSize_Table(hdc,lpTable,lpTable->width,FALSE);
	ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
}
/***************************************************************************************/
// Increase the table column size End
/***************************************************************************************/

/***************************************************************************************/
// ReCalc the TR size of the Table
/***************************************************************************************/
BOOL ReCalcSize_TR(HWND hWnd,HDC hdc,LPTR_TABLE tr,LPHTMLTABLE  lpTable,BOOL bAdjust)
{
	LPTD_TABLE td;
	DWORD width,height;
	DWORD iCols=0;
	DWORD iTDMaxWidth,tdWidth;
	DWORD i;

		width=0;
		height=0;

//		if (tr->width)
//			iTDMaxWidth=tr->width;
//		else
		iTDMaxWidth=lpTable->width;
//		if (tr->width>=PERCENTTAG)
//			tr->width=(tr->width-PERCENTTAG)*iTDMaxWidth/100;
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
			else
			{
				td->width=0;
			}
			td=td->next;
		}

		td=tr->td;
//		tdWidth=iTDMaxWidth;  // add by Jami in 2001.07.03
		while(td)
		{
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
			}
			else
			{
					tdWidth=iTDMaxWidth;  
			}
			ReCalcSize_TD(hWnd,hdc,td,tdWidth);  // Recalc the td Size
			if (td->rowSpan==1)
			{
				if (height<td->height)  // The Heght is max height of all td with the tr
					height=td->height;  
			}
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
					lpTable->widthPreCol[iCols+td->colSpan-1]+=td->width-tdWidth; // the last col width is free width
					tdWidth=td->width;
				}
			}
			width+=tdWidth;       // the width of the tr is total width all td with the tr
			if (iTDMaxWidth>tdWidth)
				iTDMaxWidth-=tdWidth;
			iCols+=td->colSpan;
			td=td->next;
		}
		tr->width=width;
		tr->height=height;
		return TRUE;
}
/***************************************************************************************/
// ReCalc the TR size of the Table End
/***************************************************************************************/

/***************************************************************************************/
// ReCalc the TD size of the Table
/***************************************************************************************/
BOOL ReCalcSize_TD(HWND hWnd,HDC hdc,LPTD_TABLE td,DWORD iTDMaxWidth)
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
			ReCalcSize_Table(hdc,(LPHTMLTABLE)lpObject->lpContent);  // Recalc the Table Size
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

	if (td->IsVirtual==TRUE)
		return TRUE;

	lpObject=td->lpItem;

	if (td->width)
		iTDMaxWidth=td->width;
//	else
//		iTDMaxWidth=0x7fff;

	td->height=0;

	size=ReCalcSize_Object(hWnd,hdc,lpObject,iTDMaxWidth);
	if (td->width<(DWORD)size.cx)
		td->width=size.cx;
	if (td->heightSet<(DWORD)size.cy)
		td->height=size.cy;
	else
		td->height=td->heightSet;
	return TRUE;

}
/***************************************************************************************/
// ReCalc the TD size of the Table End
/***************************************************************************************/


/***************************************************************************************/
// ReCalc the Object size of the Table 
/***************************************************************************************/
SIZE ReCalcSize_Object(HWND hWnd,HDC hdc,LPHTMLOBJECT lpObject,DWORD iTDMaxWidth)
{
	DWORD width,height,widthTD,widthLine;
	DWORD iLastRowHeight=0,ControlHeight;
	BOOL IsNewRow;
	DWORD iCurPos;
	SIZE size={0,0};
	RECT rect;
	DWORD iHeightLine;
	HHTMCTL lpPreControl;
	BOOL bSidePreControl=FALSE;
				

	iHeightLine=SMIE_GetFontHeight(hWnd,NULL);
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
	{
		switch(lpObject->type)
		{
		case TYPE_TABLE:


//			height+=iLastRowHeight;   
			ReCalcSize_Table(hWnd,hdc,(LPHTMLTABLE)lpObject->lpContent,iTDMaxWidth,TRUE);  // Recalc the Table Size
			if (bSidePreControl==TRUE)
			{   // the next table will place the current table's side (side by side)
				width+=((LPHTMLTABLE)(lpObject->lpContent))->width;    // Get Table width
				if (iLastRowHeight<((LPHTMLTABLE)(lpObject->lpContent))->height)
					iLastRowHeight=((LPHTMLTABLE)(lpObject->lpContent))->height;   // Get Table Height
			}
			else
			{
				height+=iLastRowHeight;   
				width=((LPHTMLTABLE)(lpObject->lpContent))->width;    // Get Table width
	//			if (iLastRowHeight<((LPHTMLTABLE)(lpObject->lpContent))->height)  // Get Table 
				iLastRowHeight=((LPHTMLTABLE)(lpObject->lpContent))->height;   // Get Table Height
			}
			if ((((LPHTMLTABLE)lpObject->lpContent)->align==ALIGN_LEFT)||
				(((LPHTMLTABLE)lpObject->lpContent)->align==ALIGN_RIGHT))
			{
				bSidePreControl=TRUE;
			}
			else
			{  // the Next control will place under of this table 
				bSidePreControl=FALSE;
				height+=iLastRowHeight;   
				iLastRowHeight=0;
			}
			if (widthTD<width)		// charge the table width and the TD width
				widthTD=width;
//			height+=iLastRowHeight;   //the TD height must add the table height
//			iLastRowHeight=0;         // this line height is zero (will enter a new line)
			widthLine=0;
//			IsNewRow=TRUE;
			lpPreControl=NULL;
			break;
		case TYPE_CONTROL:
//			ReCalcSize_Control(hdc,lpObject->lpContent,iRemainWidth);  // Recalc the Control Size
//			ReCalcSize_Control(hdc,lpObject->lpContent,0);  // Recalc the Control Size
			GetClientRect(hWnd,&rect);
			rect.left=0;
			rect.top=0;
//			if (iTDMaxWidth==0)
//				rect.right=rect.left+MAXWIDTH;
//			else
			if (iTDMaxWidth!=0)
				rect.right=rect.left+iTDMaxWidth;
			rect.bottom=MAXHEIGHT;

			SMIE_ReCalcSize_Control(hWnd,hdc,lpObject->lpContent,iCurPos,rect);  // Recalc the Control Size
			if (((LPHTMLCONTROL)(lpObject->lpContent))->image)
			{
				if (((LPHTMLCONTROL)(lpObject->lpContent))->width<(rect.right-iCurPos))
				{
					iCurPos+=((LPHTMLCONTROL)(lpObject->lpContent))->widthLastRow;
					widthLine=iCurPos;
					if (iLastRowHeight<((LPHTMLCONTROL)(lpObject->lpContent))->height)
						iLastRowHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;
				}
				else
				{
					// will put a other row,then the height must add this row height
					height+=iLastRowHeight;
					// and the current height is the image height
					iLastRowHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;
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
			if(iLastRowHeight<ControlHeight)   // if the current line height is small with the control line height
				iLastRowHeight=ControlHeight;  // set new line height
			if (widthLine>widthTD)
				widthTD=widthLine;
			lpPreControl=lpObject->lpContent;
			break;
		case TYPE_CR:   // Is a CR
//			if (iLastRowHeight)  // Exist line height
			height+=iLastRowHeight;   
//			else  // Is a empty line
//				height+=14;
			iLastRowHeight=iHeightLine;
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
	
	size.cx=widthTD;
	size.cy=height;
	return size;
}
/***************************************************************************************/
// ReCalc the Object size of the Table End
/***************************************************************************************/


/***************************************************************************************/
// Adjust the Table Size
/***************************************************************************************/
void AdjustSize_Table(LPHTMLTABLE  lpTable)
{
	LPTR_TABLE tr;

	tr=lpTable->head;  // ReCalc Size the head tr of the table
	AdjustSize_TR(tr,lpTable);

	tr=lpTable->body; // ReCalc Size the body tr of the table
	AdjustSize_TR(tr,lpTable);

	tr=lpTable->foot; // ReCalc Size the foot tr of the table
	AdjustSize_TR(tr,lpTable);
}
/***************************************************************************************/
// Adjust the Table Size End
/***************************************************************************************/

/***************************************************************************************/
// Adjust the TR size
// if the Multi Row Size >the size of total of across row size
// then the last row size will to be increase.
/***************************************************************************************/
void AdjustSize_TR(LPTR_TABLE tr,LPHTMLTABLE  lpTable)
{
	LPTD_TABLE td;
	DWORD iRow,iHeight=0;
	DWORD iCols,tdWidth;
	DWORD i;

	iRow=0;
	while(tr)
	{
		td=tr->td;
		iCols=0;
		while(td)
		{
			if (td->rowSpan>1)
			{
				iHeight=0;
				for (i=iRow;i<iRow+td->rowSpan;i++)
				{
					iHeight+=lpTable->heightPreRow[i];
				}
				if (iHeight<td->height)
					lpTable->heightPreRow[iRow+td->rowSpan-1]+=td->height-iHeight;  // the last row is the ramain height
			}

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

			iCols+=td->colSpan;
			td=td->next;
		}
		tr=tr->next;
		iRow++;
	}
}
/***************************************************************************************/
// Adjust the TR size End
/***************************************************************************************/


/***************************************************************************************/
// Renew Locate Table position
// Parameter:
// lpTable : the pointer of the table
// lpx     : the pointer of the table start x,then will return the next x position
// lpy     : the pointer of the table start y ,then will return the next y position
/***************************************************************************************/
BOOL ReLocate_Table(HWND hWnd,LPHTMLTABLE lpTable,LPDWORD lpx,LPDWORD lpy,LPDWORD lpiMaxWidth,LPDWORD iTableHeight)
{
	LPTR_TABLE tr;
	LPHTMLOBJECT lpObject;
	DWORD x,y;
	RECT rect;

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
	{
		lpTable->x=*lpx;
		lpTable->y=*lpy;
//		if (*iTableHeight>lpTable->height)
//			*lpy+=*iTableHeight;
//		else
			*lpy+=lpTable->height;
		*iTableHeight=0;
	}


	x=lpTable->x;
	y=lpTable->y;
	tr=lpTable->head;  // ReLocate the head tr of the table
	while(tr)
	{
		ReLocate_TR(hWnd,tr,lpTable,y);
		y+=tr->height;
		y+=lpTable->cellSpacing;
		tr=tr->next;
	}

	tr=lpTable->body; // ReLocate the body tr of the table
	while(tr)
	{
		ReLocate_TR(hWnd,tr,lpTable,y);
		y+=tr->height;
		y+=lpTable->cellSpacing;
		tr=tr->next;
	}

	tr=lpTable->foot; // ReLocate the foot tr of the table
	while(tr)
	{
		ReLocate_TR(hWnd,tr,lpTable,y);
		y+=tr->height;
		y+=lpTable->cellSpacing;
		tr=tr->next;
	}

	lpObject=lpTable->lpItem; // ReLocate the Control of the table

	rect.left=lpTable->x;
	rect.right=rect.left+lpTable->width;
	rect.top=lpTable->y;
	rect.bottom=rect.top+lpTable->height;
	ReLocate_Object(hWnd,lpObject,x,y,rect);

	return TRUE;
}
/***************************************************************************************/
// Renew Locate Table position End
/***************************************************************************************/


/***************************************************************************************/
// Renew Locate TR position of the Table
// Parameter:
// tr      : the pointer of will be renew locate row
// lpTable : the pointer of the table
/***************************************************************************************/
BOOL ReLocate_TR(HWND hWnd,LPTR_TABLE tr,LPHTMLTABLE  lpTable,DWORD y)
{
	DWORD x;
	LPTD_TABLE td;
	DWORD iCols=0;
	DWORD i;

	x=lpTable->x;
	tr->x=x;
	tr->y=y;
//	y=y;


		td=tr->td;
		while(td)
		{
			ReLocate_TD(hWnd,td,x,y);  // Renew Locate the td Position
			if (ChecktdIntr(tr,td)==FALSE)
			{
				DisplayRect("the TR Rect\r\n",tr->x,tr->y,tr->width,tr->height);
				DisplayRect("the TD Rect\r\n",td->x,td->y,td->width,td->height);
			}
			for (i=0;i<td->colSpan;i++)
			{
				x+=lpTable->widthPreCol[iCols];
				x+=lpTable->cellSpacing;
				iCols++;
			}
			td=td->next;
		}
		return TRUE;
}
/***************************************************************************************/
// Renew Locate TR position of the Table End
/***************************************************************************************/

/***************************************************************************************/
// Renew Locate TD position of the Table
// Parameter:
// td      : the pointer of will be renew locate col
// x       : the start x of this col
// y       : the start y of this col
/***************************************************************************************/
BOOL ReLocate_TD(HWND hWnd,LPTD_TABLE td,DWORD x_Start,DWORD y_Start)
{
	LPHTMLOBJECT lpObject;
	DWORD x,y;
//	DWORD width;
	DWORD iTableHeight=0;
	DWORD iLastRowHeight=0;
	RECT rect;


	if (td->IsVirtual==TRUE)
		return TRUE;
	lpObject=td->lpItem;


	x=x_Start;
	y=y_Start;

	td->x=x;
	td->y=y;

	rect.left=td->x;
	rect.top=td->y;
	rect.right=td->x+td->width;
	rect.bottom=td->y+td->height;
/*	
	width=td->width;
	while(lpObject)
	{
		switch(lpObject->type)
		{
		case TYPE_TABLE:
			ReLocate_Table((LPHTMLTABLE)lpObject->lpContent,&x,&y,&width);  // Recalc the Control Size
//			iTableHeight=((LPHTMLTABLE)(lpObject->lpContent))->height;   // Get Table Height
//			IsTable=TRUE;
			break;
		case TYPE_CONTROL:

			{
				char *cName;
				 cName=((LPHTMLCONTROL)(lpObject->lpContent))->name;
			}
			if (width==0)
				break;
			ReLocate_Control(lpObject->lpContent,&x,&y,rect);  // Recalc the Control Size
			if(iLastRowHeight<((LPHTMLCONTROL)(lpObject->lpContent))->height)
				iLastRowHeight=((LPHTMLCONTROL)(lpObject->lpContent))->height;
			break;
		case TYPE_CR:
			x=x_Start;
			if (iLastRowHeight)
				y+=iLastRowHeight;
			else
				y+=14;
			iLastRowHeight=0;
			break;
		default:
			break;
		}
		lpObject=lpObject->next;
	}
*/
	ReLocate_Object(hWnd,lpObject,x_Start,y_Start,rect);
	return TRUE;
}
/***************************************************************************************/
// Renew Locate TD position of the Table End
/***************************************************************************************/

/***************************************************************************************/
// Renew Locate Object position 
// Parameter:
// lpObject      : the pointer of will be renew locate Object link
// x_Start       : the start x of this col
// y_Start       : the start y of this col
/***************************************************************************************/
BOOL ReLocate_Object(HWND hWnd,LPHTMLOBJECT lpObject,DWORD x_Start,DWORD y_Start,RECT rect)
{
	DWORD x,y;
	DWORD width;
	DWORD iTableHeight=0;
	DWORD iLastRowHeight=0,iCurTabHeight=0;
	DWORD iHeightLine;
	HHTMCTL lpPreControl;

	x=x_Start;
	y=y_Start;
	
	iHeightLine=SMIE_GetFontHeight(hWnd,NULL);
	width=rect.right-rect.left;
//	if (width==0)
//		return TRUE;
	lpPreControl=NULL;
	while(lpObject)
	{
		switch(lpObject->type)
		{
		case TYPE_TABLE:

			if (iLastRowHeight)
			{
				x=x_Start;
				y+=iLastRowHeight;
			}

			ReLocate_Table(hWnd,(LPHTMLTABLE)lpObject->lpContent,&x,&y,&width,&iCurTabHeight);  // Recalc the Control Size
			iLastRowHeight=0;
			lpPreControl=NULL;
			break;
		case TYPE_CONTROL:
			if (width==0)
				break;
//			ReLocate_Control(lpObject->lpContent,&x,&y,rect,lpPreControl);  // Recalc the Control Size
			if (SMIE_ReLocate_Control(hWnd,lpObject->lpContent,&x,&y,rect,iLastRowHeight)==FALSE)  // Recalc the Control Size
			{
				// the Current line is not place this control,must to new line
				if (x!=x_Start)
				{
					y+=iLastRowHeight;
					x=x_Start;
				}
				SMIE_ReLocate_Control(hWnd,lpObject->lpContent,&x,&y,rect,iLastRowHeight);  // Recalc the Control Size
			}

			if(iLastRowHeight<((LPHTMLCONTROL)(lpObject->lpContent))->heightLine)
				iLastRowHeight=((LPHTMLCONTROL)(lpObject->lpContent))->heightLine;
			lpPreControl=lpObject->lpContent;
			break;
		case TYPE_CR:
//			if (iLastRowHeight)
			y+=iLastRowHeight;
//			else
//				y+=14;
			x=x_Start;
			iLastRowHeight=iHeightLine;
			lpPreControl=NULL;
			break;
		default:
			break;
		}
		lpObject=lpObject->next;
	}
	return TRUE;
}
/***************************************************************************************/
// Renew Locate TD position of the Table End
/***************************************************************************************/


/***************************************************************************************/
// Show Table
/***************************************************************************************/
void ShowHtmlTable(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,POINT ptOrg)
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
			
			GetClientRect(hWnd,&rect);
//			DisplayRect("the Table Rect\r\n",lpTable->x,lpTable->y,lpTable->width,lpTable->height);
//			if (((lpTable->y-ptOrg.y)>rect.bottom)||((lpTable->y-ptOrg.y+lpTable->height)<rect.top))
			if ((lpTable->y>(DWORD)(ptOrg.y+rect.bottom))||((lpTable->y+lpTable->height)<(DWORD)(rect.top+ptOrg.y)))
				return;
	}
	
	if (lpTable->bgColor!=-1)
	{
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
		rect.left=lpTable->x-ptOrg.x;
		rect.top=lpTable->y-ptOrg.y;
		rect.right=lpTable->x-ptOrg.x+lpTable->width;
		rect.bottom=lpTable->y-ptOrg.y+lpTable->height;
		ShowBackGround(hdc,rect,lpTable->bgColor);
		bgOldColor=SetBkColor(hdc,lpTable->bgColor);
	}
//	
	tr=lpTable->head;  // ReLocate the head tr of the table
	while(tr)
	{
		ShowTR(hWnd,hdc,tr,ptOrg);
		tr=tr->next;
	}

	tr=lpTable->body; // ReLocate the body tr of the table
	while(tr)
	{
		ShowTR(hWnd,hdc,tr,ptOrg);
		tr=tr->next;
	}

	tr=lpTable->foot; // ReLocate the foot tr of the table
	while(tr)
	{
		ShowTR(hWnd,hdc,tr,ptOrg);
		tr=tr->next;
	}

	lpObject=lpTable->lpItem; // ReLocate the Control of the table
	while(lpObject)
	{
		if (lpObject->type==TYPE_CONTROL)
		{
			RECT rect;

			rect.left=lpTable->x-ptOrg.x;
			rect.right=rect.left+lpTable->width;
			rect.top=lpTable->y-ptOrg.y;
			rect.bottom=rect.top+lpTable->height;

			SMIE_ShowHtmlControl(hWnd,hdc,lpObject->lpContent,rect,ptOrg);
		}
		lpObject=lpObject->next;
	}
	if (lpTable->bgColor!=-1)
	{
		SetBkColor(hdc,bgOldColor);
	}
	
	// Draw Frame and Rules
	if (lpTable->frame!=FRAME_VOID || lpTable->rules!=RULES_NONE)
	{
		// need Draw frame or rules
		if (lpTable->frame|FRAME_ABOVE)
		{
			// Draw Above frame Line
			MoveTo(hdc,lpTable->x,lpTable->y);
			LineTo(hdc,lpTable->width+lpTable->x,lpTable->y);
		}
		if (lpTable->frame|FRAME_BELOW)
		{
			// Draw Below frame Line
			MoveTo(hdc,lpTable->x,lpTable->y+lpTable->height);
			LineTo(hdc,lpTable->width+lpTable->x,lpTable->y+lpTable->height);
		}
		if (lpTable->frame|FRAME_LHS)
		{
			// Draw left frame line
			MoveTo(hdc,lpTable->x,lpTable->y);
			LineTo(hdc,lpTable->x,lpTable->y+lpTable->height);
		}
		if (lpTable->frame|FRAME_RHS)
		{
			// Draw right frame line
			MoveTo(hdc,lpTable->x+lpTable->width,lpTable->y);
			LineTo(hdc,lpTable->x+lpTable->width,lpTable->y+lpTable->height);
		}

		if (lpTable->rules|RULES_ROWS)
		{
			// draw rows lines
			DWORD i;
			DWORD rowsPos=0;
				for (i=1;i<lpTable->rows;i++)
				{
					rowsPos+=lpTable->heightPreRow[i-1]+(lpTable->cellSpacing+lpTable->cellPadding)/2;
					MoveTo(hdc,lpTable->x,lpTable->y+rowsPos);
					LineTo(hdc,lpTable->width+lpTable->x,lpTable->y+rowsPos);
				}
		}
		if (lpTable->rules|RULES_COLS)
		{
			// draw cols lines
			DWORD i;
			DWORD colsPos=0;
				for (i=1;i<lpTable->cols;i++)
				{
					colsPos+=lpTable->widthPreCol[i-1]+(lpTable->cellSpacing+lpTable->cellPadding)/2;
					MoveTo(hdc,lpTable->x+colsPos,lpTable->y);
					LineTo(hdc,lpTable->x+colsPos,lpTable->y+lpTable->height);
				}
		}
	}

}
/***************************************************************************************/
// Show Table End
/***************************************************************************************/

/***************************************************************************************/
// Show TR
/***************************************************************************************/

void ShowTR(HWND hWnd,HDC hdc,LPTR_TABLE tr,POINT ptOrg)
{
	LPTD_TABLE td;
	COLORREF bgOldColor;

		{
			RECT rect;
				
				GetClientRect(hWnd,&rect);
//				DisplayRect("the TR Rect\r\n",tr->x,tr->y,tr->width,tr->height);
//				if (((tr->y-ptOrg.y)>rect.bottom)||((tr->y-ptOrg.y+tr->height)<rect.top))
			//	if ((tr->y>(DWORD)(ptOrg.y+rect.bottom))||((tr->y+tr->height)<(DWORD)(rect.top+ptOrg.y)))
			//		return;
		}
		if (tr->bgColor!=-1)
		{
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
			rect.left=tr->x-ptOrg.x;
			rect.top=tr->y-ptOrg.y;
			rect.right=tr->x-ptOrg.x+tr->width;
			rect.bottom=tr->y-ptOrg.y+tr->height;
			ShowBackGround(hdc,rect,tr->bgColor);
			bgOldColor=SetBkColor(hdc,tr->bgColor);

		}
		td=tr->td;
		while(td)
		{
//			if (ChecktdIntr(tr,td)==FALSE)
//			{
//				DisplayRect("the TR Rect\r\n",tr->x,tr->y,tr->width,tr->height);
//				DisplayRect("the TD Rect\r\n",td->x,td->y,td->width,td->height);
//			}
			ShowTD(hWnd,hdc,td,ptOrg);  // Renew Locate the td Position
			td=td->next;
		}
		if (tr->bgColor!=-1)
		{
			SetBkColor(hdc,bgOldColor);
		}
}

/***************************************************************************************/
// Show TR End
/***************************************************************************************/


/***************************************************************************************/
// Show TD
/***************************************************************************************/

void ShowTD(HWND hWnd,HDC hdc,LPTD_TABLE td,POINT ptOrg)
{
	LPHTMLOBJECT lpObject;

	COLORREF bgOldColor;
	RECT rect;

	{
			
			GetClientRect(hWnd,&rect);
//			DisplayRect("the TD Rect\r\n",td->x,td->y,td->width,td->height);
//			if (((td->y-ptOrg.y)>rect.bottom)||((td->y-ptOrg.y+td->height)<rect.top))
			if ((td->y>(DWORD)(ptOrg.y+rect.bottom))||((td->y+td->height)<(DWORD)(rect.top+ptOrg.y)))
				return;
	}

	if (td->IsVirtual==TRUE)
		return ;

	lpObject=td->lpItem;
	rect.left=td->x-ptOrg.x;
	rect.top=td->y-ptOrg.y;
	rect.right=td->x-ptOrg.x+td->width;
	rect.bottom=td->y-ptOrg.y+td->height;

	if (td->bgColor!=-1)
	{
		ShowBackGround(hdc,rect,td->bgColor);
		bgOldColor=SetBkColor(hdc,td->bgColor);

	}
	while(lpObject)
	{
		switch(lpObject->type)
		{
		case TYPE_TABLE:
			ShowHtmlTable(hWnd,hdc,(LPHTMLTABLE)lpObject->lpContent,ptOrg);  // Recalc the Control Size
			break;
		case TYPE_CONTROL:
			SMIE_ShowHtmlControl(hWnd,hdc,lpObject->lpContent,rect,ptOrg);  // Show Html Control 
			break;
		default:
			break;
		}
		lpObject=lpObject->next;
	}
	if (td->bgColor!=-1)
	{
		SetBkColor(hdc,bgOldColor);
	}
}

/***************************************************************************************/
// Show TD End
/***************************************************************************************/


/***************************************************************************************/
// Release Table
/***************************************************************************************/
void ReleaseTable(LPHTMLTABLE lpTable)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	tr=lpTable->head;  // Release the head tr of the table
	while(tr)
	{
		nextTR=tr->next;
		ReleaseTR(tr);
		tr=nextTR;
	}

	tr=lpTable->body; // Release the body tr of the table
	while(tr)
	{
		nextTR=tr->next;
		ReleaseTR(tr);
		tr=nextTR;
	}

	tr=lpTable->foot; // Release the foot tr of the table
	while(tr)
	{
		nextTR=tr->next;
		ReleaseTR(tr);
		tr=nextTR;
	}

	lpObject=lpTable->lpItem; // Release the Control of the table
	ReleaseObject(lpObject);

	if (lpTable->heightPreRow!=0)
		free(lpTable->heightPreRow);
	if (lpTable->widthPreCol!=0)
		free(lpTable->widthPreCol);
	free(lpTable);
	return ;
}
/***************************************************************************************/
// Release Table End
/***************************************************************************************/

/***************************************************************************************/
// Release TR
/***************************************************************************************/
void ReleaseTR(LPTR_TABLE tr)
{
	LPTD_TABLE td,lpNext;
//	COLORREF bgOldColor;

		td=tr->td;
		while(td)
		{
			lpNext=td->next;
			ReleaseTD(td);  // Renew Locate the td Position
			td=lpNext;
		}
		free(tr);
}
/***************************************************************************************/
// Release TR End
/***************************************************************************************/

/***************************************************************************************/
// Release TD
/***************************************************************************************/
void ReleaseTD(LPTD_TABLE td)
{
	LPHTMLOBJECT lpObject;
//	LPTD_TABLE lpNext;

//	while(td)
//	{
//		lpNext=td->next;
		lpObject=td->lpItem;
		if (lpObject)
			ReleaseObject(lpObject);
		free(td);
//		td=lpNext;
//	}
}
/***************************************************************************************/
// Release TD End
/***************************************************************************************/

/***************************************************************************************/
// Release Object
/***************************************************************************************/
void ReleaseObject(LPHTMLOBJECT lpObject)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;
		switch(lpObject->type)
		{
		case TYPE_TABLE:
			ReleaseTable((LPHTMLTABLE)lpObject->lpContent);  // Release Table
			break;
		case TYPE_CONTROL:
			SMIE_ReleaseControl(lpObject->lpContent);  // Release Control 
			break;
		default:
			break;
		}
		free(lpObject);
		lpObject=lpNext;
	}
}
/***************************************************************************************/
// Release Object End
/***************************************************************************************/


/*************************************************************************************/
// Show Background
/*************************************************************************************/
void ShowBackGround(HDC hdc,RECT rect,COLORREF color)
{
	HBRUSH hBrush=(HBRUSH)CreateSolidBrush(color);
//		hBrush=(HBRUSH)SelectObject(hdc,hBrush);
//		if (color)
		FillRect(hdc,&rect,hBrush);
 		DeleteObject(hBrush);
}

/*************************************************************************************/
// Show Background End
/*************************************************************************************/


BOOL GetUrl(HHTML hHtml,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;
	
	lpHead=(LPHTMLHEAD)hHtml;

	if (lpHead==NULL)
		return FALSE;
	lpCurObject=lpHead->next;

	return GetUrlInObject(lpCurObject,x,y,lppUrl);
}

/*************************************************************************************/
// Get Url in the Object
/*************************************************************************************/
BOOL GetUrlInObject(LPHTMLOBJECT lpObject,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;
		switch(lpObject->type)
		{
		case TYPE_TABLE:
			if(GetUrlInTable((LPHTMLTABLE)lpObject->lpContent,x,y,lppUrl)==TRUE)
			{
				return TRUE;
			}
			break;
		case TYPE_CONTROL:
			if (SMIE_GetUrlInControl((LPHTMLTABLE)lpObject->lpContent,x,y,lppUrl)==TRUE)
			{
				return TRUE;
			}
			break;
		default:
			break;
		}
		lpObject=lpNext;
	}
	return FALSE;
}
/*************************************************************************************/
// Get Url in the Object end
/*************************************************************************************/


/*************************************************************************************/
// Get Url in the Table
/*************************************************************************************/
BOOL GetUrlInTable(LPHTMLTABLE lpTable,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	if (lpTable->x>x || x>lpTable->x+lpTable->width)
		return FALSE;

	if (lpTable->y>y || y>lpTable->y+lpTable->height)
		return FALSE;

	tr=lpTable->head;  // Release the head tr of the table
	while(tr)
	{
		nextTR=tr->next;
		if (GetUrlInTR(tr,x,y,lppUrl)==TRUE)
		{
			return TRUE;
		}
		tr=nextTR;
	}

	tr=lpTable->body; // Release the body tr of the table
	while(tr)
	{
		nextTR=tr->next;
		if (GetUrlInTR(tr,x,y,lppUrl)==TRUE)
		{
			return TRUE;
		}
		tr=nextTR;
	}

	tr=lpTable->foot; // Release the foot tr of the table
	while(tr)
	{
		nextTR=tr->next;
		if (GetUrlInTR(tr,x,y,lppUrl)==TRUE)
		{
			return TRUE;
		}
		tr=nextTR;
	}

	lpObject=lpTable->lpItem; // Release the Control of the table
	if (GetUrlInObject(lpObject,x,y,lppUrl)==TRUE)
	{
		return TRUE;
	}

	return FALSE;
}
/*************************************************************************************/
// Get Url in the Table End
/*************************************************************************************/


/*************************************************************************************/
// Get Url in the TR
/*************************************************************************************/
BOOL GetUrlInTR(LPTR_TABLE tr,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPTD_TABLE td,lpNext;

	// !!! delete by jami chen in 2002.01.09 ,because the td have colSpan and rowSpan >1, then
	// the td->width > tr->witdh and td->height > tr->height ,so ...
//		if (tr->x>x || x>tr->x+tr->width)
//			return FALSE;

//		if (tr->y>y || y>tr->y+tr->height)
//			return FALSE;
	// !!! delete end by Jami chen 2002.01.09
		td=tr->td;
		while(td)
		{
			lpNext=td->next;
			if (GetUrlInTD(td,x,y,lppUrl)==TRUE)
			{
				return TRUE;
			}
			td=lpNext;
		}
		return FALSE;
}
/*************************************************************************************/
// Get Url in the TR End
/*************************************************************************************/


/*************************************************************************************/
// Get Url in the TD
/*************************************************************************************/
BOOL GetUrlInTD(LPTD_TABLE td,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLOBJECT lpObject;

	if (td->x>x || x>td->x+td->width)
		return FALSE;

	if (td->y>y || y>td->y+td->height)
		return FALSE;
	lpObject=td->lpItem;
	if (lpObject)
	{
		if (GetUrlInObject(lpObject,x,y,lppUrl)==TRUE)
		{
			return TRUE;
		}
	}
	return FALSE;
}
/*************************************************************************************/
// Get Url in the TD End
/*************************************************************************************/

/*************************************************************************************/
// Get Html Size
/*************************************************************************************/
BOOL GetHtmlSize(HHTML hHtml,LPSIZE lpSize)
{
	LPHTMLHEAD lpHead;

		if(lpSize==NULL)  // Test the Parameter Correct
			return FALSE;
		lpHead=(LPHTMLHEAD)hHtml;
		if (lpHead==NULL)
			return FALSE;

		lpSize->cx=lpHead->width;
		lpSize->cy=lpHead->height;

		return TRUE;
}
/*************************************************************************************/
// Get Html Size
/*************************************************************************************/
BOOL GetMarkPos(HHTML hHtml,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;
	
	lpHead=(LPHTMLHEAD)hHtml;

	if (lpHead==NULL)
		return FALSE;
	lpCurObject=lpHead->next;

	return GetMarkPosInObject(lpCurObject,lpxPos,lpyPos,lpMark);
}
BOOL GetMarkPosInTable(LPHTMLTABLE lpTable,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	tr=lpTable->head;  // Release the head tr of the table
	while(tr)
	{
		nextTR=tr->next;
		if (GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)
		{
			return TRUE;
		}
		tr=nextTR;
	}

	tr=lpTable->body; // Release the body tr of the table
	while(tr)
	{
		nextTR=tr->next;
		if (GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)
		{
			return TRUE;
		}
		tr=nextTR;
	}

	tr=lpTable->foot; // Release the foot tr of the table
	while(tr)
	{
		nextTR=tr->next;
		if (GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)
		{
			return TRUE;
		}
		tr=nextTR;
	}

	lpObject=lpTable->lpItem; // Release the Control of the table
	if (GetMarkPosInObject(lpObject,lpxPos,lpyPos,lpMark)==TRUE)
	{
		return TRUE;
	}

	return FALSE;
}
BOOL GetMarkPosInTR(LPTR_TABLE tr,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPTD_TABLE td,lpNext;

		td=tr->td;
		while(td)
		{
			lpNext=td->next;
			if (GetMarkPosInTD(td,lpxPos,lpyPos,lpMark)==TRUE)
			{
				return TRUE;
			}
			td=lpNext;
		}
		return FALSE;
}
BOOL GetMarkPosInTD(LPTD_TABLE td,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLOBJECT lpObject;

	lpObject=td->lpItem;
	if (lpObject)
	{
		if (GetMarkPosInObject(lpObject,lpxPos,lpyPos,lpMark)==TRUE)
		{
			return TRUE;
		}
	}
	return FALSE;
}
BOOL GetMarkPosInObject(LPHTMLOBJECT lpObject,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;
		switch(lpObject->type)
		{
		case TYPE_TABLE:
			if(GetMarkPosInTable((LPHTMLTABLE)lpObject->lpContent,lpxPos,lpyPos,lpMark)==TRUE)
			{
				return TRUE;
			}
			break;
		case TYPE_CONTROL:
			if (SMIE_GetMarkPosInControl((LPHTMLTABLE)lpObject->lpContent,lpxPos,lpyPos,lpMark)==TRUE)
			{
				return TRUE;
			}
			break;
		default:
			break;
		}
		lpObject=lpNext;
	}
	return FALSE;
}
