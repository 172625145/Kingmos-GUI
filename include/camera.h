#ifndef	__CAMERA_H_
#define	__CAMERA_H_
/*****************************************************
文件说明：照相机api函数接口
版本号：1.0.0
开发时期：2004-11-3
作者：谭宁飞 tanphy
修改记录：
******************************************************/
////照相机光源的强度和频率
//光源的强度值
#define SOUCELIGHT_NORMAL 1 //一般亮度
#define SOUCELIGHT_LOW 2 //低亮度
#define SOUCELIGHT_DAYLIGHT 3 //白天，室外
#define SOUCELIGHT_CLOUDY 4 //阴天，室外
#define SOUCELIGHT_FLUORESCENT 5 //莹光光源
#define SOUCELIGHT_INCANDESCENT 6 //混合光光源

//光源的频率
#define SOUCELIGHT_FREQUENCE_NOCHANGE 1 //不改变频率
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

	UINT16 nMaxImgWidth;//照相机相片最大水平像素点数
	UINT16 nMaxImgHeight;//照相机相片最大垂直像素点数
	
	UINT16 nCurImgWidth;//照相机相片当前水平像素点数
	UINT16 nCurImgHeight;//照相机相片当前垂直像素点数
	
	UINT8  nCurImgFmt;//照相机图像格式
	UINT8  nCaptureMode; //拍照模式:SINGLE_CAPTUREMODE ,MULTI_CAPTUREMODE 
	UINT8  nImgQualityLevelNum;//照相机图像质量级数
	UINT8  nCurImgQualityLevel;//照相机当前图像质量级别, 0为最低,依次递增
	
	UINT8  nCurSourceLightVal;//当前光源强度值
	UINT8  nSourceLightFreqVal;//当前光源频率值
	UINT16 nReserve1;
	
	UINT16 nCurWndPosX;//照相机窗口当前相对于屏幕的水平位置
	UINT16 nCurWndPosY;//照相机窗口当前相对于屏幕的垂直位置
	
	UINT16 nCurWndWidth;//当前照相机窗口宽度
	UINT16 nCurWndHeight;//当前照相机窗口高度

	UINT16 nMaxDZMultiple; //数码变焦的最大倍数
	UINT16 nCurDZMultiple; //当前使用的数码变焦倍数
}CAMERA_PROPERTY, *PCAMERA_PROPERTY;

// **********************************************************************
// 声明：BOOL Cam_GetPorperty(HANDLE hFile, PCAMERA_PROPERTY pCamProperty)
// 参数：
//	IN - hFile--相机设备文件句柄
//	OUT - pCamProperty -- 指向CAMERA_PROPERTY
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：得到照相机属性
// **********************************************************************
BOOL Cam_GetPorperty(HANDLE hFile, PCAMERA_PROPERTY pCamProperty);

// **************************************************
// 声明：BOOL Cam_Activate(HANDLE hFile);
// 参数：
//	IN - hFile--相机设备文件句柄
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：激活相机，预览
// **************************************************
BOOL Cam_Activate(HANDLE hFile);

// **************************************************
// 声明：BOOL Cam_DeActivate(HANDLE hFile);
// 参数：
//	IN - hFile--相机设备文件句柄
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：关闭相机
// **************************************************
BOOL Cam_DeActivate(HANDLE hFile);

// **************************************************
// 声明：BOOL Cam_CapturePhoto(HANDLE hFile, UINT16 nCaptureNum)
// 参数：
//	IN - hFile--相机设备文件句柄
//	IN - nCaptureNum -- 在连拍模式时需要指定连拍的张数，单拍模式下可忽略该值
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：拍照功能
// **************************************************
BOOL Cam_CapturePhoto(HANDLE hFile, UINT16 nCaptureNum);

// **************************************************
// 声明：BOOL Cam_DwnPhotoFile(HANDLE hFile, TCHAR *pszFileName, UINT16 nPhotoIndex)
// 参数：
//	IN - hFile--相机设备文件句柄
//  IN - pszFileName -- 指定的包含全路径的文件名
//	IN - nPhotoIndex -- 像片索引号,连拍模式时需要指定索引号得到相应的相片,单拍模式下该值不生效
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：将拍下的照片下载到指定文件
// **************************************************
BOOL Cam_DwnPhotoFile(HANDLE hFile, TCHAR *pszFileName, UINT16 nPhotoIndex);

