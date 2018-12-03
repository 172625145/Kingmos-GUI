/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _MMS_PDU_H_
#define _MMS_PDU_H_

#ifndef _WSP_VAR_TYPE_H_
#include "wsp_vartype.h"
#endif

#ifndef _WSP_CHAR_SET_H_
#include "wsp_charset.h"
#endif

#ifndef _WSP_LANGUAGE_H_
#include "wsp_lang.h"
#endif

#ifndef _WSP_CONTENT_TYPE_H_
#include "wsp_contenttype.h"
#endif

#ifndef _MMS_ENC_CLT_H_
#include "mms_enc_clt.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ***********************************************************
// Field name / X_Mms_Attributes 定义
// ***********************************************************

#define	TAG_Bcc								(0x01)
#define	TAG_Cc								(0x02)
#define	TAG_X_Mms_Content_Location			(0x03)
#define	TAG_Content_Type					(0x04)
#define	TAG_Date							(0x05)
#define	TAG_X_Mms_Delivery_Report			(0x06)
#define	TAG_X_Mms_Delivery_Time				(0x07)
#define	TAG_X_Mms_Expiry					(0x08)
#define	TAG_From							(0x09)
#define	TAG_X_Mms_Message_Class				(0x0A)
#define	TAG_Message_ID						(0x0B)
#define	TAG_X_Mms_Message_Type				(0x0C)
#define	TAG_X_Mms_MMS_Version				(0x0D)
#define	TAG_X_Mms_Message_Size				(0x0E)
#define	TAG_X_Mms_Priority					(0x0F)
#define	TAG_X_Mms_Read_Report				(0x10)
#define	TAG_X_Mms_Report_Allowed			(0x11)
#define	TAG_X_Mms_Response_Status			(0x12)
#define	TAG_X_Mms_Response_Text				(0x13)
#define	TAG_X_Mms_Sender_Visibility			(0x14)
#define	TAG_X_Mms_Status					(0x15)
#define	TAG_Subject							(0x16)
#define	TAG_To								(0x17)
#define	TAG_X_Mms_Transaction_Id			(0x18)
#define	TAG_X_Mms_Retrieve_Status			(0x19)
#define	TAG_X_Mms_Retrieve_Text				(0x1A)
#define	TAG_X_Mms_Read_Status				(0x1B)
#define	TAG_X_Mms_Reply_Charging			(0x1C)
#define	TAG_X_Mms_Reply_Charging_Deadline	(0x1D)
#define	TAG_X_Mms_Reply_Charging_ID			(0x1E)
#define	TAG_X_Mms_Reply_Charging_Size		(0x1F)
#define	TAG_X_Mms_Previously_Sent_By		(0x20)
#define	TAG_X_Mms_Previously_Sent_Date		(0x21)
//for Ver1.2
#define	TAG_X_Mms_Store						(0x22)
#define	TAG_X_Mms_MM_State					(0x23)
#define	TAG_X_Mms_MM_Flags					(0x24)
#define	TAG_X_Mms_Store_Status				(0x25)
#define	TAG_X_Mms_Store_Status_Text			(0x26)
#define	TAG_X_Mms_Stored					(0x27)
#define	TAG_X_Mms_Attributes				(0x28)
#define	TAG_X_Mms_Totals					(0x29)
#define	TAG_X_Mms_Mbox_Totals				(0x2A)
#define	TAG_X_Mms_Quotas					(0x2B)
#define	TAG_X_Mms_Mbox_Quotas				(0x2C)
#define	TAG_X_Mms_Message_Count				(0x2D)
#define	TAG_Content							(0x2E)
#define	TAG_X_Mms_Start						(0x2F)
#define	TAG_Additional_headers				(0x30)
#define	TAG_X_Mms_Distribution_Indicator	(0x31)
#define	TAG_X_Mms_Element_Descriptor		(0x32)
#define	TAG_X_Mms_Limit						(0x33)


// ********************************
// X_Mms_Store value 定义
// ********************************
#define	MStore_sent_Yes						(0x80 -OCTET_SHORTINT)
#define	MStore_sent_No						(0x81 -OCTET_SHORTINT)


// ********************************
// TAG_X_Mms_Delivery_Report 定义
// ********************************
#define	MDelivery_Report_Yes				(0x80 -OCTET_SHORTINT)
#define	MDelivery_Report_No					(0x81 -OCTET_SHORTINT)


