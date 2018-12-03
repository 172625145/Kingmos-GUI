/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����ע������
�汾�ţ�1.0.0.456
����ʱ�ڣ�2002-03-06
���ߣ��½��� Jami Chen
�޸ļ�¼��    
		2004.05.11 Jami chen : �Ժ����������޸ģ�KL_Reg...  --> FileReg_...
		2004.05.11 Jami chen : ���ڴ���䣬�ͷź����޸� _kalloc , _kfree  --> malloc, free
		2004.05.11 Jami chen : �Լ��ļ����ͼ�ֵ�Ƚ�ʱ�����ִ�Сд���� strcmp,strncmp  --> stricmp,strnicmp
******************************************************/
//#define TEST_MEM_SIZE 

#ifdef KINGMOS

#include <ewindows.h>
//#include "epalloc.h"
//#include <coresrv.h>
#include "estdlib.h"

#else
    #include <windows.h>    
	//#ifndef TEST_MEM_SIZE 	
//	#define malloc malloc
//	#define free  free
	//#endif
#endif

#include "ereg.h"

// *****************************************************
// ��������
// *****************************************************
#undef ROOT_MICROSOFT

typedef struct structRegValue{
	struct structRegValue *hNextValue;  // the next value of the key
	LPTSTR lpValueName;
	DWORD dwType;    // the type of the value ,eg :DWORD ...
	LPBYTE lpData;    // the data of the value 
	DWORD cbData;   // the length og the data
} *LPREGVALUE ,REGVALUE;


typedef struct structRegKey{
	struct structRegKey *hSubKey;   // the sub key of the key
	struct structRegKey *hNextKey;  // the next key of the key
	LPREGVALUE hRegValue;  // the value of the key
	LPTSTR lpKeyName;
	LPTSTR lpClass;       // Pointer to a null-terminated string that specifies the class (object type) of this key
	BOOL bOpen;           // for the key is or not to be open ,if the key is open 
						  // then can modified the value,and read the value,otherwise
						  // can't read or write the value of the key
} *LPREGKEY,REGKEY;

#ifdef ROOT_MICROSOFT

static LPREGKEY    hkey_classes_root=NULL;
static LPREGKEY    hkey_current_user=NULL;
static LPREGKEY    hkey_local_machine=NULL;
static LPREGKEY    hkey_users=NULL;

#else
// !!! Modified By Jami chen in 2003.06.27 
//static LPREGKEY    hkey_root=NULL;
// !!!
static LPREGKEY    hkey_hardware_root=NULL;
static LPREGKEY    hkey_software_root=NULL;
// !!!

#endif

/*********************************************************************************/
// *****************************************************
// ����������
// *****************************************************
static BOOL OpenCurrentRegKey(LPREGKEY hRegKey);
static LPREGKEY MapRootKey(HKEY hKey);
static LPREGKEY SearchSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey);
static LPREGKEY SearchSubKeyByIndex(LPREGKEY hRegKey,DWORD iIndex);
static BOOL MatchSubKey(LPREGKEY hSubRegKey,LPCTSTR lpSubKey);
static DWORD GetSubKeyLen(LPCTSTR lpSubKey);
static LPREGKEY CreateNewSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey);
static LPTSTR ToNextSubKey(LPCTSTR lpSubKey);
static LPTSTR AssignSubKeyname(LPCTSTR lpSubKey);
static LPREGKEY GetTailSubKey(LPREGKEY hSubKey);
static BOOL DeleteSubRegKey(LPREGKEY hRegKey,LPREGKEY hSubKey);
static void	FreeRegKey(LPREGKEY hKey);
static BOOL IsOpenKey(LPREGKEY hRegKey);
static LPREGVALUE SearchValue(LPREGKEY hRegKey,LPCTSTR lpValueName);  
static LPREGVALUE SearchValueByIndex(LPREGKEY hRegKey, DWORD dwIndex);
static LPREGVALUE CreateNewValue(LPREGKEY hRegKey,LPCTSTR lpValueName);
static LPTSTR AssignRegValue(LPCTSTR lpValueName);
static LPREGVALUE GetTailValue(LPREGVALUE hRegValue);
static BOOL SetValue(LPREGVALUE hRegValue,DWORD dwType,const BYTE *lpData,DWORD cbData); 
static LONG GetValue(LPREGVALUE hRegValue,LPDWORD lpType,const BYTE *lpData,LPDWORD lpcbData);  
static LONG DeleteValue(LPREGKEY hRegKey,LPREGVALUE hRegValue);  
static void	FreeRegValue(LPREGVALUE hRegValue);
static void QueryInfoSubKey(LPREGKEY hRegKey,LPDWORD lpcSubKeys,LPDWORD lpcbMaxSubKeyLen,LPDWORD lpcbMaxClassLen);
static void QueryInfoValue(LPREGKEY hRegKey,LPDWORD lpcValues,LPDWORD lpcbMaxValueNameLen,LPDWORD lpcbMaxValueLen);


