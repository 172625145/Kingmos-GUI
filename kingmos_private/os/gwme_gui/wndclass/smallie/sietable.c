/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：小屏幕模式的IE浏览类处理TABLE
版本号：1.0.0.456
开发时期：2001-02-16
作者：陈建明 Jami 
修改记录：
**************************************************/
#include "eHtmView.h"
#include "SIETable.h"
#include "Locate.h"
#include "Control.h"
#include "ViewDef.h"

//extern DWORD iPage,iMove;
// **************************************************
// 定义区
// **************************************************

#define PERCENTTAG  10000
//#define MAXWIDTH 10000
#define MAXHEIGHT 10000

// **************************************************
// 函数声明区
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
// 声明：HHTML SMIE_ParseHtml(HWND hWnd,char *lpHtmlBuffer)
// 参数：
// 	IN hWnd  -- 窗口句柄
// 	IN lpHtmlBuffer  -- 一个HTML内容的缓存
// 
// 返回值：成功返回一个已经分析好的HTML句柄，否则返回NULL
// 功能描述：分析一个HTML缓存。
// 引用: 
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
	// 初始化控件结构
	structControlState.color=0;
	structControlState.font=NULL;
	structControlState.url=NULL;
	structControlState.name=NULL;
	structControlState.x=0;
	structControlState.y=0;
	structControlState.bPreProcess=FALSE;

	// 分配一个HTML HEADER
	lpHead=(LPHTMLHEAD)malloc(sizeof(HTMLHEAD));  
	if (lpHead==NULL)
	{  // 分配内存失败
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// 初始化结构
	lpHead->next=NULL;
	lpHead->width=0;
	lpHead->height=0;

	lpCurObject=lpHead->next;

//	width_up=GetSystemMetrics(SM_CXSCREEN);

	// 开始分析
	while(*lpPtr)
	{
		tagID=SMIE_LocateTag(&lpPtr);  // 得到标签标号
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
			lpObject=SMIE_ParseTable(hWnd,&lpPtr);  // 分析一个TABLE
			// 将分析好的TABLE插入到当前控制中
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
			lpObject=SMIE_ParseHtmlControl(hWnd,tagID,&lpPtr,&structControlState); // 分析一个HTML控制
			if (lpObject)
			{  // 插入控制到当前控制中
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

	// 释放空间
	if (structControlState.font)
		free(structControlState.font);
	if (structControlState.url)
		free(structControlState.url);
	if (structControlState.name)
		free(structControlState.name);
	return lpHead; // 返回
}

// **************************************************
// 声明：BOOL SMIE_ReCalcSize(HWND hWnd,HDC hdc,HHTML hHtml)  
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN hHtml -- HTML句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：计算HTML需要的尺寸。
// 引用: 
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
	width_up=GetSystemMetrics(SM_CXSCREEN);  // 得到屏幕大小
//	width_up=700;
	size=SMIE_ReCalcSize_Object(hWnd,hdc,lpCurObject,width_up); // 得到目标尺寸
	// 保存目标尺寸
	lpHead->width=size.cx;
	lpHead->height=size.cy;

	SMIE_ReLocatePosition(hWnd,lpHead);  // 重新定位各控件位置
/*
#ifndef __WCE_DEFINE
	font=(HFONT)SelectObject(hdc,font);
	DeleteObject(font);
#endif
*/
	return TRUE; // 返回成功
}
// **************************************************
// 声明：BOOL SMIE_ReLocatePosition(HWND hWnd,LPHTMLHEAD lpHead)
// 参数：
//  IN hWnd -- 窗口句柄
// 	IN lpHead -- HTML 头结构
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：重新定位各控制的位置。
// 引用: 
// **************************************************
BOOL SMIE_ReLocatePosition(HWND hWnd,LPHTMLHEAD lpHead)
{
	LPHTMLOBJECT lpCurObject;
	DWORD x,y,width;
	RECT rect;
	
	JAMIMSG(DBG_FUNCTION,(TEXT("Enter ReLocatePosition\r\n")));

	lpCurObject=lpHead->next;

	// 起始坐标为(0,0)
	x=y=0;
	width=775;  // 设置一个大宽度

	// 初始化结构
	rect.left=x;
	rect.right=rect.left+width;
	rect.top=0;
//	rect.bottom=10000;
	rect.bottom=lpHead->height;
	SMIE_ReLocate_Object(hWnd,lpCurObject,0,0,rect); // 重新定位目标

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
// 声明：void SMIE_ShowHtml(HWND hWnd,HDC hdc,HHTML hHtml,POINT ptOrg)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN hHtml -- HTML句柄
// 	IN ptOrg -- 显示原点
// 
// 返回值：无
// 功能描述：显示一个HTML。
// 引用: 
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
	GetClientRect(hWnd,&rect);  // 得到客户矩形
	SMIE_ShowBackGround(hdc,rect,RGB(0xff,0xff,0xff)); // 画背景

	lpHead=(LPHTMLHEAD)hHtml;  // 得到HTML头结构
	if (lpHead==NULL)
		return;		
	lpCurObject=lpHead->next; // 得到当前控制


	while(lpCurObject)
	{
		switch(lpCurObject->type)
		{
		case TYPE_TABLE: // 是一个TABLE
			SMIE_ShowHtmlTable(hWnd,hdc,(LPHTMLTABLE)lpCurObject->lpContent,ptOrg);  // Show Table
			break;
		case TYPE_CONTROL: // 是一个控制
//			rect.left=0;
//			rect.right=10000;
//			rect.top=0;
			rect.bottom=10000;
			SMIE_ShowHtmlControl(hWnd,hdc,lpCurObject->lpContent,rect,ptOrg);  // Show  the Control 
			break;
		default:
			break;
		}
		lpCurObject=lpCurObject->next; // 到下一个控制
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
// 声明：void SMIE_ReleaseHtml(HHTML hHtml)
// 参数：
// 	IN hHtml -- HTML句柄
// 
// 返回值：无
// 功能描述：释放HTML控件。
// 引用: 
// **************************************************
void SMIE_ReleaseHtml(HHTML hHtml)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;//,lpNext;
	

	if (hHtml==NULL)
		return;

	lpHead=(LPHTMLHEAD)hHtml; // 得到头结构
	lpCurObject=lpHead->next; // 得到当前控制

	SMIE_ReleaseObject(lpCurObject); // 释放当前控制
	free(lpHead); // 释放头结构

}

// **************************************************
// 声明：LPHTMLOBJECT SMIE_ParseHtmlHead(char **lpHtmlBuffer)
// 参数：
// 	IN/OUT lpHtmlBuffer -- 数据流
// 
// 返回值：返回一个HTML控制
// 功能描述：分析HTML头数据
// 引用: 
// **************************************************
LPHTMLOBJECT SMIE_ParseHtmlHead(char **lpHtmlBuffer)
{
	char **lpPtr;
	TAGID tagID;

		JAMIMSG(DBG_FUNCTION,(TEXT("Enter ParseHtmlHead\r\n")));

		lpPtr=lpHtmlBuffer; // 得到当前指针
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
	SMIE_ToNextTag(lpPtr); // 到下一个标签开始位置

	// Get item Content
	while(**lpPtr)
	{
		tagID=SMIE_LocateTag(lpPtr); // 得到标签标号
		if (tagID==TAGID_HEAD+TAGID_END)  // </HEAD>
		{ // HTML头结束
			SMIE_ToNextTag(lpPtr); // 到下一个标签开始位置
			break;
		}
		SMIE_ToNextTag(lpPtr);// 到下一个标签开始位置
	}
	return NULL;
}


// **************************************************
// 声明：LPHTMLOBJECT SMIE_ParseTable(HWND hWnd,char **lpHtmlBuffer)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN/OUT lpHtmlBuffer -- HTML数据流
// 
// 返回值：返回一个分析好的控制
// 功能描述：分析一个TABLE。
// 引用: 
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

	// 初始化控制结构
	structControlState.color=0;
	structControlState.font=NULL;
	structControlState.url=NULL;
	structControlState.name=NULL;
	structControlState.x=0;
	structControlState.y=0;
	structControlState.bPreProcess=FALSE;

	// 分配一个属性内容空间
	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
		return NULL; // 分配内存失败

	// 分配一个目标结构空间
	lpHtmlObject=(LPHTMLOBJECT)malloc(sizeof(HTMLOBJECT));
	if (lpHtmlObject==NULL)
	{ // 分配内存失败
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	
	// 分配一个TABLE结构空间
	lpHtmlTable=(LPHTMLTABLE)malloc(sizeof(HTMLTABLE));
	if (lpHtmlTable==NULL)
	{ // 分配失败
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}

	// 初始化目标结构
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
	
	lpCurHead=lpHtmlTable->head;  // 得到当前TABLE头
	lpCurBody=lpHtmlTable->body; // 得到当前TABLE内容
	lpCurFoot=lpHtmlTable->foot; // 得到当前TABLE脚
	lpCurObject=lpHtmlTable->lpItem; // 得到当前TABLE的条目

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
			tr=SMIE_ParseTR(hWnd,lpPtr,lpHtmlTable);  // 分析一个TR
			if (tr)
			{
				if (thePart == BODY_PART)
				{  // 是BODY部分，将TR插入到BODY
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
				{ // 是HEAD部分，将TR插入到HEAD
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
				{ // 是FOOT部分，将TR插入到FOOT
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
			lpControl=SMIE_ParseHtmlControl(hWnd,tagID,lpPtr,&structControlState);  // 分析一个控制
			if (lpControl)
			{  // 插入当前控制
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
	SMIE_AdjustTable(lpHtmlTable); // 调整TABLE

	// malloc  heightPreRow
	lpHtmlTable->heightPreRow=(LPDWORD)malloc(sizeof(DWORD)*(lpHtmlTable->rows+10)); 
	if (lpHtmlTable->heightPreRow==NULL)
	{  // 分配内存失败
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// 初始化heightPreRow
	for (i=0;i<lpHtmlTable->rows;i++)
		lpHtmlTable->heightPreRow[i]=0;

	// malloc widthPreCol
	lpHtmlTable->widthPreCol=(LPDWORD)malloc(sizeof(DWORD)*(lpHtmlTable->cols+10));
	if (lpHtmlTable->widthPreCol==NULL)
	{  // 分配内存失败
		if (lpHtmlTable->widthPreCol)
			free(lpHtmlTable->widthPreCol);
		if (lpHtmlObject)
			free(lpHtmlObject);
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// 初始化widthPreCol
	for (i=0;i<lpHtmlTable->cols;i++)
		lpHtmlTable->widthPreCol[i]=0;

	JAMIMSG(DBG_FUNCTION,(TEXT("Parse Table Success\r\n")));
	// 释放不需要的空间
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
// 声明：LPTR_TABLE SMIE_ParseTR(HWND hWnd,char **lpHtmlBuffer,LPHTMLTABLE  lpHtmlTable)
// 参数：
// 	IN hWnd -- 窗口句柄 
// 	IN/OUT lpHtmlBuffer -- HTML 数据流
// 	IN lpHtmlTable -- TABLE 结构
// 
// 返回值：返回一个TR结构
// 功能描述：分析TABLE 中的TR。
// 引用: 
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

	// 分配一个属性内容空间
	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
	{ // 分配内存失败
		return NULL;
	}
	// 分配一个TR结构
	tr=(LPTR_TABLE)malloc(sizeof(TR_TABLE)); // malloc tr
	if (tr==NULL)  // the failure to malloc tr
	{  // 分配内存失败
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// 初始化TR结构
	tr->next=NULL;
	tr->height=0;
	tr->width=0;
	tr->td=NULL;
	tr->bgColor=-1;
	lpCurTD=tr->td;  // 指向当前TD

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

	SMIE_ToNextTag(lpPtr);  // 到下一个标签位置
	// Get td Content
	while(**lpPtr)
	{
		tagID=SMIE_NextTagIs(*lpPtr);  // 得到下一个标签标号
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

		tagID=SMIE_LocateTag(lpPtr);  // 得到当前标签标号
		if (tagID==TAGID_TR+TAGID_END) // <TR>
		{  // This is Next TR
			SMIE_ToNextTag(lpPtr);  // 到下一个标签
			break;
		}
		if (tagID==TAGID_TD)  // <TD>
		{// Insert a TD to current Row
			cols++;  // have a new cols
//			if (tr->width)
//				width_up=tr->width;
//			td=ParseTD(lpPtr,width_up);
			JAMIMSG(DBG_FUNCTION,(TEXT("Parse TD \r\n")));
			td=SMIE_ParseTD(hWnd,lpPtr);  // 分析一个TD
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
			td=SMIE_ParseTD(hWnd,lpPtr);  // 分析一个TD
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
			SMIE_ToNextTag(lpPtr);  // 到下一个标签的位置
//			break;
		}
	}
	// 保存列数
	if (lpHtmlTable->cols<cols)
		lpHtmlTable->cols=cols;
	// if the parent width is know and only one son's width is unKnow in the sons
	// then the last son's width is know 
//	if (width_up)
	if (lpHtmlTable->width)
	{  // TABLE 有指定宽度 ，必须调整TR TD 的宽度
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
		{  // 没有指定宽度的TD数是1个，则将该TD的宽度为剩余宽度
//			if (width_up>width)
//				lpCurTD->width=width_up-width;
			if (lpHtmlTable->width>width)
				lpCurTD->width=lpHtmlTable->width-width;  
		}
	}
	// 释放无用的内存
	if (pPropertyContent)
		free(pPropertyContent);
	return tr;  // 成功返回
}

// **************************************************
// 声明：LPTD_TABLE SMIE_ParseTD(HWND hWnd,char **lpHtmlBuffer)
// 参数：
// 	IN hWnd -- 窗口句柄 
// 	IN/OUT lpHtmlBuffer -- HTML 数据流
// 
// 返回值：返回分析好的TD结构
// 功能描述：分析一个TD。
// 引用: 
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
	// 分配属性内容空间
	pPropertyContent=(char *)malloc(1024);
	if (pPropertyContent==NULL)
	{ // 分配内存失败
		return NULL;
	}
	// 分配一个TD结构
	td=(LPTD_TABLE)malloc(sizeof(TD_TABLE)); // malloc tr
	if (td==NULL)  // the failure to malloc tr
	{// 分配内存失败
		if (pPropertyContent)
			free(pPropertyContent);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return NULL;
	}
	// 初始化TD结构
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


	lpCurObject=td->lpItem;  // 得到当前的目标控件

	// 初始化控件状态结构
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
			td->bgColor=SMIE_GetColor(pPropertyContent);  // 得到颜色值
			break;
		case PROPERTYID_COLSPAN: // col span
			td->colSpan=atoi(pPropertyContent);  // 得到尺寸
			break;
		case PROPERTYID_ROWSPAN: // row span
			td->rowSpan=atoi(pPropertyContent);  // 得到尺寸
			break;
		case PROPERTYID_WIDTH:  // Process property "width"
			if (SMIE_IsPercent(pPropertyContent)==TRUE)
			{  // 是百分比数据
//				if (width_up)
//					td->width=atoi(pPropertyContent)*width_up/100;
//				else
					td->percent=atoi(pPropertyContent)+PERCENTTAG; // 得到百分比，PERCENTTAG -- 百分比标记
			}
			else
				td->percent=atoi(pPropertyContent);  // 得到实际尺寸
			break;
		case PROPERTYID_HEIGHT:  // Process property "height"
			td->heightSet=atoi(pPropertyContent);  // 得到高度
			break;
		case PROPERTYID_NOWRAP:  // no wrap 
			td->NoWrap=TRUE;
			break;
		}
	}

	JAMIMSG(DBG_FUNCTION,(TEXT("Parse TD Style Success\r\n")));

	SMIE_ToNextTag(lpPtr);  // 到下一个标签
	// Get td Content
	while(**lpPtr)
	{
		tagID=SMIE_NextTagIs(*lpPtr);  // 得到下一个标签标号
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

		tagID=SMIE_LocateTag(lpPtr);  // 得到当前的标签标号
		if (tagID==TAGID_TD+TAGID_END)  // </TD>
		{
			SMIE_ToNextTag(lpPtr);  // 到下一个标签
			break;
		}
		else if (tagID==TAGID_TABLE)  // <TABLE>
		{
//			if (td->width)
//			width_up=td->width;
//			lpControl=ParseTable(lpPtr,width_up);
			lpControl=SMIE_ParseTable(hWnd,lpPtr);  // 分析一个TABLE
			if (lpControl)
			{  // 插入TABLE控制到当前控制
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
			lpControl=SMIE_ParseHtmlControl(hWnd,tagID,lpPtr,&structControlState);  // 分析一个控制
			if (lpControl)
			{  // 插入控制到当前控制
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
	// 释放不需要的内存
	if (pPropertyContent)
		free(pPropertyContent);
	if (structControlState.font)
		free(structControlState.font);
	if (structControlState.url)
		free(structControlState.url);
	if (structControlState.name)
		free(structControlState.name);
	return td;  // 成功返回
}


// **************************************************
// 声明：BOOL SMIE_ReCalcSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE  lpTable,DWORD iTDMaxWidth,BOOL bAdjust)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpTable -- TABLE结构
// 	IN iTDMaxWidth -- TD的最大宽度
// 	IN bAdjust -- 是否需要重新调整
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：重新计算TABLE的尺寸。
// 引用: 
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

	{  // 得到默认的TABLE尺寸
		RECT rect;
			GetClientRect(hWnd,&rect);  // 得到窗口的矩形大小
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
	// 重新计算TABLE的HEAD
	tr=lpTable->head;  // ReCalc Size the head tr of the table
	while(tr)
	{  // 计算每一个TR
		SMIE_ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust);  // 重新计算TR
		lpTable->heightPreRow[iRows]=tr->height;
		iRows++;
		tr=tr->next;
	}

	// 重新计算TABLE的BODY
	tr=lpTable->body; // ReCalc Size the body tr of the table
	while(tr)
	{  // 计算每一个TR
		SMIE_ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust); // 重新计算TR
		lpTable->heightPreRow[iRows]=tr->height;
		iRows++;
		tr=tr->next;
	}

	// 重新计算TABLE的BODY
	tr=lpTable->foot; // ReCalc Size the foot tr of the table
	while(tr)
	{  // 计算每一个TR
		SMIE_ReCalcSize_TR(hWnd,hdc,tr,lpTable,bAdjust);  // 重新计算TR
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
	
	// 得到TABLE的高度
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
	// 得到TABLE的宽度
	lpTable->width=width;
/*
 if (lpTable->width!=0 && lpTable->width < width)
	{
//     This condition, what do it.
//		lpTable->width=width;
	}
*/
//	if (lpTable->height==0)
	// 与TABLE的设置高度比较
	if (lpTable->heightSet<height)
		lpTable->height=height;
	else
		lpTable->height=lpTable->heightSet;
	return TRUE;  // 成功返回
}

// **************************************************
// 声明：void SMIE_AdjustTable(LPHTMLTABLE  lpTable)
// 参数：
// 	IN lpTable -- TABLE结构
// 
// 返回值：无
// 功能描述：调整TABLE。
// 引用: 
// **************************************************
void SMIE_AdjustTable(LPHTMLTABLE  lpTable)
{
	LPTR_TABLE tr;
	DWORD iCol=0;

	tr=lpTable->head;  // ReCalc Size the head tr of the table
	iCol=SMIE_AdjustTR(tr);  // 调整TR
	if (iCol>lpTable->cols) 
		lpTable->cols=iCol;  // 设置最大列数

	tr=lpTable->body; // ReCalc Size the body tr of the table
	iCol=SMIE_AdjustTR(tr);  // 调整TR
	if (iCol>lpTable->cols)
		lpTable->cols=iCol;// 设置最大列数

	tr=lpTable->foot; // ReCalc Size the foot tr of the table
	iCol=SMIE_AdjustTR(tr);  // 调整TR
	if (iCol>lpTable->cols)
		lpTable->cols=iCol; // 设置最大列数


}

// **************************************************
// 声明：DWORD SMIE_AdjustTR(LPTR_TABLE tr)
// 参数：
// 	IN tr -- TR结构
// 
// 返回值： 返回该TR所包含的列数(TD)
// 功能描述：调整TR。
// 引用: 
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
	{  //调整每一个TR
		td=tr->td;
		iCol=0;
		while(td)
		{  //调整每一个Td
			if (td->rowSpan>1)
			{  // TD跨越超过一个TR
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
					SMIE_InsertEmptyTDInTR(curTR,iCol,td->colSpan);  // 插入一个空TD
				}
			}
			iCol+=td->colSpan;  // 该TD所占有的列数
			td=td->next;  // 下一个TD
		}
		if (iCol>iMaxCol)
			iMaxCol=iCol;  // 得到最大的列数
		iCol=0;
		tr=tr->next;  //下一个TR
	}
	return iMaxCol; //返回所包含的列数
}

// **************************************************
// 声明：BOOL SMIE_InsertEmptyTDInTR(LPTR_TABLE tr,DWORD iInsertCol,DWORD colSpan)
// 参数：
// 	IN tr -- TR结构
// 	IN iInsertCol -- 要插入的位置
// 	IN colSpan -- 要插入的TD所占的列数
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：在指定的TR中插入一个空TD。
// 引用: 
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
		// 找到要插入的位置
		while(td)
		{
			if (iCol==iInsertCol)
				break;
			pretd=td;
			td=td->next;
			iCol++;
		}
		// 分配一个TD结构
		newTD=(LPTD_TABLE)malloc(sizeof(TD_TABLE));
		if (newTD==NULL)
		{  // 分配失败
			MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
			return FALSE;
		}

		// 初始化结构
		memset(newTD,0,sizeof(TD_TABLE));

		// 设置要插入TD的内容
		newTD->IsVirtual=TRUE;
		newTD->rowSpan=1;
		newTD->colSpan=colSpan;

		// 插入新的TD
		newTD->next=td;
		if (iCol==0)
		{
			tr->td=newTD;
		}
		else
		{
			pretd->next=newTD;
		}

		return TRUE;  // 成功返回
}


// **************************************************
// 声明：void SMIE_CompressSize_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD widthCompress,DWORD iTDMaxWidth)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpTable -- TABLE结构
// 	IN widthCompress -- 要压缩的宽度
// 	IN iTDMaxWidth -- TD的最大宽度
// 
// 返回值：
// 功能描述：
// 引用: 
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
	// 分配一个数组来保存每一个TD所能达到的最小宽度
	lpMinWidth=(LPDWORD)malloc(lpTable->cols*sizeof(DWORD));
	if (lpMinWidth==NULL)
	{ // 分配失败
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return ;
	}
	// 分配一个数组来保存原来的宽度
	lpBackup=(LPDWORD)malloc(lpTable->cols*sizeof(DWORD));
	if (lpBackup==NULL)
	{  // 分配失败
		free(lpMinWidth);
		MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
		return ;
	}
	// 保存原来的宽度，并得到每一个TD的最小宽度
	memset(lpMinWidth,0,lpTable->cols*sizeof(DWORD));
	while(1)
	{
		// 保存原来的宽度
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			lpBackup[i]=lpTable->widthPreCol[i];
		}
		// 得到每一个TD的最小宽度
		tr=lpTable->body;
		while(tr)
		{
			td=tr->td;
			iCol=0;
			while(td)
			{
				if (td->NoWrap==TRUE)
				{  // 如果定义了NoWrap，则最小宽度为设定的宽度，否则为0
					if (lpMinWidth[iCol]<td->width)
						lpMinWidth[iCol]=td->width;
				}
				iCol+=td->colSpan;
				td=td->next;
			}
			tr=tr->next;
		}
		// 得到TABLE可以最大的压缩宽度
		width=0;
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			width+=lpTable->widthPreCol[i]-lpMinWidth[i];  // 得到每一个TD可压缩的宽度，当前宽度 - 最小宽度
		}

		if (width==0)
		{  // 不可再压缩，退出
			free(lpMinWidth);
			free(lpBackup);
			return;
		}
		// 开始压缩
		for (i=0;i<lpTable->cols;i++)		
		{
			// 得到每一个TD需要压缩的宽度，将总压缩宽度平均分配到每一个TD
			tdWidthCompress=widthCompress*(lpTable->widthPreCol[i]-lpMinWidth[i])/width;
			if (lpTable->widthPreCol[i]<tdWidthCompress)
				lpTable->widthPreCol[i]=0;
			else
				lpTable->widthPreCol[i]-=tdWidthCompress;
		}
		
		// 重新计算TABLE的宽度
		SMIE_ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
		// 再次过滤掉一些不能压缩的TD，重新计算最小的宽度
		for (i=0;i<lpTable->cols;i++)		
		{
			tdWidthCompress=widthCompress*(lpBackup[i]-lpMinWidth[i])/width;
			if (lpTable->widthPreCol[i]==0||(lpBackup[i]-lpTable->widthPreCol[i])<tdWidthCompress)
				lpMinWidth[i]=lpTable->widthPreCol[i];  
			else
				lpMinWidth[i]=0;
		}
		//查看是否还需要压缩
		for (i=0;;i++)
		{
			if (i==lpTable->cols)
			{  // 不需要再压缩，退出
				free(lpMinWidth);
				free(lpBackup);
				return ;
			}
			if (lpMinWidth[i]!=0)
				break;
		}
		// 查看是否还能压缩
		for (i=0;;i++)
		{
			if (i==lpTable->cols)
			{  // 不能再压缩，退出
				free(lpMinWidth);
				free(lpBackup);
				return ;
			}
			if (lpBackup[i]!=lpTable->widthPreCol[i])
				break;
		}
		if (width<widthPreCompresss)  // 最大压缩宽度小于可压缩的宽度，退出
			break;
		
		// 得到TABLE的总宽度
		width=0;
		for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
		{
			width+=lpTable->widthPreCol[i];
		}
		width+=lpTable->cellSpacing*(lpTable->cols-1);
		
		widthPreCompresss=width;  // 保留TABLE宽度
		if (width<lpTable->width)
			break;

		widthCompress=width-lpTable->width;  // 重新得到压缩宽度
	}
	free(lpMinWidth);
	free(lpBackup);
	// 完成返回
}

// **************************************************
// 声明：void SMIE_Increase_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD iTDMaxWidth)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpTable -- TABLE结构
// 	IN iTDMaxWidth -- TD的最大宽度
// 
// 返回值：无
// 功能描述：放大TABLE的宽度
// 引用: 
// **************************************************
void SMIE_Increase_Table(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,DWORD iTDMaxWidth)
{
	DWORD width,widthCompress,tdWidthCompress;
	DWORD i;

	if (lpTable->cols==0)
		return;
	// 得到当前TABLE的宽度
	width=0;
	for (i=0;i<lpTable->cols;i++)		// the table width is total of the col width
	{
		width+=lpTable->widthPreCol[i];
	}
	width+=lpTable->cellSpacing*(lpTable->cols-1);

	// 得到要放大的宽度
	widthCompress=lpTable->width-width;

	if (width!=0)
	{  // 得到当前TABLE的宽度不为0
		// 根据每一个TD原来的大小按比例分配放大的尺寸
		for (i=0;i<lpTable->cols;i++)		
		{
			tdWidthCompress=widthCompress*lpTable->widthPreCol[i]/width;
			lpTable->widthPreCol[i]+=tdWidthCompress;
		}
	}
	else
	{  // 当前TABLE的宽度为0
		// 平均分配每一个TD的宽度
		widthCompress /=lpTable->cols;
		for (i=0;i<lpTable->cols;i++)		
		{
			lpTable->widthPreCol[i]+=widthCompress;
		}
	}	
//	SMIE_ReCalcSize_Table(hdc,lpTable,lpTable->width,FALSE);
	// 重新计算TABLE的大小
	SMIE_ReCalcSize_Table(hWnd,hdc,lpTable,iTDMaxWidth,FALSE);
}

// **************************************************
// 声明：BOOL SMIE_ReCalcSize_TR(HWND hWnd,HDC hdc,LPTR_TABLE tr,LPHTMLTABLE  lpTable,BOOL bAdjust)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN tr -- TR结构
// 	IN lpTable -- TR所属TABLE结构
// 	IN bAdjust -- 是否需要调整
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：重新计算TR的尺寸。
// 引用: 
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
		// 得到TABLE的最大尺寸
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
			// 重新计算每一个TD的尺寸
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
			td=td->next;  // 到下一个TD
		}
		//得到TR的宽度和高度
		width=tdWidth;       // the width of the tr is total width all td with the tr
		tr->width=width;
		tr->height=height;
		return TRUE; // 成功返回
}
// **************************************************
// 声明：BOOL SMIE_ReCalcSize_TD(HWND hWnd,HDC hdc,LPTD_TABLE td,DWORD iTDMaxWidth)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN td -- TD结构
// 	IN iTDMaxWidth -- TD的最大宽度
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：重新计算TD的尺寸。
// 引用: 
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

	if (td->IsVirtual==TRUE)  // 这是一个虚的TD，不需要就是尺寸
		return TRUE;

	lpObject=td->lpItem;

//	if (td->width)
//		iTDMaxWidth=td->width;

	td->height=0;

	size=SMIE_ReCalcSize_Object(hWnd,hdc,lpObject,iTDMaxWidth);  // 重新计算目标的尺寸
	td->width=iTDMaxWidth;
//	if (td->width<size.cx)
//		td->width=size.cx;
	// 得到TD的尺寸
	if (td->heightSet<(DWORD)size.cy)
		td->height=(DWORD)size.cy;
	else
		td->height=td->heightSet;
	return TRUE;  // 成功返回

}


// **************************************************
// 声明：SIZE SMIE_ReCalcSize_Object(HWND hWnd,HDC hdc,LPHTMLOBJECT lpObject,DWORD iTDMaxWidth)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpObject -- 目标控制的结构
// 	IN iTDMaxWidth -- 最大宽度
// 
// 返回值：返回目标控制的尺寸
// 功能描述：重新计算目标控制的尺寸
// 引用: 
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

	
	iHeightLine=SMIE_GetFontHeight(hWnd,NULL);// 得到字体的高度
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
	{ // 计算每一个控制的尺寸
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // 当前控制是一个TABLE
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
		case TYPE_CONTROL: // 当前控制是一个一般控制
//			ReCalcSize_Control(hdc,lpObject->lpContent,iRemainWidth);  // Recalc the Control Size
//			ReCalcSize_Control(hdc,lpObject->lpContent,0);  // Recalc the Control Size
			GetClientRect(hWnd,&rect); // 得到窗口的尺寸
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
					iLastRowHeight = ((LPHTMLCONTROL)(lpObject->lpContent))->heightLine; // 最后一行文本的高度
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
			height+=iLastRowHeight;   // 增加最后一行的高度
//			else  // Is a empty line
//				height+=14;
			iLastRowHeight=iHeightLine;  // 设置最后一行的高度为一般字体高度
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
	// 设置目标控制的尺寸	
	size.cx=widthTD;
	size.cy=height;
	return size; // 成功返回
}
// **************************************************
// 声明：void SMIE_AdjustSize_Table(LPHTMLTABLE  lpTable)
// 参数：
// 	IN lpTable -- TABLE结构
// 
// 返回值：无
// 功能描述：重新调整TABLE的尺寸。
// 引用: 
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
// 声明：void SMIE_AdjustSize_TR(LPTR_TABLE tr,LPHTMLTABLE  lpTable)
// 参数：
// 	IN tr -- TR结构
// 	IN lpTable -- TABLE结构
// 
// 返回值：无
// 功能描述：重新调整TR的尺寸。
// 引用: 
// **************************************************
void SMIE_AdjustSize_TR(LPTR_TABLE tr,LPHTMLTABLE  lpTable)
{
	LPTD_TABLE td;
	DWORD iRow,iHeight=0;
	DWORD iCols;//,tdWidth;
	DWORD i;

	iRow=0;
	while(tr)
	{  // 调整每一个TR的尺寸
		td=tr->td; //得到TR的第一个TD
		iCols=0;
		while(td)
		{   // 调整每一个TD
			if (td->rowSpan>1)
			{  // 该TD跨越超过一个TR
				iHeight=0;
				for (i=iRow;i<iRow+td->rowSpan;i++)
				{  // 计算TD的总高度
					iHeight+=lpTable->heightPreRow[i];
				}
				if (iHeight<td->height)  // 总高度小于TD的设定，则调高最后一行的高度
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
			iCols+=td->colSpan;  // 计算总列数
			td=td->next; // 指向下一个TD
		}
		tr=tr->next;  //指向下一个TR
		iRow++;
	}
}


// **************************************************
// 声明：BOOL SMIE_ReLocate_Table(HWND hWnd,LPHTMLTABLE lpTable,LPDWORD lpx,LPDWORD lpy,LPDWORD lpiMaxWidth,LPDWORD iTableHeight)
// 参数：
//  IN hWnd -- 窗口句柄
// 	IN lpTable -- TABLE结构
// 	IN/OUT lpx -- TABLE表的开始X位置，并返回下一个控制的位置
// 	IN/OUT lpy -- TABLE表的开始Y位置，并返回下一个控制的位置
// 	IN/OUT lpiMaxWidth  -- 保留
// 	IN/OUT iTableHeight -- TABLE的高度
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：重新定位TABLE。
// 引用: 
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
		lpTable->x=*lpx;  // 设定TABLE的开始位置
		lpTable->y=*lpy;
//		if (*iTableHeight>lpTable->height)
//			*lpy+=*iTableHeight;
//		else
		*lpy+=lpTable->height;  // 计算下一个控制的位置
		*iTableHeight=0;
	}


	x=lpTable->x;
	y=lpTable->y;
	tr=lpTable->head;  // ReLocate the head tr of the table
	while(tr)
	{
		SMIE_ReLocate_TR(hWnd,tr,lpTable,y);  // 重新定位TR
		y+=tr->height;    // 得到下一个TR的位置
		y+=lpTable->cellSpacing;
		tr=tr->next;    // 指向下一个TR
	}

	tr=lpTable->body; // ReLocate the body tr of the table
	while(tr)
	{
		SMIE_ReLocate_TR(hWnd,tr,lpTable,y);  // 重新定位TR
		y+=tr->height;    // 得到下一个TR的位置
		y+=lpTable->cellSpacing;
		tr=tr->next;    // 指向下一个TR
	}

	tr=lpTable->foot; // ReLocate the foot tr of the table
	while(tr)
	{
		SMIE_ReLocate_TR(hWnd,tr,lpTable,y);  // 重新定位TR
		y+=tr->height;    // 得到下一个TR的位置
		y+=lpTable->cellSpacing;
		tr=tr->next;   // 指向下一个TR
	}

	lpObject=lpTable->lpItem; // ReLocate the Control of the table

	rect.left=lpTable->x;
	rect.right=rect.left+lpTable->width;
	rect.top=lpTable->y;
	rect.bottom=rect.top+lpTable->height;
	SMIE_ReLocate_Object(hWnd,lpObject,x,y,rect);  // 重新定位控制

	return TRUE;
}


