/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef _ECONTRACT_H
#define _ECONTRACT_H
#ifdef _cpluscplus
extern "C"{
#endif
#include <thirdpart.h>
#include <eversion.h>
#include <telephone.h>
#define CNTLS_NOCOLUMNHEADER  (0x01)
#define CNTLS_HORZLINE (0x02)
#define CNTLS_VERTLINE (0x04)
#define CNTLS_ALIGNTOP (0x08)
#define CNTLS_ALIGNLEFT (0x10)
#define CNTLS_REPORT (0x20)
#define CNTLS_SINGLESEL (0x40)
#define CNTLS_SHOWSELALWAYS (0x80)
#define CNTLS_AUTOWIDTH (0x100)
#define CNTLS_NOTINITDATA (0x200)
#define CNTLS_DATFROMFILE (0x400)
#define CNTLS_SORTASCENDING (0x800)
#define CNTLS_SORTDESCENDING (0x1000)
#define CNTLS_LIST (0x2000)
#define CNTLS_ATTACHSIMCARD (0x4000)

#define CNTVS_VIEWBAR 0x1
#define CNTVS_CNTAPVIEW 0X2

#define IDC_LISTCTRL (WM_USER + 1)
#define BASE_VERSION (100)
extern const TCHAR g_szDefCoTable[];

#define BASE_SIMCNTID ((DWORD)-1 - 300)
#define ALL_CNTCAT ((DWORD)-1)
#define SIM_CNTCAT (ALL_CNTCAT - 1)
#define CNT_UNCAT (ALL_CNTCAT - 2)

#define MAX_ITEMTITLE_LEN 12
#define NAME_LEN 20
#define PHONENUMBER_LEN (MAX_PHONE_LEN)
#define HOMEPAGE_LEN 50
#define DEPARTMENT_LEN 40
#define POSTCODE_LEN 8
#define QQ_LEN 10
#define COMPANY_LEN 20
#define POSITION_LEN 20
#define IMAGE_FILE_LEN MAX_PATH
#define RING_FILE_LEN MAX_PATH
#define BIRTHDAY_LEN 10
#define ONE_CATEGORY_ITEM_LEN (sizeof(DWORD))
#define PRAMIRY_CONTACT_LEN (sizeof(DWORD))

#ifdef ZT_PHONE
#define CONTACT_VERSION (BASE_VERSION + 1)
#define ITEM_NUM (15)
#define MAX_CATEGORY_NUM 15
#define EMAIL_LEN 20
#define MOST_CATEGORY 4
#define NOTE_LEN 256
#define CATEGORY_NAME_LEN 12
#define ADDRESS_LEN 40
#define CATEGORYITEM_LEN (MOST_CATEGORY * ONE_CATEGORY_ITEM_LEN)
#else
#define CONTACT_VERSION BASE_VERSION
#define ITEM_NUM (15)
#define ADDRESS_LEN 80
#define EMAIL_LEN 50
#define NOTE_LEN 400
#define CATEGORY_NAME_LEN 20
#define MAX_CATEGORY_NUM 100
#define CATEGORYITEM_LEN (MAX_CATEGORY_NUM * ONE_CATEGORY_ITEM_LEN)
#endif

#define CNT_SUCCESS 0
#define CNT_OPENTABLE_ERROR 1
#define CNT_NOENOUGH_SPACE 2
#define CNT_FILE_UNCOMPATIBLE 3 
#define CNT_READTABLE_ERROR 4
#define CNT_CREATEOBJEC_FAIL 5
#define CNT_NOENOUGH_MEM 6
#define CNT_RECORD_NOTFUND 7
#define CNT_CATEEXIST 8
#define CNT_NEWCATE 9
#define CNT_NOT_INTABLE 10
#define CNT_REFNOTZERO 11
#define CNT_DELETED 12
#define CNT_INC_DELETE 14
#define CNT_VERSION_LOW 15
#define CNT_UNCONVERT 16
#define CNT_UNKNOWSRVICETYPE 17
#define CNT_UNMATCHEDSERVICE 18
#define CNT_OPERWNDDESTROY 19

#define CNT_NEWCREATEONE 20
#define CNT_MODIFYONE 21
#define CNT_CATENUM_BEYOND 22

#define CNT_BELONGCURLIST 23
#define CNT_ONLYBELONGCURCATE 24
#define CNT_NOTBELONGCURLIST 25


#define ID_NAME 0
#define ID_MOBILE 1
#define ID_OPHONE 2
#define ID_OFAX 3
#define ID_HPHONE 4
#define ID_HFAX 5
#define ID_EMAIL 6
#define ID_QQ 7
#define ID_HOMEPAGE 8
#define ID_OADDRESS 9
#define ID_HADDRESS 10
#define ID_OPCODE 11
#define ID_HPCODE 12
#define ID_CATEGORY 13
#define ID_NOTE 14

#define ID_RING_FILE 15
#define ID_IMAGE_FILE 16
#define ID_PRIMARY_CONTACT 17
#define ID_COMPANY 18
#define ID_POSITION 19
#define ID_BIRTHDAY 20


#ifdef ZT_PHONE
#define ALL_ITEMID(pAllIDAry) \
	static WORD pAllIDAry[] = {ID_NAME, /*0*/\
ID_IMAGE_FILE/*1*/, \
ID_MOBILE, ID_OPHONE, ID_HPHONE, ID_OFAX, /*2-5*/\
ID_PRIMARY_CONTACT, ID_CATEGORY, /*6-7*/\
ID_COMPANY, ID_POSITION, ID_HADDRESS, ID_RING_FILE, ID_EMAIL, ID_BIRTHDAY, /*8-13*/\
ID_NOTE/*14*/\
};

#define ALLCNT_ITEMTITLE(pItemTitleAry) \
	const static ITEM_TITLE pItemTitleAry[] = {\
	{TEXT("����:"), NAME_LEN, ID_NAME},\
	{TEXT("ͼ���ļ�:"), IMAGE_FILE_LEN, ID_IMAGE_FILE},\
	{TEXT("�ֻ�:"), PHONENUMBER_LEN, ID_MOBILE},\
	{TEXT("����:"), PHONENUMBER_LEN, ID_OPHONE},\
	{TEXT("סլ:"), PHONENUMBER_LEN, ID_HPHONE},\
	{TEXT("����:"), PHONENUMBER_LEN, ID_OFAX},\
	{TEXT("Ԥ��:"), PRAMIRY_CONTACT_LEN, ID_PRIMARY_CONTACT},\
	{TEXT("���:"), CATEGORYITEM_LEN, ID_CATEGORY},\
	{TEXT("��˾:"), COMPANY_LEN, ID_COMPANY},\
	{TEXT("ְλ:"), POSITION_LEN, ID_POSITION},\
	{TEXT("��ַ:"), ADDRESS_LEN, ID_HADDRESS},\
	{TEXT("����:"), RING_FILE_LEN, ID_RING_FILE},\
	{TEXT("Email:"), EMAIL_LEN, ID_EMAIL},\
	{TEXT("����:"), BIRTHDAY_LEN, ID_BIRTHDAY},\
	{TEXT("��ע:"), NOTE_LEN, ID_NOTE}\
}

#define ALLCNT_ITEMLEN(pItemLenAry) \
	static ITEM_LEN pItemLenAry[] = {\
	{NAME_LEN, ID_NAME},\
	{IMAGE_FILE_LEN, ID_IMAGE_FILE},\
	{PHONENUMBER_LEN, ID_MOBILE},\
	{PHONENUMBER_LEN, ID_OPHONE},\
	{PHONENUMBER_LEN, ID_HPHONE},\
	{PHONENUMBER_LEN, ID_OFAX},\
	{PRAMIRY_CONTACT_LEN, ID_PRIMARY_CONTACT},\
	{CATEGORYITEM_LEN, ID_CATEGORY},\
	{COMPANY_LEN, ID_COMPANY},\
	{POSITION_LEN, ID_POSITION},\
	{ADDRESS_LEN, ID_HADDRESS},\
	{RING_FILE_LEN, ID_RING_FILE},\
	{EMAIL_LEN, ID_EMAIL},\
	{BIRTHDAY_LEN, ID_BIRTHDAY},\
	{NOTE_LEN, ID_NOTE}\
}
#else
#define ALL_ITEMID(pAllIDAry) \
	static WORD pAllIDAry[] = {ID_NAME, ID_MOBILE, ID_OPHONE, ID_OFAX, ID_HPHONE, ID_HFAX, ID_EMAIL, ID_QQ, ID_HOMEPAGE, ID_OADDRESS,\
ID_HADDRESS, ID_OPCODE, ID_HPCODE, ID_CATEGORY, ID_NOTE};

#define ALLCNT_ITEMTITLE(pItemTitleAry) \
	const static ITEM_TITLE pItemTitleAry[] = {\
	{TEXT("����:"), NAME_LEN, ID_NAME},\
	{TEXT("�ֻ�:"), PHONENUMBER_LEN, ID_MOBILE},\
	{TEXT("�绰(O):"), PHONENUMBER_LEN, ID_OPHONE},\
	{TEXT("����(O):"), PHONENUMBER_LEN, ID_OFAX},\
	{TEXT("�绰(H):"), PHONENUMBER_LEN, ID_HPHONE},\
	{TEXT("����(H):"), PHONENUMBER_LEN, ID_HFAX},\
	{TEXT("E-mail:"), EMAIL_LEN, ID_EMAIL},\
	{TEXT("QQ:"), QQ_LEN, ID_QQ},\
	{TEXT("��ҳ:"), HOMEPAGE_LEN, ID_HOMEPAGE},\
	{TEXT("��ַ(O):"), ADDRESS_LEN, ID_OADDRESS},\
	{TEXT("��ַ(H):"), ADDRESS_LEN, ID_HADDRESS},\
	{TEXT("�ʱ�(O):"), POSTCODE_LEN, ID_OPCODE},\
	{TEXT("�ʱ�(H):"), POSTCODE_LEN, ID_HPCODE},\
	{TEXT("���:"), CATEGORYITEM_LEN, ID_CATEGORY},\
	{TEXT("��ע:"), NOTE_LEN, ID_NOTE}\
}

#define ALLCNT_ITEMLEN(pItemLenAry) \
	static ITEM_LEN pItemLenAry[] = {\
	{NAME_LEN, ID_NAME},\
	{PHONENUMBER_LEN, ID_MOBILE},\
	{PHONENUMBER_LEN, ID_OPHONE},\
	{PHONENUMBER_LEN, ID_OFAX},\
	{PHONENUMBER_LEN, ID_HPHONE},\
	{PHONENUMBER_LEN, ID_HFAX},\
	{EMAIL_LEN, ID_EMAIL},\
	{QQ_LEN, ID_QQ},\
	{HOMEPAGE_LEN, ID_HOMEPAGE},\
	{ADDRESS_LEN, ID_OADDRESS},\
	{ADDRESS_LEN, ID_HADDRESS},\
	{POSTCODE_LEN, ID_OPCODE},\
	{POSTCODE_LEN, ID_HPCODE},\
	{CATEGORYITEM_LEN, ID_CATEGORY},\
	{NOTE_LEN, ID_NOTE}\
}
#endif
/////��ϵ���������ʽ������д����ϡ�
#define CNT_READ 0x1
#define CNT_WRITE 0x2
#define CNT_IMPORT 0x4
#define CNT_EXPORT 0x8
////////////////////////
/////////////��ϵ��������
#define CNS_CREATEONE 1//�½���ϵ
#define CNS_CREATELIST 2//������ϵ����ʽ
#define CNS_PUREDATA 3//�������������ڵ���ϵ������
#define CNS_VIEWONE 4//���һ����ϵ
#define CNS_MODIFYONE 5//�޸�һ����ϵ��
///////////////////////////////

////////////////////////֪ͨ��Ϣ����
#define CNN_STORE (WM_USER + 1) //����CNS_CREATEONE, CNS_MODIFYONE������ϵ�������
	//����ĳ����ϵ�����޸Ļ����һ���µ���ϵʱ�����򴴽���ʱ���ݵĸ����ھ������֪ͨ��Ϣ��
#define CNN_ACTIVATEITEM (WM_USER + 2)//����CNS_CREATELIST������ϵ�������
//��������ʽ�е�һ����Ŀʱ�����򴴽���ʱ���ݵĸ����ھ������֪ͨ��Ϣ��
#define CNN_SELITEM (WM_USER + 3)//����CNS_VIEWONE, CNS_MODIFYONE, CNS_CREATEONE������ϵ�������
								//��ѡ��һ����Ŀʱ�����򴴽���ʱ���ݵĸ����ھ������֪ͨ��Ϣ��

#define CNN_CLICK (WM_USER + 4)//����CNS_CREATELIST������ϵ�������
							  //�������ʽ�е�һ����Ŀʱ�����򴴽���ʱ���ݵĸ����ھ������֪ͨ��Ϣ��
#define CNN_ITEMCHANGED (WM_USER + 5)//����CNS_CREATELIST������ϵ�������
									//�������ʽ�е�һ����Ŀ�б仯ʱ�����򴴽���ʱ���ݵĸ����ھ������֪ͨ��Ϣ��
#define CNN_CATEADD (WM_USER + 10)
#define CNN_CATEDEL (WM_USER + 11)
#define CNN_CNTDEL (WM_USER + 12)
#define CNN_CNTADD (WM_USER + 13)
#define CNN_CNTFRESH (WM_USER + 14)
#define CNN_CATEFRESH (WM_USER + 15)

typedef struct _CNN_CATEHDR{
	NMHDR nmhdr;
	DWORD dwResult;
	DWORD dwCateID;
}CNN_CATEHDR, *PCNN_CATEHDR;

typedef struct _CNN_CNTHDR{
	NMHDR nmhdr;
	BOOL bNewCnt;//���ΪTRUE��ʽ�½���һ����ϵ�������ʽ�޸���һ����ϵ��
	DWORD dwCntID;//���ΪTRUE��ʽ�½����޸ĵ���ϵ�ı�ʶ�š�
}CNN_CNTHDR, *PCNN_CNTHDR;

typedef struct _CNN_ACTIVATEITEMHDR{
	NMHDR nmhdr;
	DWORD dwCntID;
}CNN_ACTIVATEITEMHDR, *PCNN_ACTIVATEITEMHDR;

typedef CNN_ACTIVATEITEMHDR CNN_CLICKHDR, *PCNN_CLICKHDR;

typedef struct _CNN_SELITEMHDR{
	NMHDR nmhdr;
	WORD wItemID;
	WORD wReserve;
}CNN_SELITEMHDR, *PCNN_SELITEMHDR;

/////////////////////////
typedef struct _ITEM_TITLE{
	TCHAR szTitle[MAX_ITEMTITLE_LEN];
	WORD wReserve;
	WORD wID;
}ITEM_TITLE, *PITEM_TITLE;

typedef struct _ITEM_LEN{
	WORD wMaxItemLen;
	WORD wID;
}ITEM_LEN, *PITEM_LEN;

typedef struct _CNT_ITEM{
	WORD wID;
	WORD wDataLen;
	PVOID pData;
}CNT_ITEM, *PCNT_ITEM;

typedef struct _NMCNTLIST
{
    NMHDR hdr;
	HWND  hListView;
}NMCNTLIST, *LPNMCNTLIST;

typedef struct _CNT_RECORD{
	DWORD dwCntID;
	WORD wItemNum;
	WORD wReserve;
	CNT_ITEM pItemAry[1];
}CNT_RECORD, *PCNT_RECORD;

typedef struct _CNT_CATEGORY{
	DWORD dwCntID;
	WORD wCateNum;
	WORD wReserve;
	DWORD pdwCateIDAry[1];
}CNT_CATEGORY, *PCNT_CATEGORY;

typedef struct _CNT_CATEIMPORT{
	WORD wPreNum;
	WORD wImportNum;
	DWORD pdwCateID[1][2];
}CNT_CATEIMPORT, *PCNT_CATEIMPORT;

#define MF_IMGFILE 0x1
#define MF_RINGFILE 0x2
#define MF_NAME 0x4
#define MF_ID 0x8
typedef struct _THIRDPART_FIND{
	DWORD dwSize;
	UINT fMask; //MF_IMGFILE, MF_RINGFILE, MF_NAME
	TCHAR *pszPhonNumber;//���ݵ绰������Ҷ�Ӧ��������ͼƬ������
	TCHAR *pszImgFile;
	TCHAR *pszRingFile;
	TCHAR *pszName;
	DWORD dwCntID;//out
}THIRDPART_FIND, *PTHIRDPART_FIND;

typedef struct _CNT_FIND{
	TCHAR *pszFindStr;
	DWORD dwCateID;
	WORD wID;
	WORD wCntIDBufNum;
	BOOL bFindOnlyNotEmpty;
	BOOL bMatchWhole;//�ǲ�����ȫƥ��
	BOOL bMatchCase;//�Ƿ����ִ�Сд	
	DWORD pCntIDAry[1];
}CNT_FIND, *PCNT_FIND;

LRESULT 
Cnt_CallServices(
	HANDLE hCnt,
	DWORD dwCntCmd,
	WPARAM wParam,
	LPARAM lParam
);
/////////////��CNS_CREATELIST��ص�����
/*
Describe: �õ���ϵ����ʽ�е�ǰѡ�е�������ϵ�ı�ʶ(ID)
wParam:����ĸ���,��lParamΪNULLʱ��Ч��
lParam:���ΪNULL, ��Cnt_CallServices()����ѡ�еĸ���������ָ��һ��������׵�ַ,��������ѡ�е���ϵ��ID
return value:���صõ��ĸ���.
*/
#define CNC_GETSELID 2
/*����ϵ���в�����������ϵ
Describe: ����ϵ����ʽ�в��Ҳ�����������ϵ
����ָ����Ŀ�е����ݰ���ָ������
wParam:NULL;
lParam:ָ��PCNT_FIND�ṹָ�롣
wID:ָ������ϵ��Ŀ��ʶ�š�
bFindOnlyNotEmpty:���ΪTRUE,�����ʱֻҪwID��ʾ���Ϊ������ϲ���Ҫ������ĳ�Ա��Ч��
dwCateID: Ϊ-1ʱ������������ϵ�в��ң�����ֻ����ָ��������е���ϵ��
pszFindStr: ָ��Ҫ���ҵĴ������Ϊ""��bMatchWhole = FALSE���г�wIDָ�����Ŀ���ȷ������ϵ��������ʱCNT_FIND�ṹ��������Ա����������.
bMatchWhole��ΪTRUE����ҵ�����ϵ��wID��ʾ������pszFindStrָ��Ĵ���ȣ�����ֻҪ��ϵ��wID��ʾ�����а�����pszFindStrָ��Ĵ�����Ϊ�Ƿ��ϲ���Ҫ�����ϵ��
bMatchCase: ΪTRUE����ҹ�����pszFindStrָ��Ĵ���wID��ʾ������Ƚ�ʱ���ִ�Сд���������֡�
return value:CNT_SUCESS��ʾ�ɹ�������ʧ�ܡ�
*/
#define CNC_LISTFIND 3
/*
Describe: ɾ����ϵ����ʽ��ѡ�е���ϵ��
wParam:NULL;
lParam:NULL;
*/
#define CNC_DELSELCNT 4
/*
Describe: ˢ����ϵ����ʽ
wParam:NULL;
lParam:NULL;
����CNT_SUCCESS��ɹ�������ʧ��
*/
#define CNC_FRESHLIST 6
/*
Describe: ����ϵ����ʽ�����һ����ϵ��
wParam:TRUE��ʾ����һ����FALSE��ʾ�޸�һ��;
lParam:��ϵ�ı�ʾ��;
����CNT_SUCCESS��ɹ�������ʧ��
*/
#define CNC_ADDITEMTOLIST 7
/*
Describe: ������ϵ����ʽ���п�
wParam:�ڼ���
lParam:��ȡ�
*/
#define CNC_SETCOLWIDTH 8
/*
Describe:�ж�ָ������ϵ�Ƿ�������ϵ����ʽ��
��Ϊ����ϵ����ʽ����������CNC_LISTFIND�����ǰ��ϵ��������ʾ���������һ�η���CNC_LISTFIND����
ʱָ������ϵ���Ͳ��Ҵ���ص���ϵ������CNC_VERIFYCNTBELONGCURLIST����������ָ������ϵ�Ƿ��뵱ǰ
����ϵ���Ͳ��Ҵ���ء�
wParam:��ϵ�ı�ʾ�š�
lParam:NULL;
return values:
CNT_BELONGCURLIST,��ϵ�뵱ǰ����ϵ���Ͳ��Ҵ���ء�
CNT_ONLYBELONGCURCATE,��ϵ���뵱ǰ����ϵ�����ء�
CNT_NOTBELONGCURLIST,��ϵ���뵱ǰ����ϵ���Ͳ��Ҵ����.
*/
#define CNC_VERIFYCNTBELONGCURLIST 9
/*
Describe:����ǰ��ϵ����ʽ��ѡ�е�������ϵ������Ŀ����ϵ����
wParam:Ŀ����ϵ����ʽ�ķ�������
lParam:NULL;
return values:
CNT_SUCCESS��ʾ�����ɹ���
*/
#define CNC_EXPORTLISTSELCNT 10
/*
Describe:����ǰ��ϵ����ʽ�н�ָ������ϵ��Ŀ���ס�
wParam:ָ������ϵ��ʶ;
lParam:NULL;
*/
#define CNC_SETCNTSEL 11
///////////////////////��CNS_CREATEONE��CNS_MODIFYONE��ص�����
/*
Describe: ����ϵ���½������޸Ĵ����еõ�ָ����Ŀ������
wParam:NULL;
lParam:ָ��PCNT_ITEM;
wID��ϵ����Ŀ��ʶ��
pData������ϵ�����ݵĻ��塣
wDataLen����ĳ��ȡ�
return value:���صõ������ݵĳ���(BYTE����)
*/

#define CNC_GETITEM 15
/*
Describe: ����ϵ���½������޸Ĵ���������ָ����Ŀ������
wParam:NULL;
lParam:ָ��PCNT_ITEM;
wID��ϵ����Ŀ��ʶ��
pData������ϵ�����ݵĻ��塣
wDataLen����ĳ��ȡ�
���ط�0��ʾ�ɹ�������ʧ�ܡ�
*/
#define CNC_SETITEM 16
/*
Describe: ����ϵ���½������޸�ʱ������ϵ�����ݡ�
wParam:NULL;
lParam:ָ�򱣴淵���½����޸ĵ���ϵ����ʾ�ŵĵ�ַ��
����CNT_MODIFYONE��ʾ�޸���һ����ϵ������CNT_NEWCREATEONE������һ����ϵ��
�������ʾʧ�ܡ�
*/
#define CNC_SAVECNT 17
/*
Describe: ����ϵ���½������޸Ĵ����еõ�ѡ����Ŀ������
wParam:NULL;
lParam:ָ��PCNT_ITEM;
wID:��ϵ����Ŀ��ʶ��
pData:������ϵ�����ݵĻ��塣
wDataLen:����ĳ��ȡ�
����TRUE��ʾ�ɹ���FALSE��ʾʧ�ܡ�
*/
#define CNC_GETSELITEM 18
///////////////////
/*
Describe:����ϵ��������һ����ϵ��
wParam:NULL
lParam:ָ��һ��PCNT_RECORD��ָ��;
����CNT_SUCCESS��ʾ�ɹ��������ʾʧ�ܡ�
*/
#define CNC_ADDONE 20
/*
Describe:����ϵ�����޸�һ����ϵ��
wParam:NULL;
lParam:ָ��һ��PCNT_RECORD��ָ��;
*/
#define CNC_MODIFYONE 21
/*
Describe: ɾ����ϵ����ʽ��ָ������ϵ
wParam:��ϵ�ı�ʾ��
lParam:NULL
����CNT_SUCCESS��ʾ�ɹ��������ʾʧ�ܡ�
*/
#define CNC_DELONE 22
/*
Describe:����ϵ���в�����������ϵ
����ָ����Ŀ�е����ݰ���ָ������
wParam:NULL;
lParam:ָ��һ��PCNT_FIND�ṹ, ���ΪwCntIDBufNum = 0�򷵻ز��ҵ��ĸ���;
���lParam��ΪNULLʱ
wID:ָ������ϵ��Ŀ��ʶ�š�
wCntIDBufNum:������ϵID�Ļ������.
pCntIDAry��������ϵID�Ļ�����.
bFindOnlyNotEmpty:���ΪTRUE,�����ʱֻҪwID��ʾ���Ϊ������ϲ���Ҫ������ĳ�Ա��Ч��
dwCateID: Ϊ-1ʱ������������ϵ�в��ң�����ֻ����ָ��������е���ϵ��
pszFindStr: ָ��Ҫ���ҵĴ������Ϊ""��bMatchWhole = FALSE���г�wIDָ�����Ŀ���ȷ������ϵ��������ʱCNT_FIND�ṹ��������Ա����������.
bMatchWhole��ΪTRUE����ҵ�����ϵ��wID��ʾ������pszFindStrָ��Ĵ���ȣ�����ֻҪ��ϵ��wID��ʾ�����а�����pszFindStrָ��Ĵ�����Ϊ�Ƿ��ϲ���Ҫ�����ϵ��
bMatchCase: ΪTRUE����ҹ�����pszFindStrָ��Ĵ���wID��ʾ������Ƚ�ʱ���ִ�Сд���������֡�
return value:���ز��ҵ��ĸ�����
*/
#define CNC_FIND 23
/*
Describe:�õ���ϵ�����������ı�ʶ�š�
wParam:��������ʶ�ŵĻ���ĸ���;
lParam:ָ��һ��DWORD[wParam]������׵�ַ, ���ΪNULL�򷵻����ĸ���;
���صõ�����������
*/
#define CNC_GETALLCATE 24
/*
Describe:�õ���ϵ����ָ����ʾ����ϵ��
wParam:NULL;
lParam:ָ��һ��PCNT_RECORD��ָ��;
����TRUE��ʾ�ɹ���FALSE��ʾʧ�ܡ�
ÿpItemAry���wDatalen���صõ��ĸ���ĳ���(byte)
*/
#define CNC_GETONE 25
/*
Describe:�õ���ϵ����ָ����ʾ����ϵ���
wParam:NULL;
lParam:ָ��PCNT_CATEGORY, ���ΪwCateNum = 0��ֻ���ظ���ϵ���ĸ���,����
������ϵ����������ʾ������PCNT_CATEGORYָ���ID���塣
���صõ��ĸ���ϵ���ĸ���
*/
#define CNC_GETCATE 26
/*
Describe:������ϵ����ʾ�õ���Ӧ��������ơ�
wParam:��ϵ����ʾ
lParam:����������ƵĻ����ַ��
���ط�0��ʾ�ɹ�������ʧ�ܡ�
*/
#define CNC_GETCATENAME 27
/*
Describe:������ϵ���д��ڵ��ı������⡢�ı�����ǰ����ɫ��
wParam:NULL;
lParam:һ��ָ��CTLCOLORSTRUCT�ṹ��ָ�롣
*/
#define CNC_SETCOLOR 28
/*
Describe:��ָ����ϵ���е���ϵ���뵽��ǰ��ϵ����
wParam:ָ����ϵ���ķ�������
lParam:NULL
����CNT_SUCCESS��ʾ�ɹ�����,
���򷵻���Ӧ��ֵ��
*/
#define CNC_IMPORT 29
/*
Describe:�õ���ǰ��ϵ����������Ŀ���Լ���Ŀ�ĳ��ȡ�
wParam:ָ����ITEM_LEN����
lParam:ָ��PITEM_LEN�Ľṹָ�룬��lParamΪNULLʱֻ������Ŀ����
������Ŀ������
*/
#define CNC_GETITEMLEN 30
/*
Describe:�õ���ָ����ϵ�����뵽��ǰ��ϵ��ʱ���ܻᶪʧ����ϵ��.
wParam:ָ����ϵ���ķ�����.
lParam:ָ�򱣴���ܻᶪʧ����ϵ��ı���Ļ��塣�����ֵΪ����ֻ���ؿ��ܻᶪʧ����ϵ�����;
���ؿ��ܻᶪʧ����ϵ�����;
*/
#define CNC_GETIMPORTLOSEINFO 31

/*
Describe:����ϵ�������һ�����
wParam:ָ�򱣴�����ʶ�ĵ�ַ
lParam:null-terminated string of �������;
����
CNT_CATEEXIST ��ʾ������Ѵ���
CNT_CATENUM_BEYOND ��ʾ�������ѳ���������
CNT_NOENOUGH_SPACE ��ʾ��洢�ռ䲻��
CNT_NEWCATE ��ʾ�����һ������
;
*/
#define CNC_ADDCATE 32

/*
Describe:����ϵ����ɾ��һ�����
wParam:Ҫɾ��������ʶ��
lParam:NULL
����TRUE��ʾ�ɹ��������ʾʧ��
;
*/
#define CNC_DELCATE 33

/*
Describe:����ϵ����ʽ�еõ���ǰѡ����Ŀ�������ţ�����ж��ѡ��ֻ������С��������
wParam:NULL
lParam:NULL
���������ţ�����-1��ʾʧ��
;
*/
#define CNC_GETSELINDEX 34
/*
Describe:�õ�����ϵ����ʽ��һ���ж��ٸ���Ŀ
wParam:NULL
lParam:NULL
������ϵ����ʽ��һ���ж��ٸ���Ŀ������-1��ʾʧ��
;
*/
#define CNC_GETLISTITEMCOUNT 35

/*
Describe:���ݵ绰����õ��õ绰�����Ӧ����ϵ����Ϣ
wParam:NULL
lParam:PTHIRDPART_FIND�ṹָ��
�ɹ�����CNT_SUCCESS,����ʧ��
*/
#define CNC_GETINFOFROMPHONE 36

/*
Describe:����ϵ������Ŀ���̶���ָ������������ÿ����¼�ȳ���
wParam:����
lParam:NULL
�ɹ�����CNT_SUCCESS,����ʧ��
*/
#define CNC_FIXCNTCOUNT 37

/*
Describe:�õ���ǰ��ϵ���е���ϵ��
wParam:NULL
lParam:NULL
������ϵ���е���ϵ��
*/
#define CNC_GETCNTCOUNT 38

/*
Describe:�õ���ǰ��ϵ����ʽ��ָ����������ϵ
wParam:��ʽ��Ŀ������
lParam:ָ��PCNT_RECORD
������ϵ���е���ϵ��
*/
#define CNC_GETONEFROMLISTINDEX 39
/*
Describe:����ǰ��ϵ����ʽ��ָ����������ϵ����
wParam:��ʽ��Ŀ������
lParam:NULL
�ɹ�����CNT_SUCCESS,����ʧ��
*/
#define CNC_SETSELFROMLISTINDEX 40
/*
Describe:�õ���ǰ��ϵ����ʽ�Ĵ��ھ��
wParam:NULL
lParam:NULL
������ϵ����ʽ�Ĵ��ھ��
*/
#define CNC_GETLISTHANDLE 41

/*
Describe:�õ��뵱ǰ��ϵ��������sim���ĵ�һ����Чλ��
wParam:NULL
lParam:NULL
�����뵱ǰ��ϵ��������sim���ĵ�һ����Чλ��, �������ֵΪ-1˵��sim������
*/

#define CNC_GETFIRSTVALIDSIMPOS 42

/*
Describe:�õ��뵱ǰ��ϵ��������sim�������洢����(����);
wParam:NULL
lParam:NULL
�����뵱ǰ��ϵ��������sim�������洢����(����);
*/
#define CNC_GETSIMMAXITEMNUM 44
/*
Describe:�õ��뵱ǰ��ϵ��������sim������������;
wParam:NULL
lParam:NULL
�����뵱ǰ��ϵ��������sim������������;
*/
#define CNC_GETSIMUSEDITEMNUM 45

/*
Describe:ˢ���뵱ǰ��ϵ�����������ϵ��
wParam:NULL
lParam:NULL
�ɹ�����CNT_SUCCESS��
*/
#define CNC_FRESHTABLE 46

/*
Describe:��SIM���ϵĵ绰�����뵽��ϵ���С�
wParam:NULL
lParam:NULL
�ɹ�����TRUE�����򷵻�FALSE��
*/
#define CNC_LOADALLSIMITEM 47
typedef struct _ITEM_EXID{
	WORD wExIDNum;
	WORD pExIDAry[1];
}ITEM_EXID, *PITEM_EXID;

typedef struct _CNT_COL{
	TCHAR *pszColTitle;
	PITEM_EXID pItemExID;
}CNT_COL, *PCNT_COL;

typedef struct _CNT_LISTSTRUCT{
	WORD wColNum;//�����Ϊ-1���򴴽�����ϵ����ʽΪȱʡ����ʾ�����͵绰������,pCntColAry��Ч��
	WORD wReserve;
	PCNT_COL pCntColAry;
}CNT_LISTSTRUCT, *PCNT_LISTSTRUCT;
//////////////
typedef struct _CNT_CREATESTRUCT{
	HINSTANCE hInstance;
	HWND hParent;
	WORD wViewType;
	WORD wOpMode;
	TCHAR *pszCntFile;//���Ϊ�գ��򴴽���ϵ������ʱʹ��ϵͳ��ϵ���ļ�������ʹ��ָ������ϵ���ļ���
	TCHAR *pszWndCaption;//
	int x;
	int y;
	int width;
	int height;
	HICON hIcon;
	DWORD dwExStyle;
	DWORD dwStyle;
	DWORD dwCtrlID;//if dwStyle is WS_CHILD this value must be passed;
	LPVOID *lpTypeData;
	LPCTLCOLORSTRUCT lpCntColorStruct;
}CNT_CREATESTRUCT, *PCNT_CREATESTRUCT;
////////////////////////////

HANDLE Cnt_CreateService(
	PCNT_CREATESTRUCT pCntCreateStruct
	);
void Cnt_DestroyService(
	HANDLE hCnt
);


#ifdef _cpluscplus
}

#endif
#endif
