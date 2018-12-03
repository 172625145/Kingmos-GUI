/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
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

//�������ݱ��ļ�������ݱ��ļ��������ض�����
#define CreateTable Tb_Create
HTABLE WINAPI Tb_Create( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, UINT nFieldNum );

//�̶��ļ���С�����ݱ������dwTableFileSize��== 0, ��Tb_Createһ��,�ļ���С�ǿɱ��
#define CreateTableEx Tb_CreateEx
HTABLE WINAPI Tb_CreateEx( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreate, UINT nFieldNum, DWORD dwTableFileSize );

typedef struct _CREATE_TABLE
{
	DWORD dwSize;   // must = sizeof( CREATE_TABLE )
	const char * lpcszFileName;  //���ļ���
	DWORD dwAccess; // ��ȡ��ɣ� GENERIC_WRITE , GENERIC_FILE(�ο�CreateFile)
	DWORD dwShareMode; // ����ģʽ�� FILE_SHARE_READ, FILE_SHARE_WRITE(�ο�CreateFile)
	LPSECURITY_ATTRIBUTES lpSecurityAttributes;
	DWORD dwCreate;  // ������־�� CREATE_ALWAYS, CREATE_NEWS, ...(�ο�CreateFile)
	DWORD dwFlagsAndAttributes;   //���ݿ��־���ļ�����
	UINT nFieldNum;  // �����ݿ��ж����ֶΣ�ֻ�е� CREATE_NEWS, CREATE_ALWAYS����Ч��
	DWORD dwFileSizeLimit;	//�Ƿ��������ݱ���ļ���С�����Ϊ0�������ơ�
}CREATE_TABLE, FAR * LPCREATE_TABLE;

#define CreateTableByStruct Tb_CreateByStruct
HTABLE WINAPI Tb_CreateByStruct( LPCREATE_TABLE lpct ); 

//ɾ�����ݱ��ļ�
#define DeleteTable Tb_Delete
BOOL WINAPI Tb_Delete( LPCTSTR lpcszFileName );

//�ر����ݱ��ļ�
#define CloseTable Tb_Close
BOOL WINAPI Tb_Close( HTABLE hTable );

// SetRecordPointer dwMethod
#define SRP_BEGIN   0
#define SRP_CURRENT 1
#define SRP_END     2
//���õ�ǰ�ļ�¼ָ��
#define SetRecordPointer Tb_SetRecordPointer
WORD WINAPI Tb_SetRecordPointer( HTABLE hTable, long lOffset, UINT fMethod );
//����ǰ�ļ�¼ָ�����õ���һ��
#define NextRecordPointer Tb_NextRecordPointer
WORD WINAPI Tb_NextRecordPointer( HTABLE hTable, BOOL bNext );
//��һ����¼���ֶ�
#define ReadField Tb_ReadField
WORD WINAPI Tb_ReadField( HTABLE hTable, UINT nField, WORD wStartPos, void * lpBuf, WORD wSizeLimit );
//дһ����¼���ֶ�
#define WriteField Tb_WriteField
WORD WINAPI Tb_WriteField( HTABLE hTable, UINT nField, const void * lpcvBuf, WORD wSize );

typedef struct _FIELDITEM
{
    UINT size;
    void * lpData;
}FIELDITEM, FAR * LPFIELDITEM;
#define WriteRecord Tb_WriteRecord
//дһ����¼
long WINAPI Tb_WriteRecord( HTABLE hTable, FIELDITEM fieldItem[] );

#define DR_REMOVE       0x00000000		//���ü�¼�����ͷ�
#define DR_NOREMOVE     0x00000001		//�߼�ɾ��, ������ɾ����־
#define DR_CURRENT      (-1)

#define DeleteRecord( hTable ) Tb_DeleteRecord( (hTable), DR_CURRENT, DR_REMOVE )
//ɾ��һ����¼,�����߼�ɾ������¼��˳�򲻱䣬���������ɾ�����ü�¼�µ����м�¼��
//˳��ż�һ
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
//�Լ�¼����
#define SortRecord Tb_SortRecord
BOOL WINAPI Tb_SortRecord( HTABLE hTable, WORD field, FIELDCOMPPROC lpfcProc );

// add a record at bottom
//���һ����¼��β
#define AppendRecord Tb_AppendRecord
BOOL WINAPI Tb_AppendRecord( HTABLE hTable, FIELDITEM fieldItem[] );
//��һ����¼
#define ReadRecord Tb_ReadRecord
BOOL WINAPI Tb_ReadRecord( 
					HTABLE hTable,
                    void * lpvData,
                    FIELDITEM fieldItem[],
                    DWORD * lpdwSize );

//typedef struct _RECORD_FIELD
//{
//	FIELDITEM fields[1];	//��Ҫ�����ֶ����飬���� <= ���ֶ���
//}RECORD_FIELD, FAR * LPRECORD_FIELD;

