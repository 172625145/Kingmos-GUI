/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����ƴ������ʵ��
�汾�ţ�1.0.0.456
����ʱ�ڣ�2002-03-17
���ߣ��½��� Jami chen
�޸ļ�¼��
**************************************************/
#include <eframe.h>
#include "resource.h"
#include "lxsearch.h"
#include "pysearch.h"


// **************************************************
// ��������
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
// ����������
// *******************************************************
static BOOL PYSpellValid(LPCTSTR lpSpell);
static int GetShengMuIndex(LPCTSTR lpCode);
static int GetYunMuIndex(LPCTSTR lpCode);


// **************************************************
// ������BOOL InsertKeyToSpell(LPSPELLLIST lpSpellList,KEYENTRY *pKey)
// ������
// 	IN lpSpellList -- ��ǰ��ƴд���
//  IN pKey --Ҫ����ļ�
// 
// ����ֵ���ɹ����뷵��TRUE�� ���򷵻�FALSE
// ��������������һ������ƴд����С�
// ����: 
// **************************************************
BOOL InsertKeyToSpell(LPSPELLLIST lpSpellList,KEYENTRY *pKey)
{
	TCHAR lpSpellSave[SPELLLISTNUM][8];  // ����ƴ������Ϊ6����chuang,zhuang ...��
	int i,j;
	TCHAR lpCurSpell[8];
	int iNewSpell = 0;

//		RETAILMSG(1,(TEXT(" InsertKeyToSpell ...\r\n")));
		// ��鵱ǰ��ƴд���ϵ�ǰ�����ַ����ƴд���

		if (lpSpellList->iSpellNum == 0)
		{
//			RETAILMSG(1,(TEXT(" 1\r\n")));
			// ��ǰ��ƴд��û�����ݣ�����Ҫ����ƴд���ԣ�ֱ�����
			for (j = 0; j < pKey->iKeyNum; j++)
			{
//				RETAILMSG(1,(TEXT(" 2\r\n")));
				lpCurSpell[0] = pKey->keyValue[j].bChar;  // ���ϵ�ǰ���ַ�
				lpCurSpell[1] = 0;
				if (PYSpellValid(lpCurSpell)) // ���ƴд�Ƿ���Ч
				{  // ƴд��Ч
					strcpy(lpSpellSave[iNewSpell],lpCurSpell); // ����ǰƴд��¼������
					iNewSpell ++;  // �������1
				}
			}
		}
		else
		{
//			RETAILMSG(1,(TEXT(" 3\r\n")));
			for (i = 0; i < lpSpellList->iSpellNum; i++) 
			{
//				RETAILMSG(1,(TEXT(" 4\r\n")));
				strcpy(lpCurSpell,lpSpellList->lpSpellList[i]); // �õ�ԭ����ƴд
				for (j = 0; j < pKey->iKeyNum; j++)
				{
//					RETAILMSG(1,(TEXT(" 5\r\n")));
					lpCurSpell[lpSpellList->iKeyNum] = pKey->keyValue[j].bChar;  // ���ϵ�ǰ���ַ�
					lpCurSpell[lpSpellList->iKeyNum + 1] = 0;
					if (PYSpellValid(lpCurSpell)) // ���ƴд�Ƿ���Ч
					{  // ƴд��Ч
						strcpy(lpSpellSave[iNewSpell],lpCurSpell); // ����ǰƴд��¼������
						iNewSpell ++;  // �������1
					}
				}

			}
		}
		if (iNewSpell)
		{  // ������Ч��ƴд
			// ���µ�ƴд���Ƶ������
//			RETAILMSG(1,(TEXT(" 6\r\n")));
			for (i = 0; i < iNewSpell ; i++) 
			{
				strcpy(lpSpellList->lpSpellList[i],lpSpellSave[i]);
			}
			lpSpellList->iSpellNum = iNewSpell; // ��¼��ǰ��ƴд��Ŀ
			lpSpellList->pKey[lpSpellList->iKeyNum] = pKey; // ����ǰ�ļ����뵽���Ķ�����
			lpSpellList->iKeyNum ++; // ����һ����
//			RETAILMSG(1,(TEXT(" Cur Valid Spell Num = %d\r\n"),lpSpellList->iSpellNum));
			return TRUE; // �Ѿ����뵱ǰ��
		}
//		RETAILMSG(1,(TEXT(" InsertKeyToSpell failure\r\n")));
		return FALSE; // ���ܲ��뵱ǰ��
}



