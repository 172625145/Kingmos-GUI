/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EUDEVICE_H
#define __EUDEVICE_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

enum{
    DEV_REGISTER = 1,
    DEV_DEREGISTER,
	//DEV_GETNAME,
	DEV_REGISTERFSD,
	DEV_UNREGISTERFSD,
	DEV_LOADFSD,
	DEV_ENUM,
    DEV_UNLOADFSD,
	DEV_GETSYSTEMPOWERSTATUSEX
};

enum{
    DEV_CREATEFILE = 1,
    DEV_CLOSEFILE,
    DEV_READFILE,
    DEV_WRITEFILE,
	DEV_GETFILESIZE,
	DEV_SETFILEPOINTER,
	DEV_SETENDOFFILE,
	DEV_GETFILETIME,
    DEV_SETFILETIME,
    DEV_FLUSHFILEBUFFERS,
    DEV_GETFILEINFORMATIONBYHANDLE,
    DEV_IOCONTROL
};

#ifdef __cplusplus
}
#endif  // __cplusplus


#endif // __EUDEVICE_H
