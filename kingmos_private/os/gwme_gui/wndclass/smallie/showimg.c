/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：小屏幕模式的IE浏览类, 图形显示
版本号：1.0.0.456
开发时期：2004-05-26
作者：陈建明 JAMI
修改记录：
**************************************************/
#ifdef EML_DOS
#include <dos.h>
#include <CONIO.H>
#include <STDIO.H>
#endif

//#include "eframe.h"
//#include "ewindows.h"
#include "eHtmView.h"
#include "Control.h"
//#include "HtmlLocate.h"
#include "SIETable.h"
#include "efilepth.h"
#include "ViewDef.h"
#include "jpegapi.h"


#define TYPE_NOTIMGFILE	0
#define TYPE_BMPFILE	1
#define TYPE_JPEGFILE	2
#define TYPE_GIFFILE	3

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

static FILETYPE tabImageType[] = {
	{ "*.gif" , TYPE_GIFFILE },
	{ "*.bmp" , TYPE_BMPFILE },
	{ "*.jpg" , TYPE_JPEGFILE },
	{ "*.jpeg", TYPE_JPEGFILE },
};



extern LPTSTR GetControlImg(HHTMCTL lpControl);
extern BOOL GetControlPos(HHTMCTL hControl,LPPOINT ppoint);
extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );;
extern BOOL SetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize);
extern BOOL GetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize);


static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg);
static int GetNextPicTime(HGIF hGif,int iIndex);
static UINT GetImageType(LPTSTR lpFileName);
static int GetTotalPic(HGIF hGif);
static void LoadImageFile(HWND hWnd,LPSHOWIMAGEITEM lpCurItem);
static void DrawBitMap(HDC hdc,HBITMAP hBitmap,LPRECT lprect);
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize);
static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize);


//#ifdef USE_TIMERTOSHOWIMG

// **************************************************
// 声明：DWORD WINAPI ShowImageThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：图象显示线程。
// 引用: 
// **************************************************
DWORD WINAPI ShowImage(HWND hWnd)
{
	LPHTMLVIEW lpHtmlView;
	LPSHOWIMAGEITEM lpCurItem;
	POINT ptOrg,ptControl;
//	HDC hdc;
	RECT rect;
	SIZE sizeImage;
	BOOL bRefreshScreen = FALSE;
//	static int tick1 = 0,tick2 = 0;

//		tick2 = GetTickCount();
//		RETAILMSG(1,(TEXT("Delay Time = %d"),tick2 - tick1));
//		tick1 = tick2;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

//		hdc = GetDC(hWnd);

		lpCurItem = lpHtmlView->lpShowImageList;  // 得到第一个图象
		
		if (lpHtmlView->iDisplayMode != DISPLAY_ONLYTEXT || lpHtmlView->iFileStyle == IMAGE_FILE)
		{
			while(lpCurItem)
			{
				if (lpCurItem->bDownloadOK == TRUE)
				{  // 当前文件已经下载
					// 当前图象有效
					if (lpCurItem->bShow)
					{
						// 要显示该图象
						if (lpCurItem->hGif == NULL && lpCurItem->hBmp == NULL)
						{
							// 还没有打开当前文件，先打开当前文件
							LoadImageFile(hWnd,lpCurItem);
							lpCurItem->bShow = FALSE; // 显示完成
						}
/*						if (lpCurItem->hGif)
						{
							// 是gif 文件
							ptOrg.x = lpHtmlControl->x - lpHtmlView->x_Org;
							ptOrg.y = lpHtmlControl->y - lpHtmlView->y_Org;
							DrawGifImage(hWnd,hdc,lpCurItem->hGif,lpCurItem->iIndex,ptOrg);
						}
*/
						if (lpCurItem->hBmp)
						{
							// 是 bmp 文件 或者 jpeg 文件
							GetControlPos(lpCurItem->hControl,&ptControl);
							GetControlSize(hWnd,lpCurItem->hControl,&sizeImage);
							rect.left = ptControl.x - lpHtmlView->x_Org;
							rect.top = ptControl.y - lpHtmlView->y_Org;
							rect.right = rect.left + sizeImage.cx;
							rect.bottom = rect.top + sizeImage.cy;
							DrawBitMap(lpHtmlView->hMemoryDC,lpCurItem->hBmp,&rect); // 显示位图文件
							lpCurItem->bShow = FALSE; // 显示完成
							bRefreshScreen = TRUE; // 需要刷新屏幕
						}
					}

					// 查看是否是GIF文件，且是否需要显示下一帧图象
					if (lpCurItem->hGif)
					{
						// 是gif 文件
						if (lpCurItem->iImageNum == 1)
						{
							// 只有一幅图
							if (lpCurItem->bShow) 
							{
								// 只有在需要显示时才显示
								GetControlPos(lpCurItem->hControl,&ptControl);
								ptOrg.x = ptControl.x - lpHtmlView->x_Org;
								ptOrg.y = ptControl.y - lpHtmlView->y_Org;
								DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,0,&ptOrg);
								bRefreshScreen = TRUE; // 需要刷新屏幕
							}
						}
						else
						{
							if (lpCurItem->bShow)
							{
								// 要求立即显示
								lpCurItem->iDelayTime = 0;
							}
							if (lpCurItem->iDelayTime <= 0)
							{
								//需要显示
								GetControlPos(lpCurItem->hControl,&ptControl);
//								tick1 = GetTickCount();
								ptOrg.x = ptControl.x - lpHtmlView->x_Org;
								ptOrg.y = ptControl.y - lpHtmlView->y_Org;
								DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,lpCurItem->iIndex,&ptOrg);
								lpCurItem->iDelayTime = GetNextPicTime(lpCurItem->hGif,lpCurItem->iIndex); // 得到到下一幅图象的时间
								lpCurItem->iIndex ++; // 下一幅图象
								if (lpCurItem->iIndex >= lpCurItem->iImageNum)
									lpCurItem->iIndex = 0; // 已经到最后一幅图象, 下次开始显示第一幅图象
								bRefreshScreen = TRUE; // 需要刷新屏幕
							}
							else
							{
								lpCurItem->iDelayTime -= TIMEINTERVAL; // 减去已经等待的时间
							}
						}
						lpCurItem->bShow = FALSE; // 显示完成
					}
				}

				lpCurItem = lpCurItem->next; // 得到下一个图象文件
			}
		}
		if (bRefreshScreen == TRUE)
			InvalidateRect(hWnd,NULL,TRUE);