// **************************************************
// ������ int PYSpell(LPCTSTR lpSpell,WORD *lpSpellResult,int iMaxLenght)
// ������
// 	IN lpSpell -- Ҫ����ƴд�ĵ���
//  OUT lpSpellResult -- ���ƴд���
//	IN iMaxLenght -- ���ƴд����Ļ�����Դ�Ŷ��ٸ�
//
// ����ֵ������ƴд�õ������Ŀ
// ����������ƴд���ʡ�
// ����: 
// **************************************************
int PYSpell(LPCTSTR lpSpell,WORD *lpSpellResult,int iMaxLenght)
{
	int iShengMuIndex,iYunMuIndex;
	int iDataIndex,iDataPosition,iNextPosition;
	LPCTSTR lpCode;
	int iResultNum,i;

		lpCode = lpSpell;  // �õ�ƴ��
		iShengMuIndex = GetShengMuIndex(lpCode); // �õ���ĸ����
		lpCode += strlen(g_ShengMuTab[iShengMuIndex]); // ������ĸ��λ��
		iYunMuIndex = GetYunMuIndex(lpCode); // �õ���ĸ��λ��
		if (iYunMuIndex == -1)
		{
			//�����ҵ���Ӧ��ƴ��
			return 0;
		}

		if (iShengMuIndex == 0 && iYunMuIndex == 0)
		{
			//�����ҵ���Ӧ��ƴ��
			return 0;
		}
		iDataIndex = iShengMuIndex * YUNMUNUM +iYunMuIndex -1 ; // �õ���������
		iDataPosition = PYTab_Index[iDataIndex]; // �õ����ݵ�λ��ƫ��
		if (iDataPosition == 0xffff)
		{
			//�����ҵ���Ӧ�ĺ���
			return 0;
		}

		iDataIndex ++; // �õ���һ����������
		while(PYTab_Index[iDataIndex] == 0xFFFF) iDataIndex ++; // �ҵ���һ����Ч����������
		iNextPosition = PYTab_Index[iDataIndex]; // �õ���һ�����ݵ�λ��ƫ��
		iResultNum = iNextPosition - iDataPosition; // �õ���ǰ�����ݸ���
	
		if (iResultNum > iMaxLenght)
		{
			// �ṩ���ڴ治��
			RETAILMSG(1,(TEXT("!!!!!!!! Is Enough Result Buffer [%d]\r\n"),iResultNum));
			iResultNum = iMaxLenght;
		}

		for (i = 0; i < iResultNum ; i++)
		{ // ����ƴ����������������
			lpSpellResult[i] = PYTab_Data[iDataPosition + i];
		}
		// ָ���ĵ���ƴд��Ч���ж�Ӧ�ĺ���
		return iResultNum;  
}

