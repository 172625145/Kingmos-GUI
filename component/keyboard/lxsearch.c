#include <ewindows.h>
#include "pylxtab.h"

//extern const struct LXIndexStruct { 
//        WORD wWord; 
//        DWORD nIndex; 
//} stLXIndex[];
//extern const unsigned char  PYLXData[];

static DWORD m_nIndexCount = sizeof(stLXIndex) / sizeof (struct LXIndexStruct);

static DWORD GetIndex(WORD wWord, struct LXIndexStruct *indexTab);


//void DoPYLXSearch(TCHAR chLXWord,PTRLIST *lpLXWordList) 
int DoPYLXSearch(unsigned short chLXWord,unsigned short *lpLXWordList,int dwMaxNum) 
{  
	// Search
	// TODO: Add your control notification handler code here
//	TCHAR *lpString;
	DWORD iIndex;
	LPTSTR lpCurWord;
	DWORD LXWordLen ;
	int iNum;

//		PtrListFreeAll(lpLXWordList);
//		RETAILMSG(1,(TEXT("Enter DoPYLXSearch\r\n")));

		iIndex = GetIndex(chLXWord,(struct LXIndexStruct *)stLXIndex);

//		RETAILMSG(1,(TEXT("LXWord iIndex = %d\r\n"),iIndex));

		if (iIndex == -1)
			return 0;
		lpCurWord = (LPTSTR)(PYLXData + iIndex);
		iNum = 0;
		while(*lpCurWord)
		{
			LXWordLen = strlen(lpCurWord) +1;
//			lpString = (LPTSTR)malloc((LXWordLen + 1) * sizeof(TCHAR));
//			*lpString = chLXWord;
//			wcscpy(lpString+1,lpCurWord);
			//m_hList.AddString(lpString);
//			PtrListInsert(lpLXWordList,lpString);
//			lpLXWordList[iNum] = *(unsigned short *)lpCurWord;
			lpLXWordList[iNum] = MAKEWORD(*lpCurWord,*(lpCurWord +1));
//			RETAILMSG(1,(TEXT("LXWord WORD = %X\r\n"),lpLXWordList[iNum]));
			lpCurWord += LXWordLen;
			iNum ++;
			if (iNum >= dwMaxNum )
				break;
		}
		return iNum;
}
/*
static DWORD GetIndex(WORD wWord, struct LXIndexStruct *indexTab)
{
		// 使用二分法查找
	DWORD nStart = 0, nEnd = m_nIndexCount -1;
	DWORD nCurrent = (nStart + nEnd) /2;

		while(1)
		{
			if (indexTab[nCurrent].wWord == wWord)
//			if (wcsncmp((LPTSTR)&indexTab[nCurrent].wWord,(LPTSTR)&wWord,2) == 0)
			{
				return 	indexTab[nCurrent].nIndex;
			}
			if (indexTab[nCurrent].wWord > wWord)
//			if (wcsncmp((LPTSTR)&indexTab[nCurrent].wWord,(LPTSTR)&wWord,2) > 0)
			{
				nEnd = nCurrent;
				nCurrent = (nStart + nEnd) /2;
			}
			else
			{
				nStart = nCurrent;
				nCurrent = (nStart + nEnd) /2;
			}
			if (nCurrent == nStart || nCurrent == nEnd)
			{
				if (indexTab[nStart].wWord == wWord)
//				if (wcsncmp((LPTSTR)&indexTab[nStart].wWord,(LPTSTR)&wWord,2) == 0)
				{
					return 	indexTab[nStart].nIndex;
				}
				if (indexTab[nEnd].wWord == wWord)
//				if (wcsncmp((LPTSTR)&indexTab[nEnd].wWord,(LPTSTR)&wWord,2) == 0)
				{
					return 	indexTab[nEnd].nIndex;
				}
				return -1;
			}
		}
}
*/

static DWORD GetIndex(WORD wWord, struct LXIndexStruct *indexTab)
{
		// 使用二分法查找
	DWORD nStart = 0, nEnd = m_nIndexCount -1;
	DWORD nCurrent = (nStart + nEnd) /2;

//		RETAILMSG(1,(TEXT("Enter GetIndex\r\n")));
//		RETAILMSG(1,(TEXT("Search Word = %x\r\n"),wWord));
		wWord = MAKEWORD(HIBYTE(wWord),LOBYTE(wWord));
//		RETAILMSG(1,(TEXT("Search Word = %x\r\n"),wWord));
		while(1)
		{
//			RETAILMSG(1,(TEXT("Current Word = %x\r\n"),indexTab[nCurrent].wWord));
//			RETAILMSG(1,(TEXT("Start Word = %x\r\n"),indexTab[nStart].wWord));
//			RETAILMSG(1,(TEXT("End Word = %x\r\n"),indexTab[nEnd].wWord));
			if (indexTab[nCurrent].wWord == wWord)
//			if (strncmp((LPTSTR)&indexTab[nCurrent].wWord,(LPTSTR)&wWord,2) == 0)
			{
				return 	indexTab[nCurrent].nIndex;
			}
			if (indexTab[nCurrent].wWord > wWord)
//			if (strncmp((LPTSTR)&indexTab[nCurrent].wWord,(LPTSTR)&wWord,2) > 0)
			{
				nEnd = nCurrent;
				nCurrent = (nStart + nEnd) /2;
			}
			else
			{
				nStart = nCurrent;
				nCurrent = (nStart + nEnd) /2;
			}
			if (nCurrent == nStart || nCurrent == nEnd)
			{
				if (indexTab[nStart].wWord == wWord)
//				if (strncmp((LPTSTR)&indexTab[nStart].wWord,(LPTSTR)&wWord,2) == 0)
				{
					return 	indexTab[nStart].nIndex;
				}
				if (indexTab[nEnd].wWord == wWord)
//				if (strncmp((LPTSTR)&indexTab[nEnd].wWord,(LPTSTR)&wWord,2) == 0)
				{
					return 	indexTab[nEnd].nIndex;
				}
				return -1;
			}
		}
}