// ********************************
// TAG_X_Mms_Message_Class 定义
// ********************************
#define	MMessage_Class_Personal				(0x80 -OCTET_SHORTINT)
#define	MMessage_Class_Advertisement		(0x81 -OCTET_SHORTINT)
#define	MMessage_Class_Informational		(0x82 -OCTET_SHORTINT)
#define	MMessage_Class_Auto					(0x83 -OCTET_SHORTINT)


// ********************************
// TAG_X_Mms_Read_Report 定义
// ********************************
#define	MRead_Report_Yes					(0x80 -OCTET_SHORTINT)
#define	MRead_Report_No						(0x81 -OCTET_SHORTINT)


// ********************************
// TAG_X_Mms_Priority 定义
// ********************************
#define	MPriority_Low						(0x80 -OCTET_SHORTINT)
#define	MPriority_Normal					(0x81 -OCTET_SHORTINT)
#define	MPriority_High						(0x82 -OCTET_SHORTINT)

// ********************************
// X_Mms_MM_State value 定义
// ********************************
#define	MState_Expired						(0x80 -OCTET_SHORTINT)
#define	MState_Retrieved					(0x81 -OCTET_SHORTINT)
#define	MState_Rejected						(0x82 -OCTET_SHORTINT)
#define	MState_Deferred						(0x83 -OCTET_SHORTINT)
#define	MState_Unrecognised					(0x84 -OCTET_SHORTINT)
#define	MState_Indeterminate				(0x85 -OCTET_SHORTINT)
#define	MState_Forwarded					(0x86 -OCTET_SHORTINT)


// ********************************
// X_Mms_MM_Flags value 定义
// ********************************
#define	MFlag_Add							(0x80 -OCTET_SHORTINT)
#define	MFlag_Remove						(0x81 -OCTET_SHORTINT)
#define	MFlag_Filter						(0x82 -OCTET_SHORTINT)


// ********************************
// X_Mms_Reply_Charging value 定义
// ********************************
#define	MCharging_Requested					(0x80 -OCTET_SHORTINT)
#define	MCharging_Requested_text_only		(0x81 -OCTET_SHORTINT)
#define	MCharging_Accepted					(0x82 -OCTET_SHORTINT)
#define	MCharging_Accepted_text_only		(0X83 -OCTET_SHORTINT)



// ********************************
// X_Mms_Store_Status value 定义
// ********************************
#define	MStore_Status_Success							(0x80 -OCTET_SHORTINT)

#define	MStore_Status_transient_failure					(0xC0 -OCTET_SHORTINT)//192
#define	MStore_Status_transient_network_problem			(0xC1 -OCTET_SHORTINT)//193

#define	MStore_Status_permanent_failure					(0xE0 -OCTET_SHORTINT)//224
#define	MStore_Status_permanent_service_denied			(0xE1 -OCTET_SHORTINT)//225
#define	MStore_Status_permanent_message_format_corrupt	(0xE2 -OCTET_SHORTINT)//226
#define	MStore_Status_permanent_message_not_found		(0xE3 -OCTET_SHORTINT)//227
#define	MStore_Status_permanent_mmbox_full				(0xE4 -OCTET_SHORTINT)//228



// ********************************
// X_Mms_Distribution_Indicator value 定义
// ********************************
#define	MDistribution_Indicator_Yes			(0x80 -OCTET_SHORTINT)
#define	MDistribution_Indicator_No			(0x81 -OCTET_SHORTINT)

// ********************************
// X_Mms_Report_Allowed value 定义
// ********************************
#define	MReport_Allowed_Yes					(0x80 -OCTET_SHORTINT)
#define	MReport_Allowed_No					(0x81 -OCTET_SHORTINT)


// ********************************
// X_Mms_Retrieve_Status value 定义
// ********************************
#define	MRetrieve_Status_Ok								(0x80 -OCTET_SHORTINT)

#define	MRetrieve_Status_transient_failure				(0xC0 -OCTET_SHORTINT)//192
#define	MRetrieve_Status_transient_message_not_found	(0xC1 -OCTET_SHORTINT)//193
#define	MRetrieve_Status_transient_network_problem		(0xC2 -OCTET_SHORTINT)//194

#define	MRetrieve_Status_permanent_failure				(0xE0 -OCTET_SHORTINT)//224
#define	MRetrieve_Status_permanent_service_denied		(0xE1 -OCTET_SHORTINT)//225
#define	MRetrieve_Status_permanent_message_not_found	(0xE2 -OCTET_SHORTINT)//226
#define	MRetrieve_Status_permanent_content_unsupported	(0xE3 -OCTET_SHORTINT)//227