static HANDLE WINAPI CreateRegFile( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
static BOOL ReadRegFile( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped );
static BOOL WriteRegFile( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped );
static BOOL CloseRegHandle(HANDLE  hHandle);
/*********************************************************************************/
/*
#ifdef TEST_MEM_SIZE
DWORD dwUseMem = 0;
void * malloc( UINT sz )
{
	DWORD * lp = (DWORD*)malloc( sz + sizeof( DWORD ) );
	if( lp )
	{
		*lp = sz;
		lp++;
		dwUseMem+=sz;
	}
	return lp;
}

void free( void * lpUse )
{
	if( lpUse )
	{
	    DWORD * lp = (DWORD*)lpUse;
		lp--;
		dwUseMem -= *lp;
		free( lp );
	}
}
#endif

  */
// **************************************************
// ������LONG WINAPI FileReg_CloseKey( HKEY hKey )
// ������
// 	IN hKey -- ָ���ļ��ľ��
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�����򷵻ش������
// �����������ر�ע�����
// ����: 
// **************************************************
LONG WINAPI FileReg_CloseKey( HKEY hKey )
{
	LPREGKEY hRegKey;

	hRegKey=(LPREGKEY)hKey;  // �õ�ע����ṹ
	if (hRegKey==NULL)
		return ERROR_BADKEY;   // Current Key is valid , Can't Close it
	if (hRegKey->bOpen==FALSE)
		return ERROR_BADKEY;   // Current Key Had not Open , Can't need Close it
	hRegKey->bOpen=FALSE;  // ���ùر�����
	return ERROR_SUCCESS;  // ���سɹ�

}

// **************************************************
// ������LONG WINAPI FileReg_CreateKeyEx( HKEY hKey, LPCTSTR lpSubKey, DWORD Reserved, LPTSTR lpClass, 
//							DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition )
// ������
// 	IN hKey -- Ҫ�����Ӽ��ľ��
// 	IN lpSubKey -- �Ӽ�������
// 	IN Reserved -- ����
// 	IN lpClass -- ����
// 	IN dwOptions -- ��ѡ����
// 	IN samDesired -- ����
// 	IN lpSecurityAttributes  -- ��ȫ����
// 	OUT phkResult -- �����Ѿ������ļ��ľ��
// 	OUT lpdwDisposition -- ���ش򿪵ķ�ʽ
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д���
// ��������������һ��ָ���ļ���
// ����: 
// **************************************************
LONG WINAPI FileReg_CreateKeyEx( 
							HKEY hKey, 
							LPCTSTR lpSubKey, 
							DWORD Reserved, 
							LPTSTR lpClass, 
							DWORD dwOptions, 
							REGSAM samDesired, 
							LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
							PHKEY phkResult, 
							LPDWORD lpdwDisposition 
							)
{
	LPREGKEY hRegKey,hSubKey;
	DWORD dwDisposition=REG_OPENED_EXISTING_KEY;

		if (phkResult!=NULL)           // return handle of the key
			*phkResult=(HKEY)NULL;
		if (lpdwDisposition !=NULL)   // return the disposition type 
			*lpdwDisposition=0;

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key

		if (samDesired == 0)
		{
			RETAILMSG(1,(TEXT("ERROR ERROR ERROR Create Key [%s] failure !!!!!!!\r\n"),lpSubKey));
			return ERROR_CANTOPEN;
		}
		if (hRegKey==NULL)
			return ERROR_BADKEY;
		
		while(1)
		{
			hSubKey=SearchSubKey(hRegKey,lpSubKey);  // Search the SubKey is exist in the RegKey
			if (hSubKey==NULL)  // the Sub Key is not Exist ,must to Create New Sub Key
			{
				dwDisposition=REG_CREATED_NEW_KEY;  // ����Ҫ����һ���¼�
				hSubKey=CreateNewSubKey(hRegKey,lpSubKey); // ����һ���¼�
			}
			if (hSubKey==NULL)  // Create New Sub Key Failure,maybe is memory not enough
				return ERROR_NO_LOG_SPACE;  

			hRegKey=hSubKey;  // Set the Sub Key to Current Key, to Enter next sub key

			lpSubKey=ToNextSubKey(lpSubKey);  // go to the next sub Key

			if (*lpSubKey==0)
				break; // �����
		}
		if (OpenCurrentRegKey(hRegKey)==FALSE)  // Open current the Reg Key
			return ERROR_CANTOPEN;
		if (phkResult!=NULL)           // return handle of the key
			*phkResult=(HKEY)hRegKey;
		if (lpdwDisposition !=NULL)   // return the disposition type 
			*lpdwDisposition=dwDisposition;
		return ERROR_SUCCESS;
}

// **************************************************
// ������LONG WINAPI FileReg_DeleteKey( HKEY hKey,  LPCTSTR lpSubKey )
// ������
// 	IN hKey -- Ҫɾ���Ӽ��ľ��
// 	IN lpSubKey -- Ҫɾ�����Ӽ�
// 
// ����ֵ���ɹ�����ERROR_SUCCESS��������ڴ���
// ����������ɾ��ָ���ļ���
// ����: 
// **************************************************
LONG WINAPI FileReg_DeleteKey( 
						  HKEY hKey, 
						  LPCTSTR lpSubKey 
						  )
{
	LPREGKEY hRegKey,hSubKey;

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key
		if (hRegKey==NULL)
			return ERROR_BADKEY; // ָ���ļ�������
		
		while(1)
		{
			hSubKey=SearchSubKey(hRegKey,lpSubKey);  // Search the SubKey is exist in the RegKey
			if (hSubKey==NULL)  // the Sub Key is not Exist ,must to Create New Sub Key
			{ // û���ҵ�ָ�����Ӽ�
				return ERROR_BADKEY;
			}

			lpSubKey=ToNextSubKey(lpSubKey);  // go to the next sub Key

			if (*lpSubKey==0)  // Had no sub Key
				break;  // �Ѿ�û���Ӽ���

			hRegKey=hSubKey;  // Set the Sub Key to Current Key, to Enter next sub key

		}
		if (DeleteSubRegKey(hRegKey,hSubKey)==FALSE)  // delete the sub key
			return ERROR_KEY_DELETED;
		return ERROR_SUCCESS;
}

// **************************************************
// ������LONG WINAPI FileReg_DeleteValue( HKEY hKey, LPCTSTR lpValueName )
// ������
// 	IN hKey -- Ҫɾ����ֵ�ľ��
// 	IN lpValueName -- Ҫɾ���ļ�ֵ������
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д�
// ����������ɾ��һ����ǰ�򿪵ļ��ļ�ֵ��
// ����: 
// **************************************************
LONG WINAPI FileReg_DeleteValue( 
					 HKEY hKey, 
					 LPCTSTR lpValueName 
					 )
{
	LPREGKEY hRegKey;
	LPREGVALUE hRegValue;

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key
		if (hRegKey==NULL)
			return ERROR_BADKEY;  // ��������
		if (IsOpenKey(hRegKey)==FALSE)  // the key is or not open ?
			return ERROR_BADKEY; // ��û�д�

		hRegValue=SearchValue(hRegKey,lpValueName);  // Search the Value have exist in the key 
		if (hRegValue==NULL)
		{  // ��ֵ������
			return ERROR_BADKEY;	
		}
		return DeleteValue(hRegKey,hRegValue);  // Delete current value
}

// **************************************************
// ������LONG WINAPI FileReg_EnumKeyEx(HKEY hKey, DWORD dwIndex, LPTSTR lpName, LPDWORD lpcbName, LPDWORD lpReserved, 
									//LPTSTR lpClass, LPDWORD lpcbClass, PFILETIME lpftLastWriteTime )

// ������
// 	IN hKey -- �Ѿ��򿪵ļ��ľ��
// 	IN dwIndex -- Ҫ�򿪵��Ӽ�������
// 	OUT lpName -- ����Ӽ����ƵĻ���
// 	IN/OUT lpcbName -- �Ӽ����Ƶĳ���
// 	IN lpReserved -- ����
// 	OUT lpClass -- ����Ӽ�����Ļ���
// 	IN/OUT lpcbClass -- �Ӽ�����ĳ���
// 	OUT lpftLastWriteTime -- ���һ�ε��޸�ʱ��
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д�
// �����������оٵ�ǰ�򿪵ļ��µ������Ӽ���
// ����: 
// **************************************************
LONG WINAPI FileReg_EnumKeyEx(
						  HKEY hKey, 
						  DWORD dwIndex, 
						  LPTSTR lpName, 
						  LPDWORD lpcbName, 
						  LPDWORD lpReserved, 
						  LPTSTR lpClass, 
						  LPDWORD lpcbClass, 
						  PFILETIME lpftLastWriteTime 
						  )
{
	LPREGKEY hRegKey,hSubKey;

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key
		if (hRegKey==NULL)
			return ERROR_BADKEY;  // ָ���ļ�������
		
		hSubKey=SearchSubKeyByIndex(hRegKey,dwIndex);  // Search the specical index is exist in the RegKey
		if (hSubKey==NULL)  // the Sub Key is not Exist ,have no more subkey
		{
			// �Ѿ�û���Ӽ��ˣ������ҵ����Ӽ�������Ϊ��
			if (lpName!=NULL)
				*lpName=0; 
			if (lpcbName!=NULL)
				*lpcbName=0;
			if (lpClass!=NULL)
				*lpClass=0;
			if (lpcbClass!=NULL)
				*lpcbClass=0;
			//return ERROR_SUCCESS;  // ���سɹ�
			return ERROR_NO_MORE_ITEMS; // �����Ѿ�û���Ӽ���
		}
		if (lpcbName!=NULL)
		{  // need read the key name 
			if (*lpcbName<(strlen(hSubKey->lpKeyName)+1))
			{  // the buffer is not enough  
				if (lpName!=NULL)
					*lpName=0; 
				*lpcbName=strlen(hSubKey->lpKeyName)+1;  // tell ap need the buffer size
				return ERROR_MORE_DATA;
			}
			else
			{   // read the real data to buffer
				if (lpName!=NULL)  
					strcpy(lpName,hSubKey->lpKeyName);
				*lpcbName=strlen(hSubKey->lpKeyName)+1;
			}
		}

		if (lpcbClass!=NULL)
		{  // need the key class
			if (*lpcbClass<(strlen(hSubKey->lpClass)+1))
			{  // the class buffer is not enough
				if (lpClass!=NULL)
					*lpClass=0; 
				*lpcbClass=strlen(hSubKey->lpClass)+1;  // tell the caller the buffer size
				return ERROR_MORE_DATA;  // return need more buffer
			}
			else
			{  // read the real data to the class buffer
				if (lpClass!=NULL)
					strcpy(lpClass,hSubKey->lpClass);
				*lpcbClass=strlen(hSubKey->lpClass)+1;
			}
		}

		return ERROR_SUCCESS; // ���سɹ�
}

 
// **************************************************
// ������LONG WINAPI FileReg_FlushKey( HKEY hKey )

// ������
// 	IN hKey -- ָ���ļ�
// 
// ����ֵ������ 0
// ���������� ˢ�¼�������
// ����: 
// **************************************************
LONG WINAPI FileReg_FlushKey( 
				  HKEY hKey 
				  )
{
	return 0;
}

// **************************************************
// ������LONG WINAPI FileReg_EnumValue( HKEY hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcbValueName, 
//									LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
// ������
// 	IN hKey -- �򿪵ļ�ֵ
// 	IN dwIndex -- Ҫ�õ��ļ�ֵ������
// 	OUT lpValueName -- ���ؼ�ֵ������
// 	IN/OUT lpcbValueName -- ���ؼ�ֵ�ĳ���
// 	IN lpReserved -- ����
// 	OUT lpType -- ���ؼ�ֵ���ݵ�����
// 	OUT lpData -- ���ؼ�ֵ����
// 	IN/OUT  lpcbData -- ���ؼ�ֵ���ݵĳ���
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д�
// �����������оٴ򿪵ļ��ļ�ֵ��
// ����: 
// **************************************************
LONG WINAPI FileReg_EnumValue( 
				   HKEY hKey, 
				   DWORD dwIndex, 
				   LPTSTR lpValueName, 
				   LPDWORD lpcbValueName, 
				   LPDWORD lpReserved, 
				   LPDWORD lpType, 
				   LPBYTE lpData, 
				   LPDWORD lpcbData 
				   )
{
	LPREGKEY hRegKey;
	LPREGVALUE hRegValue;

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key
		if (hRegKey==NULL)  // û���ҵ�ָ���ļ�
			return ERROR_BADKEY;
		if (IsOpenKey(hRegKey)==FALSE)  // the key is or not open ?
			return ERROR_BADKEY;

		hRegValue=SearchValueByIndex(hRegKey,dwIndex);  // Search the Value have exist in the key 
		if (hRegValue==NULL)
		{   // have not exist more value���Ѿ�û�м�ֵ��
			// �����Ѿ�û�м�ֵ
			if (lpValueName!=NULL)
				*lpValueName=0;
			if (lpcbValueName!=NULL)
				*lpcbValueName=0;
			if (lpType!=NULL)
				*lpType=0;
			if (lpcbData!=NULL) 
				*lpcbData=0;
//			return ERROR_SUCCESS;// ���سɹ�
			return ERROR_NO_MORE_ITEMS; // �����Ѿ�û���Ӽ���
		}
		if (lpcbValueName!=NULL)
		{  // need get the value name
			if (*lpcbValueName<(strlen(hRegValue->lpValueName)+1))
			{  // the value name buffer is not enough
				if (lpValueName!=NULL)
					*lpValueName=0;
				*lpcbValueName=strlen(hRegValue->lpValueName)+1;  // return the real size
				return ERROR_MORE_DATA;
			}
			else
			{ // �洢��ǰ�ļ�ֵ
				if (lpValueName!=NULL)
					strcpy(lpValueName,hRegValue->lpValueName);  // get the value name 
				*lpcbValueName=strlen(hRegValue->lpValueName)+1;  // return the real size
			}
		}
		if (lpType!=NULL)  // need get the data type
			*lpType=hRegValue->dwType;
		if (lpcbData!=NULL) 
		{  // need get the value data
			if (*lpcbData<hRegValue->cbData)
			{  // the data buffer is not enough
				*lpcbData=hRegValue->cbData;  // return the real size
				return ERROR_MORE_DATA;
			}
			else
			{  // the buffer is enough
				if (lpData!=NULL)
					memcpy(lpData,hRegValue->lpData,hRegValue->cbData);  // get the data
				*lpcbData=hRegValue->cbData;  // return the real size
			}
		}
		return ERROR_SUCCESS;// ���سɹ�
}

// **************************************************
// ������LONG WINAPI FileReg_OpenKeyEx( HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult )
// ������
// 	IN hKey -- Ҫ�����Ӽ��ľ��
// 	IN lpSubKey -- �Ӽ�������
// 	IN ulOptions -- ��ѡ����
// 	IN samDesired -- ����
// 	OUT phkResult -- �����Ѿ������ļ��ľ��
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д�
// ������������ָ�����Ӽ���
// ����: 
// **************************************************
LONG WINAPI FileReg_OpenKeyEx( 
				   HKEY hKey, 
				   LPCTSTR lpSubKey, 
				   DWORD ulOptions, 
				   REGSAM samDesired, 
				   PHKEY phkResult 
				   )
{
	LPREGKEY hRegKey,hSubKey;

		if (phkResult!=NULL)           // return handle of the key
			*phkResult=(HKEY)NULL;

		if (samDesired == 0)
		{
			RETAILMSG(1,(TEXT("ERROR ERROR ERROR Open Key [%s] failure !!!!!!!\r\n"),lpSubKey));
			return ERROR_CANTOPEN;
		}

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key
		if (hRegKey==NULL)
			return ERROR_BADKEY;  // û���ҵ�ָ���ļ�
		
		while(1)
		{
			hSubKey=SearchSubKey(hRegKey,lpSubKey);  // Search the SubKey is exist in the RegKey
			if (hSubKey==NULL)  // the Sub Key is not Exist 
			{  // û���ҵ�ָ�����Ӽ�
				return ERROR_BADKEY;
			}

			hRegKey=hSubKey;  // Set the Sub Key to Current Key, to Enter next sub key

			lpSubKey=ToNextSubKey(lpSubKey);  // go to the next sub Key

			if (*lpSubKey==0)
				break;  // �Ѿ�û���Ӽ���
		}
		if (OpenCurrentRegKey(hRegKey)==FALSE)  // Open current the Reg Key
			return ERROR_CANTOPEN;
		if (phkResult!=NULL)           // return handle of the key
			*phkResult=(HKEY)hRegKey;
		return ERROR_SUCCESS;  // ���سɹ�
}

// **************************************************
// ������LONG WINAPI FileReg_QueryInfoKey( HKEY hKey, LPTSTR lpClass, LPDWORD lpcbClass, LPDWORD lpReserved, LPDWORD lpcSubKeys, LPDWORD lpcbMaxSubKeyLen, 
//									   LPDWORD lpcbMaxClassLen, LPDWORD lpcValues, LPDWORD lpcbMaxValueNameLen, LPDWORD lpcbMaxValueLen, LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime )

// ������
// 	IN hKey -- ��ǰҪ��ѯ�ļ��ľ��
// 	OUT lpClass -- ��������
// 	OUT lpcbClass -- ��������
// 	OUT lpReserved -- ����
// 	OUT lpcSubKeys -- �Ӽ�����
// 	OUT lpcbMaxSubKeyLen -- �Ӽ����Ƶ���󳤶�
// 	OUT lpcbMaxClassLen -- �Ӽ���������󳤶�
// 	OUT lpcValues -- ��ֵ�ĸ���
// 	OUT lpcbMaxValueNameLen -- ����ֵ�����Ƶĳ���
// 	OUT lpcbMaxValueLen -- ����ֵ���ݵĳ���
// 	OUT lpcbSecurityDescriptor -- ��ȫ����
// 	OUT lpftLastWriteTime -- ���һ��д��ʱ��
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д�
// ������������ѯ������Ϣ��
// ����: 
// **************************************************
LONG WINAPI FileReg_QueryInfoKey( 
					  HKEY hKey, 
					  LPTSTR lpClass, 
					  LPDWORD lpcbClass, 
					  LPDWORD lpReserved, 
					  LPDWORD lpcSubKeys, 
					  LPDWORD lpcbMaxSubKeyLen, 
					  LPDWORD lpcbMaxClassLen, 
					  LPDWORD lpcValues, 
					  LPDWORD lpcbMaxValueNameLen, 
					  LPDWORD lpcbMaxValueLen, 
					  LPDWORD lpcbSecurityDescriptor, 
					  PFILETIME lpftLastWriteTime 
					  )
{
	LPREGKEY hRegKey;

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key
		if (hRegKey==NULL)  
			return ERROR_BADKEY;  // û��ָ���ļ�

		if (lpClass!=NULL&&lpcbClass==NULL)
			return ERROR_INVALID_PARAMETER;  // ��������
		if (lpcbClass!=NULL)
		{
			if (lpClass!=NULL)
			{  // need class data
				if (*lpcbClass<(strlen(hRegKey->lpClass)+1))
				{  // the class buffer is not enough
					*lpcbClass=strlen(hRegKey->lpClass);
					return ERROR_MORE_DATA;
				}
				strcpy(lpClass,hRegKey->lpClass);  // get class data
				*lpcbClass=strlen(hRegKey->lpClass);  // get class size
			}
			else
			{// Only Need the class size
				*lpcbClass=strlen(hRegKey->lpClass);
			}
		}
		QueryInfoSubKey(hRegKey,lpcSubKeys,lpcbMaxSubKeyLen,lpcbMaxClassLen);  // query sub key infomation
		QueryInfoValue(hRegKey,lpcValues,lpcbMaxValueNameLen,lpcbMaxValueLen); // query value infomation
		return ERROR_SUCCESS; // ���سɹ�
}

// **************************************************
// ������LONG WINAPI FileReg_QueryValueEx( HKEY hKey, LPCTSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
// ������
// 	IN hKey -- ָ���ļ�
// 	IN lpValueName -- ��ֵ
// 	OUT lpReserved -- ����
// 	OUT lpType -- ������������
// 	OUT lpData -- ��������
// 	OUT lpcbData -- �������ݵĳ���
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д�
// �����������õ�ָ����ֵ�����ݡ�
// ����: 
// **************************************************
LONG WINAPI FileReg_QueryValueEx( 
					  HKEY hKey, 
					  LPCTSTR lpValueName, 
					  LPDWORD lpReserved, 
					  LPDWORD lpType, 
					  LPBYTE lpData, 
					  LPDWORD lpcbData 
					  )
{
	LPREGKEY hRegKey;
	LPREGVALUE hRegValue;

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key
		if (hRegKey==NULL)
			return ERROR_BADKEY;
		if (IsOpenKey(hRegKey)==FALSE)  // the key is or not open ?
			return ERROR_BADKEY;

		hRegValue=SearchValue(hRegKey,lpValueName);  // Search the Value have exist in the key 
		if (hRegValue==NULL)
		{  // û������
			return ERROR_BADKEY;	
		}
		return GetValue(hRegValue,lpType,lpData,lpcbData);  // Get Data from the value
}

// **************************************************
// ������LONG WINAPI FileReg_SetValueEx( HKEY hKey, LPCTSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData )
// ������
// 	IN hKey -- �򿪵ļ��ľ��
// 	IN lpValueName -- ��ֵ
// 	IN Reserved -- ����
// 	IN dwType -- ��������
// 	IN lpData -- ����ָ��
// 	IN cbData -- ���ݳ���
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д�
// �������������ü�ֵ�����ݡ�
// ����: 
// **************************************************
LONG WINAPI FileReg_SetValueEx( 
					HKEY hKey, 
					LPCTSTR lpValueName, 
					DWORD Reserved, 
					DWORD dwType, 
					const BYTE *lpData, 
					DWORD cbData 
					)
{
	LPREGKEY hRegKey;
	LPREGVALUE hRegValue;

		hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
									// then return the key
		if (hRegKey==NULL)
			return ERROR_BADKEY;  // û���ҵ�ָ���ļ�
		if (IsOpenKey(hRegKey)==FALSE)  // the key is or not open ?
			return ERROR_BADKEY;

		hRegValue=SearchValue(hRegKey,lpValueName);  // Search the Value have exist in the key 
		if (hRegValue==NULL)
		{
			// the valuename is not exist in the key
			hRegValue=CreateNewValue(hRegKey,lpValueName);  // Create New value in the key
			if (hRegValue==NULL)
				return ERROR_NO_LOG_SPACE; // ������ֵʧ��
		}
		if (SetValue(hRegValue,dwType,lpData,cbData)==FALSE)  // Set New Data to the value
		{
			return ERROR_NO_LOG_SPACE;
		}

		return ERROR_SUCCESS;  // ���سɹ�
}




/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
// **************************************************
// ������static LPREGKEY MapRootKey(HKEY hKey)
// ������
// 	IN hKey 
// 
// ����ֵ������ָ����ֵ��ע����ṹָ��
// ����������ӳ�������
// ����: 
// **************************************************
static LPREGKEY MapRootKey(HKEY hKey)
{
#ifdef ROOT_MICROSOFT

		if (hKey==HKEY_CLASSES_ROOT)
		{  // ָ������ΪHKEY_CLASSES_ROOT
			if (hkey_classes_root==NULL)
			{  // ��û�в�������
				hkey_classes_root=(LPREGKEY)malloc(sizeof(REGKEY));  // �����ڴ�
				if (hkey_classes_root)
				{  // ����HKEY_CLASSES_ROOT����
					hkey_classes_root->bOpen=FALSE;
					hkey_classes_root->lpClass=AssignRegValue("HKEY_CLASSES_ROOT");
					hkey_classes_root->hNextKey=NULL;
					hkey_classes_root->hRegValue=NULL;
					hkey_classes_root->hSubKey=NULL;
					hkey_classes_root->lpKeyName=AssignRegValue("HKEY_CLASSES_ROOT");
				}
			}
			return hkey_classes_root;  // ����HKEY_CLASSES_ROOT�Ľṹ
		}
		if (hKey==HKEY_CURRENT_USER)
		{ // ָ������ΪHKEY_CURRENT_USER
			if (hkey_current_user==NULL)
			{  // ��û�в�������
				hkey_current_user=(LPREGKEY)malloc(sizeof(REGKEY));  // ����ռ�
				if (hkey_current_user)
				{  // ����HKEY_CURRENT_USER����
					hkey_current_user->bOpen=FALSE;
					hkey_current_user->lpClass=AssignRegValue("HKEY_CURRENT_USER");
					hkey_current_user->hNextKey=NULL;
					hkey_current_user->hRegValue=NULL;
					hkey_current_user->hSubKey=NULL;
					hkey_current_user->lpKeyName=AssignRegValue("HKEY_CURRENT_USER");
				}
			}
			return hkey_current_user;  // ����HKEY_CURRENT_USER�ṹ
		}
		if (hKey==HKEY_LOCAL_MACHINE)
		{  // ָ������ΪHKEY_LOCAL_MACHINE
			if (hkey_local_machine==NULL)
			{ // û�в���HKEY_LOCAL_MACHINE����
				hkey_local_machine=(LPREGKEY)malloc(sizeof(REGKEY));  // �����ڴ�
				if (hkey_local_machine)
				{  // ����HKEY_LOCAL_MACHINE����
					hkey_local_machine->bOpen=FALSE;
					hkey_local_machine->lpClass=AssignRegValue("HKEY_LOCAL_MACHINE");
					hkey_local_machine->hNextKey=NULL;
					hkey_local_machine->hRegValue=NULL;
					hkey_local_machine->hSubKey=NULL;
					hkey_local_machine->lpKeyName=AssignRegValue("HKEY_LOCAL_MACHINE");
				}
			}
			return hkey_local_machine;  // ����HKEY_LOCAL_MACHINE�Ľṹ
		}
		if (hKey==HKEY_USERS)
		{ // ָ������ΪHKEY_USERS
			if (hkey_users==NULL)
			{  // û�в���HKEY_USERS
				hkey_users=(LPREGKEY)malloc(sizeof(REGKEY));// �����ڴ�
				if (hkey_users)
				{  // ����HKEY_USERS �ṹ
					hkey_users->bOpen=FALSE;
					hkey_users->lpClass=AssignRegValue("HKEY_USERS");
					hkey_users->hNextKey=NULL;
					hkey_users->hRegValue=NULL;
					hkey_users->hSubKey=NULL;
					hkey_users->lpKeyName=AssignRegValue("HKEY_USERS");
				}
			}
			return hkey_users;  // ����HKEY_USERS�ṹ
		}
#else
// !!! Modified By Jami chen in 2003.06.27
/*		if (hKey==HKEY_ROOT)
		{
			if (hkey_root==NULL)
			{
				hkey_root=(LPREGKEY)malloc(sizeof(REGKEY));
				if (hkey_root)
				{
					hkey_root->bOpen=FALSE;
					hkey_root->lpClass=AssignRegValue("HKEY_ROOT");
					hkey_root->hNextKey=NULL;
					hkey_root->hRegValue=NULL;
					hkey_root->hSubKey=NULL;
					hkey_root->lpKeyName=AssignRegValue("HKEY_ROOT");
				}
			}
			return hkey_root;
		}
*/
// !!!
		if (hKey==HKEY_HARDWARE_ROOT)
		{  // ָ������ΪHKEY_HARDWARE_ROOT
			if (hkey_hardware_root==NULL)
			{  // ����HKEY_HARDWARE_ROOTû�в���
				hkey_hardware_root=(LPREGKEY)malloc(sizeof(REGKEY));  // ����ռ�
				if (hkey_hardware_root)
				{  // ����HKEY_HARDWARE_ROOT�ṹ
					hkey_hardware_root->bOpen=FALSE;
					hkey_hardware_root->lpClass=AssignRegValue("HKEY_HARDWARE_ROOT");
					hkey_hardware_root->hNextKey=NULL;
					hkey_hardware_root->hRegValue=NULL;
					hkey_hardware_root->hSubKey=NULL;
					hkey_hardware_root->lpKeyName=AssignRegValue("HKEY_HARDWARE_ROOT");
				}
			}
			return hkey_hardware_root;  // ����HKEY_HARDWARE_ROOT�ṹ
		}
		if (hKey==HKEY_SOFTWARE_ROOT)
		{ // ָ������ΪHKEY_SOFTWARE_ROOT
			if (hkey_software_root==NULL)
			{  // û�в���HKEY_SOFTWARE_ROOT
				hkey_software_root=(LPREGKEY)malloc(sizeof(REGKEY));  // �����ڴ�
				if (hkey_software_root)
				{  // ����HKEY_SOFTWARE_ROOT�ṹ
					hkey_software_root->bOpen=FALSE;
					hkey_software_root->lpClass=AssignRegValue("HKEY_SOFTWARE_ROOT");
					hkey_software_root->hNextKey=NULL;
					hkey_software_root->hRegValue=NULL;
					hkey_software_root->hSubKey=NULL;
					hkey_software_root->lpKeyName=AssignRegValue("HKEY_SOFTWARE_ROOT");
				}
			}
			return hkey_software_root;  // ����HKEY_SOFTWARE_ROOT�ṹ
		}
// !!!		

#endif

		return (LPREGKEY)hKey; //ֱ�ӷ��ش򿪵ļ��Ľṹ
}

/***********************************************************************/
// Search the Sub Key in the RegKey
/***********************************************************************/
// **************************************************
// ������static LPREGKEY SearchSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey)
// ������
// 	IN hRegKey -- ��ǰָ���ļ�
// 	IN lpSubKey -- Ҫ���ҵ��Ӽ�����
// 
// ����ֵ�����ز��ҵ���ע����ṹ��NULL��ʾû���ҵ�
// ������������ָ���ļ��²���ָ�����Ӽ�
// ����: 
// **************************************************
static LPREGKEY SearchSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey)
{
	LPREGKEY hSubRegKey;

	hSubRegKey=hRegKey->hSubKey;  // �õ���һ���Ӽ��Ľṹ
	while(hSubRegKey)
	{
		if (MatchSubKey(hSubRegKey,lpSubKey)==TRUE)  // Match the Current SubKey and the Search Sub Key
			return hSubRegKey;  // Is Match
		hSubRegKey=hSubRegKey->hNextKey;  // to the next Sub Key
	}
	return NULL;  // the Search Sub Key is Not Exist in the RegKey
}


