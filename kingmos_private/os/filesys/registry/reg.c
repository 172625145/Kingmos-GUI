/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：注册表管理
版本号：1.0.0.456
开发时期：2002-03-06
作者：陈建明 Jami Chen
修改记录：    
		2004.05.11 Jami chen : 对函数名定义修改，KL_Reg...  --> FileReg_...
		2004.05.11 Jami chen : 对内存分配，释放函数修改 _kalloc , _kfree  --> malloc, free
		2004.05.11 Jami chen : 对键的键名和键值比较时不区分大小写，将 strcmp,strncmp  --> stricmp,strnicmp
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
// 定义区域
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
// 函数声明区
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
// 声明：LONG WINAPI FileReg_CloseKey( HKEY hKey )
// 参数：
// 	IN hKey -- 指定的键的句柄
// 
// 返回值：成功返回ERROR_SUCCESS，否则返回错误代码
// 功能描述：关闭注册键。
// 引用: 
// **************************************************
LONG WINAPI FileReg_CloseKey( HKEY hKey )
{
	LPREGKEY hRegKey;

	hRegKey=(LPREGKEY)hKey;  // 得到注册键结构
	if (hRegKey==NULL)
		return ERROR_BADKEY;   // Current Key is valid , Can't Close it
	if (hRegKey->bOpen==FALSE)
		return ERROR_BADKEY;   // Current Key Had not Open , Can't need Close it
	hRegKey->bOpen=FALSE;  // 设置关闭属性
	return ERROR_SUCCESS;  // 返回成功

}

// **************************************************
// 声明：LONG WINAPI FileReg_CreateKeyEx( HKEY hKey, LPCTSTR lpSubKey, DWORD Reserved, LPTSTR lpClass, 
//							DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition )
// 参数：
// 	IN hKey -- 要创建子键的句柄
// 	IN lpSubKey -- 子键的名称
// 	IN Reserved -- 保留
// 	IN lpClass -- 类名
// 	IN dwOptions -- 可选类型
// 	IN samDesired -- 保留
// 	IN lpSecurityAttributes  -- 安全属性
// 	OUT phkResult -- 返回已经创建的键的句柄
// 	OUT lpdwDisposition -- 返回打开的方式
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错误
// 功能描述：创建一个指定的键。
// 引用: 
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
				dwDisposition=REG_CREATED_NEW_KEY;  // 设置要创建一个新键
				hSubKey=CreateNewSubKey(hRegKey,lpSubKey); // 创建一个新键
			}
			if (hSubKey==NULL)  // Create New Sub Key Failure,maybe is memory not enough
				return ERROR_NO_LOG_SPACE;  

			hRegKey=hSubKey;  // Set the Sub Key to Current Key, to Enter next sub key

			lpSubKey=ToNextSubKey(lpSubKey);  // go to the next sub Key

			if (*lpSubKey==0)
				break; // 打开完成
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
// 声明：LONG WINAPI FileReg_DeleteKey( HKEY hKey,  LPCTSTR lpSubKey )
// 参数：
// 	IN hKey -- 要删除子键的句柄
// 	IN lpSubKey -- 要删除的子键
// 
// 返回值：成功返回ERROR_SUCCESS，否则存在错误。
// 功能描述：删除指定的键。
// 引用: 
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
			return ERROR_BADKEY; // 指定的键不存在
		
		while(1)
		{
			hSubKey=SearchSubKey(hRegKey,lpSubKey);  // Search the SubKey is exist in the RegKey
			if (hSubKey==NULL)  // the Sub Key is not Exist ,must to Create New Sub Key
			{ // 没有找到指定的子键
				return ERROR_BADKEY;
			}

			lpSubKey=ToNextSubKey(lpSubKey);  // go to the next sub Key

			if (*lpSubKey==0)  // Had no sub Key
				break;  // 已经没有子键了

			hRegKey=hSubKey;  // Set the Sub Key to Current Key, to Enter next sub key

		}
		if (DeleteSubRegKey(hRegKey,hSubKey)==FALSE)  // delete the sub key
			return ERROR_KEY_DELETED;
		return ERROR_SUCCESS;
}

// **************************************************
// 声明：LONG WINAPI FileReg_DeleteValue( HKEY hKey, LPCTSTR lpValueName )
// 参数：
// 	IN hKey -- 要删除键值的句柄
// 	IN lpValueName -- 要删除的键值的名称
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错。
// 功能描述：删除一个当前打开的键的键值。
// 引用: 
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
			return ERROR_BADKEY;  // 键不存在
		if (IsOpenKey(hRegKey)==FALSE)  // the key is or not open ?
			return ERROR_BADKEY; // 键没有打开

		hRegValue=SearchValue(hRegKey,lpValueName);  // Search the Value have exist in the key 
		if (hRegValue==NULL)
		{  // 键值不存在
			return ERROR_BADKEY;	
		}
		return DeleteValue(hRegKey,hRegValue);  // Delete current value
}

// **************************************************
// 声明：LONG WINAPI FileReg_EnumKeyEx(HKEY hKey, DWORD dwIndex, LPTSTR lpName, LPDWORD lpcbName, LPDWORD lpReserved, 
									//LPTSTR lpClass, LPDWORD lpcbClass, PFILETIME lpftLastWriteTime )

// 参数：
// 	IN hKey -- 已经打开的键的句柄
// 	IN dwIndex -- 要打开的子键的索引
// 	OUT lpName -- 存放子键名称的缓存
// 	IN/OUT lpcbName -- 子键名称的长度
// 	IN lpReserved -- 保留
// 	OUT lpClass -- 存放子键的类的缓存
// 	IN/OUT lpcbClass -- 子键的类的长度
// 	OUT lpftLastWriteTime -- 最后一次的修改时间
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错。
// 功能描述：列举当前打开的键下的所有子键。
// 引用: 
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
			return ERROR_BADKEY;  // 指定的键不存在
		
		hSubKey=SearchSubKeyByIndex(hRegKey,dwIndex);  // Search the specical index is exist in the RegKey
		if (hSubKey==NULL)  // the Sub Key is not Exist ,have no more subkey
		{
			// 已经没有子键了，设置找到的子键的名称为空
			if (lpName!=NULL)
				*lpName=0; 
			if (lpcbName!=NULL)
				*lpcbName=0;
			if (lpClass!=NULL)
				*lpClass=0;
			if (lpcbClass!=NULL)
				*lpcbClass=0;
			//return ERROR_SUCCESS;  // 返回成功
			return ERROR_NO_MORE_ITEMS; // 返回已经没有子键了
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

		return ERROR_SUCCESS; // 返回成功
}

 
// **************************************************
// 声明：LONG WINAPI FileReg_FlushKey( HKEY hKey )

// 参数：
// 	IN hKey -- 指定的键
// 
// 返回值：返回 0
// 功能描述： 刷新键，保留
// 引用: 
// **************************************************
LONG WINAPI FileReg_FlushKey( 
				  HKEY hKey 
				  )
{
	return 0;
}