// ********************************
// X_Mms_Read_Status value 定义
// ********************************
#define	MRead_Status_Read					(0x80 -OCTET_SHORTINT)
#define	MRead_Status_Deleted				(0x81 -OCTET_SHORTINT)

// ********************************
// X_Mms_Quotas value 定义
// ********************************
#define	MQuotas_Yes							(0x80 -OCTET_SHORTINT)
#define	MQuotas_No							(0x81 -OCTET_SHORTINT)

// ********************************
// X_Mms_Totals value 定义
// ********************************
#define	Mbox_totals_req_Yes					(0x80 -OCTET_SHORTINT)
#define	Mbox_totals_req_No					(0x81 -OCTET_SHORTINT)


// ***********************************************************
// 通用 类型 的PDU定义
// ***********************************************************

//通用MASK
#define	MMS_GEN_MASK_MTID					0x80000000
#define	MMS_GEN_MASK_MVer					0x40000000
#define	MMS_GEN_MASK_MID					0x20000000
#define	MMS_GEN_MASK_MClass					0x10000000

#define	MMS_GEN_MASK_CntLocation			0x08000000
#define	MMS_GEN_MASK_CntType				0x04000000
#define	MMS_GEN_MASK_Date					0x02000000
#define	MMS_GEN_MASK_From					0x01000000
#define	MMS_GEN_MASK_To						0x00800000
#define	MMS_GEN_MASK_Cc						0x00400000
#define	MMS_GEN_MASK_Subject				0x00200000
#define	MMS_GEN_MASK_Body					0x00100000

#define	MMS_GEN_MASK_Priority				0x00080000
#define	MMS_GEN_MASK_Expiry					0x00040000
#define	MMS_GEN_MASK_Delivery_Report		0x00020000
#define	MMS_GEN_MASK_Read_Report			0x00010000

#define	MMS_GEN_MASK_MM_State				0x00008000
#define	MMS_GEN_MASK_MM_Flags				0x00004000
#define	MMS_GEN_MASK_Store_Status			0x00002000
#define	MMS_GEN_MASK_Response_Status		0x00001000


//通用HEADER
typedef	struct	_MMS_GEN_HDR
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer

} MMS_GEN_HDR;


//通用BODY
//typedef	struct	_MMS_GEN_BODY
//{
//	LPBYTE			pBody;
//	DWORD			dwLenBody;
//
//} MMS_GEN_BODY;


// ***********************************************************
// M_Send_req 和 M_Send_conf 类型 的PDU定义
// ***********************************************************

typedef	struct	_MSEND_REQ_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	DWORD				dwDate;						//O //Long_integer: In seconds from 1970_01_01, 00:00:00 GMT.
	SHORT_A_ENCSTR		stFrom;						//M //Value_length (Address_present_token Encoded_string_value | Insert_address_token)
	ENCODED_STR			stTo;						//O1 //Encoded_string_value
	ENCODED_STR			stCc;						//O1 //Encoded_string_value
	ENCODED_STR			stBcc;						//O1 //Encoded_string_value
	ENCODED_STR			stSubject;					//O //Encoded_string_value

	SHORT_STR			stX_Mms_Message_Class;		//O //Class_identifier | Token_text
	DATE_A_DELTASEC		stX_Mms_Expiry;				//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	DATE_A_DELTASEC		stX_Mms_Delivery_Time;		//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	SHORTINT			bX_Mms_Priority;			//O //Short_integer
	SHORTINT			bX_Mms_Sender_Visibility;	//O //Short_integer
	SHORTINT			bX_Mms_Delivery_Report;		//O //Short_integer
	SHORTINT			bX_Mms_Read_Report;			//O //Short_integer

	//for ver1.2___begin
	SHORTINT			bX_Mms_Store;				//O //Short_integer
	SHORTINT			bX_Mms_MM_State;			//O //Short_integer
	SHORT_A_ENCSTR		stX_Mms_MM_Flags;			//O //Value_length ( Add_token | Remove_token | Filter_token ) Encoded_string_value
	//for ver1.2___end

	SHORTINT			bX_Mms_Reply_Charging;		//O //Short_integer
	DATE_A_DELTASEC		stX_Mms_Reply_Charging_Deadline;//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	DWORD				dwX_Mms_Reply_Charging_Size;//O //Long_integer
	LPSTR				pszX_Mms_Reply_Charging_ID;	//O //Text_string

	CONTENT_TYPE		stContent_Type;				//M //Content_type_value

	//MMS body data
	GEN_BODY			stBodyGen;

} MSEND_REQ_PDU;

