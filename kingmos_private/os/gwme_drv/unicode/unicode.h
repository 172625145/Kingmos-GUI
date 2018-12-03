/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __UNICODE_H
#define __UNICODE_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

// ********************************************************************
//声明：DWORD Create( DWORD dwReserver );
//参数：
//	IN dwReserver - 保留
//返回值：
//	该driver的创建句柄
//功能描述：创建(打开)CodePage
//引用: 
// ********************************************************************

typedef DWORD ( * LPCREATE )( DWORD dwReserver );

// ********************************************************************
//声明：DWORD Delete( DWORD dwHandle );
//参数：
//	IN dwHandle - 由 Create 返回的值
//返回值：
//	成功， TRUE; 失败, FALSE
//功能描述：删除打开的CodePage
//引用: 
// ********************************************************************

typedef BOOL ( * LPDELETE )( DWORD dwHandle );

// ********************************************************************
//声明：int GetUnicode( DWORD dwHandle, 
//                       LPCSTR lpMultiByteStr,
//                       int cbMultiByte,
//                       LPWSTR lpWideCharStr,
//                       int cchWideChar  );
//参数：
//	IN dwHandle - 由 Create 返回的值
//  IN lpMultiByteStr-字符串
//  IN cbMultiByte-需要转换的字符数
//  IN lpWideCharStr-保存Unicode的缓存
//  IN cchWideChar-保存Unicode的缓存大小(以wide-char为单位)
//返回值：
//	写入缓存的wide-char字符数(以wide-char为单位)
//功能描述：根据字符lpMultiByteStr 得到对应的Unicode值
//引用: 
// ********************************************************************

typedef DWORD ( * LPGETUNICODE )( DWORD dwHandle,
								  LPCSTR lpMultiByteStr,
								  int cbMultiByte,  // if 0, get size need
								  LPWSTR lpWideCharStr,
								  int cchWideChar );
// ********************************************************************
//声明：int GetChar( DWORD dwHandle,
//							  LPCWSTR lpWideCharStr, 
//							  int cchWideChar, 
//							  LPSTR lpMultiByteStr, 
//							  int cbMultiByte, 
//							  LPCSTR lpDefaultChar, 
//							  LPBOOL lpUsedDefaultChar );
//参数：
//	IN dwHandle - 由 Create 返回的值
//  IN lpWideCharStr-Unicode字符串
//  IN cchWideChar-需要转换的Unicode字符数
//  IN lpMultiByteStr-保存字符的缓存
//  IN cchWideChar-保存字符的缓存大小(以byte为单位)
//  IN lpDefaultChar-当无法转换时，所填入的字符串.(可以为NULL)
//  OUT lpUsedDefaultChar-当有无法转换的, 将设置为TRUE;否则，将设置为FALSE.(可以为NULL)
//返回值：
//	写入缓存的byte-char字符数(以byte为单位)
//功能描述：根据字符lpWideCharStr 得到对应的char值
//引用: 
// ********************************************************************

typedef int ( * LPGETCHAR )( DWORD dwHandle,
							  LPCWSTR lpWideCharStr, 
							  int cchWideChar, 
							  LPSTR lpMultiByteStr, 
							  int cbMultiByte, 
							  LPCSTR lpDefaultChar, 
							  BOOL * lpUsedDefaultChar );

// ********************************************************************
//声明：int CountBufferSize( DWORD dwHandle,
//					           LPCVOID lpcvStr,
//					           int cbCountLen,
//						       BOOL IsWideChar );
//参数：
//	IN dwHandle - 由 Create 返回的值
//  IN lpcvStr-字符串指针
//  IN cbCountLen-需要统计的字符串长度(依赖于IsWideChar)
//  IN IsWideChar-指明lpcvStr以及cbCountLen是否是wide-char;
//                假如是，lpcvStr是LPCSTR并且cbCountLen是bytes;
//                假如是，lpcvStr是LPCWSTR并且cbCountLen是wide-chars;
//返回值：
//	需要的buffer大小（byte单位 or wide-char单位）
//功能描述：得到需要的buffer大小
//引用: 
// ********************************************************************

typedef int ( * LPCOUNTBUFFERSIZE )( DWORD dwHandle,
							           LPCVOID lpStr,
							           int cbCountLen,
								       BOOL IsWideChar );
							  

typedef struct _CODE_PAGE_DRV
{
	LPCREATE lpCreate;
	LPDELETE lpDelete;
	LPGETUNICODE lpGetUnicode;
	LPGETCHAR lpGetChar;
	LPCOUNTBUFFERSIZE lpCountBufferSize;
}CODE_PAGE_DRV, * PCODE_PAGE_DRV;


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__UNICODE_H
