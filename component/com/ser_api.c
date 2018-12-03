/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵������������---IO ���ƹ���(��DeviceIoControl)
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-07-01
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/

#include <ewindows.h>

/***************  ȫ���� ���壬 ���� *****************/

//----------------------------------------------------
typedef	struct	_PARAM_CLEARCOMMERR
{
	DWORD	dwErrors;
	COMSTAT	comStat;

}PARAM_CLEARCOMMERR;

/******************************************************/


// ********************************************************************
//������BOOL WINAPI  ClearCommError(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpErrors - �����ȡ���Ĵ���״̬
//	IN lpStat - �����ȡ����ͨѶ״̬
//����ֵ��
//	TRUE��ʾ�����ɹ�
//�������������ͨѶ�˿ڵĴ���״̬��ͨѶ״̬
//����: 
// ********************************************************************
BOOL WINAPI  ClearCommError(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat )
{
	PARAM_CLEARCOMMERR	ParamGet;

	if( DeviceIoControl( hFile, IOCTL_SERIAL_GET_COMMSTATUS, NULL, 0, &ParamGet, sizeof(PARAM_CLEARCOMMERR), NULL, NULL ) )
	{
		*lpErrors = ParamGet.dwErrors;
		*lpStat   = ParamGet.comStat;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ********************************************************************
//������BOOL WINAPI   ClearCommBreak(HANDLE hFile )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//����ֵ��
//	TRUE��ʾ�����ɹ�
//�������������ͨѶ�˿ڵ�Break�ź�
//����: 
// ********************************************************************
BOOL WINAPI   ClearCommBreak(HANDLE hFile )
{
	return DeviceIoControl( hFile, IOCTL_SERIAL_SET_BREAK_OFF, NULL, 0, NULL, 0, NULL, NULL );
}

// ********************************************************************
//������BOOL WINAPI  SetCommBreak( HANDLE hFile ) 
//������
//	IN hFile - ͨѶ�˿ڵľ��
//����ֵ��
//	TRUE��ʾ�����ɹ�
//��������������ͨѶ�˿ڵ�Break�ź�
//����: 
// ********************************************************************
BOOL WINAPI  SetCommBreak( HANDLE hFile ) 
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_BREAK_ON, NULL, 0, NULL, 0, NULL, NULL );
}

// ********************************************************************
//������BOOL WINAPI  EscapeCommFunction(HANDLE hFile, DWORD dwFunc )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN dwFunc - ָ���Ĳ�������
//����ֵ��
//	TRUE��ʾ�����ɹ�
//��������������ͨѶ�˿ڵ�ָ������
//����: 
// ********************************************************************
BOOL WINAPI  EscapeCommFunction(HANDLE hFile, DWORD dwFunc )
{				
	switch( dwFunc ) 
	{				
	case SETDTR:		
		return DeviceIoControl( hFile, IOCTL_SERIAL_SET_DTR, NULL, 0, NULL, 0, NULL, NULL );	
	case CLRDTR:		
		return DeviceIoControl( hFile, IOCTL_SERIAL_CLR_DTR, NULL, 0, NULL, 0, NULL, NULL );	
	case SETRTS:		
		return DeviceIoControl( hFile, IOCTL_SERIAL_SET_RTS, NULL, 0, NULL, 0, NULL, NULL );	
	case CLRRTS:		 
		return DeviceIoControl( hFile, IOCTL_SERIAL_CLR_RTS, NULL, 0, NULL, 0, NULL, NULL );	
	case SETXOFF:		
		return DeviceIoControl( hFile, IOCTL_SERIAL_SET_XOFF, NULL, 0, NULL, 0, NULL, NULL );	
	case SETXON:		
		return DeviceIoControl( hFile, IOCTL_SERIAL_SET_XON, NULL, 0, NULL, 0, NULL, NULL );	
	case CLRBREAK:		
		return DeviceIoControl( hFile, IOCTL_SERIAL_SET_BREAK_OFF, NULL, 0, NULL, 0, NULL, NULL );	
	case SETBREAK:		
		return DeviceIoControl( hFile, IOCTL_SERIAL_SET_BREAK_ON, NULL, 0, NULL, 0, NULL, NULL );	
	default:
		return FALSE;
	}	
}