//		ReleaseDC(hWnd,hdc);
		return 0;
}
//#else
// **************************************************
// 声明：DWORD WINAPI ShowImageThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：图象显示线程。
// 引用: 
// **************************************************
DWORD WINAPI ShowImageThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	LPSHOWIMAGEITEM lpCurItem;
	POINT ptOrg,ptControl;
//	HDC hdc;
	RECT rect;
	SIZE sizeImage;
	BOOL bRefreshScreen = FALSE;
//	int tick1 = 0,tick2 = 0;

	
		hWnd = (HWND)pParam;  // 得到参数
		if (hWnd == NULL)
			return FALSE;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

	//	hdc = GetDC(hWnd);
		while(1)
		{
			if (lpHtmlView->bExit)
				break;
			bRefreshScreen = FALSE; // 需要刷新屏幕
			lpCurItem = lpHtmlView->lpShowImageList;  // 得到第一个图象
			
//			if (lpHtmlView->iDisplayMode != DISPLAY_ONLYTEXT)
		    if (lpHtmlView->iDisplayMode != DISPLAY_ONLYTEXT || lpHtmlView->iFileStyle == IMAGE_FILE)
			{
				while(lpCurItem)
				{
					if (lpCurItem->bDownloadOK == TRUE)
					{  // 当前文件已经下载
						// 当前图象有效
						if (lpCurItem->bShow)
						{
							// 要显示该图象
							if (lpCurItem->hGif == NULL && lpCurItem->hBmp == NULL)
							{
								// 还没有打开当前文件，先打开当前文件
								LoadImageFile(hWnd,lpCurItem);
								lpCurItem->bShow = FALSE; // 显示完成
							}
	/*						if (lpCurItem->hGif)
							{
								// 是gif 文件
								ptOrg.x = lpHtmlControl->x - lpHtmlView->x_Org;
								ptOrg.y = lpHtmlControl->y - lpHtmlView->y_Org;
								DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,lpCurItem->iIndex,ptOrg);
							}
	*/
							if (lpCurItem->hBmp)
							{
								// 是 bmp 文件 或者 jpeg 文件
								GetControlPos(lpCurItem->hControl,&ptControl);
								GetControlSize(hWnd,lpCurItem->hControl,&sizeImage);
								rect.left = ptControl.x - lpHtmlView->x_Org;
								rect.top = ptControl.y - lpHtmlView->y_Org;
								rect.right = rect.left + sizeImage.cx;
								rect.bottom = rect.top + sizeImage.cy;
								DrawBitMap(lpHtmlView->hMemoryDC,lpCurItem->hBmp,&rect); // 显示位图文件
								lpCurItem->bShow = FALSE; // 显示完成
								bRefreshScreen = TRUE; // 需要刷新屏幕
							}
						}

						// 查看是否是GIF文件，且是否需要显示下一帧图象
						if (lpCurItem->hGif)
						{
							// 是gif 文件
							if (lpCurItem->iImageNum == 1)
							{
								// 只有一幅图
								if (lpCurItem->bShow) 
								{
									// 只有在需要显示时才显示
									GetControlPos(lpCurItem->hControl,&ptControl);
									ptOrg.x = ptControl.x - lpHtmlView->x_Org;
									ptOrg.y = ptControl.y - lpHtmlView->y_Org;
									DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,0,&ptOrg);
									bRefreshScreen = TRUE; // 需要刷新屏幕
								}
							}
							else
							{
								if (lpCurItem->bShow)
								{
									// 要求立即显示
									lpCurItem->iDelayTime = 0;
								}
								if (lpCurItem->iDelayTime <= 0)
								{
									//需要显示
									GetControlPos(lpCurItem->hControl,&ptControl);
									ptOrg.x = ptControl.x - lpHtmlView->x_Org;
									ptOrg.y = ptControl.y - lpHtmlView->y_Org;
									DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,lpCurItem->iIndex,&ptOrg);
									lpCurItem->iDelayTime = GetNextPicTime(lpCurItem->hGif,lpCurItem->iIndex); // 得到到下一幅图象的时间
									lpCurItem->iIndex ++; // 下一幅图象
									if (lpCurItem->iIndex >= lpCurItem->iImageNum)
										lpCurItem->iIndex = 0; // 已经到最后一幅图象, 下次开始显示第一幅图象

									bRefreshScreen = TRUE; // 需要刷新屏幕
								}
								else
								{
									lpCurItem->iDelayTime -= TIMEINTERVAL; // 减去已经等待的时间
								}
							}
							lpCurItem->bShow = FALSE; // 显示完成
						}
					}

					lpCurItem = lpCurItem->next; // 得到下一个图象文件
				}
			}
			if (bRefreshScreen == TRUE)
			{	HDC hdc;
//				InvalidateRect(hWnd,NULL,TRUE);
				hdc = GetDC(hWnd);
				GetClientRect(hWnd,&rect);
				BitBlt(hdc,0,0,rect.right,rect.bottom,lpHtmlView->hMemoryDC,0,0,SRCCOPY);
				ReleaseDC(hWnd,hdc);
			}
