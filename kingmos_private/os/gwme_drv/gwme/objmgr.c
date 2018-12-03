/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����GWME ������������
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
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


static LPOBJ_HANDLE_LIST lpHandleObjArray = NULL;   // ����ָ������
static UINT freeIndex = NULL_INDEX;   // ���ж�������
static UINT cObjCount = 0;			// �����������
static CRITICAL_SECTION csHandleObjList;
static UINT uRandom = 0x8D;


//#define MAKE_HANDLE_VALUE( h )          (HANDLE)( ( (h) << 8 ) | (uRandom & 0xff) )
//#define GET_INDEX_VALUE( h )            ( ( (DWORD)(h) >> 8 ) )

#define MAKE_HANDLE_VALUE( h )          ( (HANDLE)(h) )
#define GET_INDEX_VALUE( h )            ( (DWORD)(h) )


//����/�ط��� �����������
static BOOL ReallocHandleObjArray( VOID )
{
	LPOBJ_HANDLE_LIST lpList;
	UINT i, j;	

	
	lpHandleObjArray = realloc( lpHandleObjArray, cObjCount + INIT_OBJ_COUNT_NUM * sizeof( OBJ_HANDLE_LIST ) );
	if( lpHandleObjArray )
	{
		freeIndex = i = cObjCount;
		cObjCount  += INIT_OBJ_COUNT_NUM;
		
		// ������һ��
		j = cObjCount; //end index
		lpList = lpHandleObjArray + i;
		while( ++i < j )
		{
			lpList->obj.uNextIndex = i;
			lpList++;
		}
		lpList->obj.uNextIndex = NULL_INDEX;  //���һ��
		return TRUE;
	}
	return FALSE;
}

//ȫ�ֳ�ʼ���ù�����
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

//ȫ���ͷŸù�����
VOID GWME_DeinitHandleObjectMgr( VOID )
{
	DeleteCriticalSection( &csHandleObjList );
	free( lpHandleObjArray );	
}

//����һ���������
HANDLE GWME_AllocHandle( LPVOID lpPtr )
{
	HANDLE h = NULL;

	EnterCriticalSection( &csHandleObjList );

	if( freeIndex != NULL_INDEX )
	{	// �п��в�
		h = (HANDLE)freeIndex;
		freeIndex = lpHandleObjArray[freeIndex].obj.uNextIndex;
	}
	else
	{	// �޿��в�,���·���
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
//�ͷ�һ���������
VOID GWME_FreeHandle( HANDLE h, LPVOID lpPtr )
{
	UINT index;
	EnterCriticalSection( &csHandleObjList );

	index = GET_INDEX_VALUE( h );
	if( index < cObjCount )
	{	//��Ч���ֵ
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
//�õ���������Ķ���ָ��
LPVOID GWME_GetHandlePtr( HANDLE h )
{
	UINT index = GET_INDEX_VALUE( h );
	if( index < cObjCount )
		return lpHandleObjArray[index].obj.lpObjPtr;
	return NULL;
}