// **************************************************
// ������static int GetShengMuIndex(LPCTSTR lpCode)
// ������
// 	IN lpCode -- ָ��ƴ��
// 
// ����ֵ������ƴ����ĸ������
// �����������õ�ƴ����ĸ��������
// ����: 
// **************************************************
static int GetShengMuIndex(LPCTSTR lpCode)
{
	int i ;

		for (i=SHENGMUNUM-1;i>0;i--)
		{ // ��ѯƴ����ĸ��
			if ( strncmp(lpCode,g_ShengMuTab[i],strlen(g_ShengMuTab[i])) == 0 )
				return i; // �ҵ���ĸ�����ظ���ĸ������
		}
		return 0;
}
// **************************************************
// ������static int GetYunMuIndex(LPCTSTR lpCode)
// ������
// 	IN lpCode -- ƴ����ĸ����
// 
// ����ֵ������ƴ����ĸ������
// �����������õ�ƴ����ĸ��������
// ����: 
// **************************************************
static int GetYunMuIndex(LPCTSTR lpCode)
{
	int i ;

		for (i=0;i<YUNMUNUM;i++)
		{ // ��ѯƴ����ĸ��
			if ( strcmp(lpCode,g_YunMuTab[i]) == 0 )
				return i;// �ҵ���ĸ�����ظ���ĸ������
		}
		return -1;
}


// **************************************************
// ������static BOOL PYSpellValid(LPCTSTR lpSpell)
// ������
// 	IN lpSpell -- Ҫ����ƴд���Եĵ���
//
// ����ֵ��ƴд��Ч����TRUE�����򷵻�FALSE
// ������������ָ�����ʽ���ƴд���ԡ�
// ����: 
// **************************************************
static BOOL PYSpellValid(LPCTSTR lpSpell)
{
	int iShengMuIndex,iYunMuIndex;
	int iDataIndex,iDataPosition;
	LPCTSTR lpCode;

		lpCode = lpSpell;  // �õ�ƴ��
		iShengMuIndex = GetShengMuIndex(lpCode); // �õ���ĸ����
		lpCode += strlen(g_ShengMuTab[iShengMuIndex]); // ������ĸ��λ��
		iYunMuIndex = GetYunMuIndex(lpCode); // �õ���ĸ��λ��
		if (iYunMuIndex == -1)
		{
			//�����ҵ���Ӧ��ƴ��
			return FALSE;
		}

		if (iShengMuIndex == 0 && iYunMuIndex == 0)
		{
			//�����ҵ���Ӧ��ƴ��
			return FALSE;
		}
		iDataIndex = iShengMuIndex * YUNMUNUM +iYunMuIndex -1 ; // �õ���������
		iDataPosition = PYTab_Index[iDataIndex]; // �õ����ݵ�λ��ƫ��
		if (iDataPosition == 0xffff)
		{
			//�����ҵ���Ӧ�ĺ���
			return FALSE;
		}

		// ָ���ĵ���ƴд��Ч���ж�Ӧ�ĺ���
		return TRUE;  
}




// **************************************************
// ������BOOL ReSpellList(LPSPELLLIST lpSpellList)
// ������
// 	IN lpSpellList -- ��ǰ��ƴд���
// 
// ����ֵ���ɹ����뷵��TRUE�� ���򷵻�FALSE
// �������������µõ���ǰ��ƴд�б�
// ����: 
// **************************************************
BOOL ReSpellList(LPSPELLLIST lpSpellList)
{
	KEYENTRY *pKeyList[MAX_SPELLNUM] ;  // ������ǰ�û��Ѿ�����ļ�ֵ
	int iCurKeyNum,i;

		iCurKeyNum = lpSpellList->iKeyNum; // �õ���ǰ�ļ�����Ŀ
		for (i = 0; i < lpSpellList->iKeyNum ; i++)
		{
			pKeyList[i] = lpSpellList->pKey[i]; // ��ʱ���浱ǰ�ļ�
		}

		// ���ƴд��������
		lpSpellList->iKeyNum = 0; 
		lpSpellList->iSpellNum = 0;

		// ������Ӹ�������ƴд�����õ�ƴд���
		for (i = 0; i < iCurKeyNum ; i++)
		{
			InsertKeyToSpell(lpSpellList,pKeyList[i]);			
		}

		if (lpSpellList->iSpellNum > 0)
			return TRUE;  // �Ѿ�ƴд����ǰ���
		return FALSE; // ��ǰû��ƴд���
}
