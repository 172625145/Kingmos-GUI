/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵��������API������API����
�汾�ţ�1.0.0
����ʱ�ڣ�2003-04-16
���ߣ��½��� (Jami chen)
�޸ļ�¼��
******************************************************/
#include <eversion.h>

#ifndef EML_WIN32

#include "eWindows.h"
#include "EMMSys.h"
#include <Audio_ep.h>
//#include <waveddsi.h>
//#include <mmddk.h>
#include <eDevice.h>
#include <eobjcall.h>
#include <eapisrv.h>

// *****************************************************
// ����������
// *****************************************************
void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1, DWORD dw2);
//void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1);//, DWORD dw2);

typedef void (CALLBACK PROCBACK)( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance, DWORD lParam, DWORD wParam );
//typedef void (CALLBACK PROCBACK)( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance, DWORD lParam);//, DWORD wParam );


// *****************************************************
// ������
// *****************************************************
// hwo�������
typedef struct wavStruct{
	HANDLE hWav;   // �����豸���
	UINT uDeviceID;  // �豸��ʶ��
	PWAVEFORMATEX pwfx;  // ������ʽ
	DWORD dwCallback;  // �ص������������Ǻ���(Function),���ھ��(HWND),...
	DWORD dwInstance;  // ʵ�����
	DWORD fdwOpen;   //�򿪷�ʽ
	HANDLE hCallerProcess;  // ���д���ʵ�����
}WAVSTRUCT, *LPWAVSTRUCT;


static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2);

/**********************************************************/

// ********************************************************************
//������UINT     WINAPI Audio_waveOutGetNumDevs(void)
//������
//	��
//����ֵ��
//	���ص�ǰϵͳ֧�ֵ��豸����
//�����������õ��豸��Ŀ
//����: wave API�����ӿ�
// ********************************************************************
UINT     WINAPI Audio_waveOutGetNumDevs(void)
{

	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	UINT numDevs;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetNumDevs\r\n");

	// �������˿�
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);   
	if (hWav==INVALID_HANDLE_VALUE)
	{  // ��ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return 0;
	}

//	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.uDeviceID=0;
	Params.message=AM_OUT_GETNUMDEVS;  // ���õõ��豸��Ŀ��Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=0;
	Params.wParam=0;

	// ������Ϣ
	numDevs=DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0);  
	CloseHandle(hWav);  // �رվ��
	DbgOutString("-waveOutGetNumDevs\r\n");
	return numDevs;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
