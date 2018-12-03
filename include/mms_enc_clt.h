/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _MMS_ENC_CLT_H_
#define _MMS_ENC_CLT_H_


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ***********************************************************
// X_Mms_Message_Type value 定义
// ***********************************************************

#define	MType_send_req						(0x80 -OCTET_SHORTINT)
#define	MType_send_conf						(0x81 -OCTET_SHORTINT)
#define	MType_notification_ind				(0x82 -OCTET_SHORTINT)
#define	MType_notifyresp_ind				(0x83 -OCTET_SHORTINT)
#define	MType_retrieve_conf					(0x84 -OCTET_SHORTINT)
#define	MType_acknowledge_ind				(0x85 -OCTET_SHORTINT)
#define	MType_delivery_ind					(0x86 -OCTET_SHORTINT)
#define	MType_read_rec_ind					(0x87 -OCTET_SHORTINT)
#define	MType_read_orig_ind					(0x88 -OCTET_SHORTINT)
#define	MType_forward_req					(0x89 -OCTET_SHORTINT)
#define	MType_forward_conf					(0x8A -OCTET_SHORTINT)
//for Ver1.2
#define	MType_mbox_store_req				(0x8B -OCTET_SHORTINT)
#define	MType_mbox_store_conf				(0x8C -OCTET_SHORTINT)
#define	MType_mbox_view_req					(0x8D -OCTET_SHORTINT)
#define	MType_mbox_view_conf				(0x8E -OCTET_SHORTINT)
#define	MType_mbox_upload_req				(0x8F -OCTET_SHORTINT)
#define	MType_mbox_upload_conf				(0x90 -OCTET_SHORTINT)
#define	MType_mbox_delete_req				(0x91 -OCTET_SHORTINT)
#define	MType_mbox_delete_conf				(0x92 -OCTET_SHORTINT)
#define	MType_mbox_descr					(0x93 -OCTET_SHORTINT)


// ********************************
// X_Mms_MMS_Version 定义
// ********************************
#define	MMS_VER_MAKE(a,b)					( ((a<<4) & 0x70) | (b & 0x0F) )

/*
// ********************************
// From value 定义
// ********************************
//bToken
#define	MFrom_Address_present				(0x80 -OCTET_SHORTINT)
#define	MFrom_Insert_address				(0x81 -OCTET_SHORTINT)


// ********************************
// X_Mms_Message_Class value 定义
// ********************************
#define	MClass_Personal						(0x80 -OCTET_SHORTINT)
#define	MClass_Advertisement				(0x81 -OCTET_SHORTINT)
#define	MClass_Informational				(0x82 -OCTET_SHORTINT)
#define	MClass_Auto							(0x83 -OCTET_SHORTINT)


// ********************************
// X_Mms_Priority value 定义
// ********************************
#define	MPriority_Low						(0x80 -OCTET_SHORTINT)
#define	MPriority_Normal					(0x81 -OCTET_SHORTINT)
#define	MPriority_High						(0x82 -OCTET_SHORTINT)


// ********************************
// X_Mms_Sender_Visibility value 定义
// ********************************
#define	MSender_Visibility_Hide				(0x80 -OCTET_SHORTINT)
#define	MSender_Visibility_Show				(0x81 -OCTET_SHORTINT)


// ********************************
// X_Mms_Delivery_Report value 定义
// ********************************
#define	MDelivery_report_Yes				(0x80 -OCTET_SHORTINT)
#define	MDelivery_report_No					(0x81 -OCTET_SHORTINT)


// ********************************
// X_Mms_Read_Report value 定义
// ********************************
#define	MRead_report_Yes					(0x80 -OCTET_SHORTINT)
#define	MRead_report_No						(0x81 -OCTET_SHORTINT)

*/
// ********************************
// X_Mms_Response_Status value 定义
// ********************************
#define	MResponse_Status_Ok								(0x80 -OCTET_SHORTINT)
#define	MResponse_Status_unspecified					(0x81 -OCTET_SHORTINT)//129 (obsolete)
#define	MResponse_Status_service_denied					(0x82 -OCTET_SHORTINT)//130 (obsolete)
#define	MResponse_Status_message_format_corrupt			(0x83 -OCTET_SHORTINT)//131 (obsolete)
#define	MResponse_Status_sending_address_unresolved		(0x84 -OCTET_SHORTINT)//132 (obsolete)
#define	MResponse_Status_message_not_found				(0x85 -OCTET_SHORTINT)//133 (obsolete)
#define	MResponse_Status_network_problem				(0x86 -OCTET_SHORTINT)//134 (obsolete)
#define	MResponse_Status_content_not_accepted			(0x87 -OCTET_SHORTINT)//135 (obsolete)
#define	MResponse_Status_unsupported_message			(0x88 -OCTET_SHORTINT)//136

#define	MResponse_Status_transient_failure						(0xC0 -OCTET_SHORTINT)//192
#define	MResponse_Status_transient_sending_address_unresolved	(0xC1 -OCTET_SHORTINT)//193
#define	MResponse_Status_transient_message_not_found			(0xC2 -OCTET_SHORTINT)//194
#define	MResponse_Status_transient_network_problem				(0xC3 -OCTET_SHORTINT)//195
#define	MResponse_Status_transient_partial_success				(0xC4 -OCTET_SHORTINT)//196

#define	MResponse_Status_permanent_failure								(0xE0 -OCTET_SHORTINT)//224
#define	MResponse_Status_permanent_service_denied						(0xE1 -OCTET_SHORTINT)//225
#define	MResponse_Status_permanent_message_format_corrupt				(0xE2 -OCTET_SHORTINT)//226
#define	MResponse_Status_permanent_sending_address_unresolved			(0xE3 -OCTET_SHORTINT)//227
#define	MResponse_Status_permanent_message_not_found					(0xE4 -OCTET_SHORTINT)//228
#define	MResponse_Status_permanent_content_not_accepted					(0xE5 -OCTET_SHORTINT)//229
#define	MResponse_Status_permanent_reply_charging_limitations_not_met	(0xE6 -OCTET_SHORTINT)//230
#define	MResponse_Status_permanent_reply_charging_request_not_accepted	(0xE7 -OCTET_SHORTINT)//231
#define	MResponse_Status_permanent_reply_charging_forwarding_denied		(0xE8 -OCTET_SHORTINT)//232
#define	MResponse_Status_permanent_reply_charging_not_supported			(0xE9 -OCTET_SHORTINT)//233
#define	MResponse_Status_permanent_address_hiding_not_supported			(0xEA -OCTET_SHORTINT)//234


// ***********************************************************
// 各种字段 操作函数
// ***********************************************************
extern	BOOL	VarShow_Version( SHORTINT bMMSVerion, OUT LPSTR pszMMSVersion );
extern	BOOL	VarMake_Version( OUT SHORTINT* pbMMSVerion, LPSTR pszMMSVersion );

extern	BOOL	VarShow_Date( DWORD dwDate, OUT SYSTEMTIME* lpSysDate );
extern	BOOL	VarMake_Date( OUT DWORD* pdwDate, SYSTEMTIME* lpSysDate );
extern	BOOL	VarMake_CurDate( OUT DWORD* pdwDate );

extern	BOOL	VarMake_CurTID( OUT LPSTR pszMTID, DWORD dwLen );


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_MMS_ENC_CLT_H_