// ********************************************************************
//������BOOL WINAPI  GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpEvtMask - �����ȡ����MODEM״̬
//����ֵ��
//	TRUE��ʾ�����ɹ�
//������������ȡͨѶ�˿ڵ�MODEM״̬������״̬��
//����: 
// ********************************************************************
BOOL WINAPI  GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat )
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_MODEMSTATUS, NULL, 0, lpModemStat, sizeof(DWORD), &dwActualOut, NULL ); 
}

// ********************************************************************
//������BOOL WINAPI  GetCommProperties(HANDLE hFile, LPCOMMPROP lpCommProp ) 
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpEvtMask - �����ȡ����ͨѶ����
//����ֵ��
//	TRUE��ʾ�����ɹ�
//������������ȡͨѶ�˿ڵ�ͨѶ����
//����: 
// ********************************************************************
BOOL WINAPI  GetCommProperties(HANDLE hFile, LPCOMMPROP lpCommProp ) 
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_PROPERTIES, NULL, 0, lpCommProp, sizeof(COMMPROP), &dwActualOut, NULL );	
}

// ********************************************************************
//������BOOL WINAPI  PurgeComm(HANDLE hFile, DWORD dwFlags )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN dwFunc - ָ�����������:
//		PURGE_TXCLEAR��ָ��������͵�
//		PURGE_RXCLEAR��ָ��������յ�
//����ֵ��
//	TRUE��ʾ�����ɹ�
//�������������ͨѶ�˿ڵ��շ��������Ӳ���Ļ���BUFFER
//����: 
// ********************************************************************
BOOL WINAPI  PurgeComm(HANDLE hFile, DWORD dwFlags )
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_PURGE, &dwFlags, sizeof(DWORD), NULL, 0, NULL, NULL );
}

// ********************************************************************
//������BOOL WINAPI  GetCommMask(HANDLE hFile, LPDWORD lpEvtMask ) 
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpEvtMask - �����ȡ���ĵȴ�����
//����ֵ��
//	TRUE��ʾ�����ɹ�
//������������ȡͨѶ�˿ڵ�ͨѶ�¼��ĵȴ�����
//����: 
// ********************************************************************
BOOL WINAPI  GetCommMask(HANDLE hFile, LPDWORD lpEvtMask ) 
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_WAIT_MASK, NULL, 0, lpEvtMask, sizeof(DWORD), &dwActualOut, NULL );
}

// ********************************************************************
//������BOOL WINAPI  SetCommMask(HANDLE hFile, DWORD dwEvtMask )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN dwEvtMask - ָ���ĵȴ�����
//����ֵ��
//	TRUE��ʾ�����ɹ�
//��������������ͨѶ�˿ڵ�ͨѶ�¼��ĵȴ�����
//����: 
// ********************************************************************
BOOL WINAPI  SetCommMask(HANDLE hFile, DWORD dwEvtMask )
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_WAIT_MASK, &dwEvtMask, sizeof(DWORD), NULL, 0, NULL, NULL );
	//return DeviceIoControl(hFile, IOCTL_SERIAL_SET_WAIT_MASK, &dwEvtMask, sizeof(DWORD), NULL, 0, NULL, NULL );
}