// **************************************************
// 声明：BOOL SMIE_ReLocate_TR(hWnd,LPTR_TABLE tr,LPHTMLTABLE  lpTable,DWORD y)
// 参数：
//  IN hWnd -- 窗口句柄
// 	IN tr -- TR结构
// 	IN lpTable -- TABLE结构
// 	IN y -- 当前TR的开始Y坐标
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：重新定位TR的位置。
// 引用: 
// **************************************************
BOOL SMIE_ReLocate_TR(HWND hWnd,LPTR_TABLE tr,LPHTMLTABLE  lpTable,DWORD y)
{
	DWORD x;
	LPTD_TABLE td;
	DWORD iCols=0;
	//int i;

	x=lpTable->x;  // 得到TR的X坐标
	tr->x=x;  // 设置TR的坐标
	tr->y=y;
//	y=y;


		td=tr->td;  //得到第一个TD
		while(td)
		{  // 定位每一个TD
			SMIE_ReLocate_TD(hWnd,td,x,y);  // Renew Locate the td Position
/*			for (i=0;i<td->colSpan;i++)
			{
				x+=lpTable->widthPreCol[iCols];
				x+=lpTable->cellSpacing;
				iCols++;
			}
			*/
			y+=td->height;  // 得到下一个TD的坐标
			td=td->next;  // 得到下一个TD
		}
		return TRUE;
}

