/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef __EUUSUAL_H
#define __EUUSUAL_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

enum{
    GBL_ALLOC = 1 ,
    GBL_DISCARD ,
	GBL_FLAGS ,
	GBL_FREE ,
	GBL_HANDLE ,
	GBL_LOCK ,
	GBL_REALLOC ,
	GBL_SIZE ,
	GBL_UNLOCK ,
//  Clipboard function
	CLIP_OPEN,	
	CLIP_CLOSE,
	CLIP_SETDATA,
	CLIP_GETDATA,
	CLIP_EMPTY,
	CLIP_ISFORMATAVAILABLE 
};


#ifdef __cplusplus
}
#endif  // __cplusplus


#endif  //__EUUSUAL_H__
