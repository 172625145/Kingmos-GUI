/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __GPSAPI_H
#define __GPSAPI_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum{
	GPSID_NULL,//         0x0
	//常用 message id
	//Time, position and fix type data.
	GPSID_GPGGA,//			0x1	
	//Latitude, longitude, UTC time of position fix and status
	GPSID_GPGLL,//			0x2	
	//GPS receiver operating mode, satellites used in the position solution,
	//and DOP values.
	GPSID_GPGSA,//			0x3	
	//The number of GPS satellites in view satellite ID numbers, elevation,
	//azimuth, and SNR values.
	GPSID_GPGSV,//			0x4	//GNSS satellites in view 
	//Time, date, position, course and speed data.
	GPSID_GPRMC,//			0x5	//Recommended minimum specific GNSS data 
	//Course and speed information relative to the ground.
	GPSID_GPVTG,//			0x6	//Course over ground and ground speed 
	//别的ID
	GPSID_GPALM,//			0x7	//GPS almanac data 
	GPSID_GPGRS,//			0x8	//GPS range residuals 
	GPSID_GPGST,//			0x9	//GPS pseudorange noise statistics
	//Signal-to-noise ratio, signal strength, frequency, and bit rate from a
	//radio-beacon receiver.
	GPSID_GPMSS,//			0xA	//Beacon receiver status 
	GPSID_GPZDA,//			0xB	//UTC and local date/time data 

	GPSID_MAX_ID
};
// 定义GPS设备能力
//常用设备功能位
#define GPSBIT_GPNULL           0x00000001
#define	GPSBIT_GPGGA			0x00000002	
#define	GPSBIT_GPGLL			0x00000004
#define	GPSBIT_GPGSA			0x00000008	
#define	GPSBIT_GPGSV			0x00000010
#define	GPSBIT_GPRMC			0x00000020
#define	GPSBIT_GPVTG			0x00000040
//别的设备功能位
#define	GPSBIT_GPALM			0x00000080
#define	GPSBIT_GPGRS			0x00000100
#define	GPSBIT_GPGST			0x00000200
#define	GPSBIT_GPMSS			0x00000400
#define	GPSBIT_GPZDA			0x00000800

#define GPSID_TO_BIT( id ) ( 1 << (id) )

//$GPGGA Sentence (Fix data)
//Example (signal not acquired): $GPGGA,235947.000,0000.0000,N,00000.0000,E,0,00,0.0,0.0,M,,,,0000*00
//Example (signal acquired): $GPGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,19.7,M,,,,0000*1F
typedef struct _GPGGA
{
	BYTE id[7];   // Sentence ID must="$GPGGA"
	BYTE utcTime[11]; //UTC Time 092204.999 hhmmss.sss 
	BYTE latitude[12];// latitude 4250.5589 ddmm.mmmm //lht2005-3-15
	BYTE nsIndicator[2];//N/S Indicator S N = North, S = South 
	BYTE longitude[12]; // longitude 14718.5084 dddmm.mmmm //lht2005-3-15

	BYTE ewIndicator[2];//E/W Indicator E E = East, W = West 
    BYTE positionFix[2];//Position Fix 1 0 = Invalid, 1 = Valid SPS, 2 = Valid DGPS, 3 = Valid PPS 
	BYTE satellites[3];//Satellites Used 04 Satellites being used (0-12) 
	BYTE hdop[6];//HDOP 24.4 Horizontal dilution of precision //lht2005-3-15
	BYTE altitude[6];//Altitude 19.7 Altitude in meters according to WGS-84 ellipsoid //lht2005-3-17

	BYTE altitudeUnits[2];//Altitude Units M M = Meters 
	BYTE geoidSeperation[10];// Geoid seperation in meters according to WGS-84 ellipsoid 
	BYTE seperationUnits[2];// Seperation Units   M = Meters 
	BYTE dgpsAge[2]; // not support, must = 0 DGPS Age   Age of DGPS data in seconds 
	BYTE dgpsStationID[5];//DGPS Station ID 0000   

//	BYTE checksum[4];//Checksum = *1F   
}GPGGA, FAR * LPGPGGA;
//#define GPGGA_MEMBER_NUM 16

