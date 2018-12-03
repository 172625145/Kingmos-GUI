/*********************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
*********************************************************/

/*****************************************************
文件说明：Mobinonte应用程序
版本号：0.0.1
开发时期：2005
作者：liujunplus
修改记录：
******************************************************/
#include <ewindows.h>
/******************************************************
名称		Videos			Music		Games			Trave			AreoWeek		Setting
Mobi背景	255,169,0		255,126,0	255,104,136		159,109,202		96,124,220		71,149,205
Menu前景	255,58,0		255,50,0	255,0,0			136,0,179		83,83,192		3,104,192
Menu背景	255,169,0		255,126,0	255,104,136		159,109,202		96,124,220		71,149,205
注意：【Mobi背景】与【Menu背景】颜色值相同
******************************************************/

#ifndef __MOBINONTE_H
#define __MOBINONTE_H

// 定义窗口的范围
#define	WINDOW_X_S		0
#define	WINDOW_Y_S		0
#define	WINDOW_X_E		840
#define	WINDOW_Y_E		480

#define MOBINONTE_W		792
#define MOBINONTE_H		36

#define MOBI_H_MOBI_X		36
#define MOBI_H_MOBI_Y		24
#define MOBI_FORE_X			27	// 选择条的 X 坐标
#define MOBI_FORE_ITEM_X	40	// 选择条内容的 X 坐标

#define ARROW_ICON_X		396

#define LIST_X			0
#define LIST_Y			90
#define LIST_W			465
#define LIST_H			349
#define LIST_ITEM_W		393

#define ITEM_SEL_NUM	6				// 定义主选项的个数

// 定义文件目录的可操作性
#define FILE_DISABLE	0x0000
#define FILE_RIGHT		0x0001
#define FILE_LEFT		0x0002
#define FILE_BITMAP		0x0004		// 图片文件

// 定义字符串大小
#define MAINMENU_SIZE		15		// 主选项字符串长度
#define MOBINONTE_SIZE		80		// 主选项图片路径


// 定义消息
#define WM_GETMOVIEDC	WM_USER+1

// 定义主窗口——主选项名称和图片路径
typedef struct	__str_MOBINONTE {
	char	szTitle[MAINMENU_SIZE];				// 主标题名称
	char	szMobinonte[MOBINONTE_SIZE];		// Mobinonte图片----1
	char	szBigBkground[MOBINONTE_SIZE];		// 窗口大背景-------7
}MOBINONTE;

// 定义主窗口——主选项图片句柄
typedef struct	__str_HBITMAP_MOBINONTE {
	HBITMAP		hMobinonte;							// Mobinonte句柄----1
	HBITMAP		hBigBkground;						// 窗口大背景-------7
	HBRUSH		hSelForegroundColor;				// 选择条前景句柄-------3
	HBRUSH		hSelBackgroundColor;				// 选择条背景句柄-------4
}HBITMAP_MOBINONTE;

// 定义程序公用部分
typedef struct	__str_COMMON_MOBI
{
	HPEN	hPen;									// 无形外框
	HPEN	hRedPen;								// 无形外框
	HBRUSH	hWhiteBrush;							// 白色颜色画刷句柄
	char	szArrow[MOBINONTE_SIZE];				// 箭头-------------6（用ICON实现）
	HICON	hArrow;									// 箭头-------------6
}COMMON_MOBI;

// 定义全局结构
typedef struct	__str_GLOBAL_MOBI
{
	UINT				uSize;
	MOBINONTE			strMobinonte[ITEM_SEL_NUM];
	HBITMAP_MOBINONTE	hbitmapMobinonte[ITEM_SEL_NUM];
	COMMON_MOBI			cCommonMobi;
}GLOBAL_MOBI;

typedef struct	__str_CHOICE_MENU
{
	BOOL	bValid;			// 菜单选项是否有效
	int		iOldSel;		// 选定的是第几项
	char	szText[30];		// 菜单名称
}CHOICE_MENU;









#endif