typedef struct _MULTI_HEADER_INFO
{
	DWORD dwSize;   // �ṹ��С = sizeof(MULTI_HEADER)

	UINT  nStartRecord;   // ��ʼ����¼
	UINT  nRecordNumber;  // ��Ҫ���ļ�¼��	
	
	UINT  nFileldNumber;   //��Ҫ�����ֶ��������� <= ���ֶ���
	LPUINT lpFileldInfo;   //nFileldNumber ���ֶ���Ϣ
	                       //��� lpFileldInfo �� nFileldNumber ��Ϊ0
	                       //��ʾ��Ҫ��ȫ���ֶ�
}MULTI_HEADER_INFO, FAR * LPMULTI_HEADER_INFO;

typedef struct _MULTI_RECORD
{
    MULTI_HEADER_INFO mrHeaderInfo;
	//RECORD_FIELD mrRecords[1]; 
	FIELDITEM fields[1];// ���ڽ��� nRecordNumber �� ��¼�Ķ�ά�ṹ���� [record NO.][field NO.]
	                    //���� = nRecordNumber * nFileldNumber;
}MULTI_RECORD, FAR * LPMULTI_RECORD;
//������ n ����¼
#define ReadMultiRecords Tb_ReadMultiRecords
//����ʵ�ʶ��ļ�¼���� 0ʧ��
UINT WINAPI Tb_ReadMultiRecords( 
					HTABLE hTable,					
                    LPMULTI_RECORD lpmultiRecord
                   );

//����һ����¼��ָ����λ��
#define InsertRecord Tb_InsertRecord
BOOL WINAPI Tb_InsertRecord( HTABLE hTable, WORD nPos, FIELDITEM fieldItem[] );
//ͳ�����ݱ���ܼ�¼��
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
//�õ���¼��Ϣ
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
	DWORD  dwUserDataSize;  //�û�����������
}TABLEINFO, FAR * LPTABLEINFO;

//�õ����ݱ���Ϣ
#define GetTableInfo Tb_GetTableInfo
BOOL WINAPI Tb_GetTableInfo( LPCTSTR lpcszFileName, LPTABLEINFO lpti );
//�õ����ݱ���Ϣ
#define GetTableInfoByHandle Tb_GetTableInfoByHandle
BOOL WINAPI Tb_GetTableInfoByHandle( HTABLE hTable, LPTABLEINFO lpti );
//�õ���¼��С
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
//���ҷ��������ļ�¼������ҵ�һ�������ظ�����¼�Ͳ��Ҿ�������򷵻�INVALID_HANDLE_VALUE
#define FindFirstRecord Tb_FindFirst
HANDLE WINAPI Tb_FindFirst( HTABLE hTable, UINT uField, const void * lpcvFind, UINT uLen, LPFINDSTRUCT lpfs );
//���ҷ���������һ����¼
#define FindNextRecord Tb_FindNext
BOOL WINAPI Tb_FindNext( HANDLE hFind, LPFINDSTRUCT lpfs );

#define CloseTableFind Tb_CloseFind
BOOL WINAPI Tb_CloseFind( HANDLE hFind );


#define PT_NOREMOVE  0x0000			//������ɾ��֮ǰ�Ѿ����߼�ɾ���ļ�¼
#define PT_REMOVE    0x0001			//����ɾ��֮ǰ�Ѿ����߼�ɾ���ļ�¼

#define PT_COMPRESS    0x0000			//ѹ�����ݱ��ļ���С
#define PT_NOCOMPRESS  0x0002			//��ѹ�����ݱ��ļ���С

//ѹ�����ݱ��ļ�,���øú���ʱ�ñ�Ӧ��û�б���
#define PackTable( lpszName )  Tb_Pack( (lpszName), PT_NOREMOVE )
BOOL WINAPI Tb_Pack( LPCTSTR lpcszFileName, UINT uiFlag );

//ѹ�����ݱ��ļ������øú�����
//1.Ӧ�ó���Ӧ��ͬ���Լ�������
//2.��ǰλ���ڿ�ʼλ��
#define PackTableByHandle( hTable )  Tb_PackByHandle( (hTable), PT_NOREMOVE )
BOOL WINAPI Tb_PackByHandle( HTABLE hTable, UINT uiFlag );


//	д��/�����û��Զ������� nWriteBufSize ���� <= 48���ֽ�
#define SetTableUserData Tb_SetUserData
UINT WINAPI Tb_SetUserData( 
					HTABLE hTable,					
                    LPCVOID lpUserBuf,
					UINT nWriteBufSize
                   );
//	��ȡ�û��Զ�������
#define GetTableUserData Tb_GetUserData
UINT WINAPI Tb_GetUserData( 
					HTABLE hTable,					
                    LPVOID lpUserBuf,
					UINT nReadBufSize
                   );
//ˢ�����ݱ����ݣ�д����̣�
#define FlushTable Tb_Flush
BOOL WINAPI Tb_Flush( HTABLE hTable );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__ETABLE_H