// **************************************************
// 声明：BOOL SMIE_ReLocate_TD(hWnd,LPTD_TABLE td,DWORD x_Start,DWORD y_Start)
// 参数：
//  IN hWnd -- 窗口句柄
// 	IN td -- TD结构
//	 IN x_Start -- TD的开始X坐标
//	 IN y_Start -- TD的开始Y坐标
// 
// 返回值：成功返回TRUE,否则返回FALSE
// 功能描述：重新定位TD的位置。
// 引用: 
// **************************************************
BOOL SMIE_ReLocate_TD(HWND hWnd,LPTD_TABLE td,DWORD x_Start,DWORD y_Start)
{
	LPHTMLOBJECT lpObject;
	DWORD x,y;
//	DWORD width;
	DWORD iTableHeight=0;
	DWORD iLastRowHeight=0;
	RECT rect;


	if (td->IsVirtual==TRUE)  // 该TD是一个虚TD，不需要进行处理
		return TRUE;
	lpObject=td->lpItem;  // 指向第一个控制


	x=x_Start;
	y=y_Start;

	td->x=x;  // 设置TD的开始位置
	td->y=y;

	// 设置TD所包含的控制的范围
	rect.left=td->x;
	rect.top=td->y;
	rect.right=td->x+td->width;
	rect.bottom=td->y+td->height;
	SMIE_ReLocate_Object(hWnd,lpObject,x_Start,y_Start,rect);  // 重新定位控制
	return TRUE; // 成功返回
}

