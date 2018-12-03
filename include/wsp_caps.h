/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WSP_CAPS_H_
#define _WSP_CAPS_H_

#ifndef _WSP_VAR_TYPE_H_
#include "wsp_vartype.h"
#endif

#ifndef _WSP_CHAR_SET_H_
#include "wsp_charset.h"
#endif

#ifndef _WSP_LANGUAGE_H_
#include "wsp_lang.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//Capability MASK
#define	SCAP_GEN_MASK_Client_SDU_Size		0x80000000
#define	SCAP_GEN_MASK_Server_SDU_Size		0x40000000
#define	SCAP_GEN_MASK_Protocol_Options		0x20000000
#define	SCAP_GEN_MASK_Method_MOR			0x10000000
#define	SCAP_GEN_MASK_Push_MOR				0x08000000
#define	SCAP_GEN_MASK_Extended_Methods		0x04000000
#define	SCAP_GEN_MASK_Header_Code_Pages		0x02000000
#define	SCAP_GEN_MASK_Aliases				0x01000000
#define	SCAP_GEN_MASK_Client_Message_Size	0x00800000
#define	SCAP_GEN_MASK_Server_Message_Size	0x00400000


//Default value
#define	SCAP_DEFAULT_Client_SDU_Size		1400
#define	SCAP_DEFAULT_Server_SDU_Size		1400
#define	SCAP_DEFAULT_Protocol_Options		0
#define	SCAP_DEFAULT_Method_MOR				1
#define	SCAP_DEFAULT_Push_MOR				1
#define	SCAP_DEFAULT_Extended_Methods		NULL
#define	SCAP_DEFAULT_Header_Code_Pages		NULL
#define	SCAP_DEFAULT_Aliases				NULL
#define	SCAP_DEFAULT_Client_Message_Size	1400
#define	SCAP_DEFAULT_Server_Message_Size	1400

//Capability
//dwLength-dwCapID-Parameters
typedef	struct	_SCAPABILITIES
{
	DWORD				dwMask;
	
	DWORD				dwClient_SDU_Size	; //uintvar
	DWORD				dwServer_SDU_Size	; //uintvar
	
	BYTE				bProtocol_Options	; //multiple octets
	
	BYTE				bMethod_MOR			; //uint8
	BYTE				bPush_MOR			; //uint8
	LPSTR				pszExtended_Methods	; //multiple octets
	LPSTR				pszHeader_Code_Pages; //multiple octets
	LPSTR				pszAliases			; //multiple octets
	
	DWORD				dwClient_Message_Size; //uintvar
	DWORD				dwServer_Message_Size; //uintvar

} SCAPABILITIES;


extern	BOOL	VarDec_SCapabilities( DATA_DEAL* lpDealData, DWORD dwLenCap, OUT SCAPABILITIES* lpSCapabilities );
extern	BOOL	VarEnc_SCapabilities( DATA_DEAL* lpDealData, OUT DWORD* pdwLenCap, SCAPABILITIES* lpSCapabilities );
extern	BOOL	VarClone_SCapabilities( OUT SCAPABILITIES* lpSCapabilitiesDes, SCAPABILITIES* lpSCapabilitiesSrc );
extern	void	VarFree_SCapabilities( SCAPABILITIES* lpSCapabilities );



	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WSP_CAPS_H_

