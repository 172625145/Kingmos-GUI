/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：KTABLE-数据库管理模块
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：
	2005-01-21, Table_Pack 增加 句柄功能
    2004-07-27, 增加基本密码功能 和 用户数据区
	2004-06-18, 增加固定文件尺寸大小的功能，即数据表的文件大小不会动态变换

******************************************************/

#include <ewindows.h>
#include <eassert.h>
#include <eobjtype.h>
#include <etable.h>
#include <eapisrv.h>
#include <tablesrv.h>
#include <eobjcall.h>

//////////////////TABLE FORMAT///////////////////////////////////
//TABLE_FILE_HEAD
//		TABLE_FILE_HEAD.wNodeIndexStartBlock---->RNODEINDEX
//.....
//RNODEINDEX[0]
//RNODEINDEX[1]
//RNODEINDEX[2]
//RNODEINDEX[n....]
//RNODEINDEX[n].wDataStartBlock--->RNODEDATA + DATA
//...
////////////////////////////////////////////////////////////////////


#define NULL_BLOCK  0xffff

#define TABLE_VERSION   MAKE_VERSION( 1, 0, 0 )//0x01000000

#define NEXT_BLOCK( pfat, wSector )  ( *((pfat)+(wSector)) )

#define  BLOCK_SIZE 128			//默认的块大小
#define MAX_PASSWORD    16
#define MAX_USERDATA    48
//static const TCHAR id[] = TEXT("KTABLE  ");
static const TCHAR id[] =   TEXT("微逻辑-灵巧数据库");
#define ID_ALIGN_SIZE   ( ( sizeof(id) + 3 ) & (~3) )
//定义数据表文件头结构
typedef struct _TABLE_FILE_HEAD
{
    TCHAR idName[ID_ALIGN_SIZE]; // 识别码 = id
    DWORD ver;       // 版本号
	WORD wCheckSum;	 // 以下值的字节和
    
    WORD nNodeNum;	// 记录节点数
    WORD nFieldNum;	// 字段数
    WORD nNodeDataSize;   // 节点的数据大小

    WORD wTotalBlock;	//文件里的块数
    WORD wFatStartBlock;  //块分配表所在的块号 file alloc table start sector
    WORD wNodeIndexStartBlock;  //节点所在的块号
    //WORD nFatBlockNum;    // file alloc sector number    
    WORD wFreeBlock;	//空闲块链表的第一个块
    WORD nFreeCount;	//空闲块数
    WORD nBytesPerBlock;//每个块的大小

	DWORD dwFixFileSize;	//假如固定文件大小，该成员 > 0

	BYTE szPassword[MAX_PASSWORD];    //密码区
	DWORD dwPasswordID;    // = sum(password[0...16]) | (n << 12) | 有数值 | 有小写英文 | 有大写英文 | 有符号 | 有>0x80的字符 | 明码 | 有效 
	                       //    12 bits +              4bit 
	BYTE bUserData[MAX_USERDATA];		//用户自定义数据

}TABLE_FILE_HEAD, * PTFH;

#define RA_LOGIC_DELETE   0x80
// 定义节点结构
typedef struct _RNODEINDEX
{
    BYTE bAttrib; //  该节点属性
	BYTE bReserve; // 保留 for add tount index count or other option
    WORD wDataStartBlock;  //该节点数据开始块 data start block    
}RNODEINDEX, * PRNODEINDEX;

// 定义记录的每个字段长度结构
typedef struct _RNODEDATA
{
    WORD nFieldSize[1];
}RNODEDATA, * PRNODEDATA;

#define RNODEDATA_SIZE( n ) ( sizeof( RNODEDATA ) + ((n) - 1) * sizeof(WORD) )

// 定义打开的数据表对象结构
typedef struct _TABLE
{
	LPTSTR lpszFileName; // 数据表文件名
	DWORD dwRefCount; // 该数据表引用数
    HANDLE hFile; //打开的文件句柄
    TABLE_FILE_HEAD  tfh; // 该数据表文件头
    WORD *pfat; //该数据表块分配表在内存中的地址
    DWORD dwRN;   // 当前记录的记录号 current record no.
    RNODEINDEX RNodeIndex; // 当前记录节点数据
    PRNODEDATA pRNodeData;  // 当前记录字段数据 curecord record node
    CRITICAL_SECTION csTable;  // 该打开数据表的冲突段
	BYTE bIsEncrypt;	// 数据否加密
	BYTE bEncryptKey;	//加密键值
	BYTE bFlush;    //是否需要刷新
	BYTE dump;
}TABLE, FAR * LPTABLE;

// 已打开数据表对象链表结构
typedef struct _TABLE_FILE
{
	DWORD   objType; //对象类型，must be OBJ_TABLE
	struct _TABLE_FILE * lpNext; //指向下一个数据表对象
	DWORD  dwShareMode;//该数据表共享模式
	DWORD dwAccessMode; //该数据表存取模式
	HANDLE hOwnerProc;//该数据表的拥有者进程，owner process;
	LPTABLE lpTable;//该数据表文件所关联的数据表对象结构
    DWORD rn;   //当前已被缓冲的记录，用于加速存取当前记录 current record no.

}TABLE_FILE, * LPTABLE_FILE;

// 已打开数据表查找对象结构
typedef struct _TABELFIND
{
	UINT objType;    //对象类型，=OBJ_FINDTABLE
    HTABLE hTable;	//打开的数据表对象句柄 
    UINT uFieldFrom;	//查找的开始字段
    UINT uFieldTo;	//查找的结束字段
    void * lpvFind;	//查找的数据
	void * lpvBuf;  //临时缓冲，用于读取数据用
    UINT uLen;//查找的数据长度
}TABELFIND, * PTABLEFIND;

#define TABLE_ERROR 1

static BOOL MoveData( LPTABLE lpTable, WORD blockIndex, long posMoveFrom, long posMoveTo );
static BOOL _InsertRecord( LPTABLE lpTable, WORD rn, FIELDITEM fieldItem[], HANDLE hOwnerProc );
static BOOL _Exchange( LPTABLE lpTable, WORD rn0, WORD rn1 );
static BOOL _CopyRecordData( LPTABLE lpTable, FIELDITEM fieldItem[], DWORD size, BOOL bFrag, HANDLE hOwnerProc );
static WORD _SetRecordPointerAndUpdate( LPTABLE lpTable, long rn );
// 数据表对象链表冲突段
static CRITICAL_SECTION csTableList;
// 数据表对象链表头
static LPTABLE_FILE lpTableList = NULL;

// **************************************************
// 声明：BOOL InitialTable( void )
// 参数：
//		无
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		初始化数据表全局区
// 引用: 
//		
// ************************************************
BOOL InitialTable( void )
{

	InitializeCriticalSection( &csTableList );
#ifdef __DEBUG
	csTableList.lpcsName = "CS-TABLE";
#endif
	return TRUE;
}

// **************************************************
// 声明：static LPTABLE_FILE _GetHTABLEPtr( HANDLE handle )
// 参数：
//		IN handle - 数据表句柄
// 返回值：
//		假如成功，返回非NULL指针；否则，返回NULL
// 功能描述：
//		由句柄得到数据表对象指针
// 引用: 
//		
// ************************************************

static LPTABLE_FILE _GetHTABLEPtr( HANDLE handle )
{
	LPTABLE_FILE lpTableFile = (LPTABLE_FILE)handle;
	if( handle && 
		handle != INVALID_HANDLE_VALUE && 
		lpTableFile->objType == OBJ_TABLE )	//检查是否有效
	{	//是
		return lpTableFile;
	}
	else
	{
		SetLastError( ERROR_INVALID_PARAMETER );
	}
	ERRORMSG( 1, ("_GetHTABLEPtr:Invalid table handle(0x%x).\r\n", handle) );
	return NULL;
}

// **************************************************
// 声明：static PTABLEFIND _GetHTABLEFINDPtr( HANDLE handle )
// 参数：
//		IN handle - 数据表句柄
// 返回值：
//		假如成功，返回非NULL指针；否则，返回NULL
// 功能描述：
//		由句柄得到搜索对象指针
// 引用: 
//		
// ************************************************

static PTABLEFIND _GetHTABLEFINDPtr( HANDLE handle )
{
	PTABLEFIND lpFind = (PTABLEFIND)handle;
	if( lpFind && lpFind->objType == OBJ_FINDTABLE ) // 是否有效 ？
	{	//是
		return lpFind;
	}
	else
		SetLastError( ERROR_INVALID_PARAMETER );
	ERRORMSG( 1, ( "Invalid table handle(0x%x).\r\n", handle ) );
	return NULL;
}

// **************************************************
// 声明：static BOOL FreeBlock( LPTABLE lpTable, WORD wStartBlock ) 
// 参数：
//		IN lpTable - 数据表对象
//		IN wStartBlock - 需要释放的块链首地址	
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		释放块，被释放的块放在数据表的空闲块首
// 引用: 
//		
// ************************************************

static BOOL FreeBlock( LPTABLE lpTable, WORD wStartBlock ) 
{
    WORD * pfat = lpTable->pfat; // 该数据表的分配表
    WORD n, w, k;

    ASSERT( wStartBlock != NULL_BLOCK );
    k = w = wStartBlock;
	// 得到块链中的最后一个块
    for( n = 0; w != NULL_BLOCK; n++ )
    {
        k = w;
        w = NEXT_BLOCK( pfat, w ); // 在块链中的下一个块
    }
	// k 是最后一个块的索引
    *(pfat+k) = lpTable->tfh.wFreeBlock; // 最后一个块连接数据表中的空闲块
    lpTable->tfh.wFreeBlock = wStartBlock;	// 重新设定数据表的首空闲块
    lpTable->tfh.nFreeCount += n; // 增加空闲块数
    return TRUE;
}

/*
// **************************************************
// 声明：static BOOL FreeBlock( LPTABLE lpTable, WORD wStartBlock ) 
// 参数：
//		IN lpTable - 数据表对象
//		IN wStartBlock - 需要释放的块链首地址	
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		释放块，被释放的块按索引号排序放在数据表
// 引用: 
//		
// ************************************************

static BOOL FreeBlock( LPTABLE lpTable, WORD wStartBlock ) 
{
    WORD * pfat = lpTable->pfat;
    _ASSERT( wStartBlock != NULL_BLOCK );
    if( wStartBlock != NULL_BLOCK )
    {    
        WORD wPrev, wNext, wAllocNext;

        wNext = lpTable->tfh.wFreeBlock;
        while( wNext != NULL_BLOCK )
        {            
            wPrev = wNext;
            wNext = NEXT_BLOCK( pfat, wNext );
            if( wStartBlock < wNext )
            {
                *(pfat + wPrev) = wStartBlock;
                
                wAllocNext = NEXT_BLOCK( pfat, wStartBlock );
                while( wAllocNext != NULL_BLOCK )
                {                    
                    if( wAllocNext < wStartBlock )
                        break;
                    wStartBlock = wAllocNext;
                    wAllocNext = NEXT_BLOCK( pfat, wAllocNext );
                }
                *(pfat + wStartBlock) = wNext;
                wStartBlock = NEXT_BLOCK( pfat, wStartBlock );
                wNext = lpTable->tfh.wFreeBlock;
            }
        }

        lpTable->tfh.nFreeCount += n;
    }
    return TRUE;
}
*/

// **************************************************
// 声明：static WORD AllocBlock( LPTABLE lpTable, WORD nBlockNum )
// 参数：
//		IN lpTable - 数据表对象
//		IN nBlockNum - 需要从系统分配的块数
// 返回值：
//		假如成功，返回非NULL_BLOCK值；否则，返回NULL_BLOCK
// 功能描述：
//		从数据表分配一定数量的块
// 引用: 
//		
// ************************************************

static WORD AllocBlock( LPTABLE lpTable, WORD nBlockNum )
{
    WORD first, n, k;
    WORD * pfat;
	
    if( nBlockNum > 0 && lpTable->tfh.nFreeCount >= nBlockNum ) // 是否有足够的块 ？
    {   // 有
        first = lpTable->tfh.wFreeBlock;
        k = (WORD)(nBlockNum-1);
        pfat = lpTable->pfat;  // 数据表分配表
        for( n = 0; n < k; n++ ) // 先得到 nBlockNum-1 个，最后一个在循环外分配
        {
            first = NEXT_BLOCK( pfat, first );
        }
		// 分配最后一个块
        n = first;  // end block
        first = NEXT_BLOCK( pfat, first);  // get next block
        *(pfat+n) = NULL_BLOCK; // 最后一个块指向结束块

        // 将已分配的块从数据表移出，并返回分配的块 save return value
        n = lpTable->tfh.wFreeBlock;  // 分配的首块值
        lpTable->tfh.wFreeBlock = first; // 更新数据表首空闲块值
        // 更新数据表空闲块数，reset free block and count
        lpTable->tfh.nFreeCount -= (WORD)nBlockNum;
        return n;
    }
    return NULL_BLOCK;
}

// **************************************************
// 声明：static WORD ReallocBlock(
//                          LPTABLE lpTable, 
//                          WORD wAllocBlock, 
//                          WORD nBlockNum )

// 参数：
//		IN lpTable - 数据表对象指针
//		IN wAllocBlock - 之前已经分配的块索引，如果为NULL_BLOCK则等同于AllocBlock
//		IN nBlockNum - 需要重新分配的块数,如果为0,则等同于FreeBlock
// 返回值：
//		假如成功，返回非NULL_BLOCK值；否则，返回NULL_BLOCK
// 功能描述：
//		改变已经分配的块数，重新分配新的块数
// 引用: 
//		
// ************************************************

static WORD ReallocBlock(
                          LPTABLE lpTable, 
                          WORD wAllocBlock, 
                          WORD nBlockNum )
{
    WORD i;

    if( wAllocBlock == NULL_BLOCK )
    {	// 等同于AllocBlock
        return AllocBlock( lpTable, nBlockNum );
    }
    else if( nBlockNum == 0 )
    {   // 等同于FreeBlock, 释放所有的块。free all block
        FreeBlock( lpTable, wAllocBlock );
        return NULL_BLOCK;
    }
    else
    {
        WORD s = wAllocBlock;
        WORD p = wAllocBlock;
        WORD * pfat = lpTable->pfat;  // 数据表的分配表
        i = 0;
		//搜索到快结束
        while( s != NULL_BLOCK )
        {
            i++;
            if( i > nBlockNum ) // 已分配块数大于新分配块数 ？
            {  // 是，截断已分配块数并释放余下的块。decrease sector, to free other sector
                *(pfat+p) = NULL_BLOCK;
                FreeBlock( lpTable, s );
                return wAllocBlock;
            }
            p = s; // 保存前一个块
            s = NEXT_BLOCK( pfat, s ); // 下一个块
        }  
		// 
        if( i < nBlockNum ) // 已分配块数 小于< 新分配块数吗 ？ 
        {	// 是，增加新的块数，并将其连接在一起
            s = (WORD)AllocBlock( lpTable, (WORD)(nBlockNum - i) );
            if( s != NULL_BLOCK )
                *(pfat+p) = s;   // link them
            return wAllocBlock;
        }
        else if( i == nBlockNum ) // 必然相等，不必做任何事
            return wAllocBlock;
    }
    return NULL_BLOCK;
}
/*
// **************************************************
// 声明：static WORD GetBlockOffset( LPTABLE lpTable, WORD wStartBlock, WORD nOffset )
// 参数：
//		IN lpTable - 数据表指针
//		IN wStartBlock - 开始块
//		IN nOffset - 从开始块起计数的块数
// 返回值：
//		块索引
// 功能描述：
//		得到从开始块起计数的第nOffset个块的块号
// 引用: 
//		
// ************************************************

static WORD GetBlockOffset( LPTABLE lpTable, WORD wStartBlock, WORD nOffset )
{
    WORD * pfat = lpTable->pfat;
    while( wStartBlock != NULL_BLOCK && nOffset )
    {
        wStartBlock = NEXT_BLOCK( pfat, wStartBlock );        
        nOffset--;
    }
    return wStartBlock;
}
*/
// **************************************************
// 声明：static WORD GetBlockNum( LPTABLE lpTable, WORD wStartBlock )
// 参数：
//		IN lpTable - 数据表对象指针
//		IN wStartBlock - 开始块索引
// 返回值：
//		假如成功，返回块数；否则，返回0
// 功能描述：
//		得到块链表中的块数
// 引用: 
//		
// ************************************************

static WORD GetBlockNum( LPTABLE lpTable, WORD wStartBlock )
{
    WORD * pfat = lpTable->pfat;
    WORD n = 0;
    while( wStartBlock != NULL_BLOCK )
    {
        wStartBlock = NEXT_BLOCK( pfat, wStartBlock );        
        n++;
    }
    return n;
}

// **************************************************
// 声明：static BOOL ReallocFile( LPTABLE lpTable, WORD wFirstBlock, DWORD dwNewSize, WORD * lpwAlloc )
// 参数：
//		IN lpTable - 数据表对象指针
//		IN wFirstBlock - 开始块
//		IN dwNewSize - 新的文件大小
//		OUT lpwAlloc - 用于接受新分配的块索引
// 返回值：
//		假如成功，返回TRUE, lpwAlloc为新的块索引；否则，返回NULL_BLOCK
// 功能描述：
//		为一个块链重新分配大小，如果当前数据表文件没有足够的块，则扩大数据表文件的大小
// 引用: 
//		
// ************************************************

#define FAT_BLOCK_NUM( lpTable, bn ) ( ( (bn)*sizeof(WORD)+(lpTable)->tfh.nBytesPerBlock-1 ) / (lpTable)->tfh.nBytesPerBlock )

