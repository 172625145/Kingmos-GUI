/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef	_LISTUNIT_H
#define	_LISTUNIT_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//---------------------------------------------------
//���� �ֶ����ڽṹ�ĵ�ַ
#ifndef	FIELD_CONTAINER
#define FIELD_CONTAINER(address, type, field)		((type *)( (LPBYTE)(address) - (LPBYTE)(&((type *)0)->field)))
#endif	//FIELD_CONTAINER

//---------------------------------------------------
//˫�������
//defined in winnt.h
typedef	struct	_LIST_UNIT
{
	struct	_LIST_UNIT	*pNext;
	struct	_LIST_UNIT	*pPrev;

}LIST_UNIT, *PLIST_UNIT;
//
#define LIST_CONTAINER(address, type, field)		((type *)( (LPBYTE)(address) - (LPBYTE)(&((type *)0)->field)))

//defined in srvstruc.h
#define	List_InitHead( pListHead )	((pListHead)->pNext = (pListHead)->pPrev = (pListHead))
#define List_IsEmpty( pListHead )	(((pListHead)->pNext == (pListHead)) ? TRUE : FALSE )
//
#define List_RemoveUnit( pUnit )			\
{											\
	(pUnit)->pPrev->pNext = (pUnit)->pNext;	\
	(pUnit)->pNext->pPrev = (pUnit)->pPrev;	\
}
//
#define List_InsertHead( pListHead, pUnit )	\
{											\
    (pUnit)->pNext = (pListHead)->pNext;	\
    (pUnit)->pPrev = (pListHead);			\
    (pListHead)->pNext->pPrev = (pUnit);	\
    (pListHead)->pNext = (pUnit);			\
}
//
#define List_InsertTail( pListHead, pUnit )	\
{											\
    (pUnit)->pNext = (pListHead);			\
    (pUnit)->pPrev = (pListHead)->pPrev;	\
    (pListHead)->pPrev->pNext = (pUnit);	\
    (pListHead)->pPrev = (pUnit);			\
}

//��1����ͷ��˫�� ȥ���󣬲嵽ָ��˫����β��---
//�磺  head1---first1---last1---head1, head2---first2---last2---head2, 
//�����head1---first1---last1---first2---last2---head1
//	last1->pNext = first2;
//	first2->pPrev= last1;
//	head1->pPrev = last2;
//	last2->pNext = head1;
//first1: (pListHead)->pNext       , last1: (pListHead)->pPrev
//first2: (pListIns)->pNext        , last2: (pListIns)->pPrev
#define List_InsertTailByQue( pListHead, pListIns )	\
{													\
	(pListHead)->pPrev->pNext = (pListIns)->pNext;	\
	(pListIns)->pNext->pPrev  = (pListHead)->pPrev;	\
	(pListHead)->pPrev        = (pListIns)->pPrev;	\
	(pListIns)->pPrev->pNext  = (pListHead);		\
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif	//_LISTUNIT_H

