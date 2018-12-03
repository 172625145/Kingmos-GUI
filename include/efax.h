/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/

#ifndef __EFAX_H
#define __EFAX_H

#ifndef VC386
#include <econtact.h>
#endif

#ifdef _cpluscplus
extern "C" {
#endif


#define FLOAD_SUCCESS 0
#define FLOAD_CRFILFAILE 1
#define FLOAD_CRDIRFAILE 2
#define FLOAD_NOENOGHMEM 3
#define FLOAD_READFILFAIL 4
#define FLOAD_WRITFILFAIL 5
#define FLOAD_ERROR_DATA 6

#define BH_MODEL 1//����λͼ���
#define TB_MODEL 2//�����ı�������
#define TF_MODEL 3//�����ı��ļ�
#define SUBJECT_LEN 20

#ifdef VC386
#define PHONENUMBER_LEN 20
#endif
extern LPCTSTR g_szInterChangeFile;
typedef struct _FAXGUIDE{
	TCHAR szSubject[SUBJECT_LEN];//����
	TCHAR szFaxNumber[PHONENUMBER_LEN];//�Է������
	PVOID pData;//���ݣ�������λͼ������ı�������ָ�롢�ı��ļ�����װ��ʱ����ָ��
	DWORD dwDataSize;//���ݴ�С��װ��ʱ����ָ��
	DWORD dwDataMode;//���ݸ�ʽ,��������pDate�ֱ����Ӧ��BH_MODE��TB_MODEL��TF_MODEL֮һ��װ��ʱ����ָ��
}FAXGUIDE, *PFAXGUIDE;

DWORD LoadFax(PFAXGUIDE pFaxGuide);

#ifdef _cpluscplus
}	//cpluscplus
#endif


#endif