static BOOL ReallocFile( LPTABLE lpTable, WORD wFirstBlock, DWORD dwNewSize, WORD * lpwAlloc )
{
    WORD wStartBlock = NULL_BLOCK;
    long num = (WORD)( (dwNewSize + lpTable->tfh.nBytesPerBlock - 1) / lpTable->tfh.nBytesPerBlock );

    if( num )  // num为新的块数
    {
        long bn = lpTable->tfh.nFreeCount - num + GetBlockNum( lpTable, wFirstBlock ); // 需求差额
        if( bn >= 0  )  // 当前数据表有足够的块吗 ？
            wStartBlock = ReallocBlock( lpTable, wFirstBlock, (WORD)num ); // 有，直接分配
        else if( lpTable->tfh.dwFixFileSize == 0 )	//可以动态分配文件大小吗　？
        {   // 没有，需要扩大文件大小，add file size
            DWORD dwSize;
            WORD nOldFatBlock;
            WORD nNewFatBlock;
			WORD nIndexNumPerBlock;
			WORD nOtherDataBlockNum;
            
            bn = -bn;
            nOldFatBlock = GetBlockNum( lpTable, lpTable->tfh.wFatStartBlock ); // 当前数据表FAT区所占的块数
			
			// 扩大数据表大小后，数据表的新FAT区的块数
            //new algorithm : X_fatblock + Y_allotherblock  <= X_fatblock * IndexNumOfPerBlock;
			nIndexNumPerBlock = lpTable->tfh.nBytesPerBlock / sizeof( WORD ) - 1;  //
			nOtherDataBlockNum = (WORD)(lpTable->tfh.wTotalBlock - nOldFatBlock + bn); //
			nNewFatBlock = (nOtherDataBlockNum + nIndexNumPerBlock - 1 ) / nIndexNumPerBlock; //
			//

            if( nNewFatBlock > nOldFatBlock )  // 新的FAT块数大于当前的吗 ？
            {	// 是，重新分配数据指针
                void *p = realloc( lpTable->pfat, nNewFatBlock * lpTable->tfh.nBytesPerBlock );
                if( p )
                    lpTable->pfat = p;
                else
                    return FALSE;
            }
			
			bn += (nNewFatBlock - nOldFatBlock); //需要增加的真实块数
			dwSize = (lpTable->tfh.wTotalBlock + bn) * lpTable->tfh.nBytesPerBlock; //数据表文件大小
			// 设置文件大小
            SetFilePointer( lpTable->hFile, dwSize, NULL, FILE_BEGIN );
            if( SetEndOfFile( lpTable->hFile ) )
            {	// 成功，alloc file block success
                WORD * pBlock = lpTable->pfat + lpTable->tfh.wTotalBlock;
                WORD i;
				// 连接新增空闲块
                for( i = 1; i < bn; i++, pBlock++ )
                    *pBlock = lpTable->tfh.wTotalBlock+i;
                *pBlock = NULL_BLOCK;
				// 将新增空闲块加入FAT
                FreeBlock( lpTable, lpTable->tfh.wTotalBlock );
				//
				// 假如可能重新分配FAT块
                if( nNewFatBlock > nOldFatBlock )
                {
                    if( ReallocBlock( lpTable, lpTable->tfh.wFatStartBlock, nNewFatBlock ) == NULL_BLOCK )
                    {   // 不应该到这里，因为之前已经扩展了文件！
                        _ASSERT( 0 );
                        return FALSE;
                    }
                }
				// 重新分配需要的块
                wStartBlock = ReallocBlock( lpTable, wFirstBlock, (WORD)num );
                lpTable->tfh.wTotalBlock += (WORD)bn; // 增加系统块总数
            }
        }
		// 设置返回值
        if( wStartBlock != NULL_BLOCK )
        {
            *lpwAlloc = wStartBlock;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {   // 释放功能，free it
        wStartBlock = ReallocBlock( lpTable, wFirstBlock, (WORD)num );        
        *lpwAlloc = NULL_BLOCK;
        return TRUE;
    }
}


// **************************************************
// 声明：static DWORD ReadBlocks( LPTABLE lpTable, WORD wStartBlock, DWORD nOffset, BYTE * lpBuf, DWORD dwSize )
// 参数：
//		IN lpTable - 数据表对象指针
//		IN wStartBlock - 开始块
//		IN nOffset - 从开始块起的偏移（byte为单位）
//	    OUT lpBuf - 用于接受数据的缓存
//		IN dwSize - 需要读的数据字节数
// 返回值：
//		实际读的字节数	
// 功能描述：
//		从数据块wStartBlock开始起的偏移nOffset字节处读取dwSize个数据
//		应该合并ReadBlocks 和 WriteBlocks 为一个函数！！
// 引用: 
//		
// ************************************************

static DWORD ReadBlocks( LPTABLE lpTable, WORD wStartBlock, DWORD nOffset, BYTE * lpBuf, DWORD dwSize )
{
    WORD nBlockSize = (WORD)lpTable->tfh.nBytesPerBlock;
    DWORD s = dwSize;
    HANDLE hFile = lpTable->hFile;
    DWORD dwRWValue;

	// 如果偏移值大于一个块大小，则到下一个块
    while( nOffset >= nBlockSize )
    {
        wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
        nOffset -= nBlockSize;
    }
	// 
    if( wStartBlock != NULL_BLOCK )
    {	// 移动文件读取位置到第一个数据位置  
        SetFilePointer( hFile, wStartBlock * nBlockSize + nOffset, NULL, FILE_BEGIN );
		
        if( dwSize > nBlockSize - nOffset ) // 需要读取的数据字节数仅仅在一个块内吗 ？
        {	// 不是，读取块内从偏移处到块尾的所有数据
            ReadFile( hFile, lpBuf, nBlockSize-nOffset, &dwRWValue, NULL );
            dwSize -= nBlockSize - nOffset;
            lpBuf += nBlockSize - nOffset;
        }
        else
        {	// 不是，读取一部分
            ReadFile( hFile, lpBuf, dwSize, &dwRWValue, NULL );
            dwSize =  0;  // 必然读完
        }       

		wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
        while( dwSize )  // 读取剩下的部分直到完成
        {
            if( wStartBlock != NULL_BLOCK )
            {	// 移动到下一个块
				WORD wNextBlock, wPrevBlock;
				DWORD dwReadSize = nBlockSize;

				wPrevBlock = wStartBlock;
				//是否是连续的块?
				do{
					wNextBlock = NEXT_BLOCK( lpTable->pfat, wPrevBlock );
					if( wNextBlock == wPrevBlock + 1 )
					{
						wPrevBlock = wNextBlock;
						dwReadSize += nBlockSize;
						continue;
					}
					break;
				}while(dwReadSize<dwSize);

                dwReadSize = MIN( dwReadSize, dwSize );
				SetFilePointer( hFile, wStartBlock * nBlockSize, NULL, FILE_BEGIN );
				ReadFile( hFile, lpBuf, dwReadSize, &dwRWValue, NULL );
                dwSize -= dwReadSize;
                lpBuf += dwReadSize;
/*
                if( dwSize >= dwReadSize )
                {
                    ReadFile( hFile, lpBuf, dwReadSize, &dwRWValue, NULL );
                    dwSize -= dwReadSize;
                    lpBuf += dwReadSize;
                }
                else
                {
                    ReadFile( hFile, lpBuf, dwSize, &dwRWValue, NULL );
                    dwSize = 0;
                }
*/
				wStartBlock = wNextBlock;

            }
            else
                break;
        }
    }
    return s - dwSize;
}

// **************************************************
// 声明：static DWORD WriteBlocks( LPTABLE lpTable, WORD wStartBlock, DWORD nOffset, LPCTSTR lpBuf, DWORD dwSize )
// 参数：
//		IN lpTable - 数据表对象指针
//		IN wStartBlock - 开始块
//		IN nOffset - 从开始块起的偏移（byte为单位）
//	    IN lpBuf - 保存有需要写入的数据缓存
//		IN dwSize - 需要写的数据字节数
// 返回值：
//		实际写的字节数	
// 功能描述：
//		从数据块wStartBlock开始起的偏移nOffset字节处写入dwSize个数据
//		应该合并ReadBlocks 和 WriteBlocks 为一个函数！！
// 引用: 
//		
// ************************************************

static DWORD WriteBlocks( LPTABLE lpTable, WORD wStartBlock, DWORD nOffset, LPCTSTR lpBuf, DWORD dwSize )
{
    WORD nBlockSize = (WORD)lpTable->tfh.nBytesPerBlock;
    HANDLE hFile = lpTable->hFile;
    DWORD s = dwSize;
    DWORD dwRWValue;

	// 如果偏移值大于一个块大小，则到下一个块

    while( nOffset >= nBlockSize )
    {  
        wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
        nOffset -= nBlockSize;
    }

    if( wStartBlock != NULL_BLOCK )
    {	// 移动文件写入位置到第一个数据位置
        SetFilePointer( hFile, wStartBlock * nBlockSize + nOffset, NULL, FILE_BEGIN );
        if( dwSize > nBlockSize - nOffset )	// 需要写入的数据字节数仅仅在一个块内吗 ？
        {	// 不是，写入块内从偏移处到块尾的所有数据
            WriteFile( hFile, lpBuf, nBlockSize - nOffset, &dwRWValue, NULL );
            dwSize -=  nBlockSize - nOffset;
            lpBuf += nBlockSize - nOffset;
        }
        else
        {	// 不是，写入一部分
            WriteFile( hFile, lpBuf, dwSize, &dwRWValue, NULL );
            dwSize =  0;
        }       

		wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
        while( dwSize ) // 写入剩下的部分直到完成
        {         
            if( wStartBlock != NULL_BLOCK )
            {	// 移动到下一个块
				WORD wNextBlock, wPrevBlock;
				DWORD dwWriteSize = nBlockSize;

				wPrevBlock = wStartBlock;
				//是否是连续的块　？
				do{
					wNextBlock = NEXT_BLOCK( lpTable->pfat, wPrevBlock );
					if( wNextBlock == wPrevBlock + 1 )
					{
						wPrevBlock = wNextBlock;
						dwWriteSize += nBlockSize;
						continue;
					}
					break;
				}while( dwWriteSize < dwSize );

                dwWriteSize = MIN( dwSize, dwWriteSize );
				SetFilePointer( hFile, wStartBlock * nBlockSize, NULL, FILE_BEGIN );
				
				WriteFile( hFile, lpBuf, dwWriteSize, &dwRWValue, NULL );
                dwSize -= dwWriteSize;
                lpBuf += dwWriteSize;

/*
                if( dwSize >= nBlockSize )
                {	
                    WriteFile( hFile, lpBuf, dwWriteSize, &dwRWValue, NULL );
                    dwSize -= dwWriteSize;
                    lpBuf += dwWriteSize;
                }
                else
                {
                    WriteFile( hFile, lpBuf, dwSize, &dwRWValue, NULL );
                    dwSize = 0;
                }
*/
				wStartBlock = wNextBlock;
            }
            else
                break;
        }
    }
    return s - dwSize;
}

// **************************************************
// 声明：static BOOL ReadFatBlocks( LPTABLE lpTable )
// 参数：
//		IN lpTable - 数据表对象指针
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		读 FAT表所在的数据块
// 引用: 
//		
// ************************************************
static BOOL ReadFatBlocks( LPTABLE lpTable )
{
    DWORD size = ( ( lpTable->tfh.wTotalBlock * sizeof( WORD ) +  lpTable->tfh.nBytesPerBlock - 1 ) / lpTable->tfh.nBytesPerBlock ) * lpTable->tfh.nBytesPerBlock;
    WORD wStartBlock;
	DWORD dwBlockSize = lpTable->tfh.nBytesPerBlock;
    BYTE * p;

    wStartBlock = lpTable->tfh.wFatStartBlock;
    p = (BYTE*)lpTable->pfat;
	if( lpTable->tfh.dwFixFileSize )
	{	//表是固定的
		DWORD dwRead;
		SetFilePointer( lpTable->hFile, wStartBlock * dwBlockSize, NULL, FILE_BEGIN );
		ReadFile( lpTable->hFile, lpTable->pfat, size, &dwRead, NULL );
		size -= dwRead;
	}
	else
	{   //表是可变的
        while( wStartBlock != NULL_BLOCK )
        {	// 读取数据表文件的FAT数据
            ReadBlocks( lpTable, wStartBlock, 0, p, dwBlockSize );
            wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
            p += dwBlockSize;
            size -= dwBlockSize;
        }
		ASSERT( size == 0 );
	}
	return !size;  //size应该为0

}



// **************************************************
// 声明：static BOOL MoveData( LPTABLE lpTable, WORD firstBlock,  long posMoveFrom, long posMoveTo )
// 参数：
//		IN lpTable - 数据表对象指针
//		IN firstBlock - 第一个块
//		IN posMoveFrom - 移动源位置
//		IN posMoveTo - 移动目标位置
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		将数据从一个位置移动到另一个位置
// 引用: 
//		
// ************************************************

static BOOL MoveData( LPTABLE lpTable, WORD firstBlock,  long posMoveFrom, long posMoveTo )
{
    WORD nBlockSize = (WORD)lpTable->tfh.nBytesPerBlock;
    DWORD dwSize;
    long totalSize, startPos, endPos, distance, rv;
    BOOL retv = FALSE;
    BYTE buf[256];


    ASSERT( posMoveFrom >= 0 && posMoveTo >= 0 );

    if( posMoveFrom > posMoveTo ) // 源位置在目标位置的后面吗 ？ 
    {	// 是
        while( 1 )
        {          
            dwSize = ReadBlocks( lpTable, firstBlock, posMoveFrom, buf, sizeof( buf ) );
            if( dwSize )
            {
                WriteBlocks( lpTable, firstBlock, posMoveTo, buf, dwSize );
                if( dwSize < sizeof( buf ) )
                    break;
            }
            else
                break;
            posMoveFrom += sizeof( buf );   // set new pos
            posMoveTo += sizeof( buf );
        }
        retv = TRUE;
    }
    else if( posMoveFrom < posMoveTo )
    {	// 源位置在目标位置的前面，从尾部读取
        WORD bn = GetBlockNum( lpTable, firstBlock );
        distance = posMoveTo - posMoveFrom;
        totalSize = bn * nBlockSize;
        startPos = totalSize - distance - sizeof( buf );
        endPos = totalSize - distance;
        startPos = MAX( startPos, posMoveFrom );

        while( 1 )
        {
            rv = endPos - startPos;
            dwSize = MIN( sizeof( buf ), rv );
            
            dwSize = ReadBlocks( lpTable, firstBlock, startPos, buf, dwSize );
            dwSize = WriteBlocks( lpTable, firstBlock, (WORD)(startPos + distance), buf, dwSize );
 
            if( dwSize < sizeof( buf ) )
                break;    // 所有数据已经完成。all data has been moved
            endPos = startPos;
            startPos -= sizeof( buf );
            startPos = MAX( startPos, posMoveFrom );
        }
        retv = TRUE;
    }
    return retv;
}

#define PSW_VALID            0x80000000    //口令数据有效
#define PSW_SAFE_TYPE_MASK   0x70000000    //加密类型，如果为0，不加密，即口令数据本身和记录数据本身都是不加密
                                           //          如果为1，默认加密方法，即口令数据本身与0xd4 XOR和记录数据与口令数据的和进行XOR
#define GET_PSW_TYPE( dwID ) ( ( (dwID) & PSW_SAFE_TYPE_MASK ) >> 28 )
// **************************************************
// 声明：static DWORD CheckPassword( LPCBYTE lpszPassword )
// 参数：
//		IN lpszEntryPassword - 用户输入口令
// 返回值：
//		返回口令的ID
// 功能描述：
//		得到口令的ID
// 引用: 
//		
// ************************************************
static BOOL CheckPassword( LPTABLE lpTable, LPCBYTE lpszEntryPassword )
{
	DWORD id = lpTable->tfh.dwPasswordID;
	if( id & PSW_VALID )
	{
		if( GET_PSW_TYPE( id ) == 0 )
		{  //明码
			if( strncmp( lpszEntryPassword, lpTable->tfh.szPassword, 16 ) == 0 )
			{   //正确
				return TRUE;
			}
			return FALSE;
		}
		else if( GET_PSW_TYPE( id ) == 1 )
		{
			BYTE szPassword[MAX_PASSWORD+1];
			int i;
			BYTE bEncryptKey = 0;
			memcpy( szPassword, lpTable->tfh.szPassword, 16 );
			szPassword[MAX_PASSWORD] = 0;
			for( i = 0; i < MAX_PASSWORD && szPassword[i]; i++ )
			{
				szPassword[i] ^= 0xd4;
				bEncryptKey += szPassword[i];
			}
			if( strncmp( lpszEntryPassword, szPassword, 16 ) == 0 )
			{
				lpTable->bIsEncrypt = TRUE;
				lpTable->bEncryptKey = bEncryptKey;
				return TRUE;//正确
			}
			return FALSE;
		}
		return FALSE;
	}
	//口令无效
	return TRUE;	
}

// **************************************************
// 声明：static WORD GetCheckSum( LPTABLE lpTable )
// 参数：
//		IN lpTable - 数据表对象指针
// 返回值：
//		返回数据表的checksum
// 功能描述：
//		得到数据表的checksum
// 引用: 
//		
// ************************************************
static WORD GetCheckSum( TABLE_FILE_HEAD * ptfh )
{
	LPBYTE lpbCheckSum = (LPBYTE)&ptfh->wCheckSum + sizeof(ptfh->wCheckSum);
	int n = sizeof(TABLE_FILE_HEAD) - ( lpbCheckSum - (LPBYTE)ptfh );
	WORD wCheckSum = 0;
	while( n-- )
	{
		wCheckSum += *lpbCheckSum++;
	}
	return wCheckSum;
}

// **************************************************
// 声明：static BOOL UpdateTableHead( LPTABLE lpTable )
// 参数：
//		IN lpTable - 数据表对象指针
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		更新数据表头
// 引用: 
//		
// ************************************************

static BOOL UpdateTableHead( LPTABLE lpTable )
{
	//WORD wCheckSum = ptfh->wCheckSum;
	//LPBYTE lpbCheckSum = (LPBYTE)&ptfh->wCheckSum + sizeof(ptfh->wCheckSum);
	//int n = sizeof(TABLE_FILE_HEAD) - ( lpbCheckSum - (LPBYTE)ptfh );
	//while( n-- )
	//{
	//	wCheckSum -= *lpbCheckSum++;
	//}
	lpTable->tfh.wCheckSum = GetCheckSum( &lpTable->tfh );

	return WriteBlocks( lpTable, 0, 0, (BYTE*)&lpTable->tfh, sizeof( lpTable->tfh ) );
}

// **************************************************
// 声明：static BOOL UpdateNodeIndex( LPTABLE lpTable )
// 参数：
//		IN lpTable - 数据表对象指针
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		更新记录节点数据到文件
// 引用: 
//		
// ************************************************

static BOOL UpdateNodeIndex( LPTABLE lpTable )
{
    return WriteBlocks( lpTable, 
                 lpTable->tfh.wNodeIndexStartBlock, 
                 sizeof( RNODEINDEX ) * (lpTable->dwRN-1), 
                 (BYTE*)&lpTable->RNodeIndex, 
                 sizeof( RNODEINDEX ) );
}

// **************************************************
// 声明：static BOOL UpdateNodeData( LPTABLE lpTable )
// 参数：
//		IN lpTable - 数据表对象指针
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		更新记录数据到文件		
// 引用: 
//		
// ************************************************

static BOOL UpdateNodeData( LPTABLE lpTable )
{
    return WriteBlocks( lpTable, 
                        lpTable->RNodeIndex.wDataStartBlock, 
                        0, 
                        (BYTE*)lpTable->pRNodeData, 
                        lpTable->tfh.nNodeDataSize );
}

// **************************************************
// 声明：static BOOL _CopyRecordData( LPTABLE lpTable, FIELDITEM fieldItem[], DWORD size, BOOL bFrag, HANDLE hOwnerProc )
// 参数：
//		IN lpTable - 数据表对象指针
//		IN fieldItem - FIELDITEM 结构指针
//		IN size - 存放数据的字节数。假如参数bFrag为FALSE, 该参数有效, fieldItme[0].lpData指向存放的数据。
//		IN bFrag - 是否所有字段之间的数据是连续的。连续是指满足以下关系：
//				fieldItem[n-1].lpData + fieldItem[n-1].size = fieldItem[n].lpData
//		IN hOwnerProc - 拥有者进程
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		更新记录数据。假如bFrag为FALSE,会提高系统性能		
// 引用: 
//		
// ************************************************

static BOOL _CopyRecordData( LPTABLE lpTable, FIELDITEM fieldItem[], DWORD size, BOOL bFrag, HANDLE hOwnerProc )
{
    BOOL retv = FALSE;
    WORD wDataStartBlock = lpTable->RNodeIndex.wDataStartBlock;
    WORD nOffset = lpTable->tfh.nNodeDataSize;
    int i;

    if( bFrag == FALSE )
    {   // 连续的段数据，写一次。no fragment, so write once
        if( WriteBlocks( lpTable, wDataStartBlock, nOffset, MapPtrToProcess( fieldItem[0].lpData, hOwnerProc ), size ) )
            retv = TRUE;
    }
    else
    {   // 字段是非连续的。写每一个字段。has fragmental
        DWORD pointer = nOffset;
        retv = TRUE;
        for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
        {
            DWORD dwSize = WriteBlocks( lpTable, wDataStartBlock, pointer, MapPtrToProcess( fieldItem[i].lpData, hOwnerProc), fieldItem[i].size );
            if( dwSize != fieldItem[i].size )
            {
				WARNMSG( 1, ( "_CopyRecordData error: not write enough data!, data size=%d, writed size=%d.\r\n", fieldItem[i].size, dwSize ) );
                retv = FALSE;
                break;
            }
            pointer += fieldItem[i].size;
        }
    }

    ASSERT( lpTable->tfh.nFieldNum < 31 );
	// 更新字段信息
    for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
    {
        lpTable->pRNodeData->nFieldSize[i] = fieldItem[i].size;
    }
    retv = UpdateNodeData( lpTable );	//写入文件
    return retv;
}


// **************************************************
// 声明：static BOOL _InsertRecord( LPTABLE lpTable, WORD rn, FIELDITEM fieldItem[] )
// 参数：
//		IN lpTable - 数据表对象
//		IN rn - 将记录插入的记录号
//		IN fieldItem - 记录数据
//		IN hOwnerProc - 拥有者进程
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		向数据表插入一条记录
// 引用: 
//		
// ************************************************

static BOOL _InsertRecord( LPTABLE lpTable, WORD rn, FIELDITEM fieldItem[], HANDLE hOwnerProc )
{
    WORD wDataBlock;
    long size;
    BOOL retv = FALSE;
    int i;
    BOOL bFrag;
    BYTE *lpData;

    // 统计大小及数据是否是连续的 count size and has fragment ?
    size = 0; lpData = (BYTE*)fieldItem[0].lpData;
    bFrag = FALSE;
    for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
    {
        size += fieldItem[i].size;
        if( lpData == fieldItem[i].lpData )
            lpData += fieldItem[i].size;
        else
            bFrag = TRUE;  // 非连续的 is fragmental
    }

    wDataBlock = NULL_BLOCK;
	// 重新分配记录数据所占空间
    ReallocFile( lpTable, NULL_BLOCK, size+lpTable->tfh.nNodeDataSize, &wDataBlock );
    if( wDataBlock != NULL_BLOCK )
    {    
        WORD firstBlock;
		// 重新分配记录索引所占的空间
        if( ReallocFile( lpTable, lpTable->tfh.wNodeIndexStartBlock, (lpTable->tfh.nNodeNum + 1) * sizeof( RNODEINDEX ), &firstBlock )  )
        {	// 重分配成功，因为是插入数据，需要移动记录索引。
            lpTable->tfh.wNodeIndexStartBlock = firstBlock;
            if( MoveData(lpTable, lpTable->tfh.wNodeIndexStartBlock, (rn-1)*sizeof( RNODEINDEX ), rn*sizeof( RNODEINDEX ) ) )
            {
                lpTable->tfh.nNodeNum++;
                lpTable->dwRN = rn;  // set as current record
				memset( &lpTable->RNodeIndex, 0, sizeof( RNODEINDEX ) );
                lpTable->RNodeIndex.wDataStartBlock = wDataBlock;
				//	更新记录索引数据
                UpdateNodeIndex( lpTable );
				// 拷贝记录数据
                if( _CopyRecordData( lpTable, fieldItem, size, bFrag, hOwnerProc ) )
                    return TRUE;
            }
        }
		//	索引空间分配不成功，释放之前分配的记录数据空间
        FreeBlock( lpTable, wDataBlock );
    }
    return retv;
}

// **************************************************
// 声明：static BOOL _Exchange( LPTABLE lpTable, WORD rn0, WORD rn1 )
// 参数：
//		IN lpTable - 数据表对象指针 
//		IN rn0 - 第一条记录
//		IN rn1 - 第二条记录
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		交换两条记录之间的数据		
// 引用: 
//		
// ************************************************

static BOOL _Exchange( LPTABLE lpTable, WORD rn0, WORD rn1 )
{
//    WORD block0, block1;
    RNODEINDEX nodeIndex0, nodeIndex1;
	// 读取第一条记录的索引数据
    ReadBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, rn0 * sizeof( RNODEINDEX ), (BYTE*)&nodeIndex0, sizeof( RNODEINDEX ) );
    // 读取第二条记录的索引数据
	ReadBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, rn1 * sizeof( RNODEINDEX ), (BYTE*)&nodeIndex1, sizeof( RNODEINDEX ) );
	// 将第一条记录的索引数据写入第二条记录
    WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, rn0 * sizeof( RNODEINDEX ), (BYTE*)&nodeIndex1, sizeof( RNODEINDEX ) );
    // 将第二条记录的索引数据写入第一条记录
	WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, rn1 * sizeof( RNODEINDEX ), (BYTE*)&nodeIndex0, sizeof( RNODEINDEX ) );

    return TRUE;
}

