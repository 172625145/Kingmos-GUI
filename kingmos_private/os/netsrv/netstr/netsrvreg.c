/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：GPRS Config API
版本号：  1.0.0
开发时期：2004-08-24
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
//"\inc_local"
#include "netsrvreg.h"


/***************  全局区 定义， 声明 *****************/

// ----------------------------------------------------------
// 拨号模式  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
const	TCHAR	KPATH_RasM				[]=TEXT("Comm\\RasMethod");	//路径
const	TCHAR	KPATH_RasMDef			[]=TEXT("Comm\\RasMethod\\Default");	//缺省内容
const	TCHAR	KNAME_RasMDef			[]=TEXT("Default");						//缺省的名

//拨号模式的 内容
//----"comm\\RasMethod\\单位" AND "comm\\RasMethod\\Default"
const	TCHAR	KITEM_RasM_DIAL			[]=TEXT("Dial");			//
const	TCHAR	KITEM_RasM_LOCAL		[]=TEXT("Local");			//
const	TCHAR	KITEM_RasM_AREA			[]=TEXT("Area");			//
const	TCHAR	KITEM_RasM_COUNTRY		[]=TEXT("Country");			//


// ----------------------------------------------------------
// 拨号条目  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
const	TCHAR	KPATH_RasE				[]=TEXT("Comm\\RasEntries");			//路径
const	TCHAR	KPATH_RasEDef			[]=TEXT("Comm\\RasEntries\\Default");	//缺省内容
const	TCHAR	KNAME_RasEDef			[]=TEXT("Default");						//缺省的名

//
//----"comm\\RasEntries\\Default"
const	TCHAR	KITEM_RasEDef			[]=TEXT("RasEntry");		//

//
//----"comm\\RasEntries\\拨号到96169"
const	TCHAR	KITEM_RasE_ENTRY		[]=TEXT("Entry");			//
//const	TCHAR	KITEM_RasE_PHONE		[]=TEXT("PhoneNumber");	//
const	TCHAR	KITEM_RasE_USER			[]=TEXT("User");			//
const	TCHAR	KITEM_RasE_PW			[]=TEXT("Password");		//
const	TCHAR	KITEM_RasE_DOMAIN		[]=TEXT("Domain");		//
const	TCHAR	KITEM_RasE_DEVCFG		[]=TEXT("DevCfg");		//


// ----------------------------------------------------------
// GPRS配置  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
const	TCHAR	KPATH_GprsCfg			[]=TEXT("Comm\\GprsCfg");			//路径
const	TCHAR	KPATH_GprsCfg_Internet	[]=TEXT("Comm\\GprsCfg\\Internet");	//路径
const	TCHAR	KPATH_GprsCfg_Wap		[]=TEXT("Comm\\GprsCfg\\Wap");		//路径

//
//----"comm\\GprsCfg"
const	TCHAR	KITEM_GprsCfg_CurType	[]=TEXT("CurNetType");

//
//----"comm\\GprsCfg\\Internet"
const	TCHAR	KITEM_GprsCfg_APN		[]=TEXT("Apn");
const	TCHAR	KITEM_GprsCfg_PPG_IP	[]=TEXT("PPG_IP");
const	TCHAR	KITEM_GprsCfg_PPG_Port	[]=TEXT("PPG_Port");


// ----------------------------------------------------------
// MMS配置  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
const	TCHAR	KPATH_MmsCfg			[]=TEXT("Comm\\MmsCfg");				//路径

//
//----"Comm\\MmsCfg"
const	TCHAR	KITEM_MmsCfg_SC			[]=TEXT("MMSC");

/*
// ----------------------------------------------------------
// WSP配置  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
const	TCHAR	KPATH_WspCfg			[]=TEXT("Comm\\WspCfg");				//路径

//
//----"Comm\\WspCfg"
const	TCHAR	KITEM_WspCfg_PpgIP		[]=TEXT("PpgIP");
const	TCHAR	KITEM_WspCfg_PpgPort	[]=TEXT("PpgPort");
*/


/******************************************************/
