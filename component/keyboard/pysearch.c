/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：拼音键盘实现
版本号：1.0.0.456
开发时期：2002-03-17
作者：陈建明 Jami chen
修改记录：
**************************************************/
#include <eframe.h>
#include "resource.h"
#include "lxsearch.h"
#include "pysearch.h"


// **************************************************
// 定义区域
// **************************************************
#define SHENGMUNUM 24

const TCHAR *g_ShengMuTab[]={//"BCDFGHJKLMNPQRSTWXYZ";
							"",
							"b",
							"c",
							"d",
							"f",
							"g",
							"h",
							"j",
							"k",
							"l",
							"m",
							"n",
							"p",
							"q",
							"r",
							"s",
							"t",
							"w",
							"x",
							"y",
							"z",
							"ch",
							"sh",
							"zh",
							};


#define YUNMUNUM 37
const TCHAR *g_YunMuTab[]={
							  "",
							  "a",  
							  "ai",  
							  "an",  
							  "ang",
							  "ao",  
							  "e",   
							  "ei",
							  "en",
                              "eng",
							  "er",  
							  "i",   
							  "ia", 
							  "ian", 
							  "iang",
							  "iao",
							  "ie",
                              "in", 
							  "ing", 
							  "io",
							  "ion",
							  "iong",
							  "iu", 
							  "o",   
							  "on", 
							  "ong", 
							  "ou", 
							  "u",
                              "v",  
							  "ua",  
							  "uai", 
							  "uan",
							  "uang",
							  "ue",  
							  "ui", 
							  "un",
                              "uo"
                            };


extern const unsigned short PYTab_Index[];
extern const unsigned short PYTab_Data[];

// *******************************************************
// 函数声名区
// *******************************************************
static BOOL PYSpellValid(LPCTSTR lpSpell);
static int GetShengMuIndex(LPCTSTR lpCode);
static int GetYunMuIndex(LPCTSTR lpCode);


// **************************************************
// 声明：BOOL InsertKeyToSpell(LPSPELLLIST lpSpellList,KEYENTRY *pKey)
// 参数：
// 	IN lpSpellList -- 当前的拼写结果
//  IN pKey --要插入的键
// 
// 返回值：成功插入返回TRUE， 否则返回FALSE
// 功能描述：插入一个键到拼写结果中。
// 引用: 
// **************************************************
BOOL InsertKeyToSpell(LPSPELLLIST lpSpellList,KEYENTRY *pKey)
{
	TCHAR lpSpellSave[SPELLLISTNUM][8];  // 最大的拼音长度为6个（chuang,zhuang ...）
	int i,j;
	TCHAR lpCurSpell[8];
	int iNewSpell = 0;

//		RETAILMSG(1,(TEXT(" InsertKeyToSpell ...\r\n")));
		// 检查当前的拼写加上当前键的字符后的拼写组合

		if (lpSpellList->iSpellNum == 0)
		{
//			RETAILMSG(1,(TEXT(" 1\r\n")));
			// 当前的拼写中没有数据，不需要进行拼写测试，直接添加
			for (j = 0; j < pKey->iKeyNum; j++)
			{
//				RETAILMSG(1,(TEXT(" 2\r\n")));
				lpCurSpell[0] = pKey->keyValue[j].bChar;  // 加上当前的字符
				lpCurSpell[1] = 0;
				if (PYSpellValid(lpCurSpell)) // 检查拼写是否有效
				{  // 拼写有效
					strcpy(lpSpellSave[iNewSpell],lpCurSpell); // 将当前拼写记录到缓存
					iNewSpell ++;  // 结果增加1
				}
			}
		}
		else
		{
//			RETAILMSG(1,(TEXT(" 3\r\n")));
			for (i = 0; i < lpSpellList->iSpellNum; i++) 
			{
//				RETAILMSG(1,(TEXT(" 4\r\n")));
				strcpy(lpCurSpell,lpSpellList->lpSpellList[i]); // 得到原来的拼写
				for (j = 0; j < pKey->iKeyNum; j++)
				{
//					RETAILMSG(1,(TEXT(" 5\r\n")));
					lpCurSpell[lpSpellList->iKeyNum] = pKey->keyValue[j].bChar;  // 加上当前的字符
					lpCurSpell[lpSpellList->iKeyNum + 1] = 0;
					if (PYSpellValid(lpCurSpell)) // 检查拼写是否有效
					{  // 拼写有效
						strcpy(lpSpellSave[iNewSpell],lpCurSpell); // 将当前拼写记录到缓存
						iNewSpell ++;  // 结果增加1
					}
				}

			}
		}
		if (iNewSpell)
		{  // 存在有效的拼写
			// 将新的拼写复制到结果中
//			RETAILMSG(1,(TEXT(" 6\r\n")));
			for (i = 0; i < iNewSpell ; i++) 
			{
				strcpy(lpSpellList->lpSpellList[i],lpSpellSave[i]);
			}
			lpSpellList->iSpellNum = iNewSpell; // 记录当前的拼写数目
			lpSpellList->pKey[lpSpellList->iKeyNum] = pKey; // 将当前的键插入到键的队列中
			lpSpellList->iKeyNum ++; // 增加一个键
//			RETAILMSG(1,(TEXT(" Cur Valid Spell Num = %d\r\n"),lpSpellList->iSpellNum));
			return TRUE; // 已经插入当前键
		}
//		RETAILMSG(1,(TEXT(" InsertKeyToSpell failure\r\n")));
		return FALSE; // 不能插入当前键
}