// **************************************************
// 声明：static BOOL InitFat( LPTABLE lpTable )
// 参数：
//		IN lpTable - 数据表对象指针 
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		初始化数据表的FAT		
// 引用: 
//		
// ************************************************

static BOOL InitFat( LPTABLE lpTable )
{	// 确定FAT的大小
    DWORD size = ( ( lpTable->tfh.wTotalBlock * sizeof( WORD ) +  lpTable->tfh.nBytesPerBlock - 1 ) / lpTable->tfh.nBytesPerBlock ) * lpTable->tfh.nBytesPerBlock;
    lpTable->pfat = malloc( size );
    if( lpTable->pfat )
    {
		if( ReadFatBlocks( lpTable ) == FALSE )
		{
	        free( lpTable->pfat );
			lpTable->pfat = NULL;
		}
	}
	return lpTable->pfat != NULL;
/*
        WORD wBlock;
        BYTE * p;
        wBlock = lpTable->tfh.wFatStartBlock;
        p = (BYTE*)lpTable->pfat;
		if( wBlock != NULL_BLOCK )
		    size -= ReadBlocks( lpTable, wBlock, 0, p, size );

		//if( lpTable->tfh.dwFixFileSize == 0 )
		//{	//可变大小

		//	{	// 读取数据表文件的FAT数据
				//2004-06-18, remove
				//ReadBlocks( lpTable, wBlock, 0, p, lpTable->tfh.nBytesPerBlock );
				//wBlock = NEXT_BLOCK( lpTable->pfat, wBlock );
				//p += lpTable->tfh.nBytesPerBlock;
				//size -= lpTable->tfh.nBytesPerBlock;
				//2004-06-18, add
				
				//
		//	}
		//}
		//else
		//{	//固定大小
		//}

        if( size == 0 )
        {	// 读完，成功
            return TRUE;
        }
		// 读错误，释放FAT指针
        free( lpTable->pfat );
		lpTable->pfat = NULL;
*/
//    }
//    return FALSE;
}

// **************************************************
// 声明：static BOOL IsValidTable( TABLE_FILE_HEAD * ptfh )
// 参数：
//		IN ptfh - 数据表文件头
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		检查数据表是否有效
// 引用: 
//		
// ************************************************

static BOOL IsValidTable( TABLE_FILE_HEAD * ptfh )
{
    if( memcmp( ptfh->idName, id, sizeof( ptfh->idName ) ) == 0 )
    {		
        return GetCheckSum( ptfh ) == ptfh->wCheckSum;	//有效 ?
    }
    SetLastError( ERROR_INVALID_DATA );
    return FALSE;
}

// **************************************************
// 声明：static void _AddToTableList( LPTABLE_FILE lpTableFile )
// 参数：
//		IN lpTableFile - 数据表文件机构
// 返回值：
//		无
// 功能描述：
//		将打开的数据表对象加入系统链表
// 引用: 
//		
// ************************************************

static void _AddToTableList( LPTABLE_FILE lpTableFile )
{
    EnterCriticalSection( &csTableList );

	lpTableFile->hOwnerProc = GetCallerProcess(); // 拥有者进程
	lpTableFile->lpNext = lpTableList;
	lpTableList = lpTableFile;
	InterlockedIncrement( &lpTableFile->lpTable->dwRefCount ); // 增加对数据表节点的引用

	LeaveCriticalSection( &csTableList );
}

// **************************************************
// 声明：static void _RemoveFromTableList( LPTABLE_FILE lpTableFile )
// 参数：
//		IN lpTableFile - 数据表文件机构
// 返回值：
//		无
// 功能描述：
//		与_AddToTableList相反，将数据表对象移出系统链表
// 引用: 
//		
// ************************************************

static void _RemoveFromTableList( LPTABLE_FILE lpTableFile )
{
	LPTABLE_FILE lpPrev, lpObj;

    EnterCriticalSection( &csTableList );

	lpObj = lpTableList;
	lpPrev = NULL;
	while( lpObj )
	{
		if( lpObj == lpTableFile )
		{
			if( lpPrev == NULL )
			{   //first obj
				lpTableList = lpObj->lpNext;
			}
			else
				lpPrev->lpNext = lpObj->lpNext; 
			InterlockedDecrement( &lpTableFile->lpTable->dwRefCount );// 减少对数据表节点的引用
			break;
		}
		lpPrev = lpObj;
		lpObj = lpObj->lpNext;
	}

	LeaveCriticalSection( &csTableList );
}

// **************************************************
// 声明：static BOOL MakeFixTable( 
//								LPTABLE lpTable,
//								DWORD dwTableFileSize
//								)
// 参数：
//		IN lpTable - 数据表对象
//		DWORD dwTableFileSize - 需要固定的文件大小
// 返回值：
//		成功，返回TRUE; 失败，返回FALSE
// 功能描述：
//		生成固定文件大小的数据表
// 引用: 
//		
// ************************************************

static BOOL MakeFixTable( LPTABLE lpTable, DWORD dwTableFileSize )
{
	DWORD nIndexPerBlocks;
	DWORD nFatBlocks;
	DWORD dwBlockSize = BLOCK_SIZE;	//128

	if( dwTableFileSize / dwBlockSize >= 0xfff0 )
	{	//块尺寸太小
		dwBlockSize *= 2;	//256		
		if( dwTableFileSize / dwBlockSize >= 0xfff0 )
		{	//块尺寸太小
			dwBlockSize *= 2;	//512
			if( dwTableFileSize / dwBlockSize >= 0xfff0 )
			{	//块尺寸太小
				return FALSE;	//不支持
			}
		}
	}
	lpTable->tfh.nBytesPerBlock = (WORD)dwBlockSize;

	lpTable->tfh.dwFixFileSize = ( (dwTableFileSize + dwBlockSize-1) / dwBlockSize ) * dwBlockSize;
	lpTable->tfh.wTotalBlock = (WORD)(lpTable->tfh.dwFixFileSize / dwBlockSize);//(sizeof( lpTable->tfh ) + BLOCK_SIZE - 1) / BLOCK_SIZE + 1 + 1;
	lpTable->tfh.wFatStartBlock = (sizeof( lpTable->tfh ) + dwBlockSize - 1) / dwBlockSize;//lpTable->tfh.wTotalBlock-2;

    nIndexPerBlocks = lpTable->tfh.nBytesPerBlock / sizeof( WORD );
	nFatBlocks = (lpTable->tfh.wTotalBlock + nIndexPerBlocks - 1) / nIndexPerBlocks;  //

	lpTable->tfh.wNodeIndexStartBlock = (WORD)(lpTable->tfh.wFatStartBlock + nFatBlocks);	//一个索引块

	lpTable->tfh.wFreeBlock = lpTable->tfh.wNodeIndexStartBlock + 1;//NULL_BLOCK;
	lpTable->tfh.nFreeCount = lpTable->tfh.wTotalBlock - lpTable->tfh.wNodeIndexStartBlock;
	
	//分配文件
	SetFilePointer( lpTable->hFile, lpTable->tfh.dwFixFileSize, NULL, FILE_BEGIN );
	if( SetEndOfFile( lpTable->hFile ) )
	{	//分配文件空间成功
		SetFilePointer( lpTable->hFile, 0, NULL, FILE_BEGIN );
		
		//lpTable->pRNodeData = malloc( lpTable->tfh.nNodeDataSize );
		//if( lpTable->pRNodeData )
		{
			//DWORD nIndexPerBlocks = lpTable->tfh.nBytesPerBlock / sizeof( WORD );
			//DWORD nFatBlocks = (lpTable->tfh.wTotalBlock + nIndexPerBlocks - 1) / nIndexPerBlocks;  //
			DWORD dwFatSize = dwBlockSize * nFatBlocks;
			
			lpTable->pfat = malloc( dwFatSize );
			if( lpTable->pfat )
			{
				UINT i;
				DWORD nBlock;// = lpTable->tfh.wFreeBlock;
				memset( lpTable->pfat, 0, dwFatSize );

				//将索引块清为０
				i = WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)lpTable->pfat, dwBlockSize );
				
				//初始化表头块
				nBlock = lpTable->tfh.wFatStartBlock-1;
				for( i = 0; i < nBlock; i++ )
					*(lpTable->pfat+i) = i + 1;//连接所有的空闲块
				*(lpTable->pfat+i) = NULL_BLOCK;//最后一个块
				i++;
				
				//初始化FAT表块
				nBlock = lpTable->tfh.wNodeIndexStartBlock-1;
				for( ; i < nBlock; i++ )
					*(lpTable->pfat+i) = i + 1;//连接所有的空闲块
				*(lpTable->pfat+i) = NULL_BLOCK;//最后一个块
				i++;
				
				//初始化索引表块
				nBlock = lpTable->tfh.wFreeBlock - 1;
				for( ; i < nBlock; i++ )
					*(lpTable->pfat+i) = i + 1;//连接所有的空闲块
				*(lpTable->pfat+i) = NULL_BLOCK;//最后一个块
				i++;

				//初始化空闲表块
				nBlock = lpTable->tfh.wTotalBlock - 1;
				for( ; i < nBlock; i++ )
					*(lpTable->pfat+i) = i + 1;//连接所有的空闲块
				*(lpTable->pfat+i) = NULL_BLOCK;//最后一个块
				i++;

				// 更新FAT表数据
				i = WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, dwFatSize );
				if( i )
				{
					UpdateTableHead( lpTable );
					return TRUE;
				}
				// 
				// 失败，清除
				free( lpTable->pfat );
				lpTable->pfat = NULL;
			}
			free( lpTable->pRNodeData );
			lpTable->pRNodeData = NULL;
		}
	}

	return FALSE;
}

// **************************************************
// 声明：static BOOL MakeFixTable( 
//								LPTABLE lpTable
//								)
// 参数：
//		IN lpTable - 数据表对象
// 返回值：
//		成功，返回TRUE; 失败，返回FALSE
// 功能描述：
//		生成可变大小的数据表
// 引用: 
//		
// ************************************************

static BOOL MakeVariableTable( LPTABLE lpTable )
{
	//memcpy( lpTable->tfh.idName, id, sizeof(lpTable->tfh.idName) );
	//lpTable->tfh.ver = TABLE_VERSION;
	//lpTable->tfh.nNodeNum = 0;
	//lpTable->tfh.nFieldNum = nFieldNum;
	//lpTable->tfh.nNodeDataSize = RNODEDATA_SIZE( nFieldNum );
	//lpTable->tfh.nBytesPerBlock = BLOCK_SIZE;
	//lpTable->dwRN = 0;
	DWORD dwBlockSize = BLOCK_SIZE;

	lpTable->tfh.nBytesPerBlock = (WORD)dwBlockSize;

	lpTable->tfh.dwFixFileSize = 0;//( (dwTableFileSize + BLOCK_SIZE-1) / BLOCK_SIZE ) * BLOCK_SIZE;
	lpTable->tfh.wTotalBlock = (sizeof( lpTable->tfh ) + dwBlockSize - 1) / dwBlockSize + 1 + 1;
	lpTable->tfh.wFatStartBlock = lpTable->tfh.wTotalBlock-2;
	lpTable->tfh.wFreeBlock = NULL_BLOCK;
	lpTable->tfh.nFreeCount = 0;		
	lpTable->tfh.wNodeIndexStartBlock = lpTable->tfh.wTotalBlock-1;
	
	
	//lpTable->pRNodeData = malloc( lpTable->tfh.nNodeDataSize );
	//if( lpTable->pRNodeData )
	{
		lpTable->pfat = malloc( dwBlockSize );
		if( lpTable->pfat )
		{
			UINT i;
			memset( lpTable->pfat, 0, dwBlockSize );
			//将索引块清为０
			WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)lpTable->pfat, dwBlockSize );
			//

			for( i = 0; i < lpTable->tfh.wTotalBlock; i++ )
				*(lpTable->pfat+i) = NULL_BLOCK;
			// 更新FAT表数据
			i = WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, dwBlockSize );
			
			if( i )
			{
				UpdateTableHead( lpTable );
				return TRUE;
			}			
			// 
			// 失败，清除
			free( lpTable->pfat );
			lpTable->pfat = NULL;
		}
		free( lpTable->pRNodeData );
		lpTable->pRNodeData = NULL;
	}
	return FALSE;
}

