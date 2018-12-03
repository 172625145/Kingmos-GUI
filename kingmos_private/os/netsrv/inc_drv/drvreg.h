/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _DRV_REG_H_
#define _DRV_REG_H_

#ifdef __cplusplus
extern "C" {
#endif


// ----------------------------------------------------------
// ע����  �ĺ궨��
// ----------------------------------------------------------

#ifndef KEY_DEFAULT_ACCESS
#define KEY_DEFAULT_ACCESS KEY_ALL_ACCESS
#endif

//
//--- HKEY_HARDWARE_ROOT ��
//
#define	KROOT_DRIVES			HKEY_HARDWARE_ROOT				//��·��
#define	KNAME_DRIVES			"Drivers"						//

#define	KROOT_Active			HKEY_HARDWARE_ROOT				//��·��
#define	KPATH_Active			"Drivers\\Active"				//
#define	KNAME_Active			"Active"						//

#define	KPATH_BuiltIn			"Drivers\\BuiltIn"				//
#define	KNAME_BuiltIn			"BuiltIn"						//

#define	KPATH_PCMCIA			"Drivers\\PCMCIA"				//
#define	KNAME_PCMCIA			"PCMCIA"						//
#define	KPATH_USB				"Drivers\\USB"					//
#define	KNAME_USB				"USB"							//


// ----------------------------------------------------------
// ע�����  �ĺ궨��
// ----------------------------------------------------------

//
//----"Drivers\\Active\\12"
//
//1. 
#define	ITEM_Active_NAME		"Name"		//
#define	ITEM_Active_DRVPATH		"DrvPath"	//
//2. 
#define	ITEM_Active_SOCK		"Sock"		//


//
//----"Drivers\\BuiltIn\\Serial2" / "Drivers\\PCMCIA\\NE2000"
//
//1. 
#define	ITEM_Drv_DLL			"Dll"		//
#define	ITEM_Drv_PREX			"Prefix"	//
//2. 
#define	ITEM_Drv_TSP			"Tsp"		//
#define	ITEM_Drv_DEVTYPE		"DevType"	//
#define	ITEM_Drv_DEVICENAME		"DevName"	//
#define	ITEM_Drv_DEVCFG			"DevCfg"	//
#define	ITEM_Drv_MTU			"MTU"		//
//3. 
#define	ITEM_Drvs_NDMPATH		"NdmPath"	//





#ifdef __cplusplus
}	
#endif

#endif	//_DRV_REG_H_
