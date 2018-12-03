/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����ע��� OEM �������
�汾�ţ�1.0.0
����ʱ�ڣ�2004-07-27
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <ewindows.h>
#include <emlos.h>
#include <oemfunc.h>

//��ע����浽��ָ���OEM�豸
//��������֧�ָù��ܣ������²��裺
//1.ʵ��һ�����ܺ�������  BOOL OEM_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
//2.��ȫ�ֱ�����ֵ        extern LPOEM_REGISTRYIO lpOEM_RegistryIO=OEM_RegistryIO;

// ********************************************************************
//������BOOL OEM_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
//������
//	IN/OUT lpDataBuf - ���ڱ����д�������
//  IN nNumberOfBytes - lpDataBuf ��������
//	IN dwFlag - �������͵����:
//                   REGIO_BEGIN - ��һ�β���
//                   REGIO_READ -  ��
//                   REGIO_WRITE - д
//                   REGIO_END -   ��������
//����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
//����������
//	�������ע���
//����:
//	ϵͳAPI
// ********************************************************************

BOOL OEM_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
{
	static HANDLE hFile = INVALID_HANDLE_VALUE;
	//�Ƿ�ʼIO ?
	if( dwFlag & REGIO_BEGIN )
	{	//��
		hFile = Win32_CreateFile( "kmsreg.dat", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL );
	}

	if( hFile && hFile != INVALID_HANDLE_VALUE )
	{
		DWORD dwRW;
		if( dwFlag & REGIO_WRITE )
			Win32_WriteFile( hFile, lpDataBuf, nNumberOfBytes, &dwRW, NULL );
		else if( dwFlag & REGIO_READ )
			Win32_ReadFile( hFile, lpDataBuf, nNumberOfBytes, &dwRW, NULL );			
	}
	//�Ƿ����IO ?
	if( dwFlag & REGIO_END )
	{	//��
		 Win32_CloseHandle( hFile ); 
		 hFile = INVALID_HANDLE_VALUE;
	}
	return TRUE;
}

extern LPOEM_REGISTRYIO lpOEM_RegistryIO=OEM_RegistryIO;