// **************************************************
// 声明：static LPTABLE OpenDataTable( 
//                 LPCTSTR lpcszFileName,
//                 DWORD dwAccess,
//                 DWORD dwShareMode, 
//                 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
//                 DWORD dwCreate, 
//                 UINT nFieldNum,
//				   DWORD dwFlagAndAttr	 )
// 参数：
//		IN lpcszFileName - 文件文件名
//		IN dwAccess - 存取模式（参考CreateFile说明）
//		IN dwShareMode - 共享模式（参考CreateFile说明）
//		IN lpSecurityAttributes - 安全属性（参考CreateFile说明）
//		IN dwCreate - 创建/打开模式（参考CreateFile说明）
//		IN nFieldNum - 字段数
//		IN dwTableFileSize - 固定文件大小，如果为0，不固定
//		IN dwFlagAndAttr - 文件标志和属性
// 返回值：
//		无
// 功能描述：
//		打开数据表
// 引用: 
//		
// ************************************************

static LPTABLE OpenDataTable( 
                 LPCTSTR lpcszFileName,
                 DWORD dwAccess,
                 DWORD dwShareMode, 
                 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
                 DWORD dwCreate, 
                 UINT nFieldNum,
				 DWORD dwTableFileSize,
				 DWORD dwFlagAndAttr )

{    
    BOOL retv = FALSE;
	BOOL bNewFile;
	LPTABLE lpTable = NULL;
	HANDLE hFile;
	DWORD size;
	// 打开文件
	hFile = CreateFile( lpcszFileName, dwAccess, dwShareMode, NULL, dwCreate, dwFlagAndAttr, NULL );
    if( hFile == INVALID_HANDLE_VALUE )
		goto _return;
	// 分配表结构并初始化
	lpTable = malloc( sizeof( TABLE ) + strlen( lpcszFileName ) + 1 );
	if( lpTable == NULL )
		goto _return;
	memset( lpTable, 0, sizeof( TABLE ) );
	lpTable->lpszFileName = (LPTSTR)(lpTable+1);
	strcpy( lpTable->lpszFileName, lpcszFileName );
    lpTable->hFile = hFile;

	InitializeCriticalSection( &lpTable->csTable );

    size = GetFileSize( hFile, NULL );
	bNewFile = (size == 0 ? TRUE : FALSE );		

	if( bNewFile )
	{	// 新数据表
//	    WORD i;
/*
		memcpy( lpTable->tfh.idName, id, sizeof(lpTable->tfh.idName) );
		lpTable->tfh.ver = TABLE_VERSION;
		lpTable->tfh.nNodeNum = 0;
		lpTable->tfh.nFieldNum = nFieldNum;
		lpTable->tfh.nNodeDataSize = RNODEDATA_SIZE( nFieldNum );
		lpTable->tfh.nBytesPerBlock = BLOCK_SIZE;
		lpTable->tfh.dwFixFileSize = ( (dwTableFileSize + BLOCK_SIZE-1) / BLOCK_SIZE ) * BLOCK_SIZE;

		lpTable->tfh.wTotalBlock = (sizeof( lpTable->tfh ) + BLOCK_SIZE - 1) / BLOCK_SIZE + 1 + 1;
		lpTable->tfh.wFatStartBlock = lpTable->tfh.wTotalBlock-2;
		lpTable->tfh.wFreeBlock = NULL_BLOCK;
		lpTable->tfh.nFreeCount = 0;		
		lpTable->tfh.wNodeIndexStartBlock = lpTable->tfh.wTotalBlock-1;
			
		lpTable->dwRN = 0;
		lpTable->pRNodeData = malloc( lpTable->tfh.nNodeDataSize );
		if( lpTable->pRNodeData )
		{
			lpTable->pfat = malloc( BLOCK_SIZE );
			if( lpTable->pfat )
			{
				memset( lpTable->pfat, 0, BLOCK_SIZE );
				for( i = 0; i < lpTable->tfh.wTotalBlock; i++ )
					*(lpTable->pfat+i) = NULL_BLOCK;
					// 更新FAT表数据
				retv = WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, BLOCK_SIZE );
				if( retv )
				{
					// 2004-02-01
					//WriteBlocks( lpTable, 0, 0, (BYTE*)&lpTable->tfh  , sizeof( lpTable->tfh ) );
					UpdateTableHead( lpTable );
					retv = TRUE;
					goto _return;  // it's all ok
				}					
				// 
				// 失败，清除
				free( lpTable->pfat );
				lpTable->pfat = NULL;
			}
			free( lpTable->pRNodeData );
			lpTable->pRNodeData = NULL;
		}
*/
		memcpy( lpTable->tfh.idName, id, sizeof(lpTable->tfh.idName) );
		lpTable->tfh.ver = TABLE_VERSION;
		lpTable->tfh.nNodeNum = 0;
		lpTable->tfh.nFieldNum = nFieldNum;
		lpTable->tfh.nNodeDataSize = RNODEDATA_SIZE( nFieldNum );
		//lpTable->tfh.nBytesPerBlock = BLOCK_SIZE;
		lpTable->dwRN = 0;
		lpTable->pRNodeData = malloc( lpTable->tfh.nNodeDataSize );
		if( lpTable->pRNodeData )
		{			
			if( dwTableFileSize )
			{
				retv = MakeFixTable( lpTable, dwTableFileSize );
			}
			else
			{
				retv = MakeVariableTable( lpTable );
			}
			if( retv == FALSE )
			{
				free( lpTable->pRNodeData );
				lpTable->pRNodeData = NULL;
			}
		}
		lpTable->bFlush = TRUE;
	}
	else
	{   // 文件已经存在，old file
		DWORD dwSize;
		ReadFile( hFile, &lpTable->tfh, sizeof( lpTable->tfh ), &dwSize, NULL );
		// 检查是否有效
		if( IsValidTable( &lpTable->tfh ) &&
			(nFieldNum == 0 || lpTable->tfh.nFieldNum == nFieldNum) )
		{	// 有效
			BOOL bIsSafe = TRUE;
			if( lpSecurityAttributes )
			{   //需要做安全检查
				bIsSafe = CheckPassword( lpTable, lpSecurityAttributes->lpSecurityDescriptor );
			}
			if( bIsSafe )
			{
				lpTable->pRNodeData = malloc( lpTable->tfh.nNodeDataSize );
				if( lpTable->pRNodeData )
				{	// 将FAT从文件读入
					if( InitFat( lpTable ) == FALSE )
					{	//失败
						free( lpTable->pRNodeData );
						lpTable->pRNodeData = NULL;
					}
					else
						retv = TRUE;
				}
			}
		}		
	}
_return:
	if( retv == FALSE )
	{	// 如果失败，将之前分配的数据清除
		if( lpTable )
		{
			DeleteCriticalSection( &lpTable->csTable );
			free( lpTable );
		}
		if( hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle( hFile );
			if( bNewFile )
			{	//新创建的文件,删除它
				DeleteFile( lpcszFileName );
			}
		}
		lpTable = NULL;
	}
	return lpTable;
}

// **************************************************
// 声明：static BOOL CloseDataTable( LPTABLE lpTable )
// 参数：
//		IN lpTable - 数据表对象 
// 返回值：
//		假如成功，返回TRUE; 否则，失败
// 功能描述：
//		关闭数据表
// 引用: 
//		
// ************************************************

static BOOL CloseDataTable( LPTABLE lpTable )
{
	if( lpTable->dwRefCount == 0 )
	{	// 没有任何其它对象引用该对象
		CloseHandle( lpTable->hFile );
		free( lpTable->pRNodeData );
		free( lpTable->pfat );
		DeleteCriticalSection( &lpTable->csTable );
		free( lpTable );
	}

	return TRUE;
}

// **************************************************
// 声明：static LPTABLE_FILE FindTableFile( LPCTSTR lpszFileName )
// 参数：
//		IN lpszFileName - 文件名
// 返回值：
//		进入成功，返回表文件指针；否则，返回NULL
// 功能描述：
//		根据文件名查找系统当前是否有已经打开的数据表对象
// 引用: 
//		
// ************************************************

static LPTABLE_FILE FindTableFile( LPCTSTR lpszFileName )
{
	LPTABLE_FILE lpObj;

	lpObj = lpTableList;
	while( lpObj )
	{
		if( stricmp( lpszFileName, lpObj->lpTable->lpszFileName ) == 0 )
		{  // 找到。yes, find it
			break;
		}
		lpObj = lpObj->lpNext;
	}

	return lpObj;
}

#define LRF_UPDATE_RECORD  0x1
#define LRF_SET_FLUSH_FLAG  0x2

// **************************************************
// 声明：static BOOL LockRecord( LPTABLE_FILE lpTableFile, UINT uLockFlag )
// 参数：
//		IN lpTableFile - 数据表文件对象
//		IN uLockFlag - 功能，当前为：是否更新数据表对象数据 或 设置刷性标志
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		锁住当前记录
// 引用: 
//		
// ************************************************

static BOOL LockRecord( LPTABLE_FILE lpTableFile, UINT uLockFlag )
{
	EnterCriticalSection( &lpTableFile->lpTable->csTable );
	if( (lpTableFile->lpTable->dwRN != lpTableFile->rn) &&
		(uLockFlag & LRF_UPDATE_RECORD) )
	{	// 需要更新当前记录
		LPTABLE lpTable = lpTableFile->lpTable;
		DWORD rn = lpTableFile->rn;
        if( rn <= 0 )
            rn = 1;
        else if( rn > lpTable->tfh.nNodeNum )
		{	// 无效
			LeaveCriticalSection( &lpTableFile->lpTable->csTable );
			return FALSE;
		}
		// 读取当前记录信息
        lpTableFile->rn = _SetRecordPointerAndUpdate( lpTableFile->lpTable, rn );
	}
	if( uLockFlag & LRF_SET_FLUSH_FLAG )
	{
		lpTableFile->lpTable->bFlush = TRUE;
	}
	return TRUE;
}

// **************************************************
// 声明：static BOOL UnlockRecord( LPTABLE_FILE lpTableFile )
// 参数：
//		IN lpTableFile - 数据表文件对象
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		与LockRecord相反，锁住当前记录
// 引用: 
//		
// ************************************************

static BOOL UnlockRecord( LPTABLE_FILE lpTableFile )
{
	LeaveCriticalSection( &lpTableFile->lpTable->csTable );
	return TRUE;
}

// **************************************************
// 声明：BOOL WINAPI Table_CloseAll( HANDLE hProcess )
// 参数：
//		IN hProcess - 当前正关闭的进程
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		关闭进程打开的数据表
// 引用: 
//		
// ************************************************

BOOL WINAPI Table_CloseAll( HANDLE hProcess )
{
	LPTABLE_FILE lpNext, lpObj;

	EnterCriticalSection( &csTableList ); // 进入冲突段
	lpObj = lpTableList;
	while( lpObj )
	{
		lpNext = lpObj->lpNext;
		if( lpObj->hOwnerProc == hProcess )
		{	// 找到
#ifdef __DEBUG
		    WARNMSG( 1, (TEXT(" not close table handle: 0x%x\r\n"), lpObj ) );
#endif
			Table_Close( (HTABLE)lpObj );//关闭
		}
		lpObj = lpNext;
	}
	LeaveCriticalSection( &csTableList ); // 离开冲突段

	return TRUE;
}

// **************************************************
// 声明：BOOL TableAutoHandle( BOOL bShutDown )
// 参数：
//		IN bShutDown- 是否硬件断电
// 返回值： 
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		系统关闭，刷新所有的数据，如果是 shutdown 功能，关闭table 文件和 服务
// 引用: 
//		
// ************************************************

BOOL TableAutoHandle( BOOL bShutDown )
{
	LPTABLE_FILE lpObj;

	EnterCriticalSection( &csTableList ); // 进入冲突段
	lpObj = lpTableList;
	while( lpObj )
	{
		// 找到
		if( bShutDown )
		{
#ifdef __DEBUG
	    WARNMSG( 1, (TEXT(" not close table handle: 0x%x\r\n"), lpObj ) );
#endif

			Table_Close( (HTABLE)lpObj );//关闭
			lpObj = lpTableList;
		}
		else
		{			
			Table_Flush( (HTABLE)lpObj );	//仅仅刷新
			lpObj = lpObj->lpNext;
		}		
	}
	LeaveCriticalSection( &csTableList ); // 离开冲突段

	return TRUE;
}

// **************************************************
// 声明：BOOL WINAPI Table_PowerHandler( BOOL bShutDown )
// 参数：
//		IN bShutDown- 是否硬件断电
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		系统关闭，刷新所有的数据，如果是 shutdown 功能，关闭table 文件和 服务
// 引用: 
//		
// ************************************************

BOOL WINAPI Table_PowerHandler( BOOL bShutDown )
{
	return TableAutoHandle( bShutDown );
}



// **************************************************
// 声明：BOOL CheckShare( DWORD dwOldShare0, DWORD dwNewShare1, DWORD dwAccess )
// 参数：
//		IN dwOldShare0 - 已打开文件共享模式 	
//		IN dwNewShare1 - 将要打开文件共享模式
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		检查两个共享模式是否冲突
// 引用: 
//		
// ************************************************

static BOOL CheckShare( DWORD dwOldShare0, DWORD dwNewShare1, DWORD dwAccess )
{
	if( dwAccess & GENERIC_READ )
		dwNewShare1 |= FILE_SHARE_READ;
	if( dwAccess & GENERIC_WRITE )
		dwNewShare1 |= FILE_SHARE_WRITE;


	if( dwOldShare0 && 
		dwOldShare0 == dwNewShare1 )
		return TRUE;
	return FALSE;
}

// **************************************************
// 声明：HTABLE WINAPI Table_Create( 
//                 LPCTSTR lpcszFileName, 
//                 DWORD dwAccess, 
//                 DWORD dwShareMode, 
//                 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
//                 DWORD dwCreate, 
//                 UINT nFieldNum )
// 参数：
//		IN lpcszFileName - 需要打开/创建的文件名  
//      IN dwAccess - 存取模式（参看CreateFile）
//      IN dwShareMode - 共享模式（参看CreateFile）
//      IN lpSecurityAttributes - 安全属性（参看CreateFile）
//      IN dwCreate - 创建/打开模式（参看CreateFile）
//      IN nFieldNum - 字段数
// 返回值：
//		假如成功，返回数据表句柄	
// 功能描述：
//		创建数据表对象
// 引用: 
//		系统API
// ************************************************
/*
HTABLE WINAPI Table_Create( 
                 LPCTSTR lpcszFileName, 
                 DWORD dwAccess, 
                 DWORD dwShareMode, 
                 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
                 DWORD dwCreate, 
                 UINT nFieldNum,
				 DWORD dwTableFileSize )
*/
HTABLE WINAPI Table_CreateByStruct( LPCREATE_TABLE lpct )
{
	LPCTSTR lpcszFileName;
	DWORD dwAccess; 
	DWORD dwShareMode;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes;
	DWORD dwCreate;
	UINT nFieldNum;
	DWORD dwTableFileSize;
	DWORD dwFlagAndAttr;

    LPTABLE_FILE lpTableFile;
	LPTABLE_FILE lpOldTableFile;
	LPTABLE lpTable = NULL;

	if( lpct->dwSize != sizeof(CREATE_TABLE) )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return INVALID_HANDLE_VALUE;
	}

	lpcszFileName = (LPCTSTR)MapPtrToProcess( (LPVOID)lpct->lpcszFileName, GetCallerProcess() );
	dwAccess = lpct->dwAccess; 
	dwShareMode = lpct->dwShareMode;
	lpSecurityAttributes = (LPSECURITY_ATTRIBUTES)MapPtrToProcess( (LPVOID)lpct->lpSecurityAttributes, GetCallerProcess() );
	dwCreate = lpct->dwCreate;
	nFieldNum = lpct->nFieldNum;
	dwTableFileSize = lpct->dwFileSizeLimit;
	dwFlagAndAttr = lpct->dwFlagsAndAttributes;

	lpTableFile = malloc( sizeof( TABLE_FILE ) );

    if( lpTableFile  )
    {
		memset( lpTableFile, 0, sizeof( TABLE_FILE ) );

		EnterCriticalSection( &csTableList );
		
		lpOldTableFile = FindTableFile( lpcszFileName );//发现当前系统是否已经存在打开的数据表
		
		if( lpOldTableFile == NULL)
		{  // 没有发现，打开它。no find, to open 
			lpTable = OpenDataTable( lpcszFileName, 
									dwAccess,
									dwShareMode,
									lpSecurityAttributes,
									dwCreate,
									nFieldNum,
									dwTableFileSize,
									dwFlagAndAttr );
		}
		else
		{  // 系统已经打开。检查共享模式。share the table ?
			if( CheckShare( lpOldTableFile->dwShareMode, dwShareMode, dwAccess ) )
				lpTable = lpOldTableFile->lpTable; // 可以共享
		}
		if( lpTable )
		{	// 初始化数据表文件对象
			lpTableFile->objType = OBJ_TABLE;
			lpTableFile->dwShareMode = dwShareMode;
			lpTableFile->dwAccessMode = dwAccess;
			lpTableFile->lpTable = lpTable;
			// 加入表链表
			_AddToTableList( lpTableFile );
			// 设置并读取第一条记录
			lpTableFile->rn = _SetRecordPointerAndUpdate( lpTable, 1 );
			if( lpTable->bFlush )
				Table_Flush( (HANDLE)lpTableFile );
		}
		else
		{	// 失败，释放之前分配的对象
			free( lpTableFile );
			lpTableFile = NULL;
		}
		LeaveCriticalSection( &csTableList ); // 离开冲突段
 	}

	if( lpTableFile )
	    return lpTableFile;
	else
		return INVALID_HANDLE_VALUE;
}

// **************************************************
// 声明：BOOL WINAPI Table_Close( HTABLE hTable )
// 参数：
//		IN hTable - 数据表对象句柄
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		关闭数据表
// 引用: 
//		系统API
// ************************************************

BOOL WINAPI Table_Close( HTABLE hTable )
{
	BOOL bRetv = TRUE;
    LPTABLE_FILE lpTableFile;
	LPTABLE lpTable;

	EnterCriticalSection( &csTableList );	// 进入冲突段

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTable = lpTableFile->lpTable ) )
    {
        WORD n = GetBlockNum( lpTable, lpTable->tfh.wFatStartBlock );		
		//需要更新吗 ？
		if( lpTableFile->dwAccessMode & GENERIC_WRITE )
		{
			// 2004-02-01
			// 更新数据表FAT
			WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, n * lpTable->tfh.nBytesPerBlock );
			//WriteBlocks( lpTable, 0, 0, (BYTE*)&lpTable->tfh  , sizeof( lpTable->tfh ) );
			UpdateTableHead( lpTable );
		}
		// 将该表对象从系统链表移出
		_RemoveFromTableList( lpTableFile );
		// 释放数据表成员数据、关闭文件
		CloseDataTable( lpTableFile->lpTable );
		// 释放数据表对象
        lpTableFile->objType = OBJ_NULL;
		free( lpTableFile );
    }
	else
		bRetv = FALSE;
	LeaveCriticalSection( &csTableList );	//离开冲突段
    return bRetv;
}