// **************************************************
// 声明：BOOL SMIE_ReLocate_Object(HWND hWnd,LPHTMLOBJECT lpObject,DWORD x_Start,DWORD y_Start,RECT rect)
// 参数：
//	IN hWnd -- 窗口句柄
// 	IN lpObject -- 目标控制结构
// 	IN x_Start -- 目标控制的开始X坐标
// 	IN y_Start -- 目标控制的开始Y坐标
// 	IN rect -- 控制的范围
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：重新定位目标控制。
// 引用: 
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

	x=x_Start;  // 得到开始坐标
	y=y_Start;
	
	iHeightLine=SMIE_GetFontHeight(hWnd,NULL);  // 得到字体高度
	width=rect.right-rect.left;  // 得到控制的最大宽度
//	if (width==0)
//		return TRUE;
	lpPreControl=NULL;
	while(lpObject)
	{  // 定位每一个控制
		switch(lpObject->type)
		{
		case TYPE_TABLE: // 目标控制是一个TABLE

			if (iLastRowHeight)
			{
				x=x_Start;
				y+=iLastRowHeight;  // 定位到上次控制的下面
			}

			SMIE_ReLocate_Table(hWnd,(LPHTMLTABLE)lpObject->lpContent,&x,&y,&width,&iCurTabHeight);  // Recalc the Control Size 
			iLastRowHeight=0;  
			lpPreControl=NULL;
			break;
		case TYPE_CONTROL:  // 目标控制是一个一般控制
			if (width==0)
				break;
			isTextControl = IsTextControl((LPHTMLCONTROL)(lpObject->lpContent));

			if (isTextControl == FALSE)
			{
				x=x_Start;
				y+=iLastRowHeight;  // 定位到上次控制的下面
				iLastRowHeight=0;
			}
//			SMIE_ReLocate_Control(hWnd,lpObject->lpContent,&x,&y,rect,lpPreControl);  // Recalc the Control Size
			SMIE_ReLocate_Control(hWnd,(HHTMCTL)lpObject->lpContent,&x,&y,rect,iLastRowHeight);  // Recalc the Control Size
			if(iLastRowHeight<((LPHTMLCONTROL)(lpObject->lpContent))->heightLine)
				iLastRowHeight=((LPHTMLCONTROL)(lpObject->lpContent))->heightLine;
//			iLastRowHeight=0;
			lpPreControl=lpObject->lpContent;  // 保留当前控制，可能会影响到其他控制的定位
			if (isTextControl == FALSE)
			{
				y+=iLastRowHeight;   //the TD height must add the table height
				x=x_Start;
				iLastRowHeight=0;
			}
			break;
		case TYPE_CR:  // 目标控制是一个回车
//			if (iLastRowHeight)
			y+=iLastRowHeight;  // 定位到上次控制的下方
//			else
//				y+=14;
			x=x_Start;
			iLastRowHeight=iHeightLine;
			lpPreControl=NULL;
			break;
		default:
			break;
		}
		lpObject=lpObject->next;  // 得到下一个控制
	}
	return TRUE;
}


