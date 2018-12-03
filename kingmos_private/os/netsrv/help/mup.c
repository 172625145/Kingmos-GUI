/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：内存单元池 操作---"MUP": memory unit pool 
版本号：  1.0.0
开发时期：2004-02-17
作者：    肖远钢
修改记录：
说明：
    MUP的主要作用和使用目标：
	  相当于实现了1个1维数组，这样可以预分配很多个，而且这是频繁内存取舍操作，可以解决2个问题。
	  1。防止以后内存分配不足
	  2。防止造成大量的内存碎片。
******************************************************/
#include <ewindows.h>
#include <listunit.h>
//"inc_app"
#include <mup.h>
//"inc_drv"
#include "handle_check.h"


/***************  全局区 定义， 声明 *****************/

typedef	struct	_MEMUNITPOOL
{
	LPVOID				this;

	CRITICAL_SECTION	csUnit;
	WORD				wFlag;
	WORD				wLenUnit;	//每个单元的长度
	WORD				wCntMax;	//单元的总个数
	WORD				wCntUsed;	//已使用的个数
	WORD				wCntCur;	//当前的位置
	WORD				wTryEvery;	//

	LONG				nCntRef;

	//该Buffer的地址和总长度
	DWORD				dwLenBuf;	//BUFFER总长度
	BYTE				pPoolBuf[1];//BUFFER地址：包括2部分---1是所有地址数值列表，2是所有地址的真正空间

} MEMUNITPOOL;

#define	MU_HDR_SIZE		sizeof(LPVOID)
#define	MU_HDR_OFFSET	(MU_HDR_SIZE/sizeof(DWORD))

#define	MUPF_EXIT		0x8000
#define	MUPF_CLRUNIT	0x0001

/******************************************************/



// ********************************************************************
// 声明：
// 参数：
//	IN wCntMax-内存单元的个数
//	IN wLenUnit-内存单元的长度
// 返回值：
//	成功，返回非0值;失败，返回0 
// 功能描述：分配1个 内存单元池句柄，并返回
// 引用: 
// ********************************************************************
HPOOL	MUP_AllocPool( WORD wCntMax, WORD wLenUnit, BOOL fIsClearUnit, WORD wTryEvery, WORD wTryFirst )
{
	MEMUNITPOOL*	pMUP;
	DWORD			dwLenBuf;
	DWORD			dwLenAlloc;
	WORD			i;

	//参数检查
	if( wCntMax<2 )
	{
		wCntMax = 2;
	}
	if( wTryFirst > MAX_TRYFIRST )
	{
		wTryFirst = MAX_TRYFIRST;
	}
	if( wTryEvery > MAX_TRYEVERY )
	{
		wTryEvery = MAX_TRYEVERY;
	}
	//计算: dwLenBuf---所有单元的空间大小（并对齐）， 
	//      dwLenAlloc---整个句柄要分配的空间大小（并对齐）
	wLenUnit = (WORD)(( (MU_HDR_SIZE + wLenUnit + 7)/4 ) * 4);
	dwLenBuf = wLenUnit * wCntMax;
	dwLenAlloc = dwLenBuf + sizeof(MEMUNITPOOL);
	dwLenAlloc = (( dwLenAlloc + 7)/4) * 4;
	//分配
	for( i=0; i<wTryFirst; i++ )
	{
		pMUP = (MEMUNITPOOL*)malloc( dwLenAlloc );
		if( pMUP )
		{
			break;
		}
		Sleep( WAIT_TRYFIRST );		
	}
	if( !pMUP )
	{
		return NULL;
	}
	HANDLE_INIT( pMUP, dwLenAlloc );
	InitializeCriticalSection( &pMUP->csUnit );

	//保存信息
	pMUP->wLenUnit = wLenUnit;
	pMUP->wCntMax  = wCntMax;
	pMUP->dwLenBuf = dwLenBuf;
	pMUP->wTryEvery = wTryEvery;
	if( fIsClearUnit )
	{
		pMUP->wFlag |= MUPF_CLRUNIT;
	}

	return (HPOOL)pMUP;
}

// ********************************************************************
// 声明：
// 参数：
//	IN hMUP-内存单元池句柄
// 返回值：
//	无
// 功能描述：释放1个 内存单元池句柄
// 引用: 
// ********************************************************************
void	MUP_FreePool( HPOOL hMUP )
{
	MEMUNITPOOL*		pMUP = (MEMUNITPOOL*)hMUP;
	WORD				i;
	DWORD*				pBufUnit_hdr;
	WORD				wLenUnit;
	WORD				wCntMax;

	//句柄安全检查
	if( !HANDLE_CHECK(pMUP) )
	{
		return ;
	}
	//
	pMUP->wFlag |= MUPF_EXIT;

	//等待
	for( i=0; i<100; i++ )
	{
		if( pMUP->nCntRef<=0 )
		{
			break;
		}
		Sleep( 2 );
	}

	//完全退出
	if( pMUP->wFlag & MUPF_CLRUNIT )
	{
		memset( pMUP->pPoolBuf, 0, pMUP->dwLenBuf );
	}
	else
	{
		pBufUnit_hdr = (DWORD*)(pMUP->pPoolBuf);
		wLenUnit = pMUP->wLenUnit;
		wCntMax = pMUP->wCntMax;
		for( i=0; i<wCntMax; i++ )
		{
			*pBufUnit_hdr = 0;
			pBufUnit_hdr += wLenUnit;
		}
	}
	DeleteCriticalSection( &pMUP->csUnit );
	HANDLE_FREE( pMUP );
}

