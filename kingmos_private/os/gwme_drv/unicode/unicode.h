/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __UNICODE_H
#define __UNICODE_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

// ********************************************************************
//������DWORD Create( DWORD dwReserver );
//������
//	IN dwReserver - ����
//����ֵ��
//	��driver�Ĵ������
//��������������(��)CodePage
//����: 
// ********************************************************************

typedef DWORD ( * LPCREATE )( DWORD dwReserver );

// ********************************************************************
//������DWORD Delete( DWORD dwHandle );
//������
//	IN dwHandle - �� Create ���ص�ֵ
//����ֵ��
//	�ɹ��� TRUE; ʧ��, FALSE
//����������ɾ���򿪵�CodePage
//����: 
// ********************************************************************

typedef BOOL ( * LPDELETE )( DWORD dwHandle );

// ********************************************************************
//������int GetUnicode( DWORD dwHandle, 
//                       LPCSTR lpMultiByteStr,
//                       int cbMultiByte,
//                       LPWSTR lpWideCharStr,
//                       int cchWideChar  );
//������
//	IN dwHandle - �� Create ���ص�ֵ
//  IN lpMultiByteStr-�ַ���
//  IN cbMultiByte-��Ҫת�����ַ���
//  IN lpWideCharStr-����Unicode�Ļ���
//  IN cchWideChar-����Unicode�Ļ����С(��wide-charΪ��λ)
//����ֵ��
//	д�뻺���wide-char�ַ���(��wide-charΪ��λ)
//���������������ַ�lpMultiByteStr �õ���Ӧ��Unicodeֵ
//����: 
// ********************************************************************

typedef DWORD ( * LPGETUNICODE )( DWORD dwHandle,
								  LPCSTR lpMultiByteStr,
								  int cbMultiByte,  // if 0, get size need
								  LPWSTR lpWideCharStr,
								  int cchWideChar );
// ********************************************************************
//������int GetChar( DWORD dwHandle,
//							  LPCWSTR lpWideCharStr, 
//							  int cchWideChar, 
//							  LPSTR lpMultiByteStr, 
//							  int cbMultiByte, 
//							  LPCSTR lpDefaultChar, 
//							  LPBOOL lpUsedDefaultChar );
//������
//	IN dwHandle - �� Create ���ص�ֵ
//  IN lpWideCharStr-Unicode�ַ���
//  IN cchWideChar-��Ҫת����Unicode�ַ���
//  IN lpMultiByteStr-�����ַ��Ļ���
//  IN cchWideChar-�����ַ��Ļ����С(��byteΪ��λ)
//  IN lpDefaultChar-���޷�ת��ʱ����������ַ���.(����ΪNULL)
//  OUT lpUsedDefaultChar-�����޷�ת����, ������ΪTRUE;���򣬽�����ΪFALSE.(����ΪNULL)
//����ֵ��
//	д�뻺���byte-char�ַ���(��byteΪ��λ)
//���������������ַ�lpWideCharStr �õ���Ӧ��charֵ
//����: 
// ********************************************************************

typedef int ( * LPGETCHAR )( DWORD dwHandle,
							  LPCWSTR lpWideCharStr, 
							  int cchWideChar, 
							  LPSTR lpMultiByteStr, 
							  int cbMultiByte, 
							  LPCSTR lpDefaultChar, 
							  BOOL * lpUsedDefaultChar );

// ********************************************************************
//������int CountBufferSize( DWORD dwHandle,
//					           LPCVOID lpcvStr,
//					           int cbCountLen,
//						       BOOL IsWideChar );
//������
//	IN dwHandle - �� Create ���ص�ֵ
//  IN lpcvStr-�ַ���ָ��
//  IN cbCountLen-��Ҫͳ�Ƶ��ַ�������(������IsWideChar)
//  IN IsWideChar-ָ��lpcvStr�Լ�cbCountLen�Ƿ���wide-char;
//                �����ǣ�lpcvStr��LPCSTR����cbCountLen��bytes;
//                �����ǣ�lpcvStr��LPCWSTR����cbCountLen��wide-chars;
//����ֵ��
//	��Ҫ��buffer��С��byte��λ or wide-char��λ��
//�����������õ���Ҫ��buffer��С
//����: 
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
