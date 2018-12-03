/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __SMILSTRUCT_H
#define __SMILSTRUCT_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ewindows.h"


typedef struct METASTRUCT{
	int a;
}META ,*LPMETA;

// REGION�ṹ
typedef struct REGIONSTRUCT{
	LPTSTR id; // ���浱ǰREGION��ID��
	WORD top; // ��ǰregion������ľ��룬�Ե�Ϊ��λ
	WORD left; // ��ǰregion������ߵľ��룬�Ե�Ϊ��λ
	WORD width; // ��ǰregion�Ŀ�ȣ��Ե�Ϊ��λ
	WORD height; // ��ǰregion �ĸ߶ȣ��Ե�Ϊ��λ��
	WORD fit;
	WORD fill;
	WORD hidden;
	WORD meet;
	WORD slice;
	struct REGIONSTRUCT *next;
}REGION, *LPREGION;

//ROOT-LAYOUT�ṹ
#define SMIL_DEFAILTWIDTH	160
#define SMIL_DEFAILTHEIGHT  120


typedef struct ROOTLAYOUTSTRUCT{
	WORD width; // ��ǰSMIL�Ŀ�ȣ��Ե�Ϊ��λ
	WORD height; // ��ǰpar�ĸ߶ȣ��Ե�Ϊ��λ��
}ROOTLAYOUT ,*LPROOTLAYOUT;


// LAYOUT �ṹ
typedef struct LAYOUTSTRUCT{
	LPROOTLAYOUT lpRootLayout; // ��ǰSMIL�Ŀ����
	LPREGION lpRegion; // ÿһ��REGION������
}LAYOUT ,*LPLAYOUT;

// HEAD �ṹ
typedef struct HEADSTRUCT{
	LPMETA	lpMeta; // ��ǰ��Ϣ��meta-information
	LPLAYOUT lpLayout; // ��ǰ��Ϣ��REGION
}HEAD ,*LPHEAD;

// ͼ��ṹ
typedef struct IMGSTRUCT{
	LPTSTR lpRegion; // IMGָ����REGION
	LPTSTR lpSrc; // IMGָ����ͼ���ļ�
	LPTSTR lpAlt; // ����ı�
	struct PARSTRUCT *next; // ��һ��ͼ��ָ��
}IMG ,*LPIMG;

// �����ṹ
typedef struct ADUOSTRUCT{
	LPTSTR lpSrc; // AUDIOָ���������ļ�
	LPTSTR lpAlt; // ����ı�
	struct PARSTRUCT *next; // ��һ������
}AUDIO ,*LPAUDIO;

// �ı��ṹ
typedef struct TEXTSTRUCT{
	LPTSTR lpRegion; // TEXTָ����REGION
	LPTSTR lpSrc; // TEXTָ�����ı��ļ���
	LPTSTR lpAlt; // ����ı�
	struct PARSTRUCT *next;
}TEXT ,*LPTEXT;

// REF �ṹ
typedef struct REFSTRUCT{
	LPTSTR lpSrc; //REFָ�����ļ�
	LPTSTR lpAlt; // �滻�ı�
	struct PARSTRUCT *next;
}REF ,*LPREF;

// �õ�Ƭ(PAR)�ṹ
typedef struct PARSTRUCT{
	DWORD startTime; // ����õ�Ƭ��ʼ���Żõ�Ƭ��ʱ�䣬Ĭ��Ϊ0
	DWORD durTime; // ��ǰ���Żõ�ƬҪ������ʱ��
	DWORD endTime; // �����õ�Ƭ��ʱ��

	LPIMG lpImg; // ͼ��ṹ
	LPAUDIO lpAudio; // �����ṹ
	LPTEXT	lpText; // �ı��ṹ
	LPREF lpRef; // REF�ṹ
	struct PARSTRUCT *next; // ָ����һ���õ�Ƭ������
}PAR ,*LPPAR;

// BODY �ṹ
typedef struct BODYSTRUCT{
	LPPAR lpPar; // ָ��õ�Ƭ�ṹ
}BODY ,*LPBODY;
	
// SMIL�ṹ
typedef struct SMILSTRUCT{
	LPHEAD lpHead; // HEAD�ṹ
	LPBODY lpBody; // �ı��ṹ
}SMIL ,*LPSMIL;


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif //__SMILSTRUCT_H
