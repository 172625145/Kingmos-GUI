/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef __MISCELLANY_H
#define __MISCELLANY_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

//定义背光的分类标识
#define CONTRASTCOMMAND			6149

//定义对比度调整的预定义 
#define CONTRAST_CMD_GET		0	
#define CONTRAST_CMD_SET		1	
#define CONTRAST_CMD_INCREASE	2	
#define CONTRAST_CMD_DECREASE	3	
#define CONTRAST_CMD_DEFAULT	4	
#define CONTRAST_CMD_MAX		5	

#define BRIGHTCOMMAND			6150
//定义背光亮度调整的预定义 
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

// 电源管理
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
	BYTE ACLineStatus;  //AC 电源状态: 0-Offline ; 1-Online ; 255 - 未知状态
	BYTE BatteryFlag;	//电池充电状态:1-High;2-Low;4-Critical;8-Charging;128-No system battery;255-Unknown status 
	BYTE BatteryLifePercent;	//当前电池电量的百分比: 0 ~ 100; 255-status is unknown
	BYTE Reserved1;		// =0
	DWORD BatteryLifeTime;	//当前电池电量的可使用秒数, 0xFFFFFFFF if remaining seconds are unknown. 
	DWORD BatteryFullLifeTime;	//当前电池的电量充满后可使用的总秒数, or 0xFFFFFFFF if full lifetime is unknown. 
	BYTE Reserved2;		//=0
	BYTE BackupBatteryFlag;	//  后备电池充电状态. It is one of the following values: 
							//	BATTERY_FLAG_HIGH
							//	BATTERY_FLAG_CRITICAL
							//	BATTERY_FLAG_CHARGING
							//	BATTERY_FLAG_NO_BATTERY
							//	BATTERY_FLAG_UNKNOWN
							//	BATTERY_FLAG_LOW
	BYTE BackupBatteryLifePercent;	////当前后备电池电量的百分比. Must be in the range 0 to 100, or BATTERY_PERCENTAGE_UNKNOWN.
	BYTE Reserved3;		//=0
	DWORD BackupBatteryLifeTime;	//当前后备电池电量的可使用秒数, or BATTERY_LIFE_UNKNOWN if remaining seconds are unknown. 
	DWORD BackupBatteryFullLifeTime;	//当前后备电池的电量充满后可使用的总秒数, or BATTERY_LIFE_UNKNOWN if full lifetime is unknown. 
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

