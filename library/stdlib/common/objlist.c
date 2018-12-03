#include <eframe.h>
#include <eobjlist.h>


BOOL FASTCALL ObjList_Init( LPOBJLIST * lppObjList,  LPOBJLIST lpObj, DWORD objType, ULONG ulOwner )
{
	//lpObj->iRefCount = 0;
	lpObj->ulOwner = ulOwner;
	lpObj->lpNext = lpObj->lpPrev = 0;
	lpObj->objType = objType;
	lpObj->iRefCount = 1;

	if( lppObjList )
		return ObjList_Add( lppObjList, lpObj );

	return TRUE;
}

BOOL FASTCALL ObjList_Add( LPOBJLIST * lppList, LPOBJLIST lpObj )
{
	if( lppList && lpObj->lpNext == NULL && lpObj->lpPrev == NULL )
	{
		//static int count = 0;

		//count++;
		//if( count >=2 )
		//{
		//	WARNMSG( 1, ( "error: count >= 2 \r\n", count,  ) );
		//}


		if( *lppList )
			(*lppList)->lpPrev = lpObj;
		lpObj->lpNext = *lppList;
		//lpObj->lpPrev = NULL;
		*lppList = lpObj;
		//count--;
		return TRUE;
	}
	return FALSE;
}

BOOL FASTCALL ObjList_Remove( LPOBJLIST * lppList, LPOBJLIST lpObj )
{
    static int count = 0;
	ASSERT( lpObj );
	ASSERT( lppList && (*lppList) );
	//ASSERT( lpObj->lpPrev != NULL || lpObj->lpNext != NULL );
	count++;

	if( lpObj->lpPrev == NULL && lpObj->lpNext == NULL )
	{
		//ASSERT( *lppList == lpObj );
		DEBUGMSG( *lppList != lpObj, ( "error: *lppList(0x%x) != lpObj(0x%x)\r\n", *lppList, lpObj ) );
		*lppList = NULL;
		count--;
		return TRUE;
	}

	if( lpObj->lpPrev == NULL )
	{   // headlist
		//ASSERT( *lppList == lpObj );		
		DEBUGMSG( *lppList != lpObj, ( "error: *lppList(0x%x, objType=%d) != lpObj(0x%x, objType=%d), count=%d\r\n", *lppList, (*lppList)->objType, lpObj, lpObj->objType, count ) );
		*lppList = lpObj->lpNext;
		//if( lpObjList )
		(*lppList)->lpPrev = NULL;
		//lpObj->lpNext = NULL;
		lpObj->lpNext = NULL;
	}
	else if( lpObj->lpNext == NULL )
	{  // endlist
		lpObj->lpPrev->lpNext = NULL;//lpObj->lpNext;
		lpObj->lpPrev = NULL;
	}
	else
	{
		lpObj->lpPrev->lpNext = lpObj->lpNext;
		lpObj->lpNext->lpPrev = lpObj->lpPrev;
		lpObj->lpNext = lpObj->lpPrev = NULL;
	}
	count--;
	return TRUE;
}

BOOL FASTCALL ObjList_Delete( LPOBJLIST * lppList, ULONG ulOwner, DELOBJPROC lpDelFun, LPARAM lParam )
{
    LPOBJLIST lpObj = *lppList;
	if( lpObj && lpDelFun )
	{
		do{
			if( lpObj->ulOwner == ulOwner )
			{
				LPOBJLIST lpTemp = lpObj->lpNext;

				ObjList_Remove( lppList, lpObj );

				lpDelFun( lpObj, lParam );

				lpObj = lpTemp;
			}
			else
				lpObj = lpObj->lpNext;
		}while( lpObj );
		return TRUE;
	}
	return FALSE;
}

// add referent count 
/*
int FASTCALL ObjList_AddRef( LPOBJLIST lpObj )
{
    Interlock_Increment( &lpObj->iRefCount );
	//lpObj->iRefCount++;
	return lpObj->iRefCount;
}

int FASTCALL ObjList_AddRefCountNum( LPOBJLIST lpObj, int iAddNum )
{
	Interlock_ExchangeAdd( &lpObj->iRefCount, iAddNum );
	//lpObj->iRefCount += iAddNum;
	return lpObj->iRefCount;
}


// dev referent count
int FASTCALL ObjList_DecRef( LPOBJLIST lpObj )
{
	//lpObj->iRefCount--;
	Interlock_Decrement( &lpObj->iRefCount );
	return lpObj->iRefCount;
}

int FASTCALL ObjList_DecRefCountNum( LPOBJLIST lpObj, int iDecNum )
{
	//lpObj->iRefCount -= iDecNum;
	Interlock_ExchangeAdd( &lpObj->iRefCount, -iDecNum );
	return lpObj->iRefCount;
}
*/

BOOL FASTCALL ObjList_Check( LPOBJLIST * lppList )
{
	LPOBJLIST lpObj = *lppList;
	DWORD dwObjType = OBJ_NULL;
	BOOL bRetv = TRUE;
	while( lpObj )
	{
		if( lpObj->objType == OBJ_NULL )
		{
			ASSERT_NOTIFY( 0, "bad objlist\r\n" );
			bRetv = FALSE;
		}
		if( dwObjType == OBJ_NULL )
			dwObjType = lpObj->objType;
		else
		{
			if( dwObjType != lpObj->objType )
			{
				ASSERT_NOTIFY( 0, "different objtype in same objlist\r\n" );
				bRetv = FALSE;
			}
		}
		lpObj = lpObj->lpNext;
	}
	return bRetv;
}

