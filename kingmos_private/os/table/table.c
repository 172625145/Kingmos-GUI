/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����KTABLE-���ݿ����ģ��
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2005-01-21, Table_Pack ���� �������
    2004-07-27, ���ӻ������빦�� �� �û�������
	2004-06-18, ���ӹ̶��ļ��ߴ��С�Ĺ��ܣ������ݱ���ļ���С���ᶯ̬�任

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

#define  BLOCK_SIZE 128			//Ĭ�ϵĿ��С
#define MAX_PASSWORD    16
#define MAX_USERDATA    48
//static const TCHAR id[] = TEXT("KTABLE  ");
static const TCHAR id[] =   TEXT("΢�߼�-�������ݿ�");
#define ID_ALIGN_SIZE   ( ( sizeof(id) + 3 ) & (~3) )
//�������ݱ��ļ�ͷ�ṹ
typedef struct _TABLE_FILE_HEAD
{
    TCHAR idName[ID_ALIGN_SIZE]; // ʶ���� = id
    DWORD ver;       // �汾��
	WORD wCheckSum;	 // ����ֵ���ֽں�
    
    WORD nNodeNum;	// ��¼�ڵ���
    WORD nFieldNum;	// �ֶ���
    WORD nNodeDataSize;   // �ڵ�����ݴ�С

    WORD wTotalBlock;	//�ļ���Ŀ���
    WORD wFatStartBlock;  //���������ڵĿ�� file alloc table start sector
    WORD wNodeIndexStartBlock;  //�ڵ����ڵĿ��
    //WORD nFatBlockNum;    // file alloc sector number    
    WORD wFreeBlock;	//���п�����ĵ�һ����
    WORD nFreeCount;	//���п���
    WORD nBytesPerBlock;//ÿ����Ĵ�С

	DWORD dwFixFileSize;	//����̶��ļ���С���ó�Ա > 0

	BYTE szPassword[MAX_PASSWORD];    //������
	DWORD dwPasswordID;    // = sum(password[0...16]) | (n << 12) | ����ֵ | ��СдӢ�� | �д�дӢ�� | �з��� | ��>0x80���ַ� | ���� | ��Ч 
	                       //    12 bits +              4bit 
	BYTE bUserData[MAX_USERDATA];		//�û��Զ�������

}TABLE_FILE_HEAD, * PTFH;

#define RA_LOGIC_DELETE   0x80
// ����ڵ�ṹ
typedef struct _RNODEINDEX
{
    BYTE bAttrib; //  �ýڵ�����
	BYTE bReserve; // ���� for add tount index count or other option
    WORD wDataStartBlock;  //�ýڵ����ݿ�ʼ�� data start block    
}RNODEINDEX, * PRNODEINDEX;

// �����¼��ÿ���ֶγ��Ƚṹ
typedef struct _RNODEDATA
{
    WORD nFieldSize[1];
}RNODEDATA, * PRNODEDATA;

#define RNODEDATA_SIZE( n ) ( sizeof( RNODEDATA ) + ((n) - 1) * sizeof(WORD) )

// ����򿪵����ݱ����ṹ
typedef struct _TABLE
{
	LPTSTR lpszFileName; // ���ݱ��ļ���
	DWORD dwRefCount; // �����ݱ�������
    HANDLE hFile; //�򿪵��ļ����
    TABLE_FILE_HEAD  tfh; // �����ݱ��ļ�ͷ
    WORD *pfat; //�����ݱ���������ڴ��еĵ�ַ
    DWORD dwRN;   // ��ǰ��¼�ļ�¼�� current record no.
    RNODEINDEX RNodeIndex; // ��ǰ��¼�ڵ�����
    PRNODEDATA pRNodeData;  // ��ǰ��¼�ֶ����� curecord record node
    CRITICAL_SECTION csTable;  // �ô����ݱ�ĳ�ͻ��
	BYTE bIsEncrypt;	// ���ݷ����
	BYTE bEncryptKey;	//���ܼ�ֵ
	BYTE bFlush;    //�Ƿ���Ҫˢ��
	BYTE dump;
}TABLE, FAR * LPTABLE;

// �Ѵ����ݱ��������ṹ
typedef struct _TABLE_FILE
{
	DWORD   objType; //�������ͣ�must be OBJ_TABLE
	struct _TABLE_FILE * lpNext; //ָ����һ�����ݱ����
	DWORD  dwShareMode;//�����ݱ���ģʽ
	DWORD dwAccessMode; //�����ݱ��ȡģʽ
	HANDLE hOwnerProc;//�����ݱ��ӵ���߽��̣�owner process;
	LPTABLE lpTable;//�����ݱ��ļ������������ݱ����ṹ
    DWORD rn;   //��ǰ�ѱ�����ļ�¼�����ڼ��ٴ�ȡ��ǰ��¼ current record no.

}TABLE_FILE, * LPTABLE_FILE;

// �Ѵ����ݱ���Ҷ���ṹ
typedef struct _TABELFIND
{
	UINT objType;    //�������ͣ�=OBJ_FINDTABLE
    HTABLE hTable;	//�򿪵����ݱ������ 
    UINT uFieldFrom;	//���ҵĿ�ʼ�ֶ�
    UINT uFieldTo;	//���ҵĽ����ֶ�
    void * lpvFind;	//���ҵ�����
	void * lpvBuf;  //��ʱ���壬���ڶ�ȡ������
    UINT uLen;//���ҵ����ݳ���
}TABELFIND, * PTABLEFIND;

#define TABLE_ERROR 1

static BOOL MoveData( LPTABLE lpTable, WORD blockIndex, long posMoveFrom, long posMoveTo );
static BOOL _InsertRecord( LPTABLE lpTable, WORD rn, FIELDITEM fieldItem[], HANDLE hOwnerProc );
static BOOL _Exchange( LPTABLE lpTable, WORD rn0, WORD rn1 );
static BOOL _CopyRecordData( LPTABLE lpTable, FIELDITEM fieldItem[], DWORD size, BOOL bFrag, HANDLE hOwnerProc );
static WORD _SetRecordPointerAndUpdate( LPTABLE lpTable, long rn );
// ���ݱ���������ͻ��
static CRITICAL_SECTION csTableList;
// ���ݱ��������ͷ
static LPTABLE_FILE lpTableList = NULL;

// **************************************************
// ������BOOL InitialTable( void )
// ������
//		��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		��ʼ�����ݱ�ȫ����
// ����: 
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
// ������static LPTABLE_FILE _GetHTABLEPtr( HANDLE handle )
// ������
//		IN handle - ���ݱ���
// ����ֵ��
//		����ɹ������ط�NULLָ�룻���򣬷���NULL
// ����������
//		�ɾ���õ����ݱ����ָ��
// ����: 
//		
// ************************************************

