/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __DICTSRCH_H_
#define __DICTSRCH_H_

#include "eframe.h"
#include "eassert.h"

typedef void * HDICT;

typedef struct _EXPLAIN
{
	LPTSTR EnglishWord;
	WORD iEnglishWordLen;
	LPTSTR PhoneticSymbol;
	WORD iPhoneticSymbolLen;
	LPTSTR ChineseExplain;
	WORD iChineseExplainLen;
}EXPLAIN, *LPEXPLAIN;


HDICT Dict_Open(void);
BOOL Dict_SearchWord( HDICT hDict,LPTSTR lpSearchWord,LPEXPLAIN lpExplain );
BOOL Dict_SearchNextWord( HDICT hDict,LPEXPLAIN lpExplain );
BOOL Dict_SearchPreWord( HDICT hDict,LPEXPLAIN lpExplain );
void Dict_Close( HDICT hDict );

#endif
