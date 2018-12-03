/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：GWME 句柄管理管理器
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <eassert.h>

typedef struct _OBJ_HANDLE_LIST
{
	union {
		LPVOID lpObjPtr;
		UINT   uNextIndex;
	}obj;
}OBJ_HANDLE_LIST, FAR * LPOBJ_HANDLE_LIST;

#define INIT_OBJ_COUNT_NUM              256
#define NULL_INDEX                      (-1)


static LPOBJ_HANDLE_LIST lpHandleObjArray = NULL;   // 对象指针数组
static UINT freeIndex = NULL_INDEX;   // 空闲对象索引
static UINT cObjCount = 0;			// 对象数组个数
static CRITICAL_SECTION csHandleObjList;
static UINT uRandom = 0x8D;


//#define MAKE_HANDLE_VALUE( h )          (HANDLE)( ( (h) << 8 ) | (uRandom & 0xff) )
//#define GET_INDEX_VALUE( h )            ( ( (DWORD)(h) >> 8 ) )

#define MAKE_HANDLE_VALUE( h )          ( (HANDLE)(h) )
#define GET_INDEX_VALUE( h )            ( (DWORD)(h) )


//分配/重分配 句柄对象数组
static BOOL ReallocHandleObjArray( VOID )
{
	LPOBJ_HANDLE_LIST lpList;
	UINT i, j;	

	
	lpHandleObjArray = realloc( lpHandleObjArray, cObjCount + INIT_OBJ_COUNT_NUM * sizeof( OBJ_HANDLE_LIST ) );
	if( lpHandleObjArray )
	{
		freeIndex = i = cObjCount;
		cObjCount  += INIT_OBJ_COUNT_NUM;
		
		// 连接在一起
		j = cObjCount; //end index
		lpList = lpHandleObjArray + i;
		while( ++i < j )
		{
			lpList->obj.uNextIndex = i;
			lpList++;
		}
		lpList->obj.uNextIndex = NULL_INDEX;  //最后一个
		return TRUE;
	}
	return FALSE;
}

//全局初始化该管理器
BOOL GWME_InitHandleObjectMgr( VOID )
{
	lpHandleObjArray = NULL;
	freeIndex = NULL_INDEX;
	cObjCount = 0;
	if( ReallocHandleObjArray() )
	{
		InitializeCriticalSection( &csHandleObjList );
		return TRUE;
	}
	return FALSE;
}

//全局释放该管理器
VOID GWME_DeinitHandleObjectMgr( VOID )
{
	DeleteCriticalSection( &csHandleObjList );
	free( lpHandleObjArray );	
}

//分配一个句柄对象
HANDLE GWME_AllocHandle( LPVOID lpPtr )
{
	HANDLE h = NULL;

	EnterCriticalSection( &csHandleObjList );

	if( freeIndex != NULL_INDEX )
	{	// 有空闲槽
		h = (HANDLE)freeIndex;
		freeIndex = lpHandleObjArray[freeIndex].obj.uNextIndex;
	}
	else
	{	// 无空闲槽,重新分配
		if( ReallocHandleObjArray() )
		{	//ok
			h = (HANDLE)freeIndex;
			freeIndex = lpHandleObjArray[freeIndex].obj.uNextIndex;  // next
		}
	}
	uRandom++;

	LeaveCriticalSection( &csHandleObjList );

	return MAKE_HANDLE_VALUE( h );
}
//释放一个句柄对象
VOID GWME_FreeHandle( HANDLE h, LPVOID lpPtr )
{
	UINT index;
	EnterCriticalSection( &csHandleObjList );

	index = GET_INDEX_VALUE( h );
	if( index < cObjCount )
	{	//有效句柄值
		if( lpPtr == lpHandleObjArray[index].obj.lpObjPtr )
		{
			lpHandleObjArray[index].obj.uNextIndex = freeIndex;
			freeIndex = index;
		}
		else
		{	//error 
			ASSERT( 0 );
		}
	}
	else
	{
		ASSERT( 0 );
	}

	LeaveCriticalSection( &csHandleObjList );
}
//得到给定句柄的对象指针
LPVOID GWME_GetHandlePtr( HANDLE h )
{
	UINT index = GET_INDEX_VALUE( h );
	if( index < cObjCount )
		return lpHandleObjArray[index].obj.lpObjPtr;
	return NULL;
}
