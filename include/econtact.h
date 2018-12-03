/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
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
	{TEXT("姓名:"), NAME_LEN, ID_NAME},\
	{TEXT("图像文件:"), IMAGE_FILE_LEN, ID_IMAGE_FILE},\
	{TEXT("手机:"), PHONENUMBER_LEN, ID_MOBILE},\
	{TEXT("商务:"), PHONENUMBER_LEN, ID_OPHONE},\
	{TEXT("住宅:"), PHONENUMBER_LEN, ID_HPHONE},\
	{TEXT("传真:"), PHONENUMBER_LEN, ID_OFAX},\
	{TEXT("预置:"), PRAMIRY_CONTACT_LEN, ID_PRIMARY_CONTACT},\
	{TEXT("类别:"), CATEGORYITEM_LEN, ID_CATEGORY},\
	{TEXT("公司:"), COMPANY_LEN, ID_COMPANY},\
	{TEXT("职位:"), POSITION_LEN, ID_POSITION},\
	{TEXT("地址:"), ADDRESS_LEN, ID_HADDRESS},\
	{TEXT("铃声:"), RING_FILE_LEN, ID_RING_FILE},\
	{TEXT("Email:"), EMAIL_LEN, ID_EMAIL},\
	{TEXT("生日:"), BIRTHDAY_LEN, ID_BIRTHDAY},\
	{TEXT("备注:"), NOTE_LEN, ID_NOTE}\
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
	{TEXT("姓名:"), NAME_LEN, ID_NAME},\
	{TEXT("手机:"), PHONENUMBER_LEN, ID_MOBILE},\
	{TEXT("电话(O):"), PHONENUMBER_LEN, ID_OPHONE},\
	{TEXT("传真(O):"), PHONENUMBER_LEN, ID_OFAX},\
	{TEXT("电话(H):"), PHONENUMBER_LEN, ID_HPHONE},\
	{TEXT("传真(H):"), PHONENUMBER_LEN, ID_HFAX},\
	{TEXT("E-mail:"), EMAIL_LEN, ID_EMAIL},\
	{TEXT("QQ:"), QQ_LEN, ID_QQ},\
	{TEXT("主页:"), HOMEPAGE_LEN, ID_HOMEPAGE},\
	{TEXT("地址(O):"), ADDRESS_LEN, ID_OADDRESS},\
	{TEXT("地址(H):"), ADDRESS_LEN, ID_HADDRESS},\
	{TEXT("邮编(O):"), POSTCODE_LEN, ID_OPCODE},\
	{TEXT("邮编(H):"), POSTCODE_LEN, ID_HPCODE},\
	{TEXT("类别:"), CATEGORYITEM_LEN, ID_CATEGORY},\
	{TEXT("备注:"), NOTE_LEN, ID_NOTE}\
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
/////联系服务操作方式，读或写的组合。
#define CNT_READ 0x1
#define CNT_WRITE 0x2
#define CNT_IMPORT 0x4
#define CNT_EXPORT 0x8
////////////////////////
/////////////联系服务类型
#define CNS_CREATEONE 1//新建联系
#define CNS_CREATELIST 2//创建联系本列式
#define CNS_PUREDATA 3//创建不包含窗口的联系本服务
#define CNS_VIEWONE 4//浏览一个联系
#define CNS_MODIFYONE 5//修改一个联系。
///////////////////////////////

////////////////////////通知消息类型
#define CNN_STORE (WM_USER + 1) //当用CNS_CREATEONE, CNS_MODIFYONE创建联系本服务后，
	//当对某个联系进行修改或添加一条新的联系时，会向创建它时传递的父窗口句柄发该通知消息。
#define CNN_ACTIVATEITEM (WM_USER + 2)//当用CNS_CREATELIST创建联系本服务后，
//当激活列式中的一个条目时，会向创建它时传递的父窗口句柄发该通知消息。
#define CNN_SELITEM (WM_USER + 3)//当用CNS_VIEWONE, CNS_MODIFYONE, CNS_CREATEONE创建联系本服务后，
								//当选中一个条目时，会向创建它时传递的父窗口句柄发该通知消息。

#define CNN_CLICK (WM_USER + 4)//当用CNS_CREATELIST创建联系本服务后
							  //当点击列式中的一个条目时，会向创建它时传递的父窗口句柄发该通知消息。