// ********************************************************************
//������BOOL WINAPI  WaitCommEvent(HANDLE hFile, LPDWORD lpEvtMask, LPOVERLAPPED lpOverlapped )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpEvtMask - �����ȡ���ĵȴ��¼�
//	IN lpOverlapped - ָ���Ĳ�������ʱ��������
//����ֵ��
//	TRUE��ʾ�����ɹ�
//�����������ȴ�ͨѶ�˿ڵ�ͨѶ�¼�
//����: 
// ********************************************************************
BOOL WINAPI  WaitCommEvent(HANDLE hFile, LPDWORD lpEvtMask, LPOVERLAPPED lpOverlapped )
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_WAIT_ON_MASK, NULL, 0, lpEvtMask, sizeof(DWORD), &dwActualOut, lpOverlapped );
}

// ********************************************************************
//������BOOL WINAPI  GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpCommTimeouts - �����ȡ���ĳ�ʱ����
//����ֵ��
//	TRUE��ʾ�����ɹ�
//������������ȡͨѶ�˿ڵĳ�ʱ����
//����: 
// ********************************************************************
BOOL WINAPI  GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts ) 
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_TIMEOUTS, NULL, 0, lpCommTimeouts, sizeof(COMMTIMEOUTS), &dwActualOut, NULL );
}

// ********************************************************************
//������BOOL WINAPI  SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpCommTimeouts - ָ���ĳ�ʱ����
//����ֵ��
//	TRUE��ʾ�����ɹ�
//��������������ͨѶ�˿ڵĳ�ʱ����
//����: 
// ********************************************************************
BOOL WINAPI  SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts ) 
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_TIMEOUTS, lpCommTimeouts, sizeof(COMMTIMEOUTS), NULL, 0, NULL, NULL );
}

// ********************************************************************
//������BOOL WINAPI  GetCommState(HANDLE hFile, LPDCB lpDCB )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpDCB - �����ȡ����DCB����
//����ֵ��
//	TRUE��ʾ�����ɹ�
//������������ȡͨѶ�˿ڵ�DCB����
//����: 
// ********************************************************************
BOOL WINAPI  GetCommState(HANDLE hFile, LPDCB lpDCB )
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_DCB, NULL, 0, lpDCB, sizeof(DCB), &dwActualOut, NULL );
}


// ********************************************************************
//������BOOL WINAPI  SetCommState(HANDLE hFile, LPDCB lpDCB )
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN lpDCB - ָ����DCB����
//����ֵ��
//	TRUE��ʾ�����ɹ�
//��������������ͨѶ�˿ڵ�DCB����
//����: 
// ********************************************************************
BOOL WINAPI  SetCommState(HANDLE hFile, LPDCB lpDCB )
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_DCB, lpDCB, sizeof(DCB), NULL, 0, NULL , NULL );
}


// ********************************************************************
//������BOOL WINAPI  SetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue ) 
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN dwInQueue - ָ��ͨѶ�˿ڵĽ��ջ���BUFFER�Ĵ�С
//	IN dwOutQueue - ָ��ͨѶ�˿ڵķ��ͻ���BUFFER�Ĵ�С
//����ֵ��
//	TRUE��ʾ�����ɹ�
//��������������ͨѶ�˿ڵ��շ�����BUFFER�Ĵ�С
//����: 
// ********************************************************************
BOOL WINAPI  SetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue ) 
{
	DWORD	QueSize[2];
	QueSize[0] = dwInQueue;
	QueSize[1] = dwOutQueue;
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_QUEUE_SIZE, QueSize, sizeof(QueSize), NULL, 0, NULL, NULL );
}

// ********************************************************************
//������BOOL WINAPI  TransmitCommChar(HANDLE hFile, char cChar ) 
//������
//	IN hFile - ͨѶ�˿ڵľ��
//	IN cChar - ָ��Ҫ�������͵��ַ�
//����ֵ��
//	TRUE��ʾ�����ɹ�
//������������������ָ���ַ�
//����: 
// ********************************************************************
BOOL WINAPI  TransmitCommChar(HANDLE hFile, char cChar ) 
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_IMMEDIATE_CHAR, &cChar, sizeof(UCHAR), NULL, 0, NULL, NULL );
}