//dwMask of MSEND_REQ_PDU
#define	SND_REQ_MASK_Bcc						0x00000001
#define	SND_REQ_MASK_Delivery_Time				0x00000002
#define	SND_REQ_MASK_Sender_Visibility			0x00000004
#define	SND_REQ_MASK_Store						0x00000008
#define	SND_REQ_MASK_Reply_Charging				0x00000010
#define	SND_REQ_MASK_Reply_Charging_Deadline	0x00000020
#define	SND_REQ_MASK_Reply_Charging_Size		0x00000040
#define	SND_REQ_MASK_Reply_Charging_ID			0x00000080


typedef	struct	_MSEND_CONF_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	SHORTINT			bX_Mms_Response_Status;		//M //Short_integer
	ENCODED_STR			stX_Mms_Response_Text;		//O //Encoded_string_value
	LPSTR				pszMessage_ID;				//O //Text_string

	//for ver1.2___begin
	LPSTR				pszX_Mms_Content_Location;	//O //Uri_value
	SHORTINT			bX_Mms_Store_Status;		//O //Short_integer
	ENCODED_STR			stX_Mms_Store_Status_Text;	//O //Encoded_string_value
	//for ver1.2___end

} MSEND_CONF_PDU;

//dwMask of MSEND_CONF_PDU
#define	SND_CONF_MASK_Response_Text				0x00000001
#define	SND_CONF_MASK_Store_Status_Text			0x00000002


// ***********************************************************
// M_Notification_ind 和 M_NotifyResp_ind 类型 的PDU定义
// ***********************************************************


typedef	struct	_MNOTIFICATION_IND_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	SHORT_A_ENCSTR		stFrom;						//O //Value_length (Address_present_token Encoded_string_value | Insert_address_token)
	ENCODED_STR			stSubject;					//O //Encoded_string_value
	SHORTINT			bX_Mms_Delivery_Report;		//O //Short_integer

	//for ver1.2___begin
	SHORTINT			bX_Mms_Stored;				//O //Short_integer
	//for ver1.2___end

	SHORT_STR			stX_Mms_Message_Class;		//M //Class_identifier | Token_text

	//for ver1.2___begin
	SHORTINT			bX_Mms_Priority;			//O //Short_integer
	//for ver1.2___end

	DWORD				dwX_Mms_Message_Size;		//M //Long_integer
	DATE_A_DELTASEC		stX_Mms_Expiry;				//M //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)

	SHORTINT			bX_Mms_Reply_Charging;		//O //Short_integer
	DATE_A_DELTASEC		stX_Mms_Reply_Charging_Deadline;//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	DWORD				dwX_Mms_Reply_Charging_Size;//O //Long_integer
	LPSTR				pszX_Mms_Reply_Charging_ID;	//O //Text_string

	//for ver1.2___begin
	SHORTINT			bX_Mms_Distribution_Indicator;//O //Short_integer
	UNTYPE				X_Mms_Element_Descriptor;	//O //Value_length Content_Reference_value *(Parameter)
	//for ver1.2___end

	LPSTR				pszX_Mms_Content_Location;	//M //Uri_value

} MNOTIFICATION_IND_PDU;

//dwMask of MNOTIFICATION_IND_PDU
#define	NTY_IND_MASK_Mms_Stored					0x00000001
#define	NTY_IND_MASK_Message_Size				0x00000002
#define	NTY_IND_MASK_Distribution_Indicator		0x00000004
#define	NTY_IND_MASK_Element_Descriptor			0x00000008
#define	NTY_IND_MASK_Reply_Charging				0x00000010
#define	NTY_IND_MASK_Reply_Charging_Deadline	0x00000020
#define	NTY_IND_MASK_Reply_Charging_Size		0x00000040
#define	NTY_IND_MASK_Reply_Charging_ID			0x00000080


typedef	struct	_MNOTIFYRESP_IND_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	SHORTINT			bX_Mms_MM_State;			//M //Short_integer
	SHORTINT			bX_Mms_Report_Allowed;		//O //Short_integer

} MNOTIFYRESP_IND_PDU;

