/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __PYSEARCH_H
#define __PYSEARCH_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus 


typedef struct KeyValue{
	BYTE bChar; // ����ֵ
	BYTE bVk;  // �������ֵ
	WORD wSBCCase; // ȫ�����
}KEYVALUE;

typedef struct {
    KEYVALUE  keyValue[6]; // ��ż�ֵ������
	int		iKeyNum;  // ��ֵ�ĸ���
    int     iLeft;  // ��ʼλ��
	int     iWidth; // ���Ŀ��
} KEYENTRY;


// ����һ����ǰƴ������Ľṹ
#define MAX_SPELLNUM  8
#define SPELLLISTNUM  10  // ��ϵͳ��������ַ���������ƴ����ϲ��ᳬ��10��
#define MAX_PYRESULT 256  // �����ǵ�ϵͳ�£�һ��ƴ������ƴд������ᳬ��256

typedef struct SpellResultStruct{
	// ��������Ϊѡ����һ��ƴд����Ч
	int iSelectIndex; // ��ǰѡ���ƴ����ϵ�����
	WORD lpSpellResult[MAX_PYRESULT]; // ���ƴ�����
	int iSpellResultNum;  // ƴд�����Ŀ
	int iCurPage ;  // ��ǰ����ʾ����Ļ�����ݵ�ҳ����
}SPELLRESULT, *LPSPELLRESULT;

// ����һ��ƴ��ƴд�ṹ
typedef struct SpellListStruct{
	KEYENTRY *pKey[MAX_SPELLNUM] ;  // ��ǰ�û��Ѿ�����ļ�ֵ
	int iKeyNum; // ��ǰ�Ѿ�������ַ���
	TCHAR lpSpellList[SPELLLISTNUM][MAX_SPELLNUM];  // ����ƴ������Ϊ6����chuang,zhuang ...��
	int iSpellNum ;  // ��ǰһ���ж�����ƴ�����
}SPELLLIST,*LPSPELLLIST;

BOOL InsertKeyToSpell(LPSPELLLIST lpSpellList,KEYENTRY *pKey); // ����һ������ƴд�����
int PYSpell(LPCTSTR lpSpell,WORD *lpSpellResult,int iMaxLenght); // ƴд����
BOOL ReSpellList(LPSPELLLIST lpSpellList);

extern const TCHAR classSpellArea[];
ATOM RegisterSpellAreaWindow(HINSTANCE hInstance); // ע����д��������


// ƴд����
#define SPM_INSERTKEY   (WM_USER + 2100)  // ����һ������ƴд����
#define SPM_GETSELSPELL (WM_USER + 2101)  // �õ���ǰ��ѡ��ƴд
#define SPM_CLEARSPELL  (WM_USER + 2102)  // ������е�ƴд
#define SPM_DELETEKEY   (WM_USER + 2103)  // ɾ��һ����
#define SPM_GETKEYNUM   (WM_USER + 2104)  // �õ���ǰƴд�����м�����Ŀ

// ƴд����֪ͨ��Ϣ��ͨ��Message ֱ�ӷ���
#define SPN_SELCHANGE	(WM_USER + 2200)  // ƴд���ڵ�ѡ�����ı�
#define SPN_NODATA		(WM_USER + 2201)  // ƴд�����Ѿ�û������


#ifdef __cplusplus
}           
#endif  // __cplusplus
#endif  //__PYSEARCH_H