//$GPGLL Sentence (Position)
//Example (signal not acquired): $GPGLL,0000.0000,N,00000.0000,E,235947.000,V*2D
//Example (signal acquired): $GPGLL,4250.5589,S,14718.5084,E,092204.999,A*2D
typedef struct _GPGLL
{
	BYTE id[7]; //Sentence ID, must=$GPGLL 
	BYTE latitude[12];  // 4250.5589 ddmm.mmmm //lht2005-3-15
	BYTE nsIndicator[2];   // N/S Indicator S N = North, S = South 
    BYTE longitude[12]; //Longitude 14718.5084 dddmm.mmmm //lht2005-3-15
	BYTE ewIndicator[2];   //E/W Indicator E E = East, W = West 

	BYTE utcTime[11];   //UTC Time 092204.999 hhmmss.sss  
    BYTE status[2];		//Status A A = Valid, V = Invalid 
	BYTE modeIndicator[2];//lht2005-3-15
//	BYTE checksum[3];		//Checksum *2D   
}GPGLL, FAR * LPGPGLL;
//#define GPGLL_MEMBER_NUM 8

//$GPGSA Sentence (Active satellites)
//Example (signal not acquired): $GPGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0*30
//Example (signal acquired): $GPGSA,A,3,01,20,19,13,,,,,,,,,40.4,24.4,32.2*0A

typedef struct _GPGSA
{
	BYTE id[7];	//Sentence ID must=$GPGSA   
	BYTE mode1[2];//Mode 1 A A = Auto 2D/3D, M = Forced 2D/3D 
	BYTE mode2[2];//Mode 1 3 1 = No fix, 2 = 2D, 3 = 3D 
	BYTE satellite1[3];//Satellite used 1 01 Satellite used on channel 1 
	BYTE satellite2[3];//Satellite used 1 01 Satellite used on channel 2 

	BYTE satellite3[3];//Satellite used 1 01 Satellite used on channel 3 
	BYTE satellite4[3];//Satellite used 1 01 Satellite used on channel 4 
	BYTE satellite5[3];//Satellite used 1 01 Satellite used on channel 5 
	BYTE satellite6[3];//Satellite used 1 01 Satellite used on channel 6 
	BYTE satellite7[3];//Satellite used 1 01 Satellite used on channel 7 

	BYTE satellite8[3];//Satellite used 1 01 Satellite used on channel 8 
	BYTE satellite9[3];//Satellite used 1 01 Satellite used on channel 9 
	BYTE satellite10[3];//Satellite used 1 01 Satellite used on channel 10
	BYTE satellite11[3];//Satellite used 1 01 Satellite used on channel 11
	BYTE satellite12[3];//Satellite used 1 01 Satellite used on channel 12

	BYTE pdop[6];//PDOP 40.4 Position dilution of precision //lht2005-3-15
	BYTE hdop[6];//HDOP 24.4 Horizontal dilution of precision //lht2005-3-15
	BYTE vdop[6];//VDOP 32.2 Vertical dilution of precision //lht2005-3-15
//	BYTE checksum[4];//Checksum must = *0A   
}GPGSA, FAR * LPGPGSA;
//#define GPGSA_MEMBER_NUM 19

//$GPGSV Sentence (Satellites in view)
//Example (signal not acquired): $GPGSV,1,1,01,21,00,000,*4B
//Example (signal acquired): $GPGSV,3,1,10,20,78,331,45,01,59,235,47,22,41,069,,13,32,252,45*70