// **************************************************
// 声明：static WORD _SetRecordPointerAndUpdate( LPTABLE lpTable, long rn )
// 参数：
//		IN lpTable - 数据表对象指针
//		IN rn - 记录索引
// 返回值：
//		返回新设置的当前记录号
// 功能描述：
//		设置数据表的当前记录并且更新记录节点数据
// 引用: 
//		
// ************************************************

static WORD _SetRecordPointerAndUpdate( LPTABLE lpTable, long rn )
{
	// 读记录信息
    ReadBlocks( lpTable, 
                lpTable->tfh.wNodeIndexStartBlock, 
                sizeof( RNODEINDEX ) * (rn-1), 
                (BYTE*)&lpTable->RNodeIndex, 
                sizeof( RNODEINDEX ) );
    ASSERT( lpTable->RNodeIndex.wDataStartBlock != NULL_BLOCK );
    // 读记录字段信息
	ReadBlocks( lpTable, 
                lpTable->RNodeIndex.wDataStartBlock, 
                0, 
                (BYTE*)lpTable->pRNodeData, 
                lpTable->tfh.nNodeDataSize );
    lpTable->dwRN = (WORD)rn;
    return (WORD)rn;
}

// **************************************************
// 声明：static WORD _SetTableFilePointer( LPTABLE_FILE lpTableFile, long offset, UINT dwMethod )
// 参数：
//		IN lpTableFile - 数据表文件对象指针
//		IN offset - 记录偏移
//		IN dwMethod - 起始位置,必须为下列值之一：
//				SRP_BEGIN-从数据表的开始位置计算偏移 
//				SRP_CURRENT-从数据表的当前位置计算偏移
//				SRP_END-从数据表的结束位置计算偏移
// 返回值：
//		假如成功，返回当前记录号; 否则，返回0xffff
// 功能描述：
//		设置数据表的当前记录
// 引用: 
//		
// ************************************************

static WORD _SetTableFilePointer( LPTABLE_FILE lpTableFile, long offset, UINT dwMethod )
{
	LPTABLE lpTable;
    WORD retv = 0xffff;
    long rn;

	if( (lpTable = lpTableFile->lpTable) )
	{
		if( lpTable->tfh.nNodeNum )	// 数据表有记录吗 ？
		{    // 有，has records
			if( dwMethod == SRP_BEGIN )
				rn = 1 + offset;	// 从开始计算绝对位置
			else if( dwMethod == SRP_CURRENT )
				rn = lpTable->dwRN + offset;	// 从当前位置计算绝对位置
			else  // SRP_END
				rn = (long)lpTable->tfh.nNodeNum + offset;	// 从结束位置计算绝对位置
			
			if( rn <= 0 )	// 
				rn = 1;		// 设置为第一条记录
			else if( rn > lpTable->tfh.nNodeNum )
			{	// 无效位置
				return 0xffff;
			}
			
			lpTableFile->rn = rn;	//
			retv = (WORD)rn;
		}
		else
		{   // no record
			lpTableFile->rn = 0;	//数据表无记录
		}
	}
    return retv;
}

// **************************************************
// 声明：WORD WINAPI Table_SetRecordPointer( HTABLE hTable, long offset, UINT dwMethod )
// 参数：
//		IN hTable - 数据表对象句柄
//		IN offset - 记录偏移
//		IN dwMethod - 起始位置,必须为下列值之一：
//				SRP_BEGIN-从数据表的开始位置计算偏移 
//				SRP_CURRENT-从数据表的当前位置计算偏移
//				SRP_END-从数据表的结束位置计算偏移
// 返回值：
//		假如成功，返回当前记录号; 否则，返回0xffff
// 功能描述：
//		设置数据表的当前记录
// 引用: 
//		系统API
// ************************************************

WORD WINAPI Table_SetRecordPointer( HTABLE hTable, long offset, UINT dwMethod )
{
    WORD retv = 0xffff;
    //long rn;

    LPTABLE_FILE lpTableFile;
	LPTABLE lpTable;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTable = lpTableFile->lpTable ) )	//数据表有效吗？
    {
		LockRecord( lpTableFile, 0 ); // 锁住当前记录
		retv = _SetTableFilePointer( lpTableFile, offset, dwMethod );
        /*
		if( lpTable->tfh.nNodeNum )	// 数据表有记录吗 ？
        {    // 有，has records
            if( dwMethod == SRP_BEGIN )
                rn = 1 + offset;	// 从开始计算绝对位置
            else if( dwMethod == SRP_CURRENT )
                rn = lpTable->dwRN + offset;	// 从当前位置计算绝对位置
            else  // SRP_END
                rn = (long)lpTable->tfh.nNodeNum + offset;	// 从结束位置计算绝对位置

            if( rn <= 0 )	// 
                rn = 1;		// 设置为第一条记录
            else if( rn > lpTable->tfh.nNodeNum )
			{	// 无效位置，解锁，返回
				UnlockRecord( lpTableFile );
				return 0xffff;
			}

            lpTableFile->rn = rn;	//
            retv = (WORD)rn;
        }
        else
        {   // no record
            lpTableFile->rn = 0;	//数据表无记录
        }
		*/
		UnlockRecord( lpTableFile );	//解锁
    }
    return retv;
}

// **************************************************
// 声明：WORD WINAPI Table_NextRecordPointer( HTABLE hTable, BOOL bNext )
// 参数：
//		IN hTable - 数据表对象句柄
//		IN bNext - 假如为TRUE, 设置当前记录到下一条; 否则，前一条
// 返回值：
//		假如成功，返回当前记录号; 否则，返回0xffff
// 功能描述：
//		设置下/上一条记录无数据表的当前记录
// 引用: 
//		系统API
// ************************************************

WORD WINAPI Table_NextRecordPointer( HTABLE hTable, BOOL bNext )
{
    WORD retv = 0xffff;
	LPTABLE lpTable;

    LPTABLE_FILE lpTableFile;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTable = lpTableFile->lpTable ) )	//数据表有效吗？
    {
		LockRecord( lpTableFile, 0 ); // 锁住当前记录

        if( bNext )
            retv = _SetTableFilePointer( lpTableFile, 1, SRP_CURRENT );// 下一条
        else
            retv = _SetTableFilePointer( lpTableFile, -1, SRP_CURRENT );//上一条

		UnlockRecord( lpTableFile );	//解锁

//        if( bNext )
//            retv = Table_SetRecordPointer( hTable, 1, SRP_CURRENT );	// 下一条
//        else
//            retv = Table_SetRecordPointer( hTable, -1, SRP_CURRENT );	//上一条
    }
    return retv;
}

// **************************************************
// 声明：static WORD _ReadField( LPTABLE lpTable, UINT field, UINT startPos, void * lpBuf, UINT sizeLimit )
// 参数：
//		IN lpTable - 数据表对象指针
//		IN field - 字段
//		IN startPos - 该字段内的开始位置
//		IN lpBuf - 用于存放读取数据的缓存
//		IN sizeLimit - lpBuf的尺寸大小
// 返回值：
//		返回实际读的字节数据
// 功能描述：
//		读取字段内的数据
// 引用: 
//		
// ************************************************
static WORD _ReadField( LPTABLE lpTable, UINT field, UINT startPos, void * lpBuf, UINT sizeLimit )
{
	PRNODEDATA pRNode;
	DWORD size;
	WORD retv = 0;

	pRNode = lpTable->pRNodeData;
	if( startPos < pRNode->nFieldSize[field] )  // 段内偏移有效吗？
	{
		UINT i;
		for( i = 0, size = 0; i < field; i++ )
			size += pRNode->nFieldSize[i];
				
		// read data
		if( sizeLimit > pRNode->nFieldSize[field] - startPos )
			sizeLimit = pRNode->nFieldSize[field] - startPos;
		// sizeLimit 为真实需要读的数据
		// 读数据
		retv = (WORD)ReadBlocks( lpTable, 
			lpTable->RNodeIndex.wDataStartBlock, 
			size + startPos + lpTable->tfh.nNodeDataSize, 
			lpBuf, 
			sizeLimit );

		//if( lpTable->bIsEncrypt && retv )
		//{	//数据被加密,需要解密
		//	DecrypData( lpBuf, retv );
		//}
	}
	return retv;
}

// **************************************************
// 声明：WORD WINAPI Table_ReadField( HTABLE hTable, UINT field, WORD startPos, void * lpBuf, WORD sizeLimit )
// 参数：
//		IN hTable - 数据表对象句柄
//		IN field - 字段
//		IN startPos - 该字段内的开始位置
//		IN lpBuf - 用于存放读取数据的缓存
//		IN sizeLimit - lpBuf的尺寸大小
// 返回值：
//		返回实际读的字节数据
// 功能描述：
//		读取字段内的数据
// 引用: 
//		系统API
// ************************************************

WORD WINAPI Table_ReadField( HTABLE hTable, UINT field, WORD startPos, void * lpBuf, WORD sizeLimit )
{
    LPTABLE_FILE lpTableFile;
    LPTABLE lpTable;
    //PRNODEDATA pRNode;
//    UINT i;
    WORD retv = 0;
    //DWORD size;

	field--;	// 字段索引是从1开始，所以减一

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTable = lpTableFile->lpTable ) )	//数据表有效吗？ 
	{	// 
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD ) )	// 锁住当前记录
		{	
			if( field < lpTable->tfh.nFieldNum && 
				lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )	// 参数有效吗 ？
			{   // get pos of the field
				/*
				pRNode = lpTable->pRNodeData;
				if( startPos < pRNode->nFieldSize[field] )  // 段内偏移有效吗？
				{
					for( i = 0, size = 0; i < field; i++ )
						size += pRNode->nFieldSize[i];
					
					// read data
					if( sizeLimit > pRNode->nFieldSize[field] - startPos )
						sizeLimit = pRNode->nFieldSize[field] - startPos;
					// sizeLimit 为真实需要读的数据
					// 读数据
					retv = (WORD)ReadBlocks( lpTable, 
						lpTable->RNodeIndex.wDataStartBlock, 
						size + startPos + lpTable->tfh.nNodeDataSize, 
						lpBuf, 
						sizeLimit );
				}
				*/
				retv = _ReadField( lpTable, field, startPos, lpBuf, sizeLimit );
			}
			else if( lpTable && field >= lpTable->tfh.nFieldNum )
			{
				SetLastError( ERROR_INVALID_PARAMETER );
				WARNMSG(TABLE_ERROR, (TEXT("ReadField: field(%d) >= FieldNum(%d) \r\n"), field, lpTable->tfh.nFieldNum ));
			}
			UnlockRecord( lpTableFile );	//解锁
		}
	}

    return retv;
}

// **************************************************
// 声明：WORD WINAPI Table_WriteField( HTABLE hTable, UINT field, const void * lpcvBuf, WORD size )
// 参数：
//		IN hTable - 数据表对象句柄
//		IN field - 字段
//		IN lpBuf - 用于写入数据的缓存
//		IN sizeLimit - lpBuf的尺寸大小
// 返回值：
//		返回实际写的字节数据
// 功能描述：
//		写字段的数据
// 引用: 
//		系统API
// ************************************************

WORD WINAPI Table_WriteField( HTABLE hTable, UINT field, const void * lpcvBuf, WORD size )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    PRNODEDATA pRNode;
    long oldSize, newSize, n, posMoveFrom, posMoveTo, pos;
    UINT i;
    WORD newBlock;
    WORD retv = 0;

    field--;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
		( lpTable = lpTableFile->lpTable ) )	// 句柄是否有效 ？
    {   // get pre field size
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD|LRF_SET_FLUSH_FLAG ) )	// 锁住记录
		{
			if( field < lpTable->tfh.nFieldNum && 
				lpTable->dwRN &&
				lpTable->dwRN <= lpTable->tfh.nNodeNum )	// 字段有效吗 ？
			{
				pRNode = lpTable->pRNodeData;
				for( i = 0, n = 0; i < field; i++ )
					n += (long)pRNode->nFieldSize[i];
				
				pos = n;   // 新数据在记录的写入位置。pos of new data to write
				// 计算该字段数据需要移动的位置。sum the field size and get pos to move data
				posMoveFrom = oldSize = n + pRNode->nFieldSize[i];
				posMoveTo = newSize = n + size;
				
				//	统计该字段后的其它字段的字节总数 sum other field size
				i++;
				for( n = 0; i < lpTable->tfh.nFieldNum; i++ )
					n += pRNode->nFieldSize[i];
				
				oldSize += n;   // 老的记录数据 old record size
				newSize += n;   // 新的记录数据 new record size
				
				if( newSize > oldSize )	// 新的记录更大吗 ？
				{   // 更大，需要重新分配记录空间 alloc enough block
					if( ReallocFile( lpTable, lpTable->RNodeIndex.wDataStartBlock, newSize + lpTable->tfh.nNodeDataSize, &newBlock ) == FALSE )
					{
						goto _UNLOCK;    // not enough block
					}
					ASSERT( newBlock == lpTable->RNodeIndex.wDataStartBlock );
				}
				if( newSize != oldSize )  // 如果新记录大小与老记录大小不相等，则移动数据 move data if possible
					MoveData( lpTable, lpTable->RNodeIndex.wDataStartBlock, posMoveFrom + lpTable->tfh.nNodeDataSize, posMoveTo + lpTable->tfh.nNodeDataSize );
				// 写入新数据
				WriteBlocks( lpTable, lpTable->RNodeIndex.wDataStartBlock, pos + lpTable->tfh.nNodeDataSize, lpcvBuf, size );
				// 更新记录信息数据 now, update record info
				pRNode->nFieldSize[field] = size;
				UpdateNodeData( lpTable );
				retv = size;
			}
_UNLOCK:
			UnlockRecord( lpTableFile );	// 解锁
		}
    }
	else if( lpTable && field >= lpTable->tfh.nFieldNum )
	{	//参数不对
		SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG(TABLE_ERROR, (TEXT("WriteField: field(%d) >= FieldNum(%d) \r\n"), field, lpTable->tfh.nFieldNum ));		
	}

    return retv;
}

// **************************************************
// 声明：BOOL WINAPI Table_ReadRecord( 
//								HTABLE hTable, 
//								void * lpvData,
//								FIELDITEM fieldItem[],
//								DWORD * lpdwSize )
// 参数：
//		IN hTable - 数据表对象句柄
//		OUT lpvData-用于保存数据的内存
//		OUT fieldItem-FIELDITEM结构指针当成功时，返回各个字段在lpvData的起始地址和该字段数据的大小
//		OUT lpdwSize-当成功时，返回所有字段数据的大小;假如lpdwSize为NULL,则不返回大小
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	读当前记录的数据 
// 引用: 
//	系统API	
// ************************************************

BOOL WINAPI Table_ReadRecord( HTABLE hTable, 
							   void * lpvData,
							   FIELDITEM fieldItem[],
							   DWORD * lpdwSize )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;

    PRNODEDATA pRNode;
    BOOL retv = FALSE;
    DWORD size;
    int i;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTable = lpTableFile->lpTable ) )
    {
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD ) )	// 锁住记录
		{
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )	// 当前记录有效 ？
			{	// 有效
				pRNode = lpTable->pRNodeData;
				size = 0;
				//	统计记录的每个字段内存大小和记录总的大小
				for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
				{
					fieldItem[i].size = pRNode->nFieldSize[i];
					fieldItem[i].lpData = ((BYTE*)lpvData) + size;
					size += pRNode->nFieldSize[i];
				}
				//	读记录
				if( size == ReadBlocks( lpTable, lpTable->RNodeIndex.wDataStartBlock, lpTable->tfh.nNodeDataSize, lpvData, size ) )
				{	//成功
					if( lpdwSize )
						*lpdwSize = size;
					retv = TRUE;
					// 设置当前记录到下一条。go to next
					//Table_SetRecordPointer( hTable, 1, SRP_CURRENT );
					_SetTableFilePointer( lpTableFile, 1, SRP_CURRENT );
					//if( lpTable->bIsEncrypt )
					//{	//数据被加密,需要解密
					//	DecrypData( lpvData, size );
					//}
				}
				else
				{
					ASSERT( 0 );
				}
			}
			UnlockRecord( lpTableFile );	// 解锁
		}
    }
    return retv;
}

// **************************************************
// 声明：BOOL WINAPI Table_DeleteRecord( HTABLE hTable, UINT uiRecord, DWORD dwFlag )
// 参数：
//		IN hTable - 数据表文件对象句柄
//		IN uiRecord - 记录号
//		IN dwFlag - 附加功能
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		删除记录
// 引用: 
//		系统API
// ************************************************

BOOL WINAPI Table_DeleteRecord( HTABLE hTable, UINT uiRecord, DWORD dwFlag )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    BOOL retv = FALSE;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
		( lpTable = lpTableFile->lpTable ) )  // 得到表文件时针
	{
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD|LRF_SET_FLUSH_FLAG ) )	// 锁住记录
		{
			if( uiRecord != DR_CURRENT )	//	是否删除当前记录
			{	// 不是
				if( uiRecord <= lpTable->tfh.nNodeNum && uiRecord > 0 )
				{	// 设置表文件当前记录号
					_SetTableFilePointer( lpTableFile, uiRecord-1, SRP_BEGIN );//Table_SetRecordPointer( hTable, uiRecord-1, SRP_BEGIN );
					// 更新记录节点数据
					_SetRecordPointerAndUpdate( lpTableFile->lpTable, lpTableFile->rn );
				}
				else
					goto _UNLOCK;	//无效的记录号
				//return FALSE;
			}
			
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )
			{    // lock record before deleted
				
				if( dwFlag == DR_REMOVE )	// 是否将该记录物理释放
				{
					lpTable->tfh.nNodeNum--;
					//释放数据块
					FreeBlock( lpTable, lpTable->RNodeIndex.wDataStartBlock );
					MoveData(lpTable, 
						lpTable->tfh.wNodeIndexStartBlock, 
						lpTable->dwRN*sizeof( RNODEINDEX ),
						(lpTable->dwRN-1)*sizeof( RNODEINDEX ) );            
					lpTable->dwRN = 0;	//当前节点记录设为无效

					// set new record pointer
					if( lpTable->dwRN > lpTable->tfh.nNodeNum )  // 最后一条记录
						_SetTableFilePointer( lpTableFile, 0, SRP_END );//Table_SetRecordPointer( hTable, 0, SRP_END );
					else	//
						_SetTableFilePointer( lpTableFile,lpTable->dwRN-1, SRP_BEGIN );//Table_SetRecordPointer( hTable, lpTable->dwRN-1, SRP_BEGIN );
					retv = TRUE;
				}
				else if( dwFlag == DR_NOREMOVE )	
				{	// 逻辑删除, 仅仅做删除标志
					//lpTable->pRNodeData->wFlag = RF_LOGIC_DELETE;
					//retv = UpdateNodeData( lpTable );
					lpTable->RNodeIndex.bAttrib |= RA_LOGIC_DELETE;
					retv = UpdateNodeIndex( lpTable );
				}
			}