#define CNN_ITEMCHANGED (WM_USER + 5)//当用CNS_CREATELIST创建联系本服务后
									//当点击列式中的一个条目有变化时，会向创建它时传递的父窗口句柄发该通知消息。
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
	BOOL bNewCnt;//如果为TRUE表式新建了一个联系，否则表式修改了一个联系。
	DWORD dwCntID;//如果为TRUE表式新建或修改的联系的表识号。
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
	TCHAR *pszPhonNumber;//根据电话号码查找对应的人名，图片，铃声
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
	BOOL bMatchWhole;//是不是完全匹配
	BOOL bMatchCase;//是否区分大小写	
	DWORD pCntIDAry[1];
}CNT_FIND, *PCNT_FIND;

LRESULT 
Cnt_CallServices(
	HANDLE hCnt,
	DWORD dwCntCmd,
	WPARAM wParam,
	LPARAM lParam
);
/////////////与CNS_CREATELIST相关的命令
/*
Describe: 得到联系本列式中当前选中的所有联系的标识(ID)
wParam:缓冲的个数,当lParam为NULL时无效。
lParam:如果为NULL, 则Cnt_CallServices()返回选中的个数，否则指向一个缓冲的首地址,用来保存选中的联系的ID
return value:返回得到的个数.
*/
#define CNC_GETSELID 2
/*在联系本中查找这样的联系
Describe: 在联系本列式中查找查找这样的联系
它的指定条目中的内容包含指定串。
wParam:NULL;
lParam:指向PCNT_FIND结构指针。
wID:指定的联系条目标识号。
bFindOnlyNotEmpty:如果为TRUE,则查找时只要wID表示的项不为空则符合查找要求，下面的成员无效。
dwCateID: 为-1时在所有类别的联系中查找，否则只查找指定的类别中的联系。
pszFindStr: 指向要查找的串。如果为""且bMatchWhole = FALSE则列出wID指向的条目长度非零的联系，并且这时CNT_FIND结构的其它成员均视无意义.
bMatchWhole：为TRUE则查找到的联系的wID表示的项与pszFindStr指向的串相等，否则只要联系的wID表示的项中包含有pszFindStr指向的串便认为是符合查找要求的联系。
bMatchCase: 为TRUE则查找过程中pszFindStr指向的串与wID表示的项相比较时区分大小写，否则不区分。
return value:CNT_SUCESS标示成功，否则失败。
*/
#define CNC_LISTFIND 3
/*
Describe: 删除联系本列式中选中的联系。
wParam:NULL;
lParam:NULL;
*/
#define CNC_DELSELCNT 4
/*
Describe: 刷新联系本列式
wParam:NULL;
lParam:NULL;
返回CNT_SUCCESS表成功，否则失败
*/
#define CNC_FRESHLIST 6
/*
Describe: 向联系本列式中添加一条联系。
wParam:TRUE表示新增一条，FALSE表示修改一条;
lParam:联系的标示号;
返回CNT_SUCCESS表成功，否则失败
*/
#define CNC_ADDITEMTOLIST 7
/*
Describe: 设置联系本列式的列宽。
wParam:第几列
lParam:宽度。
*/
#define CNC_SETCOLWIDTH 8
/*
Describe:判断指定的联系是否属于联系本列式，
因为向联系本列式服务句柄发送CNC_LISTFIND命令后当前联系本中所显示的是与最后一次发送CNC_LISTFIND命令
时指定的联系类别和查找串相关的联系，发送CNC_VERIFYCNTBELONGCURLIST命令可以诊断指定的联系是否与当前
的联系类别和查找串相关。
wParam:联系的标示号。
lParam:NULL;
return values:
CNT_BELONGCURLIST,联系与当前的联系类别和查找串相关。
CNT_ONLYBELONGCURCATE,联系仅与当前的联系类别相关。
CNT_NOTBELONGCURLIST,联系不与当前的联系类别和查找串相关.
*/
#define CNC_VERIFYCNTBELONGCURLIST 9
/*
Describe:将当前联系本列式中选中的所有联系导出到目标联系本。
wParam:目标联系本列式的服务句柄。
lParam:NULL;
return values:
CNT_SUCCESS表示导出成功。
*/
#define CNC_EXPORTLISTSELCNT 10
/*
Describe:将当前联系本列式中将指定的联系条目反白。
wParam:指定的联系标识;
lParam:NULL;
*/
#define CNC_SETCNTSEL 11
///////////////////////与CNS_CREATEONE、CNS_MODIFYONE相关的命令
/*
Describe: 在联系本新建，或修改窗口中得到指定条目的内容
wParam:NULL;
lParam:指向PCNT_ITEM;
wID联系的条目标识；
pData保存联系的内容的缓冲。
wDataLen缓冲的长度。
return value:返回得到的内容的长度(BYTE个数)
*/