//			tick1 = GetTickCount();
			Sleep(TIMEINTERVAL);
//			tick2 = GetTickCount();
//			RETAILMSG(1,(TEXT("Delay Time = %d"),tick2 - tick1));
//			tick1 = tick2;
		}

//		ReleaseDC(hWnd,hdc);

		return 0;
}

//#endif

// **************************************************
// 声明：BOOL InsertAnimationList(HWND hWnd,HHTMCTL hControl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpHtmlControl -- 要插入图象的控件句柄
// 
// 返回值：成功返回TRUE，失败返回FALSE。
// 功能描述：插入一个图象控件到显示图象列表。
// 引用: 
// **************************************************
BOOL InsertAnimationList(HWND hWnd,HHTMCTL hControl)
{
	LPSHOWIMAGEITEM lpNewImageItem;
	LPSHOWIMAGEITEM lpCurItem;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return 0;

		lpNewImageItem = (LPSHOWIMAGEITEM)malloc(sizeof(SHOWIMAGEITEM)); // 分配一个条目空间
		if (lpNewImageItem == NULL)
		{ // 分配内存失败
			return FALSE;
		}

		// 初始化结构
		lpNewImageItem->hControl = hControl;  // 设置控件句柄
		lpNewImageItem->lpLocatePath = NULL;  // 目前还没有对应的本地文件名
		lpNewImageItem->hGif = NULL; // Gif 图象的句柄
		lpNewImageItem->hBmp  = NULL; // bitmap , jpeg 图象的句柄
		lpNewImageItem->iIndex = 0; // gif图象使用，当前正在播放的图象索引
		lpNewImageItem->iDelayTime = 0; // gif 图象使用，播放下一个索引图象还需要等待的时间
		lpNewImageItem->iImageNum = 0 ; //gif 图象使用，当前一共有多少幅图象
		lpNewImageItem->next = NULL;  // 指向下一幅GIF动画
		lpNewImageItem->bShow = FALSE; // 现在还不需要显示
		lpNewImageItem->bDownloadOK = FALSE;  // 还没有下载该文件

		// 插入当前图象到显示图象列表

		if (lpHtmlView->lpShowImageList == NULL)
		{  //当前列表中还没有数据
			lpHtmlView->lpShowImageList = lpNewImageItem; // 直接插入到列表根
		}
		else
		{
			// 已经有图象存在，插入到最后
			lpCurItem = lpHtmlView->lpShowImageList;

			// 如果下一个条目为空，则将新加条目添加到当前条目的下一个条目
			while(lpCurItem->next)
			{ //当前条目的下一个条目有数据
				lpCurItem = lpCurItem->next;  // 指向下一个条目
			}
			lpCurItem->next = lpNewImageItem; // 插入新增条目
		}

		return TRUE; // 返回成功
}