// ********************************************************************
// 声明：
// 参数：
//	IN hMUP-内存单元池句柄
// 返回值：
//	成功，返回非0值;失败，返回0 
// 功能描述：分配1个 内存单元池中的 内存单元，并返回
// 引用: 
// ********************************************************************
LPVOID	MUP_AllocUnit( HPOOL hMUP )
{
	MEMUNITPOOL*		pMUP = (MEMUNITPOOL*)hMUP;
	DWORD*				pBufUnit_hdr;
	LPBYTE				pPoolBuf;
	WORD				wCntMax;
	WORD				wCntCur;
	WORD				wLenUnit;
	WORD				i;

	//句柄安全检查
	if( HANDLE_F_FAIL(pMUP, MUPF_EXIT) )
	{
		return NULL;
	}
	pMUP->nCntRef ++;
	// 等待有空间
	wCntMax  = pMUP->wCntMax;
	for( i=0; (i<pMUP->wTryEvery) && !(pMUP->wFlag & MUPF_EXIT); i++ )
	{
		if( pMUP->wCntUsed<wCntMax )
		{
			break;
		}
		Sleep( WAIT_TRYEVERY );
	}
	if( (pMUP->wFlag & MUPF_EXIT) || (pMUP->wCntUsed>=wCntMax) )
	{
		pMUP->nCntRef --;
		return NULL;
	}

	//
	// 寻找没有被使用单元
	//
	pPoolBuf = pMUP->pPoolBuf;
	wLenUnit = pMUP->wLenUnit;
	EnterCriticalSection( &pMUP->csUnit );
	if( (pMUP->wFlag & MUPF_EXIT) || (pMUP->wCntUsed>=wCntMax) )
	{
		LeaveCriticalSection( &pMUP->csUnit );
		pMUP->nCntRef --;
		return 0;
	}
	wCntCur  = pMUP->wCntCur;
	//定位到当前位置
	pBufUnit_hdr = (DWORD*)(pPoolBuf + wCntCur * wLenUnit);
	//如果需要查找空闲位置
	if( *pBufUnit_hdr )
	{
		//继续下找
		for( i=wCntCur; !(pMUP->wFlag & MUPF_EXIT) && (i<wCntMax); i++ )
		{
			pBufUnit_hdr += wLenUnit;
			if( *pBufUnit_hdr==0 )
			{
				break;
			}
		}
		//还不行的话，就回头看
		if( *pBufUnit_hdr )
		{
			for( i=0; !(pMUP->wFlag & MUPF_EXIT) && (i<wCntCur); i++ )
			{
				pBufUnit_hdr += wLenUnit;
				if( *pBufUnit_hdr==0 )
				{
					break;
				}
			}
		}

		//
		wCntCur = i+1;
	}
	else
	{
		wCntCur ++;
	}
	// 获得
	if( *pBufUnit_hdr==0 )
	{
		//设置内存单元信息
		if( pMUP->wFlag & MUPF_CLRUNIT )
		{
			memset( pBufUnit_hdr, 0, wLenUnit );
		}
		*pBufUnit_hdr = (DWORD)pMUP;

		//设置POOL信息
		if( wCntCur>=wCntMax )
		{
			wCntCur = 0;
		}
		pMUP->wCntCur  = wCntCur;
		pMUP->wCntUsed ++;
	}
	LeaveCriticalSection( &pMUP->csUnit );

	pMUP->nCntRef --;
	return (LPVOID)(pBufUnit_hdr+MU_HDR_OFFSET);
}

// ********************************************************************
// 声明：
// 参数：
//	IN pBufUnit-内存单元池中的 内存单元
// 返回值：
//	成功，返回非0值;失败，返回0 
// 功能描述：释放1个 内存单元池中的 内存单元
// 引用: 
// ********************************************************************
void	MUP_FreeUnit( LPVOID pBufUnit )
{
	MEMUNITPOOL*		pMUP;
	DWORD*				pBufUnit_hdr;

	//参数检查
	if( !pBufUnit )
	{
		return ;
	}
	pBufUnit_hdr = (DWORD*)( (DWORD)pBufUnit - MU_HDR_SIZE );
	pMUP = (MEMUNITPOOL*)(*pBufUnit_hdr);
	//句柄安全检查
	if( HANDLE_F_FAIL(pMUP, MUPF_EXIT) )
	{
		return ;
	}
	//释放
	pMUP->nCntRef ++;
	EnterCriticalSection( &pMUP->csUnit );

	//开始释放
	if( pMUP->wFlag & MUPF_CLRUNIT )
	{
		memset( pBufUnit_hdr, 0, pMUP->wLenUnit );
	}
	else
	{
		*pBufUnit_hdr = 0;
	}
	//如果当前的单元是被使用的，则用这个释放的单元作为当前单元
	if( *( (DWORD*)(pMUP->pPoolBuf + pMUP->wCntCur*pMUP->wLenUnit) ) )
	{
		pMUP->wCntCur  = (WORD)( ((DWORD)((LPBYTE)pBufUnit_hdr - pMUP->pPoolBuf))  /  pMUP->wLenUnit );
		if( pMUP->wCntCur>=pMUP->wCntMax )
		{
			pMUP->wCntCur = 0;
		}
	}
	pMUP->wCntUsed --;
	
	LeaveCriticalSection( &pMUP->csUnit );
	pMUP->nCntRef --;
}