static LPTABLE_FILE _GetHTABLEPtr( HANDLE handle )
{
	LPTABLE_FILE lpTableFile = (LPTABLE_FILE)handle;
	if( handle && 
		handle != INVALID_HANDLE_VALUE && 
		lpTableFile->objType == OBJ_TABLE )	//����Ƿ���Ч
	{	//��
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
// ������static PTABLEFIND _GetHTABLEFINDPtr( HANDLE handle )
// ������
//		IN handle - ���ݱ���
// ����ֵ��
//		����ɹ������ط�NULLָ�룻���򣬷���NULL
// ����������
//		�ɾ���õ���������ָ��
// ����: 
//		
// ************************************************

static PTABLEFIND _GetHTABLEFINDPtr( HANDLE handle )
{
	PTABLEFIND lpFind = (PTABLEFIND)handle;
	if( lpFind && lpFind->objType == OBJ_FINDTABLE ) // �Ƿ���Ч ��
	{	//��
		return lpFind;
	}
	else
		SetLastError( ERROR_INVALID_PARAMETER );
	ERRORMSG( 1, ( "Invalid table handle(0x%x).\r\n", handle ) );
	return NULL;
}

// **************************************************
// ������static BOOL FreeBlock( LPTABLE lpTable, WORD wStartBlock ) 
// ������
//		IN lpTable - ���ݱ����
//		IN wStartBlock - ��Ҫ�ͷŵĿ����׵�ַ	
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ͷſ飬���ͷŵĿ�������ݱ�Ŀ��п���
// ����: 
//		
// ************************************************

static BOOL FreeBlock( LPTABLE lpTable, WORD wStartBlock ) 
{
    WORD * pfat = lpTable->pfat; // �����ݱ�ķ����
    WORD n, w, k;

    ASSERT( wStartBlock != NULL_BLOCK );
    k = w = wStartBlock;
	// �õ������е����һ����
    for( n = 0; w != NULL_BLOCK; n++ )
    {
        k = w;
        w = NEXT_BLOCK( pfat, w ); // �ڿ����е���һ����
    }
	// k �����һ���������
    *(pfat+k) = lpTable->tfh.wFreeBlock; // ���һ�����������ݱ��еĿ��п�
    lpTable->tfh.wFreeBlock = wStartBlock;	// �����趨���ݱ���׿��п�
    lpTable->tfh.nFreeCount += n; // ���ӿ��п���
    return TRUE;
}

/*
// **************************************************
// ������static BOOL FreeBlock( LPTABLE lpTable, WORD wStartBlock ) 
// ������
//		IN lpTable - ���ݱ����
//		IN wStartBlock - ��Ҫ�ͷŵĿ����׵�ַ	
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ͷſ飬���ͷŵĿ鰴����������������ݱ�
// ����: 
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
// ������static WORD AllocBlock( LPTABLE lpTable, WORD nBlockNum )
// ������
//		IN lpTable - ���ݱ����
//		IN nBlockNum - ��Ҫ��ϵͳ����Ŀ���
// ����ֵ��
//		����ɹ������ط�NULL_BLOCKֵ�����򣬷���NULL_BLOCK
// ����������
//		�����ݱ����һ�������Ŀ�
// ����: 
//		
// ************************************************

static WORD AllocBlock( LPTABLE lpTable, WORD nBlockNum )
{
    WORD first, n, k;
    WORD * pfat;
	
    if( nBlockNum > 0 && lpTable->tfh.nFreeCount >= nBlockNum ) // �Ƿ����㹻�Ŀ� ��
    {   // ��
        first = lpTable->tfh.wFreeBlock;
        k = (WORD)(nBlockNum-1);
        pfat = lpTable->pfat;  // ���ݱ�����
        for( n = 0; n < k; n++ ) // �ȵõ� nBlockNum-1 �������һ����ѭ�������
        {
            first = NEXT_BLOCK( pfat, first );
        }
		// �������һ����
        n = first;  // end block
        first = NEXT_BLOCK( pfat, first);  // get next block
        *(pfat+n) = NULL_BLOCK; // ���һ����ָ�������

        // ���ѷ���Ŀ�����ݱ��Ƴ��������ط���Ŀ� save return value
        n = lpTable->tfh.wFreeBlock;  // ������׿�ֵ
        lpTable->tfh.wFreeBlock = first; // �������ݱ��׿��п�ֵ
        // �������ݱ���п�����reset free block and count
        lpTable->tfh.nFreeCount -= (WORD)nBlockNum;
        return n;
    }
    return NULL_BLOCK;
}

// **************************************************
// ������static WORD ReallocBlock(
//                          LPTABLE lpTable, 
//                          WORD wAllocBlock, 
//                          WORD nBlockNum )

// ������
//		IN lpTable - ���ݱ����ָ��
//		IN wAllocBlock - ֮ǰ�Ѿ�����Ŀ����������ΪNULL_BLOCK���ͬ��AllocBlock
//		IN nBlockNum - ��Ҫ���·���Ŀ���,���Ϊ0,���ͬ��FreeBlock
// ����ֵ��
//		����ɹ������ط�NULL_BLOCKֵ�����򣬷���NULL_BLOCK
// ����������
//		�ı��Ѿ�����Ŀ��������·����µĿ���
// ����: 
//		
// ************************************************

static WORD ReallocBlock(
                          LPTABLE lpTable, 
                          WORD wAllocBlock, 
                          WORD nBlockNum )
{
    WORD i;

    if( wAllocBlock == NULL_BLOCK )
    {	// ��ͬ��AllocBlock
        return AllocBlock( lpTable, nBlockNum );
    }
    else if( nBlockNum == 0 )
    {   // ��ͬ��FreeBlock, �ͷ����еĿ顣free all block
        FreeBlock( lpTable, wAllocBlock );
        return NULL_BLOCK;
    }
    else
    {
        WORD s = wAllocBlock;
        WORD p = wAllocBlock;
        WORD * pfat = lpTable->pfat;  // ���ݱ�ķ����
        i = 0;
		//�����������
        while( s != NULL_BLOCK )
        {
            i++;
            if( i > nBlockNum ) // �ѷ�����������·������ ��
            {  // �ǣ��ض��ѷ���������ͷ����µĿ顣decrease sector, to free other sector
                *(pfat+p) = NULL_BLOCK;
                FreeBlock( lpTable, s );
                return wAllocBlock;
            }
            p = s; // ����ǰһ����
            s = NEXT_BLOCK( pfat, s ); // ��һ����
        }  
		// 
        if( i < nBlockNum ) // �ѷ������ С��< �·�������� �� 
        {	// �ǣ������µĿ�����������������һ��
            s = (WORD)AllocBlock( lpTable, (WORD)(nBlockNum - i) );
            if( s != NULL_BLOCK )
                *(pfat+p) = s;   // link them
            return wAllocBlock;
        }
        else if( i == nBlockNum ) // ��Ȼ��ȣ��������κ���
            return wAllocBlock;
    }
    return NULL_BLOCK;
}
/*
// **************************************************
// ������static WORD GetBlockOffset( LPTABLE lpTable, WORD wStartBlock, WORD nOffset )
// ������
//		IN lpTable - ���ݱ�ָ��
//		IN wStartBlock - ��ʼ��
//		IN nOffset - �ӿ�ʼ��������Ŀ���
// ����ֵ��
//		������
// ����������
//		�õ��ӿ�ʼ��������ĵ�nOffset����Ŀ��
// ����: 
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
// ������static WORD GetBlockNum( LPTABLE lpTable, WORD wStartBlock )
// ������
//		IN lpTable - ���ݱ����ָ��
//		IN wStartBlock - ��ʼ������
// ����ֵ��
//		����ɹ������ؿ��������򣬷���0
// ����������
//		�õ��������еĿ���
// ����: 
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
// ������static BOOL ReallocFile( LPTABLE lpTable, WORD wFirstBlock, DWORD dwNewSize, WORD * lpwAlloc )
// ������
//		IN lpTable - ���ݱ����ָ��
//		IN wFirstBlock - ��ʼ��
//		IN dwNewSize - �µ��ļ���С
//		OUT lpwAlloc - ���ڽ����·���Ŀ�����
// ����ֵ��
//		����ɹ�������TRUE, lpwAllocΪ�µĿ����������򣬷���NULL_BLOCK
// ����������
//		Ϊһ���������·����С�������ǰ���ݱ��ļ�û���㹻�Ŀ飬���������ݱ��ļ��Ĵ�С
// ����: 
//		
// ************************************************

#define FAT_BLOCK_NUM( lpTable, bn ) ( ( (bn)*sizeof(WORD)+(lpTable)->tfh.nBytesPerBlock-1 ) / (lpTable)->tfh.nBytesPerBlock )

static BOOL ReallocFile( LPTABLE lpTable, WORD wFirstBlock, DWORD dwNewSize, WORD * lpwAlloc )
{
    WORD wStartBlock = NULL_BLOCK;
    long num = (WORD)( (dwNewSize + lpTable->tfh.nBytesPerBlock - 1) / lpTable->tfh.nBytesPerBlock );

    if( num )  // numΪ�µĿ���
    {
        long bn = lpTable->tfh.nFreeCount - num + GetBlockNum( lpTable, wFirstBlock ); // ������
        if( bn >= 0  )  // ��ǰ���ݱ����㹻�Ŀ��� ��
            wStartBlock = ReallocBlock( lpTable, wFirstBlock, (WORD)num ); // �У�ֱ�ӷ���
        else if( lpTable->tfh.dwFixFileSize == 0 )	//���Զ�̬�����ļ���С�𡡣�
        {   // û�У���Ҫ�����ļ���С��add file size
            DWORD dwSize;
            WORD nOldFatBlock;
            WORD nNewFatBlock;
			WORD nIndexNumPerBlock;
			WORD nOtherDataBlockNum;
            
            bn = -bn;
            nOldFatBlock = GetBlockNum( lpTable, lpTable->tfh.wFatStartBlock ); // ��ǰ���ݱ�FAT����ռ�Ŀ���
			
			// �������ݱ��С�����ݱ����FAT���Ŀ���
            //new algorithm : X_fatblock + Y_allotherblock  <= X_fatblock * IndexNumOfPerBlock;
			nIndexNumPerBlock = lpTable->tfh.nBytesPerBlock / sizeof( WORD ) - 1;  //
			nOtherDataBlockNum = (WORD)(lpTable->tfh.wTotalBlock - nOldFatBlock + bn); //
			nNewFatBlock = (nOtherDataBlockNum + nIndexNumPerBlock - 1 ) / nIndexNumPerBlock; //
			//

            if( nNewFatBlock > nOldFatBlock )  // �µ�FAT�������ڵ�ǰ���� ��
            {	// �ǣ����·�������ָ��
                void *p = realloc( lpTable->pfat, nNewFatBlock * lpTable->tfh.nBytesPerBlock );
                if( p )
                    lpTable->pfat = p;
                else
                    return FALSE;
            }
			
			bn += (nNewFatBlock - nOldFatBlock); //��Ҫ���ӵ���ʵ����
			dwSize = (lpTable->tfh.wTotalBlock + bn) * lpTable->tfh.nBytesPerBlock; //���ݱ��ļ���С
			// �����ļ���С
            SetFilePointer( lpTable->hFile, dwSize, NULL, FILE_BEGIN );
            if( SetEndOfFile( lpTable->hFile ) )
            {	// �ɹ���alloc file block success
                WORD * pBlock = lpTable->pfat + lpTable->tfh.wTotalBlock;
                WORD i;
				// �����������п�
                for( i = 1; i < bn; i++, pBlock++ )
                    *pBlock = lpTable->tfh.wTotalBlock+i;
                *pBlock = NULL_BLOCK;
				// ���������п����FAT
                FreeBlock( lpTable, lpTable->tfh.wTotalBlock );
				//
				// ����������·���FAT��
                if( nNewFatBlock > nOldFatBlock )
                {
                    if( ReallocBlock( lpTable, lpTable->tfh.wFatStartBlock, nNewFatBlock ) == NULL_BLOCK )
                    {   // ��Ӧ�õ������Ϊ֮ǰ�Ѿ���չ���ļ���
                        _ASSERT( 0 );
                        return FALSE;
                    }
                }
				// ���·�����Ҫ�Ŀ�
                wStartBlock = ReallocBlock( lpTable, wFirstBlock, (WORD)num );
                lpTable->tfh.wTotalBlock += (WORD)bn; // ����ϵͳ������
            }
        }
		// ���÷���ֵ
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
    {   // �ͷŹ��ܣ�free it
        wStartBlock = ReallocBlock( lpTable, wFirstBlock, (WORD)num );        
        *lpwAlloc = NULL_BLOCK;
        return TRUE;
    }
}


// **************************************************
// ������static DWORD ReadBlocks( LPTABLE lpTable, WORD wStartBlock, DWORD nOffset, BYTE * lpBuf, DWORD dwSize )
// ������
//		IN lpTable - ���ݱ����ָ��
//		IN wStartBlock - ��ʼ��
//		IN nOffset - �ӿ�ʼ�����ƫ�ƣ�byteΪ��λ��
//	    OUT lpBuf - ���ڽ������ݵĻ���
//		IN dwSize - ��Ҫ���������ֽ���
// ����ֵ��
//		ʵ�ʶ����ֽ���	
// ����������
//		�����ݿ�wStartBlock��ʼ���ƫ��nOffset�ֽڴ���ȡdwSize������
//		Ӧ�úϲ�ReadBlocks �� WriteBlocks Ϊһ����������
// ����: 
//		
// ************************************************

static DWORD ReadBlocks( LPTABLE lpTable, WORD wStartBlock, DWORD nOffset, BYTE * lpBuf, DWORD dwSize )
{
    WORD nBlockSize = (WORD)lpTable->tfh.nBytesPerBlock;
    DWORD s = dwSize;
    HANDLE hFile = lpTable->hFile;
    DWORD dwRWValue;

	// ���ƫ��ֵ����һ�����С������һ����
    while( nOffset >= nBlockSize )
    {
        wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
        nOffset -= nBlockSize;
    }
	// 
    if( wStartBlock != NULL_BLOCK )
    {	// �ƶ��ļ���ȡλ�õ���һ������λ��  
        SetFilePointer( hFile, wStartBlock * nBlockSize + nOffset, NULL, FILE_BEGIN );
		
        if( dwSize > nBlockSize - nOffset ) // ��Ҫ��ȡ�������ֽ���������һ�������� ��
        {	// ���ǣ���ȡ���ڴ�ƫ�ƴ�����β����������
            ReadFile( hFile, lpBuf, nBlockSize-nOffset, &dwRWValue, NULL );
            dwSize -= nBlockSize - nOffset;
            lpBuf += nBlockSize - nOffset;
        }
        else
        {	// ���ǣ���ȡһ����
            ReadFile( hFile, lpBuf, dwSize, &dwRWValue, NULL );
            dwSize =  0;  // ��Ȼ����
        }       

		wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
        while( dwSize )  // ��ȡʣ�µĲ���ֱ�����
        {
            if( wStartBlock != NULL_BLOCK )
            {	// �ƶ�����һ����
				WORD wNextBlock, wPrevBlock;
				DWORD dwReadSize = nBlockSize;

				wPrevBlock = wStartBlock;
				//�Ƿ��������Ŀ�?
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
// ������static DWORD WriteBlocks( LPTABLE lpTable, WORD wStartBlock, DWORD nOffset, LPCTSTR lpBuf, DWORD dwSize )
// ������
//		IN lpTable - ���ݱ����ָ��
//		IN wStartBlock - ��ʼ��
//		IN nOffset - �ӿ�ʼ�����ƫ�ƣ�byteΪ��λ��
//	    IN lpBuf - ��������Ҫд������ݻ���
//		IN dwSize - ��Ҫд�������ֽ���
// ����ֵ��
//		ʵ��д���ֽ���	
// ����������
//		�����ݿ�wStartBlock��ʼ���ƫ��nOffset�ֽڴ�д��dwSize������
//		Ӧ�úϲ�ReadBlocks �� WriteBlocks Ϊһ����������
// ����: 
//		
// ************************************************

static DWORD WriteBlocks( LPTABLE lpTable, WORD wStartBlock, DWORD nOffset, LPCTSTR lpBuf, DWORD dwSize )
{
    WORD nBlockSize = (WORD)lpTable->tfh.nBytesPerBlock;
    HANDLE hFile = lpTable->hFile;
    DWORD s = dwSize;
    DWORD dwRWValue;

	// ���ƫ��ֵ����һ�����С������һ����

    while( nOffset >= nBlockSize )
    {  
        wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
        nOffset -= nBlockSize;
    }

    if( wStartBlock != NULL_BLOCK )
    {	// �ƶ��ļ�д��λ�õ���һ������λ��
        SetFilePointer( hFile, wStartBlock * nBlockSize + nOffset, NULL, FILE_BEGIN );
        if( dwSize > nBlockSize - nOffset )	// ��Ҫд��������ֽ���������һ�������� ��
        {	// ���ǣ�д����ڴ�ƫ�ƴ�����β����������
            WriteFile( hFile, lpBuf, nBlockSize - nOffset, &dwRWValue, NULL );
            dwSize -=  nBlockSize - nOffset;
            lpBuf += nBlockSize - nOffset;
        }
        else
        {	// ���ǣ�д��һ����
            WriteFile( hFile, lpBuf, dwSize, &dwRWValue, NULL );
            dwSize =  0;
        }       

		wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
        while( dwSize ) // д��ʣ�µĲ���ֱ�����
        {         
            if( wStartBlock != NULL_BLOCK )
            {	// �ƶ�����һ����
				WORD wNextBlock, wPrevBlock;
				DWORD dwWriteSize = nBlockSize;

				wPrevBlock = wStartBlock;
				//�Ƿ��������Ŀ顡��
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
// ������static BOOL ReadFatBlocks( LPTABLE lpTable )
// ������
//		IN lpTable - ���ݱ����ָ��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�� FAT�����ڵ����ݿ�
// ����: 
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
	{	//���ǹ̶���
		DWORD dwRead;
		SetFilePointer( lpTable->hFile, wStartBlock * dwBlockSize, NULL, FILE_BEGIN );
		ReadFile( lpTable->hFile, lpTable->pfat, size, &dwRead, NULL );
		size -= dwRead;
	}
	else
	{   //���ǿɱ��
        while( wStartBlock != NULL_BLOCK )
        {	// ��ȡ���ݱ��ļ���FAT����
            ReadBlocks( lpTable, wStartBlock, 0, p, dwBlockSize );
            wStartBlock = NEXT_BLOCK( lpTable->pfat, wStartBlock );
            p += dwBlockSize;
            size -= dwBlockSize;
        }
		ASSERT( size == 0 );
	}
	return !size;  //sizeӦ��Ϊ0

}



// **************************************************
// ������static BOOL MoveData( LPTABLE lpTable, WORD firstBlock,  long posMoveFrom, long posMoveTo )
// ������
//		IN lpTable - ���ݱ����ָ��
//		IN firstBlock - ��һ����
//		IN posMoveFrom - �ƶ�Դλ��
//		IN posMoveTo - �ƶ�Ŀ��λ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�����ݴ�һ��λ���ƶ�����һ��λ��
// ����: 
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

    if( posMoveFrom > posMoveTo ) // Դλ����Ŀ��λ�õĺ����� �� 
    {	// ��
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
    {	// Դλ����Ŀ��λ�õ�ǰ�棬��β����ȡ
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
                break;    // ���������Ѿ���ɡ�all data has been moved
            endPos = startPos;
            startPos -= sizeof( buf );
            startPos = MAX( startPos, posMoveFrom );
        }
        retv = TRUE;
    }
    return retv;
}

#define PSW_VALID            0x80000000    //����������Ч
#define PSW_SAFE_TYPE_MASK   0x70000000    //�������ͣ����Ϊ0�������ܣ����������ݱ���ͼ�¼���ݱ����ǲ�����
                                           //          ���Ϊ1��Ĭ�ϼ��ܷ��������������ݱ�����0xd4 XOR�ͼ�¼������������ݵĺͽ���XOR
#define GET_PSW_TYPE( dwID ) ( ( (dwID) & PSW_SAFE_TYPE_MASK ) >> 28 )
// **************************************************
// ������static DWORD CheckPassword( LPCBYTE lpszPassword )
// ������
//		IN lpszEntryPassword - �û��������
// ����ֵ��
//		���ؿ����ID
// ����������
//		�õ������ID
// ����: 
//		
// ************************************************
static BOOL CheckPassword( LPTABLE lpTable, LPCBYTE lpszEntryPassword )
{
	DWORD id = lpTable->tfh.dwPasswordID;
	if( id & PSW_VALID )
	{
		if( GET_PSW_TYPE( id ) == 0 )
		{  //����
			if( strncmp( lpszEntryPassword, lpTable->tfh.szPassword, 16 ) == 0 )
			{   //��ȷ
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
				return TRUE;//��ȷ
			}
			return FALSE;
		}
		return FALSE;
	}
	//������Ч
	return TRUE;	
}

// **************************************************
// ������static WORD GetCheckSum( LPTABLE lpTable )
// ������
//		IN lpTable - ���ݱ����ָ��
// ����ֵ��
//		�������ݱ��checksum
// ����������
//		�õ����ݱ��checksum
// ����: 
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
// ������static BOOL UpdateTableHead( LPTABLE lpTable )
// ������
//		IN lpTable - ���ݱ����ָ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�������ݱ�ͷ
// ����: 
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
// ������static BOOL UpdateNodeIndex( LPTABLE lpTable )
// ������
//		IN lpTable - ���ݱ����ָ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		���¼�¼�ڵ����ݵ��ļ�
// ����: 
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
// ������static BOOL UpdateNodeData( LPTABLE lpTable )
// ������
//		IN lpTable - ���ݱ����ָ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		���¼�¼���ݵ��ļ�		
// ����: 
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
// ������static BOOL _CopyRecordData( LPTABLE lpTable, FIELDITEM fieldItem[], DWORD size, BOOL bFrag, HANDLE hOwnerProc )
// ������
//		IN lpTable - ���ݱ����ָ��
//		IN fieldItem - FIELDITEM �ṹָ��
//		IN size - ������ݵ��ֽ������������bFragΪFALSE, �ò�����Ч, fieldItme[0].lpDataָ���ŵ����ݡ�
//		IN bFrag - �Ƿ������ֶ�֮��������������ġ�������ָ�������¹�ϵ��
//				fieldItem[n-1].lpData + fieldItem[n-1].size = fieldItem[n].lpData
//		IN hOwnerProc - ӵ���߽���
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		���¼�¼���ݡ�����bFragΪFALSE,�����ϵͳ����		
// ����: 
//		
// ************************************************

static BOOL _CopyRecordData( LPTABLE lpTable, FIELDITEM fieldItem[], DWORD size, BOOL bFrag, HANDLE hOwnerProc )
{
    BOOL retv = FALSE;
    WORD wDataStartBlock = lpTable->RNodeIndex.wDataStartBlock;
    WORD nOffset = lpTable->tfh.nNodeDataSize;
    int i;

    if( bFrag == FALSE )
    {   // �����Ķ����ݣ�дһ�Ρ�no fragment, so write once
        if( WriteBlocks( lpTable, wDataStartBlock, nOffset, MapPtrToProcess( fieldItem[0].lpData, hOwnerProc ), size ) )
            retv = TRUE;
    }
    else
    {   // �ֶ��Ƿ������ġ�дÿһ���ֶΡ�has fragmental
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
	// �����ֶ���Ϣ
    for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
    {
        lpTable->pRNodeData->nFieldSize[i] = fieldItem[i].size;
    }
    retv = UpdateNodeData( lpTable );	//д���ļ�
    return retv;
}


// **************************************************
// ������static BOOL _InsertRecord( LPTABLE lpTable, WORD rn, FIELDITEM fieldItem[] )
// ������
//		IN lpTable - ���ݱ����
//		IN rn - ����¼����ļ�¼��
//		IN fieldItem - ��¼����
//		IN hOwnerProc - ӵ���߽���
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�����ݱ����һ����¼
// ����: 
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

    // ͳ�ƴ�С�������Ƿ��������� count size and has fragment ?
    size = 0; lpData = (BYTE*)fieldItem[0].lpData;
    bFrag = FALSE;
    for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
    {
        size += fieldItem[i].size;
        if( lpData == fieldItem[i].lpData )
            lpData += fieldItem[i].size;
        else
            bFrag = TRUE;  // �������� is fragmental
    }

    wDataBlock = NULL_BLOCK;
	// ���·����¼������ռ�ռ�
    ReallocFile( lpTable, NULL_BLOCK, size+lpTable->tfh.nNodeDataSize, &wDataBlock );
    if( wDataBlock != NULL_BLOCK )
    {    
        WORD firstBlock;
		// ���·����¼������ռ�Ŀռ�
        if( ReallocFile( lpTable, lpTable->tfh.wNodeIndexStartBlock, (lpTable->tfh.nNodeNum + 1) * sizeof( RNODEINDEX ), &firstBlock )  )
        {	// �ط���ɹ�����Ϊ�ǲ������ݣ���Ҫ�ƶ���¼������
            lpTable->tfh.wNodeIndexStartBlock = firstBlock;
            if( MoveData(lpTable, lpTable->tfh.wNodeIndexStartBlock, (rn-1)*sizeof( RNODEINDEX ), rn*sizeof( RNODEINDEX ) ) )
            {
                lpTable->tfh.nNodeNum++;
                lpTable->dwRN = rn;  // set as current record
				memset( &lpTable->RNodeIndex, 0, sizeof( RNODEINDEX ) );
                lpTable->RNodeIndex.wDataStartBlock = wDataBlock;
				//	���¼�¼��������
                UpdateNodeIndex( lpTable );
				// ������¼����
                if( _CopyRecordData( lpTable, fieldItem, size, bFrag, hOwnerProc ) )
                    return TRUE;
            }
        }
		//	�����ռ���䲻�ɹ����ͷ�֮ǰ����ļ�¼���ݿռ�
        FreeBlock( lpTable, wDataBlock );
    }
    return retv;
}

// **************************************************
// ������static BOOL _Exchange( LPTABLE lpTable, WORD rn0, WORD rn1 )
// ������
//		IN lpTable - ���ݱ����ָ�� 
//		IN rn0 - ��һ����¼
//		IN rn1 - �ڶ�����¼
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		����������¼֮�������		
// ����: 
//		
// ************************************************

static BOOL _Exchange( LPTABLE lpTable, WORD rn0, WORD rn1 )
{
//    WORD block0, block1;
    RNODEINDEX nodeIndex0, nodeIndex1;
	// ��ȡ��һ����¼����������
    ReadBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, rn0 * sizeof( RNODEINDEX ), (BYTE*)&nodeIndex0, sizeof( RNODEINDEX ) );
    // ��ȡ�ڶ�����¼����������
	ReadBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, rn1 * sizeof( RNODEINDEX ), (BYTE*)&nodeIndex1, sizeof( RNODEINDEX ) );
	// ����һ����¼����������д��ڶ�����¼
    WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, rn0 * sizeof( RNODEINDEX ), (BYTE*)&nodeIndex1, sizeof( RNODEINDEX ) );
    // ���ڶ�����¼����������д���һ����¼
	WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, rn1 * sizeof( RNODEINDEX ), (BYTE*)&nodeIndex0, sizeof( RNODEINDEX ) );

    return TRUE;
}

// **************************************************
// ������static BOOL InitFat( LPTABLE lpTable )
// ������
//		IN lpTable - ���ݱ����ָ�� 
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		��ʼ�����ݱ��FAT		
// ����: 
//		
// ************************************************

static BOOL InitFat( LPTABLE lpTable )
{	// ȷ��FAT�Ĵ�С
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
		//{	//�ɱ��С

		//	{	// ��ȡ���ݱ��ļ���FAT����
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
		//{	//�̶���С
		//}

        if( size == 0 )
        {	// ���꣬�ɹ�
            return TRUE;
        }
		// �������ͷ�FATָ��
        free( lpTable->pfat );
		lpTable->pfat = NULL;
*/
//    }
//    return FALSE;
}

// **************************************************
// ������static BOOL IsValidTable( TABLE_FILE_HEAD * ptfh )
// ������
//		IN ptfh - ���ݱ��ļ�ͷ
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		������ݱ��Ƿ���Ч
// ����: 
//		
// ************************************************

static BOOL IsValidTable( TABLE_FILE_HEAD * ptfh )
{
    if( memcmp( ptfh->idName, id, sizeof( ptfh->idName ) ) == 0 )
    {		
        return GetCheckSum( ptfh ) == ptfh->wCheckSum;	//��Ч ?
    }
    SetLastError( ERROR_INVALID_DATA );
    return FALSE;
}

// **************************************************
// ������static void _AddToTableList( LPTABLE_FILE lpTableFile )
// ������
//		IN lpTableFile - ���ݱ��ļ�����
// ����ֵ��
//		��
// ����������
//		���򿪵����ݱ�������ϵͳ����
// ����: 
//		
// ************************************************

static void _AddToTableList( LPTABLE_FILE lpTableFile )
{
    EnterCriticalSection( &csTableList );

	lpTableFile->hOwnerProc = GetCallerProcess(); // ӵ���߽���
	lpTableFile->lpNext = lpTableList;
	lpTableList = lpTableFile;
	InterlockedIncrement( &lpTableFile->lpTable->dwRefCount ); // ���Ӷ����ݱ�ڵ������

	LeaveCriticalSection( &csTableList );
}

// **************************************************
// ������static void _RemoveFromTableList( LPTABLE_FILE lpTableFile )
// ������
//		IN lpTableFile - ���ݱ��ļ�����
// ����ֵ��
//		��
// ����������
//		��_AddToTableList�෴�������ݱ�����Ƴ�ϵͳ����
// ����: 
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
			InterlockedDecrement( &lpTableFile->lpTable->dwRefCount );// ���ٶ����ݱ�ڵ������
			break;
		}
		lpPrev = lpObj;
		lpObj = lpObj->lpNext;
	}

	LeaveCriticalSection( &csTableList );
}

