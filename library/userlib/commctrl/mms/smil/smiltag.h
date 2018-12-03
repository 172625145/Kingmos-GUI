/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __SMILTAG_H
#define __SMILTAG_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

typedef int TAGID; // ����TAGID����������

// �����ﶨ���ǩ�ı��
#define SMILTAG_UNKNOW		0x0000
#define SMILTAG_NULL		0x3180
#define SMILTAG_SMIL		(SMILTAG_NULL + 1)	// <SMIL>
#define SMILTAG_HEAD		(SMILTAG_NULL + 2)	// <HEAD>		
#define SMILTAG_BODY		(SMILTAG_NULL + 3)	// <BODY>
#define SMILTAG_META		(SMILTAG_NULL + 4)	// <META>
#define SMILTAG_LAYOUT		(SMILTAG_NULL + 5)	// <LAYOUT>
#define SMILTAG_ROOTLAYOUT	(SMILTAG_NULL + 6)	// <ROOT-LAYOUT>
#define SMILTAG_REGION		(SMILTAG_NULL + 7)	// <REGION>
#define SMILTAG_PAR			(SMILTAG_NULL + 8)	// <PAR>
#define SMILTAG_IMG			(SMILTAG_NULL + 9)	// <IMG>
#define SMILTAG_TEXT		(SMILTAG_NULL + 10)	// <TEXT>
#define SMILTAG_AUDIO		(SMILTAG_NULL + 11)	// <AUDIO>
#define SMILTAG_REF			(SMILTAG_NULL + 12)	// <REF>

#define SMILTAG_END			0x1000 // ��β��ǩ��־�������ͬһ����ǩ����
								   // �����β��ǩΪԭ��ǩ��� + ��β��ǩ��־
								   // �� <smil> id == SMILTAG_SMIL , �� </smil> id = SMILTAG_SMIL + SMILTAG_END

typedef int ATTRID; // ����ATTRID����������

// �����ﶨ�����Եı��
#define SMILATTR_UNKNOW		0x0000
#define SMILATTR_NULL		0x6290		
#define SMILATTR_ID			(SMILATTR_NULL + 1)		// "ID"
#define SMILATTR_LEFT		(SMILATTR_NULL + 2)		// "LEFT"	 
#define SMILATTR_TOP		(SMILATTR_NULL + 3)		// "TOP"
#define SMILATTR_WIDTH		(SMILATTR_NULL + 4)		// "WIDTH"
#define SMILATTR_HEIGHT		(SMILATTR_NULL + 5)		// "HEIGHT"
#define SMILATTR_FIT		(SMILATTR_NULL + 6)		// "FIT"
#define SMILATTR_FILL		(SMILATTR_NULL + 7)		// "FILL"
#define SMILATTR_HIDDEN		(SMILATTR_NULL + 8)		// "HIDDEN"
#define SMILATTR_MEET		(SMILATTR_NULL + 9)		// "MEET"
#define SMILATTR_SLICE		(SMILATTR_NULL + 10)	// "SLICE"
#define SMILATTR_SRC		(SMILATTR_NULL + 11)	// "SRC"
#define SMILATTR_REGION		(SMILATTR_NULL + 12)	// "REGION"
#define SMILATTR_ALT		(SMILATTR_NULL + 13)	// "ALT"
#define SMILATTR_BEGIN		(SMILATTR_NULL + 14)    // "BEGIN"
#define SMILATTR_END		(SMILATTR_NULL + 15)	// "END"
#define SMILATTR_DUR		(SMILATTR_NULL + 16)	// "DUR"



TAGID SMIL_LocateTag(char **pStream); // �õ���ǰ�ı�ǩ���
void SMIL_ToNextTag(char **pStream);  // ��ָ��ָ����һ����ǩ
ATTRID SMIL_LocateAttribute(char **pStream); // �õ���ǩ���Ա��
BOOL SMIL_LocateAttributeContent(char **pStream,char *pAttrContent,DWORD dwSize); // �õ���ǩ��������


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif //__SMILTAG_H

