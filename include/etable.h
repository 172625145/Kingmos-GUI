/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ETABLE_H
#define __ETABLE_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#ifndef __EFILE_H
#include <efile.h>
#endif

#define HTABLE HANDLE

//创建数据表文件或打开数据表文件，并返回对象句柄
#define CreateTable Tb_Create
HTABLE WINAPI Tb_Create( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, UINT nFieldNum );

//固定文件大小的数据表，如果　dwTableFileSize　== 0, 与Tb_Create一样,文件大小是可变的
#define CreateTableEx Tb_CreateEx
HTABLE WINAPI Tb_CreateEx( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, UINT nFieldNum, DWORD dwTableFileSize );

typedef struct _CREATE_TABLE
{
	DWORD dwSize;   // must = sizeof( CREATE_TABLE )
	const char * lpcszFileName;  //表文件名
	DWORD dwAccess; // 存取许可： GENERIC_WRITE , GENERIC_FILE(参看CreateFile)
	DWORD dwShareMode; // 共享模式： FILE_SHARE_READ, FILE_SHARE_WRITE(参看CreateFile)
	LPSECURITY_ATTRIBUTES lpSecurityAttributes;
	DWORD dwCreate;  // 创建标志： CREATE_ALWAYS, CREATE_NEWS, ...(参看CreateFile)
	DWORD dwFlagsAndAttributes;   //数据库标志和文件属性
	UINT nFieldNum;  // 该数据库有多少字段（只有当 CREATE_NEWS, CREATE_ALWAYS才有效）
	DWORD dwFileSizeLimit;	//是否限制数据表的文件大小？如果为0，不限制。
}CREATE_TABLE, FAR * LPCREATE_TABLE;

#define CreateTableByStruct Tb_CreateByStruct
HTABLE WINAPI Tb_CreateByStruct( LPCREATE_TABLE lpct ); 

//删除数据表文件
#define DeleteTable Tb_Delete
BOOL WINAPI Tb_Delete( LPCTSTR lpcszFileName );

//关闭数据表文件
#define CloseTable Tb_Close
BOOL WINAPI Tb_Close( HTABLE hTable );

// SetRecordPointer dwMethod
#define SRP_BEGIN   0
#define SRP_CURRENT 1
#define SRP_END     2
//设置当前的记录指针
#define SetRecordPointer Tb_SetRecordPointer
WORD WINAPI Tb_SetRecordPointer( HTABLE hTable, long lOffset, UINT fMethod );
//将当前的记录指针设置到下一条
#define NextRecordPointer Tb_NextRecordPointer
WORD WINAPI Tb_NextRecordPointer( HTABLE hTable, BOOL bNext );
//读一条记录的字段
#define ReadField Tb_ReadField
WORD WINAPI Tb_ReadField( HTABLE hTable, UINT nField, WORD wStartPos, void * lpBuf, WORD wSizeLimit );
//写一条记录的字段
#define WriteField Tb_WriteField
WORD WINAPI Tb_WriteField( HTABLE hTable, UINT nField, const void * lpcvBuf, WORD wSize );

typedef struct _FIELDITEM
{
    UINT size;
    void * lpData;
}FIELDITEM, FAR * LPFIELDITEM;
#define WriteRecord Tb_WriteRecord
//写一条记录
long WINAPI Tb_WriteRecord( HTABLE hTable, FIELDITEM fieldItem[] );

#define DR_REMOVE       0x00000000		//将该记录物理释放
#define DR_NOREMOVE     0x00000001		//逻辑删除, 仅仅做删除标志
#define DR_CURRENT      (-1)

#define DeleteRecord( hTable ) Tb_DeleteRecord( (hTable), DR_CURRENT, DR_REMOVE )
//删除一条记录,假如逻辑删除，记录的顺序不变，如果书物理删除，该记录下的所有记录的
//顺序号减一
// id dwRecord == -1, use current record
BOOL WINAPI Tb_DeleteRecord( HTABLE hTable, UINT uiRecord, DWORD dwFlag );

// the field size is limited to 64 bytes
// if lpField0 < lpField1 return value < 0
// if lpField0 == lpField1 return value = 0
// if lpField0 > lpField1 return value > 0
typedef struct _COMPSTRUCT
{
    WORD size0;
    BYTE field0[64];
    WORD size1;
    BYTE field1[64];
}COMPSTRUCT, FAR * LPCOMPSTRUCT;
typedef const COMPSTRUCT FAR * LPCCOMPSTRUCT;

typedef int (*FIELDCOMPPROC)( LPCCOMPSTRUCT );
//对记录排序
#define SortRecord Tb_SortRecord
BOOL WINAPI Tb_SortRecord( HTABLE hTable, WORD field, FIELDCOMPPROC lpfcProc );

// add a record at bottom
//添加一条记录到尾
#define AppendRecord Tb_AppendRecord
BOOL WINAPI Tb_AppendRecord( HTABLE hTable, FIELDITEM fieldItem[] );
//读一条记录
#define ReadRecord Tb_ReadRecord
BOOL WINAPI Tb_ReadRecord( 
					HTABLE hTable,
                    void * lpvData,
                    FIELDITEM fieldItem[],
                    DWORD * lpdwSize );

//typedef struct _RECORD_FIELD
//{
//	FIELDITEM fields[1];	//需要读的字段数组，必须 <= 总字段数
//}RECORD_FIELD, FAR * LPRECORD_FIELD;

