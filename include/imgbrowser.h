/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __IMGBROWSER_H
#define __IMGBROWSER_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

// 窗口风格

#define IBS_IMGSTRETCH		0x00000001   //  图象拉伸 ，只有BMP ，JPEG有效
#define IBS_SHOWTEXT		0X00000002	 //  需要显示文本
#define IBS_TEXTLEFT		0x00000004	 //  文本左对齐
#define IBS_TEXTCENTER		0x00000000	 //  文本水平方向居中 ，默认显示
#define IBS_TEXTRIGHT		0x00000008	 //  文本右对齐 ，以上三项只能设置一项
#define IBS_TEXTTOP			0x00000010	 //  文本显示在顶部
#define IBS_TEXTVCENTER		0x00000000	 //  文本显示在垂直方向的中间 ， 默认显示
#define IBS_TEXTBOTTOM		0x00000020   //  文本显示在底部 ，以上三项只能设置一项
#define IBS_TRANSPARENT		0x00000040	 //  文本透明显示
#define IBS_SINGLELINE		0x00000080	 //  文本单行显示


#define	IMG_UNKNOW		0X00   // 不知道类型的文件，系统会根据扩展名主动判断
#define IMG_BMPFILE		0x01   //  BMP文件
#define IMG_GIFFILE		0x02   //  GIF文件
#define IMG_JPGFILE		0x03   //  JPEG文件

/*
	设置当前要显示的MMS数据
	wParam  UINT  文件类型
	lParam  LPTSTR 文件名
*/
#define	IMB_SETIMGFILE		(WM_USER + 2000)  


// 通知消息
#define NMIMG_CLICK		4500     // 点击通知消息

typedef struct tagNMIMGBROWSER
{
    NMHDR   hdr;
    POINT   ptAction;
} NMIMGBROWSER, FAR *LPNMIMGBROWSER;


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__IMGBROWSER_H