// **************************************************
// 声明： int PYSpell(LPCTSTR lpSpell,WORD *lpSpellResult,int iMaxLenght)
// 参数：
// 	IN lpSpell -- 要进行拼写的单词
//  OUT lpSpellResult -- 存放拼写结果
//	IN iMaxLenght -- 存放拼写结果的缓存可以存放多少个
//
// 返回值：返回拼写得到结果数目
// 功能描述：拼写单词。
// 引用: 
// **************************************************
int PYSpell(LPCTSTR lpSpell,WORD *lpSpellResult,int iMaxLenght)
{
	int iShengMuIndex,iYunMuIndex;
	int iDataIndex,iDataPosition,iNextPosition;
	LPCTSTR lpCode;
	int iResultNum,i;

		lpCode = lpSpell;  // 得到拼音
		iShengMuIndex = GetShengMuIndex(lpCode); // 得到声母索引
		lpCode += strlen(g_ShengMuTab[iShengMuIndex]); // 调到韵母的位置
		iYunMuIndex = GetYunMuIndex(lpCode); // 得到韵母的位置
		if (iYunMuIndex == -1)
		{
			//不能找到对应的拼音
			return 0;
		}

		if (iShengMuIndex == 0 && iYunMuIndex == 0)
		{
			//不能找到对应的拼音
			return 0;
		}
		iDataIndex = iShengMuIndex * YUNMUNUM +iYunMuIndex -1 ; // 得到数据索引
		iDataPosition = PYTab_Index[iDataIndex]; // 得到数据的位置偏移
		if (iDataPosition == 0xffff)
		{
			//不能找到对应的汉字
			return 0;
		}

		iDataIndex ++; // 得到下一个数据索引
		while(PYTab_Index[iDataIndex] == 0xFFFF) iDataIndex ++; // 找到下一个有效的数据索引
		iNextPosition = PYTab_Index[iDataIndex]; // 得到下一个数据的位置偏移
		iResultNum = iNextPosition - iDataPosition; // 得到当前的数据个数
	
		if (iResultNum > iMaxLenght)
		{
			// 提供的内存不足
			RETAILMSG(1,(TEXT("!!!!!!!! Is Enough Result Buffer [%d]\r\n"),iResultNum));
			iResultNum = iMaxLenght;
		}

		for (i = 0; i < iResultNum ; i++)
		{ // 复制拼音结果到结果缓存中
			lpSpellResult[i] = PYTab_Data[iDataPosition + i];
		}
		// 指定的单词拼写有效，有对应的汉字
		return iResultNum;  
}