// **************************************************
// 声明：BOOL DownloadImageOK(HWND hWnd, HHTMCTL hControl,LPTSTR lpFileName)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hControl -- 已经完成下载的图象的控件句柄
//	IN lpFileName -- 下载到本地文件的文件名
// 
// 返回值：成功返回TRUE，失败返回FALSE。
// 功能描述：完成一个图象控件的下载。
// 引用: 
// **************************************************
BOOL DownloadImageOK(HWND hWnd, HHTMCTL hControl,LPTSTR lpFileName)
{
	LPSHOWIMAGEITEM lpCurItem;
	LPHTMLVIEW lpHtmlView;
	SIZE sizeImage;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return FALSE;

		// 已经有图象存在，插入到最后
		lpCurItem = lpHtmlView->lpShowImageList;

		// 如果下一个条目为空，则将新加条目添加到当前条目的下一个条目
		while(lpCurItem)
		{ //当前条目有效
			if (lpCurItem->hControl == hControl)
			{
				// 完成下载的控件就是当前控件
				lpCurItem->bDownloadOK = TRUE; // 当前控件已经下载完成
				if (lpFileName)
				{
					//有本地文件存在
					lpCurItem->lpLocatePath = BufferAssignTChar(lpFileName);  // 保留当前文件名
				}
				// 装载图象
				if (lpCurItem->hGif == NULL && lpCurItem->hBmp == NULL)
				{
					// 还没有打开当前文件，先打开当前文件
					LoadImageFile(hWnd,lpCurItem);
				}
				if (lpCurItem->hGif)
				{
					// 是GIF文件
					if (GetGIFImageSize(lpCurItem->hGif,&sizeImage) == TRUE)
					{
						// 得到尺寸成功
						SetControlSize(hWnd,lpCurItem->hControl,&sizeImage);
					}
				}
				if (lpCurItem->hBmp)
				{
					// 是BMP文件
					if (GetBMPImageSize(lpCurItem->hBmp,&sizeImage) == TRUE)
					{
						// 得到尺寸成功
						SetControlSize(hWnd,lpCurItem->hControl,&sizeImage);
					}
				}
				break;
			}
			lpCurItem = lpCurItem->next;
		}
		return TRUE;
}

// **************************************************
// 声明：BOOL ShowImageControl(HWND hWnd,HHTMCTL hControl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpHtmlControl -- 要插入图象的控件句柄
// 
// 返回值：成功返回TRUE，失败返回FALSE。
// 功能描述：插入一个图象控件到显示图象列表。
// 引用: 
// **************************************************
BOOL ShowImageControl(HWND hWnd,HHTMCTL hControl)
{
	LPSHOWIMAGEITEM lpCurItem;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return FALSE;

		// 已经有图象存在，插入到最后
		lpCurItem = lpHtmlView->lpShowImageList;

		// 如果下一个条目为空，则将新加条目添加到当前条目的下一个条目
		while(lpCurItem)
		{ //当前条目有效
			if (lpCurItem->hControl == hControl)
			{
				// 需要显示的控件就是当前控件
				lpCurItem->bShow = TRUE; // 当前控件要求显示
				break;
			}
			lpCurItem = lpCurItem->next;
		}
		return TRUE;
}


