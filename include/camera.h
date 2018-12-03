#ifndef	__CAMERA_H_
#define	__CAMERA_H_
/*****************************************************
�ļ�˵���������api�����ӿ�
�汾�ţ�1.0.0
����ʱ�ڣ�2004-11-3
���ߣ�̷���� tanphy
�޸ļ�¼��
******************************************************/
////�������Դ��ǿ�Ⱥ�Ƶ��
//��Դ��ǿ��ֵ
#define SOUCELIGHT_NORMAL 1 //һ������
#define SOUCELIGHT_LOW 2 //������
#define SOUCELIGHT_DAYLIGHT 3 //���죬����
#define SOUCELIGHT_CLOUDY 4 //���죬����
#define SOUCELIGHT_FLUORESCENT 5 //Ө���Դ
#define SOUCELIGHT_INCANDESCENT 6 //��Ϲ��Դ

//��Դ��Ƶ��
#define SOUCELIGHT_FREQUENCE_NOCHANGE 1 //���ı�Ƶ��
#define SOUCELIGHT_FREQUENCE_50HZ 2
#define SOUCELIGHT_FREQUENCE_60HZ 3

#define SINGLE_CAPTUREMODE 0
#define MULTI_CAPTUREMODE 1
#define IOCTL_GETPROPERTY 1
#define IOCTL_ACTIVECAMERA 2
#define IOCTL_DEACTIVECAMERA 3
#define IOCTL_CAPTUREPHOTO 4
#define IOCTL_DWNPHOTOTOFILE 5
#define IOCTL_SETPHOTOSIZE 6
#define IOCTL_SETLIGHT 7
#define IOCTL_ZOOM 8
#define IOCTL_STICKMARKER 9
#define IOCTL_CONTINUEPHOTO 10
#define IOCTL_SETWINDOW 11
#define IOCTL_SETPHOTOQUALITY 13
#define IOCTL_GETPHOTOFILESIZE 14
#define IOCTL_SETCAPTUREMODE 15

typedef struct _CAMERA_PROPERTY{
	UINT32 nSize;

	UINT16 nMaxImgWidth;//�������Ƭ���ˮƽ���ص���
	UINT16 nMaxImgHeight;//�������Ƭ���ֱ���ص���
	
	UINT16 nCurImgWidth;//�������Ƭ��ǰˮƽ���ص���
	UINT16 nCurImgHeight;//�������Ƭ��ǰ��ֱ���ص���
	
	UINT8  nCurImgFmt;//�����ͼ���ʽ
	UINT8  nCaptureMode; //����ģʽ:SINGLE_CAPTUREMODE ,MULTI_CAPTUREMODE 
	UINT8  nImgQualityLevelNum;//�����ͼ����������
	UINT8  nCurImgQualityLevel;//�������ǰͼ����������, 0Ϊ���,���ε���
	
	UINT8  nCurSourceLightVal;//��ǰ��Դǿ��ֵ
	UINT8  nSourceLightFreqVal;//��ǰ��ԴƵ��ֵ
	UINT16 nReserve1;
	
	UINT16 nCurWndPosX;//��������ڵ�ǰ�������Ļ��ˮƽλ��
	UINT16 nCurWndPosY;//��������ڵ�ǰ�������Ļ�Ĵ�ֱλ��
	
	UINT16 nCurWndWidth;//��ǰ��������ڿ��
	UINT16 nCurWndHeight;//��ǰ��������ڸ߶�

	UINT16 nMaxDZMultiple; //����佹�������
	UINT16 nCurDZMultiple; //��ǰʹ�õ�����佹����
}CAMERA_PROPERTY, *PCAMERA_PROPERTY;

// **********************************************************************
// ������BOOL Cam_GetPorperty(HANDLE hFile, PCAMERA_PROPERTY pCamProperty)
// ������
//	IN - hFile--����豸�ļ����
//	OUT - pCamProperty -- ָ��CAMERA_PROPERTY
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// �����������õ����������
// **********************************************************************
BOOL Cam_GetPorperty(HANDLE hFile, PCAMERA_PROPERTY pCamProperty);

// **************************************************
// ������BOOL Cam_Activate(HANDLE hFile);
// ������
//	IN - hFile--����豸�ļ����
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// �������������������Ԥ��
// **************************************************
BOOL Cam_Activate(HANDLE hFile);

// **************************************************
// ������BOOL Cam_DeActivate(HANDLE hFile);
// ������
//	IN - hFile--����豸�ļ����
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// �����������ر����
// **************************************************
BOOL Cam_DeActivate(HANDLE hFile);

// **************************************************
// ������BOOL Cam_CapturePhoto(HANDLE hFile, UINT16 nCaptureNum)
// ������
//	IN - hFile--����豸�ļ����
//	IN - nCaptureNum -- ������ģʽʱ��Ҫָ�����ĵ�����������ģʽ�¿ɺ��Ը�ֵ
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// �������������չ���
// **************************************************
BOOL Cam_CapturePhoto(HANDLE hFile, UINT16 nCaptureNum);