#define CNC_GETITEM 15
/*
Describe: 在联系本新建，或修改窗口中设置指定条目的内容
wParam:NULL;
lParam:指向PCNT_ITEM;
wID联系的条目标识；
pData保存联系的内容的缓冲。
wDataLen缓冲的长度。
返回非0表示成功，否则失败。
*/
#define CNC_SETITEM 16
/*
Describe: 在联系本新建，或修改时保存联系的内容。
wParam:NULL;
lParam:指向保存返回新建或修改的联系本标示号的地址。
返回CNT_MODIFYONE表示修改了一个联系，返回CNT_NEWCREATEONE新增了一个联系。
其它则表示失败。
*/
#define CNC_SAVECNT 17
/*
Describe: 在联系本新建，或修改窗口中得到选中条目的内容
wParam:NULL;
lParam:指向PCNT_ITEM;
wID:联系的条目标识；
pData:保存联系的内容的缓冲。
wDataLen:缓冲的长度。
返回TRUE表示成功，FALSE表示失败。
*/
#define CNC_GETSELITEM 18
///////////////////
/*
Describe:向联系本中增加一条联系。
wParam:NULL
lParam:指向一个PCNT_RECORD的指针;
返回CNT_SUCCESS表示成功，否则表示失败。
*/
#define CNC_ADDONE 20
/*
Describe:向联系本中修改一条联系。
wParam:NULL;
lParam:指向一个PCNT_RECORD的指针;
*/
#define CNC_MODIFYONE 21
/*
Describe: 删除联系本列式中指定的联系
wParam:联系的标示号
lParam:NULL
返回CNT_SUCCESS表示成功，否则表示失败。
*/
#define CNC_DELONE 22
/*
Describe:在联系本中查找这样的联系
它的指定条目中的内容包含指定串。
wParam:NULL;
lParam:指向一个PCNT_FIND结构, 如果为wCntIDBufNum = 0则返回查找到的个数;
如果lParam不为NULL时
wID:指定的联系条目标识号。
wCntIDBufNum:保存联系ID的缓冲个数.
pCntIDAry：保存联系ID的缓冲区.
bFindOnlyNotEmpty:如果为TRUE,则查找时只要wID表示的项不为空则符合查找要求，下面的成员无效。
dwCateID: 为-1时在所有类别的联系中查找，否则只查找指定的类别中的联系。
pszFindStr: 指向要查找的串。如果为""且bMatchWhole = FALSE则列出wID指向的条目长度非零的联系，并且这时CNT_FIND结构的其它成员均视无意义.
bMatchWhole：为TRUE则查找到的联系的wID表示的项与pszFindStr指向的串相等，否则只要联系的wID表示的项中包含有pszFindStr指向的串便认为是符合查找要求的联系。
bMatchCase: 为TRUE则查找过程中pszFindStr指向的串与wID表示的项相比较时区分大小写，否则不区分。
return value:返回查找到的个数。
*/
#define CNC_FIND 23
/*
Describe:得到联系本的所有类别的标识号。
wParam:保存类别标识号的缓冲的个数;
lParam:指向一个DWORD[wParam]数组的首地址, 如果为NULL则返回类别的个数;
返回得到的类别个数。
*/
#define CNC_GETALLCATE 24
/*
Describe:得到联系本中指定标示的联系。
wParam:NULL;
lParam:指向一个PCNT_RECORD的指针;
返回TRUE表示成功，FALSE表示失败。
每pItemAry项的wDatalen返回得到的各项的长度(byte)
*/
#define CNC_GETONE 25
/*
Describe:得到联系本中指定标示的联系类别。
wParam:NULL;
lParam:指向PCNT_CATEGORY, 如果为wCateNum = 0则只返回该联系类别的个数,否则
将该联系的所有类别标示号填如PCNT_CATEGORY指向的ID缓冲。
返回得到的该联系类别的个数
*/
#define CNC_GETCATE 26
/*
Describe:根据联系类别标示得到相应的类别名称。
wParam:联系类别标示
lParam:保存类别名称的缓冲地址。
返回非0表示成功，否则失败。
*/
#define CNC_GETCATENAME 27
/*
Describe:设置联系本中窗口的文本、标题、的背景和前景颜色。
wParam:NULL;
lParam:一个指向CTLCOLORSTRUCT结构的指针。
*/
#define CNC_SETCOLOR 28
/*
Describe:将指定联系本中的联系导入到当前联系本。
wParam:指定联系本的服务句柄。
lParam:NULL
返回CNT_SUCCESS表示成功导入,
否则返回相应的值。
*/
#define CNC_IMPORT 29
/*
Describe:得到当前联系本的所有条目，以及条目的长度。
wParam:指定的ITEM_LEN个数
lParam:指向PITEM_LEN的结构指针，当lParam为NULL时只返回条目个数
返回条目个数；
*/
#define CNC_GETITEMLEN 30
/*
Describe:得到将指定联系本导入到当前联系本时可能会丢失的联系项.
wParam:指定联系本的服务句柄.
lParam:指向保存可能会丢失的联系项的标题的缓冲。如果该值为空则只返回可能会丢失的联系项个数;
返回可能会丢失的联系项个数;
*/
#define CNC_GETIMPORTLOSEINFO 31