//dwMask of MNOTIFYRESP_IND_PDU
#define	NRS_IND_MASK_Report_Allowed				0x00000001


// ***********************************************************
// M_Retrieve_conf 和 M_Acknowledge_ind 类型 的PDU定义
// ***********************************************************

typedef	struct	_MRETRIEVE_CONF_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//O //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszMessage_ID;				//C //Text_string
	DWORD				dwDate;						//M //Long_integer: In seconds from 1970_01_01, 00:00:00 GMT.
	SHORT_A_ENCSTR		stFrom;						//O //Value_length (Address_present_token Encoded_string_value | Insert_address_token)
	INTEGER_A_ENCSTR	stX_Mms_Previously_Sent_By;	//O //Value_length Forwarded_count_value Encoded_string_value
	INTEGER_A_LONG		stX_Mms_Previously_Sent_Date;//O //Value_length Forwarded_count_value Date_value
	ENCODED_STR			stTo;						//O //Encoded_string_value
	ENCODED_STR			stCc;						//O //Encoded_string_value
	ENCODED_STR			stSubject;					//O //Encoded_string_value

	//for ver1.2___begin
	SHORTINT			bX_Mms_MM_State;			//O //Short_integer
	SHORT_A_ENCSTR		stX_Mms_MM_Flags;			//O //Value_length ( Add_token | Remove_token | Filter_token ) Encoded_string_value
	//for ver1.2___end

	SHORT_STR			stX_Mms_Message_Class;		//O //Class_identifier | Token_text
	SHORTINT			bX_Mms_Priority;			//O //Short_integer
	SHORTINT			bX_Mms_Delivery_Report;		//O //Short_integer
	SHORTINT			bX_Mms_Read_Report;			//O //Short_integer

	SHORTINT			bX_Mms_Reply_Charging;		//O //Short_integer
	DATE_A_DELTASEC		stX_Mms_Reply_Charging_Deadline;//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	DWORD				dwX_Mms_Reply_Charging_Size;//O //Long_integer
	LPSTR				pszX_Mms_Reply_Charging_ID;	//O //Text_string

	SHORTINT			bX_Mms_Retrieve_Status;		//O //Short_integer
	ENCODED_STR			stX_Mms_Retrieve_Text;		//O //Encoded_string_value

	//for ver1.2___begin
	SHORTINT			bX_Mms_Distribution_Indicator;//O //Short_integer
	//for ver1.2___end

	CONTENT_TYPE		stContent_Type;				//M //Content_type_value

	//MMS body data
	GEN_BODY			stBodyGen;

} MRETRIEVE_CONF_PDU;

//dwMask of MRETRIEVE_CONF_PDU
#define	RCV_CONF_MASK_Previously_Sent_By		0x00000001
#define	RCV_CONF_MASK_Previously_Sent_Date		0x00000002
#define	RCV_CONF_MASK_Retrieve_Status			0x00000004
#define	RCV_CONF_MASK_Retrieve_Text				0x00000008
#define	RCV_CONF_MASK_Reply_Charging			0x00000010
#define	RCV_CONF_MASK_Reply_Charging_Deadline	0x00000020
#define	RCV_CONF_MASK_Reply_Charging_Size		0x00000040
#define	RCV_CONF_MASK_Reply_Charging_ID			0x00000080
#define	RCV_CONF_MASK_Distribution_Indicator	0x00000100


typedef	struct	_MACKNOWLEDGE_IND_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	SHORTINT			bX_Mms_Report_Allowed;		//O //Short_integer

} MACKNOWLEDGE_IND_PDU;

//dwMask of MACKNOWLEDGE_IND_PDU
#define	ACK_IND_MASK_Report_Allowed				0x00000001


// ***********************************************************
// M_Forward_req 和 M_Forward_conf 类型 的PDU定义
// ***********************************************************

