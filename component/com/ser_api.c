/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：串口驱动---IO 控制功能(调DeviceIoControl)
版本号：  1.0.0
开发时期：2003-07-01
作者：    肖远钢
修改记录：
******************************************************/

#include <ewindows.h>

/***************  全局区 定义， 声明 *****************/

//----------------------------------------------------
typedef	struct	_PARAM_CLEARCOMMERR
{
	DWORD	dwErrors;
	COMSTAT	comStat;

}PARAM_CLEARCOMMERR;

/******************************************************/


// ********************************************************************
//声明：BOOL WINAPI  ClearCommError(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpErrors - 保存获取到的错误状态
//	IN lpStat - 保存获取到的通讯状态
//返回值：
//	TRUE表示操作成功
//功能描述：检查通讯端口的错误状态和通讯状态
//引用: 
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
//声明：BOOL WINAPI   ClearCommBreak(HANDLE hFile )
//参数：
//	IN hFile - 通讯端口的句柄
//返回值：
//	TRUE表示操作成功
//功能描述：清除通讯端口的Break信号
//引用: 
// ********************************************************************
BOOL WINAPI   ClearCommBreak(HANDLE hFile )
{
	return DeviceIoControl( hFile, IOCTL_SERIAL_SET_BREAK_OFF, NULL, 0, NULL, 0, NULL, NULL );
}

// ********************************************************************
//声明：BOOL WINAPI  SetCommBreak( HANDLE hFile ) 
//参数：
//	IN hFile - 通讯端口的句柄
//返回值：
//	TRUE表示操作成功
//功能描述：设置通讯端口的Break信号
//引用: 
// ********************************************************************
BOOL WINAPI  SetCommBreak( HANDLE hFile ) 
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_BREAK_ON, NULL, 0, NULL, 0, NULL, NULL );
}

// ********************************************************************
//声明：BOOL WINAPI  EscapeCommFunction(HANDLE hFile, DWORD dwFunc )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN dwFunc - 指定的操作功能
//返回值：
//	TRUE表示操作成功
//功能描述：操作通讯端口的指定功能
//引用: 
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
//声明：BOOL WINAPI  GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpEvtMask - 保存获取到的MODEM状态
//返回值：
//	TRUE表示操作成功
//功能描述：获取通讯端口的MODEM状态（引脚状态）
//引用: 
// ********************************************************************
BOOL WINAPI  GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat )
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_MODEMSTATUS, NULL, 0, lpModemStat, sizeof(DWORD), &dwActualOut, NULL ); 
}

// ********************************************************************
//声明：BOOL WINAPI  GetCommProperties(HANDLE hFile, LPCOMMPROP lpCommProp ) 
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpEvtMask - 保存获取到的通讯属性
//返回值：
//	TRUE表示操作成功
//功能描述：获取通讯端口的通讯属性
//引用: 
// ********************************************************************
BOOL WINAPI  GetCommProperties(HANDLE hFile, LPCOMMPROP lpCommProp ) 
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_PROPERTIES, NULL, 0, lpCommProp, sizeof(COMMPROP), &dwActualOut, NULL );	
}

// ********************************************************************
//声明：BOOL WINAPI  PurgeComm(HANDLE hFile, DWORD dwFlags )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN dwFunc - 指定的清除功能:
//		PURGE_TXCLEAR：指定清除发送的
//		PURGE_RXCLEAR：指定清除接收的
//返回值：
//	TRUE表示操作成功
//功能描述：清除通讯端口的收发的软件和硬件的缓冲BUFFER
//引用: 
// ********************************************************************
BOOL WINAPI  PurgeComm(HANDLE hFile, DWORD dwFlags )
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_PURGE, &dwFlags, sizeof(DWORD), NULL, 0, NULL, NULL );
}

// ********************************************************************
//声明：BOOL WINAPI  GetCommMask(HANDLE hFile, LPDWORD lpEvtMask ) 
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpEvtMask - 保存获取到的等待掩码
//返回值：
//	TRUE表示操作成功
//功能描述：获取通讯端口的通讯事件的等待掩码
//引用: 
// ********************************************************************
BOOL WINAPI  GetCommMask(HANDLE hFile, LPDWORD lpEvtMask ) 
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_WAIT_MASK, NULL, 0, lpEvtMask, sizeof(DWORD), &dwActualOut, NULL );
}