/*
Describe:向联系本中添加一个类别
wParam:指向保存类别标识的地址
lParam:null-terminated string of 类别名称;
返回
CNT_CATEEXIST 表示类别名已存在
CNT_CATENUM_BEYOND 表示类别个数已超过最大个数
CNT_NOENOUGH_SPACE 表示类存储空间不足
CNT_NEWCATE 表示添加了一个新类
;
*/
#define CNC_ADDCATE 32

/*
Describe:在联系本中删除一个类别
wParam:要删除的类别标识号
lParam:NULL
返回TRUE表示成功，否则表示失败
;
*/
#define CNC_DELCATE 33

/*
Describe:在联系本列式中得到当前选中条目的索引号，如果有多个选中只返回最小的索引号
wParam:NULL
lParam:NULL
返回索引号，返回-1表示失败
;
*/
#define CNC_GETSELINDEX 34
/*
Describe:得到在联系本列式中一共有多少个条目
wParam:NULL
lParam:NULL
返回联系本列式中一共有多少个条目，返回-1表示失败
;
*/
#define CNC_GETLISTITEMCOUNT 35

/*
Describe:根据电话号码得到该电话号码对应的联系人信息
wParam:NULL
lParam:PTHIRDPART_FIND结构指针
成功返回CNT_SUCCESS,否则失败
*/
#define CNC_GETINFOFROMPHONE 36

/*
Describe:将联系本的条目数固定成指定的条数，且每条记录等长。
wParam:条数
lParam:NULL
成功返回CNT_SUCCESS,否则失败
*/
#define CNC_FIXCNTCOUNT 37

/*
Describe:得到当前联系本中的联系数
wParam:NULL
lParam:NULL
返回联系本中的联系数
*/
#define CNC_GETCNTCOUNT 38

/*
Describe:得到当前联系本列式中指定索引的联系
wParam:列式条目索引号
lParam:指向PCNT_RECORD
返回联系本中的联系数
*/
#define CNC_GETONEFROMLISTINDEX 39
/*
Describe:将当前联系本列式中指定索引的联系反白
wParam:列式条目索引号
lParam:NULL
成功返回CNT_SUCCESS,否则失败
*/
#define CNC_SETSELFROMLISTINDEX 40
/*
Describe:得到当前联系本列式的窗口句柄
wParam:NULL
lParam:NULL
返回联系本列式的窗口句柄
*/
#define CNC_GETLISTHANDLE 41

/*
Describe:得到与当前联系本关联的sim卡的第一个有效位置
wParam:NULL
lParam:NULL
返回与当前联系本关联的sim卡的第一个有效位置, 如果返回值为-1说明sim卡以满
*/

#define CNC_GETFIRSTVALIDSIMPOS 42

/*
Describe:得到与当前联系本关联的sim卡的最大存储容量(条数);
wParam:NULL
lParam:NULL
返回与当前联系本关联的sim卡的最大存储容量(条数);
*/
#define CNC_GETSIMMAXITEMNUM 44
/*
Describe:得到与当前联系本关联的sim卡的已用条数;
wParam:NULL
lParam:NULL
返回与当前联系本关联的sim卡的已用条数;
*/
#define CNC_GETSIMUSEDITEMNUM 45

/*
Describe:刷新与当前联系服务关联的联系表
wParam:NULL
lParam:NULL
成功返回CNT_SUCCESS。
*/
#define CNC_FRESHTABLE 46

/*
Describe:将SIM卡上的电话本加入到联系本中。
wParam:NULL
lParam:NULL
成功返回TRUE，否则返回FALSE。
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
	WORD wColNum;//如果是为-1，则创建的联系本列式为缺省的显示姓名和电话的两列,pCntColAry无效。
	WORD wReserve;
	PCNT_COL pCntColAry;
}CNT_LISTSTRUCT, *PCNT_LISTSTRUCT;
//////////////
typedef struct _CNT_CREATESTRUCT{
	HINSTANCE hInstance;
	HWND hParent;
	WORD wViewType;
	WORD wOpMode;
	TCHAR *pszCntFile;//如果为空，则创建联系本服务时使用系统联系本文件，否则使用指定的联系本文件。
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
