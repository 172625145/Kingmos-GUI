/*********************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
*********************************************************/

/*****************************************************
�ļ�˵����MobinonteӦ�ó���
�汾�ţ�0.0.1
����ʱ�ڣ�2005
���ߣ�liujunplus
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
/******************************************************
����		Videos			Music		Games			Trave			AreoWeek		Setting
Mobi����	255,169,0		255,126,0	255,104,136		159,109,202		96,124,220		71,149,205
Menuǰ��	255,58,0		255,50,0	255,0,0			136,0,179		83,83,192		3,104,192
Menu����	255,169,0		255,126,0	255,104,136		159,109,202		96,124,220		71,149,205
ע�⣺��Mobi�������롾Menu��������ɫֵ��ͬ
******************************************************/

#ifndef __MOBINONTE_H
#define __MOBINONTE_H

// ���崰�ڵķ�Χ
#define	WINDOW_X_S		0
#define	WINDOW_Y_S		0
#define	WINDOW_X_E		840
#define	WINDOW_Y_E		480

#define MOBINONTE_W		792
#define MOBINONTE_H		36

#define MOBI_H_MOBI_X		36
#define MOBI_H_MOBI_Y		24
#define MOBI_FORE_X			27	// ѡ������ X ����
#define MOBI_FORE_ITEM_X	40	// ѡ�������ݵ� X ����

#define ARROW_ICON_X		396

#define LIST_X			0
#define LIST_Y			90
#define LIST_W			465
#define LIST_H			349
#define LIST_ITEM_W		393

#define ITEM_SEL_NUM	6				// ������ѡ��ĸ���

// �����ļ�Ŀ¼�Ŀɲ�����
#define FILE_DISABLE	0x0000
#define FILE_RIGHT		0x0001
#define FILE_LEFT		0x0002
#define FILE_BITMAP		0x0004		// ͼƬ�ļ�

// �����ַ�����С
#define MAINMENU_SIZE		15		// ��ѡ���ַ�������
#define MOBINONTE_SIZE		80		// ��ѡ��ͼƬ·��


// ������Ϣ
#define WM_GETMOVIEDC	WM_USER+1

// ���������ڡ�����ѡ�����ƺ�ͼƬ·��
typedef struct	__str_MOBINONTE {
	char	szTitle[MAINMENU_SIZE];				// ����������
	char	szMobinonte[MOBINONTE_SIZE];		// MobinonteͼƬ----1
	char	szBigBkground[MOBINONTE_SIZE];		// ���ڴ󱳾�-------7
}MOBINONTE;

// ���������ڡ�����ѡ��ͼƬ���
typedef struct	__str_HBITMAP_MOBINONTE {
	HBITMAP		hMobinonte;							// Mobinonte���----1
	HBITMAP		hBigBkground;						// ���ڴ󱳾�-------7
	HBRUSH		hSelForegroundColor;				// ѡ����ǰ�����-------3
	HBRUSH		hSelBackgroundColor;				// ѡ�����������-------4
}HBITMAP_MOBINONTE;

// ��������ò���
typedef struct	__str_COMMON_MOBI
{
	HPEN	hPen;									// �������
	HPEN	hRedPen;								// �������
	HBRUSH	hWhiteBrush;							// ��ɫ��ɫ��ˢ���
	char	szArrow[MOBINONTE_SIZE];				// ��ͷ-------------6����ICONʵ�֣�
	HICON	hArrow;									// ��ͷ-------------6
}COMMON_MOBI;

// ����ȫ�ֽṹ
typedef struct	__str_GLOBAL_MOBI
{
	UINT				uSize;
	MOBINONTE			strMobinonte[ITEM_SEL_NUM];
	HBITMAP_MOBINONTE	hbitmapMobinonte[ITEM_SEL_NUM];
	COMMON_MOBI			cCommonMobi;
}GLOBAL_MOBI;

typedef struct	__str_CHOICE_MENU
{
	BOOL	bValid;			// �˵�ѡ���Ƿ���Ч
	int		iOldSel;		// ѡ�����ǵڼ���
	char	szText[30];		// �˵�����
}CHOICE_MENU;









#endif