/***********************************************************************/
// Search the Sub Key in the RegKey By Index
/***********************************************************************/
// **************************************************
// ������static LPREGKEY SearchSubKeyByIndex(LPREGKEY hRegKey,DWORD iIndex)
// ������
// 	IN hRegKey -- ��ǰָ���ļ�
// 	IN iIndex -- ָ��������
// 
// ����ֵ�������ҵ��ļ��Ľṹ
// ��������������ָ���������Ӽ���
// ����: 
// **************************************************
static LPREGKEY SearchSubKeyByIndex(LPREGKEY hRegKey,DWORD iIndex)
{
	LPREGKEY hSubRegKey;
	DWORD iCurIndex=0;

	hSubRegKey=hRegKey->hSubKey;
	while(hSubRegKey)
	{
		if (iCurIndex==iIndex)  // Match the Current SubKey and the Search Sub Key
			return hSubRegKey;  // Is Match
		hSubRegKey=hSubRegKey->hNextKey;  // to the next Sub Key
		iCurIndex++;
	}
	return NULL;  // the Search Sub Key is Not Exist in the RegKey
}

/***********************************************************************/
// Search the Sub Key in the RegKey
/***********************************************************************/
// **************************************************
// ������static BOOL MatchSubKey(LPREGKEY hSubRegKey,LPCTSTR lpSubKey)
// ������
// 	IN hSubRegKey -- ָ����ע����ṹ
// 	IN lpSubKey -- ָ���ļ���
// 
// ����ֵ���������ƥ�䷵��TRUE�����򷵻�FALSE
// ����������ָ���ļ�������Ƿ�ƥ�䡣
// ����: 
// **************************************************
static BOOL MatchSubKey(LPREGKEY hSubRegKey,LPCTSTR lpSubKey)
{
	DWORD dwLen;
		dwLen=GetSubKeyLen(lpSubKey);  // �õ������ĳ���
		if (dwLen==strlen(hSubRegKey->lpKeyName))
		{ // ��ָ���ļ� �ļ����Ƿ񳤶���ͬ
			if (strnicmp(lpSubKey,(LPCTSTR)hSubRegKey->lpKeyName,dwLen)==0) // �Ƚ��ַ����Ƿ���ͬ
				return TRUE; // ����ƥ��
		}
		return FALSE;  // ��ƥ��
}

/***********************************************************************/
// Get the Sub Key Name Length in the Sub Key String 
/***********************************************************************/
// **************************************************
// ������static DWORD GetSubKeyLen(LPCTSTR lpSubKey)
// ������
// 	IN lpSubKey ָ���ļ���
// 
// ����ֵ�����ص�ǰ�����ĳ���
// �����������õ���ǰ�����ĳ���
// ����: 
// **************************************************
static DWORD GetSubKeyLen(LPCTSTR lpSubKey)
{
	DWORD dwlen=0;
	while(*lpSubKey)  // the character is or not terminating NULL character
	{
		if (*lpSubKey=='\\')  // the character is sub key separate character
			break;  // ���µ������Ӽ�������
		dwlen++;
		lpSubKey++;  // to the next character
	}

	return dwlen;  // return the sub key length
}

/***********************************************************************/
// Create a Sub Key (lpSubKey) in the lpRegKey Key 
/***********************************************************************/
// **************************************************
// ������static LPREGKEY CreateNewSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey)
// ������
// 	IN hRegKey -- ָ���ļ��ṹ
// 	IN lpSubKey -- Ҫ�������Ӽ�������
// 
// ����ֵ�����ش������Ӽ��Ľṹ
// ������������ָ���ļ��´���һ����ָ�������������Ӽ���
// ����: 
// **************************************************
static LPREGKEY CreateNewSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey)
{
	LPREGKEY hSubKey,hTailSubKey;

			hSubKey=(LPREGKEY)malloc(sizeof(REGKEY));  // Create New Sub Key
			if (hSubKey)
			{                  // Create Sub Key Success
				// ��ʼ�����Ľṹ
				hSubKey->bOpen=FALSE;
				hSubKey->lpClass=NULL;
				hSubKey->hNextKey=NULL;
				hSubKey->hRegValue=NULL;
				hSubKey->hSubKey=NULL;
				hSubKey->lpKeyName=AssignSubKeyname(lpSubKey);  // Copy the Sub Key Name
				if (hSubKey->lpKeyName==NULL)
				{  // ���Ƽ���ʧ��
					free(hSubKey);
					return NULL;
				}

				// Put the Sub Key to the tail
				if (hRegKey->hSubKey==NULL)
					hRegKey->hSubKey=hSubKey;  // ��û���Ӽ����嵽��һ��
				else
				{
					hTailSubKey=GetTailSubKey(hRegKey->hSubKey);  // �õ����һ���Ӽ��Ľṹ
					if (hTailSubKey)  // if the hTailSubKey ==NULL, then must have error in GetTailSubKey
						hTailSubKey->hNextKey=hSubKey;  // �嵽���һ���Ӽ��ĺ���
				}
			}
			return hSubKey;  // ���ش������Ӽ��ṹ
}