// **************************************************
// 声明：static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN hGif -- 要显示的图象句柄
//  IN iIndex -- 要显示的图象索引
//  IN ptOrg -- 图象开始显示位置
// 
// 返回值：无
// 功能描述：根据一个图形句柄显示图象。
// 引用: 
// **************************************************
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
{
//	GIF_IMAGE gifimage;

//		gifimage.dwSize = sizeof(GIF_IMAGE);
//		Gif_GetImage(hGif,iIndex,&gifimage); // 得到当前索引图象的信息
		// 绘制当前指定索引的图象
		Gif_DrawIndex(hGif,iIndex,hdc,ptOrg->x,ptOrg->y);
}

// **************************************************
// 声明：static int GetNextPicTime(HGIF hGif,int iIndex)
// 参数：
// 	IN hGif --	GIF文件句柄
//  IN iIndex -- 当前的图象的索引
// 
// 返回值：返回当前图象到下一幅图象的间隔时间
// 功能描述： 得到当前图象到下一幅图象的间隔时间.
// 引用: 
// **************************************************
static int GetNextPicTime(HGIF hGif,int iIndex)
{
	DWORD dwDelay;
	Gif_GetDelay(hGif,iIndex, &dwDelay ); // 得到下一幅图的时间延续

	return (dwDelay * 10 ); // Delay 是0.01s 为单位，转化成MS
}

// **************************************************
// 声明：static void LoadImageFile(HWND hWnd,LPSHOWIMAGEITEM lpCurItem)
// 参数：
//  IN  hWnd -- 窗口句柄
//	IN	lpCurItem -- 当前要加载的图象控件
// 
// 返回值：无
// 功能描述： 装载当前图象.
// 引用: 
// **************************************************
static void LoadImageFile(HWND hWnd,LPSHOWIMAGEITEM lpCurItem)
{
	LPTSTR lpImageFile;
	UINT uType;

		if (lpCurItem->lpLocatePath)
		{
			lpImageFile= lpCurItem->lpLocatePath;
		}
		else
		{
			lpImageFile= GetControlImg(lpCurItem->hControl);
		}

		uType = GetImageType(lpImageFile);  // 得到图形类型

		if (uType == TYPE_GIFFILE)
		{
			// 当前是GIF文件
			lpCurItem->hGif = Gif_LoadFromFile( lpImageFile ) ; // 装载GIF图象
			if (lpCurItem->hGif != NULL)
			{
				// 成功装载
				lpCurItem->iIndex = 0; // gif图象使用，当前正在播放的图象索引
				lpCurItem->iDelayTime = 0; // gif 图象使用，播放下一个索引图象还需要等待的时间
				lpCurItem->iImageNum = GetTotalPic(lpCurItem->hGif); // 得到当前GIF的图象总数
			}
		}
		if (uType == TYPE_BMPFILE)
		{
			lpCurItem->hBmp = LoadImage(0,lpImageFile,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);  // 装载BMP图象
		}
		if (uType == TYPE_JPEGFILE)
		{
			lpCurItem->hBmp = JPEG_LoadByName(lpImageFile,NULL);  // 装载JPEG图象
		}
}



// **************************************************
// 声明：static UINT GetImageType(LPTSTR lpFileName)
// 参数：
//	IN	lpFileName -- 指定的文件名
// 
// 返回值：返回指定文件的文件类型
// 功能描述： 得到当前图象文件的文件类型.
// 引用: 
// **************************************************
static UINT GetImageType(LPTSTR lpFileName)
{
	int iImageTypeNum , i;

		if (lpFileName == NULL)
			return TYPE_NOTIMGFILE; // 不是已知的图形文件

		iImageTypeNum = sizeof (tabImageType) / sizeof (FILETYPE);
		for (i = 0 ; i < iImageTypeNum; i++ )
		{
			if (FileNameCompare( tabImageType[i].lpFileExt, strlen( tabImageType[i].lpFileExt ), lpFileName, strlen(lpFileName)) == TRUE)
			{ // 比较当前文件的文件类型
				return  tabImageType[i].iFileType;  // 返回当前的文件类型
			}
		}
		return TYPE_NOTIMGFILE; //不是已知的图形文件
}

// **************************************************
// 声明：static int GetTotalPic(HGIF hGif)
// 参数：
// 	IN hGif --	GIF文件句柄
// 
// 返回值：返回当前的GIF的图象个数
// 功能描述： 得到当前的GIF的图象个数.
// 引用: 
// **************************************************
static int GetTotalPic(HGIF hGif)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // 设置结构大小
		//得到GIF的相关信息
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
			return gifInfo.nIndexNum; // 返回当前GIF的图象数
		return 0; // 没有得到图象数
}



