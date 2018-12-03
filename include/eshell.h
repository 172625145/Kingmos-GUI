/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __ESHELL_H
#define __ESHELL_H

#ifndef __EDEF_H
#include <edef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus


#define SHCNE_RENAMEITEM          0x00000001L
#define SHCNE_CREATE              0x00000002L
#define SHCNE_DELETE              0x00000004L
#define SHCNE_MKDIR               0x00000008L
#define SHCNE_RMDIR               0x00000010L
#define SHCNE_MEDIAINSERTED       0x00000020L
#define SHCNE_MEDIAREMOVED        0x00000040L
#define SHCNE_DRIVEREMOVED        0x00000080L
#define SHCNE_DRIVEADD            0x00000100L
#define SHCNE_NETSHARE            0x00000200L
#define SHCNE_NETUNSHARE          0x00000400L
#define SHCNE_ATTRIBUTES          0x00000800L
#define SHCNE_UPDATEDIR           0x00001000L
#define SHCNE_UPDATEITEM          0x00002000L
#define SHCNE_SERVERDISCONNECT    0x00004000L
#define SHCNE_UPDATEIMAGE         0x00008000L
#define SHCNE_DRIVEADDGUI         0x00010000L
#define SHCNE_RENAMEFOLDER        0x00020000L
#define SHCNE_FREESPACE           0x00040000L

#define SHCNE_EXTENDED_EVENT      0x04000000L

#define SHCNE_ASSOCCHANGED        0x08000000L

#define SHCNE_DISKEVENTS          0x0002381FL
#define SHCNE_GLOBALEVENTS        0x0C0581E0L 
#define SHCNE_ALLEVENTS           0x7FFFFFFFL
#define SHCNE_INTERRUPT           0x80000000L 

#define SHCNF_PATH              0x00000001l
#define SHCNF_FLUSHNOWAIT       0x00000002l


#define SHChangeNotify SH_ChangeNotify
VOID WINAPI SH_ChangeNotify(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2);

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif   // __ESHELL_H