/***********************************************************************/
// go to the next sub Key 
/***********************************************************************/
// **************************************************
// ������static LPTSTR ToNextSubKey(LPCTSTR lpSubKey)
// ������
// 	IN lpSubKey -- ָ����ע����ִ�
// 
// ����ֵ�������ִ�����һ���Ӽ���λ��
// �����������õ���������һ���Ӽ���λ�á�
// ����: 
// **************************************************
static LPTSTR ToNextSubKey(LPCTSTR lpSubKey)
{
	LPTSTR lpCurrent;

		lpCurrent=(LPTSTR)lpSubKey;  // �õ���ǰλ��
		while(*lpCurrent)
		{
			if (*lpCurrent=='\\')
			{  // �õ�ע����ķָ���
				lpCurrent++; // ���ָ�������һ��λ��
				break;
			}
			lpCurrent++;  // ��һ���ַ�
		}
		return lpCurrent;  // ������һ���Ӽ���λ��
}

/***********************************************************************/
// alloc the Sub Key Name Space, and Copy the Sub Key Name in
/***********************************************************************/
// **************************************************
// ������static LPTSTR AssignSubKeyname(LPCTSTR lpSubKey)
// ������
// 	IN lpSubKey ָ�����Ӽ�����
// 
// ����ֵ�� �����·���ļ���
// �������������䲢���Ƶ�ǰ�ļ���
// ����: 
// **************************************************
static LPTSTR AssignSubKeyname(LPCTSTR lpSubKey)
{
	DWORD dwLen;
	LPTSTR lpSubKeyName;

		dwLen=GetSubKeyLen(lpSubKey);  // get the len of the sub key name
		lpSubKeyName=(LPTSTR)malloc(dwLen+1);  // malloc a space include null-terminated
		if (lpSubKeyName!=NULL)
		{
			strncpy(lpSubKeyName,(LPCTSTR)lpSubKey,dwLen);  // copy the sub key name 
			lpSubKeyName[dwLen]=0;
		}
		return lpSubKeyName;  // return the new key address
}

/***********************************************************************/
// alloc the Sub Key Name Space, and Copy the Sub Key Name in
/***********************************************************************/
// **************************************************
// ������static LPREGKEY GetTailSubKey(LPREGKEY hSubKey)
// ������
// 	IN hSubKey -- ��ǰ���Ӽ��Ľṹ
// 
// ����ֵ���������һ���Ӽ��Ľṹ
// �����������õ����һ���Ӽ���
// ����: 
// **************************************************
static LPREGKEY GetTailSubKey(LPREGKEY hSubKey)
{
	while(1)
	{
		if (hSubKey==NULL)  // ��ǰ�Ӽ�Ϊ��
			return NULL;
		if (hSubKey->hNextKey==NULL)  // if the next key is NULL, then the Key is Tail
			return hSubKey;
		hSubKey=hSubKey->hNextKey;  // ����һ���Ӽ�
	}
}


/***********************************************************************/
// Set Open Status to the key
/***********************************************************************/
// **************************************************
// ������static BOOL OpenCurrentRegKey(LPREGKEY hRegKey)
// ������
// 	IN hRegKey -- ��ǰҪ�򿪵�ע����Ľṹ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ָ���ļ���
// ����: 
// **************************************************
static BOOL OpenCurrentRegKey(LPREGKEY hRegKey)
{
	if (hRegKey->bOpen==TRUE)
		return FALSE;   // Current Key Had Open , Can't Open again
	hRegKey->bOpen=TRUE;  // ���ô򿪵ı�־
	return TRUE;
}


/***********************************************************************/
// Will Delete Current key and it's Sub Key
/***********************************************************************/
// **************************************************
// ������static BOOL DeleteSubRegKey(LPREGKEY hRegKey,LPREGKEY hSubKey)
// ������
// 	IN hRegKey -- ��ǰָ���ļ�
// 	IN hSubKey -- Ҫɾ�����Ӽ�
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������ɾ��ָ�����Ӽ�
// ����: 
// **************************************************
static BOOL DeleteSubRegKey(LPREGKEY hRegKey,LPREGKEY hSubKey)
{
	LPREGKEY hSubRegKey;

	hSubRegKey=hRegKey->hSubKey;
	if (hSubRegKey==hSubKey)  // the key to will be Delete is first Sub Key
	{
		hRegKey->hSubKey=hSubKey->hNextKey;  // The Next Key will to be Set first Sub Key
	}
	else
	{
		while(1)
		{
			if (hSubRegKey->hNextKey==hSubKey)  // Match the Current SubKey and the Search Sub Key
			{
				// Is Match
				hSubRegKey->hNextKey=hSubKey->hNextKey;  // will set the Next Key of the delete Key to the Next Key of current key
				break;
			}
			hSubRegKey=hSubRegKey->hNextKey;  // to the next Sub Key
			if (hSubKey==NULL)
				return FALSE;
		}
	}

	FreeRegKey(hSubKey);  // free memory of the delete key and it's sub key
	return TRUE;
}

/***********************************************************************/
// Will free memory of the delete key and it's sub key
/***********************************************************************/
// **************************************************
// ������static void	FreeRegKey(LPREGKEY hKey)
// ������
// 	IN hKey -- Ҫ�ͷ��ڴ�ļ�
// 
// ����ֵ����
// �����������ͷ�ָ����������
// ����: 
// **************************************************
static void	FreeRegKey(LPREGKEY hKey)
{
	LPREGKEY hSubKey,hSubNextKey;

	if (hKey==NULL)
		return;
	hSubKey=hKey->hSubKey;  // Get First Sub Key
	while(hSubKey)
	{
		hSubNextKey=hSubKey->hNextKey;  // Get Next Sub Key
		FreeRegKey(hSubKey);  // Free Current Sub Key
		hSubKey=hSubNextKey;  // Set Next Sub Key to Current Sub Key
	}
	if (hKey->lpKeyName)
	{
		free(hKey->lpKeyName);  // free key Name
	}

	free(hKey);  // free current key
}


/***********************************************************************/
// the key is or not open ?
/***********************************************************************/
// **************************************************
// ������static BOOL IsOpenKey(LPREGKEY hRegKey)
// ������
// 	IN hRegKey -- ָ���ļ�
// 
// ����ֵ��������Ѿ��򿪣��򷵻�TRUE�����򷵻�FALSE��
// �����������жϼ��Ƿ��Ѿ��򿪡�
// ����: 
// **************************************************
static BOOL IsOpenKey(LPREGKEY hRegKey)
{
	return hRegKey->bOpen; // ���ص�ǰ���Ĵ�״̬
}


/***********************************************************************/
// Search the Value have exist in the key 
/***********************************************************************/
// **************************************************
// ������static LPREGVALUE SearchValue(LPREGKEY hRegKey,LPCTSTR lpValueName)
// ������
// 	IN hRegKey -- ָ���ļ�
//	 IN lpValueName -- ��ֵ����
// 
// ����ֵ�������ҵ��ļ�ֵ�Ľṹ
// ������������ָ���ļ��в���ָ���ļ�ֵ��
// ����: 
// **************************************************
static LPREGVALUE SearchValue(LPREGKEY hRegKey,LPCTSTR lpValueName)
{
	LPREGVALUE hRegValue;


		hRegValue=hRegKey->hRegValue;  // get first Value of the key
		while(hRegValue)  // have next value
		{
			if (stricmp(hRegValue->lpValueName,lpValueName)==0)  // complare the current value and the search value 
				break;  // is Match ,then break
			hRegValue=hRegValue->hNextValue;  // to next value
		}
		return hRegValue;  // return the value 
}

/***********************************************************************/
// Search the Value have exist in the key 
/***********************************************************************/
// **************************************************
// ������static LPREGVALUE SearchValueByIndex(LPREGKEY hRegKey, DWORD dwIndex)
// ������
// 	IN hRegKey -- ָ���ļ�
// 	IN dwIndex -- ָ��������
// 
// ����ֵ�������ҵ��ļ�ֵ�Ľṹ
// ������������ָ���ļ��в���ָ�������ļ�ֵ��
// ����: 
// **************************************************
static LPREGVALUE SearchValueByIndex(LPREGKEY hRegKey, DWORD dwIndex)
{
	LPREGVALUE hRegValue;
	DWORD dwCurIndex=0;


		hRegValue=hRegKey->hRegValue;  // get first Value of the key
		while(hRegValue)  // have next value
		{
			if (dwCurIndex==dwIndex)  // complare the current value and the search value 
				break;  // is Match ,then break
			hRegValue=hRegValue->hNextValue;  // to next value
			dwCurIndex++;
		}
		return hRegValue;  // return the value 
}
/***********************************************************************/
// Create New value in the key
/***********************************************************************/
// **************************************************
// ������static LPREGVALUE CreateNewValue(LPREGKEY hRegKey,LPCTSTR lpValueName)  
// ������
// 	IN hRegKey -- ָ���ļ�
// 	IN lpValueName -- ָ���ļ�ֵ����
// 
// ����ֵ���ɹ����ش����ļ�ֵ�ṹ�����򷵻�NULL
// ��������������һ��ָ����ֵ���Ƶļ�ֵ
// ����: 
// **************************************************
static LPREGVALUE CreateNewValue(LPREGKEY hRegKey,LPCTSTR lpValueName)  
{
	LPREGVALUE hRegValue,hTailValue;

		hRegValue=(LPREGVALUE)malloc(sizeof(REGVALUE));  // alloc a memory for register value
		if (hRegValue==NULL)
			return NULL;  // ����ʧ��
		hRegValue->lpData=NULL;  // Initialize the reg value
		hRegValue->cbData=0;
		hRegValue->dwType=REG_NONE;
		hRegValue->hNextValue=NULL;
		hRegValue->lpValueName=AssignRegValue(lpValueName);  // Set reg Value name to the value
		if (hRegValue->lpValueName==NULL)
		{ // ���Ƽ�ֵ����ʧ��
			free(hRegValue);
			return NULL;
		}

		// Put the Value to the tail
		if (hRegKey->hRegValue==NULL)
			hRegKey->hRegValue=hRegValue;  // ���Ǹü��µĵ�һ����ֵ
		else
		{
			hTailValue=GetTailValue(hRegKey->hRegValue);  // Get the tail value
			if (hTailValue)  // if the hTailValue ==NULL, then must have error in GetTailValue
				hTailValue->hNextValue=hRegValue;  // ���´����ļ�ֵ�ӵ����һ����ֵ����
		}
		return hRegValue;  // ���ش����ļ�ֵ
}


/***********************************************************************/
// alloc a memory ,and copy the Value name 
/***********************************************************************/
// **************************************************
// ������static LPTSTR AssignRegValue(LPCTSTR lpValueName)
// ������
// 	IN lpValueName -- ָ���ļ�ֵ����
// 
// ����ֵ�������·���ļ�ֵ����
// �������������䲢���Ƶ�ǰ��ֵ���ơ�
// ����: 
// **************************************************
static LPTSTR AssignRegValue(LPCTSTR lpValueName)
{
	DWORD dwLen;
	LPTSTR lpNewValueName;

		dwLen=strlen(lpValueName);  // get the len of the sub key name
		lpNewValueName=(LPTSTR)malloc(dwLen+1);  // malloc a space include null-terminated
		if (lpNewValueName!=NULL)
		{  // ����ɹ�
			strcpy(lpNewValueName,(LPCTSTR)lpValueName);  // copy the value name 
		}
		return lpNewValueName;  // return the new value name
}


/***********************************************************************/
// alloc a memory ,and copy the Value name 
/***********************************************************************/
// **************************************************
// ������static LPREGVALUE GetTailValue(LPREGVALUE hRegValue)
// ������
// 	IN hRegValue -- ��ǰ�ļ�ֵ
// 
// ����ֵ���������һ����ֵ�Ľṹ
// ���������� �õ����һ����ֵ��
// ����: 
// **************************************************
static LPREGVALUE GetTailValue(LPREGVALUE hRegValue)
{
	while(1)
	{
		if (hRegValue==NULL) // ��ǰ�ļ�ֵΪNULL
			return NULL; // ������Ч
		if (hRegValue->hNextValue==NULL)  // the next value is NULL, then is tail value
			return hRegValue;
		hRegValue=hRegValue->hNextValue;  // to the next value
	}
}