// **************************************************
// 声明：void SMIE_ShowHtmlTable(HWND hWnd,HDC hdc,LPHTMLTABLE lpTable,POINT ptOrg)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpTable -- TABLE结构
// 	IN ptOrg  -- 显示原点
// 
// 返回值：无
// 功能描述：显示TABLE。
// 引用: 
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
			
			GetClientRect(hWnd,&rect);  // 得到窗口矩形
//			if (((lpTable->y-ptOrg.y)>rect.bottom)||((lpTable->y-ptOrg.y+lpTable->height)<rect.top))
			if ((lpTable->y>(DWORD)(ptOrg.y+rect.bottom))||((lpTable->y+lpTable->height)<(DWORD)(rect.top+ptOrg.y)))
				return;  // 当前TABLE不在窗口内，不需要显示，返回。
	}
	if (lpTable->bgColor!=-1)
	{
		// 有设置背景颜色，画背景颜色
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
		// 得到TABLE的显示矩形
		rect.left=lpTable->x-ptOrg.x;
		rect.top=lpTable->y-ptOrg.y;
		rect.right=lpTable->x-ptOrg.x+lpTable->width;
		rect.bottom=lpTable->y-ptOrg.y+lpTable->height;
		SMIE_ShowBackGround(hdc,rect,lpTable->bgColor);   // 画背景
		bgOldColor=SetBkColor(hdc,lpTable->bgColor);  // 设置设备的背景
	}
