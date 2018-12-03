/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __ROMFILE_H
#define __ROMFILE_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

//The ioctl code is define for romfs by zb.
//#define		IOCTL_ROM_ALLOC_FILE		100
//#define		IOCTL_ROM_GET_EXE_INFO		101
//#define		IOCTL_ROM_GET_OBJ_INFO		102
//#define		IOCTL_ROM_GET_ELF_INFO		103
#define	IOCTL_ROM_ALLOC_FILE	CTL_CODE(FILE_DEVICE_ROM_FILE_SYSTEM, 100, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_ROM_GET_EXE_INFO	CTL_CODE(FILE_DEVICE_ROM_FILE_SYSTEM, 101, METHOD_BUFFERED, FILE_READ_ACCESS)
#define	IOCTL_ROM_GET_OBJ_INFO	CTL_CODE(FILE_DEVICE_ROM_FILE_SYSTEM, 102, METHOD_BUFFERED, FILE_READ_ACCESS)
#define	IOCTL_ROM_GET_ELF_INFO	CTL_CODE(FILE_DEVICE_ROM_FILE_SYSTEM, 103, METHOD_BUFFERED, FILE_READ_ACCESS)
#define	IOCTL_ROM_GET_SEG_RES	CTL_CODE(FILE_DEVICE_ROM_FILE_SYSTEM, 104, METHOD_BUFFERED, FILE_READ_ACCESS)


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__ROMFILE_H
