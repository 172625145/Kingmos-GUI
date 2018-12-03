/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EERROR_H
#define __EERROR_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#define ERROR_SUCCESS 0

#define ERROR_INVALID_FUNCTION      1

#define ERROR_FILE_NOT_FOUND        2

#define ERROR_PATH_NOT_FOUND		3

#define ERROR_ACCESS_DENIED         5

#define ERROR_INVALID_HANDLE        6
#define ERROR_ARENA_TRASHED         7
#define ERROR_NOT_ENOUGH_MEMORY     8

#define ERROR_INVALID_BLOCK         9

#define ERROR_BAD_ENVIRONMENT       10

#define ERROR_BAD_FORMAT            11

#define ERROR_INVALID_ACCESS        12

#define ERROR_INVALID_DATA          13

#define ERROR_OUTOFMEMORY           14

#define ERROR_NO_MORE_FILES         18

#define ERROR_BAD_UNIT				20

#define ERROR_SECTOR_NOT_FOUND      27

#define ERROR_WRITE_FAULT           29
#define ERROR_READ_FAULT            30
#define ERROR_GEN_FAILURE           31

#define ERROR_SHARING_VIOLATION     32

#define ERROR_INVALID_PARAMETER     87

#define ERROR_FILE_EXISTS           80

#define ERROR_OPEN_FAILED			110

#define ERROR_DISK_FULL             112

#define ERROR_INSUFFICIENT_BUFFER   122

#define ERROR_INVALID_NAME          123

#define	ERROR_DIR_NOT_EMPTY			145

#define ERROR_ALREADY_EXISTS        183

#define ERROR_MORE_DATA             234

#define ERROR_NO_MORE_ITEMS         259

#define ERROR_TRY_AGAIN             1000

#define ERROR_PARTITION_FAILURE          1105
#define ERROR_DEVICE_NOT_PARTITIONED     1107

#define ERROR_NOT_FOUND                  1168

#define ERROR_NO_MORE_DEVICES       1248


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif   // __EERROR_H