// **************************************************
// ������BOOL Cam_DwnPhotoFile(HANDLE hFile, TCHAR *pszFileName, UINT16 nPhotoIndex)
// ������
//	IN - hFile--����豸�ļ����
//  IN - pszFileName -- ָ���İ���ȫ·�����ļ���
//	IN - nPhotoIndex -- ��Ƭ������,����ģʽʱ��Ҫָ�������ŵõ���Ӧ����Ƭ,����ģʽ�¸�ֵ����Ч
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// ���������������µ���Ƭ���ص�ָ���ļ�
// **************************************************
BOOL Cam_DwnPhotoFile(HANDLE hFile, TCHAR *pszFileName, UINT16 nPhotoIndex);

// **************************************************
// ������BOOL Cam_StickMarker(HANDLE hFile, TCHAR *pszStickFile);
// ������
//	IN - hFile--����豸�ļ����
//  IN - pszFileName -- ָ���İ���ȫ·��������ļ���
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// ���������������
// **************************************************
BOOL Cam_StickMarker(HANDLE hFile, TCHAR *pszStickFile);

// **************************************************
// ������BOOL Cam_SetSourceLight(HANDLE hFile, UINT8 nSourceLightVal, UINT8 nSourceLightFreqVal)
// ������
//	IN - hFile--����豸�ļ����
//  IN - nSourceLightVal -- �ⲿ��Դ����ֵ
//	IN - nSourceLightFreqVal -- �ⲿ��ԴƵ��ֵ
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// ������������������ع��,��������ⲿ��Դ��ǿ�Ⱥ�Ƶ���Զ������ع�ʱ��
// **************************************************
BOOL Cam_SetSourceLight(HANDLE hFile, UINT8 nSourceLightVal, UINT8 nSourceLightFreqVal);

// **************************************************
// ������BOOL Cam_DigtalZoom(HANDLE hFile, UINT16 nDZMultiple)
// ������
//	IN - hFile--����豸�ļ����
//  IN - nDZMultiple -- ����佹�ı���
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// �����������������佹(��ͷ����)
// **************************************************
BOOL Cam_DigtalZoom(HANDLE hFile, UINT16 nDZMultiple);

// **************************************************
// ������BOOL Cam_SetWindow(HANDLE hFile, RECT* prt);
// ������
//	IN - hFile--����豸�ļ����
//	IN - prt -- ���ڵ�λ�úʹ�С
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// �����������������ര�ڵ�λ�úʹ�С
// **************************************************
BOOL Cam_SetWindow(HANDLE hFile, RECT* prt);

// **************************************************
// ������BOOL Cam_SetPhotoSize(HANDLE hFile, UINT16 nImageWidth, UINT16 nImageHeight)
// ������
//	IN - hFile--����豸�ļ����
//	IN - nImageWidth -- ��Ƭ���
//	IN - nImageHeight -- ��Ƭ�߶�
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// ���������������������Ƭ�Ĵ�С
// **************************************************
BOOL Cam_SetPhotoSize(HANDLE hFile, UINT16 nImageWidth, UINT16 nImageHeight);

// **************************************************
// ������BOOL Cam_SetPhotoQuality(HANDLE hFile, UINT8 nPhotoQualityLevel);
// ������
//	IN - hFile--����豸�ļ����
//	IN - nPhotoQualityLevel --ͼ����������
// ����ֵ���ɹ�����TRUE, ʧ�ܷ���FALSE
// ���������������ջ�ͼ������
// **************************************************
BOOL Cam_SetPhotoQuality(HANDLE hFile, UINT8 nPhotoQualityLevel);

// **************************************************
// ������DWORD Cam_GetPhotoSize(HANDLE hFile, INT16 nPhotoIndex);
// ������
//	IN - hFile--����豸�ļ����
//  IN - nPhotoIndex ��Ƭ������,ֻ��������ʱ��Ҫָ����
// ����ֵ����Ƭ�ļ��Ĵ�С
// �����������õ����µ���Ƭ�ļ���С
// **************************************************
DWORD Cam_GetPhotoFileSize(HANDLE hFile, INT16 nPhotoIndex);

// **************************************************
// ������BOOL Cam_SetCaptureMode(HANDLE hFile, UINT8 nCaptureMode);
// ������
//	IN - hFile--����豸�ļ����
//  IN - nCaptureMode -- ����ģʽ,��Ϊ����ֵ:
//						SINGLE_CAPTUREMODE ����ģʽ
//						MULTI_CAPTUREMODE ����ģʽ
// ����ֵ����Ƭ�ļ��Ĵ�С
// �����������������������ģʽ
// **************************************************
BOOL Cam_SetCaptureMode(HANDLE hFile, UINT8 nCaptureMode);

#endif //__CFM_CMDS_H_