// **************************************************
// 声明：LONG WINAPI FileReg_EnumValue( HKEY hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcbValueName, 
//									LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
// 参数：
// 	IN hKey -- 打开的键值
// 	IN dwIndex -- 要得到的键值的索引
// 	OUT lpValueName -- 返回键值的名称
// 	IN/OUT lpcbValueName -- 返回键值的长度
// 	IN lpReserved -- 保留
// 	OUT lpType -- 返回键值数据的类型
// 	OUT lpData -- 返回键值数据
// 	IN/OUT  lpcbData -- 返回键值数据的长度
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错。
// 功能描述：列举打开的键的键值。
// 引用: 
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
		if (hRegKey==NULL)  // 没有找到指定的键
			return ERROR_BADKEY;
		if (IsOpenKey(hRegKey)==FALSE)  // the key is or not open ?
			return ERROR_BADKEY;

		hRegValue=SearchValueByIndex(hRegKey,dwIndex);  // Search the Value have exist in the key 
		if (hRegValue==NULL)
		{   // have not exist more value，已经没有键值了
			// 设置已经没有键值
			if (lpValueName!=NULL)
				*lpValueName=0;
			if (lpcbValueName!=NULL)
				*lpcbValueName=0;
			if (lpType!=NULL)
				*lpType=0;
			if (lpcbData!=NULL) 
				*lpcbData=0;
//			return ERROR_SUCCESS;// 返回成功
			return ERROR_NO_MORE_ITEMS; // 返回已经没有子键了
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
			{ // 存储当前的键值
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
		return ERROR_SUCCESS;// 返回成功
}

// **************************************************
// 声明：LONG WINAPI FileReg_OpenKeyEx( HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult )
// 参数：
// 	IN hKey -- 要创建子键的句柄
// 	IN lpSubKey -- 子键的名称
// 	IN ulOptions -- 可选类型
// 	IN samDesired -- 保留
// 	OUT phkResult -- 返回已经创建的键的句柄
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错
// 功能描述：打开指定的子键。
// 引用: 
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
			return ERROR_BADKEY;  // 没有找到指定的键
		
		while(1)
		{
			hSubKey=SearchSubKey(hRegKey,lpSubKey);  // Search the SubKey is exist in the RegKey
			if (hSubKey==NULL)  // the Sub Key is not Exist 
			{  // 没有找到指定的子键
				return ERROR_BADKEY;
			}

			hRegKey=hSubKey;  // Set the Sub Key to Current Key, to Enter next sub key

			lpSubKey=ToNextSubKey(lpSubKey);  // go to the next sub Key

			if (*lpSubKey==0)
				break;  // 已经没有子键了
		}
		if (OpenCurrentRegKey(hRegKey)==FALSE)  // Open current the Reg Key
			return ERROR_CANTOPEN;
		if (phkResult!=NULL)           // return handle of the key
			*phkResult=(HKEY)hRegKey;
		return ERROR_SUCCESS;  // 返回成功
}

// **************************************************
// 声明：LONG WINAPI FileReg_QueryInfoKey( HKEY hKey, LPTSTR lpClass, LPDWORD lpcbClass, LPDWORD lpReserved, LPDWORD lpcSubKeys, LPDWORD lpcbMaxSubKeyLen, 
//									   LPDWORD lpcbMaxClassLen, LPDWORD lpcValues, LPDWORD lpcbMaxValueNameLen, LPDWORD lpcbMaxValueLen, LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime )

// 参数：
// 	IN hKey -- 当前要查询的键的句柄
// 	OUT lpClass -- 键的类名
// 	OUT lpcbClass -- 类名长度
// 	OUT lpReserved -- 保留
// 	OUT lpcSubKeys -- 子键个数
// 	OUT lpcbMaxSubKeyLen -- 子键名称的最大长度
// 	OUT lpcbMaxClassLen -- 子键类名的最大长度
// 	OUT lpcValues -- 键值的个数
// 	OUT lpcbMaxValueNameLen -- 最大键值的名称的长度
// 	OUT lpcbMaxValueLen -- 最大键值数据的长度
// 	OUT lpcbSecurityDescriptor -- 安全设置
// 	OUT lpftLastWriteTime -- 最后一次写的时间
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错。
// 功能描述：查询键的信息。
// 引用: 
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
			return ERROR_BADKEY;  // 没有指定的键

		if (lpClass!=NULL&&lpcbClass==NULL)
			return ERROR_INVALID_PARAMETER;  // 参数错误
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
		return ERROR_SUCCESS; // 返回成功
}

// **************************************************
// 声明：LONG WINAPI FileReg_QueryValueEx( HKEY hKey, LPCTSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
// 参数：
// 	IN hKey -- 指定的键
// 	IN lpValueName -- 键值
// 	OUT lpReserved -- 保留
// 	OUT lpType -- 返回数据类型
// 	OUT lpData -- 返回数据
// 	OUT lpcbData -- 返回数据的长度
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错
// 功能描述：得到指定键值的数据。
// 引用: 
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
		{  // 没有数据
			return ERROR_BADKEY;	
		}
		return GetValue(hRegValue,lpType,lpData,lpcbData);  // Get Data from the value
}

// **************************************************
// 声明：LONG WINAPI FileReg_SetValueEx( HKEY hKey, LPCTSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData )
// 参数：
// 	IN hKey -- 打开的键的句柄
// 	IN lpValueName -- 键值
// 	IN Reserved -- 保留
// 	IN dwType -- 数据类型
// 	IN lpData -- 数据指针
// 	IN cbData -- 数据长度
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错
// 功能描述：设置键值与数据。
// 引用: 
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
			return ERROR_BADKEY;  // 没有找到指定的键
		if (IsOpenKey(hRegKey)==FALSE)  // the key is or not open ?
			return ERROR_BADKEY;

		hRegValue=SearchValue(hRegKey,lpValueName);  // Search the Value have exist in the key 
		if (hRegValue==NULL)
		{
			// the valuename is not exist in the key
			hRegValue=CreateNewValue(hRegKey,lpValueName);  // Create New value in the key
			if (hRegValue==NULL)
				return ERROR_NO_LOG_SPACE; // 创建键值失败
		}
		if (SetValue(hRegValue,dwType,lpData,cbData)==FALSE)  // Set New Data to the value
		{
			return ERROR_NO_LOG_SPACE;
		}

		return ERROR_SUCCESS;  // 返回成功
}