// **************************************************
// 声明：BOOL ReleaseAnimationList(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE，失败返回FALSE。
// 功能描述：释放显示图象列表。
// 引用: 
// **************************************************
BOOL ReleaseAnimationList(HWND hWnd)
{
	LPSHOWIMAGEITEM lpCurItem,lpNextItem;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return 0;



		lpCurItem = lpHtmlView->lpShowImageList; //得到第一个图象条目

		// 如果下一个条目为空，则将新加条目添加到当前条目的下一个条目
		while(lpCurItem)
		{ //当前条目的下一个条目有数据
			lpNextItem = lpCurItem->next;  // 得到下一个条目 
			// 释放当前条目
			if (lpCurItem->lpLocatePath)
			{
				// 已经分配了一个本地文件名缓存，释放
				free(lpCurItem->lpLocatePath);
			}
			if (lpCurItem->hGif)
			{
				// 已经打开了一个GIF文件
				Gif_Destroy( lpCurItem->hGif ) ; // 关闭GIF文件
			}

			if (lpCurItem->hBmp)
			{ // 已经打开了BMP 或者 JPEG文件
				DeleteObject(lpCurItem->hBmp);  // 删除位图句柄
			}

			free(lpCurItem); // 释放条目结构
			lpCurItem = lpNextItem;  // 指向下一个条目
		}

		lpHtmlView->lpShowImageList = NULL; //清空条目
		return TRUE; // 返回成功
}


// **************************************************
// 声明：static void DrawBitMap(HDC hdc,HBITMAP hBitmap,LPRECT lprect)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBitmap -- 位图句柄
// 	IN rect -- 位图大小
// 	IN xOffset -- X偏移
// 	IN yOffset -- Y偏移
// 	IN dwRop -- 绘制模式
// 
// 返回值：无
// 功能描述：绘制位图。
// 引用: 
// **************************************************
static void DrawBitMap(HDC hdc,HBITMAP hBitmap,LPRECT lprect)
{
		HDC hMemoryDC;
    HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // 创建一个兼容的内存句柄
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // 将要绘制的位图设置到内存句柄中
		BitBlt( hdc, // handle to destination device context
			(short)lprect->left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)lprect->top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			0,   // x-coordinate of source rectangle's upper-left
									 // corner
			0,   // y-coordinate of source rectangle's upper-left
									 // corner

			SRCCOPY
			);

		SelectObject(hMemoryDC,hOldBitmap); // 恢复内存句柄
		DeleteDC(hMemoryDC); // 删除句柄
}

// **************************************************
// 声明：static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize)
// 参数：
// 	IN hGif --	GIF文件句柄
//  OUT lpImgSize -- 返回图象大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述： 得到当前的GIF的图象大小.
// 引用: 
// **************************************************
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // 设置结构大小
		//得到GIF的相关信息
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
		{
			// 得到信息成功
			lpImgSize->cx = gifInfo.nWidth;
			lpImgSize->cy = gifInfo.nHeight;
			return TRUE; // 返回当前GIF的图象数
		}
		return FALSE; // 没有得到图象数
}

// **************************************************
// 声明：static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize)
// 参数：
// 	IN hBmp --	BMP文件句柄
//  OUT lpImgSize -- 返回图象大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述： 得到当前的BMP的图象大小.
// 引用: 
// **************************************************
static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize)
{
	BITMAP bitmap;

		//装载成功
		if (GetObject(hBmp,sizeof(BITMAP),&bitmap))
		{
			//得到信息成功
			lpImgSize->cx = bitmap.bmWidth;
			lpImgSize->cy = bitmap.bmHeight;
			return TRUE; //  返回成功
		}
		return FALSE; // 没有得到图象数
}

// !!! Add By Jami chen in 2003.09.09
// **************************************************
// 声明：BOOL IsImageFile(LPTSTR lpFileName)
// 参数：
// 	IN lpFileName -- 指定文件
// 
// 返回值：是图象文件，则返回TRUE，否则返回FALSE
// 功能描述：判断指定文件是否是图象文件。
// 引用: 
// **************************************************
BOOL IsImageFile(LPTSTR lpFileName)
{
	UINT  uImgType;

		uImgType = GetImageType(lpFileName) ; // 

		if (uImgType == TYPE_NOTIMGFILE)
		{
			// 不是图形文件
			return FALSE;
		}

		return TRUE; // 是图形文件
}
// !!! Add By Jami chen in 2003.09.09