//	
	tr=lpTable->head;  // ReLocate the head tr of the table
	while(tr)
	{
		SMIE_ShowTR(hWnd,hdc,tr,ptOrg);  // 显示TR
		tr=tr->next;
	}

	tr=lpTable->body; // ReLocate the body tr of the table
	while(tr)
	{
		SMIE_ShowTR(hWnd,hdc,tr,ptOrg); // 显示TR
		tr=tr->next;
	}

	tr=lpTable->foot; // ReLocate the foot tr of the table
	while(tr)
	{
		SMIE_ShowTR(hWnd,hdc,tr,ptOrg); // 显示TR
		tr=tr->next;
	}

	lpObject=lpTable->lpItem; // ReLocate the Control of the table
	while(lpObject)
	{
		if (lpObject->type==TYPE_CONTROL)
		{
			RECT rect;

			// 得到控制的矩形
			rect.left=lpTable->x-ptOrg.x;
			rect.right=rect.left+lpTable->width;
			rect.top=lpTable->y-ptOrg.y;
			rect.bottom=rect.top+lpTable->height;

			SMIE_ShowHtmlControl(hWnd,hdc,lpObject->lpContent,rect,ptOrg); // 显示控制
		}
		lpObject=lpObject->next;  // 得到下一个控制
	}
	if (lpTable->bgColor!=-1)
	{  // 恢复设备的背景
		SetBkColor(hdc,bgOldColor);
	}

}

// **************************************************
// 声明：void SMIE_ShowTR(HWND hWnd,HDC hdc,LPTR_TABLE tr,POINT ptOrg)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN tr -- TR结构
// 	IN ptOrg  -- 显示原点
// 
// 返回值：无
// 功能描述：显示TR。
// 引用: 
// **************************************************
void SMIE_ShowTR(HWND hWnd,HDC hdc,LPTR_TABLE tr,POINT ptOrg)
{
	LPTD_TABLE td;
	COLORREF bgOldColor;

		{
			RECT rect;
				
				GetClientRect(hWnd,&rect);   // 得到窗口矩形
//				if (((tr->y-ptOrg.y)>rect.bottom)||((tr->y-ptOrg.y+tr->height)<rect.top))
				if ((tr->y>(DWORD)(ptOrg.y+rect.bottom))||((tr->y+tr->height)<(DWORD)(rect.top+ptOrg.y)))
					return;  //当前TR不在显示范围之内，不需要显示返回
		}

		if (tr->bgColor!=-1)
		{  // 需要设置背景
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
		// 得到TR的范围
		rect.left=tr->x-ptOrg.x;
		rect.top=tr->y-ptOrg.y;
		rect.right=tr->x-ptOrg.x+tr->width;
		rect.bottom=tr->y-ptOrg.y+tr->height;
		SMIE_ShowBackGround(hdc,rect,tr->bgColor);  // 显示背景
		bgOldColor=SetBkColor(hdc,tr->bgColor);  // 设置设备句柄的背景

		}
		td=tr->td;  //得到第一个TD
		while(td)
		{  
			SMIE_ShowTD(hWnd,hdc,td,ptOrg);  // Renew Locate the td Position
			td=td->next;  // 得到下一个TD
		}
		if (tr->bgColor!=-1)
		{  // 恢复设备背景
			SetBkColor(hdc,bgOldColor);
		}
}

// **************************************************
// 声明：void SMIE_ShowTD(HWND hWnd,HDC hdc,LPTD_TABLE td,POINT ptOrg)
// 参数：
// 	IN hWnd  -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN td -- TD结构
// 	IN ptOrg -- 显示原点
// 
// 返回值：无
// 功能描述：显示TD。
// 引用: 
// **************************************************
void SMIE_ShowTD(HWND hWnd,HDC hdc,LPTD_TABLE td,POINT ptOrg)
{
	LPHTMLOBJECT lpObject;

	COLORREF bgOldColor;
	RECT rect;

	{
		RECT rect;
			
			GetClientRect(hWnd,&rect);  // 得到窗口客户区域
//			if (((td->y-ptOrg.y)>rect.bottom)||((td->y-ptOrg.y+td->height)<rect.top))
			if ((td->y>(DWORD)(ptOrg.y+rect.bottom))||((td->y+td->height)<(DWORD)(rect.top+ptOrg.y)))
				return; // 当前TD不在显示范围内，不需要显示返回
	}

	if (td->IsVirtual==TRUE)  // 当前TD是虚的，不需要显示
		return ;

//	GetClientRect(hWnd,&rect);
	// 得到TD的范围
	rect.left=td->x-ptOrg.x;
	rect.top=td->y-ptOrg.y;
	rect.right=td->x-ptOrg.x+td->width;
	rect.bottom=td->y-ptOrg.y+td->height;

	if (td->bgColor!=-1)
	{  // 显示背景
		SMIE_ShowBackGround(hdc,rect,td->bgColor);
		bgOldColor=SetBkColor(hdc,td->bgColor); // 设置设备背景
	}
	lpObject=td->lpItem;  // 得到第一个控制

	while(lpObject)
	{  // 显示各个控制
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // 当前控制是一个TABLE
			SMIE_ShowHtmlTable(hWnd,hdc,(LPHTMLTABLE)lpObject->lpContent,ptOrg);  // 显示TABLE
			break;
		case TYPE_CONTROL:  // 是一个一般控制
			SMIE_ShowHtmlControl(hWnd,hdc,lpObject->lpContent,rect,ptOrg);  // Show Html Control 
			break;
		default:
			break;
		}
		lpObject=lpObject->next;  // 得到下一个控制
	}
	if (td->bgColor!=-1)
	{  // 恢复设备背景
		SetBkColor(hdc,bgOldColor);
	}
}


// **************************************************
// 声明：void SMIE_ReleaseTable(LPHTMLTABLE lpTable)
// 参数：
// 	IN lpTable -- TABLE结构
// 
// 返回值：无
// 功能描述：释放TABLE结构。
// 引用: 
// **************************************************
void SMIE_ReleaseTable(LPHTMLTABLE lpTable)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	tr=lpTable->head;  // Release the head tr of the table
	while(tr)
	{
		nextTR=tr->next;  // 得到下一个TR
		SMIE_ReleaseTR(tr);  // 释放TR
		tr=nextTR;
	}

	tr=lpTable->body; // Release the body tr of the table
	while(tr)
	{
		nextTR=tr->next;  // 得到下一个TR
		SMIE_ReleaseTR(tr); // 释放TR
		tr=nextTR;
	}

	tr=lpTable->foot; // Release the foot tr of the table
	while(tr)
	{
		nextTR=tr->next;   // 得到下一个TR
		SMIE_ReleaseTR(tr); // 释放TR
		tr=nextTR;
	}

	lpObject=lpTable->lpItem; // Release the Control of the table
	SMIE_ReleaseObject(lpObject);  // 释放控制

	if (lpTable->heightPreRow!=0)
		free(lpTable->heightPreRow);  // 释放高度数组
	if (lpTable->widthPreCol!=0)
		free(lpTable->widthPreCol);   // 释放宽度数组
	free(lpTable);  // 释放TABLE结构
	return ;
}

// **************************************************
// 声明：void SMIE_ReleaseTR(LPTR_TABLE tr)
// 参数：
// 	IN tr  -- TR结构
// 
// 返回值：无
// 功能描述：释放TR结构
// 引用: 
// **************************************************
void SMIE_ReleaseTR(LPTR_TABLE tr)
{
	LPTD_TABLE td,lpNext;
//	COLORREF bgOldColor;

		td=tr->td;  // 得到第一个TD
		while(td)
		{
			lpNext=td->next;  // 得到下一个TD
			SMIE_ReleaseTD(td);  // 释放TD
			td=lpNext;
		}
		free(tr);  // 释放TR结构
}

