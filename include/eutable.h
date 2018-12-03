/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EUTABLE_H
#define __EUTABLE_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

enum{
    TB_CREATE = 1,
    TB_DELETE,
    TB_CLOSE,
    TB_SETRECORDPOINTER,
    TB_NEXTRECORDPOINTER,
    TB_READFIELD,
    TB_WRITEFIELD,
    TB_WRITERECORD,
    TB_DELETERECORD,
    TB_SORTRECORD,
    TB_APPENDRECORD,
    TB_READRECORD,
    TB_INSERTRECORD,
    TB_COUNTRECORD,
    TB_GETRECORDINFO,
    TB_GETTABLEINFO,
    TB_GETRECORDSIZE,
    TB_FINDFIRST,
    TB_FINDNEXT,
    TB_CLOSEFIND,
    TB_PACK,
	TB_READMULTIRECORDS,
	TB_GETUSERDATA,
	TB_SETUSERDATA,
	TB_FLUSH,
	TB_PACKBYHANDLE
};


#ifdef __cplusplus
}
#endif  // __cplusplus


#endif // __EUTABLE_H