//������
//	UINT uDeviceID   --  �豸��ʶ��
//	LPWAVEOUTCAPS pwoc  -- һ��ָ������豸������ָ��
//  UINT cbwoc  --  �ṹWAVEOUTCAPS�Ĵ�С����BYTEΪ��λ
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ��豸����
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetDevCaps\r\n");

	// �������˿�
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // ��ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	Params.uDeviceID=uDeviceID;  // �豸���
	Params.message=AM_OUT_GETDEVCAPS;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pwoc;  // ��������
	Params.wParam=cbwoc;
	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );   // ӳ��ָ��

	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
		CloseHandle(hWav);  // �رվ��
 		return MMSYSERR_ERROR;
	}
	//��Ϣ�ɹ�
	CloseHandle(hWav);   // �رվ��
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveOutGetDevCaps\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPDWORD pdwVolume  -- ָ����������ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ���ǰ��������С
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetVolume\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

	if (lpwavStruct==NULL)
	{  // û�д�����
		// �������˿�
		hWav=CreateFile("AUD1:", 0 , FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
		if (hWav==INVALID_HANDLE_VALUE)
		{  // ��ʧ��
			DbgOutString("Can't Open Wav Device\r\n");
			return MMSYSERR_ALLOCATED;
		}
		Params.uDeviceID=0;
	}
	else
	{  // �Ѿ�������
		Params.uDeviceID=lpwavStruct->uDeviceID;
		hWav=lpwavStruct->hWav;
	}

	Params.message=AM_OUT_GETVOLUME;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pdwVolume;  // ���ò���
	Params.wParam=0;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // ӳ��ָ��
	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ������Ϣʧ��
		if (lpwavStruct==NULL)
		{  // �رմ򿪵ľ��
			CloseHandle(hWav);
		}
 		return MMSYSERR_ERROR;
	}
	DbgOutString("-waveOutGetVolume\r\n");
	if (lpwavStruct==NULL)
	{
		CloseHandle(hWav); // �رվ��
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	DWORD dwVolume  -- Ҫ����������С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������õ�ǰ��������С
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutSetVolume\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // û�д�����
		// ������
		hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
		if (hWav==INVALID_HANDLE_VALUE)
		{  // ������ʧ��
			DbgOutString("Can't Open Wav Device\r\n");
			return MMSYSERR_ALLOCATED;
		}
		Params.uDeviceID=0;
	}
	else
	{ // �Ѿ�������
		Params.uDeviceID=lpwavStruct->uDeviceID;
		hWav=lpwavStruct->hWav;  // �õ��������
	}

	Params.message=AM_OUT_SETVOLUME;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=dwVolume;// ���ò���
	Params.wParam=0;

	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ������Ϣʧ��
		if (lpwavStruct==NULL)
		{
			CloseHandle(hWav);  // �رվ��
		}
 		return MMSYSERR_ERROR;
	}
	// ������Ϣ�ɹ�
	if (lpwavStruct==NULL)
	{
		CloseHandle(hWav);  // �رվ��
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;  // ���ش���
	}
	DbgOutString("-waveOutSetVolume\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
//������
//	MMRESULT mmrError -- Ҫ�õ��ı��Ĵ�����Ϣ
//	LPTSTR pszText  --  ָ���Ŵ����ı������ָ��
//	UINT cchText  -- ����Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������õ�ǰ��������С
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
	DbgOutString("+waveOutGetErrorText\r\n");
	DbgOutString("-waveOutGetErrorText\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������ر��������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
//	PROCBACK *lpProcBack;
	AUDIOMDD_MSG  Params;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveOutClose\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

		if (lpwavStruct==NULL)
		{  // ����û�д�
			return MMSYSERR_INVALHANDLE;
		}
		else
		{ // �Ѿ�������
			Params.uDeviceID=lpwavStruct->uDeviceID;
			hWav=lpwavStruct->hWav;  // �õ��������
		}
	
		Params.message=AM_OUT_CLOSE;  // ������Ϣ
		Params.dwCallBackParam=0;
		Params.lParam=0;// ���ò���
		Params.wParam=0;
		
		// ������Ϣ
		if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // ������Ϣʧ��
			if (lpwavStruct==NULL)
			{
				CloseHandle(hWav);  // �رվ��
			}
	 		return MMSYSERR_ERROR;
		}
		// ������Ϣ�ɹ�
		if (lpwavStruct==NULL)
		{
			CloseHandle(hWav);  // �رվ��
		}
		

		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // Ҫ�󷵻ش���
			DbgOutString("will Call back Window\r\n");
			SendMessage((HWND)lpwavStruct->dwCallback,MM_WOM_CLOSE,0,0);  // ���͹ر���������Ϣ������
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{  // Ҫ��ص��߳�
			DbgOutString("will Call back Thread\r\n");
			return MMSYSERR_NOERROR;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{  // Ҫ��ص��¼�
			DbgOutString("will Call back Event\r\n");
			//SetEvent(dwCallback);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{ // Ҫ��ص�����
			DbgOutString("will Call back Function\r\n");
			
			CallApFunction(lpwavStruct,WOM_CLOSE,0,0);  // �ص�����֪ͨҪ�ر�����

//			lpProcBack=lpwavStruct->dwCallback;
//			lpProcBack(lpwavStruct,WOM_CLOSE,lpwavStruct->dwInstance,0,0);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // û��Ҫ��ص�
			DbgOutString("will Call back NULL\r\n");
		}
		else
		{  // û�����ûص�
			DbgOutString("Vaild Parameter\r\n");
		}

		EdbgOutputDebugString("lpwavStruct in waveOutClose %X\r\n",lpwavStruct);
		CloseHandle(lpwavStruct->hWav);  // �ر��������
		DbgOutString("CloseHandle Complete\r\n");
		
		if (lpwavStruct->pwfx) // �ͷſռ�
			free(lpwavStruct->pwfx);
		free(lpwavStruct);
		DbgOutString("-waveOutClose\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������׼���������ݿ��ͷ
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	//DbgOutString("+waveOutPrepareHeader\r\n");
	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // ӳ�����
	pwh->dwFlags |= WHDR_PREPARED;    // �����Ѿ�׼���õı�־
	//DbgOutString("-waveOutPrepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������ж���������ݿ��ͷ
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	//DbgOutString("+waveOutUnprepareHeader\r\n");
	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() ); // ӳ��ָ��
	pwh->dwFlags &= ~WHDR_PREPARED; // �����־
	//DbgOutString("-waveOutUnprepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������д�������ݿ鵽�豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

//	RETAILMSG(1,("+waveOutWrite\r\n"));

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // ӳ��ָ��

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_WRITE;  // ������Ϣ
	Params.dwCallBackParam=0;//lpwavStruct;
	Params.lParam=(DWORD)pwh;  // ���ò���
	Params.wParam=0;

//	RETAILMSG(1,("lpwavStruct in Write %X\r\n",lpwavStruct));
//	RETAILMSG(1,("lpwavStruct->hWav = %X\r\n",lpwavStruct->hWav));
//	RETAILMSG(1,("lpwavStruct->uDeviceID = %X\r\n",lpwavStruct->uDeviceID));
//	RETAILMSG(1,("lpwavStruct->pwfx = %X\r\n",lpwavStruct->pwfx));
//	RETAILMSG(1,("lpwavStruct->dwCallback = %X\r\n",lpwavStruct->dwCallback));
//	RETAILMSG(1,("lpwavStruct->dwInstance = %X\r\n",lpwavStruct->dwInstance));
//	RETAILMSG(1,("lpwavStruct->fdwOpen = %X\r\n",lpwavStruct->fdwOpen));

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // ӳ��ָ��

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
		DbgOutString("Write Wav Failure\r\n");
 		return MMSYSERR_WRITEERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}

//	RETAILMSG(1,("-waveOutWrite\r\n"));
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//������������ͣ��������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo)
{
	AUDIOMDD_MSG  Params;
	DWORD dwError = MMSYSERR_ERROR;
	LPWAVSTRUCT lpwavStruct;

	DbgOutString("+waveOutPause\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_PAUSE;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=0;  // ���ò���
	Params.wParam=0;

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveOutPause\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������¿�ʼ��������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutRestart\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_RESTART;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=0;  // ���ò���
	Params.wParam=0;

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveOutRestart\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������¿�ʼ��������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutReset\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_RESET;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=0;  // ���ò���
	Params.wParam=0;

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // ����
		return dwError;
	}
	DbgOutString("-waveOutReset\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������ж�ѭ������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutBreakLoop\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_BREAKLOOP;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=0; // ���ò���
	Params.wParam=0;

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // ��Ϣ�д�����
		return dwError;
	}
	DbgOutString("-waveOutBreakLoop\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPMMTIME pmmt -- ָ��洢λ�õĽṹ��ָ��
//	UINT cbmmt -- λ�õĽṹ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������ж�ѭ������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

//	DbgOutString("+waveOutGetPosition\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_GETPOS; // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pmmt;  // ������Ϣ
	Params.wParam=cbmmt;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );   // ӳ��ָ��

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // ��Ϣ�д�����
		return dwError;
	}
//	DbgOutString("-waveOutGetPosition\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPDWORD pdwPitch -- ָ��ǰ��������ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� �õ���ǰ������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetPitch\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_GETPITCH;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pdwPitch;  // ���ò���
	Params.wParam=0;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // ӳ��ָ��

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // ��Ϣ�д���
		return dwError;
	}
	DbgOutString("-waveOutGetPitch\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	DWORD dwPitch -- Ҫ���õ�����
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ���õ�ǰ������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutSetPitch\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_SETPITCH;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=dwPitch;  // ���ò���
	Params.wParam=0;

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // ��Ϣ�д���
		return dwError;
	}
	DbgOutString("-waveOutSetPitch\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPDWORD pdwRate -- ָ���ŵ�ǰ�ط����ʵ�ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� �õ���ǰ���ط�����
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetPlaybackRate\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo; // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_GETPLAYBACKRATE;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pdwRate;  // ���ò���
	Params.wParam=0;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); 

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // ��Ϣ�д���
		return dwError;
	}
	DbgOutString("-waveOutGetPlaybackRate\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	DWORD dwRate -- ��ǰҪ���õ��ط�����
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ���õ�ǰ���ط�����
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutSetPlaybackRate\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{  // ����û�д�
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_SETPLAYBACKRATE;  // ������Ϣ
	Params.dwCallBackParam=0;
	Params.lParam=dwRate;  // ���ò���
	Params.wParam=0;

	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // ��Ϣ�д���
		return dwError;
	}
	DbgOutString("-waveOutSetPlaybackRate\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	UINT *puDeviceID -- ָ���ŵ�ǰ���ŵ��豸��ʶ�ŵ�ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� �õ���ǰ���ŵ��豸��ʶ��
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID)
{
	DbgOutString("+waveOutGetID\r\n");
	DbgOutString("-waveOutGetID\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	UINT uMsg  -- Ҫ���͵���Ϣ
//	DWORD dw1  -- ����
//	DWORD dw2  -- ����
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ����һ���Զ������Ϣ���豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;
	HANDLE hWav;

	DbgOutString("+waveOutMessage\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ��ṹָ��

	if (lpwavStruct==NULL)
	{ // ����û�д�
		// �������˿�
		hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
		if (hWav==INVALID_HANDLE_VALUE)
		{  // ������ʧ��
			DbgOutString("Can't Open Wav Device\r\n");
			return MMSYSERR_ALLOCATED;
		}
		Params.uDeviceID=0;
	}
	else
	{  // �Ѿ�������
		Params.uDeviceID=lpwavStruct->uDeviceID;
		hWav=lpwavStruct->hWav; // �ﵽ�������
	}

//	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=uMsg;  // ������Ϣ
	Params.dwCallBackParam=0;  // ���ò���
	Params.wParam=dw1;
	Params.lParam=dw2;

	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
		if (lpwavStruct==NULL)
		{
			CloseHandle(hWav); // �رվ��
		}
 		return MMSYSERR_ERROR;
	}
	if (lpwavStruct==NULL)
	{
		CloseHandle(hWav); // �رվ��
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // ��Ϣ�д���
		return dwError;
	}
	DbgOutString("-waveOutMessage\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
//������
//	LPHWAVEOUT phwo	--	�����������豸����ľ��
//	UINT uDeviceID -- Ҫ�򿪵��豸�ı�ʶ��
//	PWAVEFORMATEX pwfx -- Ҫ�򿪵������ĸ�ʽ
//	DWORD dwCallback -- �ص����
//	DWORD dwInstance -- ʵ�����
//	DWORD fdwOpen -- ��ģʽ

//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ��һ�������豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	HANDLE hWav;
	AUDIOMDD_MSG  Params;
    AUDIOMDD_OPEN WOD;
	LPWAVSTRUCT lpwavStruct;
	PWAVEFORMATEX pnewwfx;
	DWORD dwError;
//	PROCBACK *lpProcBack;


	DbgOutString("mdd:waveOutOpen\r\n");
	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // �ص�����
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // �ص��̣߳���֧��
		DbgOutString("the call back thread is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED;
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{  // �ص��¼�����֧��
		DbgOutString("the call back Event is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED;
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{// �ص�������֧��
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{// û�лص�
	}
	else
	{  // û�����ûص�����������
		DbgOutString("Invaild Parameter\r\n");
		return MMSYSERR_INVALPARAM;
	}

	//	*phwo=NULL;
//	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	// �������˿�
	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{ // ������ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	lpwavStruct=(LPWAVSTRUCT)malloc(sizeof(WAVSTRUCT));  // ����ṹ

	if (lpwavStruct==NULL)
	{  // ����ʧ��
		DbgOutString("Memory is Not Enough");
		CloseHandle(hWav);  // �رվ��
		return MMSYSERR_NOMEM; // ����ʧ��
	}
	EdbgOutputDebugString("lpwavStruct in Open %X\r\n",lpwavStruct);

	pnewwfx=(PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));  // ����������ʽ�ṹ
	if (pnewwfx==NULL)
	{ // ����ʧ��
		DbgOutString("Memory is Not Enough");
		free(lpwavStruct);  // �ͷ��ڴ�
		CloseHandle(hWav); // �رվ��
		return MMSYSERR_NOMEM;  // ����ʧ��
	}
	*pnewwfx=*pwfx;  // ����������ʽ

//	RETAILMSG(1, (TEXT( "pwfx->wFormatTag=%d\r\n" ),pwfx->wFormatTag) );
//	RETAILMSG(1, (TEXT( "pwfx->nChannels=%d\r\n" ),pwfx->nChannels) );
//	RETAILMSG(1, (TEXT( "pwfx->nSamplesPerSec=%d\r\n" ),pwfx->nSamplesPerSec) );
//	RETAILMSG(1, (TEXT( "pwfx->wBitsPerSample=%d\r\n" ),pwfx->wBitsPerSample) );

//	RETAILMSG(1, (TEXT( "pnewwfx->wFormatTag=%d\r\n" ),pnewwfx->wFormatTag) );
//	RETAILMSG(1, (TEXT( "pnewwfx->nChannels=%d\r\n" ),pnewwfx->nChannels) );
//	RETAILMSG(1, (TEXT( "pnewwfx->nSamplesPerSec=%d\r\n" ),pnewwfx->nSamplesPerSec) );
//	RETAILMSG(1, (TEXT( "pnewwfx->wBitsPerSample=%d\r\n" ),pnewwfx->wBitsPerSample) );

	lpwavStruct->hWav=hWav;  // �����������
	lpwavStruct->uDeviceID=uDeviceID;
	lpwavStruct->pwfx=pnewwfx;
	lpwavStruct->dwCallback=dwCallback;  // ���ûص�
	lpwavStruct->dwInstance=dwInstance; // ����ʵ�����
	lpwavStruct->fdwOpen=fdwOpen; // ���ô򿪷�ʽ
	lpwavStruct->hCallerProcess = GetCallerProcess();  // ���ú��н���



//	WOD.hWave=NULL;
	WOD.lpFormat=pnewwfx;
//	WOD.dwCallback=dwCallback;
	WOD.dwCallback=(DWORD)DrvCallBack;  // ���������ص�
	WOD.dwInstance=dwInstance;
//	WOD.uMappedDeviceID=uDeviceID;


	Params.uDeviceID=uDeviceID;
	Params.message=AM_OUT_OPEN;  // ������Ϣ
	Params.dwCallBackParam=(DWORD)lpwavStruct;  // ���ûص�����
	Params.lParam=(DWORD)&WOD; // ������Ϣ����
	Params.wParam=fdwOpen;
	
	WOD.lpFormat = (PWAVEFORMATEX)MapPtrToProcess( (LPVOID)WOD.lpFormat, GetCurrentProcess() ); // ӳ��ָ��
	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // ӳ��ָ��
	Params.dwCallBackParam = (DWORD)MapPtrToProcess( (LPVOID)Params.dwCallBackParam, GetCurrentProcess() ); // ӳ��ָ��

//	RETAILMSG(1, (TEXT( " ****** lpwavStruct->hCallerProcess = %X\r\n" ),lpwavStruct->hCallerProcess) );
	DbgOutString("Open Wav Device Success\r\n");
	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // ��Ϣʧ��
		free(pnewwfx); // �ͷ��ڴ�
		free(lpwavStruct);
		CloseHandle(hWav); // �رվ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // ��Ϣ�д���
		free(pnewwfx);  // �ͷ��ڴ�
		free(lpwavStruct);
		CloseHandle(hWav); // �رվ��
 		return dwError;
	}
	*phwo=lpwavStruct;  // ���ص�ǰ�ṹ��Ϊ���

	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // �ص�����
		DbgOutString("will Call back Window\r\n");
		SendMessage((HWND)dwCallback,MM_WOM_OPEN,0,0);  // ���ʹ���������Ϣ������
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // �ص��߳�
		DbgOutString("will Call back Thread\r\n");
		return MMSYSERR_NOERROR;
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{ // �ص��¼�
		DbgOutString("will Call back Event\r\n");
		//SetEvent(dwCallback);
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // �ص�����
		DbgOutString("will Call back Function\r\n");

//		lpProcBack=dwCallback;
//		lpProcBack(lpwavStruct,WOM_OPEN,lpwavStruct->dwInstance,0,0);
		CallApFunction(lpwavStruct,WOM_OPEN,0,0);  // ���ûص�����֪ͨ�Ѿ�������

	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{  // û�лص�
		DbgOutString("will Call back NULL\r\n");
	}
	else
	{  // û�����ûص�
		DbgOutString("Vaild Parameter\r\n");
	}
	DbgOutString("-mdd:waveOutOpen\r\n");
	return MMSYSERR_NOERROR;
}


// ********************************************************************
//������UINT     WINAPI Audio_waveInGetNumDevs(void)
//������
//	��
//����ֵ��
//	���ص�ǰϵͳ֧�ֵ��豸����
//���������� �õ���ǰ����������豸��Ŀ
//����: wave API�����ӿ�
// ********************************************************************
UINT     WINAPI Audio_waveInGetNumDevs(void)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	UINT numDevs;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetNumDevs\r\n");

	// �������˿�
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // ������ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return 0;
	}

//	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.uDeviceID=0;
	Params.message=AM_IN_GETNUMDEVS;  // ������Ϣ
	Params.dwCallBackParam=0;  // ���ò���
	Params.lParam=0;
	Params.wParam=0;

	// ������Ϣ
	numDevs=DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0);
	CloseHandle(hWav);  // �رվ��
	DbgOutString("-waveInGetNumDevs\r\n");
	return numDevs;  // �����豸����
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
//������
//	UINT uDeviceID   --  �豸��ʶ��
//	LPWAVEOUTCAPS pwoc  -- һ��ָ�������豸�����ṹ��ָ��
//  UINT cbwoc  --  �ṹWAVEOUTCAPS�Ĵ�С����BYTEΪ��λ
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ������豸����
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetDevCaps\r\n");

	// �������˿�
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // �򿪶˿�ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	Params.uDeviceID=uDeviceID;
	Params.message=AM_IN_GETDEVCAPS;  // ������Ϣ
	Params.dwCallBackParam=0;  // ���ò���
	Params.lParam=(DWORD)pwic;
	Params.wParam=cbwic;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // ӳ��ָ��
	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
		CloseHandle(hWav);  // �رվ��
 		return MMSYSERR_ERROR;
	}
	// ��Ϣ�ɹ�
	CloseHandle(hWav);  // �رվ��
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveInGetDevCaps\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
//������
//	MMRESULT mmrError -- Ҫ�õ��ı��Ĵ�����Ϣ
//	LPTSTR pszText  --  ָ���Ŵ����ı������ָ��
//	UINT cchText  -- ����Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ����������ı�
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
	DbgOutString("+waveInGetErrorText\r\n");
	DbgOutString("-waveInGetErrorText\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi)
//������
//	HWAVEIN hwi  -- ���������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������ر���������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi)
{
	LPWAVSTRUCT lpwavStruct;
//	PROCBACK *lpProcBack;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInClose\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // ����û�д�
			return MMSYSERR_INVALHANDLE;
		}

		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // �ص�����
			DbgOutString("will Call back Window\r\n");
			SendMessage((HWND)lpwavStruct->dwCallback,MM_WIM_CLOSE,0,0);  // ���͹ر���Ϣ
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{ // �ص��߳�
			DbgOutString("will Call back Thread\r\n");
			return MMSYSERR_NOERROR;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{  // �ص��¼�
			DbgOutString("will Call back Event\r\n");
			//SetEvent(dwCallback);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{  // �ص�����
			DbgOutString("will Call back Function\r\n");

//			lpProcBack=lpwavStruct->dwCallback;
//			lpProcBack(lpwavStruct,WIM_CLOSE,lpwavStruct->dwInstance,0,0);
			CallApFunction(lpwavStruct,WIM_CLOSE,0,0);  // ���ú������͹ر���Ϣ
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // û�лص�
			DbgOutString("will Call back NULL\r\n");
		}
		else
		{  // ��Ч�ص�
			DbgOutString("Vaild Parameter\r\n");
		}

		CloseHandle(lpwavStruct->hWav); // �رվ��
		if (lpwavStruct->pwfx)  // �ͷ��ڴ�
			free(lpwavStruct->pwfx);
		free(lpwavStruct);  
		DbgOutString("-waveInClose\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEIN hwi  -- ����������
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������׼���������ݿ��ͷ
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	DbgOutString("+waveInPrepareHeader\r\n");

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );   // ӳ��ָ��
	pwh->dwFlags |= WHDR_PREPARED; // ����׼���õı�־

	DbgOutString("-waveInPrepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEIN hwi  -- ���������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������ж���������ݿ��ͷ
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	DbgOutString("+waveInUnprepareHeader\r\n");

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() ); 
	pwh->dwFlags &= ~WHDR_PREPARED;  // ȡ��׼����־

	DbgOutString("-waveInUnprepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEIN hwi  -- ���������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������豸��ȡ�������ݿ�
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInAddBuffer\r\n");

		pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // ӳ��ָ��
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // û�д�����
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID; 
		Params.message=AM_IN_ADDBUFFER; // ������Ϣ
		Params.dwCallBackParam=0;  // ���ò���
		Params.lParam=(DWORD)pwh;
		Params.wParam=cbwh;

		Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // ӳ��ָ��

		// ������Ϣ
		if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{ // ��Ϣ�豸
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInAddBuffer\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi)
//������
//	HWAVEIN hwi  -- ���������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//������������ʼ����¼��
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInStart\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{ // ����û�д�
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_START;  // ������Ϣ
		Params.dwCallBackParam=0; // ���ò���
		Params.lParam=0;
		Params.wParam=0;
 
		// ������Ϣ
		if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // ��Ϣʧ��
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInStart\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi)
//������
//	HWAVEIN hwi  -- ���������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������ֹͣ����¼��
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInStop\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // û�д�����
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_STOP;  // ������Ϣ
		Params.dwCallBackParam=0;  // ���ò���
		Params.lParam=0;
		Params.wParam=0;

		// ������Ϣ
		if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // ��Ϣʧ��
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInStop\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi)
//������
//	HWAVEIN hwi  -- ���������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������������������豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInReset\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // ����û�д�
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_RESET;  // ������Ϣ
		Params.dwCallBackParam=0; // ���ò���
		Params.lParam=0;
		Params.wParam=0;

		// ������Ϣ
		if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // ��Ϣʧ��
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInReset\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
//������
//	HWAVEIN hwi  -- ���������豸���
//	LPMMTIME pmmt -- ָ��洢λ�õĽṹ��ָ��
//	UINT cbmmt -- λ�õĽṹ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ��������ݵĳ���
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetPosition\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwi;

	if (lpwavStruct==NULL)
	{  // û�д�����
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_IN_GETPOS;  // ������Ϣ
	Params.dwCallBackParam=0; // ���ò���
	Params.lParam=(DWORD)pmmt;
	Params.wParam=cbmmt;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // Ӱ��ָ��
	// ������Ϣ
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveInGetPosition\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
//������
//	HWAVEIN hwi  -- ���������豸���
//	UINT *puDeviceID -- ָ���ŵ�ǰ���ŵ��豸��ʶ�ŵ�ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� �õ���ǰ¼�����豸��ʶ��
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
{
	DbgOutString("+waveInGetID\r\n");
	DbgOutString("-waveInGetID\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
//������
//	HWAVEIN hwi  -- ���������豸���
//	UINT uMsg  -- Ҫ���͵���Ϣ
//	DWORD dw1  -- ����
//	DWORD dw2  -- ����
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ����һ���Զ������Ϣ���豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
{
	DbgOutString("+waveInMessage\r\n");
	DbgOutString("-waveInMessage\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
//������
//	LPHWAVEIN phwi	--	������������豸����ľ��
//	UINT uDeviceID -- Ҫ�򿪵��豸�ı�ʶ��
//	PWAVEFORMATEX pwfx -- Ҫ��ʼ¼���������ĸ�ʽ
//	DWORD dwCallback -- �ص����
//	DWORD dwInstance -- ʵ�����
//	DWORD fdwOpen -- ��ģʽ

//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ��һ�����������豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	HANDLE hWav;
	AUDIOMDD_MSG  Params;
    AUDIOMDD_OPEN WOD;
	LPWAVSTRUCT lpwavStruct;
	PWAVEFORMATEX pnewwfx;
//	PROCBACK *lpProcBack;
	DWORD dwError = MMSYSERR_ERROR;


	DbgOutString("+waveInOpen\r\n");
	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // �ص�����
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // �ص��߳�
		DbgOutString("the call back thread is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED;  // Ŀǰ��֧��
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{ // �ص��¼�
		DbgOutString("the call back Event is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED; // Ŀǰ��֧��
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // �ص�����
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{ // û�лص�
	}
	else
	{ // �Ƿ��ص�
		DbgOutString("Invaild Parameter\r\n");
		return MMSYSERR_INVALPARAM;
	}
//	*phwo=NULL;
//	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	// �������˿�
	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{// �򿪶˿�ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	lpwavStruct=(LPWAVSTRUCT)malloc(sizeof(WAVSTRUCT));  // �����ڴ�

	if (lpwavStruct==NULL)
	{  // ����ʧ��
		DbgOutString("Memory is Not Enough");
		CloseHandle(hWav); // �رվ��
		return MMSYSERR_NOMEM;
	}
	EdbgOutputDebugString("lpwavStruct in Open %X\r\n",lpwavStruct);
	pnewwfx=(PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));  // �����ڴ�
	if (pnewwfx==NULL)
	{// ����ʧ��
		DbgOutString("Memory is Not Enough");
		free(lpwavStruct);  // �ͷ��Ѿ�������ڴ�
		CloseHandle(hWav); // �رվ��
		return MMSYSERR_NOMEM;
	}
	*pnewwfx=*pwfx;  // ���õ�ǰ������ģʽ

	lpwavStruct->hWav=hWav; // �����������
	lpwavStruct->uDeviceID=uDeviceID;
	lpwavStruct->pwfx=pnewwfx;  // ��������ģʽ
	lpwavStruct->dwCallback=dwCallback; // ���ûص�
	lpwavStruct->dwInstance=dwInstance; // ����ʵ�����
	lpwavStruct->fdwOpen=fdwOpen; // ���ô�ģʽ


//	WOD.hWave=NULL;
	WOD.lpFormat=pnewwfx;  // ���ýṹ
//	WOD.dwCallback=dwCallback;
	WOD.dwCallback=(DWORD)DrvCallBack;  // ���������ص�
	WOD.dwInstance=dwInstance;
//	WOD.uMappedDeviceID=uDeviceID;


	Params.uDeviceID=uDeviceID;
	Params.message=AM_IN_OPEN;  // ������Ϣ
	Params.dwCallBackParam=(DWORD)lpwavStruct;  // ���ûص�����
	Params.lParam=(DWORD)&WOD;  // ���ô�����
	Params.wParam=fdwOpen; // ���ô�ģʽ

	WOD.lpFormat = (PWAVEFORMATEX)MapPtrToProcess( (LPVOID)WOD.lpFormat, GetCurrentProcess() );  // ӳ��ָ��
	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // ӳ��ָ��
	Params.dwCallBackParam = (DWORD)MapPtrToProcess( (LPVOID)Params.dwCallBackParam, GetCurrentProcess() ); // ӳ��ָ��

	DbgOutString("Open Wav Device Success\r\n");
	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // ��Ϣʧ��
		free(pnewwfx);  // �ͷ��ڴ�
		free(lpwavStruct);
		CloseHandle(hWav); // �رվ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // �д������
		free(pnewwfx); // �ͷ��ڴ�
		free(lpwavStruct);
		CloseHandle(hWav); // �رվ��
		return dwError;
	}
	*phwi=lpwavStruct;  // ���ؾ��

	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // �ص�����
		DbgOutString("will Call back Window\r\n");
		SendMessage((HWND)dwCallback,MM_WIM_OPEN,0,0);  // ���ʹ���Ϣ
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // �ص��߳�
		DbgOutString("will Call back Thread\r\n");
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{  // �ص��¼�
		DbgOutString("will Call back Event\r\n");
		//SetEvent(dwCallback);
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // �ص�����
		DbgOutString("will Call back Function\r\n");
//		lpProcBack=dwCallback;
//		lpProcBack(lpwavStruct,WIM_OPEN,lpwavStruct->dwInstance,0,0);
		CallApFunction(lpwavStruct,WIM_OPEN,0,0);  // ���ûص�����

	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{  // û�лص�
		DbgOutString("will Call back NULL\r\n");
	}
	else
	{  // ��Ч�ص�
		DbgOutString("Vaild Parameter\r\n");
	}
	DbgOutString("-waveInOpen\r\n");
	return MMSYSERR_NOERROR;
}

// **************************************************
// ������void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1, DWORD dw2)
// ������
// 	IN hdrvr -- �豸���
// 	IN uMsg -- ��Ϣ
// 	IN dwCallBackParam -- �ص�����
// 	IN dw1 -- ����1
// 	IN dw2 -- ����2
// 
// ����ֵ����
// ������������������ص����������ṩ����������ص�ʹ�á�
// ����: 
// **************************************************
void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1, DWORD dw2)
//void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1)//, DWORD dw2)
{
	LPWAVSTRUCT lpwavStruct;
//	PROCBACK *lpProcBack;

		//DbgOutString("+CALLBACK DrvCallBack\r\n");
		lpwavStruct=(LPWAVSTRUCT)dwCallBackParam;
//		RETAILMSG(1,("+CALLBACK DrvCallBack\r\n"));
//		RETAILMSG(1,("lpwavStruct in CallBack %X\r\n",lpwavStruct));
//		RETAILMSG(1,("lpwavStruct->hWav = %X\r\n",lpwavStruct->hWav));
//		RETAILMSG(1,("lpwavStruct->uDeviceID = %X\r\n",lpwavStruct->uDeviceID));
//		RETAILMSG(1,("lpwavStruct->pwfx = %X\r\n",lpwavStruct->pwfx));
//		RETAILMSG(1,("lpwavStruct->dwCallback = %X\r\n",lpwavStruct->dwCallback));
//		RETAILMSG(1,("lpwavStruct->dwInstance = %X\r\n",lpwavStruct->dwInstance));
//		RETAILMSG(1,("lpwavStruct->fdwOpen = %X\r\n",lpwavStruct->fdwOpen));
		
		if (lpwavStruct==NULL)  // û�д�����
			return;

		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // �ص�����
			DbgOutString("will Call back Window\r\n");
//			PostMessage((HWND)lpwavStruct->dwCallback,uMsg,dw1,dw2);
			PostMessage((HWND)lpwavStruct->dwCallback,uMsg,dw1,0);  // ����ָ����Ϣ������
			return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{ // �ص��߳�
			DbgOutString("will Call back Thread\r\n");
			return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{ // �ص��¼�
			//SetEvent(lpwavStruct->dwCallback);
			DbgOutString("will Call back Event\r\n");
			return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{  // �ص�����
/*			CALLBACKDATA  CallbackData;
			
				RETAILMSG(1,("will Call back Function\r\n"));
				CallbackData.hProcess = lpwavStruct->hCallerProcess;
				CallbackData.lpfn = lpwavStruct->dwCallback;
				CallbackData.dwArg0 = lpwavStruct;
				
				RETAILMSG(1,("Call back Function ....\r\n"));
				Sys_ImplementCallBack4( &CallbackData, uMsg,lpwavStruct->dwInstance,dw1,dw2);
				RETAILMSG(1,("Call back Function OK\r\n"));
*/
//				CallApFunction(lpwavStruct,uMsg,dw1,dw2);
				CallApFunction(lpwavStruct,uMsg,dw1,dw2);  // ���ûص�����

//				lpProcBack=lpwavStruct->dwCallback;
//				lpProcBack(lpwavStruct,uMsg,lpwavStruct->dwInstance,dw1,dw2);
//				DbgOutString("will Call back Function\r\n");

				return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // û�лص�
			DbgOutString("will Call back NULL\r\n");
			return ;
		}
		else
		{  // �Ƿ��ص�
			DbgOutString("Vaild Parameter\r\n");
			return ;
		}
		//DbgOutString("-CALLBACK DrvCallBack\r\n");
//		RETAILMSG(1,("-CALLBACK DrvCallBack\r\n"));
		return ;
}
// **************************************************
// ������static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2)
// ������
// 	IN lpwavStruct -- �����ṹ
// 	IN uMsg -- �ص���Ϣ
// 	IN dw1 -- ����1
// 	IN dw2 -- ����2
// 
// ����ֵ����
// �����������ص�Ӧ�ó���ص�������
// ����: 
// **************************************************
static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2)
{
	CALLBACKDATA  CallbackData;
	
//		RETAILMSG(1, (TEXT( " ****** lpwavStruct->hCallerProcess = %X\r\n" ),lpwavStruct->hCallerProcess) );

//		RETAILMSG(1,("will Call back Function\r\n"));
		CallbackData.hProcess = lpwavStruct->hCallerProcess;  // ����Ӧ�ó�����̾��
		CallbackData.lpfn = (FARPROC)lpwavStruct->dwCallback; // ���ûص�����ָ��
		CallbackData.dwArg0 = (DWORD)lpwavStruct; // ���������ṹ
		
//		RETAILMSG(1,("Call back Function ....\r\n"));
		// ����ϵͳ�ص�����
//		Sys_ImplementCallBack4( &CallbackData, uMsg,lpwavStruct->dwInstance,dw1);//,dw2);
		Sys_ImplementCallBack( &CallbackData, uMsg,lpwavStruct->dwInstance,dw1,dw2);
//		RETAILMSG(1,("Call back Function OK\r\n"));
}

#else
#include "eWindows.h"
#include "EMMSys.h"
#include <Audio_ep.h>
//#include <waveddsi.h>
//#include <mmddk.h>
#include <eDevice.h>
#include <eobjcall.h>
#include <eapisrv.h>

#include "APICall.h"


// *****************************************************
// ����������
// *****************************************************
void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1, DWORD dw2);
//void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1);//, DWORD dw2);

typedef void (CALLBACK PROCBACK)( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance, DWORD lParam, DWORD wParam );
//typedef void (CALLBACK PROCBACK)( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance, DWORD lParam);//, DWORD wParam );


// *****************************************************
// ������
// *****************************************************
// hwo�������
typedef struct wavStruct{
	DWORD hWav;   // �����豸���
	UINT uDeviceID;  // �豸��ʶ��
	PWAVEFORMATEX pwfx;  // ������ʽ
	DWORD dwCallback;  // �ص������������Ǻ���(Function),���ھ��(HWND),...
	DWORD dwInstance;  // ʵ�����
	DWORD fdwOpen;   //�򿪷�ʽ
	HANDLE hCallerProcess;  // ���д���ʵ�����
}WAVSTRUCT, *LPWAVSTRUCT;

LPWAVSTRUCT g_lpwavStruct = NULL;



void CALLBACK ProcessFunction_API( DWORD dwWindowHWO,   
	  UINT uMsg,      
	  DWORD dwInstance,       
	  DWORD dwParam1, 
	  DWORD dwParam2 );

static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2);

/**********************************************************/

// ********************************************************************
//������UINT     WINAPI Audio_waveOutGetNumDevs(void)
//������
//	��
//����ֵ��
//	���ص�ǰϵͳ֧�ֵ��豸����
//�����������õ��豸��Ŀ
//����: wave API�����ӿ�
// ********************************************************************
UINT     WINAPI Audio_waveOutGetNumDevs(void)
{

	UINT numDevs;

		numDevs = Wnd_waveOutGetNumDevs();  // ����΢��API����
		return numDevs;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
//������
//	UINT uDeviceID   --  �豸��ʶ��
//	LPWAVEOUTCAPS pwoc  -- һ��ָ������豸������ָ��
//  UINT cbwoc  --  �ṹWAVEOUTCAPS�Ĵ�С����BYTEΪ��λ
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ��豸����
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
{
	MMRESULT  result = MMSYSERR_ERROR;
		
		result = Wnd_waveOutGetDevCaps(uDeviceID,pwoc,cbwoc);
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPDWORD pdwVolume  -- ָ����������ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ���ǰ��������С
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

	if (lpwavStruct==NULL)
	{  // û�д�����
		hWav = 0;
	}
	else
	{  // �Ѿ�������
		hWav = lpwavStruct->hWav;
	}

	result = Wnd_waveOutGetVolume(hWav,pdwVolume);

	return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	DWORD dwVolume  -- Ҫ����������С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������õ�ǰ��������С
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
	lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

	if (lpwavStruct==NULL)
	{  // û�д�����
		hWav = 0;
	}
	else
	{  // �Ѿ�������
		hWav = lpwavStruct->hWav;
	}

	result = Wnd_waveOutSetVolume(hWav, dwVolume);
	return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
//������
//	MMRESULT mmrError -- Ҫ�õ��ı��Ĵ�����Ϣ
//	LPTSTR pszText  --  ָ���Ŵ����ı������ָ��
//	UINT cchText  -- ����Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������õ�ǰ��������С
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
	wnd_waveOuGetErrorText(mmrError,pszText,cchText);
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������ر��������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutClose(hWav);

		if (lpwavStruct->pwfx) // �ͷſռ�
			free(lpwavStruct->pwfx);
		free(lpwavStruct);
		g_lpwavStruct = NULL;

		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������׼���������ݿ��ͷ
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // ӳ�����
//		pwh->dwFlags |= WHDR_PREPARED;    // �����Ѿ�׼���õı�־

		result = Wnd_waveOutPrepareHeader(hWav,pwh,cbwh);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������ж���������ݿ��ͷ
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
//		pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // ӳ�����
//		pwh->dwFlags |= WHDR_PREPARED;    // �����Ѿ�׼���õı�־

		result = Wnd_waveOutUnprepareHeader(hWav,pwh,cbwh);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������д�������ݿ鵽�豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutWrite(hWav,pwh,cbwh);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//������������ͣ��������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutPause(hWav);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������¿�ʼ��������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutRestart(hWav);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������¿�ʼ��������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutReset(hWav);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo)
//������
//	HWAVEOUT hwo  -- ��������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������ж�ѭ������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutBreakLoop(hWav);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPMMTIME pmmt -- ָ��洢λ�õĽṹ��ָ��
//	UINT cbmmt -- λ�õĽṹ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������ж�ѭ������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutGetPosition(hWav, pmmt, cbmmt);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPDWORD pdwPitch -- ָ��ǰ��������ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� �õ���ǰ������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutGetPitch(hWav, pdwPitch);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	DWORD dwPitch -- Ҫ���õ�����
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ���õ�ǰ������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutSetPitch(hWav, dwPitch);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	LPDWORD pdwRate -- ָ���ŵ�ǰ�ط����ʵ�ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� �õ���ǰ���ط�����
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutGetPlaybackRate(hWav, pdwRate);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	DWORD dwRate -- ��ǰҪ���õ��ط�����
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ���õ�ǰ���ط�����
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutSetPlaybackRate(hWav, dwRate);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	UINT *puDeviceID -- ָ���ŵ�ǰ���ŵ��豸��ʶ�ŵ�ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� �õ���ǰ���ŵ��豸��ʶ��
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutGetID(hWav, puDeviceID);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
//������
//	HWAVEOUT hwo  -- ��������豸���
//	UINT uMsg  -- Ҫ���͵���Ϣ
//	DWORD dw1  -- ����
//	DWORD dw2  -- ����
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ����һ���Զ������Ϣ���豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // �õ������ṹ

		if (lpwavStruct==NULL)
		{  // û�д�����
			hWav = 0;
		}
		else
		{  // �Ѿ�������
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutMessage(hWav, uMsg, dw1, dw2);
		return result ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
//������
//	LPHWAVEOUT phwo	--	�����������豸����ľ��
//	UINT uDeviceID -- Ҫ�򿪵��豸�ı�ʶ��
//	PWAVEFORMATEX pwfx -- Ҫ�򿪵������ĸ�ʽ
//	DWORD dwCallback -- �ص����
//	DWORD dwInstance -- ʵ�����
//	DWORD fdwOpen -- ��ģʽ

//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ��һ�������豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
//	HANDLE hWav;
//	AUDIOMDD_MSG  Params;
//    AUDIOMDD_OPEN WOD;
	LPWAVSTRUCT lpwavStruct;
	PWAVEFORMATEX pnewwfx;
//	DWORD dwError;

	MMRESULT result = MMSYSERR_ERROR;
//	PROCBACK *lpProcBack;
	
	RETAILMSG(1,(TEXT("API:waveOutOpen")));

	if (g_lpwavStruct)
	{
		RETAILMSG(1,(TEXT("The Audio Had Open \r\n")));
		return result;
	}

	//	*phwo=NULL;
	lpwavStruct=(LPWAVSTRUCT)malloc(sizeof(WAVSTRUCT));  // ����ṹ

	if (lpwavStruct==NULL)
	{  // ����ʧ��
		DbgOutString("Memory is Not Enough");
		return MMSYSERR_NOMEM; // ����ʧ��
	}

	pnewwfx=(PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));  // ����������ʽ�ṹ
	if (pnewwfx==NULL)
	{ // ����ʧ��
		free(lpwavStruct);  // �ͷ��ڴ�
		return MMSYSERR_NOMEM;  // ����ʧ��
	}
	*pnewwfx=*pwfx;  // ����������ʽ

	lpwavStruct->uDeviceID=uDeviceID;
	lpwavStruct->pwfx=pnewwfx;
	lpwavStruct->dwCallback=dwCallback;  // ���ûص�
	lpwavStruct->dwInstance=dwInstance; // ����ʵ�����
	lpwavStruct->fdwOpen=fdwOpen; // ���ô򿪷�ʽ
	lpwavStruct->hCallerProcess = GetCallerProcess();  // ���ú��н���

	g_lpwavStruct = lpwavStruct;

	result = Wnd_waveOutOpen(
			  &lpwavStruct->hWav,            
			  WAVE_MAPPER ,            
			  (LPWAVEFORMATEX)pwfx,       
			  (DWORD)ProcessFunction_API,          
			  0,  
			  CALLBACK_FUNCTION);



	*phwo=lpwavStruct;  // ���ص�ǰ�ṹ��Ϊ���


	return result;
}


// ********************************************************************
//������UINT     WINAPI Audio_waveInGetNumDevs(void)
//������
//	��
//����ֵ��
//	���ص�ǰϵͳ֧�ֵ��豸����
//���������� �õ���ǰ����������豸��Ŀ
//����: wave API�����ӿ�
// ********************************************************************
UINT     WINAPI Audio_waveInGetNumDevs(void)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	UINT numDevs;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetNumDevs\r\n");

	// �������˿�
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // ������ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return 0;
	}

//	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.uDeviceID=0;
	Params.message=AM_IN_GETNUMDEVS;  // ������Ϣ
	Params.dwCallBackParam=0;  // ���ò���
	Params.lParam=0;
	Params.wParam=0;

	// ������Ϣ
	numDevs=DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0);
	CloseHandle(hWav);  // �رվ��
	DbgOutString("-waveInGetNumDevs\r\n");
	return numDevs;  // �����豸����
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
//������
//	UINT uDeviceID   --  �豸��ʶ��
//	LPWAVEOUTCAPS pwoc  -- һ��ָ�������豸�����ṹ��ָ��
//  UINT cbwoc  --  �ṹWAVEOUTCAPS�Ĵ�С����BYTEΪ��λ
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ������豸����
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetDevCaps\r\n");

	// �������˿�
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // �򿪶˿�ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	Params.uDeviceID=uDeviceID;
	Params.message=AM_IN_GETDEVCAPS;  // ������Ϣ
	Params.dwCallBackParam=0;  // ���ò���
	Params.lParam=(DWORD)pwic;
	Params.wParam=cbwic;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // ӳ��ָ��
	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // ��Ϣʧ��
		CloseHandle(hWav);  // �رվ��
 		return MMSYSERR_ERROR;
	}
	// ��Ϣ�ɹ�
	CloseHandle(hWav);  // �رվ��
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveInGetDevCaps\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
//������
//	MMRESULT mmrError -- Ҫ�õ��ı��Ĵ�����Ϣ
//	LPTSTR pszText  --  ָ���Ŵ����ı������ָ��
//	UINT cchText  -- ����Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ����������ı�
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
	DbgOutString("+waveInGetErrorText\r\n");
	DbgOutString("-waveInGetErrorText\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi)
//������
//	HWAVEIN hwi  -- ���������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������ر���������
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi)
{
	LPWAVSTRUCT lpwavStruct;
//	PROCBACK *lpProcBack;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInClose\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // ����û�д�
			return MMSYSERR_INVALHANDLE;
		}

		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // �ص�����
			DbgOutString("will Call back Window\r\n");
			SendMessage((HWND)lpwavStruct->dwCallback,MM_WIM_CLOSE,0,0);  // ���͹ر���Ϣ
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{ // �ص��߳�
			DbgOutString("will Call back Thread\r\n");
			return MMSYSERR_NOERROR;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{  // �ص��¼�
			DbgOutString("will Call back Event\r\n");
			//SetEvent(dwCallback);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{  // �ص�����
			DbgOutString("will Call back Function\r\n");

//			lpProcBack=lpwavStruct->dwCallback;
//			lpProcBack(lpwavStruct,WIM_CLOSE,lpwavStruct->dwInstance,0,0);
			CallApFunction(lpwavStruct,WIM_CLOSE,0,0);  // ���ú������͹ر���Ϣ
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // û�лص�
			DbgOutString("will Call back NULL\r\n");
		}
		else
		{  // ��Ч�ص�
			DbgOutString("Vaild Parameter\r\n");
		}

		CloseHandle((HANDLE)lpwavStruct->hWav); // �رվ��
		if (lpwavStruct->pwfx)  // �ͷ��ڴ�
			free(lpwavStruct->pwfx);
		free(lpwavStruct);  
		DbgOutString("-waveInClose\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEIN hwi  -- ����������
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������׼���������ݿ��ͷ
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	DbgOutString("+waveInPrepareHeader\r\n");

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );   // ӳ��ָ��
	pwh->dwFlags |= WHDR_PREPARED; // ����׼���õı�־

	DbgOutString("-waveInPrepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEIN hwi  -- ���������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������ж���������ݿ��ͷ
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	DbgOutString("+waveInUnprepareHeader\r\n");

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() ); 
	pwh->dwFlags &= ~WHDR_PREPARED;  // ȡ��׼����־

	DbgOutString("-waveInUnprepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//������
//	HWAVEIN hwi  -- ���������豸���
//	LPWAVEHDR pwh -- ָ��һ���������ݿ��ָ��
//	UINT cbwh -- �������ݿ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�������������豸��ȡ�������ݿ�
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInAddBuffer\r\n");

		pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // ӳ��ָ��
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // û�д�����
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID; 
		Params.message=AM_IN_ADDBUFFER; // ������Ϣ
		Params.dwCallBackParam=0;  // ���ò���
		Params.lParam=(DWORD)pwh;
		Params.wParam=cbwh;

		Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // ӳ��ָ��

		// ������Ϣ
		if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{ // ��Ϣ�豸
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInAddBuffer\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi)
//������
//	HWAVEIN hwi  -- ���������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//������������ʼ����¼��
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInStart\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{ // ����û�д�
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_START;  // ������Ϣ
		Params.dwCallBackParam=0; // ���ò���
		Params.lParam=0;
		Params.wParam=0;
 
		// ������Ϣ
		if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // ��Ϣʧ��
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInStart\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi)
//������
//	HWAVEIN hwi  -- ���������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//����������ֹͣ����¼��
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInStop\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // û�д�����
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_STOP;  // ������Ϣ
		Params.dwCallBackParam=0;  // ���ò���
		Params.lParam=0;
		Params.wParam=0;

		// ������Ϣ
		if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // ��Ϣʧ��
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInStop\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi)
//������
//	HWAVEIN hwi  -- ���������豸���
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������������������豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInReset\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // ����û�д�
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_RESET;  // ������Ϣ
		Params.dwCallBackParam=0; // ���ò���
		Params.lParam=0;
		Params.wParam=0;

		// ������Ϣ
		if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // ��Ϣʧ��
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInReset\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
//������
//	HWAVEIN hwi  -- ���������豸���
//	LPMMTIME pmmt -- ָ��洢λ�õĽṹ��ָ��
//	UINT cbmmt -- λ�õĽṹ�Ĵ�С
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//�����������õ��������ݵĳ���
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetPosition\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwi;

	if (lpwavStruct==NULL)
	{  // û�д�����
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_IN_GETPOS;  // ������Ϣ
	Params.dwCallBackParam=0; // ���ò���
	Params.lParam=(DWORD)pmmt;
	Params.wParam=cbmmt;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // Ӱ��ָ��
	// ������Ϣ
	if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // ��Ϣʧ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveInGetPosition\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
//������
//	HWAVEIN hwi  -- ���������豸���
//	UINT *puDeviceID -- ָ���ŵ�ǰ���ŵ��豸��ʶ�ŵ�ָ��
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� �õ���ǰ¼�����豸��ʶ��
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
{
	DbgOutString("+waveInGetID\r\n");
	DbgOutString("-waveInGetID\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
//������
//	HWAVEIN hwi  -- ���������豸���
//	UINT uMsg  -- Ҫ���͵���Ϣ
//	DWORD dw1  -- ����
//	DWORD dw2  -- ����
//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ����һ���Զ������Ϣ���豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
{
	DbgOutString("+waveInMessage\r\n");
	DbgOutString("-waveInMessage\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//������MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
//������
//	LPHWAVEIN phwi	--	������������豸����ľ��
//	UINT uDeviceID -- Ҫ�򿪵��豸�ı�ʶ��
//	PWAVEFORMATEX pwfx -- Ҫ��ʼ¼���������ĸ�ʽ
//	DWORD dwCallback -- �ص����
//	DWORD dwInstance -- ʵ�����
//	DWORD fdwOpen -- ��ģʽ

//����ֵ��
//	�ɹ�����MMSYSERR_NOERROR,����ʧ��
//���������� ��һ�����������豸
//����: wave API�����ӿ�
// ********************************************************************
MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	HANDLE hWav;
	AUDIOMDD_MSG  Params;
    AUDIOMDD_OPEN WOD;
	LPWAVSTRUCT lpwavStruct;
	PWAVEFORMATEX pnewwfx;
//	PROCBACK *lpProcBack;
	DWORD dwError = MMSYSERR_ERROR;


	DbgOutString("+waveInOpen\r\n");
	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // �ص�����
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // �ص��߳�
		DbgOutString("the call back thread is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED;  // Ŀǰ��֧��
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{ // �ص��¼�
		DbgOutString("the call back Event is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED; // Ŀǰ��֧��
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // �ص�����
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{ // û�лص�
	}
	else
	{ // �Ƿ��ص�
		DbgOutString("Invaild Parameter\r\n");
		return MMSYSERR_INVALPARAM;
	}
//	*phwo=NULL;
//	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	// �������˿�
	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{// �򿪶˿�ʧ��
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	lpwavStruct=(LPWAVSTRUCT)malloc(sizeof(WAVSTRUCT));  // �����ڴ�

	if (lpwavStruct==NULL)
	{  // ����ʧ��
		DbgOutString("Memory is Not Enough");
		CloseHandle(hWav); // �رվ��
		return MMSYSERR_NOMEM;
	}
	EdbgOutputDebugString("lpwavStruct in Open %X\r\n",lpwavStruct);
	pnewwfx=(PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));  // �����ڴ�
	if (pnewwfx==NULL)
	{// ����ʧ��
		DbgOutString("Memory is Not Enough");
		free(lpwavStruct);  // �ͷ��Ѿ�������ڴ�
		CloseHandle(hWav); // �رվ��
		return MMSYSERR_NOMEM;
	}
	*pnewwfx=*pwfx;  // ���õ�ǰ������ģʽ

	lpwavStruct->hWav=(DWORD)hWav; // �����������
	lpwavStruct->uDeviceID=uDeviceID;
	lpwavStruct->pwfx=pnewwfx;  // ��������ģʽ
	lpwavStruct->dwCallback=dwCallback; // ���ûص�
	lpwavStruct->dwInstance=dwInstance; // ����ʵ�����
	lpwavStruct->fdwOpen=fdwOpen; // ���ô�ģʽ


//	WOD.hWave=NULL;
//	WOD.lpFormat=pnewwfx;  // ���ýṹ
//	WOD.dwCallback=dwCallback;
//	WOD.dwCallback=(DWORD)DrvCallBack;  // ���������ص�
//	WOD.dwInstance=dwInstance;
//	WOD.uMappedDeviceID=uDeviceID;


	Params.uDeviceID=uDeviceID;
	Params.message=AM_IN_OPEN;  // ������Ϣ
	Params.dwCallBackParam=(DWORD)lpwavStruct;  // ���ûص�����
	Params.lParam=(DWORD)&WOD;  // ���ô�����
	Params.wParam=fdwOpen; // ���ô�ģʽ

	WOD.lpFormat = (PWAVEFORMATEX)MapPtrToProcess( (LPVOID)WOD.lpFormat, GetCurrentProcess() );  // ӳ��ָ��
	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // ӳ��ָ��
	Params.dwCallBackParam = (DWORD)MapPtrToProcess( (LPVOID)Params.dwCallBackParam, GetCurrentProcess() ); // ӳ��ָ��

	DbgOutString("Open Wav Device Success\r\n");
	// ������Ϣ
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // ��Ϣʧ��
		free(pnewwfx);  // �ͷ��ڴ�
		free(lpwavStruct);
		CloseHandle(hWav); // �رվ��
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // �д������
		free(pnewwfx); // �ͷ��ڴ�
		free(lpwavStruct);
		CloseHandle(hWav); // �رվ��
		return dwError;
	}
	*phwi=lpwavStruct;  // ���ؾ��

	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // �ص�����
		DbgOutString("will Call back Window\r\n");
		SendMessage((HWND)dwCallback,MM_WIM_OPEN,0,0);  // ���ʹ���Ϣ
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // �ص��߳�
		DbgOutString("will Call back Thread\r\n");
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{  // �ص��¼�
		DbgOutString("will Call back Event\r\n");
		//SetEvent(dwCallback);
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // �ص�����
		DbgOutString("will Call back Function\r\n");
//		lpProcBack=dwCallback;
//		lpProcBack(lpwavStruct,WIM_OPEN,lpwavStruct->dwInstance,0,0);
		CallApFunction(lpwavStruct,WIM_OPEN,0,0);  // ���ûص�����

	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{  // û�лص�
		DbgOutString("will Call back NULL\r\n");
	}
	else
	{  // ��Ч�ص�
		DbgOutString("Vaild Parameter\r\n");
	}
	DbgOutString("-waveInOpen\r\n");
	return MMSYSERR_NOERROR;
}

// **************************************************
// ������void CALLBACK ProcessFunction( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,       
//	  DWORD dwParam1,  DWORD dwParam2 )
// ������
// 	IN hwi -- �������
// 	IN uMsg -- ������Ϣ
// 	IN dwInstance -- ʵ�����
// 	IN dwParam1 -- ����1
// 	IN dwParam2 -- ����2
// 
// ����ֵ����
// ����������������Ϣ��������
// ����: 
// **************************************************
void CALLBACK ProcessFunction_API( DWORD dwWindowHWO,   
	  UINT uMsg,      
	  DWORD dwInstance,       
	  DWORD dwParam1, 
	  DWORD dwParam2 )
{
//	CALLBACKDATA  CallbackData;
	LPWAVSTRUCT lpwavStruct;
	
//		RETAILMSG(1, (TEXT( " ****** lpwavStruct->hCallerProcess = %X\r\n" ),lpwavStruct->hCallerProcess) );
		
		if (g_lpwavStruct == NULL)
			return ;

		lpwavStruct = g_lpwavStruct;
		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // Ҫ�󷵻ش���
			SendMessage((HWND)lpwavStruct->dwCallback,uMsg,dwParam1,dwParam2);  // ���͹ر���������Ϣ������
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{  // Ҫ��ص��߳�
			DbgOutString("will Call back Thread\r\n");
			return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{  // Ҫ��ص��¼�
			DbgOutString("will Call back Event\r\n");
			//SetEvent(dwCallback);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{ // Ҫ��ص�����
			RETAILMSG(1,(TEXT("will Call back Function [%x]\r\n"),uMsg));
			
			CallApFunction(lpwavStruct,uMsg,dwParam1,dwParam2);  // �ص�����֪ͨҪ�ر�����

//			lpProcBack=lpwavStruct->dwCallback;
//			lpProcBack(lpwavStruct,WOM_CLOSE,lpwavStruct->dwInstance,0,0);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // û��Ҫ��ص�
			DbgOutString("will Call back NULL\r\n");
		}
		else
		{  // û�����ûص�
			DbgOutString("Vaild Parameter\r\n");
		}
}
// **************************************************
// ������static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2)
// ������
// 	IN lpwavStruct -- �����ṹ
// 	IN uMsg -- �ص���Ϣ
// 	IN dw1 -- ����1
// 	IN dw2 -- ����2
// 
// ����ֵ����
// �����������ص�Ӧ�ó���ص�������
// ����: 
// **************************************************
static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2)
{
	CALLBACKDATA  CallbackData;
	
//		RETAILMSG(1, (TEXT( " ****** lpwavStruct->hCallerProcess = %X\r\n" ),lpwavStruct->hCallerProcess) );

//		RETAILMSG(1,("will Call back Function\r\n"));
		CallbackData.hProcess = lpwavStruct->hCallerProcess;  // ����Ӧ�ó�����̾��
		CallbackData.lpfn = (FARPROC)lpwavStruct->dwCallback; // ���ûص�����ָ��
		CallbackData.dwArg0 = (DWORD)lpwavStruct; // ���������ṹ
		
		RETAILMSG(1,("Call back Function Parameter[%x][%x][%x][%x][%x]....\r\n",lpwavStruct,uMsg,lpwavStruct->dwInstance,dw1,dw2));
		// ����ϵͳ�ص�����
		Sys_ImplementCallBack( &CallbackData, uMsg,lpwavStruct->dwInstance,dw1,dw2);
//		RETAILMSG(1,("Call back Function OK\r\n"));
}


#endif