// **************************************************
// ������static BOOL MakeFixTable( 
//								LPTABLE lpTable,
//								DWORD dwTableFileSize
//								)
// ������
//		IN lpTable - ���ݱ����
//		DWORD dwTableFileSize - ��Ҫ�̶����ļ���С
// ����ֵ��
//		�ɹ�������TRUE; ʧ�ܣ�����FALSE
// ����������
//		���ɹ̶��ļ���С�����ݱ�
// ����: 
//		
// ************************************************

static BOOL MakeFixTable( LPTABLE lpTable, DWORD dwTableFileSize )
{
	DWORD nIndexPerBlocks;
	DWORD nFatBlocks;
	DWORD dwBlockSize = BLOCK_SIZE;	//128

	if( dwTableFileSize / dwBlockSize >= 0xfff0 )
	{	//��ߴ�̫С
		dwBlockSize *= 2;	//256		
		if( dwTableFileSize / dwBlockSize >= 0xfff0 )
		{	//��ߴ�̫С
			dwBlockSize *= 2;	//512
			if( dwTableFileSize / dwBlockSize >= 0xfff0 )
			{	//��ߴ�̫С
				return FALSE;	//��֧��
			}
		}
	}
	lpTable->tfh.nBytesPerBlock = (WORD)dwBlockSize;

	lpTable->tfh.dwFixFileSize = ( (dwTableFileSize + dwBlockSize-1) / dwBlockSize ) * dwBlockSize;
	lpTable->tfh.wTotalBlock = (WORD)(lpTable->tfh.dwFixFileSize / dwBlockSize);//(sizeof( lpTable->tfh ) + BLOCK_SIZE - 1) / BLOCK_SIZE + 1 + 1;
	lpTable->tfh.wFatStartBlock = (sizeof( lpTable->tfh ) + dwBlockSize - 1) / dwBlockSize;//lpTable->tfh.wTotalBlock-2;

    nIndexPerBlocks = lpTable->tfh.nBytesPerBlock / sizeof( WORD );
	nFatBlocks = (lpTable->tfh.wTotalBlock + nIndexPerBlocks - 1) / nIndexPerBlocks;  //

	lpTable->tfh.wNodeIndexStartBlock = (WORD)(lpTable->tfh.wFatStartBlock + nFatBlocks);	//һ��������

	lpTable->tfh.wFreeBlock = lpTable->tfh.wNodeIndexStartBlock + 1;//NULL_BLOCK;
	lpTable->tfh.nFreeCount = lpTable->tfh.wTotalBlock - lpTable->tfh.wNodeIndexStartBlock;
	
	//�����ļ�
	SetFilePointer( lpTable->hFile, lpTable->tfh.dwFixFileSize, NULL, FILE_BEGIN );
	if( SetEndOfFile( lpTable->hFile ) )
	{	//�����ļ��ռ�ɹ�
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

				//����������Ϊ��
				i = WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)lpTable->pfat, dwBlockSize );
				
				//��ʼ����ͷ��
				nBlock = lpTable->tfh.wFatStartBlock-1;
				for( i = 0; i < nBlock; i++ )
					*(lpTable->pfat+i) = i + 1;//�������еĿ��п�
				*(lpTable->pfat+i) = NULL_BLOCK;//���һ����
				i++;
				
				//��ʼ��FAT���
				nBlock = lpTable->tfh.wNodeIndexStartBlock-1;
				for( ; i < nBlock; i++ )
					*(lpTable->pfat+i) = i + 1;//�������еĿ��п�
				*(lpTable->pfat+i) = NULL_BLOCK;//���һ����
				i++;
				
				//��ʼ���������
				nBlock = lpTable->tfh.wFreeBlock - 1;
				for( ; i < nBlock; i++ )
					*(lpTable->pfat+i) = i + 1;//�������еĿ��п�
				*(lpTable->pfat+i) = NULL_BLOCK;//���һ����
				i++;

				//��ʼ�����б��
				nBlock = lpTable->tfh.wTotalBlock - 1;
				for( ; i < nBlock; i++ )
					*(lpTable->pfat+i) = i + 1;//�������еĿ��п�
				*(lpTable->pfat+i) = NULL_BLOCK;//���һ����
				i++;

				// ����FAT������
				i = WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, dwFatSize );
				if( i )
				{
					UpdateTableHead( lpTable );
					return TRUE;
				}
				// 
				// ʧ�ܣ����
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
// ������static BOOL MakeFixTable( 
//								LPTABLE lpTable
//								)
// ������
//		IN lpTable - ���ݱ����
// ����ֵ��
//		�ɹ�������TRUE; ʧ�ܣ�����FALSE
// ����������
//		���ɿɱ��С�����ݱ�
// ����: 
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
			//����������Ϊ��
			WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)lpTable->pfat, dwBlockSize );
			//

			for( i = 0; i < lpTable->tfh.wTotalBlock; i++ )
				*(lpTable->pfat+i) = NULL_BLOCK;
			// ����FAT������
			i = WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, dwBlockSize );
			
			if( i )
			{
				UpdateTableHead( lpTable );
				return TRUE;
			}			
			// 
			// ʧ�ܣ����
			free( lpTable->pfat );
			lpTable->pfat = NULL;
		}
		free( lpTable->pRNodeData );
		lpTable->pRNodeData = NULL;
	}
	return FALSE;
}