// **************************************************
// 声明：static int GetShengMuIndex(LPCTSTR lpCode)
// 参数：
// 	IN lpCode -- 指定拼音
// 
// 返回值：返回拼音声母的索引
// 功能描述：得到拼音声母的索引。
// 引用: 
// **************************************************
static int GetShengMuIndex(LPCTSTR lpCode)
{
	int i ;

		for (i=SHENGMUNUM-1;i>0;i--)
		{ // 查询拼音声母表
			if ( strncmp(lpCode,g_ShengMuTab[i],strlen(g_ShengMuTab[i])) == 0 )
				return i; // 找到声母，返回该声母的索引
		}
		return 0;
}
// **************************************************
// 声明：static int GetYunMuIndex(LPCTSTR lpCode)
// 参数：
// 	IN lpCode -- 拼音韵母代码
// 
// 返回值：返回拼音韵母的索引
// 功能描述：得到拼音韵母的索引。
// 引用: 
// **************************************************
static int GetYunMuIndex(LPCTSTR lpCode)
{
	int i ;

		for (i=0;i<YUNMUNUM;i++)
		{ // 查询拼音韵母表
			if ( strcmp(lpCode,g_YunMuTab[i]) == 0 )
				return i;// 找到韵母，返回该韵母的索引
		}
		return -1;
}


// **************************************************
// 声明：static BOOL PYSpellValid(LPCTSTR lpSpell)
// 参数：
// 	IN lpSpell -- 要进行拼写测试的单词
//
// 返回值：拼写有效返回TRUE，否则返回FALSE
// 功能描述：将指定单词进行拼写测试。
// 引用: 
// **************************************************
static BOOL PYSpellValid(LPCTSTR lpSpell)
{
	int iShengMuIndex,iYunMuIndex;
	int iDataIndex,iDataPosition;
	LPCTSTR lpCode;

		lpCode = lpSpell;  // 得到拼音
		iShengMuIndex = GetShengMuIndex(lpCode); // 得到声母索引
		lpCode += strlen(g_ShengMuTab[iShengMuIndex]); // 调到韵母的位置
		iYunMuIndex = GetYunMuIndex(lpCode); // 得到韵母的位置
		if (iYunMuIndex == -1)
		{
			//不能找到对应的拼音
			return FALSE;
		}

		if (iShengMuIndex == 0 && iYunMuIndex == 0)
		{
			//不能找到对应的拼音
			return FALSE;
		}
		iDataIndex = iShengMuIndex * YUNMUNUM +iYunMuIndex -1 ; // 得到数据索引
		iDataPosition = PYTab_Index[iDataIndex]; // 得到数据的位置偏移
		if (iDataPosition == 0xffff)
		{
			//不能找到对应的汉字
			return FALSE;
		}

		// 指定的单词拼写有效，有对应的汉字
		return TRUE;  
}




// **************************************************
// 声明：BOOL ReSpellList(LPSPELLLIST lpSpellList)
// 参数：
// 	IN lpSpellList -- 当前的拼写结果
// 
// 返回值：成功插入返回TRUE， 否则返回FALSE
// 功能描述：重新得到当前的拼写列表。
// 引用: 
// **************************************************
BOOL ReSpellList(LPSPELLLIST lpSpellList)
{
	KEYENTRY *pKeyList[MAX_SPELLNUM] ;  // 保留当前用户已经输入的键值
	int iCurKeyNum,i;

		iCurKeyNum = lpSpellList->iKeyNum; // 得到当前的键的数目
		for (i = 0; i < lpSpellList->iKeyNum ; i++)
		{
			pKeyList[i] = lpSpellList->pKey[i]; // 暂时保存当前的键
		}

		// 清空拼写区的内容
		lpSpellList->iKeyNum = 0; 
		lpSpellList->iSpellNum = 0;

		// 依次添加各个键到拼写区，得到拼写结果
		for (i = 0; i < iCurKeyNum ; i++)
		{
			InsertKeyToSpell(lpSpellList,pKeyList[i]);			
		}

		if (lpSpellList->iSpellNum > 0)
			return TRUE;  // 已经拼写出当前结果
		return FALSE; // 当前没有拼写结果
}
