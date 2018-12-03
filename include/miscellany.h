/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef __MISCELLANY_H
#define __MISCELLANY_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

//���屳��ķ����ʶ
#define CONTRASTCOMMAND			6149

//����Աȶȵ�����Ԥ���� 
#define CONTRAST_CMD_GET		0	
#define CONTRAST_CMD_SET		1	
#define CONTRAST_CMD_INCREASE	2	
#define CONTRAST_CMD_DECREASE	3	
#define CONTRAST_CMD_DEFAULT	4	
#define CONTRAST_CMD_MAX		5	

#define BRIGHTCOMMAND			6150
//���屳�����ȵ�����Ԥ���� 
#define BRIGHT_CMD_GET		0	
#define BRIGHT_CMD_SET		1	
#define BRIGHT_CMD_INCREASE	2	
#define BRIGHT_CMD_DECREASE	3	
#define BRIGHT_CMD_DEFAULT	4	
#define BRIGHT_CMD_MAX		5	

typedef struct LcdCmdInputParm {
	  int command;
	  int parm;
}LcdCmdInput;

// ��Դ����
//

#define AC_LINE_OFFLINE                 0x00
#define AC_LINE_ONLINE                  0x01
#define AC_LINE_BACKUP_POWER            0x02
#define AC_LINE_UNKNOWN                 0xFF

#define BATTERY_FLAG_HIGH               0x01
#define BATTERY_FLAG_LOW                0x02
#define BATTERY_FLAG_CRITICAL           0x04
#define BATTERY_FLAG_CHARGING           0x08
#define BATTERY_FLAG_NO_BATTERY         0x80
#define BATTERY_FLAG_UNKNOWN            0xFF

#define BATTERY_PERCENTAGE_UNKNOWN      0xFF

#define BATTERY_LIFE_UNKNOWN        0xFFFFFFFF

typedef struct _SYSTEM_POWER_STATUS_EX {
	BYTE ACLineStatus;  //AC ��Դ״̬: 0-Offline ; 1-Online ; 255 - δ֪״̬
	BYTE BatteryFlag;	//��س��״̬:1-High;2-Low;4-Critical;8-Charging;128-No system battery;255-Unknown status 
	BYTE BatteryLifePercent;	//��ǰ��ص����İٷֱ�: 0 ~ 100; 255-status is unknown
	BYTE Reserved1;		// =0
	DWORD BatteryLifeTime;	//��ǰ��ص����Ŀ�ʹ������, 0xFFFFFFFF if remaining seconds are unknown. 
	DWORD BatteryFullLifeTime;	//��ǰ��صĵ����������ʹ�õ�������, or 0xFFFFFFFF if full lifetime is unknown. 
	BYTE Reserved2;		//=0
	BYTE BackupBatteryFlag;	//  �󱸵�س��״̬. It is one of the following values: 
							//	BATTERY_FLAG_HIGH
							//	BATTERY_FLAG_CRITICAL
							//	BATTERY_FLAG_CHARGING
							//	BATTERY_FLAG_NO_BATTERY
							//	BATTERY_FLAG_UNKNOWN
							//	BATTERY_FLAG_LOW
	BYTE BackupBatteryLifePercent;	////��ǰ�󱸵�ص����İٷֱ�. Must be in the range 0 to 100, or BATTERY_PERCENTAGE_UNKNOWN.
	BYTE Reserved3;		//=0
	DWORD BackupBatteryLifeTime;	//��ǰ�󱸵�ص����Ŀ�ʹ������, or BATTERY_LIFE_UNKNOWN if remaining seconds are unknown. 
	DWORD BackupBatteryFullLifeTime;	//��ǰ�󱸵�صĵ����������ʹ�õ�������, or BATTERY_LIFE_UNKNOWN if full lifetime is unknown. 
} SYSTEM_POWER_STATUS_EX, *PSYSTEM_POWER_STATUS_EX, *LPSYSTEM_POWER_STATUS_EX;

#define GetSystemPowerStatusEx Dev_GetSystemPowerStatusEx
BOOL WINAPI Dev_GetSystemPowerStatusEx(
				PSYSTEM_POWER_STATUS_EX pstatus, 
				BOOL fUpdate );


#ifdef __cplusplus
}
#endif  /* __cplusplus */

///////////////////////////////////////////////////

#endif   //__MISCELLANY_H