// **************************************************
// ������static LPTABLE OpenDataTable( 
//                 LPCTSTR lpcszFileName,
//                 DWORD dwAccess,
//                 DWORD dwShareMode, 
//                 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
//                 DWORD dwCreate, 
//                 UINT nFieldNum,
//				   DWORD dwFlagAndAttr	 )
// ������
//		IN lpcszFileName - �ļ��ļ���
//		IN dwAccess - ��ȡģʽ���ο�CreateFile˵����
//		IN dwShareMode - ����ģʽ���ο�CreateFile˵����
//		IN lpSecurityAttributes - ��ȫ���ԣ��ο�CreateFile˵����
//		IN dwCreate - ����/��ģʽ���ο�CreateFile˵����
//		IN nFieldNum - �ֶ���
//		IN dwTableFileSize - �̶��ļ���С�����Ϊ0�����̶�
//		IN dwFlagAndAttr - �ļ���־������
// ����ֵ��
//		��
// ����������
//		�����ݱ�
// ����: 
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
	// ���ļ�
	hFile = CreateFile( lpcszFileName, dwAccess, dwShareMode, NULL, dwCreate, dwFlagAndAttr, NULL );
    if( hFile == INVALID_HANDLE_VALUE )
		goto _return;
	// �����ṹ����ʼ��
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
	{	// �����ݱ�
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
					// ����FAT������
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
				// ʧ�ܣ����
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
	{   // �ļ��Ѿ����ڣ�old file
		DWORD dwSize;
		ReadFile( hFile, &lpTable->tfh, sizeof( lpTable->tfh ), &dwSize, NULL );
		// ����Ƿ���Ч
		if( IsValidTable( &lpTable->tfh ) &&
			(nFieldNum == 0 || lpTable->tfh.nFieldNum == nFieldNum) )
		{	// ��Ч
			BOOL bIsSafe = TRUE;
			if( lpSecurityAttributes )
			{   //��Ҫ����ȫ���
				bIsSafe = CheckPassword( lpTable, lpSecurityAttributes->lpSecurityDescriptor );
			}
			if( bIsSafe )
			{
				lpTable->pRNodeData = malloc( lpTable->tfh.nNodeDataSize );
				if( lpTable->pRNodeData )
				{	// ��FAT���ļ�����
					if( InitFat( lpTable ) == FALSE )
					{	//ʧ��
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
	{	// ���ʧ�ܣ���֮ǰ������������
		if( lpTable )
		{
			DeleteCriticalSection( &lpTable->csTable );
			free( lpTable );
		}
		if( hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle( hFile );
			if( bNewFile )
			{	//�´������ļ�,ɾ����
				DeleteFile( lpcszFileName );
			}
		}
		lpTable = NULL;
	}
	return lpTable;
}

// **************************************************
// ������static BOOL CloseDataTable( LPTABLE lpTable )
// ������
//		IN lpTable - ���ݱ���� 
// ����ֵ��
//		����ɹ�������TRUE; ����ʧ��
// ����������
//		�ر����ݱ�
// ����: 
//		
// ************************************************

static BOOL CloseDataTable( LPTABLE lpTable )
{
	if( lpTable->dwRefCount == 0 )
	{	// û���κ������������øö���
		CloseHandle( lpTable->hFile );
		free( lpTable->pRNodeData );
		free( lpTable->pfat );
		DeleteCriticalSection( &lpTable->csTable );
		free( lpTable );
	}

	return TRUE;
}

// **************************************************
// ������static LPTABLE_FILE FindTableFile( LPCTSTR lpszFileName )
// ������
//		IN lpszFileName - �ļ���
// ����ֵ��
//		����ɹ������ر��ļ�ָ�룻���򣬷���NULL
// ����������
//		�����ļ�������ϵͳ��ǰ�Ƿ����Ѿ��򿪵����ݱ����
// ����: 
//		
// ************************************************

static LPTABLE_FILE FindTableFile( LPCTSTR lpszFileName )
{
	LPTABLE_FILE lpObj;

	lpObj = lpTableList;
	while( lpObj )
	{
		if( stricmp( lpszFileName, lpObj->lpTable->lpszFileName ) == 0 )
		{  // �ҵ���yes, find it
			break;
		}
		lpObj = lpObj->lpNext;
	}

	return lpObj;
}

#define LRF_UPDATE_RECORD  0x1
#define LRF_SET_FLUSH_FLAG  0x2

// **************************************************
// ������static BOOL LockRecord( LPTABLE_FILE lpTableFile, UINT uLockFlag )
// ������
//		IN lpTableFile - ���ݱ��ļ�����
//		IN uLockFlag - ���ܣ���ǰΪ���Ƿ�������ݱ�������� �� ����ˢ�Ա�־
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		��ס��ǰ��¼
// ����: 
//		
// ************************************************

static BOOL LockRecord( LPTABLE_FILE lpTableFile, UINT uLockFlag )
{
	EnterCriticalSection( &lpTableFile->lpTable->csTable );
	if( (lpTableFile->lpTable->dwRN != lpTableFile->rn) &&
		(uLockFlag & LRF_UPDATE_RECORD) )
	{	// ��Ҫ���µ�ǰ��¼
		LPTABLE lpTable = lpTableFile->lpTable;
		DWORD rn = lpTableFile->rn;
        if( rn <= 0 )
            rn = 1;
        else if( rn > lpTable->tfh.nNodeNum )
		{	// ��Ч
			LeaveCriticalSection( &lpTableFile->lpTable->csTable );
			return FALSE;
		}
		// ��ȡ��ǰ��¼��Ϣ
        lpTableFile->rn = _SetRecordPointerAndUpdate( lpTableFile->lpTable, rn );
	}
	if( uLockFlag & LRF_SET_FLUSH_FLAG )
	{
		lpTableFile->lpTable->bFlush = TRUE;
	}
	return TRUE;
}

// **************************************************
// ������static BOOL UnlockRecord( LPTABLE_FILE lpTableFile )
// ������
//		IN lpTableFile - ���ݱ��ļ�����
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		��LockRecord�෴����ס��ǰ��¼
// ����: 
//		
// ************************************************

static BOOL UnlockRecord( LPTABLE_FILE lpTableFile )
{
	LeaveCriticalSection( &lpTableFile->lpTable->csTable );
	return TRUE;
}

// **************************************************
// ������BOOL WINAPI Table_CloseAll( HANDLE hProcess )
// ������
//		IN hProcess - ��ǰ���رյĽ���
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�رս��̴򿪵����ݱ�
// ����: 
//		
// ************************************************

BOOL WINAPI Table_CloseAll( HANDLE hProcess )
{
	LPTABLE_FILE lpNext, lpObj;

	EnterCriticalSection( &csTableList ); // �����ͻ��
	lpObj = lpTableList;
	while( lpObj )
	{
		lpNext = lpObj->lpNext;
		if( lpObj->hOwnerProc == hProcess )
		{	// �ҵ�
#ifdef __DEBUG
		    WARNMSG( 1, (TEXT(" not close table handle: 0x%x\r\n"), lpObj ) );
#endif
			Table_Close( (HTABLE)lpObj );//�ر�
		}
		lpObj = lpNext;
	}
	LeaveCriticalSection( &csTableList ); // �뿪��ͻ��

	return TRUE;
}

// **************************************************
// ������BOOL TableAutoHandle( BOOL bShutDown )
// ������
//		IN bShutDown- �Ƿ�Ӳ���ϵ�
// ����ֵ�� 
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		ϵͳ�رգ�ˢ�����е����ݣ������ shutdown ���ܣ��ر�table �ļ��� ����
// ����: 
//		
// ************************************************

BOOL TableAutoHandle( BOOL bShutDown )
{
	LPTABLE_FILE lpObj;

	EnterCriticalSection( &csTableList ); // �����ͻ��
	lpObj = lpTableList;
	while( lpObj )
	{
		// �ҵ�
		if( bShutDown )
		{
#ifdef __DEBUG
	    WARNMSG( 1, (TEXT(" not close table handle: 0x%x\r\n"), lpObj ) );
#endif

			Table_Close( (HTABLE)lpObj );//�ر�
			lpObj = lpTableList;
		}
		else
		{			
			Table_Flush( (HTABLE)lpObj );	//����ˢ��
			lpObj = lpObj->lpNext;
		}		
	}
	LeaveCriticalSection( &csTableList ); // �뿪��ͻ��

	return TRUE;
}

// **************************************************
// ������BOOL WINAPI Table_PowerHandler( BOOL bShutDown )
// ������
//		IN bShutDown- �Ƿ�Ӳ���ϵ�
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		ϵͳ�رգ�ˢ�����е����ݣ������ shutdown ���ܣ��ر�table �ļ��� ����
// ����: 
//		
// ************************************************

BOOL WINAPI Table_PowerHandler( BOOL bShutDown )
{
	return TableAutoHandle( bShutDown );
}



// **************************************************
// ������BOOL CheckShare( DWORD dwOldShare0, DWORD dwNewShare1, DWORD dwAccess )
// ������
//		IN dwOldShare0 - �Ѵ��ļ�����ģʽ 	
//		IN dwNewShare1 - ��Ҫ���ļ�����ģʽ
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�����������ģʽ�Ƿ��ͻ
// ����: 
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
// ������HTABLE WINAPI Table_Create( 
//                 LPCTSTR lpcszFileName, 
//                 DWORD dwAccess, 
//                 DWORD dwShareMode, 
//                 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
//                 DWORD dwCreate, 
//                 UINT nFieldNum )
// ������
//		IN lpcszFileName - ��Ҫ��/�������ļ���  
//      IN dwAccess - ��ȡģʽ���ο�CreateFile��
//      IN dwShareMode - ����ģʽ���ο�CreateFile��
//      IN lpSecurityAttributes - ��ȫ���ԣ��ο�CreateFile��
//      IN dwCreate - ����/��ģʽ���ο�CreateFile��
//      IN nFieldNum - �ֶ���
// ����ֵ��
//		����ɹ����������ݱ���	
// ����������
//		�������ݱ����
// ����: 
//		ϵͳAPI
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
		
		lpOldTableFile = FindTableFile( lpcszFileName );//���ֵ�ǰϵͳ�Ƿ��Ѿ����ڴ򿪵����ݱ�
		
		if( lpOldTableFile == NULL)
		{  // û�з��֣�������no find, to open 
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
		{  // ϵͳ�Ѿ��򿪡���鹲��ģʽ��share the table ?
			if( CheckShare( lpOldTableFile->dwShareMode, dwShareMode, dwAccess ) )
				lpTable = lpOldTableFile->lpTable; // ���Թ���
		}
		if( lpTable )
		{	// ��ʼ�����ݱ��ļ�����
			lpTableFile->objType = OBJ_TABLE;
			lpTableFile->dwShareMode = dwShareMode;
			lpTableFile->dwAccessMode = dwAccess;
			lpTableFile->lpTable = lpTable;
			// ���������
			_AddToTableList( lpTableFile );
			// ���ò���ȡ��һ����¼
			lpTableFile->rn = _SetRecordPointerAndUpdate( lpTable, 1 );
			if( lpTable->bFlush )
				Table_Flush( (HANDLE)lpTableFile );
		}
		else
		{	// ʧ�ܣ��ͷ�֮ǰ����Ķ���
			free( lpTableFile );
			lpTableFile = NULL;
		}
		LeaveCriticalSection( &csTableList ); // �뿪��ͻ��
 	}

	if( lpTableFile )
	    return lpTableFile;
	else
		return INVALID_HANDLE_VALUE;
}

// **************************************************
// ������BOOL WINAPI Table_Close( HTABLE hTable )
// ������
//		IN hTable - ���ݱ������
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�ر����ݱ�
// ����: 
//		ϵͳAPI
// ************************************************

BOOL WINAPI Table_Close( HTABLE hTable )
{
	BOOL bRetv = TRUE;
    LPTABLE_FILE lpTableFile;
	LPTABLE lpTable;

	EnterCriticalSection( &csTableList );	// �����ͻ��

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTable = lpTableFile->lpTable ) )
    {
        WORD n = GetBlockNum( lpTable, lpTable->tfh.wFatStartBlock );		
		//��Ҫ������ ��
		if( lpTableFile->dwAccessMode & GENERIC_WRITE )
		{
			// 2004-02-01
			// �������ݱ�FAT
			WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, n * lpTable->tfh.nBytesPerBlock );
			//WriteBlocks( lpTable, 0, 0, (BYTE*)&lpTable->tfh  , sizeof( lpTable->tfh ) );
			UpdateTableHead( lpTable );
		}
		// ���ñ�����ϵͳ�����Ƴ�
		_RemoveFromTableList( lpTableFile );
		// �ͷ����ݱ��Ա���ݡ��ر��ļ�
		CloseDataTable( lpTableFile->lpTable );
		// �ͷ����ݱ����
        lpTableFile->objType = OBJ_NULL;
		free( lpTableFile );
    }
	else
		bRetv = FALSE;
	LeaveCriticalSection( &csTableList );	//�뿪��ͻ��
    return bRetv;
}

// **************************************************
// ������static WORD _SetRecordPointerAndUpdate( LPTABLE lpTable, long rn )
// ������
//		IN lpTable - ���ݱ����ָ��
//		IN rn - ��¼����
// ����ֵ��
//		���������õĵ�ǰ��¼��
// ����������
//		�������ݱ�ĵ�ǰ��¼���Ҹ��¼�¼�ڵ�����
// ����: 
//		
// ************************************************

static WORD _SetRecordPointerAndUpdate( LPTABLE lpTable, long rn )
{
	// ����¼��Ϣ
    ReadBlocks( lpTable, 
                lpTable->tfh.wNodeIndexStartBlock, 
                sizeof( RNODEINDEX ) * (rn-1), 
                (BYTE*)&lpTable->RNodeIndex, 
                sizeof( RNODEINDEX ) );
    ASSERT( lpTable->RNodeIndex.wDataStartBlock != NULL_BLOCK );
    // ����¼�ֶ���Ϣ
	ReadBlocks( lpTable, 
                lpTable->RNodeIndex.wDataStartBlock, 
                0, 
                (BYTE*)lpTable->pRNodeData, 
                lpTable->tfh.nNodeDataSize );
    lpTable->dwRN = (WORD)rn;
    return (WORD)rn;
}

// **************************************************
// ������static WORD _SetTableFilePointer( LPTABLE_FILE lpTableFile, long offset, UINT dwMethod )
// ������
//		IN lpTableFile - ���ݱ��ļ�����ָ��
//		IN offset - ��¼ƫ��
//		IN dwMethod - ��ʼλ��,����Ϊ����ֵ֮һ��
//				SRP_BEGIN-�����ݱ�Ŀ�ʼλ�ü���ƫ�� 
//				SRP_CURRENT-�����ݱ�ĵ�ǰλ�ü���ƫ��
//				SRP_END-�����ݱ�Ľ���λ�ü���ƫ��
// ����ֵ��
//		����ɹ������ص�ǰ��¼��; ���򣬷���0xffff
// ����������
//		�������ݱ�ĵ�ǰ��¼
// ����: 
//		
// ************************************************

static WORD _SetTableFilePointer( LPTABLE_FILE lpTableFile, long offset, UINT dwMethod )
{
	LPTABLE lpTable;
    WORD retv = 0xffff;
    long rn;

	if( (lpTable = lpTableFile->lpTable) )
	{
		if( lpTable->tfh.nNodeNum )	// ���ݱ��м�¼�� ��
		{    // �У�has records
			if( dwMethod == SRP_BEGIN )
				rn = 1 + offset;	// �ӿ�ʼ�������λ��
			else if( dwMethod == SRP_CURRENT )
				rn = lpTable->dwRN + offset;	// �ӵ�ǰλ�ü������λ��
			else  // SRP_END
				rn = (long)lpTable->tfh.nNodeNum + offset;	// �ӽ���λ�ü������λ��
			
			if( rn <= 0 )	// 
				rn = 1;		// ����Ϊ��һ����¼
			else if( rn > lpTable->tfh.nNodeNum )
			{	// ��Чλ��
				return 0xffff;
			}
			
			lpTableFile->rn = rn;	//
			retv = (WORD)rn;
		}
		else
		{   // no record
			lpTableFile->rn = 0;	//���ݱ��޼�¼
		}
	}
    return retv;
}

// **************************************************
// ������WORD WINAPI Table_SetRecordPointer( HTABLE hTable, long offset, UINT dwMethod )
// ������
//		IN hTable - ���ݱ������
//		IN offset - ��¼ƫ��
//		IN dwMethod - ��ʼλ��,����Ϊ����ֵ֮һ��
//				SRP_BEGIN-�����ݱ�Ŀ�ʼλ�ü���ƫ�� 
//				SRP_CURRENT-�����ݱ�ĵ�ǰλ�ü���ƫ��
//				SRP_END-�����ݱ�Ľ���λ�ü���ƫ��
// ����ֵ��
//		����ɹ������ص�ǰ��¼��; ���򣬷���0xffff
// ����������
//		�������ݱ�ĵ�ǰ��¼
// ����: 
//		ϵͳAPI
// ************************************************

WORD WINAPI Table_SetRecordPointer( HTABLE hTable, long offset, UINT dwMethod )
{
    WORD retv = 0xffff;
    //long rn;

    LPTABLE_FILE lpTableFile;
	LPTABLE lpTable;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTable = lpTableFile->lpTable ) )	//���ݱ���Ч��
    {
		LockRecord( lpTableFile, 0 ); // ��ס��ǰ��¼
		retv = _SetTableFilePointer( lpTableFile, offset, dwMethod );
        /*
		if( lpTable->tfh.nNodeNum )	// ���ݱ��м�¼�� ��
        {    // �У�has records
            if( dwMethod == SRP_BEGIN )
                rn = 1 + offset;	// �ӿ�ʼ�������λ��
            else if( dwMethod == SRP_CURRENT )
                rn = lpTable->dwRN + offset;	// �ӵ�ǰλ�ü������λ��
            else  // SRP_END
                rn = (long)lpTable->tfh.nNodeNum + offset;	// �ӽ���λ�ü������λ��

            if( rn <= 0 )	// 
                rn = 1;		// ����Ϊ��һ����¼
            else if( rn > lpTable->tfh.nNodeNum )
			{	// ��Чλ�ã�����������
				UnlockRecord( lpTableFile );
				return 0xffff;
			}

            lpTableFile->rn = rn;	//
            retv = (WORD)rn;
        }
        else
        {   // no record
            lpTableFile->rn = 0;	//���ݱ��޼�¼
        }
		*/
		UnlockRecord( lpTableFile );	//����
    }
    return retv;
}