typedef struct _MULTI_HEADER_INFO
{
	DWORD dwSize;   // 结构大小 = sizeof(MULTI_HEADER)

	UINT  nStartRecord;   // 开始读记录
	UINT  nRecordNumber;  // 需要读的记录数	
	
	UINT  nFileldNumber;   //需要读的字段数，必须 <= 总字段数
	LPUINT lpFileldInfo;   //nFileldNumber 个字段信息
	                       //如果 lpFileldInfo 和 nFileldNumber 都为0
	                       //表示需要读全部字段
}MULTI_HEADER_INFO, FAR * LPMULTI_HEADER_INFO;

typedef struct _MULTI_RECORD
{
    MULTI_HEADER_INFO mrHeaderInfo;
	//RECORD_FIELD mrRecords[1]; 
	FIELDITEM fields[1];// 用于接收 nRecordNumber 个 记录的二维结构数组 [record NO.][field NO.]
	                    //总数 = nRecordNumber * nFileldNumber;
}MULTI_RECORD, FAR * LPMULTI_RECORD;
//连续读 n 个记录
#define ReadMultiRecords Tb_ReadMultiRecords
//返回实际读的记录数， 0失败
UINT WINAPI Tb_ReadMultiRecords( 
					HTABLE hTable,					
                    LPMULTI_RECORD lpmultiRecord
                   );

//插入一条记录到指定的位置
#define InsertRecord Tb_InsertRecord
BOOL WINAPI Tb_InsertRecord( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] );
//统计数据表的总记录数
#define CountRecord Tb_CountRecord
int WINAPI Tb_CountRecord( HTABLE hTable );
// get current record info

#define RIM_DELETED       0x00000001
#define RIM_RECORDSIZE    0x00000002
#define RIM_FIELDSIZE     0x00000004
typedef struct _RECORDINFO
{   //ri
	UINT  uiMask;  //  = combine RIM_DELETED , RIM_RECORDSIZE...
	BOOL  bIsDeleted;   // is logic delete ? 
	UINT  uiRecordSize;        // record size
	UINT  uiFieldSize[1];      //  size of each field
}RECORDINFO, FAR * LPRECORDINFO;
//得到记录信息
#define GetRecordInfo Tb_GetRecordInfo
BOOL WINAPI Tb_GetRecordInfo( HTABLE hTable, LPRECORDINFO lpri );


typedef struct _TABLEINFO
{
	DWORD dwSize; // = sizeof( TABLEINFO )
	//  version
	DWORD dwVersion;
    // file info
	DWORD dwFileAttrib;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime; 
    FILETIME ftLastWriteTime;
	// table info
 	DWORD  dwFieldNum;
	DWORD  dwRecordNum;
	DWORD  dwUserDataSize;  //用户数据区长度
}TABLEINFO, FAR * LPTABLEINFO;

//得到数据表信息
#define GetTableInfo Tb_GetTableInfo
BOOL WINAPI Tb_GetTableInfo( LPCTSTR lpcszFileName, LPTABLEINFO lpti );
//得到数据表信息
#define GetTableInfoByHandle Tb_GetTableInfoByHandle
BOOL WINAPI Tb_GetTableInfoByHandle( HTABLE hTable, LPTABLEINFO lpti );
//得到记录大小
#define GetRecordSize Tb_GetRecordSize
DWORD WINAPI Tb_GetRecordSize( HTABLE hTable );


// if field = -1, match all field, also only match single field
// the lpcvFind not  > 127
typedef struct _FINDSTRUCT
{
    UINT nRecord;  // 
    UINT uField;
}FINDSTRUCT, FAR * LPFINDSTRUCT;
// the uLen only support <= 127 bytes
//查找符合条件的记录，如果找到一条，返回该条记录和查找句柄。否则返回INVALID_HANDLE_VALUE
#define FindFirstRecord Tb_FindFirst
HANDLE WINAPI Tb_FindFirst( HTABLE hTable, UINT uField, const void * lpcvFind, UINT uLen, LPFINDSTRUCT lpfs );
//查找符合条件下一条记录
#define FindNextRecord Tb_FindNext
BOOL WINAPI Tb_FindNext( HANDLE hFind, LPFINDSTRUCT lpfs );

#define CloseTableFind Tb_CloseFind
BOOL WINAPI Tb_CloseFind( HANDLE hFind );


#define PT_NOREMOVE  0x0000			//不物理删除之前已经被逻辑删除的记录
#define PT_REMOVE    0x0001			//物理删除之前已经被逻辑删除的记录

#define PT_COMPRESS    0x0000			//压缩数据表文件大小
#define PT_NOCOMPRESS  0x0002			//不压缩数据表文件大小

//压缩数据表文件,调用该函数时该表应该没有被打开
#define PackTable( lpszName )  Tb_Pack( (lpszName), PT_NOREMOVE )
BOOL WINAPI Tb_Pack( LPCTSTR lpcszFileName, UINT uiFlag );

//压缩数据表文件，调用该函数后
//1.应用程序应该同步自己的数据
//2.当前位置在开始位置
#define PackTableByHandle( hTable )  Tb_PackByHandle( (hTable), PT_NOREMOVE )
BOOL WINAPI Tb_PackByHandle( HTABLE hTable, UINT uiFlag );


//	写入/更新用户自定义数据 nWriteBufSize 必须 <= 48个字节
#define SetTableUserData Tb_SetUserData
UINT WINAPI Tb_SetUserData( 
					HTABLE hTable,					
                    LPCVOID lpUserBuf,
					UINT nWriteBufSize
                   );
//	读取用户自定义数据
#define GetTableUserData Tb_GetUserData
UINT WINAPI Tb_GetUserData( 
					HTABLE hTable,					
                    LPVOID lpUserBuf,
					UINT nReadBufSize
                   );
//刷新数据表数据（写入磁盘）
#define FlushTable Tb_Flush
BOOL WINAPI Tb_Flush( HTABLE hTable );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__ETABLE_H