_UNLOCK:
		UnlockRecord( lpTableFile );	// 解锁
		}
	}
//RET_DELETE:
    return retv;
}

// **************************************************
// 声明：BOOL WINAPI Table_Delete( LPCTSTR lpcszFileName )
// 参数：
//		IN lpcszFileName - 文件名
// 返回值：
//		成功：返回非零
//		否则：0
// 功能描述：
//		删除数据表文件
// 引用: 
//		系统API
// ************************************************

BOOL WINAPI Table_Delete( LPCTSTR lpcszFileName )
{
    return DeleteFile( lpcszFileName );
}

/*
// **************************************************
// 声明：static FIELDITEM * GetRemapFieldItem(  FIELDITEM fieldItem[], int nFieldNum )
// 参数：
//		IN fieldItem - 字段信息数组
//		IN nFieldNum - 字段数
// 返回值：
//		假如成功，返回字段指针；否则，返回NULL
// 功能描述：
//		映射记录指针
// 引用: 
//		
// ************************************************

static FIELDITEM * GetRemapFieldItem(  FIELDITEM fieldItem[], int nFieldNum )
{
	FIELDITEM * lpItems = malloc( sizeof( FIELDITEM ) * nFieldNum );
	if( lpItems )
	{
		HANDLE hCallerProcess = GetCallerProcess();
		int i;
		memcpy( lpItems, fieldItem, sizeof( FIELDITEM ) * nFieldNum );
		
		//lpItems[0].lpData  = MapPtrToProcess( lpItems[0].lpData, hCallerProcess );
		for( i = 0; i < nFieldNum; i++ )
		{
			lpItems[i].lpData = MapPtrToProcess( lpItems[i].lpData, hCallerProcess );
		}
	}
	return lpItems;
}

// **************************************************
// 声明：
// 参数：
//		
// 返回值：
//		
// 功能描述：
//		
// 引用: 
//		
// ************************************************

static void ReleaseRemapFieldItem( FIELDITEM * lpItems )
{
	free( lpItems );
}
*/

// **************************************************
// 声明：long WINAPI Table_WriteRecord( HTABLE hTable, FIELDITEM fieldItem[] )
// 参数：
//		IN hTable - 数据表文件对象
//		IN fieldItem - FIELDITEM结构指针，包含所有字段的数据设置数组
// 返回值：
//		成功：返回实际写的数据
//		否则：返回0
// 功能描述：
//		写数据到当前记录 
// 引用: 
//		系统API
// ************************************************

long WINAPI Table_WriteRecord( HTABLE hTable, FIELDITEM fieldItem[] )
{
    long newSize;
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    BOOL bFrag;
    BYTE *lpData;
    int i;

	newSize = 0;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
        ( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
		( lpTable = lpTableFile->lpTable ) )	// 句柄到数据表文件指针
    {
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD|LRF_SET_FLUSH_FLAG ) )	// 锁住记录
		{			
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )	// 有效记录号
			{
				FIELDITEM * lpItems = fieldItem; //GetRemapFieldItem( fieldItem, lpTable->tfh.nFieldNum );//malloc( sizeof( FIELDITEM ) * lpTable->tfh.nFieldNum );
				if( lpItems )
				{
					bFrag = FALSE;
					lpData = lpItems[0].lpData;
					// 检查数据是否连续
					for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
					{
						newSize += lpItems[i].size;
						if( lpData == lpItems[i].lpData )
							lpData += lpItems[i].size;
						else
							bFrag = TRUE;  // 非连续的 is fragmental
					}
					// 为该记录分配文件空间
					if( ReallocFile( lpTable,
						lpTable->RNodeIndex.wDataStartBlock,
						newSize + lpTable->tfh.nNodeDataSize,
						&lpTable->RNodeIndex.wDataStartBlock ) )
					{	// 拷贝用户数据到记录
						_CopyRecordData( lpTable, lpItems, newSize, bFrag, lpTableFile->hOwnerProc );
					}
					else
						newSize = 0;					
					//ReleaseRemapFieldItem( lpItems );// free( lpItems );
				}
			}
			UnlockRecord( lpTableFile );  // 解锁
		}
    }
    return newSize;
}

// **************************************************
// 声明：BOOL WINAPI Table_InsertRecord( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] )
// 参数：
//	IN hTable-数据表文件对象
//	IN nPos-插入位置
//	IN fieldItem-FIELDITEM 结构指针，包含所有字段的数据设置数组
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	插入记录到指定位置
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Table_InsertRecord( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    BOOL retv = FALSE;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
		( lpTable = lpTableFile->lpTable ) &&
		nPos )	// 得到数据表对象指针
    {
		if( LockRecord( lpTableFile, LRF_SET_FLUSH_FLAG ) )	//	锁住记录
		{
			FIELDITEM * lpItems = fieldItem;//GetRemapFieldItem( fieldItem, lpTable->tfh.nFieldNum );//malloc( sizeof( FIELDITEM ) * lpTable->tfh.nFieldNum );
			
			if( lpItems )
			{	// 插入指定位置
				if( lpTable->tfh.nNodeNum == 0 )  // like append
					retv = _InsertRecord( lpTable, 1, lpItems, lpTableFile->hOwnerProc );
				else if( nPos <= lpTable->tfh.nNodeNum  )
					retv = _InsertRecord( lpTable, nPos, lpItems, lpTableFile->hOwnerProc );
				else   // insert to end
					retv = _InsertRecord( lpTable, (WORD)(lpTable->tfh.nNodeNum + 1), lpItems, lpTableFile->hOwnerProc );
				//ReleaseRemapFieldItem( lpItems );	
			}
			UnlockRecord( lpTableFile );	// 解锁
		}
	}
    return retv;
}

// **************************************************
// 声明：BOOL WINAPI Table_AppendRecord( HTABLE hTable, FIELDITEM fieldItem[] )
// 参数：
//	IN hTable-数据表文件句柄
//	IN fieldItem-FIELDITEM结构指针，包含所有字段的数据设置数组
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	添加记录到数据表末尾位置
// 引用: 
//	系统API
// ************************************************
#define DEBUG_Table_AppendRecord 1
BOOL WINAPI Table_AppendRecord( HTABLE hTable, FIELDITEM fieldItem[] )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    BOOL retv = FALSE;

    DEBUGMSG( DEBUG_Table_AppendRecord, ( "Table_AppendRecord entry.\r\n" ) );
    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) &&
        ( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
		( lpTable = lpTableFile->lpTable ) )
    {
	    DEBUGMSG( DEBUG_Table_AppendRecord, ( "Table_AppendRecord LockRecord.\r\n" ) );    	
		if( LockRecord( lpTableFile, LRF_SET_FLUSH_FLAG ) ) // 锁住的当前记录
		{
			FIELDITEM * lpItems = fieldItem;			

			if( lpItems )
			{	// 插入记录到尾
			    DEBUGMSG( DEBUG_Table_AppendRecord, ( "Table_AppendRecord _InsertRecord.\r\n" ) );
				retv = _InsertRecord( lpTable, (WORD)(lpTable->tfh.nNodeNum + 1), lpItems, lpTableFile->hOwnerProc );
			}
			UnlockRecord( lpTableFile ); // 解锁
		}
	}
    DEBUGMSG( DEBUG_Table_AppendRecord, ( "Table_AppendRecord leave.\r\n" ) );	
    return retv;
}

// **************************************************
// 声明：int WINAPI Table_CountRecord( HTABLE hTable )
// 参数：
//	IN hTable-数据表文件句柄
// 返回值：
//	假如成功，返回当前记录数；否则，返回0	
// 功能描述：
//	统计记录数	
// 引用: 
//	系统API
// ************************************************

int WINAPI Table_CountRecord( HTABLE hTable )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTable = lpTableFile->lpTable ) )
    {
        return lpTable->tfh.nNodeNum;
    }
    return 0;
}


// **************************************************
// 声明：BOOL WINAPI Table_SortRecord( HTABLE hTable, WORD field, FIELDCOMPPROC lpFieldProc )
// 参数：
//	IN hTable - 数据表文件对象
//	IN field - 需要排序的字段索引号(基于1)
//	IN lpFieldProc - FIELDCOMPPROC类型，应用提供的字段比较回调函数
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	将数据表的记录以字段filed为标准进行排序 	
// 引用: 
//	系统API
// ************************************************
/*
// 排序基本算法
  
void Sort(int *A, int const AHigh)
{
  int I, J, T;

  for (I=0; I <= AHigh-1; I++)
    for (J=AHigh; J >= I+1; J--)
      if (A[I] > A[J])
      {
        VisualSwap(A[I], A[J], I, J);
        T = A[I];
        A[I] = A[J];
        A[J] = T;
        if (Terminated)
          return;
      }
}
*/

BOOL WINAPI Table_SortRecord( HTABLE hTable, WORD field, FIELDCOMPPROC lpFieldProc )
{
    int count;
    int i, j, cmp;
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    DWORD oldRecordNo;
    COMPSTRUCT cs;
	CALLBACKDATA cd;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
        ( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
		( lpTable = lpTableFile->lpTable ) )
    {
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD|LRF_SET_FLUSH_FLAG ) && lpTable->dwRN )  // 锁住记录
		{
			// 为回调处理准备参数
			cd.hProcess = lpTableFile->hOwnerProc;
			cd.lpfn = (FARPROC)lpFieldProc;
			cd.dwArg0 = (DWORD)&cs;

			oldRecordNo = lpTable->dwRN;
			count = lpTable->tfh.nNodeNum; // 总的记录数
			//排序过程
			for ( i = 1; i < count; i++)
			{
				cmp = 1;
				for ( j = count; j >= i + 1; j-- )
				{
					if( cmp > 0 )
					{	
						_SetTableFilePointer( lpTableFile, i, SRP_BEGIN );//Table_SetRecordPointer( hTable, i, FILE_BEGIN );
						cs.size0 = Table_ReadField( hTable, field, 0, cs.field0, sizeof( cs.field0 ) );
					}
					_SetTableFilePointer( lpTableFile, j, SRP_BEGIN );//Table_SetRecordPointer( hTable, j, FILE_BEGIN );
					cs.size1 = Table_ReadField( hTable, field,  0, cs.field1, sizeof( cs.field1 ) );
					cmp = Sys_ImplementCallBack4( &cd );
					//cmp = lpFieldProc( &cs ); // error , i must call ImpentCallBack4
					if( cmp > 0 )
						_Exchange( lpTable, (WORD)i, (WORD)j );	// 交换记录号
				}
			}
			// 恢复老的当前记录号
			_SetTableFilePointer( lpTableFile, oldRecordNo, SRP_BEGIN );//Table_SetRecordPointer( hTable, oldRecordNo, SRP_BEGIN );
			UnlockRecord( lpTableFile );	// 解锁
			return TRUE;
		}
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Table_GetRecordInfo( HTABLE hTable, LPRECORDINFO lpri )
// 参数：
//	IN hTable – 数据表文件句柄
//	IN lpri - RECORDINFO结构指针，用于接受信息
// 返回值：
//	假如成功，返回TRUE，lpri保存有当前记录信息；假如失败，返回FALSE
// 功能描述：
//	得到当前记录信息
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Table_GetRecordInfo( HTABLE hTable, LPRECORDINFO lpri )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    PRNODEDATA pRNode;
    DWORD size = 0;
    int i;
	BOOL bRetv = FALSE;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTable = lpTableFile->lpTable ) )	// 由数据表文件句柄得到对象指针
    {
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD ) )	// 锁住当前记录
		{
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )
			{
				if( lpri->uiMask & RIM_DELETED )	// 需要删除信息吗 ？
				{	// 需要
					lpri->bIsDeleted = ( (lpTable->RNodeIndex.bAttrib & RA_LOGIC_DELETE) != 0 );
				}
				if( (lpri->uiMask & RIM_RECORDSIZE) ||
					(lpri->uiMask & RIM_FIELDSIZE) ) // 需要记录大小和每个字段的信息吗 ？
				{	//需要
					pRNode = lpTable->pRNodeData;
					for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
					{
						if( lpri->uiMask & RIM_FIELDSIZE ) // 
							lpri->uiFieldSize[i] = pRNode->nFieldSize[i];//字段信息
						size += pRNode->nFieldSize[i];
					}
					if(lpri->uiMask & RIM_RECORDSIZE)
						lpri->uiRecordSize = size; //记录信息
				}
				bRetv = TRUE;
			}
			UnlockRecord( lpTableFile ); // 解锁
		}
    }
    return bRetv;
}

// **************************************************
// 声明：DWORD WINAPI Table_GetRecordSize( HTABLE hTable )
// 参数：
//		IN hTable – 数据表文件句柄
// 返回值：
//	成功：返回尺寸
//	否则：返回0
// 功能描述：
//	得到当前记录的尺寸	
// 引用: 
//		
// ************************************************

DWORD WINAPI Table_GetRecordSize( HTABLE hTable )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    PRNODEDATA pRNode;
    DWORD size = 0;
    int i;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
        ( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTable = lpTableFile->lpTable ) )	// 由数据表文件句柄得到数据表文件对象指针
    {        
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD ) )// 锁住当前记录
		{
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )
			{
				pRNode = lpTable->pRNodeData;
				for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
					size += pRNode->nFieldSize[i];
			}
			UnlockRecord( lpTableFile );// 解锁
		}
    }
    return size;
}
/*
static char * _qstrstr(const char *s1, const char *s2, int l )
{
    const unsigned char * p1, *p2, *ps;
    unsigned char c;
    int i;

    p1 = s1, p2 = s2;
    ps = p1;
    while( l )
    {
        // find first match char c
        c = *p2++;
        while( 1 )
        {
            if( c == *p1 )
                break;
            if( *p1 == 0 )
                return 0;
            p1++;
        }
        ps = p1;  // save first address
        p1++;

        // other chars match ?
        for( i = 1; i < l; i++, p1++, p2++ )
        {
            if( *p2 == 0 )
            {
                p2 = s2;  // reset p2 to start address
                p1 -= l;  //
                ps = p1;  // save p1 address
            }
            if( *p1 != *p2 )
                break;
            if( *p1 == 0 )
                return 0;
        }
        if( i == l )   // all char is match
            return (char*)ps;
        p1++;
    }
    return 0;
}
*/

// **************************************************
// 声明：static char * _qstrstr(const char *s1, int len1, const char *s2, int len2 )
// 参数：
//		IN s1 - 字符串1
//		IN len1 - 字符串1的长度
//		IN s2 - 字符串2
//		IN len2 - 字符串2的长度
// 返回值：
//		假如找到，返回在s1中具有s2的地址；否则，返回NULL
// 功能描述：
//		在字符串s1中查找是否有字符串s2。
// 引用: 
//		
// ************************************************

static char * _qstrstr(const char *s1, int len1, const char *s2, int len2 )
{
    register const unsigned char * p1, *p2, *ps, *p1end, *p2end;
    register unsigned char c;
    register int i;


    p1 = s1, p2 = s2;
    ps = p1;
	p1end = s1 + len1;
	p2end = s2 + len2;
    
    while( p1 < p1end )
    {
        // 首先得到首个字符相等的位置.find first match char c
        c = *p2++;
        do
        {
            if( c == *p1++ )
                break;
			if( p1 == p1end )
				return 0;
        }while( 1 );

        ps = p1-1;  // 保存第一个位置。save first address
        // other chars match ?
		// 其它字符相同吗 ？
        for( i = 1; i < len2; i++, p1++, p2++ )
        {
			if( p1 == p1end )
                return 0; // 比较完，相同
			if( p2 == p2end ) // 后段比较完 ？
            {	// 是，比较前段
                p2 = s2;  // reset p2 to start address
                p1 -= len2;  //
                ps = p1;  // save p1 address
            }

            if( *p1 != *p2 ) // 不等吗 ？
                break;	// 是，退出当前循环
        }
        if( i == len2 )   // 所有字符都比较完 ？all char is match
            return (char*)ps; // 是
        p1++;
    }
    return 0;
}


/*
static void * memsearch( const void *s1, int len1, const void *s2, int len2)
{
    const unsigned char *src, *pattern, *ps;
    int patternlen, i, slen;

    if( len1 > 0 && len1 < len2 )
        return 0;
    // slen is max search length
    slen = len1 - len2 + 1;

    ps = src = (const unsigned char *)s1;

    // set pattern and pattern len
    pattern = (const unsigned char *)s2;
    patternlen = len2;
    while( 1 )
    {   // find first match char c
        for( ; slen && *src != *pattern; slen--, src++ );
        if( slen == 0 )
            return 0;   // not find match
        // current pattern is match, skip next
        pattern++; patternlen--;
        // save first match address
        ps = src - 1;
        // other chars match ?
        for( i = 1; i < len2; i++, src++, pattern++, patternlen-- )
        {
            if( patternlen == 0 )
            {
                // reset pattern to start adrress
                pattern = s2;
                patternlen = len2;

                // reset src to first match pos
                src -= patternlen;
                ps = src;
            }
            if( *src != *pattern )
                break;
        }
        if( i == len2 )   // all char is match
            return (char*)ps;
        src++; slen--;
    }
}
*/


// **************************************************
// 声明：static UINT _IsMatch( PTABLEFIND lpfs )
// 参数：
//		IN lpfs - TABLEINFO结构指针
// 返回值：
//		假如得到匹配的记录，返回记录好；否则，返回0
// 功能描述：
//		在数据表中查找具有某字符串的记录，需要比较的数据不能成功127个
// 引用: 
//		
// ************************************************