// **************************************************
// ������WORD WINAPI Table_NextRecordPointer( HTABLE hTable, BOOL bNext )
// ������
//		IN hTable - ���ݱ������
//		IN bNext - ����ΪTRUE, ���õ�ǰ��¼����һ��; ����ǰһ��
// ����ֵ��
//		����ɹ������ص�ǰ��¼��; ���򣬷���0xffff
// ����������
//		������/��һ����¼�����ݱ�ĵ�ǰ��¼
// ����: 
//		ϵͳAPI
// ************************************************

WORD WINAPI Table_NextRecordPointer( HTABLE hTable, BOOL bNext )
{
    WORD retv = 0xffff;
	LPTABLE lpTable;

    LPTABLE_FILE lpTableFile;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTable = lpTableFile->lpTable ) )	//���ݱ���Ч��
    {
		LockRecord( lpTableFile, 0 ); // ��ס��ǰ��¼

        if( bNext )
            retv = _SetTableFilePointer( lpTableFile, 1, SRP_CURRENT );// ��һ��
        else
            retv = _SetTableFilePointer( lpTableFile, -1, SRP_CURRENT );//��һ��

		UnlockRecord( lpTableFile );	//����

//        if( bNext )
//            retv = Table_SetRecordPointer( hTable, 1, SRP_CURRENT );	// ��һ��
//        else
//            retv = Table_SetRecordPointer( hTable, -1, SRP_CURRENT );	//��һ��
    }
    return retv;
}

// **************************************************
// ������static WORD _ReadField( LPTABLE lpTable, UINT field, UINT startPos, void * lpBuf, UINT sizeLimit )
// ������
//		IN lpTable - ���ݱ����ָ��
//		IN field - �ֶ�
//		IN startPos - ���ֶ��ڵĿ�ʼλ��
//		IN lpBuf - ���ڴ�Ŷ�ȡ���ݵĻ���
//		IN sizeLimit - lpBuf�ĳߴ��С
// ����ֵ��
//		����ʵ�ʶ����ֽ�����
// ����������
//		��ȡ�ֶ��ڵ�����
// ����: 
//		
// ************************************************
static WORD _ReadField( LPTABLE lpTable, UINT field, UINT startPos, void * lpBuf, UINT sizeLimit )
{
	PRNODEDATA pRNode;
	DWORD size;
	WORD retv = 0;

	pRNode = lpTable->pRNodeData;
	if( startPos < pRNode->nFieldSize[field] )  // ����ƫ����Ч��
	{
		UINT i;
		for( i = 0, size = 0; i < field; i++ )
			size += pRNode->nFieldSize[i];
				
		// read data
		if( sizeLimit > pRNode->nFieldSize[field] - startPos )
			sizeLimit = pRNode->nFieldSize[field] - startPos;
		// sizeLimit Ϊ��ʵ��Ҫ��������
		// ������
		retv = (WORD)ReadBlocks( lpTable, 
			lpTable->RNodeIndex.wDataStartBlock, 
			size + startPos + lpTable->tfh.nNodeDataSize, 
			lpBuf, 
			sizeLimit );

		//if( lpTable->bIsEncrypt && retv )
		//{	//���ݱ�����,��Ҫ����
		//	DecrypData( lpBuf, retv );
		//}
	}
	return retv;
}

// **************************************************
// ������WORD WINAPI Table_ReadField( HTABLE hTable, UINT field, WORD startPos, void * lpBuf, WORD sizeLimit )
// ������
//		IN hTable - ���ݱ������
//		IN field - �ֶ�
//		IN startPos - ���ֶ��ڵĿ�ʼλ��
//		IN lpBuf - ���ڴ�Ŷ�ȡ���ݵĻ���
//		IN sizeLimit - lpBuf�ĳߴ��С
// ����ֵ��
//		����ʵ�ʶ����ֽ�����
// ����������
//		��ȡ�ֶ��ڵ�����
// ����: 
//		ϵͳAPI
// ************************************************