/***********************************************************************/
// Set New Data to the current value
/***********************************************************************/
// **************************************************
// ������static BOOL SetValue(LPREGVALUE hRegValue,DWORD dwType,const BYTE *lpData,DWORD cbData)
// ������
// 	IN hRegValue -- ��ǰ�ļ�ֵ 
// 	IN dwType -- ���ݵ�����
// 	IN lpData -- ���ݵ�ָ��
// 	IN cbData -- ���ݵĴ�С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������ָ����ֵ�����ݡ�
// ����: 
// **************************************************
static BOOL SetValue(LPREGVALUE hRegValue,DWORD dwType,const BYTE *lpData,DWORD cbData)
{
		if (dwType==REG_SZ)
		{
			cbData = strlen(lpData) + 1;  // ��Ҫ��β��
			hRegValue->lpData=(LPBYTE)malloc(cbData);  // alloc memory to save data
			if (hRegValue->lpData==NULL)
				return FALSE; // �������ݻ���ʧ��
		}
		else
		{
			hRegValue->lpData=(LPBYTE)malloc(cbData);  // alloc memory to save data
			if (hRegValue->lpData==NULL)
				return FALSE; // �������ݻ���ʧ��
		}
		hRegValue->dwType=dwType;  // save the data type
		hRegValue->cbData=cbData;  // save the data size
		memcpy((void *)hRegValue->lpData,(const void *)lpData,cbData);  // save data
		return TRUE;  // ���óɹ�
}


/***********************************************************************/
// Get Data from the value
/***********************************************************************/
// **************************************************
// ������static LONG GetValue(LPREGVALUE hRegValue,LPDWORD lpType,const BYTE *lpData,LPDWORD lpcbData)
// ������
// 	IN hRegValue -- ָ���ļ�ֵ
// 	OUT lpType -- ������������
// 	OUT lpData -- ���ݻ���
// 	IN/OUT lpcbData -- ���ݻ���Ĵ�С�����������ݵĴ�С
// 
// ����ֵ���ɹ�����ERROR_SUCCESS�������д�
// �����������õ�ָ����ֵ�����ݡ�
// ����: 
// **************************************************
static LONG GetValue(LPREGVALUE hRegValue,LPDWORD lpType,const BYTE *lpData,LPDWORD lpcbData)
{
		if (lpData!=NULL && lpcbData==NULL)  // if the lpdata is not NULL,then the lpcbData can't is 0
			return ERROR_INVALID_PARAMETER;
		if (lpData!=NULL)
		{                  // need read data
			if (hRegValue->dwType==REG_SZ)
			{  // �ִ�����
				if (hRegValue->cbData>=*lpcbData)  // the buffer is enough ? ,*lpcbData must include Terminate-NULL
					return ERROR_MORE_DATA;  // the buffer is not enough
				memcpy((void *)lpData,(const void *)hRegValue->lpData,(hRegValue->cbData+1));  // read data to buffer from value,include Terminate
				*lpcbData=hRegValue->cbData;  // read the data size
			}
			else
			{  // ��������
				if (hRegValue->cbData>*lpcbData)  // the buffer is enough ?
					return ERROR_MORE_DATA;  // the buffer is not enough
				memcpy((void *)lpData,(const void *)hRegValue->lpData,hRegValue->cbData);  // read data to buffer from value
				*lpcbData=hRegValue->cbData;  // read the data size
			}
		}
		if (lpType!=NULL)
			*lpType=hRegValue->dwType;  // read the data type
		return ERROR_SUCCESS;
}



/***********************************************************************/
// Delete current value
/***********************************************************************/
// **************************************************
// ������static LONG DeleteValue(LPREGKEY hRegKey,LPREGVALUE hRegValue)
// ������
// 	IN hRegKey -- ָ���ļ�
// 	IN hRegValue -- Ҫɾ���ļ�ֵ
// 
// ����ֵ���ɹ�����ERROR_SUCCESS �������д�
// ����������ɾ��ָ���ļ�ֵ��
// ����: 
// **************************************************
static LONG DeleteValue(LPREGKEY hRegKey,LPREGVALUE hRegValue)
{
//	LPREGKEY hSubRegKey;
	LPREGVALUE hCurRegValue;//,hNextRegValue;

	hCurRegValue=hRegKey->hRegValue;
	if (hCurRegValue==hRegValue)  // the key to will be Delete is first value
	{
		hRegKey->hRegValue=hCurRegValue->hNextValue;  // The Next value will to be Set first value
	}
	else
	{
		while(1)
		{
			if (hCurRegValue->hNextValue==hRegValue)  // Match the Current value and the Search value
			{
				// Is Match
				hCurRegValue->hNextValue=hRegValue->hNextValue;  // will set the Next Value of the delete Value to the Next Value of current Value
				break;
			}
			hCurRegValue=hCurRegValue->hNextValue;  // to the next value
			if (hCurRegValue==NULL)
				return ERROR_KEY_DELETED;  // ָ���ļ�ֵ������
		}
	}

	FreeRegValue(hRegValue);  // free memory of the delete value 
	return ERROR_SUCCESS;
}


/***********************************************************************/
// free current value
/***********************************************************************/
// **************************************************
// ������static void	FreeRegValue(LPREGVALUE hRegValue)
// ������
// 	IN hRegValue -- ָ���ļ�ֵ
// 
// ����ֵ����
// �����������ͷ�ָ���ļ�ֵ��
// ����: 
// **************************************************
static void	FreeRegValue(LPREGVALUE hRegValue)
{
	if (hRegValue->lpValueName)
	{
		free(hRegValue->lpValueName);  // free Value Name
	}

	free(hRegValue);  // free current reg value
}



/***********************************************************************/
// query sub key infomation
/***********************************************************************/
// **************************************************
// ������static void QueryInfoSubKey(LPREGKEY hRegKey,LPDWORD lpcSubKeys,LPDWORD lpcbMaxSubKeyLen,LPDWORD lpcbMaxClassLen)
// ������
// 	IN hRegKey -- ָ���ļ�
// 	OUT lpcSubKeys -- �Ӽ��ĸ���
// 	OUT lpcbMaxSubKeyLen -- �Ӽ��ļ�������󳤶�
// 	OUT lpcbMaxClassLen -- �Ӽ�����������󳤶�
// 
// ����ֵ����
// ������������ѯָ���ļ����Ӽ�����Ϣ��
// ����: 
// **************************************************
static void QueryInfoSubKey(LPREGKEY hRegKey,LPDWORD lpcSubKeys,LPDWORD lpcbMaxSubKeyLen,LPDWORD lpcbMaxClassLen)
{
	DWORD cSubKeys=0,cbMaxSubKeyLen=0,cbMaxClassLen=0;
	DWORD cbSubKeyLen=0,cbClassLen=0;
	LPREGKEY hSubKey;

		hSubKey=hRegKey->hSubKey;  // get first sub key
		while(hSubKey)
		{
			cSubKeys++;  // have a sub key
			cbSubKeyLen=strlen(hSubKey->lpKeyName);  // get the sub key name len
			if (cbSubKeyLen>cbMaxSubKeyLen)   // if the key name len > the max name len, then set max name len
				cbMaxSubKeyLen=cbSubKeyLen;

			if (hSubKey->lpClass==NULL)     // get sub key class len
				cbClassLen=0;
			else
				cbClassLen=strlen(hSubKey->lpClass);

			if (cbClassLen>cbMaxClassLen)  // if the key class len > Max class name,then set max class len
				cbMaxClassLen=cbClassLen;
			hSubKey=hSubKey->hNextKey;  // to next sub key
		}
		if (lpcSubKeys!=NULL)
			*lpcSubKeys=cSubKeys;  // need cSubKeys, then set cSubKeys
		if (lpcbMaxSubKeyLen!=NULL)
			*lpcbMaxSubKeyLen=cbMaxSubKeyLen;  // need cbMaxSubKeyLen, the set cbmaxSubKeyLen
		if (lpcbMaxClassLen!=NULL)
			*lpcbMaxClassLen=cbMaxClassLen;  // need cbmaxclassLen, then set cbmaxClassLen
}
/***********************************************************************/
// query value infomation
/***********************************************************************/
// **************************************************
// ������static void QueryInfoValue(LPREGKEY hRegKey,LPDWORD lpcValues,LPDWORD lpcbMaxValueNameLen,LPDWORD lpcbMaxValueLen)
// ������
// 	IN hRegKey -- ָ���ļ�
// 	OUT lpcValues -- ��ֵ����Ŀ
// 	OUT lpcbMaxValueNameLen -- ���ļ�ֵ�����Ƶĳ���
// 	OUT lpcbMaxValueLen -- �������ݳ���
// 
// ����ֵ����
// ������������ѯָ�����ļ�ֵ����Ϣ��
// ����: 
// **************************************************
static void QueryInfoValue(LPREGKEY hRegKey,LPDWORD lpcValues,LPDWORD lpcbMaxValueNameLen,LPDWORD lpcbMaxValueLen)
{
	DWORD cValues=0,cbMaxValueNameLen=0,cbMaxValueLen=0;
	DWORD cbValueNameLen=0,cbValueLen=0;
	LPREGVALUE hRegValue;

		hRegValue=hRegKey->hRegValue;  // get first value
		while(hRegValue)
		{
			cValues++;  // have a value
			cbValueNameLen=strlen(hRegValue->lpValueName);  // get the Value name len
			if (cbValueNameLen>cbMaxValueNameLen)   // if the value name len > the max name len, then set max name len
				cbMaxValueNameLen=cbValueNameLen;
			
			cbValueLen=hRegValue->cbData;
			if (cbValueLen>cbMaxValueLen)  // if the vlaue len > Max Value Len, then set new max Value Len
				cbMaxValueLen=cbValueLen;

			hRegValue=hRegValue->hNextValue;  // ����һ����ֵ
		}
		if (lpcValues!=NULL)
			*lpcValues=cValues;  // need cValues, then set cValues
		if (lpcbMaxValueNameLen!=NULL)
			*lpcbMaxValueNameLen=cbMaxValueNameLen;  // need cbMaxValueNameLen,then set cbmaxValueNameLen
		if (lpcbMaxValueLen!=NULL)
			*lpcbMaxValueLen=cbMaxValueLen;  // need cbMaxValueLen, then set cbMaxValueLen
}



/***********************************************************************/
// Load Register form system file
/***********************************************************************/
// the line type
#define TYPE_SUBKEY  0x0001
#define TYPE_VALUE   0x0002
#define TYPE_EMPTY   0x0003
#define TYPE_REMARK  0x0004
#define TYPE_UNKNOW  0x0005

#define TABCODE      0x09

#define MAXBINDATALEN  (4*1024)

static BOOL ReadLine(HANDLE handle,LPTSTR buffer,DWORD size);
static DWORD GetLineType(LPTSTR buffer);
static BOOL AnalyseSubKeyLine(LPTSTR buffer,PHKEY hRootKey,LPTSTR *lpSubKey);
static BOOL AnalyseValueLine(LPTSTR buffer,LPTSTR *lppValueName,LPDWORD lpdwValueType,LPTSTR *lppData,LPDWORD lpcbData);
static DWORD GetValueType(LPTSTR lpCurPos,LPDWORD lpdwValueType);
static BOOL GetValueData(LPTSTR lpCurPos,DWORD dwType,LPTSTR *lppData,LPDWORD lpcbData);

static HANDLE g_hReadFile;
static LPTSTR linebuffer;
static DWORD size=2048;

