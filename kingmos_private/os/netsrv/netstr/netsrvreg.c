/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����GPRS Config API
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-08-24
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
//"\inc_local"
#include "netsrvreg.h"


/***************  ȫ���� ���壬 ���� *****************/

// ----------------------------------------------------------
// ����ģʽ  ��ע����/�� �궨��---�� HKEY_SOFTWARE_ROOT ��
// ----------------------------------------------------------

//·��
const	TCHAR	KPATH_RasM				[]=TEXT("Comm\\RasMethod");	//·��
const	TCHAR	KPATH_RasMDef			[]=TEXT("Comm\\RasMethod\\Default");	//ȱʡ����
const	TCHAR	KNAME_RasMDef			[]=TEXT("Default");						//ȱʡ����

//����ģʽ�� ����
//----"comm\\RasMethod\\��λ" AND "comm\\RasMethod\\Default"
const	TCHAR	KITEM_RasM_DIAL			[]=TEXT("Dial");			//
const	TCHAR	KITEM_RasM_LOCAL		[]=TEXT("Local");			//
const	TCHAR	KITEM_RasM_AREA			[]=TEXT("Area");			//
const	TCHAR	KITEM_RasM_COUNTRY		[]=TEXT("Country");			//


// ----------------------------------------------------------
// ������Ŀ  ��ע����/�� �궨��---�� HKEY_SOFTWARE_ROOT ��
// ----------------------------------------------------------

//·��
const	TCHAR	KPATH_RasE				[]=TEXT("Comm\\RasEntries");			//·��
const	TCHAR	KPATH_RasEDef			[]=TEXT("Comm\\RasEntries\\Default");	//ȱʡ����
const	TCHAR	KNAME_RasEDef			[]=TEXT("Default");						//ȱʡ����

//
//----"comm\\RasEntries\\Default"
const	TCHAR	KITEM_RasEDef			[]=TEXT("RasEntry");		//

//
//----"comm\\RasEntries\\���ŵ�96169"
const	TCHAR	KITEM_RasE_ENTRY		[]=TEXT("Entry");			//
//const	TCHAR	KITEM_RasE_PHONE		[]=TEXT("PhoneNumber");	//
const	TCHAR	KITEM_RasE_USER			[]=TEXT("User");			//
const	TCHAR	KITEM_RasE_PW			[]=TEXT("Password");		//
const	TCHAR	KITEM_RasE_DOMAIN		[]=TEXT("Domain");		//
const	TCHAR	KITEM_RasE_DEVCFG		[]=TEXT("DevCfg");		//


// ----------------------------------------------------------
// GPRS����  ��ע����/�� �궨��---�� HKEY_SOFTWARE_ROOT ��
// ----------------------------------------------------------

//·��
const	TCHAR	KPATH_GprsCfg			[]=TEXT("Comm\\GprsCfg");			//·��
const	TCHAR	KPATH_GprsCfg_Internet	[]=TEXT("Comm\\GprsCfg\\Internet");	//·��
const	TCHAR	KPATH_GprsCfg_Wap		[]=TEXT("Comm\\GprsCfg\\Wap");		//·��

//
//----"comm\\GprsCfg"
const	TCHAR	KITEM_GprsCfg_CurType	[]=TEXT("CurNetType");

//
//----"comm\\GprsCfg\\Internet"
const	TCHAR	KITEM_GprsCfg_APN		[]=TEXT("Apn");
const	TCHAR	KITEM_GprsCfg_PPG_IP	[]=TEXT("PPG_IP");
const	TCHAR	KITEM_GprsCfg_PPG_Port	[]=TEXT("PPG_Port");


// ----------------------------------------------------------
// MMS����  ��ע����/�� �궨��---�� HKEY_SOFTWARE_ROOT ��
// ----------------------------------------------------------

//·��
const	TCHAR	KPATH_MmsCfg			[]=TEXT("Comm\\MmsCfg");				//·��

//
//----"Comm\\MmsCfg"
const	TCHAR	KITEM_MmsCfg_SC			[]=TEXT("MMSC");

/*
// ----------------------------------------------------------
// WSP����  ��ע����/�� �궨��---�� HKEY_SOFTWARE_ROOT ��
// ----------------------------------------------------------

//·��
const	TCHAR	KPATH_WspCfg			[]=TEXT("Comm\\WspCfg");				//·��

//
//----"Comm\\WspCfg"
const	TCHAR	KITEM_WspCfg_PpgIP		[]=TEXT("PpgIP");
const	TCHAR	KITEM_WspCfg_PpgPort	[]=TEXT("PpgPort");
*/


/******************************************************/