// **************************************************
// 声明：void SMIE_ReleaseTD(LPTD_TABLE td)
// 参数：
// 	IN td  -- TD结构
// 
// 返回值：无
// 功能描述：释放TD结构
// 引用: 
// **************************************************
void SMIE_ReleaseTD(LPTD_TABLE td)
{
	LPHTMLOBJECT lpObject;
//	LPTD_TABLE lpNext;

//	while(td)
//	{
//		lpNext=td->next;
		lpObject=td->lpItem;  // 得到第一个控制
		if (lpObject)
			SMIE_ReleaseObject(lpObject);  // 释放控制
		free(td);  // 释放TD结构
//		td=lpNext;
//	}
}

// **************************************************
// 声明：void SMIE_ReleaseObject(LPHTMLOBJECT lpObject)
// 参数：
// 	IN lpObject -- 目标控制结构
// 
// 返回值：无
// 功能描述：释放目标控制结构。
// 引用: 
// **************************************************
void SMIE_ReleaseObject(LPHTMLOBJECT lpObject)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;  // 得到下一个目标
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // 当前目标是TABLE
			SMIE_ReleaseTable((LPHTMLTABLE)lpObject->lpContent);  // Release Table
			break;
		case TYPE_CONTROL:  // 当前目标是一个控制
			SMIE_ReleaseControl(lpObject->lpContent);  // Release Control 
			break;
		default:
			break;
		}
		free(lpObject);  // 释放目标
		lpObject=lpNext;  // 指向下一个目标
	}
}


// **************************************************
// 声明：void SMIE_ShowBackGround(HDC hdc,RECT rect,COLORREF color)
// 参数：
// 	IN  hdc -- 设备句柄
// 	IN  rect -- 要填充的矩形
// 	IN  color -- 指定的背景颜色
// 
// 返回值：无
// 功能描述：显示背景。
// 引用: 
// **************************************************
void SMIE_ShowBackGround(HDC hdc,RECT rect,COLORREF color)
{
	HBRUSH hBrush=(HBRUSH)CreateSolidBrush(color); // 用指定的颜色创建刷子
//		hBrush=(HBRUSH)SelectObject(hdc,hBrush);
//		if (color)
		FillRect(hdc,&rect,hBrush);  // 用刷子填充矩形
 		DeleteObject(hBrush);  // 删除刷子
}



// **************************************************
// 声明：BOOL SMIE_GetUrl(HHTML hHtml,DWORD x,DWORD y,LPTSTR *lppUrl)
// 参数：
// 	IN hHtml  -- HTML句柄
// 	IN x  -- 指定的X坐标
// 	IN y  -- 指定的Y坐标
// 	OUT lppUrl -- 返回指定的网页地址
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到当前打开网页中指定位置的控制所指向的网址。
// 引用: 
// **************************************************
BOOL SMIE_GetUrl(HHTML hHtml,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;
	
	lpHead=(LPHTMLHEAD)hHtml;  // 得到HEAD

	if (lpHead==NULL)
		return FALSE;
	lpCurObject=lpHead->next;  // 得到当前目标

	return SMIE_GetUrlInObject(lpCurObject,x,y,lppUrl);  // 在当前目标中得到指定位置的网址
}


// **************************************************
// 声明：BOOL SMIE_GetUrlInObject(LPHTMLOBJECT lpObject,DWORD x,DWORD y,LPTSTR *lppUrl)
// 参数：
// 	IN lpObject -- 指定的目标
// 	IN x  -- 指定的X坐标
// 	IN y  -- 指定的Y坐标
// 	OUT lppUrl -- 返回指定的网页地址
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到指定的目标中指定位置的控制所指向的网址。
// 引用: 
// **************************************************
BOOL SMIE_GetUrlInObject(LPHTMLOBJECT lpObject,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;  // 得到下一个目标
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // 当前目标是TABLE
			if(SMIE_GetUrlInTable((LPHTMLTABLE)lpObject->lpContent,x,y,lppUrl)==TRUE)  // 在TABLE中得到
			{  // 指定位置在当前TABLE，返回成功
				return TRUE;
			}
			break;
		case TYPE_CONTROL:  // 当前目标是一个控制
			if (SMIE_GetUrlInControl((HHTMCTL)lpObject->lpContent,x,y,lppUrl)==TRUE) // 在CONTROL中查找
			{  // 指定位置在当前CONTROL，返回成功
				return TRUE;
			}
			break;
		default:
			break;
		}
		lpObject=lpNext;  // 指向下一个目标
	}
	return FALSE;  // 返回失败
}


// **************************************************
// 声明：BOOL SMIE_GetUrlInTable(LPHTMLTABLE lpTable,DWORD x,DWORD y,LPTSTR *lppUrl)
// 参数：
// 	IN lpTable -- TABLE结构
// 	IN x  -- 指定的X坐标
// 	IN y  -- 指定的Y坐标
// 	OUT lppUrl -- 返回指定的网页地址
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到指定的TABLE中指定位置的控制所指向的网址。
// 引用: 
// **************************************************
BOOL SMIE_GetUrlInTable(LPHTMLTABLE lpTable,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	if (lpTable->x>x || x>lpTable->x+lpTable->width)
		return FALSE;  // 指定位置不在TABLE中

	if (lpTable->y>y || y>lpTable->y+lpTable->height)
		return FALSE;  // 指定位置不在TABLE中

	tr=lpTable->head;  // Search the head tr of the table
	while(tr)
	{
		nextTR=tr->next;  // 得到下一个TR
		if (SMIE_GetUrlInTR(tr,x,y,lppUrl)==TRUE)  // 在TR中查找
		{  // 当前位置在当前TR中，返回成功
			return TRUE;
		}
		tr=nextTR;  // 指向下一个TR
	}

	tr=lpTable->body; // Search the body tr of the table
	while(tr)
	{
		nextTR=tr->next; // 得到下一个TR
		if (SMIE_GetUrlInTR(tr,x,y,lppUrl)==TRUE)  // 在TR中查找
		{  // 当前位置在当前TR中，返回成功
			return TRUE;
		}
		tr=nextTR;  // 指向下一个TR
	}

	tr=lpTable->foot; // Search the foot tr of the table
	while(tr)
	{
		nextTR=tr->next;  // 得到下一个TR
		if (SMIE_GetUrlInTR(tr,x,y,lppUrl)==TRUE)  // 在TR中查找
		{  // 当前位置在当前TR中，返回成功
			return TRUE;
		}
		tr=nextTR;  // 指向下一个TR
	}

	lpObject=lpTable->lpItem; // Search the Control of the table
	if (SMIE_GetUrlInObject(lpObject,x,y,lppUrl)==TRUE)  // 在CONTROL中查找
	{   // 当前位置在当前CONTROL中，返回成功
		return TRUE;
	}

	return FALSE;  // 返回失败  
}


// **************************************************
// 声明：BOOL SMIE_GetUrlInTR(LPTR_TABLE tr,DWORD x,DWORD y,LPTSTR *lppUrl)
// 参数：
// 	IN tr -- TR结构
// 	IN x  -- 指定的X坐标
// 	IN y  -- 指定的Y坐标
// 	OUT lppUrl -- 返回指定的网页地址
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到指定的TR中指定位置的控制所指向的网址。
// 引用: 
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
		td=tr->td; //  得到第一个TD
		while(td)
		{
			lpNext=td->next;   // 得到下一个TD
			if (SMIE_GetUrlInTD(td,x,y,lppUrl)==TRUE)  // 在TD中查找
			{  // 指定位置在当前TD中，返回成功
				return TRUE;
			}
			td=lpNext;  // 指向下一个TD
		}
		return FALSE;  // 返回失败
}


// **************************************************
// 声明：BOOL SMIE_GetUrlInTD(LPTD_TABLE td,DWORD x,DWORD y,LPTSTR *lppUrl)
// 参数：
// 	IN td -- TD结构
// 	IN x  -- 指定的X坐标
// 	IN y  -- 指定的Y坐标
// 	OUT lppUrl -- 返回指定的网页地址
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到指定的TD中指定位置的控制所指向的网址。
// 引用: 
// **************************************************
BOOL SMIE_GetUrlInTD(LPTD_TABLE td,DWORD x,DWORD y,LPTSTR *lppUrl)
{
	LPHTMLOBJECT lpObject;

	if (td->x>x || x>td->x+td->width)
		return FALSE;  // 指定位置不在当前TD中

	if (td->y>y || y>td->y+td->height)
		return FALSE;  // 指定位置不在当前TD中
	lpObject=td->lpItem;  // 得到目标
	if (lpObject)
	{
		if (SMIE_GetUrlInObject(lpObject,x,y,lppUrl)==TRUE)  // 在指定目标中查找
		{  // 指定位置在当前目标中，返回成功
			return TRUE;
		}
	}
	return FALSE;  // 返回失败
}

