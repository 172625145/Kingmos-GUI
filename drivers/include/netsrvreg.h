/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _NETSRV_REG_H_
#define _NETSRV_REG_H_

#ifndef _DRV_REG_H_
#include "drvreg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


// ----------------------------------------------------------
// 拨号模式  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
#define	KROOT_RasM		HKEY_SOFTWARE_ROOT			//根路径
extern	const	TCHAR	KPATH_RasM[];
extern	const	TCHAR	KPATH_RasMDef[];
extern	const	TCHAR	KNAME_RasMDef[];

//拨号模式的 内容
//----"comm\\RasMethod\\单位" AND "comm\\RasMethod\\Default"
extern	const	TCHAR	KITEM_RasM_DIAL[];
extern	const	TCHAR	KITEM_RasM_LOCAL[];
extern	const	TCHAR	KITEM_RasM_AREA[];
extern	const	TCHAR	KITEM_RasM_COUNTRY[];


// ----------------------------------------------------------
// 拨号条目  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
#define	KROOT_RasE		HKEY_SOFTWARE_ROOT			//根路径
extern	const	TCHAR	KPATH_RasE[];
extern	const	TCHAR	KPATH_RasEDef[];
extern	const	TCHAR	KNAME_RasEDef[];

//
//----"comm\\RasEntries\\Default"
extern	const	TCHAR	KITEM_RasEDef[];

//
//----"comm\\RasEntries\\拨号到96169"
extern	const	TCHAR	KITEM_RasE_ENTRY[];
//extern	const	TCHAR	KITEM_RasE_PHONE[];
extern	const	TCHAR	KITEM_RasE_USER[];
extern	const	TCHAR	KITEM_RasE_PW[];
extern	const	TCHAR	KITEM_RasE_DOMAIN[];
extern	const	TCHAR	KITEM_RasE_DEVCFG[];

// ----------------------------------------------------------
// GPRS配置  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
#define	KROOT_GprsCfg	HKEY_SOFTWARE_ROOT			//根路径
extern	const	TCHAR	KPATH_GprsCfg[];
extern	const	TCHAR	KPATH_GprsCfg_Internet[];
extern	const	TCHAR	KPATH_GprsCfg_Wap[];

//
//----"comm\\GprsCfg"
extern	const	TCHAR	KITEM_GprsCfg_CurType[];

//
//----"comm\\GprsCfg\\Internet"
extern	const	TCHAR	KITEM_GprsCfg_APN[];
extern	const	TCHAR	KITEM_GprsCfg_PPG_IP[];
extern	const	TCHAR	KITEM_GprsCfg_PPG_Port[];


// ----------------------------------------------------------
// MMS配置  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
#define	KROOT_MmsCfg	HKEY_SOFTWARE_ROOT			//根路径
extern	const	TCHAR	KPATH_MmsCfg[];

//
//----"Comm\\MmsCfg"
extern	const	TCHAR	KITEM_MmsCfg_SC[];

/*
// ----------------------------------------------------------
// WSP配置  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
#define	KROOT_WspCfg	HKEY_SOFTWARE_ROOT			//根路径
extern	const	TCHAR	KPATH_WspCfg[];

//
//----"Comm\\WspCfg"
extern	const	TCHAR	KITEM_WspCfg_PpgIP[];
extern	const	TCHAR	KITEM_WspCfg_PpgPort[];
*/




// ----------------------------------------------------------
// 举例
// ----------------------------------------------------------
#if 0

;xyg-2004_05_11
[HKEY_HARDWARE_ROOT\Drivers\Active\96169]
   "Name"="COM1:"
   "DrvPath"="Drivers\BuiltIn\Serial2"

[HKEY_HARDWARE_ROOT\Drivers\BuiltIn\Serial2]
   "Order"=dword:1

   "Dll"="serial.Dll"
   "IoBase"=dword:AC700000
   "Irq"=dword:0
   "IoLen"=dword:24
   "DeviceArrayIndex"=dword:4
   "Prefix"="COM"
   "Index"=dword:2       

   "Tsp"="Unimodem.dll"
   ;"DevType"=dword:0x0200
   "DevType"=dword:0x0201
   "DevName"="this is test com1 by xyg"

   "MTU"=dword:1500

[HKEY_SOFTWARE_ROOT\Comm]

;[HKEY_SOFTWARE_ROOT\Comm\RasMethod]
[HKEY_SOFTWARE_ROOT\Comm\RasMethod\Default]
   "Dial" = "T"
   "Local"= "G"
   "Area"= "FG"
   "Country"= "EFG"
[HKEY_SOFTWARE_ROOT\Comm\RasMethod\单位]
   "Dial" = "T"
   "Local"= "9,G"
   "Area"= "9,FG"
   "Country"= "9,EFG"
[HKEY_SOFTWARE_ROOT\Comm\RasMethod\家庭]
   "Dial" = "P"
   "Local"= "G"
   "Area"= "FG"
   "Country"= "EFG"


;[HKEY_SOFTWARE_ROOT\Comm\RasEntries]
[HKEY_SOFTWARE_ROOT\Comm\RasEntries\Default]
   "RasEntry"="KingSoft_Co_xyg_Dial_Default"

[HKEY_SOFTWARE_ROOT\Comm\RasEntries\KingSoft_Co_xyg_Dial_Default]
   "PhoneNumber" = "96169"
   "User" = "96169"
   "Password" = hex: 0
   "Domain" = ""
   "Entry"=hex: \
 00 ,00 ,14 ,02 ,00 ,00 ,00 ,00 ,01 ,00 ,00 ,00 ,00 ,02 ,00 ,00 ,44 ,72 ,69 \
,76 ,65 ,72 ,73 ,5C ,42 ,75 ,69 ,6C ,74 ,49 ,6E ,5C ,53 ,65 ,72 ,69 ,61 ,6C \
,32 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,03 ,00 ,00 ,00 ,39 ,36 ,31 ,36 ,39 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00

   "DevCfg"=hex: 00 ,00 ,A0 ,00 ,1C ,00 ,00 ,00 ,80 ,25 ,00 ,00 ,11 ,10 ,00 ,00 ,00 ,00 ,00 ,08 ,00 \
,02 ,08 ,00 ,01 ,11 ,13 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 \
,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00 ,00


[HKEY_SOFTWARE_ROOT\Comm\GprsCfg]
   "CurNetType"=dword:1
[HKEY_SOFTWARE_ROOT\Comm\GprsCfg\Internet]
   "Apn"="cmnet"
[HKEY_SOFTWARE_ROOT\Comm\GprsCfg\Wap]
   "Apn"="cmwap"


[HKEY_SOFTWARE_ROOT\Comm\MmsCfg]
   "MMSC"="http://mmsc.monternet.com"


[HKEY_SOFTWARE_ROOT\Comm\WspCfg]
   "PpgIP"=dword:1
   "PpgPort"=dword:1



#endif






#ifdef __cplusplus
}	
#endif

#endif	//_NETSRV_REG_H_