typedef	struct	_MFORWARD_REQ_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	DWORD				dwDate;						//O //Long_integer: In seconds from 1970_01_01, 00:00:00 GMT.
	SHORT_A_ENCSTR		stFrom;						//M //Value_length (Address_present_token Encoded_string_value | Insert_address_token)
	ENCODED_STR			stTo;						//O1 //Encoded_string_value
	ENCODED_STR			stCc;						//O1 //Encoded_string_value
	ENCODED_STR			stBcc;						//O1 //Encoded_string_value

	DATE_A_DELTASEC		stX_Mms_Expiry;				//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	DATE_A_DELTASEC		stX_Mms_Delivery_Time;		//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)

	SHORTINT			bX_Mms_Report_Allowed;		//O //Short_integer
	SHORTINT			bX_Mms_Delivery_Report;		//O //Short_integer
	SHORTINT			bX_Mms_Read_Report;			//O //Short_integer

	//for ver1.2___begin
	SHORTINT			bX_Mms_Store;				//O //Short_integer
	SHORTINT			bX_Mms_MM_State;			//O //Short_integer
	SHORT_A_ENCSTR		stX_Mms_MM_Flags;			//O //Value_length ( Add_token | Remove_token | Filter_token ) Encoded_string_value
	//for ver1.2___end

	LPSTR				pszX_Mms_Content_Location;	//M //Uri_value

} MFORWARD_REQ_PDU;



typedef	struct	_MFORWARD_CONF_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	SHORTINT			bX_Mms_Response_Status;		//M //Short_integer
	ENCODED_STR			stzX_Mms_Response_Text;		//O //Encoded_string_value
	LPSTR				pszMessage_ID;				//O //Text_string

	//for ver1.2___begin
	LPSTR				pszX_Mms_Content_Location;	//O //Uri_value
	SHORTINT			bX_Mms_Store_Status;		//O //Short_integer
	ENCODED_STR			stX_Mms_Store_Status_Text;	//O //Encoded_string_value
	//for ver1.2___end

} MFORWARD_CONF_PDU;


// ***********************************************************
// M_Delivery_ind 类型 的PDU定义
// ***********************************************************


typedef	struct	_MDELIVERY_IND_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszMessage_ID;				//M //Text_string

	ENCODED_STR			stTo;						//M //Encoded_string_value
	DWORD				dwDate;						//M //Long_integer: In seconds from 1970_01_01, 00:00:00 GMT.

	SHORTINT			bX_Mms_MM_State;			//M //Short_integer

} MDELIVERY_IND_PDU;


// ***********************************************************
// M_Read_Rec_ind 和 M_Read_Orig_ind 类型 的PDU定义
// ***********************************************************


typedef	struct	_MREAD_REC_IND_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszMessage_ID;				//M //Text_string

	ENCODED_STR			stTo;						//M //Encoded_string_value
	SHORT_A_ENCSTR		stFrom;						//M //Value_length (Address_present_token Encoded_string_value | Insert_address_token)
	DWORD				dwDate;						//O //Long_integer: In seconds from 1970_01_01, 00:00:00 GMT.

	SHORTINT			bX_Mms_Read_Status;			//M //Short_integer

} MREAD_REC_IND_PDU;


typedef	struct	_MREAD_ORIG_IND_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszMessage_ID;				//M //Text_string

	ENCODED_STR			stTo;						//M //Encoded_string_value
	SHORT_A_ENCSTR		stFrom;						//M //Value_length (Address_present_token Encoded_string_value | Insert_address_token)
	DWORD				dwDate;						//M //Long_integer: In seconds from 1970_01_01, 00:00:00 GMT.

	SHORTINT			bX_Mms_Read_Status;			//M //Short_integer

} MREAD_ORIG_IND_PDU;

// ***********************************************************
// for ver1.2___begin__PDU_TYPE
// ***********************************************************

// ***********************************************************
// M_Mbox_Store_req 和 M_Mbox_Store_conf 类型 的PDU定义
// ***********************************************************


typedef	struct	_MBOX_STORE_REQ_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszX_Mms_Content_Location;	//M //Uri_value
	SHORTINT			bX_Mms_MM_State;			//O //Short_integer
	SHORT_A_ENCSTR		stX_Mms_MM_Flags;			//O //Value_length ( Add_token | Remove_token | Filter_token ) Encoded_string_value

} MBOX_STORE_REQ_PDU;


typedef	struct	_MBOX_STORE_CONF_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszX_Mms_Content_Location;	//O //Uri_value
	SHORTINT			bX_Mms_Store_Status;		//M //Short_integer
	ENCODED_STR			stX_Mms_Store_Status_Text;	//O //Encoded_string_value

} MBOX_STORE_CONF_PDU;

// ***********************************************************
// M_Mbox_View_req 和 M_Mbox_View_conf 类型 的PDU定义
// ***********************************************************


