/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EDEVICE_H
#define __EDEVICE_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */


// 定义 IOCTL and FSCTL 功能编码
// 注意 function 0-2047 由Kingmos系统使用
//      function 2048-4095 由客户使用
// bit格式： DDDD DDDD DDDD DDDD AAFF FFFF FFFF FFMM
#define CTL_CODE( deviceType, function, method, access ) \
	( ((deviceType) << 16) | ((access) << 14) | ((function) << 2) | (method) )

// 定义 method 编码: 当进行I/O和FS控制时， buffer是怎样传递的

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

// 定义 access 值

#define FILE_ANY_ACCESS                 0x0
#define FILE_READ_ACCESS                0x1
#define FILE_WRITE_ACCESS               0x2

// 定义设备类型
// 注意 0-32767 由Kingmos系统使用
// 32768-65535  由客户使用
//

#define FILE_DEVICE_NULL                  0x00000000
#define FILE_DEVICE_UNKNOWN               0x00000001
#define FILE_DEVICE_DISK                  0x00000002
#define FILE_DEVICE_DISK_FILE_SYSTEM      0x00000003
#define FILE_DEVICE_FILE_SYSTEM           0x00000004
#define FILE_DEVICE_KEYBOARD              0x00000005
#define FILE_DEVICE_POS 
#define FILE_DEVICE_MOUSE FILE_DEVICE_POS 0x00000006
#define FILE_DEVICE_TOUCH
#define FILE_DEVICE_DATALINK
#define FILE_DEVICE_NETWORK               0x00000007
#define FILE_DEVICE_NETWORK_BROWSER       0x00000008
#define FILE_DEVICE_NETWORK_FILE_SYSTEM   0x00000009
#define FILE_DEVICE_SERIAL_PORT           0x0000000A
#define FILE_DEVICE_SOUND                 0x0000000B
#define FILE_DEVICE_STORE                 0x0000000C
#define FILE_DEVICE_SCREEN                0x0000000D
#define FILE_DEVICE_PARALLEL_PORT         0x0000000E
#define FILE_DEVICE_CD_ROM                0x0000000F
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM    0x00000010
#define FILE_DEVICE_CONTROLLER            0x00000011
#define FILE_DEVICE_ROM_FILE_SYSTEM       0x00000012



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__EDEVICE_H