// if field = 0xffff, match all field, also only match single field
// the lpFind not  > 127
static UINT _IsMatch( PTABLEFIND lpfs )
{
    //BYTE buf[256];
    long s, pos;
    BYTE field;
	BYTE * lpvBuf = (BYTE*)lpfs->lpvBuf;
	// 比较字段
    for( field = lpfs->uFieldFrom; field <= lpfs->uFieldTo; field++ )
    {   // search every fileld
        pos = 0;
        while( 1 )
        {	// 读字段数据
            s = Table_ReadField( lpfs->hTable, field, (WORD)pos, lpvBuf, 256 );
            if( s < (long)lpfs->uLen )
                break;
			// 比较
            if( _qstrstr( lpvBuf, s, lpfs->lpvFind, lpfs->uLen ) )
//            if( memsearch( buf, s, lpfs->lpFind, lpfs->len ) )
                return field;  // find match string
            if( s < 256 )
                break;
            pos += 256 - lpfs->uLen + 1;
        }
    }
    // not found
    return 0;
}

// **************************************************
// 声明：static WORD GetMax( WORD * pfat, WORD wStart, WORD * pPrev )
// 参数：
//		IN pfat - 包含有块分配信息数据指针
//		IN wStart - 快链的首块索引
//		OUT pPrev - 用于接收块链中指向最大块索引的块索引 
// 返回值：
//		返回块链中的最大块索引
// 功能描述：
//		在块链中得到最大的块索引号
// 引用: 
//		
// ************************************************

static WORD GetMax( WORD * pfat, WORD wStart, WORD * pPrev )
{
    WORD wPrev = wStart;
    WORD wNext = NEXT_BLOCK( pfat, wPrev );
    WORD wMax, wMaxPrev;
    // sort fat
    wMax = wPrev;
    wMaxPrev = NULL_BLOCK;
    while( wNext != NULL_BLOCK )
    {
        if( wNext > wMax )
        {
            wMax = wNext;
            wMaxPrev = wPrev;
        }
        wPrev = wNext;
        wNext = NEXT_BLOCK( pfat, wNext );
    }
    *pPrev = wMaxPrev;
    return wMax;
}

// **************************************************
// 声明：static WORD GetMin( WORD * pfat, WORD wStart, WORD * pPrev )
// 参数：
//		IN pfat - 包含有块分配信息数据指针
//		IN wStart - 快链的首块索引
//		OUT pPrev - 用于接收块链中指向最小块索引的块索引 
// 返回值：
//		返回块链中的最小块索引
// 功能描述：
//		在块链中得到最小的块索引号
// 引用: 
//		
// ************************************************

static WORD GetMin( WORD * pfat, WORD wStart, WORD * pPrev )
{
    WORD wPrev = wStart;
    WORD wNext = NEXT_BLOCK( pfat, wPrev );
    WORD wMin, wMinPrev;
    // sort fat
    wMin = wPrev;
    wMinPrev = NULL_BLOCK;
    while( wNext != NULL_BLOCK )
    {
        if( wNext < wMin )
        {
            wMin = wNext;
            wMinPrev = wPrev;
        }
        wPrev = wNext;
        wNext = NEXT_BLOCK( pfat, wNext );
    }
    *pPrev = wMinPrev;
    return wMin;
}

// **************************************************
// 声明：static UINT _RemoveLogicRecord( LPTABLE lpTable, PRNODEINDEX pNodeIndex )
// 参数：
//		IN lpTable - 数据表指针
//		IN pNodeIndex - 记录节点索引数组
// 返回值：
//		删除的记录数	
// 功能描述：
//		物理删除之前已被逻辑删除的记录
// 引用: 
//		
// ************************************************

static UINT _RemoveLogicRecord( LPTABLE lpTable, PRNODEINDEX pNodeIndex )
{
	UINT i;
	int iDelCount = 0;
	PRNODEINDEX pIndex = pNodeIndex;
	PRNODEINDEX pIndexHead;
	UINT uiNum = lpTable->tfh.nNodeNum;

	for( i = 0; i < uiNum; i++, pIndex++ )
	{
		if( pIndex->bAttrib & RA_LOGIC_DELETE )  // 是否以被逻辑删除 ？
		{	//是
			iDelCount++;
			FreeBlock( lpTable, pIndex->wDataStartBlock );  // 释放记录的数据
		}
	}

	if( iDelCount )
	{	// 更新记录节点索引数组
		pIndexHead = pIndex = pNodeIndex;
		for( i = 0; i < uiNum ; i++, pIndex++ )
		{
			if( (pIndex->bAttrib & RA_LOGIC_DELETE) == 0 )
			{
			    *pIndexHead++ = *pIndex;
			}
		}
		lpTable->tfh.nNodeNum -= iDelCount;
		// 将更新的数据写入磁盘
		WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)pNodeIndex,  lpTable->tfh.nNodeNum * sizeof( RNODEINDEX ) );
	}
	return iDelCount;
}

// **************************************************
// 声明：BOOL WINAPI Table_Pack( LPCTSTR lpcszFileName, UINT uiFlag )
// 参数：
//		IN lpcszFileName - 数据表文件名
//		IN uiFlag - 压缩标志，包含：
//							PT_NOREMOVE - 不物理删除之前已经被逻辑删除的记录
//							PT_REMOVE - 物理删除之前已经被逻辑删除的记录
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		压缩数据表。如果可能，先将之前逻辑删除的记录物理删除；然后将所有的数据尽可能移到文件的
//		前面，最后减少文件大小
// 引用: 
//		
// ************************************************

static BOOL DoTablePack( LPTABLE_FILE lpTableFile, UINT uiFlag )
{
    //HANDLE hTable = Table_Create( lpcszFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0 );
	LPTABLE lpTable;
	//LPTABLE_FILE lpTableFile;
	//if( hTable == INVALID_HANDLE_VALUE )
		//return FALSE;
	ASSERT( lpTableFile );
    if( ( lpTable = lpTableFile->lpTable ) )	//// 由数据表文件句柄得到数据表文件对象指针
    {
		if( LockRecord( lpTableFile, LRF_SET_FLUSH_FLAG ) )	// 锁住数据表
		{
			DWORD dwSize;
			PRNODEINDEX pIndex, pRNodeIndex;

			_SetTableFilePointer( lpTableFile, 0, SRP_BEGIN );

			dwSize = lpTable->tfh.nNodeNum * sizeof( RNODEINDEX );
			pRNodeIndex = malloc( dwSize + lpTable->tfh.nBytesPerBlock );
			
			if( pRNodeIndex )
			{   /// get a min block of free
				// 读出数据表的记录索引表
				ReadBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)pRNodeIndex, dwSize );
				if( uiFlag & PT_REMOVE )
				{	// 物理删除所有的逻辑删除的记录
					_RemoveLogicRecord( lpTable, pRNodeIndex ); 
				}
REPEAT: 
				if( lpTable->tfh.dwFixFileSize == 0 &&
					lpTable->tfh.nFreeCount &&
					(uiFlag & PT_NOCOMPRESS) == 0 )
				{
					WORD wMinPrev, wMaxPrev = 0;
					WORD wMinFree;
					WORD wMaxAlloc;
					WORD wPrev, wMax;
					WORD nIndex, nMaxIndex;
					BYTE * pBlock = (BYTE*)pRNodeIndex + dwSize;
					
					if( lpTable->tfh.nNodeNum )
					{
						while( 1 )
						{	// 得到空闲区最小的块索引号
							wMinFree = GetMin( lpTable->pfat, lpTable->tfh.wFreeBlock, &wMinPrev );
							
							//得到数据区的最大的块索引号

							wMaxAlloc = GetMax( lpTable->pfat, lpTable->tfh.wFatStartBlock, &wPrev );
							wMaxPrev = wPrev;
							wMax = GetMax( lpTable->pfat, lpTable->tfh.wNodeIndexStartBlock, &wPrev );
							if( wMax > wMaxAlloc )
							{
								wMaxAlloc = wMax;
								wMaxPrev = wPrev;
							}							
							pIndex = pRNodeIndex;
							for( nIndex = 0; nIndex < lpTable->tfh.nNodeNum; nIndex++, pIndex++ )
							{
								wMax = GetMax( lpTable->pfat, pIndex->wDataStartBlock, &wPrev );
								if( wMax > wMaxAlloc )
								{
									wMaxAlloc = wMax;
									wMaxPrev = wPrev;
									nMaxIndex = nIndex;
								}
							}
							// 
							if( wMinFree > wMaxAlloc ) // 在数据块区有空闲块吗 ？
							{  //  没有，所有的数据块都在文件的前部 all free is bottom
								WORD nNewFatBlock;
								WORD nOldFatBlock;
								// 缩小文件大小
								SetFilePointer(
											lpTable->hFile, 
											-(lpTable->tfh.nFreeCount * lpTable->tfh.nBytesPerBlock), 
											NULL,
											FILE_END );
								SetEndOfFile( lpTable->hFile );
								// 重新设置数据表文件系统信息
								lpTable->tfh.wTotalBlock -= lpTable->tfh.nFreeCount;
								lpTable->tfh.nFreeCount = 0;
								lpTable->tfh.wFreeBlock = NULL_BLOCK;
								
								nNewFatBlock = FAT_BLOCK_NUM( lpTable, lpTable->tfh.wTotalBlock );
								nOldFatBlock = GetBlockNum( lpTable, lpTable->tfh.wFatStartBlock );
								if( nNewFatBlock < nOldFatBlock )
								{
									ReallocBlock( lpTable, lpTable->tfh.wFatStartBlock, nNewFatBlock );
									goto REPEAT;
								}
								break;
							}
							else
							{   // // 在数据块区有空闲块，需要交换数据。exchange data ....
								WORD wMaxNext;
								// 将最大的数据块数据写入空闲块，并将该空闲块作为数据块并释放源数据块
								ReadBlocks( lpTable, wMaxAlloc, 0, pBlock, lpTable->tfh.nBytesPerBlock );
								WriteBlocks( lpTable, wMinFree, 0, pBlock, lpTable->tfh.nBytesPerBlock );								
								// exchange fat
								wMaxNext = NEXT_BLOCK( lpTable->pfat, wMaxAlloc );
								// 改变FAT数据
								if( wMinPrev == NULL_BLOCK )
								{
									_ASSERT( wMinFree == lpTable->tfh.wFreeBlock );
									*(lpTable->pfat + wMaxAlloc) = NEXT_BLOCK( lpTable->pfat, wMinFree );
									lpTable->tfh.wFreeBlock = wMaxAlloc;
								}
								else
								{
									*(lpTable->pfat + wMinPrev) = wMaxAlloc;
									*(lpTable->pfat + wMaxAlloc) = NEXT_BLOCK( lpTable->pfat, wMinFree );
								}
								//改变FAT数据和索引节点数据
								if( wMaxPrev == NULL_BLOCK )
								{
									_ASSERT( wMaxAlloc == (pRNodeIndex + nMaxIndex)->wDataStartBlock );
									*(lpTable->pfat + wMinFree) = wMaxNext;
									(pRNodeIndex + nMaxIndex)->wDataStartBlock = wMinFree;
								}
								else
								{
									*(lpTable->pfat + wMaxPrev) = wMinFree;
									*(lpTable->pfat + wMinFree) = wMaxNext;
								}
							}
						}
						/// update index data
						//压缩完，更新记录索引表数据
						WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)pRNodeIndex, dwSize );
					}
					else
					{  // 该表没有任何数据。只需要保留表头数据区。no data, free all, noly has header block and fat block and index block
						int i;
						lpTable->tfh.wTotalBlock = (sizeof( lpTable->tfh ) + lpTable->tfh.nBytesPerBlock - 1) / lpTable->tfh.nBytesPerBlock + 1 + 1;
						lpTable->tfh.wFatStartBlock = lpTable->tfh.wTotalBlock-2;
						lpTable->tfh.wFreeBlock = NULL_BLOCK;// | DELETE_MARK;
						lpTable->tfh.nFreeCount = 0;
						lpTable->tfh.wNodeIndexStartBlock = lpTable->tfh.wTotalBlock-1;
						
						for( i = 0; i < lpTable->tfh.wTotalBlock; i++ )
							*(lpTable->pfat+i) = NULL_BLOCK;
						dwSize = lpTable->tfh.wTotalBlock * lpTable->tfh.nBytesPerBlock;
						SetFilePointer( lpTable->hFile, dwSize, NULL, FILE_BEGIN );
						SetEndOfFile( lpTable->hFile );
					}
				}
				free( pRNodeIndex );
			}

			_SetTableFilePointer( lpTableFile, 0, SRP_BEGIN );

			UnlockRecord( lpTableFile ); // 解锁
		}
        //Table_Close( hTable );	//关闭表
    }
    return TRUE;
}

// **************************************************
// 声明：BOOL WINAPI Table_Pack( LPCTSTR lpcszFileName, UINT uiFlag )
// 参数：
//		IN lpcszFileName - 数据表文件名
//		IN uiFlag - 压缩标志，包含：
//							PT_NOREMOVE - 不物理删除之前已经被逻辑删除的记录
//							PT_REMOVE - 物理删除之前已经被逻辑删除的记录
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		压缩数据表。如果可能，先将之前逻辑删除的记录物理删除；然后将所有的数据尽可能移到文件的
//		前面，最后减少文件大小
// 引用: 
//		
// ************************************************
#define DEBUG_Table_Pack 1
BOOL WINAPI Table_Pack( LPCTSTR lpcszFileName, UINT uiFlag )
{
	BOOL bRetv = FALSE;
	HANDLE hTable;
	LPTABLE_FILE lpTableFile;
	

	DEBUGMSG( DEBUG_Table_Pack, ( "Table_Pack: entry.\r\n" ) );
	//hTable = Table_Create( lpcszFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0 );
	{
	CREATE_TABLE ct;
	memset( &ct, 0, sizeof( ct ) );

	ct.lpcszFileName = lpcszFileName;
	ct.dwAccess = GENERIC_READ|GENERIC_WRITE;
	ct.dwCreate = OPEN_EXISTING;
	hTable = Table_CreateByStruct( &ct );
	}
		
	if( hTable != INVALID_HANDLE_VALUE )
	{
		lpTableFile = _GetHTABLEPtr( hTable );
		if( lpTableFile )
			bRetv = DoTablePack( lpTableFile, uiFlag );
		Table_Close( hTable );
	}
	DEBUGMSG( DEBUG_Table_Pack, ( "Table_Pack: leave.\r\n" ) );	
	return bRetv;
}

// **************************************************
// 声明：BOOL WINAPI Table_PackByHandle( HANDLE hTable, UINT uiFlag )
// 参数：
//		IN hTable - 数据表文件句柄
//		IN uiFlag - 压缩标志，包含：
//							PT_NOREMOVE - 不物理删除之前已经被逻辑删除的记录
//							PT_REMOVE - 物理删除之前已经被逻辑删除的记录
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		压缩数据表。如果可能，先将之前逻辑删除的记录物理删除；然后将所有的数据尽可能移到文件的
//		前面，最后减少文件大小
// 引用: 
//		
// ************************************************
#define DEBUG_Table_PackByHandle 1
BOOL WINAPI Table_PackByHandle( HANDLE hTable, UINT uiFlag )
{
	BOOL bRetv = FALSE;
	LPTABLE_FILE lpTableFile;

	DEBUGMSG( DEBUG_Table_Pack, ( "Table_PackByHandle: entry.\r\n" ) );
	lpTableFile = _GetHTABLEPtr( (HANDLE)hTable );
	if( lpTableFile && 
		( lpTableFile->dwAccessMode & GENERIC_WRITE ) )
	{
		bRetv = DoTablePack( lpTableFile, uiFlag );
	}
	DEBUGMSG( DEBUG_Table_Pack, ( "Table_PackByHandle: leave.\r\n" ) );	
	return bRetv;
}

// **************************************************
// 声明：HANDLE WINAPI Table_FindFirst( 
//							HTABLE hTable, 
//							UINT uField, 
//							const void * lpcvFind, 
//							UINT uLen, 
//							LPFINDSTRUCT lpfs )
// 参数：
//	IN hTable - 数据表文件对象 
//	IN uField - 字段索引(基于1)，如果为-1，则查找所有字段
//	IN lpcvFind - 需要查找的数据
//	IN uLen - 查找的数据长度
//	OUT lpfs - FINDSTRUCT结构指针，用于接受查找到的记录信息
// 返回值：
//	成功：返回有效的句柄并在lpfs里返回符合条件的第一条记录
//	否则：返回INVALID_HANDLE_VAULE
// 功能描述：
//	搜索符合条件的记录, 并返回搜索句柄以便于搜索下一条记录
// 引用: 
//	系统API
// ************************************************

HANDLE WINAPI Table_FindFirst( HTABLE hTable, UINT uField, const void * lpcvFind, UINT uLen, LPFINDSTRUCT lpfs )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;

	// 127 ? because in _IsMatch, can read max 256 bytes
	// if lpFind > 127 , the IsMatch can't handle correctly
    // 检查参数
	if( uLen == 0 || uLen > 127 )
        return INVALID_HANDLE_VALUE;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
        ( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTable = lpTableFile->lpTable ) )	// 由数据表文件句柄得到数据表文件对象指针
    {
		if( LockRecord( lpTableFile, 0 ) )	// 锁住数据表文件
		{
			if( lpTable->dwRN )
			{	// 分配一个搜索结构对象
				PTABLEFIND pFind = malloc( sizeof( TABELFIND ) + uLen + 256 );
				
				if( pFind )
				{				    
					// 初始化搜索结构
					pFind->objType = OBJ_FINDTABLE;
					pFind->lpvFind = (VOID*)(pFind + 1);
					pFind->lpvBuf = (BYTE*)pFind->lpvFind + uLen;
					if( uField == -1 )
					{   // 查找所有字段。match all filed
						pFind->uFieldTo = lpTable->tfh.nFieldNum;
						pFind->uFieldFrom = 1;
					}
					else   // 只查找指定的字段。match only this field
						pFind->uFieldFrom = pFind->uFieldTo = (BYTE)uField;
					pFind->uLen = uLen;
					memcpy( pFind->lpvFind, lpcvFind, uLen );
					pFind->hTable = hTable;            
					lpTableFile->rn = _SetRecordPointerAndUpdate( lpTable, 1 );//读取第一条记录信息	         
					while( 1 )
					{
						UINT f;
						if( (f = _IsMatch( pFind )) != 0 )	// 该条记录是否匹配
						{	// 符合要求
							lpfs->nRecord = lpTable->dwRN;
							lpfs->uField = f;
							UnlockRecord( lpTableFile );	// 解锁
							return pFind;
						}
						// 不匹配，读取下一条记录
						if( lpTable->dwRN < lpTable->tfh.nNodeNum )
							lpTableFile->rn = _SetRecordPointerAndUpdate( lpTable, lpTable->dwRN + 1 );
						else
							break;
					}
					// 没有找到任何记录，释放数据
					pFind->objType = OBJ_NULL;
					free( pFind );
				}
			}
			UnlockRecord( lpTableFile );	// 解锁
		}
    }
    return INVALID_HANDLE_VALUE;
}