typedef struct _GPGSV
{
	BYTE id[7]; //Sentence ID must="$GPGSV"
	BYTE numberOfMessages[2];//Number of messages 3 Number of messages in complete message (1-3) 
	BYTE sequenceNumber[2];//Sequence number 1 Sequence number of this entry (1-3) 
	BYTE satellitesInView[3];//Satellites in view 10   

	BYTE satelliteID1[3];//Satellite ID 1 20 Range is 1-32 
	BYTE elevation1[3];//Elevation 1 78 Elevation in degrees (0-90) 
	BYTE azimuth1[4];//Azimuth 1 331 Azimuth in degrees (0-359) 
	BYTE snr1[3];//SNR 1 45 Signal to noise ration in dBHZ (0-99) 

	BYTE satelliteID2[3];//Satellite ID 2 20 Range is 1-32 
	BYTE elevation2[3];//Elevation 2 78 Elevation in degrees (0-90) 
	BYTE azimuth2[4];//Azimuth 2 331 Azimuth in degrees (0-359) 
	BYTE snr2[3];//SNR 2 45 Signal to noise ration in dBHZ (0-99) 

	BYTE satelliteID3[3];//Satellite ID 3 20 Range is 1-32 
	BYTE elevation3[3];//Elevation 3 78 Elevation in degrees (0-90) 
	BYTE azimuth3[4];//Azimuth 3 331 Azimuth in degrees (0-359) 
	BYTE snr3[3];//SNR 3 45 Signal to noise ration in dBHZ (0-99) 

	BYTE satelliteID4[3];//Satellite ID 4 20 Range is 1-32 
	BYTE elevation4[3];//Elevation 4 78 Elevation in degrees (0-90) 
	BYTE azimuth4[4];//Azimuth 4 331 Azimuth in degrees (0-359) 
	BYTE snr4[3];//SNR 4 45 Signal to noise ration in dBHZ (0-99) 

//	BYTE checksum[4]; //Checksum must="*70"
}GPGSV, FAR * LPGPGSV;
//#define GPGSV_MEMBER_NUM 21
//"$GPRMC" Sentence (Position and time)
//Example (signal not acquired): $GPRMC,235947.000,V,0000.0000,N,00000.0000,E,,,041299,,*1D
//Example (signal acquired): $GPRMC,092204.999,A,4250.5589,S,14718.5084,E,0.00,89.68,211200,,*25
typedef struct _GPRMC
{
	BYTE id[7];//  Sentence ID must ="$GPRMC"
	BYTE utcTime[11];   //UTC Time 092204.999 hhmmss.sss  	
	BYTE status[2];//Status A A = Valid, V = Invalid 
	BYTE latitude[12];//Latitude 4250.5589 ddmm.mmmm //lht2005-3-10 [10];
	BYTE nIndicator[2];////N/S Indicator S N = North, S = South 

	BYTE longitude[12];//Longitude 14718.5084 dddmm.mmmm //lht2005-3-10 [11];
	BYTE ewIndicator[2];//E/W Indicator E E = East, W = West 
	BYTE speedOverGround[9];//Speed over ground 0000.00 Knots //lht2005-3-10 [8];
	BYTE courseOverGround[7];//Course over ground 000.00 Degrees 
	BYTE utcDate[7];//UTC Date 211200 DDMMYY 

	BYTE magneticVariation[2];// not support  Magnetic variation   Degrees 
	BYTE magneticVariationEW[2];//lht2005-3-9
	BYTE modeIndicator[2];//lht2005-3-9  
//	BYTE checksum[4];//Checksum must="*25"
}GPRMC, FAR * LPGPRMC;
//#define GPRMC_MEMBER_NUM 12
//$GPVTG Sentence (Course over ground)
//Example (signal not acquired): $GPVTG,,T,,M,,N,,K*4E
//Example (signal acquired): $GPVTG,89.68,T,,M,0.00,N,0.0,K*5F
typedef struct _GPVTG
{
	BYTE id[7];//Sentence ID must="$GPVTG"
	BYTE course1[7];//Course 189.68 Course in degrees 
	BYTE reference1[2];// T = True heading 
	BYTE course2[7];//   Course in degrees 
	BYTE reference2[2];// 89.68 M = Magnetic heading 

	BYTE speed1[8];//Speed 0.00 Horizontal speed 
	BYTE units1[2];//Units N N = Knots 
	BYTE speed2[8];//Speed 0.00 Horizontal speed 
	BYTE units2[2];//Units K K = KM/h 
	BYTE modeIndicator[2];//lht2005-3-15
//	BYTE checksum[4];//Checksum must="*5F"
}GPVTG, FAR * LPGPVTG;
//#define GPVTG_MEMBER_NUM 10



#define	DATERR_SUCCESS		0x0000
#define	DATERR_FRAME		0x0001	//数据帧不合格
#define	DATERR_BYTE			0x0002	//含有非要求的字节
#define	DATERR_CRC			0x0003	//校验错
#define	DATERR_CMD			0x0004	//命令错



//定义GPS API
// 返回 GPS Object
HANDLE WINAPI GPS_Init( void );
BOOL WINAPI GPS_Deinit( HANDLE hGPSObj );

// 返回 GPS Open Object
HANDLE WINAPI GPS_Open( HANDLE hGPSObj );//, DWORD dwReserve, LPVOID lpReserve );
BOOL WINAPI GPS_Close( HANDLE hGPSOpenObj );

//停止处理数据和发通知消息
//BOOL WINAPI GPS_Stop( HANDLE hGPSOpenObj );
//开始处理数据和通知消息
//BOOL WINAPI GPS_Start( HANDLE hGPSOpenObj );

//清除当前的数据
BOOL WINAPI GPS_Resume( HANDLE hGPSOpenObj );

typedef struct _GPSITEM
{   
	UINT gpsId;		// gps data type
	UINT nBufSize;   // lpData buffer's size
	LPVOID lpData;    // get data buffer pointer data struct 
}GPSITEM, FAR * LPGPSITEM;