typedef	struct	_MBOX_VIEW_REQ_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszX_Mms_Content_Location;	//O //Uri_value
	SHORTINT			bX_Mms_MM_State;			//O //Short_integer
	SHORT_A_ENCSTR		stX_Mms_MM_Flags;			//O //Value_length ( Add_token | Remove_token | Filter_token ) Encoded_string_value

	DWORD				dwX_Mms_Start;				//O //Integer_value
	DWORD				dwX_Mms_Limit;				//O //Integer_value
	SHORTINT			bX_Mms_Attributes;			//O //Short_integer
	SHORTINT			bX_Mms_Totals;				//O //Short_integer
	SHORTINT			bX_Mms_Quotas;				//O //Short_integer


} MBOX_VIEW_REQ_PDU;


typedef	struct	_MBOX_VIEW_CONF_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	SHORTINT			bX_Mms_Response_Status;		//M //Short_integer
	ENCODED_STR			stX_Mms_Response_Text;		//O //Encoded_string_value

	LPSTR				pszX_Mms_Content_Location;	//O //Uri_value
	SHORTINT			bX_Mms_MM_State;			//O //Short_integer
	SHORT_A_ENCSTR		stX_Mms_MM_Flags;			//O //Value_length ( Add_token | Remove_token | Filter_token ) Encoded_string_value

	DWORD				dwX_Mms_Start;				//O //Integer_value
	DWORD				dwX_Mms_Limit;				//O //Integer_value
	SHORTINT			bX_Mms_Attributes;			//O //Short_integer
	SHORT_A_INT			stX_Mms_MBox_Totals;		//O //Value-length (Message-total-token | Size-total-token) Integer-Value
	SHORT_A_INT			stX_Mms_MBox_Quotas;		//O //Value_length (Message_quota_token | Size_quota_token) Integer_Value

	DWORD				dwX_Mms_Message_Count;		//O //Integer_Value

	CONTENT_TYPE		stContent_Type;				//M //Content_type_value

} MBOX_VIEW_CONF_PDU;


// ***********************************************************
// M_Mbox_Descr 类型 的PDU定义
// ***********************************************************


typedef	struct	_MBOX_DESCR_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer

	LPSTR				pszX_Mms_Content_Location;	//C //Uri_value
	LPSTR				pszMessage_ID;				//M //Text_string
	SHORTINT			bX_Mms_MM_State;			//C //Short_integer
	SHORT_A_ENCSTR		stX_Mms_MM_Flags;			//O //Value_length ( Add_token | Remove_token | Filter_token ) Encoded_string_value

	DWORD				dwDate;						//O //Long_integer: In seconds from 1970_01_01, 00:00:00 GMT.
	SHORT_A_ENCSTR		stFrom;						//O //Value_length (Address_present_token Encoded_string_value | Insert_address_token)
	ENCODED_STR			stTo;						//O //Encoded_string_value
	ENCODED_STR			stCc;						//O //Encoded_string_value
	ENCODED_STR			stBcc;						//O //Encoded_string_value
	SHORT_STR			stX_Mms_Message_Class;		//O //Class_identifier | Token_text
	ENCODED_STR			stSubject;					//O //Encoded_string_value

	SHORTINT			bX_Mms_Priority;			//O //Short_integer
	DATE_A_DELTASEC		stX_Mms_Delivery_Time;		//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	DATE_A_DELTASEC		stX_Mms_Expiry;				//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	SHORTINT			bX_Mms_Delivery_Report;		//O //Short_integer
	SHORTINT			bX_Mms_Read_Report;			//O //Short_integer

	DWORD				dwX_Mms_Message_Size;		//O //Long_integer

	SHORTINT			bX_Mms_Reply_Charging;		//O //Short_integer
	DATE_A_DELTASEC		stX_Mms_Reply_Charging_Deadline;//O //Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
	DWORD				dwX_Mms_Reply_Charging_Size;//O //Long_integer
	LPSTR				pszX_Mms_Reply_Charging_ID;	//O //Text_string

	INTEGER_A_ENCSTR	stX_Mms_Previously_Sent_By;	//O //Value_length Forwarded_count_value Encoded_string_value
	INTEGER_A_LONG		stX_Mms_Previously_Sent_Date;	//O //Value_length Forwarded_count_value Date_value

	CONTENT_TYPE		stContent_Type;				//M //Content_type_value

} MBOX_DESCR_PDU;

// ***********************************************************
// M_Mbox_Upload_req 和 M_Mbox_Upload_conf 类型 的PDU定义
// ***********************************************************


