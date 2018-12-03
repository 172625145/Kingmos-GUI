/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WAP_WTP_H_
#define _WAP_WTP_H_

#include "varbuf.h"

typedef LPVOID	WTP_HANDLE;

typedef DATA_DEAL  WTPRESULT;
typedef LPDATA_DEAL  LPWTPRESULT;

// UnRealiable invkoe without result
BOOL WTPAPI_TransferWithClass0(WTP_HANDLE hWTP_handle,LPTSTR lpData,DWORD dwDataLen);
// Realiable invkoe without result
BOOL WTPAPI_TransferWithClass1(WTP_HANDLE hWTP_handle,LPTSTR lpData,DWORD dwDataLen);
// Realiable invkoe with result
BOOL WTPAPI_TransferWithClass2(WTP_HANDLE hWTP_handle,LPTSTR lpData,DWORD dwDataLen,LPWTPRESULT lpResult);

WTP_HANDLE WTPAPI_TransferInitialise(void);

void WTPAPI_TransferDeInitialise(WTP_HANDLE hWTP_handle);


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WAP_WTP_H_