// **************************************************
// 声明：BOOL SMIE_GetHtmlSize(HHTML hHtml,LPSIZE lpSize)
// 参数：
// 	IN hHtml -- HTML句柄
// 	OUT lpSize -- 返回当前HTML的尺寸
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：得到当前HTML的尺寸
// 引用: 
// **************************************************
BOOL SMIE_GetHtmlSize(HHTML hHtml,LPSIZE lpSize)
{
	LPHTMLHEAD lpHead;

		if(lpSize==NULL)  // Test the Parameter Correct
			return FALSE;
		lpHead=(LPHTMLHEAD)hHtml;

		lpSize->cx=lpHead->width;  // 得到当前HTML的尺寸
		lpSize->cy=lpHead->height;

		return TRUE;
}
// **************************************************
// 声明：BOOL SMIE_GetMarkPos(HHTML hHtml,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// 参数：
// 	IN  hHtml  -- HTML句柄
// 	OUT lpxPos -- 存放标记X坐标的指针
// 	OUT lpyPos -- 存放标记Y坐标的指针
// 	IN  lpMark -- 指定的标记
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：在指定的HTML中查找指定标记的位置。
// 引用: 
// **************************************************
BOOL SMIE_GetMarkPos(HHTML hHtml,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLHEAD lpHead;
	LPHTMLOBJECT lpCurObject;
	
	lpHead=(LPHTMLHEAD)hHtml;  // 得到HTML的头

	if (lpHead==NULL)
		return FALSE;
	lpCurObject=lpHead->next;  // 得到当前目标

	return SMIE_GetMarkPosInObject(lpCurObject,lpxPos,lpyPos,lpMark);  // 在目标中查找标记
}
// **************************************************
// 声明：BOOL SMIE_GetMarkPosInTable(LPHTMLTABLE lpTable,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// 参数：
// 	IN  lpTable -- TABLE结构
// 	OUT lpxPos -- 存放标记X坐标的指针
// 	OUT lpyPos -- 存放标记Y坐标的指针
// 	IN  lpMark -- 指定的标记
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：在指定的TABLE中查找指定标记的位置。
// 引用: 
// **************************************************
BOOL SMIE_GetMarkPosInTable(LPHTMLTABLE lpTable,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPTR_TABLE tr,nextTR;
	LPHTMLOBJECT lpObject;

	tr=lpTable->head;  // Search the head tr of the table
	while(tr)
	{
		nextTR=tr->next;  // 得到下一个TR
		if (SMIE_GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)  // 在TR中查找
		{  // 已经找到，返回成功
			return TRUE;
		}
		tr=nextTR;  // 指向下一个TR
	}

	tr=lpTable->body; // Search the body tr of the table
	while(tr)
	{
		nextTR=tr->next;  // 得到下一个TR
		if (SMIE_GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)  // 在TR中查找
		{  // 已经找到，返回成功
			return TRUE;
		}
		tr=nextTR;  // 指向下一个TR
	}

	tr=lpTable->foot; // Search the foot tr of the table
	while(tr)
	{
		nextTR=tr->next; // 得到下一个TR
		if (SMIE_GetMarkPosInTR(tr,lpxPos,lpyPos,lpMark)==TRUE)  // 在TR中查找
		{  // 已经找到，返回成功
			return TRUE;
		}
		tr=nextTR;   // 指向下一个TR
	}

	lpObject=lpTable->lpItem; // Search the Control of the table
	if (SMIE_GetMarkPosInObject(lpObject,lpxPos,lpyPos,lpMark)==TRUE)  // 在目标中查找
	{  // 已经找到，返回成功
		return TRUE;
	}

	return FALSE;  // 返回失败
}
// **************************************************
// 声明：BOOL SMIE_GetMarkPosInTR(LPTR_TABLE tr,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// 参数：
// 	IN  tr -- TR结构
// 	OUT lpxPos -- 存放标记X坐标的指针
// 	OUT lpyPos -- 存放标记Y坐标的指针
// 	IN  lpMark -- 指定的标记
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：在指定的TR中查找指定标记的位置。
// 引用: 
// **************************************************
BOOL SMIE_GetMarkPosInTR(LPTR_TABLE tr,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPTD_TABLE td,lpNext;

		td=tr->td;  // 得到第一个TD
		while(td)
		{
			lpNext=td->next;  // 得到下一个TD
			if (SMIE_GetMarkPosInTD(td,lpxPos,lpyPos,lpMark)==TRUE)  // 在TD中查找
			{  // 已经找到，成功返回
				return TRUE;
			}
			td=lpNext;  // 指向下一个TD
		}
		return FALSE;  // 返回失败
}
// **************************************************
// 声明：BOOL SMIE_GetMarkPosInTD(LPTD_TABLE td,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// 参数：
// 	IN  td -- TD结构
// 	OUT lpxPos -- 存放标记X坐标的指针
// 	OUT lpyPos -- 存放标记Y坐标的指针
// 	IN  lpMark -- 指定的标记
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：在指定的TD中查找指定标记的位置。
// 引用: 
// **************************************************
BOOL SMIE_GetMarkPosInTD(LPTD_TABLE td,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLOBJECT lpObject;

	lpObject=td->lpItem;  // 得到目标
	if (lpObject)
	{
		if (SMIE_GetMarkPosInObject(lpObject,lpxPos,lpyPos,lpMark)==TRUE)  // 在目标中查找
		{ // 已经查到，返回成功
			return TRUE;
		}
	}
	return FALSE;  // 返回失败
}
// **************************************************
// 声明：BOOL SMIE_GetMarkPosInObject(LPHTMLOBJECT lpObject,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
// 参数：
// 	IN  lpObject -- OBJECT结构
// 	OUT lpxPos -- 存放标记X坐标的指针
// 	OUT lpyPos -- 存放标记Y坐标的指针
// 	IN  lpMark -- 指定的标记
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：在指定的目标中查找指定标记的位置。
// 引用: 
// **************************************************
BOOL SMIE_GetMarkPosInObject(LPHTMLOBJECT lpObject,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark)
{
	LPHTMLOBJECT lpNext;
	while(lpObject)
	{
		lpNext=lpObject->next;  // 得到下一个目标
		switch(lpObject->type)
		{
		case TYPE_TABLE:  // 当前目标是TABLE
			if(SMIE_GetMarkPosInTable((LPHTMLTABLE)lpObject->lpContent,lpxPos,lpyPos,lpMark)==TRUE)  // 在TABLE中查找
			{  // 已经找到，返回成功
				return TRUE;
			}
			break;
		case TYPE_CONTROL:  // 当前目标是CONTROL
			if (SMIE_GetMarkPosInControl((LPHTMLTABLE)lpObject->lpContent,lpxPos,lpyPos,lpMark)==TRUE)  // 在CONTROL中查找
			{  // 已经找到，返回成功
				return TRUE;
			}
			break;
		default:
			break;
		}
		lpObject=lpNext;  // 指向下一个目标
	}
	return FALSE;  // 返回失败
}

// !!! Add By Jami chen in 2003.09.09
// **************************************************
// 声明：HHTML SMIE_LoadImage(HWND hWnd,LPTSTR lpImageFile)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpImageFile -- 要装载的图形文件
// 
// 返回值：返回一个HTML句柄
// 功能描述：装载一个图形文件到当前HTML中。
// 引用: 
// **************************************************
HHTML SMIE_LoadImage(HWND hWnd,LPTSTR lpImageFile)
{
	LPHTMLHEAD  lpHead;

		lpHead=(LPHTMLHEAD)malloc(sizeof(HTMLHEAD));  // 分配一个HTML头结构
		if (lpHead==NULL)
		{
			MessageBox(NULL,TEXT("Not Enough Memory"),TEXT("Error"),MB_OK);
			return NULL;
		}
		lpHead->next=SMIE_ControlLoadImage(hWnd,lpImageFile);  // 装载图形文件到CONTROL中
		if (lpHead->next)
		{
			SIZE size;
			GetControlSize(hWnd,lpHead->next->lpContent,&size);
			lpHead->width = size.cx;  // 得到当前图象的尺寸
			lpHead->height = size.cy;
		}
		else
		{
			// 没有成功加载图象
			lpHead->width = 0;  
			lpHead->height = 0;
		}
		return lpHead;  // 返回句柄
}
// !!! Add By Jami chen in 2003.09.09