typedef	struct	_MBOX_UPLOAD_REQ_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	SHORTINT			bX_Mms_MM_State;			//O //Short_integer
	SHORT_A_ENCSTR		stX_Mms_MM_Flags;			//O //Value_length ( Add_token | Remove_token | Filter_token ) Encoded_string_value

	CONTENT_TYPE		stContent_Type;				//M //Content_type_value

} MBOX_UPLOAD_REQ_PDU;


typedef	struct	_MBOX_UPLOAD_CONF_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszX_Mms_Content_Location;	//O //Uri_value
	SHORTINT			bX_Mms_Store_Status;		//M //Short_integer
	ENCODED_STR			stX_Mms_Store_Status_Text;	//O //Encoded_string_value

} MBOX_UPLOAD_CONF_PDU;

// ***********************************************************
// M_Mbox_Delete_req 和 M_Mbox_Delete_conf 类型 的PDU定义
// ***********************************************************


typedef	struct	_MBOX_DELETE_REQ_PDU
{
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	LPSTR				pszX_Mms_Content_Location;	//M //Uri_value

} MBOX_DELETE_REQ_PDU;


typedef	struct	_MBOX_DELETE_CONF_PDU
{
	//DWORD				dwSize;
	DWORD				dwMask;

	SHORTINT			bX_Mms_Message_Type;		//M //Short_integer
	LPSTR				pszX_Mms_Transaction_ID;	//M //Text_string
	SHORTINT			bX_Mms_MMS_Version;			//M //Short_integer

	INTEGER_A_STR		stX_Mms_Content_Location;	//O //Value_length Status_count_value Content_location_value
	INTEGER_A_SHORT		stX_Mms_Response_Status;	//M //Value-length Status_count_value Response_status_value
	ENCODED_STR			stX_Mms_Response_Text;		//O //Encoded_string_value

} MBOX_DELETE_CONF_PDU;


// ***********************************************************
// for ver1.2___end__PDU_TYPE
// ***********************************************************


// ***********************************************************
// 各种类型 的PDU定义 和 操作函数
// ***********************************************************

typedef	union	_MMS_PDU
{
	MMS_GEN_HDR				HdrGen;
	
	MSEND_REQ_PDU			Send_Req;
	MSEND_CONF_PDU			Send_Conf;
	
	MNOTIFICATION_IND_PDU	Notification_Ind;
	MNOTIFYRESP_IND_PDU		Notifyresp_Ind;
	MRETRIEVE_CONF_PDU		Retrieve_Conf;
	MACKNOWLEDGE_IND_PDU	Acknowledge_Ind;
	
	MFORWARD_REQ_PDU		Forward_Req;
	MFORWARD_CONF_PDU		Forward_Conf;
	
	MDELIVERY_IND_PDU		Delivery_Ind;
	MREAD_REC_IND_PDU		Read_Rec_Ind;
	MREAD_ORIG_IND_PDU		Read_Orig_Ind;
	
	// for ver1.2___begin__PDU_union
	MBOX_STORE_REQ_PDU		MBox_Store_Req;
	MBOX_STORE_CONF_PDU		MBox_Store_Conf;
	
	MBOX_VIEW_REQ_PDU		MBox_View_Req;
	MBOX_VIEW_CONF_PDU		MBox_View_Conf;
	
	MBOX_DESCR_PDU			MBox_Descr;
	
	MBOX_UPLOAD_REQ_PDU		MBox_Upload_Req;
	MBOX_UPLOAD_CONF_PDU	MBox_Upload_Conf;
	
	MBOX_DELETE_REQ_PDU		MBox_Delete_Req;
	MBOX_DELETE_CONF_PDU	MBox_Delete_Conf;
	// for ver1.2___end__PDU_union
	
} MMS_PDU;


extern	BOOL	Mms_UnpackType( DATA_DEAL* lpDealData, SHORTINT* pbMmsMsgType );
extern	BOOL	Mms_UnpackPdu( DATA_DEAL* lpDealData, OUT MMS_PDU* pPduMms );
extern	void	Mms_FreePduUnpack( MMS_PDU* pPduMms );

extern	BOOL	Mms_PackPdu( OUT DATA_DEAL* lpDealData, BOOL fAllocData, MMS_PDU* pPduMms );



// ***********************************************************
// 各种字段 操作函数
// ***********************************************************



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_MMS_PDU_H_

