/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __IMGBROWSER_H
#define __IMGBROWSER_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

// ���ڷ��

#define IBS_IMGSTRETCH		0x00000001   //  ͼ������ ��ֻ��BMP ��JPEG��Ч
#define IBS_SHOWTEXT		0X00000002	 //  ��Ҫ��ʾ�ı�
#define IBS_TEXTLEFT		0x00000004	 //  �ı������
#define IBS_TEXTCENTER		0x00000000	 //  �ı�ˮƽ������� ��Ĭ����ʾ
#define IBS_TEXTRIGHT		0x00000008	 //  �ı��Ҷ��� ����������ֻ������һ��
#define IBS_TEXTTOP			0x00000010	 //  �ı���ʾ�ڶ���
#define IBS_TEXTVCENTER		0x00000000	 //  �ı���ʾ�ڴ�ֱ������м� �� Ĭ����ʾ
#define IBS_TEXTBOTTOM		0x00000020   //  �ı���ʾ�ڵײ� ����������ֻ������һ��
#define IBS_TRANSPARENT		0x00000040	 //  �ı�͸����ʾ
#define IBS_SINGLELINE		0x00000080	 //  �ı�������ʾ


#define	IMG_UNKNOW		0X00   // ��֪�����͵��ļ���ϵͳ�������չ�������ж�
#define IMG_BMPFILE		0x01   //  BMP�ļ�
#define IMG_GIFFILE		0x02   //  GIF�ļ�
#define IMG_JPGFILE		0x03   //  JPEG�ļ�

/*
	���õ�ǰҪ��ʾ��MMS����
	wParam  UINT  �ļ�����
	lParam  LPTSTR �ļ���
*/
#define	IMB_SETIMGFILE		(WM_USER + 2000)  


// ֪ͨ��Ϣ
#define NMIMG_CLICK		4500     // ���֪ͨ��Ϣ

typedef struct tagNMIMGBROWSER
{
    NMHDR   hdr;
    POINT   ptAction;
} NMIMGBROWSER, FAR *LPNMIMGBROWSER;


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__IMGBROWSER_H