// **************************************************
// 声明：BOOL Cam_StickMarker(HANDLE hFile, TCHAR *pszStickFile);
// 参数：
//	IN - hFile--相机设备文件句柄
//  IN - pszFileName -- 指定的包含全路径的像框文件名
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：像框功能
// **************************************************
BOOL Cam_StickMarker(HANDLE hFile, TCHAR *pszStickFile);

// **************************************************
// 声明：BOOL Cam_SetSourceLight(HANDLE hFile, UINT8 nSourceLightVal, UINT8 nSourceLightFreqVal)
// 参数：
//	IN - hFile--相机设备文件句柄
//  IN - nSourceLightVal -- 外部光源亮度值
//	IN - nSourceLightFreqVal -- 外部光源频率值
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：设置相机曝光度,相机根据外部光源的强度和频率自动设置曝光时间
// **************************************************
BOOL Cam_SetSourceLight(HANDLE hFile, UINT8 nSourceLightVal, UINT8 nSourceLightFreqVal);

// **************************************************
// 声明：BOOL Cam_DigtalZoom(HANDLE hFile, UINT16 nDZMultiple)
// 参数：
//	IN - hFile--相机设备文件句柄
//  IN - nDZMultiple -- 数码变焦的倍数
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：像机数码变焦(镜头伸缩)
// **************************************************
BOOL Cam_DigtalZoom(HANDLE hFile, UINT16 nDZMultiple);

// **************************************************
// 声明：BOOL Cam_SetWindow(HANDLE hFile, RECT* prt);
// 参数：
//	IN - hFile--相机设备文件句柄
//	IN - prt -- 窗口的位置和大小
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：设置照相窗口的位置和大小
// **************************************************
BOOL Cam_SetWindow(HANDLE hFile, RECT* prt);

// **************************************************
// 声明：BOOL Cam_SetPhotoSize(HANDLE hFile, UINT16 nImageWidth, UINT16 nImageHeight)
// 参数：
//	IN - hFile--相机设备文件句柄
//	IN - nImageWidth -- 相片宽度
//	IN - nImageHeight -- 相片高度
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：设置照相机相片的大小
// **************************************************
BOOL Cam_SetPhotoSize(HANDLE hFile, UINT16 nImageWidth, UINT16 nImageHeight);

// **************************************************
// 声明：BOOL Cam_SetPhotoQuality(HANDLE hFile, UINT8 nPhotoQualityLevel);
// 参数：
//	IN - hFile--相机设备文件句柄
//	IN - nPhotoQualityLevel --图像质量级别
// 返回值：成功返回TRUE, 失败返回FALSE
// 功能描述：设置照机图像质量
// **************************************************
BOOL Cam_SetPhotoQuality(HANDLE hFile, UINT8 nPhotoQualityLevel);

// **************************************************
// 声明：DWORD Cam_GetPhotoSize(HANDLE hFile, INT16 nPhotoIndex);
// 参数：
//	IN - hFile--相机设备文件句柄
//  IN - nPhotoIndex 相片索引号,只有在连拍时需要指定。
// 返回值：像片文件的大小
// 功能描述：得到拍下的相片文件大小
// **************************************************
DWORD Cam_GetPhotoFileSize(HANDLE hFile, INT16 nPhotoIndex);

// **************************************************
// 声明：BOOL Cam_SetCaptureMode(HANDLE hFile, UINT8 nCaptureMode);
// 参数：
//	IN - hFile--相机设备文件句柄
//  IN - nCaptureMode -- 拍照模式,可为以下值:
//						SINGLE_CAPTUREMODE 单拍模式
//						MULTI_CAPTUREMODE 连拍模式
// 返回值：像片文件的大小
// 功能描述：设置照相机拍照模式
// **************************************************
BOOL Cam_SetCaptureMode(HANDLE hFile, UINT8 nCaptureMode);

#endif //__CFM_CMDS_H_