// **************************************************
// ������BOOL LoadRegister(LPTSTR lpRegisterFile)
// ������
// 	IN lpRegisterFile -- ע����ļ�
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������
// ����: 
// **************************************************
BOOL LoadRegister(LPTSTR lpRegisterFile)
{
	DWORD dwType;
	HKEY hKey=NULL;
	DWORD dwDisposition;
	
		// Open the register file
		g_hReadFile=CreateRegFile(lpRegisterFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (g_hReadFile==INVALID_HANDLE_VALUE)
			return FALSE;  // ���ļ�ʧ��
		linebuffer=(LPTSTR)malloc(size);  // ����ռ�
		if (linebuffer==NULL)
			return FALSE;  // the memory is not enough, must return 
		while(1)
		{
			if (ReadLine(g_hReadFile,linebuffer,size)==FALSE)  // ��ȡһ�е�����
			{
				RETAILMSG(1,("Read Line Failure \r\n"));
				break;
			}
//			RETAILMSG(1,("Read Line <%s> \r\n",linebuffer));
			dwType=GetLineType(linebuffer);  // �õ���ǰ�����ݵ�����
			if (dwType==TYPE_SUBKEY)
			{  // ���ڶ����Ӽ�
				HKEY hRootKey;
				LPTSTR lpSubKey=NULL;

					if (hKey)
					{ // ��ǰ�м�����
						FileReg_CloseKey(hKey);  // �ر�ע���
						hKey=NULL;
					}
				    
					if (AnalyseSubKeyLine(linebuffer,&hRootKey,&lpSubKey)==FALSE)  // �����õ��������ݵĸ������Ӽ�
						break;
					// will create this sub key,and open it
					FileReg_CreateKeyEx(hRootKey,lpSubKey,0,NULL,0,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition); // �����Ӽ�
					if (lpSubKey)  // �ͷ��Ӽ��Ļ���
						free(lpSubKey);
			}
			else if (dwType==TYPE_VALUE)
			{ // �����Ǽ�ֵ
				LPTSTR lpValueName,lpData;
				DWORD dwValueType,cbData;
					// �������ݣ��õ���ֵ���ƣ��������ͣ����ݼ����ݴ�С
					if (AnalyseValueLine(linebuffer,&lpValueName,&dwValueType,&lpData,&cbData)==FALSE)
						break;
					// will create this value in the open key
					// ���Ѿ��򿪵ļ��´�����ֵ
					FileReg_SetValueEx(hKey,lpValueName,0,dwValueType,(const BYTE *)lpData,cbData);
					if (lpValueName)  // �ͷż�ֵ������
						free(lpValueName);
					if (lpData)  // �ͷż�ֵ������
						free(lpData);
			}
		}
		if (hKey)
		{  // �ر��Ѿ��򿪵ļ�
			FileReg_CloseKey(hKey);
			hKey=NULL;
		}
		if (linebuffer)  // �ͷŴ�ȡ���ݵĻ���
			free(linebuffer);
		CloseRegHandle(g_hReadFile); // �ر��ļ����
		return TRUE;
}


/***********************************************************************/
// Get a line form system file
/***********************************************************************/
#define CACHEBUFFERSIZE  256
static TCHAR cachebuffer[CACHEBUFFERSIZE];
static DWORD dwPos=-1;
static DWORD dwbuffersize=CACHEBUFFERSIZE;

// **************************************************
// ������static BOOL ReadLine(HANDLE handle,LPTSTR buffer,DWORD size)
// ������
// 	IN handle -- �ļ����
// 	OUT buffer -- �洢���ݵĻ���
// 	IN size -- ����Ĵ�С
// 
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE
// �����������Ӵ򿪵��ļ��ж�ȡһ�����ݡ�
// ����: 
// **************************************************
static BOOL ReadLine(HANDLE handle,LPTSTR buffer,DWORD size)
{
	DWORD dwCurPos=0;

	while(1)
	{
		if (dwPos>=dwbuffersize)
		{ // the cache had tail,must read cache data
			// ��ǰ������ָ���Ѿ�����CACHE�Ľ�β����Ҫ���¶�ȡ����
			ReadRegFile(handle,cachebuffer,CACHEBUFFERSIZE,&dwbuffersize,NULL);  // ���ļ��ж�ȡCACHE��С�����ݵ�CACHE
			dwPos=0; // ��ǰ������ָ���0
			if (dwbuffersize==0)
			{// the file had eof���Ѿ�û������
				if (dwCurPos!=0)
				{ // �Ѿ���ȡ������
					buffer[dwCurPos]=0; // set buffer null 
					return TRUE;
				}
				else
				{ // û�ж�������
					buffer[dwCurPos]=0; // set buffer null 
					return FALSE;
				}
			}
		}
		if (cachebuffer[dwPos]==0x0d && cachebuffer[dwPos+1]==0x0a)
		{  // �ǻس���
			// Is the line over,may return
			buffer[dwCurPos]=0;  // set buffer null
			dwPos+=2; // to the next line start
			return TRUE;
		}
		buffer[dwCurPos++]=cachebuffer[dwPos++]; // read a byte to the buffer
		if (dwCurPos>=size)  // ���ݻ��治�ܷ��¸��е�����
			return FALSE;  // over flow the buffer
	}
}

/***********************************************************************/
// get line type,eg: sub key,value,empty and remark
/***********************************************************************/
// **************************************************
// ������static DWORD GetLineType(LPTSTR buffer)
// ������
// 	IN buffer -- ��ǰ�е����ݻ���
// 
// ����ֵ�����ص�ǰ�е���������
// �����������õ���ǰ�е���������
// ����: 
// **************************************************
static DWORD GetLineType(LPTSTR buffer)
{
	LPTSTR lpCurPos;

		lpCurPos=buffer;
		while(1)
		{  // jump the start space����ǰ��Ŀո�Ϊ��Ч�ַ�
			if (!(*lpCurPos==' ' || *lpCurPos==TABCODE))
				break;
			lpCurPos++;
		}
		switch(*lpCurPos)
		{
			case 0:
				return TYPE_EMPTY;  // this line is none
			case '[':
				return TYPE_SUBKEY; // this is a SUB KEY Start code
			case '\"':
			case '@':
				return TYPE_VALUE;  // this is a value start code
			case ';':
				return TYPE_REMARK; // this is remark start code
			default:
				return TYPE_UNKNOW; // this line is unknow line
		}
}


/***********************************************************************/
// Analyse the sub key line, apart rootkey and subkey
/***********************************************************************/
// **************************************************
// ������static BOOL AnalyseSubKeyLine(LPTSTR buffer,PHKEY lphRootKey,LPTSTR *lppSubKey)
// ������
// 	IN buffer -- ��ǰ������
// 	OUT lphRootKey -- ���ظ����ľ��
// 	OUT lppSubKey -- �����Ӽ�������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������һ��ע����������У���������������ļ�ֵ��������һ������
// ����: 
// **************************************************
static BOOL AnalyseSubKeyLine(LPTSTR buffer,PHKEY lphRootKey,LPTSTR *lppSubKey)
{
	LPTSTR lpCurPos;
	DWORD dwLen;
		
		if (lphRootKey==NULL || lppSubKey==NULL)
			return FALSE; // ��������

		lpCurPos=buffer;  // �õ���ǰ���ݵ���ʼָ��
		while(1)
		{  // jump the start space ������ǰ��Ŀո�
			if (!(*lpCurPos==' ' || *lpCurPos==TABCODE))
				break;
			if (*lpCurPos==0)
				return FALSE;
			lpCurPos++;
		}
		if (*lpCurPos!='[')  // the Sub key start code is '[',otherwise is wrong
			return FALSE;  // ��������

		// first apart the root key
		lpCurPos++; // jump '['
		dwLen=GetSubKeyLen(lpCurPos); // �õ����ĳ���
#ifdef ROOT_MICROSOFT
		if (strnicmp(lpCurPos,"HKEY_CLASSES_ROOT",dwLen)==0)
		{  // ������HKEY_CLASSES_ROOT
			*lphRootKey=HKEY_CLASSES_ROOT;
		}
		else if(strnicmp(lpCurPos,"HKEY_CURRENT_USER",dwLen)==0)
		{  // ������HKEY_CURRENT_USER
			*lphRootKey=HKEY_CURRENT_USER;
		}
		else if(strnicmp(lpCurPos,"HKEY_LOCAL_MACHINE",dwLen)==0)
		{  // ������HKEY_LOCAL_MACHINE
			*lphRootKey=HKEY_LOCAL_MACHINE;
		}
		else if(strnicmp(lpCurPos,"HKEY_USERS",dwLen)==0)
		{  // // ������HKEY_USERS
			*lphRootKey=HKEY_USERS;
		}
		else
		{  // unknow rootkey
			return FALSE;
		}
#else
// !!! Modified By Jami chen in 2003.06.27
/*		if (strnicmp(lpCurPos,"HKEY_ROOT",dwLen)==0)
		{
			*lphRootKey=HKEY_ROOT;
		}
*/
// !!!
		if (strnicmp(lpCurPos,"HKEY_HARDWARE_ROOT",dwLen)==0)
		{  // ������HKEY_HARDWARE_ROOT
			*lphRootKey=HKEY_HARDWARE_ROOT;
		}
		else if (strnicmp(lpCurPos,"HKEY_SOFTWARE_ROOT",dwLen)==0)
		{  // // ������HKEY_SOFTWARE_ROOT
			*lphRootKey=HKEY_SOFTWARE_ROOT;
		}
// !!!
		else
		{   // unknow rootkey
			return FALSE;
		}
#endif

		// next apart the sub key
		lpCurPos+=dwLen;  
		lpCurPos++;// skip the space mark '\'
		// Get the sub key length 
		dwLen=0;
		while(1)
		{
			if (*(lpCurPos+dwLen)==0)
				return FALSE;  // No exist the End code ,this is Invalid line
			if (*(lpCurPos+dwLen)==']')
				break;     // get the End code
			if (*(lpCurPos+dwLen)=='\\')
			{
				// Will Link the next line
			}
			dwLen++;
//			lpCurPos++;
		}
		if (lppSubKey==NULL)
			return FALSE;  // the parameter Invalid
		*lppSubKey=(LPTSTR)malloc(dwLen+1);
		if (*lppSubKey==NULL)
			return FALSE;  // the memory is not enough
		memcpy(*lppSubKey,lpCurPos,dwLen);
		(*lppSubKey)[dwLen]=0;  // add the terminate NULL
		return TRUE;
}

/***********************************************************************/
// Analyse the value line, apart value and data
/***********************************************************************/
// **************************************************
// ������static BOOL AnalyseValueLine(LPTSTR buffer,LPTSTR *lppValueName,LPDWORD lpdwValueType,LPTSTR *lppData,LPDWORD lpcbData)
// ������
// 	IN buffer -- ��ǰ������
// 	OUT lppValueName -- ���ؼ�ֵ������
// 	OUT lpdwValueType -- ������������
// 	OUT lppData -- ��������
// 	OUT lpcbData -- �������ݵĴ�С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������������ֵ���ݵ����ݡ�
// ����: 
// **************************************************
static BOOL AnalyseValueLine(LPTSTR buffer,LPTSTR *lppValueName,LPDWORD lpdwValueType,LPTSTR *lppData,LPDWORD lpcbData)
{
	LPTSTR lpCurPos;
	DWORD dwLen;

		// Parameter check
		if (lpdwValueType==NULL || lppValueName==NULL || lppData==NULL || lpcbData==NULL)  
			return FALSE;  // ��������

		//Initialize the parameter
		*lpdwValueType=REG_NONE;
		*lppValueName=NULL;
		*lppData=NULL;
		*lpcbData=0;  

		lpCurPos=buffer;
		while(1)
		{  // jump the start space ��������ǰ�����Ч����
			if (!(*lpCurPos==' ' || *lpCurPos==TABCODE))
				break;
			if (*lpCurPos==0)
				return FALSE;
			lpCurPos++;
		}
		if (*lpCurPos!='\"' && *lpCurPos!='@')  // the value Start Code is \" and @ ,otherwise is wrong
			return FALSE;  // ��������
		if (*lpCurPos=='@')  // It's default Value
		{
			*lppValueName=(LPTSTR)malloc(1);
			if (*lppValueName==NULL)
				return FALSE;  // the memory is not enough
			**lppValueName = 0;  // Only is empty string
			lpCurPos++;  // skip the @
		}
		else
		{  
			// get Value name len
			lpCurPos++;  // skip the \"
			dwLen=0;
			while(1)
			{
				if (*(lpCurPos+dwLen)==0)
					return FALSE;  // error line 
				if (*(lpCurPos+dwLen)=='\"')
					break;  // The End Code of the value name
				dwLen++;
			}
			// get value name data
			*lppValueName=(LPTSTR)malloc(dwLen+1);
			if (*lppValueName==NULL)
				return FALSE;  // the memory is not enough
			memcpy(*lppValueName,lpCurPos,dwLen);
			*(*lppValueName+dwLen) = 0;  // Add terminate NULL
			lpCurPos+=dwLen;  // skip the value name
			lpCurPos++;  // skip the \"
		}

		// Will get Value Type
		while(1)
		{  // jump the start space
			if (!(*lpCurPos==' ' || *lpCurPos==TABCODE))
				break;
			if (*lpCurPos==0)
				goto EXIT;
			lpCurPos++;
		}
		if (*lpCurPos!='=')
			return FALSE;  // the Next Char Must be '='
		lpCurPos++;  // skip the code "="
		while(1)
		{  // jump the start space
			if (!(*lpCurPos==' ' || *lpCurPos==TABCODE))
				break;
			if (*lpCurPos==0)
				goto EXIT;
			lpCurPos++;
		}
		// Get the value Type ,eg:dword,bin ...
		dwLen=GetValueType(lpCurPos,lpdwValueType);
		lpCurPos+=dwLen;
		
		// Will Get Data and data len
		if (GetValueData(lpCurPos,*lpdwValueType,lppData,lpcbData)==FALSE)
			goto EXIT;
		return TRUE;

EXIT:  // the Invalid Line or not enough memory,then must free the alloced memory, 
	   // then return FALSE
		if (*lppValueName)
			free(*lppValueName);
		return FALSE;
}

/***********************************************************************/
// get the value type of the value,eg dword ,bin or sz ...
/***********************************************************************/
typedef struct structValueType{
	LPTSTR lpTypeName;
	DWORD  dwTypenameLen;
	DWORD  dwType;
}VALUETYPE;
VALUETYPE ValueTypeTab[]={
	{"dword",5,REG_DWORD},
	{"hex",3,REG_BINARY},
	{NULL,0,REG_NONE}  // the end type
};
// **************************************************
// ������static DWORD GetValueType(LPTSTR lpCurPos,LPDWORD lpdwValueType)
// ������
// 	IN lpCurPos -- ��ǰ������
// 	OUT lpdwValueType -- ���ص�ǰ���ݵ���������
// 
// ����ֵ�������������͵ĳ���
// �����������õ���ǰ���� ���������͡�
// ����: 
// **************************************************
static DWORD GetValueType(LPTSTR lpCurPos,LPDWORD lpdwValueType)
{
	DWORD dwLen=0;
	int i=0;

		if (*lpCurPos=='\"')  // the value data is string
		{  // ���ַ�������
			*lpdwValueType=REG_SZ;
			return 0;
		}
		else
		{
			while(1)
			{
				if (ValueTypeTab[i].lpTypeName==NULL)
				{  // û��ƥ����������ͣ������ǲ�֧�ֵ���������
					*lpdwValueType=REG_NONE;
					return 0;
				}
				if (strnicmp(lpCurPos,ValueTypeTab[i].lpTypeName,ValueTypeTab[i].dwTypenameLen)==0)
				{ // �ҵ�ָ������������
					*lpdwValueType=ValueTypeTab[i].dwType;
					return ValueTypeTab[i].dwTypenameLen;  // �����������͵����ݳ���
				}
				i++;
			}
		}
}

/***********************************************************************/
// get the value data and data length
/***********************************************************************/
// **************************************************
// ������static BOOL GetValueData(LPTSTR lpCurPos,DWORD dwType,LPTSTR *lppData,LPDWORD lpcbData)
// ������
// 	IN lpCurPos -- ��ǰ������
// 	IN dwType -- ��ǰ����������
// 	OUT lppData -- ������ǰ���ݵĻ���
// 	OUT lpcbData -- ��ǰ���ݵĳ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ֵ�����ݼ�����
// ����: 
// **************************************************
static BOOL GetValueData(LPTSTR lpCurPos,DWORD dwType,LPTSTR *lppData,LPDWORD lpcbData)
{
	DWORD dwLen;
	if (dwType==REG_SZ)
	{  // ���ַ�������
		dwLen=0;
		lpCurPos++;  // skip the string start code \"
		while(1)
		{   // �õ����ݳ���
			if (*(lpCurPos+dwLen)=='\"')  // Is string End code \"
				break; // ���ݽ���
			if (*(lpCurPos+dwLen)==0)  // if the line is tail, but no string end code \",then is Invalid Line
				return FALSE; // û�н���������������
			dwLen++;
		}
		if (lppData!=NULL)
		{
			*lppData=(LPTSTR)malloc(dwLen+1);  // �������ڱ������ݵĻ���
			if (*lppData==NULL)  // the memory is not enough
				return FALSE;
			memcpy(*lppData,lpCurPos,dwLen);  // ��������
			(*lppData)[dwLen]=0;  // add the terminate null
			*lpcbData=dwLen+1;  // include terminate
			return TRUE;
		}
		return FALSE;
	}
	if (dwType==REG_DWORD)
	{ // ��DWORD����
		dwLen=0;
		lpCurPos++;  // skip the DWORD type start code ':'
		while(1)
		{  // �õ����ݳ���
			if (*(lpCurPos+dwLen)==0)  // if the line is tail, but no string end code \",then is Invalid Line
				break;
			dwLen++;
		}
		if (lppData!=NULL)
		{
			LPTSTR lpTemp;
			DWORD dwValue;
			lpTemp=(LPTSTR)malloc(dwLen+1);  // ���������ݵĻ���
			if (lpTemp==NULL)  // the memory is not enough
				return FALSE;
			memcpy(lpTemp,lpCurPos,dwLen);  // �õ�����
			lpTemp[dwLen]=0;  // add the terminate null

			*lppData=(LPTSTR)malloc(sizeof(DWORD));  // ������DWORD�Ļ���
			if (*lppData==NULL)  // the memory is not enough
				return FALSE;
			//dwValue=atoi(lpTemp);
			dwValue=strtoul( lpTemp, NULL, 16);  // ������ת��ΪDWORD
			*(DWORD*)(*lppData)=dwValue;//atoi(lpTemp); // ��������
			*lpcbData=sizeof(DWORD); // �������ݳ���
			free(lpTemp);  // �ͷ��м仺��
			return TRUE;
		}
		return FALSE;
	}
	if (dwType==REG_BINARY)
	{  // �Ƕ���������
		LPTSTR lpTemp;
		TCHAR lpChar[16];
		DWORD dwChar=0;

		dwLen=0;

		lpCurPos++;  // skip the DWORD type start code ':'
		lpTemp=(LPTSTR)malloc(MAXBINDATALEN);  // �����֧�ֵ����Ķ��������ݵĻ���
		if (lpTemp==NULL)  // ����ʧ��
			return FALSE;
		while(1)
		{  // �õ�����
			if (*lpCurPos==0)  // if the line is tail, but no string end code \",then is Invalid Line
			{  // ���ݽ���
				lpChar[dwChar]=0;
				//lpTemp[dwLen]=atoi(lpChar);  // get a data
				lpTemp[dwLen]=(char)strtoul( lpChar, NULL, 16); // �����ݱ��浽����

				dwChar=0;
				dwLen++;  // ����һ������
				break;
			}
			if (*lpCurPos==',')
			{ // ���ַָ���
				lpChar[dwChar]=0;
				//lpTemp[dwLen]=atoi(lpChar);  // get a data
				lpTemp[dwLen]=(char)strtoul( lpChar, NULL, 16);  // ת�������浱ǰ����

				dwChar=0;
				dwLen++;  // ����һ������
				lpCurPos++;
				continue;
			}
			if (*lpCurPos=='\\')
			{  // Ҫ���ȡ��һ��
				// This is Connect the next line ,Will Read The Next Line
				if (ReadLine(g_hReadFile,linebuffer,size)==FALSE)
				{
						RETAILMSG(1,("Read Line Failure \r\n"));
						return FALSE;
				}
//				RETAILMSG(1,("Read Line <%s> \r\n",linebuffer));
				lpCurPos=linebuffer;
				while(1)
				{  // jump the start space
					if (!(*lpCurPos==' ' || *lpCurPos==TABCODE))
						break;
					if (*lpCurPos==0)
						break;
					lpCurPos++;
				}
				continue;
			}
			lpChar[dwChar]=*lpCurPos;  // ��ȡ����
			dwChar++;
			if (dwChar>=16)  // The max char len
			{  // ��������
				free(lpTemp);
				return FALSE;
			}
			lpCurPos++;
		}
		if (lppData!=NULL)
		{
			*lppData=(LPTSTR)malloc(dwLen);  // �������ݻ���
			if (*lppData==NULL)  // the memory is not enough
				return FALSE;
			memcpy(*lppData,lpTemp,dwLen);  // ���õ������ݸ��Ƶ����ݻ���
			*lpcbData=dwLen; // �������ݳ���
			free(lpTemp); // �ͷ���ʱ����
			return TRUE; // ���سɹ�
		}
		free(lpTemp);
		return FALSE;
	}
	return FALSE;
}



/***********************************************************************/
// SaveAs Register to Special file
/***********************************************************************/

HKEY RootKeyTab[]={

#ifdef ROOT_MICROSOFT

	HKEY_CLASSES_ROOT,
	HKEY_CURRENT_USER,
	HKEY_LOCAL_MACHINE,
	HKEY_USERS,

#else
// !!! Modified By Jami chen in 2003.06.27
//	HKEY_ROOT,
// !!! 
	HKEY_HARDWARE_ROOT,
	HKEY_SOFTWARE_ROOT,
// !!!

#endif

};

#undef ROOTKEYNUM

#ifdef ROOT_MICROSOFT
	#define ROOTKEYNUM   4
#else
	#define ROOTKEYNUM   2
#endif

static BOOL WriteKeyToFile(HANDLE handle,LPREGKEY hRegKey,LPTSTR lpKeyName);
static BOOL WriteValueToFile(HANDLE hFile,LPREGKEY hRegKey);


// **************************************************
// ������BOOL SaveAsRegister(LPTSTR lpRegisterFile)
// ������
// 	IN lpRegisterFile -- ָ�����ļ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������浱ǰ��ע���ָ�����ļ��С�
// ����: 
// **************************************************
BOOL SaveAsRegister(LPTSTR lpRegisterFile)
{
	LPREGKEY hRegKey;
	int i;
	HKEY hKey;
	HANDLE hFile;

		// ��ָ�����ļ�
		hFile=CreateRegFile(lpRegisterFile,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE)
			return FALSE;  // ���ļ�ʧ��

		for (i=0;i<ROOTKEYNUM;i++)
		{
			hKey=RootKeyTab[i];  // �õ�ָ���ĸ���
			hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
										// then return the key
			if (hRegKey==NULL)
				return ERROR_BADKEY;  // ����һ������ĸ���
			
			WriteKeyToFile(hFile,hRegKey,NULL);  // �������µ�����д���ļ���
		}	
		CloseRegHandle(hFile);  // �ر��ļ�
		return TRUE;
}

/***********************************************************************/
// Will Write the Key (include the SubKey) to the file
/***********************************************************************/
// **************************************************
// ������static BOOL WriteKeyToFile(HANDLE hFile,LPREGKEY hRegKey,LPTSTR lpKeyName)
// ������
// 	IN hFile -- �ļ����
// 	IN hRegKey -- ��ǰ�ļ��ṹ
// 	IN lpKeyName -- ����������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ָ���ļ�д���ļ��С�
// ����: 
// **************************************************
static BOOL WriteKeyToFile(HANDLE hFile,LPREGKEY hRegKey,LPTSTR lpKeyName)
{
	DWORD dwIndex=0;
	LPTSTR lpCurKeyName;
	DWORD dwKeyNameLen;
	LPREGKEY hSubKey;
	
		// �õ���ǰ���ĳ��ȣ���ǰ���ǰ����������Լ������ĳ���
		if (lpKeyName)
		{  // �и���  
			dwKeyNameLen=strlen(lpKeyName)+strlen(hRegKey->lpKeyName)+1+1;  // 1-->key space mark '\' ,1-->terminate code 
		}
		else
		{ // û�и���
			dwKeyNameLen=strlen(hRegKey->lpKeyName)+1;  //1-->terminate code 
		}
		lpCurKeyName=(LPTSTR)malloc(dwKeyNameLen);  // ���䵱ǰ���ĳ���
		if (lpCurKeyName==NULL)
			return FALSE;  // �����ڴ�ʧ��
		//sprintf(lpCurKeyName,"%s\\%s",lpKeyName,hKey->lpKeyName);
		// �õ���ǰ��
		if (lpKeyName)
		{ // �и���
			strcpy(lpCurKeyName,lpKeyName);  // ���Ƹ���������
			strcat(lpCurKeyName,"\\"); // ��ӷָ���
			strcat(lpCurKeyName,hRegKey->lpKeyName); // ����Լ��ļ���
		}
		else
		{ // û�и���
			strcpy(lpCurKeyName,hRegKey->lpKeyName);  // �����Լ��ļ���
		}
		while(1)
		{// ���Ӽ�д���ļ�
			hSubKey=SearchSubKeyByIndex(hRegKey,dwIndex);  // Search the specical index is exist in the RegKey
			if (hSubKey==NULL)  // the Sub Key is not Exist ,have no more subkey
			{ // �Ѿ�û���Ӽ���
				break;
			}
			WriteKeyToFile(hFile,hSubKey,lpCurKeyName);  // д�������ݵ��ļ�
			dwIndex++;  // ��һ���Ӽ�
		}
		if (hRegKey->hRegValue)
		{ // ��ǰ���м�ֵ������ֵд���ļ�
			WriteRegFile(hFile,"[",1,&dwKeyNameLen,NULL);  // д������Ŀ�ʼ��־
			WriteRegFile(hFile,lpCurKeyName,strlen(lpCurKeyName),&dwKeyNameLen,NULL);// д�����
			WriteRegFile(hFile,"]\r\n",3,&dwKeyNameLen,NULL); // д������Ľ�����־
			//
			// Will Write the Key Value
			WriteValueToFile(hFile,hRegKey);  // д���ֵ���ļ�
			WriteRegFile(hFile,"\r\n",2,&dwKeyNameLen,NULL); // ��ӻس���
		}
		// д�����
		free(lpCurKeyName);  // �ͷŵ�ǰ�ļ���
		return TRUE;
}


/***********************************************************************/
// Will Write the Key Value to the file
/***********************************************************************/
// **************************************************
// ������static BOOL WriteValueToFile(HANDLE hFile,LPREGKEY hRegKey)
// ������
// 	IN hFile -- �ļ����
// 	IN hRegKey -- Ҫд���ֵ�ļ��Ľṹ
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
static BOOL WriteValueToFile(HANDLE hFile,LPREGKEY hRegKey)
{
	LPREGVALUE hRegValue;
	DWORD dwWriteLen=0;

	hRegValue=hRegKey->hRegValue;  // �õ���һ����ֵ
	while(hRegValue)
	{  // ���м�ֵ
		// Write the Value Name  eg.   "abcd"=
		WriteRegFile(hFile,"    ",4,&dwWriteLen,NULL); // д��4���ո���������
		WriteRegFile(hFile,"\"",1,&dwWriteLen,NULL);  // д���ֵ�����ƵĿ�ʼ��־
		WriteRegFile(hFile,hRegValue->lpValueName,strlen(hRegValue->lpValueName),&dwWriteLen,NULL); // д���ֱ������
		WriteRegFile(hFile,"\"=",2,&dwWriteLen,NULL); // д���ֵ�����ƵĽ�����־

		// Write the Value Data
		if (hRegValue->dwType==REG_SZ)
		{  // write the string
			WriteRegFile(hFile,"\"",1,&dwWriteLen,NULL);  // Write the quotation mark
			WriteRegFile(hFile,hRegValue->lpData,strlen((const char *)hRegValue->lpData),&dwWriteLen,NULL);  // Write the string
			WriteRegFile(hFile,"\"\r\n",3,&dwWriteLen,NULL);//write the quotation mark and return code
		}
		else if (hRegValue->dwType==REG_DWORD)
		{  // write a dword data
			TCHAR buffer[16];
			WriteRegFile(hFile,"dword:",6,&dwWriteLen,NULL);  // write the dword mark "dword:"
			itoa(*(DWORD *)hRegValue->lpData,buffer,16);  // turn the dword to the char
			WriteRegFile(hFile,buffer,strlen((const char *)buffer),&dwWriteLen,NULL); // write this dword
			WriteRegFile(hFile,"\r\n",2,&dwWriteLen,NULL);  // write the return code
		}
		else if (hRegValue->dwType==REG_BINARY)
		{  // write a binary data
			TCHAR buffer[16];
			DWORD dwCurPos=0;
			int i;
			TCHAR HexTab[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

			WriteRegFile(hFile,"hex:",4,&dwWriteLen,NULL);  // Write the binary mark "hex:"
			if (hRegValue->cbData>32)
			{ // Must Write Multi Line
				while(1)
				{
					if (dwCurPos<hRegValue->cbData)
					{  // Will Write the Next Line
						WriteRegFile(hFile,"\\\r\n",3,&dwWriteLen,NULL); // Write the End Connect code '\'
						WriteRegFile(hFile,"    ",4,&dwWriteLen,NULL);  // Write the Start space Code
					}
					else
					{
						break;  // had the End
					}
					for (i=0;i<24;i++)  // Line a Line , can write 24 BYTE
					{  
						// Write a BYTE
						buffer[0]=HexTab[hRegValue->lpData[dwCurPos]/16];  // Write a First char of the BYTE
						buffer[1]=HexTab[hRegValue->lpData[dwCurPos]%16];  // Write a Second char of the BYTE
						WriteRegFile(hFile,buffer,2,&dwWriteLen,NULL);  // Write this BYTE
						dwCurPos++;  // to Next BYTE
						if (dwCurPos>=hRegValue->cbData)  // to the end ?
							break;
						WriteRegFile(hFile,",",1,&dwWriteLen,NULL);  // write space mark code ','
					}
				}
			}
			else
			{ // Only Write one line
				while(1)
				{
					// Write a BYTE
					buffer[0]=HexTab[hRegValue->lpData[dwCurPos]/16];  // Get the first Char  of a BYTE
					buffer[1]=HexTab[hRegValue->lpData[dwCurPos]%16];  // Get The second Char of a BYTE
					WriteRegFile(hFile,buffer,2,&dwWriteLen,NULL);  // Write the BYTE to The File
					dwCurPos++;    // To Next BYTE
					if (dwCurPos>=hRegValue->cbData)  // Check to the End
						break;
					WriteRegFile(hFile,",",1,&dwWriteLen,NULL);  // Write a space Mark ','
				}
			}
			WriteRegFile(hFile,"\r\n",2,&dwWriteLen,NULL);  // Write a reutrn code
		}
		else
		{
			WriteRegFile(hFile,"\r\n",2,&dwWriteLen,NULL);  // Is unKnow Type, Only write a return code
		}

		hRegValue=hRegValue->hNextValue;  // To Next value
	}
	return TRUE;
}


#define SYSTEMREGISTERFILE  "\\kingmos\\kingmos.reg"

// **************************************************
// ������BOOL InitRegistry( void )
// ������
// 	��
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ʼ��ע���
// ����: 
// **************************************************
BOOL InitRegistry( void )
{
	// RETAILMSG( 1, ( "Load system registry.\r\n" ) );
#if 1
	if (LoadRegister( NULL ))  // ��FLASH��װ��ע���
	{
		return TRUE;
	}
#endif	
	return LoadRegister( SYSTEMREGISTERFILE );  // װ��ϵͳע����ļ�
}

// **************************************************
// ������BOOL DeinitRegistry( void )
// ������
// 	��
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������˳�ע���
// ����: 
// **************************************************
BOOL DeinitRegistry( void )
{
	//RETAILMSG( 1, ( "Load system registry.\r\n" ) );
//	return SaveAsRegister( SYSTEMREGISTERFILE );  // ���浱ǰ���ݵ�ϵͳע����ļ�
	return SaveAsRegister( NULL );  // ���浱ǰ���ݵ�ϵͳע���������
}
/*
// test code
//#define HKEY_SOFTWARE_ROOT			(( HKEY ) 0x80000001 )
void LoadProgramFromRegistry( void )
{
	HKEY hSubKey;

    if( FileReg_OpenKeyEx( HKEY_SOFTWARE_ROOT, "boot", 0, 0, &hSubKey ) == ERROR_SUCCESS )
	{
		TCHAR filePath[MAX_PATH];
		int n = 0;

		KL_DebugOutString( "open root ok.\r\n" ); 

		for( ; n < 10; n++ )
		{
			DWORD dwType;
			TCHAR tcValueName[16];  // boot01
			DWORD dwValueNameSize = sizeof( tcValueName );
			TCHAR tcValue[MAX_PATH];
			DWORD dwValueSize = sizeof( tcValue );
			
			if( ERROR_SUCCESS == FileReg_EnumValue( hSubKey, n, tcValueName, &dwValueNameSize, NULL, &dwType, tcValue, &dwValueSize ) )
			{
				KL_DebugOutString( "FileReg_EnumValue ok.\r\n" ); 
				if( dwType == REG_SZ &&
					strnicmp( tcValueName, "boot", 4 ) == 0 && 
					tcValueName[4] >= '0' && tcValueName[4] <= '9' && 
					tcValueName[5] >= '0' && tcValueName[5] <= '9' )
				{
					DWORD dwSize;
					dwSize = MAX_PATH;
					{
						strcpy( filePath, "\\kingmos\\" );
						strcat( filePath,  tcValueName );

	                    KL_DebugOutString( "boot load " ); 
						KL_DebugOutString( filePath ); 
						KL_DebugOutString( ".\r\n" ); 
                        KL_CreateProcess( filePath, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL, NULL ); 
					}
				}
			}
		}
	}
}
*/


#define FLASHFILEHANDLE  0X01234567
#define FLASHREGFLAG	"JREG"  // ע����־
//  һ�������FLASH ע�������ͷ�ṹ
typedef struct structFlashHeader{
		TCHAR chFlag[4]; // ��ű�־��JREG��
		DWORD dwDataSize; // ������ݴ�С
		DWORD dwRead; //�Ѿ���ȡ������
		DWORD dwStatus;  // ��ǰ״̬ �� 1 -- ������ �� 2 -- д����
}FLASHHEADER, *LPFLASHHEADER ;

static LPFLASHHEADER g_curFlashHeader = NULL;

static HANDLE WINAPI CreateRegFile( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{

	if (lpcszFileName == NULL)
	{
//		RETAILMSG(1,("Open Flash Register File ...\r\n"));

		if (g_curFlashHeader != NULL)
		{
//			RETAILMSG(1,(TEXT("current flash had opened\r\n")));
			return INVALID_HANDLE_VALUE; // ������Ч���
		}

		g_curFlashHeader = (LPFLASHHEADER)malloc(sizeof(FLASHHEADER)); // ����һ���ռ�����ŵ�ǰҪ��ȡ�����ݵĴ�С
		if (g_curFlashHeader == NULL )
			return INVALID_HANDLE_VALUE; // ������Ч���
		if (dwAccess & GENERIC_READ)
		{
//			RETAILMSG(1,("Need Read Register Data ...\r\n"));
			// Ҫ���FLASH�ж�ȡ
			if (Sys_RegistryIO( (LPBYTE)g_curFlashHeader, sizeof(FLASHHEADER), REGIO_READ | REGIO_BEGIN ) == FALSE)// ��ȡͷ��Ϣ
			{
				// ��ȡͷ��Ϣʧ��
				free(g_curFlashHeader);
				g_curFlashHeader = NULL;
				RETAILMSG(1,(TEXT("!!!Read Head failure\r\n")));
				return INVALID_HANDLE_VALUE; // ������Ч���
			}
//			RETAILMSG(1,(TEXT("g_curFlashHeader->chFlag = %s\r\n"),g_curFlashHeader->chFlag));
			if (strncmp(g_curFlashHeader->chFlag,FLASHREGFLAG,4) != 0)
			{
				Sys_RegistryIO( NULL, 0, REGIO_END); // �ر�������Ϣ
				free(g_curFlashHeader);
				g_curFlashHeader = NULL;
				RETAILMSG(1,(TEXT("!!!error data, no data\r\n")));
				return INVALID_HANDLE_VALUE; // ������Ч���
			}
			g_curFlashHeader->dwRead = 0; // ��û�п�ʼ��ȡ����
			g_curFlashHeader->dwStatus = 1; // ������
		}
		else if (dwAccess & GENERIC_WRITE)
		{
			//Ҫ��д��FLASH
//			RETAILMSG(1,("Need Write Register Data ...\r\n"));
			strncpy(g_curFlashHeader->chFlag,FLASHREGFLAG,4);
			g_curFlashHeader->dwRead  = 0;
			g_curFlashHeader->dwDataSize = 0;
			if (Sys_RegistryIO( (LPBYTE)g_curFlashHeader, sizeof(FLASHHEADER), REGIO_WRITE | REGIO_BEGIN ) == FALSE) // д��ͷ��Ϣ
			{
				// д��ͷ��Ϣʧ��
				free(g_curFlashHeader);
				g_curFlashHeader = NULL;
				RETAILMSG(1,(TEXT("Write Head failure\r\n")));
				return INVALID_HANDLE_VALUE; // ������Ч���
			}
			g_curFlashHeader->dwStatus = 2; // д����
		}
		// ��ȷ����
//		RETAILMSG(1,("Open Register OK !!!!!!!!!\r\n"));
		return (HANDLE)FLASHFILEHANDLE;
	}
	else
	{
		// һ���ļ�
		return CreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
	}
}


static BOOL ReadRegFile( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped )
{
	if (hFile == (HANDLE)FLASHFILEHANDLE)
	{
		// ��flash�ж�ȡ
		if (g_curFlashHeader == NULL)
		{
			RETAILMSG(1,(TEXT("error Header pointer\r\n")));
			return FALSE;  // ��ǰ��ͷ�ṹ����
		}
		if (dwNumToRead > g_curFlashHeader->dwDataSize - g_curFlashHeader->dwRead)
		{
			// �Ѿ�û��̫�������
			dwNumToRead = g_curFlashHeader->dwDataSize - g_curFlashHeader->dwRead;

			if (dwNumToRead ==0)
			{
				// �Ѿ�û��������
				if (lpdwNumRead)
					*lpdwNumRead = 0;
				return FALSE;
			}
		}
		if (Sys_RegistryIO( (LPBYTE)lpvBuffer,dwNumToRead, REGIO_READ) == FALSE) // д��ͷ��Ϣ
		{
			// ��ȡ����ʧ��
			RETAILMSG(1,(TEXT("Read Data failure\r\n")));
			return FALSE; // ������Ч���
		}
//		RETAILMSG(1,(TEXT("Read Data  = %s\r\n"),lpvBuffer));
		g_curFlashHeader->dwRead += dwNumToRead; // ��ǰ��������ȷ��ȡ

		return TRUE;
	}
	else
	{
		// ������ȡ�ļ�
		return ReadFile( hFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
	}
}


static BOOL WriteRegFile( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped )
{
	if (hFile == (HANDLE)FLASHFILEHANDLE)
	{
//		RETAILMSG(1,("%s",lpvBuffer));
		// д�뵽flash
		if (g_curFlashHeader == NULL)
		{
			RETAILMSG(1,(TEXT("error Header pointer\r\n")));
			return FALSE;  // ��ǰ��ͷ�ṹ����
		}
		if (Sys_RegistryIO( (LPBYTE)lpvBuffer, dwNumToWrite, REGIO_WRITE) == FALSE) // д��ͷ��Ϣ
		{
			// д������ʧ��
			RETAILMSG(1,(TEXT("Write Data failure\r\n")));
			return FALSE; // ������Ч���
		}
		if ( lpdwNumWrite != NULL)
		{
			*lpdwNumWrite = dwNumToWrite;
		}
		g_curFlashHeader->dwDataSize += dwNumToWrite; // ��ǰ��������ȷд��

		return TRUE;
	}
	else
	{
		// ����д���ļ�
		return WriteFile( hFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
	}
}


static BOOL CloseRegHandle(HANDLE  hHandle)
{
	if (hHandle == (HANDLE)FLASHFILEHANDLE)
	{
//		RETAILMSG(1,(TEXT("Close Flash Registry Handle\r\n")));
		// �ر�FALSH ����
		Sys_RegistryIO( NULL, 0, REGIO_END); // �ر�������Ϣ
		// �����д���ݣ��������½�ͷ�ṹд��
		if (g_curFlashHeader->dwStatus == 2) // д����
		{
			Sys_RegistryIO( (LPBYTE)g_curFlashHeader, sizeof(FLASHHEADER), REGIO_WRITE | REGIO_BEGIN ); // д��ͷ��Ϣ
			Sys_RegistryIO( NULL, 0, REGIO_END); // �ر�������Ϣ
		}
		free(g_curFlashHeader); // �ͷ�ͷ�ṹ
		g_curFlashHeader = NULL;
		return TRUE;
	}
	else
	{
		// ������ȡ�ļ�
		return CloseHandle( hHandle );
	}
}