typedef struct _GPSINFO
{   
	DWORD dwSize;   // = sizeof( GPSINFO )
    DWORD dwItemCount;	// gpsData 有多少项
	GPSITEM gpsItem[1]; //
}GPSINFO, FAR * LPGPSINFO;
//得到最新的GPS信息
BOOL WINAPI GPS_GetInfo( HANDLE hGPSOpenObj, LPGPSINFO lpgpsInfo );
BOOL WINAPI GPS_GetInfoById( HANDLE hGPSOpenObj, UINT uiGPSId, LPVOID lpBuf, UINT nBufSize );

#define GPSN_NOP			0
#define GPSN_DATAIN         1			// // a new id data lParam = 0  
//#define GPSN_ID			  2			// a new id data lParam = 0;

typedef BOOL ( CALLBACK * LPGPSNOTIFY )( HANDLE hGPSOpenObj, UINT uNotifyCode, UINT uiGPSId, LPVOID lParam );

//typedef struct _GPSCONFIG
//{
	//DWORD dwSize;  // = sizeof( GPSCONFIG )
	//DWORD dwCfgFlag; 
	//LPVOID lpNotifyMethod;
//}GPSCONFIG, FAR * LPGPSCONFIG;

//配置GPS,包含：通知方法
BOOL WINAPI GPS_SetNotifyHandler( HANDLE hGPSOpenObj, LPGPSNOTIFY lpgpsNotifyHandler, LPVOID lParam );

//得到GPS数据处理能力
DWORD WINAPI GPS_GetCapabilities( HANDLE hGPSOpenObj );


//功能：完成数据的完整性和校验成功，并提交
BOOL WINAPI GPS_DataIn( HANDLE hGPSOpenObj, LPCBYTE pBuf, DWORD dwLen );


// 以下是基于Kingmos系统的高级API
//返回 hKGPSObj
//HANDLE WINAPI KGPS_InitDevice( UINT uCOM, UINT nBaudRate, DWORD dwParam );
//返回 hKGPSOpenObj

HANDLE WINAPI KGPS_Open( UINT uCOM, UINT nBaudRate, DWORD dwFlag );
BOOL WINAPI KGPS_Close( HANDLE hKGPSOpenObj );

//开始处理数据和通知消息
#define KGPSM_DEVICE      1
#define KGPSM_REPLAY      2

BOOL   WINAPI KGPS_Start( HANDLE hKGPSOpenObj, UINT uStartMode );
//停止并关闭 COM 口
//停止处理数据和发通知消息
BOOL   WINAPI KGPS_Stop( HANDLE hKGPSOpenObj );
//停止
BOOL   WINAPI KGPS_Pause( HANDLE hKGPSOpenObj );

//通知方法
#define KGSM_NOTIFY_HWND			0x00000001
#define KGSM_NOTIFY_EVENT		    0x00000002
#define KGSM_NOTIFY_CALLBACK		0x00000003
#define GSF_NOTIFY_MASK             0x0000000f

//当用窗口时，该标志有效
//#define KGSM_NOTIFY_POST            0x00000010
//#define KGSM_NOTIFY_SEND            0x00000020

typedef struct _NOTIFY_HWND
{
	HWND hWnd;		//向该窗口发通知消息
	UINT uMessage;	//向窗口发送通知消息的ID, like ( hWnd, uMessage, uiGPSId, 0 )
}NOTIFY_HWND, FAR * LPNOTIFY_HWND;

typedef VOID ( CALLBACK * LPKGPSNOTIFY )( HANDLE hKGPSOpenObj, UINT uiGPSId );

BOOL WINAPI KGPS_SetNotifyMethod( HANDLE hKGPSOpenObj, 
								  UINT flagMethod, 
								  LPVOID lpMethod, 
								  DWORD dwNotifyIdMask      // = GPSBIT_XXX的组合
								  );


#define SR_TRUNCATE_EXISTING	0x00000000		//删除已有的文件数据
#define SR_APPEND				0x00000001		//添加数据到文件尾
#define SR_REPLAY               0x00000002		//回放数据

BOOL WINAPI KGPS_SetRecord( HANDLE hKGPSOpenObj, LPCTSTR lpszSaveFileName, UINT flag );
//开始记录数据
BOOL WINAPI KGPS_StartRecord( HANDLE hKGPSOpenObj );
//停止记录数据
BOOL WINAPI KGPS_StopRecord( HANDLE hKGPSOpenObj );
//回放记录数据
BOOL WINAPI KGPS_SetReplayDelay( HANDLE hKGPSOpenObj, UINT msDelay );
//得到最新的GPS数据，返回其gps id, data
BOOL WINAPI KGPS_GetCurrentData( HANDLE hKGPSOpenObj, LPVOID lpgpsData, UINT nBufSize, LPUINT lpgpsID );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__GPSAPI_H



