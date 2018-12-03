#include <edef.h>
#include <listunit.h>

static BOOL CheckList( PLIST_UNIT pUnit )
{
	PLIST_UNIT pThis = pUnit;

	while( pUnit->pNext != pThis )
	{
		pUnit = pUnit->pNext;
	}
	pUnit = pThis;
	while( pUnit->pPrev != pThis )
	{
		pUnit = pUnit->pPrev;
	}
	return TRUE;
}

void List_RemoveUnit( PLIST_UNIT pUnit )
{
	CheckList( pUnit );
	(pUnit)->pPrev->pNext = (pUnit)->pNext;
	(pUnit)->pNext->pPrev = (pUnit)->pPrev;
}

void List_InsertHead( PLIST_UNIT pListHead, PLIST_UNIT pUnit )
{
	CheckList( pListHead );
	//CheckList( pUnit );
    (pUnit)->pNext = (pListHead)->pNext;	
    (pUnit)->pPrev = (pListHead);			
    (pListHead)->pNext->pPrev = (pUnit);	
    (pListHead)->pNext = (pUnit);
}
void List_InsertTail( PLIST_UNIT pListHead, PLIST_UNIT pUnit )
{
	CheckList( pListHead );
	//CheckList( pUnit );
    (pUnit)->pNext = (pListHead);			
    (pUnit)->pPrev = (pListHead)->pPrev;	
    (pListHead)->pPrev->pNext = (pUnit);	
    (pListHead)->pPrev = (pUnit);			
}
void List_InsertTailByQue( PLIST_UNIT pListHead, PLIST_UNIT pListIns )
{
	CheckList( pListHead );
	//CheckList( pListIns );
	(pListHead)->pPrev->pNext = (pListIns)->pNext;	
	(pListIns)->pNext->pPrev  = (pListHead)->pPrev;	
	(pListHead)->pPrev        = (pListIns)->pPrev;	
	(pListIns)->pPrev->pNext  = (pListHead);		
}
