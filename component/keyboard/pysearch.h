/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __PYSEARCH_H
#define __PYSEARCH_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus 


typedef struct KeyValue{
	BYTE bChar; // 键的值
	BYTE bVk;  // 键的虚键值
	WORD wSBCCase; // 全角输出
}KEYVALUE;

typedef struct {
    KEYVALUE  keyValue[6]; // 存放键值的数组
	int		iKeyNum;  // 键值的个数
    int     iLeft;  // 开始位置
	int     iWidth; // 键的宽度
} KEYENTRY;


// 定义一个当前拼音结果的结构
#define MAX_SPELLNUM  8
#define SPELLLISTNUM  10  // 在系统下输入的字符最多产生的拼音组合不会超过10个
#define MAX_PYRESULT 256  // 在我们的系统下，一个拼音最多的拼写结果不会超过256

typedef struct SpellResultStruct{
	// 以下内容为选择了一个拼写后有效
	int iSelectIndex; // 当前选择的拼音组合的索引
	WORD lpSpellResult[MAX_PYRESULT]; // 存放拼音结果
	int iSpellResultNum;  // 拼写结果数目
	int iCurPage ;  // 当前的显示在屏幕的内容的页索引
}SPELLRESULT, *LPSPELLRESULT;

// 定义一个拼音拼写结构
typedef struct SpellListStruct{
	KEYENTRY *pKey[MAX_SPELLNUM] ;  // 当前用户已经输入的键值
	int iKeyNum; // 当前已经输入的字符数
	TCHAR lpSpellList[SPELLLISTNUM][MAX_SPELLNUM];  // 最大的拼音长度为6个（chuang,zhuang ...）
	int iSpellNum ;  // 当前一共有多少种拼音组合
}SPELLLIST,*LPSPELLLIST;

BOOL InsertKeyToSpell(LPSPELLLIST lpSpellList,KEYENTRY *pKey); // 插入一个键到拼写结果中
int PYSpell(LPCTSTR lpSpell,WORD *lpSpellResult,int iMaxLenght); // 拼写单词
BOOL ReSpellList(LPSPELLLIST lpSpellList);

extern const TCHAR classSpellArea[];
ATOM RegisterSpellAreaWindow(HINSTANCE hInstance); // 注册手写区窗口类


// 拼写窗口
#define SPM_INSERTKEY   (WM_USER + 2100)  // 插入一个键到拼写窗口
#define SPM_GETSELSPELL (WM_USER + 2101)  // 得到当前的选择拼写
#define SPM_CLEARSPELL  (WM_USER + 2102)  // 清除所有的拼写
#define SPM_DELETEKEY   (WM_USER + 2103)  // 删除一个键
#define SPM_GETKEYNUM   (WM_USER + 2104)  // 得到当前拼写区域中键的数目

// 拼写窗口通知消息，通过Message 直接发送
#define SPN_SELCHANGE	(WM_USER + 2200)  // 拼写窗口的选择发生改变
#define SPN_NODATA		(WM_USER + 2201)  // 拼写窗口已经没有数据


#ifdef __cplusplus
}           
#endif  // __cplusplus
#endif  //__PYSEARCH_H