WORD WINAPI Table_ReadField( HTABLE hTable, UINT field, WORD startPos, void * lpBuf, WORD sizeLimit )
{
    LPTABLE_FILE lpTableFile;
    LPTABLE lpTable;
    //PRNODEDATA pRNode;
//    UINT i;
    WORD retv = 0;
    //DWORD size;

	field--;	// �ֶ������Ǵ�1��ʼ�����Լ�һ

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTable = lpTableFile->lpTable ) )	//���ݱ���Ч�� 
	{	// 
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD ) )	// ��ס��ǰ��¼
		{	
			if( field < lpTable->tfh.nFieldNum && 
				lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )	// ������Ч�� ��
			{   // get pos of the field
				/*
				pRNode = lpTable->pRNodeData;
				if( startPos < pRNode->nFieldSize[field] )  // ����ƫ����Ч��
				{
					for( i = 0, size = 0; i < field; i++ )
						size += pRNode->nFieldSize[i];
					
					// read data
					if( sizeLimit > pRNode->nFieldSize[field] - startPos )
						sizeLimit = pRNode->nFieldSize[field] - startPos;
					// sizeLimit Ϊ��ʵ��Ҫ��������
					// ������
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
			UnlockRecord( lpTableFile );	//����
		}
	}

    return retv;
}

// **************************************************
// ������WORD WINAPI Table_WriteField( HTABLE hTable, UINT field, const void * lpcvBuf, WORD size )
// ������
//		IN hTable - ���ݱ������
//		IN field - �ֶ�
//		IN lpBuf - ����д�����ݵĻ���
//		IN sizeLimit - lpBuf�ĳߴ��С
// ����ֵ��
//		����ʵ��д���ֽ�����
// ����������
//		д�ֶε�����
// ����: 
//		ϵͳAPI
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
		( lpTable = lpTableFile->lpTable ) )	// ����Ƿ���Ч ��
    {   // get pre field size
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD|LRF_SET_FLUSH_FLAG ) )	// ��ס��¼
		{
			if( field < lpTable->tfh.nFieldNum && 
				lpTable->dwRN &&
				lpTable->dwRN <= lpTable->tfh.nNodeNum )	// �ֶ���Ч�� ��
			{
				pRNode = lpTable->pRNodeData;
				for( i = 0, n = 0; i < field; i++ )
					n += (long)pRNode->nFieldSize[i];
				
				pos = n;   // �������ڼ�¼��д��λ�á�pos of new data to write
				// ������ֶ�������Ҫ�ƶ���λ�á�sum the field size and get pos to move data
				posMoveFrom = oldSize = n + pRNode->nFieldSize[i];
				posMoveTo = newSize = n + size;
				
				//	ͳ�Ƹ��ֶκ�������ֶε��ֽ����� sum other field size
				i++;
				for( n = 0; i < lpTable->tfh.nFieldNum; i++ )
					n += pRNode->nFieldSize[i];
				
				oldSize += n;   // �ϵļ�¼���� old record size
				newSize += n;   // �µļ�¼���� new record size
				
				if( newSize > oldSize )	// �µļ�¼������ ��
				{   // ������Ҫ���·����¼�ռ� alloc enough block
					if( ReallocFile( lpTable, lpTable->RNodeIndex.wDataStartBlock, newSize + lpTable->tfh.nNodeDataSize, &newBlock ) == FALSE )
					{
						goto _UNLOCK;    // not enough block
					}
					ASSERT( newBlock == lpTable->RNodeIndex.wDataStartBlock );
				}
				if( newSize != oldSize )  // ����¼�¼��С���ϼ�¼��С����ȣ����ƶ����� move data if possible
					MoveData( lpTable, lpTable->RNodeIndex.wDataStartBlock, posMoveFrom + lpTable->tfh.nNodeDataSize, posMoveTo + lpTable->tfh.nNodeDataSize );
				// д��������
				WriteBlocks( lpTable, lpTable->RNodeIndex.wDataStartBlock, pos + lpTable->tfh.nNodeDataSize, lpcvBuf, size );
				// ���¼�¼��Ϣ���� now, update record info
				pRNode->nFieldSize[field] = size;
				UpdateNodeData( lpTable );
				retv = size;
			}
_UNLOCK:
			UnlockRecord( lpTableFile );	// ����
		}
    }
	else if( lpTable && field >= lpTable->tfh.nFieldNum )
	{	//��������
		SetLastError( ERROR_INVALID_PARAMETER );
		WARNMSG(TABLE_ERROR, (TEXT("WriteField: field(%d) >= FieldNum(%d) \r\n"), field, lpTable->tfh.nFieldNum ));		
	}

    return retv;
}

// **************************************************
// ������BOOL WINAPI Table_ReadRecord( 
//								HTABLE hTable, 
//								void * lpvData,
//								FIELDITEM fieldItem[],
//								DWORD * lpdwSize )
// ������
//		IN hTable - ���ݱ������
//		OUT lpvData-���ڱ������ݵ��ڴ�
//		OUT fieldItem-FIELDITEM�ṹָ�뵱�ɹ�ʱ�����ظ����ֶ���lpvData����ʼ��ַ�͸��ֶ����ݵĴ�С
//		OUT lpdwSize-���ɹ�ʱ�����������ֶ����ݵĴ�С;����lpdwSizeΪNULL,�򲻷��ش�С
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	����ǰ��¼������ 
// ����: 
//	ϵͳAPI	
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
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD ) )	// ��ס��¼
		{
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )	// ��ǰ��¼��Ч ��
			{	// ��Ч
				pRNode = lpTable->pRNodeData;
				size = 0;
				//	ͳ�Ƽ�¼��ÿ���ֶ��ڴ��С�ͼ�¼�ܵĴ�С
				for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
				{
					fieldItem[i].size = pRNode->nFieldSize[i];
					fieldItem[i].lpData = ((BYTE*)lpvData) + size;
					size += pRNode->nFieldSize[i];
				}
				//	����¼
				if( size == ReadBlocks( lpTable, lpTable->RNodeIndex.wDataStartBlock, lpTable->tfh.nNodeDataSize, lpvData, size ) )
				{	//�ɹ�
					if( lpdwSize )
						*lpdwSize = size;
					retv = TRUE;
					// ���õ�ǰ��¼����һ����go to next
					//Table_SetRecordPointer( hTable, 1, SRP_CURRENT );
					_SetTableFilePointer( lpTableFile, 1, SRP_CURRENT );
					//if( lpTable->bIsEncrypt )
					//{	//���ݱ�����,��Ҫ����
					//	DecrypData( lpvData, size );
					//}
				}
				else
				{
					ASSERT( 0 );
				}
			}
			UnlockRecord( lpTableFile );	// ����
		}
    }
    return retv;
}

// **************************************************
// ������BOOL WINAPI Table_DeleteRecord( HTABLE hTable, UINT uiRecord, DWORD dwFlag )
// ������
//		IN hTable - ���ݱ��ļ�������
//		IN uiRecord - ��¼��
//		IN dwFlag - ���ӹ���
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		ɾ����¼
// ����: 
//		ϵͳAPI
// ************************************************

BOOL WINAPI Table_DeleteRecord( HTABLE hTable, UINT uiRecord, DWORD dwFlag )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    BOOL retv = FALSE;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
		( lpTable = lpTableFile->lpTable ) )  // �õ����ļ�ʱ��
	{
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD|LRF_SET_FLUSH_FLAG ) )	// ��ס��¼
		{
			if( uiRecord != DR_CURRENT )	//	�Ƿ�ɾ����ǰ��¼
			{	// ����
				if( uiRecord <= lpTable->tfh.nNodeNum && uiRecord > 0 )
				{	// ���ñ��ļ���ǰ��¼��
					_SetTableFilePointer( lpTableFile, uiRecord-1, SRP_BEGIN );//Table_SetRecordPointer( hTable, uiRecord-1, SRP_BEGIN );
					// ���¼�¼�ڵ�����
					_SetRecordPointerAndUpdate( lpTableFile->lpTable, lpTableFile->rn );
				}
				else
					goto _UNLOCK;	//��Ч�ļ�¼��
				//return FALSE;
			}
			
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )
			{    // lock record before deleted
				
				if( dwFlag == DR_REMOVE )	// �Ƿ񽫸ü�¼�����ͷ�
				{
					lpTable->tfh.nNodeNum--;
					//�ͷ����ݿ�
					FreeBlock( lpTable, lpTable->RNodeIndex.wDataStartBlock );
					MoveData(lpTable, 
						lpTable->tfh.wNodeIndexStartBlock, 
						lpTable->dwRN*sizeof( RNODEINDEX ),
						(lpTable->dwRN-1)*sizeof( RNODEINDEX ) );            
					lpTable->dwRN = 0;	//��ǰ�ڵ��¼��Ϊ��Ч

					// set new record pointer
					if( lpTable->dwRN > lpTable->tfh.nNodeNum )  // ���һ����¼
						_SetTableFilePointer( lpTableFile, 0, SRP_END );//Table_SetRecordPointer( hTable, 0, SRP_END );
					else	//
						_SetTableFilePointer( lpTableFile,lpTable->dwRN-1, SRP_BEGIN );//Table_SetRecordPointer( hTable, lpTable->dwRN-1, SRP_BEGIN );
					retv = TRUE;
				}
				else if( dwFlag == DR_NOREMOVE )	
				{	// �߼�ɾ��, ������ɾ����־
					//lpTable->pRNodeData->wFlag = RF_LOGIC_DELETE;
					//retv = UpdateNodeData( lpTable );
					lpTable->RNodeIndex.bAttrib |= RA_LOGIC_DELETE;
					retv = UpdateNodeIndex( lpTable );
				}
			}
_UNLOCK:
		UnlockRecord( lpTableFile );	// ����
		}
	}
//RET_DELETE:
    return retv;
}

// **************************************************
// ������BOOL WINAPI Table_Delete( LPCTSTR lpcszFileName )
// ������
//		IN lpcszFileName - �ļ���
// ����ֵ��
//		�ɹ������ط���
//		����0
// ����������
//		ɾ�����ݱ��ļ�
// ����: 
//		ϵͳAPI
// ************************************************

BOOL WINAPI Table_Delete( LPCTSTR lpcszFileName )
{
    return DeleteFile( lpcszFileName );
}

/*
// **************************************************
// ������static FIELDITEM * GetRemapFieldItem(  FIELDITEM fieldItem[], int nFieldNum )
// ������
//		IN fieldItem - �ֶ���Ϣ����
//		IN nFieldNum - �ֶ���
// ����ֵ��
//		����ɹ��������ֶ�ָ�룻���򣬷���NULL
// ����������
//		ӳ���¼ָ��
// ����: 
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
// ������
// ������
//		
// ����ֵ��
//		
// ����������
//		
// ����: 
//		
// ************************************************

static void ReleaseRemapFieldItem( FIELDITEM * lpItems )
{
	free( lpItems );
}
*/

// **************************************************
// ������long WINAPI Table_WriteRecord( HTABLE hTable, FIELDITEM fieldItem[] )
// ������
//		IN hTable - ���ݱ��ļ�����
//		IN fieldItem - FIELDITEM�ṹָ�룬���������ֶε�������������
// ����ֵ��
//		�ɹ�������ʵ��д������
//		���򣺷���0
// ����������
//		д���ݵ���ǰ��¼ 
// ����: 
//		ϵͳAPI
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
		( lpTable = lpTableFile->lpTable ) )	// ��������ݱ��ļ�ָ��
    {
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD|LRF_SET_FLUSH_FLAG ) )	// ��ס��¼
		{			
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )	// ��Ч��¼��
			{
				FIELDITEM * lpItems = fieldItem; //GetRemapFieldItem( fieldItem, lpTable->tfh.nFieldNum );//malloc( sizeof( FIELDITEM ) * lpTable->tfh.nFieldNum );
				if( lpItems )
				{
					bFrag = FALSE;
					lpData = lpItems[0].lpData;
					// ��������Ƿ�����
					for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
					{
						newSize += lpItems[i].size;
						if( lpData == lpItems[i].lpData )
							lpData += lpItems[i].size;
						else
							bFrag = TRUE;  // �������� is fragmental
					}
					// Ϊ�ü�¼�����ļ��ռ�
					if( ReallocFile( lpTable,
						lpTable->RNodeIndex.wDataStartBlock,
						newSize + lpTable->tfh.nNodeDataSize,
						&lpTable->RNodeIndex.wDataStartBlock ) )
					{	// �����û����ݵ���¼
						_CopyRecordData( lpTable, lpItems, newSize, bFrag, lpTableFile->hOwnerProc );
					}
					else
						newSize = 0;					
					//ReleaseRemapFieldItem( lpItems );// free( lpItems );
				}
			}
			UnlockRecord( lpTableFile );  // ����
		}
    }
    return newSize;
}

// **************************************************
// ������BOOL WINAPI Table_InsertRecord( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] )
// ������
//	IN hTable-���ݱ��ļ�����
//	IN nPos-����λ��
//	IN fieldItem-FIELDITEM �ṹָ�룬���������ֶε�������������
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����¼��ָ��λ��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Table_InsertRecord( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;
    BOOL retv = FALSE;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
		( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
		( lpTable = lpTableFile->lpTable ) &&
		nPos )	// �õ����ݱ����ָ��
    {
		if( LockRecord( lpTableFile, LRF_SET_FLUSH_FLAG ) )	//	��ס��¼
		{
			FIELDITEM * lpItems = fieldItem;//GetRemapFieldItem( fieldItem, lpTable->tfh.nFieldNum );//malloc( sizeof( FIELDITEM ) * lpTable->tfh.nFieldNum );
			
			if( lpItems )
			{	// ����ָ��λ��
				if( lpTable->tfh.nNodeNum == 0 )  // like append
					retv = _InsertRecord( lpTable, 1, lpItems, lpTableFile->hOwnerProc );
				else if( nPos <= lpTable->tfh.nNodeNum  )
					retv = _InsertRecord( lpTable, nPos, lpItems, lpTableFile->hOwnerProc );
				else   // insert to end
					retv = _InsertRecord( lpTable, (WORD)(lpTable->tfh.nNodeNum + 1), lpItems, lpTableFile->hOwnerProc );
				//ReleaseRemapFieldItem( lpItems );	
			}
			UnlockRecord( lpTableFile );	// ����
		}
	}
    return retv;
}

// **************************************************
// ������BOOL WINAPI Table_AppendRecord( HTABLE hTable, FIELDITEM fieldItem[] )
// ������
//	IN hTable-���ݱ��ļ����
//	IN fieldItem-FIELDITEM�ṹָ�룬���������ֶε�������������
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	��Ӽ�¼�����ݱ�ĩβλ��
// ����: 
//	ϵͳAPI
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
		if( LockRecord( lpTableFile, LRF_SET_FLUSH_FLAG ) ) // ��ס�ĵ�ǰ��¼
		{
			FIELDITEM * lpItems = fieldItem;			

			if( lpItems )
			{	// �����¼��β
			    DEBUGMSG( DEBUG_Table_AppendRecord, ( "Table_AppendRecord _InsertRecord.\r\n" ) );
				retv = _InsertRecord( lpTable, (WORD)(lpTable->tfh.nNodeNum + 1), lpItems, lpTableFile->hOwnerProc );
			}
			UnlockRecord( lpTableFile ); // ����
		}
	}
    DEBUGMSG( DEBUG_Table_AppendRecord, ( "Table_AppendRecord leave.\r\n" ) );	
    return retv;
}