// ********************************************************************
//声明：BOOL WINAPI  SetCommMask(HANDLE hFile, DWORD dwEvtMask )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN dwEvtMask - 指定的等待掩码
//返回值：
//	TRUE表示操作成功
//功能描述：设置通讯端口的通讯事件的等待掩码
//引用: 
// ********************************************************************
BOOL WINAPI  SetCommMask(HANDLE hFile, DWORD dwEvtMask )
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_WAIT_MASK, &dwEvtMask, sizeof(DWORD), NULL, 0, NULL, NULL );
	//return DeviceIoControl(hFile, IOCTL_SERIAL_SET_WAIT_MASK, &dwEvtMask, sizeof(DWORD), NULL, 0, NULL, NULL );
}

// ********************************************************************
//声明：BOOL WINAPI  WaitCommEvent(HANDLE hFile, LPDWORD lpEvtMask, LPOVERLAPPED lpOverlapped )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpEvtMask - 保存获取到的等待事件
//	IN lpOverlapped - 指定的操作，暂时保留不用
//返回值：
//	TRUE表示操作成功
//功能描述：等待通讯端口的通讯事件
//引用: 
// ********************************************************************
BOOL WINAPI  WaitCommEvent(HANDLE hFile, LPDWORD lpEvtMask, LPOVERLAPPED lpOverlapped )
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_WAIT_ON_MASK, NULL, 0, lpEvtMask, sizeof(DWORD), &dwActualOut, lpOverlapped );
}

// ********************************************************************
//声明：BOOL WINAPI  GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpCommTimeouts - 保存获取到的超时设置
//返回值：
//	TRUE表示操作成功
//功能描述：获取通讯端口的超时设置
//引用: 
// ********************************************************************
BOOL WINAPI  GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts ) 
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_TIMEOUTS, NULL, 0, lpCommTimeouts, sizeof(COMMTIMEOUTS), &dwActualOut, NULL );
}

// ********************************************************************
//声明：BOOL WINAPI  SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpCommTimeouts - 指定的超时设置
//返回值：
//	TRUE表示操作成功
//功能描述：设置通讯端口的超时设置
//引用: 
// ********************************************************************
BOOL WINAPI  SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts ) 
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_TIMEOUTS, lpCommTimeouts, sizeof(COMMTIMEOUTS), NULL, 0, NULL, NULL );
}

// ********************************************************************
//声明：BOOL WINAPI  GetCommState(HANDLE hFile, LPDCB lpDCB )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpDCB - 保存获取到的DCB内容
//返回值：
//	TRUE表示操作成功
//功能描述：获取通讯端口的DCB内容
//引用: 
// ********************************************************************
BOOL WINAPI  GetCommState(HANDLE hFile, LPDCB lpDCB )
{
	DWORD	dwActualOut;
	return DeviceIoControl(hFile, IOCTL_SERIAL_GET_DCB, NULL, 0, lpDCB, sizeof(DCB), &dwActualOut, NULL );
}


// ********************************************************************
//声明：BOOL WINAPI  SetCommState(HANDLE hFile, LPDCB lpDCB )
//参数：
//	IN hFile - 通讯端口的句柄
//	IN lpDCB - 指定的DCB内容
//返回值：
//	TRUE表示操作成功
//功能描述：设置通讯端口的DCB内容
//引用: 
// ********************************************************************
BOOL WINAPI  SetCommState(HANDLE hFile, LPDCB lpDCB )
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_DCB, lpDCB, sizeof(DCB), NULL, 0, NULL , NULL );
}


// ********************************************************************
//声明：BOOL WINAPI  SetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue ) 
//参数：
//	IN hFile - 通讯端口的句柄
//	IN dwInQueue - 指定通讯端口的接收缓冲BUFFER的大小
//	IN dwOutQueue - 指定通讯端口的发送缓冲BUFFER的大小
//返回值：
//	TRUE表示操作成功
//功能描述：设置通讯端口的收发缓冲BUFFER的大小
//引用: 
// ********************************************************************
BOOL WINAPI  SetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue ) 
{
	DWORD	QueSize[2];
	QueSize[0] = dwInQueue;
	QueSize[1] = dwOutQueue;
	return DeviceIoControl(hFile, IOCTL_SERIAL_SET_QUEUE_SIZE, QueSize, sizeof(QueSize), NULL, 0, NULL, NULL );
}

// ********************************************************************
//声明：BOOL WINAPI  TransmitCommChar(HANDLE hFile, char cChar ) 
//参数：
//	IN hFile - 通讯端口的句柄
//	IN cChar - 指定要立即发送的字符
//返回值：
//	TRUE表示操作成功
//功能描述：立即发送指定字符
//引用: 
// ********************************************************************
BOOL WINAPI  TransmitCommChar(HANDLE hFile, char cChar ) 
{
	return DeviceIoControl(hFile, IOCTL_SERIAL_IMMEDIATE_CHAR, &cChar, sizeof(UCHAR), NULL, 0, NULL, NULL );
}