/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
// **************************************************
// 声明：static LPREGKEY MapRootKey(HKEY hKey)
// 参数：
// 	IN hKey 
// 
// 返回值：返回指定键值的注册键结构指针
// 功能描述：映射根键。
// 引用: 
// **************************************************
static LPREGKEY MapRootKey(HKEY hKey)
{
#ifdef ROOT_MICROSOFT

		if (hKey==HKEY_CLASSES_ROOT)
		{  // 指定根键为HKEY_CLASSES_ROOT
			if (hkey_classes_root==NULL)
			{  // 还没有产生根键
				hkey_classes_root=(LPREGKEY)malloc(sizeof(REGKEY));  // 分配内存
				if (hkey_classes_root)
				{  // 设置HKEY_CLASSES_ROOT根键
					hkey_classes_root->bOpen=FALSE;
					hkey_classes_root->lpClass=AssignRegValue("HKEY_CLASSES_ROOT");
					hkey_classes_root->hNextKey=NULL;
					hkey_classes_root->hRegValue=NULL;
					hkey_classes_root->hSubKey=NULL;
					hkey_classes_root->lpKeyName=AssignRegValue("HKEY_CLASSES_ROOT");
				}
			}
			return hkey_classes_root;  // 返回HKEY_CLASSES_ROOT的结构
		}
		if (hKey==HKEY_CURRENT_USER)
		{ // 指定根键为HKEY_CURRENT_USER
			if (hkey_current_user==NULL)
			{  // 还没有产生根键
				hkey_current_user=(LPREGKEY)malloc(sizeof(REGKEY));  // 分配空间
				if (hkey_current_user)
				{  // 设置HKEY_CURRENT_USER根键
					hkey_current_user->bOpen=FALSE;
					hkey_current_user->lpClass=AssignRegValue("HKEY_CURRENT_USER");
					hkey_current_user->hNextKey=NULL;
					hkey_current_user->hRegValue=NULL;
					hkey_current_user->hSubKey=NULL;
					hkey_current_user->lpKeyName=AssignRegValue("HKEY_CURRENT_USER");
				}
			}
			return hkey_current_user;  // 返回HKEY_CURRENT_USER结构
		}
		if (hKey==HKEY_LOCAL_MACHINE)
		{  // 指定根键为HKEY_LOCAL_MACHINE
			if (hkey_local_machine==NULL)
			{ // 没有产生HKEY_LOCAL_MACHINE根键
				hkey_local_machine=(LPREGKEY)malloc(sizeof(REGKEY));  // 分配内存
				if (hkey_local_machine)
				{  // 设置HKEY_LOCAL_MACHINE根键
					hkey_local_machine->bOpen=FALSE;
					hkey_local_machine->lpClass=AssignRegValue("HKEY_LOCAL_MACHINE");
					hkey_local_machine->hNextKey=NULL;
					hkey_local_machine->hRegValue=NULL;
					hkey_local_machine->hSubKey=NULL;
					hkey_local_machine->lpKeyName=AssignRegValue("HKEY_LOCAL_MACHINE");
				}
			}
			return hkey_local_machine;  // 返回HKEY_LOCAL_MACHINE的结构
		}
		if (hKey==HKEY_USERS)
		{ // 指定根键为HKEY_USERS
			if (hkey_users==NULL)
			{  // 没有产生HKEY_USERS
				hkey_users=(LPREGKEY)malloc(sizeof(REGKEY));// 分配内存
				if (hkey_users)
				{  // 设置HKEY_USERS 结构
					hkey_users->bOpen=FALSE;
					hkey_users->lpClass=AssignRegValue("HKEY_USERS");
					hkey_users->hNextKey=NULL;
					hkey_users->hRegValue=NULL;
					hkey_users->hSubKey=NULL;
					hkey_users->lpKeyName=AssignRegValue("HKEY_USERS");
				}
			}
			return hkey_users;  // 返回HKEY_USERS结构
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
		{  // 指定根键为HKEY_HARDWARE_ROOT
			if (hkey_hardware_root==NULL)
			{  // 根键HKEY_HARDWARE_ROOT没有产生
				hkey_hardware_root=(LPREGKEY)malloc(sizeof(REGKEY));  // 分配空间
				if (hkey_hardware_root)
				{  // 设置HKEY_HARDWARE_ROOT结构
					hkey_hardware_root->bOpen=FALSE;
					hkey_hardware_root->lpClass=AssignRegValue("HKEY_HARDWARE_ROOT");
					hkey_hardware_root->hNextKey=NULL;
					hkey_hardware_root->hRegValue=NULL;
					hkey_hardware_root->hSubKey=NULL;
					hkey_hardware_root->lpKeyName=AssignRegValue("HKEY_HARDWARE_ROOT");
				}
			}
			return hkey_hardware_root;  // 返回HKEY_HARDWARE_ROOT结构
		}
		if (hKey==HKEY_SOFTWARE_ROOT)
		{ // 指定根键为HKEY_SOFTWARE_ROOT
			if (hkey_software_root==NULL)
			{  // 没有产生HKEY_SOFTWARE_ROOT
				hkey_software_root=(LPREGKEY)malloc(sizeof(REGKEY));  // 分配内存
				if (hkey_software_root)
				{  // 设置HKEY_SOFTWARE_ROOT结构
					hkey_software_root->bOpen=FALSE;
					hkey_software_root->lpClass=AssignRegValue("HKEY_SOFTWARE_ROOT");
					hkey_software_root->hNextKey=NULL;
					hkey_software_root->hRegValue=NULL;
					hkey_software_root->hSubKey=NULL;
					hkey_software_root->lpKeyName=AssignRegValue("HKEY_SOFTWARE_ROOT");
				}
			}
			return hkey_software_root;  // 返回HKEY_SOFTWARE_ROOT结构
		}
// !!!		

#endif

		return (LPREGKEY)hKey; //直接返回打开的键的结构
}

/***********************************************************************/
// Search the Sub Key in the RegKey
/***********************************************************************/
// **************************************************
// 声明：static LPREGKEY SearchSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey)
// 参数：
// 	IN hRegKey -- 当前指定的键
// 	IN lpSubKey -- 要查找的子键键名
// 
// 返回值：返回查找到的注册键结构，NULL表示没有找到
// 功能描述：在指定的键下查找指定的子键
// 引用: 
// **************************************************
static LPREGKEY SearchSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey)
{
	LPREGKEY hSubRegKey;

	hSubRegKey=hRegKey->hSubKey;  // 得到第一个子键的结构
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
// 声明：static LPREGKEY SearchSubKeyByIndex(LPREGKEY hRegKey,DWORD iIndex)
// 参数：
// 	IN hRegKey -- 当前指定的键
// 	IN iIndex -- 指定的索引
// 
// 返回值：返回找到的键的结构
// 功能描述：搜索指定索引的子键。
// 引用: 
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
// 声明：static BOOL MatchSubKey(LPREGKEY hSubRegKey,LPCTSTR lpSubKey)
// 参数：
// 	IN hSubRegKey -- 指定的注册键结构
// 	IN lpSubKey -- 指定的键名
// 
// 返回值：键与键名匹配返回TRUE，否则返回FALSE
// 功能描述：指定的键与键名是否匹配。
// 引用: 
// **************************************************
static BOOL MatchSubKey(LPREGKEY hSubRegKey,LPCTSTR lpSubKey)
{
	DWORD dwLen;
		dwLen=GetSubKeyLen(lpSubKey);  // 得到键名的长度
		if (dwLen==strlen(hSubRegKey->lpKeyName))
		{ // 与指定的键 的键名是否长度相同
			if (strnicmp(lpSubKey,(LPCTSTR)hSubRegKey->lpKeyName,dwLen)==0) // 比较字符串是否相同
				return TRUE; // 两者匹配
		}
		return FALSE;  // 不匹配
}

/***********************************************************************/
// Get the Sub Key Name Length in the Sub Key String 
/***********************************************************************/
// **************************************************
// 声明：static DWORD GetSubKeyLen(LPCTSTR lpSubKey)
// 参数：
// 	IN lpSubKey 指定的键名
// 
// 返回值：返回当前键名的长度
// 功能描述：得到当前键名的长度
// 引用: 
// **************************************************
static DWORD GetSubKeyLen(LPCTSTR lpSubKey)
{
	DWORD dwlen=0;
	while(*lpSubKey)  // the character is or not terminating NULL character
	{
		if (*lpSubKey=='\\')  // the character is sub key separate character
			break;  // 余下的是其子键的内容
		dwlen++;
		lpSubKey++;  // to the next character
	}

	return dwlen;  // return the sub key length
}

/***********************************************************************/
// Create a Sub Key (lpSubKey) in the lpRegKey Key 
/***********************************************************************/
// **************************************************
// 声明：static LPREGKEY CreateNewSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey)
// 参数：
// 	IN hRegKey -- 指定的键结构
// 	IN lpSubKey -- 要创建的子键的名称
// 
// 返回值：返回创建的子键的结构
// 功能描述：在指定的键下创建一个以指定名称命名的子键。
// 引用: 
// **************************************************
static LPREGKEY CreateNewSubKey(LPREGKEY hRegKey,LPCTSTR lpSubKey)
{
	LPREGKEY hSubKey,hTailSubKey;

			hSubKey=(LPREGKEY)malloc(sizeof(REGKEY));  // Create New Sub Key
			if (hSubKey)
			{                  // Create Sub Key Success
				// 初始化键的结构
				hSubKey->bOpen=FALSE;
				hSubKey->lpClass=NULL;
				hSubKey->hNextKey=NULL;
				hSubKey->hRegValue=NULL;
				hSubKey->hSubKey=NULL;
				hSubKey->lpKeyName=AssignSubKeyname(lpSubKey);  // Copy the Sub Key Name
				if (hSubKey->lpKeyName==NULL)
				{  // 复制键名失败
					free(hSubKey);
					return NULL;
				}

				// Put the Sub Key to the tail
				if (hRegKey->hSubKey==NULL)
					hRegKey->hSubKey=hSubKey;  // 还没有子键，插到第一个
				else
				{
					hTailSubKey=GetTailSubKey(hRegKey->hSubKey);  // 得到最后一个子键的结构
					if (hTailSubKey)  // if the hTailSubKey ==NULL, then must have error in GetTailSubKey
						hTailSubKey->hNextKey=hSubKey;  // 插到最后一个子键的后面
				}
			}
			return hSubKey;  // 返回创建的子键结构
}


/***********************************************************************/
// go to the next sub Key 
/***********************************************************************/
// **************************************************
// 声明：static LPTSTR ToNextSubKey(LPCTSTR lpSubKey)
// 参数：
// 	IN lpSubKey -- 指定的注册键字串
// 
// 返回值：返回字串中下一个子键的位置
// 功能描述：得到键串中下一个子键的位置。
// 引用: 
// **************************************************
static LPTSTR ToNextSubKey(LPCTSTR lpSubKey)
{
	LPTSTR lpCurrent;

		lpCurrent=(LPTSTR)lpSubKey;  // 得到当前位置
		while(*lpCurrent)
		{
			if (*lpCurrent=='\\')
			{  // 得到注册键的分隔符
				lpCurrent++; // 到分隔符的下一个位置
				break;
			}
			lpCurrent++;  // 下一个字符
		}
		return lpCurrent;  // 返回下一个子键的位置
}

/***********************************************************************/
// alloc the Sub Key Name Space, and Copy the Sub Key Name in
/***********************************************************************/
// **************************************************
// 声明：static LPTSTR AssignSubKeyname(LPCTSTR lpSubKey)
// 参数：
// 	IN lpSubKey 指定的子键键名
// 
// 返回值： 返回新分配的键名
// 功能描述：分配并复制当前的键名
// 引用: 
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
// 声明：static LPREGKEY GetTailSubKey(LPREGKEY hSubKey)
// 参数：
// 	IN hSubKey -- 当前的子键的结构
// 
// 返回值：返回最后一个子键的结构
// 功能描述：得到最后一个子键。
// 引用: 
// **************************************************
static LPREGKEY GetTailSubKey(LPREGKEY hSubKey)
{
	while(1)
	{
		if (hSubKey==NULL)  // 当前子键为空
			return NULL;
		if (hSubKey->hNextKey==NULL)  // if the next key is NULL, then the Key is Tail
			return hSubKey;
		hSubKey=hSubKey->hNextKey;  // 到下一个子键
	}
}


/***********************************************************************/
// Set Open Status to the key
/***********************************************************************/
// **************************************************
// 声明：static BOOL OpenCurrentRegKey(LPREGKEY hRegKey)
// 参数：
// 	IN hRegKey -- 当前要打开的注册键的结构
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：打开指定的键。
// 引用: 
// **************************************************
static BOOL OpenCurrentRegKey(LPREGKEY hRegKey)
{
	if (hRegKey->bOpen==TRUE)
		return FALSE;   // Current Key Had Open , Can't Open again
	hRegKey->bOpen=TRUE;  // 设置打开的标志
	return TRUE;
}


/***********************************************************************/
// Will Delete Current key and it's Sub Key
/***********************************************************************/
// **************************************************
// 声明：static BOOL DeleteSubRegKey(LPREGKEY hRegKey,LPREGKEY hSubKey)
// 参数：
// 	IN hRegKey -- 当前指定的键
// 	IN hSubKey -- 要删除的子键
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：删除指定的子键
// 引用: 
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
// 声明：static void	FreeRegKey(LPREGKEY hKey)
// 参数：
// 	IN hKey -- 要释放内存的键
// 
// 返回值：无
// 功能描述：释放指定键的内容
// 引用: 
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
// 声明：static BOOL IsOpenKey(LPREGKEY hRegKey)
// 参数：
// 	IN hRegKey -- 指定的键
// 
// 返回值：如果键已经打开，则返回TRUE，否则返回FALSE。
// 功能描述：判断键是否已经打开。
// 引用: 
// **************************************************
static BOOL IsOpenKey(LPREGKEY hRegKey)
{
	return hRegKey->bOpen; // 返回当前键的打开状态
}


/***********************************************************************/
// Search the Value have exist in the key 
/***********************************************************************/
// **************************************************
// 声明：static LPREGVALUE SearchValue(LPREGKEY hRegKey,LPCTSTR lpValueName)
// 参数：
// 	IN hRegKey -- 指定的键
//	 IN lpValueName -- 键值名称
// 
// 返回值：返回找到的键值的结构
// 功能描述：在指定的键中查找指定的键值。
// 引用: 
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
// 声明：static LPREGVALUE SearchValueByIndex(LPREGKEY hRegKey, DWORD dwIndex)
// 参数：
// 	IN hRegKey -- 指定的键
// 	IN dwIndex -- 指定的索引
// 
// 返回值：返回找到的键值的结构
// 功能描述：在指定的键中查找指定索引的键值。
// 引用: 
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
// 声明：static LPREGVALUE CreateNewValue(LPREGKEY hRegKey,LPCTSTR lpValueName)  
// 参数：
// 	IN hRegKey -- 指定的键
// 	IN lpValueName -- 指定的键值名称
// 
// 返回值：成功返回创建的键值结构，否则返回NULL
// 功能描述：创建一个指定键值名称的键值
// 引用: 
// **************************************************
static LPREGVALUE CreateNewValue(LPREGKEY hRegKey,LPCTSTR lpValueName)  
{
	LPREGVALUE hRegValue,hTailValue;

		hRegValue=(LPREGVALUE)malloc(sizeof(REGVALUE));  // alloc a memory for register value
		if (hRegValue==NULL)
			return NULL;  // 分配失败
		hRegValue->lpData=NULL;  // Initialize the reg value
		hRegValue->cbData=0;
		hRegValue->dwType=REG_NONE;
		hRegValue->hNextValue=NULL;
		hRegValue->lpValueName=AssignRegValue(lpValueName);  // Set reg Value name to the value
		if (hRegValue->lpValueName==NULL)
		{ // 复制键值名称失败
			free(hRegValue);
			return NULL;
		}

		// Put the Value to the tail
		if (hRegKey->hRegValue==NULL)
			hRegKey->hRegValue=hRegValue;  // 这是该键下的第一个键值
		else
		{
			hTailValue=GetTailValue(hRegKey->hRegValue);  // Get the tail value
			if (hTailValue)  // if the hTailValue ==NULL, then must have error in GetTailValue
				hTailValue->hNextValue=hRegValue;  // 把新创建的键值加到最后一个键值后面
		}
		return hRegValue;  // 返回创建的键值
}


/***********************************************************************/
// alloc a memory ,and copy the Value name 
/***********************************************************************/
// **************************************************
// 声明：static LPTSTR AssignRegValue(LPCTSTR lpValueName)
// 参数：
// 	IN lpValueName -- 指定的键值名称
// 
// 返回值：返回新分配的键值名称
// 功能描述：分配并复制当前键值名称。
// 引用: 
// **************************************************
static LPTSTR AssignRegValue(LPCTSTR lpValueName)
{
	DWORD dwLen;
	LPTSTR lpNewValueName;

		dwLen=strlen(lpValueName);  // get the len of the sub key name
		lpNewValueName=(LPTSTR)malloc(dwLen+1);  // malloc a space include null-terminated
		if (lpNewValueName!=NULL)
		{  // 分配成功
			strcpy(lpNewValueName,(LPCTSTR)lpValueName);  // copy the value name 
		}
		return lpNewValueName;  // return the new value name
}


/***********************************************************************/
// alloc a memory ,and copy the Value name 
/***********************************************************************/
// **************************************************
// 声明：static LPREGVALUE GetTailValue(LPREGVALUE hRegValue)
// 参数：
// 	IN hRegValue -- 当前的键值
// 
// 返回值：返回最后一个键值的结构
// 功能描述： 得到最后一个键值。
// 引用: 
// **************************************************
static LPREGVALUE GetTailValue(LPREGVALUE hRegValue)
{
	while(1)
	{
		if (hRegValue==NULL) // 当前的键值为NULL
			return NULL; // 返回无效
		if (hRegValue->hNextValue==NULL)  // the next value is NULL, then is tail value
			return hRegValue;
		hRegValue=hRegValue->hNextValue;  // to the next value
	}
}


/***********************************************************************/
// Set New Data to the current value
/***********************************************************************/
// **************************************************
// 声明：static BOOL SetValue(LPREGVALUE hRegValue,DWORD dwType,const BYTE *lpData,DWORD cbData)
// 参数：
// 	IN hRegValue -- 当前的键值 
// 	IN dwType -- 数据的类型
// 	IN lpData -- 数据的指针
// 	IN cbData -- 数据的大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：设置指定键值的数据。
// 引用: 
// **************************************************
static BOOL SetValue(LPREGVALUE hRegValue,DWORD dwType,const BYTE *lpData,DWORD cbData)
{
		if (dwType==REG_SZ)
		{
			cbData = strlen(lpData) + 1;  // 需要结尾符
			hRegValue->lpData=(LPBYTE)malloc(cbData);  // alloc memory to save data
			if (hRegValue->lpData==NULL)
				return FALSE; // 分配数据缓存失败
		}
		else
		{
			hRegValue->lpData=(LPBYTE)malloc(cbData);  // alloc memory to save data
			if (hRegValue->lpData==NULL)
				return FALSE; // 分配数据缓存失败
		}
		hRegValue->dwType=dwType;  // save the data type
		hRegValue->cbData=cbData;  // save the data size
		memcpy((void *)hRegValue->lpData,(const void *)lpData,cbData);  // save data
		return TRUE;  // 设置成功
}


/***********************************************************************/
// Get Data from the value
/***********************************************************************/
// **************************************************
// 声明：static LONG GetValue(LPREGVALUE hRegValue,LPDWORD lpType,const BYTE *lpData,LPDWORD lpcbData)
// 参数：
// 	IN hRegValue -- 指定的键值
// 	OUT lpType -- 返回数据类型
// 	OUT lpData -- 数据缓存
// 	IN/OUT lpcbData -- 数据缓存的大小，并返回数据的大小
// 
// 返回值：成功返回ERROR_SUCCESS，否则有错。
// 功能描述：得到指定键值的数据。
// 引用: 
// **************************************************
static LONG GetValue(LPREGVALUE hRegValue,LPDWORD lpType,const BYTE *lpData,LPDWORD lpcbData)
{
		if (lpData!=NULL && lpcbData==NULL)  // if the lpdata is not NULL,then the lpcbData can't is 0
			return ERROR_INVALID_PARAMETER;
		if (lpData!=NULL)
		{                  // need read data
			if (hRegValue->dwType==REG_SZ)
			{  // 字串数据
				if (hRegValue->cbData>=*lpcbData)  // the buffer is enough ? ,*lpcbData must include Terminate-NULL
					return ERROR_MORE_DATA;  // the buffer is not enough
				memcpy((void *)lpData,(const void *)hRegValue->lpData,(hRegValue->cbData+1));  // read data to buffer from value,include Terminate
				*lpcbData=hRegValue->cbData;  // read the data size
			}
			else
			{  // 其他数据
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
// 声明：static LONG DeleteValue(LPREGKEY hRegKey,LPREGVALUE hRegValue)
// 参数：
// 	IN hRegKey -- 指定的键
// 	IN hRegValue -- 要删除的键值
// 
// 返回值：成功返回ERROR_SUCCESS ，否则有错。
// 功能描述：删除指定的键值。
// 引用: 
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
				return ERROR_KEY_DELETED;  // 指定的键值不存在
		}
	}

	FreeRegValue(hRegValue);  // free memory of the delete value 
	return ERROR_SUCCESS;
}


/***********************************************************************/
// free current value
/***********************************************************************/
// **************************************************
// 声明：static void	FreeRegValue(LPREGVALUE hRegValue)
// 参数：
// 	IN hRegValue -- 指定的键值
// 
// 返回值：无
// 功能描述：释放指定的键值。
// 引用: 
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
// 声明：static void QueryInfoSubKey(LPREGKEY hRegKey,LPDWORD lpcSubKeys,LPDWORD lpcbMaxSubKeyLen,LPDWORD lpcbMaxClassLen)
// 参数：
// 	IN hRegKey -- 指定的键
// 	OUT lpcSubKeys -- 子键的个数
// 	OUT lpcbMaxSubKeyLen -- 子键的键名的最大长度
// 	OUT lpcbMaxClassLen -- 子键的类名的最大长度
// 
// 返回值：无
// 功能描述：查询指定的键的子键的信息。
// 引用: 
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
// 声明：static void QueryInfoValue(LPREGKEY hRegKey,LPDWORD lpcValues,LPDWORD lpcbMaxValueNameLen,LPDWORD lpcbMaxValueLen)
// 参数：
// 	IN hRegKey -- 指定的键
// 	OUT lpcValues -- 键值的树目
// 	OUT lpcbMaxValueNameLen -- 最大的键值的名称的长度
// 	OUT lpcbMaxValueLen -- 最大的数据长度
// 
// 返回值：无
// 功能描述：查询指定键的键值的信息。
// 引用: 
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

			hRegValue=hRegValue->hNextValue;  // 到下一个键值
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
// 声明：BOOL LoadRegister(LPTSTR lpRegisterFile)
// 参数：
// 	IN lpRegisterFile -- 注册表文件
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：
// 引用: 
// **************************************************
BOOL LoadRegister(LPTSTR lpRegisterFile)
{
	DWORD dwType;
	HKEY hKey=NULL;
	DWORD dwDisposition;
	
		// Open the register file
		g_hReadFile=CreateRegFile(lpRegisterFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (g_hReadFile==INVALID_HANDLE_VALUE)
			return FALSE;  // 打开文件失败
		linebuffer=(LPTSTR)malloc(size);  // 分配空间
		if (linebuffer==NULL)
			return FALSE;  // the memory is not enough, must return 
		while(1)
		{
			if (ReadLine(g_hReadFile,linebuffer,size)==FALSE)  // 读取一行的数据
			{
				RETAILMSG(1,("Read Line Failure \r\n"));
				break;
			}
//			RETAILMSG(1,("Read Line <%s> \r\n",linebuffer));
			dwType=GetLineType(linebuffer);  // 得到当前行数据的类型
			if (dwType==TYPE_SUBKEY)
			{  // 属于定义子键
				HKEY hRootKey;
				LPTSTR lpSubKey=NULL;

					if (hKey)
					{ // 当前有键被打开
						FileReg_CloseKey(hKey);  // 关闭注册键
						hKey=NULL;
					}
				    
					if (AnalyseSubKeyLine(linebuffer,&hRootKey,&lpSubKey)==FALSE)  // 分析得到该行数据的根键和子键
						break;
					// will create this sub key,and open it
					FileReg_CreateKeyEx(hRootKey,lpSubKey,0,NULL,0,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition); // 创建子键
					if (lpSubKey)  // 释放子键的缓存
						free(lpSubKey);
			}
			else if (dwType==TYPE_VALUE)
			{ // 数据是键值
				LPTSTR lpValueName,lpData;
				DWORD dwValueType,cbData;
					// 分析数据，得到键值名称，数据类型，数据及数据大小
					if (AnalyseValueLine(linebuffer,&lpValueName,&dwValueType,&lpData,&cbData)==FALSE)
						break;
					// will create this value in the open key
					// 在已经打开的键下创建键值
					FileReg_SetValueEx(hKey,lpValueName,0,dwValueType,(const BYTE *)lpData,cbData);
					if (lpValueName)  // 释放键值的名称
						free(lpValueName);
					if (lpData)  // 释放键值的数据
						free(lpData);
			}
		}
		if (hKey)
		{  // 关闭已经打开的键
			FileReg_CloseKey(hKey);
			hKey=NULL;
		}
		if (linebuffer)  // 释放存取数据的缓存
			free(linebuffer);
		CloseRegHandle(g_hReadFile); // 关闭文件句柄
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
// 声明：static BOOL ReadLine(HANDLE handle,LPTSTR buffer,DWORD size)
// 参数：
// 	IN handle -- 文件句柄
// 	OUT buffer -- 存储数据的缓存
// 	IN size -- 缓存的大小
// 
// 返回值： 成功返回TRUE，否则返回FALSE
// 功能描述：从打开的文件中读取一行数据。
// 引用: 
// **************************************************
static BOOL ReadLine(HANDLE handle,LPTSTR buffer,DWORD size)
{
	DWORD dwCurPos=0;

	while(1)
	{
		if (dwPos>=dwbuffersize)
		{ // the cache had tail,must read cache data
			// 当前的数据指针已经到达CACHE的结尾，需要重新读取数据
			ReadRegFile(handle,cachebuffer,CACHEBUFFERSIZE,&dwbuffersize,NULL);  // 从文件中读取CACHE大小的数据到CACHE
			dwPos=0; // 当前的数据指针回0
			if (dwbuffersize==0)
			{// the file had eof，已经没有数据
				if (dwCurPos!=0)
				{ // 已经读取了数据
					buffer[dwCurPos]=0; // set buffer null 
					return TRUE;
				}
				else
				{ // 没有读到数据
					buffer[dwCurPos]=0; // set buffer null 
					return FALSE;
				}
			}
		}
		if (cachebuffer[dwPos]==0x0d && cachebuffer[dwPos+1]==0x0a)
		{  // 是回车符
			// Is the line over,may return
			buffer[dwCurPos]=0;  // set buffer null
			dwPos+=2; // to the next line start
			return TRUE;
		}
		buffer[dwCurPos++]=cachebuffer[dwPos++]; // read a byte to the buffer
		if (dwCurPos>=size)  // 数据缓存不能放下该行的数据
			return FALSE;  // over flow the buffer
	}
}

/***********************************************************************/
// get line type,eg: sub key,value,empty and remark
/***********************************************************************/
// **************************************************
// 声明：static DWORD GetLineType(LPTSTR buffer)
// 参数：
// 	IN buffer -- 当前行的数据缓存
// 
// 返回值：返回当前行的数据类型
// 功能描述：得到当前行的数据类型
// 引用: 
// **************************************************
static DWORD GetLineType(LPTSTR buffer)
{
	LPTSTR lpCurPos;

		lpCurPos=buffer;
		while(1)
		{  // jump the start space，最前面的空格为无效字符
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
// 声明：static BOOL AnalyseSubKeyLine(LPTSTR buffer,PHKEY lphRootKey,LPTSTR *lppSubKey)
// 参数：
// 	IN buffer -- 当前的数据
// 	OUT lphRootKey -- 返回跟键的句柄
// 	OUT lppSubKey -- 返回子键的名称
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：分析一个注册键的数据行，是用来定义下面的键值是属于哪一个键。
// 引用: 
// **************************************************
static BOOL AnalyseSubKeyLine(LPTSTR buffer,PHKEY lphRootKey,LPTSTR *lppSubKey)
{
	LPTSTR lpCurPos;
	DWORD dwLen;
		
		if (lphRootKey==NULL || lppSubKey==NULL)
			return FALSE; // 参数错误

		lpCurPos=buffer;  // 得到当前数据的起始指针
		while(1)
		{  // jump the start space ，调过前面的空格
			if (!(*lpCurPos==' ' || *lpCurPos==TABCODE))
				break;
			if (*lpCurPos==0)
				return FALSE;
			lpCurPos++;
		}
		if (*lpCurPos!='[')  // the Sub key start code is '[',otherwise is wrong
			return FALSE;  // 错误数据

		// first apart the root key
		lpCurPos++; // jump '['
		dwLen=GetSubKeyLen(lpCurPos); // 得到键的长度
#ifdef ROOT_MICROSOFT
		if (strnicmp(lpCurPos,"HKEY_CLASSES_ROOT",dwLen)==0)
		{  // 根键是HKEY_CLASSES_ROOT
			*lphRootKey=HKEY_CLASSES_ROOT;
		}
		else if(strnicmp(lpCurPos,"HKEY_CURRENT_USER",dwLen)==0)
		{  // 根键是HKEY_CURRENT_USER
			*lphRootKey=HKEY_CURRENT_USER;
		}
		else if(strnicmp(lpCurPos,"HKEY_LOCAL_MACHINE",dwLen)==0)
		{  // 根键是HKEY_LOCAL_MACHINE
			*lphRootKey=HKEY_LOCAL_MACHINE;
		}
		else if(strnicmp(lpCurPos,"HKEY_USERS",dwLen)==0)
		{  // // 根键是HKEY_USERS
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
		{  // 根键是HKEY_HARDWARE_ROOT
			*lphRootKey=HKEY_HARDWARE_ROOT;
		}
		else if (strnicmp(lpCurPos,"HKEY_SOFTWARE_ROOT",dwLen)==0)
		{  // // 根键是HKEY_SOFTWARE_ROOT
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
// 声明：static BOOL AnalyseValueLine(LPTSTR buffer,LPTSTR *lppValueName,LPDWORD lpdwValueType,LPTSTR *lppData,LPDWORD lpcbData)
// 参数：
// 	IN buffer -- 当前的数据
// 	OUT lppValueName -- 返回键值的名称
// 	OUT lpdwValueType -- 返回数据类型
// 	OUT lppData -- 返回数据
// 	OUT lpcbData -- 返回数据的大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：分析定义键值数据的数据。
// 引用: 
// **************************************************
static BOOL AnalyseValueLine(LPTSTR buffer,LPTSTR *lppValueName,LPDWORD lpdwValueType,LPTSTR *lppData,LPDWORD lpcbData)
{
	LPTSTR lpCurPos;
	DWORD dwLen;

		// Parameter check
		if (lpdwValueType==NULL || lppValueName==NULL || lppData==NULL || lpcbData==NULL)  
			return FALSE;  // 参数错误

		//Initialize the parameter
		*lpdwValueType=REG_NONE;
		*lppValueName=NULL;
		*lppData=NULL;
		*lpcbData=0;  

		lpCurPos=buffer;
		while(1)
		{  // jump the start space ，调过最前面的无效数据
			if (!(*lpCurPos==' ' || *lpCurPos==TABCODE))
				break;
			if (*lpCurPos==0)
				return FALSE;
			lpCurPos++;
		}
		if (*lpCurPos!='\"' && *lpCurPos!='@')  // the value Start Code is \" and @ ,otherwise is wrong
			return FALSE;  // 错误数据
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
// 声明：static DWORD GetValueType(LPTSTR lpCurPos,LPDWORD lpdwValueType)
// 参数：
// 	IN lpCurPos -- 当前的数据
// 	OUT lpdwValueType -- 返回当前数据的数据类型
// 
// 返回值：返回数据类型的长度
// 功能描述：得到当前数据 的数据类型。
// 引用: 
// **************************************************
static DWORD GetValueType(LPTSTR lpCurPos,LPDWORD lpdwValueType)
{
	DWORD dwLen=0;
	int i=0;

		if (*lpCurPos=='\"')  // the value data is string
		{  // 是字符串数据
			*lpdwValueType=REG_SZ;
			return 0;
		}
		else
		{
			while(1)
			{
				if (ValueTypeTab[i].lpTypeName==NULL)
				{  // 没有匹配的数据类型，可能是不支持的数据类型
					*lpdwValueType=REG_NONE;
					return 0;
				}
				if (strnicmp(lpCurPos,ValueTypeTab[i].lpTypeName,ValueTypeTab[i].dwTypenameLen)==0)
				{ // 找到指定的数据类型
					*lpdwValueType=ValueTypeTab[i].dwType;
					return ValueTypeTab[i].dwTypenameLen;  // 返回数据类型的数据长度
				}
				i++;
			}
		}
}

/***********************************************************************/
// get the value data and data length
/***********************************************************************/
// **************************************************
// 声明：static BOOL GetValueData(LPTSTR lpCurPos,DWORD dwType,LPTSTR *lppData,LPDWORD lpcbData)
// 参数：
// 	IN lpCurPos -- 当前的数据
// 	IN dwType -- 当前的数据类型
// 	OUT lppData -- 保留当前数据的缓存
// 	OUT lpcbData -- 当前数据的长度
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到键值恶毒数据及长度
// 引用: 
// **************************************************
static BOOL GetValueData(LPTSTR lpCurPos,DWORD dwType,LPTSTR *lppData,LPDWORD lpcbData)
{
	DWORD dwLen;
	if (dwType==REG_SZ)
	{  // 是字符串数据
		dwLen=0;
		lpCurPos++;  // skip the string start code \"
		while(1)
		{   // 得到数据长度
			if (*(lpCurPos+dwLen)=='\"')  // Is string End code \"
				break; // 数据结束
			if (*(lpCurPos+dwLen)==0)  // if the line is tail, but no string end code \",then is Invalid Line
				return FALSE; // 没有结束符，错误数据
			dwLen++;
		}
		if (lppData!=NULL)
		{
			*lppData=(LPTSTR)malloc(dwLen+1);  // 分配用于保存数据的缓存
			if (*lppData==NULL)  // the memory is not enough
				return FALSE;
			memcpy(*lppData,lpCurPos,dwLen);  // 复制数据
			(*lppData)[dwLen]=0;  // add the terminate null
			*lpcbData=dwLen+1;  // include terminate
			return TRUE;
		}
		return FALSE;
	}
	if (dwType==REG_DWORD)
	{ // 是DWORD数据
		dwLen=0;
		lpCurPos++;  // skip the DWORD type start code ':'
		while(1)
		{  // 得到数据长度
			if (*(lpCurPos+dwLen)==0)  // if the line is tail, but no string end code \",then is Invalid Line
				break;
			dwLen++;
		}
		if (lppData!=NULL)
		{
			LPTSTR lpTemp;
			DWORD dwValue;
			lpTemp=(LPTSTR)malloc(dwLen+1);  // 分配存放数据的缓存
			if (lpTemp==NULL)  // the memory is not enough
				return FALSE;
			memcpy(lpTemp,lpCurPos,dwLen);  // 得到数据
			lpTemp[dwLen]=0;  // add the terminate null

			*lppData=(LPTSTR)malloc(sizeof(DWORD));  // 分配存放DWORD的缓存
			if (*lppData==NULL)  // the memory is not enough
				return FALSE;
			//dwValue=atoi(lpTemp);
			dwValue=strtoul( lpTemp, NULL, 16);  // 将数据转化为DWORD
			*(DWORD*)(*lppData)=dwValue;//atoi(lpTemp); // 设置数据
			*lpcbData=sizeof(DWORD); // 设置数据长度
			free(lpTemp);  // 释放中间缓存
			return TRUE;
		}
		return FALSE;
	}
	if (dwType==REG_BINARY)
	{  // 是二进制数据
		LPTSTR lpTemp;
		TCHAR lpChar[16];
		DWORD dwChar=0;

		dwLen=0;

		lpCurPos++;  // skip the DWORD type start code ':'
		lpTemp=(LPTSTR)malloc(MAXBINDATALEN);  // 分配可支持的最大的二进制数据的缓存
		if (lpTemp==NULL)  // 分配失败
			return FALSE;
		while(1)
		{  // 得到数据
			if (*lpCurPos==0)  // if the line is tail, but no string end code \",then is Invalid Line
			{  // 数据结束
				lpChar[dwChar]=0;
				//lpTemp[dwLen]=atoi(lpChar);  // get a data
				lpTemp[dwLen]=(char)strtoul( lpChar, NULL, 16); // 将数据保存到数组

				dwChar=0;
				dwLen++;  // 增加一个数字
				break;
			}
			if (*lpCurPos==',')
			{ // 数字分隔符
				lpChar[dwChar]=0;
				//lpTemp[dwLen]=atoi(lpChar);  // get a data
				lpTemp[dwLen]=(char)strtoul( lpChar, NULL, 16);  // 转化并保存当前数字

				dwChar=0;
				dwLen++;  // 增加一个数字
				lpCurPos++;
				continue;
			}
			if (*lpCurPos=='\\')
			{  // 要求读取下一行
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
			lpChar[dwChar]=*lpCurPos;  // 读取数据
			dwChar++;
			if (dwChar>=16)  // The max char len
			{  // 错误数据
				free(lpTemp);
				return FALSE;
			}
			lpCurPos++;
		}
		if (lppData!=NULL)
		{
			*lppData=(LPTSTR)malloc(dwLen);  // 分配数据缓存
			if (*lppData==NULL)  // the memory is not enough
				return FALSE;
			memcpy(*lppData,lpTemp,dwLen);  // 将得到的数据复制到数据缓存
			*lpcbData=dwLen; // 设置数据长度
			free(lpTemp); // 释放临时缓存
			return TRUE; // 返回成功
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
// 声明：BOOL SaveAsRegister(LPTSTR lpRegisterFile)
// 参数：
// 	IN lpRegisterFile -- 指定的文件名
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：保存当前的注册表到指定的文件中。
// 引用: 
// **************************************************
BOOL SaveAsRegister(LPTSTR lpRegisterFile)
{
	LPREGKEY hRegKey;
	int i;
	HKEY hKey;
	HANDLE hFile;

		// 打开指定的文件
		hFile=CreateRegFile(lpRegisterFile,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE)
			return FALSE;  // 打开文件失败

		for (i=0;i<ROOTKEYNUM;i++)
		{
			hKey=RootKeyTab[i];  // 得到指定的根键
			hRegKey=MapRootKey(hKey);  // map the root key,if is not root key ,
										// then return the key
			if (hRegKey==NULL)
				return ERROR_BADKEY;  // 这是一个错误的根键
			
			WriteKeyToFile(hFile,hRegKey,NULL);  // 将根键下的内容写到文件中
		}	
		CloseRegHandle(hFile);  // 关闭文件
		return TRUE;
}

/***********************************************************************/
// Will Write the Key (include the SubKey) to the file
/***********************************************************************/
// **************************************************
// 声明：static BOOL WriteKeyToFile(HANDLE hFile,LPREGKEY hRegKey,LPTSTR lpKeyName)
// 参数：
// 	IN hFile -- 文件句柄
// 	IN hRegKey -- 当前的键结构
// 	IN lpKeyName -- 父键的名称
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：将指定的键写到文件中。
// 引用: 
// **************************************************
static BOOL WriteKeyToFile(HANDLE hFile,LPREGKEY hRegKey,LPTSTR lpKeyName)
{
	DWORD dwIndex=0;
	LPTSTR lpCurKeyName;
	DWORD dwKeyNameLen;
	LPREGKEY hSubKey;
	
		// 得到当前键的长度，当前键是包括父键和自己键名的长度
		if (lpKeyName)
		{  // 有父键  
			dwKeyNameLen=strlen(lpKeyName)+strlen(hRegKey->lpKeyName)+1+1;  // 1-->key space mark '\' ,1-->terminate code 
		}
		else
		{ // 没有父键
			dwKeyNameLen=strlen(hRegKey->lpKeyName)+1;  //1-->terminate code 
		}
		lpCurKeyName=(LPTSTR)malloc(dwKeyNameLen);  // 分配当前键的长度
		if (lpCurKeyName==NULL)
			return FALSE;  // 分配内存失败
		//sprintf(lpCurKeyName,"%s\\%s",lpKeyName,hKey->lpKeyName);
		// 得到当前键
		if (lpKeyName)
		{ // 有父键
			strcpy(lpCurKeyName,lpKeyName);  // 复制父键的内容
			strcat(lpCurKeyName,"\\"); // 添加分隔符
			strcat(lpCurKeyName,hRegKey->lpKeyName); // 添加自己的键名
		}
		else
		{ // 没有父键
			strcpy(lpCurKeyName,hRegKey->lpKeyName);  // 复制自己的键名
		}
		while(1)
		{// 将子键写到文件
			hSubKey=SearchSubKeyByIndex(hRegKey,dwIndex);  // Search the specical index is exist in the RegKey
			if (hSubKey==NULL)  // the Sub Key is not Exist ,have no more subkey
			{ // 已经没有子键了
				break;
			}
			WriteKeyToFile(hFile,hSubKey,lpCurKeyName);  // 写键的内容到文件
			dwIndex++;  // 下一个子键
		}
		if (hRegKey->hRegValue)
		{ // 当前键有键值，将键值写到文件
			WriteRegFile(hFile,"[",1,&dwKeyNameLen,NULL);  // 写入键名的开始标志
			WriteRegFile(hFile,lpCurKeyName,strlen(lpCurKeyName),&dwKeyNameLen,NULL);// 写入键名
			WriteRegFile(hFile,"]\r\n",3,&dwKeyNameLen,NULL); // 写入键名的结束标志
			//
			// Will Write the Key Value
			WriteValueToFile(hFile,hRegKey);  // 写入键值到文件
			WriteRegFile(hFile,"\r\n",2,&dwKeyNameLen,NULL); // 添加回车符
		}
		// 写入完成
		free(lpCurKeyName);  // 释放当前的键名
		return TRUE;
}


/***********************************************************************/
// Will Write the Key Value to the file
/***********************************************************************/
// **************************************************
// 声明：static BOOL WriteValueToFile(HANDLE hFile,LPREGKEY hRegKey)
// 参数：
// 	IN hFile -- 文件句柄
// 	IN hRegKey -- 要写入键值的键的结构
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
static BOOL WriteValueToFile(HANDLE hFile,LPREGKEY hRegKey)
{
	LPREGVALUE hRegValue;
	DWORD dwWriteLen=0;

	hRegValue=hRegKey->hRegValue;  // 得到第一个键值
	while(hRegValue)
	{  // 还有键值
		// Write the Value Name  eg.   "abcd"=
		WriteRegFile(hFile,"    ",4,&dwWriteLen,NULL); // 写入4个空格用于美观
		WriteRegFile(hFile,"\"",1,&dwWriteLen,NULL);  // 写入键值的名称的开始标志
		WriteRegFile(hFile,hRegValue->lpValueName,strlen(hRegValue->lpValueName),&dwWriteLen,NULL); // 写入简直的名称
		WriteRegFile(hFile,"\"=",2,&dwWriteLen,NULL); // 写入键值的名称的结束标志

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
// 声明：BOOL InitRegistry( void )
// 参数：
// 	无
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：初始化注册表
// 引用: 
// **************************************************
BOOL InitRegistry( void )
{
	// RETAILMSG( 1, ( "Load system registry.\r\n" ) );
#if 1
	if (LoadRegister( NULL ))  // 从FLASH中装载注册表
	{
		return TRUE;
	}
#endif	
	return LoadRegister( SYSTEMREGISTERFILE );  // 装载系统注册表文件
}

// **************************************************
// 声明：BOOL DeinitRegistry( void )
// 参数：
// 	无
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：退出注册表
// 引用: 
// **************************************************
BOOL DeinitRegistry( void )
{
	//RETAILMSG( 1, ( "Load system registry.\r\n" ) );
//	return SaveAsRegister( SYSTEMREGISTERFILE );  // 保存当前内容到系统注册表文件
	return SaveAsRegister( NULL );  // 保存当前内容到系统注册表数据区
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
#define FLASHREGFLAG	"JREG"  // 注册表标志
//  一个存放在FLASH 注册表区的头结构
typedef struct structFlashHeader{
		TCHAR chFlag[4]; // 存放标志“JREG”
		DWORD dwDataSize; // 存放数据大小
		DWORD dwRead; //已经读取的数据
		DWORD dwStatus;  // 当前状态 ， 1 -- 读数据 ， 2 -- 写数据
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
			return INVALID_HANDLE_VALUE; // 返回无效句柄
		}

		g_curFlashHeader = (LPFLASHHEADER)malloc(sizeof(FLASHHEADER)); // 分配一个空间来存放当前要读取的数据的大小
		if (g_curFlashHeader == NULL )
			return INVALID_HANDLE_VALUE; // 返回无效句柄
		if (dwAccess & GENERIC_READ)
		{
//			RETAILMSG(1,("Need Read Register Data ...\r\n"));
			// 要求从FLASH中读取
			if (Sys_RegistryIO( (LPBYTE)g_curFlashHeader, sizeof(FLASHHEADER), REGIO_READ | REGIO_BEGIN ) == FALSE)// 读取头信息
			{
				// 读取头信息失败
				free(g_curFlashHeader);
				g_curFlashHeader = NULL;
				RETAILMSG(1,(TEXT("!!!Read Head failure\r\n")));
				return INVALID_HANDLE_VALUE; // 返回无效句柄
			}
//			RETAILMSG(1,(TEXT("g_curFlashHeader->chFlag = %s\r\n"),g_curFlashHeader->chFlag));
			if (strncmp(g_curFlashHeader->chFlag,FLASHREGFLAG,4) != 0)
			{
				Sys_RegistryIO( NULL, 0, REGIO_END); // 关闭数据信息
				free(g_curFlashHeader);
				g_curFlashHeader = NULL;
				RETAILMSG(1,(TEXT("!!!error data, no data\r\n")));
				return INVALID_HANDLE_VALUE; // 返回无效句柄
			}
			g_curFlashHeader->dwRead = 0; // 还没有开始读取数据
			g_curFlashHeader->dwStatus = 1; // 读数据
		}
		else if (dwAccess & GENERIC_WRITE)
		{
			//要求写到FLASH
//			RETAILMSG(1,("Need Write Register Data ...\r\n"));
			strncpy(g_curFlashHeader->chFlag,FLASHREGFLAG,4);
			g_curFlashHeader->dwRead  = 0;
			g_curFlashHeader->dwDataSize = 0;
			if (Sys_RegistryIO( (LPBYTE)g_curFlashHeader, sizeof(FLASHHEADER), REGIO_WRITE | REGIO_BEGIN ) == FALSE) // 写入头信息
			{
				// 写入头信息失败
				free(g_curFlashHeader);
				g_curFlashHeader = NULL;
				RETAILMSG(1,(TEXT("Write Head failure\r\n")));
				return INVALID_HANDLE_VALUE; // 返回无效句柄
			}
			g_curFlashHeader->dwStatus = 2; // 写数据
		}
		// 正确处理
//		RETAILMSG(1,("Open Register OK !!!!!!!!!\r\n"));
		return (HANDLE)FLASHFILEHANDLE;
	}
	else
	{
		// 一般文件
		return CreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
	}
}


static BOOL ReadRegFile( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped )
{
	if (hFile == (HANDLE)FLASHFILEHANDLE)
	{
		// 从flash中读取
		if (g_curFlashHeader == NULL)
		{
			RETAILMSG(1,(TEXT("error Header pointer\r\n")));
			return FALSE;  // 当前的头结构错误
		}
		if (dwNumToRead > g_curFlashHeader->dwDataSize - g_curFlashHeader->dwRead)
		{
			// 已经没有太多的数据
			dwNumToRead = g_curFlashHeader->dwDataSize - g_curFlashHeader->dwRead;

			if (dwNumToRead ==0)
			{
				// 已经没有数据了
				if (lpdwNumRead)
					*lpdwNumRead = 0;
				return FALSE;
			}
		}
		if (Sys_RegistryIO( (LPBYTE)lpvBuffer,dwNumToRead, REGIO_READ) == FALSE) // 写入头信息
		{
			// 读取数据失败
			RETAILMSG(1,(TEXT("Read Data failure\r\n")));
			return FALSE; // 返回无效句柄
		}
//		RETAILMSG(1,(TEXT("Read Data  = %s\r\n"),lpvBuffer));
		g_curFlashHeader->dwRead += dwNumToRead; // 当前的数据正确读取

		return TRUE;
	}
	else
	{
		// 正常读取文件
		return ReadFile( hFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
	}
}


static BOOL WriteRegFile( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped )
{
	if (hFile == (HANDLE)FLASHFILEHANDLE)
	{
//		RETAILMSG(1,("%s",lpvBuffer));
		// 写入到flash
		if (g_curFlashHeader == NULL)
		{
			RETAILMSG(1,(TEXT("error Header pointer\r\n")));
			return FALSE;  // 当前的头结构错误
		}
		if (Sys_RegistryIO( (LPBYTE)lpvBuffer, dwNumToWrite, REGIO_WRITE) == FALSE) // 写入头信息
		{
			// 写入数据失败
			RETAILMSG(1,(TEXT("Write Data failure\r\n")));
			return FALSE; // 返回无效句柄
		}
		if ( lpdwNumWrite != NULL)
		{
			*lpdwNumWrite = dwNumToWrite;
		}
		g_curFlashHeader->dwDataSize += dwNumToWrite; // 当前的数据正确写入

		return TRUE;
	}
	else
	{
		// 正常写入文件
		return WriteFile( hFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
	}
}


static BOOL CloseRegHandle(HANDLE  hHandle)
{
	if (hHandle == (HANDLE)FLASHFILEHANDLE)
	{
//		RETAILMSG(1,(TEXT("Close Flash Registry Handle\r\n")));
		// 关闭FALSH 数据
		Sys_RegistryIO( NULL, 0, REGIO_END); // 关闭数据信息
		// 如果是写数据，必须重新将头结构写入
		if (g_curFlashHeader->dwStatus == 2) // 写数据
		{
			Sys_RegistryIO( (LPBYTE)g_curFlashHeader, sizeof(FLASHHEADER), REGIO_WRITE | REGIO_BEGIN ); // 写入头信息
			Sys_RegistryIO( NULL, 0, REGIO_END); // 关闭数据信息
		}
		free(g_curFlashHeader); // 释放头结构
		g_curFlashHeader = NULL;
		return TRUE;
	}
	else
	{
		// 正常读取文件
		return CloseHandle( hHandle );
	}
}