// **************************************************
// ������int WINAPI Table_CountRecord( HTABLE hTable )
// ������
//	IN hTable-���ݱ��ļ����
// ����ֵ��
//	����ɹ������ص�ǰ��¼�������򣬷���0	
// ����������
//	ͳ�Ƽ�¼��	
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI Table_SortRecord( HTABLE hTable, WORD field, FIELDCOMPPROC lpFieldProc )
// ������
//	IN hTable - ���ݱ��ļ�����
//	IN field - ��Ҫ������ֶ�������(����1)
//	IN lpFieldProc - FIELDCOMPPROC���ͣ�Ӧ���ṩ���ֶαȽϻص�����
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ݱ�ļ�¼���ֶ�filedΪ��׼�������� 	
// ����: 
//	ϵͳAPI
// ************************************************
/*
// ��������㷨
  
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
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD|LRF_SET_FLUSH_FLAG ) && lpTable->dwRN )  // ��ס��¼
		{
			// Ϊ�ص�����׼������
			cd.hProcess = lpTableFile->hOwnerProc;
			cd.lpfn = (FARPROC)lpFieldProc;
			cd.dwArg0 = (DWORD)&cs;

			oldRecordNo = lpTable->dwRN;
			count = lpTable->tfh.nNodeNum; // �ܵļ�¼��
			//�������
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
						_Exchange( lpTable, (WORD)i, (WORD)j );	// ������¼��
				}
			}
			// �ָ��ϵĵ�ǰ��¼��
			_SetTableFilePointer( lpTableFile, oldRecordNo, SRP_BEGIN );//Table_SetRecordPointer( hTable, oldRecordNo, SRP_BEGIN );
			UnlockRecord( lpTableFile );	// ����
			return TRUE;
		}
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI Table_GetRecordInfo( HTABLE hTable, LPRECORDINFO lpri )
// ������
//	IN hTable �C ���ݱ��ļ����
//	IN lpri - RECORDINFO�ṹָ�룬���ڽ�����Ϣ
// ����ֵ��
//	����ɹ�������TRUE��lpri�����е�ǰ��¼��Ϣ������ʧ�ܣ�����FALSE
// ����������
//	�õ���ǰ��¼��Ϣ
// ����: 
//	ϵͳAPI
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
		( lpTable = lpTableFile->lpTable ) )	// �����ݱ��ļ�����õ�����ָ��
    {
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD ) )	// ��ס��ǰ��¼
		{
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )
			{
				if( lpri->uiMask & RIM_DELETED )	// ��Ҫɾ����Ϣ�� ��
				{	// ��Ҫ
					lpri->bIsDeleted = ( (lpTable->RNodeIndex.bAttrib & RA_LOGIC_DELETE) != 0 );
				}
				if( (lpri->uiMask & RIM_RECORDSIZE) ||
					(lpri->uiMask & RIM_FIELDSIZE) ) // ��Ҫ��¼��С��ÿ���ֶε���Ϣ�� ��
				{	//��Ҫ
					pRNode = lpTable->pRNodeData;
					for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
					{
						if( lpri->uiMask & RIM_FIELDSIZE ) // 
							lpri->uiFieldSize[i] = pRNode->nFieldSize[i];//�ֶ���Ϣ
						size += pRNode->nFieldSize[i];
					}
					if(lpri->uiMask & RIM_RECORDSIZE)
						lpri->uiRecordSize = size; //��¼��Ϣ
				}
				bRetv = TRUE;
			}
			UnlockRecord( lpTableFile ); // ����
		}
    }
    return bRetv;
}

// **************************************************
// ������DWORD WINAPI Table_GetRecordSize( HTABLE hTable )
// ������
//		IN hTable �C ���ݱ��ļ����
// ����ֵ��
//	�ɹ������سߴ�
//	���򣺷���0
// ����������
//	�õ���ǰ��¼�ĳߴ�	
// ����: 
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
		( lpTable = lpTableFile->lpTable ) )	// �����ݱ��ļ�����õ����ݱ��ļ�����ָ��
    {        
		if( LockRecord( lpTableFile, LRF_UPDATE_RECORD ) )// ��ס��ǰ��¼
		{
			if( lpTable->dwRN && 
				lpTable->dwRN <= lpTable->tfh.nNodeNum )
			{
				pRNode = lpTable->pRNodeData;
				for( i = 0; i < lpTable->tfh.nFieldNum; i++ )
					size += pRNode->nFieldSize[i];
			}
			UnlockRecord( lpTableFile );// ����
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
// ������static char * _qstrstr(const char *s1, int len1, const char *s2, int len2 )
// ������
//		IN s1 - �ַ���1
//		IN len1 - �ַ���1�ĳ���
//		IN s2 - �ַ���2
//		IN len2 - �ַ���2�ĳ���
// ����ֵ��
//		�����ҵ���������s1�о���s2�ĵ�ַ�����򣬷���NULL
// ����������
//		���ַ���s1�в����Ƿ����ַ���s2��
// ����: 
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
        // ���ȵõ��׸��ַ���ȵ�λ��.find first match char c
        c = *p2++;
        do
        {
            if( c == *p1++ )
                break;
			if( p1 == p1end )
				return 0;
        }while( 1 );

        ps = p1-1;  // �����һ��λ�á�save first address
        // other chars match ?
		// �����ַ���ͬ�� ��
        for( i = 1; i < len2; i++, p1++, p2++ )
        {
			if( p1 == p1end )
                return 0; // �Ƚ��꣬��ͬ
			if( p2 == p2end ) // ��αȽ��� ��
            {	// �ǣ��Ƚ�ǰ��
                p2 = s2;  // reset p2 to start address
                p1 -= len2;  //
                ps = p1;  // save p1 address
            }

            if( *p1 != *p2 ) // ������ ��
                break;	// �ǣ��˳���ǰѭ��
        }
        if( i == len2 )   // �����ַ����Ƚ��� ��all char is match
            return (char*)ps; // ��
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
// ������static UINT _IsMatch( PTABLEFIND lpfs )
// ������
//		IN lpfs - TABLEINFO�ṹָ��
// ����ֵ��
//		����õ�ƥ��ļ�¼�����ؼ�¼�ã����򣬷���0
// ����������
//		�����ݱ��в��Ҿ���ĳ�ַ����ļ�¼����Ҫ�Ƚϵ����ݲ��ܳɹ�127��
// ����: 
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
	// �Ƚ��ֶ�
    for( field = lpfs->uFieldFrom; field <= lpfs->uFieldTo; field++ )
    {   // search every fileld
        pos = 0;
        while( 1 )
        {	// ���ֶ�����
            s = Table_ReadField( lpfs->hTable, field, (WORD)pos, lpvBuf, 256 );
            if( s < (long)lpfs->uLen )
                break;
			// �Ƚ�
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
// ������static WORD GetMax( WORD * pfat, WORD wStart, WORD * pPrev )
// ������
//		IN pfat - �����п������Ϣ����ָ��
//		IN wStart - �������׿�����
//		OUT pPrev - ���ڽ��տ�����ָ�����������Ŀ����� 
// ����ֵ��
//		���ؿ����е���������
// ����������
//		�ڿ����еõ����Ŀ�������
// ����: 
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
// ������static WORD GetMin( WORD * pfat, WORD wStart, WORD * pPrev )
// ������
//		IN pfat - �����п������Ϣ����ָ��
//		IN wStart - �������׿�����
//		OUT pPrev - ���ڽ��տ�����ָ����С�������Ŀ����� 
// ����ֵ��
//		���ؿ����е���С������
// ����������
//		�ڿ����еõ���С�Ŀ�������
// ����: 
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
// ������static UINT _RemoveLogicRecord( LPTABLE lpTable, PRNODEINDEX pNodeIndex )
// ������
//		IN lpTable - ���ݱ�ָ��
//		IN pNodeIndex - ��¼�ڵ���������
// ����ֵ��
//		ɾ���ļ�¼��	
// ����������
//		����ɾ��֮ǰ�ѱ��߼�ɾ���ļ�¼
// ����: 
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
		if( pIndex->bAttrib & RA_LOGIC_DELETE )  // �Ƿ��Ա��߼�ɾ�� ��
		{	//��
			iDelCount++;
			FreeBlock( lpTable, pIndex->wDataStartBlock );  // �ͷż�¼������
		}
	}

	if( iDelCount )
	{	// ���¼�¼�ڵ���������
		pIndexHead = pIndex = pNodeIndex;
		for( i = 0; i < uiNum ; i++, pIndex++ )
		{
			if( (pIndex->bAttrib & RA_LOGIC_DELETE) == 0 )
			{
			    *pIndexHead++ = *pIndex;
			}
		}
		lpTable->tfh.nNodeNum -= iDelCount;
		// �����µ�����д�����
		WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)pNodeIndex,  lpTable->tfh.nNodeNum * sizeof( RNODEINDEX ) );
	}
	return iDelCount;
}

// **************************************************
// ������BOOL WINAPI Table_Pack( LPCTSTR lpcszFileName, UINT uiFlag )
// ������
//		IN lpcszFileName - ���ݱ��ļ���
//		IN uiFlag - ѹ����־��������
//							PT_NOREMOVE - ������ɾ��֮ǰ�Ѿ����߼�ɾ���ļ�¼
//							PT_REMOVE - ����ɾ��֮ǰ�Ѿ����߼�ɾ���ļ�¼
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		ѹ�����ݱ�������ܣ��Ƚ�֮ǰ�߼�ɾ���ļ�¼����ɾ����Ȼ�����е����ݾ������Ƶ��ļ���
//		ǰ�棬�������ļ���С
// ����: 
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
    if( ( lpTable = lpTableFile->lpTable ) )	//// �����ݱ��ļ�����õ����ݱ��ļ�����ָ��
    {
		if( LockRecord( lpTableFile, LRF_SET_FLUSH_FLAG ) )	// ��ס���ݱ�
		{
			DWORD dwSize;
			PRNODEINDEX pIndex, pRNodeIndex;

			_SetTableFilePointer( lpTableFile, 0, SRP_BEGIN );

			dwSize = lpTable->tfh.nNodeNum * sizeof( RNODEINDEX );
			pRNodeIndex = malloc( dwSize + lpTable->tfh.nBytesPerBlock );
			
			if( pRNodeIndex )
			{   /// get a min block of free
				// �������ݱ�ļ�¼������
				ReadBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)pRNodeIndex, dwSize );
				if( uiFlag & PT_REMOVE )
				{	// ����ɾ�����е��߼�ɾ���ļ�¼
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
						{	// �õ���������С�Ŀ�������
							wMinFree = GetMin( lpTable->pfat, lpTable->tfh.wFreeBlock, &wMinPrev );
							
							//�õ������������Ŀ�������

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
							if( wMinFree > wMaxAlloc ) // �����ݿ����п��п��� ��
							{  //  û�У����е����ݿ鶼���ļ���ǰ�� all free is bottom
								WORD nNewFatBlock;
								WORD nOldFatBlock;
								// ��С�ļ���С
								SetFilePointer(
											lpTable->hFile, 
											-(lpTable->tfh.nFreeCount * lpTable->tfh.nBytesPerBlock), 
											NULL,
											FILE_END );
								SetEndOfFile( lpTable->hFile );
								// �����������ݱ��ļ�ϵͳ��Ϣ
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
							{   // // �����ݿ����п��п飬��Ҫ�������ݡ�exchange data ....
								WORD wMaxNext;
								// ���������ݿ�����д����п飬�����ÿ��п���Ϊ���ݿ鲢�ͷ�Դ���ݿ�
								ReadBlocks( lpTable, wMaxAlloc, 0, pBlock, lpTable->tfh.nBytesPerBlock );
								WriteBlocks( lpTable, wMinFree, 0, pBlock, lpTable->tfh.nBytesPerBlock );								
								// exchange fat
								wMaxNext = NEXT_BLOCK( lpTable->pfat, wMaxAlloc );
								// �ı�FAT����
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
								//�ı�FAT���ݺ������ڵ�����
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
						//ѹ���꣬���¼�¼����������
						WriteBlocks( lpTable, lpTable->tfh.wNodeIndexStartBlock, 0, (BYTE*)pRNodeIndex, dwSize );
					}
					else
					{  // �ñ�û���κ����ݡ�ֻ��Ҫ������ͷ��������no data, free all, noly has header block and fat block and index block
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

			UnlockRecord( lpTableFile ); // ����
		}
        //Table_Close( hTable );	//�رձ�
    }
    return TRUE;
}

// **************************************************
// ������BOOL WINAPI Table_Pack( LPCTSTR lpcszFileName, UINT uiFlag )
// ������
//		IN lpcszFileName - ���ݱ��ļ���
//		IN uiFlag - ѹ����־��������
//							PT_NOREMOVE - ������ɾ��֮ǰ�Ѿ����߼�ɾ���ļ�¼
//							PT_REMOVE - ����ɾ��֮ǰ�Ѿ����߼�ɾ���ļ�¼
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		ѹ�����ݱ�������ܣ��Ƚ�֮ǰ�߼�ɾ���ļ�¼����ɾ����Ȼ�����е����ݾ������Ƶ��ļ���
//		ǰ�棬�������ļ���С
// ����: 
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
// ������BOOL WINAPI Table_PackByHandle( HANDLE hTable, UINT uiFlag )
// ������
//		IN hTable - ���ݱ��ļ����
//		IN uiFlag - ѹ����־��������
//							PT_NOREMOVE - ������ɾ��֮ǰ�Ѿ����߼�ɾ���ļ�¼
//							PT_REMOVE - ����ɾ��֮ǰ�Ѿ����߼�ɾ���ļ�¼
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		ѹ�����ݱ�������ܣ��Ƚ�֮ǰ�߼�ɾ���ļ�¼����ɾ����Ȼ�����е����ݾ������Ƶ��ļ���
//		ǰ�棬�������ļ���С
// ����: 
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
// ������HANDLE WINAPI Table_FindFirst( 
//							HTABLE hTable, 
//							UINT uField, 
//							const void * lpcvFind, 
//							UINT uLen, 
//							LPFINDSTRUCT lpfs )
// ������
//	IN hTable - ���ݱ��ļ����� 
//	IN uField - �ֶ�����(����1)�����Ϊ-1������������ֶ�
//	IN lpcvFind - ��Ҫ���ҵ�����
//	IN uLen - ���ҵ����ݳ���
//	OUT lpfs - FINDSTRUCT�ṹָ�룬���ڽ��ܲ��ҵ��ļ�¼��Ϣ
// ����ֵ��
//	�ɹ���������Ч�ľ������lpfs�ﷵ�ط��������ĵ�һ����¼
//	���򣺷���INVALID_HANDLE_VAULE
// ����������
//	�������������ļ�¼, ��������������Ա���������һ����¼
// ����: 
//	ϵͳAPI
// ************************************************

HANDLE WINAPI Table_FindFirst( HTABLE hTable, UINT uField, const void * lpcvFind, UINT uLen, LPFINDSTRUCT lpfs )
{
    LPTABLE lpTable;
	LPTABLE_FILE lpTableFile;

	// 127 ? because in _IsMatch, can read max 256 bytes
	// if lpFind > 127 , the IsMatch can't handle correctly
    // ������
	if( uLen == 0 || uLen > 127 )
        return INVALID_HANDLE_VALUE;

    if( ( lpTableFile = _GetHTABLEPtr( hTable ) ) && 
        ( lpTableFile->dwAccessMode & GENERIC_READ ) &&
		( lpTable = lpTableFile->lpTable ) )	// �����ݱ��ļ�����õ����ݱ��ļ�����ָ��
    {
		if( LockRecord( lpTableFile, 0 ) )	// ��ס���ݱ��ļ�
		{
			if( lpTable->dwRN )
			{	// ����һ�������ṹ����
				PTABLEFIND pFind = malloc( sizeof( TABELFIND ) + uLen + 256 );
				
				if( pFind )
				{				    
					// ��ʼ�������ṹ
					pFind->objType = OBJ_FINDTABLE;
					pFind->lpvFind = (VOID*)(pFind + 1);
					pFind->lpvBuf = (BYTE*)pFind->lpvFind + uLen;
					if( uField == -1 )
					{   // ���������ֶΡ�match all filed
						pFind->uFieldTo = lpTable->tfh.nFieldNum;
						pFind->uFieldFrom = 1;
					}
					else   // ֻ����ָ�����ֶΡ�match only this field
						pFind->uFieldFrom = pFind->uFieldTo = (BYTE)uField;
					pFind->uLen = uLen;
					memcpy( pFind->lpvFind, lpcvFind, uLen );
					pFind->hTable = hTable;            
					lpTableFile->rn = _SetRecordPointerAndUpdate( lpTable, 1 );//��ȡ��һ����¼��Ϣ	         
					while( 1 )
					{
						UINT f;
						if( (f = _IsMatch( pFind )) != 0 )	// ������¼�Ƿ�ƥ��
						{	// ����Ҫ��
							lpfs->nRecord = lpTable->dwRN;
							lpfs->uField = f;
							UnlockRecord( lpTableFile );	// ����
							return pFind;
						}
						// ��ƥ�䣬��ȡ��һ����¼
						if( lpTable->dwRN < lpTable->tfh.nNodeNum )
							lpTableFile->rn = _SetRecordPointerAndUpdate( lpTable, lpTable->dwRN + 1 );
						else
							break;
					}
					// û���ҵ��κμ�¼���ͷ�����
					pFind->objType = OBJ_NULL;
					free( pFind );
				}
			}
			UnlockRecord( lpTableFile );	// ����
		}
    }
    return INVALID_HANDLE_VALUE;
}

// **************************************************
// ������BOOL WINAPI Table_FindNext( HANDLE hFind, LPFINDSTRUCT lpfs )
// ������
//	IN hFind - ���Ҷ�����
//	IN lpfs- FINDSTRUCTָ��ṹ�����ڽ��ܲ��ҵ�������
// ����ֵ��
//	�ɹ�������TRUE, lpfs�ṹ������������ļ�¼��Ϣ
//	���򣺷���FALSE
// ����������
//	������һ�����������ļ�¼	
// ����: 
//	ϵͳAPI 
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
			( lpTable = lpTableFile->lpTable ) )  // �в��Ҿ���õ���������ָ��
        {
			if( LockRecord( lpTableFile, 0 ) )	// ��ס���ݱ�
			{
				if( lpTable->dwRN )	//
				{	
					while( lpTable->dwRN < lpTable->tfh.nNodeNum )
					{	// ��ȡ��¼����
						lpTableFile->rn = _SetRecordPointerAndUpdate( lpTable, lpTable->dwRN + 1 );
						if( (f = _IsMatch( pFind )) != 0 )	// ��¼�Ƿ����Ҫ��
						{	// ����Ҫ��
							lpfs->nRecord = lpTable->dwRN;
							lpfs->uField = f;
							UnlockRecord( lpTableFile );	// ����
							return TRUE;
						}
					}
				}
				UnlockRecord( lpTableFile );	// ����
			}
		}
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI Table_CloseFind( HANDLE hFind )
// ������
//	IN hFind - �����ļ����
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�ر��������	
// ����: 
//	ϵͳAPI
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
// ������static BOOL DoGetTableInfo( LPTABLE_FILE lpTableFile, LPTABLEINFO lpti )
// ������
//		IN lpTableFile - ���ݱ��ļ�ָ��
//		IN lpti - TABLEINFO �ṹָ�룬���ڽ������ݱ���Ϣ
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�õ����ݱ���Ϣ	
// ����: 
//	
// ************************************************

static BOOL DoGetTableInfo( LPTABLE_FILE lpTableFile, LPTABLEINFO lpti )
{
	LPTABLE lpTable;

	if( ( lpTable = lpTableFile->lpTable ) )
	{
		if( LockRecord( lpTableFile, 0 ) )	// ��ס���ݱ�
		{	// 
			lpti->dwFieldNum = lpTable->tfh.nFieldNum;
			lpti->dwRecordNum = lpTable->tfh.nNodeNum;
			lpti->dwVersion = lpTable->tfh.ver;
			lpti->dwUserDataSize = MAX_USERDATA;
			// �õ��ļ�ʱ��
			GetFileTime( lpTable->hFile, &lpti->ftCreationTime, &lpti->ftLastAccessTime, &lpti->ftLastWriteTime );
			// �õ��ļ�����
			lpti->dwFileAttrib = GetFileAttributes( lpTableFile->lpTable->lpszFileName );
			UnlockRecord( lpTableFile );	// �������ݱ�
			return TRUE;
		}
	}
	return FALSE;
}

// **************************************************
// ������BOOL WINAPI Table_GetTableInfoByHandle( HANDLE hTable, LPTABLEINFO lpti )
// ������
//		IN hTable - ���ݱ��ļ����
//		IN lpti - TABLEINFO �ṹָ�룬���ڽ������ݱ���Ϣ
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�õ����ݱ���Ϣ	
// ����: 
//	ϵͳAPI	
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
// ������BOOL WINAPI Table_GetTableInfo( LPCTSTR lpcszFileName, LPTABLEINFO lpti )
// ������
//		IN lpcszFileName - �ļ���
//		IN lpti - TABLEINFO �ṹָ�룬���ڽ������ݱ���Ϣ
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�õ����ݱ���Ϣ
// ����: 
//	ϵͳAPI	
// ************************************************

BOOL WINAPI Table_GetTableInfo( LPCTSTR lpcszFileName, LPTABLEINFO lpti )
{
	BOOL bRetv  = FALSE;

	if( lpti->dwSize == sizeof( TABLEINFO ) )
	{	// �����ݱ��ļ�
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
				bRetv = DoGetTableInfo( lpTableFile, lpti ); // �õ����ݱ���Ϣ
			}
			Table_Close( hTable );	// �ر����ݱ�
		}
	}
	else
	{
		WARNMSG( 1,  ( "error: Call Table_GetTableInfo, LPTABLEINFO's size different.\r\n" ) );
	}
	return bRetv;
}

// **************************************************
// ������UINT WINAPI Table_ReadMultiRecords( 
//								HTABLE hTable, 
//								LPMULTI_RECORD lpmultiRecord )
// ������
//		IN hTable - ���ݱ������
//		IN/OUT lpmultiRecord- ���ڱ������ݵĽṹָ��
// ����ֵ��
//	�ɹ������ط���ʵ�ʶ��ļ�¼��
//	���򣺷���0
// ����������
//	�������¼�ͼ�¼�Ķ���ֶ����� 
// ����: 
//	ϵͳAPI	
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
	// ������
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
				{   //�Ƿ�����
					SetLastError( ERROR_INVALID_PARAMETER );
					return 0;
				}
			}
		}
        DEBUGMSG( DEBUG_ReadMultiRecords, (TEXT(" Table_ReadMultiRecords: LockRecord.\r\n") ) );
		if( LockRecord( lpTableFile, 0 ) )	// ��ס��
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
				{	// ������ field
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
				{ // ��ȫ�� field
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
			UnlockRecord( lpTableFile );	// ����
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
// ������UINT WINAPI Table_GetUserData( 
//					HTABLE hTable,					
//                  LPVOID lpUserBuf,
//				    UINT nBufSize
// ������
//		IN hTable - ���ݱ������
//		OUT lpUserBuf- ���ڵõ��û����ݵ��û�ָ��
//		IN  nBufSize - �û����ݵĳ���
// ����ֵ��
//	�ɹ������ط���ʵ�ʶ������ݳ���
//	���򣺷���0
// ����������
//	��ȥ�û��Զ�������
// ����: 
//	ϵͳAPI
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
				if( LockRecord( lpTableFile, 0 ) )	// ��ס���ݱ�
				{	// 
					nBufSize = MIN( nBufSize, MAX_USERDATA );
					memcpy( lpUserBuf, lpTable->tfh.bUserData, nBufSize );
					
					UnlockRecord( lpTableFile );	// �������ݱ�
					return nBufSize;
				}
			}
		}
		else
		{ //��ȡ����
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
// ������UINT WINAPI Table_SetUserData( 
//					HTABLE hTable,					
//                  LPCVOID lpUserBuf,
//				    UINT nWriteSize
// ������
//		IN hTable - ���ݱ������
//		IN lpUserBuf- ����д�����ݱ���û����ݵ�ָ��
//		IN  nWriteSize - ��Ҫд���û����ݵĳ���
// ����ֵ��
//	�ɹ������ط���ʵ��д�����ݳ���
//	���򣺷���0
// ����������
//	д��/�����û��Զ�������
// ����: 
//	ϵͳAPI
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
				if( LockRecord( lpTableFile, LRF_SET_FLUSH_FLAG ) )	// ��ס���ݱ�
				{	// 
					nWriteSize = MIN( nWriteSize, MAX_USERDATA );
					memcpy( lpTable->tfh.bUserData, lpUserBuf, nWriteSize );
					
					UnlockRecord( lpTableFile );	// �������ݱ�
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
// ������BOOL WINAPI Table_Flush( HTABLE hTable )
// ������
//		IN hTable - ���ݱ������
// ����ֵ��
//	�ɹ������ط���TRUE
//	���򣺷���FALSE
// ����������
//	ˢ�����ݣ�д����̣�
// ����: 
//	ϵͳAPI
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

		//��Ҫ������ ��
		if( ( lpTableFile->dwAccessMode & GENERIC_WRITE ) &&
			( lpTable = lpTableFile->lpTable ) )
		{
			if( LockRecord( lpTableFile, 0 ) )	// ��ס���ݱ�
			{	
				if( lpTable->bFlush )
				{
					WORD n = GetBlockNum( lpTable, lpTable->tfh.wFatStartBlock );
					// �������ݱ�FAT
					DEBUGMSG( DEBUG_Flush | DEBUG_DOFLUSH, (TEXT(" Table_Flush: flushing(0x%x)...\r\n"), hTable ) );

					WriteBlocks( lpTable, lpTable->tfh.wFatStartBlock, 0, (BYTE*)lpTable->pfat, n * lpTable->tfh.nBytesPerBlock );
					//WriteBlocks( lpTable, 0, 0, (BYTE*)&lpTable->tfh  , sizeof( lpTable->tfh ) );
					UpdateTableHead( lpTable );
					
					FlushFileBuffers( lpTableFile->lpTable->hFile );
					lpTable->bFlush = FALSE;
				}
				UnlockRecord( lpTableFile );	// �������ݱ�
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