// **************************************************
// 声明：BOOL WINAPI Table_FindNext( HANDLE hFind, LPFINDSTRUCT lpfs )
// 参数：
//	IN hFind - 查找对象句柄
//	IN lpfs- FINDSTRUCT指针结构，用于接受查找到的数据
// 返回值：
//	成功：返回TRUE, lpfs结构保存符合条件的记录信息
//	否则：返回FALSE
// 功能描述：
//	查找下一条符合条件的记录	
// 引用: 
//	系统API 
// ************************************************

BOOL WINAPI Table_FindNext( HANDLE hFind, LPFINDSTRUCT lpfs )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    PTABLEFIND pFind = _GetHTABLEFINDPtr(hFind);
    UINT f;

	if( pFind )
    {
		if( ( lpTableFile = _GetHTABLEPtr( pFind->hTable ) ) && 
            ( lpTableFile->dwAccessMode & GENERIC_READ ) &&
			( lpTable = lpTableFile->lpTable ) )  // 有查找句柄得到搜索对象指针
        {
			if( LockRecord( lpTableFile, 0 ) )	// 锁住数据表
			{
				if( lpTable->dwRN )	//
				{	
					while( lpTable->dwRN < lpTable->tfh.nNodeNum )
					{	// 读取记录数据
						lpTableFile->rn = _SetRecordPointerAndUpdate( lpTable, lpTable->dwRN + 1 );
						if( (f = _IsMatch( pFind )) != 0 )	// 记录是否符合要求
						{	// 符合要求
							lpfs->nRecord = lpTable->dwRN;
							lpfs->uField = f;
							UnlockRecord( lpTableFile );	// 解锁
							return TRUE;
						}
					}
				}
				UnlockRecord( lpTableFile );	// 解锁
			}
		}
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Table_CloseFind( HANDLE hFind )
// 参数：
//	IN hFind - 查找文件句柄
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	关闭搜索句柄	
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI Table_CloseFind( HANDLE hFind )
{
    PTABLEFIND pFind = _GetHTABLEFINDPtr(hFind);
    if( pFind )
    {
        pFind->objType = OBJ_NULL;
		free( pFind );
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：static BOOL DoGetTableInfo( LPTABLE_FILE lpTableFile, LPTABLEINFO lpti )
// 参数：
//		IN lpTableFile - 数据表文件指针
//		IN lpti - TABLEINFO 结构指针，用于接受数据表信息
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	得到数据表信息	
// 引用: 
//	
// ************************************************

static BOOL DoGetTableInfo( LPTABLE_FILE lpTableFile, LPTABLEINFO lpti )
{
	LPTABLE lpTable;

	if( ( lpTable = lpTableFile->lpTable ) )
	{
		if( LockRecord( lpTableFile, 0 ) )	// 锁住数据表
		{	// 
			lpti->dwFieldNum = lpTable->tfh.nFieldNum;
			lpti->dwRecordNum = lpTable->tfh.nNodeNum;
			lpti->dwVersion = lpTable->tfh.ver;
			lpti->dwUserDataSize = MAX_USERDATA;
			// 得到文件时间
			GetFileTime( lpTable->hFile, &lpti->ftCreationTime, &lpti->ftLastAccessTime, &lpti->ftLastWriteTime );
			// 得到文件属性
			lpti->dwFileAttrib = GetFileAttributes( lpTableFile->lpTable->lpszFileName );
			UnlockRecord( lpTableFile );	// 解锁数据表
			return TRUE;
		}
	}
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Table_GetTableInfoByHandle( HANDLE hTable, LPTABLEINFO lpti )
// 参数：
//		IN hTable - 数据表文件句柄
//		IN lpti - TABLEINFO 结构指针，用于接受数据表信息
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	得到数据表信息	
// 引用: 
//	系统API	
// ************************************************

BOOL WINAPI Table_GetTableInfoByHandle( HANDLE hTable, LPTABLEINFO lpti )
{
	if( lpti->dwSize == sizeof( TABLEINFO ) )
	{
		LPTABLE_FILE lpTableFile;
		
		if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) )
		{
			return DoGetTableInfo( lpTableFile, lpti );
		}
	}
	else
	{
		WARNMSG( 1,  ( "error: Call Table_GetTableInfo, LPTABLEINFO's size different.\r\n" ) );
	}

	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI Table_GetTableInfo( LPCTSTR lpcszFileName, LPTABLEINFO lpti )
// 参数：
//		IN lpcszFileName - 文件名
//		IN lpti - TABLEINFO 结构指针，用于接受数据表信息
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	得到数据表信息
// 引用: 
//	系统API	
// ************************************************

BOOL WINAPI Table_GetTableInfo( LPCTSTR lpcszFileName, LPTABLEINFO lpti )
{
	BOOL bRetv  = FALSE;

	if( lpti->dwSize == sizeof( TABLEINFO ) )
	{	// 打开数据表文件
		HANDLE hTable;// = Table_Create( lpcszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );

		{
			CREATE_TABLE ct;
			memset( &ct, 0, sizeof( ct ) );
			
			ct.lpcszFileName = lpcszFileName;
			ct.dwAccess = GENERIC_READ;
			ct.dwCreate = OPEN_EXISTING;
			ct.dwShareMode = FILE_SHARE_READ;//
			hTable = Table_CreateByStruct( &ct );
		}

		
		if( hTable != INVALID_HANDLE_VALUE )
		{	//	
			LPTABLE_FILE lpTableFile;
			
			if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) )
			{
				bRetv = DoGetTableInfo( lpTableFile, lpti ); // 得到数据表信息
			}
			Table_Close( hTable );	// 关闭数据表
		}
	}
	else
	{
		WARNMSG( 1,  ( "error: Call Table_GetTableInfo, LPTABLEINFO's size different.\r\n" ) );
	}
	return bRetv;
}

// **************************************************
// 声明：UINT WINAPI Table_ReadMultiRecords( 
//								HTABLE hTable, 
//								LPMULTI_RECORD lpmultiRecord )
// 参数：
//		IN hTable - 数据表对象句柄
//		IN/OUT lpmultiRecord- 用于保存数据的结构指针
// 返回值：
//	成功：返回返回实际读的记录数
//	否则：返回0
// 功能描述：
//	读多个记录和记录的多可字段数据 
// 引用: 
//	系统API	
// ************************************************
#define DEBUG_ReadMultiRecords 0
UINT WINAPI Table_ReadMultiRecords( 
					HTABLE hTable,					
                    LPMULTI_RECORD lpmultiRecord
                   )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;

//    PRNODEDATA pRNode;
    UINT retv = 0;
//    DWORD size;
//    int i;			
	// 检查参数
	DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords entry.\r\n") ) );
    if( lpmultiRecord &&
		lpmultiRecord->mrHeaderInfo.nStartRecord &&
		lpmultiRecord->mrHeaderInfo.dwSize == sizeof(MULTI_HEADER_INFO) &&
        ( ( lpmultiRecord->mrHeaderInfo.nFileldNumber == 0 ) == ( lpmultiRecord->mrHeaderInfo.lpFileldInfo == NULL ) ) &&
		( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTable = lpTableFile->lpTable ) )
    {
		UINT fields;
		HANDLE hCallerProcess = lpTableFile->hOwnerProc;

		LPUINT lpFieldInfo = lpmultiRecord->mrHeaderInfo.lpFileldInfo;
		fields = lpmultiRecord->mrHeaderInfo.nFileldNumber ? lpmultiRecord->mrHeaderInfo.nFileldNumber : lpTable->tfh.nFieldNum;
		DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: fields=(%d).\r\n"), fields ) );
		if( lpFieldInfo )
		{
			UINT i;
			lpFieldInfo = MapPtrToProcess( lpmultiRecord->mrHeaderInfo.lpFileldInfo, hCallerProcess );
			for( i = 0; i < fields; i++ )
			{
				if( lpFieldInfo[i] == 0 ||
					lpFieldInfo[i] > lpTable->tfh.nFieldNum )
				{   //非法参数
					SetLastError( ERROR_INVALID_PARAMETER );
					return 0;
				}
			}
		}
        DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: LockRecord.\r\n") ) );
		if( LockRecord( lpTableFile, 0 ) )	// 锁住表
		{			
			UINT nRecords = lpmultiRecord->mrHeaderInfo.nRecordNumber;
			UINT nStartRecord = lpmultiRecord->mrHeaderInfo.nStartRecord;
			FIELDITEM * lpFiledItem = lpmultiRecord->fields;
			DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: Read records start.\r\n") ) );
			
			for( ; nRecords && nStartRecord <= lpTable->tfh.nNodeNum; nStartRecord++, nRecords-- )
			{
				UINT n;
				DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: Read record(no. %d).\r\n"),nRecords ) );

				_SetRecordPointerAndUpdate( lpTable, nStartRecord );
				n = 0; 
				if( lpFieldInfo )
				{	// 读部分 field
					while( n < fields )
					{
						LPBYTE lpData = MapPtrToProcess( lpFiledItem->lpData, hCallerProcess );
						DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: Read field(no. %d), buf=(0x%x), bufsize(%d).\r\n"),lpFieldInfo[n], lpFiledItem->lpData, lpFiledItem->size ) );
						_ReadField( lpTable, lpFieldInfo[n]-1, 0, lpData, lpFiledItem->size );
						n++;
						lpFiledItem++;
					}
				}
				else
				{ // 读全部 field
					while( n < fields )
					{
						LPBYTE lpData = MapPtrToProcess( lpFiledItem->lpData, hCallerProcess );
						
						DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: Read field(no. %d), buf=(0x%x), bufsize(%d).\r\n"), n+1, lpFiledItem->lpData, lpFiledItem->size ) );
						_ReadField( lpTable, n, 0, lpData, lpFiledItem->size );
						n++;
						lpFiledItem++;
					}
				}
			}
            DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: UnlockRecord.\r\n") ) );
			UnlockRecord( lpTableFile );	// 解锁
			retv = lpmultiRecord->mrHeaderInfo.nRecordNumber - nRecords;
		}
    }
	else
	{
		SetLastError( ERROR_INVALID_PARAMETER );
	}
	DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: leave.\r\n") ) );
    return retv;
}

// **************************************************
// 声明：UINT WINAPI Table_GetUserData( 
//					HTABLE hTable,					
//                  LPVOID lpUserBuf,
//				    UINT nBufSize
// 参数：
//		IN hTable - 数据表对象句柄
//		OUT lpUserBuf- 用于得到用户数据的用户指针
//		IN  nBufSize - 用户数据的长度
// 返回值：
//	成功：返回返回实际读的数据长度
//	否则：返回0
// 功能描述：
//	读去用户自定义数据
// 引用: 
//	系统API
// ************************************************
#define DEBUG_ReadUserData 0
UINT WINAPI Table_GetUserData( 
					HTABLE hTable,					
                    LPVOID lpUserBuf,
					UINT nBufSize
                   )
{
	LPTABLE_FILE lpTableFile;
		
	if( lpUserBuf && 
		nBufSize && 
		( lpTableFile = _GetHTABLEPtr( hTable ) ) )
	{
		LPTABLE lpTable;
		if( lpTableFile->dwAccessMode & GENERIC_READ )
		{
			if( ( lpTable = lpTableFile->lpTable ) )
			{
				if( LockRecord( lpTableFile, 0 ) )	// 锁住数据表
				{	// 
					nBufSize = MIN( nBufSize, MAX_USERDATA );
					memcpy( lpUserBuf, lpTable->tfh.bUserData, nBufSize );
					
					UnlockRecord( lpTableFile );	// 解锁数据表
					return nBufSize;
				}
			}
		}
		else
		{ //存取错误
			SetLastError( ERROR_ACCESS_DENIED );
		}
	}
	else
	{
		SetLastError( ERROR_INVALID_PARAMETER );
	}
	return 0;
}

// **************************************************
// 声明：UINT WINAPI Table_SetUserData( 
//					HTABLE hTable,					
//                  LPCVOID lpUserBuf,
//				    UINT nWriteSize
// 参数：
//		IN hTable - 数据表对象句柄
//		IN lpUserBuf- 用于写到数据表的用户数据的指针
//		IN  nWriteSize - 需要写的用户数据的长度
// 返回值：
//	成功：返回返回实际写的数据长度
//	否则：返回0
// 功能描述：
//	写入/更新用户自定义数据
// 引用: 
//	系统API
// ************************************************
#define DEBUG_SetUserData 0
UINT WINAPI Table_SetUserData( 
					HTABLE hTable,					
                    LPCVOID lpUserBuf,
					UINT nWriteSize
                   )
{
	LPTABLE_FILE lpTableFile;
		
	if( lpUserBuf && 
		nWriteSize && 
		nWriteSize < MAX_USERDATA &&
		( lpTableFile = _GetHTABLEPtr( hTable ) ) )
	{
		LPTABLE lpTable;

		if( lpTableFile->dwAccessMode & GENERIC_WRITE )
		{			
			if( ( lpTable = lpTableFile->lpTable ) )
			{
				if( LockRecord( lpTableFile, LRF_SET_FLUSH_FLAG ) )	// 锁住数据表
				{	// 
					nWriteSize = MIN( nWriteSize, MAX_USERDATA );
					memcpy( lpTable->tfh.bUserData, lpUserBuf, nWriteSize );
					
					UnlockRecord( lpTableFile );	// 解锁数据表
					return nWriteSize;
				}
			}
		}
		else
		{
			SetLastError( ERROR_ACCESS_DENIED );
		}
	}
	else
	{
		SetLastError( ERROR_INVALID_PARAMETER );
	}
	return 0;
}

// **************************************************
// 声明：BOOL WINAPI Table_Flush( HTABLE hTable )
// 参数：
//		IN hTable - 数据表对象句柄
// 返回值：
//	成功：返回返回TRUE
//	否则：返回FALSE
// 功能描述：
//	刷新数据（写入磁盘）
// 引用: 
//	系统API
// ************************************************
#define DEBUG_Flush 0
#define DEBUG_DOFLUSH 1
BOOL WINAPI Table_Flush( HTABLE hTable )
{
	LPTABLE_FILE lpTableFile;
	
	if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) )
	{
		LPTABLE lpTable;
		DEBUGMSG( DEBUG_Flush, (TEXT(" Table_Flush entry.\r\n") ) );

		//需要更新吗 ？
		if( ( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
			( lpTable = lpTableFile->lpTable ) )
		{
			if( LockRecord( lpTableFile, 0 ) )	// 锁住数据表
			{	
				if( lpTable->bFlush )
				{
					WORD n = GetBlockNum( lpTable, lpTable->tfh.wFatStartBlock );
					// 更新数据表FAT
					DEBUGMSG( DEBUG_Flush | DEBUG_DOFLUSH, (TEXT(" Table_Flush: flushing(0x%x)...\r\n"), hTable ) );

					WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, n * lpTable->tfh.nBytesPerBlock );
					//WriteBlocks( lpTable, 0, 0, (BYTE*)&lpTable->tfh  , sizeof( lpTable->tfh ) );
					UpdateTableHead( lpTable );
					
					FlushFileBuffers( lpTableFile->lpTable->hFile );
					lpTable->bFlush = FALSE;
				}
				UnlockRecord( lpTableFile );	// 解锁数据表
				return TRUE;
			}
		}		
	}
	return FALSE;
}

#ifdef TEST_TABLE
//=================================================================//

LRESULT  CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                      LPSTR     lpCmdLine,int       nCmdShow)
{
    HTABLE hTable = Table_Create("test.dat", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 4 );
    

    if( hTable != INVALID_HANDLE_VALUE )
    {
        FIELDITEM fi[4];
        FIELDITEM fir[4];
        BYTE buf[1024], bufr[1024], * p;
        DWORD dwSize;
        int i, n;
        int mid;
        int unit = 4;
        int count;
        int randNum;
        BOOL bExit = FALSE;

        for( i = 1; i < 1024; i++ )
            buf[i] = i;
        
        p = buf;
        
        fi[0].size = 10;
        fi[0].lpData = p;
        p+= fi[0].size;
        
        fi[1].size = 20;
        fi[1].lpData = p;
        p+= fi[1].size;
        
        fi[2].size = 30;
        fi[2].lpData = p;
        p+= fi[2].size;
        
        fi[3].size = 40;
        fi[3].lpData = p;
        p+= fi[3].size;

        while( !bExit )
        {            
            count = Table_CountRecord(hTable);
            if( count > 10000 )
            {
                if( rand() < RAND_MAX / 2 )
                {                    
                    int n = rand();
                    if( n > 10000 )
                        n /= 4;
                    for( i = 0; i <n; i++ )
                    {
                        Table_SetRecordPointer( hTable, rand() / (RAND_MAX / count), SRP_BEGIN );
                        Table_DeleteRecord( hTable, DR_CURRENT, DR_REMOVE );
                        count = Table_CountRecord(hTable);
                    }
                    Table_Close( hTable );
                    Table_Pack("test.dat", PT_NOREMOVE);
                    hTable = Table_Create("test.dat", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 4 );
                }
            }
            if( count == 0 )
                count = 1;
            randNum = rand();
            if( randNum < RAND_MAX / unit )
                Table_AppendRecord( hTable, fi );
            else if( randNum < (RAND_MAX / unit) * 2 )
                Table_InsertRecord( hTable, rand() / (RAND_MAX / count), fi );
            else if( randNum < (RAND_MAX / unit) * 3 )
            {
                Table_SetRecordPointer( hTable, rand() / (RAND_MAX / count), SRP_BEGIN );
                Table_DeleteRecord( hTable, DR_CURRENT, DR_REMOVE );
            }
            else if( randNum < (RAND_MAX / unit) * 4 )
            {
                Table_SetRecordPointer( hTable, rand() / (RAND_MAX / count), SRP_BEGIN );
                Table_WriteRecord( hTable, fi );
            }

            memset( bufr, 0, sizeof( bufr ) );
            count = Table_CountRecord(hTable);
            if( count )
            {
                Table_SetRecordPointer( hTable, rand() / (RAND_MAX / count), SRP_BEGIN );
                Table_ReadRecord(hTable, bufr, fir, &dwSize );

                for( n = 0; n < dwSize; n++ )
                {
                    if( bufr[n] != buf[n] )
                    {
                        _ASSERT( 0 );
                    }
                }
            }
        }        

        for( i = 0; i < 0; i++ )
        {
            mid = Table_CountRecord(hTable) / 2;
            Table_SetRecordPointer( hTable, mid, SRP_BEGIN );
            Table_DeleteRecord(hTable, DR_CURRENT, DR_REMOVE);
        }

        Table_Close(hTable);
        Table_Pack( "test.dat", PT_NOREMOVE );
    }
    return 0;
}

#endif    // TEST_TABLE
///================================================================
